
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_enablers_vectors_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_ENABLERS_VECTORS;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_ENABLERS_VECTORS, is_valid, maturity_level, "ENABLERS_VECTORS", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_L3;
    table_entry->table_labels[1] = DBAL_LABEL_L3_IF_RIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, DBAL_FIELD_TYPE_DEF_ROUTING_ENABLERS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 5;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ENABLERS_VECTOR, DBAL_FIELD_TYPE_DEF_ENABLERS_VECTOR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_l3.routing_enablers.routing_enable_vector_length_get(unit);
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
                access_params->access_field_id = DBAL_FIELD_ENABLERS_VECTOR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPA_MAP_RIF_PROFILE_TO_ROUTING_ENABLEm, ROUTING_ENABLE_VECTORf));
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ENABLERS_VECTOR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPA_MAP_RIF_PROFILE_TO_ROUTING_ENABLEm, ROUTING_ENABLE_VECTORf));
                    {
                        access_params->array_offset.formula_int = 4;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ENABLERS_VECTOR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPF_MAP_RIF_PROFILE_TO_ROUTING_ENABLEm, ROUTING_ENABLE_VECTORf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_ENABLERS_VECTORS, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_in_lif_profile_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_IN_LIF_PROFILE_TABLE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_IN_LIF_PROFILE_TABLE, is_valid, maturity_level, "IN_LIF_PROFILE_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_L3;
    table_entry->table_labels[1] = DBAL_LABEL_L3_IF_RIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_IN_LIF_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 11 + (((!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config))>0)?1:0) + (((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config))>0)?1:0) + (((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config))>0)?1:0) + (((!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile))>0)?1:0) + (((!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile))>0)?1:0) + (((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_cs_in_lif_profile_based_on_stages))>0)?1:0) + (((!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile))>0)?1:0) + (((!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile))>0)?1:0) + (((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_in_lif_profile_iop_mode))>0)?1:0) + (((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_mapped_in_lif_profile))>0)?1:0) + (((dnx_data_lif.in_lif.mc_bridge_fallback_per_lif_profile_get(unit))>0)?1:0) + (((!dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_orientation_selection))>0)?1:0) + (((dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPF_MODE, DBAL_FIELD_TYPE_DEF_RPF_MODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPF_MODE_IPV4, DBAL_FIELD_TYPE_DEF_RPF_MODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RPF_MODE_IPV6, DBAL_FIELD_TYPE_DEF_RPF_MODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_separate_ipv4_ipv6_urpf_mode_config);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_ORIENTATION, DBAL_FIELD_TYPE_DEF_IN_LIF_ORIENTATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_SAME_INTERFACE, DBAL_FIELD_TYPE_DEF_IN_LIF_SAME_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L_2_CP_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_L_2_CP_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICMPV4_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_ICMPV4_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICMPV6_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_ICMPV6_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ARP_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_ARP_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IGMP_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_IGMP_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DHCP_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_DHCP_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NON_AUTH_8021X_TRAP_HANDLING_PROFILE, DBAL_FIELD_TYPE_DEF_NON_AUTH_8021X_TRAP_HANDLING_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CS_IN_LIF_VTT_TO_VTT_PROFILE, DBAL_FIELD_TYPE_DEF_CS_IN_LIF_VTT_TO_VTT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_cs_in_lif_profile_based_on_stages) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CS_IN_LIF_VTT_TO_FLP_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_cs_in_lif_profile_based_on_stages);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CS_IN_LIF_FLP_TO_FLP_PROFILE, DBAL_FIELD_TYPE_DEF_CS_IN_LIF_FLP_TO_FLP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DP_PROFILE, DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DP_PROFILE, DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE, DBAL_FIELD_TYPE_DEF_DA_NOT_FOUND_DESTINATION_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OAM_DEFAULT_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_in_lif_profile_iop_mode) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_JR_MODE_IN_LIF_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_in_lif_profile_iop_mode);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_mapped_in_lif_profile) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAPPED_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_MAPPED_IN_LIF_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_mapped_in_lif_profile);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_lif.in_lif.mc_bridge_fallback_per_lif_profile_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MC_BRIDGE_FALLBACK_ALLOW, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_lif.in_lif.mc_bridge_fallback_per_lif_profile_get(unit);
            db_field.size = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_orientation_selection) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_IN_LIF_ORIENTATION, DBAL_FIELD_TYPE_DEF_JR_IN_LIF_ORIENTATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_orientation_selection);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_LB_PROFILE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_switch.load_balancing.hashing_selection_per_layer_get(unit);
            db_field.size = 2;
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
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_RPF_MODE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_PROFILE_TABLEm, IN_LIF_RPF_MODEf));
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
                    access_params->access_field_id = DBAL_FIELD_RPF_MODE_IPV4;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_PROFILE_TABLEm, IN_LIF_RPF_MODE_IPV4f));
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
                    access_params->access_field_id = DBAL_FIELD_RPF_MODE_IPV6;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_PROFILE_TABLEm, IN_LIF_RPF_MODE_IPV6f));
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
                access_params->access_field_id = DBAL_FIELD_IN_LIF_ORIENTATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ERPP_CFG_MAPPING_INLIF_PROFILEm, INLIF_ORIENTATIONf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IN_LIF_ORIENTATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ETPPC_INLIF_ORIENTATION_MAPPINGm, INLIF_ORIENTATION_MAPPINGf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IN_LIF_SAME_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ERPP_CFG_MAPPING_INLIF_PROFILEm, INLIF_SAME_IF_FILTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_L_2_CP_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, L_2_CP_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICMPV4_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, ICMPV4_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICMPV6_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, ICMPV6_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IGMP_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, IGMP_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_NON_AUTH_8021X_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, NON_AUTH_8021X_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DHCP_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, DHCP_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ARP_TRAP_HANDLING_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_TRAP_PROFILING_TABLEm, ARP_TRAP_HANDLING_PROFILEf));
                    access_params->entry_offset.formula_cb = in_lif_profile_table_l_2_cp_trap_handling_profile_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_CS_IN_LIF_VTT_TO_VTT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_IN_LIF_PROFILE, 128, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_INGRESS_CS_INLIF_PROFILE_MAP_LSB);
                    access_params->data_offset.formula_cb = in_lif_profile_table_cs_in_lif_vtt_to_vtt_profile_dataoffset_0_cb;
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CS_IN_LIF_VTT_TO_VTT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_IN_LIF_PROFILE, 127, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_INGRESS_CS_INLIF_PROFILE_MAP_MSB);
                    access_params->data_offset.formula_cb = in_lif_profile_table_cs_in_lif_vtt_to_vtt_profile_dataoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_CS_IN_LIF_FLP_TO_FLP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_LOWER_THAN, DBAL_FIELD_IN_LIF_PROFILE, 128, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_CS_INLIF_PROFILE_MAP_0r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = in_lif_profile_table_cs_in_lif_flp_to_flp_profile_dataoffset_0_cb;
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CS_IN_LIF_FLP_TO_FLP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_BIGGER_THAN, DBAL_FIELD_IN_LIF_PROFILE, 127, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_CS_INLIF_PROFILE_MAP_1r, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = in_lif_profile_table_cs_in_lif_flp_to_flp_profile_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, CFG_INLIF_DP_PROFILE_MAPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_DP_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, CFG_INLIF_DP_PROFILE_MAPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_DP_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                    {
                        uint32 result_size__inner_dp_profile = 0;
                        if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile))
                        {
                            result_size__inner_dp_profile += 2;
                        }
                        access_params->data_offset.formula_int = result_size__inner_dp_profile;
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
                access_params->access_field_id = DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, DA_NOT_FOUND_DESTINATION_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_INGRESS_LIF_PROFILE_ATTRIBUTES_1);
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OAM_DEFAULT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_OAM_IN_LIF_PROFILE_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = in_lif_profile_table_oam_default_profile_dataoffset_0_cb;
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_JR_MODE_IN_LIF_PROFILE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_PROFILE_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = in_lif_profile_table_jr_mode_in_lif_profile_dataoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_JR_MODE_IN_LIF_PROFILE;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_JR_2_TO_JR_IN_LIF_PROFILE_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = in_lif_profile_table_jr_mode_in_lif_profile_dataoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_MAPPED_IN_LIF_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_PROFILE_MAPm, MAPPED_IN_LIF_PROFILEf));
                }
            }
        }
        if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_MC_BRIDGE_FALLBACK_ALLOW;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_LIF_MC_BRIDGE_FALLBACK_ALLOWr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = in_lif_profile_table_mc_bridge_fallback_allow_dataoffset_0_cb;
                    }
                }
            }
        }
        if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_JR_IN_LIF_ORIENTATION;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_ORIENTATION_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = in_lif_profile_table_jr_in_lif_orientation_dataoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_IN_LIF_LB_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPB_IN_LIF_LB_PROFILE_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = in_lif_profile_table_in_lif_lb_profile_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_IN_LIF_PROFILE_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_mapped_in_lif_profile_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_MAPPED_IN_LIF_PROFILE_TABLE;
    int is_valid = dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_use_mapped_in_lif_profile);
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_MAPPED_IN_LIF_PROFILE_TABLE, is_valid, maturity_level, "MAPPED_IN_LIF_PROFILE_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_L3;
    table_entry->table_labels[1] = DBAL_LABEL_L3_IF_RIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAPPED_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_MAPPED_IN_LIF_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_ORIENTATION, DBAL_FIELD_TYPE_DEF_IN_LIF_ORIENTATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 3;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_SAME_INTERFACE, DBAL_FIELD_TYPE_DEF_IN_LIF_SAME_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DP_PROFILE, DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DP_PROFILE, DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_IN_LIF_ORIENTATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ERPP_CFG_MAPPING_INLIF_PROFILEm, INLIF_ORIENTATIONf));
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IN_LIF_ORIENTATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ETPPC_INLIF_ORIENTATION_MAPPINGm, INLIF_ORIENTATION_MAPPINGf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IN_LIF_SAME_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ERPP_CFG_MAPPING_INLIF_PROFILEm, INLIF_SAME_IF_FILTERf));
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, CFG_INLIF_DP_PROFILE_MAPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_DP_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, CFG_INLIF_DP_PROFILE_MAPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_DP_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                    {
                        uint32 result_size__inner_dp_profile = 0;
                        result_size__inner_dp_profile += 2;
                        access_params->data_offset.formula_int = result_size__inner_dp_profile;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_MAPPED_IN_LIF_PROFILE_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_vsi_profile_table_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_VSI_PROFILE_TABLE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_VSI_PROFILE_TABLE, is_valid, maturity_level, "VSI_PROFILE_TABLE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 2;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_L3;
    table_entry->table_labels[1] = DBAL_LABEL_L3_IF_RIF;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSI_PROFILE, DBAL_FIELD_TYPE_DEF_VSI_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 10;
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, DBAL_FIELD_TYPE_DEF_ROUTING_ENABLERS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSI_PROFILE_BRIDGE, DBAL_FIELD_TYPE_DEF_VSI_PROFILE_BRIDGE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L2_V4_MC_FWD_TYPE, DBAL_FIELD_TYPE_DEF_L2_V4_MC_FWD_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_L2_V6_MC_FWD_TYPE, DBAL_FIELD_TYPE_DEF_L2_V6_MC_FWD_TYPE, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_ROUTING_ENABLERS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPA_ROUTING_PROFILE_TABLEm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        access_params->array_offset.formula_int = 3;
                    }
                    access_params->entry_offset.formula_cb = vsi_profile_table_routing_enablers_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_routing_enablers_profile_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ROUTING_ENABLERS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPA_ROUTING_PROFILE_TABLEm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    {
                        access_params->array_offset.formula_int = 4;
                    }
                    access_params->entry_offset.formula_cb = vsi_profile_table_routing_enablers_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_routing_enablers_profile_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ROUTING_ENABLERS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPF_ROUTING_PROFILE_TABLEm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->entry_offset.formula_cb = vsi_profile_table_routing_enablers_profile_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_routing_enablers_profile_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_L2_V6_MC_FWD_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, FWD_DOMAIN_CS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_INGRESS_FWD_DOMAIN_CS_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_L2_V4_MC_FWD_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, FWD_DOMAIN_CS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_INGRESS_FWD_DOMAIN_CS_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_l2_v4_mc_fwd_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VSI_PROFILE_BRIDGE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, FWD_DOMAIN_CS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_INGRESS_FWD_DOMAIN_CS_PROFILE_MAP);
                    access_params->entry_offset.formula_cb = vsi_profile_table_l2_v6_mc_fwd_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = vsi_profile_table_vsi_profile_bridge_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_VSI_PROFILE_TABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_jr_mode_inlif_profile_orientation_map_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_JR_MODE_INLIF_PROFILE_ORIENTATION_MAP;
    int is_valid = dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_orientation_selection);
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_JR_MODE_INLIF_PROFILE_ORIENTATION_MAP, is_valid, maturity_level, "JR_MODE_INLIF_PROFILE_ORIENTATION_MAP", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_JR_MODE_IN_LIF_PROFILE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_IN_LIF_ORIENTATION, DBAL_FIELD_TYPE_DEF_JR_IN_LIF_ORIENTATION, dbal_db_field));
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
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_JR_IN_LIF_ORIENTATION;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, IPPD_IN_LIF_ORIENTATION_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                        access_params->data_offset.formula_cb = jr_mode_inlif_profile_orientation_map_jr_in_lif_orientation_dataoffset_0_cb;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_JR_MODE_INLIF_PROFILE_ORIENTATION_MAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_jr_mode_inlif_profile_egress_remap_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_JR_MODE_INLIF_PROFILE_EGRESS_REMAP;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_JR_MODE_INLIF_PROFILE_EGRESS_REMAP, is_valid, maturity_level, "JR_MODE_INLIF_PROFILE_EGRESS_REMAP", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
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
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 3);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_MODE_SYSTEM_HEADER_RESERVED_VALUE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_IN_LIF_ORIENTATION, DBAL_FIELD_TYPE_DEF_JR_IN_LIF_ORIENTATION, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_JR_MODE_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_JR_MODE_IN_LIF_PROFILE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IN_LIF_PROFILE, DBAL_FIELD_TYPE_DEF_IN_LIF_PROFILE, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_IN_LIF_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, INLIF_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_EGRESS_CFG_INLIF_PROFILE_MAPPED_IN_JERICHO_MODE);
                    access_params->entry_offset.formula_cb = field_pmf_small_exem_learning_info_map_format_value_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_JR_MODE_INLIF_PROFILE_EGRESS_REMAP, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_enablers_vectors_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_in_lif_profile_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_mapped_in_lif_profile_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_vsi_profile_table_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_jr_mode_inlif_profile_orientation_map_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_in_lif_profile_hl_pp_in_lif_profile_jr_mode_inlif_profile_egress_remap_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
