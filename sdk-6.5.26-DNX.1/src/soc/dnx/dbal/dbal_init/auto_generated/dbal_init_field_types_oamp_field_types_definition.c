
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MUX_SRC", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_LSB + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_IN_FIFO, "IN_FIFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TC_DP_LOCAL_PORT, "TC_DP_LOCAL_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_SYS_PORT, "SYS_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_NS_8, "NS_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_0, "NS_16_VAR_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_NS_16_VAR_1, "NS_16_VAR_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_PROG_VAR, "PROG_VAR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TMP_REG1, "TMP_REG1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TMP_REG2, "TMP_REG2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TMP_REG3, "TMP_REG3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TMP_REG4, "TMP_REG4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_FDBK_FIFO, "FDBK_FIFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_INST_VAR, "INST_VAR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_MSB, "TOD_OAM_MSB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TOD_OAM_LSB, "TOD_OAM_LSB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_MSB, "TOD_NTP_MSB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MUX_SRC_TOD_NTP_LSB, "TOD_NTP_LSB");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MERGE_INST", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MERGE_INST_OR + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MERGE_INST_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MERGE_INST_FEM, "FEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MERGE_INST_CONCATENATE, "CONCATENATE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MERGE_INST_OR, "OR");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BUFF_WR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_WR_NEVER, "NEVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_WR_ALWAYS, "ALWAYS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP1, "ON_N_CMP1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_WR_ON_N_CMP2, "ON_N_CMP2");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BUFF_SIZE_SRC", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_BUFF_SIZE_SRC_VLD_B + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG1, "TMP_REG1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG2, "TMP_REG2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG3, "TMP_REG3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_TMP_REG4, "TMP_REG4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_INST_CONST, "INST_CONST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF_SIZE_SRC_VLD_B, "VLD_B");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OP_SEL", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OP_SEL_PKT_LNTH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_TC_DP_LOCAL_PORT, "TC_DP_LOCAL_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_SYS_PORT, "SYS_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_NS_8, "NS_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_0, "NS_16_VAR_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_NS_16_VAR_1, "NS_16_VAR_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_TMP_REG1, "TMP_REG1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_TMP_REG2, "TMP_REG2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_TMP_REG3, "TMP_REG3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_TMP_REG4, "TMP_REG4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_PRG_VAR, "PRG_VAR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_INST_VAR, "INST_VAR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_BYTES_RD, "BYTES_RD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_PDU_OFFS, "PDU_OFFS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_ENC_OFFS, "ENC_OFFS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP_SEL_PKT_LNTH, "PKT_LNTH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ALU_ACT", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ALU_ACT_OR + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_ACT_ADD, "ADD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_ACT_SUB, "SUB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_ACT_AND, "AND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_ACT_OR, "OR");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CMP_ACT", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CMP_ACT_ALWAYS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_NEVER, "NEVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_NEG, "NEG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_ZERO, "ZERO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_POS, "POS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_NNEG, "NNEG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_NPOS, "NPOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CMP_ACT_ALWAYS, "ALWAYS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ALU_DST", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ALU_DST_TMP_REG4 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_DST_TMP_REG1, "TMP_REG1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_DST_TMP_REG2, "TMP_REG2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_DST_TMP_REG3, "TMP_REG3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ALU_DST_TMP_REG4, "TMP_REG4");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BUF_EOP", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_BUF_EOP_ALWAYS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUF_EOP_WR_NEVER, "WR_NEVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP1, "WR_ON_CMP1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUF_EOP_WR_ON_CMP2, "WR_ON_CMP2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUF_EOP_ALWAYS, "ALWAYS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OP3_SEL", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OP3_SEL_INST_VAR_MSB + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_FEM, "FEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_TMP_REG1, "TMP_REG1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_TMP_REG2, "TMP_REG2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_TMP_REG3, "TMP_REG3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_TMP_REG4, "TMP_REG4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_ALU_OUT, "ALU_OUT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_MEM_OUT, "MEM_OUT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_INST_VAR, "INST_VAR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OP3_SEL_INST_VAR_MSB, "INST_VAR_MSB");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INST_SRC", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INST_SRC_LOP_CST + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INST_SRC_LOP_NXT, "LOP_NXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INST_SRC_LOP_JMP, "LOP_JMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INST_SRC_NXT_JMP, "NXT_JMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INST_SRC_LOP_CST, "LOP_CST");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BUFF2_SIZE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_BUFF2_SIZE_4B + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF2_SIZE_1B, "1B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF2_SIZE_2B, "2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF2_SIZE_3B, "3B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BUFF2_SIZE_4B, "4B");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MSG_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MSG_TYPE_GEN_OPCODE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MSG_TYPE_LMM_LMR_SLM_SLR, "LMM_LMR_SLM_SLR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MSG_TYPE_DMM_DMR, "DMM_DMR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MSG_TYPE_OTHER, "OTHER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MSG_TYPE_GEN_RESPONSE, "GEN_RESPONSE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MSG_TYPE_GEN_OPCODE, "GEN_OPCODE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DEBUG_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DEBUG_MODE_VAR_2_3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DEBUG_MODE_OFF, "OFF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DEBUG_MODE_INST_INDEX, "INST_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DEBUG_MODE_VAR_0_1, "VAR_0_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DEBUG_MODE_VAR_2_3, "VAR_2_3");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEM_OP", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    cur_field_type_param->nof_childs = 2;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "FEM_OFFSET");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "FEM_CONST");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEM_OFFSET", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEM_CONST", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_PE_INSTRUCT_INDEX", 11, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_PE_PROGRAM", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LAST_PKT_VARS", 112, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
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
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_PE_PROG_TCAM", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_HEX, 1, FALSE));
    cur_field_type_param->nof_childs = 8;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "MEP_GENERATED");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "MEP_RESPONSE");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 2, "PUNT_PACKET");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 3, "EVENT_PACKET");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 4, "SAT_PACKET");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 5, "DMA_PACKET_1");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 6, "DMA_PACKET_2");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 7, "TXO_PACKET");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        cur_field_type_param->childs[2].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[2].encode_param1 = 2;
    }
    {
        cur_field_type_param->childs[3].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        cur_field_type_param->childs[4].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[4].encode_param1 = 4;
    }
    {
        cur_field_type_param->childs[5].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[5].encode_param1 = 5;
    }
    {
        cur_field_type_param->childs[6].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
        cur_field_type_param->childs[6].encode_param1 = 6;
    }
    {
        cur_field_type_param->childs[7].encode_type = DBAL_VALUE_FIELD_ENCODE_SUFFIX;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_GENERATED", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OAMP_MEP_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEP_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MEP_PE_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MSG_TYPE;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_RESPONSE", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OAMP_MEP_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEP_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MEP_PE_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MSG_TYPE;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PUNT_PACKET", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
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
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EVENT_PACKET", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_EVENT_PACKET_PE_SEL;
    cur_field_types_info->struct_field_info[0].length = 24;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SAT_PACKET", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_SAT_PE_KEY_MSB;
    cur_field_types_info->struct_field_info[0].length = 17;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_GENERATOR_INDEX;
    cur_field_types_info->struct_field_info[1].length = 4;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TXO_PACKET", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OAMP_MEP_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEP_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MEP_PE_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MSG_TYPE;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DMA_PACKET_1", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DMA_PACKET_2", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_PE_PROFILE_SW", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6_NO_CRC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_ECHO, "BFD_ECHO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED, "DOWN_MEP_INJ_SELF_CONTAINED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48, "MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_DEFAULT, "BFD_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6, "BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_MPLS, "BFD_IPV6_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP, "DOWN_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_MAID_48, "DOWN_MEP_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER, "UP_MEP_SERVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_SERVER_MAID_48, "UP_MEP_SERVER_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM, "1DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_1DM_MAID_48, "1DM_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA, "LMDM_FLEXIBLE_DA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_LMDM_FLEXIBLE_DA_MAID_48, "LMDM_FLEXIBLE_DA_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT, "SIGNAL_DETECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SIGNAL_DETECT_MAID_48, "SIGNAL_DETECT_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV_MAID_48, "DM_JUMBO_TLV_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DM_JUMBO_TLV, "DM_JUMBO_TLV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV4, "SEAMLESS_BFD_IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6, "SEAMLESS_BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SINGLE_HOP_BFD_RANDOM_DIP, "SINGLE_HOP_BFD_RANDOM_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV, "BFD_MPLS_TP_VCCV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL, "ADDITIONAL_GAL_SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD, "UP_MEP_DM_TOD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_UP_MEP_DM_TOD_CCM_MAID_48, "UP_MEP_DM_TOD_CCM_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV_FIXED_DIP, "BFD_MPLS_TP_VCCV_FIXED_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_NO_CRC, "BFD_IPV6_NO_CRC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6_NO_CRC, "SEAMLESS_BFD_IPV6_NO_CRC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_MPLS_TP_VCCV_FIXED_DIP].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6_NO_CRC].value_from_mapping = 33;
        enum_info[DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_SEAMLESS_BFD_IPV6_NO_CRC].value_from_mapping = 34;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_PE_PROGRAM_ENUM", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO, "BFD_ECHO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_MAID_48, "MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6, "BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS, "BFD_IPV6_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_JR1, "BFD_IPV6_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_MPLS_JR1, "BFD_IPV6_MPLS_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_ECHO_JR1, "BFD_ECHO_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_DEFAULT, "BFD_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED, "DOWN_MEP_INJ_SELF_CONTAINED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DOWN_MEP_ADD_UDH_JR1, "DOWN_MEP_ADD_UDH_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER, "UP_MEP_SERVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_SERVER_MAID_48, "UP_MEP_SERVER_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_1DM, "1DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA, "LMDM_FLEXIBLE_DA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM, "LMDM_FLEXIBLE_DA_MAID_48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_CCM, "SIGNAL_DETECT_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SIGNAL_DETECT_MAID_48_CCM, "SIGNAL_DETECT_MAID_48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV, "DM_JUMBO_TLV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_DM_JUMBO_TLV_RSP, "DM_JUMBO_TLV_RSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4, "SEAMLESS_BFD_IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6, "SEAMLESS_BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV4_JR1, "SEAMLESS_BFD_IPV4_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_JR1, "SEAMLESS_BFD_IPV6_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP, "SINGLE_HOP_BFD_RANDOM_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV, "BFD_MPLS_TP_VCCV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL, "ADDITIONAL_GAL_SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD, "UP_MEP_DM_TOD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_RSP, "UP_MEP_DM_TOD_RSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID, "UP_MEP_DM_TOD_CCM_48_MAID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV_FIXED_DIP, "BFD_MPLS_TP_VCCV_FIXED_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_NO_CRC, "BFD_IPV6_NO_CRC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_NO_CRC, "SEAMLESS_BFD_IPV6_NO_CRC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV4, "BFD_IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_INVALID, "INVALID");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_MPLS_TP_VCCV_FIXED_DIP].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV6_NO_CRC].value_from_mapping = 33;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_SEAMLESS_BFD_IPV6_NO_CRC].value_from_mapping = 34;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROGRAM_ENUM_BFD_IPV4].value_from_mapping = 35;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_PE_PROG_TCAM_ENUM", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED, "DOWN_MEP_INJ_SELF_CONTAINED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM, "DOWN_MEP_INJ_SELF_CONTAINED_MAID_48_LM_DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO_JR1, "BFD_ECHO_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS_JR1, "BFD_IPV6_MPLS_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_JR1, "BFD_IPV6_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_MPLS, "BFD_IPV6_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6, "BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_MAID_48, "MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_ECHO, "BFD_ECHO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV4_SH, "BFD_IPV4_SH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV4_MH, "BFD_IPV4_MH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT, "BFD_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1, "DOWN_MEP_ADD_UDH_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER, "UP_MEP_SERVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48, "UP_MEP_SERVER_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM, "1DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48, "1DM_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM, "1DM_MAID_48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA, "LMDM_FLEXIBLE_DA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48, "LMDM_FLEXIBLE_DA_MAID_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM, "LMDM_FLEXIBLE_DA_MAID_48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM, "SIGNAL_DETECT_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM, "SIGNAL_DETECT_MAID_48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV, "DM_JUMBO_TLV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP, "DM_JUMBO_TLV_RSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4, "SEAMLESS_BFD_IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6, "SEAMLESS_BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1, "SEAMLESS_BFD_IPV4_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1, "SEAMLESS_BFD_IPV6_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP, "SINGLE_HOP_BFD_RANDOM_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV, "BFD_MPLS_TP_VCCV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL, "ADDITIONAL_GAL_SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM, "ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD, "UP_MEP_DM_TOD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_RSP, "UP_MEP_DM_TOD_RSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID, "UP_MEP_DM_TOD_CCM_48_MAID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM, "ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV_FIXED_DIP, "BFD_MPLS_TP_VCCV_FIXED_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_NO_CRC, "BFD_IPV6_NO_CRC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_NO_CRC, "SEAMLESS_BFD_IPV6_NO_CRC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT, "DEFAULT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV4_SH].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV4_MH].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_DEFAULT].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DOWN_MEP_ADD_UDH_JR1].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_SERVER_MAID_48].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_1DM_MAID_48_CCM].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_LMDM_FLEXIBLE_DA_MAID_48_CCM].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_CCM].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SIGNAL_DETECT_MAID_48_CCM].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DM_JUMBO_TLV_RSP].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV4_JR1].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_JR1].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SINGLE_HOP_BFD_RANDOM_DIP].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV].value_from_mapping = 31;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM].value_from_mapping = 33;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_OTHER].value_from_mapping = 34;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_CCM].value_from_mapping = 35;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_1DM_MAID48_LMM].value_from_mapping = 37;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48_CCM].value_from_mapping = 38;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_MAID48].value_from_mapping = 39;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_RSP].value_from_mapping = 41;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_UP_MEP_DM_TOD_CCM_48_MAID].value_from_mapping = 42;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV].value_from_mapping = 43;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_RSP].value_from_mapping = 44;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_OTHER].value_from_mapping = 45;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_CCM].value_from_mapping = 46;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48].value_from_mapping = 47;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_RSP].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_ADDITIONAL_GAL_SPECIAL_LABEL_DM_JUMBO_TLV_MAID48_LMM].value_from_mapping = 49;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_MPLS_TP_VCCV_FIXED_DIP].value_from_mapping = 50;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_BFD_IPV6_NO_CRC].value_from_mapping = 51;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_SEAMLESS_BFD_IPV6_NO_CRC].value_from_mapping = 52;
        enum_info[DBAL_ENUM_FVAL_OAMP_PE_PROG_TCAM_ENUM_DEFAULT].value_from_mapping = 53;
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
