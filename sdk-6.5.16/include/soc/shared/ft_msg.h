/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    shr_ft_msg.h
 */

#ifndef SHR_FT_MSG_H_
#define SHR_FT_MSG_H_

#include <soc/shared/ft.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif


#define FT_MSG_BASE_VERSION  0x0

/*
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
/*
 * Enhanced shr_ft_msg_ctrl_group_flow_data_get_reply_t to add:
 *   - drop_reasons[]
 *   - num_drop_reasons
 */
/*
 * Enhanced shr_ft_msg_ctrl_init_t to include:
 *    - fsp_reinject_max_length
 */

#define FT_MSG_VERSION_1 1

/*
 * Enhanced shr_ft_msg_ctrl_group_flow_data_get_t to add:
 *   - in_port
 */
#define FT_MSG_VERSION_2 2

#define FT_MSG_VERSION  FT_MSG_VERSION_2

extern uint32 ft_msg_version;

/*
 * Per pipe info used for FT Initialization control message
 */
typedef struct __COMPILER_ATTR__ _ft_msg_ctrl_init_pipe_s {
    /* Max flows */
    uint32 max_flows;

    /* Size of reserved pools */
    uint16 ctr_pool_size[SHR_FT_MAX_CTR_POOLS];

    /* Reserved Flex counter pools per pipe */
    uint8 ctr_pools[SHR_FT_MAX_CTR_POOLS];

    /* Number of reserved counter pools */
    uint8 num_ctr_pools;

} shr_ft_msg_ctrl_init_pipe_t;

/*
 * FT Initialization control message
 */

/* Enable flow start timestamp storing */
#define SHR_FT_MSG_INIT_FLAGS_FLOW_START_TS (1 << 0)

/* Enable Elephant monitoring */
#define SHR_FT_MSG_INIT_FLAGS_ELEPHANT (1 << 1)

/* Enable drop monitoring */
#define SHR_FT_MSG_INIT_FLAGS_DROP_MONITOR (1 << 2)

typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_init_s {
    /* Initialization flags */
    uint32 flags;

    /* Max number of groups */
    uint32 max_groups;

    /* Local RX DMA channel */
    uint32 rx_channel;

    /* Current time in seconds */
    uint32 cur_time_secs;

    /* Export interval in msec */
    uint32 export_interval_msecs;

    /* Scan interval in usec */
    uint32 scan_interval_usecs;

    /* Aging timer tick in msec */
    uint32 age_timer_tick_msecs;

    /* Per H/w pipe info */
    shr_ft_msg_ctrl_init_pipe_t pipe[SHR_FT_MAX_PIPES];

    /* Maximum length of the transmitted packet */
    uint16 max_export_pkt_length;

    /* FSP re-inject max length */
    uint16 fsp_reinject_max_length;

    /* Num ports in the device */
    uint8 num_pipes;

    /* Number of elephant profiles */
    uint8 num_elph_profiles;

} shr_ft_msg_ctrl_init_t;

/*
 * EM key format export
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_em_qual_s {
    /* Offsets into the key */
    uint16 offset[SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /* Width of qualifiers */
    uint8  width[SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /* Num offsets of the qualifier */
    uint8  num_parts;
} shr_ft_msg_ctrl_qual_parts_t;

typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_em_key_format_s {
    /* Offsets and width of each qual */
    shr_ft_msg_ctrl_qual_parts_t qual_parts[SHR_FT_EM_MAX_QUAL];

    /* Size of the key in bits */
    uint16 key_size;

    /* Qualifiers */
    uint8 qual[SHR_FT_EM_MAX_QUAL];

    /* Number of qualifiers */
    uint8 num_qual;
} shr_ft_msg_ctrl_em_key_format_t;

/*
 * EM qualifier info
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_qual_info_s {
    /* Width of the qualifier in bytes */
    uint16 width;

    /* Offsets of each key part */
    uint16 part_offset[SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /* Width of each qualifier part */
    uint8 part_width[SHR_FT_EM_KEY_MAX_QUAL_PARTS];

    /* Qualifier */
    uint8 qual;

    /* Num offsets of the qualifier */
    uint8 num_parts;

    /* Base offset type of UDF */
    uint8 base_offset;

    /* Relative offset in bytes of this chunk in the pkt */
    uint8 relative_offset;

} shr_ft_msg_ctrl_qual_info_t;

/*
 * Tracking param info
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_tp_info_s {
    /* Tracking param */
    uint8 param;

    /* Number of qualifiers that make up this param */
    uint8 num_qual;

    /* Qualifiers */
    uint8 qual[SHR_FT_EM_MAX_QUAL];

    /* UDF or fixed qualifier */
    uint8 udf;
} shr_ft_msg_ctrl_tp_info_t;


