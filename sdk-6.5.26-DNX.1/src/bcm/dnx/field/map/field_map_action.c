/** \file field_map_action.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX actions and access for per DNX action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/dnx/dbal/dbal_dynamic.h>

#include <bcm_int/dnx/rx/rx_trap.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <bcm/field.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#include "field_map_local.h"

#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
/*
 * }Include files
 */

extern const dnx_field_action_map_t dnx_global_action_map[bcmFieldActionCount];
extern const char *bcm_action_description[bcmFieldActionCount];
extern const dnx_field_base_action_info_t dnx_sw_action_info[DNX_FIELD_SW_ACTION_NOF];

static const char *dnx_field_action_class_names[DNX_FIELD_ACTION_CLASS_NOF] = {
    [DNX_FIELD_ACTION_CLASS_INVALID] = "Invalid",
    [DNX_FIELD_ACTION_CLASS_STATIC] = "Static",
    [DNX_FIELD_ACTION_CLASS_USER] = "User Defined",
    [DNX_FIELD_ACTION_CLASS_SW] = "SW"
};

static const dnx_field_action_type_t dnx_field_invalid_action[DNX_FIELD_STAGE_NOF] = {
    [DNX_FIELD_STAGE_IPMF1] = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID,
    [DNX_FIELD_STAGE_IPMF2] = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID,
    [DNX_FIELD_STAGE_IPMF3] = DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID,
    [DNX_FIELD_STAGE_EPMF] = DBAL_ENUM_FVAL_EPMF_ACTION_INVALID,
    [DNX_FIELD_STAGE_ACE] = DBAL_ENUM_FVAL_ACE_ACTION_INVALID,
};

static const dnx_field_action_type_t dnx_field_invalidate_next_action[DNX_FIELD_STAGE_NOF] = {
    [DNX_FIELD_STAGE_IPMF1] = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT,
    [DNX_FIELD_STAGE_IPMF2] = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT,
    [DNX_FIELD_STAGE_IPMF3] = DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT,
    [DNX_FIELD_STAGE_EPMF] = DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT,
    [DNX_FIELD_STAGE_ACE] = DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT,
};

typedef struct
{
    const char *name;
} dnx_field_action_name_t;

