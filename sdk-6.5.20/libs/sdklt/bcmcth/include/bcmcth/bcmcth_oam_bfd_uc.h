/*! \file bcmcth_oam_bfd_uc.h
 *
 * BCMCTH OAM uC interface APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_OAM_BFD_UC_H
#define BCMCTH_OAM_BFD_UC_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util_pack.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmltd/chip/bcmltd_oam_constants.h>

/*! Structure representation of the operational fields in BFD_CONTROL. */
typedef struct bcmcth_oam_bfd_control_oper_s {
    /*! Corresponds to MAX_ENDPOINTS_OPER field in the LT. */
    uint32_t max_endpoints;

    /*! Corresponds to MAX_AUTH_SHA1_KEYS_OPER field in the LT. */
    uint32_t max_auth_sha1_keys;

    /*! Corresponds to MAX_AUTH_SIMPLE_PASSWORD_KEYS_OPER field in the LT. */
    uint32_t max_auth_sp_keys;

    /*! Corresponds to MAX_PKT_SIZE_OPER field in the LT. */
    uint16_t max_pkt_size;

    /*! Corresponds to INTERNAL_LOCAL_DISCRIMINATOR_OPER field in the LT. */
    bool internal_local_discr;

    /*! Corresponds to STATIC_REMOTE_DISCRIMINATOR_OPER field in the LT. */
    bool static_remote_discr;

    /*! Corresponds to OPERATIONAL_STATE field in the LT. */
    bcmltd_common_oam_bfd_control_state_t_t state;
} bcmcth_oam_bfd_control_oper_t;

/*! Data structure for OAM_BFD_CONTROL LT. */
typedef struct bcmcth_oam_bfd_control_s {
    /*! Bitmap of modified field Ids. */
    SHR_BITDCLNAME(fbmp, OAM_BFD_CONTROLt_FIELD_COUNT);

    /*! Corresponds to BFD field in the LT. */
    bool bfd;

    /*! Corresponds to MAX_ENDPOINTS field in the LT. */
    uint32_t max_endpoints;

    /*! corresponds to MAX_AUTH_SHA1_KEYS field in the LT. */
    uint32_t max_auth_sha1_keys;

    /*! Corresponds to MAX_AUTH_SIMPLE_PASSWORD_KEYS field in the LT. */
    uint32_t max_auth_sp_keys;

    /*! Corresponds to MAX_PKT_SIZE field in the LT. */
    uint16_t max_pkt_size;

    /*! Corresponds to INTERNAL_LOCAL_DISCRIMINATOR field in the LT. */
    bool internal_local_discr;

    /*! Corresponds to STATIC_REMOTE_DISCRIMINATOR field in the LT. */
    bool static_remote_discr;

    /*!
     * Corresponds to LOCAL_STATE_DOWN_EVENT_ON_ENDPOINT_CREATION field in
     * the LT.
     */
    bool local_state_down_event_on_endpoint_creation;

    /*! Corresponds to CONTROL_PLANE_INDEPENDENCE field in the LT. */
    bool control_plane_independence;

    /*! Corresponds to TX field in the LT. */
    bool tx;

    /*! Read only operational fields in the LT. */
    bcmcth_oam_bfd_control_oper_t oper;
} bcmcth_oam_bfd_control_t;

