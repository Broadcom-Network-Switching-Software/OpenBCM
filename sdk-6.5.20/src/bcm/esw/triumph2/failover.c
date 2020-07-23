/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * TR2 failover API
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH2_SUPPORT) &&  defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/failover.h>
#include <bcm/failover.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif
#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw/saber2.h>
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <bcm_int/esw/tomahawk2.h>
#endif
#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#include <soc/esw/ecmp.h>
#endif /*  SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_failover_check_init
 * Purpose:
 *      Check if Failover is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


STATIC int 
_bcm_failover_check_init (int unit)
{
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
            return BCM_E_UNIT;
        }

        if (!_bcm_failover_bk_info[unit].initialized) { 
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
 bcm_tr2_failover_lock (int unit)
{
   int rv;

   rv = _bcm_failover_check_init (unit);
   
   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(FAILOVER_INFO((unit))->failover_mutex, sal_mutex_FOREVER);
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
bcm_tr2_failover_unlock(int unit)
{
    int rv;

    rv = _bcm_failover_check_init (unit);

    if ( rv == BCM_E_NONE ) {
        sal_mutex_give(FAILOVER_INFO((unit))->failover_mutex);
    }
}



/*
 * Function:
 *      _bcm_tr2_failover_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */


STATIC void
_bcm_tr2_failover_free_resource(int unit, _bcm_failover_bookkeeping_t *failover_info)
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

    if (failover_info->mmu_prot_group_bitmap) {
        sal_free(failover_info->mmu_prot_group_bitmap);
        failover_info->mmu_prot_group_bitmap = NULL;
    }

}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0


/*
 * Function:
 *     _bcm_tr2_failover_alloc
 * Purpose:
 *     Alloc scache for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_failover_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size, prot_grp_idx_count;

    prot_grp_idx_count = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
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
 *     bcm_tr2_failover_sync
 * Purpose:
 *     Sync scache for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_failover_sync(int unit)
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
    prot_grp_idx_count = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);

    /* Sync PROT_GROUP_MAP */
    sal_memcpy(ptr, BCM_FAILOVER_PROT_GROUP_MAP(unit),
               SHR_BITALLOCSIZE(prot_grp_idx_count));
    ptr += SHR_BITALLOCSIZE(prot_grp_idx_count);

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_tr2_failover_prot_nhi_recover
 * Purpose:
 *     PROT_NHI_MAP recover for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_failover_prot_nhi_recover(int unit)
{
    int idx, index_min, index_max, prot_grp_idx, grp_idx_max;
    initial_prot_nhi_table_entry_t prot_nhi_entry;
    soc_field_t fld;
    soc_field_t fld_grp;

    grp_idx_max = soc_mem_index_max(unit, INITIAL_PROT_GROUP_TABLEm);

    index_min = soc_mem_index_min(unit, INITIAL_PROT_NHI_TABLEm);
    index_max = soc_mem_index_max(unit, INITIAL_PROT_NHI_TABLEm);
    if (SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit)) {
        fld = REPLACEMENT_DATAf;
    } else {
        fld = PROT_NEXT_HOP_INDEXf;
    }
    fld_grp = PROT_GROUPf;

    for (idx = index_min; idx <= index_max; idx++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, MEM_BLOCK_ANY,
                                          idx, &prot_nhi_entry));
        prot_grp_idx = soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit, &prot_nhi_entry, fld_grp);
        if ( (prot_grp_idx >= 1) && (prot_grp_idx <= grp_idx_max) &&
             (soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit, &prot_nhi_entry,
                                                      fld) != 0) ) {
            _BCM_FAILOVER_PROT_NHI_MAP_USED_SET(unit, idx);
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_tr2_failover_recovery
 * Purpose:
 *     Scache recovery for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_failover_recovery(int unit)
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
        rv = _bcm_tr2_failover_alloc(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        return BCM_E_NOT_FOUND;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* SCACHE Hande found, recover from SCAHE */
    prot_grp_idx_count = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
    ptr = scache_ptr;

    /* Recover PROT_GROUP_MAP */
    sal_memcpy(BCM_FAILOVER_PROT_GROUP_MAP(unit), ptr,
               SHR_BITALLOCSIZE(prot_grp_idx_count));
    ptr += SHR_BITALLOCSIZE(prot_grp_idx_count);

    /* Recover PROT_NHI_MAP */
    BCM_IF_ERROR_RETURN(_bcm_tr2_failover_prot_nhi_recover(unit));

    return BCM_E_NONE;
}


/* 
 * Function:
 *     _bcm_tr2_failover_reinit
 * Purpose:
 *     Reinit for warm boot.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_failover_reinit(int unit)
{
    int idx, index_min, index_max, prot_grp_idx, grp_idx_max;
    initial_prot_nhi_table_entry_t prot_nhi_entry;
    soc_field_t fld;
    soc_field_t fld_grp;
    int rv;

    if (SOC_IS_TRIUMPH3(unit)) {
        /* Try SCACHE Recovery first */
        rv = _bcm_tr2_failover_recovery(unit);

        if BCM_SUCCESS(rv) {
            /* WB recovery successfull, return */
            return (rv);
        }

        if ( (BCM_FAILURE(rv)) && (rv != BCM_E_NOT_FOUND)) {
            /* Something failed in WB recovery, return error code */
            return (rv);
        }

        /* If we are here => (rv == BCM_E_NOT_FOUND), Resort to H/w recovery */
    }

    grp_idx_max = soc_mem_index_max(unit, INITIAL_PROT_GROUP_TABLEm);

    index_min = soc_mem_index_min(unit, INITIAL_PROT_NHI_TABLEm);
    index_max = soc_mem_index_max(unit, INITIAL_PROT_NHI_TABLEm);
    if (SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit)) {
        fld = REPLACEMENT_DATAf;
    } else {
        fld = PROT_NEXT_HOP_INDEXf; 
    }
    fld_grp = PROT_GROUPf;
     
    for (idx = index_min; idx <= index_max; idx++) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, MEM_BLOCK_ANY,
                                          idx, &prot_nhi_entry));
        prot_grp_idx = soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit, &prot_nhi_entry, fld_grp);
        if ( (prot_grp_idx >= 1) && (prot_grp_idx <= grp_idx_max) &&
             (soc_INITIAL_PROT_NHI_TABLEm_field32_get(unit, &prot_nhi_entry,
                                                      fld) != 0) ) {
            _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, prot_grp_idx);
            _BCM_FAILOVER_PROT_NHI_MAP_USED_SET(unit, idx);
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *      bcm_tr2_failover_init
 * Purpose:
 *      Initialize the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


