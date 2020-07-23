/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Saber2 failover API
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_SABER2_SUPPORT) &&  defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/failover.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/triumph2.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */


typedef struct _bcm_failover_keeper_s {
    int primary_nhi;
    int secondary_nhi;
} _bcm_failover_keeper_t;
STATIC _bcm_failover_keeper_t *failover_keeper[SOC_MAX_NUM_DEVICES];


#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

    STATIC int
_bcm_sb2_failover_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size;


    alloc_size = 3 * sizeof(uint32) * 2 * soc_mem_index_count(unit,MMU_INITIAL_NHOP_TBLm);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
            &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

    int
bcm_sb2_failover_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    uint32 index;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    ptr = scache_ptr;
    for (index = 0; index < (2 * soc_mem_index_count (unit, MMU_INITIAL_NHOP_TBLm));
            index++)
    {
        *(((uint32 *)ptr)) = failover_keeper[unit][index].primary_nhi;
        ptr += sizeof(uint32);
        *(((uint32 *)ptr)) = failover_keeper[unit][index].secondary_nhi;
        ptr += sizeof(uint32);
        if (_BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(unit, index)) {
            *(((uint32 *)ptr)) = 0x1;
        } else {
            *(((uint32 *)ptr)) = 0x0;
        }
        ptr += sizeof(uint32);
    }
    return BCM_E_NONE; 
}

    int
bcm_sb2_failover_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    int stable_size, rv;
    uint16 recovered_ver;
    int index;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        /* COSQ warmboot requires extended scache support i.e. level2 warmboot*/
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FAILOVER, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &scache_ptr,
            BCM_WB_DEFAULT_VERSION,  &recovered_ver);
    if (BCM_E_NOT_FOUND == rv) {
        /* Upgrading from SDK release that does not have warmboot state */
        BCM_IF_ERROR_RETURN(_bcm_sb2_failover_wb_alloc(unit));
        return BCM_E_NONE;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    ptr = scache_ptr;

    for (index = 0; index < (2 * soc_mem_index_count (unit, MMU_INITIAL_NHOP_TBLm));
            index++) {
        failover_keeper[unit][index].primary_nhi = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
        failover_keeper[unit][index].secondary_nhi = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
        if (*(((uint32 *)ptr)) == 1) {
            _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_SET(unit, index);
        }
        ptr += sizeof(uint32);
    }
    return BCM_E_NONE; 
}
#endif 

int
bcm_sb2_failover_init(int unit)
{
    int index;
    if (failover_keeper[unit] == NULL) {
        failover_keeper[unit] =
            sal_alloc((sizeof(_bcm_failover_keeper_t) * 
                        2 * soc_mem_index_count(unit,MMU_INITIAL_NHOP_TBLm )), 
                    "failover_keeper");
    }
    for (index = 0; index < (2 * soc_mem_index_count(unit,MMU_INITIAL_NHOP_TBLm ));
            index++) { 
        failover_keeper[unit][index].primary_nhi = -1;
        failover_keeper[unit][index].secondary_nhi = -1;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_sb2_failover_reinit(unit);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_sb2_failover_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */


    return BCM_E_NONE;
}

int
bcm_sb2_failover_cleanup(int unit)
{
    if (failover_keeper[unit]) {
        sal_free(failover_keeper[unit]);
        failover_keeper[unit] = NULL;
     }
 
    return BCM_E_NONE;

}

int
bcm_sb2_failover_destroy(int unit, int failover_id) 
{
    mmu_initial_nhop_tbl_entry_t  mmu_prot_nhi_entry;
    soc_field_t field[4] =
    {
        PROT_DROP_STATUS0f,
        PROT_DROP_STATUS1f,
        PROT_DROP_STATUS2f,
        PROT_DROP_STATUS3f
    };
    if (failover_keeper[unit][failover_id].primary_nhi != -1) {

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                    MEM_BLOCK_ANY, 
                    failover_keeper[unit][failover_id].primary_nhi / 4,
                    &mmu_prot_nhi_entry));

        soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                &mmu_prot_nhi_entry,
                field[failover_keeper[unit][failover_id].primary_nhi % 4] , 0);

        BCM_IF_ERROR_RETURN (soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                    MEM_BLOCK_ALL, failover_keeper[unit][failover_id].primary_nhi / 4 ,
                    &mmu_prot_nhi_entry));

        BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                    MEM_BLOCK_ANY, 
                    failover_keeper[unit][failover_id].secondary_nhi / 4,
                    &mmu_prot_nhi_entry));

        soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                &mmu_prot_nhi_entry,
                field[failover_keeper[unit][failover_id].secondary_nhi % 4] , 0);

        BCM_IF_ERROR_RETURN (soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                    MEM_BLOCK_ALL, failover_keeper[unit][failover_id].secondary_nhi / 4 ,
                    &mmu_prot_nhi_entry));

        failover_keeper[unit][failover_id].primary_nhi = -1;
        failover_keeper[unit][failover_id].secondary_nhi = -1;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *		bcm_sb2_failover_prot_nhi_set
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
bcm_sb2_failover_prot_nhi_set(int unit, uint32 flags, int nh_index, uint32 prot_nh_index, 
                                           bcm_multicast_t  mc_group, bcm_failover_t failover_id)
{
    _BCM_GET_FAILOVER_ID(failover_id);
    if (failover_keeper[unit][failover_id].primary_nhi != -1) {
        return BCM_E_PARAM;
    }
    failover_keeper[unit][failover_id].primary_nhi = nh_index;
    failover_keeper[unit][failover_id].secondary_nhi = prot_nh_index;

    return BCM_E_NONE;

}


