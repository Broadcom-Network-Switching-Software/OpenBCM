/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        fcoe.c
 * Purpose:     Trident2 FCoE functions
 */
#include <soc/defs.h>

#if defined(BCM_TRIDENT2_SUPPORT)

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/error.h>
#include <bcm/fcoe.h>
#include <bcm/switch.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/fcoe.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/qos.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trx.h>
#include <shared/idxres_fl.h>
#include <soc/scache.h>

#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
typedef uint32 L2_ENTRY_T[SOC_MAX_MEM_FIELD_WORDS];

/* VSAN resource manager */
STATIC shr_idxres_list_handle_t vsan_id_list[BCM_MAX_NUM_UNITS] = {NULL};

#define FCOE_MAX_VSAN_ID       0xfff
#define FCOE_MAX_SRC_DEST_ID   0xffffff
#define FCOE_ZONE_MAX_CLASS_ID 0x3f

#ifdef BCM_WARM_BOOT_SUPPORT
/* VSAN ID reserve state */
#define FCOE_VSAN_ID_BYTES     512   /* 4096 IDs, 1 bits per id = 512 bytes */
typedef struct {
    uint8 state[FCOE_VSAN_ID_BYTES];  /* 1 bits per VSAN ID */
} _vsan_id_reserve_state;

STATIC _vsan_id_reserve_state _vsan_id_state[BCM_MAX_NUM_UNITS];

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1

#endif

typedef int (*fcoe_traverse_cb_f)(int unit, soc_mem_t mem, uint32 *buf,
                                  bcm_fcoe_route_t *route);

#define VFT_NUM_ENTRIES  1  /* 1 entry per set in EGR_VFT_FIELDS_PROFILE */
#define VFT_NUM_PROFILES 8  /* 8 profiles available in EGR_VFT_FIELDS_PROFILE */

/* Hardware values for FC_HDR_ENCODE field defined in architecture spec */
#define FC_HEADER_TYPE_NOT_PRESENT   0
#define FC_HEADER_TYPE_STANDARD      1
#define FC_HEADER_TYPE_VFT           2
#define FC_HEADER_TYPE_ENCAPSULATION 3
#define FC_HEADER_TYPE_IFR           4
#define FC_HEADER_TYPE_UNKNOWN       6

/* Values for EGR_L3_INTF.FCOE_VFT_ACTION */
#define FCOE_VFT_ACTION_DO_NOT_MODIFY       0
#define FCOE_VFT_ACTION_ADD_REPLACE         1
#define FCOE_VFT_ACTION_DELETE              2

/* Values for EGR_L3_INTF.FCOE_FABRIC_ID_SEL */
#define FCOE_FABRIC_ID_SEL_SET              0
#define FCOE_FABRIC_ID_SEL_INTERNAL         1
#define FCOE_FABRIC_ID_SEL_INNER_VLAN       2
#define FCOE_FABRIC_ID_SEL_OUTER_VLAN       3

/* Values for EGR_L3_INTF.FCOE_FABRIC_PRI_SEL */
#define FCOE_FABRIC_PRI_SEL_SET             0
#define FCOE_FABRIC_PRI_SEL_INTERNAL        1
#define FCOE_FABRIC_PRI_SEL_REMARK          2
#define FCOE_ROUTE_READ_RETURN_L3_INTF 0
#define FCOE_ROUTE_READ_RETURN_NHID 1

#define CHECK_FLAGS_NOT_SET(flags, values) \
    if (((flags) & (values)) != 0) { \
        return BCM_E_PARAM; \
    }

typedef struct _bcm_td2_fcoe_bookkeeping_s {
    uint8 profile_use_refcnt[VFT_NUM_PROFILES];
} _bcm_td2_fcoe_bookkeeping_t;

static _bcm_td2_fcoe_bookkeeping_t _bcm_td2_fcoe_bk_info[BCM_MAX_NUM_UNITS];

/*
 * Function:    bcm_td2_fcoe_init
 * Purpose:     Initialize the FCOE module.
 * Parameters:  unit - SOC unit number.
 * Returns:     BCM_E_XXX
 */
int
bcm_td2_fcoe_init(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 fcoe_scache_size;
    soc_scache_handle_t scache_handle;
    uint8               *fcoe_scache_ptr;
#endif
    int rv = BCM_E_NONE;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return _bcm_td2_fcoe_reinit(unit);
    }
#endif

    /* create vsan id list manager */
    rv = shr_idxres_list_create(&vsan_id_list[unit], 0, FCOE_MAX_VSAN_ID,
                                0, FCOE_MAX_VSAN_ID, "VSAN ID");
    if (rv != BCM_E_NONE) {
        return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT

    memset(&_vsan_id_state[unit], 0, sizeof(_vsan_id_reserve_state));

    fcoe_scache_size = sizeof(_vsan_id_reserve_state) * BCM_MAX_NUM_UNITS;

    fcoe_scache_size += sizeof(_bcm_td2_fcoe_bookkeeping_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FCOE, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 fcoe_scache_size, &fcoe_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

#endif

    memset(&_bcm_td2_fcoe_bk_info[unit].profile_use_refcnt, 0, 
           sizeof _bcm_td2_fcoe_bk_info[unit].profile_use_refcnt);

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

STATIC int
_bcm_td2_vsan_id_creation_state_get(int unit, int vsan_id)
{
    uint32 byte_index;
    uint32 bit_index;
    uint8  state_val;
    uint8  tmp = 0;

    /* find corresponding bit for vsan id */
    byte_index = vsan_id / 8;
    bit_index = vsan_id % 8;
    tmp = 1 << bit_index;
    state_val = _vsan_id_state[unit].state[byte_index];

    if ((state_val & tmp) != 0) {   /* state bit is 1 */
        return 1;
    } else {
        return 0;
    }
}

STATIC void
_bcm_td2_vsan_id_creation_state_set(int unit, int vsan_id, int state)
{
    uint32 byte_index;
    uint32 bit_index;
    uint8  state_val;
    uint8  tmp = 0;

    /* find corresponding bit for vsan id */
    byte_index = vsan_id / 8;
    bit_index = vsan_id % 8;
    tmp = 1 << bit_index;
    state_val = _vsan_id_state[unit].state[byte_index];

    if (state == 0) { 
        /* clear state */
        state_val = state_val & ~tmp;

    } else {
        /* set state */
        state_val = state_val | tmp;
    }
    /* update table */
    _vsan_id_state[unit].state[byte_index] = state_val;
}

STATIC int
_bcm_td2_recreate_reserved_entries(int unit)
{
    uint32 vsan_id;
    int    rv;
    int index;
    soc_mem_t mem = ING_VSANm;
    ing_vsan_entry_t vsan_entry;

    /* release VSAN list manager */
    if (vsan_id_list[unit] != NULL) {
        /* the following can be failed if we have stale pointer. Igonre it */
        (void)shr_idxres_list_destroy(vsan_id_list[unit]);
        vsan_id_list[unit] = NULL;
    }

    /* re-create vsan id list manager */
    rv = shr_idxres_list_create(&vsan_id_list[unit], 0, FCOE_MAX_VSAN_ID,
                                0, FCOE_MAX_VSAN_ID, "VSAN ID");
    if (rv != BCM_E_NONE) {
        return rv;
    }

    for (vsan_id = 0; vsan_id <= FCOE_MAX_VSAN_ID; vsan_id++) {
        if (_bcm_td2_vsan_id_creation_state_get(unit, vsan_id) != 0) {
            /*reserve entry */
            BCM_IF_ERROR_RETURN(
                shr_idxres_list_reserve(vsan_id_list[unit], vsan_id, vsan_id));

            /* Recover FC_MAP_PROFILE references */
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, vsan_id, &vsan_entry));
            index = soc_mem_field32_get(unit,mem, &vsan_entry,
                                        FCOE_FC_MAP_INDEXf);
            BCM_IF_ERROR_RETURN(
                _bcm_fc_map_profile_entry_reference(unit, index, 1));
        }
    }
    return BCM_E_NONE;
}

int
_bcm_td2_fcoe_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8               *fcoe_scache_ptr;
    int                 rv;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FCOE, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, 
                                 &fcoe_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    /* Note : If no storage is configured, the above function will return
     *        with BCM_E_NOT_FOUND. This is the correct behavior for
     *        UNSYNCHRONIZED mode.
     */

    if (rv == BCM_E_NONE) {
        /* copy data to cache */
        sal_memcpy(fcoe_scache_ptr, &_vsan_id_state[unit], 
                   sizeof(_vsan_id_reserve_state));
        fcoe_scache_ptr += sizeof(_vsan_id_reserve_state) * BCM_MAX_NUM_UNITS;

        /* BCM_WB_VERSION_1_1 */
        sal_memcpy(fcoe_scache_ptr, &_bcm_td2_fcoe_bk_info[unit],
                   sizeof(_bcm_td2_fcoe_bookkeeping_t));
        fcoe_scache_ptr += sizeof(_bcm_td2_fcoe_bookkeeping_t);
    }

    return BCM_E_NONE;
}

int
_bcm_td2_fcoe_reinit(int unit)
{
    int rv;
    soc_scache_handle_t scache_handle;
    uint8               *fcoe_scache_ptr;
    uint16 recovered_ver = 0;
    uint32 additional_scache_size = 0;
    int index;
    int refcnt = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FCOE, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, 
                                 &fcoe_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (rv == BCM_E_NOT_FOUND) {
        fcoe_scache_ptr = NULL;

    } else if (SOC_FAILURE(rv)) {
        return rv;
    } else {
        /* success, recover data (copy data from cache) */
        sal_memcpy(&_vsan_id_state[unit], fcoe_scache_ptr, 
                   sizeof(_vsan_id_reserve_state));
        fcoe_scache_ptr += sizeof(_vsan_id_reserve_state) * BCM_MAX_NUM_UNITS;
        if (recovered_ver >=  BCM_WB_VERSION_1_1) {
            sal_memcpy(&_bcm_td2_fcoe_bk_info[unit], fcoe_scache_ptr,
                       sizeof(_bcm_td2_fcoe_bookkeeping_t));
            fcoe_scache_ptr += sizeof(_bcm_td2_fcoe_bookkeeping_t);
            for (index = 0; index < VFT_NUM_PROFILES; index++) {
                if (_bcm_td2_fcoe_bk_info[unit].profile_use_refcnt[index]) {
                    refcnt = _bcm_td2_fcoe_bk_info[unit].profile_use_refcnt[index];
                    while (refcnt--) {
                        _bcm_egr_vft_fields_entry_reference(unit, index, 1);
                    }
                }
            }
        } else {
            additional_scache_size += sizeof(_bcm_td2_fcoe_bookkeeping_t);
        }
        if (additional_scache_size > 0) {
            SOC_IF_ERROR_RETURN
                (soc_scache_realloc(unit, scache_handle, additional_scache_size));
        }

        BCM_IF_ERROR_RETURN(_bcm_td2_recreate_reserved_entries(unit));
    }

    return BCM_E_NONE;
}

#endif

/*
 * Function:    bcm_td2_fcoe_cleanup
 * Purpose:     Cleanup the FCOE module.
 * Parameters:  unit - SOC unit number.
 * Returns:     BCM_E_XXX
 */
int
bcm_td2_fcoe_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    /* free vsan id list manager */
    if (vsan_id_list[unit] != NULL) {
        rv = shr_idxres_list_destroy(vsan_id_list[unit]);

        if (rv != BCM_E_NONE) {
            return rv;
        } else {
            vsan_id_list[unit] = NULL;
        }
    }

    

    return rv;
}

/* mark vsan ID as Reserved */
STATIC int
_bcm_fcoe_reserve_vsan_id (int unit, uint32 vsan_id)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);

    /* mark as created */
    _bcm_td2_vsan_id_creation_state_set(unit, vsan_id, 1);
#endif

    return shr_idxres_list_reserve(vsan_id_list[unit], vsan_id, vsan_id);
}

/* unreserve vsan ID */
STATIC int
_bcm_fcoe_unreserve_vsan_id (int unit, uint32 vsan_id)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);

    _bcm_td2_vsan_id_creation_state_set(unit, vsan_id, 0);
#endif

    return shr_idxres_list_free(vsan_id_list[unit], vsan_id);
}

STATIC int
_bcm_fcoe_zone_validate(bcm_fcoe_zone_entry_t *zone)
{
    if ((zone->vsan_id > FCOE_MAX_VSAN_ID)        ||  /* 12 bits */
        (zone->d_id > FCOE_MAX_SRC_DEST_ID)       ||  /* 24 bits */
        (zone->s_id > FCOE_MAX_SRC_DEST_ID)       ||  /* 24 bits */
        (zone->class_id < 0)                      ||
        (zone->class_id > FCOE_ZONE_MAX_CLASS_ID) ||  /* 6 bits */
        (zone->action > bcmFcoeZoneActionRedirect)) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Purpose:
 *      Add FCoE zoning entry
 * Parameters:
 *      unit       - Device unit number
 *      zone       - zone entry
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_add(int unit, bcm_fcoe_zone_entry_t *zone)
{
    L2_ENTRY_T  l2_entry;
    L2_ENTRY_T  l2_lookup;
    soc_mem_t   mem;
    soc_field_t field_key;
    int         rv = BCM_E_NONE;
    int         l2_index = 0, valid_f;

    if (_bcm_fcoe_zone_validate(zone) != BCM_E_NONE) {
        return BCM_E_PARAM;
    }

    sal_memset(&l2_entry, 0, sizeof (l2_entry));
    sal_memset(&l2_lookup, 0, sizeof (l2_lookup));

    mem = L2Xm;
    valid_f = VALIDf;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) 
        valid_f = BASE_VALIDf;
#endif

    field_key = TD2_L2_HASH_KEY_TYPE_FCOE_ZONE;

    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        valid_f, 1);                             /* [1] */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 KEY_TYPEf, field_key);         /* fcoe type */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__VSAN_IDf, zone->vsan_id);    /* vsan ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__D_IDf, zone->d_id);          /* D_ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__S_IDf, zone->s_id);          /* S_ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__CLASS_IDf, zone->class_id);  /* CLASS_ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
    if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
        soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                            FCOE_ZONE__STATIC_DUPLICATEf, 1);
    }
#endif /* BCM_HURRICANE4_SUPPORT */

    /*
     * set action field  & copy_to_cpu field
     */
    switch (zone->action) {
        case bcmFcoeZoneActionAllow:
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                FCOE_ZONE__ACTIONf, 1);
            break;
        case bcmFcoeZoneActionDeny:
            /* action field is already 0 */
            break;
        case bcmFcoeZoneActionCopyToCpu:
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                FCOE_ZONE__CPUf, 1);
            } else 
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                    FCOE_ZONE__COPY_TO_CPUf, 1);
            }
            break;
        case bcmFcoeZoneActionRedirect:
            /* fallthru */
        default:
            return BCM_E_UNAVAIL;
            break;
    }

    /* See if the entry already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);

    if (rv == SOC_E_NONE) {
        /*
         * entry already exist. (l2_index gets the location)
         * rewrite using new data
         */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, l2_index, &l2_entry);
    } else if (rv == SOC_E_NOT_FOUND) {
        /* Not found,  add entry */
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)&l2_entry);
    } else {
        return rv;
    }
    return rv;
}


/*
 * Purpose:
 *      Delete Zone check entry
 * Parameters:
 *      unit       - Device unit number
 *      zone       - zone entry to be deleted
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_delete(int unit, bcm_fcoe_zone_entry_t *zone)
{
    L2_ENTRY_T  l2_entry;
    L2_ENTRY_T  l2_lookup;
    soc_mem_t   mem;
    soc_field_t field_key;
    int         rv = BCM_E_NONE;
    int         l2_index = 0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));
    sal_memset(&l2_lookup, 0, sizeof (l2_lookup));

    mem = L2Xm;

    field_key = TD2_L2_HASH_KEY_TYPE_FCOE_ZONE;

    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 KEY_TYPEf, field_key);         /* fcoe type */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__VSAN_IDf, zone->vsan_id);    /* vsan ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__D_IDf, zone->d_id);          /* D_ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__S_IDf, zone->s_id);          /* S_ID */


    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);

    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)&l2_entry);
    return rv;
}


/*
 * Description:
 *      Delete all zone check entries
 * Parameters:
 *     unit             - device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_delete_all (int unit)
{
    int           ix;
    int           key_type;
    int           chnk_end;
    int           chunksize;
    int           chunk;
    int           chunk_max;
    int            mem_idx_max;
    L2_ENTRY_T    *l2e;
    L2_ENTRY_T    *l2ep;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t     mem;
    int           rv = BCM_E_NONE, valid_f;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    mem = L2Xm;
    valid_f  = VALIDf;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) 
        valid_f = BASE_VALIDf;
#endif

    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");

    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;

        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }

        chnk_end = (chunk_max - chunk);

        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
            if (!soc_mem_field32_get(unit, mem, l2ep, valid_f)) {
                continue;
            }

            /* we won't check set/check FCOE_ZONE__STATIC_BITf for now...
            if (soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__STATIC_BITf)) {
                continue;
            }*/

            /* Match the Key Type to FCOE */
            key_type = soc_mem_field32_get(unit, mem, l2ep, KEY_TYPEf);

            if (key_type != TD2_L2_HASH_KEY_TYPE_FCOE_ZONE) {
                continue;
            }

            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }

            if (((rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l2ep)) < 0) ||
                ((rv = soc_l2x_sync_delete(unit, (uint32 *) l2ep,
                                           (chunk + ix), 0)) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }

            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }

        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);

    return rv;
}


/*
 * Description:
 *      Delete all zone check entries matching VSAN ID
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry with VSAN ID to be deleted
 * Return:
 *      BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_delete_by_vsan (int unit, bcm_fcoe_zone_entry_t *zone)
{
    int           ix;
    int           key_type;
    int           chnk_end;
    int           chunksize;
    int           chunk;
    int           chunk_max;
    int            mem_idx_max;
    L2_ENTRY_T    *l2e;
    L2_ENTRY_T    *l2ep;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t     mem;
    int           rv = BCM_E_NONE, valid_f;
    uint32        vsan_id;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    mem = L2Xm;
    valid_f = VALIDf;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) 
        valid_f = BASE_VALIDf;
#endif

    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");

    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;

        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }

        chnk_end = (chunk_max - chunk);

        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
            if (!soc_mem_field32_get(unit, mem, l2ep, valid_f)) {
                continue;
            }

            /* we won't check set/check FCOE_ZONE__STATIC_BITf for now...
            if (soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__STATIC_BITf)) {
                continue;
            }*/

            /* Match the Key Type to FCOE */
            key_type = soc_mem_field32_get(unit, mem, l2ep, KEY_TYPEf);

            if (key_type != TD2_L2_HASH_KEY_TYPE_FCOE_ZONE) {
                continue;
            }

            /* match vsan id */
            vsan_id = soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__VSAN_IDf);

            if (vsan_id != zone->vsan_id) {
                continue;
            }

            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }

            if (((rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l2ep)) < 0) ||
                ((rv = soc_l2x_sync_delete(unit, (uint32 *) l2ep,
                                           (chunk + ix), 0)) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }

            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }

        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);

    return rv;
}


/*
 * Description:
 *      Delete all zone check entries matching Source ID
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry with S_ID to be deleted
 * Return:
 *      BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_delete_by_sid (int unit, bcm_fcoe_zone_entry_t *zone)
{
    int           ix;
    int           key_type;
    int           chnk_end;
    int           chunksize;
    int           chunk;
    int           chunk_max;
    int            mem_idx_max;
    L2_ENTRY_T    *l2e;
    L2_ENTRY_T    *l2ep;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t     mem;
    int           rv = BCM_E_NONE, valid_f;
    uint32        s_id;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    mem = L2Xm;
    valid_f = VALIDf;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit))
        valid_f = BASE_VALIDf;
#endif

    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");

    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;

        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }

        chnk_end = (chunk_max - chunk);

        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
            if (!soc_mem_field32_get(unit, mem, l2ep, valid_f)) {
                continue;
            }

            /* we won't check set/check FCOE_ZONE__STATIC_BITf for now...
            if (soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__STATIC_BITf)) {
                continue;
            }*/

            /* Match the Key Type to FCOE */
            key_type = soc_mem_field32_get(unit, mem, l2ep, KEY_TYPEf);

            if (key_type != TD2_L2_HASH_KEY_TYPE_FCOE_ZONE) {
                continue;
            }

            /* match s_id */
            s_id = soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__S_IDf);

            if (s_id != zone->s_id) {
                continue;
            }

            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }

            if (((rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l2ep)) < 0) ||
                ((rv = soc_l2x_sync_delete(unit, (uint32 *) l2ep,
                                           (chunk + ix), 0)) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }

            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }

        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);

    return rv;
}


/*
 * Description:
 *      Delete all zone check entries matching Destination ID
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry with D_ID to be deleted
 * Return:
 *      BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_delete_by_did (int unit, bcm_fcoe_zone_entry_t *zone)
{
    int           ix;
    int           key_type;
    int           chnk_end;
    int           chunksize;
    int           chunk;
    int           chunk_max;
    int            mem_idx_max;
    L2_ENTRY_T    *l2e;
    L2_ENTRY_T    *l2ep;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t     mem;
    int           rv = BCM_E_NONE, valid_f;
    uint32        d_id;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    mem = L2Xm;
    valid_f = VALIDf;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit))
        valid_f = BASE_VALIDf;
#endif

    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");

    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;

        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }

        chnk_end = (chunk_max - chunk);

        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
            if (!soc_mem_field32_get(unit, mem, l2ep, valid_f)) {
                continue;
            }

            /* we won't check set/check FCOE_ZONE__STATIC_BITf for now...
            if (soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__STATIC_BITf)) {
                continue;
            }*/

            /* Match the Key Type to FCOE */
            key_type = soc_mem_field32_get(unit, mem, l2ep, KEY_TYPEf);

            if (key_type != TD2_L2_HASH_KEY_TYPE_FCOE_ZONE) {
                continue;
            }

            /* match d_id */
            d_id = soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__D_IDf);

            if (d_id != zone->d_id) {
                continue;
            }

            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }

            if (((rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l2ep)) < 0) ||
                ((rv = soc_l2x_sync_delete(unit, (uint32 *) l2ep,
                                           (chunk + ix), 0)) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }

            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }

        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);

    return rv;
}


/*
 * Description:
 *      Retrieve Zone info
 * Parameters:
 *      unit       - device number
 *      zone       - zone entry
 * Return:
 *      BCM_E_XXX
 */
