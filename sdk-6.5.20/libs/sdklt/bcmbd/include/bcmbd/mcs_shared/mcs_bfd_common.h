/*! \file mcs_bfd_common.h
 *
 * Bidirectional Forwarding Detection (BFD) common definitions shared with
 * the uKernel.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_BFD_COMMON_H
#define MCS_BFD_COMMON_H

/*!
 * \brief BFD Error codes
 */
typedef enum mcs_bfd_uc_error_e {
    /*!
     * The operation completed successfully.
     */
    MCS_BFD_UC_E_NONE      = 0,

    /*!
     * This usually indicates that software encountered an internal
     * data inconsistency or an unanticipated hardware state.
     */
    MCS_BFD_UC_E_INTERNAL  = 1,

    /*!
     * An operation failed due to insufficient dynamically allocated
     * memory.
     */
    MCS_BFD_UC_E_MEMORY    = 2,

    /*!
     * The first argument of many API routines is a unit number. This
     * error occurs if that number refers to a non-existent unit.
     */
    MCS_BFD_UC_E_UNIT      = 3,

    /*!
     * A parameter to an API routine was invalid. A null pointer value
     * may have been passed to the routine, or an integer parameter
     * may be outside of its allowed range.
     */
    MCS_BFD_UC_E_PARAM     = 4,

    /*!
     * The operation encountered a pooled resource (e.g. a table or a
     * list) with no valid elements.
     */
    MCS_BFD_UC_E_EMPTY     = 5,

    /*!
     * The operation encountered a pooled resource (e.g. a table or a
     * list) with no room for new elements.
     */
    MCS_BFD_UC_E_FULL      = 6,

    /*!
     * The specified entry in a pooled resource (e.g. a table or a
     * list) could not be found.
     */
    MCS_BFD_UC_E_NOT_FOUND = 7,

    /*!
     * The specified entry of a pooled resource (e.g. a table or a
     * list) already exists.
     */
    MCS_BFD_UC_E_EXISTS    = 8,

    /*!
     * The operation did not complete within the maximum allowed time frame.
     */
    MCS_BFD_UC_E_TIMEOUT   = 9,

    /*!
     * An operation was attempted before the previous operation had
     * completed.
     */
    MCS_BFD_UC_E_BUSY      = 10,

    /*!
     * An operation could not be completed. This may be due to a
     * hardware or configuration problem.
     */
    MCS_BFD_UC_E_FAIL      = 11,

    /*!
     * The operation could not be completed because a required feature
     * was disabled.
     */
    MCS_BFD_UC_E_DISABLED  = 12,

    /*!
     * The specified identifier was not valid. Note that this error
     * code will normally take precedence over \ref MCS_BFD_UC_E_PARAM.
     */
    MCS_BFD_UC_E_BADID     = 13,

    /*!
     * The operation could not be completed due to lack of hardware
     * resources.
     */
    MCS_BFD_UC_E_RESOURCE  = 14,

    /*!
     * The operation could not be completed due to incomplete or
     * incorrect configurataion.
     */
    MCS_BFD_UC_E_CONFIG    = 15,

    /*!
     * The requested operation is not supported.
     */
    MCS_BFD_UC_E_UNAVAIL   = 16,

    /*!
     * An operation was attempted before initialization was complete.
     */
    MCS_BFD_UC_E_INIT      = 17,

    /*!
     * The specified port value was not valid. Note that this error
     * code will normally take precedence over \ref MCS_BFD_UC_E_PARAM.
     */
    MCS_BFD_UC_E_PORT      = 18
} mcs_bfd_uc_error_t;

/*! Max timeout for msging between SDK and UKERNEL */
#define MCS_BFD_MAX_UC_MSG_TIMEOUT (5000000) /* 5 secs */


/*!
 * Unknown session Id. BFD packets with this session Id undergo hash lookup to
 * resolve the session Id.
 */
#define MCS_BFD_UNKNOWN_SESSION_ID  (0x7FF)

