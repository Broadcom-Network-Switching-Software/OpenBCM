
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "COS_CL", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_CL_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_CL_SP1, "SP1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_CL_SP2, "SP2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_CL_SP3, "SP3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_CL_SP4, "SP4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_CL_SP_ENHANCED, "SP_ENHANCED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "COS_HR", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_COS_HR_EF9 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF1, "EF1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF2, "EF2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF3, "EF3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_BE, "BE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF4, "EF4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF5, "EF5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF6, "EF6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF7, "EF7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF8, "EF8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_COS_HR_EF9, "EF9");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CL_MODE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CL_MODE_5 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_MODE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_MODE_1, "1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_MODE_2, "2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_MODE_3, "3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_MODE_4, "4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_MODE_5, "5");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "WFQ_WEIGHT_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW, "DISCRETE_PER_FLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW, "INDEPENDENT_PER_FLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS, "DISCRETE_PER_CLASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INVALID, "INVALID");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CL_ENHANCED_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED, "DISABLED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP, "ENABLED_HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP, "ENABLED_LP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "HR_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_HR_MODE_ENHANCED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_HR_MODE_SINGLE, "SINGLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ, "DUAL_WFQ");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_HR_MODE_ENHANCED, "ENHANCED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VOQ", dnx_data_ipq.queues.queue_id_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VOQ_OFFSET", dnx_data_ipq.queues.voq_offset_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "HR_ID", dnx_data_sch.dbal.hr_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SCH_FLOW_ID", dnx_data_sch.dbal.flow_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_sch.flow.max_flow_get(unit);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SCH_INTERFACE_ID", dnx_data_sch.dbal.interface_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SCH_CAL_ID", dnx_data_sch.dbal.calendar_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NOF_PS_PRIORITIES", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY, "ONE_PRIORITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES, "TWO_PRIORITIES");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES, "FOUR_PRIORITIES");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES, "EIGHT_PRIORITIES");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FLOW_SLOW_STATUS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_NORMAL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_OFF, "OFF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_SLOW, "SLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_NORMAL, "NORMAL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RCI_BIASING_MODE", 9, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCI_BIASING_MODE_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCI_BIASING_MODE_GLOBAL, "GLOBAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCI_BIASING_MODE_DEVICE_BIASING, "DEVICE_BIASING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCI_BIASING_MODE_FLOW_BIASING, "FLOW_BIASING");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ODD_EVEN_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ODD_EVEN_MODE_ODD + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ODD_EVEN_MODE_EVEN, "EVEN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ODD_EVEN_MODE_ODD, "ODD");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SHARED_SHAPER_PROP_ORDER", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_HIGH_TO_LOW, "HIGH_TO_LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH, "LOW_TO_HIGH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SHARED_SHAPER_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL, "DUAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD, "QUAD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA, "OCTA");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QUARTET_ORDER", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_FQ_CL_CL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_CL_CL_FQ_FQ, "CL_CL_FQ_FQ");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_CL_FQ, "CL_FQ_CL_FQ");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_CL_FQ, "FQ_CL_CL_FQ");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_CL_FQ_FQ_CL, "CL_FQ_FQ_CL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_CL_FQ_CL, "FQ_CL_FQ_CL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QUARTET_ORDER_FQ_FQ_CL_CL, "FQ_FQ_CL_CL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LOW_RATE_FACTOR", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_64, "LOW_RATE_FACTOR_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LOW_RATE_FACTOR_LOW_RATE_FACTOR_32, "LOW_RATE_FACTOR_32");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VOQ_RATE_CLASS", dnx_data_ingr_congestion.voq.rate_class_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DRAM_USE_MODE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DRAM_USE_MODE_DRAM_ONLY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL, "NORMAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_USE_MODE_SRAM_ONLY, "SRAM_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_USE_MODE_DRAM_ONLY, "DRAM_ONLY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MULTICAST_PRIORITY", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MULTICAST_PRIORITY_LOW_PRIORITY, "LOW_PRIORITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MULTICAST_PRIORITY_HIGH_PRIORITY, "HIGH_PRIORITY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PG_MAP_TC_SELECT", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_VOQ_TC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_IN_TC, "IN_TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_TM_TC, "TM_TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_VOQ_TC, "VOQ_TC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DRAM_BOUND_STATE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DRAM_BOUND_STATE_NA + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_BOUND_STATE_S2F, "S2F");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_BOUND_STATE_CONGESTED, "CONGESTED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_BOUND_STATE_RECOVERY, "RECOVERY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DRAM_BOUND_STATE_NA, "NA");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SCH_CREDIT_COUNTER_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_REBOUND + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ALL_CREDITS, "ALL_CREDITS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_RETRY, "ONLY_RETRY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_FLOW, "ONLY_FLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SCH_CREDIT_COUNTER_TYPE_ONLY_REBOUND, "ONLY_REBOUND");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_GUARANTEED_STATUS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_PRE_BLOCKED, "PRE_BLOCKED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VSQ_GUARANTEED_STATUS_BLOCKED, "BLOCKED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SYSTEM_RED_AGING_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_DECREMENT, "DECREMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SYSTEM_RED_AGING_MODE_RESET, "RESET");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SYSTEM_RED_RESOURCE_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_DRAM_BDBS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_PDBS, "SRAM_PDBS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_SRAM_BUFFERS, "SRAM_BUFFERS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SYSTEM_RED_RESOURCE_TYPE_DRAM_BDBS, "DRAM_BDBS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CONGESTION_NOTIFICATION_RESOURCE_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_PDS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_TOTAL_WORDS, "TOTAL_WORDS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_BUFFERS, "SRAM_BUFFERS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONGESTION_NOTIFICATION_RESOURCE_TYPE_SRAM_PDS, "SRAM_PDS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SYSTEM_RED_Q_SIZE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIRROR_ON_DROP_GROUP_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_GLOBAL, "GLOBAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_VOQ, "VOQ");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PORT, "PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG, "PG");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "POOL_ID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_A_ID", utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit)-1;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_B_ID", utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit)-1;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_C_ID", utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit)-1;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_D_ID", utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit)-1;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_E_ID", utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_e_hw_nof_get(unit)-1;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSQ_F_ID", utilex_log2_round_up(dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_ingr_congestion.vsq.vsq_f_hw_nof_get(unit)-1;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FMQ_SHAPER_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER, "ROOT_SHAPER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER, "GUARANTEED_SHAPER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER, "BE_SHAPER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER, "TOP_SHAPER");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_ROOT_SHAPER].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_GUARANTEED_SHAPER].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_BE_SHAPER].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FMQ_SHAPER_TYPE_TOP_SHAPER].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SRAM_PACKET_BUFFER_DROP_PRIORITY", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_HIGH, "PRIORITY_HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_MID, "PRIORITY_MID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRAM_PACKET_BUFFER_DROP_PRIORITY_PRIORITY_LOW, "PRIORITY_LOW");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
