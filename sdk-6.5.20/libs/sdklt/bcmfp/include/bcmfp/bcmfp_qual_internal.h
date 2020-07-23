/*! \file bcmfp_qual_internal.h
 *
 * Enumerations for all qualifiers supported on all FP stages of all devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_QUAL_INTERNAL_H
#define BCMFP_QUAL_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_types_internal.h>

/*! Max Qualifier Count*/
#define BCMFP_QUALIFIERS_COUNT 1024

/*! Qualifier Identifier */
typedef uint32_t bcmfp_qualifier_t;

/*! Bitmap of Qualifier Ids */
typedef struct bcmfp_qset_s {
    /*! qualifier identifier bitmap */
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_QUALIFIERS_COUNT)];
} bcmfp_qset_t;

/*! Initialized the qualifier set with 0 */
#define BCMFP_QSET_INIT(qset)  \
    sal_memset(&(qset), 0, sizeof(bcmfp_qset_t))

/*! Add a qualifier Id to qualifier set */
#define BCMFP_QSET_ADD(qset, q)  SHR_BITSET(((qset).w), (q))

/*! Remove a qualifier Id from qualifier set */
#define BCMFP_QSET_REMOVE(qset, q)  SHR_BITCLR(((qset).w), (q))

/*! Check if a qualifier ID is part of Qualifier set */
#define BCMFP_QSET_TEST(qset, q)  SHR_BITGET(((qset).w), (q))

/*! Copy from one qualifier set to another */
#define BCMFP_QSET_COPY(to_qset, from_qset) \
    sal_memcpy((to_qset).w, (from_qset).w, sizeof(bcmfp_qset_t))

/*! Check if INPORTS Qualifier is part of Qualifier Set */
#define BCMFP_QSET_PBMP_TEST(qset) (BCMFP_QSET_TEST(qset, BCMFP_QUAL_INPORTS))

/*!
 * Field Qualifier IpProtocolCommon (BCMFP_QUAL_IP_PROTO_COMMON and
 * BCMFP_QUAL_INNER_IP_PROTO_COM)
 */
typedef enum bcmfp_ipprotocolcommon_e {
    /*! Don't care */
    BCMFP_IP_PROTO_ANY = 0,
    /*! TCP Packets (6) */
    BCMFP_IP_PROTO_COMMON_TCP = 1,
    /*! UDP Packets (17) */
    BCMFP_IP_PROTO_COMMON_UDP = 2,
    /*! IGMP Packets (2) */
    BCMFP_IP_PROTO_COMMON_IGMP = 3,
    /*! ICMP Packets (1) */
    BCMFP_IP_PROTO_COMMON_ICMP = 4,
    /*! IPv6 ICMP Packets (58) */
    BCMFP_IP_PROTO_COMMON_IP6ICMP = 5,
    /*! IPv6 Hop-by-Hop Packets (0) */
    BCMFP_IP_PROTO_COMMON_IP6HOPBYHOP = 6,
    /*! IPv4 Payload Packets (4) */
    BCMFP_IP_PROTO_COMMON_IPINIP = 7,
    /*! TCP and UDP Packets (6 & 17) */
    BCMFP_IP_PROTO_COMMON_TCPUDP = 8,
    /*! Non-standard IP Protocol. */
    BCMFP_IP_PROTO_UNKNOWN = 9,
    /*! Always Last. Not a usable value.*/
    BCMFP_IP_PROTO_COUNT = 10
} bcmfp_ipprotocolcommon_t;

