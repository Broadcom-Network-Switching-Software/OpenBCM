/*! \file mcs_ifa_common.h
 *
 * Inband Flow Analyzer (IFA) common definitions shared with the uKernel.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_IFA_COMMON_H
#define MCS_IFA_COMMON_H

/*!
 * \brief IFA Error codes
 */
typedef enum mcs_ifa_uc_error_e {
    /*!
     * The operation completed successfully.
     */
    MCS_IFA_UC_E_NONE      = 0,

    /*!
     * This usually indicates that software encountered an internal
     * data inconsistency or an unanticipated hardware state.
     */
    MCS_IFA_UC_E_INTERNAL  = 1,

    /*!
     * An operation failed due to insufficient dynamically allocated
     * memory.
     */
    MCS_IFA_UC_E_MEMORY    = 2,

    /*!
     * The first argument of many API routines is a unit number. This
     * error occurs if that number refers to a non-existent unit.
     */
    MCS_IFA_UC_E_UNIT      = 3,

    /*!
     * A parameter to an API routine was invalid. A null pointer value
     * may have been passed to the routine, or an integer parameter
     * may be outside of its allowed range.
     */
    MCS_IFA_UC_E_PARAM     = 4,

    /*!
     * The operation encountered a pooled resource (e.g. a table or a
     * list) with no valid elements.
     */
    MCS_IFA_UC_E_EMPTY     = 5,

    /*!
     * The operation encountered a pooled resource (e.g. a table or a
     * list) with no room for new elements.
     */
    MCS_IFA_UC_E_FULL      = 6,

    /*!
     * The specified entry in a pooled resource (e.g. a table or a
     * list) could not be found.
     */
    MCS_IFA_UC_E_NOT_FOUND = 7,

    /*!
     * The specified entry of a pooled resource (e.g. a table or a
     * list) already exists.
     */
    MCS_IFA_UC_E_EXISTS    = 8,

    /*!
     * The operation did not complete within the maximum allowed time frame.
     */
    MCS_IFA_UC_E_TIMEOUT   = 9,

    /*!
     * An operation was attempted before the previous operation had
     * completed.
     */
    MCS_IFA_UC_E_BUSY      = 10,

    /*!
     * An operation could not be completed. This may be due to a
     * hardware or configuration problem.
     */
    MCS_IFA_UC_E_FAIL      = 11,

    /*!
     * The operation could not be completed because a required feature
     * was disabled.
     */
    MCS_IFA_UC_E_DISABLED  = 12,

    /*!
     * The specified identifier was not valid. Note that this error
     * code will normally take precedence over \ref MCS_IFA_UC_E_PARAM.
     */
    MCS_IFA_UC_E_BADID     = 13,

    /*!
     * The operation could not be completed due to lack of hardware
     * resources.
     */
    MCS_IFA_UC_E_RESOURCE  = 14,

    /*!
     * The operation could not be completed due to incomplete or
     * incorrect configurataion.
     */
    MCS_IFA_UC_E_CONFIG    = 15,

    /*!
     * The requested operation is not supported.
     */
    MCS_IFA_UC_E_UNAVAIL   = 16,

    /*!
     * An operation was attempted before initialization was complete.
     */
    MCS_IFA_UC_E_INIT      = 17,

    /*!
     * The specified port value was not valid. Note that this error
     * code will normally take precedence over \ref MCS_IFA_UC_E_PARAM.
     */
    MCS_IFA_UC_E_PORT      = 18
} mcs_ifa_uc_error_t;

/*! Maximum possible size of the IFA metadata in bytes. */
#define MCS_IFA_MD_MAX_SIZE (32)

/*! Maximum number of elements that can be pushed to the FIFO. */
#define MCS_IFA_MD_FIFO_MAX_ELEMENTS (32)

/*! Maximum number of FIFO elements that make up metadata element. */
#define MCS_IFA_MD_MAX_FIFO_PARTS (2)

/*!
 * \brief IFA FIFO elements list
 *
 * List of elements that can be pushed to the Egress MD FIFO
 */