/*! Structure representation of OAM_BFD_STATS table. */
typedef struct bcmcth_oam_bfd_stats_s {
    /*! Bitmap of modified field Ids. */
    SHR_BITDCLNAME(fbmp, OAM_BFD_STATSt_FIELD_COUNT);

    /*! Number of packets received with version error. */
    uint64_t rx_pkt_version_error;

    /*! Number of packets received with invalid length. */
    uint64_t rx_pkt_length_error;

    /*! Number of packets received with detect multiplier set to 0. */
    uint64_t rx_pkt_zero_detect_multiplier;

    /*! Number of packets received with My Discriminator field set to 0. */
    uint64_t rx_pkt_zero_my_discr;

    /*! Number of packets received with P (Poll) and F (Final) bits set. */
    uint64_t rx_pkt_p_and_f_bits_set;

    /*! Number of packets received with M (Multipoint) bit set. */
    uint64_t rx_pkt_m_bit_set;

    /*! Number of packets received with incorrect authentication type. */
    uint64_t rx_pkt_auth_mismatch;

    /*!
     * Number of packets received with simple password authentication error.
     */
    uint64_t rx_pkt_sp_auth_error;

    /*! Number of packets received with SHA1 authentication error. */
    uint64_t rx_pkt_sha1_auth_error;

    /*! Number of packets received whose endpoints could not be resolved. */
    uint64_t rx_pkt_endpoint_not_found;
} bcmcth_oam_bfd_stats_t;

/*! Structure representation of OAM_BFD_AUTH_SIMPLE_PASSWORD table. */
typedef struct bcmcth_oam_bfd_auth_sp_s {
    /*! Corresponds to OAM_BFD_AUTH_SIMPLE_PASSWORD_ID field in the LT. */
    uint32_t id;

    /*! Corresponds to PASSWORD_LEN field in the LT. */
    uint8_t password_len;

    /*! Corresponds to PASSWORD field in the LT. */
    uint8_t password[SIMPLE_PASSWORD_MAX_LENGTH];

    /*! Operational state. */
    bcmltd_common_oam_bfd_auth_state_t_t oper_state;
} bcmcth_oam_bfd_auth_sp_t;

/*! Structure representation of OAM_BFD_AUTH_SHA1 table. */
typedef struct bcmcth_oam_bfd_auth_sha1_s {
    /*! Corresponds to OAM_BFD_AUTH_SHA1_ID field in the table. */
    uint32_t id;

    /*! Corresponds to SHA1_KEY field in the table. */
    uint8_t sha1_key[SHA1_KEY_LENGTH];

    /*! Operational state. */
    bcmltd_common_oam_bfd_auth_state_t_t oper_state;
} bcmcth_oam_bfd_auth_sha1_t;

/*! IPv4 BFD endpoint specific information. */
typedef struct bcmcth_oam_bfd_endpoint_ipv4_s {
    /*! Source IPv4 address. */
    shr_ip_t src_ip;

    /*! Destination IPv4 address. */
    shr_ip_t dst_ip;

    /*! IPv4 type of service. */
    uint8_t tos;

    /*! IP TTL. */
    uint8_t ttl;

    /*! Type of IP endpoint. */
    bcmltd_common_oam_bfd_ip_endpoint_type_t_t ip_ep_type;

    /*! Rx lookup VLAN ID. */
    uint16_t rx_lookup_vlan_id;
} bcmcth_oam_bfd_endpoint_ipv4_t;

/*! IPv6 BFD endpoint specific information. */
typedef struct bcmcth_oam_bfd_endpoint_ipv6_s {
    /*! Source IPv6 address. */
    shr_ip6_t src_ip;

    /*! Destination IPv6 address. */
    shr_ip6_t dst_ip;

    /*! IPv6 traffic class. */
    uint8_t traffic_class;

    /*! IPv6 hop limit. */
    uint8_t hop_limit;

    /*! Type of IP endpoint. */
    bcmltd_common_oam_bfd_ip_endpoint_type_t_t ip_ep_type;

    /*! Rx lookup VLAN ID. */
    uint16_t rx_lookup_vlan_id;
} bcmcth_oam_bfd_endpoint_ipv6_t;