int
bcm_tr2_failover_init(int unit)
{
    int rv = BCM_E_NONE;
    int num_prot_group, num_prot_nhi;
#if defined(BCM_SABER2_SUPPORT)
    int mmu_num_prot_group;
#endif
    _bcm_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);

     /*
      * allocate resources
      */
     if (failover_info->initialized) {
          BCM_IF_ERROR_RETURN(bcm_tr2_failover_cleanup(unit));
     }

     num_prot_group = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
     num_prot_nhi = soc_mem_index_count(unit, INITIAL_PROT_NHI_TABLEm);

     failover_info->prot_group_bitmap =
     sal_alloc(SHR_BITALLOCSIZE(num_prot_group), "prot_group_bitmap");
     if (failover_info->prot_group_bitmap == NULL) {
          _bcm_tr2_failover_free_resource(unit, failover_info);
          return BCM_E_MEMORY;
     }

     failover_info->prot_nhi_bitmap =
     sal_alloc(SHR_BITALLOCSIZE(num_prot_nhi), "prot_nhi_bitmap");
     if (failover_info->prot_nhi_bitmap == NULL) {
          _bcm_tr2_failover_free_resource(unit, failover_info);
          return BCM_E_MEMORY;
     }

     sal_memset(failover_info->prot_group_bitmap, 0, SHR_BITALLOCSIZE(num_prot_group));
     sal_memset(failover_info->prot_nhi_bitmap, 0, SHR_BITALLOCSIZE(num_prot_nhi));

     /* Create Failover  protection mutex. */
     failover_info->failover_mutex = sal_mutex_create("failover_mutex");
     if (!failover_info->failover_mutex) {
          _bcm_tr2_failover_free_resource(unit, failover_info);
          return BCM_E_MEMORY;
     }

#if defined(BCM_SABER2_SUPPORT)
     if (SOC_IS_SABER2(unit)) {
         /* Since each entry represent 32 group, multiplexing with 32 */
         mmu_num_prot_group = 2 * soc_mem_index_count(unit, MMU_INITIAL_NHOP_TBLm);

         failover_info->mmu_prot_group_bitmap =
             sal_alloc(SHR_BITALLOCSIZE(mmu_num_prot_group), "mmu_prot_group_bitmap");
         if (failover_info->mmu_prot_group_bitmap == NULL) {
             _bcm_tr2_failover_free_resource(unit, failover_info);
             return BCM_E_MEMORY;
         }

         sal_memset(failover_info->mmu_prot_group_bitmap, 0, SHR_BITALLOCSIZE(mmu_num_prot_group));
     }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
    if(SOC_WARM_BOOT(unit)) {
        rv = _bcm_tr2_failover_reinit(unit);
        if (BCM_FAILURE(rv)){
            _bcm_tr2_failover_free_resource(unit, failover_info);
            return rv;
        }
    } else {
        /* Cold boot, Allocate scache */
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = _bcm_tr2_failover_alloc(unit);
            if (BCM_FAILURE(rv)) {
                _bcm_tr2_failover_free_resource(unit, failover_info);
                return rv;
            }
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Mark the state as initialized */
    failover_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_failover_hw_clear
 * Purpose:
 *     Perform hw tables clean up for failover module. 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
 
STATIC int
_bcm_tr2_failover_hw_clear(int unit)
{
    int i, rv, rv_error = BCM_E_NONE;
    int num_entry;

    num_entry = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
        if (_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, i)) {
            rv = bcm_tr2_failover_destroy(unit, i);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        num_entry = 2 * soc_mem_index_count(unit, MMU_INITIAL_NHOP_TBLm) ;

        /* Index-0 is reserved */      
        for (i = 1; i < num_entry; i++) {
            if (_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, i)) {
                rv = bcm_tr2_mmu_failover_destroy(unit, i);
                if (rv < 0) {
                    rv_error = rv;
                }
            }
        }
    }
#endif
    return rv_error;
}

/*
 * Function:
 *      bcm_tr2_failover_cleanup
 * Purpose:
 *      DeInit  the Failover software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_failover_cleanup(int unit)
{
    _bcm_failover_bookkeeping_t *failover_info = FAILOVER_INFO(unit);
    int rv = BCM_E_UNAVAIL;

    if (FALSE == failover_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcm_tr2_failover_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_tr2_failover_hw_clear(unit);
    }

    /* Free software resources */
    (void) _bcm_tr2_failover_free_resource(unit, failover_info);

    bcm_tr2_failover_unlock (unit);

    /* Destroy protection mutex. */
    sal_mutex_destroy(failover_info->failover_mutex);

    /* Mark the state as uninitialized */
    failover_info->initialized = FALSE;

    return rv;
}

#if defined(BCM_SABER2_SUPPORT)
/*
 * Function:
 *		_bcm_tr2_failover_get_prot_group_table_index
 * Purpose:
 *		Obtain Index into  INITIAL_PROT_GROUP_TABLE
 * Parameters:
 *		IN :  Unit
 *           OUT : prot_index
 * Returns:
 *		BCM_E_XXX
 */


STATIC int
_bcm_tr2_failover_get_mmu_prot_group_table_index (int unit, int  *prot_index)
{
    int  i;
    int  num_entry;

    num_entry = 2 * soc_mem_index_count(unit, MMU_INITIAL_NHOP_TBLm);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
         if (!_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, i)) {
              break;
         }
    }

    if (i == num_entry) {
         return  BCM_E_RESOURCE;
    }
    _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_SET(unit, i);
    *prot_index = i;	 

    return BCM_E_NONE;
}
#endif /* BCM_SABER2_SUPPORT */


