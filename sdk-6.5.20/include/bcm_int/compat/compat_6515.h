/*
 * $Id: compat_6515.h,v 2.0 2018/09/20
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.15 routines
*/

#ifndef _COMPAT_6515_H_
#define _COMPAT_6515_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/mpls.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>

#ifdef INCLUDE_L3
/* IPMC address type. */
typedef struct bcm_compat6515_ipmc_addr_s {
    bcm_ip_t s_ip_addr;                 /* IPv4 Source address. */
    bcm_ip_t mc_ip_addr;                /* IPv4 Destination address. */
    bcm_ip6_t s_ip6_addr;               /* IPv6 Source address. */
    bcm_ip6_t mc_ip6_addr;              /* IPv6 Destination address. */
    bcm_vlan_t vid;                     /* VLAN identifier. */
    bcm_vrf_t vrf;                      /* Virtual Router Instance. */
    bcm_cos_t cos;                      /* COS based on dst IP multicast addr. */
    int ts;                             /* Source port or TGID bit. */
    int port_tgid;                      /* Source port or TGID. */
    int v;                              /* Valid bit. */
    int mod_id;                         /* Module ID. */
    bcm_multicast_t group;              /* Use this index to program IPMC table
                                           for XGS chips based on flags value.
                                           For SBX chips it is the Multicast
                                           Group index */
    uint32 flags;                       /* See BCM_IPMC_XXX flag definitions. */
    int lookup_class;                   /* Classification lookup class ID. */
    bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
    bcm_if_t l3a_intf;                  /* L3 interface associated with route. */
    int rp_id;                          /* Rendezvous point ID. */
    bcm_ip_t s_ip_mask;                 /* IPv4 Source subnet mask. */
    bcm_if_t ing_intf;                  /* L3 interface associated with this
                                           Entry */
    bcm_ip_t mc_ip_mask;                /* IPv4 Destination subnet mask. */
    bcm_ip6_t mc_ip6_mask;              /* IPv6 Destination subnet mask. */
    bcm_multicast_t group_l2;           /* Use this index to program IPMC table
                                           for l2 recipients if TTL/RPF check
                                           fails. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_ip6_t s_ip6_mask;               /* IPv6 Source subnet mask. */
    int priority;                       /* Entry priority. */
} bcm_compat6515_ipmc_addr_t;

/* Add new IPMC group. */
extern int bcm_compat6515_ipmc_add(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

/* Find info of an IPMC group. */
extern int bcm_compat6515_ipmc_find(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

/* Remove IPMC group. */
extern int bcm_compat6515_ipmc_remove(
    int unit,
    bcm_compat6515_ipmc_addr_t *data);

#endif
/* Port Resource Configuration */
typedef struct bcm_compat6515port_resource_s {
    uint32 flags;                   /* BCM_PORT_RESOURCE_XXX */
    bcm_gport_t port;               /* Local logical port number to connect to
                                       the given physical port */
    int physical_port;              /* Local physical port, -1 if the logical to
                                       physical mapping is to be deleted */
    int speed;                      /* Initial speed after FlexPort operation */
    int lanes;                      /* Number of PHY lanes for this physical
                                       port */
    bcm_port_encap_t encap;         /* Encapsulation mode for port */
    bcm_port_phy_fec_t fec_type;    /* fec_type for port */
    int phy_lane_config;            /* serdes pmd lane config for port */
    int link_training;              /* link training on or off */
    int roe_compression;            /* roe_compression enable */
} bcm_compat6515_port_resource_t;

#endif
#endif /* !_COMPAT_6515_H */
