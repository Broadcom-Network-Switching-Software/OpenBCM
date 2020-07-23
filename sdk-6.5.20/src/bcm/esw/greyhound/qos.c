/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        qos.c
 * Purpose:     Greyhound QoS functions
 */
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/qos.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/greyhound.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm/qos.h>

#ifdef BCM_GREYHOUND_SUPPORT

#define _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP     16
#define _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP 64

#define _BCM_QOS_MAX_PKT_PRIORITIES             8
#define _BCM_QOS_MAX_INT_PRIORITIES             16
#define _BCM_QOS_MAX_PKT_DES             2

#define _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS                   \
            (soc_mem_index_count(unit, ING_ETAG_PCP_MAPPINGm)/ \
             _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP)
#define _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS                   \
            (soc_mem_index_count(unit, EGR_ETAG_PCP_MAPPINGm)/ \
             _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP)

#define _BCM_QOS_MAX_L2_VLAN_ETAG_MAP_IDS    ((_BCM_QOS_MAX_L2_VLAN_ETAG_MAPS)\
                                          * _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP)
#define _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAP_IDS    ((_BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS)\
											  * _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP)

#define _BCM_QOS_MAP_TYPE_MASK           0x7ff
#define _BCM_QOS_MAP_SHIFT                  11

#define _BCM_QOS_NO_MAP                     -1

typedef struct _bcm_gh_qos_bookkeeping_s {
    SHR_BITDCL *ing_l2_vlan_etag_map;
    uint32     *ing_l2_vlan_etag_map_hwidx;   
    SHR_BITDCL *egr_l2_vlan_etag_map;
    uint32     *egr_l2_vlan_etag_map_hwidx;
    sal_mutex_t qos_mutex;
} _bcm_gh_qos_bookkeeping_t;

static _bcm_gh_qos_bookkeeping_t _bcm_gh_qos_bk_info[BCM_MAX_NUM_UNITS];
static int _bcm_gh_qos_initialized[BCM_MAX_NUM_UNITS] = { 0 };

#define GH_QOS_INFO(_unit_) (&_bcm_gh_qos_bk_info[_unit_])

#define _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(GH_QOS_INFO(_u_)->ing_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(GH_QOS_INFO((_u_))->ing_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(GH_QOS_INFO((_u_))->ing_l2_vlan_etag_map, (_identifier_))

#define _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(GH_QOS_INFO(_u_)->egr_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(GH_QOS_INFO((_u_))->egr_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(GH_QOS_INFO((_u_))->egr_l2_vlan_etag_map, (_identifier_))

#define QOS_LOCK(unit) \
        sal_mutex_take(GH_QOS_INFO(unit)->qos_mutex, sal_mutex_FOREVER);
#define QOS_UNLOCK(unit) \
        sal_mutex_give(GH_QOS_INFO(unit)->qos_mutex);

#define QOS_INIT(unit)                                    \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!_bcm_gh_qos_initialized[unit]) {            \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/*
 * Function: 
 *   _bcm_gh_qos_free_resources
 * Purpose:
 *   Free QOS bookkeeping resources associated with a given unit.  This
 *   happens either during an error initializing or during a detach operation.
 * Parameters:
 *   unit - (IN) Unit to free resources of
 * Returns:
 *   BCM_E_XXX
 */
STATIC void
_bcm_gh_qos_free_resources(int unit)
{
    _bcm_gh_qos_bookkeeping_t *qos_info = GH_QOS_INFO(unit);

    if (!qos_info) {
        /*
         * Coverity
         * This is defencive statement.
         */
        /* coverity[dead_error_line] */
        return;
    }	
    if (qos_info->ing_l2_vlan_etag_map) {
        sal_free(qos_info->ing_l2_vlan_etag_map);
        qos_info->ing_l2_vlan_etag_map = NULL;
    }

    if (qos_info->ing_l2_vlan_etag_map_hwidx) {
        sal_free(qos_info->ing_l2_vlan_etag_map_hwidx);
        qos_info->ing_l2_vlan_etag_map_hwidx = NULL;
    }

    if (qos_info->egr_l2_vlan_etag_map) {
        sal_free(qos_info->egr_l2_vlan_etag_map);
        qos_info->egr_l2_vlan_etag_map = NULL;
    }

    if (qos_info->egr_l2_vlan_etag_map_hwidx) {
        sal_free(qos_info->egr_l2_vlan_etag_map_hwidx);
        qos_info->egr_l2_vlan_etag_map_hwidx = NULL;
    }

    if (qos_info->qos_mutex) {
        sal_mutex_destroy(qos_info->qos_mutex);
        qos_info->qos_mutex = NULL;
    }
}

