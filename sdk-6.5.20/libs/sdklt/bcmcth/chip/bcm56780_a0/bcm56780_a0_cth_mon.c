/*! \file bcm56780_a0_cth_mon.c
 *
 * Chip stub for BCMCTH MON.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_mon_telemetry_drv.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmlrd/chip/bcm56780_a0/bcm56780_a0_lrd_enum_ctype.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

#define BCM56780_A0_FT_MAX_COUNTERS_PER_POOL 8192

#define BCM56780_A0_FT_MAX_UDF_TRACKING_PARAM 8

#define BCM56780_A0_FT_MAX_NUM_HIT_BIT_TYPES 3

#define BCM56780_A0_FT_PIPES_MAX 2

#define TRACKER_PARAM_TYPE_IS_UDF(type) ((type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_SRC_IPV4) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_DST_IPV4) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_SRC_IPV6) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_DST_IPV6) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_L4_SRC_PORT) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_L4_DST_PORT) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_IP_PROTOCOL) ||\
        (type == BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_CUSTOM))
/*******************************************************************************
 * Private variables
 */
static const bcmdrd_sid_t evict_pool[] = {
    FLEX_CTR_ING_COUNTER_TABLE_0m,
    FLEX_CTR_ING_COUNTER_TABLE_1m,
    FLEX_CTR_ING_COUNTER_TABLE_2m,
    FLEX_CTR_ING_COUNTER_TABLE_3m,
    FLEX_CTR_ING_COUNTER_TABLE_4m,
    FLEX_CTR_ING_COUNTER_TABLE_5m,
    FLEX_CTR_ING_COUNTER_TABLE_6m,
    FLEX_CTR_ING_COUNTER_TABLE_7m,
    FLEX_CTR_ING_COUNTER_TABLE_8m,
    FLEX_CTR_ING_COUNTER_TABLE_9m,
    FLEX_CTR_ING_COUNTER_TABLE_10m,
    FLEX_CTR_ING_COUNTER_TABLE_11m,
    FLEX_CTR_ING_COUNTER_TABLE_12m,
    FLEX_CTR_ING_COUNTER_TABLE_13m,
    FLEX_CTR_ING_COUNTER_TABLE_14m,
    FLEX_CTR_ING_COUNTER_TABLE_15m,
    FLEX_CTR_ING_COUNTER_TABLE_16m,
    FLEX_CTR_ING_COUNTER_TABLE_17m,
    FLEX_CTR_ING_COUNTER_TABLE_18m,
    FLEX_CTR_ING_COUNTER_TABLE_19m,
    FLEX_CTR_EGR_COUNTER_TABLE_0m,
    FLEX_CTR_EGR_COUNTER_TABLE_1m,
    FLEX_CTR_EGR_COUNTER_TABLE_2m,
    FLEX_CTR_EGR_COUNTER_TABLE_3m,
    FLEX_CTR_EGR_COUNTER_TABLE_4m,
    FLEX_CTR_EGR_COUNTER_TABLE_5m,
    FLEX_CTR_EGR_COUNTER_TABLE_6m,
    FLEX_CTR_EGR_COUNTER_TABLE_7m,
    EPOST_EGR_PERQ_XMT_COUNTERS_0m
};

/* Ingress/egress mon trace HA structure. */
static bcmmon_trace_state_t *mon_trace_state[BCMDRD_CONFIG_MAX_UNITS];

/* Ingress/Egress mon redirect trace HA structure. */
static bcmmon_redirect_trace_state_t *mon_redirect_trace_state[BCMDRD_CONFIG_MAX_UNITS];

/* Ingress/egress mon drop HA structure. */
static bcmmon_drop_state_t *mon_drop_state[BCMDRD_CONFIG_MAX_UNITS];

static bcmcth_mon_ft_ctr_ing_eflex_action_profile_info_lt_t
    flex_ctr_action_profile_info_lt = {
    .ltid = CTR_ING_EFLEX_ACTION_PROFILE_INFOt,
    .key_fid =
        CTR_ING_EFLEX_ACTION_PROFILE_INFOt_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
    .num_pools_fid = CTR_ING_EFLEX_ACTION_PROFILE_INFOt_NUM_POOLSf,
    .base_index_fid = CTR_ING_EFLEX_ACTION_PROFILE_INFOt_BASE_INDEXf
};

static bcmcth_mon_ft_ctr_ing_eflex_action_profile_lt_t
flex_ctr_action_profile_lt = {
    .ltid = CTR_ING_EFLEX_ACTION_PROFILEt,
    .key_fid =
        CTR_ING_EFLEX_ACTION_PROFILEt_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
    .pool_id_fid = CTR_ING_EFLEX_ACTION_PROFILEt_POOL_IDf,
    .num_counters_fid = CTR_ING_EFLEX_ACTION_PROFILEt_NUM_COUNTERSf,
    .action_fid = CTR_ING_EFLEX_ACTION_PROFILEt_ACTIONf
};

static bcmcth_mon_ft_ing_grp_template_info_lt_t
ing_grp_template_info_lt = {
    .ltid = "FP_EM_GRP_TEMPLATE_INFO",
    .key_fid = "FP_EM_GRP_TEMPLATE_ID",
    .num_partition_id_fid = "NUM_PARTITION_ID",
    .bucket_mode_fid = 0, /* Bucket mode is zero so not fetching */
};

static bcmcth_mon_ft_ing_grp_template_partition_info_lt_t
ing_grp_template_partition_info_lt = {
    .ltid = "FP_EM_GRP_TEMPLATE_PARTITION_INFO",
    .key_fid1 = "FP_EM_GRP_TEMPLATE_PARTITION_INFO_ID",
    .key_fid2 = "PARTITION_ID",
    .key_type_fid = "EM_KEY_ATTRIBUTE_INDEX",
    .num_quals = 6,
    .qual_info[0].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_IP_HDR_SIP",
    .qual_info[0].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_SIP_OFFSET",
    .qual_info[0].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_SIP_WIDTH",
    .qual_info[0].qual = MCS_SHR_FT_EM_QUAL_SRC_IPV4,

    .qual_info[1].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_IP_HDR_DIP",
    .qual_info[1].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_DIP_OFFSET",
    .qual_info[1].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_DIP_WIDTH",
    .qual_info[1].qual = MCS_SHR_FT_EM_QUAL_DST_IPV4,

    .qual_info[2].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_L4_SRC_PORT",
    .qual_info[2].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_SRC_PORT_OFFSET",
    .qual_info[2].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_SRC_PORT_WIDTH",
    .qual_info[2].qual = MCS_SHR_FT_EM_QUAL_L4_SRC_PORT,

    .qual_info[3].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_L4_DST_PORT",
    .qual_info[3].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_DST_PORT_OFFSET",
    .qual_info[3].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_DST_PORT_WIDTH",
    .qual_info[3].qual = MCS_SHR_FT_EM_QUAL_L4_DST_PORT,

    .qual_info[4].num_qual_info_fid = "NUM_QUAL_ING_OBJ_BUS_FWD_IP_LAST_PROTOCOL",
    .qual_info[4].qual_offset_fid = "QUAL_ING_OBJ_BUS_FWD_IP_LAST_PROTOCOL_OFFSET",
    .qual_info[4].qual_width_fid = "QUAL_ING_OBJ_BUS_FWD_IP_LAST_PROTOCOL_WIDTH",
    .qual_info[4].qual = MCS_SHR_FT_EM_QUAL_IP_PROTOCOL,

    .qual_info[5].num_qual_info_fid = "NUM_QUAL_ING_OBJ_BUS_INGRESS_PP_PORT",
    .qual_info[5].qual_offset_fid = "QUAL_ING_OBJ_BUS_INGRESS_PP_PORT_OFFSET",
    .qual_info[5].qual_width_fid = "QUAL_ING_OBJ_BUS_INGRESS_PP_PORT_WIDTH",
    .qual_info[5].qual = MCS_SHR_FT_EM_QUAL_IN_PORT
 };

static bcmcth_mon_dt_em_ft_grp_template_lt_t
dt_em_ft_grp_template_lt = {
    .lt_name = "DT_EM_FP_GRP_TEMPLATE",
    .key_name = "DT_EM_FP_GRP_TEMPLATE_INDEX",
    .dt_em_ft_pdd_template_id_name = "DT_EM_FP_PDD_TEMPLATE_INDEX"
};

static bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t
ing_pdd_template_partition_info_lt = {
    .ltid = "FP_EM_PDD_TEMPLATE_PARTITION_INFO",
    .key_fid1 = "FP_EM_PDD_TEMPLATE_PARTITION_INFO_ID",
    .key_fid2 = "PARTITION_ID",
    .num_actions = 2,
    .action_info[0].action = SHR_FT_EM_ACTION_CTR,
    .action_info[0].num_action_info_fid = "NUM_ACTION_FLEX_CTR_VALID",
    .action_info[0].action_offset_fid = "ACTION_FLEX_CTR_VALID_OFFSET",
    .action_info[0].action_width_fid = "ACTION_FLEX_CTR_VALID_WIDTH",

    /* Note: Both QoS Profile and IFA trigger uses same opaque object */
    .action_info[1].action = SHR_FT_EM_ACTION_OPAQUE_OBJ0,
    .action_info[1].num_action_info_fid = "NUM_ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0",
    .action_info[1].action_offset_fid = "ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0_OFFSET",
    .action_info[1].action_width_fid = "ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0_WIDTH",
};

/* FT INFO LTs */
static bcmcth_mon_ft_ing_grp_template_info_lt_t
ing_grp_template_info_ft_lt = {
    .ltid = "FP_FT_GRP_TEMPLATE_INFO",
    .key_fid = "FP_FT_GRP_TEMPLATE_ID",
    .num_partition_id_fid = "NUM_PARTITION_ID",
    .bucket_mode_fid = 0, /* Bucket mode is zero so not fetching */
};

static bcmcth_mon_ft_ing_grp_template_partition_info_lt_t
ing_grp_template_partition_info_ft_lt = {
    .ltid = "FP_FT_GRP_TEMPLATE_PARTITION_INFO",
    .key_fid1 = "FP_FT_GRP_TEMPLATE_PARTITION_INFO_ID",
    .key_fid2 = "PARTITION_ID",
    .key_type_fid = "EM_KEY_ATTRIBUTE_INDEX",
    .num_quals = 6,
    .qual_info[0].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_IP_HDR_SIP",
    .qual_info[0].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_SIP_OFFSET",
    .qual_info[0].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_SIP_WIDTH",
    .qual_info[0].qual = MCS_SHR_FT_EM_QUAL_SRC_IPV4,

    .qual_info[1].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_IP_HDR_DIP",
    .qual_info[1].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_DIP_OFFSET",
    .qual_info[1].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_IP_HDR_DIP_WIDTH",
    .qual_info[1].qual = MCS_SHR_FT_EM_QUAL_DST_IPV4,

    .qual_info[2].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_L4_SRC_PORT",
    .qual_info[2].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_SRC_PORT_OFFSET",
    .qual_info[2].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_SRC_PORT_WIDTH",
    .qual_info[2].qual = MCS_SHR_FT_EM_QUAL_L4_SRC_PORT,

    .qual_info[3].num_qual_info_fid = "NUM_QUAL_PKT_FWD_FIELD_BUS_L4_DST_PORT",
    .qual_info[3].qual_offset_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_DST_PORT_OFFSET",
    .qual_info[3].qual_width_fid = "QUAL_PKT_FWD_FIELD_BUS_L4_DST_PORT_WIDTH",
    .qual_info[3].qual = MCS_SHR_FT_EM_QUAL_L4_DST_PORT,

    .qual_info[4].num_qual_info_fid = "NUM_QUAL_ING_OBJ_BUS_FWD_IP_LAST_PROTOCOL",
    .qual_info[4].qual_offset_fid = "QUAL_ING_OBJ_BUS_FWD_IP_LAST_PROTOCOL_OFFSET",
    .qual_info[4].qual_width_fid = "QUAL_ING_OBJ_BUS_FWD_IP_LAST_PROTOCOL_WIDTH",
    .qual_info[4].qual = MCS_SHR_FT_EM_QUAL_IP_PROTOCOL,

    .qual_info[5].num_qual_info_fid = "NUM_QUAL_ING_OBJ_BUS_INGRESS_PP_PORT",
    .qual_info[5].qual_offset_fid = "QUAL_ING_OBJ_BUS_INGRESS_PP_PORT_OFFSET",
    .qual_info[5].qual_width_fid = "QUAL_ING_OBJ_BUS_INGRESS_PP_PORT_WIDTH",
    .qual_info[5].qual = MCS_SHR_FT_EM_QUAL_IN_PORT
 };

static bcmcth_mon_dt_em_ft_grp_template_lt_t
dt_em_ft_grp_template_ft_lt = {
    .lt_name = "DT_EM_FT_GRP_TEMPLATE",
    .key_name = "DT_EM_FT_GRP_TEMPLATE_INDEX",
    .dt_em_ft_pdd_template_id_name = "DT_EM_FT_PDD_TEMPLATE_INDEX"
};

static bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t
ing_pdd_template_partition_info_ft_lt = {
    .ltid = "FP_FT_PDD_TEMPLATE_PARTITION_INFO",
    .key_fid1 = "FP_FT_PDD_TEMPLATE_PARTITION_INFO_ID",
    .key_fid2 = "PARTITION_ID",
    .num_actions = 2,
    .action_info[0].action = SHR_FT_EM_ACTION_CTR,
    .action_info[0].num_action_info_fid = "NUM_ACTION_FLEX_CTR_VALID",
    .action_info[0].action_offset_fid = "ACTION_FLEX_CTR_VALID_OFFSET",
    .action_info[0].action_width_fid = "ACTION_FLEX_CTR_VALID_WIDTH",

    /* Note: Both QoS Profile and IFA trigger uses same opaque object */
    .action_info[1].action = SHR_FT_EM_ACTION_OPAQUE_OBJ0,
    .action_info[1].num_action_info_fid = "NUM_ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0",
    .action_info[1].action_offset_fid = "ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0_OFFSET",
    .action_info[1].action_width_fid = "ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0_WIDTH",
};

static bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t
ing_pdd_template_partition_info_ft_noapp_lt = {
    .ltid = "FP_FT_PDD_TEMPLATE_PARTITION_INFO",
    .key_fid1 = "FP_FT_PDD_TEMPLATE_PARTITION_INFO_ID",
    .key_fid2 = "PARTITION_ID",
    .num_actions = 10,

    .action_info[0].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_FLEX_CTR_ACTION,
    .action_info[0].num_action_info_fid = "NUM_ACTION_FLEX_CTR_VALID",
    .action_info[0].action_offset_fid = "ACTION_FLEX_CTR_VALID_OFFSET",
    .action_info[0].action_width_fid = "ACTION_FLEX_CTR_VALID_WIDTH",

    .action_info[1].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_OPAQUE_OBJ0,
    .action_info[1].num_action_info_fid = "NUM_ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0",
    .action_info[1].action_offset_fid = "ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0_OFFSET",
    .action_info[1].action_width_fid = "ACTION_ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0_WIDTH",

    .action_info[2].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_IOAM_GBP_ACTION,
    .action_info[2].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_EM_FT_IOAM_GBP_ACTION",
    .action_info[2].action_offset_fid = "ACTION_ING_CMD_BUS_EM_FT_IOAM_GBP_ACTION_OFFSET",
    .action_info[2].action_width_fid = "ACTION_ING_CMD_BUS_EM_FT_IOAM_GBP_ACTION_WIDTH",

    .action_info[3].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_COPY_TO_CPU,
    .action_info[3].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_EM_FT_COPY_TO_CPU",
    .action_info[3].action_offset_fid = "ACTION_ING_CMD_BUS_EM_FT_COPY_TO_CPU_OFFSET",
    .action_info[3].action_width_fid = "ACTION_ING_CMD_BUS_EM_FT_COPY_TO_CPU_WIDTH",

    .action_info[4].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_DROP_ACTION,
    .action_info[4].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_EM_FT_DROP_ACTION",
    .action_info[4].action_offset_fid = "ACTION_ING_CMD_BUS_EM_FT_DROP_ACTION_OFFSET",
    .action_info[4].action_width_fid = "ACTION_ING_CMD_BUS_EM_FT_DROP_ACTION_WIDTH",

    .action_info[5].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_DESTINATION,
    .action_info[5].num_action_info_fid = "NUM_ACTION_ING_OBJ_BUS_DESTINATION",
    .action_info[5].action_offset_fid = "ACTION_ING_OBJ_BUS_DESTINATION_OFFSET",
    .action_info[5].action_width_fid = "ACTION_ING_OBJ_BUS_DESTINATION_WIDTH",

    .action_info[6].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_DESTINATION_TYPE,
    .action_info[6].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_DESTINATION_TYPE",
    .action_info[6].action_offset_fid = "ACTION_ING_CMD_BUS_DESTINATION_TYPE_OFFSET",
    .action_info[6].action_width_fid = "ACTION_ING_CMD_BUS_DESTINATION_TYPE_WIDTH",

    .action_info[7].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_FLEX_STATE_ACTION,
    .action_info[7].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_EM_FT_FLEX_STATE_ACTION",
    .action_info[7].action_offset_fid = "ACTION_ING_CMD_BUS_EM_FT_FLEX_STATE_ACTION_OFFSET",
    .action_info[7].action_width_fid = "ACTION_ING_CMD_BUS_EM_FT_FLEX_STATE_ACTION_WIDTH",

    .action_info[8].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_PKT_FLOW_ELIGIBILITY,
    .action_info[8].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_PKT_FLOW_ELIGIBILITY",
    .action_info[8].action_offset_fid = "ACTION_ING_CMD_BUS_PKT_FLOW_ELIGIBILITY_OFFSET",
    .action_info[8].action_width_fid = "ACTION_ING_CMD_BUS_PKT_FLOW_ELIGIBILITY_WIDTH",

    .action_info[9].action = BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_L2_IIF_SVP_MIRROR_INDEX_0,
    .action_info[9].num_action_info_fid = "NUM_ACTION_ING_CMD_BUS_L2_IIF_SVP_MIRROR_INDEX_0",
    .action_info[9].action_offset_fid = "ACTION_ING_CMD_BUS_L2_IIF_SVP_MIRROR_INDEX_0_OFFSET",
    .action_info[9].action_width_fid = "ACTION_ING_CMD_BUS_L2_IIF_SVP_MIRROR_INDEX_0_WIDTH",
};

