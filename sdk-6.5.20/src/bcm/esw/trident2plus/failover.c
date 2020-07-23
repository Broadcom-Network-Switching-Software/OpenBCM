/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    failover.c
 * Purpose: Handle trident2plus specific failover APIs
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/types.h>
#include <bcm/module.h>
#include <soc/scache.h>
#include <bcm/error.h>
#include <bcm/failover.h>
#include <bcm/extender.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/l3.h>
#ifdef BCM_TRIDENT3_SUPPORT
#include <bcm_int/esw/trident3.h>
#endif

/* Need L3 related information to clean dependent failover state */
#define L3_INFO(unit)    (&_bcm_l3_bk_info[unit])

#define _bcm_td2p_failover_bk_info _bcm_failover_bk_info
typedef _bcm_failover_bookkeeping_t _bcm_td2p_failover_bookkeeping_t; 

/* sw state for multi level failover */
bcmi_failover_multi_level_info_t *bcmi_multi_level_sw_state[BCM_MAX_NUM_UNITS];


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

 /*
  * Function:
  *      bcmi_failover_wb_alloc
  * Purpose:
  *      Allocate space for failover
  * Parameters:
  *      unit     - Device Number
  * Returns:
  *      BCM_E_XXX
  */
STATIC int
bcmi_failover_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size = 0;
    soc_mem_t prot_group_mem;
    int  num_entry, prot_group_num_entry;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_NONE;
    }

    prot_group_mem = TX_PROT_GROUP_1_TABLEm;

    prot_group_num_entry = (soc_mem_index_count(unit, prot_group_mem)) ;

    /* Get the max range */
    num_entry = (soc_mem_index_count(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm));

    /* Allocate space for multi level attach info */
    alloc_size += sizeof(bcmi_failover_multi_level_info_t) * num_entry;
    alloc_size += SHR_BITALLOCSIZE(num_entry);

    /* Alocate space for init prot to multi failover id mapping */
    alloc_size += (sizeof(int) * (num_entry * 2));
    /* Allocated space for level-2 failover Ids */
    alloc_size += SHR_BITALLOCSIZE(prot_group_num_entry);
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
 *      bcmi_failover_sync
 * Purpose:
 *      Sync level-2 state in scache
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_failover_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    uint32 index;
    int  num_entry, prot_group_num_entry;
    bcmi_failover_multi_level_info_t *multi_level_sw_state;
    int alloc_size = 0;
    soc_mem_t prot_group_mem;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_NONE;
    }

    prot_group_mem = TX_PROT_GROUP_1_TABLEm;

    prot_group_num_entry = (soc_mem_index_count(unit, prot_group_mem));

    /* Get the max range */
    num_entry = (soc_mem_index_count(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm));

    /* Allocate space for multi level attach info */
    alloc_size += sizeof(bcmi_failover_multi_level_info_t) * num_entry;

    alloc_size += SHR_BITALLOCSIZE(num_entry);
    /* Alocate space for init prot to multi failover id mapping */
    alloc_size += (sizeof(int) * (num_entry * 2));

    /* Allocated space for level-2 failover Ids */
    alloc_size += SHR_BITALLOCSIZE(prot_group_num_entry);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));


    ptr = scache_ptr;
    for (index = 0; index < num_entry; index++)
    {
        multi_level_sw_state = (&(bcmi_multi_level_sw_state[unit][index]));
        *(((uint32 *)ptr)) = multi_level_sw_state->parent_failover_id;
        ptr += sizeof(bcm_failover_t);
        *(((uint32 *)ptr)) = multi_level_sw_state->parent_failover_type;
        ptr += sizeof(uint32);
        *(((uint32 *)ptr)) = multi_level_sw_state->child_group_1;
        ptr += sizeof(bcm_failover_t);
        *(((uint32 *)ptr)) = multi_level_sw_state->child_group_2;
        ptr += sizeof(bcm_failover_t);
    }

    sal_memcpy(ptr, BCM_MULTI_LEVEL_FAILOVER_BITMAP(unit),
            SHR_BITALLOCSIZE(num_entry));
    ptr += SHR_BITALLOCSIZE(num_entry);

    sal_memcpy(ptr, BCM_MULTI_LEVEL_FAILOVER_ID_MAP(unit),
            (sizeof(int) * num_entry * 2));
    ptr += (sizeof(int) * (num_entry *2));

    sal_memcpy(ptr, BCM_FAILOVER_PROT_GROUP_MAP(unit),
            SHR_BITALLOCSIZE(prot_group_num_entry));
    ptr += SHR_BITALLOCSIZE(prot_group_num_entry);


    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_failover_reinit
 * Purpose:
 *      re construct failover state from scache
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcmi_failover_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    int stable_size, rv;
    uint16 recovered_ver;
    int index;
    bcmi_failover_multi_level_info_t *multi_level_sw_state;
    int  num_entry, prot_group_num_entry;
    int alloc_size = 0;
    soc_mem_t prot_group_mem;


    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_NONE;
    }

    /* Get the max range */
    num_entry = (soc_mem_index_count(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm));

    prot_group_mem = TX_PROT_GROUP_1_TABLEm;

    prot_group_num_entry = (soc_mem_index_count(unit, prot_group_mem));

    /* Allocate space for multi level attach info */
    alloc_size += sizeof(bcmi_failover_multi_level_info_t) * num_entry;

    alloc_size += SHR_BITALLOCSIZE(num_entry);
    /* Alocate space for init prot to multi failover id mapping */
    alloc_size += (sizeof(int) * (num_entry * 2));

    /* Allocated space for level-2 failover Ids */
    alloc_size += SHR_BITALLOCSIZE(prot_group_num_entry);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        /* multil level failover warmboot requires extended scache support
         * level2 warmboot must be enabled
         */
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, alloc_size, &scache_ptr,
            BCM_WB_DEFAULT_VERSION,  &recovered_ver);
    if (BCM_E_NOT_FOUND == rv) {
        /* Upgrading from SDK release that does not have warmboot state */
        BCM_IF_ERROR_RETURN(bcmi_failover_wb_alloc(unit));
        return BCM_E_NONE;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    ptr = scache_ptr;

    for (index = 0; index < num_entry; index++)
    {
        multi_level_sw_state = (&(bcmi_multi_level_sw_state[unit][index]));
        multi_level_sw_state->parent_failover_id = *((uint32 *)ptr);
        ptr += sizeof(bcm_failover_t);
        multi_level_sw_state->parent_failover_type = *((uint32 *)ptr);
        ptr += sizeof(uint32);
        multi_level_sw_state->child_group_1 = *((uint32 *)ptr);
        ptr += sizeof(bcm_failover_t);
        multi_level_sw_state->child_group_2 = *((uint32 *)ptr);
        ptr += sizeof(bcm_failover_t);
    }


    sal_memcpy(BCM_MULTI_LEVEL_FAILOVER_BITMAP(unit), ptr,
        SHR_BITALLOCSIZE(num_entry));
    ptr += SHR_BITALLOCSIZE(num_entry);

    sal_memcpy(BCM_MULTI_LEVEL_FAILOVER_ID_MAP(unit), ptr,
        sizeof(int) * (num_entry *2));
    ptr += (sizeof(int) * (num_entry *2));

    sal_memcpy(BCM_FAILOVER_PROT_GROUP_MAP(unit), ptr,
            SHR_BITALLOCSIZE(prot_group_num_entry));
    ptr += SHR_BITALLOCSIZE(prot_group_num_entry);

    return BCM_E_NONE;
}
#endif
/*
 * Function:
 *      _bcm_td2p_failover_check_init
 * Purpose:
 *      Check if Failover is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_td2p_failover_check_init (int unit)
{
    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
        return BCM_E_UNIT;
    }

    if (!_bcm_td2p_failover_bk_info[unit].initialized) {
        return BCM_E_INIT;
    } else {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcm_failover_lock
 * Purpose:
 *      Take the Failover Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2p_failover_lock (int unit)
{
    int rv;

    rv = _bcm_td2p_failover_check_init(unit);

    if ( rv == BCM_E_NONE ) {
        sal_mutex_take(FAILOVER_INFO((unit))->failover_mutex,
                sal_mutex_FOREVER);
    }
    return rv; 
}

/*
 * Function:
 *      bcm_failover_unlock
 * Purpose:
 *      Release  the Failover Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
void
bcm_td2p_failover_unlock (int unit)
{
    int rv;

    rv = _bcm_td2p_failover_check_init(unit);

    if ( rv == BCM_E_NONE ) {
        sal_mutex_give(FAILOVER_INFO((unit))->failover_mutex);
    }
}

/*
 * Function:
 *    bcmi_failover_dependent_free_resources
 * Purpose:
 *    frees dependednt resources.
 * Parameters:
 *    unit                 - (IN)bcm device id
 * Returns:
 *    NONE
 */

void
bcmi_failover_dependent_free_resources(int unit)
{
    _bcm_td2p_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);

    if (L3_INFO(unit)->l3_initialized) {
        return;
    }

    if (failover_info->initialized) {
        return;
    }

    if (failover_info->init_prot_to_multi_failover_id) {
        sal_free(failover_info->init_prot_to_multi_failover_id);
        failover_info->init_prot_to_multi_failover_id = NULL;
    }

}

