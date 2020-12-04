/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        qos.c
 * Purpose:     Tomahawk QoS functions
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
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm/qos.h>

#ifdef BCM_TOMAHAWK_SUPPORT

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

typedef struct _bcm_th_qos_bookkeeping_s {
    SHR_BITDCL *ing_l2_vlan_etag_map;
    uint32     *ing_l2_vlan_etag_map_hwidx;   
    SHR_BITDCL *egr_l2_vlan_etag_map;
    uint32     *egr_l2_vlan_etag_map_hwidx;
    sal_mutex_t qos_mutex;
} _bcm_th_qos_bookkeeping_t;

static _bcm_th_qos_bookkeeping_t _bcm_th_qos_bk_info[BCM_MAX_NUM_UNITS];
static int _bcm_th_qos_initialized[BCM_MAX_NUM_UNITS] = { 0 };

#define TH_QOS_INFO(_unit_) (&_bcm_th_qos_bk_info[_unit_])

#define _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(TH_QOS_INFO(_u_)->ing_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(TH_QOS_INFO((_u_))->ing_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(TH_QOS_INFO((_u_))->ing_l2_vlan_etag_map, (_identifier_))

#define _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(TH_QOS_INFO(_u_)->egr_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(TH_QOS_INFO((_u_))->egr_l2_vlan_etag_map, (_identifier_))
#define _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(TH_QOS_INFO((_u_))->egr_l2_vlan_etag_map, (_identifier_))

#define QOS_LOCK(unit) \
        sal_mutex_take(TH_QOS_INFO(unit)->qos_mutex, sal_mutex_FOREVER);
#define QOS_UNLOCK(unit) \
        sal_mutex_give(TH_QOS_INFO(unit)->qos_mutex);

#define QOS_INIT(unit)                                    \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!_bcm_th_qos_initialized[unit]) {            \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/*
 * Function: 
 *     _bcm_th_qos_map_id_alloc
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
_bcm_th_qos_map_id_alloc(int unit, SHR_BITDCL *bitmap, int *id, 
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

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/* 
 * Function:
 *      _bcm_th_qos_sw_dump
 * Purpose:
 *      Displays QOS software state info
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      NONE
 */
void
_bcm_th_qos_sw_dump(int unit)
{
    int i;
    if (!_bcm_th_qos_initialized[unit]) {
        LOG_CLI((BSL_META_U(unit,
                     "ERROR: QOS module not initialized on Unit:%d \n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "QOS: ING_L2_VLAN_ETAG_MAP info \n")));
    for (i = 0; i < _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS; i++) {
        if (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                         "    map id:%4d    HW index:%4d\n"), i, 
                         TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[i]));                         
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "QOS: EGR_L2_VLAN_ETAG_MAP info \n")));
    for (i = 0; i < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS; i++) {
        if (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                         "    map id:%4d    HW index:%4d\n"), i, 
                         TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[i]));                         
        }
    }    
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_WARM_BOOT_SUPPORT
#define _BCM_TH_REINIT_INVALID_HW_IDX  0xff    /* used as an invalid hw idx */

