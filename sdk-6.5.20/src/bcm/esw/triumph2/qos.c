/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * QoS module
 */
#include <soc/defs.h>
#include <shared/bsl.h>
#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH2_SUPPORT)

#include <shared/util.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/qos.h>
#include <bcm/error.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/qos.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/switch.h>
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
#include <bcm_int/esw/mpls.h>
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw/saber2.h>
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/subport.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#if defined(INCLUDE_L3)
#include <bcm_int/esw/mim.h>
#endif

#define DEFAULT_VIEW_OTAG_FROM_TABLE   0
#define DEFAULT_VIEW_OTAG_FROM_QOS_MAP 1
#define L2_OTAG_VIEW_OTAG_FROM_TABLE   1
#define L2_OTAG_VIEW_OTAG_FROM_QOS_MAP 2

#define _BCM_QOS_MAP_TYPE_MASK           0x3ff
#define _BCM_QOS_MAP_SHIFT                  10

#define _BCM_QOS_MAP_CHUNK_EGR_MPLS  64
#define _BCM_QOS_MAP_CHUNK_DSCP  64
#define _BCM_QOS_MAP_CHUNK_EGR_DSCP  64
#define _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP  8
#define _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP_1 16
#define _BCM_QOS_MAP_CHUNK_OFFSET_MAP  16
#define _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP 16
#define _BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP 16
#define _BCM_QOS_MAP_CHUNK_PRI_CNG_MAX 64
#define _BCM_QOS_MAP_CHUNK_PRI_CNG  (_bcm_tr2_qos_bk_info[unit].pri_cng_chunk_size)

#define _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP    \
            (soc_mem_index_count(unit, _bcm_tr2_qos_bk_info[unit].inf_pri_cng_mapping)/ \
             _BCM_QOS_MAP_CHUNK_PRI_CNG)
#define _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS      \
            ((soc_mem_is_valid(unit, EGR_MPLS_PRI_MAPPINGm) ? \
             soc_mem_index_count(unit, EGR_MPLS_PRI_MAPPINGm) : \
             (soc_mem_is_valid(unit, EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m) ? \
             soc_mem_index_count(unit, EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m) : 1024)) / \
             _BCM_QOS_MAP_CHUNK_EGR_MPLS)
#define _BCM_QOS_MAP_LEN_DSCP_TABLE         \
            (soc_mem_index_count(unit, _bcm_tr2_qos_bk_info[unit].dscp_table)/ \
             _BCM_QOS_MAP_CHUNK_DSCP)
#define _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE     \
             (soc_mem_index_count(unit, _bcm_tr2_qos_bk_info[unit].egr_dscp_table)/ \
              _BCM_QOS_MAP_CHUNK_EGR_DSCP)
#define _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP   \
            (soc_mem_is_valid(unit, _bcm_tr2_qos_bk_info[unit].ing_mpls_exp_mapping) ? \
             (soc_mem_index_count(unit, _bcm_tr2_qos_bk_info[unit].ing_mpls_exp_mapping)/ \
              _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP) : 0)
#define _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP_1   \
            (soc_mem_is_valid(unit, ING_MPLS_EXP_MAPPING_1m) ? \
             (soc_mem_index_count(unit, ING_MPLS_EXP_MAPPING_1m)/ \
              _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP_1) : 0)
#define _BCM_QOS_MAP_LEN_EGR_MPLS_EXP_MAP_3   \
            (soc_mem_is_valid(unit, EGR_MPLS_EXP_MAPPING_3m) ? \
             (soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_3m)/ \
              _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP) : 0)

#ifdef BCM_KATANA_SUPPORT
#define _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE         \
            (soc_mem_is_valid(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm) ? \
            (soc_mem_index_count(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm)/ \
             _BCM_QOS_MAP_CHUNK_OFFSET_MAP) : 0)
#endif

#ifdef BCM_KATANA2_SUPPORT
#define _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP         \
            (SOC_REG_IS_VALID(unit, RQE_QUEUE_OFFSETr) ? \
            ((SOC_REG_NUMELS(unit, RQE_QUEUE_OFFSETr)  * 4) / \
             _BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP) : 0)
#endif

#ifdef BCM_SABER2_SUPPORT
#define _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE         \
            (soc_mem_is_valid(unit, ING_SERVICE_PRI_MAP_0m) ? \
            (soc_mem_index_count(unit, ING_SERVICE_PRI_MAP_0m)/ \
             _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP) : 0)
#endif
#define _BCM_QOS_GPORT_IS_VFI_VP_PORT(port) \
    (BCM_GPORT_IS_MIM_PORT(port) ||   \
     BCM_GPORT_IS_MPLS_PORT(port) || \
     BCM_GPORT_IS_VXLAN_PORT(port) || \
     BCM_GPORT_IS_FLOW_PORT(port) || \
     BCM_GPORT_IS_L2GRE_PORT(port))

#if defined(INCLUDE_L3)
#define MIM_INFO(_unit_) (&_bcm_tr2_mim_bk_info[_unit_])
#endif

typedef struct _bcm_tr2_qos_bookkeeping_s {
    SHR_BITDCL *ing_pri_cng_bitmap; /* ING_PRI_CNG_MAP chunks used */
    uint32 *pri_cng_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_mpls_bitmap; /* EGR_MPLS_EXP_MAPPING_1 /
                                  EGR_MPLS_PRI_MAPPING chunks used */
    uint32 *egr_mpls_hw_idx; /* Actual profile number used */
    SHR_BITDCL *dscp_table_bitmap; /* DSCP_TABLE chunks used */
    uint32 *dscp_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_dscp_table_bitmap; /* EGR_DSCP_TABLE chunks used */
    uint32 *egr_dscp_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_mpls_bitmap_flags; /* indicates if egr_mpls_bitmap
                                          entry is L2 or MPLS */
    SHR_BITDCL *ing_mpls_exp_bitmap; /* indicates which chunks of
                                ING_MPLS_EXP_MAPPINGm are used by QOS module */
#ifdef BCM_KATANA_SUPPORT
    SHR_BITDCL *offset_map_table_bitmap; /* ING_QUEUE_OFFSET_MAPPING_TABLE
                                            chunks used */
    uint32     *offset_map_hw_idx;   /* Actual profile number used */
#endif
#ifdef BCM_KATANA2_SUPPORT
    SHR_BITDCL *rqe_queue_offset_bitmap; /*  RQE_QUEUE_OFFSET
                                            chunks used */
    uint32     *rqe_queue_offset_hw_idx;   /* Actual profile number used */
    uint32     *rqe_queue_offset_refcount;        /* reference count per profile */
#endif
#ifdef BCM_SABER2_SUPPORT
    SHR_BITDCL *service_pri_map_table_bitmap; /* ING_SERVICE_PRI_MAP_0/1/2_TABLE
                                            chunks used */
    uint32     *service_pri_map_hw_idx;   /* Actual profile number used */
    uint32     *service_pri_map_flags;    /* The priority mode flag for a given
                                             id */
    uint32     pri_cng_hw_idx_reserved;   /* This reserved profile will be used for dynamic */
                                          /* update of profile  bound to interface. */
#define MAX_QOS_ING_PRI_PROFILE    64
    uint32     qos_profile_binding[BCM_MAX_NUM_UNITS][MAX_QOS_ING_PRI_PROFILE];
#endif
    soc_mem_t inf_pri_cng_mapping;
    soc_mem_t dscp_table;
    /* TD2 and before use only EGR_DSCP_TABLE to store DSCP map info for
     * both inner and outer DSCP.
     *
     * TD3 use two tables for inner and outer respectively
     */
    soc_mem_t egr_dscp_table;             /* egress inner dscp map table */
    soc_mem_t egr_dscp_table_outer;       /* egress outer dscp map table */
    soc_mem_t ing_mpls_exp_mapping;       /* ingress mpls exp mapping */
    uint32    pri_cng_chunk_size;         /* ingress PRI/CNG profile chunk size */
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    SHR_BITDCL *egr_mpls_external_bitmap; /* EGR_MPLS_EXP_MAPPING_1 /
                                  EGR_MPLS_PRI_MAPPING chunks used outside QOS*/
    uint32 *egr_mpls_external_idx_ref_count; /* Actual profile number used */
#endif
} _bcm_tr2_qos_bookkeeping_t;

STATIC _bcm_tr2_qos_bookkeeping_t  _bcm_tr2_qos_bk_info[BCM_MAX_NUM_UNITS];
STATIC sal_mutex_t _tr2_qos_mutex[BCM_MAX_NUM_UNITS] = {NULL};
STATIC int tr2_qos_initialized[BCM_MAX_NUM_UNITS]; /* Flag to check init status */

/*
 * QoS resource lock
 */
#define QOS_LOCK(unit) \
        sal_mutex_take(_tr2_qos_mutex[unit], sal_mutex_FOREVER);

#define QOS_UNLOCK(unit) \
        sal_mutex_give(_tr2_qos_mutex[unit]);

#define QOS_INFO(_unit_) (&_bcm_tr2_qos_bk_info[_unit_])
#define DSCP_CODE_POINT_CNT 64
#define DSCP_CODE_POINT_MAX (DSCP_CODE_POINT_CNT - 1)

/*
 * ING_PRI_CNG_MAP usage bitmap operations
 */
#define _BCM_QOS_ING_PRI_CNG_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->ing_pri_cng_bitmap, (_identifier_))
#define _BCM_QOS_ING_PRI_CNG_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->ing_pri_cng_bitmap, (_identifier_))
#define _BCM_QOS_ING_PRI_CNG_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->ing_pri_cng_bitmap, (_identifier_))
#ifdef BCM_SABER2_SUPPORT
#define _BCM_QOS_TRUST_DOT1P_PTR_CACHE_SET(_u_, _p_, _id_) \
        ((QOS_INFO(unit)->qos_profile_binding[_u_][_id_]) |= (1 << _p_))
#define _BCM_QOS_TRUST_DOT1P_PTR_CACHE_GET(_u_, _p_, _id_) \
        (((QOS_INFO(unit)->qos_profile_binding[_u_][_id_]) >> _p_) & 1)
#define _BCM_QOS_TRUST_DOT1P_PTR_CACHE_CLEAR(_u_, _p_, _id_) \
        ((QOS_INFO(unit)->qos_profile_binding[_u_][_id_]) &= (~(1 << _p_)))
#endif
/*
 * EGR_MPLS_EXP_MAPPING_1 / EGR_MPLS_PRI_MAPPING usage bitmap operations
 */
#define _BCM_QOS_EGR_MPLS_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_mpls_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_mpls_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_mpls_bitmap, (_identifier_))
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * EGR_MPLS_EXP_MAPPING_1 / EGR_MPLS_PRI_MAPPING external usage bitmap operations
 */
#define _BCM_QOS_EGR_MPLS_EXTERNAL_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_mpls_external_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_EXTERNAL_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_mpls_external_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_EXTERNAL_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_mpls_external_bitmap, (_identifier_))
#endif
/*
 * DSCP_TABLE usage bitmap operations
 */
#define _BCM_QOS_DSCP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_DSCP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_DSCP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->dscp_table_bitmap, (_identifier_))

/*
 * EGR_DSCP_TABLE usage bitmap operations
 */
#define _BCM_QOS_EGR_DSCP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_EGR_DSCP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_EGR_DSCP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_dscp_table_bitmap, (_identifier_))

/*
 * EGR_MPLS flags bitmap operations
 */
#define _BCM_QOS_EGR_MPLS_FLAGS_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_mpls_bitmap_flags, (_identifier_))
#define _BCM_QOS_EGR_MPLS_FLAGS_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_mpls_bitmap_flags, (_identifier_))
#define _BCM_QOS_EGR_MPLS_FLAGS_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_mpls_bitmap_flags, (_identifier_))

/*
 * ING_MPLS_EXP_MAPPING bitmap operations
 */
#define _BCM_QOS_ING_MPLS_EXP_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->ing_mpls_exp_bitmap, (_identifier_))
#define _BCM_QOS_ING_MPLS_EXP_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->ing_mpls_exp_bitmap, (_identifier_))
#define _BCM_QOS_ING_MPLS_EXP_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->ing_mpls_exp_bitmap, (_identifier_))

#define QOS_INIT(unit)                                    \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!tr2_qos_initialized[unit]) {                           \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/*
 * ING_QUEUE_OFFSET_MAPPING_TABLE usage bitmap operations
 */
#ifdef BCM_KATANA_SUPPORT
#define _BCM_QOS_OFFSET_MAP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->offset_map_table_bitmap, (_identifier_))
#define _BCM_QOS_OFFSET_MAP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->offset_map_table_bitmap, (_identifier_))
#define _BCM_QOS_OFFSET_MAP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->offset_map_table_bitmap, (_identifier_))

#endif

/*
 *RQE_QUEUE_OFFSET using bitmap operations
*/
#define _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->rqe_queue_offset_bitmap, (_identifier_))
#define _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->rqe_queue_offset_bitmap, (_identifier_))
#define _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->rqe_queue_offset_bitmap, (_identifier_))

/* ING_SERVICE_PRI_MAP_0/1/2 table usage bitmap operations */
#ifdef BCM_SABER2_SUPPORT
#define _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->service_pri_map_table_bitmap, (_identifier_))
#define _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->service_pri_map_table_bitmap, (_identifier_))
#define _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->service_pri_map_table_bitmap, (_identifier_))

#define _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_GET(_u_, idx) \
        (QOS_INFO((_u_))->service_pri_map_flags[idx])
#define _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_SET(_u_, idx, _flags_) \
        (QOS_INFO((_u_))->service_pri_map_flags[idx] = (_flags_))
#define _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_CLR(_u_, idx) \
        (QOS_INFO((_u_))->service_pri_map_flags[idx] = 0)
#endif

#if defined(INCLUDE_L3) && defined(BCM_FIREBOLT_SUPPORT)
extern soc_field_t nh_entry_type_field[SOC_MAX_NUM_DEVICES];
#endif

#ifdef BCM_TRIDENT3_SUPPORT
extern int
bcm_td3_l2_egr_combo_add(int unit,
                             uint32 flags,
                             bcm_qos_map_t *map,
                             int map_id,
                             uint32 *hw_idx,
                             uint32 entry_per_index);
#endif /* End of BCM_TRIDENT3_SUPPORT */

#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
int
_bcm_tr2_qos_egr_mpls_external_ref_add(int unit, int qos_map_id, int *hw_idx)
{
    int id = -1;
    id = qos_map_id & _BCM_QOS_MAP_TYPE_MASK;
    if ((qos_map_id >> _BCM_QOS_MAP_SHIFT) !=
            _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
        return BCM_E_BADID;
    }
    if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_qos_id2idx(unit, qos_map_id, hw_idx));
    _BCM_QOS_EGR_MPLS_EXTERNAL_USED_SET(unit, id);
    QOS_INFO(unit)->egr_mpls_external_idx_ref_count[id] ++;
    return BCM_E_NONE;
}
int
_bcm_tr2_qos_egr_mpls_external_ref_del(int unit, int hw_idx)
{
    int id = -1, qos_map_id;
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_qos_idx2id(unit, hw_idx,  _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, &qos_map_id));
    id = qos_map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_INFO(unit)->egr_mpls_external_idx_ref_count[id] --;
    if (!QOS_INFO(unit)->egr_mpls_external_idx_ref_count[id]) {
        _BCM_QOS_EGR_MPLS_EXTERNAL_USED_CLR(unit, id);
    }
    return BCM_E_NONE;
}
#endif
void
_bcm_tr2_qos_id_update(int unit, uint32 dscp_map_id, uint32 new_dscp_map_id)
{
    int ifindex;
    for (ifindex = 0; ifindex < _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE; ifindex++) {
        int hw_idx;
        hw_idx = QOS_INFO(unit)->egr_dscp_hw_idx[ifindex];
        if (hw_idx == dscp_map_id) {
            QOS_INFO(unit)->egr_dscp_hw_idx[ifindex] = new_dscp_map_id;
        }
    }
    return;
}

STATIC int
_bcm_tr2_qos_id_alloc(int unit, SHR_BITDCL *bitmap, uint8 map_type)
{
    int i, map_size = -1;

    switch (map_type) {
        case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
#ifdef BCM_HURRICANE3_SUPPORT
        case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
#endif
            map_size = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
            break;
        case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
#ifdef BCM_HURRICANE3_SUPPORT
        case _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS:
#endif
            map_size = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
            break;
        case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
            map_size = _BCM_QOS_MAP_LEN_DSCP_TABLE;
            break;
        case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
            map_size = _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
            break;
#ifdef BCM_KATANA_SUPPORT
        case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
            map_size = _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE;
            break;
#endif
#ifdef BCM_KATANA2_SUPPORT
        case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
            map_size = _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP;
            break;
#endif
#ifdef BCM_SABER2_SUPPORT
        case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
            if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
                map_size = _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE;
            } else {
                return BCM_E_PARAM;
            }
            break;
#endif
        default:
            return BCM_E_PARAM;
    }

    for (i = 0; i < map_size; i++) {
        if (!SHR_BITGET(bitmap, i)) {
            return i;
        }
    }
    return -1;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *      _bcm_qos_sw_dump
 * Purpose:
 *      Displays QOS software state info
 * Parameters:
 *      unit        : (IN) Device Unit Number
 * Returns:
 *      NONE
 */
void
_bcm_tr2_qos_sw_dump(int unit)
{
    int     i;

    if (!tr2_qos_initialized[unit]) {
        LOG_CLI((BSL_META_U(unit,
                            "ERROR: QOS module not initialized on Unit:%d \n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "QOS: ING_PRI_CNG_MAP info \n")));
    for (i=0; i<_BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; i++) {
        if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d    HW index:%4d\n"), i,
                     QOS_INFO(unit)->pri_cng_hw_idx[i]));
        }
    }

    if (SOC_MEM_IS_VALID(unit, EGR_MPLS_PRI_MAPPINGm)) {
        LOG_CLI((BSL_META_U(unit,
                            "QOS: EGR_MPLS_PRI_MAPPING info \n")));
        for (i=0; i<_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; i++) {
            if (_BCM_QOS_EGR_MPLS_USED_GET(unit, i)) {
                LOG_CLI((BSL_META_U(unit,
                                    "    map id:%4d    HW index:%4d (%s)\n"), i,
                         QOS_INFO(unit)->egr_mpls_hw_idx[i],
                         (_BCM_QOS_EGR_MPLS_FLAGS_USED_GET(unit, i)? "MPLS" : "L2")));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "QOS: DSCP_TABLE info \n")));
    for (i=0; i<_BCM_QOS_MAP_LEN_DSCP_TABLE; i++) {
        if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d    HW index:%4d\n"), i,
                     QOS_INFO(unit)->dscp_hw_idx[i]));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "QOS: EGR_DSCP_TABLE info \n")));
    for (i=0; i<_BCM_QOS_MAP_LEN_EGR_DSCP_TABLE; i++) {
        if (_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d    HW index:%4d\n"), i,
                     QOS_INFO(unit)->egr_dscp_hw_idx[i]));
        }
    }
#ifdef BCM_KATANA_SUPPORT
    if (QOS_INFO(unit)->offset_map_table_bitmap) {
    LOG_CLI((BSL_META_U(unit,
                        "QOS: OFFSET_MAP_TABLE info \n")));
    for (i=0; i < _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE; i++) {
        if (_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d    HW index:%4d\n"), i,
                     QOS_INFO(unit)->offset_map_hw_idx[i]));
        }
    }
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (QOS_INFO(unit)->rqe_queue_offset_bitmap) {
    LOG_CLI((BSL_META_U(unit,
                        "QOS: RQE_QUEUE_OFFSET info \n")));
    for (i=0; i < _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP; i++) {
        if (_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                        "    map id:%4d    HW index:%4d refcoutn %d \n"), i,
                         QOS_INFO(unit)->rqe_queue_offset_hw_idx[i],
                         QOS_INFO(unit)->rqe_queue_offset_refcount[i]));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n Bitmap: %d \n"),
             *QOS_INFO(unit)->rqe_queue_offset_bitmap));
    }
#endif /*BCM_KATANA2_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        LOG_CLI((BSL_META_U(unit,
                        "QOS: SERVICE_PRI_MAP_TABLE info \n")));
        for (i=0; i < _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE; i++) {
            if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, i)) {
                LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d    HW index:%4d\n"), i,
                            QOS_INFO(unit)->service_pri_map_hw_idx[i]));
            }
        }
    }
#endif
    LOG_CLI((BSL_META_U(unit,
                        "QOS: ING_MPLS_EXP_MAPPING info \n")));
    for (i=0; i<_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP; i++) {
        if (_BCM_QOS_ING_MPLS_EXP_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d\n"), i));
        }
    }
}
#endif

/* re-use the reinit function to populate the hw_idx_bmp
 * 'extract_only' flag is used only to extract the used map IDs in hw_idx_bmp without
 * reinitializing the current software structures */
int
_bcm_tr2_qos_reinit_from_hw_state(int unit, soc_mem_t mem, soc_field_t field,
                                 uint8 map_type, SHR_BITDCL *hw_idx_bmp,
                                 int hw_idx_bmp_len,
                                 int extract_only)
{
    int         rv = BCM_E_NONE;
    int         idx, min_idx, max_idx, map_id, hw_prof_idx, entry_type = 0;
    uint32      buf[SOC_MAX_MEM_FIELD_WORDS];
    uint32      *hw_idx_table = NULL;
    SHR_BITDCL  *map_bmp = NULL;

    if (0 == extract_only) {
        switch (map_type) {
            case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
                map_bmp = QOS_INFO(unit)->ing_pri_cng_bitmap;
                hw_idx_table = QOS_INFO(unit)->pri_cng_hw_idx;
                break;
            case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
                map_bmp = QOS_INFO(unit)->egr_mpls_bitmap;
                hw_idx_table = QOS_INFO(unit)->egr_mpls_hw_idx;
                break;
            case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
                map_bmp = QOS_INFO(unit)->dscp_table_bitmap;
                hw_idx_table = QOS_INFO(unit)->dscp_hw_idx;
                break;
            case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
                map_bmp = QOS_INFO(unit)->egr_dscp_table_bitmap;
                hw_idx_table = QOS_INFO(unit)->egr_dscp_hw_idx;
                break;
#ifdef BCM_KATANA_SUPPORT
            case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
                map_bmp = QOS_INFO(unit)->offset_map_table_bitmap;
                hw_idx_table = QOS_INFO(unit)->offset_map_hw_idx;
                break;
#endif
#ifdef BCM_KATANA2_SUPPORT
            case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
                map_bmp = QOS_INFO(unit)->rqe_queue_offset_bitmap;
                hw_idx_table = QOS_INFO(unit)->rqe_queue_offset_hw_idx;
                break;
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
            case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
                if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
                    map_bmp = QOS_INFO(unit)->service_pri_map_table_bitmap;
                    hw_idx_table = QOS_INFO(unit)->service_pri_map_hw_idx;
                } else {
                    return BCM_E_PARAM;
                }
                break;
#endif
            default:
                return BCM_E_PARAM;
        }
    }

    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);
    for (idx = min_idx; idx < max_idx; idx++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &buf);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "Error(%s) reading mem(%d) at "
                                  "index:%d \n"), soc_errmsg(rv), mem, idx));
            return rv;
        }
        if (mem == EGR_L3_NEXT_HOPm) {
            entry_type = soc_mem_field32_get(unit, mem, &buf, ENTRY_TYPEf);
            if ((entry_type != 2/* SD-Tag */) && (entry_type != 3/* MIM */)) {
                continue; /* Neither SD-tag nor MiM view */
            }
        }
        if (mem == EGR_VLAN_XLATEm) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
                if (SOC_IS_TRIUMPH3(unit)) {
                    entry_type = soc_mem_field32_get(unit, mem, &buf, KEY_TYPEf);
                } else if (SOC_IS_TD2_TT2(unit)) {
                    entry_type = soc_mem_field32_get(unit, mem, &buf, ENTRY_TYPEf);
                }
                if ((entry_type != 2) && (entry_type != 3)) {
                    continue; /* Neither MIM_ISID nor MIM_ISID_DVP */
                }
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                entry_type = soc_mem_field32_get(unit, mem, &buf, ENTRY_TYPEf);
                if ((entry_type != 3) && (entry_type != 4)) {
                    continue; /* Neither ISID_XLATE nor ISID_DVP_XLATE */
                }
            }
        }
        /* Some tables have a bit that specifies whether the profile is valid */
        if (soc_mem_field_valid(unit, mem, TRUST_OUTER_DOT1Pf)) {
            if (soc_mem_field32_get(unit, mem, &buf, TRUST_OUTER_DOT1Pf) == 0) {
                continue;
            }
        }
        if (soc_mem_field_valid(unit, mem, TRUST_DOT1Pf)) {
            if (soc_mem_field32_get(unit, mem, &buf, TRUST_DOT1Pf) == 0) {
                continue;
            }
        }
        if (soc_mem_field_valid(unit, mem, SD_TAG__SD_TAG_DOT1P_PRI_SELECTf)) {
            if (soc_mem_field32_get(unit, mem, &buf,
                                    SD_TAG__SD_TAG_DOT1P_PRI_SELECTf) == 0) {
                continue;
            }
        }
        if (soc_mem_field_valid(unit, mem, REMARK_DOT1Pf)) {
            if (soc_mem_field32_get(unit, mem, &buf, REMARK_DOT1Pf) == 0) {
                continue;
            }
        }
        if (soc_mem_field_valid(unit, mem, DOT1P_PRI_SELECTf)) {
            if (soc_mem_field32_get(unit, mem, &buf, DOT1P_PRI_SELECTf) == 0) {
                continue;
            }
        }
        if (soc_mem_field_valid(unit, mem, SD_TAG_DOT1P_PRI_SELECTf)) {
            if (soc_mem_field32_get(unit, mem, &buf,
                                    SD_TAG_DOT1P_PRI_SELECTf) == 0) {
                continue;
            }
        }
        if (soc_mem_field_valid(unit, mem,
                                MIM_ISID__SD_TAG_DOT1P_PRI_SELECTf)) {
            if (soc_mem_field32_get(unit, mem, &buf,
                                    MIM_ISID__SD_TAG_DOT1P_PRI_SELECTf) == 0) {
                continue;
            }
        }
        if ((mem == EGR_IP_TUNNELm) || (mem == EGR_L3_INTFm)) {
            if (soc_mem_field32_get(unit, mem, &buf, DSCP_SELf) != 2) {
                continue;
            }
        }

        if (mem == L3_IIFm && field == TRUST_DSCP_PTRf) {
            int field_len = 0, iif_profile_idx = 0;
            iif_profile_entry_t l3_iif_profile;

            COMPILER_REFERENCE(field_len);
            if (soc_feature(unit, soc_feature_l3_iif_profile)) {
                field_len = soc_mem_field_length(unit, L3_IIF_PROFILEm, field);
                iif_profile_idx = soc_mem_field32_get(unit, mem, &buf,
                                                      L3_IIF_PROFILE_INDEXf);
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IIF_PROFILEm,
                                     SOC_BLOCK_ANY, iif_profile_idx,
                                     &l3_iif_profile));
                hw_prof_idx = soc_mem_field32_get(unit, L3_IIF_PROFILEm,
                                                  &l3_iif_profile,
                                                  TRUST_DSCP_PTRf);
            } else {
                field_len = soc_mem_field_length(unit, mem, field);
                hw_prof_idx = soc_mem_field32_get(unit, mem, &buf, field);
            }
#if defined(INCLUDE_L3)
            if (field_len == _BCM_TR_L3_TRUST_DSCP_PTR_BIT_SIZE) {
                if (hw_prof_idx == 0x3f) {
                    continue;
                }
            } else if (field_len == _BCM_TD_L3_TRUST_DSCP_PTR_BIT_SIZE) {
                if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT2X(unit) ||
                    SOC_IS_TOMAHAWKX(unit) || SOC_IS_GREYHOUND2(unit)) {
                    if (hw_prof_idx == 0x7f) {
                        continue;
                    }
                } else {
                    if (hw_prof_idx == 0x0) {
                        continue;
                    }
                }
            }
#endif
        } else {
            hw_prof_idx = soc_mem_field32_get(unit, mem, &buf, field);
        }

        if (hw_prof_idx > (hw_idx_bmp_len - 1)) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "Invalid profile(%d) in mem(%d) "
                                  "at index:%d\n"), hw_prof_idx, mem, idx));
            return BCM_E_INTERNAL;
        }
        if (hw_prof_idx && (!SHR_BITGET(hw_idx_bmp, hw_prof_idx))) {
            /* non-zero profile id and not stored previously */
            if (0 == extract_only) {
                map_id = _bcm_tr2_qos_id_alloc(unit, map_bmp, map_type);
                if (map_id < 0) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                            (BSL_META_U(unit,
                                        "Invalid profile(%d) in mem"
                                        "(%d) at index:%d\n"), hw_prof_idx, mem, idx));
                    return BCM_E_RESOURCE;
                }
                hw_idx_table[map_id] = hw_prof_idx;
                SHR_BITSET(map_bmp, map_id);
            }
            SHR_BITSET(hw_idx_bmp, hw_prof_idx);
        }
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_4
#define _BCM_TR2_REINIT_INVALID_HW_IDX  0xff    /* used as an invalid hw idx */

/*
 * Function:
 *      _bcm_tr2_qos_reinit_scache_len_get
 * Purpose:
 *      Helper utility to determine scache details.
 * Parameters:
 *      unit        : (IN) Device Unit Number
 *      scache_len  : (OUT) Total required scache length
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_qos_reinit_scache_len_get(int unit, uint32* scache_len)
{
    if (scache_len == NULL) {
        return BCM_E_PARAM;
    }

    *scache_len = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
    *scache_len += _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
    *scache_len += _BCM_QOS_MAP_LEN_DSCP_TABLE;
    *scache_len += _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
    *scache_len += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
    *scache_len += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP);
#ifdef BCM_KATANA_SUPPORT
    *scache_len += _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE;
#endif
#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        *scache_len += _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE;
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    /* RQE_QUEUE_OFFSET
     * replication queue offse profile mapping table
     */
    *scache_len += (2 * _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE);
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        _bcm_td2_qos_reinit_scache_len_get(unit, scache_len);
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        _bcm_th_qos_reinit_scache_len_get(unit, scache_len);
    }
#endif
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)) { 
        *scache_len += (sizeof(uint32) * _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_qos_unsynchronized_reinit
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
_bcm_tr2_qos_unsynchronized_reinit(int unit)
{
    int                 rv = BCM_E_NONE;
    int                 bmp_len;
    SHR_BITDCL          *temp_bmp;
    int                 extract_only = 0;

    /* read hw tables and populate the state */
    bmp_len = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
    temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
    sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));

    if (soc_mem_is_valid(unit, SOURCE_VPm)) {
        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, SOURCE_VPm,
                           TRUST_DOT1P_PTRf, _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP,
                           temp_bmp, bmp_len, extract_only);
    }
    if (BCM_SUCCESS(rv) && soc_mem_is_valid(unit, LPORT_TABm)) {
        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, LPORT_TABm,
                           TRUST_DOT1P_PTRf, _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP,
                           temp_bmp, bmp_len, extract_only);
    }
    if (BCM_SUCCESS(rv) && soc_mem_is_valid(unit, PORT_TABm)) {
        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, PORT_TABm,
                           TRUST_DOT1P_PTRf, _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP,
                           temp_bmp, bmp_len, extract_only);
    }
    if (BCM_SUCCESS(rv) && soc_mem_field_valid(unit, VLAN_PROFILE_TABm,
                                               TRUST_DOT1P_PTRf)) {
        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, VLAN_PROFILE_TABm,
                           TRUST_DOT1P_PTRf, _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP,
                           temp_bmp, bmp_len, extract_only);
    }
    if (BCM_SUCCESS(rv) && soc_mem_is_valid(unit, VFIm) &&
        soc_mem_field_valid(unit, VFIm, TRUST_DOT1P_PTRf)) {
        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, VFIm,
                           TRUST_DOT1P_PTRf, _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP,
                           temp_bmp, bmp_len, extract_only);
    }
    sal_free(temp_bmp);

    /* now extract EGR_MPLS_PRI_MAPPING state */
    if (BCM_SUCCESS(rv)) {
        bmp_len = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
        temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
        sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_L3_NEXT_HOPm,
                           SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf,
                           _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, temp_bmp, bmp_len, extract_only);
        }
        if (BCM_SUCCESS(rv) && soc_mem_is_valid(unit, EGR_IP_TUNNELm)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_IP_TUNNELm,
                           DOT1P_MAPPING_PTRf, _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                           temp_bmp, bmp_len, extract_only);
        }
        if (BCM_SUCCESS(rv) &&
            soc_mem_is_valid(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm) &&
            soc_mem_field_valid(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                SD_TAG_DOT1P_MAPPING_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit,
                     EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                     SD_TAG_DOT1P_MAPPING_PTRf, _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                     temp_bmp, bmp_len, extract_only);
        }
        if (BCM_SUCCESS(rv) &&
            soc_mem_field_valid(unit, EGR_VLANm, DOT1P_MAPPING_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit,
                     EGR_VLANm, DOT1P_MAPPING_PTRf,
                     _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                     temp_bmp, bmp_len, extract_only);
        }
        if (BCM_SUCCESS(rv) && soc_mem_is_valid(unit, EGR_VLAN_XLATEm) &&
            soc_mem_field_valid(unit, EGR_VLAN_XLATEm,
                                MIM_ISID__DOT1P_MAPPING_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit,
                     EGR_VLAN_XLATEm, MIM_ISID__DOT1P_MAPPING_PTRf,
                     _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                     temp_bmp, bmp_len, extract_only);
        }
        sal_free(temp_bmp);
    }

    /* now extract DSCP_TABLE state */
    if (BCM_SUCCESS(rv) &&
        soc_feature(unit, soc_feature_l3_ingress_interface)) {
        bmp_len = _BCM_QOS_MAP_LEN_DSCP_TABLE;
        temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
        sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
        if (soc_mem_field_valid(unit, L3_IIFm, TRUST_DSCP_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit, L3_IIFm,
                                                   TRUST_DSCP_PTRf,
                              _BCM_QOS_MAP_TYPE_DSCP_TABLE, temp_bmp, bmp_len, extract_only);
        }
        if (BCM_SUCCESS(rv) &&
            soc_mem_is_valid(unit, SOURCE_VPm) &&
            soc_mem_field_valid(unit, SOURCE_VPm, TRUST_DSCP_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit, SOURCE_VPm,
                                                   TRUST_DSCP_PTRf,
                              _BCM_QOS_MAP_TYPE_DSCP_TABLE, temp_bmp, bmp_len, extract_only);
        }
        sal_free(temp_bmp);
    }

    /* now extract EGR_DSCP_TABLE state */
    if (BCM_SUCCESS(rv)) {
        bmp_len = _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
        temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
        sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
        if (soc_mem_field_valid(unit, EGR_IP_TUNNELm, DSCP_MAPPING_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_IP_TUNNELm,
                            DSCP_MAPPING_PTRf,_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                            temp_bmp, bmp_len, extract_only);
        }
        if (soc_mem_field_valid(unit, EGR_L3_INTFm, DSCP_MAPPING_PTRf)) {
            rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_L3_INTFm,
                            DSCP_MAPPING_PTRf,_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                            temp_bmp, bmp_len, extract_only);
        }
        sal_free(temp_bmp);
    }
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        rv = _bcm_td2_qos_unsynchronized_reinit(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        rv = _bcm_th_qos_unsynchronized_reinit(unit);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_qos_extended_reinit
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
_bcm_tr2_qos_extended_reinit(int unit)
{
    soc_scache_handle_t handle;
    uint8               *scache_ptr;
    uint32              hw_idx, scache_len, additional_scache_size = 0;
    int                 idx;
    int                 rv = BCM_E_NONE;
    uint16                 recovered_ver;
#ifdef BCM_KATANA2_SUPPORT
    int                 refcount = 0;
#endif


    BCM_IF_ERROR_RETURN(_bcm_tr2_qos_reinit_scache_len_get(unit, &scache_len));
    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_QOS, 0);

    if (SOC_WARM_BOOT(unit)) {
        /* during warm-boot recover state */
        BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, handle, FALSE,
                                            scache_len, &scache_ptr,
                                            BCM_WB_DEFAULT_VERSION, &recovered_ver));

        /* recover from scache into book-keeping structs */
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; idx++) {
            hw_idx = (uint32) (*scache_ptr++);
            if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                _BCM_QOS_ING_PRI_CNG_USED_SET(unit, idx);
                QOS_INFO(unit)->pri_cng_hw_idx[idx] = hw_idx;
            }
        }
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
            hw_idx = (uint32) (*scache_ptr++);
            if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                _BCM_QOS_EGR_MPLS_USED_SET(unit, idx);
                QOS_INFO(unit)->egr_mpls_hw_idx[idx] = hw_idx;
            }
        }
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_DSCP_TABLE; idx++) {
            hw_idx = (uint32) (*scache_ptr++);
            if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                _BCM_QOS_DSCP_TABLE_USED_SET(unit, idx);
                QOS_INFO(unit)->dscp_hw_idx[idx] = hw_idx;
            }
        }
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE; idx++) {
            hw_idx = (uint32) (*scache_ptr++);
            if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                _BCM_QOS_EGR_DSCP_TABLE_USED_SET(unit, idx);
                QOS_INFO(unit)->egr_dscp_hw_idx[idx] = hw_idx;
            }
        }
        sal_memcpy(QOS_INFO(unit)->egr_mpls_bitmap_flags, scache_ptr,
                   SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS));
        scache_ptr += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
        sal_memcpy(QOS_INFO(unit)->ing_mpls_exp_bitmap, scache_ptr,
                   SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP));
        scache_ptr += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP);