/*
 * Function:
 *		_bcm_tr2_failover_clear_prot_group_table_entry
 * Purpose:
 *		Clear INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */


STATIC void
_bcm_tr2_failover_clear_mmu_prot_group_table_entry (int unit, bcm_failover_t  prot_index)
{
   _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}


/*
 * Function:
 *		_bcm_tr2_failover_id_validate
 * Purpose:
 *		Validate the failover ID
 * Parameters:
 *           IN : failover_id
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr2_failover_mmu_id_validate (int unit, bcm_failover_t failover_id)
{
    int num_entry;

    num_entry = 2 * soc_mem_index_count(unit,  MMU_INITIAL_NHOP_TBLm);

    if ( ( failover_id < 1 ) || (failover_id > num_entry ) ) {
         return BCM_E_PARAM;
    } else if (failover_id == num_entry) {
         return  BCM_E_RESOURCE;
    }
    if (!_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *		_bcm_tr2_failover_get_prot_group_table_index
 * Purpose:
 *		Obtain Index into  INITIAL_PROT_GROUP_TABLE
 * Parameters:
 *		IN :  Unit
 *           OUT : prot_index
 * Returns:
 *		BCM_E_XXX
 */


STATIC int
_bcm_tr2_failover_get_prot_group_table_index (int unit, int  *prot_index)
{
    int  i;
    int  num_entry;

    num_entry = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);

    /* Index-0 is reserved */      
    for (i = 1; i < num_entry; i++) {
         if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, i)) {
              break;
         }
    }

    if (i == num_entry) {
         return  BCM_E_RESOURCE;
    }
    _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, i);
    *prot_index = i;	 

    return BCM_E_NONE;
}



/*
 * Function:
 *		_bcm_tr2_failover_clear_prot_group_table_entry
 * Purpose:
 *		Clear INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */


STATIC void
_bcm_tr2_failover_clear_prot_group_table_entry (int unit, bcm_failover_t  prot_index)
{
   _BCM_FAILOVER_PROT_GROUP_MAP_USED_CLR(unit, prot_index);
}



/*
 * Function:
 *		_bcm_tr2_failover_set_prot_group_table_entry
 * Purpose:
 *		Set INITIAL_PROT_GROUP_TABLE entry pointed by Index
 * Parameters:
 *		IN :  Unit
 *           IN : prot_index
 * Returns:
 *		BCM_E_XXX
 */


STATIC void
_bcm_tr2_failover_set_prot_group_table_entry (int unit, bcm_failover_t  prot_index)
{
   _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(unit, prot_index);
}

/*
 * Function:
 *		_bcm_tr2_failover_id_validate
 * Purpose:
 *		Validate the failover ID
 * Parameters:
 *           IN : failover_id
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr2_failover_id_validate (int unit, bcm_failover_t failover_id)
{
    int num_entry;

    num_entry = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);

    if ( ( failover_id < 1 ) || (failover_id > num_entry ) ) {
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
 *		bcm_tr2_failover_create
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
bcm_tr2_failover_create(int unit, uint32 flags, bcm_failover_t *failover_id)
{
    int rv = BCM_E_UNAVAIL;
    initial_prot_group_table_entry_t  prot_group_entry;
#if defined(BCM_TRIUMPH3_SUPPORT)
    rx_prot_group_table_entry_t  rx_prot_group_entry;
#endif /* BCM_TRIUMPH3_SUPPORT */
    int  num_entry;

    if (failover_id == NULL) {
        return BCM_E_PARAM;
    }

    /* Check for unsupported Flag */
    if (flags & (~(BCM_FAILOVER_REPLACE |
                   BCM_FAILOVER_WITH_ID |
                   BCM_FAILOVER_ENCAP))) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_FAILOVER_WITH_ID) || (flags & BCM_FAILOVER_REPLACE )) {

#if defined(BCM_SABER2_SUPPORT)
        if ((SOC_IS_SABER2(unit)) && (flags & BCM_FAILOVER_ENCAP)) {
            /* make sure the given id is valid */
            num_entry = 2 * soc_mem_index_count(unit, MMU_INITIAL_NHOP_TBLm) ;
            if ((*failover_id < 1) || (*failover_id >= num_entry)) {
                return BCM_E_PARAM;
            }
            if (flags & BCM_FAILOVER_WITH_ID) {
                if (_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, *failover_id)) {
                    return BCM_E_EXISTS;
                } else {
                    _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_SET(unit, *failover_id);
                }
            }
            if (flags & BCM_FAILOVER_REPLACE) {
                if (!_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, *failover_id)) {
                    return BCM_E_NOT_FOUND;
                }
            }
            _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_1_1_MC_PROTECTION_MODE); 
            return BCM_E_NONE;

        } else
#endif
        {
            /* make sure the given id is valid */
            num_entry = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
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
         _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_DEFAULT_MODE); 
    } else  if (!flags) {
         /* Get Index */
         rv = _bcm_tr2_failover_get_prot_group_table_index (unit, failover_id);
         _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_DEFAULT_MODE); 
    }
#if defined(BCM_SABER2_SUPPORT)
    else if ((SOC_IS_SABER2(unit)) && (flags & BCM_FAILOVER_ENCAP)) {
	 /* Get Index */
	    rv = _bcm_tr2_failover_get_mmu_prot_group_table_index (unit, failover_id);
	    BCM_IF_ERROR_RETURN(rv);
	    _BCM_ENCAP_TYPE_IN_FAILOVER_ID(*failover_id, _BCM_FAILOVER_1_1_MC_PROTECTION_MODE);
	     return BCM_E_NONE;
    }