/*! BFD encapsulation types. */
typedef enum mcs_bfd_encap_types_s {
    /*! BFD encap type - Raw. */
    MCS_BFD_ENCAP_TYPE_RAW = 0,

    /*! BFD encap type - IPv4-UDP. */
    MCS_BFD_ENCAP_TYPE_V4UDP = 1,

    /*! BFD encap type - IPv6-UDP. */
    MCS_BFD_ENCAP_TYPE_V6UDP = 2,

    /*! BFD encap type - MPLS-TP CC only. */
    MCS_BFD_ENCAP_TYPE_MPLS_TP_CC = 3,

    /*! BFD encap type - MPLS-TP with CC & CV. */
    MCS_BFD_ENCAP_TYPE_MPLS_TP_CC_CV = 4,

    /*! BFD encap type - MPLS with PHP. */
    MCS_BFD_ENCAP_TYPE_MPLS_PHP = 5,

    /*! Last value, not usable. */
    MCS_BFD_ENCAP_TYPE_COUNT
} mcs_bfd_encap_types_t;


/*! BFD Session States */
typedef enum mcs_bfd_session_state_e {
    /*! Admin down. */
    MCS_BFD_SESSION_STATE_ADMIN_DOWN   = 0,

    /*! Down. */
    MCS_BFD_SESSION_STATE_DOWN         = 1,

    /*! Init. */
    MCS_BFD_SESSION_STATE_INIT         = 2,

    /*! Up. */
    MCS_BFD_SESSION_STATE_UP           = 3,

    /*! Last value, not usable. */
    MCS_BFD_SESSION_STATE_COUNT        = 4
} mcs_bfd_session_state_t;

/*! BFD Diagnostic Codes */
typedef enum mcs_bfd_diag_code_e {
    /*! None. */
    MCS_BFD_DIAG_CODE_NONE = 0,

    /*! Control detect time expired. */
    MCS_BFD_DIAG_CODE_CTRL_DETECT_TIME_EXPIRED = 1,

    /*! Echo failed. */
    MCS_BFD_DIAG_CODE_ECHO_FAILED = 2,

    /*! Neighbor signaled session down. */
    MCS_BFD_DIAG_CODE_NEIGHBOR_SIGNALED_SESSION_DOWN = 3,

    /*! Forwarding plane reset. */
    MCS_BFD_DIAG_CODE_FORWARDING_PLANE_RESET = 4,

    /*! Path down. */
    MCS_BFD_DIAG_CODE_PATH_DOWN = 5,

    /*! Concatenated path down. */
    MCS_BFD_DIAG_CODE_CONCATENATED_PATH_DOWN = 6,

    /*! Admin down. */
    MCS_BFD_DIAG_CODE_ADMIN_DOWN = 7,

    /*! Reverse concatenated path down. */
    MCS_BFD_DIAG_CODE_REVERSE_CONCATENATED_PATH_DOWN = 8,

    /*! Misconnectivity defect. */
    MCS_BFD_DIAG_CODE_MIS_CONNECTIVITY_DEFECT = 9,

    /*! Last value, not usable. */
    MCS_BFD_DIAG_CODE_COUNT = 10
} mcs_bfd_diag_code_t;

/*! BFD Authentication Types */
typedef enum mcs_bfd_auth_type_e {
    /*! None. */
    MCS_BFD_AUTH_TYPE_NONE = 0,

    /*! Simple password. */
    MCS_BFD_AUTH_TYPE_SIMPLE_PASSWORD = 1,

    /*! Keyed MD5. */
    MCS_BFD_AUTH_TYPE_KEYED_MD5 = 2,

    /*! Meticulous keyed MD5. */
    MCS_BFD_AUTH_TYPE_METICULOUS_KEYED_MD5 = 3,

    /*! Keyed SHA1. */
    MCS_BFD_AUTH_TYPE_KEYED_SHA1 = 4,

    /*! Meticulous Keyed SHA1. */
    MCS_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1 = 5,

    /*! Last value, not usable. */
    MCS_BFD_AUTH_TYPE_COUNT = 6
} mcs_bfd_auth_type_t;

/*! SHA1 Authentication key length */
#define MCS_BFD_AUTH_SHA1_KEY_LENGTH (20)

/*! Simple Password Authentication key length */
#define MCS_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH (16)

/*! MPLS-TP CV Source MEP-ID TLV length */
#define MCS_BFD_MAX_MEP_ID_LENGTH (32)

/*! Maximum length of the BFD encapsulation. */
#define MCS_BFD_MAX_ENCAP_LENGTH    162

