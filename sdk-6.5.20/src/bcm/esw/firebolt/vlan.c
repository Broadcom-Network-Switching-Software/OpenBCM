/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vlan.c
 * Purpose:     Provide low-level access to XGS3 VLAN resources
 */

#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/vlan.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
/* #include <bcm_int/esw/firebolt.h> */
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/common/multicast.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#include <bcm_int/common/multicast.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/trill.h>

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/virtual.h>
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif

#define TABLE_HAS_UT_BITMAP(unit, table)                        \
     ((SOC_IS_FBX(unit) && \
      (!soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) && \
      (table) == EGR_VLANm) || \
     (SOC_IS_KATANA2 (unit) && (table) == VLAN_TABm))

#define TABLE_HAS_T_BITMAP(unit, table)         \
    (((table) == VLAN_TABLE(unit)) ||           \
     (soc_feature(unit, soc_feature_egr_vlan_check) && (table) == EGR_VLANm))

#define TABLE_HAS_PFM(unit, table)                              \
    (((table) == VLAN_TABm) ||                                  \
     (soc_feature(unit, soc_feature_egr_vlan_pfm) && (table) == EGR_VLANm))

#ifdef BCM_KATANA_SUPPORT
#define _BCM_QOS_MAP_TYPE_MASK                     0x3ff
#define _BCM_QOS_MAP_SHIFT                            10
#define _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE   5
#define _BCM_QOS_MAP_ING_QUEUE_OFFSET_MAX              7
#endif

#define BCM_PROTO_PKT_CONTROL_MAX    64
/*
 * Function :
 *    _bcm_vlan_mcast_flood_mode_to_pfm
 * 
 * Purpose  :
 *    Encode vlan flood mode to pfm value. 
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     mode  - (IN) Vlan flood mode. 
 *     pfm   - (OUT) PFM value.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_vlan_mcast_flood_mode_to_pfm(int unit, int mode, int *pfm)
{
    COMPILER_REFERENCE(unit);

    if (NULL == pfm) {
        return (BCM_E_PARAM);
    }

    switch (mode) {
      case BCM_VLAN_MCAST_FLOOD_ALL:
          *pfm = 0;
          break;
      case BCM_VLAN_MCAST_FLOOD_NONE: 
          *pfm = 2;
          break;
      default: 
          *pfm = 1;
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _bcm_vlan_mcast_pfm_to_flood_mode
 * 
 * Purpose  :
 *    Decode pfm value to vlan flood mode enumerator.
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     pfm   - (IN) PFM value.
 *     mode  - (OUT) Vlan flood mode. 
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_vlan_mcast_pfm_to_flood_mode(int unit, int pfm, 
                                  bcm_vlan_mcast_flood_t *mode)
{
    COMPILER_REFERENCE(unit);

    if (NULL == mode) {
        return (BCM_E_PARAM);
    }

    switch (pfm) {
      case 0: 
          *mode = BCM_VLAN_MCAST_FLOOD_ALL;
          break;
      case 1: 
          *mode = BCM_VLAN_MCAST_FLOOD_UNKNOWN;
          break;
      case 2: 
          *mode = BCM_VLAN_MCAST_FLOOD_NONE;
          break;
      default: 
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
typedef struct _vlan_outer_tpid_s {
    uint16                     tpid;
    int                        ref_count;
} _vlan_outer_tpid_t;

#define BCM_MAX_TPID_ENTRIES 4

STATIC _vlan_outer_tpid_t
           (*_vlan_outer_tpid_tab[BCM_MAX_NUM_UNITS])[BCM_MAX_TPID_ENTRIES];
STATIC sal_mutex_t _fb2_outer_tpid_lock[BCM_MAX_NUM_UNITS];

#define OUTER_TPID_TAB(unit) _vlan_outer_tpid_tab[unit]

#define OUTER_TPID_ENTRY(unit, index) \
    ((*_vlan_outer_tpid_tab[unit])[index].tpid)

#define OUTER_TPID_REF_COUNT(unit, index) \
    ((*_vlan_outer_tpid_tab[unit])[index].ref_count)

#define OUTER_TPID_TAB_INIT_CHECK(unit) \
    if (OUTER_TPID_TAB(unit) == NULL) { return BCM_E_INIT; }

#define BCM_FB2_TPID_MUTEX(_u_) _fb2_outer_tpid_lock[_u_]

#define BCM_FB2_TPID_LOCK(_u_)   \
         ((_fb2_outer_tpid_lock[_u_]) ? \
         sal_mutex_take(_fb2_outer_tpid_lock[_u_], sal_mutex_FOREVER) :  \
         (BCM_E_INTERNAL))

#define BCM_FB2_TPID_UNLOCK(_u_)  \
         ((_fb2_outer_tpid_lock[_u_]) ? \
         sal_mutex_give(_fb2_outer_tpid_lock[_u_]) : \
         (BCM_E_INTERNAL))


/*
 * Function:
 *      _bcm_fb2_outer_tpid_detach
 * Purpose:
 *      De-allocate and initialize memory to cache tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_fb2_outer_tpid_detach(int unit) 
{
    sal_free (OUTER_TPID_TAB(unit));
    OUTER_TPID_TAB(unit) = NULL;

    if (NULL != BCM_FB2_TPID_MUTEX(unit)) {
        sal_mutex_destroy(BCM_FB2_TPID_MUTEX(unit));
        BCM_FB2_TPID_MUTEX(unit) = NULL;
    }
    return (BCM_E_NONE); 
}

/*
 * Function:
 *      _bcm_fb2_outer_tpid_init 
 * Purpose:
 *      Allocate and initialize memory to cache tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_fb2_outer_tpid_init(int unit) {
    int               index;
    int               alloc_size;
    bcm_port_t        port;
    uint32            reg32;
    int               enabled;
    uint32            value; 
    bcm_pbmp_t        all_pbmp;

    /* Allocate memory to cache ING_OUTER_TPID entries.
     * ING_OUTER_TPID entries are always the same as
     * EGR_OUTER_TPID entries.
     */

    alloc_size = sizeof(_vlan_outer_tpid_t) * BCM_MAX_TPID_ENTRIES;
    if (OUTER_TPID_TAB(unit) == NULL) {
        OUTER_TPID_TAB(unit) = sal_alloc(alloc_size,
                                         "Cached ingress outer TPIDs");
        if (OUTER_TPID_TAB(unit) == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(OUTER_TPID_TAB(unit), 0, alloc_size);
    
    /* Cache ingress outer TPID table */
    for (index = 0; index < BCM_MAX_TPID_ENTRIES; index++) {
        uint32 outer_tpid = 0;

        if (SOC_IS_TRIDENT3X(unit)) {
            soc_reg_t ing_outer_tpid_reg[] = {
                ING_OUTER_TPID_0r,
                ING_OUTER_TPID_1r,
                ING_OUTER_TPID_2r,
                ING_OUTER_TPID_3r
            };
            if (SOC_WARM_BOOT(unit)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg32_get(unit, ing_outer_tpid_reg[index], REG_PORT_ANY,
                                   0, &outer_tpid));
                OUTER_TPID_ENTRY(unit, index) =
                    soc_reg_field_get(unit, ing_outer_tpid_reg[index], outer_tpid, TPIDf);
            } else {
                uint32 ing_outer_tpid_init_val[] = {
                    0x8100,
                    0x9100,
                    0x88a8,
                    0xaa8a
                };
                soc_reg_field_set(unit, ing_outer_tpid_reg[index],
                                  &outer_tpid, TPIDf,
                                  ing_outer_tpid_init_val[index]);
                BCM_IF_ERROR_RETURN
                    (soc_reg32_set(unit, ing_outer_tpid_reg[index], REG_PORT_ANY,
                                   0, outer_tpid));

                OUTER_TPID_ENTRY(unit, index) = ing_outer_tpid_init_val[index];
            }
        } else {
        BCM_IF_ERROR_RETURN
            (READ_ING_OUTER_TPIDr(unit, index, &outer_tpid));
            OUTER_TPID_ENTRY(unit, index) =
                soc_reg_field_get(unit, ING_OUTER_TPIDr, outer_tpid, TPIDf);
        }
    }

    /* All port has a reference to TPID */
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
    }
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
    }
#endif

    PBMP_ITER(all_pbmp, port) {
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_egr_lport_field_get(unit, port,
                    EGR_VLAN_CONTROL_1m, OUTER_TPID_INDEXf, (void *)&index));
            } else {
            BCM_IF_ERROR_RETURN(
                READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &value));
                index = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_1m, &value,
                                            OUTER_TPID_INDEXf);
            }
        } else {
            BCM_IF_ERROR_RETURN
                (READ_EGR_VLAN_CONTROL_1r(unit, port, &reg32));
            index = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r, 
                                  reg32, OUTER_TPID_INDEXf);
        }
       
        if (index < BCM_MAX_TPID_ENTRIES) {
        OUTER_TPID_REF_COUNT(unit, index)++;
        }

        enabled = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_get(unit, port,
                                      _bcmPortOuterTpidEnables, &enabled));
        index = 0;
        while(enabled) {
            if (enabled & 1) {
                OUTER_TPID_REF_COUNT(unit, index)++;
            }
            enabled = enabled >> 1;
            index++;
        } 
    } 

    if (soc_mem_is_valid(unit, SYSTEM_CONFIG_TABLEm) &&
                                    !SOC_WARM_BOOT(unit)) {
        int i, rv;
        int i_min;
        int i_max;
        void *buf = NULL;
        soc_mem_t mem = SYSTEM_CONFIG_TABLEm;
        system_config_table_entry_t *entry;

        buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem), "syscfg_tbl");
        if (buf == NULL) {
            return BCM_E_MEMORY;
        }

        i_min = soc_mem_index_min(unit, mem);
        i_max = soc_mem_index_max(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, i_min, i_max, buf);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, buf);
            return rv;
        }

        for (i = i_min; i <= i_max; i++) {
            entry = soc_mem_table_idx_to_pointer(unit, mem,
                        system_config_table_entry_t *, buf, i);
            enabled = soc_mem_field32_get(unit, mem, entry,
                                          OUTER_TPID_ENABLEf);
            index = 0;
            while (enabled) {
                if (enabled & 1) {
                    OUTER_TPID_REF_COUNT(unit, index)++;
                }
                enabled = enabled >> 1;
                index++;
            }
        }
        soc_cm_sfree(unit, buf);
    }

    if (NULL == BCM_FB2_TPID_MUTEX(unit)) {
        /* Create protection mutex. */
        BCM_FB2_TPID_MUTEX(unit) = sal_mutex_create("outer_tpid_lock");

        if (NULL == BCM_FB2_TPID_MUTEX(unit)) {
            return (BCM_E_MEMORY);
        }
    }
    return (BCM_E_NONE); 
}

/*
 * Function:
 *      _bcm_fb2_priority_map_init 
 * Purpose:
 *      Initialize the priority map table so that the incomming
 *      and outgoing priority are the same if the switch logic doesn't
 *      change it.    
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_fb2_priority_map_init(int unit)
{
    int          priority;
    int          cfi;
    bcm_color_t  color;
    bcm_port_t   port;
    bcm_color_t  color_array[] = {bcmColorGreen, bcmColorYellow, bcmColorRed};
    int          index;
    bcm_pbmp_t   all_pbmp;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
    }
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
    }
#endif

    if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit)
        || !soc_feature(unit, soc_feature_qos_profile)) {
        PBMP_ITER(all_pbmp, port) {
            for (cfi = 0; cfi <= 1; cfi++) {
                for (priority = 0; priority <= 7; priority++){
                    color = _BCM_COLOR_DECODING(unit, cfi);
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_port_vlan_priority_map_set
                            (unit, port, priority, cfi, priority, color));
                }
            }
        }
    }

    PBMP_ITER(all_pbmp, port) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                              EGR_QOS_PROFILE_INDEXf,0));
        } else if (SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                              EGR_QOS_PROFILE_INDEXf,0));
        } else
#endif
        {
            for (index = 0; index < 3; index++) {
                color = color_array[index];
                for (priority = 0; priority <= 7; priority++) {
                    cfi = (color == bcmColorRed) ? 1 : 0;
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_port_vlan_priority_unmap_set(unit, port, priority, 
                                                              color, priority, cfi));
                }
            }    
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_entry_get
 * 
 * Purpose  :
 *    Get tpid value for tpid entry index . 
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     tpid  - (OUT) TPID value.
 *     index - (IN) Entry index.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_fb2_outer_tpid_entry_get(int unit, uint16 *tpid, int index) 
{
    if ((index < 0) || (index >= BCM_MAX_TPID_ENTRIES) ||
        (OUTER_TPID_REF_COUNT(unit, index) <= 0)) {
        return (BCM_E_PARAM);
    }

    *tpid = OUTER_TPID_ENTRY(unit, index);
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_entry_delete
 * 
 * Purpose  :
 *    Delete tpid entry by index.
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     index - (IN) Entry index.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_fb2_outer_tpid_entry_delete(int unit, int index) 
{
    int rv = BCM_E_NONE; 

    _bcm_fb2_outer_tpid_tab_lock(unit);

    if ((index < 0) || (index >= BCM_MAX_TPID_ENTRIES) ||
        (OUTER_TPID_REF_COUNT(unit, index) <= 0)) {
        rv = BCM_E_PARAM;
        goto _cleanup_bcm_fb2_outer_tpid_entry_delete;
    }

    OUTER_TPID_REF_COUNT(unit, index)--;

#if defined(BCM_TRX_SUPPORT)
    if ((0 == OUTER_TPID_REF_COUNT(unit, index)) && (SOC_IS_SC_CQ(unit)) &&
        (0 == SOC_IS_SHADOW(unit))) {
        rv = _bcm_trx_egr_src_port_outer_tpid_set(unit, index, FALSE);
        if (BCM_FAILURE(rv)) {
            goto _cleanup_bcm_fb2_outer_tpid_entry_delete;
        }
    }
#endif /* BCM_TRX_SUPPORT */

_cleanup_bcm_fb2_outer_tpid_entry_delete:
    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return (rv);
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_default_get
 * 
 * Purpose  :
 *    Get system default tpid value. 
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *
 * Return :
 *    BCM_E_XXX
 */
