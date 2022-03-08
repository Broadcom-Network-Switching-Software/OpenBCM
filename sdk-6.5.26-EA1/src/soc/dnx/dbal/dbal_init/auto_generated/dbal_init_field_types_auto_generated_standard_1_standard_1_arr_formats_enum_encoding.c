
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_egress_glem_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_GLEM_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_GLEM_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_GLEM_GLOBAL_TO_LOCAL, "GLEM_GLOBAL_TO_LOCAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_GLEM_GLOBAL_TO_LOCAL_RIF_1, "GLEM_GLOBAL_TO_LOCAL_RIF_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_GLEM_GLOBAL_TO_LOCAL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_GLEM_GLOBAL_TO_LOCAL_RIF_1].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_EGRESS_GLEM_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_egress_ppmc_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_PPMC_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_PPMC_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_TRIPLE_OUTLIF + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_DOUBLE_OUTLIF, "PPMC_DOUBLE_OUTLIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_EMPTY, "PPMC_EMPTY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_RESERVED_4, "PPMC_RESERVED_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_RESERVED_5, "PPMC_RESERVED_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_SINGLE_OUTLIF, "PPMC_SINGLE_OUTLIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_TRIPLE_OUTLIF, "PPMC_TRIPLE_OUTLIF");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_DOUBLE_OUTLIF].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_EMPTY].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_RESERVED_4].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_RESERVED_5].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_SINGLE_OUTLIF].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_PPMC_FORMATS_PPMC_TRIPLE_OUTLIF].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_egress_vsi_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_VSI_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_VSI_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_VSI_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_VSI_FORMATS_EGRESS_VSI_1, "EGRESS_VSI_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_VSI_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_VSI_FORMATS_EGRESS_VSI_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_VSI_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_etpp_etps_data_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ETPS_DATA_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ETPS_DATA_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_BFR_BIT_STR, "ETPS_DATA_BFR_BIT_STR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_IPFIX_CODE, "ETPS_DATA_IPFIX_CODE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_IPV6_DIP, "ETPS_DATA_IPV6_DIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_SFLOW, "ETPS_DATA_SFLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_SID, "ETPS_DATA_SID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_BFR_BIT_STR].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_IPFIX_CODE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_IPV6_DIP].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_SFLOW].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_ETPS_DATA_SID].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_DATA_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_etpp_etps_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ETPS_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ETPS_FORMATS", dnx_data_pp.ETPP.etps_type_size_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT, "ETPS_AC_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION, "ETPS_AC_STAT_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER, "ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG, "ETPS_AC_TRIPLE_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP, "ETPS_ARP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC, "ETPS_ARP_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT, "ETPS_ARP_AC_1TAG_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM, "ETPS_ARP_SA_CUSTOM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING, "ETPS_ARP_SA_TUNNELING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP, "ETPS_BFD_IPV4_EXTRA_SIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE, "ETPS_BIER_BASE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES, "ETPS_BIER_NON_MPLS_INITIAL_4BYTES");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA, "ETPS_DATA_VAR_SIZE_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING, "ETPS_DUAL_HOMING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2, "ETPS_ERSPANV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3, "ETPS_ERSPANV3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF, "ETPS_ETH_RIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT, "ETPS_ETH_RIF_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U, "ETPS_GTPV1U");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL, "ETPS_IOAM_FL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP, "ETPS_IPFIX_PSAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL, "ETPS_IPV4_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER, "ETPS_IPV4_TUNNEL_COPY_INNER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN, "ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE, "ETPS_IPV4_TUNNEL_GRE_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP, "ETPS_IPV4_TUNNEL_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE, "ETPS_IPV4_TUNNEL_VXLAN_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN, "ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE, "ETPS_IPV6_TUNNEL_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE, "ETPS_IPV6_TUNNEL_GRE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW, "ETPS_IPV6_TUNNEL_RAW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE, "ETPS_IPV6_TUNNEL_SCALE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP, "ETPS_IPV6_TUNNEL_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN, "ETPS_IPV6_TUNNEL_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN, "ETPS_IPV6_TUNNEL_VXLAN_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2, "ETPS_L2TPV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO, "ETPS_L2_FODO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS, "ETPS_L2_FODO_VLANS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO, "ETPS_L3_FODO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1, "ETPS_MPLS_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH, "ETPS_MPLS_1_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML, "ETPS_MPLS_1_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT, "ETPS_MPLS_1_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM, "ETPS_MPLS_1_TANDEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2, "ETPS_MPLS_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH, "ETPS_MPLS_2_1_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH, "ETPS_MPLS_2_2_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH, "ETPS_MPLS_2_3_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT, "ETPS_MPLS_2_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM, "ETPS_MPLS_2_TANDEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI, "ETPS_MPLS_FHEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS, "ETPS_MPLS_FHEI_PLUS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT, "ETPS_MPLS_IFIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP, "ETPS_MPLS_PHP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT, "ETPS_MPLS_PHP_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL, "ETPS_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE, "ETPS_PPPOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2, "ETPS_RCHV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR, "ETPS_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC, "ETPS_REFLECTOR_L2_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM, "ETPS_REFLECTOR_OAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP, "ETPS_REFLECTOR_TWAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN, "ETPS_RSPAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST, "ETPS_SFLOW_EXT_FIRST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND, "ETPS_SFLOW_EXT_SECOND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP, "ETPS_SFLOW_HDR_DP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP, "ETPS_SFLOW_HDR_SAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6, "ETPS_SRV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS, "ETPS_SRV6_NO_SIDS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG, "ETPS_SVTAG");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].value_from_mapping = 65;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].value_from_mapping = 78;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].value_from_mapping = 50;
        }
    }
    else if (DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].value_from_mapping = 65;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].value_from_mapping = 78;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].value_from_mapping = 50;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_Q2A_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT].value_from_mapping = 11;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER].value_from_mapping = 9;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BFD_IPV4_EXTRA_SIP].value_from_mapping = 22;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_BASE].value_from_mapping = 23;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_BIER_NON_MPLS_INITIAL_4BYTES].value_from_mapping = 24;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DATA_VAR_SIZE_DATA].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV2].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ERSPANV3].value_from_mapping = 56;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_GTPV1U].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IOAM_FL].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPFIX_PSAMP].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL].value_from_mapping = 20;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_COPY_INNER].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_GRE_GENEVE].value_from_mapping = 19;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_UDP].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE].value_from_mapping = 17;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN].value_from_mapping = 13;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GENEVE].value_from_mapping = 44;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_GRE].value_from_mapping = 28;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_RAW].value_from_mapping = 30;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_SCALE].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_UDP].value_from_mapping = 31;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_IPV6_TUNNEL_VXLAN_ECN].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2TPV2].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_L3_FODO].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_IML].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_STAT].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_1_TANDEM].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2].value_from_mapping = 27;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_1_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_2_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_3_AH].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_STAT].value_from_mapping = 26;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_2_TANDEM].value_from_mapping = 18;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_FHEI_PLUS].value_from_mapping = 29;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_IFIT].value_from_mapping = 25;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_MPLS_PHP_STAT].value_from_mapping = 16;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_NULL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_PPPOE].value_from_mapping = 21;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RCHV2].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_L2_MC].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_OAM].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_REFLECTOR_TWAMP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_RSPAN].value_from_mapping = 43;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_FIRST].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_EXT_SECOND].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_DP].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SFLOW_HDR_SAMP].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6].value_from_mapping = 14;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SRV6_NO_SIDS].value_from_mapping = 12;
            enum_info[DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fec_entry_format_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_FEC_ENTRY_FORMAT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_FEC_ENTRY_FORMAT", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION, "FEC_ENTRY_DESTINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_EEI, "FEC_ENTRY_DESTINATION_EEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0, "FEC_ENTRY_DESTINATION_LIF0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1, "FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_HTM, "FEC_ENTRY_DESTINATION_LIF0_HTM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF, "FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_MC_RPF, "FEC_ENTRY_DESTINATION_LIF0_MC_RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_EEI, "FEC_ENTRY_JR1_DESTINATION_EEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP, "FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_EEI].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_HTM].value_from_mapping = 39;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_MC_RPF].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_EEI].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP].value_from_mapping = 20;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fec_super_entry_format_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_FEC_SUPER_ENTRY_FORMAT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_FEC_SUPER_ENTRY_FORMAT", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_W_PROTECTION_W_1_STAT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_NO_PROTECTION, "SUPER_FEC_NO_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_NO_PROTECTION_W_2_STAT, "SUPER_FEC_NO_PROTECTION_W_2_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_W_PROTECTION, "SUPER_FEC_W_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_W_PROTECTION_W_1_STAT, "SUPER_FEC_W_PROTECTION_W_1_STAT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_NO_PROTECTION].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_NO_PROTECTION_W_2_STAT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_W_PROTECTION].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_FEC_SUPER_ENTRY_FORMAT_SUPER_FEC_W_PROTECTION_W_1_STAT].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fwd_stage_floating_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FLOATING_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_FWD_STAGE_FLOATING_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_COMPRESSED_SOURCE, "COMPRESSED_SOURCE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_COMPRESSED_SOURCE_AND_INTERFACE, "COMPRESSED_SOURCE_AND_INTERFACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_EM_PMF_ADDITIONAL_DATA_RESULT, "EM_PMF_ADDITIONAL_DATA_RESULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_FWD_NAT_TRANSLATION_RESULT, "FWD_NAT_TRANSLATION_RESULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_KBP_FWD_NAT_TRANSLATION_RESULT, "KBP_FWD_NAT_TRANSLATION_RESULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_LEARN_DESTINATION, "LEARN_DESTINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_TCAM_RESULT_VRF_V4, "TCAM_RESULT_VRF_V4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_TCAM_RESULT_VRF_V6, "TCAM_RESULT_VRF_V6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_COMPRESSED_SOURCE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_COMPRESSED_SOURCE_AND_INTERFACE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_EM_PMF_ADDITIONAL_DATA_RESULT].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_FWD_NAT_TRANSLATION_RESULT].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_KBP_FWD_NAT_TRANSLATION_RESULT].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_LEARN_DESTINATION].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_TCAM_RESULT_VRF_V4].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_TCAM_RESULT_VRF_V6].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FLOATING_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fwd_stage_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_FWD_STAGE_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_RPF_DEST_W_DEFAULT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST, "FWD_DEST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_DOUBLE_OUTLIF, "FWD_DEST_DOUBLE_OUTLIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_DOUBLE_OUTLIF_STAT, "FWD_DEST_DOUBLE_OUTLIF_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_EEI, "FWD_DEST_EEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_EEI_STAT, "FWD_DEST_EEI_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF, "FWD_DEST_OUTLIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF_STAT, "FWD_DEST_OUTLIF_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF_STAT_W_DEFAULT, "FWD_DEST_OUTLIF_STAT_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF_W_DEFAULT, "FWD_DEST_OUTLIF_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_STAT, "FWD_DEST_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_STAT_W_DEFAULT, "FWD_DEST_STAT_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_W_DEFAULT, "FWD_DEST_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_DEST_LIF_STAT, "FWD_MACT_RESULT_DEST_LIF_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_DEST_STAT, "FWD_MACT_RESULT_DEST_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_EEI_FEC, "FWD_MACT_RESULT_EEI_FEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_NO_OUTLIF, "FWD_MACT_RESULT_NO_OUTLIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_NO_OUTLIF_NO_AUTO_LEARN, "FWD_MACT_RESULT_NO_OUTLIF_NO_AUTO_LEARN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_SINGLE_OUTLIF, "FWD_MACT_RESULT_SINGLE_OUTLIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_NO_ACTION, "FWD_NO_ACTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC, "KAPS_FEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_2, "KAPS_FEC_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_3, "KAPS_FEC_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_4, "KAPS_FEC_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_5, "KAPS_FEC_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_6, "KAPS_FEC_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_DEFAULT, "KAPS_FEC_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_MC_ID, "KAPS_MC_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_DEST_W_DEFAULT, "KBP_DEST_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_FORWARD_DEST_EEI_W_DEFAULT, "KBP_FORWARD_DEST_EEI_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_FORWARD_DEST_STAT_W_DEFAULT, "KBP_FORWARD_DEST_STAT_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_FORWARD_OUTLIF_W_DEFAULT, "KBP_FORWARD_OUTLIF_W_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_RPF_DEST_W_DEFAULT, "KBP_RPF_DEST_W_DEFAULT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_DOUBLE_OUTLIF].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_DOUBLE_OUTLIF_STAT].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_EEI].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_EEI_STAT].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF_STAT].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF_STAT_W_DEFAULT].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_OUTLIF_W_DEFAULT].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_STAT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_STAT_W_DEFAULT].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_DEST_W_DEFAULT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_DEST_LIF_STAT].value_from_mapping = 50;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_DEST_STAT].value_from_mapping = 49;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_EEI_FEC].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_NO_OUTLIF].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_NO_OUTLIF_NO_AUTO_LEARN].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_MACT_RESULT_SINGLE_OUTLIF].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_FWD_NO_ACTION].value_from_mapping = 55;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_2].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_3].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_4].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_5].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_6].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_FEC_DEFAULT].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KAPS_MC_ID].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_DEST_W_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_FORWARD_DEST_EEI_W_DEFAULT].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_FORWARD_DEST_STAT_W_DEFAULT].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_FORWARD_OUTLIF_W_DEFAULT].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_IRPP_FWD_STAGE_FORMATS_KBP_RPF_DEST_W_DEFAULT].value_from_mapping = 18;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_in_lif_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_IN_LIF_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_IN_LIF_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP, "IN_ETH_AC_MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P, "IN_ETH_AC_VSI_P2P");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY, "IN_ETH_VLAN_EDIT_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP, "IN_ETH_VLAN_EDIT_VSI_MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI, "IN_LIF_AC2EEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA, "IN_LIF_AC2EEI_LARGE_GENERIC_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION, "IN_LIF_AC2EEI_WITH_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP, "IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP, "IN_LIF_AC_MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE, "IN_LIF_AC_MP_LARGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA, "IN_LIF_AC_MP_LARGE_GENERIC_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION, "IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD, "IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P, "IN_LIF_AC_P2P");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE, "IN_LIF_AC_P2P_LARGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA, "IN_LIF_AC_P2P_LARGE_GENERIC_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN, "IN_LIF_AC_P2P_W_1_VLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION, "IN_LIF_AC_P2P_W_VSI_WO_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN, "IN_LIF_AC_P2P_W_VSI_W_1_VLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER, "IN_LIF_BIER_BFER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY, "IN_LIF_DUMMY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP, "IN_LIF_EVPN_EVI_MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING, "IN_LIF_EVPN_EVI_P2P_NO_LEARNING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1, "IN_LIF_IPVX_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2, "IN_LIF_IPVX_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3, "IN_LIF_IPVX_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP, "IN_LIF_ISID_MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P, "IN_LIF_ISID_P2P");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP, "IN_LIF_LSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL, "IN_LIF_MIM_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY, "IN_LIF_MPLS_DUMMY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT, "IN_LIF_P2P_COMPACT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING, "IN_LIF_PWE2EEI_NO_LEARNING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP, "IN_LIF_PWE_MP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING, "IN_LIF_PWE_P2P_NO_LEARNING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED, "IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF, "IN_LIF_RCH_CONTROL_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA, "IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA, "IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA, "IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    else if (DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    else if (DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    else if (DBAL_IS_Q2A_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    else if (DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_MP].value_from_mapping = 50;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_AC_VSI_P2P].value_from_mapping = 41;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_ONLY].value_from_mapping = 40;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_ETH_VLAN_EDIT_VSI_MP].value_from_mapping = 39;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC2EEI_WITH_PROTECTION].value_from_mapping = 38;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_EXTENDED_P2P_W_SRC_LKP].value_from_mapping = 62;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE].value_from_mapping = 60;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_LARGE_GENERIC_DATA_AND_PROTECTION].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE].value_from_mapping = 61;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_LARGE_GENERIC_DATA].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_1_VLAN].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_WO_PROTECTION].value_from_mapping = 58;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_AC_P2P_W_VSI_W_1_VLAN].value_from_mapping = 57;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_BIER_BFER].value_from_mapping = 35;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_DUMMY].value_from_mapping = 34;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_MP].value_from_mapping = 51;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_EVPN_EVI_P2P_NO_LEARNING].value_from_mapping = 52;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_1].value_from_mapping = 47;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_2].value_from_mapping = 48;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_IPVX_3].value_from_mapping = 49;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_MP].value_from_mapping = 54;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_ISID_P2P].value_from_mapping = 55;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_LSP].value_from_mapping = 33;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MIM_TUNNEL].value_from_mapping = 53;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_MPLS_DUMMY].value_from_mapping = 42;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_P2P_COMPACT].value_from_mapping = 32;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE2EEI_NO_LEARNING].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_MP].value_from_mapping = 36;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING].value_from_mapping = 37;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED].value_from_mapping = 46;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_RCH_CONTROL_LIF].value_from_mapping = 15;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA].value_from_mapping = 45;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA].value_from_mapping = 59;
            enum_info[DBAL_ENUM_FVAL_IRPP_IN_LIF_FORMATS_IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA].value_from_mapping = 56;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_isem_access_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_ISEM_ACCESS_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_ISEM_ACCESS_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_COMPRESSED_MAC_ID, "COMPRESSED_MAC_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_FORWARD_DOMAIN, "FORWARD_DOMAIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_INLIF_INDEX, "INLIF_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_IPV6_32MSBS_COMPRESSION_ID, "IPV6_32MSBS_COMPRESSION_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_IPV6_ADDRESS_MSBS_ID, "IPV6_ADDRESS_MSBS_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_IPV6_MP_TUNNEL_IDX, "IPV6_MP_TUNNEL_IDX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_COMPRESSED_MAC_ID].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_FORWARD_DOMAIN].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_INLIF_INDEX].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_IPV6_32MSBS_COMPRESSION_ID].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_IPV6_ADDRESS_MSBS_ID].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_IPV6_MP_TUNNEL_IDX].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_ISEM_ACCESS_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_my_mac_exem_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_MY_MAC_EXEM_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_MY_MAC_EXEM_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_MY_MAC_EXEM_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_MY_MAC_EXEM_FORMATS_MY_MAC_EXEM_RESULT, "MY_MAC_EXEM_RESULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_MY_MAC_EXEM_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_MY_MAC_EXEM_FORMATS_MY_MAC_EXEM_RESULT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_MY_MAC_EXEM_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_vsi_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_VSI_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_VSI_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_WITH_STATS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_BASIC, "VSI_ENTRY_BASIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_BASIC_NON_MC_DESTINATION, "VSI_ENTRY_BASIC_NON_MC_DESTINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_WITH_STATS, "VSI_ENTRY_WITH_STATS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_BASIC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_BASIC_NON_MC_DESTINATION].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_VSI_FORMATS_VSI_ENTRY_WITH_STATS].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_vtt_tcam_access_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_VTT_TCAM_ACCESS_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_VTT_TCAM_ACCESS_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_VTT_TCAM_ACCESS_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_VTT_TCAM_ACCESS_FORMATS_FOUND, "FOUND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_VTT_TCAM_ACCESS_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_VTT_TCAM_ACCESS_FORMATS_FOUND].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_VTT_TCAM_ACCESS_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_oamp_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAMP_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAMP_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAMP_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_IPV4_MULTI_HOP_SHORT_STATIC, "MEP_DB_BFD_ON_IPV4_MULTI_HOP_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_IPV4_ONE_HOP_SHORT_STATIC, "MEP_DB_BFD_ON_IPV4_ONE_HOP_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_MPLS_SHORT_STATIC, "MEP_DB_BFD_ON_MPLS_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_PWE_SHORT_STATIC, "MEP_DB_BFD_ON_PWE_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_PWE_STATIC_PART_1, "MEP_DB_BFD_ON_PWE_STATIC_PART_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_PWE_STATIC_PART_2, "MEP_DB_BFD_ON_PWE_STATIC_PART_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_SHORT_STATIC, "MEP_DB_CCM_ETH_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_STATIC_PART_1_DOWNMEP, "MEP_DB_CCM_ETH_STATIC_PART_1_DOWNMEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_STATIC_PART_1_UPMEP, "MEP_DB_CCM_ETH_STATIC_PART_1_UPMEP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_STATIC_PART_2, "MEP_DB_CCM_ETH_STATIC_PART_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_EXTRA_DATA_IN_MDB_HEADER, "MEP_DB_EXTRA_DATA_IN_MDB_HEADER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_EXTRA_DATA_IN_MDB_PAYLOAD, "MEP_DB_EXTRA_DATA_IN_MDB_PAYLOAD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_MPLS_TP_SHORT_STATIC, "MEP_DB_Y1731_ON_MPLS_TP_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_1, "MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_2, "MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_PWE_SHORT_STATIC, "MEP_DB_Y1731_ON_PWE_SHORT_STATIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_PWE_STATIC_PART_1, "MEP_DB_Y1731_ON_PWE_STATIC_PART_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_PWE_STATIC_PART_2, "MEP_DB_Y1731_ON_PWE_STATIC_PART_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_OAMP_LM_SESSION_ID, "OAMP_LM_SESSION_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_OAMP_RMEP_INDEX, "OAMP_RMEP_INDEX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_OAMP_RMEP_STATIC_DATA, "OAMP_RMEP_STATIC_DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAMP_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_IPV4_MULTI_HOP_SHORT_STATIC].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_IPV4_ONE_HOP_SHORT_STATIC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_MPLS_SHORT_STATIC].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_PWE_SHORT_STATIC].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_PWE_STATIC_PART_1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_BFD_ON_PWE_STATIC_PART_2].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_SHORT_STATIC].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_STATIC_PART_1_DOWNMEP].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_STATIC_PART_1_UPMEP].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_CCM_ETH_STATIC_PART_2].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_EXTRA_DATA_IN_MDB_HEADER].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_EXTRA_DATA_IN_MDB_PAYLOAD].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_MPLS_TP_SHORT_STATIC].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_1].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_2].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_PWE_SHORT_STATIC].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_PWE_STATIC_PART_1].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_MEP_DB_Y1731_ON_PWE_STATIC_PART_2].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_OAMP_LM_SESSION_ID].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_OAMP_RMEP_INDEX].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_OAMP_RMEP_STATIC_DATA].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_OAMP_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_oam_classifier_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_CLASSIFIER_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_CLASSIFIER_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_OAM_ACC_MEP_DB, "OAM_ACC_MEP_DB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_OAM_LIF_DB, "OAM_LIF_DB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_OAM_ACC_MEP_DB].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_OAM_LIF_DB].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_CLASSIFIER_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_pem_db_user_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PEM_DB_USER_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PEM_DB_USER_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PEM_DB_USER_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PEM_DB_USER_FORMATS_MPLS_SPECULATIVE_PARSING_PORT_TYPE, "MPLS_SPECULATIVE_PARSING_PORT_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PEM_DB_USER_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PEM_DB_USER_FORMATS_MPLS_SPECULATIVE_PARSING_PORT_TYPE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PEM_DB_USER_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_prt_vp_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRT_VP_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRT_VP_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PRT_VP_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRT_VP_FORMATS_IN_PP_PORT_INDEX_RESULT_TYPE, "IN_PP_PORT_INDEX_RESULT_TYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRT_VP_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PRT_VP_FORMATS_IN_PP_PORT_INDEX_RESULT_TYPE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PRT_VP_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_tcam_identification_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TCAM_IDENTIFICATION_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TCAM_IDENTIFICATION_FORMATS", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TCAM_IDENTIFICATION_FORMATS_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TCAM_IDENTIFICATION_FORMATS_OAM_IDENTIFICATION, "OAM_IDENTIFICATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TCAM_IDENTIFICATION_FORMATS_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_TCAM_IDENTIFICATION_FORMATS_OAM_IDENTIFICATION].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_TCAM_IDENTIFICATION_FORMATS_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_em_buster_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EEDB_TYPE_EM_BUSTER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EEDB_TYPE_EM_BUSTER", dnx_data_pp.ETPP.etps_type_size_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_RSPAN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_AC_STAT, "ETPS_AC_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_AC_STAT_PROTECTION, "ETPS_AC_STAT_PROTECTION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_AC_TRIPLE_TAG, "ETPS_AC_TRIPLE_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_ARP_AC, "ETPS_ARP_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_ARP_AC_1TAG_STAT, "ETPS_ARP_AC_1TAG_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_L2_FODO_VLANS, "ETPS_L2_FODO_VLANS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_RSPAN, "ETPS_RSPAN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_AC_STAT].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_AC_STAT_PROTECTION].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_AC_TRIPLE_TAG].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_ARP_AC].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_L2_FODO_VLANS].value_from_mapping = 33;
        enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_EM_BUSTER_ETPS_RSPAN].value_from_mapping = 43;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_vsd_enable_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EEDB_TYPE_VSD_ENABLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EEDB_TYPE_VSD_ENABLE", dnx_data_pp.ETPP.etps_type_size_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC, "ETPS_ARP_AC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT, "ETPS_ARP_AC_1TAG_STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM, "ETPS_ARP_SA_CUSTOM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING, "ETPS_ARP_SA_TUNNELING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF, "ETPS_ETH_RIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___, "ETPS_ETH_RIF___1___");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT, "ETPS_ETH_RIF_STAT");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 66;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].value_from_mapping = 67;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 66;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].value_from_mapping = 67;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_Q2A_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_AC_1TAG_STAT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_CUSTOM].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ARP_SA_TUNNELING].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF___1___].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_VSD_ENABLE_ETPS_ETH_RIF_STAT].value_from_mapping = 3;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_svtag_enable_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EEDB_TYPE_SVTAG_ENABLE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EEDB_TYPE_SVTAG_ENABLE", dnx_data_pp.ETPP.etps_type_size_bits_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG, "ETPS_SVTAG");
    }
    if (DBAL_IS_J2C_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].value_from_mapping = 50;
        }
    }
    else if (DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].value_from_mapping = 50;
        }
    }
    else if (DBAL_IS_JR2_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_Q2A_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_NONE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_EEDB_TYPE_SVTAG_ENABLE_ETPS_SVTAG].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_egress_glem_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_egress_ppmc_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_egress_vsi_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_etpp_etps_data_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_etpp_etps_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fec_entry_format_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fec_super_entry_format_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fwd_stage_floating_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fwd_stage_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_in_lif_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_isem_access_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_my_mac_exem_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_vsi_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_vtt_tcam_access_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_oamp_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_oam_classifier_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_pem_db_user_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_prt_vp_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_tcam_identification_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_em_buster_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_vsd_enable_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_svtag_enable_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