/*! Local endpoint's state has changed to ADMIN_DOWN. */
#define MCS_BFD_EVENT_LOCAL_STATE_ADMIN_DOWN (1 << 0)
/*! The endpoint's state has changed to DOWN. */
#define MCS_BFD_EVENT_LOCAL_STATE_DOWN (1 << 1)
/*! The endpoint's state has changed to INIT. */
#define MCS_BFD_EVENT_LOCAL_STATE_INIT  (1 << 2)
/*! The endpoint's state has changed to UP. */
#define MCS_BFD_EVENT_LOCAL_STATE_UP (1 << 3)
/*! The remote endpoint's  state or the mode has changed. */
#define MCS_BFD_EVENT_REMOTE_STATE_MODE_CHANGE (1 << 4)
/*!  The remote endpoint's discriminator has changed. */
#define MCS_BFD_EVENT_REMOTE_DISCRIMINATOR_CHANGE (1 << 5)
/*! One or more parameters of the remote endpoint has changed. */
#define MCS_BFD_EVENT_REMOTE_PARAMETER_CHANGE (1 << 6)
/*! Received a BFD packet from the remote endpoint with the poll bit set. */
#define MCS_BFD_EVENT_REMOTE_POLL_BIT_SET (1 << 7)
/*! Received a BFD packet from the remote endpoint with the final bit set. */
#define MCS_BFD_EVENT_REMOTE_FINAL_BIT_SET (1 << 8)
/*! Misconnectivity defect detected. */
#define MCS_BFD_EVENT_MISCONNECTIVITY_DEFECT (1 << 9)
/*! Misconnectivity defect cleared. */
#define MCS_BFD_EVENT_MISCONNECTIVITY_DEFECT_CLEAR  (1 << 10)
/*! Unexpected MEG defect detected. */
#define MCS_BFD_EVENT_UNEXPECTED_MEG_DEFECT  (1 << 11)
/*! Unexpected MEG defect cleared. */
#define MCS_BFD_EVENT_UNEXPECTED_MEG_DEFECT_CLEAR  (1 << 12)

/*! Size of the metadata prefixed to the debug trace log buffer. */
#define MCS_BFD_TRACE_LOG_METADATA_SIZE 6

/*! Get the trace log size from the buffer. */
#define MCS_BFD_TRACE_LOG_SIZE(buf)    (buf)[4]

/*! Get the current index in the trace log buffer.  */
#define MCS_BFD_TRACE_LOG_CUR_IDX(buf) (buf)[5]

/*! Increment the current index in the trace log buffer.  */
#define MCS_BFD_TRACE_LOG_IDX_INCR(buf, idx) \
    ((idx) + 1) > MCS_BFD_TRACE_LOG_SIZE(buf) ? MCS_BFD_TRACE_LOG_METADATA_SIZE \
                                            : ((idx) + 1)

/*! Decrement the current index in the trace log buffer. */
#define MCS_BFD_TRACE_LOG_IDX_DECR(buf, idx) \
    ((idx) == MCS_BFD_TRACE_LOG_METADATA_SIZE) ? (MCS_BFD_TRACE_LOG_SIZE(buf) - 1) \
                                             : ((idx) - 1)

/*! BFD filenames, used for debugging, don't change the order */
#define MCS_BFD_FILENAMES              \
{                                      \
    "Invalid          ",               \
    "bfd_lt_control.c ",               \
    "bfd_rx.c         ",               \
    "bfd_shell.c      ",               \
    "bfd_static.c     ",               \
    "bfd_test.c       ",               \
    "bfd_timer.c      ",               \
    "bfd_tx.c         ",               \
    "bfd_uc_pack.c    ",               \
    "bfd_util.c       "                \
}

/*! Enums for BFD filenames, used for debugging, don't change the order */
typedef enum mcs_bfd_filenames_e {
    MCS_BFD_FILENAME_INVALID         = 0,
    MCS_BFD_FILENAME_BFD_CONTROL_C   = 1,
    MCS_BFD_FILENAME_BFD_RX_C        = 2,
    MCS_BFD_FILENAME_BFD_SHELL_C     = 3,
    MCS_BFD_FILENAME_BFD_STATIC_C    = 4,
    MCS_BFD_FILENAME_BFD_TEST_C      = 5,
    MCS_BFD_FILENAME_BFD_TIMER_C     = 6,
    MCS_BFD_FILENAME_BFD_TX_C        = 7,
    MCS_BFD_FILENAME_BFD_UC_PACK_C   = 8,
    MCS_BFD_FILENAME_BFD_UTIL_C      = 9,
    MCS_BFD_FILENAME_MAX
} mcs_bfd_filenames_t;