static bcmcth_mon_ft_udf_policy_lt_t
udf_policy_lt = {
    .ltid = UDF_POLICYt,
    .udf_policy_fid = UDF_POLICYt_UDF_POLICY_IDf,
    .packet_header_fid = UDF_POLICYt_PACKET_HEADERf,
    .offset_fid = UDF_POLICYt_PACKET_HEADERf,
    .cont_4_byte_fid = UDF_POLICYt_CONTAINER_4_BYTEf,
    .cont_2_byte_fid = UDF_POLICYt_CONTAINER_2_BYTEf,
    .cont_1_byte_fid = UDF_POLICYt_CONTAINER_1_BYTEf
};

/*
 * TD4 chunk index to FT qualifier mapping.
 * The assumption below is chunk 0 to 9 are 2 byte chunks/containers.
 * and 10 to 12 are 1 byte chunks/containers.
 */
#define BCM56780_A0_FT_NUM_2BYTE_UDF_CONTAINERS 10
static int bcm56780_a0_ft_udf_chunk_to_2byte_cont_mapping[] = {
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK0,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK1,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK2,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK3,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK4,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK5,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK6,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK7,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK8,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK9
};

#define BCM56780_A0_FT_NUM_1BYTE_UDF_CONTAINERS 3
static int bcm56780_a0_ft_udf_chunk_to_1byte_cont_mapping[] = {
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK10,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK11,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK12
};

static bcmcth_mon_flowtracker_drv_t bcm56780_a0_flowtracker_drv;
/*******************************************************************************
 * Local functions
 */

static bcmcth_mon_flowtracker_drv_t *
bcm56780_a0_cth_mon_ft_drv_get(int unit)
{
    return &bcm56780_a0_flowtracker_drv;
}


static int
bcm56780_a0_cth_mon_udf_policy_lt_lookup (
        int unit,
        bcmcth_mon_ft_group_t *entry,
        ft_udf_policy_info_t *out)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmltd_field_t *gen_field;
    uint32_t idx = 0;
    size_t fld_count = 0;
    uint32_t sid = 0, i, fid, j=0, k=0;
    bcmcth_mon_ft_udf_policy_lt_t *lt_info = NULL;
    ft_udf_policy_info_t info[BCM56780_A0_FT_MAX_UDF_TRACKING_PARAM];

    SHR_FUNC_ENTER(unit);
    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);

    lt_info = ft_drv->udf_policy_lt;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_info->ltid,
                                &fld_count));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, 1, &in_flds));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, fld_count, &out_flds));

    sid = lt_info->ltid;
    in_flds.count = 1;
    in_flds.field[0]->id = lt_info->udf_policy_fid;

    for (i = 0; i < FLOWTRACKER_TRACKING_PARAMETERS_MAX; i++) {
        if (TRACKER_PARAM_TYPE_IS_UDF(entry->tp[i].type)) {
            in_flds.field[0]->data = entry->tp[i].udf_policy_id;
            info[k].type = entry->tp[i].type;

            if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds)
                                                    != SHR_E_NONE) {
            entry->oper =
                BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_UDF_POLICY_ID_NOT_EXISTS;
                SHR_EXIT();
            }
            for (j = 0; j < out_flds.count; j++) {
                fid = out_flds.field[j]->id;
                gen_field = out_flds.field[j];
                idx = gen_field->idx;

                if (fid == lt_info->packet_header_fid) {
                    info[k].packet_header = gen_field->data;
                } else if (fid == lt_info->offset_fid) {
                    info[k].offset = gen_field->data;
                } else if (fid == lt_info->cont_4_byte_fid) {
                    info[k].cont_4_byte[idx] = gen_field->data;
                } else if (fid == lt_info->cont_2_byte_fid) {
                    info[k].cont_2_byte[idx] = gen_field->data;
                } else if (fid == lt_info->cont_1_byte_fid) {
                    info[k].cont_1_byte[idx] = gen_field->data;
                }
            }
            k++;
        }
    }
    sal_memcpy(out, info, sizeof(info));

exit:
    bcmcth_mon_ft_util_fields_free(unit, 1, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ing_grp_template_info_field_name_to_idx(
        int unit,
        bcmcth_mon_ft_ing_grp_template_info_lt_t *in,
        bcmcth_mon_ft_ing_grp_template_info_lt_id_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->key_fid, &out->ltid, &out->key_fid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->num_partition_id_fid, &out->ltid, &out->num_partition_id_fid));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ing_grp_template_info_lt_lookup (
        int unit,
        bcmcth_mon_ft_group_t *entry,
        ing_grp_template_info_t * info)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmltd_field_t *gen_field;
    size_t fld_count = 0;
    uint32_t i;
    bcmltd_fid_t fid = 0;
    bcmcth_mon_ft_ing_grp_template_info_lt_t *lt_info = NULL;
    bcmcth_mon_ft_ing_grp_template_info_lt_id_t *lt_id_info = NULL;
    uint64_t data[4];
    bcmevm_extend_ev_data_t ev_data = {0};

    SHR_FUNC_ENTER(unit);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);

    lt_info = ft_drv->ing_grp_template_info_lt;

   /* Get NPL generated enums for LT and fields */
    SHR_ALLOC(lt_id_info,
        sizeof(bcmcth_mon_ft_ing_grp_template_info_lt_id_t),
        "bcmcthMonFtIngGrpTemplateLtId");
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_cth_mon_ing_grp_template_info_field_name_to_idx(unit,
            lt_info, lt_id_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_id_info->ltid,
                                &fld_count));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, 1, &in_flds));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, fld_count, &out_flds));

    in_flds.count = 1;
    in_flds.field[0]->id = lt_id_info->key_fid;
    in_flds.field[0]->data = entry->dt_em_grp_template_id;

    ev_data.count = 4;
    data[0] = lt_id_info->ltid;
    data[1] = BCMPTM_DIRECT_TRANS_ID;
    data[2] = (unsigned long)&in_flds;
    data[3] = (unsigned long)&out_flds;
    ev_data.data = (uint64_t *)data;

    bcmevm_publish_event_notify(unit, "FP_INFO_LOOKUP", (uintptr_t)&ev_data);

    if (ev_data.count == 0) {
        entry->oper =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_EM_GRP_TEMPLATE_ID_NOT_EXISTS;
        SHR_EXIT();
    }

    for (i = 0; i < out_flds.count; i++) {
        fid = out_flds.field[i]->id;
        gen_field = out_flds.field[i];

        if (fid == lt_id_info->num_partition_id_fid) {
            info->num_partition = gen_field->data;
        }
    }
exit:
    SHR_FREE(lt_id_info);
    bcmcth_mon_ft_util_fields_free(unit, 1, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ing_grp_templ_part_info_field_name_to_idx(
        int unit,
        bcmcth_mon_ft_ing_grp_template_partition_info_lt_t *in,
        bcmcth_mon_ft_ing_grp_template_partition_info_lt_id_t *out)
{
    int i = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->key_fid1, &out->ltid, &out->key_fid1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->key_fid2, &out->ltid, &out->key_fid2));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->key_type_fid, &out->ltid, &out->key_type_fid));

    for (i = 0; i < in->num_quals; i++) {
        out->qual_info[i].qual = in->qual_info[i].qual;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_name_to_idx(unit,
            in->ltid, in->qual_info[i].num_qual_info_fid,
            &out->ltid, &out->qual_info[i].num_qual_info_fid));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_name_to_idx(unit,
            in->ltid, in->qual_info[i].qual_offset_fid,
            &out->ltid, &out->qual_info[i].qual_offset_fid));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_name_to_idx(unit,
            in->ltid, in->qual_info[i].qual_width_fid,
            &out->ltid, &out->qual_info[i].qual_width_fid));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ing_grp_template_partition_info_lt_lookup (
    int unit,
    bcmcth_mon_ft_group_t *entry,
    ing_grp_template_info_t *in,
    ing_grp_template_partition_info_t *out,
    uint8_t *num_quals)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmltd_field_t *gen_field;
    size_t fld_count = 0;
    uint32_t i, idx, j;
    bcmltd_fid_t fid = 0;
    ing_grp_template_partition_info_t *partition_info = NULL;
    bcmcth_mon_ft_ing_grp_template_partition_info_lt_t *lt_info = NULL;
    bcmcth_mon_ft_ing_grp_template_partition_info_lt_id_t *lt_id_info = NULL;
    uint64_t data[4];
    bcmevm_extend_ev_data_t ev_data = {0};
    uint32_t actual_fld_count = 0;
    bcmlrd_field_def_t field_def;

    SHR_FUNC_ENTER(unit);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);
    lt_info = ft_drv->ing_grp_template_partition_info_lt;

    /* Get NPL generated enums for LT and fields */
    SHR_ALLOC(lt_id_info,
        sizeof(bcmcth_mon_ft_ing_grp_template_partition_info_lt_id_t),
        "bcmcthMonFtIngGrpTempPartLtIdInfo");
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_cth_mon_ing_grp_templ_part_info_field_name_to_idx(unit,
            lt_info, lt_id_info));

    SHR_ALLOC(partition_info,
          sizeof(ing_grp_template_partition_info_t),
          "bcmcthMonFtIngGrpTempPartInfo");

    sal_memset(partition_info, 0, sizeof(ing_grp_template_partition_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                lt_id_info->ltid,
                                &fld_count));

    for (i = 0; i < fld_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, lt_id_info->ltid, i, &field_def));
        if (field_def.depth != 0) {
            actual_fld_count += field_def.depth;
        } else {
            actual_fld_count++;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, 2, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, actual_fld_count,
                                         &out_flds));
    in_flds.count = 2;
    in_flds.field[0]->id = lt_id_info->key_fid1;
    in_flds.field[0]->data = entry->dt_em_grp_template_id;
    in_flds.field[1]->id = lt_id_info->key_fid2;
    in_flds.field[1]->data = 1;

    ev_data.count = 4;
    data[0] = lt_id_info->ltid;
    data[1] = BCMPTM_DIRECT_TRANS_ID;
    data[2] = (unsigned long)&in_flds;
    data[3] = (unsigned long)&out_flds;
    ev_data.data = (uint64_t *)data;

    bcmevm_publish_event_notify(unit, "FP_INFO_LOOKUP", (uintptr_t)&ev_data);

    if (ev_data.count == 0) {
        entry->oper =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_EM_GRP_TEMPLATE_ID_NOT_EXISTS;
        SHR_EXIT();
    }

    for (i = 0; i < out_flds.count; i++) {
        fid = out_flds.field[i]->id;
        gen_field = out_flds.field[i];
        idx = gen_field->idx;
        if (fid == lt_id_info->key_type_fid) {
            partition_info->key_type = gen_field->data;
            continue;
        }
        for (j = 0; j < lt_info->num_quals; j++) {
            if (fid == lt_id_info->qual_info[j].num_qual_info_fid) {
                partition_info->qual_info[j].num_qual_info =
                    gen_field->data;
                partition_info->qual_info[j].qual =
                    lt_id_info->qual_info[j].qual;
                continue;
            } else if (fid == lt_id_info->qual_info[j].qual_offset_fid) {
                partition_info->qual_info[j].qual_offset[idx] =
                    gen_field->data;
                continue;
            } else if (fid == lt_id_info->qual_info[j].qual_width_fid) {
                partition_info->qual_info[j].qual_width[idx] =
                    gen_field->data;
                continue;
            }
        }
    }

    /* Fill the return structure with consecutive index */
    *num_quals = 0;
    out->key_type = partition_info->key_type;
    for (i=0; i < lt_info->num_quals; i++) {
        if (partition_info->qual_info[i].num_qual_info) {
            sal_memcpy(&out->qual_info[(*num_quals)++],
            &partition_info->qual_info[i], sizeof(fp_qualifier_info_t));
        }
    }

exit:
    SHR_FREE(partition_info);
    SHR_FREE(lt_id_info);
    bcmcth_mon_ft_util_fields_free(unit, 2, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, actual_fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_dt_em_ft_grp_template_lt_lookup (
    int unit,
    bcmcth_mon_ft_group_t *entry,
    uint32_t *dt_em_ft_pdd_template_id)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmltd_field_t *gen_field;
    bcmcth_mon_dt_em_ft_grp_template_lt_t *lt_info;
    size_t fld_count = 0;
    uint32_t i;
    uint32_t actual_fld_count = 0;
    bcmltd_sid_t lt_id;
    bcmltd_fid_t key_id;
    bcmltd_fid_t field_id,fid;
    bcmlrd_field_def_t field_def;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);
    lt_info = ft_drv->dt_em_ft_grp_template_lt;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        lt_info->lt_name, lt_info->key_name, &lt_id, &key_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        lt_info->lt_name, lt_info->dt_em_ft_pdd_template_id_name,
        &lt_id, &field_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_id, &fld_count));

    for (i = 0; i < fld_count; i++) {
        rv = bcmlrd_table_field_def_get(unit, lt_id, i, &field_def);
        if (rv == SHR_E_UNAVAIL) {
            /*
             * For certain fields like PIPEf which only exists if certain
             * global configuration is true in FP LTs.So, if the above API
             * returns E_UNAVAIL for such fields, ignore and continue fetching
             * for other field IDs.
             */
            continue;
        } else if (rv != SHR_E_NONE) {
            SHR_ERR_EXIT(rv);
        }
        if (field_def.depth != 0) {
            actual_fld_count += field_def.depth;
        } else {
            actual_fld_count++;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, 1, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, actual_fld_count, &out_flds));

    in_flds.count = 1;
    in_flds.field[0]->id = key_id;
    in_flds.field[0]->data = entry->dt_em_grp_template_id;

    if (bcmimm_entry_lookup(unit, lt_id, &in_flds, &out_flds) != SHR_E_NONE) {
        entry->oper =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_EM_GRP_TEMPLATE_ID_NOT_EXISTS;
        SHR_EXIT();
    }

    for (i = 0; i < out_flds.count; i++) {
        fid = out_flds.field[i]->id;
        gen_field = out_flds.field[i];
        if (fid == field_id) {
            *dt_em_ft_pdd_template_id = gen_field->data;
        }
    }
exit:
    bcmcth_mon_ft_util_fields_free(unit, 1, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, actual_fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ing_pdd_templ_part_info_field_name_to_idx(
        int unit,
        bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t *in,
        bcmcth_mon_ft_ing_pdd_template_partition_info_lt_id_t *out)
{
    int i = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->key_fid1, &out->ltid, &out->key_fid1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_field_name_to_idx(unit,
        in->ltid, in->key_fid2, &out->ltid, &out->key_fid2));

    for (i= 0; i < in->num_actions; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_name_to_idx(unit,
            in->ltid, in->action_info[i].num_action_info_fid,
            &out->ltid, &out->action_info[i].num_action_info_fid));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_name_to_idx(unit,
            in->ltid, in->action_info[i].action_offset_fid,
            &out->ltid, &out->action_info[i].action_offset_fid));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_name_to_idx(unit,
            in->ltid, in->action_info[i].action_width_fid,
            &out->ltid, &out->action_info[i].action_width_fid));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ing_pdd_template_partition_info_lt_lookup (
    int unit,
    bcmltd_common_mon_flowtracker_group_state_t_t *oper,
    uint32_t dt_em_ft_pdd_template_id,
    ing_pdd_template_partition_info_t *out,
    uint8_t *num_actions)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    const bcmltd_field_t *gen_field;
    size_t fld_count = 0;
    uint32_t i, fid, idx, j;
    ing_pdd_template_partition_info_t *pdd_partition_info = NULL;
    ing_pdd_template_partition_info_t temp_pdd = {0};
    bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t *lt_info = NULL;
    bcmcth_mon_ft_ing_pdd_template_partition_info_lt_id_t *lt_id_info = NULL;
    uint64_t data[4];
    bcmevm_extend_ev_data_t ev_data = {0};
    bcmlrd_field_def_t field_def;
    uint32_t actual_fld_count = 0;

    SHR_FUNC_ENTER(unit);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);
    lt_info = ft_drv->ing_pdd_template_partition_info_lt;

    /* Get NPL generated enums for LT and fields */
    SHR_ALLOC(lt_id_info,
        sizeof(bcmcth_mon_ft_ing_pdd_template_partition_info_lt_id_t),
        "bcmcthMonFtIngPddTempPartInfoLtId");
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_cth_mon_ing_pdd_templ_part_info_field_name_to_idx(unit,
            lt_info, lt_id_info));

    SHR_ALLOC(pdd_partition_info,
            sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_ACTIONS_MAX,
            "bcmcthMonFtEmPddPartInfo");
    sal_memset(pdd_partition_info, 0,
        sizeof(ing_pdd_template_partition_info_t) *FLOWTRACKER_ACTIONS_MAX);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_id_info->ltid, &fld_count));

    for (i = 0; i < fld_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, lt_id_info->ltid, i, &field_def));
        if (field_def.depth != 0) {
            actual_fld_count += field_def.depth;
        } else {
            actual_fld_count++;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, 2, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, actual_fld_count, &out_flds));

    in_flds.count = 2;
    in_flds.field[0]->id = lt_id_info->key_fid1;
    in_flds.field[0]->data = dt_em_ft_pdd_template_id;
    in_flds.field[1]->id = lt_id_info->key_fid2;
    in_flds.field[1]->data = 1;

    ev_data.count = 4;
    data[0] = lt_id_info->ltid;
    data[1] = BCMPTM_DIRECT_TRANS_ID;
    data[2] = (unsigned long)&in_flds;
    data[3] = (unsigned long)&out_flds;
    ev_data.data = (uint64_t *)data;

    bcmevm_publish_event_notify(unit, "FP_INFO_LOOKUP", (uintptr_t)&ev_data);

    if (ev_data.count == 0) {
        *oper =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_EM_GRP_TEMPLATE_ID_NOT_EXISTS;
        SHR_EXIT();
    }

    for (i = 0; i < out_flds.count; i++) {
        fid = out_flds.field[i]->id;
        gen_field = out_flds.field[i];
        idx = gen_field->idx;

        for (j = 0; j < lt_info->num_actions; j++) {
            if (fid == lt_id_info->action_info[j].num_action_info_fid) {
                pdd_partition_info[j].num_action_info = gen_field->data;
                pdd_partition_info[j].action = lt_info->action_info[j].action;
                continue;
            } else if (fid == lt_id_info->action_info[j].action_offset_fid) {
                pdd_partition_info[j].action_offset[idx] = gen_field->data;
                continue;
            } else if (fid == lt_id_info->action_info[j].action_width_fid) {
                if (j == 0) {
                    /* TILE_AUX_MENU is 8 bits */
                    pdd_partition_info[j].action_width[idx] = 8;
                } else {
                    pdd_partition_info[j].action_width[idx] = gen_field->data;
                }
                continue;
            }
        }
    }

    /* Fill the return structure with consecutive index */
    *num_actions = 0;
    for (i=0; i < lt_info->num_actions; i++) {
        if (pdd_partition_info[i].num_action_info) {
            sal_memcpy(&out[(*num_actions)++],
                &pdd_partition_info[i], sizeof(ing_pdd_template_partition_info_t));
        }
    }

    /* Sort them based on offset */
    for (i=0; i < *num_actions; i++) {
        for (j = i + 1; j < *num_actions; j++)  {
            if (out[i].action_offset[0] > out[j].action_offset[0]) {
                sal_memcpy(&temp_pdd, &out[i], sizeof(ing_pdd_template_partition_info_t));
                sal_memcpy(&out[i],&out[j], sizeof(ing_pdd_template_partition_info_t));
                sal_memcpy(&out[j], &temp_pdd, sizeof(ing_pdd_template_partition_info_t));
            }
        }
    }
