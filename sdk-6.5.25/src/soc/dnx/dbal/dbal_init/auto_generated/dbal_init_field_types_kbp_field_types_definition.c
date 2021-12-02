
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_key_field_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_KEY_FIELD_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_KEY_FIELD_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX, "PREFIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM, "EM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE, "RANGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY, "TERNARY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE, "HOLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE].value_from_mapping = 5;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_algo_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ALGO_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ALGO_MODE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALGO_MODE_NON_ALGO, "NON_ALGO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL, "POWER_CTRL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL_ADVANCE, "POWER_CTRL_ADVANCE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALGO_MODE_NET_ACL, "NET_ACL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT, "DEFAULT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ALGO_MODE_NON_ALGO].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL_ADVANCE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ALGO_MODE_NET_ACL].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_result_location_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_RESULT_LOCATION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_RESULT_LOCATION", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    sal_memset(cur_field_type_param->define, 0x0, 2 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "FWD", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 0;
    sal_strncpy(cur_field_type_param->define[1].define_name, "RPF", sizeof(cur_field_type_param->define[1].define_name));
    cur_field_type_param->define[1].define_value = 1;
    cur_field_type_param->nof_defined_vals = 2;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_device_status_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_DEVICE_STATUS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_DEVICE_STATUS", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT, "BEFORE_INIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE, "FWD_CONFIG_DONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE, "INIT_DONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED, "CONFIG_UPDATED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_SYNC_IN_PROCESS, "SYNC_IN_PROCESS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_WB_IN_PROCESS, "WB_IN_PROCESS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED, "LOCKED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_SYNC_IN_PROCESS].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_WB_IN_PROCESS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_kbr_idx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_KBR_IDX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_KBR_IDX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0, "KEY_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1, "KEY_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2, "KEY_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_3, "KEY_3");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_3].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_lookup_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_LOOKUP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_LOOKUP_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE, "FREE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD, "FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF, "RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC, "FWD_PUBLIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC, "RPF_PUBLIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL, "ACL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL].value_from_mapping = 5;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_device_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_DEVICE_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_DEVICE_MODE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT_SMT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE, "SINGLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SHARED, "DUAL_SHARED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SMT, "DUAL_SMT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT, "SINGLE_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT_SMT, "SINGLE_PORT_SMT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SHARED].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SMT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT_SMT].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kbp_field_types_definition_kbp_opcode_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KBP_OPCODE_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KBP_OPCODE_ID", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = 201;
    cur_field_type_param->min_value = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_kbp_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_key_field_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_algo_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_result_location_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_device_status_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_kbr_idx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_lookup_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_device_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kbp_field_types_definition_kbp_opcode_id_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
