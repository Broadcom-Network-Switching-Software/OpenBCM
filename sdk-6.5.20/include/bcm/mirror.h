/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_MIRROR_H__
#define __BCM_MIRROR_H__

#include <bcm/types.h>
#include <bcm/pkt.h>

/* Mirror modes for bcm_mirror_mode_set/get. */
#define BCM_MIRROR_DISABLE      0          /* Disable mirroring. */
#define BCM_MIRROR_L2           1          /* Enable mirroring. */
#define BCM_MIRROR_L2_L3        2          /* Deprecated. */

/* Flags for bcm_mirror_port_set/get. */
#define BCM_MIRROR_PORT_ENABLE          0x00000001 /* Enable mirroring. */
#define BCM_MIRROR_PORT_INGRESS         0x00000002 /* Ingress mirroring. */
#define BCM_MIRROR_PORT_EGRESS          0x00000004 /* Egress mirroring. */
#define BCM_MIRROR_PORT_EGRESS_TRUE     0x00000008 /* True egress mirroring. */
#define BCM_MIRROR_PORT_DEST_TRUNK      0x00000010 /* Parameter dest_port is a
                                                      trunk. */
#define BCM_MIRROR_PORT_EGRESS_ACL      0x00000020 /* Enable Egress Mirroring
                                                      for ACLs */
#define BCM_MIRROR_PORT_SFLOW           0x00000040 /* sFlow mirroring */
#define BCM_MIRROR_PORT_UNTAGGED_ONLY   0x00000080 /* Mirror Untagged traffic -
                                                      Ingress Mirroring only. */
#define BCM_MIRROR_PORT_TAGGED_DEFAULT  0x00000100 /* Mirror tagged traffic with
                                                      unspecified VID - Ingress
                                                      Mirroring only. */
#define BCM_MIRROR_PORT_PSAMP           0x00000200 /* Packet Sampling - Ingress
                                                      only. */
#define BCM_MIRROR_PORT_INT             0x00000400 /* INT (Inband Network
                                                      Telemetry) Mirroring -
                                                      Ingress only. */
#define BCM_MIRROR_PORT_ELEPHANT        0x00000800 /* Elephant Flow Mirroring -
                                                      Ingress only. */
#define BCM_MIRROR_PORT_DLB_MONITOR     0x00001000 /* DLB Flow Monitor Mirroring
                                                      - Ingress only */

/* Flags for bcm_mirror_destination_create/traverse. */
#define BCM_MIRROR_DEST_REPLACE             (1U << 0)  /* Replace mirror
                                                          destination. */
#define BCM_MIRROR_DEST_WITH_ID             (1U << 1)  /* Mirror destination id
                                                          is valid. */
#define BCM_MIRROR_DEST_TUNNEL_L2           (1U << 2)  /* Mirrored packet should
                                                          be L2 tunneled. */
#define BCM_MIRROR_DEST_TUNNEL_IP_GRE       (1U << 3)  /* Mirrored packet should
                                                          be L3 GRE tunneled. */
#define BCM_MIRROR_DEST_PAYLOAD_UNTAGGED    (1U << 4)  /* Strip vlan tag from
                                                          mirrored packet. */
#define BCM_MIRROR_DEST_TUNNEL_TRILL        (1U << 5)  /* Mirrored packet should
                                                          be TRILL tunneled. */
#define BCM_MIRROR_DEST_TUNNEL_NIV          (1U << 6)  /* Mirrored packet should
                                                          be NIV tunneled. */
#define BCM_MIRROR_DEST_UPDATE_POLICER      (1U << 7)  /* update policer using
                                                          policer_id */
#define BCM_MIRROR_DEST_UPDATE_COUNTER      (1U << 8)  /* update counter using
                                                          stat_id */
#define BCM_MIRROR_DEST_DEST_MULTICAST      (1U << 9)  /* destination is ingress
                                                          multicast */
#define BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID (1U << 10) /* Mirror destination
                                                          encap_id is valid */
#define BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID (1U << 11) /* Mirrored packet should
                                                          be with ERSPAN
                                                          encapsulation */
#define BCM_MIRROR_DEST_TUNNEL_RSPAN        (1U << 12) /* Mirrored packet should
                                                          be RSPAN (VLAN)
                                                          tunneled */
#define BCM_MIRROR_DEST_INT_PRI_SET         (1U << 13) /* Mirrored packet should
                                                          set with internal
                                                          priority */
#define BCM_MIRROR_DEST_TUNNEL_ETAG         (1U << 14) /* Mirrored packet should
                                                          be ETAG tunneled. */
#define BCM_MIRROR_DEST_TUNNEL_SFLOW        (1U << 15) /* Mirrored packet should
                                                          be with sFlow
                                                          encapsulation */
#define BCM_MIRROR_DEST_IS_SNOOP            (1U << 16) /* Specify that the
                                                          destination type is a
                                                          snoop destination. */
#define BCM_MIRROR_DEST_IS_TRAP             (1U << 17) /* Specify that the
                                                          destination is a trap
                                                          destination. */
#define BCM_MIRROR_DEST_IS_STAT_SAMPLE      (1U << 17) /* Specify that the
                                                          destination type is a
                                                          STATISTIC SAMPLING
                                                          destination. */
#define BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER (1U << 18) /* The start of the
                                                          original packet system
                                                          header will be
                                                          included in the
                                                          outbound mirrored
                                                          packets. */
#define BCM_MIRROR_DEST_FIELD               (1U << 19) /* Specify that mirror
                                                          destination owner is
                                                          FP. */
#define BCM_MIRROR_DEST_PORT                (1U << 20) /* Specify that mirror
                                                          destination owner is
                                                          PORT. */
#define BCM_MIRROR_DEST_DROP_SNIFF_IF_FWD_DROPPED (1U << 21) /* Drop the sniff packet
                                                          if the original packet
                                                          is dropped */
#define BCM_MIRROR_DEST_UPDATE_COUNTER_1    (1U << 21) /* update counter No.1 */
#define BCM_MIRROR_DEST_UPDATE_COUNTER_2    (1U << 22) /* update counter No.2 */
#define BCM_MIRROR_DEST_DROP_FWD_IF_SNIFF_DROPPED (1U << 22) /* Drop the original
                                                          packet if the sniff
                                                          packet is dropped */
#define BCM_MIRROR_DEST_UPDATE_TUNNEL_SFLOW (1U << 23) /* Update mirror
                                                          destination with sflow
                                                          encapsulation */
#define BCM_MIRROR_DEST_ID_SHARE            (1U << 24) /* Share mirror
                                                          destination id among
                                                          multi ports */
#define BCM_MIRROR_DEST_EEI_INVALID         (1U << 24) /* Set EEI invalid */
#define BCM_MIRROR_DEST_MTP_ADD             (1U << 25) /* Add an MTP into a
                                                          shared-id mirror
                                                          destination */
#define BCM_MIRROR_DEST_MTP_DELETE          (1U << 26) /* Delete an MTP from a
                                                          shared-id mirror
                                                          destination */
#define BCM_MIRROR_DEST_MTP_REPLACE         (1U << 27) /* REPLACE an MTP of a
                                                          shared-id mirror
                                                          destination */
#define BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER (1U << 28) /* For packets recycled
                                                          by egress applications
                                                          and not forwarded,
                                                          retain the incoming
                                                          packet's system
                                                          headers as received
                                                          from the fabric, and
                                                          in case of an Egress
                                                          MC packet, stamp the
                                                          FTMH.OutLIF field with
                                                          the CUD. */
#define BCM_MIRROR_DEST_TUNNEL_PSAMP        (1U << 29) /* Update mirror
                                                          destination with PSAMP
                                                          encapsulation */
#define BCM_MIRROR_DEST_OUT_MIRROR_DISABLE  (1U << 30) /* Assert out mirror
                                                          disable */
#define BCM_MIRROR_DEST_TUNNEL_WITH_SEQ     (1U << 31) /* Include Sequence
                                                          number in header. */
#define BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS (1U << 31) /* Set statistic
                                                          interfaces for
                                                          mirrored packets. */
#define BCM_MIRROR_DEST_FLAGS2_TUNNEL_VXLAN (1U << 0)  /* Mirror Tunnel is
                                                          VXLAN. */
#define BCM_MIRROR_DEST_FLAGS2_PSAMP_FORMAT_2 (1U << 1)  /* Use format2 header for
                                                          PSAMP */
#define BCM_MIRROR_DEST_FLAGS2_INT_PROBE    (1U << 2)  /* Add INT(Inband Network
                                                          Telemetry) probe
                                                          header. */
#define BCM_MIRROR_DEST_FLAGS2_IFA          (1U << 3)  /* Add IFA(In-band Flow
                                                          Analyzer) header. */
#define BCM_MIRROR_DEST_FLAGS2_UPDATE_EXT_STAT_VALID (1U << 4)  /* Update external
                                                          statistics valid bit
                                                          for mirror packets */
#define BCM_MIRROR_DEST_FLAGS2_UPDATE_EXT_STAT_TYPES (1U << 5)  /* Update external
                                                          statistics types for
                                                          mirror packets */
#define BCM_MIRROR_DEST_FLAGS2_MIRROR_ON_DROP (1U << 6)  /* Indicate mirror on
                                                          drop profile. */
#define BCM_MIRROR_DEST_FLAGS2_MOD_TUNNEL_PSAMP (1U << 7)  /* Mirror packet with
                                                          PSAMP mirror-on-drop
                                                          encapsulation. */
#define BCM_MIRROR_DEST_FLAGS2_LOOPBACK_ENABLE (1U << 8)  /* Enable to mirror
                                                          packet to loopback
                                                          port. */
#define BCM_MIRROR_DEST_FLAGS2_TRUE_EGRESS  (1U << 9)  /* True egress mirroring. */
#define BCM_MIRROR_DEST_FLAGS2_BTE_TIMESTAMP_UPDATE (1U << 10) /* Enable timestamp
                                                          update in BTE. */

/* Flags for bcm_mirror_pkt_header_updates_t. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_COLOR  (1 << 0)   /* Drop precedence. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_PRIO   (1 << 1)   /* Internal packet
                                                          priority. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE (1 << 2)   /* ECN capable and
                                                          congestion encoding. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_ECN_CNM_CANCEL (1 << 3)   /* Ignore Congestion
                                                          Point. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT (1 << 4)   /* LAG load balancing
                                                          key. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_IN_PORT (1 << 5)   /* 8b should be exposed? */
#define BCM_MIRROR_PKT_HEADER_UPDATE_VSQ    (1 << 6)   /* selects STF
                                                          (statistics flow) VSQ. */
#define BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING (1 << 7)   /* changes to the fabric
                                                          headers */
