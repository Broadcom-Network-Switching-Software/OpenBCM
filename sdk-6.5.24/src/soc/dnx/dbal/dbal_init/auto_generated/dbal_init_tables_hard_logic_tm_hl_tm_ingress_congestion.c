
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_free_shared_threshold_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE, is_valid, "INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL_ID, DBAL_FIELD_TYPE_DEF_POOL_ID, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_WORDS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_WORDS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_vsq_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_vsq_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_WORDS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SHRD_FR_RJCT_THm, WORDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_WORDS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SHRD_FR_RJCT_THm, WORDS_CLR_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SHRD_FR_RJCT_THm, SRAM_BUFFERS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SHRD_FR_RJCT_THm, SRAM_BUFFERS_CLR_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SHRD_FR_RJCT_THm, SRAM_PDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SHRD_FR_RJCT_THm, SRAM_PDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_sram_per_dp_thresholds_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE, is_valid, "INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_PDBS_DP_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_PDBS_DP_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_PDBS_DP_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_PDBS_DP_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_BUFFERS_DP_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_BUFFERS_DP_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_BUFFERS_DP_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_DP_RJCT_THm, SRAM_BUFFERS_DP_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_sram_per_tc_thresholds_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE, is_valid, "INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC, DBAL_FIELD_TYPE_DEF_TC, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_PDBS_TC_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = example_arr_prefix_and_cb_destination_encoding_blockindex_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_PDBS_TC_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_tc_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_PDBS_TC_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = example_arr_prefix_and_cb_destination_encoding_blockindex_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_PDBS_TC_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_tc_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_BUFFERS_TC_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = example_arr_prefix_and_cb_destination_encoding_blockindex_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_BUFFERS_TC_RJCT_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_tc_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_BUFFERS_TC_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = example_arr_prefix_and_cb_destination_encoding_blockindex_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_SRAM_TC_RJCT_THm, SRAM_BUFFERS_TC_RJCT_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_tc_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_dram_per_dp_thresholds_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE, is_valid, "INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BDBS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BDBS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_DRAM_BDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_DRAM_DP_RJCT_THm, DRAM_BDBS_DP_RJCT_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_DRAM_DP_RJCT_THm, DRAM_BDBS_DP_RJCT_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_dram_per_tc_thresholds_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE, is_valid, "INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC, DBAL_FIELD_TYPE_DEF_TC, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BDBS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BDBS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_DRAM_BDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_DRAM_TC_RJCT_THm, DRAM_BDBS_TC_RJCT_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_FR_DRAM_TC_RJCT_THm, DRAM_BDBS_TC_RJCT_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_command_fifo_sram_reject_threshold_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD, is_valid, "INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FWD_ACTION, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            db_field.max_value = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REP_CMD_FIFO_SRAM_RSRC_RJCT_THm, FR_SRAM_PDBS_RJCT_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REP_CMD_FIFO_SRAM_RSRC_RJCT_THm, FR_SRAM_PDBS_RJCT_CLR_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REP_CMD_FIFO_SRAM_RSRC_RJCT_THm, FR_SRAM_BUFFERS_RJCT_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REP_CMD_FIFO_SRAM_RSRC_RJCT_THm, FR_SRAM_BUFFERS_RJCT_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_guaranteed_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_WORDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_GRNTD_PRMSm, GRNTD_WORDS_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_guaranteed_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_GRNTD_PRMSm, GRNTD_SRAM_BUFFERS_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_guaranteed_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_PDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_GRNTD_PRMSm, GRNTD_SRAM_PDS_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_fadt_drop_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_WORDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_FADT_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_FADT_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_FADT_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 4;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_FADT_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_fadt_drop_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_BUFFERS_RJCT_PRMSm, VOQ_FADT_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_BUFFERS_RJCT_PRMSm, VOQ_FADT_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_BUFFERS_RJCT_PRMSm, VOQ_FADT_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_BUFFERS_RJCT_PRMSm, VOQ_FADT_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_fadt_drop_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_PDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_PDS_RJCT_PRMSm, VOQ_FADT_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_PDS_RJCT_PRMSm, VOQ_FADT_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_PDS_RJCT_PRMSm, VOQ_FADT_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_PDS_RJCT_PRMSm, VOQ_FADT_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_RJCT_PRMSm, VOQ_WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_wred_ecn_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DROP_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DROP_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CNI_PRMSm, MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_MAPPING, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_A_ID, DBAL_FIELD_TYPE_DEF_VSQ_A_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_PRMSm, RATE_CLASSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_MAPPING, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_B_ID, DBAL_FIELD_TYPE_DEF_VSQ_B_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_PRMSm, RATE_CLASSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_MAPPING, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_C_ID, DBAL_FIELD_TYPE_DEF_VSQ_C_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_PRMSm, RATE_CLASSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_MAPPING, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_D_ID, DBAL_FIELD_TYPE_DEF_VSQ_D_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_PRMSm, RATE_CLASSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_MAPPING, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_E_ID, DBAL_FIELD_TYPE_DEF_VSQ_E_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_MAPPING, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
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
                    access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_PRMSm, RATE_CLASSf));
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
                    access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_PRMSm, RATE_CLASSf));
                        access_params->entry_offset.formula_cb = ingress_cong_vsq_e_rate_class_mapping_rate_class_entryoffset_0_cb;
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_PRMSm, RATE_CLASSf));
                        access_params->entry_offset.formula_cb = ingress_cong_vsq_e_rate_class_mapping_rate_class_entryoffset_1_cb;
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_MAPPING, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_F_ID, DBAL_FIELD_TYPE_DEF_VSQ_F_ID, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_PRMSm, RATE_CLASSf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WRED_RJCT_PRMSm, WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WRED_RJCT_PRMSm, WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WRED_RJCT_PRMSm, WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WRED_RJCT_PRMSm, WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 4);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL_ID, DBAL_FIELD_TYPE_DEF_POOL_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WRED_RJCT_PRMSm, WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_wred_drop_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_DROP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_DROP, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_DROP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported))>0)?1:0) + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C1, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C11, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C12, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C13, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_c1_high_precision_supported);
            db_field.size = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C2, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 32;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_C3, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGNORE_PACKET_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONTINUOUS_PROBABILITY, DBAL_FIELD_TYPE_DEF_BOOL_INV, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_wred_continuous_probabilty_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_DROP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_C_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_C_2f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_C3;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_C_3f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGNORE_PACKET_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WRED_RJCT_PRMSm, WRED_IGNR_PKT_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_DROP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_AVRG_PRMSm, AVRG_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WORDS_AVRG_PRMSm, AVRG_UPDT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WORDS_AVRG_PRMSm, AVRG_UPDT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WORDS_AVRG_PRMSm, AVRG_UPDT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WORDS_AVRG_PRMSm, AVRG_UPDT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WORDS_AVRG_PRMSm, AVRG_UPDT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_wred_avrg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_AVRG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_AVRG_PARAMS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_AVRG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_AVRG_WEIGHT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_AVRG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WORDS_AVRG_PRMSm, AVRG_UPDT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_AVRG_WEIGHT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_WORDS_AVRG_PRMSm, AVRG_WEIGHTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_WRED_AVRG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_tail_drop_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_WORDS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_RJCT_PRMSm, WORDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_tail_drop_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_RJCT_PRMSm, SRAM_BUFFERS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_tail_drop_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_RJCT_PRMSm, SRAM_PDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_TAIL_DROP_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_tail_drop_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_WORDS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_RJCT_PRMSm, WORDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_tail_drop_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_RJCT_PRMSm, SRAM_BUFFERS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_tail_drop_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_RJCT_PRMSm, SRAM_PDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_TAIL_DROP_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_tail_drop_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_WORDS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_RJCT_PRMSm, WORDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_tail_drop_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_RJCT_PRMSm, SRAM_BUFFERS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_tail_drop_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_RJCT_PRMSm, SRAM_PDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_TAIL_DROP_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_tail_drop_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_WORDS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_RJCT_PRMSm, WORDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_tail_drop_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_RJCT_PRMSm, SRAM_BUFFERS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_tail_drop_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_RJCT_PRMSm, SRAM_PDS_MAX_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_TAIL_DROP_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_resource_alloc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_WORDS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 4);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, WORDS_MAX_GRNTD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, WORDS_MAX_SHRD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, WORDS_MAX_HDRM_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_resource_alloc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 4);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, SRAM_BUFFERS_MAX_GRNTD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, SRAM_BUFFERS_MAX_SHRD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, SRAM_BUFFERS_MAX_HDRM_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_resource_alloc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 4);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, SRAM_PDS_MAX_GRNTD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, SRAM_PDS_MAX_SHRD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_RJCT_PRMSm, SRAM_PDS_MAX_HDRM_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_resource_alloc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_MIN_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_NOMINAL_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, WORDS_MAX_GRNTD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, WORDS_SHRD_SIZE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_MIN_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, WORDS_SHRD_SIZE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, WORDS_SHRD_SIZE_ADJUST_FACTORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, WORDS_SHRD_SIZE_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 4;
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
                access_params->access_field_id = DBAL_FIELD_HEADROOM_NOMINAL_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, WORDS_MAX_HDRM_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_resource_alloc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_MIN_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_NOMINAL_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_TOTAL_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_EXTENSION_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffers_vsq_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_MAX_GRNTD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_SHRD_SIZE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_MIN_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_SHRD_SIZE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_SHRD_SIZE_ADJUST_FACTORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_SHRD_SIZE_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_HEADROOM_NOMINAL_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_MAX_HDRM_NOMINAL_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_TOTAL_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_MAX_HDRM_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_EXTENSION_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_BUFFERS_MAX_HDRM_EXT_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_resource_alloc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GUARANTEED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_MAX_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_MIN_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_FADT_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_NOMINAL_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_TOTAL_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_HEADROOM_EXTENSION_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GUARANTEED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_MAX_GRNTD_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_MAX_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_SHRD_SIZE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_MIN_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_SHRD_SIZE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_SHRD_SIZE_ADJUST_FACTORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_FADT_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_SHRD_SIZE_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_HEADROOM_NOMINAL_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_MAX_HDRM_NOMINAL_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_TOTAL_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_MAX_HDRM_SIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_HEADROOM_EXTENSION_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_RJCT_PRMSm, SRAM_PDS_MAX_HDRM_EXT_SIZE_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_vsq_resource_alloc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS, is_valid, "INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOMINAL_HEADROOM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_WORDS_OC_THr, PB_VSQ_WORDS_SHRD_OC_TH_POOL_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_WORDS_OC_THr, PB_VSQ_WORDS_SHRD_OC_TH_POOL_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NOMINAL_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_WORDS_OC_THr, PB_VSQ_WORDS_HDRM_OC_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_vsq_resource_alloc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 2 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOMINAL_HEADROOM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_HEADROOM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, PB_VSQ_SRAM_BUFFERS_SHRD_OC_TH_POOL_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, PB_VSQ_SRAM_BUFFERS_SHRD_OC_TH_POOL_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NOMINAL_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_0f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_1_POOL_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_1_POOL_1f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_vsq_resource_alloc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_PDS, is_valid, "INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + 2 + 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHARED_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOMINAL_HEADROOM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_HEADROOM, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            db_field.nof_instances = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_PDS_OC_THr, PB_VSQ_SRAM_PDS_SHRD_OC_TH_POOL_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHARED_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_PDS_OC_THr, PB_VSQ_SRAM_PDS_SHRD_OC_TH_POOL_1f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NOMINAL_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_PDS_OC_THr, PB_VSQ_SRAM_PDS_HDRM_OC_TH_0f));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_PDS_OC_THr, PB_VSQ_SRAM_PDS_HDRM_OC_TH_1_POOL_0f));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_HEADROOM;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                access_params->inst_idx = 1;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_PDS_OC_THr, PB_VSQ_SRAM_PDS_HDRM_OC_TH_1_POOL_1f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_fc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_WORDS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_FC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_FC_PRMSm, WORDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_FC_PRMSm, WORDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_fc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_FC_PRMSm, SRAM_BUFFERS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_FC_PRMSm, SRAM_BUFFERS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_fc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_FC_PRMSm, SRAM_PDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQA_FC_PRMSm, SRAM_PDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_A_RATE_CLASS_FC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_fc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_WORDS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_FC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_FC_PRMSm, WORDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_FC_PRMSm, WORDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_fc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_FC_PRMSm, SRAM_BUFFERS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_FC_PRMSm, SRAM_BUFFERS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_fc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_FC_PRMSm, SRAM_PDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQB_FC_PRMSm, SRAM_PDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_B_RATE_CLASS_FC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_fc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_WORDS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_FC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_FC_PRMSm, WORDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_FC_PRMSm, WORDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_fc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_FC_PRMSm, SRAM_BUFFERS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_FC_PRMSm, SRAM_BUFFERS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_fc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_FC_PRMSm, SRAM_PDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQC_FC_PRMSm, SRAM_PDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_C_RATE_CLASS_FC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_fc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_WORDS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_FC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_FC_PRMSm, WORDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_FC_PRMSm, WORDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_fc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_FC_PRMSm, SRAM_BUFFERS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_FC_PRMSm, SRAM_BUFFERS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_fc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_FC_PRMSm, SRAM_PDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQD_FC_PRMSm, SRAM_PDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_D_RATE_CLASS_FC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_fc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_WORDS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_FC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_FC_PRMSm, WORDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_FC_PRMSm, WORDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_fc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_FC_PRMSm, SRAM_BUFFERS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_FC_PRMSm, SRAM_BUFFERS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_fc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_FC_PRMSm, SRAM_PDS_SET_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQE_FC_PRMSm, SRAM_PDS_CLR_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_FC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_fc_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_WORDS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_FC_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, WORDS_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, WORDS_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OFFSET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, WORDS_OFFSETf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, WORDS_ADJUST_FACTORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, WORDS_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 4;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_fc_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_per_pool_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_BUFFERS_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_BUFFERS_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OFFSET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_BUFFERS_OFFSETf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_BUFFERS_ADJUST_FACTORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_BUFFERS_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_fc_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_PDS, is_valid, "INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OFFSET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_PDS_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_PDS_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OFFSET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_PDS_OFFSETf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_PDS_ADJUST_FACTORf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQF_FC_PRMSm, SRAM_PDS_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_FC_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_port_to_src_vsq_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, is_valid, "INGRESS_CONG_PORT_TO_SRC_VSQ", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_CGM_PORT, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_port.general.nof_port_bits_in_pp_bus_get(unit);
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_E, DBAL_FIELD_TYPE_DEF_VSQ_E_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PG_BASE, DBAL_FIELD_TYPE_DEF_VSQ_F_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC_PG_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_VSQ_E;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_IPP_MAPm, NIF_PORTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PG_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_IPP_MAPm, PG_BASEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TC_PG_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_IPP_MAPm, TC_BITMAP_INDEXf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_tc_pg_mapping_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_TC_PG_MAPPING;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_TC_PG_MAPPING, is_valid, "INGRESS_CONG_TC_PG_MAPPING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC, DBAL_FIELD_TYPE_DEF_TC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC_PG_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PG, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_TC_PG_MAPPING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_PG;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_TC_BITMAPm, INVALIDf));
                    access_params->entry_offset.formula_cb = example_arr_prefix_and_cb_destination_encoding_blockindex_0_cb;
                    access_params->data_offset.formula_cb = ingress_cong_tc_pg_mapping_pg_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_TC_PG_MAPPING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pg_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_PG_PARAMS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, is_valid, "INGRESS_CONG_PG_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PG_ID, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit));
            db_field.max_value = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit)-1;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_LOSSLESS, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_USE_PORT_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL_ID, DBAL_FIELD_TYPE_DEF_POOL_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADMIT_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_IS_LOSSLESS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PRMSm, IS_LOSSLESSf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_USE_PORT_GUARANTEED;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PRMSm, USE_PORT_GRNTDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_POOL_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PRMSm, POOL_IDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ADMIT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PRMSm, ADMT_PROFILEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_cgm_dram_bound_state_th_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH, is_valid, "INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_VOQ_DRAM_QSIZE_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.total_bytes_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_WORDS_QSIZE_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bounds_sram_words_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_BUFFERS_QSIZE_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_PDS_QSIZE_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_BUFFERS_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_PDBS_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_VOQ_DRAM_QSIZE_TH;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_DRAM_BOUND_STATE_THr, DRAM_BOUND_VOQ_DRAM_QSIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_WORDS_QSIZE_TH;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_DRAM_BOUND_STATE_THr, DRAM_BOUND_VOQ_SRAM_WORDS_QSIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_BUFFERS_QSIZE_TH;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_DRAM_BOUND_STATE_THr, DRAM_BOUND_VOQ_SRAM_BUFFERS_QSIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_PDS_QSIZE_TH;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_DRAM_BOUND_STATE_THr, DRAM_BOUND_VOQ_SRAM_PDS_QSIZE_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_BUFFERS_TH;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_DRAM_BOUND_STATE_THr, DRAM_BOUND_TOTAL_FREE_SRAM_BUFFERS_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_PDBS_TH;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_DRAM_BOUND_STATE_THr, DRAM_BOUND_TOTAL_FREE_SRAM_PDBS_THf));
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
                    access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_AUTO_DOC_NAME_22r, AUTO_DOC_NAME_25f));
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
                    access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_AUTO_DOC_NAME_21r, AUTO_DOC_NAME_24f));
                    }
                }
            }
        }
        if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0228r, FIELD_24_24f));
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
                    access_params->access_field_id = DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0227r, FIELD_24_24f));
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_profile_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO, is_valid, "INGRESS_CONG_VOQ_PROFILE_INFO", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ, DBAL_FIELD_TYPE_DEF_VOQ, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 6;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TRAFFIC_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONNECTION_CLASS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO, is_standard_1, is_compatible_with_all_images));
    }
    table_entry->core_mode = DBAL_CORE_MODE_DPC;
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_PROFILESm, INVALIDf));
                        access_params->entry_offset.formula_cb = ingress_compensation_voq_to_credit_class_profile_map_compensation_profile_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ingress_cong_voq_profile_info_rate_class_dataoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RATE_CLASS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_PROFILESm, INVALIDf));
                        access_params->entry_offset.formula_cb = ingress_compensation_voq_to_credit_class_profile_map_compensation_profile_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = ingress_cong_voq_profile_info_rate_class_dataoffset_1_cb;
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
                access_params->access_field_id = DBAL_FIELD_TRAFFIC_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_VSQS_PRMSm, INVALIDf));
                    access_params->entry_offset.formula_cb = ingress_compensation_voq_to_credit_class_profile_map_compensation_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = ingress_cong_voq_profile_info_traffic_class_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CONNECTION_CLASS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_VSQS_PRMSm, INVALIDf));
                    access_params->entry_offset.formula_cb = ingress_compensation_voq_to_credit_class_profile_map_compensation_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = ingress_cong_voq_profile_info_connection_class_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_shared_reject_status_thr_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_WORDS, is_valid, "INGRESS_CONG_SHARED_REJECT_STATUS_THR_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, WORDS_SET_THf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, WORDS_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, WORDS_CLR_THf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, WORDS_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_shared_reject_status_thr_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_BUFFERS, is_valid, "INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_BUFFERS_SET_THf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_BUFFERS_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_BUFFERS_CLR_THf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_BUFFERS_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_shared_reject_status_thr_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_PDS, is_valid, "INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_PDS_SET_THf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_PDS_SET_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_PDS_CLR_THf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SHRD_FR_RJCT_THm, SRAM_PDS_CLR_THf));
                    access_params->entry_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_1_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_SHARED_REJECT_STATUS_THR_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_bound_sram_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_WORDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bounds_sram_words_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bounds_sram_words_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_WORDS, is_standard_1, is_compatible_with_all_images));
    }
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
                    access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMS_EXTm, SRAM_WORDS_BOUND_MAX_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMS_EXTm, SRAM_WORDS_BOUND_MIN_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMS_EXTm, SRAM_WORDS_BOUND_ADJUST_FACTORf));
                        {
                            access_params->data_offset.formula_int = 3;
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
                    access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMS_EXTm, SRAM_WORDS_BOUND_ADJUST_FACTORf));
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
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_WORDS_BOUND_MAX_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_WORDS_BOUND_MIN_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_WORDS_BOUND_ADJUST_FACTORf));
                        {
                            access_params->data_offset.formula_int = 3;
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
                    access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_WORDS_BOUND_ADJUST_FACTORf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_bound_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_BUFFERS_BOUND_FREE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_BUFFERS_BOUND_FREE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_BUFFERS_BOUND_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_BUFFERS_BOUND_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_BUFFERS_BOUND_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_BUFFERS_BOUND_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_bound_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_PDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_PDS_BOUND_FREE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_PDS_BOUND_FREE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_PDS_BOUND_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_PDS_BOUND_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_PDS_BOUND_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BOUND_PRMSm, SRAM_PDS_BOUND_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_fail_sram_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_WORDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bounds_sram_words_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bounds_sram_words_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_WORDS, is_standard_1, is_compatible_with_all_images));
    }
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
                    access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMS_EXTm, SRAM_WORDS_RECOVERY_MAX_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMS_EXTm, SRAM_WORDS_RECOVERY_MIN_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMS_EXTm, SRAM_WORDS_RECOVERY_ADJUST_FACTORf));
                        {
                            access_params->data_offset.formula_int = 3;
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
                    access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMS_EXTm, SRAM_WORDS_RECOVERY_ADJUST_FACTORf));
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
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_WORDS_RECOVERY_MAX_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_WORDS_RECOVERY_MIN_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_WORDS_RECOVERY_ADJUST_FACTORf));
                        {
                            access_params->data_offset.formula_int = 3;
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
                    access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_WORDS_RECOVERY_ADJUST_FACTORf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_fail_sram_buffers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_BUFFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_BUFFERS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_BUFFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_BUFFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_BUFFERS_RECOVERY_FREE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_BUFFERS_RECOVERY_FREE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_BUFFERS_RECOVERY_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_BUFFERS_RECOVERY_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_BUFFERS_RECOVERY_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_BUFFERS_RECOVERY_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_BUFFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_fail_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_PDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FREE_RES_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_PDS_RECOVERY_FREE_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FREE_RES_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_PDS_RECOVERY_FREE_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_PDS_RECOVERY_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_PDS_RECOVERY_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_PDS_RECOVERY_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, SRAM_PDS_RECOVERY_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_sram_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RECOVERY_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RECOVERY_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_RECOVERY_PRMSm, QSIZE_RECOVERY_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_congestion_notification_words_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_WORDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_WORDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_WORDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.info.resource_get(unit, 0)->hw_resolution_nof_bits;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_WORDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_WORDS_FADT_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_WORDS_FADT_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_WORDS_FADT_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 4;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_WORDS_FADT_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_WORDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_congestion_notification_sram_bufers_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_BUFERS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_BUFERS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_BUFERS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_BUFERS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_BUFFERS_FADT_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_BUFFERS_FADT_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_BUFFERS_FADT_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_BUFFERS_FADT_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_BUFERS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_congestion_notification_sram_pds_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_PDS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_PDS, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_PDS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FADT_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pds_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_SIGN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ALPHA_ABS, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_PDS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MAX_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_PDS_FADT_MAX_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_FADT_TH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_PDS_FADT_MIN_THf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ALPHA_SIGN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_PDS_FADT_ADJUST_FACTORf));
                    {
                        access_params->data_offset.formula_int = 3;
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
                access_params->access_field_id = DBAL_FIELD_ALPHA_ABS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_PRMSm, VOQ_SRAM_PDS_FADT_ADJUST_FACTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_PDS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pb_vsq_pg_congestion_notification_fifo_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_PB_VSQ_PG_CONGESTION_NOTIFICATION_FIFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_PB_VSQ_PG_CONGESTION_NOTIFICATION_FIFO, is_valid, "INGRESS_CONG_PB_VSQ_PG_CONGESTION_NOTIFICATION_FIFO", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE, DBAL_FIELD_TYPE_DEF_CONGESTION_NOTIFICATION_RESOURCE_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PG_INDEX, DBAL_FIELD_TYPE_DEF_VSQ_F_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_PB_VSQ_PG_CONGESTION_NOTIFICATION_FIFO, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PG_CONGESTION_FIFO_DATAr, PB_VSQ_PG_CONGESTION_FIFO_DATA_VALIDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PG_CONGESTION_FIFO_DATAr, PB_VSQ_PG_CONGESTION_FIFO_DATA_RESOURCE_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PG_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_PG_CONGESTION_FIFO_DATAr, PB_VSQ_PG_CONGESTION_FIFO_DATA_PG_INDEXf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_PB_VSQ_PG_CONGESTION_NOTIFICATION_FIFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pb_vsq_llfc_congestion_notification_fifo_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_PB_VSQ_LLFC_CONGESTION_NOTIFICATION_FIFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_PB_VSQ_LLFC_CONGESTION_NOTIFICATION_FIFO, is_valid, "INGRESS_CONG_PB_VSQ_LLFC_CONGESTION_NOTIFICATION_FIFO", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 4);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE, DBAL_FIELD_TYPE_DEF_CONGESTION_NOTIFICATION_RESOURCE_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_INDEX, DBAL_FIELD_TYPE_DEF_VSQ_F_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_POOL_ID, DBAL_FIELD_TYPE_DEF_POOL_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_PB_VSQ_LLFC_CONGESTION_NOTIFICATION_FIFO, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_LLFC_CONGESTION_FIFO_DATAr, PB_VSQ_LLFC_CONGESTION_FIFO_DATA_VALIDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_LLFC_CONGESTION_FIFO_DATAr, PB_VSQ_LLFC_CONGESTION_FIFO_DATA_RESOURCE_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_LLFC_CONGESTION_FIFO_DATAr, PB_VSQ_LLFC_CONGESTION_FIFO_DATA_PORT_INDEXf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_POOL_ID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_LLFC_CONGESTION_FIFO_DATAr, PB_VSQ_LLFC_CONGESTION_FIFO_DATA_POOL_IDf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_PB_VSQ_LLFC_CONGESTION_NOTIFICATION_FIFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_congestion_notification_fifo_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_CONGESTION_NOTIFICATION_FIFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_CONGESTION_NOTIFICATION_FIFO, is_valid, "INGRESS_CONG_VOQ_CONGESTION_NOTIFICATION_FIFO", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VALID, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE, DBAL_FIELD_TYPE_DEF_CONGESTION_NOTIFICATION_RESOURCE_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ, DBAL_FIELD_TYPE_DEF_VOQ, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_CONGESTION_NOTIFICATION_FIFO, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_VALID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_FIFO_DATAr, VOQ_CONGESTION_FIFO_DATA_VALIDf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_FIFO_DATAr, VOQ_CONGESTION_FIFO_DATA_RESOURCE_TYPEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VOQ;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_CONGESTION_FIFO_DATAr, VOQ_CONGESTION_FIFO_DATA_QUEUE_INDEXf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_CONGESTION_NOTIFICATION_FIFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_use_mode_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE, is_valid, "INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + (((dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_soft_dram_block_supported))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_USE_MODE, DBAL_FIELD_TYPE_DEF_DRAM_USE_MODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_soft_dram_block_supported) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_BLOCK_ELIGIBLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_soft_dram_block_supported);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0)
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
                    access_params->access_field_id = DBAL_FIELD_DRAM_USE_MODE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, SRAM_DRAM_ONLY_MODEf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_DRAM_BLOCK_ELIGIBLE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_RJCT_PRMSm, DRAM_BLOCK_ELIGIBLEf));
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
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_DRAM_USE_MODE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, SRAM_DRAM_ONLY_MODEf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_DRAM_BLOCK_ELIGIBLE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_PRMSm, DRAM_BLOCK_ELIGIBLEf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_override_pp_port_with_reassembly_context_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT, is_valid, "INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_OVERIDE_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_IS_OVERIDE_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SPB_OW_PP_PORT_WITH_RSM_CONTEXTr, INVALIDf));
                    access_params->data_offset.formula_cb = ingress_cong_override_pp_port_with_reassembly_context_is_overide_enable_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_init_settings_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_INIT_SETTINGS;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_INIT_SETTINGS, is_valid, "INGRESS_CONG_INIT_SETTINGS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 29 + (((dnx_data_ingr_congestion.config.feature_get(unit, dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MC_COUNT_ONCE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PG_MAP_TC_SELECT, DBAL_FIELD_TYPE_DEF_PG_MAP_TC_SELECT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DEQ_RPRT_IS_SRAM_BYTES, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNT_CROPPED_ORIG_SIZE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_STAMP_FTMH_FAP_PORT_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_STAMP_FTMH_CNI_BIT_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_STAMP_FTMH_DSP_EXT_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_STAMP_FTMH_DP_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_STAMP_FTMH_FAP_PORT_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_STAMP_FTMH_CNI_BIT_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_STAMP_FTMH_DSP_EXT_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DRAM_STAMP_FTMH_DP_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_CONG_INDICATION_EN, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_WORDS_IN_GRNTD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VOQ_SRAM_BUFFERS_IN_GRNTD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_WORDS_GRNTD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_GRNTD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PORT_WORDS_SHRD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PORT_WORDS_HDRM_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PG_WORDS_SHRD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PG_WORDS_HDRM_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_WORDS_SHRD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_WORDS_HDRM_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PORT_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PORT_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PG_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_PG_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_ingr_congestion.config.feature_get(unit, dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INB_TELEMETRY_ECO_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_ingr_congestion.config.feature_get(unit, dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_INIT_SETTINGS, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_MC_COUNT_ONCE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SETTINGSr, PB_VSQ_MC_COUNT_ONCEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PG_MAP_TC_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_SETTINGSr, PB_VSQ_TC_SELf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DEQ_RPRT_IS_SRAM_BYTES;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PRG_CTR_SETTINGSr, VOQ_PRG_CTR_QNUM_DEQ_RPRT_SELf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNT_CROPPED_ORIG_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PRG_CTR_SETTINGSr, PRG_CTR_IGNORE_CROPf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_STAMP_FTMH_FAP_PORT_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_FAP_PORTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_STAMP_FTMH_CNI_BIT_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_CNI_BITf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_STAMP_FTMH_DSP_EXT_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_DSP_EXT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SRAM_STAMP_FTMH_DP_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPP_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_DP_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_STAMP_FTMH_FAP_PORT_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_FAP_PORTf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_STAMP_FTMH_CNI_BIT_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_CNI_BITf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_STAMP_FTMH_DSP_EXT_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_DSP_EXT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DRAM_STAMP_FTMH_DP_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ITPPD_STAMPING_FABRIC_HEADER_ENABLEr, STAMP_DP_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VOQ_CONG_INDICATION_EN;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_CONGESTION_INTERRUPT_ENABLERSr, VOQ_CONGESTION_FIFO_INT_ENf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VOQ_WORDS_IN_GRNTD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 4;
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
                access_params->access_field_id = DBAL_FIELD_VOQ_SRAM_BUFFERS_IN_GRNTD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 5;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_WORDS_GRNTD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 4;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_GRNTD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 5;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PORT_WORDS_SHRD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 6;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PORT_WORDS_HDRM_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 7;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PG_WORDS_SHRD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 8;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PG_WORDS_HDRM_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 9;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_WORDS_SHRD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 10;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_WORDS_HDRM_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 11;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PORT_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 12;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PORT_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 13;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PG_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 14;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_PG_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 15;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 16;
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
                access_params->access_field_id = DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 17;
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
                access_params->access_field_id = DBAL_FIELD_INB_TELEMETRY_ECO_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_RESERVED_SPARE_2r, INVALIDf));
                    {
                        access_params->data_offset.formula_int = 0;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_INIT_SETTINGS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_dp_discard_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_DP_DISCARD;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_DP_DISCARD, is_valid, "INGRESS_CONG_GLOBAL_DP_DISCARD", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP_DISCARD, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            db_field.max_value = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_DP_DISCARD, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_DP_DISCARD;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_GLBL_RJCT_PRMSr, DP_LEVEL_RJCT_THf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_DP_DISCARD, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_last_enq_cmd_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_LAST_ENQ_CMD_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_LAST_ENQ_CMD_INFO, is_valid, "LAST_ENQ_CMD_INFO", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENQ_CMD_QNUM, DBAL_FIELD_TYPE_DEF_VOQ, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENQ_CMD_PKT_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 14;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENQ_CMD_COMPENSATION, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_LAST_ENQ_CMD_INFO, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_LAST_ENQ_CMD_QNUM;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SQM_LAST_ENQ_CMDr, INVALIDf));
                        {
                            access_params->data_offset.formula_int = 3;
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
                    access_params->access_field_id = DBAL_FIELD_LAST_ENQ_CMD_PKT_SIZE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SQM_LAST_ENQ_CMDr, INVALIDf));
                        {
                            access_params->data_offset.formula_int = 33;
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
                    access_params->access_field_id = DBAL_FIELD_LAST_ENQ_CMD_COMPENSATION;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SQM_LAST_ENQ_CMDr, INVALIDf));
                        {
                            access_params->data_offset.formula_int = 83;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_LAST_ENQ_CMD_QNUM;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SQM_LAST_ENQ_CMDr, LAST_ENQ_CMD_QNUMf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LAST_ENQ_CMD_PKT_SIZE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SQM_LAST_ENQ_CMDr, LAST_ENQ_CMD_PKT_SIZEf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_LAST_ENQ_CMD_COMPENSATION;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, SQM_LAST_ENQ_CMDr, LAST_ENQ_CMD_OMAC_DELTAf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_LAST_ENQ_CMD_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_dp_gurantee_over_admit_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_DP_GURANTEE_OVER_ADMIT;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_DP_GURANTEE_OVER_ADMIT, is_valid, "INGRESS_CONG_DP_GURANTEE_OVER_ADMIT", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_GUARANTEE_OVER_ADMIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_DP_GURANTEE_OVER_ADMIT, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_IS_GUARANTEE_OVER_ADMIT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_SETTINGSr, INVALIDf));
                    access_params->data_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IS_GUARANTEE_OVER_ADMIT;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_RJCT_SETTINGSr, INVALIDf));
                    access_params->data_offset.formula_cb = ingress_cong_global_free_sram_per_dp_thresholds_table_sram_pdbs_set_threshold_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_DP_GURANTEE_OVER_ADMIT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_words_reject_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_WORDS_REJECT_MAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_WORDS_REJECT_MAP, is_valid, "INGRESS_CONG_VSQ_WORDS_REJECT_MAP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 7);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADMIT_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_TOTAL_SHARED_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_PORT_PG_SHARED_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_TOTAL_HEADROOM_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_PORT_PG_HEADROOM_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_VOQ_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_GUARANTEED_STATUS, DBAL_FIELD_TYPE_DEF_VSQ_GUARANTEED_STATUS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REJECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_WORDS_REJECT_MAP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_REJECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_WORDS_RJCT_MAPm, REJECT_BITMAPf));
                    access_params->entry_offset.formula_cb = ingress_cong_vsq_words_reject_map_reject_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_WORDS_REJECT_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_sram_reject_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_SRAM_REJECT_MAP;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_SRAM_REJECT_MAP, is_valid, "INGRESS_CONG_VSQ_SRAM_REJECT_MAP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 9);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADMIT_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_TOTAL_SHARED_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_PORT_PG_SHARED_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_HEADROOM_EXTENSION_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_TOTAL_HEADROOM_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_PORT_HEADROOM_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_PG_HEADROOM_BLOCKED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_VOQ_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSQ_GUARANTEED_STATUS, DBAL_FIELD_TYPE_DEF_VSQ_GUARANTEED_STATUS, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_REJECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_SRAM_REJECT_MAP, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_REJECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_BUFFERS_RJCT_MAPm, REJECT_BITMAPf));
                    access_params->entry_offset.formula_cb = ingress_cong_vsq_words_reject_map_reject_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_REJECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_SRAM_PDS_RJCT_MAPm, REJECT_BITMAPf));
                    access_params->entry_offset.formula_cb = ingress_cong_vsq_words_reject_map_reject_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_SRAM_REJECT_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_guaranteed_rjct_mask_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK, is_valid, "INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDS_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_WORDS_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJCT_BIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit);
            db_field.max_nof_instances = 38;
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK, is_standard_1, is_compatible_with_all_images));
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
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit); field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->inst_idx = field_instance;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_GRNTD_RJCT_MASKm, INVALIDf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_guaranteed_rjct_mask_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK, is_valid, "INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_PDS_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SRAM_BUFFERS_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_WORDS_IN_GUARANTEED, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJCT_BIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit);
            db_field.max_nof_instances = 38;
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK, is_standard_1, is_compatible_with_all_images));
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
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit); field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->inst_idx = field_instance;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VSQ_GRNTD_RJCT_MASKm, INVALIDf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_port_based_vsq_rjct_mask_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK, is_valid, "INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ADMIT_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IS_LOSSLESS, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJCT_BIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit);
            db_field.max_nof_instances = 38;
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK, is_standard_1, is_compatible_with_all_images));
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
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit); field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->inst_idx = field_instance;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PB_VSQ_RJCT_MASKm, INVALIDf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pp_rjct_mask_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK, is_valid, "INGRESS_CONG_PP_RJCT_MASK", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PP_ADMIT_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJCT_BIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit);
            db_field.max_nof_instances = 38;
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK, is_standard_1, is_compatible_with_all_images));
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
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit); field_instance++)
                    {
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                        access_params->inst_idx = field_instance;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_PP_RJCT_MASKm, INVALIDf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_dram_block_bitmap_mask_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_BITMAP_MASK;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_BITMAP_MASK, is_valid, "INGRESS_CONG_DRAM_BLOCK_BITMAP_MASK", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + 24);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJCT_BIT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = 24;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_BITMAP_MASK, is_standard_1, is_compatible_with_all_images));
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
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < 24; field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_DP, 0, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, VOQ_DRAM_BLOCK_BMP_MASK_DP_0f));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < 24; field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_DP, 1, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, VOQ_DRAM_BLOCK_BMP_MASK_DP_1f));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < 24; field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_DP, 2, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, VOQ_DRAM_BLOCK_BMP_MASK_DP_2f));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < 24; field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_BIT;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_DP, 3, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_BMP_MASKr, VOQ_DRAM_BLOCK_BMP_MASK_DP_3f));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_BITMAP_MASK, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_dram_block_global_params_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_GLOBAL_PARAMS;
    int is_valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_soft_dram_block_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_GLOBAL_PARAMS, is_valid, "INGRESS_CONG_DRAM_BLOCK_GLOBAL_PARAMS", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FREE_SRAM_BUFFERS_FOR_SOFT_BLOCK, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_FREE_SRAM_PDBS_FOR_SOFT_BLOCK, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_GLOBAL_PARAMS, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_JR2_B0)
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
                    access_params->access_field_id = DBAL_FIELD_MIN_FREE_SRAM_BUFFERS_FOR_SOFT_BLOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_RJCT_THRESHOLDSr, VOQ_DRAM_BLOCK_SRAM_BUFFERS_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FREE_SRAM_PDBS_FOR_SOFT_BLOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_RJCT_THRESHOLDSr, VOQ_DRAM_BLOCK_SRAM_PDBS_THf));
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
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FREE_SRAM_BUFFERS_FOR_SOFT_BLOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_SETTINGSr, VOQ_DRAM_BLOCK_SRAM_BUFFERS_THf));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MIN_FREE_SRAM_PDBS_FOR_SOFT_BLOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_VOQ_DRAM_BLOCK_SETTINGSr, VOQ_DRAM_BLOCK_SRAM_PDBS_THf));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_GLOBAL_PARAMS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_fifo_size_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_FIFO_SIZE;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_FIFO_SIZE, is_valid, "INGRESS_CONG_FIFO_SIZE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIFO_SIZE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_FIFO_SIZE, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_FIFO_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_FIFO_SIZESr, MCH_FIFO_SIZEf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_FIFO_SIZESr, MCL_FIFO_SIZEf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FIFO_SIZE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_FIFO_SIZESr, UC_FIFO_SIZEf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_FIFO_SIZE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_mirror_on_drop_group_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_GROUP;
    int is_valid = dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_GROUP, is_valid, "INGRESS_CONG_MIRROR_ON_DROP_GROUP", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    table_entry->table_labels[1] = DBAL_LABEL_SNIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1 + dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RJCT_MASK, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit);
            db_field.max_nof_instances = 47;
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP_TYPE, DBAL_FIELD_TYPE_DEF_MIRROR_ON_DROP_GROUP_TYPE, dbal_db_field));
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
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_GROUP, is_standard_1, is_compatible_with_all_images));
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
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 0, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_0_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 1, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_1_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 2, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_2_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 3, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_3_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 4, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_4_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 5, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_5_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 6, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_6_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
                    }
                }
            }
            {
                {
                    int field_instance = 0;
                    for (field_instance = 0; field_instance < dnx_data_stat.drop_reasons.nof_drop_reasons_get(unit); field_instance++)
                    {
                        int condition_index = 0;
                        access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                        dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                        access_counter++;
                        access_params->access_field_id = DBAL_FIELD_RJCT_MASK;
                        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                        access_params->inst_idx = field_instance;
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_GROUP, 7, (uint32) DBAL_DB_INVALID));
                        condition_index++;
                        access_params->nof_conditions = condition_index;
                        {
                            SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_REASON_MASKr, MIRROR_RJCT_REASON_7_RJCT_MASKf));
                            access_params->data_offset.formula_cb = kleap_fwd12_resources_mapping_kbr_selector_dataoffset_0_cb;
                        }
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
                access_params->access_field_id = DBAL_FIELD_GROUP_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_SETTINGSr, MIRROR_RJCT_REASON_TYPEf));
                    access_params->data_offset.formula_cb = ingress_cong_mirror_on_drop_group_group_type_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_GROUP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_mirror_on_drop_aging_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING;
    int is_valid = dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING, is_valid, "INGRESS_CONG_MIRROR_ON_DROP_AGING", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    table_entry->table_labels[1] = DBAL_LABEL_SNIF;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP_TYPE_GLOBAL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP_TYPE_VOQ, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GROUP_TYPE_VSQ, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_GROUP_TYPE_GLOBAL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_SETTINGSr, MIRROR_RJCT_GLBL_RJCT_TIMERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GROUP_TYPE_VOQ;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_SETTINGSr, MIRROR_RJCT_VOQ_RJCT_TIMERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GROUP_TYPE_VSQ;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_SETTINGSr, MIRROR_RJCT_VSQ_RJCT_TIMERf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_mirror_on_drop_shaper_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_SHAPER;
    int is_valid = dnx_data_ingr_congestion.mirror_on_drop.feature_get(unit, dnx_data_ingr_congestion_mirror_on_drop_is_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_SHAPER, is_valid, "INGRESS_CONG_MIRROR_ON_DROP_SHAPER", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_ITM;
    table_entry->table_labels[1] = DBAL_LABEL_SNIF;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RATE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 26;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BURST, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 7;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_SHAPER, is_standard_1, is_compatible_with_all_images));
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
                access_params->access_field_id = DBAL_FIELD_RATE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_SETTINGSr, MIRROR_RJCT_MAX_RATEf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BURST;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_MIRROR_RJCT_SETTINGSr, MIRROR_RJCT_MAX_BURSTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_SHAPER, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_ecn_marking_thresholds_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE;
    int is_valid = dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_global_ecn_marking_supported);
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE, is_valid, "INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC));
    if (!is_valid)
    {
        SHR_EXIT();
    }
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 3);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_PDBS_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_BUFFERS_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ECN_MARKING_TOTAL_FREE_DRAM_BDBS_TH, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_ingr_congestion.dbal.dram_bdbs_th_nof_bits_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    {
        uint8 is_standard_1 = FALSE;
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE, is_standard_1, is_compatible_with_all_images));
    }
    if (DBAL_IS_J2P_A0)
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
                    access_params->access_field_id = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_PDBS_TH;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0224r, INVALIDf));
                        {
                            access_params->data_offset.formula_int = 0;
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
                    access_params->access_field_id = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_BUFFERS_TH;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0224r, INVALIDf));
                        {
                            access_params->data_offset.formula_int = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit);
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
                    access_params->access_field_id = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_DRAM_BDBS_TH;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0224r, INVALIDf));
                        {
                            access_params->data_offset.formula_int = dnx_data_ingr_congestion.dbal.sram_pdbs_th_nof_bits_get(unit)+dnx_data_ingr_congestion.dbal.sram_buffer_free_th_nof_bits_get(unit);
                        }
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
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
                    access_params->access_field_id = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_PDBS_TH;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0225r, FIELD_0_10f));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_SRAM_BUFFERS_TH;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0225r, FIELD_11_22f));
                    }
                }
            }
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_ECN_MARKING_TOTAL_FREE_DRAM_BDBS_TH;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, CGM_REG_0225r, FIELD_23_35f));
                    }
                }
            }
            cur_table_param->hl_access[map_idx].nof_access = access_counter;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_free_shared_threshold_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_sram_per_dp_thresholds_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_sram_per_tc_thresholds_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_dram_per_dp_thresholds_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_free_dram_per_tc_thresholds_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_command_fifo_sram_reject_threshold_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_guaranteed_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_guaranteed_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_guaranteed_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_fadt_drop_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_fadt_drop_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_fadt_drop_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_wred_ecn_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_wred_drop_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_wred_avrg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_tail_drop_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_tail_drop_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_tail_drop_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_tail_drop_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_tail_drop_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_tail_drop_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_tail_drop_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_tail_drop_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_tail_drop_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_tail_drop_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_tail_drop_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_tail_drop_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_resource_alloc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_resource_alloc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_resource_alloc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_resource_alloc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_resource_alloc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_resource_alloc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_vsq_resource_alloc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_vsq_resource_alloc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_vsq_resource_alloc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_fc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_fc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_a_rate_class_fc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_fc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_fc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_b_rate_class_fc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_fc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_fc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_c_rate_class_fc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_fc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_fc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_d_rate_class_fc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_fc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_fc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_e_rate_class_fc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_fc_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_fc_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_f_rate_class_fc_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_port_to_src_vsq_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_tc_pg_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pg_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_cgm_dram_bound_state_th_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_profile_info_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_shared_reject_status_thr_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_shared_reject_status_thr_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_shared_reject_status_thr_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_bound_sram_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_bound_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_bound_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_fail_sram_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_fail_sram_buffers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_fail_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_recovery_sram_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_congestion_notification_words_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_congestion_notification_sram_bufers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_congestion_notification_sram_pds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pb_vsq_pg_congestion_notification_fifo_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pb_vsq_llfc_congestion_notification_fifo_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_congestion_notification_fifo_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_rate_class_dram_use_mode_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_override_pp_port_with_reassembly_context_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_init_settings_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_dp_discard_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_last_enq_cmd_info_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_dp_gurantee_over_admit_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_words_reject_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_sram_reject_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_voq_guaranteed_rjct_mask_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_vsq_guaranteed_rjct_mask_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_port_based_vsq_rjct_mask_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_pp_rjct_mask_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_dram_block_bitmap_mask_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_dram_block_global_params_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_fifo_size_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_mirror_on_drop_group_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_mirror_on_drop_aging_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_mirror_on_drop_shaper_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_ingress_cong_global_ecn_marking_thresholds_table_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
