
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf1_qualifiers_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_QUALIFIERS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_QUALIFIERS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 159;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ALL_ONES;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_MEM_SOFT_ERR;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ACL_CONTEXT;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_NASID;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_FWD_CONTEXT;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_RPF_OUT_LIF;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_RPF_DEFAULT_ROUTE_FOUND;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_RPF_ROUTE_VALID;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_IS_APPLET;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_GENERAL_DATA_0;
    cur_field_types_info->struct_field_info[9].length = 128;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_GENERAL_DATA_1;
    cur_field_types_info->struct_field_info[10].length = 128;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_GENERAL_DATA_2;
    cur_field_types_info->struct_field_info[11].length = 128;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_FWD_DOMAIN_PROFILE;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_2;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_VLAN_EDIT_DEI_2;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_2;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_2;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_1;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_VLAN_EDIT_DEI_1;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_1;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_1;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_SNOOP_CODE;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_SNOOP_STRENGTH;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_RPF_DST;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_RPF_DST_VALID;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_PACKET_IS_IEEE1588;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_IEEE1588_COMMAND;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_IEEE1588_COMPENSATE_TIME_STAMP;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_IEEE1588_ENCAPSULATION;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_IEEE1588_HEADER_OFFSET;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_PACKET_IS_COMPATIBLE_MC;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_PRT_QUALIFIER_RAW;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_PACKET_HEADER_SIZE;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_MIRROR_CODE;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_VISIBILITY;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_PMF_OAM_ID;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_OAM_UP_MEP;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_OAM_SUB_TYPE;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_OAM_OFFSET;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_OAM_STAMP_OFFSET;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_LEARN_INFO;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_INGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_EGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[48].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_1;
    cur_field_types_info->struct_field_info[49].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[50].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_0;
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_1;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_FWD_ACTION_STRENGTH;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_FWD_ACTION_DESTINATION;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_FWD_ACTION_CPU_TRAP_CODE;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_FWD_ACTION_CPU_TRAP_QUAL;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_0;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_1;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_2;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_3;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_4;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_5;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_6;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_7;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_1;
    cur_field_types_info->struct_field_info[65].length = 88;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_0;
    cur_field_types_info->struct_field_info[66].length = 160;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_IN_PORT;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_LAG_LB_KEY;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_NWK_LB_KEY;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_0;
    cur_field_types_info->struct_field_info[71].length = 16;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_1;
    cur_field_types_info->struct_field_info[72].length = 16;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_2;
    cur_field_types_info->struct_field_info[73].length = 16;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_IN_TTL;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_BIER_STR_OFFSET;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_BIER_STR_SIZE;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_PACKET_IS_BIER;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_SAVED_CONTEXT_PROFILE;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_OAM_METER_DISABLE;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_INCOMING_TAG_STRUCTURE;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_METADATA_ECN;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_TRACE_PACKET;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_PACKET_HEADER_SIZE_RANGE;
    cur_field_types_info->struct_field_info[88].length = 2;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_IN_PORT_PMF_DATA;
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_IN_PORT_MAPPED_PP_PORT;
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_TM_PORT_PMF_DATA;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_VID_VALID;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_L4OPS;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_L4OPS_ENCODER0;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_L4OPS_ENCODER1;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_L4OPS_ENCODER2;
    cur_field_types_info->struct_field_info[97].struct_field_id = DBAL_FIELD_L4OPS_ENCODER3;
    cur_field_types_info->struct_field_info[98].struct_field_id = DBAL_FIELD_STATISTICS_OBJECT_10;
    cur_field_types_info->struct_field_info[98].length = 3;
    cur_field_types_info->struct_field_info[99].struct_field_id = DBAL_FIELD_CPU_TRAP_CODE_PROFILE;
    cur_field_types_info->struct_field_info[99].length = 1;
    cur_field_types_info->struct_field_info[100].struct_field_id = DBAL_FIELD_TM_FLOW_ID;
    cur_field_types_info->struct_field_info[101].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ID_3;
    cur_field_types_info->struct_field_info[102].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ID_2;
    cur_field_types_info->struct_field_info[103].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ID_1;
    cur_field_types_info->struct_field_info[104].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ID_0;
    cur_field_types_info->struct_field_info[105].struct_field_id = DBAL_FIELD_STATISTICS_ID_0;
    cur_field_types_info->struct_field_info[106].struct_field_id = DBAL_FIELD_STATISTICS_ID_1;
    cur_field_types_info->struct_field_info[107].struct_field_id = DBAL_FIELD_STATISTICS_ID_2;
    cur_field_types_info->struct_field_info[108].struct_field_id = DBAL_FIELD_STATISTICS_ID_3;
    cur_field_types_info->struct_field_info[109].struct_field_id = DBAL_FIELD_STATISTICS_ID_4;
    cur_field_types_info->struct_field_info[110].struct_field_id = DBAL_FIELD_STATISTICS_ID_5;
    cur_field_types_info->struct_field_info[111].struct_field_id = DBAL_FIELD_STATISTICS_ID_6;
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_STATISTICS_ID_7;
    cur_field_types_info->struct_field_info[113].struct_field_id = DBAL_FIELD_STATISTICS_ID_8;
    cur_field_types_info->struct_field_info[114].struct_field_id = DBAL_FIELD_STATISTICS_ID_9;
    cur_field_types_info->struct_field_info[115].struct_field_id = DBAL_FIELD_TM_FLOW_ATR;
    cur_field_types_info->struct_field_info[116].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ATR_3;
    cur_field_types_info->struct_field_info[117].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ATR_2;
    cur_field_types_info->struct_field_info[118].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ATR_1;
    cur_field_types_info->struct_field_info[119].struct_field_id = DBAL_FIELD_EXT_STATISTICS_ATR_0;
    cur_field_types_info->struct_field_info[120].struct_field_id = DBAL_FIELD_STATISTICS_ATR_0;
    cur_field_types_info->struct_field_info[121].struct_field_id = DBAL_FIELD_STATISTICS_ATR_1;
    cur_field_types_info->struct_field_info[122].struct_field_id = DBAL_FIELD_STATISTICS_ATR_2;
    cur_field_types_info->struct_field_info[123].struct_field_id = DBAL_FIELD_STATISTICS_ATR_3;
    cur_field_types_info->struct_field_info[124].struct_field_id = DBAL_FIELD_STATISTICS_ATR_4;
    cur_field_types_info->struct_field_info[125].struct_field_id = DBAL_FIELD_STATISTICS_ATR_5;
    cur_field_types_info->struct_field_info[126].struct_field_id = DBAL_FIELD_STATISTICS_ATR_6;
    cur_field_types_info->struct_field_info[127].struct_field_id = DBAL_FIELD_STATISTICS_ATR_7;
    cur_field_types_info->struct_field_info[128].struct_field_id = DBAL_FIELD_STATISTICS_ATR_8;
    cur_field_types_info->struct_field_info[129].struct_field_id = DBAL_FIELD_STATISTICS_ATR_9;
    cur_field_types_info->struct_field_info[130].struct_field_id = DBAL_FIELD_STATISTICS_META_DATA;
    cur_field_types_info->struct_field_info[131].struct_field_id = DBAL_FIELD_CONTEXT_KEY_GEN_VAR;
    cur_field_types_info->struct_field_info[132].struct_field_id = DBAL_FIELD_EGRESS_PARSING_INDEX;
    cur_field_types_info->struct_field_info[133].struct_field_id = DBAL_FIELD_FWD_LAYER_INDEX;
    cur_field_types_info->struct_field_info[134].struct_field_id = DBAL_FIELD_AC_IN_LIF_WIDE_DATA;
    cur_field_types_info->struct_field_info[135].struct_field_id = DBAL_FIELD_NATIVE_AC_IN_LIF_WIDE_DATA;
    cur_field_types_info->struct_field_info[135].length = 8;
    cur_field_types_info->struct_field_info[136].struct_field_id = DBAL_FIELD_PP_PORT_PMF_PROFILE;
    cur_field_types_info->struct_field_info[136].length = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    cur_field_types_info->struct_field_info[137].struct_field_id = DBAL_FIELD_TM_PORT_PMF_PROFILE;
    cur_field_types_info->struct_field_info[137].length = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    cur_field_types_info->struct_field_info[138].struct_field_id = DBAL_FIELD_PP_PORT_PMF_GENERAL_DATA;
    cur_field_types_info->struct_field_info[138].length = dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit);
    cur_field_types_info->struct_field_info[139].struct_field_id = DBAL_FIELD_PMF_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[140].struct_field_id = DBAL_FIELD_PMF_ETH_RIF_PROFILE;
    cur_field_types_info->struct_field_info[141].struct_field_id = DBAL_FIELD_UNKNOWN_ADDRESS;
    cur_field_types_info->struct_field_info[141].length = 1;
    cur_field_types_info->struct_field_info[142].struct_field_id = DBAL_FIELD_LEARN_SRC_MAC;
    cur_field_types_info->struct_field_info[143].struct_field_id = DBAL_FIELD_LEARN_VLAN;
    cur_field_types_info->struct_field_info[144].struct_field_id = DBAL_FIELD_LEARN_VSI;
    cur_field_types_info->struct_field_info[144].length = 18;
    cur_field_types_info->struct_field_info[145].struct_field_id = DBAL_FIELD_LEARN_DATA;
    cur_field_types_info->struct_field_info[145].length = 64;
    cur_field_types_info->struct_field_info[146].struct_field_id = DBAL_FIELD_LEARN_STATION_MOVE;
    cur_field_types_info->struct_field_info[147].struct_field_id = DBAL_FIELD_LEARN_MATCH;
    cur_field_types_info->struct_field_info[148].struct_field_id = DBAL_FIELD_LEARN_FOUND;
    cur_field_types_info->struct_field_info[149].struct_field_id = DBAL_FIELD_LEARN_EXPECTED_WON;
    cur_field_types_info->struct_field_info[150].struct_field_id = DBAL_FIELD_VRF_VALUE;
    cur_field_types_info->struct_field_info[151].struct_field_id = DBAL_FIELD_PP_PORT;
    cur_field_types_info->struct_field_info[152].struct_field_id = DBAL_FIELD_CORE;
    cur_field_types_info->struct_field_info[153].struct_field_id = DBAL_FIELD_VW_VIP_VALID;
    cur_field_types_info->struct_field_info[154].struct_field_id = DBAL_FIELD_VW_VIP_ID;
    cur_field_types_info->struct_field_info[155].struct_field_id = DBAL_FIELD_VW_MEMBER_REFERENCE;
    cur_field_types_info->struct_field_info[156].struct_field_id = DBAL_FIELD_VW_PCC_HIT;
    cur_field_types_info->struct_field_info[157].struct_field_id = DBAL_FIELD_AC_IN_LIF_WIDE_DATA_EXTENDED;
    cur_field_types_info->struct_field_info[157].length = 59;
    cur_field_types_info->struct_field_info[158].struct_field_id = DBAL_FIELD_MACT_ENTRY_GROUPING;
    cur_field_types_info->struct_field_info[158].length = 4;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf1_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_ACTIONS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 126;
    
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
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA1;
    cur_field_types_info->struct_field_info[51].length = 32;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA2;
    cur_field_types_info->struct_field_info[52].length = 32;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA3;
    cur_field_types_info->struct_field_info[53].length = 32;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_PPH_RESERVED;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_PPH_TYPE;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_RPF_DST;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_RPF_DST_VALID;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_RPF_OUT_LIF_ENCODED;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_STATISTICAL_SAMPLING;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_SNOOP_DATA;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_SLB_PAYLOAD0;
    cur_field_types_info->struct_field_info[64].length = 32;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_SLB_PAYLOAD1;
    cur_field_types_info->struct_field_info[65].length = 32;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_SLB_KEY0;
    cur_field_types_info->struct_field_info[66].length = 32;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_SLB_KEY1;
    cur_field_types_info->struct_field_info[67].length = 32;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_SLB_KEY2;
    cur_field_types_info->struct_field_info[68].length = 16;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_SLB_FOUND;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_STATE_ADDRESS_DATA;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_STATE_DATA;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_STATISTICS_ID_0;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_STATISTICS_ID_1;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_STATISTICS_ID_2;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_STATISTICS_ID_3;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_STATISTICS_ID_4;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_STATISTICS_ID_5;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_STATISTICS_ID_6;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_STATISTICS_ID_7;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_STATISTICS_ID_8;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_STATISTICS_ID_9;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_STATISTICS_ATR_0;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_STATISTICS_ATR_1;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_STATISTICS_ATR_2;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_STATISTICS_ATR_3;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_STATISTICS_ATR_4;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_STATISTICS_ATR_5;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_STATISTICS_ATR_6;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_STATISTICS_ATR_7;
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_STATISTICS_ATR_8;
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_STATISTICS_ATR_9;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_STATISTICS_META_DATA;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_STATISTICS_OBJECT_10;
    cur_field_types_info->struct_field_info[93].length = 3;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_ST_VSQ_PTR;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[97].struct_field_id = DBAL_FIELD_TM_PROFILE;
    cur_field_types_info->struct_field_info[98].struct_field_id = DBAL_FIELD_USER_HEADER_1;
    cur_field_types_info->struct_field_info[99].struct_field_id = DBAL_FIELD_USER_HEADER_1_TYPE;
    cur_field_types_info->struct_field_info[100].struct_field_id = DBAL_FIELD_USER_HEADER_2;
    cur_field_types_info->struct_field_info[101].struct_field_id = DBAL_FIELD_USER_HEADER_2_TYPE;
    cur_field_types_info->struct_field_info[102].struct_field_id = DBAL_FIELD_USER_HEADER_3;
    cur_field_types_info->struct_field_info[103].struct_field_id = DBAL_FIELD_USER_HEADER_3_TYPE;
    cur_field_types_info->struct_field_info[104].struct_field_id = DBAL_FIELD_USER_HEADER_4;
    cur_field_types_info->struct_field_info[105].struct_field_id = DBAL_FIELD_USER_HEADER_4_TYPE;
    cur_field_types_info->struct_field_info[106].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    cur_field_types_info->struct_field_info[107].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_1;
    cur_field_types_info->struct_field_info[108].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_2;
    cur_field_types_info->struct_field_info[109].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_1;
    cur_field_types_info->struct_field_info[110].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_2;
    cur_field_types_info->struct_field_info[111].struct_field_id = DBAL_FIELD_INGRESS_TIME_STAMP;
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_INT_DATA;
    cur_field_types_info->struct_field_info[113].struct_field_id = DBAL_FIELD_INGRESS_TIME_STAMP_OVERRIDE;
    cur_field_types_info->struct_field_info[114].struct_field_id = DBAL_FIELD_EVENTOR;
    cur_field_types_info->struct_field_info[115].struct_field_id = DBAL_FIELD_STAT_OBJ_LM_READ_INDEX;
    cur_field_types_info->struct_field_info[116].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_0;
    cur_field_types_info->struct_field_info[116].length = 32;
    cur_field_types_info->struct_field_info[117].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_1;
    cur_field_types_info->struct_field_info[117].length = 32;
    cur_field_types_info->struct_field_info[118].struct_field_id = DBAL_FIELD_LEARN_INFO_KEY_2_APP_DB_INDEX;
    cur_field_types_info->struct_field_info[118].length = 18;
    cur_field_types_info->struct_field_info[119].struct_field_id = DBAL_FIELD_LEARN_INFO_PAYLOAD_0;
    cur_field_types_info->struct_field_info[119].length = 32;
    cur_field_types_info->struct_field_info[120].struct_field_id = DBAL_FIELD_LEARN_INFO_PAYLOAD_1;
    cur_field_types_info->struct_field_info[120].length = 32;
    cur_field_types_info->struct_field_info[121].struct_field_id = DBAL_FIELD_LEARN_INFO_FORMAT_INDEX;
    cur_field_types_info->struct_field_info[121].length = 2;
    cur_field_types_info->struct_field_info[122].struct_field_id = DBAL_FIELD_VISIBILITY_CLEAR;
    cur_field_types_info->struct_field_info[123].struct_field_id = DBAL_FIELD_VISIBILITY;
    cur_field_types_info->struct_field_info[124].struct_field_id = DBAL_FIELD_INVALIDATE_NEXT;
    cur_field_types_info->struct_field_info[125].struct_field_id = DBAL_FIELD_INVALID;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf2_qualifiers_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF2_QUALIFIERS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF2_QUALIFIERS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 25;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_PMF1_TCAM_ACTION_0;
    cur_field_types_info->struct_field_info[0].length = 64;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PMF1_TCAM_ACTION_1;
    cur_field_types_info->struct_field_info[1].length = 64;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_PMF1_TCAM_ACTION_2;
    cur_field_types_info->struct_field_info[2].length = 64;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_PMF1_TCAM_ACTION_3;
    cur_field_types_info->struct_field_info[3].length = 64;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_PMF1_EXEM_ACTION;
    cur_field_types_info->struct_field_info[4].length = 64;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_PMF1_DIRECT_ACTION;
    cur_field_types_info->struct_field_info[5].length = 120;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_STATE_TABLE_DATA_SW;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_STATE_TABLE_ADDRESS_HW;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_KEY_F_INITIAL;
    cur_field_types_info->struct_field_info[8].length = 160;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_KEY_G_INITIAL;
    cur_field_types_info->struct_field_info[9].length = 160;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_KEY_H_INITIAL;
    cur_field_types_info->struct_field_info[10].length = 160;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_KEY_I_INITIAL;
    cur_field_types_info->struct_field_info[11].length = 160;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_KEY_J_INITIAL;
    cur_field_types_info->struct_field_info[12].length = 160;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_NWK_LB_KEY;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_LAG_LB_KEY;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_2;
    cur_field_types_info->struct_field_info[15].length = 16;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_1;
    cur_field_types_info->struct_field_info[16].length = 16;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_0;
    cur_field_types_info->struct_field_info[17].length = 16;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_TRJ_HASH;
    cur_field_types_info->struct_field_info[18].length = 16;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_CMP_KEY_0_DECODED;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_CMP_KEY_1_DECODED;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_CMP_KEY_TCAM_0_DECODED;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_CMP_KEY_TCAM_1_DECODED;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_NASID;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_STATE_TABLE_DATA_WRITE;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf3_qualifiers_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_QUALIFIERS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF3_QUALIFIERS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 132;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ALL_ONES;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_LEARN_INFO;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_RPF_ECMP_MODE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_RPF_ECMP_GROUP;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_ADMT_PROFILE;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_BIER_STR_OFFSET;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_BIER_STR_SIZE;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_BYTES_TO_REMOVE;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_WEAK_TM_VALID;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_WEAK_TM_PROFILE;
    cur_field_types_info->struct_field_info[9].length = 6;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_STAT_OBJ_LM_READ_INDEX;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_CONGESTION_INFO;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_END_OF_PACKET_EDITING;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_EGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_FWD_ACTION_CPU_TRAP_CODE;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_FWD_ACTION_CPU_TRAP_QUAL;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_DP_METER_COMMAND;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_FWD_ACTION_DESTINATION;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_FWD_ACTION_STRENGTH;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_FWD_PAYLOAD;
    cur_field_types_info->struct_field_info[22].length = 62;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_FWD_DOMAIN_PROFILE;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_GENERAL_DATA_IPMF3;
    cur_field_types_info->struct_field_info[25].length = 128;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_PEM_GENERAL_DATA3;
    cur_field_types_info->struct_field_info[26].length = 32;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_IEEE1588_COMMAND;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_IEEE1588_COMPENSATE_TIME_STAMP;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_IEEE1588_ENCAPSULATION;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_IEEE1588_HEADER_OFFSET;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_0;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_1;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_1;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_IN_PORT;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_CORE;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_IN_TTL;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_INGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_ITPP_DELTA;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_LAG_LB_KEY;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_LATENCY_FLOW_ID_VALID;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_LATENCY_FLOW_ID;
    cur_field_types_info->struct_field_info[42].length = 19;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_MIRROR_CODE;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_MIRROR_QUALIFIER;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_NWK_LB_KEY;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_PMF_OAM_ID;
    cur_field_types_info->struct_field_info[48].struct_field_id = DBAL_FIELD_OAM_OFFSET;
    cur_field_types_info->struct_field_info[49].struct_field_id = DBAL_FIELD_OAM_STAMP_OFFSET;
    cur_field_types_info->struct_field_info[50].struct_field_id = DBAL_FIELD_OAM_SUB_TYPE;
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_OAM_UP_MEP;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_2;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_3;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_IS_APPLET;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_NASID;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_PACKET_IS_BIER;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_PACKET_IS_IEEE1588;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_EGRESS_PARSING_INDEX;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_EXT_STATISTICS_VALID;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_PPH_RESERVED;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_PPH_TYPE;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_RPF_DST;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_RPF_DST_VALID;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_SNOOP_CODE;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_SNOOP_STRENGTH;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_SNOOP_QUALIFIER;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_ST_VSQ_PTR;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_STATISTICAL_SAMPLING_CODE;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_STATISTICAL_SAMPLING_QUALIFIER;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_STATISTICS_META_DATA;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_STATISTICS_OBJECT_10;
    cur_field_types_info->struct_field_info[76].length = 3;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_TM_PROFILE;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_USER_HEADER_1;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_USER_HEADER_2;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_USER_HEADER_3;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_USER_HEADER_4;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_USER_HEADER_1_TYPE;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_USER_HEADER_2_TYPE;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_USER_HEADER_3_TYPE;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_USER_HEADER_4_TYPE;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_USER_HEADERS_TYPE;
    cur_field_types_info->struct_field_info[87].length = 8;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_VISIBILITY;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_2;
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_VLAN_EDIT_VID_1;
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_VLAN_EDIT_DEI_2;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_2;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_2;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_VLAN_EDIT_DEI_1;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_1;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_VLAN_EDIT_PCP_DEI_1;
    cur_field_types_info->struct_field_info[97].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD_INDEX;
    cur_field_types_info->struct_field_info[98].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD;
    cur_field_types_info->struct_field_info[99].struct_field_id = DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[100].struct_field_id = DBAL_FIELD_SLB_LEARN_NEEDED;
    cur_field_types_info->struct_field_info[101].struct_field_id = DBAL_FIELD_SLB_KEY;
    cur_field_types_info->struct_field_info[101].length = 80;
    cur_field_types_info->struct_field_info[102].struct_field_id = DBAL_FIELD_SLB_PAYLOAD;
    cur_field_types_info->struct_field_info[102].length = 60;
    cur_field_types_info->struct_field_info[103].struct_field_id = DBAL_FIELD_TM_PORT_PMF_DATA;
    cur_field_types_info->struct_field_info[104].struct_field_id = DBAL_FIELD_IN_PORT_PMF_DATA;
    cur_field_types_info->struct_field_info[105].struct_field_id = DBAL_FIELD_IN_PORT_MAPPED_PP_PORT;
    cur_field_types_info->struct_field_info[106].struct_field_id = DBAL_FIELD_CONTEXT_KEY_GEN_VAR;
    cur_field_types_info->struct_field_info[107].struct_field_id = DBAL_FIELD_FER_STATISTICS_OBJ;
    cur_field_types_info->struct_field_info[107].length = 72;
    cur_field_types_info->struct_field_info[108].struct_field_id = DBAL_FIELD_FWD_LAYER_INDEX;
    cur_field_types_info->struct_field_info[109].struct_field_id = DBAL_FIELD_PP_PORT_PMF_PROFILE;
    cur_field_types_info->struct_field_info[109].length = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    cur_field_types_info->struct_field_info[110].struct_field_id = DBAL_FIELD_TM_PORT_PMF_PROFILE;
    cur_field_types_info->struct_field_info[110].length = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    cur_field_types_info->struct_field_info[111].struct_field_id = DBAL_FIELD_PP_PORT_PMF_GENERAL_DATA;
    cur_field_types_info->struct_field_info[111].length = dnx_data_field.profile_bits.nof_bits_in_ingress_pp_port_general_data_get(unit);
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_PMF_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[113].struct_field_id = DBAL_FIELD_PMF_ETH_RIF_PROFILE;
    cur_field_types_info->struct_field_info[114].struct_field_id = DBAL_FIELD_DUAL_QUEUE;
    cur_field_types_info->struct_field_info[114].length = dnx_data_field.signal_sizes.dual_queue_size_get(unit);
    cur_field_types_info->struct_field_info[115].struct_field_id = DBAL_FIELD_ELEPHANT_TRAP_INFO;
    cur_field_types_info->struct_field_info[115].length = 96;
    cur_field_types_info->struct_field_info[116].struct_field_id = DBAL_FIELD_ELEPHANT_LEARN_NEEDED;
    cur_field_types_info->struct_field_info[116].length = 1;
    cur_field_types_info->struct_field_info[117].struct_field_id = DBAL_FIELD_PRT_QUALIFIER_RAW;
    cur_field_types_info->struct_field_info[118].struct_field_id = DBAL_FIELD_ECL_VALID;
    cur_field_types_info->struct_field_info[118].length = 1;
    cur_field_types_info->struct_field_info[119].struct_field_id = DBAL_FIELD_BTK_OFFSET_SOP;
    cur_field_types_info->struct_field_info[119].length = 8;
    cur_field_types_info->struct_field_info[120].struct_field_id = DBAL_FIELD_BTC_OFFSET_SOP;
    cur_field_types_info->struct_field_info[120].length = 9;
    cur_field_types_info->struct_field_info[121].struct_field_id = DBAL_FIELD_BTC_MINUS_BTK;
    cur_field_types_info->struct_field_info[121].length = 9;
    cur_field_types_info->struct_field_info[122].struct_field_id = DBAL_FIELD_LEARN_SRC_MAC;
    cur_field_types_info->struct_field_info[123].struct_field_id = DBAL_FIELD_LEARN_VLAN;
    cur_field_types_info->struct_field_info[124].struct_field_id = DBAL_FIELD_LEARN_VSI;
    cur_field_types_info->struct_field_info[124].length = 18;
    cur_field_types_info->struct_field_info[125].struct_field_id = DBAL_FIELD_LEARN_DATA;
    cur_field_types_info->struct_field_info[125].length = 64;
    cur_field_types_info->struct_field_info[126].struct_field_id = DBAL_FIELD_LEARN_STATION_MOVE;
    cur_field_types_info->struct_field_info[127].struct_field_id = DBAL_FIELD_LEARN_MATCH;
    cur_field_types_info->struct_field_info[128].struct_field_id = DBAL_FIELD_LEARN_FOUND;
    cur_field_types_info->struct_field_info[129].struct_field_id = DBAL_FIELD_LEARN_EXPECTED_WON;
    cur_field_types_info->struct_field_info[130].struct_field_id = DBAL_FIELD_VRF_VALUE;
    cur_field_types_info->struct_field_info[131].struct_field_id = DBAL_FIELD_PP_PORT;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf3_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF3_ACTIONS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 113;
    
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
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_INVALID;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_ifwd2_qualifiers_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IFWD2_QUALIFIERS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IFWD2_QUALIFIERS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 134;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FWD_LAYER_INDEX;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_RPF_ROUTE_VALID;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_RESULT;
    cur_field_types_info->struct_field_info[3].length = 96;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_PROFILE;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_RESULT;
    cur_field_types_info->struct_field_info[5].length = 96;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_MP_PROFILE_SEL;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_RPF_DEFAULT_ROUTE_FOUND;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_ACL_CONTEXT;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_CONTEXT;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_NASID;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_ENABLE_PP_INJECT;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_EGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_FWD_ACTION_CPU_TRAP_CODE;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_FWD_ACTION_CPU_TRAP_QUAL;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_FWD_ACTION_DESTINATION;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_FWD_ACTION_STRENGTH;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_FWD_DOMAIN_PROFILE;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_GENERAL_DATA_0;
    cur_field_types_info->struct_field_info[21].length = 128;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_GENERAL_DATA_1;
    cur_field_types_info->struct_field_info[22].length = 128;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_GENERAL_DATA_2;
    cur_field_types_info->struct_field_info[23].length = 128;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_GENERAL_DATA_3;
    cur_field_types_info->struct_field_info[24].length = 126;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_AC_IN_LIF_WIDE_DATA;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_IEEE1588_COMMAND;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_IEEE1588_COMPENSATE_TIME_STAMP;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_IEEE1588_ENCAPSULATION;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_IEEE1588_HEADER_OFFSET;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_0;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_1;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_1;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_PP_PORT;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_INCOMING_TAG_STRUCTURE;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_INGRESS_LEARN_ENABLE;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_LEARN_INFO;
    cur_field_types_info->struct_field_info[37].length = 160;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_MEM_SOFT_ERR;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_OAM_OFFSET;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_OAM_STAMP_OFFSET;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_OAM_OPCODE;
    cur_field_types_info->struct_field_info[42].length = 4;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_OAM_YOUR_DISCR;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_MY_CFM_MAC;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_PACKET_IS_OAM;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_PACKET_IS_BFD;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_MDL;
    cur_field_types_info->struct_field_info[48].struct_field_id = DBAL_FIELD_VTT_OAM_LIF_0;
    cur_field_types_info->struct_field_info[49].struct_field_id = DBAL_FIELD_VTT_OAM_LIF_0_VALID;
    cur_field_types_info->struct_field_info[50].struct_field_id = DBAL_FIELD_VTT_OAM_LIF_1;
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_VTT_OAM_LIF_1_VALID;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_VTT_OAM_LIF_2;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_VTT_OAM_LIF_2_VALID;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_VTT_OAM_PCP_0;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_VTT_OAM_PCP_1;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_VTT_OAM_PCP_2;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_PRT_QUALIFIER_RAW;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_PACKET_HEADER_SIZE;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_IS_APPLET;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_PACKET_IS_COMPATIBLE_MC;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_PACKET_IS_IEEE1588;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_METADATA_ECN;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_RPF_DST;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_RPF_DST_VALID;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_SNOOP_CODE;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_SNOOP_STRENGTH;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_MIRROR_CODE;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_STATISTICAL_SAMPLING_CODE;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_STATISTICS_ID_0;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_STATISTICS_ID_1;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_STATISTICS_ID_2;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_STATISTICS_ID_3;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_STATISTICS_ID_4;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_STATISTICS_ID_5;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_STATISTICS_ATR_0;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_STATISTICS_ATR_1;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_STATISTICS_ATR_2;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_STATISTICS_ATR_3;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_STATISTICS_ATR_4;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_STATISTICS_ATR_5;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_VLAN_EDIT_CMD;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_TRACE_PACKET;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_IN_TTL;
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_LIF_OAM_TRAP_PROFILE;
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_VISIBILITY_CLEAR;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_VISIBILITY;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_RPF_OUT_LIF;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_ACTION_PROFILE_SA_DROP_INDEX;
    cur_field_types_info->struct_field_info[94].length = 2;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_ACTION_PROFILE_SA_NOT_FOUND_INDEX;
    cur_field_types_info->struct_field_info[95].length = 2;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_ACTION_PROFILE_DA_NOT_FOUND_INDEX;
    cur_field_types_info->struct_field_info[96].length = 1;
    cur_field_types_info->struct_field_info[97].struct_field_id = DBAL_FIELD_SAVED_CONTEXT_PROFILE;
    cur_field_types_info->struct_field_info[98].struct_field_id = DBAL_FIELD_IPV4_VERSION_ERROR;
    cur_field_types_info->struct_field_info[99].struct_field_id = DBAL_FIELD_IPV4_CHECKSUM_ERROR;
    cur_field_types_info->struct_field_info[100].struct_field_id = DBAL_FIELD_IPV4_TOTAL_LENGTH_ERROR;
    cur_field_types_info->struct_field_info[101].struct_field_id = DBAL_FIELD_IPV4_OPTIONS_ERROR;
    cur_field_types_info->struct_field_info[102].struct_field_id = DBAL_FIELD_IPV4_SIP_EQUAL_DIP_ERROR;
    cur_field_types_info->struct_field_info[103].struct_field_id = DBAL_FIELD_IPV4_DIP_ZERO_ERROR;
    cur_field_types_info->struct_field_info[104].struct_field_id = DBAL_FIELD_IPV4_SIP_IS_MC_ERROR;
    cur_field_types_info->struct_field_info[105].struct_field_id = DBAL_FIELD_IPV4_HEADER_LENGTH_ERROR;
    cur_field_types_info->struct_field_info[106].struct_field_id = DBAL_FIELD_IN_LIF_UNKNOWN_DA_PROFILE;
    cur_field_types_info->struct_field_info[106].length = 2;
    cur_field_types_info->struct_field_info[107].struct_field_id = DBAL_FIELD_NOF_VALID_LM_LIFS;
    cur_field_types_info->struct_field_info[108].struct_field_id = DBAL_FIELD_IS_IN_LIF_VALID_FOR_LM;
    cur_field_types_info->struct_field_info[109].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_STRENGTH;
    cur_field_types_info->struct_field_info[109].length = 5;
    cur_field_types_info->struct_field_info[110].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_STRENGTH;
    cur_field_types_info->struct_field_info[110].length = 5;
    cur_field_types_info->struct_field_info[111].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_FOUND;
    cur_field_types_info->struct_field_info[112].struct_field_id = DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_FOUND;
    cur_field_types_info->struct_field_info[113].struct_field_id = DBAL_FIELD_STATISTICS_META_DATA;
    cur_field_types_info->struct_field_info[114].struct_field_id = DBAL_FIELD_FWD_STAT_ATTRIBUTE_OBJ_CMD;
    cur_field_types_info->struct_field_info[115].struct_field_id = DBAL_FIELD_FWD_STAT_ATTRIBUTE_OBJ;
    cur_field_types_info->struct_field_info[116].struct_field_id = DBAL_FIELD_VSI_UNKNOWN_DA_DESTINATION;
    cur_field_types_info->struct_field_info[116].length = 21;
    cur_field_types_info->struct_field_info[117].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_0;
    cur_field_types_info->struct_field_info[117].length = 16;
    cur_field_types_info->struct_field_info[118].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_1;
    cur_field_types_info->struct_field_info[118].length = 16;
    cur_field_types_info->struct_field_info[119].struct_field_id = DBAL_FIELD_ECMP_LB_KEY_2;
    cur_field_types_info->struct_field_info[119].length = 16;
    cur_field_types_info->struct_field_info[120].struct_field_id = DBAL_FIELD_LAG_LB_KEY;
    cur_field_types_info->struct_field_info[121].struct_field_id = DBAL_FIELD_NWK_LB_KEY;
    cur_field_types_info->struct_field_info[122].struct_field_id = DBAL_FIELD_CONTEXT_CONSTANTS;
    cur_field_types_info->struct_field_info[122].length = 64;
    cur_field_types_info->struct_field_info[123].struct_field_id = DBAL_FIELD_PMF_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[124].struct_field_id = DBAL_FIELD_PMF_ETH_RIF_PROFILE;
    cur_field_types_info->struct_field_info[125].struct_field_id = DBAL_FIELD_VRF_VALUE;
    cur_field_types_info->struct_field_info[126].struct_field_id = DBAL_FIELD_UNKNOWN_ADDRESS;
    cur_field_types_info->struct_field_info[126].length = 1;
    cur_field_types_info->struct_field_info[127].struct_field_id = DBAL_FIELD_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[127].length = 8;
    cur_field_types_info->struct_field_info[128].struct_field_id = DBAL_FIELD_VW_VIP_VALID;
    cur_field_types_info->struct_field_info[129].struct_field_id = DBAL_FIELD_VW_VIP_ID;
    cur_field_types_info->struct_field_info[130].struct_field_id = DBAL_FIELD_VW_MEMBER_REFERENCE;
    cur_field_types_info->struct_field_info[131].struct_field_id = DBAL_FIELD_VW_PCC_HIT;
    cur_field_types_info->struct_field_info[132].struct_field_id = DBAL_FIELD_AC_IN_LIF_WIDE_DATA_EXTENDED;
    cur_field_types_info->struct_field_info[132].length = 59;
    cur_field_types_info->struct_field_info[133].struct_field_id = DBAL_FIELD_MACT_ENTRY_GROUPING;
    cur_field_types_info->struct_field_info[133].length = 4;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_epmf_qualifiers_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_QUALIFIERS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_QUALIFIERS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 97;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FTMH_RESERVED;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FTMH_VISIBILITY;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FTMH_TM_PROFILE;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FTMH_ECN_ENABLE;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_FTMH_CNI;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_FTMH_BIER_BFR_EXT_PRESENT;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_FTMH_FLOW_ID_EXT_PRESENT;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_FTMH_TM_DESTINATION_EXT_PRESENT;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_FTMH_TM_ACTION_IS_MC;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_FTMH_PPH_PRESENT;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_FTMH_TSH_EXT_PRESENT;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_FTMH_TM_ACTION_TYPE;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_FTMH_DP;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_PP_DSP;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_FTMH_TC;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_FTMH_PACKET_SIZE;
    cur_field_types_info->struct_field_info[18].length = 14;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_LB_KEY;
    cur_field_types_info->struct_field_info[19].length = 8;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_NWK_LB_KEY;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_STACKING_EXT;
    cur_field_types_info->struct_field_info[21].length = 16;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_BIER_BFR_EXT;
    cur_field_types_info->struct_field_info[22].length = 16;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_TM_DESTINATION_EXT;
    cur_field_types_info->struct_field_info[23].length = 24;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_APPLICATION_SPECIFIC_EXT;
    cur_field_types_info->struct_field_info[24].length = 48;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_TSH_EXT;
    cur_field_types_info->struct_field_info[25].length = 56;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_FWD_STRENGTH;
    cur_field_types_info->struct_field_info[26].length = 3;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_PPH_PARSING_START_TYPE;
    cur_field_types_info->struct_field_info[27].length = 5;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_PPH_PARSING_START_OFFSET;
    cur_field_types_info->struct_field_info[28].length = 7;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_PPH_LIF_EXT_TYPE;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_FHEI_SIZE;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_PPH_LEARN_EXT_PRESENT;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_PPH_TTL;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_PPH_IN_LIF;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_PPH_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_PPH_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_PPH_END_OF_PACKET_EDITING;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_PPH_VALUE1;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_PPH_VALUE2;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_FHEI_EXT;
    cur_field_types_info->struct_field_info[41].length = 64;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_LEARN_EXT;
    cur_field_types_info->struct_field_info[42].length = 152;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_LIF_EXT;
    cur_field_types_info->struct_field_info[43].length = 72;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_USER_HEADER_1;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_USER_HEADER_2;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_USER_HEADER_3;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_USER_HEADER_4;
    cur_field_types_info->struct_field_info[48].struct_field_id = DBAL_FIELD_USER_HEADER_1_TYPE;
    cur_field_types_info->struct_field_info[49].struct_field_id = DBAL_FIELD_USER_HEADER_2_TYPE;
    cur_field_types_info->struct_field_info[50].struct_field_id = DBAL_FIELD_USER_HEADER_3_TYPE;
    cur_field_types_info->struct_field_info[51].struct_field_id = DBAL_FIELD_USER_HEADER_4_TYPE;
    cur_field_types_info->struct_field_info[52].struct_field_id = DBAL_FIELD_IS_TDM;
    cur_field_types_info->struct_field_info[53].struct_field_id = DBAL_FIELD_CUD_OUTLIF_OR_MCDB_PTR;
    cur_field_types_info->struct_field_info[54].struct_field_id = DBAL_FIELD_DISCARD;
    cur_field_types_info->struct_field_info[55].struct_field_id = DBAL_FIELD_OUT_LIF_PROFILE;
    cur_field_types_info->struct_field_info[55].length = 4;
    cur_field_types_info->struct_field_info[56].struct_field_id = DBAL_FIELD_PMF_OUT_LIF_PROFILE;
    cur_field_types_info->struct_field_info[56].length = 1;
    cur_field_types_info->struct_field_info[57].struct_field_id = DBAL_FIELD_PMF_OUT_RIF_PROFILE;
    cur_field_types_info->struct_field_info[57].length = 1;
    cur_field_types_info->struct_field_info[58].struct_field_id = DBAL_FIELD_ETH_TAG_FORMAT;
    cur_field_types_info->struct_field_info[58].length = 10;
    cur_field_types_info->struct_field_info[59].struct_field_id = DBAL_FIELD_UPDATED_TPIDS_PACKET_DATA;
    cur_field_types_info->struct_field_info[59].length = 144;
    cur_field_types_info->struct_field_info[60].struct_field_id = DBAL_FIELD_IVE_BYTES_TO_ADD;
    cur_field_types_info->struct_field_info[60].length = 5;
    cur_field_types_info->struct_field_info[61].struct_field_id = DBAL_FIELD_IVE_BYTES_TO_REMOVE;
    cur_field_types_info->struct_field_info[61].length = 5;
    cur_field_types_info->struct_field_info[62].struct_field_id = DBAL_FIELD_ETHER_TYPE_CODE;
    cur_field_types_info->struct_field_info[62].length = 16;
    cur_field_types_info->struct_field_info[63].struct_field_id = DBAL_FIELD_FABRIC_OR_EGRESS_MC;
    cur_field_types_info->struct_field_info[64].struct_field_id = DBAL_FIELD_ESTIMATED_BTR;
    cur_field_types_info->struct_field_info[64].length = 8;
    cur_field_types_info->struct_field_info[65].struct_field_id = DBAL_FIELD_FWD_CONTEXT;
    cur_field_types_info->struct_field_info[66].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[67].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[68].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_2;
    cur_field_types_info->struct_field_info[69].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_3;
    cur_field_types_info->struct_field_info[70].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[71].struct_field_id = DBAL_FIELD_IN_LIF_PROFILE_0;
    cur_field_types_info->struct_field_info[72].struct_field_id = DBAL_FIELD_IP_MC_ELIGIBLE;
    cur_field_types_info->struct_field_info[73].struct_field_id = DBAL_FIELD_IP_MC_SHOULD_BE_BRIDGED;
    cur_field_types_info->struct_field_info[74].struct_field_id = DBAL_FIELD_LEARN_VALID;
    cur_field_types_info->struct_field_info[75].struct_field_id = DBAL_FIELD_LOCAL_OUT_LIF;
    cur_field_types_info->struct_field_info[76].struct_field_id = DBAL_FIELD_FWD_ACTION_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[77].struct_field_id = DBAL_FIELD_SNOOP_ACTION_PROFILE_INDEX;
    cur_field_types_info->struct_field_info[77].length = 2;
    cur_field_types_info->struct_field_info[78].struct_field_id = DBAL_FIELD_SNOOP_STRENGTH;
    cur_field_types_info->struct_field_info[79].struct_field_id = DBAL_FIELD_SYSTEM_HEADERS_SIZE;
    cur_field_types_info->struct_field_info[79].length = 7;
    cur_field_types_info->struct_field_info[80].struct_field_id = DBAL_FIELD_DST_SYS_PORT;
    cur_field_types_info->struct_field_info[80].length = 16;
    cur_field_types_info->struct_field_info[81].struct_field_id = DBAL_FIELD_TM_PORT;
    cur_field_types_info->struct_field_info[82].struct_field_id = DBAL_FIELD_OUT_PP_PORT;
    cur_field_types_info->struct_field_info[83].struct_field_id = DBAL_FIELD_IS_APPLET;
    cur_field_types_info->struct_field_info[84].struct_field_id = DBAL_FIELD_MEM_SOFT_ERR;
    cur_field_types_info->struct_field_info[85].struct_field_id = DBAL_FIELD_TC_MAP_PROFILE;
    cur_field_types_info->struct_field_info[86].struct_field_id = DBAL_FIELD_L4_PORT_IN_RANGE;
    cur_field_types_info->struct_field_info[86].length = 24;
    cur_field_types_info->struct_field_info[87].struct_field_id = DBAL_FIELD_PROGRAM_INDEX;
    cur_field_types_info->struct_field_info[87].length = 6;
    cur_field_types_info->struct_field_info[88].struct_field_id = DBAL_FIELD_PER_PORT_TABLE_DATA;
    cur_field_types_info->struct_field_info[88].length = 144;
    cur_field_types_info->struct_field_info[89].struct_field_id = DBAL_FIELD_PP_PORT_PMF_PROFILE;
    cur_field_types_info->struct_field_info[89].length = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    cur_field_types_info->struct_field_info[90].struct_field_id = DBAL_FIELD_TM_PORT_PMF_PROFILE;
    cur_field_types_info->struct_field_info[90].length = dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    cur_field_types_info->struct_field_info[91].struct_field_id = DBAL_FIELD_LEARN_SRC_MAC;
    cur_field_types_info->struct_field_info[92].struct_field_id = DBAL_FIELD_LEARN_VLAN;
    cur_field_types_info->struct_field_info[93].struct_field_id = DBAL_FIELD_LEARN_VSI;
    cur_field_types_info->struct_field_info[93].length = 18;
    cur_field_types_info->struct_field_info[94].struct_field_id = DBAL_FIELD_LEARN_DATA;
    cur_field_types_info->struct_field_info[94].length = 64;
    cur_field_types_info->struct_field_info[95].struct_field_id = DBAL_FIELD_PMF_IN_LIF_PROFILE;
    cur_field_types_info->struct_field_info[96].struct_field_id = DBAL_FIELD_PPH_VRF_VALUE;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_epmf_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_ACTIONS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 23;
    
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
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_INVALIDATE_NEXT;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_INVALID;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_ace_actions_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_ACE_ACTIONS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "ACE_ACTIONS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_l4ops_qualifiers_container_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS_QUALIFIERS_CONTAINER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS_QUALIFIERS_CONTAINER" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 48;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_DP;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_METADATA_ECN;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_0;
    cur_field_types_info->struct_field_info[4].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_1;
    cur_field_types_info->struct_field_info[5].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_2;
    cur_field_types_info->struct_field_info[6].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_3;
    cur_field_types_info->struct_field_info[7].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_4;
    cur_field_types_info->struct_field_info[8].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_5;
    cur_field_types_info->struct_field_info[9].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_6;
    cur_field_types_info->struct_field_info[10].struct_field_id = DBAL_FIELD_ELK_LKP_HIT_7;
    cur_field_types_info->struct_field_info[11].struct_field_id = DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_1;
    cur_field_types_info->struct_field_info[11].length = 88;
    cur_field_types_info->struct_field_info[12].struct_field_id = DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_0;
    cur_field_types_info->struct_field_info[12].length = 160;
    cur_field_types_info->struct_field_info[13].struct_field_id = DBAL_FIELD_FWD_DOMAIN;
    cur_field_types_info->struct_field_info[14].struct_field_id = DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO;
    cur_field_types_info->struct_field_info[15].struct_field_id = DBAL_FIELD_GENERAL_DATA_0;
    cur_field_types_info->struct_field_info[15].length = 128;
    cur_field_types_info->struct_field_info[16].struct_field_id = DBAL_FIELD_GENERAL_DATA_1;
    cur_field_types_info->struct_field_info[16].length = 128;
    cur_field_types_info->struct_field_info[17].struct_field_id = DBAL_FIELD_GENERAL_DATA_2;
    cur_field_types_info->struct_field_info[17].length = 128;
    cur_field_types_info->struct_field_info[18].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_1;
    cur_field_types_info->struct_field_info[19].struct_field_id = DBAL_FIELD_GLOB_IN_LIF_0;
    cur_field_types_info->struct_field_info[20].struct_field_id = DBAL_FIELD_NWK_QOS;
    cur_field_types_info->struct_field_info[21].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_1;
    cur_field_types_info->struct_field_info[22].struct_field_id = DBAL_FIELD_GLOB_OUT_LIF_0;
    cur_field_types_info->struct_field_info[23].struct_field_id = DBAL_FIELD_PRT_QUALIFIER_RAW;
    cur_field_types_info->struct_field_info[24].struct_field_id = DBAL_FIELD_RPF_DST;
    cur_field_types_info->struct_field_info[25].struct_field_id = DBAL_FIELD_RPF_DST_VALID;
    cur_field_types_info->struct_field_info[26].struct_field_id = DBAL_FIELD_RPF_OUT_LIF;
    cur_field_types_info->struct_field_info[27].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_0;
    cur_field_types_info->struct_field_info[28].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_1;
    cur_field_types_info->struct_field_info[29].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_2;
    cur_field_types_info->struct_field_info[30].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_3;
    cur_field_types_info->struct_field_info[31].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_4;
    cur_field_types_info->struct_field_info[32].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_5;
    cur_field_types_info->struct_field_info[33].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_6;
    cur_field_types_info->struct_field_info[34].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_7;
    cur_field_types_info->struct_field_info[35].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_8;
    cur_field_types_info->struct_field_info[36].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_9;
    cur_field_types_info->struct_field_info[37].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_10;
    cur_field_types_info->struct_field_info[38].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_11;
    cur_field_types_info->struct_field_info[39].struct_field_id = DBAL_FIELD_STATISTICS_OBJ_12;
    cur_field_types_info->struct_field_info[40].struct_field_id = DBAL_FIELD_OAM_STATISTICS_OBJ_0;
    cur_field_types_info->struct_field_info[41].struct_field_id = DBAL_FIELD_OAM_STATISTICS_OBJ_1;
    cur_field_types_info->struct_field_info[42].struct_field_id = DBAL_FIELD_OAM_STATISTICS_OBJ_2;
    cur_field_types_info->struct_field_info[43].struct_field_id = DBAL_FIELD_SRC_SYS_PORT;
    cur_field_types_info->struct_field_info[44].struct_field_id = DBAL_FIELD_TC;
    cur_field_types_info->struct_field_info[45].struct_field_id = DBAL_FIELD_CORE;
    cur_field_types_info->struct_field_info[46].struct_field_id = DBAL_FIELD_IN_PORT;
    cur_field_types_info->struct_field_info[47].struct_field_id = DBAL_FIELD_FWD_LAYER_INDEX;
    
    
    
    
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
_dbal_init_field_types_pmf_qualifiers_and_actions_misc_fields_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_MISC_FIELDS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "MISC_FIELDS" , 256 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_ALL_ZERO;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_VOID;
    
    
    
    
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
dbal_init_field_types_pmf_qualifiers_and_actions_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf1_qualifiers_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf1_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf2_qualifiers_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf3_qualifiers_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ipmf3_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ifwd2_qualifiers_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_epmf_qualifiers_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_epmf_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_ace_actions_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_l4ops_qualifiers_container_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_pmf_qualifiers_and_actions_misc_fields_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}