int
bcm_td2_fcoe_zone_get (int unit, bcm_fcoe_zone_entry_t *zone)
{
    L2_ENTRY_T  l2_entry;
    L2_ENTRY_T  l2_lookup;
    soc_mem_t   mem;
    soc_field_t field_key;
    int         rv = BCM_E_NONE;
    int         l2_index = 0;
    uint32      action;
    uint32      copy_to_cpu;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));
    sal_memset(&l2_lookup, 0, sizeof (l2_lookup));

    mem = L2Xm;

    field_key = TD2_L2_HASH_KEY_TYPE_FCOE_ZONE;

    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 KEY_TYPEf, field_key);         /* fcoe type */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__VSAN_IDf, zone->vsan_id);    /* vsan ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__D_IDf, zone->d_id);          /* D_ID */
    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                        FCOE_ZONE__S_IDf, zone->s_id);          /* S_ID */


    /* See if the entry exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);

    if (rv == SOC_E_NONE) {
        /* entry found */
        zone->class_id = (int)soc_mem_field32_get(unit, mem,
                                                  (void *)&l2_lookup,
                                                  FCOE_ZONE__CLASS_IDf);
        action = soc_mem_field32_get(unit, mem, (void *)&l2_lookup,
                                     FCOE_ZONE__ACTIONf);
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            copy_to_cpu = soc_mem_field32_get(unit, mem, (void *)&l2_lookup,
                                          FCOE_ZONE__CPUf);
        } else 
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            copy_to_cpu = soc_mem_field32_get(unit, mem, (void *)&l2_lookup,
                                              FCOE_ZONE__COPY_TO_CPUf);
        }

        /*
         * now, combine action & copy_to_cpu and generate a
         * bcm_fcoe_zone_action_t.
         * It means bcmFcoeZoneActionAllow and bcmFcoeZoneActionCopyToCpu are
         * mutually exclusive. If not, zone action API definition need to be
         * changed. BTW, 'redirect' action is not defined in register spec...
         */
        if (action == 1) {
            zone->action = bcmFcoeZoneActionAllow;
        } else {  /* action is 0 (deny) */
            zone->action = bcmFcoeZoneActionDeny;
            if (copy_to_cpu == 1) {
                zone->action = bcmFcoeZoneActionCopyToCpu;
            }
        }
    }

    return rv;
}

#ifdef INCLUDE_L3

/*
 * Description:
 *     Allocate a next hop if needed or if incoming route->intf is an l3 egress
 *     object then retrieve the HW nh index
 * Parameters:
 *     unit      - (IN) device number
 *     route     - (IN) route information to be used
 *     index     - (OUT) Index of nexthop info added
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_add_nh_entry(int unit, bcm_fcoe_route_t *route, bcm_if_t *index)
{
    _bcm_l3_cfg_t l3cfg;
    int rv = BCM_E_NONE;

    memset(&l3cfg, 0, sizeof l3cfg);

    if (route->flags & BCM_FCOE_COPY_TO_CPU) {
        l3cfg.l3c_flags |= BCM_L3_L2TOCPU;
    }
    if (route->flags & BCM_FCOE_DST_DISCARD) {
        l3cfg.l3c_flags |= BCM_L3_DST_DISCARD;
    }
    if (route->flags & BCM_FCOE_SRC_DISCARD) {
        l3cfg.l3c_flags |= BCM_L3_SRC_DISCARD;
    }
    if (route->flags & BCM_FCOE_NO_HOP_COUNT_CHANGE) {
        l3cfg.l3c_flags |= BCM_L3_KEEP_TTL;
    }
    if (route->flags & BCM_FCOE_MULTIPATH) {
        l3cfg.l3c_ecmp = 1;
    }

    l3cfg.l3c_vrf = route->vrf;
    l3cfg.l3c_port_tgid = BCM_GPORT_MODPORT_PORT_GET(route->port);
    l3cfg.l3c_modid = BCM_GPORT_MODPORT_MODID_GET(route->port);
    l3cfg.l3c_prio = route->pri;
    l3cfg.l3c_lookup_class = route->lookup_class;
    l3cfg.l3c_intf = route->intf;

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        l3cfg.l3c_flags2 |= BCM_L3_FLAGS2_FCOE_ONLY;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    memcpy(&l3cfg.l3c_mac_addr, route->nexthop_mac, sizeof route->nexthop_mac);
    rv = _bcm_xgs3_nh_init_add(unit, &l3cfg, NULL, index);

    return rv;
}

/*
 * Description:
 *     Create a key for the L3 table (FCOE_EXT view) based on the incoming
 *     route structure
 * Parameters:
 *     unit      - (IN) device number
 *     mem       - (IN) memory to be used
 *     table_row - (OUT) buffer containing a table row to modify
 *     route     - (IN) route information to be used
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_route_construct_key_ext(int unit, soc_mem_t mem, uint32 *table_row,
                                  bcm_fcoe_route_t *route)
{
    int key_type;
    int key_field;
    int key_value;

    if (route->vrf >= SOC_VRF_MAX(unit) || route->vrf == 0)
        return BCM_E_PARAM;

    soc_mem_field32_set(unit, mem, table_row, FCOE_EXT__VRF_IDf, route->vrf);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        int data_type;
        if (route->flags & BCM_FCOE_HOST_ROUTE) {
            key_type  = (int)TD3_L3_HASH_KEY_TYPE_FCOE_HOST_EXT;
            data_type = (int)TD3_L3_HASH_DATA_TYPE_FCOE_HOST_EXT;
            key_field = FCOE_EXT__D_IDf;
            key_value = route->nport_id;
        } else if (route->flags & BCM_FCOE_SOURCE_ROUTE) {
            key_type  = (int)TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP_EXT;
            data_type = (int)TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP_EXT;
            key_field = FCOE_EXT__S_IDf;
            key_value = route->nport_id;
        } else if (route->flags & BCM_FCOE_DOMAIN_ROUTE) {
            key_type  = (int)TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT;
            data_type = (int)TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN_EXT;
            key_field = FCOE_EXT__MASKED_D_IDf;
            key_value = route->nport_id & route->nport_id_mask;
        } else {
            /* route must be either host, source, or domain */
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf, key_type);
        soc_mem_field32_set(unit, mem, table_row, DATA_TYPEf, data_type);
        soc_mem_field32_set(unit, mem, table_row, key_field, key_value);
        soc_mem_field32_set(unit, mem, table_row, BASE_VALID_0f, 1);
        soc_mem_field32_set(unit, mem, table_row, BASE_VALID_1f, 7);
    } else
#endif
    {
        if (route->flags & BCM_FCOE_HOST_ROUTE) {
            key_type  = (int)TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT;
            key_field = FCOE_EXT__D_IDf;
            key_value = route->nport_id;
        } else if (route->flags & BCM_FCOE_SOURCE_ROUTE) {
            key_type  = (int)TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT;
            key_field = FCOE_EXT__S_IDf;
            key_value = route->nport_id;
        } else if (route->flags & BCM_FCOE_DOMAIN_ROUTE) {
            key_type  = (int)TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT;
            key_field = FCOE_EXT__MASKED_D_IDf;
            key_value = route->nport_id & route->nport_id_mask;
        } else {
            /* route must be either host, source, or domain */
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, table_row, KEY_TYPE_0f, key_type);
        soc_mem_field32_set(unit, mem, table_row, KEY_TYPE_1f, key_type);
        soc_mem_field32_set(unit, mem, table_row, key_field, key_value);
        soc_mem_field32_set(unit, mem, table_row, VALID_0f, 1);
        soc_mem_field32_set(unit, mem, table_row, VALID_1f, 1);
    }

    return BCM_E_NONE;
}

/*
 * Description:
 *     Construct an L3 table key (FCOE view) from the given route
 * Parameters:
 *     unit      - (IN) device number
 *     mem       - (IN) memory (table) used
 *     table_row - (OUT) buffer holding a table row
 *     index     - (IN) Index of nexthop info added
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_route_construct_key_normal(int unit, soc_mem_t mem, uint32 *table_row,
                                     bcm_fcoe_route_t *route)
{
    if (route->vrf >= SOC_VRF_MAX(unit) || route->vrf == 0)
        return BCM_E_PARAM;

    soc_mem_field32_set(unit, mem, table_row, FCOE__VRF_IDf, route->vrf);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        if (route->flags & BCM_FCOE_HOST_ROUTE) {
            soc_mem_field32_set(unit, mem, table_row, DATA_TYPEf,
                                TD3_L3_HASH_DATA_TYPE_FCOE_HOST);
            soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf,
                                TD3_L3_HASH_KEY_TYPE_FCOE_HOST);
            soc_mem_field32_set(unit, mem, table_row, FCOE__D_IDf,
                                route->nport_id);
        } else if (route->flags & BCM_FCOE_SOURCE_ROUTE) {
            soc_mem_field32_set(unit, mem, table_row, DATA_TYPEf,
                                TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP);
            soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf,
                                TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP);
            soc_mem_field32_set(unit, mem, table_row, FCOE__S_IDf,
                                route->nport_id);
        } else if (route->flags & BCM_FCOE_DOMAIN_ROUTE) {
            soc_mem_field32_set(unit, mem, table_row, DATA_TYPEf,
                                TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN);
            soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf,
                                TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN);
            soc_mem_field32_set(unit, mem, table_row, FCOE__MASKED_D_IDf,
                                route->nport_id & route->nport_id_mask);
        } else {
            /* route must be either host, source, or domain */
            return BCM_E_PARAM;
        }
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        if (route->flags & BCM_FCOE_HOST_ROUTE) {
            soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf,
                                TD2_L3_HASH_KEY_TYPE_FCOE_HOST);
            soc_mem_field32_set(unit, mem, table_row, FCOE__D_IDf,
                                route->nport_id);
        } else if (route->flags & BCM_FCOE_SOURCE_ROUTE) {
            soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf,
                                TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP);
            soc_mem_field32_set(unit, mem, table_row, FCOE__S_IDf,
                                route->nport_id);
        } else if (route->flags & BCM_FCOE_DOMAIN_ROUTE) {
            soc_mem_field32_set(unit, mem, table_row, KEY_TYPEf,
                                TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN);
            soc_mem_field32_set(unit, mem, table_row, FCOE__MASKED_D_IDf,
                                route->nport_id & route->nport_id_mask);
        } else {
            /* route must be either host, source, or domain */
            return BCM_E_PARAM;
        }
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, mem, table_row, BASE_VALIDf, 1);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, table_row, VALIDf, 1);
    }

    return BCM_E_NONE;
}


/* 
 * _bcm_td2_fcoe_number_bits_set
 *   Return the number of bits set in the input parameter i. It works, trust me.
 */
STATIC int
_bcm_td2_fcoe_number_bits_set(uint32 i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

/*
 * _bcm_td2_fcoe_route_is_prefix:
 *    Return TRUE for domain prefix routes, and FALSE for all other routes
 */
STATIC int
_bcm_td2_fcoe_route_is_prefix(int unit, bcm_fcoe_route_t *route)
{
    int domainPrefixLength = 0;
    int routeBitsSet = 0;
    int rv = bcm_esw_switch_control_get(unit, 
                                        bcmSwitchFcoeDomainRoutePrefixLength,
                                        &domainPrefixLength);

    if (BCM_FAILURE(rv)) {
        return FALSE;
    }

    routeBitsSet = _bcm_td2_fcoe_number_bits_set(route->nport_id_mask);

    return (route->flags & BCM_FCOE_DOMAIN_ROUTE) 
           && (domainPrefixLength == routeBitsSet);
}

/*
 * Description:
 *     Convert modport-gport into the hardware specific global logical port
 * Parameters:
 *     port      - (IN) port
 *     glp       - (OUT) resulting global logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_port_to_glp(bcm_gport_t port, int *glp)
{
    int base_port = BCM_GPORT_MODPORT_PORT_GET(port);
    int base_modid = BCM_GPORT_MODPORT_MODID_GET(port);

    int port_id = (_BCM_TD2_L3_PORT_MASK & base_port);
    int modid   = (_BCM_TD2_L3_MODID_MASK & base_modid)
                  << _BCM_TD2_L3_MODID_SHIFT;

    if (!BCM_GPORT_IS_MODPORT(port)) {
        return BCM_E_PARAM;
    }

    *glp = (modid | port_id);
    return BCM_E_NONE;
}

/*
 * Description:
 *     Convert modport-gport into the hardware specific global logical port
 * Parameters:
 *     port      - (IN) port
 *     glp       - (OUT) resulting global logical port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_glp_to_port(int glp, bcm_gport_t *port)
{
    int glp_port_id = (_BCM_TD2_L3_PORT_MASK & glp);
    int glp_modid   = (glp >> _BCM_TD2_L3_MODID_SHIFT) & _BCM_TD2_L3_MODID_MASK;

    BCM_GPORT_MODPORT_SET(*port, glp_modid, glp_port_id);

    return BCM_E_NONE;
}

/*
 * Description:
 *     Validate elements of the route bcm_fcoe_route_t structure when adding
 *     a route
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be validated
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_validate_route(int unit, bcm_fcoe_route_t *route)
{
    if ((route->flags & (BCM_FCOE_HOST_ROUTE
                        | BCM_FCOE_DOMAIN_ROUTE
                        | BCM_FCOE_SOURCE_ROUTE)) == 0) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit) == 0) {
        bcm_l3_intf_t find_if;

        bcm_l3_intf_t_init(&find_if);
        find_if.l3a_intf_id = route->intf;
        BCM_IF_ERROR_RETURN(bcm_esw_l3_intf_get(unit, &find_if));

        if (BCM_MAC_IS_ZERO(route->nexthop_mac))
            return BCM_E_PARAM;

        if (!BCM_GPORT_IS_MODPORT(route->port))
            return BCM_E_PARAM;
    }

    if (route->vrf >= SOC_VRF_MAX(unit) || route->vrf < BCM_L3_VRF_DEFAULT)
        return BCM_E_PARAM;

    if (route->nport_id == 0)
        return BCM_E_PARAM;

    if (route->nport_id > FCOE_MAX_SRC_DEST_ID)
        return BCM_E_PARAM;

    if (route->lookup_class < 0 || route->lookup_class > FCOE_ZONE_MAX_CLASS_ID)
        return BCM_E_PARAM;

    return BCM_E_NONE;
}

STATIC int _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(int unit)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {   
        return (L3_ENTRY_SINGLEm);
    }
#endif
    return (L3_ENTRY_IPV4_UNICASTm);
}

STATIC int _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(int unit)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {   
        return (L3_ENTRY_DOUBLEm);
    }
#endif
    return (L3_ENTRY_IPV4_MULTICASTm);
}
/*
 * Description:
 *     Add a route to the L3 table (FCOE view)
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be added
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_add_normal(int unit, bcm_fcoe_route_t *route, 
                               bcm_if_t nh_idx)
{
    l3_entry_ipv4_unicast_entry_t l3_entry[2];

    soc_mem_t  mem      = _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit);
    uint32     *buf     = (uint32 *)&l3_entry[0];
    uint32     *lookup  = (uint32 *)&l3_entry[1];
    int        existing_index = 0;
    int        rv;

    if (route->flags & BCM_FCOE_NO_HOP_COUNT_CHANGE) {
        return BCM_E_PARAM;
    }

    memset(&l3_entry, 0, sizeof l3_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_validate_route(unit, route));
    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_normal(unit, mem, buf,
                                                             route));

    soc_mem_field32_set(unit, mem, buf, FCOE__DST_DISCARDf,
                        (route->flags & BCM_FCOE_DST_DISCARD) ? 1 : 0);
    soc_mem_field32_set(unit, mem, buf, FCOE__PRIf, route->pri);
    soc_mem_field32_set(unit, mem, buf, FCOE__RPEf,
                        (route->flags & BCM_FCOE_RPE) ? 1 : 0);
    soc_mem_field32_set(unit, mem, buf, FCOE__CLASS_IDf, route->lookup_class);

    /* BCM_FCOE_LOCAL_ADDRESS is only valid in host route mode */
    if ((route->flags & BCM_FCOE_HOST_ROUTE)
        && (route->flags & BCM_FCOE_LOCAL_ADDRESS)) {
        soc_mem_field32_set(unit, mem, buf, FCOE__LOCAL_ADDRESSf, 1);
    }

    if (route->flags & BCM_FCOE_MULTIPATH) {
#ifdef BCM_TRIDENT3_SUPPORT 
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_dest_set(unit, mem, buf, FCOE__DESTINATIONf,
                                     SOC_MEM_FIF_DEST_ECMP, nh_idx);

        } else
#endif
        {
            soc_mem_field32_set(unit, mem, buf, FCOE__ECMP_PTR0f, nh_idx);
            soc_mem_field32_set(unit, mem, buf, FCOE__ECMPf, 1);
        }
    } else {
#ifdef BCM_TRIDENT3_SUPPORT 
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field32_dest_set(unit, mem, buf, FCOE__DESTINATIONf,
                                     SOC_MEM_FIF_DEST_NEXTHOP, nh_idx);
        } else
#endif
        {
            soc_mem_field32_set(unit, mem, buf, FCOE__NEXT_HOP_INDEXf, nh_idx);
        }
    }

    /* Search for this key in the existing database */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &existing_index, buf,
                        (void *)lookup, 0);

    /*
     * If the entry exists, overwrite it with the new data, otherwise insert
     * a new entry
     */
    if (rv == SOC_E_NONE) {
        /*
         * Route exists, and the reference count of old nhi should be adjusted.
         * The current route will be updated to new nhi.
         */
        bcm_if_t old_nh_idx = 0;
#ifdef BCM_TRIDENT3_SUPPORT 
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;

            old_nh_idx = soc_mem_field32_dest_get(unit, mem, lookup,
                                         DESTINATIONf, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                BCM_IF_ERROR_RETURN(bcm_xgs3_ecmp_group_del(unit, old_nh_idx, 0));
            } else {
                BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, old_nh_idx));
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, lookup, FCOE__ECMPf)) {
                old_nh_idx = soc_mem_field32_get(unit, mem, 
                                                 lookup, FCOE__ECMP_PTR0f);
                BCM_IF_ERROR_RETURN(bcm_xgs3_ecmp_group_del(unit, old_nh_idx, 0));
            } else {
                old_nh_idx= soc_mem_field32_get(unit, mem, lookup,
                                            FCOE__NEXT_HOP_INDEXf);
                BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, old_nh_idx));
            }
        }
    }

    L3_LOCK(unit);
    if (rv == SOC_E_NONE) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, existing_index, buf);
    } else if (rv == SOC_E_NOT_FOUND) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, buf);
    }
    L3_UNLOCK(unit);

    return rv;
}

/*
 * Description:
 *     Add a route to the L3 table (FCOE_EXT view)
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be added
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_add_ext(int unit, bcm_fcoe_route_t *route)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry[2];

    soc_mem_t  mem            = _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit);
    uint32     *buf           = (uint32 *)&l3_ext_entry[0];
    uint32     *lookup        = (uint32 *)&l3_ext_entry[1];
    int        existing_index = 0;
    int        rv;
    int        glp;

    memset(&l3_ext_entry, 0, sizeof l3_ext_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_validate_route(unit, route));
    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_ext(unit, mem, buf,
                                                          route));

    soc_mem_field32_set(unit, mem, buf, FCOE_EXT__DST_DISCARDf,
                        (route->flags & BCM_FCOE_DST_DISCARD) ? 1 : 0);
    soc_mem_field32_set(unit, mem, buf, FCOE_EXT__PRIf, route->pri);
    soc_mem_field32_set(unit, mem, buf, FCOE_EXT__RPEf,
                        (route->flags & BCM_FCOE_RPE) ? 1 : 0);
    soc_mem_field32_set(unit, mem, buf, FCOE_EXT__CLASS_IDf,
                        route->lookup_class);

    /* BCM_FCOE_LOCAL_ADDRESS is only valid in host route mode */
    if ((route->flags & BCM_FCOE_HOST_ROUTE)
        && (route->flags & BCM_FCOE_LOCAL_ADDRESS)) {

        soc_mem_field32_set(unit, mem, buf, FCOE_EXT__LOCAL_ADDRESSf, 1);
    }

    BCM_IF_ERROR_RETURN(_bcm_fcoe_port_to_glp(route->port, &glp));

    soc_mem_field32_set(unit, mem, buf, FCOE_EXT__L3_INTF_NUMf,
                        route->intf);
    soc_mem_mac_addr_set(unit, mem, buf, FCOE_EXT__MAC_ADDRf,
                        route->nexthop_mac);
    soc_mem_field32_set(unit, mem, buf, FCOE_EXT__COPY_TO_CPUf,
                        (route->flags & BCM_FCOE_COPY_TO_CPU) ? 1 : 0);
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 physical_port;
        soc_mem_field32_set(unit, mem, buf, FCOE_EXT__DO_NOT_DECREMENT_TTLf,
                        (route->flags & BCM_FCOE_NO_HOP_COUNT_CHANGE)
                            ? 1 : 0);

        physical_port = (BCM_GPORT_MODPORT_MODID_GET(route->port) << 
                         SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS) |
                         BCM_GPORT_MODPORT_PORT_GET(route->port);

        soc_mem_field32_dest_set(unit, mem, buf, FCOE_EXT__DESTINATIONf,
                                     SOC_MEM_FIF_DEST_DGPP, physical_port);
    } else
#endif
    {
        soc_mem_field32_set(unit, mem, buf, FCOE_EXT__DO_NOT_CHANGE_TTLf,
                            (route->flags & BCM_FCOE_NO_HOP_COUNT_CHANGE)
                            ? 1 : 0);
        soc_mem_field32_set(unit, mem, buf, FCOE_EXT__GLPf, glp);
    }

    /* Search for this key in the existing database */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &existing_index, buf,
                        (void *)lookup, 0);

    /*
     * If the entry exists, overwrite it with the new data, otherwise insert
     * a new entry
     */
    L3_LOCK(unit);
    if (rv == SOC_E_NONE) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, existing_index, buf);
    } else if (rv == SOC_E_NOT_FOUND) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, buf);
    }
    L3_UNLOCK(unit);

    return rv;
}

STATIC int
_bcm_fcoe_route_to_defip(int unit, bcm_fcoe_route_t *route, 
                         _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t entry;
    int nh_ecmp_index;

    sal_memset(lpm_cfg, 0, sizeof *lpm_cfg);

    if (route->flags & BCM_FCOE_COPY_TO_CPU) {
        lpm_cfg->defip_flags |= BCM_L3_L2TOCPU;
    }
    if (route->flags & BCM_FCOE_DST_DISCARD) {
        lpm_cfg->defip_flags |= BCM_L3_DST_DISCARD;
    }
    if (route->flags & BCM_FCOE_SRC_DISCARD) {
        lpm_cfg->defip_flags |= BCM_L3_SRC_DISCARD;
    }
    if (route->flags & BCM_FCOE_NO_HOP_COUNT_CHANGE) {
        lpm_cfg->defip_flags |= BCM_L3_KEEP_TTL;
    }
    if (route->flags & BCM_FCOE_MULTIPATH) {
        lpm_cfg->defip_ecmp = 1;
        lpm_cfg->defip_flags |= BCM_L3_MULTIPATH;
    }

    lpm_cfg->defip_vrf = route->vrf;
    lpm_cfg->defip_port_tgid = BCM_GPORT_MODPORT_PORT_GET(route->port);
    lpm_cfg->defip_modid = BCM_GPORT_MODPORT_MODID_GET(route->port);
    lpm_cfg->defip_prio = route->pri;
    lpm_cfg->defip_lookup_class = route->lookup_class;
    lpm_cfg->defip_intf = route->intf;
    lpm_cfg->defip_entry_type = bcmDefipEntryTypeFcoe;
    lpm_cfg->defip_fcoe_d_id = route->nport_id & route->nport_id_mask;
    lpm_cfg->defip_fcoe_d_id_mask = route->nport_id_mask;

    memcpy(&lpm_cfg->defip_mac_addr, route->nexthop_mac, 
           sizeof route->nexthop_mac);
    entry = *lpm_cfg;
    rv = _bcm_fb_lpm_get(unit, &entry, &nh_ecmp_index);
    if (BCM_SUCCESS(rv)) {
        /* Save route entry index in hw to indicate  */
        lpm_cfg->defip_index = entry.defip_index;
    } else if (rv == BCM_E_NOT_FOUND){
        /* New route. */
        lpm_cfg->defip_index = BCM_XGS3_L3_INVALID_INDEX;
        rv = BCM_E_NONE;
    }

    return rv;
}