#ifdef BCM_KATANA_SUPPORT
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE; idx++) {
            hw_idx = (uint32) (*scache_ptr++);
            if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                _BCM_QOS_OFFSET_MAP_TABLE_USED_SET(unit, idx);
                QOS_INFO(unit)->offset_map_hw_idx[idx] = hw_idx;
            }
        }
#endif
#ifdef BCM_SABER2_SUPPORT
        if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
            for (idx = 0; idx < _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE; idx++) {
                hw_idx = (uint32) (*scache_ptr++);
                if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                    _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_SET(unit, idx);
                    QOS_INFO(unit)->service_pri_map_hw_idx[idx] = hw_idx;
                }
            }
        }
#endif
#ifdef BCM_KATANA2_SUPPORT
        /* RQE_QUEUE_OFFSET
         * Replication queue offset profile mapping table
        * Hw indices
        * refcount
        */
       if (recovered_ver >=  BCM_WB_VERSION_1_1) {
           for (idx = 0; idx < _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP; idx++) {
               hw_idx = (uint32) (*scache_ptr++);
               if (hw_idx != _BCM_TR2_REINIT_INVALID_HW_IDX) {
                   _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_SET(unit,  idx);
                   QOS_INFO(unit)->rqe_queue_offset_hw_idx[idx] = hw_idx;
               }
               refcount = (uint32) (*scache_ptr++);
               if (refcount != 0) {
                   QOS_INFO(unit)->rqe_queue_offset_refcount[idx] = refcount;
               }
           }
       } else {
           additional_scache_size += 2 * _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE;
       }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
            if (recovered_ver >= BCM_WB_VERSION_1_2) {
                BCM_IF_ERROR_RETURN(_bcm_td2_qos_extended_reinit(unit, &scache_ptr));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_td2_qos_reinit_scache_len_get(unit,
                                        &additional_scache_size));
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
            if (recovered_ver >= BCM_WB_VERSION_1_2) {
                BCM_IF_ERROR_RETURN(_bcm_th_qos_extended_reinit(unit, &scache_ptr));
            } else {
                BCM_IF_ERROR_RETURN(
                    _bcm_th_qos_reinit_scache_len_get(unit,
                                                      &additional_scache_size));
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)) {
            if ((SOC_IS_METROLITE(unit) && (recovered_ver >=  BCM_WB_VERSION_1_3)) ||
                    (recovered_ver >=  BCM_WB_VERSION_1_4)) {
                for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
                    hw_idx = (uint32) (*scache_ptr++);
                    if (hw_idx != 0) {
                        _BCM_QOS_EGR_MPLS_EXTERNAL_USED_SET(unit, idx);
                        QOS_INFO(unit)->egr_mpls_external_idx_ref_count[idx] = hw_idx;
                    }
                }
            } else {
                /* egress profile external usage ref count */
                additional_scache_size += (sizeof(uint32) * _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
            }
        }
#endif /*  defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT) */

        if (additional_scache_size > 0) {
            /* Need to realloc unavailable space for subsequent syncs */
            SOC_IF_ERROR_RETURN
                (soc_scache_realloc(unit, handle, additional_scache_size));
        }
    } else {
        /* During cold-boot. Allocate a stable cache if not already done */
        rv = _bcm_esw_scache_ptr_get(unit, handle, TRUE, scache_len,
                                     &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_qos_reinit_hw_profiles_update
 * Purpose:
 *      Updates the shared memory profile tables reference counts
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_qos_reinit_hw_profiles_update (int unit)
{
    int     i;

    for (i=0; i<_BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; i++) {
        if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_ing_pri_cng_map_entry_reference(unit,
                              ((QOS_INFO(unit)->pri_cng_hw_idx[i]) *
                               _BCM_QOS_MAP_CHUNK_PRI_CNG),
                               _BCM_QOS_MAP_CHUNK_PRI_CNG));
        }
    }
    for (i=0; i<_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; i++) {
        if (_BCM_QOS_EGR_MPLS_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_egr_mpls_combo_map_entry_reference(unit,
                             ((QOS_INFO(unit)->egr_mpls_hw_idx[i]) *
                              _BCM_QOS_MAP_CHUNK_EGR_MPLS),
                             _BCM_QOS_MAP_CHUNK_EGR_MPLS));
        }
    }
    for (i=0; i<_BCM_QOS_MAP_LEN_DSCP_TABLE; i++) {
        /* For all port dscp profiles have been reserved on TD,
         * should skip this operation during warmboot
         */
        if (SOC_IS_TRIDENT(unit) &&
            ((i >= 0) && (i < SOC_PORT_MAX(unit,all)))) {
            continue;
        }
        if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_dscp_table_entry_reference(unit,
                                 ((QOS_INFO(unit)->dscp_hw_idx[i]) *
                                  _BCM_QOS_MAP_CHUNK_DSCP),
                                 _BCM_QOS_MAP_CHUNK_DSCP));
        }
    }
    for (i=0; i<_BCM_QOS_MAP_LEN_EGR_DSCP_TABLE; i++) {
        if (_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, i)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                BCM_IF_ERROR_RETURN(_bcm_egr_qos_profile_mem_reference(unit,
                                     ((QOS_INFO(unit)->egr_dscp_hw_idx[i]) *
                                      _BCM_QOS_MAP_CHUNK_EGR_DSCP),
                                     _BCM_QOS_MAP_CHUNK_EGR_DSCP));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(_bcm_egr_dscp_table_entry_reference(unit,
                                     ((QOS_INFO(unit)->egr_dscp_hw_idx[i]) *
                                      _BCM_QOS_MAP_CHUNK_EGR_DSCP),
                                     _BCM_QOS_MAP_CHUNK_EGR_DSCP));
            }
        }
    }
#ifdef BCM_KATANA_SUPPORT
    for (i=0; i< _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE; i++) {
        if (_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_offset_map_table_entry_reference(unit,
                                 ((QOS_INFO(unit)->offset_map_hw_idx[i]) *
                                  _BCM_QOS_MAP_CHUNK_OFFSET_MAP),
                                 _BCM_QOS_MAP_CHUNK_OFFSET_MAP));
        }
    }

#endif

#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        for (i=0; i< _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE; i++) {
            if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, i)) {
                BCM_IF_ERROR_RETURN(_bcm_service_pri_map_table_entry_reference(unit,
                            ((QOS_INFO(unit)->service_pri_map_hw_idx[i]) *
                             _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP),
                            _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP));
            }
        }
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_qos_reinit_hw_profiles_update(unit));
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th_qos_reinit_hw_profiles_update(unit));
    }
#endif

    return BCM_E_NONE;
}

int
_bcm_tr2_qos_reinit_profiles_ref_update (int unit)
{
    int i;
    int rv;
    int hw_idx;

    for (i = 0; i < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; i++) {
        if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, i)) {
            hw_idx = QOS_INFO(unit)->pri_cng_hw_idx[i];
            if (hw_idx == 0) {
                continue;
            }

            do {
                rv = _bcm_ing_pri_cng_map_entry_delete(
                    unit, hw_idx * _BCM_QOS_MAP_CHUNK_PRI_CNG);

                if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
                    return rv;
                }
            } while (rv == BCM_E_NONE);
        }
    }

    for (i = 0; i < _BCM_QOS_MAP_LEN_DSCP_TABLE; i++) {
        if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, i)) {
            hw_idx = QOS_INFO(unit)->dscp_hw_idx[i];
            if (hw_idx == 0) {
                continue;
            }

            do {
                rv = _bcm_dscp_table_entry_delete(
                    unit, hw_idx * _BCM_QOS_MAP_CHUNK_DSCP);

                if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
                    return rv;
                }
            } while (rv == BCM_E_NONE);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_qos_reinit
 * Purpose:
 *      Top level QOS init routine for Warm-Boot
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_qos_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int stable_size = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (stable_size == 0) {
        if (!SOC_WARM_BOOT(unit)) {
            return rv; /* do nothing in cold-boot */
        }
        rv = _bcm_tr2_qos_unsynchronized_reinit(unit);
    } else {
        /* Limited is same as extended */
        rv = _bcm_tr2_qos_extended_reinit(unit);
    }

    if (SOC_IS_TD2_TT2(unit)) {
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_tr2_qos_reinit_profiles_ref_update(unit);
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_tr2_qos_reinit_hw_profiles_update(unit);
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr2_qos_sync
 * Purpose:
 *      This routine extracts the state that needs to be stored from the
 *      book-keeping structs and stores it in the allocated scache location
 * Compression format:  0   - A     pri_cng_hw_idx table
 *                      A+1 - B     egr_mpls_hw_idx table
 *                      B+1 - C     dscp_hw_idx table
 *                      C+1 - D     egr_dscp_hw_idx table
 *                      D+1 - E     egr_mpls flags
 *                      E+1 - F     ing_mpls_exp table usage by QOS module
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_qos_sync(int unit)
{
    soc_scache_handle_t handle;
    uint8               *scache_ptr;
    uint32              hw_idx, scache_len;
    int                 idx;
#ifdef BCM_KATANA2_SUPPORT
    uint32              refcount;
#endif
    QOS_INIT(unit);
    BCM_IF_ERROR_RETURN(_bcm_tr2_qos_reinit_scache_len_get(unit, &scache_len));
    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_QOS, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, handle, FALSE,
                                                scache_len, &scache_ptr,
                                                BCM_WB_DEFAULT_VERSION, NULL));

    /* now store the state into the compressed format */
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; idx++) {
        if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->pri_cng_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
        if (_BCM_QOS_EGR_MPLS_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->egr_mpls_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_DSCP_TABLE; idx++) {
        if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->dscp_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE; idx++) {
        if (_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->egr_dscp_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    sal_memcpy(scache_ptr, QOS_INFO(unit)->egr_mpls_bitmap_flags,
               SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS));
    scache_ptr += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
    sal_memcpy(scache_ptr, QOS_INFO(unit)->ing_mpls_exp_bitmap,
               SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP));
    scache_ptr += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP);
#ifdef BCM_KATANA_SUPPORT
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE; idx++) {
        if (_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->offset_map_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE; idx++) {
            if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, idx)) {
                hw_idx = QOS_INFO(unit)->service_pri_map_hw_idx[idx];
            } else {
                hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
            }
            *scache_ptr++ = hw_idx;
        }
    }
#endif
   /* Replication queue offset profile Map table
    * RQE_QUEUE_OFFSETr
    * store the valid hardware indices
    * store the reference count per profile
    */
#ifdef BCM_KATANA2_SUPPORT
   for (idx = 0; idx < _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP; idx++) {
       if (_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, idx)) {
           hw_idx = QOS_INFO(unit)->rqe_queue_offset_hw_idx[idx];
           refcount = QOS_INFO(unit)->rqe_queue_offset_refcount[idx];
       } else {
           hw_idx = _BCM_TR2_REINIT_INVALID_HW_IDX;
           refcount = 0;
       }
       *scache_ptr++ = hw_idx;
       *scache_ptr++ = refcount;
   }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_qos_sync(unit, &scache_ptr));
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th_qos_sync(unit, &scache_ptr));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)) {
        for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
            if (_BCM_QOS_EGR_MPLS_EXTERNAL_USED_GET(unit, idx)) {
                hw_idx = QOS_INFO(unit)->egr_mpls_external_idx_ref_count[idx];
            } else {
                hw_idx = 0;
            }
            *scache_ptr++ = hw_idx;
        }
    }
#endif
    return BCM_E_NONE;
}
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
STATIC int
_bcm_th3_qos_validate_map_type(int unit, int map_id)
{
    int id;
    int map_size;

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
        case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
            map_size = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
            break;

        case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
            map_size = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
            break;

        case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
            map_size = _BCM_QOS_MAP_LEN_DSCP_TABLE;
            break;

        case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
            map_size = _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
            break;

        default:
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
                id = map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK;
                map_size = _bcm_tr_get_ing_mpls_index_size(unit);
            } else
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
            {
                return BCM_E_PARAM;
            }
    }

    return (id >= map_size) ? BCM_E_PARAM : BCM_E_NONE;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

STATIC void
_bcm_tr2_qos_free_resources(int unit)
{
    _bcm_tr2_qos_bookkeeping_t *qos_info = QOS_INFO(unit);

    if (!qos_info) {
    /* coverity[dead_error_line] */
        return;
    }

    /* Destroy ingress profile bitmap */
    if (qos_info->ing_pri_cng_bitmap) {
        sal_free(qos_info->ing_pri_cng_bitmap);
        qos_info->ing_pri_cng_bitmap = NULL;
    }
    if (qos_info->pri_cng_hw_idx) {
        sal_free(qos_info->pri_cng_hw_idx);
        qos_info->pri_cng_hw_idx = NULL;
    }

    if (qos_info->egr_mpls_hw_idx) {
        int id;
        /* Delete all combo mpls hw index profiles */
        for (id = 0; id < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; id++)
        {
            if (qos_info->egr_mpls_hw_idx[id] != 0) {
                _bcm_egr_mpls_combo_map_entry_delete (unit,
                                           (qos_info->egr_mpls_hw_idx[id] *
                                            _BCM_QOS_MAP_CHUNK_EGR_MPLS));
                qos_info->egr_mpls_hw_idx[id] = 0;
                _BCM_QOS_EGR_MPLS_USED_CLR(unit, id);
                _BCM_QOS_EGR_MPLS_FLAGS_USED_CLR(unit, id);
            }
        }
        sal_free(qos_info->egr_mpls_hw_idx);
        qos_info->egr_mpls_hw_idx = NULL;
    }

    /* Destroy egress profile usage bitmap */
    if (qos_info->egr_mpls_bitmap) {
        sal_free(qos_info->egr_mpls_bitmap);
        qos_info->egr_mpls_bitmap = NULL;
    }
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)
        && qos_info->egr_mpls_external_idx_ref_count) {
        int id;
        /* Delete all combo mpls hw index profiles */
        for (id = 0; id < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; id++)
        {
            if (qos_info->egr_mpls_external_idx_ref_count[id] != 0) {
                qos_info->egr_mpls_external_idx_ref_count[id] = 0;
                _BCM_QOS_EGR_MPLS_EXTERNAL_USED_CLR(unit, id);
            }
        }
        sal_free(qos_info->egr_mpls_external_idx_ref_count);
        qos_info->egr_mpls_external_idx_ref_count = NULL;
    }

    /* Destroy egress profile usage bitmap */
    if (qos_info->egr_mpls_external_bitmap) {
        sal_free(qos_info->egr_mpls_external_bitmap);
        qos_info->egr_mpls_external_bitmap = NULL;
    }

#endif

    /* Destroy DSCP table profile usage bitmap */
    if (qos_info->dscp_table_bitmap) {
        sal_free(qos_info->dscp_table_bitmap);
        qos_info->dscp_table_bitmap = NULL;
    }
    if (qos_info->dscp_hw_idx) {
        sal_free(qos_info->dscp_hw_idx);
        qos_info->dscp_hw_idx = NULL;
    }

    /* Destroy egress DSCP table profile usage bitmap */
    if (qos_info->egr_dscp_table_bitmap) {
        sal_free(qos_info->egr_dscp_table_bitmap);
        qos_info->egr_dscp_table_bitmap = NULL;
    }
    if (qos_info->egr_dscp_hw_idx) {
        sal_free(qos_info->egr_dscp_hw_idx);
        qos_info->egr_dscp_hw_idx = NULL;
    }

    /* Destroy egress profile flags bitmap */
    if (qos_info->egr_mpls_bitmap_flags) {
        sal_free(qos_info->egr_mpls_bitmap_flags);
        qos_info->egr_mpls_bitmap_flags = NULL;
    }

    /* Destroy ing_mpls_exp bitmap */
    if (qos_info->ing_mpls_exp_bitmap) {
        sal_free(qos_info->ing_mpls_exp_bitmap);
        qos_info->ing_mpls_exp_bitmap = NULL;
    }
#ifdef BCM_KATANA_SUPPORT
/* Destroy queue map table profile usage bitmap */
    if (qos_info->offset_map_table_bitmap) {
        sal_free(qos_info->offset_map_table_bitmap);
        qos_info->offset_map_table_bitmap = NULL;
    }
    if (qos_info->offset_map_hw_idx) {
        sal_free(qos_info->offset_map_hw_idx);
        qos_info->offset_map_hw_idx = NULL;
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        /* Destroy service pri map table profile usage bitmap */
        if (qos_info->service_pri_map_table_bitmap) {
            sal_free(qos_info->service_pri_map_table_bitmap);
            qos_info->service_pri_map_table_bitmap = NULL;
        }
        if (qos_info->service_pri_map_hw_idx) {
            sal_free(qos_info->service_pri_map_hw_idx);
            qos_info->service_pri_map_hw_idx = NULL;
        }
        if (qos_info->service_pri_map_flags) {
            sal_free(qos_info->service_pri_map_flags);
            qos_info->service_pri_map_flags = NULL;
        }
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
/* Destroy RQE QUEUE MAP table profile usage bitmap */
    if (qos_info->rqe_queue_offset_bitmap) {
        sal_free(qos_info->rqe_queue_offset_bitmap);
        qos_info->rqe_queue_offset_bitmap = NULL;
    }
    if (qos_info->rqe_queue_offset_hw_idx) {
        sal_free(qos_info->rqe_queue_offset_hw_idx);
        qos_info->rqe_queue_offset_hw_idx = NULL;
    }
    if (qos_info->rqe_queue_offset_refcount) {
        sal_free(qos_info->rqe_queue_offset_refcount);
        qos_info->rqe_queue_offset_refcount = NULL;
    }
#endif

    /* Destroy the mutex */
    if (_tr2_qos_mutex[unit]) {
        sal_mutex_destroy(_tr2_qos_mutex[unit]);
        _tr2_qos_mutex[unit] = NULL;
    }
}

/* Initialize the QoS module. */
int
bcm_tr2_qos_init(int unit)
{
    _bcm_tr2_qos_bookkeeping_t *qos_info = QOS_INFO(unit);
    int ing_profiles, egr_profiles = 0, rv = BCM_E_NONE;
    int dscp_profiles, egr_dscp_profiles, ing_mpls_profiles;
#ifdef BCM_KATANA_SUPPORT
    int offset_map_profiles = 0;
#endif
#ifdef BCM_SABER2_SUPPORT
    int service_pri_map_profiles = 0, pid, index;
    ing_pri_cng_map_entry_t pri_map[_BCM_QOS_MAP_CHUNK_PRI_CNG_MAX];
    ing_untagged_phb_entry_t phb;
    void *entries[2];
#ifdef BCM_WARM_BOOT_SUPPORT
    bcm_port_t port;
#endif
#endif
#ifdef BCM_KATANA2_SUPPORT
    int rqe_queue_offset_profiles = 0;
#endif

    if (SOC_IS_TRIDENT3X(unit)) {
        qos_info->inf_pri_cng_mapping = PHB_MAPPING_TBL_1m;
        qos_info->dscp_table = PHB_MAPPING_TBL_2m;
        qos_info->ing_mpls_exp_mapping = PHB_MAPPING_TBL_3m;
        qos_info->egr_dscp_table = EGR_ZONE_4_QOS_MAPPING_TABLEm;
        qos_info->egr_dscp_table_outer = EGR_ZONE_2_QOS_MAPPING_TABLEm;
        qos_info->pri_cng_chunk_size = 64;
    } else {
        qos_info->inf_pri_cng_mapping = ING_PRI_CNG_MAPm;
        qos_info->dscp_table = DSCP_TABLEm;
        qos_info->egr_dscp_table = EGR_DSCP_TABLEm;
        qos_info->ing_mpls_exp_mapping = ING_MPLS_EXP_MAPPINGm;
        qos_info->pri_cng_chunk_size = 16;
    }

   ing_profiles = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
   dscp_profiles = _BCM_QOS_MAP_LEN_DSCP_TABLE;
   egr_profiles = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
   egr_dscp_profiles = _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
   ing_mpls_profiles = _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP;

#ifdef BCM_KATANA_SUPPORT
    offset_map_profiles = _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE;
#endif
#ifdef BCM_KATANA2_SUPPORT
    rqe_queue_offset_profiles = _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP;
#endif
#ifdef BCM_SABER2_SUPPORT
    service_pri_map_profiles = _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE;
#endif

    if (tr2_qos_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_tr2_qos_detach(unit));
    }

    /* Create mutex */
    if (NULL == _tr2_qos_mutex[unit]) {
        _tr2_qos_mutex[unit] = sal_mutex_create("qos mutex");
        if (_tr2_qos_mutex[unit] == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    /* Allocate ingress profile usage bitmap */
    if (NULL == qos_info->ing_pri_cng_bitmap) {
        qos_info->ing_pri_cng_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(ing_profiles), "ing_pri_cng_bitmap");
        if (qos_info->ing_pri_cng_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->ing_pri_cng_bitmap, 0, SHR_BITALLOCSIZE(ing_profiles));
    if (NULL == qos_info->pri_cng_hw_idx) {
        qos_info->pri_cng_hw_idx =
            sal_alloc(sizeof(uint32) * ing_profiles, "pri_cng_hw_idx");
        if (qos_info->pri_cng_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->pri_cng_hw_idx, 0, (sizeof(uint32) * ing_profiles));

    /* Allocate egress profile usage bitmap */
    if (NULL == qos_info->egr_mpls_bitmap) {
        qos_info->egr_mpls_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(egr_profiles), "egr_mpls_bitmap");
        if (qos_info->egr_mpls_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_mpls_bitmap, 0, SHR_BITALLOCSIZE(egr_profiles));
    if (NULL == qos_info->egr_mpls_hw_idx) {
        qos_info->egr_mpls_hw_idx =
        sal_alloc(sizeof(uint32) * egr_profiles, "egr_mpls_hw_idx");
        if (qos_info->egr_mpls_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_mpls_hw_idx, 0, (sizeof(uint32) * egr_profiles));
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)) {
        /* Allocate egress profile external usage bitmap */
        if (NULL == qos_info->egr_mpls_external_bitmap) {
            qos_info->egr_mpls_external_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(egr_profiles), "egr_mpls_external_bitmap");
            if (qos_info->egr_mpls_external_bitmap == NULL) {
                _bcm_tr2_qos_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(qos_info->egr_mpls_external_bitmap, 0, SHR_BITALLOCSIZE(egr_profiles));
        if (NULL == qos_info->egr_mpls_external_idx_ref_count) {
            qos_info->egr_mpls_external_idx_ref_count =
                sal_alloc(sizeof(uint32) * egr_profiles, "egr_mpls_external_idx_ref_count");
            if (qos_info->egr_mpls_external_idx_ref_count == NULL) {
                _bcm_tr2_qos_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(qos_info->egr_mpls_external_idx_ref_count, 0, (sizeof(uint32) * egr_profiles));
    }
#endif
    /* Allocate DSCP_TABLE profile usage bitmap */
    if (NULL == qos_info->dscp_table_bitmap) {
        qos_info->dscp_table_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(dscp_profiles), "dscp_table_bitmap");
        if (qos_info->dscp_table_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->dscp_table_bitmap, 0, SHR_BITALLOCSIZE(dscp_profiles));
    if (SOC_IS_TRIDENT(unit)) {
        int i;
        for (i = 0; i < SOC_PORT_MAX(unit,all); i++) {
            SHR_BITSET(qos_info->dscp_table_bitmap, i);
        }
    }

    if (NULL == qos_info->dscp_hw_idx) {
        qos_info->dscp_hw_idx =
            sal_alloc(sizeof(uint32) * dscp_profiles, "dscp_hw_idx");
        if (qos_info->dscp_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->dscp_hw_idx, 0, (sizeof(uint32) * dscp_profiles));

    /* Allocate EGR_DSCP_TABLE profile usage bitmap */
    if (NULL == qos_info->egr_dscp_table_bitmap) {
        qos_info->egr_dscp_table_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(egr_dscp_profiles), "egr_dscp_table_bitmap");
        if (qos_info->egr_dscp_table_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_dscp_table_bitmap, 0,
               SHR_BITALLOCSIZE(egr_dscp_profiles));
    if (NULL == qos_info->egr_dscp_hw_idx) {
        qos_info->egr_dscp_hw_idx =
            sal_alloc(sizeof(uint32) * egr_dscp_profiles, "egr_dscp_hw_idx");
        if (qos_info->egr_dscp_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_dscp_hw_idx, 0, (sizeof(uint32) * egr_dscp_profiles));

    /* Allocate egress profile usage bitmap */
    if (NULL == qos_info->egr_mpls_bitmap_flags) {
        qos_info->egr_mpls_bitmap_flags =
        sal_alloc(SHR_BITALLOCSIZE(egr_profiles), "egr_mpls_bitmap_flags");
        if (qos_info->egr_mpls_bitmap_flags == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_mpls_bitmap_flags, 0, SHR_BITALLOCSIZE(egr_profiles));

    /* Allocate ingress mpls profile usage bitmap */
    if (NULL == qos_info->ing_mpls_exp_bitmap) {
        qos_info->ing_mpls_exp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(ing_mpls_profiles), "ing_mpls_exp_bitmap");
        if (qos_info->ing_mpls_exp_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->ing_mpls_exp_bitmap, 0, SHR_BITALLOCSIZE(ing_mpls_profiles));

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* For TH3 skip further initializations */
    } else
#endif/* BCM_TOMAHAWK3_SUPPORT */
    {
#ifdef BCM_KATANA_SUPPORT
     /* Allocate queue offset map usage bitmap */
    if (NULL == qos_info->offset_map_table_bitmap) {
        qos_info->offset_map_table_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(offset_map_profiles),
                                          "offset_map_table_bitmap");
        if (qos_info->offset_map_table_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->offset_map_table_bitmap, 0,
                                    SHR_BITALLOCSIZE(offset_map_profiles));
    if (NULL == qos_info->offset_map_hw_idx) {
        qos_info->offset_map_hw_idx =
               sal_alloc(sizeof(uint32) * offset_map_profiles,
                         "offset_map_hw_idx");
        if (qos_info->offset_map_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->offset_map_hw_idx, 0,
                              (sizeof(uint32) * offset_map_profiles));
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        /* Allocate queue offset map usage bitmap */
        if (NULL == qos_info->service_pri_map_table_bitmap) {
            qos_info->service_pri_map_table_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(service_pri_map_profiles),
                        "service_pri_map_table_bitmap");
            if (qos_info->service_pri_map_table_bitmap == NULL) {
                _bcm_tr2_qos_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(qos_info->service_pri_map_table_bitmap, 0,
                SHR_BITALLOCSIZE(service_pri_map_profiles));
        if (NULL == qos_info->service_pri_map_hw_idx) {
            qos_info->service_pri_map_hw_idx =
                sal_alloc(sizeof(uint32) * service_pri_map_profiles,
                        "service_pri_map_hw_idx");
            if (qos_info->service_pri_map_hw_idx == NULL) {
                _bcm_tr2_qos_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(qos_info->service_pri_map_hw_idx, 0,
                (sizeof(uint32) * service_pri_map_profiles));

        if (NULL == qos_info->service_pri_map_flags) {
            qos_info->service_pri_map_flags =
                sal_alloc(sizeof(uint32) * service_pri_map_profiles,
                        "service_pri_map_flags");
            if (qos_info->service_pri_map_flags == NULL) {
                _bcm_tr2_qos_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(qos_info->service_pri_map_flags, 0,
                (sizeof(uint32) * service_pri_map_profiles));
    }
    /* On Saber2 we need to reserve one more profile for dynamic update */
    if (SOC_IS_SABER2(unit)) {
        for (pid = 0; pid < MAX_QOS_ING_PRI_PROFILE; pid++) {
             QOS_INFO(unit)->qos_profile_binding[unit][pid] = 0;
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            for (port = SB2_CMIC + 1; port < SB2_LPBK; port++) {
                 rv = _bcm_tr2_port_tab_get(unit, port,
                                            TRUST_DOT1P_PTRf, &index);
                 if (BCM_FAILURE(rv)) {
                     return rv;
                 }
                 _BCM_QOS_TRUST_DOT1P_PTR_CACHE_SET(unit, port, index);
            }
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
        sal_memset(pri_map, 0, sizeof(ing_pri_cng_map_entry_t) *
                                      _BCM_QOS_MAP_CHUNK_PRI_CNG);
        sal_memset(&phb, 0, sizeof(phb));
        entries[0] = &pri_map;
        entries[1] = &phb;

        for (index = 0; index < _BCM_QOS_MAP_CHUNK_PRI_CNG; index ++) {
             soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map[index], PRIf,
                                 7);
             soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map[index],
                                 CNGf, 1);
        }
        BCM_IF_ERROR_RETURN(_bcm_ing_pri_cng_map_entry_add(unit, entries,
                                            _BCM_QOS_MAP_CHUNK_PRI_CNG,
                                                   (uint32 *)&index));
        QOS_INFO(unit)->pri_cng_hw_idx_reserved = index;
    }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
     /* Allocate RQE_QUEUE_OFFSET map usage bitmap */
    if (NULL == qos_info->rqe_queue_offset_bitmap) {
        qos_info->rqe_queue_offset_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(rqe_queue_offset_profiles),
                                          "rqe_queue_offset_bitmap");
        if (qos_info->rqe_queue_offset_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->rqe_queue_offset_bitmap, 0,
                                    SHR_BITALLOCSIZE(rqe_queue_offset_profiles));
    if (NULL == qos_info->rqe_queue_offset_hw_idx) {
        qos_info->rqe_queue_offset_hw_idx =
               sal_alloc(sizeof(uint32) * rqe_queue_offset_profiles,
                         "rqe_queue_offset_hw_idx");
        if (qos_info->rqe_queue_offset_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->rqe_queue_offset_hw_idx, 0,
                              (sizeof(uint32) * rqe_queue_offset_profiles));

    if (NULL == qos_info->rqe_queue_offset_refcount) {
        qos_info->rqe_queue_offset_refcount =
               sal_alloc(sizeof(uint32) * rqe_queue_offset_profiles,
                         "rqe_queue_offset_refcount");
        if (qos_info->rqe_queue_offset_refcount == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->rqe_queue_offset_refcount, 0,
                              (sizeof(uint32)* rqe_queue_offset_profiles));
#endif /* BCM_KATANA_SUPPORT */
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_tr2_qos_reinit(unit);
    if (SOC_FAILURE(rv)) {
        _bcm_tr2_qos_free_resources(unit);
        return rv;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    tr2_qos_initialized[unit] = TRUE;
    return rv;
}

/* Detach the QoS module. */
int
bcm_tr2_qos_detach(int unit)
{
    int rv = BCM_E_NONE;
    _bcm_tr2_qos_free_resources(unit);
    tr2_qos_initialized[unit] = FALSE;
    return rv;
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
STATIC int
_bcm_td2p_egr_qos_combo_map_create(int unit, uint32 flags, int *map_id)
{
    egr_qos_l2_map_entry_t  egr_pri_cng_map[_BCM_QOS_MAP_CHUNK_EGR_DSCP];
    egr_qos_l3_map_entry_t egr_dscp_table[_BCM_QOS_MAP_CHUNK_EGR_DSCP];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[2];

    /* Check for pre-specified ID */
    id = *map_id & _BCM_QOS_MAP_TYPE_MASK;

    /* Reserve a chunk in the EGR_DSCP_TABLE */
    sal_memset(egr_pri_cng_map, 0, sizeof(egr_pri_cng_map));
    entries[0] = &egr_pri_cng_map;
    sal_memset(egr_dscp_table, 0, sizeof(egr_dscp_table));
    entries[1] = &egr_dscp_table;
    BCM_IF_ERROR_RETURN(_bcm_egr_qos_profile_entry_add(unit, entries,
                                                        _BCM_QOS_MAP_CHUNK_EGR_DSCP,
                                                               (uint32 *)&index));

    QOS_INFO(unit)->egr_dscp_hw_idx[id] = (index /
                                           _BCM_QOS_MAP_CHUNK_EGR_DSCP);
    return rv;
}

STATIC int
_bcm_td2p_egr_qos_combo_map_destroy(int unit, int map_id)
{
    int id, rv = BCM_E_UNAVAIL;

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
        return BCM_E_NOT_FOUND;
    } else {
        rv = _bcm_egr_qos_profile_entry_delete
                (unit, (QOS_INFO(unit)->egr_dscp_hw_idx[id] *
                 _BCM_QOS_MAP_CHUNK_EGR_DSCP));
        if (BCM_E_NONE == rv) {
            QOS_INFO(unit)->egr_dscp_hw_idx[id] = 0;
            _BCM_QOS_EGR_DSCP_TABLE_USED_CLR(unit, id);
        }
    }
    return rv;
}

STATIC int
_bcm_td2p_egr_qos_combo_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int alloc_size, offset, cng, id, index, rv = BCM_E_NONE;
    uint8 *egr_pri_cng_map;
    uint8 *egr_dscp_table;
    void *entries[3], *entry;
    soc_mem_t qos_l2_map_mem;
    soc_mem_t qos_l3_map_mem;

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    /* Assign memory ID based on chip */
    if (SOC_IS_TRIDENT3X(unit)) {
        qos_l2_map_mem = EGR_ZONE_1_DOT1P_MAPPING_TABLE_4m;
        qos_l3_map_mem = EGR_ZONE_4_QOS_MAPPING_TABLEm;
    } else {
        qos_l2_map_mem = EGR_PRI_CNG_MAPm;
        qos_l3_map_mem = EGR_DSCP_TABLEm;
    }

    /* Allocate memory for DMA */
    alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_DSCP *
                  soc_mem_entry_words(unit, qos_l2_map_mem) * sizeof(uint32));

    egr_pri_cng_map = soc_cm_salloc(unit, alloc_size,
                                "TD2PLUS egr_pri_cng table");
    if (NULL == egr_pri_cng_map) {
     return (BCM_E_MEMORY);
    }
    sal_memset(egr_pri_cng_map, 0, alloc_size);

    alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_DSCP *
                  soc_mem_entry_words(unit, qos_l3_map_mem) * sizeof(uint32));
    egr_dscp_table = soc_cm_salloc(unit, alloc_size,
                                   "TD2PLUS egr_dscp table");
    if (NULL == egr_dscp_table) {
        soc_cm_sfree(unit, egr_pri_cng_map);
        return (BCM_E_MEMORY);
    }
    sal_memset(egr_dscp_table, 0, alloc_size);

    /* Base index of table in hardware */
    index = (QOS_INFO(unit)->egr_dscp_hw_idx[id] *
             _BCM_QOS_MAP_CHUNK_EGR_DSCP);

    /* Offset within table */
    cng = _BCM_COLOR_ENCODING(unit, map->color);
    offset = (map->int_pri << 2) | cng;

    /* Read the old profile chunk */
    rv = soc_mem_read_range(unit, qos_l2_map_mem, MEM_BLOCK_ANY,
                           index,
                           index + (_BCM_QOS_MAP_CHUNK_EGR_DSCP -1),
                           egr_pri_cng_map);
    if (BCM_FAILURE(rv)) {
       soc_cm_sfree(unit, egr_dscp_table);
       soc_cm_sfree(unit, egr_pri_cng_map);
       return (rv);
    }

    rv = soc_mem_read_range(unit, qos_l3_map_mem, MEM_BLOCK_ANY,
                            index,
                            index + (_BCM_QOS_MAP_CHUNK_EGR_DSCP -1),
                            egr_dscp_table);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, egr_dscp_table);
        soc_cm_sfree(unit, egr_pri_cng_map);
        return (rv);
    }

    /* Modify what's needed */
    entry = (void *)(egr_pri_cng_map +
                     soc_mem_entry_words(unit, qos_l2_map_mem) * sizeof(uint32) * offset);
    soc_mem_field32_set(unit, qos_l2_map_mem, (uint32 *)entry, PRIf, map->pkt_pri);
    soc_mem_field32_set(unit, qos_l2_map_mem, (uint32 *)entry, CFIf, map->pkt_cfi);

    entry = (void *)(egr_dscp_table +
                     soc_mem_entry_words(unit, qos_l3_map_mem) * sizeof(uint32) * offset);
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, qos_l3_map_mem, (uint32 *)entry, QOSf, map->dscp);
    } else {
        soc_mem_field32_set(unit, qos_l3_map_mem, (uint32 *)entry, DSCPf, map->dscp);
    }
    /* Delete the old profile chunk */
    rv = _bcm_egr_qos_profile_entry_delete(unit, index);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, egr_dscp_table);
        soc_cm_sfree(unit, egr_pri_cng_map);
        return (rv);
    }

    /* Add new chunk and store new HW index */
    entries[0] = egr_pri_cng_map;
    entries[1] = egr_dscp_table;
    if (SOC_IS_TRIDENT3X(unit)) {
        entries[2] = egr_dscp_table; /* same for Zone2 dscp */
    }
    rv = _bcm_egr_qos_profile_entry_add(unit, entries,
                                       _BCM_QOS_MAP_CHUNK_EGR_DSCP,
                                       (uint32 *)&index);

    QOS_INFO(unit)->egr_dscp_hw_idx[id] = (index /
                                          _BCM_QOS_MAP_CHUNK_EGR_DSCP);

    /* Free the DMA memory */
    soc_cm_sfree(unit, egr_dscp_table);
    soc_cm_sfree(unit, egr_pri_cng_map);

    return rv;
}
#endif

STATIC int
_bcm_tr2_qos_l2_map_create(int unit, uint32 flags, int *map_id)
{
    ing_pri_cng_map_entry_t ing_pri_map[_BCM_QOS_MAP_CHUNK_PRI_CNG_MAX];
    ing_untagged_phb_entry_t ing_untagged_phb;
    egr_mpls_pri_mapping_entry_t egr_mpls_pri_map[_BCM_QOS_MAP_CHUNK_EGR_MPLS];
    egr_mpls_exp_mapping_1_entry_t egr_mpls_exp_map[_BCM_QOS_MAP_CHUNK_EGR_MPLS];
    egr_mpls_exp_mapping_2_entry_t egr_mpls_exp_map2[_BCM_QOS_MAP_CHUNK_EGR_MPLS];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[3];

    /* Check parameters */
    if (NULL == map_id) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_QOS_MAP_INGRESS) {
        /* Check for pre-specified ID */
        if (flags & BCM_QOS_MAP_WITH_ID) {
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
            if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
                _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                return BCM_E_BADID;
            }
            if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                return BCM_E_PARAM;
            }
            if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                if (!(flags & BCM_QOS_MAP_REPLACE)) {
                    return BCM_E_EXISTS;
                }
            } else {
                _BCM_QOS_ING_PRI_CNG_USED_SET(unit, id);
            }
        } else {
            id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->ing_pri_cng_bitmap,
                                   _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_ING_PRI_CNG_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP
                      << _BCM_QOS_MAP_SHIFT);
        }
        /* Reserve a chunk in the ING_PRI_CNG_MAP table */
        sal_memset(ing_pri_map, 0, sizeof(ing_pri_map));
        sal_memset(&ing_untagged_phb, 0, sizeof(ing_untagged_phb));
        entries[0] = &ing_pri_map;
        entries[1] = &ing_untagged_phb;
        BCM_IF_ERROR_RETURN(_bcm_ing_pri_cng_map_entry_add(unit, entries,
                                            _BCM_QOS_MAP_CHUNK_PRI_CNG,
                                                           (uint32 *)&index));
        QOS_INFO(unit)->pri_cng_hw_idx[id] = (index /
                                              _BCM_QOS_MAP_CHUNK_PRI_CNG);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* Check for pre-specified ID */
        if (flags & BCM_QOS_MAP_WITH_ID) {
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
            if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
                _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                return BCM_E_BADID;
            }
            if (_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                if (!(flags & BCM_QOS_MAP_REPLACE)) {
                    return BCM_E_EXISTS;
                }
            }
        } else {
            id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->egr_mpls_bitmap,
                                   _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS
                      << _BCM_QOS_MAP_SHIFT);
        }
        /* Reserve a chunk in the EGR_MPLS_EXP_MAPPING_1/EGR_MPLS_PRI_MAPPING
           tables - these two tables always done together */

        if (SOC_IS_TRIDENT3X(unit)) {
#ifdef BCM_TRIDENT3_SUPPORT
            rv = bcm_td3_qos_map_create(unit, (uint32 *)&index, _BCM_QOS_MAP_CHUNK_EGR_MPLS);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
#endif
        } else {
            sal_memset(egr_mpls_pri_map, 0, sizeof(egr_mpls_pri_map));
            entries[0] = egr_mpls_pri_map;
            sal_memset(egr_mpls_exp_map, 0, sizeof(egr_mpls_exp_map));
            entries[1] = egr_mpls_exp_map;
            sal_memset(egr_mpls_exp_map2, 0, sizeof(egr_mpls_exp_map2));
            entries[2] = egr_mpls_exp_map2;
            BCM_IF_ERROR_RETURN(_bcm_egr_mpls_combo_map_entry_add(unit, entries,
                                                   _BCM_QOS_MAP_CHUNK_EGR_MPLS,
                                                              (uint32 *)&index));
        }
        QOS_INFO(unit)->egr_mpls_hw_idx[id] = (index /
                                               _BCM_QOS_MAP_CHUNK_EGR_MPLS);
        _BCM_QOS_EGR_MPLS_USED_SET(unit, id);
    }
    return rv;
}