static const dnx_field_action_name_t dnx_action_name[DNX_FIELD_ACTION_NOF] = {
    [DNX_FIELD_ACTION_ID_INVALID] = {"A_ID_INVALID"},
    [DNX_FIELD_ACTION_ACL_CONTEXT] = {"A_ACL_CONTEXT"},
    [DNX_FIELD_ACTION_ADMT_PROFILE] = {"A_ADMT_PROFILE"},
    [DNX_FIELD_ACTION_BIER_STR_OFFSET] = {"A_BIER_STR_OFFSET"},
    [DNX_FIELD_ACTION_BIER_STR_SIZE] = {"A_BIER_STR_SIZE"},
    [DNX_FIELD_ACTION_BYTES_TO_REMOVE] = {"A_BYTES_TO_REMOVE"},
    [DNX_FIELD_ACTION_CONGESTION_INFO] = {"A_CONGESTION_INFO"},
    [DNX_FIELD_ACTION_COS_PROFILE] = {"A_COS_PROFILE"},
    [DNX_FIELD_ACTION_CUD_OUTLIF_OR_MCDB_PTR] = {"A_CUD_OUTLIF_OR_MCDB_PTR"},
    [DNX_FIELD_ACTION_DISCARD] = {"A_DISCARD"},
    [DNX_FIELD_ACTION_DP] = {"A_DP"},
    [DNX_FIELD_ACTION_DP_METER_COMMAND] = {"A_DP_METER_COMMAND"},
    [DNX_FIELD_ACTION_DST_DATA] = {"A_DST_DATA"},
    [DNX_FIELD_ACTION_DUAL_QUEUE] = {"A_DUAL_QUEUE"},
    [DNX_FIELD_ACTION_EEI] = {"A_EEI"},
    [DNX_FIELD_ACTION_EGRESS_LEARN_ENABLE] = {"A_EGRESS_LEARN_ENABLE"},
    [DNX_FIELD_ACTION_EGRESS_PARSING_INDEX] = {"A_EGRESS_PARSING_INDEX"},
    [DNX_FIELD_ACTION_ELEPHANT_PAYLOAD] = {"A_ELEPHANT_PAYLOAD"},
    [DNX_FIELD_ACTION_END_OF_PACKET_EDITING] = {"A_END_OF_PACKET_EDITING"},
    [DNX_FIELD_ACTION_EVENTOR] = {"A_EVENTOR"},
    [DNX_FIELD_ACTION_EXT_STATISTICS_0] = {"A_EXT_STATISTICS_0"},
    [DNX_FIELD_ACTION_EXT_STATISTICS_1] = {"A_EXT_STATISTICS_1"},
    [DNX_FIELD_ACTION_EXT_STATISTICS_2] = {"A_EXT_STATISTICS_2"},
    [DNX_FIELD_ACTION_EXT_STATISTICS_3] = {"A_EXT_STATISTICS_3"},
    [DNX_FIELD_ACTION_TRAP] = {"A_TRAP"},
    [DNX_FIELD_ACTION_FTMH_PP_DSP] = {"A_FTMH_PP_DSP"},
    [DNX_FIELD_ACTION_FWD_CONTEXT] = {"A_FWD_CONTEXT"},
    [DNX_FIELD_ACTION_FWD_DOMAIN] = {"A_FWD_DOMAIN"},
    [DNX_FIELD_ACTION_FWD_DOMAIN_WITH_PROFILE] = {"A_FWD_DOMAIN_WITH_PROFILE"},
    [DNX_FIELD_ACTION_FWD_LAYER_ADDITIONAL_INFO] = {"A_FWD_LAYER_ADDITIONAL_INFO"},
    [DNX_FIELD_ACTION_FWD_LAYER_INDEX] = {"A_FWD_LAYER_INDEX"},
    [DNX_FIELD_ACTION_FWD_STRENGTH] = {"A_FWD_STRENGTH"},
    [DNX_FIELD_ACTION_GLOB_IN_LIF_0] = {"A_GLOB_IN_LIF_0"},
    [DNX_FIELD_ACTION_GLOB_IN_LIF_1] = {"A_GLOB_IN_LIF_1"},
    [DNX_FIELD_ACTION_GLOB_OUT_LIF_0] = {"A_GLOB_OUT_LIF_0"},
    [DNX_FIELD_ACTION_GLOB_OUT_LIF_1] = {"A_GLOB_OUT_LIF_1"},
    [DNX_FIELD_ACTION_GLOB_OUT_LIF_2] = {"A_GLOB_OUT_LIF_2"},
    [DNX_FIELD_ACTION_GLOB_OUT_LIF_3] = {"A_GLOB_OUT_LIF_3"},
    [DNX_FIELD_ACTION_IEEE1588_DATA] = {"A_IEEE1588_DATA"},
    [DNX_FIELD_ACTION_IN_LIF_PROFILE_0] = {"A_IN_LIF_PROFILE_0"},
    [DNX_FIELD_ACTION_IN_LIF_PROFILE_1] = {"A_IN_LIF_PROFILE_1"},
    [DNX_FIELD_ACTION_IN_TTL] = {"A_IN_TTL"},
    [DNX_FIELD_ACTION_INGRESS_LEARN_ENABLE] = {"A_INGRESS_LEARN_ENABLE"},
    [DNX_FIELD_ACTION_INGRESS_TIME_STAMP] = {"A_INGRESS_TIME_STAMP"},
    [DNX_FIELD_ACTION_INGRESS_TIME_STAMP_OVERRIDE] = {"A_INGRESS_TIME_STAMP_OVERRIDE"},
    [DNX_FIELD_ACTION_INT_DATA] = {"A_INT_DATA"},
    [DNX_FIELD_ACTION_IS_APPLET] = {"A_IS_APPLET"},
    [DNX_FIELD_ACTION_ITPP_DELTA_AND_VALID] = {"A_ITPP_DELTA_AND_VALID"},
    [DNX_FIELD_ACTION_LAG_LB_KEY] = {"A_LAG_LB_KEY"},
    [DNX_FIELD_ACTION_LATENCY_FLOW] = {"A_LATENCY_FLOW"},
    [DNX_FIELD_ACTION_LEARN_INFO_0] = {"A_LEARN_INFO_0"},
    [DNX_FIELD_ACTION_LEARN_INFO_1] = {"A_LEARN_INFO_1"},
    [DNX_FIELD_ACTION_LEARN_INFO_2] = {"A_LEARN_INFO_2"},
    [DNX_FIELD_ACTION_LEARN_INFO_3] = {"A_LEARN_INFO_3"},
    [DNX_FIELD_ACTION_LEARN_INFO_4] = {"A_LEARN_INFO_4"},
    [DNX_FIELD_ACTION_LEARN_OR_TRANSPLANT] = {"A_LEARN_OR_TRANSPLANT"},
    [DNX_FIELD_ACTION_MIRROR_DATA] = {"A_MIRROR_DATA"},
    [DNX_FIELD_ACTION_MIRROR_PROFILE] = {"A_MIRROR_PROFILE"},
    [DNX_FIELD_ACTION_NWK_HEADER_APPEND_SIZE] = {"A_NWK_HEADER_APPEND_SIZE"},
    [DNX_FIELD_ACTION_NWK_LB_KEY] = {"A_NWK_LB_KEY"},
    [DNX_FIELD_ACTION_NWK_QOS] = {"A_NWK_QOS"},
    [DNX_FIELD_ACTION_OAM_DATA] = {"A_OAM_DATA"},
    [DNX_FIELD_ACTION_OUT_LIF_PUSH] = {"A_OUT_LIF_PUSH"},
    [DNX_FIELD_ACTION_PACKET_IS_BIER] = {"A_PACKET_IS_BIER"},
    [DNX_FIELD_ACTION_PACKET_IS_COMPATIBLE_MC] = {"A_PACKET_IS_COMPATIBLE_MC"},
    [DNX_FIELD_ACTION_PARSING_START_OFFSET] = {"A_PARSING_START_OFFSET"},
    [DNX_FIELD_ACTION_PARSING_START_TYPE] = {"A_PARSING_START_TYPE"},
    [DNX_FIELD_ACTION_PEM_GENERAL_DATA_CBTS] = {"A_PEM_GENERAL_DATA_CBTS"},
    [DNX_FIELD_ACTION_GENERAL_DATA0] = {"A_GENERAL_DATA0"},
    [DNX_FIELD_ACTION_GENERAL_DATA1] = {"A_GENERAL_DATA1"},
    [DNX_FIELD_ACTION_GENERAL_DATA2] = {"A_GENERAL_DATA2"},
    [DNX_FIELD_ACTION_GENERAL_DATA3] = {"A_GENERAL_DATA3"},
    [DNX_FIELD_ACTION_GENERAL_DATA4_CONTAINER0_0_31] = {"A_GENERAL_DATA4_CONTAINER0_0_31"},
    [DNX_FIELD_ACTION_PMF_COUNTER_0_PROFILE] = {"A_PMF_COUNTER_0_PROFILE"},
    [DNX_FIELD_ACTION_PMF_COUNTER_0_PTR] = {"A_PMF_COUNTER_0_PTR"},
    [DNX_FIELD_ACTION_PMF_COUNTER_1_PROFILE] = {"A_PMF_COUNTER_1_PROFILE"},
    [DNX_FIELD_ACTION_PMF_COUNTER_1_PTR] = {"A_PMF_COUNTER_1_PTR"},
    [DNX_FIELD_ACTION_PMF_FWD_OVERRIDE_ENABLE] = {"A_PMF_FWD_OVERRIDE_ENABLE"},
    [DNX_FIELD_ACTION_PMF_FWD_STRENGTH] = {"A_PMF_FWD_STRENGTH"},
    [DNX_FIELD_ACTION_PMF_OAM_ID] = {"A_PMF_OAM_ID"},
    [DNX_FIELD_ACTION_PMF_PROFILE] = {"A_PMF_PROFILE"},
    [DNX_FIELD_ACTION_PMF_SNOOP_OVERRIDE_ENABLE] = {"A_PMF_SNOOP_OVERRIDE_ENABLE"},
    [DNX_FIELD_ACTION_PMF_SNOOP_STRENGTH] = {"A_PMF_SNOOP_STRENGTH"},
    [DNX_FIELD_ACTION_PP_DSP] = {"A_PP_DSP"},
    [DNX_FIELD_ACTION_PP_PORT] = {"A_PP_PORT"},
    [DNX_FIELD_ACTION_PPH_RESERVED] = {"A_PPH_RESERVED"},
    [DNX_FIELD_ACTION_PPH_TYPE] = {"A_PPH_TYPE"},
    [DNX_FIELD_ACTION_PTC] = {"A_PTC"},
    [DNX_FIELD_ACTION_PUSH_OUT_LIF] = {"A_PUSH_OUT_LIF"},
    [DNX_FIELD_ACTION_RPF_DST] = {"A_RPF_DST"},
    [DNX_FIELD_ACTION_RPF_DST_VALID] = {"A_RPF_DST_VALID"},
    [DNX_FIELD_ACTION_RPF_OUT_LIF] = {"A_RPF_OUT_LIF"},
    [DNX_FIELD_ACTION_SLB_FOUND] = {"A_SLB_FOUND"},
    [DNX_FIELD_ACTION_SLB_KEY0] = {"A_SLB_KEY0"},
    [DNX_FIELD_ACTION_SLB_KEY1] = {"A_SLB_KEY1"},
    [DNX_FIELD_ACTION_SLB_KEY2] = {"A_SLB_KEY2"},
    [DNX_FIELD_ACTION_SLB_PAYLOAD0] = {"A_SLB_PAYLOAD0"},
    [DNX_FIELD_ACTION_SLB_PAYLOAD1] = {"A_SLB_PAYLOAD1"},
    [DNX_FIELD_ACTION_SNOOP] = {"A_SNOOP"},
    [DNX_FIELD_ACTION_SNOOP_STRENGTH] = {"A_SNOOP_STRENGTH"},
    [DNX_FIELD_ACTION_SRC_SYS_PORT] = {"A_SRC_SYS_PORT"},
    [DNX_FIELD_ACTION_ST_VSQ_PTR] = {"A_ST_VSQ_PTR"},
    [DNX_FIELD_ACTION_STACKING_ROUTE_HISTORY_BITMAP] = {"A_STACKING_ROUTE_HISTORY_BITMAP"},
    [DNX_FIELD_ACTION_STAT_OBJ_LM_READ_INDEX] = {"A_STAT_OBJ_LM_READ_INDEX"},
    [DNX_FIELD_ACTION_STATE_ADDRESS_DATA] = {"A_STATE_ADDRESS_DATA"},
    [DNX_FIELD_ACTION_STATE_DATA] = {"A_STATE_DATA"},
    [DNX_FIELD_ACTION_STATISTICAL_SAMPLING] = {"A_STATISTICAL_SAMPLING"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_0] = {"A_STATISTICS_ATR_0"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_1] = {"A_STATISTICS_ATR_1"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_2] = {"A_STATISTICS_ATR_2"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_3] = {"A_STATISTICS_ATR_3"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_4] = {"A_STATISTICS_ATR_4"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_5] = {"A_STATISTICS_ATR_5"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_6] = {"A_STATISTICS_ATR_6"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_7] = {"A_STATISTICS_ATR_7"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_8] = {"A_STATISTICS_ATR_8"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_9] = {"A_STATISTICS_ATR_9"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_10] = {"A_STATISTICS_ATR_10"},
    [DNX_FIELD_ACTION_STATISTICS_ATR_11] = {"A_STATISTICS_ATR_11"},
    [DNX_FIELD_ACTION_STATISTICS_ID_0] = {"A_STATISTICS_ID_0"},
    [DNX_FIELD_ACTION_STATISTICS_ID_1] = {"A_STATISTICS_ID_1"},
    [DNX_FIELD_ACTION_STATISTICS_ID_2] = {"A_STATISTICS_ID_2"},
    [DNX_FIELD_ACTION_STATISTICS_ID_3] = {"A_STATISTICS_ID_3"},
    [DNX_FIELD_ACTION_STATISTICS_ID_4] = {"A_STATISTICS_ID_4"},
    [DNX_FIELD_ACTION_STATISTICS_ID_5] = {"A_STATISTICS_ID_5"},
    [DNX_FIELD_ACTION_STATISTICS_ID_6] = {"A_STATISTICS_ID_6"},
    [DNX_FIELD_ACTION_STATISTICS_ID_7] = {"A_STATISTICS_ID_7"},
    [DNX_FIELD_ACTION_STATISTICS_ID_8] = {"A_STATISTICS_ID_8"},
    [DNX_FIELD_ACTION_STATISTICS_ID_9] = {"A_STATISTICS_ID_9"},
    [DNX_FIELD_ACTION_STATISTICS_ID_10] = {"A_STATISTICS_ID_10"},
    [DNX_FIELD_ACTION_STATISTICS_ID_11] = {"A_STATISTICS_ID_11"},
    [DNX_FIELD_ACTION_STATISTICS_META_DATA] = {"A_STATISTICS_META_DATA"},
    [DNX_FIELD_ACTION_STATISTICS_OBJECT_10] = {"A_STATISTICS_OBJECT_10"},
    [DNX_FIELD_ACTION_SYSTEM_HEADER_PROFILE_INDEX] = {"A_SYSTEM_HEADER_PROFILE_INDEX"},
    [DNX_FIELD_ACTION_SYSTEM_HEADER_SIZE_ADJUST] = {"A_SYSTEM_HEADER_SIZE_ADJUST"},
    [DNX_FIELD_ACTION_TC] = {"A_TC"},
    [DNX_FIELD_ACTION_TC_MAP_PROFILE] = {"A_TC_MAP_PROFILE"},
    [DNX_FIELD_ACTION_TM_PROFILE] = {"A_TM_PROFILE"},
    [DNX_FIELD_ACTION_TUNNEL_PRIORITY] = {"A_TUNNEL_PRIORITY"},
    [DNX_FIELD_ACTION_TRACE_PACKET_ACT] = {"A_TRACE_PACKET_ACT"},
    [DNX_FIELD_ACTION_USER_HEADER_1] = {"A_USER_HEADER_1"},
    [DNX_FIELD_ACTION_USER_HEADER_1_EXT] = {"A_USER_HEADER_1_EXT"},
    [DNX_FIELD_ACTION_USER_HEADER_1_TYPE] = {"A_USER_HEADER_1_TYPE"},
    [DNX_FIELD_ACTION_USER_HEADER_2] = {"A_USER_HEADER_2"},
    [DNX_FIELD_ACTION_USER_HEADER_2_EXT] = {"A_USER_HEADER_2_EXT"},
    [DNX_FIELD_ACTION_USER_HEADER_2_TYPE] = {"A_USER_HEADER_2_TYPE"},
    [DNX_FIELD_ACTION_USER_HEADER_3] = {"A_USER_HEADER_3"},
    [DNX_FIELD_ACTION_USER_HEADER_3_TYPE] = {"A_USER_HEADER_3_TYPE"},
    [DNX_FIELD_ACTION_USER_HEADER_4] = {"A_USER_HEADER_4"},
    [DNX_FIELD_ACTION_USER_HEADER_4_TYPE] = {"A_USER_HEADER_4_TYPE"},
    [DNX_FIELD_ACTION_USER_HEADERS_TYPE] = {"A_USER_HEADERS_TYPE"},
    [DNX_FIELD_ACTION_VISIBILITY] = {"A_VISIBILITY"},
    [DNX_FIELD_ACTION_VISIBILITY_CLEAR] = {"A_VISIBILITY_CLEAR"},
    [DNX_FIELD_ACTION_VLAN_EDIT_CMD_INDEX] = {"A_VLAN_EDIT_CMD_INDEX"},
    [DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_1] = {"A_VLAN_EDIT_PCP_DEI_1"},
    [DNX_FIELD_ACTION_VLAN_EDIT_PCP_DEI_2] = {"A_VLAN_EDIT_PCP_DEI_2"},
    [DNX_FIELD_ACTION_VLAN_EDIT_VID_1] = {"A_VLAN_EDIT_VID_1"},
    [DNX_FIELD_ACTION_VLAN_EDIT_VID_2] = {"A_VLAN_EDIT_VID_2"},
    [DNX_FIELD_ACTION_TM_ACTION_TYPE] = {"A_TM_ACTION_TYPE"},
    [DNX_FIELD_ACTION_CNI] = {"A_CNI"},
    [DNX_FIELD_ACTION_ECN_ENABLE] = {"A_ECN_ENABLE"},
    [DNX_FIELD_ACTION_PPH_VALUE1] = {"A_PPH_VALUE1"},
    [DNX_FIELD_ACTION_PPH_VALUE2] = {"A_PPH_VALUE2"},
    [DNX_FIELD_ACTION_FHEI_EXT31TO00] = {"A_FHEI_EXT31TO00"},
    [DNX_FIELD_ACTION_FHEI_EXT63TO32] = {"A_FHEI_EXT63TO32"},
    [DNX_FIELD_ACTION_PPH_TTL] = {"A_PPH_TTL"},
    [DNX_FIELD_ACTION_PPH_IN_LIF_PROFILE] = {"A_PPH_IN_LIF_PROFILE"},
    [DNX_FIELD_ACTION_OUT_LIF0_CUD_OUT_LIF0VALID] = {"A_OUT_LIF0_CUD_OUT_LIF0VALID"},
    [DNX_FIELD_ACTION_OUT_LIF1_CUD_OUT_LIF1VALID] = {"A_OUT_LIF1_CUD_OUT_LIF1VALID"},
    [DNX_FIELD_ACTION_OUT_LIF2_CUD_OUT_LIF2VALID] = {"A_OUT_LIF2_CUD_OUT_LIF2VALID"},
    [DNX_FIELD_ACTION_OUT_LIF3_CUD_OUT_LIF3VALID] = {"A_OUT_LIF3_CUD_OUT_LIF3VALID"},
    [DNX_FIELD_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00] = {"A_PPH_APPLICATION_SPECIFIC_EXT_31TO00"},
    [DNX_FIELD_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32] = {"A_PPH_APPLICATION_SPECIFIC_EXT_47TO32"},
    [DNX_FIELD_ACTION_SNIFF_PROFILE] = {"A_SNIFF_PROFILE"},
    [DNX_FIELD_ACTION_ACE_CONTEXT_VALUE] = {"A_ACE_CONTEXT_VALUE"},
    [DNX_FIELD_ACTION_ACE_STAMP_VALUE] = {"A_ACE_STAMP_VALUE"},
    [DNX_FIELD_ACTION_ACE_STAT_METER_OBJ_CMD_ID] = {"A_ACE_STAT_METER_OBJ_CMD_ID"},
    [DNX_FIELD_ACTION_ACE_STAT_OBJ_CMD_ID_VALID] = {"A_ACE_STAT_OBJ_CMD_ID_VALID"},
    [DNX_FIELD_ACTION_PPH_IN_LIF] = {"A_PPH_IN_LIF"},
    [DNX_FIELD_ACTION_PRT_QUALIFIER] = {"A_PRT_QUALIFIER"},
    [DNX_FIELD_ACTION_LEARN_INFO_KEY_0] = {"A_LEARN_INFO_KEY_0"},
    [DNX_FIELD_ACTION_LEARN_INFO_KEY_1] = {"A_LEARN_INFO_KEY_1"},
    [DNX_FIELD_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX] = {"A_LEARN_INFO_KEY_2_APP_DB_INDEX"},
    [DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_0] = {"A_LEARN_INFO_PAYLOAD_0"},
    [DNX_FIELD_ACTION_LEARN_INFO_PAYLOAD_1] = {"A_LEARN_INFO_PAYLOAD_1"},
    [DNX_FIELD_ACTION_LEARN_INFO_FORMAT_INDEX] = {"A_LEARN_INFO_FORMAT_INDEX"},
    [DNX_FIELD_ACTION_EBTR_EOP_DELTA] = {"A_EBTR_EOP_DELTA"},
    [DNX_FIELD_ACTION_EBTA_EOP_DELTA] = {"A_EBTA_EOP_DELTA"},
    [DNX_FIELD_ACTION_EBTA_SOP_DELTA] = {"A_EBTA_SOP_DELTA"},
    [DNX_FIELD_ACTION_EBTR_SOP_DELTA] = {"A_EBTR_SOP_DELTA"},
    [DNX_FIELD_ACTION_WEXEM_PAYLOAD_0_31] = {"A_WEXEM_PAYLOAD_0_31"},
    [DNX_FIELD_ACTION_WEXEM_PAYLOAD_32_59] = {"A_WEXEM_PAYLOAD_32_59"},
    [DNX_FIELD_ACTION_WEXEM_KEY_0_31] = {"A_WEXEM_KEY_0_31"},
    [DNX_FIELD_ACTION_WEXEM_KEY_32_63] = {"A_WEXEM_KEY_32_63"},
    [DNX_FIELD_ACTION_WEXEM_KEY_64_95] = {"A_WEXEM_KEY_64_95"},
    [DNX_FIELD_ACTION_WEXEM_KEY_96_127] = {"A_WEXEM_KEY_96_127"},
    [DNX_FIELD_ACTION_WEXEM_KEY_128_159] = {"A_WEXEM_KEY_128_159"},
    [DNX_FIELD_ACTION_WEXEM_LEARN_NEEDED] = {"A_WEXEM_LEARN_NEEDED"},
    [DNX_FIELD_ACTION_WEXEM_LEARN_APP_DB_ID] = {"A_WEXEM_LEARN_APP_DB_ID"},
    [DNX_FIELD_ACTION_OAM_TS_DATA] = {"A_OAM_TS_DATA"},
    [DNX_FIELD_ACTION_OUT_PP_PORT] = {"A_OUT_PP_PORT"},
    [DNX_FIELD_ACTION_OUT_TM_PORT] = {"A_OUT_TM_PORT"},
    [DNX_FIELD_ACTION_DSPA] = {"A_DSPA"},
    [DNX_FIELD_ACTION_EGR_TRAP_OVERWRITE] = {"A_EGR_TRAP_OVERWRITE"},
    [DNX_FIELD_ACTION_EGR_SNOOP_OVERWRITE] = {"A_EGR_SNOOP_OVERWRITE"},
    [DNX_FIELD_ACTION_ACE_STAT0_COUNTER] = {"A_ACE_STAT0_COUNTER"},
    [DNX_FIELD_ACTION_ACE_STAT1_COUNTER] = {"A_ACE_STAT1_COUNTER"},
    [DNX_FIELD_ACTION_FTMH_AQM_PROFILE] = {"A_FTMH_AQM_PROFILE"},
    [DNX_FIELD_ACTION_FHEI_EXT_31_00] = {"A_FHEI_EXT_31_00"},
    [DNX_FIELD_ACTION_FHEI_EXT_63_32] = {"A_FHEI_EXT_63_32"},
    [DNX_FIELD_ACTION_FTMH_ASE_31_00] = {"A_FTMH_ASE_31_00"},
    [DNX_FIELD_ACTION_FTMH_ASE_47_32] = {"A_FTMH_ASE_47_32"},
    [DNX_FIELD_ACTION_ACE_STAT_METER] = {"A_ACE_STAT_METER"},
    [DNX_FIELD_ACTION_ACE_STAT_LM_ENTRY] = {"A_ACE_STAT_LM_ENTRY"},
    [DNX_FIELD_ACTION_ACE_STAT_STATE_ENTRY] = {"A_ACE_STAT_STATE_ENTRY"},
    [DNX_FIELD_ACTION_ACE_STAT_STATE_SOIDA] = {"A_ACE_STAT_STATE_SOIDA"},
    [DNX_FIELD_ACTION_INEX_QOS] = {"A_INEX_QOS"},
    [DNX_FIELD_ACTION_NWK_QOS_PLUS1] = {"A_NWK_QOS_PLUS1"},
    [DNX_FIELD_ACTION_PORT_RATE_ADDRESS] = {"A_PORT_RATE_ADDRESS"},
    [DNX_FIELD_ACTION_GLOBAL_VOQ_AGG_ADDRESS] = {"A_GLOBAL_VOQ_AGG_ADDRESS"},
    [DNX_FIELD_ACTION_FWD_PLUS1_REMARK_PROFILE] = {"A_FWD_PLUS1_REMARK_PROFILE"},
    [DNX_FIELD_ACTION_FAI_DO_NOT_DECREMENT_TTL] = {"A_FAI_DO_NOT_DECREMENT_TTL"},
    [DNX_FIELD_ACTION_TYPE_FWD_REMARK_OR_KEEP] = {"A_TYPE_FWD_REMARK_OR_KEEP"},
    [DNX_FIELD_ACTION_EES_ENTRY_TYPE] = {"A_EES_ENTRY_TYPE"},
    [DNX_FIELD_ACTION_EES_ENTRY_119_90] = {"A_EES_ENTRY_119_90"},
    [DNX_FIELD_ACTION_EES_ENTRY_89_60] = {"A_EES_ENTRY_89_60"},
    [DNX_FIELD_ACTION_EES_ENTRY_59_30] = {"A_EES_ENTRY_59_30"},
    [DNX_FIELD_ACTION_EES_ENTRY_29_0] = {"A_EES_ENTRY_29_0"},
    [DNX_FIELD_ACTION_EES_ENTRY_PUSH_PROFILE] = {"A_EES_ENTRY_PUSH_PROFILE"},
    [DNX_FIELD_ACTION_BTC_OFFSET_IN_BYTES_FROM_SOP] = {"A_BTC_OFFSET_IN_BYTES_FROM_SOP"},
    [DNX_FIELD_ACTION_BTK_OFFSET_IN_BYTES_FROM_SOP] = {"A_BTK_OFFSET_IN_BYTES_FROM_SOP"},
    [DNX_FIELD_ACTION_NWK_HEADER_TRUNCATE_SIZE] = {"A_NWK_HEADER_TRUNCATE_SIZE"},
    [DNX_FIELD_ACTION_FORWARD_CODE] = {"A_FORWARD_CODE"},
    [DNX_FIELD_ACTION_ECL_VALID] = {"A_ECL_VALID"},
    [DNX_FIELD_ACTION_OUTLIF_TO_STAMP] = {"A_OUTLIF_TO_STAMP"},
    [DNX_FIELD_ACTION_INVALIDATE_NEXT] = {"A_INVALIDATE_NEXT"}
};