/*
 * Function:
 *      _bcm_td2p_failover_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_td2p_failover_free_resource(int unit,
        _bcm_td2p_failover_bookkeeping_t *failover_info)
{
    if (!failover_info) {
        return;
    }

    if (failover_info->prot_group_bitmap) {
        sal_free(failover_info->prot_group_bitmap);
        failover_info->prot_group_bitmap = NULL;
    }

    if (failover_info->prot_nhi_bitmap) {
        sal_free(failover_info->prot_nhi_bitmap);
        failover_info->prot_nhi_bitmap = NULL;
    }

    if (failover_info->egress_prot_group_bitmap) {
        sal_free(failover_info->egress_prot_group_bitmap);
        failover_info->egress_prot_group_bitmap = NULL;
    }

    if (failover_info->ingress_prot_group_bitmap) {
        sal_free(failover_info->ingress_prot_group_bitmap);
        failover_info->ingress_prot_group_bitmap = NULL;
    }

    if (failover_info->multi_level_failover_bitmap) {
        sal_free(failover_info->multi_level_failover_bitmap);
        failover_info->multi_level_failover_bitmap = NULL;
    }

    if (bcmi_multi_level_sw_state[unit]) {
        sal_free(bcmi_multi_level_sw_state[unit]);
        bcmi_multi_level_sw_state[unit] = NULL;
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* 
 * Function:
 *     _bcm_td2p_failover_reinit
 * Purpose:
 *     Reinit for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_failover_reinit(int unit)
{
    int idx, index_min, index_max, i , bit_index;
    tx_initial_prot_group_table_entry_t prot_grp_entry;
    egr_tx_prot_group_table_entry_t egr_tx_prot_group;
    rx_prot_group_table_entry_t rx_prot_group_entry;
    uint32 buf[4];
    int value;
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
        replace_enable_field = REPLACE_ENABLEf;
    }

    index_min = soc_mem_index_min(unit, prot_group_mem);
    index_max = soc_mem_index_max(unit, prot_group_mem);

    for (idx = index_min; idx <= index_max; idx++) {

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, prot_group_mem,
                    MEM_BLOCK_ANY, idx, &prot_grp_entry));

        soc_mem_field_get(unit, prot_group_mem,
                (uint32 *)&prot_grp_entry, replace_enable_field, buf);
        for (i = 0; i < BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY; i++) {
            value = ((buf[i / 32] >> (i % 32)) & 0x1);
            if (value != 0) {
                bit_index = (((idx & 0x7) << 7) | (i & 0x7F));
                _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, bit_index);
            }
        }
    }

    index_min = soc_mem_index_min(unit, EGR_TX_PROT_GROUP_TABLEm);
    index_max = soc_mem_index_max(unit, EGR_TX_PROT_GROUP_TABLEm);
    for (idx = index_min; idx <= index_max; idx++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_TX_PROT_GROUP_TABLEm,
                    MEM_BLOCK_ANY, idx, &egr_tx_prot_group));

        soc_mem_field_get(unit, EGR_TX_PROT_GROUP_TABLEm,
                (uint32 *)&egr_tx_prot_group, DROP_BITMAPf, buf);
        for (i = 0; i < BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY; i++) {
            value = ((buf[i / 32] >> (i % 32)) & 0x1);
            if (value != 0) {
                bit_index = (((idx & 0xF) << 7) | (i & 0x7F));
                _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_SET(unit, bit_index);
            }
        }
    }

    index_min = soc_mem_index_min(unit, RX_PROT_GROUP_TABLEm);
    index_max = soc_mem_index_max(unit, RX_PROT_GROUP_TABLEm);
    for (idx = index_min; idx <= index_max; idx++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, RX_PROT_GROUP_TABLEm,
                            MEM_BLOCK_ANY, idx, &rx_prot_group_entry));
        soc_mem_field_get(unit, RX_PROT_GROUP_TABLEm,
                          (uint32 *)&rx_prot_group_entry,
                          DROP_DATA_ENABLE_BITMAPf, buf);
        for (i = 0; i < BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY; i++) {
            value = ((buf[i / 32] >> (i % 32)) & 0x1);
            if (value != 0) {
                bit_index = (((idx & 0x7) << 7) | (i & 0x7F));
                _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(unit, bit_index);
            }
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcmi_failover_multi_level_failover_offset_init
 * Purpose:
 *      Initializes offsset table for multi level proteciton
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_failover_multi_level_failover_offset_init(int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;
    initial_prot_offset_table_entry_t offset_entry;
    int offset = 0;
    int num_entries = 0;

    num_entries  = soc_mem_index_count(unit, INITIAL_PROT_OFFSET_TABLEm);

    for (; i<num_entries; i++) {
        offset = 0;

        /*
         * Offset will be zero if protection is not enabled
         * at both the levels.
         */
        if ((i & (1 << LVL_1_PROT_ENABLE)) ||
            (i & (1 << LVL_2_PROT_ENABLE))) {

            if ((!(i & (1 << LVL_2_1_REP_ENABLE))) &&
                (!(i & (1 << LVL_2_0_REP_ENABLE))) &&
                (!(i & (1 << LVL_1_REP_ENABLE)))) {

                continue;
            }

            /*
             * if protection at both levels are not
             * enabled, then just put offset as 0.
             */
            if ((i & (1 << LVL_1_PROT_MODE)) &&
                (i & (1 << LVL_2_PROT_MODE))) {
                /*
                 * Both protection modes are 1+1.
                 * The only offset used will be 0.
                 */
                offset = 0;
            } else  if (i & (1 << LVL_2_PROT_MODE)) {
                /*
                 * THis is 1:1 at level-1 and 1+1 at level-2.
                 */
                if (i & (1 << LVL_1_REP_ENABLE)) {
                        offset = 1;
                }
            } else if (i & (1 << LVL_1_PROT_MODE)) {
                /*
                 * This is 1+1 at level 1 and 1:1 at level-2.
                 */
                if ((i & (1 << LVL_2_1_REP_ENABLE)) &&
                    (i & (1 << LVL_2_0_REP_ENABLE))) {

                    offset = 3;
                } else if (i & (1 << LVL_2_1_REP_ENABLE)) {
                    offset = 2;
                } else if (i & (1 << LVL_2_0_REP_ENABLE)) {
                    offset = 1;
                }

            } else {
                /*
                 * This is 1:1 at level 1 and 1:1 at level-2.
                 */
                if (i & (1 << LVL_1_REP_ENABLE)) {

                    if (i & (1 << LVL_2_1_REP_ENABLE)) {
                        offset = 3;
                    } else {
                        offset = 2;
                    }
                } else {
                    if (i & (1 << LVL_2_0_REP_ENABLE)) {
                        offset = 1;
                    }
                }
            }
        }

        soc_mem_field32_set(unit, INITIAL_PROT_OFFSET_TABLEm,
                &offset_entry, OFFSETf, offset);

        rv = soc_mem_write(unit, INITIAL_PROT_OFFSET_TABLEm,
                MEM_BLOCK_ALL, i, &offset_entry);

        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_failover_init
 * Purpose:
 *      Initialize the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_failover_init (int unit)
{
    int rv = BCM_E_NONE;
    int num_prot_group, num_prot_nhi;
    int egress_num_prot_group;
    int ingress_num_prot_group;
    _bcm_td2p_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    int num_double_prot_nhi = 0;
#endif

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
    }

    /*
     * allocate resources
     */
    if (failover_info->initialized) {
        BCM_IF_ERROR_RETURN(bcm_td2p_failover_cleanup(unit));
    }

    num_prot_group = (soc_mem_index_count(unit, prot_group_mem)
            * BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    num_prot_nhi = soc_mem_index_count(unit, INITIAL_PROT_NHI_TABLEm);

    failover_info->prot_group_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_prot_group), "prot_group_bitmap");
    if (failover_info->prot_group_bitmap == NULL) {
        _bcm_td2p_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

    failover_info->prot_nhi_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_prot_nhi), "prot_nhi_bitmap");
    if (failover_info->prot_nhi_bitmap == NULL) {
        _bcm_td2p_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

    /*Egress Prot Group*/
    egress_num_prot_group = (soc_mem_index_count(unit, EGR_TX_PROT_GROUP_TABLEm)
            * BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    failover_info->egress_prot_group_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(egress_num_prot_group),
                                              "egress_prot_group_bitmap");
    if (failover_info->egress_prot_group_bitmap == NULL) {
        _bcm_td2p_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

    /* Rx Prot Group */
    ingress_num_prot_group = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                              BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    failover_info->ingress_prot_group_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(ingress_num_prot_group),
                  "ingress_prot_group_bitmap");
    if (failover_info->ingress_prot_group_bitmap == NULL) {
        _bcm_td2p_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        num_double_prot_nhi =
            soc_mem_index_count(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm);

        failover_info->multi_level_failover_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_double_prot_nhi),
                      "ingress_prot_group_bitmap");
        if (failover_info->multi_level_failover_bitmap == NULL) {
            _bcm_td2p_failover_free_resource(unit, failover_info);
            return BCM_E_MEMORY;
        }

        if (failover_info->init_prot_to_multi_failover_id == NULL) {
            failover_info->init_prot_to_multi_failover_id =
                sal_alloc((num_prot_nhi * sizeof(int)),
                "init_prot_to_multi_failover_id");

            if (failover_info->init_prot_to_multi_failover_id == NULL) {
                _bcm_td2p_failover_free_resource(unit, failover_info);
                return BCM_E_MEMORY;
            }
        }
    }
#endif/*BCM_MULTI_LEVEL_FAILOVER_SUPPORT*/

    sal_memset(failover_info->prot_group_bitmap, 0,
            SHR_BITALLOCSIZE(num_prot_group));
    sal_memset(failover_info->prot_nhi_bitmap, 0,
            SHR_BITALLOCSIZE(num_prot_nhi));
    sal_memset(failover_info->egress_prot_group_bitmap, 0,
            SHR_BITALLOCSIZE(egress_num_prot_group));
    sal_memset(failover_info->ingress_prot_group_bitmap, 0,
            SHR_BITALLOCSIZE(ingress_num_prot_group));
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        sal_memset(failover_info->multi_level_failover_bitmap, 0,
            SHR_BITALLOCSIZE(num_double_prot_nhi));
        sal_memset(failover_info->init_prot_to_multi_failover_id, 0,
            num_prot_nhi * sizeof(int));

        /* Allocate space for multi level failover s/w state */
        bcmi_multi_level_sw_state[unit] = sal_alloc((num_double_prot_nhi *
            sizeof(bcmi_failover_multi_level_info_t)), "multi level failover");

        if (bcmi_multi_level_sw_state[unit] == NULL) {
            _bcm_td2p_failover_free_resource(unit, failover_info);
            return BCM_E_MEMORY;
        }
        sal_memset(bcmi_multi_level_sw_state[unit], 0,
            (num_double_prot_nhi *
            sizeof(bcmi_failover_multi_level_info_t)));

    }
