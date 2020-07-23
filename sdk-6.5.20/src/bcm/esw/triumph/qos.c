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

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH_SUPPORT)

#include <shared/util.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/qos.h>
#include <bcm/error.h>
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

#define _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP    1
#define _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS      2
#define _BCM_QOS_MAP_TYPE_DSCP_TABLE         3
/* _BCM_QOS_MAP_TYPE_* ids 6-8 are defined in bcm_int/esw/trident2.h 
 * and used in esw/trident2/qos.c.  Make sure not to reuse them if new
 * types are added here.
 */

#define _BCM_QOS_MAP_TYPE_MASK           0x3ff
#define _BCM_QOS_MAP_SHIFT                  10

#define _BCM_QOS_MAP_CHUNK_PRI_CNG  16
#define _BCM_QOS_MAP_CHUNK_EGR_MPLS  64
#define _BCM_QOS_MAP_CHUNK_DSCP  64
#define _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP  8

#define _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP    \
            (soc_mem_index_count(unit, ING_PRI_CNG_MAPm)/ \
             _BCM_QOS_MAP_CHUNK_PRI_CNG)
#define _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS      \
            (soc_mem_index_count(unit, EGR_MPLS_PRI_MAPPINGm)/ \
             _BCM_QOS_MAP_CHUNK_EGR_MPLS)
#define _BCM_QOS_MAP_LEN_DSCP_TABLE         \
            (soc_mem_index_count(unit, DSCP_TABLEm)/ \
             _BCM_QOS_MAP_CHUNK_DSCP)
#define _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP   \
            (soc_mem_is_valid(unit, ING_MPLS_EXP_MAPPINGm) ? \
             (soc_mem_index_count(unit, ING_MPLS_EXP_MAPPINGm)/ \
              _BCM_QOS_MAP_CHUNK_ING_MPLS_EXP) : 0)

typedef struct _bcm_tr_qos_bookkeeping_s {
    SHR_BITDCL *ing_pri_cng_bitmap; /* ING_PRI_CNG_MAP chunks used */
    uint32 *pri_cng_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_mpls_bitmap; /* EGR_MPLS_EXP_MAPPING_1 / 
                                  EGR_MPLS_PRI_MAPPING chunks used */
    uint32 *egr_mpls_hw_idx; /* Actual profile number used */
    SHR_BITDCL *dscp_table_bitmap; /* DSCP_TABLE chunks used */
    uint32 *dscp_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_mpls_bitmap_flags; /* indicates if egr_mpls_bitmap
                                          entry is L2 or MPLS */
    SHR_BITDCL *ing_mpls_exp_bitmap; /* indicates which chunks of 
                                ING_MPLS_EXP_MAPPINGm are used by QOS module */
} _bcm_tr_qos_bookkeeping_t;

STATIC _bcm_tr_qos_bookkeeping_t  _bcm_tr_qos_bk_info[BCM_MAX_NUM_UNITS];
STATIC sal_mutex_t _tr_qos_mutex[BCM_MAX_NUM_UNITS] = {NULL};
STATIC int tr_qos_initialized[BCM_MAX_NUM_UNITS]; /* Flag to check init status */

/*
 * QoS resource lock
 */
#define QOS_LOCK(unit) \
        sal_mutex_take(_tr_qos_mutex[unit], sal_mutex_FOREVER);

#define QOS_UNLOCK(unit) \
        sal_mutex_give(_tr_qos_mutex[unit]);

#define QOS_INFO(_unit_) (&_bcm_tr_qos_bk_info[_unit_])
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

/*
 * EGR_MPLS_EXP_MAPPING_1 / EGR_MPLS_PRI_MAPPING usage bitmap operations
 */
#define _BCM_QOS_EGR_MPLS_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_mpls_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_mpls_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_mpls_bitmap, (_identifier_))

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
        if (!tr_qos_initialized[unit]) {                           \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

STATIC int
_bcm_tr_qos_id_alloc(int unit, SHR_BITDCL *bitmap, uint8 map_type)
{
    int i, map_size = -1;

    switch (map_type) {
        case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
            map_size = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
            break;
        case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
            map_size = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
            break;
        case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
            map_size = _BCM_QOS_MAP_LEN_DSCP_TABLE;
            break;
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
_bcm_tr_qos_sw_dump(int unit)
{
    int     i;

    if (!tr_qos_initialized[unit]) {
        LOG_CLI((BSL_META_U(unit,
                            "ERROR: QOS module not initialized\n")));
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
                        "QOS: ING_MPLS_EXP_MAPPING info \n")));
    for (i=0; i<_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP; i++) {
        if (_BCM_QOS_ING_MPLS_EXP_USED_GET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                                "    map id:%4d\n"), i));
        }
    }
}
#endif

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1
#define _BCM_TR_REINIT_INVALID_HW_IDX  0xff    /* used as an invalid hw idx */