uint16
_bcm_fb2_outer_tpid_default_get(int unit)
{
    COMPILER_REFERENCE(unit);
    return 0x8100;
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_lkup
 * 
 * Purpose  :
 *    Get tpid entry index for specific tpid value.
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     tpid  - (IN) TPID value.
 *     index - (OUT) Entry index.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_fb2_outer_tpid_lkup(int unit, uint16 tpid, int *index)
{
    int i;

    for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
        if (OUTER_TPID_ENTRY(unit, i) == tpid) {
            *index = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_fb2_outer_tpid_entry_add 
 * Purpose:
 *      Add a new TPID entry.
 *      Allocate and initialize memory to cache tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      tpid       - (IN) TPID to be added.
 *      index      - (OUT) Index where the the new TPID is added.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the same TPID already exists, simple increase the
 * reference count of the cached entry. Otherwise, add the entry
 * to the cached table and write the new entry to hardware.
 * Only four distinct TPID values are currently supported.
 */
int
_bcm_fb2_outer_tpid_entry_add(int unit, uint16 tpid, int *index)
{
    int i, free_index;
    uint32 tpid_reg = 0;
    int rv = BCM_E_NONE;
    soc_reg_t ing_outer_tpid_reg[] = {
        ING_OUTER_TPID_0r,
        ING_OUTER_TPID_1r,
        ING_OUTER_TPID_2r,
        ING_OUTER_TPID_3r
    };

    _bcm_fb2_outer_tpid_tab_lock(unit);
    /*
     * Search for an existing entry.
     */
    free_index = -1;
    for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
        if (OUTER_TPID_ENTRY(unit, i) == tpid) {
            OUTER_TPID_REF_COUNT(unit, i)++;
            *index = i;
            goto _cleanup_bcm_fb2_outer_tpid_entry_add;
        }
        if (OUTER_TPID_REF_COUNT(unit, i) == 0) {
            free_index = i;
        }
    } 

    if (free_index < 0) {
        rv = BCM_E_RESOURCE;
        goto _cleanup_bcm_fb2_outer_tpid_entry_add;
    }

    /* 
     * Insert the new configuration into tpid table if a free
     * entry is available.
     */
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_reg_field_set(unit, ing_outer_tpid_reg[free_index], &tpid_reg, TPIDf, tpid);
        rv = soc_reg32_set(unit, ing_outer_tpid_reg[free_index], REG_PORT_ANY, 0, tpid_reg);
    } else {
    soc_reg_field_set(unit, ING_OUTER_TPIDr, &tpid_reg, TPIDf, tpid);
    rv = WRITE_ING_OUTER_TPIDr(unit, free_index, tpid_reg);
    }
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_fb2_outer_tpid_entry_add;
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    /* Skip EGR_OUTER_TPID for low latency mode, will be set via EGR_VLAN_TAG_ACTION_FOR_BYPASSr */
    if (!(soc_feature(unit, soc_feature_fast_egr_vlan_action))) {
        rv = WRITE_EGR_OUTER_TPIDr(unit, free_index, tpid_reg);
    }
#else
    rv = WRITE_EGR_OUTER_TPIDr(unit, free_index, tpid_reg);
#endif
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_fb2_outer_tpid_entry_add;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_REG_IS_VALID(unit, EGR_PARSER_OUTER_TPIDr)) {
        rv = WRITE_EGR_PARSER_OUTER_TPIDr(unit, free_index, tpid_reg);
    }
    if (BCM_FAILURE(rv)) {
        goto _cleanup_bcm_fb2_outer_tpid_entry_add;
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_REG_IS_VALID(unit, ING_MPLS_TPID_0r)) {
        rv = WRITE_ING_MPLS_TPIDr(unit, free_index, tpid_reg);
        if (BCM_FAILURE(rv)) {
            goto _cleanup_bcm_fb2_outer_tpid_entry_add;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
        rv = _bcm_trx_egr_src_port_outer_tpid_set(unit, free_index, TRUE);
        if (BCM_FAILURE(rv)) {
            goto _cleanup_bcm_fb2_outer_tpid_entry_add;
        }
    }
#endif /* BCM_TRX_SUPPORT */
    OUTER_TPID_ENTRY(unit, free_index) = tpid;
    OUTER_TPID_REF_COUNT(unit, free_index)++;
    *index = free_index;

_cleanup_bcm_fb2_outer_tpid_entry_add:
    _bcm_fb2_outer_tpid_tab_unlock(unit);
    return rv;
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_tab_lock
 * 
 * Purpose  :
 *    Lock TPID values table.   
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_fb2_outer_tpid_tab_lock(int unit) 
{
    return BCM_FB2_TPID_LOCK(unit);
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_tab_unlock
 * 
 * Purpose  :
 *    Unlock TPID values table.   
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_fb2_outer_tpid_tab_unlock(int unit) 
{
    return BCM_FB2_TPID_UNLOCK(unit); 
}

/*
 * Function :
 *    _bcm_fb2_outer_tpid_tab_ref_count_add
 *
 * Purpose  :
 *    update tpid value reference count.
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     index - (IN) Table entry index. 
 *     count - (IN) Reference counter update diff.
 * 
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_fb2_outer_tpid_tab_ref_count_add(int unit, int index, int count)
{
    if (index >= BCM_MAX_TPID_ENTRIES) {
        return BCM_E_PARAM;
    }
    if ((OUTER_TPID_REF_COUNT(unit, index) + count) < 0) {
        return BCM_E_PARAM;
    }  
    _bcm_fb2_outer_tpid_tab_lock(unit);

    OUTER_TPID_REF_COUNT(unit, index) += count; 

    _bcm_fb2_outer_tpid_tab_unlock(unit);

    return BCM_E_NONE;
}

/* Flags for _vlan_profile_t.profile_flags. */
#define _BCM_VLAN_PROFILE_PHB2_ENABLE         0x00000001
#define _BCM_VLAN_PROFILE_PHB2_USE_INNER_TAG  0x00000002
#define _BCM_VLAN_PROFILE_TRUST_DOT1P         0x00000004

/*
 * Vlan profile structure.  
 *
 * Purpose  : Track per vlan configuration. 
 */
typedef struct _vlan_profile_s {
    uint16                 outer_tpid; 
    uint32                 flags; 
    bcm_vlan_mcast_flood_t ip6_mcast_flood_mode; 
    bcm_vlan_mcast_flood_t ip4_mcast_flood_mode; 
    bcm_vlan_mcast_flood_t l2_mcast_flood_mode; 
#if defined (BCM_TRX_SUPPORT)
    _trx_vlan_block_t      block;
#endif /* BCM_TRX_SUPPORT */
    int                    qm_ptr;
    int                    trust_dot1p_ptr;
    uint32                 profile_flags; /* _BCM_VLAN_PROFILE_XXX */
    int                    ref_count;
#if defined (BCM_TRX_SUPPORT)
    bcm_vlan_protocol_packet_ctrl_t   protocol_pkt;
#endif /* BCM_TRX_SUPPORT */
#if defined (BCM_HURRICANE3_SUPPORT)
    uint32                 learn_flags;   /* VLAN-based Learn control flags */
#endif /* BCM_HURRICANE3_SUPPORT */
} _vlan_profile_t;

static _vlan_profile_t (*_vlan_profile[BCM_MAX_NUM_UNITS])[];

#define VLAN_PROFILE(_unit_) _vlan_profile[_unit_]

#define VLAN_PROFILE_ENTRY(_unit_, _idx_) (*VLAN_PROFILE(_unit_))[(_idx_)]
       
#define VLAN_PROFILE_REF_COUNT(_unit_, _idx_) \
            VLAN_PROFILE_ENTRY(_unit_, _idx_).ref_count 

#if defined(BCM_EASY_RELOAD_SUPPORT)
typedef struct _vlan_profile_er_s {
    uint8      profile_num;
} _vlan_profile_er_t;

static _vlan_profile_er_t (*_vlan_profile_er[BCM_MAX_NUM_UNITS])[];

#define VLAN_PROFILE_ER(_unit_) (_vlan_profile_er[_unit_])
#define VLAN_PROFILE_PTR_ER(unit, vlan) \
                         ((*_vlan_profile_er[unit])[vlan].profile_num)
#endif

/*
 * Function :
 *    _vlan_profile_idx_get
 *
 * Purpose  :
 *    Get programmed vlan profile index.
 *
 * Parameters :
 *     unit        - (IN) BCM device number.
 *     mem         - (IN) Memory name.  
 *     field       - (IN) VLAN profile index field.
 *     buf         - (IN) HW entry buffer. 
 *     vlan        - (IN) Vlan id profile attached.
 *     profile_idx - (OUT) Profile index.
 * 
 * Return :
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_idx_get(int unit, soc_mem_t mem, soc_field_t field,
                      uint32 *buf, bcm_vlan_t vlan, int *profile_index)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];
    uint32 *hw_buf_ptr;
    int    rv;

    /* Input parameters check. */
    if (NULL == profile_index) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_EASY_RELOAD_SUPPORT)
    if (SOC_IS_RELOADING(unit)) {
        /*
         * Use VLAN_PROFILE_ER() as a write-through cache 
         * for VLAN profile pointers during easy reload
         */
        *profile_index = VLAN_PROFILE_PTR_ER(unit, vlan);
    } else 
#endif
    {
        if (NULL == buf) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, vlan, hw_buf);
            BCM_IF_ERROR_RETURN(rv);
            hw_buf_ptr = hw_buf;
        } else {
            hw_buf_ptr = buf;
        }
        if (0 == soc_mem_field32_get(unit, mem, hw_buf_ptr, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }
        *profile_index = soc_mem_field32_get(unit, mem, hw_buf_ptr, field);
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _vlan_profile_idx_set
 *
 * Purpose  :
 *    Set vlan profile index in vlan table.
 *
 * Parameters :
 *     unit        - (IN) BCM device number.
 *     mem         - (IN) Memory name.  
 *     field       - (IN) VLAN profile index field.
 *     buf         - (IN) HW entry buffer. 
 *     vlan        - (IN) Vlan id profile attached.
 *     profile_idx - (IN) Profile index.
 * 
 * Return :
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_idx_set(int unit, soc_mem_t mem, soc_field_t field,
                      uint32 *buf, bcm_vlan_t vlan, int profile_index)
{
    /* Input parameters check. */
    if (NULL == buf) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_EASY_RELOAD_SUPPORT)
    if (SOC_IS_RELOADING(unit)) {
        /*
         * Use VLAN_PROFILE_ER() as a write-through cache 
         * for VLAN profile pointers during easy reload
         */
        VLAN_PROFILE_PTR_ER(unit, vlan) = (uint8) profile_index;
    } else 
#endif
    {
        soc_mem_field32_set(unit, mem, buf, field, profile_index);
    }
    return (BCM_E_NONE);
}

  /*
  * Function:
  *      _bcm_xgs3_protocol_pkt_ctrl_get
  * Purpose:
  *      function to get protocol packet action for given protocol
  *      control register table index.
  * Parameters:
  *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
  *      proto_index - (IN) index in PROTOCOL_PKT_CONTROLr and 
  *                                         IGMP_MLD_PKT_CONTROLr
  *      protocol_pkt- (OUT) protocol packet actions configured
  * Returns:
  *      BCM_E_NONE - protocol packet action get is successful.
  *      BCM_E_XXX  - Other error
  * Notes:
  *      None.
  */
int
_bcm_xgs3_protocol_pkt_ctrl_get(int unit, int proto_index,
                                bcm_vlan_protocol_packet_ctrl_t *protocol_pkt)
{
#if defined (BCM_TRX_SUPPORT)
    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
        soc_reg_t  reg;
        uint64     regval;
        uint32     value = 0;
        int        pkt_action = 0;
    
        COMPILER_64_ZERO(regval);
    
        reg = PROTOCOL_PKT_CONTROLr;
        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, proto_index, &regval));
    
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REPLY_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval, ARP_REPLY_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REPLY_DROPf)) {
            value = soc_reg64_field32_get(unit, reg, regval, ARP_REPLY_DROPf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        }
        protocol_pkt->arp_reply_action = pkt_action;
    
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REQUEST_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          ARP_REQUEST_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REQUEST_DROPf)) {
            value = soc_reg64_field32_get(unit, reg, regval, ARP_REQUEST_DROPf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        }
        protocol_pkt->arp_request_action = pkt_action;
        
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, ND_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval, ND_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, ND_PKT_DROPf)) {
            value = soc_reg64_field32_get(unit, reg, regval, ND_PKT_DROPf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        }
        protocol_pkt->nd_action = pkt_action;
    
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, DHCP_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval, DHCP_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, DHCP_PKT_DROPf)) {
            value = soc_reg64_field32_get(unit, reg, regval, DHCP_PKT_DROPf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        }
        protocol_pkt->dhcp_action = pkt_action;
    
        pkt_action = 0;
        if (SOC_REG_FIELD_VALID(unit, reg, MMRP_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval, MMRP_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, MMRP_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval, MMRP_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->mmrp_action = pkt_action;
        
        pkt_action = 0;
        if (SOC_REG_FIELD_VALID(unit, reg, SRP_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval, SRP_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, SRP_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval, SRP_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->srp_action = pkt_action;
    
        reg = IGMP_MLD_PKT_CONTROLr;
        BCM_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, proto_index, 
                                        &regval));

        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_REP_LEAVE_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IGMP_REP_LEAVE_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_REP_LEAVE_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IGMP_REP_LEAVE_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->igmp_report_leave_action = pkt_action;
        
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_QUERY_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IGMP_QUERY_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_QUERY_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IGMP_QUERY_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->igmp_query_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_UNKNOWN_MSG_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IGMP_UNKNOWN_MSG_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_UNKNOWN_MSG_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IGMP_UNKNOWN_MSG_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->igmp_unknown_msg_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_REP_DONE_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          MLD_REP_DONE_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_REP_DONE_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          MLD_REP_DONE_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->mld_report_done_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_QUERY_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval, MLD_QUERY_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_QUERY_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          MLD_QUERY_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->mld_query_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IPV4_RESVD_MC_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV4_RESVD_MC_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IPV4_RESVD_MC_PKT_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV4_RESVD_MC_PKT_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->ip4_rsvd_mc_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IPV6_RESVD_MC_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV6_RESVD_MC_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IPV6_RESVD_MC_PKT_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV6_RESVD_MC_PKT_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->ip6_rsvd_mc_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IPV4_MC_ROUTER_ADV_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV4_MC_ROUTER_ADV_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg,IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->ip4_mc_router_adv_action = pkt_action;
        pkt_action = 0; 
        if (SOC_REG_FIELD_VALID(unit, reg, IPV6_MC_ROUTER_ADV_PKT_TO_CPUf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV6_MC_ROUTER_ADV_PKT_TO_CPUf);
            pkt_action |= value ? BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        }
        if (SOC_REG_FIELD_VALID(unit, reg,IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                          IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf);
            pkt_action |= (value == 2) ? BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
                          (value == 1) ? BCM_VLAN_PROTO_PKT_DROP_ENABLE :
                                         BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        }
        protocol_pkt->ip6_mc_router_adv_action = pkt_action;
        
        value = 0;
        if (SOC_REG_FIELD_VALID(unit, reg, PFM_RULE_APPLYf)) {
            value = soc_reg64_field32_get(unit, reg, regval,
                                            PFM_RULE_APPLYf);
        }
        protocol_pkt->flood_action_according_to_pfm = value ? TRUE : FALSE;

        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

 /*
 * Function:
 *      _bcm_xgs3_protocol_pkt_ctrl_set
 * Purpose:
 *      function to set protocol packet action for given protocol
 *      control register table index.
 *      
 * Parameters:
 *      unit - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan_profile_index - (IN) index in vlan_profile table
 *      protocol_pkt- protocol packet actions to be configured
 *      protocol_pkt_index (OUT) - the index in PROTOCOL_PKT_CONTROLr
 *                    and IGPM_PLD_PKT_CONTROLr
 * Returns:
 *      BCM_E_NONE - protocol packet action configuration is successful.
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int
_bcm_xgs3_protocol_pkt_ctrl_set(int unit, int old_protocol_pkt_index,
                                bcm_vlan_protocol_packet_ctrl_t *protocol_pkt,
                                int *protocol_pkt_index)
{
#if defined (BCM_TRX_SUPPORT)
    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
        uint32      value;
        uint32 prot_pkt_ctrl = 0, igmp_mld_pkt_ctrl = 0;
        int pkt_action;
    
        uint32 index;
        soc_reg_t reg;
        int ref_count = 0;
    
        if (NULL == protocol_pkt) {
            return BCM_E_PARAM;
        }
   
        reg = PROTOCOL_PKT_CONTROLr;
        pkt_action = protocol_pkt->arp_reply_action;
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REPLY_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, ARP_REPLY_TO_CPUf, 
                              value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REPLY_DROPf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, ARP_REPLY_DROPf,
                              value);
        }
        pkt_action = protocol_pkt->arp_request_action;
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REQUEST_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl,
                    ARP_REQUEST_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, ARP_REQUEST_DROPf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, ARP_REQUEST_DROPf, 
                              value);
        }
        pkt_action = protocol_pkt->nd_action;
        if (SOC_REG_FIELD_VALID(unit, reg, ND_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, ND_PKT_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, ND_PKT_DROPf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, ND_PKT_DROPf, value);
        }
        pkt_action = protocol_pkt->dhcp_action;
        if (SOC_REG_FIELD_VALID(unit, reg, DHCP_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, DHCP_PKT_TO_CPUf, 
                              value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, DHCP_PKT_DROPf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, DHCP_PKT_DROPf, value);
        }
        pkt_action = protocol_pkt->mmrp_action;
        if (SOC_REG_FIELD_VALID(unit, reg, MMRP_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, MMRP_PKT_TO_CPUf, 
                              value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, MMRP_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, MMRP_FWD_ACTIONf, 
                              value);
        }
        pkt_action = protocol_pkt->srp_action;
        if (SOC_REG_FIELD_VALID(unit, reg, SRP_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, SRP_PKT_TO_CPUf, 
                              value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, SRP_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &prot_pkt_ctrl, SRP_FWD_ACTIONf, 
                              value);
        }
    
        reg = IGMP_MLD_PKT_CONTROLr;
        pkt_action = protocol_pkt->igmp_report_leave_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_REP_LEAVE_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IGMP_REP_LEAVE_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_REP_LEAVE_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IGMP_REP_LEAVE_FWD_ACTIONf, value);
        }
        
        pkt_action = protocol_pkt->igmp_query_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_QUERY_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl, 
                    IGMP_QUERY_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_QUERY_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IGMP_QUERY_FWD_ACTIONf, value);
        }
        
        pkt_action = protocol_pkt->igmp_unknown_msg_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_UNKNOWN_MSG_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IGMP_UNKNOWN_MSG_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IGMP_UNKNOWN_MSG_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IGMP_UNKNOWN_MSG_FWD_ACTIONf, value);
        }
    
        pkt_action = protocol_pkt->mld_report_done_action;
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_REP_DONE_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    MLD_REP_DONE_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_REP_DONE_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    MLD_REP_DONE_FWD_ACTIONf, value);
        }
        
        pkt_action = protocol_pkt->mld_query_action;
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_QUERY_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    MLD_QUERY_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, MLD_QUERY_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    MLD_QUERY_FWD_ACTIONf, value);
        }
    
        pkt_action = protocol_pkt->ip4_rsvd_mc_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IPV4_RESVD_MC_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV4_RESVD_MC_PKT_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IPV4_RESVD_MC_PKT_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV4_RESVD_MC_PKT_FWD_ACTIONf, value);
        }
    
        pkt_action = protocol_pkt->ip6_rsvd_mc_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IPV6_RESVD_MC_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV6_RESVD_MC_PKT_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, IPV6_RESVD_MC_PKT_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV6_RESVD_MC_PKT_FWD_ACTIONf, value);
        }
    
        pkt_action = protocol_pkt->ip4_mc_router_adv_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IPV4_MC_ROUTER_ADV_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV4_MC_ROUTER_ADV_PKT_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, 
                                IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf, value);
        }
    
        pkt_action = protocol_pkt->ip6_mc_router_adv_action;
        if (SOC_REG_FIELD_VALID(unit, reg, IPV6_MC_ROUTER_ADV_PKT_TO_CPUf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV6_MC_ROUTER_ADV_PKT_TO_CPUf, value);
        }
        if (SOC_REG_FIELD_VALID(unit, reg, 
                                IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf)) {
            value = pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
                   (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                    IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf, value);
        }

        if (SOC_REG_FIELD_VALID(unit, reg, PFM_RULE_APPLYf)) {
            value = protocol_pkt->flood_action_according_to_pfm ? 1 : 0;
            soc_reg_field_set(unit, reg, &igmp_mld_pkt_ctrl,
                        PFM_RULE_APPLYf, value);
        }
        BCM_IF_ERROR_RETURN(
            _bcm_prot_pkt_ctrl_add(unit, prot_pkt_ctrl, igmp_mld_pkt_ctrl, 
                                   &index));
        if (old_protocol_pkt_index > 0) {
            BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_ref_count_get(
                                   unit, old_protocol_pkt_index, &ref_count));
            if (ref_count > 0) {
                BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_delete(unit, 
                                     old_protocol_pkt_index));
            }
        }
        *protocol_pkt_index = index;
        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    _vlan_profile_read
 * 
 * Purpose:
 *    Read a vlan profile entry from vlan profile table.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     index - (IN) Entry index.
 *     entry - (IN) Vlan profile entry. 
 *
 * Return:
 *    BCM_E_XXX
 *
 * Notes:
 *      Only used as part of reload.
 */
STATIC int 
_vlan_profile_read(int unit, int index, _vlan_profile_t *entry)
{
    uint32              buf[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.     */
    uint32              value;                        /* Field specific value.*/
    soc_mem_t           mem;          /* Vlan profile table memory.           */
#if defined (BCM_TRX_SUPPORT)
    _trx_vlan_block_t   *block;       /* Vlan block profile pointer.          */
    uint32 proto_index;
#endif /* BCM_TRX_SUPPORT */
    int rv = BCM_E_NONE;

    /* Input parameters check. */
    if ((NULL == entry) || (index < 0) ||
        (index > soc_mem_index_max(unit, VLAN_PROFILE_TABm))) {
        return (BCM_E_PARAM) ;
    }

    /* Reset output buffer. */
    sal_memset(entry, 0, sizeof(_vlan_profile_t));

    /* Reset hw buffer. */
    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    mem = VLAN_PROFILE_TABm;

    /* READ vlan profile table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, buf);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Enable/Disable MIM termination on a vlan. */
    if (soc_mem_field_valid(unit, mem, MIM_TERM_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, MIM_TERM_ENABLEf)) {
            entry->flags |=  BCM_VLAN_MIM_TERM_DISABLE;
        }
    }

    /* Enable/Disable MPLS forwarding on a vlan. */
    if (soc_mem_field_valid(unit, mem, MPLS_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, MPLS_ENABLEf)) {
            entry->flags |=  BCM_VLAN_MPLS_DISABLE;
        }
    }

    /* Drop non-unicast, broadcast or multicast packets that miss L2 lookup. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, L2_NON_UCAST_DROPf)) {
        if (soc_mem_field32_get(unit, mem, buf, L2_NON_UCAST_DROPf)) {
            entry->flags |=  BCM_VLAN_NON_UCAST_DROP;
        }
    }

    /* Copy to cpu non-unicast, broadcast or multicast packets 
     * that miss L2 lookup. 
     */
    if (SOC_MEM_FIELD_VALID(unit, mem, BCM_VLAN_NON_UCAST_TOCPU)) {
        if (soc_mem_field32_get(unit, mem, buf, L2_NON_UCAST_TOCPUf)) {
            entry->flags |=  BCM_VLAN_NON_UCAST_TOCPU;
        }
    }

    /* Drop packets that miss L2 lookup. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, L2_MISS_DROPf)) {
        if (soc_mem_field32_get(unit, mem, buf, L2_MISS_DROPf)) {
            entry->flags |=  BCM_VLAN_UNKNOWN_UCAST_DROP;
        }
    }

    /* Copy to cpu packets that miss L2 lookup. */
    if (SOC_REG_FIELD_VALID(unit, VLAN_PROFILE_TABr, L2_MISS_TOCPUf)) {
        uint32 rval;
        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, VLAN_PROFILE_TABr, REG_PORT_ANY, 0, &rval));
        if (soc_reg_field_get(unit, VLAN_PROFILE_TABr, rval, L2_MISS_TOCPUf)) {
            entry->flags |=  BCM_VLAN_UNKNOWN_UCAST_TOCPU;
        }
    } else if (SOC_MEM_FIELD_VALID(unit, mem, L2_MISS_TOCPUf)) {
        if (soc_mem_field32_get(unit, mem, buf, L2_MISS_TOCPUf)) {
            entry->flags |=  BCM_VLAN_UNKNOWN_UCAST_TOCPU;
        }
    }

    /* L3 Enable for IPv4 packets on the VLAN. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPV4L3_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, IPV4L3_ENABLEf)) {
            entry->flags |=  BCM_VLAN_IP4_DISABLE;
        }
    }

    /* L3 Enable for IPv6 packets on the VLAN. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPV6L3_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, IPV6L3_ENABLEf)) {
            entry->flags |=  BCM_VLAN_IP6_DISABLE;
        }
    }

    /* Enable L2-only forwarding of IPMCv4 packets on this VLAN.  */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV4_L2_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, IPMCV4_L2_ENABLEf)) {
            entry->flags |=  BCM_VLAN_IP4_MCAST_L2_DISABLE;
        }
    }

    /* Enable L2-only forwarding of IPMCv6 packets on this VLAN. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV6_L2_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, IPMCV6_L2_ENABLEf)) {
            entry->flags |=  BCM_VLAN_IP6_MCAST_L2_DISABLE;
        }
    }

    /* IPMC Enable IPv4 packets on this VLAN.  */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV4_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, IPMCV4_ENABLEf)) {
            entry->flags |= BCM_VLAN_IP4_MCAST_DISABLE;
        }
    }

    /* IPMC Enable IPv6 packets on this VLAN.  */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV6_ENABLEf)) {
        if (0 == soc_mem_field32_get(unit, mem, buf, IPMCV6_ENABLEf)) {
            entry->flags |= BCM_VLAN_IP6_MCAST_DISABLE;
        }
    }

    if (soc_feature(unit, soc_feature_ipmc_unicast)) {
        if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV4_UNICAST_MACDA_ENABLEf)) {
            if (0 != soc_mem_field32_get(unit, mem, buf, 
                    IPMCV4_UNICAST_MACDA_ENABLEf)) {
                entry->flags |= BCM_VLAN_IPMC4_UCAST_ENABLE;
            }
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV6_UNICAST_MACDA_ENABLEf)) {
            if (0 != soc_mem_field32_get(unit, mem, buf, 
                    IPMCV6_UNICAST_MACDA_ENABLEf)) {
                entry->flags |= BCM_VLAN_IPMC6_UCAST_ENABLE;
            }
        }
    }

    /* Enables copy to cpu when ICMP redirect packet is needed. */
    if (SOC_MEM_FIELD_VALID(unit, mem, ICMP_REDIRECT_TOCPUf)) {
        if (soc_mem_field32_get(unit, mem, buf, ICMP_REDIRECT_TOCPUf)) {
            entry->flags |= BCM_VLAN_ICMP_REDIRECT_TOCPU;
        }
    }

    /* Unknown IPMC (IPMC miss) for IPv6 packets sent to CPU enable. */
    if (SOC_MEM_FIELD_VALID(unit, mem, UNKNOWN_IPV6_MC_TOCPUf)) {
        if (soc_mem_field32_get(unit, mem, buf, UNKNOWN_IPV6_MC_TOCPUf)) {
            entry->flags |= BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU;
        }
    }

    /* Unknown IPMC (IPMC miss) for IPv4 packets sent to CPU enable. */
    if (SOC_MEM_FIELD_VALID(unit, mem, UNKNOWN_IPV4_MC_TOCPUf)) {
        if (soc_mem_field32_get(unit, mem, buf, UNKNOWN_IPV4_MC_TOCPUf)) {
            entry->flags |= BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU;
        }
    }

    /* Outer tpid index Used when PORT_TABLE.OUTER_TPID_VERIFY, 
     * is enabled and indicates which of the ING_OUTER_TPID[xx]
     * register is expected to match for this VLAN
     */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, OUTER_TPID_INDEX_MASKf)) {
        int i, field_len;
        value = soc_mem_field32_get(unit, mem, buf, OUTER_TPID_INDEX_MASKf);
        field_len = soc_mem_field_length(unit, mem, OUTER_TPID_INDEX_MASKf);
        for (i = 0; i < field_len; i++) {
            if (1 << i == value) {
                value = i;
                break;
            }
        }
        if (i == field_len) {
            value = 0;
        }
    } else {
    value = soc_mem_field32_get(unit, mem, buf, OUTER_TPID_INDEXf);
    }
    entry->outer_tpid= OUTER_TPID_ENTRY(unit, value); 

    /* Disable learning on this vlan. */
    if (soc_mem_field32_get(unit, mem, buf, LEARN_DISABLEf)) {
        entry->flags |= BCM_VLAN_LEARN_DISABLE;
    }

    /* PFM value for IPv6 multicast lookups */
    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IPV6_PFMf)) {
        value = soc_mem_field32_get(unit, mem, buf, L3_IPV6_PFMf);
        rv = _bcm_vlan_mcast_pfm_to_flood_mode(unit, (int)value, 
                                           &entry->ip6_mcast_flood_mode);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* PFM value for IPv4 multicast lookups */
    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IPV4_PFMf)) {
        value = soc_mem_field32_get(unit, mem, buf, L3_IPV4_PFMf);
        rv =  _bcm_vlan_mcast_pfm_to_flood_mode(unit, (int)value, 
                                            &entry->ip4_mcast_flood_mode);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* PFM value for L2 multicast lookups */
    value = soc_mem_field32_get(unit, mem, buf, L2_PFMf);
    rv = _bcm_vlan_mcast_pfm_to_flood_mode(unit, (int)value, 
                                           &entry->l2_mcast_flood_mode);
    BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_HURRICANE3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, USE_VLAN_CMLf)) {
        if (0 != soc_mem_field32_get(unit, mem, buf, USE_VLAN_CMLf)) {
            entry->learn_flags = BCM_VLAN_LEARN_CONTROL_ENABLE;
            value = soc_mem_field32_get(unit, mem, buf, CML_FLAGS_NEWf);
            if (!(value & (1 << 0))) {
               entry->learn_flags |= BCM_VLAN_LEARN_CONTROL_FWD;
            }
            if (value & (1 << 1)) {
               entry->learn_flags |= BCM_VLAN_LEARN_CONTROL_CPU;
            }
            if (value & (1 << 2)) {
               entry->learn_flags |= BCM_VLAN_LEARN_CONTROL_PENDING;
            }
            if (value & (1 << 3)) {
               entry->learn_flags |= BCM_VLAN_LEARN_CONTROL_ARL;
            }
        } else {
            entry->learn_flags = 0x0;
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined (BCM_TRIUMPH2_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, TRUST_DOT1Pf)) {
        if (soc_mem_field32_get(unit, mem, buf, TRUST_DOT1Pf)) {
            entry->profile_flags |= _BCM_VLAN_PROFILE_TRUST_DOT1P;
        }
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, TRUST_DOT1P_PTRf)) {
        entry->trust_dot1p_ptr =
            soc_mem_field32_get(unit, mem, buf, TRUST_DOT1P_PTRf);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined (BCM_TRX_SUPPORT)
    if (SOC_MEM_FIELD_VALID (unit,mem, PROTOCOL_PKT_INDEXf)) {
        proto_index = soc_mem_field32_get (unit, mem, buf, PROTOCOL_PKT_INDEXf);
        rv = _bcm_xgs3_protocol_pkt_ctrl_get(unit,
                                         proto_index, &entry->protocol_pkt);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return (rv);
        }
    }

    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m)) {
        /* READ VLAN_PROFILE_2 portion. */
        sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        mem = VLAN_PROFILE_2m;
        block = &entry->block;

        /* READ vlan profile 2 table. */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, buf);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        /* Bitmap A  */
        soc_mem_pbmp_field_get(unit, mem, buf, BLOCK_MASK_Af,
                               &block->first_mask);

        /* Bitmap B  */
        soc_mem_pbmp_field_get(unit, mem, buf, BLOCK_MASK_Bf,
                               &block->second_mask);

        /* Broadcast mask select. */
        block->broadcast_mask_sel = soc_mem_field32_get(unit, mem, buf, BCAST_MASK_SELf);

        /* Unknown unicast mask select. */
        block->unknown_unicast_mask_sel = 
            soc_mem_field32_get(unit, mem, buf, UNKNOWN_UCAST_MASK_SELf);

        /* Unknown multicast mask select. */
        block->unknown_mulitcast_mask_sel = 
            soc_mem_field32_get(unit, mem, buf, UNKNOWN_MCAST_MASK_SELf);

        /* Unknown multicast mask select. */
        block->known_mulitcast_mask_sel = 
            soc_mem_field32_get(unit, mem, buf, KNOWN_MCAST_MASK_SELf);
    }
#endif /* BCM_TRX_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function : _vlan_profile_detach
 *
 * Purpose  : to de-initialize hardware VLAN_PROFILE_TAB and free memory
 *            used to cache hardware table in RAM.
 */
int
_bcm_xgs3_vlan_profile_detach(int unit)
{
    sal_free(VLAN_PROFILE(unit));
    VLAN_PROFILE(unit) = NULL; 

    return BCM_E_NONE;
}

/*
 * Function : _vlan_profile_init
 *
 * Purpose  : to initialize hardware VLAN_PROFILE_TAB and allocate memory
 *            to cache hardware table in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile table and initialize it.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
STATIC int
_vlan_profile_init(int unit)
{
    int alloc_size;   /* Memory allocation size.  */
    int rv;           /* Operation return status. */

    if ((0 == SOC_WARM_BOOT(unit)) && (0 == SOC_IS_RCPU_ONLY(unit))) {
        /* Clear the hardware table */
        rv = soc_mem_clear(unit, VLAN_PROFILE_TABm, MEM_BLOCK_ALL, TRUE);
        BCM_IF_ERROR_RETURN(rv);
    }

    alloc_size = sizeof(_vlan_profile_t) * 
        soc_mem_index_count(unit, VLAN_PROFILE_TABm);
    if (VLAN_PROFILE(unit) == NULL) {
        /* Allocate memory to cache VLAN Profile table */
        VLAN_PROFILE(unit) = sal_alloc(alloc_size, "Cached VLAN Profile Table");
        if (NULL == VLAN_PROFILE(unit)) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(VLAN_PROFILE(unit), 0, alloc_size);

    if (SOC_WARM_BOOT(unit)) {
        int index;
        int index_max = soc_mem_index_max(unit, VLAN_PROFILE_TABm);
        for (index = 0; index <= index_max; index++) {
            rv = _vlan_profile_read(unit, index,
                                        &VLAN_PROFILE_ENTRY(unit, index));
            BCM_IF_ERROR_RETURN(rv);
        }
    }

#if defined(BCM_EASY_RELOAD_SUPPORT)
    if (SOC_IS_RELOADING(unit)) {
         if (NULL == VLAN_PROFILE_ER(unit)) {
             alloc_size = sizeof(_vlan_profile_er_t) * BCM_VLAN_COUNT;
 
             VLAN_PROFILE_ER(unit) =
                 sal_alloc(alloc_size, "Cached VLAN Profile Pointers");
 
             if (NULL == VLAN_PROFILE_ER(unit)) {
                 sal_free(VLAN_PROFILE(unit));
                 return BCM_E_MEMORY;
             }
         }
         sal_memset(VLAN_PROFILE_ER(unit), 0, alloc_size);
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function :
 *    _vlan_profile_compare
 *
 * Purpose  :
 *    Compare two vlan profiles.
 *
 * Parameters :
 *     unit   - (IN) BCM device number.
 *     first  - (IN) First vlan profile entry. 
 *     second - (IN) Second vlan profile entry. 
 *     equal  - (OUT) Profile is identical flag.
 * 
 * Return :
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_compare(int unit, _vlan_profile_t *first, 
                          _vlan_profile_t *second,  uint8 *equal) 
{
    /* Input parameters check. */
    if ((NULL == first) || (NULL == second) || (NULL == equal))  {
        return (BCM_E_PARAM);
    }

#if defined (BCM_TRX_SUPPORT)
    if (0 != sal_memcmp(&first->block, &second->block, 
                        sizeof (_trx_vlan_block_t))) {
        *equal = FALSE;
    } else  
#endif /* BCM_TRX_SUPPORT */

    if ((first->outer_tpid != second->outer_tpid) ||
        (first->flags != second->flags) ||
        (first->ip6_mcast_flood_mode != second->ip6_mcast_flood_mode) ||
        (first->ip4_mcast_flood_mode != second->ip4_mcast_flood_mode) ||
        (first->l2_mcast_flood_mode != second->l2_mcast_flood_mode) ||
        (first->qm_ptr != second->qm_ptr) ||
        (first->trust_dot1p_ptr != second->trust_dot1p_ptr) ||
        (first->profile_flags != second->profile_flags)) {
        *equal = FALSE;
    } else  {
        *equal = TRUE;
    }

#if defined (BCM_TRX_SUPPORT)
    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)
        && (*equal == TRUE)) {
        if (0 != sal_memcmp(&first->protocol_pkt, &second->protocol_pkt, 
                        sizeof (bcm_vlan_protocol_packet_ctrl_t))) {
            *equal = FALSE;
        }
    }
#endif /* BCM_TRX_SUPPORT */

#if defined (BCM_HURRICANE3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, USE_VLAN_CMLf)
        && (*equal == TRUE)) {
        if (0 != sal_memcmp(&first->learn_flags, &second->learn_flags, 
                        sizeof (uint32))) {
            *equal = FALSE;
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    return (BCM_E_NONE);
}

/*
 * Function :
 *    _vlan_profile_entry_init
 *
 * Purpose  :
 *    Initialize sw vlan profile entry.
 *
 * Parameters :
 *     unit   - (IN) BCM device number.
 *     index  - (IN) Inserted entry index.
 *     entry  - (IN) Inserted entry data.
 * 
 * Return :
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_entry_init(int unit, int index, _vlan_profile_t *entry)
{
    _vlan_profile_t *vte;

    /* Input parameters check. */
    if (NULL == entry) {
        return (BCM_E_PARAM); 
    }
    if ((index < 0) || 
        (index > soc_mem_index_max(unit, VLAN_PROFILE_TABm))) {
        return (BCM_E_PARAM); 
    }

    vte = &VLAN_PROFILE_ENTRY(unit, index);
    /* Vlan control vlan portion. */
    vte->outer_tpid = entry->outer_tpid;
    vte->flags = entry->flags;
    vte->profile_flags = entry->profile_flags;
    vte->ip6_mcast_flood_mode = entry->ip6_mcast_flood_mode;
    vte->ip4_mcast_flood_mode = entry->ip4_mcast_flood_mode;
    vte->l2_mcast_flood_mode  = entry->l2_mcast_flood_mode;
    vte->qm_ptr               = entry->qm_ptr; 
    vte->trust_dot1p_ptr      = entry->trust_dot1p_ptr; 
#if defined (BCM_TRX_SUPPORT)
    /* Vlan block portion. */
    sal_memcpy(&VLAN_PROFILE_ENTRY(unit, index).block, 
               &entry->block, sizeof (_trx_vlan_block_t));

    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
        sal_memcpy(&VLAN_PROFILE_ENTRY(unit, index).protocol_pkt,
                &entry->protocol_pkt, sizeof (bcm_vlan_protocol_packet_ctrl_t));
    }
#endif /* BCM_TRX_SUPPORT */

