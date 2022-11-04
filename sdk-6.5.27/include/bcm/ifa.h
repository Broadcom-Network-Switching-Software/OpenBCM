/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_IFA_H__
#define __BCM_IFA_H__

#include <bcm/types.h>
#include <bcm/collector.h>
#include <bcm/time.h>

/* Maximum number of INT_PRI. */
#define BCM_IFA_MAX_NUM_INT_PRI 16         

/* Maximum number of ports. */
#define BCM_IFA_MAX_NUM_PORTS   512        

/* Int_pri to cosq mapping for the ports. */
typedef struct bcm_ifa_int_pri_cosq_map_s {
    uint8 cosq[BCM_IFA_MAX_NUM_INT_PRI]; /* Cosq value. */
} bcm_ifa_int_pri_cosq_map_t;

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
    uint32 vxlan_flow_aging_interval;   /* Interval in milliseconds at which
                                           VXLAN entries are aged out. */
    bcm_ifa_int_pri_cosq_map_t cosq_map[BCM_IFA_MAX_NUM_PORTS]; /* Int_pri to COSQ mapping. */
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
    uint32 vxlan_term_lookup_miss_cnt;  /* Number of VXLAN terminated IFA
                                           packets which had lookup miss. */
    uint32 vxlan_data_flow_table_insert_fail_cnt; /* Number of VXLAN data packets with
                                           flow table insertion failures. */
} bcm_ifa_stat_info_t;

/* 
 * Match rule value used by IFA2 Embedded App to identify VXLAN
 * terminated data packet.
 */
#define BCM_FIELD_IFA_MATCHED_RULE_VXLAN_DATA_PACKET 1          

/* 
 * Match rule value used by IFA2 Embedded App to identify VXLAN
 * terminated IFA packet.
 */
#define BCM_FIELD_IFA_MATCHED_RULE_VXLAN_IFA_PACKET 2          

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
    bcm_gport_t mirror_dest_id;         /* Mirror destination id. */
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

/* Flag to indicate IFA CC configured for IFA2.0. */
#define BCM_IFA_CC_IFA2         0x4        

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
    uint8 gns;          /* Global namespace. */
    uint8 flags;        /* IFA flags. */
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

/* IFA controls. */
typedef enum bcm_ifa_control_e {
    bcmIfaControlIpProtocol = 0,        /* IP protocol used for IFA packets. */
    bcmIfaControlDeviceId = 1,          /* Device ID to be added to IFA
                                           metadata. */
    bcmIfaControlInitActionVector = 2,  /* Action vector to be added in the
                                           metadata header when IFA is
                                           initiated. */
    bcmIfaControlInitRequestVector = 3, /* Request vector to be added in the
                                           metadata header when IFA is
                                           initiated. */
    bcmIfaControlCosqStatsReportMode = 4, /* CosQ stats report mode. */
    bcmIfaControlCosqStatsInvalidValue = 5, /* Value to be populated in the metadata
                                           when CosQ stats are included in the
                                           metadata but the value could not be
                                           determined by the device. */
    bcmIfaControlPerItmCosqStats = 6,   /* Enable per-ITM CosQ stats. */
    bcmIfaControlOpaqueDataProfileLookupMode = 7, /* Key to lookup the opaque data
                                           profile. */
    bcmIfaControlOpaqueDataProfileView = 8, /* View to use to lookup the opaque data
                                           profile. */
    bcmIfaControlCosqStatsDynamicMaxBytesEnable = 9, /* Enable reporting of dynamic max bytes
                                           in the metadata. Require BST to be
                                           enabled. Not applicable for
                                           cut-through packets. */
    bcmIfaControlCount = 10             /* Last value, not usable. */
} bcm_ifa_control_t;

#define BCM_IFA_CONTROL_STRINGS \
{ \
    "IpProtocol", \
    "DeviceId", \
    "InitActionVector", \
    "InitRequestVector", \
    "CosqStatsReportMode", \
    "CosqStatsInvalidValue", \
    "PerItmCosqStats", \
    "OpaqueDataProfileLookupMode", \
    "OpaqueDataProfileView", \
    "CosqStatsDynamicMaxBytesEnable", \
    "Count"  \
}

