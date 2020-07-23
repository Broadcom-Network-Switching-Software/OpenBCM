/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    failover.c
 * Purpose: Handle TH2 specific failover APIs
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(INCLUDE_L3)
#if defined(BCM_TOMAHAWK2_SUPPORT)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <soc/scache.h>
#include <bcm/failover.h>
#include <bcm/extender.h>

#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/tomahawk2.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/triumph2.h>

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0


#define _bcm_th2_failover_bk_info _bcm_failover_bk_info
typedef _bcm_failover_bookkeeping_t _bcm_th2_failover_bookkeeping_t; 

/*
 * Function:
 *      _bcm_th2_failover_check_init
 * Purpose:
 *      Check if Failover is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_th2_failover_check_init (int unit)
{
    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
        return BCM_E_UNIT;
    }

    if (!_bcm_th2_failover_bk_info[unit].initialized) {
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
bcm_th2_failover_lock (int unit)
{
    int rv;

    rv = _bcm_th2_failover_check_init(unit);

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
bcm_th2_failover_unlock (int unit)
{
    int rv;

    rv = _bcm_th2_failover_check_init(unit);

    if ( rv == BCM_E_NONE ) {
        sal_mutex_give(FAILOVER_INFO((unit))->failover_mutex);
    }
}

/*
 * Function:
 *      _bcm_th2_failover_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_th2_failover_free_resource(int unit,
        _bcm_th2_failover_bookkeeping_t *failover_info)
{
    if (!failover_info) {
        return;
    }

    if (failover_info->prot_group_bitmap) {
        sal_free(failover_info->prot_group_bitmap);
        failover_info->prot_group_bitmap = NULL;
    }

    if (failover_info->ingress_prot_group_bitmap) {
        sal_free(failover_info->ingress_prot_group_bitmap);
        failover_info->ingress_prot_group_bitmap = NULL;
    }
    return;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
* Function:
*      _bcm_th2_failover_wb_alloc
* Purpose:
*      Allocate space for failover
* Parameters:
*      unit     - Device Number
* Returns:
*      BCM_E_XXX
*/
STATIC int
_bcm_th2_failover_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, stable_size, alloc_size = 0;
    int  num_prot_group, rx_num_prot_group;
    num_prot_group = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm)
                      * BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    alloc_size += SHR_BITALLOCSIZE(num_prot_group);

    rx_num_prot_group = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm)
                      * BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    alloc_size += SHR_BITALLOCSIZE(rx_num_prot_group);
   
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
 
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
 *      _bcm_th2_failover_sync
 * Purpose:
 *      Sync level-2 state in scache
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_th2_failover_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    int  num_prot_group, rx_num_prot_group, stable_size;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    num_prot_group = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                      BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    rx_num_prot_group = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                         BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL));

    ptr = scache_ptr;

    sal_memcpy(ptr, BCM_FAILOVER_PROT_GROUP_MAP(unit),
               SHR_BITALLOCSIZE(num_prot_group));
    ptr += SHR_BITALLOCSIZE(num_prot_group);
    
    sal_memcpy(ptr, BCM_FAILOVER_INGRESS_PROT_GROUP_MAP(unit),
               SHR_BITALLOCSIZE(rx_num_prot_group));
    ptr += SHR_BITALLOCSIZE(rx_num_prot_group);


    return BCM_E_NONE;
}


/* 
 * Function:
 *     _bcm_th2_failover_reinit
 * Purpose:
 *     Reinit for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_failover_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    int stable_size, rv;
    uint16 recovered_ver;
    int  num_prot_group, rx_num_prot_group;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    num_prot_group = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                      BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    rx_num_prot_group = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                         BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION,  &recovered_ver);
    if (BCM_E_NOT_FOUND == rv) {
        /* Upgrading from SDK release that does not have warmboot state */
        BCM_IF_ERROR_RETURN(_bcm_th2_failover_wb_alloc(unit));
        return BCM_E_NONE;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    ptr = scache_ptr;
    
    sal_memcpy(BCM_FAILOVER_PROT_GROUP_MAP(unit), ptr,
               SHR_BITALLOCSIZE(num_prot_group));
    ptr += SHR_BITALLOCSIZE(num_prot_group);

    sal_memcpy(BCM_FAILOVER_INGRESS_PROT_GROUP_MAP(unit), ptr,
               SHR_BITALLOCSIZE(rx_num_prot_group));
    ptr += SHR_BITALLOCSIZE(rx_num_prot_group);

    return BCM_E_NONE;
}