#if defined (BCM_HURRICANE3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, USE_VLAN_CMLf)) {
        sal_memcpy(&VLAN_PROFILE_ENTRY(unit, index).learn_flags,
                &entry->learn_flags, sizeof (uint32));
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    VLAN_PROFILE_REF_COUNT(unit, index) = 1;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _vlan_profile_write
 * 
 * Purpose:
 *    Write a vlan profile entry to vlan profile table.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     index - (IN) Entry index.
 *     entry - (IN) Vlan profile entry. 
 *
 * Return:
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_write(int unit, int index, _vlan_profile_t *entry)
{
    uint32              buf[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.     */
    uint32              value;                        /* Field specific value.*/
    soc_mem_t           mem;          /* Vlan profile table memory.           */
    int                 tpid_index;   /* Outer tpid index.                    */
    int                 rv;           /* Operation return status.             */
#if defined (BCM_TRX_SUPPORT)
    _trx_vlan_block_t   *block;       /* Vlan block profile pointer.          */
    int protocol_pkt_index;
#endif /* BCM_TRX_SUPPORT */

    /* Input parameters check. */
    if ((NULL == entry) || (index < 0) ||
        (index > soc_mem_index_max(unit, VLAN_PROFILE_TABm))) {
        return (BCM_E_PARAM);
    }


    /* Reset hw buffer. */
    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    mem = VLAN_PROFILE_TABm;

    /* Enable/Disable MIM termination on a vlan. */
    if (soc_mem_field_valid(unit, mem, MIM_TERM_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_MIM_TERM_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, MIM_TERM_ENABLEf, value);
    }

    /* Enable/Disable MPLS forwarding on a vlan. */
    if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_MPLS_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, MPLS_ENABLEf, value);
    } 

    if (SOC_MEM_FIELD_VALID(unit, mem, PHB2_ENABLEf)) {
        value = entry->profile_flags & _BCM_VLAN_PROFILE_PHB2_ENABLE ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, PHB2_ENABLEf, value);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PHB2_USE_INNER_DOT1Pf)) {
        value = entry->profile_flags & _BCM_VLAN_PROFILE_PHB2_USE_INNER_TAG ?
            1 : 0;
        soc_mem_field32_set(unit, mem, buf, PHB2_USE_INNER_DOT1Pf, value);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PHB2_ENABLEf)) {
        value = entry->profile_flags & _BCM_VLAN_PROFILE_PHB2_ENABLE ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, PHB2_ENABLEf, value);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, PHB2_DOT1P_MAPPING_PTRf)) {
        soc_mem_field32_set(unit, mem, buf, PHB2_DOT1P_MAPPING_PTRf,
                            entry->qm_ptr);
    }

    /* Drop non-unicast, broadcast or multicast packets that miss L2 lookup. */
    if (SOC_MEM_FIELD_VALID(unit, mem, L2_NON_UCAST_DROPf)) {
        value = (entry->flags & BCM_VLAN_NON_UCAST_DROP) ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, L2_NON_UCAST_DROPf, value);
    }

    /* Copy to cpu non-unicast, broadcast or multicast packets 
     * that miss L2 lookup. 
     */
    if (SOC_MEM_FIELD_VALID(unit, mem, L2_NON_UCAST_TOCPUf)) {
        value = (entry->flags & BCM_VLAN_NON_UCAST_TOCPU) ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, L2_NON_UCAST_TOCPUf, value);
    }

    /* Drop packets that miss L2 lookup. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, L2_MISS_DROPf)) {
        value = (entry->flags & BCM_VLAN_UNKNOWN_UCAST_DROP) ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, L2_MISS_DROPf, value);
    }

    /* Copy to cpu packets that miss L2 lookup. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, L2_MISS_TOCPUf)) {
        value = (entry->flags & BCM_VLAN_UNKNOWN_UCAST_TOCPU) ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, L2_MISS_TOCPUf, value);
    }

    /* L3 Enable for IPv4 packets on the VLAN. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPV4L3_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_IP4_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, IPV4L3_ENABLEf, value);
    }

    /* L3 Enable for IPv6 packets on the VLAN. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPV6L3_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_IP6_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, IPV6L3_ENABLEf, value);
    }

    /* Enable L2-only forwarding of IPMCv4 packets on this VLAN.  */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV4_L2_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_IP4_MCAST_L2_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, IPMCV4_L2_ENABLEf, value);
    }

    /* Enable L2-only forwarding of IPMCv6 packets on this VLAN. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV6_L2_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_IP6_MCAST_L2_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, IPMCV6_L2_ENABLEf, value);
    }


    /* IPMC Enable IPv4 packets on this VLAN.  */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV4_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_IP4_MCAST_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, IPMCV4_ENABLEf, value);
    }

    /* IPMC Enable IPv6 packets on this VLAN.  */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV6_ENABLEf)) {
        value = (entry->flags & BCM_VLAN_IP6_MCAST_DISABLE) ? 0 : 1;
        soc_mem_field32_set(unit, mem, buf, IPMCV6_ENABLEf, value);
    }

    if (soc_feature(unit, soc_feature_ipmc_unicast)) {
        if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV4_UNICAST_MACDA_ENABLEf)) {
            value = (entry->flags & BCM_VLAN_IPMC4_UCAST_ENABLE) ? 1 : 0;
            soc_mem_field32_set(unit, mem, buf, 
                    IPMCV4_UNICAST_MACDA_ENABLEf, value);
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, IPMCV6_UNICAST_MACDA_ENABLEf)) {
            value = (entry->flags & BCM_VLAN_IPMC6_UCAST_ENABLE) ? 1 : 0;
            soc_mem_field32_set(unit, mem, buf, 
                    IPMCV6_UNICAST_MACDA_ENABLEf, value);
        }
    }

    /* Enable copy to cpu when ICMP redirect packet is needed. */ 
    if (SOC_MEM_FIELD_VALID(unit, mem, ICMP_REDIRECT_TOCPUf)) {
        value = (entry->flags & BCM_VLAN_ICMP_REDIRECT_TOCPU) ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, ICMP_REDIRECT_TOCPUf, value);
    }

    /* Unknown IPMC (IPMC miss) for IPv6 packets sent to CPU enable. */
    if (SOC_MEM_FIELD_VALID(unit, mem, UNKNOWN_IPV6_MC_TOCPUf)) {
        value = (entry->flags & BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU)? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, UNKNOWN_IPV6_MC_TOCPUf, value);
    }

    /* Unknown IPMC (IPMC miss) for IPv4 packets sent to CPU enable. */
    if (SOC_MEM_FIELD_VALID(unit, mem, UNKNOWN_IPV4_MC_TOCPUf)) {
        value = (entry->flags & BCM_VLAN_UNKNOWN_IP4_MCAST_TOCPU)? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, UNKNOWN_IPV4_MC_TOCPUf, value);
    }

    /* Outer tpid index Used when PORT_TABLE.OUTER_TPID_VERIFY, 
     * is enabled and indicates which of the ING_OUTER_TPID[xx]
     * register is expected to match for this VLAN
     */ 
    
    if(!SOC_IS_SHADOW(unit)) {
        rv = _bcm_fb2_outer_tpid_lkup(unit, entry->outer_tpid, &tpid_index);
        BCM_IF_ERROR_RETURN(rv);
        value = (uint32) tpid_index;
        if (SOC_MEM_FIELD_VALID(unit, mem, OUTER_TPID_INDEX_MASKf)) {
            soc_mem_field32_set(unit, mem, buf, OUTER_TPID_INDEX_MASKf,
                                1 << value);
        } else {
        soc_mem_field32_set(unit, mem, buf, OUTER_TPID_INDEXf, value);
    }
    }

    /* Disable learning on this vlan. */
    if (SOC_MEM_FIELD_VALID(unit, mem, LEARN_DISABLEf)) {
        value = (entry->flags & BCM_VLAN_LEARN_DISABLE) ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, LEARN_DISABLEf, value);
    }

    /* PFM value for IPv6 multicast lookups */
    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IPV6_PFMf)) {
        rv = _bcm_vlan_mcast_flood_mode_to_pfm(unit, entry->ip6_mcast_flood_mode, 
                                               (int *)&value);
        BCM_IF_ERROR_RETURN(rv);
        soc_mem_field32_set(unit, mem, buf, L3_IPV6_PFMf, value);
    }

    /* PFM value for IPv4 multicast lookups */
    if (SOC_MEM_FIELD_VALID(unit, mem, L3_IPV4_PFMf)) {
        rv = _bcm_vlan_mcast_flood_mode_to_pfm(unit, entry->ip4_mcast_flood_mode,
                                               (int *)&value);
        BCM_IF_ERROR_RETURN(rv);
        soc_mem_field32_set(unit, mem, buf, L3_IPV4_PFMf, value);
    }

    /* PFM value for L2 multicast lookups */
    rv = _bcm_vlan_mcast_flood_mode_to_pfm(unit, entry->l2_mcast_flood_mode,
                                           (int *)&value);
    BCM_IF_ERROR_RETURN(rv);
    soc_mem_field32_set(unit, mem, buf, L2_PFMf, value);

#if defined (BCM_TRX_SUPPORT)
    /*Get the protocol_pkt_control register index  and program
      in PROTOCOL_PKT_INDEX field*/
    if (SOC_MEM_FIELD_VALID (unit,mem, PROTOCOL_PKT_INDEXf)) {
        rv = _bcm_xgs3_protocol_pkt_ctrl_set(unit, -1,
                &entry->protocol_pkt, &protocol_pkt_index);
        if (BCM_SUCCESS(rv)) {
            value = (uint32)protocol_pkt_index;
            soc_mem_field32_set(unit, mem, buf, PROTOCOL_PKT_INDEXf, value);
        } else if (rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }
#endif /* BCM_TRX_SUPPORT */

#if defined (BCM_TRIUMPH2_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, TRUST_DOT1Pf)) {
        value = entry->profile_flags & _BCM_VLAN_PROFILE_TRUST_DOT1P ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, TRUST_DOT1Pf, value);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, TRUST_DOT1P_PTRf)) {
        soc_mem_field32_set(unit, mem, buf, TRUST_DOT1P_PTRf, 
                                            entry->trust_dot1p_ptr);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined (BCM_HURRICANE3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, USE_VLAN_CMLf)) {
        value = entry->learn_flags & BCM_VLAN_LEARN_CONTROL_ENABLE ? 1 : 0;
        soc_mem_field32_set(unit, mem, buf, USE_VLAN_CMLf, value);

        if (value) {
            value = 0;
            if (!(entry->learn_flags & BCM_VLAN_LEARN_CONTROL_FWD)) {
               value |= (1 << 0);
            }
            if (entry->learn_flags & BCM_VLAN_LEARN_CONTROL_CPU) {
               value |= (1 << 1);
            }
            if (entry->learn_flags & BCM_VLAN_LEARN_CONTROL_PENDING) {
               value |= (1 << 2);
            }
            if (entry->learn_flags & BCM_VLAN_LEARN_CONTROL_ARL) {
               value |= (1 << 3);
            }
            soc_mem_field32_set(unit, mem, buf, CML_FLAGS_MOVEf, value);
            soc_mem_field32_set(unit, mem, buf, CML_FLAGS_NEWf, value);
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    /* Write vlan profile table entry. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, buf); 
    BCM_IF_ERROR_RETURN(rv);

    /* Copy to cpu packets that miss L2 lookup. */
    if (SOC_REG_FIELD_VALID(unit, VLAN_PROFILE_TABr, L2_MISS_TOCPUf)) {
        value = (entry->flags & BCM_VLAN_UNKNOWN_UCAST_TOCPU) ? 1 : 0;
        soc_reg_field32_modify(unit, VLAN_PROFILE_TABr, REG_PORT_ANY,
                               L2_MISS_TOCPUf, value);
    }

#if defined (BCM_TRX_SUPPORT)
    /* WRITE VLAN_PROFILE_2 portion. */
    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m)) {
        sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        mem = VLAN_PROFILE_2m;
        block = &entry->block;

        /* Bitmap A  */
        soc_mem_pbmp_field_set(unit, mem, buf, BLOCK_MASK_Af,
                               &block->first_mask);

        /* Bitmap B  */
        soc_mem_pbmp_field_set(unit, mem, buf, BLOCK_MASK_Bf,
                               &block->second_mask);

        /* Broadcast mask select. */
        soc_mem_field32_set(unit, mem, buf, BCAST_MASK_SELf,
                            block->broadcast_mask_sel);

        /* Unknown unicast mask select. */
        if (soc_mem_field_valid(unit, VLAN_PROFILE_2m, 
                                UNKNOWN_UCAST_MASK_SELf)) {
            soc_mem_field32_set(unit, mem, buf, UNKNOWN_UCAST_MASK_SELf,
                            block->unknown_unicast_mask_sel);
        }

        /* Unknown multicast mask select. */
        if (soc_mem_field_valid(unit, VLAN_PROFILE_2m, 
                                UNKNOWN_MCAST_MASK_SELf)) {
            soc_mem_field32_set(unit, mem, buf, UNKNOWN_MCAST_MASK_SELf, 
                            block->unknown_mulitcast_mask_sel);
        }

        /* Unknown multicast mask select. */
        if (soc_mem_field_valid(unit, VLAN_PROFILE_2m, KNOWN_MCAST_MASK_SELf)) {
            soc_mem_field32_set(unit, mem, buf, KNOWN_MCAST_MASK_SELf, 
                            block->known_mulitcast_mask_sel);
        }

        /* Write vlan profile 2 entry. */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, buf);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_TRX_SUPPORT */

    return (BCM_E_NONE);
}
/*
 * Function:
 *    _bcm_vlan_profile_add
 * 
 * Purpose:
 *    Add a new entry to vlan profile table
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     entry - (IN) Vlan profile entry. 
 *     index - (OUT) Allocated/existing entry index.
 *
 * Return:
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_add(int unit, _vlan_profile_t *entry, int *index) 
{
    int   free_index = -1;    /* Unused proile index.    */
    int   idx;                /* Vlan profiles iterator. */
    int   idx_max;            /* Profile entry count.    */ 
    uint8 equal;              /* Prifiles are equal flag.*/
    int   rv;                 /* Operation return status.*/

    /* Input parameters check. */
    if (NULL == entry) {
        return (BCM_E_PARAM); 
    }

    idx_max = soc_mem_index_max(unit, VLAN_PROFILE_TABm);
    /* Search for an existing entry that has the same configuration.  */
    for (idx = 0; idx <= idx_max; idx++) {
        /* Compare profiles. */
        rv = _vlan_profile_compare(unit, &VLAN_PROFILE_ENTRY(unit, idx),
                                   entry, &equal);
        BCM_IF_ERROR_RETURN(rv);
        if (equal) {
            VLAN_PROFILE_REF_COUNT(unit, idx)++; 
#if defined (BCM_TRX_SUPPORT)
            if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
                int pkt_idx;

                /* should only increase reference counter when called
                 * on the existing entry
                 */
                rv = _bcm_xgs3_protocol_pkt_ctrl_set(unit, -1,
                            &entry->protocol_pkt, 
                            &pkt_idx);
                BCM_IF_ERROR_RETURN(rv);
            }
#endif /* BCM_TRX_SUPPORT */
            *index = idx;
            return (BCM_E_NONE);
        }

        if ((-1 == free_index) && (0 == VLAN_PROFILE_REF_COUNT(unit, idx))) {
            free_index = idx;
        }
    }

    /*  Entry not found & no free indexes. */
    if (-1 == free_index) {
        return BCM_E_RESOURCE;
    }

    /* 
     * Insert the new configuration into profile table
     */
    rv = _vlan_profile_write(unit, free_index, entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_profile_entry_init(unit, free_index, entry);
    BCM_IF_ERROR_RETURN(rv);

    *index = free_index;
    return BCM_E_NONE;
}

/*
 * Function :
 *    _vlan_profile_delete
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     index - (IN) Existing entry index.
 * 
 * Purpose  :
 *    Remove an entry from vlan profile table.
 *
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_profile_delete(int unit, int index) 
{
    uint32  buf[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.         */
    int     rv;                           /* Operation return status. */
#if defined (BCM_TRX_SUPPORT)
    vlan_profile_tab_entry_t entry;
    uint32 prot_pkt_index;
    int ref_count;
#endif /* BCM_TRX_SUPPORT */

    /* Input parameters check. */
    if ((index < 0) || (index > soc_mem_index_max(unit, VLAN_PROFILE_TABm))) {
        return (BCM_E_PARAM);
    }

    /* Decrement profile reference counter. */
    if (VLAN_PROFILE_REF_COUNT(unit, index) > 0) {
        VLAN_PROFILE_REF_COUNT(unit, index)--;
    }

#if defined (BCM_TRX_SUPPORT)
    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)
        && SOC_MEM_FIELD_VALID (unit,VLAN_PROFILE_TABm, PROTOCOL_PKT_INDEXf)) {
        BCM_IF_ERROR_RETURN
           (soc_mem_read(unit, VLAN_PROFILE_TABm, MEM_BLOCK_ANY, index, &entry));
        prot_pkt_index = soc_mem_field32_get(unit, VLAN_PROFILE_TABm, &entry,
                                             PROTOCOL_PKT_INDEXf);
        BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_ref_count_get (unit,
                               prot_pkt_index, &ref_count));
        if (ref_count > 0) {
            BCM_IF_ERROR_RETURN(_bcm_prot_pkt_ctrl_delete(unit,prot_pkt_index));
        }
    }
#endif /* BCM_TRX_SUPPORT */
    if (0 == VLAN_PROFILE_REF_COUNT(unit, index)) {
        /* Reset hw table entries. */
        sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
#if defined (BCM_TRX_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m)) {
            rv = soc_mem_write(unit, VLAN_PROFILE_2m, MEM_BLOCK_ALL, index, buf);
            BCM_IF_ERROR_RETURN(rv);
        }

#endif /* BCM_TRX_SUPPORT */

        rv = soc_mem_write(unit, VLAN_PROFILE_TABm, MEM_BLOCK_ALL, index, buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Reset sw vlan profile entry. */
        sal_memset(&VLAN_PROFILE_ENTRY(unit, index), 0, sizeof(_vlan_profile_t));
    }
    return (BCM_E_NONE);
}

#if defined (BCM_TRX_SUPPORT)
/*
 * Function:
 *    _vlan_protocol_pkt_ctrl_default_add
 *
 * Purpose:
 *    Add default vlan protocol packet control value
 *
 * Parameters:
 *     unit  - (IN)  BCM device number.
 *     protocol_pkt - (OUT) protocol packet actions to be configured.
 * Return:
 *    BCM_E_XXX
 */
STATIC int
_vlan_protocol_pkt_ctrl_default_add(int unit, bcm_vlan_protocol_packet_ctrl_t *protocol_pkt)
{
    soc_reg_t reg = PROTOCOL_PKT_CONTROLr;

    if (!soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
        return BCM_E_UNAVAIL;
    }

    if (SOC_REG_FIELD_VALID(unit, reg, MMRP_FWD_ACTIONf)) {
        protocol_pkt->mmrp_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, SRP_FWD_ACTIONf)) {
        protocol_pkt->srp_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    reg = IGMP_MLD_PKT_CONTROLr;
    if (SOC_REG_FIELD_VALID(unit, reg, IGMP_REP_LEAVE_FWD_ACTIONf)) {
        protocol_pkt->igmp_report_leave_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, IGMP_QUERY_FWD_ACTIONf)) {
        protocol_pkt->igmp_query_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, IGMP_UNKNOWN_MSG_FWD_ACTIONf)) {
        protocol_pkt->igmp_unknown_msg_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, MLD_REP_DONE_FWD_ACTIONf)) {
        protocol_pkt->mld_report_done_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, MLD_QUERY_FWD_ACTIONf)) {
        protocol_pkt->mld_query_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf)) {
        protocol_pkt->ip4_mc_router_adv_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, IPV4_RESVD_MC_PKT_FWD_ACTIONf)) {
        protocol_pkt->ip4_rsvd_mc_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, IPV6_RESVD_MC_PKT_FWD_ACTIONf)) {
        protocol_pkt->ip6_rsvd_mc_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }
    if (SOC_REG_FIELD_VALID(unit, reg, IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf)) {
        protocol_pkt->ip6_mc_router_adv_action= BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    return BCM_E_NONE;

}
#endif /*BCM_TRX_SUPPORT*/

/*
 * Function:
 *    _vlan_profile_default_add
 * 
 * Purpose:
 *    Add a default entry to vlan profile table
 *
 * Parameters:
 *     unit  - (IN)  BCM device number.
 *     index - (OUT) Profile index.
 * Return:
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_default_add(int unit, int *index) 
{
    int                     rv;        /* Operation return status. */
    _vlan_profile_t         entry;     /* Vlan profile_entry.      */
    bcm_vlan_mcast_flood_t  mode;      /* Mcast flood mode.        */

    /* Input parameters check. */
    if (NULL == index) {
        return (BCM_E_PARAM);
    }

    /* Reset entry structure. */
    sal_memset (&entry, 0, sizeof(_vlan_profile_t));

    /* Multicast flood mode. */
    rv = _bcm_esw_vlan_flood_default_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);

    entry.ip6_mcast_flood_mode = mode;
    entry.ip4_mcast_flood_mode = mode;
    entry.l2_mcast_flood_mode = mode;

    /* Default outer tpid value. */
    entry.outer_tpid = _bcm_fb2_outer_tpid_default_get(unit);

    /* Default VLAN Protocol Packet control */
#if defined (BCM_TRX_SUPPORT)
    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
        BCM_IF_ERROR_RETURN
            (_vlan_protocol_pkt_ctrl_default_add(unit, &entry.protocol_pkt));
    }
#endif

    return  _vlan_profile_add (unit, &entry, index);
}

/*
 * Function:
 *    _vlan_profile_update
 * 
 * Purpose:
 *    Update existing vlan "profile" 
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vlan    - (IN) VLAN id.
 *     buf     - (IN) Vlan table entry.
 *     profile - (IN) Updated profile
 * Return:
 *    BCM_E_XXX
 */
STATIC int 
_vlan_profile_update(int unit, bcm_vlan_t vlan, _vlan_profile_t *profile)
{
    uint32  buf[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.     */
    int rv;                       /* Operation return status.     */
    int idx;                      /* Vlan profile index.          */
    int oidx;                     /* Original vlan profile index. */
    uint8 equal;                  /* Profiles are equal flag.     */


    /* Input parameters check. */
    if (NULL == profile) {
        return (BCM_E_PARAM);
    }

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int) vlan, buf);
    BCM_IF_ERROR_RETURN(rv);

    BCM_IF_ERROR_RETURN(_bcm_vlan_valid_check(unit, VLAN_TABm, 
                                         (vlan_tab_entry_t *)buf, vlan));

    /* Get original vlan profile. */
    rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                               buf, vlan, &oidx);
    BCM_IF_ERROR_RETURN(rv);

    /* Compare original profile with new one. */ 
    rv = _vlan_profile_compare(unit, &VLAN_PROFILE_ENTRY(unit, oidx),
                               profile, &equal);
    BCM_IF_ERROR_RETURN(rv);

    if (TRUE == equal) {
        return (BCM_E_NONE);
    }

    /* Add new profile. */
    rv = _vlan_profile_add(unit, profile, &idx);
    if (BCM_SUCCESS(rv)) {
    }
    if (BCM_E_RESOURCE == rv) {
        /* Check if we can update an existing profile. */
        if (1 == VLAN_PROFILE_REF_COUNT(unit, oidx)) {
            rv = _vlan_profile_write(unit, oidx, profile);
            BCM_IF_ERROR_RETURN(rv);

            VLAN_PROFILE_ENTRY(unit, oidx) = *profile;
            VLAN_PROFILE_REF_COUNT(unit, oidx) = 1;
            idx = oidx;
        }
    } 
    BCM_IF_ERROR_RETURN(rv);

    /* Set new vlan profile index. */
    rv = _vlan_profile_idx_set(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                               buf, vlan, idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Write back vlan table entry. */
    rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int) vlan, buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete original vlan profile if new one was allocated. */
    if (oidx != idx) {
         rv = _vlan_profile_delete(unit, oidx);
         BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *    bcm_xgs3_vlan_mpls_table_init
 *
 * Purpose:
 *      Initialize the VLAN_MPLS table with the default entry in it.
 *
 * Parameters:
 *     unit  - (IN)  BCM device number.
 *     vd  - (IN)  VLAN data.
 *
 * Return:
 *    BCM_E_XXX
 */
STATIC int
_bcm_xgs3_vlan_mpls_table_init(int unit, bcm_vlan_data_t *vd)
{
    vlan_mpls_entry_t vlan_mpls;

    if ((SAL_BOOT_QUICKTURN || SAL_BOOT_PLISIM) && (!SAL_BOOT_BCMSIM) &&
         !SOC_IS_HAWKEYE(unit) && !SOC_IS_KATANA2(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "SIMULATION: skipped VLAN_MPLS table clear "
                            "(assuming hardware did it)\n")));
    } else if (!SOC_IS_RCPU_ONLY(unit)) {
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, VLAN_MPLSm, MEM_BLOCK_ALL, TRUE));
    }

    sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY, vd->vlan_tag,
                      &vlan_mpls));

    soc_mem_field32_set(unit, VLAN_MPLSm, &vlan_mpls, L3_IIFf, vd->vlan_tag);

    if (soc_feature(unit, soc_feature_mpls)) {
        soc_mem_field32_set(unit, VLAN_MPLSm, &vlan_mpls, MPLS_ENABLEf, 0x1);
    }
    if (soc_feature(unit, soc_feature_mim)) {
        soc_mem_field32_set(unit, VLAN_MPLSm, &vlan_mpls, MIM_TERM_ENABLEf,
                            0x1);
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_filter)) {
        
        soc_mem_field32_set(unit, VLAN_MPLSm, &vlan_mpls, EN_IFILTERf,
                            0x1);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, VLAN_MPLSm, MEM_BLOCK_ALL, vd->vlan_tag,
                       &vlan_mpls));

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *    bcm_xgs3_vlan_init
 * 
 * Purpose:
 *      Initialize the VLAN tables with the default entry in it.
 *
 * Parameters:
 *     unit  - (IN)  BCM device number.
 * 
 * Return:
 *    BCM_E_XXX
 */
static int
_bcm_xgs3_vlan_table_init(int unit, bcm_vlan_data_t *vd, soc_mem_t table)
{
    vlan_tab_entry_t    ve;
    bcm_pbmp_t          pbm;
    int                 rv;

#ifdef BCM_SHADOW_SUPPORT
    /* No EGR_VLAN support for shadow */
    if(SOC_IS_SHADOW(unit)) {
        return BCM_E_NONE;
    }
#endif

    assert(sizeof(vlan_tab_entry_t) >= sizeof(egr_vlan_entry_t));
    assert(sizeof(vlan_tab_entry_t) >= sizeof(vlan_2_tab_entry_t));

    if ((SAL_BOOT_QUICKTURN || SAL_BOOT_PLISIM) && (!SAL_BOOT_BCMSIM) &&
         !soc_mem_field32_get(unit, table,
                              soc_mem_entry_null(unit, table), VALIDf) && 
         !SOC_IS_HAWKEYE(unit) && !SOC_IS_KATANA2(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "SIMULATION: skipped VLAN table clear "
                            "(assuming hardware did it)\n")));
    } else if (!SOC_IS_RCPU_ONLY(unit)) {
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, table, MEM_BLOCK_ALL, TRUE));
    }

    sal_memcpy(&ve, soc_mem_entry_null(unit, table),
               soc_mem_entry_words(unit, table) * 4);

    BCM_PBMP_ASSIGN(pbm, vd->ut_port_bitmap);
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        bcm_pbmp_t temp_pbm;

        BCM_PBMP_ASSIGN(temp_pbm, PBMP_E_ALL(unit));
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &temp_pbm));

#ifndef BCM_VLAN_NO_DEFAULT_ETHER
        BCM_PBMP_OR(pbm, temp_pbm);
#endif /* BCM_VLAN_NO_DEFAULT_ETHER */
    } else
#endif /* BCM_KATANA2_SUPPORT */
    {
#ifndef BCM_VLAN_NO_DEFAULT_ETHER
        BCM_PBMP_OR(pbm, PBMP_E_ALL(unit));
#endif /* BCM_VLAN_NO_DEFAULT_ETHER */
    }

    if (TABLE_HAS_T_BITMAP(unit, table)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((table == VLAN_TABLE(unit)) &&
            soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {

            /* Use profile based approach to the port bitmap */
            uint32 profile_idx;

            /* Initialize the port bitmap profile table */
            BCM_IF_ERROR_RETURN(_bcm_th3_vlan_pbmp_profile_init(unit));

            /* Insert the port bitmap into the profile table */
            BCM_IF_ERROR_RETURN(
                _bcm_th3_vlan_pbmp_profile_entry_add(
                    unit, vd->port_bitmap, &profile_idx));

            /* Set the corressponding profile index into the VLAN table */
            soc_mem_field32_set(
                unit, table, &ve, PORT_BITMAP_PROFILE_PTRf, profile_idx);
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
        {
            soc_mem_pbmp_field_set(
                unit, table, &ve, PORT_BITMAPf, &vd->port_bitmap);
        }

        if ((table == VLAN_TABLE(unit)) &&
            SOC_MEM_FIELD_VALID(unit, table, ING_PORT_BITMAPf)) { 
            soc_mem_pbmp_field_set(unit, table, &ve, ING_PORT_BITMAPf, 
                                &vd->port_bitmap); 
        } 
    }
    /* This function will be called with VLAN_TABm, EGR_VLANm, and VLAN_2_TABm
     * for TH2 and TH3. In TH2/TH3, VLAN_2_TABm has STGf while VLAN_TABm does
     * not. The check is to prevent STGf being set wrongly.
     */
    if (SOC_MEM_FIELD_VALID(unit, table, STGf)) {
    soc_mem_field32_set(unit, table, &ve, STGf, BCM_STG_DEFAULT);
    }
    soc_mem_field32_set(unit, table, &ve, VALIDf, 1);

#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_FIELD_VALID(unit, table, L3_IIFf)) {
        uint32  l3_iif_value;
        
        if (soc_feature(unit, soc_feature_l3_iif_under_4k)) {
#if defined(BCM_HURRICANE4_SUPPORT)
            if (soc_feature(unit, soc_feature_vlan_default_l3_iif_as_one)) {
                l3_iif_value = 1;
            } else
#endif
            {
                l3_iif_value = 0;
            }
        } else {
            if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
                l3_iif_value = vd->vlan_tag;
            } else {
                /* L3_IIFf value should be 0 once device can't support L3 
                 * ingress interface 
                */
                l3_iif_value = 0;
            }
        }
       soc_mem_field32_set(unit, table, &ve, L3_IIFf, l3_iif_value);
    }
#endif
#if defined(BCM_EASY_RELOAD_SUPPORT)
    if (SOC_IS_RELOADING(unit)) { 
        _bcm_vlan_valid_set(unit, table, vd->vlan_tag, 1);
    }