typedef enum mcs_ifa_fifo_elements_e {
    /*! Invalid element */
    MCS_IFA_FIFO_ELEMENT_INVALID                           = 0,

    /*! MMU INT STAT1 [15:0] bits */
    MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT1_15_0                = 1,

    /*! MMU INT STAT1 [18:16] bits */
    MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT1_18_16               = 2,

    /*! MMU INT STAT0 [15:0] bits */
    MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT0_15_0                = 3,

    /*! MMU INT STAT0 [18:16] bits */
    MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT0_18_16               = 4,

    /*! Drop byte count [15:0] bits */
    MCS_IFA_FIFO_ELEMENT_IFA_ATTR_QUE_TX_BCOUNT_15_0       = 5,

    /*! Drop byte count [31:16] bits */
    MCS_IFA_FIFO_ELEMENT_IFA_ATTR_QUE_TX_BCOUNT_31_16      = 6,

    /*! Residence time nanosecond [15:0] bits */
    MCS_IFA_FIFO_ELEMENT_RESIDENCE_TIME_NSEC_15_0          = 7,

    /*! Residence time nanosecond [31:16] bits */
    MCS_IFA_FIFO_ELEMENT_RESIDENCE_TIME_NSEC_31_16         = 8,

    /*! Ingress nanosecond timestamp [15:0] bits */
    MCS_IFA_FIFO_ELEMENT_ING_PTP_NSEC_15_0                 = 9,

    /*! Ingress nanosecond timestamp [31:16] bits */
    MCS_IFA_FIFO_ELEMENT_ING_PTP_NSEC_31_16                = 10,

    /*! Ingress port */
    MCS_IFA_FIFO_ELEMENT_SYS_SOURCE                        = 11,

    /*! Egress Port */
    MCS_IFA_FIFO_ELEMENT_GSH_SYS_DESTINATION               = 12,

    /*! Ingress second timestamp [15:0] bits */
    MCS_IFA_FIFO_ELEMENT_ING_PTP_SEC_15_0                  = 13,

    /*! Ingress second timestamp [31:16] bits */
    MCS_IFA_FIFO_ELEMENT_ING_PTP_SEC_31_16                 = 14,

    /*! CCBE_CTRL_2 */
    MCS_IFA_FIFO_ELEMENT_CCBE_CTRL_2                       = 15,

    /*! CCBE_CTRL_1 */
    MCS_IFA_FIFO_ELEMENT_CCBE_CTRL_1                       = 16,

    /*! Congestion */
    MCS_IFA_FIFO_ELEMENT_INT_CONGESTION                    = 17,

    /*! Port speed */
    MCS_IFA_FIFO_ELEMENT_IFA_ATTR_PORT_SPEED               = 18,

    /*! TTL */
    MCS_IFA_FIFO_ELEMENT_FWD_HDR_TTL                       = 19,

    /*! Last value, not usable */
    MCS_IFA_FIFO_ELEMENT_COUNT
} mcs_ifa_fifo_elements_t;

/*! Names of the IFA FIFO elements. */
#define MCS_IFA_FIFO_ELEMENT_NAMES      \
    {                                   \
        "INVALID                   ",   \
        "MMU_STAT1_15_0            ",   \
        "MMU_STAT1_18_16           ",   \
        "MMU_STAT0_15_0            ",   \
        "MMU_STAT0_18_16           ",   \
        "QUE_TX_BCOUNT_15_0        ",   \
        "QUE_TX_BCOUNT_31_16       ",   \
        "RESIDENCE_TIME_NSEC_15_0  ",   \
        "RESIDENCE_TIME_NSEC_31_16 ",   \
        "ING_PTP_NSEC_15_0         ",   \
        "ING_PTP_NSEC_31_16        ",   \
        "SYS_SOURCE                ",   \
        "SYS_DESTINATION           ",  \
        "ING_PTP_SEC_15_0          ",   \
        "ING_PTP_SEC_31_16         ",   \
        "CCBE_CTRL_2               ",   \
        "CCBE_CTRL_1               ",   \
        "CONGESTION                ",   \
        "PORT_SPEED                ",   \
        "TTL                       "    \
    }

