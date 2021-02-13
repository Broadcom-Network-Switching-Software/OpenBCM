/** \file dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf1_initial.c
 * Logical DB enums \n 
 * DO NOT EDIT THIS FILE!\n 
 * This file is auto-generated.\n 
 * Edits to this file will be lost when it is regenerated.\n 
 * \n 
 */
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf1_initial_kleap_ipmf1_initial_kbr_info_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO;
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    SHR_IF_ERR_EXIT(dbal_db_init_general_info_set(unit, table_entry, DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO /* table id*/, is_valid /* is valid*/, "KLEAP_IPMF1_INITIAL_KBR_INFO" /* table name*/, DBAL_ACCESS_METHOD_HARD_LOGIC /* access method*/));
    /** maturity level  */
    /** default maturity level  */
    table_entry->maturity_level = DBAL_MATURITY_HIGH;
    /** *************  */
    /** set interface  */
    /** *************  */
    /** set type  */
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    /** set labels  */
    /** nof labels  */
    table_entry->nof_labels = 1;
    SHR_ALLOC_SET_ZERO(table_entry->table_labels, table_entry->nof_labels * sizeof(dbal_labels_e), "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    table_entry->table_labels[0] = DBAL_LABEL_FIELD;
    /** interface keys  */
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        SHR_ALLOC_SET_ZERO(table_entry->keys_info, 2 * sizeof(dbal_table_field_info_t), "key fields info allocation", "%s%s%s\r\n", "KLEAP_IPMF1_INITIAL_KBR_INFO" /* table name*/, EMPTY, EMPTY);
        /** interface key fields init  */
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIELD_KEY /* field id*/, DBAL_FIELD_TYPE_DEF_FIELD_KEY /* Type*/, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.min_value = 5;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FIELD_PMF_CTX_ID /* field id*/, DBAL_FIELD_TYPE_DEF_FIELD_PMF_CTX_ID /* Type*/, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, TRUE));
            field_index += dbal_db_field->nof_instances;
        }
        table_entry->nof_key_fields = field_index;
        dbal_db_init_table_calculate_key_size(table_entry);
    }
    /** interface results  */
    {
        int field_index = 0;
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        /** interface single result  */
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 1);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        /** interface result fields init  */
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FFC_BITMAP /* field id*/, DBAL_FIELD_TYPE_DEF_BITMAP /* Type*/, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 64;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
    }
    /** *************  */
    /** app to phy db  */
    /** *************  */
    /** clear access section  */
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    /** image infos  */
    {
        /** is_standard_1_table is set by Autocoder  */
        uint8 is_standard_1 = FALSE;
        /** is_compatible_with_all_images is set by Autocoder  */
        uint8 is_compatible_with_all_images = TRUE;
        DBAL_DB_INIT_TABLE_IMAGE_SPECIFIC_TABLE_INDICATION_SET(table_entry, is_standard_1, is_compatible_with_all_images);
        SHR_IF_ERR_EXIT(dbal_db_init_table_set_table_incompatible_image_sw_state_indication(unit, DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO, is_standard_1, is_compatible_with_all_images));
    }
    /** set app to phy db general information  */
    /** core mode  */
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    /** result type hw value by index  */
    dbal_db_init_table_add_result_type_physical_values_by_result_type_index(table_entry);
    /** build access hl direct  */
    {
        int map_idx = 0;
        int access_counter;
        map_idx = 0;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, EMPTY /* result type*/, sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        /** set (hl direct) accesses for field: FFC_BITMAP  */
        {
            table_db_access_params_struct_t * access_params;
            /** set access  */
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                /** set hard logic access type  */
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE /* is register*/, TRUE /* is memory*/););
                /** set access information  */
                /** set access field id  */
                access_params->access_field_id = DBAL_FIELD_FFC_BITMAP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 /* field size*/, 0 /* field offset*/, DBAL_VALUE_FIELD_ENCODE_NONE /* encode type*/, 0 /* encode value*/));
                /** add conditions  */
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO /* type*/, DBAL_FIELD_FIELD_KEY /* field*/, 0 /* value*/, (uint32) DBAL_ENUM_FVAL_FIELD_KEY_F /* enum value*/));
                condition_index++;
                access_params->nof_conditions = condition_index;
                /** set register/memory information for current access  */
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    /** set reg/mem hw Entity id  */
                    regMemHwEntityId = IPPC_PMF_KBR_PASS_1m;
                    /** set field hw Entity id  */
                    fieldHwEntityId = KEY_F_FFC_VALIDSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY /* group id*/, regMemHwEntityId /* reg/mem hw entity id*/, fieldHwEntityId /* field hw entity id*/, INVALIDm /* alias hw entity id*/));
                    access_params->entry_offset.formula_cb = field_context_hash_configuration_compression_dataoffset_0_cb;
                }
            }
            /** set access  */
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                /** set hard logic access type  */
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE /* is register*/, TRUE /* is memory*/););
                /** set access information  */
                /** set access field id  */
                access_params->access_field_id = DBAL_FIELD_FFC_BITMAP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 /* field size*/, 0 /* field offset*/, DBAL_VALUE_FIELD_ENCODE_NONE /* encode type*/, 0 /* encode value*/));
                /** add conditions  */
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO /* type*/, DBAL_FIELD_FIELD_KEY /* field*/, 0 /* value*/, (uint32) DBAL_ENUM_FVAL_FIELD_KEY_G /* enum value*/));
                condition_index++;
                access_params->nof_conditions = condition_index;
                /** set register/memory information for current access  */
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    /** set reg/mem hw Entity id  */
                    regMemHwEntityId = IPPC_PMF_KBR_PASS_1m;
                    /** set field hw Entity id  */
                    fieldHwEntityId = KEY_G_FFC_VALIDSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY /* group id*/, regMemHwEntityId /* reg/mem hw entity id*/, fieldHwEntityId /* field hw entity id*/, INVALIDm /* alias hw entity id*/));
                    access_params->entry_offset.formula_cb = field_context_hash_configuration_compression_dataoffset_0_cb;
                }
            }
            /** set access  */
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                /** set hard logic access type  */
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE /* is register*/, TRUE /* is memory*/););
                /** set access information  */
                /** set access field id  */
                access_params->access_field_id = DBAL_FIELD_FFC_BITMAP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 /* field size*/, 0 /* field offset*/, DBAL_VALUE_FIELD_ENCODE_NONE /* encode type*/, 0 /* encode value*/));
                /** add conditions  */
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO /* type*/, DBAL_FIELD_FIELD_KEY /* field*/, 0 /* value*/, (uint32) DBAL_ENUM_FVAL_FIELD_KEY_H /* enum value*/));
                condition_index++;
                access_params->nof_conditions = condition_index;
                /** set register/memory information for current access  */
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    /** set reg/mem hw Entity id  */
                    regMemHwEntityId = IPPC_PMF_KBR_PASS_1m;
                    /** set field hw Entity id  */
                    fieldHwEntityId = KEY_H_FFC_VALIDSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY /* group id*/, regMemHwEntityId /* reg/mem hw entity id*/, fieldHwEntityId /* field hw entity id*/, INVALIDm /* alias hw entity id*/));
                    access_params->entry_offset.formula_cb = field_context_hash_configuration_compression_dataoffset_0_cb;
                }
            }
            /** set access  */
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                /** set hard logic access type  */
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE /* is register*/, TRUE /* is memory*/););
                /** set access information  */
                /** set access field id  */
                access_params->access_field_id = DBAL_FIELD_FFC_BITMAP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 /* field size*/, 0 /* field offset*/, DBAL_VALUE_FIELD_ENCODE_NONE /* encode type*/, 0 /* encode value*/));
                /** add conditions  */
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO /* type*/, DBAL_FIELD_FIELD_KEY /* field*/, 0 /* value*/, (uint32) DBAL_ENUM_FVAL_FIELD_KEY_I /* enum value*/));
                condition_index++;
                access_params->nof_conditions = condition_index;
                /** set register/memory information for current access  */
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    /** set reg/mem hw Entity id  */
                    regMemHwEntityId = IPPC_PMF_KBR_PASS_1m;
                    /** set field hw Entity id  */
                    fieldHwEntityId = KEY_I_FFC_VALIDSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY /* group id*/, regMemHwEntityId /* reg/mem hw entity id*/, fieldHwEntityId /* field hw entity id*/, INVALIDm /* alias hw entity id*/));
                    access_params->entry_offset.formula_cb = field_context_hash_configuration_compression_dataoffset_0_cb;
                }
            }
            /** set access  */
            {
                int instance_index = 0;
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                /** set hard logic access type  */
                SHR_IF_ERR_EXIT(dbal_db_init_hard_logic_direct_set_access_type(unit, access_params, FALSE /* is register*/, TRUE /* is memory*/););
                /** set access information  */
                /** set access field id  */
                access_params->access_field_id = DBAL_FIELD_FFC_BITMAP + instance_index;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_field(unit, access_params, 0 /* field size*/, 0 /* field offset*/, DBAL_VALUE_FIELD_ENCODE_NONE /* encode type*/, 0 /* encode value*/));
                /** add conditions  */
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO /* type*/, DBAL_FIELD_FIELD_KEY /* field*/, 0 /* value*/, (uint32) DBAL_ENUM_FVAL_FIELD_KEY_J /* enum value*/));
                condition_index++;
                access_params->nof_conditions = condition_index;
                /** set register/memory information for current access  */
                {
                    int regMemHwEntityId = INVALIDm;
                    int fieldHwEntityId = INVALIDf;
                    /** set reg/mem hw Entity id  */
                    regMemHwEntityId = IPPC_PMF_KBR_PASS_1m;
                    /** set field hw Entity id  */
                    fieldHwEntityId = KEY_J_FFC_VALIDSf;
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HW_ENTITY_GROUP_EMPTY /* group id*/, regMemHwEntityId /* reg/mem hw entity id*/, fieldHwEntityId /* field hw entity id*/, INVALIDm /* alias hw entity id*/));
                    access_params->entry_offset.formula_cb = field_context_hash_configuration_compression_dataoffset_0_cb;
                }
            }
        }
        /** set nof access for field mapping  */
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf1_initial_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf1_initial_kleap_ipmf1_initial_kbr_info_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