/*
 * Function: 
 *   bcm_gh_qos_detach
 * Purpose:
 *   Detach the QOS module.  Called when the module should de-initialize.
 * Parameters:
 *   unit - (IN) Unit being detached.
 * Returns:
 *   BCM_E_XXX
 */
int 
bcm_gh_qos_detach(int unit)
{
    if (_bcm_gh_qos_initialized[unit]) {
        _bcm_gh_qos_initialized[unit] = 0;
        _bcm_gh_qos_free_resources(unit);
    }

    return BCM_E_NONE;
}
		
/*
 * Function: 
 *   bcm_gh_qos_init
 * Purpose:
 *   Initialize the GH QOS module.  Allocate bookkeeping information.
 * Parameters:
 *   unit - (IN) Unit being initialized
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_gh_qos_init(int unit)
{
    _bcm_gh_qos_bookkeeping_t *qos_info = GH_QOS_INFO(unit);
    int l2_vlan_tag_map_size  = _BCM_QOS_MAX_L2_VLAN_ETAG_MAP_IDS;
    int egr_l2_vlan_tag_map_size  = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAP_IDS;

    if (_bcm_gh_qos_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_gh_qos_detach(unit));
    }

    memset(qos_info, 0, sizeof *qos_info);

    /* ingress l2 vlan etag map bitmap */
    qos_info->ing_l2_vlan_etag_map = 
                    sal_alloc(SHR_BITALLOCSIZE(l2_vlan_tag_map_size), 
                    "ing_l2_vlan_etag_map");
    if (qos_info->ing_l2_vlan_etag_map == NULL) {
        _bcm_gh_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->ing_l2_vlan_etag_map, 0, 
                SHR_BITALLOCSIZE(l2_vlan_tag_map_size));

    /* ingress l2 vlan etag map hardware indexes */
    qos_info->ing_l2_vlan_etag_map_hwidx = 
                sal_alloc(sizeof(uint32) * l2_vlan_tag_map_size, 
                "ing_l2_vlan_etag_map_hwidx");
    if (qos_info->ing_l2_vlan_etag_map_hwidx == NULL) {
        _bcm_gh_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->ing_l2_vlan_etag_map_hwidx, 0, 
                sizeof(uint32) * l2_vlan_tag_map_size);

    /* egress l2 vlan etag priority map bitmap */
    qos_info->egr_l2_vlan_etag_map = sal_alloc(SHR_BITALLOCSIZE(egr_l2_vlan_tag_map_size), 
                                "egr_l2_vlan_etag_map");
    if (qos_info->egr_l2_vlan_etag_map == NULL) {
        _bcm_gh_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->egr_l2_vlan_etag_map, 0, 
                SHR_BITALLOCSIZE(egr_l2_vlan_tag_map_size));

    /* egress l2 vlan etag map hardware indexes */
    qos_info->egr_l2_vlan_etag_map_hwidx = 
                sal_alloc(sizeof(uint32) * egr_l2_vlan_tag_map_size, 
                "egr_l2_vlan_etag_map_hwidx");
    if (qos_info->egr_l2_vlan_etag_map_hwidx == NULL) {
        _bcm_gh_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->egr_l2_vlan_etag_map_hwidx, 0, 
                sizeof(uint32) * egr_l2_vlan_tag_map_size);

    /* accessor mutex */
    qos_info->qos_mutex = sal_mutex_create("vft qos_mutex");
    if (qos_info->qos_mutex == NULL) {
        _bcm_gh_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }

    _bcm_gh_qos_initialized[unit] = 1;
    return BCM_E_NONE;
}

