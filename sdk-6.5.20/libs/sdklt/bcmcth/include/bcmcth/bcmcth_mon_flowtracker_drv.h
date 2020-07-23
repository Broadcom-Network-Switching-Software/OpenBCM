/*! \file bcmcth_mon_flowtracker_drv.h
 *
 * BCMCTH Flowtracker Driver related definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_DRV_H
#define BCMCTH_MON_FLOWTRACKER_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_hash_str.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_udf_constants.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmcth/generated/flowtracker_ha.h>
#include <bcmbd/mcs_shared/mcs_msg_common.h>
#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>
#include <bcmltd/chip/bcmltd_mon_constants.h>
#include <bcmltd/chip/bcmltd_fp_constants.h>
#include <bcmcth/bcmcth_mon_flowtracker_util.h>
#include <bcmcth/bcmcth_mon_collector.h>

/*! Maximum number of EM group actions supported */
#define FLOWTRACKER_EM_ACTION_MAX 4

/*! Maximum number of EM group action info parts supported. */
#define FLOWTRACKER_ACTION_INFO_MAX  FP_INFO_MAX

/*! Maximum number of EM qualifiers supported. */
#define FLOWTRACKER_EM_QUAL_MAX  MCS_SHR_FT_EM_MAX_QUAL

/*! Minimum export length required */
#define FLOWTRACKER_MIN_EXPORT_LENGTH 128

/*! Max export length supported 9K - Jumbo */
#define FLOWTRACKER_MAX_EXPORT_LENGTH 9000

/*! Default value of export interval. 100 ms */
#define FLOWTRACKER_DEF_EXPORT_INTERVAL_MSECS (100)

/*! Default value of scan interval. 100 ms */
#define FLOWTRACKER_DEF_SCAN_INTERVAL_USECS (FLOWTRACKER_DEF_EXPORT_INTERVAL_MSECS * 1000)

/*! Scan interval must be in steps of 1 msec */
#define FLOWTRACKER_SCAN_INTERVAL_STEP_USECS (1 * 1000)

/*! Check if HW learn is enabled */
#define FLOWTRACKER_HW_LEARN_ENABLED(hw_learn) \
    (hw_learn == BCMLTD_COMMON_FLOWTRACKER_HARDWARE_LEARN_T_T_ENABLE ||\
     hw_learn == BCMLTD_COMMON_FLOWTRACKER_HARDWARE_LEARN_T_T_ENABLE_HARDWARE_ONLY)

/*! Check if HW learn without eapp is enabled */
#define FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(hw_learn) \
    (hw_learn == BCMLTD_COMMON_FLOWTRACKER_HARDWARE_LEARN_T_T_ENABLE_HARDWARE_ONLY)

/*! Check if HW learn with eapp is enabled */
#define FLOWTRACKER_HW_LEARN_WITH_EAPP_ENABLED(hw_learn) \
    (hw_learn == BCMLTD_COMMON_FLOWTRACKER_HARDWARE_LEARN_T_T_ENABLE)

/*! Maximum number of words in FT_GROUP_TABLE_CONFIG */
#define FLOWTRACKER_HW_LEARN_ACTION_DATA_WORDS_MAX 4

/*!
 * \brief  Different data formats possible for FT HW learning.
 */
typedef enum {
    /*! Data only format. The entire 105 bits is just data. */
    FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_ONLY = 1,

    /*! PDD index(5bits)+SBR index(4bits)+Data(96bits) format. */
    FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_PLUS_PDD_IDX_PLUS_SBR_IDX = 2,
}bcmcth_mon_ft_hw_learn_data_fmt_t;

/*!
 * \brief  Structure to access CTR_ING_EFLEX_ACTION_PROFILE_INFO LT.
 */
typedef struct bcmcth_mon_ft_ctr_ing_eflex_action_profile_info_lt_s{
    /*! Logical table ID. */
    uint32_t    ltid;

    /*! Logical table key field ID. */
    uint32_t    key_fid;

    /*! Number of flex counter pools allocated. */
    uint32_t    num_pools_fid;

    /*! Field ID for base counter index of the flex counters allocated. */
    uint32_t    base_index_fid;
} bcmcth_mon_ft_ctr_ing_eflex_action_profile_info_lt_t;


/*!
 * \brief Structure to access CTR_ING_EFLEX_ACTION_PROFILE LT.
 */
typedef struct bcmcth_mon_ft_ctr_ing_eflex_action_profile_lt_s{
    /*! Logical table ID. */
    uint32_t    ltid;

    /*! Logical table key field ID. */
    uint32_t    key_fid;

    /*!  Pool ID is for the starting counter block. */
    uint32_t     pool_id_fid;

    /*! Number of flex counters. */
    uint32_t    num_counters_fid;

    /*! Ctr Action Id .*/
    uint32_t    action_fid;
} bcmcth_mon_ft_ctr_ing_eflex_action_profile_lt_t;

/*!
 * \brief Structure to access FP_ING_GRP_TEMPLATE_INFO LT.
 */
typedef struct bcmcth_mon_ft_ing_grp_template_info_lt_s{
    /*! Logical table ID. */
    const char*    ltid;

    /*! Logical table key field ID.
        DT_EM_GRP_TEMPLATE_ID from FT GRP LT*/
    const char*   key_fid;

    /*! Number of partitions used for groups width expansion. */
    const char*     num_partition_id_fid;

    /*! Bucket mode. */
    const char*     bucket_mode_fid;

} bcmcth_mon_ft_ing_grp_template_info_lt_t;

/*!
 * \brief FP_ING_GRP_TEMPLATE_INFO LT and field ID structutre.
 */
typedef struct bcmcth_mon_ft_ing_grp_template_info_lt_id_s{
    /*! Logical table ID. */
    bcmltd_sid_t     ltid;

    /*! Logical table key field ID.
        DT_EM_GRP_TEMPLATE_ID from FT GRP LT*/
    bcmltd_fid_t    key_fid;

    /*! Number of partitions used for groups width expansion. */
    bcmltd_fid_t     num_partition_id_fid;

    /*! Bucket mode. */
    bcmltd_fid_t     bucket_mode_fid;

} bcmcth_mon_ft_ing_grp_template_info_lt_id_t;

/*! Structure to store FP_ING_GRP_TEMPLATE_INFO lookup info. */
typedef struct ing_grp_template_info_s {
    /*! Number of partitions used for groups width expansion. */
    uint8_t num_partition;

    /*! Bucket mode. */
    uint8_t bucket_mode;

    /*! Key type. */
    uint8_t key_type;
}ing_grp_template_info_t;

/*! Max tracking parameters type. */
#define FLOWTRACKER_TRACKING_TYPE_MAX 16

/*!
 * Maximum offsets any qualifier can have. This is the sum of multiplexers
 * available at final stage of keygen block for widest possible group mode.
 */
#define FP_QUAL_INFO_MAX FP_INFO_MAX

/*! Qualifier info structure*/
typedef struct fp_qualifier_info_s {
    /*! Qualifier */
    uint8_t qual;

    /*! Qualifier name. */
    const char* name;

    /*!
     * Number of valid indexes in all the offsets,
     * widths, partition ID arrays.
     */
    uint8_t  num_qual_info;

    /*! Offsets of qualifier in the final key. */
    uint16_t qual_offset[FP_QUAL_INFO_MAX];

    /*! Widths of qualifier in the final key. */
   uint16_t qual_width[FP_QUAL_INFO_MAX];
}fp_qualifier_info_t;