/* CoSQ stats report mode. */
typedef enum bcm_ifa_control_cosq_stats_report_mode_e {
    bcmIfaControlCosqStatsReportModePort = 0, /* Report port based MMU stats. */
    bcmIfaControlCosqStatsReportModeQueue = 1, /* Report queue based MMU stats. */
    bcmIfaControlCosqStatsReportModeCount = 2 /* Last value, not usable. */
} bcm_ifa_control_cosq_stats_report_mode_t;

#define BCM_IFA_COSQ_STATS_REPORT_MODE_STRINGS \
{ \
    "Port", \
    "Queue", \
    "Count"  \
}

/* Opaque data profile lookup mode. */
typedef enum bcm_ifa_control_opaque_data_profile_lookup_mode_e {
    bcmIfaControlOpaqueDataProfileLookupModeDirectIndex = 0, /* Index provided by IFP. */
    bcmIfaControlOpaqueDataProfileLookupModeEgressPort = 1, /* Index based on egress port. */
    bcmIfaControlOpaqueDataProfileLookupModeEgressPortQueue = 2, /* Indexed based on combination of
                                           egress port and queue. */
    bcmIfaControlOpaqueDataProfileLookupModeCount = 3 /* Last value, not usable. */
} bcm_ifa_control_opaque_data_profile_lookup_mode_t;

#define BCM_IFA_OPAQUE_DATA_PROFILE_LOOKUP_MODE_STRINGS \
{ \
    "DirectIndex", \
    "EgressPort", \
    "EgressPortQueue", \
    "Count"  \
}

/* Opaque data profile view. */
typedef enum bcm_ifa_control_opaque_data_profile_view_e {
    bcmIfaControlOpaqueDataProfileViewSingle = 0, /* Single wide view. */
    bcmIfaControlOpaqueDataProfileViewDouble = 1, /* Double wide view. */
    bcmIfaControlOpaqueDataProfileViewCount = 2 /* Last value, not usable. */
} bcm_ifa_control_opaque_data_profile_view_t;

#define BCM_IFA_OPAQUE_DATA_PROFILE_VIEW_STRINGS \
{ \
    "Single", \
    "Double", \
    "Count"  \
}

/* IFA control information. */
typedef struct bcm_ifa_control_info_s {
    uint8 ip_protocol;                  /* IFA IP protocol number. Applicable
                                           when type is bcmIfaControlIpProtocol. */
    uint32 device_id;                   /* Device ID. Applicable when type is
                                           bcmIfaControlDeviceId. */
    uint8 init_action_vector;           /* Action vector used when initiating
                                           IFA. Applicable when type is
                                           bcmIfaControlInitActionVector. */
    uint8 init_request_vector;          /* Request vector used when initiating
                                           IFA. Applicable when type is
                                           bcmIfaControlInitRequestVector. */
    bcm_ifa_control_cosq_stats_report_mode_t cosq_stats_report_mode; /* CoSQ stats report mode. Applicable
                                           when type is
                                           bcmIfaControlCosqStatsReportMode. */
    uint32 cosq_stats_invalid_value;    /* CosQ stats invalid value. Applicable
                                           when type is
                                           bcmIfaControlCosqStatsInvalidValue. */
    int per_itm_cosq_stats_enable;      /* Set to 1 to enable per ITM CosQ
                                           stats. Applicable when type is
                                           bcmIfaControlPerItmCosqStats. */
    bcm_ifa_control_opaque_data_profile_lookup_mode_t opaque_data_profile_lookup_mode; /* Opaque data profile lookup mode.
                                           Applicable when type is
                                           bcmIfaControlOpaqueDataProfileLookupMode. */
    bcm_ifa_control_opaque_data_profile_view_t opaque_data_profile_view; /* Opaque data profile view. Applicable
                                           when type is
                                           bcmIfaControlOpaqueDataProfileView. */
    int dynamic_max_bytes_enable;       /* Set to 1 to enable reporting of
                                           dynamic max bytes in the metadata.
                                           Applicable when type is
                                           bcmIfaControlCosqStatsDynamicMaxBytesEnable. */
} bcm_ifa_control_info_t;

