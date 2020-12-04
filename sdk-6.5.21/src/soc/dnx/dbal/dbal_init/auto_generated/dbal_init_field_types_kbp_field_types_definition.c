
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_KEY_FIELD_TYPE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX].name_from_interface, "PREFIX", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_PREFIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM].name_from_interface, "EM", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_EM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE].name_from_interface, "RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_RANGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY].name_from_interface, "TERNARY", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_TERNARY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE].name_from_interface, "HOLE", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KEY_FIELD_TYPE_HOLE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ALGO_MODE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 5;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_NON_ALGO].name_from_interface, "NON_ALGO", sizeof(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_NON_ALGO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL].name_from_interface, "POWER_CTRL", sizeof(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL_ADVANCE].name_from_interface, "POWER_CTRL_ADVANCE", sizeof(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_POWER_CTRL_ADVANCE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_NET_ACL].name_from_interface, "NET_ACL", sizeof(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_NET_ACL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_ALGO_MODE_DEFAULT].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_RESULT_LOCATION" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->define, 0x0, 2 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "FWD", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 0;
    sal_strncpy(cur_field_type_param->define[1].define_name, "RPF", sizeof(cur_field_type_param->define[1].define_name));
    cur_field_type_param->define[1].define_value = 1;
    cur_field_type_param->nof_defined_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_DEVICE_STATUS" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT].name_from_interface, "BEFORE_INIT", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE].name_from_interface, "FWD_CONFIG_DONE", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE].name_from_interface, "INIT_DONE", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED].name_from_interface, "CONFIG_UPDATED", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_SYNC_IN_PROCESS].name_from_interface, "SYNC_IN_PROCESS", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_SYNC_IN_PROCESS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_WB_IN_PROCESS].name_from_interface, "WB_IN_PROCESS", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_WB_IN_PROCESS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED].name_from_interface, "LOCKED", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_KBR_IDX" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0].name_from_interface, "KEY_0", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1].name_from_interface, "KEY_1", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2].name_from_interface, "KEY_2", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_3].name_from_interface, "KEY_3", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_3].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_LOOKUP_TYPE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE].name_from_interface, "FREE", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FREE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD].name_from_interface, "FWD", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF].name_from_interface, "RPF", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC].name_from_interface, "FWD_PUBLIC", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_FWD_PUBLIC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC].name_from_interface, "RPF_PUBLIC", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_RPF_PUBLIC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL].name_from_interface, "ACL", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_LOOKUP_TYPE_ACL].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_DEVICE_MODE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 5;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE].name_from_interface, "SINGLE", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SHARED].name_from_interface, "DUAL_SHARED", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SHARED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SMT].name_from_interface, "DUAL_SMT", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_DUAL_SMT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT].name_from_interface, "SINGLE_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT_SMT].name_from_interface, "SINGLE_PORT_SMT", sizeof(enum_info[DBAL_ENUM_FVAL_KBP_DEVICE_MODE_SINGLE_PORT_SMT].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KBP_OPCODE_ID" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = 201;
    
    cur_field_type_param->min_value = 1;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
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
