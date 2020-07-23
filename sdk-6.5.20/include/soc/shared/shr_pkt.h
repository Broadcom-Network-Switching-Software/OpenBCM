
/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        shr_pkt.h
 * Purpose:     Common Packet Format definitions shared by
 *              SDK and UKERNEL
 */

#ifndef   _SOC_SHARED_SHR_PKT_H_
#define   _SOC_SHARED_SHR_PKT_H_

#include <shared/pack.h>

#include <bcm/types.h>

/* LoopBack Header */
#define SHR_LB_HEADER_LENGTH             16

/* INT Header */
#define SHR_INT_HEADER_LENGTH            28

/* UDP Header */
#define SHR_UDP_HEADER_LENGTH            8

/* IPv4 Header */
#define SHR_IPV4_VERSION                 4
#define SHR_IPV4_HEADER_LENGTH           20    /* Bytes */
#define SHR_IPV4_HEADER_LENGTH_WORDS     5     /* Words */

/* IPv6 Header */
#define SHR_IPV6_VERSION                 6

/* L2 Header */
#define SHR_L2_ETYPE_IPV4                  0x0800
#define SHR_L2_ETYPE_IPV6                  0x86DD
#define SHR_MAC_ADDR_LENGTH    (sizeof(bcm_mac_t))

/* IP Protocols */
#define SHR_IP_PROTOCOL_UDP              17

/* Length of L2 CRC */
#define SHR_L2_CRC_LENGTH 4

/* Initial UDP Payload length. */
#define SHR_UDP_PAYLOAD_LENGTH           0x10

/* LoopBack Header */
typedef struct shr_lb_header_s {   /* <num bits>: ... */
    uint8 start;             /* 8: Start    */
    uint8 input_pri;         /* 4: Input Priority    */

    uint8 common_hdr0;       /* 4: Common Header field    */
    uint8 common_hdr1;       /* 1: Common Header field    */

    uint8 source_type;       /* 1: Source Type    */
    uint8 source0;           /* 6: Source 0 Module Id    */
    uint8 source1;           /* 8: Source 1 Ingress port    */
    uint8 source2;           /* 2: Source 2 Ingress port    */

    uint8 visibility;        /* 1: Visibility    */
    uint8 pkt_profile;       /* 3: Packet Profile Details    */

    uint8 reserved0_0;       /* 2:  Reserved Field   */
    uint8 reserved0_1;       /* 8:  Reserved Field   */
    uint8 reserved1_0;       /* 8:  Reserved Field   */
    uint8 reserved1_1;       /* 2:  Reserved Field   */

    uint8 zero;              /* 1:  Zero   */
    uint8 reserved2;         /* 5:  Reserved Field   */
    uint8 reserved3;         /* 8:  Reserved Field   */
    uint8 reserved4;         /* 8:  Reserved Field   */
    uint8 reserved5;         /* 8:  Reserved Field   */
    uint8 reserved6;         /* 8:  Reserved Field   */
    uint8 reserved7;         /* 8:  Reserved Field   */
    uint8 reserved8;         /* 8:  Reserved Field   */
    uint8 reserved9;         /* 8:  Reserved Field   */

    uint8 pp_port;           /* 8:  Ingress port    */
} shr_lb_header_t;

/* INT Header */
typedef struct shr_int_header_s {   /* <num bits>: ... */
    uint32  probemarker1;           /* 32: Probemerker1 value */
    uint32  probemarker2;           /* 32: Probemerker2 value */
    uint8   ver;                    /*  8: Version */
    uint8   type;                   /*  8: Type */
    uint16  flags;                  /* 16: Flags */
    uint32  requestvector;          /* 32: RequestVector */
    uint8   hoplimit;               /*  8: HopLimit */
    uint8   hopcount;               /*  8: HopCount */
    uint16  mustbezeros;            /* 16: MustBeZeros */
    uint16  maximumlength;          /* 16: MaximumLength of Payload */
    uint16  currentlength;          /* 16: CurrentLength of Payload */
    uint16  senderhandle;           /* 16: Sender Handle */
    uint16  seqnumber;              /* 16: Sequence Number */
} shr_int_header_t;

/* UDP Header */
typedef struct shr_udp_header_s {  /* <num bits>: ... */
    uint16  src;         /* 16: Source Port Number */
    uint16  dst;         /* 16: Destination Port Number */
    uint16  length;      /* 16: Length */
    uint16  sum;         /* 16: Checksum */
} shr_udp_header_t;

/* IPv4 Header */
typedef struct shr_ipv4_header_s {
    uint8   version;     /*  4: Version */
    uint8   h_length;    /*  4: Header length */
    uint8   dscp;        /*  6: Differentiated Services Code Point */
    uint8   ecn;         /*  2: Explicit Congestion Notification */
    uint16  length;      /* 16: Total Length bytes (header + data) */
    uint16  id;          /* 16: Identification */
    uint8   flags;       /*  3: Flags */
    uint16  f_offset;    /* 13: Fragment Offset */
    uint8   ttl;         /*  8: Time to Live */
    uint8   protocol;    /*  8: Protocol */
    uint16  sum;         /* 16: Header Checksum */
    uint32  src;         /* 32: Source IP Address */
    uint32  dst;         /* 32: Destination IP Address */
} shr_ipv4_header_t;

