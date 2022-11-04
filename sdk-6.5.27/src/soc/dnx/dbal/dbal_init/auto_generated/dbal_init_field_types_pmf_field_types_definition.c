
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_acl_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACL_CONTEXT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACL_CONTEXT", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_pmf_layer_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PMF_LAYER_TYPE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PMF_LAYER_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CONTEXT_KEY_GEN_VAR", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PMF_DATA", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OUT_TM_PORT_PMF_DATA", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IN_PORT_PMF_DATA", dnx_data_field.profile_bits.ingress_pp_port_key_gen_var_size_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "AC_IN_LIF_WIDE_DATA", dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NATIVE_AC_IN_LIF_WIDE_DATA", dnx_data_field.general_data_qualifiers.native_ac_in_lif_wide_data_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "AC_IN_LIF_WIDE_DATA_EXTENDED", dnx_data_field.general_data_qualifiers.ac_in_lif_wide_data_extended_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PWE_IN_LIF_WIDE_DATA", dnx_data_field.general_data_qualifiers.pwe_in_lif_wide_data_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_field_types_definition_ip_tunnel_in_lif_wide_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IP_TUNNEL_IN_LIF_WIDE_DATA];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IP_TUNNEL_IN_LIF_WIDE_DATA", dnx_data_field.general_data_qualifiers.ip_tunnel_in_lif_wide_data_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VW_VIP_VALID", dnx_data_field.general_data_qualifiers.vw_vip_valid_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VW_VIP_ID", dnx_data_field.general_data_qualifiers.vw_vip_id_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VW_MEMBER_REFERENCE", dnx_data_field.general_data_qualifiers.vw_member_reference_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VW_PCC_HIT", dnx_data_field.general_data_qualifiers.vw_pcc_hit_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OUT_PP_PORT_PMF_DATA", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RPF_DST", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ALL_ONES", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CMP_KEY_DECODED", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IEEE1588_COMMAND", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IEEE1588_HEADER_OFFSET", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_PROFILE", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TRAP_QUALIFIER", 19, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ADMT_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BIER_STR_OFFSET", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BIER_STR_SIZE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "METADATA_ECN", dnx_data_field.signal_sizes.ecn_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CONGESTION_INFO", dnx_data_field.signal_sizes.congestion_info_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DST_QUALIFIER", 11, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DST_DATA", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_DST_QUALIFIER;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PARSING_START_TYPE_ACTION", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VALID;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PARSING_START_TYPE_ENCODING;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_LAYER_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DP_METER_COMMAND", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TRACE_PACKET_ACT", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ELEPHANT_PAYLOAD", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATISTICS_OBJECT", 20, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXT_STATISTICS", 23, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VALID;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_OPCODE;
    cur_field_types_info->struct_field_info[1].length = 2;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ID;
    cur_field_types_info->struct_field_info[2].length = 20;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RPF_OUT_LIF_ENCODED", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VALID;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FOUND;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_RPF_OUT_LIF;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEM_SOFT_ERR", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NASID", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_NASID_FWD2 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_VT1, "VT1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_VT2, "VT2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_VT3, "VT3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_VT4, "VT4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_VT5, "VT5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_FWD1, "FWD1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NASID_FWD2, "FWD2");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_CONTEXT", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RPF_ECMP_GROUP", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RPF_ECMP_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NWK_QOS", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DISCARD", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SNOOP_DATA", 20, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SNOOP_STRENGTH", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_STRENGTH", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PMF_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SNOOP_QUALIFIER", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SNOOP_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IN_TTL", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INVALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INVALIDATE_NEXT", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PPH_VALUE", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_DOMAIN_PROFILE", 10, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_ADDRESS_DATA", 19, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_DATA", 9, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_TABLE_IF_READ", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF2 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF1, "IPMF1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_READ_IPMF2, "IPMF2");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_TABLE_IF_WRITE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF1, "IPMF1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_IPMF2, "IPMF2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_ACR, "ACR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_WRITE_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_TABLE_IF_RMW", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_FALLBACK + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF1, "IPMF1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_IPMF2, "IPMF2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_ACR, "ACR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IF_RMW_FALLBACK, "FALLBACK");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_TABLE_ADDRESS_RESOLUTION_RMW", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_EIGHTH_ENTRY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_FULL_ENTRY, "FULL_ENTRY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_HALF_ENTRY, "HALF_ENTRY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_QUARTER_ENTRY, "QUARTER_ENTRY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_ADDRESS_RESOLUTION_RMW_EIGHTH_ENTRY, "EIGHTH_ENTRY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATE_TABLE_IPMF1_KEY_SELECT", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_F_LSB + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_J_MSB, "KEY_J_MSB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STATE_TABLE_IPMF1_KEY_SELECT_KEY_F_LSB, "KEY_F_LSB");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATISTICS_META_DATA", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_DATA", 21, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PPH_RESERVED", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATISTICAL_SAMPLING", 25, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_PARSING_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATISTICAL_SAMPLING_CODE", 9, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STATISTICAL_SAMPLING_QUALIFIER", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SRC_SYS_PORT", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "HEADER_SIZE", dnx_data_field.signal_sizes.packet_header_size_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PACKET_IS_BIER", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IS_APPLET", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PACKET_IS_IEEE1588", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4OPS", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIRROR_DATA", 17, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIRROR_CODE", 9, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIRROR_QUALIFIER", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STACKING_ROUTE_HISTORY_BITMAP", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BYTES_TO_REMOVE", 9, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VISIBILITY", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VISIBILITY_CLEAR", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TIME_STAMP", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INT_DATA", 10, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IEEE1588_DATA", 13, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LATENCY_FLOW", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PMF_OAM_ID", 19, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "STAT_OBJ_LM_READ_INDEX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_0, "0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_1, "1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_2, "2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_STAT_OBJ_LM_READ_INDEX_INVALID, "INVALID");
    }
    if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FWD_TRAP", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ITPP_DELTA", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ITPP_DELTA_AND_VALID", 9, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EVENTOR", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "USER_HEADER", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "USER_HEADER_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NWK_HEADER_APPEND_SIZE", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TC_MAP_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SYSTEM_HEADER_PROFILE_INDEX", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SYSTEM_HEADER_SIZE_ADJUST", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TM_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VRF_VALUE", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPMF1_ACTION", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY, "EMPTY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT, "ACL_CONTEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE, "ADMT_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET, "BIER_STR_OFFSET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE, "BIER_STR_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO, "CONGESTION_INFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, "DST_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_DP, "DP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND, "DP_METER_COMMAND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EEI, "EEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE, "EGRESS_LEARN_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD, "ELEPHANT_PAYLOAD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT, "TRACE_PACKET_ACT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING, "END_OF_PACKET_EDITING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0, "EXT_STATISTICS_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1, "EXT_STATISTICS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2, "EXT_STATISTICS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3, "EXT_STATISTICS_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD, "FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN, "FWD_DOMAIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO, "FWD_LAYER_ADDITIONAL_INFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT, "FWD_CONTEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX, "FWD_LAYER_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA, "IEEE1588_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE, "INGRESS_LEARN_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0, "GLOB_IN_LIF_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1, "GLOB_IN_LIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0, "IN_LIF_PROFILE_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1, "IN_LIF_PROFILE_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT, "IN_PP_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL, "IN_TTL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA, "ITPP_DELTA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW, "LATENCY_FLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0, "LEARN_INFO_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1, "LEARN_INFO_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2, "LEARN_INFO_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3, "LEARN_INFO_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4, "LEARN_INFO_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT, "LEARN_OR_TRANSPLANT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA, "MIRROR_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS, "NWK_QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA, "OAM_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID, "OAM_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0, "GLOB_OUT_LIF_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1, "GLOB_OUT_LIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2, "GLOB_OUT_LIF_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3, "GLOB_OUT_LIF_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH, "OUT_LIF_PUSH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC, "PACKET_IS_COMPATIBLE_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER, "PACKET_IS_BIER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX, "EGRESS_PARSING_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0, "PEM_GENERAL_DATA0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1, "PEM_GENERAL_DATA1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2, "PEM_GENERAL_DATA2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3, "PEM_GENERAL_DATA3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED, "PPH_RESERVED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE, "PPH_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST, "RPF_DST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID, "RPF_DST_VALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF, "RPF_OUT_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PTC, "PTC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING, "STATISTICAL_SAMPLING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA, "SNOOP_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT, "SRC_SYS_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP, "STACKING_ROUTE_HISTORY_BITMAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0, "SLB_PAYLOAD0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1, "SLB_PAYLOAD1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0, "SLB_KEY0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1, "SLB_KEY1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2, "SLB_KEY2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND, "SLB_FOUND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA, "STATE_ADDRESS_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA, "STATE_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA, "STATISTICS_META_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0, "STATISTICS_ID_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1, "STATISTICS_ID_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2, "STATISTICS_ID_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3, "STATISTICS_ID_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4, "STATISTICS_ID_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5, "STATISTICS_ID_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6, "STATISTICS_ID_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7, "STATISTICS_ID_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8, "STATISTICS_ID_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9, "STATISTICS_ID_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0, "STATISTICS_ATR_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1, "STATISTICS_ATR_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2, "STATISTICS_ATR_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3, "STATISTICS_ATR_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4, "STATISTICS_ATR_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5, "STATISTICS_ATR_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6, "STATISTICS_ATR_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7, "STATISTICS_ATR_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8, "STATISTICS_ATR_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9, "STATISTICS_ATR_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10, "STATISTICS_OBJECT_10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR, "ST_VSQ_PTR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX, "SYSTEM_HEADER_PROFILE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_TC, "TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE, "TM_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, "USER_HEADER_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE, "USER_HEADER_1_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2, "USER_HEADER_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE, "USER_HEADER_2_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3, "USER_HEADER_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE, "USER_HEADER_3_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4, "USER_HEADER_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE, "USER_HEADER_4_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX, "VLAN_EDIT_CMD_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1, "VLAN_EDIT_PCP_DEI_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2, "VLAN_EDIT_PCP_DEI_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1, "VLAN_EDIT_VID_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2, "VLAN_EDIT_VID_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP, "INGRESS_TIME_STAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA, "INT_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE, "INGRESS_TIME_STAMP_OVERRIDE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR, "EVENTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX, "STAT_OBJ_LM_READ_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR, "VISIBILITY_CLEAR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_DUAL_QUEUE, "DUAL_QUEUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY, "VISIBILITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER, "PRT_QUALIFIER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_TUNNEL_PRIORITY, "TUNNEL_PRIORITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0, "LEARN_INFO_KEY_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1, "LEARN_INFO_KEY_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX, "LEARN_INFO_KEY_2_APP_DB_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0, "LEARN_INFO_PAYLOAD_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1, "LEARN_INFO_PAYLOAD_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX, "LEARN_INFO_FORMAT_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT, "INVALIDATE_NEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID, "INVALID");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    else if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
            enum_info[DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA].is_invalid_value_from_mapping = TRUE;
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPMF3_ACTION", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY, "EMPTY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT, "ACL_CONTEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE, "ADMT_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET, "BIER_STR_OFFSET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE, "BIER_STR_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE, "BYTES_TO_REMOVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO, "CONGESTION_INFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_DP, "DP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND, "DP_METER_COMMAND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EEI, "EEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE, "EGRESS_LEARN_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING, "END_OF_PACKET_EDITING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0, "EXT_STATISTICS_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1, "EXT_STATISTICS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2, "EXT_STATISTICS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3, "EXT_STATISTICS_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_FWD, "FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA, "DST_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN, "FWD_DOMAIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO, "FWD_LAYER_ADDITIONAL_INFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX, "FWD_LAYER_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0, "GENERAL_DATA_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1, "GENERAL_DATA_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2, "GENERAL_DATA_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3, "GENERAL_DATA_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA, "IEEE1588_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE, "INGRESS_LEARN_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP, "INGRESS_TIME_STAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE, "INGRESS_TIME_STAMP_OVERRIDE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0, "GLOB_IN_LIF_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0, "IN_LIF_PROFILE_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1, "GLOB_IN_LIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1, "IN_LIF_PROFILE_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT, "IN_PP_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA, "INT_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL, "IN_TTL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA, "ITPP_DELTA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY, "LAG_LB_KEY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW, "LATENCY_FLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0, "LEARN_INFO_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1, "LEARN_INFO_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2, "LEARN_INFO_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3, "LEARN_INFO_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4, "LEARN_INFO_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT, "LEARN_OR_TRANSPLANT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA, "MIRROR_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY, "NWK_LB_KEY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS, "NWK_QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE, "NWK_HEADER_APPEND_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA, "OAM_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID, "OAM_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0, "GLOB_OUT_LIF_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1, "GLOB_OUT_LIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2, "GLOB_OUT_LIF_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3, "GLOB_OUT_LIF_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET, "PACKET_IS_APPLET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER, "PACKET_IS_BIER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX, "EGRESS_PARSING_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED, "PPH_RESERVED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE, "PPH_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF, "PUSH_OUT_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA, "SNOOP_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT, "SRC_SYS_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP, "STACKING_ROUTE_HISTORY_BITMAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL, "STATISTICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA, "STATISTICS_META_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0, "STATISTICS_ID_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1, "STATISTICS_ID_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10, "STATISTICS_OBJECT_10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2, "STATISTICS_ID_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3, "STATISTICS_ID_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4, "STATISTICS_ID_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5, "STATISTICS_ID_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6, "STATISTICS_ID_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7, "STATISTICS_ID_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8, "STATISTICS_ID_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9, "STATISTICS_ID_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0, "STATISTICS_ATR_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1, "STATISTICS_ATR_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2, "STATISTICS_ATR_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3, "STATISTICS_ATR_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4, "STATISTICS_ATR_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5, "STATISTICS_ATR_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6, "STATISTICS_ATR_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7, "STATISTICS_ATR_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8, "STATISTICS_ATR_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9, "STATISTICS_ATR_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR, "ST_VSQ_PTR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX, "SYSTEM_HEADER_PROFILE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST, "SYSTEM_HEADER_SIZE_ADJUST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_TC, "TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE, "TM_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1, "USER_HEADER_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2, "USER_HEADER_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3, "USER_HEADER_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4, "USER_HEADER_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE, "USER_HEADERS_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY, "VISIBILITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX, "VLAN_EDIT_CMD_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1, "VLAN_EDIT_PCP_DEI_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2, "VLAN_EDIT_PCP_DEI_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1, "VLAN_EDIT_VID_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2, "VLAN_EDIT_VID_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET, "PARSING_START_OFFSET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE, "PARSING_START_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX, "STAT_OBJ_LM_READ_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE, "TM_ACTION_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_DUAL_QUEUE, "DUAL_QUEUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_TRUNCATE_SIZE, "NWK_HEADER_TRUNCATE_SIZE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0, "LEARN_INFO_KEY_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1, "LEARN_INFO_KEY_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX, "LEARN_INFO_KEY_2_APP_DB_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0, "LEARN_INFO_PAYLOAD_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1, "LEARN_INFO_PAYLOAD_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX, "LEARN_INFO_FORMAT_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT, "INVALIDATE_NEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID, "INVALID");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    else if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IPMF3_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EPMF_ACTION", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EPMF_ACTION_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY, "EMPTY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD, "DISCARD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE, "TC_MAP_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_TC, "TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_DP, "DP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR, "CUD_OUTLIF_OR_MCDB_PTR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP, "PP_DSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE, "COS_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE, "MIRROR_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE, "SNOOP_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH, "SNOOP_STRENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX, "FWD_ACTION_PROFILE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH, "FWD_STRENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH, "PMF_FWD_STRENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH, "PMF_SNOOP_STRENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE, "PMF_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE, "PMF_SNOOP_OVERRIDE_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE, "PMF_FWD_OVERRIDE_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE, "PMF_COUNTER_0_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR, "PMF_COUNTER_0_PTR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE, "PMF_COUNTER_1_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR, "PMF_COUNTER_1_PTR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP, "EBTR_EOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP, "EBTA_EOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP, "EBTA_SOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP, "EBTR_SOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT, "INVALIDATE_NEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EPMF_ACTION_INVALID, "INVALID");
    }
    if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EPMF_ACTION_EMPTY].is_invalid_value_from_mapping = TRUE;
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACE_ACTION", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ACE_ACTION_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_EMPTY, "EMPTY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP, "PP_DSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC, "FTMH_TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP, "FTMH_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE, "FTMH_TM_ACTION_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI, "FTMH_CNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE, "FTMH_ECN_ENABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY, "FTMH_VISIBILITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN, "PPH_FORWARD_DOMAIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS, "PPH_NWK_QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1, "PPH_VALUE1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2, "PPH_VALUE2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00, "FHEI_EXT31TO00");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32, "FHEI_EXT63TO32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL, "PPH_TTL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF, "PPH_IN_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE, "PPH_IN_LIF_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID, "CUD_OUT_LIF0_CUD_OUT_LIF0VALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID, "CUD_OUT_LIF1_CUD_OUT_LIF1VALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID, "CUD_OUT_LIF2_CUD_OUT_LIF2VALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID, "CUD_OUT_LIF3_CUD_OUT_LIF3VALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO, "PPH_FORWARDING_LAYER_ADDITIONAL_INFO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX, "PPH_FWD_LAYER_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH, "PPH_FORWANDING_STRENGTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING, "PPH_END_OF_PACKET_EDITING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00, "PPH_APPLICATION_SPECIFIC_EXT_31TO00");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32, "PPH_APPLICATION_SPECIFIC_EXT_47TO32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE, "SNIFF_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT, "FTMH_SOURCE_SYSTEM_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE, "ACE_CONTEXT_VALUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE, "ACE_STAMP_VALUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID, "ACE_STAT_METER_OBJ_CMD_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID, "ACE_STAT_OBJ_CMD_ID_VALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_ACE_FWD_ACTION_PROFILE_VALID_PROFILE, "ACE_FWD_ACTION_PROFILE_VALID_PROFILE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT, "INVALIDATE_NEXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ACE_ACTION_INVALID, "INVALID");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PMF_LOOKUP_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM, "EXEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT, "DIRECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0, "TCAM_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1, "TCAM_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2, "TCAM_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3, "TCAM_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE, "STATE_TABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_INVALID, "INVALID");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_INVALID].value_from_mapping = 7;
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TCAM_BANK_BLOCK_OWNER", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_ERPP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_PMF, "PMF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_FLP, "FLP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_VTT, "VTT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TCAM_BANK_BLOCK_OWNER_ERPP, "ERPP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_A_FES_KEY_SELECT", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0, "TCAM_0_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32, "TCAM_1_31_0_TCAM_0_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_1_63_0, "TCAM_1_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32, "TCAM_2_31_0_TCAM_1_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_63_0, "TCAM_2_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_31_0_TCAM_2_63_32, "TCAM_3_31_0_TCAM_2_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_3_63_0, "TCAM_3_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_31_0_TCAM_3_63_32, "TCAM_4_31_0_TCAM_3_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_4_63_0, "TCAM_4_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_31_0_TCAM_4_63_32, "TCAM_5_31_0_TCAM_4_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_5_63_0, "TCAM_5_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_31_0_TCAM_5_63_32, "TCAM_6_31_0_TCAM_5_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_6_63_0, "TCAM_6_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_31_0_TCAM_6_63_32, "TCAM_7_31_0_TCAM_6_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_7_63_0, "TCAM_7_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_27_0_4_B0_TCAM_7_63_32, "LEXEM_27_0_4_B0_TCAM_7_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_LEXEM_59_0_4_B0, "LEXEM_59_0_4_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_27_0_4_B0_LEXEM_59_28, "SEXEM_27_0_4_B0_LEXEM_59_28");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_SEXEM_59_0_4_B0, "SEXEM_59_0_4_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_31_0_SEXEM_59_28, "KEY_I_31_0_SEXEM_59_28");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_63_0, "KEY_I_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_95_32, "KEY_I_95_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_127_64, "KEY_I_127_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_I_159_96, "KEY_I_159_96");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_31_0_KEY_I_159_128, "KEY_J_31_0_KEY_I_159_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_63_0, "KEY_J_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_95_32, "KEY_J_95_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_127_64, "KEY_J_127_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_KEY_J_159_96, "KEY_J_159_96");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_23_0_8_B0_KEY_J_159_128, "DIRECT_0_23_0_8_B0_KEY_J_159_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_55_0_8_B0, "DIRECT_0_55_0_8_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_87_24, "DIRECT_0_87_24");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_0_119_56, "DIRECT_0_119_56");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_23_0_8_B0_DIRECT_0_119_88, "DIRECT_1_23_0_8_B0_DIRECT_0_119_88");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_55_0_8_B0, "DIRECT_1_55_0_8_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_87_24, "DIRECT_1_87_24");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_56, "DIRECT_1_119_56");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_DIRECT_1_119_88, "DIRECT_1_119_88");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0_DIRECT_1_119_88");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0_DIRECT_1_119_88");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_INVALID_FES_KEY_SELECT, "INVALID_FES_KEY_SELECT");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    else if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_B_FES_KEY_SELECT", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_119_56 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0, "TCAM_0_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_31_0_TCAM_0_63_32, "TCAM_1_31_0_TCAM_0_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_1_63_0, "TCAM_1_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32, "SEXEM_27_0_4_B0_TCAM_1_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_59_0_4_B0, "SEXEM_59_0_4_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28, "KEY_A_31_0_SEXEM_59_28");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_63_0, "KEY_A_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_95_32, "KEY_A_95_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_127_64, "KEY_A_127_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_159_96, "KEY_A_159_96");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_31_0_KEY_A_159_128, "KEY_B_31_0_KEY_A_159_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_63_0, "KEY_B_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_95_32, "KEY_B_95_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_127_64, "KEY_B_127_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_B_159_96, "KEY_B_159_96");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_31_0_KEY_B_159_128, "DIRECT_31_0_KEY_B_159_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_55_0_8_B0, "DIRECT_55_0_8_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_87_24, "DIRECT_87_24");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_DIRECT_119_56, "DIRECT_119_56");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_E_PMF_FES_KEY_SELECT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_15 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0, "LEXEM_4_B0_59_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32, "TCAM_0_PAYLOAD_31_0_4_B0_LEXEM_59_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_59_0_4_B0, "LEXEM_59_0_4_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_31_0_LEXEM_59_28, "TCAM_0_PAYLOAD_31_0_LEXEM_59_28");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0, "TCAM_0_PAYLOAD_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32, "TCAM_1_PAYLOAD_31_0_TCAM_0_PAYLOAD_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0, "TCAM_1_PAYLOAD_63_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_KEY_D_31_0_TCAM_1_PAYLOAD_63_32, "KEY_D_31_0_TCAM_1_PAYLOAD_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_6, "NOT_IN_USE_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_7, "NOT_IN_USE_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_8, "NOT_IN_USE_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_9, "NOT_IN_USE_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_10, "NOT_IN_USE_10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_11, "NOT_IN_USE_11");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_12, "NOT_IN_USE_12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_13, "NOT_IN_USE_13");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_14, "NOT_IN_USE_14");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_NOT_IN_USE_15, "NOT_IN_USE_15");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_A_FEM_KEY_SELECT", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_31_0, "TCAM_0_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_47_16, "TCAM_0_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_0_63_32, "TCAM_0_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_15_0_TCAM_0_63_48, "TCAM_1_15_0_TCAM_0_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_31_0, "TCAM_1_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_47_16, "TCAM_1_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_1_63_32, "TCAM_1_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_15_0_TCAM_1_63_48, "TCAM_2_15_0_TCAM_1_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_31_0, "TCAM_2_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_47_16, "TCAM_2_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_2_63_32, "TCAM_2_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_15_0_TCAM_2_63_48, "TCAM_3_15_0_TCAM_2_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_31_0, "TCAM_3_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_47_16, "TCAM_3_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_3_63_32, "TCAM_3_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_15_0_TCAM_3_63_48, "TCAM_4_15_0_TCAM_3_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_31_0, "TCAM_4_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_47_16, "TCAM_4_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_4_63_32, "TCAM_4_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_15_0_TCAM_4_63_48, "TCAM_5_15_0_TCAM_4_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_31_0, "TCAM_5_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_47_16, "TCAM_5_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_5_63_32, "TCAM_5_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_15_0_TCAM_5_63_48, "TCAM_6_15_0_TCAM_5_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_31_0, "TCAM_6_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_47_16, "TCAM_6_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_6_63_32, "TCAM_6_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_15_0_TCAM_6_63_48, "TCAM_7_15_0_TCAM_6_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_31_0, "TCAM_7_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_47_16, "TCAM_7_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_TCAM_7_63_32, "TCAM_7_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_11_0_4_B0_TCAM_7_63_48, "LEXEM_11_0_4_B0_TCAM_7_63_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_27_0_4_B0, "LEXEM_27_0_4_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_43_12, "LEXEM_43_12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_LEXEM_59_28, "LEXEM_59_28");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_11_0_4_B0_LEXEM_59_44, "SEXEM_11_0_4_B0_LEXEM_59_44");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0, "SEXEM_27_0_4_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_43_12, "SEXEM_43_12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_59_28, "SEXEM_59_28");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_16_0_SEXEM_59_44, "KEY_I_16_0_SEXEM_59_44");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_31_0, "KEY_I_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_47_16, "KEY_I_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_63_32, "KEY_I_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_79_48, "KEY_I_79_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_95_64, "KEY_I_95_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_111_80, "KEY_I_111_80");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_127_96, "KEY_I_127_96");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_143_112, "KEY_I_143_112");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_I_159_128, "KEY_I_159_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_15_0_KEY_I_159_144, "KEY_J_15_0_KEY_I_159_144");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_31_0, "KEY_J_31_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_47_16, "KEY_J_47_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_63_32, "KEY_J_63_32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_79_48, "KEY_J_79_48");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_95_64, "KEY_J_95_64");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_111_80, "KEY_J_111_80");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_127_96, "KEY_J_127_96");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_143_112, "KEY_J_143_112");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_KEY_J_159_128, "KEY_J_159_128");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_7_0_8_B0_KEY_J_159_144, "DIRECT_0_7_0_8_B0_KEY_J_159_144");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_23_0_8_B0, "DIRECT_0_23_0_8_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_39_8, "DIRECT_0_39_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_55_24, "DIRECT_0_55_24");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_71_40, "DIRECT_0_71_40");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_87_56, "DIRECT_0_87_56");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_103_72, "DIRECT_0_103_72");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_0_119_88, "DIRECT_0_119_88");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_7_0_8_B0_DIRECT_0_119_104, "DIRECT_1_7_0_8_B0_DIRECT_0_119_104");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_23_0_8_B0, "DIRECT_1_23_0_8_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_39_8, "DIRECT_1_39_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_55_24, "DIRECT_1_55_24");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_71_40, "DIRECT_1_71_40");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_87_56, "DIRECT_1_87_56");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_103_72, "DIRECT_1_103_72");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_DIRECT_1_119_88, "DIRECT_1_119_88");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104, "STATE_TABLE_ADDRESS_1_0_14_B0_DIRECT_1_119_104");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_9_0_14_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104, "STATE_TABLE_ADDRESS_4_0_11_B0_DIRECT_1_119_104");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0, "STATE_TABLE_DATA_7_0_STATE_TABLE_ADDRESS_12_0_11_B0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT, "INVALID_FEM_KEY_SELECT");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    else if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_FES_DATA_2MSB", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_INVALID_BITS", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_SHIFT", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_POLARITY", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_CHOSEN_MASK", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_CTX_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_ACE_RESULT_TYPE", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "APP_DB_ID", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_CTX_ID_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_NAS_ID", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_NAS_ID_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_ID", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_MASK_ID", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_ACTION_MASK", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FES_PGM_ID", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_ACTION_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE, "FEM_ACTIONS_IS_NOT_ACTIVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE, "FEM_ACTIONS_IS_ACTIVE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_PGM_ID", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_REPLACE_LSB_FLAG", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE, "REPLACE_LSB_IS_NOT_ACTIVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE, "REPLACE_LSB_IS_ACTIVE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_REPLACE_LSB_SELECT", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_7 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0, "TCAM_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_1, "TCAM_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_2, "TCAM_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_3, "TCAM_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_4, "TCAM_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_5, "TCAM_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_6, "TCAM_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_7, "TCAM_7");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_CONDITION_MS_BIT", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_SELECTED_BIT", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_MAP_INDEX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_ID", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_CONDITION", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_FEM_MAP_DATA", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CONTEXT_HASH_FUNCTION", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED, "FIRST_RESERVED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED, "SECOND_RESERVED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED, "THIRD_RESERVED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC, "CRC16_BISYNC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1, "CRC16_XOR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2, "CRC16_XOR2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4, "CRC16_XOR4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8, "CRC16_XOR8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16, "XOR16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT, "CRC16_CCITT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW, "CRC32_ALOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH, "CRC32_AHIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW, "CRC32_BLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH, "CRC32_BHIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_INVALID, "INVALID");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
        enum_info[DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_INVALID].is_invalid_value_from_mapping = TRUE;
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FIELD_PMF_RANGE_ID", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACE_CONTEXT_VALUE", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 7;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ETPP_FWD_CODE_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ETPP_PRP2_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ETPP_TERM_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_ETPP_FWD_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_ETPP_ENCAP_1_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_ETPP_ENCAP_2_ACE_CTXT_VALUE;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_ETPP_TRAP_ACE_CTXT_VALUE;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_RANGE_ID", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_ENCODE_IDX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_FFC_IDX", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_EXTND_RANGES_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_PACKET_HEADER_SIZE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_DST_PORT, "L4_DST_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_L4_SRC_PORT, "L4_SRC_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_IN_TLL, "IN_TLL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_LOW, "FFC_1_LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_1_HIGH, "FFC_1_HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_LOW, "FFC_2_LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_FFC_2_HIGH, "FFC_2_HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_TYPE_PACKET_HEADER_SIZE, "PACKET_HEADER_SIZE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_EXTND_RANGES_OP_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_OR + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_AND, "AND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_TYPE_OR, "OR");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_EXTND_RANGES_OP_WIDTH", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_64 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_32, "32");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_WIDTH_64, "64");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_EXTND_RANGES_OP_PAIR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_BOTH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_FIRST, "FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_SECOND, "SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_OP_PAIR_BOTH, "BOTH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L4_OPS_EXTND_RANGES_ENCODE_SELECT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD_FORTH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST, "FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND, "SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD, "THIRD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FORTH, "FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND, "FIRST_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD, "FIRST_THIRD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_FORTH, "FIRST_FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD, "SECOND_THIRD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_FORTH, "SECOND_FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_THIRD_FORTH, "THIRD_FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD, "FIRST_SECOND_THIRD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_FORTH, "FIRST_SECOND_FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_THIRD_FORTH, "FIRST_THIRD_FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_SECOND_THIRD_FORTH, "SECOND_THIRD_FORTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L4_OPS_EXTND_RANGES_ENCODE_SELECT_FIRST_SECOND_THIRD_FORTH, "FIRST_SECOND_THIRD_FORTH");
    }
    if (DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pmf_layer_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_context_key_gen_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ptc_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_out_tm_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_in_port_pmf_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_native_ac_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ac_in_lif_wide_data_extended_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_pwe_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_field_types_definition_ip_tunnel_in_lif_wide_data_init(unit, cur_field_type_param, field_types_info));
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
