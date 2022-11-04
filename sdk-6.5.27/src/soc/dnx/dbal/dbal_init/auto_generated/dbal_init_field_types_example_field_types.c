
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_example_field_types_example_field_with_illegal_values_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXAMPLE_FIELD_WITH_ILLEGAL_VALUES];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXAMPLE_FIELD_WITH_ILLEGAL_VALUES", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = 25;
    cur_field_type_param->min_value = 4;
    cur_field_type_param->illegal_values[0] = 12;
    cur_field_type_param->illegal_values[1] = 13;
    cur_field_type_param->illegal_values[2] = 22;
    cur_field_type_param->nof_illegal_value = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_example_field_types_example_field_with_illegal_values_cb_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXAMPLE_FIELD_WITH_ILLEGAL_VALUES_CB];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXAMPLE_FIELD_WITH_ILLEGAL_VALUES_CB", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_example_field_types_dummy_field_type1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DUMMY_FIELD_TYPE1];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DUMMY_FIELD_TYPE1", 100, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_example_field_types_example_general_uint_field_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXAMPLE_GENERAL_UINT_FIELD];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXAMPLE_GENERAL_UINT_FIELD", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_example_field_types_example_duplicated_enum_field_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE];
    int is_valid = dnx_data_tunnel.tunnel_type.udp_tunnel_type_get(unit);
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    if (!is_valid)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_E + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_A, "A");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_B, "B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_C, "C");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_A].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_B].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_C].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_example_field_types_example_duplicated_enum_field_type_1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE];
    int is_valid = 1;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_E + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_B, "B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_E, "E");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_B].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EXAMPLE_DUPLICATED_ENUM_FIELD_TYPE_E].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_example_field_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_example_field_types_example_field_with_illegal_values_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_example_field_types_example_field_with_illegal_values_cb_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_example_field_types_dummy_field_type1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_example_field_types_example_general_uint_field_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_example_field_types_example_duplicated_enum_field_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_example_field_types_example_duplicated_enum_field_type_1_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