#endif
    
    if (BCM_SUCCESS(rv)) {
        /* Init table entry */
        sal_memset(&prot_group_entry, 0, sizeof(initial_prot_group_table_entry_t));
        rv = soc_mem_write(unit, INITIAL_PROT_GROUP_TABLEm,
                MEM_BLOCK_ALL, *failover_id,
                &prot_group_entry);
        if (rv < 0) {
            _bcm_tr2_failover_clear_prot_group_table_entry(unit, *failover_id);
            return BCM_E_RESOURCE;
        }
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
         /* Init table entry */
         sal_memset(&rx_prot_group_entry, 0, sizeof(rx_prot_group_table_entry_t));
         rv = soc_mem_write(unit, RX_PROT_GROUP_TABLEm,
                                  MEM_BLOCK_ALL, *failover_id,
                                  &rx_prot_group_entry);
         if (rv < 0) {
              _bcm_tr2_failover_clear_prot_group_table_entry(unit, *failover_id);
              return BCM_E_RESOURCE;
         }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return rv;
}

/*
 * Function:
 *		bcm_tr2_failover_destroy
 * Purpose:
 *		Destroy a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 * Returns:
 *		BCM_E_XXX
 */

int 
bcm_tr2_failover_destroy(int unit, bcm_failover_t  failover_id)
{
    int rv = BCM_E_UNAVAIL;
    initial_prot_group_table_entry_t  prot_group_entry;
#if defined(BCM_TRIUMPH3_SUPPORT)
    rx_prot_group_table_entry_t  rx_prot_group_entry;
#endif /* BCM_TRIUMPH3_SUPPORT */

    rv = bcm_tr2_failover_id_validate ( unit, failover_id );
    BCM_IF_ERROR_RETURN(rv);

    if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
         return BCM_E_NOT_FOUND;
    }

    /* Release index */
    _bcm_tr2_failover_clear_prot_group_table_entry (unit, failover_id);

   /* Clear entry */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_GROUP_TABLEm, 
                             MEM_BLOCK_ANY, failover_id, &prot_group_entry));
    sal_memset(&prot_group_entry, 0, 
                             sizeof(initial_prot_group_table_entry_t));
    rv = soc_mem_write(unit, INITIAL_PROT_GROUP_TABLEm,
                             MEM_BLOCK_ALL, failover_id,
                             &prot_group_entry);
    if (rv < 0) {
         _bcm_tr2_failover_set_prot_group_table_entry(unit, failover_id);
         return BCM_E_RESOURCE;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
         /* Init table entry */
         sal_memset(&rx_prot_group_entry, 0, sizeof(rx_prot_group_table_entry_t));

         rv = soc_mem_write(unit, RX_PROT_GROUP_TABLEm,
                                  MEM_BLOCK_ALL, failover_id,
                                  &rx_prot_group_entry);
         if (rv < 0) {
              _bcm_tr2_failover_set_prot_group_table_entry(unit, failover_id);
              return BCM_E_RESOURCE;
         }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return rv;
}

int 
bcm_tr2_mmu_failover_destroy(int unit, bcm_failover_t  failover_id)
{
    _BCM_GET_FAILOVER_ID(failover_id);
    if (!_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
         return BCM_E_NOT_FOUND;
    }

    /* Release index */
    _bcm_tr2_failover_clear_mmu_prot_group_table_entry (unit, failover_id);
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        bcm_sb2_failover_destroy(unit, failover_id);
    }
#endif
    return BCM_E_NONE;
}
/*
 * Function:
 *		bcm_tr2_failover_set
 * Purpose:
 *		Set the parameters for  a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 *           IN :  value
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr2_failover_set(int unit, bcm_failover_t failover_id, int value)
{
    int rv = BCM_E_UNAVAIL;
    initial_prot_group_table_entry_t  prot_group_entry;

    BCM_IF_ERROR_RETURN( bcm_tr2_failover_id_validate ( unit, failover_id ));
    if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
         return BCM_E_NOT_FOUND;
    }
    if ((value < 0) || (value > 1)) {
       return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_GROUP_TABLEm, 
                   MEM_BLOCK_ANY, failover_id, &prot_group_entry));

    soc_mem_field32_set(unit, INITIAL_PROT_GROUP_TABLEm, 
                   &prot_group_entry, STATUSf, 
                   value);

    rv =  soc_mem_write(unit, INITIAL_PROT_GROUP_TABLEm,
                   MEM_BLOCK_ALL, failover_id,
                   &prot_group_entry);

    return rv;

}


/*
 * Function:
 *		bcm_tr2_failover_get
 * Purpose:
 *		Get the parameters for  a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 *           OUT :  value
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_tr2_failover_get(int unit, bcm_failover_t failover_id, int  *value)
{
    initial_prot_group_table_entry_t  prot_group_entry;

    BCM_IF_ERROR_RETURN( bcm_tr2_failover_id_validate ( unit, failover_id ));
    if (!_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, failover_id)) {
         return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_GROUP_TABLEm, 
                   MEM_BLOCK_ANY, failover_id, &prot_group_entry));

    *value =   soc_mem_field32_get(unit, INITIAL_PROT_GROUP_TABLEm,
                   &prot_group_entry, STATUSf);

    return BCM_E_NONE;
}


/*
 * Function:
 *		bcm_tr2_failover_prot_nhi_create
 * Purpose:
 *		Create  the  entry for  PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  Primary Next Hop Index
 * Returns:
 *		BCM_E_XXX
 */


int
bcm_tr2_failover_prot_nhi_create (int unit, int nh_index)
{
    initial_prot_nhi_table_entry_t  prot_nhi_entry;
    int rv = BCM_E_UNAVAIL;

    sal_memset(&prot_nhi_entry, 0, sizeof(initial_prot_nhi_table_entry_t));

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
                       MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

    return rv;

}


