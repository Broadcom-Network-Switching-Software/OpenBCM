
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_aod_bool_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_AOD_BOOL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "AOD_BOOL", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_AOD_BOOL_TRUE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_AOD_BOOL_FALSE, "FALSE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_AOD_BOOL_TRUE, "TRUE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_AOD_BOOL_FALSE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_AOD_BOOL_TRUE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_erspan_v3_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ERSPAN_V3_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ERSPAN_V3_CANDIDATE_HEADER", 160, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 18;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VERSION_AG;
    cur_field_types_info->struct_field_info[0].length = 4;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_VLAN_AG;
    cur_field_types_info->struct_field_info[1].length = 12;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_COS_AG;
    cur_field_types_info->struct_field_info[2].length = 3;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_ENABLE_AG;
    cur_field_types_info->struct_field_info[3].length = 2;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_T_AG;
    cur_field_types_info->struct_field_info[4].length = 1;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_SESSION_ID_AG;
    cur_field_types_info->struct_field_info[5].length = 10;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_TIMESTAP_AG;
    cur_field_types_info->struct_field_info[6].length = 32;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_SGT_AG;
    cur_field_types_info->struct_field_info[7].length = 16;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_P_AG;
    cur_field_types_info->struct_field_info[8].length = 1;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_FT_AG;
    cur_field_types_info->struct_field_info[9].length = 5;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_HW_ID_AG;
    cur_field_types_info->struct_field_info[10].length = 6;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_DIRECTION_AG;
    cur_field_types_info->struct_field_info[11].length = 1;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_GRA_AG;
    cur_field_types_info->struct_field_info[12].length = 2;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_O_AG;
    cur_field_types_info->struct_field_info[13].length = 1;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_PLATFORM_ID_AG;
    cur_field_types_info->struct_field_info[14].length = 6;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_SWITCH_ID_AG;
    cur_field_types_info->struct_field_info[15].length = 10;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_PORT_ID_AG;
    cur_field_types_info->struct_field_info[16].length = 16;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_TIMESTAMP2_AG;
    cur_field_types_info->struct_field_info[17].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_erspan_v3_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ERSPAN_V3_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ERSPAN_V3_ENC1_CANDIDATE_HEADER", 160, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 18;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_TIMESTAMP2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PORT_ID_AG;
    cur_field_types_info->struct_field_info[1].length = 16;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_SWITCH_ID_AG;
    cur_field_types_info->struct_field_info[2].length = 10;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_PLATFORM_ID_AG;
    cur_field_types_info->struct_field_info[3].length = 6;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_O_AG;
    cur_field_types_info->struct_field_info[4].length = 1;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_GRA_AG;
    cur_field_types_info->struct_field_info[5].length = 2;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_DIRECTION_AG;
    cur_field_types_info->struct_field_info[6].length = 1;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_HW_ID_AG;
    cur_field_types_info->struct_field_info[7].length = 6;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_FT_AG;
    cur_field_types_info->struct_field_info[8].length = 5;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_P_AG;
    cur_field_types_info->struct_field_info[9].length = 1;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_SGT_AG;
    cur_field_types_info->struct_field_info[10].length = 16;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_TIMESTAP_AG;
    cur_field_types_info->struct_field_info[11].length = 32;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_SESSION_ID_AG;
    cur_field_types_info->struct_field_info[12].length = 10;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_T_AG;
    cur_field_types_info->struct_field_info[13].length = 1;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_ENABLE_AG;
    cur_field_types_info->struct_field_info[14].length = 2;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_COS_AG;
    cur_field_types_info->struct_field_info[15].length = 3;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_VLAN_AG;
    cur_field_types_info->struct_field_info[16].length = 12;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_VERSION_AG;
    cur_field_types_info->struct_field_info[17].length = 4;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_etpp_encapsulation_main_header_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENCAPSULATION_MAIN_HEADER_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_RAW + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_256_128, "BIER_MPLS_1ST_256_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_64, "BIER_MPLS_1ST_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_256, "BIER_MPLS_2ND_256");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_128_64, "BIER_MPLS_2ND_128_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_256_128, "BIER_TI_1ST_256_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_64, "BIER_TI_1ST_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_256_HDR_START, "BIER_TI_2ND_256_HDR_START");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_HDR_START, "BIER_TI_2ND_HDR_START");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_TRILL, "TRILL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_1, "MPLS_PUSH_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_2, "MPLS_PUSH_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_1, "MPLS_TANDEM_PUSH_1_PLUS_PUSH_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_2, "MPLS_TANDEM_PUSH_1_PLUS_PUSH_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_1, "MPLS_TANDEM_PUSH_2_PLUS_PUSH_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_2, "MPLS_TANDEM_PUSH_2_PLUS_PUSH_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021Q, "8021Q");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021AH, "8021AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPFIX_PSAMP, "IPFIX_PSAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V2, "ERSPAN_V2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V3, "ERSPAN_V3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_RAW, "RAW");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_256_128].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_64].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_256].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_128_64].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_256_128].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_64].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_256_HDR_START].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_HDR_START].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV6].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV4].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_TRILL].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_1].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_2].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_1].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_2].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_1].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_2].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021Q].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021AH].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPFIX_PSAMP].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V2].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V3].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_RAW].value_from_mapping = 24;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_etpp_etps_formats_msb_2_bits_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ETPS_FORMATS_MSB_2_BITS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ETPS_FORMATS_MSB_2_BITS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY, "DATA_ENTRY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_etpp_forwarding_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_FORWARDING_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_FORWARDING_CANDIDATE_HEADER", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_ETHERNET + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLPUSHFWD, "NULLPUSHFWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLREMOVEFWD, "NULLREMOVEFWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAP, "MPLSSWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLS2SELF, "MPLS2SELF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAPCOUPLED, "MPLSSWAPCOUPLED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSPHP, "MPLSPHP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_TRILL, "TRILL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_MPLS, "BIER_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_TI, "BIER_TI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_ETHERNET, "ETHERNET");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLPUSHFWD].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLREMOVEFWD].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLS2SELF].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAPCOUPLED].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSPHP].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV4].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV6].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_TRILL].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_MPLS].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_TI].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_ETHERNET].value_from_mapping = 11;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_etpp_fwd_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_FWD_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_FWD_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_SRV6_ENDPOINT_MIDDLE_OF_COMPRESSION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR, "L2_INTERNAL_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC, "L2_INTERNAL_REFLECTOR_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_SRV6_ENDPOINT_MIDDLE_OF_COMPRESSION, "SRV6_ENDPOINT_MIDDLE_OF_COMPRESSION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_SRV6_ENDPOINT_MIDDLE_OF_COMPRESSION].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_fer_trap_same_interface_filter_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FER_TRAP_SAME_INTERFACE_FILTER_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FER_TRAP_SAME_INTERFACE_FILTER_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FER_TRAP_SAME_INTERFACE_FILTER_PROFILE_IP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FER_TRAP_SAME_INTERFACE_FILTER_PROFILE_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FER_TRAP_SAME_INTERFACE_FILTER_PROFILE_IP, "IP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FER_TRAP_SAME_INTERFACE_FILTER_PROFILE_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FER_TRAP_SAME_INTERFACE_FILTER_PROFILE_IP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_fhei_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FHEI_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FHEI_SIZE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FHEI_SIZE_8B + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FHEI_SIZE_0B, "0B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FHEI_SIZE_3B, "3B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FHEI_SIZE_5B, "5B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FHEI_SIZE_8B, "8B");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_0B].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_3B].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_5B].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_8B].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_general_defines_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GENERAL_DEFINES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "GENERAL_DEFINES", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS, "FWD2_ACL_NOF_CTXS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX, "FWD2_ACL_START_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_gtp_base_header_key_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GTP_BASE_HEADER_KEY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "GTP_BASE_HEADER_KEY", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VERSION_AG;
    cur_field_types_info->struct_field_info[0].length = 3;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PT_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_TEID_EXIST_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_gtp_v1_header_key_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GTP_V1_HEADER_KEY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "GTP_V1_HEADER_KEY", 19, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 5;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_E_AG;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_S_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_N_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MESSAGE_TYPE_AG;
    cur_field_types_info->struct_field_info[3].length = 8;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_NEXT_EXT_AG;
    cur_field_types_info->struct_field_info[4].length = 8;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_iana_ip_protocol_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IANA_IP_PROTOCOL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IANA_IP_PROTOCOL", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP, "HOPBYHOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE, "IPV6_ROUTE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG, "IPV6_FRAG");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE].value_from_mapping = 43;
        enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG].value_from_mapping = 44;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_ipv6_2nd_dual_homing_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPV6_2ND_DUAL_HOMING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPV6_2ND_DUAL_HOMING", 256, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ETPS_TOS_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 128;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ETPS_TOS_PLUS1_ARRAY_AG;
    cur_field_types_info->struct_field_info[1].length = 128;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_jr2_system_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_JR2_SYSTEM_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "JR2_SYSTEM_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE, "JR1_MODE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE, "JR2_MODE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_lif_fwd1_csp_1bit_eth_rif_bridge_fallback_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE, "IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE, "IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_lif_vtt5_csp_1bit_nat_direction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT5_CSP_1BIT_NAT_DIRECTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT5_CSP_1BIT_NAT_DIRECTION", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_macsec_error_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MACSEC_ERROR_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MACSEC_ERROR_CODE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_REPLAY_FAILURE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SP_TCAM_MISS, "SP_TCAM_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_TAG_CONTROL_PORT_DISABLED_PKT, "TAG_CONTROL_PORT_DISABLED_PKT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_UNTAG_CONTROL_PORT_DISABLED_PKT, "UNTAG_CONTROL_PORT_DISABLED_PKT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_IPV4_CHECKSUM_MISMATCH_OR_MPLS_BOS_NOT_FOUND, "IPV4_CHECKSUM_MISMATCH_OR_MPLS_BOS_NOT_FOUND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_INVALID_SECTAG, "INVALID_SECTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SC_TCAM_MISS, "SC_TCAM_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NO_SA, "NO_SA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_REPLAY_FAILURE, "REPLAY_FAILURE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SP_TCAM_MISS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_TAG_CONTROL_PORT_DISABLED_PKT].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_UNTAG_CONTROL_PORT_DISABLED_PKT].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_IPV4_CHECKSUM_MISMATCH_OR_MPLS_BOS_NOT_FOUND].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_INVALID_SECTAG].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SC_TCAM_MISS].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NO_SA].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_REPLAY_FAILURE].value_from_mapping = 23;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls1plus1_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS1PLUS1_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS1PLUS1_JR2_A0_ENC1_CANDIDATE_HEADER", 64, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls1plus2_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS1PLUS2_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS1PLUS2_JR2_A0_ENC1_CANDIDATE_HEADER", 96, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls2mpls1_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS2MPLS1_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS2MPLS1_JR2_A0_ENC1_CANDIDATE_HEADER", 96, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_AH_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls2mpls2_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS2MPLS2_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS2MPLS2_JR2_A0_ENC1_CANDIDATE_HEADER", 128, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 64;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls2mpls3_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS2MPLS3_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS2MPLS3_JR2_A0_ENC1_CANDIDATE_HEADER", 160, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls2plus1_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS2PLUS1_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS2PLUS1_JR2_A0_ENC1_CANDIDATE_HEADER", 96, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls2plus2_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS2PLUS2_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS2PLUS2_JR2_A0_ENC1_CANDIDATE_HEADER", 128, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls2_jr2_a0_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS2_JR2_A0_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS2_JR2_A0_ENC1_CANDIDATE_HEADER", 64, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_1plus1_enc1_candidate_header_j2b0_above_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_1PLUS1_ENC1_CANDIDATE_HEADER_J2B0_ABOVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_1PLUS1_ENC1_CANDIDATE_HEADER_J2B0_ABOVE", 192, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_3AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_1plus2_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_1PLUS2_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_1PLUS2_ENC1_CANDIDATE_HEADER", 96, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_1plus2_enc1_candidate_header_j2b0_above_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_1PLUS2_ENC1_CANDIDATE_HEADER_J2B0_ABOVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_1PLUS2_ENC1_CANDIDATE_HEADER_J2B0_ABOVE", 192, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_3AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus1_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_2PLUS1_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_2PLUS1_ENC1_CANDIDATE_HEADER", 96, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus1_enc1_candidate_header_j2b0_above_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_2PLUS1_ENC1_CANDIDATE_HEADER_J2B0_ABOVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_2PLUS1_ENC1_CANDIDATE_HEADER_J2B0_ABOVE", 192, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_3AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus2_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_2PLUS2_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_2PLUS2_ENC1_CANDIDATE_HEADER", 128, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus2_enc1_candidate_header_j2b0_above_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_2PLUS2_ENC1_CANDIDATE_HEADER_J2B0_ABOVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_2PLUS2_ENC1_CANDIDATE_HEADER_J2B0_ABOVE", 224, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 5;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_3AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MPLS_1_1_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_MPLS_1_2_AG;
    cur_field_types_info->struct_field_info[4].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_2_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_2_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_2_ENC1_CANDIDATE_HEADER", 64, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_2_enc1_candidate_header_j2b0_above_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_2_ENC1_CANDIDATE_HEADER_J2B0_ABOVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_2_ENC1_CANDIDATE_HEADER_J2B0_ABOVE", 160, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MPLS_3AH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_0_1_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MPLS_0_2_AG;
    cur_field_types_info->struct_field_info[2].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_qualifier_label_range_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_QUALIFIER_LABEL_RANGE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_QUALIFIER_LABEL_RANGE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE, "SPECIAL_LABEL_RANGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE, "IML_LABEL_RANGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_BIER_BIFT_LABEL_RANGE, "BIER_BIFT_LABEL_RANGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE, "DEFAULT_LABEL_RANGE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_BIER_BIFT_LABEL_RANGE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_mpls_special_label_nibble_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_SPECIAL_LABEL_NIBBLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_SPECIAL_LABEL_NIBBLE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_15 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_IPV4_EXPLICIT_NULL, "IPV4_EXPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_ROUTER_ALERT, "ROUTER_ALERT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_IPV6_EXPLICIT_NULL, "IPV6_EXPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_3, "NIBBLE_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_4, "NIBBLE_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_5, "NIBBLE_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_6, "NIBBLE_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_ELI, "ELI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_8, "NIBBLE_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_9, "NIBBLE_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_10, "NIBBLE_10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_11, "NIBBLE_11");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_12, "NIBBLE_12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_GAL, "GAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_OAM_ALERT, "OAM_ALERT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_15, "NIBBLE_15");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_IPV4_EXPLICIT_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_ROUTER_ALERT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_IPV6_EXPLICIT_NULL].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_3].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_4].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_5].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_6].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_ELI].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_8].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_9].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_10].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_11].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_12].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_GAL].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_OAM_ALERT].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_MPLS_SPECIAL_LABEL_NIBBLE_NIBBLE_15].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_network_header_sizes_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_HEADER_SIZES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NETWORK_HEADER_SIZES", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_VXLAN_AND_UDP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_128_BITSTRING, "BIER_128_BITSTRING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_256_BITSTRING, "BIER_256_BITSTRING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_64_BITSTRING, "BIER_64_BITSTRING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_NON_MPLS_FIRST_4B, "BIER_NON_MPLS_FIRST_4B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV2, "ERSPANV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV3, "ERSPANV3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_1_VLAN, "ETH_1_VLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_2_VLANS, "ETH_2_VLANS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS, "ETH_3_VLANS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS_SVTAG, "ETH_3_VLANS_SVTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_BASE, "ETH_BASE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETHERTYPE_ONLY, "ETHERTYPE_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GENEVE, "GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GRE, "GRE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_OPTIONS_AND_8B_EXTENSION, "GTPV1_OPTIONS_AND_8B_EXTENSION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_WO_OPTIONS, "GTPV1_WO_OPTIONS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_W_OPTIONS, "GTPV1_W_OPTIONS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_FB, "INT_FB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_P4, "INT_P4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPFIX_PSAMP, "IPFIX_PSAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_DO_BASE, "IPV6_DO_BASE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE, "IPV4_BASE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GENEVE, "IPV4_BASE_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE4, "IPV4_BASE_GRE4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE8, "IPV4_BASE_GRE8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_UDP, "IPV4_AND_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_2UDP, "IPV4_AND_VXLAN_AND_2UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_UDP, "IPV4_AND_VXLAN_AND_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_ADDRESS, "IPV6_ADDRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_AND_SRH, "IPV6_AND_SRH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_L2TPV2_8B, "L2TPV2_8B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_10, "MAX_ENCAP_10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_32, "MAX_ENCAP_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_40, "MAX_ENCAP_40");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_10_LABELS, "MPLS_10_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_1_LABEL, "MPLS_1_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_2_LABELS, "MPLS_2_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_3_LABELS, "MPLS_3_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_4_LABELS, "MPLS_4_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_5_LABELS, "MPLS_5_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_6_LABELS, "MPLS_6_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_7_LABELS, "MPLS_7_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_8_LABELS, "MPLS_8_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_9_LABELS, "MPLS_9_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOE, "PPPOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOL2TPV2_10B, "PPPOL2TPV2_10B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOPPPOE, "PPPOPPPOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_28B, "SFLOW_HDR_DP_28B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_32B, "SFLOW_HDR_DP_32B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE, "SRH_BASE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_1_SID, "SRH_BASE_1_SID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_2_SIDS, "SRH_BASE_2_SIDS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_3_SIDS, "SRH_BASE_3_SIDS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TCP, "TCP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_THREE_IPV6_ADDRESSES, "THREE_IPV6_ADDRESSES");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TWO_IPV6_ADDRESSES, "TWO_IPV6_ADDRESSES");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_VXLAN_AND_UDP, "VXLAN_AND_UDP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_128_BITSTRING].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_256_BITSTRING].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_64_BITSTRING].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_NON_MPLS_FIRST_4B].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV2].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV3].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_1_VLAN].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_2_VLANS].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS_SVTAG].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_BASE].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETHERTYPE_ONLY].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GENEVE].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GRE].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_OPTIONS_AND_8B_EXTENSION].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_WO_OPTIONS].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_W_OPTIONS].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_FB].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_P4].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPFIX_PSAMP].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_DO_BASE].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GENEVE].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE4].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE8].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_UDP].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_2UDP].value_from_mapping = 44;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_UDP].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_ADDRESS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_AND_SRH].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_L2TPV2_8B].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_10].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_32].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_40].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_10_LABELS].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_1_LABEL].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_2_LABELS].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_3_LABELS].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_4_LABELS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_5_LABELS].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_6_LABELS].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_7_LABELS].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_8_LABELS].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_9_LABELS].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOE].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOL2TPV2_10B].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOPPPOE].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_28B].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_32B].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_1_SID].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_2_SIDS].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_3_SIDS].value_from_mapping = 56;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TCP].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_THREE_IPV6_ADDRESSES].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TWO_IPV6_ADDRESSES].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_UDP].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_VXLAN_AND_UDP].value_from_mapping = 16;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_oam_to_cpu_jr2_trap_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_TO_CPU_JR2_TRAP_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_TO_CPU_JR2_TRAP_CANDIDATE_HEADER", 280, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 5;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_J2_FHEI_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 40;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PPH_BASE_ARRAY_AG;
    cur_field_types_info->struct_field_info[1].length = 96;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_APPLICATION_SPECIFIC_EXT_ARRAY_AG;
    cur_field_types_info->struct_field_info[2].length = 48;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FTMH_ARRAY_AG;
    cur_field_types_info->struct_field_info[3].length = 80;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[4].length = 16;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_oam_to_cpu_jr_trap_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_TO_CPU_JR_TRAP_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_TO_CPU_JR_TRAP_CANDIDATE_HEADER", 246, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 7;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FHEI_AG;
    cur_field_types_info->struct_field_info[0].length = 24;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PPH_ARRAY_AG;
    cur_field_types_info->struct_field_info[1].length = 56;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_OTSH_ARRAY_AG;
    cur_field_types_info->struct_field_info[2].length = 48;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FTMH_ARRAY_AG;
    cur_field_types_info->struct_field_info[3].length = 72;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_OUTLIF_TO_STAMP_AG;
    cur_field_types_info->struct_field_info[4].length = 22;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_PP_DSP_AG;
    cur_field_types_info->struct_field_info[5].length = 8;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[6].length = 16;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_otmh_trap_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OTMH_TRAP_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OTMH_TRAP_CANDIDATE_HEADER", 144, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 5;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_J2_FTMH_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 80;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_OUTLIF_TO_STAMP_AG;
    cur_field_types_info->struct_field_info[1].length = 22;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_PP_DSP_AG;
    cur_field_types_info->struct_field_info[2].length = 10;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[3].length = 16;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_ACE_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[4].length = 16;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_port_termination_ptc_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PORT_TERMINATION_PTC_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PORT_TERMINATION_PTC_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_SRV6_USP_PSP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2, "PTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1, "PTCH1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE, "COE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH, "RCH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1, "PTCH2_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ITMH, "ITMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RAW, "RAW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1, "UD1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD2, "UD2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD3, "UD3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD4, "UD4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD5, "UD5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD6, "UD6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PON, "PON");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_SRV6_USP_PSP, "SRV6_USP_PSP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ITMH].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RAW].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD1].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD2].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD3].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD4].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD5].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_UD6].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PON].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_SRV6_USP_PSP].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_raw_enc1_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RAW_ENC1_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RAW_ENC1_CANDIDATE_HEADER", 288, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 5;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ETPS_TOS_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 128;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_QOS_VARIABLE_AG;
    cur_field_types_info->struct_field_info[1].length = 8;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_TTL_AG;
    cur_field_types_info->struct_field_info[2].length = 8;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_CURRENT_NEXT_PROTOCOL_AG;
    cur_field_types_info->struct_field_info[3].length = 16;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_ETPS_TOS_PLUS1_ARRAY_AG;
    cur_field_types_info->struct_field_info[4].length = 128;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_rch_trap_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RCH_TRAP_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RCH_TRAP_CANDIDATE_HEADER", 318, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 8;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_PPH_BASE_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 96;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_TSH_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_APPLICATION_SPECIFIC_EXT_ARRAY_AG;
    cur_field_types_info->struct_field_info[2].length = 48;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_LB_KEY_EXT_AG;
    cur_field_types_info->struct_field_info[3].length = 24;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_FTMH_ARRAY_AG;
    cur_field_types_info->struct_field_info[4].length = 80;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_OUTLIF_TO_STAMP_AG;
    cur_field_types_info->struct_field_info[5].length = 22;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_PP_DSP_AG;
    cur_field_types_info->struct_field_info[6].length = 8;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[7].length = 8;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_recycle_cmd_prt_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RECYCLE_CMD_PRT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RECYCLE_CMD_PRT_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2, "PTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_SRV6_USP_PSP, "SRV6_USP_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_1, "RESERVED_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_2, "RESERVED_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_ETH_APPEND64, "ETH_APPEND64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_3, "RESERVED_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_4, "RESERVED_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_5, "RESERVED_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_6, "RESERVED_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_7, "RESERVED_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_8, "RESERVED_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1, "802_1BR_TYPE1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2, "802_1BR_TYPE2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE, "COE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_SRV6_USP_PSP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_ETH_APPEND64].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_3].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_4].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_5].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_6].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_7].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_RESERVED_8].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE].value_from_mapping = 14;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_srh_base_flags_unifiedtype_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRH_BASE_FLAGS_UNIFIEDTYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SRH_BASE_FLAGS_UNIFIEDTYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID, "IPV6_128B_SID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED, "IPV4_32B_SID_NOT_SUPPORTED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID, "MPLS_20LSB_SID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED, "RESERVED_NOT_SUPPORTED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_stacking_trap_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STACKING_TRAP_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STACKING_TRAP_CANDIDATE_HEADER", 166, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 6;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_STACKING_EXTENSION_AG;
    cur_field_types_info->struct_field_info[0].length = 16;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_LB_KEY_EXT_AG;
    cur_field_types_info->struct_field_info[1].length = 24;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FTMH_ARRAY_AG;
    cur_field_types_info->struct_field_info[2].length = 80;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_OUTLIF_TO_STAMP_AG;
    cur_field_types_info->struct_field_info[3].length = 22;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_PP_DSP_AG;
    cur_field_types_info->struct_field_info[4].length = 8;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[5].length = 16;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_tail_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TAIL_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TAIL_HEADER", 240, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 14;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_LATENCY_IN_NS_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_NEW_INGRESS_1588_TOD_ARRAY_AG;
    cur_field_types_info->struct_field_info[1].length = 48;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_EGRESS_1588_TOD_48_ARRAY_AG;
    cur_field_types_info->struct_field_info[2].length = 48;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FTMH_SOURCE_SYSTEM_PORT_AG;
    cur_field_types_info->struct_field_info[3].length = 16;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_DST_SYS_PORT_AG;
    cur_field_types_info->struct_field_info[4].length = 16;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_QUEUE_SIZE_DATA_AG;
    cur_field_types_info->struct_field_info[5].length = 12;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_ZERO_PADDING_1_AG;
    cur_field_types_info->struct_field_info[6].length = 4;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_QUEUE_ID_AG;
    cur_field_types_info->struct_field_info[7].length = 18;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_ZERO_PADDING_2_AG;
    cur_field_types_info->struct_field_info[8].length = 6;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_NODE_ID_AG;
    cur_field_types_info->struct_field_info[9].length = 16;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_QUEUE_SIZE_BINNING_AG;
    cur_field_types_info->struct_field_info[10].length = 3;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_ZERO_PADDING_3_AG;
    cur_field_types_info->struct_field_info[11].length = 5;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_NIF_PORT_RATE_AG;
    cur_field_types_info->struct_field_info[12].length = 10;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_ZERO_PADDING_4_AG;
    cur_field_types_info->struct_field_info[13].length = 6;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_tdm_trap_candidate_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TDM_TRAP_CANDIDATE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TDM_TRAP_CANDIDATE_HEADER", dnx_data_dnx2_aod_sizes.AOD.TDM_TRAP_CANDIDATE_HEADER_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 5;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_OPTIMIZED_TDM_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_STANDARD_TDM_ARRAY_AG;
    cur_field_types_info->struct_field_info[1].length = 72;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_OUTLIF_TO_STAMP_AG;
    cur_field_types_info->struct_field_info[2].length = 22;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_PP_DSP_AG;
    cur_field_types_info->struct_field_info[3].length = dnx_data_dnx2_aod_sizes.AOD.TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get(unit);
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_STAMP_VALUE_AG;
    cur_field_types_info->struct_field_info[4].length = 16;
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_udh_nat_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_UDH_NAT_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "UDH_NAT_HEADER", 98, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 6;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_UDP_SRC_PORT_AG;
    cur_field_types_info->struct_field_info[0].length = 16;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_SIP_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_TRANSLATE_SOURCE_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_UDP_DST_PORT_AG;
    cur_field_types_info->struct_field_info[3].length = 16;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_DIP_AG;
    cur_field_types_info->struct_field_info[4].length = 32;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_TRANSLATE_DESTINATION_AG;
    cur_field_types_info->struct_field_info[5].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_vtt_lif_service_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VTT_LIF_SERVICE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VTT_LIF_SERVICE_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_EXTENDED_P2P_WITH_SOURCE_LOOKUP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP, "P2MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P, "P2P");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_EXTENDED_P2P_WITH_SOURCE_LOOKUP, "EXTENDED_P2P_WITH_SOURCE_LOOKUP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_EXTENDED_P2P_WITH_SOURCE_LOOKUP].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_vtt_mpls_special_label_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VTT_MPLS_SPECIAL_LABEL_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VTT_MPLS_SPECIAL_LABEL_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_TUNNEL_LABEL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL, "IPV4_EXPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL, "IPV6_EXPLICIT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ROUTER_ALERT, "ROUTER_ALERT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI, "ELI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_GAL, "GAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_OAM_ALERT, "OAM_ALERT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_INVALID_LABEL, "INVALID_LABEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_TUNNEL_LABEL, "TUNNEL_LABEL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV4_EXPLICIT_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_IPV6_EXPLICIT_NULL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ROUTER_ALERT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_ELI].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_GAL].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_OAM_ALERT].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_INVALID_LABEL].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_VTT_MPLS_SPECIAL_LABEL_PROFILE_TUNNEL_LABEL].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_vtt_tt_process_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VTT_TT_PROCESS_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VTT_TT_PROCESS_CODE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC, "MAC_IN_MAC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS, "MPLS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_profile_field_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_aod_bool_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_erspan_v3_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_erspan_v3_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_encapsulation_main_header_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_etps_formats_msb_2_bits_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_forwarding_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_fwd_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_fer_trap_same_interface_filter_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_fhei_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_general_defines_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_gtp_base_header_key_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_gtp_v1_header_key_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_iana_ip_protocol_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_ipv6_2nd_dual_homing_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_jr2_system_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_lif_fwd1_csp_1bit_eth_rif_bridge_fallback_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_lif_vtt5_csp_1bit_nat_direction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_macsec_error_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls1plus1_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls1plus2_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls2mpls1_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls2mpls2_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls2mpls3_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls2plus1_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls2plus2_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls2_jr2_a0_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_1plus1_enc1_candidate_header_j2b0_above_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_1plus2_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_1plus2_enc1_candidate_header_j2b0_above_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus1_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus1_enc1_candidate_header_j2b0_above_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus2_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_2plus2_enc1_candidate_header_j2b0_above_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_2_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_2_enc1_candidate_header_j2b0_above_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_qualifier_label_range_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_special_label_nibble_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_network_header_sizes_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_oam_to_cpu_jr2_trap_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_oam_to_cpu_jr_trap_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_otmh_trap_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_port_termination_ptc_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_raw_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_rch_trap_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_recycle_cmd_prt_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_srh_base_flags_unifiedtype_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_stacking_trap_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_tail_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_tdm_trap_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_udh_nat_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_lif_service_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_mpls_special_label_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_tt_process_code_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
