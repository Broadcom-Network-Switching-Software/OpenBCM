
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ac_inlif_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_AC_INLIF_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "AC_INLIF_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_BRIDGE_NAMESPACE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_IS_ON_LAG;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_IP_ADDRESS_SPOOFING_CHECK_MODE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_RESERVED_AG;
    cur_field_types_info->struct_field_info[3].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_appdb_label_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_APPDB_LABEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "APPDB_LABEL", 10, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_APPDB_LABEL_MS_MACSEC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_NONE_1, "NONE_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_NONE_2, "NONE_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SYSTEM, "SYSTEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_EGQ, "EGQ");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_ESB, "ESB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_IQS, "IQS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_FAILOVER, "FAILOVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_L2, "L2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_L3, "L3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_LIF, "LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_FCOE, "FCOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SNIF, "SNIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_ECGM, "ECGM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_ICGM, "ICGM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SCHEDULER, "SCHEDULER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_ITM, "ITM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_ETM, "ETM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_STACK, "STACK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_LAG, "LAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MULTICAST, "MULTICAST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_FABRIC, "FABRIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_NIF, "NIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_FC, "FC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_CRPS, "CRPS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_PTP, "PTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MIRROR, "MIRROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SNOOP, "SNOOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_OAM, "OAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_BFD, "BFD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_PARSER, "PARSER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_VSWITCH, "VSWITCH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_PP_PORT, "PP_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_VLAN_TRANSLATION, "VLAN_TRANSLATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_VLAN, "VLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_L2_LEARNING, "L2_LEARNING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_VPLS, "VPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_VPWS, "VPWS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_EVPN, "EVPN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_TRILL, "TRILL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_L2GRE, "L2GRE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MIN, "MIN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_VXLAN, "VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_ROO, "ROO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_HASHING, "HASHING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_QOS, "QOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_QOS_PHB, "QOS_PHB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_QOS_REMARKING, "QOS_REMARKING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_QOS_ECN, "QOS_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_QOS_TTL, "QOS_TTL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_STG, "STG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SIT, "SIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_L3_IF_RIF, "L3_IF_RIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_TRAP, "TRAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_FIELD, "FIELD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_TUNNEL, "TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_METER, "METER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SYNCE, "SYNCE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SAT, "SAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SVTAG, "SVTAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_EGW, "EGW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_KBP, "KBP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_PLL, "PLL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_RCPU, "RCPU");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_DRAM, "DRAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_STIF, "STIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MIB, "MIB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_PP_STATISTICS, "PP_STATISTICS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_INSTRU, "INSTRU");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SFLOW, "SFLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_DIAGNOSTICS, "DIAGNOSTICS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_LATENCY, "LATENCY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_MIDSTACK, "DRAM_PHY_MIDSTACK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_DWORD, "DRAM_PHY_DWORD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_AWORD, "DRAM_PHY_AWORD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_CONTROL, "DRAM_PHY_CONTROL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_EEDB, "EEDB");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_KBP_FWD, "KBP_FWD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_KBP_MNGM, "KBP_MNGM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_IPMC, "IPMC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_STAT, "STAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_TDM, "TDM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_J2P_NOTREV, "J2P_NOTREV");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_TINY_MAC, "TINY_MAC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_SEC, "SEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MACSEC_IP, "MACSEC_IP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_APPDB_LABEL_MS_MACSEC, "MS_MACSEC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_NONE_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_NONE_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SYSTEM].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_EGQ].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_ESB].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_IQS].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_FAILOVER].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_L2].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_L3].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_LIF].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MPLS].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_FCOE].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SNIF].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_ECGM].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_ICGM].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SCHEDULER].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_ITM].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_ETM].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_STACK].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_LAG].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MULTICAST].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_FABRIC].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_NIF].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_FC].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_CRPS].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_PTP].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MIRROR].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SNOOP].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_OAM].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_BFD].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_PARSER].value_from_mapping = 31;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_VSWITCH].value_from_mapping = 32;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_PP_PORT].value_from_mapping = 33;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_VLAN_TRANSLATION].value_from_mapping = 34;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_VLAN].value_from_mapping = 35;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_L2_LEARNING].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_VPLS].value_from_mapping = 37;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_VPWS].value_from_mapping = 38;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_EVPN].value_from_mapping = 39;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_TRILL].value_from_mapping = 40;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_L2GRE].value_from_mapping = 41;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MIN].value_from_mapping = 42;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_VXLAN].value_from_mapping = 43;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_ROO].value_from_mapping = 44;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_HASHING].value_from_mapping = 45;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_QOS].value_from_mapping = 46;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_QOS_PHB].value_from_mapping = 47;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_QOS_REMARKING].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_QOS_ECN].value_from_mapping = 49;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_QOS_TTL].value_from_mapping = 50;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_STG].value_from_mapping = 51;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SIT].value_from_mapping = 52;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_L3_IF_RIF].value_from_mapping = 53;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_TRAP].value_from_mapping = 54;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_FIELD].value_from_mapping = 55;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_TUNNEL].value_from_mapping = 56;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_METER].value_from_mapping = 57;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SYNCE].value_from_mapping = 58;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SAT].value_from_mapping = 59;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SVTAG].value_from_mapping = 60;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_EGW].value_from_mapping = 61;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_KBP].value_from_mapping = 62;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_PLL].value_from_mapping = 63;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_RCPU].value_from_mapping = 64;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_DRAM].value_from_mapping = 65;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_STIF].value_from_mapping = 66;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MIB].value_from_mapping = 67;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_PP_STATISTICS].value_from_mapping = 68;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_INSTRU].value_from_mapping = 69;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SFLOW].value_from_mapping = 70;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_DIAGNOSTICS].value_from_mapping = 71;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_LATENCY].value_from_mapping = 72;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_MIDSTACK].value_from_mapping = 73;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_DWORD].value_from_mapping = 74;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_AWORD].value_from_mapping = 75;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_DRAM_PHY_CONTROL].value_from_mapping = 76;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_EEDB].value_from_mapping = 77;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_KBP_FWD].value_from_mapping = 78;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_KBP_MNGM].value_from_mapping = 79;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_IPMC].value_from_mapping = 80;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_STAT].value_from_mapping = 81;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_TDM].value_from_mapping = 82;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_J2P_NOTREV].value_from_mapping = 83;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_TINY_MAC].value_from_mapping = 84;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_SEC].value_from_mapping = 85;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MACSEC_IP].value_from_mapping = 86;
        enum_info[DBAL_ENUM_FVAL_APPDB_LABEL_MS_MACSEC].value_from_mapping = 87;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ase_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ASE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ASE_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ASE_TYPE_PTP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ASE_TYPE_OTHER, "OTHER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ASE_TYPE_OAM, "OAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ASE_TYPE_PTP, "PTP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ASE_TYPE_OTHER].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ASE_TYPE_OAM].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ASE_TYPE_PTP].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_bridging_fwd12fwd2_inlif_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_BRIDGING_FWD12FWD2_INLIF_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "BRIDGING_FWD12FWD2_INLIF_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_EXTENDED_P2P_NOF_SD_TAGS;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_RESERVED_AG;
    cur_field_types_info->struct_field_info[1].length = 2;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ctx_additional_header_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CTX_ADDITIONAL_HEADER_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CTX_ADDITIONAL_HEADER_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_IOAM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE, "CTX_AHP_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE, "CTX_AHP_GRE_OR_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP, "CTX_AHP_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE, "CTX_AHP_VXLAN_OR_VXLAN_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH, "CTX_MPLS_AH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI, "CTX_MPLS_AH_ESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP, "CTX_AHP_IP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_DOUBLE_UDP, "CTX_AHP_VXLAN_DOUBLE_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_IOAM, "CTX_MPLS_AH_IOAM");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_GRE_OR_GENEVE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_UDP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_OR_VXLAN_GPE].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_ESI].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_IP].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_AHP_VXLAN_DOUBLE_UDP].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_CTX_ADDITIONAL_HEADER_PROFILE_CTX_MPLS_AH_IOAM].value_from_mapping = 8;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_current_protocol_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_CURRENT_PROTOCOL_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "CURRENT_PROTOCOL_TYPE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GENEVE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC, "IPV4_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC, "IPV6_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS_UA, "MPLS_UA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ARP, "ARP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FCOE, "FCOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC, "IPV4_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC, "IPV6_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_L2TP, "L2TP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PTP, "PTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SFLOW, "SFLOW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_BIER_NON_MPLS, "BIER_NON_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_BIER_MPLS, "BIER_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PPPOE_SESSION, "PPPOE_SESSION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT, "SRV6_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_TDM_BS, "TDM_BS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_SRH_ONLY, "SRV6_SRH_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN, "VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_INGRESS_SCTP_EGRESS_FTMH, "INGRESS_SCTP_EGRESS_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GTP, "GTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV2, "ERSPANV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV3, "ERSPANV3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE, "GRE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN_GPE, "VXLAN_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UNKNOWN, "UNKNOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GENEVE, "GENEVE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ETHERNET].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_UC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_UC].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_MPLS_UA].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ARP].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_FCOE].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UDP].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV4_MC].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_IPV6_MC].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_L2TP].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PTP].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SFLOW].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_BIER_NON_MPLS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_BIER_MPLS].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_PPPOE_SESSION].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_ENDPOINT].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_TDM_BS].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_SRV6_SRH_ONLY].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_INGRESS_SCTP_EGRESS_FTMH].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GTP].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV2].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_ERSPANV3].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GRE].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_VXLAN_GPE].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_UNKNOWN].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_CURRENT_PROTOCOL_TYPE_GENEVE].value_from_mapping = 31;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_egress_1st_parser_parser_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_1ST_PARSER_PARSER_CONTEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_1ST_PARSER_PARSER_CONTEXT", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_TDM_BS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_UNKNOWN, "UNKNOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_ETH_A1, "ETH_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_TM, "TM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV4_A1, "IPV4_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV6_A1, "IPV6_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6ENDPOINT_A1, "SRV6ENDPOINT_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6_BEYOND_A1, "SRV6_BEYOND_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_BIER_MPLS_A1, "BIER_MPLS_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_BIER_NON_MPLS_A1, "BIER_NON_MPLS_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_FTMH_A1, "FTMH_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_PPP_A, "PPP_A");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV4_12B_A1, "IPV4_12B_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV6_8B_A1, "IPV6_8B_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_MPLS_A1, "MPLS_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_DUMMY_ETH, "DUMMY_ETH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_RCH_EXTENDED_ENCAP, "RCH_EXTENDED_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6_PSP_EXTENDED_TERMINATION, "SRV6_PSP_EXTENDED_TERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6ENDPOINT_PSP_A1, "SRV6ENDPOINT_PSP_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_MPLS_DUMMY, "MPLS_DUMMY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_FORCE_MIRROR_OR_SS, "FORCE_MIRROR_OR_SS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_TDM_BS, "TDM_BS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_UNKNOWN].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_ETH_A1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_TM].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV4_A1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV6_A1].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6ENDPOINT_A1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6_BEYOND_A1].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_BIER_MPLS_A1].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_BIER_NON_MPLS_A1].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_FTMH_A1].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_PPP_A].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV4_12B_A1].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_IPV6_8B_A1].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_MPLS_A1].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_DUMMY_ETH].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_RCH_EXTENDED_ENCAP].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6_PSP_EXTENDED_TERMINATION].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_SRV6ENDPOINT_PSP_A1].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_MPLS_DUMMY].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_FORCE_MIRROR_OR_SS].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_EGRESS_1ST_PARSER_PARSER_CONTEXT_TDM_BS].value_from_mapping = 21;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_egress_fwd_action_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_FWD_ACTION_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_FWD_ACTION_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_4 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_DROP, "DROP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_GENERAL_TRAP, "GENERAL_TRAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_1, "USER_TRAP_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_2, "USER_TRAP_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_3, "USER_TRAP_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_4, "USER_TRAP_4");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_DROP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_GENERAL_TRAP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_3].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_ACTION_PROFILE_USER_TRAP_4].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_egress_fwd_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EGRESS_FWD_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EGRESS_FWD_CODE", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MIRROR_OR_SS, "MIRROR_OR_SS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0, "IPV4_UC_R0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0, "IPV4_MC_R0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0, "IPV6_UC_R0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0, "IPV6_MC_R0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_MPLS_BFR, "BIER_MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_NON_MPLS_BFR, "BIER_NON_MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_INGRESS_TRAPPED_JR1_LEGACY, "INGRESS_TRAPPED_JR1_LEGACY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RAW_PROCESSING, "RAW_PROCESSING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_CPU_PORT, "CPU_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ERPP_TRAPPED, "ERPP_TRAPPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_STACKING_PORT, "STACKING_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT, "SRV6_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP, "SRV6_ENDPOINT_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP_EXTENDED_CUT, "SRV6_ENDPOINT_PSP_EXTENDED_CUT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TM, "TM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_HBH_PT_MIDPOINT, "SRV6_ENDPOINT_HBH_PT_MIDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TDM, "TDM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS_INJECTED_FROM_OAMP, "MPLS_INJECTED_FROM_OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_DO_NOT_EDIT, "DO_NOT_EDIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PPP, "PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1, "IPV4_UC_R1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1, "IPV4_MC_R1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1, "IPV6_UC_R1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1, "IPV6_MC_R1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6__HBH_PT_MIDPOINT_UC_R0, "IPV6__HBH_PT_MIDPOINT_UC_R0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6__HBH_PT_MIDPOINT_UC_R1, "IPV6__HBH_PT_MIDPOINT_UC_R1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_DROP_AND_CONT, "RCH_DROP_AND_CONT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC, "RCH_PTCH_ENC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_EXT_ENCAP, "RCH_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_XTERMINATION, "RCH_XTERMINATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_VRF_REDIRECT, "RCH_VRF_REDIRECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_EXT_ENCAP_DO_NOTHING, "EXT_ENCAP_DO_NOTHING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_SRV6_USP_PSP, "RCH_SRV6_USP_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPFIX_RX_FWD_CODE, "IPFIX_RX_FWD_CODE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TDM_BS, "TDM_BS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE, "FWD_CODE_BUG_CODE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MIRROR_OR_SS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_MPLS_BFR].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_NON_MPLS_BFR].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_INGRESS_TRAPPED_JR1_LEGACY].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RAW_PROCESSING].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_CPU_PORT].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ERPP_TRAPPED].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_STACKING_PORT].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_PSP_EXTENDED_CUT].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TM].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_SRV6_ENDPOINT_HBH_PT_MIDPOINT].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TDM].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS_INJECTED_FROM_OAMP].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_DO_NOT_EDIT].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_PPP].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1].value_from_mapping = 34;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1].value_from_mapping = 35;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1].value_from_mapping = 36;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1].value_from_mapping = 37;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6__HBH_PT_MIDPOINT_UC_R0].value_from_mapping = 38;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6__HBH_PT_MIDPOINT_UC_R1].value_from_mapping = 39;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_DROP_AND_CONT].value_from_mapping = 48;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_PTCH_ENC].value_from_mapping = 49;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_EXT_ENCAP].value_from_mapping = 50;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_XTERMINATION].value_from_mapping = 51;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_VRF_REDIRECT].value_from_mapping = 52;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_EXT_ENCAP_DO_NOTHING].value_from_mapping = 53;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_RCH_SRV6_USP_PSP].value_from_mapping = 54;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPFIX_RX_FWD_CODE].value_from_mapping = 55;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_TDM_BS].value_from_mapping = 56;
        enum_info[DBAL_ENUM_FVAL_EGRESS_FWD_CODE_FWD_CODE_BUG_CODE].value_from_mapping = 63;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_erpp_ebtr_terminate_network_headers_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE_NEVER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE_ALWAYS, "ALWAYS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE_NEVER, "NEVER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE_ALWAYS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_NETWORK_HEADERS_MODE_NEVER].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_erpp_ebtr_terminate_system_headers_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE_NEVER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE_ALWAYS, "ALWAYS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE_NEVER, "NEVER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE_ALWAYS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ERPP_EBTR_TERMINATE_SYSTEM_HEADERS_MODE_NEVER].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_esem1_default_result_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ESEM1_DEFAULT_RESULT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ESEM1_DEFAULT_RESULT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_RSPAN_ADVANCED_NO_SSPA + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_GENEVE_NO_FODO2VNI_MAPPING, "IPV6_GENEVE_NO_FODO2VNI_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_VXLAN_NO_FODO2VNI_MAPPING, "IPV6_VXLAN_NO_FODO2VNI_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_VXLAN_GPE_NO_FODO2VNI_MAPPING, "IPV6_VXLAN_GPE_NO_FODO2VNI_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_GRE_NO_FODO2VNI_MAPPING, "IPV6_GRE_NO_FODO2VNI_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV4_VXLAN_NO_FODO2VNI_MAPPING, "IPV4_VXLAN_NO_FODO2VNI_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV4_VXLAN_GPE_NO_FODO2VNI_MAPPING, "IPV4_VXLAN_GPE_NO_FODO2VNI_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IML_NO_FODO_OUTLIF_MAP, "IML_NO_FODO_OUTLIF_MAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_RSPAN_ADVANCED_NO_SSPA, "RSPAN_ADVANCED_NO_SSPA");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_GENEVE_NO_FODO2VNI_MAPPING].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_VXLAN_NO_FODO2VNI_MAPPING].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_VXLAN_GPE_NO_FODO2VNI_MAPPING].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV6_GRE_NO_FODO2VNI_MAPPING].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV4_VXLAN_NO_FODO2VNI_MAPPING].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IPV4_VXLAN_GPE_NO_FODO2VNI_MAPPING].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_IML_NO_FODO_OUTLIF_MAP].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_ESEM1_DEFAULT_RESULT_RSPAN_ADVANCED_NO_SSPA].value_from_mapping = 8;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_esem2_default_result_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ESEM2_DEFAULT_RESULT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ESEM2_DEFAULT_RESULT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ESEM2_DEFAULT_RESULT_IML_NO_SSPA_NAMESPACE_MAP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM2_DEFAULT_RESULT_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM2_DEFAULT_RESULT_IML_NO_SSPA_NAMESPACE_MAP, "IML_NO_SSPA_NAMESPACE_MAP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ESEM2_DEFAULT_RESULT_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ESEM2_DEFAULT_RESULT_IML_NO_SSPA_NAMESPACE_MAP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_esem_command_attribute_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ESEM_COMMAND_ATTRIBUTE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ESEM_COMMAND_ATTRIBUTE", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_VXLAN_GPE_FODO_NAMESPACE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_GRE_FODO_NAMESPACE, "IPV6_GRE_FODO_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_EVPN_IML_FODO_OUTLIF_AND_SSPA_NAMESPACE, "EVPN_IML_FODO_OUTLIF_AND_SSPA_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_RSPAN_ADVANCED_SSPA_NAMESPACE, "RSPAN_ADVANCED_SSPA_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_SVTAG_TUNNEL, "SVTAG_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_SVTAG_PORT, "SVTAG_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV4_VXLAN_FODO_NAMESPACE, "IPV4_VXLAN_FODO_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV4_VXLAN_GPE_FODO_NAMESPACE, "IPV4_VXLAN_GPE_FODO_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_GENEVE_FODO_NAMESPACE, "IPV6_GENEVE_FODO_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_VXLAN_FODO_NAMESPACE, "IPV6_VXLAN_FODO_NAMESPACE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_VXLAN_GPE_FODO_NAMESPACE, "IPV6_VXLAN_GPE_FODO_NAMESPACE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_GRE_FODO_NAMESPACE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_EVPN_IML_FODO_OUTLIF_AND_SSPA_NAMESPACE].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_RSPAN_ADVANCED_SSPA_NAMESPACE].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_SVTAG_TUNNEL].value_from_mapping = 56;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_SVTAG_PORT].value_from_mapping = 57;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV4_VXLAN_FODO_NAMESPACE].value_from_mapping = 58;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV4_VXLAN_GPE_FODO_NAMESPACE].value_from_mapping = 59;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_GENEVE_FODO_NAMESPACE].value_from_mapping = 61;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_VXLAN_FODO_NAMESPACE].value_from_mapping = 62;
        enum_info[DBAL_ENUM_FVAL_ESEM_COMMAND_ATTRIBUTE_IPV6_VXLAN_GPE_FODO_NAMESPACE].value_from_mapping = 63;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_eth_qualifier_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETH_QUALIFIER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETH_QUALIFIER_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_OTHER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_ETH2, "ETH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_ETH1OSI, "ETH1OSI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_ETH1SNAP, "ETH1SNAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_OTHER, "OTHER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_ETH2].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_ETH1OSI].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_ETH1SNAP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETH_QUALIFIER_TYPE_OTHER].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_additional_header_profile_stack_attributes_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_7 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE, "AHS_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI_CW, "AHS_ESI_CW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI, "AHS_ESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOELOELI, "AHS_CWOELOELI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFL, "AHS_CWOFL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ELOELI, "AHS_ELOELI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CW, "AHS_CW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FL, "AHS_FL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE, "AHS_VXLAN_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GENEVE, "AHS_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN, "AHS_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE4, "AHS_GRE4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_KEY, "AHS_GRE8_W_KEY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V4, "AHS_GRE8_TNI_FROM_ENTRY_V4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP, "AHS_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE12, "AHS_GRE12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V6, "AHS_GRE8_TNI_FROM_ENTRY_V6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_SN, "AHS_GRE8_W_SN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IP_ESP, "AHS_IP_ESP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_ESP, "AHS_UDP_ESP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFLOESI, "AHS_CWOFLOESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FLOESI, "AHS_FLOESI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_DOUBLE_UDP_ESP, "AHS_VXLAN_DOUBLE_UDP_ESP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IOAM_FLOFLI, "AHS_IOAM_FLOFLI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_8, "AHS_UDP_SESSION_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_1, "AHS_UDP_SESSION_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_2, "AHS_UDP_SESSION_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_3, "AHS_UDP_SESSION_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_4, "AHS_UDP_SESSION_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_5, "AHS_UDP_SESSION_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_6, "AHS_UDP_SESSION_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_7, "AHS_UDP_SESSION_7");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI_CW].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOELOELI].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFL].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ELOELI].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CW].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FL].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GENEVE].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE4].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_KEY].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V4].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE12].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V6].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_SN].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IP_ESP].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_ESP].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFLOESI].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FLOESI].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_DOUBLE_UDP_ESP].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IOAM_FLOFLI].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_8].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_1].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_2].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_3].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_4].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_5].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_6].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP_SESSION_7].value_from_mapping = 31;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc1_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENC1_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENC1_PORT_CS_VAR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_EXTENDED_ENCAPSULATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_NAT, "NAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_MACSEC, "MACSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_EXTENDED_ENCAPSULATION, "EXTENDED_ENCAPSULATION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_NAT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_MACSEC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC1_PORT_CS_VAR_EXTENDED_ENCAPSULATION].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc2_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENC2_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENC2_PORT_CS_VAR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENC2_PORT_CS_VAR_ETHERNET_TYPICAL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC2_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC2_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc3_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENC3_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENC3_PORT_CS_VAR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENC3_PORT_CS_VAR_EXTENDED_ENCAPSULATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC3_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC3_PORT_CS_VAR_EXTENDED_ENCAPSULATION, "EXTENDED_ENCAPSULATION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC3_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC3_PORT_CS_VAR_EXTENDED_ENCAPSULATION].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc4_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENC4_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENC4_PORT_CS_VAR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENC4_PORT_CS_VAR_EXTENDED_ENCAPSULATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC4_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC4_PORT_CS_VAR_EXTENDED_ENCAPSULATION, "EXTENDED_ENCAPSULATION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC4_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC4_PORT_CS_VAR_EXTENDED_ENCAPSULATION].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc5_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENC5_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENC5_PORT_CS_VAR", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_MACSEC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_NPV_UPSTREAM, "NPV_UPSTREAM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_MACSEC, "MACSEC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_NPV_UPSTREAM].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENC5_PORT_CS_VAR_MACSEC].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_encap_1_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENCAP_1_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENCAP_1_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_SRV6_T_INSERT_EXTENDED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_RFC8321, "RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_PUSH_TWO_MPLS_LABELS, "PUSH_TWO_MPLS_LABELS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_SRV6_T_INSERT_EXTENDED, "SRV6_T_INSERT_EXTENDED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_RFC8321].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_PUSH_TWO_MPLS_LABELS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_1_ACE_CTXT_VALUE_SRV6_T_INSERT_EXTENDED].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_encap_2_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ENCAP_2_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ENCAP_2_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_B6_INSERT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_T_INSERT_EXT_OR_MPLS_RFC8321, "SRV6_T_INSERT_EXT_OR_MPLS_RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_EXT_ENCAP, "SRV6_EXT_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_B6_INSERT, "SRV6_B6_INSERT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_T_INSERT_EXT_OR_MPLS_RFC8321].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_EXT_ENCAP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_ENCAP_2_ACE_CTXT_VALUE_SRV6_B6_INSERT].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_esem_name_space_encoding_prp_ees_arr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR_ESEM_NAME_SPACE_TNIORVNI + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR_ESEM_NAME_SPACE_TNIORVNI, "ESEM_NAME_SPACE_TNIORVNI");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_ESEM_NAME_SPACE_ENCODING_PRP_EES_ARR_ESEM_NAME_SPACE_TNIORVNI].value_from_mapping = 248;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_forward_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_FORWARD_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_FORWARD_PORT_CS_VAR", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_NAT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACSEC, "MACSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_SBFD_REFLECTOR, "SBFD_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACT_JR1_DSP_CMD_TO_REFRESH, "MACT_JR1_DSP_CMD_TO_REFRESH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_COE_PORT, "COE_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_CHOOSE_NOP_CTX, "CHOOSE_NOP_CTX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_NAT, "NAT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACSEC].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_SBFD_REFLECTOR].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_MACT_JR1_DSP_CMD_TO_REFRESH].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_COE_PORT].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_CHOOSE_NOP_CTX].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_FORWARD_PORT_CS_VAR_NAT].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_fwd_code_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_FWD_CODE_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_FWD_CODE_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_J_MODE_FWD_CODE_OVERRIDE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_ERPP_TRAPPED, "ERPP_TRAPPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_J_MODE_FWD_CODE_OVERRIDE, "J_MODE_FWD_CODE_OVERRIDE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_ERPP_TRAPPED].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_FWD_CODE_ACE_CTXT_VALUE_J_MODE_FWD_CODE_OVERRIDE].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_next_protocol_namespace_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_NEXT_PROTOCOL_NAMESPACE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_NEXT_PROTOCOL_NAMESPACE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE, "ETHERTYPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL, "IP_NEXT_PROTOCOL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL, "VXLAN_GPE_NEXT_PROTOCOL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS, "MPLS_BOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_BIER_NEXT_PROTOCOL, "BIER_NEXT_PROTOCOL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT, "UDP_DESTINATION_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL, "PPP_NEXT_PROTOCOL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_ETHERTYPE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_IP_NEXT_PROTOCOL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_VXLAN_GPE_NEXT_PROTOCOL].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_MPLS_BOS].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_BIER_NEXT_PROTOCOL].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_UDP_DESTINATION_PORT].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_NEXT_PROTOCOL_NAMESPACE_PPP_NEXT_PROTOCOL].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_php_ttl_model_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_PHP_TTL_MODEL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_PHP_TTL_MODEL", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_PHP_TTL_MODEL_TTL_MODEL_PIPE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PHP_TTL_MODEL_TTL_MODEL_UNIFORM, "TTL_MODEL_UNIFORM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PHP_TTL_MODEL_TTL_MODEL_PIPE, "TTL_MODEL_PIPE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_PHP_TTL_MODEL_TTL_MODEL_UNIFORM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_PHP_TTL_MODEL_TTL_MODEL_PIPE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_prp2_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_PRP2_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_PRP2_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_IPFIX_RX_PACKET + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_CANCEL_SYSTEM_HEADER_LIFS_ADD_DEST_PORT_BASED_LIF_LOOKUP, "CANCEL_SYSTEM_HEADER_LIFS_ADD_DEST_PORT_BASED_LIF_LOOKUP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_IPFIX_RX_PACKET, "IPFIX_RX_PACKET");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_CANCEL_SYSTEM_HEADER_LIFS_ADD_DEST_PORT_BASED_LIF_LOOKUP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_PRP2_ACE_CTXT_VALUE_IPFIX_RX_PACKET].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_prp2_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_PRP2_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_PRP2_PORT_CS_VAR", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_CANCEL_SYSHDR_ENC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_CANCEL_SYSHDR_ENC, "CANCEL_SYSHDR_ENC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_PRP2_PORT_CS_VAR_CANCEL_SYSHDR_ENC].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_qos_var_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_QOS_VAR_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_QOS_VAR_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_GTP_QFI + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI, "PCP_DEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS, "IPV4_TOS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC, "IPV6_TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC, "MPLS_TC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_BIER_DSCP, "BIER_DSCP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS_ECN, "IPV4_TOS_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC_ECN, "IPV6_TC_ECN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_GTP_QFI, "GTP_QFI");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_BIER_DSCP].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS_ECN].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC_ECN].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_GTP_QFI].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_termination_port_cs_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_TERMINATION_PORT_CS_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_TERMINATION_PORT_CS_VAR", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_BUILD_FTMH_FROM_PIPE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_ETHERNET_TYPICAL, "ETHERNET_TYPICAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_OAMP, "OAMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_SWITCH_DEST_ENC_JR1_ARAD, "MACT_JR1_DSP_SWITCH_DEST_ENC_JR1_ARAD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_SBFD_REFLECTOR, "SBFD_REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR2_DSP_CMD_TO_REFRESH, "MACT_JR2_DSP_CMD_TO_REFRESH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_IFA2_0_EGRESS_FORWARD, "IFA2_0_EGRESS_FORWARD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_TERMINATE_UP_2_PSO_CONSTRUCT_NOTHING_DO_NOTHING, "TERMINATE_UP_2_PSO_CONSTRUCT_NOTHING_DO_NOTHING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_NAT, "NAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS, "MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_BUILD_FTMH_FROM_PIPE, "BUILD_FTMH_FROM_PIPE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_ETHERNET_TYPICAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_OAMP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_SWITCH_DEST_ENC_JR1_ARAD].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_SBFD_REFLECTOR].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR2_DSP_CMD_TO_REFRESH].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_IFA2_0_EGRESS_FORWARD].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_TERMINATE_UP_2_PSO_CONSTRUCT_NOTHING_DO_NOTHING].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_NAT].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_MACT_JR1_DSP_COPY_DSP_CMD_23_TO_0_AS_IS].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERMINATION_PORT_CS_VAR_BUILD_FTMH_FROM_PIPE].value_from_mapping = 9;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_term_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_TERM_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_TERM_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_TM_TO_COE_TEMP_OR_MPLS_TRANSIT_CCM_SD_OR_L2_REFLECTOR_IP_SWAP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_REFLECTOR_UDP_SWAP, "REFLECTOR_UDP_SWAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_IOAM_JP, "IOAM_JP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_TM_TO_COE_TEMP_OR_MPLS_TRANSIT_CCM_SD_OR_L2_REFLECTOR_IP_SWAP, "TM_TO_COE_TEMP_OR_MPLS_TRANSIT_CCM_SD_OR_L2_REFLECTOR_IP_SWAP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_REFLECTOR_UDP_SWAP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_IOAM_JP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_TM_TO_COE_TEMP_OR_MPLS_TRANSIT_CCM_SD_OR_L2_REFLECTOR_IP_SWAP].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_ace_ctxt_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_TRAP_ACE_CTXT_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_TRAP_ACE_CTXT_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_IFA_TRAP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_RECYCLE_W_PTCH2, "RECYCLE_W_PTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_OVERRIDE_DROP, "OVERRIDE_DROP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_IFA_TRAP, "IFA_TRAP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_RECYCLE_W_PTCH2].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_OVERRIDE_DROP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_ACE_CTXT_VALUE_IFA_TRAP].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_context_port_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_TRAP_CONTEXT_PORT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_TRAP_CONTEXT_PORT_PROFILE", dnx_data_dnx2_aod_sizes.AOD.ETPP_TRAP_CONTEXT_PORT_PROFILE_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_BUILD_PTCH1_FROM_FTMH_SSPA + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL, "NORMAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_DROP_ALL, "DROP_ALL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_MC_CUD, "TDM_STAMP_MC_CUD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_PORT_VAL, "TDM_STAMP_PORT_VAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_NOTHING_OR_BUILD_PTCH1, "TDM_STAMP_NOTHING_OR_BUILD_PTCH1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_APPEND_64B, "APPEND_64B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_INT_EGRESS_PORT, "INT_EGRESS_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_BUILD_PTCH1_FROM_FTMH_SSPA, "BUILD_PTCH1_FROM_FTMH_SSPA");
    }
    if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0 || DBAL_IS_J2P_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_DROP_ALL].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_MC_CUD].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_PORT_VAL].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_NOTHING_OR_BUILD_PTCH1].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_APPEND_64B].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_INT_EGRESS_PORT].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_BUILD_PTCH1_FROM_FTMH_SSPA].value_from_mapping = 7;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_fwd_rcy_cmd_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_TRAP_FWD_RCY_CMD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_TRAP_FWD_RCY_CMD", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_FWD_COPY_OF_SNOOP_OR_MIRROR + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_EGRESS_TRAPPED, "EGRESS_TRAPPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_REFLECTED, "REFLECTED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_FWD_COPY_OF_SNOOP_OR_MIRROR, "FWD_COPY_OF_SNOOP_OR_MIRROR");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_EGRESS_TRAPPED].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_REFLECTED].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_FWD_RCY_CMD_FWD_COPY_OF_SNOOP_OR_MIRROR].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_rcy_cmd_map_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPP_TRAP_RCY_CMD_MAP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPP_TRAP_RCY_CMD_MAP", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_ETHOAPPEND64 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_RCH, "RCH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_PTCH2, "PTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_FTMH, "FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_RCHOPTCH2, "RCHOPTCH2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_ETHOAPPEND64, "ETHOAPPEND64");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_RCH].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_PTCH2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_FTMH].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_RCHOPTCH2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_ETPP_TRAP_RCY_CMD_MAP_ETHOAPPEND64].value_from_mapping = 5;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etps_gtpv1u_raw_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPS_GTPV1U_RAW_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPS_GTPV1U_RAW_DATA", 35, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_TEID_AG;
    cur_field_types_info->struct_field_info[0].length = 32;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_HEADER_TYPE_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_SESSION_TYPE_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_SESSION_RQI_AG;
    cur_field_types_info->struct_field_info[3].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etps_srv6_raw_data_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ETPS_SRV6_RAW_DATA];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ETPS_SRV6_RAW_DATA", 61, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_SRH_BASE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_NOF_SIDS_AG;
    cur_field_types_info->struct_field_info[1].length = 5;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_evpn_iml_inlif_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EVPN_IML_INLIF_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EVPN_IML_INLIF_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_TAG_NAMESPACE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_IML_ADDITIONAL_HEADERS;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_RESERVED0_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_exem_default_result_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EXEM_DEFAULT_RESULT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EXEM_DEFAULT_RESULT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_EXEM_DEFAULT_RESULT_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_EXEM_DEFAULT_RESULT_NONE, "NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_EXEM_DEFAULT_RESULT_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_bier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_BIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_BIER", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FAI_BIER_TYPE_AG;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FAI_TTL_PROCCESING_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FAI_FORWARD_HEADER_REMARKING_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ethernet_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_ETHERNET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_ETHERNET", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FAI_FORWARD_RESULT_AG;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FAI_FORWARD_P1_HEADER_REMARKING_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FAI_TTL_PROCCESING_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_fallback_to_bridge_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_FALLBACK_TO_BRIDGE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_FALLBACK_TO_BRIDGE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FAI_FALLBACK_TO_BRIDGE_NON_ELIGIBLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FALLBACK_TO_BRIDGE_ELIGIBLE, "ELIGIBLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FALLBACK_TO_BRIDGE_NON_ELIGIBLE, "NON_ELIGIBLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FAI_FALLBACK_TO_BRIDGE_ELIGIBLE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FAI_FALLBACK_TO_BRIDGE_NON_ELIGIBLE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_forward_header_remarking_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_FORWARD_HEADER_REMARKING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_FORWARD_HEADER_REMARKING", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FAI_FORWARD_HEADER_REMARKING_IRPP_FORWARD_HEADER_REMARK_PROFILE1 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FORWARD_HEADER_REMARKING_IRPP_FORWARD_HEADER_REMARK_PROFILE0, "IRPP_FORWARD_HEADER_REMARK_PROFILE0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FORWARD_HEADER_REMARKING_IRPP_FORWARD_HEADER_REMARK_PROFILE1, "IRPP_FORWARD_HEADER_REMARK_PROFILE1");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FAI_FORWARD_HEADER_REMARKING_IRPP_FORWARD_HEADER_REMARK_PROFILE0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FAI_FORWARD_HEADER_REMARKING_IRPP_FORWARD_HEADER_REMARK_PROFILE1].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_forward_p1_header_remarking_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_FORWARD_P1_HEADER_REMARKING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_FORWARD_P1_HEADER_REMARKING", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FAI_FORWARD_P1_HEADER_REMARKING_IRPP_FORWARD_P1_HEADER_REMARK_PROFILE1 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FORWARD_P1_HEADER_REMARKING_IRPP_FORWARD_P1_HEADER_REMARK_PROFILE0, "IRPP_FORWARD_P1_HEADER_REMARK_PROFILE0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FORWARD_P1_HEADER_REMARKING_IRPP_FORWARD_P1_HEADER_REMARK_PROFILE1, "IRPP_FORWARD_P1_HEADER_REMARK_PROFILE1");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FAI_FORWARD_P1_HEADER_REMARKING_IRPP_FORWARD_P1_HEADER_REMARK_PROFILE0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FAI_FORWARD_P1_HEADER_REMARKING_IRPP_FORWARD_P1_HEADER_REMARK_PROFILE1].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_forward_result_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_FORWARD_RESULT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_FORWARD_RESULT", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FAI_FORWARD_RESULT_UNKNOWN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FORWARD_RESULT_KNOWN, "KNOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_FORWARD_RESULT_UNKNOWN, "UNKNOWN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FAI_FORWARD_RESULT_KNOWN].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FAI_FORWARD_RESULT_UNKNOWN].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_inject_indication_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_INJECT_INDICATION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_INJECT_INDICATION", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FAI_INJECT_INDICATION_NON_ELIGIBLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_INJECT_INDICATION_ELIGIBLE, "ELIGIBLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_INJECT_INDICATION_NON_ELIGIBLE, "NON_ELIGIBLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FAI_INJECT_INDICATION_ELIGIBLE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FAI_INJECT_INDICATION_NON_ELIGIBLE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ipv4_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_IPV4];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_IPV4", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FAI_FORWARD_RESULT_AG;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FAI_FALLBACK_TO_BRIDGE_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FAI_TTL_PROCCESING_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FAI_FORWARD_HEADER_REMARKING_AG;
    cur_field_types_info->struct_field_info[3].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ipv6_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_IPV6];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_IPV6", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FAI_FORWARD_RESULT_AG;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FAI_FALLBACK_TO_BRIDGE_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FAI_TTL_PROCCESING_AG;
    cur_field_types_info->struct_field_info[2].length = 1;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FAI_FORWARD_HEADER_REMARKING_AG;
    cur_field_types_info->struct_field_info[3].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_mpls_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_MPLS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_MPLS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FAI_INJECT_INDICATION_AG;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FAI_TTL_PROCCESING_AG;
    cur_field_types_info->struct_field_info[1].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ttl_proccesing_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FAI_TTL_PROCCESING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FAI_TTL_PROCCESING", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FAI_TTL_PROCCESING_DECREMENT_TTL_ENABLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_TTL_PROCCESING_DECREMENT_TTL_DISABLE, "DECREMENT_TTL_DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FAI_TTL_PROCCESING_DECREMENT_TTL_ENABLE, "DECREMENT_TTL_ENABLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FAI_TTL_PROCCESING_DECREMENT_TTL_DISABLE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FAI_TTL_PROCCESING_DECREMENT_TTL_ENABLE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fcoe_first_additional_header_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FCOE_FIRST_ADDITIONAL_HEADER_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FCOE_FIRST_ADDITIONAL_HEADER_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_ENCAPSULATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_VFT, "VFT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_ENCAPSULATION, "ENCAPSULATION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_VFT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FCOE_FIRST_ADDITIONAL_HEADER_TYPE_ENCAPSULATION].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fcoe_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FCOE_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FCOE_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FCOE_TYPE_FIP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FCOE_TYPE_FCOE, "FCOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FCOE_TYPE_FIP, "FIP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FCOE_TYPE_FCOE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FCOE_TYPE_FIP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fodo_pd_result_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FODO_PD_RESULT_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FODO_PD_RESULT_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_NOT_FOUND, "RESULT_NOT_FOUND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND, "RESULT_FOUND");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_NOT_FOUND].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FODO_PD_RESULT_TYPE_RESULT_FOUND].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_forward_app_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FORWARD_APP_TYPES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FORWARD_APP_TYPES", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID_NO_DEFAULT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID, "INVALID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV4_MULTICAST_PRIVATE_W_UC_RPF, "IPV4_MULTICAST_PRIVATE_W_UC_RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV6_MULTICAST_PRIVATE_W_UC_RPF, "IPV6_MULTICAST_PRIVATE_W_UC_RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV4_UNICAST_PRIVATE_W_UC_RPF, "IPV4_UNICAST_PRIVATE_W_UC_RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV6_UNICAST_PRIVATE_W_UC_RPF, "IPV6_UNICAST_PRIVATE_W_UC_RPF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_BRIDGE_SVL, "BRIDGE_SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_NAT_MAPPING, "NAT_MAPPING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_VIRTUAL_IPV4_PRIVATE_UC, "VIRTUAL_IPV4_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_VIRTUAL_IPV6_PRIVATE_UC, "VIRTUAL_IPV6_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SIMPLE_IPV4_PRIVATE_UC, "SIMPLE_IPV4_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SIMPLE_IPV6_PRIVATE_UC, "SIMPLE_IPV6_PRIVATE_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_CLASSIC, "SRV6_ENDPOINT_CLASSIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID, "SRV6_ENDPOINT_USID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_BIER_NON_MPLS_BFR, "BIER_NON_MPLS_BFR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_GSID, "SRV6_ENDPOINT_GSID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_NOP, "NOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_ETHERNET_P2P, "ETHERNET_P2P");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID_NO_DEFAULT, "SRV6_ENDPOINT_USID_NO_DEFAULT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_INVALID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV4_MULTICAST_PRIVATE_W_UC_RPF].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV6_MULTICAST_PRIVATE_W_UC_RPF].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV4_UNICAST_PRIVATE_W_UC_RPF].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_IPV6_UNICAST_PRIVATE_W_UC_RPF].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_MPLS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_BRIDGE_SVL].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_NAT_MAPPING].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_VIRTUAL_IPV4_PRIVATE_UC].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_VIRTUAL_IPV6_PRIVATE_UC].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SIMPLE_IPV4_PRIVATE_UC].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SIMPLE_IPV6_PRIVATE_UC].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_CLASSIC].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_BIER_NON_MPLS_BFR].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_GSID].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_NOP].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_ETHERNET_P2P].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID_NO_DEFAULT].value_from_mapping = 18;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_forward_domain_assignment_mode_enum_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF, "FORWARD_DOMAIN_FROM_LIF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN, "FORWARD_DOMAIN_FROM_VLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP, "FORWARD_DOMAIN_FROM_LOOKUP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_forward_domain_mask_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FORWARD_DOMAIN_MASK_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FORWARD_DOMAIN_MASK_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_PD_17_0 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_0, "FROM_LIF_17_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_12_FROM_PD_11_0, "FROM_LIF_17_12_FROM_PD_11_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_PD_17_0, "FROM_PD_17_0");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_LIF_17_12_FROM_PD_11_0].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FORWARD_DOMAIN_MASK_PROFILE_FROM_PD_17_0].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_gtp_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GTP_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "GTP_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_GTP_TYPE_GTP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GTP_TYPE_GTP_TAG, "GTP_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GTP_TYPE_GTP, "GTP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_GTP_TYPE_GTP_TAG].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_GTP_TYPE_GTP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_gtp_version_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_GTP_VERSION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "GTP_VERSION", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_GTP_VERSION_V2 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GTP_VERSION_V0, "V0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GTP_VERSION_V1, "V1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_GTP_VERSION_V2, "V2");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_GTP_VERSION_V0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_GTP_VERSION_V1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_GTP_VERSION_V2].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_incoming_tag_structure_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INCOMING_TAG_STRUCTURE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INCOMING_TAG_STRUCTURE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1, "UNTAGGED_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_1, "C_C_TAG_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_2, "C_C_TAG_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1, "S_TAG_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_2, "S_TAG_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_3, "S_TAG_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_4, "S_TAG_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1, "C_TAG_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_2, "C_TAG_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_3, "C_TAG_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_4, "C_TAG_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_5, "C_TAG_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_6, "C_TAG_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_7, "C_TAG_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_8, "C_TAG_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_1, "S_C_TAG_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_2, "S_C_TAG_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_3, "S_C_TAG_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_4, "S_C_TAG_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_5, "S_C_TAG_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_6, "S_C_TAG_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_7, "S_C_TAG_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_8, "S_C_TAG_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1, "S_PRIORITY_C_TAG_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_2, "S_PRIORITY_C_TAG_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_3, "S_PRIORITY_C_TAG_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_4, "S_PRIORITY_C_TAG_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_1, "S_S_TAG_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_2, "S_S_TAG_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY, "S_PRIORITY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_C_TAG_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_3].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_4].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_2].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_3].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_4].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_5].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_6].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_7].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_8].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_1].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_2].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_3].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_4].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_5].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_6].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_7].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_C_TAG_8].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_2].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_3].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_4].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_1].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_S_TAG_2].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY].value_from_mapping = 29;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ingress_mpls_range_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INGRESS_MPLS_RANGE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "INGRESS_MPLS_RANGE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_INGRESS_MPLS_RANGE_EVPN_IML + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_MPLS_RANGE_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_INGRESS_MPLS_RANGE_EVPN_IML, "EVPN_IML");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_INGRESS_MPLS_RANGE_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_INGRESS_MPLS_RANGE_EVPN_IML].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_in_lif_same_interface_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IN_LIF_SAME_INTERFACE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IN_LIF_SAME_INTERFACE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_INVALID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_DEVICE_SCOPE, "DEVICE_SCOPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_SYSTEM_SCOPE, "SYSTEM_SCOPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_DISABLE, "DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_INVALID, "INVALID");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_DEVICE_SCOPE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_SYSTEM_SCOPE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_DISABLE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IN_LIF_SAME_INTERFACE_INVALID].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_in_pp_port_flp_kg_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IN_PP_PORT_FLP_KG_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IN_PP_PORT_FLP_KG_VAR", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VMPI_AG;
    cur_field_types_info->struct_field_info[0].length = 6;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_in_pp_port_vtt_kg_var_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IN_PP_PORT_VTT_KG_VAR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IN_PP_PORT_VTT_KG_VAR", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VMPI_AG;
    cur_field_types_info->struct_field_info[0].length = 6;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv4_additional_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPV4_ADDITIONAL_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPV4_ADDITIONAL_HEADER", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE8_WITH_SN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_RAW, "RAW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE4, "GRE4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE8_WITH_KEY, "GRE8_WITH_KEY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE12_WITH_KEY_WITH_SN, "GRE12_WITH_KEY_WITH_SN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_ESP, "ESP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE8_WITH_SN, "GRE8_WITH_SN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_RAW].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE4].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE8_WITH_KEY].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE12_WITH_KEY_WITH_SN].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_UDP].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_ESP].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IPV4_ADDITIONAL_HEADER_GRE8_WITH_SN].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv4_qualifier_tunnel_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPV4_QUALIFIER_TUNNEL_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPV4_QUALIFIER_TUNNEL_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPUOUDP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_RAW, "RAW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE8_WITH_SN, "GRE8_WITH_SN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4, "GRE4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE8_WITH_KEY, "GRE8_WITH_KEY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE12, "GRE12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP, "VXLANOUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP, "VXLAN_GPEOUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GENEVEOUDP, "GENEVEOUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_L2TPV3OUDP, "L2TPV3OUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4OUDP, "GRE4OUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC, "ESPOIPSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC_UDP, "ESPOIPSEC_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPV2OUDP, "GTPV2OUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPUOUDP, "GTPUOUDP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_RAW].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE8_WITH_SN].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE8_WITH_KEY].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE12].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_UDP].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLANOUDP].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_VXLAN_GPEOUDP].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GENEVEOUDP].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_L2TPV3OUDP].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GRE4OUDP].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_ESPOIPSEC_UDP].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPV2OUDP].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_IPV4_QUALIFIER_TUNNEL_TYPE_GTPUOUDP].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv6_additional_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPV6_ADDITIONAL_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPV6_ADDITIONAL_HEADER", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_BIGGER_THAN_64 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE, "NOT_AVAILABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE4, "GRE4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE8, "GRE8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE12, "GRE12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_L2TPV3, "L2TPV3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN, "VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN_GPE, "VXLAN_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_ESP_UDP, "ESP_UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GENEVE, "GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6, "IPV6_SRV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0, "IPV6_SRV6_SL_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_AH_RAW, "IPV6_AH_RAW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_HOP_BY_HOP, "IPV6_EH_HOP_BY_HOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_FRAGMENT, "IPV6_EH_FRAGMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_ESP, "IPV6_EH_ESP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_AUTH, "IPV6_EH_AUTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_DO, "IPV6_EH_DO");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_MOBILITY, "IPV6_EH_MOBILITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_HIP, "IPV6_EH_HIP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_SHIM6, "IPV6_EH_SHIM6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_HOP_BY_HOP_PT, "IPV6_EH_HOP_BY_HOP_PT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_GTPOUDP, "IPV6_GTPOUDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_BIGGER_THAN_64, "IPV6_EH_BIGGER_THAN_64");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE4].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE8].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GRE12].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_UDP].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_L2TPV3].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_VXLAN_GPE].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_ESP_UDP].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_GENEVE].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_AH_RAW].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_HOP_BY_HOP].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_FRAGMENT].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_ESP].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_AUTH].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_DO].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_MOBILITY].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_HIP].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_SHIM6].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_HOP_BY_HOP_PT].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_GTPOUDP].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_EH_BIGGER_THAN_64].value_from_mapping = 31;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv6_dip_idx_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPV6_DIP_IDX_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPV6_DIP_IDX_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_32_USID_16_NO_DEFAULT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_INTERMIDIATE_RESULT, "INTERMIDIATE_RESULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_LIF_ELIGIBLE, "LIF_ELIGIBLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16, "CLASSIC_SID_LOCATER64_FUNCTION_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16, "CLASSIC_SID_LOCATER96_FUNCTION_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_32_USID_16, "USID_PREFIX_32_USID_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_48_GSID_16, "GSID_PREFIX_48_GSID_16");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT, "CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT, "CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_48_GSID_16_NO_DEFAULT, "GSID_PREFIX_48_GSID_16_NO_DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_32_USID_16_NO_DEFAULT, "USID_PREFIX_32_USID_16_NO_DEFAULT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_INTERMIDIATE_RESULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_LIF_ELIGIBLE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_32_USID_16].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_48_GSID_16].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_FUNCTION_16_NO_DEFAULT].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_FUNCTION_16_NO_DEFAULT].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_GSID_PREFIX_48_GSID_16_NO_DEFAULT].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_USID_PREFIX_32_USID_16_NO_DEFAULT].value_from_mapping = 9;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv6_extension_pre_srv6_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPV6_EXTENSION_PRE_SRV6];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPV6_EXTENSION_PRE_SRV6", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IPV6_EXTENSION_PRE_SRV6_HBH_PT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_EXTENSION_PRE_SRV6_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IPV6_EXTENSION_PRE_SRV6_HBH_PT, "HBH_PT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IPV6_EXTENSION_PRE_SRV6_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IPV6_EXTENSION_PRE_SRV6_HBH_PT].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ip_address_spoofing_check_modes_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IP_ADDRESS_SPOOFING_CHECK_MODES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IP_ADDRESS_SPOOFING_CHECK_MODES", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IP_ADDRESS_SPOOFING_CHECK_MODES_ENABLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IP_ADDRESS_SPOOFING_CHECK_MODES_DISABLE, "DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IP_ADDRESS_SPOOFING_CHECK_MODES_ENABLE, "ENABLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IP_ADDRESS_SPOOFING_CHECK_MODES_DISABLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IP_ADDRESS_SPOOFING_CHECK_MODES_ENABLE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_1st_parser_parser_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_1ST_PARSER_PARSER_CONTEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_1ST_PARSER_PARSER_CONTEXT", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_NOP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_A1, "ITMH_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1, "ETH_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RCH_A1, "RCH_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RCH_SRV6_USP_PSP, "RCH_SRV6_USP_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_JR1_A1, "ITMH_JR1_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ETH_JR1_A1, "ETH_JR1_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RAW_MPLS_A1, "RAW_MPLS_A1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_NOP, "NOP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_A1].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ETH_A1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RCH_A1].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RCH_SRV6_USP_PSP].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ITMH_JR1_A1].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_ETH_JR1_A1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_RAW_MPLS_A1].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_1ST_PARSER_PARSER_CONTEXT_NOP].value_from_mapping = 30;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_2nd_parser_parser_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_2ND_PARSER_PARSER_CONTEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_2ND_PARSER_PARSER_CONTEXT", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NOP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_MPLS_SPECULATIVEOAMB1, "MPLS_SPECULATIVEOAMB1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1, "ETH_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPVX_DEMUX_B1, "IPVX_DEMUX_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV4_NOETHB2, "IPV4_NOETHB2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV6_NOETHB2, "IPV6_NOETHB2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIER, "BIER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIER_NON_MPLS, "BIER_NON_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TCP_B1, "TCP_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_UDP_B1, "UDP_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_MPLS_B1, "MPLS_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_SCTP_B1, "SCTP_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TCP_NO_LB_B1, "TCP_NO_LB_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_UDP_NO_LB_B1, "UDP_NO_LB_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_PPP_B1, "PPP_B1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TDM_BS, "TDM_BS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NOP, "NOP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_MPLS_SPECULATIVEOAMB1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_ETH_B1].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPVX_DEMUX_B1].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV4_NOETHB2].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_IPV6_NOETHB2].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIER].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_BIER_NON_MPLS].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TCP_B1].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_UDP_B1].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_MPLS_B1].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_SCTP_B1].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TCP_NO_LB_B1].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_UDP_NO_LB_B1].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_PPP_B1].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_TDM_BS].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_IRPP_2ND_PARSER_PARSER_CONTEXT_NOP].value_from_mapping = 30;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_next_layer_network_domain_encoding_vtt_lif_arr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NONE, "NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NEXT_LAYER_NETWORK_DOMAIN_TNIORVNI, "NEXT_LAYER_NETWORK_DOMAIN_TNIORVNI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS, "NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NEXT_LAYER_NETWORK_DOMAIN_TNIORVNI].value_from_mapping = 15925248;
        enum_info[DBAL_ENUM_FVAL_IRPP_NEXT_LAYER_NETWORK_DOMAIN_ENCODING_VTT_LIF_ARR_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS].value_from_mapping = 15859712;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_system_header_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IRPP_SYSTEM_HEADER_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IRPP_SYSTEM_HEADER_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_NO_SYSTEM_HEADERS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH, "FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE, "FTMH_FDE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_TSH, "FTMH_TSH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE_TSH, "FTMH_FDE_TSH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_PPH, "FTMH_PPH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE_PPH, "FTMH_FDE_PPH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_TSH_PPH, "FTMH_TSH_PPH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE_TSH_PPH, "FTMH_FDE_TSH_PPH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_TSH_PPH_UDH, "FTMH_TSH_PPH_UDH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_NO_SYSTEM_HEADERS, "NO_SYSTEM_HEADERS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_TSH].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE_TSH].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_PPH].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE_PPH].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_TSH_PPH].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_FDE_TSH_PPH].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_FTMH_TSH_PPH_UDH].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_IRPP_SYSTEM_HEADER_PROFILE_NO_SYSTEM_HEADERS].value_from_mapping = 9;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_jr_fwd_code_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_JR_FWD_CODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "JR_FWD_CODE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_JR_FWD_CODE_TM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE, "BRIDGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC, "IPV4_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC, "IPV4_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC, "IPV6_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC, "IPV6_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_TRILL, "TRILL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_CPU_TRAP1, "CPU_TRAP1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED0, "RESERVED0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_CUSTOMER_1, "CUSTOMER_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_CUSTOMER_2, "CUSTOMER_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_SNOOP_OR_MIRROR, "SNOOP_OR_MIRROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED1, "RESERVED1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED2, "RESERVED2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED3, "RESERVED3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_JR_FWD_CODE_TM, "TM");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_BRIDGE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_UC].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_IPV4_MC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_UC].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_IPV6_MC].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_MPLS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_TRILL].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_CPU_TRAP1].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED0].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_CUSTOMER_1].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_CUSTOMER_2].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_SNOOP_OR_MIRROR].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED1].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED2].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_RESERVED3].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_JR_FWD_CODE_TM].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2tp_d_or_c_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L2TP_D_OR_C];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L2TP_D_OR_C", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L2TP_D_OR_C_CONTROL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L2TP_D_OR_C_DATA, "DATA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L2TP_D_OR_C_CONTROL, "CONTROL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_L2TP_D_OR_C_DATA].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_L2TP_D_OR_C_CONTROL].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2_fodo_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L2_FODO_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L2_FODO_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_V6_MC_FWD_TYPE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_V4_MC_FWD_TYPE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_IVL_OR_SVL;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_RESERVED_AG;
    cur_field_types_info->struct_field_info[3].length = 1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2_v4_mc_fwd_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L2_V4_MC_FWD_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L2_V4_MC_FWD_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_IPV4MC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_V4BRIDGE, "V4BRIDGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_IPV4MC, "IPV4MC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_V4BRIDGE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_L2_V4_MC_FWD_TYPE_IPV4MC].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2_v6_mc_fwd_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L2_V6_MC_FWD_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "L2_V6_MC_FWD_TYPE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_IPV6MC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_V6BRIDGE, "V6BRIDGE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_IPV6MC, "IPV6MC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_V6BRIDGE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_L2_V6_MC_FWD_TYPE_IPV6MC].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_layer_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LAYER_TYPES];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LAYER_TYPES", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LAYER_TYPES_ITMH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION, "INITIALIZATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_IFIT, "MPLS_IFIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_ARP, "ARP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE, "FCOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_TCP, "TCP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_UDP, "UDP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP, "BFD_SINGLE_HOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP, "BFD_MULTI_HOP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_L2TP, "L2TP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_PTP, "PTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731, "Y_1731");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_ICMP, "ICMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_BIER_NON_MPLS, "BIER_NON_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_IN_BIER_BASE_EG_BIER_MPLS, "IN_BIER_BASE_EG_BIER_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_RCH, "RCH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE, "PPPOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT, "SRV6_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_TDM_BS, "TDM_BS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_IGMP, "IGMP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_RCH_USP_PSP_AND_PSP_EXT, "SRV6_RCH_USP_PSP_AND_PSP_EXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH, "INGRESS_SCTP_EGRESS_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_GTP, "GTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_PPP, "PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM, "MPLS_UNTERM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL, "ITMH_J1_VAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_IPVX, "IPVX");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN, "UNKNOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_ITMH, "ITMH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_IPV4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_IPV6].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_MPLS].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_IFIT].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_ARP].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_FCOE].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_TCP].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_UDP].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_L2TP].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_PTP].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_ICMP].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_BIER_NON_MPLS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_IN_BIER_BASE_EG_BIER_MPLS].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_RCH].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_TDM_BS].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_IGMP].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_RCH_USP_PSP_AND_PSP_EXT].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_GTP].value_from_mapping = 25;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_PPP].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_IPVX].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_ITMH].value_from_mapping = 31;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_layer_types_system_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LAYER_TYPES_SYSTEM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LAYER_TYPES_SYSTEM", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_UNKNOWN_SYSTEM + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_INITIALIZATION_SYSTEM, "INITIALIZATION_SYSTEM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_ASE, "ASE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_TSH, "TSH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_PPH, "PPH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_UDH, "UDH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_FTMH, "FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_OTSH_JR1_VAL, "OTSH_JR1_VAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_PPH_JR1_VAL, "PPH_JR1_VAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_UNKNOWN_SYSTEM, "UNKNOWN_SYSTEM");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_INITIALIZATION_SYSTEM].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_ASE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_TSH].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_PPH].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_UDH].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_FTMH].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_OTSH_JR1_VAL].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_PPH_JR1_VAL].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_LAYER_TYPES_SYSTEM_UNKNOWN_SYSTEM].value_from_mapping = 31;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_learn_payload_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LEARN_PAYLOAD_CONTEXT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LEARN_PAYLOAD_CONTEXT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF0__ANY_DEST__16_GROUP__2L_STRENGTH__SVL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__SVL, "SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__SVL, "SYMMETRIC_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__IVL, "SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__IVL, "SYMMETRIC_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__SVL, "SYMMETRIC_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__IVL, "SYMMETRIC_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__SVL, "ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__IVL, "ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__BOTH, "NO_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__BOTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__BOTH, "NO_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__BOTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__BOTH, "NO_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__BOTH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_EEI__ANY_DEST__16_GROUP__2L_STRENGTH__SVL, "ASYMMETRIC_EEI__ANY_DEST__16_GROUP__2L_STRENGTH__SVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF0__ANY_DEST__16_GROUP__2L_STRENGTH__SVL, "SYMMETRIC_LIF0__ANY_DEST__16_GROUP__2L_STRENGTH__SVL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__SVL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__SVL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__IVL].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__IVL].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__SVL].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__IVL].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__SVL].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_LIF__ANY_DEST__16_GROUP__2L_STRENGTH__IVL].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__FEC_OR_SPA_OR_TRAP_DEST__16_GROUP__2L_STRENGTH__BOTH].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__TMF_OR_MC_DEST__16_GROUP__2L_STRENGTH__BOTH].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF__SYMMETRIC_DEST__16_GROUP__2L_STRENGTH__BOTH].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_ASYMMETRIC_EEI__ANY_DEST__16_GROUP__2L_STRENGTH__SVL].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF0__ANY_DEST__16_GROUP__2L_STRENGTH__SVL].value_from_mapping = 14;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_general_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_AHP_GENERAL];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_AHP_GENERAL", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_AHP_GENERAL_LIF_AHP_NONE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GENERAL_LIF_AHP_NONE, "LIF_AHP_NONE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GENERAL_LIF_AHP_NONE].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_gre_geneve_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_AHP_GRE_GENEVE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_AHP_GRE_GENEVE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_NONE, "LIF_AHP_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GENEVE, "LIF_AHP_GENEVE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY, "LIF_AHP_GRE8_W_KEY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4, "LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE12, "LIF_AHP_GRE12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6, "LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE4, "LIF_AHP_GRE4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN, "LIF_AHP_GRE8_W_SN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GENEVE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V4].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE12].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_KEY_TNI_FROM_ENTRY_V6].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE4].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_GRE_GENEVE_LIF_AHP_GRE8_W_SN].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_ip_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_AHP_IP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_AHP_IP", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_IP_ESP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_NONE, "LIF_AHP_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_IP_ESP, "LIF_AHP_IP_ESP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_IP_LIF_AHP_IP_ESP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_mpls_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_AHP_MPLS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_AHP_MPLS", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE, "LIF_AHP_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL, "LIF_AHP_CWOFL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_ELOELI, "LIF_AHP_ELOELI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW, "LIF_AHP_CW");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL, "LIF_AHP_FL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI, "LIF_AHP_CWOELOELI");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOFL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_ELOELI].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CW].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_FL].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_MPLS_LIF_AHP_CWOELOELI].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_udp_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_AHP_UDP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_AHP_UDP", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_8 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE, "LIF_AHP_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_1, "LIF_AHP_UDP_SESSION_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_2, "LIF_AHP_UDP_SESSION_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_3, "LIF_AHP_UDP_SESSION_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_4, "LIF_AHP_UDP_SESSION_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_5, "LIF_AHP_UDP_SESSION_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_6, "LIF_AHP_UDP_SESSION_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_7, "LIF_AHP_UDP_SESSION_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_TUNNEL, "LIF_AHP_UDP_TUNNEL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_ESP, "LIF_AHP_UDP_ESP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_8, "LIF_AHP_UDP_SESSION_8");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_3].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_4].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_5].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_6].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_7].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_TUNNEL].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_ESP].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_UDP_LIF_AHP_UDP_SESSION_8].value_from_mapping = 10;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_vxlan_vxlan_gpe_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_AHP_VXLAN_VXLAN_GPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_AHP_VXLAN_VXLAN_GPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_DOUBLE_UDP_ESP_VXLAN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_NONE, "LIF_AHP_NONE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN, "LIF_AHP_VXLAN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN_GPE, "LIF_AHP_VXLAN_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_DOUBLE_UDP_ESP_VXLAN, "LIF_AHP_DOUBLE_UDP_ESP_VXLAN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_NONE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_VXLAN_GPE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LIF_AHP_VXLAN_VXLAN_GPE_LIF_AHP_DOUBLE_UDP_ESP_VXLAN].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_fwd1_csp_1bit_urpf_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_FWD1_CSP_1BIT_URPF_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_FWD1_CSP_1BIT_URPF_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_URPF_MODE_URPF_LOOKUP_ENABLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_URPF_MODE_URPF_LOOKUP_DISABLE, "URPF_LOOKUP_DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_URPF_MODE_URPF_LOOKUP_ENABLE, "URPF_LOOKUP_ENABLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_URPF_MODE_URPF_LOOKUP_DISABLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_FWD1_CSP_1BIT_URPF_MODE_URPF_LOOKUP_ENABLE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt1_csp_1bit_mim_namespace_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT1_CSP_1BIT_MIM_NAMESPACE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT1_CSP_1BIT_MIM_NAMESPACE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT1_CSP_1BIT_MIM_NAMESPACE_SOURCE_SCOPED_ISID + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT1_CSP_1BIT_MIM_NAMESPACE_NWK_SCOPED_ISID, "NWK_SCOPED_ISID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT1_CSP_1BIT_MIM_NAMESPACE_SOURCE_SCOPED_ISID, "SOURCE_SCOPED_ISID");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT1_CSP_1BIT_MIM_NAMESPACE_NWK_SCOPED_ISID].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT1_CSP_1BIT_MIM_NAMESPACE_SOURCE_SCOPED_ISID].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt4_csp_1bit_pwe_learn_native_ac_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC_ENABLE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC_DISABLE, "DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC_ENABLE, "ENABLE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC_DISABLE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_LEARN_NATIVE_AC_ENABLE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt4_csp_1bit_pwe_tag_namespace_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_LIF_SCOPED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_NWK_SCOPED, "NWK_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_LIF_SCOPED, "LIF_SCOPED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_NWK_SCOPED].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT4_CSP_1BIT_PWE_TAG_NAMESPACE_LIF_SCOPED].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_bridge_namespace_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE_LIF_SCOPED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE_NWK_SCOPED, "NWK_SCOPED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE_LIF_SCOPED, "LIF_SCOPED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE_NWK_SCOPED].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_BRIDGE_NAMESPACE_LIF_SCOPED].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_is_on_lag_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT5_CSP_1BIT_IS_ON_LAG];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT5_CSP_1BIT_IS_ON_LAG", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_IS_ON_LAG_TRUE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_IS_ON_LAG_FALSE, "FALSE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_IS_ON_LAG_TRUE, "TRUE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_IS_ON_LAG_FALSE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_IS_ON_LAG_TRUE].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_l3lif_mc_db_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_ONLY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_PUBLIC_OPTIMIZED, "MC_PRIVATE_PUBLIC_OPTIMIZED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_ONLY, "MC_PRIVATE_ONLY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_PUBLIC_OPTIMIZED].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_MC_DB_MODE_MC_PRIVATE_ONLY].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_l3lif_uc_db_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_ONLY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED, "UC_PRIVATE_PUBLIC_OPTIMIZED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_ONLY, "UC_PRIVATE_ONLY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_PUBLIC_OPTIMIZED].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_L3LIF_UC_DB_MODE_UC_PRIVATE_ONLY].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_nat_direction_and_virtual_ip_and_srv6_gsid_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID_NAT_DOWNSTREAM_VIP_ENABLED_SRV6_GSID_EOC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID_NAT_UPSTREAM_VIP_DISABLED_SRV6_GSID_SOC, "NAT_UPSTREAM_VIP_DISABLED_SRV6_GSID_SOC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID_NAT_DOWNSTREAM_VIP_ENABLED_SRV6_GSID_EOC, "NAT_DOWNSTREAM_VIP_ENABLED_SRV6_GSID_EOC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID_NAT_UPSTREAM_VIP_DISABLED_SRV6_GSID_SOC].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTT5_CSP_1BIT_NAT_DIRECTION_AND_VIRTUAL_IP_AND_SRV6_GSID_NAT_DOWNSTREAM_VIP_ENABLED_SRV6_GSID_EOC].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vttx_cs_evpn_iml_ah_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_VTTX_CS_EVPN_IML_AH];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_VTTX_CS_EVPN_IML_AH", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_FL_AND_CW + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_NO_FL, "NO_FL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_FL_ONLY, "FL_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_FL_AND_CW, "FL_AND_CW");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_NO_FL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_FL_ONLY].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LIF_VTTX_CS_EVPN_IML_AH_FL_AND_CW].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_xxxx_csp_1bit_l3lif_mpls_namespace_and_algo_acl_or_esp_vxlan_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN_MPLS_PER_INTERFACE_AACL_OR_ESP_VXLAN_ENABLED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN_MPLS_PER_PLATFORM_AACL_OR_ESP_VXLAN_DISABLED, "MPLS_PER_PLATFORM_AACL_OR_ESP_VXLAN_DISABLED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN_MPLS_PER_INTERFACE_AACL_OR_ESP_VXLAN_ENABLED, "MPLS_PER_INTERFACE_AACL_OR_ESP_VXLAN_ENABLED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN_MPLS_PER_PLATFORM_AACL_OR_ESP_VXLAN_DISABLED].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LIF_XXXX_CSP_1BIT_L3LIF_MPLS_NAMESPACE_AND_ALGO_ACL_OR_ESP_VXLAN_MPLS_PER_INTERFACE_AACL_OR_ESP_VXLAN_ENABLED].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_llr_cs_port_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_LLR_CS_PORT_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "LLR_CS_PORT_CS_PROFILE", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SA_BASED_CLASSIFICATION_PP_PORT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT, "TYPICAL_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_PON_STC_PORT, "PON_STC_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_PON_DTC_PORT, "PON_DTC_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_RAW_PORT, "RAW_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR, "802_1_BR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION, "SUPPORT_FRR_OPTIMIZATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION, "802_1_BR_WITH_FRR_OPTIMIZATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SA_BASED_CLASSIFICATION_PP_PORT, "SA_BASED_CLASSIFICATION_PP_PORT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_TYPICAL_PORT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_PON_STC_PORT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_PON_DTC_PORT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_RAW_PORT].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SUPPORT_FRR_OPTIMIZATION].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_802_1_BR_WITH_FRR_OPTIMIZATION].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_LLR_CS_PORT_CS_PROFILE_SA_BASED_CLASSIFICATION_PP_PORT].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mact_learn_format_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MACT_LEARN_FORMAT_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MACT_LEARN_FORMAT_INDEX", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_D + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_A, "MACT_ENTRY_VMV_A");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_B, "MACT_ENTRY_VMV_B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_C, "MACT_ENTRY_VMV_C");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_D, "MACT_ENTRY_VMV_D");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_A].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_B].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_C].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_MACT_LEARN_FORMAT_INDEX_MACT_ENTRY_VMV_D].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mac_table_aging_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MAC_TABLE_AGING_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MAC_TABLE_AGING_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MAC_TABLE_AGING_PROFILE_CPU_MANAGEMENT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MAC_TABLE_AGING_PROFILE_HW_MANAGEMENT, "HW_MANAGEMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MAC_TABLE_AGING_PROFILE_CPU_MANAGEMENT, "CPU_MANAGEMENT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MAC_TABLE_AGING_PROFILE_HW_MANAGEMENT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MAC_TABLE_AGING_PROFILE_CPU_MANAGEMENT].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mac_table_entry_event_forwarding_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE_CPU_MANAGEMENT + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE_HW_MANAGEMENT, "HW_MANAGEMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE_CPU_MANAGEMENT, "CPU_MANAGEMENT");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE_HW_MANAGEMENT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MAC_TABLE_ENTRY_EVENT_FORWARDING_PROFILE_CPU_MANAGEMENT].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mim_inlif_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MIM_INLIF_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MIM_INLIF_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MIM_NAMESPACE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_RESERVED_AG;
    cur_field_types_info->struct_field_info[1].length = 3;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mpls_extended_forward_domain_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MPLS_EXTENDED_FORWARD_DOMAIN_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MPLS_EXTENDED_FORWARD_DOMAIN_TYPE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MPLS_EXTENDED_FORWARD_DOMAIN_TYPE_MPLS_FWD_UA + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_EXTENDED_FORWARD_DOMAIN_TYPE_MPLS_FWD_PER_IF, "MPLS_FWD_PER_IF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MPLS_EXTENDED_FORWARD_DOMAIN_TYPE_MPLS_FWD_UA, "MPLS_FWD_UA");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MPLS_EXTENDED_FORWARD_DOMAIN_TYPE_MPLS_FWD_PER_IF].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MPLS_EXTENDED_FORWARD_DOMAIN_TYPE_MPLS_FWD_UA].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_my_mac_exem_da_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MY_MAC_EXEM_DA_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MY_MAC_EXEM_DA_PROFILE", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_IGNORE_MAC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_0, "LEGACY_VRID_BITMAP_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_1, "LEGACY_VRID_BITMAP_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_2, "LEGACY_VRID_BITMAP_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_3, "LEGACY_VRID_BITMAP_3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_4, "LEGACY_VRID_BITMAP_4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_5, "LEGACY_VRID_BITMAP_5");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_6, "LEGACY_VRID_BITMAP_6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_7, "LEGACY_VRID_BITMAP_7");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_8, "LEGACY_VRID_BITMAP_8");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_9, "LEGACY_VRID_BITMAP_9");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_10, "LEGACY_VRID_BITMAP_10");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_11, "LEGACY_VRID_BITMAP_11");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_12, "LEGACY_VRID_BITMAP_12");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_13, "LEGACY_VRID_BITMAP_13");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_14, "LEGACY_VRID_BITMAP_14");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_15, "LEGACY_VRID_BITMAP_15");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_MY_MAC, "PER_PORT_MY_MAC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0, "PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1, "PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_VRID_MY_MAC_WITH_PROTOCOL, "VRID_MY_MAC_WITH_PROTOCOL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_IGNORE_MAC, "PER_PORT_IGNORE_MAC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_3].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_4].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_5].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_6].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_7].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_8].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_9].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_10].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_11].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_12].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_13].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_14].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_LEGACY_VRID_BITMAP_15].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_MY_MAC].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1].value_from_mapping = 18;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_VRID_MY_MAC_WITH_PROTOCOL].value_from_mapping = 19;
        enum_info[DBAL_ENUM_FVAL_MY_MAC_EXEM_DA_PROFILE_PER_PORT_IGNORE_MAC].value_from_mapping = 20;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_nat_udh_struct_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NAT_UDH_STRUCT];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NAT_UDH_STRUCT", 96, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_L4_SRC_PORT_AG;
    cur_field_types_info->struct_field_info[0].length = 16;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_SIP_AG;
    cur_field_types_info->struct_field_info[1].length = 32;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_L4_DST_PORT_AG;
    cur_field_types_info->struct_field_info[2].length = 16;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_DIP_AG;
    cur_field_types_info->struct_field_info[3].length = 32;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_nof_sd_tags_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_NOF_SD_TAGS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "NOF_SD_TAGS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_NOF_SD_TAGS_X_SD_TAGS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_SD_TAGS_0_SD_TAGS, "0_SD_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_SD_TAGS_1_SD_TAGS, "1_SD_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_SD_TAGS_2_SD_TAGS, "2_SD_TAGS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_NOF_SD_TAGS_X_SD_TAGS, "X_SD_TAGS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_NOF_SD_TAGS_0_SD_TAGS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_NOF_SD_TAGS_1_SD_TAGS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_NOF_SD_TAGS_2_SD_TAGS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_NOF_SD_TAGS_X_SD_TAGS].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_oam_sub_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OAM_SUB_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OAM_SUB_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_TX + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL, "NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT, "LOSS_MEASUREMENT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588, "DELAY_MEASUREMENT_1588");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP, "DELAY_MEASUREMENT_NTP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK, "LOOPBACK");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM, "CCM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_1ST, "TWAMP_REFLECTOR_1ST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_2ND, "TWAMP_REFLECTOR_2ND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR, "OAM_LEVEL_ERROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR, "OAM_PASSIVE_ERROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_RFC8321, "RFC8321");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPV6_RFC8321_ON_FLOW_ID, "IPV6_RFC8321_ON_FLOW_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_BFDV6_CHECKSUM_ERROR, "BFDV6_CHECKSUM_ERROR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_SFLOW_SEQUENCE_RESET, "SFLOW_SEQUENCE_RESET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPFIX_TX_COMMAND, "IPFIX_TX_COMMAND");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_TX, "TWAMP_TX");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_1ST].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_2ND].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_RFC8321].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPV6_RFC8321_ON_FLOW_ID].value_from_mapping = 11;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_BFDV6_CHECKSUM_ERROR].value_from_mapping = 12;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_SFLOW_SEQUENCE_RESET].value_from_mapping = 13;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_IPFIX_TX_COMMAND].value_from_mapping = 14;
        enum_info[DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_TX].value_from_mapping = 15;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_out_pp_port_egress_forward_code_selection_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_RAW_PROCESSING + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_NORMAL, "NORMAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_STACKING_PORT, "STACKING_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_CPU_PORT, "CPU_PORT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_RAW_PROCESSING, "RAW_PROCESSING");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_NORMAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_STACKING_PORT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_CPU_PORT].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_OUT_PP_PORT_EGRESS_FORWARD_CODE_SELECTION_PROFILE_RAW_PROCESSING].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_parsing_start_type_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PARSING_START_TYPE_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PARSING_START_TYPE_ENCODING", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ITMH_VAL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INITIALIZATION, "INITIALIZATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ETHERNET, "ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS, "MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ARP, "ARP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FCOE, "FCOE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4_12B, "IPV4_12B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6_8B, "IPV6_8B");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT_PSP, "SRV6_ENDPOINT_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_DUMMY, "MPLS_DUMMY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_BIER_NON_MPLS, "BIER_NON_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IN_BIER_BASE_EG_BIER_MPLS, "IN_BIER_BASE_EG_BIER_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT, "SRV6_ENDPOINT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_TDM_BS, "TDM_BS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_DUMMY_ETHERNET, "DUMMY_ETHERNET");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_RCH_USP_PSP_AND_PSP_EXT, "SRV6_RCH_USP_PSP_AND_PSP_EXT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INGRESS_SCTP_EGRESS_FTMH, "INGRESS_SCTP_EGRESS_FTMH");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_PPP, "PPP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_UNTERM, "MPLS_UNTERM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_RCH_EXTENDED_ENCAP, "RCH_EXTENDED_ENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FORCE_MIRROR_OR_SS, "FORCE_MIRROR_OR_SS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_UNKNOWN, "UNKNOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ITMH_VAL, "ITMH_VAL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INITIALIZATION].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ETHERNET].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ARP].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FCOE].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV4_12B].value_from_mapping = 8;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6_8B].value_from_mapping = 9;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT_PSP].value_from_mapping = 10;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_DUMMY].value_from_mapping = 15;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_BIER_NON_MPLS].value_from_mapping = 16;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IN_BIER_BASE_EG_BIER_MPLS].value_from_mapping = 17;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_ENDPOINT].value_from_mapping = 20;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_TDM_BS].value_from_mapping = 21;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_DUMMY_ETHERNET].value_from_mapping = 22;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_SRV6_RCH_USP_PSP_AND_PSP_EXT].value_from_mapping = 23;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_INGRESS_SCTP_EGRESS_FTMH].value_from_mapping = 24;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_PPP].value_from_mapping = 26;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_MPLS_UNTERM].value_from_mapping = 27;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_RCH_EXTENDED_ENCAP].value_from_mapping = 28;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_FORCE_MIRROR_OR_SS].value_from_mapping = 29;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_UNKNOWN].value_from_mapping = 30;
        enum_info[DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_ITMH_VAL].value_from_mapping = 31;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_pppoe_s_or_d_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PPPOE_S_OR_D];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PPPOE_S_OR_D", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PPPOE_S_OR_D_DISCOVERY + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PPPOE_S_OR_D_SESSION, "SESSION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PPPOE_S_OR_D_DISCOVERY, "DISCOVERY");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PPPOE_S_OR_D_SESSION].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PPPOE_S_OR_D_DISCOVERY].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_prt_qualifier_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRT_QUALIFIER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRT_QUALIFIER", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRT_QUALIFIER_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRT_QUALIFIER_SET_VISIBILITY, "SET_VISIBILITY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION_DOWN, "OAMP_INJECTION_DOWN");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION, "OAMP_INJECTION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PRT_QUALIFIER_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PRT_QUALIFIER_SET_VISIBILITY].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION_DOWN].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_fwd1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_FWD1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_FWD1", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_EXTENDED_TERMINATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_STANDARD, "STANDARD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_NAT, "NAT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_2ND_PASS_EXTENDED_ENCAPSULATION, "2ND_PASS_EXTENDED_ENCAPSULATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_EXTENDED_TERMINATION, "EXTENDED_TERMINATION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_STANDARD].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_NAT].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_2ND_PASS_EXTENDED_ENCAPSULATION].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD1_EXTENDED_TERMINATION].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_fwd2_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_FWD2];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_FWD2", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD2_STANDARD + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD2_STANDARD, "STANDARD");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_FWD2_STANDARD].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_VTT1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_VTT1", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_FTMH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_RAW_MPLS, "RAW_MPLS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_IPSEC_MACSEC, "IPSEC_MACSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_FTMH, "FTMH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_RAW_MPLS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_IPSEC_MACSEC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT1_FTMH].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt2_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_VTT2];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_VTT2", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_2ND_PASS_DROP_AND_CONTINUE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_EXTENDED_ENCAPSULATION, "EXTENDED_ENCAPSULATION");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_IPSEC_MACSEC, "IPSEC_MACSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_2ND_PASS_DROP_AND_CONTINUE, "2ND_PASS_DROP_AND_CONTINUE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_EXTENDED_ENCAPSULATION].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_IPSEC_MACSEC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT2_2ND_PASS_DROP_AND_CONTINUE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt3_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_VTT3];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_VTT3", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_2ND_PASS_DROP_AND_CONTINUE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_P2P_ONLY, "P2P_ONLY");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_IPSEC_MACSEC, "IPSEC_MACSEC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_2ND_PASS_DROP_AND_CONTINUE, "2ND_PASS_DROP_AND_CONTINUE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_P2P_ONLY].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_IPSEC_MACSEC].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT3_2ND_PASS_DROP_AND_CONTINUE].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt4_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_VTT4];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_VTT4", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_IPSEC_MACSEC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_IPSEC_MACSEC, "IPSEC_MACSEC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT4_IPSEC_MACSEC].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt5_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_CS_PROFILE_VTT5];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_CS_PROFILE_VTT5", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_IPSEC_MACSEC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_IPSEC_MACSEC, "IPSEC_MACSEC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_CS_PROFILE_VTT5_IPSEC_MACSEC].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_fwd1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_FWD1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_FWD1", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_FWD2;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_llr_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_LLR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_LLR", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_VTT1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_VTT1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_VTT1", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_VTT2;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ROUTING_ENABLE_PROFILE_VTT1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt2_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_VTT2];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_VTT2", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_VTT3;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ROUTING_ENABLE_PROFILE_VTT1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt3_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_VTT3];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_VTT3", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_VTT4;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ROUTING_ENABLE_PROFILE_VTT1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt4_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_VTT4];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_VTT4", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_VTT5;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ROUTING_ENABLE_PROFILE_VTT1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt5_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_PROFILE_VTT5];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_PROFILE_VTT5", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_CS_PROFILE_FWD1;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ROUTING_ENABLE_PROFILE_VTT1;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_routing_enable_profile_vtt1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PTC_ROUTING_ENABLE_PROFILE_VTT1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PTC_ROUTING_ENABLE_PROFILE_VTT1", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_NEVERTERMINATE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_ALWAYSTERMINATE, "ALWAYSTERMINATE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_NEVERTERMINATE, "NEVERTERMINATE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_ALWAYSTERMINATE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_PTC_ROUTING_ENABLE_PROFILE_VTT1_NEVERTERMINATE].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_pwe_inlif_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PWE_INLIF_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PWE_INLIF_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_TAG_NAMESPACE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_NOF_SD_TAGS;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_LEARN_NATIVE_AC_BUG;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_forward_p1_explicit_remark_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_NON_ECN + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_EXPLICIT, "REMARK_TYPE_EXPLICIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_ALL, "REMARK_TYPE_INHERIT_REMARK_ALL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_NON_ECN, "REMARK_TYPE_INHERIT_REMARK_NON_ECN");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_EXPLICIT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_ALL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_NON_ECN].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_forward_p1_remark_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_FORWARD_P1_REMARK_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_FORWARD_P1_REMARK_PROFILE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_1 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_0, "INGRESS_REMARK_PROFILE_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_1, "INGRESS_REMARK_PROFILE_1");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_1].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_lr_forward_p1_type_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_LR_FORWARD_P1_TYPE_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_LR_FORWARD_P1_TYPE_INDEX", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_OTHER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_RESERVED, "RESERVED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_OTHER, "OTHER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV4].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV6].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_RESERVED].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_OTHER].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_typefwdplus1_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_QOS_TYPEFWDPLUS1_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "QOS_TYPEFWDPLUS1_TYPE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_OTHER + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_OTHER, "OTHER");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV4].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV6].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_OTHER].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_rch_type_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_RCH_TYPE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "RCH_TYPE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_RCH_TYPE_SRV6_EXT_ENCAPSULATION + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_DROPANDCONTINUE, "DROPANDCONTINUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_EXTENCAP, "EXTENCAP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_EXTTERM, "EXTTERM");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_REFLECTOR, "REFLECTOR");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_VRFREDIRECT, "VRFREDIRECT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_SRV6_USP_PSP, "SRV6_USP_PSP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_RCH_TYPE_SRV6_EXT_ENCAPSULATION, "SRV6_EXT_ENCAPSULATION");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_DROPANDCONTINUE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_EXTENCAP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_EXTTERM].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_REFLECTOR].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_VRFREDIRECT].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_SRV6_USP_PSP].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_RCH_TYPE_SRV6_EXT_ENCAPSULATION].value_from_mapping = 6;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_routing_fwd12fwd2_inlif_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ROUTING_FWD12FWD2_INLIF_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ROUTING_FWD12FWD2_INLIF_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_URPF_MODE;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_RESERVED_AG;
    cur_field_types_info->struct_field_info[1].length = 2;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_MC_MODE;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_routing_vtt2fwd1_inlif_cs_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ROUTING_VTT2FWD1_INLIF_CS_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ROUTING_VTT2FWD1_INLIF_CS_PROFILE", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_NAT_DIRECTION_OR_VIP_MODE_OR_SRV6_GSID;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MPLS_NAMESPACE_OR_AACL_OR_ESP_VXLAN;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_UC_DB_MODE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_MC_DB_MODE;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_srv6_additional_cs_bits_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRV6_ADDITIONAL_CS_BITS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SRV6_ADDITIONAL_CS_BITS", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_VPN_SID_COSTRUCT_FROM_UDH + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_NO_SRH_BUILD, "NO_SRH_BUILD");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_VPN_SID_COSTRUCT_FROM_UDH, "VPN_SID_COSTRUCT_FROM_UDH");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_NO_SRH_BUILD].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_SRV6_ADDITIONAL_CS_BITS_VPN_SID_COSTRUCT_FROM_UDH].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_srv6_encapsulation_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRV6_ENCAPSULATION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SRV6_ENCAPSULATION", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SRV6_ENCAPSULATION_REDUCED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_ENCAPSULATION_NORMAL, "NORMAL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_ENCAPSULATION_REDUCED, "REDUCED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_SRV6_ENCAPSULATION_NORMAL].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_SRV6_ENCAPSULATION_REDUCED].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_srv6_gsid_si_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SRV6_GSID_SI];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "SRV6_GSID_SI", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID0, "GSID0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID1, "GSID1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID2, "GSID2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID3, "GSID3");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_SRV6_GSID_SI_GSID3].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_tail_edit_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TAIL_EDIT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TAIL_EDIT_PROFILE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA2_0_INTERMEDIATE + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_DO_NOT_TAIL_EDIT, "DO_NOT_TAIL_EDIT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_TAIL, "IPT_TAIL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_UDP_GPE, "IPT_UDP_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA, "IPT_IFA");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_FIRST_UDP_GPE, "IPT_FIRST_UDP_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA2_0_INGRESS, "IPT_IFA2_0_INGRESS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_LAST_UDP_GPE, "IPT_LAST_UDP_GPE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA2_0_INTERMEDIATE, "IPT_IFA2_0_INTERMEDIATE");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_DO_NOT_TAIL_EDIT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_TAIL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_UDP_GPE].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_FIRST_UDP_GPE].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA2_0_INGRESS].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_LAST_UDP_GPE].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_TAIL_EDIT_PROFILE_IPT_IFA2_0_INTERMEDIATE].value_from_mapping = 7;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_tm_otmh_outlif_ext_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TM_OTMH_OUTLIF_EXT_MODE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TM_OTMH_OUTLIF_EXT_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_RESERVED + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_NEVER, "NEVER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_IF_MC, "IF_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_ALWAYS, "ALWAYS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_RESERVED, "RESERVED");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_NEVER].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_IF_MC].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_ALWAYS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_TM_OTMH_OUTLIF_EXT_MODE_RESERVED].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ttl_action_profile_value_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_TTL_ACTION_PROFILE_VALUE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "TTL_ACTION_PROFILE_VALUE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_MPLS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_IPV4, "IPV4");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_IPV6, "IPV6");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_MPLS, "MPLS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_IPV4].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_IPV6].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_TTL_ACTION_PROFILE_VALUE_MPLS].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_udh_data_type_to_data_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_UDH_DATA_TYPE_TO_DATA_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "UDH_DATA_TYPE_TO_DATA_SIZE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_0, "UDH_DT_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_1, "UDH_DT_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_2, "UDH_DT_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_3, "UDH_DT_3");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_1].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_2].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_UDH_DATA_TYPE_TO_DATA_SIZE_UDH_DT_3].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_udh_struct_highscale_ipv6_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_UDH_STRUCT_HIGHSCALE_IPV6];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "UDH_STRUCT_HIGHSCALE_IPV6", 64, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_IPV6_MSB_ARRAY_AG;
    cur_field_types_info->struct_field_info[0].length = 64;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_vsi_profile_bridge_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VSI_PROFILE_BRIDGE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VSI_PROFILE_BRIDGE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_SVL + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_IVL, "IVL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_SVL, "SVL");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_IVL].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_VSI_PROFILE_BRIDGE_SVL].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_vtt_save_context_profile_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VTT_SAVE_CONTEXT_PROFILE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VTT_SAVE_CONTEXT_PROFILE", 6, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_DEFAULT, "DEFAULT");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_S_TAG, "VTT1___BRIDGE___PON_DTC_S_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_C_TAG, "VTT1___BRIDGE___PON_DTC_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_SP_C_TAG, "VTT1___BRIDGE___PON_DTC_SP_C_TAG");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_UNTAGGED, "VTT1___BRIDGE___PON_DTC_UNTAGGED");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTTX___MPLS___EVPN_IML, "VTTX___MPLS___EVPN_IML");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTTX___SRV6___NEXT_SID_IS_NULL, "VTTX___SRV6___NEXT_SID_IS_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTTX___SRV6___NEXT_SID_IS_NOT_NULL, "VTTX___SRV6___NEXT_SID_IS_NOT_NULL");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP, "VTT1___BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_DEFAULT].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_S_TAG].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_C_TAG].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_SP_C_TAG].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PON_DTC_UNTAGGED].value_from_mapping = 4;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTTX___MPLS___EVPN_IML].value_from_mapping = 5;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTTX___SRV6___NEXT_SID_IS_NULL].value_from_mapping = 6;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTTX___SRV6___NEXT_SID_IS_NOT_NULL].value_from_mapping = 7;
        enum_info[DBAL_ENUM_FVAL_VTT_SAVE_CONTEXT_PROFILE_VTT1___BRIDGE___PP_PORT_SA_BASED_CLASSIFICATION_PP_PORT_LKP].value_from_mapping = 8;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ac_inlif_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_appdb_label_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ase_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_bridging_fwd12fwd2_inlif_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ctx_additional_header_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_current_protocol_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_egress_1st_parser_parser_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_egress_fwd_action_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_egress_fwd_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_erpp_ebtr_terminate_network_headers_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_erpp_ebtr_terminate_system_headers_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_esem1_default_result_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_esem2_default_result_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_esem_command_attribute_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_eth_qualifier_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_additional_header_profile_stack_attributes_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc1_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc2_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc3_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc4_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_enc5_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_encap_1_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_encap_2_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_esem_name_space_encoding_prp_ees_arr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_forward_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_fwd_code_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_next_protocol_namespace_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_php_ttl_model_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_prp2_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_prp2_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_qos_var_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_termination_port_cs_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_term_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_ace_ctxt_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_context_port_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_fwd_rcy_cmd_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etpp_trap_rcy_cmd_map_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etps_gtpv1u_raw_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_etps_srv6_raw_data_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_evpn_iml_inlif_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_exem_default_result_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_bier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ethernet_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_fallback_to_bridge_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_forward_header_remarking_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_forward_p1_header_remarking_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_forward_result_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_inject_indication_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ipv4_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ipv6_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_mpls_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fai_ttl_proccesing_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fcoe_first_additional_header_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fcoe_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_fodo_pd_result_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_forward_app_types_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_forward_domain_assignment_mode_enum_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_forward_domain_mask_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_gtp_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_gtp_version_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_incoming_tag_structure_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ingress_mpls_range_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_in_lif_same_interface_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_in_pp_port_flp_kg_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_in_pp_port_vtt_kg_var_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv4_additional_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv4_qualifier_tunnel_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv6_additional_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv6_dip_idx_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ipv6_extension_pre_srv6_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ip_address_spoofing_check_modes_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_1st_parser_parser_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_2nd_parser_parser_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_next_layer_network_domain_encoding_vtt_lif_arr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_irpp_system_header_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_jr_fwd_code_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2tp_d_or_c_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2_fodo_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2_v4_mc_fwd_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_l2_v6_mc_fwd_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_layer_types_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_layer_types_system_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_learn_payload_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_general_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_gre_geneve_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_ip_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_mpls_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_udp_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_ahp_vxlan_vxlan_gpe_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_fwd1_csp_1bit_urpf_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt1_csp_1bit_mim_namespace_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt4_csp_1bit_pwe_learn_native_ac_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt4_csp_1bit_pwe_tag_namespace_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_bridge_namespace_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_is_on_lag_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_l3lif_mc_db_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_l3lif_uc_db_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vtt5_csp_1bit_nat_direction_and_virtual_ip_and_srv6_gsid_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_vttx_cs_evpn_iml_ah_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_lif_xxxx_csp_1bit_l3lif_mpls_namespace_and_algo_acl_or_esp_vxlan_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_llr_cs_port_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mact_learn_format_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mac_table_aging_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mac_table_entry_event_forwarding_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mim_inlif_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_mpls_extended_forward_domain_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_my_mac_exem_da_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_nat_udh_struct_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_nof_sd_tags_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_oam_sub_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_out_pp_port_egress_forward_code_selection_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_parsing_start_type_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_pppoe_s_or_d_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_prt_qualifier_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_fwd1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_fwd2_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt2_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt3_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt4_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_cs_profile_vtt5_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_fwd1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_llr_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt2_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt3_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt4_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_profile_vtt5_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ptc_routing_enable_profile_vtt1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_pwe_inlif_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_forward_p1_explicit_remark_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_forward_p1_remark_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_lr_forward_p1_type_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_qos_typefwdplus1_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_rch_type_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_routing_fwd12fwd2_inlif_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_routing_vtt2fwd1_inlif_cs_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_srv6_additional_cs_bits_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_srv6_encapsulation_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_srv6_gsid_si_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_tail_edit_profile_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_tm_otmh_outlif_ext_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_ttl_action_profile_value_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_udh_data_type_to_data_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_udh_struct_highscale_ipv6_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_vsi_profile_bridge_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_profile_field_types_vtt_save_context_profile_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
