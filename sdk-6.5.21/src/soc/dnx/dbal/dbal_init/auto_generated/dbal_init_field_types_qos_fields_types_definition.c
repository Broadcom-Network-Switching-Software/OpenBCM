
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_qos_fields_types_definition_qos_model_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_MODEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_MODEL" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_MODEL_UNIFORM].name_from_interface, "UNIFORM", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_MODEL_UNIFORM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_MODEL_PIPE].name_from_interface, "PIPE", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_MODEL_PIPE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ENCAP_QOS_MODEL" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM].name_from_interface, "UNIFORM", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE].name_from_interface, "PIPE_NEXT_SPACE", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE].name_from_interface, "PIPE_MY_SPACE", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INITIAL].name_from_interface, "INITIAL", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INITIAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM_ECN].name_from_interface, "UNIFORM_ECN", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM_ECN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN].name_from_interface, "PIPE_ECN", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INIT_ECN].name_from_interface, "INIT_ECN", sizeof(enum_info[DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INIT_ECN].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FORWARD_QOS_MODEL" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_UNIFORM].name_from_interface, "UNIFORM", sizeof(enum_info[DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_UNIFORM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_PIPE].name_from_interface, "PIPE", sizeof(enum_info[DBAL_ENUM_FVAL_FORWARD_QOS_MODEL_PIPE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_qos_fields_types_definition_model_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MODEL_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MODEL_TYPE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM].name_from_interface, "UNIFORM", sizeof(enum_info[DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MODEL_TYPE_PIPE].name_from_interface, "PIPE", sizeof(enum_info[DBAL_ENUM_FVAL_MODEL_TYPE_PIPE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_PROFILE" , 10 , is_valid , TRUE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_ecn_qos_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ECN_QOS_PROFILE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_ecn_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_ECN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_ECN" , dnx_data_qos.qos.ecn_bits_width_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_CS_PROFILE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER].name_from_interface, "TWO_TAGS_INNER", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_INNER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE].name_from_interface, "TWO_TAGS_DOUBLE", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_TWO_TAGS_DOUBLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2].name_from_interface, "L3_L2", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_L3_L2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL].name_from_interface, "PWE_TAG_TYPICAL", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_TYPICAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER].name_from_interface, "PWE_TAG_OUTER", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_CS_PROFILE_PWE_TAG_OUTER].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_PROFILE_NAME_SPACE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2].name_from_interface, "L3_L2", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_L3_L2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET].name_from_interface, "ETHERNET", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_ETHERNET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE].name_from_interface, "TAGGED_PWE", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROFILE_NAME_SPACE_TAGGED_PWE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NETWORK_QOS_INITIAL_SELECT" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_INGRESS_NETWORK_QOS].name_from_interface, "INGRESS_NETWORK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_INGRESS_NETWORK_QOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_NETWORK_QOS].name_from_interface, "FORWARD_NETWORK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_NETWORK_QOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_PLUS_1_NETWORK_QOS].name_from_interface, "FORWARD_PLUS_1_NETWORK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_PLUS_1_NETWORK_QOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS].name_from_interface, "TC_NETWORK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_fwd_plus_one_explicit_remark_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_FWD_PLUS_ONE_EXPLICIT_REMARK_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_FWD_PLUS_ONE_EXPLICIT_REMARK_PROFILE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_dp_is_preserve_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DP_IS_PRESERVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DP_IS_PRESERVE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_meter_dp_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_METER_DP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "METER_DP_PROFILE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_network_qos_is_preserve_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_QOS_IS_PRESERVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NETWORK_QOS_IS_PRESERVE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_dp_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_DP_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_DP_SELECT" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE].name_from_interface, "PIPE", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_UNIFORM].name_from_interface, "UNIFORM", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_UNIFORM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_INITIAL].name_from_interface, "INITIAL", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_INITIAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP].name_from_interface, "PIPE_DP", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_PROPAG_STRENGTH" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW].name_from_interface, "LOW", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_LOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE].name_from_interface, "MIDDLE", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_MIDDLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH].name_from_interface, "HIGH", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_HIGH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_STRONG].name_from_interface, "STRONG", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_PROPAG_STRENGTH_STRONG].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_CAM_PRIORITY" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_int_map_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_INT_MAP_ID" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_int_map_key_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INT_MAP_KEY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_INT_MAP_KEY" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_mask_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_CS_PROFILE_MASK];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_CS_PROFILE_MASK" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_ACTION" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_key_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_KEY_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_KEY_OFFSET" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_field_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_FIELD_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_FIELD_OFFSET" , 11 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_opcode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_OPCODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_OPCODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_field_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_FIELD_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_FIELD_SIZE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_INDEX" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_ttl_inheritance_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TTL_INHERITANCE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TTL_INHERITANCE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_ttl_pipe_map_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TTL_PIPE_MAP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TTL_PIPE_MAP" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_qos_resolution_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_RESOLUTION_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_RESOLUTION_MODE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT].name_from_interface, "EXPLICIT", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_EXPLICIT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED].name_from_interface, "MAPPED", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_RESOLUTION_MODE_MAPPED].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_INT_MAP_KEY_SIZE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_ingress_qos_map_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INGRESS_QOS_MAP_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INGRESS_QOS_MAP_INDEX" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 16;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG].name_from_interface, "L2_ONE_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS].name_from_interface, "L2_TWO_TAGS", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH].name_from_interface, "RCH", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_7].name_from_interface, "7", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_8].name_from_interface, "8", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_9].name_from_interface, "9", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_9].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_10].name_from_interface, "10", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_10].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_11].name_from_interface, "11", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_11].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_12].name_from_interface, "12", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_12].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_13].name_from_interface, "13", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_13].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_14].name_from_interface, "14", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_14].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_15].name_from_interface, "15", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_15].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_DEFAULT].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_ONE_TAG].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_L2_TWO_TAGS].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV4].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_IPV6].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_MPLS].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_RCH].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_7].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_INGRESS_QOS_MAP_INDEX_8].value_from_mapping = 8;
        
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INGRESS_NWK_QOS_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER].name_from_interface, "OTHER", sizeof(enum_info[DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INGRESS_QOS_NETWORK_VALID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_ftmh_ecn_eligible_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FTMH_ECN_ELIGIBLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FTMH_ECN_ELIGIBLE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_DISABLE].name_from_interface, "DISABLE", sizeof(enum_info[DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_DISABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_ENABLE].name_from_interface, "ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_ENABLE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ECN_IS_ELIGIBLE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE].name_from_interface, "NOT_ELIGIBLE", sizeof(enum_info[DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE].name_from_interface, "ELIGIBLE", sizeof(enum_info[DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KEEP_ECN_BITS" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KEEP_ECN_BITS_DONT_KEEP_ECN_BITS].name_from_interface, "DONT_KEEP_ECN_BITS", sizeof(enum_info[DBAL_ENUM_FVAL_KEEP_ECN_BITS_DONT_KEEP_ECN_BITS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_KEEP_ECN_BITS_KEEP_ECN_BITS].name_from_interface, "KEEP_ECN_BITS", sizeof(enum_info[DBAL_ENUM_FVAL_KEEP_ECN_BITS_KEEP_ECN_BITS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "REMARK_OR_PRESERVE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP].name_from_interface, "KEEP", sizeof(enum_info[DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK].name_from_interface, "REMARK", sizeof(enum_info[DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_INGRESS_APP_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB].name_from_interface, "PHB", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_PHB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK].name_from_interface, "REMARK", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_REMARK].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN].name_from_interface, "ECN", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_ECN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL].name_from_interface, "TTL", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_INGRESS_APP_TYPE_TTL].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QOS_LAYER_PROTOCOL" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_INIT].name_from_interface, "INIT", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_INIT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_ETH].name_from_interface, "ETH", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_ETH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_OTHER].name_from_interface, "OTHER", sizeof(enum_info[DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_OTHER].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NETWORK_QOS" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_pcp_dei_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PCP_DEI];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PCP_DEI" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_qos_fields_types_definition_network_qos_pipe_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_QOS_PIPE_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NETWORK_QOS_PIPE_PROFILE" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
dbal_init_field_types_qos_fields_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_encap_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_forward_qos_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_model_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_ecn_qos_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_ecn_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_profile_name_space_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_network_qos_initial_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_fwd_plus_one_explicit_remark_profile_init(unit, cur_field_type_param, field_types_info));
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
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_key_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_field_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_opcode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_qos_fields_types_definition_qos_field_size_init(unit, cur_field_type_param, field_types_info));
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