/*
 * Function: 
 *   _bcm_gh_qos_flags_sanity_check
 * Purpose:
 *   Check that the combination of flags specified is valid.
 * Parameters:
 *   flags  - (IN) BCM_QOS_MAP_* flags (eg. ingress/egress/with_id/replace)
  * Returns:
 *   BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_flags_sanity_check(uint32 flags)
{
    int rv = BCM_E_NONE;

    /* must be ingress or egress, not both */
    if ((flags & BCM_QOS_MAP_INGRESS) && (flags & BCM_QOS_MAP_EGRESS)) {
        rv = BCM_E_PARAM;
    }

    /* must be ingress or egress, not neither */
    if (!(flags & BCM_QOS_MAP_INGRESS) && !(flags & BCM_QOS_MAP_EGRESS)) {
        rv = BCM_E_PARAM;
    }

    /* support BCM_QOS_MAP_L2_VLAN_ETAG */
    if (!(flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
        rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function: 
 *     _bcm_gh_qos_assign_creation_table_vars
 * Purpose:
 *     Assign the various pointers to tables and table sizes and bitmaps used
 *     for the bcm_gh_qos_map_create so that it can create the different types
 *     of maps with common logic.  
 * 
 *     Assumes "flags" has already been validated.
 * Parameters:
 *     unit       - (IN)  Unit to be operated on
 *     flags      - (IN)  BCM_QOS_MAP_* flags (eg. ingress/egress/with_id/etc)
 *     is_ingress - (OUT) 1 for ingress, 0 for egress map
 *     bitmap     - (OUT) pointer to priority map's bitmap
 *     hwmap      - (OUT) pointer to priority map's hwidx mapping
 *     map_type   - (OUT) _BCM_QOS_MAP_TYPE_*
 *     map_size   - (OUT) size of the selected map
 *     max_maps   - (OUT) maximum number of maps available for the selected type
 * Returns:
 *     None
 */
STATIC void
_bcm_gh_qos_assign_creation_table_vars(int unit, int flags, int *is_ingress, 
                                        SHR_BITDCL **bitmap,
                                        uint32 **hwmap, int *map_type, 
                                        int *map_size, int *max_maps)
{
    if (flags & BCM_QOS_MAP_INGRESS) {
        *is_ingress = 1;
        if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
            *bitmap     = GH_QOS_INFO(unit)->ing_l2_vlan_etag_map;
            *hwmap      = GH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx;
            *map_type   = _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP;
            *map_size   = _BCM_QOS_MAX_L2_VLAN_ETAG_MAP_IDS;
            *max_maps   = _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS;
        }        
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        *is_ingress = 0;

        if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
            *bitmap     = GH_QOS_INFO(unit)->egr_l2_vlan_etag_map;
            *hwmap      = GH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx;
            *map_type   = _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP;
            *map_size   = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAP_IDS;
            *max_maps   = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS;
        }
    } 
}

/*
 * Function: 
 *     _bcm_gh_qos_count_used_maps
 * Purpose:
 *     Count the number maps currently used in hardware.  Since IDs in app-space
 *     can point to the same hardware index, count how many hardware indexes are
 *     used, ignoring duplicates.
 * Parameters:
 *     unit   - (IN) Unit to be operated on
 *     bitmap - (IN) Bitmap of used map IDs
 *     hwmap  - (IN) Array that maps userspace IDs to hardware IDs
 * Returns:
 *     integer containing the number of used hardware indexes
 */
STATIC int
_bcm_gh_qos_count_used_maps(int unit, SHR_BITDCL *bitmap, uint32 *hwmap, 
                             int map_size, int max_maps) 
{
    uint8 *hw_used = sal_alloc(map_size, "gh_qos_used_maps");
    int total_used = 0;
    int i = 0;
    
    if (hw_used == NULL) {
        return max_maps;
    }

    memset(hw_used, 0, map_size);

    for (i = 0; i < map_size; i++) {
        if (SHR_BITGET(bitmap, i) && hw_used[hwmap[i]] == 0) {
            hw_used[hwmap[i]] = 1;
            total_used++;
        }
    }

    sal_free(hw_used);
    return total_used;
}