#endif/*BCM_MULTI_LEVEL_FAILOVER_SUPPORT*/

    /* Create Failover  protection mutex. */
    failover_info->failover_mutex = sal_mutex_create("failover_mutex");
    if (!failover_info->failover_mutex) {
        _bcm_td2p_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if(SOC_WARM_BOOT(unit)) {
        rv = _bcm_td2p_failover_reinit(unit);
        if (BCM_FAILURE(rv)){
            _bcm_td2p_failover_free_resource(unit, failover_info);
            return rv;
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = bcm_td3_failover_reinit(unit);
            if (BCM_FAILURE(rv)) {
                _bcm_td2p_failover_free_resource(unit, failover_info);
                return rv;
            }
        }
#endif

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
        if (soc_feature(unit, soc_feature_hierarchical_protection)) {
            rv = bcmi_failover_reinit(unit);
            if (BCM_FAILURE(rv)){
                _bcm_td2p_failover_free_resource(unit, failover_info);
                return rv;
            }
        }
#endif /*BCM_MULTI_LEVEL_FAILOVER_SUPPORT*/
    } else {
        if (soc_feature(unit, soc_feature_hierarchical_protection)) {
            rv = bcmi_failover_wb_alloc(unit);
            if (BCM_FAILURE(rv)){
                _bcm_td2p_failover_free_resource(unit, failover_info);
                return rv;
            }
        }
#ifdef BCM_TRIDENT3_SUPPORT
        else if (SOC_IS_TRIDENT3X(unit)) {
            rv = bcm_td3_failover_alloc(unit);
            if (BCM_FAILURE(rv)){
                _bcm_td2p_failover_free_resource(unit, failover_info);
                return rv;
            }
        }
#endif
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        /* Initialize multi level protection offset table */
        rv = bcmi_failover_multi_level_failover_offset_init(unit);
        if (BCM_FAILURE(rv)){
            _bcm_td2p_failover_free_resource(unit, failover_info);
            return rv;
        }
    }
#endif /* BCM_MULTI_LEVEL_FAILOVER_SUPPORT */


    /* Mark the state as initialized */
    failover_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_failover_hw_clear
 * Purpose:
 *     Perform hw tables clean up for failover module. 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_failover_hw_clear (int unit)
{
    int i, rv, rv_error = BCM_E_NONE;
    int num_entry;
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
    }

    num_entry = (soc_mem_index_count(unit, prot_group_mem) *
            BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
        if (_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, i)) {
            rv = bcm_td2p_failover_destroy(unit, i);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    if(rv_error != BCM_E_NONE)
        return rv_error;

    num_entry = (soc_mem_index_count(unit, EGR_TX_PROT_GROUP_TABLEm) *
            BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */
    for (i = 1; i < num_entry; i++) {
        if (_BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(unit, i)) {
            /*Encap type is needed to destroy used failoverid*/
            _BCM_ENCAP_TYPE_IN_FAILOVER_ID(i,
                              _BCM_FAILOVER_1_1_MC_PROTECTION_MODE);
            rv = bcm_td2p_failover_destroy(unit, i);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                 BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    /* Index-0 is reserved */
    for (i = 1; i < num_entry; i++) {
        if (_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(unit, i)) {
            /* Encap type is needed to destroy used failoverid */
            _BCM_ENCAP_TYPE_IN_FAILOVER_ID(i, _BCM_FAILOVER_INGRESS);
            rv = bcm_td2p_failover_destroy(unit, i);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_td2p_failover_cleanup
 * Purpose:
 *      DeInit  the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_failover_cleanup (int unit)
{
    _bcm_td2p_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);
    int rv = BCM_E_UNAVAIL;

    if (FALSE == failover_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcm_td2p_failover_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_td2p_failover_hw_clear(unit);
    }

    /* Free software resources */
    (void) _bcm_td2p_failover_free_resource(unit, failover_info);

    bcm_td2p_failover_unlock(unit);

    /* Destroy protection mutex. */
    sal_mutex_destroy(failover_info->failover_mutex);

    /* Mark the state as uninitialized */
    failover_info->initialized = FALSE;

    /* Check for dependent failover state after failover cleanup */
    bcmi_failover_dependent_free_resources(unit);

    return rv;
}


/*
 * Function:
 *		_bcm_td2p_failover_get_prot_group_table_index
 * Purpose:
 *		Obtain Index into  INITIAL_PROT_GROUP_TABLE
 * Parameters:
 *		IN :  Unit
 *           OUT : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC int
_bcm_td2p_failover_get_prot_group_table_index (int unit, int *prot_index)
{
    int  i;
    int  num_entry;
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
    }

    num_entry = (soc_mem_index_count(unit, prot_group_mem) *
            BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
        if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, i)) {
            break;
        }
    }

    if (i == num_entry) {
        return BCM_E_RESOURCE;
    }
    _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, i);
    *prot_index = i;	 

    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_td2p_failover_clear_prot_group_table_entry
 * Purpose:
 *		Clear INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC void
_bcm_td2p_failover_clear_prot_group_table_entry (int unit,
        bcm_failover_t prot_index)
{
   _BCM_FAILOVER_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}

/*
 * Function:
 *		_bcm_td2p_failover_set_prot_group_table_entry
 * Purpose:
 *		Set INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC void
_bcm_td2p_failover_set_prot_group_table_entry (int unit,
        bcm_failover_t prot_index)
{
   _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, prot_index);
}

/*
 * Function:
 *		_bcm_td2p_failover_id_validate
 * Purpose:
 *		Validate the failover ID
 * Parameters:
 *           IN : failover_id
 * Returns:
 *		BCM_E_XXX
 */
STATIC int
_bcm_td2p_failover_id_validate (int unit, bcm_failover_t failover_id)
{
    int num_entry;
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
    }

    num_entry = (soc_mem_index_count(unit, prot_group_mem) *
            BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    if ((failover_id < 1) || (failover_id > num_entry)) {
        return BCM_E_PARAM;
    } else if (failover_id == num_entry) {
        return  BCM_E_RESOURCE;
    }
    if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *		_bcm_td2p_egress_failover_id_validate
 * Purpose:
 *		Validate the egress failover ID
 * Parameters:
 *           IN : failover_id
 * Returns:
 *		BCM_E_XXX
 */
STATIC int
_bcm_td2p_egress_failover_id_validate (int unit, bcm_failover_t failover_id)
{
    int num_entry;

    num_entry = (soc_mem_index_count(unit, EGR_TX_PROT_GROUP_TABLEm) *
            BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    if ((failover_id < 1) || (failover_id >= num_entry)) {
        return BCM_E_PARAM;
    } else if (failover_id == num_entry) {
        return  BCM_E_RESOURCE;
    }
    if (!_BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *		_bcm_td2p_failover_get_egress_prot_group_table_index
 * Purpose:
 *		Obtain Index into EGR_TX_PROT_GROUP_TABLE
 * Parameters:
 *		IN :  Unit
 *           OUT : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC int
_bcm_td2p_failover_get_egress_prot_group_table_index (int unit,
                                             int  *prot_index)
{
    int  i;
    int  num_entry;

    num_entry = (soc_mem_index_count(unit, EGR_TX_PROT_GROUP_TABLEm) *
            BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */
    for (i = 1; i < num_entry; i++) {
        if (!_BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(unit, i)) {
            break;
        }
    }

    if (i == num_entry) {
        return BCM_E_RESOURCE;
    }

    *prot_index = i;
    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_td2p_failover_clear_egress_prot_group_table_index
 * Purpose:
 *		Clear EGR_TX_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC void
_bcm_td2p_failover_clear_egress_prot_group_table_index (int unit,
                                      bcm_failover_t  prot_index)
{
   _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}

/*
 * Function:
 *		_bcm_td2p_failover_set_egress_prot_group_table_index
 * Purpose:
 *		Set EGR_TX_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC void
_bcm_td2p_failover_set_egress_prot_group_table_index (int unit,
                                    bcm_failover_t  prot_index)
{
   _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_SET(unit,prot_index);
}

/*
 * Function:
 *     _bcm_td2p_failover_ingress_id_validate
 * Purpose:
 *     Validate the ingress failover ID
 * Parameters:
 *     unit        : (IN) Device Unit Number
 *     failover_id : (IN) Ingress failover ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_failover_ingress_id_validate(int unit,
                                       bcm_failover_t failover_id)
{
    int num_entry;

    num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                 BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    if ((failover_id < 1) || (failover_id >= num_entry)) {
        return BCM_E_PARAM;
    }

    if (!_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_failover_get_ingress_prot_group_table_index
 * Purpose:
 *     Obtain Index into RX_PROT_GROUP_TABLE
 * Parameters:
 *     unit        : (IN)  Device Unit Number
 *     prot_index  : (OUT) RX_PROT_GROUP_TABLE bit index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_failover_get_ingress_prot_group_table_index(int unit,
                                                      int *prot_index)
{
    int  i;
    int  num_entry;

    num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                 BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */
    for (i = 1; i < num_entry; i++) {
        if (!_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(unit, i)) {
            break;
        }
    }

    if (i == num_entry) {
        return BCM_E_RESOURCE;
    }

    *prot_index = i;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_failover_clear_ingress_prot_group_table_index
 * Purpose:
 *     Clear RX_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *     unit        : (IN) Device Unit Number
 *     prot_index  : (IN) RX_PROT_GROUP_TABLE bit index
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_td2p_failover_clear_ingress_prot_group_table_index(
    int unit, bcm_failover_t prot_index)
{
   _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}

/*
 * Function:
 *     _bcm_td2p_failover_set_ingress_prot_group_table_index
 * Purpose:
 *     Set RX_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *     unit        : (IN) Device Unit Number
 *     prot_index  : (IN) RX_PROT_GROUP_TABLE bit index
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_td2p_failover_set_ingress_prot_group_table_index(int unit,
                                                      bcm_failover_t prot_index)
{
   _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(unit, prot_index);
}

/*
 * Function:
 *      _bcm_failover_multi_level_index_get
 * Purpose:
 *      Obtain Index for multi level failover
 * Parameters:
 *      IN :  Unit
 *      OUT : index
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_failover_multi_level_index_get(int unit, int *index)
{
    int  i;
    int  num_entry;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_UNAVAIL;
    }
    num_entry = (soc_mem_index_count(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm));

    /* Index-0 is reserved */
    for (i = 1; i < num_entry; i++) {
        if (!_BCM_FAILOVER_MULTI_LEVEL_MAP_USED_GET(unit, i)) {
            break;
        }
    }

    if (i == num_entry) {
        return BCM_E_RESOURCE;
    }

    *index = i;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_failover_multi_level_index_set
 * Purpose:
 *      Obtain Index for multi level failover
 * Parameters:
 *      IN :  Unit
 *      IN : index
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
_bcm_failover_multi_level_index_set(int unit, int index)
{

    _BCM_FAILOVER_MULTI_LEVEL_MAP_USED_SET(unit, index);
}

/*
 * Function:
 *      _bcm_failover_multi_level_index_validate
 * Purpose:
 *      Obtain Index for multi level failover
 * Parameters:
 *      IN :  Unit
 *      IN : index
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_failover_multi_level_index_validate(int unit, int index)
{
    int  num_entry;

    /* Get the max range */
    num_entry = (soc_mem_index_count(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm));

    /* make sure the given id is valid */
    if ((index < 1) || (index >= num_entry)) {
                return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_failover_multi_level_index_clear
 * Purpose:
 *      Obtain Index for multi level failover
 * Parameters:
 *      IN :  Unit
 *      IN : index
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
_bcm_failover_multi_level_index_clear(int unit, int index)
{

    _BCM_FAILOVER_MULTI_LEVEL_MAP_USED_CLR(unit, index);
}

/*
 * Function:
 *		bcm_td2p_failover_create
 * Purpose:
 *		Create a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  flags
 *           OUT :  failover_id
 * Returns:
 *		BCM_E_XXX
 */
int 
bcm_td2p_failover_create (int unit, uint32 flags, bcm_failover_t *failover_id)
{
    int rv = BCM_E_UNAVAIL;
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    egr_tx_prot_group_table_entry_t egr_tx_prot_group;
    int  num_entry,local_failover_id;
    int  failover_type;
    uint32 table_index;
    uint32 bit_index;
    uint32 buf[4];
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;
#if defined(BCM_MULTI_LEVEL_FAILOVER_SUPPORT)
    tx_prot_group_1_1_table_entry_t tx_prot_bitmap_entry;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
        prot_group_entry = &tx_prot_bitmap_entry;
        replace_enable_field = REPLACE_ENABLEf;
    }
#endif

    if (failover_id == NULL) {
        return BCM_E_PARAM;
    }

    /* Check for unsupported Flag */
    if (flags & (~(BCM_FAILOVER_REPLACE |
                   BCM_FAILOVER_WITH_ID |
                   BCM_FAILOVER_ENCAP |
                   BCM_FAILOVER_INGRESS|
                   BCM_FAILOVER_MULTI_LEVEL_TYPE))) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_FAILOVER_WITH_ID) || (flags & BCM_FAILOVER_REPLACE)) {

        if ((flags & BCM_FAILOVER_ENCAP) &&
            (!(flags & BCM_FAILOVER_MULTI_LEVEL_TYPE)) &&
            (!(flags & BCM_FAILOVER_INGRESS))) {
            local_failover_id = *failover_id;

            /*Allow failoverIds of type _BCM_FAILOVER_1_1_MC_PROTECTION_MODE*/
            _BCM_GET_FAILOVER_TYPE(local_failover_id, failover_type);
            if (!(failover_type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE)) {
                return BCM_E_PARAM;
            }

            _BCM_GET_FAILOVER_ID(local_failover_id);
            /* make sure the given id is valid */
            num_entry = (soc_mem_index_count(unit, EGR_TX_PROT_GROUP_TABLEm) *
                              BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
            if ((local_failover_id < 1) || (local_failover_id >= num_entry)) {
                return BCM_E_PARAM;
            }
            if (flags & BCM_FAILOVER_WITH_ID) {
                if (_BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(unit, local_failover_id)) {
                    return BCM_E_EXISTS;
                } else {
                    _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_SET(unit, local_failover_id);
                }
            }
            if (flags & BCM_FAILOVER_REPLACE) {
                if (!_BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(unit, local_failover_id)) {
                    return BCM_E_NOT_FOUND;
                }
            }
        } else if ((flags & BCM_FAILOVER_INGRESS) &&
                    (!(flags & BCM_FAILOVER_MULTI_LEVEL_TYPE)) &&
                    (!(flags & BCM_FAILOVER_ENCAP))) {
            local_failover_id = *failover_id;

            /* Allow failoverIds of type _BCM_FAILOVER_INGRESS */
            _BCM_GET_FAILOVER_TYPE(local_failover_id, failover_type);
            if (!(failover_type & _BCM_FAILOVER_INGRESS)) {
                return BCM_E_PARAM;
            }

            _BCM_GET_FAILOVER_ID(local_failover_id);
            /* make sure the given id is valid */
            num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                         BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
            if ((local_failover_id < 1) || (local_failover_id >= num_entry)) {
                return BCM_E_PARAM;
            }

            if (flags & BCM_FAILOVER_WITH_ID) {
                if (_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(
                    unit, local_failover_id)) {
                    return BCM_E_EXISTS;
                } else {
                    _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(
                        unit, local_failover_id);
                }
            }

            if (flags & BCM_FAILOVER_REPLACE) {
                if (!_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(
                    unit, local_failover_id)) {
                    return BCM_E_NOT_FOUND;
                }
            }
        } else if (flags & BCM_FAILOVER_MULTI_LEVEL_TYPE) {
            local_failover_id = *failover_id;

            /* Allow failoverIds of type _BCM_FAILOVER_INGRESS */
            _BCM_GET_FAILOVER_TYPE(local_failover_id, failover_type);
            if (!(failover_type & _BCM_FAILOVER_MULTI_LEVEL)) {
                return BCM_E_PARAM;
            }

            _BCM_GET_FAILOVER_ID(local_failover_id);
            rv = _bcm_failover_multi_level_index_validate(unit, local_failover_id);
            if (BCM_SUCCESS(rv)) {
                if (flags & BCM_FAILOVER_REPLACE) {
                    if (!_BCM_FAILOVER_MULTI_LEVEL_MAP_USED_GET(
                        unit, local_failover_id)) {
                        return BCM_E_NOT_FOUND;
                    }
                }
                if (flags & BCM_FAILOVER_WITH_ID) {
                    if (_BCM_FAILOVER_MULTI_LEVEL_MAP_USED_GET(unit, local_failover_id)) {
                        if (!(flags & BCM_FAILOVER_REPLACE)) {
                            return BCM_E_EXISTS;
                        }
                    } else {
                        _bcm_failover_multi_level_index_set(unit, local_failover_id);
                        if (flags & BCM_FAILOVER_ENCAP) {
                            bcmi_failover_multi_level_info_t *ml_state;
                            ml_state =
                                (&(bcmi_multi_level_sw_state[unit][local_failover_id]));

                        /* Program everything in the software state */
                            ml_state->parent_failover_type =
                                _BCM_FAILOVER_1_1_MC_PROTECTION_MODE;
                        }
                    }
                }
                /* Nothing to be done for this type of failover. return */
                return rv;
            } else {
                return BCM_E_PARAM;
            }
        } else if ((flags & BCM_FAILOVER_ENCAP) && (flags & BCM_FAILOVER_INGRESS)) {
            local_failover_id = *failover_id;

            /*
             * Allow failoverIds of type _BCM_FAILOVER_INGRESS
             * and BCM_FAILOVER_ENCAP
             */
            _BCM_GET_FAILOVER_TYPE(local_failover_id, failover_type);
            if (!(failover_type == _BCM_FAILOVER_ING_MC_PROTECTION_MODE)) {
                return BCM_E_PARAM;
            }

            _BCM_GET_FAILOVER_ID(local_failover_id);

            /* make sure the given id is valid */
            num_entry = (soc_mem_index_count(unit, prot_group_mem) *
                              BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
            if ((local_failover_id < 1) || (local_failover_id >= num_entry)) {
                return BCM_E_PARAM;
            }
            if (flags & BCM_FAILOVER_WITH_ID) {
                if (_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, local_failover_id)) {
                    if (!(flags & BCM_FAILOVER_REPLACE)) {
                        return BCM_E_EXISTS;
                    }
                } else {
                    _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, local_failover_id);
                }
            }
            if (flags & BCM_FAILOVER_REPLACE) {
                if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, local_failover_id)) {
                    return BCM_E_NOT_FOUND;
                }
            }
        } else {
            /* make sure the given id is valid */
            num_entry = (soc_mem_index_count(unit, prot_group_mem) *
                              BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
            if ((*failover_id < 1) || (*failover_id >= num_entry)) {
                return BCM_E_PARAM;
            }
            if (flags & BCM_FAILOVER_WITH_ID) {
                if (_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, *failover_id)) {
                    return BCM_E_EXISTS;
                } else {
                    _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, *failover_id);
                }
            }
            if (flags & BCM_FAILOVER_REPLACE) {
                if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, *failover_id)) {
                    return BCM_E_NOT_FOUND;
                }
            }
        }
        rv = BCM_E_NONE;
    } else if (!flags) {
        /* Get Index */
        rv = _bcm_td2p_failover_get_prot_group_table_index(unit, failover_id);
    } else if ((flags & BCM_FAILOVER_ENCAP) && (flags & BCM_FAILOVER_INGRESS)) {
        rv = _bcm_td2p_failover_get_prot_group_table_index(unit, failover_id);
        _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id,
            _BCM_FAILOVER_ING_MC_PROTECTION_MODE);
        if (!(BCM_SUCCESS(rv))) {
            return rv;
        }
    } else if ((flags & BCM_FAILOVER_ENCAP) &&
                (!(flags & BCM_FAILOVER_MULTI_LEVEL_TYPE))) {
        rv = _bcm_td2p_failover_get_egress_prot_group_table_index(unit, failover_id);
        _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_SET(unit, *failover_id);
        _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_1_1_MC_PROTECTION_MODE);
    } else if ((flags & BCM_FAILOVER_INGRESS) &&
                    (!(flags & BCM_FAILOVER_MULTI_LEVEL_TYPE))) {
        rv = _bcm_td2p_failover_get_ingress_prot_group_table_index(unit,
                                                                   failover_id);
        _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(unit, *failover_id);
        _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_INGRESS);
    } else if (flags & BCM_FAILOVER_MULTI_LEVEL_TYPE) {
        rv = _bcm_failover_multi_level_index_get(unit, failover_id);
        if (BCM_SUCCESS(rv)) {
            _bcm_failover_multi_level_index_set(unit, *failover_id);
            if (flags & BCM_FAILOVER_ENCAP) {
                bcmi_failover_multi_level_info_t *ml_state;
                ml_state =
                    (&(bcmi_multi_level_sw_state[unit][*failover_id]));

                /* Program everything in the software state */
                ml_state->parent_failover_type =
                    _BCM_FAILOVER_1_1_MC_PROTECTION_MODE;
            }
            _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id,
                _BCM_FAILOVER_MULTI_LEVEL);
        }
        /* Nothing to be done for this type of failover scheme. return */
        return rv;
    }

    if (BCM_SUCCESS(rv)) {
        if ((flags & BCM_FAILOVER_ENCAP) && (!(flags & BCM_FAILOVER_INGRESS))) {
            local_failover_id = *failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            table_index = ((local_failover_id >> 7) & 0xF);
            bit_index = (local_failover_id & 0x7F);
            BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_TX_PROT_GROUP_TABLEm,
                        MEM_BLOCK_ANY, table_index, &egr_tx_prot_group));
            sal_memcpy(buf, &egr_tx_prot_group, sizeof(buf));

            buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));

            soc_mem_field_set(unit, EGR_TX_PROT_GROUP_TABLEm,
                    (uint32 *)&egr_tx_prot_group, DROP_BITMAPf,buf);

            rv = soc_mem_write(unit, EGR_TX_PROT_GROUP_TABLEm,
                    MEM_BLOCK_ALL, table_index, &egr_tx_prot_group);
            if (rv < 0) {
                _bcm_td2p_failover_clear_egress_prot_group_table_index(unit,
                                                          local_failover_id);
                return rv;
            }
        } else if ((flags & BCM_FAILOVER_INGRESS) && (!(flags & BCM_FAILOVER_ENCAP))) {

            local_failover_id = *failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            table_index = ((local_failover_id >> 7) & 0x7F);
            bit_index = (local_failover_id & 0x7F);
            BCM_IF_ERROR_RETURN (
                soc_mem_read(unit, RX_PROT_GROUP_TABLEm, MEM_BLOCK_ANY,
                             table_index, &rx_prot_group_entry));
            sal_memcpy(buf, &rx_prot_group_entry, sizeof(buf));

            buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));

            soc_mem_field_set(unit, RX_PROT_GROUP_TABLEm,
                              (uint32 *)&rx_prot_group_entry,
                              DROP_DATA_ENABLE_BITMAPf, buf);

            rv = soc_mem_write(unit, RX_PROT_GROUP_TABLEm, MEM_BLOCK_ALL,
                               table_index, &rx_prot_group_entry);
            if (rv < 0) {
                _bcm_td2p_failover_clear_ingress_prot_group_table_index(
                    unit, local_failover_id);
                return rv;
            }
        } else {
            local_failover_id = *failover_id;
            _BCM_GET_FAILOVER_ID(*failover_id);
            /* Init table entry */
            table_index = ((*failover_id >> 7) & BCMI_TX_PROT_GROUP_MASK(unit));
            bit_index = (*failover_id & 0x7F);
            BCM_IF_ERROR_RETURN (soc_mem_read(unit, prot_group_mem,
                        MEM_BLOCK_ANY, table_index, prot_group_entry));
            sal_memcpy(buf, prot_group_entry, sizeof(buf));

            buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));

            soc_mem_field_set(unit, prot_group_mem,
                    (uint32 *)prot_group_entry, replace_enable_field, buf);

            rv = soc_mem_write(unit, prot_group_mem,
                    MEM_BLOCK_ALL, table_index, prot_group_entry);
            if (rv < 0) {
                _bcm_td2p_failover_clear_prot_group_table_entry(unit, *failover_id);
                return BCM_E_RESOURCE;
            }
            *failover_id = local_failover_id;
        }
    }

    return rv;
}