#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_th2_failover_init
 * Purpose:
 *      Initialize the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_failover_init (int unit)
{
    int rv = BCM_E_NONE;
    int num_prot_group, rx_num_prot_group;
    _bcm_th2_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);

    /*
     * allocate resources
     */
    if (failover_info->initialized) {
        BCM_IF_ERROR_RETURN(bcm_th2_failover_cleanup(unit));
    }

    num_prot_group = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm)
                      * BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

    failover_info->prot_group_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_prot_group), "prot_group_bitmap");
    if (failover_info->prot_group_bitmap == NULL) {
        _bcm_th2_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

    sal_memset(failover_info->prot_group_bitmap, 0,
               SHR_BITALLOCSIZE(num_prot_group));


    rx_num_prot_group = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm)
                         * BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

    failover_info->ingress_prot_group_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(rx_num_prot_group), "ingress_prot_group_bitmap");
    if (failover_info->ingress_prot_group_bitmap == NULL) {
        _bcm_th2_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

    sal_memset(failover_info->ingress_prot_group_bitmap, 0,
               SHR_BITALLOCSIZE(rx_num_prot_group));    

    /* Create Failover  protection mutex. */
    failover_info->failover_mutex = sal_mutex_create("failover_mutex");
    if (!failover_info->failover_mutex) {
        _bcm_th2_failover_free_resource(unit, failover_info);
        return BCM_E_MEMORY;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if(SOC_WARM_BOOT(unit)) {
        rv = _bcm_th2_failover_reinit(unit);
        if (BCM_FAILURE(rv)){
            _bcm_th2_failover_free_resource(unit, failover_info);
            return rv;
        }
    } else {
        rv = _bcm_th2_failover_wb_alloc(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_th2_failover_free_resource(unit, failover_info);
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* Mark the state as initialized */
    failover_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *      _bcm_th2_failover_hw_clear
 * Purpose:
 *     Perform hw tables clean up for failover module. 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th2_failover_hw_clear (int unit)
{
    int i, rv, rv_error = BCM_E_NONE;
    int num_entry;

    num_entry = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                 BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
        if (_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, i)) {
            rv = bcm_th2_failover_destroy(unit, i);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }
    
    num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                 BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
        if (_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(unit, i)) {
            _BCM_ENCAP_TYPE_IN_FAILOVER_ID(i, _BCM_FAILOVER_INGRESS);
            rv = bcm_th2_failover_destroy(unit, i);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_th2_failover_cleanup
 * Purpose:
 *      DeInit  the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th2_failover_cleanup (int unit)
{
    _bcm_th2_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);
    int rv = BCM_E_UNAVAIL;

    if (FALSE == failover_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcm_th2_failover_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_th2_failover_hw_clear(unit);
    }

    /* Free software resources */
    (void) _bcm_th2_failover_free_resource(unit, failover_info);

    bcm_th2_failover_unlock(unit);

    /* Destroy protection mutex. */
    sal_mutex_destroy(failover_info->failover_mutex);

    /* Mark the state as uninitialized */
    failover_info->initialized = FALSE;

    return rv;
}


/*
 * Function:
 *		_bcm_th2_failover_get_prot_group_table_index
 * Purpose:
 *		Obtain Index into  INITIAL_PROT_GROUP_TABLE
 * Parameters:
 *		IN :  Unit
 *           OUT : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC int
_bcm_th2_failover_get_prot_group_table_index (int unit, int *prot_index)
{
    int  i;
    int  num_entry;

    num_entry = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                 BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

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
 *		_bcm_th2_failover_clear_prot_group_table_entry
 * Purpose:
 *		Clear INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC void
_bcm_th2_failover_clear_prot_group_table_entry (int unit,
        bcm_failover_t prot_index)
{
   _BCM_FAILOVER_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}

/*
 * Function:
 *		_bcm_th2_failover_set_prot_group_table_entry
 * Purpose:
 *		Set INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */
STATIC void
_bcm_th2_failover_set_prot_group_table_entry (int unit,
        bcm_failover_t prot_index)
{
   _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, prot_index);
}

/*
 * Function:
 *		_bcm_th2_failover_id_validate
 * Purpose:
 *		Validate the failover ID
 * Parameters:
 *           IN : failover_id
 * Returns:
 *		BCM_E_XXX
 */
STATIC int
_bcm_th2_failover_id_validate (int unit, bcm_failover_t failover_id)
{
    int num_entry;

    num_entry = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                 BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

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
 *     _bcm_th2_failover_clear_ingress_prot_group_table_index
 * Purpose:
 *     Clear RX_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *     unit        : (IN) Device Unit Number
 *     prot_index  : (IN) RX_PROT_GROUP_TABLE bit index
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th2_failover_clear_ingress_prot_group_table_index(
    int unit, bcm_failover_t prot_index)
{
   _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}

/*
 * Function:
 *     _bcm_th2_failover_ingress_id_validate
 * Purpose:
 *     Validate the ingress failover ID
 * Parameters:
 *     unit        : (IN) Device Unit Number
 *     failover_id : (IN) Ingress failover ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th2_failover_ingress_id_validate(int unit,
                                       bcm_failover_t failover_id)
{
    int num_entry;

    num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                 BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

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
 *     _bcm_th2_failover_get_ingress_prot_group_table_index
 * Purpose:
 *     Obtain Index into RX_PROT_GROUP_TABLE
 * Parameters:
 *     unit        : (IN)  Device Unit Number
 *     prot_index  : (OUT) RX_PROT_GROUP_TABLE bit index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th2_failover_get_ingress_prot_group_table_index(int unit,
                                                      int *prot_index)
{
    int  i;
    int  num_entry;

    num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                 BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);

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
 *     _bcm_th2_failover_set_ingress_prot_group_table_index
 * Purpose:
 *     Set RX_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *     unit        : (IN) Device Unit Number
 *     prot_index  : (IN) RX_PROT_GROUP_TABLE bit index
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_th2_failover_set_ingress_prot_group_table_index(int unit,
                                                      bcm_failover_t prot_index)
{
   _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(unit, prot_index);
}


/*
 * Function:
 *		bcm_th2_failover_create
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
bcm_th2_failover_create (int unit, uint32 flags, bcm_failover_t *failover_id)
{
    int rv = BCM_E_UNAVAIL;
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    int  num_entry,local_failover_id;
    int  failover_type;
    uint32 table_index;
    uint32 bit_index;
    uint32 buf[4];
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;

    if (failover_id == NULL) {
        return BCM_E_PARAM;
    }

    /* Check for unsupported Flag */
    if (flags & (~(BCM_FAILOVER_REPLACE |
                   BCM_FAILOVER_WITH_ID |
                   BCM_FAILOVER_INGRESS))) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_FAILOVER_WITH_ID) || (flags & BCM_FAILOVER_REPLACE)) {

        if (flags & BCM_FAILOVER_INGRESS) {
            local_failover_id = *failover_id;

            /* Allow failoverIds of type _BCM_FAILOVER_INGRESS */
            _BCM_GET_FAILOVER_TYPE(local_failover_id, failover_type);
            if (!(failover_type & _BCM_FAILOVER_INGRESS)) {
                return BCM_E_PARAM;
            }

            _BCM_GET_FAILOVER_ID(local_failover_id);
            /* make sure the given id is valid */
            num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                         BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
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
        } else {
            /* make sure the given id is valid */
            num_entry = (soc_mem_index_count(unit, prot_group_mem) *
                         BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
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
        rv = _bcm_th2_failover_get_prot_group_table_index(unit, failover_id);
    } else if (flags & BCM_FAILOVER_INGRESS) {
        rv = _bcm_th2_failover_get_ingress_prot_group_table_index(unit,
                                                                   failover_id);
        _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(unit, *failover_id);
        _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_INGRESS);
    }
    if (BCM_SUCCESS(rv)) {
        if (flags & BCM_FAILOVER_INGRESS) {
            local_failover_id = *failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            table_index = ((local_failover_id >> 7) & 
                           (soc_mem_index_count(unit,RX_PROT_GROUP_TABLEm) - 1));
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
                _bcm_th2_failover_clear_ingress_prot_group_table_index(
                    unit, local_failover_id);
                return rv;
            }
        } else {
            local_failover_id = *failover_id;
            _BCM_GET_FAILOVER_ID(*failover_id);
            /* Init table entry */
            table_index = ((*failover_id >> 7) & 
                           (soc_mem_index_count(unit,prot_group_mem) - 1));
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
                _bcm_th2_failover_clear_prot_group_table_entry(unit, *failover_id);
                return BCM_E_RESOURCE;
            }
            *failover_id = local_failover_id;
        }
    }

    return rv;
}


