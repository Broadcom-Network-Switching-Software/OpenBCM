
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

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
_dbal_init_field_types_pmf_field_types_definition_tm_port_pmf_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TM_PORT_PMF_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TM_PORT_PMF_DATA" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ARRAY32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "AC_IN_LIF_WIDE_DATA" , dnx_data_field.qual.ac_lif_wide_size_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_VIP_VALID" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_VIP_ID" , 12 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_MEMBER_REFERENCE" , 14 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VW_PCC_HIT" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 8 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "VT1", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "VT2", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "VT3", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "VT4", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "VT5", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FWD1", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "FWD2", sizeof(cur_field_type_param->enums[7].name_from_interface));
    cur_field_type_param->nof_enum_vals = 8;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "IPMF1", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "IPMF2", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "IPMF1", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "IPMF2", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ACR", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "IPMF1", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "IPMF2", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ACR", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FALLBACK", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "FULL_ENTRY", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "HALF_ENTRY", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "QUARTER_ENTRY", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "EIGHTH_ENTRY", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "KEY_J_MSB", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "KEY_F_LSB", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "0", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "1", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "2", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "INVALID", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    if (DBAL_IS_JR2_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 1;
        cur_field_type_param->enums[1].value_from_mapping = 2;
        cur_field_type_param->enums[2].value_from_mapping = 3;
        cur_field_type_param->enums[3].value_from_mapping = 0;
    }
    else
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 130 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "EMPTY", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ACL_CONTEXT", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ADMT_PROFILE", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BIER_STR_OFFSET", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BIER_STR_SIZE", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "CONGESTION_INFO", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "DST_DATA", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "DP", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "DP_METER_COMMAND", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "EEI", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "EGRESS_LEARN_ENABLE", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "ELEPHANT_PAYLOAD", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "TRACE_PACKET_ACT", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "END_OF_PACKET_EDITING", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "EXT_STATISTICS_0", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "EXT_STATISTICS_1", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "EXT_STATISTICS_2", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "EXT_STATISTICS_3", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "FWD", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "FWD_DOMAIN", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "FWD_LAYER_ADDITIONAL_INFO", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "FWD_CONTEXT", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "FWD_LAYER_INDEX", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IEEE1588_DATA", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "INGRESS_LEARN_ENABLE", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "GLOB_IN_LIF_0", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "GLOB_IN_LIF_1", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "IN_LIF_PROFILE_0", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "IN_LIF_PROFILE_1", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "IN_PP_PORT", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "IN_TTL", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "ITPP_DELTA", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "LATENCY_FLOW", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "LEARN_INFO_0", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "LEARN_INFO_1", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "LEARN_INFO_2", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "LEARN_INFO_3", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "LEARN_INFO_4", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "LEARN_OR_TRANSPLANT", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "MIRROR_DATA", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "NWK_QOS", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "OAM_DATA", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "OAM_ID", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "GLOB_OUT_LIF_0", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "GLOB_OUT_LIF_1", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "GLOB_OUT_LIF_2", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "GLOB_OUT_LIF_3", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "OUT_LIF_PUSH", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "PACKET_IS_COMPATIBLE_MC", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "PACKET_IS_BIER", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "EGRESS_PARSING_INDEX", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "PEM_GENERAL_DATA0", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "PEM_GENERAL_DATA1", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "PEM_GENERAL_DATA2", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "PEM_GENERAL_DATA3", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "PPH_RESERVED", sizeof(cur_field_type_param->enums[55].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[56].name_from_interface, "PPH_TYPE", sizeof(cur_field_type_param->enums[56].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[57].name_from_interface, "RPF_DST", sizeof(cur_field_type_param->enums[57].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[58].name_from_interface, "RPF_DST_VALID", sizeof(cur_field_type_param->enums[58].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[59].name_from_interface, "RPF_OUT_LIF", sizeof(cur_field_type_param->enums[59].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[60].name_from_interface, "PTC", sizeof(cur_field_type_param->enums[60].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[61].name_from_interface, "STATISTICAL_SAMPLING", sizeof(cur_field_type_param->enums[61].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[62].name_from_interface, "SNOOP_DATA", sizeof(cur_field_type_param->enums[62].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[63].name_from_interface, "SRC_SYS_PORT", sizeof(cur_field_type_param->enums[63].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[64].name_from_interface, "STACKING_ROUTE_HISTORY_BITMAP", sizeof(cur_field_type_param->enums[64].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[65].name_from_interface, "SLB_PAYLOAD0", sizeof(cur_field_type_param->enums[65].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[66].name_from_interface, "SLB_PAYLOAD1", sizeof(cur_field_type_param->enums[66].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[67].name_from_interface, "SLB_KEY0", sizeof(cur_field_type_param->enums[67].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[68].name_from_interface, "SLB_KEY1", sizeof(cur_field_type_param->enums[68].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[69].name_from_interface, "SLB_KEY2", sizeof(cur_field_type_param->enums[69].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[70].name_from_interface, "SLB_FOUND", sizeof(cur_field_type_param->enums[70].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[71].name_from_interface, "STATE_ADDRESS_DATA", sizeof(cur_field_type_param->enums[71].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[72].name_from_interface, "STATE_DATA", sizeof(cur_field_type_param->enums[72].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[73].name_from_interface, "STATISTICS_META_DATA", sizeof(cur_field_type_param->enums[73].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[74].name_from_interface, "STATISTICS_ID_0", sizeof(cur_field_type_param->enums[74].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[75].name_from_interface, "STATISTICS_ID_1", sizeof(cur_field_type_param->enums[75].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[76].name_from_interface, "STATISTICS_ID_2", sizeof(cur_field_type_param->enums[76].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[77].name_from_interface, "STATISTICS_ID_3", sizeof(cur_field_type_param->enums[77].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[78].name_from_interface, "STATISTICS_ID_4", sizeof(cur_field_type_param->enums[78].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[79].name_from_interface, "STATISTICS_ID_5", sizeof(cur_field_type_param->enums[79].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[80].name_from_interface, "STATISTICS_ID_6", sizeof(cur_field_type_param->enums[80].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[81].name_from_interface, "STATISTICS_ID_7", sizeof(cur_field_type_param->enums[81].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[82].name_from_interface, "STATISTICS_ID_8", sizeof(cur_field_type_param->enums[82].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[83].name_from_interface, "STATISTICS_ID_9", sizeof(cur_field_type_param->enums[83].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[84].name_from_interface, "STATISTICS_ATR_0", sizeof(cur_field_type_param->enums[84].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[85].name_from_interface, "STATISTICS_ATR_1", sizeof(cur_field_type_param->enums[85].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[86].name_from_interface, "STATISTICS_ATR_2", sizeof(cur_field_type_param->enums[86].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[87].name_from_interface, "STATISTICS_ATR_3", sizeof(cur_field_type_param->enums[87].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[88].name_from_interface, "STATISTICS_ATR_4", sizeof(cur_field_type_param->enums[88].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[89].name_from_interface, "STATISTICS_ATR_5", sizeof(cur_field_type_param->enums[89].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[90].name_from_interface, "STATISTICS_ATR_6", sizeof(cur_field_type_param->enums[90].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[91].name_from_interface, "STATISTICS_ATR_7", sizeof(cur_field_type_param->enums[91].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[92].name_from_interface, "STATISTICS_ATR_8", sizeof(cur_field_type_param->enums[92].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[93].name_from_interface, "STATISTICS_ATR_9", sizeof(cur_field_type_param->enums[93].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[94].name_from_interface, "STATISTICS_OBJECT_10", sizeof(cur_field_type_param->enums[94].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[95].name_from_interface, "ST_VSQ_PTR", sizeof(cur_field_type_param->enums[95].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[96].name_from_interface, "SYSTEM_HEADER_PROFILE_INDEX", sizeof(cur_field_type_param->enums[96].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[97].name_from_interface, "TC", sizeof(cur_field_type_param->enums[97].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[98].name_from_interface, "TM_PROFILE", sizeof(cur_field_type_param->enums[98].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[99].name_from_interface, "USER_HEADER_1", sizeof(cur_field_type_param->enums[99].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[100].name_from_interface, "USER_HEADER_1_TYPE", sizeof(cur_field_type_param->enums[100].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[101].name_from_interface, "USER_HEADER_2", sizeof(cur_field_type_param->enums[101].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[102].name_from_interface, "USER_HEADER_2_TYPE", sizeof(cur_field_type_param->enums[102].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[103].name_from_interface, "USER_HEADER_3", sizeof(cur_field_type_param->enums[103].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[104].name_from_interface, "USER_HEADER_3_TYPE", sizeof(cur_field_type_param->enums[104].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[105].name_from_interface, "USER_HEADER_4", sizeof(cur_field_type_param->enums[105].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[106].name_from_interface, "USER_HEADER_4_TYPE", sizeof(cur_field_type_param->enums[106].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[107].name_from_interface, "VLAN_EDIT_CMD_INDEX", sizeof(cur_field_type_param->enums[107].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[108].name_from_interface, "VLAN_EDIT_PCP_DEI_1", sizeof(cur_field_type_param->enums[108].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[109].name_from_interface, "VLAN_EDIT_PCP_DEI_2", sizeof(cur_field_type_param->enums[109].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[110].name_from_interface, "VLAN_EDIT_VID_1", sizeof(cur_field_type_param->enums[110].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[111].name_from_interface, "VLAN_EDIT_VID_2", sizeof(cur_field_type_param->enums[111].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[112].name_from_interface, "INGRESS_TIME_STAMP", sizeof(cur_field_type_param->enums[112].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[113].name_from_interface, "INT_DATA", sizeof(cur_field_type_param->enums[113].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[114].name_from_interface, "INGRESS_TIME_STAMP_OVERRIDE", sizeof(cur_field_type_param->enums[114].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[115].name_from_interface, "EVENTOR", sizeof(cur_field_type_param->enums[115].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[116].name_from_interface, "STAT_OBJ_LM_READ_INDEX", sizeof(cur_field_type_param->enums[116].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[117].name_from_interface, "VISIBILITY_CLEAR", sizeof(cur_field_type_param->enums[117].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[118].name_from_interface, "DUAL_QUEUE", sizeof(cur_field_type_param->enums[118].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[119].name_from_interface, "VISIBILITY", sizeof(cur_field_type_param->enums[119].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[120].name_from_interface, "PRT_QUALIFIER", sizeof(cur_field_type_param->enums[120].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[121].name_from_interface, "TUNNEL_PRIORITY", sizeof(cur_field_type_param->enums[121].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[122].name_from_interface, "LEARN_INFO_KEY_0", sizeof(cur_field_type_param->enums[122].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[123].name_from_interface, "LEARN_INFO_KEY_1", sizeof(cur_field_type_param->enums[123].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[124].name_from_interface, "LEARN_INFO_KEY_2_APP_DB_INDEX", sizeof(cur_field_type_param->enums[124].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[125].name_from_interface, "LEARN_INFO_PAYLOAD_0", sizeof(cur_field_type_param->enums[125].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[126].name_from_interface, "LEARN_INFO_PAYLOAD_1", sizeof(cur_field_type_param->enums[126].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[127].name_from_interface, "LEARN_INFO_FORMAT_INDEX", sizeof(cur_field_type_param->enums[127].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[128].name_from_interface, "INVALIDATE_NEXT", sizeof(cur_field_type_param->enums[128].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[129].name_from_interface, "INVALID", sizeof(cur_field_type_param->enums[129].name_from_interface));
    cur_field_type_param->nof_enum_vals = 130;
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 26;
        cur_field_type_param->enums[28].value_from_mapping = 27;
        cur_field_type_param->enums[29].value_from_mapping = 28;
        cur_field_type_param->enums[30].value_from_mapping = 29;
        cur_field_type_param->enums[31].value_from_mapping = 30;
        cur_field_type_param->enums[32].value_from_mapping = 31;
        cur_field_type_param->enums[33].value_from_mapping = 32;
        cur_field_type_param->enums[34].value_from_mapping = 33;
        cur_field_type_param->enums[35].value_from_mapping = 34;
        cur_field_type_param->enums[36].value_from_mapping = 35;
        cur_field_type_param->enums[37].value_from_mapping = 36;
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[122].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[123].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[124].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[125].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[126].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[39].value_from_mapping = 38;
        cur_field_type_param->enums[40].value_from_mapping = 39;
        cur_field_type_param->enums[41].value_from_mapping = 40;
        cur_field_type_param->enums[42].value_from_mapping = 41;
        cur_field_type_param->enums[43].value_from_mapping = 42;
        cur_field_type_param->enums[44].value_from_mapping = 43;
        cur_field_type_param->enums[45].value_from_mapping = 44;
        cur_field_type_param->enums[46].value_from_mapping = 45;
        cur_field_type_param->enums[47].value_from_mapping = 47;
        cur_field_type_param->enums[48].value_from_mapping = 48;
        cur_field_type_param->enums[49].value_from_mapping = 49;
        cur_field_type_param->enums[50].value_from_mapping = 50;
        cur_field_type_param->enums[51].value_from_mapping = 51;
        cur_field_type_param->enums[52].value_from_mapping = 52;
        cur_field_type_param->enums[53].value_from_mapping = 53;
        cur_field_type_param->enums[54].value_from_mapping = 54;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[77].value_from_mapping = 77;
        cur_field_type_param->enums[78].value_from_mapping = 78;
        cur_field_type_param->enums[79].value_from_mapping = 79;
        cur_field_type_param->enums[80].value_from_mapping = 80;
        cur_field_type_param->enums[81].value_from_mapping = 81;
        cur_field_type_param->enums[82].value_from_mapping = 82;
        cur_field_type_param->enums[83].value_from_mapping = 83;
        cur_field_type_param->enums[84].value_from_mapping = 84;
        cur_field_type_param->enums[85].value_from_mapping = 85;
        cur_field_type_param->enums[86].value_from_mapping = 86;
        cur_field_type_param->enums[87].value_from_mapping = 87;
        cur_field_type_param->enums[88].value_from_mapping = 88;
        cur_field_type_param->enums[89].value_from_mapping = 89;
        cur_field_type_param->enums[90].value_from_mapping = 90;
        cur_field_type_param->enums[91].value_from_mapping = 91;
        cur_field_type_param->enums[92].value_from_mapping = 92;
        cur_field_type_param->enums[93].value_from_mapping = 93;
        cur_field_type_param->enums[94].value_from_mapping = 94;
        cur_field_type_param->enums[95].value_from_mapping = 95;
        cur_field_type_param->enums[96].value_from_mapping = 96;
        cur_field_type_param->enums[97].value_from_mapping = 97;
        cur_field_type_param->enums[98].value_from_mapping = 98;
        cur_field_type_param->enums[99].value_from_mapping = 99;
        cur_field_type_param->enums[100].value_from_mapping = 100;
        cur_field_type_param->enums[101].value_from_mapping = 101;
        cur_field_type_param->enums[102].value_from_mapping = 102;
        cur_field_type_param->enums[103].value_from_mapping = 103;
        cur_field_type_param->enums[104].value_from_mapping = 104;
        cur_field_type_param->enums[105].value_from_mapping = 105;
        cur_field_type_param->enums[106].value_from_mapping = 106;
        cur_field_type_param->enums[107].value_from_mapping = 107;
        cur_field_type_param->enums[108].value_from_mapping = 108;
        cur_field_type_param->enums[109].value_from_mapping = 109;
        cur_field_type_param->enums[110].value_from_mapping = 110;
        cur_field_type_param->enums[111].value_from_mapping = 111;
        cur_field_type_param->enums[112].value_from_mapping = 112;
        cur_field_type_param->enums[113].value_from_mapping = 113;
        cur_field_type_param->enums[114].value_from_mapping = 114;
        cur_field_type_param->enums[115].value_from_mapping = 115;
        cur_field_type_param->enums[116].value_from_mapping = 116;
        cur_field_type_param->enums[118].value_from_mapping = 117;
        cur_field_type_param->enums[119].value_from_mapping = 118;
        cur_field_type_param->enums[120].value_from_mapping = 119;
        cur_field_type_param->enums[121].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[127].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[128].value_from_mapping = 120;
        cur_field_type_param->enums[129].value_from_mapping = 127;
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
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 26;
        cur_field_type_param->enums[28].value_from_mapping = 27;
        cur_field_type_param->enums[29].value_from_mapping = 28;
        cur_field_type_param->enums[30].value_from_mapping = 29;
        cur_field_type_param->enums[31].value_from_mapping = 30;
        cur_field_type_param->enums[32].value_from_mapping = 31;
        cur_field_type_param->enums[122].value_from_mapping = 32;
        cur_field_type_param->enums[123].value_from_mapping = 33;
        cur_field_type_param->enums[124].value_from_mapping = 34;
        cur_field_type_param->enums[125].value_from_mapping = 35;
        cur_field_type_param->enums[126].value_from_mapping = 36;
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[33].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[34].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[35].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[36].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[37].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[39].value_from_mapping = 38;
        cur_field_type_param->enums[40].value_from_mapping = 39;
        cur_field_type_param->enums[41].value_from_mapping = 40;
        cur_field_type_param->enums[42].value_from_mapping = 41;
        cur_field_type_param->enums[43].value_from_mapping = 42;
        cur_field_type_param->enums[44].value_from_mapping = 43;
        cur_field_type_param->enums[45].value_from_mapping = 44;
        cur_field_type_param->enums[46].value_from_mapping = 45;
        cur_field_type_param->enums[47].value_from_mapping = 47;
        cur_field_type_param->enums[48].value_from_mapping = 48;
        cur_field_type_param->enums[49].value_from_mapping = 49;
        cur_field_type_param->enums[50].value_from_mapping = 50;
        cur_field_type_param->enums[51].value_from_mapping = 51;
        cur_field_type_param->enums[52].value_from_mapping = 52;
        cur_field_type_param->enums[53].value_from_mapping = 53;
        cur_field_type_param->enums[54].value_from_mapping = 54;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[77].value_from_mapping = 77;
        cur_field_type_param->enums[78].value_from_mapping = 78;
        cur_field_type_param->enums[79].value_from_mapping = 79;
        cur_field_type_param->enums[80].value_from_mapping = 80;
        cur_field_type_param->enums[81].value_from_mapping = 81;
        cur_field_type_param->enums[82].value_from_mapping = 82;
        cur_field_type_param->enums[83].value_from_mapping = 83;
        cur_field_type_param->enums[84].value_from_mapping = 84;
        cur_field_type_param->enums[85].value_from_mapping = 85;
        cur_field_type_param->enums[86].value_from_mapping = 86;
        cur_field_type_param->enums[87].value_from_mapping = 87;
        cur_field_type_param->enums[88].value_from_mapping = 88;
        cur_field_type_param->enums[89].value_from_mapping = 89;
        cur_field_type_param->enums[90].value_from_mapping = 90;
        cur_field_type_param->enums[91].value_from_mapping = 91;
        cur_field_type_param->enums[92].value_from_mapping = 92;
        cur_field_type_param->enums[93].value_from_mapping = 93;
        cur_field_type_param->enums[94].value_from_mapping = 94;
        cur_field_type_param->enums[95].value_from_mapping = 95;
        cur_field_type_param->enums[96].value_from_mapping = 96;
        cur_field_type_param->enums[97].value_from_mapping = 97;
        cur_field_type_param->enums[98].value_from_mapping = 98;
        cur_field_type_param->enums[99].value_from_mapping = 99;
        cur_field_type_param->enums[100].value_from_mapping = 100;
        cur_field_type_param->enums[101].value_from_mapping = 101;
        cur_field_type_param->enums[102].value_from_mapping = 102;
        cur_field_type_param->enums[103].value_from_mapping = 103;
        cur_field_type_param->enums[104].value_from_mapping = 104;
        cur_field_type_param->enums[105].value_from_mapping = 105;
        cur_field_type_param->enums[106].value_from_mapping = 106;
        cur_field_type_param->enums[107].value_from_mapping = 107;
        cur_field_type_param->enums[108].value_from_mapping = 108;
        cur_field_type_param->enums[109].value_from_mapping = 109;
        cur_field_type_param->enums[110].value_from_mapping = 110;
        cur_field_type_param->enums[111].value_from_mapping = 111;
        cur_field_type_param->enums[112].value_from_mapping = 112;
        cur_field_type_param->enums[113].value_from_mapping = 113;
        cur_field_type_param->enums[114].value_from_mapping = 114;
        cur_field_type_param->enums[115].value_from_mapping = 115;
        cur_field_type_param->enums[116].value_from_mapping = 116;
        cur_field_type_param->enums[118].value_from_mapping = 117;
        cur_field_type_param->enums[119].value_from_mapping = 118;
        cur_field_type_param->enums[120].value_from_mapping = 119;
        cur_field_type_param->enums[121].value_from_mapping = 120;
        cur_field_type_param->enums[127].value_from_mapping = 121;
        cur_field_type_param->enums[128].value_from_mapping = 122;
        cur_field_type_param->enums[129].value_from_mapping = 127;
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 26;
        cur_field_type_param->enums[28].value_from_mapping = 27;
        cur_field_type_param->enums[29].value_from_mapping = 28;
        cur_field_type_param->enums[30].value_from_mapping = 29;
        cur_field_type_param->enums[31].value_from_mapping = 30;
        cur_field_type_param->enums[32].value_from_mapping = 31;
        cur_field_type_param->enums[33].value_from_mapping = 32;
        cur_field_type_param->enums[34].value_from_mapping = 33;
        cur_field_type_param->enums[35].value_from_mapping = 34;
        cur_field_type_param->enums[36].value_from_mapping = 35;
        cur_field_type_param->enums[37].value_from_mapping = 36;
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[122].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[123].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[124].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[125].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[126].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[39].value_from_mapping = 38;
        cur_field_type_param->enums[40].value_from_mapping = 39;
        cur_field_type_param->enums[41].value_from_mapping = 40;
        cur_field_type_param->enums[42].value_from_mapping = 41;
        cur_field_type_param->enums[43].value_from_mapping = 42;
        cur_field_type_param->enums[44].value_from_mapping = 43;
        cur_field_type_param->enums[45].value_from_mapping = 44;
        cur_field_type_param->enums[46].value_from_mapping = 45;
        cur_field_type_param->enums[47].value_from_mapping = 47;
        cur_field_type_param->enums[48].value_from_mapping = 48;
        cur_field_type_param->enums[49].value_from_mapping = 49;
        cur_field_type_param->enums[50].value_from_mapping = 50;
        cur_field_type_param->enums[51].value_from_mapping = 51;
        cur_field_type_param->enums[52].value_from_mapping = 52;
        cur_field_type_param->enums[53].value_from_mapping = 53;
        cur_field_type_param->enums[54].value_from_mapping = 54;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[77].value_from_mapping = 77;
        cur_field_type_param->enums[78].value_from_mapping = 78;
        cur_field_type_param->enums[79].value_from_mapping = 79;
        cur_field_type_param->enums[80].value_from_mapping = 80;
        cur_field_type_param->enums[81].value_from_mapping = 81;
        cur_field_type_param->enums[82].value_from_mapping = 82;
        cur_field_type_param->enums[83].value_from_mapping = 83;
        cur_field_type_param->enums[84].value_from_mapping = 84;
        cur_field_type_param->enums[85].value_from_mapping = 85;
        cur_field_type_param->enums[86].value_from_mapping = 86;
        cur_field_type_param->enums[87].value_from_mapping = 87;
        cur_field_type_param->enums[88].value_from_mapping = 88;
        cur_field_type_param->enums[89].value_from_mapping = 89;
        cur_field_type_param->enums[90].value_from_mapping = 90;
        cur_field_type_param->enums[91].value_from_mapping = 91;
        cur_field_type_param->enums[92].value_from_mapping = 92;
        cur_field_type_param->enums[93].value_from_mapping = 93;
        cur_field_type_param->enums[94].value_from_mapping = 94;
        cur_field_type_param->enums[95].value_from_mapping = 95;
        cur_field_type_param->enums[96].value_from_mapping = 96;
        cur_field_type_param->enums[97].value_from_mapping = 97;
        cur_field_type_param->enums[98].value_from_mapping = 98;
        cur_field_type_param->enums[99].value_from_mapping = 99;
        cur_field_type_param->enums[100].value_from_mapping = 100;
        cur_field_type_param->enums[101].value_from_mapping = 101;
        cur_field_type_param->enums[102].value_from_mapping = 102;
        cur_field_type_param->enums[103].value_from_mapping = 103;
        cur_field_type_param->enums[104].value_from_mapping = 104;
        cur_field_type_param->enums[105].value_from_mapping = 105;
        cur_field_type_param->enums[106].value_from_mapping = 106;
        cur_field_type_param->enums[107].value_from_mapping = 107;
        cur_field_type_param->enums[108].value_from_mapping = 108;
        cur_field_type_param->enums[109].value_from_mapping = 109;
        cur_field_type_param->enums[110].value_from_mapping = 110;
        cur_field_type_param->enums[111].value_from_mapping = 111;
        cur_field_type_param->enums[112].value_from_mapping = 112;
        cur_field_type_param->enums[113].value_from_mapping = 113;
        cur_field_type_param->enums[114].value_from_mapping = 114;
        cur_field_type_param->enums[115].value_from_mapping = 115;
        cur_field_type_param->enums[116].value_from_mapping = 116;
        cur_field_type_param->enums[117].value_from_mapping = 117;
        cur_field_type_param->enums[119].value_from_mapping = 118;
        cur_field_type_param->enums[120].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[121].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[127].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[128].value_from_mapping = 119;
        cur_field_type_param->enums[129].value_from_mapping = 127;
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 117 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "EMPTY", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ACL_CONTEXT", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ADMT_PROFILE", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BIER_STR_OFFSET", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "BIER_STR_SIZE", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "BYTES_TO_REMOVE", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "CONGESTION_INFO", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "DP", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "DP_METER_COMMAND", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "EEI", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "EGRESS_LEARN_ENABLE", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "END_OF_PACKET_EDITING", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "EXT_STATISTICS_0", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "EXT_STATISTICS_1", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "EXT_STATISTICS_2", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "EXT_STATISTICS_3", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "FWD", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "DST_DATA", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "FWD_DOMAIN", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "FWD_LAYER_ADDITIONAL_INFO", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "FWD_LAYER_INDEX", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "GENERAL_DATA_0", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "GENERAL_DATA_1", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "GENERAL_DATA_2", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "GENERAL_DATA_3", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IEEE1588_DATA", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "INGRESS_LEARN_ENABLE", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "INGRESS_TIME_STAMP", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "INGRESS_TIME_STAMP_OVERRIDE", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "GLOB_IN_LIF_0", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "IN_LIF_PROFILE_0", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "GLOB_IN_LIF_1", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "IN_LIF_PROFILE_1", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "IN_PP_PORT", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "INT_DATA", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "IN_TTL", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "ITPP_DELTA", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "LAG_LB_KEY", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "LATENCY_FLOW", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "LEARN_INFO_0", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "LEARN_INFO_1", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "LEARN_INFO_2", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "LEARN_INFO_3", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "LEARN_INFO_4", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "LEARN_OR_TRANSPLANT", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "MIRROR_DATA", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "NWK_LB_KEY", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "NWK_QOS", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "NWK_HEADER_APPEND_SIZE", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "OAM_DATA", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "OAM_ID", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "GLOB_OUT_LIF_0", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "GLOB_OUT_LIF_1", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "GLOB_OUT_LIF_2", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "GLOB_OUT_LIF_3", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "PACKET_IS_APPLET", sizeof(cur_field_type_param->enums[55].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[56].name_from_interface, "PACKET_IS_BIER", sizeof(cur_field_type_param->enums[56].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[57].name_from_interface, "EGRESS_PARSING_INDEX", sizeof(cur_field_type_param->enums[57].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[58].name_from_interface, "PPH_RESERVED", sizeof(cur_field_type_param->enums[58].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[59].name_from_interface, "PPH_TYPE", sizeof(cur_field_type_param->enums[59].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[60].name_from_interface, "PUSH_OUT_LIF", sizeof(cur_field_type_param->enums[60].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[61].name_from_interface, "SNOOP_DATA", sizeof(cur_field_type_param->enums[61].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[62].name_from_interface, "SRC_SYS_PORT", sizeof(cur_field_type_param->enums[62].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[63].name_from_interface, "STACKING_ROUTE_HISTORY_BITMAP", sizeof(cur_field_type_param->enums[63].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[64].name_from_interface, "STATISTICAL", sizeof(cur_field_type_param->enums[64].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[65].name_from_interface, "STATISTICS_META_DATA", sizeof(cur_field_type_param->enums[65].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[66].name_from_interface, "STATISTICS_ID_0", sizeof(cur_field_type_param->enums[66].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[67].name_from_interface, "STATISTICS_ID_1", sizeof(cur_field_type_param->enums[67].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[68].name_from_interface, "STATISTICS_OBJECT_10", sizeof(cur_field_type_param->enums[68].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[69].name_from_interface, "STATISTICS_ID_2", sizeof(cur_field_type_param->enums[69].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[70].name_from_interface, "STATISTICS_ID_3", sizeof(cur_field_type_param->enums[70].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[71].name_from_interface, "STATISTICS_ID_4", sizeof(cur_field_type_param->enums[71].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[72].name_from_interface, "STATISTICS_ID_5", sizeof(cur_field_type_param->enums[72].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[73].name_from_interface, "STATISTICS_ID_6", sizeof(cur_field_type_param->enums[73].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[74].name_from_interface, "STATISTICS_ID_7", sizeof(cur_field_type_param->enums[74].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[75].name_from_interface, "STATISTICS_ID_8", sizeof(cur_field_type_param->enums[75].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[76].name_from_interface, "STATISTICS_ID_9", sizeof(cur_field_type_param->enums[76].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[77].name_from_interface, "STATISTICS_ATR_0", sizeof(cur_field_type_param->enums[77].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[78].name_from_interface, "STATISTICS_ATR_1", sizeof(cur_field_type_param->enums[78].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[79].name_from_interface, "STATISTICS_ATR_2", sizeof(cur_field_type_param->enums[79].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[80].name_from_interface, "STATISTICS_ATR_3", sizeof(cur_field_type_param->enums[80].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[81].name_from_interface, "STATISTICS_ATR_4", sizeof(cur_field_type_param->enums[81].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[82].name_from_interface, "STATISTICS_ATR_5", sizeof(cur_field_type_param->enums[82].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[83].name_from_interface, "STATISTICS_ATR_6", sizeof(cur_field_type_param->enums[83].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[84].name_from_interface, "STATISTICS_ATR_7", sizeof(cur_field_type_param->enums[84].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[85].name_from_interface, "STATISTICS_ATR_8", sizeof(cur_field_type_param->enums[85].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[86].name_from_interface, "STATISTICS_ATR_9", sizeof(cur_field_type_param->enums[86].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[87].name_from_interface, "ST_VSQ_PTR", sizeof(cur_field_type_param->enums[87].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[88].name_from_interface, "SYSTEM_HEADER_PROFILE_INDEX", sizeof(cur_field_type_param->enums[88].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[89].name_from_interface, "SYSTEM_HEADER_SIZE_ADJUST", sizeof(cur_field_type_param->enums[89].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[90].name_from_interface, "TC", sizeof(cur_field_type_param->enums[90].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[91].name_from_interface, "TM_PROFILE", sizeof(cur_field_type_param->enums[91].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[92].name_from_interface, "USER_HEADER_1", sizeof(cur_field_type_param->enums[92].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[93].name_from_interface, "USER_HEADER_2", sizeof(cur_field_type_param->enums[93].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[94].name_from_interface, "USER_HEADER_3", sizeof(cur_field_type_param->enums[94].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[95].name_from_interface, "USER_HEADER_4", sizeof(cur_field_type_param->enums[95].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[96].name_from_interface, "USER_HEADERS_TYPE", sizeof(cur_field_type_param->enums[96].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[97].name_from_interface, "VISIBILITY", sizeof(cur_field_type_param->enums[97].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[98].name_from_interface, "VLAN_EDIT_CMD_INDEX", sizeof(cur_field_type_param->enums[98].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[99].name_from_interface, "VLAN_EDIT_PCP_DEI_1", sizeof(cur_field_type_param->enums[99].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[100].name_from_interface, "VLAN_EDIT_PCP_DEI_2", sizeof(cur_field_type_param->enums[100].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[101].name_from_interface, "VLAN_EDIT_VID_1", sizeof(cur_field_type_param->enums[101].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[102].name_from_interface, "VLAN_EDIT_VID_2", sizeof(cur_field_type_param->enums[102].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[103].name_from_interface, "PARSING_START_OFFSET", sizeof(cur_field_type_param->enums[103].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[104].name_from_interface, "PARSING_START_TYPE", sizeof(cur_field_type_param->enums[104].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[105].name_from_interface, "STAT_OBJ_LM_READ_INDEX", sizeof(cur_field_type_param->enums[105].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[106].name_from_interface, "TM_ACTION_TYPE", sizeof(cur_field_type_param->enums[106].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[107].name_from_interface, "DUAL_QUEUE", sizeof(cur_field_type_param->enums[107].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[108].name_from_interface, "NWK_HEADER_TRUNCATE_SIZE", sizeof(cur_field_type_param->enums[108].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[109].name_from_interface, "LEARN_INFO_KEY_0", sizeof(cur_field_type_param->enums[109].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[110].name_from_interface, "LEARN_INFO_KEY_1", sizeof(cur_field_type_param->enums[110].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[111].name_from_interface, "LEARN_INFO_KEY_2_APP_DB_INDEX", sizeof(cur_field_type_param->enums[111].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[112].name_from_interface, "LEARN_INFO_PAYLOAD_0", sizeof(cur_field_type_param->enums[112].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[113].name_from_interface, "LEARN_INFO_PAYLOAD_1", sizeof(cur_field_type_param->enums[113].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[114].name_from_interface, "LEARN_INFO_FORMAT_INDEX", sizeof(cur_field_type_param->enums[114].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[115].name_from_interface, "INVALIDATE_NEXT", sizeof(cur_field_type_param->enums[115].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[116].name_from_interface, "INVALID", sizeof(cur_field_type_param->enums[116].name_from_interface));
    cur_field_type_param->nof_enum_vals = 117;
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 26;
        cur_field_type_param->enums[28].value_from_mapping = 27;
        cur_field_type_param->enums[29].value_from_mapping = 28;
        cur_field_type_param->enums[30].value_from_mapping = 29;
        cur_field_type_param->enums[31].value_from_mapping = 30;
        cur_field_type_param->enums[32].value_from_mapping = 31;
        cur_field_type_param->enums[33].value_from_mapping = 32;
        cur_field_type_param->enums[34].value_from_mapping = 33;
        cur_field_type_param->enums[35].value_from_mapping = 34;
        cur_field_type_param->enums[36].value_from_mapping = 35;
        cur_field_type_param->enums[37].value_from_mapping = 36;
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[39].value_from_mapping = 38;
        cur_field_type_param->enums[40].value_from_mapping = 39;
        cur_field_type_param->enums[41].value_from_mapping = 40;
        cur_field_type_param->enums[42].value_from_mapping = 41;
        cur_field_type_param->enums[43].value_from_mapping = 42;
        cur_field_type_param->enums[44].value_from_mapping = 43;
        cur_field_type_param->enums[109].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[110].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[111].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[112].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[113].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[45].value_from_mapping = 44;
        cur_field_type_param->enums[46].value_from_mapping = 45;
        cur_field_type_param->enums[47].value_from_mapping = 46;
        cur_field_type_param->enums[48].value_from_mapping = 47;
        cur_field_type_param->enums[49].value_from_mapping = 48;
        cur_field_type_param->enums[50].value_from_mapping = 49;
        cur_field_type_param->enums[51].value_from_mapping = 50;
        cur_field_type_param->enums[52].value_from_mapping = 51;
        cur_field_type_param->enums[53].value_from_mapping = 52;
        cur_field_type_param->enums[54].value_from_mapping = 53;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[77].value_from_mapping = 77;
        cur_field_type_param->enums[78].value_from_mapping = 78;
        cur_field_type_param->enums[79].value_from_mapping = 79;
        cur_field_type_param->enums[80].value_from_mapping = 80;
        cur_field_type_param->enums[81].value_from_mapping = 81;
        cur_field_type_param->enums[82].value_from_mapping = 82;
        cur_field_type_param->enums[83].value_from_mapping = 83;
        cur_field_type_param->enums[84].value_from_mapping = 84;
        cur_field_type_param->enums[85].value_from_mapping = 85;
        cur_field_type_param->enums[86].value_from_mapping = 86;
        cur_field_type_param->enums[87].value_from_mapping = 87;
        cur_field_type_param->enums[88].value_from_mapping = 88;
        cur_field_type_param->enums[89].value_from_mapping = 89;
        cur_field_type_param->enums[90].value_from_mapping = 90;
        cur_field_type_param->enums[91].value_from_mapping = 91;
        cur_field_type_param->enums[92].value_from_mapping = 92;
        cur_field_type_param->enums[93].value_from_mapping = 93;
        cur_field_type_param->enums[94].value_from_mapping = 94;
        cur_field_type_param->enums[95].value_from_mapping = 95;
        cur_field_type_param->enums[96].value_from_mapping = 96;
        cur_field_type_param->enums[97].value_from_mapping = 97;
        cur_field_type_param->enums[98].value_from_mapping = 98;
        cur_field_type_param->enums[99].value_from_mapping = 99;
        cur_field_type_param->enums[100].value_from_mapping = 100;
        cur_field_type_param->enums[101].value_from_mapping = 101;
        cur_field_type_param->enums[102].value_from_mapping = 102;
        cur_field_type_param->enums[103].value_from_mapping = 103;
        cur_field_type_param->enums[104].value_from_mapping = 104;
        cur_field_type_param->enums[105].value_from_mapping = 105;
        cur_field_type_param->enums[106].value_from_mapping = 106;
        cur_field_type_param->enums[107].value_from_mapping = 107;
        cur_field_type_param->enums[108].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[114].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[115].value_from_mapping = 108;
        cur_field_type_param->enums[116].value_from_mapping = 127;
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
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 26;
        cur_field_type_param->enums[28].value_from_mapping = 27;
        cur_field_type_param->enums[29].value_from_mapping = 28;
        cur_field_type_param->enums[30].value_from_mapping = 29;
        cur_field_type_param->enums[31].value_from_mapping = 30;
        cur_field_type_param->enums[32].value_from_mapping = 31;
        cur_field_type_param->enums[33].value_from_mapping = 32;
        cur_field_type_param->enums[34].value_from_mapping = 33;
        cur_field_type_param->enums[35].value_from_mapping = 34;
        cur_field_type_param->enums[36].value_from_mapping = 35;
        cur_field_type_param->enums[37].value_from_mapping = 36;
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[109].value_from_mapping = 38;
        cur_field_type_param->enums[110].value_from_mapping = 39;
        cur_field_type_param->enums[111].value_from_mapping = 40;
        cur_field_type_param->enums[112].value_from_mapping = 41;
        cur_field_type_param->enums[113].value_from_mapping = 42;
        cur_field_type_param->enums[44].value_from_mapping = 43;
        cur_field_type_param->enums[39].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[40].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[41].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[42].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[43].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[45].value_from_mapping = 44;
        cur_field_type_param->enums[46].value_from_mapping = 45;
        cur_field_type_param->enums[47].value_from_mapping = 46;
        cur_field_type_param->enums[48].value_from_mapping = 47;
        cur_field_type_param->enums[49].value_from_mapping = 48;
        cur_field_type_param->enums[50].value_from_mapping = 49;
        cur_field_type_param->enums[51].value_from_mapping = 50;
        cur_field_type_param->enums[52].value_from_mapping = 51;
        cur_field_type_param->enums[53].value_from_mapping = 52;
        cur_field_type_param->enums[54].value_from_mapping = 53;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[77].value_from_mapping = 77;
        cur_field_type_param->enums[78].value_from_mapping = 78;
        cur_field_type_param->enums[79].value_from_mapping = 79;
        cur_field_type_param->enums[80].value_from_mapping = 80;
        cur_field_type_param->enums[81].value_from_mapping = 81;
        cur_field_type_param->enums[82].value_from_mapping = 82;
        cur_field_type_param->enums[83].value_from_mapping = 83;
        cur_field_type_param->enums[84].value_from_mapping = 84;
        cur_field_type_param->enums[85].value_from_mapping = 85;
        cur_field_type_param->enums[86].value_from_mapping = 86;
        cur_field_type_param->enums[87].value_from_mapping = 87;
        cur_field_type_param->enums[88].value_from_mapping = 88;
        cur_field_type_param->enums[89].value_from_mapping = 89;
        cur_field_type_param->enums[90].value_from_mapping = 90;
        cur_field_type_param->enums[91].value_from_mapping = 91;
        cur_field_type_param->enums[92].value_from_mapping = 92;
        cur_field_type_param->enums[93].value_from_mapping = 93;
        cur_field_type_param->enums[94].value_from_mapping = 94;
        cur_field_type_param->enums[95].value_from_mapping = 95;
        cur_field_type_param->enums[96].value_from_mapping = 96;
        cur_field_type_param->enums[97].value_from_mapping = 97;
        cur_field_type_param->enums[98].value_from_mapping = 98;
        cur_field_type_param->enums[99].value_from_mapping = 99;
        cur_field_type_param->enums[100].value_from_mapping = 100;
        cur_field_type_param->enums[101].value_from_mapping = 101;
        cur_field_type_param->enums[102].value_from_mapping = 102;
        cur_field_type_param->enums[103].value_from_mapping = 103;
        cur_field_type_param->enums[104].value_from_mapping = 104;
        cur_field_type_param->enums[105].value_from_mapping = 105;
        cur_field_type_param->enums[106].value_from_mapping = 106;
        cur_field_type_param->enums[107].value_from_mapping = 107;
        cur_field_type_param->enums[108].value_from_mapping = 108;
        cur_field_type_param->enums[114].value_from_mapping = 109;
        cur_field_type_param->enums[115].value_from_mapping = 113;
        cur_field_type_param->enums[116].value_from_mapping = 127;
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 26;
        cur_field_type_param->enums[28].value_from_mapping = 27;
        cur_field_type_param->enums[29].value_from_mapping = 28;
        cur_field_type_param->enums[30].value_from_mapping = 29;
        cur_field_type_param->enums[31].value_from_mapping = 30;
        cur_field_type_param->enums[32].value_from_mapping = 31;
        cur_field_type_param->enums[33].value_from_mapping = 32;
        cur_field_type_param->enums[34].value_from_mapping = 33;
        cur_field_type_param->enums[35].value_from_mapping = 34;
        cur_field_type_param->enums[36].value_from_mapping = 35;
        cur_field_type_param->enums[37].value_from_mapping = 36;
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[39].value_from_mapping = 38;
        cur_field_type_param->enums[40].value_from_mapping = 39;
        cur_field_type_param->enums[41].value_from_mapping = 40;
        cur_field_type_param->enums[42].value_from_mapping = 41;
        cur_field_type_param->enums[43].value_from_mapping = 42;
        cur_field_type_param->enums[44].value_from_mapping = 43;
        cur_field_type_param->enums[109].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[110].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[111].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[112].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[113].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[45].value_from_mapping = 44;
        cur_field_type_param->enums[46].value_from_mapping = 45;
        cur_field_type_param->enums[47].value_from_mapping = 46;
        cur_field_type_param->enums[48].value_from_mapping = 47;
        cur_field_type_param->enums[49].value_from_mapping = 48;
        cur_field_type_param->enums[50].value_from_mapping = 49;
        cur_field_type_param->enums[51].value_from_mapping = 50;
        cur_field_type_param->enums[52].value_from_mapping = 51;
        cur_field_type_param->enums[53].value_from_mapping = 52;
        cur_field_type_param->enums[54].value_from_mapping = 53;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[77].value_from_mapping = 77;
        cur_field_type_param->enums[78].value_from_mapping = 78;
        cur_field_type_param->enums[79].value_from_mapping = 79;
        cur_field_type_param->enums[80].value_from_mapping = 80;
        cur_field_type_param->enums[81].value_from_mapping = 81;
        cur_field_type_param->enums[82].value_from_mapping = 82;
        cur_field_type_param->enums[83].value_from_mapping = 83;
        cur_field_type_param->enums[84].value_from_mapping = 84;
        cur_field_type_param->enums[85].value_from_mapping = 85;
        cur_field_type_param->enums[86].value_from_mapping = 86;
        cur_field_type_param->enums[87].value_from_mapping = 87;
        cur_field_type_param->enums[88].value_from_mapping = 88;
        cur_field_type_param->enums[89].value_from_mapping = 89;
        cur_field_type_param->enums[90].value_from_mapping = 90;
        cur_field_type_param->enums[91].value_from_mapping = 91;
        cur_field_type_param->enums[92].value_from_mapping = 92;
        cur_field_type_param->enums[93].value_from_mapping = 93;
        cur_field_type_param->enums[94].value_from_mapping = 94;
        cur_field_type_param->enums[95].value_from_mapping = 95;
        cur_field_type_param->enums[96].value_from_mapping = 96;
        cur_field_type_param->enums[97].value_from_mapping = 97;
        cur_field_type_param->enums[98].value_from_mapping = 98;
        cur_field_type_param->enums[99].value_from_mapping = 99;
        cur_field_type_param->enums[100].value_from_mapping = 100;
        cur_field_type_param->enums[101].value_from_mapping = 101;
        cur_field_type_param->enums[102].value_from_mapping = 102;
        cur_field_type_param->enums[103].value_from_mapping = 103;
        cur_field_type_param->enums[104].value_from_mapping = 104;
        cur_field_type_param->enums[105].value_from_mapping = 105;
        cur_field_type_param->enums[106].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[107].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[108].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[114].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[115].value_from_mapping = 106;
        cur_field_type_param->enums[116].value_from_mapping = 127;
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 28 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "EMPTY", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "DISCARD", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TC_MAP_PROFILE", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TC", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "DP", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "CUD_OUTLIF_OR_MCDB_PTR", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "PP_DSP", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "COS_PROFILE", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "MIRROR_PROFILE", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "SNOOP_PROFILE", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "SNOOP_STRENGTH", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "FWD_ACTION_PROFILE_INDEX", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "FWD_STRENGTH", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "PMF_FWD_STRENGTH", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "PMF_SNOOP_STRENGTH", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "PMF_PROFILE", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "PMF_SNOOP_OVERRIDE_ENABLE", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "PMF_FWD_OVERRIDE_ENABLE", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "PMF_COUNTER_0_PROFILE", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "PMF_COUNTER_0_PTR", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "PMF_COUNTER_1_PROFILE", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "PMF_COUNTER_1_PTR", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "EBTR_EOP", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "EBTA_EOP", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "EBTA_SOP", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "EBTR_SOP", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "INVALIDATE_NEXT", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "INVALID", sizeof(cur_field_type_param->enums[27].name_from_interface));
    cur_field_type_param->nof_enum_vals = 28;
    if (DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].value_from_mapping = 21;
        cur_field_type_param->enums[23].value_from_mapping = 22;
        cur_field_type_param->enums[24].value_from_mapping = 23;
        cur_field_type_param->enums[25].value_from_mapping = 24;
        cur_field_type_param->enums[26].value_from_mapping = 25;
        cur_field_type_param->enums[27].value_from_mapping = 127;
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 0;
        cur_field_type_param->enums[2].value_from_mapping = 1;
        cur_field_type_param->enums[3].value_from_mapping = 2;
        cur_field_type_param->enums[4].value_from_mapping = 3;
        cur_field_type_param->enums[5].value_from_mapping = 4;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 6;
        cur_field_type_param->enums[8].value_from_mapping = 7;
        cur_field_type_param->enums[9].value_from_mapping = 8;
        cur_field_type_param->enums[10].value_from_mapping = 9;
        cur_field_type_param->enums[11].value_from_mapping = 10;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 12;
        cur_field_type_param->enums[14].value_from_mapping = 13;
        cur_field_type_param->enums[15].value_from_mapping = 14;
        cur_field_type_param->enums[16].value_from_mapping = 15;
        cur_field_type_param->enums[17].value_from_mapping = 16;
        cur_field_type_param->enums[18].value_from_mapping = 17;
        cur_field_type_param->enums[19].value_from_mapping = 18;
        cur_field_type_param->enums[20].value_from_mapping = 19;
        cur_field_type_param->enums[21].value_from_mapping = 20;
        cur_field_type_param->enums[22].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[23].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[24].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[25].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[26].value_from_mapping = 21;
        cur_field_type_param->enums[27].value_from_mapping = 127;
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 36 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "EMPTY", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "PP_DSP", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "FTMH_TC", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FTMH_DP", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FTMH_TM_ACTION_TYPE", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "FTMH_CNI", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FTMH_ECN_ENABLE", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "FTMH_VISIBILITY", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "PPH_FORWARD_DOMAIN", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "PPH_NWK_QOS", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "PPH_VALUE1", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "PPH_VALUE2", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "FHEI_EXT31TO00", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "FHEI_EXT63TO32", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "PPH_TTL", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "PPH_IN_LIF", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "PPH_IN_LIF_PROFILE", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "CUD_OUT_LIF0_CUD_OUT_LIF0VALID", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "CUD_OUT_LIF1_CUD_OUT_LIF1VALID", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "CUD_OUT_LIF2_CUD_OUT_LIF2VALID", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "CUD_OUT_LIF3_CUD_OUT_LIF3VALID", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "PPH_FORWARDING_LAYER_ADDITIONAL_INFO", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "PPH_FWD_LAYER_INDEX", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "PPH_FORWANDING_STRENGTH", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "PPH_END_OF_PACKET_EDITING", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "PPH_APPLICATION_SPECIFIC_EXT_31TO00", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "PPH_APPLICATION_SPECIFIC_EXT_47TO32", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "SNIFF_PROFILE", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "FTMH_SOURCE_SYSTEM_PORT", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "ACE_CONTEXT_VALUE", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "ACE_STAMP_VALUE", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "ACE_STAT_METER_OBJ_CMD_ID", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "ACE_STAT_OBJ_CMD_ID_VALID", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "ACE_FWD_ACTION_PROFILE_VALID_PROFILE", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "INVALIDATE_NEXT", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "INVALID", sizeof(cur_field_type_param->enums[35].name_from_interface));
    cur_field_type_param->nof_enum_vals = 36;
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
    }
    else
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
        cur_field_type_param->enums[33].is_invalid_value_from_mapping = TRUE;
        cur_field_type_param->enums[34].value_from_mapping = 33;
        cur_field_type_param->enums[35].value_from_mapping = 34;
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 7 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "EXEM", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "DIRECT", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TCAM_0", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TCAM_1", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TCAM_2", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "TCAM_3", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "STATE_TABLE", sizeof(cur_field_type_param->enums[6].name_from_interface));
    cur_field_type_param->nof_enum_vals = 7;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "PMF", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FLP", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "VTT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "ERPP", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 41 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "TCAM_0_63_0", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TCAM_1_31_0_TCAM_0_63_32", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TCAM_1_63_0", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TCAM_2_31_0_TCAM_1_63_32", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TCAM_2_63_0", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "TCAM_3_31_0_TCAM_2_63_32", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "TCAM_3_63_0", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "TCAM_4_31_0_TCAM_3_63_32", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "TCAM_4_63_0", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "TCAM_5_31_0_TCAM_4_63_32", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "TCAM_5_63_0", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "TCAM_6_31_0_TCAM_5_63_32", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "TCAM_6_63_0", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "TCAM_7_31_0_TCAM_6_63_32", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "TCAM_7_63_0", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "LEXEM_27_0_4_B0_TCAM_7_63_32", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "LEXEM_59_0_4_B0", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "SEXEM_27_0_4_B0_LEXEM_59_28", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "SEXEM_59_0_4_B0", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "KEY_I_31_0_SEXEM_59_28", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "KEY_I_63_0", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "KEY_I_95_32", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "KEY_I_127_64", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "KEY_I_159_96", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "KEY_J_31_0_KEY_I_159_128", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "KEY_J_63_0", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "KEY_J_95_32", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "KEY_J_127_64", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "KEY_J_159_96", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "DIRECT_0_23_0_8_B0_KEY_J_159_128", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "DIRECT_0_55_0_8_B0", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "DIRECT_0_87_24", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "DIRECT_0_119_56", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "DIRECT_1_23_0_8_B0_DIRECT_0_119_88", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "DIRECT_1_55_0_8_B0", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "DIRECT_1_87_24", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "DIRECT_1_119_56", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "DIRECT_1_119_88", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "INVALID_FES_KEY_SELECT", sizeof(cur_field_type_param->enums[40].name_from_interface));
    cur_field_type_param->nof_enum_vals = 41;
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
        cur_field_type_param->enums[38].value_from_mapping = 37;
        cur_field_type_param->enums[40].value_from_mapping = 63;
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
        cur_field_type_param->enums[39].value_from_mapping = 37;
        cur_field_type_param->enums[40].value_from_mapping = 63;
    }
    else
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 19 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "TCAM_0_63_0", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TCAM_1_31_0_TCAM_0_63_32", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TCAM_1_63_0", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "SEXEM_27_0_4_B0_TCAM_1_63_32", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "SEXEM_59_0_4_B0", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "KEY_A_31_0_SEXEM_59_28", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "KEY_A_63_0", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "KEY_A_95_32", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "KEY_A_127_64", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "KEY_A_159_96", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "KEY_B_31_0_KEY_A_159_128", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "KEY_B_63_0", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "KEY_B_95_32", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "KEY_B_127_64", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "KEY_B_159_96", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "DIRECT_31_0_KEY_B_159_128", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "DIRECT_55_0_8_B0", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "DIRECT_87_24", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "DIRECT_119_56", sizeof(cur_field_type_param->enums[18].name_from_interface));
    cur_field_type_param->nof_enum_vals = 19;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 18 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "LEXEM_4_B0_59_0", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "LEXEM_59_0_4_B0", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TCAM_0_PAYLOAD_31_0_LEXEM_59_28", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TCAM_0_PAYLOAD_63_0", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "TCAM_1_PAYLOAD_63_0", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "KEY_D_31_0_TCAM_1_PAYLOAD_63_32", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "NOT_IN_USE_6", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "NOT_IN_USE_7", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "NOT_IN_USE_8", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "NOT_IN_USE_9", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "NOT_IN_USE_10", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "NOT_IN_USE_11", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "NOT_IN_USE_12", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "NOT_IN_USE_13", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "NOT_IN_USE_14", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "NOT_IN_USE_15", sizeof(cur_field_type_param->enums[17].name_from_interface));
    cur_field_type_param->nof_enum_vals = 18;
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0)
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[2].value_from_mapping = 0;
        cur_field_type_param->enums[3].value_from_mapping = 1;
        cur_field_type_param->enums[4].value_from_mapping = 2;
        cur_field_type_param->enums[5].value_from_mapping = 3;
        cur_field_type_param->enums[6].value_from_mapping = 4;
        cur_field_type_param->enums[7].value_from_mapping = 5;
        cur_field_type_param->enums[8].value_from_mapping = 6;
        cur_field_type_param->enums[9].value_from_mapping = 7;
        cur_field_type_param->enums[10].value_from_mapping = 8;
        cur_field_type_param->enums[11].value_from_mapping = 9;
        cur_field_type_param->enums[12].value_from_mapping = 10;
        cur_field_type_param->enums[13].value_from_mapping = 11;
        cur_field_type_param->enums[14].value_from_mapping = 12;
        cur_field_type_param->enums[15].value_from_mapping = 13;
        cur_field_type_param->enums[16].value_from_mapping = 14;
        cur_field_type_param->enums[17].value_from_mapping = 15;
    }
    else
    {
        
        
        
        
        cur_field_type_param->nof_illegal_value = 0;
        cur_field_type_param->min_value = DBAL_DB_INVALID;
        cur_field_type_param->max_value = DBAL_DB_INVALID;
        cur_field_type_param->const_value = DBAL_DB_INVALID;
        cur_field_type_param->default_val = 0;
        cur_field_type_param->default_val_valid = FALSE;
        
        cur_field_type_param->enums[0].value_from_mapping = 0;
        cur_field_type_param->enums[1].value_from_mapping = 1;
        cur_field_type_param->enums[4].value_from_mapping = 2;
        cur_field_type_param->enums[5].value_from_mapping = 3;
        cur_field_type_param->enums[6].value_from_mapping = 4;
        cur_field_type_param->enums[8].value_from_mapping = 6;
        cur_field_type_param->enums[9].value_from_mapping = 7;
        cur_field_type_param->enums[10].value_from_mapping = 8;
        cur_field_type_param->enums[11].value_from_mapping = 9;
        cur_field_type_param->enums[12].value_from_mapping = 10;
        cur_field_type_param->enums[13].value_from_mapping = 11;
        cur_field_type_param->enums[14].value_from_mapping = 12;
        cur_field_type_param->enums[15].value_from_mapping = 13;
        cur_field_type_param->enums[16].value_from_mapping = 14;
        cur_field_type_param->enums[17].value_from_mapping = 15;
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 80 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "TCAM_0_31_0", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TCAM_0_47_16", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TCAM_0_63_32", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TCAM_1_15_0_TCAM_0_63_48", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TCAM_1_31_0", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "TCAM_1_47_16", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "TCAM_1_63_32", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "TCAM_2_15_0_TCAM_1_63_48", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "TCAM_2_31_0", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "TCAM_2_47_16", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "TCAM_2_63_32", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "TCAM_3_15_0_TCAM_2_63_48", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "TCAM_3_31_0", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "TCAM_3_47_16", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "TCAM_3_63_32", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "TCAM_4_15_0_TCAM_3_63_48", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "TCAM_4_31_0", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "TCAM_4_47_16", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "TCAM_4_63_32", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "TCAM_5_15_0_TCAM_4_63_48", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "TCAM_5_31_0", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "TCAM_5_47_16", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "TCAM_5_63_32", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "TCAM_6_15_0_TCAM_5_63_48", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "TCAM_6_31_0", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "TCAM_6_47_16", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "TCAM_6_63_32", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "TCAM_7_15_0_TCAM_6_63_48", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "TCAM_7_31_0", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "TCAM_7_47_16", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "TCAM_7_63_32", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "LEXEM_11_0_4_B0_TCAM_7_63_48", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "LEXEM_27_0_4_B0", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "LEXEM_43_12", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "LEXEM_59_28", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "SEXEM_11_0_4_B0_LEXEM_59_44", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "SEXEM_27_0_4_B0", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "SEXEM_43_12", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "SEXEM_59_28", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "KEY_I_16_0_SEXEM_59_44", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "KEY_I_31_0", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "KEY_I_47_16", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "KEY_I_63_32", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "KEY_I_79_48", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "KEY_I_95_64", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "KEY_I_111_80", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "KEY_I_127_96", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "KEY_I_143_112", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "KEY_I_159_128", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "KEY_J_15_0_KEY_I_159_144", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "KEY_J_31_0", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "KEY_J_47_16", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "KEY_J_63_32", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "KEY_J_79_48", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "KEY_J_95_64", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "KEY_J_111_80", sizeof(cur_field_type_param->enums[55].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[56].name_from_interface, "KEY_J_127_96", sizeof(cur_field_type_param->enums[56].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[57].name_from_interface, "KEY_J_143_112", sizeof(cur_field_type_param->enums[57].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[58].name_from_interface, "KEY_J_159_128", sizeof(cur_field_type_param->enums[58].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[59].name_from_interface, "DIRECT_0_7_0_8_B0_KEY_J_159_144", sizeof(cur_field_type_param->enums[59].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[60].name_from_interface, "DIRECT_0_23_0_8_B0", sizeof(cur_field_type_param->enums[60].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[61].name_from_interface, "DIRECT_0_39_8", sizeof(cur_field_type_param->enums[61].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[62].name_from_interface, "DIRECT_0_55_24", sizeof(cur_field_type_param->enums[62].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[63].name_from_interface, "DIRECT_0_71_40", sizeof(cur_field_type_param->enums[63].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[64].name_from_interface, "DIRECT_0_87_56", sizeof(cur_field_type_param->enums[64].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[65].name_from_interface, "DIRECT_0_103_72", sizeof(cur_field_type_param->enums[65].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[66].name_from_interface, "DIRECT_0_119_88", sizeof(cur_field_type_param->enums[66].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[67].name_from_interface, "DIRECT_1_7_0_8_B0_DIRECT_0_119_104", sizeof(cur_field_type_param->enums[67].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[68].name_from_interface, "DIRECT_1_23_0_8_B0", sizeof(cur_field_type_param->enums[68].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[69].name_from_interface, "DIRECT_1_39_8", sizeof(cur_field_type_param->enums[69].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[70].name_from_interface, "DIRECT_1_55_24", sizeof(cur_field_type_param->enums[70].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[71].name_from_interface, "DIRECT_1_71_40", sizeof(cur_field_type_param->enums[71].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[72].name_from_interface, "DIRECT_1_87_56", sizeof(cur_field_type_param->enums[72].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[73].name_from_interface, "DIRECT_1_103_72", sizeof(cur_field_type_param->enums[73].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[74].name_from_interface, "DIRECT_1_119_88", sizeof(cur_field_type_param->enums[74].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[75].name_from_interface, "STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104", sizeof(cur_field_type_param->enums[75].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[76].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0", sizeof(cur_field_type_param->enums[76].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[77].name_from_interface, "STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104", sizeof(cur_field_type_param->enums[77].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[78].name_from_interface, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0", sizeof(cur_field_type_param->enums[78].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[79].name_from_interface, "INVALID_FEM_KEY_SELECT", sizeof(cur_field_type_param->enums[79].name_from_interface));
    cur_field_type_param->nof_enum_vals = 80;
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
        cur_field_type_param->enums[54].value_from_mapping = 54;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[75].value_from_mapping = 75;
        cur_field_type_param->enums[76].value_from_mapping = 76;
        cur_field_type_param->enums[79].value_from_mapping = 127;
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
        cur_field_type_param->enums[54].value_from_mapping = 54;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[77].value_from_mapping = 75;
        cur_field_type_param->enums[78].value_from_mapping = 76;
        cur_field_type_param->enums[79].value_from_mapping = 127;
    }
    else
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
        cur_field_type_param->enums[54].value_from_mapping = 54;
        cur_field_type_param->enums[55].value_from_mapping = 55;
        cur_field_type_param->enums[56].value_from_mapping = 56;
        cur_field_type_param->enums[57].value_from_mapping = 57;
        cur_field_type_param->enums[58].value_from_mapping = 58;
        cur_field_type_param->enums[59].value_from_mapping = 59;
        cur_field_type_param->enums[60].value_from_mapping = 60;
        cur_field_type_param->enums[61].value_from_mapping = 61;
        cur_field_type_param->enums[62].value_from_mapping = 62;
        cur_field_type_param->enums[63].value_from_mapping = 63;
        cur_field_type_param->enums[64].value_from_mapping = 64;
        cur_field_type_param->enums[65].value_from_mapping = 65;
        cur_field_type_param->enums[66].value_from_mapping = 66;
        cur_field_type_param->enums[67].value_from_mapping = 67;
        cur_field_type_param->enums[68].value_from_mapping = 68;
        cur_field_type_param->enums[69].value_from_mapping = 69;
        cur_field_type_param->enums[70].value_from_mapping = 70;
        cur_field_type_param->enums[71].value_from_mapping = 71;
        cur_field_type_param->enums[72].value_from_mapping = 72;
        cur_field_type_param->enums[73].value_from_mapping = 73;
        cur_field_type_param->enums[74].value_from_mapping = 74;
        cur_field_type_param->enums[79].value_from_mapping = 127;
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "FEM_ACTIONS_IS_NOT_ACTIVE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FEM_ACTIONS_IS_ACTIVE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "REPLACE_LSB_IS_NOT_ACTIVE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "REPLACE_LSB_IS_ACTIVE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 8 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "TCAM_0", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "TCAM_1", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "TCAM_2", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "TCAM_3", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "TCAM_4", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "TCAM_5", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "TCAM_6", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "TCAM_7", sizeof(cur_field_type_param->enums[7].name_from_interface));
    cur_field_type_param->nof_enum_vals = 8;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 14 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "FIRST_RESERVED", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "SECOND_RESERVED", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "THIRD_RESERVED", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "CRC16_BISYNC", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "CRC16_XOR1", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "CRC16_XOR2", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "CRC16_XOR4", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "CRC16_XOR8", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "XOR16", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "CRC16_CCITT", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "CRC32_ALOW", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "CRC32_AHIGH", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "CRC32_BLOW", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "CRC32_BHIGH", sizeof(cur_field_type_param->enums[13].name_from_interface));
    cur_field_type_param->nof_enum_vals = 14;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 8 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "L4_DST_PORT", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "L4_SRC_PORT", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "IN_TLL", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FFC_1_LOW", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FFC_1_HIGH", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "FFC_2_LOW", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FFC_2_HIGH", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "PACKET_HEADER_SIZE", sizeof(cur_field_type_param->enums[7].name_from_interface));
    cur_field_type_param->nof_enum_vals = 8;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 3 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "AND", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "OR", sizeof(cur_field_type_param->enums[2].name_from_interface));
    cur_field_type_param->nof_enum_vals = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 2;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "32", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "64", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FIRST", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "SECOND", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "BOTH", sizeof(cur_field_type_param->enums[3].name_from_interface));
    cur_field_type_param->nof_enum_vals = 4;
    
    
    
    
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
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 16 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FIRST", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "SECOND", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "THIRD", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FORTH", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "FIRST_SECOND", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FIRST_THIRD", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "FIRST_FORTH", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "SECOND_THIRD", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "SECOND_FORTH", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "THIRD_FORTH", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "FIRST_SECOND_THIRD", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "FIRST_SECOND_FORTH", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "FIRST_THIRD_FORTH", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "SECOND_THIRD_FORTH", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "FIRST_SECOND_THIRD_FORTH", sizeof(cur_field_type_param->enums[15].name_from_interface));
    cur_field_type_param->nof_enum_vals = 16;
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
        cur_field_type_param->enums[3].value_from_mapping = 4;
        cur_field_type_param->enums[4].value_from_mapping = 8;
        cur_field_type_param->enums[5].value_from_mapping = 3;
        cur_field_type_param->enums[6].value_from_mapping = 5;
        cur_field_type_param->enums[7].value_from_mapping = 9;
        cur_field_type_param->enums[8].value_from_mapping = 6;
        cur_field_type_param->enums[9].value_from_mapping = 10;
        cur_field_type_param->enums[10].value_from_mapping = 12;
        cur_field_type_param->enums[11].value_from_mapping = 7;
        cur_field_type_param->enums[12].value_from_mapping = 11;
        cur_field_type_param->enums[13].value_from_mapping = 13;
        cur_field_type_param->enums[14].value_from_mapping = 14;
        cur_field_type_param->enums[15].value_from_mapping = 15;
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
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_tm_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_in_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
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
