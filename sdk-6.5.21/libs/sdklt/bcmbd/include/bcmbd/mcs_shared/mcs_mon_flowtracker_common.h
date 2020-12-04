/*! \file mcs_mon_flowtracker_common.h
 *
 * Flowtracker shared definitions
 * Definitions in this filw should be kept in sync with
 * Ukernel file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_MON_FT_COMMON_H
#define MCS_MON_FT_COMMON_H

/*! Max flex counter pools across all devices supporting FT */
#define MCS_SHR_FT_MAX_CTR_POOLS  20

/*! Key Word size */
#define MCS_SHR_FT_EM_KEY_DWORD_SIZE 32

/*! Max number of pipes across all devices */
#define MCS_SHR_FT_MAX_PIPES 8

/*! Max EM key qualifier parts */
#define MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS 8

/*! Max flex counter pools across all devices supporting FT */
#define MCS_SHR_FT_MAX_CTR_POOLS  20

/*! Max flex counter EM actions */
#define MCS_SHR_FT_EM_MAX_ACTIONS 4

/*! Max EM key parts */
#define MCS_SHR_FT_EM_MAX_KEY_PARTS 4

/*! Max length of the system ID */
#define MCS_SHR_FT_PB_SYSTEM_ID_MAX_LENGTH 32

/*! Max size of the IPFIX packet encap */
#define MCS_SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH 128

/*! Max length of the custom key */
#define MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH 16

/*! Max number of drop reasons per flow */
#define MCS_SHR_FT_DROP_REASONS_MAX_COUNT 2

/*! Max size of the template set encap */
#define MCS_SHR_FT_MAX_TEMPLATE_SET_ENCAP_LENGTH 64

/*! Maximum number of filters (promotion + demotion) in an elephant profile */
#define MCS_SHR_FT_ELPH_MAX_FILTERS 3

/*! Max timeout for msging between SDK and UKERNEL */
#define MCS_SHR_FT_MAX_UC_MSG_TIMEOUT 5000000 /*! 5 secs */

/*! Max UFT banks */
#define MCS_SHR_FT_MAX_UFT_BANKS 4

/*! Enable flow start timestamp storing */
#define MCS_SHR_FT_MSG_INIT_FLAGS_FLOW_START_TS (1 << 0)

/*! Enable Elephant monitoring */
#define MCS_SHR_FT_MSG_INIT_FLAGS_ELEPHANT (1 << 1)

/*! Enable drop monitoring */
#define MCS_SHR_FT_MSG_INIT_FLAGS_DROP_MONITOR (1 << 2)

/*! Enable Host Memory Access  */
#define MCS_SHR_FT_MSG_INIT_FLAGS_HOSTMEM_ENABLE (1 << 3)

/*! Enable Queue Congestion monitoring */
#define MCS_SHR_FT_MSG_INIT_FLAGS_CONGESTION_MONITOR (1 << 4)

/*! QOS profile mask within EM_FT_OPAQUE_OBJ0 */
#define MCS_SHR_FT_QOS_PROFILE_MASK 0x3

/*! Group IDX mask within EM_FT_OPAQUE_OBJ0 */
#define MCS_SHR_FT_GROUP_IDX_MASK 0x3ff0

/*! Group IDX offset within EM_FT_OPAQUE_OBJ0 */
#define MCS_SHR_FT_GROUP_IDX_OFFSET 4

/*! Max number of hit bit banks - Single, double, quad */
#define MCS_SHR_FT_NUM_HITBIT_BANKS_MAX 3

/*! Max number of em groups */
#define MCS_SHR_FT_MAX_EM_GROUPS 4

/*! Max number of RXPMD flex profiles - maximum for all chips combined */
#define MCS_FT_RXPMD_FLEX_PROFILES_COUNT 128

