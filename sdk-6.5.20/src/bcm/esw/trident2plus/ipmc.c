/*
 *  
 *
 * File:        ipmc.c
 * Purpose:     Handle Trident2plus specific IPMC implementation
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/defs.h>
#include <assert.h>
#include <sal/core/libc.h>
#include <shared/util.h>

#if (defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3))
#include <soc/mem.h>
#include <soc/l3x.h>
#include <bcm/l3.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/l3.h>

#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/ipmc.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define TD2P_DEFIP_USED_SET(n)  \
{                                     \
    defip_ipmc_info[n]++;             \
}

#define TD2P_DEFIP_USED_GET(n)  defip_ipmc_info[n]

int *defip_ipmc_info;

/*
 * Function:
 *      bcm_td2p_defip_mc_route_check
 * Purpose:
 *      Check if the entry is a MC route and take appropriate action.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      lpm_cfg    - (IN)DEFIP information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_defip_mc_route_check (int unit, _bcm_defip_cfg_t *lpm_cfg)
{
    int ipmc_group;

    if (lpm_cfg->defip_flags & BCM_L3_IPMC) {
        ipmc_group = lpm_cfg->defip_mc_group;
        TD2P_DEFIP_USED_SET(ipmc_group);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_defip_ipmc_count_update
 * Purpose:
 *      Update IPMC DEFIP ref count based on local ref count
 * Parameters:
 *      unit       - (IN)SOC unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_defip_ipmc_count_update(int unit)
{
    int i, ipmc_count;

    for (i = 0; i < IPMC_GROUP_NUM(unit); i++) {
        ipmc_count = TD2P_DEFIP_USED_GET(i);
        if (ipmc_count) {
            IPMC_DEFIP_COUNT_SET(unit, i, ipmc_count);
        }
    }
    bcm_td2p_l3_defip_deinit(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_l3_defip_init
 * Purpose:
 *      Initialize defip_ipmc_info array to holdinformation about
 *      IPMC groups used by DEFIP Multicast entries
 * Parameters:
 *      unit       - (IN)SOC unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_l3_defip_init(int unit)
{
    int ipmc_count;
    ipmc_count = soc_mem_index_count(unit, L3_IPMCm);

    if (defip_ipmc_info != NULL) {
        bcm_td2p_l3_defip_deinit(unit);
    }
    defip_ipmc_info = sal_alloc(ipmc_count * sizeof(int),
            "DEFIP IPMC info");
    if (defip_ipmc_info == NULL) {
        return (BCM_E_MEMORY);
    }

    sal_memset(defip_ipmc_info, 0, ipmc_count * sizeof(int));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_l3_defip_deinit
 * Purpose:
 *      Free defip_ipmc_info array
 * Parameters:
 *      unit       - (IN)SOC unit number
 * Returns:
 *      None
 */
void
bcm_td2p_l3_defip_deinit(int unit)
{
    if (defip_ipmc_info != NULL) {
        sal_free(defip_ipmc_info);
        defip_ipmc_info = NULL;
    }
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