/* 
 * Function:
 *      _bcm_th_qos_reinit_scache_len_get
 * Purpose:
 *      Helper utility to determine scache details.
 * Parameters:
 *      unit        : (IN) Device Unit Number
 *      scache_len  : (OUT) Total required scache length
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_qos_reinit_scache_len_get(int unit, uint32 *scache_len)
{
    if (scache_len == NULL) {
        return BCM_E_PARAM;
    }

    *scache_len += _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS * sizeof(uint32);
    *scache_len += _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS * sizeof(uint32);

    return BCM_E_NONE;
}

int
_bcm_th_qos_reinit_from_hw_state(int unit, soc_mem_t mem, 
                                 soc_field_t field, uint8 map_type, 
                                 SHR_BITDCL *hw_idx_bmp, 
                                 int hw_idx_bmp_len)
{
    int         rv = BCM_E_NONE;
    int         idx, min_idx, max_idx, map_size, map_id, hw_prof_idx;
    uint32      buf[SOC_MAX_MEM_FIELD_WORDS];
    uint32      *hw_idx_table;
    SHR_BITDCL  *map_bmp;

    switch (map_type) {
    case _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP:
        map_bmp = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map;
        hw_idx_table = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx;
        map_size = _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS;
        break;
    case _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP:
        map_bmp = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map;
        hw_idx_table = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx;
        map_size = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS;
        break;
    default:
        return BCM_E_PARAM;
    }

    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);

    for (idx = min_idx; idx <= max_idx; idx++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &buf);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "Error(%s) reading mem(%d) at "
                                  "index:%d \n"), soc_errmsg(rv), mem, idx));
            return rv;
        }

        hw_prof_idx = soc_mem_field32_get(unit, mem, &buf, field);

        if (hw_prof_idx > (hw_idx_bmp_len - 1)) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "Invalid profile(%d) in mem(%d) "
                                  "at index:%d\n"), hw_prof_idx, mem, idx));
            return BCM_E_INTERNAL;
        }
        if (hw_prof_idx && (!SHR_BITGET(hw_idx_bmp, hw_prof_idx))) {
            /* non-zero profile id and not stored previously */
            BCM_IF_ERROR_RETURN(_bcm_th_qos_map_id_alloc(unit, map_bmp, &map_id,
                                     map_size));
            if (map_id < 0) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "Invalid profile(%d) in mem"
                                      "(%d) at index:%d\n"), hw_prof_idx, mem, idx));
                return BCM_E_RESOURCE;
            }
            hw_idx_table[map_id] = hw_prof_idx;
            SHR_BITSET(hw_idx_bmp, hw_prof_idx);
            SHR_BITSET(map_bmp, map_id);
        }
    }
    
    return rv;
}

/* 
 * Function:
 *      _bcm_th_qos_unsynchronized_reinit
 * Purpose:
 *      This routine handles Level 1 Warmboot init. In this style of reinit, the 
 *      map-ids allocated previously are not guaranteed to be the same.
 *      Cold-boot: Does nothing
 *      Warm-boot: The data-structures are populated by reading the HW tables. 
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_qos_unsynchronized_reinit(int unit)
{
    int                 rv = BCM_E_NONE;
    int                 bmp_len;
    SHR_BITDCL          *temp_bmp;

    /* read hw tables and populate the state */
    bmp_len = _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS;
    temp_bmp = NULL;
    temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
    if (temp_bmp == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
    
    if (soc_mem_is_valid(unit, PORT_TABm)) {
        rv = _bcm_th_qos_reinit_from_hw_state(unit, PORT_TABm, 
                     ETAG_PCP_DE_MAPPING_PTRf, 
                     _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP,
                     temp_bmp, bmp_len);
    }
    sal_free(temp_bmp);

    /* now extract EGR_MPLS_PRI_MAPPING state */
    if (BCM_SUCCESS(rv)) {
        bmp_len = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS;
        temp_bmp = NULL;
        temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
        if (temp_bmp == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
        if (soc_mem_field_valid(unit, PORT_TABm, 
                                ETAG_DOT1P_MAPPING_PTRf)) {
            rv = _bcm_th_qos_reinit_from_hw_state(unit, PORT_TABm,                      
                     ETAG_DOT1P_MAPPING_PTRf, 
                     _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP,
                     temp_bmp, bmp_len);
        }
        sal_free(temp_bmp);
    }

    return rv;
}

/* 
 * Function:
 *      _bcm_th_qos_extended_reinit
 * Purpose:
 *      This routine handles Level 2 Warmboot init. 
 *      Cold-boot: scache location is allocated.
 *      Warm-boot: The data-structs are populated from recovered scache state
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_qos_extended_reinit(int unit, uint8 **scache_ptr)
{
    uint32              hw_idx;
    int                 idx;

    for (idx = 0; idx < _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS; idx++) {
        sal_memcpy(&hw_idx, (*scache_ptr), sizeof(uint32));
        (*scache_ptr) += sizeof(hw_idx);
        if (hw_idx != _BCM_TH_REINIT_INVALID_HW_IDX) {
            _BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_SET(unit, idx);
            TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[idx] = hw_idx;
        }
    }
    for (idx = 0; idx < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS; idx++) {
        sal_memcpy(&hw_idx, (*scache_ptr), sizeof(uint32));
        (*scache_ptr) += sizeof(hw_idx);
        if (hw_idx != _BCM_TH_REINIT_INVALID_HW_IDX) {
            _BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_SET(unit, idx);
            TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[idx] = hw_idx;
        }
    }

    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_th_qos_reinit_hw_profiles_update
 * Purpose:
 *      Updates the shared memory profile tables reference counts
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_qos_reinit_hw_profiles_update(int unit)
{
    int     i;

    for (i = 0; i < _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS; i++) {
        if (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_ing_l2_vlan_etag_map_entry_reference(unit, 
                               ((TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[i]) * 
                               _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP), 
                               _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP));
        }
    }
    for (i = 0; i < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS; i++) {
        if (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_egr_l2_vlan_etag_map_entry_reference(unit, 
                               ((TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[i]) * 
                               _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP), 
                               _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP));
        }
    }    
 
    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_th_qos_sync
 * Purpose:
 *      This routine extracts the state that needs to be stored from the 
 *      book-keeping structs and stores it in the allocated scache location 
 * Compression format:  0   - A     l2_vlan_etag table
 *                      A+1 - B     egr_l2_vlan_etag table
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_qos_sync(int unit, uint8 **scache_ptr)
{
    uint32              hw_idx;
    int                 idx;    

    /* now store the state into the compressed format */
    for (idx = 0; idx < _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS; idx++) {
        if (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, idx)) {
            hw_idx = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[idx];
        } else {
            hw_idx = _BCM_TH_REINIT_INVALID_HW_IDX;
        }
        sal_memcpy((*scache_ptr), &hw_idx, sizeof(uint32));
        (*scache_ptr) += sizeof(hw_idx);
    }
    for (idx = 0; idx < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS; idx++) {
        if (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, idx)) {
            hw_idx = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[idx];
        } else {
            hw_idx = _BCM_TH_REINIT_INVALID_HW_IDX;
        }
        sal_memcpy((*scache_ptr), &hw_idx, sizeof(uint32));
        (*scache_ptr) += sizeof(hw_idx);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function: 
 *   _bcm_th_qos_free_resources
 * Purpose:
 *   Free QOS bookkeeping resources associated with a given unit.  This
 *   happens either during an error initializing or during a detach operation.
 * Parameters:
 *   unit - (IN) Unit to free resources of
 * Returns:
 *   BCM_E_XXX
 */