/* Initialize ifa control information structure. */
extern void bcm_ifa_control_info_t_init(
    bcm_ifa_control_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set an IFA global control. */
extern int bcm_ifa_control_set(
    int unit, 
    bcm_ifa_control_t type, 
    bcm_ifa_control_info_t *info);

/* Get an IFA global control. */
extern int bcm_ifa_control_get(
    int unit, 
    bcm_ifa_control_t type, 
    bcm_ifa_control_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Options that can be passed during metadata profile creation. */
#define BCM_IFA_METADATA_PROFILE_CREATE_OPTIONS_WITH_ID (1 << 0)   /* Create a metadata
                                                          profile with specific
                                                          ID. */

/* IFA metadata fields. */
typedef enum bcm_ifa_metadata_field_e {
    bcmIfaMetadataFieldPadOne = 0,      /* Pad ones. */
    bcmIfaMetadataFieldPadZero = 1,     /* Pad zeros. */
    bcmIfaMetadataFieldOpaqueData0 = 2, /* Opaque data 0. */
    bcmIfaMetadataFieldOpaqueData1 = 3, /* Opaque data 1. */
    bcmIfaMetadataFieldOpaqueData2 = 4, /* Opaque data 2. */
    bcmIfaMetadataFieldOpaqueData3 = 5, /* Opaque data 3. */
    bcmIfaMetadataFieldOpaqueData4 = 6, /* Opaque data 4. */
    bcmIfaMetadataFieldOpaqueData5 = 7, /* Opaque data 5. */
    bcmIfaMetadataFieldOpaqueData6 = 8, /* Opaque data 6. */
    bcmIfaMetadataFieldOpaqueData7 = 9, /* Opaque data 7. */
    bcmIfaMetadataFieldIfaLns = 10,     /* Local namespace. */
    bcmIfaMetadataFieldDeviceId = 11,   /* Device ID. */
    bcmIfaMetadataFieldResidenceTimeSubSeconds = 12, /* Sub-seconds portion of residence
                                           time. */
    bcmIfaMetadataFieldResidenceTimeSeconds = 13, /* Seconds portion of residence time. */
    bcmIfaMetadataFieldCosqStat0 = 14,  /* Stat 0 generated by traffic manager
                                           (TM). */
    bcmIfaMetadataFieldCosqStat1 = 15,  /* Stat 1 generated by traffic manager
                                           (TM). */
    bcmIfaMetadataFieldIngressTimestampSubSeconds = 16, /* Sub-seconds portion of ingress
                                           timestamp. */
    bcmIfaMetadataFieldIngressTimestampSecondsLower = 17, /* Lower seconds portion of ingress
                                           timestamp. */
    bcmIfaMetadataFieldIngressTimestampSecondsUpper = 18, /* Higher seconds portion of ingress
                                           timestamp. */
    bcmIfaMetadataFieldEgressTimestampSubSeconds = 19, /* Sub-seconds portion of egress
                                           timestamp. */
    bcmIfaMetadataFieldEgressTimestampSecondsLower = 20, /* Lower seconds portion of egress
                                           timestamp. */
    bcmIfaMetadataFieldEgressTimestampSecondsUpper = 21, /* Higher seconds portion of egress
                                           timestamp. */
    bcmIfaMetadataFieldOverlayL3EgrIntf = 22, /* Overlay L3 egress interface. */
    bcmIfaMetadataFieldL3EgrIntf = 23,  /* L3 egress interface. */
    bcmIfaMetadataFieldL3IngIntf = 24,  /* L3 ingress interface. */
    bcmIfaMetadataFieldIngPortId = 25,  /* Ingress port ID. */
    bcmIfaMetadataFieldEgrPortId = 26,  /* Egress port ID. */
    bcmIfaMetadataFieldVrf = 27,        /* VRF. */
    bcmIfaMetadataFieldVfi = 28,        /* VFI. */
    bcmIfaMetadataFieldDvp = 29,        /* DVP. */
    bcmIfaMetadataFieldSvp = 30,        /* SVP. */
    bcmIfaMetadataFieldTunnelEncapIndex = 31, /* Tunnel encapsulation index. */
    bcmIfaMetadataFieldCng = 32,        /* Internal color/drop precedence. */
    bcmIfaMetadataFieldCongestion = 33, /* Congestion marked status. */
    bcmIfaMetadataFieldQueueId = 34,    /* Destination queue ID. */
    bcmIfaMetadataFieldProfileId = 35,  /* Metadata profile ID. */
    bcmIfaMetadataFieldCutThrough = 36, /* Cut through status. */
    bcmIfaMetadataFieldIpTtl = 37,      /* IP TTL. */
    bcmIfaMetadataFieldEgressPortQueueCountUpper = 38, /* Higher portion of port or queue
                                           counts. */
    bcmIfaMetadataFieldEgressPortQueueCountLower = 39, /* Lower portion of port or queue
                                           counts. */
    bcmIfaMetadataFieldCount = 40       /* Always last. Not a usable value. */
} bcm_ifa_metadata_field_t;

#define BCM_IFA_METADATA_FIELD_STRINGS \
{ \
    "PadOne", \
    "PadZero", \
    "OpaqueData0", \
    "OpaqueData1", \
    "OpaqueData2", \
    "OpaqueData3", \
    "OpaqueData4", \
    "OpaqueData5", \
    "OpaqueData6", \
    "OpaqueData7", \
    "IfaLns", \
    "DeviceId", \
    "ResidenceTimeSubSeconds", \
    "ResidenceTimeSeconds", \
    "CosqStat0", \
    "CosqStat1", \
    "IngressTimestampSubSeconds", \
    "IngressTimestampSecondsLower", \
    "IngressTimestampSecondsUpper", \
    "EgressTimestampSubSeconds", \
    "EgressTimestampSecondsLower", \
    "EgressTimestampSecondsUpper", \
    "OverlayL3EgrIntf", \
    "L3EgrIntf", \
    "L3IngIntf", \
    "IngPortId", \
    "EgrPortId", \
    "Vrf", \
    "Vfi", \
    "Dvp", \
    "Svp", \
    "TunnelEncapIndex", \
    "Cng", \
    "Congestion", \
    "QueueId", \
    "ProfileId", \
    "CutThrough", \
    "IpTtl", \
    "EgressPortQueueCountUpper", \
    "EgressPortQueueCountLower", \
    "Count"  \
}

/* Metadata field information. */
typedef struct bcm_ifa_metadata_field_info_s {
    int size;   /* Size of the metadata field in bits. */
} bcm_ifa_metadata_field_info_t;

/* Metadata field entry. */
typedef struct bcm_ifa_metadata_field_entry_s {
    bcm_ifa_metadata_field_t field; /* Metadata field. */
    int start_offset;               /* Bit position from the LSB of the field
                                       value which should be inserted into the
                                       metadata. */
    int bit_width;                  /* Number of bits of the field value that is
                                       inserted in the metadata. Set to 0 to
                                       insert the full value in the metadata. */
    uint32 value;                   /* Value of the metadata field. Only
                                       applicable to those fields whose value
                                       can be configured. */
} bcm_ifa_metadata_field_entry_t;

/* Initialize metadata field information structure. */
extern void bcm_ifa_metadata_field_info_t_init(
    bcm_ifa_metadata_field_info_t *field_info);

/* Initialize metadata field entry structure. */
extern void bcm_ifa_metadata_field_entry_t_init(
    bcm_ifa_metadata_field_entry_t *field_entry);

#ifndef BCM_HIDE_DISPATCHABLE

/* Get metadata field info. */
extern int bcm_ifa_metadata_field_info_get(
    int unit, 
    bcm_ifa_metadata_field_t field, 
    bcm_ifa_metadata_field_info_t *field_info);

/* Create a metadata profile. */
extern int bcm_ifa_metadata_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id);

/* Destroy a metadata profile. */
extern int bcm_ifa_metadata_profile_destroy(
    int unit, 
    int profile_id);

/* Get all metadata profiles. */
extern int bcm_ifa_metadata_profile_get_all(
    int unit, 
    int max_size, 
    int *profile_id, 
    int *count);

/* Set the fields in the metadata. */
extern int bcm_ifa_metadata_profile_fields_set(
    int unit, 
    int profile_id, 
    int num_fields, 
    bcm_ifa_metadata_field_entry_t *field_entries);

/* Get the fields in the metadata. */
extern int bcm_ifa_metadata_profile_fields_get(
    int unit, 
    int profile_id, 
    int max_fields, 
    bcm_ifa_metadata_field_entry_t *field_entries, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_IFA_MAX_OPAQUE_DATA 8          /* Maximum number of opaque datas in
                                              a profile. */

/* Options that can be passed during opaque data profile creation. */
#define BCM_IFA_OPAQUE_DATA_PROFILE_CREATE_OPTIONS_WITH_ID (1 << 0)   /* Create an opaque
                                                          profile with specific
                                                          ID. */
#define BCM_IFA_OPAQUE_DATA_PROFILE_CREATE_OPTIONS_REPLACE (1 << 1)   /* Update an existing
                                                          opaque data profile. */

/* Opaque data profile. */
typedef struct bcm_ifa_opaque_data_profile_s {
    uint32 opaque_data[BCM_IFA_MAX_OPAQUE_DATA]; /* Opaque data field. */
} bcm_ifa_opaque_data_profile_t;

/* Opaque data profile lookup key. */
typedef struct bcm_ifa_opaque_data_profile_lookup_key_s {
    bcm_port_t egress_port; /* Egress port. */
    bcm_cos_queue_t cosq;   /* CoSQ. */
} bcm_ifa_opaque_data_profile_lookup_key_t;

/* Initialize opaque data profile information structure. */
extern void bcm_ifa_opaque_data_profile_t_init(
    bcm_ifa_opaque_data_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an opaque data profile. */
extern int bcm_ifa_opaque_data_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_ifa_opaque_data_profile_t *profile);

/* Get an opaque data profile. */
extern int bcm_ifa_opaque_data_profile_get(
    int unit, 
    int profile_id, 
    bcm_ifa_opaque_data_profile_t *profile);

/* Destroy an opaque data profile. */
extern int bcm_ifa_opaque_data_profile_destroy(
    int unit, 
    int profile_id);

/* Get all opaque data profiles. */
extern int bcm_ifa_opaque_data_profile_get_all(
    int unit, 
    int max_size, 
    int *profile_id, 
    int *count);

/* Get the opaque data profile index. */
extern int bcm_ifa_opaque_data_profile_lookup_index_get(
    int unit, 
    bcm_ifa_opaque_data_profile_lookup_key_t *key, 
    int *index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Options that can be passed during action profile creation. */
#define BCM_IFA_ACTION_PROFILE_CREATE_OPTIONS_WITH_ID (1 << 0)   /* Create an action
                                                          profile with specific
                                                          ID. */
#define BCM_IFA_ACTION_PROFILE_CREATE_OPTIONS_REPLACE (1 << 1)   /* Update an existing
                                                          action profile. */

/* Action profile flags. */
#define BCM_IFA_ACTION_PROFILE_FLAGS_UDP_HDR_UPDATE (1 << 0)   /* Update UDP header
                                                          after metadata
                                                          insertion. */
#define BCM_IFA_ACTION_PROFILE_FLAGS_METADATA_INNER_IP_TTL (1 << 1)   /* Use inner IP TTL in
                                                          metadata. */
#define BCM_IFA_ACTION_PROFILE_FLAGS_L3_MTU_CHECK_FAIL_DROP (1 << 2)   /* Drop IFA packets
                                                          failing L3 MTU check
                                                          due to
                                                          num_bytes_added.
                                                          Applicable only when
                                                          action is
                                                          bcmIfaActionTransit. */

/* IFA actions. */
typedef enum bcm_ifa_action_e {
    bcmIfaActionNone = 0,               /* None. */
    bcmIfaActionTransit = 1,            /* Insert metadata on transit IFA
                                           packet. */
    bcmIfaActionTerminate = 2,          /* Insert last node metadata and
                                           terminate IFA packet. */
    bcmIfaActionInbandInitiate = 3,     /* Inband initiation. */
    bcmIfaActionProbeInitiate = 4,      /* Probe initiation. */
    bcmIfaActionProbeInitiateTerminate = 5, /* Initiate and terminate probe packet. */
    bcmIfaActionCount = 6               /* Last value, not usable. */
} bcm_ifa_action_t;

#define BCM_IFA_ACTION_STRINGS \
{ \
    "None", \
    "Transit", \
    "Terminate", \
    "InbandInitiate", \
    "ProbeInitiate", \
    "ProbeInitiateTerminate", \
    "Count"  \
}

/* IFA initiation layer. */
typedef enum bcm_ifa_initate_layer_e {
    bcmIfaInitateLayerForwarding = 0,   /* Initiate on the forwarding layer. */
    bcmIfaInitateLayerForwardingNoEncap = 1, /* Initiate on the forwarding layer if
                                           no encap is happening. */
    bcmIfaInitateLayerVxlanEncap = 2,   /* Initiate on VxLAN encap layer, if
                                           VxLAN tunnel initiation is happening. */
    bcmIfaInitateLayerPayload = 3,      /* Initiate on the payload layer. */
    bcmIfaInitateLayerCount = 4         /* Always last. Not a usable value. */
} bcm_ifa_initate_layer_t;

#define BCM_IFA_INITATE_LAYER_STRINGS \
{ \
    "Forwarding", \
    "ForwardingNoEncap", \
    "VxlanEncap", \
    "Payload", \
    "Count"  \
}

/* Action profile. */
typedef struct bcm_ifa_action_profile_s {
    uint32 flags;                       /* Flags, see
                                           BCM_IFA_ACTION_PROFILE_FLAGS_XXX. */
    bcm_time_format_t timestamp_format; /* Timestamp format. */
    bcm_ifa_action_t action;            /* Action to be performed on the packet. */
    int metadata_profile_id;            /* Metadata profile Id to be looked up
                                           to construct the metadata. */
    bcm_ifa_header_t ifa_header;        /* IFA header used to initiate IFA. */
    bcm_ifa_initate_layer_t initiate_layer; /* Layer on which to initiate IFA. */
} bcm_ifa_action_profile_t;

/* Initialize action profile information structure. */
extern void bcm_ifa_action_profile_t_init(
    bcm_ifa_action_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an action profile. */
extern int bcm_ifa_action_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_ifa_action_profile_t *profile);

/* Get an action profile. */
extern int bcm_ifa_action_profile_get(
    int unit, 
    int profile_id, 
    bcm_ifa_action_profile_t *profile);

/* Destroy an action profile. */
extern int bcm_ifa_action_profile_destroy(
    int unit, 
    int profile_id);

/* Get all action profiles. */
extern int bcm_ifa_action_profile_get_all(
    int unit, 
    int max_size, 
    int *profile_id, 
    int *count);

/* Set the egress ports for which IFA needs to be terminated. */
extern int bcm_ifa_termination_ports_set(
    int unit, 
    int num_ports, 
    bcm_port_t *ports);

/* Get all the egress ports for which IFA needs to be terminated. */
extern int bcm_ifa_termination_ports_get(
    int unit, 
    int max_ports, 
    bcm_port_t *ports, 
    int *num_ports);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_IFA_H__ */