#endif

    if (TABLE_HAS_UT_BITMAP(unit, table)) {
        soc_mem_pbmp_field_set(unit, table, &ve, UT_BITMAPf, &pbm);
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_filter)) {
        if (table == VLAN_TABm && SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, table, &ve, EN_IFILTERf, 0x1);
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    if ((table == EGR_VLANm && SOC_IS_FBX(unit)) ||
        (table == VLAN_TABm && SOC_IS_KATANA2(unit))) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {
            uint16                    tpid;
            int                       tpid_index;

            tpid = _bcm_fb2_outer_tpid_default_get(unit);

            /* Add the default outer TPID entry twice during init so that
             * the default entry does not get removed even when no table 
             * entry is referencing to the default TPID entry.
             */  
            BCM_IF_ERROR_RETURN
                (_bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_index));
            BCM_IF_ERROR_RETURN
                (_bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_index));
            soc_mem_field32_set(unit, table, &ve, OUTER_TPID_INDEXf,
                                tpid_index); 
        } 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)

        if (soc_feature(unit, soc_feature_vlan_filter)) {
            
            soc_mem_field32_set(unit, table, &ve, EN_EFILTERf, 0x1);
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    }

    if (TABLE_HAS_PFM(unit, table)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {
            int                       profile_index;

            BCM_IF_ERROR_RETURN (_vlan_profile_init(unit));
            rv = _vlan_profile_default_add(unit, &profile_index);
            BCM_IF_ERROR_RETURN(rv);

            rv = _vlan_profile_idx_set(unit, table, VLAN_PROFILE_PTRf,
                                       (uint32 *)&ve, vd->vlan_tag, 
                                       profile_index);
            BCM_IF_ERROR_RETURN(rv);
        } else
#endif
        {
            bcm_vlan_mcast_flood_t mode;
            int pfm;

            rv = _bcm_esw_vlan_flood_default_get(unit, &mode);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_vlan_mcast_flood_mode_to_pfm(unit, mode, &pfm);
            BCM_IF_ERROR_RETURN(rv);
            if (SOC_MEM_FIELD_VALID(unit, table, PFMf)) {
            soc_mem_field32_set(unit, table, &ve, PFMf, pfm);
        }
    }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
        vlan_attrs_1_entry_t vlan_attrs;
        sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs_1_entry_t));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_ATTRS_1m, MEM_BLOCK_ANY, vd->vlan_tag, 
                          &vlan_attrs));

        soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, STGf,
                            BCM_STG_DEFAULT);
        soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, VALIDf, 1);
        soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, FID_IDf, 0);

        if (soc_feature(unit, soc_feature_mpls)) {
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, MPLS_ENABLEf, 0x1);
        }
        if (soc_feature(unit, soc_feature_mim)) {
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, MIM_TERM_ENABLEf, 0x1);
        }
        if (soc_feature(unit, soc_feature_vlan_filter)) {
            
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, EN_IFILTERf,
                    0x1);
        }
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, VLAN_ATTRS_1m, MEM_BLOCK_ALL, vd->vlan_tag, 
                           &vlan_attrs)); 
    }
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L3_IIFf)) {
        vlan_tab_entry_t vlan_tab;
        sal_memset(&vlan_tab, 0, sizeof(vlan_tab_entry_t));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, vd->vlan_tag,
                          &vlan_tab));

        soc_mem_field32_set(unit, VLAN_TABm, &vlan_tab, L3_IIFf,
                            vd->vlan_tag);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, vd->vlan_tag,
                           &vlan_tab));
    }
#endif

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, table, MEM_BLOCK_ALL, vd->vlan_tag, &ve));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (table == EGR_VLANm &&
        soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_vlan_vfi_untag_init(unit, vd->vlan_tag,
                                         vd->ut_port_bitmap));
    }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_set(
            unit, vd->vlan_tag, (table == EGR_VLANm), TRUE, vd->port_bitmap));
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_vlan_create
 * Purpose:
 *      Create a VLAN entry in VLAN_TAB
 */

static int
_bcm_xgs3_vlan_table_create(int unit, bcm_vlan_t vid, int table)
{
    vlan_tab_entry_t            vt;
    int                         rv;
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    int       ingress_map_mode=0;
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
    int use_default = TRUE;  /* use default value for VLAN_TABm  */

    assert(sizeof(vlan_tab_entry_t) >= sizeof(egr_vlan_entry_t));
    assert(sizeof(vlan_tab_entry_t) >= sizeof(vlan_2_tab_entry_t));

    /* Upper layer already checks that vid is valid */

    soc_mem_lock(unit, table);

    if ((rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt)) < 0) {
        soc_mem_unlock(unit, table);
        return rv;
    }

    if (!SOC_IS_RELOADING(unit)) {
        if (soc_mem_field32_get(unit, table, &vt, VALIDf) !=
            soc_mem_field32_get(unit, table,
                                soc_mem_entry_null(unit, table), VALIDf)) {
            if (!SOC_IS_KATANA2(unit)) {
                soc_mem_unlock(unit, table);
                return BCM_E_EXISTS;
            }
        }
    }

    /* don't use default value if already pre-configured*/
    if (SHR_BITGET(vlan_info[unit].pre_cfg_bmp.w,vid)) { 
        use_default = FALSE;
    }
    /* This function will be called with VLAN_TABm, EGR_VLANm, and VLAN_2_TABm
     * in TH2/TH3. In TH2/TH3, VLAN_2_TABm has STGf while VLAN_TABm does not.
     * The check is to prevent STGf being set wrongly. The STG value doesn't
     * matter much; it will get changed immediately after this routine completes
     */
    if (SOC_MEM_FIELD_VALID(unit, table, STGf)) {
    soc_mem_field32_set(unit, table, &vt, STGf, 0);
    }
    soc_mem_field32_set(unit, table, &vt, VALIDf, 1);

#if defined(BCM_EASY_RELOAD_SUPPORT)
    if (SOC_IS_RELOADING(unit)) {
        _bcm_vlan_valid_set(unit, table, vid, 1);
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if ((table == VLAN_TABLE(unit)) &&
            soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        pbmp_t pbmp;
        uint32 profile_idx;

        /* Insert a profile entry with empty port bitmap */
        SOC_PBMP_CLEAR(pbmp);
        rv = _bcm_th3_vlan_pbmp_profile_entry_add(unit, pbmp, &profile_idx);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, table);
            return rv;
        }

        /* Set the corressponding profile index into the VLAN table entry */
        soc_mem_field32_set(
            unit, table, &vt, PORT_BITMAP_PROFILE_PTRf, profile_idx);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    /* Set default vrf id if supported. */
    if (use_default && SOC_MEM_FIELD_VALID(unit, table, VRF_IDf)) {
        soc_mem_field32_set(unit, table, &vt, VRF_IDf, 0);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_SCORPION_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    /* Set L3_IIF, using vid as index into L3_IIF table */
    if (SOC_MEM_FIELD_VALID(unit, table, L3_IIFf)) {
        uint32 ingress_intf = 0;

        /* Check L3 Ingress Interface Map mode. */
        if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
           BCM_IF_ERROR_RETURN(
              bcm_xgs3_l3_ingress_intf_map_get(unit, &ingress_map_mode));

            if (ingress_map_mode) {
                if (soc_feature(unit, soc_feature_l3_iif_under_4k)) {
#if defined(BCM_HURRICANE4_SUPPORT)
                    if (soc_feature(unit, soc_feature_vlan_default_l3_iif_as_one)) {
                        ingress_intf = 1;
                    } else
#endif
                    {
                        ingress_intf = 0;
                    }
                } else {
                    if (use_default) {
                        ingress_intf = BCM_L3_INGRESS_INTERFACE_INVALID;
                    }
                }
            } else {
                ingress_intf = vid;
            }
        } else {
            /* L3_IIFf for those device can't support L3 ingress interface 
             * should be never referenced. Remaining the value at 0.
             */
            ingress_intf = 0;
        }

        soc_mem_field32_set(unit, table, &vt, L3_IIFf, ingress_intf);
    }
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */

    if (TABLE_HAS_UT_BITMAP(unit, table)) {
        pbmp_t pbmp;
        SOC_PBMP_CLEAR(pbmp);
        soc_mem_pbmp_field_set(unit, table, &vt, UT_BITMAPf, &pbmp);
    }

    if ((table == EGR_VLANm && SOC_IS_FBX(unit)) ||
        (table == VLAN_TABm && SOC_IS_KATANA2(unit))) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if (use_default && soc_feature(unit, soc_feature_vlan_ctrl)) {
            uint16                    tpid;
            int                       tpid_index;

            tpid = _bcm_fb2_outer_tpid_default_get(unit);

            _bcm_fb2_outer_tpid_tab_lock(unit);
            rv =  _bcm_fb2_outer_tpid_entry_add(unit, tpid, &tpid_index);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
    
            if (BCM_SUCCESS(rv)) {
                soc_mem_field32_set(unit, table, &vt, OUTER_TPID_INDEXf,
                                    tpid_index); 
            }
        } 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_filter)) {
            
            soc_mem_field32_set(unit, table, &vt, EN_EFILTERf, 0x1);
        }
#endif
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_filter)) {
        if (table == VLAN_TABm && SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_set(unit, table, &vt, EN_IFILTERf, 0x1);
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
    if (table == EGR_VLANm &&
        soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) {
        pbmp_t pbmp;
        SOC_PBMP_CLEAR(pbmp);
        BCM_IF_ERROR_RETURN
            (bcm_td3_vlan_vfi_untag_add(unit, vid, pbmp, pbmp));
    }
#endif /* BCM_TRIDENT3_SUPPORT */

   /*
     * Multicast flood setting
     */
    if (TABLE_HAS_PFM(unit, table)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {

            if (use_default) {
                int    profile_index, rv;
    
                rv = _vlan_profile_default_add(unit, &profile_index);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, table);
                    return rv;
                }
    
                rv = _vlan_profile_idx_set(unit, table, VLAN_PROFILE_PTRf,
                                           (uint32 *)&vt, vid, profile_index);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, table);
                    return rv;
                }
    
                rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);
                if (BCM_FAILURE(rv)) {
                    _vlan_profile_delete(unit, profile_index);
                }
                soc_mem_unlock(unit, table);
                return rv;
            }
        } else 
#endif
        {
            bcm_vlan_mcast_flood_t mode;
            int pfm;

            rv =_bcm_esw_vlan_flood_default_get(unit, &mode);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, table);
                return rv;
            }
            rv = _bcm_vlan_mcast_flood_mode_to_pfm(unit, mode, &pfm);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, table);
                return rv;
            }

            if (SOC_MEM_FIELD_VALID(unit, table, PFMf)) {
            soc_mem_field32_set(unit, table, &vt, PFMf, pfm);
        }
    }
    }

    if (BCM_SUCCESS(rv)) {
        rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);
    }

    soc_mem_unlock(unit, table);

    return rv;
}

/*
 * Function:
 *      bcm_xgs3_vlan_destroy
 * Purpose:
 *      Clear a VLAN entry in VLAN_TAB
 */

static int
_bcm_xgs3_vlan_table_destroy(int unit, bcm_vlan_t vid, int table)
{
    vlan_tab_entry_t            vt;
    int                         rv;

    /* Upper layer already checks that vid is valid */

    soc_mem_lock(unit, table);

    if ((rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt)) < 0) {
        soc_mem_unlock(unit, table);
        return rv;
    }

    if (_bcm_vlan_valid_check(unit, table, &vt, vid) == BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, table);
        return BCM_E_NOT_FOUND;
    }

#if defined(BCM_EASY_RELOAD_SUPPORT)
    if (SOC_IS_RELOADING(unit)) {
        _bcm_vlan_valid_set(unit, table, vid, 0);
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if ((VLAN_TABLE(unit) == table) &&
        soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
        int profile_ptr =
            soc_mem_field32_get(unit, table, &vt, PORT_BITMAP_PROFILE_PTRf);
        rv = _bcm_th3_vlan_pbmp_profile_entry_delete(unit, profile_ptr);

        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, table);
            return rv;
        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid,
                       soc_mem_entry_null(unit, table));
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, table);
        return rv;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        if (table == VLAN_TABm) {
            int     profile_ptr, tpid_ptr;

            _bcm_fb2_outer_tpid_tab_lock(unit);

            rv = _vlan_profile_idx_get(unit, table, VLAN_PROFILE_PTRf,
                                       (uint32 *)&vt, vid, &profile_ptr);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, table);
                return rv;
            }

            rv = _bcm_fb2_outer_tpid_lkup(unit, 
                                          VLAN_PROFILE_ENTRY(unit, profile_ptr).outer_tpid,
                                          &tpid_ptr);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, table);
                return rv;
            }

            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_ptr);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, table);
                return rv;
            }

            rv = _vlan_profile_delete(unit, profile_ptr);
            _bcm_fb2_outer_tpid_tab_unlock(unit);
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        /* Release Service counter, if any */
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeService, vid);
    }
#endif

    soc_mem_unlock(unit, table);
    return rv;
}

/*
 * Function:
 *      _bcm_xgs3_vlan_table_port_add
 * Purpose:
 *      Add ports to the VLAN bitmap in a VLAN_TAB entry.
 */
int
_bcm_xgs3_vlan_table_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp,
                              pbmp_t ubmp, pbmp_t ing_pbmp, int table)
{
    vlan_tab_entry_t            vt;
    int                         rv;
    bcm_pbmp_t                  cur_pbmp;

    /* Only allow untagged ports on the vlan. */
    SOC_PBMP_AND(ubmp, pbmp);

    /* Draco does not have room for CMIC/IPIC in ubmp */
    SOC_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));
    SOC_PBMP_REMOVE(ubmp, PBMP_ST_ALL(unit));

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        soc_info_t *si = &SOC_INFO(unit);
        soc_port_t port;

        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {
            SOC_PBMP_PORT_REMOVE(pbmp, port);
        }
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    /* Upper layer already checks that vid is valid */

    soc_mem_lock(unit, table);

    if ((rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt)) < 0) {
        soc_mem_unlock(unit, table);
        return rv;
    }

    if ((rv = _bcm_vlan_valid_check(unit, table, &vt, vid)) == BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, table);
        return rv;
    }

    /*
     * Update the entry for this VLAN.
     */
    if (TABLE_HAS_T_BITMAP(unit, table)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((table == VLAN_TABLE(unit)) &&
            soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
            rv = _bcm_th3_vlan_pbmp_port_add(unit, &vt, &pbmp);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, table);
                return rv;
            }
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
        {
            soc_mem_pbmp_field_get(unit, table, &vt, PORT_BITMAPf, &cur_pbmp);
            BCM_PBMP_OR(cur_pbmp, pbmp);
            soc_mem_pbmp_field_set(unit, table, &vt, PORT_BITMAPf, &cur_pbmp);
        }
    }

    if (TABLE_HAS_UT_BITMAP(unit, table)) {
        soc_mem_pbmp_field_get(unit, table, &vt, UT_BITMAPf, &cur_pbmp);
        BCM_PBMP_REMOVE(cur_pbmp, pbmp);
        BCM_PBMP_OR(cur_pbmp, ubmp);
        soc_mem_pbmp_field_set(unit, table, &vt, UT_BITMAPf, &cur_pbmp);
    }

    if (soc_mem_field_valid(unit, table, ING_PORT_BITMAPf)) {
        soc_mem_pbmp_field_get(unit, table, &vt, ING_PORT_BITMAPf, &cur_pbmp);
        BCM_PBMP_OR(cur_pbmp, ing_pbmp);
        soc_mem_pbmp_field_set(unit, table, &vt, ING_PORT_BITMAPf, &cur_pbmp);
    }

    rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);

    soc_mem_unlock(unit, table);

    return rv;
}

/*
 * Function:
 *      bcm_xgs3_vlan_port_remove
 * Purpose:
 *      Remove ports from the VLAN bitmap in a VLAN_TAB entry.
 */

int
_bcm_xgs3_vlan_table_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp,
                                 pbmp_t ubmp, pbmp_t ing_pbmp, int table)
{
    vlan_tab_entry_t            vt;
    int                         rv;
    bcm_pbmp_t                  cur_pbmp;

    /* Upper layer already checks that vid is valid */

    soc_mem_lock(unit, table);

    if ((rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt)) < 0) {
        soc_mem_unlock(unit, table);
        return rv;
    }

    if ((rv = _bcm_vlan_valid_check(unit, table, &vt, vid)) == BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, table);
        return rv;
    }

    if (TABLE_HAS_T_BITMAP(unit, table)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((table == VLAN_TABLE(unit)) &&
            soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
            rv = _bcm_th3_vlan_pbmp_port_remove(unit, &vt, &pbmp);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, table);
                return rv;
            }
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
        {
            soc_mem_pbmp_field_get(unit, table, &vt, PORT_BITMAPf, &cur_pbmp);
            BCM_PBMP_REMOVE(cur_pbmp, pbmp);
            soc_mem_pbmp_field_set(unit, table, &vt, PORT_BITMAPf, &cur_pbmp);
        }
    }

    if (TABLE_HAS_UT_BITMAP(unit, table)) {
        soc_mem_pbmp_field_get(unit, table, &vt, UT_BITMAPf, &cur_pbmp);
        BCM_PBMP_REMOVE(cur_pbmp, ubmp);
        soc_mem_pbmp_field_set(unit, table, &vt, UT_BITMAPf, &cur_pbmp);
    }

    if (soc_mem_field_valid(unit, table, ING_PORT_BITMAPf)) {
        soc_mem_pbmp_field_get(unit, table, &vt, ING_PORT_BITMAPf, &cur_pbmp);
        BCM_PBMP_REMOVE(cur_pbmp, ing_pbmp);
        soc_mem_pbmp_field_set(unit, table, &vt, ING_PORT_BITMAPf, &cur_pbmp);
    }

    rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);

    soc_mem_unlock(unit, table);

    return rv;
}

/*
 * Function:
 *      bcm_xgs3_vlan_table_port_get
 * Purpose:
 *      Read the port bitmap from a vlan entry. Both VLAN_TAB and EGR_VLAN
 *      shall be identical, except for the untagged bitmap, which is only
 *      available via EGR_VLAN.
 */

int
_bcm_xgs3_vlan_table_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp,
                              pbmp_t *ubmp, pbmp_t *ing_pbmp, uint32 table)
{
    vlan_tab_entry_t            vt;

    /* Upper layer already checks that vid is valid */

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt));

    BCM_IF_ERROR_RETURN(_bcm_vlan_valid_check(unit, table, &vt, vid));

    if (TABLE_HAS_T_BITMAP(unit, table)) {
        if (pbmp != NULL) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if ((table == VLAN_TABLE(unit)) &&
                soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {

                int rv;
                uint32 profile_idx;

                /* Retrieve the profile index from the VLAN table entry */
                profile_idx = soc_mem_field32_get(
                    unit, VLAN_2_TABm, &vt, PORT_BITMAP_PROFILE_PTRf);

                /* Based on the index retrieve the pbmp from the profile mem */
                rv = _bcm_th3_vlan_pbmp_profile_entry_get(  unit,
                                                            profile_idx,
                                                            pbmp);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
            {
            soc_mem_pbmp_field_get(unit, table, &vt, PORT_BITMAPf, pbmp);
        }
    }
    }

    if (soc_mem_field_valid(unit, table, UT_BITMAPf)) {
        if (ubmp != NULL) {
            soc_mem_pbmp_field_get(unit, table, &vt, UT_BITMAPf, ubmp);
        }
    }

    if (soc_mem_field_valid(unit, table, ING_PORT_BITMAPf)) {
        if (ing_pbmp != NULL) {
            soc_mem_pbmp_field_get(unit, table, &vt, ING_PORT_BITMAPf,
                                   ing_pbmp);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_vlan_stg_get
 * Purpose:
 *      Read the STG number from a vlan entry.
 */

int
bcm_xgs3_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    vlan_tab_entry_t    vt;

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
     if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
         return bcm_td2p_vp_group_vlan_vpn_stg_get(unit, vid, FALSE, stg_ptr);
     }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, VLAN_TABLE(unit), MEM_BLOCK_ANY, (int) vid, &vt));

    BCM_IF_ERROR_RETURN(_bcm_vlan_valid_check(unit, VLAN_TABLE(unit), &vt, vid));

    *stg_ptr = soc_mem_field32_get(unit, VLAN_TABLE(unit), &vt, STGf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_vlan_stg_set
 * Purpose:
 *      Update the STG number for a VLAN_TAB entry.
 */

static int
_bcm_xgs3_vlan_table_stg_set(int unit, bcm_vlan_t vid,
                              bcm_stg_t stg, int table)
{
    vlan_tab_entry_t    vt;
    int                 rv;

    /* Upper layer already checks that vid is valid */

    soc_mem_lock(unit, table);

    rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt);

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_vlan_valid_check(unit, table, &vt, vid);
    }

    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, table, &vt, STGf, stg);

        rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);
    }

    if (BCM_SUCCESS(rv) &&
        SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
        rv = soc_mem_field32_modify(unit, VLAN_ATTRS_1m, (int)vid, STGf, stg);
    }

    soc_mem_unlock(unit, table);

    return rv;
}

#ifdef BCM_KATANA_SUPPORT
/*
 * Function :
 *    _vlan_kt_cosq_map_set
 * 
 * Purpose  :
 *    Set per vlan cosq number.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     vid   - (IN) Vlan table entry buffer.
 *     control - (IN) vlan control structure
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_kt_cosq_map_set(int unit, bcm_vlan_t vid, bcm_vlan_control_vlan_t *control)
{
    vlan_cos_map_entry_t vcm;
    int rv;

    rv = soc_mem_read(unit, VLAN_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &vcm);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field32_set(unit, VLAN_COS_MAPm, &vcm, VALIDf, 
                        ((control->flags & BCM_VLAN_COSQ_ENABLE) ? 1 : 0));

    if (control->flags & BCM_VLAN_COSQ_ENABLE) {

        if ((control->cosq < 0) || (control->cosq > 63)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, VLAN_COS_MAPm, &vcm, VLAN_COSf, control->cosq);
        if ((((control->qos_map_id >> _BCM_QOS_MAP_SHIFT) !=
                     _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE)) ||
                     ((control->qos_map_id & _BCM_QOS_MAP_TYPE_MASK) >
                                     _BCM_QOS_MAP_ING_QUEUE_OFFSET_MAX)) {
            /* value of 0 is not a error case. It should set
                                   the QUEUE_OFFSET_PROFILE_INDEXf to 0 */
            if (control->qos_map_id != 0) {
                return (BCM_E_PARAM);
            }
        }
        soc_mem_field32_set(unit, VLAN_COS_MAPm, &vcm,
                     QUEUE_OFFSET_PROFILE_INDEXf,
                     (control->qos_map_id & _BCM_QOS_MAP_TYPE_MASK));
    }

    /* Write back updated buffer. */
    rv = soc_mem_write(unit, VLAN_COS_MAPm, MEM_BLOCK_ALL, (int)vid, &vcm);
    return (rv);
}

/*
 * Function :
 *    _vlan_kt_cosq_map_get
 * 
 * Purpose  :
 *    Get per vlan cosq number.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     vid   - (IN) Vlan table entry buffer.
 *     control- (OUT) vlan control structure.
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_kt_cosq_map_get(int unit, bcm_vlan_t vid, bcm_vlan_control_vlan_t *control) 
{
    vlan_cos_map_entry_t vcm;
    int rv;

    rv = soc_mem_read(unit, VLAN_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &vcm);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == soc_mem_field32_get(unit, VLAN_COS_MAPm, &vcm, VALIDf)) {
        return (BCM_E_NONE);
    }

    control->flags |= BCM_VLAN_COSQ_ENABLE;
    control->cosq = soc_mem_field32_get(unit, VLAN_COS_MAPm, &vcm, VLAN_COSf);
    control->qos_map_id = _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE <<
                                                               _BCM_QOS_MAP_SHIFT;
    control->qos_map_id |= soc_mem_field32_get(unit, VLAN_COS_MAPm, &vcm,
                                            QUEUE_OFFSET_PROFILE_INDEXf);
    return (BCM_E_NONE);
}
#endif /* BCM_KATANA_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_vlan_reload
 * Purpose:
 *      VLAN reload
 */

static int
_bcm_xgs3_vlan_table_reload(int unit, vbmp_t *bmp, int *count, int table)
{
    vlan_tab_entry_t *vlan_entry, *vlan_tbl;
    int index, total, vlan_tbl_size;
    bcm_pbmp_t cur_pbmp;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
        || defined(BCM_RAVEN_SUPPORT)
    int profile_ptr;
    int outer_tpid, i;
#endif
#if defined(BCM_TRX_SUPPORT)
    int   rv;                 /* Operation return status.*/
    uint32  buf[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.     */
    uint32  value;
#endif

    vlan_tbl_size = sizeof(vlan_tab_entry_t) * \
                    soc_mem_index_count(unit, table);
    vlan_tbl = soc_cm_salloc(unit, vlan_tbl_size, "vlan tbl dma");
    if (vlan_tbl == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset((void *)vlan_tbl, 0, vlan_tbl_size);
    if (soc_mem_read_range(unit, table, MEM_BLOCK_ANY,
                           soc_mem_index_min(unit, table),
                           soc_mem_index_max(unit, table), vlan_tbl) < 0) {
        soc_cm_sfree(unit, vlan_tbl);
        return BCM_E_INTERNAL;
    }

    soc_mem_lock(unit, table);

    total = *count;

    for (index = soc_mem_index_min(unit, table);
         index <= soc_mem_index_max(unit, table); index++) {
         vlan_entry = soc_mem_table_idx_to_pointer(unit, table,
                                                   vlan_tab_entry_t *,
                                                   vlan_tbl, index);

         if (soc_mem_field32_get(unit, table, vlan_entry, VALIDf)) {
             if (TABLE_HAS_T_BITMAP(unit, table)) {
#if defined(BCM_TRX_SUPPORT) && defined(BCM_TOMAHAWK3_SUPPORT)
                if ((table == VLAN_TABLE(unit)) &&
                    soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
                    BCM_PBMP_CLEAR(cur_pbmp);
                    profile_ptr =
                        soc_mem_field32_get(
                            unit, table, vlan_entry, PORT_BITMAP_PROFILE_PTRf);
                    rv = _bcm_th3_vlan_pbmp_profile_entry_get(
                                                unit, profile_ptr, &cur_pbmp);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, table);
                        soc_cm_sfree(unit, vlan_tbl);
                        return rv;
                    }
                } else
#endif /* BCM_TRX_SUPPORT && BCM_TOMAHAWK3_SUPPORT */
                {
                    soc_mem_pbmp_field_get(
                        unit, table, vlan_entry, PORT_BITMAPf, &cur_pbmp);
                }
                 if (BCM_PBMP_IS_NULL(cur_pbmp)) {
                    if ((EGR_VLANm != table) && !SHR_BITGET(bmp->w, index)) {
                         
                         continue;
                     }
                 }
             }
             if (!SHR_BITGET(bmp->w, index)) {
                 SHR_BITSET(bmp->w, index);
                 total++;
             }
 
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
        || defined(BCM_RAVEN_SUPPORT)
             profile_ptr = -1;
             if (soc_mem_field_valid(unit, table, VLAN_PROFILE_PTRf)) {
                 profile_ptr = soc_mem_field32_get(unit, table, vlan_entry, 
                                                   VLAN_PROFILE_PTRf);
             }

             if (profile_ptr >= 0) {
                 VLAN_PROFILE_REF_COUNT(unit, profile_ptr)++;

                 outer_tpid = VLAN_PROFILE_ENTRY(unit, profile_ptr).outer_tpid;
                 for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
                     if (OUTER_TPID_ENTRY(unit, i) == outer_tpid) {
                         OUTER_TPID_REF_COUNT(unit, i)++;
                         break;
                     }
                }
#if defined(BCM_TRX_SUPPORT)
                if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl) &&
                    SOC_MEM_FIELD_VALID (unit,VLAN_PROFILE_TABm, PROTOCOL_PKT_INDEXf)) {
                     
                    rv = soc_mem_read(unit, VLAN_PROFILE_TABm, MEM_BLOCK_ANY, profile_ptr, buf);
                    BCM_IF_ERROR_RETURN(rv);
                    
                    value = soc_mem_field32_get(unit, VLAN_PROFILE_TABm, buf, PROTOCOL_PKT_INDEXf);
                    
                    _bcm_prot_pkt_ctrl_reference(unit, value);
                 }
#endif /*  BCM_TRX_SUPPORT  */             
             }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
             if (soc_feature(unit, soc_feature_gport_service_counters)) {
                 if (soc_mem_field_valid(unit, table, SERVICE_CTR_IDXf)) {
                     int fs_idx;
                     fs_idx =
                         soc_mem_field32_get(unit, table, vlan_entry,
                                             SERVICE_CTR_IDXf);
                     if (fs_idx) {
                         _bcm_esw_flex_stat_reinit_add(unit,
                                _bcmFlexStatTypeService, fs_idx, index);
                     }
                 }
             }
#endif
        }
    }
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if ((VLAN_TABm == table) &&
        soc_mem_field_valid(unit, VLAN_TABm, VLAN_PROFILE_PTRf)) {
        /* Create a dummy reference to default outer TPID entry so
         * that the default entry never gets removed.  */
        OUTER_TPID_REF_COUNT(unit, 0)++;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */                                      
    soc_mem_unlock(unit, table);
    soc_cm_sfree(unit, vlan_tbl);
    *count = total;

    return BCM_E_NONE;
}

int
bcm_xgs3_vlan_init(int unit, bcm_vlan_data_t *vd)
{
    if (SOC_IS_HAWKEYE(unit)) {
        uint32 rval;
        SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &rval));
        soc_reg_field_set(unit, VLAN_CTRLr, &rval, USE_LEARN_VIDf, 0);
        soc_reg_field_set(unit, VLAN_CTRLr, &rval, LEARN_VIDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, rval));
        
        SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, EGR_CONFIGr, &rval, USE_LEARN_VIDf, 0);
        soc_reg_field_set(unit, EGR_CONFIGr, &rval, LEARN_VIDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_mpls_table_init(unit, vd));
    }
