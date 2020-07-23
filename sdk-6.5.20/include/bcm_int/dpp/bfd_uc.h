/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BFD UC Internal header
 */
#ifndef _BCM_INT_DPP_BFD_UC_H_
#define _BCM_INT_DPP_BFD_UC_H_

#include <bcm/bfd.h>
#include <bcm_int/dpp/oam.h>

/* BFD endpoint object. */
typedef struct bfd_endpoint_info_s {
    uint32 flags;                       /* Control flags. */
    bcm_bfd_endpoint_t id;              /* Endpoint identifier. */
    bcm_bfd_endpoint_t remote_id;       /* Remote endpoint identifier. */
    bcm_bfd_tunnel_type_t type;         /* Type of BFD encapsulation. */
    bcm_gport_t gport;                  /* Gport identifier. */
    bcm_gport_t tx_gport;               /* TX gport associated with this
                                           endpoint. */
    bcm_gport_t remote_gport;           /* Gport identifier of trapping
                                           destination: OAMP or CPU, local or
                                           remote. */
    int bfd_period;                     /* For local endpoints, this is the BFD
                                           transmission period in ms. */
    bcm_vpn_t vpn;                      /* VPN. */
    uint8 vlan_pri;                     /* VLAN tag priority. */
    uint8 inner_vlan_pri;               /* Inner VLAN tag priority. */
    bcm_vrf_t vrf_id;                   /* Vrf identifier. */
    bcm_mac_t dst_mac;                  /* Destination MAC. */
    bcm_mac_t src_mac;                  /* Source MAC. */
    bcm_vlan_t pkt_inner_vlan_id;       /* TX Packet inner Vlan Id. */
    bcm_ip_t dst_ip_addr;               /* Destination IPv4 address. */
    uint8 dst_ip6_addr[16];                 /* Destination IPv6 address. */
    bcm_ip_t src_ip_addr;               /* Source IPv4 address. */
    uint8 src_ip6_addr[16];             /* Source IPv6 address. */
    uint8 ip_tos;                       /* IPv4 Tos / IPv6 Traffic Class. */
    uint8 ip_ttl;                       /* IPv4 TTL / IPv6 Hop Count. */
    bcm_ip_t inner_dst_ip_addr;         /* Inner destination IPv4 address. */
    uint8 inner_dst_ip6_addr[16];       /* Inner destination IPv6 address. */
    bcm_ip_t inner_src_ip_addr;         /* Inner source IPv4 address. */
    uint8 inner_src_ip6_addr[16];       /* Inner source IPv6 address. */
    uint8 inner_ip_tos;                 /* Inner IPv4 Tos / IPv6 Traffic Class. */
    uint8 inner_ip_ttl;                 /* Inner IPv4 TTL / IPv6 Hop Count. */
    uint32 udp_src_port;                /* UDP source port for Ipv4, Ipv6. */
    bcm_mpls_label_t label;             /* Incoming inner label. */
    bcm_mpls_label_t mpls_hdr;          /* MPLS Header. */
    bcm_mpls_egress_label_t egress_label; /* The MPLS outgoing label information. */
    bcm_if_t egress_if;                 /* Egress interface. */
    uint8 mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CC/CV TLV and Source MEP ID. */
    uint8 mep_id_length;                /* Length of MPLS-TP CC/CV TLV and
                                           MEP-ID. */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing BFD to
                                           remote. */
    uint8 cpu_qid;                      /* CPU queue for BFD. */
    bcm_bfd_state_t local_state;        /* Local session state. */
    uint32 local_discr;                 /* Local discriminator. */
    bcm_bfd_diag_code_t local_diag;     /* Local diagnostic code. */
    uint32 local_flags;                 /* Flags combination on outgoing frames. */
    uint32 local_min_tx;                /* Desired local min tx interval. */
    uint32 local_min_rx;                /* Required local rx interval. */
    uint32 local_min_echo;              /* Local minimum echo interval. */
    uint8 local_detect_mult;            /* Local detection interval multiplier. */
    bcm_bfd_auth_type_t auth;           /* Authentication type. */
    uint32 auth_index;                  /* Authentication index. */
    uint32 tx_auth_seq;                 /* Tx authentication sequence id. */
    uint32 rx_auth_seq;                 /* Rx authentication sequence id. */
    uint32 remote_flags;                /* Remote flags. */
    bcm_bfd_state_t remote_state;       /* Remote session state. */
    uint32 remote_discr;                /* Remote discriminator. */
    bcm_bfd_diag_code_t remote_diag;    /* Remote diagnostic code. */
    uint32 remote_min_tx;               /* Desired remote min tx interval. */
    uint32 remote_min_rx;               /* Required remote rx interval. */
    uint32 remote_min_echo;             /* Remote minimum echo interval. */
    uint8 remote_detect_mult;           /* Remote detection interval multiplier. */
    int sampling_ratio;                 /* 0 - No packets sampled to the CPU.
                                           1-8 - Count of packets (with events)
                                           that need to arrive before one is
                                           sampled to the CPU. */
    uint8 loc_clear_threshold;          /* Number of packets required to reset
                                           the Loss-of-Continuity status per
                                           endpoint. */
    uint32 ip_subnet_length;            /* The subnet length for incoming packet
                                           validity check. Value 0 indicates no
                                           check is performed, positive values
                                           indicate the amount of MSBs to be
                                           compared. */
    uint8 remote_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CC/CV TLV and Remote MEP ID. */
    uint8 remote_mep_id_length;         /* Length of MPLS-TP CC/CV TLV and
                                           REMOTE MEP-ID. */
    uint8 mis_conn_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CC/CV TLV and Mis
                                           connectivity MEP ID. */
    uint8 mis_conn_mep_id_length;       /* Length of MPLS-TP CC/CV TLV and Mis
                                           connectivity MEP-ID. */
    uint32 bfd_detection_time;          /* Optional: BFD Detection Time, in
                                           microseconds. */
    bcm_vlan_t pkt_vlan_id;             /* TX Packet Vlan Id. */
    bcm_vlan_t rx_pkt_vlan_id;          /* RX Packet Vlan Id. */
    bcm_mpls_label_t gal_label;         /* MPLS GAL label. */
    uint32 faults;                      /* Fault flags. */
    uint32 flags2;                      /* Second set of control flags. */
    int ipv6_extra_data_index;          /* Pointer to first extended data entry */
    int punt_good_packet_period;        /* BFD good packets sampling period.
                                           Every punt_good_packet_period
                                           milliseconds, a single packet is
                                           punted to the CPU */
    uint8 bfd_period_cluster;           /* Tx period group. All MEPs in a group
                                           must be created with the same Tx
                                           period. Modifying the Tx period in
                                           one MEP affects the others. */
} bfd_endpoint_info_t;