/*! FP_EM_GRP_TEMPLATE_PARTITION_INFO LT information */
typedef struct ing_grp_template_partition_info_s {
    /*! Key type - KEY_ATTRIBUTES index */
    uint8_t key_type;

    /*! Qualifier information */
    fp_qualifier_info_t qual_info[FLOWTRACKER_EM_QUAL_MAX];
}ing_grp_template_partition_info_t;

/*! Qualifier LT info */
typedef struct fp_qualifier_lt_info_s {
    /*! Qualifier */
    uint8_t qual;

    /*! Qualifier name. */
    const char* name;

    /*!
     * Field ID for number of valid indexes in all the offsets,
     * widths, partition ID arrays
     */
    const char* num_qual_info_fid;

    /*! Field ID for offsets of qualifier in the final key. */
    const char* qual_offset_fid;

    /*!
     * Field ID for widths of qualifier in the final key.
     * The width at each index corresponds to the offset at the same index.
     */
    const char* qual_width_fid;
}fp_qualifier_lt_info_t;

/*! Group template partition infor LT access structure. */
typedef struct bcmcth_mon_ft_ing_grp_template_partition_info_lt_s{
    /*! Logical table ID. */
    const char*    ltid;

    /*! Logical table key field ID.
      DT_EM_GRP_TEMPLATE_ID from FT GRP LT*/
    const char*    key_fid1;

    /*! Logical table key field ID.
      num_partition_id from FP_ING_GRP_TEMPLATE_INFO*/
    const char*    key_fid2;

    /*! Qualifier info */
    fp_qualifier_lt_info_t qual_info[FLOWTRACKER_EM_QUAL_MAX];

    /*! KEY_TYPE field ID. */
    const char*     key_type_fid;

    /*! Number of em qualifiers. */
    uint8_t     num_quals;
} bcmcth_mon_ft_ing_grp_template_partition_info_lt_t;

/*! Qualifier LT and Field ID strucutre */
typedef struct fp_qualifier_lt_id_info_s {
    /*!
     * Qualifier
     */
    uint8_t qual;

    /*! Qualifier name. */
    const char* name;

    /*!
     * Field ID for number of valid indexes in all the offsets,
     * widths, partition ID arrays
     */
    bcmltd_fid_t num_qual_info_fid;

    /*! Field ID for offsets of qualifier in the final key. */
    bcmltd_fid_t qual_offset_fid;

    /*!
     * Field ID for widths of qualifier in the final key.
     * The width at each index corresponds to the offset at the same index.
     */
    bcmltd_fid_t qual_width_fid;
}fp_qualifier_lt_id_info_t;


/*! Group template partition info LT and Field ID  structure. */
typedef struct bcmcth_mon_ft_ing_grp_template_partition_info_lt_id_s{
    /*! Logical table ID. */
    bcmltd_sid_t    ltid;

    /*! Logical table key field ID.
      DT_EM_GRP_TEMPLATE_ID from FT GRP LT*/
    bcmltd_fid_t    key_fid1;

    /*! Logical table key field ID.
      num_partition_id from FP_ING_GRP_TEMPLATE_INFO*/
    bcmltd_fid_t    key_fid2;

    /*! Number of em qualifiers. */
    bcmltd_fid_t     num_quals;

    /*! Qualifier info */
    fp_qualifier_lt_id_info_t qual_info[FLOWTRACKER_EM_QUAL_MAX];

    /*! KEY_TYPE field ID. */
    bcmltd_fid_t     key_type_fid;

} bcmcth_mon_ft_ing_grp_template_partition_info_lt_id_t;

/*!
 * \brief Structure to access DT_EM_FT_GRP_TEMPLATE LT.
 */
typedef struct bcmcth_mon_dt_em_ft_grp_template_lt_s{
    /*! Logical table ID. */
    const char*    lt_name;

    /*! Logical table key field ID.
        DT_EM_GRP_TEMPLATE_ID from FT GRP LT*/
    const char*    key_name;

    /*! PDD template ID. */
    const char*    dt_em_ft_pdd_template_id_name;

    /*! Group mode. */
    const char*    mode_name;
} bcmcth_mon_dt_em_ft_grp_template_lt_t;

/*! Action info structure*/
typedef struct ing_pdd_template_partition_info_s {
    /*! Action */
    uint8_t  action;

    /*! Number of parts of action info. */
    uint8_t  num_action_info;

    /*! Offsets of qualifier in the final key. */
    uint16_t action_offset[FLOWTRACKER_ACTION_INFO_MAX];

    /*! Widths of qualifier in the final key. */
    uint16_t action_width[FLOWTRACKER_ACTION_INFO_MAX];
}ing_pdd_template_partition_info_t;

/*! Action info */
typedef struct fp_action_info_s {
    /*! Action */
    uint8_t action;

    /*! Field ID for number of parts of info for a given action */
    const char* num_action_info_fid;

    /*! Field ID for offsets of action in the final policy. */
    const char* action_offset_fid;

    /*!
     * Field ID for widths of copy to cpu action in the final policy.
     * Width at each index is corresponding to offset at the same index.
     */
    const char* action_width_fid;
}fp_action_info_t;

/*!
 * \brief Structure to access FP_ING_PDD_TEMPLATE_PARTITION_INFO LT.
 */
typedef struct bcmcth_mon_ft_ing_pdd_template_partition_info_lt_s{
    /*! Logical table ID. */
    const char*    ltid;

    /*! Logical table key field ID.
        DT_EM_FT_PDD_TEMPLATE_ID from DT_EM_FT_GRP_TEMPLATE LT*/
    const char*    key_fid1;

     /*! Logical table key field ID.
        num_partition_id from FP_ING_GRP_TEMPLATE_INFO*/
    const char*    key_fid2;

    /*! Number of actions we are interested in */
    uint8_t    num_actions;

    /*! Action info */
    fp_action_info_t action_info[FLOWTRACKER_ACTIONS_MAX];

} bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t;

/*! Action info Field ID structure */
typedef struct fp_action_info_id_s {
    /*! Action */
    bcmltd_fid_t action;

    /*! Field ID for number of parts of info for a given action */
    bcmltd_fid_t num_action_info_fid;

    /*! Field ID for offsets of action in the final policy. */
    bcmltd_fid_t action_offset_fid;

    /*!
     * Field ID for widths of copy to cpu action in the final policy.
     * Width at each index is corresponding to offset at the same index.
     */
    bcmltd_fid_t action_width_fid;
}fp_action_info_id_t;

/*!
 * \brief FP_ING_PDD_TEMPLATE_PARTITION_INFO LT and Field ID structure.
 */