STATIC void
_bcm_th_qos_free_resources(int unit)
{
    _bcm_th_qos_bookkeeping_t *qos_info = TH_QOS_INFO(unit);

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
 *   bcm_th_qos_detach
 * Purpose:
 *   Detach the QOS module.  Called when the module should de-initialize.
 * Parameters:
 *   unit - (IN) Unit being detached.
 * Returns:
 *   BCM_E_XXX
 */
int 
bcm_th_qos_detach(int unit)
{
    if (_bcm_th_qos_initialized[unit]) {
        _bcm_th_qos_initialized[unit] = 0;
        _bcm_th_qos_free_resources(unit);
    }

    return BCM_E_NONE;
}
		
/*
 * Function: 
 *   bcm_th_qos_init
 * Purpose:
 *   Initialize the TH QOS module.  Allocate bookkeeping information.
 * Parameters:
 *   unit - (IN) Unit being initialized
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_th_qos_init(int unit)
{
    _bcm_th_qos_bookkeeping_t *qos_info = TH_QOS_INFO(unit);
    int l2_vlan_tag_map_size  = _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS;
    int egr_l2_vlan_tag_map_size  = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS;
    bcm_pbmp_t all_pbmp;
    uint32 entry[SOC_MAX_MEM_WORDS];
    bcm_port_t port;
    int index;

    if (_bcm_th_qos_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_th_qos_detach(unit));
    }

    memset(qos_info, 0, sizeof *qos_info);

    /* ingress l2 vlan etag map bitmap */
    qos_info->ing_l2_vlan_etag_map = 
                    sal_alloc(SHR_BITALLOCSIZE(l2_vlan_tag_map_size), 
                    "ing_l2_vlan_etag_map");
    if (qos_info->ing_l2_vlan_etag_map == NULL) {
        _bcm_th_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->ing_l2_vlan_etag_map, 0, 
                SHR_BITALLOCSIZE(l2_vlan_tag_map_size));

    /* ingress l2 vlan etag map hardware indexes */
    qos_info->ing_l2_vlan_etag_map_hwidx = 
                sal_alloc(sizeof(uint32) * l2_vlan_tag_map_size, 
                "ing_l2_vlan_etag_map_hwidx");
    if (qos_info->ing_l2_vlan_etag_map_hwidx == NULL) {
        _bcm_th_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->ing_l2_vlan_etag_map_hwidx, 0, 
                sizeof(uint32) * l2_vlan_tag_map_size);

    /* egress l2 vlan etag priority map bitmap */
    qos_info->egr_l2_vlan_etag_map = sal_alloc(SHR_BITALLOCSIZE(egr_l2_vlan_tag_map_size), 
                                "egr_l2_vlan_etag_map");
    if (qos_info->egr_l2_vlan_etag_map == NULL) {
        _bcm_th_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->egr_l2_vlan_etag_map, 0, 
                SHR_BITALLOCSIZE(egr_l2_vlan_tag_map_size));

    /* egress l2 vlan etag map hardware indexes */
    qos_info->egr_l2_vlan_etag_map_hwidx = 
                sal_alloc(sizeof(uint32) * egr_l2_vlan_tag_map_size, 
                "egr_l2_vlan_etag_map_hwidx");
    if (qos_info->egr_l2_vlan_etag_map_hwidx == NULL) {
        _bcm_th_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(qos_info->egr_l2_vlan_etag_map_hwidx, 0, 
                sizeof(uint32) * egr_l2_vlan_tag_map_size);

    /* accessor mutex */
    qos_info->qos_mutex = sal_mutex_create("vft qos_mutex");
    if (qos_info->qos_mutex == NULL) {
        _bcm_th_qos_free_resources(unit);
        return BCM_E_MEMORY;
    }

    if (SOC_WARM_BOOT(unit)) {
        /* Update reference counts for DSCP_TABLE profile*/
        BCM_PBMP_CLEAR(all_pbmp);
        BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

        PBMP_ITER(all_pbmp, port) {
            if (IS_LB_PORT(unit, port)) {
                /* Loopback port base starts from 64 */
                continue;
            }
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ALL,
                                          port, &entry));
            index = soc_mem_field32_get(unit, PORT_TABm, &entry, TRUST_DSCP_PTRf);
            SOC_IF_ERROR_RETURN(_bcm_dscp_table_entry_reference(unit, index * 64, 64));
        }
    }

    _bcm_th_qos_initialized[unit] = 1;
    return BCM_E_NONE;
}

