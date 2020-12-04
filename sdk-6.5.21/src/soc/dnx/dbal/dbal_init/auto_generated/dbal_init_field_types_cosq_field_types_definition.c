
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_cos_cl_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_COS_CL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "COS_CL" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_CL_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_COS_CL_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_CL_SP1].name_from_interface, "SP1", sizeof(enum_info[DBAL_ENUM_FVAL_COS_CL_SP1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_CL_SP2].name_from_interface, "SP2", sizeof(enum_info[DBAL_ENUM_FVAL_COS_CL_SP2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_CL_SP3].name_from_interface, "SP3", sizeof(enum_info[DBAL_ENUM_FVAL_COS_CL_SP3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_CL_SP4].name_from_interface, "SP4", sizeof(enum_info[DBAL_ENUM_FVAL_COS_CL_SP4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED].name_from_interface, "SP_ENHANCED", sizeof(enum_info[DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_COS_CL_INVALID].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_COS_CL_SP1].value_from_mapping = 252;
        
        enum_info[DBAL_ENUM_FVAL_COS_CL_SP2].value_from_mapping = 253;
        
        enum_info[DBAL_ENUM_FVAL_COS_CL_SP3].value_from_mapping = 254;
        
        enum_info[DBAL_ENUM_FVAL_COS_CL_SP4].value_from_mapping = 255;
        
        enum_info[DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED].value_from_mapping = 251;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_cos_hr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_COS_HR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "COS_HR" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 11;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF1].name_from_interface, "EF1", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF2].name_from_interface, "EF2", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF3].name_from_interface, "EF3", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_BE].name_from_interface, "BE", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_BE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF4].name_from_interface, "EF4", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF5].name_from_interface, "EF5", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF6].name_from_interface, "EF6", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF7].name_from_interface, "EF7", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF8].name_from_interface, "EF8", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_COS_HR_EF9].name_from_interface, "EF9", sizeof(enum_info[DBAL_ENUM_FVAL_COS_HR_EF9].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_INVALID].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF1].value_from_mapping = 252;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF2].value_from_mapping = 253;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF3].value_from_mapping = 254;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_BE].value_from_mapping = 255;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF4].value_from_mapping = 48;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF5].value_from_mapping = 112;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF6].value_from_mapping = 176;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF7].value_from_mapping = 240;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF8].value_from_mapping = 228;
        
        enum_info[DBAL_ENUM_FVAL_COS_HR_EF9].value_from_mapping = 225;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_cl_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CL_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CL_MODE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_MODE_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_CL_MODE_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_MODE_1].name_from_interface, "1", sizeof(enum_info[DBAL_ENUM_FVAL_CL_MODE_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_MODE_2].name_from_interface, "2", sizeof(enum_info[DBAL_ENUM_FVAL_CL_MODE_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_MODE_3].name_from_interface, "3", sizeof(enum_info[DBAL_ENUM_FVAL_CL_MODE_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_MODE_4].name_from_interface, "4", sizeof(enum_info[DBAL_ENUM_FVAL_CL_MODE_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_MODE_5].name_from_interface, "5", sizeof(enum_info[DBAL_ENUM_FVAL_CL_MODE_5].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_CL_MODE_NONE].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_CL_MODE_1].value_from_mapping = 15;
        
        enum_info[DBAL_ENUM_FVAL_CL_MODE_2].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_CL_MODE_3].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_CL_MODE_4].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_CL_MODE_5].value_from_mapping = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_wfq_weight_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_WFQ_WEIGHT_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "WFQ_WEIGHT_MODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW].name_from_interface, "DISCRETE_PER_FLOW", sizeof(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW].name_from_interface, "INDEPENDENT_PER_FLOW", sizeof(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS].name_from_interface, "DISCRETE_PER_CLASS", sizeof(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INVALID].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INVALID].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_cl_enhanced_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CL_ENHANCED_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CL_ENHANCED_MODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED].name_from_interface, "DISABLED", sizeof(enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP].name_from_interface, "ENABLED_HP", sizeof(enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP].name_from_interface, "ENABLED_LP", sizeof(enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_hr_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_HR_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "HR_MODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_HR_MODE_SINGLE].name_from_interface, "SINGLE", sizeof(enum_info[DBAL_ENUM_FVAL_HR_MODE_SINGLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ].name_from_interface, "DUAL_WFQ", sizeof(enum_info[DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_HR_MODE_ENHANCED].name_from_interface, "ENHANCED", sizeof(enum_info[DBAL_ENUM_FVAL_HR_MODE_ENHANCED].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_HR_MODE_SINGLE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_HR_MODE_ENHANCED].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_voq_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VOQ];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VOQ" , dnx_data_ipq.queues.queue_id_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_cosq_field_types_definition_voq_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VOQ_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VOQ_OFFSET" , dnx_data_ipq.queues.voq_offset_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_cosq_field_types_definition_hr_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_HR_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "HR_ID" , dnx_data_sch.dbal.hr_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_cosq_field_types_definition_sch_flow_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SCH_FLOW_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SCH_FLOW_ID" , dnx_data_sch.dbal.flow_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_sch.flow.max_flow_get(unit);
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_sch_interface_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SCH_INTERFACE_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SCH_INTERFACE_ID" , dnx_data_sch.dbal.interface_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_sch.interface.reserved_get(unit);
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_sch_cal_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SCH_CAL_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SCH_CAL_ID" , dnx_data_sch.dbal.calendar_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_cosq_field_types_definition_nof_ps_priorities_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NOF_PS_PRIORITIES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NOF_PS_PRIORITIES" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY].name_from_interface, "ONE_PRIORITY", sizeof(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES].name_from_interface, "TWO_PRIORITIES", sizeof(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES].name_from_interface, "FOUR_PRIORITIES", sizeof(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES].name_from_interface, "EIGHT_PRIORITIES", sizeof(enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES].value_from_mapping = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_flow_slow_status_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FLOW_SLOW_STATUS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FLOW_SLOW_STATUS" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_OFF].name_from_interface, "OFF", sizeof(enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_OFF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_SLOW].name_from_interface, "SLOW", sizeof(enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_SLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_NORMAL].name_from_interface, "NORMAL", sizeof(enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_NORMAL].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_OFF].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_SLOW].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_NORMAL].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_rci_biasing_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RCI_BIASING_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "RCI_BIASING_MODE" , 9 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_GLOBAL].name_from_interface, "GLOBAL", sizeof(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_GLOBAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_DEVICE_BIASING].name_from_interface, "DEVICE_BIASING", sizeof(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_DEVICE_BIASING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING].name_from_interface, "FLOW_BIASING", sizeof(enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_INVALID].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_GLOBAL].value_from_mapping = 23;
        
        enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_DEVICE_BIASING].value_from_mapping = 287;
        
        enum_info[DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING].value_from_mapping = 191;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_odd_even_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ODD_EVEN_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ODD_EVEN_MODE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ODD_EVEN_MODE_EVEN].name_from_interface, "EVEN", sizeof(enum_info[DBAL_ENUM_FVAL_ODD_EVEN_MODE_EVEN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ODD_EVEN_MODE_ODD].name_from_interface, "ODD", sizeof(enum_info[DBAL_ENUM_FVAL_ODD_EVEN_MODE_ODD].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_ODD_EVEN_MODE_EVEN].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_ODD_EVEN_MODE_ODD].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_shared_shaper_prop_order_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SHARED_SHAPER_PROP_ORDER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SHARED_SHAPER_PROP_ORDER" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_HIGH_TO_LOW].name_from_interface, "HIGH_TO_LOW", sizeof(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_HIGH_TO_LOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH].name_from_interface, "LOW_TO_HIGH", sizeof(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_HIGH_TO_LOW].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_shared_shaper_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SHARED_SHAPER_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SHARED_SHAPER_MODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL].name_from_interface, "DUAL", sizeof(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD].name_from_interface, "QUAD", sizeof(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA].name_from_interface, "OCTA", sizeof(enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_quartet_order_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QUARTET_ORDER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "QUARTET_ORDER" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_CL_FQ_FQ].name_from_interface, "CL_CL_FQ_FQ", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_CL_FQ_FQ].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_CL_FQ].name_from_interface, "CL_FQ_CL_FQ", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_CL_FQ].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_CL_FQ].name_from_interface, "FQ_CL_CL_FQ", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_CL_FQ].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_FQ_CL].name_from_interface, "CL_FQ_FQ_CL", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_FQ_CL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_FQ_CL].name_from_interface, "FQ_CL_FQ_CL", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_FQ_CL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_FQ_CL_CL].name_from_interface, "FQ_FQ_CL_CL", sizeof(enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_FQ_CL_CL].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_INVALID].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_CL_FQ_FQ].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_CL_FQ].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_CL_FQ].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_FQ_CL].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_FQ_CL].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_FQ_CL_CL].value_from_mapping = 12;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_low_rate_factor_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LOW_RATE_FACTOR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LOW_RATE_FACTOR" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64].name_from_interface, "LOW_RATE_FACTOR_64", sizeof(enum_info[DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32].name_from_interface, "LOW_RATE_FACTOR_32", sizeof(enum_info[DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_voq_rate_class_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VOQ_RATE_CLASS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VOQ_RATE_CLASS" , dnx_data_ingr_congestion.voq.rate_class_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_cosq_field_types_definition_dram_use_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DRAM_USE_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DRAM_USE_MODE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL].name_from_interface, "NORMAL", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_SRAM_ONLY].name_from_interface, "SRAM_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_SRAM_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_DRAM_ONLY].name_from_interface, "DRAM_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_DRAM_ONLY].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_SRAM_ONLY].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_USE_MODE_DRAM_ONLY].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_multicast_priority_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MULTICAST_PRIORITY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MULTICAST_PRIORITY" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MULTICAST_PRIORITY_LOW_PRIORITY].name_from_interface, "LOW_PRIORITY", sizeof(enum_info[DBAL_ENUM_FVAL_MULTICAST_PRIORITY_LOW_PRIORITY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY].name_from_interface, "HIGH_PRIORITY", sizeof(enum_info[DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_MULTICAST_PRIORITY_LOW_PRIORITY].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_pg_map_tc_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PG_MAP_TC_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PG_MAP_TC_SELECT" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_IN_TC].name_from_interface, "IN_TC", sizeof(enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_IN_TC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_TM_TC].name_from_interface, "TM_TC", sizeof(enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_TM_TC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_VOQ_TC].name_from_interface, "VOQ_TC", sizeof(enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_VOQ_TC].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_IN_TC].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_TM_TC].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_VOQ_TC].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_dram_bound_state_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DRAM_BOUND_STATE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DRAM_BOUND_STATE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F].name_from_interface, "S2F", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_CONGESTED].name_from_interface, "CONGESTED", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_CONGESTED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_RECOVERY].name_from_interface, "RECOVERY", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_RECOVERY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_NA].name_from_interface, "NA", sizeof(enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_NA].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_CONGESTED].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_RECOVERY].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_DRAM_BOUND_STATE_NA].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_sch_credit_counter_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SCH_CREDIT_COUNTER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SCH_CREDIT_COUNTER_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ALL_CREDITS].name_from_interface, "ALL_CREDITS", sizeof(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ALL_CREDITS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_RETRY].name_from_interface, "ONLY_RETRY", sizeof(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_RETRY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_FLOW].name_from_interface, "ONLY_FLOW", sizeof(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_FLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_REBOUND].name_from_interface, "ONLY_REBOUND", sizeof(enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_REBOUND].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ALL_CREDITS].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_RETRY].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_FLOW].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_REBOUND].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_vsq_guaranteed_status_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_GUARANTEED_STATUS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_GUARANTEED_STATUS" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_PRE_BLOCKED].name_from_interface, "PRE_BLOCKED", sizeof(enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_PRE_BLOCKED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED].name_from_interface, "BLOCKED", sizeof(enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_PRE_BLOCKED].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_system_red_aging_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SYSTEM_RED_AGING_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SYSTEM_RED_AGING_MODE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_DECREMENT].name_from_interface, "DECREMENT", sizeof(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_DECREMENT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET].name_from_interface, "RESET", sizeof(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_DECREMENT].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_system_red_resource_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SYSTEM_RED_RESOURCE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SYSTEM_RED_RESOURCE_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_PDBS].name_from_interface, "SRAM_PDBS", sizeof(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_PDBS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_BUFFERS].name_from_interface, "SRAM_BUFFERS", sizeof(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_BUFFERS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_DRAM_BDBS].name_from_interface, "DRAM_BDBS", sizeof(enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_DRAM_BDBS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_PDBS].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_BUFFERS].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_DRAM_BDBS].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_congestion_notification_resource_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CONGESTION_NOTIFICATION_RESOURCE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CONGESTION_NOTIFICATION_RESOURCE_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_TOTAL_WORDS].name_from_interface, "TOTAL_WORDS", sizeof(enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_TOTAL_WORDS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_BUFFERS].name_from_interface, "SRAM_BUFFERS", sizeof(enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_BUFFERS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_PDS].name_from_interface, "SRAM_PDS", sizeof(enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_PDS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_TOTAL_WORDS].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_BUFFERS].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_PDS].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_system_red_q_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SYSTEM_RED_Q_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SYSTEM_RED_Q_SIZE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_mirror_on_drop_group_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIRROR_ON_DROP_GROUP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MIRROR_ON_DROP_GROUP_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_GLOBAL].name_from_interface, "GLOBAL", sizeof(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_GLOBAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_VOQ].name_from_interface, "VOQ", sizeof(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_VOQ].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PORT].name_from_interface, "PORT", sizeof(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG].name_from_interface, "PG", sizeof(enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_GLOBAL].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_VOQ].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PORT].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_pool_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_POOL_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "POOL_ID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_cosq_field_types_definition_vsq_a_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_A_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_A_ID" , utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit)) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit)-1;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_vsq_b_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_B_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_B_ID" , utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit)) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit)-1;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_vsq_c_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_C_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_C_ID" , utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit)) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit)-1;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_vsq_d_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_D_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_D_ID" , utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit)) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit)-1;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_vsq_e_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_E_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_E_ID" , utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get(unit)) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get(unit)-1;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_vsq_f_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSQ_F_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VSQ_F_ID" , utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit)) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit)-1;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_fmq_shaper_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FMQ_SHAPER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FMQ_SHAPER_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER].name_from_interface, "ROOT_SHAPER", sizeof(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER].name_from_interface, "GUARANTEED_SHAPER", sizeof(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER].name_from_interface, "BE_SHAPER", sizeof(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER].name_from_interface, "TOP_SHAPER", sizeof(enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER].name_from_interface));
    }
    if (DBAL_IS_J2C_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER].value_from_mapping = 3;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_cosq_field_types_definition_sram_packet_buffer_drop_priority_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRAM_PACKET_BUFFER_DROP_PRIORITY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SRAM_PACKET_BUFFER_DROP_PRIORITY" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH].name_from_interface, "PRIORITY_HIGH", sizeof(enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID].name_from_interface, "PRIORITY_MID", sizeof(enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW].name_from_interface, "PRIORITY_LOW", sizeof(enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_cosq_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_cos_cl_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_cos_hr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_cl_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_wfq_weight_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_cl_enhanced_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_hr_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_voq_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_voq_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_hr_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_sch_flow_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_sch_interface_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_sch_cal_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_nof_ps_priorities_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_flow_slow_status_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_rci_biasing_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_odd_even_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_shared_shaper_prop_order_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_shared_shaper_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_quartet_order_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_low_rate_factor_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_voq_rate_class_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_dram_use_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_multicast_priority_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_pg_map_tc_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_dram_bound_state_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_sch_credit_counter_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_guaranteed_status_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_system_red_aging_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_system_red_resource_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_congestion_notification_resource_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_system_red_q_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_mirror_on_drop_group_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_pool_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_a_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_b_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_c_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_d_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_e_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_vsq_f_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_fmq_shaper_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_cosq_field_types_definition_sram_packet_buffer_drop_priority_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