/*
 * Function:
 *		bcm_td2_failover_destroy
 * Purpose:
 *		Destroy a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 * Returns:
 *		BCM_E_XXX
 */
int 
bcm_td2p_failover_destroy (int unit, bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
    int type = _BCM_FAILOVER_DEFAULT_MODE;
    int local_failover_id=0;
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    egr_tx_prot_group_table_entry_t egr_tx_prot_group;
    uint32 table_index;
    uint32 bit_index;
    uint32 buf[4];
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    bcmi_failover_multi_level_info_t *local_multi_level_state;
    void *prot_group_entry = &tx_init_prot_group_entry;
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    tx_prot_group_1_table_entry_t tx_prot_group_entry;
    tx_prot_group_1_1_table_entry_t tx_prot_bitmap_entry;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
        prot_group_entry = &tx_prot_bitmap_entry;
        replace_enable_field = REPLACE_ENABLEf;
    }
#endif

    /*Check failover id type Ingress or Egress Encap*/
    _BCM_GET_FAILOVER_TYPE(failover_id, type);
    if ((type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) &&
            (!(type & _BCM_FAILOVER_MULTI_LEVEL))) {

        local_failover_id = failover_id;
        _BCM_GET_FAILOVER_ID(local_failover_id);

        rv = _bcm_td2p_egress_failover_id_validate(unit, local_failover_id);
        BCM_IF_ERROR_RETURN(rv);

        /*Release software index*/
        _bcm_td2p_failover_clear_egress_prot_group_table_index(unit,
                                                         local_failover_id);

        /*clear hw entry*/
        table_index = ((local_failover_id >> 7) & 0xF);
        bit_index = (local_failover_id & 0x7F);
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_TX_PROT_GROUP_TABLEm,
                    MEM_BLOCK_ANY, table_index, &egr_tx_prot_group));

        sal_memcpy(buf, &egr_tx_prot_group, sizeof(buf));

        buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
        soc_mem_field_set(unit, EGR_TX_PROT_GROUP_TABLEm,
                (uint32 *)&egr_tx_prot_group, DROP_BITMAPf, buf);

        rv = soc_mem_write(unit, EGR_TX_PROT_GROUP_TABLEm, MEM_BLOCK_ALL,
                table_index, &egr_tx_prot_group);
        if (rv < 0) {
            _bcm_td2p_failover_set_egress_prot_group_table_index(unit,
                                                       local_failover_id);
            return rv;
        }
    } else if ((type & _BCM_FAILOVER_INGRESS) &&
            (!(type == _BCM_FAILOVER_MULTI_LEVEL))) {
        local_failover_id = failover_id;
        _BCM_GET_FAILOVER_ID(local_failover_id);

        rv = _bcm_td2p_failover_ingress_id_validate(unit, local_failover_id);
        BCM_IF_ERROR_RETURN(rv);

        /*Release software index*/
        _bcm_td2p_failover_clear_ingress_prot_group_table_index(
            unit, local_failover_id);
        /*clear hw entry*/
        table_index = ((local_failover_id >> 7) & 0x7F);
        bit_index = (local_failover_id & 0x7F);
        BCM_IF_ERROR_RETURN (
            soc_mem_read(unit, RX_PROT_GROUP_TABLEm,
                         MEM_BLOCK_ANY, table_index, &rx_prot_group_entry));

        sal_memcpy(buf, &rx_prot_group_entry, sizeof(buf));

        buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
        soc_mem_field_set(unit, RX_PROT_GROUP_TABLEm,
                          (uint32 *)&rx_prot_group_entry,
                          DROP_DATA_ENABLE_BITMAPf, buf);

        rv = soc_mem_write(unit, RX_PROT_GROUP_TABLEm, MEM_BLOCK_ALL,
                table_index, &rx_prot_group_entry);
        if (rv < 0) {
            _bcm_td2p_failover_set_ingress_prot_group_table_index(
                unit, local_failover_id);
            return rv;
        }
    } else if (type & _BCM_FAILOVER_MULTI_LEVEL) {
        local_failover_id = failover_id;
        _BCM_GET_FAILOVER_ID(local_failover_id);

        rv = _bcm_failover_multi_level_index_validate(unit, local_failover_id);
        BCM_IF_ERROR_RETURN(rv);
        local_multi_level_state =
            (&(bcmi_multi_level_sw_state[unit][local_failover_id]));

        local_multi_level_state->parent_failover_id = 0;
        local_multi_level_state->child_group_1 = 0;
        local_multi_level_state->child_group_2 = 0;

        _bcm_failover_multi_level_index_clear(unit, local_failover_id);

    } else if (type == _BCM_FAILOVER_ING_MC_PROTECTION_MODE) {
        local_failover_id = failover_id;
        _BCM_GET_FAILOVER_ID(local_failover_id);

        rv = _bcm_td2p_failover_id_validate(unit, local_failover_id);
        BCM_IF_ERROR_RETURN(rv);

        if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, local_failover_id)) {
            return BCM_E_NOT_FOUND;
        }

        /*Release index*/
        _bcm_td2p_failover_clear_prot_group_table_entry(unit, local_failover_id);

        /* Clear entry */
        table_index = ((local_failover_id >> 7) & BCMI_TX_PROT_GROUP_MASK(unit));
        bit_index = (local_failover_id & 0x7F);
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, prot_group_mem,
                    MEM_BLOCK_ANY, table_index, prot_group_entry));

        sal_memcpy(buf, prot_group_entry, sizeof(buf));

        buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
        soc_mem_field_set(unit, prot_group_mem,
                (uint32 *)prot_group_entry, replace_enable_field, buf);

        rv = soc_mem_write(unit, prot_group_mem, MEM_BLOCK_ALL,
                table_index, prot_group_entry);

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT

        if (soc_feature(unit, soc_feature_hierarchical_protection)) {
            if (type == _BCM_FAILOVER_ING_MC_PROTECTION_MODE) {
                /* Set Mode for level-2 protection group. */
                 sal_memset(&tx_prot_group_entry, 0, sizeof(tx_prot_group_1_table_entry_t));

                BCM_IF_ERROR_RETURN (soc_mem_write(unit, TX_PROT_GROUP_1_TABLEm,
                    MEM_BLOCK_ALL, local_failover_id, &tx_prot_group_entry));
            }
        }
