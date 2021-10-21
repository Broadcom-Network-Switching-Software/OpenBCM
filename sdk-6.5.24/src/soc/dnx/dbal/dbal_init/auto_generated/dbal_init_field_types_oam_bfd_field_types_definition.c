
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_dip_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_DIP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_DIP_PROFILE", dnx_data_bfd.general.nof_bits_dips_for_bfd_multihop_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_server_trap_code_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_SERVER_TRAP_CODE_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_SERVER_TRAP_CODE_INDEX", dnx_data_bfd.general.nof_bits_bfd_server_trap_codes_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_mep_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_MEP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_MEP_PROFILE", dnx_data_oam.general.oam_nof_bits_acc_action_profiles_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oamp_entry_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_ENTRY_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_ENTRY_ID", 17, is_valid, TRUE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oamp_mep_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_MEP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_MEP_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM, "ETH_OAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP, "Y1731_MPLSTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE, "Y1731_PWE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP, "BFD_IPV4_1HOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP, "BFD_IPV4_MHOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS, "BFD_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE, "BFD_PWE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374, "RFC_6374");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB, "LM_DB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT, "LM_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT, "DM_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR, "EXT_DATA_HDR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD, "EXT_DATA_PLD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY, "DM_STAT_ONE_WAY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY, "OFFLOADED_SECOND_ENTRY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_tx_rate_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_TX_RATE_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_TX_RATE_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_itmh_tc_dp_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ITMH_TC_DP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ITMH_TC_DP_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_your_disc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_YOUR_DISC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "YOUR_DISC", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_my_disc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MY_DISC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MY_DISC", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_src_ip_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_SRC_IP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_SRC_IP_PROFILE", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ip_ttl_tos_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IP_TTL_TOS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IP_TTL_TOS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_flags_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_FLAGS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_FLAGS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_state_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_STATE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_STATE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_detect_mult_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DETECT_MULT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DETECT_MULT", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_min_interval_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIN_INTERVAL_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIN_INTERVAL_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_fec_id_or_glob_out_lif_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_ID_OR_GLOB_OUT_LIF];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_ID_OR_GLOB_OUT_LIF", 23, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    cur_field_type_param->nof_childs = 2;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "FEC");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "GLOB_OUT_LIF");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 4;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[1].encode_param1 = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ivec_address_factor_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IVEC_ADDRESS_FACTOR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IVEC_ADDRESS_FACTOR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_diag_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_DIAG_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_DIAG_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mep_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_PROFILE", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mep_pe_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_PE_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_PE_PROFILE", dnx_data_oam.oamp.mep_pe_profile_nof_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mep_db_bank_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_DB_BANK];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_DB_BANK", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = 11;
    cur_field_type_param->min_value = 0;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_slm_measurement_period_timer_profile_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SLM_MEASUREMENT_PERIOD_TIMER_PROFILE_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SLM_MEASUREMENT_PERIOD_TIMER_PROFILE_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = 7;
    cur_field_type_param->min_value = 0;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ip_subnet_len_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IP_SUBNET_LEN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IP_SUBNET_LEN", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ccm_interval_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CCM_INTERVAL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CCM_INTERVAL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CCM_INTERVAL_10M + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX, "NO_TX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_3MS, "3MS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_10MS, "10MS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_100MS, "100MS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_1S, "1S");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_10S, "10S");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_1M, "1M");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CCM_INTERVAL_10M, "10M");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_3MS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_10MS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_100MS].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_1S].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_10S].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_1M].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_CCM_INTERVAL_10M].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_maid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MAID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MAID", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_endpoint_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_ENDPOINT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_ENDPOINT_ID", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_endpoint_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_ENDPOINT_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_ENDPOINT_ID", 25, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_short_icc_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SHORT_ICC_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SHORT_ICC_INDEX", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    sal_memset(cur_field_type_param->define, 0x0, 1 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "SHORT_FORMAT", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 15;
    cur_field_type_param->nof_defined_vals = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_nof_vlan_tags_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NOF_VLAN_TAGS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NOF_VLAN_TAGS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_up_ptch_opaque_pt_attr_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_UP_PTCH_OPAQUE_PT_ATTR_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "UP_PTCH_OPAQUE_PT_ATTR_PROFILE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_port_status_tlv_en_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_EN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PORT_STATUS_TLV_EN", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_port_status_tlv_val_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV_VAL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PORT_STATUS_TLV_VAL", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_interface_status_tlv_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INTERFACE_STATUS_TLV_CODE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lmm_da_lsb_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LMM_DA_LSB_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LMM_DA_LSB_PROFILE", 7, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lmm_da_msb_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LMM_DA_MSB_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LMM_DA_MSB_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_sa_gen_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SA_GEN_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SA_GEN_PROFILE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_da_uc_mc_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DA_UC_MC_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DA_UC_MC_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DA_UC_MC_TYPE_MC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DA_UC_MC_TYPE_UC, "UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DA_UC_MC_TYPE_MC, "MC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_DA_UC_MC_TYPE_UC].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_DA_UC_MC_TYPE_MC].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_delay_dm_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DELAY_DM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DELAY_DM", 35, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY8, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_last_entry_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LAST_ENTRY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LAST_ENTRY", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ext_data_length_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXT_DATA_LENGTH];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXT_DATA_LENGTH", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_opcodes_to_prepend_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OPCODES_TO_PREPEND];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OPCODES_TO_PREPEND", 14, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_crc16_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CRC16];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CRC16", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_extra_data_segment_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXTRA_DATA_SEGMENT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXTRA_DATA_SEGMENT", 160, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY8, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_my_tx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_MY_TX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_MY_TX", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_my_rx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_MY_RX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_MY_RX", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_peer_tx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_PEER_TX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_PEER_TX", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_peer_rx_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_PEER_RX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_PEER_RX", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_cntrs_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_CNTRS_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_CNTRS_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_last_lm_far_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LAST_LM_FAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LAST_LM_FAR", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_last_lm_near_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LAST_LM_NEAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LAST_LM_NEAR", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_acc_lm_far_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACC_LM_FAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACC_LM_FAR", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_acc_lm_near_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACC_LM_NEAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACC_LM_NEAR", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_max_lm_far_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MAX_LM_FAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MAX_LM_FAR", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_max_lm_near_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MAX_LM_NEAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MAX_LM_NEAR", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ach_sel_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACH_SEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACH_SEL", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_measure_next_received_slr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEASURE_NEXT_RECEIVED_SLR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEASURE_NEXT_RECEIVED_SLR", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_timestamp_format_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TIMESTAMP_FORMAT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TIMESTAMP_FORMAT", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TIMESTAMP_FORMAT_TS_1588 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TIMESTAMP_FORMAT_TS_NTP, "TS_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TIMESTAMP_FORMAT_TS_1588, "TS_1588");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_TIMESTAMP_FORMAT_TS_NTP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_TIMESTAMP_FORMAT_TS_1588].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oamp_error_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_ERROR_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_ERROR_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_RFC_PUNT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_PACKET, "ERR_PACKET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_CHAN_TYPE_MISS, "CHAN_TYPE_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TYPE_MISS, "TYPE_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_EMC_MISS, "EMC_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_MAID_MISS, "MAID_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_FLEX_CRC_MISS, "FLEX_CRC_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_MDL_MISS, "MDL_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_CCM_INTRVL_MISS, "CCM_INTRVL_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_MY_DISC_MISS, "ERR_MY_DISC_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_SRC_IP_MISS, "ERR_SRC_IP_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_YOUR_DISC_MISS, "ERR_YOUR_DISC_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_SRC_PORT_MISS, "SRC_PORT_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_RMEP_STATE_CHANGE, "RMEP_STATE_CHANGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_PARITY_ERROR, "PARITY_ERROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_DEBUG, "DEBUG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TIMESTAMP_MISS, "TIMESTAMP_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TRAP_CODE_MISS, "TRAP_CODE_MISS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_RFC_PUNT, "RFC_PUNT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_PACKET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_CHAN_TYPE_MISS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TYPE_MISS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_EMC_MISS].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_MAID_MISS].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_FLEX_CRC_MISS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_MDL_MISS].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_CCM_INTRVL_MISS].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_MY_DISC_MISS].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_SRC_IP_MISS].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_ERR_YOUR_DISC_MISS].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_SRC_PORT_MISS].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_RMEP_STATE_CHANGE].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_PARITY_ERROR].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_DEBUG].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TIMESTAMP_MISS].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_TRAP_CODE_MISS].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_OAMP_ERROR_TYPE_RFC_PUNT].value_from_mapping = 18;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_session_identifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SESSION_IDENTIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SESSION_IDENTIFIER", 26, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_dm_t_flag_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DM_T_FLAG];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DM_T_FLAG", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_dm_ds_field_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DM_DS_FIELD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DM_DS_FIELD", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_cw_choose_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_CW_CHOOSE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_CW_CHOOSE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_full_delay_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FULL_DELAY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FULL_DELAY", 42, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY8, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_port_core_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PORT_CORE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PORT_CORE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_report_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_REPORT_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "REPORT_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_REPORT_MODE_RAW + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_REPORT_MODE_NORMAL, "NORMAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_REPORT_MODE_COMPACT, "COMPACT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_REPORT_MODE_RAW, "RAW");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_REPORT_MODE_NORMAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_REPORT_MODE_COMPACT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_REPORT_MODE_RAW].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_opcode_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OPCODE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OPCODE_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OPCODE_TYPE_AIS_1_FRAME_PER_MIN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_TYPE_AIS_1_FRAME_PER_SEC, "AIS_1_FRAME_PER_SEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_TYPE_AIS_1_FRAME_PER_MIN, "AIS_1_FRAME_PER_MIN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OPCODE_TYPE_AIS_1_FRAME_PER_SEC].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OPCODE_TYPE_AIS_1_FRAME_PER_MIN].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_mac_da_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_MAC_DA_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_MAC_DA_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_MAC_DA_TYPE_CCM_TYPE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_MAC_DA_TYPE_DMM_LMM_TYPE, "DMM_LMM_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_MAC_DA_TYPE_CCM_TYPE, "CCM_TYPE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_MAC_DA_TYPE_DMM_LMM_TYPE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_MAC_DA_TYPE_CCM_TYPE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_endpoint_sw_state_flags_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_ENDPOINT_SW_STATE_FLAGS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_ENDPOINT_SW_STATE_FLAGS", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    sal_memset(cur_field_type_param->define, 0x0, 22 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "IS_ACCELERATED", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 1;
    sal_strncpy(cur_field_type_param->define[1].define_name, "USE_OAMB_PROFILE", sizeof(cur_field_type_param->define[1].define_name));
    cur_field_type_param->define[1].define_value = 2;
    sal_strncpy(cur_field_type_param->define[2].define_name, "BFD_IS_IPV6", sizeof(cur_field_type_param->define[2].define_name));
    cur_field_type_param->define[2].define_value = 4;
    sal_strncpy(cur_field_type_param->define[3].define_name, "MULTI_HOP_BFD", sizeof(cur_field_type_param->define[3].define_name));
    cur_field_type_param->define[3].define_value = 8;
    sal_strncpy(cur_field_type_param->define[4].define_name, "EXPLICIT_DETECTION_TIME", sizeof(cur_field_type_param->define[4].define_name));
    cur_field_type_param->define[4].define_value = 16;
    sal_strncpy(cur_field_type_param->define[5].define_name, "BFD_ECHO", sizeof(cur_field_type_param->define[5].define_name));
    cur_field_type_param->define[5].define_value = 32;
    sal_strncpy(cur_field_type_param->define[6].define_name, "USE_MY_BFD_DIP_DESTINATION", sizeof(cur_field_type_param->define[6].define_name));
    cur_field_type_param->define[6].define_value = 64;
    sal_strncpy(cur_field_type_param->define[7].define_name, "LOCAL_DISCR_IS_NULL", sizeof(cur_field_type_param->define[7].define_name));
    cur_field_type_param->define[7].define_value = 128;
    sal_strncpy(cur_field_type_param->define[8].define_name, "FROM_OAMP", sizeof(cur_field_type_param->define[8].define_name));
    cur_field_type_param->define[8].define_value = 256;
    sal_strncpy(cur_field_type_param->define[9].define_name, "TX_STATISTICS", sizeof(cur_field_type_param->define[9].define_name));
    cur_field_type_param->define[9].define_value = 512;
    sal_strncpy(cur_field_type_param->define[10].define_name, "RX_STATISTICS", sizeof(cur_field_type_param->define[10].define_name));
    cur_field_type_param->define[10].define_value = 1024;
    sal_strncpy(cur_field_type_param->define[11].define_name, "PER_OPCODE_STATISTICS", sizeof(cur_field_type_param->define[11].define_name));
    cur_field_type_param->define[11].define_value = 2048;
    sal_strncpy(cur_field_type_param->define[12].define_name, "LOCAL_DISCR_MSB", sizeof(cur_field_type_param->define[12].define_name));
    cur_field_type_param->define[12].define_value = 4096;
    sal_strncpy(cur_field_type_param->define[13].define_name, "BFD_SERVER", sizeof(cur_field_type_param->define[13].define_name));
    cur_field_type_param->define[13].define_value = 8192;
    sal_strncpy(cur_field_type_param->define[14].define_name, "BFD_SERVER_CLIENT", sizeof(cur_field_type_param->define[14].define_name));
    cur_field_type_param->define[14].define_value = 16384;
    sal_strncpy(cur_field_type_param->define[15].define_name, "TX_GPORT_IS_TRUNK", sizeof(cur_field_type_param->define[15].define_name));
    cur_field_type_param->define[15].define_value = 32768;
    sal_strncpy(cur_field_type_param->define[16].define_name, "TX_GPORT_IS_INVALID", sizeof(cur_field_type_param->define[16].define_name));
    cur_field_type_param->define[16].define_value = 65536;
    sal_strncpy(cur_field_type_param->define[17].define_name, "EGRESS_IF_IS_NULL", sizeof(cur_field_type_param->define[17].define_name));
    cur_field_type_param->define[17].define_value = 131072;
    sal_strncpy(cur_field_type_param->define[18].define_name, "SEAMLESS_BFD_INITIATOR", sizeof(cur_field_type_param->define[18].define_name));
    cur_field_type_param->define[18].define_value = 262144;
    sal_strncpy(cur_field_type_param->define[19].define_name, "SEAMLESS_BFD_REFLECTOR", sizeof(cur_field_type_param->define[19].define_name));
    cur_field_type_param->define[19].define_value = 524288;
    sal_strncpy(cur_field_type_param->define[20].define_name, "SINGLE_HOP_RANDOM_DIP", sizeof(cur_field_type_param->define[20].define_name));
    cur_field_type_param->define[20].define_value = 1048576;
    sal_strncpy(cur_field_type_param->define[21].define_name, "BFD_VCCV_TYPE", sizeof(cur_field_type_param->define[21].define_name));
    cur_field_type_param->define[21].define_value = 2097152;
    cur_field_type_param->nof_defined_vals = 22;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_counter_base_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_COUNTER_BASE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "COUNTER_BASE", dnx_data_oam.general.oam_nof_bits_counter_base_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_counter_if_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_COUNTER_IF];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "COUNTER_IF", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_rmep_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RMEP_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RMEP_INDEX", dnx_data_oam.oamp.rmep_db_rmep_index_nof_bits_get(unit), is_valid, TRUE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lifetime_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIFETIME];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIFETIME", 10, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lifetime_units_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIFETIME_UNITS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIFETIME_UNITS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_10000 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1, "SCAN_PERIOD_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_100, "SCAN_PERIOD_100");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_10000, "SCAN_PERIOD_10000");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_100].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_10000].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_loc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LOC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LOC", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_loc_clear_limit_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LOC_CLEAR_LIMIT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LOC_CLEAR_LIMIT", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_loc_clear_enable_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LOC_CLEAR_ENABLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LOC_CLEAR_ENABLE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_punt_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PUNT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PUNT_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_punt_good_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PUNT_GOOD_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PUNT_GOOD_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_punt_next_good_packet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PUNT_NEXT_GOOD_PACKET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PUNT_NEXT_GOOD_PACKET", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_rdi_received_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RDI_RECEIVED];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RDI_RECEIVED", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_eth_port_state_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETH_PORT_STATE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETH_PORT_STATE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_eth_interface_state_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETH_INTERFACE_STATE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETH_INTERFACE_STATE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_packet_is_oam_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PACKET_IS_OAM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PACKET_IS_OAM", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_counter_ptr_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_COUNTER_PTR_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "COUNTER_PTR_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mp_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MP_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_lm_counter_read_idx_sel_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_LM_COUNTER_READ_IDX_SEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_LM_COUNTER_READ_IDX_SEL", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_2 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_0, "COUNTER_IF_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_1, "COUNTER_IF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_2, "COUNTER_IF_2");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_LM_COUNTER_READ_IDX_SEL_COUNTER_IF_2].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_counter_stamp_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_COUNTER_STAMP_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_COUNTER_STAMP_OFFSET", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_ID", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_stamp_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_STAMP_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_STAMP_OFFSET", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_OFFSET", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_your_discr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_YOUR_DISCR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_YOUR_DISCR", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_up_mep_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_UP_MEP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_UP_MEP", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_meter_disable_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_METER_DISABLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_METER_DISABLE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mep_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MEP_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MEP_ID", 13, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_key_base_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_KEY_BASE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_KEY_BASE", 22, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_set_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_LIF_SET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_LIF_SET", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_prefix_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_LIF_PREFIX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_LIF_PREFIX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_LIF];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_LIF", 22, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_endpoint_sw_state_flags_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_ENDPOINT_SW_STATE_FLAGS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_ENDPOINT_SW_STATE_FLAGS", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    sal_memset(cur_field_type_param->define, 0x0, 17 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "UP_FACING", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 1;
    sal_strncpy(cur_field_type_param->define[1].define_name, "INTERMEDIATE", sizeof(cur_field_type_param->define[1].define_name));
    cur_field_type_param->define[1].define_value = 2;
    sal_strncpy(cur_field_type_param->define[2].define_name, "REMOTE", sizeof(cur_field_type_param->define[2].define_name));
    cur_field_type_param->define[2].define_value = 4;
    sal_strncpy(cur_field_type_param->define[3].define_name, "ACCELERATED", sizeof(cur_field_type_param->define[3].define_name));
    cur_field_type_param->define[3].define_value = 8;
    sal_strncpy(cur_field_type_param->define[4].define_name, "PCP_BASED", sizeof(cur_field_type_param->define[4].define_name));
    cur_field_type_param->define[4].define_value = 16;
    sal_strncpy(cur_field_type_param->define[5].define_name, "TX_STATISTICS", sizeof(cur_field_type_param->define[5].define_name));
    cur_field_type_param->define[5].define_value = 32;
    sal_strncpy(cur_field_type_param->define[6].define_name, "RX_STATISTICS", sizeof(cur_field_type_param->define[6].define_name));
    cur_field_type_param->define[6].define_value = 64;
    sal_strncpy(cur_field_type_param->define[7].define_name, "PER_OPCODE_STATISTICS", sizeof(cur_field_type_param->define[7].define_name));
    cur_field_type_param->define[7].define_value = 128;
    sal_strncpy(cur_field_type_param->define[8].define_name, "DOWN_EGRESS_INJECTION", sizeof(cur_field_type_param->define[8].define_name));
    cur_field_type_param->define[8].define_value = 256;
    sal_strncpy(cur_field_type_param->define[9].define_name, "TX_GPORT_IS_TRUNK", sizeof(cur_field_type_param->define[9].define_name));
    cur_field_type_param->define[9].define_value = 512;
    sal_strncpy(cur_field_type_param->define[10].define_name, "SIGNAL_DETECT", sizeof(cur_field_type_param->define[10].define_name));
    cur_field_type_param->define[10].define_value = 1024;
    sal_strncpy(cur_field_type_param->define[11].define_name, "JUMBO_DM_TLV", sizeof(cur_field_type_param->define[11].define_name));
    cur_field_type_param->define[11].define_value = 2048;
    sal_strncpy(cur_field_type_param->define[12].define_name, "ADDITIONAL_GAL_SPECIAL_LABEL", sizeof(cur_field_type_param->define[12].define_name));
    cur_field_type_param->define[12].define_value = 4096;
    sal_strncpy(cur_field_type_param->define[13].define_name, "DELAY_ONE_WAY", sizeof(cur_field_type_param->define[13].define_name));
    cur_field_type_param->define[13].define_value = 8192;
    sal_strncpy(cur_field_type_param->define[14].define_name, "DELAY_ONE_WAY_STATS", sizeof(cur_field_type_param->define[14].define_name));
    cur_field_type_param->define[14].define_value = 16384;
    sal_strncpy(cur_field_type_param->define[15].define_name, "DOWN_EGRESS_INJECTION_IVEC_DELETE_ONE_VLAN_TAG", sizeof(cur_field_type_param->define[15].define_name));
    cur_field_type_param->define[15].define_value = 32768;
    sal_strncpy(cur_field_type_param->define[16].define_name, "DOWN_EGRESS_INJECTION_IVEC_DELETE_TWO_VLAN_TAGS", sizeof(cur_field_type_param->define[16].define_name));
    cur_field_type_param->define[16].define_value = 65536;
    cur_field_type_param->nof_defined_vals = 17;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_counter_disable_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_COUNTER_DISABLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_COUNTER_DISABLE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_access_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACCESS_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACCESS_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_opcode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_OPCODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_OPCODE", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    sal_memset(cur_field_type_param->define, 0x0, 18 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "BFD_OR_OAM_DATA", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 0;
    sal_strncpy(cur_field_type_param->define[1].define_name, "CCM", sizeof(cur_field_type_param->define[1].define_name));
    cur_field_type_param->define[1].define_value = 1;
    sal_strncpy(cur_field_type_param->define[2].define_name, "LBR", sizeof(cur_field_type_param->define[2].define_name));
    cur_field_type_param->define[2].define_value = 2;
    sal_strncpy(cur_field_type_param->define[3].define_name, "LBM", sizeof(cur_field_type_param->define[3].define_name));
    cur_field_type_param->define[3].define_value = 3;
    sal_strncpy(cur_field_type_param->define[4].define_name, "LTR", sizeof(cur_field_type_param->define[4].define_name));
    cur_field_type_param->define[4].define_value = 4;
    sal_strncpy(cur_field_type_param->define[5].define_name, "LTM", sizeof(cur_field_type_param->define[5].define_name));
    cur_field_type_param->define[5].define_value = 5;
    sal_strncpy(cur_field_type_param->define[6].define_name, "AIS", sizeof(cur_field_type_param->define[6].define_name));
    cur_field_type_param->define[6].define_value = 33;
    sal_strncpy(cur_field_type_param->define[7].define_name, "LCK", sizeof(cur_field_type_param->define[7].define_name));
    cur_field_type_param->define[7].define_value = 35;
    sal_strncpy(cur_field_type_param->define[8].define_name, "LINEAR_APS", sizeof(cur_field_type_param->define[8].define_name));
    cur_field_type_param->define[8].define_value = 39;
    sal_strncpy(cur_field_type_param->define[9].define_name, "LMR", sizeof(cur_field_type_param->define[9].define_name));
    cur_field_type_param->define[9].define_value = 42;
    sal_strncpy(cur_field_type_param->define[10].define_name, "LMM", sizeof(cur_field_type_param->define[10].define_name));
    cur_field_type_param->define[10].define_value = 43;
    sal_strncpy(cur_field_type_param->define[11].define_name, "1DM", sizeof(cur_field_type_param->define[11].define_name));
    cur_field_type_param->define[11].define_value = 45;
    sal_strncpy(cur_field_type_param->define[12].define_name, "DMR", sizeof(cur_field_type_param->define[12].define_name));
    cur_field_type_param->define[12].define_value = 46;
    sal_strncpy(cur_field_type_param->define[13].define_name, "DMM", sizeof(cur_field_type_param->define[13].define_name));
    cur_field_type_param->define[13].define_value = 47;
    sal_strncpy(cur_field_type_param->define[14].define_name, "EXR", sizeof(cur_field_type_param->define[14].define_name));
    cur_field_type_param->define[14].define_value = 48;
    sal_strncpy(cur_field_type_param->define[15].define_name, "EXM", sizeof(cur_field_type_param->define[15].define_name));
    cur_field_type_param->define[15].define_value = 49;
    sal_strncpy(cur_field_type_param->define[16].define_name, "SLR", sizeof(cur_field_type_param->define[16].define_name));
    cur_field_type_param->define[16].define_value = 54;
    sal_strncpy(cur_field_type_param->define[17].define_name, "SLM", sizeof(cur_field_type_param->define[17].define_name));
    cur_field_type_param->define[17].define_value = 55;
    cur_field_type_param->nof_defined_vals = 18;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_opcode_for_count_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OPCODE_FOR_COUNT_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OPCODE_FOR_COUNT_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_OTHER_OPCODE_INDEX + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_CCM_OPCODE_INDEX, "CCM_OPCODE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_LMM_OPCODE_INDEX, "LMM_OPCODE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_LMR_OPCODE_INDEX, "LMR_OPCODE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_DMM_OPCODE_INDEX, "DMM_OPCODE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_DMR_OPCODE_INDEX, "DMR_OPCODE_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_OTHER_OPCODE_INDEX, "OTHER_OPCODE_INDEX");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_CCM_OPCODE_INDEX].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_LMM_OPCODE_INDEX].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_LMR_OPCODE_INDEX].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_DMM_OPCODE_INDEX].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_DMR_OPCODE_INDEX].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OPCODE_FOR_COUNT_INDEX_OTHER_OPCODE_INDEX].value_from_mapping = 5;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mip_above_mdl_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIP_ABOVE_MDL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIP_ABOVE_MDL", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_packet_is_bfd_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PACKET_IS_BFD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PACKET_IS_BFD", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mdl_mp_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MDL_MP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MDL_MP_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP, "NO_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP, "MIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP, "ACTIVE_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP, "PASSIVE_MEP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_ingress_oam_data_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INGRESS_OAM_DATA_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INGRESS_OAM_DATA_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_COUNTER_VALUE, "COUNTER_VALUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_1588, "TOD_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_NTP, "TOD_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_COUNTER_VALUE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_1588].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_NTP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_NONE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_egress_oam_data_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_OAM_DATA_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_OAM_DATA_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_NTP, "TOD_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_1588, "TOD_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_COUNTER_VALUE, "COUNTER_VALUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_NTP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_1588].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_COUNTER_VALUE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_NONE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_nof_mep_below_mdl_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NOF_MEP_BELOW_MDL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NOF_MEP_BELOW_MDL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_db_access_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_LIF_DB_ACCESS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_LIF_DB_ACCESS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_nof_mep_above_mdl_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NOF_MEP_ABOVE_MDL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NOF_MEP_ABOVE_MDL", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_inject_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_INJECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_INJECT", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_my_cfm_mac_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_MY_CFM_MAC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_MY_CFM_MAC", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_da_is_mc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_DA_IS_MC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_DA_IS_MC", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_ingress_up_mep_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_INGRESS_UP_MEP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_INGRESS_UP_MEP", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_ingress_meter_disable_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_INGRESS_METER_DISABLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_INGRESS_METER_DISABLE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_direction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_DIRECTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_DIRECTION", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, "DOWN_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP, "UP_MEP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_process_action_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_PROCESS_ACTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_PROCESS_ACTION", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_1588, "INGRESS_STAMP_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_1588, "EGRESS_STAMP_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_NTP, "INGRESS_STAMP_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP, "EGRESS_STAMP_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_STAMP_COUNTER, "STAMP_COUNTER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP, "READ_COUNTER_AND_STAMP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_1588].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_1588].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_NTP].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_STAMP_COUNTER].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mp_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MP_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MP_TYPE_BELOW_MIP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP, "BELOW_LOWER_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW, "ACTIVE_MATCH_LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW, "PASSIVE_MATCH_LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW, "BETWEEN_MIDDLE_AND_LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE, "ACTIVE_MATCH_MIDDLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE, "PASSIVE_MATCH_MIDDLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE, "BETWEEN_HIGH_AND_MIDDLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH, "ACTIVE_MATCH_HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH, "PASSIVE_MATCH_HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP, "ABOVE_UPPER_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH, "MIP_MATCH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_BFD, "BFD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP, "BETWEEN_MIP_AND_MEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER, "ACTIVE_MATCH_NO_COUNTER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER, "PASSIVE_MATCH_NO_COUNTER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MP_TYPE_BELOW_MIP, "BELOW_MIP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_BFD].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_MP_TYPE_BELOW_MIP].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_internal_opcode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_INTERNAL_OPCODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_INTERNAL_OPCODE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_BFD, "ETH_BFD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM, "ETH_CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBR, "ETH_LBR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM, "ETH_LBM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTR, "ETH_LTR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTM, "ETH_LTM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR, "ETH_LMR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM, "ETH_LMM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR, "ETH_DMR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM, "ETH_DMM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM, "ETH_1DM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM, "ETH_SLM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR, "ETH_SLR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_AIS, "ETH_AIS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LINEAR_APS, "ETH_LINEAR_APS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER, "ETH_OTHER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_BFD].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBR].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTR].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTM].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_AIS].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LINEAR_APS].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_ing_key_base_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_ING_KEY_BASE_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_ING_KEY_BASE_SELECT", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF, "OAM_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_3, "OAM_LIF_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_2, "OAM_LIF_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_1, "OAM_LIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED1, "RESERVED1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED2, "RESERVED2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED3, "RESERVED3");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_3].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_2].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_1].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED2].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_RESERVED3].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_eg_key_base_select_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_EG_KEY_BASE_SELECT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_EG_KEY_BASE_SELECT", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_1, "OAM_LIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_2, "OAM_LIF_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_3, "OAM_LIF_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF, "OAM_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED1, "RESERVED1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED2, "RESERVED2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED3, "RESERVED3");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_3].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED2].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_RESERVED3].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_key_prefix_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_KEY_PREFIX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_KEY_PREFIX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH, "HIGH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_MID, "MID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_LOW, "LOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED1, "RESERVED1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED2, "RESERVED2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED3, "RESERVED3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED4, "RESERVED4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD, "BFD");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_MID].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_LOW].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED3].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_RESERVED4].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_mp_profile_sel_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MP_PROFILE_SEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MP_PROFILE_SEL", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_format_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_FORMAT_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_FORMAT_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_RFC_6374 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_Y1731, "Y1731");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_RAW, "BFD_RAW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_IPV4, "BFD_IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_IPV6, "BFD_IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_RFC_6374, "RFC_6374");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_Y1731].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_RAW].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_IPV4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_BFD_IPV6].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_FORMAT_TYPE_RFC_6374].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_maid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_MAID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_MAID", 384, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_group_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GROUP_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "GROUP_ID", 18, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_in_lif_valid_for_lm_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IN_LIF_VALID_FOR_LM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IN_LIF_VALID_FOR_LM", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_nof_valid_lm_lifs_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NOF_VALID_LM_LIFS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NOF_VALID_LM_LIFS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_your_disc_valid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_YOUR_DISC_VALID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "YOUR_DISC_VALID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_group_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_GROUP_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_GROUP_ID", 32, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_your_discr_range_min_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_YOUR_DISCR_RANGE_MIN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_YOUR_DISCR_RANGE_MIN", 14, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_your_discr_range_max_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_YOUR_DISCR_RANGE_MAX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_YOUR_DISCR_RANGE_MAX", 14, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bcm_bfd_tunnel_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BCM_BFD_TUNNEL_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BCM_BFD_TUNNEL_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_COUNT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV4IN4, "IPV4IN4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV6IN4, "IPV6IN4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV4IN6, "IPV4IN6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV6IN6, "IPV6IN6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE4IN4, "GRE4IN4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_CONTROL_WORD, "PWE_CONTROL_WORD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_ROUTER_ALERT, "PWE_ROUTER_ALERT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_TTL, "PWE_TTL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS_TP_CC, "MPLS_TP_CC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS_TP_CC_CV, "MPLS_TP_CC_CV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE6IN4, "GRE6IN4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE4IN6, "GRE4IN6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE6IN6, "GRE6IN6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_GAL, "PWE_GAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS_PHP, "MPLS_PHP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_COUNT, "COUNT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_UDP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV4IN4].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV6IN4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV4IN6].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_IPV6IN6].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE4IN4].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_CONTROL_WORD].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_ROUTER_ALERT].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_TTL].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS_TP_CC].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS_TP_CC_CV].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE6IN4].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE4IN6].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_GRE6IN6].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_PWE_GAL].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_MPLS_PHP].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_BCM_BFD_TUNNEL_TYPE_COUNT].value_from_mapping = 17;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_counter_interface_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_COUNTER_INTERFACE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "COUNTER_INTERFACE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_icc_map_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ICC_MAP_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ICC_MAP_INDEX", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = 15;
    cur_field_type_param->min_value = 0;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_icc_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ICC_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ICC_DATA", 48, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY8, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oamp_mep_db_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_MEP_DB_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_MEP_DB_INDEX", 15, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_umc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_UMC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "UMC", 40, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY8, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_profile_rx_state_update_en_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PROFILE_RX_STATE_UPDATE_EN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PROFILE_RX_STATE_UPDATE_EN", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_GEN_PACKET + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_DONT_UPDATE_STATE_RDI_GEN_PACKET, "DONT_UPDATE_STATE_RDI_GEN_PACKET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_DONT_GEN_PACKET, "UPD_STATE_RDI_DONT_GEN_PACKET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_IF_FIFO_NOT_FULL_GEN_PKT, "UPD_STATE_RDI_IF_FIFO_NOT_FULL_GEN_PKT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_GEN_PACKET, "UPD_STATE_RDI_GEN_PACKET");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_DONT_UPDATE_STATE_RDI_GEN_PACKET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_DONT_GEN_PACKET].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_IF_FIFO_NOT_FULL_GEN_PKT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_GEN_PACKET].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_profile_scan_state_update_en_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PROFILE_SCAN_STATE_UPDATE_EN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PROFILE_SCAN_STATE_UPDATE_EN", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_GEN_PACKET + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_DONT_UPDATE_RMEP_DB_STATE_GEN_PACKET, "DONT_UPDATE_RMEP_DB_STATE_GEN_PACKET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_DONT_GEN_PACKET, "UPD_RMEP_DB_STATE_DONT_GEN_PACKET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_IF_FIFO_NOT_FULL_GEN_PKT, "UPD_RMEP_DB_STATE_IF_FIFO_NOT_FULL_GEN_PKT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_GEN_PACKET, "UPD_RMEP_DB_STATE_GEN_PACKET");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_DONT_UPDATE_RMEP_DB_STATE_GEN_PACKET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_DONT_GEN_PACKET].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_IF_FIFO_NOT_FULL_GEN_PKT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_GEN_PACKET].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_lm_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LM_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LM_MODE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_INVALID, "LM_MODE_INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_LM, "LM_MODE_IS_LM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_DUAL_ENDED, "LM_MODE_IS_DUAL_ENDED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_NONE, "LM_MODE_IS_NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LM_MODE_LM_MODE_INVALID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_LM].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_DUAL_ENDED].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_NONE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_dm_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DM_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DM_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_MODE_DM_MODE_INVALID, "DM_MODE_INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_1588, "DM_MODE_IS_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NTP, "DM_MODE_IS_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NONE, "DM_MODE_IS_NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_DM_MODE_DM_MODE_INVALID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_1588].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NTP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NONE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_is_oam_section_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IS_OAM_SECTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IS_OAM_SECTION", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_SECTION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_NORMAL, "OAM_NORMAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_SECTION, "OAM_SECTION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_NORMAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_SECTION].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_hlm_dis_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_HLM_DIS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "HLM_DIS", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_BOOL, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_tos_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TOS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TOS", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_loc_event_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LOC_EVENT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LOC_EVENT", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LOC_EVENT_LOC_CLEAR_EVENT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LOC_EVENT_NO_LOC_EVENT, "NO_LOC_EVENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LOC_EVENT_LOC_EVENT, "LOC_EVENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LOC_EVENT_ALMOST_LOC_EVENT, "ALMOST_LOC_EVENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LOC_EVENT_LOC_CLEAR_EVENT, "LOC_CLEAR_EVENT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LOC_EVENT_NO_LOC_EVENT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LOC_EVENT_LOC_EVENT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LOC_EVENT_ALMOST_LOC_EVENT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LOC_EVENT_LOC_CLEAR_EVENT].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_jr2_oamp_event_format_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_JR2_OAMP_EVENT_FORMAT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "JR2_OAMP_EVENT_FORMAT", 48, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 8;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_RMEP_INDEX;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_LOC_EVENT;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_RDI_SET;
    cur_field_types_info->struct_field_info[2].length = 1;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_RDI_CLEAR;
    cur_field_types_info->struct_field_info[3].length = 1;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_RMEP_STATE_CHANGE;
    cur_field_types_info->struct_field_info[4].length = 1;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_RESERVED_1;
    cur_field_types_info->struct_field_info[5].length = 2;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_RMEP_STATE;
    cur_field_types_info->struct_field_info[6].length = 18;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_RESERVED_2;
    cur_field_types_info->struct_field_info[7].length = 7;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_local_port_2_system_port_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LOCAL_PORT_2_SYSTEM_PORT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LOCAL_PORT_2_SYSTEM_PORT_PROFILE", dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_offloaded_mep_type_suffix_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OFFLOADED_MEP_TYPE_SUFFIX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OFFLOADED_MEP_TYPE_SUFFIX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_PWE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_CCM_ETHERNET, "CCM_ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_MPLSTP, "MPLSTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_PWE, "PWE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_CCM_ETHERNET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_MPLSTP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OFFLOADED_MEP_TYPE_SUFFIX_PWE].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_short_oam_entry_mep_type_suffix_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_PWE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_CCM_ETHERNET, "CCM_ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_MPLSTP, "MPLSTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_PWE, "PWE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_CCM_ETHERNET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_MPLSTP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_SHORT_OAM_ENTRY_MEP_TYPE_SUFFIX_PWE].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_bfd_local_discr_lsb_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BFD_LOCAL_DISCR_LSB];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BFD_LOCAL_DISCR_LSB", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_short_bfd_entry_mep_type_suffix_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_PWE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_1_HOP, "1_HOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_M_HOP, "M_HOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_PWE, "PWE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_INVALID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_1_HOP].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_M_HOP].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_MPLS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_SHORT_BFD_ENTRY_MEP_TYPE_SUFFIX_PWE].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_rdi_gen_method_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RDI_GEN_METHOD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RDI_GEN_METHOD", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_BOTH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_DEFAULT, "RDI_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_SCANNER, "RDI_FROM_SCANNER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_RX, "RDI_FROM_RX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_BOTH, "RDI_FROM_BOTH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_SCANNER].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_RX].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_RDI_GEN_METHOD_RDI_FROM_BOTH].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_opcode_rate_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OPCODE_RATE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OPCODE_RATE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_dm_one_way_timestamp_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DM_ONE_WAY_TIMESTAMP_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DM_ONE_WAY_TIMESTAMP_ENCODING", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_2_WAY, "ENCODING_2_WAY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT1, "ENCODING_1_WAY_FORMAT1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT2, "ENCODING_1_WAY_FORMAT2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT3, "ENCODING_1_WAY_FORMAT3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4, "ENCODING_1_WAY_FORMAT4");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_2_WAY].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT3].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_DM_ONE_WAY_TIMESTAMP_ENCODING_ENCODING_1_WAY_FORMAT4].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_oam_priority_map_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_PRIORITY_MAP_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_PRIORITY_MAP_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, FALSE));
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_interface_status_tlv_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INTERFACE_STATUS_TLV];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INTERFACE_STATUS_TLV", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_LOWER_LAYER_DOWN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_NONE, "INTERFACE_STATUS_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_UP, "INTERFACE_STATUS_UP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_DOWN, "INTERFACE_STATUS_DOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_TEST, "INTERFACE_STATUS_TEST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_UKNOWN, "INTERFACE_STATUS_UKNOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_DORMANT, "INTERFACE_STATUS_DORMANT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_NOT_PRESENT, "INTERFACE_STATUS_NOT_PRESENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_LOWER_LAYER_DOWN, "INTERFACE_STATUS_LOWER_LAYER_DOWN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_UP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_DOWN].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_TEST].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_UKNOWN].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_DORMANT].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_NOT_PRESENT].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_INTERFACE_STATUS_TLV_INTERFACE_STATUS_LOWER_LAYER_DOWN].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_oam_bfd_field_types_definition_port_status_tlv_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PORT_STATUS_TLV];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PORT_STATUS_TLV", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_UP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_NONE, "PORT_STATUS_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_BLOCKED, "PORT_STATUS_BLOCKED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_UP, "PORT_STATUS_UP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_BLOCKED].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PORT_STATUS_TLV_PORT_STATUS_UP].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_oam_bfd_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_dip_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_server_trap_code_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_mep_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oamp_entry_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oamp_mep_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_tx_rate_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_itmh_tc_dp_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_your_disc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_my_disc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_src_ip_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ip_ttl_tos_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_flags_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_state_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_detect_mult_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_min_interval_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_fec_id_or_glob_out_lif_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ivec_address_factor_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_diag_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mep_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mep_pe_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mep_db_bank_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_slm_measurement_period_timer_profile_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ip_subnet_len_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ccm_interval_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_maid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_endpoint_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_endpoint_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_short_icc_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_nof_vlan_tags_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_up_ptch_opaque_pt_attr_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_port_status_tlv_en_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_port_status_tlv_val_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_interface_status_tlv_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lmm_da_lsb_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lmm_da_msb_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_sa_gen_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_da_uc_mc_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_delay_dm_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_last_entry_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ext_data_length_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_opcodes_to_prepend_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_crc16_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_extra_data_segment_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_my_tx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_my_rx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_peer_tx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_peer_rx_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_cntrs_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_last_lm_far_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_last_lm_near_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_acc_lm_far_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_acc_lm_near_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_max_lm_far_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_max_lm_near_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ach_sel_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_measure_next_received_slr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_timestamp_format_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oamp_error_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_session_identifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_dm_t_flag_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_dm_ds_field_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_cw_choose_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_full_delay_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_port_core_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_report_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_opcode_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_mac_da_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_endpoint_sw_state_flags_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_counter_base_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_counter_if_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_rmep_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lifetime_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lifetime_units_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_loc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_loc_clear_limit_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_loc_clear_enable_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_punt_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_punt_good_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_punt_next_good_packet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_rdi_received_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_eth_port_state_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_eth_interface_state_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_packet_is_oam_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_counter_ptr_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mp_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_lm_counter_read_idx_sel_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_counter_stamp_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_stamp_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_your_discr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_up_mep_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_meter_disable_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mep_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_key_base_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_set_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_prefix_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_endpoint_sw_state_flags_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_counter_disable_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_access_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_opcode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_opcode_for_count_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mip_above_mdl_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_packet_is_bfd_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mdl_mp_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_ingress_oam_data_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_egress_oam_data_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_nof_mep_below_mdl_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_lif_db_access_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_nof_mep_above_mdl_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_inject_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_my_cfm_mac_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_da_is_mc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_ingress_up_mep_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_ingress_meter_disable_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_direction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_process_action_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mp_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_internal_opcode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_ing_key_base_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_eg_key_base_select_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_key_prefix_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_mp_profile_sel_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_format_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_maid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_group_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_in_lif_valid_for_lm_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_nof_valid_lm_lifs_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_your_disc_valid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_group_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_your_discr_range_min_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_your_discr_range_max_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bcm_bfd_tunnel_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_counter_interface_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_icc_map_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_icc_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oamp_mep_db_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_umc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_profile_rx_state_update_en_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_profile_scan_state_update_en_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_lm_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_dm_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_is_oam_section_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_hlm_dis_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_tos_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_loc_event_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_jr2_oamp_event_format_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_local_port_2_system_port_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_offloaded_mep_type_suffix_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_short_oam_entry_mep_type_suffix_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_bfd_local_discr_lsb_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_short_bfd_entry_mep_type_suffix_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_rdi_gen_method_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_opcode_rate_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_dm_one_way_timestamp_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_oam_priority_map_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_interface_status_tlv_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_oam_bfd_field_types_definition_port_status_tlv_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