#define BCM_MIRROR_PKT_HEADER_UPDATE_METADATA (1 << 14)  /* The metadata for CRPS
                                                          should be set
                                                          explicitly updated */

/* Flags for mirror NIV tunneling */
#define BCM_MIRROR_NIV_LOOP     1          /* Mirrored packet is a loop in NIV
                                              path */

/*  represents the options for the mirroring of packets */
typedef struct bcm_mirror_options_s {
    uint32 flags; 
    uint8 forward_strength; 
    uint8 copy_strength; 
    uint32 recycle_cmd;     /* dedicated for ETPP traps */
} bcm_mirror_options_t;

/* Flags for ftmh_valid in bcm_mirror_pkt_header_fabric_t */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_PACKET_SIZE (1 << 0)   /* Packet size. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_PRIO (1 << 1)   /* Traffic Class. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_SRC_SYSPORT (1 << 2)   /* Source System port. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_DST_PORT (1 << 3)   /* Destination local
                                                          port. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_DP (1 << 4)   /* Drop precedence. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_ACTION_TYPE (1 << 5)   /* Action type */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_OUT_MIRROR_DISABLE (1 << 6)   /* Disable Outbound
                                                          mirroring. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_IS_MC_TRAFFIC (1 << 7)   /* Indicate if the
                                                          traffic is multicast */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_MULTICAST_ID (1 << 8)   /* Multicast ID */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_OUT_VPORT (1 << 9)   /* Virtual port */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_CNI (1 << 10)  /* Congestion indication */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_LB_KEY (1 << 11)  /* Load Balancing Key */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_DST_SYSPORT (1 << 12)  /* Destination System
                                                          Port */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_STACK_ROUTE (1 << 13)  /* Route bitmap to
                                                          prevent loops in
                                                          stacking system */

/* Flags for oam_ts_valid in bcm_mirror_pkt_header_fabric_t. */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_TYPE (1 << 0)   /* Type */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_SUB_TYPE (1 << 1)   /* OAM-Sub-Type */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_UP_MEP (1 << 2)   /* MEP-Type */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_TP_CMD (1 << 3)   /* TP-Cmd */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_ENCAPS (1 << 4)   /* TS-Encaps */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_DATA (1 << 5)   /* OAM-TS-Data */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_LATENCY_FLOW_ID (1 << 6)   /* Latency flow ID */
#define BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_OFFSET (1 << 7)   /* Offset */

/* Fabric header updates of mirrored packets */
typedef struct bcm_mirror_pkt_header_fabric_s {
    uint32 ftmh_valid;                  /* Used to specify which fields to use
                                           for header changes. Possible values
                                           will be named
                                           BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_ */
    uint32 internal_valid;              /* Used to specify which fields to use
                                           for header changes. Possible values
                                           will be named
                                           BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_INTERNAL_ */
    uint32 oam_ts_valid;                /* Used to specify which fields to use
                                           for header changes. Possible values
                                           will be named
                                           BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_ */
    bcm_pkt_dnx_ftmh_t ftmh;            /* FTMH header fields */
    bcm_pkt_dnx_internal_t internal;    /* Internal header fields */
    bcm_pkt_dnx_otsh_t oam_ts;          /* OAM-TS header fields */
} bcm_mirror_pkt_header_fabric_t;

/* Flags for bcm_mirror_options_s */
#define BCM_MIRROR_OPTIONS_COPY_DROP    (1 << 0)   /* drop the copied packet */
#define BCM_MIRROR_OPTIONS_FORWARD_DROP (1 << 1)   /* drop the original packet */

/*   represents header updates of mirrored packets */
typedef struct bcm_mirror_pkt_header_updates_s {
    uint32 valid;                       /* Used to specify which fields to use
                                           for header changes. Possible values
                                           will be named
                                           BCM_MIRROR_PKT_HEADER_UPDATE_* */
    bcm_color_t color;                  /* drop precedence */
    uint8 prio;                         /* the internal packet priority (traffic
                                           class before ingress mapping to cosq) */
    uint8 ecn_value;                    /* ECN capable and congestion encoding */
    uint8 cnm_cancel;                   /* Ignore Congestion Point (CNM) */
    uint32 trunk_hash_result;           /* LAG load balancing key */
    bcm_gport_t in_port;                /* 8b should be exposed? */
    uint16 vsq;                         /*  selects STF (statistics flow) VSQ */
    uint16 fabric_header_editing;       /* changes to the fabric headers */
    bcm_mirror_pkt_header_fabric_t header_fabric; /* Fabric header updates of mirrored
                                           packets */
    uint32 metadata;                    /* Metadata for counting */
} bcm_mirror_pkt_header_updates_t;

/* Do not perform truncation or payload zeroing */
#define BCM_MIRROR_PAYLOAD_DO_NOT_TRUNCATE  0          

/* Perform payload truncation of mirrored packets */
#define BCM_MIRROR_PAYLOAD_TRUNCATE 1          

/* Perform payload truncation and zeroing of mirrored packets */
#define BCM_MIRROR_PAYLOAD_TRUNCATE_AND_ZERO 2          

/* 
 * Invalid protocol or port value. Used when there is no associated
 * protocol or port value for a given offset
 */
#define BCM_MIRROR_PAYLOAD_ZERO_INVALID_PROTO_PORT 0xFFFFFFFF 

/* Invalid offset value. Used to set a offset field in case of an error */
#define BCM_MIRROR_PAYLOAD_ZERO_INVALID_OFFSET 0xFFFFFFFF 

/* 
 * The following enum defines identifiers for accessing protocol/port
 * numbers and/or the corresponding offset values for a particular
 * protocol, or a layer
 */
typedef enum bcm_mirror_payload_zero_field_e {
    bcmMirrorPayloadZeroIp4Protocol0 = 0, /* 1st of upto 3 IPv4 protocol values */
    bcmMirrorPayloadZeroIp4Protocol1 = 1, /* 2nd of upto 3 IPv4 protocol values */
    bcmMirrorPayloadZeroIp4Protocol2 = 2, /* 3rd of upto 3 IPv4 protocol values */
    bcmMirrorPayloadZeroIp6NxtHdr0 = 3, /* 1st of upto 3 IPv6 nxthdr values */
    bcmMirrorPayloadZeroIp6NxtHdr1 = 4, /* 2nd of upto 3 IPv6 nxthdr values */
    bcmMirrorPayloadZeroIp6NxtHdr2 = 5, /* 3rd of upto 3 IPv6 nxthdr values */
    bcmMirrorPayloadZeroUdpPort0 = 6,   /* First of upto 2 UDP port values */
    bcmMirrorPayloadZeroUdpPort1 = 7,   /* Second of upto 2 UDP port values */
    bcmMirrorPayloadZeroIpInIpOffset = 8, /* IpInIP offset. No corresponding
                                           protocol value associated */
    bcmMirrorPayloadZeroL2Offset = 9,   /* L2 offset. No corresponding protocol
                                           value associated */
    bcmMirrorPayloadZeroL3Offset = 10,  /* L3 offset. No corresponding protocol
                                           value associated */
    bcmMirrorPayloadZeroMplsOffset = 11, /* MPLS offset. No corresponding
                                           protocol value associated */
    bcmMirrorPayloadZeroCount = 12      /* Last member, not a usable value */
} bcm_mirror_payload_zero_field_t;

/* 
 * Structure to pass information from, or to the caller. Used for
 * accessing protocol/port numbers and/or the corresponding offset values
 * for a particular protocol, or a layer
 */
typedef struct bcm_mirror_payload_zero_info_s {
    bcm_mirror_payload_zero_field_t field_id; /* Identifies protocol or port item to
                                           be programmed, or read. Values below
                                           belong to this item */
    uint32 protocol_port_value;         /* Specifies protocol number or port
                                           number. Is not valid for all
                                           field_id's */
    uint32 protocol_port_offset;        /* Offset, in bytes, corresponding to
                                           field_id. Valid for all field_id's */
} bcm_mirror_payload_zero_info_t;

/* 
 * Sflow/Psamp hdr's meta data field carries one of the values mentioned
 * below
 */
typedef enum bcm_mirror_psamp_fmt2_meta_data_e {
    bcmMirrorPsampFmt2HeaderZeroes = 0, /* All zeroes */
    bcmMirrorPsampFmt2HeaderI2eClassId = 1, /* I2E class id */
    bcmMirrorPsampFmt2HeaderNextHopClassId = 2, /* Next hop class id */
    bcmMirrorPsampFmt2HeaderUserMeta = 3, /* User meta data (opaque) value */
    bcmMirrorPsampFmt2HeaderCount = 4   /* Last member. Not a usable value */
} bcm_mirror_psamp_fmt2_meta_data_t;

/* Timestamp mode for Mirror encapsulation. */
typedef enum bcm_mirror_timestamp_mode_e {
    bcmMirrorTimestampModePTP = 0,  /* PTP timestamp mode. */
    bcmMirrorTimestampModeNTP = 1,  /* NTP timestamp mode. */
    bcmMirrorTimestampModeCount = 2 /* Must be last. Not a valid value. */
} bcm_mirror_timestamp_mode_t;

/* Multiple destination IP group. */
typedef int bcm_mirror_multi_dip_group_t;

/* 
 * Invalid multiple destination IP group (feature disabled). Default
 * value for multi_dip_group.
 */
#define BCM_MIRROR_MULTI_DIP_GROUP_INVALID  0          /* Default value for
                                                          multi_dip_group. */

/* Port Selection Criteria for mirroring. */
typedef enum bcm_mirror_psc_e {
    bcmMirrorPscNone = 0,       /* Invalid rtag. */
    bcmMirrorPscSrcMac = 1,     /* Source MAC address. */
    bcmMirrorPscDstMac = 2,     /* Destination MAC address. */
    bcmMirrorPscSrcDstMac = 3,  /* Source+dest MAC address. */
    bcmMirrorPscSrcIp = 4,      /* Source IP address. */
    bcmMirrorPscDstIp = 5,      /* Destination IP address. */
    bcmMirrorPscSrcDstIp = 6,   /* Source+dest IP address. */
    bcmMirrorPscCount = 7       /* Must be last. Not a usable value. */
} bcm_mirror_psc_t;

#define BCM_MIRROR_EXT_STAT_ID_COUNT    4          /* Number of statistic
                                                      interfaces for mirrored
                                                      packets. */