/*! Assign local discriminator internally. */
#define MCS_BFD_MSG_INIT_F_INTERNAL_LOCAL_DISCR (1 << 0)
/*! Static remote discriminator. */
#define MCS_BFD_MSG_INIT_F_STATIC_REMOTE_DISCR (1 << 1)

/*!
 * \brief BFD init message
 *
 * Initialize the BFD EApp
 */
typedef struct mcs_bfd_msg_ctrl_init_s {
    /*! Flags. */
    uint32_t flags;

    /*! Maximum number of BFD sessions. */
    uint32_t num_sessions;

    /*! Maximum number of SHA1 auth keys. */
    uint32_t num_auth_sha1_keys;

    /*! Maximum number of simple password auth keys. */
    uint32_t num_auth_sp_keys;

    /*! Maximum size of the Tx BFD PDU. */
    uint16_t max_pkt_size;
} mcs_bfd_msg_ctrl_init_t;

/*
 * BFD endpoint set message flags.
 */

/*! Flag to create a BFD endpoint. */
#define MCS_BFD_EP_SET_F_CREATE                     (1 << 0)

/*! Flag indicating that the endpoint is a micro BFD endpoint. */
#define MCS_BFD_EP_SET_F_MICRO_BFD                  (1 << 1)

/*! Flag indicating that the BFD endpoint is implemented over MPLS with PHP. */
#define MCS_BFD_EP_SET_F_MPLS_PHP                   (1 << 2)

/*! Flag indicating that the BFD packet will be raw injected. */
#define MCS_BFD_EP_SET_F_TX_RAW_INGRESS             (1 << 3)

/*! Flag indicating that the BFD packet is with loopback header. */
#define MCS_BFD_EP_SET_F_TX_LB_HDR                  (1 << 4)

/*
 * BFD endpoint set message update flags.
 */

/*! Flag to update the BFD local discriminator. */
#define MCS_BFD_EP_SET_UPDATE_F_LOCAL_DISC (1 << 0)

/*! Flag to update the local min Tx interval of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_MIN_TX (1 << 1)

/*! Flag to update the local min Rx interval of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_MIN_RX (1 << 2)

/*! Flag to update the min echo Rx interval of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_MIN_ECHO_RX (1 << 3)

/*! Flag to update the diag code of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_DIAG (1 << 4)

/*! Flag to update the demand mode of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_DEMAND (1 << 5)

/*! Flag to update the detect multiplier of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_DETECT_MULT (1 << 6)

/*! Flag to update the remote discriminator of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_REMOTE_DISC (1 << 7)

/*! Flag to update the authentication parameters of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_AUTH (1 << 8)

/*! Flag to enable/disable echo mode on a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_ECHO (1 << 9)

/*! Flag to update the source MEP Id length of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_SOURCE_MEP_ID_LENGTH (1 << 10)

/*! Flag to update the source MEP Id of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_SOURCE_MEP_ID (1 << 11)

/*! Flag to update the remote MEP Id length of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_REMOTE_MEP_ID_LENGTH (1 << 12)

/*! Flag to update the remote MEP Id of a BFD endpoint. */
#define MCS_BFD_EP_SET_UPDATE_F_REMOTE_MEP_ID (1 << 13)

/*! Flag indicating that the BFD endpoint's encapsulation is changing. */
#define MCS_BFD_EP_SET_UPDATE_F_ENCAP (1 << 14)

/*!
 * \brief BFD endpoint set message.
 *
 * Create or update a BFD endpoint.
 */
