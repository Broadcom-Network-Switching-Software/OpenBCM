/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	ipmc.c
 * Purpose: 	Tracks and manages IPMC tables.
 */

#ifdef INCLUDE_L3

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>

int
bcm_hercules_ipmc_init(int unit)
{
#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
	return soc_mem_clear(unit, MEM_IPMCm, MEM_BLOCK_ALL, 0);
    }
#endif
#ifdef BCM_HUMV_SUPPORT
    if (SOC_IS_HUMV(unit)) {
	ipmc_entry_t    ent;
	int	        i, imin, imax;

	imin = soc_mem_index_min(unit, L3_IPMCm);
	imax = soc_mem_index_max(unit, L3_IPMCm);

        sal_memset(&ent, 0, sizeof(ent));
        soc_L3_IPMCm_field32_set(unit, &ent, VALIDf, 1);
        for (i = imin; i <= imax; i++) {
            SOC_IF_ERROR_RETURN
                (WRITE_L3_IPMCm(unit, SOC_BLOCK_ALL, i, &ent));
	}
	return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_detach(int unit)
{
    /* Just clear tables, as done in init */
    return bcm_hercules_ipmc_init(unit);
}

int
bcm_hercules_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_delete_all(int unit)
{
    return bcm_hercules_ipmc_init(unit);
}

int
bcm_hercules_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_out,
                      void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_ipmc_traverse(int unit, uint32 flags, bcm_ipmc_traverse_cb cb, 
                           void *user_data)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_ipmc_enable(int unit, int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_src_port_check(int unit, int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_src_ip_search(int unit, int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_egress_port_set(int unit, bcm_port_t port, 
                                const bcm_mac_t mac,  int untag, 
                                bcm_vlan_t vid, int ttl_thresh)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_egress_port_get(int unit, bcm_port_t port, sal_mac_addr_t mac,
                         int *untag, bcm_vlan_t *vid, int *ttl_thresh)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_repl_init(int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_repl_detach(int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_repl_get(int unit, int index, bcm_port_t port, 
                           bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_repl_add(int unit, int index, bcm_port_t port, 
                           bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_repl_delete(int unit, int index, bcm_port_t port, 
                              bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_repl_delete_all(int unit, int index, bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_egress_intf_add(int unit, int index, bcm_port_t port, 
                                  bcm_l3_intf_t *l3_intf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_hercules_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port, 
                                     bcm_l3_intf_t *l3_intf)
{
    return BCM_E_UNAVAIL;
}
#endif	/* INCLUDE_L3 */

typedef int _bcm_esw_hercules_ipmc_not_empty; /* Make ISO compilers happy. */