typedef struct bcmcth_mon_ft_ing_pdd_template_partition_info_lt_id_s{
    /*! Logical table ID. */
    bcmltd_sid_t    ltid;

    /*! Logical table key field ID.
        DT_EM_FT_PDD_TEMPLATE_ID from DT_EM_FT_GRP_TEMPLATE LT*/
    bcmltd_fid_t    key_fid1;

     /*! Logical table key field ID.
        num_partition_id from FP_ING_GRP_TEMPLATE_INFO*/
    bcmltd_fid_t    key_fid2;

    /*! Number of actions we are interested in */
    bcmltd_fid_t    num_actions;

    /*! Action info */
    fp_action_info_id_t action_info[FLOWTRACKER_ACTIONS_MAX];

} bcmcth_mon_ft_ing_pdd_template_partition_info_lt_id_t;


/*!
 * \brief Structure to access UDF_POLICY LT.
 */
typedef struct bcmcth_mon_ft_udf_policy_lt_s{
    /*! Logical table ID. */
    uint32_t    ltid;

    /*! Logical table UDF_POLICY_ID field id.*/
    uint32_t    udf_policy_fid;

     /*! Corresponds to PACKET_HEADER fid*/
    uint32_t    packet_header_fid;

    /*! Corresponds to OFFSET fid */
    uint32_t    offset_fid;

    /*! Corresponds to CONTAINER_4_BYTE field */
    uint32_t    cont_4_byte_fid;

    /*! Corresponds to CONTAINER_2_BYTE field */
    uint32_t    cont_2_byte_fid;

    /*! Corresponds to CONTAINER_1_BYTE field */
    uint32_t    cont_1_byte_fid;
} bcmcth_mon_ft_udf_policy_lt_t;

/*! Structure to hold UDF_POLICY info. */
typedef struct ft_udf_policy_info_s {
    /*! The tracking parameter type used for tracking a flow. */
    bcmltd_common_flowtracker_tracking_param_type_t_t   type;

    /*! Corresponds to PACKET_HEADER field.*/
    uint16_t packet_header;

    /*! Corresponds to OFFSET field.*/
    uint32_t offset;

    /*! Corresponds to CONTAINER_4_BYTE field */
    bool     cont_4_byte[NUM_UDF_CONTAINER];

    /*! Corresponds to CONTAINER_2_BYTE field */
    bool     cont_2_byte[NUM_UDF_CONTAINER];

    /*! Corresponds to CONTAINER_1_BYTE field */
    bool     cont_1_byte[NUM_UDF_CONTAINER];
} ft_udf_policy_info_t;

/*! FT info data structure */
typedef struct bcmcth_mon_ft_info_s {
    /*! Persistent info to be recovered from HA memory */
    bcmcth_mon_ft_ha_t *ha;

    /*! DMA buffer for holding send/receive messages */
    uint8_t *dma_buffer;

    /*! Physical address of the DMA buffer */
    uint64_t dma_buffer_p_addr;

    /*!  Features supported by the EApp*/
    uint32_t uc_features;

    /*! Elephant mode */
    uint8_t eleph_mode;

    /*! Configuration mode */
    uint32_t cfg_mode;

    /*!
     * Current flow index used for traverse of
     * MON_FLOWTRACKER_FLOW_DATA LT
     */
    uint32_t curr_flow_index;

    /*!
     * Current flow index used for traverse of
     * MON_FLOWTRACKER_FLOW_STATIC LT
     */
    uint32_t static_curr_flow_index;

    /*!
     * Current msging version used for messaging
     * by firmware.
     */
    uint32_t uc_msg_version;
} bcmcth_mon_ft_info_t;

/*! Data strucuture for MON_FLOWTRACKER_STATS LT. */
typedef struct bcmcth_mon_ft_stats_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_STATSt_FIELD_COUNT);

    /*! Corresponds to LEARN_PKT_DISCARD_PARSE_ERROR field in the LT. */
    uint32_t learn_pkt_discard_parse_err_cnt;

    /*! Corresponds to LEARN_PKT_DISCARD_INVALID_GROUP field in the LT. */
    uint32_t learn_pkt_discard_invalid_grp_cnt;

    /*! Corresponds to LEARN_PKT_DISCARD_FLOW_LIMIT_EXCEED field in the LT. */
    uint32_t learn_pkt_discard_flow_limit_exceed_cnt;

    /*! Corresponds to LEARN_PKT_DISCARD_PIPE_LIMIT_EXCEED field in the LT. */
    uint32_t learn_pkt_discard_pipe_limit_exceed_cnt;

    /*! Corresponds to LEARN_PKT_DISCARD_EM_FAIL field in the LT. */
    uint32_t learn_pkt_discard_em_fail_cnt;

    /*! Corresponds to LEARN_PKT_DISCARD_DUPLICATE field in the LT. */
    uint32_t learn_pkt_discard_duplicate_cnt;

    /*! Corresponds to NUM_FLOWS_LEARNT field in the LT. */
    uint32_t num_flows_learnt;

    /*! Corresponds to NUM_FLOWS_EXPORTED field in the LT. */
    uint32_t num_flow_exported;

    /*! Corresponds to NUM_PACKETS_EXPORTED field in the LT. */
    uint32_t num_packets_exported;

    /*! Corresponds to NUM_FLOWS_AGED field in the LT. */
    uint32_t num_flows_aged;

    /*! Corresponds to NUM_FLOWS_ELEPHANT field in the LT. */
    uint32_t num_elephant_flows;
}bcmcth_mon_ft_stats_t;

/*!
 * Data structure for the read-only operational fields in
 * MON_FLOWTRACKER_CONTROL LT.
 */
typedef struct bcmcth_mon_ft_control_oper_fields_s {

    /*! Corresponds to MAX_GROUPS_OPER field in the LT. */
    uint32_t max_groups;

    /*! Corresponds to MAX_EXPORT_LENGTH_OPER field in the LT. */
    uint32_t max_export_length;

    /*! Corresponds to MAX_FLOWS_OPER field in the LT. */
    uint32_t max_flows[FLOWTRACKER_PIPES_MAX];

    /*! Corresponds to CTR_ING_FLEX_ACTION_PROFILE_ID_OPER field in the LT. */
    uint32_t ctr_ing_flex_action_profile_id;

    /*!
     * Corresponds to
     * DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPER
     * field in the LT.
     */
    uint32_t db_ewide_hb_ctr_ing_flex_action_profile_id;

    /*!
     * Corresponds to
     * QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPER
     * field in the LT.
     */
    uint32_t qd_ewide_hb_ctr_ing_flex_action_profile_id;

    /*!
     * Corresponds to
     * DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPER
     * field in the LT.
     */
    uint32_t db_ewide_hb_ctr_ing_flex_grp_action;

    /*!
     * Corresponds to
     * QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPER
     * field in the LT.
     */
    uint32_t qd_ewide_hb_ctr_ing_flex_grp_action;

    /*! Corresponds to ETRAP_OPER field in the LT. */
    uint32_t etrap;

    /*! Corresponds to SCAN_INTERVAL_USECS_OPER field in the LT. */
    uint32_t scan_interval_usecs;

    /*! Corresponds to  HOST_MEM_OPER field in the LT. */
    uint16_t host_mem;

    /*! Corresponds to FLOW_START_TIMSTAMP_ENABLE_OPER field in the LT. */
    uint32_t flow_start_timestamp_enable;

    /*! Corresponds to HARDWARE_LEARN_OPER field in the LT. */
    bcmltd_common_flowtracker_hardware_learn_t_t hw_learn;

    /*! Corresponds to OPERATIONAL_STATE fields in the LT. */
    uint32_t operational_state;

}bcmcth_mon_ft_control_oper_fields_t;