/*!
 * \brief IFA metadata elements list
 *
 * List of elements that can be part of IFA metadata
 */
typedef enum mcs_ifa_md_elements_e {
    /*! Invalid element */
    MCS_IFA_MD_ELEMENT_INVALID                          = 0,

    /*! 32b device ID or (4b LNS + device ID) */
    MCS_IFA_MD_ELEMENT_LNS_DEVICE_ID                    = 1,

    /*! IP TTL  */
    MCS_IFA_MD_ELEMENT_IP_TTL                           = 2,

    /*! Egress port speed */
    MCS_IFA_MD_ELEMENT_EGRESS_PORT_SPEED                = 3,

    /*! Congestion */
    MCS_IFA_MD_ELEMENT_CONGESTION                       = 4,

    /*! Queue ID */
    MCS_IFA_MD_ELEMENT_QUEUE_ID                         = 5,

    /*! Rx timestamp seconds portion */
    MCS_IFA_MD_ELEMENT_RX_TIMESTAMP_SEC                 = 6,

    /*! Egress system port */
    MCS_IFA_MD_ELEMENT_EGRESS_SYSTEM_PORT               = 7,

    /*! Ingress system port */
    MCS_IFA_MD_ELEMENT_INGRESS_SYSTEM_PORT              = 8,

    /*! Rx timestamp nanosecond portion */
    MCS_IFA_MD_ELEMENT_RX_TIMESTAMP_NANO_SEC            = 9,

    /*! Residence time in nanoseconds */
    MCS_IFA_MD_ELEMENT_RESIDENCE_TIME_NANO_SEC          = 10,

    /*! Opaque data 1 */
    MCS_IFA_MD_ELEMENT_OPAQUE_DATA_1                    = 11,

    /*! Opaque Data 2 */
    MCS_IFA_MD_ELEMENT_OPAQUE_DATA_2                    = 12,

    /*! Opaque Data 3 */
    MCS_IFA_MD_ELEMENT_OPAQUE_DATA_3                    = 13,

    /*! Last value, not usable */
    MCS_IFA_MD_ELEMENT_COUNT
} mcs_ifa_md_elements_t;

/*! Names of the metadata elements. */
#define MCS_IFA_MD_ELEMENT_NAMES               \
    {                                          \
        "INVALID                ",             \
        "LNS_DEVICE_ID          ",             \
        "IP_TTL                 ",             \
        "EGRESS_PORT_SPEED      ",             \
        "CONGESTION             ",             \
        "QUEUE_ID               ",             \
        "RX_TIMESTAMP_SEC       ",             \
        "EGRESS_SYSTEM_PORT     ",             \
        "INGRESS_SYSTEM_PORT    ",             \
        "RX_TIMESTAMP_NANO_SEC  ",             \
        "RESIDENCE_TIME_NANO_SEC",             \
        "OPAQUE_DATA_1          ",             \
        "OPAQUE_DATA_2          ",             \
        "OPAQUE_DATA_3          "              \
    }

/*! IPFIX template elements. */
typedef enum mcs_ifa_ipfix_template_elements_s {
    /*! IFA headers in the received packet. */
    MCS_IFA_IPFIX_TEMPLATE_ELEMENT_IFA_HEADERS = 0,

    /*! The IFA metadata stack in the received packet. */
    MCS_IFA_IPFIX_TEMPLATE_ELEMENT_IFA_MD_STACK = 1,

    /*! The received packet with the IFA data stripped off. */
    MCS_IFA_IPFIX_TEMPLATE_ELEMENT_PKT = 2,

    /*! Last value, not usable. */
    MCS_IFA_IPFIX_TEMPLATE_ELEMENT_COUNT
} mcs_ifa_ipfix_template_elements_t;

/*!
 * \brief IFA init message
 *
 * Initialize the IFA EApp
 */