/*
 * Function: bcm_sb2_failover_prot_nhi_get
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
bcm_sb2_failover_prot_nhi_get(int unit, int nh_index, 
            bcm_failover_t  *failover_id, int  *prot_nh_index, bcm_multicast_t  *mc_group)
{

    _BCM_GET_FAILOVER_ID(*failover_id);

    *prot_nh_index = failover_keeper[unit][*failover_id].secondary_nhi;

    return BCM_E_NONE;

}


/*
 * Function:
 *        bcm_sb2_failover_status_set
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
bcm_sb2_failover_status_set(int unit,
                                     bcm_failover_element_t *failover,
                                     int value)
{
    int rv = BCM_E_UNAVAIL;
    mmu_initial_nhop_tbl_entry_t  mmu_prot_nhi_entry;
    soc_field_t field[4] =
    {
        PROT_DROP_STATUS0f,
        PROT_DROP_STATUS1f,
        PROT_DROP_STATUS2f,
        PROT_DROP_STATUS3f
    };

    if ((value < 0) || (value > 1)) {
       return BCM_E_PARAM;
    }
    _BCM_GET_FAILOVER_ID(failover->failover_id);
    if ((failover_keeper[unit][failover->failover_id].secondary_nhi == -1) ||
        (failover_keeper[unit][failover->failover_id].primary_nhi == -1)) {
       return BCM_E_INTERNAL;
    }
    if (failover->failover_id != BCM_FAILOVER_INVALID) {
         /* Group protection for Port and Tunnel: Egress and Ingress */
         BCM_IF_ERROR_RETURN(
              bcm_tr2_failover_mmu_id_validate ( unit, failover->failover_id ));

         if (value) {
             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[unit][failover->failover_id].secondary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[unit][failover->failover_id].secondary_nhi % 4] , 0);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[unit][failover->failover_id].secondary_nhi / 4 ,
                     &mmu_prot_nhi_entry);

             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[unit][failover->failover_id].primary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[unit][failover->failover_id].primary_nhi % 4] , 1);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[unit][failover->failover_id].primary_nhi / 4 ,
                     &mmu_prot_nhi_entry);
         } else {
             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[unit][failover->failover_id].primary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[unit][failover->failover_id].primary_nhi % 4] , 0);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[unit][failover->failover_id].primary_nhi / 4 ,
                     &mmu_prot_nhi_entry);

             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[unit][failover->failover_id].secondary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[unit][failover->failover_id].secondary_nhi % 4] , 1);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[unit][failover->failover_id].secondary_nhi / 4 ,
                     &mmu_prot_nhi_entry);

         }
    } else {
       return BCM_E_PARAM;
    } 
    return rv;
}


/*
 * Function:
 *        bcm_sb2_failover_status_get
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
bcm_sb2_failover_status_get(int unit,
                                     bcm_failover_element_t *failover,
                                     int  *value)
{
    mmu_initial_nhop_tbl_entry_t   prot_nhi_entry;
    soc_field_t field[4] =
    {
        PROT_DROP_STATUS0f,
        PROT_DROP_STATUS1f,
        PROT_DROP_STATUS2f,
        PROT_DROP_STATUS3f
    };

    _BCM_GET_FAILOVER_ID(failover->failover_id);
    if (failover->failover_id != BCM_FAILOVER_INVALID) {

         BCM_IF_ERROR_RETURN(
            bcm_tr2_failover_mmu_id_validate ( unit, failover->failover_id ));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ANY, 
                     failover_keeper[unit][failover->failover_id].secondary_nhi / 4,
                     &prot_nhi_entry));

         *value =   soc_mem_field32_get(unit, MMU_INITIAL_NHOP_TBLm,
                 &prot_nhi_entry, 
                 field[failover_keeper[unit][failover->failover_id].secondary_nhi % 4]);
         *value = *value ? 0 : 1 ;
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#endif /* defined(BCM_SABER2_SUPPORT) &&  defined(INCLUDE_L3) */
