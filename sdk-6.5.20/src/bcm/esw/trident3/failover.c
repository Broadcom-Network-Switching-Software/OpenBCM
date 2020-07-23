/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * TD3 failover API
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIDENT3_SUPPORT) &&  defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/failover.h>
#include <bcm/failover.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0


/*
 * Function:
 *     bcm_td3_failover_alloc
 * Purpose:
 *     Alloc scache for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_failover_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size, prot_grp_idx_count;

    prot_grp_idx_count = soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm);
    alloc_size = SHR_BITALLOCSIZE(prot_grp_idx_count);
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     bcm_td3_failover_reinit
 * Purpose:
 *     Scache recovery for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_failover_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    int rv, prot_grp_idx_count;
    uint16 recovered_ver;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        /* Upgrading from SDK release that does not have warmboot state,
           Alloc scache for now on */
        rv = bcm_td3_failover_alloc(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        return BCM_E_NOT_FOUND;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* SCACHE Hande found, recover from SCAHE */
    prot_grp_idx_count = soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm);
    ptr = scache_ptr;

    /* Recover PROT_GROUP_MAP */
    sal_memcpy(BCM_FAILOVER_PROT_GROUP_MAP(unit), ptr,
               SHR_BITALLOCSIZE(prot_grp_idx_count));
    ptr += SHR_BITALLOCSIZE(prot_grp_idx_count);

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_td3_failover_sync
 * Purpose:
 *     Sync scache for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td3_failover_sync(int unit)
{   
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    int prot_grp_idx_count;
    int rv;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv)) {
        if (rv == BCM_E_NOT_FOUND) {
            return BCM_E_NONE;
        }
        return rv;
    }

    ptr = scache_ptr;
    prot_grp_idx_count = soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm);

    /* Sync PROT_GROUP_MAP */
    sal_memcpy(ptr, BCM_FAILOVER_PROT_GROUP_MAP(unit),
               SHR_BITALLOCSIZE(prot_grp_idx_count));
    ptr += SHR_BITALLOCSIZE(prot_grp_idx_count);

    return BCM_E_NONE;
}


#endif
#endif