STATIC int
_bcm_fcoe_defip_to_route(int unit, _bcm_defip_cfg_t *lpm_cfg, 
                         bcm_fcoe_route_t *route)
{
    int rv = BCM_E_NONE;

    sal_memset(route, 0, sizeof *route);

    route->flags = BCM_FCOE_DOMAIN_ROUTE;
    
    if (lpm_cfg->defip_flags & BCM_L3_L2TOCPU) {
        route->flags |= BCM_FCOE_COPY_TO_CPU;
    }

    if (lpm_cfg->defip_flags & BCM_L3_DST_DISCARD) {
        route->flags |= BCM_FCOE_DST_DISCARD;
    }

    if (lpm_cfg->defip_flags & BCM_L3_SRC_DISCARD) {
        route->flags |= BCM_FCOE_SRC_DISCARD;
    }

    if (lpm_cfg->defip_flags & BCM_L3_KEEP_TTL) {
        route->flags |= BCM_FCOE_NO_HOP_COUNT_CHANGE;
    }

    if (lpm_cfg->defip_ecmp == 1) {
        route->flags |= BCM_FCOE_MULTIPATH;
    }

    route->vrf = lpm_cfg->defip_vrf;
    BCM_GPORT_MODPORT_SET(route->port, lpm_cfg->defip_modid, 
                          lpm_cfg->defip_port_tgid);

    route->pri = lpm_cfg->defip_prio;
    route->lookup_class = lpm_cfg->defip_lookup_class;
    route->intf = lpm_cfg->defip_intf;
    route->nport_id = lpm_cfg->defip_fcoe_d_id;
    route->nport_id_mask = lpm_cfg->defip_fcoe_d_id_mask;

    memcpy(route->nexthop_mac, &lpm_cfg->defip_mac_addr, 
           sizeof route->nexthop_mac);

    return rv;
}

STATIC int
_bcm_td2_fcoe_route_add_prefix(int unit, bcm_fcoe_route_t *route)
{
    _bcm_defip_cfg_t lpm_cfg;
    int              nh_idx         = 0;
    int              rv             = BCM_E_NONE;
    int              old_nh_idx     = 0;
    _bcm_defip_cfg_t old_lpm_cfg;

    sal_memset(&lpm_cfg, 0, sizeof lpm_cfg);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_validate_route(unit, route));
    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_to_defip(unit, route, &lpm_cfg));
    sal_memcpy(&old_lpm_cfg, &lpm_cfg, sizeof(_bcm_defip_cfg_t));
    rv = _bcm_fb_lpm_get(unit, &old_lpm_cfg, &old_nh_idx);
    if (BCM_SUCCESS(rv)) {
        /*
         * Route exists, and the reference count of old nhi should be adjusted.
         * The current route is updated to new nhi.
         */
        if (old_lpm_cfg.defip_flags & BCM_L3_MULTIPATH) {
            BCM_IF_ERROR_RETURN(bcm_xgs3_ecmp_group_del(unit, old_nh_idx, 0));
        } else {
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, old_nh_idx));
        }
    }
    BCM_IF_ERROR_RETURN(_bcm_xgs3_nh_init_add(unit, NULL, &lpm_cfg, &nh_idx));

    rv = _bcm_fb_lpm_add(unit, &lpm_cfg, nh_idx);
    if (BCM_FAILURE(rv)) {
        if (route->flags & BCM_FCOE_MULTIPATH) {
            (void) bcm_xgs3_ecmp_group_del(unit, nh_idx, 0);
        } else {
            (void) bcm_xgs3_nh_del(unit, 0, nh_idx);
        }
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Description: 
 *      Add an entry into FCOE host routing table. Supports add for both
 *      normal (with next-hop) and extended views (embedded nh)
 * Parameters: 
 *      unit    - device number
 *      route   - route information to be added
 * Returns: 
 *      BCM_E_XXX  
 */
STATIC int
_bcm_td2_fcoe_route_add_host(int unit, bcm_fcoe_route_t *route)
{
    int         rv = BCM_E_NONE;
    bcm_if_t    nh_idx;                                    

    BCM_IF_ERROR_RETURN(_bcm_fcoe_add_nh_entry(unit, route, &nh_idx));

    if (BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, route->intf, nh_idx)) {
        rv = _bcm_td2_fcoe_route_add_ext(unit, route);
    } else {
        rv = _bcm_td2_fcoe_route_add_normal(unit, route, nh_idx);
    }

    if (BCM_FAILURE(rv) && (nh_idx != BCM_XGS3_L3_INVALID_INDEX) && 
        !(route->flags & BCM_FCOE_MULTIPATH)) {
        (void)bcm_xgs3_nh_del(unit, 0, nh_idx);
    }

    return rv;
}

#endif

/*
 * Description:
 *     Add an entry into the FCOE routing table.  This API supports all
 *     three routes:
 *       - Host, source-map, Domain (goes in L3_ENTRY table)
 *       - Prefix (goes in DEFIP table)
 * Parameters:
 *     unit      - device number
 *     route     - route information to be added
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_route_add(int unit, bcm_fcoe_route_t *route)
{
#ifdef INCLUDE_L3
    if (_bcm_td2_fcoe_route_is_prefix(unit, route)) {
        return _bcm_td2_fcoe_route_add_prefix(unit, route);
    } else {
        return _bcm_td2_fcoe_route_add_host(unit, route);
    }
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef INCLUDE_L3

/*
 * Description:
 *     Delete a route from the DEFIP table
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_delete_prefix(int unit, uint32 *lookup)
{
    int nh_idx = 0;
    _bcm_defip_cfg_t lpm_cfg;

    sal_memset(&lpm_cfg, 0, sizeof lpm_cfg);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        uint32 dest_type;

        /* delete the left half if it's a valid FCOE entry */
        if (soc_L3_DEFIPm_field32_get(unit, lookup, VALID0f)
            && (soc_L3_DEFIPm_field32_get(unit, lookup, DATA_TYPE0f) == 3)) {

            BCM_IF_ERROR_RETURN(
                _bcm_fb_lpm_ent_parse(unit, &lpm_cfg, &nh_idx,
                                      (uint32 *)lookup, NULL));
            lpm_cfg.defip_index = BCM_XGS3_L3_INVALID_INDEX;
            BCM_IF_ERROR_RETURN(_bcm_fb_lpm_del(unit, &lpm_cfg));
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx));
        }

        /* 
         * if the right half is a valid FCOE entry, copy it to the left half and
         * then delete 
         */
        if (soc_L3_DEFIPm_field32_get(unit, lookup, VALID1f)
            && (soc_L3_DEFIPm_field32_get(unit, lookup, DATA_TYPE1f) == 3)) {
            nh_idx = soc_mem_field32_dest_get(unit, L3_DEFIPm, lookup,
                                         DESTINATIONf, &dest_type);
            BCM_IF_ERROR_RETURN(soc_fb_lpm_ip4entry1_to_0(unit, lookup, lookup, 0));
            BCM_IF_ERROR_RETURN(soc_fb_lpm_delete(unit, lookup));
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx));
        }
    } else 
#endif
    {
        /* delete the left half if it's a valid FCOE entry */
        if (soc_L3_DEFIPm_field32_get(unit, lookup, VALID0f)
            && (soc_L3_DEFIPm_field32_get(unit, lookup, ENTRY_TYPE0f) == 1)) {
    
            BCM_IF_ERROR_RETURN(
                _bcm_fb_lpm_ent_parse(unit, &lpm_cfg, &nh_idx,
                                      (uint32 *)lookup, NULL));
    
            lpm_cfg.defip_index = BCM_XGS3_L3_INVALID_INDEX;
            BCM_IF_ERROR_RETURN(_bcm_fb_lpm_del(unit, &lpm_cfg));
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx));
        }
    
        /* 
         * if the right half is a valid FCOE entry, copy it to the left half and
         * then delete 
         */
        if (soc_L3_DEFIPm_field32_get(unit, lookup, VALID1f)
            && (soc_L3_DEFIPm_field32_get(unit, lookup, ENTRY_TYPE1f) == 1)) {

            nh_idx = soc_L3_DEFIPm_field32_get(unit, lookup, NEXT_HOP_INDEX1f);
            BCM_IF_ERROR_RETURN(soc_fb_lpm_ip4entry1_to_0(unit, lookup, lookup, 0));
            BCM_IF_ERROR_RETURN(soc_fb_lpm_delete(unit, lookup));
            BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx));
        }
    }

    return BCM_E_NONE;
}

/*
 * Description:
 *     Delete a route from hardware.  If the route has a corresponding nexthop
 *     index, delete that too (or decrement refcount).
 * Parameters:
 *     unit      - (IN) unit number
 *     mem       - (IN) memory to be deleted from
 *     lookup    - (IN) table lookup result to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_internal_delete_route(int unit, bcm_fcoe_route_t *route, 
                                    soc_mem_t mem, uint32 *lookup) 
{
    int rv = BCM_E_NONE;
    int next_hop_index = 0;
    int nh_field = FCOE__NEXT_HOP_INDEXf;

    if (mem == L3_DEFIPm) {
        return _bcm_td2_fcoe_route_delete_prefix(unit, lookup);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        nh_field = DESTINATIONf;
    }
#endif

    if (SOC_MEM_FIELD_VALID(unit, mem, nh_field)) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 dest_type;
            next_hop_index = soc_mem_field32_dest_get(unit, mem, lookup,
                                         nh_field, &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_ECMP){
                if ((route != NULL) && !(route->flags & BCM_FCOE_MULTIPATH)) {
                    return BCM_E_PARAM;
                }
                rv = bcm_xgs3_ecmp_group_del(unit, next_hop_index, 0);
            } else {
                rv = bcm_xgs3_nh_del(unit, 0, next_hop_index);
            }
        } else 
#endif
        {
            next_hop_index = soc_mem_field32_get(unit, mem, lookup, nh_field);
            if (SOC_MEM_FIELD_VALID(unit, mem, FCOE__ECMPf) &&
                soc_mem_field32_get(unit, mem, lookup, FCOE__ECMPf)) {
                if ((route != NULL) && !(route->flags & BCM_FCOE_MULTIPATH)) {
                    return BCM_E_PARAM;
                }
                rv = bcm_xgs3_ecmp_group_del(unit, next_hop_index, 0);
            } else {
                rv = bcm_xgs3_nh_del(unit, 0, next_hop_index);
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        L3_LOCK(unit);
        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)lookup);
        L3_UNLOCK(unit);
    }

    return rv;
}

/*
 * Description:
 *     Delete a route from the L3 table (FCOE view)
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_delete_normal(int unit, bcm_fcoe_route_t *route)
{
    l3_entry_ipv4_unicast_entry_t   l3_entry[2];

    soc_mem_t  mem      = _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit);
    uint32     *buf     = (uint32 *)&l3_entry[0];
    uint32     *lookup  = (uint32 *)&l3_entry[1];
    int        existing_index = 0;
    int        rv;

    memset(&l3_entry, 0, sizeof l3_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_normal(unit, mem, buf,
                                                             route));

    /* Search for this key in the existing database */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &existing_index, buf,
                        (void *)lookup, 0);
    switch (rv) {
        case SOC_E_NOT_FOUND: return BCM_E_NOT_FOUND;
        case SOC_E_NONE: break;
        default: return BCM_E_PARAM;
    }

    return _bcm_td2_fcoe_internal_delete_route(unit, route, mem, lookup);
}

/*
 * Description:
 *     Delete a route from the L3 table (FCOE_EXT view)
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_delete_ext(int unit, bcm_fcoe_route_t *route)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry[2];

    soc_mem_t  mem      = _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit);
    uint32     *buf     = (uint32 *)&l3_ext_entry[0];
    uint32     *lookup  = (uint32 *)&l3_ext_entry[1];
    int        existing_index = 0;
    int        rv;

    memset(&l3_ext_entry, 0, sizeof l3_ext_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_ext(unit, mem, buf,
                                                          route));

    /* Search for this key in the existing database */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &existing_index, buf,
                        (void *)lookup, 0);
    switch (rv) {
        case SOC_E_NOT_FOUND: return BCM_E_NOT_FOUND;
        case SOC_E_NONE: break;
        default: return BCM_E_PARAM;
    }

    return _bcm_td2_fcoe_internal_delete_route(unit, route, mem, lookup);
}

STATIC int
_bcm_td2_find_and_delete_defip(int unit, bcm_fcoe_route_t *route)
{
    _bcm_defip_cfg_t lpm_cfg;
    int nh_idx;

    sal_memset(&lpm_cfg, 0, sizeof lpm_cfg);
    
    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_to_defip(unit, route, &lpm_cfg));
    BCM_IF_ERROR_RETURN(_bcm_fb_lpm_get(unit, &lpm_cfg, &nh_idx));
    BCM_IF_ERROR_RETURN(_bcm_fb_lpm_del(unit, &lpm_cfg));
    if (route->flags & BCM_FCOE_MULTIPATH) {
        BCM_IF_ERROR_RETURN(
            bcm_xgs3_ecmp_group_del(unit, nh_idx, 0));
    } else {
        BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_idx));
    }

    return BCM_E_NONE;
}

#endif

/*
 * Description:
 *     Delete a route from the L3 table
 * Parameters:
 *     unit      - (IN) unit number
 *     route     - (IN) route to be added
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_route_delete(int unit, bcm_fcoe_route_t *route)
{
    int rv = BCM_E_UNAVAIL;

#ifdef INCLUDE_L3
    if (route == NULL) {
        return BCM_E_NONE;
    }

    if (_bcm_td2_fcoe_route_is_prefix(unit, route)) {
        return _bcm_td2_find_and_delete_defip(unit, route);
    } else {
        rv = _bcm_td2_fcoe_route_delete_ext(unit, route);
        if (rv == BCM_E_NOT_FOUND) {
            rv = _bcm_td2_fcoe_route_delete_normal(unit, route);
        }
    }
#endif

    return rv;
}

#ifdef INCLUDE_L3

/*
 * Description:
 *     Given a buffer holding a table row, retrieve the VRF
 * Parameters:
 *     unit      - (IN) unit number
 *     mem       - (IN) table type
 *     buf       - (IN) buffer holding table data
 *     vrf       - (OUT) VRF from table
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_mem_get_vrf(int unit, soc_mem_t mem, uint32 *buf, int *vrf)
{
    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        *vrf = soc_mem_field32_get(unit, mem, buf, FCOE_EXT__VRF_IDf);
    } else if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
        *vrf = soc_mem_field32_get(unit, mem, buf, FCOE__VRF_IDf);
    } else if (mem == L3_DEFIPm) {
        int entry_type = soc_mem_field32_get(unit, mem, buf, ENTRY_TYPE0f);
        if (entry_type == 1) {
            *vrf = soc_mem_field32_get(unit, mem, buf, VRF_ID_0f);
        } else {
            *vrf = soc_mem_field32_get(unit, mem, buf, VRF_ID_1f);
        }
    } else {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Description:
 *     Given a buffer holding a table row, retrieve the L3 interface number
 * Parameters:
 *     unit      - (IN) unit number
 *     mem       - (IN) table type
 *     buf       - (IN) buffer holding table data
 *     intf      - (OUT) L3_INTF_NUM from table
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_mem_get_intf(int unit, soc_mem_t mem, uint32 *buf, int *intf)
{
    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        *intf = soc_mem_field32_get(unit, mem, buf, FCOE_EXT__L3_INTF_NUMf);
        return BCM_E_NONE;
    } else {
        return BCM_E_FAIL;
    }
}

/*
 * Description:
 *     Given a buffer holding a table row, retrieve the key type
 * Parameters:
 *     unit      - (IN) unit number
 *     mem       - (IN) table type
 *     buf       - (IN) buffer holding table data
 *     key_type  - (OUT) KEY_TYPE from table
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_mem_get_key_type(int unit, soc_mem_t mem, uint32 *buf, int *key_type)
{
    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            *key_type = soc_mem_field32_get(unit, mem, buf, KEY_TYPEf);
        } else
#endif
        {
            *key_type = soc_mem_field32_get(unit, mem, buf, KEY_TYPE_0f);
        }
    } else if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
        *key_type = soc_mem_field32_get(unit, mem, buf, KEY_TYPEf);
    } else if (mem == L3_DEFIPm) {
        *key_type = TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN;
    } else {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Description:
 *     Given a buffer holding a table row, retrieve the nport_id
 * Parameters:
 *     unit      - (IN) unit number
 *     mem       - (IN) table type
 *     buf       - (IN) buffer holding table data
 *     id        - (OUT) D_ID or S_ID from table
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_mem_get_id(int unit, soc_mem_t mem, uint32 *buf, uint32 *id)
{
    int key_type;
    int id_key;

    BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_key_type(unit, mem, buf, &key_type));

    if (mem == L3_DEFIPm) {
        int entry_type = soc_mem_field32_get(unit, mem, buf, ENTRY_TYPE0f);
        if (entry_type == 1) {
            id_key = D_ID0f;
        } else {
            id_key = D_ID1f;
        }
    } else {
        switch (key_type) {

            case TD2_L3_HASH_KEY_TYPE_FCOE_HOST:
            case TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT:
                id_key = (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit))
                         ? FCOE_EXT__D_IDf : FCOE__D_IDf;
                break;
            case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
            case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT:
                id_key = (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit))
                         ? FCOE_EXT__MASKED_D_IDf : FCOE__MASKED_D_IDf;
                break;
            case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP:
            case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT:
                id_key = (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit))
                         ? FCOE_EXT__S_IDf : FCOE__S_IDf;
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    *id = soc_mem_field32_get(unit, mem, buf, id_key);

    return BCM_E_NONE;
}
/*
 * Description:
 *     Determine if the given key type is an FCOE key for the given table.
 * Parameters:
 *     mem           - (IN) table type
 *     key_type      - (IN) key type field from the table row
 * Returns:
 *     0 indicates "not an FCOE key"
 *     1 indicates "is an FCOE key"
 */
#ifdef BCM_TRIDENT3_SUPPORT
STATIC int
_bcm_td3_key_type_is_fcoe(int unit, soc_mem_t mem, int key_type_0, int key_type_1)
{
    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        if ((key_type_0 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_HOST) &&
            (key_type_1 == (int)TD3_L3_HASH_DATA_TYPE_FCOE_HOST_EXT))
            return (1);

        if ((key_type_0 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN) &&
            (key_type_1 == (int)TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN_EXT))
            return (1);

        if ((key_type_0 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP) &&
            (key_type_1 == (int)TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP_EXT))
            return (1);
    } else if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
        if ((key_type_0 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_HOST) &&
            (key_type_1 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_HOST_EXT))
            return (1);

        if ((key_type_0 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN) &&
            (key_type_1 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT))
            return (1);

        if ((key_type_0 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP) &&
            (key_type_1 == (int)TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP_EXT))
            return (1);
    } 

    return 0;
}
#endif

/*
 * Description:
 *     Determine if the given key type is an FCOE key for the given table.
 * Parameters:
 *     mem           - (IN) table type
 *     key_type      - (IN) key type field from the table row
 * Returns:
 *     0 indicates "not an FCOE key"
 *     1 indicates "is an FCOE key"
 */
STATIC int
_bcm_td2_key_type_is_fcoe(int unit, soc_mem_t mem, int key_type)
{
    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        return (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT)
               || (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT)
               || (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT);
    } else if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
        return (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_HOST)
               || (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN)
               || (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP);
    } else {
        return 0;
    }
}

/*
 * Description:
 *     Traverse the contents of a given memory (either L3 unicast or L3 mcast).
 *     For each row, call the given function and supply the given route
 *     as a parameter.
 * Parameters:
 *     unit          - (IN) unit number
 *     mem           - (IN) table type
 *     traverse_func - (IN) callback function
 *     route         - (IN) route to be passed to callback
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_traverse_table(int unit, soc_mem_t mem,
                             fcoe_traverse_cb_f traverse_func,
                             bcm_fcoe_route_t *route)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry;

    uint32      *buf = (uint32 *)&l3_ext_entry;
    int         idx;
    int         idx_max;
    int         key_type_0;
    int         key_type_1;
    int         rv = BCM_E_NONE;

    idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (idx = 0; idx < idx_max; idx++) {
        sal_memset(buf, 0, sizeof l3_ext_entry);

        rv = BCM_XGS3_MEM_READ(unit, mem, idx, buf);
        if (BCM_FAILURE(rv)) {
            break;
        }

        if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                if (!soc_mem_field32_get(unit, mem, buf, BASE_VALIDf)) {
                    continue;
                }
                key_type_0 = soc_mem_field32_get(unit, mem, buf,
                                                 KEY_TYPEf);
                key_type_1 = soc_mem_field32_get(unit, mem, buf,
                                                     DATA_TYPEf);
            } else 
#endif         
            {
                if (!soc_mem_field32_get(unit, mem, buf, VALIDf)) {
                    continue;
                }
                key_type_0 = soc_mem_field32_get(unit, mem, buf,
                                                 KEY_TYPEf);
                key_type_1 = key_type_0; 
            }
        } else {
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                if (!soc_mem_field32_get(unit, mem, buf, BASE_VALID_0f)
                    || !soc_mem_field32_get(unit, mem, buf, BASE_VALID_1f)) {
                    continue;
                }
                key_type_0 = soc_mem_field32_get(unit, mem, buf,
                                                 KEY_TYPEf);
                key_type_1 = soc_mem_field32_get(unit, mem, buf,
                                                 DATA_TYPEf);
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, mem, buf, VALID_0f)
                   || !soc_mem_field32_get(unit, mem, buf, VALID_1f)) {
                    continue;
                }
                key_type_0 = soc_mem_field32_get(unit, mem, buf,
                                                 KEY_TYPE_0f);
                key_type_1 = soc_mem_field32_get(unit, mem, buf,
                                                 KEY_TYPE_1f);
            }
        }
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (!_bcm_td3_key_type_is_fcoe(unit, mem, key_type_0, key_type_1)) {
                continue;
            }
        } else
#endif
        {
            if (!_bcm_td2_key_type_is_fcoe(unit, mem, key_type_0)
                || (key_type_0 != key_type_1)) {
                continue;
            }
        }

        rv = traverse_func(unit, mem, buf, route);
        if (rv != BCM_E_NONE) {
            break;
        }
    }

    /* coverity[overrun-local] */
    soc_mem_unlock(unit, mem);

    return rv;
}

