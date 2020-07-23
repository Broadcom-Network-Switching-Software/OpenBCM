/*! \file shr_util_pack.h
 *
 * Pack and unpack macros using Big-Endian (network byte order).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   SHR_UTIL_PACK_H
#define   SHR_UTIL_PACK_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>

/*! Packing size for uint8_t type. */
#define SHR_UTIL_PACKLEN_U8     1

/*! Packing size for uint16_t type. */
#define SHR_UTIL_PACKLEN_U16    2

/*! Packing size for uint32_t type. */
#define SHR_UTIL_PACKLEN_U32    4

/*! Packing size for uint64_t type. */
#define SHR_UTIL_PACKLEN_U64    8

/*!
 * \brief Packs uint8_t variable into packed buffer.
 *
 * \param [in] _buf Buffer pointer where data need to be updated.
 * \param [in] _var Variable to pack.
 *
 * \return Incremented buffer pointer.
 */
#define SHR_UTIL_PACK_U8(_buf, _var)            \
    *_buf++ = (_var) & 0xff

/*!
 * \brief Unpacks uint8_t variable from packed buffer.
 *
 * \param [in] _buf buffer pointer from where to unpack.
 * \param [out] _var Output variable.
 *
 * \return updated input variable.
 */
#define SHR_UTIL_UNPACK_U8(_buf, _var)          \
    _var = *_buf++

/*!
 * \brief Packs uint16_t variable into packed buffer.
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
 * \brief Unpacks uint16_t variable from packed buffer.
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
 * \brief Packs uint32_t variable into packed buffer.
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
 * \brief Unpacks uint32_t variable from packed buffer.
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
 * \brief Packs uint64_t variable into packed buffer.
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
 * \brief Unpacks uint64_t variable from packed buffer.
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
 * \brief UDP header fields.
 */
typedef struct shr_util_pack_udp_header_s {
    /*! Source port number. */
    uint16_t src;
    /*! Destination port number. */
    uint16_t dst;
    /*! Total length in bytes (header + data). */
    uint16_t length;
    /*! Checksum. */
    uint16_t sum;
} shr_util_pack_udp_header_t;

/*!
 * \brief IPv4 header fields.
 */
typedef struct shr_util_pack_ipv4_header_s {
    /*! Header version. */
    uint8_t version;
    /*! Header length. */
    uint8_t h_length;
    /*! Differentiated Services Code Point. */
    uint8_t dscp;
    /*! Explicit Congestion Notification. */
    uint8_t ecn;
    /*! Total length in bytes (header + data). */
    uint16_t length;
    /*! Identification. */
    uint16_t id;
    /*! Flags. */
    uint8_t flags;
    /*! Fragment offset. */
    uint16_t f_offset;
    /*! Time to Live. */
    uint8_t ttl;
    /*! Protocol. */
    uint8_t protocol;
    /*! Header checksum. */
    uint16_t sum;
    /*! Source IP address. */
    uint32_t src;
    /*! Destination IP address. */
    uint32_t dst;
} shr_util_pack_ipv4_header_t;

/*!
 * \brief IPv6 header fields.
 */
typedef struct shr_util_pack_ipv6_header_s {
    /*! Header version. */
    uint8_t version;
    /*! Traffic Class (6:dscp, 2:ecn). */
    uint8_t t_class;
    /*! Flow Label. */
    uint32_t f_label;
    /*! Payload length. */
    uint16_t p_length;
    /*! Next header. */
    uint8_t next_header;
    /*! Hop limit. */
    uint8_t hop_limit;
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
    uint8_t checksum;
    /*! Routing present. */
    uint8_t routing;
    /*! Key present. */
    uint8_t key;
    /*! Sequence number present. */
    uint8_t sequence_num;
    /*! Strict Source Routing. */
    uint8_t strict_src_route;
    /*! Recursion Control. */
    uint8_t recursion_control;
    /*! Flags. */
    uint8_t flags;
    /*! Version. */
    uint8_t version;
    /*! Protocol type. */
    uint16_t protocol;
} shr_util_pack_gre_header_t;

/*!
 * \brief ACH - Associated Channel Header fields.
 */
typedef struct shr_util_pack_ach_header_s {
    /*! First nibble, must be 1. */
    uint8_t f_nibble;
    /*! Version. */
    uint8_t version;
    /*! Reserved. */
    uint8_t reserved;
    /*! Channel Type. */
    uint16_t channel_type;
} shr_util_pack_ach_header_t;

/*!
 * \brief MPLS - Multiprotocol Label Switching Label header fields.
 */
typedef struct shr_util_pack_mpls_label_s {
    /*! Label. */
    uint32_t label;
    /*! Experimental, Traffic Class, ECN. */
    uint8_t exp;
    /*! Bottom of stack. */
    uint8_t s;
    /*! Time to Live. */
    uint8_t ttl;
} shr_util_pack_mpls_label_t;

/*!
 * \brief VLAN Tag - 802.1Q header fields.
 */
typedef struct shr_util_pack_vlan_tag_s {
    /*! Tag Protocol Identifier. */
    uint16_t tpid;
    struct {
        /*! Priority Code Point. */
        uint8_t prio;
        /*! Canonical Format Indicator. */
        uint8_t cfi;
        /*! VLAN identifier. */
        uint16_t vid;
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
    uint16_t etype;
} shr_util_pack_l2_header_t;

/*!
 * \brief Pack UDP heeader data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] udp UDP header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t *
shr_util_pack_udp_header_pack(uint8_t *buf, shr_util_pack_udp_header_t *udp);

/*!
 * \brief Pack IPv4 header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] ip IPv4 header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t *
shr_util_pack_ipv4_header_pack(uint8_t *buf,
                               shr_util_pack_ipv4_header_t *ip);

/*!
 * \brief Pack IPv6 header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] ip  IPv6 header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t *
shr_util_pack_ipv6_header_pack(uint8_t *buf,
                               shr_util_pack_ipv6_header_t *ip);

/*!
 * \brief Pack GRE header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] gre GRE header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t *
shr_util_pack_gre_header_pack(uint8_t *buf, shr_util_pack_gre_header_t *gre);

/*!
 * \brief Pack Associated Channel Header (ACH) header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] ach ACH header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t *
shr_util_pack_ach_header_pack(uint8_t *buf, shr_util_pack_ach_header_t *ach);

/*!
 * \brief Pack MPLS header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] mpls MPLS header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t *
shr_util_pack_mpls_label_pack(uint8_t *buf,
                              shr_util_pack_mpls_label_t *mpls);

/*!
 * \brief Pack L2 header data into a buffer.
 *
 * \param [out] buf Output buffer pointer.
 * \param [in] l2 L2 header pointer.
 *
 * \return Final buffer pointer after packing all fields.
 */
extern uint8_t*
shr_util_pack_l2_header_pack(uint8_t *buf, shr_util_pack_l2_header_t *l2);

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
extern uint32_t
shr_util_pack_initial_chksum_get(
    uint16_t length,
    uint8_t *data);

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
extern uint32_t
shr_util_pack_udp_initial_checksum_get(
    int v6,
    shr_util_pack_ipv4_header_t *ipv4,
    shr_util_pack_ipv6_header_t *ipv6,
    uint8_t *payload,
    shr_util_pack_udp_header_t *udp);

#endif /* SHR_UTIL_PACK_H */
