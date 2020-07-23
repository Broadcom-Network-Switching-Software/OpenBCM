/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_IFA_H__
#define __BCM_IFA_H__

#include <bcm/types.h>
#include <bcm/collector.h>

/* IFA configuration information data. */
typedef struct bcm_ifa_config_info_s {
    uint32 probemarker_1;               /* INT header ProbeMarker1 Value. */
    uint32 probemarker_2;               /* INT header ProbeMarker2 Value. */
    uint32 device_id;                   /* Meta Data header Device ID. */
    uint16 max_payload_length;          /* INT header Maximum payload. */
    uint16 module_id;                   /* Meta Data header Module Id used to
                                           identify loopback source port
                                           information. */
    uint8 hop_limit;                    /* INT header Hop Limit condition. */
    uint16 rx_packet_payload_length;    /* Recieved packet payload length that
                                           can be sent to collector. */
    uint16 lb_port_1;                   /* Loopback port for IFA E_APP. */
    uint16 lb_port_2;                   /* Loopback port for IFA E_APP. */
    uint8 optional_headers;             /* Option to send with recieved packet
                                           headers. */
    uint8 true_hop_count;               /* Number of hops in the topology is
                                           greater than Hop limit,  IFA MD stack
                                           contains Hop limit number of MDs in
                                           MD stack. */
    uint8 template_id;                  /* Template id for IFA probe header */
    uint16 senders_handle;              /* Sender's Handle for Probe Header */
    uint16 rx_packet_export_max_length; /* Maximum length of the Rx pkt that
                                           needs to be exported to collector. */
} bcm_ifa_config_info_t;

/* IFA statistics information data. */
typedef struct bcm_ifa_stat_info_s {
    uint32 rx_pkt_cnt;                  /* Number of packet recieved in FW. */
    uint32 tx_pkt_cnt;                  /* Number of packet transmitted from FW. */
    uint32 ifa_no_config_drop;          /* Number of pkt dropped due to missing
                                           configuration. */
    uint32 ifa_collector_not_present_drop; /* Number of pkt dropped due to missing
                                           collector configuration. */
    uint32 ifa_hop_cnt_invalid_drop;    /* Number of pkt dropped due to Hop
                                           count and Hop limit are out of order. */
    uint32 ifa_int_hdr_len_invalid_drop; /* Number of pkt dropped due to maximum
                                           length and current length are out of
                                           order. */
    uint32 ifa_pkt_size_invalid_drop;   /* Number of pkt dropped due to invalid
                                           packet size. */
    uint32 rx_pkt_length_exceed_max_drop_count; /* Number of Rx packets dropped due to
                                           its length exceeding the max length
                                           configured using the config property
                                           ifa_rx_pkt_max_length. */
    uint32 rx_pkt_parse_error_drop_count; /* Number of Rx packets dropped due to
                                           errors in parsing. */
    uint32 rx_pkt_unknown_namespace_drop_count; /* Number of Rx packets dropped as the
                                           namespace is unknown. */
    uint32 rx_pkt_excess_rate_drop_count; /* Number of Rx packets dropped as the
                                           incoming rate is too high to process. */
    uint32 tx_record_count;             /* Number of IFA records exported to
                                           collector. */
    uint32 tx_pkt_failure_count;        /* Number of Tx packets that could not
                                           be exported due to some failure. */
    uint32 ifa_template_not_present_drop; /* Number of packets dropped due to
                                           missing Template configuration. */
    uint32 ifa_incomplete_metadata_drop_count; /* Number of Rx packets dropped due to
                                           incomplete metadata stack. */
} bcm_ifa_stat_info_t;

/* 
 * Macros for bcm_ifa_collector_eth_header_t.vlan_tag_structure to
 * indicate the tag structure of vlans.
 */
#define BCM_IFA_COLLECTOR_ETH_HDR_UNTAGGED  0x1        
#define BCM_IFA_COLLECTOR_ETH_HDR_SINGLE_TAGGED 0x2        
#define BCM_IFA_COLLECTOR_ETH_HDR_DOUBLE_TAGGED 0x3        

/* 
 * Ethernet header in the encap used to send the IFA packet to the
 * collector.
 */