/*! RXPMD fields of interest for Flowtracker */
typedef enum mcs_shr_ft_rxpmd_flex_field_e {
    /*! I2E class ID field in the flex data */
   MCS_FT_RXPMD_FLEX_FIELD_I2E_CLASS_ID_15_0 = 0,

   /*! Last value, used only for counting. */
   MCS_FT_RXPMD_FLEX_FIELD_COUNT
}mcs_shr_ft_rxpmd_flex_field_t;

/*! Supported qualifiers in EM key */
typedef enum mcs_shr_ft_em_qual_e {
    MCS_SHR_FT_EM_QUAL_SRC_IPV4              = 0,
    MCS_SHR_FT_EM_QUAL_DST_IPV4              = 1,
    MCS_SHR_FT_EM_QUAL_L4_SRC_PORT           = 2,
    MCS_SHR_FT_EM_QUAL_L4_DST_PORT           = 3,
    MCS_SHR_FT_EM_QUAL_IP_PROTOCOL           = 4,
    MCS_SHR_FT_EM_QUAL_VNID                  = 5,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK0            = 6,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK1            = 7,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK2            = 8,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK3            = 9,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK4            = 10,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK5            = 11,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK6            = 12,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK7            = 13,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK8            = 14,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK9            = 15,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK10           = 16,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK11           = 17,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK12           = 18,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK13           = 19,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK14           = 20,
    MCS_SHR_FT_EM_QUAL_UDF_CHUNK15           = 21,
    MCS_SHR_FT_EM_QUAL_LT_ID                 = 22,
    MCS_SHR_FT_EM_QUAL_IN_PORT               = 23,
    MCS_SHR_FT_EM_QUAL_INNER_SRC_IPV4        = 24,
    MCS_SHR_FT_EM_QUAL_INNER_DST_IPV4        = 25,
    MCS_SHR_FT_EM_QUAL_INNER_SRC_IPV6_PART0  = 26,
    MCS_SHR_FT_EM_QUAL_INNER_SRC_IPV6_PART1  = 27,
    MCS_SHR_FT_EM_QUAL_INNER_SRC_IPV6_PART2  = 28,
    MCS_SHR_FT_EM_QUAL_INNER_SRC_IPV6_PART3  = 29,
    MCS_SHR_FT_EM_QUAL_INNER_DST_IPV6_PART0  = 30,
    MCS_SHR_FT_EM_QUAL_INNER_DST_IPV6_PART1  = 31,
    MCS_SHR_FT_EM_QUAL_INNER_DST_IPV6_PART2  = 32,
    MCS_SHR_FT_EM_QUAL_INNER_DST_IPV6_PART3  = 33,
    MCS_SHR_FT_EM_QUAL_INNER_L4_SRC_PORT     = 34,
    MCS_SHR_FT_EM_QUAL_INNER_L4_DST_PORT     = 35,
    MCS_SHR_FT_EM_QUAL_INNER_IP_PROTOCOL     = 36,
    MCS_SHR_FT_EM_MAX_QUAL
} mcs_shr_ft_em_qual_e;

/*! Supported tracking params */
typedef enum mcs_shr_ft_tp_e {
    MCS_SHR_FT_TP_SRC_IPV4             = 0,
    MCS_SHR_FT_TP_DST_IPV4             = 1,
    MCS_SHR_FT_TP_L4_SRC_PORT          = 2,
    MCS_SHR_FT_TP_L4_DST_PORT          = 3,
    MCS_SHR_FT_TP_IP_PROTOCOL          = 4,
    MCS_SHR_FT_TP_VNID                 = 5,
    MCS_SHR_FT_TP_CUSTOM               = 6,
    MCS_SHR_FT_TP_INNER_SRC_IPV4       = 7,
    MCS_SHR_FT_TP_INNER_DST_IPV4       = 8,
    MCS_SHR_FT_TP_INNER_L4_SRC_PORT    = 9,
    MCS_SHR_FT_TP_INNER_L4_DST_PORT    = 10,
    MCS_SHR_FT_TP_INNER_IP_PROTOCOL    = 11,
    MCS_SHR_FT_TP_IN_PORT              = 12,
    MCS_SHR_FT_TP_INNER_SRC_IPV6       = 13,
    MCS_SHR_FT_TP_INNER_DST_IPV6       = 14,
    MCS_SHR_FT_MAX_TP
} mcs_shr_ft_tp_t;