#endif

    /* Must call on EGR_VLANm before VLAN_TABm */
    if (SOC_IS_FBX(unit) && SOC_MEM_IS_VALID(unit, EGR_VLANm)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_init(unit, vd, EGR_VLANm));
    }

    BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_init(unit, vd, VLAN_TABm));
    if (SOC_MEM_IS_VALID(unit,VLAN_2_TABm)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_init(unit, vd, VLAN_2_TABm));
    }
    return BCM_E_NONE;
}

int
bcm_xgs3_vlan_create(int unit, bcm_vlan_t vid)
{

#if defined(BCM_TRIUMPH2_SUPPORT)
    soc_mem_t vlan_mem = VLAN_MPLSm;
    int l3IntfMapMode=0;
    uint32    entry[SOC_MAX_MEM_FIELD_WORDS];
    int       vlan_mem_valid = 0;
#endif


#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        vlan_mem = VLAN_TABm;
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, vlan_mem)) {
        vlan_mem_valid = 1;
        sal_memset (entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, vlan_mem, MEM_BLOCK_ANY, vid, entry));
    }

    COMPILER_REFERENCE(vlan_mem_valid);
    COMPILER_REFERENCE(l3IntfMapMode);
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_filter) && vlan_mem_valid) {
            soc_mem_field32_set(unit, vlan_mem, entry, EN_IFILTERf,
                    0x1);
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, vlan_mem, MEM_BLOCK_ALL, vid, entry));
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */


#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)  /* the API requires INCLUDE_L3 */

    if (vlan_mem_valid) {
        if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
          BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_intf_map_get(unit, &l3IntfMapMode));
        }
        if (l3IntfMapMode) {
           /* don't use default if pre-configuration exists */
           if (!SHR_BITGET(vlan_info[unit].pre_cfg_bmp.w,vid)) {
               /* Disable 1:1 mapping */
               if (soc_feature(unit, soc_feature_l3_iif_under_4k)) {
                   /* use zero as invalid in case of IIF entries less than 4k */
#if defined(BCM_HURRICANE4_SUPPORT)
                   if (soc_feature(unit, soc_feature_vlan_default_l3_iif_as_one)) {
                       soc_mem_field32_set(unit, vlan_mem, entry, L3_IIFf, 1);
                   } else
#endif
                   {
                       soc_mem_field32_set(unit, vlan_mem, entry, L3_IIFf, 0);
                   }
               } else {
                   soc_mem_field32_set(unit, vlan_mem, entry, L3_IIFf,
                                   BCM_L3_INGRESS_INTERFACE_INVALID);
               }
           }
        } else {
            if ((soc_feature(unit, soc_feature_l3_iif_under_4k)) &&
                    (soc_mem_field32_fit(unit, vlan_mem, L3_IIFf, vid) != SOC_E_NONE)) {
                /* use zero in case of IIF entries less than 4k and does not fit */
                soc_mem_field32_set(unit, vlan_mem, entry, L3_IIFf, 0);
            }else {
                soc_mem_field32_set(unit, vlan_mem, entry, L3_IIFf, vid);
            }
        }

        if (SOC_MEM_FIELD_VALID(unit, vlan_mem, MPLS_ENABLEf) &&
                soc_feature(unit, soc_feature_mpls)) {
            soc_mem_field32_set(unit, vlan_mem, entry, MPLS_ENABLEf, 0x1);
        }

        if (SOC_MEM_FIELD_VALID(unit, vlan_mem, MIM_TERM_ENABLEf) &&
                soc_feature(unit, soc_feature_mim)) {
            soc_mem_field32_set(unit, vlan_mem, entry, MIM_TERM_ENABLEf, 0x1);
        }
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, vlan_mem, MEM_BLOCK_ALL, vid, entry));
    }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
        vlan_attrs_1_entry_t vlan_attrs;
        sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs_1_entry_t));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_ATTRS_1m, MEM_BLOCK_ANY, vid, &vlan_attrs));

        soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, VALIDf, 0x1);

        if (soc_feature(unit, soc_feature_mpls)) {
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, MPLS_ENABLEf, 0x1);
        }
        if (soc_feature(unit, soc_feature_mim)) {
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, MIM_TERM_ENABLEf, 0x1);
        }
        if (soc_feature(unit, soc_feature_vlan_filter)) {
            
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, EN_IFILTERf, 0x1);
        }
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, VLAN_ATTRS_1m, MEM_BLOCK_ALL, vid, &vlan_attrs));
    }
#endif

    /* Must call on EGR_VLANm before VLAN_TABm on Firebolt 2 */
    if (SOC_IS_FBX(unit) && SOC_MEM_IS_VALID(unit, EGR_VLANm)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_create(unit, vid, EGR_VLANm));
    }
    BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_create(unit, vid, VLAN_TABm));
    if (SOC_MEM_IS_VALID(unit, VLAN_2_TABm)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_create(unit, vid, VLAN_2_TABm));
    }
    return BCM_E_NONE;
}

int
bcm_xgs3_vlan_destroy(int unit, bcm_vlan_t vid)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_vlan_vfi_untag_destroy(unit, vid));
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN
            (bcm_td2p_vlan_vpn_membership_delete(unit, vid));
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
         vlan_mpls_entry_t vlan_mpls;

         sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
         BCM_IF_ERROR_RETURN
              (soc_mem_write(unit, VLAN_MPLSm, MEM_BLOCK_ALL, vid, &vlan_mpls));
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
        vlan_attrs_1_entry_t vlan_attrs;

         sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs_1_entry_t));
         BCM_IF_ERROR_RETURN
              (soc_mem_write(unit, VLAN_ATTRS_1m, MEM_BLOCK_ALL, vid, &vlan_attrs));
    }
#endif
    /* Must call on EGR_VLANm before VLAN_TABm on Firebolt 2 */
    if (((SOC_IS_FBX(unit) && !SOC_IS_KATANA2(unit)) || (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM))
           && (SOC_MEM_IS_VALID(unit, EGR_VLANm))) {
 /* VLAN table in IP and EGR_VLAN table in EP are overlay memories and share the same physical memory.
    In BCMSIM overlay of  memories is not possible, so need to explicity delete*/
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_destroy(unit, vid, EGR_VLANm));
    }
    if (SOC_MEM_IS_VALID(unit, VLAN_2_TABm)) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_vlan_table_destroy(unit, vid, VLAN_2_TABm));
    }
    return _bcm_xgs3_vlan_table_destroy(unit, vid, VLAN_TABm);
}

int
bcm_xgs3_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                       pbmp_t ing_pbmp)
{
    if (SOC_IS_FBX(unit)) {
        /* Check for the presence of 'ING_PORT_BITMAPf', this could either
           reside in VLAN_TABm or in the 'ING_VLAN_VFI_MEMBERSHIPm' if
           'soc_feature_vlan_vfi_membership' is true */
        if (!soc_mem_field_valid(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf) &&
            !(soc_feature(unit, soc_feature_vlan_vfi_membership))) {
            BCM_PBMP_OR(pbmp, ing_pbmp);
        }
        if (SOC_MEM_IS_VALID(unit, EGR_VLANm) && (vid < BCM_VLAN_INVALID)) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_vlan_table_port_add(unit, vid, pbmp, ubmp, ing_pbmp,
                                               EGR_VLANm));
        }
    } else {
        SOC_PBMP_OR(pbmp, ing_pbmp);
    }

    if (vid < BCM_VLAN_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_port_add(unit, vid, pbmp, ubmp,
                        ing_pbmp, VLAN_TABLE(unit)));
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_vlan_vfi_untag_add(unit, vid, pbmp, ubmp));
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_set(
                                    unit, vid, TRUE, TRUE, pbmp));
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_set(
                                unit, vid, FALSE, TRUE, ing_pbmp));
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    return BCM_E_NONE;
}

int
bcm_xgs3_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp,
                          pbmp_t ing_pbmp)
{
    if (SOC_IS_FBX(unit) && SOC_MEM_IS_VALID(unit, EGR_VLANm) &&
        (vid < BCM_VLAN_INVALID)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_port_remove(unit, vid, pbmp, ubmp,
                                              ing_pbmp, EGR_VLANm));
    }

    if (vid < BCM_VLAN_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_port_remove(unit, vid, pbmp, ubmp,
                               ing_pbmp, VLAN_TABLE(unit)));
    }
		
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_vlan_vfi_untag_delete(unit, vid, ubmp));
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_set(
                                    unit, vid, TRUE, FALSE, pbmp));
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_set(
                                unit, vid, FALSE, FALSE, ing_pbmp));
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    return BCM_E_NONE;
}

int
bcm_xgs3_vlan_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp, pbmp_t *ubmp,
                       pbmp_t *ing_pbmp)
{
    pbmp_t local_pbmp, local_ing_pbmp;
    BCM_PBMP_CLEAR(local_pbmp);
    BCM_PBMP_CLEAR(local_ing_pbmp);
    if (SOC_IS_FBX(unit)) {
        if (vid < BCM_VLAN_INVALID) {
            if (SOC_MEM_IS_VALID(unit, EGR_VLANm)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_xgs3_vlan_table_port_get(unit, vid, &local_pbmp, ubmp, NULL,
                                                   EGR_VLANm));
            }
            /* coverity[result_independent_of_operands] */
            if (!TABLE_HAS_T_BITMAP(unit, EGR_VLANm)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_xgs3_vlan_table_port_get(unit, vid, &local_pbmp, NULL,
                                                   NULL, VLAN_TABLE(unit)));
            }

            if (soc_mem_field_valid(unit, VLAN_TABLE(unit), ING_PORT_BITMAPf)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_xgs3_vlan_table_port_get(unit, vid, NULL, NULL,
                                                   &local_ing_pbmp, VLAN_TABLE(unit)));
            } else {
                BCM_PBMP_ASSIGN(local_ing_pbmp, local_pbmp);
            }
        }
    } else {
        if (vid < BCM_VLAN_INVALID) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_vlan_table_port_get(unit, vid, &local_pbmp, ubmp, NULL,
                                               VLAN_TABLE(unit)));
                BCM_PBMP_ASSIGN(local_ing_pbmp, local_pbmp);
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_vfi_untag_profile)) {
        BCM_IF_ERROR_RETURN
            (bcm_td3_vlan_table_ut_port_get(unit, vid, ubmp));
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_get(
                                    unit, vid, TRUE, &local_pbmp));
        BCM_IF_ERROR_RETURN(bcm_td2p_vlan_vfi_mbrship_port_get(
                                unit, vid, FALSE, &local_ing_pbmp));
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    if (pbmp != NULL) {
        BCM_PBMP_ASSIGN(*pbmp, local_pbmp);
    }
    if (ing_pbmp != NULL) {
        BCM_PBMP_ASSIGN(*ing_pbmp, local_ing_pbmp);
    }

    return BCM_E_NONE;
}

int
bcm_xgs3_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg)
{
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vlan_vfi_membership)) {
        BCM_IF_ERROR_RETURN(
            bcm_td2p_vp_group_vlan_vpn_stg_set(unit, vid, TRUE, stg));

        return bcm_td2p_vp_group_vlan_vpn_stg_set(unit, vid, FALSE, stg);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    if (SOC_IS_FBX(unit) && SOC_MEM_IS_VALID(unit, EGR_VLANm)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_vlan_table_stg_set(unit, vid, stg, EGR_VLANm));
    }

    return _bcm_xgs3_vlan_table_stg_set(unit, vid, stg, VLAN_TABLE(unit));
}

int
bcm_xgs3_vlan_reload(int unit, vbmp_t *bmp, int *count)
{
    int rv = BCM_E_NONE;

    if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_ctrl)) {
            BCM_IF_ERROR_RETURN(_vlan_profile_init(unit));
        }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (soc_feature(unit, soc_feature_vlan_pbmp_profile_mgmt)) {
            BCM_IF_ERROR_RETURN(_bcm_th3_vlan_pbmp_profile_init(unit));
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

        if (SOC_MEM_IS_VALID(unit, EGR_VLANm)) {
            rv = _bcm_xgs3_vlan_table_reload(unit, bmp, count, EGR_VLANm);
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_xgs3_vlan_table_reload(unit, bmp, count, VLAN_TABm);
    }
    if (BCM_SUCCESS(rv) && SOC_MEM_IS_VALID(unit, VLAN_2_TABm)) {
        rv = _bcm_xgs3_vlan_table_reload(unit, bmp, count, VLAN_2_TABm);
    }

    return rv;
}

/*
 * Function :
 *    _bcm_xgs3_vlan_mcast_flood_set
 * 
 * Purpose  :
 *    Set multicast flood mode. 
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     vlan  - (IN) Vlan id. 
 *     mode  - (IN) Vlan flood mode.
 *
 * Return :
 *    BCM_E_XXX
 */
int _bcm_xgs3_vlan_mcast_flood_set(int unit,
                            bcm_vlan_t vlan,
                            bcm_vlan_mcast_flood_t mode)
{

    vlan_tab_entry_t    vt; 
    int                 rv;
    int                 pfm;

    /* Upper layer already checks that vid is valid */

    soc_mem_lock(unit, VLAN_TABm);

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int) vlan, &vt);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

    if ((rv =_bcm_vlan_valid_check(unit, VLAN_TABm, &vt, vlan)) 
        == BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        int                  profile_ptr;
        _vlan_profile_t      vpe;

        rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                                   (uint32 *)&vt, vlan, &profile_ptr);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        sal_memcpy(&vpe, &VLAN_PROFILE_ENTRY(unit, profile_ptr),  
                   sizeof(_vlan_profile_t));
        vpe.ip6_mcast_flood_mode = mode;
        vpe.ip4_mcast_flood_mode = mode;
        vpe.l2_mcast_flood_mode = mode;

        rv = _vlan_profile_update(unit, vlan, &vpe);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    {
        /*
         * Update the multicast flood setting for this VLAN.
         */
        rv = _bcm_vlan_mcast_flood_mode_to_pfm(unit, mode, &pfm);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }
        soc_mem_field32_set(unit, VLAN_TABm, &vt, PFMf, pfm);
        rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int) vlan, &vt);
    }

    soc_mem_unlock(unit, VLAN_TABm);

    if (TABLE_HAS_PFM(unit, EGR_VLANm)) {
        soc_mem_lock(unit, EGR_VLANm);
        if ((rv = soc_mem_read(unit, EGR_VLANm,
                               MEM_BLOCK_ANY, (int) vlan, &vt)) < 0) {
            soc_mem_unlock(unit, EGR_VLANm);
            return rv;
        }
        soc_mem_field32_set(unit, EGR_VLANm, &vt, PFMf, mode);
        rv = soc_mem_write(unit, EGR_VLANm, MEM_BLOCK_ALL, (int) vlan, &vt);
        soc_mem_unlock(unit, EGR_VLANm);
    }

    return rv;
}

/*
 * Function :
 *    _bcm_xgs3_vlan_mcast_flood_get
 * 
 * Purpose  :
 *    Get multicast flood mode. 
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     vlan  - (IN) Vlan id. 
 *     mode  - (OUT) Vlan flood mode.
 *
 * Return :
 *    BCM_E_XXX
 */
int _bcm_xgs3_vlan_mcast_flood_get(int unit,
                            bcm_vlan_t vlan,
                            bcm_vlan_mcast_flood_t *mode)
{
    vlan_tab_entry_t            vt;
    int                         rv;

    /* Upper layer already checks that vid is valid */

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int) vlan, &vt);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(_bcm_vlan_valid_check(unit, VLAN_TABm, &vt, vlan));

    /*
     * Get the multicast flood setting for this VLAN.
     */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        int index;
        _vlan_profile_t *vpe;

        rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                                   (uint32 *)&vt, vlan, &index);
        BCM_IF_ERROR_RETURN(rv);

        vpe   = &VLAN_PROFILE_ENTRY(unit, index);
        *mode = vpe->l2_mcast_flood_mode;
    } else 
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, PFMf)) {
        *mode = soc_mem_field32_get(unit, VLAN_TABm, &vt, PFMf);
        } else {
            *mode = 0;
        }
    }
    rv = _bcm_vlan_mcast_pfm_to_flood_mode(unit, *mode, mode);
    return rv;
}
#if defined (BCM_TRX_SUPPORT)
/*
 * Function :
 *    _vlan_control_trx_forwarding_mode_set
 * 
 * Purpose  :
 *    Set cross - connect entry type.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     buf   - (IN) Vlan table entry buffer.
 *     type  - (IN) Entry type.
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_control_trx_forwarding_mode_set(int unit, uint32 *buf, 
                                      bcm_vlan_forward_t type) 
{
    uint32 value;

    /* Input parameters check. */
    if (NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (type) {
      case bcmVlanForwardBridging:
          value = 0;
          break;
      case bcmVlanForwardSingleCrossConnect:
          value = 1;
          break;
      case bcmVlanForwardDoubleCrossConnect:
          value = 2;
          break;
      default: 
          return (BCM_E_PARAM);
    }
    soc_mem_field32_set(unit, VLAN_TABm, buf, L2_ENTRY_KEY_TYPEf, value);
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _vlan_control_trx_forwarding_mode_get
 * 
 * Purpose  :
 *    Get cross - connect entry type.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     buf   - (IN) Vlan table entry buffer.
 *     type  - (OUT) Entry type.
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_control_trx_forwarding_mode_get(int unit, uint32 *buf, 
                                      bcm_vlan_forward_t *type) 
{
    uint32 value;

    /* Input parameters check. */
    if ((NULL == buf) || (NULL == type)) {
        return (BCM_E_PARAM);
    }
    value = soc_mem_field32_get(unit, VLAN_TABm, buf, L2_ENTRY_KEY_TYPEf);
    switch (value) {
      case 0x2:
          *type = bcmVlanForwardDoubleCrossConnect;
          break;
      case 0x1:
          *type  = bcmVlanForwardSingleCrossConnect;
          break;
      default:
          *type = bcmVlanForwardBridging;
          break;
    }
    return (BCM_E_NONE);
}
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function :
 *    _vlan_cosq_map_set
 * 
 * Purpose  :
 *    Set per vlan cosq number.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     vid   - (IN) Vlan table entry buffer.
 *     cosq  - (IN) VLAN control structure.
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_cosq_map_set(int unit, bcm_vlan_t vid, bcm_vlan_control_vlan_t *control)
{
    vlan_cos_map_entry_t vcm;
    int rv;
    bcm_cos_queue_t cosq = 0;
    rv = soc_mem_read(unit, VLAN_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &vcm);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field32_set(unit, VLAN_COS_MAPm, &vcm, VALIDf, 
                      ((control->flags & BCM_VLAN_COSQ_ENABLE) ? 1 : 0));

    if (control->flags & BCM_VLAN_COSQ_ENABLE) {

        /* Add 8 since VLAN COS queues are from 8..23 */
        cosq = control->cosq + 8;
        if ((cosq < 8) || (cosq > 23)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, VLAN_COS_MAPm, &vcm, VLAN_COSf, cosq);
    }

    /* Write back updated buffer. */
    rv = soc_mem_write(unit, VLAN_COS_MAPm, MEM_BLOCK_ALL, (int)vid, &vcm);
    return (rv);
}

/*
 * Function :
 *    _vlan_cosq_map_get
 * 
 * Purpose  :
 *    Get per vlan cosq number.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     vid   - (IN) Vlan table entry buffer.
 *     control- (OUT) vlan control structure.
 * Return :
 *    BCM_E_XXX
 */
STATIC int
_vlan_cosq_map_get(int unit, bcm_vlan_t vid, bcm_vlan_control_vlan_t *control) 
{
    vlan_cos_map_entry_t vcm;
    int rv;

    rv = soc_mem_read(unit, VLAN_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &vcm);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == soc_mem_field32_get(unit, VLAN_COS_MAPm, &vcm, VALIDf)) {
        return (BCM_E_NONE);
    }

    control->flags |= BCM_VLAN_COSQ_ENABLE;
    /* Subtract 8 since VLAN COS queues are from 8..23 */
    control->cosq = soc_mem_field32_get(unit, VLAN_COS_MAPm, &vcm, VLAN_COSf) - 8;
    return (BCM_E_NONE);
}
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(INCLUDE_L3)
/*
 * Function :
 *     _bcm_xgs3_vlan_control_vpn_get
 *
 * Purpose  :
 *     Reads VFI table and fills vlan_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (OUT) Configuration structure.
 *
 * Return :
 *    BCM_E_XXX
 */
 int
 _bcm_xgs3_vlan_control_vpn_get(int unit, bcm_vlan_t vid, uint32 valid_fields,
                                bcm_vlan_control_vlan_t *control)
{
    int rv;
    int vfi;
    int value;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    /* Read the VFI memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    if (!SOC_SUCCESS(rv)) {
        return rv;
    }

    /* Get the CLASS_ID */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
        if (soc_mem_field_valid(unit, mem, CLASS_IDf)) {
            value = soc_mem_field32_get(unit, mem, &vfi_entry, CLASS_IDf);
            control->if_class = value;
            control->flags |= (control->if_class) ? BCM_VLAN_L2_CLASS_ID_ONLY : 0;
        }
    }

    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

uint32
_bcm_xgs3_vlan_mcast_idx_get(int unit, const void *entbuf, soc_field_t field)
{
    uint32 fval = 0;
    uint32 dest_type = 0;
    soc_mem_t vtab = VLAN_TABm;
    int fidx = 0;
    soc_field_t idxfld;

    soc_field_t idx_f[3] = {
        BC_IDXf,
        UMC_IDXf,
        UUC_IDXf
    };
    soc_field_t dst_idx_f[3] = {
        BC_DESTINATIONf,
        UMC_DESTINATIONf,
        UUC_DESTINATIONf
    };

    switch (field) {
        case BC_IDXf:
            fidx = 0;
            break;
        case UMC_IDXf:
            fidx = 1;
            break;
        case UUC_IDXf:
            fidx = 2;
            break;
        default:
            return fval;
            break;
    }
    if (soc_feature(unit, soc_feature_generic_dest)) {
        idxfld = dst_idx_f[fidx];
    } else {
        idxfld = idx_f[fidx];
    }

    if (SOC_MEM_FIELD_VALID(unit, vtab, idxfld)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            fval = soc_mem_field32_dest_get(unit, vtab, entbuf, idxfld,
                                            &dest_type);
            if (dest_type != SOC_MEM_FIF_DEST_IPMC) {
                fval = 0;
            }
        } else {
            fval = soc_mem_field32_get(unit, vtab, entbuf, idxfld);
        }
    }

    return fval;
}

void
_bcm_xgs3_vlan_mcast_idx_set(int unit, void *entbuf, soc_field_t field,
                             uint32 fval)
{
    soc_mem_t vtab = VLAN_TABm;
    int fidx = 0;
    soc_field_t idxfld;

    soc_field_t idx_f[3] = {
        BC_IDXf,
        UMC_IDXf,
        UUC_IDXf
    };
    soc_field_t dst_idx_f[3] = {
        BC_DESTINATIONf,
        UMC_DESTINATIONf,
        UUC_DESTINATIONf
    };

    switch (field) {
        case BC_IDXf:
            fidx = 0;
            break;
        case UMC_IDXf:
            fidx = 1;
            break;
        case UUC_IDXf:
            fidx = 2;
            break;
        default:
            break;
    }
    if (soc_feature(unit, soc_feature_generic_dest)) {
        idxfld = dst_idx_f[fidx];
    } else {
        idxfld = idx_f[fidx];
    }

    if (SOC_MEM_FIELD_VALID(unit, vtab, idxfld)) {
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, vtab, entbuf, idxfld,
                                     SOC_MEM_FIF_DEST_IPMC, fval);
        } else {
            soc_mem_field32_set(unit, vtab, entbuf, idxfld, fval);
        }
    }

    return ;
}

/*
 * Function :
 *    _bcm_xgs3_vlan_control_vlan_get
 * 
 * Purpose  :
 *    Get vlan specific traffic forwarding parameters
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id. 
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) Configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_xgs3_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                          uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int  rv = BCM_E_UNAVAIL;

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
    int                          profile_index;
    vlan_tab_entry_t             vt;
    _vlan_profile_t              *vpt;
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    int enable;
    int  ingress_map_mode=0;
    _bcm_l3_ingress_intf_t iif;
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
    int pre_cfg = FALSE;

    if (!soc_feature(unit, soc_feature_vlan_ctrl)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(control, 0, sizeof(bcm_vlan_control_vlan_t));

#if (defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
     defined(BCM_GREYHOUND2_SUPPORT)) && defined(INCLUDE_L3)
    if (_BCM_VPN_VFI_IS_SET(vid)) {
        if (soc_feature(unit, soc_feature_vfi_profile)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            return bcmi_td2p_vlan_control_vpn_get(
                    unit, vid, valid_fields, control);
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        } else {
#ifdef BCM_GREYHOUND2_SUPPORT
            if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
                return bcmi_gh2_vlan_control_vpn_get(unit, vid, valid_fields,
                                                     control);
            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
            {
                return _bcm_xgs3_vlan_control_vpn_get(unit, vid, valid_fields,
                                                      control);
            }
        }
    }
#endif /* (TRIDENT2PLUS || TOMAHAWK || GREYHOUND2) && INCLUDE_L3 */

    /* check if pre-configuration exists before vlan is created*/
    if (!SHR_BITGET(vlan_info[unit].bmp.w, vid)) {
        if (!SHR_BITGET(vlan_info[unit].pre_cfg_bmp.w,vid)) {
            return BCM_E_PARAM;
        } else {
            pre_cfg = TRUE;
        }
    }

    soc_mem_lock(unit, VLAN_TABm);
    /* Don't need to lock the cached outer TPID table.
     * because the TPID entries referenced by the VLAN_TAB cannot
     * be removed if the VLAN_TAB is lock.
     */

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vt);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
        rv = bcm_xgs3_l3_ingress_intf_map_get(unit, &ingress_map_mode);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT  && INCLUDE_L3 */

    /* Get VRF */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_VRF_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VRF_IDf)) {
           control->vrf = soc_mem_field32_get(unit, VLAN_TABm, &vt, VRF_IDf);
        }
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
        else if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_l3) && 
                soc_feature(unit, soc_feature_l3_ingress_interface) && 
                SOC_MEM_IS_VALID(unit,L3_IIFm)) {
            if (soc_feature(unit, soc_feature_no_vlan_vrf)) {
                /* 
                 * Return E_UNAVAIL for those device can't support VID 1:1 
                 * mapping to L3_IIFm to set VLAN based VRF_ID
                 */
#if defined(BCM_GREYHOUND2_SUPPORT)
                if (SOC_IS_GREYHOUND2(unit)) {
                    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L3_IIFf)) {
                        if (soc_mem_field32_get(unit, VLAN_TABm, &vt, L3_IIFf)
                            && (ingress_map_mode)) {
                            iif.intf_id = soc_mem_field32_get(unit, VLAN_TABm,
                                                              &vt, L3_IIFf);
                            rv = _bcm_tr_l3_ingress_interface_get(unit, NULL,
                                                                  &iif);
                            if (BCM_FAILURE(rv)) {
                                soc_mem_unlock(unit, VLAN_TABm);
                                return rv;
                            }
                            control->vrf = iif.vrf;
                        }
                    }
                } else
#endif /* BCM_GREYHOUND2_SUPPORT */
                {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return BCM_E_UNAVAIL;
                }
            } else if (!ingress_map_mode) {
            rv = _bcm_tr_l3_intf_vrf_get(unit, vid, &control->vrf);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
        }
        }
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
    }

#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3) && 
            soc_feature(unit, soc_feature_l3_ingress_interface) &&
            (!ingress_map_mode)) {
        if (SOC_MEM_FIELD_VALID(unit, L3_IIFm, ALLOW_GLOBAL_ROUTEf) ||
            SOC_IS_TD2_TT2(unit)) {
            rv =  _bcm_tr_l3_intf_global_route_enable_get(unit, vid, &enable);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (0 == enable) {
                control->flags |= BCM_VLAN_L3_VRF_GLOBAL_DISABLE;
            }
        }
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L3_IIFf) && 
                soc_feature(unit, soc_feature_l3_ingress_interface) && 
                soc_feature(unit, soc_feature_l3)) {

                /* Get Vlan mapping */
                control->ingress_if = soc_mem_field32_get(unit,
                           VLAN_TABm, &vt, L3_IIFf);
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRIUMPH2_SUPPORT) && defined (INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3) 
            && soc_feature(unit, soc_feature_l3_ingress_interface) 
            && (!ingress_map_mode)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_URPF_MODE_MASK) {
            if (SOC_MEM_FIELD_VALID(unit, L3_IIFm, URPF_MODEf) ||
                           soc_feature(unit, soc_feature_l3_iif_profile)) {
                rv = _bcm_tr2_l3_intf_urpf_mode_get(unit, vid, &control->urpf_mode);
                if (BCM_FAILURE(rv)) {
                   soc_mem_unlock(unit, VLAN_TABm);
                   return rv;
                }
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, L3_IIFm, URPF_DEFAULTROUTECHECKf) ||
                   soc_feature(unit, soc_feature_l3_iif_profile)) {
            rv =  _bcm_tr2_l3_intf_urpf_default_route_get(unit, vid, &enable);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (0 == enable) {
                control->flags |= BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE;
            }
        }
    }
    if (soc_feature(unit, soc_feature_mpls)) {
        int mpls_enable = 0;
        if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
        vlan_mpls_entry_t vlan_mpls_entry;
        sal_memset(&vlan_mpls_entry, 0, sizeof(vlan_mpls_entry_t));
        rv = soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY, vid,
            &vlan_mpls_entry);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
            mpls_enable = soc_mem_field32_get(unit, VLAN_MPLSm,
                                &vlan_mpls_entry, MPLS_ENABLEf);
            if (0 == mpls_enable) {
                control->flags |= BCM_VLAN_MPLS_DISABLE;
            }
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
            vlan_attrs_1_entry_t vlan_attrs;
            sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs_1_entry_t));
            rv = soc_mem_read(unit, VLAN_ATTRS_1m, MEM_BLOCK_ANY, vid,
                              &vlan_attrs);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            mpls_enable = soc_mem_field32_get(unit, VLAN_ATTRS_1m,
                                              &vlan_attrs, MPLS_ENABLEf);
            if (0 == mpls_enable) {
                control->flags |= BCM_VLAN_MPLS_DISABLE;
            }
        }