/*
 * Function:
 *		bcm_tr2_failover_prot_nhi_set
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
bcm_tr2_failover_prot_nhi_set(int unit, int nh_index, uint32 prot_nh_index, bcm_failover_t failover_id)
{
    initial_prot_nhi_table_entry_t   prot_nhi_entry;
    int rv = BCM_E_UNAVAIL;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, 
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_entry));

    soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
                                       &prot_nhi_entry, PROT_NEXT_HOP_INDEXf, 
                                       prot_nh_index);

    soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm, 
                                       &prot_nhi_entry, PROT_GROUPf, 
                                       (uint32) failover_id);

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
                       MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

    return rv;

}


/*
 * Function:
 *		bcm_tr2_failover_prot_nhi_get
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
bcm_tr2_failover_prot_nhi_get(int unit, int nh_index, bcm_failover_t  *failover_id, int  *prot_nh_index)
{
    initial_prot_nhi_table_entry_t   prot_nhi_entry;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, 
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_entry));

    *failover_id =   soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
                              &prot_nhi_entry, PROT_GROUPf);

    *prot_nh_index =   soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
                              &prot_nhi_entry, PROT_NEXT_HOP_INDEXf);

    return BCM_E_NONE;

}

/*
 * Function:
 *		bcm_tr2_failover_prot_nhi_cleanup
 * Purpose:
 *		Create  the  entry for  PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  Primary Next Hop Index
 * Returns:
 *		BCM_E_XXX
 */


int
bcm_tr2_failover_prot_nhi_cleanup  (int unit, int nh_index)
{
    initial_prot_nhi_table_entry_t  prot_nhi_entry;
    int rv = BCM_E_UNAVAIL;

    rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm, 
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_entry);

   if (rv < 0){
         return BCM_E_NOT_FOUND;
    }

    sal_memset(&prot_nhi_entry, 0, sizeof(initial_prot_nhi_table_entry_t));

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
                       MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

    return rv;

}

/*
 * Function:
 *    bcm_tr2_failover_prot_nhi_update
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
bcm_tr2_failover_prot_nhi_update  (int unit, int old_nh_index, int new_nh_index)
{
    int rv=BCM_E_NONE;
    int num_entry, index;
    int prot_nh_index;
    uint32 *pprot_nhi = NULL;
    uint32 *pprot_nhi_entry = NULL; 
    int entry_size; 

    num_entry = soc_mem_index_count(unit, INITIAL_PROT_NHI_TABLEm);
    entry_size = soc_mem_entry_words(unit, INITIAL_PROT_NHI_TABLEm); 
    pprot_nhi = soc_cm_salloc(unit, num_entry * entry_size * 4, "temp_buf");
    if (pprot_nhi == NULL) 
        return BCM_E_MEMORY; 
    rv = soc_mem_read_range(unit, INITIAL_PROT_NHI_TABLEm, MEM_BLOCK_ANY, 0, 
                            num_entry-1, pprot_nhi);
    if (rv == SOC_E_NONE) {
        for (index=0, pprot_nhi_entry = pprot_nhi; index < num_entry; index++) {        
            prot_nh_index = soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLEm,
                                                pprot_nhi_entry, 
                                                PROT_NEXT_HOP_INDEXf);
        
            if (prot_nh_index == old_nh_index) {
                soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLEm,
                                    pprot_nhi_entry, PROT_NEXT_HOP_INDEXf,
                                    new_nh_index);
            }    
            pprot_nhi_entry += entry_size;   
        }
        rv = soc_mem_write_range(unit, INITIAL_PROT_NHI_TABLEm, MEM_BLOCK_ANY, 
                                 0, num_entry-1, pprot_nhi);
    }
    if (pprot_nhi)
        soc_cm_sfree(unit, pprot_nhi);
    return rv;
}

/*
 * Function:
 *    bcm_tr2_failover_ecmp_prot_nhi_set
 * Purpose:
 *    Set the parameters for PROT_NHI
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  member offset from base ptr
 *    IN :  Primary Next Hop Index
 *    IN :  Protection Next Hop Index
 *    IN :  Failover Group Index
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_tr2_failover_ecmp_prot_nhi_set(int unit, int ecmp, int index, 
                                   int nh_index, 
                                   bcm_failover_t failover_id, 
                                   int prot_nh_index)
{
    int i, base_ptr = 0, count = 0, nhi, idx_max;
    uint32 hw_buf[SOC_MAX_MEM_WORDS]; /* Buffer to read hw entry. */

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    uint8 protected_ecmp = 0;
    int ecmp_max_paths = 0, rv = BCM_E_NONE;
    uint32 alloc_sz = 0;
    soc_esw_ecmp_group_info_t grp_info = {0};
    soc_esw_ecmp_member_info_t *mem_array = NULL;
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        idx_max = (soc_mem_index_count(unit, TX_PROT_GROUP_1_1_TABLEm) *
                BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    } else if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        idx_max = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    } else
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        idx_max = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                   BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    } else
#endif
    {
        idx_max = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
    }

    if (ecmp < 0) {
        base_ptr = soc_mem_index_min(unit, INITIAL_L3_ECMPm);
        count    = soc_mem_index_count(unit, INITIAL_L3_ECMPm);
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_l3_ecmp_grp_info_get(unit, ecmp, &count, &base_ptr));
    }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_esw_ecmp_protected_enabled(unit)) {
        protected_ecmp = 1;
        rv = _bcm_xgs3_ecmp_max_grp_size_get(unit, ecmp, &ecmp_max_paths);
        if (rv != BCM_E_NONE) {
            return rv;
        }

        alloc_sz = (ecmp_max_paths * sizeof(soc_esw_ecmp_member_info_t));
        mem_array = sal_alloc(alloc_sz, "NH array");
        if (NULL == mem_array) {
            return BCM_E_MEMORY;
        }

        SOC_ESW_ECMP_LOCK(unit);
        sal_memset(mem_array, 0, alloc_sz);
        /* Calculate Base_ptr table index. */
        grp_info.ecmp_grp = ecmp;
        rv = soc_esw_ecmp_protected_grp_get(unit, &grp_info, mem_array);
        if (rv != BCM_E_NONE) {
            sal_free(mem_array);
            SOC_ESW_ECMP_UNLOCK(unit);
            return rv;
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    for (i = 0; i < count; i++) {
        if (index >= 0 && index < count && index != i) {
            continue;
        }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
        if (protected_ecmp == 1) {
            nhi = mem_array[i].nh;
            if (nhi == nh_index) {
                mem_array[i].prot_nh = prot_nh_index;
                if (failover_id > 0 &&
                    failover_id < idx_max) {
                    mem_array[i].prot_grp = failover_id;
                }
            } else { /* Mismatch between offset and nh_index */
                if (index == i) {
                    sal_free(mem_array);
                    SOC_ESW_ECMP_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            }
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, INITIAL_L3_ECMPm,
                             MEM_BLOCK_ANY, base_ptr + i, hw_buf));

            nhi = soc_mem_field32_get(unit, INITIAL_L3_ECMPm, hw_buf,
                                      NEXT_HOP_INDEXf);
            if (nhi == nh_index) {
                soc_mem_field32_set(unit, INITIAL_L3_ECMPm, hw_buf,
                                    PROT_NEXT_HOP_INDEXf, (uint32)prot_nh_index);
                if (failover_id > 0 &&
                    failover_id < idx_max) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMPm, hw_buf,
                                        PROT_GROUPf, (uint32)failover_id);
                }
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit, INITIAL_L3_ECMPm,
                                  MEM_BLOCK_ANY, base_ptr + i, hw_buf));
            } else { /* Mismatch between offset and nh_index */
                if (index == i) {
                    return BCM_E_PARAM;
                }
            }
        }
    }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    if (protected_ecmp == 1) {
        soc_esw_ecmp_group_info_t_init(&grp_info);
        grp_info.ecmp_grp = ecmp;
        grp_info.ecmp_member_base = base_ptr;
        grp_info.max_paths = ecmp_max_paths;
        rv = soc_esw_ecmp_protected_grp_add(unit, &grp_info, mem_array);
        if (rv != BCM_E_NONE) {
            sal_free(mem_array);
            SOC_ESW_ECMP_UNLOCK(unit);
            return rv;
        }
        sal_free(mem_array);
        SOC_ESW_ECMP_UNLOCK(unit);

        if (base_ptr != grp_info.ecmp_member_base) {
            ecmp_count_entry_t l3_ecmp_count_entry;
            initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;
    
            BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                        base_ptr, ecmp_max_paths);
            base_ptr = grp_info.ecmp_member_base;
            BCM_XGS3_L3_ENT_REF_CNT_INC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                        base_ptr, ecmp_max_paths);
    
            sal_memset(&l3_ecmp_count_entry, 0, sizeof(l3_ecmp_count_entry));
            sal_memset(&initial_l3_ecmp_group_entry, 0, sizeof(initial_l3_ecmp_group_entry));
    
            SOC_IF_ERROR_RETURN(
                READ_INITIAL_L3_ECMP_GROUPm(unit, MEM_BLOCK_ANY,
                                            ecmp, &initial_l3_ecmp_group_entry));
            soc_INITIAL_L3_ECMP_GROUPm_field32_set(unit, &initial_l3_ecmp_group_entry,
                                                   BASE_PTRf, base_ptr);
            SOC_IF_ERROR_RETURN(
                WRITE_INITIAL_L3_ECMP_GROUPm(unit, MEM_BLOCK_ALL,
                                             ecmp, &initial_l3_ecmp_group_entry));
            SOC_IF_ERROR_RETURN(
                READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                                    ecmp, &l3_ecmp_count_entry));
            soc_L3_ECMP_COUNTm_field32_set(unit, &l3_ecmp_count_entry,
                                           BASE_PTRf, base_ptr);
            SOC_IF_ERROR_RETURN(
                WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL,
                                     ecmp, &l3_ecmp_count_entry));
    
        }

    }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    return BCM_E_NONE;
}


