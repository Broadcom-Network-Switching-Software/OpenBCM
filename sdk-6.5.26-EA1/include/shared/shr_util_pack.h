/*! \file shr_util_pack.h
 *
 * Pack and unpack macros using Big-Endian (network byte order).
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef   SHR_UTIL_PACK_H
#define   SHR_UTIL_PACK_H

#include <sal/types.h>

/*! IPv4 header version. */
#define SHR_IPV4_VERSION                4

/*! IPv4 header length in bytes. */
#define SHR_IPV4_HEADER_LENGTH          20

/*! IPv4 header length in 32-bit words. */
#define SHR_IPV4_HDR_WLEN               5

/*! IPv4 Address length. */
#define SHR_IPV4_ADDR_LEN               4

/*! IPv6 header version. */
#define SHR_IPV6_VERSION                6

/*! IPv6 header length in bytes. */
#define SHR_IPV6_HEADER_LENGTH          40

/*! IPv6 Address length. */
#define SHR_IPV6_ADDR_LEN               16

/*! L2 header EthertType for IPv4. */
#define SHR_L2_ETYPE_IPV4               0x0800

/*! L2 header ether type for IPv6. */
#define SHR_L2_ETYPE_IPV6               0x86dd

/*! L2 header ether type for MPLS unicast. */
#define SHR_L2_ETYPE_MPLS_UNICAST       0x8847

/*! L2 ether type length. */
#define SHR_L2_ETYPE_LEN                2

/*! IP protocol number of UDP. */
#define SHR_IP_PROTO_UDP                17

/*! IP protocol number of IPV4 (IPv4 in IP encapsulation). */
#define SHR_IP_PROTO_IPV4               4

/*! IP protocol number of IPV6 (IPv6 in IP encapsulation). */
#define SHR_IP_PROTO_IPV6               41

/*! IP protocol number of GRE (Generic Routing Encapsulation). */
#define SHR_IP_PROTO_GRE                47

/*! UDP header length */
#define SHR_UDP_HDR_LEN                 8

/*! Initial UDP Payload length. */
#define SHR_UDP_PAYLOAD_LEN             0x10

/*! Length of L2 CRC. */
#define SHR_L2_CRC_LEN                  4

/*! Ethernet MAC address length. */
#define SHR_MAC_ADDR_LEN                6

/*! VLAN header length. */
#define SHR_VLAN_HDR_LEN                4

/*! Length of the MPLS header. */
#define SHR_MPLS_HDR_LEN                4

/*! MPLS Associated Channel Header (ACH) length. */
#define SHR_MPLS_ACH_LEN                4

/*! Port number type. */
typedef uint16 shr_port_t;

/*! Ethernet MAC address type. */
typedef uint8 shr_mac_t[SHR_MAC_ADDR_LEN];

/*! Check if Ethernet MAC address is a multicast address. */
#define SHR_MAC_IS_MCAST(_mac)  \
    (_mac[0] & 0x1)

/*! Check if Ethernet MAC address is all zeros. */
#define SHR_MAC_IS_ZERO(_mac)  \
    (((_mac)[0] | (_mac)[1] | (_mac)[2] | \
      (_mac)[3] | (_mac)[4] | (_mac)[5]) == 0)

/*! Virtual eXtensible Local Area Network(VxLAN) header length. */
#define SHR_VXLAN_HDR_LEN               8

/*! VxLAN Network Identifier(VNID) length. */
#define SHR_VXLAN_VNID_LEN              3

/*! VxLAN Network Identifier type. */
typedef uint32 shr_vxlan_vnid_t;

/*! IPv4 address type. */
typedef uint32 shr_ip_t;

/*! IPv6 address type. */
typedef uint8 shr_ip6_t[SHR_IPV6_ADDR_LEN];

/*! Packing size for uint8 type. */
#define SHR_UTIL_PACKLEN_U8     1

/*! Packing size for uint16 type. */
#define SHR_UTIL_PACKLEN_U16    2

/*! Packing size for uint32 type. */
#define SHR_UTIL_PACKLEN_U32    4

/*! Packing size for uint64 type. */
#define SHR_UTIL_PACKLEN_U64    8

/*!
 * \brief Packs uint8 variable into packed buffer.
 *
 * \param [in] _buf Buffer pointer where data need to be updated.
 * \param [in] _var Variable to pack.
 *
 * \return Incremented buffer pointer.
 */