/*
 * Function: 
 *     _bcm_gh_qos_map_create_with_id
 * Purpose:
 *     Allocate a new QOS map ID where the app has specified the ID.  So just
 *     check that the requested ID is available, and mark it as used.
 * Parameters:
 *     unit       - (IN) Unit to be operated on
 *     flags      - (IN) Used only to test if BCM_QOS_MAP_REPLACE is set
 *     bitmap     - (IN/OUT) Bitmap containing the set of used IDs
 *     map_id     - (IN) The ID requested by the application
 *     is_ingress - (IN) 1=ingress map, 0=egress map
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_map_create_with_id(int unit, int flags, SHR_BITDCL *bitmap, 
                                    int *map_id, int is_ingress, int max_ids)
{
    int id = 0, map_type = 0, rv = BCM_E_NONE;

    id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
    map_type = *map_id >> _BCM_QOS_MAP_SHIFT;

    if (id < 0 || id >= max_ids) {
        return BCM_E_BADID;
    }

    if ((map_type == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP && is_ingress)) {

        if (SHR_BITGET(bitmap, id) == 1) {
            if (!(flags & BCM_QOS_MAP_REPLACE)) {
                rv = BCM_E_EXISTS;
            }
        }

        if (BCM_SUCCESS(rv)) {
            SHR_BITSET(bitmap, id);
        }
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function: 
 *     _bcm_gh_qos_map_id_alloc
 * Purpose:
 *     Allocate a new QOS map ID.  The bitmap given contains bits set for 
 *     existing IDs, so find the first empty one, set it, and return it.
 * Parameters:
 *     unit   - (IN) Unit to be operated on
 *     bitmap - (IN/OUT)
 *     id     - (OUT)
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_map_id_alloc(int unit, SHR_BITDCL *bitmap, int *id, 
                          int map_size)
{
    int i;

    for (i = 0; i < map_size; i++) {
        if (!SHR_BITGET(bitmap, i)) {
            SHR_BITSET(bitmap, i);
            *id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}


/*
 * Function: 
 *     bcm_gh_qos_map_create
 * Purpose:
 *     Create a new QOS map.
 * Parameters:
 *     unit   - (IN) Unit to be operated on
 *     flags  - (IN) BCM_QOS_MAP_* flags (eg. ingress/egress/with_id/replace)
 *     map_id - (IN/OUT) If BCM_QOS_MAP_WITH_ID is set, then this is the ID
 *                       being requested.  Otherwise, it's assigned as the ID
 *                       allocated.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_gh_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int        is_ingress = 0;
    int        map_type = 0;
    SHR_BITDCL *bitmap = NULL;
    uint32     index = 0;
    int        rv    = BCM_E_NONE;
    int        id    = 0;
    void       *entries[1];
    uint32     *hwmap = NULL;
    int        map_size = 0;
    int        max_maps = 0;

    QOS_INIT(unit);
    QOS_LOCK(unit);

    rv = _bcm_gh_qos_flags_sanity_check(flags);

    if (BCM_SUCCESS(rv)) {
        _bcm_gh_qos_assign_creation_table_vars(unit, flags, &is_ingress, 
                                                &bitmap, &hwmap, &map_type, 
                                                &map_size, &max_maps);
    }

    if (BCM_SUCCESS(rv)) {
        if (_bcm_gh_qos_count_used_maps(unit, bitmap, hwmap, map_size, 
                                         max_maps) >= max_maps) {
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        if (flags & BCM_QOS_MAP_WITH_ID) {
            rv = _bcm_gh_qos_map_create_with_id(unit, flags, bitmap, map_id, 
                                                 is_ingress, map_size);
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
        } else {
            rv = _bcm_gh_qos_map_id_alloc(unit, bitmap, &id, map_size);
            if (BCM_SUCCESS(rv)) {
                /*_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_SET(unit, id);*/
                *map_id = id | (map_type << _BCM_QOS_MAP_SHIFT);
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        if ((flags & BCM_QOS_MAP_INGRESS) 
                  && (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
            ing_etag_pcp_mapping_entry_t 
                                table[_BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP];
            memset(&table, 0, sizeof table);
            entries[0] = &table;

            rv = _bcm_ing_l2_vlan_etag_map_entry_add(unit, entries, 
                                           _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP, 
                                           &index);

            if (BCM_SUCCESS(rv)) {
                GH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] = index;
            }
        } else if ((flags & BCM_QOS_MAP_EGRESS)
                    && (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
            egr_etag_pcp_mapping_entry_t 
                                table[_BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP];
            memset(&table, 0, sizeof table);
            entries[0] = &table;

            rv = _bcm_egr_l2_vlan_etag_map_entry_add(unit, entries, 
                                _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP, 
                                &index);

            if (BCM_SUCCESS(rv)) {
                GH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] = index;
            }
        } 
    }

    QOS_UNLOCK(unit);

    return rv;
}

