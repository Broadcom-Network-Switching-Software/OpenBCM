
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt1_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT1_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT1_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 35 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___RCH_TERM_BFIR", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BRIDGE___UNTAGGED", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BRIDGE___C_TAG", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BRIDGE___S_TAG", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BRIDGE___S_C_TAGS", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BRIDGE___C_C_TAGS", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "BRIDGE___S_S_TAGS", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "BRIDGE___E_TAG", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "BRIDGE___E_C_TAG", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "BRIDGE___E_S_TAG", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "BRIDGE___E_S_C_TAGS", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "BRIDGE___S_PRIORITY_C_TAGS", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "BRIDGE___UNTAGGED_FRR", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "BRIDGE___C_TAG_FRR", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "BRIDGE___S_TAG_FRR", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "BRIDGE___S_C_TAGS_FRR", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "BRIDGE___E_TAG_FRR", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "BRIDGE___E_C_TAG_FRR", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "BRIDGE___E_S_TAG_FRR", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "BRIDGE___E_S_C_TAGS_FRR", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "BRIDGE___S_PRIORITY_C_TAGS_FRR", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "BRIDGE___PON_STC_C_TAG", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "BRIDGE___PON_STC_S_TAG", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "BRIDGE___PON_STC_UNTAGGED", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "BRIDGE___PON_DTC_SP_C_TAG", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "BRIDGE___PON_DTC_C_TAG", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "BRIDGE___PON_DTC_S_TAG", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "BRIDGE___PON_DTC_UNTAGGED", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "GENERAL___RCH_TERM_DROP_AND_CONT", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "GENERAL___RCH_TERM_JR_MODE", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "GENERAL___RCH_TERM_REFLECTOR", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "GENERAL___RCH_TERM_XTERMINATION", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "MPLS___RAW_MPLS", sizeof(cur_field_type_param->enums[34].name_from_interface));
    cur_field_type_param->nof_enum_vals = 35;
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
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt2_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT2_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT2_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 22 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___TT_P2P_MPLS", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BIER___TT_P2P_TI", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "FCOE___FCF_VRF_BY_VSI", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FCOE___FCF_VRF_BY_VFT", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FCOE___FCF_VRF_BY_PORT", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "IPV4___TT_P2P_TUNNEL", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "IPV4___2ND_PASS_TT_P2P_TUNNEL", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "IPV4___TT_IPSEC", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "IPV4___TT_IPSEC_UDP", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "IPV6___TT_SIP_MSBS", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "IPV6___TT_IPSEC", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "IPV6___TT_IPSEC_UDP", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "MAC_IN_MAC___LEARN", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "MPLS___TT", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "MPLS___2ND_PASS_TT", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "MPLS___EVPN_IML", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(cur_field_type_param->enums[21].name_from_interface));
    cur_field_type_param->nof_enum_vals = 22;
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
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt3_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT3_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT3_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 22 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___TT_MP_MPLS", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BIER___TT_MP_TI", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "GRE_WITH_KEY___TNI2VRF_VTT3", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GRE_WITH_KEY___TNI2VSI_VTT3", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "IPV4___TT_MP_AND_TCAM", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "IPV4___2ND_PASS_TT_MP_AND_TCAM", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "IPV4___TT_P2P_AND_TCAM", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "IPV6___TT_DIP", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "IPV6___2ND_PASS_TT_DIP", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "MPLS___TT", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "MPLS___EVPN_IML", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "MPLS___EVPN_FL_AFTER_IML", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "MPLS___EVPN_FL_AND_CW_AFTER_IML", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "MPLS___SPECULATIVE_PARSING_FIX", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "MPLS___P2P_ONLY", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "PPPOE___SA_COMPRESSION", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "VXLAN_GPE___VNI2VSI_VTT3", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "VXLAN_GPE___VNI2VRF_VTT3", sizeof(cur_field_type_param->enums[21].name_from_interface));
    cur_field_type_param->nof_enum_vals = 22;
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
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt4_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT4_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT4_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 25 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "GENERAL___2ND_PARSING", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "GRE_WITH_KEY___TNI2VRF_VTT4", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "GRE_WITH_KEY___TNI2VSI_VTT4", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "IPV4___TT_P2P_TUNNEL", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "IPV4___TT_MP_TUNNEL", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "IPV6___TT_SIP_IDX_DIP_OR_DIP", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "IPV6___TT_SIP_IDX_DIP", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "L2TPV2___TT_WO_LENGTH", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "L2TPV2___TT_W_LENGTH", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "MAC_IN_MAC___ISID_WITHOUT_DOMAIN", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "MAC_IN_MAC___ISID_WITH_DOMAIN", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "MPLS___TT", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "MPLS___EVPN_IML", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "MPLS___EVPN_FL_AFTER_IML", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "MPLS___EVPN_FL_AND_CW_AFTER_IML", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "MPLS___P2P_ONLY", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "PPPOE___TT_CMPRS_SA", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "PPPOE___TT_FULL_SA", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "VXLAN_GPE___VNI2VSI_VTT4", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "VXLAN_GPE___VNI2VRF_VTT4", sizeof(cur_field_type_param->enums[24].name_from_interface));
    cur_field_type_param->nof_enum_vals = 25;
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
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt5_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT5_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT5_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 41 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "BRIDGE___PON_DTC_OTHER", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "BRIDGE___PON_DTC_C_C_TAG", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "BRIDGE___PON_DTC_S_S_TAG", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "BRIDGE___PON_DTC_S_C_TAG", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "GRE_WITH_KEY___TNI2VRF_VTT5", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "GRE_WITH_KEY___TNI2VSI_VTT5", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "GTP___TT_NWK_SCOPED_TEID", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "IPV4___TT_P2P_TUNNEL", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "IPV4___TT_MP_TUNNEL", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "IPV4___TT_IPSEC", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IPV4___TT_IPSEC_UDP", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "IPV6___TT_IPSEC", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IPV6___TT_IPSEC_UDP", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "MPLS___TT", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "MPLS___EVPN_IML", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "MPLS___P2P_ONLY", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "PPPOE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "SRV6___EXTENDED_TUNNEL_TERMINATE", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "SRV6___ONE_PASS_TUNNEL_TERMINATE", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "SRV6___USID_ENDPOINT", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "SRV6___UNIFIED_END_POINT_SID_EXTRACT_0", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "SRV6___UNIFIED_END_POINT_SID_EXTRACT_1", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "SRV6___UNIFIED_END_POINT_SID_EXTRACT_2", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "VXLAN_GPE___VNI2VSI_VTT5", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "VXLAN_GPE___VNI2VRF_VTT5", sizeof(cur_field_type_param->enums[40].name_from_interface));
    cur_field_type_param->nof_enum_vals = 41;
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
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
        cur_field_type_param->enums[6].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[7].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[8].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[9].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[10].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[11].is_invalid_value_from_mapping = TRUE;
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
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_fwd1_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD1_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD1_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 46 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BIER___TI_FORWARDING", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BIER___MPLS_FORWARDING", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BRIDGE___SVL", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BRIDGE___IPV4MC_SVL", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BRIDGE___IPV6MC_SVL", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "FCOE___FCF_NO_VFT", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FCOE___FCF_NPV_NO_VFT", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "FCOE___FCF_VFT", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "FCOE___FCF_NPV_VFT", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "FCOE___TRANSIT", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "FCOE___TRANSIT_FIP", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "GENERAL___NOP_CTX_MACT_SA_LKP", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "IPV4___MC_PRIVATE_PUBLIC", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "IPV4___MC_PRIVATE", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "IPV4___MC_PRIVATE_MACT_SA_LKP", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "IPV4___PRIVATE_UC", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "IPV4___PRIVATE_UC_W_OPTIONS", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IPV4___PRIVATE_UC_MACT_SA_LKP", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "IPV4___NAT_UPSTREAM", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "IPV4___NAT_DOWNSTREAM", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "IPV6___MC_PRIVATE_PUBLIC", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "IPV6___MC_PRIVATE", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "IPV6___MC_PRIVATE_MACT_SA_LKP", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "IPV6___PRIVATE_UC", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "IPV6___PRIVATE_UC_MACT_SA_LKP", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "MPLS___FWD", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "MPLS___FWD_MACT_SA_LKP", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "MPLS___FWD_PER_INTERFACE", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "MPLS___OAM_ONLY", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "SLLB___IPV4_ROUTE", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "SLLB___IPV6_ROUTE", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "SRV6___SEGMENT_END_POINT_FORWARDING", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "SRV6___USID_END_POINT_FORWARDING", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "SRV6___USID_NOP_CTX", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "SRV6___UNIFIED_END_POINT_FORWARDING", sizeof(cur_field_type_param->enums[45].name_from_interface));
    cur_field_type_param->nof_enum_vals = 46;
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
        cur_field_type_param->enums[5].value_from_mapping = 5;
        cur_field_type_param->enums[6].value_from_mapping = 6;
        cur_field_type_param->enums[7].value_from_mapping = 7;
        cur_field_type_param->enums[8].value_from_mapping = 8;
        cur_field_type_param->enums[9].value_from_mapping = 9;
        cur_field_type_param->enums[10].value_from_mapping = 10;
        cur_field_type_param->enums[11].value_from_mapping = 11;
        cur_field_type_param->enums[12].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[13].value_from_mapping = 13;
        cur_field_type_param->enums[14].value_from_mapping = 14;
        cur_field_type_param->enums[15].value_from_mapping = 15;
        cur_field_type_param->enums[16].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[17].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[18].value_from_mapping = 18;
        cur_field_type_param->enums[19].value_from_mapping = 19;
        cur_field_type_param->enums[20].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[21].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[22].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[23].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[24].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[25].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[26].value_from_mapping = 26;
        cur_field_type_param->enums[27].value_from_mapping = 27;
        cur_field_type_param->enums[28].value_from_mapping = 28;
        cur_field_type_param->enums[29].value_from_mapping = 29;
        cur_field_type_param->enums[30].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[31].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[32].value_from_mapping = 32;
        cur_field_type_param->enums[33].value_from_mapping = 33;
        cur_field_type_param->enums[34].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[35].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[36].value_from_mapping = 36;
        cur_field_type_param->enums[37].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[38].value_from_mapping = 38;
        cur_field_type_param->enums[39].value_from_mapping = 39;
        cur_field_type_param->enums[40].value_from_mapping = 40;
        cur_field_type_param->enums[41].value_from_mapping = 41;
        cur_field_type_param->enums[42].value_from_mapping = 42;
        cur_field_type_param->enums[43].value_from_mapping = 43;
        cur_field_type_param->enums[44].value_from_mapping = 44;
        cur_field_type_param->enums[45].value_from_mapping = 45;
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
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_fwd2_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD2_CONTEXT_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 61 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "BRIDGE___IPV4MC_SVL", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "BRIDGE___IPV6MC_SVL", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "BRIDGE___SVL", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FCOE___FCF", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FCOE___FCF_FIP", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "GENERAL___NOP_CTX", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "IPV4___MC_PRIVATE_W_BF_W_F2B", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "IPV4___MC_PRIVATE_WO_BF_W_F2B", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "IPV4___MC_PRIVATE_WO_BF_WO_F2B", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "IPV4___PRIVATE_UC", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "IPV4___PRIVATE_UC_LPM_ONLY", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "IPV4___NAT_UPSTREAM", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "IPV4___NAT_DOWNSTREAM", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "IPV6___MC_PRIVATE_W_BF_W_F2B", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "IPV6___MC_PRIVATE_WO_BF_W_F2B", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "IPV6___MC_PRIVATE_WO_BF_WO_F2B", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "IPV6___PRIVATE_UC", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "IPV6___PRIVATE_UC_LPM_ONLY", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "IPV6___PRIVATE_UC_BFD", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "MPLS___OAM_ONLY", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "MPLS___FWD", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "SLLB___VIRTUAL_IPV4_ROUTE", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "SLLB___VIRTUAL_IPV6_ROUTE", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "SLLB___SIMPLE_IPV4_ROUTE", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "SLLB___SIMPLE_IPV6_ROUTE", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "SRV6___PRIVATE_UC", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "SRV6___USID_PRIVATE_UC", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "SRV6___USID_NOP_CTX", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "ACL_CONTEXT_32", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "ACL_CONTEXT_33", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "ACL_CONTEXT_34", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "ACL_CONTEXT_35", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "ACL_CONTEXT_36", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "ACL_CONTEXT_37", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "ACL_CONTEXT_38", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "ACL_CONTEXT_39", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "ACL_CONTEXT_40", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "ACL_CONTEXT_41", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "ACL_CONTEXT_42", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "ACL_CONTEXT_43", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "ACL_CONTEXT_44", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "ACL_CONTEXT_45", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "ACL_CONTEXT_46", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "ACL_CONTEXT_47", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "ACL_CONTEXT_48", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "ACL_CONTEXT_49", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "ACL_CONTEXT_50", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "ACL_CONTEXT_51", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "ACL_CONTEXT_52", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "ACL_CONTEXT_53", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "ACL_CONTEXT_54", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "ACL_CONTEXT_55", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "ACL_CONTEXT_56", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "ACL_CONTEXT_57", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "ACL_CONTEXT_58", sizeof(cur_field_type_param->enums[55].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[56].name_from_interface, "ACL_CONTEXT_59", sizeof(cur_field_type_param->enums[56].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[57].name_from_interface, "ACL_CONTEXT_60", sizeof(cur_field_type_param->enums[57].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[58].name_from_interface, "ACL_CONTEXT_61", sizeof(cur_field_type_param->enums[58].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[59].name_from_interface, "ACL_CONTEXT_62", sizeof(cur_field_type_param->enums[59].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[60].name_from_interface, "ACL_CONTEXT_63", sizeof(cur_field_type_param->enums[60].name_from_interface));
    cur_field_type_param->nof_enum_vals = 61;
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
        cur_field_type_param->enums[29].value_from_mapping = 32;
        cur_field_type_param->enums[30].value_from_mapping = 33;
        cur_field_type_param->enums[31].value_from_mapping = 34;
        cur_field_type_param->enums[32].value_from_mapping = 35;
        cur_field_type_param->enums[33].value_from_mapping = 36;
        cur_field_type_param->enums[34].value_from_mapping = 37;
        cur_field_type_param->enums[35].value_from_mapping = 38;
        cur_field_type_param->enums[36].value_from_mapping = 39;
        cur_field_type_param->enums[37].value_from_mapping = 40;
        cur_field_type_param->enums[38].value_from_mapping = 41;
        cur_field_type_param->enums[39].value_from_mapping = 42;
        cur_field_type_param->enums[40].value_from_mapping = 43;
        cur_field_type_param->enums[41].value_from_mapping = 44;
        cur_field_type_param->enums[42].value_from_mapping = 45;
        cur_field_type_param->enums[43].value_from_mapping = 46;
        cur_field_type_param->enums[44].value_from_mapping = 47;
        cur_field_type_param->enums[45].value_from_mapping = 48;
        cur_field_type_param->enums[46].value_from_mapping = 49;
        cur_field_type_param->enums[47].value_from_mapping = 50;
        cur_field_type_param->enums[48].value_from_mapping = 51;
        cur_field_type_param->enums[49].value_from_mapping = 52;
        cur_field_type_param->enums[50].value_from_mapping = 53;
        cur_field_type_param->enums[51].value_from_mapping = 54;
        cur_field_type_param->enums[52].value_from_mapping = 55;
        cur_field_type_param->enums[53].value_from_mapping = 56;
        cur_field_type_param->enums[54].value_from_mapping = 57;
        cur_field_type_param->enums[55].value_from_mapping = 58;
        cur_field_type_param->enums[56].value_from_mapping = 59;
        cur_field_type_param->enums[57].value_from_mapping = 60;
        cur_field_type_param->enums[58].value_from_mapping = 61;
        cur_field_type_param->enums[59].value_from_mapping = 62;
        cur_field_type_param->enums[60].value_from_mapping = 63;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt1_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt2_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt3_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt4_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_vt5_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_fwd1_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_ingress_ctx_id_field_types_definition_fwd2_context_id_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