typedef struct mcs_bfd_msg_ctrl_ep_set_s {
    /*! Endpoint Id. */
    uint32_t endpoint_id;

    /*! Flags. */
    uint32_t flags;

    /*! Flags indicating which parameter was updated. */
    uint32_t update_flags;

    /*! Local discriminator. */
    uint32_t local_discr;

    /*! Remote discriminator. */
    uint32_t remote_discr;

    /*! Desired minimum Tx interval. */
    uint32_t local_min_tx;

    /*! Desired minimum Rx interval. */
    uint32_t local_min_rx;

    /*! Desired minimum echo Rx interval. */
    uint32_t local_min_echo_rx;

    /*! Authentication key. */
    uint32_t auth_key;

    /*! Authentication initial sequence number. */
    uint32_t xmt_auth_seq;

    /*! Incoming MPLS label. */
    uint32_t mpls_label;

    /*! Length of the encapsulation data. */
    uint16_t encap_length;

    /*! Length of the source MEP ID. */
    uint16_t source_mep_id_length;

    /*! Length of the remote MEP ID. */
    uint16_t remote_mep_id_length;

    /*! Port on which to transmit BFD packets. */
    uint16_t tx_port;

    /*! CoS on which the packet is sent out. */
    uint16_t tx_cos;

    /*! Rx packet VLAN ID. */
    uint16_t rx_pkt_vlan_id;

    /*! Lookup key ethertype. */
    uint16_t lkey_etype;

    /*! Lookup key offset. */
    uint16_t lkey_offset;

    /*! Lookup key length. */
    uint16_t lkey_length;

    /*! Encapsulation type (See mcs_bfd_encap_types_t). */
    uint8_t encap_type;

    /*! 1 - Enable echo mode, 0 - Don't enable echo mode. */
    uint8_t local_echo;

    /*! 1 - Passive mode, 0 - Active mode. */
    uint8_t passive;

    /*! 1 - Demand mode, 0 - Asynchrous mode. */
    uint8_t local_demand;

    /*! Local diagnostic code. */
    uint8_t local_diag;

    /*! Detection multiplier. */
    uint8_t local_detect_mult;

    /*! Authentication type. */
    uint8_t auth_type;

    /*! Encapsulation data. */
    uint8_t encap_data[MCS_BFD_MAX_ENCAP_LENGTH];

    /*! MPLS-TP CV Source MEP-ID. */
    uint8_t source_mep_id[MCS_BFD_MAX_MEP_ID_LENGTH];

    /*! MPLS-TP CV Remote MEP ID. */
    uint8_t remote_mep_id[MCS_BFD_MAX_MEP_ID_LENGTH];
} mcs_bfd_msg_ctrl_ep_set_t;

/*!
 * \brief BFD endpoint status get message.
 *
 * Get the status of a BFD endpoint.
 */
typedef struct mcs_bfd_msg_ctrl_ep_status_get_s {
    /*! Discriminator of the local endpoint. */
    uint32_t local_discr;

    /*! Discriminator of the remote endpoint. */
    uint32_t remote_discr;

    /*! Desired minimum Tx interval of the remote endpoint. */
    uint32_t remote_min_tx;

    /*! Desired minimim Rx interval of the remote endpoint. */
    uint32_t remote_min_rx;

    /*! Desired minimum Rx echo packet Rx interval of the remote enpoint. */
    uint32_t remote_min_echo_rx;

    /*! Last transmitted authentication sequence number. */
    uint32_t xmt_auth_seq;

    /*! Last received authentication sequence number. */
    uint32_t rcv_auth_seq;

    /*! Length of the mis-connectivity MEP ID. */
    uint16_t mis_conn_mep_id_length;

    /*! 1 - Poll sequence is active on the endpoint, 0 - Not active. */
    uint8_t poll;

    /*! 1 - Remote endpoint is in demand mode, 0 - Not in demand mode. */
    uint8_t remote_demand;

    /*! Diag code being transmitted by the local endpoint. */
    uint8_t local_diag;

    /*! Diag code recevied from the remote endpoint. */
    uint8_t remote_diag;

    /*! State of the local endpoint. */
    uint8_t local_state;

    /*! State of the remote endpoint. */
    uint8_t remote_state;

    /*! Detect multipler of the remote endpoint. */
    uint8_t remote_detect_mult;

    /*! Mis-connectivity MEP ID. */
    uint8_t mis_conn_mep_id[MCS_BFD_MAX_MEP_ID_LENGTH];
} mcs_bfd_msg_ctrl_ep_status_get_t;

/*! Indicates that pkts_in stats is valid */
#define MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_IN (1 << 0)

/*! Indicates that pkts_out stats is valid */
#define MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_OUT (1 << 1)

/*! Indicates that pkts_drop stats is valid */
#define MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_DROP (1 << 2)

/*! Indicates that pkts_auth_drop stats is valid */
#define MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_AUTH_DROP (1 << 3)

/*! Indicates that pkts_echo_reply stats is valid */
#define MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_ECHO_REPLY (1 << 4)

/*!
 * \brief BFD endpoint statistics set message.
 *
 * Set the statistics of a BFD endpoint.
 */