#endif
        if (rv < 0) {
            _bcm_td2p_failover_set_prot_group_table_entry(unit, failover_id);
            return BCM_E_RESOURCE;
        }
    } else {
        rv = _bcm_td2p_failover_id_validate(unit, failover_id);
        BCM_IF_ERROR_RETURN(rv);

        if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
            return BCM_E_NOT_FOUND;
        }

        /*Release index*/
        _bcm_td2p_failover_clear_prot_group_table_entry(unit, failover_id);

        /* Clear entry */
        table_index = ((failover_id >> 7) & BCMI_TX_PROT_GROUP_MASK(unit));
        bit_index = (failover_id & 0x7F);
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, prot_group_mem,
                    MEM_BLOCK_ANY, table_index, prot_group_entry));

        sal_memcpy(buf, prot_group_entry, sizeof(buf));

        buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
        soc_mem_field_set(unit, prot_group_mem,
                (uint32 *)prot_group_entry, replace_enable_field, buf);

        rv = soc_mem_write(unit, prot_group_mem, MEM_BLOCK_ALL,
                table_index, prot_group_entry);
        if (rv < 0) {
            _bcm_td2p_failover_set_prot_group_table_entry(unit, failover_id);
            return BCM_E_RESOURCE;
        }
    }

    return rv;
}

STATIC int
_bcm_td2p_failover_nhi_get(int unit, bcm_gport_t port, int *nh_index)
{
    int vp=0xFFFF;
    ing_dvp_table_entry_t dvp;

    if (!BCM_GPORT_IS_MPLS_PORT(port) &&
            !BCM_GPORT_IS_MIM_PORT(port) ) {
        return BCM_E_PARAM;
    }

    if ( BCM_GPORT_IS_MPLS_PORT(port)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
    } else if ( BCM_GPORT_IS_MIM_PORT(port)) {
        vp = BCM_GPORT_MIM_PORT_ID_GET(port);
    }

    if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    /* Next-hop index is used for multicast replication */
    *nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    return BCM_E_NONE;
}

/*
 * Function:
 *        bcm_td2p_failover_status_set
 * Purpose:
 *        Set the parameters for a failover object
 * Parameters:
 *        IN :  unit
 *           IN :  failover_id
 *           IN :  value
 * Returns:
 *        BCM_E_XXX
 */
int
bcm_td2p_failover_status_set (int unit,
        bcm_failover_element_t *failover,
        int value)
{
    int rv = BCM_E_UNAVAIL;
    int nh_index;
    int local_failover_id=0;
    uint32 table_index = 0;
    uint32 bit_index = 0;
    uint32 buf[4];
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    initial_prot_nhi_table_1_entry_t   prot_nhi_1_entry;
    egr_tx_prot_group_table_entry_t egr_tx_prot_group;
    egr_l3_next_hop_1_entry_t egr_l3_nhop_1_entry;
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    tx_prot_group_1_table_entry_t tx_prot_group_entry;
    initial_prot_nhi_table_entry_t   prot_nhi_entry;
    initial_prot_nhi_double_wide_table_entry_t   prot_nhi_double_entry;
    tx_prot_group_1_1_table_entry_t tx_prot_bitmap_entry;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
        prot_group_entry = &tx_prot_bitmap_entry;
        replace_enable_field = REPLACE_ENABLEf;
    }
#endif

    if ((value < 0) || (value > 1)) {
        return BCM_E_PARAM;
    }

    if (failover->failover_id != BCM_FAILOVER_INVALID) {
        /* Check for UC or MC protection.
         * As we can have multiple flags set together, we
         * need to check if this particular failover id
         * is equal to a particular type.
         * if some other flag is set then it can
         * represent other failover type.
         */
        if (failover->flags == BCM_FAILOVER_ENCAP) {
            /*Egr tx prot group table*/
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            BCM_IF_ERROR_RETURN(
               _bcm_td2p_egress_failover_id_validate(unit,local_failover_id));

            table_index = ((local_failover_id >> 7) & 0xF);
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_TX_PROT_GROUP_TABLEm,
                        MEM_BLOCK_ANY, table_index, &egr_tx_prot_group));

            sal_memcpy(buf, &egr_tx_prot_group, sizeof(buf));
            if (value == 0) {
                buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
            } else {
                buf[bit_index / 32] |= (0x1 << (bit_index % 32));
            }

            soc_mem_field_set(unit, EGR_TX_PROT_GROUP_TABLEm,
                    (uint32 *)&egr_tx_prot_group, DROP_BITMAPf, buf);

            rv = soc_mem_write(unit, EGR_TX_PROT_GROUP_TABLEm,
                    MEM_BLOCK_ALL, table_index,  &egr_tx_prot_group);

        } else if (failover->flags == BCM_FAILOVER_INGRESS) {
            /* RX prot group table*/
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            BCM_IF_ERROR_RETURN(
               _bcm_td2p_failover_ingress_id_validate(unit, local_failover_id));

            table_index = ((local_failover_id >> 7) & 0x7F);
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, RX_PROT_GROUP_TABLEm,
                             MEM_BLOCK_ANY, table_index, &rx_prot_group_entry));

            sal_memcpy(buf, &rx_prot_group_entry, sizeof(buf));
            if (value == 0) {
                buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
            } else {
                buf[bit_index / 32] |= (0x1 << (bit_index % 32));
            }

            soc_mem_field_set(unit, RX_PROT_GROUP_TABLEm,
                              (uint32 *)&rx_prot_group_entry,
                              DROP_DATA_ENABLE_BITMAPf, buf);

            rv = soc_mem_write(unit, RX_PROT_GROUP_TABLEm, MEM_BLOCK_ALL,
                               table_index, &rx_prot_group_entry);
        } else {
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);

            /* Group protection for Port and Tunnel: Egress and Ingress */
            table_index = ((local_failover_id >> 7) & BCMI_TX_PROT_GROUP_MASK(unit));
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN(
                    _bcm_td2p_failover_id_validate(unit, local_failover_id));

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
            if (soc_feature(unit, soc_feature_hierarchical_protection)) {

                BCM_IF_ERROR_RETURN (soc_mem_read(unit, TX_PROT_GROUP_1_TABLEm,
                        MEM_BLOCK_ANY, local_failover_id, &tx_prot_group_entry));

                soc_mem_field32_set(unit, TX_PROT_GROUP_1_TABLEm,
                    (uint32 *)&tx_prot_group_entry, PROT_ENABLEf, 
                    ((failover->element_flags &
                    BCM_FAILOVER_ELEMENT_USE_SECONDARY) ? 1 : 0));

                BCM_IF_ERROR_RETURN (soc_mem_write(unit, TX_PROT_GROUP_1_TABLEm,
                    MEM_BLOCK_ALL, local_failover_id, &tx_prot_group_entry));
            }