/*! Tunnel IPv4 BFD endpoint specific information. */
typedef struct bcmcth_oam_bfd_endpoint_tnl_ipv4_s {
    /*! Source IPv4 address. */
    shr_ip_t src_ip;

    /*! Destination IPv4 address. */
    shr_ip_t dst_ip;

    /*! IPv4 type of service. */
    uint8_t tos;

    /*! IPv4 TTL. */
    uint8_t ttl;

    /*! Indicates if GRE header needs to be added to the packet. */
    bool gre;

    /*! Type of the inner IP header. */
    bcmltd_common_oam_bfd_tnl_ip_inner_ip_type_t_t inner_ip_type;

    /*! Inner IPv4 encapsulation. */
    struct {
        /*! Source IPv4 address. */
        shr_ip_t src_ip;

        /*! Destination IPv4 address. */
        shr_ip_t dst_ip;

        /*! IPv4 type of service. */
        uint8_t tos;
    } inner_ipv4;

    /*! Inner IPv6 encapsulation. */
    struct {
        /*! Source IPv6 address. */
        shr_ip6_t src_ip;

        /*! Destination IPv6 address. */
        shr_ip6_t dst_ip;

        /*! IPv6 traffic class. */
        uint8_t traffic_class;
    } inner_ipv6;

    /*! Rx lookup VLAN ID. */
    uint16_t rx_lookup_vlan_id;
} bcmcth_oam_bfd_endpoint_tnl_ipv4_t;

/*! Tunnel IPv6 BFD endpoint specific information. */
typedef struct bcmcth_oam_bfd_endpoint_tnl_ipv6_s {
    /*! Source IPv4 address. */
    shr_ip6_t src_ip;

    /*! Destination IPv6 address. */
    shr_ip6_t dst_ip;

    /*! IPv6 traffic class. */
    uint8_t traffic_class;

    /*! IPv6 hop limit. */
    uint8_t hop_limit;

    /*! Indicates if GRE header needs to be added to the packet. */
    bool gre;

    /*! Type of the inner IP header. */
    bcmltd_common_oam_bfd_tnl_ip_inner_ip_type_t_t inner_ip_type;

    /*! Inner IPv4 encapsulation. */
    struct {
        /*! Source IPv4 address. */
        shr_ip_t src_ip;

        /*! Destination IPv4 address. */
        shr_ip_t dst_ip;

        /*! IPv4 type of service. */
        uint8_t tos;
    } inner_ipv4;

    /*! Inner IPv6 encapsulation. */
    struct {
        /*! Source IPv6 address. */
        shr_ip6_t src_ip;

        /*! Destination IPv6 address. */
        shr_ip6_t dst_ip;

        /*! IPv6 traffic class. */
        uint8_t traffic_class;
    } inner_ipv6;

    /*! Rx lookup VLAN ID. */
    uint16_t rx_lookup_vlan_id;
} bcmcth_oam_bfd_endpoint_tnl_ipv6_t;

/*! Tunnel MPLS BFD endpoint specific information. */
typedef struct bcmcth_oam_bfd_endpoint_tnl_mpls_s {
    /*! Type of MPLS encapsulation. */
    bcmltd_common_oam_bfd_tnl_mpls_endpoint_encap_type_t_t encap_type;

    /*! Type of IP encapsulation. */
    bcmltd_common_oam_bfd_tnl_mpls_endpoint_ip_encap_type_t_t ip_encap_type;

    /*! Rx MPLS lookup label. */
    uint32_t rx_lookup_label;

    /*! Number of MPLS labels. */
    uint8_t num_labels;

    /*! Label stack. */
    uint32_t label_stack[MAX_MPLS_LABELS];

    /*! Indicates if pseudowire ACH is present in the transmit packet. */
    bool pw_ach;

    /*! IPv4 encapsulation. */
    struct {
        /*! Source IPv4 address. */
        shr_ip_t src_ip;

        /*! Destination IPv4 address. */
        shr_ip_t dst_ip;

        /*! IPv4 type of service. */
        uint8_t tos;
    } ipv4;

    /*! IPv6 encapsulation. */
    struct {
        /*! Source IPv6 address. */
        shr_ip6_t src_ip;

        /*! Destination IPv6 address. */
        shr_ip6_t dst_ip;

        /*! IPv6 traffic class. */
        uint8_t traffic_class;
    } ipv6;

    /*! Length of the source MEP ID. */
    uint8_t source_mep_id_len;

    /*! Source MEP ID. */
    uint8_t source_mep_id[MEP_IDENTIFIER_MAX_LENGTH];

    /*! Length of the remote MEP ID. */
    uint8_t remote_mep_id_len;

    /*! Remote MEP ID. */
    uint8_t remote_mep_id[MEP_IDENTIFIER_MAX_LENGTH];
} bcmcth_oam_bfd_endpoint_tnl_mpls_t;

