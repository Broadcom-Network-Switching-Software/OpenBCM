/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-5.3.0 routines
 */

#ifdef	BCM_RPC_SUPPORT

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/l2.h>

#include <bcm_int/compat/compat_531.h>

/* bcm_l2_addr_t conversion functions */
STATIC void
_bcm_compat531in_l2_cache_addr_t(
    bcm_compat531_l2_cache_addr_t *from,
    bcm_l2_cache_addr_t *to)
{
    to->flags = from->flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    sal_memcpy(to->mac_mask, from->mac_mask, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->vlan_mask = from->vlan_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->dest_modid = from->dest_modid;
    to->dest_port = from->dest_port;
    to->dest_trunk = from->dest_trunk;
    to->prio = from->prio;
    BCM_PBMP_CLEAR(to->dest_ports);
}

STATIC void
_bcm_compat531out_l2_cache_addr_t(
    bcm_l2_cache_addr_t *from,
    bcm_compat531_l2_cache_addr_t *to)
{
    to->flags = from->flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    sal_memcpy(to->mac_mask, from->mac_mask, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->vlan_mask = from->vlan_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->dest_modid = from->dest_modid;
    to->dest_port = from->dest_port;
    to->dest_trunk = from->dest_trunk;
    to->prio = from->prio;
    /* ignore from->dest_ports */
}

int
bcm_compat531_l2_cache_set(
    int unit,
    int index, 
    bcm_compat531_l2_cache_addr_t *addr,
    int *index_used)
{
    int			rv;
    bcm_l2_cache_addr_t	n_addr;

    if (addr == NULL) {
        return BCM_E_PARAM;
    }
    _bcm_compat531in_l2_cache_addr_t(addr, &n_addr);
    rv = bcm_l2_cache_set(unit, index, &n_addr, index_used);
    return rv;
}

int
bcm_compat531_l2_cache_get(
    int unit,
    int index,
    bcm_compat531_l2_cache_addr_t *addr)
{
    int			rv;
    bcm_l2_cache_addr_t	n_addr;

    if (addr == NULL) {
        return BCM_E_PARAM;
    }
    rv = bcm_l2_cache_get(unit, index, &n_addr);
    if (rv >= 0) {
	_bcm_compat531out_l2_cache_addr_t(&n_addr, addr);
    }
    return rv;
}

#endif	/* BCM_RPC_SUPPORT */