/*! List of supported Info elements that can be exported by app */
typedef enum mcs_shr_ft_template_info_elem_e {
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_SRC_IPV4           = 0,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_DST_IPV4           = 1,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT        = 2,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_L4_DST_PORT        = 3,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL        = 4,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT    = 5,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT   = 6,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC = 7,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC        = 8,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_GROUP_ID           = 9,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_VNID               = 10,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_CUSTOM             = 11,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4     = 12,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4     = 13,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT  = 14,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT  = 15,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL  = 16,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6     = 17,
    MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6     = 18,
    MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS
} mcs_shr_ft_template_info_elem_t;

/*! Learn status */
typedef enum mcs_shr_ft_learn_pkt_status_e {
    MCS_SHR_FT_LEARN_PKT_NOT_READY             = 0,
    MCS_SHR_FT_LEARN_PKT_RUNT                  = 1,
    MCS_SHR_FT_LEARN_PKT_NON_IP                = 2,
    MCS_SHR_FT_LEARN_PKT_NON_IPV4              = 3,
    MCS_SHR_FT_LEARN_PKT_INVALID_GROUP         = 4,
    MCS_SHR_FT_LEARN_PKT_GROUP_NOT_ENABLED     = 5,
    MCS_SHR_FT_LEARN_PKT_GROUP_THRESHOLD       = 6,
    MCS_SHR_FT_LEARN_PKT_PIPE_THRESHOLD        = 7,
    MCS_SHR_FT_LEARN_PKT_EM_LOOKUP_SCHAN_FAIL  = 8,
    MCS_SHR_FT_LEARN_PKT_DUPLICATE             = 9,
    MCS_SHR_FT_LEARN_PKT_NEW_FLOW              = 10,
    MCS_SHR_FT_LEARN_PKT_EM_INSERT_FAIL        = 11,
    MCS_SHR_FT_LEARN_PKT_UNKNOWN_PIPE          = 12,
    MCS_SHR_FT_LEARN_PKT_EM_GROUP_NOT_ATTACHED = 13,
    MCS_SHR_FT_LEARN_PKT_PARSE_ERROR           = 14,
    MCS_SHR_FT_LEARN_PKT_DROP_MONITOR_EVENT    = 15,
    MCS_SHR_FT_LEARN_PKT_DEFAULT_GROUP         = 16,
    MCS_SHR_FT_LEARN_PKT_DROP_NO_CHANGE        = 17,
    MCS_SHR_FT_LEARN_PKT_MAX_STATUS
} mcs_shr_ft_learn_pkt_status_t;

/*!
 * Flow states
 */
typedef enum mcs_shr_ft_elph_flow_state_e {
    MCS_SHR_FT_ELPH_FLOW_STATE_NEW           = 0,
    MCS_SHR_FT_ELPH_FLOW_STATE_UNDETERMINED  = 1,
    MCS_SHR_FT_ELPH_FLOW_STATE_MOUSE         = 2,
    MCS_SHR_FT_ELPH_FLOW_STATE_CANDIDATE     = 3,
    MCS_SHR_FT_ELPH_FLOW_STATE_ELEPHANT      = 4,
    MCS_SHR_FT_ELPH_FLOW_STATE_COUNT
} mcs_shr_ft_elph_flow_state_t;

/*! Elephant monitor actions */
typedef enum mcs_shr_ft_elph_action_e {
    MCS_SHR_FT_ELPH_ACTION_NONE         = 0,
    MCS_SHR_FT_ELPH_ACTION_PROMOTE      = 1,
    MCS_SHR_FT_ELPH_ACTION_DEMOTE       = 2,
    MCS_SHR_FT_ELPH_ACTION_COUNT
} mcs_shr_ft_elph_action_t;