typedef struct mcs_bfd_msg_ctrl_ep_stats_set_s {
    /*! Indicates which stats are valid */
    uint32_t valid_bmp;

    /*! Endpoint Id. */
    uint32_t endpoint_id;

    /*! Number of received packets - upper 32 bits. */
    uint32_t pkts_in_hi;

    /*! Number of received packets - lower 32 bits. */
    uint32_t pkts_in_lo;

    /*! Number of transmitted packets - upper 32 bits. */
    uint32_t pkts_out_hi;

    /*! Number of transmitted packets - lower 32 bits. */
    uint32_t pkts_out_lo;

    /*! Number of dropped packets - upper 32 bits. */
    uint32_t pkts_drop_hi;

    /*! Number of dropped packets - lower 32 bits. */
    uint32_t pkts_drop_lo;

    /*!
     * Number of dropped packets due to authentication failures - upper 32
     * bits.
     */
    uint32_t pkts_auth_drop_hi;

    /*!
     * Number of dropped packets due to authentication failures - lower 32
     * bits.
     */
    uint32_t pkts_auth_drop_lo;

    /*! Number of echo reply packets received - upper 32 bits. */
    uint32_t pkts_echo_reply_hi;

    /*! Number of echo reply packets received - lower 32 bits. */
    uint32_t pkts_echo_reply_lo;
} mcs_bfd_msg_ctrl_ep_stats_set_t;

/*!
 * \brief BFD endpoint statistics get message.
 *
 * Get the statistics of a BFD endpoint.
 */
typedef mcs_bfd_msg_ctrl_ep_stats_set_t mcs_bfd_msg_ctrl_ep_stats_get_t;

/*! Indicates that ver_err stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_VER_ERR (1 << 0)

/*! Indicates that len_err stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_LEN_ERR (1 << 1)

/*! Indicates that zero_detect_mult stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_ZERO_DETECT_MULT (1 << 2)

/*! Indicates that zero_my_disc stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_ZERO_MY_DISC (1 << 3)

/*! Indicates that p_f_bit stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_P_F_BIT (1 << 4)

/*! Indicates that m_bit stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_M_BIT (1 << 5)

/*! Indicates that auth_type_mismatch stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_TYPE_MISMATCH (1 << 6)

/*! Indicates that auth_sp_err stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_SP_ERR (1 << 7)

/*! Indicates that auth_sha1_err stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_SHA1_ERR (1 << 8)

/*! Indicates that ep_not_found stats is valid */
#define MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_EP_NOT_FOUND (1 << 9)

/*!
 * \brief BFD global global statistics set message.
 *
 * Set the global Rx packet global statistics.
 */
typedef struct mcs_bfd_msg_ctrl_global_stats_set_s {
    /*! Indicates which stats are valid */
    uint32_t valid_bmp;

    /*! Number of packet received with version error - upper 32 bits. */
    uint32_t ver_err_hi;

    /*! Number of packet received with version error - lower 32 bits. */
    uint32_t ver_err_lo;

    /*! Number of packets received with invalid length - upper 32 bits. */
    uint32_t len_err_hi;

    /*! Number of packets received with invalid length - lower 32 bits. */
    uint32_t len_err_lo;

    /*!
     * Number of packets received with detect multiplier set to 0 - upper 32
     * bits.
     */
    uint32_t zero_detect_mult_hi;

    /*!
     * Number of packets received with detect multiplier set to 0 - lower 32
     * bits.
     */
    uint32_t zero_detect_mult_lo;

    /*!
     * Number of packets received with My Discriminator field set to 0 - upper
     * 32 bits.
     */
    uint32_t zero_my_disc_hi;

    /*!
     * Number of packets received with My Discriminator field set to 0 - lower
     * 32 bits.
     */
    uint32_t zero_my_disc_lo;

    /*!
     * Number of packets received with P (Poll) and F (Final) bits set - upper
     * 32 bits.
     */
    uint32_t p_f_bit_hi;

    /*!
     * Number of packets received with P (Poll) and F (Final) bits set - lower
     * 32 bits.
     */
    uint32_t p_f_bit_lo;

    /*!
     * Number of packets received with M (Multipoint) bit set - upper 32 bits.
     */
    uint32_t m_bit_hi;

    /*!
     * Number of packets received with M (Multipoint) bit set - lower 32 bits.
 */
    uint32_t m_bit_lo;

    /*!
     * Number of packets received with incorrect authentication type - upper 32
     * bits.
     */
    uint32_t auth_type_mismatch_hi;

    /*!
     * Number of packets received with incorrect authentication type - lower 32
     * bits.
     */
    uint32_t auth_type_mismatch_lo;

    /*!
     * Number of packets received with simple password authentication error
     * - upper 32 bits.
     */
    uint32_t auth_sp_err_hi;

    /*!
     * Number of packets received with simple password authentication error
     * - lower 32 bits.
     */
    uint32_t auth_sp_err_lo;

    /*!
     * Number of packets received with SHA1 authentication error - upper 32 bits.
     */
    uint32_t auth_sha1_err_hi;

    /*!
     * Number of packets received with SHA1 authentication error - lower 32 bits.
     */
    uint32_t auth_sha1_err_lo;

    /*!
     * Number of packets received whose endpoints could not be resolved - upper
     * 32 bits.
     */
    uint32_t ep_not_found_hi;

    /*!
     * Number of packets received whose endpoints could not be resolved - lower
     * 32 bits.
     */
    uint32_t ep_not_found_lo;
} mcs_bfd_msg_ctrl_global_stats_set_t;