STATIC int
_bcm_td2_fcoe_traverse_defip(int unit, fcoe_traverse_cb_f traverse_func,
                             bcm_fcoe_route_t *route)
{
    soc_mem_t   mem = L3_DEFIPm;
    int         idx;
    int         rv = BCM_E_NONE;
    char        *defip_tbl_ptr = NULL;
    int         defip_table_size = 0;
    defip_entry_t *lpm_entry;

    soc_mem_lock(unit, mem);

    rv = bcm_xgs3_l3_tbl_dma(unit, mem, BCM_XGS3_L3_ENT_SZ(unit, defip),
                             "lpm_tbl", &defip_tbl_ptr, &defip_table_size);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    for (idx = 0; idx < defip_table_size; idx++) {
        lpm_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                                 defip_entry_t *,
                                                 defip_tbl_ptr, idx);

        if ((soc_mem_field32_get(unit, mem, (uint32 *)lpm_entry, VALID0f) &&
            soc_mem_field32_get(unit, mem, (uint32 *)lpm_entry, ENTRY_TYPE0f))
            || (soc_mem_field32_get(unit, mem, (uint32 *)lpm_entry, VALID1f) &&
            soc_mem_field32_get(unit, mem, (uint32 *)lpm_entry, ENTRY_TYPE1f))
            ) {
            rv = traverse_func(unit, mem, (uint32 *)lpm_entry, route);
            if (rv != BCM_E_NONE) {
                break;
            }
        }
    }
    soc_cm_sfree(unit, defip_tbl_ptr);
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Description:
 *     Traverse the contents of the L3 unicast and L3 mcast memories.
 *     For each row, call the given function and supply the given route
 *     as a parameter.
 * Parameters:
 *     unit          - (IN) unit number
 *     traverse_func - (IN) callback function
 *     route         - (IN) route to be passed to callback
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_traverse_tables(int unit, fcoe_traverse_cb_f traverse_func,
                              bcm_fcoe_route_t *route)
{
    int rv = _bcm_td2_fcoe_traverse_table(unit, _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit),
                                          traverse_func, route);
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_td2_fcoe_traverse_table(unit, _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit),
                                          traverse_func, route);
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_td2_fcoe_traverse_defip(unit, traverse_func, route);
    }
    return rv;
}

/*
 * Description:
 *     Callback function used in bcm_td2_fcoe_route_delete_by_prefix for each
 *     table row
 * Parameters:
 *     unit      - device number
 *     mem       - memory being accessed
 *     buf       - buffer holding table data
 *     route     - route information to be checked
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_delete_by_prefix_cb(int unit, soc_mem_t mem, uint32 *buf,
                                        bcm_fcoe_route_t *route)
{
    int searching_prefix = route->nport_id & route->nport_id_mask;
    int vrf;
    uint32 id;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_id(unit, mem, buf, &id));
    BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_vrf(unit, mem, buf, &vrf));

    if (route->vrf != vrf) {
        return BCM_E_NONE;
    }

    if ((id & route->nport_id_mask) == searching_prefix) {
        rv = _bcm_td2_fcoe_internal_delete_route(unit, route, mem, buf);

        if (rv != SOC_E_NONE) {
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

#endif

/*
 * Description:
 *     Delete FCOE entries based on Masked Domain prefix (network)
 * Parameters:
 *     unit      - device number
 *     route     - route information to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_route_delete_by_prefix(int unit, bcm_fcoe_route_t *route)
{
#ifdef INCLUDE_L3
    return _bcm_td2_fcoe_traverse_tables(unit,
                                    &_bcm_td2_fcoe_route_delete_by_prefix_cb,
                                    route);
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef INCLUDE_L3

STATIC int
_bcm_fcoe_read_nexthop_into_route(int unit, int nexthop,
                                  bcm_fcoe_route_t *route, int flag);

/*
 * Description:
 *     Callback function used in bcm_td2_fcoe_route_delete_by_interface for each
 *     table row
 * Parameters:
 *     unit      - device number
 *     mem       - memory being accessed
 *     buf       - buffer holding table data
 *     route     - route information to be checked
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_delete_by_intf_cb(int unit, soc_mem_t mem, uint32 *buf,
                                      bcm_fcoe_route_t *route)
{
    int vrf;
    int intf;
    int nexthop;
    bcm_fcoe_route_t tbl_route;
    int rv = BCM_E_NONE;

    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_vrf(unit, mem, buf, &vrf));
        BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_intf(unit, mem, buf, &intf));

        if (route->vrf == vrf && route->intf == intf) {
            rv = _bcm_td2_fcoe_internal_delete_route(unit, route, mem, buf);

            if (rv != SOC_E_NONE) {
                return BCM_E_FAIL;
            }
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_vrf(unit, mem, buf, &vrf));
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 dest_type;
            nexthop = soc_mem_field32_dest_get(unit, mem, buf,
                                         DESTINATIONf, &dest_type);
        } else
#endif
        {
            nexthop = soc_mem_field32_get(unit, mem, buf, FCOE__NEXT_HOP_INDEXf);
        }


        sal_memset(&tbl_route, 0x0, sizeof(tbl_route));

        BCM_IF_ERROR_RETURN(
            _bcm_fcoe_read_nexthop_into_route(unit, nexthop, &tbl_route,
                                              FCOE_ROUTE_READ_RETURN_L3_INTF));

        if (route->vrf == vrf && route->intf == tbl_route.intf) {
            rv = _bcm_td2_fcoe_internal_delete_route(unit, route, mem, buf);

            if (rv != SOC_E_NONE) {
                return BCM_E_FAIL;
            }
        }
    }

    return rv;
}

#endif

/*
 * Description:
 *     Delete FCOE entries that do/don't match a specified L3 interface
 * Parameters:
 *     unit      - device number
 *     route     - route information to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_route_delete_by_interface(int unit, bcm_fcoe_route_t *route)
{
#ifdef INCLUDE_L3
    return _bcm_td2_fcoe_traverse_tables(unit, 
                                        &_bcm_td2_fcoe_route_delete_by_intf_cb,
                                        route);
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef INCLUDE_L3
/*
 * Description:
 *     Callback function used in bcm_td2_fcoe_route_delete_all for each
 *     table row
 * Parameters:
 *     unit      - device number
 *     mem       - memory being accessed
 *     buf       - buffer holding table data
 *     route     - route information to be checked
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_delete_all_cb(int unit, soc_mem_t mem, uint32 *buf,
                                  bcm_fcoe_route_t *route)
{
    int rv = _bcm_td2_fcoe_internal_delete_route(unit, route, mem, buf);

    if (rv != SOC_E_NONE) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}
#endif

/*
 * Description:
 *     Deletes all FCOE routing table entries
 * Parameters:
 *     unit      - device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_route_delete_all(int unit)
{
#ifdef INCLUDE_L3
    return _bcm_td2_fcoe_traverse_tables(unit,
                                         &_bcm_td2_fcoe_route_delete_all_cb,
                                         NULL);
#else
    return BCM_E_UNAVAIL;
#endif
}

#ifdef INCLUDE_L3
/*
 * Description:
 *     Read the nexthop table and convert relevant info into the fcoe route
 *     structure
 * Parameters:
 *     unit      - device number
 *     nexthop   - table index
 *     route     - route information to be filled in
 *     flag      - internal flag indicates return l3 intf or nexthop index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_read_nexthop_into_route(int unit, int nexthop,
                                  bcm_fcoe_route_t *route, int flag)
{
    bcm_l3_egress_t nh_entry;
    int rv;

    if (BCM_XGS3_L3_HWCALL_CHECK(unit, nh_get)) {
        rv = BCM_XGS3_L3_HWCALL_EXEC(unit, nh_get) (unit, nexthop, &nh_entry);
    } else {
        rv = BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN(rv);

    if (nh_entry.flags & BCM_L3_L2TOCPU) {
        route->flags |= BCM_FCOE_COPY_TO_CPU;
    }

    if (nh_entry.flags & BCM_L3_DST_DISCARD) {
        route->flags |= BCM_FCOE_DST_DISCARD;
    }

    if (nh_entry.flags & BCM_L3_SRC_DISCARD) {
        route->flags |= BCM_FCOE_SRC_DISCARD;
    }

    if (nh_entry.flags & BCM_L3_KEEP_TTL) {
        route->flags |= BCM_FCOE_NO_HOP_COUNT_CHANGE;
    }

    BCM_IF_ERROR_RETURN(_bcm_fcoe_glp_to_port(nh_entry.port, &route->port));
    if (flag == FCOE_ROUTE_READ_RETURN_NHID) {
        if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
            if (route->flags & BCM_FCOE_MULTIPATH) {
                route->intf = nexthop + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
            } else {
                route->intf = nexthop + BCM_XGS3_EGRESS_IDX_MIN(unit);
            }
            return BCM_E_NONE;
        }
    }
    if (flag != FCOE_ROUTE_READ_RETURN_L3_INTF) {
        return BCM_E_INTERNAL; 
    }
    route->intf = nh_entry.intf;

    return BCM_E_NONE;
}

/*
 * Description:
 *     Read a buffer from the L3 hardware table into a fcoe route structure
 * Parameters:
 *     unit      - device number
 *     mem       - table to be used
 *     buf       - buffer holding the table row
 *     route     - route information to be created
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fcoe_read_mem_into_route(int unit, soc_mem_t mem, uint32 *buf,
                                  bcm_fcoe_route_t *route)
{
    int key_type;
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 dest_type = 0;
#endif

    BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_key_type(unit, mem, buf, &key_type));
    BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_id(unit, mem, buf,
                                            &route->nport_id));

    switch (key_type) {
        case TD2_L3_HASH_KEY_TYPE_FCOE_HOST:
        case TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT:
            route->flags = BCM_FCOE_HOST_ROUTE;
            break;

        case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP:
        case TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT:
            route->flags = BCM_FCOE_SOURCE_ROUTE;
            break;

        case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN:
        case TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT:
            route->flags = BCM_FCOE_DOMAIN_ROUTE;
            break;

        default:
            return BCM_E_FAIL;
    }

    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        int port;

        if (soc_mem_field32_get(unit, mem, buf, FCOE_EXT__DST_DISCARDf)) {
            route->flags |= BCM_FCOE_DST_DISCARD;
        }
        if (soc_mem_field32_get(unit, mem, buf, FCOE_EXT__RPEf)) {
            route->flags |= BCM_FCOE_RPE;
        }

        route->pri = soc_mem_field32_get(unit, mem, buf, FCOE_EXT__PRIf);
        route->lookup_class = soc_mem_field32_get(unit, mem, buf,
                                                  FCOE_EXT__CLASS_IDf);

        if ((route->flags & BCM_FCOE_HOST_ROUTE)
            && soc_mem_field32_get(unit, mem, buf, FCOE_EXT__LOCAL_ADDRESSf)) {
            route->flags |= BCM_FCOE_LOCAL_ADDRESS;
        }

        BCM_IF_ERROR_RETURN(_bcm_td2_mem_get_intf(unit, mem, buf,
                                                  &route->intf));

        soc_mem_mac_addr_get(unit, mem, buf, FCOE_EXT__MAC_ADDRf,
                             route->nexthop_mac);

        if (soc_mem_field32_get(unit, mem, buf, FCOE_EXT__COPY_TO_CPUf)) {
            route->flags |= BCM_FCOE_COPY_TO_CPU;
        }


#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            uint32 dest;
            if (soc_mem_field32_get(unit, mem, buf, FCOE_EXT__DO_NOT_DECREMENT_TTLf)) {
                route->flags |= BCM_FCOE_NO_HOP_COUNT_CHANGE;
            }
            dest = soc_mem_field32_dest_get(unit, mem, buf, FCOE_EXT__DESTINATIONf,
                                     &dest_type);
            if (dest_type == SOC_MEM_FIF_DEST_DGPP) {
                port = BCM_GPORT_MODPORT_PORT_GET(dest);
            } else {
                return BCM_E_INTERNAL;
            }
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            if (soc_mem_field32_get(unit, mem, buf, FCOE_EXT__DO_NOT_CHANGE_TTLf)) {
                route->flags |= BCM_FCOE_NO_HOP_COUNT_CHANGE;
            }

            port = soc_mem_field32_get(unit, mem, buf, FCOE_EXT__GLPf);
            BCM_IF_ERROR_RETURN(_bcm_fcoe_glp_to_port(port, &route->port));
        }
    } else {
        int nexthop;

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            nexthop = soc_mem_field32_dest_get(unit, mem, buf, DESTINATIONf, &dest_type);
        } else
#endif
        {
            nexthop = soc_mem_field32_get(unit, mem, buf, FCOE__NEXT_HOP_INDEXf);
        }

        route->pri = soc_mem_field32_get(unit, mem, buf, FCOE__PRIf);
        route->lookup_class = soc_mem_field32_get(unit, mem, buf,
                                                  FCOE__CLASS_IDf);

        if ((route->flags & BCM_FCOE_HOST_ROUTE)
            && soc_mem_field32_get(unit, mem, buf, FCOE__LOCAL_ADDRESSf)) {
            route->flags |= BCM_FCOE_LOCAL_ADDRESS;
        }

        if (soc_mem_field32_get(unit, mem, buf, FCOE__DST_DISCARDf)) {
            route->flags |= BCM_FCOE_DST_DISCARD;
        }
        if (soc_mem_field32_get(unit, mem, buf, FCOE__RPEf)) {
            route->flags |= BCM_FCOE_RPE;
        }
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (dest_type == SOC_MEM_FIF_DEST_ECMP){
                route->flags |= BCM_FCOE_MULTIPATH;
            }
        } else
#endif
        {
            if (soc_mem_field32_get(unit, mem, buf, FCOE__ECMPf)) {  
                route->flags |= BCM_FCOE_MULTIPATH;
            }
        }

        BCM_IF_ERROR_RETURN(
            _bcm_fcoe_read_nexthop_into_route(unit, nexthop, route,
                                              FCOE_ROUTE_READ_RETURN_NHID));
    }

    return BCM_E_NONE;
}

/*
 * Description:
 *     Look up an FCOE routing table entry based on nport id
 * Parameters:
 *     unit      - device number
 *     route     - route information to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_find_ext(int unit, bcm_fcoe_route_t *route)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry[2];

    soc_mem_t  mem = _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit);
    uint32     *buf = (uint32 *)&l3_ext_entry[0];
    uint32     *lookup = (uint32 *)&l3_ext_entry[1];
    int        existing_index = 0;
    int        rv;

    memset(&l3_ext_entry, 0, sizeof l3_ext_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_ext(unit, mem, buf,
                                                          route));

    /* Search for this key in the existing database */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &existing_index, buf,
                        (void *)lookup, 0);
    if (rv == SOC_E_NOT_FOUND) {
        return BCM_E_NOT_FOUND;
    }

    _bcm_fcoe_read_mem_into_route(unit, mem, lookup, route);

    return BCM_E_NONE;
}

/*
 * Description:
 *     Look up an FCOE routing table entry based on nport id
 * Parameters:
 *     unit      - device number
 *     route     - route information to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_find_normal(int unit, bcm_fcoe_route_t *route)
{
    l3_entry_ipv4_unicast_entry_t   l3_entry[2];

    soc_mem_t  mem = _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit);
    uint32     *buf = (uint32 *)&l3_entry[0];
    uint32     *lookup = (uint32 *)&l3_entry[1];
    int        existing_index = 0;
    int        rv;

    memset(&l3_entry, 0, sizeof l3_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_normal(unit, mem, buf,
                                                             route));

    /* Search for this key in the existing database */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &existing_index, buf,
                        (void *)lookup, 0);
    if (rv == SOC_E_NOT_FOUND) {
        return BCM_E_NOT_FOUND;
    }

    _bcm_fcoe_read_mem_into_route(unit, mem, lookup, route);

    return BCM_E_NONE;
}

/*
 * Description:
 *     Look up an FCOE route in the L3_DEFIP table
 * Parameters:
 *     unit      - device number
 *     route     - route information to be deleted
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_route_find_defip(int unit, bcm_fcoe_route_t *route)
{
    _bcm_defip_cfg_t lpm_cfg;
    int nh_idx = 0;

    sal_memset(&lpm_cfg, 0, sizeof lpm_cfg);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_to_defip(unit, route, &lpm_cfg));
    BCM_IF_ERROR_RETURN(_bcm_fb_lpm_get(unit, &lpm_cfg, &nh_idx));
    BCM_IF_ERROR_RETURN(_bcm_fcoe_defip_to_route(unit, &lpm_cfg, route));
    if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
        if (route->flags & BCM_FCOE_MULTIPATH) {
            route->intf = nh_idx + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        } else {
            route->intf = nh_idx + BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Description:
 *     Look up an FCOE routing table entry based on nport id
 * Parameters:
 *     unit      - device number
 *     route     - route information to be deleted
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_fcoe_route_find(int unit, bcm_fcoe_route_t *route)
{
#ifdef INCLUDE_L3
    int rv = _bcm_td2_fcoe_route_find_normal(unit, route);

    if (rv != BCM_E_NONE) {
        rv = _bcm_td2_fcoe_route_find_ext(unit, route);
    }

    if (rv != BCM_E_NONE) {
        rv = _bcm_td2_fcoe_route_find_defip(unit, route);
    }
#else
    int rv = BCM_E_UNAVAIL;
#endif
    
    return rv;
}

/* validate vsan entry input */
STATIC int
_bcm_fcoe_vsan_param_validate (bcm_fcoe_vsan_t *vsan, bcm_fcoe_vsan_id_t vsan_id)
{
    if ((vsan_id > FCOE_MAX_VSAN_ID) ||
        (vsan->fcmap > 0xffffff)) {     /* fcmap is 24 bits */
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/* return fcmap value for given index */
STATIC int
_bcm_fcoe_fc_profile_map_value_get (int unit, int index,
                                    bcm_fcoe_fcmap_t *fcmap)
{
    int       rv;
    uint32    entry[SOC_MAX_MEM_FIELD_WORDS];
    void      *entries[1];

    entries[0] = entry;
    rv = _bcm_fc_map_profile_entry_get(unit, index, 1, entries);

    if (rv == BCM_E_NONE) {
        *fcmap = entry[0];
    }

    return rv;
}

/* Allocate and configure a VSAN */
int 
bcm_td2_fcoe_vsan_create (int unit, uint32 options, bcm_fcoe_vsan_t *vsan,
                          bcm_fcoe_vsan_id_t *vsan_id)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index = *vsan_id;
    uint32            fcmap_index = 0;
    void              *entries[1];
    uint32            fcmap_to_set;

    rv = _bcm_fcoe_vsan_param_validate(vsan, *vsan_id);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    /* check if vsan id is already created */
    if (shr_idxres_list_elem_state(vsan_id_list[unit], *vsan_id) !=
            BCM_E_NOT_FOUND) {
        /* exception : if intention is to replace existing ID, overwrite
         * existing entry */
        if ((options & BCM_FCOE_VSAN_OPTION_REPLACE) == 0) {
            return BCM_E_EXISTS;
        }
    }

    fcmap_to_set = vsan->fcmap;
    entries[0] = &fcmap_to_set;

    rv = _bcm_fc_map_profile_entry_add(unit, entries, 1, &fcmap_index);

    if (rv != SOC_E_NONE) {
        return rv;
    }

    /* read table entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    if (vsan->config_flags & BCM_FCOE_VSAN_CONFIG_NORMALIZED_ZONE_CHECK) {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                            ZONE_CHECK_NORMALIZEDf, 1);
    } else {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                            ZONE_CHECK_NORMALIZEDf, 0);
    }
    if (vsan->config_flags & BCM_FCOE_VSAN_CONFIG_FCOE_ROUTE_ENABLE) {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                            FCOE_ROUTE_ENABLEf, 1);
    } else {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                            FCOE_ROUTE_ENABLEf, 0);
    }

    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                        L3_IIFf, vsan->l3_intf_id);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                        FCOE_FC_MAP_INDEXf, fcmap_index);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &vsan_entry);

    soc_mem_unlock(unit, mem);

    if (rv == BCM_E_NONE) {
        /* mark this vsan id as "created" */
        if (shr_idxres_list_elem_state(vsan_id_list[unit], *vsan_id) ==
            BCM_E_NOT_FOUND) {
            return _bcm_fcoe_reserve_vsan_id(unit, *vsan_id);
        }
    }
    return rv;
}

/* Destroy a VSAN */
int 
bcm_td2_fcoe_vsan_destroy (int unit,  bcm_fcoe_vsan_id_t vsan_id)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index = vsan_id;
    int               fcmap_index;

    /* check if vsan id is existing */
    if (shr_idxres_list_elem_state(vsan_id_list[unit], vsan_id) !=
            BCM_E_EXISTS) {
        return BCM_E_NOT_FOUND;
    }

    /* read table entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* delete fc map entry */
    fcmap_index = soc_mem_field32_get(unit,mem, &vsan_entry, FCOE_FC_MAP_INDEXf);
    rv = _bcm_fc_map_profile_entry_delete(unit, fcmap_index);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* NOTE : zero out all fields (including flex counter fields)
     */ 
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                        FLEX_CTR_OFFSET_MODEf, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                        FLEX_CTR_POOL_NUMBERf, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                        FLEX_CTR_BASE_COUNTER_IDXf, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                        FCOE_ROUTE_ENABLEf, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                        ZONE_CHECK_NORMALIZEDf, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, L3_IIFf, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                        FCOE_FC_MAP_INDEXf, 0);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &vsan_entry);

    soc_mem_unlock(unit, mem);

    if (rv == BCM_E_NONE) {
        /* unreserve this vsan id */
        return _bcm_fcoe_unreserve_vsan_id(unit, vsan_id);
    }
    return rv;
}

/* Destroy all VSANs */
int 
bcm_td2_fcoe_vsan_destroy_all (int unit)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index;
    int               fcmap_index;

    soc_mem_lock(unit, mem);
    for (index = 0; index <= FCOE_MAX_VSAN_ID; index++) {

        /* check if vsan id is existing */
        if (shr_idxres_list_elem_state(vsan_id_list[unit], index) !=
                BCM_E_EXISTS) {
            continue;
        }

        /* read table entry */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
        if (rv != BCM_E_NONE) {
            break;
        }

        /* delete fc map entry */
        fcmap_index = soc_mem_field32_get(unit,mem, &vsan_entry, 
                                          FCOE_FC_MAP_INDEXf);
        rv = _bcm_fc_map_profile_entry_delete(unit, fcmap_index);
        if (rv != BCM_E_NONE) {
            soc_mem_unlock(unit, mem);
            return rv;
        }

        /* NOTE : zero out all fields (including flex counter fields)
         */ 
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FLEX_CTR_OFFSET_MODEf, 0);
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FLEX_CTR_POOL_NUMBERf, 0);
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FLEX_CTR_BASE_COUNTER_IDXf, 0);
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FCOE_ROUTE_ENABLEf, 0);
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            ZONE_CHECK_NORMALIZEDf, 0);
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, L3_IIFf, 0);
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FCOE_FC_MAP_INDEXf, 0);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &vsan_entry);

        if (rv == BCM_E_NONE) {
            /* unreserve this vsan id */
            rv = _bcm_fcoe_unreserve_vsan_id(unit, index);
            if (rv != BCM_E_NONE) {
                break;
            }
        } else {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    return rv;
}