/*
 * EM group info
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_em_group_create_s {
    /* Offsets and width of each qual */
    shr_ft_msg_ctrl_qual_info_t qual_info[SHR_FT_EM_MAX_QUAL];

    /* Tracking param info */
    shr_ft_msg_ctrl_tp_info_t tp_info[SHR_FT_MAX_TP];

    /* Action set bitmap */
    uint32 aset;

    /* Size of the key in bits */
    uint16 key_size;

    /* EM group index */
    uint8 group_idx;

    /* EM LT ID */
    uint8 lt_id[SHR_FT_MAX_PIPES];

    /* Number of pipes in the device */
    uint8 num_pipes;

    /* Number of qualifiers */
    uint8 num_qual;

    /* Number of tracking params */
    uint8 num_tp;

} shr_ft_msg_ctrl_em_group_create_t;


/*
 * FT group create
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_create_s {
    /* Flags */
    uint32 flags;

    /* Max flows that can be learned on this group */
    uint32  flow_limit;

    /* Aging interval in msecs */
    uint32 aging_interval_msecs;

    /* IPFIX domain id */
    uint32 domain_id;

    /* Group Index */
    uint16  group_idx;
} shr_ft_msg_ctrl_group_create_t;

/*
 * FT group get
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_get_s {
    /* Flags */
    uint32 flags;

    /* Max flows that can be learned on this group */
    uint32 flow_limit;

    /* Num flows that learnt on this group */
    uint32 flow_count;

    /* Aging interval in msecs */
    uint32 aging_interval;

    /* IPFIX domain id */
    uint32 domain_id;

    /* Group Index */
    uint16 group_idx;

    /* Elephant profile Id */
    uint16 elph_profile_id;

    /* QoS profile Id */
    uint16 qos_profile_id;

    /* Collector Id */
    uint16 collector_id;

    /* Template Id */
    uint16 template_id;

} shr_ft_msg_ctrl_group_get_t;

/*
 * FT group update
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_update_s {
    /* Flags */
    uint32  flags;

    /* Indicates what is being updated */
    uint32  update;

    /* Param value depending on the update */
    uint32  param0;

    /* Group Index */
    uint16 group_idx;
} shr_ft_msg_ctrl_group_update_t;

/*
 * Create IPFIX template
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_template_create_s {
    /* Flags */
    uint32 flags;

    /* Software handle for template */
    uint16 id;

    /* IPFIX set id */
    uint16 set_id;

    /* Number of information elements */
    uint8 num_info_elems;

    /* Information elements in the order they are to be exported */
    uint8 info_elems[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];

    /* Size in bytes of the info elems */
    uint16 data_size[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} shr_ft_msg_ctrl_template_create_t;

/*
 * Get IPFIX template
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_template_get_s {
    /* Flags */
    uint32 flags;

    /* Software handle for template */
    uint16 id;

    /* IPFIX set id */
    uint16 set_id;

    /* Number of information elements */
    uint8 num_info_elems;

    /* Information elements in the order they are to be exported */
    uint8 info_elems[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];

    /* Size in bytes of the info elems */
    uint16 data_size[SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS];
} shr_ft_msg_ctrl_template_get_t;

/* Collector msg flags */
#define SHR_FT_MSG_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE (1 << 0)

/*
 * Create  collector
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_collector_create_s {
    /* Flags */
    uint32 flags;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Component Id for Protobuf export */
    uint32 component_id;

    /* Software handle for collector */
    uint16 id;

    /* Length of the IPFIX encapsulation */
    uint16 encap_length;

    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;

    /* Length of the system Id used in Protobuf export */
    uint16 system_id_length;

    uint8 system_id[SHR_FT_PB_SYSTEM_ID_MAX_LENGTH];

    /* Collector encapsulation */
    uint8 encap[SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH];
} shr_ft_msg_ctrl_collector_create_t;

/*
 * Get collector
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_collector_get_s {
    /* Flags */
    uint32 flags;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Component Id for Protobuf export */
    uint32 component_id;

    /* Software handle for collector */
    uint16 id;

    /* Length of the IPFIX encapsulation */
    uint16 encap_length;

    /* Collector encapsulation */
    uint16 encap[SHR_FT_MAX_COLLECTOR_ENCAP_LENGTH];

    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Length of the system Id used in Protobuf export */
    uint16 system_id_length;

    uint8 system_id[SHR_FT_PB_SYSTEM_ID_MAX_LENGTH];

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;
} shr_ft_msg_ctrl_collector_get_t;