/*!
 * \brief BFD global global statistics get message.
 *
 * Get the global Rx packet global statistics.
 */
typedef mcs_bfd_msg_ctrl_global_stats_set_t mcs_bfd_msg_ctrl_global_stats_get_t;

/*!
 * \brief BFD SHA1 authentication set message.
 *
 * Set the key set for SHA1 authentication.
 */
typedef struct mcs_bfd_msg_ctrl_auth_sha1_set_s {
    /*! Index in the SHA1 key table. */
    uint32_t index;

    /*! SHA1 key. */
    uint8_t key[MCS_BFD_AUTH_SHA1_KEY_LENGTH];
} mcs_bfd_msg_ctrl_auth_sha1_set_t;

/*!
 * \brief BFD Simple Password Authentication set message.
 *
 * Set the password used for simple password authentication.
 */
typedef struct mcs_bfd_msg_ctrl_auth_sp_set_s {
    /*! Index in the simple password table. */
    uint32_t index;

    /*! Length of the simple password. */
    uint8_t length;

    /*! Password. */
    uint8_t password[MCS_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH];
} mcs_bfd_msg_ctrl_auth_sp_set_t;

/*!
 * \brief BFD debug trace enable reply message.
 *
 * Reply message after enabling the debug trace log containing the log buffer
 * memory address.
 */
typedef struct mcs_bfd_msg_ctrl_trace_log_enable_reply_s {
    /*!
     * Upper 32 bits of the log buffer address (currently unused as ARM
     * addresses are 32b).
     */
    uint32_t addr_hi;

    /*! Lower 32 bits of the log buffer address. */
    uint32_t addr_lo;
} mcs_bfd_msg_ctrl_trace_log_enable_reply_t;

/*! Set endpoint state as down during create. */
#define MCS_BFD_MSG_GLOBAL_PARAM_EP_STATE_DOWN_ON_CREATE (1)
/*! Set/unset control plane independence. */
#define MCS_BFD_MSG_GLOBAL_PARAM_CONTROL_PLANE_INDEPENDENCE (2)
/*! Set/unset tx of BFD packets from endpoints. */
#define MCS_BFD_MSG_GLOBAL_PARAM_TX (3)

/*!
 * \brief BFD global param set message.
 *
 * Update global parameters.
 */
typedef struct mcs_bfd_msg_ctrl_global_param_set_s {
    /*! Param to be updated. */
    uint32_t param;

    /*! Parameter value. */
    uint32_t value;
} mcs_bfd_msg_ctrl_global_param_set_t;

/*! Rx PMD flex fields used by BFD EApp. */
typedef enum mcs_bfd_rxpmd_flex_fields_s {
    /*! I2E class ID [15:0] bits. */
    MCS_BFD_RXPMD_FLEX_FIELD_I2E_CLASS_ID_15_0 = 0,

    

    /*! Last value, not usable. */
    MCS_BFD_RXPMD_FLEX_FIELD_COUNT
} mcs_bfd_rxpmd_flex_fields_t;

/*! Max number of Flex Flow IDs. */
#define MCS_BFD_MAX_FLEX_FLOW_IDS (128)

/*!
 * \brief BFD Rx PMD flex format.
 *
 * Contains the offsets and widths of the flex fields in Rx PMD.
 */