/* 
 * Function:
 *      _bcm_tr_qos_reinit_scache_len_get
 * Purpose:
 *      Helper utility to determine scache details.
 * Parameters:
 *      unit        : (IN) Device Unit Number
 *      scache_len  : (OUT) Total required scache length
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_qos_reinit_scache_len_get(int unit, uint32* scache_len)
{
    if (scache_len == NULL) {
        return BCM_E_PARAM;
    }

    *scache_len = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
    *scache_len += _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
    *scache_len += _BCM_QOS_MAP_LEN_DSCP_TABLE;
    *scache_len += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
    *scache_len += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP);

    return BCM_E_NONE;
}

int
_bcm_tr_qos_reinit_from_hw_state(int unit, soc_mem_t mem, soc_field_t field,
                                 uint8 map_type, SHR_BITDCL *hw_idx_bmp, 
                                 int hw_idx_bmp_len)
{
    int         prof_count, rv = BCM_E_NONE;
    int         i, idx, min_idx, max_idx, map_id, hw_prof_idx;
    uint32      buf[SOC_MAX_MEM_FIELD_WORDS];
    uint32      *hw_idx_table;
    SHR_BITDCL  *map_bmp;
    soc_field_t prof_field[4];

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
    default:
        return BCM_E_PARAM;
    }

    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);
    if (mem == EGR_IP_TUNNEL_MPLSm) {
        prof_count = 4;
        prof_field[0] = MPLS_EXP_MAPPING_PTR_0f;
        prof_field[1] = MPLS_EXP_MAPPING_PTR_1f;
        prof_field[2] = MPLS_EXP_MAPPING_PTR_2f;
        prof_field[3] = MPLS_EXP_MAPPING_PTR_3f;
    } else {
        prof_count = 1;
        prof_field[0] = field;
    }
    for (idx = min_idx; idx < max_idx; idx++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &buf);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "Error(%s) reading mem(%d) at "
                                  "index:%d \n"), soc_errmsg(rv), mem, idx));
            return rv;
        }
        if (soc_mem_field_valid(unit, mem, MPLS_ACTION_IF_BOSf)) {
            if (soc_mem_field32_get(unit, mem, &buf, 
                                    MPLS_ACTION_IF_BOSf) != 1) {
                continue;
            }
        }
        for (i = 0; i < prof_count; i++) {
            hw_prof_idx = soc_mem_field32_get(unit, mem, &buf, prof_field[i]);
            if (hw_prof_idx == 0x3f && mem == L3_IIFm &&
                                                     field == TRUST_DSCP_PTRf) {
                continue;
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
                map_id = _bcm_tr_qos_id_alloc(unit, map_bmp, map_type);
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
    }
    
    return rv;
}

/* 
 * Function:
 *      _bcm_tr_qos_unsynchronized_reinit
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
_bcm_tr_qos_unsynchronized_reinit(int unit)
{
    int                 rv = BCM_E_NONE;
    int                 bmp_len;
    SHR_BITDCL          *temp_bmp;

    /* read hw tables and populate the state */
    bmp_len = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
    temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
    sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
    
    if (soc_mem_is_valid(unit, MPLS_ENTRYm)) {
        rv = _bcm_tr_qos_reinit_from_hw_state(unit, MPLS_ENTRYm, 
                     TRUST_OUTER_DOT1P_PTRf, _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP,
                     temp_bmp, bmp_len);
    }
    sal_free(temp_bmp);

    /* now extract EGR_MPLS_PRI_MAPPING state */
    if (BCM_SUCCESS(rv)) {
        bmp_len = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
        temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
        sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
        if (BCM_SUCCESS(rv) && 
            soc_mem_field_valid(unit, EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                                MPLS_EXP_MAPPING_PTRf)) {
            rv = _bcm_tr_qos_reinit_from_hw_state(unit, 
                     EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, 
                     MPLS_EXP_MAPPING_PTRf, _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                     temp_bmp, bmp_len);
        }
        if (BCM_SUCCESS(rv) && 
            soc_mem_field_valid(unit, EGR_IP_TUNNEL_MPLSm, 
                                MPLS_EXP_MAPPING_PTR_0f)) {
            rv = _bcm_tr_qos_reinit_from_hw_state(unit, 
                     EGR_IP_TUNNEL_MPLSm, 
                     MPLS_EXP_MAPPING_PTR_0f, _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                     temp_bmp, bmp_len);
        }
        sal_free(temp_bmp);
    }

    /* now extract DSCP_TABLE state */
    if (BCM_SUCCESS(rv)) {
        bmp_len = _BCM_QOS_MAP_LEN_DSCP_TABLE;
        temp_bmp = sal_alloc(SHR_BITALLOCSIZE(bmp_len), "temp_bmp");
        sal_memset(temp_bmp, 0x0, SHR_BITALLOCSIZE(bmp_len));
        if (soc_mem_is_valid(unit, L3_IIFm)) {
            rv = _bcm_tr_qos_reinit_from_hw_state(unit, L3_IIFm, 
                                                  TRUST_DSCP_PTRf,
                              _BCM_QOS_MAP_TYPE_DSCP_TABLE, temp_bmp, bmp_len);
        }
        sal_free(temp_bmp);
    }

    return rv;
}