typedef struct mcs_ifa_msg_ctrl_init_s {
    /*! Flags */
    uint32_t flags;

    /*! Current NPL version number - unused. */
    uint32_t npl_version;

    /*! Device Id. */
    uint32_t device_id;

    /*! Upper 32b of init timestamp */
    uint32_t init_time_hi;

    /*! Lower 32b of init timestamp */
    uint32_t init_time_lo;

    /*! Maximum length of the exported packet */
    uint16_t max_export_pkt_length;

    /*! Maximum length of the incoming packet */
    uint16_t max_rx_pkt_length;

    /*! Width of the MD-FIFO in bytes */
    uint16_t fifo_width;

    /*! Number of entries in the MD-FIFO */
    uint16_t fifo_depth;
} mcs_ifa_msg_ctrl_init_t;

/*! Rx PMD flex fields used by IFA EApp. */
typedef enum mcs_ifa_rxpmd_flex_fields_s {
    /*!
     * Currently no fields are extracted from flex portion. Define a dummy
     * field for the messaging structure initialization. Replace dummy field
     * with an actual field if flex fields need to be extracted in future.
     */
    MCS_IFA_RXPMD_FLEX_FIELD_DUMMY = 0,

    MCS_IFA_RXPMD_FLEX_FIELD_COUNT
} mcs_ifa_rxpmd_flex_fields_t;

/*!
 * Max number of Rx PMD profiles. Currently unused, set to 1 toff allow the
 * messaging structure definition.
 */
#define MCS_IFA_RXPMD_MAX_PROFILES (1)

/*!
 * \brief IFA Rx PMD flex format.
 *
 * Contains the offsets and widths of the flex fields in Rx PMD.
 */
typedef struct mcs_ifa_msg_ctrl_rxpmd_flex_format_s {
    /*! Start offset of the fields. */
    uint16_t offsets[MCS_IFA_RXPMD_MAX_PROFILES][MCS_IFA_RXPMD_FLEX_FIELD_COUNT];

    /*! Widths of the fields. */
    uint8_t widths[MCS_IFA_RXPMD_MAX_PROFILES][MCS_IFA_RXPMD_FLEX_FIELD_COUNT];

    /*! Number of profiles. */
    uint16_t num_profiles;

    /*! Profiles. */
    uint16_t profiles[MCS_IFA_RXPMD_MAX_PROFILES];

    /*! Number of fields. */
    uint8_t num_fields;

    /*! List of fields. */
    uint8_t fields[MCS_IFA_RXPMD_FLEX_FIELD_COUNT];

} mcs_ifa_msg_ctrl_rxpmd_flex_format_t;

/*!
 * \brief IFA FIFO info set message
 *
 * Set IFA MD FIFO contents and width
 */
typedef struct mcs_ifa_msg_ctrl_fifo_format_set_s {
    /*! Flags */
    uint32_t flags;

    /*! FIFO Id - currently unused. */
    uint16_t id;

    /*! Number of elements in the FIFO */
    uint16_t num_elements;

    /*! Elements in the FIFO */
    uint16_t elements[MCS_IFA_MD_FIFO_MAX_ELEMENTS];
} mcs_ifa_msg_ctrl_fifo_format_set_t;

/*! Indicates that the namespace is local */
#define MCS_IFA_MSG_CTRL_MD_FORMAT_SET_FLAGS_LNS (1 << 0)

/*!
 * \brief IFA MD format set message
 *
 * Set IFA metadata format
 */
typedef struct mcs_ifa_msg_ctrl_md_format_set_s {
    /*! Flags */
    uint32_t flags;

    /*! MD Id - currently unused. */
    uint16_t id;

    /*! FIFO Id used by this MD - currently unused. */
    uint16_t fifo_id;

    /*! Number of elements in the metadata */
    uint16_t num_elements;

    /*! Elements in the metadata, in the order in which they are packed */
    uint16_t elements[MCS_IFA_MD_ELEMENT_COUNT];

    /*! Width of each element in bits */
    uint16_t element_widths[MCS_IFA_MD_ELEMENT_COUNT];

    /*! FIFO elements making up each metadata element. */
    uint16_t fifo_parts[MCS_IFA_MD_ELEMENT_COUNT][MCS_IFA_MD_MAX_FIFO_PARTS];

    /*! Start offset of FIFO parts. */
    uint16_t fifo_part_offsets[MCS_IFA_MD_ELEMENT_COUNT][MCS_IFA_MD_MAX_FIFO_PARTS];

    /*! Width of FIFO parts. */
    uint16_t fifo_part_widths[MCS_IFA_MD_ELEMENT_COUNT][MCS_IFA_MD_MAX_FIFO_PARTS];

    /*! Number of FIFO elements to construct one MD element. */
    uint8_t num_fifo_parts[MCS_IFA_MD_ELEMENT_COUNT];

    /*! Local or global namespace for this metadata */
    uint8_t namespace;
} mcs_ifa_msg_ctrl_md_format_set_t;

