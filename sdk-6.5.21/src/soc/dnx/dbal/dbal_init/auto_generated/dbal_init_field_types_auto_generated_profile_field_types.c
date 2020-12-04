
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IANA_IP_PROTOCOL" , dnx_data_aod_sizes.AOD.sizes_get(unit, 0)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP].name_from_interface, "HOPBYHOP", sizeof(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE].name_from_interface, "IPV6_ROUTE", sizeof(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG].name_from_interface, "IPV6_FRAG", sizeof(enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_HOPBYHOP].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_ROUTE].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IANA_IP_PROTOCOL_IPV6_FRAG].value_from_mapping = 44;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "AOD_BOOL" , dnx_data_aod_sizes.AOD.sizes_get(unit, 1)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_FALSE].name_from_interface, "FALSE", sizeof(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_FALSE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_TRUE].name_from_interface, "TRUE", sizeof(enum_info[DBAL_ENUM_FVAL_AOD_BOOL_TRUE].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_AOD_BOOL_FALSE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_AOD_BOOL_TRUE].value_from_mapping = 1;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SRH_BASE_FLAGS_UNIFIEDTYPE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 37)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID].name_from_interface, "IPV6_128B_SID", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED].name_from_interface, "IPV4_32B_SID_NOT_SUPPORTED", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID].name_from_interface, "MPLS_20LSB_SID", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED].name_from_interface, "RESERVED_NOT_SUPPORTED", sizeof(enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV6_128B_SID].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_IPV4_32B_SID_NOT_SUPPORTED].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_MPLS_20LSB_SID].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_SRH_BASE_FLAGS_UNIFIEDTYPE_RESERVED_NOT_SUPPORTED].value_from_mapping = 3;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_oam_op_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_OP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAM_OP" , dnx_data_aod_sizes.AOD.sizes_get(unit, 45)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 24;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_CCM].name_from_interface, "CCM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_CCM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LBR].name_from_interface, "LBR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LBR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LBM].name_from_interface, "LBM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LBM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LTR].name_from_interface, "LTR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LTR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LTM].name_from_interface, "LTM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LTM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_AIS].name_from_interface, "AIS", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_AIS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LCK].name_from_interface, "LCK", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LCK].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_TST].name_from_interface, "TST", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_TST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LINEAR_APS].name_from_interface, "LINEAR_APS", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LINEAR_APS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_RING_APS].name_from_interface, "RING_APS", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_RING_APS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_MCC].name_from_interface, "MCC", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_MCC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LMR].name_from_interface, "LMR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LMR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_LMM].name_from_interface, "LMM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_LMM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_1DM].name_from_interface, "1DM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_1DM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_DMR].name_from_interface, "DMR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_DMR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_DMM].name_from_interface, "DMM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_DMM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_EXR].name_from_interface, "EXR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_EXR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_EXM].name_from_interface, "EXM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_EXM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_VSR].name_from_interface, "VSR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_VSR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_VSM].name_from_interface, "VSM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_VSM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_CSF].name_from_interface, "CSF", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_CSF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_SLR].name_from_interface, "SLR", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_SLR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_OP_SLM].name_from_interface, "SLM", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_OP_SLM].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_NONE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_CCM].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LBR].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LBM].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LTR].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LTM].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_AIS].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LCK].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_TST].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LINEAR_APS].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_RING_APS].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_MCC].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LMR].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_LMM].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_1DM].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_DMR].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_DMM].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_EXR].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_EXM].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_VSR].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_VSM].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_CSF].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_SLR].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_OAM_OP_SLM].value_from_mapping = 55;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_oam_ccm_period_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_CCM_PERIOD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAM_CCM_PERIOD" , dnx_data_aod_sizes.AOD.sizes_get(unit, 46)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 8;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_3_33MS].name_from_interface, "3_33MS", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_3_33MS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10MS].name_from_interface, "10MS", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10MS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_100MS].name_from_interface, "100MS", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_100MS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_1S].name_from_interface, "1S", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_1S].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10S].name_from_interface, "10S", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10S].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_1MIN].name_from_interface, "1MIN", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_1MIN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10MIN].name_from_interface, "10MIN", sizeof(enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10MIN].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_INVALID].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_3_33MS].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10MS].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_100MS].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_1S].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10S].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_1MIN].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_OAM_CCM_PERIOD_10MIN].value_from_mapping = 7;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_e_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_E_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "E_TYPE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 47)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_VLAN].name_from_interface, "VLAN", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_VLAN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_MPLS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_MPLS_UA].name_from_interface, "MPLS_UA", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_MPLS_UA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_E_TYPE_OAM].name_from_interface, "OAM", sizeof(enum_info[DBAL_ENUM_FVAL_E_TYPE_OAM].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_NONE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_IPV4].value_from_mapping = 2048;
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_VLAN].value_from_mapping = 33024;
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_IPV6].value_from_mapping = 34525;
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_MPLS].value_from_mapping = 34887;
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_MPLS_UA].value_from_mapping = 34888;
            
            enum_info[DBAL_ENUM_FVAL_E_TYPE_OAM].value_from_mapping = 35074;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_gpe_next_protocol_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GPE_NEXT_PROTOCOL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "GPE_NEXT_PROTOCOL" , dnx_data_aod_sizes.AOD.sizes_get(unit, 48)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_ETH].name_from_interface, "ETH", sizeof(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_ETH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_NETWORK_SERVICE_HEADER].name_from_interface, "NETWORK_SERVICE_HEADER", sizeof(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_NETWORK_SERVICE_HEADER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_MPLS].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_NONE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_IPV4].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_IPV6].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_ETH].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_NETWORK_SERVICE_HEADER].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_GPE_NEXT_PROTOCOL_MPLS].value_from_mapping = 5;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_profile_field_types_bier_next_prtcl_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BIER_NEXT_PRTCL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BIER_NEXT_PRTCL" , dnx_data_aod_sizes.AOD.sizes_get(unit, 49)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 8;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_RESERVED].name_from_interface, "RESERVED", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_RESERVED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_MPLS_DOWNSTREAM].name_from_interface, "MPLS_DOWNSTREAM", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_MPLS_DOWNSTREAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_MPLS_UPSTREAM].name_from_interface, "MPLS_UPSTREAM", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_MPLS_UPSTREAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_ETH].name_from_interface, "ETH", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_ETH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_OAM].name_from_interface, "OAM", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_OAM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_RESERVED2].name_from_interface, "RESERVED2", sizeof(enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_RESERVED2].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_RESERVED].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_MPLS_DOWNSTREAM].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_MPLS_UPSTREAM].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_ETH].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_IPV4].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_OAM].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_IPV6].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_BIER_NEXT_PRTCL_RESERVED2].value_from_mapping = 63;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FHEI_SIZE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 58)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_0B].name_from_interface, "0B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_0B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_3B].name_from_interface, "3B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_3B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_5B].name_from_interface, "5B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_5B].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_8B].name_from_interface, "8B", sizeof(enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_8B].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_0B].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_3B].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_5B].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FHEI_SIZE_8B].value_from_mapping = 3;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_ETPS_FORMATS_MSB_2_BITS" , dnx_data_aod_sizes.AOD.sizes_get(unit, 62)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY].name_from_interface, "DATA_ENTRY", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_NONE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_MSB_2_BITS_DATA_ENTRY].value_from_mapping = 3;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VTT_TT_PROCESS_CODE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 63)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 5;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET].name_from_interface, "ETHERNET", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC].name_from_interface, "MAC_IN_MAC", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4].name_from_interface, "IPV4", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6].name_from_interface, "IPV6", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS].name_from_interface, "MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_ETHERNET].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MAC_IN_MAC].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV4].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_IPV6].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_VTT_TT_PROCESS_CODE_MPLS].value_from_mapping = 4;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "GENERAL_DEFINES" , dnx_data_aod_sizes.AOD.sizes_get(unit, 64)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS].name_from_interface, "FWD2_ACL_NOF_CTXS", sizeof(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX].name_from_interface, "FWD2_ACL_START_CTX", sizeof(enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_NONE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_NOF_CTXS].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_GENERAL_DEFINES_FWD2_ACL_START_CTX].value_from_mapping = 48;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_FORWARDING_CANDIDATE_HEADER" , dnx_data_aod_sizes.AOD.sizes_get(unit, 65)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 12;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MACSEC_ERROR_CODE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 66)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 9;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LIF_VTT5_CSP_1BIT_NAT_DIRECTION" , dnx_data_aod_sizes.AOD.sizes_get(unit, 70)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 1;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_NONE].value_from_mapping = 0;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "JR2_SYSTEM_MODE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 71)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE].name_from_interface, "JR1_MODE", sizeof(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE].name_from_interface, "JR2_MODE", sizeof(enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR1_MODE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_JR2_SYSTEM_MODE_JR2_MODE].value_from_mapping = 1;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_ENCAPSULATION_MAIN_HEADER_TYPE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 73)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 23;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MPLS_QUALIFIER_LABEL_RANGE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 75)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE].name_from_interface, "SPECIAL_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE].name_from_interface, "IML_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE].name_from_interface, "DEFAULT_LABEL_RANGE", sizeof(enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_SPECIAL_LABEL_RANGE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_DEFAULT_LABEL_RANGE].value_from_mapping = 15;
        }
    }
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ERSPAN_V3_ENC1_CANDIDATE_HEADER" , dnx_data_aod_sizes.AOD.sizes_get(unit, 76)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_SWITCH_ID;
    cur_field_types_info->struct_field_info[0].length = 10;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_HW_ID;
    cur_field_types_info->struct_field_info[1].length = 6;
    
    
    
    
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
_dbal_init_field_types_auto_generated_profile_field_types_vtt_lif_service_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VTT_LIF_SERVICE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VTT_LIF_SERVICE_TYPE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 128)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].name_from_interface, "P2MP", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].name_from_interface, "P2P", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP].name_from_interface, "P2P_WITH_SOURCE_LOOKUP", sizeof(enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2MP].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_P2P_WITH_SOURCE_LOOKUP].value_from_mapping = 2;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 169)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE].name_from_interface, "IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE", sizeof(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE].name_from_interface, "IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_DISABLE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_ETH_RIF_BRIDGE_FALLBACK_MODE_IP_COMPATIBLE_MC_BRIDGE_FALLBACK_ENABLE].value_from_mapping = 1;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_FWD_ACE_CTXT_VALUE" , dnx_data_aod_sizes.AOD.sizes_get(unit, 197)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].name_from_interface, "NULL", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].name_from_interface, "L2_INTERNAL_REFLECTOR", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].name_from_interface, "L2_INTERNAL_REFLECTOR_MC", sizeof(enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_ETPP_FWD_ACE_CTXT_VALUE_L2_INTERNAL_REFLECTOR_MC].value_from_mapping = 2;
        }
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ERSPAN_V3_CANDIDATE_HEADER" , dnx_data_aod_sizes.AOD.sizes_get(unit, 331)->value , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_HW_ID;
    cur_field_types_info->struct_field_info[0].length = 6;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_SWITCH_ID;
    cur_field_types_info->struct_field_info[1].length = 10;
    
    
    
    
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
dbal_init_field_types_auto_generated_profile_field_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_iana_ip_protocol_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_aod_bool_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_srh_base_flags_unifiedtype_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_oam_op_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_oam_ccm_period_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_e_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_gpe_next_protocol_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_bier_next_prtcl_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_fhei_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_etps_formats_msb_2_bits_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_tt_process_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_general_defines_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_forwarding_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_macsec_error_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_lif_vtt5_csp_1bit_nat_direction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_jr2_system_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_encapsulation_main_header_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_mpls_qualifier_label_range_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_erspan_v3_enc1_candidate_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_vtt_lif_service_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_lif_fwd1_csp_1bit_eth_rif_bridge_fallback_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_etpp_fwd_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_profile_field_types_erspan_v3_candidate_header_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