/*! Field Qualifier IpType (BCMFP_QUAL_IP_TYPE). */
typedef enum bcmfp_iptype_e {
    /*! Don't care. */
    BCMFP_IPTYPE_ANY = 0,
    /*! Non-Ip packet. */
    BCMFP_IPTYPE_NONIP = 1,
    /*! IPv4 without options. */
    BCMFP_IPTYPE_IPV4NOOPTS = 2,
    /*! IPv4 with options. */
    BCMFP_IPTYPE_IPV4WITHOPTS = 3,
    /*! Any IPv4 packet. */
    BCMFP_IPTYPE_IPV4ANY = 4,
    /*!
     * IPv6 packet without any extension
     * header.
     */
    BCMFP_IPTYPE_IPV6NOEXTHDR = 5,
    /*!
     * IPv6 packet with one extension
     * header.
     */
    BCMFP_IPTYPE_IPV6ONEEXTHDR = 6,

    /*!
     * IPv6 packet with two or more
     * extension headers.
     */
    BCMFP_IPTYPE_IPV6TWOEXTHDR = 7,

    /*! Any IPv6 packet. */
    BCMFP_IPTYPE_IPV6_ANY = 8,
    /*! IPv4 and IPv6 packets. */
    BCMFP_IPTYPE_IP = 9,
    /*! ARP/RARP. */
    BCMFP_IPTYPE_ARP = 10,
    /*! ARP Request. */
    BCMFP_IPTYPE_ARPREQUEST = 11,
    /*! ARP Reply. */
    BCMFP_IPTYPE_ARPREPLY = 12,
    /*!
     * Mpls unicast frame (ethertype =
     * 0x8847).
     */
    BCMFP_IPTYPE_MPLSUNICAST = 13,
    /*!
     * Mpls mcast frame   (ethertype =
     *  0x8848).
     */
    BCMFP_IPTYPE_MPLSMULTICAST = 14,

    /*! Trill packet. */
    BCMFP_IPTYPE_TRILL = 15,
    /*! Mac-in-Mac frame. */
    BCMFP_IPTYPE_MIM = 16,
    /*! Always Last. Not a usable value. */
    BCMFP_IPTYPE_COUNT = 17
} bcmfp_iptype_t;


/*! Reserved Packet color. */
#define BCMFP_COLOR_PRESERVE    0
/*! Grenn Packet color. */
#define BCMFP_COLOR_GREEN       1
/*! Yellow Packet color. */
#define BCMFP_COLOR_YELLOW      2
/*! Red Packet color. */
#define BCMFP_COLOR_RED         3
/*! Black Packet color. */
#define BCMFP_COLOR_BLACK       4


/*!
 * Field Qualifier L2Format (for BCMFP_QUAL_L2_FORMAT).
 * Used to qualify packets based on L2 header format.
 */
typedef enum bcmfp_l2format_e {
    /*! Do not qualify on L2 format. */
    BCMFP_L2_FORMAT_ANY = 0,
    /*! Ethernet 2 (802.2). */
    BCMFP_L2_FORMAT_ETHII = 1,
    /*! Sub-Network Access Protocol (SNAP). */
    BCMFP_L2_FORMAT_SNAP = 2,
    /*! Logical Link Control. */
    BCMFP_L2_FORMAT_LLC = 3,
    /*! 802.3 frame format. */
    BCMFP_L2_FORMAT_802DOT3 = 4,
    /*!
     * Sub-Network Access Protocol (SNAP).
     * Vendor specific protocol.
     */
    BCMFP_L2_FORMAT_SNAPPRIVATE = 5,

    /*! MAC-In-MAC. */
    BCMFP_L2_FORMAT_MIM = 6,
    /*! PPPoE frame. */
    BCMFP_L2_FORMAT_PPPOE = 7,
    /*! Any other L2 format. */
    BCMFP_L2_FORMAT_OTHER = 8,
    /*! Always Last. Not a usable value. */
    BCMFP_L2_FORMAT_COUNT = 9
} bcmfp_l2format_t;


/*!
 * Field Qualifier Forwarding Type (for BCMFP_QUAL_FWD_TYPE)
 */