/*! Structure representation of all the different kinds of BFD endpoints. */
typedef struct bcmcth_oam_bfd_endpoint_s {
    /*! Type of the BFD endpoint. */
    bcmltd_common_oam_bfd_endpoint_type_t_t ep_type;

    /*! Endpoint Id. {sid, endpoint_id} form an unique pair. */
    uint32_t endpoint_id;

    /*! Role played by the endpoint in session initialization. */
    bcmltd_common_oam_bfd_endpoint_session_init_role_t_t role;

    /*! Endpoint mode. */
    bcmltd_common_oam_bfd_endpoint_mode_t_t mode;

    /*! Echo function on endpoint. */
    bool echo;

    /*! Local discriminator. */
    uint32_t local_discr;

    /*! Remote discriminator. */
    uint32_t remote_discr;

    /*! Diag code. */
    bcmltd_common_oam_bfd_diag_code_t_t diag_code;

    /*! Minimum Tx interval in usecs. */
    uint32_t min_tx_interval_usecs;

    /*! Minium Rx interval in usecs. */
    uint32_t min_rx_interval_usecs;

    /*! Minimum echo interval in usecs. */
    uint32_t min_echo_rx_interval_usecs;

    /*! Detection mulitpler. */
    uint8_t detect_multiplier;

    /*! Authentication type. */
    bcmltd_common_oam_bfd_auth_type_t_t auth_type;

    /*! Indicates if the simple password authentication entry was found. */
    bool auth_sp_found;

    /*! Simple password authentication. */
    bcmcth_oam_bfd_auth_sp_t sp;

    /*! Indicates if the SHA1 authentication entry was found. */
    bool auth_sha1_found;

    /*! SHA1 authentication. */
    bcmcth_oam_bfd_auth_sha1_t sha1;

    /*! Initial SHA1 sequence number. */
    uint32_t initial_sha1_seq_num;

    /*! Increment SHA1 sequence number. */
    bool sha1_seq_num_incr;

    /*! Initiate poll sequence. */
    bool poll_sequence;

    /*! Tx port. */
    shr_port_t port;

    /*! CoS. */
    uint8_t cos;

    /*! Destination MAC. */
    shr_mac_t dst_mac;

    /*! Source MAC. */
    shr_mac_t src_mac;

    /*! Number of vlans - 0/1/2. */
    uint8_t num_vlans;

    /*! Outer VLAN Id. */
    shr_util_pack_vlan_tag_t outer_vlan;

    /*! Outer VLAN Id. */
    shr_util_pack_vlan_tag_t inner_vlan;

    /*! Endpoint type specific information. */
    union {
        /*! IPv4 endpoint information. */
        bcmcth_oam_bfd_endpoint_ipv4_t ipv4;

        /*! IPv6 endpoint information. */
        bcmcth_oam_bfd_endpoint_ipv6_t ipv6;

        /*! Tunnel IPv4 endpoint information. */
        bcmcth_oam_bfd_endpoint_tnl_ipv4_t tnl_ipv4;

        /*! Tunnel IPv6 endpoint information. */
        bcmcth_oam_bfd_endpoint_tnl_ipv6_t tnl_ipv6;

        /*! Tunnel MPLS endpoint information. */
        bcmcth_oam_bfd_endpoint_tnl_mpls_t tnl_mpls;
    } type;

    /*! UDP source port. */
    uint16_t udp_src_port;

    /*! Transmission mode. */
    bcmltd_common_oam_bfd_endpoint_tx_mode_t_t tx_mode;

    /*! Operational state. */
    bcmltd_common_oam_bfd_endpoint_state_t_t oper_state;
} bcmcth_oam_bfd_endpoint_t;