#define SHR_UTIL_PACK_U8(_buf, _var)            \
    *_buf++ = (_var) & 0xff

/*!
 * \brief Unpacks uint8 variable from packed buffer.
 *
 * \param [in] _buf buffer pointer from where to unpack.
 * \param [out] _var Output variable.
 *
 * \return updated input variable.
 */
#define SHR_UTIL_UNPACK_U8(_buf, _var)          \
    _var = *_buf++

/*!
 * \brief Packs uint16 variable into packed buffer.
 *
 * \param [in] _buf Buffer pointer where data need to be updated.
 * \param [in] _var Variable to pack.
 *
 * \return Incremented buffer pointer.
 */
#define SHR_UTIL_PACK_U16(_buf, _var)           \
    do {                                        \
        (_buf)[0] = ((_var) >> 8) & 0xff;       \
        (_buf)[1] = (_var) & 0xff;              \
        (_buf) += SHR_UTIL_PACKLEN_U16;         \
    } while (0)

/*!
 * \brief Unpacks uint16 variable from packed buffer.
 *
 * \param [in] _buf buffer pointer from where to unpack.
 * \param [out] _var Output variable.
 *
 * \return updated input variable.
 */
#define SHR_UTIL_UNPACK_U16(_buf, _var)         \
    do {                                        \
        (_var) = (((_buf)[0] << 8) |            \
                  (_buf)[1]);                   \
        (_buf) += SHR_UTIL_PACKLEN_U16;         \
    } while (0)

/*!
 * \brief Packs uint32 variable into packed buffer.
 *
 * \param [in] _buf Buffer pointer where data need to be updated.
 * \param [in] _var Variable to pack.
 *
 * \return Incremented buffer pointer.
 */
#define SHR_UTIL_PACK_U32(_buf, _var)           \
    do {                                        \
        (_buf)[0] = ((_var) >> 24) & 0xff;      \
        (_buf)[1] = ((_var) >> 16) & 0xff;      \
        (_buf)[2] = ((_var) >> 8) & 0xff;       \
        (_buf)[3] = (_var) & 0xff;              \
        (_buf) += SHR_UTIL_PACKLEN_U32;         \
    } while (0)

/*!
 * \brief Unpacks uint32 variable from packed buffer.
 *
 * \param [in] _buf buffer pointer from where to unpack.
 * \param [out] _var Output variable.
 *
 * \return updated input variable.
 */
#define SHR_UTIL_UNPACK_U32(_buf, _var)         \
    do {                                        \
        (_var) = (((_buf)[0] << 24) |           \
                  ((_buf)[1] << 16) |           \
                  ((_buf)[2] << 8)  |           \
                  (_buf)[3]);                   \
        (_buf) += SHR_UTIL_PACKLEN_U32;         \
    } while (0)

/*!
 * \brief Packs uint64 variable into packed buffer.
 *
 * \param [in] _buf Buffer pointer where data need to be updated.
 * \param [in] _var Variable to pack.
 *
 * \return Incremented buffer pointer.
 */
#define SHR_UTIL_PACK_U64(_buf, _var)           \
    do {                                        \
        (_buf)[0] = ((_var) >> 56) & 0xff;      \
        (_buf)[1] = ((_var) >> 48) & 0xff;      \
        (_buf)[2] = ((_var) >> 40) & 0xff;      \
        (_buf)[3] = ((_var) >> 32) & 0xff;      \
        (_buf)[4] = ((_var) >> 24) & 0xff;      \
        (_buf)[5] = ((_var) >> 16) & 0xff;      \
        (_buf)[6] = ((_var) >>  8) & 0xff;      \
        (_buf)[7] = (_var) & 0xff;              \
        (_buf) += SHR_UTIL_PACKLEN_U64;         \
    } while (0)

/*!
 * \brief Unpacks uint64 variable from packed buffer.
 *
 * \param [in] _buf buffer pointer from where to unpack.
 * \param [out] _var Output variable.
 *
 * \return updated input variable.
 */