/* Get VSAN controls */
int 
bcm_td2_fcoe_control_vsan_get (int unit,  
                               bcm_fcoe_vsan_id_t vsan_id, 
                               bcm_fcoe_vsan_control_t type, 
                               int *arg)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index = vsan_id;

    if (vsan_id > FCOE_MAX_VSAN_ID) {
        return BCM_E_PARAM;
    }

    /* read table entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    soc_mem_unlock(unit, mem);

    switch (type) {
        case bcmFcoeVsanNormalizedZoneCheck:
            *arg = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry, 
                                       ZONE_CHECK_NORMALIZEDf);
            break;

        case bcmFcoeVsanRouteEnable:
            *arg = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry, 
                                      FCOE_ROUTE_ENABLEf);
            break;

        default:
            rv = BCM_E_PARAM;
            break;
    }

    return rv;
}

/* Set VSAN controls */
int 
bcm_td2_fcoe_control_vsan_set (int unit, 
                               bcm_fcoe_vsan_id_t vsan_id, 
                               bcm_fcoe_vsan_control_t type, 
                               int arg)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index = vsan_id;

    if (vsan_id > FCOE_MAX_VSAN_ID) {
        return BCM_E_PARAM;
    }

    /* read table entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    switch (type) {
        case bcmFcoeVsanNormalizedZoneCheck:
            soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                                ZONE_CHECK_NORMALIZEDf, (arg != 0) ? 1 : 0);
            break;

        case bcmFcoeVsanRouteEnable:
            soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                                FCOE_ROUTE_ENABLEf, (arg != 0) ? 1 : 0);
            break;

        default:
            rv = BCM_E_PARAM;
            break;
    }

    if (rv == BCM_E_NONE) {
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &vsan_entry);
    }
    soc_mem_unlock(unit, mem);
    return rv;
}

/* Get VSAN properties associated with a VSAN ID */
int 
bcm_td2_fcoe_vsan_get (int unit, bcm_fcoe_vsan_id_t vsan_id, 
                       bcm_fcoe_vsan_t *vsan)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index = vsan_id;
    int               flag = 0;
    int               fcmap_index = 0;
    bcm_fcoe_fcmap_t  fcmap;

    if (vsan_id > FCOE_MAX_VSAN_ID) {
        return BCM_E_PARAM;
    }

    /* NOTE : for now, reading a not-initialized(not created) entry is not
     *        allowed. we may change this behavior....
     */
    if (shr_idxres_list_elem_state(vsan_id_list[unit], vsan_id) !=
            BCM_E_EXISTS) {
        return BCM_E_NOT_FOUND;
    }

    /* read table entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
    soc_mem_unlock(unit, mem);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    fcmap_index = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry,
                                      FCOE_FC_MAP_INDEXf);
    /* get fcmap value for fcmap index */
    rv = _bcm_fcoe_fc_profile_map_value_get (unit, fcmap_index, &fcmap);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    vsan->fcmap = fcmap;

    vsan->config_flags = 0;
    flag = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry, 
                               ZONE_CHECK_NORMALIZEDf);
    if (flag) {
        vsan->config_flags |= BCM_FCOE_VSAN_CONFIG_NORMALIZED_ZONE_CHECK;
    }
    flag = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry, 
                               FCOE_ROUTE_ENABLEf);
    if (flag) {
        vsan->config_flags |= BCM_FCOE_VSAN_CONFIG_FCOE_ROUTE_ENABLE;
    }

    vsan->l3_intf_id = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry,
                                           L3_IIFf);
    vsan->fcmap = fcmap;

    return rv;
}

/* Update VSAN properties associated with a VSAN ID */
int 
bcm_td2_fcoe_vsan_set (int unit, bcm_fcoe_vsan_id_t vsan_id, 
                       bcm_fcoe_vsan_t *vsan)
{
    ing_vsan_entry_t  vsan_entry;
    soc_mem_t         mem = ING_VSANm;
    int               rv = BCM_E_NONE;
    int               index = vsan_id;
    uint32            fcmap_index = 0;
    int               old_fcmap_index;
    void              *entries[1];
    uint32            fcmap_to_set;

    if (vsan_id > FCOE_MAX_VSAN_ID) {
        return BCM_E_PARAM;
    }

    /* check if vsan id is already created */
    if (shr_idxres_list_elem_state(vsan_id_list[unit], vsan_id) !=
            BCM_E_EXISTS) {
        return BCM_E_NOT_FOUND;
    }

    /* read table entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
    if (rv != BCM_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    old_fcmap_index = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry,
                                          FCOE_FC_MAP_INDEXf);
    fcmap_to_set = vsan->fcmap;
    entries[0] = &fcmap_to_set;

    rv = _bcm_fc_map_profile_entry_add(unit, entries, 1, &fcmap_index);

    if (rv != SOC_E_NONE) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    if (vsan->config_flags & BCM_FCOE_VSAN_CONFIG_NORMALIZED_ZONE_CHECK) {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            ZONE_CHECK_NORMALIZEDf, 1);
    } else {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            ZONE_CHECK_NORMALIZEDf, 0);
    }
    if (vsan->config_flags & BCM_FCOE_VSAN_CONFIG_FCOE_ROUTE_ENABLE) {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FCOE_ROUTE_ENABLEf, 1);
    } else {
        soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry, 
                            FCOE_ROUTE_ENABLEf, 0);
    }

    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                        L3_IIFf, vsan->l3_intf_id);
    soc_mem_field32_set(unit, mem, (uint32 *)&vsan_entry,
                        FCOE_FC_MAP_INDEXf, fcmap_index);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &vsan_entry);

    if (rv == BCM_E_NONE) {
        rv = _bcm_fc_map_profile_entry_delete(unit, old_fcmap_index);
    }

    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Description:
 *      Check that parameters sent to bcm_td2_fcoe_fc_header_type_* functions
 *      are valid.
 * Parameters:
 *      flags      - (IN) BCM_FCOE_FC_HEADER_TYPE_INGRESS/EGRESS
 *      hdr_type   - (IN) Header type to force this value of r_ctl to use
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_fc_header_sanity_check(uint32 flags, 
                                     bcm_fcoe_fc_header_type_t hdr_type)
{
    int rv = BCM_E_NONE;

    if (hdr_type > bcmFcoeFcHeaderTypeCount 
        || hdr_type < bcmFcoeFcHeaderTypeNone) {
        rv = BCM_E_PARAM;
    }

    if (!(flags & BCM_FCOE_FC_HEADER_TYPE_INGRESS)
        && !(flags & BCM_FCOE_FC_HEADER_TYPE_EGRESS)) {
        rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Description:
 *      Convert a hardware value of header type to the corresponding
 *      bcm_fcoe_fc_header_type_t enum
 * Parameters:
 *      encoded_value - (IN) hardware value to convert
 *      hdr_type      - (OUT) resulting header type enum
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_hdr_encode_to_hdr_type(uint32 encoded_value, 
                                     bcm_fcoe_fc_header_type_t *hdr_type)
{
    int rv = BCM_E_NONE;

    switch (encoded_value) {
        case FC_HEADER_TYPE_NOT_PRESENT:
            *hdr_type = bcmFcoeFcHeaderTypeNone;
            break;

        case FC_HEADER_TYPE_STANDARD:
            *hdr_type = bcmFcoeFcHeaderTypeStd;
            break;

        case FC_HEADER_TYPE_ENCAPSULATION:
            *hdr_type = bcmFcoeFcHeaderTypeEncap;
            break;

        case FC_HEADER_TYPE_VFT:
            *hdr_type = bcmFcoeFcHeaderTypeVft;
            break;

        case FC_HEADER_TYPE_IFR:
            *hdr_type = bcmFcoeFcHeaderTypeIfr;
            break;

        case FC_HEADER_TYPE_UNKNOWN:
            *hdr_type = bcmFcoeFcHeaderTypeUnknown;
            break;

        default:
            rv = BCM_E_PARAM;
            break;
    }

    return rv;
}

/*
 * Description:
 *      Convert a bcm_fcoe_fc_header_type_t enum to the corresponding value that
 *      should be programmed to hardware
 * Parameters:
 *      hdr_type      - (IN) header type enum to convert
 *      encoded_value - (OUT) resulting hardware value
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_hdr_type_to_hdr_encode(bcm_fcoe_fc_header_type_t hdr_type, 
                                     uint32 *encoded_value)
{
    int rv = BCM_E_NONE;

    switch (hdr_type) {
        case bcmFcoeFcHeaderTypeNone:
            *encoded_value = FC_HEADER_TYPE_NOT_PRESENT;
            break;

        case bcmFcoeFcHeaderTypeStd:
            *encoded_value = FC_HEADER_TYPE_STANDARD;
            break;

        case bcmFcoeFcHeaderTypeEncap:
            *encoded_value = FC_HEADER_TYPE_ENCAPSULATION;
            break;

        case bcmFcoeFcHeaderTypeVft:
            *encoded_value = FC_HEADER_TYPE_VFT;
            break;

        case bcmFcoeFcHeaderTypeIfr:
            *encoded_value = FC_HEADER_TYPE_IFR;
            break;

        case bcmFcoeFcHeaderTypeUnknown:
            *encoded_value = FC_HEADER_TYPE_UNKNOWN;
            break;

        default:
            rv = BCM_E_PARAM;
            break;
    }

    return rv;
}

/*
 * Description:
 *      Write header type to hardware
 * Parameters:
 *      unit       - (IN) device number
 *      mem        - (IN) device memory to write
 *      r_ctl      - (IN) [0-255] value of r_ctl to write
 *      hdr_encode - (IN) Header type to store at this r_ctl value
 * Return:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_fc_program_header(int unit, soc_mem_t mem, uint8 r_ctl, 
                                uint32 hdr_encode)
{
    ing_fc_header_type_entry_t ing_entry;
    egr_fc_header_type_entry_t egr_entry;
    uint32                     *buf = NULL;
    int                        rv = BCM_E_NONE;

    sal_memset(&ing_entry, 0, sizeof ing_entry);
    sal_memset(&egr_entry, 0, sizeof egr_entry);

    if ((mem == ING_FC_HEADER_TYPEm) ||
        (mem == PARSER1_ING_FC_HEADER_TYPEm) ||
        (mem == PARSER2_ING_FC_HEADER_TYPEm)) {
        buf = (uint32 *)&ing_entry;
    } else if (mem == EGR_FC_HEADER_TYPEm) {
        buf = (uint32 *)&egr_entry;
    } else {
        return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, mem, buf, FC_HDR_ENCODEf, hdr_encode);

    soc_mem_lock(unit, mem);
    rv = BCM_XGS3_MEM_WRITE(unit, mem, r_ctl, buf);
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Description:
 *      Read header type from hardware and return it
 * Parameters:
 *      unit       - (IN) device number
 *      mem        - (IN) device memory to read
 *      r_ctl      - (IN) [0-255] value of r_ctl to read
 *      hdr_encode - (OUT) Header type stored at this r_ctl value
 * Return:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2_fcoe_fc_read_header(int unit, soc_mem_t mem, uint8 r_ctl, 
                             uint32 *hdr_encode)
{
    ing_fc_header_type_entry_t ing_entry;
    egr_fc_header_type_entry_t egr_entry;
    uint32                     *buf = NULL;
    int                        rv = BCM_E_NONE;

    if ((mem == ING_FC_HEADER_TYPEm) ||
        (mem == PARSER1_ING_FC_HEADER_TYPEm) ||
        (mem == PARSER2_ING_FC_HEADER_TYPEm)) {
        buf = (uint32 *)&ing_entry;
    } else if (mem == EGR_FC_HEADER_TYPEm) {
        buf = (uint32 *)&egr_entry;
    } else {
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, mem);
    rv = BCM_XGS3_MEM_READ(unit, mem, r_ctl, buf);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    soc_mem_unlock(unit, mem);

    *hdr_encode = soc_mem_field32_get(unit, mem, buf, FC_HDR_ENCODEf);

    return rv;
}

/*
 * Description:
 *      Set FC header type
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) BCM_FCOE_FC_HEADER_TYPE_INGRESS/EGRESS
 *      r_ctl      - (IN) [0-255] value of r_ctl to modify
 *      hdr_type   - (IN) Header type to force this value of r_ctl to use
 * Return:
 *      BCM_E_XXX
 */
 int
 bcm_td2_fcoe_fc_header_type_set(int unit, uint32 flags, uint8 r_ctl, 
                                 bcm_fcoe_fc_header_type_t hdr_type)
{
    int rv = BCM_E_NONE;
    uint32 encoded_value = 0;

    rv = _bcm_td2_fcoe_fc_header_sanity_check(flags, hdr_type);

    if (rv == BCM_E_NONE) {
        rv = _bcm_td2_fcoe_hdr_type_to_hdr_encode(hdr_type, &encoded_value);
    }

    if ((rv == BCM_E_NONE) && (flags & BCM_FCOE_FC_HEADER_TYPE_INGRESS)) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            rv = _bcm_td2_fcoe_fc_program_header(unit, PARSER1_ING_FC_HEADER_TYPEm, 
                                                 r_ctl, encoded_value);
            if (rv == BCM_E_NONE) {
                rv = _bcm_td2_fcoe_fc_program_header(unit, PARSER2_ING_FC_HEADER_TYPEm, 
                                                 r_ctl, encoded_value);
            }
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            rv = _bcm_td2_fcoe_fc_program_header(unit, ING_FC_HEADER_TYPEm, 
                                                 r_ctl, encoded_value);
        }
    }

    if ((rv == BCM_E_NONE) && (flags & BCM_FCOE_FC_HEADER_TYPE_EGRESS)) {
#ifdef BCM_TRIDENT3_SUPPORT
        if (!(SOC_IS_TRIDENT3X(unit)))
#endif /* BCM_TRIDENT3_SUPPORT */
        {
            rv = _bcm_td2_fcoe_fc_program_header(unit, EGR_FC_HEADER_TYPEm, 
                                             r_ctl, encoded_value);
        }
    }

    return rv;
}

/*
 * Description:
 *      Retrieve FC header type
 * Parameters:
 *      unit       - (IN) device number
 *      flags      - (IN) BCM_FCOE_FC_HEADER_TYPE_INGRESS/EGRESS
 *      r_ctl      - (IN) [0-255] value of r_ctl to retrieve
 *      hdr_type   - (OUT) Header type that this value of r_ctl is forced to
 * Return:
 *      BCM_E_XXX
 */
 int
 bcm_td2_fcoe_fc_header_type_get(int unit, uint32 flags, uint8 r_ctl, 
                                 bcm_fcoe_fc_header_type_t *hdr_type)
{
    int rv = BCM_E_NONE, mem;
    uint32 encoded_value = 0;

    if ((flags & BCM_FCOE_FC_HEADER_TYPE_INGRESS)
        && (flags & BCM_FCOE_FC_HEADER_TYPE_EGRESS)) {
        return (BCM_E_PARAM);
    }

    if (flags & BCM_FCOE_FC_HEADER_TYPE_INGRESS) {
        mem = (SOC_IS_TRIDENT3X(unit)) ?  PARSER1_ING_FC_HEADER_TYPEm :
                                         ING_FC_HEADER_TYPEm;
    } else if (flags & BCM_FCOE_FC_HEADER_TYPE_EGRESS) {
        mem = (SOC_IS_TRIDENT3X(unit)) ? PARSER2_ING_FC_HEADER_TYPEm :
                                       EGR_FC_HEADER_TYPEm;
    } else {
        return (BCM_E_PARAM);
    }
    rv = _bcm_td2_fcoe_fc_read_header(unit, mem, r_ctl, &encoded_value);

    if (rv == BCM_E_NONE) {
        rv = _bcm_td2_fcoe_hdr_encode_to_hdr_type(encoded_value, hdr_type);
    }

    return rv;
}

/*
 * Provides relevant flex table information(table-name,index with
 *  direction) for given VSAN ID.
 */
STATIC bcm_error_t 
_bcm_td2_vsan_stat_get_table_info (int                        unit,
                                   bcm_fcoe_vsan_id_t         vsan,
                                   uint32                     *num_of_tables,
                                   bcm_stat_flex_table_info_t *table_info)
{
    int index = 0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Get index into ING_VSAN table. VSAN ID is index (12 bits). */
    if (vsan > 0xfff) {
        return BCM_E_PARAM;
    }
    index = (int)vsan;

    /* Ingress VSAN */
    table_info[*num_of_tables].table=ING_VSANm;
    table_info[*num_of_tables].index=index;
    table_info[*num_of_tables].direction=bcmStatFlexDirectionIngress;
    (*num_of_tables)++;

    return BCM_E_NONE;
}

/* Allocate flex counter and assign counter index/offset for
 * FCOE VSAN ID
 */
int 
bcm_td2_fcoe_vsan_stat_attach (int                 unit,
                               bcm_fcoe_vsan_id_t  vsan,
                               uint32              stat_counter_id)
{
    int                        rv = BCM_E_NONE;
    int                        counter_flag = 0;
    soc_mem_t                  table = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];


    /* Get pool, base index group mode and offset modes from stat counter id */
    _bcm_esw_stat_get_counter_id_info(
                  unit, stat_counter_id,
                  &group_mode, &object, &offset_mode, &pool_number, 
                  &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(
                        unit, object, 1, &actual_num_tables, &table, 
                        &direction));
    BCM_IF_ERROR_RETURN(_bcm_td2_vsan_stat_get_table_info(
                        unit, vsan, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
                  counter_flag = 1;
                  rv = _bcm_esw_stat_flex_attach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         offset_mode,
                         base_index,
                         pool_number);

                if(BCM_FAILURE(rv)) {
                        break;
                  }

              } else {
                  /* no egress table...*/
              } 
         }
    }

    if (counter_flag == 0) {
        rv = BCM_E_NOT_FOUND;
    }
    
    return rv;
}

/* Detach flex counter for FCOE VSAN ID  */
int 
bcm_td2_fcoe_vsan_stat_detach (int unit, bcm_fcoe_vsan_id_t  vsan)
{
    int                        counter_flag = 0;
    uint32                     count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    int                        rv = BCM_E_NONE;
    

    /* Get the table index to be detached */
    BCM_IF_ERROR_RETURN(_bcm_td2_vsan_stat_get_table_info(
                   unit, vsan, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             counter_flag = 1;
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
         } else {
             /* no egress table...*/
         }
    }

    if (counter_flag == 0) {
        return BCM_E_NOT_FOUND;
    }

    return rv;
}

/* Get flex counter for FCOE VSAN ID */                          
int 
bcm_td2_fcoe_vsan_stat_counter_get (int                  unit, 
                                    int                  sync_mode,
                                    bcm_fcoe_vsan_id_t   vsan,
                                    bcm_fcoe_vsan_stat_t stat, 
                                    uint32               num_entries, 
                                    uint32               *counter_indexes, 
                                    bcm_stat_value_t     *counter_values)
{
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmFcoeVsanInPackets) ||
        (stat == bcmFcoeVsanInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         return BCM_E_PARAM;
    }
    if (stat == bcmFcoeVsanInBytes) {
        byte_flag = 1;
    }

    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_td2_vsan_stat_get_table_info(
                        unit, vsan, &num_of_tables, &table_info[0]));

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_get(
                                      unit, sync_mode,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }

    return BCM_E_NONE;
}

/* Set flex counter for FCOE VSAN ID */                               
int 
bcm_td2_fcoe_vsan_stat_counter_set (int                  unit, 
                                    bcm_fcoe_vsan_id_t   vsan,
                                    bcm_fcoe_vsan_stat_t stat, 
                                    uint32               num_entries, 
                                    uint32               *counter_indexes, 
                                    bcm_stat_value_t     *counter_values)
{
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat == bcmFcoeVsanInPackets) ||
        (stat == bcmFcoeVsanInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         return BCM_E_PARAM;
    }
    if (stat == bcmFcoeVsanInBytes) {
        byte_flag = 1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(_bcm_td2_vsan_stat_get_table_info(
                        unit, vsan, &num_of_tables, &table_info[0]));

    /* Set the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries; index_count++) {
                  BCM_IF_ERROR_RETURN(_bcm_esw_stat_counter_set(
                                      unit,
                                      table_info[table_count].index,
                                      table_info[table_count].table,
                                      0,
                                      byte_flag,
                                      counter_indexes[index_count],
                                      &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
}

/* Get stat counter ID for FCOE VSAN ID */                               
int 
bcm_td2_fcoe_vsan_stat_id_get (int                  unit,
                               bcm_fcoe_vsan_id_t   vsan,
                               bcm_fcoe_vsan_stat_t stat, 
                               uint32               *stat_counter_id)
{
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index = 0;
    uint32                     num_stat_counter_ids = 0;

    if ((stat == bcmFcoeVsanInPackets) ||
        (stat == bcmFcoeVsanInBytes)) {
         direction = bcmStatFlexDirectionIngress;
    } else {
         return BCM_E_PARAM;
    }

    /*  Get Tables, for which flex counter are attached  */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_vsan_stat_get_table_info(unit, vsan, &num_of_tables, 
                                          &table_info[0]));

    /* Retrieve stat counter id */
    for (index = 0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction) {
             return _bcm_esw_stat_flex_get_counter_id(
                                  unit, 1, &table_info[index],
                                  &num_stat_counter_ids, stat_counter_id);
         }
    }
    return BCM_E_NOT_FOUND;
}
                               
#if defined (INCLUDE_L3)

STATIC int
_bcm_td2_fcoe_route_index_get_ext(int unit, bcm_fcoe_route_t *route, int *index)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry[2];

    soc_mem_t  mem = _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit);
    uint32     *buf = (uint32 *)&l3_ext_entry[0];
    uint32     *lookup = (uint32 *)&l3_ext_entry[1];

    memset(&l3_ext_entry, 0, sizeof l3_ext_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_ext(unit, mem, buf,
                                                          route));

    /* Search for this key in the existing database */
    return soc_mem_search(unit, mem, MEM_BLOCK_ANY, index, buf,
                          (void *)lookup, 0);
}


STATIC int
_bcm_td2_fcoe_route_index_get_normal(int unit, bcm_fcoe_route_t *route, int *index)
{
    l3_entry_ipv4_unicast_entry_t   l3_entry[2];

    soc_mem_t  mem = _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit);
    uint32     *buf = (uint32 *)&l3_entry[0];
    uint32     *lookup = (uint32 *)&l3_entry[1];

    memset(&l3_entry, 0, sizeof l3_entry);

    BCM_IF_ERROR_RETURN(_bcm_fcoe_route_construct_key_normal(unit, mem, buf,
                                                             route));

    /* Search for this key in the existing database */
    return soc_mem_search(unit, mem, MEM_BLOCK_ANY, index, buf,
                          (void *)lookup, 0);
}


STATIC int
_bcm_td2_fcoe_route_index_get(int unit, bcm_fcoe_route_t *route, int *index)
{
    int rv;

    rv = _bcm_td2_fcoe_route_index_get_normal(unit, route, index);

    if (rv != BCM_E_NONE) {
        rv = _bcm_td2_fcoe_route_index_get_ext(unit, route, index);
    }

    return rv;
}

/*
 * Provides relevant flex table information(table-name,index with
 *  direction)  for given route info.
 */
STATIC bcm_error_t 
_bcm_td2_fcoe_route_stat_get_table_info (
                                   int                        unit,
                                   bcm_fcoe_route_t           *route,
                                   uint32                     *num_of_tables,
                                   bcm_stat_flex_table_info_t *table_info)
{
    int index = 0;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    /* Get unique index for the route info.
     * Support all fcoe types: FCOE_DOMAIN_EXT, FCOE_HOST_EXT,
     *                         FCOE_SOURCE_MAP_EXT
     */
    if (_bcm_td2_fcoe_route_index_get(unit, route, &index) != BCM_E_NONE) {
        return BCM_E_NOT_FOUND;
    }

    /* Ingress FCOE */
    table_info[*num_of_tables].table = _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit);
    table_info[*num_of_tables].index = (uint32)index;
    table_info[*num_of_tables].direction = bcmStatFlexDirectionIngress;
    (*num_of_tables)++;

    return BCM_E_NONE;
}
#endif  /* INCLUDE_L3*/

/* Allocate flex counter and assign counter index/offset for  
 * FCOE Route
 */
