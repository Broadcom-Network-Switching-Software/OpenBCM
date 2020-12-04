/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    failover.c
 * Purpose: Handle TH3 specific failover APIs
 *          with fixed next hop offset.
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(INCLUDE_L3)
#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/failover.h>

#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/failover.h>

#define REPLACE_ENABLE_BITMAP_SIZE 128

#define FAILOVER_LOCK(u)   sal_mutex_take(FAILOVER_INFO(u)->failover_mutex, sal_sem_FOREVER)
#define FAILOVER_UNLOCK(u) sal_mutex_give(FAILOVER_INFO(u)->failover_mutex)

/*
 * Function:
 *      bcm_th3_failover_hw_clear
 * Purpose:
 *      Cleart Failover Hw memory
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
static int
bcm_th3_failover_hw_clear(int unit)
{
    uint8 *mbuf;
    int size, rv = BCM_E_NONE;
    soc_mem_t mem;

    mem = PROT_NHI_TABLE_1m;
    size = SOC_MEM_TABLE_BYTES(unit, mem);
    mbuf = soc_cm_salloc(unit, size, "failover_mem");
    if (mbuf != NULL) {
        sal_memset(mbuf, 0, size);
        FAILOVER_LOCK(unit);
        rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                                 soc_mem_index_min(unit, mem),
                                 soc_mem_index_max(unit, mem), mbuf);
        FAILOVER_UNLOCK(unit);
        soc_cm_sfree(unit, mbuf);

    } else {
        rv = BCM_E_MEMORY;
        LOG_ERROR(BSL_LS_SOC_L3,
                  (BSL_META_U(unit, "FAILOVER HW mem %s clear failed.\n"),
                   SOC_MEM_NAME(unit, mem)));
    }

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_cleanup
 * Purpose:
 *      DeInit  the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_cleanup(int unit)
{
    _bcm_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);
    int rv = BCM_E_UNAVAIL;

    if (!failover_info->initialized) {
        return (BCM_E_NONE);
    }

    if (!SOC_HW_ACCESS_DISABLE(unit)) {
        rv = bcm_th3_failover_hw_clear(unit);
    }

    /* Destroy protection mutex. */
    sal_mutex_destroy(failover_info->failover_mutex);

    /* Mark the state as uninitialized */
    failover_info->initialized = FALSE;
    failover_info->prot_offset = 0;

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_init
 * Purpose:
 *      Initialize the Failover software module
 * Parameters:
 *      unit     - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_init(int unit)
{
    int rv = BCM_E_NONE;
    uint32 rval = 0;
    int prot_offset;
    _bcm_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);

    if (failover_info->initialized) {
        BCM_IF_ERROR_RETURN(bcm_th3_failover_cleanup(unit));
    }

    /* Create Failover  protection mutex. */
    failover_info->failover_mutex = sal_mutex_create("failover_mutex");
    if (!failover_info->failover_mutex) {
        return BCM_E_MEMORY;
    }

    if (soc_property_get(unit, spn_FAILOVER_FIXED_NH_OFFSET_ENABLE, 0)) {

        /* Fixed protection NHI offset = half of egr_l3_next_hop size (16384) */
        prot_offset = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) >> 1;
        if (prot_offset == 0) {
            return BCM_E_MEMORY;
        }

        /* Configure PROT_OFFSETf on PROT_NHI_CONFIGr */
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, PROT_NHI_CONFIGr,
                                          REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, PROT_NHI_CONFIGr, &rval,
                          PROT_OFFSETf, prot_offset);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, PROT_NHI_CONFIGr,
                                          REG_PORT_ANY, 0, rval));
        
        /* Clear failover Hw mem: PROT_NHI_TABLE_1m */
        if (!SOC_WARM_BOOT(unit)) {
            rv = bcm_th3_failover_hw_clear(unit);
        }
        BCM_IF_ERROR_RETURN(rv);

        failover_info->prot_offset = prot_offset;
        cli_out("Failover enabled with fixed next hop offset %d\n",
                prot_offset);
    } else {
        failover_info->prot_offset = 0;
    }

    failover_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_egress_set
 * Purpose:
 *      This function can be used to set the protected NH for
 *      the specified primary nexthop (intf)
 * Parameters:
 *      unit            - (IN) Device Number
 *      intf            - (IN) Primary NH interface
 *      failover_egr    - (IN/OUT) Egress object to be used as the
 *                        protection nexthop for primary NH.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_egress_set(int unit, bcm_if_t intf,
                            bcm_l3_egress_t *failover_egr)
{
    int rv = BCM_E_NONE;
    uint32 flags;
    bcm_if_t prot_intf;

    if (!FAILOVER_INFO(unit)->initialized) {
        return BCM_E_INIT;
    }

    if (FAILOVER_INFO(unit)->prot_offset == 0) {
        return BCM_E_CONFIG;
    }

    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) &&
        !BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        return BCM_E_PARAM;
    }

    /* Set prot_intf with fixed offset from primary intf */
    prot_intf = intf + FAILOVER_INFO(unit)->prot_offset;

    /* Create protected egress nh BCM_L3_WITH_ID flags */
    flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);

    /* For TH3 failover_id shouldn't be used by user. It's used internally
       only to distinguish between primary_intf and prot_intf egress creation.
       This flag bit will be clear in bcm_xgs3_l3_egress_create. */
    failover_egr->failover_id |= _FAILOVER_FIXED_NH_OFFSET_ON;
    rv = bcm_esw_l3_egress_create(unit, flags, failover_egr, &prot_intf);

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_egress_get
 * Purpose:
 *      This function can be used to retrieve the protected NH for
 *      the specified primary nexthop (intf)
 * Parameters:
 *      unit            - (IN) Device Number
 *      intf            - (IN) Primary NH interface
 *      failover_egr    - (OUT) Egress object used as the
 *                        protection nexthop for primary NH.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_egress_get(int unit, bcm_if_t intf,
                            bcm_l3_egress_t *failover_egr)
{
    int rv = BCM_E_NONE;
    bcm_if_t prot_intf;

    if (!FAILOVER_INFO(unit)->initialized) {
        return BCM_E_INIT;
    }

    if (FAILOVER_INFO(unit)->prot_offset == 0) {
        return BCM_E_CONFIG;
    }

    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) &&
        !BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        return BCM_E_PARAM;
    }

    /* Set prot_intf with fixed offset from primary intf */
    prot_intf = intf + FAILOVER_INFO(unit)->prot_offset;

    bcm_l3_egress_t_init(failover_egr);

    rv = bcm_esw_l3_egress_get(unit, prot_intf, failover_egr);

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_egress_clear
 * Purpose:
 *      This function can be used to clear the protected NH set earlier
 *      for the specified primary nexthop (intf)
 * Parameters:
 *      unit            - (IN) Device Number
 *      intf            - (IN) Primary NH interface
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_egress_clear(int unit, bcm_if_t intf)
{
    int rv = BCM_E_NONE;
    bcm_if_t prot_intf;

    if (!FAILOVER_INFO(unit)->initialized) {
        return BCM_E_INIT;
    }

    if (FAILOVER_INFO(unit)->prot_offset == 0) {
        return BCM_E_CONFIG;
    }

    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) &&
        !BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        return BCM_E_PARAM;
    }

    /* Set prot_intf with fixed offset from primary intf */
    prot_intf = intf + FAILOVER_INFO(unit)->prot_offset;

    /* First disable egress status for prot_intf (i.e., use primary intf) */
    rv = bcm_th3_failover_egress_status_set(unit, intf, FALSE);

    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_l3_egress_destroy(unit, prot_intf);
    }

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_egress_status_set
 * Purpose:
 *      This function controls (set/clear) the active state of use of
 *      primary NH or protected NH
 * Parameters:
 *      unit            - (IN) Device Number
 *      intf            - (IN) Primary NH interface
 *      enable          - (IN) Enable=True for use of protected NH,
 *                        Enable=False for use of Primary NH (intf).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_egress_status_set(int unit, bcm_if_t intf, int enable)
{
    int rv = BCM_E_NONE;
    int nh_idx, index, bit_pos;
    uint32 entry[5], data[5];
    soc_mem_t mem = PROT_NHI_TABLE_1m;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit) ||
        !FAILOVER_INFO(unit)->initialized) {
        return (BCM_E_INIT);
    }

    if (FAILOVER_INFO(unit)->prot_offset == 0) {
        return (BCM_E_CONFIG);
    }

    /* Check l3 switching mode. */
    if (!(BCM_XGS3_L3_EGRESS_MODE_ISSET(unit))) {
        return (BCM_E_DISABLED);
    }

    /* Input parameters check. */
    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) &&
        !BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        return (BCM_E_PARAM);
    }

    /* Calculate next hop index. */
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
        nh_idx = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else {
        nh_idx = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    }

    /* Validate primary nh_idx */
    if (nh_idx >= FAILOVER_INFO(unit)->prot_offset) {
        return BCM_E_PARAM;
    }

    /* Calculate PROT_NHI_TABLE_1: index, and REPLACE_ENABLE_BITMAP: bit position */
    index = nh_idx / REPLACE_ENABLE_BITMAP_SIZE;
    bit_pos = nh_idx % REPLACE_ENABLE_BITMAP_SIZE;

    FAILOVER_LOCK(unit);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field_get(unit, mem, entry, REPLACE_ENABLE_BITMAPf, data);
        SHR_BITWRITE(data, bit_pos, enable);
        soc_mem_field_set(unit, mem, entry, REPLACE_ENABLE_BITMAPf, data);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    }
    FAILOVER_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_th3_failover_egress_status_get
 * Purpose:
 *      This function retrieves the current status of use of
  *      primary NH or protected NH
 * Parameters:
 *      unit            - (IN) Device Number
 *      intf            - (IN) Primary NH interface
 *      enable          - (OUT) Enable=True for use of protected NH,
 *                        Enable=False for use of Primary NH (intf).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_failover_egress_status_get(int unit, bcm_if_t intf, int *enable)
{
    int rv = BCM_E_NONE;
    int nh_idx, index, bit_pos;
    uint32 entry[5], data[5];
    soc_mem_t mem = PROT_NHI_TABLE_1m;

    /*  Make sure module was initialized. */
    if (!BCM_XGS3_L3_INITIALIZED(unit) ||
        !FAILOVER_INFO(unit)->initialized) {
        return (BCM_E_INIT);
    }

    if (FAILOVER_INFO(unit)->prot_offset == 0) {
        return (BCM_E_CONFIG);
    }

    /* Check l3 switching mode. */
    if (!(BCM_XGS3_L3_EGRESS_MODE_ISSET(unit))) {
        return (BCM_E_DISABLED);
    }

    /* Input parameters check. */
    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf) &&
        !BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf)) {
        return (BCM_E_PARAM);
    }

    /* Calculate next hop index. */
    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf)) {
        nh_idx = intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else {
        nh_idx = intf - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    }

    /* Validate primary nh_idx */
    if ((nh_idx >= FAILOVER_INFO(unit)->prot_offset) || (enable == NULL)) {
        return BCM_E_PARAM;
    }

    /* Calculate PROT_NHI_TABLE_1: index, and REPLACE_ENABLE_BITMAP: bit position */
    index = nh_idx / REPLACE_ENABLE_BITMAP_SIZE;
    bit_pos = nh_idx % REPLACE_ENABLE_BITMAP_SIZE;

    FAILOVER_LOCK(unit);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field_get(unit, mem, entry, REPLACE_ENABLE_BITMAPf, data);
        *enable = (SHR_BITGET(data, bit_pos) ? 1 : 0);
    }
    FAILOVER_UNLOCK(unit);

    return rv;
}

#endif /* defined(BCM_TOMAHAWK3_SUPPORT) */
#endif /* defined(INCLUDE_L3) */