STATIC int
_bcm_tr2_qos_l3_map_create(int unit, uint32 flags, int *map_id)
{
    dscp_table_entry_t dscp_table[_BCM_QOS_MAP_CHUNK_DSCP];
    egr_dscp_table_entry_t egr_dscp_table[_BCM_QOS_MAP_CHUNK_EGR_DSCP];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[2];

    /* Check parameters */
    if (NULL == map_id) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_QOS_MAP_INGRESS) {
        /* Check for pre-specified ID */
        if (flags & BCM_QOS_MAP_WITH_ID) {
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
            if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
                _BCM_QOS_MAP_TYPE_DSCP_TABLE) {
                return BCM_E_BADID;
            }
            if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
                if (!(flags & BCM_QOS_MAP_REPLACE)) {
                    return BCM_E_EXISTS;
                }
            } else {
                _BCM_QOS_DSCP_TABLE_USED_SET(unit, id);
            }
        } else {
            id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->dscp_table_bitmap,
                                   _BCM_QOS_MAP_TYPE_DSCP_TABLE);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_DSCP_TABLE_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_DSCP_TABLE
                      << _BCM_QOS_MAP_SHIFT);
        }
        /* Reserve a chunk in the DSCP_TABLE */
        sal_memset(dscp_table, 0, sizeof(dscp_table));
        entries[0] = &dscp_table;
        BCM_IF_ERROR_RETURN(_bcm_dscp_table_entry_add(unit, entries,
                                                      _BCM_QOS_MAP_CHUNK_DSCP,
                                                      (uint32 *)&index));
        QOS_INFO(unit)->dscp_hw_idx[id] = index / _BCM_QOS_MAP_CHUNK_DSCP;
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* Check for pre-specified ID */
        if (flags & BCM_QOS_MAP_WITH_ID) {
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
            if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
                _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) {
                return BCM_E_BADID;
            }
            if (_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                if (!(flags & BCM_QOS_MAP_REPLACE)) {
                    return BCM_E_EXISTS;
                }
            } else {
                _BCM_QOS_EGR_DSCP_TABLE_USED_SET(unit, id);
            }
        } else {
            id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->egr_dscp_table_bitmap,
                                   _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_EGR_DSCP_TABLE_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE
                      << _BCM_QOS_MAP_SHIFT);
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
            rv = _bcm_td2p_egr_qos_combo_map_create(unit, flags, map_id);
            if (BCM_FAILURE(rv)) {
                return (rv);
            }
        } else
#endif
        {
            /* Reserve a chunk in the EGR_DSCP_TABLE */
            sal_memset(egr_dscp_table, 0, sizeof(egr_dscp_table));
            entries[0] = &egr_dscp_table;
            if (SOC_IS_TRIDENT3X(unit)) {
                /* use same setting for both Zone2 and Zone4 */
                entries[1] = &egr_dscp_table; /* for Zone2 */
            }
            BCM_IF_ERROR_RETURN(_bcm_egr_dscp_table_entry_add(unit, entries,
                                                       _BCM_QOS_MAP_CHUNK_EGR_DSCP,
                                                              (uint32 *)&index, 0));
            QOS_INFO(unit)->egr_dscp_hw_idx[id] = (index /
                                                   _BCM_QOS_MAP_CHUNK_EGR_DSCP);
        }
    }
    return rv;
}

STATIC int
_bcm_tr2_qos_mpls_map_create(int unit, uint32 flags, int *map_id)
{
    /* Check parameters */
    if (NULL == map_id) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_QOS_MAP_INGRESS) {
        /* The ING_MPLS_EXP_MAPPING table is used */
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        uint32 mpls_exp_map_flags = BCM_MPLS_EXP_MAP_INGRESS;

        if (flags & BCM_QOS_MAP_WITH_ID) {
            mpls_exp_map_flags |= BCM_MPLS_EXP_MAP_WITH_ID;
        }

        BCM_IF_ERROR_RETURN(bcm_tr_mpls_exp_map_create(unit,
                                            mpls_exp_map_flags, map_id));
        _BCM_QOS_ING_MPLS_EXP_USED_SET(unit,
                        ((*map_id) & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK));
#endif
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* The EGR_MPLS_EXP_MAPPING_1 table is used */
        /* Shared with L2 egress map */
        uint32 qos_map_flags = BCM_QOS_MAP_EGRESS;

        if (flags & BCM_QOS_MAP_WITH_ID) {
            qos_map_flags |= BCM_QOS_MAP_WITH_ID;
        }

        BCM_IF_ERROR_RETURN(_bcm_tr2_qos_l2_map_create(unit, qos_map_flags,
                                                       map_id));
        _BCM_QOS_EGR_MPLS_FLAGS_USED_SET(unit,
                                         ((*map_id) & _BCM_QOS_MAP_TYPE_MASK));
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#ifdef BCM_APACHE_SUPPORT
STATIC int
_bcm_tr2_qos_mpls_exp_to_phb_cng_map_create(int unit, uint32 flags, int *map_id)
{
    if (flags & BCM_QOS_MAP_INGRESS) {
        /* The ING_MPLS_EXP_MAPPING_1 table is used */
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        uint32 mpls_exp_map_flags = BCM_MPLS_EXP_MAP_INGRESS;

        if (flags & BCM_QOS_MAP_WITH_ID) {
            mpls_exp_map_flags |= BCM_MPLS_EXP_MAP_WITH_ID;
        }

        BCM_IF_ERROR_RETURN(bcm_tr_mpls_exp_to_phb_cng_map_create(unit,
                                            mpls_exp_map_flags, map_id));
#endif
    } else if (flags & BCM_QOS_MAP_EGRESS) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        /* The EGR_MPLS_EXP_MAPPING_3 table is used */
        BCM_IF_ERROR_RETURN(bcm_tr_mpls_exp_to_phb_cng_map_create(unit,
                                            BCM_MPLS_EXP_MAP_EGRESS, map_id));
#endif
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif

#ifdef BCM_HURRICANE3_SUPPORT
STATIC int
_bcm_hr3_qos_miml_map_create(int unit, uint32 flags, int *map_id)
{
    ing_pri_cng_map_entry_t ing_pri_map[_BCM_QOS_MAP_CHUNK_PRI_CNG_MAX];
    ing_untagged_phb_entry_t ing_untagged_phb;
    egr_mpls_pri_mapping_entry_t egr_mpls_pri_map[_BCM_QOS_MAP_CHUNK_EGR_MPLS];
    egr_mpls_exp_mapping_1_entry_t egr_mpls_exp_map[_BCM_QOS_MAP_CHUNK_EGR_MPLS];
    egr_mpls_exp_mapping_2_entry_t egr_mpls_exp_map2[_BCM_QOS_MAP_CHUNK_EGR_MPLS];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[3];
    if (flags & BCM_QOS_MAP_INGRESS) {
        /* Check for pre-specified ID */
        if (flags & BCM_QOS_MAP_WITH_ID) {
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
            if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
                _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP) {
                return BCM_E_BADID;
            }
            if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                return BCM_E_PARAM;
            }
            if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                if (!(flags & BCM_QOS_MAP_REPLACE)) {
                    return BCM_E_EXISTS;
                }
            } else {
                _BCM_QOS_ING_PRI_CNG_USED_SET(unit, id);
            }
        } else {
            id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->ing_pri_cng_bitmap,
                                   _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_ING_PRI_CNG_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP
                      << _BCM_QOS_MAP_SHIFT);
        }
        /* Reserve a chunk in the ING_PRI_CNG_MAP table */
        sal_memset(ing_pri_map, 0, sizeof(ing_pri_map));
        entries[0] = &ing_pri_map;
        entries[1] = &ing_untagged_phb;
        BCM_IF_ERROR_RETURN(_bcm_ing_pri_cng_map_entry_add(unit, entries,
                                            _BCM_QOS_MAP_CHUNK_PRI_CNG,
                                                           (uint32 *)&index));
        QOS_INFO(unit)->pri_cng_hw_idx[id] = (index /
                                              _BCM_QOS_MAP_CHUNK_PRI_CNG);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* Check for pre-specified ID */
        if (flags & BCM_QOS_MAP_WITH_ID) {
            id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
            if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
                _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS) {
                return BCM_E_BADID;
            }
            if (_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                if (!(flags & BCM_QOS_MAP_REPLACE)) {
                    return BCM_E_EXISTS;
                }
            } else {
                _BCM_QOS_EGR_MPLS_USED_SET(unit, id);
            }
        } else {
            id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->egr_mpls_bitmap,
                                   _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_EGR_MPLS_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS
                      << _BCM_QOS_MAP_SHIFT);
        }
        /* Reserve a chunk in the EGR_MPLS_EXP_MAPPING_1/EGR_MPLS_PRI_MAPPING
           tables - these two tables always done together */
        sal_memset(egr_mpls_pri_map, 0, sizeof(egr_mpls_pri_map));
        entries[0] = egr_mpls_pri_map;
        sal_memset(egr_mpls_exp_map, 0, sizeof(egr_mpls_exp_map));
        entries[1] = egr_mpls_exp_map;
        sal_memset(egr_mpls_exp_map2, 0, sizeof(egr_mpls_exp_map2));
        entries[2] = egr_mpls_exp_map2;
        BCM_IF_ERROR_RETURN(_bcm_egr_mpls_combo_map_entry_add(unit, entries,
                                                   _BCM_QOS_MAP_CHUNK_EGR_MPLS,
                                                              (uint32 *)&index));
        QOS_INFO(unit)->egr_mpls_hw_idx[id] = (index /
                                               _BCM_QOS_MAP_CHUNK_EGR_MPLS);
    }
    return rv;
}
#endif /* BCM_HURRICANE3_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
/* Create a ING_QUEUE_OFFSET_MAP profile */
STATIC int
_bcm_kt_ing_queue_offset_map_create(int unit, uint32 flags, int *map_id)
{
    ing_queue_offset_mapping_table_entry_t
                             offset_map_table[_BCM_QOS_MAP_CHUNK_OFFSET_MAP];
    void *entries[1];
    int id = 0, index = -1;
    if (flags & BCM_QOS_MAP_WITH_ID) {
      /* Check for pre-specified ID */
        id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
        if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
            _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE) {
            return BCM_E_BADID;
        }
        if (_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, id)) {
            if (!(flags & BCM_QOS_MAP_REPLACE)) {
                return BCM_E_EXISTS;
            }
        } else {
                _BCM_QOS_OFFSET_MAP_TABLE_USED_SET(unit, id);
        }

    } else {
        id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->offset_map_table_bitmap,
                                  _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_OFFSET_MAP_TABLE_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE
                                                     << _BCM_QOS_MAP_SHIFT);
    }
    /* Reserve a chunk in the ING_QUEUE_OFFSET_MAPPING_TABLE */
    sal_memset(offset_map_table, 0, sizeof(offset_map_table));
    entries[0] = &offset_map_table;
    BCM_IF_ERROR_RETURN(_bcm_offset_map_table_entry_add(unit, entries,
                     _BCM_QOS_MAP_CHUNK_OFFSET_MAP, (uint32 *)&index));
    QOS_INFO(unit)->offset_map_hw_idx[id] =
                                   (index / _BCM_QOS_MAP_CHUNK_OFFSET_MAP);
    return BCM_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
/* Create a RQE_QUEUE_OFFSET profile */
STATIC int
_bcm_kt2_rqe_queue_offset_map_create(int unit, uint32 flags, int *map_id)
{
    int id = 0;
    if (flags & BCM_QOS_MAP_WITH_ID) {
      /* Check for pre-specified ID */
        id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
        if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
            _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP) {
            return BCM_E_BADID;
        }
        if (_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id)) {
            if (!(flags & BCM_QOS_MAP_REPLACE)) {
                return BCM_E_EXISTS;
            }
        } else {
            _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_SET(unit, id);
        }
    } else {
        id = _bcm_tr2_qos_id_alloc(unit, QOS_INFO(unit)->rqe_queue_offset_bitmap,
                                   _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP
                                                     << _BCM_QOS_MAP_SHIFT);
    }
    QOS_INFO(unit)->rqe_queue_offset_hw_idx[id] = id;
    return BCM_E_NONE;
}
STATIC
int
_bcm_rqe_queue_offset_map_table_compare(int unit, uint32 *entry, int id, int *profile_offset)
{
    int i = 0, hw_idx;
    int pri = 0;
    int no_match_flag = 1;
    int value = 0;
    int queue_offset = 0;
    int rv = BCM_E_NONE;
    soc_field_t profile_field[] = {Q_OFFSET0f,
                                   Q_OFFSET1f,
                                   Q_OFFSET2f,
                                   Q_OFFSET3f};

    hw_idx = QOS_INFO(unit)->rqe_queue_offset_hw_idx[id];
    while (i < _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP) {
        if (!_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, i)) {
            i++;
            continue;
        }
        if (i == hw_idx) {
            i++;
            continue;
        }

        for (pri = 0, no_match_flag =0;
             pri < _BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP;
             pri++) {
            value = soc_reg_field_get(unit, RQE_QUEUE_OFFSETr,
                                         entry[pri]/* regval[priority]*/,
                                          profile_field[hw_idx]/* profile  being set*/);
            queue_offset = soc_reg_field_get(unit , RQE_QUEUE_OFFSETr,
                                     entry[pri] /* regval[priority]*/,
                                     profile_field[i] /* profile compared against*/);

            if (value != queue_offset) {
              /* single mismatch is no profile match*/
              no_match_flag = 1;
              break;
            }
        }
        if (!no_match_flag) {
        /* first match return profile_offset id */
           *profile_offset = i;
           break;
        }
        /* increment to next profile */
        i++;
    }
    if (no_match_flag) {
        *profile_offset = id;
        rv = BCM_E_NOT_FOUND;
    }
    return rv;
}
STATIC
int
_bcm_rqe_queue_offset_map_table_sw_delete (int unit, int id)
{

    int i = 0;
    int p_idx = 0;
    int rv = BCM_E_NONE;
    int refcount = 0;
    uint32 rqe_queue_offset_profile[_BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP];
    soc_field_t profile_field[] = {Q_OFFSET0f,
                                   Q_OFFSET1f,
                                   Q_OFFSET2f,
                                   Q_OFFSET3f};

    if (!_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id)) {
        return BCM_E_NOT_FOUND;
    }
    sal_memset(rqe_queue_offset_profile, 0, sizeof(rqe_queue_offset_profile));
    /* Read the profile chunk */
    for (i = 0; i <_BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP;
         i++) {
        rv = READ_RQE_QUEUE_OFFSETr(unit, i,
                           &rqe_queue_offset_profile[i]);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* get the hw id
     * decrement the refcount
     * if refcount is 0
     * delete the profile_offset fields
     */
    p_idx = QOS_INFO(unit)->rqe_queue_offset_hw_idx[id];
    QOS_INFO(unit)->rqe_queue_offset_refcount[p_idx]--;
    refcount = QOS_INFO(unit)->rqe_queue_offset_refcount[p_idx];

    if (!refcount) {
        for(i=0; i<_BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP;
         i++) {
            soc_reg_field_set(unit, RQE_QUEUE_OFFSETr,
                  &rqe_queue_offset_profile[i], profile_field[p_idx],
                  0);
            rv = WRITE_RQE_QUEUE_OFFSETr(unit, i,
                           rqe_queue_offset_profile[i]);
            if (BCM_FAILURE(rv)) {
                return rv;
            }

         }
    }

    return BCM_E_NONE;

}
int
_bcm_rqe_queue_offset_map_multi_get(int unit, uint32 flags,
                          int id, int array_size,
                          bcm_qos_map_t *array, int *array_count)
{
    int idx;
    int count = 0;
    int hw_id = 0;
    int num_entries = 0;

    uint32      entry;
    bcm_qos_map_t   *map = NULL;
    soc_field_t profile_field[] = {Q_OFFSET0f,
                                   Q_OFFSET1f,
                                   Q_OFFSET2f,
                                   Q_OFFSET3f};
    /* QOS_LOCK -lock is taken by parent
     */
    num_entries = _BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP;
    hw_id = QOS_INFO(unit)->rqe_queue_offset_hw_idx[id] ;

    for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
        /* read the profile offset on the hw_id
         */
        map = &array[idx];
        map->int_pri = idx;
        READ_RQE_QUEUE_OFFSETr(unit, idx, &entry);
        map->queue_offset = soc_reg_field_get(unit, RQE_QUEUE_OFFSETr,
              entry, profile_field[hw_id]);
        count++;

    }

    *array_count = count;
    return BCM_E_NONE;


}
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
/* Create a ING_SERVICE_PRI_MAP profile */
STATIC int
_bcm_sb2_ing_service_pri_map_create(int unit, uint32 flags, int *map_id)
{
    ing_service_pri_map_0_entry_t
        service_pri_map_table[_BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP];
    void *entries[_BCM_SB2_OAM_PRI_MAP_TABLE_MAX];
    int pri = 0;
    int id = 0, index = -1;
    soc_mem_t mem = ING_SERVICE_PRI_MAP_0m;
    int table;
    if (flags & BCM_QOS_MAP_WITH_ID) {
      /* Check for pre-specified ID */
        id = *map_id & _BCM_QOS_MAP_TYPE_MASK;
        if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
            _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE) {
            return BCM_E_BADID;
        }
        if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id)) {
            if (!(flags & BCM_QOS_MAP_REPLACE)) {
                return BCM_E_EXISTS;
            }
        } else {
                _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_SET(unit, id);
        }

    } else {
        id = _bcm_tr2_qos_id_alloc(unit,
             QOS_INFO(unit)->service_pri_map_table_bitmap,
             _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE
                                                     << _BCM_QOS_MAP_SHIFT);
    }
    /* Reserve a chunk in the ING_SERVICE_PRI_MAP_0/1/2 TABLE */
    for ( pri = 0; pri < _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP; pri++) {
        /* Clear ingress service pri map profile entry. */
        sal_memcpy(&service_pri_map_table[pri], soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));

        soc_mem_field32_set(unit, mem,
                &service_pri_map_table[pri], OFFSET_VALIDf, 1);
    }
    for (table = 0; table < _BCM_SB2_OAM_PRI_MAP_TABLE_MAX; table++) {
        /* Assign every table with same set of values */
        entries[table] = &service_pri_map_table;
    }
    BCM_IF_ERROR_RETURN(_bcm_service_pri_map_table_entry_add(unit, entries,
                     _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP, (uint32 *)&index));
    QOS_INFO(unit)->service_pri_map_hw_idx[id] =
                                   (index / _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP);
    _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_SET(unit, id, flags);
    return BCM_E_NONE;
}
#endif /* BCM_SABER2_SUPPORT */

/* Create a QoS map profile */
/* This will allocate an ID and a profile index with all-zero mapping */
int
bcm_tr2_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int rv = BCM_E_UNAVAIL;
    QOS_INIT(unit);
    if (flags == 0) {
        return BCM_E_PARAM;
    }
    QOS_LOCK(unit);
    if (flags & BCM_QOS_MAP_L2) {
        rv = _bcm_tr2_qos_l2_map_create(unit, flags, map_id);
    } else if (flags & BCM_QOS_MAP_L3) {
        rv = _bcm_tr2_qos_l3_map_create(unit, flags, map_id);
    } else if (flags & BCM_QOS_MAP_MPLS) {
        if (!soc_feature(unit, soc_feature_mpls)) {
            QOS_UNLOCK(unit);
            return rv;
        }
        rv = _bcm_tr2_qos_mpls_map_create(unit, flags, map_id);
    }
#ifdef BCM_KATANA_SUPPORT
    else if (flags & BCM_QOS_MAP_QUEUE) {
        rv = _bcm_kt_ing_queue_offset_map_create(unit, flags, map_id);
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    else if (flags & BCM_QOS_MAP_REPLICATION) {
        rv = _bcm_kt2_rqe_queue_offset_map_create(unit, flags, map_id);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    else if ((flags & BCM_QOS_MAP_OAM_MPLS_EXP) &&
            ((flags & BCM_QOS_MAP_INGRESS) ||
             (flags & BCM_QOS_MAP_EGRESS))) {
            if (soc_feature(unit, soc_feature_mpls_exp_to_phb_cng_map)) {
                rv = _bcm_tr2_qos_mpls_exp_to_phb_cng_map_create(unit, flags, map_id);
            } else {
                QOS_UNLOCK(unit);
                return BCM_E_UNAVAIL;
            }
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    else if (
            (flags & BCM_QOS_MAP_OAM_INTPRI)         ||
            (flags & BCM_QOS_MAP_OAM_OUTER_VLAN_PCP) ||
            (flags & BCM_QOS_MAP_OAM_INNER_VLAN_PCP) ||
            (flags & BCM_QOS_MAP_OAM_MPLS_EXP)
            ) {
        if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
            rv = _bcm_sb2_ing_service_pri_map_create(unit, flags, map_id);
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    }
#endif

#ifdef BCM_HURRICANE3_SUPPORT
    else if (flags & BCM_QOS_MAP_MIML) {
        if (!soc_feature(unit, soc_feature_miml)) {
            QOS_UNLOCK(unit);
            return rv;
        }
        rv = _bcm_hr3_qos_miml_map_create(unit, flags, map_id);
    }
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    QOS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_qos_vlan_port_egress_inner_pri_mapping_clear
 * Purpose:
 *      Clear the IPRI mapping pointer from EGR_VLAN_CONTROL_3 register for all ports
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      index      - (IN) Index to access EGR_MPLS_EXP_MAPPING_2m table.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_qos_vlan_port_egress_inner_pri_mapping_clear(int unit, int index)
{
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
	    int rv, alloc_size;
        uint32 rval;
        char *buf;
        void *entries[1];
        uint8 pri = 0;
        int  id=0;
		bcm_port_t port;
        uint32 old_index=0;

        if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
            alloc_size = 64 * sizeof(egr_pri_cng_map_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls exp map2");
            if (NULL == buf) {
                return (BCM_E_MEMORY);
            }
            id = (index * _BCM_QOS_MAP_CHUNK_EGR_MPLS);
            rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_2m, MEM_BLOCK_ANY,
                                    id,
                                    (id + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                    (void *) buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                return rv;
            }

            /* Check if priority is configured for given QOS MAP ID profile */
            for (id = 0; id < _BCM_QOS_MAP_CHUNK_EGR_MPLS; id++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit,
                                                  EGR_MPLS_EXP_MAPPING_2m,
                                                  void *, buf, id);
                if (!SOC_IS_TRIDENT3X(unit)) {
                    pri = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_2m,
                                              entries[0], PRIf);
                }
                /* Priority configured, proceed with configuration of
                 * VLAN_CONTROL_3 register, with IPRI_MAPPING_PTR and
                 * IPRI_CIF_SEL fields.
                 */
                if (pri) {
                      break;
                }
            }
            soc_cm_sfree(unit, buf);
        }

        /* Priority field is not configured in EGR_MPLS_EXP_MAPPING_2m table,
         * skip VLAN_CONTROL_3 register configuration.
         */
        if ((!pri)) {
            return BCM_E_NONE;
        }

		/* Iterate over ports, clear the mapping pointer */
		PBMP_PORT_ITER(unit, port)
		{
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                soc_mem_t mem = EGR_VLAN_CONTROL_3m;
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_get(unit,
                        port, mem, IPRI_MAPPING_PTRf, &old_index));
                    if (old_index == index) {
                        soc_field_t fields[2] = {
                            IPRI_MAPPING_PTRf, IPRI_CFI_SELf
                        };
                        uint32 values[2] = {0, 0};
                        BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_fields_set(
                            unit, port, mem, 2, fields, values));
                    }
                } else {
                    egr_vlan_control_3_entry_t entry;
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &entry);
                    BCM_IF_ERROR_RETURN(rv);

                    old_index = soc_mem_field32_get(unit, mem, &entry,
                                                    IPRI_MAPPING_PTRf);
                    if (old_index == index) {
                        soc_mem_field32_set(unit, mem, &entry,
                            IPRI_MAPPING_PTRf, 0);
                        soc_mem_field32_set(unit, mem, &entry,
                            IPRI_CFI_SELf, 0);
                        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, port,
                                &entry);
                        BCM_IF_ERROR_RETURN(rv);
                    }
                }
            } else
#endif
            {
                rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
                BCM_IF_ERROR_RETURN(rv);
                old_index = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                        rval, IPRI_MAPPING_PTRf);
                /* Clear mapping pointer */
                if (old_index == index) {
                    soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                                      IPRI_MAPPING_PTRf, 0);
                    soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                                      IPRI_CFI_SELf, 0);
                    rv = WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval);
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
		}
    }

    return BCM_E_NONE;
}


/* Destroy a QoS map profile */
/* This will free the profile index and de-allocate the ID */
int
bcm_tr2_qos_map_destroy(int unit, int map_id)
{
    int id, rv = BCM_E_UNAVAIL;
    int                 bmp_len;
    SHR_BITDCL          *temp_bmp;
    int                 extract_only = 0;

    QOS_INIT(unit);
#if defined (BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_qos_validate_map_type(unit, map_id));
    }
#endif

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
#endif
        if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_ing_pri_cng_map_entry_delete
                     (unit, QOS_INFO(unit)->pri_cng_hw_idx[id] *
                      _BCM_QOS_MAP_CHUNK_PRI_CNG);
            QOS_INFO(unit)->pri_cng_hw_idx[id] = 0;
            _BCM_QOS_ING_PRI_CNG_USED_CLR(unit, id);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS:
#endif
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)
               && _BCM_QOS_EGR_MPLS_EXTERNAL_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_BUSY;
        }
#endif
            if ( SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                 (SOC_IS_TD_TT(unit) && !SOC_IS_TOMAHAWK3(unit))) {
                /* Clear existing map from port EGR_VLAN_CONTROL_3 register */
                rv = _bcm_trx_qos_vlan_port_egress_inner_pri_mapping_clear(unit,
                                          QOS_INFO(unit)->egr_mpls_hw_idx[id]);
                if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "Clear qos mapid (%d) from "
                           " EGR_VLAN_CONTROL_3 register failed\n"),
                           QOS_INFO(unit)->egr_mpls_hw_idx[id]));
                }
            }
            rv = _bcm_egr_mpls_combo_map_entry_delete (unit,
                                       (QOS_INFO(unit)->egr_mpls_hw_idx[id] *
                                        _BCM_QOS_MAP_CHUNK_EGR_MPLS));
            QOS_INFO(unit)->egr_mpls_hw_idx[id] = 0;
            _BCM_QOS_EGR_MPLS_USED_CLR(unit, id);
            _BCM_QOS_EGR_MPLS_FLAGS_USED_CLR(unit, id);
        }
        break;
#ifdef BCM_APACHE_SUPPORT
    case _BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP:
        if (soc_feature(unit, soc_feature_mpls_exp_to_phb_cng_map)) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            rv = bcm_tr_mpls_exp_to_phb_cng_map_destroy(unit, map_id);