typedef enum bcmfp_forwardingtype_e {
    /*! Don't care. */
    BCMFP_FWD_TYPE_ANY = 0,
    /*! L2 switching forwarding. */
    BCMFP_FWD_TYPE_L2 = 1,
    /*! L3 routing forwarding. */
    BCMFP_FWD_TYPE_L3 = 2,
    /*! Shared vlan L2 forwarding. */
    BCMFP_FWD_TYPE_L2SHARED = 3,
    /*! Vlan based L2 forwarding. */
    BCMFP_FWD_TYPE_L2INDEPENDENT = 4,
    /*! VFI based L2 forwarding. */
    BCMFP_FWD_TYPE_L2VPN = 5,
    /*! L2 point to point vpn forwarding. */
    BCMFP_FWD_TYPE_L2VPNDIRECT = 6,
    /*! L3 predefined next hop forwarding. */
    BCMFP_FWD_TYPE_L3DIRECT = 7,
    /*! IPv4 Unicast Routing forwarding. */
    BCMFP_FWD_TYPE_IP4UCAST = 8,
    /*! IPv4 Multicast Routing forwarding. */
    BCMFP_FWD_TYPE_IP4MCAST = 9,
    /*! IPv6 Unicast Routing forwarding. */
    BCMFP_FWD_TYPE_IP6UCAST = 10,
    /*! IPv6 Multicast Routing forwarding. */
    BCMFP_FWD_TYPE_IP6MCAST = 11,
    /*! MPLS Switching forwarding. */
    BCMFP_FWD_TYPE_MPLS = 12,
    /*! Trill forwarding. */
    BCMFP_FWD_TYPE_TRILL = 13,
    /*! Forwarding according to a RxReason. */
    BCMFP_FWD_TYPE_RXREASON = 14,
    /*!
     * Traffic Management forwarding, when
     * an external Packet Processor sets the
     * forwarding decision.
     */
    BCMFP_FWD_TYPE_TRAFFICMANAGEMENT = 15,


    /*! Snooped packet. */
    BCMFP_FWD_TYPE_SNOOP = 16,
    /*!
     * MPLS Switching forwarding according
     * to the 1st label
     */
    BCMFP_FWD_TYPE_MPLSLABEL1 = 17,

    /*!
     * MPLS Switching forwarding according
     * to the 2nd label
     */
    BCMFP_FWD_TYPE_MPLSLABEL2 = 18,

    /*!
     * MPLS Switching forwarding according
     * to the 3rd label
     */
    BCMFP_FWD_TYPE_MPLSLABEL3 = 19,

    /*! VNTAG/ETAG based forwarding. */
    BCMFP_FWD_TYPE_PORTEXTENDER = 20,
    /*! GSH transit based forwarding. */
    BCMFP_FWD_TYPE_GSH_TRANSIT = 21,
    /*! Unknown forwarding type. */
    BCMFP_FWD_TYPE_UNKNOWN= 22,

    /*! Invalid fwd type */
    BCMFP_FWD_TYPE_INVALID = 23,
    /*! Vlan Fwd type */
    BCMFP_FWD_TYPE_VLAN = 24,
    /*! vrf fwd type */
    BCMFP_FWD_TYPE_VRF = 25,

    /*! Always Last. Not a usable value. */
    BCMFP_FWD_TYPE_COUNT = 26
} bcmfp_forwardingtype_t;


/*! Field Qualifier IpFrag */
typedef enum bcmfp_ipfrag_e {
    /*! Don't care. */
    BCMFP_IPFRAG_NON_OR_ANY = 0,
    /*! Non-fragmented packet. */
    BCMFP_IPFRAG_NON = 1,
    /*! First fragment of fragmented packet. */
    BCMFP_IPFRAG_FIRST = 2,
    /*! Non-fragmented or first fragment. */
    BCMFP_IPFRAG_NONORFIRST = 3,
    /*! Not the first fragment. */
    BCMFP_IPFRAG_NOTFIRST = 4,
    /*! Any fragment of fragmented packet. */
    BCMFP_IPFRAG_ANY = 5,
    /*! Always last. Not a usable value. */
    BCMFP_IPFRAG_COUNT = 6
} bcmfp_ipfrag_t;

/*! Field Qualifier for TPID*/
typedef enum bcmfp_tpid_e {
    /*! Don't care. */
    BCMFP_TPID_ANY  = 0,
    /*! TPID is 0x8100. */
    BCMFP_TPID_8100 = 1,
    /*! TPID is 0x9100. */
    BCMFP_TPID_9100 = 2,
    /*! TPID is 0x88a8. */
    BCMFP_TPID_88A8 = 3,
    /*! TPID is different than Above Options. */
    BCMFP_TPID_OTHER = 4
} bcmfp_tpid_t;

/*!
 * Field qualifier for STP state
 * (for BCMFP_QUAL_INGRESS_STP_STATE )
 */
typedef enum bcmfp_stg_stp_e {
    /*! Don't Care */
    BCMFP_STG_STP_ANY = 0,
    /*! STP Disabled State */
    BCMFP_STG_STP_DISABLE = 1,
    /*! STP Block State */
    BCMFP_STG_STP_BLOCK = 2,
    /*! STP Listen State */
    BCMFP_STG_STP_LISTEN = 3,
    /*! STP Learn State */
    BCMFP_STG_STP_LEARN = 4,
    /*! STP Forward State */
    BCMFP_STG_STP_FWD = 5,
    /*! STP Learn Forward State */
    BCMFP_STG_STP_LEARN_FWD = 6,
    /*! STP Learn Disabled State */
    BCMFP_STG_STP_LEARN_DISABLE = 7,
    /*! STP Forward Block State */
    BCMFP_STG_STP_FWD_BLOCK = 8,
    /*! STP Disable Block State */
    BCMFP_STG_STP_DISABLE_BLOCK = 9,
    /*! Always Last. Not a usable value */
    BCMFP_STG_STP_COUNT = 10
} bcmfp_stg_stp_t;