/* IPv6 Header */
typedef struct shr_ipv6_header_s {
    uint8      version;        /*   4: Version */
    uint8      t_class;        /*   8: Traffic Class (6:dscp, 2:ecn) */
    uint32     f_label;        /*  20: Flow Label */
    uint16     p_length;       /*  16: Payload Length */
    uint8      next_header;    /*   8: Next Header */
    uint8      hop_limit;      /*   8: Hop Limit */
    bcm_ip6_t  src;            /* 128: Source IP Address */
    bcm_ip6_t  dst;            /* 128: Destination IP Address */
} shr_ipv6_header_t;

/* GRE - Generic Routing Encapsulation */
typedef struct shr_gre_header_s {
    uint8   checksum;           /*  1: Checksum present */
    uint8   routing;            /*  1: Routing present */
    uint8   key;                /*  1: Key present */
    uint8   sequence_num;       /*  1: Sequence number present */
    uint8   strict_src_route;   /*  1: Strict Source Routing */
    uint8   recursion_control;  /*  3: Recursion Control */
    uint8   flags;              /*  5: Flags */
    uint8   version;            /*  3: Version */
    uint16  protocol;           /* 16: Protocol Type */
} shr_gre_header_t;

/* ACH - Associated Channel Header */
typedef struct shr_ach_header_s {
    uint8   f_nibble;        /*  4: First nibble, must be 1 */
    uint8   version;         /*  4: Version */
    uint8   reserved;        /*  8: Reserved */
    uint16  channel_type;    /* 16: Channel Type */
} shr_ach_header_t;

/* MPLS - Multiprotocol Label Switching Label */
typedef struct shr_mpls_label_s {
    uint32  label;    /* 20: Label */
    uint8   exp;      /*  3: Experimental, Traffic Class, ECN */
    uint8   s;        /*  1: Bottom of Stack */
    uint8   ttl;      /*  8: Time to Live */
} shr_mpls_label_t;

/* VLAN Tag - 802.1Q */
typedef struct shr_vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} shr_vlan_tag_t;

/* L2 Header */
typedef struct shr_l2_header_t {
    bcm_mac_t       dst_mac;      /* 48: Destination MAC */
    bcm_mac_t       src_mac;      /* 48: Source MAC */
    shr_vlan_tag_t  outer_vlan_tag;    /* VLAN Tag */
    shr_vlan_tag_t  inner_vlan_tag;    /* inner VLAN Tag */
    uint16          etype;        /* 16: Ether Type */
} shr_l2_header_t;

/* Ipfix Header */
typedef struct shr_ipfix_header_s {
    uint16 version;         /* 16: version */
    uint16 length;          /* 16: Header and payload length */
    uint32 export_time;     /* 32: export time */
    uint32 sequence_num;    /* 32: sequcene number */
    uint32 observation_id;  /* 32: observation domain id */
} shr_ipfix_header_t;

/*******************************************
 * Pack/Unpack Macros
 * 
 * Data is packed/unpacked in/from Network byte order
 */
#define SHR_PKT_PACK_U8(_buf, _var)     _SHR_PACK_U8(_buf, _var)
#define SHR_PKT_PACK_U16(_buf, _var)    _SHR_PACK_U16(_buf, _var)
#define SHR_PKT_PACK_U32(_buf, _var)    _SHR_PACK_U32(_buf, _var)
#define SHR_PKT_UNPACK_U8(_buf, _var)   _SHR_UNPACK_U8(_buf, _var)
#define SHR_PKT_UNPACK_U16(_buf, _var)  _SHR_UNPACK_U16(_buf, _var)
#define SHR_PKT_UNPACK_U32(_buf, _var)  _SHR_UNPACK_U32(_buf, _var)

/* Pack routines used to pack these header information into
 * provided buffers in network byte order.
 */
uint8 *
shr_lb_header_pack(uint8 *buffer, shr_lb_header_t *lb);
uint8 *
shr_int_header_pack(uint8 *buffer, shr_int_header_t *int_h);
uint8 *
shr_udp_header_pack(uint8 *buffer, shr_udp_header_t *udp);
uint8 *
shr_ipv4_header_pack(uint8 *buffer, shr_ipv4_header_t *ip);
uint8 *
shr_ipv6_header_pack(uint8 *buffer, shr_ipv6_header_t *ip);
uint8 *
shr_gre_header_pack(uint8 *buffer, shr_gre_header_t *gre);
uint8 *
shr_ach_header_pack(uint8 *buffer, shr_ach_header_t *ach);
uint8 *
shr_mpls_label_pack(uint8 *buffer, shr_mpls_label_t *mpls);
uint8 *
shr_l2_header_pack(uint8 *buffer, shr_l2_header_t *l2);
uint8 *
shr_ipfix_header_pack(uint8 *buffer, shr_ipfix_header_t *ipfix);

/* Utility functions */
uint32
shr_initial_chksum_get(uint16 length, uint8 *data);

uint32
shr_udp_initial_checksum_get(int v6,
                             shr_ipv4_header_t *ipv4,
                             shr_ipv6_header_t *ipv6,
                             uint8 *payload,
                             shr_udp_header_t  *udp);

#endif/* _SOC_SHARED_SHR_PKT_H_ */
