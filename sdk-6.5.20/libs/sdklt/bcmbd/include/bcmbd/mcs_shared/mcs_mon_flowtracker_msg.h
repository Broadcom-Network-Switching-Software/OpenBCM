/*! \file mcs_mon_flowtracker_msg.h
 *
 * Flowtracker shared  messaging definitions
 * Definitions and structures defined here should be
 * kept in sync with ukernel files.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_MON_FLOWTRACKER_MSG_H
#define MCS_MON_FLOWTRACKER_MSG_H

#include <bcmbd/mcs_shared/mcs_mon_flowtracker_common.h>

/*! Base messaging version */
#define FT_MSG_BASE_VERSION  0x0

/*!
 * Enhanced shr_ft_msg_ctrl_group_flow_data_get_t msg to include:
 *   - inner_src_ip
 *   - inner_dst_ip
 *   - vnid
 *   - inner_l4_src_port
 *   - inner_l4_dst_port
 *   - inner_ip_protocol
 *   - custom_length
 *   - custom[]
 */
/*!
 * Enhanced shr_ft_msg_ctrl_group_flow_data_get_reply_t to add:
 *   - drop_reasons[]
 *   - num_drop_reasons
 */
/*!
 * Enhanced shr_ft_msg_ctrl_init_t to include:
 *    - fsp_reinject_max_length
 */
#define FT_MSG_VERSION_1 1

/*!
 * Enhanced shr_ft_msg_ctrl_group_flow_data_get_t to add:
 *   - in_port
 */
#define FT_MSG_VERSION_2 2

/*!
 * Enhanced mcs_ft_msg_ctrl_init_s to add:
 *  - flex_ctr_action_idx
 */
#define FT_MSG_VERSION_3 3

/*!
 * Enhanced mcs_ft_msg_ctrl_init_s to add:
 *  - hw learn
 */
#define FT_MSG_VERSION_4 4

/*!
 * Enhanced multiple messages to support:
 *  - Multiple EM groups.
 *  - Inner IPv6 EM keys.
 */
#define FT_MSG_VERSION_5 5

/*! Current messaging version */
#define FT_MSG_VERSION  FT_MSG_VERSION_5

/*!
 * Per pipe info used for FT Initialization control message
 */
typedef struct mcs_ft_msg_ctrl_init_pipe_s {
    /*! Max flows */
    uint32_t max_flows;

    /*! Size of reserved pools */
    uint16_t ctr_pool_size[MCS_SHR_FT_MAX_CTR_POOLS];

    /*! Reserved Flex counter pools per pipe */
    uint8_t ctr_pools[MCS_SHR_FT_MAX_CTR_POOLS];

    /*! Number of reserved counter pools */
    uint8_t num_ctr_pools;

} mcs_ft_msg_ctrl_init_pipe_t;
/*!
 * Flowtracker intialization control message.
 */
typedef struct mcs_ft_msg_ctrl_init_s {
    /*! Initialization flags */
    uint32_t flags;

    /*! Max number of groups */
    uint32_t max_groups;

    /*! Current time in seconds */
    uint32_t cur_time_secs;

    /*! Export interval in msec */
    uint32_t export_interval_msecs;

    /*! Scan interval in usec */
    uint32_t scan_interval_usecs;

    /*! Aging timer tick in msec */
    uint32_t age_timer_tick_msecs;

    /*! Per H/w pipe info */
    mcs_ft_msg_ctrl_init_pipe_t pipe[MCS_SHR_FT_MAX_PIPES];

    /*! Maximum length of the transmitted packet */
    uint16_t max_export_pkt_length;

    /*! FSP re-inject max length */
    uint16_t fsp_reinject_max_length;

    /*! Num ports in the device */
    uint8_t num_pipes;

    /*! Number of elephant profiles */
    uint8_t num_elph_profiles;

    /*!
     * Flex counter action index.
     * Valid only on FC2.0 platforms.
     */
    uint8_t flex_ctr_action_idx;

    /*!
     * Hw learn enable/disable.
     */
    uint8_t hw_learn;
} mcs_ft_msg_ctrl_init_t;


/*!
 * EM key format export
 */
