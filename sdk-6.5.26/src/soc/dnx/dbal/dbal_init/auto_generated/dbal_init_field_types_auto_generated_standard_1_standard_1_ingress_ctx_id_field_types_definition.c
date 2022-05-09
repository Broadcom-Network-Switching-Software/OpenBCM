
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VT1_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___FTMH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED, "BRIDGE___UNTAGGED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG, "BRIDGE___C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG, "BRIDGE___S_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS, "BRIDGE___S_C_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_C_TAGS, "BRIDGE___C_C_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_S_TAGS, "BRIDGE___S_S_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG, "BRIDGE___E_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG, "BRIDGE___E_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG, "BRIDGE___E_S_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS, "BRIDGE___E_S_C_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS, "BRIDGE___S_PRIORITY_C_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR, "BRIDGE___UNTAGGED_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG_FRR, "BRIDGE___C_TAG_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG_FRR, "BRIDGE___S_TAG_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS_FRR, "BRIDGE___S_C_TAGS_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG_FRR, "BRIDGE___E_TAG_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG_FRR, "BRIDGE___E_C_TAG_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG_FRR, "BRIDGE___E_S_TAG_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS_FRR, "BRIDGE___E_S_C_TAGS_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS_FRR, "BRIDGE___S_PRIORITY_C_TAGS_FRR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_C_TAG, "BRIDGE___PON_STC_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_S_TAG, "BRIDGE___PON_STC_S_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_UNTAGGED, "BRIDGE___PON_STC_UNTAGGED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_SP_C_TAG, "BRIDGE___PON_DTC_SP_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_C_TAG, "BRIDGE___PON_DTC_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_S_TAG, "BRIDGE___PON_DTC_S_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_UNTAGGED, "BRIDGE___PON_DTC_UNTAGGED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP, "BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_DROP_AND_CONT, "GENERAL___RCH_TERM_DROP_AND_CONT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE, "GENERAL___RCH_TERM_JR_MODE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE, !(!dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR, "GENERAL___RCH_TERM_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION, "GENERAL___RCH_TERM_XTERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, "GENERAL___RCH_TERM_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_MPLS___RAW_MPLS, "MPLS___RAW_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP, "SRV6___RCH_SRV6_USP_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___FTMH, "GENERAL___FTMH");
    }
    if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_C_TAGS].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_S_TAGS].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG_FRR].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG_FRR].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS_FRR].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG_FRR].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG_FRR].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG_FRR].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS_FRR].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS_FRR].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_C_TAG].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_S_TAG].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_UNTAGGED].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_SP_C_TAG].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_C_TAG].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_S_TAG].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_UNTAGGED].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_DROP_AND_CONT].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_MPLS___RAW_MPLS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___FTMH].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_C_TAGS].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_S_TAGS].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___UNTAGGED_FRR].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___C_TAG_FRR].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_TAG_FRR].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_C_TAGS_FRR].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_TAG_FRR].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_C_TAG_FRR].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_TAG_FRR].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___E_S_C_TAGS_FRR].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___S_PRIORITY_C_TAGS_FRR].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_C_TAG].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_S_TAG].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_STC_UNTAGGED].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_SP_C_TAG].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_C_TAG].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_S_TAG].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PON_DTC_UNTAGGED].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_DROP_AND_CONT].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_JR_MODE].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_REFLECTOR].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_XTERMINATION].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_MPLS___RAW_MPLS].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_SRV6___RCH_SRV6_USP_PSP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT1_CONTEXT_ID_GENERAL___FTMH].value_from_mapping = 33;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VT2_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___BFER_2ND_PASS, "BIER___BFER_2ND_PASS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI, "FCOE___FCF_VRF_BY_VSI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT, "FCOE___FCF_VRF_BY_VFT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT, "FCOE___FCF_VRF_BY_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, "GENERAL___RCH_TERM_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION, "GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___MAC_SA_COMPRESSION, "GENERAL___MAC_SA_COMPRESSION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL, "IPV4___TT_P2P_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL, "IPV4___2ND_PASS_TT_P2P_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS, "IPV6___TT_SIP_MSBS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN, "MAC_IN_MAC___LEARN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT, "MPLS___TT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL, "MPLS___TT_PER_INTERFACE_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT, "MPLS___2ND_PASS_TT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL, "MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML, "MPLS___EVPN_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL, "MPLS___SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BRIDGE___NOP_IVL, "BRIDGE___NOP_IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC, "IPV4___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP, "IPV4___TT_IPSEC_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC, "IPV6___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP, "IPV6___TT_IPSEC_UDP");
    }
    if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___BFER_2ND_PASS].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___MAC_SA_COMPRESSION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BRIDGE___NOP_IVL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 15;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___BFER_2ND_PASS].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___MAC_SA_COMPRESSION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BRIDGE___NOP_IVL].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BIER___BFER_2ND_PASS].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VSI].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_VFT].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_FCOE___FCF_VRF_BY_PORT].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___GLOBAL_IN_LIF_RECLASSIFICATION].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_GENERAL___MAC_SA_COMPRESSION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___2ND_PASS_TT_P2P_TUNNEL].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_SIP_MSBS].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MAC_IN_MAC___LEARN].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___2ND_PASS_TT_PER_INTERFACE_LABEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_BRIDGE___NOP_IVL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT2_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VT3_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC_UDP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BNG___PPPOE_SA_COMPRESSION, "BNG___PPPOE_SA_COMPRESSION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT, "BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, "GENERAL___RCH_TERM_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX, "GENERAL___IPSEC_PARSING_CONTEXT_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPV4_MAC_SA_COMPRESSION, "GENERAL___IPV4_MAC_SA_COMPRESSION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VSI, "GENEVE___VNI2VSI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VRF, "GENEVE___VNI2VRF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3, "GRE_WITH_KEY___TNI2VRF_VTT3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3, "GRE_WITH_KEY___TNI2VSI_VTT3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM, "IPV4___TT_MP_AND_TCAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM, "IPV4___2ND_PASS_TT_MP_AND_TCAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM, "IPV4___TT_P2P_AND_TCAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___VRF_RESELECT, "IPV4___VRF_RESELECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP, "IPV6___TT_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP, "IPV6___2ND_PASS_TT_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX, "IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX, "IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS, "IPV6___IPV6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT, "MPLS___TT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL, "MPLS___TT_PER_INTERFACE_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML, "MPLS___EVPN_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML, "MPLS___EVPN_FL_AFTER_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML, "MPLS___EVPN_FL_AND_CW_AFTER_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX, "MPLS___SPECULATIVE_PARSING_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY, "MPLS___P2P_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL, "MPLS___SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3, "VXLAN_GPE___VNI2VSI_VTT3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3, "VXLAN_GPE___VNI2VRF_VTT3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___NOP_IVL, "BRIDGE___NOP_IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT, "MPLS___IFIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC, "IPV4___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_UDP, "IPV4___TT_IPSEC_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_DOUBLE_UDP, "IPV4___TT_IPSEC_DOUBLE_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC, "IPV6___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC_UDP, "IPV6___TT_IPSEC_UDP");
    }
    if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BNG___PPPOE_SA_COMPRESSION].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPV4_MAC_SA_COMPRESSION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___VRF_RESELECT].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___NOP_IVL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_DOUBLE_UDP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 21;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BNG___PPPOE_SA_COMPRESSION].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPV4_MAC_SA_COMPRESSION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___VRF_RESELECT].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___NOP_IVL].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_DOUBLE_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BNG___PPPOE_SA_COMPRESSION].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_DEFAULT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPSEC_PARSING_CONTEXT_FIX].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENERAL___IPV4_MAC_SA_COMPRESSION].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT3].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT3].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_MP_AND_TCAM].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___2ND_PASS_TT_MP_AND_TCAM].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_P2P_AND_TCAM].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___VRF_RESELECT].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___2ND_PASS_TT_DIP_W_IPV6_EH_PARSING_CONTEXT_FIX].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___IPV6_EH_PARSING_CXT_FIX_AND_SIP_COMPRESS].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECULATIVE_PARSING_FIX].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT3].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT3].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_BRIDGE___NOP_IVL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_MPLS___IFIT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV4___TT_IPSEC_DOUBLE_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT3_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VT4_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC_UDP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_CMPRS_SA, "BNG___PPPOE_TT_CMPRS_SA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_FULL_SA, "BNG___PPPOE_TT_FULL_SA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_WO_LENGTH, "BNG___L2TP_TT_WO_LENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_W_LENGTH, "BNG___L2TP_TT_W_LENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED, "BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING, "GENERAL___2ND_PARSING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VSI, "GENEVE___VNI2VSI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VRF, "GENEVE___VNI2VRF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4, "GRE_WITH_KEY___TNI2VRF_VTT4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4, "GRE_WITH_KEY___TNI2VSI_VTT4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL, "IPV4___TT_P2P_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL, "IPV4___TT_MP_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP, "IPV6___TT_SIP_IDX_DIP_OR_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP, "IPV6___TT_SIP_IDX_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16, "IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16, "IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16, "IPV6___TT_USID_PREFIX_32_USID_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16, "IPV6___TT_GSID_PREFIX_48_GSID_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16, "IPV6___TT_GSID_PREFIX_64_GSID_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT, "IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT, "IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT, "IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT, "IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT, "IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN, "MAC_IN_MAC___ISID_WITHOUT_DOMAIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN, "MAC_IN_MAC___ISID_WITH_DOMAIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION, "MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION, "MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT, "MPLS___TT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL, "MPLS___TT_PER_INTERFACE_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML, "MPLS___EVPN_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML, "MPLS___EVPN_FL_AFTER_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML, "MPLS___EVPN_FL_AND_CW_AFTER_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY, "MPLS___P2P_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL, "MPLS___SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4, "VXLAN_GPE___VNI2VSI_VTT4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4, "VXLAN_GPE___VNI2VRF_VTT4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT, "MPLS___IFIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC, "IPV4___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC_UDP, "IPV4___TT_IPSEC_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC, "IPV6___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC_UDP, "IPV6___TT_IPSEC_UDP");
    }
    if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_CMPRS_SA].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_FULL_SA].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_WO_LENGTH].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_W_LENGTH].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 18;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_CMPRS_SA].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_FULL_SA].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_WO_LENGTH].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_W_LENGTH].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_CMPRS_SA].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___PPPOE_TT_FULL_SA].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_WO_LENGTH].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BNG___L2TP_TT_W_LENGTH].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_TAGGED].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENERAL___2ND_PARSING].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT4].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT4].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP_OR_DIP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_SIP_IDX_DIP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_USID_PREFIX_32_USID_16_NO_DEFAULT].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_48_GSID_16_NO_DEFAULT].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_GSID_PREFIX_64_GSID_16_NO_DEFAULT].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITHOUT_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MAC_IN_MAC___ISID_WITH_DOMAIN_LEAF_NODE_OPTIMIZATION].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AFTER_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___EVPN_FL_AND_CW_AFTER_IML].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT4].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT4].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_MPLS___IFIT].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT4_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VT5_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BNG___PPPOE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK, "BNG___PPPOE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED, "BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED, "BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED, "BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED, "BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED, "BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED, "BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC, "BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC, "BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC, "BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC, "BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC, "BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC, "BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER, "BRIDGE___PON_DTC_OTHER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG, "BRIDGE___PON_DTC_C_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG, "BRIDGE___PON_DTC_S_S_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG, "BRIDGE___PON_DTC_S_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_SPECIFIC, "BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_SPECIFIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED, "BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___IPSEC_LAYER_FIX, "GENERAL___IPSEC_LAYER_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSI, "GENEVE___VNI2VSI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRF, "GENEVE___VNI2VRF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSIOIPV6, "GENEVE___VNI2VSIOIPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRFOIPV6, "GENEVE___VNI2VRFOIPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5, "GRE_WITH_KEY___TNI2VRF_VTT5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5, "GRE_WITH_KEY___TNI2VSI_VTT5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL, "IPV4___TT_P2P_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL, "IPV4___TT_MP_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT, "MPLS___TT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL, "MPLS___TT_PER_INTERFACE_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML, "MPLS___EVPN_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY, "MPLS___P2P_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL, "MPLS___SPECIAL_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX, "MPLS___GAL_WITH_FLOW_LABEL_FIX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE, "SRV6___EXTENDED_TUNNEL_TERMINATE");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT, "SRV6___CLASSIC_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT, "SRV6___USID_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0, "SRV6___GSID_ENDPOINT_SID_EXTRACT_0");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1, "SRV6___GSID_ENDPOINT_SID_EXTRACT_1");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2, "SRV6___GSID_ENDPOINT_SID_EXTRACT_2");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_3, "SRV6___GSID_ENDPOINT_SID_EXTRACT_3");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_3, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_UPF___TT_NWK_SCOPED_TEID, "UPF___TT_NWK_SCOPED_TEID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5, "VXLAN_GPE___VNI2VSI_VTT5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5, "VXLAN_GPE___VNI2VRF_VTT5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC, "IPV4___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP, "IPV4___TT_IPSEC_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC, "IPV6___TT_IPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP, "IPV6___TT_IPSEC_UDP");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BNG___PPPOE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_SPECIFIC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___IPSEC_LAYER_FIX].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSIOIPV6].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRFOIPV6].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_3].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_UPF___TT_NWK_SCOPED_TEID].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BNG___PPPOE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_SPECIFIC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___IPSEC_LAYER_FIX].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSIOIPV6].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRFOIPV6].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_3].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_UPF___TT_NWK_SCOPED_TEID].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].value_from_mapping = 11;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BNG___PPPOE_ADDITIONAL_PROCESSING_PPP_TT_SESSION_ID_SPOOFING_CHECK].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_NWK_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_0_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_1_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___INNER_ETHERNET_2_VTAG_LIF_SCOPED_LEARN_NATIVE_AC].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_OTHER].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_C_C_TAG].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_S_TAG].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PON_DTC_S_C_TAG].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED_VLAN_SPECIFIC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_SA_BASED].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENERAL___IPSEC_LAYER_FIX].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSI].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRF].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VSIOIPV6].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GENEVE___VNI2VRFOIPV6].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VRF_VTT5].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_GRE_WITH_KEY___TNI2VSI_VTT5].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_P2P_TUNNEL].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_MP_TUNNEL].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___TT_PER_INTERFACE_LABEL].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___EVPN_IML].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___P2P_ONLY].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___SPECIAL_LABEL].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_MPLS___GAL_WITH_FLOW_LABEL_FIX].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___EXTENDED_TUNNEL_TERMINATE].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___CLASSIC_ENDPOINT].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___USID_ENDPOINT].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_0].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_1].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_2].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_SRV6___GSID_ENDPOINT_SID_EXTRACT_3].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_UPF___TT_NWK_SCOPED_TEID].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VSI_VTT5].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_VXLAN_GPE___VNI2VRF_VTT5].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV4___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_VT5_CONTEXT_ID_IPV6___TT_IPSEC_UDP].is_invalid_value_from_mapping = TRUE;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD1_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR, "BIER___NON_MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR_SA_LKP, "BIER___NON_MPLS_BFR_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP, "BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK, "BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK, "BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP, "BRIDGE___SVL__W_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK, "BRIDGE___SVL__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK, "BRIDGE___SVL__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP, "BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK, "BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP, "BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK, "BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP, "GENERAL___NOP_CTX_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, "GENERAL___RCH_TERM_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC, "IPV4___MC_PRIVATE_PUBLIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE, "IPV4___MC_PRIVATE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP, "IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP, "IPV4___MC_PRIVATE_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC, "IPV4___PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS, "IPV4___PRIVATE_UC_W_OPTIONS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP, "IPV4___PRIVATE_UC_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP, "IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP, "IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP, "IPV4___NOP_CTX_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC, "IPV6___MC_PRIVATE_PUBLIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE, "IPV6___MC_PRIVATE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP, "IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP, "IPV6___MC_PRIVATE_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC, "IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC, "IPV6___PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP, "IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP, "IPV6___PRIVATE_UC_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___NOP_CTX_MACT_SA_LKP, "IPV6___NOP_CTX_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD, "MPLS___FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP, "MPLS___FWD_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE, "MPLS___FWD_PER_INTERFACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY, "MPLS___OAM_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_OAM___BRIDGE_TRUNK, "OAM___BRIDGE_TRUNK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE, "SLLB___IPV4_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE, "SLLB___IPV6_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING, "SRV6___SEGMENT_END_POINT_FORWARDING");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX, "SRV6___CLASSIC_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING, "SRV6___USID_END_POINT_FORWARDING");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX, "SRV6___USID_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_END_POINT_FORWARDING, "SRV6___USID_NO_DEFAULT_END_POINT_FORWARDING");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_END_POINT_FORWARDING, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX, "SRV6___USID_NO_DEFAULT_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_0, "SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_0");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_0, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_1, "SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_1");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_1, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_2, "SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_2");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_2, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_3, "SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_3");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_3, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_MACT_SA_LKP, "BRIDGE___IVL__W_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP, "BRIDGE___IVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP, "BRIDGE___IVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM, "IPV4___NAT_UPSTREAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM, "IPV4___NAT_DOWNSTREAM");
    }
    if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR_SA_LKP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___NOP_CTX_MACT_SA_LKP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_OAM___BRIDGE_TRUNK].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_END_POINT_FORWARDING].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_0].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_1].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_2].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_3].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_MACT_SA_LKP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].value_from_mapping = 31;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR_SA_LKP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___NOP_CTX_MACT_SA_LKP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_OAM___BRIDGE_TRUNK].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_END_POINT_FORWARDING].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_0].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_1].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_2].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_3].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR_SA_LKP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___WO_FORWARD_LKP__WO_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV4_SIP_ADDR_CK].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___SVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP__W_IPV6_SIP_ADDR_CK].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___NOP_CTX_MACT_SA_LKP].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_PUBLIC_OPTIMIZED_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___PRIVATE_UC_W_OPTIONS_MACT_SA_LKP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NOP_CTX_MACT_SA_LKP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_PUBLIC_MACT_SA_LKP].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___MC_PRIVATE_MACT_SA_LKP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_PUBLIC_OPTIMIZED_UC_MACT_SA_LKP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___PRIVATE_UC_MACT_SA_LKP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV6___NOP_CTX_MACT_SA_LKP].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_MACT_SA_LKP].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___FWD_PER_INTERFACE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_OAM___BRIDGE_TRUNK].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV4_ROUTE].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SLLB___IPV6_ROUTE].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___SEGMENT_END_POINT_FORWARDING].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_END_POINT_FORWARDING].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_END_POINT_FORWARDING].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_0].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_1].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_2].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_SRV6___GSID_ENDPOINT_FORWARDING_SID_EXTRACT_3].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_MACT_SA_LKP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV4MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BRIDGE___IVL__W_IPV6MC_FORWARD_LKP__W_MACT_SA_LKP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].value_from_mapping = 31;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD2_CONTEXT_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX, "GENERAL___NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BIER___NON_MPLS_BFR, "BIER___NON_MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL, "BRIDGE___IPV4MC_SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL, "BRIDGE___IPV6MC_SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL, "BRIDGE___SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_0TAG, "BRIDGE___EXTENDED_P2P_0TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_1TAG, "BRIDGE___EXTENDED_P2P_1TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_2TAG, "BRIDGE___EXTENDED_P2P_2TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P, "BRIDGE___EXTENDED_P2P");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF, "FCOE___FCF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP, "FCOE___FCF_FIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, "GENERAL___RCH_TERM_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B, "IPV4___MC_PRIVATE_W_BF_W_F2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B, "IPV4___MC_PRIVATE_WO_BF_W_F2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B, "IPV4___MC_PRIVATE_WO_BF_WO_F2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC, "IPV4___PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY, "IPV4___PRIVATE_UC_LPM_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___WO_FIB_LOOKUP, "IPV4___WO_FIB_LOOKUP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B, "IPV6___MC_PRIVATE_W_BF_W_F2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B, "IPV6___MC_PRIVATE_WO_BF_W_F2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B, "IPV6___MC_PRIVATE_WO_BF_WO_F2B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC, "IPV6___PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY, "IPV6___PRIVATE_UC_LPM_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD, "IPV6___PRIVATE_UC_BFD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___WO_FIB_LOOKUP, "IPV6___WO_FIB_LOOKUP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY, "MPLS___OAM_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD, "MPLS___FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE, "SLLB___VIRTUAL_IPV4_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE, "SLLB___VIRTUAL_IPV6_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE, "SLLB___SIMPLE_IPV4_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE, "SLLB___SIMPLE_IPV6_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC, "SRV6___PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX, "SRV6___CLASSIC_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC, "SRV6___USID_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX, "SRV6___USID_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_PRIVATE_UC, "SRV6___USID_NO_DEFAULT_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_PRIVATE_UC, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX, "SRV6___USID_NO_DEFAULT_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_NOP_CTX, "SRV6___GSID_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_NOP_CTX, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_PRIVATE_UC, "SRV6___GSID_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_PRIVATE_UC, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_64B_PREFIX_PRIVATE_UC, "SRV6___GSID_64B_PREFIX_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_INVALID_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_64B_PREFIX_PRIVATE_UC, !(dnx_data_headers.system_headers.system_headers_mode_get(unit)));
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_48, "ACL_CONTEXT_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_49, "ACL_CONTEXT_49");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_50, "ACL_CONTEXT_50");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_51, "ACL_CONTEXT_51");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_52, "ACL_CONTEXT_52");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_53, "ACL_CONTEXT_53");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_54, "ACL_CONTEXT_54");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_55, "ACL_CONTEXT_55");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_56, "ACL_CONTEXT_56");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_57, "ACL_CONTEXT_57");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_58, "ACL_CONTEXT_58");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_59, "ACL_CONTEXT_59");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_60, "ACL_CONTEXT_60");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_61, "ACL_CONTEXT_61");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_62, "ACL_CONTEXT_62");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_ACL_CONTEXT_63, "ACL_CONTEXT_63");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_IVL, "BRIDGE___IPV4MC_IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_IVL, "BRIDGE___IPV6MC_IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IVL, "BRIDGE___IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM, "IPV4___NAT_UPSTREAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM, "IPV4___NAT_DOWNSTREAM");
    }
    if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_0TAG].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_1TAG].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_2TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___WO_FIB_LOOKUP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___WO_FIB_LOOKUP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_PRIVATE_UC].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_NOP_CTX].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_PRIVATE_UC].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_64B_PREFIX_PRIVATE_UC].value_from_mapping = 44;
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
    else if (DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___NOP_CTX].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BIER___NON_MPLS_BFR].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_SVL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_SVL].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___SVL].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_0TAG].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_1TAG].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P_2TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___EXTENDED_P2P].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_FCOE___FCF_FIP].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_GENERAL___RCH_TERM_EXT_ENCAP].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC_LPM_ONLY].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___WO_FIB_LOOKUP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_W_BF_W_F2B].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_LPM_ONLY].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC_BFD].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___WO_FIB_LOOKUP].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___OAM_ONLY].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_MPLS___FWD].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV4_ROUTE].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___VIRTUAL_IPV6_ROUTE].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV4_ROUTE].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SLLB___SIMPLE_IPV6_ROUTE].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___PRIVATE_UC].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___CLASSIC_NOP_CTX].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_PRIVATE_UC].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NOP_CTX].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_PRIVATE_UC].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___USID_NO_DEFAULT_NOP_CTX].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_NOP_CTX].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_PRIVATE_UC].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_SRV6___GSID_64B_PREFIX_PRIVATE_UC].value_from_mapping = 44;
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
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV4MC_IVL].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IPV6MC_IVL].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_BRIDGE___IVL].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM].value_from_mapping = 21;
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