/* Contains information required for ERSPAN encapsulation header. */
typedef struct bcm_mirror_pkt_erspan_encap_s {
    uint8 truncated_flag;           /* If set to 0x1, the frame copy
                                       encapsulated in the ERSPAN packet will be
                                       truncated. This should be set if ERSPAN
                                       encapsulated frame exceeds the configured
                                       MTU. */
    uint16 session_id;              /* Identification associated with each
                                       ERSPAN session. */
    uint16 gbp_sid;                 /* Group Based Policy Source Id */
    uint8 hw_id;                    /* Unique identifier of an ERSPAN engine
                                       within a system. */
    uint8 timestamp_granularity;    /* If it is not set, NTP timestamp will be
                                       used. If it is set to 0x1, PTP timestamp
                                       will used. */
    uint8 optional_hdr;             /* Indicates if the optional
                                       platform-specific sub-header is present. */
    uint8 platform_id;              /* Platform identifier to parse the optional
                                       platform-specific sub-header. Valid only
                                       if optional_hdr is set. */
    uint16 user_defined_value;      /* To store the user defined field */
    uint16 switch_id;               /* Identifies a source switch at the
                                       receiving end. Valid only if optional_hdr
                                       is set and platform_id as 0x5 or 0x6. */
} bcm_mirror_pkt_erspan_encap_t;

/* Mirror truncate mode. */
typedef enum bcm_mirror_truncate_mode_e {
    bcmMirrorTruncateDefault = 0,       /* Truncate to one cell by default. */
    bcmMirrorTruncateAdjustment = 1,    /* Adjust payload truncate length. */
    bcmMirrorTruncateAdjustFromL2Encap = 2, /* Truncate from the end of L2
                                           encapsulation headers with
                                           adjustment. */
    bcmMirrorTruncateAdjustFromL3L4Encap = 3, /* Truncate from the end of L3 and L4
                                           encapsulation headers with
                                           adjustment. */
    bcmMirrorTruncateAdjustFromL2 = 4,  /* Truncate from the end of the known L2
                                           headers with adjustment. */
    bcmMirrorTruncateAdjustFromL3L4 = 5, /* Truncate from the end of the known L3
                                           and L4 headers with adjustment. */
    bcmMirrorTruncateToMultipleOf64 = 6, /* Truncate to the size in multiples of
                                           64 bytes where adjustment_length
                                           specifies the multiplier. */
    bcmMirrorTruncateCount = 7          /* Last member, not a usable value. */
} bcm_mirror_truncate_mode_t;

/* Flags for bcm_mirror_int_probe_header_t. */
#define BCM_MIRROR_INT_PROBE_HEADER_UPDATE_MAX_LENGTH (1 << 0)   /* Indicates to fill
                                                          maximum length field
                                                          with specified value. */
#define BCM_MIRROR_INT_PROBE_HEADER_UPDATE_HOP_LIMIT (1 << 1)   /* Indicates to fill hop
                                                          limit field with
                                                          specified value. */

/* INT(Inband Network Telemetry) probe header information. */
typedef struct bcm_mirror_int_probe_header_s {
    uint32 flags;       /* See BCM_MIRROR_INT_PROBE_HEADER_xxx flag definitions. */
    uint16 max_length;  /* Maximum length. */
    uint8 hop_limit;    /* Hop limit */
} bcm_mirror_int_probe_header_t;

/* Mirror loopback header type. */
typedef enum bcm_mirror_loopback_header_type_e {
    bcmMirrorLoopbackHeaderGeneric = 0, /* Generic loopback header. */
    bcmMirrorLoopbackHeaderIfa = 1,     /* IFA specific loopback header. */
    bcmMirrorLoopbackHeaderCount = 2    /* Last member, not a usable value. */
} bcm_mirror_loopback_header_type_t;

/* 
 * Mirror destination Structure
 * 
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_mirror_destination_s {
    bcm_gport_t mirror_dest_id;         /* Unique mirror destination and
                                           encapsulation identifier. */
    uint32 flags;                       /* See BCM_MIRROR_DEST_xxx flag
                                           definitions. */
    bcm_gport_t gport;                  /* Mirror destination. */
    uint8 version;                      /* IP header version. */
    uint8 tos;                          /* Traffic Class/Tos byte. */
    uint8 ttl;                          /* Hop limit. */
    bcm_ip_t src_addr;                  /* Tunnel source ip address (IPv4). */
    bcm_ip_t dst_addr;                  /* Tunnel destination ip address (IPv4). */
    bcm_ip6_t src6_addr;                /* Tunnel source ip address (IPv6). */
    bcm_ip6_t dst6_addr;                /* Tunnel destination ip address (IPv6). */
    uint32 flow_label;                  /* IPv6 header flow label field. */
    bcm_mac_t src_mac;                  /* L2 source mac address. */
    bcm_mac_t dst_mac;                  /* L2 destination mac address. */
    uint16 tpid;                        /* L2 header outer TPID. */
    bcm_vlan_t vlan_id;                 /* Vlan id. */
    bcm_trill_name_t trill_src_name;    /* TRILL source bridge nickname */
    bcm_trill_name_t trill_dst_name;    /* TRILL destination bridge nickname */
    int trill_hopcount;                 /* TRILL hop count */
    uint16 niv_src_vif;                 /* Source Virtual Interface of NIV tag */
    uint16 niv_dst_vif;                 /* Destination Virtual Interface of NIV
                                           tag */
    uint32 niv_flags;                   /* NIV flags BCM_MIRROR_NIV_XXX */
    uint16 gre_protocol;                /* L3 GRE header protocol */
    bcm_policer_t policer_id;           /* policer_id */
    int stat_id;                        /* stat_id */
    int stat_id2;                       /* stat_id2 for second counter pointer */
    bcm_if_t encap_id;                  /* Encapsulation index */
    bcm_if_t tunnel_id;                 /* IP tunnel for encapsulation. Valid
                                           only if BCM_MIRROR_DEST_TUNNEL_IP_GRE
                                           is set */
    uint16 span_id;                     /* SPAN-ID. Valid only if
                                           BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID
                                           is set */
    uint8 pkt_prio;                     /* L2 header PCP */
    uint32 sample_rate_dividend;        /* The probability of mirroring a packet
                                           is: sample_rate_dividend >=
                                           sample_rate_divisor ? 1 :
                                           sample_rate_dividend /
                                           sample_rate_divisor */
    uint32 sample_rate_divisor; 
    uint8 int_pri;                      /* Internal Priority */
    uint16 etag_src_vid;                /* Extended (source) port vlan id */
    uint16 etag_dst_vid;                /* Extended (destination) port vlan id */
    uint16 udp_src_port;                /* UDP source port */
    uint16 udp_dst_port;                /* UDP destination port */
    uint32 egress_sample_rate_dividend; /* The probability of outbound mirroring
                                           a packet from the destination is
                                           sample_rate_dividend >=
                                           sample_rate_divisor ? 1 :
                                           sample_rate_dividend /
                                           sample_rate_divisor */
    uint32 egress_sample_rate_divisor; 
    uint8 recycle_context;              /* recycle context of egress originating
                                           packets */
    uint16 packet_copy_size;            /*  If non zero, the copied packet will
                                           be truncated to the first
                                           packet_copy_size . Current supported
                                           values for DNX are 0, 64, 128, 192 */
    uint16 egress_packet_copy_size;     /* If non zero and the packet is copied
                                           from the egress, the packet will be
                                           truncated to the first
                                           packet_copy_size . Current supported
                                           values for DNX are 0, 256. */
    bcm_mirror_pkt_header_updates_t packet_control_updates; 
    bcm_mirror_psc_t rtag;              /* specify RTAG HASH algorithm used for
                                           shared-id mirror destination */
    uint8 df;                           /* Set the do not fragment bit of IP
                                           header in mirror encapsulation */
    uint8 truncate;                     /* Setting truncate would result in
                                           mirroring a truncated frame */
    uint16 template_id;                 /* Template ID for IPFIX HDR */
    uint32 observation_domain;          /* Observation domain for IPFIX HDR */
    uint32 is_recycle_strict_priority;  /* Recycle priority (1-lossless, 0-high) */
    int ext_stat_id[BCM_MIRROR_EXT_STAT_ID_COUNT]; /* ext_stat_id to support statistic
                                           interface for mirror packets. */
    uint32 flags2;                      /* See BCM_MIRROR_DEST_FLAGS2_xxx flag
                                           definitions. */
    uint32 vni;                         /* Virtual Network Interface ID. */
    uint32 gre_seq_number;              /* Sequence number value used in GRE
                                           header. If no value is provided,
                                           gre_seq_number will start with 0.
                                           Valid only if
                                           BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    bcm_mirror_pkt_erspan_encap_t erspan_header; /* ERSPAN encapsulation header fields.
                                           Valid only if only
                                           BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    uint32 initial_seq_number;          /* Starting seq number in SFLOW or PSAMP
                                           header */
    bcm_mirror_psamp_fmt2_meta_data_t meta_data_type; /* Type of item in 'meta_data' */
    uint16 meta_data;                   /* Actual value of class id or meta
                                           data, based on 'meta_data_type' */
    int ext_stat_valid;                 /* Validates the ext_stat. */
    int ext_stat_type_id[BCM_MIRROR_EXT_STAT_ID_COUNT]; /* Type ids for statistic interface. */
    uint16 ipfix_version;               /* IPFIX version number in PSAMP
                                           encapsulation. */
    uint16 psamp_epoch;                 /* PSAMP epoch field. */
    int cosq;                           /* Queue id that mirror copy is enqueued
                                           into. */
    uint8 cfi;                          /* CFI of VLAN tag. */
    bcm_mirror_timestamp_mode_t timestamp_mode; /* Set timestamp mode for mirror
                                           encapsulation. */
    bcm_mirror_multi_dip_group_t multi_dip_group; /* Multiple destination IP group. */
    int drop_group_bmp; 
    bcm_port_t second_pass_src_port;    /* Source port in the second pass. */
    bcm_mirror_truncate_mode_t encap_truncate_mode; /* Truncate mode for mirror
                                           encapsulation packets. */
    int encap_truncate_profile_id;      /* Truncate length profile ID for mirror
                                           encapsulation packets. */
    bcm_mirror_loopback_header_type_t loopback_header_type; /* Loopback header type. */
    bcm_port_t second_pass_dst_port;    /* Destination port in the second pass. */
    bcm_mirror_int_probe_header_t int_probe_header; /* INT(Inband Network Telemetry) probe
                                           header for mirror encapsulation
                                           packets. */
    uint8 duplicate_pri;                /* The priority of duplicate mirror
                                           packets. The packet with highest
                                           priority (0 is lowest) will be
                                           mirrored and others will be dropped.
                                           Applicable only if duplication is not
                                           allowed. */
    int ip_proto;                       /* IP Protocol number in ERSPAN IP
                                           encap. */
} bcm_mirror_destination_t;

