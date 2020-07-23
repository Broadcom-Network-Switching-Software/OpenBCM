/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <sal/core/boot.h>

#include <bcm/error.h>
#include <bcm/multicast.h>

/*
 * Function:
 *      bcm_vlan_ip_t_init
 * Purpose:
 *      Initialize the vlan_ip_t structure
 * Parameters:
 *      vlan_ip - Pointer to structure which should be initialized
 * Returns:
 *      NONE
*/
void 
bcm_multicast_replication_t_init(bcm_multicast_replication_t *replication)
{
    if (NULL != replication) {
        sal_memset(replication, 0, sizeof (bcm_multicast_replication_t));
    }
    return;
}

/*
 * Function:
 *      bcm_multicast_multi_info_t_init
 * Purpose:
 *      Initialize multicast multi group struct.
 * Parameters:
 *      ecmp_member - pointer to multicast multi struct.
 * Returns:
 *      NONE
 */
void
bcm_multicast_multi_info_t_init(bcm_multicast_multi_info_t *mc_multi_info)
{
    if (NULL != mc_multi_info) {
        sal_memset(mc_multi_info, 0, sizeof (*mc_multi_info));
    }
    return;
}

/*
 *     bcm_multicast_encap_t_init
 * Description:
 *     Initialize a bcm_multicast_encap_t structure.
 * Parameters:
 *     mc_encap - pointer to bcm_multicast_encap_t strucutre.
 * Returns:
 *      NONE
 */
void
bcm_multicast_encap_t_init(bcm_multicast_encap_t *mc_encap)
{
    if (mc_encap != NULL) {
        sal_memset(mc_encap, 0, sizeof(bcm_multicast_encap_t));
    }
    return;
}