/*
 * Function: 
 *   _bcm_th_qos_flags_sanity_check
 * Purpose:
 *   Check that the combination of flags specified is valid.
 * Parameters:
 *   flags  - (IN) BCM_QOS_MAP_* flags (eg. ingress/egress/with_id/replace)
  * Returns:
 *   BCM_E_XXX
 */
STATIC int
_bcm_th_qos_flags_sanity_check(uint32 flags)
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
 *     _bcm_th_qos_assign_creation_table_vars
 * Purpose:
 *     Assign the various pointers to tables and table sizes and bitmaps used
 *     for the bcm_th_qos_map_create so that it can create the different types
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
_bcm_th_qos_assign_creation_table_vars(int unit, int flags, int *is_ingress, 
                                        SHR_BITDCL **bitmap,
                                        uint32 **hwmap, int *map_type, 
                                        int *map_size, int *max_maps)
{
    if (flags & BCM_QOS_MAP_INGRESS) {
        *is_ingress = 1;
        if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
            *bitmap     = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map;
            *hwmap      = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx;
            *map_type   = _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP;
            *map_size   = _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS;
            *max_maps   = _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS;
        }        
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        *is_ingress = 0;

        if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
            *bitmap     = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map;
            *hwmap      = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx;
            *map_type   = _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP;
            *map_size   = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS;
            *max_maps   = _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS;
        }
    } 
}

