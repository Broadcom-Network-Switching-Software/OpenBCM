/*! \file shr_types.h
 *
 * Shared data types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_TYPES_H
#define SHR_TYPES_H

#include <sal/sal_types.h>

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

/*! Generic Routing Encapsulation (GRE) header length. */
#define SHR_GRE_HDR_LEN                 16

/*! Length of the MPLS header. */
#define SHR_MPLS_HDR_LEN                4

/*! MPLS Associated Channel Header (ACH) length. */
#define SHR_MPLS_ACH_LEN                4

/*! Port number type. */
typedef uint16_t shr_port_t;

/*! Ethernet MAC address type. */
typedef uint8_t shr_mac_t[SHR_MAC_ADDR_LEN];

/*! Check if Ethernet MAC address is a multicast address. */
#define SHR_MAC_IS_MCAST(_mac)  \
    (_mac[0] & 0x1)

/*! Check if Ethernet MAC address is all zeros. */
#define SHR_MAC_IS_ZERO(_mac)  \
    (((_mac)[0] | (_mac)[1] | (_mac)[2] | \
      (_mac)[3] | (_mac)[4] | (_mac)[5]) == 0)

/*! IPv4 address type. */
typedef uint32_t shr_ip_t;

/*! IPv6 address type. */
typedef uint8_t shr_ip6_t[SHR_IPV6_ADDR_LEN];

/*!
 * \brief Enum string map entry.
 *
 * This type is intended for mapping enum values to strings, e.g. when
 * an enum value needs to be printed on the console.
 *
 * The complete enum map will be an array of this type, i.e. with one
 * entry per enum value.
 *
 * The type can also be used directly for CLI commands which take an
 * enum value as a parameter.
 */
typedef struct shr_enum_map_s {

    /*! Enum name. */
    char *name;

    /*! Enum value. */
    int val;

} shr_enum_map_t;

#endif /* SHR_TYPES_H */