/*!
 * \brief IFA configuration update param
 *
 * Configuration parameter being updated
 */
typedef enum mcs_ifa_msg_ctrl_config_update_param_e {
    MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_INVALID                   = 0,
    MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_DEVICE_ID                 = 1,
    MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_EXPORT_ENABLE             = 2,
    MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_RX_PKT_EXPORT_MAX_LENGTH  = 3,
    MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_IPFIX_ENTERPRISE_NUMBER   = 4,
    MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT
} mcs_ifa_msg_ctrl_config_update_param_t;

/*! Maximum number of values that can be accepted per parameter */
#define MCS_IFA_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES 1

/*!
 * \brief IFA configuration update message
 *
 * Update the IFA configuration.
 */
typedef struct mcs_ifa_msg_ctrl_config_update_s {
    /*! Flags */
    uint32_t flags;

    /*! Parmeters being updated */
    uint32_t params[MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];

    /*! Parameter values depending on the update */
    uint32_t values[MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT][MCS_IFA_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES];

    /*! Number of values per parameter */
    uint16_t num_values[MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];

    /*! Number of parameters being updated */
    uint16_t num_params;

} mcs_ifa_msg_ctrl_config_update_t;

/*! Use IPv4 if this flag is set, else IPv6. */
#define MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_L3_TYPE_IPV4 (1 << 0)
/*!
 * UDP Checksum needs to be calculated on the exported packet if
 * this flag is set.
 */
#define MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE (1 << 1)
/*!
 * User num_records instead of mtu to determine how many records to pack in a
 * single packet.
 */
#define MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_USE_NUM_RECORDS (1 << 2)

/*! Maximum length of the collector encapsulation */
#define MCS_IFA_MSG_CTRL_COLLECTOR_CREATE_MAX_ENCAP_LENGTH 128

/*!
 * \brief IFA collector create message.
 *
 * Create a collector for the IFA EApp
 */
typedef struct mcs_ifa_msg_ctrl_collector_create_s {
    /*! Flags */
    uint32_t flags;

    /*! 32b Base IP header checksum excluding length */
    uint32_t ip_base_checksum;

    /*! 32b Base UDP psuedo header checksum */
    uint32_t udp_base_checksum;

    /*! IPFIX observation domain Id. */
    uint32_t observation_domain;

    /*! Length of the collector encapsulation */
    uint16_t encap_length;

    /*! Maximum size of the packet that can be exported to collector */
    uint16_t mtu;

    /*!
     * Number of records that need to be encoded in a single packet exported to
     * collector.
     */
    uint16_t num_records;

    /*! Offset to start of IP header in the encapsulation */
    uint16_t ip_offset;

    /*! Offset to start of UDP header in the encap */
    uint16_t udp_offset;

    /*! Collector encapsulation */
    uint8_t encap[MCS_IFA_MSG_CTRL_COLLECTOR_CREATE_MAX_ENCAP_LENGTH];
} mcs_ifa_msg_ctrl_collector_create_t;

/*! Maximum length of the IPFIX template set. */
#define MCS_IFA_IPFIX_TEMPLATE_SET_MAX_LENGTH (32)

/*!
 * \brief IFA IPFIX template set message.
 *
 * Set the IPFIX template format for the exported packets.
 */