/*!
 * FT uC Error codes
 */
typedef enum mcs_shr_ft_uc_error_e {
    MCS_SHR_FT_UC_E_NONE      = 0,
    MCS_SHR_FT_UC_E_INTERNAL  = 1,
    MCS_SHR_FT_UC_E_MEMORY    = 2,
    MCS_SHR_FT_UC_E_UNIT      = 3,
    MCS_SHR_FT_UC_E_PARAM     = 4,
    MCS_SHR_FT_UC_E_EMPTY     = 5,
    MCS_SHR_FT_UC_E_FULL      = 6,
    MCS_SHR_FT_UC_E_NOT_FOUND = 7,
    MCS_SHR_FT_UC_E_EXISTS    = 8,
    MCS_SHR_FT_UC_E_TIMEOUT   = 9,
    MCS_SHR_FT_UC_E_BUSY      = 10,
    MCS_SHR_FT_UC_E_FAIL      = 11,
    MCS_SHR_FT_UC_E_DISABLED  = 12,
    MCS_SHR_FT_UC_E_BADID     = 13,
    MCS_SHR_FT_UC_E_RESOURCE  = 14,
    MCS_SHR_FT_UC_E_CONFIG    = 15,
    MCS_SHR_FT_UC_E_UNAVAIL   = 16,
    MCS_SHR_FT_UC_E_INIT      = 17,
    MCS_SHR_FT_UC_E_PORT      = 18
} mcs_shr_ft_uc_error_t;

/*! Group update params */
typedef enum mcs_shr_ft_group_update_e {
    MCS_SHR_FT_GROUP_UPDATE_INVALID                    = 0,
    MCS_SHR_FT_GROUP_UPDATE_FLOW_THRESHOLD             = 1,
    MCS_SHR_FT_GROUP_UPDATE_FLUSH                      = 2,
    MCS_SHR_FT_GROUP_UPDATE_COUNTER_CLEAR              = 3,
    MCS_SHR_FT_GROUP_UPDATE_OBSERVATION_DOMAIN         = 4,
    MCS_SHR_FT_GROUP_UPDATE_AGING_INTERVAL             = 5,
    MCS_SHR_FT_GROUP_UPDATE_TEMPLATE_COLLECTOR_SET     = 6,
    MCS_SHR_FT_GROUP_UPDATE_TEMPLATE_COLLECTOR_UNSET   = 7,
    MCS_SHR_FT_GROUP_UPDATE_ELPH_PROFILE               = 8,
    MCS_SHR_FT_GROUP_UPDATE_ELPH_QOS_PROFILE_ID        = 9,
    MCS_SHR_FT_GROUP_UPDATE_EM_GROUP_IDX               = 10,
    MCS_SHR_FT_GROUP_UPDATE_EXPORT_TRIGGERS            = 11,
    MCS_SHR_FT_GROUP_UPDATE_DEFAULT_GROUP              = 12,
    MCS_SHR_FT_GROUP_UPDATE_TEMPLATE_SET               = 13,
    MCS_SHR_FT_GROUP_UPDATE_TEMPLATE_UNSET             = 14,
    MCS_SHR_FT_GROUP_UPDATE_COLLECTOR_SET              = 15,
    MCS_SHR_FT_GROUP_UPDATE_COLLECTOR_UNSET            = 16,
    MCS_SHR_FT_GROUP_UPDATE_COUNT
} mcs_shr_ft_group_update_t;

/*! Features supported by uC */

/*! Elephant profile feature */
#define MCS_SHR_FT_UC_FEATURE_ELEPH                      (1 << 0)

/*! IPFIX export feature */
#define MCS_SHR_FT_UC_FEATURE_IPFIX_EXPORT              (1 << 1)

/*! Protobuf export feature */
#define MCS_SHR_FT_UC_FEATURE_PROTOBUF_EXPORT           (1 << 2)

