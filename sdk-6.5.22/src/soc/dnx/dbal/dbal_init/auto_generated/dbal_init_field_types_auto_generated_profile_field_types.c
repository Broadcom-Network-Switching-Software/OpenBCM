
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "AOD_BOOL" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_AOD_BOOL_TRUE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_FALSE].name_from_interface, "FALSE", sizeof(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_FALSE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_TRUE].name_from_interface, "TRUE", sizeof(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_TRUE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_auto_generated_profile_field_types_etpp_encapsulation_main_header_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_ENCAPSULATION_MAIN_HEADER_TYPE" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_RAW + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_256_128].name_from_interface, "BIER_MPLS_1ST_256_128", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_256_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_64].name_from_interface, "BIER_MPLS_1ST_64", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_1ST_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_256].name_from_interface, "BIER_MPLS_2ND_256", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_256].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_128_64].name_from_interface, "BIER_MPLS_2ND_128_64", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_MPLS_2ND_128_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_256_128].name_from_interface, "BIER_TI_1ST_256_128", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_256_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_64].name_from_interface, "BIER_TI_1ST_64", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_1ST_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_256_HDR_START].name_from_interface, "BIER_TI_2ND_256_HDR_START", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_256_HDR_START].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_HDR_START].name_from_interface, "BIER_TI_2ND_HDR_START", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_BIER_TI_2ND_HDR_START].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_TRILL].name_from_interface, "TRILL", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_TRILL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_1].name_from_interface, "MPLS_PUSH_1", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_2].name_from_interface, "MPLS_PUSH_2", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_PUSH_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_1].name_from_interface, "MPLS_TANDEM_PUSH_1_PLUS_PUSH_1", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_2].name_from_interface, "MPLS_TANDEM_PUSH_1_PLUS_PUSH_2", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_1_PLUS_PUSH_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_1].name_from_interface, "MPLS_TANDEM_PUSH_2_PLUS_PUSH_1", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_2].name_from_interface, "MPLS_TANDEM_PUSH_2_PLUS_PUSH_2", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_MPLS_TANDEM_PUSH_2_PLUS_PUSH_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021Q].name_from_interface, "8021Q", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021Q].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021AH].name_from_interface, "8021AH", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_8021AH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPFIX_PSAMP].name_from_interface, "IPFIX_PSAMP", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_IPFIX_PSAMP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V2].name_from_interface, "ERSPAN_V2", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V3].name_from_interface, "ERSPAN_V3", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_ERSPAN_V3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_RAW].name_from_interface, "RAW", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ENCAPSULATION_MAIN_HEADER_TYPE_RAW].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_ETPS_FORMATS_MSB_2_BITS" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY].name_from_interface, "DATA_ENTRY", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_FORWARDING_CANDIDATE_HEADER" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_ETHERNET + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLPUSHFWD].name_from_interface, "NULLPUSHFWD", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLPUSHFWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLREMOVEFWD].name_from_interface, "NULLREMOVEFWD", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_NULLREMOVEFWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAP].name_from_interface, "MPLSSWAP", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLS2SELF].name_from_interface, "MPLS2SELF", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLS2SELF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAPCOUPLED].name_from_interface, "MPLSSWAPCOUPLED", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSSWAPCOUPLED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSPHP].name_from_interface, "MPLSPHP", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_MPLSPHP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_TRILL].name_from_interface, "TRILL", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_TRILL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_MPLS].name_from_interface, "BIER_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_TI].name_from_interface, "BIER_TI", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_BIER_TI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_ETHERNET].name_from_interface, "ETHERNET", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FORWARDING_CANDIDATE_HEADER_ETHERNET].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_FWD_ACE_CTXT_VALUE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].name_from_interface, "NULL", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].name_from_interface, "L2_INTERNAL_REFLECTOR", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].name_from_interface, "L2_INTERNAL_REFLECTOR_MC", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].value_from_mapping = 2;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FHEI_SIZE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FHEI_SIZE_8B + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_0B].name_from_interface, "0B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_0B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_3B].name_from_interface, "3B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_3B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_5B].name_from_interface, "5B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_5B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_8B].name_from_interface, "8B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_8B].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "GENERAL_DEFINES" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS].name_from_interface, "FWD2_ACL_NOF_CTXS", sizeof(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX].name_from_interface, "FWD2_ACL_START_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_auto_generated_profile_field_types_iana_ip_protocol_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IANA_IP_PROTOCOL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IANA_IP_PROTOCOL" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP].name_from_interface, "HOPBYHOP", sizeof(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE].name_from_interface, "IPV6_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG].name_from_interface, "IPV6_FRAG", sizeof(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_auto_generated_profile_field_types_jr2_system_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_JR2_SYSTEM_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "JR2_SYSTEM_MODE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE].name_from_interface, "JR1_MODE", sizeof(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE].name_from_interface, "JR2_MODE", sizeof(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE].name_from_interface, "IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE", sizeof(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE].name_from_interface, "IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LIF_VTT5_CSP_1BIT_NAT_DIRECTION" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MACSEC_ERROR_CODE" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_REPLAY_FAILURE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SP_TCAM_MISS].name_from_interface, "SP_TCAM_MISS", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SP_TCAM_MISS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_TAG_CONTROL_PORT_DISABLED_PKT].name_from_interface, "TAG_CONTROL_PORT_DISABLED_PKT", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_TAG_CONTROL_PORT_DISABLED_PKT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_UNTAG_CONTROL_PORT_DISABLED_PKT].name_from_interface, "UNTAG_CONTROL_PORT_DISABLED_PKT", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_UNTAG_CONTROL_PORT_DISABLED_PKT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_IPV4_CHECKSUM_MISMATCH_OR_MPLS_BOS_NOT_FOUND].name_from_interface, "IPV4_CHECKSUM_MISMATCH_OR_MPLS_BOS_NOT_FOUND", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_IPV4_CHECKSUM_MISMATCH_OR_MPLS_BOS_NOT_FOUND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_INVALID_SECTAG].name_from_interface, "INVALID_SECTAG", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_INVALID_SECTAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SC_TCAM_MISS].name_from_interface, "SC_TCAM_MISS", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_SC_TCAM_MISS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NO_SA].name_from_interface, "NO_SA", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_NO_SA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_REPLAY_FAILURE].name_from_interface, "REPLAY_FAILURE", sizeof(enum_info[DBAL_ENUM_FVAL_MACSEC_ERROR_CODE_REPLAY_FAILURE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_auto_generated_profile_field_types_mpls_qualifier_label_range_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_QUALIFIER_LABEL_RANGE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MPLS_QUALIFIER_LABEL_RANGE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE].name_from_interface, "SPECIAL_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE].name_from_interface, "IML_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_BIER_BIFT_LABEL_RANGE].name_from_interface, "BIER_BIFT_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_BIER_BIFT_LABEL_RANGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE].name_from_interface, "DEFAULT_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_auto_generated_profile_field_types_network_header_sizes_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NETWORK_HEADER_SIZES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NETWORK_HEADER_SIZES" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_VXLAN_AND_UDP + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_128_BITSTRING].name_from_interface, "BIER_128_BITSTRING", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_128_BITSTRING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_256_BITSTRING].name_from_interface, "BIER_256_BITSTRING", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_256_BITSTRING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_64_BITSTRING].name_from_interface, "BIER_64_BITSTRING", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_64_BITSTRING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_NON_MPLS_FIRST_4B].name_from_interface, "BIER_NON_MPLS_FIRST_4B", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_NON_MPLS_FIRST_4B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV2].name_from_interface, "ERSPANV2", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV3].name_from_interface, "ERSPANV3", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS].name_from_interface, "ETH_3_VLANS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS_SVTAG].name_from_interface, "ETH_3_VLANS_SVTAG", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS_SVTAG].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_BASE].name_from_interface, "ETH_BASE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_BASE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETHERTYPE_ONLY].name_from_interface, "ETHERTYPE_ONLY", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETHERTYPE_ONLY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GENEVE].name_from_interface, "GENEVE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GENEVE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GRE].name_from_interface, "GRE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GRE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_WO_OPTIONS].name_from_interface, "GTPV1_WO_OPTIONS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_WO_OPTIONS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_FB].name_from_interface, "INT_FB", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_FB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_P4].name_from_interface, "INT_P4", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_P4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPFIX_PSAMP].name_from_interface, "IPFIX_PSAMP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPFIX_PSAMP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE].name_from_interface, "IPV4_BASE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GENEVE].name_from_interface, "IPV4_BASE_GENEVE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GENEVE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE4].name_from_interface, "IPV4_BASE_GRE4", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE8].name_from_interface, "IPV4_BASE_GRE8", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_BASE_GRE8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_UDP].name_from_interface, "IPV4_AND_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_2UDP].name_from_interface, "IPV4_AND_VXLAN_AND_2UDP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_2UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_UDP].name_from_interface, "IPV4_AND_VXLAN_AND_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV4_AND_VXLAN_AND_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_ADDRESS].name_from_interface, "IPV6_ADDRESS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_ADDRESS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_AND_SRH].name_from_interface, "IPV6_AND_SRH", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPV6_AND_SRH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_L2TPV2_6B].name_from_interface, "L2TPV2_6B", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_L2TPV2_6B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_10].name_from_interface, "MAX_ENCAP_10", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_10].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_32].name_from_interface, "MAX_ENCAP_32", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_40].name_from_interface, "MAX_ENCAP_40", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_40].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_1_LABEL].name_from_interface, "MPLS_1_LABEL", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_1_LABEL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_2_LABELS].name_from_interface, "MPLS_2_LABELS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_2_LABELS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_3_LABELS].name_from_interface, "MPLS_3_LABELS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_3_LABELS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_4_LABELS].name_from_interface, "MPLS_4_LABELS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_4_LABELS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_5_LABELS].name_from_interface, "MPLS_5_LABELS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_5_LABELS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOE].name_from_interface, "PPPOE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOL2TPV2_6B].name_from_interface, "PPPOL2TPV2_6B", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOL2TPV2_6B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOPPPOE].name_from_interface, "PPPOPPPOE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOPPPOE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_28B].name_from_interface, "SFLOW_HDR_DP_28B", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_28B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_32B].name_from_interface, "SFLOW_HDR_DP_32B", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SFLOW_HDR_DP_32B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE].name_from_interface, "SRH_BASE", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_1_SID].name_from_interface, "SRH_BASE_1_SID", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_1_SID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_2_SIDS].name_from_interface, "SRH_BASE_2_SIDS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_2_SIDS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_3_SIDS].name_from_interface, "SRH_BASE_3_SIDS", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_SRH_BASE_3_SIDS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TCP].name_from_interface, "TCP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TCP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_THREE_IPV6_ADDRESSES].name_from_interface, "THREE_IPV6_ADDRESSES", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_THREE_IPV6_ADDRESSES].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TWO_IPV6_ADDRESSES].name_from_interface, "TWO_IPV6_ADDRESSES", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_TWO_IPV6_ADDRESSES].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_UDP].name_from_interface, "UDP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_UDP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_VXLAN_AND_UDP].name_from_interface, "VXLAN_AND_UDP", sizeof(enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_VXLAN_AND_UDP].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_128_BITSTRING].value_from_mapping = 24;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_256_BITSTRING].value_from_mapping = 40;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_64_BITSTRING].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_BIER_NON_MPLS_FIRST_4B].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV2].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ERSPANV3].value_from_mapping = 20;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS].value_from_mapping = 26;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_3_VLANS_SVTAG].value_from_mapping = 30;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETH_BASE].value_from_mapping = 14;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_ETHERTYPE_ONLY].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GENEVE].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GRE].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_GTPV1_WO_OPTIONS].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_FB].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_INT_P4].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_IPFIX_PSAMP].value_from_mapping = 36;
        
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
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_L2TPV2_6B].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_10].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_32].value_from_mapping = 32;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MAX_ENCAP_40].value_from_mapping = 40;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_1_LABEL].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_2_LABELS].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_3_LABELS].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_4_LABELS].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_MPLS_5_LABELS].value_from_mapping = 20;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOE].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_NETWORK_HEADER_SIZES_PPPOL2TPV2_6B].value_from_mapping = 10;
        
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
_dbal_init_field_types_auto_generated_profile_field_types_srh_base_flags_unifiedtype_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRH_BASE_FLAGS_UNIFIEDTYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SRH_BASE_FLAGS_UNIFIEDTYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID].name_from_interface, "IPV6_128B_SID", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED].name_from_interface, "IPV4_32B_SID_NOT_SUPPORTED", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID].name_from_interface, "MPLS_20LSB_SID", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED].name_from_interface, "RESERVED_NOT_SUPPORTED", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
_dbal_init_field_types_auto_generated_profile_field_types_vtt_lif_service_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VTT_LIF_SERVICE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VTT_LIF_SERVICE_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].name_from_interface, "P2MP", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].name_from_interface, "P2P", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP].name_from_interface, "P2P_WITH_SOURCE_LOOKUP", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP].value_from_mapping = 2;
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VTT_TT_PROCESS_CODE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS + 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET].name_from_interface, "ETHERNET", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC].name_from_interface, "MAC_IN_MAC", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_encapsulation_main_header_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_etps_formats_msb_2_bits_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_forwarding_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_fwd_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_fhei_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_general_defines_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_iana_ip_protocol_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_jr2_system_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_lif_fwd1_csp_1bit_eth_rif_bridge_fallback_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_lif_vtt5_csp_1bit_nat_direction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_macsec_error_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_qualifier_label_range_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_network_header_sizes_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_srh_base_flags_unifiedtype_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_lif_service_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_tt_process_code_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