typedef struct bcm_ifa_collector_eth_header_s {
    bcm_mac_t dst_mac; 
    bcm_mac_t src_mac; 
    uint8 vlan_tag_structure; 
    uint16 outer_tpid; 
    uint16 inner_tpid; 
    bcm_vlan_tag_t outer_vlan_tag; 
    bcm_vlan_tag_t inner_vlan_tag; 
} bcm_ifa_collector_eth_header_t;

/* IPv4 header in the encap used to send the IFA packet to the collector. */
typedef struct bcm_ifa_collector_ipv4_header_s {
    bcm_ip_t src_ip; 
    bcm_ip_t dst_ip; 
    uint8 dscp; 
    uint8 ttl; 
} bcm_ifa_collector_ipv4_header_t;

/* IPv6 header in the encap used to send the IFA packet to the collector. */
typedef struct bcm_ifa_collector_ipv6_header_s {
    bcm_ip6_t src_ip; 
    bcm_ip6_t dst_ip; 
    uint8 traffic_class; 
    uint32 flow_label; 
    uint8 hop_limit; 
} bcm_ifa_collector_ipv6_header_t;

/* UDP header in the encap used to send the IFA packet to the collector. */
typedef struct bcm_ifa_collector_udp_header_s {
    bcm_l4_port_t src_port; 
    bcm_l4_port_t dst_port; 
} bcm_ifa_collector_udp_header_t;

/* Transport types that are supported for the collector */
typedef enum bcm_ifa_collector_transport_type_e {
    bcmIfaCollectorTransportTypeIpv4Udp = 0, /* The encap will be of the format UDP
                                           over IPv4. */
    bcmIfaCollectorTransportTypeIpv6Udp = 1 /* The encap will be of the format UDP
                                           over IPv6. */
} bcm_ifa_collector_transport_type_t;

/* In-band flow analyzer collector information. */
typedef struct bcm_ifa_collector_info_s {
    bcm_ifa_collector_transport_type_t transport_type; /* Transport type used for sending data
                                           to the collector. */
    bcm_ifa_collector_eth_header_t eth; /* Ethernet encapsulation of the packet
                                           sent to collector. */
    bcm_ifa_collector_ipv4_header_t ipv4; /* IPv4 encapsulation of the packet sent
                                           to collector. */
    bcm_ifa_collector_ipv6_header_t ipv6; /* IPv6 encapsulation of the packet sent
                                           to collector. */
    bcm_ifa_collector_udp_header_t udp; /* UDP encapsulation of the packet sent
                                           to collector. */
    uint16 mtu;                         /* The maximum packet length transmitted
                                           by collector. */
} bcm_ifa_collector_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Initializes ifa subsystem. */
extern int bcm_ifa_init(
    int unit);

/* Detaches ifa subsystem. */
extern int bcm_ifa_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Options that can be passed via options parameter during configuration
 * set.
 */
#define BCM_IFA_CONFIG_ADD      (1 << 0)   /* Add configuration to transfer ifa
                                              information. */
#define BCM_IFA_CONFIG_MODIFY   (1 << 1)   /* Update a configuration
                                              information. */
#define BCM_IFA_CONFIG_CLEAR    (1 << 2)   /* Clear configuration information. */

/* Options that can be passed via options parameter during collector set. */
#define BCM_IFA_COLLECTOR_ADD       (1 << 0)   /* Add collector to transfer ifa
                                                  information. */
#define BCM_IFA_COLLECTOR_MODIFY    (1 << 1)   /* Update a collector
                                                  information. */
#define BCM_IFA_COLLECTOR_DETACH    (1 << 2)   /* Detach collector. */

/* Software ID given for a IFA export template configuration. */
typedef int bcm_ifa_export_template_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a In-band Flow Analyzer collector. */
extern int bcm_ifa_collector_set(
    int unit, 
    uint32 options, 
    bcm_ifa_collector_info_t *collector_info);

/* Get a In-band Flow Analyzer collector. */
extern int bcm_ifa_collector_get(
    int unit, 
    bcm_ifa_collector_info_t *collector_info);

