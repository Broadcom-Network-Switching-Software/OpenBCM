/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-5.3.0 routines
 */

#ifndef _COMPAT_530_H_
#define _COMPAT_530_H_

#ifdef	BCM_RPC_SUPPORT

#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm_int/compat/compat_6517.h>
#include <bcm_int/compat/compat_6516.h>

typedef struct bcm_compat530_l2_addr_s {
    uint32              flags;          /* BCM_L2_XXX flags */
    bcm_mac_t           mac;            /* 802.3 MAC address */
    bcm_vlan_t          vid;            /* VLAN identifier */
    int                 port;           /* Zero-based port number */
    int                 modid;          /* XGS: modid */
    bcm_trunk_t         tgid;           /* Trunk group ID */
    int                 rtag;           /* Trunk port select formula */
    bcm_cos_t           cos_dst;        /* COS based on dst addr */
    bcm_cos_t           cos_src;        /* COS based on src addr */
    int                 l2mc_index;     /* XGS: index in L2MC table */
    bcm_pbmp_t          block_bitmap;   /* XGS: blocked egress bitmap */
    int			auth;		/* Used if auth enabled on port */
} bcm_compat530_l2_addr_t;

extern int bcm_compat530_l2_addr_add(
    int unit,
    bcm_compat530_l2_addr_t *l2addr);
extern int bcm_compat530_l2_conflict_get(
    int unit,
    bcm_compat530_l2_addr_t *addr,
    bcm_compat530_l2_addr_t *cf_array,
    int cf_max,
    int *cf_count);
extern int bcm_compat530_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_compat530_l2_addr_t *l2addr);


#ifdef INCLUDE_L3

typedef struct bcm_compat530_l3_tunnel_initiator_s {
    uint32               l3t_flags;      /* Config flags */
    bcm_tunnel_type_t    l3t_type;       /* Tunnel type */
    int                  l3t_ttl;        /* Tunnel header TTL */
    bcm_mac_t            l3t_dmac;       /* DA MAC */
    int                  l3t_ip4_df_sel; /* IP tunnel hdr DF bit for IPv4 */
    int                  l3t_ip6_df_sel; /* IP tunnel hdr DF bit for IPv6 */
    bcm_ip_t             l3t_dip;        /* Tunnel header DIP address */
    bcm_ip_t             l3t_sip;        /* Tunnel header SIP address */
    int                  l3t_dscp_sel;   /* Tunnel header DSCP select */
    int                  l3t_dscp;       /* Tunnel header DSCP value */
} bcm_compat530_l3_tunnel_initiator_t;

typedef struct bcm_compat530_l3_tunnel_terminator_s {
    uint32               l3t_flags;    /* Config flags */
    bcm_ip_t             l3t_sip;      /* SIP for tunnel header match */
    bcm_ip_t             l3t_dip;      /* DIP for tunnel header match */
    bcm_ip_t             l3t_sip_mask; /* SIP mask */
    bcm_ip_t             l3t_dip_mask; /* DIP mask */
    uint32               l3t_udp_dst_port;  /* UDP dst port for UDP packets */
    uint32               l3t_udp_src_port;  /* UDP src port for UDP packets */
    bcm_tunnel_type_t    l3t_type;     /* Tunnel type */
    bcm_pbmp_t           l3t_pbmp;     /* Port bitmap for this tunnel */
    bcm_vlan_t           l3t_vlan;     /* The VLAN ID for IPMC lookup */
} bcm_compat530_l3_tunnel_terminator_t;

extern int bcm_compat530_l3_tunnel_initiator_set(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat530_l3_tunnel_initiator_t *tunnel);
extern int bcm_compat530_l3_tunnel_initiator_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat530_l3_tunnel_initiator_t *tunnel);
extern int bcm_compat530_l3_tunnel_terminator_add(
    int unit,
    bcm_compat530_l3_tunnel_terminator_t *info);
extern int bcm_compat530_l3_tunnel_terminator_delete(
    int unit,
    bcm_compat530_l3_tunnel_terminator_t *info);
extern int bcm_compat530_l3_tunnel_terminator_update(
    int unit,
    bcm_compat530_l3_tunnel_terminator_t *info);
extern int bcm_compat530_l3_tunnel_terminator_get(
    int unit,
    bcm_compat530_l3_tunnel_terminator_t *info);

#endif	/* INCLUDE_L3 */

#endif	/* BCM_RPC_SUPPORT */

#endif	/* !_COMPAT_530_H */