exit:
    SHR_FREE(pdd_partition_info);
    SHR_FREE(lt_id_info);
    bcmcth_mon_ft_util_fields_free(unit, 2, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, actual_fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_flex_em_grp_key_part_info_extract(
        int unit,
        ing_grp_template_partition_info_t *grp_template_partition_info,
        uint8_t num_quals,
        mcs_ft_msg_ctrl_flex_em_group_create_t *msg)
{
    fp_qualifier_info_t *qual_info = NULL;
    int total_key_width = 0, rem_key_width = 0;
    int num_key_parts = 0;
    uint16_t key_start_offset[MCS_SHR_FT_EM_MAX_KEY_PARTS];
    uint16_t key_end_offset[MCS_SHR_FT_EM_MAX_KEY_PARTS];
    int i, j, temp = 0;
    uint16_t biggest_offset = 0, biggest_width = 0;
    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_quals; i++) {
        /* Get the qualifier info for that Tracking param */
        qual_info = &(grp_template_partition_info->qual_info[i]);
        for (j = 0; j < qual_info->num_qual_info; j++) {
            if (qual_info->qual_offset[j] > biggest_offset) {
                biggest_offset = qual_info->qual_offset[j];
                biggest_width  = qual_info->qual_width[j];
            }
        }
    }
    total_key_width = biggest_offset + biggest_width;

    /*
     * 113 = Base entry size (120 bits)
     *       - key_type size (4 bits) - base_valid size (3 bits)
     *       for 1st base entry
     */
    rem_key_width = total_key_width;
    num_key_parts = 1;
    rem_key_width -= 113;
    key_start_offset[num_key_parts -1] = 7;
    if (rem_key_width <= 0) {
        /*
         * If there is nothing remaining in key, key end offset is calculated
         * from key start offset and total key width.
         */
        key_end_offset[num_key_parts - 1] =
            key_start_offset[num_key_parts -1] + total_key_width -1;
    } else {
        /* 7 + 113 (1st base entry key part size) - 1 */
        key_end_offset[num_key_parts - 1] = 119;
    }
    while (rem_key_width > 0) {
        temp = rem_key_width;
        /*
         * 117 = Base entry size (120 bits)
         *       - base_valid size (3 bits)
         *       for 2nd and above base entries.
         */
        rem_key_width -= 117;
        num_key_parts++;
        /*
         * Key start offset will be
         * (number of base entries passed * size of one base entry) +
         * 3 bits for base valid
         */
        key_start_offset[num_key_parts -1] = ((num_key_parts - 1) * 120) + 3;
        if (rem_key_width <= 0) {
            key_end_offset[num_key_parts - 1] =
                key_start_offset[num_key_parts -1] + temp - 1;
        } else {
            key_end_offset[num_key_parts - 1] =
                key_start_offset[num_key_parts -1] + 117 - 1;
        }
    }

    if (num_key_parts > 4) {
        /* Not possible. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    msg->num_key_parts = num_key_parts;
    msg->key_size = total_key_width;
    for (i = 0; i < num_key_parts; i++) {
        msg->key_start_offset[i] = key_start_offset[i];
        msg->key_end_offset[i]   = key_end_offset[i];
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_flex_em_grp_action_info_extract(
        int unit,
        bcmcth_mon_ft_group_t *entry,
        ing_pdd_template_partition_info_t *pdd_template_partition_info,
        uint8_t num_actions,
        mcs_ft_msg_ctrl_flex_em_group_create_t *msg)
{
    int total_width = 0;
    int data_width = 0;
    int i, j;
    SHR_FUNC_ENTER(unit);

    msg->num_actions = num_actions;

    /* Assumption is number of key parts are already filled */
    total_width = msg->num_key_parts * 120;

    /* Data is filled from MSB */
    msg->action_end_offset = total_width - 1;

    for (i = 0; i < num_actions; i++) {
        msg->aset[i] = pdd_template_partition_info[i].action;
        msg->aset_widths[i] = 0;

        for (j = 0; j < pdd_template_partition_info[i].num_action_info; j++) {
            msg->aset_widths[i] +=
                pdd_template_partition_info[i].action_width[j];
        }

        data_width +=msg->aset_widths[i];
    }

    msg->action_start_offset = msg->action_end_offset - data_width + 1;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_flex_em_grp_udf_info_extract(
        int unit,
        uint16_t udf_policy_id,
        bcmcth_mon_ft_group_t *entry,
        mcs_ft_msg_ctrl_tp_info_t *tp_info,
        mcs_ft_msg_ctrl_qual_info_t *qual_info,
        ft_udf_policy_info_t *udf_policy_info,
        uint8_t *num_udf_quals)
{
    int num_chunks = 0, chunk;
    uint8_t base_offset = 0;
    uint64_t value = 0;
    SHR_FUNC_ENTER(unit);

    do {
        SHR_IF_ERR_EXIT
            (bcmlrd_enum_symbol_to_scalar_by_type(unit, "UDF_HDR_FORMAT_T",
                                                  "OUTER_L2_HDR_L2", &value));
        if ( value == udf_policy_info->packet_header) {
            base_offset = MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L2;
            break;
        }

        SHR_IF_ERR_EXIT
            (bcmlrd_enum_symbol_to_scalar_by_type(unit, "UDF_HDR_FORMAT_T",
                                                  "OUTER_L3_L4_HDR_IPV4", &value));
        if ( value == udf_policy_info->packet_header) {
            base_offset = MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L3;
            break;
        }

        SHR_IF_ERR_EXIT
            (bcmlrd_enum_symbol_to_scalar_by_type(unit, "UDF_HDR_FORMAT_T",
                                                  "INNER_L3_L4_HDR_UDP", &value));
        if ( value == udf_policy_info->packet_header) {
            base_offset = MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L4;
            break;
        }
    } while (0);

    if (base_offset == 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "FT(unit %d) Error: Invalid base offset: "
                            "%d\n"),
                 unit, udf_policy_info->packet_header));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num_chunks = 0;
    *num_udf_quals = 0;
    for (chunk = 0; chunk < BCM56780_A0_FT_NUM_2BYTE_UDF_CONTAINERS; chunk++) {
        if (udf_policy_info->cont_2_byte[chunk] == true) {
            /* Populate the qualifier info */
            qual_info[*num_udf_quals].qual =
                    bcm56780_a0_ft_udf_chunk_to_2byte_cont_mapping[chunk];
            qual_info[*num_udf_quals].base_offset = base_offset;
            tp_info->qual[num_chunks] =
                bcm56780_a0_ft_udf_chunk_to_2byte_cont_mapping[chunk];
            tp_info->udf = 1;


            qual_info[*num_udf_quals].width = 2;
            qual_info[*num_udf_quals].relative_offset =
                (2 * num_chunks) +
                udf_policy_info->offset;

            num_chunks++;
            (*num_udf_quals)++;
        }
    }
    for (chunk = 0; chunk < BCM56780_A0_FT_NUM_1BYTE_UDF_CONTAINERS; chunk++) {
        if (udf_policy_info->cont_1_byte[chunk] == true) {
            /* Populate the qualifier info */
            qual_info[*num_udf_quals].qual =
                    bcm56780_a0_ft_udf_chunk_to_1byte_cont_mapping[chunk];
            qual_info[*num_udf_quals].base_offset = base_offset;
            tp_info->qual[num_chunks] =
                bcm56780_a0_ft_udf_chunk_to_1byte_cont_mapping[chunk];
            tp_info->udf = 1;


            qual_info[*num_udf_quals].width = 1;
            qual_info[*num_udf_quals].relative_offset =
                (1 * num_chunks) +
                udf_policy_info->offset;

            num_chunks++;
            (*num_udf_quals)++;
        }
    }
    tp_info->num_qual = num_chunks;

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_mon_flowtracker_flex_em_grp_qual_info_extract(
        int unit,
        bcmcth_mon_ft_group_t *entry,
        ing_grp_template_partition_info_t *grp_tmp_part_info,
        ft_udf_policy_info_t *udf_policy_info,
        uint8_t num_quals,
        mcs_ft_msg_ctrl_flex_em_group_create_t *msg)
{
    int i, j, k, qidx;
    uint16_t offset, next_offset, width, word_boundary;
    int rv;
    int udf;
    int num_tp = 0;
    uint8_t num_udf_quals = 0,ft_qual;
    mcs_ft_msg_ctrl_qual_info_t *qual_info = NULL;
    SHR_FUNC_ENTER(unit);

    /* Some qual parts are contiguous but field stores them as separate parts,
     * combine them to reduce the number of bitops the FW has to perform on each
     * flow
     */
    for (i = 0; i < num_quals; i++) {
        j = 0;
        while (j < (grp_tmp_part_info->qual_info[i].num_qual_info - 1)) {
            offset      = grp_tmp_part_info->qual_info[i].qual_offset[j];
            width       = grp_tmp_part_info->qual_info[i].qual_width[j];
            next_offset = grp_tmp_part_info->qual_info[i].qual_offset[j + 1];

            if ((offset + width) == next_offset) {
                /* Increase the width  */
                grp_tmp_part_info->qual_info[i].qual_width[j] +=
                            grp_tmp_part_info->qual_info[i].qual_width[j + 1];

                /* Remove the next element */
                for (k = j + 1;
                     k < (grp_tmp_part_info->qual_info[i].num_qual_info - 1);
                     k++) {
                    grp_tmp_part_info->qual_info[i].qual_offset[k] =
                        grp_tmp_part_info->qual_info[i].qual_offset[k + 1];
                    grp_tmp_part_info->qual_info[i].qual_width[k]  =
                        grp_tmp_part_info->qual_info[i].qual_width[k + 1];
                }
                grp_tmp_part_info->qual_info[i].num_qual_info--;
            } else {
                j++;
            }
        }
    }

    /* Fill up the tracking params and part of qualifier info  */
    qidx = 0;
    msg->num_tp = entry->num_tp;
    /*
     * Assiging entry->num_tp to a temporary int variable to avoid compilation
     * error regarding sign difference when i is checked against num_tp.
     */
    num_tp = entry->num_tp;
    for (i = 0; i < num_tp; i++) {
        udf  = 0;
        switch (entry->tp[i].type) {
            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_SRC_IPV4:
                msg->tp_info[i].param     = MCS_SHR_FT_TP_SRC_IPV4;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_SRC_IPV4;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_SRC_IPV4;
                qidx++;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_DST_IPV4:
                msg->tp_info[i].param     = MCS_SHR_FT_TP_DST_IPV4;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_DST_IPV4;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_DST_IPV4;
                qidx++;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_L4_SRC_PORT:
                msg->tp_info[i].param     = MCS_SHR_FT_TP_L4_SRC_PORT;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_L4_SRC_PORT;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_L4_SRC_PORT;
                qidx++;
                break;
            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_L4_DST_PORT:
                msg->tp_info[i].param     = MCS_SHR_FT_TP_L4_DST_PORT;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_L4_DST_PORT;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_L4_DST_PORT;
                qidx++;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_IP_PROTOCOL:
                msg->tp_info[i].param     = MCS_SHR_FT_TP_IP_PROTOCOL;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_IP_PROTOCOL;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_IP_PROTOCOL;
                qidx++;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_VNID:
                msg->tp_info[i].param     = MCS_SHR_FT_TP_VNID;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_VNID;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_VNID;
                qidx++;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_SRC_IPV4:
                msg->tp_info[i].param = MCS_SHR_FT_TP_INNER_SRC_IPV4;
                udf                   = 1;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_DST_IPV4:
                msg->tp_info[i].param = MCS_SHR_FT_TP_INNER_DST_IPV4;
                udf                   = 1;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_L4_SRC_PORT:
                msg->tp_info[i].param = MCS_SHR_FT_TP_INNER_L4_SRC_PORT;
                udf                   = 1;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_L4_DST_PORT:
                msg->tp_info[i].param = MCS_SHR_FT_TP_INNER_L4_DST_PORT;
                udf                   = 1;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_INNER_IP_PROTOCOL:
                msg->tp_info[i].param = MCS_SHR_FT_TP_INNER_IP_PROTOCOL;
                udf                   = 1;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_CUSTOM:
                msg->tp_info[i].param = MCS_SHR_FT_TP_CUSTOM;
                udf                   = 1;
                break;

            case BCMLTD_COMMON_FLOWTRACKER_TRACKING_PARAM_TYPE_T_T_IN_PORT:
                msg->tp_info[i].param = MCS_SHR_FT_TP_IN_PORT;
                msg->tp_info[i].num_qual  = 1;
                msg->tp_info[i].qual[0]   = MCS_SHR_FT_EM_QUAL_IN_PORT;
                msg->qual_info[qidx].qual = MCS_SHR_FT_EM_QUAL_IN_PORT;
                qidx++;
                break;

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                         "FT(unit %d) Error: Invalid tracking param: "
                         "%d\n"),
                         unit, entry->tp[i].type));
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (udf == 1) {
            rv = bcm56780_a0_cth_mon_flowtracker_flex_em_grp_udf_info_extract(
                    unit,
                    entry->tp[i].udf_policy_id,
                    entry,
                    &(msg->tp_info[i]),
                    &(msg->qual_info[qidx]),
                    &(udf_policy_info[entry->tp[i].type]),
                    &num_udf_quals);
            if (rv != SHR_E_NONE) {
                SHR_ERR_EXIT(rv);
            }
            qidx += num_udf_quals;
        }
    }
    msg->num_qual = qidx;

    if (msg->num_qual != num_quals) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                   "FT(unit %d) Error: Unexpected number of qualifiers: "
                   "(%u, %u)\n"),
                   unit, msg->num_qual, num_quals));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < num_quals; i++) {
        ft_qual = grp_tmp_part_info->qual_info[i].qual;

        /* Find the info structure corresponding to this qualifier */
        for (j = 0; j < num_quals; j++) {
            if (msg->qual_info[j].qual == ft_qual) {
                qual_info = &(msg->qual_info[j]);
                break;
            }
        }
        if (j >= num_quals) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: %u qualifier not found\n"),
                     unit, ft_qual));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        for (j = 0, k = 0;
             j < grp_tmp_part_info->qual_info[i].num_qual_info;
             j++) {
            offset = grp_tmp_part_info->qual_info[i].qual_offset[j];
            width  = grp_tmp_part_info->qual_info[i].qual_width[j];

            if (k >= MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "FT(unit %d) Error:"
                                    " Excess num_offsets than expected "
                                    "num_offsets=%u\n"),
                         unit, k));
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }

            word_boundary = offset + (MCS_SHR_FT_EM_KEY_DWORD_SIZE -
                    (offset % MCS_SHR_FT_EM_KEY_DWORD_SIZE));
            if ((offset + width) > word_boundary) {
                /* The key is present in multiple words, split it into different
                 * offsets to help the FW
                 */
                qual_info->part_offset[k] = offset;
                qual_info->part_width[k]  = word_boundary - offset;

                do {
                    k++;
                    if (k >= MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "FT(unit %d) Error:"
                                            "Excess num_offsets than expected "
                                            "num_offsets=%u\n"),
                                 unit, k));
                        SHR_ERR_EXIT(SHR_E_RESOURCE);
                    }
                    qual_info->part_offset[k] = word_boundary;
                    qual_info->part_width[k]  = width - (word_boundary - offset);
                    word_boundary += MCS_SHR_FT_EM_KEY_DWORD_SIZE;
                } while (qual_info->part_width[k] > MCS_SHR_FT_EM_KEY_DWORD_SIZE);

                k++;
                if (k > MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                    LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: Excess num_offsets than"
                                  " expected num_offsets=%u\n"),
                       unit, k));
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            } else {
                qual_info->part_offset[k] = offset;
                qual_info->part_width[k]  = width;
                k++;
                if (k > MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                        "FT(unit %d) Error: Excess num_offsets than expected "
                        "num_offsets=%u\n"),
                         unit, k));
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }
        }
        qual_info->num_parts = k;

    }


exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_grp_flex_em_msg_init(
    int unit,
    bcmcth_mon_ft_group_t *entry,
    mcs_ft_msg_ctrl_flex_em_group_create_t *msg,
    uint32_t *pdd_index)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    ing_grp_template_info_t  grp_template_info;
    ing_grp_template_partition_info_t *grp_template_partition_info = NULL;
    uint32_t dt_em_ft_pdd_template_id = 0;
    ing_pdd_template_partition_info_t *pdd_template_partition_info = NULL;
    ft_udf_policy_info_t *udf_policy_info = NULL;
    uint8_t num_actions = 0;
    uint8_t num_quals = 0;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(grp_template_partition_info,
            sizeof(ing_grp_template_partition_info_t) * FLOWTRACKER_EM_QUAL_MAX,
            "bcmcthMonFtEmTempPartInfo");

    SHR_ALLOC(pdd_template_partition_info,
            sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_EM_ACTION_MAX,
            "bcmcthMonFtEmPddTempPartInfo");

    SHR_ALLOC(udf_policy_info,
            sizeof(ft_udf_policy_info_t) * BCM56780_A0_FT_MAX_UDF_TRACKING_PARAM,
            "bcmcthMonFtUdfInfo");
    sal_memset(&grp_template_info, 0 , sizeof(grp_template_info));
    sal_memset(grp_template_partition_info, 0,
                sizeof(ing_grp_template_partition_info_t) * FLOWTRACKER_EM_QUAL_MAX);
    sal_memset(pdd_template_partition_info, 0,
                sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_EM_ACTION_MAX);
    sal_memset(udf_policy_info, 0,
                sizeof(ft_udf_policy_info_t) * BCM56780_A0_FT_MAX_UDF_TRACKING_PARAM);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);
    if (ft_drv && ft_drv->ing_grp_template_info_lt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_mon_ing_grp_template_info_lt_lookup
             (unit,
              entry,
              &grp_template_info));
        if (entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
            SHR_EXIT();
        }
    }

    if (ft_drv && ft_drv->ing_grp_template_partition_info_lt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_mon_ing_grp_template_partition_info_lt_lookup
             (unit,
              entry,
              &grp_template_info,
              grp_template_partition_info,
              &num_quals));
        if (entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
            SHR_EXIT();
        }
    }
    if (ft_drv) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_mon_dt_em_ft_grp_template_lt_lookup
             (unit,
              entry,
              &dt_em_ft_pdd_template_id));
        if (entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
            SHR_EXIT();
        }
    }
    if (ft_drv && ft_drv->ing_pdd_template_partition_info_lt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_mon_ing_pdd_template_partition_info_lt_lookup
             (unit,
              &(entry->oper),
              dt_em_ft_pdd_template_id,
              pdd_template_partition_info,
              &num_actions));
        if (entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
            SHR_EXIT();
        }
    }
    if (ft_drv && ft_drv->udf_policy_lt) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_mon_udf_policy_lt_lookup
             (unit,
              entry,
              udf_policy_info));
        if (entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
            SHR_EXIT();
        }
    }

    msg->num_pipes = BCM56780_A0_FT_PIPES_MAX;
    msg->key_type = grp_template_partition_info->key_type;
    /*
     * On TD4 only BUCKET_MODE_0  is supported for
     * SW based hash insertion support
     */
    msg->bucket_mode = MCS_SHR_FT_HASH_BM_0;
    if (num_quals > 0) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm56780_a0_cth_mon_flowtracker_flex_em_grp_key_part_info_extract(
                    unit,
                    grp_template_partition_info,
                    num_quals,
                    msg));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm56780_a0_cth_mon_flowtracker_flex_em_grp_qual_info_extract(
                    unit,
                    entry,
                    grp_template_partition_info,
                    udf_policy_info,
                    num_quals,
                    msg));
    } else {
        /* num quals <= 0 is invalid */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (num_actions > 0) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcm56780_a0_cth_mon_flowtracker_flex_em_grp_action_info_extract(
                    unit,
                    entry,
                    pdd_template_partition_info,
                    num_actions,
                    msg));
    } else {
        /* At least ctr should have been there. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    
    *pdd_index = 0x1f;
exit:
    SHR_FREE(grp_template_partition_info);
    SHR_FREE(pdd_template_partition_info);
    SHR_FREE(udf_policy_info);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_ft_disable_lp(int unit, uint8_t bank_idx)
{
    int index = 0;
    bcmltd_sid_t lt_id = 0;
    bcmdrd_sid_t pt_id = 0;
    uint32_t *buf_ptr = NULL;

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_t i90_e2t_00_bank;
    IFTA90_E2T_00_HASH_CONTROLm_t i90_e2t_00_hash;
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_t i90_e2t_01_bank;
    IFTA90_E2T_01_HASH_CONTROLm_t i90_e2t_01_hash;

    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_CONTROLt;

    if (bank_idx == 0 || bank_idx == 1) {
        pt_id = IFTA90_E2T_00_SHARED_BANKS_CONTROLm;
        buf_ptr = (uint32_t *)(&i90_e2t_00_bank);

        SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_pt_iread(unit, lt_id, pt_id, index, buf_ptr));
        IFTA90_E2T_00_SHARED_BANKS_CONTROLm_BANK_DISABLE_LPf_SET(
                i90_e2t_00_bank, 0x3);
        SHR_IF_ERR_EXIT
            (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id, index, buf_ptr));

        buf_ptr = (uint32_t *)(&i90_e2t_00_hash);
        pt_id = IFTA90_E2T_00_HASH_CONTROLm;
        SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_pt_iread(unit, lt_id, pt_id, index, buf_ptr));
        IFTA90_E2T_00_HASH_CONTROLm_DISABLE_LPf_SET(i90_e2t_00_hash, 0x1);
        IFTA90_E2T_00_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i90_e2t_00_hash, 0x0);
        SHR_IF_ERR_EXIT
            (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id, index, buf_ptr));

    }

    if (bank_idx == 2 || bank_idx ==3) {
        pt_id = IFTA90_E2T_01_SHARED_BANKS_CONTROLm;
        buf_ptr = (uint32_t *)(&i90_e2t_01_bank);
        SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mon_pt_iread(unit, lt_id, pt_id, index, buf_ptr));
        IFTA90_E2T_01_SHARED_BANKS_CONTROLm_BANK_DISABLE_LPf_SET(
                i90_e2t_01_bank, 0x3);
        SHR_IF_ERR_EXIT
            (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id, index, buf_ptr));

        buf_ptr = (uint32_t *)(&i90_e2t_01_hash);
        pt_id = IFTA90_E2T_01_HASH_CONTROLm;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_pt_iread(unit, lt_id, pt_id, index, buf_ptr));
        IFTA90_E2T_01_HASH_CONTROLm_DISABLE_LPf_SET(i90_e2t_01_hash, 0x1);
        IFTA90_E2T_01_HASH_CONTROLm_ROBUST_HASH_ENf_SET(i90_e2t_01_hash, 0x0);
        SHR_IF_ERR_EXIT
            (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id, index, buf_ptr));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_em_uft_msg_init(
    int unit,
    mcs_ft_msg_ctrl_em_bank_uft_info_t *msg,
    bcmltd_common_flowtracker_hardware_learn_t_t hw_learn)
{
    bcmltd_sid_t ltid;
    bcmptm_pt_banks_info_t banks_info;
    uint32_t bank_id_offset = 0;
    uint8_t bank_idx = 0;
    bcmptm_pt_banks_info_t pt_banks_info;

    SHR_FUNC_ENTER(unit);

    if (FLOWTRACKER_HW_LEARN_ENABLED(hw_learn)) {
        ltid = DT_EM_FT_ENTRYt;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_cth_uft_bank_info_get_from_ptcache(unit,
                                                       IFTA80_E2T_00_B0_QUADm,
                                                       &pt_banks_info));
        bank_id_offset = pt_banks_info.bank[0].bank_id;
    } else {
        ltid = DT_EM_FP_ENTRYt;
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_cth_uft_bank_info_get_from_ptcache(unit,
                                                       IFTA90_E2T_00_B0_QUADm,
                                                       &pt_banks_info));
        bank_id_offset = pt_banks_info.bank[0].bank_id;
    }


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_cth_uft_bank_info_get_from_lt(unit, ltid, 0, &banks_info));

    msg->num_banks = banks_info.bank_cnt;
    if (msg->num_banks > MCS_SHR_FT_MAX_UFT_BANKS) {
        /* Max we can support is 4 */
        msg->num_banks = MCS_SHR_FT_MAX_UFT_BANKS;
    }
    for (bank_idx = 0; bank_idx < msg->num_banks; bank_idx++) {
        msg->bank_id[bank_idx] =
            banks_info.bank[bank_idx].bank_id - bank_id_offset;
        msg->bank_offset[bank_idx] = banks_info.bank[bank_idx].hash_offset;
        if (FLOWTRACKER_HW_LEARN_ENABLED(hw_learn)) {
            /* IFTA80_E2T series */
            msg->bank_num_buckets[bank_idx] = 4096;
        } else {
            /* IFTA90_E2T series */
            msg->bank_num_buckets[bank_idx] = 16384;
        }

        /* Disable LP */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_cth_mon_ft_disable_lp(unit, bank_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_flex_ctr_action_profile_info_get(
                                        int unit,
                                        uint32_t ctr_ing_flex_action_profile_id,
                                        uint8_t *num_ctr_pools,
                                        uint8_t *ctr_base_idx)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    uint32_t fid = 0, sid = 0;
    size_t fld_count = 0;
    const bcmltd_field_t *gen_field;
    unsigned int i;
    SHR_FUNC_ENTER(unit);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);

    if (ft_drv && ft_drv->flex_ctr_action_profile_info_lt) {
        sid = ft_drv->flex_ctr_action_profile_info_lt->ltid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_count_get(unit, sid, &fld_count));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_util_fields_alloc(unit, 1, &in_flds));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_util_fields_alloc(unit, fld_count, &out_flds));

        in_flds.count = 1;
        in_flds.field[0]->id =
                        ft_drv->flex_ctr_action_profile_info_lt->key_fid;
        in_flds.field[0]->data = ctr_ing_flex_action_profile_id;

        if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
                SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        for (i = 0; i < out_flds.count; i++) {
            gen_field = out_flds.field[i];
            fid = gen_field->id;
            if (fid == ft_drv->flex_ctr_action_profile_info_lt->num_pools_fid) {
                *num_ctr_pools = gen_field->data;
            }
            if (fid ==
                    ft_drv->flex_ctr_action_profile_info_lt->base_index_fid) {
                *ctr_base_idx = gen_field->data;
            }
        }
    }
exit:
    bcmcth_mon_ft_util_fields_free(unit, 1, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_flex_ctr_action_profile_get(
                                        int      unit,
                                        uint32_t ctr_ing_flex_action_profile_id,
                                        uint8_t  *pool_id,
                                        uint32_t *num_ctrs,
                                        uint16_t *action_id)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    const bcmltd_field_t *gen_field;
    uint32_t fid = 0, sid = 0;
    size_t fld_count = 0;
    unsigned int i;
    SHR_FUNC_ENTER(unit);

    ft_drv = bcm56780_a0_cth_mon_ft_drv_get(unit);
    if (ft_drv && ft_drv->flex_ctr_action_profile_lt) {
        sid = ft_drv->flex_ctr_action_profile_lt->ltid;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_count_get(unit, sid, &fld_count));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_util_fields_alloc(unit, 1, &in_flds));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_util_fields_alloc(unit, fld_count, &out_flds));

        in_flds.count = 1;
        in_flds.field[0]->id = ft_drv->flex_ctr_action_profile_lt->key_fid;
        in_flds.field[0]->data = ctr_ing_flex_action_profile_id;

        if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) !=
                SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        for (i = 0; i < out_flds.count; i++) {
            gen_field = out_flds.field[i];
            fid = gen_field->id;
            if (fid == ft_drv->flex_ctr_action_profile_lt->pool_id_fid) {
                *pool_id = gen_field->data;
            }
            if (fid == ft_drv->flex_ctr_action_profile_lt->num_counters_fid) {
                *num_ctrs = gen_field->data;
            }
            if (fid == ft_drv->flex_ctr_action_profile_lt->action_fid) {
                *action_id = gen_field->data;
            }
        }
    }