/*! Drop monitor feature */
#define MCS_SHR_FT_UC_FEATURE_DROP_MONITOR              (1 << 3)

/*! FSP reinject feature */
#define MCS_SHR_FT_UC_FEATURE_FSP_REINJECT              (1 << 4)

/*! Export trigger new learn feature */
#define MCS_SHR_FT_UC_FEATURE_EXPORT_TRIGGER_NEW_LEARN  (1 << 5)

/*! Export trigger age out feature */
#define MCS_SHR_FT_UC_FEATURE_EXPORT_TRIGGER_AGE_OUT    (1 << 6)

/*! Export trigger drop feature */
#define MCS_SHR_FT_UC_FEATURE_EXPORT_TRIGGER_DROP       (1 << 7)

/*! Export interval change feature */
#define MCS_SHR_FT_UC_FEATURE_EXPORT_INTERVAL_CHANGE    (1 << 8)

/*! Export Triggers : periodic learn */
#define MCS_SHR_FT_EXPORT_TRIGGER_PERIODIC  (1 << 0)

/*! Export Triggers : new flow learn */
#define MCS_SHR_FT_EXPORT_TRIGGER_NEW_LEARN (1 << 1)

/*! Export Triggers : age out */
#define MCS_SHR_FT_EXPORT_TRIGGER_AGE_OUT   (1 << 2)

/*! Export Triggers :drop */
#define MCS_SHR_FT_EXPORT_TRIGGER_DROP      (1 << 3)

/*! Max number of counter pools per hitbit counter bank */
#define MCS_SHR_FT_MAX_NUM_CTR_POOLS_PER_HITBIT_BANK 2

/*! Max number of hitbit counter banks */
#define MCS_SHR_FT_MAX_NUM_HITBIT_BANKS 3

/*! Hit bit type - Single base entry wide EM entry */
#define MCS_SHR_FT_HITBIT_TYPE_1x 0

/*! Hit bit type - Double base entry wide EM entry */
#define MCS_SHR_FT_HITBIT_TYPE_2x 1

/*! Hit bit type - Quad base entry wide EM entry */
#define MCS_SHR_FT_HITBIT_TYPE_4x 2


/*! Flowtracker config modes */
typedef enum mcs_shr_ft_cfg_modes_e {
    /*!
     * V1 - Implicit EM group creation
     *    - Dedicated collector APIs
     *    - Fixed IPv4 5-tuple tracking
     *    - Only IPFIX export
     */
    MCS_SHR_FT_CFG_MODE_V1 = 1,

    /*!
     * V2 - EM group creation by customer
     *    - Use common collector infra
     *    - Support IPFIX & protoBuf based export
     *    - Flexible tracking params
     */
    MCS_SHR_FT_CFG_MODE_V2 = 2,

    MCS_SHR_FT_CFG_MODE_COUNT
} mcs_shr_ft_cfg_modes_t;

/*! EM actions supported */
typedef enum mcs_shr_ft_em_action_e {
 SHR_FT_EM_ACTION_CTR          = 0,
 SHR_FT_EM_ACTION_OPAQUE_OBJ0  = 1,
 SHR_FT_EM_ACTION_METER        = 2,
 SHR_FT_EM_ACTION_QOS_PROFILE  = 3,
 SHR_FT_EM_ACTION_IFA_TRIGGER  = 4,
 SHR_FT_EM_ACTION_EM_HIT       = 5
}mcs_shr_ft_em_action_t;

/*! Supported UDF base offsets */
typedef enum mcs_shr_ft_udf_base_offset_e {
    MCS_SHR_FT_UDF_BASE_OFFSET_INVALID   = 0,
    MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L2  = 1,
    MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L3  = 2,
    MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L4  = 3,
    MCS_SHR_FT_UDF_BASE_OFFSET_OUTER_L5  = 4,
    MCS_SHR_FT_UDF_BASE_OFFSET_MAX
} mcs_shr_ft_udf_base_offset_t;