int 
bcm_td2_fcoe_route_stat_attach (int                 unit,
                                bcm_fcoe_route_t    *route,
                                uint32              stat_counter_id)
{
#if defined(INCLUDE_L3)

    int                        rv = BCM_E_NONE;
    int                        counter_flag = 0;
    soc_mem_t                  table = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object = bcmStatObjectIngPort;
    bcm_stat_group_mode_t      group_mode = bcmStatGroupModeSingle;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if (!BCM_XGS3_L3_EGRESS_MODE_ISSET(unit) ||
        (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, route->intf)) ||
        (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, route->intf)) ||
        (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, route->intf)) ||
        (BCM_XGS3_PROXY_EGRESS_IDX_VALID(unit, route->intf))) {
        return BCM_E_PARAM;
    }

    /* Get pool, base index group mode and offset modes from stat counter id */
    _bcm_esw_stat_get_counter_id_info(unit, stat_counter_id, &group_mode, &object, 
                                      &offset_mode, &pool_number, &base_index);

    /* Validate object and group mode */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));

    /* Get Table index to attach flexible counter */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_stat_flex_get_table_info(unit, object, 1, &actual_num_tables, 
                                          &table, &direction));
    BCM_IF_ERROR_RETURN(
        _bcm_td2_fcoe_route_stat_get_table_info(unit, route, &num_of_tables, 
                                          &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
         if ((table_info[count].direction == direction) &&
             (table_info[count].table == table) ) {
              if (direction == bcmStatFlexDirectionIngress) {
                  counter_flag = 1;
                  rv = _bcm_esw_stat_flex_attach_ingress_table_counters(
                         unit,
                         table_info[count].table,
                         table_info[count].index,
                         offset_mode,
                         base_index,
                         pool_number);
                if(BCM_FAILURE(rv)) {
                        break;
                  }
              } else {
                  /* shouldn't reach here, only ingress is supported */
              } 
         }
    }

    if (counter_flag == 0) {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* INCLUDE_L3 */
}

/* Detach flex counter for FCOE Route */                                
int 
bcm_td2_fcoe_route_stat_detach (int unit, bcm_fcoe_route_t *route)
{
#if defined(INCLUDE_L3)
    int                        counter_flag = 0;
    uint32                     count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    int                        rv = BCM_E_NONE;

    /* Get the table index to be detached */
    BCM_IF_ERROR_RETURN(_bcm_td2_fcoe_route_stat_get_table_info(
                        unit, route, &num_of_tables, &table_info[0]));

    for (count = 0; count < num_of_tables; count++) {
         if (table_info[count].direction == bcmStatFlexDirectionIngress) {
             counter_flag = 1;
             rv = _bcm_esw_stat_flex_detach_ingress_table_counters(
                        unit, table_info[count].table, table_info[count].index);
         } else {
             /* no egress support */
         }
    }

    if (counter_flag == 0) {
        return BCM_E_NOT_FOUND;
    }

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* INCLUDE_L3 */
}

/* Get flex counter for FCOE Route */                                
int 
bcm_td2_fcoe_route_stat_counter_get (int                   unit, 
                                     int                   sync_mode, 
                                     bcm_fcoe_route_t      *route,
                                     bcm_fcoe_route_stat_t stat, 
                                     uint32                num_entries, 
                                     uint32                *counter_indexes, 
                                     bcm_stat_value_t      *counter_values)
{
#if defined(INCLUDE_L3)
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat != bcmFcoeRouteInPackets) && (stat != bcmFcoeRouteInBytes)) {
        return BCM_E_PARAM;
    }

    if (stat == bcmFcoeRouteInBytes) {
        byte_flag = 1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_fcoe_route_stat_get_table_info(unit, route, &num_of_tables, 
                                          &table_info[0]));

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries ; index_count++) {
                  BCM_IF_ERROR_RETURN(
                      _bcm_esw_stat_counter_get(unit, sync_mode,
                                                table_info[table_count].index,
                                                table_info[table_count].table,
                                                0,
                                                byte_flag,
                                                counter_indexes[index_count],
                                                &counter_values[index_count]));
             }
         }
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif  /* INCLUDE_L3 */
}

/* Set flex counter for FCOE Route */
int 
bcm_td2_fcoe_route_stat_counter_set (int                   unit, 
                                     bcm_fcoe_route_t      *route,
                                     bcm_fcoe_route_stat_t stat, 
                                     uint32                num_entries, 
                                     uint32                *counter_indexes, 
                                     bcm_stat_value_t      *counter_values)
{
#if defined(INCLUDE_L3)
    uint32                     table_count = 0;
    uint32                     index_count = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     byte_flag = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    if ((stat != bcmFcoeRouteInPackets) && (stat != bcmFcoeRouteInBytes)) {
        return BCM_E_PARAM;
    }

    if (stat == bcmFcoeRouteInBytes) {
        byte_flag = 1;
    }
   
    /*  Get Table index to read the flex counter */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_fcoe_route_stat_get_table_info(unit, route, &num_of_tables, 
                                          &table_info[0]));

    /* Get the flex counter for the attached table index */
    for (table_count = 0; table_count < num_of_tables ; table_count++) {
         if (table_info[table_count].direction == direction) {
             for (index_count = 0; index_count < num_entries; index_count++) {
                  BCM_IF_ERROR_RETURN(
                      _bcm_esw_stat_counter_set(unit,
                                                table_info[table_count].index,
                                                table_info[table_count].table,
                                                0,
                                                byte_flag,
                                                counter_indexes[index_count],
                                                &counter_values[index_count]));
             }
         }
    }
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif  /* INCLUDE_L3 */
}
       
/* Get stat counter ID for FCOE Route */       
int 
bcm_td2_fcoe_route_stat_id_get (int                   unit,
                                bcm_fcoe_route_t      *route,
                                bcm_fcoe_route_stat_t stat, 
                                uint32                *stat_counter_id)
{
#if defined(INCLUDE_L3)
    bcm_stat_flex_direction_t  direction = bcmStatFlexDirectionIngress;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];
    uint32                     index = 0;
    uint32                     num_stat_counter_ids = 0;

    if ((stat != bcmFcoeRouteInPackets) && (stat != bcmFcoeRouteInBytes)) {
        return BCM_E_PARAM;
    }

    /*  Get Tables, for which flex counter are attached  */
    BCM_IF_ERROR_RETURN(
        _bcm_td2_fcoe_route_stat_get_table_info(unit, route, &num_of_tables, 
                                          &table_info[0]));

    /* Retrieve stat counter id */
    for (index = 0; index < num_of_tables ; index++) {
         if (table_info[index].direction == direction)
             return _bcm_esw_stat_flex_get_counter_id(unit, 
                                                      1, 
                                                      &table_info[index],
                                                      &num_stat_counter_ids,
                                                      stat_counter_id);
    }
    return BCM_E_NOT_FOUND;
#else
    return BCM_E_UNAVAIL;
#endif  /* INCLUDE_L3 */
}

#ifdef INCLUDE_L3

/* 
 * Purpose:
 *   Require exactly 1 of the following 3 flags:
 *     BCM_FCOE_INTF_VFT_DO_NOT_MODIFY
 *     BCM_FCOE_INTF_VFT_ADD_REPLACE
 *     BCM_FCOE_INTF_VFT_DELETE
 *
 * Parameters: 
 *   flags      - (IN)  flags given by the application
 *   vft_action - (OUT) the option that was chosen
 *
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_bcm_td2_vft_action_from_flags(uint32 flags, int *vft_action)
{
    if (flags & BCM_FCOE_INTF_VFT_DO_NOT_MODIFY) {
        CHECK_FLAGS_NOT_SET(flags, BCM_FCOE_INTF_VFT_ADD_REPLACE
                                   | BCM_FCOE_INTF_VFT_DELETE);
        *vft_action = FCOE_VFT_ACTION_DO_NOT_MODIFY;
    } else if (flags & BCM_FCOE_INTF_VFT_ADD_REPLACE) {
        CHECK_FLAGS_NOT_SET(flags, BCM_FCOE_INTF_VFT_DO_NOT_MODIFY
                                   | BCM_FCOE_INTF_VFT_DELETE);
        *vft_action = FCOE_VFT_ACTION_ADD_REPLACE;
    } else if (flags & BCM_FCOE_INTF_VFT_DELETE) {
        CHECK_FLAGS_NOT_SET(flags, BCM_FCOE_INTF_VFT_DO_NOT_MODIFY
                                   | BCM_FCOE_INTF_VFT_ADD_REPLACE);
        *vft_action = FCOE_VFT_ACTION_DELETE;
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_fcoe_vsan_id_sel_check(bcm_fcoe_intf_vsan_id_source_t src,
                                int *fabric_id_sel)
{
    if (src == bcmFcoeIntfVsanSet) {
        *fabric_id_sel = FCOE_FABRIC_ID_SEL_SET;
    } else if (src == bcmFcoeIntfVsanInternal) {
        *fabric_id_sel = FCOE_FABRIC_ID_SEL_INTERNAL;
    } else if (src == bcmFcoeIntfVsanInnerVlan) {
        *fabric_id_sel = FCOE_FABRIC_ID_SEL_INNER_VLAN;
    } else if (src == bcmFcoeIntfVsanOuterVlan) {
        *fabric_id_sel = FCOE_FABRIC_ID_SEL_OUTER_VLAN;
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_fcoe_vsan_pri_sel_check(bcm_fcoe_intf_vsan_pri_source_t src,
                                 int *fabric_pri_sel)
{
    if (src == bcmFcoeIntfVsanPriSet) {
        *fabric_pri_sel = FCOE_FABRIC_PRI_SEL_SET;
    } else if (src == bcmFcoeIntfVsanPriInternal) {
        *fabric_pri_sel = FCOE_FABRIC_PRI_SEL_INTERNAL;
    } else if (src == bcmFcoeIntfVsanPriRemark) {
        *fabric_pri_sel = FCOE_FABRIC_PRI_SEL_REMARK;
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#endif

/* 
 * _bcm_td2_add_vft_fields_profile
 *    Add data in *cfg to EGR_VFT_FIELDS_PROFILE and store the index used into
 *    vft_fields_idx
 */
STATIC int
_bcm_td2_add_vft_fields_profile(int unit, bcm_fcoe_vsan_vft_t *cfg,
                                uint32 *vft_fields_idx)
{
    int     rv;
    soc_mem_t mem = EGR_VFT_FIELDS_PROFILEm;
    egr_vft_fields_profile_entry_t entry;
    void    *entries = (void *)&entry;

    sal_memset(&entry, 0, sizeof entry);

    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_R_CTLf, 
                        cfg->r_ctl);
    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_VERSIONf, 
                        cfg->version);
    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_TYPEf, 
                        cfg->type);
    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_HOP_COUNTf, 
                        cfg->hop_count);

    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_R1f, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_R2f, 0);
    soc_mem_field32_set(unit, mem, (uint32 *)&entry, FCOE_VFT_Ef, 0);

    rv = _bcm_egr_vft_fields_entry_add(unit, &entries, VFT_NUM_ENTRIES, 
                                       vft_fields_idx);
    return rv;
}

/* 
 * _bcm_td2_inc_vft_profile_refcnt
 *    Increment the reference count for the given vft fields index
 */
STATIC int
_bcm_td2_inc_vft_profile_refcnt(int unit, uint32 index)
{
    _bcm_td2_fcoe_bk_info[unit].profile_use_refcnt[index]++;
    return BCM_E_NONE;
}

/* 
 * _bcm_td2_dec_vft_profile_refcnt
 *    Decrement the reference count for the given vft fields index.  Delete the
 *    table entry if the counter reaches zero.
 */
STATIC int
_bcm_td2_dec_vft_profile_refcnt(int unit, uint32 index)
{
    _bcm_td2_fcoe_bk_info[unit].profile_use_refcnt[index]--;

    if (_bcm_td2_fcoe_bk_info[unit].profile_use_refcnt[index] == 0) {
        return _bcm_egr_vft_fields_entry_del(unit, index);
    }   

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3

/* 
 * _bcm_td2_vft_fields_profile_sanity
 *    Sanity check for the values in a bcm_fcoe_vsan_vft_t structure.  They're 
 *    all uint8s, so only check the 2 values that don't allow all 8 bits to be 
 *    used.
 */
STATIC int
_bcm_td2_vft_fields_profile_sanity(bcm_fcoe_vsan_vft_t *cfg)
{
    if ((cfg->version > 3) || (cfg->type > 15)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* 
 * _bcm_td2_set_intf_config
 *    Store values into the EGR_L3_INTF table
 */
STATIC int
_bcm_td2_set_intf_config(int unit, bcm_if_t intf, bcm_fcoe_intf_config_t *cfg, 
                         uint32 vft_fields_idx, int vft_action, 
                         int fabric_id_sel, int fabric_pri_sel)
{
    egr_l3_intf_entry_t intf_entry;
    uint32              *buf = (uint32 *)&intf_entry;
    int                 rv = BCM_E_NONE;
    soc_mem_t           mem = EGR_L3_INTFm;
    int                 old_fields_idx;
    int                 qos_map_hwidx;

    if (intf >= soc_mem_index_max(unit, mem)) {
        return BCM_E_PARAM;
    }

    if (fabric_pri_sel == FCOE_FABRIC_PRI_SEL_REMARK) {
        BCM_IF_ERROR_RETURN(bcm_td2_qos_get_egr_vsan_hw_idx(unit, cfg->qos_map_id,
                                                             &qos_map_hwidx));

    }

    sal_memset(buf, 0, sizeof intf_entry);

    soc_mem_lock(unit, mem);
    rv = BCM_XGS3_MEM_READ(unit, mem, intf, buf);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }

    old_fields_idx = soc_mem_field32_get(unit, mem, buf, 
                                        FCOE_VFT_FIELDS_PROFILE_INDEXf);
    if ((old_fields_idx != vft_fields_idx) && (old_fields_idx != 0)) {
        rv = _bcm_td2_dec_vft_profile_refcnt(unit, old_fields_idx);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
    }

    soc_mem_field32_set(unit, mem, buf, FCOE_VFT_ACTIONf, vft_action);
    soc_mem_field32_set(unit, mem, buf, FCOE_FABRIC_ID_SELf, fabric_id_sel);
    soc_mem_field32_set(unit, mem, buf, FCOE_FABRIC_PRI_SELf, fabric_pri_sel);
    soc_mem_field32_set(unit, mem, buf, FCOE_VSAN_IDf, cfg->vsan);
    if (fabric_pri_sel == FCOE_FABRIC_PRI_SEL_SET) {
        soc_mem_field32_set(unit, mem, buf, FCOE_VSAN_PRIf, cfg->vsan_pri);
    } else if (fabric_pri_sel == FCOE_FABRIC_PRI_SEL_REMARK) {
        soc_mem_field32_set(unit, mem, buf, FCOE_VSAN_PRI_MAPPING_PROFILEf,
                            qos_map_hwidx);
    }
    soc_mem_field32_set(unit, mem, buf, FCOE_VFT_FIELDS_PROFILE_INDEXf, 
                        vft_fields_idx);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, intf, buf);

    soc_mem_unlock(unit, mem);

    return rv;
}


/* Set parameters for FCOE L3 interface */
int
bcm_td2_fcoe_intf_config_set(int unit, uint32 flags, bcm_if_t intf, 
                             bcm_fcoe_intf_config_t *cfg)
{
    int rv             = BCM_E_NONE;
    int vft_action     = 0;
    int fabric_id_sel  = 0;
    int fabric_pri_sel = 0;
    uint32 vft_fields_idx = 0;

    /* Translate flags into hardware values and test validity */
    BCM_IF_ERROR_RETURN(_bcm_td2_vft_action_from_flags(cfg->flags,
                                                       &vft_action));
    BCM_IF_ERROR_RETURN(_bcm_td2_fcoe_vsan_pri_sel_check(cfg->vsan_pri_source,
                                                         &fabric_pri_sel));
    BCM_IF_ERROR_RETURN(_bcm_td2_fcoe_vsan_id_sel_check(cfg->vsan_source,
                                                        &fabric_id_sel));

    /* Check that the VFT profile is valid, and add it */
    BCM_IF_ERROR_RETURN(_bcm_td2_vft_fields_profile_sanity(&cfg->vft));
    BCM_IF_ERROR_RETURN(_bcm_td2_add_vft_fields_profile(unit, &cfg->vft, 
                                                        &vft_fields_idx));
    BCM_IF_ERROR_RETURN(_bcm_td2_inc_vft_profile_refcnt(unit, vft_fields_idx));

    /* Set interface properties */
    rv = _bcm_td2_set_intf_config(unit, intf, cfg, vft_fields_idx, vft_action,
                                  fabric_id_sel, fabric_pri_sel);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_dec_vft_profile_refcnt(unit, 
                                                            vft_fields_idx));
        return rv;
    }

    return rv;
}

#endif

/* Read data out of the EGR_VFT_FIELDS_PROFILE table and store it in a 
 * bcm_fcoe_vsan_vft_t structure */
STATIC int
_bcm_td2_get_fields_profile(int unit, int vft_fields_idx, 
                            bcm_fcoe_vsan_vft_t *vft)
{
    soc_mem_t mem = EGR_VFT_FIELDS_PROFILEm;
    egr_vft_fields_profile_entry_t entry;
    void    *entries = (void *)&entry;

    sal_memset(&entry, 0, sizeof entry);
    BCM_IF_ERROR_RETURN(_bcm_egr_vft_fields_entry_get(unit, vft_fields_idx, 
                                                      VFT_NUM_ENTRIES, 
                                                      &entries));

    vft->r_ctl = soc_mem_field32_get(unit, mem, &entry, FCOE_VFT_R_CTLf);
    vft->version = soc_mem_field32_get(unit, mem, &entry, FCOE_VFT_VERSIONf);
    vft->type = soc_mem_field32_get(unit, mem, &entry, FCOE_VFT_TYPEf);
    vft->hop_count = soc_mem_field32_get(unit, mem, &entry, FCOE_VFT_HOP_COUNTf);

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3

/* Get parameters for FCOE L3 interface */
int
bcm_td2_fcoe_intf_config_get(int unit, uint32 flags, bcm_if_t intf, 
                             bcm_fcoe_intf_config_t *cfg)
{
    egr_l3_intf_entry_t intf_entry;
    uint32              *buf = (uint32 *)&intf_entry;
    int                 rv = BCM_E_NONE;
    soc_mem_t           mem = EGR_L3_INTFm;
    int                 vft_fields_idx = 0;
    int                 qos_map_hwidx = 0;

    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, intf, buf));

    vft_fields_idx = soc_mem_field32_get(unit, mem, buf, 
                                        FCOE_VFT_FIELDS_PROFILE_INDEXf);

    switch (soc_mem_field32_get(unit, mem, buf, FCOE_VFT_ACTIONf)) {
        case FCOE_VFT_ACTION_DO_NOT_MODIFY: 
            cfg->flags |= BCM_FCOE_INTF_VFT_DO_NOT_MODIFY;
            break;

        case FCOE_VFT_ACTION_ADD_REPLACE:
            cfg->flags |= BCM_FCOE_INTF_VFT_ADD_REPLACE;
            break;

        case FCOE_VFT_ACTION_DELETE:
            cfg->flags |= BCM_FCOE_INTF_VFT_DELETE;
            break;

        default:
            return BCM_E_FAIL;
    }

    switch (soc_mem_field32_get(unit, mem, buf, FCOE_FABRIC_ID_SELf)) {
        case FCOE_FABRIC_ID_SEL_SET:
            cfg->vsan_source = bcmFcoeIntfVsanSet;
            break;

        case FCOE_FABRIC_ID_SEL_INTERNAL:
            cfg->vsan_source = bcmFcoeIntfVsanInternal;
            break;

        case FCOE_FABRIC_ID_SEL_INNER_VLAN:
            cfg->vsan_source = bcmFcoeIntfVsanInnerVlan;
            break;

        case FCOE_FABRIC_ID_SEL_OUTER_VLAN:
            cfg->vsan_source = bcmFcoeIntfVsanOuterVlan;
            break;

        default:
            return BCM_E_FAIL;
    }

    switch (soc_mem_field32_get(unit, mem, buf, FCOE_FABRIC_PRI_SELf)) {
        case FCOE_FABRIC_PRI_SEL_SET:
            cfg->vsan_pri_source = bcmFcoeIntfVsanPriSet;
            break;

        case FCOE_FABRIC_PRI_SEL_INTERNAL:
            cfg->vsan_pri_source = bcmFcoeIntfVsanPriInternal;
            break;

        case FCOE_FABRIC_PRI_SEL_REMARK:
            cfg->vsan_pri_source = bcmFcoeIntfVsanPriRemark;
            break;

        default:
            return BCM_E_FAIL;
    }

    cfg->vsan = soc_mem_field32_get(unit, mem, buf, FCOE_VSAN_IDf);
    vft_fields_idx = soc_mem_field32_get(unit, mem, buf, 
                                         FCOE_VFT_FIELDS_PROFILE_INDEXf);
    if (cfg->vsan_pri_source == bcmFcoeIntfVsanPriSet) {
        cfg->vsan_pri = soc_mem_field32_get(unit, mem, buf, FCOE_VSAN_PRIf);
    } else if (cfg->vsan_pri_source == bcmFcoeIntfVsanPriRemark) {
        qos_map_hwidx = soc_mem_field32_get(unit, mem, buf,
                                            FCOE_VSAN_PRI_MAPPING_PROFILEf);
        BCM_IF_ERROR_RETURN(bcm_td2_vsan_profile_to_qos_id(unit, qos_map_hwidx,
                                                           &cfg->qos_map_id));
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_get_fields_profile(unit, vft_fields_idx, 
                                                    &cfg->vft));

    return rv;
}

#endif /* INCLUDE_L3 */