#endif
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_filter)) {
        vlan_tab_entry_t vlan_tab;
        sal_memset(&vlan_tab, 0, sizeof(vlan_tab));
        rv = soc_mem_read(unit, EGR_VLANm, MEM_BLOCK_ANY,
                            vid, &vlan_tab);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        if (soc_mem_field32_get(unit, EGR_VLANm,
                                &vlan_tab, EN_EFILTERf) == 0) {
            control->flags2 |= BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE;
        }

        if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
            vlan_attrs_1_entry_t vlan_attrs;
            sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs));
            rv = soc_mem_read(unit, VLAN_ATTRS_1m, MEM_BLOCK_ANY,
                                vid, &vlan_attrs);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }

            if (soc_mem_field32_get(unit, VLAN_ATTRS_1m,
                                    &vlan_attrs, EN_IFILTERf) == 0) {
                control->flags2 |= BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE;
            }
        } else if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
            vlan_mpls_entry_t vlan_mpls;
            sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls));
            rv = soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY,
                                vid, &vlan_mpls);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }

            if (soc_mem_field32_get(unit, VLAN_MPLSm,
                                    &vlan_mpls, EN_IFILTERf) == 0) {
                control->flags2 |= BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE;
            }
        }
    }
#endif
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRIDENT2_SUPPORT) && defined (INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3_iif_profile) && (!ingress_map_mode)) {
        rv = _bcm_td2_l3_intf_iif_profile_get(unit, vid, control);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
    }

#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VLAN_CLASS_IDf)) {
            control->if_class =
                soc_mem_field32_get(unit, VLAN_TABm, &vt, VLAN_CLASS_IDf);
                control->flags |= BCM_VLAN_L2_CLASS_ID_ONLY;
        }
    }
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L3_IF_CLASS_MASK) {
        if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
            if (!ingress_map_mode) {
                iif.intf_id = vid;
                rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
                control->l3_if_class = iif.if_class;
                control->flags |= BCM_VLAN_L3_CLASS_ID;
            }
        }
    }
#endif  /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L2_ENTRY_KEY_TYPEf)) {
            rv = _vlan_control_trx_forwarding_mode_get(unit, (uint32 *)&vt,
                                                       &control->forwarding_mode);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_wirelss_traffic_select)) {
        rv = soc_mem_field32_get(unit, VLAN_TABm, &vt, WIRED_WIRELESSf);
        if(rv == 0x1) {
            control->flags2 |= BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT;
        }
        if(rv == 0x2) {
            control->flags2 |= BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT;
        }
    }
#endif
#if defined(BCM_TRX_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, FID_IDf)) {
            control->forwarding_vlan = soc_mem_field32_get(unit, VLAN_TABm,
                                                           &vt, FID_IDf);
        }
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        if(soc_feature(unit, soc_feature_l3)) {
            if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
                vlan_mpls_entry_t vlan_mpls;

                sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
                rv = soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY, vid,
                                  &vlan_mpls);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
                /* Get Vlan mapping */
                control->ingress_if = soc_mem_field32_get(unit, VLAN_MPLSm,
                        &vlan_mpls, L3_IIFf);

#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_METROLITE_SUPPORT)
                if ((SOC_IS_MONTEREY(unit) || SOC_IS_METROLITE(unit)) &&
                    soc_feature(unit, soc_feature_l3_ingress_interface) &&
                    !soc_feature(unit, soc_feature_no_vlan_vrf) &&
                    (valid_fields & BCM_VLAN_CONTROL_VLAN_VRF_MASK) &&
                    (ingress_map_mode)) {

                    iif.intf_id = control->ingress_if;

                    rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }

                    control->vrf = iif.vrf;
                }
#endif
#if defined(BCM_MONTEREY_SUPPORT)
                if ((SOC_IS_MONTEREY(unit) &&
                    (ingress_map_mode) &&
                    (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK))) {
                    BCM_IF_ERROR_RETURN(
                            _bcm_td2_l3_intf_iif_profile_get(unit, control->ingress_if, control));
                }
#endif /* BCM_MONTEREY_SUPPORT */
            }
#ifdef BCM_HURRICANE4_SUPPORT
            else if (SOC_IS_HURRICANE4(unit) &&
                     soc_feature(unit, soc_feature_l3_ingress_interface) &&
                     !soc_feature(unit, soc_feature_no_vlan_vrf) &&
                     SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L3_IIFf) &&
                     (ingress_map_mode)) {

                if (valid_fields & BCM_VLAN_CONTROL_VLAN_VRF_MASK) {

                    /* Get Vlan mapping */
                    control->ingress_if = soc_mem_field32_get(unit,
                                                              VLAN_TABm, &vt,
                                                              L3_IIFf);
                    iif.intf_id = control->ingress_if;

                    rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }

                    control->vrf = iif.vrf;
                }

                if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
                    BCM_IF_ERROR_RETURN(
                            _bcm_td2_l3_intf_iif_profile_get(unit,
                                                             control->ingress_if,
                                                             control));
                }
            }
#endif
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
        int idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vt, BC_IDXf);
        if (idx) {
            rv = _bcm_tr_multicast_ipmc_group_type_get(unit, idx,
                    &control->broadcast_group);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            }
        }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
        int idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vt, UMC_IDXf);
        if (idx) {
            rv = _bcm_tr_multicast_ipmc_group_type_get(unit, idx,
                    &control->unknown_multicast_group);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            }
        }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
        int idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vt, UUC_IDXf);
        if (idx) {
            rv = _bcm_tr_multicast_ipmc_group_type_get(unit, idx,
                    &control->unknown_unicast_group);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_trill)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_TRILL_NONUNICAST_GROUP_MASK) {
            if (soc_mem_field32_get(unit, VLAN_TABm,
                        &vt, TRILL_DOMAIN_NONUC_REPL_INDEXf)) {
                _BCM_MULTICAST_GROUP_SET(control->trill_nonunicast_group,
                    _BCM_MULTICAST_TYPE_TRILL,
                    soc_mem_field32_get(unit, VLAN_TABm,
                        &vt, TRILL_DOMAIN_NONUC_REPL_INDEXf));
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                    TRILL_TRANSIT_IGMP_MLD_PAYLOAD_TO_CPUf)) {
            if ( !soc_mem_field32_get(unit, VLAN_TABm, &vt,
                        TRILL_TRANSIT_IGMP_MLD_PAYLOAD_TO_CPUf )) {
                control->flags |= BCM_VLAN_TRILL_IGMP_SNOOP_DISABLE;
            }
        }
        /* Get per-Vlan trill_name */
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_SOURCE_TRILL_NAME_MASK) {
            if (soc_mem_field_valid (unit, VLAN_TABm, TRILL_RBRIDGE_NICKNAME_INDEXf)) {
                rv = bcm_td_trill_source_trill_idx_name_get(unit,
                                  soc_mem_field32_get(unit, VLAN_TABm,
                                  &vt, TRILL_RBRIDGE_NICKNAME_INDEXf),
                                  &control->source_trill_name);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_VP_MCAST_MASK) {
        if (SHR_BITGET(vlan_info[unit].vp_mode, vid)) {
            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf) &&
                soc_mem_field32_get(unit, VLAN_TABm, &vt, VIRTUAL_PORT_ENf)) {
                control->vp_mc_ctrl = bcmVlanVPMcControlEnable;
            } else {
                control->vp_mc_ctrl = bcmVlanVPMcControlDisable;
            }
        } else {
            control->vp_mc_ctrl = bcmVlanVPMcControlAuto;
        }
    }
#endif

    if (pre_cfg == FALSE) {
        /* after vlan is created */

        rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                      (uint32 *)&vt,  vid, &profile_index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return (rv);
        }
    } else {
#if defined(BCM_EASY_RELOAD_SUPPORT)
        if (SOC_IS_RELOADING(unit)) {
            /*
             * Use VLAN_PROFILE_ER() as a write-through cache
             * for VLAN profile pointers during easy reload
             */
            profile_index = VLAN_PROFILE_PTR_ER(unit, vid);
        } else
#endif
        {
            profile_index = soc_mem_field32_get(unit, VLAN_TABm, &vt,
                                       VLAN_PROFILE_PTRf);
        }
    }

    vpt = &VLAN_PROFILE_ENTRY(unit, profile_index);
    if (SOC_IS_TRIDENT3X(unit)) {
        /* always use the result from vlan_attrs_1 for this flag */
        control->flags |= ((vpt->flags) & (~BCM_VLAN_MPLS_DISABLE));
    } else {
        control->flags |= vpt->flags;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        control->outer_tpid = vpt->outer_tpid;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK) {
        control->ip6_mcast_flood_mode = vpt->ip6_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK) {
        control->ip4_mcast_flood_mode = vpt->ip4_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK) {
        control->l2_mcast_flood_mode = vpt->l2_mcast_flood_mode;
    }
#if defined(BCM_TRX_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK) {
        if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)
            && SOC_MEM_FIELD_VALID (unit, VLAN_PROFILE_TABm,
                                       PROTOCOL_PKT_INDEXf)) {
            sal_memcpy(&control->protocol_pkt,
                &vpt->protocol_pkt, sizeof(bcm_vlan_protocol_packet_ctrl_t));
        }
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, USE_VLAN_CMLf)) {
        if (vpt->learn_flags & BCM_VLAN_LEARN_CONTROL_ENABLE) {
            control->learn_flags = vpt->learn_flags;
        } else {
            control->learn_flags = 0x0;
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_COSQ_MASK) {
        if (SOC_MEM_IS_VALID(unit, VLAN_COS_MAPm)) {
#if defined(BCM_KATANA_SUPPORT)
            if (SOC_IS_KATANAX(unit)) {
                rv = _vlan_kt_cosq_map_get(unit, vid, control);
            } else
#endif
            {
                rv = _vlan_cosq_map_get(unit, vid, control);
            }
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }
        }
    }
#endif

    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                            ENABLE_IGMP_MLD_SNOOPINGf)) {
        if (0 == soc_mem_field32_get(unit, VLAN_TABm, &vt,
                                     ENABLE_IGMP_MLD_SNOOPINGf)) {
            control->flags |= BCM_VLAN_IGMP_SNOOP_DISABLE;
        }
    }

#if defined(INCLUDE_L3) && defined(BCM_TRIDENT2_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_NAT_REALM_ID_MASK) {
        if ((soc_feature(unit, soc_feature_nat) ||
            soc_feature(unit, soc_feature_large_scale_nat)) &&
            SOC_MEM_IS_VALID(unit, L3_IIFm) &&
            SOC_MEM_FIELD_VALID(unit, L3_IIFm, SRC_REALM_IDf) &&
            (!ingress_map_mode)) {
            rv = _bcm_tr_l3_intf_nat_realm_id_get(unit, vid,
                                &control->nat_realm_id);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }
        }
    }
#endif /* INCLUDE_L3  && BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vfi_from_vlan_tables)) {
            int vfi_index = -1;
        if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm) &&
            SOC_MEM_FIELD_VALID(unit, VLAN_MPLSm, VFIf)) {
            vlan_mpls_entry_t vlan_mpls;

            sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
            soc_mem_lock(unit, VLAN_MPLSm);
            rv = READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vid, &vlan_mpls);
            soc_mem_unlock(unit, VLAN_MPLSm);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            vfi_index = soc_mem_field32_get(unit, VLAN_MPLSm,
                                            &vlan_mpls, VFIf);
        }
#if defined(BCM_TRIDENT3_SUPPORT)
        else if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VFIf)) {
            vfi_index = soc_mem_field32_get(unit, VLAN_TABm,
                                            &vt, VFIf);
        }
#endif/* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */
            _BCM_VPN_SET(control->vpn, _BCM_VPN_TYPE_VFI, vfi_index);
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3*/

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_SR_FLAGS_MASK) {
            if (soc_mem_field32_get(unit, VLAN_TABm, &vt, SR_ENABLEf)){
                control->sr_flags |= BCM_VLAN_CONTROL_SR_FLAG_ENABLE;
                if (soc_mem_field32_get(unit, VLAN_TABm, &vt, SR_LAN_IDf)){
                    control->sr_flags |= BCM_VLAN_CONTROL_SR_FLAG_LAN_ID;
                }
            }
        }
   }
#endif /* BCM_TSN_SR_SUPPORT*/

    soc_mem_unlock(unit, VLAN_TABm);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    return rv;
}

#if defined (BCM_TRX_SUPPORT)
STATIC int _vlan_protocol_pkt_action_validate(int pkt_action)
{
    if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
         (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function :
 *    _bcm_xgs3_protocol_packet_actions_validate
 *
 * Purpose  :
 *    Validate the protocol packet control actions 
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     pkt     - (IN) protocol packet control actions.
 *
 * Return :
 *    BCM_E_XXX
 */
int _bcm_xgs3_protocol_packet_actions_validate(int unit, 
                bcm_vlan_protocol_packet_ctrl_t *pkt)
{
#if defined (BCM_TRX_SUPPORT)
    if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)) {
        int pkt_action;
       
        pkt_action = pkt->mmrp_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    ==BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->srp_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->arp_reply_action;
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
            (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
            return BCM_E_PARAM;
        }
        pkt_action = pkt->arp_request_action;
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
            (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
            return BCM_E_PARAM;
        }
        pkt_action = pkt->nd_action;
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
            (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
            return BCM_E_PARAM;
        }
        pkt_action = pkt->dhcp_action;
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
            (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
            return BCM_E_PARAM;
        }
        
        pkt_action = pkt->igmp_report_leave_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->igmp_query_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->igmp_unknown_msg_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->mld_report_done_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->mld_query_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->ip4_rsvd_mc_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->ip6_rsvd_mc_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->ip4_mc_router_adv_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        pkt_action = pkt->ip6_mc_router_adv_action;
        if (pkt_action) {
            if (_vlan_protocol_pkt_action_validate(pkt_action)
                    == BCM_E_PARAM) {
                return BCM_E_PARAM;
            }
        }
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)

/*
 * Function :
 *    _bcm_xgs3_vlan_control_vpn_set
 *
 * Purpose  :
 *    Configures various controls on the VFI.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) Configuration structure.
 *
 * Return :
 *    BCM_E_XXX
 */

 int
 _bcm_xgs3_vlan_control_vpn_set(int unit, bcm_vlan_t vid, uint32 valid_fields,
                                bcm_vlan_control_vlan_t *control)
{
     int rv;
     int vfi;
     int value;
     vfi_entry_t vfi_entry;
     soc_mem_t mem = VFIm;

     _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

     /* Read the VFI memory */
     MEM_LOCK(unit, mem);
     rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
     if (!SOC_SUCCESS(rv)) {
         MEM_UNLOCK(unit, mem);
         return rv;
     }

     /* Program the VFI table */
     if (control->flags & BCM_VLAN_L2_CLASS_ID_ONLY) {
         if (soc_mem_field_valid(unit, mem, CLASS_IDf)) {
             if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
                 if ((control->if_class < 0) ||
                     (control->if_class > SOC_INTF_CLASS_MAX(unit))) {
                     rv = BCM_E_PARAM;
                 } else {
                     value = control->if_class;
                     soc_mem_field32_set(unit, mem, &vfi_entry, CLASS_IDf, value);
                 }
             } else {
                 rv = BCM_E_PARAM;
             }
         }
     }

     if (!SOC_SUCCESS(rv)) {
         MEM_UNLOCK(unit, mem);
         return rv;
     }

     rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);

     MEM_UNLOCK(unit, mem);
     return rv;
}
#endif /* INCLUDE_L3 */

/*
 * Function :
 *    _bcm_xgs3_vlan_control_vlan_set
 * 
 * Purpose  :
 *    Get vlan specific traffic forwarding parameters
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id. 
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (OUT) Configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_xgs3_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
                          uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int                rv = BCM_E_UNAVAIL;

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    uint16             old_outer_tpid;
    int                tpid_index = -1; /* used also as check flag for cleanup */
    int                profile_index;
    _vlan_profile_t    *old_vpt, vpt;
    vlan_tab_entry_t   vt;
    egr_vlan_entry_t   evt;
    int                disable_mask;
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    int                ingress_map_mode=0;
    int                if_class;
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    int  trill_name_index=-1;
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
    int iif_profile_valid;
    int pre_cfg = FALSE;   /* pre-configuration flag */

    if (!soc_feature(unit, soc_feature_vlan_ctrl)) {
        return (BCM_E_UNAVAIL);
    }

#if (defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
     defined(BCM_GREYHOUND2_SUPPORT)) && defined(INCLUDE_L3)
    if (_BCM_VPN_VFI_IS_SET(vid)) {
        if (soc_feature(unit, soc_feature_vfi_profile)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
            return bcmi_td2p_vlan_control_vpn_set(
                        unit, vid, valid_fields, control);
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        } else {
#ifdef BCM_GREYHOUND2_SUPPORT
            if (soc_feature(unit, soc_feature_vxlan_lite_riot)) {
                return bcmi_gh2_vlan_control_vpn_set(unit, vid, valid_fields,
                                                     control);
            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
            {
                return _bcm_xgs3_vlan_control_vpn_set(unit, vid, valid_fields,
                                                      control);
            }
        }
    }
#endif /* (TRIDENT2PLUS || TOMAHAWK || GREYHOUND2) && INCLUDE_L3*/

    /* check if perform pre-configuration before vlan is created */
    if (!SHR_BITGET(vlan_info[unit].bmp.w, vid)) {
        pre_cfg = TRUE;
    }

#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
        BCM_IF_ERROR_RETURN(bcm_xgs3_l3_ingress_intf_map_get(unit, &ingress_map_mode));
    }
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */

    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit) ||
        SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || 
        SOC_IS_GREYHOUND2(unit)) {
        if(control->flags & ( BCM_VLAN_MPLS_DISABLE |
                              BCM_VLAN_COSQ_ENABLE |
                              BCM_VLAN_IGMP_SNOOP_DISABLE |
                              BCM_VLAN_PIM_SNOOP_DISABLE |
                              BCM_VLAN_USE_FABRIC_DISTRIBUTION |
                              BCM_VLAN_TRILL_IGMP_SNOOP_DISABLE |
                              BCM_VLAN_MIM_TERM_DISABLE)) {
            return BCM_E_PARAM;
        }
    }

    if (!soc_feature(unit, soc_feature_l3_ingress_interface)) {
        if(control->flags & BCM_VLAN_L3_CLASS_ID) {
            return BCM_E_PARAM;
        }
    } else {
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
        /* to check the ingress interface id */
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
            /* VLAN_TABm.L3_IIFm can be assigned for ingress_map_mode=1 */
            if (ingress_map_mode) {
                if (control->ingress_if > soc_mem_index_max(unit, L3_IIFm) ||
                    control->ingress_if < soc_mem_index_min(unit, L3_IIFm) ) {
                    return BCM_E_PARAM;
                }
            }
            /* cannot set l3 class id of vlan if ingress_map_mode=1 */
            if(control->flags & BCM_VLAN_L3_CLASS_ID) {
                if (ingress_map_mode) {
                    return BCM_E_PARAM;
                }
                if (!SOC_MEM_FIELD_VALID(unit, L3_IIFm, CLASS_IDf)) {
                    return BCM_E_PARAM;
                }
            }
        }
#endif
    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    /* TIME-BEING ::: as soc_feature_service_queuing is FALSE for KATANA2 */
    if (soc_feature(unit, soc_feature_service_queuing) || (SOC_IS_KATANA2(unit))) {
        /* vlan cosq supported thru bcm_cosq_classifier_xxx() APIs */
        if (control->flags & BCM_VLAN_COSQ_ENABLE) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_KATANA2_SUPPORT */


    disable_mask = (BCM_VLAN_IP4_DISABLE |
                    BCM_VLAN_IP6_DISABLE |
                    BCM_VLAN_IP4_MCAST_DISABLE |
                    BCM_VLAN_IP6_MCAST_DISABLE |
                    BCM_VLAN_IP4_MCAST_L2_DISABLE |
                    BCM_VLAN_IP6_MCAST_L2_DISABLE);

    iif_profile_valid = FALSE;
#if defined(BCM_TRIDENT2_SUPPORT) && defined (INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3_iif_profile)) {
        iif_profile_valid = TRUE;
        if (!ingress_map_mode) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_l3_intf_iif_profile_update(unit, vid, control));
        }
#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        else if ((SOC_IS_MONTEREY(unit) || SOC_IS_HURRICANE4(unit)) &&
                 (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK)) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_l3_intf_iif_profile_update(unit, control->ingress_if, control));
        }
#endif
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    if (control->flags & disable_mask) {
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
        if (0 == SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, IPV4L3_ENABLEf) &&
            !iif_profile_valid) {
            /* This device does not have per-vlan enable controls */
            return BCM_E_PARAM;
        }
#else /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
        return BCM_E_PARAM;
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
    }

    if ((control->flags & BCM_VLAN_IGMP_SNOOP_DISABLE)
        && !SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                                ENABLE_IGMP_MLD_SNOOPINGf)) {
        return BCM_E_PARAM;
    }


    if (control->flags & (BCM_VLAN_PIM_SNOOP_DISABLE |
                          BCM_VLAN_USE_FABRIC_DISTRIBUTION)) {
        return BCM_E_PARAM;
    }

    if ((control->flags &
         (BCM_VLAN_MPLS_DISABLE | BCM_VLAN_COSQ_ENABLE)) &&
        !SOC_IS_TR_VL(unit)) {
        return BCM_E_PARAM;
    }

    if ((control->flags & BCM_VLAN_MIM_TERM_DISABLE) && !SOC_IS_ENDURO(unit)) {
        return BCM_E_PARAM;
    }

    if (control->forwarding_vlan > BCM_VLAN_MAX) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if ((control->forwarding_mode == bcmVlanForwardSingleCrossConnect) ||
        (control->forwarding_mode == bcmVlanForwardDoubleCrossConnect)) {
        /* No L2 learning for cross-connect */
        if (0 == (control->flags & BCM_VLAN_LEARN_DISABLE)) {
            return (BCM_E_PARAM);
        }
    }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    if ((0 == SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, ICMP_REDIRECT_TOCPUf)) &&
        (!iif_profile_valid) &&
        (control->flags & BCM_VLAN_ICMP_REDIRECT_TOCPU)) {
        return (BCM_E_PARAM);
    }
    if ((control->flags & BCM_VLAN_UNKNOWN_UCAST_TOCPU) &&
        !SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, L2_MISS_TOCPUf)) {
        return BCM_E_PARAM;
    }
    if ((control->flags & BCM_VLAN_UNKNOWN_UCAST_DROP) &&
        !SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, L2_MISS_DROPf)) {
        return BCM_E_PARAM;
    }
    if ((control->flags & BCM_VLAN_NON_UCAST_TOCPU) &&
        !SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm,
                             L2_NON_UCAST_TOCPUf)) {
        return BCM_E_PARAM;
    }
    if ((control->flags & BCM_VLAN_NON_UCAST_DROP) &&
        !SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm,
                             L2_NON_UCAST_DROPf)) {
        return BCM_E_PARAM;
    }
    if (control->if_class < 0 || control->if_class > SOC_INTF_CLASS_MAX(unit)){
        return BCM_E_PARAM;
    }
    if (control->l3_if_class < 0 ||
        control->l3_if_class > SOC_INTF_CLASS_MAX(unit)){
        return BCM_E_PARAM;
    }
#endif /* BCM_TRX_SUPPORT */

    /* validate parameter protocol_pkt */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, PROTOCOL_PKT_INDEXf)) {
#if defined(BCM_TRIDENT_SUPPORT)
            bcm_vlan_protocol_packet_ctrl_t zero_protocol_pkt;
            bcm_vlan_control_vlan_t dft_control;
            sal_memset(&zero_protocol_pkt, 0,
                       sizeof(bcm_vlan_protocol_packet_ctrl_t));
            bcm_vlan_control_vlan_t_init(&dft_control);
#if defined(BCM_TRX_SUPPORT)
            BCM_IF_ERROR_RETURN(_vlan_protocol_pkt_ctrl_default_add
                                             (unit, &dft_control.protocol_pkt));
#endif /*BCM_TRX_SUPPORT*/
            if (sal_memcmp(&zero_protocol_pkt, &control->protocol_pkt,
                sizeof(bcm_vlan_protocol_packet_ctrl_t)) &&
                sal_memcmp(&dft_control.protocol_pkt, &control->protocol_pkt,
                sizeof(bcm_vlan_protocol_packet_ctrl_t))) {
                if (SOC_IS_TRIDENT(unit) &&
                    !SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                                         ENABLE_IGMP_MLD_SNOOPINGf)) {
                    return BCM_E_UNAVAIL;
                }
            }
#endif /* BCM_TRIDENT_SUPPORT */
            rv = _bcm_xgs3_protocol_packet_actions_validate(unit,
                                         &control->protocol_pkt);
            if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
                return rv;
            }
        }
#if defined(BCM_TRX_SUPPORT)
        else {
            bcm_vlan_protocol_packet_ctrl_t tmp_protocol_pkt;
            sal_memset(&tmp_protocol_pkt, 0, sizeof(bcm_vlan_protocol_packet_ctrl_t));
            if (sal_memcmp(&tmp_protocol_pkt, &control->protocol_pkt,
                sizeof(bcm_vlan_protocol_packet_ctrl_t))) {
                return BCM_E_UNAVAIL;
            }
        }
#endif /* BCM_TRX_SUPPORT */
    }

#if defined(BCM_TRIUMPH_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_COSQ_MASK) {
        if (SOC_MEM_IS_VALID(unit, VLAN_COS_MAPm)) {
#if defined(BCM_KATANA_SUPPORT)
            if (SOC_IS_KATANAX(unit)) {
                rv = _vlan_kt_cosq_map_set(unit, vid, control);
            } else
#endif
            {
                rv = _vlan_cosq_map_set(unit, vid, control);
            }
            BCM_IF_ERROR_RETURN(rv);
        }
    }
#if defined(INCLUDE_L3)
    /* Check L3 Ingress Interface Map mode. */
    if (soc_feature(unit, soc_feature_l3) && 
            soc_feature(unit, soc_feature_l3_ingress_interface) && 
            (!ingress_map_mode)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
            if (!(control->flags & BCM_VLAN_L2_CLASS_ID_ONLY ) ||
                 (control->flags & BCM_VLAN_L3_CLASS_ID)) {
               if_class = control->if_class;
               if (valid_fields & BCM_VLAN_CONTROL_VLAN_L3_IF_CLASS_MASK) {
                   if (control->flags & BCM_VLAN_L3_CLASS_ID) {
                       if_class = control->l3_if_class;
                   }
               }
               if ((SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L3_IIFf) ||
                   SOC_MEM_FIELD_VALID(unit, VLAN_MPLSm, L3_IIFf)) &&
                   SOC_MEM_IS_VALID(unit, L3_IIFm)) {
                   rv =  _bcm_tr_l3_intf_class_set(unit, vid, if_class);
                   BCM_IF_ERROR_RETURN(rv);
               }
            }
        }
        if (SOC_MEM_FIELD_VALID(unit, L3_IIFm, ALLOW_GLOBAL_ROUTEf)) {
             rv =  _bcm_tr_l3_intf_global_route_enable_set(unit, vid,
                          (0 == (control->flags & BCM_VLAN_L3_VRF_GLOBAL_DISABLE)));
             BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        if ((soc_feature(unit, soc_feature_l3) && (ingress_map_mode) && 
                soc_feature(unit, soc_feature_l3_ingress_interface)) ||
                (BCM_VLAN_VALID(control->forwarding_vlan ) &&
                (control->ingress_if <= soc_mem_index_max(unit, L3_IIFm))) ) {
            if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
                vlan_mpls_entry_t vlan_mpls;

                sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY, vid,
                              &vlan_mpls));
                /* Configure Vlan mapping */
                soc_mem_field32_set(unit, VLAN_MPLSm, &vlan_mpls,
                        L3_IIFf, control->ingress_if);

                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, VLAN_MPLSm, MEM_BLOCK_ALL, vid,
                               &vlan_mpls));
            }
        }
    }