/*! Structure representation of the OAM_BFD_XXX_ENDPOINT_STATUS LT. */
typedef struct bcmcth_oam_bfd_endpoint_status_s {
    /*! Local endpoint state. */
    bcmltd_common_oam_bfd_session_state_t_t local_state;

    /*! Local endpoint state. */
    bcmltd_common_oam_bfd_session_state_t_t remote_state;

    /*! Local endpoint diag code. */
    bcmltd_common_oam_bfd_diag_code_t_t local_diag_code;

    /*! Remote endpoint diag code. */
    bcmltd_common_oam_bfd_diag_code_t_t remote_diag_code;

    /*!
     * Last authentication sequence number transmitted by the local endpoint.
     */
    uint32_t local_auth_seq_num;

    /*! Last authentication sequence number received from the local endpoint. */
    uint32_t remote_auth_seq_num;

    /*! Indicates if the poll sequence is currently active. */
    bool poll_sequence_active;

    /*! Remote endpoint mode. */
    bcmltd_common_oam_bfd_endpoint_mode_t_t remote_mode;

    /*! Discriminator of the local endpoint. */
    uint32_t local_discr;

    /*! Discriminator of the remote endpoint. */
    uint32_t remote_discr;

    /*! Detection interval multiplier of the remote endpoint. */
    uint8_t remote_detect_multiplier;

    /*!
     * Desired minimum transmit interval in microseconds of the remote
     * endpoint.
     */
    uint32_t remote_min_tx_interval_usecs;

    /*!
     * Required minimum receive interval in microseconds of the remote
     * endpoint.
     */
    uint32_t remote_min_rx_interval_usecs;

    /*!
     * Minimum echo packet receive interval in microseconds of the remote
     * endpoint.
     */
    uint32_t remote_min_echo_rx_interval_usecs;

    /*! Length of the Misconnectivity MEP ID. */
    uint8_t misconn_mep_id_len;

    /*! Misconnectivity MEP ID. */
    uint8_t misconn_mep_id[MEP_IDENTIFIER_MAX_LENGTH];

    /*! Endpoint ID state. */
    bcmltd_common_oam_bfd_endpoint_id_state_t_t endpoint_state;
} bcmcth_oam_bfd_endpoint_status_t;

/*! Structure representation of OAM_BFD_XXX_ENDPOINT_STATS LT. */
typedef struct bcmcth_oam_bfd_endpoint_stats_s {
    /*! Indicates that the rx_pkt_cnt field was updated. */
    bool rx_pkt_cnt_update;

    /*! Total number of packets received by the endpoint. */
    uint64_t rx_pkt_cnt;

    /*! Indicates that the rx_pkt_discard_cnt field was updated. */
    bool rx_pkt_discard_cnt_update;

    /*!
     * Total number of received packets that were discarded by the endpoint.
     */
    uint64_t rx_pkt_discard_cnt;

    /*! Indicates that the rx_pkt_auth_failure_discard_cnt field was updated. */
    bool rx_pkt_auth_failure_discard_cnt_update;

    /*!
     * Total number of received packets that were discarded by the endpoint
     * due to authentication failures.
     */
    uint64_t rx_pkt_auth_failure_discard_cnt;

    /*! Indicates that the rx_echo_reply_pkt_cnt field was updated. */
    bool rx_echo_reply_pkt_cnt_update;

    /*! Total number of echo reply packets received by the endpoint. */
    uint64_t rx_echo_reply_pkt_cnt;

    /*! Indicates that the tx_pkt_cnt field was updated. */
    bool tx_pkt_cnt_update;

    /*! Total number of packets transmitted by the endpoint. */
    uint64_t tx_pkt_cnt;
} bcmcth_oam_bfd_endpoint_stats_t;