#endif
            BCM_IF_ERROR_RETURN (soc_mem_read(unit, prot_group_mem,
                        MEM_BLOCK_ANY, table_index, prot_group_entry));

            sal_memcpy(buf, prot_group_entry, sizeof(buf));
            if (value == 0) {
                buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
            } else {
                buf[bit_index / 32] |= (0x1 << (bit_index % 32));
            }

            soc_mem_field_set(unit, prot_group_mem,
                    (uint32 *)prot_group_entry, replace_enable_field, buf);

            rv = soc_mem_write(unit, prot_group_mem,
                               MEM_BLOCK_ALL, table_index, prot_group_entry);
        }

    } else if (failover->intf != BCM_IF_INVALID) {
        /* Only Egress is applicable */
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, failover->intf)) {
            nh_index = failover->intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            nh_index = failover->intf  - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }

        if (failover->flags == BCM_FAILOVER_ENCAP) {
            /*Per NHI Egress Protection*/
            table_index = ((nh_index >> 7) & 0x1FF);
            bit_index = (nh_index & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOP_1m,
                        MEM_BLOCK_ANY, table_index, &egr_l3_nhop_1_entry));

            sal_memcpy(buf, &egr_l3_nhop_1_entry, sizeof(buf));
            if (value == 0) {
                buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
            } else {
                buf[bit_index / 32] |= (0x1 << (bit_index % 32));
            }

            soc_mem_field_set(unit, EGR_L3_NEXT_HOP_1m,
                    (uint32 *)&egr_l3_nhop_1_entry, DROP_BITMAPf, buf);

            rv = soc_mem_write(unit, EGR_L3_NEXT_HOP_1m,
                    MEM_BLOCK_ALL, table_index, &egr_l3_nhop_1_entry);
            if (rv < 0) {
                return rv;
            }
        } else {
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
            if (soc_feature(unit, soc_feature_hierarchical_protection)) {
                if (bcmi_l3_nh_multi_count_get(unit, nh_index) > 1) {

                    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
                        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                        MEM_BLOCK_ANY, nh_index/2, &prot_nhi_double_entry));

                    soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                        &prot_nhi_double_entry, PROT_ENABLEf, 
                        ((failover->element_flags &
                        BCM_FAILOVER_ELEMENT_USE_SECONDARY) ? 1 : 0));

                   rv = soc_mem_write(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                        MEM_BLOCK_ALL, nh_index/2, &prot_nhi_double_entry);

                   if (rv < 0) {
                       return BCM_E_NOT_FOUND;
                    }
                } else if (bcmi_l3_nh_multi_count_get(unit, nh_index - 1) > 1) {
                    /* cant set failover status on next to base nh index.
                     * If count is more than 1 then next NH is also used and 
                     * entry is read/written in double wide more.
                     * Reading in single mode will generate SER errors.
                     */
                    return BCM_E_NONE;
                } else {

                    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
                        INITIAL_PROT_NHI_TABLEm,
                        MEM_BLOCK_ANY, nh_index, &prot_nhi_entry));

                    soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm,
                        &prot_nhi_entry, PROT_ENABLEf, 
                        ((failover->element_flags &
                        BCM_FAILOVER_ELEMENT_USE_SECONDARY) ? 1 : 0));

                    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
                        MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

                    if (rv < 0) {
                        return BCM_E_NOT_FOUND;
                    }
                }
            }
#endif
            table_index = ((nh_index >> 7) & 0x1FF);
            bit_index = (nh_index & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                        MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry));

            sal_memcpy(buf, &prot_nhi_1_entry, sizeof(buf));
            if (value == 0) {
                buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
            } else {
                buf[bit_index / 32] |= (0x1 << (bit_index % 32));
            }

            soc_mem_field_set(unit, INITIAL_PROT_NHI_TABLE_1m,
                    (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);

            rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
                    MEM_BLOCK_ALL, table_index, &prot_nhi_1_entry);
            if (rv < 0) {
                return rv;
            }
        }

    } else if (failover->port != BCM_GPORT_INVALID) {
        /* Individual protection for Pseudo-wire: Egress and Ingress */
        BCM_IF_ERROR_RETURN (
                _bcm_td2p_failover_nhi_get(unit, failover->port , &nh_index));

        table_index = ((nh_index >> 7) & 0x1FF);
        bit_index = (nh_index & 0x7F);

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                    MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry));

        sal_memcpy(buf, &prot_nhi_1_entry, sizeof(buf));
        if (value == 0) {
            buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
        } else {
            buf[bit_index / 32] |= (0x1 << (bit_index % 32));
        }

        soc_mem_field_set(unit, INITIAL_PROT_NHI_TABLE_1m,
                (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);

        rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
                MEM_BLOCK_ALL, table_index, &prot_nhi_1_entry);

    }
    return rv;
}


/*
 * Function:
 *        bcm_td2p_failover_status_get
 * Purpose:
 *        Get the parameters for a failover object
 * Parameters:
 *        IN :  unit
 *           IN :  failover_id
 *           OUT :  value
 * Returns:
 *        BCM_E_XXX
 */
int
bcm_td2p_failover_status_get (int unit,
        bcm_failover_element_t *failover,
        int  *value)
{
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    initial_prot_nhi_table_1_entry_t   prot_nhi_1_entry;
    egr_tx_prot_group_table_entry_t egr_tx_prot_group;
    egr_l3_next_hop_1_entry_t egr_l3_nhop_1_entry;
    int nh_index;
    int local_failover_id=0;
    uint32 table_index = 0;
    uint32 bit_index = 0;
    uint32 buf[4];
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;
#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    tx_prot_group_1_table_entry_t tx_prot_group_entry;
    initial_prot_nhi_table_entry_t   prot_nhi_entry;
    initial_prot_nhi_double_wide_table_entry_t   prot_nhi_double_entry;
    tx_prot_group_1_1_table_entry_t tx_prot_bitmap_entry;

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
        prot_group_entry = &tx_prot_bitmap_entry;
        replace_enable_field = REPLACE_ENABLEf;
    }
#endif

    if (failover->failover_id != BCM_FAILOVER_INVALID) {
        /* As we can have multiple flags set together, we
         * need to check if this particular failover id
         * is equal to a particular type.
         * if some other flag is set then it can
         * represent other failover type.
         */
        if (failover->flags == BCM_FAILOVER_ENCAP) {
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            BCM_IF_ERROR_RETURN(
               _bcm_td2p_egress_failover_id_validate(unit,local_failover_id));

            table_index = ((local_failover_id >> 7) & 0xF);
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_TX_PROT_GROUP_TABLEm,
                        MEM_BLOCK_ANY, table_index, &egr_tx_prot_group));

            soc_mem_field_get(unit, EGR_TX_PROT_GROUP_TABLEm,
                    (uint32 *)&egr_tx_prot_group, DROP_BITMAPf, buf);
            *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);

        } else if (failover->flags == BCM_FAILOVER_INGRESS) {
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            BCM_IF_ERROR_RETURN(
               _bcm_td2p_failover_ingress_id_validate(unit, local_failover_id));

            table_index = ((local_failover_id >> 7) & 0x7F);
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN (
                soc_mem_read(unit, RX_PROT_GROUP_TABLEm,
                             MEM_BLOCK_ANY, table_index, &rx_prot_group_entry));

            soc_mem_field_get(unit, RX_PROT_GROUP_TABLEm,
                              (uint32 *)&rx_prot_group_entry,
                              DROP_DATA_ENABLE_BITMAPf, buf);
            *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);
        } else {
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);

            BCM_IF_ERROR_RETURN(
                    _bcm_td2p_failover_id_validate(unit, local_failover_id));

            table_index = ((local_failover_id >> 7) & BCMI_TX_PROT_GROUP_MASK(unit));
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, prot_group_mem,
                        MEM_BLOCK_ANY, table_index, prot_group_entry));

            soc_mem_field_get(unit, prot_group_mem,
                    (uint32 *)prot_group_entry, replace_enable_field, buf);
            *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
            if (soc_feature(unit, soc_feature_hierarchical_protection)) {

                BCM_IF_ERROR_RETURN (soc_mem_read(unit, TX_PROT_GROUP_1_TABLEm,
                        MEM_BLOCK_ANY, local_failover_id, &tx_prot_group_entry));

                if (soc_mem_field32_get(unit, TX_PROT_GROUP_1_TABLEm,
                    &tx_prot_group_entry, PROT_ENABLEf)) {

                    failover->element_flags |=
                        BCM_FAILOVER_ELEMENT_USE_SECONDARY;
                }
            }
#endif
        }

    } else if (failover->intf != BCM_IF_INVALID) {

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, failover->intf)) {
            nh_index = failover->intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            nh_index = failover->intf  - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }

        if (failover->flags == BCM_FAILOVER_ENCAP) {
            table_index = ((nh_index >> 7) & 0x1FF);
            bit_index = (nh_index & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOP_1m,
                        MEM_BLOCK_ANY, table_index, &egr_l3_nhop_1_entry));

            soc_mem_field_get(unit, EGR_L3_NEXT_HOP_1m,
                    (uint32 *)&egr_l3_nhop_1_entry, DROP_BITMAPf, buf);

            *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);

        } else {
            table_index = ((nh_index >> 7) & 0x1FF);
            bit_index = (nh_index & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                        MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry));

            soc_mem_field_get(unit, INITIAL_PROT_NHI_TABLE_1m,
                    (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);

            *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
            if (soc_feature(unit, soc_feature_hierarchical_protection)) {
                if (bcmi_l3_nh_multi_count_get(unit, nh_index) > 1) {

                    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
                        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                        MEM_BLOCK_ANY, nh_index/2, &prot_nhi_double_entry));

                    if (soc_mem_field32_get(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                        &prot_nhi_double_entry, PROT_ENABLEf)) {

                        failover->element_flags |=
                            BCM_FAILOVER_ELEMENT_USE_SECONDARY;
                    }
                } else if (bcmi_l3_nh_multi_count_get(unit, nh_index - 1) > 1) {
                    /* cant set failover status on next to base nh index.
                     * If count is more than 1 then next NH is also used and
                     * entry is read/written in double wide more.
                     * Reading in single mode will generate SER errors.
                     */
                    return BCM_E_NONE;
                } else {
                    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
                        INITIAL_PROT_NHI_TABLEm,
                        MEM_BLOCK_ANY, nh_index, &prot_nhi_entry));

                    if (soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
                        &prot_nhi_entry, PROT_ENABLEf)) {

                        failover->element_flags |=
                            BCM_FAILOVER_ELEMENT_USE_SECONDARY;
                    }
                }
            }
#endif
        }

    } else if (failover->port != BCM_GPORT_INVALID) {

        BCM_IF_ERROR_RETURN (
                _bcm_td2p_failover_nhi_get(unit, failover->port , &nh_index));

        table_index = ((nh_index >> 7) & 0x1FF);
        bit_index = (nh_index & 0x7F);

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                    MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry));

        soc_mem_field_get(unit, INITIAL_PROT_NHI_TABLE_1m,
                (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);

        *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *		bcm_td2p_failover_prot_nhi_set
 * Purpose:
 *		Set the parameters for PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  Primary Next Hop Index
 *           IN :  Protection Next Hop Index
 *           IN :  Failover Group Index
 * Returns:
 *		BCM_E_XXX
 */
int
bcm_td2p_failover_prot_nhi_set(int unit, uint32 flags, int nh_index,
        uint32 prot_nh_index, bcm_multicast_t  mc_group,
        bcm_failover_t failover_id)
{
    initial_prot_nhi_table_entry_t   prot_nhi_entry;
    int rv;
    int l3mc_index;
    int replacement_type_field = REPLACEMENT_TYPEf;
    int replacement_data_field = REPLACEMENT_DATAf;
    int len;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, 
                MEM_BLOCK_ANY, nh_index, &prot_nhi_entry));

    _BCM_GET_FAILOVER_ID(failover_id);

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm,
                    &prot_nhi_entry, PROT_ENABLEf, 1);

        replacement_type_field = PROT_MODEf;
        replacement_data_field = NEW_NHI_OR_MC_GROUPf;

        /*
         * As part of 2 level protection, failover id space is increased
         * to 16K. But we can still only pass first 1K failover Ids to
         * single level failover.
         * It should be okay as we can share failover id in single level
         *  and double level.
         */
        len = soc_mem_field_length(unit, INITIAL_PROT_NHI_TABLEm, PROT_GROUPf);
        if (failover_id >= (1 << len)) {
            return BCM_E_PARAM;
        }
    }

    /* if mc_group is valid */
    if (_BCM_MULTICAST_IS_SET(mc_group)) {
        l3mc_index = _BCM_MULTICAST_ID_GET(mc_group);
        if ((l3mc_index >= 0) && (l3mc_index <
                    soc_mem_index_count(unit, L3_IPMCm))) {
            soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
                    &prot_nhi_entry, replacement_data_field,
                    l3mc_index);
            soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
                    &prot_nhi_entry, replacement_type_field,
                    0x1);
        }
    } else {
        /* if prot_nh_index is valid */
        soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
                &prot_nhi_entry, replacement_data_field,
                prot_nh_index);
        soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
                &prot_nhi_entry, replacement_type_field,
                0x0);
    }

    /* if failover_id is valid */
    soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
            &prot_nhi_entry, PROT_GROUPf, 
            (uint32) failover_id);

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
            MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

    return rv;

}