/*! Data strucuture for MON_FLOWTRACKER_CONTROL LT. */
typedef struct bcmcth_mon_ft_control_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_CONTROLt_FIELD_COUNT);

    /*! Bitmap of index to be operated in max_flows. */
    SHR_BITDCLNAME(fbmp_maxflows, FLOWTRACKER_PIPES_MAX);

    /*! Corresponds to FLOWTRACKER field in the LT. */
    bool flowtracker;

    /*! Corresponds to OBSERVATION_DOMAIN field in the LT. */
    uint32_t observation_domain;

    /*! Corresponds to MAX_GROUPS field in the LT. */
    uint32_t max_groups;

    /*! Corresponds to MAX_EXPORT_LENGTH field in the LT. */
    uint32_t max_export_length;

    /*! Corresponds to MAX_FLOWS field in the LT. */
    uint32_t max_flows[FLOWTRACKER_PIPES_MAX];

    /*!
     * Corresponds to PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_ID field in the LT.
     */
    uint32_t ctr_ing_flex_action_profile_id;

    /*!
     * Corresponds to DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID
     * field in the LT.
     */
    uint32_t db_ewide_hb_ctr_ing_flex_action_profile_id;

    /*!
     * Corresponds to QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID
     * field in the LT.
     */
    uint32_t qd_ewide_hb_ctr_ing_flex_action_profile_id;

    /*!
     * Corresponds to
     * DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION
     * field in the LT.
     */
    uint32_t db_ewide_hb_ctr_ing_flex_grp_action;

    /*!
     * Corresponds to
     * QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION
     * field in the LT.
     */
    uint32_t qd_ewide_hb_ctr_ing_flex_grp_action;

    /*! Corresponds to ETRAP field in the LT. */
    uint32_t etrap;

    /*! Corresponds to SCAN_INTERVAL_USECS field in the LT. */
    uint32_t scan_interval_usecs;

    /*! Corresponds to HOST_MEM field in the LT. */
    uint16_t host_mem;

    /*! Corresponds to FLOW_START_TIMSTAMP_ENABLE field in the LT. */
    uint32_t flow_start_timestamp_enable;

    /*!
     * Structure containing the read-only fields in the LT. This structure
     * contains the the fields that are updated internally and sent back to IMM
     * during INSERT or UPDATE operation.
     */
    bcmcth_mon_ft_control_oper_fields_t oper;

    /*!
     * HW learning enable/disable/enable_without_eapp.
     */
    bcmltd_common_flowtracker_hardware_learn_t_t hw_learn;
}bcmcth_mon_ft_control_t;


/*!
 * FLOWTRACKER_ACTION_INFO_T is used to describe the action that can
 * be configured on a flowtracker group.
 */
typedef struct flowtracker_action_info_s {
        /*! The type of flowtracker group action. */
        bcmltd_common_flowtracker_action_type_t_t      type;

        /*!
         * When the TYPE field is EM_FT_OPAQUE_OBJ0, this field indicates
         * the value associated with the opaque object.
         */
        uint16_t        em_ft_opaque_obj0_val;

        /*!
         * When the TYPE field is EM_FT_IOAM_GBP_ACTION, this field indicates
         * the type of action to be performed.
         * If bit 0 is set, indicates IOAM/IFA insert enable.
         * If bit 1 is set, indicates IOAM/IFA delete enable.
         * If bit 2 is set, indicates use of residence time.
         * If bit 3 is set, indicates GBP_VALID.
 */
        uint8_t                         em_ft_ioam_gbp_action_val;

        /*!
         * When the TYPE field is EM_FT_COPY_TO_CPU, this field indicates
         * whether copy to cpu action is enabled or disabled.
         */
        uint8_t                         em_ft_copy_to_cpu_val;

        /*!
         * When the TYPE field is EM_FT_DROP_ACTION, this field indicates
         * whether drop action is enabled or disabled.
         */
        uint8_t                         em_ft_drop_action_val;

        /*!
         * When the TYPE field is DESTINATION, this field indicates
         * the value of the destination.
         */
        uint16_t                        destination_val;

        /*!
         * When the TYPE field is DESTINATION_TYPE, this field indicates
         * the value of the destination type.
         */
        uint8_t                         destination_type_val;

        /*!
         * When the TYPE field is EM_FT_FLEX_STATE_ACTION, this field
         * indicates the flex state action profile.
         */
        uint8_t                         em_ft_flex_state_action_val;

        /*!
         * When the TYPE field is FLEX_CTR_ACTION, this field
         * indicates the flex counter action profile.
         */
        uint8_t                         flex_ctr_action_val;

        /*!
         * When the TYPE field is PKT_FLOW_ELIGIBILITY, this field
         * indicates the flex counter action profile.
         */
        uint8_t                         pkt_flow_eligibility_val;

        /*!
         * When the TYPE field is L2_IIF_SVP_MIRROR_INDEX_0, this field
         * indicates the value of the mirror index.
         */
        uint8_t                         l2_iif_svp_mirror_index_0_val;
}flowtracker_action_info_t;

/*!
 * FLOWTRACKER_TRACKING_PARAM_T is used to describe
 * the tracking parameters for a flow.
 */
typedef struct flowtracker_tracking_param_s {
        /*! The tracking parameter type used for tracking a flow. */
        bcmltd_common_flowtracker_tracking_param_type_t_t   type;

        /*!
         * If the tracking parameter is implemented using user defined
         * fields in the EM table, this field indicates the index into
         * the UDF_POLICY logical table which defines the
         * user defined field.
         */
        uint16_t udf_policy_id;
}flowtracker_tracking_param_t;

/*! Data strucuture for MON_FLOWTRACKER_GROUP LT. */
typedef struct bcmcth_mon_ft_group_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_GROUPt_FIELD_COUNT);

    /*! Index into the MON_FLOWTRACKER_GROUP table. */
    uint32_t mon_flowtracker_group_id;

    /*!
     * Enable to indicate to the flowtracker embedded application to
     * start learning flows. If disabled, indicates flowtracker
     * embedded application to stop learning and delete all
     * the learnt flows.
     */
    bool learn;

    /*!
     * Index into the DT_EM_GRP_TEMPLATE table. This index corresponds
     * to the EM group created for tracking flows based on the flowtracker
     * group tracking parameters defined in the TRACKING_PARAMETERS field.
     */
    uint32_t dt_em_grp_template_id;

    /*! Number of flow tracking parameters used to track a flow. */
    uint32_t  num_tp;

    /*! List of flow tracking parameters used to track a flow. */
    flowtracker_tracking_param_t tp[FLOWTRACKER_TRACKING_PARAMETERS_MAX];

    /*! Maximum number of flows that can be learnt on this group. */
    uint32_t                        flow_limit;

    /*!
     * Interval in milliseconds after which flows are aged out
     * (removed from the flow table) if the flow is idle.
     */
    uint32_t                        aging_interval_ms;

    /*!
     * Number of export triggers. Used to indicate the number of valid
     * export triggers filled into the EXPORT_TRIGGERS field.
     */
    uint8_t                         num_export_triggers;

    /*! List of triggers/events on which export to collector happens.*/
    bcmltd_common_flowtracker_export_trigger_type_t_t
        export_triggers[FLOWTRACKER_EXPORT_TRIGGERS_MAX];

    /*! Number of actions that are configured on the flow group. */
    uint32_t                        num_actions;

    /*! List of actions that are configured on the flow group. */
    flowtracker_action_info_t       actions[FLOWTRACKER_ACTIONS_MAX];

    /*! Index into the MON_FLOWTRACKER_ELEPHANT_PROFILE table. */
    uint32_t                        mon_flowtracker_elephant_profile_id;

    /*!
     * Read-only field which reflects the operational state of
     * the entry.
     */
    bcmltd_common_mon_flowtracker_group_state_t_t   oper;
}bcmcth_mon_ft_group_t;