typedef struct mcs_ifa_msg_ctrl_ipfix_template_set_s {
    /*! Flags. */
    uint32_t flags;

    /*! Tx interval. */
    uint32_t tx_interval;

    /*! Initial burst. */
    uint32_t initial_burst;

    /*! IPFIX set Id. */
    uint16_t set_id;

    /*! Length of the template set.*/
    uint16_t template_set_length;

    /*! Number of template elements. */
    uint8_t num_elements;

    /*! Template elements. */
    uint8_t elements[MCS_IFA_IPFIX_TEMPLATE_ELEMENT_COUNT];

    /*! Template set that needs to be exported. */
    uint8_t template_set[MCS_IFA_IPFIX_TEMPLATE_SET_MAX_LENGTH];
} mcs_ifa_msg_ctrl_ipfix_template_set_t;


/*! Indicates that rx_pkt_cnt stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_CNT (1 << 0)

/*! Indicates that tx_pkt_cnt stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_TX_PKT_CNT (1 << 1)

/*! Indicates that tx_record_cnt stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_TX_RECORD_CNT (1 << 2)

/*! Indicates that rx_pkt_export_disabled_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_EXPORT_DISABLED_DISCARD (1 << 3)

/*! Indicates that rx_pkt_no_export_config_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_NO_EXPORT_CONFIG_DISCARD (1 << 4)

/*! Indicates that rx_pkt_no_ipfix_config_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_NO_IPFIX_CONFIG_DISCARD (1 << 5)

/*! Indicates that rx_pkt_current_length_exceed_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_CURRENT_LENGTH_EXCEED_DISCARD (1 << 6)

/*! Indicates that rx_pkt_length_exceed_max_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_LENGTH_EXCEED_MAX_DISCARD (1 << 7)

/*! Indicates that rx_pkt_parse_error_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_PARSE_ERROR_DISCARD (1 << 8)

/*! Indicates that rx_pkt_unknown_namespace_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_UNKNOWN_NAMESPACE_DISCARD (1 << 9)

/*! Indicates that rx_pkt_excess_rate_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_EXCESS_RATE_DISCARD (1 << 10)

/*! Indicates that rx_pkt_incomplete_metadata_discard stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_INCOMPLETE_METADATA_DISCARD (1 << 11)

/*! Indicates that tx_pkt_failure_cnt stats is valid */
#define MCS_IFA_MSG_STATS_SET_BMP_TX_PKT_FAILURE_CNT (1 << 12)


/*!
 * \brief IFA statistics set message
 *
 * Set IFA statistics
 */
