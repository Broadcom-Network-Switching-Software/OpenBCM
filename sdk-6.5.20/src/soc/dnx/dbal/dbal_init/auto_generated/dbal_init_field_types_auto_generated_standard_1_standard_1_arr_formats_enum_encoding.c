
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EGRESS_GLEM_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 3 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "GLEM_GLOBAL_TO_LOCAL", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "GLEM_GLOBAL_TO_LOCAL_RIF_1", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[2].name_from_interface));
    cur_field_type_param->nof_enum_vals = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 2;
    cur_field_type_param->enums[2].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EGRESS_PPMC_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 6 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "PPMC_DOUBLE_OUTLIF", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "PPMC_EMPTY", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "PPMC_RESERVED_4", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "PPMC_RESERVED_5", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "PPMC_SINGLE_OUTLIF", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "PPMC_TRIPLE_OUTLIF", sizeof(cur_field_type_param->enums[5].name_from_interface));
    cur_field_type_param->nof_enum_vals = 6;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 2;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    cur_field_type_param->enums[2].value_from_mapping = 4;
    cur_field_type_param->enums[3].value_from_mapping = 5;
    cur_field_type_param->enums[4].value_from_mapping = 1;
    cur_field_type_param->enums[5].value_from_mapping = 3;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EGRESS_VSI_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "EGRESS_VSI_1", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ETPP_ETPS_FORMATS" , dnx_data_pp.ETPP.etps_type_size_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 66 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "ETPS_AC", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ETPS_AC_QOS_OAM", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ETPS_AC_STAT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "ETPS_AC_STAT_PROTECTION", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "ETPS_AC_TRIPLE_TAG", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "ETPS_ARP", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "ETPS_ARP_AC", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "ETPS_ARP_AC_1TAG_STAT", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "ETPS_ARP_PLUS_AC_1TAG", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "ETPS_ARP_SA_CUSTOM", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "ETPS_ARP_SA_TUNNELING", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "ETPS_BFD_IPV4_EXTRA_SIP", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "ETPS_DATA_128B", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "ETPS_DATA_VAR_SIZE_DATA", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "ETPS_DUAL_HOMING", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "ETPS_ERSPANV2", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "ETPS_ERSPANV3", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "ETPS_ETH_RIF", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "ETPS_ETH_RIF_STAT", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "ETPS_GTPV1U", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "ETPS_IFA_COUNT", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "ETPS_IPFIX_PSAMP", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "ETPS_IPV4_TUNNEL", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "ETPS_IPV4_TUNNEL_GRE_GENEVE", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "ETPS_IPV4_TUNNEL_UDP", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "ETPS_IPV4_TUNNEL_VXLAN_GPE", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "ETPS_IPV6_TUNNEL_GENEVE", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "ETPS_IPV6_TUNNEL_GRE", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "ETPS_IPV6_TUNNEL_RAW", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "ETPS_IPV6_TUNNEL_UDP", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "ETPS_IPV6_TUNNEL_VXLAN", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "ETPS_IPV6_TUNNEL_VXLAN_ECN", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "ETPS_L2TPV2", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "ETPS_L2_FODO", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "ETPS_L2_FODO_VLANS", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "ETPS_L3_FODO", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "ETPS_MPLS_1", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "ETPS_MPLS_1_AH", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "ETPS_MPLS_1_IML", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "ETPS_MPLS_1_STAT", sizeof(cur_field_type_param->enums[40].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[41].name_from_interface, "ETPS_MPLS_1_TANDEM", sizeof(cur_field_type_param->enums[41].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[42].name_from_interface, "ETPS_MPLS_2", sizeof(cur_field_type_param->enums[42].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[43].name_from_interface, "ETPS_MPLS_2_1_AH", sizeof(cur_field_type_param->enums[43].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[44].name_from_interface, "ETPS_MPLS_2_2_AH", sizeof(cur_field_type_param->enums[44].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[45].name_from_interface, "ETPS_MPLS_2_3_AH", sizeof(cur_field_type_param->enums[45].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[46].name_from_interface, "ETPS_MPLS_2_STAT", sizeof(cur_field_type_param->enums[46].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[47].name_from_interface, "ETPS_MPLS_2_TANDEM", sizeof(cur_field_type_param->enums[47].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[48].name_from_interface, "ETPS_MPLS_FHEI", sizeof(cur_field_type_param->enums[48].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[49].name_from_interface, "ETPS_MPLS_PHP", sizeof(cur_field_type_param->enums[49].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[50].name_from_interface, "ETPS_MPLS_PHP_STAT", sizeof(cur_field_type_param->enums[50].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[51].name_from_interface, "ETPS_NULL", sizeof(cur_field_type_param->enums[51].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[52].name_from_interface, "ETPS_PPPOE", sizeof(cur_field_type_param->enums[52].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[53].name_from_interface, "ETPS_RCHV2", sizeof(cur_field_type_param->enums[53].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[54].name_from_interface, "ETPS_REFLECTOR", sizeof(cur_field_type_param->enums[54].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[55].name_from_interface, "ETPS_REFLECTOR_L2_MC", sizeof(cur_field_type_param->enums[55].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[56].name_from_interface, "ETPS_REFLECTOR_OAM", sizeof(cur_field_type_param->enums[56].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[57].name_from_interface, "ETPS_REFLECTOR_TWAMP", sizeof(cur_field_type_param->enums[57].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[58].name_from_interface, "ETPS_RSPAN", sizeof(cur_field_type_param->enums[58].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[59].name_from_interface, "ETPS_SFLOW_EXT_FIRST", sizeof(cur_field_type_param->enums[59].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[60].name_from_interface, "ETPS_SFLOW_EXT_SECOND", sizeof(cur_field_type_param->enums[60].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[61].name_from_interface, "ETPS_SFLOW_HDR_DP", sizeof(cur_field_type_param->enums[61].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[62].name_from_interface, "ETPS_SFLOW_HDR_SAMP", sizeof(cur_field_type_param->enums[62].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[63].name_from_interface, "ETPS_SRV6", sizeof(cur_field_type_param->enums[63].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[64].name_from_interface, "ETPS_SRV6_T_INSERT", sizeof(cur_field_type_param->enums[64].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[65].name_from_interface, "ETPS_SVTAG", sizeof(cur_field_type_param->enums[65].name_from_interface));
    cur_field_type_param->nof_enum_vals = 66;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 11;
    cur_field_type_param->enums[1].value_from_mapping = 12;
    cur_field_type_param->enums[2].value_from_mapping = 9;
    cur_field_type_param->enums[3].value_from_mapping = 8;
    cur_field_type_param->enums[4].value_from_mapping = 10;
    cur_field_type_param->enums[5].value_from_mapping = 2;
    cur_field_type_param->enums[6].value_from_mapping = 4;
    cur_field_type_param->enums[7].value_from_mapping = 6;
    cur_field_type_param->enums[8].value_from_mapping = 47;
    cur_field_type_param->enums[9].value_from_mapping = 5;
    cur_field_type_param->enums[10].value_from_mapping = 7;
    cur_field_type_param->enums[11].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___194_get(unit);
    cur_field_type_param->enums[12].value_from_mapping = 3;
    cur_field_type_param->enums[13].value_from_mapping = 55;
    cur_field_type_param->enums[14].value_from_mapping = 61;
    cur_field_type_param->enums[15].value_from_mapping = 60;
    cur_field_type_param->enums[16].value_from_mapping = 56;
    cur_field_type_param->enums[17].value_from_mapping = 1;
    cur_field_type_param->enums[18].value_from_mapping = 3;
    cur_field_type_param->enums[19].value_from_mapping = 46;
    cur_field_type_param->enums[20].value_from_mapping = 50;
    cur_field_type_param->enums[21].value_from_mapping = 58;
    cur_field_type_param->enums[22].value_from_mapping = 20;
    cur_field_type_param->enums[23].value_from_mapping = 19;
    cur_field_type_param->enums[24].value_from_mapping = 32;
    cur_field_type_param->enums[25].value_from_mapping = 17;
    cur_field_type_param->enums[26].value_from_mapping = 13;
    cur_field_type_param->enums[27].value_from_mapping = 44;
    cur_field_type_param->enums[28].value_from_mapping = 37;
    cur_field_type_param->enums[29].value_from_mapping = 30;
    cur_field_type_param->enums[30].value_from_mapping = 31;
    cur_field_type_param->enums[31].value_from_mapping = 34;
    cur_field_type_param->enums[32].value_from_mapping = 35;
    cur_field_type_param->enums[33].value_from_mapping = 41;
    cur_field_type_param->enums[34].value_from_mapping = 49;
    cur_field_type_param->enums[35].value_from_mapping = 33;
    cur_field_type_param->enums[36].value_from_mapping = 45;
    cur_field_type_param->enums[37].value_from_mapping = 28;
    cur_field_type_param->enums[38].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___195_get(unit);
    cur_field_type_param->enums[39].value_from_mapping = 39;
    cur_field_type_param->enums[40].value_from_mapping = 29;
    cur_field_type_param->enums[41].value_from_mapping = 38;
    cur_field_type_param->enums[42].value_from_mapping = 27;
    cur_field_type_param->enums[43].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___196_get(unit);
    cur_field_type_param->enums[44].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___197_get(unit);
    cur_field_type_param->enums[45].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___198_get(unit);
    cur_field_type_param->enums[46].value_from_mapping = 26;
    cur_field_type_param->enums[47].value_from_mapping = 18;
    cur_field_type_param->enums[48].value_from_mapping = 36;
    cur_field_type_param->enums[49].value_from_mapping = 62;
    cur_field_type_param->enums[50].value_from_mapping = 15;
    cur_field_type_param->enums[51].value_from_mapping = 0;
    cur_field_type_param->enums[52].value_from_mapping = 21;
    cur_field_type_param->enums[53].value_from_mapping = 48;
    cur_field_type_param->enums[54].value_from_mapping = 52;
    cur_field_type_param->enums[55].value_from_mapping = 51;
    cur_field_type_param->enums[56].value_from_mapping = 53;
    cur_field_type_param->enums[57].value_from_mapping = 54;
    cur_field_type_param->enums[58].value_from_mapping = 43;
    cur_field_type_param->enums[59].value_from_mapping = 59;
    cur_field_type_param->enums[60].value_from_mapping = 42;
    cur_field_type_param->enums[61].value_from_mapping = 16;
    cur_field_type_param->enums[62].value_from_mapping = 40;
    cur_field_type_param->enums[63].value_from_mapping = 14;
    cur_field_type_param->enums[64].value_from_mapping = 57;
    cur_field_type_param->enums[65].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___199_get(unit);
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_FEC_ENTRY_FORMAT" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 9 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "FEC_ENTRY_DESTINATION", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FEC_ENTRY_DESTINATION_EEI", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "FEC_ENTRY_DESTINATION_LIF0", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FEC_ENTRY_DESTINATION_LIF0_HTM", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FEC_ENTRY_DESTINATION_LIF0_MC_RPF", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "FEC_ENTRY_JR1_DESTINATION_EEI", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "FEC_ENTRY_JR1_DESTINATION_ETH_RIF_ARP", sizeof(cur_field_type_param->enums[8].name_from_interface));
    cur_field_type_param->nof_enum_vals = 9;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    cur_field_type_param->enums[2].value_from_mapping = 21;
    cur_field_type_param->enums[3].value_from_mapping = 11;
    cur_field_type_param->enums[4].value_from_mapping = 39;
    cur_field_type_param->enums[5].value_from_mapping = 4;
    cur_field_type_param->enums[6].value_from_mapping = 3;
    cur_field_type_param->enums[7].value_from_mapping = 3;
    cur_field_type_param->enums[8].value_from_mapping = 20;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_FEC_SUPER_ENTRY_FORMAT" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 4 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "SUPER_FEC_NO_PROTECTION", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "SUPER_FEC_NO_PROTECTION_W_2_STAT", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "SUPER_FEC_W_PROTECTION", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "SUPER_FEC_W_PROTECTION_W_1_STAT", sizeof(cur_field_type_param->enums[3].name_from_interface));
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
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_irpp_fwd_stage_floating_formats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_FWD_STAGE_FLOATING_FORMATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_FWD_STAGE_FLOATING_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 8 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "COMPRESSED_SOURCE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "COMPRESSED_SOURCE_AND_INTERFACE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "EM_PMF_ADDITIONAL_DATA_RESULT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FWD_NAT_TRANSLATION_RESULT", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "KBP_FWD_NAT_TRANSLATION_RESULT", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "LEARN_DESTINATION", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "TCAM_RESULT_VRF", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[7].name_from_interface));
    cur_field_type_param->nof_enum_vals = 8;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 2;
    cur_field_type_param->enums[2].value_from_mapping = 3;
    cur_field_type_param->enums[3].value_from_mapping = 4;
    cur_field_type_param->enums[4].value_from_mapping = 5;
    cur_field_type_param->enums[5].value_from_mapping = 6;
    cur_field_type_param->enums[6].value_from_mapping = 7;
    cur_field_type_param->enums[7].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_FWD_STAGE_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 33 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "FWD_DEST", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FWD_DEST_DOUBLE_OUTLIF", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "FWD_DEST_DOUBLE_OUTLIF_STAT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "FWD_DEST_EEI", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "FWD_DEST_EEI_STAT", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "FWD_DEST_OUTLIF", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "FWD_DEST_OUTLIF_STAT", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "FWD_DEST_OUTLIF_STAT_W_DEFAULT", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "FWD_DEST_OUTLIF_W_DEFAULT", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "FWD_DEST_STAT", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "FWD_DEST_STAT_W_DEFAULT", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "FWD_DEST_W_DEFAULT", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "FWD_MACT_RESULT_DEST_LIF_STAT", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "FWD_MACT_RESULT_DEST_STAT", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "FWD_MACT_RESULT_DOUBLE_OUTLIF", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "FWD_MACT_RESULT_EEI_FEC", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "FWD_MACT_RESULT_NO_OUTLIF", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "FWD_MACT_RESULT_NO_OUTLIF_NO_AUTO_LEARN", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "FWD_MACT_RESULT_SINGLE_OUTLIF", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "FWD_NO_ACTION", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "KAPS_FEC", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "KAPS_FEC_2", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "KAPS_FEC_3", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "KAPS_FEC_4", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "KAPS_FEC_5", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "KAPS_FEC_6", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "KAPS_FEC_DEFAULT", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "KAPS_MC_ID", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "KBP_DEST_W_DEFAULT", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "KBP_FORWARD_DEST_EEI_W_DEFAULT", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "KBP_FORWARD_DEST_STAT_W_DEFAULT", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "KBP_FORWARD_OUTLIF_W_DEFAULT", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "KBP_RPF_DEST_W_DEFAULT", sizeof(cur_field_type_param->enums[32].name_from_interface));
    cur_field_type_param->nof_enum_vals = 33;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 10;
    cur_field_type_param->enums[1].value_from_mapping = 7;
    cur_field_type_param->enums[2].value_from_mapping = 8;
    cur_field_type_param->enums[3].value_from_mapping = 3;
    cur_field_type_param->enums[4].value_from_mapping = 4;
    cur_field_type_param->enums[5].value_from_mapping = 5;
    cur_field_type_param->enums[6].value_from_mapping = 6;
    cur_field_type_param->enums[7].value_from_mapping = 9;
    cur_field_type_param->enums[8].value_from_mapping = 11;
    cur_field_type_param->enums[9].value_from_mapping = 2;
    cur_field_type_param->enums[10].value_from_mapping = 13;
    cur_field_type_param->enums[11].value_from_mapping = 1;
    cur_field_type_param->enums[12].value_from_mapping = 50;
    cur_field_type_param->enums[13].value_from_mapping = 49;
    cur_field_type_param->enums[14].value_from_mapping = 12;
    cur_field_type_param->enums[15].value_from_mapping = 40;
    cur_field_type_param->enums[16].value_from_mapping = 16;
    cur_field_type_param->enums[17].value_from_mapping = 23;
    cur_field_type_param->enums[18].value_from_mapping = 48;
    cur_field_type_param->enums[19].value_from_mapping = 55;
    cur_field_type_param->enums[20].value_from_mapping = 0;
    cur_field_type_param->enums[21].value_from_mapping = 1;
    cur_field_type_param->enums[22].value_from_mapping = 2;
    cur_field_type_param->enums[23].value_from_mapping = 3;
    cur_field_type_param->enums[24].value_from_mapping = 4;
    cur_field_type_param->enums[25].value_from_mapping = 5;
    cur_field_type_param->enums[26].value_from_mapping = 6;
    cur_field_type_param->enums[27].value_from_mapping = 7;
    cur_field_type_param->enums[28].value_from_mapping = 0;
    cur_field_type_param->enums[29].value_from_mapping = 17;
    cur_field_type_param->enums[30].value_from_mapping = 14;
    cur_field_type_param->enums[31].value_from_mapping = 15;
    cur_field_type_param->enums[32].value_from_mapping = 18;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_IN_LIF_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 41 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "IN_ETH_AC_MP", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "IN_ETH_AC_VSI_P2P", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "IN_ETH_VLAN_EDIT_ONLY", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "IN_ETH_VLAN_EDIT_VSI_MP", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "IN_LIF_AC2EEI", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "IN_LIF_AC2EEI_LARGE_GENERIC_DATA", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "IN_LIF_AC_MP", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "IN_LIF_AC_MP_LARGE", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "IN_LIF_AC_MP_LARGE_GENERIC_DATA", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "IN_LIF_AC_P2P", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "IN_LIF_AC_P2P_LARGE", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "IN_LIF_AC_P2P_LARGE_GENERIC_DATA", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "IN_LIF_AC_P2P_W_1_VLAN", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "IN_LIF_AC_P2P_W_VSI_WO_PROTECTION", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "IN_LIF_AC_P2P_W_VSI_W_1_VLAN", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "IN_LIF_BIER_MPLS", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "IN_LIF_BIER_TI", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "IN_LIF_DROPPED_COPY", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "IN_LIF_DUMMY", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "IN_LIF_DUMMY_WITH_DESTINATION", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "IN_LIF_EVPN_EVI_MP", sizeof(cur_field_type_param->enums[21].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[22].name_from_interface, "IN_LIF_EVPN_EVI_P2P_NO_LEARNING", sizeof(cur_field_type_param->enums[22].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[23].name_from_interface, "IN_LIF_IPVX_1", sizeof(cur_field_type_param->enums[23].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[24].name_from_interface, "IN_LIF_IPVX_2", sizeof(cur_field_type_param->enums[24].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[25].name_from_interface, "IN_LIF_IPVX_3", sizeof(cur_field_type_param->enums[25].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[26].name_from_interface, "IN_LIF_ISID_MP", sizeof(cur_field_type_param->enums[26].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[27].name_from_interface, "IN_LIF_ISID_P2P", sizeof(cur_field_type_param->enums[27].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[28].name_from_interface, "IN_LIF_LSP", sizeof(cur_field_type_param->enums[28].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[29].name_from_interface, "IN_LIF_MIM_TUNNEL", sizeof(cur_field_type_param->enums[29].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[30].name_from_interface, "IN_LIF_MPLS_DUMMY", sizeof(cur_field_type_param->enums[30].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[31].name_from_interface, "IN_LIF_MPLS_DUMMY_WITH_PARSER", sizeof(cur_field_type_param->enums[31].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[32].name_from_interface, "IN_LIF_PWE2EEI_NO_LEARNING", sizeof(cur_field_type_param->enums[32].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[33].name_from_interface, "IN_LIF_PWE_MP", sizeof(cur_field_type_param->enums[33].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[34].name_from_interface, "IN_LIF_PWE_P2P_NO_LEARNING", sizeof(cur_field_type_param->enums[34].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[35].name_from_interface, "IN_LIF_PWE_P2P_NO_LEARNING_OPTIMIZED", sizeof(cur_field_type_param->enums[35].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[36].name_from_interface, "IN_LIF_RCH_CONTROL_LIF", sizeof(cur_field_type_param->enums[36].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[37].name_from_interface, "IN_LIF_SRV6_TERMINATED", sizeof(cur_field_type_param->enums[37].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[38].name_from_interface, "IN_LIF_TUNNEL_W_L3_PAYLOAD_LARGE_W_GENERIC_DATA", sizeof(cur_field_type_param->enums[38].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[39].name_from_interface, "IN_LIF_TUNNEL_W_L3_PAYLOAD_MP_FORWARD_W_GENERIC_DATA", sizeof(cur_field_type_param->enums[39].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[40].name_from_interface, "IN_LIF_TUNNEL_W_L3_PAYLOAD_P2P_FORWARD_TO_FEC_W_GENERIC_DATA", sizeof(cur_field_type_param->enums[40].name_from_interface));
    cur_field_type_param->nof_enum_vals = 41;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 50;
    cur_field_type_param->enums[1].value_from_mapping = 41;
    cur_field_type_param->enums[2].value_from_mapping = 40;
    cur_field_type_param->enums[3].value_from_mapping = 39;
    cur_field_type_param->enums[4].value_from_mapping = 2;
    cur_field_type_param->enums[5].value_from_mapping = 4;
    cur_field_type_param->enums[6].value_from_mapping = 1;
    cur_field_type_param->enums[7].value_from_mapping = 60;
    cur_field_type_param->enums[8].value_from_mapping = 4;
    cur_field_type_param->enums[9].value_from_mapping = 6;
    cur_field_type_param->enums[10].value_from_mapping = 0;
    cur_field_type_param->enums[11].value_from_mapping = 61;
    cur_field_type_param->enums[12].value_from_mapping = 5;
    cur_field_type_param->enums[13].value_from_mapping = 1;
    cur_field_type_param->enums[14].value_from_mapping = 58;
    cur_field_type_param->enums[15].value_from_mapping = 57;
    cur_field_type_param->enums[16].value_from_mapping = 35;
    cur_field_type_param->enums[17].value_from_mapping = 43;
    cur_field_type_param->enums[18].value_from_mapping = 34;
    cur_field_type_param->enums[19].value_from_mapping = 44;
    cur_field_type_param->enums[20].value_from_mapping = 7;
    cur_field_type_param->enums[21].value_from_mapping = 51;
    cur_field_type_param->enums[22].value_from_mapping = 52;
    cur_field_type_param->enums[23].value_from_mapping = 47;
    cur_field_type_param->enums[24].value_from_mapping = 48;
    cur_field_type_param->enums[25].value_from_mapping = 49;
    cur_field_type_param->enums[26].value_from_mapping = 54;
    cur_field_type_param->enums[27].value_from_mapping = 55;
    cur_field_type_param->enums[28].value_from_mapping = 33;
    cur_field_type_param->enums[29].value_from_mapping = 53;
    cur_field_type_param->enums[30].value_from_mapping = 42;
    cur_field_type_param->enums[31].value_from_mapping = 62;
    cur_field_type_param->enums[32].value_from_mapping = 3;
    cur_field_type_param->enums[33].value_from_mapping = 36;
    cur_field_type_param->enums[34].value_from_mapping = 37;
    cur_field_type_param->enums[35].value_from_mapping = 46;
    cur_field_type_param->enums[36].value_from_mapping = 15;
    cur_field_type_param->enums[37].value_from_mapping = 32;
    cur_field_type_param->enums[38].value_from_mapping = 45;
    cur_field_type_param->enums[39].value_from_mapping = 59;
    cur_field_type_param->enums[40].value_from_mapping = 56;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_ISEM_ACCESS_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 6 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "COMPRESSED_MAC_ID", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "FORWARD_DOMAIN", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "INLIF_INDEX", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "IPV6_ADDRESS_MSBS_ID", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "IPV6_MP_TUNNEL_IDX", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[5].name_from_interface));
    cur_field_type_param->nof_enum_vals = 6;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 2;
    cur_field_type_param->enums[2].value_from_mapping = 3;
    cur_field_type_param->enums[3].value_from_mapping = 4;
    cur_field_type_param->enums[4].value_from_mapping = 5;
    cur_field_type_param->enums[5].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_MY_MAC_EXEM_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "MY_MAC_EXEM_RESULT", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_VSI_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 3 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "VSI_ENTRY_BASIC", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "VSI_ENTRY_BASIC_NON_MC_DESTINATION", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "VSI_ENTRY_WITH_STATS", sizeof(cur_field_type_param->enums[2].name_from_interface));
    cur_field_type_param->nof_enum_vals = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 2;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    cur_field_type_param->enums[2].value_from_mapping = 3;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IRPP_VTT_TCAM_ACCESS_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "FOUND", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAMP_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 22 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "MEP_DB_BFD_ON_IPV4_MULTI_HOP_SHORT_STATIC", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "MEP_DB_BFD_ON_IPV4_ONE_HOP_SHORT_STATIC", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "MEP_DB_BFD_ON_MPLS_SHORT_STATIC", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "MEP_DB_BFD_ON_PWE_SHORT_STATIC", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "MEP_DB_BFD_ON_PWE_STATIC_PART_1", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "MEP_DB_BFD_ON_PWE_STATIC_PART_2", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "MEP_DB_CCM_ETH_SHORT_STATIC", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "MEP_DB_CCM_ETH_STATIC_PART_1_DOWNMEP", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "MEP_DB_CCM_ETH_STATIC_PART_1_UPMEP", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "MEP_DB_CCM_ETH_STATIC_PART_2", sizeof(cur_field_type_param->enums[9].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[10].name_from_interface, "MEP_DB_EXTRA_DATA_IN_MDB_HEADER", sizeof(cur_field_type_param->enums[10].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[11].name_from_interface, "MEP_DB_EXTRA_DATA_IN_MDB_PAYLOAD", sizeof(cur_field_type_param->enums[11].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[12].name_from_interface, "MEP_DB_Y1731_ON_MPLS_TP_SHORT_STATIC", sizeof(cur_field_type_param->enums[12].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[13].name_from_interface, "MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_1", sizeof(cur_field_type_param->enums[13].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[14].name_from_interface, "MEP_DB_Y1731_ON_MPLS_TP_STATIC_PART_2", sizeof(cur_field_type_param->enums[14].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[15].name_from_interface, "MEP_DB_Y1731_ON_PWE_SHORT_STATIC", sizeof(cur_field_type_param->enums[15].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[16].name_from_interface, "MEP_DB_Y1731_ON_PWE_STATIC_PART_1", sizeof(cur_field_type_param->enums[16].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[17].name_from_interface, "MEP_DB_Y1731_ON_PWE_STATIC_PART_2", sizeof(cur_field_type_param->enums[17].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[18].name_from_interface, "OAMP_LM_SESSION_ID", sizeof(cur_field_type_param->enums[18].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[19].name_from_interface, "OAMP_RMEP_INDEX", sizeof(cur_field_type_param->enums[19].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[20].name_from_interface, "OAMP_RMEP_STATIC_DATA", sizeof(cur_field_type_param->enums[20].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[21].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[21].name_from_interface));
    cur_field_type_param->nof_enum_vals = 22;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 2;
    cur_field_type_param->enums[2].value_from_mapping = 3;
    cur_field_type_param->enums[3].value_from_mapping = 4;
    cur_field_type_param->enums[4].value_from_mapping = 5;
    cur_field_type_param->enums[5].value_from_mapping = 6;
    cur_field_type_param->enums[6].value_from_mapping = 7;
    cur_field_type_param->enums[7].value_from_mapping = 8;
    cur_field_type_param->enums[8].value_from_mapping = 9;
    cur_field_type_param->enums[9].value_from_mapping = 10;
    cur_field_type_param->enums[10].value_from_mapping = 11;
    cur_field_type_param->enums[11].value_from_mapping = 12;
    cur_field_type_param->enums[12].value_from_mapping = 13;
    cur_field_type_param->enums[13].value_from_mapping = 14;
    cur_field_type_param->enums[14].value_from_mapping = 15;
    cur_field_type_param->enums[15].value_from_mapping = 16;
    cur_field_type_param->enums[16].value_from_mapping = 17;
    cur_field_type_param->enums[17].value_from_mapping = 18;
    cur_field_type_param->enums[18].value_from_mapping = 19;
    cur_field_type_param->enums[19].value_from_mapping = 20;
    cur_field_type_param->enums[20].value_from_mapping = 21;
    cur_field_type_param->enums[21].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "OAM_CLASSIFIER_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 3 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "OAM_ACC_MEP_DB", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "OAM_LIF_DB", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[2].name_from_interface));
    cur_field_type_param->nof_enum_vals = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 2;
    cur_field_type_param->enums[2].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PRT_VP_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "IN_PP_PORT_INDEX_RESULT_TYPE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "TCAM_IDENTIFICATION_FORMATS" , 6 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "OAM_IDENTIFICATION", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 1;
    cur_field_type_param->enums[1].value_from_mapping = 0;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EEDB_TYPE_EM_BUSTER" , dnx_data_pp.ETPP.etps_type_size_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 10 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ETPS_AC", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ETPS_AC_QOS_OAM", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "ETPS_AC_STAT", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "ETPS_AC_STAT_PROTECTION", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "ETPS_AC_TRIPLE_TAG", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "ETPS_ARP_AC", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "ETPS_ARP_AC_1TAG_STAT", sizeof(cur_field_type_param->enums[7].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[8].name_from_interface, "ETPS_L2_FODO_VLANS", sizeof(cur_field_type_param->enums[8].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[9].name_from_interface, "ETPS_RSPAN", sizeof(cur_field_type_param->enums[9].name_from_interface));
    cur_field_type_param->nof_enum_vals = 10;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 11;
    cur_field_type_param->enums[2].value_from_mapping = 12;
    cur_field_type_param->enums[3].value_from_mapping = 9;
    cur_field_type_param->enums[4].value_from_mapping = 8;
    cur_field_type_param->enums[5].value_from_mapping = 10;
    cur_field_type_param->enums[6].value_from_mapping = 4;
    cur_field_type_param->enums[7].value_from_mapping = 6;
    cur_field_type_param->enums[8].value_from_mapping = 33;
    cur_field_type_param->enums[9].value_from_mapping = 43;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EEDB_TYPE_VSD_ENABLE" , dnx_data_pp.ETPP.etps_type_size_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 8 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ETPS_ARP_AC", sizeof(cur_field_type_param->enums[1].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[2].name_from_interface, "ETPS_ARP_AC_1TAG_STAT", sizeof(cur_field_type_param->enums[2].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[3].name_from_interface, "ETPS_ARP_PLUS_AC_1TAG", sizeof(cur_field_type_param->enums[3].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[4].name_from_interface, "ETPS_ARP_SA_CUSTOM", sizeof(cur_field_type_param->enums[4].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[5].name_from_interface, "ETPS_ARP_SA_TUNNELING", sizeof(cur_field_type_param->enums[5].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[6].name_from_interface, "ETPS_ETH_RIF", sizeof(cur_field_type_param->enums[6].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[7].name_from_interface, "ETPS_ETH_RIF_STAT", sizeof(cur_field_type_param->enums[7].name_from_interface));
    cur_field_type_param->nof_enum_vals = 8;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 4;
    cur_field_type_param->enums[2].value_from_mapping = 6;
    cur_field_type_param->enums[3].value_from_mapping = 47;
    cur_field_type_param->enums[4].value_from_mapping = 5;
    cur_field_type_param->enums[5].value_from_mapping = 7;
    cur_field_type_param->enums[6].value_from_mapping = 1;
    cur_field_type_param->enums[7].value_from_mapping = 3;
    
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EEDB_TYPE_SVTAG_ENABLE" , dnx_data_pp.ETPP.etps_type_size_bits_get(unit) , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "NONE", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "ETPS_SVTAG", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = dnx_data_mdb_app_db.fields.numeric_mdb_field___200_get(unit);
    
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
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_prt_vp_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_tcam_identification_formats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_em_buster_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_vsd_enable_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_formats_enum_encoding_eedb_type_svtag_enable_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