/*! Data strucuture for MON_FLOWTRACKER_GROUP_STATUS LT. */
typedef struct bcmcth_mon_ft_group_status_s {
    /*! Corresponds to MON_FLOWTRACKER_GROUP_ID field in the LT. */
    uint32_t group_id;

    /*! Corresponds to FLOW_COUNT field in the LT. */
    uint32_t flow_count;
}bcmcth_mon_ft_group_status_t;

/*!
 * The MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROL table is used for
 * deleting or modifying the action data of a HW learnt flow entry.
 */
typedef struct bcmcth_mon_ft_hw_learn_flow_action_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_HW_LEARN_FLOW_ACTION_CONTROLt_FIELD_COUNT);

    /*! Pipe index. */
    uint8_t pipe;

    /*! Exact match table index. */
    uint32_t exact_match_idx;

    /*! Exact match table mode. */
    bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode;

    /*! Index into the MON_FLOWTRACKER_GROUP table. */
    uint32_t mon_flowtracker_group_id;

    /*!
     * List of actions that are configured on the flow group
     * that has to be modified.
     */
    flowtracker_action_info_t       actions[FLOWTRACKER_ACTIONS_MAX];

    /*! Delete or modify action */
    bcmltd_common_flowtracker_hw_learn_flow_cmd_t_t cmd;

    /*! Action data buffer - used for passing to chip specific layer */
    uint32_t action_data[FLOWTRACKER_HW_LEARN_ACTION_DATA_WORDS_MAX];

}bcmcth_mon_ft_hw_learn_flow_action_t;

/*!
 * \brief Flowtracker init fucntion
 *
 * Initializes the flowtracker application.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int bcmcth_mon_ft_eapp_init(int unit);

/*!
 * \brief Flowtracker resource alloc function
 *
 * Allocates global data and DMA buffers needed for telemetry applications.
 *
 * \param [in] unit Unit number.
 * \param [in] warm warmboot or not.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int bcmcth_mon_ft_sw_resources_alloc(int unit, bool warm);

/*!
 * \brief Free memory used by the FT EApp.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmcth_mon_ft_uc_free(int unit);

/*!
 * \brief Flowtracker information structure get
 *
 * Returns the flowtracker info structure
 *
 * \param [in] unit Unit number.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern bcmcth_mon_ft_info_t * bcmcth_mon_ft_info_get(int unit);

/*!
 * \brief Register FT IMM handlers.
 *
 * Register the IMM handlers required by the MON_FLOWTRACKER_XXX
 * LTs. This function is called as part of MON IMM init.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates warmboot or coldboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int bcmcth_mon_ft_imm_register(int unit, bool warm);

/*!
 * \brief MON_FLOWTRACKER_CONTROL LT insert handler.
 *
 * This function handles the INSERT opcode for the MON_FLOWTRACKER_CONTROL
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] insert Structure containing the fields being inserted into IMM.
 * \param [out] oper Structure containing the operational values that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_control_insert(int unit,
                             bcmcth_mon_ft_control_t *insert,
                             bcmcth_mon_ft_control_oper_fields_t *oper);

/*!
 * \brief MON_FLOWTRACKER_CONTROL LT update handler.
 *
 * This function handles the UPDATE opcode for the MON_FLOWTRACKER_CONTROL
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure containing the fields that are currently
 *             present in the IMM.
 * \param [in] update Structure containing the fields being updated.
 * \param [out] oper Structure containing the operational values that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_control_update(int unit,
                              bcmcth_mon_ft_control_t *cur,
                              bcmcth_mon_ft_control_t *update,
                              bcmcth_mon_ft_control_oper_fields_t *oper);

/*!
 * \brief MON_FLOWTRACKER_CONTROL LT delete handler.
 *
 * This function handles the DELETE opcode for the MON_FLOWTRACKER_CONTROL
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_control_delete(int unit);

/*!
 * \brief MON_FLOWTRACKER_STATS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for the MON_FLOWTRACKER_STATS
 * LT. This function is called from the lookup callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Structure containing the fields that are retreived.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_uc_stats_lookup(int unit, bcmcth_mon_ft_stats_t *entry);

/*!
 * \brief Flowtracker information structure get
 *
 * Returns the flowtracker info structure
 *
 * \param [in] unit Unit number.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern bcmcth_mon_ft_info_t * bcmcth_mon_ft_info_get(int unit);

/*!
 * \brief Initialize MON_FLOWTRACKER_COTNROLt embedded application.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the message structure required to initialize
 * flowtracker app
 *
 * \param [in] unit Unit number.
 * \param [in] entry flowtracker control instance.
 * \param [out] msg to be initialized.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_flowtracker_control_msg_init_f)(
    int unit,
    bcmcth_mon_ft_control_t *entry,
    mcs_ft_msg_ctrl_init_t *msg);

/*!
 * \brief Initialize EM Bank UFT message
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the message structure required to initialize
 * EM bank UFT information
 *
 * \param [in] unit Unit number.
 * \param [out] msg to be initialized.
 * \param [in] Hw learn is enabled or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_flowtracker_em_bank_uft_msg_init_f)(
    int unit,
    mcs_ft_msg_ctrl_em_bank_uft_info_t *msg,
    bcmltd_common_flowtracker_hardware_learn_t_t hw_learn_en);

/*!
 * \brief Initialize MON_FLOWTRACKER_GROUPt embedded application.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the message structure required to create
 * flowtracker group
 *
 * \param [in] unit Unit number.
 * \param [in] entry flowtracker control instance.
 * \param [out] msg to be initialized.
 * \param [out] pdd_index EM action data profile index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mon_flowtracker_grp_flex_em_msg_init_f)(
    int unit,
    bcmcth_mon_ft_group_t *entry,
    mcs_ft_msg_ctrl_flex_em_group_create_t *msg,
    uint32_t *pdd_index);

/*!
 * \brief Initialize the message for HW learn related options sent to UKERNEL.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the message structure of HW learn related options
 * to UKERNEL.
 *
 * \param [in] unit Unit number.
 * \param [in] entry flowtracker control instance.
 * \param [out] msg to be initialized.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_hw_learn_opt_msg_init_f)(
                                int unit,
                                bcmcth_mon_ft_control_t *entry,
                                mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg);

/*!
 * \brief Configure HW for a particular flowtracker group.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to configure flowtracker group in HW.
 *
 * \param [in] unit Unit number.
 * \param [in] ctrl_entry flowtracker control instance.
 * \param [in] entry flowtracker group instance.
 * \param [in] msg Flex em msg sent to UKERNEL.
 * \param [in] pdd_index EM data PDD profile index.
 * \param [in] sbr_index EM data SBR profile index.
 * \param [in] data_format Format of the DATA.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_grp_hw_learn_config_f)(
                                int unit,
                                bcmcth_mon_ft_control_t *ctrl_entry,
                                bcmcth_mon_ft_group_t *entry,
                                mcs_ft_msg_ctrl_flex_em_group_create_t *msg,
                                uint32_t pdd_index,
                                uint32_t sbr_index,
                                uint8_t  data_format);

/*!
 * \brief Configure HW for enabling flowtracker learning.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to configure HW to enable flowtracker learning.
 *
 * \param [in] unit Unit number.
 * \param [in] entry flowtracker control instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_hw_learn_init_f)(int unit,
                                          bcmcth_mon_ft_control_t *entry);


/*!
 * \brief Get number of max groups supported by HW Flowtracker.
 *
 * \param [in] unit Unit number.
 * \param [out] size number of groups supported by HW Flowtracker.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_hw_learn_max_groups_get_f)(
                                    int unit,
                                    uint32_t  *size);

/*!
 * \brief Update flow limit value of a Flowtracker HW learning group.
 *
 * \param [in] unit  Unit number.
 * \param [in] entry Flowtracker group entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_hw_learn_flow_limit_upd_f)(
                                    int unit,
                                    bcmcth_mon_ft_group_t *entry);

/*!
 * \brief Get the list of packet rx metadata fields used by Flowtracker.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  max_fids Maximum number of field IDs.
 * \param [out] num_fids Actual number of field IDs.
 * \param [out] fids     The list of field IDs.
 * \param [out] uc_fids  The list of field IDs understood by UKERNEL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_rxpmd_flex_fids_get_f)(
                                    int unit,
                                    int max_fids,
                                    int *num_fids,
                                    int *fids,
                                    int *uc_fids);

/*!
 * \brief Configure the flowtracker group hw learning tables.
 *
 * This routine is called only when embedded app is not used.
 *
 * \param [in]  unit     Unit number.
 * \param [in] entry     Flowtracker group entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_grp_hw_learn_config_wo_eapp_f)(
                                    int unit,
                                    bcmcth_mon_ft_group_t *entry);

/*!
 * \brief Uninitialize the flowtracker group hw learning tables.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  group_id Flowtracker group ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_grp_hw_learn_deinit_f)(
                                    int unit,
                                    uint32_t group_id);

/*!
 * \brief Get the number of flows learnt in a flowtracker group.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  group_id Flowtracker group ID.
 * \param [out] flow_cnt Number of flows learnt in the group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_grp_hw_learn_flow_cnt_get_f)(
                                    int unit,
                                    uint32_t group_id,
                                    uint32_t *flow_cnt);
/*!
 * \brief Update the global flow learn threshold.
 *
 * \param [in]  unit     Unit number.
 * \param [in] entry flowtracker control instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_hw_learn_update_glb_flow_threshold_f)(
                                    int unit,
                                    bcmcth_mon_ft_control_t *entry);

/*!
 * \brief De initialize the HW learning mechanism.
 *
 * \param [in]  unit     Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_hw_learn_deinit_f)(
                                    int unit);

/*!
 * \brief Set the FT_COMMAND memory to delete or modify HW learnt flow entry.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  entry     HW learn flow entry info.
 * \param [in]  grp_entry Flowtracker group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int
(*bcmcth_mon_flowtracker_ft_command_set_f)(
                                    int unit,
                                    bcmcth_mon_ft_hw_learn_flow_action_t *entry,
                                    bcmcth_mon_ft_group_t *grp_entry);
/*!
 * \brief Get the string corresponding to the match id.
 *
 * Get the string corresponding to the match id that is required by the EApp.
 * Return NULL if the match ID is not found.
 *
 * \param [in] unit Unit number.
 * \param [in] match_id Match id required by EApp
 *
 * \retval Match id string.
 */