/*
 * Function:
 *    bcm_tr2_failover_ecmp_prot_nhi_get
 * Purpose:
 *    Get the parameters for ecmp PROT_NHI
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  member offset from base ptr
 *    IN :  primary Next Hop Index
 *    OUT : Failover Group Index
 *    OUT : Protection Next Hop Index
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_tr2_failover_ecmp_prot_nhi_get(int unit, int ecmp, int index, 
                                   int nh_index, 
                                   bcm_failover_t *failover_id,
                                   int *prot_nh_index)
{
    int i, base_ptr = 0, count = 0, nhi;
    uint32 hw_buf[SOC_MAX_MEM_WORDS]; /* Buffer to read hw entry. */
#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    uint8 protected_ecmp = 0;
    int ecmp_max_paths = 0, rv = BCM_E_NONE;
    uint32 alloc_sz = 0;
    soc_esw_ecmp_group_info_t grp_info = {0};
    soc_esw_ecmp_member_info_t *mem_array = NULL;
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */


    if (ecmp < 0) {
        base_ptr = soc_mem_index_min(unit, INITIAL_L3_ECMPm);
        count    = soc_mem_index_count(unit, INITIAL_L3_ECMPm);
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_l3_ecmp_grp_info_get(unit, ecmp, &count, &base_ptr));
    }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_esw_ecmp_protected_enabled(unit)) {
        protected_ecmp = 1;
        rv = _bcm_xgs3_ecmp_max_grp_size_get(unit, ecmp, &ecmp_max_paths);
        if (rv != BCM_E_NONE) {
            return rv;
        }

        alloc_sz = (ecmp_max_paths * sizeof(soc_esw_ecmp_member_info_t));
        mem_array = sal_alloc(alloc_sz, "NH array");
        if (NULL == mem_array) {
            return BCM_E_MEMORY;
        }

        SOC_ESW_ECMP_LOCK(unit);
        sal_memset(mem_array, 0, alloc_sz);
        grp_info.ecmp_grp = ecmp;
        rv = soc_esw_ecmp_protected_grp_get(unit, &grp_info, mem_array);
        if (BCM_FAILURE(rv)) {
            sal_free(mem_array);
            SOC_ESW_ECMP_UNLOCK(unit);
            return rv;
        }
        SOC_ESW_ECMP_UNLOCK(unit);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    for (i = 0; i < count; i++) {
        if (index >= 0 && index < count && index != i) {
            continue;
        }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
        if (protected_ecmp == 1) {
            nhi = mem_array[i].nh;
            if (nhi == nh_index) {
                *failover_id   = mem_array[i].prot_grp;
                *prot_nh_index = mem_array[i].prot_nh;
                if (*failover_id == 0 && *prot_nh_index == 0) {
                    sal_free(mem_array);
                    return BCM_E_NOT_FOUND;
                }
                break;
            } else { /* Mismatch between offset and nh_index */
                if (index == i) {
                    sal_free(mem_array);
                    return BCM_E_PARAM;
                }
            }
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */
        {

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, INITIAL_L3_ECMPm,
                             MEM_BLOCK_ANY, base_ptr + i, hw_buf));

            nhi = soc_mem_field32_get(unit, INITIAL_L3_ECMPm, hw_buf,
                                      NEXT_HOP_INDEXf);
            if (nhi == nh_index) {
                *failover_id   = soc_mem_field32_get(unit, INITIAL_L3_ECMPm, hw_buf,
                                                     PROT_GROUPf);
                *prot_nh_index = soc_mem_field32_get(unit, INITIAL_L3_ECMPm, hw_buf,
                                                     PROT_NEXT_HOP_INDEXf);
                if (*failover_id == 0 && *prot_nh_index == 0) {
                    return BCM_E_NOT_FOUND;
                }
                break;
            } else { /* Mismatch between offset and nh_index */
                if (index == i) {
                    return BCM_E_PARAM;
                }
            }
        }
    }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    sal_free(mem_array);
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcm_tr2_failover_prot_nhi_cleanup
 * Purpose:
 *    Create  the  entry for  PROT_NHI
 * Parameters:
 *    IN :  unit
 *    IN :  ecmp index /dvp group pointer
 *    IN :  member offset from base ptr
 *    IN :  Primary Next Hop Index
 * Returns:
 *    BCM_E_XXX
 */