/*
 * Get data associated with a flow
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_flow_data_get_s {
    /* Source IP */
    uint32 src_ip;

    /* Destination IP */
    uint32 dst_ip;

    /* Source IP */
    uint32 inner_src_ip;

    /* Destination IP */
    uint32 inner_dst_ip;

    /* VNID */
    uint32 vnid;

    /* Group Index */
    uint16 group_idx;

    /* L4 source port */
    uint16 l4_src_port;

    /* L4 destination port */
    uint16 l4_dst_port;

    /* L4 source port */
    uint16 inner_l4_src_port;

    /* L4 destination port */
    uint16 inner_l4_dst_port;

    /* IP protocol */
    uint8 ip_protocol;

    /* IP protocol */
    uint8 inner_ip_protocol;

    /* Custom key length */
    uint8 custom_length;

    /* Custom */
    uint8 custom[SHR_FT_CUSTOM_KEY_MAX_LENGTH];

    /* In port */
    uint16 in_port;
} shr_ft_msg_ctrl_group_flow_data_get_t;

/*
 * Data associated with a flow
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_flow_data_get_reply_s {
    /* Upper 32 bits of the packet counter */
    uint32 pkt_count_upper;

    /* Lower 32 bits of the packet counter */
    uint32 pkt_count_lower;

    /* Upper 32 bits of the byte counter */
    uint32 byte_count_upper;

    /* Lower 32 bits of the byte counter */
    uint32 byte_count_lower;

    /* Upper 32 bits of the flow start timestamp */
    uint32 flow_start_ts_upper;

    /* Lower 32 bits of the flow start timestamp */
    uint32 flow_start_ts_lower;

    /* Upper 32 bits of the Observation timestamp */
    uint32 obs_ts_upper;

    /* Lower 32 bits of the Observation timestamp */
    uint32 obs_ts_lower;

    /* Drop reasons  */
    uint16 drop_reasons[SHR_FT_DROP_REASONS_MAX_COUNT];

    /* Number of drop reasons */
    uint8 num_drop_reasons;

} shr_ft_msg_ctrl_group_flow_data_get_reply_t;

/*
 * FT SER event
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_ser_event_s {
    /* Flags */
    uint32 flags;

    /* Memory */
    uint32 mem;

    /* Index */
    uint32 index;

    /* Pipe */
    uint8 pipe;
} shr_ft_msg_ctrl_ser_event_t;

/*
 * Start the transmission of the template set
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_template_xmit_s {
    /* Flags */
    uint32 flags;

    /* Template id */
    uint16 template_id;

    /* Collector id */
    uint16 collector_id;

    /* Initial burst of packets to be sent out */
    uint16 initial_burst;

    /* Transmit Interval */
    uint16 interval_secs;

    /* Length of the encap */
    uint16 encap_length;

    /* Template encap */
    uint8 encap[SHR_FT_MAX_TEMPLATE_SET_ENCAP_LENGTH];

} shr_ft_msg_ctrl_template_xmit_t;

/*
 * Elephant profile filter
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_elph_profile_filter_s {
    /* Upper 32 bits of the size threshold in bytes */
    uint32 size_threshold_bytes_upper_32;

    /* Upper 32 bits of the size threshold in bytes */
    uint32 size_threshold_bytes_lower_32;

    /* Low rate threshold in bytes/100usec */
    uint32 rate_low_threshold;

    /* High rate threshold in bytes/100usec */
    uint32 rate_high_threshold;

    /* Scan count */
    uint16 scan_count;

    /* 1 - Rate has to increase every scan */
    uint8 incr_rate;
} shr_ft_msg_ctrl_elph_profile_filter_t;

/*
 * Add an elephant profile
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_elph_profile_create_s {
    /* Filters arranged in order, 2 Promotions and 1 Demotion */
    shr_ft_msg_ctrl_elph_profile_filter_t filters[SHR_FT_ELPH_MAX_FILTERS];

    /* Profile Id */
    uint8 profile_idx;

    /* Number of filters, added for completeness sake, must be always 3 */
    uint8 num_filters;
} shr_ft_msg_ctrl_elph_profile_create_t;