/*! Structure representation of the OAM_BFD_EVENT_CONTROL LT. */
typedef struct bcmcth_oam_bfd_event_control_s {
    /*! Corresponds to LOCAL_STATE_ADMIN_DOWN field in the LT. */
    bool local_state_admin_down;

    /*! Corresponds to LOCAL_STATE_DOWN field in the LT. */
    bool local_state_down;

    /*! Corresponds to LOCAL_STATE_INIT field in the LT. */
    bool local_state_init;

    /*! Corresponds to LOCAL_STATE_UP field in the LT. */
    bool local_state_up;

    /*! Corresponds to REMOTE_STATE_MODE_CHANGE field in the LT. */
    bool remote_state_mode_change;

    /*! Corresponds to REMOTE_DISCRIMINATOR_CHANGE field in the LT. */
    bool remote_discr_change;

    /*! Corresponds to REMOTE_PARAMETER_CHANGE field in the LT. */
    bool remote_param_change;

    /*! Corresponds to REMOTE_POLL_BIT_SET field in the LT. */
    bool remote_poll_bit_set;

    /*! Corresponds to REMOTE_FINAL_BIT_SET field in the LT. */
    bool remote_final_bit_set;

    /*! Corresponds to MISCONNECTIVITY_DEFECT field in the LT. */
    bool misconn_defect;

    /*! Corresponds to MISCONNECTIVITY_DEFECT_CLEAR field in the LT. */
    bool misconn_clear;

    /*! Corresponds to UNEXPECTED_MEG_DEFECT field in the LT. */
    bool unexpected_meg_defect;

    /*! Corresponds to UNEXPECTED_MEG_DEFECT_CLEAR field in the LT. */
    bool unexpected_meg_defect_clear;
} bcmcth_oam_bfd_event_control_t;