/*!
 * Field qualifier for VLAN translation look up hit (for
 * BCMFP_QUAL_VXLT_LOOKUP_HIT )
 */
typedef enum bcmfp_vxlt_lookup_hit_e {
    /*! Dont care. */
    BCMFP_VXLT_ANY                 = 0,
    /*! VXLT Lookup Not Hit. */
    BCMFP_VXLT_NO_HIT              = 1,
    /*! First VXLT Lookup Hit. */
    BCMFP_VXLT_FIRST_LOOKUP_HIT    = 2,
    /*! Second VXLT Lookup Hit. */
    BCMFP_VXLT_SECOND_LOOKUP_HIT   = 3,
    /*! First or Second VXLT Lookup Hit. */
    BCMFP_VXLT_ANY_LOOKUP_HIT      = 4
} bcmfp_vxlt_lookup_hit_t;

/*!
 * Field qualified for mpls forwarding label action
 * (for BCMFP_QUAL_MPLS_FWD_LABEL_ACTION )
 */
typedef enum bcmfp_mpls_fwd_label_action_e {
    /*! MPLS Forward Label Action is Invalid. */
    BCMFP_MPLS_ACT_INVALID             = 0,
    /*! MPLS Forward Label Action is PHP. */
    BCMFP_MPLS_ACT_PHP                 = 1,
    /*! MPLS Forward Label Action is Swap. */
    BCMFP_MPLS_ACT_SWAP                = 2,
    /*! POP the MPLS label */
    BCMFP_MPLS_ACT_POP                 = 3,
    /*! POP the MPLS label and use L2 VPN. */
    BCMFP_MPLS_ACT_POP_USE_L2_VPN      = 4,
    /*! POP the MPLS label and use L3 VPN. */
    BCMFP_MPLS_ACT_POP_USE_L3_VPN      = 5,
    /*! POP the MPLS label and use Entropy. */
    BCMFP_MPLS_ACT_POP_USE_ENTROPY     = 6
} bcmfp_mpls_fwd_label_action_t;

/*!
 * Field qualifier for loopback type
 * (for EFP QUAL_LOOPBACK_TYPE)
 */
typedef enum bcmfp_lb_type_e {
    /*! Don't care. */
    BCMFP_LB_ANY                = 0,
    /*! Tunnel type. */
    BCMFP_LB_TUNNEL             = 1,
    /*! TRILL network type. */
    BCMFP_LB_TRILL_NETWORK      = 2,
    /*! TRILL access type. */
    BCMFP_LB_TRILL_ACCESS       = 3,
    /*! TRILL masquearde type. */
    BCMFP_LB_TRILL_MASQUERADE   = 4,
} bcmfp_lb_type_t;

/*!
 * Field qualifier for loopback type
 * (for QUAL_LOOPBACK_TYPE)
 */
typedef enum bcmfp_loopback_type_e {
    /*! Dont Care. */
    BCMFP_LOOPBACK_TYPE_ANY = 0,
    /*! Generic loopback type. */
    BCMFP_LOOPBACK_TYPE_GENERIC        = 1,
} bcmfp_loopback_type_t;

/*!
 * Field qualifier for Tunnel type
 * (for BCMFP_QUAL_TNL_TYPE)
 */
typedef enum bcmfp_tunnel_type_e {
    /*! Don't care. */
    BCMFP_TNL_TYPE_ANY                 = 0,
    /*! IP in IP, Istap, GRE. */
    BCMFP_TNL_TYPE_IP                  = 1,
    /*! MPLS. */
    BCMFP_TNL_TYPE_MPLS                = 2,
    /*! Mac in Mac. */
    BCMFP_TNL_TYPE_MIM                 = 3,
    /*! IPV4 automatic multicast. */
    BCMFP_TNL_TYPE_AUTO_IP4_MC         = 4,
    /*! Trill. */
    BCMFP_TNL_TYPE_TRILL               = 5,
    /*! L2 GRE. */
    BCMFP_TNL_TYPE_L2GRE               = 6,
    /*! Vxlan tunnel Packet. */
    BCMFP_TNL_TYPE_VXLAN               = 7,
    /*! SRv6 tunnel Packet. */
    BCMFP_TNL_TYPE_SRV6                = 8,
    /*! L2 termination. */
    BCMFP_TNL_TYPE_NONE                = 9
} bcmfp_tunnel_type_t;