exit:
    bcmcth_mon_ft_util_fields_free(unit, 1, &in_flds);
    bcmcth_mon_ft_util_fields_free(unit, fld_count, &out_flds);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_control_msg_init(
    int unit,
    bcmcth_mon_ft_control_t *entry,
    mcs_ft_msg_ctrl_init_t *msg)
{
    uint32_t max_flows = 0;
    uint8_t num_ctr_pools = 0, ctr_base_idx = 0, pool_id = 0, i = 0, j = 0;
    uint32_t num_counters = 0;
    uint16_t action_id = 0;
    bcmdrd_sid_t sid = 0;
    SHR_FUNC_ENTER(unit);

    if (bcm56780_a0_cth_mon_flowtracker_flex_ctr_action_profile_info_get(
                                unit,
                                entry->ctr_ing_flex_action_profile_id,
                                &num_ctr_pools,
                                &ctr_base_idx
                                ) == SHR_E_NOT_FOUND) {
        entry->oper.operational_state =
            BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_CTR_ING_FLEX_ACTION_PROFILE_ID_NOT_EXISTS;
        SHR_EXIT();
    }

    if (bcm56780_a0_cth_mon_flowtracker_flex_ctr_action_profile_get(
                                unit,
                                entry->ctr_ing_flex_action_profile_id,
                                &pool_id,
                                &num_counters,
                                &action_id
                                ) == SHR_E_NOT_FOUND) {
        entry->oper.operational_state =
            BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_CTR_ING_FLEX_ACTION_PROFILE_ID_NOT_EXISTS;
        SHR_EXIT();
    }

    msg->flex_ctr_action_idx = action_id;
    for (i = 0; i < BCM56780_A0_FT_PIPES_MAX; i++) {
        max_flows = entry->max_flows[i];
        if (max_flows == 0) {
            msg->pipe[i].num_ctr_pools = 0;
        } else {
            msg->pipe[i].num_ctr_pools = num_ctr_pools;
        }

        if (num_counters < max_flows) {
            entry->oper.operational_state =
                BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_CTR_ING_FLEX_POOLS_NOT_SUFFICIENT;
            SHR_EXIT();
        }


        num_ctr_pools = msg->pipe[i].num_ctr_pools;
        for (j = 0; j < num_ctr_pools; j++) {
            msg->pipe[i].ctr_pools[j] = pool_id + j;

            sid = evict_pool[pool_id + j];
            SHR_IF_ERR_EXIT
                (bcmptm_cci_evict_control_set(unit, sid,
                                              TRUE , CTR_EVICT_MODE_DISABLE));

            if (j == num_ctr_pools - 1) {
                /*
                 * Only the last pool will have counters less
                 * than BCM56780_A0_FT_MAX_COUNTERS_PER_POOL
                 */
                if (entry->max_flows[i] %
                        BCM56780_A0_FT_MAX_COUNTERS_PER_POOL == 0) {
                    msg->pipe[i].ctr_pool_size[j] =
                        BCM56780_A0_FT_MAX_COUNTERS_PER_POOL;
                } else {
                    msg->pipe[i].ctr_pool_size[j] =
                        max_flows % BCM56780_A0_FT_MAX_COUNTERS_PER_POOL;
                }
            } else {
                msg->pipe[i].ctr_pool_size[j] =
                    BCM56780_A0_FT_MAX_COUNTERS_PER_POOL;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static uint32_t
bcm56780_a0_cth_mon_flowtracker_get_action_val(
                           uint32_t num_actions,
                           flowtracker_action_info_t *actions,
                           bcmltd_common_flowtracker_action_type_t_t req_type)
{
    uint32_t i;
    uint32_t val = 0;
    for (i = 0; i < num_actions; i++) {
        if (actions[i].type == req_type) {
            switch (req_type) {
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_OPAQUE_OBJ0:
                    val = actions[i].em_ft_opaque_obj0_val;
                    return (val & 0xffff);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_IOAM_GBP_ACTION:
                    val = actions[i].em_ft_ioam_gbp_action_val;
                    return (val & 0xf);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_COPY_TO_CPU:
                    val = actions[i].em_ft_copy_to_cpu_val;
                    return (val & 0x1);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_DROP_ACTION:
                    val = actions[i].em_ft_drop_action_val;
                    return (val & 0x3);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_DESTINATION:
                    val = actions[i].destination_val;
                    return (val & 0xffff);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_DESTINATION_TYPE:
                    val = actions[i].destination_type_val;
                    return (val & 0xf);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_FLEX_STATE_ACTION:
                    val = actions[i].em_ft_flex_state_action_val;
                    return (val & 0xf);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_PKT_FLOW_ELIGIBILITY:
                    val = actions[i].pkt_flow_eligibility_val;
                    return (val & 0xf);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_FLEX_CTR_ACTION:
                    /*
                     * For ctr, data is simply ctr action ID set in 2 to 7 bits of
                     * TILE_AUX_PDD_MENU.
                     */
                    val = actions[i].flex_ctr_action_val;
                    return ((val << 2) & 0xff);
                    break;
                case BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_L2_IIF_SVP_MIRROR_INDEX_0:
                    val = actions[i].l2_iif_svp_mirror_index_0_val;
                    return (val & 0xf);
                    break;
                default:
                    /* Unexpected */
                    return 0;
                    break;
            }
        }
    }
    return 0;
}

static uint32_t
bcm56780_a0_cth_mon_flowtracker_get_em_ft_opaque_obj(
                                bcmcth_mon_ft_group_t *entry)
{
    uint32_t i;
    for (i = 0; i < entry->num_actions; i++) {
        if (entry->actions[i].type ==
            BCMLTD_COMMON_FLOWTRACKER_ACTION_TYPE_T_T_EM_FT_OPAQUE_OBJ0) {
            /*
             * Only pick out the group idx which is fixed.
             * Every other portion of opaque object will be configured
             * by UKERNEL dynamically.
             */
            return (((entry->mon_flowtracker_group_id)
                   << MCS_SHR_FT_GROUP_IDX_OFFSET) & MCS_SHR_FT_GROUP_IDX_MASK);
        }
    }
    return 0;
}

static int
bcm56780_a0_cth_mon_flowtracker_grp_flow_limit_update(
                                int unit,
                                bcmcth_mon_ft_group_t *entry
                                )
{
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_GROUPt;
    bcmdrd_sid_t pt_id = FT_GROUP_TABLE_CONFIGm;
    FT_GROUP_TABLE_CONFIGm_t ft_group_table_config;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ft_group_table_config, 0, sizeof(ft_group_table_config));

    /* Read from the PT */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iread(unit,
                             lt_id,
                             pt_id,
                             entry->mon_flowtracker_group_id,
                             &ft_group_table_config));

    /*
     * Flow threshold is got from flow_limit in the group entry.
     */
    FT_GROUP_TABLE_CONFIGm_FLOW_THRESHOLDf_SET(ft_group_table_config,
                                               entry->flow_limit);

    /* Write into the PT */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite(unit,
                              lt_id,
                              pt_id,
                              entry->mon_flowtracker_group_id,
                              &ft_group_table_config));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_hitbit_type_get_from_key_data_sizes(
                    uint32_t key_size,
                    uint32_t action_width_in_bits)
{
    uint32_t first_entry_key_type_base_valid_size = 7; /* 4 bits(key_type) +
                                                        * 3 bits(base_valid)
                                                        */
    uint32_t second_entry_base_valid_size = 3;
    uint32_t one_base_entry_size = 120; /* 120 bits */

    /* Currently we do not support single wide entries */
    if ((key_size + action_width_in_bits +  first_entry_key_type_base_valid_size
        + second_entry_base_valid_size) <= (2 * one_base_entry_size)) {
        return MCS_SHR_FT_HIT_BIT_TYPE_2x;
    } else {
        return MCS_SHR_FT_HIT_BIT_TYPE_4x;
    }

    return MCS_SHR_FT_HIT_BIT_TYPE_NONE;
}

static void
bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config_data_type_width_set(
                                                int unit,
                                                FT_GROUP_TABLE_CONFIGm_t *ft_group_table_config,
                                                uint32_t *buf,
                                                uint32_t total_width_in_bits,
                                                uint32_t sbr_index,
                                                uint32_t pdd_index,
                                                uint32_t data_format,
                                                uint16_t *curr_offset)
{
    uint32_t val = 0;
    int      data_width = 0;
    uint32_t add_bits = 0;
    /*
     * NOTE: As per TD4X9-20(1st comment), there are 3 formats compared
     *       to 2 in X11.But we would support the 2 formats only since
     *       as per EM team, they have not implemented the other format.
     *
     * Format 1: Data (104 bits) only
     * Format 2: PDD idx(5bits) + SBR idx(4bits) + Data (95bits)
     * For Format 2 just put the PDD idx and SBR idx in the DATA portion.
     */

    if (data_format ==
            FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_PLUS_PDD_IDX_PLUS_SBR_IDX) {
        /* Fill 5 bits of PDD index first */
        val = pdd_index;
        bcmdrd_field_set(buf, 99, 103, &val);

        /* Fill 4 bits of SBR index next */
        val = pdd_index;
        bcmdrd_field_set(buf, 95, 98, &val);

        /*
         * Indicate how many additional bits apart from actual data is stored.
         * In this case 5 + 4 bits of PDD and SBR indexes are additionally
         * stored.
         */
        add_bits = 9;

    }

    /* Calculation of start offset for actual data within the buf */
    *curr_offset = 104 - add_bits - total_width_in_bits;

    /*
     * Data width is configured in following format.
     * 4 bits = 0, 8 bits = 1, 12 bits = 2, 16 bits = 3, 20 bits = 4,
     * 24 bits = 5 and so on.
     * Add additional_bits to action data size.
     */
    data_width = (((total_width_in_bits+add_bits)+3)/4) - 1;
    if (data_width < 0) {
        data_width = 0;
    }
    val = data_width;
    FT_GROUP_TABLE_CONFIGm_DATA_WIDTHf_SET((*ft_group_table_config), val);
}

static int
bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config(
                                int unit,
                                bcmcth_mon_ft_control_t *ctrl_entry,
                                bcmcth_mon_ft_group_t *entry,
                                mcs_ft_msg_ctrl_flex_em_group_create_t *msg,
                                uint32_t pdd_index,
                                uint32_t sbr_index,
                                uint8_t  data_format)
{
    uint32_t buf[4] = {0}; /*
                            * Flex policy data is 100 bits
                            * + data type(5 bits) + data_width(5 bits) =
                            * associated_data = 110 bits (bits 0 to 109)
                            */
    uint32_t total_width_in_bits = 0;
    uint32_t val = 0;
    uint32_t em_ft_opaque_obj_val_0 = 0;
    uint16_t curr_offset = 0;
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_GROUPt;
    bcmdrd_sid_t pt_id = FT_GROUP_TABLE_CONFIGm;
    int i;
    int hitbit_type = MCS_SHR_FT_HIT_BIT_TYPE_NONE;
    FT_GROUP_TABLE_CONFIGm_t ft_group_table_config;

    SHR_FUNC_ENTER(unit);

    /* Find action total width in bits */
    for (i = 0; i < msg->num_actions; i++) {
        total_width_in_bits += msg->aset_widths[i];
    }

    hitbit_type =
        bcm56780_a0_cth_mon_flowtracker_hitbit_type_get_from_key_data_sizes(
                                                        msg->key_size,
                                                        total_width_in_bits);

    hitbit_type =
        bcm56780_a0_cth_mon_flowtracker_hitbit_type_get_from_key_data_sizes(
                                                        msg->key_size,
                                                        total_width_in_bits);


    sal_memset(&ft_group_table_config, 0, sizeof(ft_group_table_config));


    bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config_data_type_width_set(
                                                            unit,
                                                            &ft_group_table_config,
                                                            buf,
                                                            total_width_in_bits,
                                                            sbr_index,
                                                            pdd_index,
                                                            data_format,
                                                            &curr_offset);

    for (i = 0; i < msg->num_actions; i++) {
        switch (msg->aset[i]) {
            case SHR_FT_EM_ACTION_CTR:
                /*
                 * For ctr, data is simply ctr action ID set in 2 to 7 bits of
                 * TILE_AUX_PDD_MENU.
                 * We need to use group action ID for both main ctr action
                 * and hitbit ctr action to take effect.
                 */
                if (hitbit_type == MCS_SHR_FT_HIT_BIT_TYPE_2x) {
                    val =
                  ((ctrl_entry->db_ewide_hb_ctr_ing_flex_grp_action)<<2) & 0xff;
                } else if (hitbit_type == MCS_SHR_FT_HIT_BIT_TYPE_4x) {
                    val =
                  ((ctrl_entry->qd_ewide_hb_ctr_ing_flex_grp_action)<<2) & 0xff;
                }
                break;
            case SHR_FT_EM_ACTION_OPAQUE_OBJ0:
                em_ft_opaque_obj_val_0 =
                    bcm56780_a0_cth_mon_flowtracker_get_em_ft_opaque_obj(entry);
                val = em_ft_opaque_obj_val_0 & 0xffff;
                break;
            default:
                /* No other action is supported. Others are set to 0. */
                val = 0;
                break;
        }
        bcmdrd_field_set(buf, curr_offset,
                         curr_offset + msg->aset_widths[i] -1,
                         &val);
        curr_offset = curr_offset + msg->aset_widths[i];
    }

    /* For both the formats, buf holds FIXED_POLICY_DATA field. */
    FT_GROUP_TABLE_CONFIGm_FLEX_POLICY_DATAf_SET(ft_group_table_config,
            buf);

    /*
     * Flow threshold is got from flow_limit in the group entry.
     */
    FT_GROUP_TABLE_CONFIGm_FLOW_THRESHOLDf_SET(ft_group_table_config,
                                               entry->flow_limit);

    /* Write into the PT */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite(unit,
                              lt_id,
                              pt_id,
                              entry->mon_flowtracker_group_id,
                              &ft_group_table_config));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config_wo_eapp(
                                int unit,
                                bcmcth_mon_ft_group_t *entry)
{
    uint32_t buf[4] = {0}; /*
                            * Flex policy data is 100 bits
                            * + data type(5 bits) + data_width(5 bits) =
                            * associated_data = 110 bits (bits 0 to 109)
                            */
    uint32_t total_width_in_bits = 0;
    uint32_t val = 0;
    uint16_t curr_offset = 0;
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_GROUPt;
    bcmdrd_sid_t pt_id = FT_GROUP_TABLE_CONFIGm;
    int i, j;
    FT_GROUP_TABLE_CONFIGm_t ft_group_table_config;
    uint32_t dt_em_ft_pdd_template_id;
    uint16_t aset_widths[FLOWTRACKER_ACTIONS_MAX];
    uint8_t num_actions = 0;
    ing_pdd_template_partition_info_t *pdd_template_partition_info = NULL;
    
    uint32_t pdd_index = 0;
    uint32_t sbr_index = 0;
    uint8_t  data_format = FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_ONLY;
    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(pdd_template_partition_info,
            sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_ACTIONS_MAX,
            "bcmcthMonFtEmPddTempPartInfo");
    sal_memset(pdd_template_partition_info, 0,
            sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_ACTIONS_MAX);
    /* Lookup EM_FT_GROUP to get the PDD template ID */
    SHR_IF_ERR_EXIT(bcm56780_a0_cth_mon_dt_em_ft_grp_template_lt_lookup(
                unit,
                entry,
                &dt_em_ft_pdd_template_id));

    /*
     * Use PDD template ID to lookup PDD template partition info LT and get the
     * offsets and widths for actions configured in the group.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_cth_mon_ing_pdd_template_partition_info_lt_lookup
         (unit,
          &(entry->oper),
          dt_em_ft_pdd_template_id,
          pdd_template_partition_info,
          &num_actions));
    if (entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
        SHR_EXIT();
    }

    /* Find action total width in bits */
    for (i = 0; i < num_actions; i++) {
        aset_widths[i] = 0;
        for (j = 0; j < pdd_template_partition_info[i].num_action_info; j++) {
            aset_widths[i] +=
                pdd_template_partition_info[i].action_width[j];
        }
        total_width_in_bits += aset_widths[i];
    }

    sal_memset(&ft_group_table_config, 0, sizeof(ft_group_table_config));


    bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config_data_type_width_set(
                                                            unit,
                                                            &ft_group_table_config,
                                                            buf,
                                                            total_width_in_bits,
                                                            sbr_index,
                                                            pdd_index,
                                                            data_format,
                                                            &curr_offset);

    for (i = 0; i < num_actions; i++) {
        val = bcm56780_a0_cth_mon_flowtracker_get_action_val(
                entry->num_actions,
                entry->actions,
                pdd_template_partition_info[i].action);
        bcmdrd_field_set(buf, curr_offset,
                curr_offset + aset_widths[i] -1,
                &val);
        curr_offset = curr_offset + aset_widths[i];
    }

    /* For both the formats, buf holds FIXED_POLICY_DATA field. */
    FT_GROUP_TABLE_CONFIGm_FLEX_POLICY_DATAf_SET(ft_group_table_config,
            buf);

    /*
     * Flow threshold is got from flow_limit in the group entry.
     */
    FT_GROUP_TABLE_CONFIGm_FLOW_THRESHOLDf_SET(ft_group_table_config,
                                               entry->flow_limit);

    /* Write into the PT */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite(unit,
                              lt_id,
                              pt_id,
                              entry->mon_flowtracker_group_id,
                              &ft_group_table_config));
exit:
    SHR_FREE(pdd_template_partition_info);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_deinit(
                                int unit,
                                uint32_t mon_flowtracker_group_id)
{
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_GROUPt;
    bcmdrd_sid_t pt_id = FT_GROUP_TABLE_CONFIGm;
    FT_GROUP_TABLE_CONFIGm_t ft_group_table_config;

    SHR_FUNC_ENTER(unit);


    /* Reset the particular group table */
    sal_memset(&ft_group_table_config, 0, sizeof(ft_group_table_config));


    /*
     * Write into the PT - Since the acctype is duplicate,
     * it will get replicated across all pipes
     */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite(unit,
                              lt_id,
                              pt_id,
                              mon_flowtracker_group_id,
                              &ft_group_table_config));
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_mon_flowtracker_hw_learn_opt_msg_init(
                                int unit,
                                bcmcth_mon_ft_control_t *entry,
                                mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg)
{
    uint8_t num_ctr_pools = 0, ctr_base_idx = 0, pool_id = 0, i = 0;
    uint32_t num_ctrs = 0;
    int hit_bit_type;
    uint8_t hit_bit_type_ctr_action_index = 0;
    uint16_t remaining_ctr_size = 0;
    mcs_shr_ft_hitbit_ctr_bank_info_t *ctr_bank_info = NULL;
    uint16_t action_id = 0;
    bcmdrd_sid_t sid = 0;

    SHR_FUNC_ENTER(unit);

    msg->num_hitbit_ctr_banks = 0;
    for (hit_bit_type = MCS_SHR_FT_HIT_BIT_TYPE_START;
         hit_bit_type <= MCS_SHR_FT_HIT_BIT_TYPE_END;
         hit_bit_type++) {

        if (hit_bit_type == MCS_SHR_FT_HIT_BIT_TYPE_2x) {
            hit_bit_type_ctr_action_index =
                entry->db_ewide_hb_ctr_ing_flex_action_profile_id;
        } else if (hit_bit_type == MCS_SHR_FT_HIT_BIT_TYPE_4x) {
            hit_bit_type_ctr_action_index =
                entry->qd_ewide_hb_ctr_ing_flex_action_profile_id;
        } else {
            /* Rest are not supported. Simply continue */
            continue;
        }

        if (hit_bit_type_ctr_action_index == 0) {
            /* Not configured. Ignore */
            continue;
        }

        if (bcm56780_a0_cth_mon_flowtracker_flex_ctr_action_profile_info_get(
                    unit,
                    hit_bit_type_ctr_action_index,
                    &num_ctr_pools,
                    &ctr_base_idx
                    ) == SHR_E_NOT_FOUND) {
            entry->oper.operational_state =
                BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_CTR_ING_FLEX_ACTION_PROFILE_ID_NOT_EXISTS;
            SHR_EXIT();
        }

        if (bcm56780_a0_cth_mon_flowtracker_flex_ctr_action_profile_get(
                    unit,
                    hit_bit_type_ctr_action_index,
                    &pool_id,
                    &num_ctrs,
                    &action_id
                    ) == SHR_E_NOT_FOUND) {
            entry->oper.operational_state =
                BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_CTR_ING_FLEX_ACTION_PROFILE_ID_NOT_EXISTS;
            SHR_EXIT();
        }
        msg->ctr_bank_info[msg->num_hitbit_ctr_banks].hitbit_type =
            hit_bit_type;
        msg->ctr_bank_info[msg->num_hitbit_ctr_banks].num_ctr_pools =
            num_ctr_pools;
        msg->ctr_bank_info[msg->num_hitbit_ctr_banks].ctr_bank_base_idx =
            ctr_base_idx * 128; /*
                                 * 128 is size of
                                 * one ctr section within a pool
                                 */

        remaining_ctr_size = num_ctrs;
        for (i = 0; i < num_ctr_pools; i++) {

            ctr_bank_info = &(msg->ctr_bank_info[msg->num_hitbit_ctr_banks]);
            ctr_bank_info->ctr_pool_id[i] =
                pool_id + i;
            sid = evict_pool[pool_id + i];
            SHR_IF_ERR_EXIT
                (bcmptm_cci_evict_control_set(unit, sid,
                                              TRUE , CTR_EVICT_MODE_DISABLE));

            if (i == 0) /* first pool */ {
                ctr_bank_info->ctr_pool_start_idx[i] =
                    ctr_bank_info->ctr_bank_base_idx;
                ctr_bank_info->ctr_pool_size[i] =
                    BCM56780_A0_FT_MAX_COUNTERS_PER_POOL -
                    ctr_bank_info->ctr_bank_base_idx;
                if (ctr_bank_info->ctr_pool_size[i] > remaining_ctr_size) {
                    ctr_bank_info->ctr_pool_size[i] = remaining_ctr_size;
                }
            } else if (i == num_ctr_pools - 1) /* Last pool */ {
                ctr_bank_info->ctr_pool_start_idx[i] = 0;
                ctr_bank_info->ctr_pool_size[i] =
                    remaining_ctr_size;
            } else {
                ctr_bank_info->ctr_pool_start_idx[i] = 0;
                ctr_bank_info->ctr_pool_size[i] =
                    BCM56780_A0_FT_MAX_COUNTERS_PER_POOL;
            }
            remaining_ctr_size -=ctr_bank_info->ctr_pool_size[i];
        }
        msg->num_hitbit_ctr_banks++;
    }
exit:
    SHR_FUNC_EXIT();
}

static void
bcm56780_a0_cth_mon_flowtracker_hw_lt_info_init(int unit, bool with_eapp)
{
    int ltid = 0;

    /* FP related LTs */
    ltid = bcmlrd_table_name_to_idx(unit, ing_grp_template_info_ft_lt.ltid);
    if (ltid != SHR_E_UNAVAIL) {
        bcm56780_a0_flowtracker_drv.ing_grp_template_info_lt =
            &ing_grp_template_info_ft_lt;
    }
    ltid = bcmlrd_table_name_to_idx(unit, dt_em_ft_grp_template_ft_lt.lt_name);
    if (ltid != SHR_E_UNAVAIL) {
        bcm56780_a0_flowtracker_drv.dt_em_ft_grp_template_lt =
            &dt_em_ft_grp_template_ft_lt;
    }
    ltid = bcmlrd_table_name_to_idx(unit,
            ing_grp_template_partition_info_ft_lt.ltid);
    if (ltid != SHR_E_UNAVAIL) {
        bcm56780_a0_flowtracker_drv.ing_grp_template_partition_info_lt =
            &ing_grp_template_partition_info_ft_lt;
    }
    if (with_eapp) {
        ltid = bcmlrd_table_name_to_idx(unit,
                ing_pdd_template_partition_info_ft_lt.ltid);
        if (ltid != SHR_E_UNAVAIL) {
            bcm56780_a0_flowtracker_drv.ing_pdd_template_partition_info_lt =
                &ing_pdd_template_partition_info_ft_lt;
        }
    } else {
        ltid = bcmlrd_table_name_to_idx(unit,
                ing_pdd_template_partition_info_ft_noapp_lt.ltid);
        if (ltid != SHR_E_UNAVAIL) {
            bcm56780_a0_flowtracker_drv.ing_pdd_template_partition_info_lt =
                &ing_pdd_template_partition_info_ft_noapp_lt;
        }
    }
}

static int
bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_flow_cnt_get(
                                int unit,
                                uint32_t mon_flowtracker_group_id,
                                uint32_t *flow_cnt)
{
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_GROUPt;
    bcmdrd_sid_t pt_id = FT_GROUP_TABLE_CNTm;
    int pipe;
    FT_GROUP_TABLE_CNTm_t ft_group_table_cnt;

    SHR_FUNC_ENTER(unit);


    *flow_cnt = 0;

    /*
     * Read from the PT for 4 pipes and sum them
     */
    for (pipe = 0; pipe < BCM56780_A0_FT_PIPES_MAX; pipe++) {
        sal_memset(&ft_group_table_cnt, 0, sizeof(ft_group_table_cnt));
        SHR_IF_ERR_EXIT
            (bcmcth_mon_pt_iread_pipe(unit,
                                  lt_id,
                                  pt_id,
                                  pipe,
                                  mon_flowtracker_group_id,
                                  &ft_group_table_cnt));
        *flow_cnt += FT_GROUP_TABLE_CNTm_FLOW_CNTf_GET(ft_group_table_cnt);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_hw_learn_deinit(int unit)
{
    IFTA80_E2T_00_HASH_CONTROLm_t ifta80_e2t_00_hash_control;
    IFTA80_E2T_01_HASH_CONTROLm_t ifta80_e2t_01_hash_control;
    IFTA80_E2T_02_HASH_CONTROLm_t ifta80_e2t_02_hash_control;
    IFTA80_E2T_03_HASH_CONTROLm_t ifta80_e2t_03_hash_control;
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_t ifta80_e2t_01_shared_banks_control;
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_t ifta80_e2t_02_shared_banks_control;
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_t ifta80_e2t_03_shared_banks_control;
    uint8_t e2t_00_htbl_bnk_config, e2t_01_htbl_bnk_config;
    uint8_t e2t_02_htbl_bnk_config, e2t_03_htbl_bnk_config;
    uint8_t e2t_00_sbus_num_bnks, e2t_01_sbus_num_bnks;
    uint8_t e2t_02_sbus_num_bnks, e2t_03_sbus_num_bnks;
    uint8_t e2t_00_ft_enable, e2t_01_ft_enable;
    uint8_t e2t_02_ft_enable, e2t_03_ft_enable;
    uint8_t e2t_01_bank_bitmap_start_offset;
    uint8_t e2t_02_bank_bitmap_start_offset, e2t_03_bank_bitmap_start_offset;
    FT_GLOBAL_TABLE_CONFIGm_t ft_global_table_config;
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_CONTROLt;
    bcmdrd_sid_t pt_id = 0;
    int index = 0;
    SHR_FUNC_ENTER(unit);

    /* Reset the global flow threshold config */
    sal_memset(&ft_global_table_config, 0, sizeof(ft_global_table_config));
    pt_id = FT_GLOBAL_TABLE_CONFIGm;
    FT_GLOBAL_TABLE_CONFIGm_FLOW_THRESHOLDf_SET(ft_global_table_config,
                                                0);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ft_global_table_config));

    /*
     *  Reset the configs in IFTA80 tiles.
     */
    e2t_00_sbus_num_bnks = 1;
    e2t_01_sbus_num_bnks = 1;
    e2t_02_sbus_num_bnks = 1;
    e2t_03_sbus_num_bnks = 1;
    e2t_00_htbl_bnk_config = 0x3;
    e2t_01_htbl_bnk_config = 0x3;
    e2t_02_htbl_bnk_config = 0x3;
    e2t_03_htbl_bnk_config = 0x3;
    e2t_00_ft_enable = 0;
    e2t_01_ft_enable = 0;
    e2t_02_ft_enable = 0;
    e2t_03_ft_enable = 0;
    e2t_01_bank_bitmap_start_offset = 0;
    e2t_02_bank_bitmap_start_offset = 0;
    e2t_03_bank_bitmap_start_offset = 0;

    /* IFTA80_E2T_00 settings */
    sal_memset(&ifta80_e2t_00_hash_control,
        0, sizeof(ifta80_e2t_00_hash_control));
    pt_id = IFTA80_E2T_00_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_00_hash_control));
    IFTA80_E2T_00_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                             ifta80_e2t_00_hash_control, e2t_00_ft_enable);
    IFTA80_E2T_00_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                             ifta80_e2t_00_hash_control, e2t_00_sbus_num_bnks);
    IFTA80_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                             ifta80_e2t_00_hash_control, e2t_00_htbl_bnk_config);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_00_hash_control));

    /* IFTA80_E2T_01 settings */
    sal_memset(&ifta80_e2t_01_hash_control,
        0, sizeof(ifta80_e2t_01_hash_control));
    pt_id = IFTA80_E2T_01_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_01_hash_control));
    IFTA80_E2T_01_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                             ifta80_e2t_01_hash_control, e2t_01_ft_enable);
    IFTA80_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                             ifta80_e2t_01_hash_control, e2t_01_htbl_bnk_config);
    IFTA80_E2T_01_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                            ifta80_e2t_01_hash_control, e2t_01_sbus_num_bnks);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_01_hash_control));

    /* IFTA80_E2T_02 settings */
    sal_memset(&ifta80_e2t_02_hash_control,
        0, sizeof(ifta80_e2t_02_hash_control));
    pt_id = IFTA80_E2T_02_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_02_hash_control));
    IFTA80_E2T_02_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                            ifta80_e2t_02_hash_control, e2t_02_ft_enable);
    IFTA80_E2T_02_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                            ifta80_e2t_02_hash_control, e2t_02_sbus_num_bnks);
    IFTA80_E2T_02_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                            ifta80_e2t_02_hash_control, e2t_02_htbl_bnk_config);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_02_hash_control));

    /* IFTA80_E2T_03 settings */
    sal_memset(&ifta80_e2t_03_hash_control,
        0, sizeof(ifta80_e2t_03_hash_control));
    pt_id = IFTA80_E2T_03_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_03_hash_control));
    IFTA80_E2T_03_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                             ifta80_e2t_03_hash_control, e2t_03_ft_enable);
    IFTA80_E2T_03_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                             ifta80_e2t_03_hash_control, e2t_03_htbl_bnk_config);
    IFTA80_E2T_03_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                            ifta80_e2t_03_hash_control, e2t_03_sbus_num_bnks);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_03_hash_control));

    /* IFTA80_E2T_01_SHARED_BANKS_CONTROL settings */
    sal_memset(&ifta80_e2t_01_shared_banks_control,
        0, sizeof(ifta80_e2t_01_shared_banks_control));
    pt_id = IFTA80_E2T_01_SHARED_BANKS_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_01_shared_banks_control));
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_BANK_BITMAP_START_OFFSETf_SET(
                             ifta80_e2t_01_shared_banks_control,
                             e2t_01_bank_bitmap_start_offset);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_01_shared_banks_control));

    /* IFTA80_E2T_02_SHARED_BANKS_CONTROL settings */
    sal_memset(&ifta80_e2t_02_shared_banks_control,
        0, sizeof(ifta80_e2t_02_shared_banks_control));
    pt_id = IFTA80_E2T_02_SHARED_BANKS_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_02_shared_banks_control));
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_BANK_BITMAP_START_OFFSETf_SET(
                             ifta80_e2t_02_shared_banks_control,
                             e2t_02_bank_bitmap_start_offset);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_02_shared_banks_control));

    /* IFTA80_E2T_03_SHARED_BANKS_CONTROL settings */
    sal_memset(&ifta80_e2t_03_shared_banks_control,
        0, sizeof(ifta80_e2t_03_shared_banks_control));
    pt_id = IFTA80_E2T_03_SHARED_BANKS_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_03_shared_banks_control));
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_BANK_BITMAP_START_OFFSETf_SET(
                             ifta80_e2t_03_shared_banks_control,
                             e2t_03_bank_bitmap_start_offset);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_03_shared_banks_control));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_hw_learn_init(int unit,
                                              bcmcth_mon_ft_control_t *entry)
{
    IFTA80_E2T_00_HASH_CONTROLm_t ifta80_e2t_00_hash_control;
    IFTA80_E2T_01_HASH_CONTROLm_t ifta80_e2t_01_hash_control;
    IFTA80_E2T_02_HASH_CONTROLm_t ifta80_e2t_02_hash_control;
    IFTA80_E2T_03_HASH_CONTROLm_t ifta80_e2t_03_hash_control;
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_t ifta80_e2t_01_shared_banks_control;
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_t ifta80_e2t_02_shared_banks_control;
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_t ifta80_e2t_03_shared_banks_control;
    uint8_t e2t_00_htbl_bnk_config, e2t_01_htbl_bnk_config;
    uint8_t e2t_02_htbl_bnk_config, e2t_03_htbl_bnk_config;
    uint8_t e2t_00_sbus_num_bnks, e2t_01_sbus_num_bnks;
    uint8_t e2t_02_sbus_num_bnks, e2t_03_sbus_num_bnks;
    uint8_t e2t_00_ft_enable, e2t_01_ft_enable;
    uint8_t e2t_02_ft_enable, e2t_03_ft_enable;
    uint8_t e2t_01_bank_bitmap_start_offset;
    uint8_t e2t_02_bank_bitmap_start_offset, e2t_03_bank_bitmap_start_offset;
    FT_GLOBAL_TABLE_CONFIGm_t ft_global_table_config;
    bcmltd_sid_t lt_id = MON_FLOWTRACKER_CONTROLt;
    bcmdrd_sid_t pt_id = 0;
    int index = 0;
    uint32_t max_flows = 0;
    /* In terms of number of double wide entries.*/
    const uint32_t size_of_one_tile = (16 * 1024);
    bool with_eapp = true;
    SHR_FUNC_ENTER(unit);

    if (entry->flowtracker == false) {
        SHR_EXIT();
    }

    /* Initialize driver structure with FT INFO LTs */
    if (FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry->hw_learn)) {
        with_eapp = false;
    }
    bcm56780_a0_cth_mon_flowtracker_hw_lt_info_init(unit, with_eapp);
    max_flows = entry->max_flows[0];

    /* Global flow threshold config */
    sal_memset(&ft_global_table_config, 0, sizeof(ft_global_table_config));
    pt_id = FT_GLOBAL_TABLE_CONFIGm;
    /*
     * Per pipe value only needs to be configured,
     * since the access type is duplicate for this table.
     */
    FT_GLOBAL_TABLE_CONFIGm_FLOW_THRESHOLDf_SET(ft_global_table_config,
                                                max_flows);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ft_global_table_config));

    /*
     * The number of IFTA80 E2T tiles that will be enabled is decided based
     * on flow threshold per pipe.
     * NOTE: flow threshold is in terms of double wide entries by default.
     * <=16K         -> 1 tile or 2 banks (IFTA80_E2T_00_B0/B1)
     * >16K && <=32K -> 2 tiles or 4 banks (IFTA80_E2T_00/01_B0/B1)
     * >32K && <=48K -> 3 tiles or 6 banks (IFTA80_E2T_00/01/02_B0/B1)
     * >48K && <=64K -> 4 tiles or 8 banks (IFTA80_E2T_00/01/02/03_B0/B1)
     */
    if (max_flows <= size_of_one_tile) {
        e2t_00_sbus_num_bnks = 1;
        e2t_01_sbus_num_bnks = 1;
        e2t_02_sbus_num_bnks = 1;
        e2t_03_sbus_num_bnks = 1;
        e2t_00_htbl_bnk_config = 0x3;
        e2t_01_htbl_bnk_config = 0x3;
        e2t_02_htbl_bnk_config = 0x3;
        e2t_03_htbl_bnk_config = 0x3;
        e2t_00_ft_enable = 1;
        e2t_01_ft_enable = 0;
        e2t_02_ft_enable = 0;
        e2t_03_ft_enable = 0;
        e2t_01_bank_bitmap_start_offset = 0;
        e2t_02_bank_bitmap_start_offset = 0;
        e2t_03_bank_bitmap_start_offset = 0;
    } else if (max_flows <= (2 * size_of_one_tile)) {
        e2t_00_sbus_num_bnks = 2;
        e2t_01_sbus_num_bnks = 0;
        e2t_02_sbus_num_bnks = 1;
        e2t_03_sbus_num_bnks = 1;
        e2t_00_htbl_bnk_config = 0xf;
        e2t_01_htbl_bnk_config = 0x0;
        e2t_02_htbl_bnk_config = 0x3;
        e2t_03_htbl_bnk_config = 0x3;
        e2t_00_ft_enable = 1;
        e2t_01_ft_enable = 1;
        e2t_02_ft_enable = 0;
        e2t_03_ft_enable = 0;
        e2t_01_bank_bitmap_start_offset = 3;
        e2t_02_bank_bitmap_start_offset = 0;
        e2t_03_bank_bitmap_start_offset = 0;
    } else if (max_flows <= (3 * size_of_one_tile)) {
        e2t_00_sbus_num_bnks = 3;
        e2t_01_sbus_num_bnks = 0;
        e2t_02_sbus_num_bnks = 0;
        e2t_03_sbus_num_bnks = 1;
        e2t_00_htbl_bnk_config = 0x3f;
        e2t_01_htbl_bnk_config = 0x0;
        e2t_02_htbl_bnk_config = 0x0;
        e2t_03_htbl_bnk_config = 0x3;
        e2t_00_ft_enable = 1;
        e2t_01_ft_enable = 1;
        e2t_02_ft_enable = 1;
        e2t_03_ft_enable = 0;
        e2t_01_bank_bitmap_start_offset = 3;
        e2t_02_bank_bitmap_start_offset = 5;
        e2t_03_bank_bitmap_start_offset = 0;
    } else if (max_flows <= (4 * size_of_one_tile)) {
        e2t_00_sbus_num_bnks = 4;
        e2t_01_sbus_num_bnks = 0;
        e2t_02_sbus_num_bnks = 0;
        e2t_03_sbus_num_bnks = 0;
        e2t_00_htbl_bnk_config = 0xff;
        e2t_01_htbl_bnk_config = 0x0;
        e2t_02_htbl_bnk_config = 0x0;
        e2t_03_htbl_bnk_config = 0x0;
        e2t_00_ft_enable = 1;
        e2t_01_ft_enable = 1;
        e2t_02_ft_enable = 1;
        e2t_03_ft_enable = 1;
        e2t_01_bank_bitmap_start_offset = 3;
        e2t_02_bank_bitmap_start_offset = 5;
        e2t_03_bank_bitmap_start_offset = 7;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* IFTA80_E2T_00 enable settings */
    sal_memset(&ifta80_e2t_00_hash_control,
        0, sizeof(ifta80_e2t_00_hash_control));
    pt_id = IFTA80_E2T_00_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_00_hash_control));
    IFTA80_E2T_00_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                             ifta80_e2t_00_hash_control, e2t_00_ft_enable);
    IFTA80_E2T_00_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                             ifta80_e2t_00_hash_control, e2t_00_sbus_num_bnks);
    IFTA80_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                             ifta80_e2t_00_hash_control, e2t_00_htbl_bnk_config);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_00_hash_control));

    /* IFTA80_E2T_01 enable settings */
    sal_memset(&ifta80_e2t_01_hash_control,
        0, sizeof(ifta80_e2t_01_hash_control));
    pt_id = IFTA80_E2T_01_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_01_hash_control));
    IFTA80_E2T_01_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                             ifta80_e2t_01_hash_control, e2t_01_ft_enable);
    IFTA80_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                             ifta80_e2t_01_hash_control, e2t_01_htbl_bnk_config);
    IFTA80_E2T_01_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                            ifta80_e2t_01_hash_control, e2t_01_sbus_num_bnks);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_01_hash_control));

    /* IFTA80_E2T_02 enable settings */
    sal_memset(&ifta80_e2t_02_hash_control,
        0, sizeof(ifta80_e2t_02_hash_control));
    pt_id = IFTA80_E2T_02_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_02_hash_control));
    IFTA80_E2T_02_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                            ifta80_e2t_02_hash_control, e2t_02_ft_enable);
    IFTA80_E2T_02_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                            ifta80_e2t_02_hash_control, e2t_02_sbus_num_bnks);
    IFTA80_E2T_02_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                            ifta80_e2t_02_hash_control, e2t_02_htbl_bnk_config);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_02_hash_control));

    /* IFTA80_E2T_03 enable settings */
    sal_memset(&ifta80_e2t_03_hash_control,
        0, sizeof(ifta80_e2t_03_hash_control));
    pt_id = IFTA80_E2T_03_HASH_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_03_hash_control));
    IFTA80_E2T_03_HASH_CONTROLm_FLOW_TRACKER_ENABLEf_SET(
                             ifta80_e2t_03_hash_control, e2t_03_ft_enable);
    IFTA80_E2T_03_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(
                             ifta80_e2t_03_hash_control, e2t_03_htbl_bnk_config);
    IFTA80_E2T_03_HASH_CONTROLm_SBUS_NUM_BANKSf_SET(
                            ifta80_e2t_03_hash_control, e2t_03_sbus_num_bnks);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_03_hash_control));

    /* IFTA80_E2T_01_SHARED_BANKS_CONTROL enable settings */
    sal_memset(&ifta80_e2t_01_shared_banks_control,
        0, sizeof(ifta80_e2t_01_shared_banks_control));
    pt_id = IFTA80_E2T_01_SHARED_BANKS_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_01_shared_banks_control));
    IFTA80_E2T_01_SHARED_BANKS_CONTROLm_BANK_BITMAP_START_OFFSETf_SET(
                             ifta80_e2t_01_shared_banks_control,
                             e2t_01_bank_bitmap_start_offset);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_01_shared_banks_control));

    /* IFTA80_E2T_02_SHARED_BANKS_CONTROL enable settings */
    sal_memset(&ifta80_e2t_02_shared_banks_control,
        0, sizeof(ifta80_e2t_02_shared_banks_control));
    pt_id = IFTA80_E2T_02_SHARED_BANKS_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_02_shared_banks_control));
    IFTA80_E2T_02_SHARED_BANKS_CONTROLm_BANK_BITMAP_START_OFFSETf_SET(
                             ifta80_e2t_02_shared_banks_control,
                             e2t_02_bank_bitmap_start_offset);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_02_shared_banks_control));

    /* IFTA80_E2T_03_SHARED_BANKS_CONTROL enable settings */
    sal_memset(&ifta80_e2t_03_shared_banks_control,
        0, sizeof(ifta80_e2t_03_shared_banks_control));
    pt_id = IFTA80_E2T_03_SHARED_BANKS_CONTROLm;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iread(unit, lt_id, pt_id,
                             index, &ifta80_e2t_03_shared_banks_control));
    IFTA80_E2T_03_SHARED_BANKS_CONTROLm_BANK_BITMAP_START_OFFSETf_SET(
                             ifta80_e2t_03_shared_banks_control,
                             e2t_03_bank_bitmap_start_offset);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_iwrite(unit, lt_id, pt_id,
                              index, &ifta80_e2t_03_shared_banks_control));
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_mon_flowtracker_hw_learn_max_grps_get(
        int unit,
        uint32_t *size)
{
    bcmdrd_sid_t pt_id;
    bcmdrd_sym_info_t sinfo;

    SHR_FUNC_ENTER(unit);

    pt_id = FT_GROUP_TABLE_CONFIGm;

    /* Check if max_groups is within max groups possible for HW learning */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, pt_id, &sinfo));
    *size = (sinfo.index_max) - (sinfo.index_min) + 1;
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_cth_mon_flowtracker_rxpmd_flex_fids_get(int unit, int max_fids,
                                                    int *num_fids, int *fids,
                                                    int *uc_fids)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    /* bcm57880 requires 1 field to be extracted from Rx PMD flex data. */
    if (max_fids < 1) {
        /* Not enough memory allocted by caller. */
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    *num_fids = 0;
    rv = bcmpkt_rxpmd_flex_field_id_get("I2E_CLASS_ID_15_0", &fids[*num_fids]);
    if (SHR_SUCCESS(rv)) {
        uc_fids[*num_fids] = MCS_FT_RXPMD_FLEX_FIELD_I2E_CLASS_ID_15_0;
        (*num_fids)++;
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

static inline
void bcm56780_a0_cth_mon_flowtracker_ft_command_mode_set(
              bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode,
              FT_COMMANDm_t *ft_command)
{
    /*
     * From regsfile,
     * EXACT_MATCH_SINGLE => {
     *    VALUE => 0 ,
     *    DESC => "HASH_TABLE_SINGLE" ,
     * },
     * EXACT_MATCH_DOUBLE => {
     *    VALUE => 1 ,
     *    DESC => "HASH_TABLE_DOUBLE" ,
     * },
     * EXACT_MATCH_QUAD => {
     *    VALUE => 2 ,
     *    DESC => "HASH_TABLE_QUAD" ,
     * },
     */
    if (mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_SINGLE) {
        FT_COMMANDm_INDEX_MODEf_SET(*ft_command, 0);
    } else if (mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_DOUBLE) {
        FT_COMMANDm_INDEX_MODEf_SET(*ft_command, 1);
    } else if (mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_QUAD) {
        FT_COMMANDm_INDEX_MODEf_SET(*ft_command, 2);
    }
}

static int
bcm56780_a0_cth_mon_flowtracker_ft_delete_command_set(
                        int unit,
                        bcmcth_mon_ft_hw_learn_flow_action_t *entry)
{
    int index = 0;
    bcmltd_sid_t lt_id = 0;
    bcmdrd_sid_t pt_id = 0;
    uint32_t *buf_ptr = NULL;
    FT_COMMANDm_t ft_command = {{0}};

    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt;

    pt_id = FT_COMMANDm;
    buf_ptr = (uint32_t *)(&ft_command);

    /*
     * From regsfile,
     * DELETE_HASH_TABLE_ENTRY => {
     *        VALUE => 1 ,
     *        DESC => "Delete Hash Table Entry.  Modifies Hash Table and FT_GROUP_TABLE_CNT.FLOW_CNT" ,
     *     },
     */
    FT_COMMANDm_CMDf_SET(ft_command, 1);

    bcm56780_a0_cth_mon_flowtracker_ft_command_mode_set(entry->mode,
                                                        &ft_command);

    FT_COMMANDm_GROUP_IDf_SET(ft_command, entry->mon_flowtracker_group_id);

    FT_COMMANDm_INDEXf_SET(ft_command, entry->exact_match_idx);

    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite_pipe(unit, lt_id, pt_id,
                                   entry->pipe, index, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static void
bcm56780_a0_cth_mon_flowtracker_ft_modify_command_read_mem_info(
                        int unit,
                        bcmcth_mon_ft_hw_learn_flow_action_t *entry,
                        bcmdrd_sid_t *pt_id,
                        int *index)
{
    bcmdrd_sid_t pt_ids_single[]= {
        IFTA80_E2T_00_B0_SINGLEm,
        IFTA80_E2T_00_B1_SINGLEm,
        IFTA80_E2T_01_B0_SINGLEm,
        IFTA80_E2T_01_B1_SINGLEm,
        IFTA80_E2T_02_B0_SINGLEm,
        IFTA80_E2T_02_B1_SINGLEm,
        IFTA80_E2T_03_B0_SINGLEm,
        IFTA80_E2T_03_B1_SINGLEm,
    };
    bcmdrd_sid_t pt_ids_double[]= {
        IFTA80_E2T_00_B0_DOUBLEm,
        IFTA80_E2T_00_B1_DOUBLEm,
        IFTA80_E2T_01_B0_DOUBLEm,
        IFTA80_E2T_01_B1_DOUBLEm,
        IFTA80_E2T_02_B0_DOUBLEm,
        IFTA80_E2T_02_B1_DOUBLEm,
        IFTA80_E2T_03_B0_DOUBLEm,
        IFTA80_E2T_03_B1_DOUBLEm,
    };
    bcmdrd_sid_t pt_ids_quad[]= {
        IFTA80_E2T_00_B0_QUADm,
        IFTA80_E2T_00_B1_QUADm,
        IFTA80_E2T_01_B0_QUADm,
        IFTA80_E2T_01_B1_QUADm,
        IFTA80_E2T_02_B0_QUADm,
        IFTA80_E2T_02_B1_QUADm,
        IFTA80_E2T_03_B0_QUADm,
        IFTA80_E2T_03_B1_QUADm,
    };
    int num_entries_per_single_bank = (16 * 1024);
    int num_entries_per_double_bank = (8 * 1024);
    int num_entries_per_quad_bank = (4 * 1024);
    int bank_id = 0;

    if (entry->mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_SINGLE) {
        bank_id = (entry->exact_match_idx / num_entries_per_single_bank);
        *pt_id = pt_ids_single[bank_id];
        *index = (entry->exact_match_idx % num_entries_per_single_bank);
    } else if (entry->mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_DOUBLE) {
        bank_id = (entry->exact_match_idx / num_entries_per_double_bank);
        *pt_id = pt_ids_double[bank_id];
        *index = (entry->exact_match_idx % num_entries_per_double_bank);
    } else if (entry->mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_QUAD) {
        bank_id = (entry->exact_match_idx / num_entries_per_quad_bank);
        *pt_id = pt_ids_quad[bank_id];
        *index = (entry->exact_match_idx % num_entries_per_quad_bank);
    }
}

static int
bcm56780_a0_cth_mon_flowtracker_ft_modify_command_data_buf_prepare(
                        int unit,
                        bcmcth_mon_ft_hw_learn_flow_action_t *entry,
                        bcmcth_mon_ft_group_t *grp_entry,
                        uint32_t *buf)
{
    int index = 0;
    bcmltd_sid_t lt_id = 0;
    bcmdrd_sid_t pt_id = 0;
    uint32_t dt_em_ft_pdd_template_id;
    uint32_t total_width_in_bits = 0, val = 0;
    int i, j;
    uint16_t aset_widths[FLOWTRACKER_ACTIONS_MAX];
    ing_pdd_template_partition_info_t *pdd_template_partition_info = NULL;
    
    uint8_t  data_format = FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_ONLY;
    uint8_t num_actions = 0;
    uint16_t end_offset = 0, curr_offset = 0;
    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt;

    bcm56780_a0_cth_mon_flowtracker_ft_modify_command_read_mem_info(
            unit, entry, &pt_id, &index);
    /* Read the entry at given index */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iread_pipe(unit, lt_id, pt_id,
                                   entry->pipe, index, buf));

    /* Lookup EM_FT_GROUP to get the PDD template ID */
    SHR_IF_ERR_EXIT(bcm56780_a0_cth_mon_dt_em_ft_grp_template_lt_lookup(
                unit,
                grp_entry,
                &dt_em_ft_pdd_template_id));
    SHR_ALLOC(pdd_template_partition_info,
            sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_ACTIONS_MAX,
            "bcmcthMonFtEmPddTempPartInfo");
    sal_memset(pdd_template_partition_info, 0,
            sizeof(ing_pdd_template_partition_info_t) * FLOWTRACKER_ACTIONS_MAX);
    /*
     * Use PDD template ID to lookup PDD template partition info LT and get the
     * offsets and widths for actions configured in the group.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_cth_mon_ing_pdd_template_partition_info_lt_lookup
         (unit,
          &(grp_entry->oper),
          dt_em_ft_pdd_template_id,
          pdd_template_partition_info,
          &num_actions));
    if (grp_entry->oper !=
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
        SHR_EXIT();
    }

    /* Find action total width in bits */
    for (i = 0; i < num_actions; i++) {
        aset_widths[i] = 0;
        for (j = 0; j < pdd_template_partition_info[i].num_action_info; j++) {
            aset_widths[i] +=
                pdd_template_partition_info[i].action_width[j];
        }
        total_width_in_bits += aset_widths[i];
    }

    if (entry->mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_SINGLE) {
        end_offset = 120;
    } else if (entry->mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_DOUBLE) {
        end_offset = 240;
    } else if (entry->mode ==
            BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_EXACT_MATCH_IDX_MODE_T_T_QUAD) {
        end_offset = 480;
    }


    if (data_format ==
            FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_PLUS_PDD_IDX_PLUS_SBR_IDX) {
        /* 9 bits taken up by PDD idx + SBR idx */
        curr_offset = end_offset - 9 - total_width_in_bits;
    } else if (data_format == FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_ONLY) {
        curr_offset = end_offset - total_width_in_bits;
    }

    /*
     * Loop through the PDD template partition info for actions
     * and set the actions in the buffer.
     */
    for (i = 0; i < num_actions; i++) {
        val = bcm56780_a0_cth_mon_flowtracker_get_action_val(
                grp_entry->num_actions,
                entry->actions,
                pdd_template_partition_info[i].action);
        bcmdrd_field_set(buf, curr_offset,
                curr_offset + aset_widths[i] -1,
                &val);
        curr_offset = curr_offset + aset_widths[i];
    }
exit:
    SHR_FREE(pdd_template_partition_info);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_ft_modify_command_set(
                        int unit,
                        bcmcth_mon_ft_hw_learn_flow_action_t *entry,
                        bcmcth_mon_ft_group_t *grp_entry)
{
    int index = 0;
    bcmltd_sid_t lt_id = 0;
    bcmdrd_sid_t pt_id = 0;
    uint32_t *buf_ptr = NULL;
    /* 15 * 32 bits = 480 bits - Max QUAD entry size */
    uint32_t ft_command_data[15] = {0};
    FT_COMMANDm_t ft_command = {{0}};

    SHR_FUNC_ENTER(unit);
    lt_id = MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt;

    buf_ptr = (uint32_t *)(&ft_command);

   SHR_IF_ERR_VERBOSE_EXIT(
        bcm56780_a0_cth_mon_flowtracker_ft_modify_command_data_buf_prepare(
                unit,
                entry,
                grp_entry,
                ft_command_data));
    /*
     * From regsfile,
     * MODIFY_HASH_TABLE_ENTRY => {
     *    VALUE => 2 ,
     *    DESC => "Modify Hash Table Entry.  Modifies Hash Table.
     *             CAN ONLY MODIFY ASSOC_DATA FIELDS,
     *             CAN NOT MODIFY THE KEY FIELDS."
     * },
     */
    FT_COMMANDm_CMDf_SET(ft_command, 2);

    FT_COMMANDm_INDEXf_SET(ft_command, entry->exact_match_idx);

    FT_COMMANDm_GROUP_IDf_SET(ft_command, entry->mon_flowtracker_group_id);

    bcm56780_a0_cth_mon_flowtracker_ft_command_mode_set(entry->mode,
                                                        &ft_command);
    pt_id = FT_COMMAND_DATAm;
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite_pipe(unit, lt_id, pt_id,
                                   entry->pipe, index, ft_command_data));
    pt_id = FT_COMMANDm;
    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_iwrite_pipe(unit, lt_id, pt_id,
                                   entry->pipe, index, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_flowtracker_ft_command_set(
                        int unit,
                        bcmcth_mon_ft_hw_learn_flow_action_t *entry,
                        bcmcth_mon_ft_group_t *grp_entry)
{
    SHR_FUNC_ENTER(unit);
    if (entry->cmd == BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_CMD_T_T_DELETE) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm56780_a0_cth_mon_flowtracker_ft_delete_command_set(
                    unit,
                    entry));
    } else if (entry->cmd == BCMLTD_COMMON_FLOWTRACKER_HW_LEARN_FLOW_CMD_T_T_MODIFY) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm56780_a0_cth_mon_flowtracker_ft_modify_command_set(
                    unit,
                    entry,
                    grp_entry));
    }
exit:
    SHR_FUNC_EXIT();
}


static bcmcth_mon_flowtracker_drv_t bcm56780_a0_flowtracker_drv =
{
    .flex_ctr_action_profile_info_lt = NULL,
    .flex_ctr_action_profile_lt = NULL,
    .ing_grp_template_info_lt = NULL,
    .ing_grp_template_partition_info_lt = NULL,
    .ing_pdd_template_partition_info_lt = NULL,
    .dt_em_ft_grp_template_lt = NULL,
    .ft_control_init = &bcm56780_a0_cth_mon_flowtracker_control_msg_init,
    .ft_grp_init = &bcm56780_a0_cth_mon_flowtracker_grp_flex_em_msg_init,
    .ft_em_uft_init = &bcm56780_a0_cth_mon_flowtracker_em_uft_msg_init,
    .ft_hw_learn_opt_msg_init =
                      &bcm56780_a0_cth_mon_flowtracker_hw_learn_opt_msg_init,
    .ft_grp_hw_learn_config =
                      &bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config,
    .ft_hw_learn_init = &bcm56780_a0_cth_mon_flowtracker_hw_learn_init,
    .ft_hw_learn_max_grps_get =
                      bcm56780_a0_cth_mon_flowtracker_hw_learn_max_grps_get,
    .ft_hw_learn_flow_limit_update =
                      bcm56780_a0_cth_mon_flowtracker_grp_flow_limit_update,
    .ft_rxpmd_flex_fids_get =
                      &bcm56780_a0_cth_mon_flowtracker_rxpmd_flex_fids_get,
    .ft_grp_hw_learn_config_wo_eapp =
                 &bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_config_wo_eapp,
    .ft_grp_hw_learn_deinit =
                &bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_deinit,
    .ft_grp_hw_learn_flow_cnt_get =
          &bcm56780_a0_cth_mon_flowtracker_grp_hw_learn_flow_cnt_get,
    /*
     * Updating threshold will mean max flows change. So we have to modify
     * IFTA80 E2T Tiles setting as well. So its good to call hw learn init
     * itself.
     */
    .ft_hw_learn_update_glb_flow_threshold =
            &bcm56780_a0_cth_mon_flowtracker_hw_learn_init,
    .ft_hw_learn_deinit =
            &bcm56780_a0_cth_mon_flowtracker_hw_learn_deinit,
    .ft_command_set = &bcm56780_a0_cth_mon_flowtracker_ft_command_set,
    .num_pipes = 2

};


static int
bcm56780_a0_cth_mon_mirror_iupdate(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bool trace,
    uint32_t container,
    bcmdrd_pbmp_t mask)
{
    int index = 0;
    IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_0m_t trace_buf;
    IPOST_TRACE_DROP_EVENT_DROP_EVENT_MIRROR_CONTAINER_MASK_0m_t drop_buf;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t *buf_ptr = NULL;
    bcmdrd_sid_t trace_pt_sid[] = {
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_0m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_1m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_2m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_3m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_4m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_5m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_6m,
        IPOST_TRACE_DROP_EVENT_TRACE_EVENT_MIRROR_CONTAINER_MASK_7m
    };

    SHR_FUNC_ENTER(unit);

    /* Write to memory */
    if (trace) {
        if (container >= BCMMON_MIRROR_CONTAINER_MAX) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        buf_ptr = (uint32_t *)(&trace_buf);
        pt_id = trace_pt_sid[container];
    } else {
        buf_ptr = (uint32_t *)(&drop_buf);
        pt_id = IPOST_TRACE_DROP_EVENT_DROP_EVENT_MIRROR_CONTAINER_MASK_0m;
    }
    pt_fid = MASKf;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_read(unit, trans_id, lt_id, pt_id, index, buf_ptr));

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit,
                            pt_id,
                            buf_ptr,
                            pt_fid,
                            (uint32_t *) (&mask)));

    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_write(unit, trans_id, lt_id, pt_id, index, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_mon_copy2cpu_iupdate(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bool trace,
    bcmdrd_pbmp_t mask)
{
    int index = 0;
    IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COPYTOCPU_MASKm_t trace_buf;
    IPOST_TRACE_DROP_EVENT_DROP_EVENT_COPYTOCPU_MASKm_t drop_buf;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Write to memory */
    if (trace) {
        buf_ptr = (uint32_t *)(&trace_buf);
        pt_id = IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COPYTOCPU_MASKm;
    } else {
        buf_ptr = (uint32_t *)(&drop_buf);
        pt_id = IPOST_TRACE_DROP_EVENT_DROP_EVENT_COPYTOCPU_MASKm;
    }
    pt_fid = MASKf;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_read(unit, trans_id, lt_id, pt_id, index, buf_ptr));

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit,
                            pt_id,
                            buf_ptr,
                            pt_fid,
                            (uint32_t *) (&mask)));

    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_write(unit, trans_id, lt_id, pt_id, index, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_loopback_iupdate(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmdrd_pbmp_t mask)
{
    int index = 0;
    IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_LOOPBACK_PORT_DROP_MASKm_t drop_buf;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Write to memory */
    buf_ptr = (uint32_t *)(&drop_buf);
    pt_id = IPOST_MEMBERSHIP_CHECK_BLOCK_MASK_LOOPBACK_PORT_DROP_MASKm;
    pt_fid = MASKf;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_read(unit, trans_id, lt_id, pt_id, index, buf_ptr));

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit,
                            pt_id,
                            buf_ptr,
                            pt_fid,
                            (uint32_t *) (&mask)));

    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_write(unit, trans_id, lt_id, pt_id, index, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_trace_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_trace_op_t op,
    bcmcth_mon_trace_t *entry)
{
    bool trace = TRUE, iupdate;
    bcmdrd_pbmp_t mask;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Copy to cpu action */
    iupdate = FALSE;
    switch (op) {
        case BCMCTH_MON_ING_TRACE_CPU_OP_SET:
            iupdate = TRUE;
            if (entry->cpu) {
                BCMDRD_PBMP_PORT_ADD(mon_trace_state[unit]->ievent_cpu,
                                     entry->ievent);
            } else {
                BCMDRD_PBMP_PORT_REMOVE(mon_trace_state[unit]->ievent_cpu,
                                        entry->ievent);
            }
            break;
        case BCMCTH_MON_ING_TRACE_CPU_OP_DEL:
            iupdate = TRUE;
            BCMDRD_PBMP_PORT_REMOVE(mon_trace_state[unit]->ievent_cpu,
                                    entry->ievent);
            break;
        default:
            break;
    }

    /* Ingress copy2cpu update */
    if (iupdate) {
        BCMDRD_PBMP_CLEAR(mask);
        BCMDRD_PBMP_OR(mask, mon_trace_state[unit]->ievent_cpu);
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_mon_copy2cpu_iupdate(unit, trans_id, lt_id,
                                                  trace,
                                                  mask));
    }

    /* Mirror action */
    iupdate = FALSE;
    for (idx = 0; idx < BCMMON_MIRROR_CONTAINER_MAX; idx++) {
        /* Only update input's container(s). */
        if (SHR_BITGET(entry->fbmp_mir, idx) == 0) {
            continue;
        }
        switch (op) {
        case BCMCTH_MON_ING_TRACE_MIRR_OP_SET:
            iupdate = TRUE;
            if (entry->mirr[idx]) {
                BCMDRD_PBMP_PORT_ADD(mon_trace_state[unit]->ievent_mirr[idx],
                                     entry->ievent);
            } else {
                BCMDRD_PBMP_PORT_REMOVE(mon_trace_state[unit]->ievent_mirr[idx],
                                        entry->ievent);
            }
            break;
        case BCMCTH_MON_ING_TRACE_MIRR_OP_DEL:
            iupdate = TRUE;
            BCMDRD_PBMP_PORT_REMOVE(mon_trace_state[unit]->ievent_mirr[idx],
                                    entry->ievent);
            break;
        default:
            break;
        }

        /* Ingress mirror update */
        if (iupdate) {
            BCMDRD_PBMP_CLEAR(mask);
            BCMDRD_PBMP_OR(mask, mon_trace_state[unit]->ievent_mirr[idx]);
            SHR_IF_ERR_EXIT
                (bcm56780_a0_cth_mon_mirror_iupdate(unit, trans_id, lt_id,
                                                    trace,
                                                    idx,
                                                    mask));
        }
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_cth_mon_drop_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_drop_op_t op,
    bcmcth_mon_drop_t *entry)
{
    bool trace = FALSE, iupdate;
    bcmdrd_pbmp_t mask;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Copy to cpu action */
    iupdate = FALSE;
    switch (op) {
        case BCMCTH_MON_ING_DROP_CPU_OP_SET:
            iupdate = TRUE;
            if (entry->cpu) {
                BCMDRD_PBMP_PORT_ADD(mon_drop_state[unit]->ievent_cpu,
                                     entry->ievent);
            } else {
                BCMDRD_PBMP_PORT_REMOVE(mon_drop_state[unit]->ievent_cpu,
                                        entry->ievent);
            }
            break;
        case BCMCTH_MON_ING_DROP_CPU_OP_DEL:
            iupdate = TRUE;
            BCMDRD_PBMP_PORT_REMOVE(mon_drop_state[unit]->ievent_cpu,
                                    entry->ievent);
            break;
        default:
            break;
    }

    /* Ingress copy2cpu update */
    if (iupdate) {
        BCMDRD_PBMP_CLEAR(mask);
        BCMDRD_PBMP_OR(mask, mon_drop_state[unit]->ievent_cpu);
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_mon_copy2cpu_iupdate(unit, trans_id, lt_id,
                                                  trace,
                                                  mask));
    }

    /* Mirror action */
    iupdate = FALSE;
    switch (op) {
        case BCMCTH_MON_ING_DROP_MIRR_OP_SET:
            iupdate = TRUE;
            if (entry->mirr) {
                BCMDRD_PBMP_PORT_ADD(mon_drop_state[unit]->ievent_mirr,
                                     entry->ievent);
            } else {
                BCMDRD_PBMP_PORT_REMOVE(mon_drop_state[unit]->ievent_mirr,
                                        entry->ievent);
            }
            break;
        case BCMCTH_MON_ING_DROP_MIRR_OP_DEL:
            iupdate = TRUE;
            BCMDRD_PBMP_PORT_REMOVE(mon_drop_state[unit]->ievent_mirr,
                                    entry->ievent);
            break;
        default:
            break;
    }

    /* Ingress mirror action update */
    if (iupdate) {
        BCMDRD_PBMP_CLEAR(mask);
        BCMDRD_PBMP_OR(mask, mon_drop_state[unit]->ievent_mirr);
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_mon_mirror_iupdate(unit, trans_id, lt_id,
                                                trace,
                                                0,
                                                mask));
    }

    /* loopback action */
    iupdate = FALSE;
    switch (op) {
        case BCMCTH_MON_ING_DROP_LOOPBACK_OP_SET:
            iupdate = TRUE;
            if (entry->loopback) {
                BCMDRD_PBMP_PORT_REMOVE(mon_drop_state[unit]->ievent_no_lp,
                                        entry->ievent);
            } else {
                BCMDRD_PBMP_PORT_ADD(mon_drop_state[unit]->ievent_no_lp,
                                     entry->ievent);
            }
            break;
        case BCMCTH_MON_ING_DROP_LOOPBACK_OP_DEL:
            iupdate = TRUE;
            BCMDRD_PBMP_PORT_REMOVE(mon_drop_state[unit]->ievent_no_lp,
                                    entry->ievent);
            break;
        default:
            break;
    }

    /* Ingress mirror action update */
    if (iupdate) {
        BCMDRD_PBMP_CLEAR(mask);
        BCMDRD_PBMP_OR(mask, mon_drop_state[unit]->ievent_no_lp);
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_mon_loopback_iupdate(unit,
                                                  trans_id,
                                                  lt_id,
                                                  mask));
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_cth_mon_collector_ipv4_vlan_info_copy(
    int unit,
    bcmcth_mon_collector_ipv4_t *collector,
    shr_util_pack_l2_header_t   *l2)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(collector, SHR_E_PARAM);
    SHR_NULL_CHECK(l2, SHR_E_PARAM);
    if (collector->tag_type != BCM56780_A0_LRD_VLAN_TAG_TYPE_T_T_UNTAGGED) {
        sal_memcpy(&l2->outer_vlan_tag, &collector->outer_vlan_tag,
                   sizeof(l2->outer_vlan_tag));

        if (collector->tag_type ==
            BCM56780_A0_LRD_VLAN_TAG_TYPE_T_T_DOUBLE_TAGGED) {
            sal_memcpy(&l2->inner_vlan_tag, &collector->inner_vlan_tag,
                       sizeof(l2->inner_vlan_tag));
        }
    }