/* BFD Endpoint Configuration SW Data Structure */
typedef struct bfd_endpoint_config_s {
    int endpoint_index;
    bcm_module_t  modid;    /* Destination module ID */
    bcm_port_t port;        /* Destination port */
    bcm_port_t tx_port;     /* Local physical port to TX BFD packet */
    int encap_type;         /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    int encap_length;       /* BFD Encapsulation length */
    int lkey_etype;         /* Lookup key Ether Type */
    int lkey_offset;        /* Lookup key offset */
    int lkey_length;        /* Lookup key length */
    bfd_endpoint_info_t info;
    int is_l3_vpn;          /* 1 for L3 VPN, 0 for L2 VPN */
    uint8 dip_index; /* Used by m-hop entry*/
    uint8 acc_mep_profile;/* Used by m-hop entry*/
    uint8 non_acc_profile;/* Used by m-hop entry */
    uint8 local_echo;     /*1 for BFD echo session*/
} bfd_endpoint_config_t;

/* BFD Event Handler */
typedef struct bfd_event_handler_s {
    PARSER_HINT_PTR struct bfd_event_handler_s *next;
    bcm_bfd_event_types_t event_types;
    bcm_bfd_event_cb cb;
    PARSER_HINT_PTR uint8 *user_data;
} bfd_event_handler_t;

typedef struct bfd_ipv6_no_wb_info_s {
    int initialized;            /* If set, BFD has been initialized */
    int rx_channel;             /* Local RX DMA channel for BHH packets */
    int uc_num;                 /* uController number running BHH appl */
    int endpoint_count;         /* Max number of BFD endpoints */
    int num_auth_sha1_keys;     /* Max number of sha1 auth keys */
    int num_auth_sp_keys;       /* Max number of simple pwd auth keys */
    bcm_cos_queue_t cpu_cosq;   /* CPU cos queue for RX BFD packets */
    bfd_event_handler_t *event_handler_list;
    int dma_buffer_len;         /* DMA max buffer size */
    PARSER_HINT_ARR uint8 *dma_buffer;          /* DMA buffer */
    sal_thread_t event_thread_id;            /* Event handler thread id */
    int event_thread_kill;                   /* Signal Event thread exit */
    uint32 event_mask;                       /* Mask of all events */
    uint32 trace_addr;            /* Address in the uC holding the stack trace */
    /* Protection Packet Feature */
    uint32 protection_packet_enable; /* BFD IPV6 Protection packet feature enable */
    uint32 protection_pkt_event_fifo_size; /* Indicates internal event FIFO threshold size */
    uint32 protection_pkt_event_fifo_timer; /* Indicates maximum wait timer to read internal event FIFO */
    int session_count;         /* Max number of BFD sessions */
    uint8 use_local_discr_as_session_id;    /* Use local discriminator as session ID feature enable */
} bfd_ipv6_no_wb_info_t;