int
bcm_tr2_failover_ecmp_prot_nhi_cleanup(int unit, int ecmp, int index,
                                       int nh_index)
{
    int i, base_ptr = 0, count = 0, nhi;
    uint32 hw_buf[SOC_MAX_MEM_WORDS]; /* Buffer to read hw entry. */

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    uint8 protected_ecmp = 0;
    int ecmp_max_paths = 0, rv = BCM_E_NONE;
    uint32 alloc_sz = 0;
    soc_esw_ecmp_group_info_t grp_info = {0};
    soc_esw_ecmp_member_info_t *mem_array = NULL;
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    if (ecmp < 0) {
        base_ptr = soc_mem_index_min(unit, INITIAL_L3_ECMPm);
        count    = soc_mem_index_count(unit, INITIAL_L3_ECMPm);
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_l3_ecmp_grp_info_get(unit, ecmp, &count, &base_ptr));
    }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_esw_ecmp_protected_enabled(unit)) {
        protected_ecmp = 1;
        rv = _bcm_xgs3_ecmp_max_grp_size_get(unit, ecmp, &ecmp_max_paths);
        if (rv != BCM_E_NONE) {
            return rv;
        }

        alloc_sz = (ecmp_max_paths * sizeof(soc_esw_ecmp_member_info_t));
        mem_array = sal_alloc(alloc_sz, "NH array");
        if (NULL == mem_array) {
            return BCM_E_MEMORY;
        }

        SOC_ESW_ECMP_LOCK(unit);
        sal_memset(mem_array, 0, alloc_sz);
        /* Calculate Base_ptr table index. */
        grp_info.ecmp_grp = ecmp;
        rv = soc_esw_ecmp_protected_grp_get(unit, &grp_info, mem_array);
        if (rv != BCM_E_NONE) {
            sal_free(mem_array);
            SOC_ESW_ECMP_UNLOCK(unit);
            return rv;
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    for (i = 0; i < count; i++) {
        if (index >= 0 && index < count && index != i) {
            continue;
        }
#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
        if (protected_ecmp == 1) {
            nhi = mem_array[i].nh;
            if (nhi == nh_index) {
                mem_array[i].prot_nh = 0;
                mem_array[i].prot_grp = 0;
            } else { /* Mismatch between offset and nh_index */
                if (index == i) {
                    sal_free(mem_array);
                    SOC_ESW_ECMP_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            }
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

        {
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, INITIAL_L3_ECMPm,
                             MEM_BLOCK_ANY, base_ptr + i, hw_buf));

            nhi = soc_mem_field32_get(unit, INITIAL_L3_ECMPm, hw_buf,
                                      NEXT_HOP_INDEXf);
            if (nhi == nh_index) {
                soc_mem_field32_set(unit, INITIAL_L3_ECMPm, hw_buf,
                                    PROT_NEXT_HOP_INDEXf, 0);
                soc_mem_field32_set(unit, INITIAL_L3_ECMPm, hw_buf,
                                    PROT_GROUPf, 0);
                BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit, INITIAL_L3_ECMPm,
                                  MEM_BLOCK_ANY, base_ptr + i, hw_buf));
            } else { /* Mismatch between offset and nh_index */
                if (index == i) {
                    return BCM_E_PARAM;
                }
            }
        }
    }

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
#if defined(BCM_TRIDENT3_SUPPORT)
    if (protected_ecmp == 1) {
        soc_esw_ecmp_group_info_t_init(&grp_info);
        grp_info.ecmp_grp = ecmp;
        grp_info.ecmp_member_base = base_ptr;
        grp_info.max_paths = ecmp_max_paths;
        rv = soc_esw_ecmp_protected_grp_add(unit, &grp_info, mem_array);
        if (rv != BCM_E_NONE) {
            sal_free(mem_array);
            SOC_ESW_ECMP_UNLOCK(unit);
            return rv;
        }
        sal_free(mem_array);
        SOC_ESW_ECMP_UNLOCK(unit);

        if (base_ptr != grp_info.ecmp_member_base) {
            ecmp_count_entry_t l3_ecmp_count_entry;
            initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;

            BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                        base_ptr, ecmp_max_paths);
            base_ptr = grp_info.ecmp_member_base;
            BCM_XGS3_L3_ENT_REF_CNT_INC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                        base_ptr, ecmp_max_paths);

            sal_memset(&l3_ecmp_count_entry, 0, sizeof(l3_ecmp_count_entry));
            sal_memset(&initial_l3_ecmp_group_entry, 0, sizeof(initial_l3_ecmp_group_entry));

            SOC_IF_ERROR_RETURN(
                READ_INITIAL_L3_ECMP_GROUPm(unit, MEM_BLOCK_ANY,
                                            ecmp, &initial_l3_ecmp_group_entry));
            soc_INITIAL_L3_ECMP_GROUPm_field32_set(unit, &initial_l3_ecmp_group_entry,
                                                   BASE_PTRf, base_ptr);
            SOC_IF_ERROR_RETURN(
                WRITE_INITIAL_L3_ECMP_GROUPm(unit, MEM_BLOCK_ALL,
                                             ecmp, &initial_l3_ecmp_group_entry));
            SOC_IF_ERROR_RETURN(
                READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                                    ecmp, &l3_ecmp_count_entry));
            soc_L3_ECMP_COUNTm_field32_set(unit, &l3_ecmp_count_entry,
                                           BASE_PTRf, base_ptr);
            SOC_IF_ERROR_RETURN(
                WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL,
                                     ecmp, &l3_ecmp_count_entry));
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcm_tr2_failover_id_check
 * Purpose:
 *    Check Failover identifier
 * Parameters:
 *    IN :  unit
 *    IN :  failover_id
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_tr2_failover_id_check(int unit, bcm_failover_t failover_id)
{
    int num_count;
#if defined(BCM_SABER2_SUPPORT)
    int type;
    _BCM_GET_FAILOVER_TYPE(failover_id, type);
    if (type & _BCM_FAILOVER_1_1_MC_PROTECTION_MODE) {
        _BCM_GET_FAILOVER_ID(failover_id);
        num_count = 2 * soc_mem_index_count(unit, MMU_INITIAL_NHOP_TBLm);
        if (failover_id > 0 && failover_id < num_count) {
            return (BCM_E_NONE);
        }
    } else 
#endif
    {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
       if (soc_feature(unit, soc_feature_hierarchical_protection)) {
            num_count = soc_mem_index_count(unit, TX_PROT_GROUP_1_1_TABLEm);
        } else if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
            num_count = soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm);
        } else
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            num_count = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                         BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
        } else