typedef struct mcs_ifa_msg_ctrl_stats_set_s {
    /*! Indicates which stats are valid */
    uint32_t valid_bmp;

    /*! Number of received packets - upper 32 bits */
    uint32_t rx_pkt_cnt_hi;

    /*! Number of received packets - lower 32 bits. */
    uint32_t rx_pkt_cnt_lo;

    /*! Number of transmitted packets - upper 32 bits. */
    uint32_t tx_pkt_cnt_hi;

    /*! Number of transmitted packets - lower 32 bits. */
    uint32_t tx_pkt_cnt_lo;

    /*! Number of transmitted records - upper 32 bits. */
    uint32_t tx_record_cnt_hi;

    /*! Number of transmitted records - lower 32 bits. */
    uint32_t tx_record_cnt_lo;

    /*!
     * Number of Rx packets discarded as export is not enabled - upper 32 bits.
     */
    uint32_t rx_pkt_export_disabled_discard_hi;

    /*!
     * Number of Rx packets discarded as export is not enabled - lower 32 bits.
     */
    uint32_t rx_pkt_export_disabled_discard_lo;

    /*!
     * Number of Rx packets discarded as export configuration
     * is not configured - upper 32 bits.
     */
    uint32_t rx_pkt_no_export_config_discard_hi;

    /*!
     * Number of Rx packets discarded as export configuration
     * is not configured - lower 32 bits.
     */
    uint32_t rx_pkt_no_export_config_discard_lo;

    /*!
     * Number of Rx packets discarded as IPFIX configuration is not
     * done - upper 32 bits.
     */
    uint32_t rx_pkt_no_ipfix_config_discard_hi;

    /*!
     * Number of Rx packets discarded as IPFIX configuration is not
     * done - lower 32 bits.
     */
    uint32_t rx_pkt_no_ipfix_config_discard_lo;

    /*!
     * Number of Rx packets discarded due to current length exceeding maximum
     * length field in the IFA header - upper 32 bits.
     */
    uint32_t rx_pkt_current_length_exceed_discard_hi;

    /*!
     * Number of Rx packets discarded due to current length exceeding maximum
     * length field in the IFA header - lower 32 bits.
     */
    uint32_t rx_pkt_current_length_exceed_discard_lo;

    /*!
     * Number of Rx packets discarded as the packet's length exceeded the size
     * of the Rx buffer - upper 32 bits.
     */
    uint32_t rx_pkt_length_exceed_max_discard_hi;

    /*!
     * Number of Rx packets discarded as the packet's length exceeded the size
     * of the Rx buffer - lower 32 bits.
     */
    uint32_t rx_pkt_length_exceed_max_discard_lo;

    /*!
     * Number of Rx packets discarded due to errors in
     * parsing  - upper 32 bits.
     */
    uint32_t rx_pkt_parse_error_discard_hi;

    /*! Number of Rx packets discarded due to errors in parsing - lower 32 bits */
    uint32_t rx_pkt_parse_error_discard_lo;

    /*!
     * Number of Rx pkts discarded as the namespace is unknown - upper 32 bits.
     */
    uint32_t rx_pkt_unknown_namespace_discard_hi;

    /*!
     * Number of Rx pkts discarded as the namespace is unknown - lower 32 bits.
     */
    uint32_t rx_pkt_unknown_namespace_discard_lo;

    /*!
     * Number of Rx packets discarded due to excess incoming rate - upper 32
     * bits.
     */
    uint32_t rx_pkt_excess_rate_discard_hi;

    /*!
     * Number of Rx packets discarded due to excess incoming rate - lower 32
     * bits.
     */
    uint32_t rx_pkt_excess_rate_discard_lo;

    /*!
     * Number of Rx packets discarded as the metadata stack is incomplete -
     * upper 32 bits.
     */
    uint32_t rx_pkt_incomplete_metadata_discard_hi;

    /*!
     * Number of Rx packets discarded as the metadata stack is incomplete -
     * lower 32 bits.
     */
    uint32_t rx_pkt_incomplete_metadata_discard_lo;

    /*!
     * Number of Tx packets that could not be exported due to some
     * failure - upper 32 bits.
     */
    uint32_t tx_pkt_failure_cnt_hi;

    /*!
     * Number of Tx packets that could not be exported due to some
     * failure - lower 32 bits.
     */
    uint32_t tx_pkt_failure_cnt_lo;
} mcs_ifa_msg_ctrl_stats_set_t;

/*!
 * \brief IFA statistics get message
 *
 * Get IFA statistics
 */
typedef mcs_ifa_msg_ctrl_stats_set_t mcs_ifa_msg_ctrl_stats_get_t;


/*!
 * \brief IFA messages
 *
 * Union of all IFA messages, used to size the DMA buffer.
 */
typedef union mcs_ifa_msg_ctrl_s {
    /*! Init message */
    mcs_ifa_msg_ctrl_init_t init;

    /*! Packet flex fields format message */
    mcs_ifa_msg_ctrl_rxpmd_flex_format_t pmd_flex_format;

    /*! FIFO info set message */
    mcs_ifa_msg_ctrl_fifo_format_set_t fifo_format_set;

    /*! MD format set message */
    mcs_ifa_msg_ctrl_md_format_set_t md_format_set;

    /*! Config update message. */
    mcs_ifa_msg_ctrl_config_update_t config_update;

    /*! Collector set message. */
    mcs_ifa_msg_ctrl_collector_create_t collector_create;

    /*! IPFIX template set message. */
    mcs_ifa_msg_ctrl_ipfix_template_set_t ipfix_template_set;

    /*! Stat set message. */
    mcs_ifa_msg_ctrl_stats_set_t stats_set;

    /*! Stat get message. */
    mcs_ifa_msg_ctrl_stats_get_t stats_get;
} mcs_ifa_msg_ctrl_t;

#endif /* MCS_IFA_COMMON_H */