#define SHR_UTIL_UNPACK_U64(_buf, _var)         \
    do {                                        \
        (_var) = (((_buf)[0] << 56) |           \
                  ((_buf)[1] << 48) |           \
                  ((_buf)[2] << 40) |           \
                  ((_buf)[3] << 32) |           \
                  ((_buf)[4] << 24) |           \
                  ((_buf)[5] << 16) |           \
                  ((_buf)[6] <<  8) |           \
                  (_buf)[7]);                   \
        (_buf) += SHR_UTIL_PACKLEN_U64;         \
    } while (0)
/*!
 * \brief Ipfix header fields.
 */
typedef struct shr_util_pack_ipfix_header_s {
    /*! Version. */
    uint16 version;
    /*! Header and payload length */
    uint16 length;
    /*! Export time. */
    uint32 export_time;
    /*! Sequence number. */
    uint32 sequence_num;
    /*! Observation domain id. */
    uint32 observation_id;
} shr_util_pack_ipfix_header_t;

/*!
 * \brief VxLAN header fields.
 */
typedef struct shr_util_pack_vxlan_header_s {
    /*! Flags. */
    uint32 flags;
    /*! VXLAN Network Identifier (VNI). */
    uint32 vnid;
} shr_util_pack_vxlan_header_t;

/*!
 * \brief UDP header fields.
 */
typedef struct shr_util_pack_udp_header_s {
    /*! Source port number. */
    uint16 src;
    /*! Destination port number. */
    uint16 dst;
    /*! Total length in bytes (header + data). */
    uint16 length;
    /*! Checksum. */
    uint16 sum;
} shr_util_pack_udp_header_t;

/*!
 * \brief IPv4 header fields.
 */
typedef struct shr_util_pack_ipv4_header_s {
    /*! Header version. */
    uint8 version;
    /*! Header length. */
    uint8 h_length;
    /*! Differentiated Services Code Point. */
    uint8 dscp;
    /*! Explicit Congestion Notification. */
    uint8 ecn;
    /*! Total length in bytes (header + data). */
    uint16 length;
    /*! Identification. */
    uint16 id;
    /*! Flags. */
    uint8 flags;
    /*! Fragment offset. */
    uint16 f_offset;
    /*! Time to Live. */
    uint8 ttl;
    /*! Protocol. */
    uint8 protocol;
    /*! Header checksum. */
    uint16 sum;
    /*! Source IP address. */
    uint32 src;
    /*! Destination IP address. */
    uint32 dst;
} shr_util_pack_ipv4_header_t;

/*!
 * \brief IPv6 header fields.
 */
typedef struct shr_util_pack_ipv6_header_s {
    /*! Header version. */
    uint8 version;
    /*! Traffic Class (6:dscp, 2:ecn). */
    uint8 t_class;
    /*! Flow Label. */
    uint32 f_label;
    /*! Payload length. */
    uint16 p_length;
    /*! Next header. */
    uint8 next_header;
    /*! Hop limit. */
    uint8 hop_limit;
    /*! Source IP address. */
    shr_ip6_t src;
    /*! Destination IP address. */
    shr_ip6_t dst;
} shr_util_pack_ipv6_header_t;

/*!
 * \brief GRE - Generic Routing Encapsulation header fields.
 */
typedef struct shr_util_pack_gre_header_s {
    /*! Checksum present. */
    uint8 checksum;
    /*! Routing present. */
    uint8 routing;
    /*! Key present. */
    uint8 key;
    /*! Sequence number present. */
    uint8 sequence_num;
    /*! Strict Source Routing. */
    uint8 strict_src_route;
    /*! Recursion Control. */
    uint8 recursion_control;
    /*! Flags. */
    uint8 flags;
    /*! Version. */
    uint8 version;
    /*! Protocol type. */
    uint16 protocol;
} shr_util_pack_gre_header_t;

/*!
 * \brief ACH - Associated Channel Header fields.
 */
typedef struct shr_util_pack_ach_header_s {
    /*! First nibble, must be 1. */
    uint8 f_nibble;
    /*! Version. */
    uint8 version;
    /*! Reserved. */
    uint8 reserved;
    /*! Channel Type. */
    uint16 channel_type;
} shr_util_pack_ach_header_t;

/*!
 * \brief MPLS - Multiprotocol Label Switching Label header fields.
 */
typedef struct shr_util_pack_mpls_label_s {
    /*! Label. */
    uint32 label;
    /*! Experimental, Traffic Class, ECN. */
    uint8 exp;
    /*! Bottom of stack. */
    uint8 s;
    /*! Time to Live. */
    uint8 ttl;
} shr_util_pack_mpls_label_t;