/*! Support EM entry hitbit types */
typedef enum mcs_shr_ft_hitbit_type_e {
    MCS_SHR_FT_HIT_BIT_TYPE_NONE   = 0,
    MCS_SHR_FT_HIT_BIT_TYPE_1x     = 1,
    MCS_SHR_FT_HIT_BIT_TYPE_START  = MCS_SHR_FT_HIT_BIT_TYPE_1x,
    MCS_SHR_FT_HIT_BIT_TYPE_2x     = 2,
    MCS_SHR_FT_HIT_BIT_TYPE_4x     = 3,
    MCS_SHR_FT_HIT_BIT_TYPE_END    = MCS_SHR_FT_HIT_BIT_TYPE_4x
}mcs_shr_ft_hitbit_type_t;

/*! Hitbit counter banks information passed to UKERNEL */
typedef struct mcs_shr_ft_hitbit_ctr_bank_info_s {
    /*! Indicates if the hit bit type is 1x, 2x or 4x type */
    uint8_t hitbit_type;
    /*! Number of counter pools for a hitbit bank */
    uint8_t num_ctr_pools;
    /*! Ctr bank's base index.This is the first index in the first pool */
    uint16_t ctr_bank_base_idx;
    /*! Ctr pool IDs */
    uint8_t ctr_pool_id[MCS_SHR_FT_MAX_NUM_CTR_POOLS_PER_HITBIT_BANK];
    /*! Ctr pool start index */
    uint16_t ctr_pool_start_idx[MCS_SHR_FT_MAX_NUM_CTR_POOLS_PER_HITBIT_BANK];
    /*! Num ctrs per pool */
    uint16_t ctr_pool_size[MCS_SHR_FT_MAX_NUM_CTR_POOLS_PER_HITBIT_BANK];
}mcs_shr_ft_hitbit_ctr_bank_info_t;

/*! Hash bucket mode */
typedef enum mcs_shr_ft_hash_bm_e {
    /* Hash bucket mode None */
    MCS_SHR_FT_HASH_BM_NONE,

    /* Hash bucket mode 0
     * Single base entry width.
     */
    MCS_SHR_FT_HASH_BM_0,

    /* Hash bucket mode 1
     * Single base entry width.
     */
    MCS_SHR_FT_HASH_BM_1,

    /* Hash bucket mode 2
     * Single base entry width.
     */
    MCS_SHR_FT_HASH_BM_2

}mcs_shr_ft_hash_bm_t;

/*! Match ID fields used by FT EApp. */
typedef enum mcs_shr_ft_match_id_fields_s {
    /*! Outer vlan tag. */
    MCS_SHR_FT_MATCH_ID_OUTER_L2_OTAG = 0,

    /*! Outer vlan tag. */
    MCS_SHR_FT_MATCH_ID_OUTER_L2_ITAG = 1,

    /*! Outer IPv4. */
    MCS_SHR_FT_MATCH_ID_OUTER_L3_IPV4 = 2,

    /*! Outer IPv6. */
    MCS_SHR_FT_MATCH_ID_OUTER_L3_IPV6 = 3,

    /*! VxLAN. */
    MCS_SHR_FT_MATCH_ID_VXLAN = 4,

    /*! Inner vlan tag. */
    MCS_SHR_FT_MATCH_ID_INNER_L2_OTAG = 5,

    /*! Inner vlan tag. */
    MCS_SHR_FT_MATCH_ID_INNER_L2_ITAG = 6,

    /*! Outer IPv4. */
    MCS_SHR_FT_MATCH_ID_INNER_L3_IPV4 = 7,

    /*! Outer IPv6. */
    MCS_SHR_FT_MATCH_ID_INNER_L3_IPV6 = 8,

    /*! Last value, not usable. */
    MCS_SHR_FT_MATCH_ID_FIELD_COUNT
} mcs_shr_ft_match_id_fields_t;

#endif /* MCS_MON_FT_COMMON_H */