/*
 * Function: 
 *     _bcm_th_qos_count_used_maps
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
_bcm_th_qos_count_used_maps(int unit, SHR_BITDCL *bitmap, uint32 *hwmap, 
                             int map_size, int max_maps) 
{
    uint8 *hw_used = sal_alloc(map_size, "TH_qos_used_maps");
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
 *     _bcm_th_qos_map_create_with_id
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
_bcm_th_qos_map_create_with_id(int unit, int flags, SHR_BITDCL *bitmap, 
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
 *     bcm_th_qos_map_create
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
bcm_th_qos_map_create(int unit, uint32 flags, int *map_id)
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

    rv = _bcm_th_qos_flags_sanity_check(flags);

    if (BCM_SUCCESS(rv)) {
        _bcm_th_qos_assign_creation_table_vars(unit, flags, &is_ingress, 
                                                &bitmap, &hwmap, &map_type, 
                                                &map_size, &max_maps);
    }

    if (BCM_SUCCESS(rv)) {
        if (_bcm_th_qos_count_used_maps(unit, bitmap, hwmap, map_size, 
                                         max_maps) >= max_maps) {
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        if (flags & BCM_QOS_MAP_WITH_ID) {
            rv = _bcm_th_qos_map_create_with_id(unit, flags, bitmap, map_id, 
                                                 is_ingress, map_size);
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
        } else {
            rv = _bcm_th_qos_map_id_alloc(unit, bitmap, &id, map_size);
            if (BCM_SUCCESS(rv)) {
                /*_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_SET(unit, id);*/
                *map_id = id | (map_type << _BCM_QOS_MAP_SHIFT);
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        if ((flags & BCM_QOS_MAP_INGRESS) &&
            (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
            ing_etag_pcp_mapping_entry_t 
                                table[_BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP];
            memset(&table, 0, sizeof table);
            entries[0] = &table;

            rv = _bcm_ing_l2_vlan_etag_map_entry_add(unit, entries, 
                                           _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP, 
                                           &index);

            if (BCM_SUCCESS(rv)) {
                TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] = index /
                                           _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP;
            }
        } else if ((flags & BCM_QOS_MAP_EGRESS) &&
                   (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
            egr_etag_pcp_mapping_entry_t 
                                table[_BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP];
            memset(&table, 0, sizeof table);
            entries[0] = &table;

            rv = _bcm_egr_l2_vlan_etag_map_entry_add(unit, entries, 
                                _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP, 
                                &index);

            if (BCM_SUCCESS(rv)) {
                TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] = index /
                                        _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP;
            }
        } 
    }

    QOS_UNLOCK(unit);

    return rv;
}

