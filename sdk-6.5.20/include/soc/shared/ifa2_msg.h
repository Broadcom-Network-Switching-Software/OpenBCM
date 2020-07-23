/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ifa2_msg.h
 */

#ifndef SHR_IFA2_MSG_H_
#define SHR_IFA2_MSG_H_

#include <soc/shared/ifa2.h>

/*
 * IFA2 init message
 */
typedef struct shr_ifa2_msg_ctrl_init_s {
    /* Flags */
    uint32 flags;

    /* Upper 32b of init timestamp */
    uint32 init_time_hi;

    /* Lower 32b of init timestamp */
    uint32 init_time_lo;

    /* Maximum length of the exported packet */
    uint16 max_export_pkt_length;

    /* Maximum length of the incoming packet */
    uint16 max_rx_pkt_length;
} shr_ifa2_msg_ctrl_init_t;

/* Indicates that the namespace is local */
#define SHR_IFA2_MSG_CTRL_MD_FORMAT_SET_FLAGS_LNS (1 << 0)

/*
 * IFA2 MD format set message
 */
typedef struct shr_ifa2_msg_ctrl_md_format_set_s {
    /* Flags */
    uint32 flags;

    /* MD Id - currently unused. */
    uint16 id;

    /* Number of elements in the metadata */
    uint16 num_elements;

    /* Elements in the metadata, in the order in which they are packed */
    uint16 elements[SHR_IFA2_MD_ELEMENT_COUNT];

    /* Width of each element in bits */
    uint16 element_widths[SHR_IFA2_MD_ELEMENT_COUNT];

    /* Local or global namespace for this metadata */
    uint8 namespace;
} shr_ifa2_msg_ctrl_md_format_set_t;

/*
 * IFA2 configuration update param
 */
typedef enum shr_ifa2_msg_ctrl_config_update_param_e {
    SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_INVALID                   = 0,
    SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_RX_PKT_EXPORT_MAX_LENGTH  = 1,
    SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT
} shr_ifa2_msg_ctrl_config_update_param_t;

/* Maximum number of values that can be accepted per parameter */
#define SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES 1

/*
 * \brief IFA2 configuration update message
 *
 * Update the IFA2 configuration.
 */
typedef struct shr_ifa2_msg_ctrl_config_update_s {
    /* Flags */
    uint32 flags;

    /* Parmeters being updated */
    uint32 params[SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];

    /* Parameter values depending on the update */
    uint32 values[SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT][SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES];

    /* Number of values per parameter */
    uint16 num_values[SHR_IFA2_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];

    /* Number of parameters being updated */
    uint16 num_params;
} shr_ifa2_msg_ctrl_config_update_t;

/* Use IPv4 if this flag is set, else IPv6. */
#define SHR_IFA2_MSG_CTRL_COLLECTOR_FLAGS_L3_TYPE_IPV4 (1 << 0)
/*
 * UDP Checksum needs to be calculated on the exported packet if
 * this flag is set.
 */
#define SHR_IFA2_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE (1 << 1)
/*
 * User num_records instead of mtu to determine how many records to pack in a
 * single packet.
 */
#define SHR_IFA2_MSG_CTRL_COLLECTOR_FLAGS_USE_NUM_RECORDS (1 << 2)

/* Maximum length of the system ID. */
#define SHR_IFA2_MSG_CTRL_COLLECTOR_CREATE_MAX_SYSTEM_ID_LENGTH 32

/* Maximum length of the collector encapsulation */
#define SHR_IFA2_MSG_CTRL_COLLECTOR_CREATE_MAX_ENCAP_LENGTH 128

/*
 * IFA2 collector create message.
 */
typedef struct shr_ifa2_msg_ctrl_collector_create_s {
    /* Flags */
    uint32 flags;

    /* 32b Base IP header checksum excluding length */
    uint32 ip_base_checksum;

    /* 32b Base UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Observation domain ID */
    uint32 observation_domain_id;

    /* Length of the collector encapsulation */
    uint16 encap_length;

    /* Maximum size of the packet that can be exported to collector */
    uint16 mtu;

    /*
     * Number of records that need to be encoded in a single packet exported to
     * collector.
     */
    uint16 num_records;

    /* Offset to start of IP header in the encapsulation */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;

    /* Collector encapsulation */
    uint8 encap[SHR_IFA2_MSG_CTRL_COLLECTOR_CREATE_MAX_ENCAP_LENGTH];
} shr_ifa2_msg_ctrl_collector_create_t;

/* List of supported Info elements that can be exported by app */
typedef enum shr_ifa2_template_info_elem_e {
    SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_HEADERS           = 0,
    SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_MD_STACK          = 1,
    SHR_IFA2_TEMPLATE_INFO_ELEM_IFA_PKT_DATA          = 2,
    SHR_IFA2_TEMPLATE_MAX_INFO_ELEMENTS
} shr_ifa2_template_info_elem_t;

/*
 * Internal data structure to store the information element data, used in
 * template set export
 */
typedef struct shr_ifa2_info_elem_s {
    /* Export element */
    shr_ifa2_template_info_elem_t element;

    /* Size of each element */
    uint32 data_size;

    /* Export element Id */
    uint16 id;

    /* 1 - Enterprise element, 0 - Non enterprise element */
    uint8 enterprise;
} shr_ifa2_export_element_t;

/* Internal data structure to store per export template
 * information.
 */
typedef struct shr_ifa2_msg_export_template_info_s {
    /* Software ID of the template */
    uint32 template_id;

    /* Number of information elements */
    uint32 num_export_elements;

    /* Set ID that is put into the IPFIX packet */
    uint16 set_id;

    /* Export elements */
    shr_ifa2_export_element_t elements[SHR_IFA2_TEMPLATE_MAX_INFO_ELEMENTS];
} shr_ifa2_msg_export_template_info_t;