/* 
 * Function:
 *      _bcm_tr_qos_extended_reinit
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
_bcm_tr_qos_extended_reinit(int unit)
{
    soc_scache_handle_t handle;
    uint16              recovered_ver;
    uint8               *scache_ptr;
    uint32              hw_idx, scache_len;
    int                 idx;
    int                 rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_tr_qos_reinit_scache_len_get(unit, &scache_len));
    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_QOS, 0);

    if (SOC_WARM_BOOT(unit)) {
        /* during warm-boot recover state */
        rv = _bcm_esw_scache_ptr_get(unit, handle, FALSE,
                                      scache_len, &scache_ptr, 
                                      BCM_WB_DEFAULT_VERSION, &recovered_ver);

        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }

        if (rv == BCM_E_NOT_FOUND) {
            BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, handle,
                      TRUE, scache_len, &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));
        } else {

            if (recovered_ver >= BCM_WB_VERSION_1_1) {
                /* recover from scache into book-keeping structs */
                for (idx = 0; idx < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; idx++) {
                    hw_idx = (uint32) (*scache_ptr++);
                    if (hw_idx != _BCM_TR_REINIT_INVALID_HW_IDX) {
                        _BCM_QOS_ING_PRI_CNG_USED_SET(unit, idx);
                        QOS_INFO(unit)->pri_cng_hw_idx[idx] = hw_idx;
                    }
                }
                for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
                    hw_idx = (uint32) (*scache_ptr++);
                    if (hw_idx != _BCM_TR_REINIT_INVALID_HW_IDX) {
                        _BCM_QOS_EGR_MPLS_USED_SET(unit, idx);
                        QOS_INFO(unit)->egr_mpls_hw_idx[idx] = hw_idx;
                    }
                }
                for (idx = 0; idx < _BCM_QOS_MAP_LEN_DSCP_TABLE; idx++) {
                    hw_idx = (uint32) (*scache_ptr++);
                    if (hw_idx != _BCM_TR_REINIT_INVALID_HW_IDX) {
                        _BCM_QOS_DSCP_TABLE_USED_SET(unit, idx);
                        QOS_INFO(unit)->dscp_hw_idx[idx] = hw_idx;
                    }
                }
                sal_memcpy(QOS_INFO(unit)->egr_mpls_bitmap_flags, scache_ptr, 
                           SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS));
                scache_ptr += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
                sal_memcpy(QOS_INFO(unit)->ing_mpls_exp_bitmap, scache_ptr, 
                           SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP));
            }
        }
    } else {
        /* During cold-boot. Allocate a stable cache if not already done */
        BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, handle,
                  TRUE, scache_len, &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));
    }

    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_tr_qos_reinit_hw_profiles_update
 * Purpose:
 *      Updates the shared memory profile tables reference counts
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_qos_reinit_hw_profiles_update (int unit)
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
        if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(_bcm_dscp_table_entry_reference(unit, 
                                 ((QOS_INFO(unit)->dscp_hw_idx[i]) * 
                                  _BCM_QOS_MAP_CHUNK_DSCP), 
                                 _BCM_QOS_MAP_CHUNK_DSCP));
        }
    }

    return BCM_E_NONE;
}

/* 
 * Function:
 *      _bcm_tr_qos_reinit
 * Purpose:
 *      Top level QOS init routine for Warm-Boot
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_qos_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int stable_size = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (stable_size == 0) {
        if (!SOC_WARM_BOOT(unit)) {
            return rv; /* do nothing in cold-boot */
        }
        rv = _bcm_tr_qos_unsynchronized_reinit(unit);
    } else {
        /* Limited is same as extended */
        rv = _bcm_tr_qos_extended_reinit(unit);
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_tr_qos_reinit_hw_profiles_update(unit);
    }

    return rv;
}

/* 
 * Function:
 *      _bcm_tr_qos_sync
 * Purpose:
 *      This routine extracts the state that needs to be stored from the 
 *      book-keeping structs and stores it in the allocated scache location 
 * Compression format:  0   - A     pri_cng_hw_idx table
 *                      A+1 - B     egr_mpls_hw_idx table
 *                      D+1 - E     egr_mpls flags
 *                      E+1 - F     ing_mpls_exp table usage by QOS module
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_qos_sync(int unit)
{
    soc_scache_handle_t handle;
    uint8               *scache_ptr;
    uint32              hw_idx, scache_len;
    int                 idx;
    
    BCM_IF_ERROR_RETURN(_bcm_tr_qos_reinit_scache_len_get(unit, &scache_len));
    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_QOS, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, handle, FALSE,
                                                scache_len, &scache_ptr, 
                                                BCM_WB_DEFAULT_VERSION, NULL));

    /* now store the state into the compressed format */
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP; idx++) {
        if (_BCM_QOS_ING_PRI_CNG_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->pri_cng_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS; idx++) {
        if (_BCM_QOS_EGR_MPLS_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->egr_mpls_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    for (idx = 0; idx < _BCM_QOS_MAP_LEN_DSCP_TABLE; idx++) {
        if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, idx)) {
            hw_idx = QOS_INFO(unit)->dscp_hw_idx[idx];
        } else {
            hw_idx = _BCM_TR_REINIT_INVALID_HW_IDX;
        }
        *scache_ptr++ = hw_idx;
    }
    sal_memcpy(scache_ptr, QOS_INFO(unit)->egr_mpls_bitmap_flags, 
               SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS));
    scache_ptr += SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_EGR_MPLS_MAPS);
    sal_memcpy(scache_ptr, QOS_INFO(unit)->ing_mpls_exp_bitmap, 
               SHR_BITALLOCSIZE(_BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP));

    return BCM_E_NONE;
}
#endif