/* Mirror destination callback function prototype. */
typedef int (*bcm_mirror_destination_traverse_cb)(
    int unit, 
    bcm_mirror_destination_t *mirror_dest, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the mirroring module to its initial configuration. */
extern int bcm_mirror_init(
    int unit);

/* Set the current mirroring mode for the device. */
extern int bcm_mirror_mode_set(
    int unit, 
    int mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * extended versions of existing _dest_ APIs using a bcm_mirror_options_t
 * argument
 */
extern void bcm_mirror_options_t_init(
    bcm_mirror_options_t *mirror_dest);

/* Initialized bcm_mirror_pkt_header_updates_t struct */
extern void bcm_mirror_pkt_header_updates_t_init(
    bcm_mirror_pkt_header_updates_t *mirror_dest);

/* Initialize a bcm_mirror_int_probe_header_t struct */
extern void bcm_mirror_int_probe_header_t_init(
    bcm_mirror_int_probe_header_t *header);

/* Initialize a mirror destination structure. */
extern void bcm_mirror_destination_t_init(
    bcm_mirror_destination_t *mirror_dest);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add mirror destination descriptor. */
extern int bcm_mirror_destination_create(
    int unit, 
    bcm_mirror_destination_t *mirror_dest);

/* Read content of mirror destination descriptor */
extern int bcm_mirror_destination_get(
    int unit, 
    bcm_gport_t mirror_dest_id, 
    bcm_mirror_destination_t *mirror_dest);

/* Remove mirror destination descriptor. */
extern int bcm_mirror_destination_destroy(
    int unit, 
    bcm_gport_t mirror_dest_id);

/* Traverse over all installed mirror destination descriptors. */
extern int bcm_mirror_destination_traverse(
    int unit, 
    bcm_mirror_destination_traverse_cb cb, 
    void *user_data);

/* Get the current mirroring mode for the device. */
extern int bcm_mirror_mode_get(
    int unit, 
    int *mode);

/* Set the mirror-to port for the device. */
extern int bcm_mirror_to_set(
    int unit, 
    bcm_port_t port);

/* Get the mirror-to port for the device. */
extern int bcm_mirror_to_get(
    int unit, 
    bcm_port_t *port);

/* Enable or Disable ingress mirroring for a port on the device. */
extern int bcm_mirror_ingress_set(
    int unit, 
    bcm_port_t port, 
    int val);

/* 
 * Get the ingress mirroring enabled/disabled status for a port on the
 * device.
 */
extern int bcm_mirror_ingress_get(
    int unit, 
    bcm_port_t port, 
    int *val);

/* Enable or Disable egress mirroring for a port on the device. */
extern int bcm_mirror_egress_set(
    int unit, 
    bcm_port_t port, 
    int val);

/* 
 * Get the egress mirroring enabled/disabled status for a port on the
 * device.
 */
extern int bcm_mirror_egress_get(
    int unit, 
    bcm_port_t port, 
    int *val);

/* Set the mirror-to port bitmap for the device. */
extern int bcm_mirror_to_pbmp_set(
    int unit, 
    bcm_port_t port, 
    bcm_pbmp_t pbmp);

/* Get the mirror-to port bitmap for the device. */
extern int bcm_mirror_to_pbmp_get(
    int unit, 
    bcm_port_t port, 
    bcm_pbmp_t *pbmp);

/* Set mirroring configuration for a port. */
extern int bcm_mirror_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_module_t dest_mod, 
    bcm_port_t dest_port, 
    uint32 flags);

/* Get mirroring configuration for a port. */
extern int bcm_mirror_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_module_t *dest_mod, 
    bcm_port_t *dest_port, 
    uint32 *flags);

/* Set VLAN for egressing mirrored packets on a port (RSPAN). */
extern int bcm_mirror_vlan_set(
    int unit, 
    bcm_port_t port, 
    uint16 tpid, 
    uint16 vlan);

/* Get VLAN for egressing mirrored packets on a port (RSPAN). */
extern int bcm_mirror_vlan_get(
    int unit, 
    bcm_port_t port, 
    uint16 *tpid, 
    uint16 *vlan);

/* Add mirroring destination to a port */
extern int bcm_mirror_port_dest_add(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_gport_t mirror_dest_id);

/* 
 * extended versions of existing _dest_ APIs using a bcm_mirror_options_t
 * argument
 */
extern int bcm_mirror_port_destination_add(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_gport_t mirror_dest, 
    bcm_mirror_options_t options);

/* Delete mirroring destination from a port */
extern int bcm_mirror_port_dest_delete(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_gport_t mirror_dest_id);

/* Delete all mirroring destinations from a port */
extern int bcm_mirror_port_dest_delete_all(
    int unit, 
    bcm_port_t port, 
    uint32 flags);

/* Get port mirroring destinations */
extern int bcm_mirror_port_dest_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int mirror_dest_size, 
    bcm_gport_t *mirror_dest, 
    int *mirror_dest_count);

/* 
 * extended versions of existing _dest_ APIs using a bcm_mirror_options_t
 * argument
 */
extern int bcm_mirror_port_destination_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    int mirror_dest_size, 
    bcm_gport_t *mirror_dest, 
    int *mirror_dest_count, 
    bcm_mirror_options_t *options);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Backward compatibility. */
#define bcm_mirror_mode         bcm_mirror_mode_set 

#ifndef BCM_HIDE_DISPATCHABLE

/* Set mirroring configuration for a port-vlan. */
extern int bcm_mirror_port_vlan_set(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    bcm_gport_t destport, 
    uint32 flags);

/* Get mirroring configuration for a port-vlan */
extern int bcm_mirror_port_vlan_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    bcm_gport_t *dest_port, 
    uint32 *flags);

/* Add mirroring destination to a port-vlan */
extern int bcm_mirror_port_vlan_dest_add(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    uint32 flags, 
    bcm_gport_t destid);

/* 
 * extended versions of existing _dest_ APIs using a bcm_mirror_options_t
 * argument
 */
extern int bcm_mirror_port_vlan_destination_add(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    uint32 flags, 
    bcm_gport_t destid, 
    bcm_mirror_options_t options);

/* Delete mirroring destination from a port-vlan */
extern int bcm_mirror_port_vlan_dest_delete(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    uint32 flags, 
    bcm_gport_t destid);

/* Delete all mirroring destinations from a port-vlan. */
extern int bcm_mirror_port_vlan_dest_delete_all(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    uint32 flags);

/* Get port-vlan mirroring destinations */
extern int bcm_mirror_port_vlan_dest_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    uint32 flags, 
    uint32 mirror_dest_size, 
    bcm_gport_t *destid, 
    uint32 *destcount);

/* 
 * extended versions of existing _dest_ APIs using a bcm_mirror_options_t
 * argument
 */
extern int bcm_mirror_port_vlan_destination_get(
    int unit, 
    bcm_port_t port, 
    bcm_vlan_t vlan, 
    uint32 flags, 
    uint32 mirror_dest_size, 
    bcm_gport_t *destid, 
    uint32 *destcount, 
    bcm_mirror_options_t *options);

/* Set values of one or more payload zeroing parameters. */
extern int bcm_mirror_payload_zero_control_multi_set(
    int unit, 
    bcm_mirror_payload_zero_info_t *payload_zero_config_array, 
    int num_configs);

/* Get values of one or more payload zeroing parameters. */
extern int bcm_mirror_payload_zero_control_multi_get(
    int unit, 
    bcm_mirror_payload_zero_info_t *payload_zero_config_array, 
    int num_configs);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a payload zero information structure. */
extern void bcm_mirror_payload_zero_info_t_init(
    bcm_mirror_payload_zero_info_t *info);

/* 
 * Mirror port Structure
 * 
 * Contains information required for mirror port settings.
 */
typedef struct bcm_mirror_port_info_s {
    bcm_gport_t mirror_system_id;   /* Unique mirror System identifier. */
} bcm_mirror_port_info_t;

/* Initialize a mirror port information structure. */
extern void bcm_mirror_port_info_t_init(
    bcm_mirror_port_info_t *info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set mirroring information per port. */
extern int bcm_mirror_port_info_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_mirror_port_info_t *info);