#endif
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
#endif
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_dscp_table_entry_delete
                     (unit, (QOS_INFO(unit)->dscp_hw_idx[id] *
                      _BCM_QOS_MAP_CHUNK_DSCP));
            QOS_INFO(unit)->dscp_hw_idx[id] = 0;
            _BCM_QOS_DSCP_TABLE_USED_CLR(unit, id);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
            rv = _bcm_td2p_egr_qos_combo_map_destroy(unit, map_id);
            if (BCM_FAILURE(rv)) {
                QOS_UNLOCK(unit);
                return (rv);
            }
        } else
#endif
        {
            if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                QOS_UNLOCK(unit);
                return BCM_E_NOT_FOUND;
            } else {
                /* check for external references before delete */
                if (soc_feature(unit, soc_feature_egr_dscp_map_ext_ref_check)) {
                    bmp_len = _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
                    temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
                    if (NULL == temp_bmp) {
                        QOS_UNLOCK(unit);
                        return (BCM_E_MEMORY);
                    }
                    sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
                    extract_only = 1; /* extract the used map IDs bitmap */
                    if (soc_mem_field_valid(unit, EGR_IP_TUNNELm, DSCP_MAPPING_PTRf)) {
                        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_IP_TUNNELm,
                                DSCP_MAPPING_PTRf,_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                                temp_bmp, bmp_len, extract_only);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_INTFm, DSCP_MAPPING_PTRf)) {
                        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_L3_INTFm,
                                DSCP_MAPPING_PTRf,_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                                temp_bmp, bmp_len, extract_only);
                    }
                    if (SHR_BITGET(temp_bmp, QOS_INFO(unit)->egr_dscp_hw_idx[id])) {
                        /* external reference found*/
                        QOS_UNLOCK(unit);
                        sal_free(temp_bmp);
                        return BCM_E_BUSY;
                    }
                    sal_free(temp_bmp);
                }
                rv = _bcm_egr_dscp_table_entry_delete
                        (unit, (QOS_INFO(unit)->egr_dscp_hw_idx[id] *
                         _BCM_QOS_MAP_CHUNK_EGR_DSCP));
                if (BCM_E_NONE == rv) {
                    QOS_INFO(unit)->egr_dscp_hw_idx[id] = 0;
                    _BCM_QOS_EGR_DSCP_TABLE_USED_CLR(unit, id);
                }
            }
        }
        break;
#ifdef BCM_KATANA_SUPPORT
    case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
        if (!_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_offset_map_table_entry_delete
                     (unit, (QOS_INFO(unit)->offset_map_hw_idx[id] *
                      _BCM_QOS_MAP_CHUNK_OFFSET_MAP));
            QOS_INFO(unit)->offset_map_hw_idx[id] = 0;
            _BCM_QOS_OFFSET_MAP_TABLE_USED_CLR(unit, id);
        }
        break;
#endif

#ifdef BCM_SABER2_SUPPORT
    case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
        if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
            if (!_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id)) {
                QOS_UNLOCK(unit);
                return BCM_E_NOT_FOUND;
            } else {
                rv = _bcm_service_pri_map_table_entry_delete
                    (unit, (QOS_INFO(unit)->service_pri_map_hw_idx[id] *
                            _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP));
                QOS_INFO(unit)->service_pri_map_hw_idx[id] = 0;
                _BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_CLR(unit, id);
                _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_CLR(unit, id);
            }
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
#endif
#ifdef BCM_KATANA2_SUPPORT
    case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
        if (!_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_rqe_queue_offset_map_table_sw_delete(unit,
                 id);
            QOS_INFO(unit)->rqe_queue_offset_hw_idx[id] = 0;
            _BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_CLR(unit, id);
        }
        break;
#endif

    default:
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
            if (!soc_feature(unit, soc_feature_mpls)) {
                QOS_UNLOCK(unit);
                return rv;
            }
            rv = bcm_tr_mpls_exp_map_destroy(unit, map_id);
            if (BCM_SUCCESS(rv)) {
                _BCM_QOS_ING_MPLS_EXP_USED_CLR(unit,
                    (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK));
            }
        } else
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
        {
            rv = BCM_E_PARAM;
        }
        break;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    QOS_UNLOCK(unit);
    return rv;
}

#ifdef BCM_SABER2_SUPPORT
STATIC int
_bcm_sb2_qos_oam_pri_map_mpls_exp_check(bcm_qos_map_t *map)
{
    if ((map->exp > _BCM_SB2_MPLS_EXP_MAX) ||
       (map->exp < _BCM_SB2_MPLS_EXP_MIN)) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_sb2_qos_oam_pri_map_pkt_pri_check(bcm_qos_map_t *map)
{
    if (map->pkt_pri > _BCM_SB2_PKT_PRI_MAX) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_sb2_qos_oam_pri_map_int_pri_check(bcm_qos_map_t *map)
{
    if ((map->int_pri > _BCM_SB2_INT_PRI_MAX) ||
       (map->int_pri < _BCM_SB2_INT_PRI_MIN)) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_sb2_qos_oam_pri_map_counter_offset_check(bcm_qos_map_t *map)
{
    if (map->counter_offset > _BCM_SB2_OAM_PRI_MAP_COUNTER_OFFSET_MAX ||
       map->counter_offset < _BCM_SB2_OAM_PRI_MAP_COUNTER_OFFSET_MIN) {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_sb2_qos_oam_pri_map_params_check(uint32 flags, bcm_qos_map_t *map)
{
    BCM_IF_ERROR_RETURN(_bcm_sb2_qos_oam_pri_map_counter_offset_check(map));

    if (flags & BCM_QOS_MAP_OAM_INTPRI) {
        BCM_IF_ERROR_RETURN(_bcm_sb2_qos_oam_pri_map_int_pri_check(map));
    } else if ((flags & BCM_QOS_MAP_OAM_OUTER_VLAN_PCP) ||
               (flags & BCM_QOS_MAP_OAM_INNER_VLAN_PCP)) {
        BCM_IF_ERROR_RETURN(_bcm_sb2_qos_oam_pri_map_pkt_pri_check(map));
    } else if (flags & BCM_QOS_MAP_OAM_MPLS_EXP) {
        BCM_IF_ERROR_RETURN(_bcm_sb2_qos_oam_pri_map_mpls_exp_check(map));
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_sb2_service_pri_map_flags_pri_get(bcm_qos_map_t *map, uint32 flags, int *pri)
{
    if (flags & BCM_QOS_MAP_OAM_INTPRI) {
            *pri = map->int_pri;
    } else if ((flags & BCM_QOS_MAP_OAM_OUTER_VLAN_PCP) ||
               (flags & BCM_QOS_MAP_OAM_INNER_VLAN_PCP)) {
            *pri = map->pkt_pri;
    } else if (flags & BCM_QOS_MAP_OAM_MPLS_EXP) {
            *pri = map->exp;
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
_bcm_sb2_service_pri_map_flags_pri_set(uint32 flags, int pri, bcm_qos_map_t *map)
{
    if (flags & BCM_QOS_MAP_OAM_INTPRI) {
            map->int_pri = pri;
    } else if ((flags & BCM_QOS_MAP_OAM_OUTER_VLAN_PCP) ||
               (flags & BCM_QOS_MAP_OAM_INNER_VLAN_PCP)) {
            map->pkt_pri = pri;
    } else if (flags & BCM_QOS_MAP_OAM_MPLS_EXP) {
            map->exp = pri;
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
int
_bcm_sb2_service_pri_map_flags_max_pri_get(uint32 flags, int *max_pri)
{
    if (flags & BCM_QOS_MAP_OAM_INTPRI) {
           *max_pri = _BCM_SB2_INT_PRI_MAX;
    } else if ((flags & BCM_QOS_MAP_OAM_OUTER_VLAN_PCP) ||
               (flags & BCM_QOS_MAP_OAM_INNER_VLAN_PCP)) {
            *max_pri = _BCM_SB2_PKT_PRI_MAX;
    } else if (flags & BCM_QOS_MAP_OAM_MPLS_EXP) {
            *max_pri = _BCM_SB2_MPLS_EXP_MAX;
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#endif
#ifdef BCM_SABER2_SUPPORT
STATIC int
bcm_tr2_qos_port_map_set_internal(int unit, bcm_port_t port,
                                  soc_field_t field, int hw_index)
{
   int rv;

   soc_mem_lock(unit, PORT_TABm);
   rv = _bcm_tr2_port_tab_set(unit, port, field,
                              hw_index/_BCM_QOS_MAP_CHUNK_PRI_CNG);
   soc_mem_unlock(unit, PORT_TABm);
   return rv;
}
STATIC int
bcm_tr2_qos_copy_ing_pri_cng_entry(int unit,
                                   int src_index,
                                   int dest_index,
                                   int num_entry)
{
   uint32 pri_map[SOC_MAX_MEM_FIELD_WORDS];
   int idx;

   for (idx = 0; idx < num_entry; idx++) {
        SOC_IF_ERROR_RETURN(
            READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                  src_index + idx, &pri_map));
        SOC_IF_ERROR_RETURN(
            WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                   dest_index + idx, &pri_map));
   }
   return SOC_E_NONE;
}
#endif
/* Add an entry to a QoS map */
/* Read the existing profile chunk, modify what's needed and add the
 * new profile. This can result in the HW profile index changing for a
 * given ID */
int
bcm_tr2_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id, int internal)
{
    int alloc_size, offset, cng, id, index, rv = BCM_E_NONE, int_pri_max = 15;
    ing_pri_cng_map_entry_t ing_pri_map[16];
    phb_mapping_tbl_1_entry_t new_ing_pri_map[_BCM_QOS_MAP_CHUNK_PRI_CNG_MAX];
    ing_untagged_phb_entry_t ing_untagged_phb;
    egr_mpls_pri_mapping_entry_t *egr_mpls_pri_map;
    egr_mpls_exp_mapping_1_entry_t *egr_mpls_exp_map;
    egr_mpls_exp_mapping_2_entry_t *egr_mpls_exp_map2;
    dscp_table_entry_t *dscp_table;
    egr_dscp_table_entry_t *egr_dscp_table;
    void *entries[3], *entry;
#ifdef BCM_KATANA_SUPPORT
    ing_queue_offset_mapping_table_entry_t *offset_map_table;
#endif
#ifdef BCM_KATANA2_SUPPORT
    int hw_idx = -1;
    uint32 rqe_queue_offset_profile[_BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP];
    soc_field_t profile_field[] = {Q_OFFSET0f,
                                   Q_OFFSET1f,
                                   Q_OFFSET2f,
                                   Q_OFFSET3f};
    int i;
    int profile_offset = 0;
#endif

#ifdef BCM_SABER2_SUPPORT
    ing_service_pri_map_0_entry_t *service_pri_map_table;
    int pri = 0;
    uint32 conf_flags = 0;
    void *mem_entries[_BCM_SB2_OAM_PRI_MAP_TABLE_MAX];
    int table = 0;
    bcm_port_t port;
#endif
    int untagged = FALSE;
    int                 bmp_len;
    SHR_BITDCL          *temp_bmp;
    int                 extract_only = 0;

    QOS_INIT(unit);

    /* Check incoming parameters */
    if (NULL == map) {
        return BCM_E_PARAM;
    }

#if defined (BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_qos_validate_map_type(unit, map_id));
    }
#endif

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
#endif
        if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
           QOS_UNLOCK(unit);
           return BCM_E_PARAM;
        }
        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
#ifdef BCM_HURRICANE3_SUPPORT
            if (soc_feature(unit, soc_feature_miml)) {
                if (!((flags & BCM_QOS_MAP_L2) || (flags & BCM_QOS_MAP_MIML)) ||
                    !(flags & BCM_QOS_MAP_INGRESS)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            } else
#endif
            {
                if (!(flags & BCM_QOS_MAP_L2) || !(flags & BCM_QOS_MAP_INGRESS)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            }

            if (SOC_IS_TRIDENT3X(unit) || (SOC_MEM_IS_VALID(unit, ING_UNTAGGED_PHBm))) {
                if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                    untagged = TRUE;
                    map->pkt_pri = 0;
                    map->pkt_cfi = 0;
                }
            }

            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_ENDURO(unit)||
                SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
                int_pri_max = 7;
            }

            if ((map->int_pri > int_pri_max) || (map->int_pri < 0) ||
                (map->pkt_pri > 7) || (map->pkt_cfi > 1) ||
                ((map->color != bcmColorGreen) &&
                (map->color != bcmColorYellow) &&
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if (SOC_IS_TRIDENT3X(unit)) {
                entries[0] = new_ing_pri_map;
            } else {
                entries[0] = ing_pri_map;
            }
            entries[1] = &ing_untagged_phb;

            /* Base index of table in hardware */
            index = QOS_INFO(unit)->pri_cng_hw_idx[id] *
                    _BCM_QOS_MAP_CHUNK_PRI_CNG;

            rv = _bcm_ing_pri_cng_map_entry_get(unit, index,
                                                _BCM_QOS_MAP_CHUNK_PRI_CNG,
                                                entries);
            if (BCM_FAILURE(rv)) {
                QOS_UNLOCK(unit);
                return (rv);
            }

            if (untagged) {
                if (SOC_IS_TRIDENT3X(unit)) {
                    /* TD3 tagged and untagged shared the same table
                       bit 5 is indicating untaged packet or not */
                    /* Offset within table */
                    offset = 1 << 4 |(map->pkt_pri << 1) | map->pkt_cfi;

                    /* Modify what's needed */
                    entry = &new_ing_pri_map[offset];
                    soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, entry,
                                        INT_PRIf, map->int_pri);
                    soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, entry,
                                        CNGf, _BCM_COLOR_ENCODING(unit, map->color));
                } else {
                    soc_mem_field32_set(unit, ING_UNTAGGED_PHBm,
                                        &ing_untagged_phb, PRIf, map->int_pri);
                    soc_mem_field32_set(unit, ING_UNTAGGED_PHBm,
                                        &ing_untagged_phb, CNGf,
                                        _BCM_COLOR_ENCODING(unit, map->color));
                }
            } else {
                /* Offset within table */
                offset = (map->pkt_pri << 1) | map->pkt_cfi;
                /* Modify what's needed */
                if (SOC_IS_TRIDENT3X(unit)) {
                    entry = &new_ing_pri_map[offset];
                    soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, entry,
                                        INT_PRIf, map->int_pri);
                    soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, entry,
                                        CNGf, _BCM_COLOR_ENCODING(unit, map->color));
                } else {
                    entry = &ing_pri_map[offset];
                    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, entry,
                                        PRIf, map->int_pri);
                    soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, entry,
                                        CNGf, _BCM_COLOR_ENCODING(unit, map->color));
               }
           }
#ifdef BCM_SABER2_SUPPORT
           for (port = SB2_CMIC + 1; port < SB2_LPBK; port++) {
                if (_BCM_QOS_TRUST_DOT1P_PTR_CACHE_GET(unit, port,
                    index/_BCM_QOS_MAP_CHUNK_PRI_CNG)) {
                    break;
                 }
           }
           if (SOC_IS_SABER2(unit) && (!internal) && (port != SB2_LPBK)) {
               rv = bcm_tr2_qos_copy_ing_pri_cng_entry(unit, index,
                    QOS_INFO(unit)->pri_cng_hw_idx_reserved,
                    _BCM_QOS_MAP_CHUNK_PRI_CNG);
               if (BCM_FAILURE(rv)) {
                   QOS_UNLOCK(unit);
                   return (rv);
               }
               for (port = SB2_CMIC + 1; port < SB2_LPBK; port++) {
                    if (_BCM_QOS_TRUST_DOT1P_PTR_CACHE_GET(unit, port,
                        index/_BCM_QOS_MAP_CHUNK_PRI_CNG)) {
                        rv = bcm_tr2_qos_port_map_set_internal(unit, port,
                             TRUST_DOT1P_PTRf,
                             QOS_INFO(unit)->pri_cng_hw_idx_reserved);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return (rv);
                        }
                    }
               }
           }
#endif
           /* Delete the old profile chunk */
           rv = _bcm_ing_pri_cng_map_entry_delete(unit, index);
           if (BCM_FAILURE(rv)) {
               QOS_UNLOCK(unit);
               return (rv);
           }

           /* Add new chunk and store new HW index */
           rv = _bcm_ing_pri_cng_map_entry_add(unit, entries,
                                              _BCM_QOS_MAP_CHUNK_PRI_CNG,
                                              (uint32 *)&index);
           QOS_INFO(unit)->pri_cng_hw_idx[id] = (index /
                                                 _BCM_QOS_MAP_CHUNK_PRI_CNG);
#ifdef BCM_SABER2_SUPPORT
           if (SOC_IS_SABER2(unit) && (!internal)) {
               for (port = SB2_CMIC; port < SB2_LPBK; port++) {
                    if (_BCM_QOS_TRUST_DOT1P_PTR_CACHE_GET(unit, port,
                        index/_BCM_QOS_MAP_CHUNK_PRI_CNG)) {
                        rv = bcm_tr2_qos_port_map_set_internal(unit, port,
                             TRUST_DOT1P_PTRf, index);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return (rv);
                        }
                    }
               }
           }
#endif
        }
        break;
#ifdef BCM_APACHE_SUPPORT
    case _BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP:
        if (soc_feature(unit, soc_feature_mpls_exp_to_phb_cng_map)) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            bcm_mpls_exp_map_t ing_exp_map;
            bcm_mpls_exp_map_t_init(&ing_exp_map);
            if (flags & BCM_QOS_MAP_OAM_MPLS_EXP){
                if((!(flags & BCM_QOS_MAP_INGRESS)) &&
                   (!(flags & BCM_QOS_MAP_EGRESS))){
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            } else {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                    (map->exp > 7) || (map->exp < 0) ||
                    ((map->color != bcmColorGreen) &&
                     (map->color != bcmColorYellow) &&
                     (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Call the triumph/mpls.c implementation */
            ing_exp_map.exp = map->exp;
            ing_exp_map.priority = map->int_pri;
            ing_exp_map.color = map->color;
            rv = bcm_tr_mpls_exp_to_phb_cng_map_set(unit, map_id, &ing_exp_map);
#endif
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
#endif
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS:
#endif
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else if (SOC_IS_TRIDENT3X(unit)) {
#ifdef BCM_TRIDENT3_SUPPORT
            /* TRIDENT3 QoS memory map and binding takes Flex Path */
            rv = bcm_td3_l2_egr_combo_add(unit, flags, map, map_id,
                                          &QOS_INFO(unit)->egr_mpls_hw_idx[id],
                                          _BCM_QOS_MAP_CHUNK_EGR_MPLS);
            if (BCM_FAILURE(rv)) {
               QOS_UNLOCK(unit);
               return rv;
            }
#endif /* End of BCM_TRIDENT3_SUPPORT */
        } else {
            /*  BCM_QOS_MAP_L2 is pseudonym of BCM_QOS_MAP_L2_OUTER_TAG.*/
#ifdef BCM_HURRICANE3_SUPPORT
            if (soc_feature(unit, soc_feature_miml)) {
                if (!((flags & BCM_QOS_MAP_L2) ||
                      (flags & BCM_QOS_MAP_L2_INNER_TAG)||
                      (flags & BCM_QOS_MAP_MPLS) ||
                      (flags & BCM_QOS_MAP_MIML))||
                    !(flags & BCM_QOS_MAP_EGRESS)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            } else
#endif
            {
                if (!((flags & BCM_QOS_MAP_L2) ||
                      (flags & BCM_QOS_MAP_L2_INNER_TAG)||
                      (flags & BCM_QOS_MAP_MPLS))||
                    !(flags & BCM_QOS_MAP_EGRESS)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }

#if defined (BCM_TOMAHAWK3_SUPPORT)
                /* No double/inner tag support in TH3 */
                if (SOC_IS_TOMAHAWK3(unit) &&
                    (flags & BCM_QOS_MAP_L2_INNER_TAG)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
#endif
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->pkt_pri > 7) || (map->pkt_cfi > 1) ||
                ((map->color != bcmColorGreen) &&
                (map->color != bcmColorYellow) &&
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if (flags & BCM_QOS_MAP_MPLS) {
                if ((map->exp > 7) || (map->exp < 0)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            }

            /* Allocate memory for DMA */
            alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_MPLS *
                         sizeof(egr_mpls_pri_mapping_entry_t));
            egr_mpls_pri_map = soc_cm_salloc(unit, alloc_size,
                                             "TR2 egr mpls pri map");
            if (NULL == egr_mpls_pri_map) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_pri_map, 0, alloc_size);

            alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_MPLS *
                          sizeof(egr_mpls_exp_mapping_1_entry_t));
            egr_mpls_exp_map = soc_cm_salloc(unit, alloc_size,
                                             "TR2 egr mpls exp map");
            if (NULL == egr_mpls_exp_map) {
                sal_free(egr_mpls_pri_map);
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_exp_map, 0, alloc_size);

            alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_MPLS *
                          sizeof(egr_mpls_exp_mapping_2_entry_t));
            egr_mpls_exp_map2= soc_cm_salloc(unit, alloc_size,
                                             "TR2 egr mpls exp map2");
            if (NULL == egr_mpls_exp_map2) {
                sal_free(egr_mpls_pri_map);
                sal_free(egr_mpls_exp_map);
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_exp_map2, 0, alloc_size);

            /* Base index of table in hardware */
            index = (QOS_INFO(unit)->egr_mpls_hw_idx[id] *
                     _BCM_QOS_MAP_CHUNK_EGR_MPLS);

            /* Offset within table */
            cng = _BCM_COLOR_ENCODING(unit, map->color);
            offset = (map->int_pri << 2) | cng;

            /* Read the old profile chunk */
            rv = soc_mem_read_range(unit, EGR_MPLS_PRI_MAPPINGm, MEM_BLOCK_ANY,
                                    index,
                                    (index + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                    egr_mpls_pri_map);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_1m, MEM_BLOCK_ANY,
                                        index,
                                        (index + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                        egr_mpls_exp_map);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            }

            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_2m, MEM_BLOCK_ANY,
                                        index,
                                        (index + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                        egr_mpls_exp_map2);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            }

            /* Modify what's needed */
            entry = &egr_mpls_pri_map[offset];
            soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm, entry,
                                NEW_PRIf, map->pkt_pri);
            soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm, entry,
                                NEW_CFIf, map->pkt_cfi);
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                entry = &egr_mpls_exp_map[offset];

                /* Modify for OUTER VLAN remarking */
                if (!SOC_IS_TRIDENT3X(unit)) {
                    if (flags & BCM_QOS_MAP_L2_OUTER_TAG ) {
                        soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m, entry,
                                        PRIf, map->pkt_pri);
                        soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m, entry,
                                        CFIf, map->pkt_cfi);
                    }
                }
                if (flags & BCM_QOS_MAP_MPLS) {
                    soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m, entry,
                                        MPLS_EXPf, map->exp);
                }
            }
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
                entry = &egr_mpls_exp_map2[offset];

                /* Modify for INNER VLAN remarking */
                if (!SOC_IS_TRIDENT3X(unit)) {
                    if (flags & BCM_QOS_MAP_L2_INNER_TAG ) {
                        soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_2m, entry,
                                            PRIf, map->pkt_pri);
                        soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_2m, entry,
                                            CFIf, map->pkt_cfi);
                    }
                }
                if (flags & BCM_QOS_MAP_MPLS) {
                    soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_2m, entry,
                                        MPLS_EXPf, map->exp);
                }
            }

            /* Delete the old profile chunk */
            rv = _bcm_egr_mpls_combo_map_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            /* Add new chunk and store new HW index */
            entries[0] = egr_mpls_pri_map;
            entries[1] = egr_mpls_exp_map;
            entries[2] = egr_mpls_exp_map2;
            rv = _bcm_egr_mpls_combo_map_entry_add(unit, entries,
                                                   _BCM_QOS_MAP_CHUNK_EGR_MPLS,
                                                   (uint32 *)&index);
            QOS_INFO(unit)->egr_mpls_hw_idx[id] = (index /
                                                  _BCM_QOS_MAP_CHUNK_EGR_MPLS);

            cleanup:
                /* Free the DMA memory */
                soc_cm_sfree(unit, egr_mpls_pri_map);
                soc_cm_sfree(unit, egr_mpls_exp_map);
                soc_cm_sfree(unit, egr_mpls_exp_map2);
        }
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L3) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->dscp > DSCP_CODE_POINT_MAX) || (map->dscp < 0) ||
                ((map->color != bcmColorGreen) &&
                 (map->color != bcmColorYellow) &&
                 (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }

            /* Allocate memory for DMA */
            if (SOC_IS_TRIDENT3X(unit)) {
                alloc_size = _BCM_QOS_MAP_CHUNK_DSCP * sizeof(phb_mapping_tbl_2_entry_t);
            } else {
                alloc_size = _BCM_QOS_MAP_CHUNK_DSCP * sizeof(dscp_table_entry_t);
            }

            dscp_table = soc_cm_salloc(unit, alloc_size,
                                       "TR2 dscp table");
            if (NULL == dscp_table) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(dscp_table, 0, alloc_size);

            /* Base index of table in hardware */
            index = QOS_INFO(unit)->dscp_hw_idx[id] * _BCM_QOS_MAP_CHUNK_DSCP;

            /* Offset within table */
            offset = map->dscp;

            /* Read the old profile chunk */
            rv = soc_mem_read_range(unit, _bcm_tr2_qos_bk_info[unit].dscp_table,
                                    MEM_BLOCK_ANY,
                                    index, index + (_BCM_QOS_MAP_CHUNK_DSCP-1),
                                    dscp_table);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, dscp_table);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Modify what's needed */
            entry = &dscp_table[offset];
            cng = _BCM_COLOR_ENCODING(unit, map->color);

            if (SOC_IS_TRIDENT3X(unit)) {
                soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, DSCPf, map->dscp);
                soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, INT_PRIf, map->int_pri);
                soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, DSCP_VALIDf, 1);
                soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, CNGf, cng);
            } else {
                soc_DSCP_TABLEm_field32_set(unit, entry, DSCPf, map->dscp);
                soc_DSCP_TABLEm_field32_set(unit, entry, PRIf, map->int_pri);
                soc_DSCP_TABLEm_field32_set(unit, entry, CNGf, cng);
            }

            /* Delete the old profile chunk */
            rv = _bcm_dscp_table_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, dscp_table);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = dscp_table;
            rv = _bcm_dscp_table_entry_add(unit, entries, _BCM_QOS_MAP_CHUNK_DSCP,
                                           (uint32 *)&index);
            QOS_INFO(unit)->dscp_hw_idx[id] = index / _BCM_QOS_MAP_CHUNK_DSCP;

            /* Free the DMA memory */
            soc_cm_sfree(unit, dscp_table);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L3) || !(flags & BCM_QOS_MAP_EGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->dscp > DSCP_CODE_POINT_MAX) || (map->dscp < 0) ||
                ((map->color != bcmColorGreen) &&
                 (map->color != bcmColorYellow) &&
                 (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                rv = _bcm_td2p_egr_qos_combo_map_add(unit, flags, map, map_id);
                if (BCM_FAILURE(rv)) {
                    QOS_UNLOCK(unit);
                    return (rv);
                }

            } else
#endif
            {
                /* check for external references before delete */
                if (soc_feature(unit, soc_feature_egr_dscp_map_ext_ref_check) &&
                    (1 == internal)) {
                    bmp_len = _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE;
                    temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
                    if (NULL == temp_bmp) {
                        QOS_UNLOCK(unit);
                        return (BCM_E_MEMORY);
                    }
                    sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
                    extract_only = 1; /* extract the used map IDs bitmap */
                    if (soc_mem_field_valid(unit, EGR_IP_TUNNELm, DSCP_MAPPING_PTRf)) {
                        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_IP_TUNNELm,
                                DSCP_MAPPING_PTRf,_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                                temp_bmp, bmp_len, extract_only);
                    }
                    if (soc_mem_field_valid(unit, EGR_L3_INTFm, DSCP_MAPPING_PTRf)) {
                        rv = _bcm_tr2_qos_reinit_from_hw_state(unit, EGR_L3_INTFm,
                                DSCP_MAPPING_PTRf,_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                                temp_bmp, bmp_len, extract_only);
                    }
                    if (SHR_BITGET(temp_bmp, QOS_INFO(unit)->egr_dscp_hw_idx[id])) {
                        /* external reference found*/
                        QOS_UNLOCK(unit);
                        sal_free(temp_bmp);
                        return BCM_E_BUSY;
                    }
                    sal_free(temp_bmp);
                }
                /* Allocate memory for DMA */
                if (SOC_IS_TRIDENT3X(unit)) {
                    alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_DSCP *
                                  sizeof(egr_zone_2_qos_mapping_table_entry_t));
                } else {
                    alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_DSCP *
                                  sizeof(egr_dscp_table_entry_t));
                }
                egr_dscp_table = soc_cm_salloc(unit, alloc_size,
                                               "TR2 egr_dscp table");
                if (NULL == egr_dscp_table) {
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                sal_memset(egr_dscp_table, 0, alloc_size);

                /* Base index of table in hardware */
                index = (QOS_INFO(unit)->egr_dscp_hw_idx[id] *
                         _BCM_QOS_MAP_CHUNK_EGR_DSCP);

                /* Offset within table */
                cng = _BCM_COLOR_ENCODING(unit, map->color);
                offset = (map->int_pri << 2) | cng;

                /* Read the old profile chunk */
                /* For now, TD3 keeps inner and outer DSCP table the same, so we
                 * read only one table for read acces but two tables for write access
                 */
                rv = soc_mem_read_range(unit, _bcm_tr2_qos_bk_info[unit].egr_dscp_table,
                                        MEM_BLOCK_ANY,
                                        index,
                                        index + (_BCM_QOS_MAP_CHUNK_EGR_DSCP -1),
                                        egr_dscp_table);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, egr_dscp_table);
                    QOS_UNLOCK(unit);
                    return (rv);
                }

                /* Modify what's needed */
                entry = &egr_dscp_table[offset];
                if (SOC_IS_TRIDENT3X(unit)) {
                    soc_EGR_ZONE_4_QOS_MAPPING_TABLEm_field32_set(unit, entry, QOSf, map->dscp);
                } else {
                    soc_EGR_DSCP_TABLEm_field32_set(unit, entry, DSCPf, map->dscp);
                }

                /* Delete the old profile chunk */
                rv = _bcm_egr_dscp_table_entry_delete(unit, index);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, egr_dscp_table);
                    QOS_UNLOCK(unit);
                    return (rv);
                }

                /* Add new chunk and store new HW index */
                entries[0] = egr_dscp_table;
                if (SOC_IS_TRIDENT3X(unit)) {
                    /* use same setting for both Zone2 and Zone4 */
                    entries[1] = egr_dscp_table; /* for Zone2 */
                }
                rv = _bcm_egr_dscp_table_entry_add(unit, entries,
                                                   _BCM_QOS_MAP_CHUNK_EGR_DSCP,
                                                   (uint32 *)&index, 0);
                QOS_INFO(unit)->egr_dscp_hw_idx[id] = (index /
                                                      _BCM_QOS_MAP_CHUNK_EGR_DSCP);

                /* Free the DMA memory */
                soc_cm_sfree(unit, egr_dscp_table);
            }
        }
        break;
#ifdef BCM_KATANA_SUPPORT
    case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
        if (!_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->queue_offset < 0) || (map->queue_offset > 7)) { /* max no of queues =8 */
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = _BCM_QOS_MAP_CHUNK_OFFSET_MAP *
                               sizeof(ing_queue_offset_mapping_table_entry_t);
            offset_map_table = soc_cm_salloc(unit, alloc_size,
                                       "KT Offset map table");
            if (NULL == offset_map_table) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(offset_map_table, 0, alloc_size);

            /* Base index of table in hardware */
            index = QOS_INFO(unit)->offset_map_hw_idx[id] *
                                   _BCM_QOS_MAP_CHUNK_OFFSET_MAP;

            /* Offset within table */
            offset = map->queue_offset;

            /* Read the old profile chunk */
            rv = soc_mem_read_range(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm,
                                    MEM_BLOCK_ANY, index,
                                    index + (_BCM_QOS_MAP_CHUNK_OFFSET_MAP - 1),
                                    offset_map_table);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, offset_map_table);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Modify what's needed */
            entry = &offset_map_table[map->int_pri];
            soc_ING_QUEUE_OFFSET_MAPPING_TABLEm_field32_set(unit, entry,
                                           QUEUE_OFFSETf, map->queue_offset);

            /* Delete the old profile chunk */
            rv = _bcm_offset_map_table_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, offset_map_table);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = offset_map_table;
            rv = _bcm_offset_map_table_entry_add(unit, entries,
                                      _BCM_QOS_MAP_CHUNK_OFFSET_MAP,
                                                    (uint32 *)&index);
            QOS_INFO(unit)->offset_map_hw_idx[id] =
                                 index / _BCM_QOS_MAP_CHUNK_OFFSET_MAP;

            /* Free the DMA memory */
            soc_cm_sfree(unit, offset_map_table);
        }
        break;