/**
 * \brief Checks if a action's mapping include value conversion.
 * \param [in] action_map_entry_p - Pointer to action mapping of type dnx_field_action_map_t.
 * \return
 *      TRUE  - If the mapping includes value conversion.
 *      FALSE - Otherwise.
 */
#define DNX_FIELD_MAP_ACTION_MAPPING_HAS_VALUE_CONVERSION(action_map_entry_p) \
    ((action_map_entry_p != NULL) && (action_map_entry_p->conversion_cb != NULL))

shr_error_e dbal_fields_field_types_info_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t ** field_type_info);

/**
* \brief
*  Gets all relevant action info that the end user might need and fills it in action_info_in_p.
* \param [in] unit              - Device ID
* \param [in] bcm_action        - BCM Action ID
* \param [out] action_info_p    - holds all relevant info for the action, size, stage, action and name.
* \param [out] dbal_field_id_p  - The DBAL field ID of the user defined action.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_user_action_get_int(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_action_in_info_t * action_info_p,
    dbal_fields_e * dbal_field_id_p)
{
    dnx_field_user_action_info_t user_action_info;
    dnx_field_action_id_t action_id;
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_in_p");

    if (bcm_action < dnx_data_field.action.user_1st_get(unit) ||
        bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
    /*
     * Cannot retrieve info on a user action which has not been created. Action is marked as 'invalid'.
     */
    if (valid == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

    sal_strncpy_s(action_info_p->name, dbal_field_to_string(unit, user_action_info.field_id), DBAL_MAX_STRING_LENGTH);
    action_info_p->size = user_action_info.size;
    action_info_p->prefix = user_action_info.prefix;
    action_info_p->prefix_size = user_action_info.prefix_size;
    action_info_p->stage = DNX_ACTION_STAGE(user_action_info.base_dnx_action);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, action_info_p->stage, user_action_info.base_dnx_action, &(action_info_p->bcm_action)));

    *dbal_field_id_p = user_action_info.field_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Provides all the information per DNX action requested for HW configuration
 * \param [in] unit - Identifier of HW platform.
 * \param [in] stage - Stage identifier
 * \param [in] dnx_action - HW action value
 * \param [in,out] dnx_action_info_p - pointer to DNX action info structure, data will be copied into it
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - One of input parameters is out of range
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  The assumption is that the amount of data is modest, so we can copy info data
 *  Pay attention that dnx_qual_info_t structure memory is in user responsibility
 */