typedef struct bcm_dpp_bfd_ipv6_info_s
{
    PARSER_HINT_ARR bfd_endpoint_config_t *endpoints;  /* Array to Endpoints */
    PARSER_HINT_ARR bcm_bfd_auth_sha1_t *auth_sha1; /* Array of sha1 auth keys */
    PARSER_HINT_ARR bcm_bfd_auth_simple_password_t *auth_sp; /* Array simple pwd auth keys */
    PARSER_HINT_PTR SHR_BITDCL *endpoints_in_use;   /* Indicates used endpoint indexes */
    PARSER_HINT_PTR SHR_BITDCL *sessions_in_use;   /* Indicates used session indexes */
    PARSER_HINT_ARR bcm_bfd_endpoint_t *endpoint_to_session_map;  /* Array from Endpoint IDs to session IDs map */
 } bcm_dpp_bfd_ipv6_info_t;


/*
 * Function:
 *      bcm_petra_bfd_uc_is_init
 * Purpose:
 *      Whether uKernel BFD has been inited.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_is_init(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_init
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_init(int unit);


/*
 * Function:
 *      bcm_petra_bfd_uc_deinit
 * Purpose:
 *      Send a BFD message to deinit the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_deinit(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_tx_start
 * Purpose:
 *      Start all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to start TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_tx_start(int unit);

/*
 * Function:
 *      _bcm_petra_bfd_uc_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to stop TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_tx_stop(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_create
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_endpoint_create(int unit,
                                        bcm_bfd_endpoint_info_t *endpoint_info);
/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_get
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
extern int 
bcm_petra_bfd_uc_endpoint_get(int unit,
                                    bcm_bfd_endpoint_t endpoint,
                                    bcm_bfd_endpoint_info_t *endpoint_info);


/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_destroy
 * Purpose:
 *      Destroy an BFD endpoint object.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to destroy.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to destroy endpoint which does not exist
 *      BCM_E_INTERNAL  Unable to release resource /
 *                      Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_destroy_all
 * Purpose:
 *      Destroy all BFD endpoint objects.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_endpoint_destroy_all(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND BFD Session ID not in use
 *      BCM_E_INTERNAL  Unable to read or write resource
 *      BCM_E_PARAM     Session not in Demand Mode/
 *                      Session not in UP state
 * Notes: The poll operation will be initiated.  Poll failure will be
 * signalled via asynchronous callback (aka session failure)
 */
extern int
bcm_petra_bfd_uc_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint);


/*
 * Function:
 *      bcm_petra_bfd_uc_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
extern int
bcm_petra_bfd_uc_event_register(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data);

/*
 * Function:
 *      bcm_petra_bfd_uc_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
extern int
bcm_petra_bfd_uc_event_unregister(int unit,
                               bcm_bfd_event_types_t event_types,
                               bcm_bfd_event_cb cb);

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to counter information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_endpoint_stat_get(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint8 clear);
/*
 * Function:
 *      bcm_petra_bfd_uc_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_sha1_set(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1);

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_sha1_get(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1);

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_simple_password_set(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp);

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
extern int
bcm_petra_bfd_uc_auth_simple_password_get(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp);

/*
 * Function:
 *      bcm_petra_bfd_uc_dump_trace
 * Purpose:
 *      Dump the BFD stack trace
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      None
 * Notes:
 */
void
_bcm_petra_bfd_uc_dump_trace(int unit);

/*
 * Function:
 *      bcm_petra_bfd_uc_discard_stat_get
 * Purpose:
 *      Get global discarded packet statistics for BFD IPv6 FW.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to discarded structure.
 *      BCM_E_INTERNAL Unable to reach FIRMWARE
 * Notes:
 */
int
bcm_petra_bfd_uc_discard_stat_get(int unit,
                             bcm_bfd_discard_stat_t *discarded_info);

/*
 * Function:
 *      bcm_petra_bfd_uc_discard_stat_set
 * Purpose:
 *      Reset global discarded packet statistics for BFD IPv6 FW.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info  - (IN) Pointer to SHA1 info structure to set the data. Ignored.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_INTERNAL Unable to reach FIRMWARE
 * Notes:
 */
int
bcm_petra_bfd_uc_discard_stat_set(int unit,
        bcm_bfd_discard_stat_t *discarded_info);

#endif /*_BCM_INT_DPP_BFD_UC_H_*/