/*!
 * Field qualifier for IFP class ID type
 * (for BCMFP_QUAL_ING_CLASS_ID_TYPE)
 */
typedef enum bcmfp_ifp_class_id_type_e {
    /*! ANY CLASS ID. */
    BCMFP_ANY_CLASS_ID             = 0,
    /*! Source port based class ID. */
    BCMFP_ING_INTF_PORT_CLASS_ID   = 1,
    /*! Source virtual port based class ID. */
    BCMFP_ING_INTF_VPORT_CLASS_ID  = 2,
    /*! L3_IIF based class ID. */
    BCMFP_L3_INTF_CLASS            = 3,
    /*! VFP upper class ID. */
    BCMFP_FP_VLAN_CLASS1           = 4,
    /*! VFP lower class ID. */
    BCMFP_FP_VLAN_CLASS0           = 5,
    /*! L2 source based class ID. */
    BCMFP_L2_SRC_CLASS             = 6,
    /*! L2 destination based class ID. */
    BCMFP_L2DEST_CLASS             = 7,
    /*! L3 source based class ID. */
    BCMFP_L3_SRC_CLASS             = 8,
    /*! L3 destination based class ID. */
    BCMFP_L3DEST_CLASS             = 9,
    /*! VLAN based class ID. */
    BCMFP_VLAN_CLASS               = 10,
    /*! VRF based class ID. */
    BCMFP_VRF_CLASS                = 11,
    /*! IFP based class ID. */
    BCMFP_ING_CLASS_ID             = 15,
    /*! Invalid classs ID. */
    BCMFP_INVALID_CLASS_ID         = 16
} bcmfp_ifp_class_id_type_e;

/*!
 * \brief Qualifier Flags
 */
typedef enum bcmfp_qualifier_flags_e {

    /*! Action has to be in one part */
    BCMFP_QUALIFIER_FLAGS_ONE_PART = 0,

    /*! Action has to be in all parts */
    BCMFP_QUALIFIER_FLAGS_ALL_PARTS,

    /*! Last in the list */
    BCMFP_QUALIFIER_FLAGS_COUNT,

} bcmfp_qualifier_flags_t;
/*!
 * \brief Qualifier Bitmap Structure
 */
typedef struct bcmfp_qual_bitmap_s {
    /*!
     * Indicates all or partial bits of
     * qualifier needs to be extracted.
     */
    bool partial;

    /*! Compress the qualifier. */
    bool compress;

    /*! Qualifier ID. */
    bcmfp_qualifier_t qual;

    /*! Field ID in the group template LT */
    bcmltd_fid_t fid;

    /*!
     * Index in the array if LT field is of
     * array type.
     */
    uint16_t fidx;
    /*! Bitmap of the qual to be extracted */
    uint32_t bitmap[BCMFP_MAX_WSIZE];

    /*! Pointer to the next qual bitmap struct */
    struct bcmfp_qual_bitmap_s *next;

} bcmfp_qual_bitmap_t;

/*!
 * \brief Qualifier data(key, mask).
 */
typedef struct bcmfp_qual_data_s {
    /*! Qualifier ID */
    bcmfp_qualifier_t qual_id;

    /*! Qualifier LT field ID */
    bcmltd_fid_t fid;

    /*!
     * Array index if qualifier is array
     * type LT field.
     */
    uint16_t fidx;

    /*! Compress the qualifier. */
    bool compress;

    /*! Qualifier key value */
    uint32_t key[BCMFP_MAX_WSIZE];

    /*! Qualifier mask value */
    uint32_t mask[BCMFP_MAX_WSIZE];

    /*! Qualifier flags */
    bcmfp_qualifier_flags_t flags;

    /*! Pointer to next qualifier data */
    struct bcmfp_qual_data_s * next;
} bcmfp_qual_data_t;

/*! Field Qualifier PORTS bitmap type. */
typedef enum bcmfp_qual_pbmp_type_e {
    /*! NONE. */
    BCMFP_NONE             = 0,

    /*! INPORTS bitmap. */
    BCMFP_INPORTS          = 1,

    /*! SYSTEM PORTS bitmap. */
    BCMFP_SYSTEM_PORTS     = 2,

    /*! DEVICE PORTS bitmap. */
    BCMFP_DEVICE_PORTS     = 3,
} bcmfp_qual_pbmp_type_t;
#endif /* BCMFP_QUAL_INTERNAL_H */
