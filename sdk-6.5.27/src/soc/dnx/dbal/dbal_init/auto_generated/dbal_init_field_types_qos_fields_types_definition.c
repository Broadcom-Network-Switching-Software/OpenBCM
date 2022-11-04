
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_model_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_MODEL];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_MODEL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_MODEL_PIPE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_MODEL_UNIFORM, "UNIFORM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_MODEL_PIPE, "PIPE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_MODEL_UNIFORM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_MODEL_PIPE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_encap_qos_model_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ENCAP_QOS_MODEL];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ENCAP_QOS_MODEL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INIT_ECN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM, "UNIFORM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE, "PIPE_NEXT_SPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE, "PIPE_MY_SPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INITIAL, "INITIAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM_ECN, "UNIFORM_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN, "PIPE_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INIT_ECN, "INIT_ECN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INITIAL].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM_ECN].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INIT_ECN].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_forward_qos_model_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FORWARD_QOS_MODEL];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FORWARD_QOS_MODEL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_PIPE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_UNIFORM, "UNIFORM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_PIPE, "PIPE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_UNIFORM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_PIPE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_rch_qos_model_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RCH_QOS_MODEL];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RCH_QOS_MODEL", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_RCH_QOS_MODEL_UNIFORM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_QOS_MODEL_INITIAL, "INITIAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_QOS_MODEL_UNIFORM, "UNIFORM");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_RCH_QOS_MODEL_INITIAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_RCH_QOS_MODEL_UNIFORM].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_model_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MODEL_TYPE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MODEL_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MODEL_TYPE_PIPE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM, "UNIFORM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MODEL_TYPE_PIPE, "PIPE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MODEL_TYPE_PIPE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_PROFILE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_PROFILE", 10, is_valid, TRUE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_ecn_qos_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ECN_QOS_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_ecn_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_ECN];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_ECN", dnx_data_qos.qos.ecn_bits_width_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER, "TWO_TAGS_INNER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE, "TWO_TAGS_DOUBLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2, "L3_L2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL, "PWE_TAG_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER, "PWE_TAG_OUTER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER].value_from_mapping = 5;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_profile_name_space_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_PROFILE_NAME_SPACE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_PROFILE_NAME_SPACE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2, "L3_L2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE, "TAGGED_PWE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_network_qos_initial_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_QOS_INITIAL_SELECT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NETWORK_QOS_INITIAL_SELECT", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_INGRESS_NETWORK_QOS, "INGRESS_NETWORK_QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_NETWORK_QOS, "FORWARD_NETWORK_QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_PLUS_1_NETWORK_QOS, "FORWARD_PLUS_1_NETWORK_QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS, "TC_NETWORK_QOS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_INGRESS_NETWORK_QOS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_NETWORK_QOS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_PLUS_1_NETWORK_QOS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_dp_is_preserve_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DP_IS_PRESERVE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DP_IS_PRESERVE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_meter_dp_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "METER_DP_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_network_qos_is_preserve_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_QOS_IS_PRESERVE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NETWORK_QOS_IS_PRESERVE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_dp_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_DP_SELECT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_DP_SELECT", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE, "PIPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_DP_SELECT_UNIFORM, "UNIFORM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_DP_SELECT_INITIAL, "INITIAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP, "PIPE_DP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_UNIFORM].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_INITIAL].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_propag_strength_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_PROPAG_STRENGTH];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_PROPAG_STRENGTH", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_STRONG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW, "LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE, "MIDDLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH, "HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_STRONG, "STRONG");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_STRONG].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_cam_priority_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_CAM_PRIORITY];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_CAM_PRIORITY", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_int_map_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_INT_MAP_ID", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_int_map_key_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_INT_MAP_KEY", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_mask_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE_MASK];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_CS_PROFILE_MASK", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_ACTION];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_ACTION", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INDEX];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_ttl_inheritance_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TTL_INHERITANCE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TTL_INHERITANCE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TTL_INHERITANCE_UNIFORM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TTL_INHERITANCE_PIPE, "PIPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TTL_INHERITANCE_UNIFORM, "UNIFORM");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_TTL_INHERITANCE_PIPE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_TTL_INHERITANCE_UNIFORM].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_ttl_pipe_map_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TTL_PIPE_MAP];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TTL_PIPE_MAP", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_qos_resolution_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_RESOLUTION_MODE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_RESOLUTION_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT, "EXPLICIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED, "MAPPED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_int_map_key_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY_SIZE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_INT_MAP_KEY_SIZE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_ingress_qos_map_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INGRESS_QOS_MAP_INDEX];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INGRESS_QOS_MAP_INDEX", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_15 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_FIRST_TAG, "L2_FIRST_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_SECOND_TAG, "L2_SECOND_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS, "L2_TWO_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH, "RCH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_GTP, "GTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_9, "9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_10, "10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_11, "11");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_12, "12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_13, "13");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_14, "14");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_15, "15");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_FIRST_TAG].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_SECOND_TAG].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_GTP].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_9].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_10].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_11].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_12].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_13].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_14].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_15].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_ingress_nwk_qos_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INGRESS_NWK_QOS_TYPE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INGRESS_NWK_QOS_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER, "OTHER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV4].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV6].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_MPLS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_ingress_qos_network_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INGRESS_QOS_NETWORK_VALID];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INGRESS_QOS_NETWORK_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_ftmh_ecn_eligible_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FTMH_ECN_ELIGIBLE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FTMH_ECN_ELIGIBLE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_ENABLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_DISABLE, "DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_ENABLE, "ENABLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_DISABLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_ENABLE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_ecn_is_eligible_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ECN_IS_ELIGIBLE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ECN_IS_ELIGIBLE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE, "NOT_ELIGIBLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE, "ELIGIBLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_keep_ecn_bits_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KEEP_ECN_BITS];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "KEEP_ECN_BITS", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_KEEP_ECN_BITS_KEEP_ECN_BITS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KEEP_ECN_BITS_DONT_KEEP_ECN_BITS, "DONT_KEEP_ECN_BITS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_KEEP_ECN_BITS_KEEP_ECN_BITS, "KEEP_ECN_BITS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_KEEP_ECN_BITS_DONT_KEEP_ECN_BITS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_KEEP_ECN_BITS_KEEP_ECN_BITS].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_remark_or_preserve_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_REMARK_OR_PRESERVE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "REMARK_OR_PRESERVE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP, "KEEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK, "REMARK");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_ingress_app_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INGRESS_APP_TYPE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_INGRESS_APP_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB, "PHB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK, "REMARK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN, "ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL, "TTL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_layer_protocol_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_LAYER_PROTOCOL];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_LAYER_PROTOCOL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_OTHER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_INIT, "INIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_ETH, "ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_OTHER, "OTHER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_INIT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_ETH].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV6].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_MPLS].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_OTHER].value_from_mapping = 5;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_network_qos_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_QOS];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NETWORK_QOS", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_pcp_dei_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PCP_DEI];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PCP_DEI", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
_dbal_init_field_types_qos_fields_types_definition_network_qos_pipe_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_QOS_PIPE_PROFILE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NETWORK_QOS_PIPE_PROFILE", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
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
dbal_init_field_types_qos_fields_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_encap_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_forward_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_rch_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_model_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ecn_qos_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_ecn_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_profile_name_space_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_network_qos_initial_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_dp_is_preserve_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_meter_dp_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_network_qos_is_preserve_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_dp_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_propag_strength_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_cam_priority_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_int_map_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_int_map_key_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_mask_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ttl_inheritance_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ttl_pipe_map_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_resolution_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_int_map_key_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ingress_qos_map_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ingress_nwk_qos_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ingress_qos_network_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ftmh_ecn_eligible_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ecn_is_eligible_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_keep_ecn_bits_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_remark_or_preserve_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_ingress_app_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_layer_protocol_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_network_qos_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_pcp_dei_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_network_qos_pipe_profile_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
