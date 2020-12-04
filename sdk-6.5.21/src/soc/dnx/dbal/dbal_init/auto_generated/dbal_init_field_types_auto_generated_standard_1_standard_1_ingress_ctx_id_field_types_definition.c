
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 36;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BIER___RCH_TERM_BFIR].name_from_interface, "BIER___RCH_TERM_BFIR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BIER___RCH_TERM_BFIR].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BIER___RCH_TERM_BFIR].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE].name_from_interface, "GENERAL___RCH_TERM_JR_MODE", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE].is_invalid_value_from_mapping = !(!dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED].name_from_interface, "BRIDGE___UNTAGGED", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG].name_from_interface, "BRIDGE___C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG].name_from_interface, "BRIDGE___S_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS].name_from_interface, "BRIDGE___S_C_TAGS", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_C_TAGS].name_from_interface, "BRIDGE___C_C_TAGS", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_C_TAGS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_S_TAGS].name_from_interface, "BRIDGE___S_S_TAGS", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_S_TAGS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG].name_from_interface, "BRIDGE___E_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG].name_from_interface, "BRIDGE___E_C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG].name_from_interface, "BRIDGE___E_S_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS].name_from_interface, "BRIDGE___E_S_C_TAGS", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS].name_from_interface, "BRIDGE___S_PRIORITY_C_TAGS", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR].name_from_interface, "BRIDGE___UNTAGGED_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG_FRR].name_from_interface, "BRIDGE___C_TAG_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG_FRR].name_from_interface, "BRIDGE___S_TAG_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS_FRR].name_from_interface, "BRIDGE___S_C_TAGS_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG_FRR].name_from_interface, "BRIDGE___E_TAG_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG_FRR].name_from_interface, "BRIDGE___E_C_TAG_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG_FRR].name_from_interface, "BRIDGE___E_S_TAG_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS_FRR].name_from_interface, "BRIDGE___E_S_C_TAGS_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS_FRR].name_from_interface, "BRIDGE___S_PRIORITY_C_TAGS_FRR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS_FRR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_C_TAG].name_from_interface, "BRIDGE___PON_STC_C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_S_TAG].name_from_interface, "BRIDGE___PON_STC_S_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_S_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_UNTAGGED].name_from_interface, "BRIDGE___PON_STC_UNTAGGED", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_UNTAGGED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_SP_C_TAG].name_from_interface, "BRIDGE___PON_DTC_SP_C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_SP_C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_C_TAG].name_from_interface, "BRIDGE___PON_DTC_C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_S_TAG].name_from_interface, "BRIDGE___PON_DTC_S_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_S_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_UNTAGGED].name_from_interface, "BRIDGE___PON_DTC_UNTAGGED", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_UNTAGGED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_DROP_AND_CONT].name_from_interface, "GENERAL___RCH_TERM_DROP_AND_CONT", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_DROP_AND_CONT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR].name_from_interface, "GENERAL___RCH_TERM_REFLECTOR", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION].name_from_interface, "GENERAL___RCH_TERM_XTERMINATION", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_MPLS___RAW_MPLS].name_from_interface, "MPLS___RAW_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_MPLS___RAW_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].name_from_interface, "SRV6___RCH_SRV6_USP_PSP", sizeof(enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
    }
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BIER___RCH_TERM_BFIR].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_C_TAGS].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_S_TAGS].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG_FRR].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG_FRR].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS_FRR].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG_FRR].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG_FRR].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG_FRR].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS_FRR].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS_FRR].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_C_TAG].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_S_TAG].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_UNTAGGED].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_SP_C_TAG].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_C_TAG].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_S_TAG].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_UNTAGGED].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_DROP_AND_CONT].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_MPLS___RAW_MPLS].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].value_from_mapping = 34;
        }
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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 22;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___TT_P2P_MPLS].name_from_interface, "BIER___TT_P2P_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___TT_P2P_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___TT_P2P_TI].name_from_interface, "BIER___TT_P2P_TI", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___TT_P2P_TI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI].name_from_interface, "FCOE___FCF_VRF_BY_VSI", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT].name_from_interface, "FCOE___FCF_VRF_BY_VFT", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT].name_from_interface, "FCOE___FCF_VRF_BY_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].name_from_interface, "GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].name_from_interface, "IPV4___TT_P2P_TUNNEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL].name_from_interface, "IPV4___2ND_PASS_TT_P2P_TUNNEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC].name_from_interface, "IPV4___TT_IPSEC", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP].name_from_interface, "IPV4___TT_IPSEC_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS].name_from_interface, "IPV6___TT_SIP_MSBS", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC].name_from_interface, "IPV6___TT_IPSEC", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP].name_from_interface, "IPV6___TT_IPSEC_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN].name_from_interface, "MAC_IN_MAC___LEARN", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT].name_from_interface, "MPLS___TT", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT].name_from_interface, "MPLS___2ND_PASS_TT", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL].name_from_interface, "MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface, "MPLS___EVPN_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface));
    }
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___TT_P2P_MPLS].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___TT_P2P_TI].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 21;
        }
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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 27;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_MPLS].name_from_interface, "BIER___TT_MP_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_TI].name_from_interface, "BIER___TT_MP_TI", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_TI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].name_from_interface, "GENERAL___IPSEC_PARSING_CONTEXT_FIX", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].name_from_interface, "GRE_WITH_KEY___TNI2VRF_VTT3", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].name_from_interface, "GRE_WITH_KEY___TNI2VSI_VTT3", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].name_from_interface, "IPV4___TT_MP_AND_TCAM", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].name_from_interface, "IPV4___2ND_PASS_TT_MP_AND_TCAM", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].name_from_interface, "IPV4___TT_P2P_AND_TCAM", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].name_from_interface, "IPV6___TT_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].name_from_interface, "IPV6___2ND_PASS_TT_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].name_from_interface, "IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].name_from_interface, "IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CONTEXT_FIX].name_from_interface, "IPV6___IPV6_EH_PARSING_CONTEXT_FIX", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CONTEXT_FIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].name_from_interface, "MPLS___TT", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface, "MPLS___EVPN_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].name_from_interface, "MPLS___EVPN_FL_AFTER_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].name_from_interface, "MPLS___EVPN_FL_AND_CW_AFTER_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].name_from_interface, "MPLS___SPECULATIVE_PARSING_FIX", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].name_from_interface, "MPLS___P2P_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_PPPOE___SA_COMPRESSION].name_from_interface, "PPPOE___SA_COMPRESSION", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_PPPOE___SA_COMPRESSION].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].name_from_interface, "VXLAN_GPE___VNI2VSI_VTT3", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].name_from_interface, "VXLAN_GPE___VNI2VRF_VTT3", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].name_from_interface, "MPLS___IFIT", sizeof(enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].name_from_interface));
    }
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_MPLS].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_TI].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_PPPOE___SA_COMPRESSION].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_MPLS].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BIER___TT_MP_TI].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_PPPOE___SA_COMPRESSION].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].value_from_mapping = 23;
        }
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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 30;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].name_from_interface, "GENERAL___2ND_PARSING", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].name_from_interface, "GRE_WITH_KEY___TNI2VRF_VTT4", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].name_from_interface, "GRE_WITH_KEY___TNI2VSI_VTT4", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].name_from_interface, "IPV4___TT_P2P_TUNNEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].name_from_interface, "IPV4___TT_MP_TUNNEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].name_from_interface, "IPV6___TT_SIP_IDX_DIP_OR_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].name_from_interface, "IPV6___TT_SIP_IDX_DIP", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].name_from_interface, "IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].name_from_interface, "IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].name_from_interface, "IPV6___TT_USID_PREFIX_32_USID_16", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].name_from_interface, "IPV6___TT_GSID_PREFIX_48_GSID_16", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_WO_LENGTH].name_from_interface, "L2TPV2___TT_WO_LENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_WO_LENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_W_LENGTH].name_from_interface, "L2TPV2___TT_W_LENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_W_LENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].name_from_interface, "MAC_IN_MAC___ISID_WITHOUT_DOMAIN", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].name_from_interface, "MAC_IN_MAC___ISID_WITH_DOMAIN", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].name_from_interface, "MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].name_from_interface, "MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].name_from_interface, "MPLS___TT", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface, "MPLS___EVPN_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].name_from_interface, "MPLS___EVPN_FL_AFTER_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].name_from_interface, "MPLS___EVPN_FL_AND_CW_AFTER_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].name_from_interface, "MPLS___P2P_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_CMPRS_SA].name_from_interface, "PPPOE___TT_CMPRS_SA", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_CMPRS_SA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_FULL_SA].name_from_interface, "PPPOE___TT_FULL_SA", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_FULL_SA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].name_from_interface, "VXLAN_GPE___VNI2VSI_VTT4", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].name_from_interface, "VXLAN_GPE___VNI2VRF_VTT4", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].name_from_interface, "MPLS___IFIT", sizeof(enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].name_from_interface));
    }
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_WO_LENGTH].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_W_LENGTH].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_CMPRS_SA].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_FULL_SA].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_WO_LENGTH].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_L2TPV2___TT_W_LENGTH].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_CMPRS_SA].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_PPPOE___TT_FULL_SA].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].value_from_mapping = 25;
        }
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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 43;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].name_from_interface, "BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].name_from_interface, "BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].name_from_interface, "BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].name_from_interface, "BRIDGE___PON_DTC_OTHER", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].name_from_interface, "BRIDGE___PON_DTC_C_C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].name_from_interface, "BRIDGE___PON_DTC_S_S_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].name_from_interface, "BRIDGE___PON_DTC_S_C_TAG", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].name_from_interface, "GRE_WITH_KEY___TNI2VRF_VTT5", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].name_from_interface, "GRE_WITH_KEY___TNI2VSI_VTT5", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GTP___TT_NWK_SCOPED_TEID].name_from_interface, "GTP___TT_NWK_SCOPED_TEID", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GTP___TT_NWK_SCOPED_TEID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].name_from_interface, "IPV4___TT_P2P_TUNNEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].name_from_interface, "IPV4___TT_MP_TUNNEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].name_from_interface, "IPV4___TT_IPSEC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].name_from_interface, "IPV4___TT_IPSEC_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].name_from_interface, "IPV6___TT_IPSEC", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].name_from_interface, "IPV6___TT_IPSEC_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].name_from_interface, "MPLS___TT", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface, "MPLS___TT_PER_INTERFACE_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface, "MPLS___EVPN_IML", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].name_from_interface, "MPLS___P2P_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface, "MPLS___SPECIAL_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].name_from_interface, "MPLS___GAL_WITH_FLOW_LABEL_FIX", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_PPPOE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].name_from_interface, "PPPOE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_PPPOE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].name_from_interface, "SRV6___EXTENDED_TUNNEL_TERMINATE", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE].name_from_interface, "SRV6___ONE_PASS_TUNNEL_TERMINATE", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH].name_from_interface, "SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].name_from_interface, "SRV6___CLASSIC_ENDPOINT", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].name_from_interface, "SRV6___USID_ENDPOINT", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].name_from_interface, "SRV6___GSID_ENDPOINT_SID_EXTRACT_0", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].name_from_interface, "SRV6___GSID_ENDPOINT_SID_EXTRACT_1", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].name_from_interface, "SRV6___GSID_ENDPOINT_SID_EXTRACT_2", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].name_from_interface, "VXLAN_GPE___VNI2VSI_VTT5", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].name_from_interface, "VXLAN_GPE___VNI2VRF_VTT5", sizeof(enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].name_from_interface));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GTP___TT_NWK_SCOPED_TEID].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_PPPOE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].value_from_mapping = 42;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GTP___TT_NWK_SCOPED_TEID].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_PPPOE___ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___ONE_PASS_TUNNEL_TERMINATE_ETH].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].value_from_mapping = 42;
        }
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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 52;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___TI_FORWARDING].name_from_interface, "BIER___TI_FORWARDING", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___TI_FORWARDING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___MPLS_FORWARDING].name_from_interface, "BIER___MPLS_FORWARDING", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___MPLS_FORWARDING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL].name_from_interface, "BRIDGE___SVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_SVL].name_from_interface, "BRIDGE___IPV4MC_SVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_SVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_SVL].name_from_interface, "BRIDGE___IPV6MC_SVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_SVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NO_VFT].name_from_interface, "FCOE___FCF_NO_VFT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NO_VFT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_NO_VFT].name_from_interface, "FCOE___FCF_NPV_NO_VFT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_NO_VFT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_VFT].name_from_interface, "FCOE___FCF_VFT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_VFT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_VFT].name_from_interface, "FCOE___FCF_NPV_VFT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_VFT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT].name_from_interface, "FCOE___TRANSIT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT_FIP].name_from_interface, "FCOE___TRANSIT_FIP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT_FIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].name_from_interface, "GENERAL___NOP_CTX_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].name_from_interface, "IPV4___MC_PRIVATE_PUBLIC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].name_from_interface, "IPV4___MC_PRIVATE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].name_from_interface, "IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].name_from_interface, "IPV4___MC_PRIVATE_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].name_from_interface, "IPV4___PRIVATE_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].name_from_interface, "IPV4___PRIVATE_UC_W_OPTIONS", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].name_from_interface, "IPV4___PRIVATE_UC_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].name_from_interface, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].name_from_interface, "IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].name_from_interface, "IPV4___NOP_CTX_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX].name_from_interface, "IPV4___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].name_from_interface, "IPV6___MC_PRIVATE_PUBLIC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].name_from_interface, "IPV6___MC_PRIVATE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].name_from_interface, "IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].name_from_interface, "IPV6___MC_PRIVATE_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].name_from_interface, "IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].name_from_interface, "IPV6___PRIVATE_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].name_from_interface, "IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].name_from_interface, "IPV6___PRIVATE_UC_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].name_from_interface, "MPLS___FWD", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].name_from_interface, "MPLS___FWD_MACT_SA_LKP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].name_from_interface, "MPLS___FWD_PER_INTERFACE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].name_from_interface, "MPLS___OAM_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].name_from_interface, "SLLB___IPV4_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].name_from_interface, "SLLB___IPV6_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].name_from_interface, "SRV6___SEGMENT_END_POINT_FORWARDING", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].name_from_interface, "SRV6___CLASSIC_NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].name_from_interface, "SRV6___USID_END_POINT_FORWARDING", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].name_from_interface, "SRV6___USID_NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING].name_from_interface, "SRV6___GSID_ENDPOINT_FORWARDING", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL].name_from_interface, "BRIDGE___IVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_IVL].name_from_interface, "BRIDGE___IPV4MC_IVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_IVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_IVL].name_from_interface, "BRIDGE___IPV6MC_IVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_IVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].name_from_interface, "IPV4___NAT_UPSTREAM", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].name_from_interface, "IPV4___NAT_DOWNSTREAM", sizeof(enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].name_from_interface));
    }
    if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___TI_FORWARDING].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___MPLS_FORWARDING].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NO_VFT].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_NO_VFT].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_VFT].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_VFT].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT_FIP].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_IVL].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_IVL].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].value_from_mapping = 30;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___TI_FORWARDING].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___MPLS_FORWARDING].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NO_VFT].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_NO_VFT].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_VFT].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_VFT].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT_FIP].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_IVL].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_IVL].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___TI_FORWARDING].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___MPLS_FORWARDING].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NO_VFT].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_NO_VFT].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_VFT].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___FCF_NPV_VFT].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_FCOE___TRANSIT_FIP].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV4MC_IVL].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IPV6MC_IVL].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].value_from_mapping = 30;
        }
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
    
    
    
    
    cur_field_type_param->nof_enum_vals = 49;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface, "GENERAL___NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL].name_from_interface, "BRIDGE___IPV4MC_SVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL].name_from_interface, "BRIDGE___IPV6MC_SVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL].name_from_interface, "BRIDGE___SVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF].name_from_interface, "FCOE___FCF", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP].name_from_interface, "FCOE___FCF_FIP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface, "GENERAL___RCH_TERM_EXT_ENCAP", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B].name_from_interface, "IPV4___MC_PRIVATE_W_BF_W_F2B", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B].name_from_interface, "IPV4___MC_PRIVATE_WO_BF_W_F2B", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B].name_from_interface, "IPV4___MC_PRIVATE_WO_BF_WO_F2B", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC].name_from_interface, "IPV4___PRIVATE_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY].name_from_interface, "IPV4___PRIVATE_UC_LPM_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B].name_from_interface, "IPV6___MC_PRIVATE_W_BF_W_F2B", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B].name_from_interface, "IPV6___MC_PRIVATE_WO_BF_W_F2B", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B].name_from_interface, "IPV6___MC_PRIVATE_WO_BF_WO_F2B", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC].name_from_interface, "IPV6___PRIVATE_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY].name_from_interface, "IPV6___PRIVATE_UC_LPM_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD].name_from_interface, "IPV6___PRIVATE_UC_BFD", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY].name_from_interface, "MPLS___OAM_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD].name_from_interface, "MPLS___FWD", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE].name_from_interface, "SLLB___VIRTUAL_IPV4_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE].name_from_interface, "SLLB___VIRTUAL_IPV6_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE].name_from_interface, "SLLB___SIMPLE_IPV4_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE].name_from_interface, "SLLB___SIMPLE_IPV6_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].name_from_interface, "SRV6___PRIVATE_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].name_from_interface, "SRV6___CLASSIC_NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].name_from_interface, "SRV6___USID_PRIVATE_UC", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].name_from_interface, "SRV6___USID_NOP_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].name_from_interface));
        enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].is_invalid_value_from_mapping = !(dnx_data_headers.system_headers.system_headers_mode_get(unit));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_48].name_from_interface, "ACL_CONTEXT_48", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_49].name_from_interface, "ACL_CONTEXT_49", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_49].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_50].name_from_interface, "ACL_CONTEXT_50", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_50].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_51].name_from_interface, "ACL_CONTEXT_51", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_51].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_52].name_from_interface, "ACL_CONTEXT_52", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_52].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_53].name_from_interface, "ACL_CONTEXT_53", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_53].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_54].name_from_interface, "ACL_CONTEXT_54", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_54].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_55].name_from_interface, "ACL_CONTEXT_55", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_55].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_56].name_from_interface, "ACL_CONTEXT_56", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_56].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_57].name_from_interface, "ACL_CONTEXT_57", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_57].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_58].name_from_interface, "ACL_CONTEXT_58", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_58].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_59].name_from_interface, "ACL_CONTEXT_59", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_59].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_60].name_from_interface, "ACL_CONTEXT_60", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_60].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_61].name_from_interface, "ACL_CONTEXT_61", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_61].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_62].name_from_interface, "ACL_CONTEXT_62", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_62].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_63].name_from_interface, "ACL_CONTEXT_63", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_63].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_IVL].name_from_interface, "BRIDGE___IPV4MC_IVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_IVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_IVL].name_from_interface, "BRIDGE___IPV6MC_IVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_IVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IVL].name_from_interface, "BRIDGE___IVL", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IVL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM].name_from_interface, "IPV4___NAT_UPSTREAM", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].name_from_interface, "IPV4___NAT_DOWNSTREAM", sizeof(enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].name_from_interface));
    }
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_48].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_49].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_50].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_51].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_52].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_53].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_54].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_55].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_56].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_57].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_58].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_59].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_60].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_61].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_62].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_63].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_IVL].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_IVL].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IVL].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_48].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_49].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_50].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_51].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_52].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_53].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_54].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_55].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_56].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_57].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_58].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_59].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_60].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_61].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_62].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_63].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_IVL].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_IVL].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IVL].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].value_from_mapping = 16;
        }
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
