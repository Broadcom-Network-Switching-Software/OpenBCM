/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mcast.c
 * Purpose:     Tracks and manages L2 Multicast tables.
 *
 * NOTE: These are empty stubs because the standard bcm_mcast layer
 * calls are not intended for programming a fabric chip.
 * Fabric device mcast programming should use the bcm_mcast_bitmap_* APIs.
 */

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>


int
bcm_hercules_mcast_addr_add_w_l2mcindex(int unit,bcm_mcast_addr_t *mcaddr)
{
  return (BCM_E_UNAVAIL);
}

int
bcm_hercules_mcast_addr_remove_w_l2mcindex(int unit, bcm_mcast_addr_t *mcaddr)
{
  return (BCM_E_UNAVAIL);
}

int
bcm_hercules_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr)
{
  return (BCM_E_UNAVAIL);
}

int
bcm_hercules_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr)
{
  return (BCM_E_UNAVAIL);
}

int
bcm_hercules_mcast_addr_add(int unit, bcm_mcast_addr_t *mcaddr)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_mcast_addr_remove(int unit, sal_mac_addr_t mac, bcm_vlan_t vid)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_hercules_mcast_port_get(int unit,
                            sal_mac_addr_t mac, bcm_vlan_t vid,
                            bcm_mcast_addr_t *mcaddr)
{
    return (BCM_E_UNAVAIL);
}

int
_bcm_hercules_mcast_detach(int unit)
{
    return BCM_E_NONE;
}

int
bcm_hercules_mcast_init(int unit)
{
#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        return soc_mem_clear(unit, MEM_MCm, MEM_BLOCK_ALL, 0);
    }
#endif
    return BCM_E_UNAVAIL;
}
