
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_acl_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACL_CONTEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ACL_CONTEXT" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_context_key_gen_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CONTEXT_KEY_GEN_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CONTEXT_KEY_GEN_VAR" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ptc_pmf_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PMF_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PTC_PMF_DATA" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ARRAY32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_out_tm_port_pmf_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OUT_TM_PORT_PMF_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OUT_TM_PORT_PMF_DATA" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ARRAY32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_in_port_pmf_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IN_PORT_PMF_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IN_PORT_PMF_DATA" , dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ARRAY32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_AC_IN_LIF_WIDE_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "AC_IN_LIF_WIDE_DATA" , dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_native_ac_in_lif_wide_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NATIVE_AC_IN_LIF_WIDE_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NATIVE_AC_IN_LIF_WIDE_DATA" , dnx_data_field.general_data_qualifiers.native_ac_in_lif_wide_data_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_extended_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_AC_IN_LIF_WIDE_DATA_EXTENDED];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "AC_IN_LIF_WIDE_DATA_EXTENDED" , dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ARRAY32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pwe_in_lif_wide_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PWE_IN_LIF_WIDE_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PWE_IN_LIF_WIDE_DATA" , dnx_data_field.general_data_qualifiers.pwe_in_lif_wide_data_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_vw_vip_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VW_VIP_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_VIP_VALID" , dnx_data_field.general_data_qualifiers.vw_vip_valid_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_vw_vip_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VW_VIP_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_VIP_ID" , dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_vw_member_reference_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VW_MEMBER_REFERENCE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_MEMBER_REFERENCE" , dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_vw_pcc_hit_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VW_PCC_HIT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_PCC_HIT" , dnx_data_field.general_data_qualifiers.vw_pcc_hit_nof_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_out_pp_port_pmf_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OUT_PP_PORT_PMF_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OUT_PP_PORT_PMF_DATA" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ARRAY32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_rpf_dst_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RPF_DST];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "RPF_DST" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_all_ones_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ALL_ONES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ALL_ONES" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_cmp_key_decoded_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CMP_KEY_DECODED];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CMP_KEY_DECODED" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ieee1588_command_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IEEE1588_COMMAND];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IEEE1588_COMMAND" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ieee1588_header_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IEEE1588_HEADER_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IEEE1588_HEADER_OFFSET" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_fwd_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_PROFILE" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_trap_qualifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TRAP_QUALIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TRAP_QUALIFIER" , 19 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_admt_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ADMT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ADMT_PROFILE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_bier_str_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BIER_STR_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BIER_STR_OFFSET" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_bier_str_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BIER_STR_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BIER_STR_SIZE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_metadata_ecn_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_METADATA_ECN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "METADATA_ECN" , dnx_data_field.signal_sizes.ecn_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_congestion_info_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CONGESTION_INFO];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CONGESTION_INFO" , dnx_data_field.signal_sizes.congestion_info_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_dst_qualifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DST_QUALIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DST_QUALIFIER" , 11 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_dst_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DST_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DST_DATA" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_DST_QUALIFIER;
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_parsing_start_type_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PARSING_START_TYPE_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PARSING_START_TYPE_ACTION" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VALID;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PARSING_START_TYPE_ENCODING;
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_fwd_layer_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_LAYER_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_LAYER_INDEX" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_dp_meter_command_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DP_METER_COMMAND];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DP_METER_COMMAND" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_trace_packet_act_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TRACE_PACKET_ACT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TRACE_PACKET_ACT" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_elephant_payload_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ELEPHANT_PAYLOAD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ELEPHANT_PAYLOAD" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_statistics_object_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATISTICS_OBJECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATISTICS_OBJECT" , 20 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ext_statistics_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXT_STATISTICS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EXT_STATISTICS" , 23 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VALID;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_OPCODE;
    cur_field_types_info->struct_field_info[1].length = 2;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ID;
    cur_field_types_info->struct_field_info[2].length = 20;
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_rpf_out_lif_encoded_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RPF_OUT_LIF_ENCODED];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "RPF_OUT_LIF_ENCODED" , 24 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VALID;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FOUND;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_RPF_OUT_LIF;
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_mem_soft_err_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEM_SOFT_ERR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MEM_SOFT_ERR" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_nasid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NASID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NASID" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 8;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_VT1].name_from_interface, "VT1", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_VT1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_VT2].name_from_interface, "VT2", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_VT2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_VT3].name_from_interface, "VT3", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_VT3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_VT4].name_from_interface, "VT4", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_VT4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_VT5].name_from_interface, "VT5", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_VT5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_FWD1].name_from_interface, "FWD1", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_FWD1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_NASID_FWD2].name_from_interface, "FWD2", sizeof(enum_info[DBAL_ENUM_FVAL_NASID_FWD2].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_NASID_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_NASID_VT1].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_NASID_VT2].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_NASID_VT3].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_NASID_VT4].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_NASID_VT5].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_NASID_FWD1].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_NASID_FWD2].value_from_mapping = 7;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_fwd_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_CONTEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_CONTEXT" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_rpf_ecmp_group_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RPF_ECMP_GROUP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "RPF_ECMP_GROUP" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_rpf_ecmp_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RPF_ECMP_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "RPF_ECMP_MODE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_nwk_qos_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NWK_QOS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NWK_QOS" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_discard_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DISCARD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DISCARD" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_snoop_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SNOOP_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SNOOP_DATA" , 20 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_snoop_strength_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SNOOP_STRENGTH];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SNOOP_STRENGTH" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_fwd_strength_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_STRENGTH];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_STRENGTH" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pmf_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PMF_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PMF_PROFILE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pmf_counter_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PMF_COUNTER_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PMF_COUNTER_PROFILE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pmf_counter_ptr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PMF_COUNTER_PTR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PMF_COUNTER_PTR" , 20 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_snoop_qualifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SNOOP_QUALIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SNOOP_QUALIFIER" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_snoop_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SNOOP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SNOOP_PROFILE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_in_ttl_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IN_TTL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IN_TTL" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_invalid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INVALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INVALID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_invalidate_next_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INVALIDATE_NEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INVALIDATE_NEXT" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pph_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PPH_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PPH_VALUE" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_fwd_domain_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_DOMAIN_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_DOMAIN_PROFILE" , 10 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_state_address_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_ADDRESS_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_ADDRESS_DATA" , 19 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_state_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_DATA" , 9 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_state_table_if_read_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_TABLE_IF_READ];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_TABLE_IF_READ" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF1].name_from_interface, "IPMF1", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF2].name_from_interface, "IPMF2", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF2].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF1].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF2].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_state_table_if_write_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_TABLE_IF_WRITE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_TABLE_IF_WRITE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF1].name_from_interface, "IPMF1", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF2].name_from_interface, "IPMF2", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_ACR].name_from_interface, "ACR", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_ACR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_NONE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF1].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF2].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_ACR].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_NONE].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_state_table_if_rmw_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_TABLE_IF_RMW];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_TABLE_IF_RMW" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF1].name_from_interface, "IPMF1", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF2].name_from_interface, "IPMF2", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_ACR].name_from_interface, "ACR", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_ACR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_FALLBACK].name_from_interface, "FALLBACK", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_FALLBACK].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF1].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF2].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_ACR].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_FALLBACK].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_state_table_address_resolution_rmw_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_TABLE_ADDRESS_RESOLUTION_RMW];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_TABLE_ADDRESS_RESOLUTION_RMW" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_FULL_ENTRY].name_from_interface, "FULL_ENTRY", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_FULL_ENTRY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_HALF_ENTRY].name_from_interface, "HALF_ENTRY", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_HALF_ENTRY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_QUARTER_ENTRY].name_from_interface, "QUARTER_ENTRY", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_QUARTER_ENTRY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_EIGHTH_ENTRY].name_from_interface, "EIGHTH_ENTRY", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_EIGHTH_ENTRY].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_FULL_ENTRY].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_HALF_ENTRY].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_QUARTER_ENTRY].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_EIGHTH_ENTRY].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_state_table_ipmf1_key_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATE_TABLE_IPMF1_KEY_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATE_TABLE_IPMF1_KEY_SELECT" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_J_MSB].name_from_interface, "KEY_J_MSB", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_J_MSB].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_F_LSB].name_from_interface, "KEY_F_LSB", sizeof(enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_F_LSB].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_J_MSB].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_F_LSB].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_statistics_meta_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATISTICS_META_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATISTICS_META_DATA" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_oam_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAM_DATA" , 21 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pph_reserved_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PPH_RESERVED];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PPH_RESERVED" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_statistical_sampling_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATISTICAL_SAMPLING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATISTICAL_SAMPLING" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_egress_parsing_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_PARSING_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EGRESS_PARSING_INDEX" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_statistical_sampling_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATISTICAL_SAMPLING_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATISTICAL_SAMPLING_CODE" , 9 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_statistical_sampling_qualifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STATISTICAL_SAMPLING_QUALIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STATISTICAL_SAMPLING_QUALIFIER" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_src_sys_port_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRC_SYS_PORT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SRC_SYS_PORT" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_header_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_HEADER_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "HEADER_SIZE" , dnx_data_field.signal_sizes.packet_header_size_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_packet_is_bier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PACKET_IS_BIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PACKET_IS_BIER" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_is_applet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IS_APPLET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IS_APPLET" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_packet_is_ieee1588_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PACKET_IS_IEEE1588];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PACKET_IS_IEEE1588" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_l4ops_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS" , 24 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_mirror_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIRROR_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MIRROR_DATA" , 17 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_mirror_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIRROR_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MIRROR_CODE" , 9 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_mirror_qualifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIRROR_QUALIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MIRROR_QUALIFIER" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_stacking_route_history_bitmap_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STACKING_ROUTE_HISTORY_BITMAP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STACKING_ROUTE_HISTORY_BITMAP" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_bytes_to_remove_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BYTES_TO_REMOVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "BYTES_TO_REMOVE" , 9 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_visibility_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VISIBILITY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VISIBILITY" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_visibility_clear_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VISIBILITY_CLEAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VISIBILITY_CLEAR" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_time_stamp_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TIME_STAMP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TIME_STAMP" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_int_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INT_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INT_DATA" , 10 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ieee1588_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IEEE1588_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IEEE1588_DATA" , 13 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_latency_flow_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LATENCY_FLOW];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "LATENCY_FLOW" , 24 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_pmf_oam_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PMF_OAM_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PMF_OAM_ID" , 19 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_stat_obj_lm_read_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_STAT_OBJ_LM_READ_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "STAT_OBJ_LM_READ_INDEX" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_0].name_from_interface, "0", sizeof(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_1].name_from_interface, "1", sizeof(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_2].name_from_interface, "2", sizeof(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_0].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_1].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_2].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID].value_from_mapping = 0;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_1].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_2].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID].value_from_mapping = 3;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_fwd_trap_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_TRAP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_TRAP" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_itpp_delta_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ITPP_DELTA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ITPP_DELTA" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_itpp_delta_and_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ITPP_DELTA_AND_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ITPP_DELTA_AND_VALID" , 9 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_eventor_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EVENTOR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EVENTOR" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_user_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_USER_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "USER_HEADER" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_user_header_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_USER_HEADER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "USER_HEADER_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_nwk_header_append_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NWK_HEADER_APPEND_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "NWK_HEADER_APPEND_SIZE" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_tc_map_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TC_MAP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TC_MAP_PROFILE" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_system_header_profile_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SYSTEM_HEADER_PROFILE_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SYSTEM_HEADER_PROFILE_INDEX" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_system_header_size_adjust_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SYSTEM_HEADER_SIZE_ADJUST];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SYSTEM_HEADER_SIZE_ADJUST" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_tm_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TM_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TM_PROFILE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_vrf_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VRF_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VRF_VALUE" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ipmf1_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_ACTION" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 130;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].name_from_interface, "EMPTY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT].name_from_interface, "ACL_CONTEXT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE].name_from_interface, "ADMT_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET].name_from_interface, "BIER_STR_OFFSET", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE].name_from_interface, "BIER_STR_SIZE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO].name_from_interface, "CONGESTION_INFO", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA].name_from_interface, "DST_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP].name_from_interface, "DP", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND].name_from_interface, "DP_METER_COMMAND", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EEI].name_from_interface, "EEI", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EEI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE].name_from_interface, "EGRESS_LEARN_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD].name_from_interface, "ELEPHANT_PAYLOAD", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT].name_from_interface, "TRACE_PACKET_ACT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING].name_from_interface, "END_OF_PACKET_EDITING", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0].name_from_interface, "EXT_STATISTICS_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1].name_from_interface, "EXT_STATISTICS_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2].name_from_interface, "EXT_STATISTICS_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3].name_from_interface, "EXT_STATISTICS_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD].name_from_interface, "FWD", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN].name_from_interface, "FWD_DOMAIN", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO].name_from_interface, "FWD_LAYER_ADDITIONAL_INFO", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT].name_from_interface, "FWD_CONTEXT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX].name_from_interface, "FWD_LAYER_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA].name_from_interface, "IEEE1588_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE].name_from_interface, "INGRESS_LEARN_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0].name_from_interface, "GLOB_IN_LIF_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1].name_from_interface, "GLOB_IN_LIF_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0].name_from_interface, "IN_LIF_PROFILE_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1].name_from_interface, "IN_LIF_PROFILE_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT].name_from_interface, "IN_PP_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL].name_from_interface, "IN_TTL", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA].name_from_interface, "ITPP_DELTA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW].name_from_interface, "LATENCY_FLOW", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0].name_from_interface, "LEARN_INFO_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1].name_from_interface, "LEARN_INFO_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2].name_from_interface, "LEARN_INFO_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3].name_from_interface, "LEARN_INFO_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4].name_from_interface, "LEARN_INFO_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT].name_from_interface, "LEARN_OR_TRANSPLANT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA].name_from_interface, "MIRROR_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS].name_from_interface, "NWK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA].name_from_interface, "OAM_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID].name_from_interface, "OAM_ID", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0].name_from_interface, "GLOB_OUT_LIF_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1].name_from_interface, "GLOB_OUT_LIF_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2].name_from_interface, "GLOB_OUT_LIF_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3].name_from_interface, "GLOB_OUT_LIF_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH].name_from_interface, "OUT_LIF_PUSH", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC].name_from_interface, "PACKET_IS_COMPATIBLE_MC", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER].name_from_interface, "PACKET_IS_BIER", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX].name_from_interface, "EGRESS_PARSING_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0].name_from_interface, "PEM_GENERAL_DATA0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1].name_from_interface, "PEM_GENERAL_DATA1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2].name_from_interface, "PEM_GENERAL_DATA2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3].name_from_interface, "PEM_GENERAL_DATA3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED].name_from_interface, "PPH_RESERVED", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE].name_from_interface, "PPH_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST].name_from_interface, "RPF_DST", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID].name_from_interface, "RPF_DST_VALID", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF].name_from_interface, "RPF_OUT_LIF", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PTC].name_from_interface, "PTC", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PTC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING].name_from_interface, "STATISTICAL_SAMPLING", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA].name_from_interface, "SNOOP_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT].name_from_interface, "SRC_SYS_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP].name_from_interface, "STACKING_ROUTE_HISTORY_BITMAP", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0].name_from_interface, "SLB_PAYLOAD0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1].name_from_interface, "SLB_PAYLOAD1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0].name_from_interface, "SLB_KEY0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1].name_from_interface, "SLB_KEY1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2].name_from_interface, "SLB_KEY2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND].name_from_interface, "SLB_FOUND", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA].name_from_interface, "STATE_ADDRESS_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA].name_from_interface, "STATE_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA].name_from_interface, "STATISTICS_META_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0].name_from_interface, "STATISTICS_ID_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1].name_from_interface, "STATISTICS_ID_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2].name_from_interface, "STATISTICS_ID_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3].name_from_interface, "STATISTICS_ID_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4].name_from_interface, "STATISTICS_ID_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5].name_from_interface, "STATISTICS_ID_5", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6].name_from_interface, "STATISTICS_ID_6", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7].name_from_interface, "STATISTICS_ID_7", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8].name_from_interface, "STATISTICS_ID_8", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9].name_from_interface, "STATISTICS_ID_9", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0].name_from_interface, "STATISTICS_ATR_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1].name_from_interface, "STATISTICS_ATR_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2].name_from_interface, "STATISTICS_ATR_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3].name_from_interface, "STATISTICS_ATR_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4].name_from_interface, "STATISTICS_ATR_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5].name_from_interface, "STATISTICS_ATR_5", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6].name_from_interface, "STATISTICS_ATR_6", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7].name_from_interface, "STATISTICS_ATR_7", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8].name_from_interface, "STATISTICS_ATR_8", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9].name_from_interface, "STATISTICS_ATR_9", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10].name_from_interface, "STATISTICS_OBJECT_10", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR].name_from_interface, "ST_VSQ_PTR", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX].name_from_interface, "SYSTEM_HEADER_PROFILE_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TC].name_from_interface, "TC", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE].name_from_interface, "TM_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1].name_from_interface, "USER_HEADER_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE].name_from_interface, "USER_HEADER_1_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2].name_from_interface, "USER_HEADER_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE].name_from_interface, "USER_HEADER_2_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3].name_from_interface, "USER_HEADER_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE].name_from_interface, "USER_HEADER_3_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4].name_from_interface, "USER_HEADER_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE].name_from_interface, "USER_HEADER_4_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX].name_from_interface, "VLAN_EDIT_CMD_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1].name_from_interface, "VLAN_EDIT_PCP_DEI_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2].name_from_interface, "VLAN_EDIT_PCP_DEI_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1].name_from_interface, "VLAN_EDIT_VID_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2].name_from_interface, "VLAN_EDIT_VID_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP].name_from_interface, "INGRESS_TIME_STAMP", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA].name_from_interface, "INT_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE].name_from_interface, "INGRESS_TIME_STAMP_OVERRIDE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR].name_from_interface, "EVENTOR", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX].name_from_interface, "STAT_OBJ_LM_READ_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR].name_from_interface, "VISIBILITY_CLEAR", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DUAL_QUEUE].name_from_interface, "DUAL_QUEUE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DUAL_QUEUE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY].name_from_interface, "VISIBILITY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER].name_from_interface, "PRT_QUALIFIER", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TUNNEL_PRIORITY].name_from_interface, "TUNNEL_PRIORITY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TUNNEL_PRIORITY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0].name_from_interface, "LEARN_INFO_KEY_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1].name_from_interface, "LEARN_INFO_KEY_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].name_from_interface, "LEARN_INFO_KEY_2_APP_DB_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0].name_from_interface, "LEARN_INFO_PAYLOAD_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1].name_from_interface, "LEARN_INFO_PAYLOAD_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX].name_from_interface, "LEARN_INFO_FORMAT_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT].name_from_interface, "INVALIDATE_NEXT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID].name_from_interface));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EEI].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PTC].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3].value_from_mapping = 77;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4].value_from_mapping = 78;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5].value_from_mapping = 79;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6].value_from_mapping = 80;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7].value_from_mapping = 81;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8].value_from_mapping = 82;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9].value_from_mapping = 83;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0].value_from_mapping = 84;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1].value_from_mapping = 85;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2].value_from_mapping = 86;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3].value_from_mapping = 87;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4].value_from_mapping = 88;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5].value_from_mapping = 89;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6].value_from_mapping = 90;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7].value_from_mapping = 91;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8].value_from_mapping = 92;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9].value_from_mapping = 93;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10].value_from_mapping = 94;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR].value_from_mapping = 95;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX].value_from_mapping = 96;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TC].value_from_mapping = 97;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE].value_from_mapping = 98;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1].value_from_mapping = 99;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE].value_from_mapping = 100;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2].value_from_mapping = 101;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE].value_from_mapping = 102;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3].value_from_mapping = 103;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE].value_from_mapping = 104;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4].value_from_mapping = 105;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE].value_from_mapping = 106;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX].value_from_mapping = 107;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1].value_from_mapping = 108;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2].value_from_mapping = 109;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1].value_from_mapping = 110;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2].value_from_mapping = 111;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP].value_from_mapping = 112;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA].value_from_mapping = 113;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE].value_from_mapping = 114;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR].value_from_mapping = 115;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX].value_from_mapping = 116;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DUAL_QUEUE].value_from_mapping = 117;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY].value_from_mapping = 118;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER].value_from_mapping = 119;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TUNNEL_PRIORITY].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT].value_from_mapping = 120;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EEI].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PTC].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3].value_from_mapping = 77;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4].value_from_mapping = 78;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5].value_from_mapping = 79;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6].value_from_mapping = 80;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7].value_from_mapping = 81;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8].value_from_mapping = 82;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9].value_from_mapping = 83;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0].value_from_mapping = 84;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1].value_from_mapping = 85;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2].value_from_mapping = 86;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3].value_from_mapping = 87;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4].value_from_mapping = 88;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5].value_from_mapping = 89;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6].value_from_mapping = 90;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7].value_from_mapping = 91;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8].value_from_mapping = 92;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9].value_from_mapping = 93;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10].value_from_mapping = 94;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR].value_from_mapping = 95;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX].value_from_mapping = 96;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TC].value_from_mapping = 97;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE].value_from_mapping = 98;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1].value_from_mapping = 99;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE].value_from_mapping = 100;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2].value_from_mapping = 101;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE].value_from_mapping = 102;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3].value_from_mapping = 103;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE].value_from_mapping = 104;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4].value_from_mapping = 105;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE].value_from_mapping = 106;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX].value_from_mapping = 107;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1].value_from_mapping = 108;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2].value_from_mapping = 109;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1].value_from_mapping = 110;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2].value_from_mapping = 111;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP].value_from_mapping = 112;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA].value_from_mapping = 113;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE].value_from_mapping = 114;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR].value_from_mapping = 115;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX].value_from_mapping = 116;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DUAL_QUEUE].value_from_mapping = 117;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY].value_from_mapping = 118;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER].value_from_mapping = 119;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TUNNEL_PRIORITY].value_from_mapping = 120;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX].value_from_mapping = 121;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT].value_from_mapping = 122;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EEI].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PTC].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3].value_from_mapping = 77;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4].value_from_mapping = 78;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5].value_from_mapping = 79;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6].value_from_mapping = 80;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7].value_from_mapping = 81;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8].value_from_mapping = 82;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9].value_from_mapping = 83;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0].value_from_mapping = 84;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1].value_from_mapping = 85;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2].value_from_mapping = 86;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3].value_from_mapping = 87;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4].value_from_mapping = 88;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5].value_from_mapping = 89;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6].value_from_mapping = 90;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7].value_from_mapping = 91;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8].value_from_mapping = 92;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9].value_from_mapping = 93;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10].value_from_mapping = 94;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR].value_from_mapping = 95;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX].value_from_mapping = 96;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TC].value_from_mapping = 97;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE].value_from_mapping = 98;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1].value_from_mapping = 99;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE].value_from_mapping = 100;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2].value_from_mapping = 101;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE].value_from_mapping = 102;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3].value_from_mapping = 103;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE].value_from_mapping = 104;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4].value_from_mapping = 105;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE].value_from_mapping = 106;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX].value_from_mapping = 107;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1].value_from_mapping = 108;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2].value_from_mapping = 109;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1].value_from_mapping = 110;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2].value_from_mapping = 111;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP].value_from_mapping = 112;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA].value_from_mapping = 113;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE].value_from_mapping = 114;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR].value_from_mapping = 115;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX].value_from_mapping = 116;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR].value_from_mapping = 117;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_DUAL_QUEUE].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY].value_from_mapping = 118;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_TUNNEL_PRIORITY].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT].value_from_mapping = 119;
            
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_ipmf3_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF3_ACTION" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 117;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].name_from_interface, "EMPTY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT].name_from_interface, "ACL_CONTEXT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE].name_from_interface, "ADMT_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET].name_from_interface, "BIER_STR_OFFSET", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE].name_from_interface, "BIER_STR_SIZE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE].name_from_interface, "BYTES_TO_REMOVE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO].name_from_interface, "CONGESTION_INFO", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP].name_from_interface, "DP", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND].name_from_interface, "DP_METER_COMMAND", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EEI].name_from_interface, "EEI", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EEI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE].name_from_interface, "EGRESS_LEARN_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING].name_from_interface, "END_OF_PACKET_EDITING", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0].name_from_interface, "EXT_STATISTICS_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1].name_from_interface, "EXT_STATISTICS_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2].name_from_interface, "EXT_STATISTICS_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3].name_from_interface, "EXT_STATISTICS_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD].name_from_interface, "FWD", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA].name_from_interface, "DST_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN].name_from_interface, "FWD_DOMAIN", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO].name_from_interface, "FWD_LAYER_ADDITIONAL_INFO", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX].name_from_interface, "FWD_LAYER_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0].name_from_interface, "GENERAL_DATA_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1].name_from_interface, "GENERAL_DATA_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2].name_from_interface, "GENERAL_DATA_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3].name_from_interface, "GENERAL_DATA_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA].name_from_interface, "IEEE1588_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE].name_from_interface, "INGRESS_LEARN_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP].name_from_interface, "INGRESS_TIME_STAMP", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE].name_from_interface, "INGRESS_TIME_STAMP_OVERRIDE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0].name_from_interface, "GLOB_IN_LIF_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0].name_from_interface, "IN_LIF_PROFILE_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1].name_from_interface, "GLOB_IN_LIF_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1].name_from_interface, "IN_LIF_PROFILE_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT].name_from_interface, "IN_PP_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA].name_from_interface, "INT_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL].name_from_interface, "IN_TTL", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA].name_from_interface, "ITPP_DELTA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY].name_from_interface, "LAG_LB_KEY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW].name_from_interface, "LATENCY_FLOW", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0].name_from_interface, "LEARN_INFO_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1].name_from_interface, "LEARN_INFO_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2].name_from_interface, "LEARN_INFO_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3].name_from_interface, "LEARN_INFO_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4].name_from_interface, "LEARN_INFO_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT].name_from_interface, "LEARN_OR_TRANSPLANT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA].name_from_interface, "MIRROR_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY].name_from_interface, "NWK_LB_KEY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS].name_from_interface, "NWK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE].name_from_interface, "NWK_HEADER_APPEND_SIZE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA].name_from_interface, "OAM_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID].name_from_interface, "OAM_ID", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0].name_from_interface, "GLOB_OUT_LIF_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1].name_from_interface, "GLOB_OUT_LIF_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2].name_from_interface, "GLOB_OUT_LIF_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3].name_from_interface, "GLOB_OUT_LIF_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET].name_from_interface, "PACKET_IS_APPLET", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER].name_from_interface, "PACKET_IS_BIER", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX].name_from_interface, "EGRESS_PARSING_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED].name_from_interface, "PPH_RESERVED", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE].name_from_interface, "PPH_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF].name_from_interface, "PUSH_OUT_LIF", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA].name_from_interface, "SNOOP_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT].name_from_interface, "SRC_SYS_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP].name_from_interface, "STACKING_ROUTE_HISTORY_BITMAP", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL].name_from_interface, "STATISTICAL", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA].name_from_interface, "STATISTICS_META_DATA", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0].name_from_interface, "STATISTICS_ID_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1].name_from_interface, "STATISTICS_ID_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10].name_from_interface, "STATISTICS_OBJECT_10", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2].name_from_interface, "STATISTICS_ID_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3].name_from_interface, "STATISTICS_ID_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4].name_from_interface, "STATISTICS_ID_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5].name_from_interface, "STATISTICS_ID_5", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6].name_from_interface, "STATISTICS_ID_6", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7].name_from_interface, "STATISTICS_ID_7", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8].name_from_interface, "STATISTICS_ID_8", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9].name_from_interface, "STATISTICS_ID_9", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0].name_from_interface, "STATISTICS_ATR_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1].name_from_interface, "STATISTICS_ATR_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2].name_from_interface, "STATISTICS_ATR_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3].name_from_interface, "STATISTICS_ATR_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4].name_from_interface, "STATISTICS_ATR_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5].name_from_interface, "STATISTICS_ATR_5", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6].name_from_interface, "STATISTICS_ATR_6", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7].name_from_interface, "STATISTICS_ATR_7", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8].name_from_interface, "STATISTICS_ATR_8", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9].name_from_interface, "STATISTICS_ATR_9", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR].name_from_interface, "ST_VSQ_PTR", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX].name_from_interface, "SYSTEM_HEADER_PROFILE_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST].name_from_interface, "SYSTEM_HEADER_SIZE_ADJUST", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TC].name_from_interface, "TC", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE].name_from_interface, "TM_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1].name_from_interface, "USER_HEADER_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2].name_from_interface, "USER_HEADER_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3].name_from_interface, "USER_HEADER_3", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4].name_from_interface, "USER_HEADER_4", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE].name_from_interface, "USER_HEADERS_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY].name_from_interface, "VISIBILITY", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX].name_from_interface, "VLAN_EDIT_CMD_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1].name_from_interface, "VLAN_EDIT_PCP_DEI_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2].name_from_interface, "VLAN_EDIT_PCP_DEI_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1].name_from_interface, "VLAN_EDIT_VID_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2].name_from_interface, "VLAN_EDIT_VID_2", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET].name_from_interface, "PARSING_START_OFFSET", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE].name_from_interface, "PARSING_START_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX].name_from_interface, "STAT_OBJ_LM_READ_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE].name_from_interface, "TM_ACTION_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DUAL_QUEUE].name_from_interface, "DUAL_QUEUE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DUAL_QUEUE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_TRUNCATE_SIZE].name_from_interface, "NWK_HEADER_TRUNCATE_SIZE", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_TRUNCATE_SIZE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0].name_from_interface, "LEARN_INFO_KEY_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1].name_from_interface, "LEARN_INFO_KEY_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].name_from_interface, "LEARN_INFO_KEY_2_APP_DB_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0].name_from_interface, "LEARN_INFO_PAYLOAD_0", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1].name_from_interface, "LEARN_INFO_PAYLOAD_1", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX].name_from_interface, "LEARN_INFO_FORMAT_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT].name_from_interface, "INVALIDATE_NEXT", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID].name_from_interface));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EEI].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0].value_from_mapping = 77;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1].value_from_mapping = 78;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2].value_from_mapping = 79;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3].value_from_mapping = 80;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4].value_from_mapping = 81;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5].value_from_mapping = 82;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6].value_from_mapping = 83;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7].value_from_mapping = 84;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8].value_from_mapping = 85;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9].value_from_mapping = 86;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR].value_from_mapping = 87;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX].value_from_mapping = 88;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST].value_from_mapping = 89;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TC].value_from_mapping = 90;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE].value_from_mapping = 91;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1].value_from_mapping = 92;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2].value_from_mapping = 93;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3].value_from_mapping = 94;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4].value_from_mapping = 95;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE].value_from_mapping = 96;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY].value_from_mapping = 97;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX].value_from_mapping = 98;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1].value_from_mapping = 99;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2].value_from_mapping = 100;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1].value_from_mapping = 101;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2].value_from_mapping = 102;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET].value_from_mapping = 103;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE].value_from_mapping = 104;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX].value_from_mapping = 105;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE].value_from_mapping = 106;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DUAL_QUEUE].value_from_mapping = 107;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_TRUNCATE_SIZE].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT].value_from_mapping = 108;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EEI].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0].value_from_mapping = 77;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1].value_from_mapping = 78;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2].value_from_mapping = 79;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3].value_from_mapping = 80;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4].value_from_mapping = 81;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5].value_from_mapping = 82;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6].value_from_mapping = 83;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7].value_from_mapping = 84;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8].value_from_mapping = 85;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9].value_from_mapping = 86;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR].value_from_mapping = 87;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX].value_from_mapping = 88;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST].value_from_mapping = 89;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TC].value_from_mapping = 90;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE].value_from_mapping = 91;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1].value_from_mapping = 92;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2].value_from_mapping = 93;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3].value_from_mapping = 94;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4].value_from_mapping = 95;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE].value_from_mapping = 96;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY].value_from_mapping = 97;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX].value_from_mapping = 98;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1].value_from_mapping = 99;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2].value_from_mapping = 100;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1].value_from_mapping = 101;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2].value_from_mapping = 102;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET].value_from_mapping = 103;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE].value_from_mapping = 104;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX].value_from_mapping = 105;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE].value_from_mapping = 106;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DUAL_QUEUE].value_from_mapping = 107;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_TRUNCATE_SIZE].value_from_mapping = 108;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX].value_from_mapping = 109;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT].value_from_mapping = 113;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EEI].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0].value_from_mapping = 77;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1].value_from_mapping = 78;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2].value_from_mapping = 79;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3].value_from_mapping = 80;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4].value_from_mapping = 81;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5].value_from_mapping = 82;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6].value_from_mapping = 83;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7].value_from_mapping = 84;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8].value_from_mapping = 85;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9].value_from_mapping = 86;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR].value_from_mapping = 87;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX].value_from_mapping = 88;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST].value_from_mapping = 89;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TC].value_from_mapping = 90;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE].value_from_mapping = 91;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1].value_from_mapping = 92;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2].value_from_mapping = 93;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3].value_from_mapping = 94;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4].value_from_mapping = 95;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE].value_from_mapping = 96;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY].value_from_mapping = 97;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX].value_from_mapping = 98;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1].value_from_mapping = 99;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2].value_from_mapping = 100;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1].value_from_mapping = 101;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2].value_from_mapping = 102;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET].value_from_mapping = 103;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE].value_from_mapping = 104;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX].value_from_mapping = 105;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_DUAL_QUEUE].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_TRUNCATE_SIZE].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT].value_from_mapping = 106;
            
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_epmf_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_ACTION" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 28;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY].name_from_interface, "EMPTY", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD].name_from_interface, "DISCARD", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE].name_from_interface, "TC_MAP_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC].name_from_interface, "TC", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DP].name_from_interface, "DP", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR].name_from_interface, "CUD_OUTLIF_OR_MCDB_PTR", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP].name_from_interface, "PP_DSP", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE].name_from_interface, "COS_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE].name_from_interface, "MIRROR_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE].name_from_interface, "SNOOP_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH].name_from_interface, "SNOOP_STRENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX].name_from_interface, "FWD_ACTION_PROFILE_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH].name_from_interface, "FWD_STRENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH].name_from_interface, "PMF_FWD_STRENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH].name_from_interface, "PMF_SNOOP_STRENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE].name_from_interface, "PMF_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE].name_from_interface, "PMF_SNOOP_OVERRIDE_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE].name_from_interface, "PMF_FWD_OVERRIDE_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE].name_from_interface, "PMF_COUNTER_0_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR].name_from_interface, "PMF_COUNTER_0_PTR", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE].name_from_interface, "PMF_COUNTER_1_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR].name_from_interface, "PMF_COUNTER_1_PTR", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP].name_from_interface, "EBTR_EOP", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP].name_from_interface, "EBTA_EOP", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP].name_from_interface, "EBTA_SOP", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP].name_from_interface, "EBTR_SOP", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT].name_from_interface, "INVALIDATE_NEXT", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALID].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DP].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_TC].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_DP].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_INVALID].value_from_mapping = 127;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_ace_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACE_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ACE_ACTION" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 36;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_EMPTY].name_from_interface, "EMPTY", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_EMPTY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP].name_from_interface, "PP_DSP", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC].name_from_interface, "FTMH_TC", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP].name_from_interface, "FTMH_DP", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE].name_from_interface, "FTMH_TM_ACTION_TYPE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI].name_from_interface, "FTMH_CNI", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE].name_from_interface, "FTMH_ECN_ENABLE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY].name_from_interface, "FTMH_VISIBILITY", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN].name_from_interface, "PPH_FORWARD_DOMAIN", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS].name_from_interface, "PPH_NWK_QOS", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1].name_from_interface, "PPH_VALUE1", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2].name_from_interface, "PPH_VALUE2", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00].name_from_interface, "FHEI_EXT31TO00", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32].name_from_interface, "FHEI_EXT63TO32", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL].name_from_interface, "PPH_TTL", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF].name_from_interface, "PPH_IN_LIF", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE].name_from_interface, "PPH_IN_LIF_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID].name_from_interface, "CUD_OUT_LIF0_CUD_OUT_LIF0VALID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID].name_from_interface, "CUD_OUT_LIF1_CUD_OUT_LIF1VALID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID].name_from_interface, "CUD_OUT_LIF2_CUD_OUT_LIF2VALID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID].name_from_interface, "CUD_OUT_LIF3_CUD_OUT_LIF3VALID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO].name_from_interface, "PPH_FORWARDING_LAYER_ADDITIONAL_INFO", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX].name_from_interface, "PPH_FWD_LAYER_INDEX", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH].name_from_interface, "PPH_FORWANDING_STRENGTH", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING].name_from_interface, "PPH_END_OF_PACKET_EDITING", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00].name_from_interface, "PPH_APPLICATION_SPECIFIC_EXT_31TO00", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32].name_from_interface, "PPH_APPLICATION_SPECIFIC_EXT_47TO32", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE].name_from_interface, "SNIFF_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT].name_from_interface, "FTMH_SOURCE_SYSTEM_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE].name_from_interface, "ACE_CONTEXT_VALUE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE].name_from_interface, "ACE_STAMP_VALUE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID].name_from_interface, "ACE_STAT_METER_OBJ_CMD_ID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID].name_from_interface, "ACE_STAT_OBJ_CMD_ID_VALID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_FWD_ACTION_PROFILE_VALID_PROFILE].name_from_interface, "ACE_FWD_ACTION_PROFILE_VALID_PROFILE", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_FWD_ACTION_PROFILE_VALID_PROFILE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT].name_from_interface, "INVALIDATE_NEXT", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALID].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_FWD_ACTION_PROFILE_VALID_PROFILE].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALID].value_from_mapping = 35;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_EMPTY].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_ACE_FWD_ACTION_PROFILE_VALID_PROFILE].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_ACE_ACTION_INVALID].value_from_mapping = 34;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_pmf_lookup_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PMF_LOOKUP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PMF_LOOKUP_TYPE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 7;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM].name_from_interface, "EXEM", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT].name_from_interface, "DIRECT", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0].name_from_interface, "TCAM_0", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1].name_from_interface, "TCAM_1", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2].name_from_interface, "TCAM_2", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3].name_from_interface, "TCAM_3", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE].name_from_interface, "STATE_TABLE", sizeof(enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE].value_from_mapping = 6;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_tcam_bank_block_owner_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TCAM_BANK_BLOCK_OWNER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TCAM_BANK_BLOCK_OWNER" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_PMF].name_from_interface, "PMF", sizeof(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_PMF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_FLP].name_from_interface, "FLP", sizeof(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_FLP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_VTT].name_from_interface, "VTT", sizeof(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_VTT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_ERPP].name_from_interface, "ERPP", sizeof(enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_ERPP].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_PMF].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_FLP].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_VTT].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_ERPP].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_pmf_a_fes_key_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_A_FES_KEY_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_A_FES_KEY_SELECT" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 41;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0].name_from_interface, "TCAM_0_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].name_from_interface, "TCAM_1_31_0_TCAM_0_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0].name_from_interface, "TCAM_1_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32].name_from_interface, "TCAM_2_31_0_TCAM_1_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0].name_from_interface, "TCAM_2_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32].name_from_interface, "TCAM_3_31_0_TCAM_2_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0].name_from_interface, "TCAM_3_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32].name_from_interface, "TCAM_4_31_0_TCAM_3_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0].name_from_interface, "TCAM_4_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32].name_from_interface, "TCAM_5_31_0_TCAM_4_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0].name_from_interface, "TCAM_5_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32].name_from_interface, "TCAM_6_31_0_TCAM_5_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0].name_from_interface, "TCAM_6_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32].name_from_interface, "TCAM_7_31_0_TCAM_6_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0].name_from_interface, "TCAM_7_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32].name_from_interface, "LEXEM_27_0_4_B0_TCAM_7_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0].name_from_interface, "LEXEM_59_0_4_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28].name_from_interface, "SEXEM_27_0_4_B0_LEXEM_59_28", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0].name_from_interface, "SEXEM_59_0_4_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28].name_from_interface, "KEY_I_31_0_SEXEM_59_28", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0].name_from_interface, "KEY_I_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32].name_from_interface, "KEY_I_95_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64].name_from_interface, "KEY_I_127_64", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96].name_from_interface, "KEY_I_159_96", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128].name_from_interface, "KEY_J_31_0_KEY_I_159_128", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0].name_from_interface, "KEY_J_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32].name_from_interface, "KEY_J_95_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64].name_from_interface, "KEY_J_127_64", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96].name_from_interface, "KEY_J_159_96", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128].name_from_interface, "DIRECT_0_23_0_8_B0_KEY_J_159_128", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0].name_from_interface, "DIRECT_0_55_0_8_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24].name_from_interface, "DIRECT_0_87_24", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56].name_from_interface, "DIRECT_0_119_56", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88].name_from_interface, "DIRECT_1_23_0_8_B0_DIRECT_0_119_88", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0].name_from_interface, "DIRECT_1_55_0_8_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24].name_from_interface, "DIRECT_1_87_24", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56].name_from_interface, "DIRECT_1_119_56", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88].name_from_interface, "DIRECT_1_119_88", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT].name_from_interface, "INVALID_FES_KEY_SELECT", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT].name_from_interface));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT].value_from_mapping = 63;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT].value_from_mapping = 63;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT].is_invalid_value_from_mapping = TRUE;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_pmf_b_fes_key_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_B_FES_KEY_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_B_FES_KEY_SELECT" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 19;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0].name_from_interface, "TCAM_0_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].name_from_interface, "TCAM_1_31_0_TCAM_0_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_63_0].name_from_interface, "TCAM_1_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32].name_from_interface, "SEXEM_27_0_4_B0_TCAM_1_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0].name_from_interface, "SEXEM_59_0_4_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28].name_from_interface, "KEY_A_31_0_SEXEM_59_28", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_63_0].name_from_interface, "KEY_A_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_95_32].name_from_interface, "KEY_A_95_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_95_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_127_64].name_from_interface, "KEY_A_127_64", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_127_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_159_96].name_from_interface, "KEY_A_159_96", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_159_96].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128].name_from_interface, "KEY_B_31_0_KEY_A_159_128", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_63_0].name_from_interface, "KEY_B_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_95_32].name_from_interface, "KEY_B_95_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_95_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_127_64].name_from_interface, "KEY_B_127_64", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_127_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_159_96].name_from_interface, "KEY_B_159_96", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_159_96].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128].name_from_interface, "DIRECT_31_0_KEY_B_159_128", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_55_0_8_B0].name_from_interface, "DIRECT_55_0_8_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_55_0_8_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_87_24].name_from_interface, "DIRECT_87_24", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_87_24].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_119_56].name_from_interface, "DIRECT_119_56", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_119_56].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_63_0].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_63_0].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_95_32].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_127_64].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_159_96].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_63_0].value_from_mapping = 11;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_95_32].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_127_64].value_from_mapping = 13;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_159_96].value_from_mapping = 14;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128].value_from_mapping = 15;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_55_0_8_B0].value_from_mapping = 16;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_87_24].value_from_mapping = 17;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_119_56].value_from_mapping = 18;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_e_pmf_fes_key_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_E_PMF_FES_KEY_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_E_PMF_FES_KEY_SELECT" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 18;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0].name_from_interface, "LEXEM_4_B0_59_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32].name_from_interface, "TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_59_0_4_B0].name_from_interface, "LEXEM_59_0_4_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_59_0_4_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_LEXEM_59_28].name_from_interface, "TCAM_0_PAYLOAD_31_0_LEXEM_59_28", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_LEXEM_59_28].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0].name_from_interface, "TCAM_0_PAYLOAD_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32].name_from_interface, "TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0].name_from_interface, "TCAM_1_PAYLOAD_63_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_KEY_D_31_0_TCAM_1_PAYLOAD_63_32].name_from_interface, "KEY_D_31_0_TCAM_1_PAYLOAD_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_KEY_D_31_0_TCAM_1_PAYLOAD_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_6].name_from_interface, "NOT_IN_USE_6", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_7].name_from_interface, "NOT_IN_USE_7", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_7].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_8].name_from_interface, "NOT_IN_USE_8", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_9].name_from_interface, "NOT_IN_USE_9", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_9].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_10].name_from_interface, "NOT_IN_USE_10", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_10].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_11].name_from_interface, "NOT_IN_USE_11", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_11].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_12].name_from_interface, "NOT_IN_USE_12", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_12].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_13].name_from_interface, "NOT_IN_USE_13", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_13].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_14].name_from_interface, "NOT_IN_USE_14", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_14].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_15].name_from_interface, "NOT_IN_USE_15", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_15].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_59_0_4_B0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_LEXEM_59_28].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_KEY_D_31_0_TCAM_1_PAYLOAD_63_32].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_6].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_7].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_8].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_9].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_10].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_11].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_12].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_13].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_14].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_15].value_from_mapping = 15;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_59_0_4_B0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_LEXEM_59_28].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_KEY_D_31_0_TCAM_1_PAYLOAD_63_32].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_6].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_7].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_8].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_9].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_10].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_11].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_12].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_13].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_14].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_15].value_from_mapping = 15;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_pmf_a_fem_key_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_A_FEM_KEY_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_A_FEM_KEY_SELECT" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 80;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0].name_from_interface, "TCAM_0_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16].name_from_interface, "TCAM_0_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32].name_from_interface, "TCAM_0_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48].name_from_interface, "TCAM_1_15_0_TCAM_0_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0].name_from_interface, "TCAM_1_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16].name_from_interface, "TCAM_1_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32].name_from_interface, "TCAM_1_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48].name_from_interface, "TCAM_2_15_0_TCAM_1_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0].name_from_interface, "TCAM_2_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16].name_from_interface, "TCAM_2_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32].name_from_interface, "TCAM_2_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48].name_from_interface, "TCAM_3_15_0_TCAM_2_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0].name_from_interface, "TCAM_3_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16].name_from_interface, "TCAM_3_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32].name_from_interface, "TCAM_3_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48].name_from_interface, "TCAM_4_15_0_TCAM_3_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0].name_from_interface, "TCAM_4_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16].name_from_interface, "TCAM_4_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32].name_from_interface, "TCAM_4_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48].name_from_interface, "TCAM_5_15_0_TCAM_4_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0].name_from_interface, "TCAM_5_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16].name_from_interface, "TCAM_5_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32].name_from_interface, "TCAM_5_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48].name_from_interface, "TCAM_6_15_0_TCAM_5_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0].name_from_interface, "TCAM_6_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16].name_from_interface, "TCAM_6_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32].name_from_interface, "TCAM_6_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48].name_from_interface, "TCAM_7_15_0_TCAM_6_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0].name_from_interface, "TCAM_7_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16].name_from_interface, "TCAM_7_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32].name_from_interface, "TCAM_7_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48].name_from_interface, "LEXEM_11_0_4_B0_TCAM_7_63_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0].name_from_interface, "LEXEM_27_0_4_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12].name_from_interface, "LEXEM_43_12", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28].name_from_interface, "LEXEM_59_28", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44].name_from_interface, "SEXEM_11_0_4_B0_LEXEM_59_44", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0].name_from_interface, "SEXEM_27_0_4_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12].name_from_interface, "SEXEM_43_12", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28].name_from_interface, "SEXEM_59_28", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44].name_from_interface, "KEY_I_16_0_SEXEM_59_44", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0].name_from_interface, "KEY_I_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16].name_from_interface, "KEY_I_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32].name_from_interface, "KEY_I_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48].name_from_interface, "KEY_I_79_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64].name_from_interface, "KEY_I_95_64", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80].name_from_interface, "KEY_I_111_80", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96].name_from_interface, "KEY_I_127_96", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112].name_from_interface, "KEY_I_143_112", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128].name_from_interface, "KEY_I_159_128", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144].name_from_interface, "KEY_J_15_0_KEY_I_159_144", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0].name_from_interface, "KEY_J_31_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16].name_from_interface, "KEY_J_47_16", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32].name_from_interface, "KEY_J_63_32", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48].name_from_interface, "KEY_J_79_48", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64].name_from_interface, "KEY_J_95_64", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80].name_from_interface, "KEY_J_111_80", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96].name_from_interface, "KEY_J_127_96", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112].name_from_interface, "KEY_J_143_112", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128].name_from_interface, "KEY_J_159_128", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144].name_from_interface, "DIRECT_0_7_0_8_B0_KEY_J_159_144", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0].name_from_interface, "DIRECT_0_23_0_8_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8].name_from_interface, "DIRECT_0_39_8", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24].name_from_interface, "DIRECT_0_55_24", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40].name_from_interface, "DIRECT_0_71_40", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56].name_from_interface, "DIRECT_0_87_56", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72].name_from_interface, "DIRECT_0_103_72", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88].name_from_interface, "DIRECT_0_119_88", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104].name_from_interface, "DIRECT_1_7_0_8_B0_DIRECT_0_119_104", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0].name_from_interface, "DIRECT_1_23_0_8_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8].name_from_interface, "DIRECT_1_39_8", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24].name_from_interface, "DIRECT_1_55_24", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40].name_from_interface, "DIRECT_1_71_40", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56].name_from_interface, "DIRECT_1_87_56", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72].name_from_interface, "DIRECT_1_103_72", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88].name_from_interface, "DIRECT_1_119_88", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104].name_from_interface, "STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104].name_from_interface, "STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT].name_from_interface, "INVALID_FEM_KEY_SELECT", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT].name_from_interface));
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT].value_from_mapping = 127;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104].value_from_mapping = 75;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0].value_from_mapping = 76;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT].value_from_mapping = 127;
        }
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48].value_from_mapping = 15;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0].value_from_mapping = 16;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16].value_from_mapping = 17;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32].value_from_mapping = 18;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48].value_from_mapping = 19;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0].value_from_mapping = 20;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16].value_from_mapping = 21;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32].value_from_mapping = 22;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48].value_from_mapping = 23;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0].value_from_mapping = 24;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16].value_from_mapping = 25;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32].value_from_mapping = 26;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48].value_from_mapping = 27;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0].value_from_mapping = 28;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16].value_from_mapping = 29;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32].value_from_mapping = 30;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48].value_from_mapping = 31;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0].value_from_mapping = 32;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12].value_from_mapping = 33;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28].value_from_mapping = 34;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44].value_from_mapping = 35;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0].value_from_mapping = 36;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12].value_from_mapping = 37;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28].value_from_mapping = 38;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44].value_from_mapping = 39;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0].value_from_mapping = 40;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16].value_from_mapping = 41;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32].value_from_mapping = 42;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48].value_from_mapping = 43;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64].value_from_mapping = 44;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80].value_from_mapping = 45;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96].value_from_mapping = 46;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112].value_from_mapping = 47;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128].value_from_mapping = 48;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144].value_from_mapping = 49;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0].value_from_mapping = 50;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16].value_from_mapping = 51;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32].value_from_mapping = 52;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48].value_from_mapping = 53;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64].value_from_mapping = 54;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80].value_from_mapping = 55;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96].value_from_mapping = 56;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112].value_from_mapping = 57;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128].value_from_mapping = 58;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144].value_from_mapping = 59;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0].value_from_mapping = 60;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8].value_from_mapping = 61;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24].value_from_mapping = 62;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40].value_from_mapping = 63;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56].value_from_mapping = 64;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72].value_from_mapping = 65;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88].value_from_mapping = 66;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104].value_from_mapping = 67;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0].value_from_mapping = 68;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8].value_from_mapping = 69;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24].value_from_mapping = 70;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40].value_from_mapping = 71;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56].value_from_mapping = 72;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72].value_from_mapping = 73;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88].value_from_mapping = 74;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0].is_invalid_value_from_mapping = TRUE;
            
            enum_info[DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT].value_from_mapping = 127;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_pmf_fes_data_2msb_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_FES_DATA_2MSB];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_FES_DATA_2MSB" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_invalid_bits_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_INVALID_BITS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_INVALID_BITS" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_shift_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_SHIFT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_SHIFT" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_TYPE" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_polarity_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_POLARITY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_POLARITY" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_chosen_mask_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_CHOSEN_MASK];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_CHOSEN_MASK" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_pmf_presel_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_PRESEL_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_PRESEL_ID" , 7 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_pmf_ctx_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_CTX_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_CTX_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_ace_result_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_ACE_RESULT_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_ACE_RESULT_TYPE" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_app_db_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_APP_DB_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "APP_DB_ID" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_pmf_ctx_id_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_CTX_ID_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_CTX_ID_VALID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_pmf_nas_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_NAS_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_NAS_ID" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_pmf_nas_id_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_NAS_ID_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_NAS_ID_VALID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_ID" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_mask_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_MASK_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_MASK_ID" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_action_mask_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_ACTION_MASK];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_ACTION_MASK" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fes_pgm_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FES_PGM_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FES_PGM_ID" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_action_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_ACTION_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_ACTION_VALID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE].name_from_interface, "FEM_ACTIONS_IS_NOT_ACTIVE", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE].name_from_interface, "FEM_ACTIONS_IS_ACTIVE", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_fem_pgm_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_PGM_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_PGM_ID" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_replace_lsb_flag_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_REPLACE_LSB_FLAG];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_REPLACE_LSB_FLAG" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE].name_from_interface, "REPLACE_LSB_IS_NOT_ACTIVE", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE].name_from_interface, "REPLACE_LSB_IS_ACTIVE", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_fem_replace_lsb_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_REPLACE_LSB_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_REPLACE_LSB_SELECT" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 8;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0].name_from_interface, "TCAM_0", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_1].name_from_interface, "TCAM_1", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_2].name_from_interface, "TCAM_2", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_3].name_from_interface, "TCAM_3", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_3].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_4].name_from_interface, "TCAM_4", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_5].name_from_interface, "TCAM_5", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_5].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_6].name_from_interface, "TCAM_6", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_6].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_7].name_from_interface, "TCAM_7", sizeof(enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_7].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_1].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_2].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_3].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_4].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_5].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_6].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_7].value_from_mapping = 7;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_fem_condition_ms_bit_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_CONDITION_MS_BIT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_CONDITION_MS_BIT" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_selected_bit_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_SELECTED_BIT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_SELECTED_BIT" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_map_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_MAP_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_MAP_INDEX" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_ID" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_condition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_CONDITION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_CONDITION" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_field_fem_map_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_FEM_MAP_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_FEM_MAP_DATA" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_context_hash_function_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CONTEXT_HASH_FUNCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "CONTEXT_HASH_FUNCTION" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 14;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED].name_from_interface, "FIRST_RESERVED", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED].name_from_interface, "SECOND_RESERVED", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED].name_from_interface, "THIRD_RESERVED", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC].name_from_interface, "CRC16_BISYNC", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1].name_from_interface, "CRC16_XOR1", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2].name_from_interface, "CRC16_XOR2", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4].name_from_interface, "CRC16_XOR4", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8].name_from_interface, "CRC16_XOR8", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16].name_from_interface, "XOR16", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT].name_from_interface, "CRC16_CCITT", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW].name_from_interface, "CRC32_ALOW", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH].name_from_interface, "CRC32_AHIGH", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW].name_from_interface, "CRC32_BLOW", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH].name_from_interface, "CRC32_BHIGH", sizeof(enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8].value_from_mapping = 7;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16].value_from_mapping = 8;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT].value_from_mapping = 9;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW].value_from_mapping = 10;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH].value_from_mapping = 11;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW].value_from_mapping = 12;
        
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH].value_from_mapping = 13;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_field_pmf_range_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FIELD_PMF_RANGE_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FIELD_PMF_RANGE_ID" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_ace_context_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACE_CONTEXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ACE_CONTEXT_VALUE" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 7;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ETPP_FWD_CODE_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ETPP_PRP_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[1].length = 2;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ETPP_TERM_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_ETPP_FWD_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[3].length = 2;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_ETPP_ENCAP_1_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_ETPP_ENCAP_2_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_ETPP_TRAP_ACE_CTXT_VALUE;
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_l4_ops_range_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_RANGE_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_RANGE_ID" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_l4_ops_encode_idx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_ENCODE_IDX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_ENCODE_IDX" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_l4_ops_ffc_idx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_FFC_IDX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_FFC_IDX" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_BOOL , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_EXTND_RANGES_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_EXTND_RANGES_TYPE" , 3 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 8;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_DST_PORT].name_from_interface, "L4_DST_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_DST_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_SRC_PORT].name_from_interface, "L4_SRC_PORT", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_SRC_PORT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_IN_TLL].name_from_interface, "IN_TLL", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_IN_TLL].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_LOW].name_from_interface, "FFC_1_LOW", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_LOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_HIGH].name_from_interface, "FFC_1_HIGH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_HIGH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_LOW].name_from_interface, "FFC_2_LOW", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_LOW].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_HIGH].name_from_interface, "FFC_2_HIGH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_HIGH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_PACKET_HEADER_SIZE].name_from_interface, "PACKET_HEADER_SIZE", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_PACKET_HEADER_SIZE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_DST_PORT].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_SRC_PORT].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_IN_TLL].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_LOW].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_HIGH].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_LOW].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_HIGH].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_PACKET_HEADER_SIZE].value_from_mapping = 7;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_op_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_EXTND_RANGES_OP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_EXTND_RANGES_OP_TYPE" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_AND].name_from_interface, "AND", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_AND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_OR].name_from_interface, "OR", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_OR].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_AND].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_OR].value_from_mapping = 2;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_op_width_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_EXTND_RANGES_OP_WIDTH];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_EXTND_RANGES_OP_WIDTH" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 2;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_32].name_from_interface, "32", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_32].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_64].name_from_interface, "64", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_64].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_32].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_64].value_from_mapping = 1;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_op_pair_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_EXTND_RANGES_OP_PAIR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_EXTND_RANGES_OP_PAIR" , 2 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_FIRST].name_from_interface, "FIRST", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_FIRST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_SECOND].name_from_interface, "SECOND", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_SECOND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_BOTH].name_from_interface, "BOTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_BOTH].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_NONE].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_FIRST].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_SECOND].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_BOTH].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_encode_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4_OPS_EXTND_RANGES_ENCODE_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4_OPS_EXTND_RANGES_ENCODE_SELECT" , 4 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 16;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_NONE].name_from_interface, "NONE", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_NONE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST].name_from_interface, "FIRST", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND].name_from_interface, "SECOND", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD].name_from_interface, "THIRD", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FORTH].name_from_interface, "FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND].name_from_interface, "FIRST_SECOND", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD].name_from_interface, "FIRST_THIRD", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_FORTH].name_from_interface, "FIRST_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD].name_from_interface, "SECOND_THIRD", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_FORTH].name_from_interface, "SECOND_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD_FORTH].name_from_interface, "THIRD_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD].name_from_interface, "FIRST_SECOND_THIRD", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_FORTH].name_from_interface, "FIRST_SECOND_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD_FORTH].name_from_interface, "FIRST_THIRD_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD_FORTH].name_from_interface, "SECOND_THIRD_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD_FORTH].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD_FORTH].name_from_interface, "FIRST_SECOND_THIRD_FORTH", sizeof(enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD_FORTH].name_from_interface));
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
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_NONE].value_from_mapping = 0;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST].value_from_mapping = 1;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND].value_from_mapping = 2;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD].value_from_mapping = 4;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FORTH].value_from_mapping = 8;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND].value_from_mapping = 3;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD].value_from_mapping = 5;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_FORTH].value_from_mapping = 9;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD].value_from_mapping = 6;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_FORTH].value_from_mapping = 10;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD_FORTH].value_from_mapping = 12;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD].value_from_mapping = 7;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_FORTH].value_from_mapping = 11;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD_FORTH].value_from_mapping = 13;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD_FORTH].value_from_mapping = 14;
            
            enum_info[DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD_FORTH].value_from_mapping = 15;
        }
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_pmf_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_acl_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_context_key_gen_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ptc_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_out_tm_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_in_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_native_ac_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_extended_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pwe_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_vw_vip_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_vw_vip_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_vw_member_reference_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_vw_pcc_hit_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_out_pp_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_rpf_dst_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_all_ones_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_cmp_key_decoded_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ieee1588_command_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ieee1588_header_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_fwd_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_trap_qualifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_admt_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_bier_str_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_bier_str_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_metadata_ecn_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_congestion_info_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_dst_qualifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_dst_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_parsing_start_type_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_fwd_layer_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_dp_meter_command_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_trace_packet_act_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_elephant_payload_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_statistics_object_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ext_statistics_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_rpf_out_lif_encoded_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_mem_soft_err_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_nasid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_fwd_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_rpf_ecmp_group_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_rpf_ecmp_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_nwk_qos_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_discard_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_snoop_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_snoop_strength_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_fwd_strength_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pmf_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pmf_counter_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pmf_counter_ptr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_snoop_qualifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_snoop_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_in_ttl_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_invalid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_invalidate_next_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pph_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_fwd_domain_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_address_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_table_if_read_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_table_if_write_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_table_if_rmw_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_table_address_resolution_rmw_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_state_table_ipmf1_key_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_statistics_meta_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_oam_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pph_reserved_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_statistical_sampling_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_egress_parsing_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_statistical_sampling_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_statistical_sampling_qualifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_src_sys_port_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_header_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_packet_is_bier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_is_applet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_packet_is_ieee1588_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4ops_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_mirror_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_mirror_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_mirror_qualifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_stacking_route_history_bitmap_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_bytes_to_remove_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_visibility_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_visibility_clear_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_time_stamp_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_int_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ieee1588_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_latency_flow_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pmf_oam_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_stat_obj_lm_read_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_fwd_trap_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_itpp_delta_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_itpp_delta_and_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_eventor_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_user_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_user_header_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_nwk_header_append_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_tc_map_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_system_header_profile_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_system_header_size_adjust_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_tm_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_vrf_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ipmf1_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ipmf3_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_epmf_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ace_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pmf_lookup_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_tcam_bank_block_owner_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_a_fes_key_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_b_fes_key_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_e_pmf_fes_key_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_a_fem_key_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_fes_data_2msb_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_invalid_bits_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_shift_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_polarity_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_chosen_mask_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_presel_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_ctx_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_ace_result_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_app_db_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_ctx_id_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_nas_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_nas_id_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_mask_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_action_mask_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fes_pgm_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_action_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_pgm_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_replace_lsb_flag_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_replace_lsb_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_condition_ms_bit_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_selected_bit_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_map_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_condition_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_fem_map_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_context_hash_function_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_field_pmf_range_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ace_context_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_range_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_encode_idx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_ffc_idx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_op_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_op_width_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_op_pair_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_l4_ops_extnd_ranges_encode_select_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
