
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf1_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPMF1_ACTIONS_CONTAINER", 256, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 128;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ACL_CONTEXT;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ADMT_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_BIER_STR_OFFSET;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_BIER_STR_SIZE;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_CONGESTION_INFO;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_DST_DATA;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_DP_METER_COMMAND;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_EGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_ELEPHANT_PAYLOAD;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_TRACE_PACKET_ACT;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_END_OF_PACKET_EDITING;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_EXT_STATISTICS_0;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_EXT_STATISTICS_1;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_EXT_STATISTICS_2;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_EXT_STATISTICS_3;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_FWD_TRAP;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_FWD_CONTEXT;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_FWD_LAYER_INDEX;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_IEEE1588_DATA;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_INGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_1;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_0;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_1;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_PP_PORT;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_IN_TTL;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_ITPP_DELTA_AND_VALID;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_LATENCY_FLOW;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_LEARN_INFO_0;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_LEARN_INFO_1;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_LEARN_INFO_2;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_LEARN_INFO_3;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_LEARN_INFO_4;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_LEARN_OR_TRANSPLANT;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_MIRROR_DATA;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_OAM_DATA;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_PMF_OAM_ID;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_2;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_3;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_OUT_LIF_PUSH;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_PACKET_IS_COMPATIBLE_MC;
    cur_field_types_info->struct_field_info[48].struct_field_id = DBAL_FIELD_PACKET_IS_BIER;
    cur_field_types_info->struct_field_info[49].struct_field_id = DBAL_FIELD_EGRESS_PARSING_INDEX;
    cur_field_types_info->struct_field_info[50].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA0;
    cur_field_types_info->struct_field_info[50].length = 32;
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA_CBTS;
    cur_field_types_info->struct_field_info[51].length = 19;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA1;
    cur_field_types_info->struct_field_info[52].length = 32;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA2;
    cur_field_types_info->struct_field_info[53].length = 32;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA3;
    cur_field_types_info->struct_field_info[54].length = 32;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_PPH_RESERVED;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_PPH_TYPE;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_RPF_DST;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_RPF_DST_VALID;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_RPF_OUT_LIF_ENCODED;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_STATISTICAL_SAMPLING;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_SNOOP_DATA;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_SLB_PAYLOAD0;
    cur_field_types_info->struct_field_info[65].length = 32;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_SLB_PAYLOAD1;
    cur_field_types_info->struct_field_info[66].length = 32;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_SLB_KEY0;
    cur_field_types_info->struct_field_info[67].length = 32;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_SLB_KEY1;
    cur_field_types_info->struct_field_info[68].length = 32;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_SLB_KEY2;
    cur_field_types_info->struct_field_info[69].length = 16;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_SLB_FOUND;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_STATE_ADDRESS_DATA;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_STATE_DATA;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_STATISTICS_ID_0;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_STATISTICS_ID_1;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_STATISTICS_ID_2;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_STATISTICS_ID_3;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_STATISTICS_ID_4;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_STATISTICS_ID_5;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_STATISTICS_ID_6;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_STATISTICS_ID_7;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_STATISTICS_ID_8;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_STATISTICS_ID_9;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_STATISTICS_ATR_0;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_STATISTICS_ATR_1;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_STATISTICS_ATR_2;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_STATISTICS_ATR_3;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_STATISTICS_ATR_4;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_STATISTICS_ATR_5;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_STATISTICS_ATR_6;
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_STATISTICS_ATR_7;
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_STATISTICS_ATR_8;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_STATISTICS_ATR_9;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_STATISTICS_META_DATA;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_STATISTICS_OBJECT_10;
    cur_field_types_info->struct_field_info[94].length = 3;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_ST_VSQ_PTR;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[97].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[98].struct_field_id = DBAL_FIELD_TM_PROFILE;
    cur_field_types_info->struct_field_info[99].struct_field_id = DBAL_FIELD_USER_HEADER_1;
    cur_field_types_info->struct_field_info[100].struct_field_id = DBAL_FIELD_USER_HEADER_1_TYPE;
    cur_field_types_info->struct_field_info[101].struct_field_id = DBAL_FIELD_USER_HEADER_2;
    cur_field_types_info->struct_field_info[102].struct_field_id = DBAL_FIELD_USER_HEADER_2_TYPE;
    cur_field_types_info->struct_field_info[103].struct_field_id = DBAL_FIELD_USER_HEADER_3;
    cur_field_types_info->struct_field_info[104].struct_field_id = DBAL_FIELD_USER_HEADER_3_TYPE;
    cur_field_types_info->struct_field_info[105].struct_field_id = DBAL_FIELD_USER_HEADER_4;
    cur_field_types_info->struct_field_info[106].struct_field_id = DBAL_FIELD_USER_HEADER_4_TYPE;
    cur_field_types_info->struct_field_info[107].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    cur_field_types_info->struct_field_info[108].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_1;
    cur_field_types_info->struct_field_info[109].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_2;
    cur_field_types_info->struct_field_info[110].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_1;
    cur_field_types_info->struct_field_info[111].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_2;
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_INGRESS_TIME_STAMP;
    cur_field_types_info->struct_field_info[113].struct_field_id = DBAL_FIELD_INT_DATA;
    cur_field_types_info->struct_field_info[114].struct_field_id = DBAL_FIELD_INGRESS_TIME_STAMP_OVERRIDE;
    cur_field_types_info->struct_field_info[115].struct_field_id = DBAL_FIELD_EVENTOR;
    cur_field_types_info->struct_field_info[116].struct_field_id = DBAL_FIELD_STAT_OBJ_LM_READ_INDEX;
    cur_field_types_info->struct_field_info[117].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_0;
    cur_field_types_info->struct_field_info[117].length = 32;
    cur_field_types_info->struct_field_info[118].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_1;
    cur_field_types_info->struct_field_info[118].length = 32;
    cur_field_types_info->struct_field_info[119].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_2_APP_DB_INDEX;
    cur_field_types_info->struct_field_info[119].length = 18;
    cur_field_types_info->struct_field_info[120].struct_field_id = DBAL_FIELD_LEARN_INFO_PAYLOAD_0;
    cur_field_types_info->struct_field_info[120].length = 32;
    cur_field_types_info->struct_field_info[121].struct_field_id = DBAL_FIELD_LEARN_INFO_PAYLOAD_1;
    cur_field_types_info->struct_field_info[121].length = 32;
    cur_field_types_info->struct_field_info[122].struct_field_id = DBAL_FIELD_LEARN_INFO_FORMAT_INDEX;
    cur_field_types_info->struct_field_info[122].length = 2;
    cur_field_types_info->struct_field_info[123].struct_field_id = DBAL_FIELD_PRT_QUALIFIER_RAW;
    cur_field_types_info->struct_field_info[124].struct_field_id = DBAL_FIELD_VISIBILITY_CLEAR;
    cur_field_types_info->struct_field_info[125].struct_field_id = DBAL_FIELD_VISIBILITY;
    cur_field_types_info->struct_field_info[126].struct_field_id = DBAL_FIELD_INVALIDATE_NEXT;
    cur_field_types_info->struct_field_info[127].struct_field_id = DBAL_FIELD_INVALID;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf3_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "IPMF3_ACTIONS_CONTAINER", 256, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 114;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ACL_CONTEXT;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_ADMT_PROFILE;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_BIER_STR_OFFSET;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_BIER_STR_SIZE;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_BYTES_TO_REMOVE;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_CONGESTION_INFO;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_DP_METER_COMMAND;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_EGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_END_OF_PACKET_EDITING;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_EXT_STATISTICS_0;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_EXT_STATISTICS_1;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_EXT_STATISTICS_2;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_EXT_STATISTICS_3;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_FWD_TRAP;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_DST_DATA;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_FWD_LAYER_INDEX;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_ACTION_GENERAL_DATA_0;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_ACTION_GENERAL_DATA_1;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_ACTION_GENERAL_DATA_2;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_ACTION_GENERAL_DATA_3;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_IEEE1588_DATA;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_INGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_INGRESS_TIME_STAMP;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_INGRESS_TIME_STAMP_OVERRIDE;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_INT_DATA;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_0;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_1;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_1;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_PP_PORT;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_IN_TTL;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_ITPP_DELTA_AND_VALID;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_LAG_LB_KEY;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_LATENCY_FLOW;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_LEARN_INFO_0;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_LEARN_INFO_1;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_LEARN_INFO_2;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_LEARN_INFO_3;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_LEARN_INFO_4;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_LEARN_OR_TRANSPLANT;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_MIRROR_DATA;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_NWK_LB_KEY;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_NWK_HEADER_APPEND_SIZE;
    cur_field_types_info->struct_field_info[48].struct_field_id = DBAL_FIELD_OAM_DATA;
    cur_field_types_info->struct_field_info[49].struct_field_id = DBAL_FIELD_PMF_OAM_ID;
    cur_field_types_info->struct_field_info[50].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_2;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_3;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_IS_APPLET;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_PACKET_IS_BIER;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_EGRESS_PARSING_INDEX;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_PPH_RESERVED;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_PPH_TYPE;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_PUSH_OUT_LIF;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_SNOOP_DATA;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_STATISTICAL_SAMPLING;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_STATISTICS_META_DATA;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_STATISTICS_ID_0;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_STATISTICS_ID_1;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_STATISTICS_ID_2;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_STATISTICS_ID_3;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_STATISTICS_ID_4;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_STATISTICS_ID_5;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_STATISTICS_ID_6;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_STATISTICS_ID_7;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_STATISTICS_ID_8;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_STATISTICS_ID_9;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_STATISTICS_ATR_0;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_STATISTICS_ATR_1;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_STATISTICS_ATR_2;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_STATISTICS_ATR_3;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_STATISTICS_ATR_4;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_STATISTICS_ATR_5;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_STATISTICS_ATR_6;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_STATISTICS_ATR_7;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_STATISTICS_ATR_8;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_STATISTICS_ATR_9;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_STATISTICS_OBJECT_10;
    cur_field_types_info->struct_field_info[85].length = 3;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_ST_VSQ_PTR;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_SYSTEM_HEADER_SIZE_ADJUST;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_TM_PROFILE;
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_USER_HEADER_1;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_USER_HEADER_2;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_USER_HEADER_3;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_USER_HEADER_4;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_USER_HEADERS_TYPE;
    cur_field_types_info->struct_field_info[95].length = 8;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_VISIBILITY;
    cur_field_types_info->struct_field_info[97].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    cur_field_types_info->struct_field_info[98].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_1;
    cur_field_types_info->struct_field_info[99].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_2;
    cur_field_types_info->struct_field_info[100].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_1;
    cur_field_types_info->struct_field_info[101].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_2;
    cur_field_types_info->struct_field_info[102].struct_field_id = DBAL_FIELD_PARSING_START_OFFSET;
    cur_field_types_info->struct_field_info[102].length = ((dnx_data_field.signal_sizes.parsing_start_offset_size_get(unit)-dnx_data_field.features.parsing_start_offset_msb_meaningless_get(unit))+1);
    cur_field_types_info->struct_field_info[103].struct_field_id = DBAL_FIELD_PARSING_START_TYPE_ACTION;
    cur_field_types_info->struct_field_info[104].struct_field_id = DBAL_FIELD_STAT_OBJ_LM_READ_INDEX;
    cur_field_types_info->struct_field_info[105].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_0;
    cur_field_types_info->struct_field_info[105].length = 32;
    cur_field_types_info->struct_field_info[106].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_1;
    cur_field_types_info->struct_field_info[106].length = 32;
    cur_field_types_info->struct_field_info[107].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_2_APP_DB_INDEX;
    cur_field_types_info->struct_field_info[107].length = 18;
    cur_field_types_info->struct_field_info[108].struct_field_id = DBAL_FIELD_LEARN_INFO_PAYLOAD_0;
    cur_field_types_info->struct_field_info[108].length = 32;
    cur_field_types_info->struct_field_info[109].struct_field_id = DBAL_FIELD_LEARN_INFO_PAYLOAD_1;
    cur_field_types_info->struct_field_info[109].length = 32;
    cur_field_types_info->struct_field_info[110].struct_field_id = DBAL_FIELD_LEARN_INFO_FORMAT_INDEX;
    cur_field_types_info->struct_field_info[110].length = 2;
    cur_field_types_info->struct_field_info[111].struct_field_id = DBAL_FIELD_INVALIDATE_NEXT;
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_TM_ACTION_TYPE;
    cur_field_types_info->struct_field_info[113].struct_field_id = DBAL_FIELD_INVALID;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_qualifiers_and_actions_epmf_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "EPMF_ACTIONS_CONTAINER", 256, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 27;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_DISCARD;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_TC_MAP_PROFILE;
    cur_field_types_info->struct_field_info[1].length = 3;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_CUD_OUTLIF_OR_MCDB_PTR;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_PP_DSP;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_COS_PROFILE;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_MIRROR_PROFILE;
    cur_field_types_info->struct_field_info[7].length = 2;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_SNOOP_PROFILE;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_SNOOP_STRENGTH;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_FWD_ACTION_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_FWD_STRENGTH;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_PMF_FWD_STRENGTH;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_PMF_SNOOP_STRENGTH;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_PMF_PROFILE;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_PMF_SNOOP_OVERRIDE_ENABLE;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_PMF_FWD_OVERRIDE_ENABLE;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_PMF_COUNTER_0_PROFILE;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_PMF_COUNTER_0_PTR;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_PMF_COUNTER_1_PROFILE;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_PMF_COUNTER_1_PTR;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_EBTR_SOP_DELTA;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_EBTR_EOP_DELTA;
    cur_field_types_info->struct_field_info[22].length = 10;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_EBTA_SOP_DELTA;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_EBTA_EOP_DELTA;
    cur_field_types_info->struct_field_info[24].length = 7;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_INVALIDATE_NEXT;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_INVALID;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_qualifiers_and_actions_ace_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACE_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "ACE_ACTIONS_CONTAINER", 256, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 34;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_PP_DSP;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FTMH_TC;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FTMH_DP;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FTMH_TM_ACTION_TYPE;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_FTMH_CNI;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_FTMH_ECN_ENABLE;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_FTMH_VISIBILITY;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_PPH_FORWARD_DOMAIN;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_PPH_VALUE1;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_PPH_VALUE2;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_FHEI_EXT31TO00;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_FHEI_EXT63TO32;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_PPH_TTL;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_PPH_IN_LIF;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_PPH_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_OUT_LIF0_CUD_OUT_LIF0VALID;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_OUT_LIF1_CUD_OUT_LIF1VALID;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_OUT_LIF2_CUD_OUT_LIF2VALID;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_OUT_LIF3_CUD_OUT_LIF3VALID;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_PPH_FWD_LAYER_INDEX;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_PPH_FORWANDING_STRENGTH;
    cur_field_types_info->struct_field_info[22].length = 1;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_PPH_END_OF_PACKET_EDITING;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_PPH_APPLICATION_SPECIFIC_EXT_31TO00;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_PPH_APPLICATION_SPECIFIC_EXT_47TO32;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_SNIFF_PROFILE;
    cur_field_types_info->struct_field_info[26].length = 4;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_FTMH_SOURCE_SYSTEM_PORT;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_ACE_CONTEXT_VALUE;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_ACE_STAMP_VALUE;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_ACE_STAT_METER_OBJ_CMD_ID;
    cur_field_types_info->struct_field_info[30].length = 25;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_CE_STAT_OBJ_CMD_ID_VALID;
    cur_field_types_info->struct_field_info[31].length = 26;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_INVALIDATE_NEXT;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_INVALID;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_pmf_qualifiers_and_actions_misc_fields_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MISC_FIELDS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "MISC_FIELDS", 256, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, FALSE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ALL_ZERO;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_VOID;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_pmf_qualifiers_and_actions_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf1_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf3_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_epmf_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ace_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_misc_fields_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
