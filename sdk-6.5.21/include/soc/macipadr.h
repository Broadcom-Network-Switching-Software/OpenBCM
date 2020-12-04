/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        macipadr.h
 * Purpose:     Typedefs for MAC and IP addresses
 */

#ifndef _SYS_MACIPADR_H
#define _SYS_MACIPADR_H

#include <sal/core/libc.h>
#include <soc/types.h>
#include <shared/l3.h>

typedef _shr_ip_addr_t  ip_addr_t;      /* IP Address */
typedef _shr_ip6_addr_t  ip6_addr_t;    /* IPv6 Address */

extern const sal_mac_addr_t _soc_mac_spanning_tree;
extern const sal_mac_addr_t _soc_mac_all_routers;
extern const sal_mac_addr_t _soc_mac_all_zeroes;
extern const sal_mac_addr_t _soc_mac_all_ones;

/* sal_mac_addr_t mac;  Just generate a list of the macs for display */
#define MAC_ADDR_LIST(mac) \
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

/* sal_mac_addr_t m1, m2;  True if equal */
#define MAC_ADDR_EQUAL(m1, m2) (!sal_memcmp(m1, m2, sizeof(sal_mac_addr_t)))

/* sal_mac_addr_t m1, m2;  like memcmp, returns -1, 0, or 1 */
#define MAC_ADDR_CMP(m1, m2) sal_memcmp(m1, m2, sizeof(sal_mac_addr_t))

/* sal_mac_addr_t mac; True if multicast mac address */
#define MAC_IS_MCAST(mac) (((mac)[0] & 0x1) ? TRUE : FALSE)

#define MACADDR_STR_LEN 18              /* Formatted MAC address */
#define IPADDR_STR_LEN  16              /* Formatted IP address */
#define IP6ADDR_STR_LEN 64              /* Formatted IP address */


/* Adjust justification for uint32 writes to fields */
/* dst is an array name of type uint32 [] */
#define MAC_ADDR_TO_UINT32(mac, dst) do {\
        dst[0] = (((uint32)mac[2]) << 24 | \
                  ((uint32)mac[3]) << 16 | \
                  ((uint32)mac[4]) << 8 | \
                  ((uint32)mac[5])); \
        dst[1] = (((uint32)mac[0]) << 8 | \
                  ((uint32)mac[1])); \
    } while (0)

/* Adjust justification for uint32 writes to fields */
/* dst is an array name of type uint32 [] */
#define MAC_ADDR_FROM_UINT32(mac, src) do {\
        mac[0] = (uint8) (src[1] >> 8 & 0xff); \
        mac[1] = (uint8) (src[1] & 0xff); \
        mac[2] = (uint8) (src[0] >> 24); \
        mac[3] = (uint8) (src[0] >> 16 & 0xff); \
        mac[4] = (uint8) (src[0] >> 8 & 0xff); \
        mac[5] = (uint8) (src[0] & 0xff); \
    } while (0)

#endif  /* _SYS_MACIPADR_H */
