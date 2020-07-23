
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Common Multicast Initializers
 */


#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/feature.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/mcast.h>
#include <bcm/vlan.h>
#include <bcm/debug.h>

/*
 * Function:
 *	bcm_mcast_addr_t_init
 * Description:
 *	Initialize a bcm_mcast_addr_t to a specified MAC address and VLAN,
 *	while zeroing all other fields.
 * Parameters:
 *	mcaddr - Pointer to bcm_mcast_addr_t
 * Returns:
 *	Nothing.
 */

void
bcm_mcast_addr_t_init(bcm_mcast_addr_t *mcaddr,
		      sal_mac_addr_t mac, bcm_vlan_t vid)
{
    if (NULL != mcaddr) { 
        sal_memset(mcaddr, 0, sizeof (*mcaddr));
        sal_memcpy(mcaddr->mac, mac, sizeof (sal_mac_addr_t));
        mcaddr->vid = vid;
    }
    return;
}