/*
 * Function: 
 *     _bcm_th_qos_map_destroy
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
_bcm_th_qos_map_destroy(int unit, int table, int id)
{
    int rv = BCM_E_NONE;
    _bcm_th_qos_bookkeeping_t *qos_info = TH_QOS_INFO(unit);

    if (table == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) {
        rv = _bcm_ing_l2_vlan_etag_map_entry_del(unit, 
                                    qos_info->ing_l2_vlan_etag_map_hwidx[id] *
                                    _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP);
    } else if (table == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) {
        rv = _bcm_egr_l2_vlan_etag_map_entry_del(unit, 
                                    qos_info->egr_l2_vlan_etag_map_hwidx[id] *
                                    _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP);
    }

    return rv;
}

/*
 * Function: 
 *     bcm_th_qos_map_destroy
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
bcm_th_qos_map_destroy(int unit, int map_id)
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
                rv = _bcm_th_qos_map_destroy(unit, 
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
                rv = _bcm_th_qos_map_destroy(unit, 
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
 *     _bcm_th_qos_map_add_sanity_check
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
_bcm_th_qos_map_add_sanity_check(int unit, uint32 flags, bcm_qos_map_t *map, 
                                 int map_id)
{
    int map_type = map_id >> _BCM_QOS_MAP_SHIFT;
    int id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    int rv;

    rv = _bcm_th_qos_flags_sanity_check(flags);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if ((flags & BCM_QOS_MAP_INGRESS) &&
        (map_type != _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP)) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_QOS_MAP_EGRESS) &&
        (map_type != _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP)) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_QOS_MAP_INGRESS) &&
        (map_type == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) &&
        (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id) == 0)) {
        return BCM_E_BADID;
    }

    if ((flags & BCM_QOS_MAP_EGRESS) &&
        (map_type == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) &&
        (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id) == 0)) {
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

    if ((flags & BCM_QOS_MAP_L2_VLAN_ETAG) &&
        (flags & BCM_QOS_MAP_EGRESS)) {
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
 *     _bcm_th_ing_qos_map_update_index
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
_bcm_th_ing_qos_map_update_index(int unit, bcm_qos_map_t *map, int map_id)
{
    void    *entries[1];
    int     rv = BCM_E_NONE;
    uint32  index;
    int     id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    if (QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        ing_etag_pcp_mapping_entry_t table[_BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP];        
        index = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] *
                                            _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP;

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
            TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] = index /
                                            _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP;  
        }
    }
    return rv;
}

/*
 * Function: 
 *     _bcm_th_egr_qos_map_update_index
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
_bcm_th_egr_qos_map_update_index(int unit, bcm_qos_map_t *map, int map_id)
{
    void    *entries[1];
    int     rv = BCM_E_NONE;
    int     cng;
    uint32  index;
    int     id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    if (QOS_MAP_IS_L2_VLAN_ETAG(map_id)) {
        egr_etag_pcp_mapping_entry_t table[_BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP];
        index = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] * 
                                            _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP;

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
            TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] = index /
                                            _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP;  
        }
    }

    return rv;
}

/*
 * Function: 
 *     bcm_th_qos_map_add
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
bcm_th_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;

    QOS_INIT(unit);
    QOS_LOCK(unit);

    rv = _bcm_th_qos_map_add_sanity_check(unit, flags, map, map_id);
    if (BCM_SUCCESS(rv)) {
        if ((flags & BCM_QOS_MAP_INGRESS) &&
            (flags & BCM_QOS_MAP_L2_VLAN_ETAG)) {
            rv = _bcm_th_ing_qos_map_update_index(unit, map, map_id);
        } else {
            rv = _bcm_th_egr_qos_map_update_index(unit, map, map_id);
        }
    }

    QOS_UNLOCK(unit);

    return rv;
}

/* bcm_th_qos_map_multi_get */
int
bcm_th_qos_map_multi_get(int unit, uint32 flags,
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
        hw_id = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id] *
                                            _BCM_QOS_MAP_CHUNK_L2_VLAN_ETAG_MAP;
        mem = ING_ETAG_PCP_MAPPINGm;
        break;
    case _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP:
        if ((flags != (BCM_QOS_MAP_L2_VLAN_ETAG | BCM_QOS_MAP_EGRESS)) ||
            (!_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP;
        entry_size = sizeof(egr_etag_pcp_mapping_entry_t);
        hw_id = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id] *
                                        _BCM_QOS_MAP_CHUNK_EGR_L2_VLAN_ETAG_MAP;
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
    dma_buf = soc_cm_salloc(unit, alloc_size, "TH qos multi get DMA buf");
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
 *     bcm_th_qos_map_delete
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
bcm_th_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t clr_map;
    
    QOS_INIT(unit);

    sal_memcpy(&clr_map, map, sizeof clr_map);

    rv = _bcm_th_qos_flags_sanity_check(flags);
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
        rv = bcm_th_qos_map_add(unit, flags, &clr_map, map_id);
    }

    return rv;
}