/*
 * Function: bcm_td2p_failover_prot_nhi_get
 * Purpose:
 *		Get the parameters for PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  primary Next Hop Index
 *           OUT :  Failover Group Index
 *           OUT : Protection Next Hop Index
 * Returns:
 *		BCM_E_XXX
 */
int
bcm_td2p_failover_prot_nhi_get(int unit, int nh_index, 
        bcm_failover_t  *failover_id, int  *prot_nh_index,
        bcm_multicast_t  *mc_group)
{
    initial_prot_nhi_table_entry_t   prot_nhi_entry;
    uint32 replacement_type;
    int l3mc_index;
    int replacement_type_field = REPLACEMENT_TYPEf;
    int replacement_data_field = REPLACEMENT_DATAf;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, 
                MEM_BLOCK_ANY, nh_index, &prot_nhi_entry));

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        replacement_type_field = PROT_MODEf;
        replacement_data_field = NEW_NHI_OR_MC_GROUPf;
    }

    *failover_id = soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
            &prot_nhi_entry, PROT_GROUPf);

    replacement_type = soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
            &prot_nhi_entry, replacement_type_field);

    if (!replacement_type) {
        *prot_nh_index = soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
                &prot_nhi_entry, replacement_data_field);
    } else {
        l3mc_index = soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
                &prot_nhi_entry, replacement_data_field);
        _BCM_MULTICAST_GROUP_SET(*mc_group,
                _BCM_MULTICAST_TYPE_EGRESS_OBJECT,
                l3mc_index); 
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcm_td2p_failover_prot_nhi_update
 * Purpose:
 *    Update the next_hop  entry for failover
 * Parameters:
 *    IN :  unit
 *    IN :  Old Failover next hop index
 *    IN :  New Failover next hop index
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_td2p_failover_prot_nhi_update (int unit, int old_nh_index,
        int new_nh_index)
{
    int rv=BCM_E_NONE;
    int  num_entry, index;
    int prot_nh_index;
    uint32 replacement_type;
    int replacement_type_field = REPLACEMENT_TYPEf;
    int replacement_data_field = REPLACEMENT_DATAf;
    uint32 *pprot_nhi = NULL;
    uint32 *pprot_nhi_entry = NULL;
    int entry_size;

  num_entry = soc_mem_index_count(unit, INITIAL_PROT_NHI_TABLEm);

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        replacement_type_field = PROT_MODEf;
        replacement_data_field = NEW_NHI_OR_MC_GROUPf;
    }

    entry_size = soc_mem_entry_words(unit, INITIAL_PROT_NHI_TABLEm);
    pprot_nhi = soc_cm_salloc(unit, num_entry * entry_size * 4, "temp_buf");
    if (pprot_nhi == NULL) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, INITIAL_PROT_NHI_TABLEm, MEM_BLOCK_ANY, 0,
                            num_entry - 1, pprot_nhi);
    if (rv == SOC_E_NONE) {
        for (index = 0, pprot_nhi_entry = pprot_nhi; index < num_entry;
             index++) {
            if (soc_feature(unit, soc_feature_hierarchical_protection)) {
                if ((bcmi_l3_nh_multi_count_get(unit, index)) > 1) {
                    /* it is a double wide entry and therefore,
                     * we can not read it in single mode.
                     * skip the next index also as double wide mode uses
                     * two indexes.
                     */
                    index++;
                    pprot_nhi_entry += entry_size * 2;
                    continue;
                }
            }

            replacement_type = soc_mem_field32_get(unit,
                                                   INITIAL_PROT_NHI_TABLEm,
                                                   pprot_nhi_entry,
                                                   replacement_type_field);

            if (!replacement_type) {
                prot_nh_index = soc_mem_field32_get(unit,
                                                    INITIAL_PROT_NHI_TABLEm,
                                                    pprot_nhi_entry,
                                                    replacement_data_field);

                if (prot_nh_index == old_nh_index) {
                    soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm,
                                        pprot_nhi_entry, replacement_data_field,
                                        new_nh_index);
                }
            }
            pprot_nhi_entry += entry_size;
        }
        rv = soc_mem_write_range(unit, INITIAL_PROT_NHI_TABLEm, MEM_BLOCK_ANY,
                                 0, num_entry - 1, pprot_nhi);
    }
    if (pprot_nhi) {
        soc_cm_sfree(unit, pprot_nhi);
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_NONE;
    }
#if defined(BCM_MULTI_LEVEL_FAILOVER_SUPPORT)
    entry_size = soc_mem_entry_words(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm);
    pprot_nhi =
        soc_cm_salloc(unit, (num_entry / 2) * entry_size * 4, "temp_buf");
    if (pprot_nhi == NULL) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                            MEM_BLOCK_ANY, 0, (num_entry / 2) - 1, pprot_nhi);
    if (rv == SOC_E_NONE) {
        for (index = 0, pprot_nhi_entry = pprot_nhi; index < num_entry;
             index++) {
            if ((bcmi_l3_nh_multi_count_get(unit, index)) > 1) {

                replacement_type =
                    soc_mem_field32_get(unit,
                                        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                                        pprot_nhi_entry,
                                        replacement_type_field);
                if (!replacement_type) {
                    prot_nh_index =
                        soc_mem_field32_get(unit,
                                            INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                                            pprot_nhi_entry,
                                            replacement_data_field);

                    if (prot_nh_index == old_nh_index) {
                        soc_mem_field32_set(unit,
                                            INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                                            pprot_nhi_entry,
                                            replacement_data_field,
                                            new_nh_index);
                    }
                }
            }
            /* move index further if this is double wide entry */
            index++;
            pprot_nhi_entry += entry_size;
        }
        rv = soc_mem_write_range(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                                 MEM_BLOCK_ANY, 0,
                                 (num_entry / 2) - 1, pprot_nhi);
    }
    if (pprot_nhi) {
        soc_cm_sfree(unit, pprot_nhi);
    }
#endif

    return rv;
}

/*
 * Function:
 *        bcm_td2p_failover_prot_nhi_cleanup
 * Purpose:
 *        Cleanup  the  entry  for  PROT_NHI
 * Parameters:
 *        IN :  unit
 *           IN :  Primary Next Hop Index
 * Returns:
 *        BCM_E_XXX
 */
int
bcm_td2p_failover_prot_nhi_cleanup (int unit, int nh_index)
{
    initial_prot_nhi_table_entry_t    prot_nhi_entry;
    initial_prot_nhi_table_1_entry_t  prot_nhi_1_entry;
    uint32 table_index = 0;
    uint32 bit_index = 0;
    uint32 buf[4];
    int rv;

#ifdef BCM_MULTI_LEVEL_FAILOVER_SUPPORT
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        if (bcmi_l3_nh_multi_count_get(unit, nh_index) > 1) {

            return bcmi_failover_multi_level_prot_nhi_cleanup(unit, nh_index);
        }
    }
#endif

    rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm,
            MEM_BLOCK_ANY, nh_index, &prot_nhi_entry);

    if (rv < 0){
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&prot_nhi_entry, 0, sizeof(initial_prot_nhi_table_entry_t));

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
            MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

    if (rv < 0){
        return rv;
    }

    table_index = ((nh_index >> 7) & 0x1FF);
    bit_index = (nh_index & 0x7F);

    rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
            MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry);
    if (rv < 0){
        return BCM_E_NOT_FOUND;
    }
    sal_memcpy(buf, &prot_nhi_1_entry, sizeof(buf));

    buf[bit_index / 32] &= ~(0x1 << (bit_index % 32)); 
    soc_mem_field_set(unit, INITIAL_PROT_NHI_TABLE_1m,
            (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);
 
    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
            MEM_BLOCK_ALL, table_index, &prot_nhi_1_entry);

    return rv;

}
/*
 * Function:
 *        bcmi_failover_multi_level_attach
 * Purpose:
 *        Attach multi level failover objects.
 * Parameters:
 *     unit        - (IN) :  BCM device number
 *     attach_info - (IN) :  Information to attach two levels of failover.
 *
 * Desc:
 *        This function only keeps all the parameters in s/w copy.
 *        This software database will be used to setup the hardware
 *        when multi level failover id is passed to create
 *        egress object of failover/primary ports.
 * Returns:
 *        BCM_E_XXX
 */
int
bcmi_failover_multi_level_attach(int unit,
        bcm_failover_multi_level_t attach_info)
{
    int num_entry;
    int id;
    bcmi_failover_multi_level_info_t *local_multi_level_state;
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    int parent_failover_type = 0;
    tx_prot_group_1_table_entry_t tx_prot_group_entry;
    int child_failover_id = 0;
    int child_failover_type_1 = 0;
    int child_failover_type_2 = 0;

    _BCM_GET_FAILOVER_TYPE(attach_info.failover_id, parent_failover_type);
    if (parent_failover_type != _BCM_FAILOVER_MULTI_LEVEL) {
        return BCM_E_PARAM;
    }

    id = attach_info.failover_id;
    _BCM_GET_FAILOVER_ID(id);

    BCM_IF_ERROR_RETURN(_bcm_failover_multi_level_index_validate(unit, id));

    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        prot_group_mem = TX_PROT_GROUP_1_1_TABLEm;
    }

    /* make sure the given id is valid */
    num_entry = (soc_mem_index_count(unit, prot_group_mem) *
                              BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);

    child_failover_id = attach_info.prot_group_1;
    _BCM_GET_FAILOVER_ID(child_failover_id);

    if ((child_failover_id < 1) ||
        (child_failover_id >= num_entry)) {

        return BCM_E_PARAM;
    }
    /* We also need to check here that both the groups protection type is of same type */


    child_failover_id = attach_info.prot_group_2;
    _BCM_GET_FAILOVER_ID(child_failover_id);

    if ((child_failover_id < 1) ||
        (child_failover_id >= num_entry)) {

        return BCM_E_PARAM;
    }


    _BCM_GET_FAILOVER_TYPE(attach_info.prot_group_1, child_failover_type_1);
    _BCM_GET_FAILOVER_TYPE(attach_info.prot_group_2, child_failover_type_2);


    if (child_failover_type_1 != child_failover_type_2) {
        return BCM_E_PARAM;
    }

    if (child_failover_type_1 == _BCM_FAILOVER_ING_MC_PROTECTION_MODE) {

        child_failover_id = attach_info.prot_group_1;
        _BCM_GET_FAILOVER_ID(child_failover_id);

        /* Set Mode for level-2 protection group. */
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, TX_PROT_GROUP_1_TABLEm,
            MEM_BLOCK_ANY, child_failover_id, &tx_prot_group_entry));

        soc_mem_field32_set(unit, TX_PROT_GROUP_1_TABLEm,
            (uint32 *)&tx_prot_group_entry, PROT_MODEf, 1);

       BCM_IF_ERROR_RETURN (soc_mem_write(unit, TX_PROT_GROUP_1_TABLEm,
            MEM_BLOCK_ALL, child_failover_id, &tx_prot_group_entry));

        /* Write for second level-2 group also. */
        child_failover_id = attach_info.prot_group_2;
        _BCM_GET_FAILOVER_ID(child_failover_id);

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, TX_PROT_GROUP_1_TABLEm,
            MEM_BLOCK_ANY, child_failover_id, &tx_prot_group_entry));

        soc_mem_field32_set(unit, TX_PROT_GROUP_1_TABLEm,
            (uint32 *)&tx_prot_group_entry, PROT_MODEf, 1);

        BCM_IF_ERROR_RETURN (soc_mem_write(unit, TX_PROT_GROUP_1_TABLEm,
            MEM_BLOCK_ALL, child_failover_id, &tx_prot_group_entry));

    }


    local_multi_level_state = (&(bcmi_multi_level_sw_state[unit][id]));

    /* Program everything in the software state */
    local_multi_level_state->parent_failover_id = attach_info.failover_id;
    local_multi_level_state->child_group_1      = attach_info.prot_group_1;
    local_multi_level_state->child_group_2      = attach_info.prot_group_2;

    return BCM_E_NONE;
}