/*
 * Get an elephant profile
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_elph_profile_get_s {
    /* Filters arranged in order, 2 Promotions and 1 Demotion */
    shr_ft_msg_ctrl_elph_profile_filter_t filters[SHR_FT_ELPH_MAX_FILTERS];

    /* Profile Id */
    uint8 profile_idx;

    /* Number of filters, added for completeness sake, must be always 3 */
    uint8 num_filters;
} shr_ft_msg_ctrl_elph_profile_get_t;

/*
 * Get learn stats
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_stats_learn_s {
    /* Counts corresponding to different stats */
    uint32 count[SHR_FT_LEARN_PKT_MAX_STATUS];

    /* Average time taken for each status */
    uint32 avg_usecs[SHR_FT_LEARN_PKT_MAX_STATUS];

    /* Number of status */
    uint16 num_status;
} shr_ft_msg_ctrl_stats_learn_t;

/*
 * Get export stats
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_stats_export_s {
    /* Number of packets exported */
    uint32 num_pkts;

    /* Number of flows exported */
    uint32 num_flows;

    /* Current Export Timestamp*/
    uint32 ts;

    /* Current IPFIX Sequence number */
    uint32 seq_num;

} shr_ft_msg_ctrl_stats_export_t;

/*
 * Get age stats
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_stats_age_s {
    /* Number of flows aged */
    uint32 num_flows;
} shr_ft_msg_ctrl_stats_age_t;

/*
 * Get elph stats
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_stats_elph_s {
    /* State transition counts */
    uint32 trans_count[SHR_FT_ELPH_FLOW_STATE_COUNT][SHR_FT_ELPH_ACTION_COUNT];

    /* Number of states */
    uint16 num_states;

    /* Number of actions */
    uint16 num_actions;
} shr_ft_msg_ctrl_stats_elph_t;

/*
 * Features supported by the SDK
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_sdk_features_s {
    /* Feature bitmap */
    uint32 features;

    /* SDK cfg mode */
    uint8 cfg_mode;
} shr_ft_msg_ctrl_sdk_features_t;

/*
 * Features supported by the uC
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_uc_features_s {
    /* Feature bitmap */
    uint32 features;
} shr_ft_msg_ctrl_uc_features_t;

/*
 * Set actions on a group
 */
typedef struct __COMPILER_ATTR__ shr_ft_msg_ctrl_group_actions_set_s {
    /* Action bitmap */
    uint32 actions;

    /* Group Idx */
    uint16 group_idx;

    /* CoSQ for FSP action */
    uint16 fsp_cosq;
} shr_ft_msg_ctrl_group_actions_set_t;

/*
 *  FT control messages
 */
typedef union shr_ft_msg_ctrl_s {
    shr_ft_msg_ctrl_init_t                      init;
    shr_ft_msg_ctrl_em_key_format_t             em_key_format;
    shr_ft_msg_ctrl_group_create_t              group_create;
    shr_ft_msg_ctrl_group_get_t                 group_get;
    shr_ft_msg_ctrl_group_update_t              group_update;
    shr_ft_msg_ctrl_template_create_t           template_create;
    shr_ft_msg_ctrl_template_get_t              template_get;
    shr_ft_msg_ctrl_collector_create_t          collector_create;
    shr_ft_msg_ctrl_collector_get_t             collector_get;
    shr_ft_msg_ctrl_group_flow_data_get_t       data_get;
    shr_ft_msg_ctrl_group_flow_data_get_reply_t data_get_reply;
    shr_ft_msg_ctrl_ser_event_t                 ser_event;
    shr_ft_msg_ctrl_template_xmit_t             tmpl_xmit;
    shr_ft_msg_ctrl_elph_profile_create_t       eprofile_create;
    shr_ft_msg_ctrl_elph_profile_get_t          eprofile_get;
    shr_ft_msg_ctrl_stats_learn_t               stats_learn;
    shr_ft_msg_ctrl_stats_export_t              stats_export;
    shr_ft_msg_ctrl_stats_age_t                 stats_age;
    shr_ft_msg_ctrl_stats_elph_t                stats_elph;
    shr_ft_msg_ctrl_sdk_features_t              sdk_features;
    shr_ft_msg_ctrl_uc_features_t               uc_features;
    shr_ft_msg_ctrl_em_group_create_t           em_group_create;
    shr_ft_msg_ctrl_group_actions_set_t         group_actions_set;
} shr_ft_msg_ctrl_t;


#endif /* SHR_FT_MSG_H_ */