/*
 * Function:
 *		bcm_th2_failover_destroy
 * Purpose:
 *		Destroy a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 * Returns:
 *		BCM_E_XXX
 */
int 
bcm_th2_failover_destroy (int unit, bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
    int type = _BCM_FAILOVER_DEFAULT_MODE;
    int local_failover_id=0;
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    uint32 table_index;
    uint32 bit_index;
    uint32 buf[4];
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;

    /*Check failover id type Ingress or Egress Encap*/
    _BCM_GET_FAILOVER_TYPE(failover_id, type);
    if (type & _BCM_FAILOVER_INGRESS) {
        local_failover_id = failover_id;
        _BCM_GET_FAILOVER_ID(local_failover_id);

        rv = _bcm_th2_failover_ingress_id_validate(unit, local_failover_id);
        BCM_IF_ERROR_RETURN(rv);

        /*Release software index*/
        _bcm_th2_failover_clear_ingress_prot_group_table_index(
            unit, local_failover_id);
        /*clear hw entry*/
        table_index = ((local_failover_id >> 7) & 
                       (soc_mem_index_count(unit,RX_PROT_GROUP_TABLEm) - 1));
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
            _bcm_th2_failover_set_ingress_prot_group_table_index(
                unit, local_failover_id);
            return rv;
        }
    }  else {
        rv = _bcm_th2_failover_id_validate(unit, failover_id);
        BCM_IF_ERROR_RETURN(rv);

        if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
            return BCM_E_NOT_FOUND;
        }

        /*Release index*/
        _bcm_th2_failover_clear_prot_group_table_entry(unit, failover_id);

        /* Clear entry */
        table_index = ((failover_id >> 7) & 
                       (soc_mem_index_count(unit,prot_group_mem) - 1));
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
            _bcm_th2_failover_set_prot_group_table_entry(unit, failover_id);
            return BCM_E_RESOURCE;
        }
    }

    return rv;
}