#endif
#ifdef BCM_SABER2_SUPPORT
    case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
        if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
            if (!_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            } else {

                conf_flags = _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_GET(unit, id);
                if (flags != conf_flags) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(
                    _bcm_sb2_qos_oam_pri_map_params_check(flags, map));

                /* Allocate memory for DMA */
                alloc_size = _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP *
                    sizeof(ing_service_pri_map_0_entry_t);
                service_pri_map_table = soc_cm_salloc(unit, alloc_size,
                        "SB2 service pri map table");
                if (NULL == service_pri_map_table) {
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                sal_memset(service_pri_map_table, 0, alloc_size);

                /* Base index of table in hardware */
                index = QOS_INFO(unit)->service_pri_map_hw_idx[id] *
                    _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP;

                /* Offset within table */
                offset = map->counter_offset;

                /* Read the old profile chunk from one of the tables.*/
                
                rv = soc_mem_read_range(unit, ING_SERVICE_PRI_MAP_0m,
                        MEM_BLOCK_ANY, index,
                        index + (_BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP - 1),
                        service_pri_map_table);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, service_pri_map_table);
                    QOS_UNLOCK(unit);
                    return (rv);
                }

                /* Modify what's needed */
                rv = _bcm_sb2_service_pri_map_flags_pri_get(map, flags, &pri);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, service_pri_map_table);
                    QOS_UNLOCK(unit);
                    return (rv);
                }
                entry = &service_pri_map_table[pri];
                soc_ING_SERVICE_PRI_MAP_0m_field32_set(unit, entry,
                        OFFSETf, offset);
                soc_ING_SERVICE_PRI_MAP_0m_field32_set(unit, entry,
                        OFFSET_VALIDf, 1);
                soc_ING_SERVICE_PRI_MAP_0m_field32_set(unit, entry,
                        LM_COLORf, map->color);

                /* Delete the old profile chunk */
                rv = _bcm_service_pri_map_table_entry_delete(unit, index);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, service_pri_map_table);
                    QOS_UNLOCK(unit);
                    return (rv);
                }

                /* Add new chunk and store new HW index */
                for (table = 0; table < _BCM_SB2_OAM_PRI_MAP_TABLE_MAX; table++) {
                    mem_entries[table] = service_pri_map_table;
                }
                rv = _bcm_service_pri_map_table_entry_add(unit, mem_entries,
                        _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP,
                        (uint32 *)&index);

                QOS_INFO(unit)->service_pri_map_hw_idx[id] =
                    index / _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP;

                /* Free the DMA memory */
                soc_cm_sfree(unit, service_pri_map_table);
            }
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
#endif
#ifdef BCM_KATANA2_SUPPORT
    case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
        if (!_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->queue_offset < 0) || (map->queue_offset > 7)) { /* max no of queues =8 */
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }

            sal_memset(rqe_queue_offset_profile, 0,
                       sizeof(rqe_queue_offset_profile));

            /* Offset within table */
            offset = map->queue_offset;

            /* Read the profile chunk */
            for (i = 0; i <_BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP;
                 i++) {
                rv = READ_RQE_QUEUE_OFFSETr(unit, i,
                                   &rqe_queue_offset_profile[i]);
                if (BCM_FAILURE(rv)) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }

            hw_idx = QOS_INFO(unit)->rqe_queue_offset_hw_idx[id];
            entry = &rqe_queue_offset_profile[0];
            /* Modify what's needed */
            soc_reg_field_set(unit, RQE_QUEUE_OFFSETr,
                  &rqe_queue_offset_profile[map->int_pri], profile_field[id],
                  map->queue_offset);

            /* delete the old profile chunk or decrement the refcount*/
            if (QOS_INFO(unit)->rqe_queue_offset_refcount[hw_idx]) {
                QOS_INFO(unit)->rqe_queue_offset_refcount[hw_idx]--;
            }
            /* update the cache priority entry with new offset
             * compare function to campare is new map_id is
             * same as exisiting map id
             * if same increment reference count
             * else push the changes to the hw
             */
            _bcm_rqe_queue_offset_map_table_compare(unit, entry, id, &profile_offset);

            if (profile_offset == id) {
                if (id != hw_idx) {
                    for (i = 0; i <_BCM_QOS_MAP_CHUNK_RQE_QUEUE_OFFSET_MAP; i++) {
                         if (i != map->int_pri) {
                             soc_reg_field_set(unit, RQE_QUEUE_OFFSETr,
                                               &rqe_queue_offset_profile[i], profile_field[id],
                                               soc_reg_field_get(unit, RQE_QUEUE_OFFSETr,
                                                                 rqe_queue_offset_profile[i],
                                                                 profile_field[hw_idx]));
                         }
                         rv = WRITE_RQE_QUEUE_OFFSETr(unit, i,
                                                      rqe_queue_offset_profile[i]);
                         if (BCM_FAILURE(rv)) {
                             QOS_INFO(unit)->rqe_queue_offset_refcount[hw_idx]++;
                             QOS_UNLOCK(unit);
                             return rv;
                         }
                    }
                } else {
               /* write to hardware and increment the refcount */
               /* Add new chunk and store new HW index */
                rv = WRITE_RQE_QUEUE_OFFSETr(unit, map->int_pri,
                       rqe_queue_offset_profile[map->int_pri]);
                if (BCM_FAILURE(rv)) {
                    QOS_INFO(unit)->rqe_queue_offset_refcount[hw_idx]++;
                    QOS_UNLOCK(unit);
                    return rv;
                }
                }
                QOS_INFO(unit)->rqe_queue_offset_refcount[id]++;
                QOS_INFO(unit)->rqe_queue_offset_hw_idx[id] = id;
            } else {
                /* matching profile found
                 * delete the profile at current index
                 * Temporaryily increment the refcount for the current index.
                 * delete the profile at current index
                 * increment the refcount of the matching profile index
                 */
                QOS_INFO(unit)->rqe_queue_offset_refcount[hw_idx]++;
                rv = _bcm_rqe_queue_offset_map_table_sw_delete(unit, hw_idx);
                if (BCM_FAILURE(rv)) {
                    QOS_UNLOCK(unit);
                    return rv;
                }

                QOS_INFO(unit)->rqe_queue_offset_refcount[profile_offset]++;

                QOS_INFO(unit)->rqe_queue_offset_hw_idx[id] = profile_offset;

            }
        }
        break;
#endif

    default:
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
            bcm_mpls_exp_map_t ing_exp_map;
            bcm_mpls_exp_map_t_init(&ing_exp_map);
            if (!(flags & BCM_QOS_MAP_MPLS) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->exp > 7) || (map->exp < 0) ||
                ((map->color != bcmColorGreen) &&
                (map->color != bcmColorYellow) &&
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Call the triumph/mpls.c implementation */
            ing_exp_map.exp = map->exp;
            ing_exp_map.priority = map->int_pri;
            ing_exp_map.color = map->color;
            rv = bcm_tr_mpls_exp_map_set(unit, map_id, &ing_exp_map);
        } else
#endif /* defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT) */
#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            soc_feature(unit, soc_feature_channelized_switching)) &&
            (flags & BCM_QOS_MAP_SUBPORT)) {
            rv = bcmi_xgs5_subport_egr_subtag_dot1p_map_add(unit, map);
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit) && (flags == BCM_QOS_MAP_SUBPORT)) {
            rv = bcm_kt2_subport_egr_subtag_dot1p_map_add(unit, map);
        } else
#endif /* defined(BCM_KATANA2_SUPPORT) */
        {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    QOS_UNLOCK(unit);
    return rv;
}

/* Clear an entry from a QoS map */
/* Read the existing profile chunk, modify what's needed and add the
 * new profile. This can result in the HW profile index changing for a
 * given ID */
int
bcm_tr2_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t clr_map;

    QOS_INIT(unit);

    /* Check for incoming parameter */
    if (NULL == map) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_qos_validate_map_type(unit, map_id));
    }
#endif

    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
#endif
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.int_pri = 0;
        clr_map.color = bcmColorGreen;
        rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS:
#endif
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.pkt_pri = 0;
        clr_map.pkt_cfi = 0;
        clr_map.exp = 0;
        rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.int_pri = 0;
        clr_map.color = bcmColorGreen;
        rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.pkt_pri = 0;
        clr_map.pkt_cfi = 0;
        clr_map.dscp = 0;
        rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        break;
#ifdef BCM_KATANA_SUPPORT
    case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.queue_offset = 0;
        rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        break;
#endif
#ifdef BCM_KATANA2_SUPPORT
    case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.queue_offset = 0;
        rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        break;
#endif
#ifdef BCM_SABER2_SUPPORT
    case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
        if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
            sal_memcpy(&clr_map, map, sizeof(clr_map));
            clr_map.counter_offset = 0;
            rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        } else {
            rv = BCM_E_PARAM;
        }
        break;
#endif

    default:
#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            soc_feature(unit, soc_feature_channelized_switching)) &&
            (flags & BCM_QOS_MAP_SUBPORT)) {
            rv = bcmi_xgs5_subport_egr_subtag_dot1p_map_delete(unit, map);
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit) && (flags == BCM_QOS_MAP_SUBPORT)) {
            rv = bcm_kt2_subport_egr_subtag_dot1p_map_delete(unit, map);
        } else
#endif /* defined(BCM_KATANA2_SUPPORT) */
        {
            sal_memcpy(&clr_map, map, sizeof(clr_map));
            clr_map.int_pri = 0;
            clr_map.color = bcmColorGreen;
            rv = bcm_tr2_qos_map_add(unit, flags, &clr_map, map_id, 1);
        }
        break;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_trx_qos_vlan_port_egress_inner_pri_mapping_set
 * Purpose:
 *      Configure the IPRI mapping index to port's EGR_VLAN_CONTROL_3 register
 *      for inner vlan priority remarking through EGR_MPLS_EXP_MAPPING_2 table.
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      index      - (IN) Index to access EGR_MPLS_EXP_MAPPING_2m table.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_qos_vlan_port_egress_inner_pri_mapping_set(int unit,
                                                    bcm_port_t port,
                                                    int index)
{
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
	    int rv, alloc_size;
        uint32 rval;
        char *buf;
        void *entries[1];
        uint8 pri = 0;
        int  id=0;
        if (SOC_IS_TRIDENT3X(unit)) {
            if (SOC_MEM_IS_VALID(unit, EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m)) {
                alloc_size = 64 * sizeof(egr_zone_1_dot1p_mapping_table_3_entry_t);
                buf = soc_cm_salloc(unit, alloc_size, "TD3 EGR ZONE 1 PRI buf");
                if (NULL == buf) {
                    return (BCM_E_MEMORY);
                }
                id = (index * _BCM_QOS_MAP_CHUNK_EGR_MPLS);
                rv = soc_mem_read_range(unit, EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m, MEM_BLOCK_ANY,
                                        id,
                                        (id + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                        (void *) buf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    return rv;
                }

                /* Check if priority is configured for given QOS MAP ID profile */
                for (id = 0; id < _BCM_QOS_MAP_CHUNK_EGR_MPLS; id++) {
                    entries[0] = soc_mem_table_idx_to_pointer(unit,
                                                      EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m,
                                                      void *, buf, id);
                    pri = soc_mem_field32_get(unit, EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m,
                                              entries[0], PRIf);
                    /* Priority configured, proceed with configuration of
                     * VLAN_CONTROL_3 register, with IPRI_MAPPING_PTR and
                     * IPRI_CIF_SEL fields.
                     */
                    if (pri) {
                          break;
                    }
                }
                soc_cm_sfree(unit, buf);
            }
        } else {
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
                alloc_size = 64 * sizeof(egr_pri_cng_map_entry_t);
                buf = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls exp map2");
                if (NULL == buf) {
                    return (BCM_E_MEMORY);
                }
                id = (index * _BCM_QOS_MAP_CHUNK_EGR_MPLS);
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_2m, MEM_BLOCK_ANY,
                                        id,
                                        (id + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                        (void *) buf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    return rv;
                }

                /* Check if priority is configured for given QOS MAP ID profile */
                for (id = 0; id < _BCM_QOS_MAP_CHUNK_EGR_MPLS; id++) {
                    entries[0] = soc_mem_table_idx_to_pointer(unit,
                                                      EGR_MPLS_EXP_MAPPING_2m,
                                                      void *, buf, id);
                    pri = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_2m,
                                              entries[0], PRIf);
                    /* Priority configured, proceed with configuration of
                     * VLAN_CONTROL_3 register, with IPRI_MAPPING_PTR and
                     * IPRI_CIF_SEL fields.
                     */
                    if (pri) {
                          break;
                    }
                }
                soc_cm_sfree(unit, buf);
            }
        }

        /* Priority field is not configured in EGR_MPLS_EXP_MAPPING_2m table,
         * skip VLAN_CONTROL_3 register configuration.
         */
        if ((!pri)) {
            return BCM_E_NONE;
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            egr_vlan_control_3_entry_t entry;
            soc_field_t fields[2];
            uint32 values[2];

            if (SOC_IS_TRIDENT3X(unit)) {
                fields[0] = IPRI_MAPPING_PTRf;
                fields[1] = IPRI_CFI_SELf;
                values[0] = index;
                values[1] = 1;

                /* Write 2 field/value pair to EGR_LPORT_PROFILE */
                BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_fields_set(
                                    unit, port, EGR_VLAN_CONTROL_3m, 2, fields, values));
            } else {
                rv = READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry);
                BCM_IF_ERROR_RETURN(rv);

                soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m, &entry,
                                    IPRI_MAPPING_PTRf, index);

                soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m, &entry,
                                    IPRI_CFI_SELf, 1);
                rv = WRITE_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry);
                BCM_IF_ERROR_RETURN(rv);
            }
        } else
#endif
        {
            rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
            BCM_IF_ERROR_RETURN(rv);
            soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                    IPRI_MAPPING_PTRf, index);

            soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRI_CFI_SELf, 1);
            rv = WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * configure the L3 QoS map.
 */
STATIC int
_bcm_mim_qos_egr_map_set(int unit, int vp, int qos_map_id)
{
#if (defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3))
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
        egr_dvp_attribute_entry_t dvp_entry;
        int hw_idx;
        int rv;

        if (!((MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_NETWORK) ||
                        (MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_PEER))) {
            return BCM_E_PARAM;
        }

        sal_memset(&dvp_entry, 0, sizeof(egr_dvp_attribute_entry_t));
        MEM_LOCK(unit, mem);

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &dvp_entry);
        if (BCM_FAILURE(rv)) {
            MEM_UNLOCK(unit, mem);
            return rv;
        }

        if (qos_map_id) {
            rv = _bcm_tr2_qos_id2idx(unit, qos_map_id, &hw_idx);
            if (BCM_FAILURE(rv)) {
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            soc_mem_field32_set(unit, mem, &dvp_entry,
                                        FLEX_MIM__QOS_ACTION_SELf, 2);
            soc_mem_field32_set(unit, mem, &dvp_entry,
                                        FLEX_MIM__QOS_MAPPING_PTRf, hw_idx);
        } else {
            soc_mem_field32_set(unit, mem, &dvp_entry,
                                        FLEX_MIM__QOS_ACTION_SELf, 0);
            soc_mem_field32_set(unit, mem, &dvp_entry,
                                        FLEX_MIM__QOS_MAPPING_PTRf, 0);
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vp, &dvp_entry);
        MEM_UNLOCK(unit, mem);
        BCM_IF_ERROR_RETURN(rv);
        return rv;
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }
}

STATIC int
_bcm_mim_qos_egr_map_get(int unit, int vp, int *qos_map_id)
{
#if (defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3))
    if (soc_feature(unit, soc_feature_mim_decoupled_mode)) {
        soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
        egr_dvp_attribute_entry_t dvp_entry;
        int hw_idx;
        int rv;
        int qos_sel;

        if (!((MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_NETWORK) ||
                        (MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_PEER))) {
            return BCM_E_PARAM;
        }

        sal_memset(&dvp_entry, 0, sizeof(egr_dvp_attribute_entry_t));

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &dvp_entry);
        BCM_IF_ERROR_RETURN(rv);

        qos_sel = soc_mem_field32_get(unit, mem, &dvp_entry,
                                        FLEX_MIM__QOS_ACTION_SELf);
        if (qos_sel == 2) {
            hw_idx = soc_mem_field32_get(unit, mem, &dvp_entry,
                                        FLEX_MIM__QOS_MAPPING_PTRf);
            rv = _bcm_tr2_qos_idx2id(unit, hw_idx,
                        _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE, qos_map_id);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            return BCM_E_UNAVAIL;
        }
        return BCM_E_NONE;
    } else
#endif /* defined(BCM_TRIDENT3_SUPPORT) */
    {
        return BCM_E_UNAVAIL;
    }
}

/* Attach a QoS map to an object (Gport) */
int
bcm_tr2_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int index, id, alloc_size, rv = BCM_E_NONE, idx;
    uint8 pri, cfi;
    char *buf, *buf2;
    void *entries[1];
    soc_mem_t source_mem = INVALIDm;
    soc_field_t new_pri_field=0;
    soc_field_t new_cfi_field=0;
    uint32 rval = 0;
#ifdef INCLUDE_L3
    uint32 otag_select;
    source_vp_entry_t svp;
    egr_wlan_dvp_entry_t wlan_dvp;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int tunnel, vp, encap_id, nhi, l3_iif = 0;
    int tunnel_id_max, tunnel_count;
    egr_l3_next_hop_entry_t egr_nh;
    soc_field_t dot1p_pri_select_field=0;
    soc_field_t dot1p_mapping_ptr_field=0;
    int l3_nh_entry_type = 0;
    int vc_swap_idx;
    egr_mpls_vc_and_swap_label_table_entry_t vc_swap_entry;
    _bcm_vp_info_t vp_info;
    ing_dvp_table_entry_t dvp;
#endif

    QOS_INIT(unit);

    QOS_LOCK(unit);
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Validate given ingress map for non supported types and IDs */
        if ((ing_map > 0) &&
            (BCM_E_NONE != _bcm_th3_qos_validate_map_type(unit, ing_map))) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        /* Validate given egress map for non supported types and IDs */
        if ((egr_map > 0) &&
            (BCM_E_NONE != _bcm_th3_qos_validate_map_type(unit, egr_map))) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef INCLUDE_L3
    if (BCM_GPORT_IS_SET(port) && !BCM_GPORT_IS_MODPORT(port) &&
        !BCM_GPORT_IS_LOCAL(port)) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) && !BCM_GPORT_IS_TUNNEL(port)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

        /* Deal with different types of gports */
        if (_BCM_QOS_GPORT_IS_VFI_VP_PORT(port) ||
            BCM_GPORT_IS_VLAN_PORT(port) ||
            BCM_GPORT_IS_NIV_PORT(port) ||
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
            /* Deal with MiM and MPLS ports */
            if (BCM_GPORT_IS_VLAN_PORT(port)) {
                vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if (BCM_GPORT_IS_MIM_PORT(port)) {
                vp = BCM_GPORT_MIM_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if BCM_GPORT_IS_VXLAN_PORT(port) {
                vp = BCM_GPORT_VXLAN_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if BCM_GPORT_IS_L2GRE_PORT(port) {
                vp = BCM_GPORT_L2GRE_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if BCM_GPORT_IS_FLOW_PORT(port) {
                vp = BCM_GPORT_FLOW_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
                vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if (BCM_GPORT_IS_NIV_PORT(port)) {
                vp = BCM_GPORT_NIV_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else {
                vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            }

            if (ing_map != -1) { /* -1 means no change */
                /* TRUST_DOT1P_PTR from SOURCE_VP table */
                rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                if (ing_map == 0) {
                    /* Clear the existing map */
                    soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DOT1P_PTRf, 0);
                    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V4f)) {
                        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_V4f, 0);
                    }
                    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V6f)) {
                        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_V6f, 0);
                    }
                    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_PTRf)) {
                        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_PTRf, 0);
                    }
                } else {
                    /* Use the provided map */
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    switch (ing_map >> _BCM_QOS_MAP_SHIFT) {
                      case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
                        if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DOT1P_PTRf,
                        QOS_INFO(unit)->pri_cng_hw_idx[id]);
                        break;

                      case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
                        if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_PTRf)) {
                            soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_PTRf,
                            QOS_INFO(unit)->dscp_hw_idx[id]);
                        }
                        if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V4f)) {
                            soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_V4f, 1);
                        }

                        if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V6f)) {
                            soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_V6f, 1);
                        }
                        break;
                    }
                }
                rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
            if ((egr_map != -1) && 
                ((egr_map >> _BCM_QOS_MAP_SHIFT) ==
                          _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) &&
                 BCM_GPORT_IS_MIM_PORT(port)) {
                 BCM_IF_ERROR_RETURN(_bcm_mim_qos_egr_map_set(unit,vp,egr_map));

            } else if (egr_map != -1) { /* -1 means no change */
                /* SD_TAG::DOT1P_MAPPING_PTR from EGR_L3_NEXT_HOP */
                if (BCM_GPORT_IS_MIM_PORT(port)) {
                    rv = bcm_tr2_multicast_mim_encap_get(unit, 0, 0, port,
                                                         &encap_id);
                } else if (BCM_GPORT_IS_VXLAN_PORT(port)) {
                    rv = bcm_esw_multicast_vxlan_encap_get(unit, 0, 0, port,
                                                           &encap_id);
                } else if (BCM_GPORT_IS_FLOW_PORT(port)) {

                    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
                    if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf)) {
                        return BCM_E_UNAVAIL;
                    }
                    encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
                    encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                } else {
                    rv = bcm_tr2_multicast_vpls_encap_get(unit, 0, 0, port,
                                                          &encap_id);
                }
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit) && BCM_GPORT_IS_MIM_PORT(port)) {
                    nhi = encap_id;
                } else
#endif /* BCM_ENDURO_SUPPORT */
                {
                    nhi = encap_id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                }
                rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                  nhi, &egr_nh);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }

                if(BCM_GPORT_IS_MPLS_PORT(port)) {
                    l3_nh_entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                           &egr_nh, nh_entry_type_field[unit]);
                }

                if (BCM_GPORT_IS_MPLS_PORT(port) &&
                    (l3_nh_entry_type == 0x1)) {
                    if (soc_feature(unit, soc_feature_mpls_enhanced)) {
                        vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(
                                         unit, &egr_nh,
                                         MPLS__VC_AND_SWAP_INDEXf);
                    } else {
                        vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(
                                         unit, &egr_nh, VC_AND_SWAP_INDEXf);
                    }
                    if (vc_swap_idx > 0) {
                        rv = soc_mem_read(unit,
                                          EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                          MEM_BLOCK_ANY, vc_swap_idx,
                                          &vc_swap_entry);
                        if (rv < 0) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        if (soc_mem_field_valid(unit,
                            EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                            SD_TAG_DOT1P_PRI_SELECTf)) {
                            if (egr_map == 0) {
                                /* Clear the existing map */
                                index = 0;
                                soc_mem_field32_set(
                                    unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_swap_entry, SD_TAG_DOT1P_PRI_SELECTf,
                                    0);
                            } else {
                                /* Use the provided map */
                                if ((egr_map >> _BCM_QOS_MAP_SHIFT) !=
                                     _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                                    QOS_UNLOCK(unit);
                                    return BCM_E_PARAM;
                                }
                                id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                                if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                                    QOS_UNLOCK(unit);
                                    return BCM_E_PARAM;
                                }
                                index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                                soc_mem_field32_set(
                                    unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    &vc_swap_entry, SD_TAG_DOT1P_PRI_SELECTf,
                                    1);
                            }
                            soc_mem_field32_set(
                                unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                &vc_swap_entry, SD_TAG_DOT1P_MAPPING_PTRf,
                                index);
                            rv = soc_mem_write(
                                    unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                    MEM_BLOCK_ALL, vc_swap_idx, &vc_swap_entry);
                            if (rv < 0) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                        }
                    }
                } else {
                    /*  For Katana chip, bit position for DOT1P_PRI_SELECT and
                        DOT1P_MAPPING_PTR fields are different in MIM and SD_TAG
                        views hence below adding below expandable logic for
                        selecting appropriate fields.
                     */
                    if (BCM_GPORT_IS_MIM_PORT(port) &&
                        ((MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_NETWORK) ||
                        (MIM_INFO(unit)->port_info[vp].flags &
                        _BCM_MIM_PORT_TYPE_PEER))) {
                        if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                MIM__DOT1P_PRI_SELECTf) &&
                            soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                MIM__DOT1P_MAPPING_PTRf)) {
                            dot1p_pri_select_field = MIM__DOT1P_PRI_SELECTf;
                            dot1p_mapping_ptr_field = MIM__DOT1P_MAPPING_PTRf;
                        } else {
                            QOS_UNLOCK(unit);
                            return BCM_E_INTERNAL;
                        }
                    } else if (BCM_GPORT_IS_VXLAN_PORT(port) ||
                               BCM_GPORT_IS_FLOW_PORT(port)){
                        rv = _bcm_vp_info_get(unit, vp, &vp_info);
                        if (rv < 0) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        if (!(vp_info.flags & _BCM_VP_INFO_NETWORK_PORT)) {
#if defined(BCM_TRIDENT3_SUPPORT)
                            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                                dot1p_pri_select_field =
                                    L2_OTAG__PCP_DE_SELf;
                                dot1p_mapping_ptr_field =
                                    L2_OTAG__PCP_DE_MAPPING_PTRf;
                            } else
#endif
                            {
                                dot1p_pri_select_field =
                                    SD_TAG__SD_TAG_DOT1P_PRI_SELECTf;
                                dot1p_mapping_ptr_field =
                                    SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf;
                            }
                        } else {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                    } else {
#if defined(BCM_TRIDENT3_SUPPORT)
                        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                            dot1p_pri_select_field =
                                L2_OTAG__PCP_DE_SELf;
                            dot1p_mapping_ptr_field =
                                L2_OTAG__PCP_DE_MAPPING_PTRf;
                        } else
#endif
                        {
                            dot1p_pri_select_field =
                                SD_TAG__SD_TAG_DOT1P_PRI_SELECTf;
                            dot1p_mapping_ptr_field =
                                SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf;
                        }
                    }

                    if (egr_map == 0) {
                        /* Clear the existing map */
                        index = 0;
                        otag_select = DEFAULT_VIEW_OTAG_FROM_TABLE;
#if defined(BCM_TRIDENT3_SUPPORT)
                        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
                            (dot1p_pri_select_field == L2_OTAG__PCP_DE_SELf)) {
                            otag_select = L2_OTAG_VIEW_OTAG_FROM_TABLE;
                        }
                        if (BCM_GPORT_IS_MIM_PORT(port) &&
                            soc_feature(unit, soc_feature_mim_decoupled_mode)) {
                            BCM_IF_ERROR_RETURN(_bcm_mim_qos_egr_map_set(unit,
                                                vp,egr_map));
                        }
#endif
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            dot1p_pri_select_field, otag_select);
                    } else {
                        /* Use the provided map */
                        if ((egr_map >> _BCM_QOS_MAP_SHIFT) !=
                             _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                        otag_select = DEFAULT_VIEW_OTAG_FROM_QOS_MAP;
#if defined(BCM_TRIDENT3_SUPPORT)
                        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
                            (dot1p_pri_select_field == L2_OTAG__PCP_DE_SELf)) {
                            otag_select = L2_OTAG_VIEW_OTAG_FROM_QOS_MAP;
                        }
#endif
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            dot1p_pri_select_field, otag_select);
                    }
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                        dot1p_mapping_ptr_field, index);
                    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                                       nhi, &egr_nh);
                    if (rv < 0) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        } else if (BCM_GPORT_IS_WLAN_PORT(port)) {
            /* Deal with WLAN ports */
            vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                QOS_UNLOCK(unit);
                return BCM_E_BADID;
            }
            if (ing_map != -1) { /* -1 means no change */
                if (ing_map == 0) {
                    /* Clear the existing map */
                    index = 0;
                } else {
                    /* Use the provided map */
                    if ((ing_map >> _BCM_QOS_MAP_SHIFT) !=
                         _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->pri_cng_hw_idx[id];
                }
#if defined(BCM_TRIUMPH3_SUPPORT)
                if (SOC_IS_TRIUMPH3(unit)) {
                    rv = bcm_tr3_wlan_lport_field_set(unit, port,
                                                      TRUST_DOT1P_PTRf, index);
                } else
#endif /* BCM_TRIUMPH3_SUPPORT */
                {
                    rv = bcm_tr2_wlan_lport_field_set(unit, port,
                                                      TRUST_DOT1P_PTRf, index);
                }
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }

            if (egr_map != -1) { /* -1 means no change */
#if defined(BCM_TRIUMPH3_SUPPORT)
                if (SOC_IS_TRIUMPH3(unit)) {
                    if (egr_map == 0) {
                        /* Clear the existing map */
                        index = 0;
                    } else {
                        if ((egr_map >> _BCM_QOS_MAP_SHIFT) !=
                             _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                    }

                    rv = _bcm_tr3_qos_wlan_port_map_set(
                                            unit, port, egr_map, index);
                    if (rv < 0) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                } else
#endif /* BCM_TRIUMPH3_SUPPORT */
                {
                    /* EGR_WLAN_DVP.TUNNEL_INDEX points to CAPWAP initiator.
                     * EGR_IP_TUNNEL.DOT1P_MAPPING_PTR contains the map */
                    rv = READ_EGR_WLAN_DVPm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp);
                    if (rv < 0) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                    tunnel = soc_EGR_WLAN_DVPm_field32_get(unit, &wlan_dvp,
                                                           TUNNEL_INDEXf);
                    rv = soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                                      tunnel, tnl_entry);
                    if (rv < 0) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                    if (egr_map == 0) {
                        /* Clear the existing map */
                        index = 0;
                        soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,
                                            DOT1P_PRI_SELECTf, 0);
                    } else {
                        /* Use the provided map */
                        if ((egr_map >> _BCM_QOS_MAP_SHIFT) !=
                             _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                            QOS_UNLOCK(unit);
                            return BCM_E_PARAM;
                        }
                        index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                        soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,
                                            DOT1P_PRI_SELECTf, 1);
                    }
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,
                                        DOT1P_MAPPING_PTRf, index);
                    rv = soc_mem_write(unit, EGR_IP_TUNNELm, MEM_BLOCK_ALL,
                                       tunnel, tnl_entry);
                    if (rv < 0) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        } else if (BCM_GPORT_IS_TUNNEL(port)) {
            /* Deal with tunnel initiators and terminators */
            tunnel = BCM_GPORT_TUNNEL_ID_GET(port);

            if (ing_map != -1) { /* -1 means no change */

                if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                /* Get L3_IIF from L3_TUNNEL.
                 * Get TRUST_DSCP_PTR from L3_IIF */
                tunnel_id_max = soc_mem_index_max(unit, L3_TUNNELm);
                tunnel_count = soc_mem_index_count(unit, L3_TUNNELm);
                alloc_size = tunnel_count * sizeof(tnl_entry);
                buf = soc_cm_salloc(unit, alloc_size, "TR2 L3 TUNNEL");
                if (NULL == buf) {
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                rv = soc_mem_read_range(unit, L3_TUNNELm, MEM_BLOCK_ANY,
                                        0, tunnel_id_max, (void *)buf);
                if (rv < 0) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }

                    for (index = 0; index < tunnel_id_max; index++)
                    {
                        entries[0] = soc_mem_table_idx_to_pointer(unit, L3_TUNNELm,
                                                                  void *, buf,
                                                                  index);
                        if (tunnel == soc_mem_field32_get(unit, L3_TUNNELm,
                                                          entries[0], TUNNEL_IDf)) {
                            /* Tunnel found - get L3_IIF */
                            l3_iif = soc_mem_field32_get(unit, L3_TUNNELm,
                                                         entries[0], L3_IIFf);
                            break;
                        }
                    }

                soc_cm_sfree(unit, buf);
                if (index == tunnel_id_max) {
                    QOS_UNLOCK(unit);
                    return BCM_E_NOT_FOUND; /* Tunnel not found */
                }
                } else {
                    QOS_UNLOCK(unit);
                    return BCM_E_UNAVAIL;
                }

                if (ing_map == 0) {
                    /* Clear the existing map */
                    index = 0;
                } else {
                    /* Use the provided map */
                    if ((ing_map >> _BCM_QOS_MAP_SHIFT) !=
                         _BCM_QOS_MAP_TYPE_DSCP_TABLE) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->dscp_hw_idx[id];
                }
                if (soc_mem_field_valid(unit, L3_IIFm, TRUST_DSCP_PTRf)) {
                    rv = soc_mem_field32_modify(unit, L3_IIFm, l3_iif,
                                            TRUST_DSCP_PTRf, index);
                }
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
            if (egr_map != -1) { /* -1 means no change */
                /* DSCP_MAPPING_PTR from EGR_IP_TUNNEL */
                rv = soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                                  tunnel, tnl_entry);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                if (egr_map == 0) {
                    /* Clear existing map */
                    index = 0;
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,
                                        DSCP_SELf, 0);
                } else {
                    /* Use the provided map */
                    if ((egr_map >> _BCM_QOS_MAP_SHIFT) !=
                         _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->egr_dscp_hw_idx[id];
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,
                                        DSCP_SELf, 2);
                }
                soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,
                                    DSCP_MAPPING_PTRf, index);
                rv = soc_mem_write(unit, EGR_IP_TUNNELm, MEM_BLOCK_ALL,
                                   tunnel, tnl_entry);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
        } else if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(BCM_HGPROXY_COE_SUPPORT)
            if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                soc_feature(unit, soc_feature_channelized_switching)) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                if (ing_map != -1) { /* -1 means no change */
                    /* Make the port's TRUST_DOT1_PTR point to the profile index */
                    if (ing_map == 0) {
                        /* Clear the existing map */
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                                   TRUST_DOT1P_PTRf, 0);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    } else {
                        id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                        switch (ing_map >> _BCM_QOS_MAP_SHIFT) {
                          case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
                            if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                                QOS_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }
                            if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                                QOS_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }
                            rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                                   TRUST_DOT1P_PTRf,
                                                   QOS_INFO(unit)->pri_cng_hw_idx[id]);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            break;
                          default:
                            rv = BCM_E_CONFIG;
                        }
                    }
                }

                if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                    if (egr_map != -1) { /* -1 means no change */
                        if (egr_map == 0) {
                            /* Clear the existing map */
                            rv = _bcm_esw_egr_port_tab_set(unit,
                                                            port,
                                                            EGR_QOS_PROFILE_INDEXf,
                                                            0);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                        } else {
                            if ((egr_map >> _BCM_QOS_MAP_SHIFT) != _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) {
                                QOS_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }
                            id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                            if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                                QOS_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }

                            index = QOS_INFO(unit)->egr_dscp_hw_idx[id];
                            rv = _bcm_esw_egr_port_tab_set(unit,
                                                        port,
                                                        EGR_QOS_PROFILE_INDEXf,
                                                        index);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                        }
                    }
                }
            }