STATIC void
_bcm_tr_qos_free_resources(int unit)
{
    _bcm_tr_qos_bookkeeping_t *qos_info = QOS_INFO(unit);

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

    /* Destroy egress profile usage bitmap */
    if (qos_info->egr_mpls_bitmap) {
        sal_free(qos_info->egr_mpls_bitmap);
        qos_info->egr_mpls_bitmap = NULL;
    }
    if (qos_info->egr_mpls_hw_idx) {
        sal_free(qos_info->egr_mpls_hw_idx);
        qos_info->egr_mpls_hw_idx = NULL;
    }

    /* Destroy DSCP table profile usage bitmap */
    if (qos_info->dscp_table_bitmap) {
        sal_free(qos_info->dscp_table_bitmap);
        qos_info->dscp_table_bitmap = NULL;
    }
    if (qos_info->dscp_hw_idx) {
        sal_free(qos_info->dscp_hw_idx);
        qos_info->dscp_hw_idx = NULL;
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

    /* Destroy the mutex */
    if (_tr_qos_mutex[unit]) {
        sal_mutex_destroy(_tr_qos_mutex[unit]);
        _tr_qos_mutex[unit] = NULL;
    }
}

/* Initialize the QoS module. */
int 
bcm_tr_qos_init(int unit)
{
    _bcm_tr_qos_bookkeeping_t *qos_info = QOS_INFO(unit);
    int ing_profiles, egr_profiles, rv = BCM_E_NONE;
    int dscp_profiles, ing_mpls_profiles;
    ing_profiles = _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP;
    egr_profiles = _BCM_QOS_MAP_LEN_EGR_MPLS_MAPS;
    dscp_profiles = _BCM_QOS_MAP_LEN_DSCP_TABLE;
    ing_mpls_profiles = _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP;

    if (tr_qos_initialized[unit]) {
        BCM_IF_ERROR_RETURN(bcm_tr_qos_detach(unit));
    }

    /* Create mutex */
    if (NULL == _tr_qos_mutex[unit]) {
        _tr_qos_mutex[unit] = sal_mutex_create("qos mutex");
        if (_tr_qos_mutex[unit] == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    /* Allocate ingress profile usage bitmap */
    if (NULL == qos_info->ing_pri_cng_bitmap) {
        qos_info->ing_pri_cng_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(ing_profiles), "ing_pri_cng_bitmap");
        if (qos_info->ing_pri_cng_bitmap == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->ing_pri_cng_bitmap, 0, SHR_BITALLOCSIZE(ing_profiles));
    if (NULL == qos_info->pri_cng_hw_idx) {
        qos_info->pri_cng_hw_idx = 
            sal_alloc(sizeof(uint32) * ing_profiles, "pri_cng_hw_idx");
        if (qos_info->pri_cng_hw_idx == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->pri_cng_hw_idx, 0, (sizeof(uint32) * ing_profiles));

    /* Allocate egress profile usage bitmap */
    if (NULL == qos_info->egr_mpls_bitmap) {
        qos_info->egr_mpls_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(egr_profiles), "egr_mpls_bitmap");
        if (qos_info->egr_mpls_bitmap == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_mpls_bitmap, 0, SHR_BITALLOCSIZE(egr_profiles));
    if (NULL == qos_info->egr_mpls_hw_idx) {
        qos_info->egr_mpls_hw_idx = 
            sal_alloc(sizeof(uint32) * egr_profiles, "egr_mpls_hw_idx");
        if (qos_info->egr_mpls_hw_idx == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->egr_mpls_hw_idx, 0, (sizeof(uint32) * egr_profiles));

    /* Allocate DSCP_TABLE profile usage bitmap */
    if (NULL == qos_info->dscp_table_bitmap) {
        qos_info->dscp_table_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(dscp_profiles), "dscp_table_bitmap");
        if (qos_info->dscp_table_bitmap == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->dscp_table_bitmap, 0, SHR_BITALLOCSIZE(dscp_profiles));
    if (NULL == qos_info->dscp_hw_idx) {
        qos_info->dscp_hw_idx = 
            sal_alloc(sizeof(uint32) * dscp_profiles, "dscp_hw_idx");
        if (qos_info->dscp_hw_idx == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->dscp_hw_idx, 0, (sizeof(uint32) * dscp_profiles));

    /* Allocate egress profile usage bitmap */
    if (NULL == qos_info->egr_mpls_bitmap_flags) {
        qos_info->egr_mpls_bitmap_flags =
            sal_alloc(SHR_BITALLOCSIZE(egr_profiles), "egr_mpls_bitmap_flags");
        if (qos_info->egr_mpls_bitmap_flags == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_mpls_bitmap_flags, 0, SHR_BITALLOCSIZE(egr_profiles));

    /* Allocate ingress mpls profile usage bitmap */
    if (NULL == qos_info->ing_mpls_exp_bitmap) {
        qos_info->ing_mpls_exp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(ing_mpls_profiles), "ing_mpls_exp_bitmap");
        if (qos_info->ing_mpls_exp_bitmap == NULL) {
            _bcm_tr_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->ing_mpls_exp_bitmap, 0, SHR_BITALLOCSIZE(ing_mpls_profiles));

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_tr_qos_reinit(unit);
    if (SOC_FAILURE(rv)) {
        _bcm_tr_qos_free_resources(unit);
        return rv;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    tr_qos_initialized[unit] = TRUE;
    return rv;
}

/* Detach the QoS module. */
int 
bcm_tr_qos_detach(int unit)
{
    int rv = BCM_E_NONE;
    _bcm_tr_qos_free_resources(unit);
    tr_qos_initialized[unit] = FALSE;
    return rv;
}

STATIC int
_bcm_tr_qos_l2_map_create(int unit, uint32 flags, int *map_id) 
{
    ing_pri_cng_map_entry_t ing_pri_map[_BCM_QOS_MAP_CHUNK_PRI_CNG];
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
            id = _bcm_tr_qos_id_alloc(unit, QOS_INFO(unit)->ing_pri_cng_bitmap, 
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
        entries[0] = &ing_pri_map;
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
            } else {
                _BCM_QOS_EGR_MPLS_USED_SET(unit, id);
            }
        } else {
            id = _bcm_tr_qos_id_alloc(unit, QOS_INFO(unit)->egr_mpls_bitmap, 
                                   _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS);
            if (id == -1) {
                return BCM_E_RESOURCE;
            }
            _BCM_QOS_EGR_MPLS_USED_SET(unit, id);
            *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS 
                      << _BCM_QOS_MAP_SHIFT);
        }
        /* Reserve a chunk in the EGR_MPLS_EXP_MAPPING_1/EGR_MPLS_PRI_MAPPING 
           tables - these two tables always done together */
        sal_memset(egr_mpls_pri_map, 0, sizeof(egr_mpls_pri_map));
        entries[0] = egr_mpls_pri_map;
        sal_memset(egr_mpls_exp_map, 0, sizeof(egr_mpls_exp_map));
        entries[1] = egr_mpls_exp_map;
        sal_memset(egr_mpls_exp_map, 0, sizeof(egr_mpls_exp_map2));
        entries[2] = egr_mpls_exp_map2;
        BCM_IF_ERROR_RETURN(_bcm_egr_mpls_combo_map_entry_add(unit, entries, 
                                                   _BCM_QOS_MAP_CHUNK_EGR_MPLS,
                                                              (uint32 *)&index));
        QOS_INFO(unit)->egr_mpls_hw_idx[id] = (index / 
                                               _BCM_QOS_MAP_CHUNK_EGR_MPLS);
    }
    return rv;
} 

STATIC int
_bcm_tr_qos_l3_map_create(int unit, uint32 flags, int *map_id) 
{
    dscp_table_entry_t dscp_table[_BCM_QOS_MAP_CHUNK_DSCP];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[1];
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
            id = _bcm_tr_qos_id_alloc(unit, QOS_INFO(unit)->dscp_table_bitmap, 
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
        /* Only physical port based mapping is available for EGR_DSCP */
        return BCM_E_UNAVAIL;
    }
    return rv;
}

STATIC int
_bcm_tr_qos_mpls_map_create(int unit, uint32 flags, int *map_id) 
{
    if (flags & BCM_QOS_MAP_INGRESS) {
        /* The ING_MPLS_EXP_MAPPING table is used */
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        BCM_IF_ERROR_RETURN(bcm_tr_mpls_exp_map_create(unit,
                                            BCM_MPLS_EXP_MAP_INGRESS, map_id));
        _BCM_QOS_ING_MPLS_EXP_USED_SET(unit, 
                        ((*map_id) & _BCM_TR_MPLS_EXP_MAP_TABLE_NUM_MASK));
#endif
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* The EGR_MPLS_EXP_MAPPING_1 table is used */
        /* Shared with L2 egress map */
        BCM_IF_ERROR_RETURN(_bcm_tr_qos_l2_map_create(unit, 
                                                  BCM_QOS_MAP_EGRESS, map_id));
        _BCM_QOS_EGR_MPLS_FLAGS_USED_SET(unit, 
                                         ((*map_id) & _BCM_QOS_MAP_TYPE_MASK));
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/* Create a QoS map profile */
/* This will allocate an ID and a profile index with all-zero mapping */
int 
bcm_tr_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int rv = BCM_E_UNAVAIL;
    QOS_INIT(unit);
    if (flags == 0) {
        return BCM_E_PARAM;
    }
    QOS_LOCK(unit);
    if (flags & BCM_QOS_MAP_L2) {
        rv = _bcm_tr_qos_l2_map_create(unit, flags, map_id);
    } else if (flags & BCM_QOS_MAP_L3) {
        rv = _bcm_tr_qos_l3_map_create(unit, flags, map_id);
    } else if (flags & BCM_QOS_MAP_MPLS) {
        if (!soc_feature(unit, soc_feature_mpls)) {
            QOS_UNLOCK(unit);
            return rv;
        }
        rv = _bcm_tr_qos_mpls_map_create(unit, flags, map_id);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    QOS_UNLOCK(unit);
    return rv;
}

/* Destroy a QoS map profile */
/* This will free the profile index and de-allocate the ID */
int 
bcm_tr_qos_map_destroy(int unit, int map_id)
{
    int id, rv = BCM_E_UNAVAIL;
    QOS_INIT(unit);
    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
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
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_egr_mpls_combo_map_entry_delete (unit, 
                                       (QOS_INFO(unit)->egr_mpls_hw_idx[id] *
                                        _BCM_QOS_MAP_CHUNK_EGR_MPLS));
            QOS_INFO(unit)->egr_mpls_hw_idx[id] = 0;
            _BCM_QOS_EGR_MPLS_USED_CLR(unit, id);
            _BCM_QOS_EGR_MPLS_FLAGS_USED_CLR(unit, id);
        }
        break;
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
    default:
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
            if (!soc_feature(unit, soc_feature_mpls)) {
                QOS_UNLOCK(unit);
                return rv;
            }
            rv = bcm_tr_mpls_exp_map_destroy(unit, map_id);
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

/* Add an entry to a QoS map */
/* Read the existing profile chunk, modify what's needed and add the 
 * new profile. This can result in the HW profile index changing for a 
 * given ID */
int 
bcm_tr_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int alloc_size, offset, cng, id, index, rv = BCM_E_NONE;
    ing_pri_cng_map_entry_t ing_pri_map[_BCM_QOS_MAP_CHUNK_PRI_CNG];
    ing_untagged_phb_entry_t ing_untagged_phb;
    egr_mpls_pri_mapping_entry_t *egr_mpls_pri_map;
    egr_mpls_exp_mapping_1_entry_t *egr_mpls_exp_map;
    egr_mpls_exp_mapping_2_entry_t *egr_mpls_exp_map2;
    dscp_table_entry_t *dscp_table;
    void *entries[3], *entry;

    QOS_INIT(unit);

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
         }
        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L2) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) || 
                (map->pkt_pri > 7) || (map->pkt_cfi > 1) || 
                ((map->color != bcmColorGreen) && 
                (map->color != bcmColorYellow) && 
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }

            entries[0] = ing_pri_map;
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

            /* Offset within table */
            offset = (map->pkt_pri << 1) | map->pkt_cfi;

            /* Modify what's needed */
            entry = &ing_pri_map[offset];
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, entry,
                                PRIf, map->int_pri);
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, entry,
                                 CNGf, _BCM_COLOR_ENCODING(unit, map->color));

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
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!((flags & BCM_QOS_MAP_L2) || (flags & BCM_QOS_MAP_MPLS))
                 || !(flags & BCM_QOS_MAP_EGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
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
                                             "TR egr mpls pri map");
            if (NULL == egr_mpls_pri_map) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_pri_map, 0, alloc_size);

            alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_MPLS * 
                          sizeof(egr_mpls_exp_mapping_1_entry_t));
            egr_mpls_exp_map = soc_cm_salloc(unit, alloc_size,
                                             "TR egr mpls exp map");
            if (NULL == egr_mpls_exp_map) {
                sal_free(egr_mpls_pri_map);
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_exp_map, 0, alloc_size);

            alloc_size = (_BCM_QOS_MAP_CHUNK_EGR_MPLS * 
                          sizeof(egr_mpls_exp_mapping_2_entry_t));
            egr_mpls_exp_map2 = soc_cm_salloc(unit, alloc_size,
                                             "TR egr mpls exp map");
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
                soc_cm_sfree(unit, egr_mpls_pri_map);
                soc_cm_sfree(unit, egr_mpls_exp_map);
                QOS_UNLOCK(unit);
                return (rv);
            }
            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_1m)) {
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_1m, MEM_BLOCK_ANY, 
                                        index, 
                                        (index + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                        egr_mpls_exp_map);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, egr_mpls_pri_map);
                    soc_cm_sfree(unit, egr_mpls_exp_map);
                    QOS_UNLOCK(unit);
                    return (rv);
                }
            }

            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_2m, MEM_BLOCK_ANY, 
                                        index, 
                                        (index + (_BCM_QOS_MAP_CHUNK_EGR_MPLS -1)),
                                        egr_mpls_exp_map2);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, egr_mpls_pri_map);
                    soc_cm_sfree(unit, egr_mpls_exp_map);
                    soc_cm_sfree(unit, egr_mpls_exp_map2);
                    QOS_UNLOCK(unit);
                    return (rv);
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
                if (flags & BCM_QOS_MAP_MPLS) {
                    soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m, entry, 
                                        MPLS_EXPf, map->exp);
                }
            }

            if (SOC_MEM_IS_VALID(unit, EGR_MPLS_EXP_MAPPING_2m)) {
                entry = &egr_mpls_exp_map2[offset];
                if (flags & BCM_QOS_MAP_MPLS) {
                    soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_2m, entry, 
                                        MPLS_EXPf, map->exp);
                }
            }

            /* Delete the old profile chunk */
            rv = _bcm_egr_mpls_combo_map_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, egr_mpls_pri_map);
                soc_cm_sfree(unit, egr_mpls_exp_map);
                soc_cm_sfree(unit, egr_mpls_exp_map2);
                QOS_UNLOCK(unit);
                return (rv);
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
            alloc_size = _BCM_QOS_MAP_CHUNK_DSCP * sizeof(dscp_table_entry_t);
            dscp_table = soc_cm_salloc(unit, alloc_size,
                                       "TR dscp table");
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
            rv = soc_mem_read_range(unit, DSCP_TABLEm, MEM_BLOCK_ANY, 
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
            soc_DSCP_TABLEm_field32_set(unit, entry, DSCPf, map->dscp);
            soc_DSCP_TABLEm_field32_set(unit, entry, PRIf, map->int_pri);
            soc_DSCP_TABLEm_field32_set(unit, entry, CNGf, cng);

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
bcm_tr_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t clr_map;

    QOS_INIT(unit);

    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.int_pri = 0;
        clr_map.color = bcmColorGreen;
        rv = bcm_tr_qos_map_add(unit, flags, &clr_map, map_id);
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.pkt_pri = 0;
        clr_map.pkt_cfi = 0;
        rv = bcm_tr_qos_map_add(unit, flags, &clr_map, map_id);
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.int_pri = 0;
        clr_map.color = bcmColorGreen;
        rv = bcm_tr_qos_map_add(unit, flags, &clr_map, map_id);
        break;
    default:
        sal_memcpy(&clr_map, map, sizeof(clr_map));
        clr_map.int_pri = 0;
        clr_map.color = bcmColorGreen;
        rv = bcm_tr_qos_map_add(unit, flags, &clr_map, map_id);
        break;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    return rv;
}

