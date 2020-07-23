
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PRP2_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 11 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "GENERAL___CPU_PORT", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "GENERAL___CPU_PORT_CANCEL_SYSHDR_ENC", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "GENERAL___RAW_PORT_CANCEL_SYSHDR_ENC", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___CANCEL_SYSHDR_ENC", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___CANCEL_SYSHDR_LIF_PORT_BASED_LIF", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___NOP", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___JR1_IP_FORWARD_CODE_FIX", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "OAM___MPLS_DM_INJECTION", sizeof(cur_field_type_param->enums[10].name_from_interface));
    cur_field_type_param->nof_enum_vals = 11;
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TERM_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 56 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "GENERAL___HEADER_TERMINATION", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "GENERAL___HEADER_TERMINATION_AND_POP", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "GENERAL___HEADER_TERMINATION_DEEP", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___MPLS_INJECTED_FROM_OAMP", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___W_FWD_PLUS_1_REMARK_HEADER_TERMINATION", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___TERMINATE_UP_TO_PSO", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___BUILD_FTMH_FROM_PIPE", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___1588", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___OAM", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___OAM_TX_INJECT_JR1", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "GENERAL___INT", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GENERAL___IPT_TAIL", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "GENERAL___IPT_IFA", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "GENERAL___IPT_IFA_FIRST_DP", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "GENERAL___IPT_IFA2_0_INGRESS", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "GENERAL___IPT_UDP_GPE", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "GENERAL___IPT_FIRST_UDP_GPE", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "GENERAL___IPT_LAST_UDP_GPE", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "GENERAL___RAW_PROC", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "GENERAL___INGRESS_TRAPPED_OAMP", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "GENERAL___CPU_PORT", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "GENERAL___MIRROR_OR_SS", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "MPLS___TRANSIT_CCM_SD", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "IPV4___HEADER_TERMINATION_NAT_UDP", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "IPV4___HEADER_TERMINATION_NAT_TCP", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "MPLS___TRANSIT_CCM_SD_1_ADDITIONAL_LABEL", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "MPLS___TRANSIT_CCM_SD_2_ADDITIONAL_LABELS", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "MPLS___TRANSIT_CCM_SD_3_ADDITIONAL_LABELS", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "MPLS___PHP", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "MPLS___PHP_1588", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "REFLECTORS___TWAMPOIPV4_1ST_PASS", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "REFLECTORS___TWAMPOIPV6_1ST_PASS", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "REFLECTORS___TWAMP_2ND_PASS", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "REFLECTORS___L2_OAM_TERM", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "REFLECTORS___IP_UDP_SWAP", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "GENERAL___SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "GENERAL___SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "GENERAL___SFLOW_EXT_FIRST", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "GENERAL___SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "REFLECTORS___IPV6_UDP_SWAP", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "REFLECTORS___UDP_SWAP", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "REFLECTORS___SEAMLESS_BFD", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "SRV6___SEGMENT_ENDPOINT", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "SRV6___PSP", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "SRV6___IPV6_TERM_INTO_SRV6", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "SRV6___UNIFIED_SEGMENT_ENDPOINT", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "TM___TX_INJECT", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "TM___MACT_JR1_DSP_DEST_SWITCH_BETWEEN_JR1_ARAD_ENCODING", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "TM___MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "TM___MACT_JR2_DSP_CMD_TO_REFRESH", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "TM___ETH_INTO_COE", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "GENERAL___IOAM_JP", sizeof(cur_field_type_param->enums[55].name_from_interface));
    cur_field_type_param->nof_enum_vals = 56;
    if (DBAL_IS_J2C_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[23].value_from_mapping = 24;
        cur_field_type_param->enums[24].value_from_mapping = 25;
        cur_field_type_param->enums[25].value_from_mapping = 26;
        cur_field_type_param->enums[26].value_from_mapping = 27;
        cur_field_type_param->enums[27].value_from_mapping = 28;
        cur_field_type_param->enums[28].value_from_mapping = 29;
        cur_field_type_param->enums[29].value_from_mapping = 30;
        cur_field_type_param->enums[30].value_from_mapping = 31;
        cur_field_type_param->enums[31].value_from_mapping = 32;
        cur_field_type_param->enums[32].value_from_mapping = 33;
        cur_field_type_param->enums[33].value_from_mapping = 34;
        cur_field_type_param->enums[34].value_from_mapping = 35;
        cur_field_type_param->enums[35].value_from_mapping = 36;
        cur_field_type_param->enums[36].value_from_mapping = 37;
        cur_field_type_param->enums[37].value_from_mapping = 38;
        cur_field_type_param->enums[38].value_from_mapping = 39;
        cur_field_type_param->enums[39].value_from_mapping = 40;
        cur_field_type_param->enums[40].value_from_mapping = 41;
        cur_field_type_param->enums[41].value_from_mapping = 42;
        cur_field_type_param->enums[42].value_from_mapping = 43;
        cur_field_type_param->enums[43].value_from_mapping = 44;
        cur_field_type_param->enums[44].value_from_mapping = 45;
        cur_field_type_param->enums[45].value_from_mapping = 46;
        cur_field_type_param->enums[46].value_from_mapping = 47;
        cur_field_type_param->enums[47].value_from_mapping = 48;
        cur_field_type_param->enums[48].value_from_mapping = 49;
        cur_field_type_param->enums[49].value_from_mapping = 50;
        cur_field_type_param->enums[50].value_from_mapping = 51;
        cur_field_type_param->enums[51].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[52].value_from_mapping = 53;
        cur_field_type_param->enums[53].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[54].value_from_mapping = 55;
        cur_field_type_param->enums[55].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[23].value_from_mapping = 24;
        cur_field_type_param->enums[24].value_from_mapping = 25;
        cur_field_type_param->enums[25].value_from_mapping = 26;
        cur_field_type_param->enums[26].value_from_mapping = 27;
        cur_field_type_param->enums[27].value_from_mapping = 28;
        cur_field_type_param->enums[28].value_from_mapping = 29;
        cur_field_type_param->enums[29].value_from_mapping = 30;
        cur_field_type_param->enums[30].value_from_mapping = 31;
        cur_field_type_param->enums[31].value_from_mapping = 32;
        cur_field_type_param->enums[32].value_from_mapping = 33;
        cur_field_type_param->enums[33].value_from_mapping = 34;
        cur_field_type_param->enums[34].value_from_mapping = 35;
        cur_field_type_param->enums[35].value_from_mapping = 36;
        cur_field_type_param->enums[36].value_from_mapping = 37;
        cur_field_type_param->enums[37].value_from_mapping = 38;
        cur_field_type_param->enums[38].value_from_mapping = 39;
        cur_field_type_param->enums[39].value_from_mapping = 40;
        cur_field_type_param->enums[40].value_from_mapping = 41;
        cur_field_type_param->enums[41].value_from_mapping = 42;
        cur_field_type_param->enums[42].value_from_mapping = 43;
        cur_field_type_param->enums[43].value_from_mapping = 44;
        cur_field_type_param->enums[44].value_from_mapping = 45;
        cur_field_type_param->enums[45].value_from_mapping = 46;
        cur_field_type_param->enums[46].value_from_mapping = 47;
        cur_field_type_param->enums[47].value_from_mapping = 48;
        cur_field_type_param->enums[48].value_from_mapping = 49;
        cur_field_type_param->enums[49].value_from_mapping = 50;
        cur_field_type_param->enums[50].value_from_mapping = 51;
        cur_field_type_param->enums[51].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[52].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[53].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[54].value_from_mapping = 55;
        cur_field_type_param->enums[55].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        
        
        
        
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
        cur_field_type_param->enums[23].value_from_mapping = 24;
        cur_field_type_param->enums[24].value_from_mapping = 25;
        cur_field_type_param->enums[25].value_from_mapping = 26;
        cur_field_type_param->enums[26].value_from_mapping = 27;
        cur_field_type_param->enums[27].value_from_mapping = 28;
        cur_field_type_param->enums[28].value_from_mapping = 29;
        cur_field_type_param->enums[29].value_from_mapping = 30;
        cur_field_type_param->enums[30].value_from_mapping = 31;
        cur_field_type_param->enums[31].value_from_mapping = 32;
        cur_field_type_param->enums[32].value_from_mapping = 33;
        cur_field_type_param->enums[33].value_from_mapping = 34;
        cur_field_type_param->enums[34].value_from_mapping = 35;
        cur_field_type_param->enums[35].value_from_mapping = 36;
        cur_field_type_param->enums[36].value_from_mapping = 37;
        cur_field_type_param->enums[37].value_from_mapping = 38;
        cur_field_type_param->enums[38].value_from_mapping = 39;
        cur_field_type_param->enums[39].value_from_mapping = 40;
        cur_field_type_param->enums[40].value_from_mapping = 41;
        cur_field_type_param->enums[41].value_from_mapping = 42;
        cur_field_type_param->enums[42].value_from_mapping = 43;
        cur_field_type_param->enums[43].value_from_mapping = 44;
        cur_field_type_param->enums[44].value_from_mapping = 45;
        cur_field_type_param->enums[45].value_from_mapping = 46;
        cur_field_type_param->enums[46].value_from_mapping = 47;
        cur_field_type_param->enums[47].value_from_mapping = 48;
        cur_field_type_param->enums[48].value_from_mapping = 49;
        cur_field_type_param->enums[49].value_from_mapping = 50;
        cur_field_type_param->enums[50].value_from_mapping = 51;
        cur_field_type_param->enums[51].value_from_mapping = 52;
        cur_field_type_param->enums[52].value_from_mapping = 53;
        cur_field_type_param->enums[53].value_from_mapping = 54;
        cur_field_type_param->enums[54].value_from_mapping = 55;
        cur_field_type_param->enums[55].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
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
        cur_field_type_param->enums[23].value_from_mapping = 24;
        cur_field_type_param->enums[24].value_from_mapping = 25;
        cur_field_type_param->enums[25].value_from_mapping = 26;
        cur_field_type_param->enums[26].value_from_mapping = 27;
        cur_field_type_param->enums[27].value_from_mapping = 28;
        cur_field_type_param->enums[28].value_from_mapping = 29;
        cur_field_type_param->enums[29].value_from_mapping = 30;
        cur_field_type_param->enums[30].value_from_mapping = 31;
        cur_field_type_param->enums[31].value_from_mapping = 32;
        cur_field_type_param->enums[32].value_from_mapping = 33;
        cur_field_type_param->enums[33].value_from_mapping = 34;
        cur_field_type_param->enums[34].value_from_mapping = 35;
        cur_field_type_param->enums[35].value_from_mapping = 36;
        cur_field_type_param->enums[36].value_from_mapping = 37;
        cur_field_type_param->enums[37].value_from_mapping = 38;
        cur_field_type_param->enums[38].value_from_mapping = 39;
        cur_field_type_param->enums[39].value_from_mapping = 40;
        cur_field_type_param->enums[40].value_from_mapping = 41;
        cur_field_type_param->enums[41].value_from_mapping = 42;
        cur_field_type_param->enums[42].value_from_mapping = 43;
        cur_field_type_param->enums[43].value_from_mapping = 44;
        cur_field_type_param->enums[44].value_from_mapping = 45;
        cur_field_type_param->enums[45].value_from_mapping = 46;
        cur_field_type_param->enums[46].value_from_mapping = 47;
        cur_field_type_param->enums[47].value_from_mapping = 48;
        cur_field_type_param->enums[48].value_from_mapping = 49;
        cur_field_type_param->enums[49].value_from_mapping = 50;
        cur_field_type_param->enums[50].value_from_mapping = 51;
        cur_field_type_param->enums[51].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[52].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[53].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[54].value_from_mapping = 55;
        cur_field_type_param->enums[55].value_from_mapping = 23;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 64 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___TI_FORWARDING", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BRIDGE___ETPP_FORWARDING", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BRIDGE___ETPP_FORWARDING_WO_ETPS_POP", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___BFD_PWE_CW_WA", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___DO_NOT_EDIT", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___DO_NOT_EDIT_AND_POP_ETPS", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___DO_NOT_EDIT_AND_POP_VSD", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___IPT_TAIL_AM", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___IPT_TAIL", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "GENERAL___IPT_IFA", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GENERAL___IPT_IFA_FIRST_DP", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE_NO_MD", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "GENERAL___IPT_IFA2_0_INGRESS", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD_FORWARD", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "GENERAL___IPT_UDP_GPE", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "GENERAL___IPT_FIRST_UDP_GPE", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "GENERAL___IPT_LAST_UDP_GPE", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "GENERAL___RAW_PROC", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "GENERAL___NOP_AND_POP_2", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "GENERAL___NOP_AND_POP", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "IPV4___ETPP_FORWARDING_WO_ETPS_POP", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IPV4___FORWARDING", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "IPV6___FORWARDING", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IPV4___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "IPV4___NAT_EDITING", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "IPV6___ETPP_FORWARDING_WO_ETPS_POP", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "IPV6___ETPP_FORWARDING_WO_ETPS_POP_REMARK_TOS_P1", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "MPLS___SWAP", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "MPLS___SWAPTOSELF", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "MPLS___SWAP_COUPLED", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "MPLS___PHP", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "OAM___OAM_REFLECTOR", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "OAM___UP_MEP_COE", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "GENERAL___SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "GENERAL___SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "GENERAL___SFLOW_EXT_FIRST", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "GENERAL___SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "PPP___NOP_CTX", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "REFLECTORS___L2_SWAP", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "REFLECTORS___L2_SWAP_WITH_IPV6_SWAP", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "REFLECTORS___L2_SWAP_AND_MC_SA", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "REFLECTORS___IPV4_SWAP", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "REFLECTORS___SEAMLESS_BFD_OVER_MPLS", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "REFLECTORS___IPV6_SWAP", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "REFLECTORS___TWAMP_IPV4", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "REFLECTORS___TWAMP_IPV6", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "SRV6___IPV6_FWD_INTO_SRV6", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "SRV6___T_INSERT", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "SRV6___SRV6_EXT_ENCAP", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "SRV6___SRV6_SEGMENT_ENDPOINT", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "SRV6___SRV6_SEGMENT_ENDPOINT_PSP", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "SRV6___SRV6_USID_SEGMENT_ENDPOINT", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "SRV6___SRV6_UNIFIED_SEGMENT_ENDPOINT", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "TM___MACT_JR1_DSP_NULLIFY_KEY_MSBS", sizeof(cur_field_type_param->enums[55].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[56].name_from_interface, "TM___ETH_INTO_COE", sizeof(cur_field_type_param->enums[56].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[57].name_from_interface, "TM___MACT_JR1_DSP_COPY_DSP_LSBS_AND_CMD_TO_REFRESH", sizeof(cur_field_type_param->enums[57].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[58].name_from_interface, "TM___MACT_JR1_DSP_CMD_TO_REFRESH", sizeof(cur_field_type_param->enums[58].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[59].name_from_interface, "IPV4___BFD_SIP_WA", sizeof(cur_field_type_param->enums[59].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[60].name_from_interface, "MPLS___SWAP_WO_ETPS_POP", sizeof(cur_field_type_param->enums[60].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[61].name_from_interface, "GENERAL___IOAM_JP", sizeof(cur_field_type_param->enums[61].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[62].name_from_interface, "BRIDGE___ETPP_FORWARDING_SVTAG", sizeof(cur_field_type_param->enums[62].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[63].name_from_interface, "GENERAL___NOP_ADD_SVTAG", sizeof(cur_field_type_param->enums[63].name_from_interface));
    cur_field_type_param->nof_enum_vals = 64;
    if (DBAL_IS_J2C_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 2;
        cur_field_type_param->enums[3].value_from_mapping = 4;
        cur_field_type_param->enums[4].value_from_mapping = 5;
        cur_field_type_param->enums[5].value_from_mapping = 6;
        cur_field_type_param->enums[6].value_from_mapping = 7;
        cur_field_type_param->enums[7].value_from_mapping = 8;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].value_from_mapping = 20;
        cur_field_type_param->enums[19].value_from_mapping = 22;
        cur_field_type_param->enums[20].value_from_mapping = 23;
        cur_field_type_param->enums[21].value_from_mapping = 24;
        cur_field_type_param->enums[22].value_from_mapping = 25;
        cur_field_type_param->enums[23].value_from_mapping = 26;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 30;
        cur_field_type_param->enums[27].value_from_mapping = 31;
        cur_field_type_param->enums[28].value_from_mapping = 32;
        cur_field_type_param->enums[29].value_from_mapping = 33;
        cur_field_type_param->enums[30].value_from_mapping = 35;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 51;
        cur_field_type_param->enums[47].value_from_mapping = 52;
        cur_field_type_param->enums[48].value_from_mapping = 53;
        cur_field_type_param->enums[49].value_from_mapping = 54;
        cur_field_type_param->enums[50].value_from_mapping = 55;
        cur_field_type_param->enums[51].value_from_mapping = 56;
        cur_field_type_param->enums[52].value_from_mapping = 57;
        cur_field_type_param->enums[53].value_from_mapping = 58;
        cur_field_type_param->enums[54].value_from_mapping = 59;
        cur_field_type_param->enums[55].value_from_mapping = 60;
        cur_field_type_param->enums[56].value_from_mapping = 61;
        cur_field_type_param->enums[57].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[58].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[59].value_from_mapping = 29;
        cur_field_type_param->enums[60].value_from_mapping = 34;
        cur_field_type_param->enums[61].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[62].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[63].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 2;
        cur_field_type_param->enums[3].value_from_mapping = 4;
        cur_field_type_param->enums[4].value_from_mapping = 5;
        cur_field_type_param->enums[5].value_from_mapping = 6;
        cur_field_type_param->enums[6].value_from_mapping = 7;
        cur_field_type_param->enums[7].value_from_mapping = 8;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].value_from_mapping = 20;
        cur_field_type_param->enums[19].value_from_mapping = 22;
        cur_field_type_param->enums[20].value_from_mapping = 23;
        cur_field_type_param->enums[21].value_from_mapping = 24;
        cur_field_type_param->enums[22].value_from_mapping = 25;
        cur_field_type_param->enums[23].value_from_mapping = 26;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 30;
        cur_field_type_param->enums[27].value_from_mapping = 31;
        cur_field_type_param->enums[28].value_from_mapping = 32;
        cur_field_type_param->enums[29].value_from_mapping = 33;
        cur_field_type_param->enums[30].value_from_mapping = 35;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 51;
        cur_field_type_param->enums[47].value_from_mapping = 52;
        cur_field_type_param->enums[48].value_from_mapping = 53;
        cur_field_type_param->enums[49].value_from_mapping = 54;
        cur_field_type_param->enums[50].value_from_mapping = 55;
        cur_field_type_param->enums[51].value_from_mapping = 56;
        cur_field_type_param->enums[52].value_from_mapping = 57;
        cur_field_type_param->enums[53].value_from_mapping = 58;
        cur_field_type_param->enums[54].value_from_mapping = 59;
        cur_field_type_param->enums[55].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[56].value_from_mapping = 61;
        cur_field_type_param->enums[57].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[58].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[59].value_from_mapping = 29;
        cur_field_type_param->enums[60].value_from_mapping = 34;
        cur_field_type_param->enums[61].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[62].value_from_mapping = 3;
        cur_field_type_param->enums[63].value_from_mapping = 9;
    }
    else if (DBAL_IS_JR2_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 2;
        cur_field_type_param->enums[3].value_from_mapping = 4;
        cur_field_type_param->enums[4].value_from_mapping = 5;
        cur_field_type_param->enums[5].value_from_mapping = 6;
        cur_field_type_param->enums[6].value_from_mapping = 7;
        cur_field_type_param->enums[7].value_from_mapping = 8;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].value_from_mapping = 20;
        cur_field_type_param->enums[19].value_from_mapping = 22;
        cur_field_type_param->enums[20].value_from_mapping = 23;
        cur_field_type_param->enums[21].value_from_mapping = 24;
        cur_field_type_param->enums[22].value_from_mapping = 25;
        cur_field_type_param->enums[23].value_from_mapping = 26;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 30;
        cur_field_type_param->enums[27].value_from_mapping = 31;
        cur_field_type_param->enums[28].value_from_mapping = 32;
        cur_field_type_param->enums[29].value_from_mapping = 33;
        cur_field_type_param->enums[30].value_from_mapping = 35;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 51;
        cur_field_type_param->enums[47].value_from_mapping = 52;
        cur_field_type_param->enums[48].value_from_mapping = 53;
        cur_field_type_param->enums[49].value_from_mapping = 54;
        cur_field_type_param->enums[50].value_from_mapping = 55;
        cur_field_type_param->enums[51].value_from_mapping = 56;
        cur_field_type_param->enums[52].value_from_mapping = 57;
        cur_field_type_param->enums[53].value_from_mapping = 58;
        cur_field_type_param->enums[54].value_from_mapping = 59;
        cur_field_type_param->enums[55].value_from_mapping = 60;
        cur_field_type_param->enums[56].value_from_mapping = 61;
        cur_field_type_param->enums[57].value_from_mapping = 62;
        cur_field_type_param->enums[58].value_from_mapping = 63;
        cur_field_type_param->enums[59].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[60].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[61].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[62].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[63].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_JR2_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 2;
        cur_field_type_param->enums[3].value_from_mapping = 4;
        cur_field_type_param->enums[4].value_from_mapping = 5;
        cur_field_type_param->enums[5].value_from_mapping = 6;
        cur_field_type_param->enums[6].value_from_mapping = 7;
        cur_field_type_param->enums[7].value_from_mapping = 8;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].value_from_mapping = 20;
        cur_field_type_param->enums[19].value_from_mapping = 22;
        cur_field_type_param->enums[20].value_from_mapping = 23;
        cur_field_type_param->enums[21].value_from_mapping = 24;
        cur_field_type_param->enums[22].value_from_mapping = 25;
        cur_field_type_param->enums[23].value_from_mapping = 26;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 30;
        cur_field_type_param->enums[27].value_from_mapping = 31;
        cur_field_type_param->enums[28].value_from_mapping = 32;
        cur_field_type_param->enums[29].value_from_mapping = 33;
        cur_field_type_param->enums[30].value_from_mapping = 35;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 51;
        cur_field_type_param->enums[47].value_from_mapping = 52;
        cur_field_type_param->enums[48].value_from_mapping = 53;
        cur_field_type_param->enums[49].value_from_mapping = 54;
        cur_field_type_param->enums[50].value_from_mapping = 55;
        cur_field_type_param->enums[51].value_from_mapping = 56;
        cur_field_type_param->enums[52].value_from_mapping = 57;
        cur_field_type_param->enums[53].value_from_mapping = 58;
        cur_field_type_param->enums[54].value_from_mapping = 59;
        cur_field_type_param->enums[55].value_from_mapping = 60;
        cur_field_type_param->enums[56].value_from_mapping = 61;
        cur_field_type_param->enums[57].value_from_mapping = 62;
        cur_field_type_param->enums[58].value_from_mapping = 63;
        cur_field_type_param->enums[59].value_from_mapping = 29;
        cur_field_type_param->enums[60].value_from_mapping = 34;
        cur_field_type_param->enums[61].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[62].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[63].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 2;
        cur_field_type_param->enums[3].value_from_mapping = 4;
        cur_field_type_param->enums[4].value_from_mapping = 5;
        cur_field_type_param->enums[5].value_from_mapping = 6;
        cur_field_type_param->enums[6].value_from_mapping = 7;
        cur_field_type_param->enums[7].value_from_mapping = 8;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].value_from_mapping = 20;
        cur_field_type_param->enums[19].value_from_mapping = 22;
        cur_field_type_param->enums[20].value_from_mapping = 23;
        cur_field_type_param->enums[21].value_from_mapping = 24;
        cur_field_type_param->enums[22].value_from_mapping = 25;
        cur_field_type_param->enums[23].value_from_mapping = 26;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 30;
        cur_field_type_param->enums[27].value_from_mapping = 31;
        cur_field_type_param->enums[28].value_from_mapping = 32;
        cur_field_type_param->enums[29].value_from_mapping = 33;
        cur_field_type_param->enums[30].value_from_mapping = 35;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 51;
        cur_field_type_param->enums[47].value_from_mapping = 52;
        cur_field_type_param->enums[48].value_from_mapping = 53;
        cur_field_type_param->enums[49].value_from_mapping = 54;
        cur_field_type_param->enums[50].value_from_mapping = 55;
        cur_field_type_param->enums[51].value_from_mapping = 56;
        cur_field_type_param->enums[52].value_from_mapping = 57;
        cur_field_type_param->enums[53].value_from_mapping = 58;
        cur_field_type_param->enums[54].value_from_mapping = 59;
        cur_field_type_param->enums[55].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[56].value_from_mapping = 61;
        cur_field_type_param->enums[57].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[58].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[59].value_from_mapping = 29;
        cur_field_type_param->enums[60].value_from_mapping = 34;
        cur_field_type_param->enums[61].value_from_mapping = 21;
        cur_field_type_param->enums[62].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[63].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ENCAP_1_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 51 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BRIDGE___ETH_COMPATIBLE_MC_BASIC", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BRIDGE___IN_ETH_COMPATIBLE_MC_WO_ETPS_POP", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BRIDGE___IN_ETH_W_1_ETPS_POP", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BRIDGE___ARP_AC_ETH", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BRIDGE___ARP_PLUS_AC_ETH", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___NOP_AND_POP_CTX", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___IPT_IFA_FIRST_DP", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___IPT_IFA", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___IPT_IFA2_0_INGRESS", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "GENERAL___RAW_DATA_AT_VAR_SIZE", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "IPV4___NAT_BUILD_IP", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "L2TPV2___WO_PPP", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "L2TPV2___W_PPP", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "MPLS___IMPLICIT_NULL_WO_AH", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "MPLS___MPLS_1", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "MPLS___1_MPLS_W_AH", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "MPLS___MPLS_2", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "MPLS___MPLS_2_RFC8321", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "MPLS___2_MPLS_1_AH", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "MPLS___2_MPLS_2_AH", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "MPLS___2_MPLS_3_AH", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "MPLS___MPLS_1PLUS1", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "MPLS___MPLS_1PLUS2", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "MPLS___MPLS_2PLUS1", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "MPLS___MPLS_2PLUS1_RFC8321", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "MPLS___PUSH_TWO_MPLS_LABELS_FROM_UDH", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "MPLS___MPLS_2PLUS2", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "MPLS___MPLS_2PLUS2_RFC8321", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "MPLS___EVPN_WITH_ESI", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "MPLS___EVPN_NO_ESI", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "PPPOE___WO_PPP", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "PPPOE___W_PPP", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "GENERAL___SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "GENERAL___SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "GENERAL___SFLOW_EXT_FIRST", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "GENERAL___SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "SRV6___ENCAP_SEGMENT_N_TH", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "TELEMETRY___ERSPANV2", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "TELEMETRY___ERSPANV3", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "TELEMETRY___4B_LAWFUL_INTERCEPT", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "TELEMETRY___IPFIX_PSAMP__OVER_UDP_OR_TCP", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "TELEMETRY___INT_P4_TBD", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "TELEMETRY___INT_FB_TBD", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "MPLS___MPLS_2_BOS_LABEL_USED_IN_FWD_AS_SWAP", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "MPLS___MPLS_2PLUS1_BOS_LABEL_USED_IN_FWD_AS_SWAP", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "MPLS___MPLS_2PLUS2_BOS_LABEL_USED_IN_FWD_AS_SWAP", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "BRIDGE___ARP_AC_ETH_SVTAG", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "BRIDGE___ARP_PLUS_AC_ETH_SVTAG", sizeof(cur_field_type_param->enums[50].name_from_interface));
    cur_field_type_param->nof_enum_vals = 51;
    if (DBAL_IS_J2P_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[5].value_from_mapping = 7;
        cur_field_type_param->enums[6].value_from_mapping = 8;
        cur_field_type_param->enums[7].value_from_mapping = 9;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[19].value_from_mapping = 21;
        cur_field_type_param->enums[20].value_from_mapping = 22;
        cur_field_type_param->enums[21].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[22].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[23].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 29;
        cur_field_type_param->enums[27].value_from_mapping = 30;
        cur_field_type_param->enums[28].value_from_mapping = 31;
        cur_field_type_param->enums[29].value_from_mapping = 32;
        cur_field_type_param->enums[30].value_from_mapping = 33;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 23;
        cur_field_type_param->enums[47].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[48].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[49].value_from_mapping = 5;
        cur_field_type_param->enums[50].value_from_mapping = 6;
    }
    else if (DBAL_IS_JR2_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[5].value_from_mapping = 7;
        cur_field_type_param->enums[6].value_from_mapping = 8;
        cur_field_type_param->enums[7].value_from_mapping = 9;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].value_from_mapping = 20;
        cur_field_type_param->enums[19].value_from_mapping = 21;
        cur_field_type_param->enums[20].value_from_mapping = 22;
        cur_field_type_param->enums[21].value_from_mapping = 24;
        cur_field_type_param->enums[22].value_from_mapping = 25;
        cur_field_type_param->enums[23].value_from_mapping = 26;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 29;
        cur_field_type_param->enums[27].value_from_mapping = 30;
        cur_field_type_param->enums[28].value_from_mapping = 31;
        cur_field_type_param->enums[29].value_from_mapping = 32;
        cur_field_type_param->enums[30].value_from_mapping = 33;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[47].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[48].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[49].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[50].is_invalid_value_from_mapping = TRUE;
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
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
        cur_field_type_param->enums[5].value_from_mapping = 7;
        cur_field_type_param->enums[6].value_from_mapping = 8;
        cur_field_type_param->enums[7].value_from_mapping = 9;
        cur_field_type_param->enums[8].value_from_mapping = 10;
        cur_field_type_param->enums[9].value_from_mapping = 11;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 13;
        cur_field_type_param->enums[12].value_from_mapping = 14;
        cur_field_type_param->enums[13].value_from_mapping = 15;
        cur_field_type_param->enums[14].value_from_mapping = 16;
        cur_field_type_param->enums[15].value_from_mapping = 17;
        cur_field_type_param->enums[16].value_from_mapping = 18;
        cur_field_type_param->enums[17].value_from_mapping = 19;
        cur_field_type_param->enums[18].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[19].value_from_mapping = 21;
        cur_field_type_param->enums[20].value_from_mapping = 22;
        cur_field_type_param->enums[21].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[22].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[23].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[24].value_from_mapping = 27;
        cur_field_type_param->enums[25].value_from_mapping = 28;
        cur_field_type_param->enums[26].value_from_mapping = 29;
        cur_field_type_param->enums[27].value_from_mapping = 30;
        cur_field_type_param->enums[28].value_from_mapping = 31;
        cur_field_type_param->enums[29].value_from_mapping = 32;
        cur_field_type_param->enums[30].value_from_mapping = 33;
        cur_field_type_param->enums[31].value_from_mapping = 36;
        cur_field_type_param->enums[32].value_from_mapping = 37;
        cur_field_type_param->enums[33].value_from_mapping = 38;
        cur_field_type_param->enums[34].value_from_mapping = 39;
        cur_field_type_param->enums[35].value_from_mapping = 40;
        cur_field_type_param->enums[36].value_from_mapping = 41;
        cur_field_type_param->enums[37].value_from_mapping = 42;
        cur_field_type_param->enums[38].value_from_mapping = 43;
        cur_field_type_param->enums[39].value_from_mapping = 44;
        cur_field_type_param->enums[40].value_from_mapping = 45;
        cur_field_type_param->enums[41].value_from_mapping = 46;
        cur_field_type_param->enums[42].value_from_mapping = 47;
        cur_field_type_param->enums[43].value_from_mapping = 48;
        cur_field_type_param->enums[44].value_from_mapping = 49;
        cur_field_type_param->enums[45].value_from_mapping = 50;
        cur_field_type_param->enums[46].value_from_mapping = 23;
        cur_field_type_param->enums[47].value_from_mapping = 34;
        cur_field_type_param->enums[48].value_from_mapping = 35;
        cur_field_type_param->enums[49].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[50].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ENCAP_2_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 45 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "GENERAL___SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "GENERAL___SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___SFLOW_EXT_FIRST", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___IPT_IFA_FIRST_DP", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___IPT_IFA", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___IPT_IFA2_0_INGRESS", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "GENERAL___RAW_DATA_AT_VAR_SIZE", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GTP___GTPV1U_GPDU_HEADER", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "IPV4___NAT_BUILD_IP", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "IPV4___TUNNEL", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "IPV4___IPV4_VXLAN", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "IPV4___IPV4_VXLAN_ECN", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "IPV4___IPV4_GRE_WO_TNI", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "IPV4___IPV4_UDP", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "IPV4___IPV4_GRE8_GRE12_GENEVE", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "IPV6___TUNNEL", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "IPV6___GRE_ONLY", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "IPV6___GRE_ONLY_WO_TNI", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "IPV6___TUNNEL_ECN", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IPV6___VXLAN_ONLY", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "IPV6___GENEVE_ONLY", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IPV6___UDP_ONLY", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "MPLS___IMPLICIT_NULL_WO_AH", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "MPLS___MPLS_1", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "MPLS___MPLS_1_W_AH", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "MPLS___1_MPLS_W_AH", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "MPLS___MPLS_2", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "MPLS___MPLS_2_RFC8321", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "MPLS___2_MPLS_1_AH", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "MPLS___2_MPLS_2_AH", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "MPLS___2_MPLS_3_AH", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "MPLS___MPLS_1PLUS1", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "MPLS___MPLS_1PLUS2", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "MPLS___MPLS_2PLUS1", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "MPLS___MPLS_2PLUS1_RFC8321", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "MPLS___MPLS_2PLUS2", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "MPLS___MPLS_2PLUS2_RFC8321", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "MPLS___EVPN_WITH_ESI", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "SRV6___ENCAP_2_SEGMENTS", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "SRV6___ENCAP_1_SEGMENT", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "SRV6___ENCAP_0_SEGMENTS", sizeof(cur_field_type_param->enums[44].name_from_interface));
    cur_field_type_param->nof_enum_vals = 45;
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].value_from_mapping = 29;
        cur_field_type_param->enums[30].value_from_mapping = 30;
        cur_field_type_param->enums[31].value_from_mapping = 31;
        cur_field_type_param->enums[32].value_from_mapping = 32;
        cur_field_type_param->enums[33].value_from_mapping = 33;
        cur_field_type_param->enums[34].value_from_mapping = 34;
        cur_field_type_param->enums[35].value_from_mapping = 35;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].value_from_mapping = 37;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].value_from_mapping = 44;
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[30].value_from_mapping = 30;
        cur_field_type_param->enums[31].value_from_mapping = 31;
        cur_field_type_param->enums[32].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[33].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[34].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[35].value_from_mapping = 35;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].value_from_mapping = 37;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].value_from_mapping = 44;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ENCAP_3_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 47 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___BIER_MPLS_64", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BIER___BIER_TI_64", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BIER___BIER_MPLS_256_128", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BIER___BIER_TI_256_128", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___SFLOW_EXT_FIRST", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___IPT_IFA_FIRST_DP", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "GENERAL___IPT_IFA", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GENERAL___IPT_IFA2_0_INGRESS", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "GENERAL___IPT_IFA2_0_INTERMEDIATE", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "GENERAL___IPT_IFA2_0_EGRESS_NO_MD", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "GENERAL___RAW_DATA_AT_VAR_SIZE", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "IPV4___KEEP_ORIGINAL_ETH", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "IPV4___TUNNEL", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "IPV4___IPV4_VXLAN", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "IPV4___IPV4_VXLAN_ECN", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "IPV4___IPV4_GRE_WO_TNI", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "IPV4___IPV4_UDP", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "IPV4___IPV4_GRE8_GRE12_GENEVE", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "IPV6___TUNNEL", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IPV6___TUNNEL_ECN", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "IPV6___VXLAN_ONLY", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IPV6___GENEVE_ONLY", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "IPV6___GRE_ONLY", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "IPV6___GRE_ONLY_WO_TNI", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "IPV6___UDP_ONLY", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "MPLS___IMPLICIT_NULL_WO_AH", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "MPLS___MPLS_1", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "MPLS___1_MPLS_W_AH", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "MPLS___MPLS_2", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "MPLS___2_MPLS_1_AH", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "MPLS___2_MPLS_2_AH", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "MPLS___2_MPLS_3_AH", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "MPLS___MPLS_1PLUS1", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "MPLS___MPLS_1PLUS2", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "MPLS___MPLS_2PLUS1", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "MPLS___MPLS_2PLUS1_RFC8321", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "MPLS___MPLS_2PLUS2", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "MPLS___MPLS_2PLUS2_RFC8321", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "SRV6___SRH_AND_2_SEGMENTS", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "SRV6___TWO_SEGMENTS", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "SRV6___ONE_SEGMENT", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "SRV6___SRH_AND_1_SEGMENT", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "SRV6___SRH_AND_0_SEGMENT", sizeof(cur_field_type_param->enums[46].name_from_interface));
    cur_field_type_param->nof_enum_vals = 47;
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].value_from_mapping = 29;
        cur_field_type_param->enums[30].value_from_mapping = 30;
        cur_field_type_param->enums[31].value_from_mapping = 31;
        cur_field_type_param->enums[32].value_from_mapping = 32;
        cur_field_type_param->enums[33].value_from_mapping = 33;
        cur_field_type_param->enums[34].value_from_mapping = 34;
        cur_field_type_param->enums[35].value_from_mapping = 35;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].value_from_mapping = 37;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].value_from_mapping = 44;
        cur_field_type_param->enums[45].value_from_mapping = 45;
        cur_field_type_param->enums[46].value_from_mapping = 46;
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].value_from_mapping = 29;
        cur_field_type_param->enums[30].value_from_mapping = 30;
        cur_field_type_param->enums[31].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[32].value_from_mapping = 32;
        cur_field_type_param->enums[33].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[34].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[35].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].value_from_mapping = 37;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].value_from_mapping = 44;
        cur_field_type_param->enums[45].value_from_mapping = 45;
        cur_field_type_param->enums[46].value_from_mapping = 46;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ENCAP_4_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 29 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___BIER_MPLS_128_64", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BIER___BIER_MPLS_256", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BIER___BIER_TI_128_64", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BIER___BIER_TI_256", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___RAW_DATA_AT_VAR_SIZE", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "IPV4___TUNNEL", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "IPV4___IPV4_VXLAN", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "IPV4___IPV4_VXLAN_ECN", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "IPV4___IPV4_GRE_WO_TNI", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "IPV4___IPV4_UDP", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "IPV4___IPV4_GRE8_GRE12_GENEVE", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "IPV6___TUNNEL", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "IPV6___TUNNEL_ECN", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "MPLS___IMPLICIT_NULL_WO_AH", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "MPLS___MPLS_1", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "MPLS___1_MPLS_W_AH", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "MPLS___MPLS_2", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "MPLS___2_MPLS_1_AH", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "MPLS___2_MPLS_2_AH", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "MPLS___2_MPLS_3_AH", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "MPLS___MPLS_1PLUS1", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "MPLS___MPLS_1PLUS2", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "MPLS___MPLS_2PLUS1", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "MPLS___MPLS_2PLUS2", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "SRV6___ONE_SEGMENT", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "SRV6___SRV6_IPV6_HEADER", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "SRV6___SRV6_IPV6_HEADER_EXT_ENCAP", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "SRV6___T_INSERT", sizeof(cur_field_type_param->enums[28].name_from_interface));
    cur_field_type_param->nof_enum_vals = 29;
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[16].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[17].value_from_mapping = 17;
        cur_field_type_param->enums[18].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[19].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[20].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[21].value_from_mapping = 21;
        cur_field_type_param->enums[22].value_from_mapping = 22;
        cur_field_type_param->enums[23].value_from_mapping = 23;
        cur_field_type_param->enums[24].value_from_mapping = 24;
        cur_field_type_param->enums[25].value_from_mapping = 25;
        cur_field_type_param->enums[26].value_from_mapping = 26;
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ENCAP_5_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 7 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BRIDGE___ARP_AC_ETH", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BRIDGE___ARP_PLUS_AC_ETH", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "FCOE___NPV_US", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___RCH_ENC", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BRIDGE___ARP_AC_ETH_SVTAG", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "BRIDGE___ARP_PLUS_AC_ETH_SVTAG", sizeof(cur_field_type_param->enums[6].name_from_interface));
    cur_field_type_param->nof_enum_vals = 7;
    if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 4;
        cur_field_type_param->enums[3].value_from_mapping = 5;
        cur_field_type_param->enums[4].value_from_mapping = 6;
        cur_field_type_param->enums[5].value_from_mapping = 2;
        cur_field_type_param->enums[6].value_from_mapping = 3;
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[2].value_from_mapping = 4;
        cur_field_type_param->enums[3].value_from_mapping = 5;
        cur_field_type_param->enums[4].value_from_mapping = 6;
        cur_field_type_param->enums[5].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[6].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TRAP_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 54 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "GENERAL___BFD_PWE_CW_WA", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "GENERAL___TRJ_TRACE_TYPE2", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___RCH_DROP_AND_CONT", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GENERAL___RCH_EXT_ENCAP", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___RCH_PTCH_ENC", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___RCH_XTERMINATION", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___PUNT_W_PTCH1", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "GENERAL___PTCH1_FROM_FTMH_SSPA", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "GENERAL___PUNT_W_PTCH2", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "GENERAL___RECYCLE_W_PTCH2", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GENERAL___OAM_UPMEP_REFLECTOR", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "GENERAL___NORMAL_FWD", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "GENERAL___NORMAL_FWD_NO_MIRROR", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "GENERAL___DROP_PKT", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "GENERAL___VISIBILITY", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "GENERAL___CPU_PORT_TRAPPED", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "GENERAL___CPU_TRAPPED_KEEP_ORIGINAL", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "GENERAL___USER_TRAP_1", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "GENERAL___USER_TRAP_2", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "GENERAL___USER_TRAP_3", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "GENERAL___USER_TRAP_4", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "GENERAL___EGRESS_TRAPPED", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "GENERAL___STACKING_LEFT", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "GENERAL___STACKING_RIGHT", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "GENERAL___IPT_IFA_FIRST_DP", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "GENERAL___RCH_JMODE_VRF", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "OAM___OAM_DM_TO_OAMP", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "OAM___OAM_DM_TO_CPU", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "OAM___OAM_DM_TO_FPGA", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "OAM___OAM_LM_TO_OAMP", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "OAM___OAM_LM_TO_CPU", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "OAM___OAM_LM_TO_FPGA", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "OAM___OAM_CCM_TO_OAMP", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "OAM___OAM_CCM_TO_CPU", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "OAM___OAM_CCM_TO_FPGA", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "OAM___OAM_LVLERR", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "OAM___OAM_PSVERR", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "OAM___OAM_REFLECTOR", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "OAM___BFDV6_CHECKSUM_ERROR", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "TELEMETRY___IFA_EGRESS_PORT", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "TM___RAW_PORT", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "TM___DEFAULT", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "TM___JR1_COMP_MACT_LEARNING_JR2_TO_JR1", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "TM___TDM_NO_OTMH", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "TM___TDM_STAMP_MC_CUD_ON_OTMH", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "TM___TDM_STAMP_PORT_VAL_ON_OTMH", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "TM___TDM_STAMP_NOTHING_ON_OTMH", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "TM___TDM_STAMP_MC_CUD_ON_FTMH", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "TM___TDM_STAMP_PORT_VAL_ON_FTMH", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "TM___TDM_STAMP_NOTHING_ON_FTMH", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "TM___TDM_STAMP_MC_CUD_ON_OPT_FTMH", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "TM___TDM_STAMP_PORT_VAL_ON_OPT_FTMH", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "TM___TDM_STAMP_NOTHING_ON_OPT_FTMH", sizeof(cur_field_type_param->enums[53].name_from_interface));
    cur_field_type_param->nof_enum_vals = 54;
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].value_from_mapping = 29;
        cur_field_type_param->enums[30].value_from_mapping = 30;
        cur_field_type_param->enums[31].value_from_mapping = 31;
        cur_field_type_param->enums[32].value_from_mapping = 32;
        cur_field_type_param->enums[33].value_from_mapping = 33;
        cur_field_type_param->enums[34].value_from_mapping = 34;
        cur_field_type_param->enums[35].value_from_mapping = 35;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].value_from_mapping = 37;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].value_from_mapping = 44;
        cur_field_type_param->enums[45].value_from_mapping = 45;
        cur_field_type_param->enums[46].value_from_mapping = 46;
        cur_field_type_param->enums[47].value_from_mapping = 47;
        cur_field_type_param->enums[48].value_from_mapping = 48;
        cur_field_type_param->enums[49].value_from_mapping = 49;
        cur_field_type_param->enums[50].value_from_mapping = 50;
        cur_field_type_param->enums[51].value_from_mapping = 51;
        cur_field_type_param->enums[52].value_from_mapping = 52;
        cur_field_type_param->enums[53].value_from_mapping = 53;
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
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
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].value_from_mapping = 29;
        cur_field_type_param->enums[30].value_from_mapping = 30;
        cur_field_type_param->enums[31].value_from_mapping = 31;
        cur_field_type_param->enums[32].value_from_mapping = 32;
        cur_field_type_param->enums[33].value_from_mapping = 33;
        cur_field_type_param->enums[34].value_from_mapping = 34;
        cur_field_type_param->enums[35].value_from_mapping = 35;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].value_from_mapping = 37;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[45].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[46].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[47].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[48].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[49].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[50].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[51].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[52].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[53].is_invalid_value_from_mapping = TRUE;
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