#if defined(BCM_TRIDENT2_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_NAT_REALM_ID_MASK) {
        /* set the src realm id in L3_IIF table */
        if ((soc_feature(unit, soc_feature_nat) ||
            soc_feature(unit, soc_feature_large_scale_nat))
            && (SOC_MEM_IS_VALID(unit, L3_IIFm))
            && (SOC_MEM_FIELD_VALID(unit, L3_IIFm, SRC_REALM_IDf)) &&
            (!ingress_map_mode)) {
            /* validate realm id param */
            if ((control->nat_realm_id < 0) || (control->nat_realm_id > 3)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(_bcm_tr_l3_intf_nat_realm_id_set(unit, vid,
                                    control->nat_realm_id));
            if(control->flags & BCM_VLAN_SRC_DST_NAT_REALM_ID) {
                BCM_IF_ERROR_RETURN(
                    _bcm_td2_nh_nat_id_update_by_vlan(
                        unit, vid, control->nat_realm_id));
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* INCLUDE_L3 */
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3) && 
            soc_feature(unit, soc_feature_l3_ingress_interface) &&
            (!ingress_map_mode)) {
        if (SOC_MEM_FIELD_VALID(unit, L3_IIFm, URPF_DEFAULTROUTECHECKf) ||
                            soc_feature(unit, soc_feature_l3_iif_profile)) {
            rv =  _bcm_tr2_l3_intf_urpf_default_route_set(unit, vid,
                        (0 == (control->flags & BCM_VLAN_L3_URPF_DEFAULT_ROUTE_CHECK_DISABLE)));
            BCM_IF_ERROR_RETURN(rv);
            if (valid_fields & BCM_VLAN_CONTROL_VLAN_URPF_MODE_MASK) {
                if (SOC_MEM_FIELD_VALID(unit, L3_IIFm, URPF_MODEf) ||
                                soc_feature(unit, soc_feature_l3_iif_profile)) {
                    rv =  _bcm_tr2_l3_intf_urpf_mode_set(unit, vid,
                                                         control->urpf_mode);
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
        }
    }
    if (soc_feature(unit, soc_feature_mpls)) {
        if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
        vlan_mpls_entry_t vlan_mpls_entry;
        sal_memset(&vlan_mpls_entry, 0, sizeof(vlan_mpls_entry_t));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY, vid,
                &vlan_mpls_entry));
        soc_mem_field32_set(unit, VLAN_MPLSm,
            &vlan_mpls_entry, MPLS_ENABLEf,
            (control->flags & BCM_VLAN_MPLS_DISABLE) ? 0x0 : 0x1);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, VLAN_MPLSm, MEM_BLOCK_ALL, vid,
                &vlan_mpls_entry));
    }
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
            vlan_attrs_1_entry_t vlan_attrs;
            sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs));
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, VLAN_ATTRS_1m, MEM_BLOCK_ANY, vid,
                    &vlan_attrs));
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, MPLS_ENABLEf,
                (control->flags & BCM_VLAN_MPLS_DISABLE) ? 0x0 : 0x1);
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, FID_IDf,
                                control->forwarding_vlan);
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, VLAN_ATTRS_1m, MEM_BLOCK_ALL, vid,
                    &vlan_attrs));
        }
#endif
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_filter)) {
        vlan_tab_entry_t vlan_tab;
        sal_memset(&vlan_tab, 0, sizeof(vlan_tab));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, EGR_VLANm, MEM_BLOCK_ANY, vid,
                &vlan_tab));
        soc_mem_field32_set(unit, EGR_VLANm, &vlan_tab, EN_EFILTERf,
            (control->flags2 & BCM_VLAN_FLAGS2_MEMBER_EGRESS_DISABLE) ? 0x0 : 0x1);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_VLANm, MEM_BLOCK_ALL, vid,
                &vlan_tab));

        if (SOC_MEM_IS_VALID(unit, VLAN_ATTRS_1m)) {
            vlan_attrs_1_entry_t vlan_attrs;
            sal_memset(&vlan_attrs, 0, sizeof(vlan_attrs));
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, VLAN_ATTRS_1m, MEM_BLOCK_ANY, vid,
                    &vlan_attrs));
            soc_mem_field32_set(unit, VLAN_ATTRS_1m, &vlan_attrs, EN_IFILTERf,
                (control->flags2 & BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE) ? 0x0 : 0x1);
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, VLAN_ATTRS_1m, MEM_BLOCK_ALL, vid,
                    &vlan_attrs));
        } else if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
            vlan_mpls_entry_t vlan_mpls;
            sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, VLAN_MPLSm, MEM_BLOCK_ANY, vid,
                    &vlan_mpls));
            soc_mem_field32_set(unit, VLAN_MPLSm, &vlan_mpls, EN_IFILTERf,
                (control->flags2 & BCM_VLAN_FLAGS2_MEMBER_INGRESS_DISABLE) ? 0x0 : 0x1);
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, VLAN_MPLSm, MEM_BLOCK_ALL, vid,
                    &vlan_mpls));
        }
    }
#endif
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

    soc_mem_lock(unit, VLAN_TABm);
    _bcm_fb2_outer_tpid_tab_lock(unit);

    if (pre_cfg == FALSE) {
        /* after vlan is created */
        rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                               NULL, vid, &profile_index);
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            soc_mem_unlock(unit, VLAN_TABm);
            return (rv);
        }
        old_vpt        = &VLAN_PROFILE_ENTRY(unit, profile_index);
        vpt            = *old_vpt;
        old_outer_tpid = old_vpt->outer_tpid;

    } else {  /* before vlan is created */
        uint16 dft_tpid;

        rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vt);
        if (BCM_FAILURE(rv)) {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            soc_mem_unlock(unit, VLAN_TABm);
            return (rv);
        }

#if defined(BCM_EASY_RELOAD_SUPPORT)
        if (SOC_IS_RELOADING(unit)) {
            /*
             * Use VLAN_PROFILE_ER() as a write-through cache
             * for VLAN profile pointers during easy reload
             */
            profile_index = VLAN_PROFILE_PTR_ER(unit, vid);
        } else
#endif
        {
            profile_index = soc_mem_field32_get(unit, VLAN_TABm, &vt,
                                           VLAN_PROFILE_PTRf);
        }
        old_vpt        = &VLAN_PROFILE_ENTRY(unit, profile_index);
        vpt            = *old_vpt;
        old_outer_tpid = old_vpt->outer_tpid;

        /* check if first time doing pre-configuration */
        if (!SHR_BITGET(vlan_info[unit].pre_cfg_bmp.w,vid)) {
            /* no profile added, hence no valid tpid  */
            dft_tpid = _bcm_fb2_outer_tpid_default_get(unit);
            rv = _bcm_fb2_outer_tpid_entry_add(unit, dft_tpid,
                                           &tpid_index);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }
            old_outer_tpid = dft_tpid;
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        /* If old TPID and new TPID are different, deletes the old entry
         * and adds the new entry.
         */
        if (old_outer_tpid != control->outer_tpid) {
            if (!SOC_MEM_IS_VALID(unit, EGR_VLANm)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return BCM_E_UNAVAIL;
            }

            rv = soc_mem_read(unit, EGR_VLANm, MEM_BLOCK_ANY, (int)vid, &evt);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }

            rv = _bcm_fb2_outer_tpid_lkup(unit, old_outer_tpid, &tpid_index);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }

            rv = _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }

            rv = _bcm_fb2_outer_tpid_entry_add(unit, control->outer_tpid,
                                               &tpid_index);
            if (BCM_FAILURE(rv)) {
                (void)_bcm_fb2_outer_tpid_entry_add(unit, old_outer_tpid,
                                                    &tpid_index);
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }

            soc_mem_field32_set(unit, EGR_VLANm, &evt, OUTER_TPID_INDEXf, tpid_index);
            rv = WRITE_EGR_VLANm(unit, MEM_BLOCK_ALL, (int)vid, &evt);
            if (BCM_FAILURE(rv)) {
                _bcm_fb2_outer_tpid_tab_unlock(unit);
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }
        }
    }
    vpt.flags = control->flags;
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        vpt.outer_tpid = control->outer_tpid;
    } else {
        vpt.outer_tpid = old_outer_tpid;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK) {
        vpt.ip6_mcast_flood_mode = control->ip6_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK) {
        vpt.ip4_mcast_flood_mode = control->ip4_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK) {
        vpt.l2_mcast_flood_mode = control->l2_mcast_flood_mode;
    }
#if defined(BCM_TRX_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK) {
        if (soc_feature (unit, soc_feature_vlan_protocol_pkt_ctrl)
            && SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm,
                                      PROTOCOL_PKT_INDEXf)) {
            sal_memcpy(&vpt.protocol_pkt, &control->protocol_pkt,
                        sizeof(bcm_vlan_protocol_packet_ctrl_t));
        }
    }
#endif /* BCM_TRX_SUPPORT */

    if (!SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, USE_VLAN_CMLf) &&
        (control->learn_flags != 0)) {
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        soc_mem_unlock(unit, VLAN_TABm);
        return BCM_E_UNAVAIL;
    }
#if defined(BCM_HURRICANE3_SUPPORT)
    else if (SOC_MEM_FIELD_VALID(unit, VLAN_PROFILE_TABm, USE_VLAN_CMLf)) {
        if (control->learn_flags &
            ~(BCM_VLAN_LEARN_CONTROL_ENABLE |
               BCM_VLAN_LEARN_CONTROL_ARL |
               BCM_VLAN_LEARN_CONTROL_CPU |
               BCM_VLAN_LEARN_CONTROL_FWD |
               BCM_VLAN_LEARN_CONTROL_PENDING)) {
            _bcm_fb2_outer_tpid_tab_unlock(unit);
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_PARAM;
        }
        vpt.learn_flags= control->learn_flags;
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    /* Update the vlan profile table only if the old and new entries are
     * different.
     */
    if (pre_cfg == FALSE) {
        /* after vlan is created */
        rv = _vlan_profile_update(unit, vid, &vpt);

    } else {  /* before vlan is created */
        uint8 equal;
        int idx;

        /* Compare original profile with new one. */
        rv = _vlan_profile_compare(unit, &VLAN_PROFILE_ENTRY(unit, profile_index),
                               &vpt, &equal);
        if (rv == BCM_E_NONE && (equal != TRUE)) {
            /* Add new profile. */
            rv = _vlan_profile_add(unit, &vpt, &idx);

            if (BCM_SUCCESS(rv)) {
                /* Set new vlan profile index. */
                soc_mem_field32_set(unit, VLAN_TABm, &vt, VLAN_PROFILE_PTRf, idx);

                /* Write back vlan table entry. */
                rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int)vid, &vt);
                if (BCM_SUCCESS(rv)) {
                    /* Delete original vlan profile if new one was allocated and
                       the original one is valid. */
                    if (SHR_BITGET(vlan_info[unit].pre_cfg_bmp.w,vid) &&
                         (profile_index != idx)) {
                         rv = _vlan_profile_delete(unit, profile_index);
                    }
                }
            }
        }
    }
    if (BCM_FAILURE(rv)) {
        if (-1 != tpid_index) {
            _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        (void)_bcm_fb2_outer_tpid_entry_add(unit, old_outer_tpid, &tpid_index);
        _bcm_fb2_outer_tpid_tab_unlock(unit);
        soc_mem_unlock(unit, VLAN_TABm);
        return (rv);
    }

    _bcm_fb2_outer_tpid_tab_unlock(unit);

    rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vt);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return (rv);
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_VRF_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VRF_IDf)) {
            soc_mem_field32_set(unit, VLAN_TABm, &vt, VRF_IDf, control->vrf);
        }
#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
        else if (SOC_IS_TR_VL(unit) && soc_feature(unit, soc_feature_l3) && 
                soc_feature(unit, soc_feature_l3_ingress_interface) && 
                SOC_MEM_IS_VALID(unit,L3_IIFm)) {
            if (soc_feature(unit, soc_feature_no_vlan_vrf)) {
                /* 
                 * Return E_UNAVAIL for those device can't support VID 1:1 
                 * mapping to L3_IIFm to set VLAN based VRF_ID
                 */
                soc_mem_unlock(unit, VLAN_TABm);
                return BCM_E_UNAVAIL;
            } else if (!ingress_map_mode) {
            rv = _bcm_tr_l3_intf_vrf_bind(unit, vid, control->vrf);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
        }
        }
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
    }

#if defined(BCM_TRIUMPH_SUPPORT) && defined(INCLUDE_L3)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L3_IIFf) && 
                soc_feature(unit, soc_feature_l3_ingress_interface) && 
                soc_feature(unit, soc_feature_l3)) {
            if (ingress_map_mode) {
                /* Configure Vlan mapping */
                soc_mem_field32_set(unit, VLAN_TABm, &vt,
                            L3_IIFf, control->ingress_if);
            }
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, FID_IDf)) {
            soc_mem_field32_set(unit, VLAN_TABm, &vt, FID_IDf,
                                control->forwarding_vlan);
        } else {
              /* For Raven and Hawkeye devices FID_ID is not valid.*/
              soc_mem_unlock(unit, VLAN_TABm);
              return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    /* if_class is set for any of below conditions:
     * BCM_VLAN_L2_CLASS_ID_ONLY and BCM_VLAN_L3_CLASS_ID are not set
     * Only BCM_VLAN_L2_CLASS_ID_ONLY is set
     * When both BCM_VLAN_L2_CLASS_ID_ONLY and BCM_VLAN_L3_CLASS_ID are set */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
        if (!((!(control->flags & BCM_VLAN_L2_CLASS_ID_ONLY)) &&
             (control->flags & BCM_VLAN_L3_CLASS_ID))) {
            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VLAN_CLASS_IDf)) {
                soc_mem_field32_set(unit, VLAN_TABm, &vt, VLAN_CLASS_IDf,
                                    control->if_class);
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, L2_ENTRY_KEY_TYPEf)) {
            rv = _vlan_control_trx_forwarding_mode_set(unit, (uint32 *)&vt,
                                                       control->forwarding_mode);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_wirelss_traffic_select)) {

        if (control->flags2 & BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT &&
            control->flags2 & BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT) {
            return BCM_E_PARAM;
        }
        if (control->flags2 & BCM_VLAN_FLAGS2_WIRED_COS_MAP_SELECT) {
            soc_mem_field32_set(unit, VLAN_TABm, &vt, WIRED_WIRELESSf, 0x1);
        }

        if (control->flags2 & BCM_VLAN_FLAGS2_WIRELESS_COS_MAP_SELECT) {
            soc_mem_field32_set(unit, VLAN_TABm, &vt, WIRED_WIRELESSf, 0x2);
        }
    }
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_VP_MCAST_MASK) {
        /* Should update manual_vpctrl before trill setting */
    	if (control->vp_mc_ctrl != bcmVlanVPMcControlAuto) {
            SHR_BITSET(vlan_info[unit].vp_mode, vid);
        } else {
            SHR_BITCLR(vlan_info[unit].vp_mode, vid);
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (0 == control->broadcast_group) {
            _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf, 0);
        }
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (0 == control->unknown_multicast_group) {
            _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf, 0);
        }
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (0 == control->unknown_unicast_group) {
            _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf, 0);
        }
    }

    if (soc_feature(unit, soc_feature_wlan)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_WLAN(control->broadcast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_WLAN(control->unknown_multicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_WLAN(control->unknown_unicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_unicast_group));
            }
        }
    }

    if (soc_feature(unit, soc_feature_vlan_vp)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_VLAN(control->broadcast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_VLAN(control->unknown_multicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_VLAN(control->unknown_unicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_unicast_group));
            }
        }
   }
#endif /* (BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_trill)) {
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
              TRILL_TRANSIT_IGMP_MLD_PAYLOAD_TO_CPUf)) {
              if (control->flags & BCM_VLAN_TRILL_IGMP_SNOOP_DISABLE) {
                   soc_mem_field32_set(unit, VLAN_TABm, &vt,
                        TRILL_TRANSIT_IGMP_MLD_PAYLOAD_TO_CPUf, 0x0);
              } else {
                   soc_mem_field32_set(unit, VLAN_TABm, &vt,
                        TRILL_TRANSIT_IGMP_MLD_PAYLOAD_TO_CPUf, 0x1);
              }
        }

        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_VLAN(control->broadcast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_VLAN(control->unknown_multicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_VLAN(control->unknown_unicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_unicast_group));
            }
        }

        if (valid_fields & BCM_VLAN_CONTROL_VLAN_TRILL_NONUNICAST_GROUP_MASK) {
            if (BCM_SUCCESS(bcm_td_trill_multicast_check(unit,
                            control->trill_nonunicast_group))) { 
                /*  Obtain matching l3mc for given l2mc  */
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        TRILL_DOMAIN_NONUC_REPL_INDEXf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                TRILL_DOMAIN_NONUC_REPL_INDEXf,
                                _BCM_MULTICAST_ID_GET(control->trill_nonunicast_group));
                }
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        BC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                       BC_TRILL_NETWORK_RECEIVERS_PRESENTf, 0x1);
                }
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        UMC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                       UMC_TRILL_NETWORK_RECEIVERS_PRESENTf, 0x1);
                }
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        UUC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                       UUC_TRILL_NETWORK_RECEIVERS_PRESENTf, 0x1);
                }
                rv = bcm_td_trill_vlan_multicast_group_add(unit, vid, &vt);
            } else {
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        TRILL_DOMAIN_NONUC_REPL_INDEXf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                TRILL_DOMAIN_NONUC_REPL_INDEXf, 0);
                }
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        BC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                       BC_TRILL_NETWORK_RECEIVERS_PRESENTf, 0);
                }
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        UMC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                       UMC_TRILL_NETWORK_RECEIVERS_PRESENTf, 0);
                }
                if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm,
                        UUC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                             soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                       UUC_TRILL_NETWORK_RECEIVERS_PRESENTf, 0);
                }
                rv = bcm_td_trill_vlan_multicast_group_delete(unit, vid, &vt);
            }

            /* Specify per-Vlan trill_name */
            if (BCM_SUCCESS(rv)) {
                if (valid_fields & BCM_VLAN_CONTROL_VLAN_SOURCE_TRILL_NAME_MASK) {
                    if (control->source_trill_name &&
                        soc_mem_field_valid (unit, VLAN_TABm, TRILL_RBRIDGE_NICKNAME_INDEXf)) {
                        rv = bcm_td_trill_source_trill_name_idx_get(unit,
                                  control->source_trill_name, &trill_name_index);
                        if (BCM_SUCCESS(rv)) {
                               soc_mem_field32_set(unit, VLAN_TABm, &vt,
                                    TRILL_RBRIDGE_NICKNAME_INDEXf,
                                    trill_name_index);
                        }
                    }
                }
            } else {
                soc_mem_unlock(unit, VLAN_TABm);
                return (rv);
            }
        }
    }

    if (soc_feature(unit, soc_feature_niv)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_NIV(control->broadcast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_NIV(control->unknown_multicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_NIV(control->unknown_unicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_unicast_group));
            }
        }
    }

    if (soc_feature(unit, soc_feature_l3) && soc_property_get(unit, spn_L3_ENABLE, 1)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_L3(control->broadcast_group)) {
                soc_mem_field32_set(unit, VLAN_TABm, &vt, BC_IDXf,
                                    _BCM_MULTICAST_ID_GET
                                    (control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_L3(control->unknown_multicast_group)) {
                soc_mem_field32_set(unit, VLAN_TABm, &vt, UMC_IDXf,
                                    _BCM_MULTICAST_ID_GET
                                    (control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_L3(control->unknown_unicast_group)) {
                soc_mem_field32_set(unit, VLAN_TABm, &vt, UUC_IDXf,
                                    _BCM_MULTICAST_ID_GET
                                    (control->unknown_unicast_group));
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_port_extension)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_EXTENDER(control->broadcast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_EXTENDER(control->unknown_multicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_EXTENDER(control->unknown_unicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_unicast_group));
            }
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_SUBPORT(control->broadcast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, BC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->broadcast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_SUBPORT(control->unknown_multicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UMC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_multicast_group));
            }
        }
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
            if (_BCM_MULTICAST_IS_SUBPORT(control->unknown_unicast_group)) {
                _bcm_xgs3_vlan_mcast_idx_set(unit, &vt, UUC_IDXf,
                    _BCM_MULTICAST_ID_GET(control->unknown_unicast_group));
            }
        }
    }
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) */


#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_VP_MCAST_MASK) {
        if (control->vp_mc_ctrl == bcmVlanVPMcControlEnable) {
            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
                int i, mc_idx;
                bcm_pbmp_t vlan_pbmp, vlan_ubmp, pbmp, l3_pbmp;
                soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
#if defined(BCM_HGPROXY_COE_SUPPORT)
                bcm_port_t         port_iter;
                int                port_type;
#endif
                /* Also need to copy the physical port members to the L2_BITMAP of
                 * the IPMC entry for each group once we've gone virtual */
                rv = mbcm_driver[unit]->mbcm_vlan_port_get
                   (unit, vid, &vlan_pbmp, &vlan_ubmp, NULL);

                if (rv < 0) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
#if defined(BCM_HGPROXY_COE_SUPPORT)
                /* Coe cascaded port should be removed from l2bmap of L3_IPMC. */
                if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, PORT_TYPEf)) {
                    BCM_PBMP_ITER(vlan_pbmp, port_iter) {
                        rv = _bcm_esw_port_tab_get(unit, port_iter,
                                                   PORT_TYPEf, &port_type);
                        if (rv < 0) {
                            soc_mem_unlock(unit, VLAN_TABm);
                            return rv;
                        }
                        if (port_type == _BCM_COE_PORT_TYPE_CASCADED) {
                            BCM_PBMP_PORT_REMOVE(vlan_pbmp, port_iter);
                        }
                    }
                }
#endif
                /* Deal with each group */
                for (i = 0; i < 3; i++) {
                    mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, &vt,
                                                group_type[i]);
                    rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &pbmp,
                                                     &l3_pbmp);
                    if (rv < 0) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return rv;
                    }

                    if (BCM_PBMP_IS_NULL(pbmp)) {
                        rv = _bcm_esw_multicast_ipmc_write(unit, mc_idx, vlan_pbmp,
                                                          l3_pbmp, TRUE);
                        if (rv < 0) {
                            soc_mem_unlock(unit, VLAN_TABm);
                            return rv;
                        }
                    }
                }
                soc_mem_field32_set(unit, VLAN_TABm, &vt, VIRTUAL_PORT_ENf, 1);
            }
        } else if (control->vp_mc_ctrl == bcmVlanVPMcControlDisable) {
            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
                soc_mem_field32_set(unit, VLAN_TABm, &vt, VIRTUAL_PORT_ENf, 0);
            }
        }
    }

#endif /* defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3) */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vfi_from_vlan_tables)) {
        if (_BCM_VPN_VFI_IS_SET (control->vpn)) {
                int vfi_index = -1, num_vfi = -1;
                _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, control->vpn);
                num_vfi = soc_mem_index_count(unit, VFIm);
                if (vfi_index > (num_vfi-1)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return BCM_E_PARAM;
                }
                if (vfi_index &&
                    (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeAny))) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return BCM_E_NOT_FOUND;
                }
            if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm) &&
                SOC_MEM_FIELD_VALID(unit, VLAN_MPLSm, VFIf)) {
                rv = soc_mem_field32_modify(unit, VLAN_MPLSm, vid, VFIf, vfi_index); 
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
                }
#if defined(BCM_TRIDENT3_SUPPORT)
            else if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VFIf)) {
                soc_mem_field32_set(unit, VLAN_TABm, &vt, VFIf, vfi_index);
            }
#endif/* BCM_TRIDENT3_SUPPORT */
        } else if (control->vpn) {
            soc_mem_unlock(unit, VLAN_TABm);
            return (BCM_E_PARAM);
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_SR_FLAGS_MASK) {
            int i;
            void *entbuf;
            soc_mem_t mem[2] = {VLAN_TABm, EGR_VLANm};

            if ((!(control->sr_flags & BCM_VLAN_CONTROL_SR_FLAG_ENABLE)) &&
                  (control->sr_flags & BCM_VLAN_CONTROL_SR_FLAG_LAN_ID)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return BCM_E_PARAM;
            }

            for (i = 0; i < 2; i++){
                if (1 == i) {
                    /* handle egress vlan table, skip it if there is no such table */
                    entbuf = &evt;
                    if(SOC_MEM_IS_VALID(unit, mem[i])){
                        rv = soc_mem_read(unit, mem[i], MEM_BLOCK_ANY,
                                          (int)vid, &evt);
                        if (BCM_FAILURE(rv)) {
                            soc_mem_unlock(unit, VLAN_TABm);
                            return (rv);
                        }
                    } else {
                        break;
                    }
                } else {
                    entbuf = &vt;
                }
                if (control->sr_flags & BCM_VLAN_CONTROL_SR_FLAG_ENABLE) {
                    soc_mem_field32_set(unit, mem[i], entbuf, SR_ENABLEf, 1);
                    soc_mem_field32_set(unit, mem[i], entbuf, SR_LAN_ID_VALIDf, 1);
                    if (control->sr_flags & BCM_VLAN_CONTROL_SR_FLAG_LAN_ID) {
                        soc_mem_field32_set(unit, mem[i], entbuf, SR_LAN_IDf, 1);
                    } else {
                        soc_mem_field32_set(unit, mem[i], entbuf, SR_LAN_IDf, 0);
                    }
                } else {
                    soc_mem_field32_set(unit, mem[i], entbuf, SR_ENABLEf, 0);
                    soc_mem_field32_set(unit, mem[i], entbuf, SR_LAN_ID_VALIDf, 0);
                }
                if (1 == i) {
                    /* handle egress vlan table */
                    rv = soc_mem_write(unit, mem[i], MEM_BLOCK_ANY,
                                       (int)vid, entbuf);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, VLAN_TABm);
                        return (rv);
                    }
                }
            }
        }
   }
#endif /* BCM_TSN_SR_SUPPORT*/



    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, ENABLE_IGMP_MLD_SNOOPINGf)) {
        soc_mem_field32_set(unit, VLAN_TABm, &vt, ENABLE_IGMP_MLD_SNOOPINGf,
                      (0 == (control->flags & BCM_VLAN_IGMP_SNOOP_DISABLE)));
    }

    rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vt);
    soc_mem_unlock(unit, VLAN_TABm);
    if (BCM_SUCCESS(rv) && (pre_cfg == TRUE)) {
        SHR_BITSET(vlan_info[unit].pre_cfg_bmp.w,vid);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */
    return (rv);
}

#if defined (BCM_TRX_SUPPORT)
/*
 * Function :
 *    _vlan_block_mask_create
 *
 * Purpose  :
 *    Compose block mask based 
 *
 * Parameters:
 *     unit        - (IN)  BCM device number.
 *     first_mask  - (IN)  First mask.
 *     second_mask - (IN)  Second mask.
 *     select      - (IN)  Mask select. 
 *     result_mask - (OUT) Output mask.     
 * Return :
 *    BCM_E_XXX
 */
