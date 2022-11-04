
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_mep_db_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_OAMP_MEP_DB;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_OAMP_MEP_DB, is_valid, maturity_level, "OAMP_MEP_DB", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_OAM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OAMP_ENTRY_ID, DBAL_FIELD_TYPE_DEF_OAMP_ENTRY_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_oam.oamp.max_value_of_mep_id_get(unit);
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
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 22);
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 32);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MDL, DBAL_FIELD_TYPE_DEF_MDL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAID, DBAL_FIELD_TYPE_DEF_MAID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_VLAN_TAGS, DBAL_FIELD_TYPE_DEF_NOF_VLAN_TAGS, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, DBAL_FIELD_TYPE_DEF_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_EN, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_VAL, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_BASE, DBAL_FIELD_TYPE_DEF_COUNTER_BASE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_INTERFACE, DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LMM_DA_LSB_PROFILE, DBAL_FIELD_TYPE_DEF_LMM_DA_LSB_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_MAC_LSB, DBAL_FIELD_TYPE_DEF_L2_MAC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_GEN_PROFILE, DBAL_FIELD_TYPE_DEF_SA_GEN_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DA_UC_MC_TYPE, DBAL_FIELD_TYPE_DEF_DA_UC_MC_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "ETH_OAM_DOWN_MEP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP_EGRESS_INJECTION;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 30 + (((dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0) + (((dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0) + (((!dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0) + (((dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0) + (((!dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0) + (((dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0) + (((!dnx_data_oam.oamp.mep_db_jr1_mode_get(unit))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MDL, DBAL_FIELD_TYPE_DEF_MDL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAID, DBAL_FIELD_TYPE_DEF_MAID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_VLAN_TAGS, DBAL_FIELD_TYPE_DEF_NOF_VLAN_TAGS, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, DBAL_FIELD_TYPE_DEF_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_EN, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_VAL, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_BASE, DBAL_FIELD_TYPE_DEF_COUNTER_BASE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_INTERFACE, DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GLOB_OUT_LIF_JR1, DBAL_FIELD_TYPE_DEF_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LMM_DA_LSB_PROFILE, DBAL_FIELD_TYPE_DEF_LMM_DA_LSB_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_MAC_LSB, DBAL_FIELD_TYPE_DEF_L2_MAC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_GEN_PROFILE, DBAL_FIELD_TYPE_DEF_SA_GEN_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DA_UC_MC_TYPE, DBAL_FIELD_TYPE_DEF_DA_UC_MC_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSI, DBAL_FIELD_TYPE_DEF_VSI, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_VSI_JR1, DBAL_FIELD_TYPE_DEF_VSI, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_oam.oamp.mep_db_jr1_mode_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IVEC_ADDRESS_FACTOR, DBAL_FIELD_TYPE_DEF_IVEC_ADDRESS_FACTOR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_oam.oamp.mep_db_jr1_mode_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "ETH_OAM_DOWN_MEP_EGRESS_INJECTION", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 33);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PP_PORT, DBAL_FIELD_TYPE_DEF_PP_PORT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_CORE, DBAL_FIELD_TYPE_DEF_PORT_CORE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_device.general.nof_cores_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MDL, DBAL_FIELD_TYPE_DEF_MDL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAID, DBAL_FIELD_TYPE_DEF_MAID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_VLAN_TAGS, DBAL_FIELD_TYPE_DEF_NOF_VLAN_TAGS, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, DBAL_FIELD_TYPE_DEF_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_EN, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_VAL, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_BASE, DBAL_FIELD_TYPE_DEF_COUNTER_BASE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_INTERFACE, DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LMM_DA_LSB_PROFILE, DBAL_FIELD_TYPE_DEF_LMM_DA_LSB_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_MAC_LSB, DBAL_FIELD_TYPE_DEF_L2_MAC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_GEN_PROFILE, DBAL_FIELD_TYPE_DEF_SA_GEN_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DA_UC_MC_TYPE, DBAL_FIELD_TYPE_DEF_DA_UC_MC_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "ETH_OAM_UP_MEP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 32);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PP_PORT, DBAL_FIELD_TYPE_DEF_PP_PORT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_CORE, DBAL_FIELD_TYPE_DEF_PORT_CORE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_device.general.nof_cores_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MDL, DBAL_FIELD_TYPE_DEF_MDL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAID, DBAL_FIELD_TYPE_DEF_MAID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INNER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_VID, DBAL_FIELD_TYPE_DEF_VLAN_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OUTER_DEI_PCP, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 4;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_NOF_VLAN_TAGS, DBAL_FIELD_TYPE_DEF_NOF_VLAN_TAGS, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, DBAL_FIELD_TYPE_DEF_UP_PTCH_OPAQUE_PT_ATTR_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_EN, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_VAL, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LMM_DA_LSB_PROFILE, DBAL_FIELD_TYPE_DEF_LMM_DA_LSB_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_MAC_LSB, DBAL_FIELD_TYPE_DEF_L2_MAC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_GEN_PROFILE, DBAL_FIELD_TYPE_DEF_SA_GEN_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DA_UC_MC_TYPE, DBAL_FIELD_TYPE_DEF_DA_UC_MC_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SERVER_DESTINATION, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "ETH_OAM_UP_MEP_SERVER", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_MPLSTP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 25);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYSTEM_PORT, DBAL_FIELD_TYPE_DEF_SYSTEM_PORT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MDL, DBAL_FIELD_TYPE_DEF_MDL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAID, DBAL_FIELD_TYPE_DEF_MAID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_TYPE_DEF_MPLS_LABEL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_PUSH_PROFILE, DBAL_FIELD_TYPE_DEF_MPLS_PUSH_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_EN, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_VAL, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_BASE, DBAL_FIELD_TYPE_DEF_COUNTER_BASE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_INTERFACE, DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SIGNAL_DEGRADATION, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "Y1731_MPLSTP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_PWE;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 25);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYSTEM_PORT, DBAL_FIELD_TYPE_DEF_SYSTEM_PORT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MDL, DBAL_FIELD_TYPE_DEF_MDL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAID, DBAL_FIELD_TYPE_DEF_MAID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_TYPE_DEF_MPLS_LABEL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_PUSH_PROFILE, DBAL_FIELD_TYPE_DEF_MPLS_PUSH_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_EN, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_STATUS_TLV_VAL, DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_INTERFACE_STATUS_TLV_CODE, DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_BASE, DBAL_FIELD_TYPE_DEF_COUNTER_BASE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_INTERFACE, DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 9;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SIGNAL_DEGRADATION, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "Y1731_PWE", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_ONE_HOP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 24);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TX_RATE_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_YOUR_DISC, DBAL_FIELD_TYPE_DEF_YOUR_DISC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_SRC_IP_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_SRC_IP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DETECT_MULT, DBAL_FIELD_TYPE_DEF_DETECT_MULT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_DIAG_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_DIAG_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TUNNEL_MPLS_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_FLAGS_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_FLAGS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_STATE, DBAL_FIELD_TYPE_DEF_BFD_STATE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_DIP, DBAL_FIELD_TYPE_DEF_IPV4, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IP_SUBNET_LEN, DBAL_FIELD_TYPE_DEF_IP_SUBNET_LEN, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MICRO_BFD, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_DISCR_MSB, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_DISCR_LSB, DBAL_FIELD_TYPE_DEF_BFD_LOCAL_DISCR_LSB, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BFD_IPV4_ONE_HOP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_MULTI_HOP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 23);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TX_RATE_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_YOUR_DISC, DBAL_FIELD_TYPE_DEF_YOUR_DISC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IPV4_DIP, DBAL_FIELD_TYPE_DEF_IPV4, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_SRC_IP_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_SRC_IP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IP_TTL_TOS_PROFILE, DBAL_FIELD_TYPE_DEF_IP_TTL_TOS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DETECT_MULT, DBAL_FIELD_TYPE_DEF_DETECT_MULT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_DIAG_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_DIAG_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_FLAGS_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_FLAGS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_STATE, DBAL_FIELD_TYPE_DEF_BFD_STATE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_DISCR_MSB, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_SEAMLESS_PORTS, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_DISCR_LSB, DBAL_FIELD_TYPE_DEF_BFD_LOCAL_DISCR_LSB, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BFD_IPV4_MULTI_HOP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_MPLS;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 24);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TX_RATE_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_YOUR_DISC, DBAL_FIELD_TYPE_DEF_YOUR_DISC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_SRC_IP_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_SRC_IP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DETECT_MULT, DBAL_FIELD_TYPE_DEF_DETECT_MULT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_DIAG_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_DIAG_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_TYPE_DEF_MPLS_LABEL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_PUSH_PROFILE, DBAL_FIELD_TYPE_DEF_MPLS_PUSH_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_IP_TTL_TOS_PROFILE, DBAL_FIELD_TYPE_DEF_IP_TTL_TOS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_FLAGS_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_FLAGS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_STATE, DBAL_FIELD_TYPE_DEF_BFD_STATE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_DISCR_MSB, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_SEAMLESS_PORTS, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_DISCR_LSB, DBAL_FIELD_TYPE_DEF_BFD_LOCAL_DISCR_LSB, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BFD_MPLS", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_PWE;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 26);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TX_RATE_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYSTEM_PORT, DBAL_FIELD_TYPE_DEF_SYSTEM_PORT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_YOUR_DISC, DBAL_FIELD_TYPE_DEF_YOUR_DISC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DETECT_MULT, DBAL_FIELD_TYPE_DEF_DETECT_MULT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_TX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_RX_INTERVAL_PROFILE, DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF, DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_DIAG_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_DIAG_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_LABEL, DBAL_FIELD_TYPE_DEF_MPLS_LABEL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_PUSH_PROFILE, DBAL_FIELD_TYPE_DEF_MPLS_PUSH_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_FLAGS_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_FLAGS_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_STATE, DBAL_FIELD_TYPE_DEF_BFD_STATE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACH, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_GAL, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ROUTER_ALERT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACH_SEL, DBAL_FIELD_TYPE_DEF_ACH_SEL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_FLEX_LM_DM_PTR, DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SET_DISCR_MSB, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_DISCR_LSB, DBAL_FIELD_TYPE_DEF_BFD_LOCAL_DISCR_LSB, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "BFD_PWE", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_RFC_6374;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 9);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_BASE, DBAL_FIELD_TYPE_DEF_COUNTER_BASE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_COUNTER_INTERFACE, DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TIMESTAMP_FORMAT, DBAL_FIELD_TYPE_DEF_TIMESTAMP_FORMAT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SESSION_IDENTIFIER, DBAL_FIELD_TYPE_DEF_SESSION_IDENTIFIER, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DM_T_FLAG, DBAL_FIELD_TYPE_DEF_DM_T_FLAG, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DM_DS_FIELD, DBAL_FIELD_TYPE_DEF_DM_DS_FIELD, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_CW_CHOOSE, DBAL_FIELD_TYPE_DEF_LM_CW_CHOOSE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENTRY, DBAL_FIELD_TYPE_DEF_LAST_ENTRY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "RFC_6374", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_MY_TX, DBAL_FIELD_TYPE_DEF_LM_MY_TX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_MY_RX, DBAL_FIELD_TYPE_DEF_LM_MY_RX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_PEER_TX, DBAL_FIELD_TYPE_DEF_LM_PEER_TX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_PEER_RX, DBAL_FIELD_TYPE_DEF_LM_PEER_RX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENTRY, DBAL_FIELD_TYPE_DEF_LAST_ENTRY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_CNTRS_VALID, DBAL_FIELD_TYPE_DEF_LM_CNTRS_VALID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR, DBAL_FIELD_TYPE_DEF_MEASURE_NEXT_RECEIVED_SLR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "LM_DB", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_STAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_LM_FAR, DBAL_FIELD_TYPE_DEF_LAST_LM_FAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_LM_NEAR, DBAL_FIELD_TYPE_DEF_LAST_LM_NEAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACC_LM_FAR, DBAL_FIELD_TYPE_DEF_ACC_LM_FAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACC_LM_NEAR, DBAL_FIELD_TYPE_DEF_ACC_LM_NEAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_LM_FAR, DBAL_FIELD_TYPE_DEF_MAX_LM_FAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_LM_NEAR, DBAL_FIELD_TYPE_DEF_MAX_LM_NEAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENTRY, DBAL_FIELD_TYPE_DEF_LAST_ENTRY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "LM_STAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_TWO_WAY;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 5);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_DELAY, DBAL_FIELD_TYPE_DEF_FULL_DELAY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_DELAY, DBAL_FIELD_TYPE_DEF_FULL_DELAY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_DELAY, DBAL_FIELD_TYPE_DEF_FULL_DELAY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENTRY, DBAL_FIELD_TYPE_DEF_LAST_ENTRY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "DM_STAT_TWO_WAY", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_HEADER;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXT_DATA_LENGTH, DBAL_FIELD_TYPE_DEF_EXT_DATA_LENGTH, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OPCODES_TO_PREPEND, DBAL_FIELD_TYPE_DEF_OPCODES_TO_PREPEND, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CHECK_CRC_VALUE_1, DBAL_FIELD_TYPE_DEF_CRC16, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CHECK_CRC_VALUE_2, DBAL_FIELD_TYPE_DEF_CRC16, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_SEGMENT, DBAL_FIELD_TYPE_DEF_EXTRA_DATA_SEGMENT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "EXTRA_DATA_HEADER", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_PAYLOAD;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 2);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXTRA_DATA_SEGMENT, DBAL_FIELD_TYPE_DEF_EXTRA_DATA_SEGMENT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "EXTRA_DATA_PAYLOAD", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_ONE_WAY;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 8);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_DELAY_ONE_WAY, DBAL_FIELD_TYPE_DEF_DELAY_DM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_DELAY_ONE_WAY, DBAL_FIELD_TYPE_DEF_DELAY_DM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_DELAY_ONE_WAY, DBAL_FIELD_TYPE_DEF_DELAY_DM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_DELAY_ONE_WAY_NEAR_END, DBAL_FIELD_TYPE_DEF_DELAY_DM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_DELAY_ONE_WAY_NEAR_END, DBAL_FIELD_TYPE_DEF_DELAY_DM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_DELAY_ONE_WAY_NEAR_END, DBAL_FIELD_TYPE_DEF_DELAY_DM, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_ENTRY, DBAL_FIELD_TYPE_DEF_LAST_ENTRY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "DM_STAT_ONE_WAY", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 20);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_OFFLOADED_MEP_TYPE_SUFFIX, DBAL_FIELD_TYPE_DEF_OFFLOADED_MEP_TYPE_SUFFIX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DM_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_STAT_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DM_STAT_ENABLE, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_MY_TX, DBAL_FIELD_TYPE_DEF_LM_MY_TX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_MY_RX, DBAL_FIELD_TYPE_DEF_LM_MY_RX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_PEER_TX, DBAL_FIELD_TYPE_DEF_LM_PEER_TX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_PEER_RX, DBAL_FIELD_TYPE_DEF_LM_PEER_RX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LM_CNTRS_VALID, DBAL_FIELD_TYPE_DEF_LM_CNTRS_VALID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_LM_FAR, DBAL_FIELD_TYPE_DEF_LAST_LM_FAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_LM_NEAR, DBAL_FIELD_TYPE_DEF_LAST_LM_NEAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_LM_FAR, DBAL_FIELD_TYPE_DEF_MAX_LM_FAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PART_2_PTR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 15;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR, DBAL_FIELD_TYPE_DEF_MEASURE_NEXT_RECEIVED_SLR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "LM_DM_OFFLOADED_PART_1", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_2;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 7);
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_LM_NEAR, DBAL_FIELD_TYPE_DEF_MAX_LM_NEAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACC_LM_FAR, DBAL_FIELD_TYPE_DEF_ACC_LM_FAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ACC_LM_NEAR, DBAL_FIELD_TYPE_DEF_ACC_LM_NEAR, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LAST_DELAY, DBAL_FIELD_TYPE_DEF_FULL_DELAY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAX_DELAY, DBAL_FIELD_TYPE_DEF_FULL_DELAY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MIN_DELAY, DBAL_FIELD_TYPE_DEF_FULL_DELAY, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "LM_DM_OFFLOADED_PART_2", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 11 + (((!dnx_data_oam.oamp.modified_mep_db_structure_get(unit))>0)?1:0) + (((dnx_data_oam.oamp.modified_mep_db_structure_get(unit))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX, DBAL_FIELD_TYPE_DEF_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_oam.oamp.modified_mep_db_structure_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_oam.oamp.modified_mep_db_structure_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.modified_mep_db_structure_get(unit);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "Q_ENTRY_OAM_FORMAT_DOWN_MEP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 12 + (((!dnx_data_oam.oamp.modified_mep_db_structure_get(unit))>0)?1:0) + (((dnx_data_oam.oamp.modified_mep_db_structure_get(unit))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX, DBAL_FIELD_TYPE_DEF_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_oam.oamp.modified_mep_db_structure_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_oam.oamp.modified_mep_db_structure_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CCM_INTERVAL, DBAL_FIELD_TYPE_DEF_CCM_INTERVAL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.modified_mep_db_structure_get(unit);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PP_PORT, DBAL_FIELD_TYPE_DEF_PP_PORT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PORT_CORE, DBAL_FIELD_TYPE_DEF_PORT_CORE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.max_value = dnx_data_device.general.nof_cores_get(unit)-1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ICC_MAP_INDEX, DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_ID, DBAL_FIELD_TYPE_DEF_MEP_ID, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_SCANNER, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RDI_FROM_PACKET, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_CRPS_CORE_SELECT, DBAL_FIELD_TYPE_DEF_BOOL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "Q_ENTRY_OAM_FORMAT_UP_MEP", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
        result_type_counter = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT;
        field_index = 0;
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 6 + (((!dnx_data_oam.oamp.modified_mep_db_structure_get(unit))>0)?1:0) + (((dnx_data_oam.oamp.modified_mep_db_structure_get(unit))>0)?1:0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_TYPE_DEF_RESULT_TYPE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 8;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX, DBAL_FIELD_TYPE_DEF_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.min_value = 1;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (!dnx_data_oam.oamp.modified_mep_db_structure_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TX_RATE_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = !dnx_data_oam.oamp.modified_mep_db_structure_get(unit);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_oam.oamp.modified_mep_db_structure_get(unit) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_BFD_TX_RATE_PROFILE, DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_oam.oamp.modified_mep_db_structure_get(unit);
            db_field.size = 2;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_YOUR_DISC_LSB, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 26;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MEP_PE_PROFILE, DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        sal_strncpy(table_entry->multi_res_info[result_type_counter].result_type_name, "Q_ENTRY_BFD_FORMAT", sizeof(table_entry->multi_res_info[result_type_counter].result_type_name));
    }
    dbal_db_init_table_db_struct_access_clear(cur_table_param, table_entry);
    table_entry->core_mode = DBAL_CORE_MODE_SBC;
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP], 0)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP_EGRESS_INJECTION], 128)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP], 32)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER], 160)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_MPLSTP], 1)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_MPLSTP], 33)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_PWE], 2)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_PWE], 34)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_ONE_HOP], 3)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_ONE_HOP], 35)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_MULTI_HOP], 4)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_MULTI_HOP], 36)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_MPLS], 5)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_MPLS], 37)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_PWE], 6)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_PWE], 38)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_RFC_6374], 7)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_RFC_6374], 39)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB], 8)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB], 40)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_STAT], 9)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_STAT], 41)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_TWO_WAY], 10)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_TWO_WAY], 42)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_HEADER], 11)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_HEADER], 43)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_PAYLOAD], 12)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_PAYLOAD], 44)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_ONE_WAY], 13)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_ONE_WAY], 45)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 16)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 17)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 18)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 22)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 48)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 49)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 50)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1], 54)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_2], 95)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_2], 127)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP], 64)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP], 65)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP], 66)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP], 96)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP], 97)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP], 98)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 67)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 68)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 69)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 70)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 99)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 100)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 101)
    DBAL_DB_INIT_RESULT_TYPE_HW_VALUE_SET(table_entry->multi_res_info[DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT], 102)
    {
        int map_idx = 0;
        int access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "ETH_OAM_DOWN_MEP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 5;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_1_DOWN_0f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MDL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MDLf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MAIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_INNER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_OUTER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_NOF_VLAN_TAGS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ENCAPSULATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_PTCH_OPAQUE_PT_ATTR_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_EN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_ENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_VAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_VALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INTERFACE_STATUS_TLV_CODE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INTERFACE_STATUS_TLV_CODEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, COUNTER_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, COUNTER_INTERFACEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LMM_DA_LSB_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LMM_DA_NIC_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_MAC_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_GEN_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        uint32 result_size__sa_mac_lsb = 0;
                        result_size__sa_mac_lsb += 15;
                        access_params->data_offset.formula_int = result_size__sa_mac_lsb;
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
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DA_UC_MC_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DA_MC_1_UC_0_SELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_DOWN_MEP_EGRESS_INJECTION;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "ETH_OAM_DOWN_MEP_EGRESS_INJECTION", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 5;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_1_DOWN_0f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MDL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MDLf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MAIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_INNER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_OUTER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_NOF_VLAN_TAGS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ENCAPSULATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_PTCH_OPAQUE_PT_ATTR_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_EN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_ENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_VAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_VALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INTERFACE_STATUS_TLV_CODE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INTERFACE_STATUS_TLV_CODEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, COUNTER_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, COUNTER_INTERFACEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 16;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 195;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 21;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 111;
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
                access_params->access_field_id = DBAL_FIELD_GLOB_OUT_LIF_JR1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GLOB_OUT_LIF_JR1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                access_params->access_offset = 16;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 196;
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
                access_params->access_field_id = DBAL_FIELD_LMM_DA_LSB_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LMM_DA_NIC_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_MAC_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_GEN_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        uint32 result_size__sa_mac_lsb = 0;
                        result_size__sa_mac_lsb += 15;
                        access_params->data_offset.formula_int = result_size__sa_mac_lsb;
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
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DA_UC_MC_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DA_MC_1_UC_0_SELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VSI;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 15;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 117;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VSI;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 3;
                access_params->access_offset = 15;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 112;
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
                access_params->access_field_id = DBAL_FIELD_VSI_JR1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 14;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, DBAL_HL_ACCESS_HW_ENTITY_INVALID, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_GROUP_SET(access_params, DBAL_HW_ENTITY_GROUP_OAMP_PE_GEN_MEM);
                    access_params->entry_offset.formula_cb = oamp_mep_db_vsi_jr1_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 2;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_VSI_JR1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                access_params->access_offset = 14;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 198;
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
                access_params->access_field_id = DBAL_FIELD_IVEC_ADDRESS_FACTOR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 2;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 115;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "ETH_OAM_UP_MEP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 5;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_1_DOWN_0f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PP_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_PORT_CORE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LOCAL_PORTf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                        access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                        {
                            access_params->data_offset.formula_int = 8;
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
                access_params->access_field_id = DBAL_FIELD_MDL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MDLf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MAIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_INNER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_OUTER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_NOF_VLAN_TAGS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ENCAPSULATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_PTCH_OPAQUE_PT_ATTR_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_EN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_ENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_VAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_VALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INTERFACE_STATUS_TLV_CODE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INTERFACE_STATUS_TLV_CODEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, COUNTER_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, COUNTER_INTERFACEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LMM_DA_LSB_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LMM_DA_NIC_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_MAC_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_GEN_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        uint32 result_size__sa_mac_lsb = 0;
                        result_size__sa_mac_lsb += 15;
                        access_params->data_offset.formula_int = result_size__sa_mac_lsb;
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
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DA_UC_MC_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DA_MC_1_UC_0_SELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_ETH_OAM_UP_MEP_SERVER;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "ETH_OAM_UP_MEP_SERVER", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 5;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_1_DOWN_0f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PP_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_PORT_CORE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LOCAL_PORTf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                        access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                        {
                            access_params->data_offset.formula_int = 8;
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
                access_params->access_field_id = DBAL_FIELD_MDL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MDLf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MAIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_INNER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INNER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INNER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_OUTER_TPID_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_TPIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_VID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OUTER_DEI_PCP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, OUTER_VID_DEI_PCPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_NOF_VLAN_TAGS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, ENCAPSULATIONf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_UP_PTCH_OPAQUE_PT_ATTR_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, UP_PTCH_OPAQUE_PT_ATTR_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_EN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_ENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_VAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, PORT_STATUS_TLV_VALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INTERFACE_STATUS_TLV_CODE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, INTERFACE_STATUS_TLV_CODEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LMM_DA_LSB_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, LMM_DA_NIC_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_MAC_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SA_GEN_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, SA_GEN_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        uint32 result_size__sa_mac_lsb = 0;
                        result_size__sa_mac_lsb += 15;
                        access_params->data_offset.formula_int = result_size__sa_mac_lsb;
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
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DA_UC_MC_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DA_MC_1_UC_0_SELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SERVER_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = dnx_data_oam.oamp.server_destination_lsb_bits_get(unit);
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 216-dnx_data_oam.oamp.server_destination_lsb_bits_get(unit);
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SERVER_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = dnx_data_oam.oamp.server_destination_msb_bits_get(unit);
                access_params->access_offset = dnx_data_oam.oamp.server_destination_lsb_bits_get(unit);
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 116;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SERVER_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16-dnx_data_oam.oamp.server_destination_lsb_bits_get(unit);
                access_params->access_offset = dnx_data_oam.oamp.server_destination_lsb_bits_get(unit)+dnx_data_oam.oamp.server_destination_msb_bits_get(unit);
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SERVER_DESTINATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16-dnx_data_oam.oamp.server_destination_msb_bits_get(unit);
                access_params->access_offset = 16+dnx_data_oam.oamp.server_destination_msb_bits_get(unit);
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_ETHm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 116+dnx_data_oam.oamp.server_destination_msb_bits_get(unit);
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_MPLSTP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "Y1731_MPLSTP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYSTEM_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, DEST_SYS_PORT_AGRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MDL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MDLf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MAIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, EEPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_LABEL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MPLS_LABELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_PUSH_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MPLS_PUSH_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_EN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, PORT_STATUS_TLV_ENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_VAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, PORT_STATUS_TLV_VALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INTERFACE_STATUS_TLV_CODE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, INTERFACE_STATUS_TLV_CODEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, COUNTER_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, COUNTER_INTERFACEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SIGNAL_DEGRADATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_MPLSTPm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 193;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_Y1731_PWE;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "Y1731_PWE", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYSTEM_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, DEST_SYS_PORT_AGRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MDL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, MDLf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, MAIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, EEPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_LABEL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, PWE_LABELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_PUSH_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, PWE_PUSH_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_EN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, PORT_STATUS_TLV_ENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PORT_STATUS_TLV_VAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, PORT_STATUS_TLV_VALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_INTERFACE_STATUS_TLV_CODE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, INTERFACE_STATUS_TLV_CODEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, COUNTER_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, COUNTER_INTERFACEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_TO_OAMP_PE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SIGNAL_DEGRADATION;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_Y_1731_ON_PWEm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 193;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_ONE_HOP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BFD_IPV4_ONE_HOP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_TX_RATE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, TX_RATEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_YOUR_DISC;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, YOUR_DISCf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_SRC_IP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, SRC_IP_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DETECT_MULT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, DETECT_MULTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, MIN_TX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_RX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, MIN_RX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, EEPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_DIAG_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, DIAG_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_TUNNEL_MPLS_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, TUNNEL_IS_MPLSf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_FLAGS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, FLAGS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_STATE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, STAf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_DIP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, DST_IPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IP_SUBNET_LEN;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, IP_SUBNET_LENf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MICRO_BFD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, MICRO_BFDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_DISCR_MSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 199;
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
                access_params->access_field_id = DBAL_FIELD_LOCAL_DISCR_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_IPV4_MULTI_HOP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BFD_IPV4_MULTI_HOP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_TX_RATE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, TX_RATEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_YOUR_DISC;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, YOUR_DISCf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IPV4_DIP;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, DST_IPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_SRC_IP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, SRC_IP_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IP_TTL_TOS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, IP_TTL_TOS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DETECT_MULT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, DETECT_MULTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, MIN_TX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_RX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, MIN_RX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, EEPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_DIAG_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, DIAG_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_FLAGS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, FLAGS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_STATE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, STAf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_DISCR_MSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 199;
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
                access_params->access_field_id = DBAL_FIELD_SET_SEAMLESS_PORTS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 198;
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
                access_params->access_field_id = DBAL_FIELD_LOCAL_DISCR_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_IPV4_MULTI_HOPm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_MPLS;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BFD_MPLS", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_TX_RATE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, TX_RATEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_YOUR_DISC;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, YOUR_DISCf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_SRC_IP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, SRC_IP_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DETECT_MULT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, DETECT_MULTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MIN_TX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_RX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MIN_RX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, EEPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_DIAG_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, DIAG_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_LABEL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MPLS_LABELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_PUSH_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MPLS_PUSH_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_IP_TTL_TOS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, IP_TTL_TOS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_FLAGS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, FLAGS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_STATE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, STAf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_DISCR_MSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 199;
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
                access_params->access_field_id = DBAL_FIELD_SET_SEAMLESS_PORTS;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 198;
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
                access_params->access_field_id = DBAL_FIELD_LOCAL_DISCR_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_MPLSm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_BFD_PWE;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "BFD_PWE", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_TX_RATE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, TX_RATEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SYSTEM_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, DEST_SYS_PORT_AGRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_YOUR_DISC;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, YOUR_DISCf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DETECT_MULT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, DETECT_MULTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_TX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, MIN_TX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_RX_INTERVAL_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, MIN_RX_INTERVAL_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FEC_ID_OR_GLOB_OUT_LIF;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, EEPf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_DIAG_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, DIAG_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_LABEL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, PWE_LABELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MPLS_PUSH_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, PWE_PUSH_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_FLAGS_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, FLAGS_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_STATE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, STAf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, ACHf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_GAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, GALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ROUTER_ALERT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, ROUTER_ALERTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACH_SEL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, ACH_SELf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, EXTRA_DATA_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_FLEX_LM_DM_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, FLEX_LM_DM_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SET_DISCR_MSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 199;
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
                access_params->access_field_id = DBAL_FIELD_LOCAL_DISCR_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 16;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_ON_PWEm, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 200;
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_RFC_6374;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "RFC_6374", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_BASE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, COUNTER_POINTERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_COUNTER_INTERFACE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, COUNTER_INTERFACEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_TIMESTAMP_FORMAT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, TIMESTAMP_FORMATf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SESSION_IDENTIFIER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, SESSION_IDENTIFIERf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DM_T_FLAG;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, DM_T_FLAGf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DM_DS_FIELD;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, DM_DS_FIELDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_CW_CHOOSE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, LM_CW_CHOOSEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_ENTRY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, LAST_ENTRYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DB;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "LM_DB", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_MY_TX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, MY_TXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_MY_RX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, MY_RXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_PEER_TX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, PEER_TXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_PEER_RX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, PEER_RXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_ENTRY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, LAST_ENTRYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_CNTRS_VALID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, LM_CNTRS_VALIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_DBm, MEASURE_NEXT_RECEIVED_SLRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_STAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "LM_STAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_LM_FAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, LAST_LM_FARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_LM_NEAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, LAST_LM_NEARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACC_LM_FAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, ACC_LM_FARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACC_LM_NEAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, ACC_LM_NEARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_LM_FAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, MAX_LM_FARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_LM_NEAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, MAX_LM_NEARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_ENTRY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_LM_STATm, LAST_ENTRYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_TWO_WAY;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "DM_STAT_TWO_WAY", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_DELAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_TWO_WAYm, LAST_DELAYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_DELAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MAX_DELAYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_DELAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_TWO_WAYm, MIN_DELAYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_ENTRY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_TWO_WAYm, LAST_ENTRYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_HEADER;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "EXTRA_DATA_HEADER", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_HDRm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXT_DATA_LENGTH;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_HDRm, EXT_DATA_LENGTHf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OPCODES_TO_PREPEND;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_HDRm, OPCODES_TO_PREPENDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CHECK_CRC_VALUE_1;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_HDRm, CHECK_CRC_VALUE_1f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CHECK_CRC_VALUE_2;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_HDRm, CHECK_CRC_VALUE_2f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_SEGMENT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_HDRm, VALUEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_EXTRA_DATA_PAYLOAD;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "EXTRA_DATA_PAYLOAD", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_PLDm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXTRA_DATA_SEGMENT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_EXT_DATA_PLDm, VALUEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_DM_STAT_ONE_WAY;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "DM_STAT_ONE_WAY", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_DELAY_ONE_WAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, LAST_DELAY_DMMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_DELAY_ONE_WAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, MAX_DELAY_DMMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_DELAY_ONE_WAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, MIN_DELAY_DMMf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_DELAY_ONE_WAY_NEAR_END;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, LAST_DELAY_DMRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_DELAY_ONE_WAY_NEAR_END;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, MAX_DELAY_DMRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_DELAY_ONE_WAY_NEAR_END;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, MIN_DELAY_DMRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_ENTRY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_DM_STAT_ONE_WAYm, LAST_ENTRYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_1;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "LM_DM_OFFLOADED_PART_1", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_OFFLOADED_MEP_TYPE_SUFFIX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, LM_ENABLEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DM_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, DM_ENABLEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_STAT_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, LM_STAT_ENABLEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_DM_STAT_ENABLE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, DM_STAT_ENABLEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    {
                        access_params->data_offset.formula_int = 1;
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
                access_params->access_field_id = DBAL_FIELD_LM_MY_TX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MY_TXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_MY_RX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MY_RXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_PEER_TX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, PEER_TXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_PEER_RX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, PEER_RXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LM_CNTRS_VALID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, LM_CNTRS_VALIDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_LM_FAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, LAST_LM_FARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_LM_NEAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, LAST_LM_NEARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_LM_FAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MAX_LM_FARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PART_2_PTR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, PART_2_PTRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEASURE_NEXT_RECEIVED_SLR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MEASURE_NEXT_RECEIVED_SLRf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_1m, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_LM_DM_OFFLOADED_PART_2;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "LM_DM_OFFLOADED_PART_2", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_LM_NEAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, MAX_LM_NEARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACC_LM_FAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, ACC_LM_FARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ACC_LM_NEAR;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, ACC_LM_NEARf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LAST_DELAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, LAST_DELAYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAX_DELAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, MAX_DELAYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MIN_DELAY;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_OFFLOADED_FORMAT_PART_2m, MIN_DELAYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_DOWN_MEP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "Q_ENTRY_OAM_FORMAT_DOWN_MEP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 5;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, UP_1_DOWN_0f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_rdi_from_packet_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_OAM_FORMAT_UP_MEP;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "Q_ENTRY_OAM_FORMAT_UP_MEP", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 1;
                access_params->access_offset = 5;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, UP_1_DOWN_0f));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CCM_INTERVAL;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, CCM_INTERVALf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_PP_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
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
                    access_params->access_field_id = DBAL_FIELD_PORT_CORE;
                    access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, LOCAL_PORTf));
                        DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                        access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                        access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                        access_params->data_offset.formula_cb = oamp_mep_db_port_core_dataoffset_0_cb;
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
                access_params->access_field_id = DBAL_FIELD_ICC_MAP_INDEX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, ICC_INDEXf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_ID;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_IDf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_SCANNER;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RDI_FROM_PACKET;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, RDIf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_rdi_from_packet_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_CRPS_CORE_SELECT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_CCM_Y_1731_SHORT_FORMATm, RX_TX_COUNTERS_CRPS_CORE_SELECTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
        map_idx = DBAL_RESULT_TYPE_OAMP_MEP_DB_Q_ENTRY_BFD_FORMAT;
        access_counter = 0;
        sal_strncpy(cur_table_param->hl_access[map_idx].mapping_result_name, "Q_ENTRY_BFD_FORMAT", sizeof(cur_table_param->hl_access[map_idx].mapping_result_name));
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                access_params->access_size = 5;
                access_params->access_offset = 0;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_RESULT_TYPE;
                access_params->access_type = DBAL_HL_ACCESS_SW;
                access_params->access_size = 2;
                access_params->access_offset = 6;
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, MEP_TYPEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_BFD_TX_RATE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, TX_RATEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_ITMH_TC_DP_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, PRIORITYf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, LOCAL_PORTf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_YOUR_DISC_LSB;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, YOUR_DISC_LSBf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MEP_PE_PROFILE;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MEP_DB_BFD_SHORT_FORMATm, MEP_PE_PROFILEf));
                    DBAL_DB_INIT_ACCESS_PARAMS_ALIAS_SET(access_params, OAMP_MEP_DBm);
                    access_params->array_offset.formula_cb = oamp_mep_db_result_type_arrayoffset_0_cb;
                    access_params->entry_offset.formula_cb = oamp_mep_db_result_type_entryoffset_0_cb;
                    access_params->data_offset.formula_cb = oamp_mep_db_result_type_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_OAMP_MEP_DB, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_itmh_priority_profile_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, is_valid, maturity_level, "OAMP_ITMH_PRIORITY_PROFILE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_OAM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_ITMH_TC_DP_PROFILE, DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TC, DBAL_FIELD_TYPE_DEF_TC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_DP, DBAL_FIELD_TYPE_DEF_DP, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_TC;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_PR_2_FW_DTCr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = oamp_itmh_priority_profile_tc_dataoffset_0_cb;
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
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_PR_2_FWDDPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = oamp_itmh_priority_profile_dp_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_mpls_pwe_exp_ttl_profile_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, is_valid, maturity_level, "OAMP_MPLS_PWE_EXP_TTL_PROFILE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_OAM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MPLS_PUSH_PROFILE, DBAL_FIELD_TYPE_DEF_MPLS_PUSH_PROFILE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TTL, DBAL_FIELD_TYPE_DEF_TTL, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_EXP, DBAL_FIELD_TYPE_DEF_EXP, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_TTL;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MPLS_PWE_PROFILEr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = oamp_mpls_pwe_exp_ttl_profile_ttl_dataoffset_0_cb;
                }
            }
        }
        {
            table_db_access_params_struct_t * access_params;
            {
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_EXP;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_MPLS_PWE_PROFILEr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = oamp_mpls_pwe_exp_ttl_profile_exp_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_oam_sa_mac_msb_profile_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE, is_valid, maturity_level, "OAMP_OAM_SA_MAC_MSB_PROFILE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_OAM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SA_GEN_PROFILE, DBAL_FIELD_TYPE_DEF_SA_GEN_PROFILE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_MAC_MSB, DBAL_FIELD_TYPE_DEF_L2_MAC, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 40;
            db_field.offset = 8;
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
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAC_MSB;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SA_GEN_PROFILE, 0, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_CCM_MAC_SAr, BASE_MAC_SA_PROFILE_0f));
                }
            }
            {
                int condition_index = 0;
                access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                access_counter++;
                access_params->access_field_id = DBAL_FIELD_MAC_MSB;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_condition(unit, &access_params->access_condition[condition_index], DBAL_CONDITION_EQUAL_TO, DBAL_FIELD_SA_GEN_PROFILE, 1, (uint32) DBAL_DB_INVALID));
                condition_index++;
                access_params->nof_conditions = condition_index;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_CCM_MAC_SAr, BASE_MAC_SA_PROFILE_1f));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_local_port_2_system_port_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, is_valid, maturity_level, "OAMP_LOCAL_PORT_2_SYSTEM_PORT", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_OAM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_SYSTEM_PORT, DBAL_FIELD_TYPE_DEF_SYSTEM_PORT, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_SYSTEM_PORT;
                access_params->access_type = DBAL_HL_ACCESS_MEMORY;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_LOCAL_PORT_2_SYSTEM_PORTm, SYS_PORTf));
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_oam_tpid_profile_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_OAMP_OAM_TPID_PROFILE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    char * current_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, is_valid, maturity_level, "OAMP_OAM_TPID_PROFILE", DBAL_ACCESS_METHOD_HARD_LOGIC, current_image, &is_skip_table_init));
    if (is_skip_table_init)
    {
        SHR_EXIT();
    }
    table_entry->table_type = DBAL_TABLE_TYPE_DIRECT;
    table_entry->nof_labels = 1;
    DBAL_DB_INIT_TABLE_LABELS_ALLOC(table_entry)
    table_entry->table_labels[0] = DBAL_LABEL_OAM;
    {
        int field_index = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_INTERFACE_KEY_FIELDS_ALLOC(table_entry, 1);
        {
            dbal_db_field = &table_entry->keys_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPID_INDEX, DBAL_FIELD_TYPE_DEF_TPID_INDEX, dbal_db_field));
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
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_TPID, DBAL_FIELD_TYPE_DEF_TPID, dbal_db_field));
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
                access_params->access_field_id = DBAL_FIELD_TPID;
                access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, OAMP_CCM_TPID_MAPr, DBAL_HL_ACCESS_HW_FIELD_INVALID));
                    access_params->data_offset.formula_cb = oamp_oam_tpid_profile_tpid_dataoffset_0_cb;
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_mep_db_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_itmh_priority_profile_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_mpls_pwe_exp_ttl_profile_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_oam_sa_mac_msb_profile_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_local_port_2_system_port_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_pp_oam_hl_pp_oam_oamp_mep_db_oamp_oam_tpid_profile_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
