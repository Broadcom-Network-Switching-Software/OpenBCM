/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-5.3.1 routines
 */

#ifndef _COMPAT_531_H_
#define _COMPAT_531_H_

#ifdef	BCM_RPC_SUPPORT

#include <bcm/types.h>

typedef struct bcm_compat531_l2_cache_addr_s {
    uint32              flags;          /* BCM_L2_CACHE_XXX flags */
    bcm_mac_t           mac;            /* dest MAC address to match */
    bcm_mac_t           mac_mask;       /* mask */
    bcm_vlan_t          vlan;           /* VLAN to match */
    bcm_vlan_t          vlan_mask;      /* mask */
    bcm_port_t          src_port;       /* ingress port to match (BCM5660x) */
    bcm_port_t          src_port_mask;  /* mask (must be 0 if not BCM5660x) */
    bcm_module_t        dest_modid;     /* switch destination module */
    bcm_port_t          dest_port;      /* switch destination port */
    bcm_trunk_t         dest_trunk;     /* switch destination trunk ID */
    int                 prio;           /* -1 to not set internal priority */
} bcm_compat531_l2_cache_addr_t;

extern int bcm_compat531_l2_cache_set(
    int unit, 
    int index, 
    bcm_compat531_l2_cache_addr_t *addr,
    int *index_used);

extern int bcm_compat531_l2_cache_get(
    int unit,
    int index,
    bcm_compat531_l2_cache_addr_t *addr);

#endif	/* BCM_RPC_SUPPORT */

#endif	/* !_COMPAT_531_H */