/*
 * Function: 
 *     _bcm_gh_qos_map_destroy
 * Purpose:
 *     Destroy a map ID by removing it from hardware.
 * Parameters:
 *     unit  - (IN) The unit being operated on
 *     table - (IN) Indicates either ingress or egress
 *     id    - (IN) The map ID being destroyed
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_map_destroy(int unit, int table, int id)
{
    int rv = BCM_E_NONE;
    _bcm_gh_qos_bookkeeping_t *qos_info = GH_QOS_INFO(unit);

    if (table == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) {
        rv = _bcm_ing_l2_vlan_etag_map_entry_del(unit, 
                                    qos_info->ing_l2_vlan_etag_map_hwidx[id]);
    } else if (table == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) {
        rv = _bcm_egr_l2_vlan_etag_map_entry_del(unit, 
                                    qos_info->egr_l2_vlan_etag_map_hwidx[id]);
    }

    return rv;
}

/*
 * Function: 
 *     bcm_gh_qos_map_destroy
 * Purpose:
 *     Destroy a QOS map ID by removing it from hardware and deleting the
 *     associated software state info.
 * Parameters:
 *     unit   - (IN) Unit being operated on
 *     map_id - (IN) QOS map ID to be destroyed
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_gh_qos_map_destroy(int unit, int map_id)
{
    int id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    int rv = BCM_E_UNAVAIL;

    QOS_INIT(unit);
    QOS_LOCK(unit);

    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
        case _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP:
            if (!_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id)) {
                rv = BCM_E_NOT_FOUND;
            } else {
                rv = _bcm_gh_qos_map_destroy(unit, 
                        _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP, id);
        
                if (BCM_SUCCESS(rv)) {
                    _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_CLR(unit, id);
                }
            }
            break;
        case _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP:
            if (!_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id)) {
                rv = BCM_E_NOT_FOUND;
            } else {
                rv = _bcm_gh_qos_map_destroy(unit, 
                    _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP, id);
		
                if (BCM_SUCCESS(rv)) {
                    _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_CLR(unit, id);
                }
            }
            break;
        default:
            rv = BCM_E_PARAM;
    }

    QOS_UNLOCK(unit);

    return rv;
}

/*
 * Function: 
 *     _bcm_gh_qos_map_add_sanity_check
 * Purpose:
 *     Check that all the paramters given to the bcm_qos_map_add() function make
 *     sense and are internally consistent.
 * Parameters:
 *     unit   - (IN) The unit being operated on
 *     flags  - (IN) BCM_QOS_MAP_* flags sent in by the app
 *     map    - (IN) The map structure sent in by the app
 *     map_id - (IN) The map ID sent in by the app
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_map_add_sanity_check(int unit, uint32 flags, bcm_qos_map_t *map, 
                                  int map_id)
{
    int map_type = map_id >> _BCM_QOS_MAP_SHIFT;
    int id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    int rv;

    rv = _bcm_gh_qos_flags_sanity_check(flags);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if ((flags & BCM_QOS_MAP_INGRESS) 
        && (map_type != _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP)) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_QOS_MAP_EGRESS) 
        && (map_type != _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP)) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_QOS_MAP_INGRESS) 
        && (map_type == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) 
        && (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id) == 0)) {
        return BCM_E_BADID;
    }

    if ((flags & BCM_QOS_MAP_EGRESS) 
        && (map_type == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP)
        && (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id) == 0)) {
        return BCM_E_BADID;
    }

    if (!(flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
        if (map->int_pri < 0) {
            return BCM_E_PARAM;
        }
    }

    if (map->pkt_pri >= _BCM_QOS_MAX_PKT_PRIORITIES) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
        if ((map->etag_pcp >= _BCM_QOS_MAX_PKT_PRIORITIES)
            || (map->etag_de >= _BCM_QOS_MAX_PKT_DES)
            || (map->pkt_cfi >= _BCM_QOS_MAX_PKT_DES)) {
            return BCM_E_PARAM;
        }
    } 

    if ((flags & BCM_QOS_MAP_L2_VLAN_ETAG)
        && (flags & BCM_QOS_MAP_EGRESS)) {
        if (map->color != bcmColorGreen && map->color != bcmColorYellow
            && map->color != bcmColorRed) {
            return BCM_E_PARAM;
        }
        if (map->int_pri >= _BCM_QOS_MAX_INT_PRIORITIES) {
            return BCM_E_PARAM;
        }
    } 

    return BCM_E_NONE;
}

/*
 * Function: 
 *     _bcm_gh_ing_qos_map_update_index
 * Purpose:
 *     Update an existing profile (or a newly created one) in the hardware's
 *     table with a new mapping.  
 *   
 *     The structure is: (1) get existing profile for this map from hardware, 
 *     (2) update the profile in RAM, (3) delete the old profile from hardware, 
 *     and (4) push the updated profile in RAM to hardware.
 * Parameters:
 *     unit   - (IN) The unit being operated on
 *     map    - (IN) Map structure sent in by app to be applied to hardware
 *     map_id - (IN) Map ID sent in by app to be applied to hardware
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_ing_qos_map_update_index(int unit, bcm_qos_map_t *map, int map_id)
{
    void    *entries[1];
    int     rv = BCM_E_NONE;
    uint32  index;
    int     id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    if (QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        ing_etag_pcp_mapping_entry_t table[_BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP];        
        index = GH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id];

        entries[0] = &table;
        rv = _bcm_ing_l2_vlan_etag_map_entry_get(unit, index, 
                                            _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP, 
                                            entries);

        if (BCM_SUCCESS(rv)) {
            ing_etag_pcp_mapping_entry_t *entry = 
                                    &table[(map->pkt_pri << 1) | map->pkt_cfi];

            soc_mem_field32_set(unit, ING_ETAG_PCP_MAPPINGm, 
                                entry, PCPf, map->etag_pcp);
            soc_mem_field32_set(unit, ING_ETAG_PCP_MAPPINGm, 
                                entry, DEf, map->etag_de);
            
            rv = _bcm_ing_l2_vlan_etag_map_entry_del(unit, (int)index);
        }

        if (BCM_SUCCESS(rv)) {
            rv = _bcm_ing_l2_vlan_etag_map_entry_add(unit, entries, 
                                            _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP,
                                            &index);
            GH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] = index;  
        }
    }
    return rv;
}

/*
 * Function: 
 *     _bcm_gh_egr_qos_map_update_index
 * Purpose:
 *     Update an existing profile (or a newly created one) in the hardware's
 *     table with a new mapping.  
 *   
 *     The structure is: (1) get existing profile for this map from hardware, 
 *     (2) update the profile in RAM, (3) delete the old profile from hardware, 
 *     and (4) push the updated profile in RAM to hardware.
 * Parameters:
 *     unit   - (IN) The unit being operated on
 *     map    - (IN) Map structure sent in by app to be applied to hardware
 *     map_id - (IN) Map ID sent in by app to be applied to hardware
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_egr_qos_map_update_index(int unit, bcm_qos_map_t *map, int map_id)
{
    void    *entries[1];
    int     rv = BCM_E_NONE;
    int     cng;
    uint32  index;
    int     id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    if (QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        egr_etag_pcp_mapping_entry_t table[_BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP];
        index = GH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id];

        entries[0] = &table;
        rv = _bcm_egr_l2_vlan_etag_map_entry_get(unit, index, 
                                            _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP,
                                            entries);
        if (BCM_SUCCESS(rv)) {
            egr_etag_pcp_mapping_entry_t *entry; 
            cng = _BCM_COLOR_ENCODING(unit, map->color);
            entry = &table[(map->int_pri << 2) | cng];

            soc_mem_field32_set(unit, EGR_ETAG_PCP_MAPPINGm, 
                                entry, PCPf, map->etag_pcp);
            soc_mem_field32_set(unit, EGR_ETAG_PCP_MAPPINGm, 
                                entry, DEf, map->etag_de);

            rv = _bcm_egr_l2_vlan_etag_map_entry_del(unit, (int)index);
        }

        if (BCM_SUCCESS(rv)) {
            rv = _bcm_egr_l2_vlan_etag_map_entry_add(unit, entries, 
                                            _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP,
                                            &index);
            GH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] = index;  
        }
    }

    return rv;
}

/*
 * Function: 
 *     bcm_gh_qos_map_add
 * Purpose:
 *     Add a new QOS mapping to hardware.
 * Parameters:
 *     unit   - (IN) The unit being operated on
 *     flags  - (IN) BCM_QOS_MAP_* flags
 *     map    - (IN) Structure containing the priorities to be mapped
 *     map_id - (IN) The ID of the map to add the new information to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_gh_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;

    QOS_INIT(unit);
    QOS_LOCK(unit);

    rv = _bcm_gh_qos_map_add_sanity_check(unit, flags, map, map_id);
    if (BCM_SUCCESS(rv)) {
        if ((flags & BCM_QOS_MAP_INGRESS) 
                    && (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
            rv = _bcm_gh_ing_qos_map_update_index(unit, map, map_id);
        } else {
            rv = _bcm_gh_egr_qos_map_update_index(unit, map, map_id);
        }
    }

    QOS_UNLOCK(unit);

    return rv;
}

/* bcm_gh_qos_map_multi_get */
int
bcm_gh_qos_map_multi_get(int unit, uint32 flags,
                          int map_id, int array_size, 
                          bcm_qos_map_t *array, int *array_count)
{
    int             rv = BCM_E_NONE;
    int             num_entries, idx, id, hw_id, alloc_size, entry_size, count;
    uint8           *dma_buf;
    void            *entry;
    soc_mem_t       mem;
    bcm_qos_map_t   *map;
    int cng = 0;

    /* ignore with_id & replace flags */
    flags &= (~(BCM_QOS_MAP_WITH_ID | BCM_QOS_MAP_REPLACE));
    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    mem = INVALIDm;
    hw_id = 0;
    num_entries = 0;
    entry_size = 0;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP:
        if ((flags != (BCM_QOS_MAP_L2_VLAN_ETAG | BCM_QOS_MAP_INGRESS)) ||
            (!_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP;
        entry_size = sizeof(ing_etag_pcp_mapping_entry_t);
        hw_id = GH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id];
        mem = ING_ETAG_PCP_MAPPINGm;
        break;
    case _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP:
        if ((flags != (BCM_QOS_MAP_L2_VLAN_ETAG | BCM_QOS_MAP_EGRESS)) ||
            (!_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP;
        entry_size = sizeof(egr_etag_pcp_mapping_entry_t);
        hw_id = GH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id];
        mem = EGR_ETAG_PCP_MAPPINGm;
        break;
    default:
        rv = BCM_E_PARAM;
    }
    QOS_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (array_size == 0) { /* querying the size of map-chunk */
        *array_count = num_entries;
        return BCM_E_NONE;
    }
    if (!array || !array_count) {
        return BCM_E_PARAM;
    }

    /* Allocate memory for DMA & regular */
    alloc_size = num_entries * entry_size;
    dma_buf = soc_cm_salloc(unit, alloc_size, "GH qos multi get DMA buf");
    if (!dma_buf) {
        return BCM_E_MEMORY;
    }
    sal_memset(dma_buf, 0, alloc_size);

    /* Read the profile chunk */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, hw_id, 
                            (hw_id + num_entries - 1), dma_buf);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, dma_buf);
        return rv;
    }

    count = 0;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->pkt_pri = ((idx & 0xe) >> 1);
            map->pkt_cfi = (idx & 0x1);
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                                 dma_buf, idx);
            map->etag_pcp = soc_mem_field32_get(unit, mem, entry, PCPf);
            map->etag_de = soc_mem_field32_get(unit, mem, entry, DEf);
            count++;
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->int_pri = ((idx & 0x3c) >> 2);
            cng = (idx & 0x3);
            map->color = _BCM_COLOR_DECODING(unit, cng);
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                            dma_buf, idx);
            map->etag_pcp = soc_mem_field32_get(unit, mem, entry, PCPf);
            map->etag_de = soc_mem_field32_get(unit, mem, entry, DEf);
            count++;
        }
        break;
    default:
        rv = BCM_E_INTERNAL; /* should not hit this */
    }
    QOS_UNLOCK(unit);
    soc_cm_sfree(unit, dma_buf);

    BCM_IF_ERROR_RETURN(rv);

    *array_count = count;
    return BCM_E_NONE;
}

