/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_COLLECTOR_H__
#define __BCM_COLLECTOR_H__

#include <bcm/types.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* Initializes collector module. */
extern int bcm_collector_init(
    int unit);

/* Detach collector module. */
extern int bcm_collector_detach(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Wire formats that are supported for exporting stats data to the
 * collector
 */
typedef enum bcm_collector_wire_format_e {
    bcmCollectorWireFormatIpfix = 0,    /* Ipfix wire format. */
    bcmCollectorWireFormatProtoBuf = 1, /* Protocol Buffer wire format . */
    bcmCollectorWireFormatCount = 2     /* Last Value. Not Usable. */
} bcm_collector_wire_format_t;

/* Export profile flags. */
#define BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS (1 << 0)   /* Use num_records field
                                                          in export profile
                                                          instead of
                                                          max_packet_length. */

/* PROTOBUF system ID flag. */
#define BCM_COLLECTOR_PROTOBUF_MAX_SYSTEM_ID_LENGTH 32         /* Max ProtoBuf system Id
                                                          length. */

/* Export profile information of stats data sent to the collector. */
typedef struct bcm_collector_export_profile_s {
    uint32 flags;                       /* Export profile flags, see
                                           BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_XXX. */
    bcm_collector_wire_format_t wire_format; /* wire format of export profile. */
    uint32 export_interval;             /* export interval of export profile in
                                           micro seconds. */
    uint32 max_packet_length;           /* Maximum length of export packet sent
                                           to collector. */
    int num_records;                    /* Number of records to be sent in one
                                           packet, can be used instead of
                                           max_packet_length. */
} bcm_collector_export_profile_t;

/* 
 * Defines used to define invalid/default values for export profile
 * structure parameters.
 */
#define BCM_COLLECTOR_EXPORT_INTERVAL_INVALID 0xffffffff /* Invalid export
                                                          interval. Used to
                                                          initialize export
                                                          interval. */

/* Initializes a export profile information structure. */
extern void bcm_collector_export_profile_t_init(
    bcm_collector_export_profile_t *export_profile_info);

/* 
 * Options that can be passed via options parameter during collector
 * export profile creation.
 */
#define BCM_COLLECTOR_EXPORT_PROFILE_WITH_ID (1 << 0)   /* Creates a collector
                                                          export profile with
                                                          ID. */
#define BCM_COLLECTOR_EXPORT_PROFILE_REPLACE (1 << 1)   /* Replaces a collector
                                                          export profile with
                                                          ID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Creates an export profile with given export information. */
extern int bcm_collector_export_profile_create(
    int unit, 
    uint32 options, 
    int *export_profile_id, 
    bcm_collector_export_profile_t *export_profile_info);

/* Fetches an export profile information whose ID is provided. */
extern int bcm_collector_export_profile_get(
    int unit, 
    int export_profile_id, 
    bcm_collector_export_profile_t *export_profile_info);

/* Get all the configured collector export profiles. */
extern int bcm_collector_export_profile_ids_get_all(
    int unit, 
    int max_size, 
    int *export_profile_ids_list, 
    int *list_size);

/* Destroy a collector export profile. */
extern int bcm_collector_export_profile_destroy(
    int unit, 
    int export_profile_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Macros for bcm_collector_eth_header_t.vlan_tag_structure to indicate
 * the tag structure of vlans.
 */
#define BCM_COLLECTOR_ETH_HDR_UNTAGGED      0x1        
#define BCM_COLLECTOR_ETH_HDR_SINGLE_TAGGED 0x2        
#define BCM_COLLECTOR_ETH_HDR_DOUBLE_TAGGED 0x3        

/* 
 * Ethernet header in the encap used to send the stats data packet to the
 * collector.
 */
typedef struct bcm_collector_eth_header_s {
    bcm_mac_t dst_mac; 
    bcm_mac_t src_mac; 
    uint8 vlan_tag_structure; 
    uint16 outer_tpid; 
    uint16 inner_tpid; 
    bcm_vlan_tag_t outer_vlan_tag; 
    bcm_vlan_tag_t inner_vlan_tag; 
} bcm_collector_eth_header_t;

/* 
 * IPv4 header in the encap used to send the stats data packet to the
 * collector.
 */
typedef struct bcm_collector_ipv4_header_s {
    bcm_ip_t src_ip; 
    bcm_ip_t dst_ip; 
    uint8 dscp; 
    uint8 ttl; 
} bcm_collector_ipv4_header_t;

/* 
 * IPv6 header in the encap used to send the stats data packet to the
 * collector.
 */
typedef struct bcm_collector_ipv6_header_s {
    bcm_ip6_t src_ip; 
    bcm_ip6_t dst_ip; 
    uint8 traffic_class; 
    uint32 flow_label; 
    uint8 hop_limit; 
} bcm_collector_ipv6_header_t;

/* UDP header flags. */
#define BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE (1 << 0)   /* Add UDP checksum in
                                                          the export packet */

/* 
 * UDP header in the encap used to send the stats data packet to the
 * collector.
 */
typedef struct bcm_collector_udp_header_s {
    uint32 flags;           /* UDP header flags, see
                               BCM_COLLECTOR_UDP_FLAGS_XXX. */
    bcm_l4_port_t src_port; 
    bcm_l4_port_t dst_port; 
} bcm_collector_udp_header_t;

/* IPFIX observation domain. */
typedef uint32 bcm_collector_ipfix_observation_domain_t;

/* 
 * IPFIX header in the encap used to send the data packet to the remote
 * collector.
 */
typedef struct bcm_collector_ipfix_header_s {
    int version;                        /* Ipfix Protocol Version. */
    int initial_sequence_num;           /* Ipfix initial sequence number. */
    bcm_collector_ipfix_observation_domain_t observation_domain_id; /* IPFIX observation domain from which
                                           this collector is receiving the
                                           export data.. */
    uint32 enterprise_number;           /* IPFIX enterprise number. */
} bcm_collector_ipfix_header_t;

/* ProtoBuf config structure. */
typedef struct bcm_collector_protobuf_header_s {
    int system_id_length;               /* system_id_length. */
    uint8 system_id[BCM_COLLECTOR_PROTOBUF_MAX_SYSTEM_ID_LENGTH]; /* System Id. */
    uint32 component_id;                /* Component Id. */
} bcm_collector_protobuf_header_t;

/* Software ID given for a Collector configuration. */
typedef int bcm_collector_t;

/* 
 * Transport types that are supported for exporting stats data to the
 * collector
 */
typedef enum bcm_collector_transport_type_e {
    bcmCollectorTransportTypeIpv4Udp = 0, /* The encap will be of the format UDP
                                           over IPv4. */
    bcmCollectorTransportTypeIpv6Udp = 1, /* The encap will be of the format UDP
                                           over IPv6. */
    bcmCollectorTransportTypeRaw = 2,   /* Indicates no encap. This will be
                                           useful when the raw stats data PDUs
                                           need to be transported to the local
                                           CPU. */
    bcmCollectorTransportTypeCount = 3  /* Last Value. Not Usable. */
} bcm_collector_transport_type_t;

/* Collector types. */
typedef enum bcm_collector_type_e {
    bcmCollectorRemote = 0, /* The collector is a remote machine expecting flow
                               information records using encap scheme defined
                               through TransportTypeXxx. */
    bcmCollectorLocal = 1,  /* The collector is local CPU receiving flow
                               information records usually without any encap.
                               TransportTypeRaw. */
    bcmCollectorCount = 2   /* Last Value. Not Usable. */
} bcm_collector_type_t;

/* The different timestamp sources in collector to compute ToD */
typedef enum bcm_collector_timestamp_source_e {
    bcmCollectorTimestampSourceLegacy = 0, /* Legacy source */
    bcmCollectorTimestampSourceNTP = 1, /* Network Time Protocol source */
    bcmCollectorTimestampSourcePTP = 2, /* Precision Time Protocol source */
    bcmCollectorTimestampSourceCount = 3 /* Always Last. Not a usable value */
} bcm_collector_timestamp_source_t;

#define BCM_COLLECTOR_TIMESTAMP_SOURCE_STRINGS \
{ \
    "Legacy", \
    "NTP", \
    "PTP"  \
}

/* Collector information. */
typedef struct bcm_collector_info_s {
    bcm_collector_type_t collector_type; /* Collector type. Remote vs Local */
    bcm_collector_transport_type_t transport_type; /* Transport type used for exporting
                                           data to the collector. This
                                           identifies the usable fields within
                                           the encap structure member defined
                                           below. */
    bcm_collector_eth_header_t eth;     /* Ethernet encapsulation of the packet
                                           sent to collector. */
    bcm_collector_ipv4_header_t ipv4;   /* IPv4 encapsulation of the packet sent
                                           to collector. */
    bcm_collector_ipv6_header_t ipv6;   /* IPv6 encapsulation of the packet sent
                                           to collector. */
    bcm_collector_udp_header_t udp;     /* UDP encapsulation of the packet sent
                                           to collector. */
    bcm_collector_ipfix_header_t ipfix; /* IPFIX encapsulation of the packet
                                           sent to collector. */
    bcm_collector_protobuf_header_t protobuf; /* Protobuf header information. */
    bcm_collector_timestamp_source_t src_ts; /* Timestamp source info. */
    int max_records_reserve;            /* Maximum number of export record
                                           entries reserved in the hardware FIFO
                                           for the collector. */
} bcm_collector_info_t;

/* Initializes a collector information structure. */
extern void bcm_collector_info_t_init(
    bcm_collector_info_t *collector_info);

/* 
 * Options that can be passed via options parameter during collector
 * creation.
 */
#define BCM_COLLECTOR_WITH_ID   (1 << 0)   /* Create a collector with ID. */
#define BCM_COLLECTOR_REPLACE   (1 << 1)   /* Replace a collector with ID. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a collector. */
extern int bcm_collector_create(
    int unit, 
    uint32 options, 
    bcm_collector_t *collector_id, 
    bcm_collector_info_t *collector_info);

/* Gets a Collector information. */
extern int bcm_collector_get(
    int unit, 
    bcm_collector_t id, 
    bcm_collector_info_t *collector_info);

/* Get all the configured collectors. */
extern int bcm_collector_get_all(
    int unit, 
    int max_size, 
    bcm_collector_t *collector_list, 
    int *list_size);

/* Destroys a collector. */
extern int bcm_collector_destroy(
    int unit, 
    bcm_collector_t id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Max size of the data record received at the CPU through the FIFO DMA
 * reception.
 */
#define BCM_COLLECTOR_EXPORT_RECORD_LENGTH  BITS2BYTES(1024) /* Maximum length of a
                                                          data record. 1024
                                                          bits. */

/* 
 * Collector Export record structure. Data record info passed to
 * application software by SDK Software. The unprocessed data record will
 * be presented in network byte order.
 */
typedef struct bcm_collector_export_record_s {
    int size;                           /* Number of valid octets in the
                                           data_record array for the device. */
    uint8 data_record[BCM_COLLECTOR_EXPORT_RECORD_LENGTH]; /* Raw data record in network byte
                                           order. */
} bcm_collector_export_record_t;

/* bcm_collector_export_record_t_init */
extern void bcm_collector_export_record_t_init(
    bcm_collector_export_record_t *record);

/* 
 * Defines for priorities of registered handlers for Collector export
 * records. Callback handlers are always called from highest priority to
 * lowest priority until one of the handlers returns record_handled or
 * record_handled_owned.
 * 
 * 
 * When callback handling proecedure starts, a default callback handler
 * is installed at BCM_COLLECTOR_EXPORT_CB_PRIO_MIN priority that simply
 * discards the record.
 */
#define BCM_COLLECTOR_EXPORT_CB_PRIO_MIN    ((uint8)0) 
#define BCM_COLLECTOR_EXPORT_CB_PRIO_MAX    ((uint8)255) 

/* Export callback options */
typedef struct bcm_collector_callback_options_s {
    int callback_prio;  /* Priority of the callback function in the list. */
} bcm_collector_callback_options_t;

/* bcm_collector_callback_options_t_init */
extern void bcm_collector_callback_options_t_init(
    bcm_collector_callback_options_t *callback_options);

/* Return values from export record callout routines. */
typedef enum bcm_collector_export_record_handle_e {
    BCM_COLLECTOR_EXPORT_RECORD_INVALID = 0, /* Invalid return value. */
    BCM_COLLECTOR_EXPORT_RECORD_NOT_HANDLED = 1, /* Export record not processed. */
    BCM_COLLECTOR_EXPORT_RECORD_HANDLED = 2, /* Export record handled, not owned. */
    BCM_COLLECTOR_EXPORT_RECORD_HANDLED_OWNED = 3 /* Export record handled and owned. */
} bcm_collector_export_record_handle_t;

/* Callback routine for collector record exported to local collector. */
typedef bcm_collector_export_record_handle_t (*bcm_collector_export_record_cb_f)(
    int unit, 
    bcm_collector_t collector_id, 
    bcm_collector_export_record_t *record, 
    void *userdata);

#ifndef BCM_HIDE_DISPATCHABLE

/* Register callback routine for Local collector record export. */
extern int bcm_collector_export_record_register(
    int unit, 
    bcm_collector_t collector_id, 
    bcm_collector_callback_options_t callback_options, 
    bcm_collector_export_record_cb_f callback_fn, 
    void *userdata);

/* Unregister callback routine for Local collector record export. */
extern int bcm_collector_export_record_unregister(
    int unit, 
    bcm_collector_t collector_id, 
    bcm_collector_callback_options_t callback_options, 
    bcm_collector_export_record_cb_f callback_fn);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_COLLECTOR_H__ */