/*
 * Function: 
 *     _bcm_th_qos_apply_ing_map_to_port
 * Purpose:
 *     Apply a fully created ingress map to a hardware port.  This is the final
 *     step in the process, where the mapping becomes operational.
 * Parameters:
 *     unit       - (IN) Unit being operated on
 *     ing_map    - (IN) ID of the map to be applied
 *     local_port - (IN) Local port number to apply map to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_qos_apply_ing_map_to_port(int unit, int ing_map, bcm_port_t local_port)
{
    int rv = BCM_E_PARAM;
    int id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
    int profile_idx = TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[id]; 
    
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
 *     _bcm_th_qos_apply_egr_map_to_port
 * Purpose:
 *     Apply a fully created ingress map to a hardware port.  This is the final
 *     step in the process, where the mapping becomes operational.
 * Parameters:
 *     unit       - (IN) Unit being operated on
 *     ing_map    - (IN) ID of the map to be applied
 *     local_port - (IN) Local port number to apply map to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_qos_apply_egr_map_to_port(int unit, int egr_map, bcm_port_t local_port)
{
    int rv = BCM_E_PARAM;
    int id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
    int profile_idx = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id];  
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
 *     bcm_th_qos_port_map_set
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
bcm_th_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
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
            rv = _bcm_th_qos_apply_ing_map_to_port(unit, ing_map, local_port);
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
            rv = _bcm_th_qos_apply_egr_map_to_port(unit, egr_map, local_port);
        }
    }

    QOS_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *     _bcm_th_qos_port_ing_profile_get
 * Purpose:
 *     Retrieve a specific ingress QoS map index from given port.
 * Parameters:
 *     unit               - (IN) Unit being operated on
 *     local_port         - (IN) Local port number to apply map to
 *     ing_map_type       - (IN) type of the map to be retrieved
 *     profile_idx        - (OUT) Hardware index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_qos_port_ing_profile_get(int unit, bcm_port_t local_port,
                                 int ing_map_type, int *profile_idx)
{
    int rv = BCM_E_PARAM;

    if (profile_idx == NULL) {
        return BCM_E_PARAM;
    }

    if (ing_map_type == _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP) {
        rv = _bcm_esw_port_tab_get(unit, local_port, ETAG_PCP_DE_MAPPING_PTRf,
                                   profile_idx);
    }

    return rv;
}

/*
 * Function:
 *     _bcm_th_qos_port_egr_profile_get
 * Purpose:
 *     Retrieve a specific egress QoS map index from given port.
 * Parameters:
 *     unit                 - (IN) Unit being operated on
 *     local_port           - (IN) Local port number to apply map to
 *     egr_map_type         - (IN) type of the map to be retrieved
 *     profile_idx          - (OUT) Hardware index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th_qos_port_egr_profile_get(int unit, bcm_port_t local_port,
                                 int egr_map_type, int *profile_idx)
{
    int rv = BCM_E_PARAM;

    if (profile_idx == NULL) {
        return BCM_E_PARAM;
    }

    if (egr_map_type == _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP) {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}


/* Function:
*     _bcm_th_qos_id2idx
* Purpose:
*    Translate hardware table index into map ID used by API
* Parameters:
* Returns:
*    BCM_E_XXX
*/
int
_bcm_th_qos_idx2id(int unit, int hw_idx, int type, int *map_id)
{
    int idx;
    int rv = BCM_E_NOT_FOUND;

    if (map_id == NULL) {
        return BCM_E_PARAM;
    }

    QOS_INIT(unit);

    switch (type) {
    case _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP:
        for (idx = 0; idx < _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS; idx++) {
            if (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, idx)) {
                if (TH_QOS_INFO(unit)->ing_l2_vlan_etag_map_hwidx[idx]
                        == hw_idx) {
                    *map_id = idx | (_BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP
                                         << _BCM_QOS_MAP_SHIFT);
                    rv = BCM_E_NONE;
                    break;
                }
            }
        }

        break;

    case _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP:
        for (idx = 0; idx < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS; idx++) {
            if (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, idx)) {
                if (TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[idx]
                        == hw_idx) {
                    *map_id = idx | (_BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP
                                         << _BCM_QOS_MAP_SHIFT);
                    rv = BCM_E_NONE;
                    break;
                }
            }
        }

        break;

    default:
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *     bcm_th_qos_port_map_type_get
 * Purpose:
 *     Retrieve ingress or egress map from a given port.
 * Parameters:
 *     unit         - (IN) The unit to be operated on
 *     port         - (IN) The port number for the map to be retrieved from
 *     flags        - (IN) type of the map which is of interest
 *     map_id       - (OUT) The ID of map returned
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th_qos_port_map_type_get(int unit, bcm_gport_t port, uint32 flags,
                             int *map_id)
{
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    int map_type;
    int profile_idx;

    QOS_INIT(unit);

    if (map_id == NULL) {
        rv = BCM_E_PARAM;
    }
    if (!BCM_GPORT_IS_SET(port)) {
        rv = BCM_E_PARAM;
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_port_local_get(unit, port, &local_port);
    }

    if (BCM_SUCCESS(rv) && !SOC_PORT_VALID(unit, local_port)) {
        rv = BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(rv);

    if (flags & BCM_QOS_MAP_INGRESS) {
        if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
            map_type = _BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP;
        } else {
           rv = BCM_E_PARAM;
           return rv;
        }
        rv = _bcm_th_qos_port_ing_profile_get(unit, local_port, map_type,
                                              &profile_idx);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        if (flags & BCM_QOS_MAP_L2_VLAN_ETAG) {
            map_type = _BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP;
        } else {
            rv = BCM_E_PARAM;
            return rv;
        }
        rv = _bcm_th_qos_port_egr_profile_get(unit, local_port, map_type,
                                              &profile_idx);
    } else {
        rv = BCM_E_PARAM;
        return rv;
    }

    if(BCM_SUCCESS(rv)) {
        /* map H/W profile ID to API QoS map ID */
        rv = _bcm_th_qos_idx2id(unit, profile_idx, map_type, map_id);
    }

    return rv;
}