typedef struct mcs_ft_msg_ctrl_em_qual_s {
    /*! Offsets into the key */
    uint16_t offset[MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /*! Width of qualifiers */
    uint8_t  width[MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /*! Num offsets of the qualifier */
    uint8_t  num_parts;
} mcs_ft_msg_ctrl_qual_parts_t;

/*!
 * EM key format
 */
typedef struct mcs_ft_msg_ctrl_em_key_format_s {
    /*! Offsets and width of each qual */
    mcs_ft_msg_ctrl_qual_parts_t qual_parts[MCS_SHR_FT_EM_MAX_QUAL];

    /*! Size of the key in bits */
    uint16_t key_size;

    /*! Qualifiers */
    uint8_t qual[MCS_SHR_FT_EM_MAX_QUAL];

    /*! Number of qualifiers */
    uint8_t num_qual;
}mcs_ft_msg_ctrl_em_key_format_t;


/*!
 * EM qualifier info
 */
typedef struct mcs_ft_msg_ctrl_qual_info_s {
    /*! Width of the qualifier in bytes */
    uint16_t width;

    /*! Offsets of each key part */
    uint16_t part_offset[MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /*! Width of each qualifier part */
    uint8_t part_width[MCS_SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /*! Qualifier */
    uint8_t qual;

    /*! Num offsets of the qualifier */
    uint8_t num_parts;

    /*! Base offset type of UDF */
    uint8_t base_offset;

    /*! Relative offset in bytes of this chunk in the pkt */
    uint8_t relative_offset;

} mcs_ft_msg_ctrl_qual_info_t;

/*!
 * Tracking param info
 */
typedef struct mcs_ft_msg_ctrl_tp_info_s {
    /*! Tracking param */
    uint8_t param;

    /*! Number of qualifiers that make up this param */
    uint8_t num_qual;

    /*! Qualifiers */
    uint8_t qual[MCS_SHR_FT_EM_MAX_QUAL];

    /*! UDF or fixed qualifier */
    uint8_t udf;
} mcs_ft_msg_ctrl_tp_info_t;

/*!
 * EM group create
 */
typedef struct mcs_ft_msg_ctrl_em_group_create_s {
    /*! Offsets and width of each qual */
    mcs_ft_msg_ctrl_qual_info_t qual_info[MCS_SHR_FT_EM_MAX_QUAL];

    /*! Tracking param info */
    mcs_ft_msg_ctrl_tp_info_t tp_info[MCS_SHR_FT_MAX_TP];

    /*! Action set bitmap */
    uint32_t aset;

    /*! Size of the key in bits */
    uint16_t key_size;

    /*! EM group index */
    uint8_t group_idx;

    /*! EM LT ID */
    uint8_t lt_id[MCS_SHR_FT_MAX_PIPES];

    /*! Number of pipes in the device */
    uint8_t num_pipes;

    /*! Number of qualifiers */
    uint8_t num_qual;

    /*! Number of tracking params */
    uint8_t num_tp;
}mcs_ft_msg_ctrl_em_group_create_t;

/*!
 * Flex EM group info
 */
typedef struct mcs_ft_msg_ctrl_flex_em_group_create_s {
    /*! Key type - This is the index to the KEY_ATTRIBUTES table */
    uint8_t key_type;

    /*! Number of key parts - Number of base entries that key covers */
    uint8_t num_key_parts;

    /*! Key start offsets within each base entry */
    uint16_t key_start_offset[MCS_SHR_FT_EM_MAX_KEY_PARTS];

    /*! Key start offsets within each base entry */
    uint16_t key_end_offset[MCS_SHR_FT_EM_MAX_KEY_PARTS];

    /*!  Bucket mode for the key */
    uint8_t bucket_mode;

    /*! Action data start offset */
    uint16_t action_start_offset;

    /*! Action data end offset */
    uint16_t action_end_offset;

    /*! Offsets and width of each qual */
    mcs_ft_msg_ctrl_qual_info_t qual_info[MCS_SHR_FT_EM_MAX_QUAL];

    /*! Tracking param info */
    mcs_ft_msg_ctrl_tp_info_t tp_info[MCS_SHR_FT_MAX_TP];

    /*! Num actions */
    uint8_t num_actions;

    /*! Action set array*/
    uint8_t aset[MCS_SHR_FT_EM_MAX_ACTIONS];

    /*! Action set offsets within action data */
    uint16_t aset_offsets[MCS_SHR_FT_EM_MAX_ACTIONS];

    /*! Action set widths within action data */
    uint16_t aset_widths[MCS_SHR_FT_EM_MAX_ACTIONS];

    /*! Size of the key in bits */
    uint16_t key_size;

    /*! EM group index */
    uint8_t group_idx;

    /*! EM LT ID */
    uint8_t lt_id[MCS_SHR_FT_MAX_PIPES];

    /*! Number of pipes in the device */
    uint8_t num_pipes;

    /*! Number of qualifiers */
    uint8_t num_qual;

    /*! Number of tracking params */
    uint8_t num_tp;

    /*! Number of base entries that make up one entry. */
    uint8_t num_base_entries;
} mcs_ft_msg_ctrl_flex_em_group_create_t;

/*!
 * FT Group Create
 */
typedef struct mcs_ft_msg_ctrl_group_create_s {
    /*! Flags */
    uint32_t flags;

    /*! Max flows that can be learned on this group */
    uint32_t  flow_limit;

    /*! Aging interval in msecs */
    uint32_t aging_interval_msecs;

    /*! IPFIX domain id */
    uint32_t domain_id;

    /*! Group Index */
    uint16_t  group_idx;
} mcs_ft_msg_ctrl_group_create_t;

/*!
 * FT group get
 */
typedef struct mcs_ft_msg_ctrl_group_get_s {
    /*! Flags */
    uint32_t flags;

    /*! Max flows that can be learned on this group */
    uint32_t flow_limit;

    /*! Num flows that learnt on this group */
    uint32_t flow_count;

    /*! Aging interval in msecs */
    uint32_t aging_interval;

    /*! IPFIX domain id */
    uint32_t domain_id;

    /*! Group Index */
    uint16_t group_idx;

    /*! Elephant profile Id */
    uint16_t elph_profile_id;

    /*! QoS profile Id */
    uint16_t qos_profile_id;

    /*! Collector Id */
    uint16_t collector_id;

    /*! Template Id */
    uint16_t template_id;

} mcs_ft_msg_ctrl_group_get_t;

/*!
 * FT group update
 */
typedef struct mcs_ft_msg_ctrl_group_update_s {
    /*! Flags */
    uint32_t  flags;

    /*! Indicates what is being updated */
    uint32_t  update;

    /*! Param value depending on the update */
    uint32_t  param0;

    /*! Group Index */
    uint16_t group_idx;
} mcs_ft_msg_ctrl_group_update_t;


/*!
 * Create IPFIX template
 */
typedef struct mcs_ft_msg_ctrl_template_create_s {
    /*! Flags */
    uint32_t flags;

    /*! Software handle for template */
    uint16_t id;

    /*! IPFIX set id */
    uint16_t set_id;

    /*! Number of information elements */
    uint8_t num_info_elems;

    /*! Information elements in the order they are to be exported */
    uint8_t info_elems[MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];

    /*! Size in bytes of the info elems */
    uint16_t data_size[MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} mcs_ft_msg_ctrl_template_create_t;

/*!
 * Get IPFIX template
 */
typedef struct mcs_ft_msg_ctrl_template_get_s {
    /*! Flags */
    uint32_t flags;

    /*! Software handle for template */
    uint16_t id;

    /*! IPFIX set id */
    uint16_t set_id;

    /*! Number of information elements */
    uint8_t num_info_elems;

    /*! Information elements in the order they are to be exported */
    uint8_t info_elems[MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];

    /*! Size in bytes of the info elems */
    uint16_t data_size[MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} mcs_ft_msg_ctrl_template_get_t;

/*!
 * Create  collector
 */
typedef struct mcs_ft_msg_ctrl_collector_create_s {
    /*! Flags */
    uint32_t flags;

    /*! Checksum of IP header excluding length */
    uint32_t ip_base_checksum;

    /*! UDP psuedo header checksum */
    uint32_t udp_base_checksum;

    /*! Component Id for Protobuf export */
    uint32_t component_id;

    /*! Software handle for collector */
    uint16_t id;

    /*! Length of the IPFIX encapsulation */
    uint16_t encap_length;

    /*! Max size of the packet that can be sent to collector */
    uint16_t mtu;

    /*! Offset to start of IP header in the encap */
    uint16_t ip_offset;

    /*! Offset to start of UDP header in the encap */
    uint16_t udp_offset;

    /*! Length of the system Id used in Protobuf export */
    uint16_t system_id_length;

    /*! System Id name */
    uint8_t system_id[MCS_SHR_FT_PB_SYSTEM_ID_MAX_LENGTH];

    /*! Collector encapsulation */
    uint8_t encap[MCS_SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH];
} mcs_ft_msg_ctrl_collector_create_t;

/*!
 * Get collector
 */
typedef struct mcs_ft_msg_ctrl_collector_get_s {
    /*! Flags */
    uint32_t flags;

    /*! Checksum of IP header excluding length */
    uint32_t ip_base_checksum;

    /*! UDP psuedo header checksum */
    uint32_t udp_base_checksum;

    /*! Component Id for Protobuf export */
    uint32_t component_id;

    /*! Software handle for collector */
    uint16_t id;

    /*! Length of the IPFIX encapsulation */
    uint16_t encap_length;

    /*! Collector encapsulation */
    uint16_t encap[MCS_SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH];

    /*! Max size of the packet that can be sent to collector */
    uint16_t mtu;

    /*! Offset to start of IP header in the encap */
    uint16_t ip_offset;

    /*! Length of the system Id used in Protobuf export */
    uint16_t system_id_length;

    /*! System Id name */
    uint8_t system_id[MCS_SHR_FT_PB_SYSTEM_ID_MAX_LENGTH];

    /*! Offset to start of UDP header in the encap */
    uint16_t udp_offset;
} mcs_ft_msg_ctrl_collector_get_t;

/*!
 * Get data associated with a flow
 * Used for MCS_FT_GROUP_FLOW_DATA_SET, MCS_FT_SET_STATIC_FLOW
 */
typedef struct mcs_ft_msg_ctrl_group_flow_data_get_s {
    /*! Source IP */
    uint32_t src_ip;

    /*! Destination IP */
    uint32_t dst_ip;

    /*! Source IP */
    uint32_t inner_src_ip;

    /*! Destination IP */
    uint32_t inner_dst_ip;

    /*! VNID */
    uint32_t vnid;

    /*! Group Index */
    uint16_t group_idx;

    /*! L4 source port */
    uint16_t l4_src_port;

    /*! L4 destination port */
    uint16_t l4_dst_port;

    /*! L4 source port */
    uint16_t inner_l4_src_port;

    /*! L4 destination port */
    uint16_t inner_l4_dst_port;

    /*! IP protocol */
    uint8_t ip_protocol;

    /*! IP protocol */
    uint8_t inner_ip_protocol;

    /*! Custom key length */
    uint8_t custom_length;

    /*! Custom */
    uint8_t custom[MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH];

    /*! In port */
    uint16_t in_port;

    /*! Inner source IPv6 address. */
    uint8_t inner_src_ipv6[16];

    /*! Inner destination IPv6 address. */
    uint8_t inner_dst_ipv6[16];
} mcs_ft_msg_ctrl_group_flow_data_get_t;

/*!
 * Data associated with a flow
 */
typedef struct mcs_ft_msg_ctrl_group_flow_data_get_reply_s {
    /*! Upper 32 bits of the packet counter */
    uint32_t pkt_count_upper;

    /*! Lower 32 bits of the packet counter */
    uint32_t pkt_count_lower;

    /*! Upper 32 bits of the byte counter */
    uint32_t byte_count_upper;

    /*! Lower 32 bits of the byte counter */
    uint32_t byte_count_lower;

    /*! Upper 32 bits of the flow start timestamp */
    uint32_t flow_start_ts_upper;

    /*! Lower 32 bits of the flow start timestamp */
    uint32_t flow_start_ts_lower;

    /*! Upper 32 bits of the Observation timestamp */
    uint32_t obs_ts_upper;

    /*! Lower 32 bits of the Observation timestamp */
    uint32_t obs_ts_lower;

    /*! Drop reasons  */
    uint16_t drop_reasons[MCS_SHR_FT_DROP_REASONS_MAX_COUNT];

    /*! Number of drop reasons */
    uint8_t num_drop_reasons;

    /*! Is the flow static ? */
    uint8_t is_static;
} mcs_ft_msg_ctrl_group_flow_data_get_reply_t;

/*!
 * FT SER event
 */
typedef struct mcs_ft_msg_ctrl_ser_event_s {
    /*! Flags */
    uint32_t flags;

    /*! Memory */
    uint32_t mem;

    /*! Index */
    uint32_t index;

    /*! Pipe */
    uint8_t pipe;
} mcs_ft_msg_ctrl_ser_event_t;

/*!
 * Start the transmission of the template set
 */
typedef struct mcs_ft_msg_ctrl_template_xmit_s {
    /*! Flags */
    uint32_t flags;

    /*! Template id */
    uint16_t template_id;

    /*! Collector id */
    uint16_t collector_id;

    /*! Initial burst of packets to be sent out */
    uint16_t initial_burst;

    /*! Transmit Interval */
    uint16_t interval_secs;

    /*! Length of the encap */
    uint16_t encap_length;

    /*! Template encap */
    uint8_t encap[MCS_SHR_FT_MAX_TEMPLATE_SET_ENCAP_LENGTH];

} mcs_ft_msg_ctrl_template_xmit_t;

/*!
 * Elephant profile filter
 */
typedef struct mcs_ft_msg_ctrl_elph_profile_filter_s {
    /*! Upper 32 bits of the size threshold in bytes */
    uint32_t size_threshold_bytes_upper_32;

    /*! Upper 32 bits of the size threshold in bytes */
    uint32_t size_threshold_bytes_lower_32;

    /*! Low rate threshold in bytes/100usec */
    uint32_t rate_low_threshold;

    /*! High rate threshold in bytes/100usec */
    uint32_t rate_high_threshold;

    /*! Scan count */
    uint16_t scan_count;

    /*! 1 - Rate has to increase every scan */
    uint8_t incr_rate;
} mcs_ft_msg_ctrl_elph_profile_filter_t;

/*!
 * Add an elephant profile
 */
typedef struct mcs_ft_msg_ctrl_elph_profile_create_s {
    /*! Filters arranged in order, 2 Promotions and 1 Demotion */
    mcs_ft_msg_ctrl_elph_profile_filter_t filters[MCS_SHR_FT_ELPH_MAX_FILTERS];

    /*! Profile Id */
    uint8_t profile_idx;

    /*! Number of filters, added for completeness sake, must be always 3 */
    uint8_t num_filters;
} mcs_ft_msg_ctrl_elph_profile_create_t;

/*!
 * Get an elephant profile
 */
typedef struct mcs_ft_msg_ctrl_elph_profile_get_s {
    /*! Filters arranged in order, 2 Promotions and 1 Demotion */
    mcs_ft_msg_ctrl_elph_profile_filter_t filters[MCS_SHR_FT_ELPH_MAX_FILTERS];

    /*! Profile Id */
    uint8_t profile_idx;

    /*! Number of filters, added for completeness sake, must be always 3 */
    uint8_t num_filters;
} mcs_ft_msg_ctrl_elph_profile_get_t;

/*!
 * Get learn stats
 */
typedef struct mcs_ft_msg_ctrl_stats_learn_s {
    /*! Counts corresponding to different stats */
    uint32_t count[MCS_SHR_FT_LEARN_PKT_MAX_STATUS];

    /*! Average time taken for each status */
    uint32_t avg_usecs[MCS_SHR_FT_LEARN_PKT_MAX_STATUS];

    /*! Number of status */
    uint16_t num_status;
} mcs_ft_msg_ctrl_stats_learn_t;

/*!
 * Get export stats
 */
typedef struct mcs_ft_msg_ctrl_stats_export_s {
    /*! Number of packets exported */
    uint32_t num_pkts;

    /*! Number of flows exported */
    uint32_t num_flows;

    /*! Current Export Timestamp*/
    uint32_t ts;

    /*! Current IPFIX Sequence number */
    uint32_t seq_num;

} mcs_ft_msg_ctrl_stats_export_t;
/*!
 * Get age stats
 */
typedef struct mcs_ft_msg_ctrl_stats_age_s {
    /*! Number of flows aged */
    uint32_t num_flows;
} mcs_ft_msg_ctrl_stats_age_t;

/*!
 * Get elph stats
 */
typedef struct mcs_ft_msg_ctrl_stats_elph_s {
    /*! State transition counts */
    uint32_t trans_count[MCS_SHR_FT_ELPH_FLOW_STATE_COUNT][MCS_SHR_FT_ELPH_ACTION_COUNT];

    /*! Number of states */
    uint16_t num_states;

    /*! Number of actions */
    uint16_t num_actions;
} mcs_ft_msg_ctrl_stats_elph_t;

/*!
 * Features supported by the SDK
 */
typedef struct mcs_ft_msg_ctrl_sdk_features_s {
    /*! Feature bitmap */
    uint32_t features;

    /*! SDK cfg mode */
    uint8_t cfg_mode;
} mcs_ft_msg_ctrl_sdk_features_t;

/*!
 * Features supported by the uC
 */
typedef struct mcs_ft_msg_ctrl_uc_features_s {
    /*! Feature bitmap */
    uint32_t features;
} mcs_ft_msg_ctrl_uc_features_t;

/*!
 * Set actions on a group
 */
typedef struct mcs_ft_msg_ctrl_group_actions_set_s {
    /*! Action bitmap */
    uint32_t actions;

    /*! Group Idx */
    uint16_t group_idx;

    /*! CoSQ for FSP action */
    uint16_t fsp_cosq;
} mcs_ft_msg_ctrl_group_actions_set_t;

/*!
 * EM UFT banks info.
 */
typedef struct mcs_ft_msg_ctrl_em_bank_uft_info_s {
    /*! Number of banks */
    uint8_t num_banks;

    /*! Bank IDs */
    uint32_t bank_id[MCS_SHR_FT_MAX_UFT_BANKS];

    /*! Bank hash offset */
    uint32_t  bank_offset[MCS_SHR_FT_MAX_UFT_BANKS];

    /*! Number of base buckets in each bank */
    uint32_t  bank_num_buckets[MCS_SHR_FT_MAX_UFT_BANKS];
}mcs_ft_msg_ctrl_em_bank_uft_info_t;

/*! Flow data message. */
typedef struct mcs_ft_msg_ctrl_flow_data_get_first_reply_s {

    /*! Flow index. */
    uint16_t flow_idx;

    /*! Group index. */
    uint16_t group_idx;

    /*! Indicates if flow was statically installed by application. */
    bool     is_static;

    /*! Flow tuples */

    /*! Source IPv4 address. */
    uint32_t src_ipv4;

    /*! Destination IPv4 address. */
    uint32_t dst_ipv4;

    /*! Layer 4 UDP source port. */
    uint16_t src_l4_port;

    /*! Layer 4 UDP destination port. */
    uint16_t dst_l4_port;

    /*! IP protocol used. */
    uint8_t ip_proto;

    /*! Inner source IPv4 address. */
    uint32_t inner_src_ipv4;

    /*! Inner destination IPv4 address. */
    uint32_t inner_dst_ipv4;

    /*! Inner layer 4 UDP source port. */
    uint16_t inner_src_l4_port;

    /*! Inner layer 4 UDP destination port. */
    uint16_t inner_dst_l4_port;

    /*! Inner IP protocol used. */
    uint8_t inner_ip_proto;
    /*! VxLAN network ID. */
    uint32_t vnid;

    /*! Custom user defined key length. */
    uint8_t custom_length;

    /*! Custom user defined  key. */
    uint8_t custom[MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH];

    /*! The ingress port of the packet. */
    uint16_t in_port;

    /*! Inner source IPv6 address. */
    uint8_t inner_src_ipv6[16];

    /*! Inner destination IPv6 address. */
    uint8_t inner_dst_ipv6[16];

    /*! Flow data */

    /*! Upper 32 bits of the packet counter */
    uint32_t pkt_count_upper;

    /*! Lower 32 bits of the packet counter */
    uint32_t pkt_count_lower;

    /*! Upper 32 bits of the byte counter */
    uint32_t byte_count_upper;

    /*! Lower 32 bits of the byte counter */
    uint32_t byte_count_lower;

    /*! Upper 32 bits of the flow start timestamp */
    uint32_t flow_start_ts_upper;

    /*! Lower 32 bits of the flow start timestamp */
    uint32_t flow_start_ts_lower;

    /*! Upper 32 bits of the Observation timestamp */
    uint32_t obs_ts_upper;

    /*! Lower 32 bits of the Observation timestamp */
    uint32_t obs_ts_lower;

} mcs_ft_msg_ctrl_flow_data_get_first_reply_t;

/*! Flow data get next reply message. */
typedef mcs_ft_msg_ctrl_flow_data_get_first_reply_t mcs_ft_msg_ctrl_flow_data_get_next_reply_t;


/*! Static flow data message. */
typedef struct mcs_ft_msg_ctrl_static_flow_get_first_reply_s {

    /*! Flow index. */
    uint16_t flow_idx;

    /*! Group index. */
    uint16_t group_idx;

    /* Flow tuples */

    /*! Source IPv4 address. */
    uint32_t src_ipv4;

    /*! Destination IPv4 address. */
    uint32_t dst_ipv4;

    /*! Layer 4 UDP source port. */
    uint16_t src_l4_port;

    /*! Layer 4 UDP destination port. */
    uint16_t dst_l4_port;

    /*! IP protocol used. */
    uint8_t ip_proto;

    /*! Inner source IPv4 address. */
    uint32_t inner_src_ipv4;

    /*! Inner destination IPv4 address. */
    uint32_t inner_dst_ipv4;

    /*! Inner layer 4 UDP source port. */
    uint16_t inner_src_l4_port;

    /*! Inner layer 4 UDP destination port. */
    uint16_t inner_dst_l4_port;

    /*! Inner IP protocol used. */
    uint8_t inner_ip_proto;

    /*! VxLAN network ID. */
    uint32_t vnid;

    /*! Custom user defined key length. */
    uint8_t custom_length;

    /*! Custom user defined  key. */
    uint8_t custom[MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH];

    /*! The ingress port of the packet. */
    uint16_t in_port;

    /*! Inner source IPv6 address. */
    uint8_t inner_src_ipv6[16];

    /*! Inner destination IPv6 address. */
    uint8_t inner_dst_ipv6[16];

    /*! Flow data */

    /*! Upper 32 bits of the packet counter */
    uint32_t pkt_count_upper;

    /*! Lower 32 bits of the packet counter */
    uint32_t pkt_count_lower;

    /*! Upper 32 bits of the byte counter */
    uint32_t byte_count_upper;

    /*! Lower 32 bits of the byte counter */
    uint32_t byte_count_lower;

    /*! Upper 32 bits of the flow start timestamp */
    uint32_t flow_start_ts_upper;

    /*! Lower 32 bits of the flow start timestamp */
    uint32_t flow_start_ts_lower;

    /*! Upper 32 bits of the Observation timestamp */
    uint32_t obs_ts_upper;

    /*! Lower 32 bits of the Observation timestamp */
    uint32_t obs_ts_lower;

} mcs_ft_msg_ctrl_static_flow_get_first_reply_t;

/*! Static flow data get next reply */
typedef mcs_ft_msg_ctrl_static_flow_get_first_reply_t mcs_ft_msg_ctrl_static_flow_get_next_reply_t;

/*!
 * Group flow index and data
 */
typedef struct mcs_ft_msg_ctrl_group_flow_data_set_s {

    /*! Group index. */
    uint16_t group_idx;

    /* Flow tuples */

    /*! Source IPv4 address. */
    uint32_t src_ipv4;

    /*! Destination IPv4 address. */
    uint32_t dst_ipv4;

    /*! Layer 4 UDP source port. */
    uint16_t src_l4_port;

    /*! Layer 4 UDP destination port. */
    uint16_t dst_l4_port;

    /*! IP protocol used. */
    uint8_t ip_proto;

    /*! Inner source IPv4 address. */
    uint32_t inner_src_ipv4;

    /*! Inner destination IPv4 address. */
    uint32_t inner_dst_ipv4;

    /*! Inner layer 4 UDP source port. */
    uint16_t inner_src_l4_port;

    /*! Inner layer 4 UDP destination port. */
    uint16_t inner_dst_l4_port;

    /*! Inner IP protocol used. */
    uint8_t inner_ip_proto;

    /*! VxLAN network ID. */
    uint32_t vnid;

    /*! Custom user defined key length. */
    uint8_t custom_length;

    /*! Custom user defined  key. */
    uint8_t custom[MCS_SHR_FT_CUSTOM_KEY_MAX_LENGTH];

    /*! The ingress port of the packet. */
    uint16_t in_port;

    /*! Inner source IPv6 address. */
    uint8_t inner_src_ipv6[16];

    /*! Inner destination IPv6 address. */
    uint8_t inner_dst_ipv6[16];

} mcs_ft_msg_ctrl_group_flow_data_set_t;

/*! Static flow create message */
typedef mcs_ft_msg_ctrl_group_flow_data_set_t mcs_ft_msg_ctrl_static_flow_set_t;

/*! HW learn opt message */
typedef struct mcs_ft_msg_ctrl_hw_learn_opt_msg_s {
    /*! Number of hitbit ctr banks */
    uint8_t num_hitbit_ctr_banks;

    /*! Hitbit ctr banks' info */
    mcs_shr_ft_hitbit_ctr_bank_info_t
            ctr_bank_info[MCS_SHR_FT_NUM_HITBIT_BANKS_MAX];
}mcs_ft_msg_ctrl_hw_learn_opt_msg_t;

/*! RX PMD flex fields message */
typedef struct mcs_ft_msg_ctrl_rxpmd_flex_format_s {
    /*! List of fields. */
    uint8_t  fids[MCS_FT_RXPMD_FLEX_FIELD_COUNT];

    /*! List of profiles. */
    uint8_t  profiles[MCS_FT_RXPMD_FLEX_PROFILES_COUNT];

    /*! Start offset of the fields. */
    uint16_t offsets[MCS_FT_RXPMD_FLEX_FIELD_COUNT][MCS_FT_RXPMD_FLEX_PROFILES_COUNT];

    /*! Widths of the fields. */
    uint8_t widths[MCS_FT_RXPMD_FLEX_FIELD_COUNT][MCS_FT_RXPMD_FLEX_PROFILES_COUNT];

    /*! Number of profiles. */
    uint16_t num_profiles;

    /*! Number of fields. */
    uint8_t num_fields;
}mcs_ft_msg_ctrl_rxpmd_flex_format_t;

/*! Match Id format message. */
typedef struct mcs_ft_msg_ctrl_match_id_set_s {
    /*! Number of match Id fields. */
    uint16_t num_fields;

    /*! Match ID fields. */
    uint16_t fields[MCS_SHR_FT_MATCH_ID_FIELD_COUNT];

    /*! Offset of the fields in the match ID. */
    uint16_t offsets[MCS_SHR_FT_MATCH_ID_FIELD_COUNT];

    /*! Width of the fields in the match ID. */
    uint16_t widths[MCS_SHR_FT_MATCH_ID_FIELD_COUNT];

    /*! Value of the fields in the match ID. */
    uint16_t values[MCS_SHR_FT_MATCH_ID_FIELD_COUNT];
} mcs_ft_msg_ctrl_match_id_set_t;

/*!
 * Union of  FT control messages, used to size dma buffer
 */
typedef union mcs_ft_msg_ctrl_s {
    /*! Init message */
    mcs_ft_msg_ctrl_init_t                        init;

    /*! EM key format  message */
    mcs_ft_msg_ctrl_em_key_format_t               em_key_format;

    /*! Group create message */
    mcs_ft_msg_ctrl_group_create_t                group_create;

    /*! Group get message */
    mcs_ft_msg_ctrl_group_get_t                   group_get;

    /*! Group update message */
    mcs_ft_msg_ctrl_group_update_t                group_update;

    /*! Template create message */
    mcs_ft_msg_ctrl_template_create_t             template_create;

    /*! Template get message */
    mcs_ft_msg_ctrl_template_get_t                template_get;

    /*! Collector create message */
    mcs_ft_msg_ctrl_collector_create_t            collector_create;

    /*! Collector get message */
    mcs_ft_msg_ctrl_collector_get_t               collector_get;

    /*! Group flow data get message */
    mcs_ft_msg_ctrl_group_flow_data_get_t         data_get;

    /*! Group flow data get reply message */
    mcs_ft_msg_ctrl_group_flow_data_get_reply_t   data_get_reply;

    /*! SER event message */
    mcs_ft_msg_ctrl_ser_event_t                   ser_event;

    /*! Start of template transmit message */
    mcs_ft_msg_ctrl_template_xmit_t               tmpl_xmit;

    /*! Elephant profile create message */
    mcs_ft_msg_ctrl_elph_profile_create_t         eprofile_create;

    /*! Elephant profile get */
    mcs_ft_msg_ctrl_elph_profile_get_t            eprofile_get;

    /*! Stats learn message */
    mcs_ft_msg_ctrl_stats_learn_t                 stats_learn;

    /*! Stats export message */
    mcs_ft_msg_ctrl_stats_export_t                stats_export;

    /*! Stats age message */
    mcs_ft_msg_ctrl_stats_age_t                   stats_age;

    /*! Elephant profile stats message */
    mcs_ft_msg_ctrl_stats_elph_t                  stats_elph;

    /*! SDK features message */
    mcs_ft_msg_ctrl_sdk_features_t                sdk_features;

    /*! UC features message */
    mcs_ft_msg_ctrl_uc_features_t                 uc_features;

    /*! EM group create message */
    mcs_ft_msg_ctrl_em_group_create_t             em_group_create;

    /*! Group actions set message */
    mcs_ft_msg_ctrl_group_actions_set_t           group_actions_set;

    /*! EM bank uft info message */
    mcs_ft_msg_ctrl_em_bank_uft_info_t            em_bank_uft_info;

    /*! Flow data get reply message */
    mcs_ft_msg_ctrl_flow_data_get_first_reply_t   flow_data;

    /*! Staitc flow data get reply message */
    mcs_ft_msg_ctrl_static_flow_get_first_reply_t static_flow_info;

    /*! Group flow data clear message */
    mcs_ft_msg_ctrl_group_flow_data_set_t         group_data;

    /*! Hw learning options message */
    mcs_ft_msg_ctrl_hw_learn_opt_msg_t            hw_learn_opt;

    /*! RXPMD flex field message */
    mcs_ft_msg_ctrl_rxpmd_flex_format_t           rxpmd_flex_format;

    /*! Match ID set message. */
    mcs_ft_msg_ctrl_match_id_set_t                match_id_set;
} mcs_ft_msg_ctrl_t;

#endif /* MCS_MON_FLOWTRACKER_MSG_H */