#endif
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    } else
#endif
    {
        /* Deal with physical ports */
        if (BCM_GPORT_IS_SET(port)) {
            if (bcm_esw_port_local_get(unit, port, &port) < 0) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        }
        if (!SOC_PORT_VALID(unit, port)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        if (ing_map != -1) { /* -1 means no change */
            /* Make the port's TRUST_DOT1_PTR point to the profile index */
            if (ing_map == 0) {
                /* Clear the existing map */
                if (SOC_IS_TRIDENT3X(unit)) {
                    PORT_LOCK(unit);
                    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                               TRUST_DOT1P_PTRf, 0);
                    if (BCM_SUCCESS(rv)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   TRUST_DSCP_V4f, 0);
                    }
                    if (BCM_SUCCESS(rv)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   TRUST_DSCP_V6f, 0);
                    }
                    if (BCM_SUCCESS(rv)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   TRUST_DSCP_PTRf, 0);
                    }
                    PORT_UNLOCK(unit);
                } else {
                    soc_mem_lock(unit, PORT_TABm);
#ifdef BCM_SABER2_SUPPORT
                    if (SOC_IS_SABER2(unit)) {
                        rv = _bcm_tr2_port_tab_get(unit, port, TRUST_DOT1P_PTRf, &idx);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        /* coverity[overrun-local:FALSE] */
                        _BCM_QOS_TRUST_DOT1P_PTR_CACHE_CLEAR(unit, port, idx);
                    }
#endif

                    rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DOT1P_PTRf, 0);
                    if (BCM_SUCCESS(rv) &&
                        soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_V4f)) {
                        rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DSCP_V4f, 0);
                    }
                    if (BCM_SUCCESS(rv) &&
                        soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_V6f)) {
                        rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DSCP_V6f, 0);
                    }
                    if (BCM_SUCCESS(rv) &&
                        soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_PTRf)) {
                        rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DSCP_PTRf, 0);
                    }
                    soc_mem_unlock(unit, PORT_TABm);
                }
            } else {
                id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                switch (ing_map >> _BCM_QOS_MAP_SHIFT) {
                  case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
                    if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    if (SOC_IS_TRIDENT3X(unit)) {
                        PORT_LOCK(unit);
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER, TRUST_DOT1P_PTRf,
                                               QOS_INFO(unit)->pri_cng_hw_idx[id]);
                        PORT_UNLOCK(unit);
                    } else {
#ifdef BCM_SABER2_SUPPORT
                        if (SOC_IS_SABER2(unit)) {
                            rv = _bcm_tr2_port_tab_get(unit, port,TRUST_DOT1P_PTRf, &idx);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            /* coverity[overrun-local:FALSE] */
                            _BCM_QOS_TRUST_DOT1P_PTR_CACHE_CLEAR(unit, port, idx/
                                                                _BCM_QOS_MAP_CHUNK_PRI_CNG);
                            /* coverity[overrun-local:FALSE] */
                            _BCM_QOS_TRUST_DOT1P_PTR_CACHE_SET(unit, port,
                                                              QOS_INFO(unit)->pri_cng_hw_idx[id]);
                        }
#endif
                        soc_mem_lock(unit, PORT_TABm);
                        rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DOT1P_PTRf,
                                               QOS_INFO(unit)->pri_cng_hw_idx[id]);
                        soc_mem_unlock(unit, PORT_TABm);
                    }
                    break;
                  case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
                    if (SOC_IS_KATANA(unit)) {
                        /* TRUST_DSCP_PTR is already reserved for katana. So don't overwrite
                         * Return error if user is tyring to set some other map other than
                         * the default one.
                         */
                        soc_mem_lock(unit, PORT_TABm);
                        rv = _bcm_tr2_port_tab_get(unit, port,TRUST_DSCP_PTRf, &idx);
                        if (ing_map != idx) {
                            rv = BCM_E_CONFIG;
                        }
                        soc_mem_unlock(unit, PORT_TABm);
                    } else if (soc_mem_field_valid(unit,PORT_TABm,
                                                   TRUST_DSCP_PTRf)) {
                        soc_mem_lock(unit, PORT_TABm);
                        rv = _bcm_tr2_port_tab_set(
                                unit, port, TRUST_DSCP_PTRf,
                                QOS_INFO(unit)->dscp_hw_idx[id]);
                        soc_mem_unlock(unit, PORT_TABm);
                    } else if (SOC_IS_TRIDENT3X(unit)) {
                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   TRUST_DSCP_PTRf, QOS_INFO(unit)->dscp_hw_idx[id]);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }

                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   TRUST_DSCP_V4f, 1);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }

                        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_BOTH,
                                                   TRUST_DSCP_V6f, 1);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    } else if (SOC_IS_TRIDENT(unit)) {
                        /* Allocate memory for DMA */
                        alloc_size = _BCM_QOS_MAP_CHUNK_DSCP *
                                     sizeof(dscp_table_entry_t);
                        buf = soc_cm_salloc(unit, alloc_size,
                                            "TR2 dscp table");
                        if (NULL == buf) {
                            QOS_UNLOCK(unit);
                            return BCM_E_MEMORY;
                        }
                        sal_memset(buf, 0, alloc_size);
                        /* Base index of table in hardware */
                        index = QOS_INFO(unit)->dscp_hw_idx[id] *
                                _BCM_QOS_MAP_CHUNK_DSCP;
                        /* Read the profile chunk */
                        rv = soc_mem_read_range(
                                 unit, _bcm_tr2_qos_bk_info[unit].dscp_table,
                                 MEM_BLOCK_ANY, index,
                                 index + _BCM_QOS_MAP_CHUNK_DSCP - 1,
                                 buf);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        /* Write the port's chunk */
                        rv = soc_mem_write_range(
                                 unit, _bcm_tr2_qos_bk_info[unit].dscp_table,
                                 MEM_BLOCK_ANY,
                                 port * _BCM_QOS_MAP_CHUNK_DSCP,
                                 (port + 1) * _BCM_QOS_MAP_CHUNK_DSCP - 1,
                                 buf);
                        soc_cm_sfree(unit, buf);
                    }

                    soc_mem_lock(unit, PORT_TABm);
                    if (BCM_SUCCESS(rv) &&
                        soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_V4f)) {
                        rv = _bcm_tr2_port_tab_set(unit, port,
                                                   TRUST_DSCP_V4f, 1);
                    }
                    if (BCM_SUCCESS(rv) &&
                        soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_V6f)) {
                        rv = _bcm_tr2_port_tab_set(unit, port,
                                                   TRUST_DSCP_V6f, 1);
                    }
                    soc_mem_unlock(unit, PORT_TABm);
                    break;
#ifdef BCM_HURRICANE3_SUPPORT
                  case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
                    if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    if (!soc_feature(unit, soc_feature_miml)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    soc_mem_lock(unit, PORT_TABm);
                    rv = _bcm_tr2_port_tab_set(unit, port, MIML_DOT1P_MAPPING_PTRf,
                                           QOS_INFO(unit)->pri_cng_hw_idx[id]);
                    soc_mem_unlock(unit, PORT_TABm);
                    break;
#endif
                  default:
                    rv = BCM_E_CONFIG;
                }
            }
        }
        if (egr_map != -1) { /* -1 means no change */
            alloc_size = 64 * sizeof(egr_pri_cng_map_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr pri cng map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(buf, 0, alloc_size);
            index = port << 6;
            if (egr_map == 0) {
                /* Clear the existing map */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                   rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_QOS_PROFILE_INDEXf, 0);
                   if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                } else if (SOC_IS_TRIDENT3X(unit)) {
                   /* Reset L3 pointer */
                   rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_QOS_PROFILE_INDEXf, 0);
                   if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                   /* Reset L2 pointer */
                   rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_DOT1P_PROFILE_INDEXf, 0);
                   if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                } else
#endif
                {
                    rv = soc_mem_write_range(unit, EGR_PRI_CNG_MAPm, MEM_BLOCK_ALL,
                                             index, index + 63, (void *)buf);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                }
                if ( SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                     (SOC_IS_TD_TT(unit) && !SOC_IS_TOMAHAWK3(unit) )) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                    if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                        soc_mem_t mem = EGR_VLAN_CONTROL_3m;
                        if (soc_feature(unit, soc_feature_egr_all_profile)) {
                            soc_field_t fields[2] = {
                                IPRI_MAPPING_PTRf, IPRI_CFI_SELf
                            };
                            uint32 values[2] = {0, 0};
                            BCM_IF_ERROR_RETURN(
                                bcm_esw_port_egr_lport_fields_set(unit,
                                port, mem, 2, fields, values));
                        } else {
                            egr_vlan_control_3_entry_t entry;

                            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                              port, &entry);
                            if (BCM_FAILURE(rv)) {
                                soc_cm_sfree(unit, buf);
                                QOS_UNLOCK(unit);
                                return rv;
                            }

                            soc_mem_field32_set(unit, mem, &entry,
                                                IPRI_MAPPING_PTRf, 0);
                            soc_mem_field32_set(unit, mem, &entry,
                                                IPRI_CFI_SELf, 0);
                            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                               port, &entry);
                            if (BCM_FAILURE(rv)) {
                                soc_cm_sfree(unit, buf);
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                        }
                    } else
#endif
                    {
                        /* Clear existing map from port EGR_VLAN_CONTROL_3 register */
                        rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }

                        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                                IPRI_MAPPING_PTRf, 0);
                        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                                IPRI_CFI_SELf, 0);
                        rv = WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    }
                }
            } else {
                if (((egr_map >> _BCM_QOS_MAP_SHIFT) != _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) &&
                   ((egr_map >> _BCM_QOS_MAP_SHIFT) != _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
                id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id) &&
                    !_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                    if ((egr_map >> _BCM_QOS_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) {
                        id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                        index = QOS_INFO(unit)->egr_dscp_hw_idx[id];
                        rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_QOS_PROFILE_INDEXf, index);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    }
                } else if (SOC_IS_TRIDENT3X(unit)) {
                    /* TD3 can not support egr_qos_combo_profile, but it needs
                       to program the same port reference pointer */
                    if ((egr_map >> _BCM_QOS_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                        id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                        index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                        rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_DOT1P_PROFILE_INDEXf, index);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    } else if ((egr_map >> _BCM_QOS_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) {
                        id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                        index = QOS_INFO(unit)->egr_dscp_hw_idx[id];
                        rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_QOS_PROFILE_INDEXf, index);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    }
                } else
#endif
                {
                    /* Copy the corresponding chunk from EGR_MPLS profiles to the
                     * EGR_PRI_CNG_MAP table, which is directly indexed by port */
                    buf2 = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls exp map");
                    if (NULL == buf2) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return (BCM_E_MEMORY);
                    }
                    sal_memset(buf2, 0, alloc_size);
                    index = (QOS_INFO(unit)->egr_mpls_hw_idx[id] *
                             _BCM_QOS_MAP_CHUNK_EGR_MPLS);

                    if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                        source_mem = EGR_MPLS_EXP_MAPPING_1m;
                        new_pri_field = PRIf;
                        new_cfi_field = CFIf;
                    } else {
                        source_mem = EGR_MPLS_PRI_MAPPINGm;
                        new_pri_field = NEW_PRIf;
                        new_cfi_field = NEW_CFIf;
                    }
                    rv = soc_mem_read_range(unit, source_mem,
                                            MEM_BLOCK_ANY, index, index + 63,
                                            (void *)buf2);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        soc_cm_sfree(unit, buf2);
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                    for (index = 0; index < _BCM_QOS_MAP_CHUNK_EGR_MPLS; index++) {
                        entries[0] = soc_mem_table_idx_to_pointer(unit,
                                                          source_mem,
                                                          void *, buf2, index);
                        pri = soc_mem_field32_get(unit, source_mem,
                                                  entries[0], new_pri_field);
                        cfi = soc_mem_field32_get(unit, source_mem,
                                                  entries[0], new_cfi_field);
                        entries[0] = soc_mem_table_idx_to_pointer(unit,
                                                  EGR_PRI_CNG_MAPm,
                                                  void *, buf, index);
                        soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm,
                                            entries[0], PRIf, pri);
                        soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm,
                                            entries[0], CFIf, cfi);
                    }

                    index = port << 6;
                    rv = soc_mem_write_range(unit, EGR_PRI_CNG_MAPm, MEM_BLOCK_ALL,
                                             index, index + 63, (void *)buf);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        soc_cm_sfree(unit, buf2);
                        QOS_UNLOCK(unit);
                        return rv;
                    }
                    soc_cm_sfree(unit, buf2);
                }
                if ((egr_map >> _BCM_QOS_MAP_SHIFT) == _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                    if ( SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                         (SOC_IS_TD_TT(unit) && !SOC_IS_TOMAHAWK3(unit))) {

                        /*  Add IPRI_MAPPING_PTR to EGR_VLAN_CONTROL_3 register for
                         *  inner vlan priority remarking to access appropriate
                         *  index from EGR_MPLS_EXP_MAPPING_2m table.
                         */
                        index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                        rv = _bcm_trx_qos_vlan_port_egress_inner_pri_mapping_set(unit, port, index);

                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    }
                }
            }
            soc_cm_sfree(unit, buf);
        }
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    QOS_UNLOCK(unit);
    return rv;
}



int
bcm_tr2_qos_port_map_type_get(int unit, bcm_gport_t port, uint32 flags,
                              int *map_id)
{
    int rv = BCM_E_NONE;
    uint32 rval = 0;
    int32 profile_idx = -1;
    int32 trust_dscp_v4 = 0;
    int32 trust_dscp_v6 = 0;
    int32 use_profile;
#ifdef INCLUDE_L3
    int32 use_profile_from_map = 0;
    source_vp_entry_t svp;
    egr_wlan_dvp_entry_t wlan_dvp;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int tunnel, vp, encap_id, nhi, l3_iif = 0;
    int tunnel_id_max, tunnel_count;
    egr_l3_next_hop_entry_t egr_nh;
    soc_field_t dot1p_pri_select_field=0;
    soc_field_t dot1p_mapping_ptr_field=0;
    int l3_nh_entry_type = 0;
    int vc_swap_idx;
    egr_mpls_vc_and_swap_label_table_entry_t vc_swap_entry;
    _bcm_vp_info_t vp_info;
    ing_dvp_table_entry_t dvp;
#endif
    int index;
    int alloc_size;
    void *entries[1];
    soc_mem_t source_mem = INVALIDm;
    soc_field_t new_pri_field = 0;
    soc_field_t new_cfi_field = 0;
    int id, num_map;
    uint8 pri, cfi, new_pri, new_cfi;
    char *buf, *buf2;

    if (map_id == NULL) {
        return BCM_E_PARAM;
    }

    flags &= (~(BCM_QOS_MAP_WITH_ID | BCM_QOS_MAP_REPLACE));

    QOS_INIT(unit);
    QOS_LOCK(unit);

#ifdef INCLUDE_L3
    if (BCM_GPORT_IS_SET(port) && !BCM_GPORT_IS_MODPORT(port) &&
        !BCM_GPORT_IS_LOCAL(port)) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) && !BCM_GPORT_IS_TUNNEL(port)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

        /* Deal with different types of gports */
        if (_BCM_QOS_GPORT_IS_VFI_VP_PORT(port) ||
            BCM_GPORT_IS_VLAN_PORT(port) ||
            BCM_GPORT_IS_NIV_PORT(port) ||
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
            /* Deal with MiM and MPLS ports */
            if (BCM_GPORT_IS_VLAN_PORT(port)) {
                vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if (BCM_GPORT_IS_MIM_PORT(port)) {
                vp = BCM_GPORT_MIM_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if BCM_GPORT_IS_VXLAN_PORT(port) {
                vp = BCM_GPORT_VXLAN_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if BCM_GPORT_IS_L2GRE_PORT(port) {
                vp = BCM_GPORT_L2GRE_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if BCM_GPORT_IS_FLOW_PORT(port) {
                vp = BCM_GPORT_FLOW_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
                vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else if (BCM_GPORT_IS_NIV_PORT(port)) {
                vp = BCM_GPORT_NIV_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            } else {
                vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                }
            }

            if ((flags & BCM_QOS_MAP_INGRESS) == BCM_QOS_MAP_INGRESS) {
                flags &= ~BCM_QOS_MAP_INGRESS;
                rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
                if (BCM_FAILURE(rv)) {
                    QOS_UNLOCK(unit);
                    return rv;
                }

                if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                    flags &= ~BCM_QOS_MAP_L2;
                    profile_idx = soc_SOURCE_VPm_field32_get(unit,
                                      &svp, TRUST_DOT1P_PTRf);
                    rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                            _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP, map_id);
                } else if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                    flags &= ~BCM_QOS_MAP_L3;
                    if (soc_mem_field_valid(unit, SOURCE_VPm, TRUST_DSCP_V4f)) {
                       trust_dscp_v4 = soc_SOURCE_VPm_field32_get(unit, &svp, TRUST_DSCP_V4f);
                    }
                    if (soc_mem_field_valid(unit, SOURCE_VPm, TRUST_DSCP_V6f)) {
                        trust_dscp_v6 = soc_SOURCE_VPm_field32_get(unit, &svp, TRUST_DSCP_V6f);
                    }
                    /* if DSCP trust enabled */
                    if ((trust_dscp_v4 != 0) || (trust_dscp_v6 != 0)) {
                        if (soc_mem_field_valid(unit, SOURCE_VPm, TRUST_DSCP_PTRf)) {
                            profile_idx = soc_SOURCE_VPm_field32_get(unit, &svp,
                                            TRUST_DSCP_PTRf);
                            rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                    _BCM_QOS_MAP_TYPE_DSCP_TABLE, map_id);
                        } else {
                            rv = BCM_E_INTERNAL;
                        }
                    } else {
                        rv = BCM_E_NOT_FOUND;
                    }
                } else {
                    rv = BCM_E_PARAM;
                }
                /* invalid flags */
                if (flags) {
                    rv = BCM_E_PARAM;
                }
                /* could return with map_id now */
                QOS_UNLOCK(unit);
                return rv;
            } else if ((flags & BCM_QOS_MAP_EGRESS) == BCM_QOS_MAP_EGRESS) {
                flags &= ~BCM_QOS_MAP_EGRESS;
                if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                    flags &= ~BCM_QOS_MAP_L2;

                    /* SD_TAG::DOT1P_MAPPING_PTR from EGR_L3_NEXT_HOP */
                    if (BCM_GPORT_IS_MIM_PORT(port)) {
                        rv = bcm_tr2_multicast_mim_encap_get(unit, 0, 0, port,
                                                             &encap_id);
                    } else if (BCM_GPORT_IS_VXLAN_PORT(port)) {
                        rv = bcm_esw_multicast_vxlan_encap_get(unit, 0, 0, port,
                                                               &encap_id);
                    } else if (BCM_GPORT_IS_FLOW_PORT(port)) {

                        BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
                        if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf)) {
                            return BCM_E_UNAVAIL;
                        }
                        encap_id = (bcm_if_t) soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
                        encap_id += BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

                    } else {
                        rv = bcm_tr2_multicast_vpls_encap_get(unit, 0, 0, port,
                                                              &encap_id);
                    }
                    if (BCM_FAILURE(rv)) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }
#if defined(BCM_ENDURO_SUPPORT)
                    if (SOC_IS_ENDURO(unit) && BCM_GPORT_IS_MIM_PORT(port)) {
                        nhi = encap_id;
                    } else
#endif /* BCM_ENDURO_SUPPORT */
                    {
                        nhi = encap_id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    }
                    rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nhi, &egr_nh);
                    if (BCM_FAILURE(rv)) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }

                    if (BCM_GPORT_IS_MPLS_PORT(port)) {
                        l3_nh_entry_type = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                               &egr_nh, nh_entry_type_field[unit]);
                    }

                    if (BCM_GPORT_IS_MPLS_PORT(port) &&
                        (l3_nh_entry_type == 0x1)) {
                        if (soc_feature(unit, soc_feature_mpls_enhanced)) {
                             vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(
                                              unit, &egr_nh,
                                              MPLS__VC_AND_SWAP_INDEXf);
                        } else {
                             vc_swap_idx = soc_EGR_L3_NEXT_HOPm_field32_get(
                                              unit, &egr_nh,
                                              VC_AND_SWAP_INDEXf);
                        }
                        if (vc_swap_idx > 0) {
                            rv = soc_mem_read(unit,
                                              EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                              MEM_BLOCK_ANY, vc_swap_idx,
                                              &vc_swap_entry);
                            if (rv < 0) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            if (soc_mem_field_valid(unit,
                                EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                SD_TAG_DOT1P_PRI_SELECTf)) {
                                use_profile =
                                    soc_mem_field32_get(
                                        unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_swap_entry,
                                        SD_TAG_DOT1P_PRI_SELECTf);
                                if (use_profile == 1) {
                                    profile_idx = soc_mem_field32_get(
                                        unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm,
                                        &vc_swap_entry,
                                        SD_TAG_DOT1P_MAPPING_PTRf);
                                    rv = _bcm_tr2_qos_idx2id(
                                            unit, profile_idx,
                                            _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                                            map_id);
                                } else {
                                    rv = BCM_E_NOT_FOUND;
                                }
                            }
                        }
                    } else {
                        /*
                         * For Katana chip, bit position for DOT1P_PRI_SELECT and
                         * DOT1P_MAPPING_PTR fields are different in MIM and SD_TAG
                         * views.
                         */
                        if (BCM_GPORT_IS_MIM_PORT(port) &&
                            ((MIM_INFO(unit)->port_info[vp].flags &
                            _BCM_MIM_PORT_TYPE_NETWORK) ||
                            (MIM_INFO(unit)->port_info[vp].flags &
                            _BCM_MIM_PORT_TYPE_PEER))) {
                            if (soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                    MIM__DOT1P_PRI_SELECTf) &&
                                soc_mem_field_valid(unit, EGR_L3_NEXT_HOPm,
                                                    MIM__DOT1P_MAPPING_PTRf)) {
                                dot1p_pri_select_field = MIM__DOT1P_PRI_SELECTf;
                                dot1p_mapping_ptr_field =
                                    MIM__DOT1P_MAPPING_PTRf;
                            } else {
                                QOS_UNLOCK(unit);
                                return BCM_E_INTERNAL;
                            }
                        } else if (BCM_GPORT_IS_VXLAN_PORT(port) ||
                                   BCM_GPORT_IS_FLOW_PORT(port)) {
                            rv = _bcm_vp_info_get(unit, vp, &vp_info);
                            if (rv < 0) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            if (!(vp_info.flags & _BCM_VP_INFO_NETWORK_PORT)) {
#if defined(BCM_TRIDENT3_SUPPORT)
                                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                                    dot1p_pri_select_field =
                                        L2_OTAG__PCP_DE_SELf;
                                    dot1p_mapping_ptr_field =
                                        L2_OTAG__PCP_DE_MAPPING_PTRf;
                                } else
#endif
                                {
                                    dot1p_pri_select_field =
                                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf;
                                    dot1p_mapping_ptr_field =
                                        SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf;
                                }
                            } else {
                                QOS_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }
                        } else {
                            dot1p_pri_select_field =
                                SD_TAG__SD_TAG_DOT1P_PRI_SELECTf;
                            dot1p_mapping_ptr_field =
                                SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf;
                        }

                        use_profile_from_map = 0;
                        use_profile = soc_mem_field32_get(
                                         unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                         dot1p_pri_select_field);
#if defined(BCM_TRIDENT3_SUPPORT)
                        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
                            (dot1p_pri_select_field == L2_OTAG__PCP_DE_SELf)) {
                            if (use_profile == L2_OTAG_VIEW_OTAG_FROM_QOS_MAP) {
                                use_profile_from_map = 1;
                            } else if (use_profile == L2_OTAG_VIEW_OTAG_FROM_TABLE) {
                                use_profile_from_map = 0;
                            }
                        } else
#endif
                        {
                            if (use_profile == DEFAULT_VIEW_OTAG_FROM_QOS_MAP) {
                                use_profile_from_map = 1;
                            }
                        }

                        if (use_profile_from_map == 1) {
                            profile_idx = soc_mem_field32_get(
                                             unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                             dot1p_mapping_ptr_field);
                            rv = _bcm_tr2_qos_idx2id(
                                    unit, profile_idx,
                                    _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, map_id);
                        } else {
                            rv = BCM_E_NOT_FOUND;
                        }
                    }
                } else if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                    flags &= ~BCM_QOS_MAP_L3;
                    if (BCM_GPORT_IS_MIM_PORT(port)) {
                       rv = _bcm_mim_qos_egr_map_get(unit, vp, map_id);
                    } else {
                       rv = BCM_E_UNAVAIL;
                    }
                }else {
                    rv = BCM_E_PARAM;
                }
                /* invalid flags */
                if (flags) {
                    rv = BCM_E_PARAM;
                }

                QOS_UNLOCK(unit);
                return rv;
            } else {
                QOS_UNLOCK(unit);
                rv = BCM_E_PARAM;
                return rv;
            }
        } else if (BCM_GPORT_IS_WLAN_PORT(port)) {
            /* Deal with WLAN ports */
            vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                QOS_UNLOCK(unit);
                return BCM_E_BADID;
            }

            if ((flags & BCM_QOS_MAP_INGRESS) == BCM_QOS_MAP_INGRESS) {
                flags &= ~BCM_QOS_MAP_INGRESS;

                if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                    flags &= ~BCM_QOS_MAP_L2;
#if defined(BCM_TRIUMPH3_SUPPORT)
                    if (SOC_IS_TRIUMPH3(unit)) {
                        rv = bcm_tr3_wlan_lport_field_get(unit, port,
                                 TRUST_DOT1P_PTRf, &profile_idx);
                    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
                    {
                        rv = bcm_tr2_wlan_lport_field_get(unit, port,
                                 TRUST_DOT1P_PTRf, &profile_idx);
                    }
                    if (BCM_FAILURE(rv)) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }

                    rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                             _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP, map_id);
                } else {
                    rv = BCM_E_PARAM;
                }

                if (flags) {
                    rv = BCM_E_PARAM;
                }

                QOS_UNLOCK(unit);
                return rv;
            } else if ((flags & BCM_QOS_MAP_EGRESS) == BCM_QOS_MAP_EGRESS){
                flags &= ~BCM_QOS_MAP_EGRESS;

                if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                    flags &= ~BCM_QOS_MAP_L2;

#if defined(BCM_TRIUMPH3_SUPPORT)
                    if (SOC_IS_TRIUMPH3(unit)) {
                        /* not support */
                        rv = BCM_E_UNAVAIL;
                        QOS_UNLOCK(unit);
                        return rv;
                    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
                    {
                        /*
                         * EGR_WLAN_DVP.TUNNEL_INDEX points to CAPWAP initiator.
                         * EGR_IP_TUNNEL.DOT1P_MAPPING_PTR contains the map
                         */
                        rv = READ_EGR_WLAN_DVPm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        tunnel = soc_EGR_WLAN_DVPm_field32_get(unit, &wlan_dvp,
                                                               TUNNEL_INDEXf);
                        rv = soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                                          tunnel, tnl_entry);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        use_profile = soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                          tnl_entry, DOT1P_PRI_SELECTf);
                        if (use_profile) {
                            profile_idx = soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                              tnl_entry, DOT1P_MAPPING_PTRf);
                            rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                     _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, map_id);
                        } else {
                            rv = BCM_E_NOT_FOUND;
                        }
                    }
                } else {
                    rv = BCM_E_PARAM;
                }

                if (flags) {
                    rv = BCM_E_PARAM;
                }

                QOS_UNLOCK(unit);
                return rv;
            } else {
                rv = BCM_E_PARAM;
                QOS_UNLOCK(unit);
                return rv;
            }
        } else if (BCM_GPORT_IS_TUNNEL(port)) {
            /* Deal with tunnel initiators and terminators */
            tunnel = BCM_GPORT_TUNNEL_ID_GET(port);
            if ((flags & BCM_QOS_MAP_INGRESS) == BCM_QOS_MAP_INGRESS) {
                flags &= ~BCM_QOS_MAP_INGRESS;

                if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                    flags &= ~BCM_QOS_MAP_L3;

                    if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                    /*
                     * Get L3_IIF from L3_TUNNEL.
                     * Get TRUST_DSCP_PTR from L3_IIF
                     */
                    tunnel_id_max = soc_mem_index_max(unit, L3_TUNNELm);
                    tunnel_count = soc_mem_index_count(unit, L3_TUNNELm);
                    alloc_size = tunnel_count * sizeof(tnl_entry);
                    buf = soc_cm_salloc(unit, alloc_size, "TR2 L3 TUNNEL");
                    if (NULL == buf) {
                        QOS_UNLOCK(unit);
                        return (BCM_E_MEMORY);
                    }
                    rv = soc_mem_read_range(unit, L3_TUNNELm, MEM_BLOCK_ANY,
                                            0, tunnel_id_max, (void *)buf);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return rv;
                    }

                        for (index = 0; index < tunnel_id_max; index++) {
                            entries[0] = soc_mem_table_idx_to_pointer(unit, L3_TUNNELm,
                                                                      void *, buf,
                                                                      index);
                            if (tunnel == soc_mem_field32_get(unit, L3_TUNNELm,
                                              entries[0], TUNNEL_IDf)) {
                                /* Tunnel found - get L3_IIF */
                                l3_iif = soc_mem_field32_get(unit, L3_TUNNELm,
                                                             entries[0], L3_IIFf);
                                break;
                            }
                        }

                    soc_cm_sfree(unit, buf);
                    if (index == tunnel_id_max) {
                        QOS_UNLOCK(unit);
                        return BCM_E_NOT_FOUND; /* Tunnel not found */
                    }
                    } else {
                        QOS_UNLOCK(unit);
                        return BCM_E_UNAVAIL;
                    }

                    if (soc_mem_field_valid(unit, L3_IIFm, TRUST_DSCP_PTRf)) {
                        profile_idx = soc_mem_field32_get(unit, L3_IIFm, &l3_iif,
                                          TRUST_DSCP_PTRf);
                        rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                 _BCM_QOS_MAP_TYPE_DSCP_TABLE, map_id);
                    }
                } else {
                    rv = BCM_E_PARAM;
                }

                if (flags) {
                    rv = BCM_E_PARAM;
                }

                QOS_UNLOCK(unit);
                return rv;
            } else if ((flags & BCM_QOS_MAP_EGRESS) == BCM_QOS_MAP_EGRESS) {
                flags &= ~BCM_QOS_MAP_EGRESS;

                if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                    flags &= ~BCM_QOS_MAP_L3;
                    /* DSCP_MAPPING_PTR from EGR_IP_TUNNEL */
                    rv = soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY,
                                      tunnel, tnl_entry);
                    if (rv < 0) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }

                    use_profile = soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                      tnl_entry, DSCP_SELf);
                    if (use_profile == 2) {
                        profile_idx = soc_mem_field32_get(unit, EGR_IP_TUNNELm,
                                          tnl_entry, DSCP_MAPPING_PTRf);
                        rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                 _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE, map_id);
                    } else {
                        rv = BCM_E_NOT_FOUND;
                    }
                } else {
                    rv = BCM_E_PARAM;
                }

                if (flags) {
                    rv = BCM_E_PARAM;
                }

                QOS_UNLOCK(unit);
                return rv;
            } else {
                rv = BCM_E_PARAM;
                QOS_UNLOCK(unit);
                return rv;
            }
        } else if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(BCM_HGPROXY_COE_SUPPORT)
            if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                soc_feature(unit, soc_feature_channelized_switching)) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                if ((flags & BCM_QOS_MAP_INGRESS) == BCM_QOS_MAP_INGRESS) {
                    flags &= ~BCM_QOS_MAP_INGRESS;

                    if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                        flags &= ~BCM_QOS_MAP_L2;
                        rv = _bcm_esw_port_tab_get(unit, port,
                                                  TRUST_DOT1P_PTRf,
                                                  &profile_idx);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                        rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                 _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP, map_id);
                    } else {
                        rv = BCM_E_PARAM;
                    }

                    if (flags) {
                        /*more flags that could not be handled*/
                        rv = BCM_E_PARAM;
                    }
                    /*could return with map_id now*/
                    QOS_UNLOCK(unit);
                    return rv;
                } else if ((flags & BCM_QOS_MAP_EGRESS) == BCM_QOS_MAP_EGRESS) {
                    flags &= ~BCM_QOS_MAP_EGRESS;
                    if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                        flags &= ~BCM_QOS_MAP_L3;
                        if (soc_feature(unit, soc_feature_egr_qos_combo_profile) ||
                            SOC_IS_TRIDENT3X(unit)) {
                            /* TRIDENT 3 programs same port index pointer
                               without feature combo_profile */
                            rv = _bcm_esw_egr_port_tab_get(unit, port,
                                                          EGR_QOS_PROFILE_INDEXf,
                                                          &profile_idx);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                     _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE, map_id);
                        }
                    } else {
                        rv = BCM_E_PARAM;
                    }
                    /*invalid flags*/
                    if (flags) {
                        rv = BCM_E_PARAM;
                    }

                    QOS_UNLOCK(unit);
                    return rv;
                } else {
                    QOS_UNLOCK(unit);
                    rv = BCM_E_PARAM;
                    return rv;
                }
            }
#endif
        } else {
            QOS_UNLOCK(unit);
            rv = BCM_E_PARAM;
            return rv;
        }

    } else
#endif
    {
        /* Deal with physical ports */
        if (BCM_GPORT_IS_SET(port)) {
            if (bcm_esw_port_local_get(unit, port, &port) < 0) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        }
        if (!SOC_PORT_VALID(unit, port)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }

        if ((flags & BCM_QOS_MAP_INGRESS) == BCM_QOS_MAP_INGRESS) {
            flags &= ~BCM_QOS_MAP_INGRESS;

            if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                flags &= ~BCM_QOS_MAP_L2;
                if (SOC_IS_TRIDENT3X(unit)) {
                    rv = _bcm_esw_port_tab_get(unit, port, TRUST_DOT1P_PTRf, &profile_idx);
                } else {
                    rv = _bcm_tr2_port_tab_get(unit, port, TRUST_DOT1P_PTRf, &profile_idx);
                }
                if (BCM_FAILURE(rv)) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                         _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP, map_id);
            } else if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                flags &= ~BCM_QOS_MAP_L3;

                if (SOC_IS_KATANA(unit)) {
                    rv = _bcm_tr2_port_tab_get(unit, port, TRUST_DSCP_PTRf, &profile_idx);
                } else if (soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_PTRf)) {
                    rv = _bcm_tr2_port_tab_get(unit, port, TRUST_DSCP_PTRf,
                                               &profile_idx);
                } else if (SOC_IS_TRIDENT3X(unit)) {
                    rv = _bcm_esw_port_tab_get(unit, port, TRUST_DSCP_PTRf, &profile_idx);
                } else if (SOC_IS_TRIDENT(unit)) {
                    rv = BCM_E_UNAVAIL;
                }

                if (BCM_SUCCESS(rv) &&
                    soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_V4f)) {
                        rv = _bcm_tr2_port_tab_get(unit, port,
                                 TRUST_DSCP_V4f, &trust_dscp_v4);
                }
                if (BCM_SUCCESS(rv) &&
                    soc_mem_field_valid(unit, PORT_TABm, TRUST_DSCP_V6f)) {
                        rv = _bcm_tr2_port_tab_get(unit, port,
                                 TRUST_DSCP_V6f, &trust_dscp_v6);
                }

                /* if DSCP trust enabled */
                if (BCM_SUCCESS(rv)){
                    if (SOC_IS_TRIDENT3X(unit)) {
                       rv = _bcm_esw_port_tab_get(unit, port, TRUST_DSCP_V4f, &trust_dscp_v4);
                       if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }

                       rv = _bcm_esw_port_tab_get(unit, port, TRUST_DSCP_V6f, &trust_dscp_v6);
                       if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                       }
                    }

                    if ((trust_dscp_v4 != 0) || (trust_dscp_v6 != 0)) {
                        rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                                 _BCM_QOS_MAP_TYPE_DSCP_TABLE,
                                                 map_id);
                    } else {
                      rv = BCM_E_NOT_FOUND;
                    }
                }
            }

#ifdef BCM_HURRICANE3_SUPPORT
            else if ((flags & BCM_QOS_MAP_MIML) == BCM_QOS_MAP_MIML) {
                flags &= ~BCM_QOS_MAP_MIML;

                if (!soc_feature(unit, soc_feature_miml)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
                rv = _bcm_tr2_port_tab_get(unit, port, MIML_DOT1P_MAPPING_PTRf,
                                           &profile_idx);
                if (BCM_SUCCESS(rv)){
                    rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                             _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP, map_id);
                }
            }
#endif
            else {
                rv = BCM_E_PARAM;
            }

            if (flags) {
                /*more flags that could not be handled*/
                rv = BCM_E_PARAM;
            }
            /*could return with map_id now*/
            QOS_UNLOCK(unit);
            return rv;
        } else if ((flags & BCM_QOS_MAP_EGRESS) == BCM_QOS_MAP_EGRESS) {
            flags &= ~BCM_QOS_MAP_EGRESS;
            if ((flags & BCM_QOS_MAP_L2) == BCM_QOS_MAP_L2) {
                flags &= ~BCM_QOS_MAP_L2;

#if defined(BCM_TOMAHAWK3_SUPPORT)
                if (SOC_IS_TOMAHAWK3(unit)) {
                    /* EGR_PRI_CNG_MAPm will be searched if rv = NOT_FOUND */
                    rv = BCM_E_NOT_FOUND;
                } else
#endif
                if ( SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                                                          SOC_IS_TD_TT(unit)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                    if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                        egr_vlan_control_3_entry_t entry;
                        uint32 values[2];
                        soc_field_t fields[2];

                        fields[0] = IPRI_MAPPING_PTRf;
                        fields[1] = IPRI_CFI_SELf;

                        if (SOC_IS_TRIDENT3X(unit)) {
                            rv = bcm_esw_port_egr_lport_fields_get(unit, port, EGR_VLAN_CONTROL_3m,
                                                                   2, fields, values);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            profile_idx = values[0];
                            use_profile = values[1];
                        } else {
                            rv = READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port,
                                                      &entry);
                            if (BCM_FAILURE(rv)) {
                                QOS_UNLOCK(unit);
                                return rv;
                            }

                            profile_idx = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m,
                                                              &entry, IPRI_MAPPING_PTRf);
                            use_profile = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m,
                                                              &entry, IPRI_CFI_SELf);
                        }
                    } else