typedef const char* (*bcmcth_mon_flowtracker_match_id_string_get_f) (int unit,
                                                                     mcs_shr_ft_match_id_fields_t match_id);

/*!
 * \brief Flowtracker chip specific driver structure.
 *
 * Maintains information about chip specific Flowtracker driver.
 */
typedef struct bcmcth_mon_flowtracker_drv_s {

    /*! Pointer to access CTR_ING_EFLEX_ACTION_PROFILE_INFO LT. */
    bcmcth_mon_ft_ctr_ing_eflex_action_profile_info_lt_t
                                            *flex_ctr_action_profile_info_lt;

    /*! Pointer to access CTR_ING_EFLEX_ACTION_PROFILE LT. */
    bcmcth_mon_ft_ctr_ing_eflex_action_profile_lt_t
                                            *flex_ctr_action_profile_lt;

    /*! Pointer to access FP_ING_GRP_TEMPLATE_INFO */
    bcmcth_mon_ft_ing_grp_template_info_lt_t  *ing_grp_template_info_lt;

    /*! Pointer to access FP_ING_GRP_TEMPLATE_PARTITION_INFO LT. */
    bcmcth_mon_ft_ing_grp_template_partition_info_lt_t
                                        *ing_grp_template_partition_info_lt;

    /*! Pointer to access FP_ING_PDD_TEMPLATE_PARTITION_INFO LT. */
    bcmcth_mon_ft_ing_pdd_template_partition_info_lt_t
                                        *ing_pdd_template_partition_info_lt;

    /*! Pointer to access DT_EM_FT_GRP_TEMPLATE LT. */
    bcmcth_mon_dt_em_ft_grp_template_lt_t *dt_em_ft_grp_template_lt;

    /*! Pointer to access UDF_POLICY LT. */
    bcmcth_mon_ft_udf_policy_lt_t *udf_policy_lt;

    /*! Flowtracker control init function. */
    bcmcth_mon_flowtracker_control_msg_init_f ft_control_init;

    /*! Flowtracker group flex em message init function. */
    bcmcth_mon_flowtracker_grp_flex_em_msg_init_f ft_grp_init;

    /*! Flowtracker EM UFT bank message init function. */
    bcmcth_mon_flowtracker_em_bank_uft_msg_init_f ft_em_uft_init;

    /*! Flowtracker HW learn opt info msg init function. */
    bcmcth_mon_flowtracker_hw_learn_opt_msg_init_f ft_hw_learn_opt_msg_init;

    /*! Flowtracker HW learn group configuration function. */
    bcmcth_mon_flowtracker_grp_hw_learn_config_f  ft_grp_hw_learn_config;

    /*! Flowtracker HW learn enable configuration function. */
    bcmcth_mon_flowtracker_hw_learn_init_f ft_hw_learn_init;

    /*! Flowtracker HW learn max groups supported get function. */
    bcmcth_mon_flowtracker_hw_learn_max_groups_get_f ft_hw_learn_max_grps_get;

    /*! Flowtracker HW learn flow limit update function. */
    bcmcth_mon_flowtracker_hw_learn_flow_limit_upd_f
                                        ft_hw_learn_flow_limit_update;

    /*! Flowtracker RX packet meta data field ID(s) get function. */
    bcmcth_mon_flowtracker_rxpmd_flex_fids_get_f
                                        ft_rxpmd_flex_fids_get;

    /*! Flowtracker HW learn group configuration without eapp function. */
    bcmcth_mon_flowtracker_grp_hw_learn_config_wo_eapp_f
                                        ft_grp_hw_learn_config_wo_eapp;

    /*! Flowtracker HW learn group de initialization function. */
    bcmcth_mon_flowtracker_grp_hw_learn_deinit_f
                                        ft_grp_hw_learn_deinit;

    /*! Flowtracker HW learn group flow count get function. */
    bcmcth_mon_flowtracker_grp_hw_learn_flow_cnt_get_f
                                        ft_grp_hw_learn_flow_cnt_get;

    /*! Flowtracker HW learn global flow threshold update function. */
    bcmcth_mon_flowtracker_hw_learn_update_glb_flow_threshold_f
                                        ft_hw_learn_update_glb_flow_threshold;

    /*! Flowtracker HW learn global de initialization function. */
    bcmcth_mon_flowtracker_hw_learn_deinit_f
                                        ft_hw_learn_deinit;

    /*! Flowtracker HW learnt flow delete/modify function. */
    bcmcth_mon_flowtracker_ft_command_set_f ft_command_set;

    /*! Number of pipes */
    uint8_t num_pipes;

    /*! Get the match ID strings. */
    bcmcth_mon_flowtracker_match_id_string_get_f match_id_string_get;
}bcmcth_mon_flowtracker_drv_t;