STATIC int
_bcm_th2_failover_nhi_get(int unit, bcm_gport_t port, int *nh_index)
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
 *        bcm_th2_failover_status_set
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
bcm_th2_failover_status_set (int unit,
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
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;

    if ((value < 0) || (value > 1)) {
        return BCM_E_PARAM;
    }

    if (failover->failover_id != BCM_FAILOVER_INVALID) {
        if (failover->flags == BCM_FAILOVER_INGRESS) {
            /* RX prot group table*/
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            BCM_IF_ERROR_RETURN(
               _bcm_th2_failover_ingress_id_validate(unit, local_failover_id));

            table_index = ((local_failover_id >> 7) & 
                           (soc_mem_index_count(unit,RX_PROT_GROUP_TABLEm) - 1));
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
            table_index = ((local_failover_id >> 7) & 
                           (soc_mem_index_count(unit,prot_group_mem) - 1));
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN(
                    _bcm_th2_failover_id_validate(unit, local_failover_id));

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
    } else if (failover->port != BCM_GPORT_INVALID) {
        /* Individual protection for Pseudo-wire: Egress and Ingress */
        BCM_IF_ERROR_RETURN (
                _bcm_th2_failover_nhi_get(unit, failover->port , &nh_index));

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
 *        bcm_th2_failover_status_get
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
bcm_th2_failover_status_get (int unit,
        bcm_failover_element_t *failover,
        int  *value)
{
    tx_initial_prot_group_table_entry_t  tx_init_prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    initial_prot_nhi_table_1_entry_t   prot_nhi_1_entry;
    int nh_index;
    int local_failover_id=0;
    uint32 table_index = 0;
    uint32 bit_index = 0;
    uint32 buf[4];
    uint32 prot_group_mem = TX_INITIAL_PROT_GROUP_TABLEm;
    uint32 replace_enable_field = REPLACE_ENABLE_BITMAPf;
    void *prot_group_entry = &tx_init_prot_group_entry;
    
    if (failover->failover_id != BCM_FAILOVER_INVALID) {
        if (failover->flags == BCM_FAILOVER_INGRESS) {
            local_failover_id = failover->failover_id;
            _BCM_GET_FAILOVER_ID(local_failover_id);
            BCM_IF_ERROR_RETURN(
               _bcm_th2_failover_ingress_id_validate(unit, local_failover_id));

            table_index = ((local_failover_id >> 7) & 
                           (soc_mem_index_count(unit,RX_PROT_GROUP_TABLEm) - 1));
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
                    _bcm_th2_failover_id_validate(unit, local_failover_id));

            table_index = ((local_failover_id >> 7) & 
                           (soc_mem_index_count(unit,prot_group_mem) - 1));
            bit_index = (local_failover_id & 0x7F);

            BCM_IF_ERROR_RETURN (soc_mem_read(unit, prot_group_mem,
                        MEM_BLOCK_ANY, table_index, prot_group_entry));

            soc_mem_field_get(unit, prot_group_mem,
                    (uint32 *)prot_group_entry, replace_enable_field, buf);
            *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);

        }
    } else if (failover->intf != BCM_IF_INVALID) {

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, failover->intf)) {
            nh_index = failover->intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            nh_index = failover->intf  - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }

        table_index = ((nh_index >> 7) & 0x1FF);
        bit_index = (nh_index & 0x7F);

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                    MEM_BLOCK_ANY, table_index, &prot_nhi_1_entry));

        soc_mem_field_get(unit, INITIAL_PROT_NHI_TABLE_1m,
                (uint32 *)&prot_nhi_1_entry, REPLACE_ENABLE_BITMAPf, buf);

        *value = ((buf[bit_index / 32] >> (bit_index % 32)) & 0x1);

    } else if (failover->port != BCM_GPORT_INVALID) {

        BCM_IF_ERROR_RETURN (
                _bcm_th2_failover_nhi_get(unit, failover->port , &nh_index));

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


#endif /* defined(BCM_TOMAHAWK2_SUPPORT) */
#endif /* defined(INCLUDE_L3) */