/* construct a key for VLAN_XLATE table search */
STATIC int
_bcm_td2_fcoe_ing_vlan_translate_entry_assemble(
                                int                                  unit, 
                                void                                 *vent,
                                bcm_fcoe_vsan_translate_key_config_t *key)
{
    if (soc_feature(unit, soc_feature_fcoe)) {
        bcm_module_t    mod_out;
        bcm_port_t      port_out;
        bcm_trunk_t     trunk_out;
        int             tmp_id;
        int             key_type_value;
        int             key_type;
        int             use_glp = 1;
        int             mem = VLAN_XLATEm;

        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }

        switch (key->key_type) {
            case bcmFcoeVsanTranslateKeyDouble:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                break;

            case bcmFcoeVsanTranslateKeyOuter:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_OVID;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                break;

            case bcmFcoeVsanTranslateKeyInner:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_IVID;
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                break;

            case bcmFcoeVsanTranslateKeyOuterTag:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_OTAG;

                /* vlan id should also have PRI/CFI bits */
                soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                                            key->outer_vlan);
                break;

            case bcmFcoeVsanTranslateKeyInnerTag:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                /* vlan id should also have PRI/CFI bits */
                soc_mem_field32_set(unit, mem, vent, XLATE__ITAGf,
                                            key->inner_vlan);
                break;

            case bcmFcoeVsanTranslateKeyOuterPri:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;

                /* vlan id should have PRI/CFI bits */
                soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                                            key->outer_vlan);
                break;

            case bcmFcoeVsanTranslateKeyPortDouble:
                key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                break;

            case bcmFcoeVsanTranslateKeyPortOuter:
                key_type = VLXLT_HASH_KEY_TYPE_OVID;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                break;

            case bcmFcoeVsanTranslateKeyPortInner:
                key_type = VLXLT_HASH_KEY_TYPE_IVID;
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                break;

            case bcmFcoeVsanTranslateKeyPortOuterTag:
                key_type = VLXLT_HASH_KEY_TYPE_OTAG;
                /* vlan id should also have PRI/CFI bits */
                soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                                            key->outer_vlan);
                break;

            case bcmFcoeVsanTranslateKeyPortInnerTag:
                key_type = VLXLT_HASH_KEY_TYPE_ITAG;
                /* vlan id should also have PRI/CFI bits */
                soc_mem_field32_set(unit, mem, vent, XLATE__ITAGf,
                                            key->inner_vlan);
                break;

            case bcmFcoeVsanTranslateKeyPortOuterPri:
                key_type = VLXLT_HASH_KEY_TYPE_PRI_CFI;
                /* vlan id should have PRI/CFI bits */
                soc_mem_field32_set(unit, mem, vent, XLATE__OTAGf,
                                            key->outer_vlan);
                break;

            case bcmFcoeVsanTranslateKeyDoubleVsan:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__VSAN_IDf,
                                            key->vsan);
                break;

            case bcmFcoeVsanTranslateKeyInnerVsan:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_IVID_VSAN;
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__VSAN_IDf,
                                            key->vsan);
                break;

            case bcmFcoeVsanTranslateKeyOuterVsan:
                use_glp = 0;
                key_type = VLXLT_HASH_KEY_TYPE_OVID_VSAN;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__VSAN_IDf,
                                            key->vsan);
                break;

            case bcmFcoeVsanTranslateKeyPortDoubleVsan:
                key_type = VLXLT_HASH_KEY_TYPE_IVID_OVID_VSAN;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__VSAN_IDf,
                                            key->vsan);
                break;

            case bcmFcoeVsanTranslateKeyPortInnerVsan:
                key_type = VLXLT_HASH_KEY_TYPE_IVID_VSAN;
                soc_mem_field32_set(unit, mem, vent, XLATE__IVIDf,
                                            key->inner_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__VSAN_IDf,
                                            key->vsan);
                break;

            case bcmFcoeVsanTranslateKeyPortOuterVsan:
                key_type = VLXLT_HASH_KEY_TYPE_OVID_VSAN;
                soc_mem_field32_set(unit, mem, vent, XLATE__OVIDf,
                                            key->outer_vlan);
                soc_mem_field32_set(unit, mem, vent, XLATE__VSAN_IDf,
                                            key->vsan);
                break;

            default:
                return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_esw_vlan_xlate_key_type_value_get(unit, key_type,
                                                   &key_type_value));
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);


        if (use_glp) {
            /* SGLP - Use SGLP {trunk, ModId(7,0), Port(6,0)} as the Source
               Field */
            if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
            }

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, key->port, &mod_out, &port_out,
                                       &trunk_out, &tmp_id));
            if (BCM_GPORT_IS_TRUNK(key->port)) {
                soc_mem_field32_set(unit, mem, vent, XLATE__Tf, 1);
                soc_mem_field32_set(unit, mem, vent, XLATE__TGIDf,
                                            trunk_out);
            } else {
                soc_mem_field32_set(unit, mem, vent, XLATE__MODULE_IDf,
                                            mod_out);
                soc_mem_field32_set(unit, mem, vent, XLATE__PORT_NUMf,
                                            port_out);
            }
        } else {
            if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 0);
            }
            /* incoming port not used in lookup key, set field to all 1's */
            soc_mem_field32_set(unit, mem, vent, GLPf,
                                        SOC_VLAN_XLATE_GLP_WILDCARD(unit));
        }

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

uint32
_bcm_td2_fcoe_ing_vft_pri_action_encode(bcm_fcoe_vsan_action_t action)
{
    switch (action) {
        case bcmFcoeVsanActionNone:
            return _BCM_TD2_ING_VFT_PRI_ACTION_NONE;
        case bcmFcoeVsanActionInnerVlan:
            return _BCM_TD2_ING_VFT_PRI_ACTION_USE_INNER_PRI;
        case bcmFcoeVsanActionOuterVlan:
            return _BCM_TD2_ING_VFT_PRI_ACTION_USE_OUTER_PRI;
        case bcmFcoeVsanActionReplace:
            return _BCM_TD2_ING_VFT_PRI_ACTION_REPLACE;
        case bcmFcoeVsanActionDelete:
        case bcmFcoeVsanActionInternal:
        default:
            return _BCM_TD2_ING_VFT_PRI_ACTION_NONE;
    }
}

/*
 * add a new entry to vlan action profile table -- ingress
 */
STATIC int
_bcm_td2_ing_vlan_action_profile_entry_add(
                                       int                        unit,
                                       bcm_fcoe_vsan_action_set_t *action,
                                       uint32                     *index)
{   

    ing_vlan_tag_action_profile_entry_t ing_profile; 
    ing_vlan_tag_action_profile_2_entry_t ing_profile_2; 
    void *entry;
    soc_mem_t mem;
    int   ent_sz;

    if (SOC_MEM_IS_VALID(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {
        mem = ING_VLAN_TAG_ACTION_PROFILE_2m;
        ent_sz = sizeof(ing_vlan_tag_action_profile_2_entry_t);
        entry = &ing_profile_2;
    } else {
        mem = ING_VLAN_TAG_ACTION_PROFILEm;
        ent_sz = sizeof(ing_vlan_tag_action_profile_entry_t);
        entry = &ing_profile;
    }
 

    /* Note : on ingress, no VFT Tag manipulation will happen. Only Pri
     *        manipulation is allowed.
     *        So, mark all Tag actions to None (it is done by above sal_memset)
     */

    sal_memset(entry, 0, ent_sz);
    /* 3 new PRI actions */
    soc_mem_field32_set(unit, mem, entry,
                        DT_VT_VPRI_ACTIONf,
                        _bcm_td2_fcoe_ing_vft_pri_action_encode(
                                                      action->dt_vsan_pri));
    soc_mem_field32_set(unit, mem, entry,
                        SOT_VT_VPRI_ACTIONf,
                        _bcm_td2_fcoe_ing_vft_pri_action_encode(
                                                      action->ot_vsan_pri));
    soc_mem_field32_set(unit, mem, entry,
                        SIT_VT_VPRI_ACTIONf,
                        _bcm_td2_fcoe_ing_vft_pri_action_encode(
                                                      action->it_vsan_pri));

    soc_mem_field32_set(
        unit, mem, entry, DT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_outer));
    soc_mem_field32_set(
        unit, mem, entry, DT_POTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_outer_prio));
    soc_mem_field32_set(
        unit, mem, entry, DT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_inner));
    soc_mem_field32_set(
        unit, mem, entry, DT_PITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_inner_prio));
    soc_mem_field32_set(
        unit, mem, entry, SOT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ot_outer));
    soc_mem_field32_set(
        unit, mem, entry, SOT_POTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ot_outer_prio));
    soc_mem_field32_set(
        unit, mem, entry, SOT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ot_inner));
    soc_mem_field32_set(
        unit, mem, entry, SIT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.it_outer));
    soc_mem_field32_set(
        unit, mem, entry, SIT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.it_inner));
    soc_mem_field32_set(
        unit, mem, entry, SIT_PITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.it_inner_prio));
    soc_mem_field32_set(
        unit, mem, entry, UT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ut_outer));
    soc_mem_field32_set(
        unit, mem, entry, UT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ut_inner));

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_mem_field32_set(
            unit, mem, entry, DT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.dt_outer_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, DT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.dt_outer_cfi));
        soc_mem_field32_set(
            unit, mem, entry, DT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.dt_inner_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, DT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.dt_inner_cfi));
        soc_mem_field32_set(
            unit, mem, entry, SOT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ot_outer_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, SOT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ot_outer_cfi));
        soc_mem_field32_set(
            unit, mem, entry, SOT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ot_inner_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, SOT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ot_inner_cfi));
        soc_mem_field32_set(
            unit, mem, entry, SIT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.it_outer_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, SIT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.it_outer_cfi));
        soc_mem_field32_set(
            unit, mem, entry, SIT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.it_inner_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, SIT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.it_inner_cfi));
        soc_mem_field32_set(
            unit, mem, entry, UT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ut_outer_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, UT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ut_outer_cfi));
        soc_mem_field32_set(
            unit, mem, entry, UT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ut_inner_pkt_prio));
        soc_mem_field32_set(
            unit, mem, entry, UT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ut_inner_cfi));
    }

    return _bcm_trx_ing_vlan_action_profile_entry_no_mod_add(unit, entry,
                                                             index);
}

STATIC int 
_bcm_td2_fcoe_egr_vlan_translate_entry_assemble(
                                int                                  unit,
                                int                                  port_class,
                                void                                 *vent,
                                bcm_fcoe_vsan_translate_key_config_t *key)
{
    int dvp_id = BCM_GPORT_INVALID;
    soc_mem_t    mem = EGR_VLAN_XLATEm;
    soc_field_t  ef = ENTRY_TYPEf;

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
        sal_memset(vent, 0, sizeof(egr_vlan_xlate_1_double_entry_t));
        ef = DATA_TYPEf;
    } else {
        sal_memset(vent, 0, sizeof(egr_vlan_xlate_entry_t));
    }

    VLAN_CHK_ID(unit, key->outer_vlan);
    VLAN_CHK_ID(unit, key->inner_vlan);

     if (BCM_GPORT_IS_SUBPORT_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_subport)) {
            dvp_id = BCM_GPORT_SUBPORT_PORT_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_MIM_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_mim)) {
            dvp_id = BCM_GPORT_MIM_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
        
    } else if (BCM_GPORT_IS_WLAN_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_wlan)) {
            dvp_id = BCM_GPORT_WLAN_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_VLAN_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_vlan_vp)) {
            dvp_id = BCM_GPORT_VLAN_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_MPLS_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_mpls)) {
            dvp_id = BCM_GPORT_MPLS_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    }

    soc_mem_field32_set(unit, mem, vent, OVIDf, key->outer_vlan);
    soc_mem_field32_set(unit, mem, vent, IVIDf, key->inner_vlan);
    soc_mem_field32_set(unit, mem, vent, VSAN_IDf, key->vsan);


    if (BCM_GPORT_INVALID != dvp_id) {
        soc_mem_field32_set(unit, mem, vent, ef, 
                                    TD2_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP);
        soc_mem_field32_set(unit, mem, vent, DVPf, dvp_id);
    } else  {
        soc_mem_field32_set(unit, mem, vent, PORT_GROUP_IDf, port_class);
        soc_mem_field32_set(unit, mem, vent, ef, 
                                        TD2_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE);
    }

    return BCM_E_NONE;
}

uint32
_bcm_td2_fcoe_egr_vft_pri_action_encode(bcm_fcoe_vsan_action_t action)
{
    switch (action) {
        case bcmFcoeVsanActionNone:
            return _BCM_TD2_EGR_VFT_PRI_ACTION_NONE;
        case bcmFcoeVsanActionInnerVlan:
            return _BCM_TD2_EGR_VFT_PRI_ACTION_USE_INTERNAL_IPRI;
        case bcmFcoeVsanActionOuterVlan:
            return _BCM_TD2_EGR_VFT_PRI_ACTION_USE_INTERNAL_OPRI;
        case bcmFcoeVsanActionReplace:
            return _BCM_TD2_EGR_VFT_PRI_ACTION_REPLACE;
        case bcmFcoeVsanActionDelete:
        case bcmFcoeVsanActionInternal:
        default:
            return _BCM_TD2_EGR_VFT_PRI_ACTION_NONE;
    }
}

uint32
_bcm_td2_fcoe_egr_vft_tag_action_encode(bcm_fcoe_vsan_action_t action)
{
    switch (action) {
        case bcmFcoeVsanActionNone:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_NONE;
        case bcmFcoeVsanActionInnerVlan:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_USE_INTERNAL_IVID;
        case bcmFcoeVsanActionOuterVlan:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_USE_INTERNAL_OVID;
        case bcmFcoeVsanActionDelete:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_DELETE;
        case bcmFcoeVsanActionInternal:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_USE_INTERNAL_VSAN;
        case bcmFcoeVsanActionReplace:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_REPLACE;
        default:
            return _BCM_TD2_EGR_VFT_TAG_ACTION_NONE;
    }
}

/*
 * add a new entry to egress vlan action profile table
 */
int
_bcm_td2_fcoe_egr_vlan_action_profile_entry_add(
                                            int unit,
                                            bcm_fcoe_vsan_action_set_t *action,
                                            uint32 *index)
{   
    egr_vlan_tag_action_profile_entry_t entry;

    sal_memset(&entry, 0, sizeof(egr_vlan_tag_action_profile_entry_t));

    /* Note : 
     *        Above sal_memset() will set all tag/pri/cfi actions to
     *        none. We only need to set new FCOE actions.
     */
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_VT_VPRI_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->dt_vsan_pri));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_VT_VPRI_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ot_vsan_pri));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SIT_VT_VPRI_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->it_vsan_pri));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        UT_VT_VPRI_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ut_vsan_pri));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        DT_VT_VTAG_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->dt_vsan));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SOT_VT_VTAG_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ot_vsan));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        SIT_VT_VTAG_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->it_vsan));
    soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                        UT_VT_VTAG_ACTIONf,   
                        _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ut_vsan));

    /* actions for no VFT header */
    if (action->flags & BCM_FCOE_VSAN_ACTION_VFT_NONE) {
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            DT_NOVT_VPRI_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->dt_vsan_pri));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            SOT_NOVT_VPRI_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ot_vsan_pri));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            SIT_NOVT_VPRI_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->it_vsan_pri));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            UT_NOVT_VPRI_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ut_vsan_pri));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            DT_NOVT_VTAG_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->dt_vsan));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            SOT_NOVT_VTAG_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ot_vsan));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            SIT_NOVT_VTAG_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->it_vsan));
        soc_mem_field32_set(unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry,
                            UT_NOVT_VTAG_ACTIONf,   
                            _bcm_td2_fcoe_egr_vft_pri_action_encode(
                                                         action->ut_vsan));
    }

    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_outer));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_POTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_outer_prio));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_inner));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_PITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.dt_inner_prio));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ot_outer));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_POTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ot_outer_prio));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ot_inner));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.it_outer));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.it_inner));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_PITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.it_inner_prio));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, UT_OTAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ut_outer));
    soc_mem_field32_set(
        unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, UT_ITAG_ACTIONf,
        _BCM_TRX_TAG_ACTION_ENCODE(action->vlan_action.ut_inner));

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.dt_outer_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.dt_outer_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.dt_inner_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, DT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.dt_inner_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ot_outer_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ot_outer_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ot_inner_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SOT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ot_inner_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.it_outer_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.it_outer_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.it_inner_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, SIT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.it_inner_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, UT_OPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ut_outer_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, UT_OCFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ut_outer_cfi));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, UT_IPRI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(
            action->vlan_action.ut_inner_pkt_prio));
        soc_mem_field32_set(
            unit, EGR_VLAN_TAG_ACTION_PROFILEm, &entry, UT_ICFI_ACTIONf,
            _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->vlan_action.ut_inner_cfi));
    }

    return _bcm_trx_egr_vlan_action_profile_entry_no_mod_add(unit, &entry,
                                                             index);
}


/* Add VSAN translate action */
int 
bcm_td2_fcoe_vsan_translate_action_add (
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key, 
                                    bcm_fcoe_vsan_action_set_t *action)
{
    int                rv = BCM_E_NONE;
    int  mem = VLAN_XLATEm;
    int  vf  = VALIDf;
    int  ent_sz = sizeof(vlan_xlate_entry_t);
 
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        ent_sz = sizeof(vlan_xlate_1_double_entry_t);
    }

    if (action->flags & BCM_FCOE_VSAN_ACTION_INGRESS) {

        uint32             profile_idx; 
        int                search_rv;
        int                old_profile_idx = 0;
        int                index;
        void               *vent, *search_vent;
        vlan_xlate_entry_t vx_ent;
        vlan_xlate_entry_t search_vx_ent;

#ifdef BCM_TRIDENT3_SUPPORT
        vlan_xlate_1_double_entry_t vx1d_ent;
        vlan_xlate_1_double_entry_t search_vx1d_ent;

        if (SOC_IS_TRIDENT3X(unit)) {
            vent = &vx1d_ent;
            search_vent= &search_vx1d_ent;
            ent_sz = sizeof(vx1d_ent);
        } else
#endif
        {
            vent = &vx_ent;
            search_vent= &search_vx_ent;
            ent_sz = sizeof(vx_ent);
        }

        sal_memset(vent, 0, ent_sz);

        /* first, search VLAN_XLATE table using provided key */
        BCM_IF_ERROR_RETURN(
            _bcm_td2_fcoe_ing_vlan_translate_entry_assemble(unit, vent, 
                                                            key)); 
    
        sal_memcpy(search_vent, vent, ent_sz);
     
        search_rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                                   search_vent, vent, 0);
    
        if (search_rv == SOC_E_NONE) {   /* already exist */
            old_profile_idx = soc_mem_field32_get(unit, mem, vent, 
                                              XLATE__TAG_ACTION_PROFILE_PTRf);
        }
    
        soc_mem_field32_set(unit, mem, vent, NEW_IVIDf,
                            action->vlan_action.new_inner_vlan);
        soc_mem_field32_set(unit, mem, vent, NEW_OVIDf,
                            action->vlan_action.new_outer_vlan);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (action->vlan_action.priority >= BCM_PRIO_MIN &&
                action->vlan_action.priority <= BCM_PRIO_MAX) {
                soc_mem_field32_set(unit, mem, vent, NEW_OPRIf,
                                    action->vlan_action.priority);
            }
            soc_mem_field32_set(unit, mem, vent, NEW_OCFIf,
                                action->vlan_action.new_outer_cfi);
            soc_mem_field32_set(unit, mem, vent, NEW_IPRIf,
                                action->vlan_action.new_inner_pkt_prio);
            soc_mem_field32_set(unit, mem, vent, NEW_ICFIf,
                                action->vlan_action.new_inner_cfi);
        }

        /* update or fill in the vlan xlate entry*/
        BCM_IF_ERROR_RETURN(
            _bcm_td2_ing_vlan_action_profile_entry_add(unit, action, 
                                                       &profile_idx));

        /* update table index */
        soc_mem_field32_set(unit, mem, vent, XLATE__TAG_ACTION_PROFILE_PTRf,
                                    profile_idx);

        /* mark action is valid */
        if (SOC_MEM_FIELD_VALID(unit, mem, XLATE__VLAN_ACTION_VALIDf)) {
            soc_mem_field32_set(unit, mem, vent, 
                                        XLATE__VLAN_ACTION_VALIDf, 1);
        }

        /* VSAN ID & Pri. VSAN ID is not used for ingress VSAN translation...*/
        soc_mem_field32_set(unit, mem, vent, XLATE__FCOE_VSAN_IDf, 
                                    action->new_vsan);
        soc_mem_field32_set(unit, mem, vent, XLATE__FCOE_VSAN_PRIf, 
                                    action->new_vsan_pri);

        /* mark this entry(VLAN_XLATE) is valid */
        if (mem == VLAN_XLATEm) {
            soc_mem_field32_set(unit, mem, vent, vf, 1);
        } else {
            soc_mem_field32_set(unit, mem, vent, vf, 3);
            soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
        }

        /* NOTE : action->vft (remaining field of VFT header) is not used
         *  on ingress side.
         */

        /* write entry into table */
        if (search_rv == SOC_E_NONE) {
            /* overrides existing one */
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vent);
        } else { /* case search_rv == SOC_E_NOT_FOUND */
           rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent);
        }
    
        if (BCM_FAILURE(rv)) {
            /* Delete the allocated vlan translate profile entry */
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                                             XLATE__TAG_ACTION_PROFILE_PTRf);
            (void) _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        } else {
            /* delete the old profile entry */
            if (search_rv == SOC_E_NONE) {
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                               old_profile_idx);
            }
        }

        if (rv != BCM_E_NONE) {
            return rv;
        }
    }


    if (action->flags & BCM_FCOE_VSAN_ACTION_EGRESS) {
        uint32                 profile_idx;
        uint32                 old_profile_idx = 0;
        uint32                 vent[SOC_MAX_MEM_FIELD_WORDS];
        uint32                 vent_old[SOC_MAX_MEM_FIELD_WORDS];
        uint32                 *vent_ptr;
        int                    index;
        uint32                 vft_profile_index = 0;
        uint32                 old_vft_profile_index = 0;
        uint32                 port_class;
        int                    search_rv, vf = VALIDf;
        soc_mem_t              egr_mem = EGR_VLAN_XLATEm;
 
        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
            egr_mem = EGR_VLAN_XLATE_1_DOUBLEm;
            vf = BASE_VALID_0f;
        }
    
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_class_get(unit, key->port, 
                               bcmPortClassVlanTranslateEgress, &port_class));

        vent_ptr = vent;
        BCM_IF_ERROR_RETURN(
            _bcm_td2_fcoe_egr_vlan_translate_entry_assemble(unit, port_class,
                                                            vent_ptr, key));

        soc_mem_lock(unit, egr_mem);
        /* check if the entry with same key already exists */
        search_rv = soc_mem_search(unit, egr_mem, MEM_BLOCK_ANY, &index,
                                   vent_ptr, vent_old, 0);

        if (search_rv == SOC_E_NONE) {
            /* alredy exists, update on the existing one, so the flex
            * counter related fields in TR3/Katana can be preserved.
            */
            vent_ptr = vent_old;
            old_profile_idx = soc_mem_field32_get(unit, egr_mem, vent_ptr,
                                               TAG_ACTION_PROFILE_PTRf); 
            old_vft_profile_index = 
                soc_mem_field32_get(unit, egr_mem,  vent_ptr,
                                               FCOE_VFT_FIELDS_PROFILE_INDEXf); 
        } else if (search_rv != SOC_E_NOT_FOUND) {
            soc_mem_unlock(unit, egr_mem);
            return rv;  /* error condition */
        } else {
            /* doesn't exist, update the new entry */
            vent_ptr = vent; 
        }
 
        /* new VSAN & PRI */
        soc_mem_field32_set(unit, egr_mem, vent_ptr, FCOE_VSAN_IDf, 
                                        action->new_vsan);
        soc_mem_field32_set(unit, egr_mem, vent_ptr, FCOE_VSAN_PRIf, 
                                        action->new_vsan_pri);


        soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_IVIDf,
                            action->vlan_action.new_inner_vlan);
        soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_OVIDf,
                            action->vlan_action.new_outer_vlan);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (action->vlan_action.priority >= BCM_PRIO_MIN &&
                action->vlan_action.priority <= BCM_PRIO_MAX) {
                soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_OPRIf,
                                    action->vlan_action.priority);
            }
            soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_OCFIf,
                                action->vlan_action.new_outer_cfi);
            soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_IPRIf,
                                action->vlan_action.new_inner_pkt_prio);
            soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_ICFIf,
                                action->vlan_action.new_inner_cfi);
        }
        /* action profile index */
        rv = _bcm_td2_fcoe_egr_vlan_action_profile_entry_add(unit, action,
                                                            &profile_idx);
        if (rv != SOC_E_NONE) {
            soc_mem_unlock(unit, egr_mem);
            return rv;
        }
        soc_mem_field32_set(unit, egr_mem, vent_ptr, TAG_ACTION_PROFILE_PTRf,
                                        profile_idx);

        /* new FCOE_VFT_FIELDS_PROFILE_INDEX, creaate one if not exist  */
        rv = _bcm_td2_add_vft_fields_profile(unit, &action->vft, 
                                            &vft_profile_index);
        if (rv != SOC_E_NONE) {
            soc_mem_unlock(unit, egr_mem);
            return rv;
        }

        rv = _bcm_td2_inc_vft_profile_refcnt(unit, vft_profile_index);
        if (rv != SOC_E_NONE) {
            soc_mem_unlock(unit, egr_mem);
            return rv;
        }

        soc_mem_field32_set(unit, egr_mem, vent_ptr, NEW_OTAG_VPTAG_SELf, 0);
        if (vf == VALIDf) {
            soc_mem_field32_set(unit, egr_mem, vent_ptr, vf, 1);
        } else {
            soc_mem_field32_set(unit, egr_mem, vent_ptr, vf, 3);
            soc_mem_field32_set(unit, egr_mem, vent_ptr, BASE_VALID_1f, 7);
        }

        if (search_rv == SOC_E_NONE) {
            /* write back the existing one */
            rv = soc_mem_write(unit, egr_mem,
                               MEM_BLOCK_ALL, index, vent_ptr);
            if (rv == SOC_E_NONE) {
                /* write was succeful */
                rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                                                              old_profile_idx);
                if (old_vft_profile_index != vft_profile_index) {
                    rv += _bcm_td2_dec_vft_profile_refcnt(unit, 
                                                         old_vft_profile_index);
                }
            }
        } else {
            /* insert new one */
            rv = soc_mem_insert(unit, egr_mem, MEM_BLOCK_ALL, vent_ptr);
        }

        soc_mem_unlock(unit, egr_mem);

        return rv;
    }

    return BCM_E_NONE;
}

