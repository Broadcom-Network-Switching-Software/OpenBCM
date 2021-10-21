
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_prp2_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRP2_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRP2_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPFIX_RX + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___NOP, "GENERAL___NOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CPU_PORT, "GENERAL___CPU_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CPU_PORT_CANCEL_SYSHDR_ENC, "GENERAL___CPU_PORT_CANCEL_SYSHDR_ENC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___RAW_PORT_CANCEL_SYSHDR_ENC, "GENERAL___RAW_PORT_CANCEL_SYSHDR_ENC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CANCEL_SYSHDR_ENC, "GENERAL___CANCEL_SYSHDR_ENC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CANCEL_SYSHDR_LIF_PORT_BASED_LIF, "GENERAL___CANCEL_SYSHDR_LIF_PORT_BASED_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___NOP_ETH, "GENERAL___NOP_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___JR1_IP_FORWARD_CODE_FIX, "GENERAL___JR1_IP_FORWARD_CODE_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___JR1_IP_FORWARD_CODE_FIX, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, "GENERAL___SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, "GENERAL___SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, "GENERAL___SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPV6_PARTIAL_TERMINATION, "GENERAL___IPV6_PARTIAL_TERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPV6_PARTIAL_TERMINATION, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___RFC8321_IPV6_ON_FLOW_ID, "GENERAL___RFC8321_IPV6_ON_FLOW_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_OAM___MPLS_DM_INJECTION, "OAM___MPLS_DM_INJECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPFIX_RX, "GENERAL___IPFIX_RX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPFIX_RX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
    }
    if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___NOP].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CPU_PORT_CANCEL_SYSHDR_ENC].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___RAW_PORT_CANCEL_SYSHDR_ENC].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CANCEL_SYSHDR_ENC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CANCEL_SYSHDR_LIF_PORT_BASED_LIF].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___NOP_ETH].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___JR1_IP_FORWARD_CODE_FIX].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPV6_PARTIAL_TERMINATION].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___RFC8321_IPV6_ON_FLOW_ID].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_OAM___MPLS_DM_INJECTION].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___NOP].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CPU_PORT_CANCEL_SYSHDR_ENC].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___RAW_PORT_CANCEL_SYSHDR_ENC].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CANCEL_SYSHDR_ENC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___CANCEL_SYSHDR_LIF_PORT_BASED_LIF].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___NOP_ETH].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___JR1_IP_FORWARD_CODE_FIX].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPV6_PARTIAL_TERMINATION].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___RFC8321_IPV6_ON_FLOW_ID].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_OAM___MPLS_DM_INJECTION].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_PRP2_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_term_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TERM_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TERM_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION, "GENERAL___HEADER_TERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_BIER___ANY_BFR, "BIER___ANY_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_AND_POP, "GENERAL___HEADER_TERMINATION_AND_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP, "GENERAL___HEADER_TERMINATION_DEEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP_UP_MEP, "GENERAL___HEADER_TERMINATION_DEEP_UP_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MPLS_INJECTED_FROM_OAMP, "GENERAL___MPLS_INJECTED_FROM_OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION, "GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___TERMINATE_UP_TO_PSO, "GENERAL___TERMINATE_UP_TO_PSO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1, "GENERAL___OAM_TX_INJECT_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, "GENERAL___SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, "GENERAL___SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING, "TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, "GENERAL___SFLOW_EXT_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS, "TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, "GENERAL___SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, "GENERAL___IPT_IFA2_0_INTERMEDIATE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA, "GENERAL___IPT_IFA");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, "GENERAL___IPT_IFA_FIRST_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, "GENERAL___IPT_IFA2_0_INGRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___BUILD_FTMH_FROM_PIPE, "GENERAL___BUILD_FTMH_FROM_PIPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___ETH_INTO_COE, "TM___ETH_INTO_COE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH, "TM___MACT_JR2_DSP_CMD_TO_REFRESH");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD, "GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT, "SRV6___PSP_EXTENDED_CUT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588, "GENERAL___1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM, "GENERAL___OAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INT, "GENERAL___INT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL, "GENERAL___IPT_TAIL");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___RAW_PROC, "GENERAL___RAW_PROC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX, "GENERAL___IPFIX_RX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX, "GENERAL___IPFIX_TX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INGRESS_TRAPPED_OAMP, "GENERAL___INGRESS_TRAPPED_OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___CPU_PORT, "GENERAL___CPU_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MIRROR_OR_SS, "GENERAL___MIRROR_OR_SS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___DM_CPU_PORT, "GENERAL___DM_CPU_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD, "MPLS___TRANSIT_CCM_SD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL, "MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS, "MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS, "MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP, "MPLS___PHP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP_1588, "MPLS___PHP_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV4_1ST_PASS, "REFLECTORS___TWAMPOIPV4_1ST_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV6_1ST_PASS, "REFLECTORS___TWAMPOIPV6_1ST_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET, "GENERAL___SFLOW_SEQUENCE_RESET");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS, "REFLECTORS___TWAMP_2ND_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___L2_OAM_TERM, "REFLECTORS___L2_OAM_TERM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IP_UDP_SWAP, "REFLECTORS___IP_UDP_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IPV6_UDP_SWAP, "REFLECTORS___IPV6_UDP_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___UDP_SWAP, "REFLECTORS___UDP_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_IPV6, "REFLECTORS___SEAMLESS_BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS, "SRV6___EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD, "REFLECTORS___SEAMLESS_BFD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT, "SRV6___SEGMENT_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP, "SRV6___PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___TX_INJECT, "TM___TX_INJECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_UDP, "IPV4___HEADER_TERMINATION_NAT_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_TCP, "IPV4___HEADER_TERMINATION_NAT_TCP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS, "SRV6___T_OR_B_INSERT_EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IOAM_JP, "GENERAL___IOAM_JP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION, "GENERAL___1588_HEADER_SIZE_COMPENSATION");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_BIER___ANY_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_AND_POP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP_UP_MEP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MPLS_INJECTED_FROM_OAMP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___TERMINATE_UP_TO_PSO].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___BUILD_FTMH_FROM_PIPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INGRESS_TRAPPED_OAMP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MIRROR_OR_SS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___DM_CPU_PORT].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP_1588].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV4_1ST_PASS].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV6_1ST_PASS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___L2_OAM_TERM].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IP_UDP_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IPV6_UDP_SWAP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___UDP_SWAP].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_IPV6].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___TX_INJECT].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_UDP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_TCP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_BIER___ANY_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_AND_POP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP_UP_MEP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MPLS_INJECTED_FROM_OAMP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___TERMINATE_UP_TO_PSO].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___BUILD_FTMH_FROM_PIPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INGRESS_TRAPPED_OAMP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MIRROR_OR_SS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___DM_CPU_PORT].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP_1588].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV4_1ST_PASS].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV6_1ST_PASS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___L2_OAM_TERM].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IP_UDP_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IPV6_UDP_SWAP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___UDP_SWAP].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_IPV6].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___TX_INJECT].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_UDP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_TCP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION].value_from_mapping = 19;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_BIER___ANY_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_AND_POP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP_UP_MEP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MPLS_INJECTED_FROM_OAMP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___TERMINATE_UP_TO_PSO].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___BUILD_FTMH_FROM_PIPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INGRESS_TRAPPED_OAMP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MIRROR_OR_SS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___DM_CPU_PORT].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP_1588].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV4_1ST_PASS].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV6_1ST_PASS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___L2_OAM_TERM].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IP_UDP_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IPV6_UDP_SWAP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___UDP_SWAP].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_IPV6].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___TX_INJECT].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_TCP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_BIER___ANY_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_AND_POP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP_UP_MEP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MPLS_INJECTED_FROM_OAMP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___TERMINATE_UP_TO_PSO].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___BUILD_FTMH_FROM_PIPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INGRESS_TRAPPED_OAMP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MIRROR_OR_SS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___DM_CPU_PORT].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP_1588].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV4_1ST_PASS].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV6_1ST_PASS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___L2_OAM_TERM].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IP_UDP_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IPV6_UDP_SWAP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___UDP_SWAP].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_IPV6].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___TX_INJECT].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_UDP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_TCP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_BIER___ANY_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_AND_POP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___HEADER_TERMINATION_DEEP_UP_MEP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MPLS_INJECTED_FROM_OAMP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___TERMINATE_UP_TO_PSO].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM_TX_INJECT_JR1].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___BUILD_FTMH_FROM_PIPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___MACT_JR2_DSP_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___OAM].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___INGRESS_TRAPPED_OAMP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___CPU_PORT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___MIRROR_OR_SS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___DM_CPU_PORT].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_MPLS___PHP_1588].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV4_1ST_PASS].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMPOIPV6_1ST_PASS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___SFLOW_SEQUENCE_RESET].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___TWAMP_2ND_PASS].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___L2_OAM_TERM].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IP_UDP_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___IPV6_UDP_SWAP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___UDP_SWAP].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_IPV6].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___SEGMENT_ENDPOINT].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___PSP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_TM___TX_INJECT].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_UDP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_IPV4___HEADER_TERMINATION_NAT_TCP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___IOAM_JP].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_TERM_CONTEXT_ID_GENERAL___1588_HEADER_SIZE_COMPENSATION].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_fwd_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___MPLS_BFR, "BIER___MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___NON_MPLS_BFR, "BIER___NON_MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING, "BRIDGE___ETPP_FORWARDING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_ETPS_POP, "BRIDGE___ETPP_FORWARDING_WO_ETPS_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS, "TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_VSD_POP, "BRIDGE___ETPP_FORWARDING_WO_VSD_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA, "GENERAL___BFD_PWE_CW_WA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, "GENERAL___SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH, "TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, "GENERAL___SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH, "TM___MACT_JR1_DSP_CMD_TO_REFRESH");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, "GENERAL___SFLOW_EXT_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, "GENERAL___SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, "GENERAL___IPT_IFA2_0_INTERMEDIATE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA, "GENERAL___IPT_IFA");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, "GENERAL___IPT_IFA_FIRST_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, "GENERAL___IPT_IFA2_0_INGRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___ETH_INTO_COE, "TM___ETH_INTO_COE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD, "GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT, "SRV6___PSP_EXTENDED_CUT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT, "GENERAL___DO_NOT_EDIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT_AND_POP_ETPS, "GENERAL___DO_NOT_EDIT_AND_POP_ETPS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM, "GENERAL___IPT_TAIL_AM");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL, "GENERAL___IPT_TAIL");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___FORWARDING, "IPV4___FORWARDING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___FORWARDING, "IPV6___FORWARDING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP, "GENERAL___NOP_AND_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2, "GENERAL___NOP_AND_POP_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX, "GENERAL___IPFIX_RX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX, "GENERAL___IPFIX_TX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___RAW_PROC, "GENERAL___RAW_PROC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP, "IPV4___ETPP_FORWARDING_WO_ETPS_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1, "IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP, "IPV6___ETPP_FORWARDING_WO_ETPS_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1, "IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP, "MPLS___SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAPTOSELF, "MPLS___SWAPTOSELF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___PHP, "MPLS___PHP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___OAM_REFLECTOR, "OAM___OAM_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___UP_MEP_COE, "OAM___UP_MEP_COE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BNG___NOP_CTX, "BNG___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP, "REFLECTORS___L2_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR, "REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP, "REFLECTORS___L2_SWAP_WITH_IPV6_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE, "REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR, "REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV6_SWAP, "REFLECTORS___IPV6_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS, "SRV6___EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA, "REFLECTORS___L2_SWAP_AND_MC_SA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV4_SWAP, "REFLECTORS___IPV4_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_OVER_MPLS, "REFLECTORS___SEAMLESS_BFD_OVER_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV4, "REFLECTORS___TWAMP_IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV6, "REFLECTORS___TWAMP_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT, "SRV6___SRV6_SEGMENT_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP, "SRV6___SRV6_SEGMENT_ENDPOINT_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___NAT_EDITING, "IPV4___NAT_EDITING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___BFD_IPV4_EXTRA_SIP, "IPV4___BFD_IPV4_EXTRA_SIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP_WO_ETPS_POP, "MPLS___SWAP_WO_ETPS_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_IPV6, "SRV6___T_INSERT_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT, "SRV6___T_INSERT_SRV6_ENPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT_PSP, "SRV6___T_INSERT_SRV6_ENPOINT_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS, "SRV6___T_OR_B_INSERT_EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP, "SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IOAM_JP, "GENERAL___IOAM_JP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_SVTAG, "BRIDGE___ETPP_FORWARDING_SVTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG, "GENERAL___NOP_ADD_SVTAG");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_VSD_POP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT_AND_POP_ETPS].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___FORWARDING].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___FORWARDING].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAPTOSELF].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___PHP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___UP_MEP_COE].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BNG___NOP_CTX].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV6_SWAP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV4_SWAP].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_OVER_MPLS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV4].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV6].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___NAT_EDITING].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___BFD_IPV4_EXTRA_SIP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP_WO_ETPS_POP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_IPV6].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT_PSP].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_VSD_POP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT_AND_POP_ETPS].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___FORWARDING].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___FORWARDING].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAPTOSELF].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___PHP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___UP_MEP_COE].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BNG___NOP_CTX].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV6_SWAP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV4_SWAP].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_OVER_MPLS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV4].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV6].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___NAT_EDITING].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___BFD_IPV4_EXTRA_SIP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP_WO_ETPS_POP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_IPV6].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT_PSP].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_SVTAG].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG].value_from_mapping = 19;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_VSD_POP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT_AND_POP_ETPS].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___FORWARDING].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___FORWARDING].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAPTOSELF].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___PHP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___UP_MEP_COE].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BNG___NOP_CTX].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV6_SWAP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV4_SWAP].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_OVER_MPLS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV4].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV6].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___NAT_EDITING].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___BFD_IPV4_EXTRA_SIP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP_WO_ETPS_POP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_IPV6].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT_PSP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_VSD_POP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT_AND_POP_ETPS].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___FORWARDING].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___FORWARDING].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAPTOSELF].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___PHP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___UP_MEP_COE].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BNG___NOP_CTX].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV6_SWAP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV4_SWAP].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_OVER_MPLS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV4].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV6].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___NAT_EDITING].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___BFD_IPV4_EXTRA_SIP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP_WO_ETPS_POP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_IPV6].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT_PSP].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IOAM_JP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_WO_VSD_POP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___MACT_JR1_DSP_CMD_TO_REFRESH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_TM___ETH_INTO_COE].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___PSP_EXTENDED_CUT].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___DO_NOT_EDIT_AND_POP_ETPS].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL_AM].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPT_TAIL].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___FORWARDING].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___FORWARDING].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_AND_POP_2].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IPFIX_TX].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___RAW_PROC].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAPTOSELF].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___PHP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_OAM___UP_MEP_COE].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BNG___NOP_CTX].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_INTERNAL_REFLECTOR].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_WITH_IPV6_SWAP_COE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA_INTERNAL_REFLECTOR].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV6_SWAP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___EXTENDED_PASS].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___L2_SWAP_AND_MC_SA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___IPV4_SWAP].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___SEAMLESS_BFD_OVER_MPLS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV4].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_REFLECTORS___TWAMP_IPV6].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___SRV6_SEGMENT_ENDPOINT_PSP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___NAT_EDITING].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_IPV4___BFD_IPV4_EXTRA_SIP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_MPLS___SWAP_WO_ETPS_POP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_IPV6].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_SRV6_ENPOINT_PSP].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_SRV6___T_INSERT_NO_SIDS_SRV6_ENPOINT_PSP].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___IOAM_JP].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_BRIDGE___ETPP_FORWARDING_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD_CONTEXT_ID_GENERAL___NOP_ADD_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_1_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ENCAP_1_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ENCAP_1_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFR_NON_MPLS_INITIAL_4B, "BIER___BFR_NON_MPLS_INITIAL_4B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_256BSL, "BIER___BFIR_256BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_128BSL, "BIER___BFIR_128BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_64BSL, "BIER___BFIR_64BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_WO_PPP, "BNG___PPPOE_WO_PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_W_PPP, "BNG___PPPOE_W_PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_WO_PPP, "BNG___L2TP_WO_PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, "GENERAL___SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, "GENERAL___SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, "GENERAL___SFLOW_EXT_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, "GENERAL___SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, "GENERAL___IPT_IFA2_0_INTERMEDIATE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA, "GENERAL___IPT_IFA");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, "GENERAL___IPT_IFA_FIRST_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, "GENERAL___IPT_IFA2_0_INGRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___1_MPLS_W_AH, "MPLS___1_MPLS_W_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_W_PPP, "BNG___L2TP_W_PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC, "BRIDGE___ETH_COMPATIBLE_MC_BASIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_1_AH, "MPLS___2_MPLS_1_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_COMPATIBLE_MC_WO_ETPS_POP, "BRIDGE___IN_ETH_COMPATIBLE_MC_WO_ETPS_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_W_1_ETPS_POP, "BRIDGE___IN_ETH_W_1_ETPS_POP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH, "BRIDGE___ARP_AC_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH, "BRIDGE___ARP_PLUS_AC_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_2_AH, "MPLS___2_MPLS_2_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_3_AH, "MPLS___2_MPLS_3_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_AND_POP_CTX, "GENERAL___NOP_AND_POP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE, "GENERAL___RAW_DATA_AT_VAR_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IMPLICIT_NULL, "MPLS___IMPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1, "MPLS___MPLS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2, "MPLS___MPLS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_RFC8321, "MPLS___MPLS_2_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS1, "MPLS___MPLS_1PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS2, "MPLS___MPLS_1PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1, "MPLS___MPLS_2PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321, "MPLS___MPLS_2PLUS1_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___PUSH_TWO_MPLS_LABELS_FROM_UDH, "MPLS___PUSH_TWO_MPLS_LABELS_FROM_UDH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2, "MPLS___MPLS_2PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321, "MPLS___MPLS_2PLUS2_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_WITH_ESI, "MPLS___EVPN_WITH_ESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_NO_ESI, "MPLS___EVPN_NO_ESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS, "SRV6___ENCAP_SEGMENT_EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS_REDUCED, "SRV6___ENCAP_SEGMENT_EXTENDED_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS_REDUCED, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS, "SRV6___ENCAP_SEGMENT_MAIN_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS_REDUCED, "SRV6___ENCAP_SEGMENT_MAIN_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS_REDUCED, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS, "SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS_REDUCED, "SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS_REDUCED, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS, "SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS_REDUCED, "SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS_REDUCED, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV2, "TELEMETRY___ERSPANV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV3, "TELEMETRY___ERSPANV3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___4B_LAWFUL_INTERCEPT, "TELEMETRY___4B_LAWFUL_INTERCEPT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPFIX_RX, "GENERAL___IPFIX_RX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPFIX_RX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_IPV4___NAT_BUILD_IP, "IPV4___NAT_BUILD_IP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_BOS_LABEL_USED_IN_FWD_AS_SWAP, "MPLS___MPLS_2PLUS1_BOS_LABEL_USED_IN_FWD_AS_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_BOS_LABEL_USED_IN_FWD_AS_SWAP, "MPLS___MPLS_2PLUS2_BOS_LABEL_USED_IN_FWD_AS_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_BOS_LABEL_USED_IN_FWD_AS_SWAP, "MPLS___MPLS_2_BOS_LABEL_USED_IN_FWD_AS_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IFIT_MPLS, "MPLS___IFIT_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS, "SRV6___B6_INSERT_SEGMENT_MAIN_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS_REDUCED, "SRV6___B6_INSERT_SEGMENT_MAIN_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID, "SRV6___ENCAP_T_INSERT_ENCAP_SID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID_REDUCED, "SRV6___ENCAP_T_INSERT_ENCAP_SID_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS, "SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS_REDUCED, "SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS, "SRV6___T_INSERT_SEGMENT_EXTENDED_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS_REDUCED, "SRV6___T_INSERT_SEGMENT_EXTENDED_PASS_REDUCED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC_SVTAG, "BRIDGE___ETH_COMPATIBLE_MC_BASIC_SVTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG, "BRIDGE___ARP_AC_ETH_SVTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG, "BRIDGE___ARP_PLUS_AC_ETH_SVTAG");
    }
    if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_WO_PPP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_W_PPP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_WO_PPP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_W_PPP].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_COMPATIBLE_MC_WO_ETPS_POP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_W_1_ETPS_POP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_AND_POP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_RFC8321].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___PUSH_TWO_MPLS_LABELS_FROM_UDH].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_WITH_ESI].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_NO_ESI].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS_REDUCED].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS_REDUCED].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS_REDUCED].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV3].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___4B_LAWFUL_INTERCEPT].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_IPV4___NAT_BUILD_IP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_BOS_LABEL_USED_IN_FWD_AS_SWAP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_BOS_LABEL_USED_IN_FWD_AS_SWAP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_BOS_LABEL_USED_IN_FWD_AS_SWAP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IFIT_MPLS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS_REDUCED].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID_REDUCED].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC_SVTAG].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG].value_from_mapping = 26;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_WO_PPP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_W_PPP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_WO_PPP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___1_MPLS_W_AH].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_W_PPP].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_1_AH].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_COMPATIBLE_MC_WO_ETPS_POP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_W_1_ETPS_POP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_2_AH].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_3_AH].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_AND_POP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_RFC8321].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___PUSH_TWO_MPLS_LABELS_FROM_UDH].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_WITH_ESI].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_NO_ESI].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS_REDUCED].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS_REDUCED].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS_REDUCED].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV3].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___4B_LAWFUL_INTERCEPT].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_IPV4___NAT_BUILD_IP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_BOS_LABEL_USED_IN_FWD_AS_SWAP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_BOS_LABEL_USED_IN_FWD_AS_SWAP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_BOS_LABEL_USED_IN_FWD_AS_SWAP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IFIT_MPLS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS_REDUCED].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID_REDUCED].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS_REDUCED].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS_REDUCED].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_WO_PPP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___PPPOE_W_PPP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_WO_PPP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BNG___L2TP_W_PPP].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_COMPATIBLE_MC_WO_ETPS_POP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___IN_ETH_W_1_ETPS_POP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___NOP_AND_POP_CTX].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_RFC8321].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___PUSH_TWO_MPLS_LABELS_FROM_UDH].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_WITH_ESI].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___EVPN_NO_ESI].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_MAIN_PASS_REDUCED].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_PASS_REDUCED].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_SEGMENT_VPN_ENHANCED_NO_SIDS_PASS_REDUCED].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___ERSPANV3].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_TELEMETRY___4B_LAWFUL_INTERCEPT].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_GENERAL___IPFIX_RX].value_from_mapping = 63;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_IPV4___NAT_BUILD_IP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS1_BOS_LABEL_USED_IN_FWD_AS_SWAP].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2PLUS2_BOS_LABEL_USED_IN_FWD_AS_SWAP].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___MPLS_2_BOS_LABEL_USED_IN_FWD_AS_SWAP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_MPLS___IFIT_MPLS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_MAIN_PASS_REDUCED].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___ENCAP_T_INSERT_ENCAP_SID_REDUCED].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___B6_INSERT_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_SRV6___T_INSERT_SEGMENT_EXTENDED_PASS_REDUCED].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ETH_COMPATIBLE_MC_BASIC_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_1_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_2_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ENCAP_2_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ENCAP_2_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B, "BIER___BFIR_NON_MPLS_INITIAL_4B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_256BSL, "BIER___BFIR_256BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_128BSL, "BIER___BFIR_128BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_64BSL, "BIER___BFIR_64BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE, "GENERAL___RAW_DATA_AT_VAR_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL, "IPV4___TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___1_MPLS_W_AH, "MPLS___1_MPLS_W_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, "GENERAL___SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, "GENERAL___SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, "GENERAL___SFLOW_EXT_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, "GENERAL___SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, "GENERAL___IPT_IFA2_0_INTERMEDIATE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA, "GENERAL___IPT_IFA");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, "GENERAL___IPT_IFA_FIRST_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, "GENERAL___IPT_IFA2_0_INGRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_1_AH, "MPLS___2_MPLS_1_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN, "IPV4___IPV4_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN, "IPV4___IPV4_VXLAN_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_2_AH, "MPLS___2_MPLS_2_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI, "IPV4___IPV4_GRE_WO_TNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_UDP, "IPV4___IPV4_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE, "IPV4___IPV4_GRE8_GRE12_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL, "IPV6___TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL_ECN, "IPV6___TUNNEL_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___VXLAN_ONLY, "IPV6___VXLAN_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___IMPLICIT_NULL, "MPLS___IMPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1, "MPLS___MPLS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1_W_AH, "MPLS___MPLS_1_W_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2, "MPLS___MPLS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2_RFC8321, "MPLS___MPLS_2_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_3_AH, "MPLS___2_MPLS_3_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS1, "MPLS___MPLS_1PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS2, "MPLS___MPLS_1PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1, "MPLS___MPLS_2PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321, "MPLS___MPLS_2PLUS1_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2, "MPLS___MPLS_2PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321, "MPLS___MPLS_2PLUS2_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___EVPN_WITH_ESI, "MPLS___EVPN_WITH_ESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___UDP_ONLY, "IPV6___UDP_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GENEVE_ONLY, "IPV6___GENEVE_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY, "IPV6___GRE_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI, "IPV6___GRE_ONLY_WO_TNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_2_SEGMENTS, "SRV6___ENCAP_2_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_2_SEGMENTS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_1_SEGMENT, "SRV6___ENCAP_1_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_1_SEGMENT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_0_SEGMENTS, "SRV6___ENCAP_0_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_0_SEGMENTS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_UPF___GTPV1U_GPDU_HEADER, "UPF___GTPV1U_GPDU_HEADER");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_UPF___GTPV1U_GPDU_HEADER, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___NAT_BUILD_IP, "IPV4___NAT_BUILD_IP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_2_SEGMENTS, "SRV6___ENCAP_T_INSERT_2_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_1_SEGMENT, "SRV6___ENCAP_T_INSERT_1_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_0_SEGMENTS, "SRV6___ENCAP_T_INSERT_0_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL_COPY_INNER, "IPV4___TUNNEL_COPY_INNER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN, "IPV4___IPV4_DOUBLE_UDP_VXLAN");
    }
    if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___VXLAN_ONLY].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1_W_AH].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2_RFC8321].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___EVPN_WITH_ESI].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___UDP_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GENEVE_ONLY].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_2_SEGMENTS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_1_SEGMENT].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_0_SEGMENTS].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_UPF___GTPV1U_GPDU_HEADER].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___NAT_BUILD_IP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_2_SEGMENTS].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_1_SEGMENT].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_0_SEGMENTS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL_COPY_INNER].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN].value_from_mapping = 20;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___1_MPLS_W_AH].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_1_AH].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_2_AH].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___VXLAN_ONLY].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1_W_AH].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2_RFC8321].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_3_AH].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___EVPN_WITH_ESI].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___UDP_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GENEVE_ONLY].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_2_SEGMENTS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_1_SEGMENT].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_0_SEGMENTS].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_UPF___GTPV1U_GPDU_HEADER].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___NAT_BUILD_IP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_2_SEGMENTS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_1_SEGMENT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_0_SEGMENTS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___VXLAN_ONLY].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1_W_AH].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2_RFC8321].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_MPLS___EVPN_WITH_ESI].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___UDP_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GENEVE_ONLY].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_2_SEGMENTS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_1_SEGMENT].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_0_SEGMENTS].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_UPF___GTPV1U_GPDU_HEADER].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___NAT_BUILD_IP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_2_SEGMENTS].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_1_SEGMENT].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_SRV6___ENCAP_T_INSERT_0_SEGMENTS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_2_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_3_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ENCAP_3_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ENCAP_3_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B, "BIER___BFIR_NON_MPLS_INITIAL_4B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_256BSL, "BIER___BFIR_256BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_128BSL, "BIER___BFIR_128BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_64BSL, "BIER___BFIR_64BSL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE, "GENERAL___RAW_DATA_AT_VAR_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___TUNNEL, "IPV4___TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN, "IPV4___IPV4_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, "GENERAL___SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, "GENERAL___SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, "GENERAL___SFLOW_EXT_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, "GENERAL___SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, "GENERAL___IPT_IFA2_0_INTERMEDIATE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA, "GENERAL___IPT_IFA");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, "GENERAL___IPT_IFA_FIRST_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, "GENERAL___IPT_IFA2_0_INGRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___1_MPLS_W_AH, "MPLS___1_MPLS_W_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN, "IPV4___IPV4_VXLAN_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_1_AH, "MPLS___2_MPLS_1_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI, "IPV4___IPV4_GRE_WO_TNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_UDP, "IPV4___IPV4_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE, "IPV4___IPV4_GRE8_GRE12_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL, "IPV6___TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL_ECN, "IPV6___TUNNEL_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___VXLAN_ONLY, "IPV6___VXLAN_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GENEVE_ONLY, "IPV6___GENEVE_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY, "IPV6___GRE_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI, "IPV6___GRE_ONLY_WO_TNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___UDP_ONLY, "IPV6___UDP_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___IMPLICIT_NULL, "MPLS___IMPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1, "MPLS___MPLS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2, "MPLS___MPLS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_2_AH, "MPLS___2_MPLS_2_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_3_AH, "MPLS___2_MPLS_3_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS1, "MPLS___MPLS_1PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS2, "MPLS___MPLS_1PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1, "MPLS___MPLS_2PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321, "MPLS___MPLS_2PLUS1_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2, "MPLS___MPLS_2PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321, "MPLS___MPLS_2PLUS2_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___TWO_SEGMENTS, "SRV6___TWO_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___TWO_SEGMENTS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ONE_SEGMENT, "SRV6___ONE_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ONE_SEGMENT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ZERO_SEGMENTS, "SRV6___ZERO_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ZERO_SEGMENTS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_2_SEGMENTS, "SRV6___SRH_AND_2_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_2_SEGMENTS, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_1_SEGMENT, "SRV6___SRH_AND_1_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_1_SEGMENT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_0_SEGMENT, "SRV6___SRH_AND_0_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_0_SEGMENT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___KEEP_ORIGINAL_ETH, "IPV4___KEEP_ORIGINAL_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_2_SEGMENTS, "SRV6___T_INSERT_SRH_AND_2_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_1_SEGMENT, "SRV6___T_INSERT_SRH_AND_1_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_0_SEGMENT, "SRV6___T_INSERT_SRH_AND_0_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_TWO_SEGMENTS, "SRV6___T_INSERT_TWO_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ONE_SEGMENT, "SRV6___T_INSERT_ONE_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ZERO_SEGMENTS, "SRV6___T_INSERT_ZERO_SEGMENTS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_1_SEGMENT, "SRV6___VPN_ENHANCED_SRH_AND_1_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_0_SEGMENT, "SRV6___VPN_ENHANCED_SRH_AND_0_SEGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN, "IPV4___IPV4_DOUBLE_UDP_VXLAN");
    }
    if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___VXLAN_ONLY].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GENEVE_ONLY].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___UDP_ONLY].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___TWO_SEGMENTS].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ONE_SEGMENT].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ZERO_SEGMENTS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_2_SEGMENTS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_1_SEGMENT].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_0_SEGMENT].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___KEEP_ORIGINAL_ETH].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_2_SEGMENTS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_1_SEGMENT].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_0_SEGMENT].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_TWO_SEGMENTS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ONE_SEGMENT].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ZERO_SEGMENTS].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_1_SEGMENT].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_0_SEGMENT].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN].value_from_mapping = 19;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___1_MPLS_W_AH].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_1_AH].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___VXLAN_ONLY].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GENEVE_ONLY].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___UDP_ONLY].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_2_AH].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_3_AH].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___TWO_SEGMENTS].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ONE_SEGMENT].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ZERO_SEGMENTS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_2_SEGMENTS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_1_SEGMENT].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_0_SEGMENT].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___KEEP_ORIGINAL_ETH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_2_SEGMENTS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_1_SEGMENT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_0_SEGMENT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_TWO_SEGMENTS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ONE_SEGMENT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ZERO_SEGMENTS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_1_SEGMENT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_0_SEGMENT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_256BSL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_128BSL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_BIER___BFIR_64BSL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_DP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_HDR_SAMP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_FIRST].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___SFLOW_EXT_SECOND].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_EGRESS_NO_MD].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_GENERAL___IPT_IFA2_0_INGRESS].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___VXLAN_ONLY].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GENEVE_ONLY].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___GRE_ONLY_WO_TNI].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV6___UDP_ONLY].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS1_RFC8321].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_MPLS___MPLS_2PLUS2_RFC8321].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___TWO_SEGMENTS].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ONE_SEGMENT].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___ZERO_SEGMENTS].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_2_SEGMENTS].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_1_SEGMENT].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___SRH_AND_0_SEGMENT].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___KEEP_ORIGINAL_ETH].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_2_SEGMENTS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_1_SEGMENT].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_SRH_AND_0_SEGMENT].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_TWO_SEGMENTS].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ONE_SEGMENT].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___T_INSERT_ZERO_SEGMENTS].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_1_SEGMENT].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_SRV6___VPN_ENHANCED_SRH_AND_0_SEGMENT].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ENCAP_3_CONTEXT_ID_IPV4___IPV4_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_4_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ENCAP_4_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ENCAP_4_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS_MIDDLE_OR_LAST + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B, "BIER___BFIR_NON_MPLS_INITIAL_4B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE, "GENERAL___RAW_DATA_AT_VAR_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___TUNNEL, "IPV4___TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_VXLAN, "IPV4___IPV4_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN, "IPV4___IPV4_VXLAN_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI, "IPV4___IPV4_GRE_WO_TNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_UDP, "IPV4___IPV4_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE, "IPV4___IPV4_GRE8_GRE12_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV6___TUNNEL, "IPV6___TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV6___TUNNEL_ECN, "IPV6___TUNNEL_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___IMPLICIT_NULL, "MPLS___IMPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1, "MPLS___MPLS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___1_MPLS_W_AH, "MPLS___1_MPLS_W_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2, "MPLS___MPLS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_1_AH, "MPLS___2_MPLS_1_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_2_AH, "MPLS___2_MPLS_2_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_3_AH, "MPLS___2_MPLS_3_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1PLUS1, "MPLS___MPLS_1PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1PLUS2, "MPLS___MPLS_1PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2PLUS1, "MPLS___MPLS_2PLUS1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2PLUS2, "MPLS___MPLS_2PLUS2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___SRV6_IPV6_HEADER, "SRV6___SRV6_IPV6_HEADER");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___SRV6_IPV6_HEADER, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_FIRST, "SRV6___EXTENDED_PASS_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_FIRST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_MIDDLE, "SRV6___EXTENDED_PASS_MIDDLE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_MIDDLE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_LAST, "SRV6___EXTENDED_PASS_LAST");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_LAST, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_INSERT, "SRV6___T_INSERT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_INSERT_EXTENDED_PASS_FIRST, "SRV6___T_INSERT_EXTENDED_PASS_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___B6_INSERT_EXTENDED_PASS_FIRST, "SRV6___B6_INSERT_EXTENDED_PASS_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS_MIDDLE_OR_LAST, "SRV6___T_OR_B_INSERT_EXTENDED_PASS_MIDDLE_OR_LAST");
    }
    if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___1_MPLS_W_AH].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_1_AH].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_2_AH].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_3_AH].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___SRV6_IPV6_HEADER].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_FIRST].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_MIDDLE].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_LAST].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_INSERT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_INSERT_EXTENDED_PASS_FIRST].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___B6_INSERT_EXTENDED_PASS_FIRST].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS_MIDDLE_OR_LAST].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_BIER___BFIR_NON_MPLS_INITIAL_4B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_GENERAL___RAW_DATA_AT_VAR_SIZE].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___TUNNEL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_VXLAN].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_VXLAN_ECN].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_GRE_WO_TNI].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_UDP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV4___IPV4_GRE8_GRE12_GENEVE].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV6___TUNNEL].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_IPV6___TUNNEL_ECN].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___IMPLICIT_NULL].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___1_MPLS_W_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___2_MPLS_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1PLUS1].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_1PLUS2].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2PLUS1].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_MPLS___MPLS_2PLUS2].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___SRV6_IPV6_HEADER].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_FIRST].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_MIDDLE].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___EXTENDED_PASS_LAST].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_INSERT].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_INSERT_EXTENDED_PASS_FIRST].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___B6_INSERT_EXTENDED_PASS_FIRST].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ENCAP_4_CONTEXT_ID_SRV6___T_OR_B_INSERT_EXTENDED_PASS_MIDDLE_OR_LAST].value_from_mapping = 17;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_5_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ENCAP_5_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ENCAP_5_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_AC_ETH, "BRIDGE___ARP_AC_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH, "BRIDGE___ARP_PLUS_AC_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_FCOE___NPV_US, "FCOE___NPV_US");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___RCH_ENC, "GENERAL___RCH_ENC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG, "BRIDGE___ARP_AC_ETH_SVTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG, "BRIDGE___ARP_PLUS_AC_ETH_SVTAG");
    }
    if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_AC_ETH].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_FCOE___NPV_US].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___RCH_ENC].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG].value_from_mapping = 4;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_AC_ETH].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_FCOE___NPV_US].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_GENERAL___RCH_ENC].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_AC_ETH_SVTAG].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ENCAP_5_CONTEXT_ID_BRIDGE___ARP_PLUS_AC_ETH_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_trap_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TRAP_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TRAP_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___STAMP_OTMH_OUTLIF_EXT_IF_MC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RAW_PORT_APPEND_64B, "GENERAL___RAW_PORT_APPEND_64B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA, "GENERAL___BFD_PWE_CW_WA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___TRJ_TRACE_TYPE2, "GENERAL___TRJ_TRACE_TYPE2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_DROP_AND_CONT, "GENERAL___RCH_DROP_AND_CONT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_EXT_ENCAP, "GENERAL___RCH_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_XTERMINATION, "GENERAL___RCH_XTERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PUNT_W_PTCH1, "GENERAL___PUNT_W_PTCH1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PTCH1_FROM_FTMH_SSPA, "GENERAL___PTCH1_FROM_FTMH_SSPA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PUNT_W_PTCH2, "GENERAL___PUNT_W_PTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RECYCLE_W_PTCH2, "GENERAL___RECYCLE_W_PTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___OAM_UPMEP_REFLECTOR, "GENERAL___OAM_UPMEP_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NORMAL_FWD, "GENERAL___NORMAL_FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NORMAL_FWD_NO_MIRROR, "GENERAL___NORMAL_FWD_NO_MIRROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___DROP_PKT, "GENERAL___DROP_PKT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___VISIBILITY, "GENERAL___VISIBILITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___CPU_PORT_TRAPPED, "GENERAL___CPU_PORT_TRAPPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_1, "GENERAL___USER_TRAP_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_2, "GENERAL___USER_TRAP_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_3, "GENERAL___USER_TRAP_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_4, "GENERAL___USER_TRAP_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___EGRESS_TRAPPED, "GENERAL___EGRESS_TRAPPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, "GENERAL___IPT_IFA_FIRST_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___JR1_COMP_MACT_LEARNING_JR2_TO_JR1, "TM___JR1_COMP_MACT_LEARNING_JR2_TO_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___JR1_COMP_MACT_LEARNING_JR2_TO_JR1, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_VRF_REDIRECT, "GENERAL___RCH_VRF_REDIRECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___DM_CPU_PORT_TRAPPED, "GENERAL___DM_CPU_PORT_TRAPPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_OAMP, "OAM___OAM_DM_TO_OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_CPU, "OAM___OAM_DM_TO_CPU");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_FPGA, "OAM___OAM_DM_TO_FPGA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_OAMP, "OAM___OAM_LM_TO_OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_CPU, "OAM___OAM_LM_TO_CPU");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_FPGA, "OAM___OAM_LM_TO_FPGA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_OAMP, "OAM___OAM_CCM_TO_OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_CPU, "OAM___OAM_CCM_TO_CPU");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_FPGA, "OAM___OAM_CCM_TO_FPGA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LVLERR, "OAM___OAM_LVLERR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_PSVERR, "OAM___OAM_PSVERR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_REFLECTOR, "OAM___OAM_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___BFDV6_CHECKSUM_ERROR, "OAM___BFDV6_CHECKSUM_ERROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP, "SRV6___RCH_SRV6_USP_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TELEMETRY___IFA_EGRESS_PORT, "TELEMETRY___IFA_EGRESS_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___RAW_PORT, "TM___RAW_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___DEFAULT, "TM___DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_NO_OTMH, "TM___TDM_NO_OTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OTMH, "TM___TDM_STAMP_MC_CUD_ON_OTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OTMH, "TM___TDM_STAMP_PORT_VAL_ON_OTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OTMH, "TM___TDM_STAMP_NOTHING_ON_OTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_FTMH, "TM___TDM_STAMP_MC_CUD_ON_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_FTMH, "TM___TDM_STAMP_PORT_VAL_ON_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_FTMH, "TM___TDM_STAMP_NOTHING_ON_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OPT_FTMH, "TM___TDM_STAMP_MC_CUD_ON_OPT_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OPT_FTMH, "TM___TDM_STAMP_PORT_VAL_ON_OPT_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OPT_FTMH, "TM___TDM_STAMP_NOTHING_ON_OPT_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___STAMP_OTMH_OUTLIF_EXT_IF_MC, "TM___STAMP_OTMH_OUTLIF_EXT_IF_MC");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RAW_PORT_APPEND_64B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___TRJ_TRACE_TYPE2].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_DROP_AND_CONT].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_EXT_ENCAP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_XTERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PUNT_W_PTCH1].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PTCH1_FROM_FTMH_SSPA].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PUNT_W_PTCH2].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RECYCLE_W_PTCH2].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___OAM_UPMEP_REFLECTOR].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NORMAL_FWD].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NORMAL_FWD_NO_MIRROR].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___DROP_PKT].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___VISIBILITY].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___CPU_PORT_TRAPPED].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_1].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_2].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_3].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_4].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___EGRESS_TRAPPED].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___JR1_COMP_MACT_LEARNING_JR2_TO_JR1].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_VRF_REDIRECT].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___DM_CPU_PORT_TRAPPED].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_OAMP].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_CPU].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_FPGA].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_OAMP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_CPU].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_FPGA].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_OAMP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_CPU].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_FPGA].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LVLERR].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_PSVERR].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___BFDV6_CHECKSUM_ERROR].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TELEMETRY___IFA_EGRESS_PORT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___RAW_PORT].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___DEFAULT].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_NO_OTMH].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OTMH].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OTMH].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OTMH].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_FTMH].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_FTMH].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_FTMH].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OPT_FTMH].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OPT_FTMH].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OPT_FTMH].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___STAMP_OTMH_OUTLIF_EXT_IF_MC].value_from_mapping = 52;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RAW_PORT_APPEND_64B].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___BFD_PWE_CW_WA].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___TRJ_TRACE_TYPE2].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_DROP_AND_CONT].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_EXT_ENCAP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_XTERMINATION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PUNT_W_PTCH1].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PTCH1_FROM_FTMH_SSPA].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___PUNT_W_PTCH2].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RECYCLE_W_PTCH2].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___OAM_UPMEP_REFLECTOR].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NORMAL_FWD].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___NORMAL_FWD_NO_MIRROR].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___DROP_PKT].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___VISIBILITY].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___CPU_PORT_TRAPPED].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_1].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_2].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_3].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___USER_TRAP_4].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___EGRESS_TRAPPED].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___IPT_IFA_FIRST_DP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___JR1_COMP_MACT_LEARNING_JR2_TO_JR1].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___RCH_VRF_REDIRECT].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_GENERAL___DM_CPU_PORT_TRAPPED].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_OAMP].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_CPU].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_DM_TO_FPGA].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_OAMP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_CPU].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LM_TO_FPGA].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_OAMP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_CPU].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_CCM_TO_FPGA].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_LVLERR].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_PSVERR].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___OAM_REFLECTOR].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_OAM___BFDV6_CHECKSUM_ERROR].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TELEMETRY___IFA_EGRESS_PORT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___RAW_PORT].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___DEFAULT].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_NO_OTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_FTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_FTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_FTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_MC_CUD_ON_OPT_FTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_PORT_VAL_ON_OPT_FTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___TDM_STAMP_NOTHING_ON_OPT_FTMH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_TRAP_CONTEXT_ID_TM___STAMP_OTMH_OUTLIF_EXT_IF_MC].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_prp2_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_term_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_fwd_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_1_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_2_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_3_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_4_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_encap_5_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_egress_ctx_id_field_types_definition_trap_context_id_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