/*!
 * \brief VLAN Tag - 802.1Q header fields.
 */
typedef struct shr_util_pack_vlan_tag_s {
    /*! Tag Protocol Identifier. */
    uint16 tpid;
    struct {
        /*! Priority Code Point. */
        uint8 prio;
        /*! Canonical Format Indicator. */
        uint8 cfi;
        /*! VLAN identifier. */
        uint16 vid;
    } /*! Tag Control Identifier. */ tci;
} shr_util_pack_vlan_tag_t;

/*!
 * \brief L2 Header fields.
 */
typedef struct shr_util_pack_l2_header_t {
    /*! Destination MAC address. */
    shr_mac_t dst_mac;
    /*! Source MAC address. */
    shr_mac_t src_mac;
    /*! VLAN tag. */
    shr_util_pack_vlan_tag_t outer_vlan_tag;
    /*! Inner VLAN tag. */
    shr_util_pack_vlan_tag_t inner_vlan_tag;
    /*! EtherType. */
    uint16 etype;
} shr_util_pack_l2_header_t;

/*!
 * \brief Pack IPfix header data into a buffer.
 *
 * \param [out] buf     Output buffer pointer.
 * \param [in] ipfix    Ipfix header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_ipfix_header_pack(uint8 *buf, shr_util_pack_ipfix_header_t *ipfix);

/*!
 * \brief Pack VxLAN header data into a buffer.
 *
 * \param [out] buf     Output buffer pointer.
 * \param [in] vxlan    VxLAN header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_vxlan_header_pack(uint8 *buf, shr_util_pack_vxlan_header_t *vxlan);

/*!
 * \brief Pack UDP heeader data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] udp UDP header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_udp_header_pack(uint8 *buf, shr_util_pack_udp_header_t *udp);

/*!
 * \brief Pack IPv4 header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] ip IPv4 header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_ipv4_header_pack(uint8 *buf,
                               shr_util_pack_ipv4_header_t *ip);

/*!
 * \brief Pack IPv6 header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] ip  IPv6 header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_ipv6_header_pack(uint8 *buf,
                               shr_util_pack_ipv6_header_t *ip);

/*!
 * \brief Pack GRE header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] gre GRE header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_gre_header_pack(uint8 *buf, shr_util_pack_gre_header_t *gre);

/*!
 * \brief Pack Associated Channel Header (ACH) header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] ach ACH header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_ach_header_pack(uint8 *buf, shr_util_pack_ach_header_t *ach);

/*!
 * \brief Pack MPLS header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] mpls MPLS header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8 *
shr_util_pack_mpls_label_pack(uint8 *buf,
                              shr_util_pack_mpls_label_t *mpls);

/*!
 * \brief Pack L2 header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] l2 L2 header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8*
shr_util_pack_l2_header_pack(uint8 *buf, shr_util_pack_l2_header_t *l2);

/*!
 * \brief Calculate the initial checksum.
 *
 * Return the partial checksum as uint32 value. This routine simply adds up
 * the data as 16 bit words. The final 16 bit checksum has to be calculated
 * later.
 *
 * \param [in] length Length of the data buffer.
 * \param [in] data  Data buffer.
 *
 * \return final buffer pointer after packing all fields.
 */
extern uint32
shr_util_pack_initial_chksum_get(
    uint16 length,
    uint8 *data);

/*!
 * \brief Get the UDP initial checksum (excludes checksum for the data).
 *
 * The checksum includes the IP pseudo-header and UDP header.  It does
 * not include the checksum for the data.  The data checksum will be
 * added to UDP initial checksum each time a packet is sent, since
 * data payload may vary.
 *
 * \param [in] v6 True if IPv6 header, otherwise false.
 * \param [in] ipv4 IPv4 header pointer.
 * \param [in] ipv6 IPv6 header pointer.
 * \param [in] payload UDP payload.
 * \param [in] udp UDP header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint32
shr_util_pack_udp_initial_checksum_get(
    int v6,
    shr_util_pack_ipv4_header_t *ipv4,
    shr_util_pack_ipv6_header_t *ipv6,
    uint8 *payload,
    shr_util_pack_udp_header_t *udp);

#endif /* SHR_UTIL_PACK_H */