/* Delete VSAN translate action */
int 
bcm_td2_fcoe_vsan_translate_action_delete (
                                    int unit, 
                                    bcm_fcoe_vsan_translate_key_config_t *key) 
{
    int                    rv = BCM_E_NONE; 
    int                    search_rv;
    int                    index;
    uint32                 port_class;
    vlan_xlate_entry_t     ing_vent;
    uint32                 ing_profile_idx;
    uint32                 egr_profile_idx;
    uint32                 egr_vft_profile_idx;
    soc_mem_t              mem = VLAN_XLATEm;
    soc_mem_t              egr_mem = EGR_VLAN_XLATEm;
    int                    ent_sz, vf = VALIDf;
    vlan_xlate_1_double_entry_t     ing_1d_vent;
    void                   *in_vent;
    uint32                 egr_vent[SOC_MAX_MEM_FIELD_WORDS];

    in_vent = &ing_vent;
    ent_sz = sizeof(ing_vent);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        egr_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        vf  = BASE_VALID_0f;
        in_vent = &ing_1d_vent;
        ent_sz = sizeof(ing_1d_vent);
    }

    /* ingress */
    sal_memset(in_vent, 0, ent_sz);
    BCM_IF_ERROR_RETURN(
        _bcm_td2_fcoe_ing_vlan_translate_entry_assemble(unit, in_vent, 
                                                        key)); 
    
    search_rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                               in_vent, in_vent, 0);
    
    if (search_rv == SOC_E_NONE) {   /* exist */
        ing_profile_idx = soc_mem_field32_get(unit, mem, in_vent, 
                                              XLATE__TAG_ACTION_PROFILE_PTRf);
        /* mark entry as invalid */
        if (mem == VLAN_XLATEm) {
            soc_mem_field32_set(unit, mem, in_vent, vf, 0);
        } else {
            soc_mem_field32_set(unit, mem, in_vent, vf, 0);
            soc_mem_field32_set(unit, mem, in_vent, BASE_VALID_1f, 0);
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, in_vent);

        if (rv == BCM_E_NONE) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, 
                                                           ing_profile_idx);
        }
    } else if (search_rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;    /* if INGRESS flag was not used when adding action,
                             * this is an expected return 
                             */
    }

    if (rv != BCM_E_NONE) {
        return rv;
    }

    /* egress */

    BCM_IF_ERROR_RETURN(
        bcm_esw_port_class_get(unit, key->port, bcmPortClassVlanTranslateEgress,
                           &port_class));

    sal_memset(egr_vent, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    BCM_IF_ERROR_RETURN(
            _bcm_td2_fcoe_egr_vlan_translate_entry_assemble(unit, port_class,
                                                            egr_vent, key));

    soc_mem_lock(unit, egr_mem);

    /* check if the entry with same key already exists */
    rv = soc_mem_search(unit, egr_mem, MEM_BLOCK_ANY, &index,
                        egr_vent, egr_vent, 0);

    if (rv == SOC_E_NONE) {
        /* exists */
        egr_profile_idx = soc_mem_field32_get(unit, egr_mem, egr_vent,
                                                     TAG_ACTION_PROFILE_PTRf); 
        egr_vft_profile_idx =  soc_mem_field32_get(unit, egr_mem, egr_vent,
                                               FCOE_VFT_FIELDS_PROFILE_INDEXf); 

        /* mark entry as invalid */
        if (egr_mem == EGR_VLAN_XLATEm) {
            soc_mem_field32_set(unit, egr_mem, egr_vent, vf, 0);
        } else {
            soc_mem_field32_set(unit, egr_mem, egr_vent, vf, 0);
            soc_mem_field32_set(unit, egr_mem, egr_vent, BASE_VALID_1f, 0);
        }
        rv = soc_mem_write(unit, egr_mem, MEM_BLOCK_ALL, index, egr_vent);

        if (rv == BCM_E_NONE) {

            /* Delete vlan action profile entry */
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, 
                                                               egr_profile_idx);
            /* delete vft profile entry */
            rv += _bcm_td2_dec_vft_profile_refcnt(unit, egr_vft_profile_idx);
        }
    } else if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;    /* if EGRESS flag was not used when adding action,
                             * this is an expected return 
                             */
    }
    soc_mem_unlock(unit, egr_mem);
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void 
_bcm_td2_fcoe_sw_dump(int unit)
{
    uint32 vsan_id;
    int valid = 0;
    int count = 0;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information FCOE - Unit %d\n"), unit));

    LOG_CLI((BSL_META_U(unit,
                        "VSAN ID Creation State : \n")));
    for (vsan_id = 0; vsan_id <= FCOE_MAX_VSAN_ID; vsan_id++) {
        valid = _bcm_td2_vsan_id_creation_state_get(unit, vsan_id);
        if (valid) {
            count++;
            LOG_CLI((BSL_META_U(unit, "%4d "), vsan_id));
            if (count == 16) {
                count = 0;
                LOG_CLI((BSL_META_U(unit, "\n")));
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* traverse zone entries and call user callback function */
int 
bcm_td2_fcoe_zone_traverse(int unit,
                           uint32 flags,
                           bcm_fcoe_zone_traverse_cb trav_fn,
                           void *user_data)
{
    int           ix;
    int           key_type;
    int           chnk_end;
    int           chunksize;
    int           chunk;
    int           chunk_max;
    int            mem_idx_max;
    L2_ENTRY_T    *l2e;
    L2_ENTRY_T    *l2ep;
    soc_mem_t     mem;
    uint32        action;
    uint32        copy_to_cpu;
    int           rv = BCM_E_NONE;
    bcm_fcoe_zone_entry_t zone;

    /* note : flags parameter is not used at the moment */

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    mem = L2Xm;

    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");

    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;

        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }

        chnk_end = (chunk_max - chunk);

        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                if (!soc_mem_field32_get(unit, mem, l2ep, BASE_VALIDf)) {
                    continue;
                }
            } else
#endif
            {
                if (!soc_mem_field32_get(unit, mem, l2ep, VALIDf)) {
                    continue;
                }
            }

            /* Match the Key Type to FCOE */
            key_type = soc_mem_field32_get(unit, mem, l2ep, KEY_TYPEf);

            if (key_type != TD2_L2_HASH_KEY_TYPE_FCOE_ZONE) {
                continue;
            }

            /* found zone entry */
           sal_memset(&zone, 0, sizeof(bcm_fcoe_zone_entry_t));
           zone.vsan_id = soc_mem_field32_get(unit, mem, l2ep, 
                                              FCOE_ZONE__VSAN_IDf); 
           zone.d_id = soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__D_IDf);
           zone.s_id = soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__S_IDf);
           zone.class_id = (int)soc_mem_field32_get(unit, mem, l2ep,
                                                    FCOE_ZONE__CLASS_IDf);
           action = soc_mem_field32_get(unit, mem, l2ep, FCOE_ZONE__ACTIONf);
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               copy_to_cpu = soc_mem_field32_get(unit, mem, l2ep,
                                             FCOE_ZONE__CPUf);
           } else 
#endif /* BCM_TRIDENT3_SUPPORT */
           {
               copy_to_cpu = soc_mem_field32_get(unit, mem, l2ep,
                                                 FCOE_ZONE__COPY_TO_CPUf);
           }

           if (action == 1) {
               zone.action = bcmFcoeZoneActionAllow;
           } else {  /* action is 0 (deny) */
               zone.action = bcmFcoeZoneActionDeny;
               if (copy_to_cpu == 1) {
                   zone.action = bcmFcoeZoneActionCopyToCpu;
               }
           }

           /* call user callback function with a zone entry */
           rv = (trav_fn)(unit, &zone, user_data);

           if (rv < 0) {
               break;
           }
        }

        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);

    return rv;
}

/* traverse vsan entries and call user callback function */
int 
bcm_td2_fcoe_vsan_traverse(int unit,
                           uint32 flags,
                           bcm_fcoe_vsan_traverse_cb trav_fn,
                           void *user_data)
{
    ing_vsan_entry_t vsan_entry;
    soc_mem_t        mem = ING_VSANm;
    int              rv = BCM_E_NONE;
    int              index;
    bcm_fcoe_vsan_t  vsan;
    int              flag = 0;
    int              fcmap_index = 0;
    bcm_fcoe_fcmap_t fcmap;

    /* note : flags parameter is not used at the moment */

    soc_mem_lock(unit, mem);
    for (index = 0; index <= FCOE_MAX_VSAN_ID; index++) {

        /* check if vsan id is existing */
        if (shr_idxres_list_elem_state(vsan_id_list[unit], index) !=
                BCM_E_EXISTS) {
            continue;
        }

        /* read table entry */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &vsan_entry);
        if (rv != BCM_E_NONE) {
            break;
        }

        sal_memset(&vsan, 0, sizeof(bcm_fcoe_vsan_t));

        /* get FC MAP */
        fcmap_index = soc_mem_field32_get(unit,mem, (uint32 *)&vsan_entry,
                                          FCOE_FC_MAP_INDEXf);

        rv = _bcm_fcoe_fc_profile_map_value_get (unit, fcmap_index, &fcmap);
        if (rv != BCM_E_NONE) {
            break;
        }
        vsan.fcmap = fcmap;

        flag = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry,
                                   ZONE_CHECK_NORMALIZEDf);
        if (flag) {
            vsan.config_flags |= BCM_FCOE_VSAN_CONFIG_NORMALIZED_ZONE_CHECK;
        }

        flag = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry,
                                   FCOE_ROUTE_ENABLEf);
        if (flag) {
            vsan.config_flags |= BCM_FCOE_VSAN_CONFIG_FCOE_ROUTE_ENABLE;
        }

        vsan.l3_intf_id = soc_mem_field32_get(unit, mem, (uint32 *)&vsan_entry,
                                              L3_IIFf);

        /* call user callback function with a vsan entry. index is vsan_id */
        rv = (trav_fn)(unit, index, &vsan, user_data);

        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);
    return rv;
}

#ifdef INCLUDE_L3
#ifdef BCM_TRIDENT3_SUPPORT
STATIC int
_bcm_td3_fcoe_correct_key_type_for_flags(int unit, uint32    flags, 
                                         soc_mem_t mem, 
                                         int       key_type,
                                         int       data_type)
{
    int rv = BCM_E_NONE;

    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_FCOE_HOST) || 
            (data_type == (int)TD3_L3_HASH_DATA_TYPE_FCOE_HOST_EXT)) {
            if ((flags & BCM_FCOE_HOST_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN) || 
                   (data_type == (int)TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN_EXT)) {
            if ((flags & BCM_FCOE_DOMAIN_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP) ||
                   (data_type == (int)TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP_EXT)) {
            if ((flags & BCM_FCOE_SOURCE_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    } else if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
        if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_FCOE_HOST) || 
            (data_type == (int)TD3_L3_HASH_DATA_TYPE_FCOE_HOST)) {
            if ((flags & BCM_FCOE_HOST_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_FCOE_DOMAIN) ||
                   (data_type == (int)TD3_L3_HASH_DATA_TYPE_FCOE_DOMAIN)) {
            if ((flags & BCM_FCOE_DOMAIN_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if ((key_type == (int)TD3_L3_HASH_KEY_TYPE_FCOE_SOURCE_MAP) ||
                   (data_type == (int)TD3_L3_HASH_DATA_TYPE_FCOE_SOURCE_MAP)) {
            if ((flags & BCM_FCOE_SOURCE_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}
#endif /* BCM_TRIDENT3_SUPPORT */
STATIC int
_bcm_td2_fcoe_correct_key_type_for_flags(int unit, uint32    flags, 
                                         soc_mem_t mem, 
                                         int       key_type)
{
    int rv = BCM_E_NONE;

    if (mem == _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit)) {
        if (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_HOST_EXT) {
            if ((flags & BCM_FCOE_HOST_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN_EXT) {
            if ((flags & BCM_FCOE_DOMAIN_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP_EXT) {
            if ((flags & BCM_FCOE_SOURCE_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    } else if (mem == _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit)) {
        if (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_HOST) {
            if ((flags & BCM_FCOE_HOST_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_DOMAIN) {
            if ((flags & BCM_FCOE_DOMAIN_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else if (key_type == TD2_L3_HASH_KEY_TYPE_FCOE_SRC_MAP) {
            if ((flags & BCM_FCOE_SOURCE_ROUTE) == 0) {
                rv = BCM_E_NOT_FOUND;
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

STATIC int
_bcm_fcoe_route_tbl_traverse_normal(int unit, 
                                    uint32 flags,
                                    bcm_fcoe_route_traverse_cb trav_fn,
                                    void *user_data)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry;

    soc_mem_t        mem = _bcm_fcoe_l3_entry_ipv4_unicast_mem_get(unit);
    uint32           *buf = (uint32 *)&l3_ext_entry;
    int              idx;
    int              idx_max;
    int              key_type_0;
    int              rv = BCM_E_NONE;
    bcm_fcoe_route_t route;

    idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (idx = 0; idx < idx_max; idx++) {
        sal_memset(buf, 0, sizeof l3_ext_entry);

        rv = BCM_XGS3_MEM_READ(unit, mem, idx, buf);
        if (BCM_FAILURE(rv)) {
            break;
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            int key_type_1;
            if (!soc_mem_field32_get(unit, mem, buf, BASE_VALIDf)) {
                continue;
            }
            key_type_0 = soc_mem_field32_get(unit, mem, buf,
                                             KEY_TYPEf);
            key_type_1 = soc_mem_field32_get(unit, mem, buf,
                                             DATA_TYPEf);

            if (!_bcm_td3_key_type_is_fcoe(unit, mem, key_type_0, key_type_1)) {
                continue;
            }

            if (_bcm_td3_fcoe_correct_key_type_for_flags(unit, flags, mem, key_type_0, key_type_1) != 
                 BCM_E_NONE) {
                continue;
            }
        } else
#endif
        {
            if (!soc_mem_field32_get(unit, mem, buf, VALIDf)) {
               continue;
            }

            key_type_0 = soc_mem_field32_get(unit, mem, buf,
                                             KEY_TYPEf);

            if (!_bcm_td2_key_type_is_fcoe(unit, mem, key_type_0)) {
                continue;
            }

            if (_bcm_td2_fcoe_correct_key_type_for_flags(unit, flags, mem, key_type_0) != 
                 BCM_E_NONE) {
                continue;
            }
        }

        /* build route entry */
        sal_memset(&route, 0, sizeof(bcm_fcoe_route_t));

        _bcm_fcoe_read_mem_into_route(unit, mem, buf, &route);

        /* call user callback function */
        rv = (trav_fn)(unit, &route, user_data);
        if (rv != BCM_E_NONE) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);

    return rv;
}


STATIC int
_bcm_fcoe_route_tbl_traverse_ext(int unit, 
                                 uint32 flags,
                                 bcm_fcoe_route_traverse_cb trav_fn,
                                 void *user_data)
{
    l3_entry_ipv4_multicast_entry_t l3_ext_entry;

    soc_mem_t        mem = _bcm_fcoe_l3_entry_ipv4_multicast_mem_get(unit);
    uint32           *buf = (uint32 *)&l3_ext_entry;
    int              idx;
    int              idx_max;
    int              key_type_0, key_type_0f = KEY_TYPE_0f;
    int              key_type_1, key_type_1f = KEY_TYPE_1f;
    int              valid_0f = VALID_0f;
    int              valid_1f = VALID_1f;
    int              rv = BCM_E_NONE;
    bcm_fcoe_route_t route;

    idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        valid_0f = BASE_VALID_0f;
        valid_1f = BASE_VALID_1f;
        key_type_0f = KEY_TYPEf;
        key_type_1f = DATA_TYPEf;
    } 
#endif
    for (idx = 0; idx < idx_max; idx++) {
        sal_memset(buf, 0, sizeof l3_ext_entry);

        rv = BCM_XGS3_MEM_READ(unit, mem, idx, buf);
        if (BCM_FAILURE(rv)) {
            break;
        }

        if (!soc_mem_field32_get(unit, mem, buf, valid_0f) ||
            !soc_mem_field32_get(unit, mem, buf, valid_1f)) {
            continue;
        }

        key_type_0 = soc_mem_field32_get(unit, mem, buf, key_type_0f);
        key_type_1 = soc_mem_field32_get(unit, mem, buf, key_type_1f);

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (!_bcm_td3_key_type_is_fcoe(unit, mem, key_type_0, key_type_1)) {
                continue;
            }
            if (_bcm_td3_fcoe_correct_key_type_for_flags(unit, flags, mem, key_type_0, key_type_1) !=
                 BCM_E_NONE) {
                continue;
            }
        } else
#endif   
        {
            if (!_bcm_td2_key_type_is_fcoe(unit, mem, key_type_0) ||
                (key_type_0 != key_type_1)) {
                continue;
            }

            if (_bcm_td2_fcoe_correct_key_type_for_flags(unit, flags, mem, key_type_0) !=
                 BCM_E_NONE) {
                continue;
            }
        }

        /* build route entry */
        sal_memset(&route, 0, sizeof(bcm_fcoe_route_t));

        _bcm_fcoe_read_mem_into_route(unit, mem, buf, &route);

        /* call user callback function */
        rv = (trav_fn)(unit, &route, user_data);
        if (rv != BCM_E_NONE) {
            break;
        }
    }

    soc_mem_unlock(unit, mem);

    return rv;
}

STATIC int
_bcm_fcoe_route_tbl_traverse_prefix(int unit, 
                                    bcm_fcoe_route_traverse_cb trav_fn,
                                    void *user_data)
{
    uint32    entry[SOC_MAX_MEM_WORDS];
    uint32           *buf = entry;
    soc_mem_t        mem = L3_DEFIPm;
    int              idx;
    int              idx_max;
    int              rv = BCM_E_NONE;
    bcm_fcoe_route_t route;
    int              nh_idx = 0;
    _bcm_defip_cfg_t lpm_cfg;

    idx_max = soc_mem_index_max(unit, mem);

    soc_mem_lock(unit, mem);

    for (idx = 0; idx < idx_max; idx++) {
        sal_memset(buf, 0, sizeof entry);

        rv = BCM_XGS3_MEM_READ(unit, mem, idx, buf);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* first entry */
        if (soc_mem_field32_get(unit, mem, buf, VALID0f) &&
            soc_mem_field32_get(unit, mem, buf, ENTRY_TYPE0f)) { 

            /* build route entry */
            sal_memset(&route, 0, sizeof(bcm_fcoe_route_t));
            sal_memset(&lpm_cfg , 0, sizeof(_bcm_defip_cfg_t));

            BCM_IF_ERROR_RETURN(
                _bcm_fb_lpm_ent_parse(unit, &lpm_cfg, &nh_idx,
                                      buf, NULL));

            rv = _bcm_fcoe_defip_to_route(unit, &lpm_cfg, &route);

            if (rv != BCM_E_NONE) {
                break;
            }

            if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
                if (route.flags & BCM_FCOE_MULTIPATH) {
                    route.intf = nh_idx + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                } else {
                    route.intf = nh_idx + BCM_XGS3_EGRESS_IDX_MIN(unit);
                }
            }

            /* call user callback function */
            rv = (trav_fn)(unit, &route, user_data);

            if (rv != BCM_E_NONE) {
                break;
            }
        }

        /* second entry */
        if (soc_mem_field32_get(unit, mem, buf, VALID1f) &&
            soc_mem_field32_get(unit, mem, buf, ENTRY_TYPE1f)) {

            /* build route entry */
            sal_memset(&route, 0, sizeof(bcm_fcoe_route_t));
            sal_memset(&lpm_cfg , 0, sizeof(_bcm_defip_cfg_t));

            rv = soc_fb_lpm_ip4entry1_to_0(unit, buf, buf, 0);

            if (rv != BCM_E_NONE) {
                break;
            }

            BCM_IF_ERROR_RETURN(
            _bcm_fb_lpm_ent_parse(unit, &lpm_cfg, &nh_idx,
                                  buf, NULL));

            rv = _bcm_fcoe_defip_to_route(unit, &lpm_cfg, &route);

            if (rv != BCM_E_NONE) {
                break;
            }

            if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
                if (route.flags & BCM_FCOE_MULTIPATH) {
                    route.intf = nh_idx + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                } else {
                    route.intf = nh_idx + BCM_XGS3_EGRESS_IDX_MIN(unit);
                }
            }

            /* call user callback function */
            rv = (trav_fn)(unit, &route, user_data);

            if (rv != BCM_E_NONE) {
                break;
            }
        }
    }

    soc_mem_unlock(unit, mem);

    return rv;
}

#endif  /* INCLUDE_L3 */

/* traverse routing entries and call user callback function */
int 
bcm_td2_fcoe_route_traverse(int unit,
                            uint32 flags,
                            bcm_fcoe_route_traverse_cb trav_fn,
                            void *user_data)
{
#ifdef INCLUDE_L3
    int rv = BCM_E_NONE;

    if (BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
        /* search L3_ENTRY_IPV4_UNICAST table */
        rv = _bcm_fcoe_route_tbl_traverse_normal(unit, flags, trav_fn, 
                                                 user_data);
        if (rv == BCM_E_NONE) {
            /* search L3_ENTRY_IPV4_MULTICAST table */
            rv = _bcm_fcoe_route_tbl_traverse_ext(unit, flags, trav_fn, 
                                                  user_data);
        }
    } else {
        /* search L3_ENTRY_IPV4_MULTICAST table */
        rv = _bcm_fcoe_route_tbl_traverse_ext(unit, flags, trav_fn, user_data);
    }

    if (rv != BCM_E_NONE) {
        return rv;
    }
    
    if (flags & BCM_FCOE_DOMAIN_ROUTE) {
        /* L3_DEFIP table for prefix route */
        rv = _bcm_fcoe_route_tbl_traverse_prefix(unit, trav_fn, user_data);
    }

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif
}


#endif /* BCM_TRIDENT2_SUPORT */
