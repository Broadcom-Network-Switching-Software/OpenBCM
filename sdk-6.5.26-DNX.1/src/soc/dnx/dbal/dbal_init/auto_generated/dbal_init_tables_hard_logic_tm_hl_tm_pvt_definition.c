
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_pvt_mon_enable_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PVT_MON_ENABLE;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    uint8 is_compatible_with_all_images = TRUE;
    char * custom_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_PVT_MON_ENABLE, is_valid, maturity_level, "PVT_MON_ENABLE", DBAL_ACCESS_METHOD_HARD_LOGIC, custom_image, is_compatible_with_all_images, &is_skip_table_init));
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
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_pvt.general.nof_pvt_monitors_get(unit) + dnx_data_pvt.general.nof_pvt_monitors_get(unit) + dnx_data_pvt.general.nof_pvt_monitors_get(unit) + dnx_data_pvt.general.nof_pvt_monitors_get(unit) + dnx_data_pvt.general.nof_pvt_monitors_get(unit) + (((dnx_data_pvt.general.feature_get(unit, dnx_data_pvt_general_pvt_peak_clear_support))>0)?dnx_data_pvt.general.nof_pvt_monitors_get(unit):0));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_MON_CONTROL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_MON_CONTROL_RESET, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_MON_CONTROL_SEL, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_pvt.general.pvt_mon_control_sel_nof_bits_get(unit);
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        if (dnx_data_pvt.general.feature_get(unit, dnx_data_pvt_general_pvt_peak_clear_support) != 0)
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.valid = dnx_data_pvt.general.feature_get(unit, dnx_data_pvt_general_pvt_peak_clear_support);
            db_field.size = 1;
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
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
        if (DBAL_IS_J2C_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
                        {
                            access_params->data_offset.formula_int = 0;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
                        {
                            access_params->data_offset.formula_int = 32;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
                        {
                            access_params->data_offset.formula_int = 33;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
                        {
                            access_params->data_offset.formula_int = 34;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
                        {
                            access_params->data_offset.formula_int = 35;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Ef));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Ff));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Gf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_PEAK_THERMAL_DATA_CLEARf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_PEAK_THERMAL_DATA_CLEARf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 0;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 32;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_RESET;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 33;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 34;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_SEL;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 35;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Ef));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Ff));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, PEAK_THERMAL_DATA_CLEAR_Gf));
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PVT_MON_ENABLE, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_pvt_temperature_monitor_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PVT_TEMPERATURE_MONITOR;
    int is_valid = TRUE;
    uint8 is_skip_table_init = FALSE;
    uint8 is_compatible_with_all_images = TRUE;
    char * custom_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_HIGH;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_PVT_TEMPERATURE_MONITOR, is_valid, maturity_level, "PVT_TEMPERATURE_MONITOR", DBAL_ACCESS_METHOD_HARD_LOGIC, custom_image, is_compatible_with_all_images, &is_skip_table_init));
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
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_pvt.general.nof_pvt_monitors_get(unit) + dnx_data_pvt.general.nof_pvt_monitors_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_THERMAL_DATA, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_pvt.general.thermal_data_nof_bits_get(unit);
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
            db_field.permission = DBAL_PERMISSION_READONLY;
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_optional_or_default_values(unit, table_entry, dbal_db_field, &db_field, FALSE));
            field_index += dbal_db_field->nof_instances;
        }
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PEAK_THERMAL_DATA, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = dnx_data_pvt.general.thermal_data_nof_bits_get(unit);
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
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
        if (DBAL_IS_J2C_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, THERMAL_DATA_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_THERMAL_DATAr, THERMAL_DATA_Ef));
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
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, PEAK_THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, PEAK_THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, PEAK_THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, PEAK_THERMAL_DATA_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_THERMAL_DATAr, PEAK_THERMAL_DATA_Ef));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, THERMAL_DATA_Df));
                    }
                }
            }
        }
        if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, PEAK_THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, PEAK_THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, PEAK_THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, PEAK_THERMAL_DATA_Df));
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
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, THERMAL_DATA_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_THERMAL_DATAr, THERMAL_DATA_Ef));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_THERMAL_DATAr, THERMAL_DATA_Ff));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_THERMAL_DATAr, THERMAL_DATA_Gf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_THERMAL_DATAr, PVT_MON_THERMAL_DATAf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_THERMAL_DATAr, PVT_MON_THERMAL_DATAf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, PEAK_THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, PEAK_THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, PEAK_THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, PEAK_THERMAL_DATA_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_THERMAL_DATAr, PEAK_THERMAL_DATA_Ef));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_THERMAL_DATAr, PEAK_THERMAL_DATA_Ff));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_THERMAL_DATAr, PEAK_THERMAL_DATA_Gf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_THERMAL_DATAr, PVT_MON_PEAK_THERMAL_DATAf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_THERMAL_DATAr, PVT_MON_PEAK_THERMAL_DATAf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, THERMAL_DATA_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_THERMAL_DATAr, THERMAL_DATA_Ef));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_THERMAL_DATAr, THERMAL_DATA_Ff));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_THERMAL_DATAr, THERMAL_DATA_Gf));
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
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_THERMAL_DATAr, PEAK_THERMAL_DATA_Af));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_THERMAL_DATAr, PEAK_THERMAL_DATA_Bf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_THERMAL_DATAr, PEAK_THERMAL_DATA_Cf));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_THERMAL_DATAr, PEAK_THERMAL_DATA_Df));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_THERMAL_DATAr, PEAK_THERMAL_DATA_Ef));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_THERMAL_DATAr, PEAK_THERMAL_DATA_Ff));
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PEAK_THERMAL_DATA;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_THERMAL_DATAr, PEAK_THERMAL_DATA_Gf));
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PVT_TEMPERATURE_MONITOR, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_pvt_mon_lock_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    dbal_logical_table_t * table_entry = table_info + DBAL_TABLE_PVT_MON_LOCK;
    int is_valid = dnx_data_pvt.general.feature_get(unit, dnx_data_pvt_general_pvt_lock_support);
    uint8 is_skip_table_init = FALSE;
    uint8 is_compatible_with_all_images = TRUE;
    char * custom_image = NULL;
    dbal_maturity_level_e maturity_level;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_table_db_struct_clear(cur_table_param);
    maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
    SHR_IF_ERR_EXIT(dbal_db_init_table_general_info_and_image_handle(unit, table_entry, DBAL_TABLE_PVT_MON_LOCK, is_valid, maturity_level, "PVT_MON_LOCK", DBAL_ACCESS_METHOD_HARD_LOGIC, custom_image, is_compatible_with_all_images, &is_skip_table_init));
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
        int result_type_counter = 0;
        dbal_table_field_info_t * dbal_db_field;
        table_db_field_params_struct_t db_field;
        DBAL_DB_INIT_TABLE_NOF_RESULT_TYPES_SET(table_entry, 1);
        SHR_IF_ERR_EXIT(dbal_db_init_table_db_interface_results_alloc(unit, table_entry));
        DBAL_DB_INIT_TABLE_NOF_RESULT_FIELDS_SET(table_entry, (table_entry->multi_res_info[result_type_counter]), 0 + dnx_data_pvt.general.nof_pvt_monitors_get(unit));
        DBAL_DB_INIT_TABLE_INTERFACE_RESULT_FIELDS_ALLOC(table_entry, table_entry->multi_res_info[result_type_counter]);
        {
            dbal_db_field = &table_entry->multi_res_info[result_type_counter].results_info[field_index];
            SHR_IF_ERR_EXIT(dbal_db_init_table_field_set_mandatory_values(unit, DBAL_FIELD_PVT_LOCK, DBAL_FIELD_TYPE_DEF_UINT, dbal_db_field));
            dbal_db_init_table_field_params_init(&db_field);
            db_field.size = 1;
            db_field.nof_instances = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
            db_field.is_validate_nof_instances = TRUE;
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
        if (DBAL_IS_J2P_A0)
        {
            {
                table_db_access_params_struct_t * access_params;
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 7;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 0;
                        }
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 8;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, HBMC_PVT_MON_CONTROL_REGr, PVT_MON_CTRLf));
                        {
                            access_params->block_index.formula_int = 1;
                        }
                        {
                            access_params->data_offset.formula_int = 31;
                        }
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
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_A_CONTROL_REGr, CTRL_Af));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 1;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_B_CONTROL_REGr, CTRL_Bf));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 2;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_C_CONTROL_REGr, CTRL_Cf));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 3;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_D_CONTROL_REGr, CTRL_Df));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 4;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_E_CONTROL_REGr, CTRL_Ef));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 5;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_F_CONTROL_REGr, CTRL_Ff));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
                {
                    access_params = &cur_table_param->hl_access[map_idx].access[access_counter];
                    dbal_db_init_table_db_struct_hl_access_params_clear(access_params);
                    access_counter++;
                    access_params->access_field_id = DBAL_FIELD_PVT_LOCK;
                    access_params->access_type = DBAL_HL_ACCESS_REGISTER;
                    access_params->inst_idx = 6;
                    {
                        SHR_IF_ERR_EXIT(dbal_db_init_table_set_access_register_memory(unit, access_params, ECI_PVT_MON_G_CONTROL_REGr, CTRL_Gf));
                        {
                            access_params->data_offset.formula_int = 31;
                        }
                    }
                }
            }
        }
        cur_table_param->hl_access[map_idx].nof_access = access_counter;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_table_add(unit, cur_table_param, DBAL_TABLE_PVT_MON_LOCK, table_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_pvt_mon_enable_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_pvt_temperature_monitor_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(_dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_pvt_mon_lock_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