/*
 * Function: 
 *     bcm_gh_qos_map_delete
 * Purpose:
 *     Delete a mapping (for example, the map from 7 to 3).  This is equivalent
 *     to changing it to a map from 7 to 0, so this function maps the call into
 *     bcm_td2_qos_map_add().
 * Parameters:
 *     unit   - (IN) The unit being operated on
 *     flags  - (IN) BCM_QOS_MAP_* flags
 *     map    - (IN) Structure containing the priorities to be mapped
 *     map_id - (IN) The ID of the map to add the new information to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_gh_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t clr_map;
    
    QOS_INIT(unit);

    sal_memcpy(&clr_map, map, sizeof clr_map);

    rv = _bcm_gh_qos_flags_sanity_check(flags);
    if (BCM_SUCCESS(rv)) {
        if (flags & BCM_QOS_MAP_INGRESS) {
            if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
                clr_map.etag_pcp = 0;
                clr_map.etag_de = 0;
            }
        } else {
            if(flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
                clr_map.etag_pcp = 0;
                clr_map.etag_de = 0;
            } else {
            clr_map.pkt_pri = 0;
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcm_gh_qos_map_add(unit, flags, &clr_map, map_id);
    }

    return rv;
}

/*
 * Function: 
 *     _bcm_gh_qos_apply_ing_map_to_port
 * Purpose:
 *     Apply a fully created ingress map to a hardware port.  This is the final
 *     step in the process, where the mapping becomes operational.
 * Parameters:
 *     unit       - (IN) Unit being operated on
 *     ing_map         - (IN) ID of the map to be applied
 *     local_port - (IN) Local port number to apply map to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_apply_ing_map_to_port(int unit, int ing_map, bcm_port_t local_port)
{
    int rv = BCM_E_PARAM;
    int id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
    int profile_idx = (GH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] / 
                          _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP);
    
    soc_mem_lock(unit, PORT_TABm);
    
    if ((ing_map >> _BCM_QOS_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) {
        rv = _bcm_tr2_port_tab_set(unit, local_port, ETAG_PCP_DE_MAPPING_PTRf,
                                 profile_idx);
    }

    soc_mem_unlock(unit, PORT_TABm);

    return rv;
}

/*
 * Function: 
 *     _bcm_gh_qos_apply_egr_map_to_port
 * Purpose:
 *     Apply a fully created ingress map to a hardware port.  This is the final
 *     step in the process, where the mapping becomes operational.
 * Parameters:
 *     unit       - (IN) Unit being operated on
 *     ing_map         - (IN) ID of the map to be applied
 *     local_port - (IN) Local port number to apply map to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_gh_qos_apply_egr_map_to_port(int unit, int egr_map, bcm_port_t local_port)
{
    int rv = BCM_E_PARAM;
    int id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
    int profile_idx = (GH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] / 
                          _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP);
    soc_mem_lock(unit, PORT_TABm);
    
    if ((egr_map >> _BCM_QOS_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) {
        rv = _bcm_tr2_port_tab_set(unit, local_port, ETAG_DOT1P_MAPPING_PTRf,
                                 profile_idx);
    }

    soc_mem_unlock(unit, PORT_TABm);

    return rv;
}

/*
 * Function: 
 *     bcm_gh_qos_port_map_set
 * Purpose:
 *     Apply a fully created ingress and/or egress map to a given port.  If
 *     either map is given as -1 (_BCM_QOS_NO_MAP), then the parameter should be
 *     ignored.
 * Parameters:
 *     unit    - (IN) The unit to be operated on
 *     port    - (IN) The port number for the map to be applied to
 *     ing_map - (IN) The ID of an ingress map to be applied
 *     egr_map - (IN) The ID of an egress map to be applied
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_gh_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    
    QOS_INIT(unit);
    QOS_LOCK(unit);

    if (!BCM_GPORT_IS_SET(port)) {
        rv = BCM_E_PARAM;
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_port_local_get(unit, port, &local_port);
    }

    if (BCM_SUCCESS(rv) && !SOC_PORT_VALID(unit, local_port)) {
        rv = BCM_E_PARAM;
    }

    if (BCM_SUCCESS(rv) && ing_map != _BCM_QOS_NO_MAP) {
        int id = ing_map & _BCM_QOS_MAP_TYPE_MASK;

        if ((ing_map >> _BCM_QOS_MAP_SHIFT) != 
            _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP){
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv) 
            && (((ing_map >> _BCM_QOS_MAP_SHIFT) == 
            _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) 
            && (!_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id)))) {
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv)) {
            rv = _bcm_gh_qos_apply_ing_map_to_port(unit, ing_map, local_port);
        }
    }

    if (BCM_SUCCESS(rv) && egr_map != _BCM_QOS_NO_MAP) {
        int id = egr_map & _BCM_QOS_MAP_TYPE_MASK;

        if ((egr_map >> _BCM_QOS_MAP_SHIFT) != 
            _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP){
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv) 
            && (((egr_map >> _BCM_QOS_MAP_SHIFT) == 
            _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) 
            && (!_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id)))) {
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv)) {
            rv = _bcm_gh_qos_apply_egr_map_to_port(unit, egr_map, local_port);
        }
    }

    QOS_UNLOCK(unit);

    return rv;
}

#endif /* BCM_GREYHOUND_SUPPORT */