typedef struct mcs_bfd_msg_ctrl_rxpmd_flex_format_s {
    /*! Start offset of the fields. */
    uint16_t offsets[MCS_BFD_RXPMD_FLEX_FIELD_COUNT][MCS_BFD_MAX_FLEX_FLOW_IDS];

    /*! Widths of the fields. */
    uint8_t widths[MCS_BFD_RXPMD_FLEX_FIELD_COUNT][MCS_BFD_MAX_FLEX_FLOW_IDS];

    /*! Number of flow IDs. */
    uint16_t num_flow_ids;

    /*! Number of fields. */
    uint8_t num_fields;
} mcs_bfd_msg_ctrl_rxpmd_flex_format_t;

/*! Match ID fields used by BFD EApp. */
typedef enum mcs_bfd_match_id_fields_s {
    /*! Inner vlan tag. */
    MCS_BFD_MATCH_ID_OTAG = 0,

    /*! Inner vlan tag. */
    MCS_BFD_MATCH_ID_ITAG = 1,

    /*! MPLS label-0 */
    MCS_BFD_MATCH_ID_MPLS_0 = 2,

    /*! MPLS label-1 */
    MCS_BFD_MATCH_ID_MPLS_1 = 3,

    /*! MPLS label-2 */
    MCS_BFD_MATCH_ID_MPLS_2 = 4,

    /*! MPLS label-3 */
    MCS_BFD_MATCH_ID_MPLS_3 = 5,

    /*! MPLS label-4 */
    MCS_BFD_MATCH_ID_MPLS_4 = 6,

    /*! MPLS label-5 */
    MCS_BFD_MATCH_ID_MPLS_5 = 7,

    /*! MPLS label-6 */
    MCS_BFD_MATCH_ID_MPLS_6 = 8,

    /*! MPLS Associated Channel Header. */
    MCS_BFD_MATCH_ID_ACH = 9,

    /*! Inner IPv4. */
    MCS_BFD_MATCH_ID_INNER_IPV4 = 10,

    /*! Inner IPv6. */
    MCS_BFD_MATCH_ID_INNER_IPV6 = 11,

    /*! Generic Routing Encapsulation. */
    MCS_BFD_MATCH_ID_GRE = 12,

    /*! Last value, not usable. */
    MCS_BFD_MATCH_ID_FIELD_COUNT
} mcs_bfd_match_id_fields_t;

/*!
 * \brief BFD match Id format set message.
 *
 * Set the match Id format.
 */
typedef struct mcs_bfd_msg_ctrl_match_id_set_s {
    /*! Number of match Id fields. */
    uint16_t num_fields;

    /*! Offset of the fields in the match ID. */
    uint16_t offsets[MCS_BFD_MATCH_ID_FIELD_COUNT];

    /*! Width of the fields in the match ID. */
    uint16_t widths[MCS_BFD_MATCH_ID_FIELD_COUNT];

    /*! Value of the fields in the match ID. */
    uint16_t values[MCS_BFD_MATCH_ID_FIELD_COUNT];
} mcs_bfd_msg_ctrl_match_id_set_t;

/*!
 * \brief BFD messages.
 *
 * Union of all BFD messages, used to size the DMA buffer.
 */
typedef union mcs_bfd_msg_ctrl_s {
    /*! Init message */
    mcs_bfd_msg_ctrl_init_t init;

    /*! Endpoint set message. */
    mcs_bfd_msg_ctrl_ep_set_t ep_set;

    /*! Endpoint status get message. */
    mcs_bfd_msg_ctrl_ep_status_get_t ep_status_get;

    /*! Endpoint statistics set message. */
    mcs_bfd_msg_ctrl_ep_stats_set_t ep_stats_set;

    /*! Endpoint statistics get message. */
    mcs_bfd_msg_ctrl_ep_stats_get_t ep_stats_get;

    /*! Global global statistics get message. */
    mcs_bfd_msg_ctrl_global_stats_get_t global_stats_get;

    /*! SHA1 key set. */
    mcs_bfd_msg_ctrl_auth_sha1_set_t sha1;

    /*! Simple password set. */
    mcs_bfd_msg_ctrl_auth_sp_set_t sp;

    /*! Trace log enable reply message/ */
    mcs_bfd_msg_ctrl_trace_log_enable_reply_t trace_log;

    /*! Set global parameters. */
    mcs_bfd_msg_ctrl_global_param_set_t global_param_set;

    /*! Rx PMD flex format set message. */
    mcs_bfd_msg_ctrl_rxpmd_flex_format_t rxpmd_flex_format;

    /*! Match Id format set message. */
    mcs_bfd_msg_ctrl_match_id_set_t match_id_set;
} mcs_bfd_msg_ctrl_t;

#endif /* MCS_BFD_COMMON_H */
