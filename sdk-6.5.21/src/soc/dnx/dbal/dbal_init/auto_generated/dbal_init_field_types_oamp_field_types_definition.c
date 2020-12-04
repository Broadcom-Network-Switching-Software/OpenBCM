
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_mux_src_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MUX_SRC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MUX_SRC" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 18;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_IN_FIFO].name_from_interface, "IN_FIFO", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_IN_FIFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TC_DP_LOCAL_PORT].name_from_interface, "TC_DP_LOCAL_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TC_DP_LOCAL_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_SYS_PORT].name_from_interface, "SYS_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_SYS_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_8].name_from_interface, "NS_8", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_0].name_from_interface, "NS_16_VAR_0", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_1].name_from_interface, "NS_16_VAR_1", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_PROG_VAR].name_from_interface, "PROG_VAR", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_PROG_VAR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG1].name_from_interface, "TMP_REG1", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG2].name_from_interface, "TMP_REG2", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG3].name_from_interface, "TMP_REG3", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG4].name_from_interface, "TMP_REG4", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_FDBK_FIFO].name_from_interface, "FDBK_FIFO", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_FDBK_FIFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_INST_VAR].name_from_interface, "INST_VAR", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_INST_VAR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_MSB].name_from_interface, "TOD_OAM_MSB", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_MSB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_LSB].name_from_interface, "TOD_OAM_LSB", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_LSB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_MSB].name_from_interface, "TOD_NTP_MSB", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_MSB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_LSB].name_from_interface, "TOD_NTP_LSB", sizeof(enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_LSB].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_IN_FIFO].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TC_DP_LOCAL_PORT].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_SYS_PORT].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_8].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_0].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_1].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_PROG_VAR].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG1].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG2].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG3].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TMP_REG4].value_from_mapping = 11;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_FDBK_FIFO].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_INST_VAR].value_from_mapping = 13;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_MSB].value_from_mapping = 14;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_LSB].value_from_mapping = 15;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_MSB].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_LSB].value_from_mapping = 17;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_merge_inst_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MERGE_INST];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MERGE_INST" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MERGE_INST_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_MERGE_INST_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MERGE_INST_FEM].name_from_interface, "FEM", sizeof(enum_info[DBAL_ENUM_FVAL_MERGE_INST_FEM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MERGE_INST_CONCATENATE].name_from_interface, "CONCATENATE", sizeof(enum_info[DBAL_ENUM_FVAL_MERGE_INST_CONCATENATE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MERGE_INST_OR].name_from_interface, "OR", sizeof(enum_info[DBAL_ENUM_FVAL_MERGE_INST_OR].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_MERGE_INST_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_MERGE_INST_FEM].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_MERGE_INST_CONCATENATE].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_MERGE_INST_OR].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_buff_wr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BUFF_WR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BUFF_WR" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_WR_NEVER].name_from_interface, "NEVER", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_WR_NEVER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_WR_ALWAYS].name_from_interface, "ALWAYS", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_WR_ALWAYS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1].name_from_interface, "ON_N_CMP1", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2].name_from_interface, "ON_N_CMP2", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_WR_NEVER].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_WR_ALWAYS].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_buff_size_src_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BUFF_SIZE_SRC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BUFF_SIZE_SRC" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG1].name_from_interface, "TMP_REG1", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG2].name_from_interface, "TMP_REG2", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG3].name_from_interface, "TMP_REG3", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG4].name_from_interface, "TMP_REG4", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_INST_CONST].name_from_interface, "INST_CONST", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_INST_CONST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_VLD_B].name_from_interface, "VLD_B", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_VLD_B].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG1].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG2].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG3].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG4].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_INST_CONST].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_BUFF_SIZE_SRC_VLD_B].value_from_mapping = 6;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_op_sel_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OP_SEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OP_SEL" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 16;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_TC_DP_LOCAL_PORT].name_from_interface, "TC_DP_LOCAL_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_TC_DP_LOCAL_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_SYS_PORT].name_from_interface, "SYS_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_SYS_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_8].name_from_interface, "NS_8", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_0].name_from_interface, "NS_16_VAR_0", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_1].name_from_interface, "NS_16_VAR_1", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG1].name_from_interface, "TMP_REG1", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG2].name_from_interface, "TMP_REG2", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG3].name_from_interface, "TMP_REG3", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG4].name_from_interface, "TMP_REG4", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_PRG_VAR].name_from_interface, "PRG_VAR", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_PRG_VAR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_INST_VAR].name_from_interface, "INST_VAR", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_INST_VAR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_BYTES_RD].name_from_interface, "BYTES_RD", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_BYTES_RD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_PDU_OFFS].name_from_interface, "PDU_OFFS", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_PDU_OFFS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_ENC_OFFS].name_from_interface, "ENC_OFFS", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_ENC_OFFS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP_SEL_PKT_LNTH].name_from_interface, "PKT_LNTH", sizeof(enum_info[DBAL_ENUM_FVAL_OP_SEL_PKT_LNTH].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_TC_DP_LOCAL_PORT].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_SYS_PORT].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_8].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_0].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_1].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG1].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG2].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG3].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_TMP_REG4].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_PRG_VAR].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_INST_VAR].value_from_mapping = 11;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_BYTES_RD].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_PDU_OFFS].value_from_mapping = 13;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_ENC_OFFS].value_from_mapping = 14;
        
        enum_info[DBAL_ENUM_FVAL_OP_SEL_PKT_LNTH].value_from_mapping = 15;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_alu_act_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ALU_ACT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ALU_ACT" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_ACT_ADD].name_from_interface, "ADD", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_ACT_ADD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_ACT_SUB].name_from_interface, "SUB", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_ACT_SUB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_ACT_AND].name_from_interface, "AND", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_ACT_AND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_ACT_OR].name_from_interface, "OR", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_ACT_OR].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_ALU_ACT_ADD].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_ALU_ACT_SUB].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_ALU_ACT_AND].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_ALU_ACT_OR].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_cmp_act_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CMP_ACT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CMP_ACT" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NEVER].name_from_interface, "NEVER", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NEVER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NEG].name_from_interface, "NEG", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NEG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_ZERO].name_from_interface, "ZERO", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_ZERO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_POS].name_from_interface, "POS", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_POS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NNEG].name_from_interface, "NNEG", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NNEG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NPOS].name_from_interface, "NPOS", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_NPOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CMP_ACT_ALWAYS].name_from_interface, "ALWAYS", sizeof(enum_info[DBAL_ENUM_FVAL_CMP_ACT_ALWAYS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_NEVER].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_NEG].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_ZERO].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_POS].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_NNEG].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_NPOS].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_CMP_ACT_ALWAYS].value_from_mapping = 6;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_alu_dst_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ALU_DST];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ALU_DST" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG1].name_from_interface, "TMP_REG1", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG2].name_from_interface, "TMP_REG2", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG3].name_from_interface, "TMP_REG3", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG4].name_from_interface, "TMP_REG4", sizeof(enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG4].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG1].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG2].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG3].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_ALU_DST_TMP_REG4].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_buf_eop_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BUF_EOP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BUF_EOP" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER].name_from_interface, "WR_NEVER", sizeof(enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1].name_from_interface, "WR_ON_CMP1", sizeof(enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2].name_from_interface, "WR_ON_CMP2", sizeof(enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUF_EOP_ALWAYS].name_from_interface, "ALWAYS", sizeof(enum_info[DBAL_ENUM_FVAL_BUF_EOP_ALWAYS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_BUF_EOP_ALWAYS].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_op3_sel_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OP3_SEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OP3_SEL" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 10;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_FEM].name_from_interface, "FEM", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_FEM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG1].name_from_interface, "TMP_REG1", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG2].name_from_interface, "TMP_REG2", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG3].name_from_interface, "TMP_REG3", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG4].name_from_interface, "TMP_REG4", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_ALU_OUT].name_from_interface, "ALU_OUT", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_ALU_OUT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_MEM_OUT].name_from_interface, "MEM_OUT", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_MEM_OUT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_INST_VAR].name_from_interface, "INST_VAR", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_INST_VAR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OP3_SEL_INST_VAR_MSB].name_from_interface, "INST_VAR_MSB", sizeof(enum_info[DBAL_ENUM_FVAL_OP3_SEL_INST_VAR_MSB].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_FEM].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG1].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG2].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG3].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_TMP_REG4].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_ALU_OUT].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_MEM_OUT].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_INST_VAR].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_OP3_SEL_INST_VAR_MSB].value_from_mapping = 9;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_inst_src_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INST_SRC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INST_SRC" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_NXT].name_from_interface, "LOP_NXT", sizeof(enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_NXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_JMP].name_from_interface, "LOP_JMP", sizeof(enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_JMP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INST_SRC_NXT_JMP].name_from_interface, "NXT_JMP", sizeof(enum_info[DBAL_ENUM_FVAL_INST_SRC_NXT_JMP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_CST].name_from_interface, "LOP_CST", sizeof(enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_CST].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_NXT].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_JMP].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_INST_SRC_NXT_JMP].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_INST_SRC_LOP_CST].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_buff2_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BUFF2_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BUFF2_SIZE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_1B].name_from_interface, "1B", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_1B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_2B].name_from_interface, "2B", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_3B].name_from_interface, "3B", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_3B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_4B].name_from_interface, "4B", sizeof(enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_4B].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_1B].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_2B].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_3B].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_BUFF2_SIZE_4B].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_msg_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MSG_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MSG_TYPE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 5;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR].name_from_interface, "LMM_LMR_SLM_SLR", sizeof(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR].name_from_interface, "DMM_DMR", sizeof(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_OTHER].name_from_interface, "OTHER", sizeof(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_OTHER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_GEN_RESPONSE].name_from_interface, "GEN_RESPONSE", sizeof(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_GEN_RESPONSE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_GEN_OPCODE].name_from_interface, "GEN_OPCODE", sizeof(enum_info[DBAL_ENUM_FVAL_MSG_TYPE_GEN_OPCODE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_MSG_TYPE_OTHER].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_MSG_TYPE_GEN_RESPONSE].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_MSG_TYPE_GEN_OPCODE].value_from_mapping = 4;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_debug_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DEBUG_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DEBUG_MODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_OFF].name_from_interface, "OFF", sizeof(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_OFF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_INST_INDEX].name_from_interface, "INST_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_INST_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_VAR_0_1].name_from_interface, "VAR_0_1", sizeof(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_VAR_0_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_VAR_2_3].name_from_interface, "VAR_2_3", sizeof(enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_VAR_2_3].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_OFF].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_INST_INDEX].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_VAR_0_1].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_DEBUG_MODE_VAR_2_3].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_fem_op_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEM_OP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FEM_OP" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 2 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "FEM_OFFSET", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "FEM_CONST", sizeof(cur_field_type_param->childs[1].name));
    cur_field_type_param->nof_childs = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_fem_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEM_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FEM_OFFSET" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_fem_const_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEM_CONST];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FEM_CONST" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_oamp_pe_instruct_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_PE_INSTRUCT_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAMP_PE_INSTRUCT_INDEX" , 11 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_oamp_pe_program_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_PE_PROGRAM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAMP_PE_PROGRAM" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_last_pkt_vars_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LAST_PKT_VARS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LAST_PKT_VARS" , 112 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 12;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_RESERVED;
    cur_field_types_info->struct_field_info[0].length = 2;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_LOCAL_PORT;
    cur_field_types_info->struct_field_info[3].length = 9;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_SYSTEM_PORT;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_NIBLE_SELECT_8_0;
    cur_field_types_info->struct_field_info[5].length = 8;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_NIBLE_SELECT_8_1;
    cur_field_types_info->struct_field_info[6].length = 8;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_NIBLE_SELECT_16_0;
    cur_field_types_info->struct_field_info[7].length = 16;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_NIBLE_SELECT_16_1;
    cur_field_types_info->struct_field_info[8].length = 16;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_PDU_OFFSET;
    cur_field_types_info->struct_field_info[9].length = 8;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_ENCAP_OFFSET;
    cur_field_types_info->struct_field_info[10].length = 8;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_PKT_LENGTH;
    cur_field_types_info->struct_field_info[11].length = 16;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_oamp_pe_prog_tcam_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_PE_PROG_TCAM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAMP_PE_PROG_TCAM" , 24 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 8 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "MEP_GENERATED", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "MEP_RESPONSE", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "PUNT_PACKET", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "EVENT_PACKET", sizeof(cur_field_type_param->childs[3].name));
    sal_strncpy(cur_field_type_param->childs[4].name, "SAT_PACKET", sizeof(cur_field_type_param->childs[4].name));
    sal_strncpy(cur_field_type_param->childs[5].name, "DMA_PACKET_1", sizeof(cur_field_type_param->childs[5].name));
    sal_strncpy(cur_field_type_param->childs[6].name, "DMA_PACKET_2", sizeof(cur_field_type_param->childs[6].name));
    sal_strncpy(cur_field_type_param->childs[7].name, "TXO_PACKET", sizeof(cur_field_type_param->childs[7].name));
    cur_field_type_param->nof_childs = 8;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 2;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[4].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[4].encode_param1 = 4;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[5].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[5].encode_param1 = 5;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[6].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[6].encode_param1 = 6;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[7].encode_type, "SUFFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[7].encode_param1 = 7;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_mep_generated_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_GENERATED];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MEP_GENERATED" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OAMP_MEP_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEP_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MEP_PE_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MSG_TYPE;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_mep_response_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_RESPONSE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MEP_RESPONSE" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OAMP_MEP_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEP_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MEP_PE_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MSG_TYPE;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_punt_packet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PUNT_PACKET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PUNT_PACKET" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 5;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_PUNT_RESERVED_1;
    cur_field_types_info->struct_field_info[0].length = 4;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PUNT_PE_PROFILE;
    cur_field_types_info->struct_field_info[1].length = 2;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_PUNT_RESERVED_2;
    cur_field_types_info->struct_field_info[2].length = 5;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_PUNT_TRAP_CODE;
    cur_field_types_info->struct_field_info[3].length = 8;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_PUNT_RESERVED_3;
    cur_field_types_info->struct_field_info[4].length = 2;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_event_packet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EVENT_PACKET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EVENT_PACKET" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 1;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_EVENT_PACKET_PE_SEL;
    cur_field_types_info->struct_field_info[0].length = 24;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_sat_packet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SAT_PACKET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SAT_PACKET" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_SAT_PE_KEY_MSB;
    cur_field_types_info->struct_field_info[0].length = 17;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_GENERATOR_INDEX;
    cur_field_types_info->struct_field_info[1].length = 4;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_txo_packet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TXO_PACKET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TXO_PACKET" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OAMP_MEP_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEP_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MEP_PE_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MSG_TYPE;
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_dma_packet_1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DMA_PACKET_1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DMA_PACKET_1" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_dma_packet_2_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DMA_PACKET_2];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DMA_PACKET_2" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_oamp_field_types_definition_mep_pe_profile_sw_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE_SW];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MEP_PE_PROFILE_SW" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 32;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO].name_from_interface, "BFD_ECHO", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48].name_from_interface, "MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT].name_from_interface, "BFD_DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6].name_from_interface, "BFD_IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS].name_from_interface, "BFD_IPV6_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP].name_from_interface, "DOWN_MEP", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48].name_from_interface, "DOWN_MEP_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER].name_from_interface, "UP_MEP_SERVER", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48].name_from_interface, "UP_MEP_SERVER_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM].name_from_interface, "1DM", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48].name_from_interface, "1DM_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA].name_from_interface, "LMDM_FLEXIBLE_DA", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT].name_from_interface, "SIGNAL_DETECT", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48].name_from_interface, "SIGNAL_DETECT_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].name_from_interface, "DM_JUMBO_TLV_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].name_from_interface, "DM_JUMBO_TLV", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4].name_from_interface, "SEAMLESS_BFD_IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6].name_from_interface, "SEAMLESS_BFD_IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP].name_from_interface, "SINGLE_HOP_BFD_RANDOM_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV].name_from_interface, "BFD_MPLS_TP_VCCV", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD].name_from_interface, "UP_MEP_DM_TOD", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD_CCM_MAID_48].name_from_interface, "UP_MEP_DM_TOD_CCM_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD_CCM_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48].value_from_mapping = 11;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48].value_from_mapping = 13;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA].value_from_mapping = 14;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48].value_from_mapping = 15;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48].value_from_mapping = 17;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48].value_from_mapping = 18;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV].value_from_mapping = 19;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4].value_from_mapping = 20;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6].value_from_mapping = 21;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP].value_from_mapping = 22;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV].value_from_mapping = 23;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL].value_from_mapping = 24;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].value_from_mapping = 25;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].value_from_mapping = 26;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].value_from_mapping = 27;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD].value_from_mapping = 28;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD_CCM_MAID_48].value_from_mapping = 29;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].value_from_mapping = 30;
        
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].value_from_mapping = 31;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_oamp_pe_program_enum_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_PE_PROGRAM_ENUM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAMP_PE_PROGRAM_ENUM" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 33;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO].name_from_interface, "BFD_ECHO", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48].name_from_interface, "MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6].name_from_interface, "BFD_IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS].name_from_interface, "BFD_IPV6_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1].name_from_interface, "BFD_IPV6_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1].name_from_interface, "BFD_IPV6_MPLS_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1].name_from_interface, "BFD_ECHO_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DEFAULT].name_from_interface, "BFD_DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_ADD_UDH_JR1].name_from_interface, "DOWN_MEP_ADD_UDH_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_ADD_UDH_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER].name_from_interface, "UP_MEP_SERVER", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER_MAID_48].name_from_interface, "UP_MEP_SERVER_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM].name_from_interface, "1DM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA].name_from_interface, "LMDM_FLEXIBLE_DA", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_CCM].name_from_interface, "SIGNAL_DETECT_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].name_from_interface, "SIGNAL_DETECT_MAID_48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV].name_from_interface, "DM_JUMBO_TLV", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP].name_from_interface, "DM_JUMBO_TLV_RSP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4].name_from_interface, "SEAMLESS_BFD_IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6].name_from_interface, "SEAMLESS_BFD_IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1].name_from_interface, "SEAMLESS_BFD_IPV4_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1].name_from_interface, "SEAMLESS_BFD_IPV6_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].name_from_interface, "SINGLE_HOP_BFD_RANDOM_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV].name_from_interface, "BFD_MPLS_TP_VCCV", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD].name_from_interface, "UP_MEP_DM_TOD", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_RSP].name_from_interface, "UP_MEP_DM_TOD_RSP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_RSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].name_from_interface, "UP_MEP_DM_TOD_CCM_48_MAID", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_INVALID].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DEFAULT].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_ADD_UDH_JR1].value_from_mapping = 11;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER_MAID_48].value_from_mapping = 13;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM].value_from_mapping = 14;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA].value_from_mapping = 15;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_CCM].value_from_mapping = 17;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].value_from_mapping = 18;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV].value_from_mapping = 19;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP].value_from_mapping = 20;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4].value_from_mapping = 21;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6].value_from_mapping = 22;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1].value_from_mapping = 23;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1].value_from_mapping = 24;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].value_from_mapping = 25;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV].value_from_mapping = 26;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].value_from_mapping = 27;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].value_from_mapping = 28;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD].value_from_mapping = 29;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_RSP].value_from_mapping = 30;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].value_from_mapping = 31;
        
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_INVALID].value_from_mapping = 127;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oamp_field_types_definition_oamp_pe_prog_tcam_enum_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_PE_PROG_TCAM_ENUM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAMP_PE_PROG_TCAM_ENUM" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 49;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1].name_from_interface, "BFD_ECHO_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1].name_from_interface, "BFD_IPV6_MPLS_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1].name_from_interface, "BFD_IPV6_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS].name_from_interface, "BFD_IPV6_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6].name_from_interface, "BFD_IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48].name_from_interface, "MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO].name_from_interface, "BFD_ECHO", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT].name_from_interface, "BFD_DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1].name_from_interface, "DOWN_MEP_ADD_UDH_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER].name_from_interface, "UP_MEP_SERVER", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48].name_from_interface, "UP_MEP_SERVER_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM].name_from_interface, "1DM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48].name_from_interface, "1DM_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM].name_from_interface, "1DM_MAID_48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA].name_from_interface, "LMDM_FLEXIBLE_DA", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM].name_from_interface, "SIGNAL_DETECT_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].name_from_interface, "SIGNAL_DETECT_MAID_48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV].name_from_interface, "DM_JUMBO_TLV", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP].name_from_interface, "DM_JUMBO_TLV_RSP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4].name_from_interface, "SEAMLESS_BFD_IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6].name_from_interface, "SEAMLESS_BFD_IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1].name_from_interface, "SEAMLESS_BFD_IPV4_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1].name_from_interface, "SEAMLESS_BFD_IPV6_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].name_from_interface, "SINGLE_HOP_BFD_RANDOM_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV].name_from_interface, "BFD_MPLS_TP_VCCV", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD].name_from_interface, "UP_MEP_DM_TOD", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_RSP].name_from_interface, "UP_MEP_DM_TOD_RSP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_RSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].name_from_interface, "UP_MEP_DM_TOD_CCM_48_MAID", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM].name_from_interface, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT].name_from_interface, "DEFAULT", sizeof(enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].value_from_mapping = 31;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].value_from_mapping = 33;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM].value_from_mapping = 34;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM].value_from_mapping = 35;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM].value_from_mapping = 37;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD].value_from_mapping = 38;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_RSP].value_from_mapping = 39;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].value_from_mapping = 41;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP].value_from_mapping = 42;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER].value_from_mapping = 43;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].value_from_mapping = 44;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP].value_from_mapping = 45;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM].value_from_mapping = 46;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM].value_from_mapping = 47;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT].value_from_mapping = 48;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_oamp_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_mux_src_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_merge_inst_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_buff_wr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_buff_size_src_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_op_sel_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_alu_act_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_cmp_act_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_alu_dst_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_buf_eop_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_op3_sel_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_inst_src_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_buff2_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_msg_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_debug_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_fem_op_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_fem_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_fem_const_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_oamp_pe_instruct_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_oamp_pe_program_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_last_pkt_vars_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_oamp_pe_prog_tcam_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_mep_generated_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_mep_response_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_punt_packet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_event_packet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_sat_packet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_txo_packet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_dma_packet_1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_dma_packet_2_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_mep_pe_profile_sw_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_oamp_pe_program_enum_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oamp_field_types_definition_oamp_pe_prog_tcam_enum_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