#endif
                    {
                        rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }

                        profile_idx =  soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                                         rval, IPRI_MAPPING_PTRf);
                        use_profile =  soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                                         rval, IPRI_CFI_SELf);

                    }
                    if (use_profile) {
                        rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                                  _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS, map_id);
                    } else {
                        rv = BCM_E_NOT_FOUND;
                    }
                }

                if (!soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                    if (rv == BCM_E_NOT_FOUND) {
                        alloc_size = 64 * sizeof(egr_pri_cng_map_entry_t);
                        buf = soc_cm_salloc(unit, alloc_size,
                                            "TR2 egr pri cng map");
                        if (NULL == buf) {
                            QOS_UNLOCK(unit);
                            return (BCM_E_MEMORY);
                        }
                        sal_memset(buf, 0, alloc_size);

                        buf2 = soc_cm_salloc(unit, alloc_size,
                                             "TR2 egr mpls exp map");
                        if (NULL == buf2) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return (BCM_E_MEMORY);
                        }
                        sal_memset(buf2, 0, alloc_size);

                        if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                            source_mem = EGR_MPLS_EXP_MAPPING_1m;
                            new_pri_field = PRIf;
                            new_cfi_field = CFIf;
                        } else {
                            source_mem = EGR_MPLS_PRI_MAPPINGm;
                            new_pri_field = NEW_PRIf;
                            new_cfi_field = NEW_CFIf;
                        }

                        index = port << 6;
                        rv = soc_mem_read_range(unit, EGR_PRI_CNG_MAPm,
                                                MEM_BLOCK_ANY, index,
                                                index + 63, (void *)buf);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            soc_cm_sfree(unit, buf2);
                            QOS_UNLOCK(unit);
                            return rv;
                        }

                        num_map = soc_mem_index_count(unit, source_mem) / 64;
                        for (id = 0; id < num_map; id++) {
                            if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                                continue;
                            }
                            index = (QOS_INFO(unit)->egr_mpls_hw_idx[id] *
                                     _BCM_QOS_MAP_CHUNK_EGR_MPLS);
                            rv = soc_mem_read_range(unit, source_mem,
                                                    MEM_BLOCK_ANY, index,
                                                    index + 63,
                                                    (void *)buf2);
                            if (BCM_FAILURE(rv)) {
                                soc_cm_sfree(unit, buf);
                                soc_cm_sfree(unit, buf2);
                                QOS_UNLOCK(unit);
                                return rv;
                            }
                            for (index = 0; index < _BCM_QOS_MAP_CHUNK_EGR_MPLS;
                                 index++) {
                                entries[0] = soc_mem_table_idx_to_pointer(
                                                 unit, source_mem, void *, buf2,
                                                 index);
                                new_pri = soc_mem_field32_get(unit, source_mem,
                                                              entries[0],
                                                              new_pri_field);
                                new_cfi = soc_mem_field32_get(unit, source_mem,
                                                              entries[0],
                                                              new_cfi_field);
                                entries[0] = soc_mem_table_idx_to_pointer(
                                                 unit, EGR_PRI_CNG_MAPm, void *,
                                                 buf, index);
                                pri = soc_mem_field32_get(unit,
                                                          EGR_PRI_CNG_MAPm,
                                                          entries[0], PRIf);
                                cfi = soc_mem_field32_get(unit,
                                                          EGR_PRI_CNG_MAPm,
                                                          entries[0], CFIf);
                                if ((new_pri != pri) || (new_cfi != cfi)) {
                                    break;
                                }
                            }
                            if (index >= _BCM_QOS_MAP_CHUNK_EGR_MPLS) {
                                *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS
                                                << _BCM_QOS_MAP_SHIFT);
                                break;
                            }
                        }

                        soc_cm_sfree(unit, buf);
                        soc_cm_sfree(unit, buf2);

                        if (id >= num_map) {
                            rv = BCM_E_NOT_FOUND;
                        }
                    }
                }
            } else if ((flags & BCM_QOS_MAP_L3) == BCM_QOS_MAP_L3) {
                flags &= ~BCM_QOS_MAP_L3;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (soc_feature(unit, soc_feature_egr_qos_combo_profile) ||
                   (SOC_IS_TRIDENT3X(unit))) {
                    /* TRIDENT 3 programs same port index pointer without feature combo_profile */
                    rv = _bcm_esw_egr_port_tab_get(unit, port, EGR_QOS_PROFILE_INDEXf, &profile_idx);
                    if (BCM_FAILURE(rv)) {
                        QOS_UNLOCK(unit);
                        return rv;
                    }

                    rv = _bcm_tr2_qos_idx2id(unit, profile_idx,
                             _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE, map_id);

                } else
#endif
                {
                    rv = BCM_E_PARAM;
                }
            } else {
                rv = BCM_E_PARAM;
            }
            /*invalid flags*/
            if (flags) {
                rv = BCM_E_PARAM;
            }

            QOS_UNLOCK(unit);
            return rv;
        } else {
            QOS_UNLOCK(unit);
            rv = BCM_E_PARAM;
            return rv;
        }
    }

    QOS_UNLOCK(unit);
    return rv;
}



/* Retrieve a QOS map attached to an Object (Gport) */
int
bcm_tr2_qos_port_map_get(int unit, bcm_gport_t port,
                         int *ing_map, int *egr_map)
{
    return BCM_E_UNAVAIL;
}

/* Attach a QoS map to vlan,port */
int
bcm_tr2_qos_port_vlan_map_set(int unit, bcm_gport_t port, bcm_vlan_t vid, int ing_map, int egr_map)
{
    pbmp_t pbmp, ubmp;
    int rv = BCM_E_NONE;
    int id, cur_id, map_id;
    egr_vlan_entry_t egr_vtab;

    QOS_INIT(unit);

    /* Validate Args */
    /* Deal with physical ports */
    if (BCM_GPORT_IS_SET(port)) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) &&
            !BCM_GPORT_IS_LOCAL(port) &&
            !BCM_GPORT_IS_MODPORT(port)) {
            return BCM_E_PARAM;
        }
#endif

        if (bcm_esw_port_local_get(unit, port, &port) < 0) {
            return BCM_E_PARAM;
        }
    }

    if (vid == BCM_VLAN_NONE) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PARAM;
    }

    /* check vlan membership for the port */
    BCM_IF_ERROR_RETURN(
        bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    QOS_LOCK(unit);

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Validate given ingress map for non supported types and IDs */
        if ((ing_map > 0) &&
            (BCM_E_NONE != _bcm_th3_qos_validate_map_type(unit, ing_map))) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        /* Validate given egress map for non supported types and IDs */
        if ((egr_map > 0) &&
            (BCM_E_NONE != _bcm_th3_qos_validate_map_type(unit, egr_map))) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    map_id = 0;
    if (ing_map != -1) { /* -1 means no change */
        /*
         * chips that do not support ingress mapping, only accept ing_map=-1
         */
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || \
            SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND2(unit)) {
            QOS_UNLOCK(unit);
            return BCM_E_UNAVAIL;
        }

        /* Make the port's vlan TRUST_DOT1_PTR point to the profile index */
        if (ing_map != 0) {
            if ((ing_map >> _BCM_QOS_MAP_SHIFT) !=
                 _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
            if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            map_id = QOS_INFO(unit)->pri_cng_hw_idx[id];
        } else {
            map_id = -1;
        }
    }

    if (ing_map != -1) { /* -1 means no change */
        QOS_UNLOCK(unit);
        /* Since below function does not require QoS lock
         * and is causing race condition with VLAN and L3IIF locks,
         * its being released.
         */
        rv = _bcm_tr2_qos_pvmid_set(unit, vid, map_id);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        /* Lock re-acquired for further processing. */
        QOS_LOCK(unit);
    }

    map_id = 0;
    if (egr_map != -1) { /* -1 means no change */
        if (egr_map != 0) {
            if ((egr_map >> _BCM_QOS_MAP_SHIFT) !=
                 _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
            if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            map_id = QOS_INFO(unit)->egr_mpls_hw_idx[id];
        }

        soc_mem_lock(unit, EGR_VLANm);

        sal_memset(&egr_vtab, 0, sizeof(egr_vlan_entry_t));
        rv = soc_mem_read(unit, EGR_VLANm, MEM_BLOCK_ANY, vid, &egr_vtab);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_VLANm);
            QOS_UNLOCK(unit);
            return rv;
        }

        /* Check for VLAN validity */
        if (0 == soc_EGR_VLANm_field32_get(unit, &egr_vtab, VALIDf)) {
            soc_mem_unlock(unit, EGR_VLANm);
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        }

        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || \
            SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND2(unit)) {
            soc_EGR_VLANm_field32_set(unit, &egr_vtab, DOT1P_MAPPING_PTRf, map_id);

            soc_EGR_VLANm_field32_set(unit, &egr_vtab, REMARK_DOT1Pf, 1);

            rv = soc_mem_write(unit, EGR_VLANm,MEM_BLOCK_ALL, vid, &egr_vtab);
        } else {
            cur_id = soc_EGR_VLANm_field32_get(unit, &egr_vtab, DOT1P_MAPPING_PTRf);
            if (map_id != cur_id) {
                soc_EGR_VLANm_field32_set(unit, &egr_vtab, DOT1P_MAPPING_PTRf, map_id);

                if (SOC_MEM_FIELD_VALID(unit, EGR_VLANm, REMARK_DOT1Pf)) {
                    soc_EGR_VLANm_field32_set(unit, &egr_vtab, REMARK_DOT1Pf,
                                              (map_id == 0) ? 0 : 1);
                }

                if (SOC_MEM_FIELD_VALID(unit, EGR_VLANm, REMARK_CFIf)) {
                    soc_EGR_VLANm_field32_set(unit, &egr_vtab, REMARK_CFIf,
                                                (map_id == 0) ? 0 : 1);
                }
                rv = soc_mem_write(unit, EGR_VLANm,MEM_BLOCK_ALL, vid, &egr_vtab);
            }
        }

        soc_mem_unlock(unit, EGR_VLANm);
    }

    QOS_UNLOCK(unit);

    return rv;
}


/* Retrieve a QOS map attached to an vlan and port */
int
bcm_tr2_qos_port_vlan_map_get(int unit, bcm_gport_t port, bcm_vlan_t vid,
                         int *ing_map, int *egr_map)
{
    pbmp_t pbmp, ubmp;
    int rv, idx;
    int map_id;
    egr_vlan_entry_t egr_vtab;

    QOS_INIT(unit);

    if (ing_map == NULL) {
        return BCM_E_PARAM;
    }

    if (egr_map == NULL) {
        return BCM_E_PARAM;
    }

    /* Deal with physical ports */
    if (BCM_GPORT_IS_SET(port)) {

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) &&
            !BCM_GPORT_IS_LOCAL(port) &&
            !BCM_GPORT_IS_MODPORT(port)) {
            return BCM_E_PARAM;
        }
#endif

        if (bcm_esw_port_local_get(unit, port, &port) < 0) {
            return BCM_E_PARAM;
        }
    }

    if (vid == BCM_VLAN_NONE) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PARAM;
    }


    /* check vlan membership for the port */
    BCM_IF_ERROR_RETURN(
        bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    QOS_LOCK(unit);

    rv = _bcm_tr2_qos_pvmid_get(unit, vid, &map_id);
    if (BCM_FAILURE(rv)) {
        QOS_UNLOCK(unit);
        return rv;
    }

    *ing_map = -1;
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; idx++) {
        if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, idx) &&
            (QOS_INFO(unit)->pri_cng_hw_idx[idx] == map_id)) {
            *ing_map = idx |
                    (_BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP << _BCM_QOS_MAP_SHIFT);
            break;
        }
    }

    soc_mem_lock(unit, EGR_VLANm);

    sal_memset(&egr_vtab, 0, sizeof(egr_vlan_entry_t));
    rv = soc_mem_read(unit, EGR_VLANm, MEM_BLOCK_ANY, vid, &egr_vtab);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, EGR_VLANm);
        QOS_UNLOCK(unit);
        return rv;
    }

    /* Check for VLAN validity */
    if (0 == soc_EGR_VLANm_field32_get(unit, &egr_vtab, VALIDf)) {
        soc_mem_unlock(unit, EGR_VLANm);
        QOS_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    map_id = soc_EGR_VLANm_field32_get(unit, &egr_vtab, DOT1P_MAPPING_PTRf);

    soc_mem_unlock(unit, EGR_VLANm);

    *egr_map = -1;
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
         if (_BCM_QOS_EGR_MPLS_USED_GET(unit, idx) &&
             (QOS_INFO(unit)->egr_mpls_hw_idx[idx] == map_id)) {
             *egr_map = idx |
             (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS << _BCM_QOS_MAP_SHIFT);
             break;
         }
    }

    QOS_UNLOCK(unit);
    return rv;
}

/* bcm_tr2_qos_map_multi_get */
int
bcm_tr2_qos_map_multi_get(int unit, uint32 flags,
                          int map_id, int array_size,
                          bcm_qos_map_t *array, int *array_count)
{
    int             rv = BCM_E_NONE;
    int             num_entries, idx, id, hw_id, alloc_size, entry_size, count;
    int             entry_size_ext = 0;
    int             entry_size_ext_2 = 0;
    int             alloc_size_ext = 0;
    int             alloc_size_ext_2 = 0;
    uint8           *dma_buf = NULL;
    uint8           *dma_buf_ext = NULL;
    uint8           *dma_buf_ext_2 = NULL;
    void            *entry;
    soc_mem_t       mem;
    soc_mem_t       mem_ext;
    soc_mem_t       mem_ext_2;

    bcm_qos_map_t   *map;
    soc_field_t     field;

    int multi_get_mode;

    /* ignore with_id & replace flags */
    flags &= (~(BCM_QOS_MAP_WITH_ID | BCM_QOS_MAP_REPLACE));
    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    mem = INVALIDm;
    mem_ext = INVALIDm;

    mem_ext_2 = INVALIDm;

    hw_id = 0;
    num_entries = 0;
    entry_size = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_qos_validate_map_type(unit, map_id));
    }
#endif

    QOS_LOCK(unit);

    if (SOC_IS_TRIDENT3X(unit)) {
       field = INT_PRIf;
    } else {
       field = PRIf;
    }

    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
            rv = BCM_E_PARAM;
            QOS_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(rv);
        }
        if (((flags != (BCM_QOS_MAP_L2 | BCM_QOS_MAP_INGRESS)) &&
            (flags != (BCM_QOS_MAP_L2 | BCM_QOS_MAP_INGRESS |
                       BCM_QOS_MAP_L2_UNTAGGED))) ||
            (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }

        if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
            if (SOC_IS_TRIDENT3X(unit)) {
                hw_id = QOS_INFO(unit)->pri_cng_hw_idx[id] * _BCM_QOS_MAP_CHUNK_PRI_CNG;
                num_entries = _BCM_QOS_MAP_CHUNK_PRI_CNG;
                mem = PHB_MAPPING_TBL_1m;
                entry_size = sizeof(phb_mapping_tbl_1_entry_t);
            } else {
                hw_id = QOS_INFO(unit)->pri_cng_hw_idx[id];
                num_entries = 1;
                entry_size = sizeof(ing_untagged_phb_entry_t);
                mem = ING_UNTAGGED_PHBm;
            }
        } else {
            num_entries = _BCM_QOS_MAP_CHUNK_PRI_CNG;
            hw_id = QOS_INFO(unit)->pri_cng_hw_idx[id]* _BCM_QOS_MAP_CHUNK_PRI_CNG;
            if (SOC_IS_TRIDENT3X(unit)) {
                 mem = PHB_MAPPING_TBL_1m;
                 entry_size = sizeof(phb_mapping_tbl_1_entry_t);
            } else {
                 mem = ING_PRI_CNG_MAPm;
                 entry_size = sizeof(ing_pri_cng_map_entry_t);
            }
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        if (((flags != (BCM_QOS_MAP_L2 | BCM_QOS_MAP_EGRESS)) &&
            (flags != (BCM_QOS_MAP_L2_INNER_TAG | BCM_QOS_MAP_EGRESS)) &&
            (flags != (BCM_QOS_MAP_MPLS | BCM_QOS_MAP_EGRESS))) ||
            (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        if ((flags & BCM_QOS_MAP_L2_OUTER_TAG) &&
            (flags & BCM_QOS_MAP_L2_INNER_TAG)) {
            rv = BCM_E_PARAM;
        }
#if defined(BCM_TOMAHAWK3_SUPPORT)
        /* TH3 does not support double/inner VLAN Tag */
        if (SOC_IS_TOMAHAWK3(unit) &&
            (flags & BCM_QOS_MAP_L2_INNER_TAG)) {
            rv = BCM_E_PARAM;
        }
#endif

        num_entries = _BCM_QOS_MAP_CHUNK_EGR_MPLS;
        entry_size = sizeof(egr_mpls_pri_mapping_entry_t);
        hw_id = (QOS_INFO(unit)->egr_mpls_hw_idx[id] *
                _BCM_QOS_MAP_CHUNK_EGR_MPLS);
        mem = EGR_MPLS_PRI_MAPPINGm;
        if (SOC_IS_TRIDENT3X(unit)) {
            /* Check if mem is invalid then assign proper valid mem */
            if (!SOC_MEM_IS_VALID(unit, EGR_MPLS_PRI_MAPPINGm)) {
                mem = EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m;
            }
            if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                entry_size_ext = sizeof(egr_zone_1_dot1p_mapping_table_3_entry_t);
                mem_ext = EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m;
            }
            if (flags & BCM_QOS_MAP_MPLS) {
                entry_size_ext_2 = sizeof(egr_mpls_exp_mapping_1_entry_t);
                mem_ext_2 = EGR_MPLS_EXP_MAPPING_1m;
            }
        } else if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m) &&
		          ((flags & BCM_QOS_MAP_L2_OUTER_TAG) ||
                  (flags & BCM_QOS_MAP_MPLS))) {
            entry_size_ext = sizeof(egr_mpls_exp_mapping_1_entry_t);
            mem_ext = EGR_MPLS_EXP_MAPPING_1m;
        } else if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m) &&
            ((flags & BCM_QOS_MAP_L2_INNER_TAG ||
             (flags & BCM_QOS_MAP_MPLS)))) {
            entry_size_ext_2 = sizeof(egr_mpls_exp_mapping_2_entry_t);
            mem_ext_2 = EGR_MPLS_EXP_MAPPING_2m;
        }

        break;
#ifdef BCM_APACHE_SUPPORT
    case _BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP:
        if (soc_feature(unit, soc_feature_mpls_exp_to_phb_cng_map)) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            id = id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK;

            if ((flags == (BCM_QOS_MAP_OAM_MPLS_EXP | BCM_QOS_MAP_INGRESS)) &&
                (bcm_tr_mpls_exp_to_phb_cng_map_usage_get(unit, flags,
                                                          id) != BCM_E_NONE)) {
                rv = BCM_E_PARAM;
                QOS_UNLOCK(unit);
                BCM_IF_ERROR_RETURN(rv);
            }
            if ((flags == (BCM_QOS_MAP_OAM_MPLS_EXP | BCM_QOS_MAP_EGRESS)) &&
                (bcm_tr_mpls_exp_to_phb_cng_map_usage_get(unit, flags,
                                                          id) != BCM_E_NONE)) {
                rv = BCM_E_PARAM;
                QOS_UNLOCK(unit);
                BCM_IF_ERROR_RETURN(rv);
            }
            rv = bcm_tr_mpls_exp_to_phb_cng_map_profile_id_get(unit, flags,
                    id, &hw_id);
            if(rv == BCM_E_NONE){
                hw_id = hw_id * _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP;
                num_entries = _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP;
                if (flags & BCM_QOS_MAP_INGRESS){
                    entry_size = sizeof(ing_mpls_exp_mapping_1_entry_t);
                    mem = ING_MPLS_EXP_MAPPING_1m;
                }else if (flags & BCM_QOS_MAP_EGRESS){
                    entry_size = sizeof(egr_mpls_exp_mapping_3_entry_t);
                    mem = EGR_MPLS_EXP_MAPPING_3m;
                }
            } else {
                rv = BCM_E_PARAM;
            }
#endif

        } else {
            rv = BCM_E_PARAM;
        }
        break;
#endif
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if ((flags != (BCM_QOS_MAP_L3 | BCM_QOS_MAP_INGRESS)) ||
            (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_DSCP;
        if (SOC_IS_TRIDENT3X(unit)) {
            entry_size = sizeof(phb_mapping_tbl_2_entry_t);
        } else {
            entry_size = sizeof(dscp_table_entry_t);
        }
        hw_id = QOS_INFO(unit)->dscp_hw_idx[id] * _BCM_QOS_MAP_CHUNK_DSCP;
        mem = _bcm_tr2_qos_bk_info[unit].dscp_table;
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        if ((flags != (BCM_QOS_MAP_L3 | BCM_QOS_MAP_EGRESS)) ||
            (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_EGR_DSCP;
        if (SOC_IS_TRIDENT3X(unit)) {
            entry_size = sizeof(egr_zone_4_qos_mapping_table_entry_t);
        } else {
            entry_size = sizeof(egr_dscp_table_entry_t);
        }
        hw_id = (QOS_INFO(unit)->egr_dscp_hw_idx[id] *
                _BCM_QOS_MAP_CHUNK_EGR_DSCP);
        mem = _bcm_tr2_qos_bk_info[unit].egr_dscp_table;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
            entry_size_ext = sizeof(egr_pri_cng_map_entry_t);
            mem_ext = EGR_PRI_CNG_MAPm;
        }
#endif
        break;
#ifdef BCM_KATANA_SUPPORT
    case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
        if ((flags != (BCM_QOS_MAP_QUEUE)) ||
            (!_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_OFFSET_MAP;
        entry_size = sizeof(ing_queue_offset_mapping_table_entry_t);
        hw_id = QOS_INFO(unit)->offset_map_hw_idx[id] *
                                       _BCM_QOS_MAP_CHUNK_OFFSET_MAP;
        mem = ING_QUEUE_OFFSET_MAPPING_TABLEm;
        break;
#endif
#ifdef BCM_KATANA2_SUPPORT
    case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
        if ((flags != (BCM_QOS_MAP_REPLICATION)) ||
            (!_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
            QOS_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(rv);
        }
        rv = _bcm_rqe_queue_offset_map_multi_get(unit, flags, id,
                               array_size, array, array_count);

        QOS_UNLOCK(unit);
        return rv;
#endif
#ifdef BCM_SABER2_SUPPORT
    case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
        if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
            if (((flags != BCM_QOS_MAP_OAM_INTPRI) &&
                        (flags != BCM_QOS_MAP_OAM_OUTER_VLAN_PCP)&&
                        (flags != BCM_QOS_MAP_OAM_INNER_VLAN_PCP)&&
                        (flags != BCM_QOS_MAP_OAM_MPLS_EXP      )) ||
                    (!_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id))) {
                rv = BCM_E_PARAM;
            }
            num_entries = _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP;
            entry_size = sizeof(ing_service_pri_map_0_entry_t);
            hw_id = QOS_INFO(unit)->service_pri_map_hw_idx[id] *
                _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP;
            /* All memories (ING_SERVICE_PRI_MAP_0/1/2m and EGR_SERVICE_PRI_MAP_0/1/2m)
             * have similar profiles. So choosing one of the memories.
             */
            mem = ING_SERVICE_PRI_MAP_0m;
        } else {
            rv = BCM_E_PARAM;
        }
        break;
#endif
    default:
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        if ((map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) ==
            _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
            /* Map created using TR mpls module */
            if (flags != (BCM_QOS_MAP_MPLS | BCM_QOS_MAP_INGRESS)) {
                rv = BCM_E_PARAM;
            }
            id = map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK;
            if (!_BCM_QOS_ING_MPLS_EXP_USED_GET(unit, id)) {
                rv = BCM_E_PARAM;
            }
            num_entries = _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP;
            if (SOC_IS_TRIDENT3X(unit)) {
                entry_size = sizeof(phb_mapping_tbl_3_entry_t);
                mem = PHB_MAPPING_TBL_3m;
            } else {
                entry_size = sizeof(ing_mpls_exp_mapping_entry_t);
                mem = ING_MPLS_EXP_MAPPINGm;
            }

            if (soc_feature(unit, soc_feature_td3_style_mpls)) {
                hw_id = id << 6;
            } else {
                hw_id = id * num_entries; /* soc profile mem is not used. So direct hw index */
            }
        } else
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
        {
            rv = BCM_E_PARAM;
        }
    }
    QOS_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (NULL == array_count) {
        return BCM_E_PARAM;
    }

    if (array_size == 0) { /* querying the size of map-chunk */
        *array_count = num_entries;
        return BCM_E_NONE;
    }
    if (!array) {
        return BCM_E_PARAM;
    }
    if (soc_mem_is_valid(unit, mem)) {
        /* Allocate memory for DMA & regular */
        alloc_size = num_entries * entry_size;
        dma_buf = soc_cm_salloc(unit, alloc_size, "TR2 qos multi get DMA buf");
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
    }
    if (mem_ext != INVALIDm) {
        alloc_size_ext = num_entries * entry_size_ext;
        dma_buf_ext = soc_cm_salloc(unit, alloc_size_ext, "TR2 qos multi get external DMA buf");
        if (!dma_buf_ext) {
            soc_cm_sfree(unit, dma_buf);
            return BCM_E_MEMORY;
        }
        sal_memset(dma_buf_ext, 0, alloc_size_ext);

        /* Read the profile chunk */
        rv = soc_mem_read_range(unit, mem_ext, MEM_BLOCK_ANY, hw_id,
                                (hw_id + num_entries - 1), dma_buf_ext);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, dma_buf);
            soc_cm_sfree(unit, dma_buf_ext);
            return rv;
        }
    }

    if (mem_ext_2 != INVALIDm) {
        alloc_size_ext_2 = num_entries * entry_size_ext_2;
        dma_buf_ext_2 = soc_cm_salloc(unit, alloc_size_ext_2, "TR2 qos multi get external_2 DMA buf");
        if (!dma_buf_ext_2) {
            soc_cm_sfree(unit, dma_buf);
            if (dma_buf_ext) {
                soc_cm_sfree(unit, dma_buf_ext);
            }
            return BCM_E_MEMORY;
        }
        sal_memset(dma_buf_ext_2, 0, alloc_size_ext_2);

        /* Read the profile chunk */
        rv = soc_mem_read_range(unit, mem_ext_2, MEM_BLOCK_ANY, hw_id,
                                (hw_id + num_entries - 1), dma_buf_ext_2);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, dma_buf);
            if (dma_buf_ext) {
                soc_cm_sfree(unit, dma_buf_ext);
            }
            soc_cm_sfree(unit, dma_buf_ext_2);
            return rv;
        }
    }

    multi_get_mode = soc_property_get(unit, spn_QOS_MAP_MULTI_GET_MODE, 0);
    count = 0;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->pkt_pri = ((idx & 0x1e) >> 1);
            map->pkt_cfi = (idx & 0x1);
            if (soc_mem_is_valid(unit, mem)) {
                entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                     dma_buf, idx);
                map->int_pri = soc_mem_field32_get(unit, mem, entry, field);
                map->color = _BCM_COLOR_DECODING(unit, soc_mem_field32_get(unit,
                                     mem, entry, CNGf));
            }
            count++;
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            if (multi_get_mode && ((idx & 0x3) == _BCM_XGS3_COLOR_RESERVED)) {
                /*
                 * If multi_get_mode set, then only valid mappings are returned.
                 */
                continue;
            }
            map = &array[count];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->int_pri = ((idx & 0x3c) >> 2);
            map->color = _BCM_COLOR_DECODING(unit, (idx & 0x3));
            if (soc_mem_is_valid(unit, mem)) {
                if (mem == EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m) {
                    entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                         dma_buf, idx);
                    map->pkt_pri = soc_mem_field32_get(unit, mem, entry, PRIf);
                    map->pkt_cfi = soc_mem_field32_get(unit, mem, entry, CFIf);
                } else {
                    entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                         dma_buf, idx);
                    map->pkt_pri = soc_mem_field32_get(unit, mem, entry, NEW_PRIf);
                    map->pkt_cfi = soc_mem_field32_get(unit, mem, entry, NEW_CFIf);
                }
            }
            if (dma_buf_ext) {
                entry = soc_mem_table_idx_to_pointer(unit, mem_ext, void *,
                                                     dma_buf_ext, idx);
                if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                    map->pkt_pri = soc_mem_field32_get(unit,
                                                       mem_ext,
                                                       entry,
                                                       PRIf);
                    map->pkt_cfi = soc_mem_field32_get(unit,
                                                       mem_ext,
                                                       entry,
                                                       CFIf);
                }
                if (!SOC_IS_TRIDENT3X(unit)) {
                   /* For none Trident3 chips, MPLS_EXP is coming from same memory map */
                    if (flags & BCM_QOS_MAP_MPLS) {
                        map->exp = soc_mem_field32_get(unit, mem_ext, entry,
                                                       MPLS_EXPf);
                    }
                }
            } else if (dma_buf_ext_2) {
                entry = soc_mem_table_idx_to_pointer(unit, mem_ext_2, void *,
                                                     dma_buf_ext_2, idx);
                if (flags & BCM_QOS_MAP_MPLS) {
                    map->exp = soc_mem_field32_get(unit, mem_ext_2, entry,
                                                   MPLS_EXPf);
                }
            }
            count++;
        }
        break;
#ifdef BCM_APACHE_SUPPORT
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
    case _BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->exp = idx;
            if (soc_mem_is_valid(unit, mem)) {
                entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                        dma_buf, idx);
                map->color = _BCM_COLOR_DECODING(unit,soc_mem_field32_get(unit,
                            mem, entry, CNGf));
                map->int_pri = soc_mem_field32_get(unit, mem, entry, PHBf);
            }
            count++;
        }
        break;
#endif
#endif
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->dscp = idx;
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                 dma_buf, idx);
            map->int_pri = soc_mem_field32_get(unit, mem, entry, field);
            map->color = _BCM_COLOR_DECODING(unit, soc_mem_field32_get(unit,
                                 mem, entry, CNGf));
            count++;
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->int_pri = ((idx & 0x3c) >> 2);
            map->color = _BCM_COLOR_DECODING(unit, (idx & 0x3));
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                 dma_buf, idx);
            if (SOC_IS_TRIDENT3X(unit)) {
                map->dscp = soc_mem_field32_get(unit, mem, entry, QOSf);
            } else {
                map->dscp = soc_mem_field32_get(unit, mem, entry, DSCPf);
            }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
                if (dma_buf_ext) {
                    entry = soc_mem_table_idx_to_pointer(unit, mem_ext, void *,
                                                         dma_buf_ext, idx);
                    map->pkt_pri = soc_mem_field32_get(unit,
                                                       EGR_PRI_CNG_MAPm,
                                                       entry,
                                                       PRIf);
                    map->pkt_cfi = soc_mem_field32_get(unit,
                                                       EGR_PRI_CNG_MAPm,
                                                       entry,
                                                       CFIf);
                }
            }
#endif
            count++;
        }
        break;
#ifdef BCM_KATANA_SUPPORT
    case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->int_pri = idx;
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                 dma_buf, idx);
            map->queue_offset = soc_mem_field32_get(unit, mem, entry,
                                                          QUEUE_OFFSETf);
            count++;
        }
        break;
#endif
#ifdef BCM_SABER2_SUPPORT
    case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->int_pri = idx;
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                 dma_buf, idx);
            map->counter_offset = soc_mem_field32_get(unit, mem, entry,
                                                          OFFSETf);
            count++;
        }
        break;
#endif
    default:
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        if ((map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_MASK) ==
            _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
            for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
                map = &array[idx];
                sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
                map->exp = idx;
                entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                                                     dma_buf, idx);
                map->color = _BCM_COLOR_DECODING(unit,soc_mem_field32_get(unit,
                                 mem, entry, CNGf));
                map->int_pri = soc_mem_field32_get(unit, mem, entry, field);
                count++;
            }
        } else
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
        {
            rv = BCM_E_INTERNAL; /* should not hit this */
        }
    }
    QOS_UNLOCK(unit);
	if (dma_buf) {
        soc_cm_sfree(unit, dma_buf);
	}
    if (dma_buf_ext) {
        soc_cm_sfree(unit, dma_buf_ext);
    }
    if (dma_buf_ext_2) {
        soc_cm_sfree(unit, dma_buf_ext_2);
    }
    BCM_IF_ERROR_RETURN(rv);

    *array_count = count;
    return BCM_E_NONE;
}

/* Get the list of all created Map-ids along with corresponding flags */
int
bcm_tr2_qos_multi_get(int unit, int array_size, int *map_ids_array,
                      int *flags_array, int *array_count)
{
    int rv = BCM_E_NONE;
    int idx, count;

    QOS_INIT(unit);
    QOS_LOCK(unit);
    if (array_size == 0) {
        /* querying the number of map-ids for storage allocation */
        if (array_count == NULL) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            *array_count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->ing_pri_cng_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP);
            *array_count += count;
            count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->egr_mpls_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
            *array_count += count;
            count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->dscp_table_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_DSCP_TABLE);
            /* For all port dscp profiles have been reserved on TD. */
            if (SOC_IS_TRIDENT(unit)) {
                count -= SOC_PORT_MAX(unit, all);
            }
            *array_count += count;
            count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->egr_dscp_table_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE);
            *array_count += count;
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            if (soc_feature(unit, soc_feature_mpls)) {
                count = 0;
                SHR_BITCOUNT_RANGE(QOS_INFO(unit)->ing_mpls_exp_bitmap, count,
                                   0, _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP);
                *array_count += count;
            }
#ifdef BCM_APACHE_SUPPORT
            if (soc_feature(unit, soc_feature_mpls_exp_to_phb_cng_map)) {
                count = 0;
                bcm_tr_mpls_exp_to_phb_cng_map_profile_range_get(unit,
                        BCM_QOS_MAP_INGRESS,
                        &count);
                *array_count += count;
                count = 0;
                bcm_tr_mpls_exp_to_phb_cng_map_profile_range_get(unit,
                        BCM_QOS_MAP_EGRESS,
                        &count);
                *array_count += count;
            }
#endif
#endif
#ifdef BCM_KATANA_SUPPORT
            count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->offset_map_table_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE);
            *array_count += count;
#endif
#ifdef BCM_SABER2_SUPPORT
            if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
                count = 0;
                SHR_BITCOUNT_RANGE(QOS_INFO(unit)->service_pri_map_table_bitmap, count,
                        0, _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE);
                *array_count += count;
            }