#endif
        {
            num_count = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
        }
        if (failover_id > 0 && failover_id < num_count) {
            return (BCM_E_NONE);
        }
    }
    return (BCM_E_PARAM);
}

/*
 * Function:
 *    bcm_tr2_failover_egr_check
 * Purpose:
 *    Check Failover for Egress Object
 * Parameters:
 *    IN :  unit
 *    IN :  Egress Object
 * Returns:
 *    BCM_E_XXX
 */


int
bcm_tr2_failover_egr_check (int unit, bcm_l3_egress_t  *egr)
{
    if (egr->failover_id > 0 && egr->failover_id < 1024) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, egr->failover_if_id) || 
            BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, egr->failover_if_id)) {
             return (BCM_E_NONE);
        }
    }
    return (BCM_E_PARAM);
}

/*
 * Function:
 *    bcm_tr2_failover_mpls_check
 * Purpose:
 *    Check for failover enable - MPLS port
 * Parameters:
 *    IN :  unit
 *    IN :  Egress Object
 * Returns:
 *    BCM_E_XXX
 */


int
bcm_tr2_failover_mpls_check (int unit, bcm_mpls_port_t  *mpls_port)
{
    int failover_vp = -1;

    if ( ( (mpls_port->failover_id) > 0)  &&  ( (mpls_port->failover_id) < 1024) ) {
         /* Get egress next-hop index from Failover MPLS gport */
         failover_vp = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->failover_port_id);
         if (failover_vp == -1) {
             return BCM_E_PARAM;
         }
         if ( (failover_vp >= 1)  && (failover_vp < soc_mem_index_count(unit, SOURCE_VPm)) ) {
             return BCM_E_NONE;
         }
    }
    return BCM_E_PARAM;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_tr2_failover_sw_dump
 * Purpose:
 *     Displays State information maintained by software
 *     for 'Failover' module.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     Void
 */
void
_bcm_tr2_failover_sw_dump(int unit)
{
    int  i;
    int  num_entry = 0;

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_hierarchical_protection)) {
        num_entry = (soc_mem_index_count(unit, TX_PROT_GROUP_1_1_TABLEm) *
                     BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    } else if (soc_feature(unit, soc_feature_td2p_mpls_linear_protection)) {
        num_entry = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                     BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY);
    } else
#endif
#if defined (BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        num_entry = (soc_mem_index_count(unit, TX_INITIAL_PROT_GROUP_TABLEm) *
                     BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
    {
        num_entry = soc_mem_index_count(unit, INITIAL_PROT_GROUP_TABLEm);
    }

    LOG_CLI((BSL_META_U(unit,
                        "Protection Group usage bitmap:\n")));

    for (i = 0; i < num_entry; i++) {
         if (_BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(unit, i)) {
             LOG_CLI((BSL_META_U(unit,
                                 "%d "), i));     
         }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
#if defined (BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        num_entry = (soc_mem_index_count(unit, RX_PROT_GROUP_TABLEm) *
                     BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY);
        LOG_CLI((BSL_META_U(unit,
                            "Ingress Protection Group usage bitmap:\n")));
    
        for (i = 0; i < num_entry; i++) {
             if (_BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(unit, i)) {
                 LOG_CLI((BSL_META_U(unit,
                                     "%d "), i));     
             }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));    
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