static shr_error_e
dnx_field_map_action_info(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dnx_field_map_action_info_t * dnx_action_info_p)
{
    dnx_field_action_class_e dnx_action_class = DNX_ACTION_CLASS(dnx_action);
    dnx_field_stage_e dnx_stage = DNX_ACTION_STAGE(dnx_action);
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_action_info_p, _SHR_E_PARAM, "dnx_action_info_p");

    DNX_FIELD_STAGE_VERIFY(stage);
    if (stage != dnx_stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inconsistency between API:\"%s\" and encoded:\"%s\" stages\n",
                     dnx_field_stage_text(unit, stage), dnx_field_stage_text(unit, dnx_stage));
    }

    dnx_action_info_p->class = dnx_action_class;
    dnx_action_info_p->dnx_stage = stage;

    switch (dnx_action_class)
    {
        case DNX_FIELD_ACTION_CLASS_USER:
        {
            dnx_field_user_action_info_t user_action_info;
            dnx_field_action_t base_dnx_action;
            dnx_field_map_action_info_t base_dnx_action_info;
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
            /*
             * Cannot retrieve info for a user action that has not yet been created, in which case the
             * action is marked as 'invalid'.
             */
            if (valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The user action(%d) in stage %s does not exist!",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

            dnx_action_info_p->field_id = user_action_info.field_id;
            /*
             * In order to find the action type we check the action type of the base action.
             * Note that that is a recursive action that calls dnx_field_map_action_info(), but we rely on
             * the fact that the parent may not be a user defined action as a stopping condition.
             */
            base_dnx_action = user_action_info.base_dnx_action;
            SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, base_dnx_action, &base_dnx_action_info));
            dnx_action_info_p->action_type = base_dnx_action_info.action_type;
            dnx_action_info_p->base_dnx_action = base_dnx_action;
            dnx_action_info_p->prefix = user_action_info.prefix;
            dnx_action_info_p->prefix_size = user_action_info.prefix_size;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_STATIC:
        {
            if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(dnx_stage, action_id) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported action %d for stage %s \n",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.predefined.
                            field_id.get(unit, action_id, &dnx_action_info_p->field_id));
            /**
             * For static actions action_id is the same as action_type
             */
            dnx_action_info_p->action_type =
                (dnx_field_action_type_t) DNX_FIELD_DNX_ACTION_PARAM_GET(dnx_stage, action_id, id);
            dnx_action_info_p->base_dnx_action = DNX_FIELD_ACTION_INVALID;
            dnx_action_info_p->prefix = 0;
            dnx_action_info_p->prefix_size = 0;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_SW:
        {
            const dnx_field_base_action_info_t *sw_action_info_p;
            sw_action_info_p = &dnx_sw_action_info[action_id];
            if (sw_action_info_p->field_id == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "SW Action:%d is not supported\n", action_id);
            }
            dnx_action_info_p->field_id = sw_action_info_p->field_id;
            dnx_action_info_p->action_type = DNX_FIELD_ACTION_ID_INVALID;
            dnx_action_info_p->base_dnx_action = DNX_FIELD_ACTION_INVALID;
            dnx_action_info_p->prefix = 0;
            dnx_action_info_p->prefix_size = 0;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_VW:
        {
            int action_index_within_vw;
            dnx_field_vw_action_info_t vw_action_info;
            dnx_field_per_vw_action_info_t per_vw_action_info;
            SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get(unit, action_id, &vw_action_info));
            action_index_within_vw = vw_action_info.action_index_within_vw;
            SHR_IF_ERR_EXIT(dnx_field_map_per_vw_action_info_get(unit, action_id, &per_vw_action_info));
            if (per_vw_action_info.nof_actions_per_stage[stage] <= vw_action_info.action_index_within_vw)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The vw action(%d) does not exist!", action_id);
            }
            if (per_vw_action_info.field_id[action_index_within_vw] == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "VW \"%s\" has %d actions but action %d is empty.\n",
                             dbal_field_to_string(unit, per_vw_action_info.field_id[0]),
                             per_vw_action_info.nof_actions_per_stage[stage], vw_action_info.action_index_within_vw);
            }
            dnx_action_info_p->field_id = per_vw_action_info.field_id[action_index_within_vw];
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type
                            (unit, stage, per_vw_action_info.base_dnx_action_per_stage[action_index_within_vw][stage],
                             &(dnx_action_info_p->action_type)));
            dnx_action_info_p->base_dnx_action = DNX_FIELD_ACTION_INVALID;
            dnx_action_info_p->prefix = 0;
            dnx_action_info_p->prefix_size = 0;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Action Class:%d \n", dnx_action_class);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_bcm_action_text(
    int unit,
    bcm_field_action_t bcm_action)
{
    dbal_fields_e dbal_field_id;
    dnx_field_action_in_info_t action_info;
    static char *bcm_action_text[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;
    int user_defined_first = dnx_data_field.action.user_1st_get(unit);
    int user_defined_last = dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1;
    int vw_first = dnx_data_field.action.vw_1st_get(unit);
    int vw_last = dnx_data_field.action.vw_1st_get(unit) + dnx_data_field.action.vw_nof_get(unit) - 1;

    if ((int) bcm_action >= 0 && bcm_action < bcmFieldActionCount)
    {
        const char *bcm_action_name = NULL;
        /** Switch case to handle the unambiguous BCM actions. */
        switch (bcm_action)
        {
            case bcmFieldActionRedirect:
            {
                bcm_action_name = "Redirect";
                break;
            }
            default:
            {
                bcm_action_name = bcm_action_text[bcm_action];
                break;
            }
        }
        /** Predefined action.*/
        if (ISEMPTY(bcm_action_name))
        {
            return "Invalid BCM action";
        }
        return bcm_action_name;
    }
    else if ((bcm_action >= user_defined_first) && (bcm_action <= user_defined_last))
    {
        /** User defined action.*/
        if (dnx_field_user_action_get_int(unit, bcm_action, &action_info, &dbal_field_id) == _SHR_E_NONE)
        {
            return dbal_field_to_string(unit, dbal_field_id);
        }
        else
        {
            return "Invalid BCM action";
        }
    }
    else if ((bcm_action >= vw_first) && (bcm_action <= vw_last))
    {
        dnx_field_vw_action_info_t vw_action_info;
        dnx_field_per_vw_action_info_t per_vw_action_info;
        if (dnx_field_map_vw_action_info_get(unit, bcm_action - vw_first, &vw_action_info) != _SHR_E_NONE)
        {
            return "Invalid BCM action";
        }
        if (dnx_field_map_per_vw_action_info_get(unit, bcm_action - vw_first, &per_vw_action_info) != _SHR_E_NONE)
        {
            return "Invalid BCM action";
        }
        dbal_field_id = per_vw_action_info.field_id[vw_action_info.action_index_within_vw];
        if (dbal_field_id != DBAL_FIELD_EMPTY)
        {
            return dbal_field_to_string(unit, dbal_field_id);
        }
        else
        {
            return "Invalid BCM action";
        }
    }
    else
    {
        return "Invalid BCM action";
    }
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_dnx_action_text(
    int unit,
    dnx_field_action_t dnx_action)
{
    dnx_field_map_action_info_t dnx_action_info;

    if (dnx_field_map_action_info(unit, DNX_ACTION_STAGE(dnx_action), dnx_action, &dnx_action_info) != _SHR_E_NONE)
    {
        return "Invalid DNX Action";
    }
    return dbal_field_to_string(unit, dnx_action_info.field_id);
}

/*
 * See field_map.h
 */
char *
dnx_field_action_class_text(
    dnx_field_action_class_e action_class)
{
    char *class_name;
    if ((action_class < DNX_FIELD_ACTION_CLASS_FIRST) || (action_class >= DNX_FIELD_ACTION_CLASS_NOF))
    {
        class_name = "Invalid Class";
    }
    else if (dnx_field_action_class_names[action_class] == NULL)
    {
        class_name = "Unnamed Class";
    }
    else
    {
        class_name = (char *) dnx_field_action_class_names[action_class];
    }

    return class_name;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_ref_dec(
    int unit,
    dnx_field_action_t action)
{
    uint32 fg_refs = 0;
    int action_id = DNX_ACTION_ID(action);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.get(unit, action_id, &fg_refs));
    if (fg_refs <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action id %d didn't have any Field groups ref's (unused)\n", action);
    }
    fg_refs -= 1;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.set(unit, action_id, fg_refs));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_ref_inc(
    int unit,
    dnx_field_action_t action)
{
    uint32 fg_refs = 0;
    int action_id = DNX_ACTION_ID(action);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.get(unit, action_id, &fg_refs));
    fg_refs += 1;
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.ref_count.set(unit, action_id, fg_refs));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_in_info_t_init(
    int unit,
    dnx_field_action_in_info_t * action_info_in_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_info_in_p, _SHR_E_PARAM, "action_info_in_p");

    sal_memset(action_info_in_p, 0, sizeof(*action_info_in_p));

    /** Set the action_info structure fields to constant invalid values */
    action_info_in_p->bcm_action = bcmFieldActionCount;
    sal_strncpy_s(action_info_in_p->name, "", DBAL_MAX_STRING_LENGTH);
    action_info_in_p->size = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;
    action_info_in_p->stage = DNX_FIELD_STAGE_INVALID;
    /** Explicitly set the prefix and prefix size, despite the zeroing being done by sal_memset.*/
    action_info_in_p->prefix = 0;
    action_info_in_p->prefix_size = 0;

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  Clear the dnx_field_user_action_info_t, set it to preferred init values
* \param [in] unit              - Device ID
* \param [in] user_action_info  - Pointer to input structure of user_action_info that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_user_action_info_t_init(
    int unit,
    dnx_field_user_action_info_t * user_action_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_action_info, _SHR_E_PARAM, "user_action_info");

    sal_memset(user_action_info, 0x0, sizeof(*user_action_info));

    user_action_info->field_id = DBAL_FIELD_EMPTY;
    user_action_info->base_dnx_action = DNX_FIELD_ACTION_INVALID;
    user_action_info->bcm_id = bcmFieldActionCount;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_create(
    int unit,
    dnx_field_action_flags_e flags,
    dnx_field_action_in_info_t * action_info_in_p,
    bcm_field_action_t * bcm_action_p,
    dnx_field_action_t * dnx_action_p)
{
    dnx_field_action_id_t action_id;
    int alloc_flag = 0;
    bcm_field_action_t bcm_action = bcmFieldActionCount;
    dnx_field_user_action_info_t user_action_info;
    char field_name[DBAL_MAX_STRING_LENGTH];
    dnx_field_action_t base_dnx_action;
    unsigned int base_action_size;
    int is_void;
    int base_action_is_legal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_info_in_p, _SHR_E_PARAM, "action_info_in_p");
    SHR_NULL_CHECK(bcm_action_p, _SHR_E_PARAM, "bcm_action_p");
    DNX_FIELD_STAGE_VERIFY(action_info_in_p->stage);

    SHR_IF_ERR_EXIT(dnx_field_user_action_info_t_init(unit, &user_action_info));

    /** We are getting the DNX action so we can do error checks and assignments */
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit,
                                                    action_info_in_p->stage,
                                                    action_info_in_p->bcm_action, &base_dnx_action));
    /** Verify that the base DNX action isn't a user defined action.*/
    if (DNX_ACTION_CLASS(base_dnx_action) == DNX_FIELD_ACTION_CLASS_USER)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot base a used defined action on another user defined action.\n");
    }

    /** Verify that the base DNX action is usable on the device.*/
    SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal
                    (unit, action_info_in_p->stage, base_dnx_action, &base_action_is_legal));
    if (base_action_is_legal == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Base action (\"%s\") is not usable on this device.\n",
                     dnx_field_dnx_action_text(unit, base_dnx_action));
    }

    /** Check if we are based on a void action (and so the created action is also a void action).*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, action_info_in_p->stage, base_dnx_action, &is_void));

    /** Get the size of the base DNX action.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, action_info_in_p->stage, base_dnx_action, &base_action_size));

    /*
     * Preform size and prefix verifications, depending on whether the action is void or not.
     */
    if (is_void == FALSE)
    {
        /*
         * Verify that the number of bits in the prefix (excluding trailing zeros in the msb) does not exceed prefix size.
         * The prefix is a constant that would by appended to the msb of each action vlaue by the FES.
         * The prefix size is the number of bits appended.
         * We do not check prefix size for void actions.
         */
        if ((action_info_in_p->prefix_size < SAL_UINT32_NOF_BITS) &&
            ((action_info_in_p->prefix & SAL_FROM_BIT(action_info_in_p->prefix_size)) != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Prefix does not fit into the number of bits given by prefix_size. "
                         "values given are prefix 0x%X and prefix size %d.\n",
                         action_info_in_p->prefix, action_info_in_p->prefix_size);
        }

        /**
         * We check if the requested size, including the prefix size, is different from the base action size,
         * and return an error, it is not allowed.
         */
        if ((action_info_in_p->size + action_info_in_p->prefix_size) != base_action_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size set %d plus prefix size %d is %d, different from  the size of the "
                         "base action %s size: %d\n",
                         action_info_in_p->size,
                         action_info_in_p->prefix_size,
                         action_info_in_p->size + action_info_in_p->prefix_size,
                         dnx_field_bcm_action_text(unit, action_info_in_p->bcm_action), base_action_size);
        }

        /** Sanity check to protect against wrap around  */
        if (action_info_in_p->size > base_action_size || action_info_in_p->prefix_size > base_action_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size set %u plus prefix size %u is %u, wrapped around. \n",
                         action_info_in_p->size, action_info_in_p->prefix_size,
                         action_info_in_p->size + action_info_in_p->prefix_size);
        }
    }
    else
    {
        /** For void action, make sure that the prefix is zero. we do not care about the prefix size.*/
        if (action_info_in_p->prefix != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Void actions cannot have prefix. Received prefix 0x%X.\n",
                         action_info_in_p->prefix);
        }
        /*
         * Verify that the size of the user defined void action does not exceed the size of the predefined void action.
         * Note that there is no technical reason for this scenario not to work.
         */
        if (action_info_in_p->size > base_action_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size set %d for the void action exceeds the  size of the base action %d. "
                         "Base action is %s.\n",
                         action_info_in_p->size, base_action_size,
                         dnx_field_bcm_action_text(unit, action_info_in_p->bcm_action));
        }
    }

    if (flags & DNX_FIELD_ACTION_FLAG_WITH_ID)
    {
        uint32 valid;
        /*
         * BCM Action is leading number, we'll extract DNX one from BCM
         */
        bcm_action = *bcm_action_p;
        if ((bcm_action < dnx_data_field.action.user_1st_get(unit)) ||
            (bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action ID:%d is out of range:%d-%d\n", bcm_action,
                         dnx_data_field.action.user_1st_get(unit),
                         dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit));
        }
        action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
        if (valid == TRUE)
        {
            /*
             * Cannot create an action which is already marked as 'valid' (i.e., Has already
             * been created).
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action ID:%d was already created\n", bcm_action);
        }

        alloc_flag = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_user_action_id_allocate(unit, alloc_flag, &action_id));

    sal_strncpy_s(field_name, action_info_in_p->name, DBAL_MAX_STRING_LENGTH);

    SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name, &user_action_info.field_id));

    if (!(flags & DNX_FIELD_ACTION_FLAG_WITH_ID))
    {
        bcm_action = dnx_data_field.action.user_1st_get(unit) + action_id;
        *bcm_action_p = bcm_action;
    }

    user_action_info.flags = flags;
    user_action_info.ref_count = 0;
    user_action_info.size = action_info_in_p->size;
    user_action_info.prefix = action_info_in_p->prefix;
    user_action_info.prefix_size = action_info_in_p->prefix_size;
    user_action_info.base_dnx_action = base_dnx_action;
    user_action_info.valid = TRUE;
    user_action_info.bcm_id = bcm_action;

    /**If allocation did not fail we can set the values and SW state*/
    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.set(unit, action_id, &user_action_info));
    if (dnx_action_p != NULL)
    {
        *dnx_action_p = DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, action_info_in_p->stage, action_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_action_destroy(
    int unit,
    bcm_field_action_t bcm_action)
{
    dnx_field_action_id_t action_id;
    dnx_field_user_action_info_t user_action_info_get;
    dnx_field_user_action_info_t user_action_info_set;

    SHR_FUNC_INIT_VARS(unit);

    if ((bcm_action < dnx_data_field.action.user_1st_get(unit)) ||
        (bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action ID:%d is out of range:%d-%d\n", bcm_action,
                     dnx_data_field.action.user_1st_get(unit),
                     dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit));
    }

    action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info_get));

    if (user_action_info_get.ref_count > 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't Destroy, Reference number = %d\n", user_action_info_get.ref_count);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_user_action_id_deallocate(unit, action_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, user_action_info_get.field_id));

    SHR_IF_ERR_EXIT(dnx_field_user_action_info_t_init(unit, &user_action_info_set));

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.set(unit, action_id, &user_action_info_set));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_user_action_stage_get(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_stage_e * dnx_stage_p)
{

    dnx_field_user_action_info_t user_action_info;
    dnx_field_action_id_t action_id;
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_stage_p, _SHR_E_PARAM, "dnx_stage_p");

    if (bcm_action < dnx_data_field.action.user_1st_get(unit) ||
        bcm_action >= dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
    /*
     * Cannot retrieve info on a user action which has not been created. Action is marked as 'invalid'.
     */
    if (valid == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

    *dnx_stage_p = DNX_ACTION_STAGE(user_action_info.base_dnx_action);

exit:
    SHR_FUNC_EXIT;

}
/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_user_action_get(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_action_in_info_t * action_info_p)
{
    dbal_fields_e dbal_field_id;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_p");

    rv = dnx_field_user_action_get_int(unit, bcm_action, action_info_p, &dbal_field_id);

    if (rv == _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The user defined action (%d) is not within range (%d-%d)!",
                     bcm_action, dnx_data_field.action.user_1st_get(unit),
                     dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1);
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The user defined action (%d) does not exist!", bcm_action);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_bcm_to_dnx_int(
    int unit,
    dnx_field_stage_e dnx_stage,
    uint32 print_errors,
    bcm_field_action_t bcm_action,
    dnx_field_action_t * dnx_action_p,
    const dnx_field_action_map_t ** action_map_entry_pp)
{
    dnx_field_action_t dnx_action = 0;
    int bare_metal_support;
    int action_is_legal;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");
    /*
     * First check if the action is user defined one
     */
    if (DNX_FIELD_BCM_ACTION_IS_USER(unit, bcm_action))
    {
        uint32 valid;
        dnx_field_stage_e user_action_dnx_stage;
        /*
         * To obtain DNX from BCM just subtract 1st
         */
        dnx_field_action_id_t action_id = bcm_action - dnx_data_field.action.user_1st_get(unit);
        /*
         * Verify validity of user defined action
         */
        SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
        if (valid == FALSE)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "User defined BCM Action %s(%d) doesn't exist!%s",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        /*
         * Verify the stage
         */
        SHR_IF_ERR_EXIT(dnx_field_user_action_stage_get(unit, bcm_action, &user_action_dnx_stage));
        if (dnx_stage != user_action_dnx_stage)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "User defined BCM Action %s stage %s does not match requested stage: %s",
                             dnx_field_bcm_action_text(unit, bcm_action), dnx_field_stage_text(unit,
                                                                                               user_action_dnx_stage),
                             dnx_field_stage_text(unit, dnx_stage));
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }

        dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, dnx_stage, action_id);
    }
    else if (((int) bcm_action >= 0) && (bcm_action < bcmFieldActionCount))
    {
        *action_map_entry_pp = &dnx_global_action_map[bcm_action];
        /*
         * Static action
         */
        /*
         * Check if the action is a global action, otherwise it is a stage specific action.
         */
        if ((*action_map_entry_pp)->dnx_action != DNX_FIELD_ACTION_ID_INVALID)
        {
            dnx_action = (*action_map_entry_pp)->dnx_action;
            /** Add the stage to the action.*/
            dnx_action |= (dnx_stage << DNX_ACTION_STAGE_SHIFT);
        }
        else
        {
            /*
             * If it is not a global action, then it is a stage specific action.
             */
            uint8 is_std_1;
            if (dnx_field_map_stage_info[dnx_stage].static_action_id_map == NULL)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            (*action_map_entry_pp) = &dnx_field_map_stage_info[dnx_stage].static_action_id_map[bcm_action];
            if ((*action_map_entry_pp)->dnx_action == DNX_FIELD_ACTION_ID_INVALID)
            {
                if (print_errors)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                 "BCM Action %s is not supported for stage: %s!\n "
                                 "To see the supported actions, run: field action predefined BCM stage=%s",
                                 dnx_field_bcm_action_text(unit, bcm_action), dnx_field_stage_text(unit, dnx_stage),
                                 dnx_field_stage_text(unit, dnx_stage));
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                    SHR_EXIT();
                }
            }

            /*
             * Map BCM action to DNX
             */
            SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
            if (!is_std_1 && ((*action_map_entry_pp)->conversion_cb != NULL))
            {
                SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check
                                (unit, (*action_map_entry_pp)->conversion_cb, &bare_metal_support));
                if (!bare_metal_support)
                {
                    if (print_errors)
                    {
                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                     "BCM Action %s(%d) is not supported in BareMetal mode!%s",
                                     dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
                    }
                    else
                    {
                        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                        SHR_EXIT();
                    }
                }
            }

            dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage, (*action_map_entry_pp)->dnx_action);
        }
    }
    else if ((bcm_action >= dnx_data_field.action.vw_1st_get(unit)) &&
             (bcm_action < dnx_data_field.action.vw_1st_get(unit) + dnx_data_field.action.vw_nof_get(unit)))
    {
        /*
         * Virtual wire action.
         */
        dnx_field_action_id_t action_id;
        dnx_field_vw_action_info_t vw_action_info;
        dnx_field_per_vw_action_info_t per_vw_action_info;
        uint8 is_std_1;
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "BCM Action %s(%d) is a virtual wire action. All virtual wires are not supported on standard 1!%s",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        action_id = bcm_action - dnx_data_field.action.vw_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get(unit, action_id, &vw_action_info));
        if (vw_action_info.vw_index < 0)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Virtual wire BCM Action %s(%d) does not exist!%s!",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        SHR_IF_ERR_EXIT(dnx_field_map_per_vw_action_info_get(unit, action_id, &per_vw_action_info));
        if (per_vw_action_info.nof_actions_per_stage[dnx_stage] <= vw_action_info.action_index_within_vw)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Virtual wire BCM Action %s(%d) is not supported for stage %s!",
                             dnx_field_bcm_action_text(unit, bcm_action), bcm_action, dnx_field_stage_text(unit,
                                                                                                           dnx_stage));
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        if (per_vw_action_info.field_id[vw_action_info.action_index_within_vw] == DBAL_FIELD_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "VW action %d. VW \"%s\" has %d actions but action %d is empty.\n",
                         bcm_action, dbal_field_to_string(unit, per_vw_action_info.field_id[0]),
                         per_vw_action_info.nof_actions_per_stage[dnx_stage], vw_action_info.action_index_within_vw);
        }

        dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_VW, dnx_stage, action_id);
    }
    else
    {
        /*
         * Not in any legal range.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM Action %s(%d) is not a legal BCM id%s!",
                     dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal(unit, dnx_stage, dnx_action, &action_is_legal));
    if (action_is_legal == FALSE)
    {
        if (print_errors)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "BCM Action %s(%d) is not usable on this device.%s!",
                         dnx_field_bcm_action_text(unit, bcm_action), bcm_action, EMPTY);
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
    }

    *dnx_action_p = dnx_action;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_bcm_to_dnx(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_action_t bcm_action,
    dnx_field_action_t * dnx_action_p)
{
    const dnx_field_action_map_t *action_map_entry_p = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx_int
                    (unit, stage, TRUE, bcm_action, dnx_action_p, &action_map_entry_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_info_bcm_to_dnx(
    int unit,
    bcm_core_t core,
    dnx_field_stage_e stage,
    bcm_field_entry_action_t * bcm_action_info,
    dnx_field_entry_action_t * dnx_action_info)
{
    const dnx_field_action_map_t *action_map_entry_p = NULL;
    int i_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_action_info, _SHR_E_PARAM, "bcm_action_info");
    SHR_NULL_CHECK(dnx_action_info, _SHR_E_PARAM, "dnx_action_info");

    DNX_FIELD_STAGE_VERIFY(stage);

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx_int
                    (unit, stage, TRUE, bcm_action_info->type, &dnx_action_info->dnx_action, &action_map_entry_p));

    if ((action_map_entry_p == NULL) || (action_map_entry_p->conversion_cb == NULL))
    {
        for (i_data = 0; i_data < BCM_FIELD_ACTION_WIDTH_IN_WORDS; i_data++)
        {
            if ((i_data != 0) && (bcm_action_info->value[i_data] != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Action %s(%d) only supports one value",
                             dnx_field_bcm_action_text(unit, bcm_action_info->type), bcm_action_info->type);
            }
            if (bcm_action_info->type == bcmFieldActionTrap && stage == DNX_FIELD_STAGE_EPMF)
            {
                dnx_action_info->action_value[i_data] = DNX_RX_TRAP_ID_TYPE_GET(bcm_action_info->value[i_data]);
            }
            else
            {
                dnx_action_info->action_value[i_data] = bcm_action_info->value[i_data];
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(action_map_entry_p->conversion_cb
                                 (unit, 0, core, bcm_action_info->value, dnx_action_info->action_value),
                                 "BCM Action %s(%d) failed to convert value.%s",
                                 dnx_field_bcm_action_text(unit, bcm_action_info->type), bcm_action_info->type, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_map_action_dnx_to_bcm_sw_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    bcm_field_action_t * bcm_action_p,
    uint8 *found_p)
{
    bcm_field_action_t i_bcm_act;
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);
    dnx_field_action_class_e action_class = DNX_ACTION_CLASS(dnx_action);
    const dnx_field_action_map_t *global_action_info_p;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(found_p, _SHR_E_INTERNAL, "found_p");

    *found_p = FALSE;

    /*
     * first look for BASIC_OBJECT 
     */
    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {
        global_action_info_p = &dnx_global_action_map[i_bcm_act];
        /*
         * If Additional means not main object
         */
        if ((global_action_info_p->flags & BCM_TO_DNX_ADDITIONAL_OBJ))
        {
            continue;
        }
        /*
         * For SW action stage doesnt have meaning
         */
        if (DNX_ACTION_ID(global_action_info_p->dnx_action) == action_id &&
            DNX_ACTION_CLASS(global_action_info_p->dnx_action) == action_class)
        {
            *bcm_action_p = i_bcm_act;
            *found_p = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_map_action_dnx_to_bcm_static_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    bcm_field_action_t * bcm_action_p,
    uint8 *found_p)
{
    bcm_field_action_t i_bcm_act;
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);
    const dnx_field_action_map_t *static_action_info_p;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(found_p, _SHR_E_INTERNAL, "found_p");

    *found_p = FALSE;

    /*
     * first look for BASIC_OBJECT 
     */
    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {
        static_action_info_p = &dnx_field_map_stage_info[stage].static_action_id_map[i_bcm_act];
        /*
         * If Additional means not main object
         */
        if ((static_action_info_p->flags & BCM_TO_DNX_ADDITIONAL_OBJ))
        {
            continue;
        }

        if (static_action_info_p->dnx_action == action_id)
        {
            *bcm_action_p = i_bcm_act;
            *found_p = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_dnx_to_bcm_int(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    bcm_field_action_t * bcm_action_p,
    uint8 *found_p)
{
    dnx_field_action_class_e dnx_action_class = DNX_ACTION_CLASS(dnx_action);
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_action_p, _SHR_E_PARAM, "bcm_action_p");
    SHR_NULL_CHECK(found_p, _SHR_E_PARAM, "found_p");

    if (DNX_ACTION_STAGE(dnx_action) != stage)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX action 0x%x has encoded stage %d (\"%s\"), "
                     "but the function is called with stage %d (\"%s\").\n",
                     dnx_action, DNX_ACTION_STAGE(dnx_action), dnx_field_stage_text(unit, DNX_ACTION_STAGE(dnx_action)),
                     stage, dnx_field_stage_text(unit, stage));
    }

    switch (dnx_action_class)
    {
        case DNX_FIELD_ACTION_CLASS_USER:
        {
            dnx_field_user_action_info_t user_action_info;

            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
            if (valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The user action(%d) in stage %s does not exist!",
                             action_id, dnx_field_stage_text(unit, stage));
            }
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

            *bcm_action_p = user_action_info.bcm_id;
            *found_p = TRUE;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_SW:
        {
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_sw_get(unit, stage, dnx_action, bcm_action_p, found_p));
            break;
        }
        case DNX_FIELD_ACTION_CLASS_STATIC:
        {
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_static_get(unit, stage, dnx_action, bcm_action_p, found_p));
            break;
        }
        case DNX_FIELD_ACTION_CLASS_VW:
        {

            *bcm_action_p = dnx_data_field.action.vw_1st_get(unit) + action_id;
            *found_p = TRUE;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Action Class:%d \n", dnx_action_class);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_dnx_to_bcm(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    bcm_field_action_t * bcm_action_p)
{
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_action_p, _SHR_E_PARAM, "bcm_action_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int(unit, stage, dnx_action, bcm_action_p, &found));

    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX action 0x%x action_id %d class \"%s\" stage \"%s\" couldn't be mapped a to BCM one.\n",
                     dnx_action, DNX_ACTION_ID(dnx_action), dnx_field_action_class_text(DNX_ACTION_CLASS(dnx_action)),
                     dnx_field_stage_text(unit, stage));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_action_list(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t ** dnx_action_list_p,
    int *dnx_action_nof_p)
{
    int i_action, nof_action;
    dnx_field_action_t *dnx_action_list = NULL;
    dnx_field_action_t dnx_action;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(stage);

    SHR_NULL_CHECK(dnx_action_list_p, _SHR_E_PARAM, "dnx_action_list_p");
    SHR_NULL_CHECK(dnx_action_nof_p, _SHR_E_PARAM, "dnx_action_nof_p");

    /*
     * Pointer should be released in calling function
     */
    SHR_ALLOC_ERR_EXIT(dnx_action_list, DNX_FIELD_ACTION_NOF * sizeof(dnx_field_action_t), "act_list", "%s%s%s",
                       "Error allocating memory of dnx action list for stage:\"", dnx_field_stage_text(unit, stage),
                       "\"");

    nof_action = 0;
    for (i_action = 0; i_action < DNX_FIELD_ACTION_NOF; i_action++)
    {
        if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(stage, i_action))
        {
            dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, stage, i_action);
            dnx_action_list[nof_action++] = dnx_action;
        }
    }

    *dnx_action_list_p = dnx_action_list;
    *dnx_action_nof_p = nof_action;

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_to_dbal_action(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dbal_fields_e * dbal_field_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_field_p, _SHR_E_PARAM, "dbal_field_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    *dbal_field_p = dnx_action_info.field_id;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_to_action_type(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dnx_field_action_type_t * action_type_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_type_p, _SHR_E_PARAM, "action_type_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    if (dnx_action_info.action_type == DNX_FIELD_ACTION_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "DNX action 0x%X has no action type associated with it.\n", dnx_action);
    }

    *action_type_p = dnx_action_info.action_type;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_action_is_legal(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    int *action_is_legal_p)
{
    int action_is_void;
    dnx_field_action_class_e dnx_action_class = DNX_ACTION_CLASS(dnx_action);
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_is_legal_p, _SHR_E_PARAM, "action_is_legal_p");

    (*action_is_legal_p) = FALSE;

    switch (dnx_action_class)
    {

        case DNX_FIELD_ACTION_CLASS_USER:
        {
            dnx_field_user_action_info_t user_action_info;
            uint32 valid = FALSE;
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.valid.get(unit, action_id, &valid));
            if (valid == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));

                SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal
                                (unit, stage, user_action_info.base_dnx_action, action_is_legal_p));
            }
            break;
        }
        case DNX_FIELD_ACTION_CLASS_STATIC:
        {
            if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(stage, action_id))
            {
                (*action_is_legal_p) = TRUE;
            }
            break;
        }
        case DNX_FIELD_ACTION_CLASS_SW:
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void(unit, stage, dnx_action, &action_is_void));
            if (action_is_void)
            {
                (*action_is_legal_p) = TRUE;
            }
            break;
        }
        case DNX_FIELD_ACTION_CLASS_VW:
        {
            int action_index_within_vw;
            dnx_field_vw_action_info_t vw_action_info;
            dnx_field_per_vw_action_info_t per_vw_action_info;
            SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get(unit, action_id, &vw_action_info));
            action_index_within_vw = vw_action_info.action_index_within_vw;
            SHR_IF_ERR_EXIT(dnx_field_map_per_vw_action_info_get(unit, action_id, &per_vw_action_info));
            if (per_vw_action_info.nof_actions_per_stage[stage] <= vw_action_info.action_index_within_vw)
            {
                break;
            }
            if (per_vw_action_info.field_id[action_index_within_vw] == DBAL_FIELD_EMPTY)
            {
                break;
            }
            if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(stage,
                                                         DNX_ACTION_ID(per_vw_action_info.base_dnx_action_per_stage
                                                                       [action_index_within_vw][stage])))
            {
                (*action_is_legal_p) = TRUE;
            }

            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Action Class:%d \n", dnx_action_class);
            break;

    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_action_type_to_predef_dnx_action(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_type_t action_type,
    dnx_field_action_t * dnx_action_p)
{
    dnx_field_action_id_t action_id;
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_action_p, _SHR_E_PARAM, "dnx_action_p");

    /*
     * dnx_field_action_type_t is DBAL ENUM value of the action, need to finds its DNX_FIELD_ACTION_
     * */
    for (action_id = DNX_FIELD_ACTION_FIRST; action_id < DNX_FIELD_ACTION_NOF; action_id++)
    {
        if (action_type == DNX_FIELD_DNX_ACTION_PARAM_GET(dnx_stage, action_id, id))
        {
            break;
        }
    }

    /*
     * Check if the action exists.
     */
    if (action_id == DNX_FIELD_ACTION_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The action_type(%d) in stage %s not supporeted",
                     action_type, dnx_field_stage_text(unit, dnx_stage));
    }

    /*
     * Build a static action.
     */
    *dnx_action_p = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, dnx_stage, action_id);

    /** Sanity check, verify that there is no problem with the action.*/
    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, dnx_stage, *dnx_action_p, &dnx_action_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_size(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    unsigned int *action_size_p)
{
    uint32 size;

    dnx_field_action_class_e dnx_action_class = DNX_ACTION_CLASS(dnx_action);
    dnx_field_stage_e dnx_stage = DNX_ACTION_STAGE(dnx_action);
    dnx_field_action_id_t action_id = DNX_ACTION_ID(dnx_action);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_size_p, _SHR_E_PARAM, "action_size_p");

    DNX_FIELD_STAGE_VERIFY(stage);

    if (stage != dnx_stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inconsistency between API:\"%s\" and encoded:\"%s\" stages\n",
                     dnx_field_stage_text(unit, stage), dnx_field_stage_text(unit, dnx_stage));
    }

    switch (dnx_action_class)
    {
        case DNX_FIELD_ACTION_CLASS_USER:
        {
            dnx_field_user_action_info_t user_action_info;

            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.info.get(unit, action_id, &user_action_info));
            size = user_action_info.size;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_STATIC:
        {

            if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(stage, action_id) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The action:%s(%d) in stage %s does not exist!",
                             dnx_action_name[action_id].name, action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            size = DNX_FIELD_DNX_ACTION_PARAM_GET(stage, action_id, size);

            break;
        }
        case DNX_FIELD_ACTION_CLASS_SW:
        {
            const dnx_field_base_action_info_t *sw_action_info_p;
            sw_action_info_p = &dnx_sw_action_info[action_id];
            if (sw_action_info_p->field_id == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "SW Action:%d is not supported\n", action_id);
            }
            size = sw_action_info_p->size_sw;
            break;
        }
        case DNX_FIELD_ACTION_CLASS_VW:
        {
            unsigned int uint_size;
            dnx_field_vw_action_info_t vw_action_info;
            dnx_field_per_vw_action_info_t per_vw_action_info;
            SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get(unit, action_id, &vw_action_info));
            SHR_IF_ERR_EXIT(dnx_field_map_per_vw_action_info_get(unit, action_id, &per_vw_action_info));
            if (vw_action_info.action_index_within_vw >= per_vw_action_info.nof_actions_per_stage[stage])
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The VW action(%d) in stage %s not found",
                             action_id, dnx_field_stage_text(unit, dnx_stage));
            }
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                            (unit, stage,
                             per_vw_action_info.base_dnx_action_per_stage[vw_action_info.action_index_within_vw][stage],
                             &uint_size));
            size = uint_size;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Action Class:%d \n", dnx_action_class);
            break;
    }

    *action_size_p = size;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_prefix(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    uint32 *prefix_p,
    unsigned int *prefix_size_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(prefix_p, _SHR_E_PARAM, "prefix_p");
    SHR_NULL_CHECK(prefix_size_p, _SHR_E_PARAM, "prefix_size_p");

    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    /*
     * Only user defined actions have prefixes. Perform sanity check.
     */
    if (DNX_ACTION_CLASS(dnx_action) != DNX_FIELD_ACTION_CLASS_USER
        && (dnx_action_info.prefix != 0 || dnx_action_info.prefix_size != 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX action 0x%X is of class %s. Only user defined actions are supposed to "
                     "have a prefix, and yet it has one: prefix 0x%X prefix size %d.\n",
                     dnx_action, dnx_field_action_class_text(DNX_ACTION_CLASS(dnx_action)),
                     dnx_action_info.prefix, dnx_action_info.prefix_size);
    }

    *prefix_p = dnx_action_info.prefix;
    *prefix_size_p = dnx_action_info.prefix_size;

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_base_action(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    dnx_field_action_t * base_dnx_action_p)
{
    dnx_field_map_action_info_t dnx_action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(base_dnx_action_p, _SHR_E_PARAM, "base_dnx_action_p");

    /*
     * Check if the action is user defined.
     * Alternatively, we could check after dnx_field_map_action_info() if we receive a valid base dnx action.
     */
    if (DNX_ACTION_CLASS(dnx_action) != DNX_FIELD_ACTION_CLASS_USER)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX action 0x%X is of class %s. Only user defined actions have a base action.\n",
                     dnx_action, dnx_field_action_class_text(DNX_ACTION_CLASS(dnx_action)));
    }
    SHR_IF_ERR_EXIT(dnx_field_map_action_info(unit, stage, dnx_action, &dnx_action_info));

    (*base_dnx_action_p) = dnx_action_info.base_dnx_action;

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_bcm_action_base_action(
    int unit,
    bcm_field_action_t bcm_action,
    bcm_field_action_t * base_bcm_action_p)
{
    dnx_field_action_in_info_t action_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(base_bcm_action_p, _SHR_E_PARAM, "base_bcm_action_p");

    /*
     * Check if the action is user defined.
     */
    if (DNX_FIELD_BCM_ACTION_IS_USER(unit, bcm_action))
    {
        SHR_IF_ERR_EXIT(dnx_field_user_action_get(unit, bcm_action, &action_info));
        (*base_bcm_action_p) = action_info.bcm_action;
    }
    else
    {
        /*
         * Predefined actions don't have base action, so we return an invalid value.
         */
        (*base_bcm_action_p) = bcmFieldActionCount;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_map.h
*/
shr_error_e
dnx_field_map_dnx_action_is_void(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_t dnx_action,
    int *action_is_void_p)
{
    dnx_field_action_t base_dnx_action;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_is_void_p, _SHR_E_PARAM, "action_is_void_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    if (dnx_action == DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Action is invalid.\n");
    }

    /*
     * Void action is defined as an action that is the predefined void action or a user defined action
     * based on it.
     */
    (*action_is_void_p) = FALSE;
    if (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_SW
        && DNX_ACTION_ID(dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        /*
         * The action is the predefined void action.
         */
        (*action_is_void_p) = TRUE;
    }
    else if (DNX_ACTION_CLASS(dnx_action) == DNX_FIELD_ACTION_CLASS_USER)
    {
        /*
         * The action is a user defined action.
         * Check if the user defined action is based on the predefined void action.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action(unit, stage, dnx_action, &base_dnx_action));
        if (base_dnx_action != DNX_FIELD_ACTION_INVALID
            && DNX_ACTION_CLASS(base_dnx_action) == DNX_FIELD_ACTION_CLASS_SW
            && DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
        {
            (*action_is_void_p) = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_get_invalid_action_type(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_type_t * action_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_type_p, _SHR_E_PARAM, "action_type_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    *action_type_p = dnx_field_invalid_action[dnx_stage];

    if (*action_type_p == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid action not defined on Stage:%d\n", dnx_stage);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_get_invalidate_next_action_type(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_action_type_t * action_type_p)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_type_p, _SHR_E_PARAM, "action_type_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    *action_type_p = dnx_field_invalidate_next_action[dnx_stage];

    if (*action_type_p == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid action not defined on Stage:%d\n", dnx_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_fem_ext_output_src_type_bcm_to_dnx(
    int unit,
    bcm_field_fem_extraction_output_source_type_t bcm_fem_ext_output_src_type,
    dnx_field_fem_bit_format_e * dnx_fem_bit_format_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_fem_bit_format_p, _SHR_E_PARAM, "dnx_fem_bit_format_p");

    switch (bcm_fem_ext_output_src_type)
    {
        case bcmFieldFemExtractionOutputSourceTypeForce:
        {
            *dnx_fem_bit_format_p = DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD;
            break;
        }
        case bcmFieldFemExtractionOutputSourceTypeKey:
        {
            *dnx_fem_bit_format_p = DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid FEM Extraction Output Source Type:%d \n",
                         bcm_fem_ext_output_src_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_fem_ext_output_src_type_dnx_to_bcm(
    int unit,
    dnx_field_fem_bit_format_e dnx_fem_bit_format,
    bcm_field_fem_extraction_output_source_type_t * bcm_fem_ext_output_src_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_fem_ext_output_src_type_p, _SHR_E_PARAM, "bcm_fem_ext_output_src_type_p");

    switch (dnx_fem_bit_format)
    {
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD:
        {
            *bcm_fem_ext_output_src_type_p = bcmFieldFemExtractionOutputSourceTypeForce;
            break;
        }
        case DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT:
        {
            *bcm_fem_ext_output_src_type_p = bcmFieldFemExtractionOutputSourceTypeKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid FEM Extraction Output Source Type:%d \n", dnx_fem_bit_format);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_name_to_action(
    int unit,
    const char name[DBAL_MAX_STRING_LENGTH],
    int *nof_actions,
    bcm_field_action_t bcm_actions[DNX_FIELD_ACTION_MAX_ACTION_PER_VM])
{
    bcm_field_action_t bcm_action_ndx;
    int nof_found = 0;
    int user_defined_first = dnx_data_field.action.user_1st_get(unit);
    int user_defined_last = dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1;
    int vw_first = dnx_data_field.action.vw_1st_get(unit);
    int vw_last = dnx_data_field.action.vw_1st_get(unit) + dnx_data_field.action.vw_nof_get(unit) - 1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    SHR_NULL_CHECK(bcm_actions, _SHR_E_PARAM, "bcm_actions");

    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }
    if (0 == sal_strncmp(name, "Invalid BCM action", DBAL_MAX_STRING_LENGTH))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is \"%.*s\". \"%s\" is an illegal string.\n",
                     DBAL_MAX_STRING_LENGTH, name, "Invalid BCM action");
    }

    /*
     * Go over virtual wires actions. Can be more than one.
     * Ignore the names of the non first actions, only relate to the name of the first action (the virtual wire name).
     */
    for (bcm_action_ndx = vw_first; bcm_action_ndx <= vw_last; bcm_action_ndx++)
    {
        dnx_field_vw_action_info_t vw_action_info, vw_action_info_not_found, vw_action_info_not_found_1;
        int action_index = bcm_action_ndx - vw_first;
        SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get(unit, action_index, &vw_action_info));
        if ((vw_action_info.vw_index >= 0) && (vw_action_info.action_index_within_vw == 0))
        {
            dbal_fields_e field_id_0;
            dnx_field_per_vw_action_info_t per_vw_action_info;
            SHR_IF_ERR_EXIT(dnx_field_map_per_vw_action_info_get(unit, action_index, &per_vw_action_info));
            field_id_0 = per_vw_action_info.field_id[0];
            if (field_id_0 == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action %d has an index but no DBAL field.\n", bcm_action_ndx);
            }
            if (0 == sal_strncmp(name, dbal_field_to_string(unit, field_id_0), DBAL_MAX_STRING_LENGTH))
            {
                for (nof_found = 0; nof_found < DNX_FIELD_ACTION_MAX_ACTION_PER_VM; nof_found++)
                {
                    if (per_vw_action_info.field_id[nof_found] == DBAL_FIELD_EMPTY)
                    {
                        break;
                    }
                    /*
                     * We assume that in init the actions for the same VW are added one after another.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get
                                    (unit, action_index + nof_found, &vw_action_info_not_found));
                    if (nof_found > 0)
                    {
                        SHR_IF_ERR_EXIT(dnx_field_map_vw_action_info_get
                                        (unit, action_index + nof_found - 1, &vw_action_info_not_found_1));
                        if (vw_action_info_not_found.vw_index != vw_action_info_not_found_1.vw_index)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Name is \"%.*s\". Action %d is not consecutively mapped.\n",
                                         DBAL_MAX_STRING_LENGTH, name, nof_found);
                        }
                    }
                    if (vw_action_info_not_found.action_index_within_vw != nof_found)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Name is \"%.*s\". Action %d is not consecutively mapped.\n",
                                     DBAL_MAX_STRING_LENGTH, name, nof_found);
                    }
                    bcm_actions[nof_found] = bcm_action_ndx + nof_found;
                }
            }
        }
    }

    /*
     * Go over user defined actions.
     */
    if (nof_found == 0)
    {
        for (bcm_action_ndx = user_defined_first; bcm_action_ndx <= user_defined_last; bcm_action_ndx++)
        {
            if (0 == sal_strncmp(name, dnx_field_bcm_action_text(unit, bcm_action_ndx), DBAL_MAX_STRING_LENGTH))
            {
                nof_found = 1;
                bcm_actions[0] = bcm_action_ndx;
                break;
            }
        }
    }

    /*
     * Go over predefined actions.
     */
    if (nof_found == 0)
    {
        for (bcm_action_ndx = 0; bcm_action_ndx < bcmFieldActionCount; bcm_action_ndx++)
        {
            if (0 == sal_strncmp(name, dnx_field_bcm_action_text(unit, bcm_action_ndx), DBAL_MAX_STRING_LENGTH))
            {
                nof_found = 1;
                bcm_actions[0] = bcm_action_ndx;
                break;
            }
        }
    }

    if (nof_found <= 0)
    {
        /** We set the first element for consistency, even though nof_actions is zero.*/
        bcm_actions[0] = bcmFieldActionCount;
    }
    (*nof_actions) = nof_found;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_action_init(
    int unit)
{
    bcm_field_action_t i_bcm_act;
    const dnx_field_action_map_t *action_map_info_p;
    dnx_field_stage_e dnx_stage;
    dnx_field_action_t dnx_action;
    int i_id;
    dbal_fields_e field_id = DBAL_FIELD_EMPTY;
    char field_name[DBAL_MAX_STRING_LENGTH];
    const dnx_field_base_action_info_t *sw_action_info_p;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize SW Actions
     */
    {
        int i_info;
        for (i_info = DNX_FIELD_SW_ACTION_FIRST; i_info < DNX_FIELD_SW_ACTION_NOF; i_info++)
        {
            sw_action_info_p = &dnx_sw_action_info[i_info];
            if (sw_action_info_p->field_id == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "No DBAL field ID for sw action: %d\n", i_info);
            }
        }
    }

    /*
     * Initialize stage specific Static Actions
     */
    if (DNX_DATA_MAX_FIELD_ACTION_PREDEFINED_NOF < DNX_FIELD_ACTION_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Size of Sw state for predefined action is smaller (%d) that the amount of action (%d)\n",
                     DNX_DATA_MAX_FIELD_ACTION_PREDEFINED_NOF, DNX_FIELD_ACTION_NOF);
    }

    for (i_id = DNX_FIELD_ACTION_FIRST; i_id < DNX_FIELD_ACTION_NOF; i_id++)
    {
        if (dnx_action_name[i_id].name == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX Qualifier:%d has no valid name\n", i_id);
        }
        SHR_IF_ERR_EXIT(dnx_field_action_sw_db.predefined.field_id.get(unit, i_id, &field_id));
        if (field_id == DBAL_FIELD_EMPTY)
        {
            sal_strncpy_s(field_name, dnx_action_name[i_id].name, DBAL_MAX_STRING_LENGTH);
            SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name, &field_id));
            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.predefined.field_id.set(unit, i_id, field_id));
        }
    }

    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {
        dnx_field_action_class_e action_class;
        dnx_field_action_id_t action_id;
        dnx_field_stage_e dnx_stage;
        action_map_info_p = &dnx_global_action_map[i_bcm_act];
        if (action_map_info_p->dnx_action != DNX_FIELD_ACTION_ID_INVALID)
        {
            action_id = DNX_ACTION_ID(action_map_info_p->dnx_action);
            action_class = DNX_ACTION_CLASS(action_map_info_p->dnx_action);
            if (action_class == DNX_FIELD_ACTION_CLASS_SW)
            {
                sw_action_info_p = &dnx_sw_action_info[action_id];
                if (sw_action_info_p->size_sw == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:%s mapped to invalid static dnx action type:%s\n",
                                 dnx_field_bcm_action_text(unit, i_bcm_act),
                                 dnx_field_dnx_action_text(unit, action_id));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "BCM Action:%s mapped as global action to dnx class: %s, "
                             "yet only sw action class has non stage specific information array.\n",
                             dnx_field_bcm_action_text(unit, i_bcm_act), dnx_field_action_class_text(action_class));
            }
            if (ISEMPTY(bcm_action_description[i_bcm_act]))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:\"%s\" has no valid description\n",
                             dnx_field_bcm_action_text(unit, i_bcm_act));
            }
        }
        else
        {

            DNX_FIELD_STAGE_ACTION_ITERATOR(dnx_stage, dnx_field_map_stage_info)
            {
                dnx_action = dnx_field_map_stage_info[dnx_stage].static_action_id_map[i_bcm_act].dnx_action;
                if (DNX_FIELD_DNX_ACTION_IS_DEVICE_SUPPORTED(dnx_stage, dnx_action))
                {
                    if (ISEMPTY(bcm_action_description[i_bcm_act]))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Action:\"%s\" is mapped but has no valid description\n",
                                     dnx_field_bcm_action_text(unit, i_bcm_act));
                    }
                }
            }
        }
    }

    /*
     * Verify that all conversion CBs, have BareMetal support macro check
     */
    for (i_bcm_act = 0; i_bcm_act < bcmFieldActionCount; i_bcm_act++)
    {

        int bare_metal_support;
        DNX_FIELD_STAGE_ACTION_ITERATOR(dnx_stage, dnx_field_map_stage_info)
        {
            if (dnx_field_map_stage_info[dnx_stage].static_action_id_map[i_bcm_act].conversion_cb != NULL)
            {
                SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check
                                (unit,
                                 dnx_field_map_stage_info[dnx_stage].static_action_id_map[i_bcm_act].conversion_cb,
                                 &bare_metal_support));
            }
        }
    }

    if (dnx_data_field.action.user_1st_get(unit) != BCM_FIELD_FIRST_USER_ACTION_ID)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In DNX data the first BCM user defined action is %d, "
                     "while BCM_FIELD_FIRST_USER_ACTION_ID is %d.\n",
                     dnx_data_field.action.user_1st_get(unit), BCM_FIELD_FIRST_USER_ACTION_ID);
    }

    if (dnx_data_field.action.user_1st_get(unit) < bcmFieldActionCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First BCM User Action ID is %d, should be least bcmFieldActionCount:%d\n",
                     dnx_data_field.action.user_1st_get(unit), bcmFieldActionCount);
    }

    if ((dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit)) >
        dnx_data_field.action.vw_1st_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First VW action ID is %d, and last User defined ation ID is %d.\n",
                     dnx_data_field.action.vw_1st_get(unit),
                     dnx_data_field.action.user_1st_get(unit) + dnx_data_field.action.user_nof_get(unit) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}