/*!
 * \brief Get the FT driver structure.
 *
 * This function returns a structure which contains the device specific info for
 * FT.
 *
 * \param [in] unit Unit number.
 *
 * \return INT driver API structure
 */
extern bcmcth_mon_flowtracker_drv_t *bcmcth_mon_ft_drv_get(int unit);



/*!
 * \brief MON_FLOWTRACKER_GROUP_STATUS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for the MON_FLOWTRACKER_GROUP_STATUS
 * LT. This function is called from the lookup callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Structure containing the fields that are retreived.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_uc_group_status_lookup(int unit, bcmcth_mon_ft_group_status_t *entry);

/*!
 * FLOWTRACKER_ELEPHANT_FILTER_T is used to describe criteria for
 * promoting a flow to an elephant and demoting a flow to a mouse.
 */
typedef struct bcmcth_mon_flowtracker_elephant_filter_s {

    /*! Enable if flow rate must increase every scan interval. */
    bool incr_rate;

    /*!
     * The time interval for which the flow is to be monitored
     * (in microseconds).
     */
    uint32_t monitor_interval_usecs;

    /*!
     * Minimum rate the flow must have, to continue monitoring
     * (kbits_sec = 1000 bits/sec).
     */
    uint32_t rate_low_threshold_kbits_sec;

    /*!
     * Rate that must be observed at least once during
     * monitoring interval for a flow to pass the monitoring
     * criteria (kbits_sec = 1000 bits/sec).
     */
    uint32_t rate_high_threshold_kbits_sec;

    /*!
     * Total size in bytes that must be observed after the
     * monitoring interval for a flow to pass the monitoring criteria.
     */
    uint64_t size_threshold_bytes;

}bcmcth_mon_flowtracker_elephant_filter_t;

/*!
 * The MON_FLOWTRACKER_ELEPHANT_PROFILE table is used for configuring
 * elephant trap promotion and demotion filters.
 */

typedef struct bcmcth_mon_flowtracker_elephant_profile_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_ELEPHANT_PROFILEt_FIELD_COUNT);

    /*! Bitmap of Fields to be operated in promotion_filters. */
    SHR_BITDCLNAME(fbmp_pro_filters, FLOWTRACKER_PROMO_FILTER_MAX);

    /*! Index into the MON_FLOWTRACKER_ELEPHANT_PROFILE table. */
    uint32_t mon_flowtracker_elephant_profile_id;

    /*! Number of promotion filters. */
    uint8_t num_promotion_filters;

    /*! Corresponds to SCAN_INTERVAL_USECS field in the MON_FLOWTRACKER_CONTROL LT. */
    uint32_t scan_interval_usecs;

    /*!  Promotion filters to promote a flow to elephant. */
    bcmcth_mon_flowtracker_elephant_filter_t promotion_filters[FLOWTRACKER_PROMO_FILTER_MAX];

    /*! Demotion filter to demote a flow to mouse. */
    bcmcth_mon_flowtracker_elephant_filter_t    demotion_filter;

}bcmcth_mon_flowtracker_elephant_profile_t;

/*!
 * The MON_FLOWTRACKER_EXPORT_TEMPLATE table is used to specify
 * the IPFIX template in which flow information is exported.
 */
typedef struct flowtracker_export_element_info_s {
    /*! The type of the information element. */
    bcmltd_common_flowtracker_export_element_type_t_t type;

    /*! The size of the information element. */
    uint16_t data_size;

    /*!Specifies if element is of type ENTERPRISE. */
    bool    enterprise;

    /*! IANA assigned id for non enterprise element. */
    uint16_t id;
}flowtracker_export_element_info_t;

/*!
 * The MON_FLOWTRACKER_EXPORT_TEMPLATE table is used to specify
 * the IPFIX template in which flow information is exported.
 */
typedef struct bcmcth_mon_ft_export_template_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_EXPORT_TEMPLATEt_FIELD_COUNT);

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp_export_ele, FLOWTRACKER_EXPORT_ELEMENTS_MAX);

    /*! Index into the MON_FLOWTRACKER_EXPORT_TEMPLATE table. */
    uint32_t mon_ft_export_template_id;

    /*! IPFIX set ID for the template. */
    uint16_t set_id;

    /*! Number of export elements in the template. */
    uint32_t num_export_elements;

    /*! The list of export elements in the template. */
    flowtracker_export_element_info_t
        export_elements[FLOWTRACKER_EXPORT_ELEMENTS_MAX];

    /*! Specifies the collector type */
    bcmltd_common_mon_collector_type_t_t  collector_type;

    /*!
     * Index into the MON_COLLECTOR_IPV4 table, if the collector to
     * which the IPFIX template set for this export template needs to
     * be periodically transmitted is an IPV4 collector. Applicable
     * only if COLLECTOR_TYPE == IPV4.
     */
    uint32_t collector_ipv4_id;

    /*!
     * Indicates that the entry corresponding to collector_ipv4_id was found.
     */
    bool collector_ipv4_found;

    /*!
     * Structure containing the fields of the IPv4 collector entry specified by
     * ipv4_collector_id.
     */
    bcmcth_mon_collector_ipv4_t collector_ipv4;

    /*!
     * Index into the MON_COLLECTOR_IPV6 table, if the collector to
     * which the IPFIX template set for this export template needs to
     * be periodically transmitted is an IPV6 collector.Applicable
     * only if COLLECTOR_TYPE == IPV6.
     */
    uint32_t collector_ipv6_id;

    /*!
     * Indicates that the entry corresponding to collector_ipv6_id was found.
     */
    bool collector_ipv6_found;

    /*!
     * Structure containing the fields of the IPv6 collector entry specified by
     * ipv6_collector_id.
     */
    bcmcth_mon_collector_ipv6_t collector_ipv6;

    /*!
     * Interval in seconds at which the IPFIX template set has to be
     * transmitted.
     */
    uint32_t transmit_interval;

    /*!
     * Number of times the IPFIX template set needs to be sent before
     * settling to a periodic export.
     */
    uint32_t initial_burst;

    /*!
     * Read-only field which reflects the operational
     * state of the entry.
     */
    bcmltd_common_mon_flowtracker_export_template_state_t_t oper_status;
} bcmcth_mon_ft_export_template_t;