/* Attach a QoS map to an object (Gport) */
int 
bcm_tr_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int index, id, alloc_size, rv = BCM_E_NONE;
    uint8 pri=0, cfi=0;
    bcm_color_t  color;
    char *buf, *buf2;
    void *entries[1];
#ifdef INCLUDE_L3
    mpls_entry_entry_t *mpls_entry=NULL;
    int vp;
    int index_max, index_min;
#endif
    bcm_color_t  used_colors[3] = {bcmColorGreen,bcmColorYellow,bcmColorRed};
    int idx;

    QOS_INIT(unit);

    QOS_LOCK(unit);
#ifdef INCLUDE_L3
    if (BCM_GPORT_IS_SET(port) && !BCM_GPORT_IS_MODPORT(port) && 
        !BCM_GPORT_IS_LOCAL(port)) {
        /* Deal with MPLS gports */
        if (BCM_GPORT_IS_MPLS_PORT(port)) {
            /* Deal with MiM and MPLS ports */
            vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                QOS_UNLOCK(unit);
                return BCM_E_BADID;
            } 
            if (ing_map != -1) { /* -1 means no change */
                /* TRUST_DOT1P_PTR from MPLS_ENTRY table */
                index_max = soc_mem_index_max(unit, MPLS_ENTRYm);
                index_min = soc_mem_index_min(unit, MPLS_ENTRYm);
                alloc_size = (index_max - index_min + 1) * 
                             sizeof(mpls_entry_entry_t);
                buf = soc_cm_salloc(unit, alloc_size, "TR MPLS_ENTRY");
                if (NULL == buf) {
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                rv = soc_mem_read_range(unit, MPLS_ENTRYm, MEM_BLOCK_ANY,  
                                        index_min, index_max, (void *)buf);
                if (rv < 0) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                for (index = index_min; index <= index_max; index++) {
                    mpls_entry = soc_mem_table_idx_to_pointer(unit, MPLS_ENTRYm, 
                                                           mpls_entry_entry_t *, 
                                                           buf, index);
                    if ((vp == soc_mem_field32_get(unit, MPLS_ENTRYm, 
                                                   mpls_entry, SOURCE_VPf)) &&
                        (soc_mem_field32_get(unit, MPLS_ENTRYm, 
                          mpls_entry, MPLS_ACTION_IF_BOSf) == 1) /* L2_SVP */) {
                        break; 
                    }
                }
                if (index == index_max + 1) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return BCM_E_NOT_FOUND; /* VP not found */
                }
                if (ing_map == 0) {
                    /* Clear the existing map */
                    soc_MPLS_ENTRYm_field32_set(unit, mpls_entry, 
                                                TRUST_OUTER_DOT1P_PTRf, 0);
                } else {
                    /* Use the provided map */
                    if ((ing_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                        soc_cm_sfree(unit, buf);
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    soc_MPLS_ENTRYm_field32_set(unit, mpls_entry, 
                                           TRUST_OUTER_DOT1P_PTRf, 
                                           QOS_INFO(unit)->pri_cng_hw_idx[id]);
                }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        soc_MPLS_ENTRYm_field32_set(unit, mpls_entry, KEY_TYPEf, 0x10);
    }
#endif
                rv = WRITE_MPLS_ENTRYm(unit, MEM_BLOCK_ALL, index, mpls_entry);
                if (rv < 0) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                soc_cm_sfree(unit, buf);
            }
            if (egr_map != -1) { /* -1 means no change */
                /* Can only do for physical port, not VP */
                QOS_UNLOCK(unit);
                return BCM_E_UNAVAIL;
            }
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
            if (ing_map == 0) {
                /* Clear the existing map - make identity */
                for (cfi = 0; cfi <= 1; cfi++) {
                    for (pri = 0; pri <= 7; pri++){
                        color = _BCM_COLOR_DECODING(unit, cfi);
                        rv = bcm_esw_port_vlan_priority_map_set
                                (unit, port, pri, cfi, pri, color);
                        if (BCM_FAILURE(rv)) {
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    }
                }    
            } else {
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
                /* Copy the corresponding chunk from ING_PRI_CNG_MAP profile 
                 * to the specific port's region */
                alloc_size = 16 * sizeof(ing_pri_cng_map_entry_t);
                buf = soc_cm_salloc(unit, alloc_size, "TR ing pri cng map");
                if (NULL == buf) {
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                sal_memset(buf, 0, alloc_size);
                index = (QOS_INFO(unit)->pri_cng_hw_idx[id] * 
                         _BCM_QOS_MAP_CHUNK_PRI_CNG);
                rv = soc_mem_read_range(unit, ING_PRI_CNG_MAPm,  
                                        MEM_BLOCK_ANY, index, index + 15, 
                                        (void *)buf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                index = port << 4;
                rv = soc_mem_write_range(unit, ING_PRI_CNG_MAPm, MEM_BLOCK_ALL, 
                                         index, index + 15, (void *)buf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                soc_cm_sfree(unit, buf);
            }
        }
        if (egr_map != -1) { /* -1 means no change */
            /* Copy the corresponding chunk from EGR_MPLS profiles to the 
             * EGR_PRI_CNG_MAP table, which is directly indexed by port */
            alloc_size = 64 * sizeof(egr_pri_cng_map_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR egr pri cng map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(buf, 0, alloc_size);
            index = port << 6;
            if (egr_map == 0) {
                /* Clear the existing map - make identity */
                for (idx = 0; idx < sizeof(used_colors)/sizeof(used_colors[0]);
                     idx++) {
                     color = used_colors[idx];
                    for (pri = 0; pri <= 7; pri++) {
                        cfi = (color == bcmColorRed) ? 1 : 0;
                        rv = bcm_esw_port_vlan_priority_unmap_set(unit, port, 
                                                        pri, color, pri, cfi);
                        if (BCM_FAILURE(rv)) {
                            soc_cm_sfree(unit, buf);
                            QOS_UNLOCK(unit);
                            return rv;
                        }
                    }
                }
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
                buf2 = soc_cm_salloc(unit, alloc_size, "TR egr mpls exp map");
                if (NULL == buf2) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                sal_memset(buf2, 0, alloc_size);
                index = (QOS_INFO(unit)->egr_mpls_hw_idx[id] * 
                         _BCM_QOS_MAP_CHUNK_EGR_MPLS);
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_1m,  
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

/* Retrieve a QOS map attached to an Object (Gport) */
int
bcm_tr_qos_port_map_get(int unit, bcm_gport_t port, 
                         int *ing_map, int *egr_map)
{
    return BCM_E_UNAVAIL;
}

/* bcm_tr_qos_map_multi_get */
int
bcm_tr_qos_map_multi_get(int unit, uint32 flags,
                          int map_id, int array_size, 
                          bcm_qos_map_t *array, int *array_count)
{
    int             rv = BCM_E_NONE;
    int             num_entries, idx, id, hw_id, alloc_size, entry_size, count;
    uint8           *dma_buf;
    void            *entry;
    soc_mem_t       mem;
    bcm_qos_map_t   *map;

    /* ignore with_id & replace flags */
    flags &= (~(BCM_QOS_MAP_WITH_ID | BCM_QOS_MAP_REPLACE));
    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    mem = INVALIDm;
    hw_id = 0;
    num_entries = 0;
    entry_size = 0;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        if (id >= _BCM_QOS_MAP_LEN_ING_PRI_CNG_MAP) {
            rv = BCM_E_PARAM;
            QOS_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(rv);
         }
        if ((flags != (BCM_QOS_MAP_L2 | BCM_QOS_MAP_INGRESS)) ||
            (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_PRI_CNG;
        entry_size = sizeof(ing_pri_cng_map_entry_t);
        hw_id = QOS_INFO(unit)->pri_cng_hw_idx[id]* _BCM_QOS_MAP_CHUNK_PRI_CNG;
        mem = ING_PRI_CNG_MAPm;
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        if (((flags != (BCM_QOS_MAP_L2 | BCM_QOS_MAP_EGRESS)) &&
             (flags != (BCM_QOS_MAP_MPLS | BCM_QOS_MAP_EGRESS))) ||
            (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_EGR_MPLS;
        entry_size = sizeof(egr_mpls_pri_mapping_entry_t);
        hw_id = (QOS_INFO(unit)->egr_mpls_hw_idx[id] * 
                _BCM_QOS_MAP_CHUNK_EGR_MPLS);
        mem = EGR_MPLS_PRI_MAPPINGm;
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if ((flags != (BCM_QOS_MAP_L3 | BCM_QOS_MAP_INGRESS)) ||
            (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id))) {
            rv = BCM_E_PARAM;
        }
        num_entries = _BCM_QOS_MAP_CHUNK_DSCP;
        entry_size = sizeof(dscp_table_entry_t);
        hw_id = QOS_INFO(unit)->dscp_hw_idx[id] * _BCM_QOS_MAP_CHUNK_DSCP;
        mem = DSCP_TABLEm;
        break;
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
            entry_size = sizeof(ing_mpls_exp_mapping_entry_t);
            hw_id = id; /* soc profile mem is not used. So direct hw index */
            mem = ING_MPLS_EXP_MAPPINGm;
        } else 
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
        {
            rv = BCM_E_PARAM;
        }
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
    dma_buf = soc_cm_salloc(unit, alloc_size, "TR qos multi get DMA buf");
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
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->pkt_pri = ((idx & 0x1e) >> 1);
            map->pkt_cfi = (idx & 0x1);
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                                 dma_buf, idx);
            map->int_pri = soc_mem_field32_get(unit, mem, entry, PRIf);
            map->color = _BCM_COLOR_DECODING(unit, soc_mem_field32_get(unit, 
                                 mem, entry, CNGf));
            count++;
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->int_pri = ((idx & 0x3c) >> 2);
            map->color = _BCM_COLOR_DECODING(unit, (idx & 0x3));
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                                 dma_buf, idx);
            map->pkt_pri = soc_mem_field32_get(unit, mem, entry, NEW_PRIf);
            map->pkt_cfi = soc_mem_field32_get(unit, mem, entry, NEW_CFIf);
            count++;
        }
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        for (idx=0; ((idx<num_entries) && (count < array_size)); idx++) {
            map = &array[idx];
            sal_memset(map, 0x0, sizeof(bcm_qos_map_t));
            map->dscp = idx;
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *, 
                                                 dma_buf, idx);
            map->int_pri = soc_mem_field32_get(unit, mem, entry, PRIf);
            map->color = _BCM_COLOR_DECODING(unit, soc_mem_field32_get(unit, 
                                 mem, entry, CNGf));
            count++;
        }
        break;
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
                map->int_pri = soc_mem_field32_get(unit, mem, entry, PRIf);
                count++;
            }
        } else 
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
        {
            rv = BCM_E_INTERNAL; /* should not hit this */
        }
    }
    QOS_UNLOCK(unit);
    soc_cm_sfree(unit, dma_buf);

    BCM_IF_ERROR_RETURN(rv);

    *array_count = count;
    return BCM_E_NONE;
}

/* Get the list of all created Map-ids along with corresponding flags */
int
bcm_tr_qos_multi_get(int unit, int array_size, int *map_ids_array, 
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
            QOS_LOCK(unit);
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
            *array_count += count;
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            count = 0;
            SHR_BITCOUNT_RANGE(QOS_INFO(unit)->ing_mpls_exp_bitmap, count,
                               0, _BCM_QOS_MAP_LEN_ING_MPLS_EXP_MAP);
            *array_count += count;
#endif
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
                if (_BCM_QOS_DSCP_TABLE_USED_GET(unit, idx)) {
                    *(map_ids_array + count) = idx | 
                     (_BCM_QOS_MAP_TYPE_DSCP_TABLE << _BCM_QOS_MAP_SHIFT);
                    *(flags_array + count) = (BCM_QOS_MAP_L3 | 
                                              BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
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
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */
            *array_count = count;
        }
    }
    QOS_UNLOCK(unit);
    return rv;
}

/* Function:
*	   _bcm_tr_qos_id2idx
* Purpose:
*	   Translate map ID into hardware table index used by API
* Parameters:
* Returns:
*	   BCM_E_XXX
*/	   
int
_bcm_tr_qos_id2idx(int unit, int map_id, int *hw_idx)
{
    int id;

    QOS_INIT(unit);

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;

    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
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
    default:
         QOS_UNLOCK(unit);
         return BCM_E_NOT_FOUND;
    }
    QOS_UNLOCK(unit);
    return BCM_E_NONE;
}

/* Function:
*	   _bcm_tr_qos_id2idx
* Purpose:
*	   Translate hardware table index into map ID used by API 
* Parameters:
* Returns:
*	   BCM_E_XXX
*/	   
int
_bcm_tr_qos_idx2id(int unit, int hw_idx, int type, int *map_id)
{
    int id, num_map;

    QOS_INIT(unit);

    switch (type) {
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
         num_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
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
         num_map = soc_mem_index_count(unit, DSCP_TABLEm) / 64;
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
    default:
         return BCM_E_NOT_FOUND;
    }
    return BCM_E_NOT_FOUND;
}
#endif