/*
 * Function:
 *        bcmi_failover_multi_level_prot_nhi_create
 * Purpose:
 *        Creates/inializes an entry of multi level failover type.
 * Parameters:
 *     unit        - (IN) :  BCM device number
 *     nh_index    - (IN) :  nh index at which the entry is being created.
 *
 * Desc:
 *        This function initializes entry for multi level double
 *        wide initial protection nhi.
 * Returns:
 *        BCM_E_XXX
 */
int
bcmi_failover_multi_level_prot_nhi_create(int unit, int nh_index)
{
    initial_prot_nhi_double_wide_table_entry_t prot_nhi_double_entry;
    int rv = BCM_E_UNAVAIL;

    sal_memset(&prot_nhi_double_entry, 0,
        sizeof(initial_prot_nhi_double_wide_table_entry_t));

    /* now write at the half the index for this double wide
     * entry. As the ASIC reads two prot nhi entryies in
     * double mode.
     */
    rv = soc_mem_write(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                       MEM_BLOCK_ALL, (nh_index/2), &prot_nhi_double_entry);

    return rv;
}

/*
 * Function:
 *        bcmi_failover_multi_level_prot_nhi_set
 * Purpose:
 *        Set multi level protection nhi failover entry
 * Parameters:
 *     unit          - (IN) : BCM device number
 *     nh_index      - (IN) : next hop index to set failover entry
 *     prot_nh_index - (IN) : protection next hop index
 *     mc_group      - (IN) : multicast group
 *     failover_id   - (IN) : failover group id
 *
 * Desc:
 *        This function sets protection table entry at
 *        index provided by nh index.
 * Returns:
 *        BCM_E_XXX
 */
int
bcmi_failover_multi_level_prot_nhi_set(int unit, int nh_index,
        uint32 prot_nh_index, bcm_multicast_t mc_group,
        bcm_failover_t failover_id)

{
    initial_prot_nhi_double_wide_table_entry_t prot_nhi_double_entry;
    int rv = BCM_E_UNAVAIL;
    bcmi_failover_multi_level_info_t *local_multi_level_state;
    int id;
    int l3mc_index;
    bcm_failover_t prot_group_1_id, prot_group_2_id, parent_failover_id;
    int prot_group_1, prot_group_2;
    int parent_failover_type, child_failover_type;
    int mc_protection = 0;
    void *null_entry = NULL;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_UNAVAIL;
    }

    null_entry = soc_mem_entry_null(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm);
    sal_memcpy(&prot_nhi_double_entry, null_entry,
        sizeof(initial_prot_nhi_double_wide_table_entry_t));

    id = failover_id;
    _BCM_GET_FAILOVER_ID(id);

    local_multi_level_state = (&(bcmi_multi_level_sw_state[unit][id]));

    /* Program everything in the software state */
    parent_failover_id = local_multi_level_state->parent_failover_id;
    parent_failover_type= local_multi_level_state->parent_failover_type;
    prot_group_1      = local_multi_level_state->child_group_1;
    prot_group_2      = local_multi_level_state->child_group_2;

    prot_group_1_id = prot_group_1;
    prot_group_2_id = prot_group_2;
    _BCM_GET_FAILOVER_ID(prot_group_1_id);
    _BCM_GET_FAILOVER_ID(prot_group_2_id);

    /*
     * No need to check if both child protection type is same.
     * it is already checked when attach was done.
     */
    _BCM_GET_FAILOVER_TYPE(prot_group_1, child_failover_type);

    if ((parent_failover_type == _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) ||
        (child_failover_type == _BCM_FAILOVER_ING_MC_PROTECTION_MODE)) {
        mc_protection = TRUE;
    }

    /* set the entry as double wide. */
    soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            &prot_nhi_double_entry, DOUBLE_WIDE_MODEf, 1);

    /* Write prot groups for second level group hierarchy.*/
    soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            &prot_nhi_double_entry, PROT_GROUP_INDEX_0f, prot_group_1_id);

    soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            &prot_nhi_double_entry, PROT_GROUP_INDEX_1f, prot_group_2_id);

    /* we are using new priotection (hierarchy) technique. */
    soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            &prot_nhi_double_entry, USE_NEW_PROTf, 1);

    if (mc_protection) {

        if (parent_failover_type == _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) {
            soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                    &prot_nhi_double_entry, PROT_MODEf, 1);
        }

        l3mc_index = _BCM_MULTICAST_ID_GET(mc_group);
        /* validate the index */
        if ((l3mc_index < 0) || (l3mc_index >
                    soc_mem_index_count(unit, L3_IPMCm))) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                &prot_nhi_double_entry, NEW_NHI_OR_MC_GROUPf, l3mc_index);
    } else {

        /* if user makes base index of multi
         * level protection as primary,  then he might
         * want to use the same NH as prot NH.
         * that way next 3 NHs are used as backup NHs.
         * other option is that he might be sending mc group,
         * but in that case we will not even use failover_if_id.
         */
        if (bcmi_l3_nh_multi_count_get(unit, nh_index) == 4) {

            soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                &prot_nhi_double_entry, NEW_NHI_OR_MC_GROUPf, nh_index);
        } else {
            soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                &prot_nhi_double_entry, NEW_NHI_OR_MC_GROUPf, prot_nh_index);
        }

        /*
         * Just making sure to set it to zero in case someone
         * tries to change failover groups and then try to set prot nhi again
         */
        soc_mem_field32_set(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                &prot_nhi_double_entry, PROT_MODEf, 0);
    }

    /* set s/w state of failover id at this NH */
    _BCM_FAILOVER_MULTI_LEVEL_FAILOVER_ID_SET(unit, nh_index, parent_failover_id);

    /* now write at the half the index for this double wide
     * entry. As the ASIC reads two prot nhi entryies in
     * double mode.
     */
    rv = soc_mem_write(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
                       MEM_BLOCK_ALL, (nh_index/2), &prot_nhi_double_entry);

    return rv;
}
/*
 * Function:
 *        bcmi_failover_multi_level_prot_nhi_cleanup
 * Purpose:
 *        Clean failover entry at this next hop.
 * Parameters:
 *     unit        - (IN) :  BCM device number
 *     nh_index    - (IN) :  nh index where the failover entry is pointing.
 *
 * Returns:
 *        BCM_E_XXX
 */
int
bcmi_failover_multi_level_prot_nhi_cleanup (int unit, int nh_index)
{
    initial_prot_nhi_double_wide_table_entry_t  prot_nhi_double_entry;
    initial_prot_nhi_table_1_entry_t  prot_nhi_1_entry;
    uint32 table_index = 0;
    uint32 bit_index = 0;
    uint32 buf[4];
    int rv;

    rv = soc_mem_read(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            MEM_BLOCK_ANY, nh_index/2, &prot_nhi_double_entry);

    if (rv < 0){
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&prot_nhi_double_entry, 0,
        sizeof(initial_prot_nhi_double_wide_table_entry_t));

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            MEM_BLOCK_ANY, nh_index/2, &prot_nhi_double_entry);

    if (rv < 0){
        return rv;
    }

    table_index = ((nh_index >> 7) & 0x1FF);
    bit_index = (nh_index & 0x7F);

    rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
            MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry);
    if (rv < 0){
        return BCM_E_NOT_FOUND;
    }
    sal_memcpy(buf, &prot_nhi_1_entry, sizeof(buf));

    buf[bit_index / 32] &= ~(0x1 << (bit_index % 32));
    soc_mem_field_set(unit, INITIAL_PROT_NHI_TABLE_1m,
            (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
            MEM_BLOCK_ALL, table_index, &prot_nhi_1_entry);

    return rv;
}

/*
 * Function:
 *        bcmi_failover_multi_level_prot_nhi_get
 * Purpose:
 *        Get failover paramenters at this next hop.
 * Parameters:
 *     unit        - (IN) :  BCM device number
 *     nh_index    - (IN) :  nh index where the failover entry is pointing.
 *     failover_id - (IN) :  Failover group
 *     prot_nh_index- (IN):  protected next hop
 *     mc_group       (IN):  multicast group
 *
 * Returns:
 *        BCM_E_XXX
 */
int
bcmi_failover_multi_level_prot_nhi_get(int unit, int nh_index,
        bcm_failover_t  *failover_id, int  *prot_nh_index,
        bcm_multicast_t  *mc_group)
{
    uint32 replacement_type;
    int l3mc_index = 0 , prot_group_idx = 0, prot_group_prot_mode = 0;
    initial_prot_nhi_double_wide_table_entry_t   prot_nhi_double_entry;
    tx_prot_group_1_table_entry_t tx_prot_group_entry;

    if (!(soc_feature(unit, soc_feature_hierarchical_protection))) {
        return BCM_E_UNAVAIL;
    }


    BCM_IF_ERROR_RETURN (soc_mem_read(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
        MEM_BLOCK_ANY, nh_index/2, &prot_nhi_double_entry));


    _BCM_FAILOVER_MULTI_LEVEL_FAILOVER_ID_GET(unit, nh_index, *failover_id);

    replacement_type = soc_mem_field32_get(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
        &prot_nhi_double_entry, PROT_MODEf);

    prot_group_idx = soc_mem_field32_get(unit,
        INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
        &prot_nhi_double_entry, PROT_GROUP_INDEX_1f);

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, TX_PROT_GROUP_1_TABLEm,
        MEM_BLOCK_ANY, prot_group_idx, &tx_prot_group_entry));

    prot_group_prot_mode = soc_mem_field32_get(unit,
        TX_PROT_GROUP_1_TABLEm, (&tx_prot_group_entry),
        PROT_MODEf);

    /*
     * If protection mode at level 1
     * or level 2 is 1 then we will have multicast group
     * set in the double wide protection tables.
     */
    replacement_type |= prot_group_prot_mode;

    if (!replacement_type) {
        *prot_nh_index = soc_mem_field32_get(unit,
            INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            &prot_nhi_double_entry, NEW_NHI_OR_MC_GROUPf);
        /* In case of multi level swithing, we can have a primary port
         * pointing to a failover port but the primary port is the
         * starting point of all the backup links.
         * Backup links are created in the next 3 indexes of primary
         * nexthop. but to access all primary and backup links,
         * SDK will point primary to primary and then
         * backup links will be accessed from the offset generated
         * from failover tables.
         * If the primary port is pointing to its own NH, then it is sure
         * that failover link was on the next nh index of priamry.
         */
        if (*prot_nh_index == nh_index) {
            *prot_nh_index += 1;
        }
    } else {
        l3mc_index = soc_mem_field32_get(unit,
            INITIAL_PROT_NHI_DOUBLE_WIDE_TABLEm,
            &prot_nhi_double_entry, NEW_NHI_OR_MC_GROUPf);

         _BCM_MULTICAST_GROUP_SET(*mc_group,
            _BCM_MULTICAST_TYPE_EGRESS_OBJECT,
            l3mc_index);
    }

    return BCM_E_NONE;
}


#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) &&  defined(INCLUDE_L3) */