/* Attach an In-band Flow Analyzer collector. */
extern int bcm_ifa_collector_attach(
    int unit, 
    bcm_collector_t collector_id, 
    int export_profile_id, 
    bcm_ifa_export_template_t template_id);

/* Get the attached In-band Flow Analyzer collector and export profile. */
extern int bcm_ifa_collector_attach_get(
    int unit, 
    bcm_collector_t *collector_id, 
    int *export_profile_id, 
    bcm_ifa_export_template_t *template_id);

/* Detach In-band Flow Analyzer collector. */
extern int bcm_ifa_collector_detach(
    int unit, 
    bcm_collector_t collector_id, 
    int export_profile_id, 
    bcm_ifa_export_template_t template_id);

/* Set a In-band Flow Analyzer configuration information. */
extern int bcm_ifa_config_info_set(
    int unit, 
    uint32 options, 
    bcm_ifa_config_info_t *config_data);

/* Get a In-band Flow Analyzer Configuration information. */
extern int bcm_ifa_config_info_get(
    int unit, 
    bcm_ifa_config_info_t *config_data);

/* In-band flow analyzer - IFA statistics information */
extern int bcm_ifa_stat_info_get(
    int unit, 
    bcm_ifa_stat_info_t *stat_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a config information structure. */
extern void bcm_ifa_config_info_t_init(
    bcm_ifa_config_info_t *config_info);

/* Initialize a collector information structure. */
extern void bcm_ifa_collector_info_t_init(
    bcm_ifa_collector_info_t *collector_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* In-band flow analyzer - IFA statistics information */
extern int bcm_ifa_stat_info_set(
    int unit, 
    bcm_ifa_stat_info_t *stat_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Maximum number of queues supported by IFA-CC Eapp. */
#define BCM_IFA_CC_MAX_NUM_QUEUE    0x200      

/* 
 * Macro for bcm_ifa_cc_config_info _t flag to indicate usage of
 * QUEUE_ID, INT_PRI and CLASS_ID.
 */
#define BCM_IFA_CC_QUEUE_ID_INT_PRI         0x1        
#define BCM_IFA_CC_QUEUE_ID_INT_PRI_CLASS_ID 0x2        

/* IFA-CC Configuration information. */
typedef struct bcm_ifa_cc_config_s {
    uint8 flag;                         /* Indicate usage of queue_id, int_pri
                                           and class_id. */
    uint16 num_gports;                  /* Number of gports to be monitor by
                                           IFA-CC App. */
    uint8 class_id[BCM_IFA_CC_MAX_NUM_QUEUE]; /* Array of Class_ID assicated with
                                           Queue ID. */
    uint16 int_pri[BCM_IFA_CC_MAX_NUM_QUEUE]; /* Array of INT_PRI assicated with Queue
                                           ID. */
    bcm_gport_t gport_list[BCM_IFA_CC_MAX_NUM_QUEUE]; /* Array of gports that are being
                                           monitored by IFA-CC App. */
} bcm_ifa_cc_config_t;

/* Initialize ifa cc config structure. */
extern void bcm_ifa_cc_config_t_init(
    bcm_ifa_cc_config_t *config_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set IFA-CC Configuration. */
extern int bcm_ifa_cc_config_set(
    int unit, 
    bcm_ifa_cc_config_t *config_data);

/* Get IFA-CC Configuration. */
extern int bcm_ifa_cc_config_get(
    int unit, 
    bcm_ifa_cc_config_t *config_data);

#endif /* BCM_HIDE_DISPATCHABLE */

/* IFA export element flags. */
#define BCM_IFA_EXPORT_ELEMENT_FLAGS_ENTERPRISE (1 << 0)   /* Indicates that the
                                                          element is an
                                                          enterprise specific
                                                          element */

/* 
 * Options that can be passed via options parameter during export
 * template creation.
 */
#define BCM_IFA_EXPORT_TEMPLATE_WITH_ID (1 << 0)   /* Create an export template
                                                      with ID. */

/* The different export element types that constitute an template. */
typedef enum bcm_ifa_export_element_type_e {
    bcmIfaExportElementTypeIFAHeaders = 0, /* IFA Base and MetaData headers */
    bcmIfaExportElementTypeMetadataStack = 1, /* IFA MetaData stack */
    bcmIfaExportElementTypePktData = 2, /* IFA RX Packet Headers and Data */
    bcmIfaExportElementTypeCount = 3 
} bcm_ifa_export_element_type_t;

/* IFA export template elements information. */
typedef struct bcm_ifa_export_element_info_s {
    uint32 flags;                       /* See BCM_IFA_EXPORT_ELEMENT_FLAGS_XXX. */
    bcm_ifa_export_element_type_t element; /* Type of export element. */
    uint32 data_size;                   /* Size of information element in the
                                           export record in bytes. If the
                                           data_size is given as 0, then the
                                           default RFC size is used. */
    uint16 info_elem_id;                /* Information element to be used, when
                                           the template set is exported. */
} bcm_ifa_export_element_info_t;

/* IFA template set transmit configuration. */
typedef struct bcm_ifa_template_transmit_config_s {
    int retransmit_interval_secs;   /* Interval in seconds at which the template
                                       set has to be transmitted. */
    int initial_burst;              /* Number of times the template set needs to
                                       be sent before settling to a periodic
                                       export. */
} bcm_ifa_template_transmit_config_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a ifa export template. */
extern int bcm_ifa_export_template_create(
    int unit, 
    uint32 options, 
    bcm_ifa_export_template_t *id, 
    uint16 set_id, 
    int num_export_elements, 
    bcm_ifa_export_element_info_t *list_of_export_elements);

/* Get a ifa export template. */
extern int bcm_ifa_export_template_get(
    int unit, 
    bcm_ifa_export_template_t id, 
    uint16 *set_id, 
    int max_size, 
    bcm_ifa_export_element_info_t *list_of_export_elements, 
    int *list_size);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize ifa template transmit configuration. */
extern void bcm_ifa_template_transmit_config_t_init(
    bcm_ifa_template_transmit_config_t *config);

#ifndef BCM_HIDE_DISPATCHABLE

/* Destroy a ifa export template. */
extern int bcm_ifa_export_template_destroy(
    int unit, 
    bcm_ifa_export_template_t id);

/* Set the template transmit configuration. */
extern int bcm_ifa_template_transmit_config_set(
    int unit, 
    bcm_ifa_export_template_t template_id, 
    bcm_collector_t collector_id, 
    bcm_ifa_template_transmit_config_t *config);

/* Set the template transmit configuration. */
extern int bcm_ifa_template_transmit_config_get(
    int unit, 
    bcm_ifa_export_template_t template_id, 
    bcm_collector_t collector_id, 
    bcm_ifa_template_transmit_config_t *config);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Options that can be passed via options parameter during IFA header
 * creation.
 */
#define BCM_IFA_HEADER_OPTIONS_WITH_ID  (1 << 0)   /* Create the IFA header with
                                                      ID. */
#define BCM_IFA_HEADER_OPTIONS_REPLACE  (1 << 1)   /* Replace the IFA header. */

/* IFA header data. */
typedef struct bcm_ifa_header_s {
    uint8 ip_protocol;  /* IP protocol to indicate IFA. */
    uint8 max_length;   /* The max length in the IFA base header. */
    uint8 hop_limit;    /* The hop limit in the IFA metadata header. */
} bcm_ifa_header_t;

/* IFA header data callback function prototype. */
typedef int(*bcm_ifa_header_traverse_cb)(
    int unit, 
    int *header_id, 
    bcm_ifa_header_t *header, 
    void *user_data);

/* Initialize the bcm_ifa_header_t struct. */
extern void bcm_ifa_header_t_init(
    bcm_ifa_header_t *header);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the IFA header. */
extern int bcm_ifa_header_create(
    int unit, 
    uint32 options, 
    int *header_id, 
    bcm_ifa_header_t *header);

/* Get the IFA header. */
extern int bcm_ifa_header_get(
    int unit, 
    int header_id, 
    bcm_ifa_header_t *header);

/* Destroy the IFA header. */
extern int bcm_ifa_header_destroy(
    int unit, 
    int header_id);

/* Traverse all IFA headers. */
extern int bcm_ifa_header_traverse(
    int unit, 
    bcm_ifa_header_traverse_cb cb, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_IFA_H__ */