/*
 * Start the transmission of the template set
 */
typedef struct shr_ifa2_msg_ctrl_template_xmit_s {
    /* Flags */
    uint32 flags;

    /* Template id */
    uint32 template_id;

    /* Collector id */
    uint32 collector_id;

    /* Initial burst of packets to be sent out */
    uint32 initial_burst;

    /* Transmit Interval */
    uint32 interval_secs;

    /* Length of the encap */
    uint16 encap_length;

    /* Template encap */
    uint8 encap[SHR_IFA2_MAX_TEMPLATE_SET_ENCAP_LENGTH];

} shr_ifa2_msg_ctrl_template_xmit_t;


/*
 * IFA2 statistics set message
 */
typedef struct shr_ifa2_msg_ctrl_stats_set_s {
    /* Number of received packets - upper 32 bits */
    uint32 rx_pkt_cnt_hi;

    /* Number of received packets. */
    uint32 rx_pkt_cnt_lo;

    /*
     * Number of Rx packets discarded due to excess incoming rate - upper 32
     * bits.
     */
    uint32 rx_pkt_excess_rate_discard_hi;

    /*
     * Number of Rx packets discarded due to excess incoming rate - lower 32
     * bits.
     */
    uint32 rx_pkt_excess_rate_discard_lo;

    /*
     * Number of Rx packets discarded as export is not enabled - upper 32 bits.
     */
    uint32 rx_pkt_export_disabled_discard_hi;

    /*
     * Number of Rx packets discarded as export is not enabled - lower 32 bits.
     */
    uint32 rx_pkt_export_disabled_discard_lo;

    /*
     * Number of Rx packets discarded as export configuration
     * is not configured - upper 32 bits.
     */
    uint32 rx_pkt_no_export_config_discard_hi;

    /*
     * Number of Rx packets discarded as export configuration
     * is not configured - lower 32 bits.
     */
    uint32 rx_pkt_no_export_config_discard_lo;

    /* Number of Rx packets discarded due to current length exceeding maximum
     * length field in the IFA header - upper 32 bits.
     */
    uint32 rx_pkt_current_length_exceed_discard_hi;

    /* Number of Rx packets discarded due to current length exceeding maximum
     * length field in the IFA header - lower 32 bits.
     */
    uint32 rx_pkt_current_length_exceed_discard_lo;

    /*
     * Number of Rx packets discarded as the packet's length exceeded the size
     * of the Rx buffer - upper 32 bits.
     */
    uint32 rx_pkt_length_exceed_max_discard_hi;

    /*
     * Number of Rx packets discarded as the packet's length exceeded the size
     * of the Rx buffer - lower 32 bits.
     */
    uint32 rx_pkt_length_exceed_max_discard_lo;

    /*
     * Number of Rx packets discarded due to errors in
     * parsing  - upper 32 bits.
     */
    uint32 rx_pkt_parse_error_discard_hi;

    /* Number of Rx packets discarded due to errors in parsing - lower 32 bits */
    uint32 rx_pkt_parse_error_discard_lo;

    /*
     * Number of Rx pkts discarded as the namespace is unknown - upper 32 bits.
     */
    uint32 rx_pkt_unknown_namespace_discard_hi;

    /*
     * Number of Rx pkts discarded as the namespace is unknown - lower 32 bits.
     */
    uint32 rx_pkt_unknown_namespace_discard_lo;

    /* Number of transmitted packets - upper 32 bits. */
    uint32 tx_pkt_cnt_hi;

    /* Number of transmitted packets - lower 32 bits. */
    uint32 tx_pkt_cnt_lo;

    /* Number of transmitted records - upper 32 bits. */
    uint32 tx_record_cnt_hi;

    /* Number of transmitted records - lower 32 bits. */
    uint32 tx_record_cnt_lo;

    /*
     * Number of Tx packets that could not be exported due to some
     * failure - upper 32 bits.
     */
    uint32 tx_pkt_failure_cnt_hi;

    /*
     * Number of Tx packets that could not be exported due to some
     * failure - lower 32 bits.
     */
    uint32 tx_pkt_failure_cnt_lo;

    /*
     * Number of Rx packets discarded as template configuration
     * is not configured - upper 32 bits.
     */
    uint32 rx_pkt_no_template_config_discard_hi;

    /*
     * Number of Rx packets discarded as template configuration
     * is not configured - lower 32 bits.
     */
    uint32 rx_pkt_no_template_config_discard_lo;
} shr_ifa2_msg_ctrl_stats_set_t;

/*
 * IFA2 statistics get message
 */
typedef shr_ifa2_msg_ctrl_stats_set_t shr_ifa2_msg_ctrl_stats_get_t;


/*
 * IFA2 messages
 *
 * Union of all IFA2 messages, used to size the DMA buffer.
 */
typedef union shr_ifa2_msg_ctrl_s {
    /* Init message */
    shr_ifa2_msg_ctrl_init_t init;

    /* MD format set message */
    shr_ifa2_msg_ctrl_md_format_set_t md_format_set;

    /* Config update message. */
    shr_ifa2_msg_ctrl_config_update_t config_update;

    /* Collector set message. */
    shr_ifa2_msg_ctrl_collector_create_t collector_create;

    /* Template info message. */
    shr_ifa2_msg_export_template_info_t template_info;

    /* Template xmit message. */
    shr_ifa2_msg_ctrl_template_xmit_t template_xmit;

    /* Stat set message. */
    shr_ifa2_msg_ctrl_stats_set_t stats_set;

    /* Stat get message. */
    shr_ifa2_msg_ctrl_stats_get_t stats_get;
} shr_ifa2_msg_ctrl_t;

#endif /* SHR_IFA2_MSG_H_ */
