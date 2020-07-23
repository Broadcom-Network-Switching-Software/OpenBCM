
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 18 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "IN_FIFO", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TC_DP_LOCAL_PORT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "SYS_PORT", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "NS_8", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "NS_16_VAR_0", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "NS_16_VAR_1", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "PROG_VAR", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "TMP_REG1", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "TMP_REG2", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "TMP_REG3", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "TMP_REG4", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "FDBK_FIFO", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "INST_VAR", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "TOD_OAM_MSB", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "TOD_OAM_LSB", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "TOD_NTP_MSB", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "TOD_NTP_LSB", sizeof(cur_field_type_param->enums[17].name_from_interface));
    cur_field_type_param->nof_enum_vals = 18;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    cur_field_type_param->enums[7].value_from_mapping = 7;
    cur_field_type_param->enums[8].value_from_mapping = 8;
    cur_field_type_param->enums[9].value_from_mapping = 9;
    cur_field_type_param->enums[10].value_from_mapping = 10;
    cur_field_type_param->enums[11].value_from_mapping = 11;
    cur_field_type_param->enums[12].value_from_mapping = 12;
    cur_field_type_param->enums[13].value_from_mapping = 13;
    cur_field_type_param->enums[14].value_from_mapping = 14;
    cur_field_type_param->enums[15].value_from_mapping = 15;
    cur_field_type_param->enums[16].value_from_mapping = 16;
    cur_field_type_param->enums[17].value_from_mapping = 17;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FEM", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "CONCATENATE", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "OR", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NEVER", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ALWAYS", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ON_N_CMP1", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "ON_N_CMP2", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 7 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TMP_REG1", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TMP_REG2", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TMP_REG3", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TMP_REG4", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "INST_CONST", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "VLD_B", sizeof(cur_field_type_param->enums[6].name_from_interface));
    cur_field_type_param->nof_enum_vals = 7;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 16 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TC_DP_LOCAL_PORT", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "SYS_PORT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "NS_8", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "NS_16_VAR_0", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "NS_16_VAR_1", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "TMP_REG1", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "TMP_REG2", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "TMP_REG3", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "TMP_REG4", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "PRG_VAR", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "INST_VAR", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "BYTES_RD", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "PDU_OFFS", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "ENC_OFFS", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "PKT_LNTH", sizeof(cur_field_type_param->enums[15].name_from_interface));
    cur_field_type_param->nof_enum_vals = 16;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    cur_field_type_param->enums[7].value_from_mapping = 7;
    cur_field_type_param->enums[8].value_from_mapping = 8;
    cur_field_type_param->enums[9].value_from_mapping = 9;
    cur_field_type_param->enums[10].value_from_mapping = 10;
    cur_field_type_param->enums[11].value_from_mapping = 11;
    cur_field_type_param->enums[12].value_from_mapping = 12;
    cur_field_type_param->enums[13].value_from_mapping = 13;
    cur_field_type_param->enums[14].value_from_mapping = 14;
    cur_field_type_param->enums[15].value_from_mapping = 15;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "ADD", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "SUB", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "AND", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "OR", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 7 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NEVER", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "NEG", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ZERO", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "POS", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "NNEG", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "NPOS", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "ALWAYS", sizeof(cur_field_type_param->enums[6].name_from_interface));
    cur_field_type_param->nof_enum_vals = 7;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "TMP_REG1", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TMP_REG2", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TMP_REG3", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TMP_REG4", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "WR_NEVER", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "WR_ON_CMP1", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "WR_ON_CMP2", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "ALWAYS", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 10 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FEM", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TMP_REG1", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TMP_REG2", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TMP_REG3", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "TMP_REG4", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "ALU_OUT", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "MEM_OUT", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "INST_VAR", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "INST_VAR_MSB", sizeof(cur_field_type_param->enums[9].name_from_interface));
    cur_field_type_param->nof_enum_vals = 10;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    cur_field_type_param->enums[7].value_from_mapping = 7;
    cur_field_type_param->enums[8].value_from_mapping = 8;
    cur_field_type_param->enums[9].value_from_mapping = 9;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "LOP_NXT", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "LOP_JMP", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "NXT_JMP", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "LOP_CST", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "1B", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "2B", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "3B", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "4B", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 5 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "LMM_LMR_SLM_SLR", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "DMM_DMR", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "OTHER", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GEN_RESPONSE", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GEN_OPCODE", sizeof(cur_field_type_param->enums[4].name_from_interface));
    cur_field_type_param->nof_enum_vals = 5;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "OFF", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "INST_INDEX", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "VAR_0_1", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "VAR_2_3", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 24 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "DEFAULT", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BFD_ECHO", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "MAID_48", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BFD_DEFAULT", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "BFD_IPV6", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "BFD_IPV6_MPLS", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "DOWN_MEP", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "DOWN_MEP_MAID_48", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "UP_MEP_SERVER", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "UP_MEP_SERVER_MAID_48", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "1DM", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "1DM_MAID_48", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "LMDM_FLEXIBLE_DA", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "SIGNAL_DETECT", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "SIGNAL_DETECT_MAID_48", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "DM_JUMBO_TLV_MAID_48", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "DM_JUMBO_TLV", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "SEAMLESS_BFD_IPV4", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "SEAMLESS_BFD_IPV6", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "SINGLE_HOP_BFD_RANDOM_DIP", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "BFD_MPLS_TP_VCCV", sizeof(cur_field_type_param->enums[23].name_from_interface));
    cur_field_type_param->nof_enum_vals = 24;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    cur_field_type_param->enums[7].value_from_mapping = 7;
    cur_field_type_param->enums[8].value_from_mapping = 8;
    cur_field_type_param->enums[9].value_from_mapping = 9;
    cur_field_type_param->enums[10].value_from_mapping = 10;
    cur_field_type_param->enums[11].value_from_mapping = 11;
    cur_field_type_param->enums[12].value_from_mapping = 12;
    cur_field_type_param->enums[13].value_from_mapping = 13;
    cur_field_type_param->enums[14].value_from_mapping = 14;
    cur_field_type_param->enums[15].value_from_mapping = 15;
    cur_field_type_param->enums[16].value_from_mapping = 16;
    cur_field_type_param->enums[17].value_from_mapping = 17;
    cur_field_type_param->enums[18].value_from_mapping = 18;
    cur_field_type_param->enums[19].value_from_mapping = 19;
    cur_field_type_param->enums[20].value_from_mapping = 20;
    cur_field_type_param->enums[21].value_from_mapping = 21;
    cur_field_type_param->enums[22].value_from_mapping = 22;
    cur_field_type_param->enums[23].value_from_mapping = 23;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 28 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "DEFAULT", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BFD_ECHO", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "MAID_48", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BFD_IPV6", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BFD_IPV6_MPLS", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BFD_IPV6_JR1", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "BFD_IPV6_MPLS_JR1", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "BFD_ECHO_JR1", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "BFD_DEFAULT", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "DOWN_MEP_ADD_UDH_JR1", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "UP_MEP_SERVER", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "UP_MEP_SERVER_MAID_48", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "1DM", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "LMDM_FLEXIBLE_DA", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48_CCM", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "SIGNAL_DETECT_CCM", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "SIGNAL_DETECT_MAID_48_CCM", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "DM_JUMBO_TLV", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "DM_JUMBO_TLV_RSP", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "SEAMLESS_BFD_IPV4", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "SEAMLESS_BFD_IPV6", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "SEAMLESS_BFD_IPV4_JR1", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "SEAMLESS_BFD_IPV6_JR1", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "SINGLE_HOP_BFD_RANDOM_DIP", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "BFD_MPLS_TP_VCCV", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "INVALID", sizeof(cur_field_type_param->enums[27].name_from_interface));
    cur_field_type_param->nof_enum_vals = 28;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    cur_field_type_param->enums[7].value_from_mapping = 7;
    cur_field_type_param->enums[8].value_from_mapping = 8;
    cur_field_type_param->enums[9].value_from_mapping = 9;
    cur_field_type_param->enums[10].value_from_mapping = 10;
    cur_field_type_param->enums[11].value_from_mapping = 11;
    cur_field_type_param->enums[12].value_from_mapping = 12;
    cur_field_type_param->enums[13].value_from_mapping = 13;
    cur_field_type_param->enums[14].value_from_mapping = 14;
    cur_field_type_param->enums[15].value_from_mapping = 15;
    cur_field_type_param->enums[16].value_from_mapping = 16;
    cur_field_type_param->enums[17].value_from_mapping = 17;
    cur_field_type_param->enums[18].value_from_mapping = 18;
    cur_field_type_param->enums[19].value_from_mapping = 19;
    cur_field_type_param->enums[20].value_from_mapping = 20;
    cur_field_type_param->enums[21].value_from_mapping = 21;
    cur_field_type_param->enums[22].value_from_mapping = 22;
    cur_field_type_param->enums[23].value_from_mapping = 23;
    cur_field_type_param->enums[24].value_from_mapping = 24;
    cur_field_type_param->enums[25].value_from_mapping = 25;
    cur_field_type_param->enums[26].value_from_mapping = 26;
    cur_field_type_param->enums[27].value_from_mapping = 127;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 32 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BFD_ECHO_JR1", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BFD_IPV6_MPLS_JR1", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BFD_IPV6_JR1", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "BFD_IPV6_MPLS", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "BFD_IPV6", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "MAID_48", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "BFD_ECHO", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "BFD_DEFAULT", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "DOWN_MEP_ADD_UDH_JR1", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "DOWN_MEP_MAID48_CCM_UDH_JR1", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "UP_MEP_SERVER", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "UP_MEP_SERVER_MAID_48", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "1DM", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "1DM_MAID_48", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "1DM_MAID_48_CCM", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "LMDM_FLEXIBLE_DA", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "LMDM_FLEXIBLE_DA_MAID_48_CCM", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "SIGNAL_DETECT_CCM", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "SIGNAL_DETECT_MAID_48_CCM", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "DM_JUMBO_TLV", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "DM_JUMBO_TLV_RSP", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "SEAMLESS_BFD_IPV4", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "SEAMLESS_BFD_IPV6", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "SEAMLESS_BFD_IPV4_JR1", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "SEAMLESS_BFD_IPV6_JR1", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "SINGLE_HOP_BFD_RANDOM_DIP", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "BFD_MPLS_TP_VCCV", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "DEFAULT", sizeof(cur_field_type_param->enums[31].name_from_interface));
    cur_field_type_param->nof_enum_vals = 32;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        int enum_index = 0;
        for (enum_index = 0; enum_index < 32; enum_index++)
        {
            cur_field_type_param->enums[enum_index].value_from_mapping = enum_index;
        }
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