/* Function:
 *      bcm_th_qos_egr_etag_id2profile
 *  Purpose:
 *      Given a QOS map ID for a egress etag mapping profile, return the profile number
 *     into the EGR_ETAG_PCP_MAPPING table.
 *  Parameters:
 *      unit - (IN) Unit being operated on
 *      map_id - (IN) ID of the map to be retrieved
 *      profile_num - (OUT) Hardware profile number that map_id maps to
 *  Returns:
 *      BCM_E_XXX
 */
int
bcm_th_qos_egr_etag_id2profile(int unit, int map_id, int *profile_num)
{
    int id;

    QOS_INIT(unit);
    id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    QOS_LOCK(unit);
    if (!_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, id)) {
        QOS_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    } else {
        *profile_num = TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[id];
    }

    QOS_UNLOCK(unit);
    return BCM_E_NONE;
}

/* Function:
*      bcm_th_qos_egr_etag_profile2id
* Purpose:
*      Given a EGR_ETAG_PCP_MAPPING table profile number, find the QOS map ID which maps to it.
* Parameters:
*      unit - (IN) Unit being operated on
*      profile_num - (IN) profile number of the map to be retrieved
 *     map_id       - (OUT) Map ID that points to profile_num
* Returns:
*      BCM_E_XXX
*/
int
bcm_th_qos_egr_etag_profile2id(int unit, int profile_num, int *map_id)
{
    int i;

    QOS_INIT(unit);

    QOS_LOCK(unit);
    for (i = 0; i < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAP_IDS; i++) {
        if (TH_QOS_INFO(unit)->egr_l2_vlan_etag_map_hwidx[i] == profile_num) {
                *map_id = (_BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP << _BCM_QOS_MAP_SHIFT) | i;
                 QOS_UNLOCK(unit);
                 return BCM_E_NONE;
        }
    }

    QOS_UNLOCK(unit);
    return BCM_E_BADID;
}

int
_bcm_th_qos_multi_get(int unit, int array_size, int *map_ids_array,
                      int *flags_array, int *array_count)
{
    /* Assume that QoS lock has been taken */

    int idx, count;
    int rv = BCM_E_NONE;

    if (array_size == 0) {
        if (array_count == NULL) {
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv)) {
            *array_count = 0;
            count = 0;
            SHR_BITCOUNT_RANGE(TH_QOS_INFO(unit)->ing_l2_vlan_etag_map, count,
                               0, _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS);
            *array_count += count;
            count = 0;
            SHR_BITCOUNT_RANGE(TH_QOS_INFO(unit)->egr_l2_vlan_etag_map, count,
                               0, _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS);
            *array_count += count;
        }
    } else {
        if ((map_ids_array == NULL) || (flags_array == NULL) ||
            (array_count == NULL)) {
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv)) {
            count = 0;
            for (idx = 0; ((idx < _BCM_QOS_MAX_L2_VLAN_ETAG_MAPS) &&
                 (count < array_size)); idx++) {
                if (_BCM_QOS_ING_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, idx)) {
                    *(map_ids_array + count) =
                        idx | (_BCM_QOS_MAP_TYPE_ING_L2_VLAN_ETAG_MAP <<
                               _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = BCM_QOS_MAP_L2_VLAN_ETAG | BCM_QOS_MAP_INGRESS;
                    count++;
                }
            }
            for (idx = 0; ((idx < _BCM_QOS_MAX_EGR_L2_VLAN_ETAG_MAPS) &&
                 (count < array_size)); idx++) {
                if (_BCM_QOS_EGR_L2_VLAN_ETAG_MAP_TABLE_USED_GET(unit, idx)) {
                    *(map_ids_array + count) =
                        idx | (_BCM_QOS_MAP_TYPE_EGR_L2_VLAN_ETAG_MAP <<
                               _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = BCM_QOS_MAP_L2_VLAN_ETAG | BCM_QOS_MAP_EGRESS;
                    count++;
                }
            }

            *array_count = count;
        }

    }

    return rv;
}

#endif /* BCM_TOMAHAWK_SUPPORT */