exit:
        SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_collector_ipv6_vlan_info_copy(
    int unit,
    bcmcth_mon_collector_ipv6_t *collector,
    shr_util_pack_l2_header_t   *l2)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(collector, SHR_E_PARAM);
    SHR_NULL_CHECK(l2, SHR_E_PARAM);
    if (collector->tag_type != BCM56780_A0_LRD_VLAN_TAG_TYPE_T_T_UNTAGGED) {
        sal_memcpy(&l2->outer_vlan_tag, &collector->outer_vlan_tag,
                   sizeof(l2->outer_vlan_tag));

        if (collector->tag_type ==
            BCM56780_A0_LRD_VLAN_TAG_TYPE_T_T_DOUBLE_TAGGED) {
            sal_memcpy(&l2->inner_vlan_tag, &collector->inner_vlan_tag,
                       sizeof(l2->inner_vlan_tag));
        }
    }
exit:
        SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_telemetry_control_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_control_op_t op,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *output_fields)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Streaming telemetry chip specific handling if any in future*/
exit:
        SHR_FUNC_EXIT();

}

static int
bcm56780_a0_cth_mon_telemetry_instance_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_instance_op_t op,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);


    /* Streaming telemetry chip specific handling if any in future*/
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_cth_mon_int_fifo_init(int unit)
{
    bcmbd_pt_dyn_info_t pt_info;
    EGR_METADATA_CONFIGr_t md_config;
    EGR_METADATA_BITP_PROFILEm_t bitp_profile;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = -1;

    /* Set the granularity to 2 for 64B extraction. This will need to be changed
     * to be user configurable, once it is supported.
     */
    EGR_METADATA_CONFIGr_CLR(md_config);
    EGR_METADATA_CONFIGr_GRANf_SET(md_config, 2);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, EGR_METADATA_CONFIGr, &pt_info,
                                    NULL, NULL,
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)(&md_config), 0,
                                    MON_INBAND_TELEMETRY_CONTROLt,
                                    BCMPTM_DIRECT_TRANS_ID,
                                    NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));


    /* Set the CAPTURE_EN field in EGR_METADATA_BITP_PROFILE */
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    /* Currently only index-0 is used, set that index CAPTURE_EN=1 */
    pt_info.index = 0;
    pt_info.tbl_inst = -1;

    EGR_METADATA_BITP_PROFILEm_CLR(bitp_profile);
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, EGR_METADATA_BITP_PROFILEm);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, EGR_METADATA_BITP_PROFILEm,
                                    &pt_info, NULL, NULL,
                                    BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, 0,
                                    BCMPTM_DIRECT_TRANS_ID,
                                    NULL, NULL,
                                    (uint32_t *)(&bitp_profile),
                                    &rsp_ltid, &rsp_flags));

    EGR_METADATA_BITP_PROFILEm_CAPTURE_ENf_SET(bitp_profile, 1);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, EGR_METADATA_BITP_PROFILEm,
                                    &pt_info, NULL, NULL,
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)(&bitp_profile), 0,
                                    MON_INBAND_TELEMETRY_CONTROLt,
                                    BCMPTM_DIRECT_TRANS_ID,
                                    NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_deinit(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56780_a0_cth_mon_init(int unit, bool warm)
{
    const bcmlrd_map_t *map1 = NULL, *map2 = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    int rv1, rv2;
    int ltid = 0;

    SHR_FUNC_ENTER(unit);

    mon_trace_state[unit] = NULL;
    mon_redirect_trace_state[unit] = NULL;
    mon_drop_state[unit] = NULL;

    rv1 = bcmlrd_map_get(unit, MON_ING_TRACE_EVENTt, &map1);
    if (SHR_SUCCESS(rv1) && map1) {

        /* Allocate trace event */
        ha_req_size = sizeof(bcmmon_trace_state_t);
        ha_alloc_size = ha_req_size;
        mon_trace_state[unit] =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_MON_COMP_ID, BCMMON_TRACE_SUB_COMP_ID,
                             "monTraceState",
                             &ha_alloc_size);

        SHR_NULL_CHECK(mon_trace_state[unit], SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(mon_trace_state[unit], 0, ha_alloc_size);
        }

    }

    rv1 = bcmlrd_map_get(unit, MON_EGR_REDIRECT_TRACE_EVENTt, &map1);
    if (SHR_SUCCESS(rv1) && map1) {

        /* Allocate trace event */
        ha_req_size = sizeof(bcmmon_redirect_trace_state_t);
        ha_alloc_size = ha_req_size;
        mon_redirect_trace_state[unit] =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_MON_COMP_ID, BCMMON_REDIRECT_TRACE_SUB_COMP_ID,
                             "monRecircleTraceState",
                             &ha_alloc_size);

        SHR_NULL_CHECK(mon_redirect_trace_state[unit], SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(mon_redirect_trace_state[unit], 0, ha_alloc_size);
        }

    }

    rv1 = bcmlrd_map_get(unit, MON_ING_DROP_EVENTt, &map1);
    if (SHR_SUCCESS(rv1) && map1) {

        /* Allocate drop event */
        ha_req_size = sizeof(bcmmon_drop_state_t);
        ha_alloc_size = ha_req_size;
        mon_drop_state[unit] =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_MON_COMP_ID, BCMMON_DROP_SUB_COMP_ID,
                             "monDropState",
                             &ha_alloc_size);

        SHR_NULL_CHECK(mon_drop_state[unit], SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(mon_drop_state[unit], 0, ha_alloc_size);
        }

    }

    rv1 = bcmlrd_map_get(unit, MON_INBAND_TELEMETRY_CONTROLt, &map1);
    rv2 = bcmlrd_map_get(unit, MON_INBAND_TELEMETRY_STATSt, &map2);
    if (SHR_SUCCESS(rv1) && map1 &&
        SHR_SUCCESS(rv2) && map2) {

        if (!warm) {
            /* Set the MD-FIFO width. */
            SHR_IF_ERR_VERBOSE_EXIT(bcm56780_a0_cth_mon_int_fifo_init(unit));
        }
    }

    rv1 = bcmlrd_map_get(unit, MON_FLOWTRACKER_CONTROLt, &map1);
    if (SHR_SUCCESS(rv1) && map1) {
        bcm56780_a0_flowtracker_drv.ft_control_init =
            &bcm56780_a0_cth_mon_flowtracker_control_msg_init;
        bcm56780_a0_flowtracker_drv.ft_grp_init =
            &bcm56780_a0_cth_mon_flowtracker_grp_flex_em_msg_init;
        bcm56780_a0_flowtracker_drv.ft_em_uft_init =
            &bcm56780_a0_cth_mon_flowtracker_em_uft_msg_init;
        rv1 = bcmlrd_map_get(unit, flex_ctr_action_profile_info_lt.ltid, &map1);
        if (SHR_SUCCESS(rv1) && map1) {
            bcm56780_a0_flowtracker_drv.flex_ctr_action_profile_info_lt =
                                            &flex_ctr_action_profile_info_lt;
        }
        rv1 = bcmlrd_map_get(unit, flex_ctr_action_profile_lt.ltid, &map1);
        if (SHR_SUCCESS(rv1) && map1) {
            bcm56780_a0_flowtracker_drv.flex_ctr_action_profile_lt =
                &flex_ctr_action_profile_lt;
        }
        /* FP related LTs */
        ltid = bcmlrd_table_name_to_idx(unit, ing_grp_template_info_lt.ltid);
        if (ltid != SHR_E_UNAVAIL) {
            bcm56780_a0_flowtracker_drv.ing_grp_template_info_lt =
                &ing_grp_template_info_lt;
        }
        ltid = bcmlrd_table_name_to_idx(unit,
               ing_grp_template_partition_info_lt.ltid);
        if (ltid != SHR_E_UNAVAIL) {
            bcm56780_a0_flowtracker_drv.ing_grp_template_partition_info_lt =
                &ing_grp_template_partition_info_lt;
        }
        ltid = bcmlrd_table_name_to_idx(unit,
               ing_pdd_template_partition_info_lt.ltid);
        if (ltid != SHR_E_UNAVAIL) {
            bcm56780_a0_flowtracker_drv.ing_pdd_template_partition_info_lt =
                &ing_pdd_template_partition_info_lt;
        }
        ltid = bcmlrd_table_name_to_idx(unit, dt_em_ft_grp_template_lt.lt_name);
        if (ltid != SHR_E_UNAVAIL) {
            bcm56780_a0_flowtracker_drv.dt_em_ft_grp_template_lt =
                &dt_em_ft_grp_template_lt;
        }
        rv1 = bcmlrd_map_get(unit, udf_policy_lt.ltid, &map1);
        if (SHR_SUCCESS(rv1) && map1) {
            bcm56780_a0_flowtracker_drv.udf_policy_lt =
                &udf_policy_lt;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (mon_trace_state[unit] != NULL) {
            shr_ha_mem_free(unit, mon_trace_state[unit]);
            mon_trace_state[unit] = NULL;
        }
        if (mon_drop_state[unit] != NULL) {
            shr_ha_mem_free(unit, mon_drop_state[unit]);
            mon_drop_state[unit] = NULL;
        }
        if (mon_redirect_trace_state[unit] != NULL) {
            shr_ha_mem_free(unit, mon_redirect_trace_state[unit]);
            mon_redirect_trace_state[unit] = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_redirect_trace_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmdrd_pbmp_t mask)
{
    EPOST_EGR_TRACE_BITMAP_RECIRC_CTRLr_t redirect_buf;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Write to memory */
    buf_ptr = (uint32_t *)(&redirect_buf);
    pt_id = EPOST_EGR_TRACE_BITMAP_RECIRC_CTRLr;
    pt_fid = EGR_TRACE_BITMAP_MASKf;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_pt_read(unit, trans_id, lt_id, pt_id, 0, buf_ptr));

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit,
                            pt_id,
                            buf_ptr,
                            pt_fid,
                            (uint32_t *) (&mask)));

    SHR_IF_ERR_EXIT
        (bcmcth_mon_pt_write(unit, trans_id, lt_id, pt_id, 0, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_mon_redirect_trace_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_redirect_trace_op_t op,
    bcmcth_mon_redirect_trace_t *entry)
{
    bool update;
    bcmdrd_pbmp_t mask;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Copy to cpu action */
    update = FALSE;
    switch (op) {
        case BCMCTH_MON_EGR_TRACE_REDIRECT_OP_SET:
            update = TRUE;
            if (entry->redirect) {
                BCMDRD_PBMP_PORT_ADD(
                    mon_redirect_trace_state[unit]->eevent_redirect,
                    entry->eevent);
            } else {
                BCMDRD_PBMP_PORT_REMOVE(
                    mon_redirect_trace_state[unit]->eevent_redirect,
                    entry->eevent);
            }
            break;
        case BCMCTH_MON_EGR_TRACE_REDIRECT_OP_DEL:
            update = TRUE;
            BCMDRD_PBMP_PORT_REMOVE(
                mon_redirect_trace_state[unit]->eevent_redirect,
                entry->eevent);
            break;
        default:
            break;
    }

    /* Egress redirect update */
    if (update) {
        BCMDRD_PBMP_CLEAR(mask);
        BCMDRD_PBMP_OR(
           mask,
           mon_redirect_trace_state[unit]->eevent_redirect);
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_mon_redirect_trace_update(unit,
                                                       trans_id,
                                                       lt_id,
                                                       mask));
    }


exit:
    SHR_FUNC_EXIT();

}

static bcmcth_mon_drv_t bcm56780_a0_cth_mon_drv = {
   .mon_init = &bcm56780_a0_cth_mon_init,
   .mon_deinit = &bcm56780_a0_cth_mon_deinit,
   .trace_op = &bcm56780_a0_cth_mon_trace_op,
   .drop_op = &bcm56780_a0_cth_mon_drop_op,
   .telemetry_control_op = &bcm56780_a0_cth_mon_telemetry_control_op,
   .telemetry_instance_op = &bcm56780_a0_cth_mon_telemetry_instance_op,
   .collector_ipv4_vlan_info_copy =
       &bcm56780_a0_cth_mon_collector_ipv4_vlan_info_copy,
   .collector_ipv6_vlan_info_copy =
       &bcm56780_a0_cth_mon_collector_ipv6_vlan_info_copy,
   .ft_drv = &bcm56780_a0_flowtracker_drv,
   .redirect_op = &bcm56780_a0_cth_mon_redirect_trace_op,
};

/*******************************************************************************
 * Public functions
 */

bcmcth_mon_drv_t *
bcm56780_a0_cth_mon_drv_get(int unit)
{
    return &bcm56780_a0_cth_mon_drv;
}