/*!
 * \brief Alloc memory required for the BFD EApp.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating whether it is a cold or a warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_uc_alloc(int unit, bool warm);

/*!
 * \brief Free memory used by the BFD EApp.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmcth_oam_bfd_uc_free(int unit);

/*!
 * \brief OAM_BFD_CONTROL LT insert handler.
 *
 * This function handles the INSERT opcode for the OAM_BFD_CONTROL LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the BFD control entry. The
 *                   fields not filled in by user are populated with the default
 *                   values.
 * \param [out] oper Structure containing the operational values that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_control_insert(int unit,
                              bcmcth_oam_bfd_control_t *entry,
                              bcmcth_oam_bfd_control_oper_t *oper);

/*!
 * \brief OAM_BFD_CONTROL LT update handler.
 *
 * This function handles the UPDATE opcode for the OAM_BFD_CONTROL LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure representing the view of the IMM entry after the
 *                 update operation has completed.
 * \param [in] prev Structure representing the current view of the IMM entry
 *                  with defaults populated for the unfilled values.
 * \param [out] oper Structure containing the operational values that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_control_update(int unit,
                              bcmcth_oam_bfd_control_t *cur,
                              bcmcth_oam_bfd_control_t *prev,
                              bcmcth_oam_bfd_control_oper_t *oper);

/*!
 * \brief OAM_BFD_CONTROL LT delete handler.
 *
 * This function handles the DELETE opcode for the OAM_BFD_CONTROL LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_control_delete(int unit);

/*!
 * \brief OAM_BFD_STATS LT update handler.
 *
 * This function handles the UPDATE opcode for the OAM_BFD_STATS LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the BFD stats entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_stats_update(int unit,
                            bcmcth_oam_bfd_stats_t *entry);

/*!
 * \brief OAM_BFD_STATS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for the OAM_BFD_STATS LT. This
 * function will retreive the stats from the app.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the BFD stats entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_stats_lookup(int unit,
                            bcmcth_oam_bfd_stats_t *entry);
/*!
 * \brief OAM_BFD_AUTH_SIMPLE_PASSWORD LT insert handler.
 *
 * This function handles the INSERT opcode for the simple password
 * authentication LT. This function will inform the app about the changes by
 * sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the simple password
 *                   authentication entry.
 * \param [out] oper_state Operational state of the entry.

 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_sp_insert(int unit,
                              bcmcth_oam_bfd_auth_sp_t *entry,
                              bcmltd_common_oam_bfd_auth_state_t_t *oper_state);

/*!
 * \brief OAM_BFD_AUTH_SIMPLE_PASSWORD LT update handler.
 *
 * This function handles the UPDATE opcode for the simple password
 * authentication LT. This function will inform the app about the changes by
 * sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the simple password
 *                   authentication entry.
 * \param [out] oper_state Operational state of the entry.

 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_sp_update(int unit,
                              bcmcth_oam_bfd_auth_sp_t *entry,
                              bcmltd_common_oam_bfd_auth_state_t_t *oper_state);

/*!
 * \brief OAM_BFD_AUTH_SIMPLE_PASSWORD LT delete handler.
 *
 * This function handles the DELETE opcode for the simple password
 * authentication LT. This function will inform the app about the changes by
 * sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] sp_id Index of the simple password authentication entry to be
 *                   deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_sp_delete(int unit, uint32_t sp_id);

/*!
 * \brief OAM_BFD_AUTH_SHA1 LT insert handler.
 *
 * This function handles the INSERT opcode for the SHA1 authentication LT. This
 * function will inform the app about the changes bysending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the simple password
 *                   authentication entry.
 * \param [out] oper_state Operational state of the entry.

 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_sha1_insert(int unit,
                                bcmcth_oam_bfd_auth_sha1_t *entry,
                                bcmltd_common_oam_bfd_auth_state_t_t *oper_state);

/*!
 * \brief OAM_BFD_AUTH_SHA1 LT update handler.
 *
 * This function handles the UPDATE opcode for the SHA1 authentication LT. This
 * function will inform the app about the changes bysending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the simple password
 *                   authentication entry.
 * \param [out] oper_state Operational state of the entry.

 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_sha1_update(int unit,
                                bcmcth_oam_bfd_auth_sha1_t *entry,
                                bcmltd_common_oam_bfd_auth_state_t_t *oper_state);

/*!
 * \brief OAM_BFD_AUTH_SHA1 LT delete handler.
 *
 * This function handles the DELETE opcode for the SHA1 authentication LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] sha1_id Index of the SHA1 authentication entry to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_sha1_delete(int unit, uint32_t sha1_id);

/*!
 * \brief OAM_BFD_XXX_ENDPOINT LT insert handler.
 *
 * This function handles the INSERT opcode for all the OAM_BFD endpoint LTs.
 * This function will inform the app about the changes by sending a message to
 * it.
 *
 * \param [in] unit Unit number.
 * \param [in] ctrl OAM_BFD_CONTROL entry
 * \param [in] entry Structure representation of a BFD endpoint entry. The
 *                   fields not filled in by user are populated with the default
 *                   values.
 * \param [out] oper_state Operational state of the entry to be updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_insert(int unit,
                               bcmcth_oam_bfd_control_t *ctrl,
                               bcmcth_oam_bfd_endpoint_t *entry,
                               bcmltd_common_oam_bfd_endpoint_state_t_t *oper_state);

/*!
 * \brief OAM_BFD_XXX_ENDPOINT LT update handler.
 *
 * This function handles the UPDATE opcode for all the OAM_BFD endpoint LTs.
 * This function will inform the app about the changes by sending a message to
 * it.
 *
 * \param [in] unit Unit number.
 * \param [in] ctrl OAM_BFD_CONTROL entry
 * \param [in] cur Structure representing the view of the IMM entry after the
 *                 update operation has completed.
 * \param [in] prev Structure representing the current view of the IMM entry
 *                  with defaults populated for the unfilled values.
 * \param [out] oper_state Operational state of the entry to be updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_update(int unit,
                               bcmcth_oam_bfd_control_t *ctrl,
                               bcmcth_oam_bfd_endpoint_t *cur,
                               bcmcth_oam_bfd_endpoint_t *prev,
                               bcmltd_common_oam_bfd_endpoint_state_t_t *oper_state);

/*!
 * \brief OAM_BFD_XXX_ENDPOINT LT delete handler.
 *
 * This function handles the DELETE opcode for all the OAM_BFD endpoint LTs.
 * This function will inform the app about the changes by sending a message to
 * it.
 *
 * \param [in] unit Unit number.
 * \param [in] ep_type Type of the BFD endpoint.
 * \param [in] endpoint_id Endpoint ID of the endpoint.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_delete(int unit,
                               bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                               uint32_t endpoint_id);

/*!
 * \brief OAM_BFD_XXX_ENDPOINT_STATUS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for all the OAM_BFD endpoint status
 * LTs. This function will retreive the status from the app by sending a message
 * to it.
 *
 * \param [in] unit Unit number.
 * \param [in] ep_type Type of the BFD endpoint.
 * \param [in] endpoint_id Endpoint ID of the endpoint.
 * \param [out] entry Structure containing the current status of the endpoint.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_status_lookup(int unit,
                                      bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                                      uint32_t endpoint_id,
                                      bcmcth_oam_bfd_endpoint_status_t *entry);

/*!
 * \brief OAM_BFD_XXX_ENDPOINT_STATS LT update handler.
 *
 * This function handles the UPDATE opcode for all the OAM_BFD endpoint stats
 * LTs. This function will inform the app about the changes by sending a message
 * to it.
 *
 * \param [in] unit Unit number.
 * \param [in] ep_type Type of the BFD endpoint.
 * \param [in] endpoint_id Endpoint ID of the endpoint.
 * \param [in] entry Structure containing endpoint stats fields to be updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_stats_update(int unit,
                                     bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                                     uint32_t endpoint_id,
                                     bcmcth_oam_bfd_endpoint_stats_t *entry);

/*!
 * \brief OAM_BFD_XXX_ENDPOINT_STATS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for all the OAM_BFD endpoint stats
 * LTs. This function will retreive the statistics from the app by sending a
 * message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] ep_type Type of the BFD endpoint.
 * \param [in] endpoint_id Endpoint ID of the endpoint.
 * \param [out] entry Structure containing the retreived endpoint stats fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_stats_lookup(int unit,
                                     bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                                     uint32_t endpoint_id,
                                     bcmcth_oam_bfd_endpoint_stats_t *entry);

/*!
 * \brief OAM_BFD_EVENT_CONTROL LT update handler.
 *
 * This function handles the UPDATE opcode for OAM_BFD_EVENT_CONTROL table. This
 * function will inform the EApp about the modified event mask.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Structure containing the modified OAM_BFD_EVENT_CONTROL
 *              fields, the unmodified fields are filled with the values already
 *              in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_event_control_update(int unit,
                                    bcmcth_oam_bfd_event_control_t *entry);
/*!
 * \brief Report BFD events.
 *
 * This function updates the OAM_BFD_EVENT LT with the events received from the
 * EApp. The OAM_BFD_EVENT_STATUS is also updated with the last written index.
 *
 * \param [in] unit Unit number.
 * \param [in] ep_type Type of the BFD endpoint.
 * \param [in] endpoint_id Endpoint ID of the endpoint.
 * \param [in] num_events Number of events to be reported on the endpoint.
 * \param [in] events Events to be reported.
 *
 * \return None
 */
extern void
bcmcth_oam_bfd_event_report(int unit,
                            bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                            uint32_t endpoint_id,
                            int num_events,
                            bcmltd_common_oam_bfd_endpoint_event_t_t *events);

#endif /* BCMCTH_OAM_BFD_UC_H */