/*!
 * The MON_FLOWTRACKER_GROUP_COLLECTOR_MAP table is used to
 * provide mapping of flowtracker group to collector, export
 * profile and export template.
 */
typedef struct bcmcth_mon_ft_group_col_map_s {

    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_FIELD_COUNT);

    /*! Index into the MON_FLOWTRACKER_GROUP_COLLECTOR_MAP table. */
    uint32_t mon_ft_group_col_map_id;

    /*! Index into the MON_FLOWTRACKER_GROUP table. */
    uint32_t mon_ft_group_id;

    /*! Collector type. */
    bcmltd_common_mon_collector_type_t_t collector_type;

    /*!
     * Index into the MON_COLLECTOR_IPV4 table, if the collector is
     * an IPV4 collector. Applicable only if COLLECTOR_TYPE == IPV4.
     */
    uint32_t collector_ipv4_id;

    /*!
     * Indicates that the entry corresponding to collector_ipv4_id was found.
     */
    bool collector_ipv4_found;

    /*!
     * Structure containing the fields of the IPv4 collector entry specified by
     * ipv4_collector_id.
     */
    bcmcth_mon_collector_ipv4_t collector_ipv4;

    /*!
     * Index into the MON_COLLECTOR_IPV6 table, if the collector is
     * an IPV6 collector. Applicable only if COLLECTOR_TYPE == IPV6.
     */
    uint32_t collector_ipv6_id;

     /*!
     * Indicates that the entry corresponding to collector_ipv6_id was found.
     */
    bool collector_ipv6_found;

    /*!
     * Structure containing the fields of the IPv6 collector entry specified by
     * ipv6_collector_id.
     */
    bcmcth_mon_collector_ipv6_t collector_ipv6;

    /*! Index into the MON_EXPORT_PROFILE table. */
    uint32_t export_profile_id;

    /*!
     * Indicates that the entry corresponding to export_profile_id was found.
     */
    bool export_profile_found;

    /*!
     * Structure containing the fields of the export profile entry specified by
     * the export_profile_id.
     */
    bcmcth_mon_export_profile_t export_profile;

    /*! Index into the MON_FLOWTRACKER_EXPORT_TEMPLATE table. */
    uint32_t export_template_id;

    /*!
     * Indicates that the entry corresponding to export_profile_template_id was found.
     */
    bool export_template_found;

    /*!
     * Structure containing the fields of the export template entry specified by
     * the export_template_id.
     */
    bcmcth_mon_ft_export_template_t export_template;

    /*!
     * Read-only field which reflects the operational
     * state of the entry.
     */
    bcmltd_common_mon_flowtracker_group_collector_map_state_t_t oper_status;
}bcmcth_mon_ft_group_col_map_t;


/*!
 * \brief MON_FLOWTRACKER_ELEPHANT_PROFILE  LT insert handler.
 *
 * This function handles the insert opcode for the MON_FLOWTRACKER_ELEPHANT_PROFILE
 * LT. This function is called from the lookup callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [out] insert Structure containing the fields to be inserted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_elephant_profile_insert(int unit,
    bcmcth_mon_flowtracker_elephant_profile_t *insert);


/*!
 * \brief MON_FLOWTRACKER_ELEPHANT_PROFILE  LT delete handler.
 *
 * This function handles the delete opcode for the MON_FLOWTRACKER_ELEPHANT_PROFILE
 * LT. This function is called from the lookup callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Structure containing the fields to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_elephant_profile_delete(int unit,
    bcmcth_mon_flowtracker_elephant_profile_t *entry);
/*!
 * \brief MON_FLOWTRACKER_GROUP LT insert handler.
 *
 * This function handles the INSERT opcode for the MON_FLOWTRACKER_GROUP
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] insert Structure containing the fields being inserted into IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_group_insert(int unit,
                           bcmcth_mon_ft_group_t *insert);

/*!
 * \brief MON_FLOWTRACKER_GROUP LT update handler.
 *
 * This function handles the UPDATE opcode for the MON_FLOWTRACKER_GROUP
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure containing the fields that are currently
 *             present in the IMM.
 * \param [in] update Structure containing the fields being updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_group_update(int unit,
                           bcmcth_mon_ft_group_t *cur,
                           bcmcth_mon_ft_group_t *update);

/*!
 * \brief MON_FLOWTRACKER_GROUP LT delete handler.
 *
 * This function handles the DELETE opcode for the MON_FLOWTRACKER_GROUP
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Flow group id to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_group_delete(int unit, uint32_t grp_id);

/*!
 * \brief MON_FLOWTRACKER_EXPORT_TEMPLATE LT insert handler.
 *
 * This function handles the INSERT opcode for the MON_FLOWTRACKER_EXPORT_TEMPLATE
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] insert Structure containing the fields being inserted into IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_export_template_insert(int unit,
                             bcmcth_mon_ft_export_template_t *insert);

/*!
 * \brief MON_FLOWTRACKER_EXPORT_TEMPLATE LT delete handler.
 *
 * This function handles the DELETE opcode for the MON_FLOWTRACKER_EXPORT_TEMPLATE
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] id Export template id to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_export_template_delete(int unit, uint32_t id);

/*!
 * \brief MON_FLOWTRACKER_GROUP_COLLECTOR_MAP LT insert handler.
 *
 * This function handles the INSERT opcode for the MON_FLOWTRACKER_GROUP_COLLECTOR_MAP
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] insert Structure containing the fields being inserted into IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_group_col_map_insert(int unit,
                             bcmcth_mon_ft_group_col_map_t *insert);

/*!
 * \brief MON_FLOWTRACKER_GROUP_COLLECTOR_MAP LT delete handler.
 *
 * This function handles the DELETE opcode for the MON_FLOWTRACKER_GROUP_COLLECTOR_MAP
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure containing the fields being deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_group_col_map_delete(int unit,
                            bcmcth_mon_ft_group_col_map_t *entry);

/*!
 * \brief MON_FLOWTRACKER_EXPORT_TEMPLATE LT update handler.
 *
 * This function handles the UPDATE opcode for the MON_FLOWTRACKER_EXPORT_TEMPLATE
 * LT. This function is called from the stage callback handler for the LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure containing the fields that are currently
 *             present in the IMM.
 * \param [in] update Structure containing the fields being updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_export_template_update(int unit,
                           bcmcth_mon_ft_export_template_t *cur,
                           bcmcth_mon_ft_export_template_t *update);

#endif /* BCMCTH_MON_FLOWTRACKER_DRV_H */