#endif
#ifdef BCM_KATANA2_SUPPORT
            count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->rqe_queue_offset_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP);
            *array_count += count;
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_fcoe)) {
                _bcm_td2_qos_fcoe_get(unit, array_size, NULL, NULL, &count);
                *array_count += count;
            }
            if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
                count = 0;
                rv = _bcm_td2_qos_multi_get(unit, array_size, NULL, NULL, &count);
                if (BCM_SUCCESS(rv)) {
                    *array_count += count;
                }
            }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
                count = 0;
                rv = _bcm_th_qos_multi_get(unit, array_size, NULL, NULL, &count);
                if (BCM_SUCCESS(rv)) {
                    *array_count += count;
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
        }
    } else {
        if ((map_ids_array == NULL) || (flags_array == NULL) ||
            (array_count == NULL)) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) &&
                         (count < array_size)); idx++) {
                if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, idx)) {
                    *(map_ids_array + count) = idx |
                     (_BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP << _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = (BCM_QOS_MAP_L2 |
                                              BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS) &&
                          (count < array_size)); idx++) {
                 if (_BCM_QOS_EGR_MPLS_USED_GET(unit, idx)) {
                     *(map_ids_array + count) = idx |
                     (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS << _BCM_QOS_MAP_SHIFT);
                     *(flags_array + count) = (BCM_QOS_MAP_EGRESS |
                                   (_BCM_QOS_EGR_MPLS_FLAGS_USED_GET(unit, idx)?
                                   BCM_QOS_MAP_MPLS : BCM_QOS_MAP_L2));
                     count++;
                 }
            }
            for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_DSCP_TABLE) &&
                         (count < array_size)); idx++) {
                /* For all port dscp profiles have been reserved on TD. */
                if (SOC_IS_TRIDENT(unit)) {
                    if (idx < SOC_PORT_MAX(unit, all)) {
                        continue;
                    }
                }
                if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, idx)) {
                    *(map_ids_array + count) = idx |
                     (_BCM_QOS_MAP_TYPE_DSCP_TABLE << _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = (BCM_QOS_MAP_L3 |
                                              BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_EGR_DSCP_TABLE) &&
                         (count < array_size)); idx++) {
                if (_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, idx)) {
                    *(map_ids_array + count) = idx |
                     (_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE << _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = (BCM_QOS_MAP_L3 |
                                              BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            if (soc_feature(unit, soc_feature_mpls)) {
                for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP) &&
                              (count < array_size)); idx++) {
                     if (_BCM_QOS_ING_MPLS_EXP_USED_GET(unit, idx)) {
                         *(map_ids_array + count) = (idx |
                                           _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS);
                         *(flags_array + count) = (BCM_QOS_MAP_MPLS |
                                                   BCM_QOS_MAP_INGRESS);
                         count++;
                     }
                }
            }
#ifdef BCM_APACHE_SUPPORT
            if (soc_feature(unit, soc_feature_mpls_exp_to_phb_cng_map)) {
                for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP_1) &&
                            (count < array_size)); idx++) {
                    if ((bcm_tr_mpls_exp_to_phb_cng_map_usage_get(unit,
                                (BCM_QOS_MAP_MPLS |
                                 BCM_QOS_MAP_INGRESS),
                                idx)) == BCM_E_NONE) {
                        *(map_ids_array + count) = (idx |
                                _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) |
                            (_BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP
                             << _BCM_QOS_MAP_SHIFT);
                        *(flags_array + count) = (BCM_QOS_MAP_OAM_MPLS_EXP |
                                BCM_QOS_MAP_INGRESS);
                        count++;
                    }
                }
                for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_EGR_MPLS_EXP_MAP_3) &&
                            (count < array_size)); idx++) {
                    if ((bcm_tr_mpls_exp_to_phb_cng_map_usage_get(unit,
                                (BCM_QOS_MAP_MPLS |
                                 BCM_QOS_MAP_EGRESS),
                                idx)) == BCM_E_NONE) {
                        *(map_ids_array + count) = (idx |
                                _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS) |
                            (_BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP
                             << _BCM_QOS_MAP_SHIFT);
                        *(flags_array + count) = (BCM_QOS_MAP_OAM_MPLS_EXP |
                                BCM_QOS_MAP_EGRESS);
                        count++;
                    }
                }
            }
#endif
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
            for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_OFFSET_MAP_TABLE) &&
                         (count < array_size)); idx++) {
                if (_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, idx)) {
                    *(map_ids_array + count) = idx |
                     (_BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE <<
                                                     _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = BCM_QOS_MAP_QUEUE;
                    count++;
                }
            }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
            if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
                for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE) &&
                            (count < array_size)); idx++) {
                    if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, idx)) {
                        *(map_ids_array + count) = idx |
                            (_BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE <<
                             _BCM_QOS_MAP_SHIFT);
                        *(flags_array + count) =
                            _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_GET(unit, idx);
                        count++;
                    }
                }
            }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
            for (idx = 0; ((idx < _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP) &&
                         (count < array_size)); idx++) {
                if (_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, idx)) {
                    *(map_ids_array + count) = idx |
                     (_BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP <<
                                                     _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = BCM_QOS_MAP_REPLICATION;
                    count++;
                }
            }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
            if (soc_feature(unit, soc_feature_fcoe)) {
                _bcm_td2_qos_fcoe_get(unit, array_size, map_ids_array,
                                      flags_array, &count);
            }
            *array_count = count;
            if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT3X(unit)) {
                rv = _bcm_td2_qos_multi_get(unit, (array_size - count),
                                                  (map_ids_array + count),
                                                  (flags_array + count), &count);
                if (BCM_SUCCESS(rv)) {
                    *array_count += count;
                }
                count = *array_count;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
                if (count < array_size) {
                    rv = _bcm_th_qos_multi_get(unit, (array_size - count),
                                                     (map_ids_array + count),
                                                     (flags_array + count), array_count);
                    if (BCM_SUCCESS(rv)) {
                        count += *array_count;
                    }
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
            *array_count = count;
        }
    }
    QOS_UNLOCK(unit);
    return rv;
}

/* Function:
*	   _bcm_tr2_qos_id2idx
* Purpose:
*	   Translate map ID into hardware table index used by API
* Parameters:
* Returns:
*	   BCM_E_XXX
*/
int
_bcm_tr2_qos_id2idx(int unit, int map_id, int *hw_idx)
{
    int id;

    QOS_INIT(unit);

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
#endif
         if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
         }
         if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
              QOS_UNLOCK(unit);
              return BCM_E_NOT_FOUND;
         } else {
              *hw_idx = QOS_INFO(unit)->pri_cng_hw_idx[id];
         }
         break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS:
#endif
         if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
              QOS_UNLOCK(unit);
              return BCM_E_NOT_FOUND;
         } else {
              *hw_idx = QOS_INFO(unit)->egr_mpls_hw_idx[id];
         }
         break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
         if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
              QOS_UNLOCK(unit);
              return BCM_E_NOT_FOUND;
         } else {
              *hw_idx = QOS_INFO(unit)->dscp_hw_idx[id];
         }
         break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
         if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
              QOS_UNLOCK(unit);
              return BCM_E_NOT_FOUND;
         } else {
              *hw_idx =  QOS_INFO(unit)->egr_dscp_hw_idx[id];
         }
         break;
#ifdef BCM_KATANA_SUPPORT
    case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
         if (!_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, id)) {
              QOS_UNLOCK(unit);
              return BCM_E_NOT_FOUND;
         } else {
              *hw_idx = QOS_INFO(unit)->offset_map_hw_idx[id];
         }
         break;
#endif
#ifdef BCM_SABER2_SUPPORT
    case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
         if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
             if (!_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id)) {
                 QOS_UNLOCK(unit);
                 return BCM_E_NOT_FOUND;
             } else {
                 *hw_idx = QOS_INFO(unit)->service_pri_map_hw_idx[id];
             }
         } else {
             QOS_UNLOCK(unit);
             return BCM_E_PARAM;
         }
         break;
#endif
#ifdef BCM_KATANA2_SUPPORT
    case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
         if (!_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id)) {
              QOS_UNLOCK(unit);
              return BCM_E_NOT_FOUND;
         } else {
              *hw_idx = QOS_INFO(unit)->rqe_queue_offset_hw_idx[id];
         }
         break;
#endif

    default:
         QOS_UNLOCK(unit);
         return BCM_E_NOT_FOUND;
    }
    QOS_UNLOCK(unit);
    return BCM_E_NONE;
}

/* Function:
*	   _bcm_tr2_qos_id2idx
* Purpose:
*	   Translate hardware table index into map ID used by API
* Parameters:
* Returns:
*	   BCM_E_XXX
*/
int
_bcm_tr2_qos_idx2id(int unit, int hw_idx, int type, int *map_id)
{
    int id, num_map;

    QOS_INIT(unit);

    switch (type) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        for (id = 0; id <_BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; id++) {
            if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                if (QOS_INFO(unit)->pri_cng_hw_idx[id] == hw_idx) {
                     *map_id = id | (_BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP
                                                             << _BCM_QOS_MAP_SHIFT);
                     return BCM_E_NONE;
                }
            }
        }
        break;

#ifdef BCM_HURRICANE3_SUPPORT
     case _BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP:
         for (id = 0; id <_BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; id++) {
             if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                 if (QOS_INFO(unit)->pri_cng_hw_idx[id] == hw_idx) {
                      *map_id = id | (_BCM_QOS_MAP_TYPE_MIML_ING_PRI_CNG_MAP
                                                              << _BCM_QOS_MAP_SHIFT);
                      return BCM_E_NONE;
                 }
             }
         }
         break;
#endif

    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
         if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
             num_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
         } else {
             num_map = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
         }
         for (id = 0; id < num_map; id++) {
              if (_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                   if (QOS_INFO(unit)->egr_mpls_hw_idx[id] == hw_idx) {
                        *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS
                                                                << _BCM_QOS_MAP_SHIFT);
                        return BCM_E_NONE;
                   }
              }
         }
         break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
         num_map = soc_mem_index_count(unit, _bcm_tr2_qos_bk_info[unit].dscp_table) / 64;
         for (id = 0; id < num_map; id++) {
              if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
                   if (QOS_INFO(unit)->dscp_hw_idx[id] == hw_idx) {
                        *map_id = id | (_BCM_QOS_MAP_TYPE_DSCP_TABLE
                                                                << _BCM_QOS_MAP_SHIFT);
                        return BCM_E_NONE;
                   }
              }
         }
         break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
         num_map = soc_mem_index_count(unit, _bcm_tr2_qos_bk_info[unit].egr_dscp_table) / 64;
         for (id = 0; id < num_map; id++) {
              if (_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                   if (QOS_INFO(unit)->egr_dscp_hw_idx[id] == hw_idx) {
                        *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE
                                                                << _BCM_QOS_MAP_SHIFT);
                        return BCM_E_NONE;
                   }
              }
         }
         break;
#ifdef BCM_KATANA_SUPPORT
     case _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE:
         num_map = soc_mem_index_count(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm) /
                                                 _BCM_QOS_MAP_CHUNK_OFFSET_MAP;
         for (id = 0; id < num_map; id++) {
              if (_BCM_QOS_OFFSET_MAP_TABLE_USED_GET(unit, id)) {
                   if (QOS_INFO(unit)->offset_map_hw_idx[id] == hw_idx) {
                        *map_id =
                           id | (_BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE <<
                                                           _BCM_QOS_MAP_SHIFT);
                        return BCM_E_NONE;
                   }
              }
         }
         break;
#endif
#ifdef BCM_KATANA2_SUPPORT
     case _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP:
         num_map = _BCM_QOS_MAP_LEN_RQE_QUEUE_OFFSET_MAP;
         for (id = 0; id < num_map; id++) {
              if (_BCM_QOS_RQE_QUEUE_OFFSET_MAP_USED_GET(unit, id)) {
                   if (QOS_INFO(unit)->rqe_queue_offset_hw_idx[id] == hw_idx) {
                        *map_id =
                           id | (_BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP <<
                                                           _BCM_QOS_MAP_SHIFT);
                        return BCM_E_NONE;
                   }
              }
         }
         break;
#endif
#ifdef BCM_SABER2_SUPPORT
     case _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE:
         if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
             for (id = 0; id < _BCM_QOS_MAP_LEN_SERVICE_PRI_MAP_TABLE; id++) {
                 if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id)) {
                     if (QOS_INFO(unit)->service_pri_map_hw_idx[id] == hw_idx) {
                         *map_id =
                             id | (_BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE <<
                                     _BCM_QOS_MAP_SHIFT);
                         return BCM_E_NONE;
                     }
                 }
             }
         }
         break;
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    case _BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS:
         if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
             num_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
         } else {
             num_map = soc_mem_index_count(unit, EGR_MPLS_PRI_MAPPINGm) / 64;
         }
         for (id = 0; id < num_map; id++) {
              if (_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                   if (QOS_INFO(unit)->egr_mpls_hw_idx[id] == hw_idx) {
                        *map_id = id | (_BCM_QOS_MAP_TYPE_MIML_EGR_MPLS_MAPS
                                                                << _BCM_QOS_MAP_SHIFT);
                        return BCM_E_NONE;
                   }
              }
         }
         break;
#endif
    default:
         return BCM_E_NOT_FOUND;
    }
    return BCM_E_NOT_FOUND;
}

#ifdef INCLUDE_L3
int
_bcm_tr2_vp_dscp_map_mode_set(int unit, bcm_gport_t port, int mode)
{
    source_vp_entry_t svp;
    int rv = BCM_E_NONE;
    uint32 vp = -1;
    int mode_ipv4 = 0, mode_ipv6 = 0;
    /* Deal with different types of gports */
    if (_BCM_QOS_GPORT_IS_VFI_VP_PORT(port)) {
        /* Deal with MiM and MPLS ports */
        if (BCM_GPORT_IS_MIM_PORT(port)) {
            vp = BCM_GPORT_MIM_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                return BCM_E_BADID;
            }
        } else if BCM_GPORT_IS_VXLAN_PORT(port) {
            vp = BCM_GPORT_VXLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                return BCM_E_BADID;
            }
        } else if BCM_GPORT_IS_L2GRE_PORT(port) {
            vp = BCM_GPORT_L2GRE_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                return BCM_E_BADID;
            }
        } else if BCM_GPORT_IS_FLOW_PORT(port) {
            vp = BCM_GPORT_FLOW_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                return BCM_E_BADID;
            }
        } else {
            vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                return BCM_E_BADID;
            }
        }
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V4f)) {
        mode_ipv4 = soc_SOURCE_VPm_field32_get(unit, &svp, TRUST_DSCP_V4f);
    }
    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V6f)) {
        mode_ipv6 = soc_SOURCE_VPm_field32_get(unit, &svp, TRUST_DSCP_V6f);
    }
    switch (mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            mode_ipv4 = 0;
            mode_ipv6 = 0;
            break;
        case BCM_PORT_DSCP_MAP_ZERO:
            if (soc_feature(unit, soc_feature_dscp_map_mode_all)) {
                return BCM_E_UNAVAIL;
            }
            /* coverity[MISSING_BREAK : FALSE] */
        case BCM_PORT_DSCP_MAP_ALL:
            mode_ipv4 = 1;
            mode_ipv6 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV4_ONLY:
            mode_ipv4 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV6_ONLY:
            mode_ipv6 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV4_NONE:
            mode_ipv4 = 0;
            break;
        case BCM_PORT_DSCP_MAP_IPV6_NONE:
            mode_ipv6 = 0;
            break;
    }
    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V4f)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_V4f, mode_ipv4);
    }
    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V6f)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DSCP_V6f, mode_ipv6);
    }
    /* coverity[negative_returns : FALSE] */
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_vp_dscp_map_set
 * Description:
 *      Set packet dscp to internal priority and internal dscp
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      srccp        - (IN) Packet dscp
 *      mapcp        - (IN) Internal dscp
 *      prio         - (IN) Internal priority
 * Return Value:
 *      BCM_E_XXX
 *
 * Note:
 *      This function should be invoked after initializing SVP DSCP_PTR
 */
int
_bcm_tr2_vp_dscp_map_set(int unit, bcm_gport_t port, int srccp, int mapcp,
                         int prio)
{
    source_vp_entry_t        svp;
    void                     *entries[2];
    uint32                   old_profile_index = 0;
    int                      index = 0;
    int                      rv = BCM_E_NONE;
    dscp_table_entry_t       dscp_table[64];
    int                      offset = 0, i = 0, cng;
    void                     *entry;
    int                      vp = -1;
    bcm_module_t             mod_out;
    bcm_port_t               port_out;
    bcm_trunk_t              trunk_out;

    /* Deal with different types of gports */
    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &mod_out,
                                   &port_out, &trunk_out, &vp));
    } else {
        return BCM_E_PORT;
    }

    cng = 0; /* Green */
    if (prio < BCM_PRIO_MIN) {
        return BCM_E_PARAM;
    }
    if (prio & BCM_PRIO_RED) {
        cng = 0x01;  /* Red */
        prio &= ~BCM_PRIO_RED;
    } else if (prio & BCM_PRIO_YELLOW) {
        cng = 0x03;  /* Yellow  */
        prio &= ~BCM_PRIO_YELLOW;
    }
    if ((prio & ~BCM_PRIO_MASK) != 0) {
        return BCM_E_PARAM;
    }
    if (prio > BCM_PRIO_MAX) {
         return BCM_E_PARAM;
    }

    if (!soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_PTRf)) {
        return BCM_E_INTERNAL;
    }

    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    old_profile_index = _BCM_QOS_MAP_CHUNK_DSCP *
                        soc_mem_field32_get(unit, SOURCE_VPm,
                                            &svp, TRUST_DSCP_PTRf);

    sal_memset(dscp_table, 0, sizeof(dscp_table));

    /* Base index of table in hardware */

    entries[0] = &dscp_table;

    rv =  _bcm_dscp_table_entry_get(unit, old_profile_index,
                                    _BCM_QOS_MAP_CHUNK_DSCP, entries);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    offset = srccp;
    /* Modify what's needed */

    if (offset < 0) {
        for (i = 0; i <= DSCP_CODE_POINT_MAX; i++) {
             entry = &dscp_table[i];
             if (SOC_IS_TRIDENT3X(unit)) {
                 soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, DSCPf, mapcp);
                 soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, DSCP_VALIDf, 1);
                 soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, INT_PRIf, prio);
                 soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, CNGf, cng);
             } else {
                 soc_DSCP_TABLEm_field32_set(unit, entry, DSCPf, mapcp);
                 soc_DSCP_TABLEm_field32_set(unit, entry, PRIf, prio);
                 soc_DSCP_TABLEm_field32_set(unit, entry, CNGf, cng);
             }
        }
    } else {
        entry = &dscp_table[offset];
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, DSCPf, mapcp);
            soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, DSCP_VALIDf, 1);
            soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, INT_PRIf, prio);
            soc_PHB_MAPPING_TBL_2m_field32_set(unit, entry, CNGf, cng);
        } else {
            soc_DSCP_TABLEm_field32_set(unit, entry, DSCPf, mapcp);
            soc_DSCP_TABLEm_field32_set(unit, entry, PRIf, prio);
            soc_DSCP_TABLEm_field32_set(unit, entry, CNGf, cng);
        }
    }

    /* Add new chunk and store new HW index */

    rv = _bcm_dscp_table_entry_add(unit, entries, _BCM_QOS_MAP_CHUNK_DSCP,
                                   (uint32 *)&index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (index != old_profile_index) {
        soc_mem_field32_set(unit, SOURCE_VPm, &svp, TRUST_DSCP_PTRf,
                            index / _BCM_QOS_MAP_CHUNK_DSCP);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (BCM_FAILURE(rv)) {
            return rv;;
        }
    }

    rv = _bcm_dscp_table_entry_delete(unit, old_profile_index);
    return rv;
}
#endif

/*
 * Function:
 *      _bcm_tr2_vp_ing_pri_cng_set
 * Description:
 *      Set packet priority and cfi to internal priority and color mapping
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      untagged     - (IN) For untagged packet (ignore pkt_pri, cfi argumnet)
 *      pkt_pri      - (IN) Packet priority (802.1p cos)
 *      cfi          - (IN) Packet CFI
 *      int_pri      - (IN) Internal priority
 *      color        - (IN) Color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      When both pkt_pri and cfi are -1, the setting is for untagged packet
 *      This function should be invoked after initializing SVP DOT1P_PTR
 */
int
_bcm_tr2_vp_ing_pri_cng_set(int unit, bcm_gport_t port, int untagged,
                            int pkt_pri, int cfi,
                            int int_pri, bcm_color_t color)
{
    source_vp_entry_t        svp;
    ing_pri_cng_map_entry_t  map[16];
    phb_mapping_tbl_1_entry_t phb_new_map[_BCM_QOS_MAP_CHUNK_PRI_CNG_MAX];
    ing_untagged_phb_entry_t phb;
    void                     *entries[2], *entry;
    uint32                   profile_index, old_profile_index;
    int                      index;
    int                      pkt_pri_cur, pkt_pri_min, pkt_pri_max;
    int                      cfi_cur, cfi_min, cfi_max;
    int                      rv = BCM_E_NONE;
    int                      vp = -1;
    bcm_module_t             mod_out;
    bcm_port_t               port_out;
    bcm_trunk_t              trunk_out;

    /* Deal with different types of gports */
    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &mod_out,
                                   &port_out, &trunk_out, &vp));
    } else {
        return BCM_E_PORT;
    }

    if (pkt_pri < 0) {
        pkt_pri_min = 0;
        pkt_pri_max = 7;
    } else {
        pkt_pri_min = pkt_pri;
        pkt_pri_max = pkt_pri;
    }

    if (cfi < 0) {
        cfi_min = 0;
        cfi_max = 1;
    } else {
        cfi_min = cfi;
        cfi_max = cfi;
    }

    if (!soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DOT1P_PTRf)) {
        return BCM_E_INTERNAL;
    }

    /* Get profile index from source_vp table. */
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    old_profile_index =
        soc_mem_field32_get(unit, SOURCE_VPm, &svp, TRUST_DOT1P_PTRf) * _BCM_QOS_MAP_CHUNK_PRI_CNG;

    if (SOC_IS_TRIDENT3X(unit)) {
        entries[0] = phb_new_map;
    } else {
        entries[0] = map;
    }
    entries[1] = &phb;

    rv = _bcm_ing_pri_cng_map_entry_get(unit, old_profile_index, _BCM_QOS_MAP_CHUNK_PRI_CNG, entries);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (untagged) {
         if (SOC_IS_TRIDENT3X(unit)) {
             /* TD3 tagged and untagged shared the same table
                bit 5 is indicating untaged packet or not */
             /* Offset within table */
             index  = 1 << 4 | pkt_pri << 1 | cfi;

             /* Modify what's needed */
             entry = &phb_new_map[index];
             soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, entry,
                                 INT_PRIf, int_pri);
             soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, entry,
                                 CNGf, _BCM_COLOR_ENCODING(unit, color));
        } else {
            if (int_pri >= 0) {
                soc_mem_field32_set(unit, ING_UNTAGGED_PHBm, &phb, PRIf, int_pri);
            }
            if (color >= 0) {
                soc_mem_field32_set(unit, ING_UNTAGGED_PHBm, &phb, CNGf,
                                    _BCM_COLOR_ENCODING(unit, color));
            }
        }
    } else {
        for (pkt_pri_cur = pkt_pri_min; pkt_pri_cur <= pkt_pri_max;
             pkt_pri_cur++) {
            for (cfi_cur = cfi_min; cfi_cur <= cfi_max; cfi_cur++) {
                index = (pkt_pri_cur << 1) | cfi_cur;
                if (int_pri >= 0) {
                    if (SOC_IS_TRIDENT3X(unit)) {
                        soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &phb_new_map[index],
                                            INT_PRIf, int_pri);
                    } else {
                        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &map[index],
                                            PRIf, int_pri);
                    }
                }
                if (color >= 0) {
                    if (SOC_IS_TRIDENT3X(unit)) {
                        soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &phb_new_map[index],
                                            CNGf, _BCM_COLOR_ENCODING(unit, color));
                    } else {
                        soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &map[index],
                                            CNGf, _BCM_COLOR_ENCODING(unit, color));
                    }
                }
            }
        }
    }

    rv = _bcm_ing_pri_cng_map_entry_add(unit, entries, 16, &profile_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (profile_index != old_profile_index) {
        soc_mem_field32_set(unit, SOURCE_VPm, &svp, TRUST_DOT1P_PTRf,
                            profile_index / 16);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    rv = _bcm_ing_pri_cng_map_entry_delete(unit, old_profile_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return rv;
}

#ifdef INCLUDE_L3
int
_bcm_tr2_vp_dscp_map_mode_get(int unit, bcm_gport_t port, int *mode)
{
    source_vp_entry_t svp;
    int rv = BCM_E_NONE;
    uint32 vp = -1;
    int mode_ipv4 = 0, mode_ipv6 = 0;
    /* Deal with different types of gports */
    if (_BCM_QOS_GPORT_IS_VFI_VP_PORT(port)) {
        /* Deal with MiM and MPLS ports */
        if (BCM_GPORT_IS_MIM_PORT(port)) {
            vp = BCM_GPORT_MIM_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                return BCM_E_BADID;
            }
        } else if BCM_GPORT_IS_VXLAN_PORT(port) {
            vp = BCM_GPORT_VXLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                return BCM_E_BADID;
            }
        } else if BCM_GPORT_IS_L2GRE_PORT(port) {
            vp = BCM_GPORT_L2GRE_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                return BCM_E_BADID;
            }
        } else if BCM_GPORT_IS_FLOW_PORT(port) {
            vp = BCM_GPORT_FLOW_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
                return BCM_E_BADID;
            }
        } else {
            vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                return BCM_E_BADID;
            }
        }
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V4f)) {
        mode_ipv4 = soc_SOURCE_VPm_field32_get(unit, &svp, TRUST_DSCP_V4f);
    }
    if (soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_V6f)) {
        mode_ipv6 = soc_SOURCE_VPm_field32_get(unit, &svp, TRUST_DSCP_V6f);
    }
    if (mode_ipv4 && mode_ipv6) {
        if (soc_feature(unit, soc_feature_dscp_map_mode_all)) {
            *mode = BCM_PORT_DSCP_MAP_ALL;
        } else {
            *mode = BCM_PORT_DSCP_MAP_ZERO;
        }
    } else if (mode_ipv4) {
        *mode = BCM_PORT_DSCP_MAP_IPV4_ONLY;
    } else if (mode_ipv6) {
        *mode = BCM_PORT_DSCP_MAP_IPV6_ONLY;
    } else {
        *mode = BCM_PORT_DSCP_MAP_NONE;
    }
    return rv;
}


/*
 * Function:
 *      _bcm_tr2_vp_dscp_map_get
 * Description:
 *      Get packet dscp to internal priority and internal dscp
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      srccp        - (IN) Packet dscp
 *      mapcp        - (OUT) Internal dscp
 *      prio         - (OUT) Internal priority
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_tr2_vp_dscp_map_get(int unit, bcm_gport_t port, int srccp, int *mapcp,
                      int *prio)
{
    source_vp_entry_t       svp;
    int                     rv = BCM_E_NONE;
    int                     index = 0;
    int                     base;
    int                     cng;
    uint32 de[SOC_MAX_MEM_FIELD_WORDS];
    int                     vp = -1;
    bcm_module_t            mod_out;
    bcm_port_t              port_out;
    bcm_trunk_t             trunk_out;

    /* Deal with different types of gports */
    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &mod_out,
                                   &port_out, &trunk_out, &vp));
    } else {
        return BCM_E_PORT;
    }

    if (!soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_PTRf)) {
        return BCM_E_INTERNAL;
    }

    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    index = soc_mem_field32_get(unit, SOURCE_VPm, &svp, TRUST_DSCP_PTRf);

    if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX ||
        mapcp == NULL || prio == NULL) {
        return BCM_E_PARAM;
    }

    /* look up in DSCP_TABLEm */
    if (srccp < 0) {
        srccp = 0;
    }

    base = (index * DSCP_CODE_POINT_CNT);

    if (SOC_IS_TRIDENT3X(unit)) {
        SOC_IF_ERROR_RETURN(
                READ_PHB_MAPPING_TBL_2m(unit, MEM_BLOCK_ANY, base + srccp, &de));
        *mapcp = soc_PHB_MAPPING_TBL_2m_field32_get(unit, &de, DSCPf);
        *prio = soc_PHB_MAPPING_TBL_2m_field32_get(unit, &de, INT_PRIf);

        cng   = soc_PHB_MAPPING_TBL_2m_field32_get(unit, &de, CNGf);
    } else {
        SOC_IF_ERROR_RETURN(
             READ_DSCP_TABLEm(unit, MEM_BLOCK_ANY, base + srccp, &de));
        *mapcp = soc_DSCP_TABLEm_field32_get(unit, &de, DSCPf);
        *prio = soc_DSCP_TABLEm_field32_get(unit, &de, PRIf);

        cng   = soc_DSCP_TABLEm_field32_get(unit, &de, CNGf);
    }

    if (cng == 1) {
        *prio |= BCM_PRIO_RED;
    } else if (cng == 3) {
        *prio |= BCM_PRIO_YELLOW;
    } else if (cng != 0) {
        *prio |= BCM_PRIO_DROP_FIRST;
    }

    return BCM_E_NONE;
}
#endif

#ifdef BCM_SABER2_SUPPORT
/* Called from OAM module to get the index and the priority mode
 * configured.
 */
int _bcm_sb2_ing_service_pri_map_info_get(int unit, int pri_map_id,
                                          uint32 *hw_idx, uint32 *pri_mode)
{
    int id = 0;
    if (soc_feature(unit, soc_feature_oam_service_pri_map)) {
        id = pri_map_id & _BCM_QOS_MAP_TYPE_MASK;
        if (_BCM_QOS_SERVICE_PRI_MAP_TABLE_USED_GET(unit, id)) {
            *hw_idx = QOS_INFO(unit)->service_pri_map_hw_idx[id] *
                                    _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP;
            *pri_mode = _BCM_QOS_SERVICE_PRI_MAP_TABLE_FLAGS_GET(unit, id);
            return BCM_E_NONE;
        }
    } else {
        return BCM_E_UNAVAIL;
    }
    /* Invalid unit or pri_map_id if it reaches here */
    return BCM_E_PARAM;
}

int _bcm_sb2_service_pri_map_info_id_get(int unit, uint32 hw_idx, int *pri_map_id)
{
   return _bcm_tr2_qos_idx2id(unit, (hw_idx / _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP),
                               _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE, pri_map_id);
}

int _bcm_sb2_service_pri_map_reference_add(int unit, int pri_map_id)
{
    int id = 0;
    id = pri_map_id & _BCM_QOS_MAP_TYPE_MASK;
    BCM_IF_ERROR_RETURN(_bcm_service_pri_map_table_entry_reference(unit,
                ((QOS_INFO(unit)->service_pri_map_hw_idx[id]) *
                 _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP),
                _BCM_QOS_MAP_CHUNK_SERVICE_PRI_MAP));
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_tr2_vp_ing_pri_cng_get
 * Description:
 *      Get packet priority and cfi to internal priority and color mapping
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      untagged     - (IN) For untagged packet (ignore pkt_pri, cfi argumnet)
 *      pkt_pri      - (IN) Packet priority (802.1p cos)
 *      cfi          - (IN) Packet CFI
 *      int_pri      - (OUT) Internal priority
 *      color        - (OUT) Color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      When both pkt_pri and cfi are -1, the setting is for untagged packet
 */
int
_bcm_tr2_vp_ing_pri_cng_get(int unit, bcm_gport_t port, int untagged,
                            int pkt_pri, int cfi,
                            int *int_pri, bcm_color_t *color)
{
    source_vp_entry_t        svp;
    ing_pri_cng_map_entry_t  map[16];
    phb_mapping_tbl_1_entry_t phb_new_map[_BCM_QOS_MAP_CHUNK_PRI_CNG_MAX];
    ing_untagged_phb_entry_t phb;
    void                     *entries[2];
    uint32                   profile_index;
    int                      index;
    int                      hw_color;
    int                      vp = -1;
    bcm_module_t             mod_out;
    bcm_port_t               port_out;
    bcm_trunk_t              trunk_out;

    /* Deal with different types of gports */
    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &mod_out,
                                   &port_out, &trunk_out, &vp));
    } else {
        return BCM_E_PORT;
    }

    if (!soc_mem_field_valid(unit,SOURCE_VPm,TRUST_DSCP_PTRf)) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));

    profile_index =
        soc_mem_field32_get(unit, SOURCE_VPm, &svp, TRUST_DOT1P_PTRf) * _BCM_QOS_MAP_CHUNK_PRI_CNG;

    if (SOC_IS_TRIDENT3X(unit)) {
        entries[0] = phb_new_map;
    } else {
        entries[0] = map;
    }
    entries[1] = &phb;

    BCM_IF_ERROR_RETURN
        (_bcm_ing_pri_cng_map_entry_get(unit, profile_index, _BCM_QOS_MAP_CHUNK_PRI_CNG, entries));
    if (untagged) {
        if (SOC_IS_TRIDENT3X(unit)) {
            index = 1<<4 | (pkt_pri << 1) | cfi;
            if (int_pri != NULL) {
                *int_pri = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, &phb_new_map[index], PRIf);
            }
            if (color != NULL) {
                hw_color = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, &phb_new_map[index], CNGf);
                *color = _BCM_COLOR_DECODING(unit, hw_color);
            }
        } else {
            if (int_pri != NULL) {
                *int_pri = soc_mem_field32_get(unit, ING_UNTAGGED_PHBm, &phb,
                                               PRIf);
            }
            if (color != NULL) {
                hw_color = soc_mem_field32_get(unit, ING_UNTAGGED_PHBm, &phb,
                                           CNGf);
                *color = _BCM_COLOR_DECODING(unit, hw_color);
            }
        }
    } else {
        index = (pkt_pri << 1) | cfi;
        if (SOC_IS_TRIDENT3X(unit)) {
            if (int_pri != NULL) {
                *int_pri = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, &phb_new_map[index], PRIf);
            }
            if (color != NULL) {
                hw_color = soc_mem_field32_get(unit, PHB_MAPPING_TBL_1m, &phb_new_map[index], CNGf);
                *color = _BCM_COLOR_DECODING(unit, hw_color);
            }
        } else {
            if (int_pri != NULL) {
                *int_pri = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &map[index],
                                               PRIf);
            }
            if (color != NULL) {
                hw_color = soc_mem_field32_get(unit, ING_PRI_CNG_MAPm, &map[index],
                                               CNGf);
                *color = _BCM_COLOR_DECODING(unit, hw_color);
            }
        }
    }

    return BCM_E_NONE;
}

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
int
bcm_td3_mpls_combo_add (int unit, uint32 flags, bcm_mpls_exp_map_t *mpls_map,
        int map_id, uint32 hw_idx, uint32 entry_per_index)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t qos_map;

      if (flags == BCM_QOS_MAP_EGRESS)
      {
          if (mpls_map == NULL) {
             return BCM_E_PARAM;
          }
          sal_memset(&qos_map, 0x0, sizeof(bcm_qos_map_t));
          qos_map.int_pri  = mpls_map->priority;
          qos_map.exp      = mpls_map->exp;
          qos_map.pkt_pri  = mpls_map->pkt_pri;
          qos_map.pkt_cfi  = mpls_map->pkt_cfi;
          /*color validation check to continue Legacy
           *behaviour of mpls exp map config*/
          if (mpls_map->color > bcmColorRed) {
              mpls_map->color = bcmColorGreen;
          }
          qos_map.color    = mpls_map->color;
          rv = bcm_td3_l2_egr_combo_add (unit, BCM_QOS_MAP_MPLS | BCM_QOS_MAP_EGRESS, &qos_map, map_id,
                      &hw_idx, _BCM_QOS_MAP_CHUNK_EGR_MPLS);
      }
    return rv;
}
#endif

#endif