/* Get mirroring information per port. */
extern int bcm_mirror_port_info_get(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    bcm_mirror_port_info_t *info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Invalid mirror/snoop profile value, used in bcm_mirror_port_dest_get
 * and bcm_mirror_destination_get
 */
#define BCM_MIRROR_INVALID_PROFILE  0xffff     /* Invalid mirror/snoop profile
                                                  value */

#define BCM_MIRROR_DROP_GROUP_BMP_ALL   -1         /* Map all mirror on drop
                                                      profile to the same
                                                      profile. */

/* 
 * Value to disable counting of the snooped/mirrored copy, used in
 * bcm_mirror_destination_create when flags
 * BCM_MIRROR_DEST_UPDATE_COUNTER_1 and BCM_MIRROR_DEST_UPDATE_COUNTER_2
 * are given.
 */
#define BCM_MIRROR_DEST_NO_COUNT    -2         /* Value to disable counting of
                                                  the snooped/mirrored copy. */

/* 
 * Default value to be used for Template id for the IPFIX header in case
 * of PSAMP.
 */
#define BCM_MIRROR_IPFIX_TEMPLATE_ID_DFLT   0x1234     /* Default value to be
                                                          used for Template id. */

/* 
 * Default value to be used for Template id for the IPFIX header in case
 * of PSAMP.
 */
#define BCM_MIRROR_IPFIX_OBSERVATION_DOMAIN_DFLT 100        /* Default value to be
                                                          used for observation
                                                          domain. */

/* Info about mirror port and priorities */
typedef struct bcm_mirror_port_to_rcy_map_info_s {
    bcm_port_t rcy_mirror_port; /* Mirror port */
    uint32 priority_bitmap;     /* Used to specify which priorities to map.
                                   Possible values will be named
                                   BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_ */
} bcm_mirror_port_to_rcy_map_info_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Map/unmap forward port to recycle mirror port (to unmap set
 * rcy_mirror_port to BCM_MIRROR_INVALID_RCY_PORT)
 */
extern int bcm_mirror_port_to_rcy_port_map_set(
    int unit, 
    uint32 flags, 
    bcm_port_t forward_port, 
    bcm_mirror_port_to_rcy_map_info_t *rcy_map_info);

/* Get recycle mirror port from forward port. */
extern int bcm_mirror_port_to_rcy_port_map_get(
    int unit, 
    uint32 flags, 
    bcm_port_t forward_port, 
    bcm_mirror_port_to_rcy_map_info_t *rcy_map_info);

#endif /* BCM_HIDE_DISPATCHABLE */

#define BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL 0x00000001 /* Assign the port to
                                                          normal priority
                                                          recycle context. */
#define BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH 0x00000002 /* Assign the port to
                                                          high priority recycle
                                                          context. */

/* Mirror profile counting parameters for command. */
typedef struct bcm_mirror_profile_cmd_count_s {
    int is_counted;     /* Is the mirror packet should be counted for this
                           command. */
    int object_stat_id; /* Statistic id to be counted for this command. */
    int type_id;        /* Type id to be counted for this command. */
    int is_meter;       /* Object is meter or crps. */
} bcm_mirror_profile_cmd_count_t;

#define BCM_MIRROR_COUNT_AS_ORIG    -1         

#define BCM_MIRROR_PORT_DELETE_ALL  0x00000400 /* Delete all destinations */

#ifndef BCM_HIDE_DISPATCHABLE

extern int bcm_mirror_destination_count_command_get(
    int unit, 
    bcm_core_t core_id, 
    bcm_gport_t mirror_dest_id, 
    int counter_cmd_id, 
    uint32 flags, 
    bcm_mirror_profile_cmd_count_t *mirror_count_info);

extern int bcm_mirror_destination_count_command_set(
    int unit, 
    bcm_core_t core_id, 
    bcm_gport_t mirror_dest_id, 
    int counter_cmd_id, 
    uint32 flags, 
    bcm_mirror_profile_cmd_count_t *mirror_count_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* TM header information for mirrored packet in DNX devices */
typedef struct bcm_mirror_pkt_dnx_ftmh_header_s {
    bcm_gport_t src_sysport;            /* Source System port */
    uint8 tc;                           /* Traffic class */
    uint8 dp;                           /* Drop precedence */
    uint8 is_mc_traffic;                /* Destination is multicast */
    bcm_multicast_t mc_id;              /* Destination Multicast ID (when
                                           is_mc_traffic is set) */
    bcm_gport_t out_vport;              /* Destination Outlif (when
                                           is_mc_traffic is cleared) */
    bcm_pkt_dnx_ftmh_ase_extension_t ase_ext; /* FTMH Application Specific Extension */
    bcm_pkt_dnx_ftmh_flow_id_extension_t flow_id_ext; /* FTMH Flow-ID Extension */
} bcm_mirror_pkt_dnx_ftmh_header_t;

/* PP header information for mirrored packet in DNX devices */
typedef struct bcm_mirror_pkt_dnx_pp_header_s {
    uint8 tail_edit_profile;        /* Tail edit profile, used at ETPP for tail
                                       editing */
    uint8 eth_header_remove;        /* Indication to remove Ethernet header from
                                       mirror copy */
    bcm_gport_t out_vport_ext[3];   /* Extended outlif(s) */
} bcm_mirror_pkt_dnx_pp_header_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm; /* TM header */
    bcm_mirror_pkt_dnx_pp_header_t pp;  /* PP header */
    bcm_pkt_dnx_udh_t udh[4];           /* UDH header */
} bcm_mirror_header_info_t;

/* Initialize a bcm_mirror_header_info_t structure. */
extern void bcm_mirror_header_info_t_init(
    bcm_mirror_header_info_t *mirror_header_info);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set system header information */
extern int bcm_mirror_header_info_set(
    int unit, 
    uint32 flags, 
    bcm_gport_t mirror_dest_id, 
    bcm_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_mirror_header_info_get(
    int unit, 
    bcm_gport_t mirror_dest_id, 
    uint32 *flags, 
    bcm_mirror_header_info_t *mirror_header_info);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Denotes that profile is used by Field Processor Module */
#define BCM_MIRROR_PAYLOAD_ZERO_FP  (1 << 0)   

/* Mirror Payload Zeroing Profile Information */
typedef struct bcm_mirror_payload_zero_offsets_s {
    int l2_offset;          /* offset from the end of the known L2 headers to
                               start zeroing */
    int l3_offset;          /* offset from the end of the known L3 headers to
                               start zeroing for non-UDP packets */
    int udp_offset;         /* offset from the end of the known L3 headers to
                               start zeroing for UDP packets */
    int l2_offset_disable;  /* disable zeroing the truncated mirrored packets
                               from the end of the known L2 header */
    int l3_offset_disable;  /* disable zeroing the truncated mirrored non-UDP
                               packets from the end of the known L3 header */
    int udp_offset_disable; /* disable zeroing the truncated mirrored UDP
                               packets from the end of the known L3 header */
} bcm_mirror_payload_zero_offsets_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Creates a profile for Payload Zeroing. */
extern int bcm_mirror_payload_zero_profile_create(
    int unit, 
    bcm_mirror_payload_zero_offsets_t *mirror_payload_offset_info, 
    uint32 flags, 
    uint32 *profile_index);

/* 
 * Deletes a created profile for Payload Zeroing.
 * Please make sure that all related configurations
 * in other dependent modules like Field
 * need to be cleaned up before calling the destroy.
 * Otherwise the related configs will become stale/improper
 * entries in the dependent modules.
 */
extern int bcm_mirror_payload_zero_profile_destroy(
    int unit, 
    uint32 flags, 
    uint32 profile_index);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Initialize a bcm_mirror_payload_zero_offsets_t structure. */
extern void bcm_mirror_payload_zero_offsets_t_init(
    bcm_mirror_payload_zero_offsets_t *mirror_payload_zero_offset);

/* Sample type. */
typedef enum bcm_mirror_sample_type_e {
    bcmMirrorSampleTypeIngress = 0, /* Ingress sample. */
    bcmMirrorSampleTypeFlex = 1,    /* Flex sample. */
    bcmMirrorSampleTypeCount = 2    /* The last one, not valid. */
} bcm_mirror_sample_type_t;

/* Sample mirror mode. */
typedef enum bcm_mirror_sample_mirror_mode_e {
    bcmMirrorSampleMirrorModeNone = 0,  /* No mirror. */
    bcmMirrorSampleMirrorModeBelowRate = 1, /* Mirror packets below sample rate. */
    bcmMirrorSampleMirrorModeAboveRate = 2, /* Mirror packets above sample rate. */
    bcmMirrorSampleMirrorModeCount = 3  /* The last one, not valid. */
} bcm_mirror_sample_mirror_mode_t;

/* Sample profile structure. */
typedef struct bcm_mirror_sample_profile_s {
    uint8 enable;                       /* Sample enable. */
    uint32 rate;                        /* Sample rate. */
    uint8 pool_count_enable;            /* Enable to count the packets seen by
                                           the sampler. */
    uint8 sample_count_enable;          /* Enable to count the packets sampled
                                           by the sampler. */
    uint8 trace_enable;                 /* Enable to generate trace event. */
    bcm_mirror_sample_mirror_mode_t mirror_mode; /* Sample mirror mode. */
} bcm_mirror_sample_profile_t;

/* Initialize a sample profile structure. */
extern void bcm_mirror_sample_profile_t_init(
    bcm_mirror_sample_profile_t *profile);

/* Sample profile option flags. */
#define BCM_MIRROR_SAMPLE_PROFILE_OPTIONS_WITH_ID 0x00000001 /* Create sample profile
                                                          with id. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a sample profile. */
extern int bcm_mirror_sample_profile_create(
    int unit, 
    uint32 options, 
    bcm_mirror_sample_type_t type, 
    int *profile_id);

/* Destroy a sample profile. */
extern int bcm_mirror_sample_profile_destroy(
    int unit, 
    bcm_mirror_sample_type_t type, 
    int profile_id);

/* Set sample profile entry with given profile id and type. */
extern int bcm_mirror_sample_profile_set(
    int unit, 
    bcm_mirror_sample_type_t type, 
    int profile_id, 
    bcm_mirror_sample_profile_t *profile);

/* Get sample profile entry with given profile id and type. */
extern int bcm_mirror_sample_profile_get(
    int unit, 
    bcm_mirror_sample_type_t type, 
    int profile_id, 
    bcm_mirror_sample_profile_t *profile);

/* Attach counter entries to a sample type. */
extern int bcm_mirror_sample_stat_attach(
    int unit, 
    bcm_mirror_sample_type_t type, 
    uint32 stat_counter_id);

/* Detach counter entries from a sample type. */
extern int bcm_mirror_sample_stat_detach(
    int unit, 
    bcm_mirror_sample_type_t type, 
    uint32 stat_counter_id);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Mirror source type enum. */
typedef enum bcm_mirror_source_type_e {
    bcmMirrorSourceTypePort = 0,        /* Port/gport type. */
    bcmMirrorSourceTypePktTraceEvent = 1, /* Packet trace event type. */
    bcmMirrorSourceTypePktDropEvent = 2, /* Packet drop event type. */
    bcmMirrorSourceTypeCosqMod = 3,     /* MMU mirror-on-drop type. */
    bcmMirrorSourceTypeSampleIngress = 4, /* Ingress sample type. */
    bcmMirrorSourceTypeSampleFlex = 5,  /* Flex sample type. */
    bcmMirrorSourceTypeIngressMod = 6,  /* Ingress mirror-on-drop type. */
    bcmMirrorSourceTypeEpRecirc = 7,    /* EP Recirculate type. */
    bcmMirrorSourceTypeCount = 8        /* Must Be Last. Not a valid value. */
} bcm_mirror_source_type_t;

/* Mirror source flags. */
#define BCM_MIRROR_SOURCE_INGRESS   0x00000001 /* Ingress Mirroring. */
#define BCM_MIRROR_SOURCE_EGRESS    0x00000002 /* Egress Mirroring. */

/* Mirror source structure. */
typedef struct bcm_mirror_source_s {
    bcm_mirror_source_type_t type;      /* Mirror source type. */
    uint32 flags;                       /* Mirror source flags only with port
                                           source type. */
    bcm_port_t port;                    /* Port/gport id. */
    bcm_pkt_trace_event_t trace_event;  /* Trace event. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    int mod_profile_id;                 /* Mirror-on-drop profile id. */
    uint32 sample_profile_id;           /* Sample profile id. */
    int ep_recirc_profile_id;           /* EP Recirculate profile id. */
} bcm_mirror_source_t;

/* Initialize mirror source structure. */
extern void bcm_mirror_source_t_init(
    bcm_mirror_source_t *source);

#ifndef BCM_HIDE_DISPATCHABLE

/* Add a mirror destination to a mirror source. */
extern int bcm_mirror_source_dest_add(
    int unit, 
    bcm_mirror_source_t *source, 
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from a mirror source. */
extern int bcm_mirror_source_dest_delete(
    int unit, 
    bcm_mirror_source_t *source, 
    bcm_gport_t mirror_dest_id);

/* Delete all mirror destinations applied on a mirror source. */
extern int bcm_mirror_source_dest_delete_all(
    int unit, 
    bcm_mirror_source_t *source);

/* Get all mirror destinations applied on a mirror source. */
extern int bcm_mirror_source_dest_get_all(
    int unit, 
    bcm_mirror_source_t *source, 
    int array_size, 
    bcm_gport_t *mirror_dest, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Mirror statistics object structure. */
typedef struct bcm_mirror_stat_object_s {
    uint32 source_flags;        /* Mirror source flags. */
    bcm_gport_t mirror_dest_id; /* Mirror destination id. */
} bcm_mirror_stat_object_t;

/* Initialize mirror statistics object structure. */
extern void bcm_mirror_stat_object_t_init(
    bcm_mirror_stat_object_t *object);

/* Mirror statistics type enum. */
typedef enum bcm_mirror_stat_e {
    bcmMirrorStatPackets = 0,   /* Packet statistics. */
    bcmMirrorStatCount = 1      /* Max value defined for this enum. */
} bcm_mirror_stat_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Get statistics counter for specific mirror statistics object and type. */
extern int bcm_mirror_stat_get(
    int unit, 
    bcm_mirror_stat_object_t *object, 
    bcm_mirror_stat_t type, 
    uint64 *value);

/* Get statistics counter for specific mirror statistics object and type. */
extern int bcm_mirror_stat_sync_get(
    int unit, 
    bcm_mirror_stat_object_t *object, 
    bcm_mirror_stat_t type, 
    uint64 *value);

/* Set statistics counter for specific mirror statistics object and type. */
extern int bcm_mirror_stat_set(
    int unit, 
    bcm_mirror_stat_object_t *object, 
    bcm_mirror_stat_t type, 
    uint64 value);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Ingress mirror-on-drop event type enum. */
typedef enum bcm_mirror_ingress_mod_event_e {
    bcmMirrorIngModEventInvalidVlan = 0, /* Enable to select packets dropped due
                                           to invalid VLAN. */
    bcmMirrorIngModEventTagUntagDiscard = 1, /* Enable to count packets dropped due
                                           to port configuration for dropping
                                           all tagged packets or untagged
                                           packets. */
    bcmMirrorIngModEventInvalidTpid = 2, /* Enable to select packets dropped due
                                           to TPID mismatch. */
    bcmMirrorIngModEventPvlanVidMismatch = 3, /* Enable to select packets dropped due
                                           to PVLAN VID mismatch. */
    bcmMirrorIngModEventPortIngVlanMembership = 4, /* Enable to select packets dropped due
                                           to ingress port not in VLAN
                                           membership */
    bcmMirrorIngModEventVlanCcOrPbt = 5, /* Enable to select VLAN cross-connect
                                           drop. */
    bcmMirrorIngModEventVlanFp = 6,     /* Enable to select packets dropped due
                                           to VLAN FP. */
    bcmMirrorIngModEventMcIndexErr = 7, /* Enable to select packets with
                                           multicast index error. */
    bcmMirrorIngModEventMplsTtlCheckFail = 8, /* Enable to select packets dropped due
                                           to MPLS TTL check fail. */
    bcmMirrorIngModEventMplsLabelMiss = 9, /* Enable to select packets dropped due
                                           to MPLS label lookup miss. */
    bcmMirrorIngModEventMplsInvalidPayload = 10, /* Enable to select packets dropped due
                                           to MPLS invalid payload. */
    bcmMirrorIngModEventMplsInvalidCw = 11, /* Enable to select packets dropped due
                                           to MPLS invalid control word. */
    bcmMirrorIngModEventMplsInvalidAction = 12, /* Enable to select packets dropped due
                                           to MPLS invalid action. */
    bcmMirrorIngModEventMplsGalLabel = 13, /* Enable to select packets dropped due
                                           to MPLS generic associated label
                                           lookup miss. */
    bcmMirrorIngModEventTnlDecapEcnDrop = 14, /* Enable to select tunnel decap ECN
                                           error drop. */
    bcmMirrorIngModEventTimeSyncDrop = 15, /* Enable to select time sync drop. */
    bcmMirrorIngModEventSrcRoute = 16,  /* Enable to select packets dropped due
                                           to source route. */
    bcmMirrorIngModEventSpanningTreeState = 17, /* Enable to select packets dropped due
                                           to spanning tree. */
    bcmMirrorIngModEventTnlErr = 18,    /* Enable to select packets dropped due
                                           to tunnel errors. */
    bcmMirrorIngModEventProtectionDataDrop = 19, /* Enable to select packets dropped due
                                           to protection. */
    bcmMirrorIngModEventNhopDrop = 20,  /* Enable to select packets dropped due
                                           to next hop indication. */
    bcmMirrorIngModEventFpIngDrop = 21, /* Enable to select packets dropped due
                                           to ingress FP drop. */
    bcmMirrorIngModEventFpRedirectDrop = 22, /* Enable to select packets dropped due
                                           to invalid FP redirection. */
    bcmMirrorIngModEventEcmpResolutionErr = 23, /* Enable to select packets dropped due
                                           to ECMP resolution error. */
    bcmMirrorIngModEventProtcolPktCtrlDrop = 24, /* Enable to select packets dropped due
                                           to protocol packet drop control. */
    bcmMirrorIngModEventPortFilteringMode = 25, /* Enable to select port filtering mode
                                           drop. */
    bcmMirrorIngModEventL2MyStation = 26, /* Enable to select packets dropped due
                                           to L2 my station lookup miss. */
    bcmMirrorIngModEventMplsStage = 27, /* Enable to select MPLS stage drop. */
    bcmMirrorIngModEventSrcMacZero = 28, /* Enable to select source MAC address
                                           is zero. */
    bcmMirrorIngModEventL3TtlErr = 29,  /* Enable to select L3 TTL error drop. */
    bcmMirrorIngModEventL3HdrErr = 30,  /* Enable to select L3 header error
                                           drop. */
    bcmMirrorIngModEventDstL3LookupMiss = 31, /* Enable to select L3 destination
                                           lookup miss drop. */
    bcmMirrorIngModEventDstL3Discard = 32, /* Enable to select L3 destination
                                           discard drop. */
    bcmMirrorIngModEventSrcL2StaticMove = 33, /* Enable to select L2 source static
                                           move drop. */
    bcmMirrorIngModEventSrcL2Discard = 34, /* Enable to select packets dropped due
                                           to L2 source discard. */
    bcmMirrorIngModEventDstL2Discard = 35, /* Enable to select packets dropped due
                                           to L2 destination discard. */
    bcmMirrorIngModEventIpMcDrop = 36,  /* Enable to select IP multicast header
                                           error drop. */
    bcmMirrorIngModEventSrcMacEqualsDstMac = 37, /* Enable to select packets dropped due
                                           to source MAC address equals to
                                           destination MAC address. */
    bcmMirrorIngModEventTrunkFailLoopback = 38, /* Enable to select packets looped back
                                           because of trunk failover in packet
                                           discard stage. */
    bcmMirrorIngModEventDosL4Attack = 39, /* Enable to select packets dropped due
                                           to L4 DoS attack. */
    bcmMirrorIngModEventDosL3Attack = 40, /* Enable to select packets dropped due
                                           to L3 DoS attack. */
    bcmMirrorIngModEventIpv6ProtocolErr = 41, /* Enable to select packets dropped due
                                           to IPv6 protocol error. */
    bcmMirrorIngModEventIpv4ProtocolErr = 42, /* Enable to select packets dropped due
                                           to IPv4 protocol error. */
    bcmMirrorIngModEventMacControlFrame = 43, /* Enable to select packets dropped due
                                           to MAC control frame. */
    bcmMirrorIngModEventCompositeError = 44, /* Enable to select packets dropped due
                                           to parity/ECC errors. */
    bcmMirrorIngModEventCpuManagedLearning = 45, /* Enable to select packets dropped due
                                           to CPU managed learning (CML). */
    bcmMirrorIngModEventCfiOrL3Disable = 46, /* Enable to select packets dropped due
                                           to CFI or L3 disable. */
    bcmMirrorIngModEventBpdu = 47,      /* Enable to select packets dropped due
                                           to BPDU. */
    bcmMirrorIngModEventBfdTerminated = 48, /* Enable to select BFD terminated drop. */
    bcmMirrorIngModEventBfdUnknownVerOrDiscard = 49, /* Enable to select counter update for
                                           unknown BFD version or discard
                                           dropped. */
    bcmMirrorIngModEventBfdUnknownAchErr = 50, /* Enable to select counter update for
                                           BFD packets with non-zero version in
                                           the ACH header or the ACH channel
                                           type unknown. */
    bcmMirrorIngModEventBfdUnknownCtrlPkt = 51, /* Enable to select counter update for
                                           unrecognized BFD control packets
                                           received from the PW VCCM type 1/2/3
                                           or MPLS-TP control channel. */
    bcmMirrorIngModEventDosL2 = 52,     /* Enable to select counter update for
                                           L2 DoS attack packets. */
    bcmMirrorIngModEventParityErrDrop = 53, /* Enable to select counter update for
                                           packets dropped due to parity error. */
    bcmMirrorIngModEventTrunkFailoverLoopbackDiscard = 54, /* Enable to select counter update for
                                           trunk failover loopback packets
                                           discarded due to the backup port
                                           being down in packet pre-processing
                                           stage. */
    bcmMirrorIngModEventTrunkFailoverLoopback = 55, /* Enable to select counter update for
                                           trunk failover loopback packets in
                                           packet pre-processing stage. */
    bcmMirrorIngModEventDosFragment = 56, /* Enable to select counter update for
                                           DoS fragment error packets. */
    bcmMirrorIngModEventDosIcmp = 57,   /* Enable to select counter update for
                                           DoS ICMP error packets. */
    bcmMirrorIngModEventRxTnlErr = 58,  /* Enable to select counter update for
                                           received tunnel error packets. */
    bcmMirrorIngModEventRxTnl = 59,     /* Enable to select counter update for
                                           the number of tunnel packets
                                           received. */
    bcmMirrorIngModEventRxDrop = 60,    /* Enable to select counter update for
                                           zero port bitmap drop condition. */
    bcmMirrorIngModEventRxMcDrop = 61,  /* Enable to select counter update for
                                           multicast (L2+L3) packets that are
                                           dropped. */
    bcmMirrorIngModEventMcBridged = 62, /* Enable to select counter update for
                                           bridged multicast packets. */
    bcmMirrorIngModEventRxPortDrop = 63, /* Enable to select counter update for
                                           packets dropped when the ingress port
                                           is not in a forwarding state. */
    bcmMirrorIngModEventRxUc = 64,      /* Enable to select counter update for
                                           good received unicast (L2+L3)
                                           packets. */
    bcmMirrorIngModEventIpv6McRouted = 65, /* Enable to select counter update for
                                           routed IPv6 multicast packets. */
    bcmMirrorIngModEventRxIpv6HdrErr = 66, /* Enable to select counter update for
                                           received IPv6 header error packets
                                           (IPv6 martian error addresses + link
                                           local). */
    bcmMirrorIngModEventRxIpv6 = 67,    /* Enable to select counter update for
                                           good received IPv6 L3 packets,
                                           including tunneled packets. */
    bcmMirrorIngModEventRxIpv6Discard = 68, /* Enable to select counter update for
                                           received L3 IPv6 discarded packets. */
    bcmMirrorIngModEventIpv4McRouted = 69, /* Enable to select counter update for
                                           routed IPv4 multicast packets. */
    bcmMirrorIngModEventRxIpv4HdrErr = 70, /* Enable to select counter update for
                                           received IPv4 header error packets
                                           (martian error address + unicast with
                                           Class D + multicast & ~ClassD) */
    bcmMirrorIngModEventRxIpv4 = 71,    /* Enable to select counter update for
                                           good received IPv4 L3, including
                                           tunneled packets. */
    bcmMirrorIngModEventRxIpv4Discard = 72, /* Enable to select counter update for
                                           received L3 IPv4 discarded packets. */
    bcmMirrorIngModEventVlanMembershipDrop = 73, /* Enable to select counter update for
                                           packets dropped due to mismatched
                                           egress port for the VLAN. */
    bcmMirrorIngModEventVlanBlockedDrop = 74, /* Enable to select counter update for
                                           packets dropped due to VLAN blocked
                                           ports. */
    bcmMirrorIngModEventVlanStgDrop = 75, /* Enable to select counter update for
                                           packets dropped due to spanning tree
                                           group state. */
    bcmMirrorIngModEventNonucTrunkResolutionMask = 76, /* Enable to select counter update for
                                           packets dropped due to multicast and
                                           broadcast trunk block mask. */
    bcmMirrorIngModEventNonucMask = 77, /* Enable to select counter update for
                                           packets dropped due to unknown
                                           unicast, unknown multicast, known
                                           multicast, and broadcast block masks. */
    bcmMirrorIngModEventMcDrop = 78,    /* Enable to select counter update for
                                           packets dropped due to multicast
                                           errors. */
    bcmMirrorIngModEventMacMask = 79,   /* Enable to select counter update for
                                           packets dropped due to MAC block
                                           mask. */
    bcmMirrorIngModEventIngEgrMask = 80, /* Enable to select counter update for
                                           packets dropped due to block traffic
                                           from egressing based on the ingress
                                           port. */
    bcmMirrorIngModEventFpIngDelayedDrop = 81, /* Enable to select counter update for
                                           packets dropped due to ingress FP
                                           delayed action. */
    bcmMirrorIngModEventFpRedirectMask = 82, /* Enable to select counter update for
                                           packets dropped due to FP redirection
                                           mask. */
    bcmMirrorIngModEventEgrMask = 83,   /* Enable to select counter update for
                                           packets dropped due to egress mask. */
    bcmMirrorIngModEventSrcPortKnockoutDrop = 84, /* Enable to select counter update for
                                           packets dropped due to source port
                                           knockout. */
    bcmMirrorIngModEventNonIpDiscard = 85, /* Enable to select counter update for
                                           non-IP packet discard dropped. */
    bcmMirrorIngModEventMtuCheckFail = 86, /* Enable to select counter update for
                                           packets dropped due to MTU check
                                           fail. */
    bcmMirrorIngModEventBlockMaskDrop = 87, /* Enable to select counter update for
                                           packets dropped due to block masks. */
    bcmMirrorIngModEventBfdError = 88,  /* Enable to select packet copied to CPU
                                           due to BFD error. */
    bcmMirrorIngModEventBfdSlowpath = 89, /* Enable to select packet copied to CPU
                                           due to BFD slow path. */
    bcmMirrorIngModEventTimeSyncToCpu = 90, /* Enable to select network time sync
                                           packet copied to CPU. */
    bcmMirrorIngModEventIeee1588UnknownVersion = 91, /* Enable to select packet copied to CPU
                                           due to IEEE 1588 unknown version. */
    bcmMirrorIngModEventMplsUnknownAchType = 92, /* Enable to select packet copied to CPU
                                           due to MPLS unknown generic
                                           associated channel type or version. */
    bcmMirrorIngModEventMplsIllegalReservedLabel = 93, /* Enable to select packet copied to CPU
                                           due to illegal reserved MPLS label.
                                           Such packets have GAL (Generic
                                           Associated Channel Label) that is not
                                           at the bottom of the stack. */
    bcmMirrorIngModEventMplsAlertLabelExposed = 94, /* Enable to select packet copied to CPU
                                           due to GAL (Generic Associated
                                           channel Label) or RAL (Router Alert
                                           Label) being exposed. */
    bcmMirrorIngModEventMplsTtlCheckFailToCpu = 95, /* Enable to select packet copied to CPU
                                           due to MPLS Header TTL value being 0
                                           or 1. */
    bcmMirrorIngModEventMplsInvalidPayloadToCpu = 96, /* Enable to select packet copied to CPU
                                           due to MPLS payload being invalid. */
    bcmMirrorIngModEventMplsInvalidActionToCpu = 97, /* Enable to select packet copied to CPU
                                           due to MPLS invalid action. */
    bcmMirrorIngModEventMplsLabelMissToCpu = 98, /* Enable to select packet copied to CPU
                                           due to MPLS label lookup miss.
                                           Applicable only for MPLS tunnel
                                           termination. */
    bcmMirrorIngModEventSflowFlexSample = 99, /* Enable to select SFLOW flex sampled
                                           packet copied to CPU. */
    bcmMirrorIngModEventSflowIngSample = 100, /* Enable to select SFLOW ingress
                                           sampled packet copied to CPU. */
    bcmMirrorIngModEventSflowEgrSample = 101, /* Enable to select SFLOW egress sampled
                                           packet copied to CPU. */
    bcmMirrorIngModEventTnlDecapEcnToCpu = 102, /* Enable to select packet copied to CPU
                                           from tunnel decap ECN. */
    bcmMirrorIngModEventTnlErrToCpu = 103, /* Enable to select packet copied to CPU
                                           due to tunnel error. */
    bcmMirrorIngModEventCbsmPrevented = 104, /* Enable to select packet copied to CPU
                                           due to CBSM (Class Based Station
                                           Movement) prevention. */
    bcmMirrorIngModEventDlbMonitor = 105, /* Enable to select packet copied to CPU
                                           by DLB monitor. */
    bcmMirrorIngModEventEtrapMonitor = 106, /* Enable to select packet copied to CPU
                                           by Etrap (elephant trap) monitor. */
    bcmMirrorIngModEventFpIng = 107,    /* Enable to select packet copied to CPU
                                           due to ingress FP action. */
    bcmMirrorIngModEventVfp = 108,      /* Enable to select packet copied to CPU
                                           due to VFP action. */
    bcmMirrorIngModEventMcIndexErrToCpu = 109, /* Enable to select packet copied to CPU
                                           due to multicast index error. */
    bcmMirrorIngModEventIcmpRedirect = 110, /* Enable to select ICMP redirect packet
                                           copied to CPU. */
    bcmMirrorIngModEventNhop = 111,     /* Enable to select packet copied to CPU
                                           from next hop table. */
    bcmMirrorIngModEventMartianAddr = 112, /* Enable to select packet copied to CPU
                                           due to Martian address. */
    bcmMirrorIngModEventL3HdrErrToCpu = 113, /* Enable to select packet copied to CPU
                                           due to L3 header error. */
    bcmMirrorIngModEventIpMcMiss = 114, /* Enable to select packet copied to CPU
                                           due to IPMC miss. */
    bcmMirrorIngModEventIpMcL3IifMismatch = 115, /* Enable to select packet copied to CPU
                                           due to IP multicast interface
                                           mismatch. */
    bcmMirrorIngModEventL3SrcMove = 116, /* Enable to select packet copied to CPU
                                           due to L3 station movement. */
    bcmMirrorIngModEventL3DstMiss = 117, /* Enable to select packet copied to CPU
                                           due to L3 destination IP miss or
                                           broadcast destination IP. */
    bcmMirrorIngModEventL3SrcMiss = 118, /* Enable to select packet copied to CPU
                                           due to L3 source IP miss. */
    bcmMirrorIngModEventMtuCheckFailToCpu = 119, /* Enable to select packet copied to CPU
                                           due to L3 MTU check failure. */
    bcmMirrorIngModEventIpOptionsPkt = 120, /* Enable to select packet copied to CPU
                                           due to an IP packet with options. */
    bcmMirrorIngModEventTtl1 = 121,     /* Enable to select IPMC or L3UC packet
                                           with TTL equal to 1 copied to CPU. */
    bcmMirrorIngModEventPbtNonucPkt = 122, /* Enable to select packet copied to CPU
                                           due to PBT (Provider Backbone
                                           Transport) packet being non unicast. */
    bcmMirrorIngModEventL2 = 123,       /* Enable to select packet copied to CPU
                                           from L2 table. */
    bcmMirrorIngModEventL2Move = 124,   /* Enable to select packet copied to CPU
                                           due to L2 station movement. */
    bcmMirrorIngModEventL2DstLookupFailure = 125, /* Enable to select packet copied to CPU
                                           due to L2 destination lookup failure. */
    bcmMirrorIngModEventL2SrcLookupFailure = 126, /* Enable to select packet copied to CPU
                                           due to L2 source lookup failure. */
    bcmMirrorIngModEventL2MyStationHit = 127, /* Enable to select packet copied to CPU
                                           due to MY_STATION hit. */
    bcmMirrorIngModEventL2McMiss = 128, /* Enable to select packet copied to CPU
                                           due to L2MC miss. */
    bcmMirrorIngModEventParityError = 129, /* Enable to select packet copied to CPU
                                           due to parity error in ingress
                                           pipeline tables. */
    bcmMirrorIngModEventPktTrace = 130, /* Enable to select packet copied to CPU
                                           due to packet trace function. */
    bcmMirrorIngModEventDosAttack = 131, /* Enable to select DOS attack packet
                                           copied to CPU. */
    bcmMirrorIngModEventPvlanIdMismatchToCpu = 132, /* Enable to select Private VLAN ID
                                           mismatch packet copied to CPU. */
    bcmMirrorIngModEventUnknownVlan = 133, /* Enable to select Unknown VLAN packet
                                           copied to CPU. */
    bcmMirrorIngModEventL2Proto = 134,  /* Enable to select L2 protocol packet
                                           copied to CPU. */
    bcmMirrorIngModEventSrpProtocol = 135, /* Enable to select SRP (Stream
                                           Registration Protocol) packet copied
                                           to CPU. */
    bcmMirrorIngModEventMmrpProtocol = 136, /* Enable to select MMRP (Multiple Mac
                                           Registration Protocol) packet copied
                                           to CPU. */
    bcmMirrorIngModEventArpProtocol = 137, /* Enable to select ARP packet copied to
                                           CPU. */
    bcmMirrorIngModEventIgmpProtocol = 138, /* Enable to select IGMP packet copied
                                           to CPU. */
    bcmMirrorIngModEventDhcpProtocol = 139, /* Enable to select DHCP packet copied
                                           to CPU. */
    bcmMirrorIngModEventIpMcRsvdProtocol = 140, /* Enable to select IPMC reserved packet
                                           copied to CPU. */
    bcmMirrorIngModEventBpduProtocol = 141, /* Enable to select BPDU packet copied
                                           to CPU. */
    bcmMirrorIngModEventVfiIngAdaptLookupMiss = 142, /* Enable to select VFI ingress
                                           adaptation first lookup miss drop or
                                           both first and second lookup miss
                                           drop. */
    bcmMirrorIngModEventVfiIngAdaptLookupMissToCpu = 143, /* Enable to select VFI ingress
                                           adaptation first lookup miss or both
                                           first and second lookup miss copied
                                           to CPU. */
    bcmMirrorIngModEventOlUlNhopEifRangeError = 144, /* Enable to select overlay and underlay
                                           next hop or L3 egress interface ID
                                           range error drop. */
    bcmMirrorIngModEventCount = 145     /* Must be last. Not a valid value. */
} bcm_mirror_ingress_mod_event_t;

/* Ingress mirror-on-drop event group. */
typedef int bcm_mirror_ingress_mod_event_group_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an ingress mirror-on-drop event group. */
extern int bcm_mirror_ingress_mod_event_group_create(
    int unit, 
    bcm_mirror_ingress_mod_event_t *event_array, 
    int num_events, 
    bcm_mirror_ingress_mod_event_group_t *group);

/* Destroy an ingress mirror-on-drop event group. */
extern int bcm_mirror_ingress_mod_event_group_destroy(
    int unit, 
    bcm_mirror_ingress_mod_event_group_t group);

/* Get all ingress mirror-on-drop event groups. */
extern int bcm_mirror_ingress_mod_event_group_get_all(
    int unit, 
    bcm_mirror_ingress_mod_event_group_t *group_array, 
    int *count);

/* Get an ingress mirror-on-drop event group. */
extern int bcm_mirror_ingress_mod_event_group_get(
    int unit, 
    bcm_mirror_ingress_mod_event_group_t group, 
    bcm_mirror_ingress_mod_event_t *event_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Ingress mirror-on-drop event profile structure. */
typedef struct bcm_mirror_ingress_mod_event_profile_s {
    uint8 reason_code;  /* Configurable reason code for ingress mirror-on-drop
                           packets. */
    uint32 sample_rate; /* Sampler threshold. Sample the mirror packets if the
                           generated sFlow random number is less than or equal
                           to the threshold. Set 0 to disable sampler. */
    uint32 priority;    /* Profile priority. Higher value means higher priority.
                           When multiple profiles are hit simultaneously, the
                           profile with the highest priority will be chosen to
                           mirror. */
} bcm_mirror_ingress_mod_event_profile_t;

/* Initialize an ingress mirror-on-drop event profile structure. */
extern void bcm_mirror_ingress_mod_event_profile_t_init(
    bcm_mirror_ingress_mod_event_profile_t *profile);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create an ingress mirror-on-drop event profile. */
extern int bcm_mirror_ingress_mod_event_profile_create(
    int unit, 
    bcm_mirror_ingress_mod_event_profile_t *profile, 
    int *profile_id);

/* Destroy an ingress mirror-on-drop event profile. */
extern int bcm_mirror_ingress_mod_event_profile_destroy(
    int unit, 
    int profile_id);

/* Get all ingress mirror-on-drop event profile ids. */
extern int bcm_mirror_ingress_mod_event_profile_get_all(
    int unit, 
    int *profile_id_array, 
    int *count);

/* Get an ingress mirror-on-drop event profile config. */
extern int bcm_mirror_ingress_mod_event_profile_get(
    int unit, 
    int profile_id, 
    bcm_mirror_ingress_mod_event_profile_t *profile);

/* Add an ingress mirror-on-drop event group to an event profile. */
extern int bcm_mirror_ingress_mod_event_profile_group_add(
    int unit, 
    int profile_id, 
    bcm_mirror_ingress_mod_event_group_t group, 
    int hit);

/* Delete an ingress mirror-on-drop event group to an event profile. */
extern int bcm_mirror_ingress_mod_event_profile_group_delete(
    int unit, 
    int profile_id, 
    bcm_mirror_ingress_mod_event_group_t group);

/* Get all event groups of an ingress mirror-on-drop event profile. */
extern int bcm_mirror_ingress_mod_event_profile_group_get_all(
    int unit, 
    int profile_id, 
    bcm_mirror_ingress_mod_event_group_t *group_array, 
    int *count);

/* 
 * Get hit expectation of an ingress mirror-on-drop event group from an
 * event profile.
 */
extern int bcm_mirror_ingress_mod_event_profile_group_get(
    int unit, 
    int profile_id, 
    bcm_mirror_ingress_mod_event_group_t group, 
    int *hit);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Number of entries for a group of multiple destination IP addresses. */
typedef enum bcm_mirror_multi_dip_entry_num_e {
    bcmMirrorMultiDipEntryNum1 = 0,     /* 1 entry. */
    bcmMirrorMultiDipEntryNum2 = 1,     /* 2 entries. */
    bcmMirrorMultiDipEntryNum4 = 2,     /* 4 entries. */
    bcmMirrorMultiDipEntryNum8 = 3,     /* 8 entries. */
    bcmMirrorMultiDipEntryNum16 = 4,    /* 16 entries. */
    bcmMirrorMultiDipEntryNum32 = 5,    /* 32 entries. */
    bcmMirrorMultiDipEntryNum64 = 6,    /* 64 entries. */
    bcmMirrorMultiDipEntryNumCount = 7  /* Must Be Last. Not a valid value. */
} bcm_mirror_multi_dip_entry_num_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* Create a group of multiple destination IPv4 addresses. */
extern int bcm_mirror_multi_dip_group_ipv4_create(
    int unit, 
    bcm_mirror_multi_dip_entry_num_t num_entries, 
    bcm_ip_t *ipv4_addr_array, 
    bcm_mirror_multi_dip_group_t *group);

/* Remove a group of multiple destination IPv4 addresses. */
extern int bcm_mirror_multi_dip_group_ipv4_destroy(
    int unit, 
    bcm_mirror_multi_dip_group_t group);

/* Get all groups of multiple destination IPv4 addresses. */
extern int bcm_mirror_multi_dip_group_ipv4_get_all(
    int unit, 
    bcm_mirror_multi_dip_group_t *ipv4_group_array, 
    int *count);

/* Get a group of multiple destination IPv4 addresses. */
extern int bcm_mirror_multi_dip_group_ipv4_get(
    int unit, 
    bcm_mirror_multi_dip_group_t group, 
    bcm_ip_t *ipv4_addr_array, 
    int *count);

/* Create a group of multiple destination IPv6 addresses. */
extern int bcm_mirror_multi_dip_group_ipv6_create(
    int unit, 
    bcm_mirror_multi_dip_entry_num_t num_entries, 
    bcm_ip6_t *ipv6_addr_array, 
    bcm_mirror_multi_dip_group_t *group);

/* Remove a group of multiple destination IPv6 addresses. */
extern int bcm_mirror_multi_dip_group_ipv6_destroy(
    int unit, 
    bcm_mirror_multi_dip_group_t group);

/* Get all groups of multiple destination IPv6 addresses. */
extern int bcm_mirror_multi_dip_group_ipv6_get_all(
    int unit, 
    bcm_mirror_multi_dip_group_t *ipv6_group_array, 
    int *count);

/* Get a group of multiple destination IPv6 addresses. */
extern int bcm_mirror_multi_dip_group_ipv6_get(
    int unit, 
    bcm_mirror_multi_dip_group_t group, 
    bcm_ip6_t *ipv6_addr_array, 
    int *count);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Truncate length profile structure. */
typedef struct bcm_mirror_truncate_length_profile_s {
    bcm_mirror_truncate_mode_t mode;    /* Truncate mode. */
    int adjustment_length;              /* Mirror payload adjustment length
                                           after truncation. */
} bcm_mirror_truncate_length_profile_t;

/* Initialize bcm_mirror_truncate_length_profile_t struct */
extern void bcm_mirror_truncate_length_profile_t_init(
    bcm_mirror_truncate_length_profile_t *profile);

/* Truncate length profile option flags. */
#define BCM_MIRROR_TRUNCATE_LENGTH_PROFILE_OPTIONS_REPLACE 0x00000001 /* Update truncate length
                                                          profile entry. */
#define BCM_MIRROR_TRUNCATE_LENGTH_PROFILE_OPTIONS_WITH_ID 0x00000002 /* Create or update
                                                          truncate length
                                                          profile with id. */

#ifndef BCM_HIDE_DISPATCHABLE

/* Create the truncate length profile. */
extern int bcm_mirror_truncate_length_profile_create(
    int unit, 
    uint32 options, 
    int *profile_id, 
    bcm_mirror_truncate_length_profile_t *profile);

/* Destroy the truncate length profile. */
extern int bcm_mirror_truncate_length_profile_destroy(
    int unit, 
    int profile_id);

/* Get the truncate length profile entry with a given profile id. */
extern int bcm_mirror_truncate_length_profile_get(
    int unit, 
    int profile_id, 
    bcm_mirror_truncate_length_profile_t *profile);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Mirror global truncate for non-encapsulation mirror packets. */
typedef struct bcm_mirror_global_truncate_s {
    bcm_mirror_truncate_mode_t mode;    /* Truncate mode. */
    int profile_id;                     /* Profile ID to adjust payload truncate
                                           length.Applicable only for adjustment
                                           truncate mode. */
} bcm_mirror_global_truncate_t;

/* Initialize a mirror global truncate structure. */
extern void bcm_mirror_global_truncate_t_init(
    bcm_mirror_global_truncate_t *global_truncate);

#ifndef BCM_HIDE_DISPATCHABLE

/* Set mirror global truncate for non-encapsulation mirror packets. */
extern int bcm_mirror_global_truncate_set(
    int unit, 
    bcm_mirror_global_truncate_t *global_truncate);

/* Get mirror global truncate for non-encapsulation mirror packets. */
extern int bcm_mirror_global_truncate_get(
    int unit, 
    bcm_mirror_global_truncate_t *global_truncate);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_MIRROR_H__ */