int
_vlan_block_mask_create(int unit, bcm_pbmp_t first_mask, 
                         bcm_pbmp_t second_mask, 
                         uint8 select, bcm_pbmp_t *result_mask)
{
    /* Input parameters check */
    if (NULL == result_mask) {
        return (BCM_E_PARAM);
    }

    switch (select) {
      case 0:
          BCM_PBMP_CLEAR(*result_mask);
          break;
      case 1:
          BCM_PBMP_ASSIGN(*result_mask, first_mask);
          break;
      case 2:
          BCM_PBMP_ASSIGN(*result_mask, second_mask);
          break;
      case 3:
          BCM_PBMP_ASSIGN(*result_mask, first_mask);
          BCM_PBMP_OR(*result_mask, second_mask);
          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _bcm_trx_vlan_block_get
 * 
 * Purpose  :
 *    Get vlan specific traffic blocking parameters
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id. 
 *     block   - (IN) Block configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_trx_vlan_block_get (int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    _trx_vlan_block_t  *hw_block; /* Vlan block profile.      */
    int                rv;        /* Operation return status. */
    int                idx;       /* Vlan profile index.      */

    /* Input parameters check. */
    if (NULL == block) {
        return (BCM_E_PARAM);
    }

    /* Get original vlan profile. */
    rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                               NULL, vid, &idx);
    BCM_IF_ERROR_RETURN(rv);

    hw_block = &VLAN_PROFILE_ENTRY(unit, idx).block;

    rv = _vlan_block_mask_create (unit, hw_block->first_mask, 
                                  hw_block->second_mask, 
                                  hw_block->broadcast_mask_sel, 
                                  &block->broadcast);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_mask_create (unit, hw_block->first_mask, 
                                  hw_block->second_mask, 
                                  hw_block->unknown_unicast_mask_sel, 
                                  &block->unknown_unicast);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_mask_create (unit, 
                                  hw_block->first_mask, 
                                  hw_block->second_mask, 
                                  hw_block->unknown_mulitcast_mask_sel, 
                                  &block->unknown_multicast);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_mask_create (unit, 
                                  hw_block->first_mask, 
                                  hw_block->second_mask, 
                                  hw_block->known_mulitcast_mask_sel, 
                                  &block->known_multicast);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/* Number of lookup types in bcm_vlan_block_t structure. */
#define _BCM_VLAN_BLOCK_LOOKUP_TYPES (4)

/*
 * Function :
 *    _vlan_block_bitmap_min
 * 
 * Purpose  :
 *    Find shortest bitmap. 
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     array   - (IN) Bitmaps array. 
 *     arr_sz  - (IN) Array size. 
 *     min_pbm - (OUT) Shortest bitmap.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_vlan_block_bitmap_min(int unit, bcm_pbmp_t *array, 
                       int arr_size, bcm_pbmp_t *min_pbm)
{      
    int  idx;       /* Bitmaps array iteration index. */
    int  min;       /* Mask length minimum.           */
    int  mask_len;  /* Bitmap length.                 */

    if (NULL == array) {
        return (BCM_E_PARAM);
    }
    BCM_PBMP_CLEAR(*min_pbm);
    min = 0;
    for (idx = 0; idx < arr_size; idx++) {

        BCM_PBMP_COUNT(array[idx], mask_len);
        if (0 == mask_len) {
            continue;
        } else {
            if ((0 == min) || ((0 != min) && (mask_len < min)))  {
                min = mask_len;
                BCM_PBMP_ASSIGN(*min_pbm, array[idx]);
            }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _vlan_block_bitmap_clear
 * 
 * Purpose  :
 *    Clear those masks in the array that exactly match the given mask. 
 *
 * Parameters:
 *     unit    - (IN)     BCM device number.
 *     array   - (IN/OUT) Bitmaps array 
 *     arr_sz  - (IN)     Array size. 
 *     mask    - (IN)     Mask.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_vlan_block_bitmap_clear(int unit, bcm_pbmp_t *array, 
                       int arr_size, bcm_pbmp_t mask)
{      
    int  idx;       /* Bitmaps array iteration index. */

    if (NULL == array) {
        return (BCM_E_PARAM);
    }

    for (idx = 0; idx < arr_size; idx++) {
        if (BCM_PBMP_EQ(array[idx], mask)) {
            BCM_PBMP_CLEAR(array[idx]);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _vlan_block_select_get
 * 
 * Purpose  :
 *    Identify block mask combination which will produce 
 *    requested block mask 
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     req         - (IN) Requested mask.
 *     first       - (IN) Bitmaps array. 
 *     second      - (IN) Bitmaps array.
 *     select      - (OUT)Block mask.
 * Return :
 *    BCM_E_XXX
 */
int
_vlan_block_select_get(int unit, bcm_pbmp_t request, 
                       bcm_pbmp_t first, 
                       bcm_pbmp_t second, 
                       uint8 *sel)
{      
    bcm_pbmp_t or_mask;

    /* Input parameters check. */
    if (NULL == sel) {
        return (BCM_E_PARAM);
    }

    if (BCM_PBMP_IS_NULL(request)) {
        *sel = 0;
    } else if (BCM_PBMP_EQ(first, request)) {
        *sel = 0x1;
    } else if (BCM_PBMP_EQ(second, request)) {
        *sel = 0x2;
    } else {
        BCM_PBMP_ASSIGN(or_mask, first);
        BCM_PBMP_OR(or_mask, second);
        if (BCM_PBMP_EQ(or_mask, request)) {
            *sel = 0x3; 
        } else {
            return (BCM_E_RESOURCE);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _bcm_trx_vlan_block_set
 * 
 * Purpose  :
 *    sjt vlan specific traffic blocking parameters
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id. 
 *     block   - (IN) Block configuration.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_trx_vlan_block_set (int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    bcm_pbmp_t         array[_BCM_VLAN_BLOCK_LOOKUP_TYPES];
    _trx_vlan_block_t  hw_block;       /* HW block structure.      */
    _vlan_profile_t    profile;        /* Original vlan profile.   */
    int                profile_idx;    /* Profile index.           */
    int                rv;             /* Operation return status. */

    /* Input parameters check. */
    if (NULL == block) {
        return (BCM_E_PARAM);
    }

    /* Store block mask in a array. */
    BCM_PBMP_ASSIGN(array[0], block->broadcast);
    BCM_PBMP_ASSIGN(array[1], block->unknown_unicast);
    BCM_PBMP_ASSIGN(array[2], block->unknown_multicast);
    BCM_PBMP_ASSIGN(array[3], block->known_multicast);

    /* Select first  shortest mask. */

    /* Coverity
     * The index coverity thinks outside of boundary is actually for 
     * the field pbits of the pbmp structure.
     */
    /* coverity[overrun-buffer-val: FALSE] */
    rv = _vlan_block_bitmap_min(unit, array,
                                _BCM_VLAN_BLOCK_LOOKUP_TYPES,
                                &hw_block.first_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear those masks in the array that exactly match the shortest mask. */
    /* coverity[overrun-buffer-val: FALSE] */
    rv = _vlan_block_bitmap_clear(unit, array,
                                  _BCM_VLAN_BLOCK_LOOKUP_TYPES,
                                  hw_block.first_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Select second shortest mask. */
    /* coverity[overrun-buffer-val: FALSE] */
    rv = _vlan_block_bitmap_min(unit, array,
                                _BCM_VLAN_BLOCK_LOOKUP_TYPES,
                                &hw_block.second_mask);
    BCM_IF_ERROR_RETURN(rv);


    /* Get mask selector for each lookup type. */
    rv = _vlan_block_select_get(unit, block->broadcast, hw_block.first_mask,
                                hw_block.second_mask, &hw_block.broadcast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_select_get(unit, block->unknown_unicast, hw_block.first_mask,
                                hw_block.second_mask, &hw_block.unknown_unicast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_select_get(unit, block->unknown_multicast, hw_block.first_mask,
                                hw_block.second_mask, &hw_block.unknown_mulitcast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    rv = _vlan_block_select_get(unit, block->known_multicast, hw_block.first_mask,
                                hw_block.second_mask, &hw_block.known_mulitcast_mask_sel);
    BCM_IF_ERROR_RETURN(rv);

    /* Get original vlan profile. */
    rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf,
                               NULL, vid, &profile_idx);
    BCM_IF_ERROR_RETURN(rv);

    profile = VLAN_PROFILE_ENTRY(unit, profile_idx);
    profile.block = hw_block;

    /* Update vlan profile. */
    rv = _vlan_profile_update(unit, vid, &profile);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *    _bcm_tr2_vlan_qmid_set
 * Purpose:
 *    Set vlan queue mapping pointer in vlan profile table
 * Parameters:
 *     unit            device number
 *     vlan            vlan idenetifier
 *     qm_ptr          queue mapping identifier (use -1 to disable)
 *     use_inner_tag   use priority from inner tag
 * Return:
 *    BCM_E_XXX
 */
int
_bcm_tr2_vlan_qmid_set(int unit, bcm_vlan_t vlan, int qmid,
                       int use_inner_tag)
{
    _vlan_profile_t profile;
    int profile_idx;
    uint32 profile_flags;

    BCM_IF_ERROR_RETURN
        (_vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf, NULL,
                               vlan, &profile_idx));

    profile = VLAN_PROFILE_ENTRY(unit, profile_idx);
    profile_flags = profile.profile_flags;
    profile_flags &= ~(_BCM_VLAN_PROFILE_PHB2_ENABLE |
                       _BCM_VLAN_PROFILE_PHB2_USE_INNER_TAG);
    if (qmid >= 0) {
        profile_flags |= _BCM_VLAN_PROFILE_PHB2_ENABLE;
    } else {
        qmid = 0;
    }
    if (use_inner_tag) {
        profile_flags |= _BCM_VLAN_PROFILE_PHB2_USE_INNER_TAG;
    }

    if (profile.qm_ptr == qmid && profile.profile_flags == profile_flags) {
        return BCM_E_NONE;
    }

    profile.profile_flags = profile_flags;
    profile.qm_ptr = qmid;
    BCM_IF_ERROR_RETURN(_vlan_profile_update(unit, vlan, &profile));

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_tr2_vlan_qmid_get
 * Purpose:
 *    get vlan queue mapping pointer in vlan profile table
 * Parameters:
 *     unit            device number
 *     vlan            vlan idenetifier
 *     qm_ptr          (OUT) queue mapping identifier (use -1 to disable)
 *     use_inner_tag   (OUT) use priority from inner tag
 * Return:
 *    BCM_E_XXX
 */
int
_bcm_tr2_vlan_qmid_get(int unit, bcm_vlan_t vlan, int *qmid,
                       int *use_inner_tag)
{
    _vlan_profile_t profile;
    int profile_idx;

    if (qmid == NULL || use_inner_tag == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf, NULL,
                               vlan, &profile_idx));

    profile = VLAN_PROFILE_ENTRY(unit, profile_idx);
    if (profile.profile_flags & _BCM_VLAN_PROFILE_PHB2_ENABLE) {
        *qmid = profile.qm_ptr;
    } else {
        *qmid = -1;
	return BCM_E_NOT_FOUND;
    }
    if (profile.profile_flags & _BCM_VLAN_PROFILE_PHB2_USE_INNER_TAG) {
        *use_inner_tag = 1;
    } else {
        *use_inner_tag = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_tr2_qos_pvmid_set
 * Purpose:
 *    Set vlan port vlan mapping pointer in vlan profile table
 * Parameters:
 *     unit            device number
 *     vlan            vlan idenetifier
 *     pvm_ptr         port vlan mapping identifier (use -1 to disable)
 * Return:
 *    BCM_E_XXX
 */
int
_bcm_tr2_qos_pvmid_set(int unit, bcm_vlan_t vlan, int pvmid)
{
    int rv;
    _vlan_profile_t profile;
    int profile_idx;
    uint32 profile_flags;

    soc_mem_lock(unit, VLAN_TABm);

    rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf, NULL,
                               vlan, &profile_idx);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

    profile = VLAN_PROFILE_ENTRY(unit, profile_idx);
    profile_flags = profile.profile_flags;
    profile_flags &= ~_BCM_VLAN_PROFILE_TRUST_DOT1P;
    if (pvmid >= 0) {
        profile_flags |= _BCM_VLAN_PROFILE_TRUST_DOT1P;
    } else {
        pvmid = 0;
    }

    if (profile.trust_dot1p_ptr == pvmid && 
        profile.profile_flags == profile_flags) {
        soc_mem_unlock(unit, VLAN_TABm);
        return BCM_E_NONE;
    }

    profile.profile_flags = profile_flags;
    profile.trust_dot1p_ptr = pvmid;

    rv = _vlan_profile_update(unit, vlan, &profile);

    soc_mem_unlock(unit, VLAN_TABm);

    return rv;
}

/*
 * Function:
 *    _bcm_tr2_qos_pvmid_get
 * Purpose:
 *    get vlan port vlan mapping pointer in vlan profile table
 * Parameters:
 *     unit            device number
 *     vlan            vlan idenetifier
 *     pvm_ptr         (OUT) port vlan mapping identifier (use -1 to disable)
 * Return:
 *    BCM_E_XXX
 */
int
_bcm_tr2_qos_pvmid_get(int unit, bcm_vlan_t vlan, int *pvmid)
{
    int rv;
    _vlan_profile_t profile;
    int profile_idx;

    if (pvmid == NULL) {
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, VLAN_TABm);

    rv = _vlan_profile_idx_get(unit, VLAN_TABm, VLAN_PROFILE_PTRf, NULL,
                               vlan, &profile_idx);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }

    profile = VLAN_PROFILE_ENTRY(unit, profile_idx);
    if (profile.profile_flags & _BCM_VLAN_PROFILE_TRUST_DOT1P) {
        *pvmid = profile.trust_dot1p_ptr;
    } else {
        *pvmid = -1;
    }

    soc_mem_unlock(unit, VLAN_TABm);

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      _bcm_fb_vlan_translate_add
 * Purpose:
 *      Helper function to bcm_esw_vlan_translate_add to add vlan translation 
 *      for Firebolt
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 *      xtable  - Memory to use for Firebolt
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int
_bcm_fb_vlan_translate_add(int unit, int port, bcm_vlan_t old_vid,
                           bcm_vlan_t new_vid, int prio,
                           int xtable)
{
    vlan_xlate_entry_t  vtent;
    int                 index_min, index_max;
    soc_mem_t           mem;
    int                 addvid;
    uint32              *vtcachep, vtcache;
    uint32              ve_valid, ve_port, ve_vid, ve_add;
    int                 empty, match, slot, i, rv;

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    switch (xtable) {
    case BCM_VLAN_XLATE_ING:
        mem = VLAN_XLATEm;
        addvid = 0;
        break;
    case BCM_VLAN_XLATE_EGR:
        mem = EGR_VLAN_XLATEm;
        addvid = -1;
        break;
    case BCM_VLAN_XLATE_DTAG:
        mem = VLAN_XLATEm;
        addvid = 1;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    if (((port < 0) || (old_vid == BCM_VLAN_NONE)) && port != -1) {
        return BCM_E_PARAM;
    }

#ifdef BCM_KATANA2_SUPPORT
    if ((port != -1) && ((soc_feature(unit, soc_feature_linkphy_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, port)) ||
         (soc_feature(unit, soc_feature_subtag_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, port)))) {
    } else
#endif
    if (port >= 0 && !SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if ((prio != -1) && ((prio & ~BCM_PRIO_MASK) != 0)) {
        return BCM_E_PARAM;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    /* get vt cache, allocate if necessary */
    if (mem == EGR_VLAN_XLATEm) {
        vtcachep = vlan_info[unit].egr_trans;
    } else {
        vtcachep = vlan_info[unit].ing_trans;
    }
    if (vtcachep == NULL) {
        vtcachep = sal_alloc(sizeof(*vtcachep) * (index_max+1),
                             "vlan trans cache");
        if (vtcachep == NULL) {
            soc_mem_unlock(unit, mem);
            return BCM_E_MEMORY;
        }
        sal_memset(vtcachep, 0, sizeof(*vtcachep) * (index_max+1));
        if (mem == EGR_VLAN_XLATEm) {
            vlan_info[unit].egr_trans = vtcachep;
        } else {
            vlan_info[unit].ing_trans = vtcachep;
        }
    }

    /* search vt cache for match */
    match = empty = -1;
    for (i = index_min; i <= index_max; i++) {
        vtcache = vtcachep[i];
        ve_valid = BCM_VTCACHE_VALID_GET(vtcache);
        if (empty < 0 && !ve_valid) {
            empty = i;
        }
        ve_port = BCM_VTCACHE_PORT_GET(vtcache);
        if ((int)ve_port != port) {
            continue;
        }
        ve_vid = BCM_VTCACHE_VID_GET(vtcache);
        if (ve_vid != old_vid) {
            continue;
        }
        if (addvid >= 0) {
            ve_add = BCM_VTCACHE_ADD_GET(vtcache);
            if ((int)ve_add != addvid) {
                soc_mem_unlock(unit, mem);
                return BCM_E_EXISTS;
            }
        }
        match = i;
        break;
    }

    if (match != -1) {
        slot = match;
    } else if (empty != -1) {
        slot = empty;
    } else {
        soc_mem_unlock(unit, mem);
        return BCM_E_FULL;
    }

    sal_memset(&vtent, 0, sizeof(vtent));

    soc_mem_field32_set(unit, mem, &vtent, VALIDf, 1);
    if (port == -1) { 
        soc_mem_field32_set(unit, mem, &vtent, PORTf, 0);
    } else {
        soc_mem_field32_set(unit, mem, &vtent, PORTf, port);
    }
    soc_mem_field32_set(unit, mem, &vtent, OLD_VLAN_IDf, old_vid);
    soc_mem_field32_set(unit, mem, &vtent, NEW_VLAN_IDf, new_vid);
    if (soc_mem_field_valid(unit, mem, MASKf)) {
        if (port == -1) { 
            if (mem == VLAN_XLATEm) {
                soc_mem_field32_set(unit, mem, &vtent, MASKf, 0x1ffe0);
            } else {  /* mem == EGR_VLAN_XLATEm */
                soc_mem_field32_set(unit, mem, &vtent, MASKf, 0xfff);
            }
        } else {
            soc_mem_field32_set(unit, mem, &vtent, MASKf, 
                            (1 << soc_mem_field_length(unit, mem, MASKf)) - 1);
        }
    }
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    if (soc_mem_field_valid(unit, mem, RESERVED_MASKf)) {
        soc_mem_field32_set(unit, mem, &vtent, RESERVED_MASKf, 0);
    }
    if (soc_mem_field_valid(unit, mem, RESERVED_DATAf)) {
        soc_mem_field32_set(unit, mem, &vtent, RESERVED_DATAf, 0);
    }
    if (soc_mem_field_valid(unit, mem, RESERVED_KEYf)) {
        soc_mem_field32_set(unit, mem, &vtent, RESERVED_KEYf, 0);
    }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */

    if (prio >= 0) {
        soc_mem_field32_set(unit, mem, &vtent, RPEf, 1);
        soc_mem_field32_set(unit, mem, &vtent, PRIf, prio);
    }
    if (addvid >= 0) {
        soc_mem_field32_set(unit, mem, &vtent, ADD_VIDf, addvid);
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot, &vtent);
    if (rv >= 0) {
        vtcache = 0;
        BCM_VTCACHE_VALID_SET(vtcache, 1);
        BCM_VTCACHE_PORT_SET(vtcache, port);
        BCM_VTCACHE_VID_SET(vtcache, old_vid);
        if (addvid >= 0) {
            BCM_VTCACHE_ADD_SET(vtcache, addvid);
        }
        vtcachep[slot] = vtcache;
    }
    soc_mem_unlock(unit, mem);
    return rv;
}

/*
 * Function:
 *      _bcm_fb_vlan_translate_delete
 * Purpose:
 *      Helper function to bcm_esw_vlan_translate_delete to delete vlan 
 *      translation for Firebolt family
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr can be wildcarded (port = -1)
 *      old_vid - Old VLAN ID to delete translation for can be wildcarded (old_vid = BCM_VLAN_NONE)
 *      xtable  - Memory table
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int
_bcm_fb_vlan_translate_delete(int unit, int port, bcm_vlan_t old_vid,
                              int xtable)
{
    soc_mem_t           mem;
    int                 addvid, index_min, index_max, rv;
    int                 nmatch, nwrongadd, i;
    vlan_xlate_entry_t  vtempty;
    uint32              *vtcachep, vtcache;
    uint32              ve_valid, ve_port, ve_vid, ve_add;

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    switch (xtable) {
    case BCM_VLAN_XLATE_ING:
        mem = VLAN_XLATEm;
        addvid = 0;
        break;
    case BCM_VLAN_XLATE_EGR:
        mem = EGR_VLAN_XLATEm;
        addvid = -1;
        break;
    case BCM_VLAN_XLATE_DTAG:
        mem = VLAN_XLATEm;
        addvid = 1;
        break;
    default:
        return BCM_E_INTERNAL;
    }

#ifdef BCM_KATANA2_SUPPORT
    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, port)) ||
         (soc_feature(unit, soc_feature_subtag_coe) &&
          BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, port))) {
    } else
#endif
    if (port >= 0 && !SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    soc_mem_lock(unit, mem);

    /* get vt cache */
    if (mem == EGR_VLAN_XLATEm) {
        vtcachep = vlan_info[unit].egr_trans;
    } else {
        vtcachep = vlan_info[unit].ing_trans;
    }
    if (vtcachep == NULL) {             /* vt cache empty */
        soc_mem_unlock(unit, mem);
        return BCM_E_NOT_FOUND;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    rv = BCM_E_NOT_FOUND;
    sal_memset(&vtempty, 0, sizeof(vtempty));

    /* search vt cache for matches */
    nmatch = nwrongadd = 0;
    for (i = index_min; i <= index_max; i++) {
        vtcache = vtcachep[i];
        ve_valid = BCM_VTCACHE_VALID_GET(vtcache);
        if (!ve_valid) {
            continue;
        }
        if (port >= 0) {
            ve_port = BCM_VTCACHE_PORT_GET(vtcache);
            if ((int)ve_port != port) {
                continue;
            }
        }
        if (old_vid != BCM_VLAN_NONE) {
            ve_vid = BCM_VTCACHE_VID_GET(vtcache);
            if (ve_vid != old_vid) {
                continue;
            }
        }
        if (addvid >= 0) {
            ve_add = BCM_VTCACHE_ADD_GET(vtcache);
            if ((int)ve_add != addvid) {
                nwrongadd += 1;
                continue;
            }
        }

        /* matched entry, delete it */
        nmatch += 1;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &vtempty);
        if (rv < 0) {
            break;
        }
        vtcachep[i] = 0;
    }

    soc_mem_unlock(unit, mem);
    if (nmatch == 0 && nwrongadd == 1 &&
        port >= 0 && old_vid != BCM_VLAN_NONE) {
        rv = BCM_E_EXISTS;      /* specific delete that only missed addvid */
    }
    return rv;
}

/*
 * Function:
 *      _bcm_fb_vlan_translate_delete
 * Purpose:
 *      Helper function to bcm_esw_vlan_translate_delete to delete vlan 
 *      translation for Firebolt family
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr can be wildcarded (port = -1)
 *      old_vid - Old VLAN ID to delete translation for can be wildcarded (old_vid = BCM_VLAN_NONE)
 *      xtable  - Memory table
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
STATIC int 
_bcm_fb_vlan_translate_get_cb(int unit, void *trvs_info, int *stop)
{

    _bcm_vlan_translate_data_t      *get_info;
    _bcm_vlan_translate_traverse_t  *trvs_str;
    bcm_vlan_t                      old_vlan, new_vlan;
    bcm_port_t                      port;
    bcm_module_t                    modid;
    bcm_trunk_t                     trunk;
    int                             id;

    if ((NULL == trvs_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trvs_info;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }

    get_info = (_bcm_vlan_translate_data_t *)trvs_str->user_data;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, trvs_str->gport, &modid, &port, 
                               &trunk, &id));

    if ((port == get_info->port) && (old_vlan == get_info->old_vlan)) {
        *(get_info->new_vlan) = new_vlan;
        *(get_info->prio) = trvs_str->action->priority;
        *stop = TRUE;
    } else {
        *stop = FALSE;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_fb_vlan_translate_delete
 * Purpose:
 *      Helper function to bcm_esw_vlan_translate_delete to delete vlan 
 *      translation for Firebolt family
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr can be wildcarded (port = -1)
 *      old_vid - Old VLAN ID to delete translation for can be wildcarded (old_vid = BCM_VLAN_NONE)
 *      xtable  - Memory table
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int
_bcm_fb_vlan_translate_get(int unit, int port, bcm_vlan_t old_vid,
                           bcm_vlan_t *new_vid, int *prio, int xtable)
{
    _bcm_vlan_translate_data_t          get_info;
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    soc_mem_t                           mem;
    bcm_vlan_t                          new_vlan;
    int                                 priority;

    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return BCM_E_PORT;
    }
    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_UNAVAIL;
    }

    if ((NULL == new_vid) || (NULL == prio)){
        return BCM_E_PARAM;
    }
    
    sal_memset(&get_info, 0, sizeof(_bcm_vlan_translate_data_t));
    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    
    new_vlan = BCM_VLAN_INVALID;
    priority = -1;
    get_info.old_vlan = old_vid;
    get_info.port = port;
    get_info.new_vlan = &new_vlan;
    get_info.prio = &priority;

    trvs_st.user_data = (void *)&get_info;
    trvs_st.action = &action;
    trvs_st.user_cb_st = NULL;
    trvs_st.int_cb = _bcm_fb_vlan_translate_get_cb;

    switch (xtable) {
    case BCM_VLAN_XLATE_ING:
        mem = VLAN_XLATEm;
        break;
    case BCM_VLAN_XLATE_EGR:
        mem = EGR_VLAN_XLATEm;
        break;
    case BCM_VLAN_XLATE_DTAG:
        mem = VLAN_XLATEm;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(
    _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st));
    if ((BCM_VLAN_INVALID != new_vlan) && (-1 != priority) ){
        *new_vid = new_vlan;
        *prio = priority;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function   :
 *      _bcm_fb_vlan_translate_parse
 * Description   :
 *      Helper function for an API to parse a vlan translate 
 *      entry for Firebolt and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN/OUT) Traverse structure that contain all relevant info
 */
int 
_bcm_fb_vlan_translate_parse(int unit, soc_mem_t mem, uint32* vent,
                             _bcm_vlan_translate_traverse_t *trvs_info)
{
    bcm_port_t  port;
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return rv;
    }

    port = soc_mem_field32_get(unit, mem , vent, PORTf);
    rv = bcm_esw_port_gport_get(unit, port, &(trvs_info->gport));
    BCM_IF_ERROR_RETURN(rv);

    trvs_info->outer_vlan = soc_mem_field32_get(unit, mem, vent, OLD_VLAN_IDf);
    trvs_info->action->new_outer_vlan = soc_mem_field32_get(unit, mem, vent, 
                                                            NEW_VLAN_IDf);
    trvs_info->action->priority = soc_mem_field32_get(unit, mem, vent, PRIf);
    return (rv);
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAPTOR_SUPPORT)
#ifndef BCM_SW_STATE_DUMP_DISABLE
void _bcm_fb2_vlan_profile_sw_dump(int unit)
{
    int index;
    int index_max;
    int outer_tpid, flags;
    int ip4_mcast_mode, ip6_mcast_mode, l2_mcast_mode, ref_count;
    int outer_tpid_entry, outer_tpid_ref_count;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information VLAN Profile - Unit %d\n"), unit));

    index_max = soc_mem_index_max(unit, VLAN_PROFILE_TABm);
    for (index = 0; index <= index_max; index++) {
        outer_tpid = VLAN_PROFILE_ENTRY(unit, index).outer_tpid;
        flags = VLAN_PROFILE_ENTRY(unit, index).flags;
        ip6_mcast_mode = VLAN_PROFILE_ENTRY(unit, index).ip6_mcast_flood_mode;
        ip4_mcast_mode = VLAN_PROFILE_ENTRY(unit, index).ip4_mcast_flood_mode;
        l2_mcast_mode = VLAN_PROFILE_ENTRY(unit, index).l2_mcast_flood_mode;
        ref_count = VLAN_PROFILE_REF_COUNT (unit, index);

        if (outer_tpid == 0 && flags == 0 &&
            ip6_mcast_mode == 0 && ip4_mcast_mode == 0 &&
            l2_mcast_mode == 0 && ref_count == 0) {
            continue;
        } 
        LOG_CLI((BSL_META_U(unit, " Index = 0x%x, "), index));
        LOG_CLI((BSL_META_U(unit, " TPID = 0x%x, "), outer_tpid));
        LOG_CLI((BSL_META_U(unit, " Flags = 0x%x, "), flags));
        LOG_CLI((BSL_META_U(unit, " IP6 PFM = 0x%x, "), ip6_mcast_mode));
        LOG_CLI((BSL_META_U(unit, " IP4 PFM = 0x%x, "), ip4_mcast_mode));
        LOG_CLI((BSL_META_U(unit, " L2 PFM = 0x%x, "), l2_mcast_mode));
        LOG_CLI((BSL_META_U(unit, " Ref count  = %d\n"), ref_count));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information outer TPIDs - Unit %d\n"), unit));

    for (index = 0; index < BCM_MAX_TPID_ENTRIES; index++) {
        outer_tpid_entry = OUTER_TPID_ENTRY(unit, index);
        outer_tpid_ref_count = OUTER_TPID_REF_COUNT(unit, index);

        if (outer_tpid_entry == 0 &&
            outer_tpid_ref_count == 0) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit, " Index = 0x%x, "), index));
        LOG_CLI((BSL_META_U(unit, " TPID = 0x%x, "), outer_tpid_entry));
        LOG_CLI((BSL_META_U(unit, " Ref count = %d\n"), outer_tpid_ref_count));
    }
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT \
          || BCM_RAPTOR_SUPPORT */ 
