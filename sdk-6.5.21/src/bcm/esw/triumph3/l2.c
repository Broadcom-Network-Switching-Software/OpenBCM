/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Triumph3 L2 function implementations
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_TRIUMPH3_SUPPORT)

#include <assert.h>
 
#include <sal/core/libc.h>
 
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/l2u.h>
#include <soc/triumph3.h>
#include <soc/ism_hash.h>
#ifdef BCM_KATANA2_SUPPORT 
#include <soc/katana2.h> 
#endif 
#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm/failover.h>
 
#include <bcm_int/esw/l2gre.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/trill.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/triumph.h>

/****************************************************************
 *
 * L2 software tables, per unit.
 */
static int _bcm_tr3_l2_init[BCM_MAX_NUM_UNITS];
static soc_profile_mem_t *_bcm_l2_tr3_port_cbl_profile[BCM_MAX_NUM_UNITS];

/*
 * Define:
 * BCM_TR3_L2_INIT
 * Purpose:
 *  Causes a routine to return BCM_E_INIT  if L2 software module
 *  is not yet initialized.
 */

#define BCM_TR3_L2_INIT(unit) \
    if (0 == _bcm_tr3_l2_init[unit]) return BCM_E_INIT

/***********************************************************************
 *
 * L2X Message Registration
 */

#define _BCM_TR3_L2_CB_MAX 3

typedef struct _bcm_tr3_l2x_cb_entry_s {
    bcm_l2_addr_callback_t fn;
    void                   *fn_data;
} _bcm_tr3_l2_cb_entry_t;

#define _BCM_TR3_L2X_THREAD_STOP (1 << 0)
typedef struct _bcm_tr3_l2_data_s {
    _bcm_tr3_l2_cb_entry_t cb[_BCM_TR3_L2_CB_MAX];
    int                    cb_count;
    int                    flags;
    sal_mutex_t            l2_mutex;
} _bcm_tr3_l2_data_t;

static _bcm_tr3_l2_data_t *_bcm_tr3_l2_data[SOC_MAX_NUM_DEVICES];


#define BCM_TR3_L2_CB_LOCK(unit) \
    sal_mutex_take(_bcm_tr3_l2_data[unit]->l2_mutex, sal_mutex_FOREVER)

#define BCM_TR3_L2_CB_UNLOCK(unit) \
    sal_mutex_give(_bcm_tr3_l2_data[unit]->l2_mutex)

#define BCM_TR3_L2_CB_MUTEX(unit) \
    if (_bcm_tr3_l2_data[unit] == NULL || \
        (_bcm_tr3_l2_data[unit]->l2_mutex == NULL && \
        (_bcm_tr3_l2_data[unit]->l2_mutex = \
            sal_mutex_create("bcm_tr3_l2_lock")) == NULL))  \
            return BCM_E_MEMORY

/****************************************************************
 *
 * L2 MAC block software accounting, per unit.
 */

typedef struct _bcm_tr3_mac_block_info_s {
    bcm_pbmp_t mb_pbmp;
    int ref_count;
} _bcm_tr3_mac_block_info_t;

static _bcm_tr3_mac_block_info_t *_tr3_mbi_entries[BCM_MAX_NUM_UNITS];
static int _tr3_mbi_num[BCM_MAX_NUM_UNITS];

/****************************************************************
 *
 * L2 learn limit software representation, per unit.
 */

#define BCM_MAC_LIMIT_MAX(_u_) (soc_feature(_u_, soc_feature_esm_support))       ?  \
                                ((soc_mem_index_count(_u_, L2_ENTRY_1m))     +  \
                                 (soc_mem_index_count(_u_, EXT_L2_ENTRY_1m)) +  \
                                 (soc_mem_index_count(_u_, EXT_L2_ENTRY_2m))):  \
                                 (soc_mem_index_count(_u_, L2_ENTRY_1m))

/****************************************************************
 *
 * L2 entry types in one structure.
 */

#define _BCM_TR3_L2_SELECT_REDUCED          \
         (_BCM_TR3_L2_SELECT_L2_ENTRY_1 | _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1)
#define _BCM_TR3_L2_SELECT_EXTENDED         \
         (_BCM_TR3_L2_SELECT_L2_ENTRY_2 | _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2)
#define _BCM_TR3_L2_SELECT_INTERNAL         \
         (_BCM_TR3_L2_SELECT_L2_ENTRY_1 | _BCM_TR3_L2_SELECT_L2_ENTRY_2)
#define _BCM_TR3_L2_SELECT_EXTERNAL         \
         (_BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1 | _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2)
#define _BCM_TR3_L2_SELECT_ALL_TABLES       \
         (_BCM_TR3_L2_SELECT_REDUCED | _BCM_TR3_L2_SELECT_EXTENDED)

static uint8 _bcm_tr3_l2_mems_valid[SOC_MAX_NUM_DEVICES];

#define _BCM_TR3_L2_SELECT_ALL(unit) _bcm_tr3_l2_mems_valid[unit]

#define _BCM_TR3_L2_MEM_BANKS_ALL       (-1)

#define _BCM_TR3_L2_MEM_TYPE_VALID(_entries_, _mem_type_) \
        (0 != (_entries_->entry_flags & _BCM_TR3_L2_SELECT_##_mem_type_))

/****************************************************************
 *
 * L2 entry field abstractions
 */

typedef enum _bcm_tr3_l2_mem_type_e {
    _BCM_TR3_L2_MEM_L2_ENTRY_1,
    _BCM_TR3_L2_MEM_L2_ENTRY_2,
    _BCM_TR3_L2_MEM_EXT_L2_ENTRY_1,
    _BCM_TR3_L2_MEM_EXT_L2_ENTRY_2,
    _BCM_TR3_L2_MEM_NUM
} _bcm_tr3_l2_mem_type_t;

static soc_mem_t _bcm_tr3_l2_mems[_BCM_TR3_L2_MEM_NUM] = {
    L2_ENTRY_1m,
    L2_ENTRY_2m,
    EXT_L2_ENTRY_1m,
    EXT_L2_ENTRY_2m
};

typedef enum _bcm_tr3_l2_memacc_type_e {
    _BCM_TR3_L2_MEMACC_VALID,
    _BCM_TR3_L2_MEMACC_VALID_1,
    _BCM_TR3_L2_MEMACC_FREE,
    _BCM_TR3_L2_MEMACC_KEY_TYPE,
    _BCM_TR3_L2_MEMACC_KEY_TYPE_1,
    _BCM_TR3_L2_MEMACC_VLAN_ID,
    _BCM_TR3_L2_MEMACC_VFI,
    _BCM_TR3_L2_MEMACC_MAC_ADDR,
    _BCM_TR3_L2_MEMACC_PORT_NUM,
    _BCM_TR3_L2_MEMACC_TGID,
    _BCM_TR3_L2_MEMACC_L2MC_PTR,
    _BCM_TR3_L2_MEMACC_MODULE_ID,
    _BCM_TR3_L2_MEMACC_DESTINATION,
    _BCM_TR3_L2_MEMACC_DEST_TYPE,
    _BCM_TR3_L2_MEMACC_PENDING,
    _BCM_TR3_L2_MEMACC_LIMIT_COUNTED,
    _BCM_TR3_L2_MEMACC_REMOTE,
    _BCM_TR3_L2_MEMACC_RPE,
    _BCM_TR3_L2_MEMACC_PRI,
    _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX,
    _BCM_TR3_L2_MEMACC_CLASS_ID,
    _BCM_TR3_L2_MEMACC_CLASS_ID_FULL,
    _BCM_TR3_L2_MEMACC_SCP,
    _BCM_TR3_L2_MEMACC_SRC_DISCARD,
    _BCM_TR3_L2_MEMACC_DST_DISCARD,
    _BCM_TR3_L2_MEMACC_DST_CPU,
    _BCM_TR3_L2_MEMACC_WIDE,
    _BCM_TR3_L2_MEMACC_WIDE_1,
    _BCM_TR3_L2_MEMACC_STATIC_BIT,
    _BCM_TR3_L2_MEMACC_STATIC_BIT_1,
    _BCM_TR3_L2_MEMACC_HITSA,
    _BCM_TR3_L2_MEMACC_HITDA,
    _BCM_TR3_L2_MEMACC_LOCAL_SA,
    _BCM_TR3_L2_MEMACC_Q_TAG,
    _BCM_TR3_L2_MEMACC_EH_TAG_TYPE,
    _BCM_TR3_L2_MEMACC_EH_TM,
    _BCM_TR3_L2_MEMACC_EH_QUEUE_TAG,
    _BCM_TR3_L2_MEMACC_NUM
} _bcm_tr3_l2_memacc_type_t;

static soc_field_t _bcm_tr3_l2_entry_1_field_memacc_map[_BCM_TR3_L2_MEMACC_NUM] = {
    VALIDf,
    INVALIDf,
    INVALIDf,
    KEY_TYPEf,
    INVALIDf,
    L2__VLAN_IDf,
    L2__VFIf,
    L2__MAC_ADDRf,
    L2__PORT_NUMf,
    L2__TGIDf,
    L2__L2MC_PTRf,
    L2__MODULE_IDf,
    L2__DESTINATIONf,
    L2__DEST_TYPEf,
    L2__PENDINGf,
    L2__LIMIT_COUNTEDf,
    L2__REMOTEf,
    L2__RPEf,
    L2__PRIf,
    L2__MAC_BLOCK_INDEXf,
    L2__CLASS_IDf,
    L2__CLASS_ID_FULLf,
    L2__SCPf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
    WIDEf,
    INVALIDf,
    STATIC_BITf,
    INVALIDf,
    HITSAf,
    HITDAf,
    LOCAL_SAf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
};

static soc_field_t _bcm_tr3_l2_entry_2_field_memacc_map[_BCM_TR3_L2_MEMACC_NUM] = {
    VALID_0f,
    VALID_1f,
    INVALIDf,
    KEY_TYPE_0f,
    KEY_TYPE_1f,
    L2__VLAN_IDf,
    L2__VFIf,
    L2__MAC_ADDRf,
    L2__PORT_NUMf,
    L2__TGIDf,
    L2__L2MC_PTRf,
    L2__MODULE_IDf,
    L2__DESTINATIONf,
    L2__DEST_TYPEf,
    L2__PENDINGf,
    L2__LIMIT_COUNTEDf,
    L2__REMOTEf,
    L2__RPEf,
    L2__PRIf,
    L2__MAC_BLOCK_INDEXf,
    L2__CLASS_IDf,
    L2__CLASS_ID_FULLf,
    L2__SCPf,
    L2__SRC_DISCARDf,
    L2__DST_DISCARDf,
    L2__DST_CPUf,
    WIDE_0f,
    WIDE_1f,
    STATIC_BIT_0f,
    STATIC_BIT_1f,
    HITSAf,
    HITDAf,
    LOCAL_SAf,
    L2__SIRIUS_Q_TAGf,
    L2__EH_TAG_TYPEf,
    L2__EH_TMf,
    L2__EH_QUEUE_TAGf,
};

static soc_field_t _bcm_tr3_ext_l2_entry_1_field_memacc_map[_BCM_TR3_L2_MEMACC_NUM] = {
    INVALIDf,
    INVALIDf,
    FREEf,
    KEY_TYPEf,
    INVALIDf,
    VLAN_IDf,
    VFIf,
    MAC_ADDRf,
    PORT_NUMf,
    TGIDf,
    INVALIDf,
    MODULE_IDf,
    DESTINATIONf,
    DEST_TYPEf,
    PENDINGf,
    LIMIT_COUNTEDf,
    REMOTEf,
    RPEf,
    PRIf,
    MAC_BLOCK_INDEXf,
    CLASS_IDf,
    CLASS_ID_FULLf,
    SCPf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
    STATIC_BITf,
    INVALIDf,
    HITSAf,
    HITDAf,
    LOCAL_SAf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
    INVALIDf,
};

static soc_field_t _bcm_tr3_ext_l2_entry_2_field_memacc_map[_BCM_TR3_L2_MEMACC_NUM] = {
    INVALIDf,
    INVALIDf,
    FREEf,
    KEY_TYPEf,
    INVALIDf,
    VLAN_IDf,
    VFIf,
    MAC_ADDRf,
    PORT_NUMf,
    TGIDf,
    INVALIDf,
    MODULE_IDf,
    DESTINATIONf,
    DEST_TYPEf,
    PENDINGf,
    LIMIT_COUNTEDf,
    REMOTEf,
    RPEf,
    PRIf,
    MAC_BLOCK_INDEXf,
    CLASS_IDf,
    CLASS_ID_FULLf,
    SCPf,
    SRC_DISCARDf,
    DST_DISCARDf,
    CPUf,
    INVALIDf,
    INVALIDf,
    STATIC_BITf,
    INVALIDf,
    HITSAf,
    HITDAf,
    LOCAL_SAf,
    SIRIUS_Q_TAGf,
    EH_TAG_TYPEf,
    EH_TMf,
    EH_QUEUE_TAGf,
};

static soc_field_t *_bcm_tr3_l2_field_memacc_maps[_BCM_TR3_L2_MEM_NUM] = {
    _bcm_tr3_l2_entry_1_field_memacc_map,
    _bcm_tr3_l2_entry_2_field_memacc_map,
    _bcm_tr3_ext_l2_entry_1_field_memacc_map,
    _bcm_tr3_ext_l2_entry_2_field_memacc_map,
};

static soc_memacc_t *_bcm_tr3_l2_memacc_list[BCM_MAX_NUM_UNITS][_BCM_TR3_L2_MEM_NUM];

#define _BCM_TR3_L2_MEMACC(_unit_, _mem_type_, _memacc_type_) \
        &(_bcm_tr3_l2_memacc_list[_unit_][_BCM_TR3_L2_MEM_##_mem_type_] \
                [_memacc_type_])

#define _BCM_TR3_L2_FIELD_MEMACC(_unit_, _mem_type_, _field_type_) \
        &(_bcm_tr3_l2_memacc_list[_unit_][_BCM_TR3_L2_MEM_##_mem_type_] \
                [_BCM_TR3_L2_MEMACC_##_field_type_])

#define _BCM_TR3_L2_1_FIELD_MEMACC(_unit_, _field_type_) \
        &(_bcm_tr3_l2_memacc_list[_unit_][_BCM_TR3_L2_MEM_L2_ENTRY_1] \
                [_BCM_TR3_L2_MEMACC_##_field_type_])

#define _BCM_TR3_L2_1_FIELD32_MEMACC_GET(_unit_, _entry_, _field_type_) \
        soc_memacc_field32_get(_BCM_TR3_L2_1_FIELD_MEMACC(_unit_, \
                                                   _field_type_), _entry_)

#define _BCM_TR3_L2_2_FIELD_MEMACC(_unit_, _field_type_) \
        &(_bcm_tr3_l2_memacc_list[_unit_][_BCM_TR3_L2_MEM_L2_ENTRY_2] \
                [_BCM_TR3_L2_MEMACC_##_field_type_])

#define _BCM_TR3_L2_2_FIELD32_MEMACC_GET(_unit_, _entry_, _field_type_) \
        soc_memacc_field32_get(_BCM_TR3_L2_2_FIELD_MEMACC(_unit_, \
                                                   _field_type_), _entry_)

#define _BCM_TR3_EXT_L2_1_FIELD_MEMACC(_unit_, _field_type_) \
        &(_bcm_tr3_l2_memacc_list[_unit_][_BCM_TR3_L2_MEM_EXT_L2_ENTRY_1] \
                [_BCM_TR3_L2_MEMACC_##_field_type_])

#define _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(_unit_, _entry_, _field_type_) \
        soc_memacc_field32_get(_BCM_TR3_EXT_L2_1_FIELD_MEMACC(_unit_, \
                                                   _field_type_), _entry_)

#define _BCM_TR3_EXT_L2_2_FIELD_MEMACC(_unit_, _field_type_) \
        &(_bcm_tr3_l2_memacc_list[_unit_][_BCM_TR3_L2_MEM_EXT_L2_ENTRY_2] \
                [_BCM_TR3_L2_MEMACC_##_field_type_])

#define _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(_unit_, _entry_, _field_type_) \
        soc_memacc_field32_get(_BCM_TR3_EXT_L2_2_FIELD_MEMACC(_unit_, \
                                                   _field_type_), _entry_)

/****************************************************************
 *
 * L2 data definitions
 */
#define _BCM_TR3_L2_CLASS_ID_REDUCED_MASK 0x3f

typedef struct _bcm_tr3_l2_bookkeeping_s {
    uint16 *my_station_hash;      /* Hash values for MY STATION entries */
    my_station_tcam_entry_t *my_station_shadow;
    my_station_tcam_entry_t my_station_l3_mask;
    my_station_tcam_entry_t my_station_tunnel_mask;
} _bcm_tr3_l2_bookkeeping_t;

static _bcm_tr3_l2_bookkeeping_t  _bcm_tr3_l2_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

#define L2_INFO(_unit_)   (&_bcm_tr3_l2_bk_info[_unit_])
#define MY_STATION_HASH(_unit_, _index_) \
        (_bcm_tr3_l2_bk_info[_unit_].my_station_hash[_index_])

/****************************************************************
 *
 * Misc definitions
 */
#define _BCM_ALL_L2X_MEM_LOCK(unit) \
        SOC_L2X_MEM_LOCK(unit); \
        if (soc_feature(unit, soc_feature_esm_support)) { \
            SOC_EXT_L2X_MEM_LOCK(unit); \
        }

#define _BCM_ALL_L2X_MEM_UNLOCK(unit) \
        if (soc_feature(unit, soc_feature_esm_support)) { \
            SOC_EXT_L2X_MEM_UNLOCK(unit); \
        } \
        SOC_L2X_MEM_UNLOCK(unit);

#define _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv) \
        if (!BCM_SUCCESS(rv)) { \
            if (soc_feature(unit, soc_feature_esm_support)) { \
                SOC_EXT_L2X_MEM_UNLOCK(unit); \
            } \
            SOC_L2X_MEM_UNLOCK(unit); \
            return rv; \
        }

/****************************************************************
 *
 * Debug L2 counts
 */
/* Definition for extern variable '_l2_addr_counts' */
l2_count_data_t _l2_addr_counts;

STATIC int 
_bcm_tr3_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr, 
                    bcm_module_t new_module, bcm_port_t new_port, 
                    bcm_trunk_t new_trunk);

/*
 * Function:
 *      _bcm_tr3_l2_memacc_init
 * Description:
 *      Initialize the memory accelerators for the L2 tables enabled
 *      on this device variation.
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

STATIC int
_bcm_tr3_l2_memacc_init(int unit)
{
    int alloc_size, rv = BCM_E_NONE;
    soc_field_t *memacc_map;
    soc_memacc_t *memacc_list;
    int mem_idx, mam_ix, memacc_map_size = _BCM_TR3_L2_MEMACC_NUM;

    alloc_size = (memacc_map_size * sizeof(soc_memacc_t));

    for (mem_idx = _BCM_TR3_L2_MEM_L2_ENTRY_1;
         mem_idx < _BCM_TR3_L2_MEM_NUM;
         mem_idx++) {
        if (0 == (_BCM_TR3_L2_SELECT_ALL(unit) & (1 << mem_idx))) {
            /* Table not enabled in this configuration */
            continue;
        }

        _bcm_tr3_l2_memacc_list[unit][mem_idx] =
            sal_alloc(alloc_size, "L2 tables memacc data");
        if (NULL == _bcm_tr3_l2_memacc_list[unit][mem_idx]) {
            rv = BCM_E_MEMORY;
            break;
        }

        memacc_map = _bcm_tr3_l2_field_memacc_maps[mem_idx];
        memacc_list = _bcm_tr3_l2_memacc_list[unit][mem_idx];

        for (mam_ix = 0; mam_ix < memacc_map_size; mam_ix++) {
            if (INVALIDf == memacc_map[mam_ix]) {
                SOC_MEMACC_INVALID_SET(&(memacc_list[mam_ix]));
                continue;
            }
            rv = soc_memacc_init(unit, _bcm_tr3_l2_mems[mem_idx],
                                 memacc_map[mam_ix],
                                 &(memacc_list[mam_ix]));
            if (BCM_FAILURE(rv)) {
                break;
            }
        }

        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

typedef struct _bcm_tr3_l2_valid_memacc_s {
    soc_memacc_t *valid;
    soc_memacc_t *key_type;
    soc_memacc_t *free;
    soc_memacc_t *wide;
} _bcm_tr3_l2_valid_memacc_t;

/*
 * Function:
 *     _bcm_tr3_l2_entry_traverse_valid
 * Description:
 *      Check if given traversed L2 entry is Valid
 * Parameters:
 *      memacc_list L2 memory accessors pre-loaded
 *      l2_entry    L2 entry read from HW
 *      valid       (OUT) Entry valid indicator
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_tr3_l2_entry_traverse_valid(_bcm_tr3_l2_valid_memacc_t *memacc_list,
                                 uint32 *l2_entry, int *valid)
{
    uint32  fval = 0;
    int     key_type;

    if ((NULL == l2_entry) || (NULL == valid) || (NULL == memacc_list)) {
        return BCM_E_PARAM;
    }

    if (NULL != memacc_list->valid) {
        fval = soc_memacc_field32_get(memacc_list->valid, l2_entry);

        if (fval && memacc_list->key_type) {
            key_type =
                soc_memacc_field32_get(memacc_list->key_type, l2_entry);
            if ((SOC_MEM_KEY_L2_ENTRY_1_L2_VFI != key_type) &&
                (SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE != key_type) &&
                 (SOC_MEM_KEY_L2_ENTRY_2_L2_VFI != key_type) &&
                 (SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE != key_type)) {
                /* Skip non-bridge/VFI L2 entries */
                fval = 0;
            }
        }
    } else if (NULL != memacc_list->free) {
        fval = !soc_memacc_field32_get(memacc_list->free, l2_entry);
    }

    *valid = fval ? TRUE : FALSE;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_tr3_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_tr3_l2_traverse to itterate over given
 *      memory and actually read the table and parse entries.
 * Parameters:
 *     unit         device number
 *     mem          L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_tr3_l2_traverse_mem(int unit, soc_mem_t mem,
                         _bcm_l2_traverse_t *trav_st)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, valid, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *l2_tbl_chnk;
    uint32          *l2_entry;
    int             rv = BCM_E_NONE;
    uint32          l2_entry_2[SOC_MAX_MEM_WORDS];
    uint32          *l2_src[4] = {0};
    _bcm_tr3_l2_valid_memacc_t l2_trav_memaccs;
    
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 traverse");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    switch (mem) {
    case L2_ENTRY_1m:
        l2_trav_memaccs.valid = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                                   _BCM_TR3_L2_MEMACC_VALID);
        l2_trav_memaccs.key_type = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                         _BCM_TR3_L2_MEMACC_KEY_TYPE);
        l2_trav_memaccs.free = NULL;
        l2_trav_memaccs.wide = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                         _BCM_TR3_L2_MEMACC_WIDE);
        break;
    case EXT_L2_ENTRY_1m:
        l2_trav_memaccs.valid = NULL;
        l2_trav_memaccs.key_type = NULL;
        l2_trav_memaccs.free = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                                  _BCM_TR3_L2_MEMACC_FREE);
        l2_trav_memaccs.wide = NULL;
        break;
    case EXT_L2_ENTRY_2m:
        l2_trav_memaccs.valid = NULL;
        l2_trav_memaccs.key_type = NULL;
        l2_trav_memaccs.free = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                                  _BCM_TR3_L2_MEMACC_FREE);
        l2_trav_memaccs.wide = NULL;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        soc_mem_unlock(unit, mem);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = 
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             l2_tbl_chnk, ent_idx);
            rv = _bcm_tr3_l2_entry_traverse_valid(&l2_trav_memaccs,
                                                  l2_entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }

            if (FALSE == valid) {
                continue;
            }

            if ((NULL != l2_trav_memaccs.wide) &&
                (0 != soc_memacc_field32_get(l2_trav_memaccs.wide,
                                             l2_entry))) {
                /* The first element of an L2_ENTRY_2 entry detected */
                l2_src[0] = l2_entry;
                l2_src[1] = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             l2_tbl_chnk, ent_idx + 1);

                soc_mem_base_to_wide_entry_conv(unit, L2_ENTRY_2m,
                                                L2_ENTRY_1m,
                                                l2_entry_2, l2_src,
                                                TYPE_1_TO_TYPE_2);

                trav_st->data = l2_entry_2;
                trav_st->mem = L2_ENTRY_2m;
                trav_st->mem_idx = (chnk_idx + ent_idx) / 2;
                ent_idx++;
            } else {
                trav_st->data = l2_entry;
                trav_st->mem = mem;
                trav_st->mem_idx = chnk_idx + ent_idx;
            }
            rv = trav_st->int_cb(unit, (void *)trav_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_tbl_chnk);
    return rv;        
}

/*
 * Function:
 *     _bcm_tr3_l2_traverse
 * Description:
 *      Helper function to bcm_tr3_l2_traverse to itterate over tables 
 *      and actually read the momery
 * Parameters:
 *     unit         device number
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_tr3_l2_traverse(int unit, _bcm_l2_traverse_t *trav_st)
{
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l2_traverse_mem(unit, L2_ENTRY_1m, trav_st));

    if (soc_feature(unit, soc_feature_esm_support)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_traverse_mem(unit, EXT_L2_ENTRY_1m, trav_st));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_traverse_mem(unit, EXT_L2_ENTRY_2m, trav_st));
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_tr3_l2_reload_mbi_cb(int unit, void *trav_st)
{
    int                 mb_index;
    _bcm_l2_traverse_t *trv = (_bcm_l2_traverse_t *)trav_st;
    _bcm_tr3_mac_block_info_t *mbi = trv->user_data;

    trv = (_bcm_l2_traverse_t *)trav_st;

    switch(trv->mem) {
    case L2_ENTRY_1m:
        mb_index = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, trv->data,
                                                    MAC_BLOCK_INDEX);
        break;
    case L2_ENTRY_2m:
        mb_index = _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, trv->data,
                                                    MAC_BLOCK_INDEX);
        break;
    case EXT_L2_ENTRY_1m:
        mb_index = _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, trv->data,
                                                        MAC_BLOCK_INDEX);
        break;
    case EXT_L2_ENTRY_2m:
        mb_index = _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, trv->data,
                                                        MAC_BLOCK_INDEX);
        break;
    default:
        return BCM_E_INTERNAL;
    }

    mbi[mb_index].ref_count++;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_reload_mbi
 * Description:
 *      Load MAC block info from hardware into software data structures.
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

STATIC int
_bcm_tr3_l2_reload_mbi(int unit)
{
    _bcm_tr3_mac_block_info_t *mbi = _tr3_mbi_entries[unit];
    mac_block_entry_t   mbe;
    int                 mb_index;
    pbmp_t              mb_pbmp;
    _bcm_l2_traverse_t  trav_st;

    /*
     * Refresh MAC Block information from the hardware tables.
     */

    for (mb_index = 0; mb_index < _tr3_mbi_num[unit]; mb_index++) {
        SOC_IF_ERROR_RETURN
            (READ_MAC_BLOCKm(unit, MEM_BLOCK_ANY, mb_index, &mbe));

        SOC_PBMP_CLEAR(mb_pbmp);

        SOC_PBMP_WORD_SET(mb_pbmp, 0, soc_MAC_BLOCKm_field32_get(unit,
                             &mbe, MAC_BLOCK_MASK_W0f));
        SOC_PBMP_WORD_SET(mb_pbmp, 1, soc_MAC_BLOCKm_field32_get(unit,
                             &mbe, MAC_BLOCK_MASK_W1f));
        BCM_PBMP_ASSIGN(mbi[mb_index].mb_pbmp, mb_pbmp);
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));
    
        trav_st.user_cb = NULL;
        trav_st.user_data = mbi;
        trav_st.int_cb = _bcm_tr3_l2_reload_mbi_cb;
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_traverse(unit, &trav_st));
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_tr3_l2_mbi_init
 * Purpose:
 *      Initialize BCM L2 MAC block index data.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_tr3_l2_mbi_init(int unit)
{
    if (_tr3_mbi_entries[unit] != NULL) {
        sal_free(_tr3_mbi_entries[unit]);
        _tr3_mbi_entries[unit] = NULL;
    }

    _tr3_mbi_num[unit] = soc_mem_index_count(unit, MAC_BLOCKm);
    _tr3_mbi_entries[unit] = sal_alloc(_tr3_mbi_num[unit] *
                                   sizeof(_bcm_tr3_mac_block_info_t),
                                   "BCM L2X MAC blocking info");
    if (!_tr3_mbi_entries[unit]) {
        return BCM_E_MEMORY;
    }

    sal_memset(_tr3_mbi_entries[unit], 0,
               _tr3_mbi_num[unit] * sizeof(_bcm_tr3_mac_block_info_t));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_reload_mbi(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_mac_block_insert
 * Purpose:
 *      Find or create a MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_pbmp - egress port bitmap for source MAC blocking
 *      mb_index - (OUT) Index of MAC_BLOCK table with bitmap.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No more MAC_BLOCK entries available
 *      BCM_E_PARAM             Bad bitmap supplied
 */
STATIC int
_bcm_tr3_mac_block_insert(int unit, bcm_pbmp_t mb_pbmp, int *mb_index)
{
    int cur_index = 0;
    _bcm_tr3_mac_block_info_t *mbi = _tr3_mbi_entries[unit];
    mac_block_entry_t mbe;
    bcm_pbmp_t temp_pbmp;

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mb_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mb_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    for (cur_index = 0; cur_index < _tr3_mbi_num[unit]; cur_index++) {
        if (BCM_PBMP_EQ(mbi[cur_index].mb_pbmp, mb_pbmp)) {
            mbi[cur_index].ref_count++;
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Not in table already, see if any space free */
    for (cur_index = 1; cur_index < _tr3_mbi_num[unit]; cur_index++) {
        if (mbi[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(&mbe, 0, sizeof(mac_block_entry_t));

            soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                   &mb_pbmp); 
            SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCKm(unit, MEM_BLOCK_ALL,
                                                 cur_index, &mbe));
            mbi[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mbi[cur_index].mb_pbmp, mb_pbmp);
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_tr3_mac_block_delete
 * Purpose:
 *      Remove reference to MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_index - Index of MAC_BLOCK table with bitmap.
 */
STATIC void
_bcm_tr3_mac_block_delete(int unit, int mb_index)
{
    if (_tr3_mbi_entries[unit][mb_index].ref_count > 0) {
        _tr3_mbi_entries[unit][mb_index].ref_count--;
    } else if (mb_index) {
        
        /* Someone reran init without flushing the L2 table */
    } /* else mb_index = 0, as expected for learning */
}

/*
 * Function:
 *      _bcm_tr3_l2hw_entries_field32_set
 * Purpose:
 *      Set a field32 value for a Triumph3 L2 entry collection
 * Parameters:
 *      unit        Unit number
 *      l2_entries  (IN/OUT) Triumph3 L2 entries structure
 *      l2_field    Memacc type
 *      val         Field value to set
 */
void
_bcm_tr3_l2hw_entries_field32_set(int unit,
                                  _bcm_tr3_l2_entries_t *l2_entries,
                                  _bcm_tr3_l2_memacc_type_t l2_field,
                                  uint32 val)
{
    soc_memacc_t *memacc;

    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        /* L2_ENTRY_1 */
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_1), val);
        }
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        /* L2_ENTRY_2 */
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), val);
        }
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) {
        /* EXT_L2_ENTRY_1 */
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            soc_memacc_field32_set(memacc,
                                   &(l2_entries->ext_l2_entry_1), val);
        }
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) {
        /* EXT_L2_ENTRY_2 */
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            soc_memacc_field32_set(memacc,
                                   &(l2_entries->ext_l2_entry_2), val);
        }
    }
}

/*
 * Function:
 *      _bcm_tr3_l2hw_entries_field32_get
 * Purpose:
 *      Get a field32 value from a Triumph3 L2 entry collection
 * Parameters:
 *      unit        Unit number
 *      l2_entries  Triumph3 L2 entries structure
 *      l2_field    
 */
uint32
_bcm_tr3_l2hw_entries_field32_get(int unit,
                                  _bcm_tr3_l2_entries_t *l2_entries,
                                  _bcm_tr3_l2_memacc_type_t l2_field)
{
    soc_memacc_t *memacc;

    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        /* L2_ENTRY_1 */
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            return soc_memacc_field32_get(memacc, &(l2_entries->l2_entry_1));
        }
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        /* L2_ENTRY_2 */
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            return soc_memacc_field32_get(memacc, &(l2_entries->l2_entry_2));
        }
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) {
        /* EXT_L2_ENTRY_1 */
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            return soc_memacc_field32_get(memacc,
                                          &(l2_entries->ext_l2_entry_1));
        }
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) {
        /* L2_ENTRY_2 */
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2, l2_field);
        if (SOC_MEMACC_VALID(memacc)) {
            return soc_memacc_field32_get(memacc,
                                          &(l2_entries->ext_l2_entry_2));
        }
    }

    return 0;
}

/*
 * Function:
 *      _bcm_tr3_l2api_to_l2hw
 * Purpose:
 *      Convert an L2 API data structure to a Triumph3 L2 entry collection
 * Parameters:
 *      unit        Unit number
 *      l2_entries  (OUT) Triumph3 L2 entries structure
 *      l2addr      L2 API data structure
 *      key_only    Only construct key portion
 */
int
_bcm_tr3_l2api_to_l2hw(int unit, _bcm_tr3_l2_entries_t *l2_entries,
                       bcm_l2_addr_t *l2addr, int key_only)
{
    int is_vfi = FALSE;
    bcm_vlan_t vid, vlan_vfi;
    _bcm_tr3_l2_memacc_type_t field_type;
    soc_memacc_t *memacc;
    uint32 field_val;
    int pri_field_len;
    int max_pri;

    pri_field_len = soc_mem_field_length(unit, L2_ENTRY_1m, PRIf);
    max_pri = (1 << pri_field_len) - 1;

    /* Allow the maximum priority as per the device table support */
    if ((l2addr->cos_dst < BCM_PRIO_MIN) || (l2addr->cos_dst > max_pri)) {
            return BCM_E_PARAM;
    }

    /*  BCM_L2_MIRROR is not supported starting from Triumph */
    if (l2addr->flags & BCM_L2_MIRROR) {
        return BCM_E_PARAM;
    }

    /* Setting both flags is not a valid configuration. */
    if ((l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT) &&
        (l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL)) {
        return BCM_E_PARAM;
    }

    vlan_vfi = vid = l2addr->vid;
    if (_BCM_VPN_VFI_IS_SET(vid)) {
        is_vfi = TRUE;
        _BCM_VPN_GET(vlan_vfi, _BCM_VPN_TYPE_VFI, vid);
    }

    field_type = _BCM_TR3_L2_MEMACC_VLAN_ID;
    if (is_vfi) {
        field_type = _BCM_TR3_L2_MEMACC_VFI;
    } else if (!_BCM_VPN_VFI_IS_SET(vid)) {
        VLAN_CHK_ID(unit, vid);
    }

    if (is_vfi || !_BCM_VPN_VFI_IS_SET(vid)) {
        /* VLAN field, all desired entries */
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                          field_type, vlan_vfi);
        /* The key types are different per entry type,
         * so handle individually here */
        if (_BCM_TR3_L2_MEM_TYPE_VALID(l2_entries, L2_ENTRY_1)) {
            /* L2_ENTRY_1 */
            field_val = is_vfi ?
                SOC_MEM_KEY_L2_ENTRY_1_L2_VFI :
                SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE;
            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                        _BCM_TR3_L2_MEMACC_KEY_TYPE);
            soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_1), 
                                   field_val);
        }
        if (_BCM_TR3_L2_MEM_TYPE_VALID(l2_entries, L2_ENTRY_2)) {
            /* L2_ENTRY_2 */
            field_val = is_vfi ?
                SOC_MEM_KEY_L2_ENTRY_2_L2_VFI :
                SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE;
            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                        _BCM_TR3_L2_MEMACC_KEY_TYPE);
            soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 
                                   field_val);
            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                        _BCM_TR3_L2_MEMACC_KEY_TYPE_1);
            soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 
                                   field_val);
        }
        if (_BCM_TR3_L2_MEM_TYPE_VALID(l2_entries, EXT_L2_ENTRY_1)) {
            /* EXT_L2_ENTRY_1 */
            field_val = is_vfi ?
                SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI :
                SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE;
            memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                        _BCM_TR3_L2_MEMACC_KEY_TYPE);
            soc_memacc_field32_set(memacc, &(l2_entries->ext_l2_entry_1), 
                                   field_val);
        }
        if (_BCM_TR3_L2_MEM_TYPE_VALID(l2_entries, EXT_L2_ENTRY_2)) {
            /* EXT_L2_ENTRY_2 */
            field_val = is_vfi ?
                SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI :
                SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE;
            memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                        _BCM_TR3_L2_MEMACC_KEY_TYPE);
            soc_memacc_field32_set(memacc, &(l2_entries->ext_l2_entry_2), 
                                   field_val);
        }
    }

    /* MAC address has individual macros, so handle here */
    field_type = _BCM_TR3_L2_MEMACC_MAC_ADDR;
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                    _BCM_TR3_L2_MEMACC_MAC_ADDR);
        soc_memacc_mac_addr_set(memacc, &(l2_entries->l2_entry_1), 
                                l2addr->mac);
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_MAC_ADDR);
        soc_memacc_mac_addr_set(memacc, &(l2_entries->l2_entry_2), 
                                l2addr->mac);
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) {
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                    _BCM_TR3_L2_MEMACC_MAC_ADDR);
        soc_memacc_mac_addr_set(memacc, &(l2_entries->ext_l2_entry_1), 
                                l2addr->mac);
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) {
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_MAC_ADDR);
        soc_memacc_mac_addr_set(memacc, &(l2_entries->ext_l2_entry_2), 
                                l2addr->mac);
    }

    /* At this point, we've set the bridge/VFI key info ISM or ESM entries.
     * We can use these to search for any existing entry of this key.
     */
    if (key_only) {
        return BCM_E_NONE;
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        if (l2addr->l2mc_group < 0) {
            return BCM_E_PARAM;
        }
        if (_BCM_MULTICAST_IS_VPLS(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_MIM(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_WLAN(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_VLAN(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_NIV(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_L2GRE(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_SUBPORT(l2addr->l2mc_group) ||
            _BCM_MULTICAST_IS_EXTENDER(l2addr->l2mc_group)) {
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_DEST_TYPE, 0x3);
        }

        if (_BCM_MULTICAST_IS_SET(l2addr->l2mc_group)) {
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_DESTINATION,
                                 _BCM_MULTICAST_ID_GET(l2addr->l2mc_group));
        } else {
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_L2MC_PTR,
                                              l2addr->l2mc_group);
        }
    } else {
        bcm_port_t      port = -1;
        bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
        bcm_module_t    modid = -1;
        int             gport_id = -1;

        if (BCM_GPORT_IS_SET(l2addr->port)) {
            _bcm_l2_gport_params_t  g_params;

            if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
                /* Must use an extended entry for this property */
                l2_entries->entry_flags &= _BCM_TR3_L2_SELECT_EXTENDED;
                _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                              _BCM_TR3_L2_MEMACC_SRC_DISCARD, 1);
            } else {
                /*
                 * The SRC_DISCARD field only exists in some entry types,
                 * was cleared with the init of the entries structure
                 * when this was called.
                 */
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_l2_gport_parse(unit, l2addr, &g_params));

                switch (g_params.type) {
                case _SHR_GPORT_TYPE_TRUNK:
                    tgid = g_params.param0;
                    break;
                case  _SHR_GPORT_TYPE_MODPORT:
                    port = g_params.param0;
                    modid = g_params.param1;
                    break;
                case _SHR_GPORT_TYPE_LOCAL_CPU:
                    port = g_params.param0;
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_stk_my_modid_get(unit, &modid));
                    break;
                case _SHR_GPORT_TYPE_SUBPORT_GROUP:
                    gport_id = g_params.param0;
                    break;
                case _SHR_GPORT_TYPE_SUBPORT_PORT:
                    gport_id = g_params.param0;
                    break;
                case _SHR_GPORT_TYPE_MPLS_PORT:
                    gport_id = g_params.param0;
                    break;
                case _SHR_GPORT_TYPE_MIM_PORT:
                case _SHR_GPORT_TYPE_WLAN_PORT:
                case _SHR_GPORT_TYPE_VLAN_PORT:
                case _SHR_GPORT_TYPE_NIV_PORT:
                case _SHR_GPORT_TYPE_TRILL_PORT:
                case _SHR_GPORT_TYPE_L2GRE_PORT:
                case _SHR_GPORT_TYPE_VXLAN_PORT:
                case _SHR_GPORT_TYPE_EXTENDER_PORT:
                    gport_id = g_params.param0;
                    break;
                default:
                    return BCM_E_PORT;
                }
            }
        } else if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            tgid = l2addr->tgid;

        } else {
            PORT_DUALMODID_VALID(unit, l2addr->port);
            BCM_IF_ERROR_RETURN(
                _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                       l2addr->modid, l2addr->port,
                                       &modid, &port));
            /* Check parameters */
            if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port)) {
                return BCM_E_PORT;
            }
        }

        /* Setting l2_entries fields according to parameters */
        if ( BCM_TRUNK_INVALID != tgid) {
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_DEST_TYPE, 0x1);
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_TGID, tgid);
            /*
             * Note:  RTAG is ignored here.  Use bcm_trunk_psc_set to
             * to set for a given trunk.
             */
            if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
                _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                              _BCM_TR3_L2_MEMACC_REMOTE, 1);
            }
        } else if (-1 != port) {
            /* DEST_TYPE field was cleared by calling function. */
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_MODULE_ID, modid);
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_PORT_NUM, port);
        } else if (-1 != gport_id) {
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                          _BCM_TR3_L2_MEMACC_DEST_TYPE, 0x2);
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                           _BCM_TR3_L2_MEMACC_DESTINATION,
                                              gport_id);
        }
    }

    field_type = _BCM_TR3_L2_MEMACC_CLASS_ID_FULL;
    if (l2addr->flags & BCM_L2_SETPRI) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                          _BCM_TR3_L2_MEMACC_RPE, 1);
        if ((SOC_CONTROL(unit)->l2x_group_enable) &&
            (l2addr->group != (l2addr->group &
                               _BCM_TR3_L2_CLASS_ID_REDUCED_MASK))) {
            /* The priority field is an overlay on the high bits of the
             * class id, so we can't use the full size if RPE is set
             * on the reduced entry size.  Must be extended entry now.
             */
            l2_entries->entry_flags &= _BCM_TR3_L2_SELECT_EXTENDED;
        } else {
            field_type = _BCM_TR3_L2_MEMACC_CLASS_ID;
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                          field_type, l2addr->group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions below */

    if (l2addr->flags & BCM_L2_SETPRI) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                  _BCM_TR3_L2_MEMACC_PRI, l2addr->cos_dst);
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        /* CPU bit is only in the extended entry format. */
        l2_entries->entry_flags &= _BCM_TR3_L2_SELECT_EXTENDED;
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                      _BCM_TR3_L2_MEMACC_DST_CPU, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        /* Destination discard bit is only in the extended entry format. */
        l2_entries->entry_flags &= _BCM_TR3_L2_SELECT_EXTENDED;
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                      _BCM_TR3_L2_MEMACC_DST_DISCARD, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        /* Source discard bit is only in the extended entry format. */
        l2_entries->entry_flags &= _BCM_TR3_L2_SELECT_EXTENDED;
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                      _BCM_TR3_L2_MEMACC_SRC_DISCARD, 1);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                      _BCM_TR3_L2_MEMACC_SCP, 1);
    }

    if ((!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_MCAST |
                            BCM_L2_STATIC | BCM_L2_LEARN_LIMIT_EXEMPT))) || 
        l2addr->flags & BCM_L2_LEARN_LIMIT) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                      _BCM_TR3_L2_MEMACC_LIMIT_COUNTED, 1);
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                      _BCM_TR3_L2_MEMACC_PENDING, 1);
    }

    /* If BCM_L2_DES_HIT is set, HITDA has to be set. But if only BCM_L2_HIT
       is set, HITDA has to be set based on aging scheme */
    if ((l2addr->flags & BCM_L2_DES_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        if ((!SOC_CONTROL(unit)->l2x_age_hitsa_only) ||
            (l2addr->flags & BCM_L2_DES_HIT)) {
            _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                              _BCM_TR3_L2_MEMACC_HITDA, 1);
        }
    }

    if ((l2addr->flags & BCM_L2_SRC_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                          _BCM_TR3_L2_MEMACC_HITSA, 1);
    }

    if (l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL) {
       _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                          _BCM_TR3_L2_MEMACC_REMOTE, 1);
       _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                         _BCM_TR3_L2_MEMACC_LIMIT_COUNTED, 1);
    }

    

    if (l2addr->flags & BCM_L2_STATIC) {
        _bcm_tr3_l2hw_entries_field32_set(unit, l2_entries,
                                          _BCM_TR3_L2_MEMACC_STATIC_BIT, 1);
        if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                        _BCM_TR3_L2_MEMACC_STATIC_BIT_1);
            soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 1);
        }
    }

    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                    _BCM_TR3_L2_MEMACC_VALID);
        soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_1), 1);
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_VALID);
        soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 1);
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_VALID_1);
        soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 1);

        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_WIDE);
        soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 1);
        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_WIDE_1);
        soc_memacc_field32_set(memacc, &(l2_entries->l2_entry_2), 1);
    }

    /* EXT L2 tables use FREE field instead of VALID */
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) {
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                    _BCM_TR3_L2_MEMACC_FREE);
        soc_memacc_field32_set(memacc, &(l2_entries->ext_l2_entry_1), 0);
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) {
        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                    _BCM_TR3_L2_MEMACC_FREE);
        soc_memacc_field32_set(memacc, &(l2_entries->ext_l2_entry_2), 0);
    }

    if (0 == l2_entries->entry_flags) {
        /* We couldn't find a memory which can satisfy the requested
         * L2 addr specification.
         */
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_from_l2_1
 * Purpose:
 *      Convert an L2_ENTRY_1 to an L2 API data structure

 * Parameters:
 *      unit         Unit number
 *      l2addr       (OUT) L2 API data structure
 *      L2_entry_1 L2_ENTRY_1 hardware entry
 */
int
_bcm_tr3_l2_from_l2_1(int unit, bcm_l2_addr_t *l2addr,
                      l2_entry_1_entry_t *l2_entry_1)
{
    _bcm_gport_dest_t       dest;
    int                     mb_index;
    bcm_module_t            mod;
    bcm_port_t              port;
    soc_memacc_t            *memacc;
    int                     isGport = 0;
#if defined(INCLUDE_L3)
    int vfi;
#endif
    sal_memset(l2addr, 0, sizeof(*l2addr));

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, KEY_TYPE) ==
        SOC_MEM_KEY_L2_ENTRY_1_L2_VFI) {
#if defined(INCLUDE_L3)
         vfi = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, VFI);
         /* VPLS or MIM VPN */
         if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
              _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
              _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
              BCM_IF_ERROR_RETURN(
                   _bcm_tr3_l2gre_vpn_get(unit, vfi, &l2addr->vid));
         }
#endif
    } else {
         l2addr->vid =
             _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, VLAN_ID);
    }

    memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                _BCM_TR3_L2_MEMACC_MAC_ADDR);
    soc_memacc_mac_addr_get(memacc, l2_entry_1, l2addr->mac);

    _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                         DEST_TYPE) == 0x2) {
         int vp;

         vp = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                               DESTINATION);
         if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                              KEY_TYPE) ==
             SOC_MEM_KEY_L2_ENTRY_1_L2_VFI) {
              /* MPLS/MiM virtual port unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                   dest.mpls_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                   dest.l2gre_id= vp;
                   dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
                   isGport=1;
              } else {
                    /* L2 entries with Stale VPN */
                    dest.gport_type = BCM_GPORT_INVALID;
                    isGport=0;
              }
         } else {
              /* Subport/WLAN unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                   dest.subport_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                   dest.wlan_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                   dest.vlan_vp_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
                    dest.trill_id  = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_TRILL_PORT;
                    isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                    dest.niv_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
                    isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                    dest.extender_id = vp;
                    dest.gport_type = _SHR_GPORT_TYPE_EXTENDER_PORT;
                    isGport=1;
              } else {
                    /* L2 entries with Stale Gport */
                    dest.gport_type = BCM_GPORT_INVALID;
                    isGport=0;
              }
         }
    } else {
#endif /* INCLUDE_L3 */
        BCM_IF_ERROR_RETURN
            (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                         DEST_TYPE) == 0x1) {
            int psc = 0;
            l2addr->tgid =
                _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, TGID);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;

            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                                 REMOTE)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
        } else {
            if (BCM_MAC_IS_MCAST(l2addr->mac)) {
                l2addr->flags |= BCM_L2_MCAST;
                l2addr->l2mc_group =
                    _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                                     L2MC_PTR);
                if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_1, KEY_TYPE) ==
                    SOC_MEM_KEY_L2_ENTRY_1_L2_VFI) {
#if defined(INCLUDE_L3)
                    vfi = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, VFI);
                    if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                         _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                _BCM_MULTICAST_TYPE_VPLS, l2addr->l2mc_group);
                    } else
#endif /* INCLUDE_L3 */
                    {
                         _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                _BCM_MULTICAST_TYPE_MIM, l2addr->l2mc_group);
                    }
                }
                isGport = 0;
            } else {
                mod = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                                       MODULE_ID);
                port = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                                        PORT_NUM);
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                             mod, port, &mod, &port));
                l2addr->modid = mod;
                l2addr->port = port;
                dest.port = l2addr->port;
                dest.modid = l2addr->modid;
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            }
        }
#if defined(INCLUDE_L3)
    }
#endif /* INCLUDE_L3 */

    if (isGport) {
         BCM_IF_ERROR_RETURN
             (_bcm_esw_gport_construct(unit, &dest, &l2addr->port));
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, RPE)) {
        l2addr->flags |= BCM_L2_SETPRI;
        l2addr->cos_dst =
            _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, PRI);
    }
    l2addr->cos_src = l2addr->cos_dst;

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        if (l2addr->flags & BCM_L2_SETPRI) {
            l2addr->group =
                _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                                 CLASS_ID);
        } else {
            /* Use extended field name when available for non-RPE case. */
            l2addr->group = 
                _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                                 CLASS_ID_FULL);
        }
    } else {
        mb_index =
            _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                             MAC_BLOCK_INDEX);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _tr3_mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, SCP)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, PENDING)) {
        l2addr->flags |= BCM_L2_PENDING;
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, STATIC_BIT)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, HITDA)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        if (!SOC_CONTROL(unit)->l2x_age_hitsa_only) {
            l2addr->flags |= BCM_L2_HIT;
        }
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, HITSA)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, LOCAL_SA)) {
        l2addr->flags |= BCM_L2_NATIVE;
    }

    if ((!(l2addr->flags & BCM_L2_STATIC)) || 
        l2addr->flags & BCM_L2_LEARN_LIMIT ) {
        if (!_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1,
                                              LIMIT_COUNTED)) {
            l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
        }
    }

    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_1, REMOTE)) {
       l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_from_l2_2
 * Purpose:
 *      Convert an L2_ENTRY_2 to an L2 API data structure

 * Parameters:
 *      unit         Unit number
 *      l2addr       (OUT) L2 API data structure
 *      l2_entry_2   L2_ENTRY_2 hardware entry
 */
int
_bcm_tr3_l2_from_l2_2(int unit, bcm_l2_addr_t *l2addr,
                      l2_entry_2_entry_t *l2_entry_2)
{
    _bcm_gport_dest_t       dest;
    int                     mb_index;
    bcm_module_t            mod;
    bcm_port_t              port;
    soc_memacc_t            *memacc;
    int  isGport = 0;
#if defined(INCLUDE_L3)
    int vfi;
#endif 
    sal_memset(l2addr, 0, sizeof(*l2addr));

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, KEY_TYPE) ==
        SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) {
#if defined(INCLUDE_L3)
         vfi = _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, VFI);
         /* VPLS or MIM VPN */
         if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
              _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
              _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
              BCM_IF_ERROR_RETURN(
                   _bcm_tr3_l2gre_vpn_get(unit, vfi, &l2addr->vid));
         }

#endif
    } else {
         l2addr->vid =
             _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, VLAN_ID);
    }

    memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                _BCM_TR3_L2_MEMACC_MAC_ADDR);
    soc_memacc_mac_addr_get(memacc, l2_entry_2, l2addr->mac);

    _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                         DEST_TYPE) == 0x2) {
         int vp;

         vp = _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                               DESTINATION);
         if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                              KEY_TYPE) ==
             SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) {
              /* MPLS/MiM virtual port unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                   dest.mpls_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
                   isGport=1;
              } else {
                    /* L2 entries with Stale VPN */
                    dest.gport_type = BCM_GPORT_INVALID;
                    isGport=0;
              }
         } else {
              /* Subport/WLAN unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                   dest.subport_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                   dest.wlan_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                   dest.vlan_vp_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                   isGport=1;
              } else {
                    /* L2 entries with Stale Gport */
                    dest.gport_type = BCM_GPORT_INVALID;
                    isGport=0;
              }
         }
    } else {
#endif /* INCLUDE_L3 */
        BCM_IF_ERROR_RETURN
            (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                         DEST_TYPE) == 0x1) {
            int psc = 0;
            l2addr->tgid =
                _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, TGID);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;

            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                                 REMOTE)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
        } else {
            if (BCM_MAC_IS_MCAST(l2addr->mac)) {
                l2addr->flags |= BCM_L2_MCAST;
                l2addr->l2mc_group =
                    _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                                     L2MC_PTR);
                if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, KEY_TYPE) ==
                    SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) {
#if defined(INCLUDE_L3)
                    vfi = _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry_2, VFI);
                    if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
                         _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                _BCM_MULTICAST_TYPE_VPLS, l2addr->l2mc_group);
                    } else
#endif /* INCLUDE_L3 */
                    {
                         _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group,
                                _BCM_MULTICAST_TYPE_MIM, l2addr->l2mc_group);
                    }
                }    
                isGport = 0;
            } else {
                mod = _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                                       MODULE_ID);
                port = _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                                        PORT_NUM);
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                             mod, port, &mod, &port));
                l2addr->modid = mod;
                l2addr->port = port;
                dest.port = l2addr->port;
                dest.modid = l2addr->modid;
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            }
        }
#if defined(INCLUDE_L3)
    }
#endif /* INCLUDE_L3 */

    if (isGport) {
         BCM_IF_ERROR_RETURN
             (_bcm_esw_gport_construct(unit, &dest, &l2addr->port));
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, RPE)) {
        l2addr->flags |= BCM_L2_SETPRI;
        l2addr->cos_dst =
            _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, PRI);
    }
    l2addr->cos_src = l2addr->cos_dst;

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = 
            _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                             CLASS_ID_FULL);
    } else {
        mb_index =
            _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                             MAC_BLOCK_INDEX);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _tr3_mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, DST_CPU)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, DST_DISCARD)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, SRC_DISCARD)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, SCP)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, PENDING)) {
        l2addr->flags |= BCM_L2_PENDING;
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, STATIC_BIT)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, HITDA)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        if (!SOC_CONTROL(unit)->l2x_age_hitsa_only) {
            l2addr->flags |= BCM_L2_HIT;
        }
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, HITSA)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }
    
    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, LOCAL_SA)) {
        l2addr->flags |= BCM_L2_NATIVE;
    }

    if ((!(l2addr->flags & BCM_L2_STATIC)) || 
        l2addr->flags & BCM_L2_LEARN_LIMIT ) {
        if (!_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2,
                                              LIMIT_COUNTED)) {
            l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
        }
    }

    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry_2, REMOTE)) {
        l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL;
    }

    

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_from_ext_l2_1
 * Purpose:
 *      Convert an EXT_L2_ENTRY_1 to an L2 API data structure

 * Parameters:
 *      unit         Unit number
 *      l2addr       (OUT) L2 API data structure
 *      ext_l2_entry_1 EXT_L2_ENTRY_1 hardware entry
 */
int
_bcm_tr3_l2_from_ext_l2_1(int unit, bcm_l2_addr_t *l2addr,
                          ext_l2_entry_1_entry_t *ext_l2_entry_1)
{
    _bcm_gport_dest_t       dest;
    int                     mb_index;
    bcm_module_t            mod;
    bcm_port_t              port;
    soc_memacc_t            *memacc;
    int  isGport = 0;
#if defined(INCLUDE_L3)
    int vfi;
#endif 
    sal_memset(l2addr, 0, sizeof(*l2addr));

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             KEY_TYPE) ==
        SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI) {
#if defined(INCLUDE_L3)
        vfi = _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                   VFI);
         /* VPLS or MIM VPN */
         if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
              _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
              _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
              BCM_IF_ERROR_RETURN(
                   _bcm_tr3_l2gre_vpn_get(unit, vfi, &l2addr->vid));
         }
#endif
    } else {
         l2addr->vid =
             _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                  VLAN_ID);
    }

    memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                _BCM_TR3_L2_MEMACC_MAC_ADDR);
    soc_memacc_mac_addr_get(memacc, ext_l2_entry_1, l2addr->mac);

    _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             DEST_TYPE) == 0x2) {
        int vp;

        vp = _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                  DESTINATION);
        if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                 KEY_TYPE) ==
            SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI) {
            /* MPLS/MiM virtual port unicast */
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                dest.mpls_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                dest.mim_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                dest.l2gre_id= vp;
                dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
                dest.vxlan_id= vp;
                dest.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
                isGport=1;
            } else {
                /* L2 entries with Stale VPN */
                dest.gport_type = BCM_GPORT_INVALID;
                isGport=0;
            }
       } else {
            /* Subport/WLAN unicast */
            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                dest.subport_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                dest.wlan_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                dest.vlan_vp_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
                dest.trill_id  = vp;
                dest.gport_type = _SHR_GPORT_TYPE_TRILL_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                dest.niv_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
                isGport=1;
            } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
                dest.extender_id = vp;
                dest.gport_type = _SHR_GPORT_TYPE_EXTENDER_PORT;
                isGport=1;
            } else {
                /* L2 entries with Stale VPN */
                dest.gport_type = BCM_GPORT_INVALID;
                isGport=0;
           }
       }
    } else {
#endif /* INCLUDE_L3 */
        if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                 DEST_TYPE) == 0x1) {
            int psc = 0;
            l2addr->tgid =
                _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                     TGID);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;

            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                     REMOTE)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
        } else {
            mod = _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                      MODULE_ID);
            port = _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                       PORT_NUM);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod, port, &mod, &port));
            l2addr->modid = mod;
            l2addr->port = port;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        BCM_IF_ERROR_RETURN
            (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
#if defined(INCLUDE_L3)
    }
#endif /* INCLUDE_L3 */

    if (isGport) {
         BCM_IF_ERROR_RETURN
             (_bcm_esw_gport_construct(unit, &dest, &l2addr->port));
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1, RPE)) {
        l2addr->flags |= BCM_L2_SETPRI;
        l2addr->cos_dst =
            _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1, 
                                                 PRI);
    }
    l2addr->cos_src = l2addr->cos_dst;

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        if (l2addr->flags & BCM_L2_SETPRI) {
            l2addr->group =
                _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                     CLASS_ID);
        } else {
            l2addr->group =
                _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                     CLASS_ID_FULL);
        }
    } else {
        mb_index =
            _BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                 MAC_BLOCK_INDEX);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _tr3_mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1, SCP)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             PENDING)) {
        l2addr->flags |= BCM_L2_PENDING;
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             STATIC_BIT)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             HITDA)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        if (!SOC_CONTROL(unit)->l2x_age_hitsa_only) {
            l2addr->flags |= BCM_L2_HIT;
        }
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             HITSA)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             LOCAL_SA)) {
        l2addr->flags |= BCM_L2_NATIVE;
    }
    
    if ((!(l2addr->flags & BCM_L2_STATIC)) || 
        l2addr->flags & BCM_L2_LEARN_LIMIT) {
        if (!_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                                  LIMIT_COUNTED)) {
            l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
        }
    }

    if (_BCM_TR3_EXT_L2_1_FIELD32_MEMACC_GET(unit, ext_l2_entry_1,
                                             REMOTE)) {
        l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_from_ext_l2_2
 * Purpose:
 *      Convert an EXT_L2_ENTRY_2 to an L2 API data structure

 * Parameters:
 *      unit         Unit number
 *      l2addr       (OUT) L2 API data structure
 *      ext_l2_entry_2 EXT_L2_ENTRY_2 hardware entry
 */
int
_bcm_tr3_l2_from_ext_l2_2(int unit, bcm_l2_addr_t *l2addr,
                          ext_l2_entry_2_entry_t *ext_l2_entry_2)
{
    _bcm_gport_dest_t       dest;
    int                     mb_index;
    bcm_module_t            mod;
    bcm_port_t              port;
    soc_memacc_t            *memacc;
    int  isGport = 0;
#if defined(INCLUDE_L3)
    int vfi;
#endif
    sal_memset(l2addr, 0, sizeof(*l2addr));

     if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                              KEY_TYPE) ==
         SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI) {
#if defined(INCLUDE_L3)
         vfi = _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                    VFI);
         /* VPLS or MIM VPN */
         if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
              _BCM_MPLS_VPN_SET(l2addr->vid, _BCM_MPLS_VPN_TYPE_VPLS, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
              _BCM_MIM_VPN_SET(l2addr->vid, _BCM_MIM_VPN_TYPE_MIM, vfi);
         } else  if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeL2Gre)) {
              BCM_IF_ERROR_RETURN(
                   _bcm_tr3_l2gre_vpn_get(unit, vfi, &l2addr->vid));
         }
#endif
    } else {
         l2addr->vid =
             _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                  VLAN_ID);
    }

    memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                _BCM_TR3_L2_MEMACC_MAC_ADDR);
    soc_memacc_mac_addr_get(memacc, ext_l2_entry_2, l2addr->mac);

    _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
    if (soc_mem_field32_get(unit, EXT_L2_ENTRY_2m,
                            ext_l2_entry_2, DEST_TYPEf) == 0x2) {
         int vp;

         vp = _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                   DESTINATION);
         if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                  KEY_TYPE) ==
             SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI) {
              /* MPLS/MiM virtual port unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                   dest.mpls_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre)) {
                   dest.mim_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
                   isGport=1;
              } else {
                    /* L2 entries with Stale VPN */
                    dest.gport_type = BCM_GPORT_INVALID;
                    isGport=0;
              }
         } else {
              /* Subport/WLAN unicast */
              if (_bcm_vp_used_get(unit, vp, _bcmVpTypeSubport)) {
                   dest.subport_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                   dest.wlan_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
                   isGport=1;
              } else if (_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                   dest.vlan_vp_id = vp;
                   dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
                   isGport=1;
              } else {
                   return BCM_E_INTERNAL; /* Cannot reach here */
              }
         }
    } else {
#endif /* INCLUDE_L3 */
        if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                 DEST_TYPE) == 0x1) {
            int psc = 0;
            l2addr->tgid =
                _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                     TGID);
            bcm_esw_trunk_psc_get(unit, l2addr->tgid, &psc);
            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;

            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                    REMOTE)) {
                l2addr->flags |= BCM_L2_REMOTE_TRUNK;
            }
        } else {
            mod =
                _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                     MODULE_ID);
            port =
                _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                     PORT_NUM);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         mod, port, &mod, &port));
            l2addr->modid = mod;
            l2addr->port = port;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        BCM_IF_ERROR_RETURN
            (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
#if defined(INCLUDE_L3)
    }
#endif /* INCLUDE_L3 */

    if (isGport) {
         BCM_IF_ERROR_RETURN
             (_bcm_esw_gport_construct(unit, &dest, &l2addr->port));
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2, RPE)) {
        l2addr->flags |= BCM_L2_SETPRI;
        l2addr->cos_dst =
            _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2, PRI);
    }
    l2addr->cos_src = l2addr->cos_dst;

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group =
            _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2, 
                                                 CLASS_ID_FULL);
    } else {
        mb_index =
            _BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2, 
                                                 MAC_BLOCK_INDEX);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _tr3_mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             DST_CPU)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             DST_DISCARD)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             SRC_DISCARD)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             SCP)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             PENDING)) {
        l2addr->flags |= BCM_L2_PENDING;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             STATIC_BIT)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             HITDA)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        if (!SOC_CONTROL(unit)->l2x_age_hitsa_only) {
            l2addr->flags |= BCM_L2_HIT;
        }
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             HITSA)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             LOCAL_SA)) {
        l2addr->flags |= BCM_L2_NATIVE;
    }

    if ((!(l2addr->flags & BCM_L2_STATIC)) || 
        l2addr->flags & BCM_L2_LEARN_LIMIT ) {
        if (!_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                                  LIMIT_COUNTED)) {
            l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
        }
    }

    if (_BCM_TR3_EXT_L2_2_FIELD32_MEMACC_GET(unit, ext_l2_entry_2,
                                             REMOTE)) {
        l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2api_from_l2hw
 * Purpose:
 *      Convert an EXT_L2_ENTRY_2 to an L2 API data structure

 * Parameters:
 *      unit         Unit number
 *      l2addr       (OUT) L2 API data structure
 *      l2_entries  Triumph3 L2 entries structure
 *
 * Note:
 *      The l2_entries structure should only have one valid entry.
 */
int
_bcm_tr3_l2api_from_l2hw(int unit, bcm_l2_addr_t *l2addr,
                        _bcm_tr3_l2_entries_t *l2_entries)
{
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        /* L2_ENTRY_1 */
        return _bcm_tr3_l2_from_l2_1(unit, l2addr, &(l2_entries->l2_entry_1));
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        /* L2_ENTRY_2 */
        return _bcm_tr3_l2_from_l2_2(unit, l2addr, &(l2_entries->l2_entry_2));
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) {
        /* EXT_L2_ENTRY_1 */
        return _bcm_tr3_l2_from_ext_l2_1(unit, l2addr,
                                         &(l2_entries->ext_l2_entry_1));
    }
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) {
        /* L2_ENTRY_2 */
        return _bcm_tr3_l2_from_ext_l2_2(unit, l2addr,
                                         &(l2_entries->ext_l2_entry_2));
    }
    return BCM_E_NOT_FOUND;  
}

/*
 * Function:
 *      _bcm_tr3_l2_entries_lookup
 * Purpose:
 *      Serach for an L2 entry specified in an L2 entries structure

 * Parameters:
 *      unit         Unit number
 *      l2_entries   (IN) Triumph3 L2 entries structure (search keys)
 *      l2_entries_lookup  (OUT) Triumph3 L2 entries structure (match return)
 */
int
_bcm_tr3_l2_entries_lookup(int unit, _bcm_tr3_l2_entries_t *l2_entries,
                           _bcm_tr3_l2_entries_t *l2_entries_lookup)
{
    int l2_index, rv = BCM_E_NOT_FOUND;

    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        rv = soc_mem_generic_lookup(unit, L2_ENTRY_1m, MEM_BLOCK_ANY,
                                    _BCM_TR3_L2_MEM_BANKS_ALL,
                                    &(l2_entries->l2_entry_1),
                                    &(l2_entries_lookup->l2_entry_1), &l2_index);

        if (BCM_FAILURE(rv) && (BCM_E_NOT_FOUND != rv)) {
            return rv;
        } else if (BCM_SUCCESS(rv)) {
            /* Found in L2_ENTRY_1 */
            l2_entries_lookup->entry_flags = _BCM_TR3_L2_SELECT_L2_ENTRY_1;
        } /* else not found */
    }

    if ((BCM_E_NOT_FOUND == rv) &&
        (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2)) {
        rv = soc_mem_generic_lookup(unit, L2_ENTRY_2m, MEM_BLOCK_ANY,
                                    _BCM_TR3_L2_MEM_BANKS_ALL,
                                    &(l2_entries->l2_entry_2),
                                    &(l2_entries_lookup->l2_entry_2),
                                    &l2_index);

        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            return rv;
        } else if (BCM_SUCCESS(rv)) {
            /* Found in L2_ENTRY_2 */
            l2_entries_lookup->entry_flags = _BCM_TR3_L2_SELECT_L2_ENTRY_2;
        } /* else not found */
    }

    if ((BCM_E_NOT_FOUND == rv) &&
        (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1)) {
        rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRY_1m, MEM_BLOCK_ANY,
                                    _BCM_TR3_L2_MEM_BANKS_ALL,
                                    &(l2_entries->ext_l2_entry_1),
                                    &(l2_entries_lookup->ext_l2_entry_1),
                                    &l2_index);

        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            return rv;
        } else if (BCM_SUCCESS(rv)) {
            /* Found in EXT_L2_ENTRY_1 */
            l2_entries_lookup->entry_flags =
                _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1;
        } /* else not found */
    }

    if ((BCM_E_NOT_FOUND == rv) &&
        (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2)) {
        rv = soc_mem_generic_lookup(unit, EXT_L2_ENTRY_2m, MEM_BLOCK_ANY,
                                    _BCM_TR3_L2_MEM_BANKS_ALL,
                                    &(l2_entries->ext_l2_entry_2),
                                    &(l2_entries_lookup->ext_l2_entry_2),
                                    &l2_index);

        if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
            return rv;
        } else if (BCM_SUCCESS(rv)) {
            /* Found in EXT_L2_ENTRY_2 */
            l2_entries_lookup->entry_flags =
                _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2;
        } /* else not found */
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2_entries_delete
 * Purpose:
 *      Delete an L2 entry specified in an L2 entries structure

 * Parameters:
 *      unit         Unit number
 *      l2_entries  Triumph3 L2 entries structure
 *
 * Note:
 *      The l2_entries structure should only have one valid entry.
 */
int
_bcm_tr3_l2_entries_delete(int unit, _bcm_tr3_l2_entries_t *l2_entries)
{
    int rv, mb_index = 0;
    int l2_index;    
    soc_control_t *soc = SOC_CONTROL(unit);
    SOC_L2_DEL_SYNC_LOCK(soc);
    /* The entry changed tables on update, so remove the old entry */
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        rv = soc_mem_search(unit, L2_ENTRY_1m, MEM_BLOCK_ANY, &l2_index,
                            (void *)&(l2_entries->l2_entry_1),
                            (void *)&(l2_entries->l2_entry_1), 0);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_delete_return_old(unit, L2_ENTRY_1m, MEM_BLOCK_ANY,
                                           (void *)&(l2_entries->l2_entry_1),
                                           (void *)&(l2_entries->l2_entry_1));
            if (BCM_SUCCESS(rv)) {
                rv = soc_tr3_l2x_sync_delete(unit, L2_ENTRY_1m,
                                             (uint32 *)&(l2_entries->l2_entry_1),
                                             l2_index,0);
            }
        }
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        rv = soc_mem_search(unit, L2_ENTRY_2m, MEM_BLOCK_ANY, &l2_index,
                            (void *)&(l2_entries->l2_entry_2),
                            (void *)&(l2_entries->l2_entry_2), 0);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_delete_return_old(unit, L2_ENTRY_2m, MEM_BLOCK_ANY,
                                           (void *)&(l2_entries->l2_entry_2),
                                           (void *)&(l2_entries->l2_entry_2));
            if (BCM_SUCCESS(rv)) {
                rv = soc_tr3_l2x_sync_delete(unit, L2_ENTRY_2m,
                                             (uint32 *)&(l2_entries->l2_entry_2),
                                             l2_index, 0);
            }
        }
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) {
        rv = soc_mem_search(unit, EXT_L2_ENTRY_1m, MEM_BLOCK_ANY, &l2_index,
                            (void *)&(l2_entries->ext_l2_entry_1),
                            (void *)&(l2_entries->ext_l2_entry_1), 0);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_delete_return_old(unit, EXT_L2_ENTRY_1m,
                                           MEM_BLOCK_ANY,
                                           (void *)&(l2_entries->ext_l2_entry_1),
                                           (void *)&(l2_entries->ext_l2_entry_1));
            if (BCM_SUCCESS(rv)) {
                rv = soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_1m,
                                             (uint32 *)&(l2_entries->ext_l2_entry_1),
                                             l2_index, 0);
            }
        }
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) {
        rv = soc_mem_search(unit, EXT_L2_ENTRY_2m, MEM_BLOCK_ANY, &l2_index,
                            (void *)&(l2_entries->ext_l2_entry_2),
                            (void *)&(l2_entries->ext_l2_entry_2), 0);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_delete_return_old(unit, EXT_L2_ENTRY_2m,
                                           MEM_BLOCK_ANY,
                                           (void *)&(l2_entries->ext_l2_entry_2),
                                           (void *)&(l2_entries->ext_l2_entry_2));
            if (BCM_SUCCESS(rv)) {
                rv = soc_tr3_l2x_sync_delete(unit, EXT_L2_ENTRY_2m,
                                             (uint32 *)&(l2_entries->ext_l2_entry_2),
                                             l2_index, 0);
            }
        }
    } else {
        SOC_L2_DEL_SYNC_UNLOCK(soc);
        /* This should have been caught earlier. */
        return BCM_E_INTERNAL;
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    if (BCM_SUCCESS(rv)) {
        /* Delete the MAC_BLOCK reference, if appropriate */
        if (!SOC_L2X_GROUP_ENABLE_GET(unit)) {
            mb_index = _bcm_tr3_l2hw_entries_field32_get(unit,
                                                         l2_entries,
                                 _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX);
            _bcm_tr3_mac_block_delete(unit, mb_index);
        }
    }

    return rv;
}

#ifdef BCM_KATANA2_SUPPORT 
STATIC int 
_kt2_l2_delete_all_by_hw(int unit, int vfi, uint32 replace_flags) 
{ 
    int rv = BCM_E_NONE; 
    sal_usecs_t start_time; 
    int diff_time; 
    uint32 rval; 
    int field_len; 
    l2x_entry_t l2x_entry; 
    uint32 l2_bulk_match_mask[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32 l2_bulk_match_data[SOC_MAX_MEM_FIELD_WORDS] = {0};
    bcm_mac_t mac_data;
    uint8 static_bit_val = 0;

    sal_memset(&l2x_entry, 0, sizeof(l2x_entry)); 

    soc_mem_field32_set(unit, L2Xm, &l2x_entry, VALIDf, 1); 
           
    field_len = soc_mem_field_length(unit, L2Xm, KEY_TYPEf); 
    soc_mem_field32_set(unit, L2Xm, &l2x_entry, KEY_TYPEf, 
                        (1 << field_len) - 1); 

    /* Match for STATIC_BIT in the Mask */
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, STATIC_BITf, 1);
    }

    /* Match for Unicast or Multicast Mac in the Mask */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

        mac_data[0] = 0x01;
        if (vfi <= 0) {
            soc_mem_mac_addr_set(unit, L2Xm, &l2x_entry, L2__MAC_ADDRf,
                                                              mac_data);
        } else {
            soc_mem_mac_addr_set(unit, L2Xm, &l2x_entry, VFI__MAC_ADDRf,
                                                              mac_data);
        }
    }

    sal_memcpy(l2_bulk_match_mask, &l2x_entry,
               sizeof(l2_bulk_match_mask_entry_t));
    _BCM_ALL_L2X_MEM_LOCK(unit);
    /*Set match mask*/
    rv = WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0, l2_bulk_match_mask);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);

    /* Ensure that STATIC_BIT is set in data as per flag */
    static_bit_val = 0;
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

        if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
            static_bit_val = 1;
        }
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, STATIC_BITf,
                                                 static_bit_val);
    }

    /* Ensure that Bit 40 is set in data as per flag */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

        if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
            mac_data[0] = 0x01;
        }
        if (vfi <= 0) {
            soc_mem_mac_addr_set(unit, L2Xm, &l2x_entry, L2__MAC_ADDRf,
                                                              mac_data);
        } else {
            soc_mem_mac_addr_set(unit, L2Xm, &l2x_entry, VFI__MAC_ADDRf,
                                                               mac_data);
        }
    }
    
    if (vfi <= 0) { 
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, KEY_TYPEf, 
                            KT2_L2_HASH_KEY_TYPE_BRIDGE);
    }

    sal_memcpy(l2_bulk_match_data, &l2x_entry,
               sizeof(l2_bulk_match_data_entry_t));
    /*Set match data*/
    rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0, l2_bulk_match_data);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
  
    rv = READ_L2_BULK_CONTROLr(unit, &rval); 
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv); 
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0); 
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1); 
    rv = WRITE_L2_BULK_CONTROLr(unit, rval); 
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv); 
                     
    /* Delete entries for KT2_L2_HASH_KEY_TYPE_BRIDGE */ 
    if (vfi <= 0) { 
        start_time = sal_time_usecs(); 
        diff_time = 0; 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr); 
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time); 
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv); 
        LOG_VERBOSE(BSL_LS_SOC_COMMON,(BSL_META_U(unit, 
                    "Time taken for bulk del L2_ENTRY_L2_BRIDGE: %d\n"), 
                    diff_time)); 
    } else { 
        /* Delete entries for KT2_L2_HASH_KEY_TYPE_VFI */ 
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, KEY_TYPEf, 
                            KT2_L2_HASH_KEY_TYPE_VFI); 

        sal_memcpy(l2_bulk_match_data, &l2x_entry,
                   sizeof(l2_bulk_match_data_entry_t));
        /*Set match data*/
        rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                                       l2_bulk_match_data);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        
        start_time = sal_time_usecs(); 
        diff_time = 0; 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr); 
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time); 
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv); 
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, 
                    "Time taken for bulk del L2_ENTRY_L2_VFI: %d\n"), 
                    diff_time)); 
    }
    /* Clear parity status on L2 bulk operation */
    BCM_IF_ERROR_RETURN
       (soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                               REG_PORT_ANY, STARTf, 0));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L2_BULK_CONTROLr,
                                REG_PORT_ANY, COMPLETEf, 0));
    _BCM_ALL_L2X_MEM_UNLOCK(unit);
    return rv; 
} 
#endif /* BCM_KATANA2_SUPPORT */

STATIC int
_tr3_l2_delete_all_by_hw(int unit, int vfi, uint32 replace_flags)
{
    int rv = BCM_E_NONE;
    sal_usecs_t start_time;
    int diff_time;        
    uint32 rval;
    int field_len;
    l2_bulk_entry_t l2_bulk;
    l2_entry_1_entry_t l2_entry_1;
    l2_entry_2_entry_t l2_entry_2;
    ext_l2_entry_1_entry_t ext_l2_entry_1;
    ext_l2_entry_2_entry_t ext_l2_entry_2;
    int ext_l2;
    bcm_mac_t mac_data;
    uint8 static_bit_val = 0;

    sal_memset(&l2_entry_1, 0, sizeof(l2_entry_1));
    sal_memset(&l2_entry_2, 0, sizeof(l2_entry_2));
    
    /* First work on L2_ENTRY_1 */
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, VALIDf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, WIDEf, 1);
    
    field_len = soc_mem_field_length(unit, L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf,
                        (1 << field_len) - 1);

    /* Match for STATIC_BIT in the Mask */
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, STATIC_BITf, 1);
    }

    /* Match for Unicast/Multicast Mac in the Mask */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

        mac_data[0] = 0x01;
        if (vfi <= 0) {
            soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &l2_entry_1, L2__MAC_ADDRf,
                                                                      mac_data);
        } else {
            soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &l2_entry_1, VFI__MAC_ADDRf,
                                                                      mac_data);
        }
    }

    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    _BCM_ALL_L2X_MEM_LOCK(unit);
    /* Set match mask */
    /* Index 1 points to L2_BULK_MATCH_MASK address in L2_BULK table */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, WIDEf, 0);

    /* Ensure that STATIC_BIT is set as per flag in data */
    static_bit_val = 0;
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

        if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
            static_bit_val = 1;
        }
        soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, STATIC_BITf,
                                                         static_bit_val);
    }

    /* Ensure that Bit 40 is set as per flag in data */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

        if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
            mac_data[0]=0x01;
        }
        if (vfi <= 0) {
            soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &l2_entry_1, L2__MAC_ADDRf,
                                                                      mac_data);
        } else {
            soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &l2_entry_1, VFI__MAC_ADDRf,
                                                                       mac_data);
        }
    }

    if (vfi <= 0) {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf, 
                            SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE);
    }
    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    /* Set match data */
    /* Index 0 points to L2_BULK_MATCH_DATA address in L2_BULK table */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_1m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    if (vfi <= 0) {
        start_time = sal_time_usecs();
        diff_time = 0;    
        /* Delete entries for SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE */ 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del L2_ENTRY_1_L2_BRIDGE: %d\n"), 
                     diff_time));
    } else {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf, 
                            SOC_MEM_KEY_L2_ENTRY_1_L2_VFI);
        sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
        /* Set match data */
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries for SOC_MEM_KEY_L2_ENTRY_1_L2_VFI */
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del L2_ENTRY_1_L2_VFI: %d\n"), 
                     diff_time));
    }
    /* Then work on L2_ENTRY_2 */
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, VALID_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, VALID_1f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, WIDE_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, WIDE_1f, 1);
    field_len = soc_mem_field_length(unit, L2_ENTRY_2m, KEY_TYPE_0f);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f,
                        (1 << field_len) - 1);

    /* Match for STATIC_BIT in the Mask */
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_0f, 1);
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_1f, 1);
    }

    /* Match for Unicast/Multicast Mac in the Mask */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

        mac_data[0] = 0x01;
        if (vfi <= 0) {
            soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &l2_entry_2, L2__MAC_ADDRf,
                                                                      mac_data);
        } else {
            soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &l2_entry_2, VFI__MAC_ADDRf,
                                                                      mac_data);
        }
    }

    sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
    /* Set match mask */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    if (vfi <= 0) {
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f, 
                                SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f, 
                                SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
    }

    /* Ensure that STATIC_BIT is set as per flag in data */
    static_bit_val = 0;
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

        if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
            static_bit_val = 1;
        }
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_0f,
                                                           static_bit_val);
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_1f,
                                                           static_bit_val);
    }

    /* Ensure that Bit 40 is set as per flag in data */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

        if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
            mac_data[0]=0x01;
        }
        if (vfi <= 0) {
            soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &l2_entry_2, L2__MAC_ADDRf,
                                                                      mac_data);
        } else {
            soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &l2_entry_2, VFI__MAC_ADDRf,
                                                                      mac_data);
        }
    }

    sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_2m));
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    if (vfi <= 0) {
        /* Set the Key type match */
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));

        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries for SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE */ 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del L2_ENTRY_2_L2_BRIDGE: %d\n"), 
                     diff_time));
    } else {
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f, 
                            SOC_MEM_KEY_L2_ENTRY_2_L2_VFI);
        soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f, 
                            SOC_MEM_KEY_L2_ENTRY_2_L2_VFI);
        sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
        /* Set match data */
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        /* Set the Key type match */
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_2_L2_VFI);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_2_L2_VFI);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));

        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries for SOC_MEM_KEY_L2_ENTRY_2_L2_VFI */
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del L2_ENTRY_2_L2_VFI: %d\n"), 
                     diff_time));
    }
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    
    if (!soc_feature(unit, soc_feature_esm_support)) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }
    ext_l2 = soc_property_get(unit, "EXT_L2_BULK_CLEAR", 0);
    if (ext_l2 == 2) {
        goto _ext_l2_2_bulk;
    }
    /* First work on EXT_L2_ENTRY_1 */
    sal_memset(&ext_l2_entry_1, 0, sizeof(ext_l2_entry_1));
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, FREEf, 1);
    if (vfi >= 0) {
        field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, KEY_TYPEf);
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, KEY_TYPEf,
                            (1 << field_len) - 1);
    }

    /* Match for STATIC_BIT in the Mask */
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1,
                                                    STATIC_BITf, 1);
    }

    /* Match for Unicast/Multicast Mac in the Mask */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, MAC_ADDRf,
                                                                      mac_data);
    }

    sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));
    /* Set match mask */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    if (vfi <= 0) {
        sal_memset(&ext_l2_entry_1, 0, sizeof(ext_l2_entry_1));
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, KEY_TYPEf, 
                            SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE);

        /* Ensure that STATIC_BIT is set as per flag in data */
        static_bit_val = 0;
        if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
                static_bit_val = 1;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1,
                                           STATIC_BITf, static_bit_val);
        }

        /* Ensure that Bit 40 is set as per flag in data */
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
                mac_data[0]=0x01;
            }
            soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, MAC_ADDRf,
                                                                          mac_data);
        }
        sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));
    }    
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    if (vfi <= 0) {
        /* Set the Key type match */
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));

        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries */ 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del EXT_L2_ENTRY_1: %d\n"), 
                     diff_time));
    } else {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, KEY_TYPEf, 
                            SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI);

        /* Ensure that STATIC_BIT is set as per flag in data */
        static_bit_val = 0;
        if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
                static_bit_val = 1;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1,
                                           STATIC_BITf, static_bit_val);
        }

        /* Ensure that Bit 40 is set as per flag in data */
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
                mac_data[0]=0x01;
            }
            soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, MAC_ADDRf,
                                                                          mac_data);
        }
        sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));
        /* Set match data */
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        /* Set the Key type match */
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));

        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries */ 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del EXT_L2_ENTRY_1: %d\n"), 
                     diff_time));
    }
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    /* Issue dummy op */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);

_ext_l2_2_bulk:
    if (ext_l2 == 1) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }
    /* Then work on EXT_L2_ENTRY_2 */
    sal_memset(&ext_l2_entry_2, 0, sizeof(ext_l2_entry_2));
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_2, FREEf, 1);
    if (vfi >= 0) {
        field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, KEY_TYPEf);
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, KEY_TYPEf,
                            (1 << field_len) - 1);
    }

    /* Match for STATIC_BIT in the Mask */
    if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2,
                                                    STATIC_BITf, 1);
    }

    /* Match for Unicast/Multicast Mac in the Mask */
    sal_memset(&mac_data, 0x00, sizeof(mac_data));
    if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
        (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, MAC_ADDRf,
                                                                      mac_data);
    }

    sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
    /* Set match mask */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    if (vfi <= 0) {
        sal_memset(&ext_l2_entry_2, 0, sizeof(ext_l2_entry_2));
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, KEY_TYPEf, 
                            SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE);

        /* Ensure that STATIC_BIT is set as per flag in data */
        static_bit_val = 0;
        if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
                static_bit_val = 1;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2,
                    STATIC_BITf, static_bit_val);
        }

        /* Ensure that Bit 40 is set as per flag in data */
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
                mac_data[0]=0x01;
            }
            soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, MAC_ADDRf,
                                                                          mac_data);
        }
        sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
    }
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    if (vfi <= 0) {
        /* Set the Key type match */
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));

        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries */ 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del EXT_L2_ENTRY_2: %d\n"), 
                     diff_time));
    } else {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, KEY_TYPEf, 
                            SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI);

        /* Ensure that STATIC_BIT is set as per flag in data */
        static_bit_val = 0;
        if ((replace_flags & BCM_L2_REPLACE_DYNAMIC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_STATIC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_STATIC) {
                static_bit_val = 1;
            }
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2,
                    STATIC_BITf, static_bit_val);
        }

        /* Ensure that Bit 40 is set as per flag in data */
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        if ((replace_flags & BCM_L2_REPLACE_MATCH_UC) ||
            (replace_flags & BCM_L2_REPLACE_MATCH_MC)) {

            if (replace_flags & BCM_L2_REPLACE_MATCH_MC) {
                mac_data[0]=0x01;
            }
            soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, MAC_ADDRf,
                                                                          mac_data);
        }
        sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
        /* Set match data */
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        /* Set the Key type match */
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
        soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                          KEY_TYPEf, SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_VFI);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));

        start_time = sal_time_usecs();
        diff_time = 0;
        /* Delete entries */ 
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
        diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
        _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Time taken for bulk del EXT_L2_ENTRY_2: %d\n"), 
                     diff_time));
    }
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    /* Issue dummy op */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    _BCM_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv);
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _BCM_ALL_L2X_MEM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _tr3_l2x_delete_all
 * Purpose:
 *      Clear the ISM L2 table by invalidating entries.
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_tr3_l2x_delete_all(int unit)
{
    int chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int chunksize, chnk_end;
    int index_min, wide_entry_half, clear_entry;
    int rv = BCM_E_NONE;
    int *buffer = NULL;
    int mem_size;
    int modified;
    l2_entry_1_entry_t *l2_entry_1;
    soc_memacc_t *memacc_valid, *memacc_key_type;
    uint32 key_type;

    if(soc_property_get(unit, spn_MEM_CLEAR_HW_ACCELERATION, 1) && 
       !SAL_BOOT_SIMULATION) {
        return _tr3_l2_delete_all_by_hw(unit, -1, 0);
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    index_min = soc_mem_index_min(unit, L2_ENTRY_1m);
    mem_idx_max = soc_mem_index_max(unit, L2_ENTRY_1m); /* Current size */
    mem_size = chunksize * sizeof(l2_entry_1_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "L2_ENTRY_1_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    memacc_valid = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                      _BCM_TR3_L2_MEMACC_VALID);
    memacc_key_type = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                         _BCM_TR3_L2_MEMACC_KEY_TYPE);

    soc_mem_lock(unit, L2_ENTRY_1m);
    for (chnk_idx = index_min; chnk_idx <= mem_idx_max; chnk_idx += chunksize) {
        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, L2_ENTRY_1m, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, buffer);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        modified = FALSE;
        wide_entry_half = FALSE;
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry_1 =
                soc_mem_table_idx_to_pointer(unit, L2_ENTRY_1m,
                                             l2_entry_1_entry_t *,
                                             buffer, ent_idx);
            if (wide_entry_half) {
                /* Clear the second half of a wide entry */
                sal_memcpy(l2_entry_1,
                           soc_mem_entry_null(unit, L2_ENTRY_1m),
                           sizeof(l2_entry_1_entry_t));
                wide_entry_half = FALSE;
                continue;
            }

            if (!soc_memacc_field32_get(memacc_valid, l2_entry_1)) {
                continue;
            }
            key_type = soc_memacc_field32_get(memacc_key_type, l2_entry_1);

            
            clear_entry = FALSE;
            if ((SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE == key_type) ||
                (SOC_MEM_KEY_L2_ENTRY_1_L2_VFI == key_type)) {
                clear_entry = TRUE;
            }
            if ((SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE == key_type) ||
                (SOC_MEM_KEY_L2_ENTRY_2_L2_VFI == key_type)) {
                clear_entry = TRUE;
                wide_entry_half = TRUE;
            }
            if (clear_entry) {
                sal_memcpy(l2_entry_1,
                           soc_mem_entry_null(unit, L2_ENTRY_1m),
                           sizeof(l2_entry_1_entry_t));
                modified = TRUE;
            }
        }
        if (!modified) {
            continue;
        }
        rv = soc_mem_write_range(unit, L2_ENTRY_1m, MEM_BLOCK_ALL,
                                 chnk_idx, chnk_idx_max, buffer);
        if (SOC_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, buffer);

    soc_mem_unlock(unit, L2_ENTRY_1m);

    /* External memories don't need key_type checks, so use
     * standard memory clear operation. */
    if (soc_feature(unit, soc_feature_esm_support)) {
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_clear(unit, EXT_L2_ENTRY_1m, COPYNO_ALL, FALSE);
        }
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_clear(unit, EXT_L2_ENTRY_2m, COPYNO_ALL, FALSE);
        }
    }

    if (SOC_CONTROL(unit)->arlShadow != NULL) {
        sal_mutex_take(SOC_CONTROL(unit)->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(SOC_CONTROL(unit)->arlShadow);
        sal_mutex_give(SOC_CONTROL(unit)->arlShadowMutex);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2_learn_limit_system_set
 * Purpose:
 *      Set MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_learn_limit_system_set(int unit, uint32 flags, int limit)
{
    uint32 rval, orval, regval;
    int tocpu, drop, enable;
    
    if (limit < 0) {
        tocpu = 0;
        drop = 0;
        limit = BCM_MAC_LIMIT_MAX(unit);
        enable = 0;
    } else {
        tocpu = flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0;
        drop = flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0;
        enable = 1;
    }

    /* Starting from Triumph this flag is not supported */
    if (flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_RSEL1_MISC_CONTROLr(unit, &regval));
    soc_reg_field_set(unit, RSEL1_MISC_CONTROLr, &regval,
                      MAC_LIMIT_ENABLEf, enable); 
    BCM_IF_ERROR_RETURN(WRITE_RSEL1_MISC_CONTROLr(unit, regval));

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    orval = rval;
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, 
                      SYS_OVER_LIMIT_TOCPUf, tocpu);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval,
                      SYS_OVER_LIMIT_DROPf, drop);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, SYS_LIMITf, limit);
    /*
     * Enable/Disable MAC limiting for internal L2 and also
     * for external L2 if it is available.
     */
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, 
                      ENABLE_INTERNAL_L2_ENTRYf, enable);
    if (soc_feature(unit, soc_feature_esm_support)) {
        if ((SOC_MEM_IS_VALID(unit, EXT_L2_ENTRY_1m) &&
             (0 < soc_mem_index_count(unit, EXT_L2_ENTRY_1m))) ||
            ((SOC_MEM_IS_VALID(unit, EXT_L2_ENTRY_2m) &&
             (0 < soc_mem_index_count(unit, EXT_L2_ENTRY_2m))))) {
            soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval,
                    ENABLE_EXTERNAL_L2_ENTRYf, enable);
        }
    }
    if (rval != orval) {
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMIT_CONTROLr(unit, rval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_learn_limit_init
 * Purpose:
 *      Init the system to support MAC learn limit
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_learn_limit_init(int unit)
{
    int32 maxInitVal;
    int rv, idx, idx_min, idx_max;
    uint8 *port_or_trunk_mac_limit_buf = NULL;
    uint8 *vlan_or_vfi_mac_limit_buf = NULL;
    vlan_or_vfi_mac_limit_entry_t   *vlan_or_vfi_mac_limit_entry = NULL;
    port_or_trunk_mac_limit_entry_t *port_or_trunk_mac_limit_entry = NULL;
    maxInitVal = BCM_MAC_LIMIT_MAX(unit); 

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_learn_limit_system_set(unit, 0, -1));
    BCM_IF_ERROR_RETURN
        (soc_mem_clear(unit, PORT_OR_TRUNK_MAC_LIMITm, COPYNO_ALL, FALSE));
    BCM_IF_ERROR_RETURN
        (soc_mem_clear(unit, VLAN_OR_VFI_MAC_LIMITm, COPYNO_ALL, FALSE));

    /* PORT_OR_TRUNK_MAC_LIMIT table*/
    port_or_trunk_mac_limit_buf = soc_cm_salloc(unit,
                   SOC_MEM_TABLE_BYTES(unit,PORT_OR_TRUNK_MAC_LIMITm),
                   "port_or_trunk");
    if (port_or_trunk_mac_limit_buf == NULL) {
        return BCM_E_MEMORY;
    }

    idx_min = soc_mem_index_min(unit, PORT_OR_TRUNK_MAC_LIMITm); 
    idx_max = soc_mem_index_max(unit, PORT_OR_TRUNK_MAC_LIMITm);
    rv = soc_mem_read_range(unit, PORT_OR_TRUNK_MAC_LIMITm,
                            MEM_BLOCK_ANY, idx_min, idx_max,
                            port_or_trunk_mac_limit_buf);
    
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for(idx = idx_min; idx<= idx_max; idx++) {
        port_or_trunk_mac_limit_entry = 
            soc_mem_table_idx_to_pointer(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                        port_or_trunk_mac_limit_entry_t *,
                                        port_or_trunk_mac_limit_buf,
                                        idx);
        soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit,
                            port_or_trunk_mac_limit_entry,
                            LIMITf, maxInitVal);
    }
    rv = soc_mem_write_range(unit, PORT_OR_TRUNK_MAC_LIMITm, MEM_BLOCK_ALL,
                             idx_min, idx_max, port_or_trunk_mac_limit_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
        
    /* VLAN_OR_VFI_MAC_LIMIT table*/
    vlan_or_vfi_mac_limit_buf = soc_cm_salloc(unit,
                   SOC_MEM_TABLE_BYTES(unit, VLAN_OR_VFI_MAC_LIMITm),
                   "vlan_or_vfi");
    if (vlan_or_vfi_mac_limit_buf == NULL) {
        return BCM_E_MEMORY;
    }
       
    idx_min = soc_mem_index_min(unit, VLAN_OR_VFI_MAC_LIMITm); 
    idx_max = soc_mem_index_max(unit, VLAN_OR_VFI_MAC_LIMITm);
    rv = soc_mem_read_range(unit, VLAN_OR_VFI_MAC_LIMITm,
                            MEM_BLOCK_ANY, idx_min, idx_max,
                            vlan_or_vfi_mac_limit_buf);
    
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for(idx = idx_min; idx<= idx_max; idx++) {
        vlan_or_vfi_mac_limit_entry = 
            soc_mem_table_idx_to_pointer(unit, VLAN_OR_VFI_MAC_LIMITm,
                                        vlan_or_vfi_mac_limit_entry_t *,
                                        vlan_or_vfi_mac_limit_buf,
                                        idx);
        soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit,
                            vlan_or_vfi_mac_limit_entry,
                            LIMITf, maxInitVal);
    }
    rv = soc_mem_write_range(unit, VLAN_OR_VFI_MAC_LIMITm, MEM_BLOCK_ALL,
                             idx_min, idx_max, vlan_or_vfi_mac_limit_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
        

cleanup:

    if (port_or_trunk_mac_limit_buf) {
        soc_cm_sfree(unit, port_or_trunk_mac_limit_buf);
    }

    if (vlan_or_vfi_mac_limit_buf) {
        soc_cm_sfree(unit, vlan_or_vfi_mac_limit_buf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2_hash_dynamic_replace
 * Purpose:
 *      Given an L2 entries structure, search for dynamic entries to
 *      eject in favor of the new entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_entries  (IN/OUT) Triumph3 L2 entries structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      In case new entry is double wide,we check for any dynamic narrow entries
 *      which can be deleted or free, and then do an insert. del_low_idx and
 *      del_high_idx are set if the entry is not free and has to be deleted
 *      before insert.
 */
int 
_bcm_tr3_l2_hash_dynamic_replace(int unit, _bcm_tr3_l2_entries_t *l2_entries)
{
    uint32 *kt, valid;
    void *entry;
    int rv, rvt, base_index, l2_index=0, skip_entry;
    int cf_hit, cf_unhit, narrow_avail = 0;
    int del_low_idx = 0, del_high_idx = 0;
    soc_mem_t mem;
    soc_field_t ktf;
    uint8 i, bidx, num_ent;
    _soc_ism_mem_banks_t mem_banks;
    soc_memacc_t *memacc;
    bcm_mac_t mac;
    uint32 l2_entry[SOC_MAX_MEM_WORDS], result, bucket_index;
    uint32 key_type_1[] = {SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE,
                           SOC_MEM_KEY_L2_ENTRY_1_L2_VFI};
    uint32 key_type_2[] = {SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE,
                           SOC_MEM_KEY_L2_ENTRY_2_L2_VFI};
    int index_cache[_SOC_ISM_MAX_BANKS] = {-1};
    
    /* NOTE: Implementation assumes that enough criteria/info will be set 
             in the bcm_l2_addr_t param to not have any ambiguity between
             ENTRY_1 and ENTRY_2, i.e the following conditions would not
             be true together */ 
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        mem = L2_ENTRY_1m;
        entry = &l2_entries->l2_entry_1;
        kt = key_type_1;
        valid = VALIDf;
        ktf = KEY_TYPEf;
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        mem = L2_ENTRY_2m;
        entry = &l2_entries->l2_entry_2;
        kt = key_type_2;
        valid = VALID_0f;
        ktf = KEY_TYPE_0f;
    } else {
        return BCM_E_PARAM;
    }
    rv = soc_ism_get_banks_for_mem(unit, mem, mem_banks.banks, 
                                   mem_banks.bank_size, &mem_banks.count);
    if (SOC_FAILURE(rv)) {
        return BCM_E_INTERNAL;
    }
    if (mem_banks.count == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* Don't let the table rearrange in the background */
    BCM_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    cf_hit = cf_unhit = -1;
    SOC_L2X_MEM_LOCK(unit);
    for (bidx = 0; bidx < mem_banks.count; bidx++) {
        /* base index depends on the view, L2_ENTRY_1 or L2_ENTRY_2 */ 
        rv = soc_generic_hash(unit, mem, entry,
                              (uint32)1 << mem_banks.banks[bidx],
                              0, &base_index, &result,
                              &bucket_index, &num_ent);
        if (SOC_FAILURE(rv)) {
            rv = BCM_E_INTERNAL;
            break;
        }
        /* Cache indexes for later use, based on the newer entry type*/
        index_cache[bidx] = base_index;
        for (i = 0; i < num_ent; i++ ) {
            skip_entry = FALSE;
            l2_index = base_index + i;
            rv = soc_mem_read(unit, mem, COPYNO_ALL, l2_index, l2_entry);
            if (SOC_FAILURE(rv)) {
                rv = BCM_E_MEMORY;
                break;
            }
            if (!soc_mem_field32_get(unit, mem, &l2_entry, valid)) {
                if (mem == L2_ENTRY_2m) {
                    soc_mem_t mem1 = L2_ENTRY_1m;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL,
                                      (2 * l2_index) + 1, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if (soc_mem_field32_get(unit, mem1, &l2_entry, VALIDf)) {
                        continue;
                    }
                }
                /* Found invalid entry - stop the search, victim found */
                cf_unhit = l2_index; 
                break;
            }
            /* Check if the newer entry and existing entry belong to the same 
               memory view */
            if (((soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[0]) ||
                 (soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[1]))) {
                if (mem == L2_ENTRY_1m) {
                    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                         STATIC_BIT)) {
                        /* Skip static entries */
                        skip_entry = TRUE;
                    }
                    if (!skip_entry) {
                        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                          _BCM_TR3_L2_MEMACC_MAC_ADDR);
                        soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                        if (BCM_MAC_IS_MCAST(mac)) {
                            skip_entry = TRUE;
                        }
                    }
                    if (!skip_entry) {
                        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             HITDA) ||
                            _BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             HITSA)) {
                            cf_hit =  l2_index;
                        } else {
                            /* Found unhit entry -
                             * stop search, victim found */
                            cf_unhit = l2_index;
                            break;
                        }
                    }
                } else {
                    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                         STATIC_BIT)) {
                        /* Skip static entries */
                        skip_entry = TRUE;
                    }
                    if (!skip_entry) {
                        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                          _BCM_TR3_L2_MEMACC_MAC_ADDR);
                        soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                        if (BCM_MAC_IS_MCAST(mac)) {
                            skip_entry = TRUE;
                        }
                    }
                    if (!skip_entry) {
                        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             HITDA) ||
                            _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             HITSA)) {
                            cf_hit = l2_index;
                        } else {
                            /* Found unhit entry -
                             * stop search, victim found */
                            cf_unhit = l2_index;
                            break;
                        }
                    }
                }
            }
        }
        if (SOC_FAILURE(rv) || (cf_unhit >= 0)) {
            break;
        }
    }
    
    if ((cf_unhit >= 0) || (cf_hit >= 0)) {
        /* Prefer unhit entries over hit entries */
        l2_index = (cf_unhit >= 0)? cf_unhit : cf_hit;
        rv = soc_mem_delete_index(unit, mem, MEM_BLOCK_ALL, 
                                                 l2_index);
    } else {
        if (mem == L2_ENTRY_2m) {
            soc_mem_t mem1 = L2_ENTRY_1m;
            
            /* Try replacing 2 consecutive narrow entries with one wide */
            kt = key_type_1;
            for (bidx = 0; bidx < mem_banks.count; bidx++) {
                for (i = 0; i < 2; i++) {
                    narrow_avail = 0;
                    del_low_idx  = 0;
                    del_high_idx = 0;
                    /* We cached the wide entry index pre-emptively */
                    l2_index = index_cache[bidx]*2 + i*2;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL, l2_index, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if (!soc_mem_field32_get(unit, mem1, &l2_entry, VALIDf)) {
                        /* Found one invalid entry */
                        narrow_avail++;
                    } else if (((soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[0]) ||
                                (soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[1]))) {
                        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             STATIC_BIT)) {
                            /* Skip static entries */
                            continue;
                        } else {
                            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                              _BCM_TR3_L2_MEMACC_MAC_ADDR);
                            soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                            if (BCM_MAC_IS_MCAST(mac)) {
                                continue;
                            }
                        }
                        narrow_avail++;
                        del_low_idx = 1;
                    }
                    /* narrow_avail should be one */ 
                    l2_index++;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL, l2_index, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if (!soc_mem_field32_get(unit, mem1, &l2_entry, VALIDf)) {
                        /* Found two invalid entry */
                        narrow_avail++;
                        break;
                    } else if (((soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[0]) ||
                                (soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[1]))) {
                        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             STATIC_BIT)) {
                            /* Skip static entries */
                            continue;
                        } else {
                            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                              _BCM_TR3_L2_MEMACC_MAC_ADDR);
                            soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                            if (BCM_MAC_IS_MCAST(mac)) {
                                continue;
                            }
                        }
                        narrow_avail++;
                        del_high_idx = 1;
                        break;
                    }
                }
                if (SOC_FAILURE(rv)) {
                    break;
                }
                if (narrow_avail == 2) {
                    if (del_low_idx) {
                        rv = soc_mem_delete_index(unit, mem1, MEM_BLOCK_ALL, l2_index-1);
                        if (SOC_FAILURE(rv)) {
                            rv = BCM_E_MEMORY;
                            break;
                        }
                    }
                    if (del_high_idx) {
                        rv = soc_mem_delete_index(unit, mem1, MEM_BLOCK_ALL, l2_index);
                        if (SOC_FAILURE(rv)) {
                            rv = BCM_E_MEMORY;
                            break;
                        }   
                    }
                    /* Get the cached wide entry index */
                    l2_index = index_cache[bidx] + i;
                    break;
                }
            }
            /* we may endup here when first entry is invalid and 
               second one is static */
            if (SOC_FAILURE(rv) || (narrow_avail < 2)) {
                /* coverity[value_overwrite] */
                l2_index = 0; /* Never used, but stops compiler uninit complaint */
                rv = BCM_E_FULL;     /* no dynamics to delete */
            }
        } else {
            /* Try to find a wide entry and replace with a narrow one */
            soc_mem_t mem1 = L2_ENTRY_2m;
            kt = key_type_2;
            for (bidx = 0; bidx < mem_banks.count; bidx++) {
                for (i = 0; i < 2; i++) {
                    l2_index = index_cache[bidx]/2 + i;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL, l2_index, l2_entry);  
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if ((soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[0]) ||
                         (soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[1])) {
                        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                                     STATIC_BIT)) {
                            continue;
                        }
                        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                                      _BCM_TR3_L2_MEMACC_MAC_ADDR);
                        soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                        if (BCM_MAC_IS_MCAST(mac)) {
                            continue;
                        }
                        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry, HITDA) ||
                            _BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry, HITSA)) {
                             cf_hit = l2_index;
                        } else {
                            /* Found unhit entry -
                             * stop search, victim found */
                             cf_unhit = l2_index;
                             break;
                        }
                    }
                }
                if (cf_unhit < 0) {
                    if (cf_hit >= 0) {
                        l2_index = cf_hit;
                    } else {
                       rv = BCM_E_FULL;    
                    }
                }
            }
            if (SOC_SUCCESS(rv)) {
                rv = soc_mem_delete_index(unit, mem1, MEM_BLOCK_ALL, l2_index);
           }
        }
    }
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
    }
    SOC_L2X_MEM_UNLOCK(unit);
    rvt = soc_l2x_thaw(unit);
    return SOC_FAILURE(rv) ? rv : rvt;
}

/*
 * Function:
 *      _bcm_tr3_l2_hash_pending_override
 * Purpose:
 *      Given an L2 entries structure, search for pending entries to
 *      eject in favor of the new entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_entries  (IN/OUT) Triumph3 L2 entries structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      In case new entry is double wide,we check for any pending narrow entries
 *      which can be deleted or free, and then do an insert. del_low_idx and
 *      del_high_idx are set if the entry is not free and has to be deleted
 *      before insert.
 */
int
_bcm_tr3_l2_hash_pending_override(int unit, _bcm_tr3_l2_entries_t *l2_entries)
{
    uint32 *kt, valid;
    void *entry;
    int rv, rvt, base_index, l2_index=0, skip_entry;
    int p_hit, narrow_avail = 0;
    int del_low_idx = 0, del_high_idx = 0;
    soc_mem_t mem;
    soc_field_t ktf;
    uint8 i, bidx, num_ent;
    _soc_ism_mem_banks_t mem_banks;
    soc_memacc_t *memacc;
    bcm_mac_t mac;
    uint32 l2_entry[SOC_MAX_MEM_WORDS], result, bucket_index;
    uint32 key_type_1[] = {SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE,
                           SOC_MEM_KEY_L2_ENTRY_1_L2_VFI};
    uint32 key_type_2[] = {SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE,
                           SOC_MEM_KEY_L2_ENTRY_2_L2_VFI};
    int index_cache[_SOC_ISM_MAX_BANKS] = {-1};

    /* NOTE: Implementation assumes that enough criteria/info will be set
             in the bcm_l2_addr_t param to not have any ambiguity between
             ENTRY_1 and ENTRY_2, i.e the following conditions would not
             be true together */
    if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        mem = L2_ENTRY_1m;
        entry = &l2_entries->l2_entry_1;
        kt = key_type_1;
        valid = VALIDf;
        ktf = KEY_TYPEf;
    } else if (l2_entries->entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        mem = L2_ENTRY_2m;
        entry = &l2_entries->l2_entry_2;
        kt = key_type_2;
        valid = VALID_0f;
        ktf = KEY_TYPE_0f;
    } else {
        return BCM_E_PARAM;
    }
    rv = soc_ism_get_banks_for_mem(unit, mem, mem_banks.banks,
                                   mem_banks.bank_size, &mem_banks.count);
    if (SOC_FAILURE(rv)) {
        return BCM_E_INTERNAL;
    }
    if (mem_banks.count == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* Don't let the table rearrange in the background */
    BCM_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    p_hit = -1;
    SOC_L2X_MEM_LOCK(unit);
    for (bidx = 0; bidx < mem_banks.count; bidx++) {
        /* base index depends on the view, L2_ENTRY_1 or L2_ENTRY_2 */
        rv = soc_generic_hash(unit, mem, entry,
                              (uint32)1 << mem_banks.banks[bidx],
                              0, &base_index, &result,
                              &bucket_index, &num_ent);
        if (SOC_FAILURE(rv)) {
            rv = BCM_E_INTERNAL;
            break;
        }
        /* Cache indexes for later use, based on the newer entry type*/
        index_cache[bidx] = base_index;
        for (i = 0; i < num_ent; i++ ) {
            skip_entry = FALSE;
            l2_index = base_index + i;
            rv = soc_mem_read(unit, mem, COPYNO_ALL, l2_index, l2_entry);
            if (SOC_FAILURE(rv)) {
                rv = BCM_E_MEMORY;
                break;
            }
            if (!soc_mem_field32_get(unit, mem, &l2_entry, valid)) {
                if (mem == L2_ENTRY_2m) {
                    soc_mem_t mem1 = L2_ENTRY_1m;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL,
                                      (2 * l2_index) + 1, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if (soc_mem_field32_get(unit, mem1, &l2_entry, VALIDf)) {
                        continue;
                    }
                }
                /* Found invalid entry - stop the search, victim found */
                p_hit = l2_index;
                break;
            }
            /* Check if the newer entry and existing entry belong to the same
               memory view */
            if (((soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[0]) ||
                 (soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[1]))) {
                if (mem == L2_ENTRY_1m) {
                    if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                         STATIC_BIT)) {
                        /* Skip static entries */
                        skip_entry = TRUE;
                    }
                    if (!skip_entry) {
                        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                          _BCM_TR3_L2_MEMACC_MAC_ADDR);
                        soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                        if (BCM_MAC_IS_MCAST(mac)) {
                            skip_entry = TRUE;
                        }
                    }
                    if (!skip_entry) {
                        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                    PENDING)) {
                            p_hit = l2_index;
                            break;
                        }
                    }
                } else {
                    if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                         STATIC_BIT)) {
                        /* Skip static entries */
                        skip_entry = TRUE;
                    }
                    if (!skip_entry) {
                        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                          _BCM_TR3_L2_MEMACC_MAC_ADDR);
                        soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                        if (BCM_MAC_IS_MCAST(mac)) {
                            skip_entry = TRUE;
                        }
                    }
                    if (!skip_entry) {
                        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             PENDING)) {
                            p_hit = l2_index;
                            break;
                        }
                    }
                }
            }
        }
        if (SOC_FAILURE(rv) || (p_hit >= 0)) {
            break;
        }
    }

    if (p_hit >= 0) {
        l2_index = p_hit;
        rv = soc_mem_delete_index(unit, mem, MEM_BLOCK_ALL,
                                                 l2_index);
    } else {
        if (mem == L2_ENTRY_2m) {
            soc_mem_t mem1 = L2_ENTRY_1m;

            /* Try replacing 2 consecutive narrow entries with one wide */
            kt = key_type_1;
            for (bidx = 0; bidx < mem_banks.count; bidx++) {
                for (i = 0; i < 2; i++) {
                    narrow_avail = 0;
                    del_low_idx  = 0;
                    del_high_idx = 0;
                    /* We cached the wide entry index pre-emptively */
                    l2_index = index_cache[bidx]*2 + i*2;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL, l2_index, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if (!soc_mem_field32_get(unit, mem1, &l2_entry, VALIDf)) {
                        /* Found one invalid entry */
                        narrow_avail++;
                    } else if (((soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[0]) ||
                                (soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[1]))) {
                        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             STATIC_BIT)) {
                            /* Skip static entries */
                            continue;
                        } else {
                            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                              _BCM_TR3_L2_MEMACC_MAC_ADDR);
                            soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                            if (BCM_MAC_IS_MCAST(mac)) {
                                continue;
                            }
                        }
                        narrow_avail++;
                        del_low_idx = 1;
                    }
                    /* narrow_avail should be one */
                    l2_index++;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL, l2_index, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if (!soc_mem_field32_get(unit, mem1, &l2_entry, VALIDf)) {
                        /* Found two invalid entry */
                        narrow_avail++;
                        break;
                    } else if (((soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[0]) ||
                                (soc_mem_field32_get(unit, mem1, &l2_entry,
                                                     KEY_TYPEf) == kt[1]))) {
                        if (_BCM_TR3_L2_1_FIELD32_MEMACC_GET(unit, l2_entry,
                                                             STATIC_BIT)) {
                            /* Skip static entries */
                            continue;
                        } else {
                            memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_1,
                                              _BCM_TR3_L2_MEMACC_MAC_ADDR);
                            soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                            if (BCM_MAC_IS_MCAST(mac)) {
                                continue;
                            }
                        }
                        narrow_avail++;
                        del_high_idx = 1;
                        break;
                    }
                }
                if (SOC_FAILURE(rv)) {
                    break;
                }
                if (narrow_avail == 2) {
                    if (del_low_idx) {
                        rv = soc_mem_delete_index(unit, mem1, MEM_BLOCK_ALL, l2_index-1);
                        if (SOC_FAILURE(rv)) {
                            rv = BCM_E_MEMORY;
                            break;
                        }
                    }
                    if (del_high_idx) {
                        rv = soc_mem_delete_index(unit, mem1, MEM_BLOCK_ALL, l2_index);
                        if (SOC_FAILURE(rv)) {
                            rv = BCM_E_MEMORY;
                            break;
                        }
                    }
                    /* Get the cached wide entry index */
                    l2_index = index_cache[bidx] + i;
                    break;
                }
            }
            /* we may endup here when first entry is invalid and
               second one is static */
            if (SOC_FAILURE(rv) || (narrow_avail < 2)) {
                /* coverity[value_overwrite] */
                l2_index = 0; /* Never used, but stops compiler uninit complaint */
                rv = BCM_E_FULL;     /* no dynamics to delete */
            }
        } else {
            /* Try to find a wide entry and replace with a narrow one */
            soc_mem_t mem1 = L2_ENTRY_2m;
            kt = key_type_2;
            for (bidx = 0; bidx < mem_banks.count; bidx++) {
                for (i = 0; i < 2; i++) {
                    l2_index = index_cache[bidx]/2 + i;
                    rv = soc_mem_read(unit, mem1, COPYNO_ALL, l2_index, l2_entry);
                    if (SOC_FAILURE(rv)) {
                        rv = BCM_E_MEMORY;
                        break;
                    }
                    if ((soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[0]) ||
                         (soc_mem_field32_get(unit, mem, &l2_entry, ktf) == kt[1])) {
                        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry,
                                                                     STATIC_BIT)) {
                            continue;
                        }
                        memacc = _BCM_TR3_L2_MEMACC(unit, L2_ENTRY_2,
                                                      _BCM_TR3_L2_MEMACC_MAC_ADDR);
                        soc_memacc_mac_addr_get(memacc, l2_entry, mac);
                        if (BCM_MAC_IS_MCAST(mac)) {
                            continue;
                        }
                        if (_BCM_TR3_L2_2_FIELD32_MEMACC_GET(unit, l2_entry, PENDING)) {
                             p_hit = l2_index;
                             break;
                        }
                    }
                }
                if (p_hit >= 0) {
                    l2_index = p_hit;
                } else {
                    rv = BCM_E_FULL;
                }
            }
            if (SOC_SUCCESS(rv)) {
                rv = soc_mem_delete_index(unit, mem1, MEM_BLOCK_ALL, l2_index);
           }
        }
    }
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
    }
    SOC_L2X_MEM_UNLOCK(unit);
    rvt = soc_l2x_thaw(unit);
    return SOC_FAILURE(rv) ? rv : rvt;
}

/****************************************************************************
 *
 * L2 Callbacks Registration
 */
static bcm_l2_addr_callback_t _bcm_tr3_l2_cbs[SOC_MAX_NUM_DEVICES];
static void *_bcm_tr3_l2_cb_data[SOC_MAX_NUM_DEVICES];

void
_bcm_tr3_l2_register_callback(int unit, uint32 flags,
                              soc_mem_t mem_type,
                              l2_combo_entry_t *entry_del,
                              l2_combo_entry_t *entry_add,
                              void *fn_data)
{
    if (_bcm_tr3_l2_cbs[unit] != NULL) {
        bcm_l2_addr_t l2addr_del, l2addr_add;
        uint32 set_flags = 0; /* Common flags: Move, From/to native */
        _bcm_tr3_l2_entries_t l2_entries;

        /* First, set up the entries:  decode HW entries and set flags */
        if (entry_del != NULL) {
            l2_entries.entry_flags = 0;
            switch (mem_type) {
            case L2_ENTRY_1m:
                sal_memcpy(&l2_entries.l2_entry_1, entry_del, 
                           sizeof(l2_entry_1_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_L2_ENTRY_1;
                break;
            case L2_ENTRY_2m:
                sal_memcpy(&l2_entries.l2_entry_2, entry_del, 
                           sizeof(l2_entry_2_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_L2_ENTRY_2;
                break;
            case EXT_L2_ENTRY_1m:
                sal_memcpy(&l2_entries.ext_l2_entry_1, entry_del, 
                           sizeof(ext_l2_entry_1_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1;
                break;
            case EXT_L2_ENTRY_2m:
                sal_memcpy(&l2_entries.ext_l2_entry_2, entry_del, 
                           sizeof(ext_l2_entry_2_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2;
                break;
            default: assert(0);
            }
            (void)_bcm_tr3_l2api_from_l2hw(unit, &l2addr_del, &l2_entries);
        }
        if (entry_add != NULL) {
            l2_entries.entry_flags = 0;
            switch (mem_type) {
            case L2_ENTRY_1m:
                sal_memcpy(&l2_entries.l2_entry_1, entry_add, 
                           sizeof(l2_entry_1_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_L2_ENTRY_1;
                break;
            case L2_ENTRY_2m:
                sal_memcpy(&l2_entries.l2_entry_2, entry_add, 
                           sizeof(l2_entry_2_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_L2_ENTRY_2;
                break;
            case EXT_L2_ENTRY_1m:
                sal_memcpy(&l2_entries.ext_l2_entry_1, entry_add, 
                           sizeof(ext_l2_entry_1_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1;
                break;
            case EXT_L2_ENTRY_2m:
                sal_memcpy(&l2_entries.ext_l2_entry_2, entry_add, 
                           sizeof(ext_l2_entry_2_entry_t));
                l2_entries.entry_flags |= _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2;
                break;
            default: assert(0);
            }
            (void)_bcm_tr3_l2api_from_l2hw(unit, &l2addr_add, &l2_entries);
        }

        if ((entry_del != NULL) && (entry_add != NULL)) { /* It's a move */
            set_flags |= BCM_L2_MOVE;
            if (SOC_USE_GPORT(unit)) {
                if (l2addr_del.port != l2addr_add.port) {
                    set_flags |= BCM_L2_MOVE_PORT;
                }
            } else {
                if (l2addr_del.modid != l2addr_add.modid ||
                    l2addr_del.tgid != l2addr_add.tgid ||
                    l2addr_del.port != l2addr_add.port) {
                    set_flags |= BCM_L2_MOVE_PORT;
                }
            }
            if (!(l2addr_del.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tr3_l2_port_native(unit, l2addr_del.modid,
                                           l2addr_del.port) > 0) {
                    set_flags |= BCM_L2_FROM_NATIVE;
                    set_flags |= BCM_L2_NATIVE;
                }
            }
            if (!(l2addr_add.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tr3_l2_port_native(unit, l2addr_add.modid,
                                           l2addr_add.port) > 0) {
                    set_flags |= BCM_L2_TO_NATIVE;
                    set_flags |= BCM_L2_NATIVE;
                }
            }
            l2addr_del.flags |= set_flags;
            l2addr_add.flags |= set_flags;
        } else if (entry_del != NULL) { /* Age out or simple delete */
            if (!(l2addr_del.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tr3_l2_port_native(unit, l2addr_del.modid,
                                            l2addr_del.port) > 0) {
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
            }
        } else if (entry_add != NULL) { /* Insert or learn */
            if (!(l2addr_add.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_tr3_l2_port_native(unit, l2addr_add.modid,
                                           l2addr_add.port) > 0) {
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
            }
            if (flags & SOC_L2X_ENTRY_OVERFLOW) {
                l2addr_add.flags |= BCM_L2_ENTRY_OVERFLOW;
            }
        }

        /* The entries are now set up.  Make the callbacks */
        if (entry_del != NULL) {
            _bcm_tr3_l2_cbs[unit](unit, &l2addr_del, BCM_L2_CALLBACK_DELETE,
                                  _bcm_tr3_l2_cb_data[unit]);
        }
        if (entry_add != NULL) {
            _bcm_tr3_l2_cbs[unit](unit, &l2addr_add, BCM_L2_CALLBACK_ADD,
                                  _bcm_tr3_l2_cb_data[unit]);
        }
    }
}

/*
 * Function:
 *      _bcm_tr3_l2_hw_init
 * Purpose:
 *      Initialize the BCM L2 HW subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_tr3_l2_hw_init(int unit)
{
    int         rv, lclass, was_running = FALSE;
    uint32      flags;
    sal_usecs_t interval;

    if (soc_l2x_running(unit, &flags, &interval)) { 
        was_running = TRUE; 
        BCM_IF_ERROR_RETURN(soc_tr3_l2x_stop(unit)); 
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        if (!(SAL_BOOT_QUICKTURN || SAL_BOOT_SIMULATION ||
              SAL_BOOT_BCMSIM)) {
            (void)_tr3_l2x_delete_all(unit);

            /*
             * Init class based learning
             */
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, PORT_CBL_TABLEm, 
                                              MEM_BLOCK_ALL, 0));
            for (lclass = 0; lclass < SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr);
                 lclass++) {
                BCM_IF_ERROR_RETURN(WRITE_CBL_ATTRIBUTEr(unit, lclass, 0));
            }
        }
    }

    

    rv = _bcm_tr3_l2_mbi_init(unit);
    if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
        return rv;
    }

    /* bcm_l2_register clients */
    soc_l2_entry_register(unit, _bcm_tr3_l2_register_callback, NULL);

    if (was_running) {
        interval = (SAL_BOOT_BCMSIM) ? BCMSIM_L2XMSG_INTERVAL : interval;
        soc_l2x_start(unit, flags, interval);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_cache_gport_resolve
 * Purpose:
 *      Convert destination in a GPORT format to modid port pair or trunk id
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (IN/OUT) Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr3_l2_cache_gport_resolve(int unit, bcm_l2_cache_addr_t *l2caddr)
{
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             id;

    if (BCM_GPORT_IS_SET(l2caddr->dest_port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, l2caddr->dest_port, 
                                                   &modid, &port, &tgid, &id));
        /* MPLS MIM and WLAN GPORTS are not currently supported. */
        if (-1 != id) {
            return (BCM_E_UNAVAIL);
        }
        /* Trunk GPORT */
        if (BCM_TRUNK_INVALID != tgid) {
            l2caddr->flags |= BCM_L2_CACHE_TRUNK;
            l2caddr->dest_trunk = tgid;
        } else {    /* MODPORT GPORT */
            l2caddr->dest_port = port;
            l2caddr->dest_modid = modid;
        }
    }

    if (BCM_GPORT_IS_SET(l2caddr->src_port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, l2caddr->src_port, 
                                                   &port));
        l2caddr->src_port = port;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_cache_gport_construct
 * Purpose:
 *      Constract a GPORT destination format from modid port pair or trunk id
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (IN/OUT) Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr3_l2_cache_gport_construct(int unit, bcm_l2_cache_addr_t *l2caddr)
{
    bcm_gport_t         gport;
    _bcm_gport_dest_t   gport_dst;

     _bcm_gport_dest_t_init(&gport_dst);

     if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
         gport_dst.gport_type = BCM_GPORT_TYPE_TRUNK;
         gport_dst.tgid = l2caddr->dest_trunk;
     } else {
         gport_dst.gport_type = BCM_GPORT_TYPE_MODPORT;
         gport_dst.modid = l2caddr->dest_modid;
         gport_dst.port = l2caddr->dest_port;
     }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_construct(unit, &gport_dst, &gport));

    l2caddr->dest_port = gport;

    gport_dst.gport_type = BCM_GPORT_TYPE_MODPORT;
    gport_dst.port = l2caddr->src_port;
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &gport_dst.modid));
    if (gport_dst.modid < 0) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_construct(unit, &gport_dst, &gport));

    l2caddr->src_port = gport;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_cache_to_l2u
 * Purpose:
 *      Convert a hardware-independent L2 cache entry to a L2 User table entry.
 * Parameters:
 *      unit - Unit number
 *      l2u_entry - (OUT) Firebolt L2 User entry
 *      l2caddr - Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_cache_to_l2u(int unit, 
                     l2u_entry_t *l2u_entry, bcm_l2_cache_addr_t *l2caddr)
{
    bcm_module_t    mod_in, mod_out;
    bcm_port_t      port_in, port_out;
    uint16          vfi;
    uint32          mask[2];
    soc_field_t     port_field = 0;
    int             skip_l2u, isGport, modid_field_len, port_field_len;
    bcm_vlan_t      vlan;
    int             field_len;      /* Num of bits in field */
    l2u_entry_t     l2u_mask_entry; /* Dummy entry to set the mask field
                                       generically */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
    
    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        int pri_field_len;
        int max_pri;
        /* If VPN specified do not perform vlan id check */
        if (!_BCM_VPN_IS_SET(l2caddr->vlan)) {
            VLAN_CHK_ID(unit, l2caddr->vlan);
        }

        if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
            pri_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm, PRIf);
            max_pri = (1 << pri_field_len) - 1;

            /* Allow the maximum internal priority as per the device support */
            if ((l2caddr->prio < BCM_PRIO_MIN) || (l2caddr->prio > max_pri)) {
                    return BCM_E_PARAM;
            }
        }

        sal_memset(l2u_entry, 0, sizeof (*l2u_entry));
        sal_memset(&l2u_mask_entry, 0, sizeof(l2u_entry_t));

        _l2u_field32_set(unit, l2u_entry, VALIDf, 1);
        _l2u_mac_addr_set(unit, l2u_entry, MAC_ADDRf, l2caddr->mac);
        _l2u_mac_addr_set(unit, &l2u_mask_entry, MAC_ADDRf, l2caddr->mac_mask);

        if (_BCM_VPN_IS_SET(l2caddr->vlan)) {
            _l2u_field32_set(unit, l2u_entry, KEY_TYPEf, 1);
            _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, l2caddr->vlan);
            _l2u_field32_set(unit, l2u_entry, VFIf, vfi);
            field_len =  soc_mem_field_length(unit, L2_USER_ENTRYm, VFIf);
            _l2u_field32_set(unit, &l2u_mask_entry, VFIf,
                             (((1U << field_len) - 1) & l2caddr->vlan_mask));

            if (l2caddr->flags & BCM_L2_CACHE_PROTO_PKT) {
                if (soc_mem_field_valid(unit, L2_USER_ENTRYm, 
                                        L2_PROTOCOL_PKTf)) {
                    _l2u_field32_set(unit, l2u_entry, L2_PROTOCOL_PKTf, 1);
                } else {
                  return BCM_E_UNAVAIL;
                }
            }

        } else {
            _l2u_field32_set(unit, l2u_entry, KEY_TYPEf, 0);
            vlan = l2caddr->vlan;
            _l2u_field32_set(unit, l2u_entry, VLAN_IDf, vlan);
            _l2u_field32_set(unit, &l2u_mask_entry, VLAN_IDf,
                             (0xFFF & l2caddr->vlan_mask));
        }

        /* Key Type should always be set in Mask */
        _l2u_field32_set(unit, &l2u_mask_entry, KEY_TYPEf, 1);

        if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
            _l2u_field32_set(unit, l2u_entry, PRIf, l2caddr->prio);
            _l2u_field32_set(unit, l2u_entry, RPEf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_CPU) {
            _l2u_field32_set(unit, l2u_entry, CPUf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_BPDU) {
            _l2u_field32_set(unit, l2u_entry, BPDUf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
            _l2u_field32_set(unit, l2u_entry, DST_DISCARDf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_LEARN_DISABLE) {
            _l2u_field32_set(unit, l2u_entry, DO_NOT_LEARN_MACSAf, 1);
        }

        if (BCM_GPORT_IS_SET(l2caddr->dest_port) || 
            BCM_GPORT_IS_SET(l2caddr->src_port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr3_l2_cache_gport_resolve(unit, l2caddr));
            isGport = 1;
        } else {
            isGport = 0;
        }
        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
                _l2u_field32_set(unit, l2u_entry, Tf, 1);
                _l2u_field32_set(unit, l2u_entry, TGIDf, l2caddr->dest_trunk);
            } else {
                port_field = PORT_NUMf;
            }
        } else {
            if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
                _l2u_field32_set(unit, l2u_entry, MODULE_IDf,
                                 BCM_TRUNK_TO_MODIDf(unit,
                                                     l2caddr->dest_trunk));
                _l2u_field32_set(unit, l2u_entry, PORT_TGIDf,
                                 BCM_TRUNK_TO_TGIDf(unit,
                                                    l2caddr->dest_trunk));
            } else {
                port_field = PORT_TGIDf;
            }
        }

        if (!((l2caddr->flags & BCM_L2_CACHE_TRUNK) ||
             (l2caddr->flags & BCM_L2_CACHE_MULTICAST))) {
            mod_in = l2caddr->dest_modid;
            port_in = l2caddr->dest_port;
            if (!isGport) {
                PORT_DUALMODID_VALID(unit, port_in);
            }
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                         mod_in, port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return BCM_E_PORT;
            }
            _l2u_field32_set(unit, l2u_entry, MODULE_IDf, mod_out);
            _l2u_field32_set(unit, l2u_entry, port_field, port_out);
        }

        if ((l2caddr->flags & BCM_L2_CACHE_MULTICAST) &&
            !_BCM_VPN_IS_SET(l2caddr->vlan)) {
            if (_BCM_MULTICAST_IS_SET(l2caddr->group)) {
                if (_BCM_MULTICAST_IS_L2(l2caddr->group)) {
                    port_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                          port_field);
                    modid_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                           MODULE_IDf);
                    /* If MACDA is MC, HW interprets value encoded in MODULE_IDf
                       and PORT_FIELD as L2MC_INDEX */
                    _l2u_field32_set(unit, l2u_entry, port_field,
                                     (_BCM_MULTICAST_ID_GET(l2caddr->group) &
                                     ((1 << port_field_len) - 1)));
                    _l2u_field32_set(unit, l2u_entry, MODULE_IDf,
                                     ((_BCM_MULTICAST_ID_GET(l2caddr->group) >>
                                     port_field_len) &
                                     ((1 << modid_field_len) - 1)));
                } else {
                    return BCM_E_PARAM;
                }
            }
        }

        if (l2caddr->flags & BCM_L2_CACHE_L3) {
            _l2u_field32_set(unit, l2u_entry, RESERVED_0f, 1);
        }

        if (!(l2caddr->flags & BCM_L2_CACHE_L3)) {
            if ((l2caddr->src_port) || (l2caddr->src_port_mask) ) {
                return BCM_E_PORT;
            }
        }

        /* Set the Mask field */
        _l2u_field_get(unit, &l2u_mask_entry, KEYf, mask);
        _l2u_field_set(unit, l2u_entry, MASKf, mask);

        _l2u_field32_set(unit, l2u_entry, CLASS_IDf, l2caddr->lookup_class);

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_tr3_l2_from_l2u
 * Purpose:
 *      Convert L2 User table entry to a hardware-independent L2 cache entry.
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (OUT) Hardware-independent L2 entry
 *      l2u_entry - Firebolt L2 User entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_cache_from_l2u(int unit, 
                       bcm_l2_cache_addr_t *l2caddr, l2u_entry_t *l2u_entry)
{
    bcm_module_t    mod_in = 0, mod_out;
    bcm_port_t      port_in = 0, port_out;
    uint32          mask[2];
    int             skip_l2u, isGport, port_field_len, modid_field_len, rval;
#if defined(INCLUDE_L3)
    bcm_vlan_t      vlan;
#endif

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        sal_memset(l2caddr, 0, sizeof (*l2caddr));

        if (!_l2u_field32_get(unit, l2u_entry, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        _l2u_mac_addr_get(unit, l2u_entry, MAC_ADDRf, l2caddr->mac);

        if (_l2u_field32_get(unit, l2u_entry, KEY_TYPEf)) {
#if defined(INCLUDE_L3)
            /* key type 1 i.e. VFI */
            vlan = _l2u_field32_get(unit, l2u_entry, VFIf);
            if (_bcm_vfi_used_get(unit, vlan, _bcmVfiTypeMpls)) {
                _BCM_VPN_SET(l2caddr->vlan, _BCM_VPN_TYPE_VFI, vlan);
            } else  if (_bcm_vfi_used_get(unit, vlan, _bcmVfiTypeMim)) {
                _BCM_VPN_SET(l2caddr->vlan, _BCM_VPN_TYPE_VFI, vlan);
            } else  if (_bcm_vfi_used_get(unit, vlan, _bcmVfiTypeL2Gre)) {
                BCM_IF_ERROR_RETURN(
                   _bcm_tr3_l2gre_vpn_get(unit, (int) vlan, &l2caddr->vlan));
            }

#endif
        } else { /* key type 0 i.e. VLAN ID */
            l2caddr->vlan = _l2u_field32_get(unit, l2u_entry, VLAN_IDf);
        }

        l2caddr->prio = _l2u_field32_get(unit, l2u_entry, PRIf);

        if (_l2u_field32_get(unit, l2u_entry, RPEf)) {
            l2caddr->flags |= BCM_L2_CACHE_SETPRI;
        }

        if (_l2u_field32_get(unit, l2u_entry, CPUf)) {
            l2caddr->flags |= BCM_L2_CACHE_CPU;
        }

        if (_l2u_field32_get(unit, l2u_entry, BPDUf)) {
            l2caddr->flags |= BCM_L2_CACHE_BPDU;
        }

        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, L2_PROTOCOL_PKTf)) {
            if (_l2u_field32_get(unit, l2u_entry, L2_PROTOCOL_PKTf)) {
                l2caddr->flags |= BCM_L2_CACHE_PROTO_PKT;
            }
        }

        if (_l2u_field32_get(unit, l2u_entry, DST_DISCARDf)) {
            l2caddr->flags |= BCM_L2_CACHE_DISCARD;
        }

        if (_l2u_field32_get(unit, l2u_entry, DO_NOT_LEARN_MACSAf)) {
            l2caddr->flags |= BCM_L2_CACHE_LEARN_DISABLE;
        }

        if (_l2u_field32_get(unit, l2u_entry, Tf)) {
            l2caddr->flags |= BCM_L2_CACHE_TRUNK;
            l2caddr->dest_trunk = _l2u_field32_get(unit, l2u_entry, TGIDf);
        }

        if (BCM_MAC_IS_MCAST(l2caddr->mac) &&
            !_BCM_VPN_IS_SET(l2caddr->vlan)) {
            l2caddr->flags |= BCM_L2_CACHE_MULTICAST;
            mod_in = _l2u_field32_get(unit, l2u_entry, MODULE_IDf);
            port_in = _l2u_field32_get(unit, l2u_entry, PORT_NUMf);
            l2caddr->dest_modid = mod_in;
            l2caddr->dest_port = port_in;
            port_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                  PORT_NUMf);
            modid_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                   MODULE_IDf);
            /* If MACDA is MC, HW interprets value encoded in MODULE_IDf
               and PORT_FIELD as L2MC_INDEX */
            l2caddr->group = ((port_in & ((1 << port_field_len) - 1)) |
                              ((mod_in & ((1 << modid_field_len) - 1))
                              << port_field_len));
            /* Translate l2mc index */
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType,
                                           &rval));
            if (rval) {
                _BCM_MULTICAST_GROUP_SET(l2caddr->group, _BCM_MULTICAST_TYPE_L2,
                                         l2caddr->group);
            }
        }

        if (!((l2caddr->flags & BCM_L2_CACHE_TRUNK) ||
            (l2caddr->flags & BCM_L2_CACHE_MULTICAST))) {
            mod_in = _l2u_field32_get(unit, l2u_entry, MODULE_IDf);
            port_in = _l2u_field32_get(unit, l2u_entry, PORT_NUMf);

            if (!SOC_MODID_ADDRESSABLE(unit, mod_in)) {
                return BCM_E_BADID;
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                       mod_in, port_in, &mod_out,
                                                       &port_out));
            l2caddr->dest_modid = mod_out;
            l2caddr->dest_port = port_out;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

        if (_l2u_field32_get(unit, l2u_entry, RESERVED_0f)) {
            l2caddr->flags |= BCM_L2_CACHE_L3;
        }

        if (isGport) {
            BCM_IF_ERROR_RETURN(
                _bcm_tr3_l2_cache_gport_construct(unit, l2caddr));
        }

        _l2u_mac_addr_get(unit, l2u_entry, MASKf, l2caddr->mac_mask);
        _l2u_field_get(unit, l2u_entry, MASKf, mask);
        l2caddr->vlan_mask = (mask[1] >> 16) & 0xfff;
        l2caddr->lookup_class = _l2u_field32_get(unit, l2u_entry, CLASS_IDf);

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_tr3_l2cache_to_my_station
 * Purpose:
 *      Convert an L2 API data structure to a MY_STATION_TCAM entry
 * Parameters:
 *      unit              Unit number
 *      my_station_entry  (OUT) MY_STATION_TCAM entry
 *      l2caddr            L2 API data structure
 */
STATIC void
_bcm_tr3_l2cache_to_my_station(int unit, 
                              my_station_tcam_entry_t *my_station_entry,
                              bcm_l2_cache_addr_t *l2caddr)
{

    soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry, VALIDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry, VLAN_IDf,
                        l2caddr->vlan);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry, VLAN_ID_MASKf,
                        l2caddr->vlan_mask);
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, my_station_entry, MAC_ADDRf,
                         l2caddr->mac);
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, my_station_entry, MAC_ADDR_MASKf,
                         l2caddr->mac_mask);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry, ING_PORT_NUMf,
                        l2caddr->src_port);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry, 
                        ING_PORT_NUM_MASKf, l2caddr->src_port_mask);

    if (l2caddr->flags & BCM_L2_CACHE_L3) {
        soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry,
                            IPV4_TERMINATION_ALLOWEDf, 1);
        soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry,
                            IPV6_TERMINATION_ALLOWEDf, 1);
        soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry,
                            ARP_RARP_TERMINATION_ALLOWEDf, 1);
    }

    if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
        soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry,
                            DISCARDf, 1);
    }
    if (l2caddr->flags & BCM_L2_CACHE_CPU) {
        soc_mem_field32_set(unit, MY_STATION_TCAMm, my_station_entry,
                            COPY_TO_CPUf, 1);
    }
}

/*
 * Function:
 *      _bcm_tr3_l2cache_from_my_station
 * Purpose:
 *      Convert a MY_STATION_TCAM entry to an L2cache API data structure
 * Parameters:
 *      unit              (IN) Unit number
 *      l2caddr            (OUT) L2cache API data structure
 *      my_station_entry  MY_STATION_TCAM entry
 */
STATIC void
_bcm_tr3_l2cache_from_my_station(int unit, bcm_l2_cache_addr_t *l2caddr,
                                my_station_tcam_entry_t *my_station_entry)
{
    sal_memset(l2caddr, 0, sizeof(*l2caddr));

    /* Valid bit is ignored here; entry is assumed valid */

    soc_mem_mac_addr_get(unit, MY_STATION_TCAMm, (uint32 *)my_station_entry,
                         MAC_ADDRf, l2caddr->mac);
    soc_mem_mac_addr_get(unit, MY_STATION_TCAMm, (uint32 *)my_station_entry,
                         MAC_ADDR_MASKf, l2caddr->mac_mask);
    l2caddr->vlan = soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                                       VLAN_IDf);
    l2caddr->vlan_mask = soc_mem_field32_get(unit, MY_STATION_TCAMm, 
                                            my_station_entry, VLAN_ID_MASKf);

    l2caddr->src_port = soc_mem_field32_get(unit, MY_STATION_TCAMm,
                                           my_station_entry, ING_PORT_NUMf);

    l2caddr->src_port_mask = soc_mem_field32_get(unit, MY_STATION_TCAMm,
                              my_station_entry, ING_PORT_NUM_MASKf);

    l2caddr->flags |= BCM_L2_CACHE_L3;
    if (soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                            DISCARDf)) {
        l2caddr->flags |= BCM_L2_CACHE_DISCARD;
    }
    if (soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                            COPY_TO_CPUf)) {
        l2caddr->flags |= BCM_L2_CACHE_CPU;
    }
}

/*
 * Function:
 *     _bcm_tr3_my_station_lookup
 * Purpose:
 *     Perform following exact matched in MY_STATION_TCAM table:
 *     - target entry lookup:
 *       match the entry specified by vlan/mac and optional port
 *     - (optional) alternate entry lookup: (when alt_index != NULL)
 *       match the possible entry having the same vlan/mac as the target entry
 * Parameters:
 *     unit              Unit number
 *     mac               MAC address key
 *     vlan              VLAN id key
 *     port              Source port number key (-1 means to match any)
 *     index_to_be_skip  The index to be omit during lookup
 *     entry_index       (OUT) target entry index (if BCM_E_NONE)
 *                             available entry index or -1 (if BCM_E_NOT_FOUND)
 *     alt_index         (OUT) alternate entry index or -1 (if BCM_E_NOT_FOUND)
 *                       if alt_index is NULL, that means lookup only
 * Returns:
 *      BCM_E_NONE - target entry found
 *      BCM_E_NOT_FOUND - target entry not found
 * Notes:
 *     if (target entry is found) {
 *         entry_index: index for the matched target entry
 *         alt_index: N/A
 *     } else if (alternate entry is found) {
 *         entry_index: index for the free entry to be used (-1 if table full)
 *         alt_index: index for the matched alternate entry
 *         *** caller needs to compare both indices, move the entry if needed
 *     } else {
 *         entry_index: index for the free entry to be used (-1 if table full)
 *         alt_index: -1
 *     }
 */
STATIC int
_bcm_tr3_my_station_lookup(int unit, bcm_mac_t mac, bcm_vlan_t vlan,
                          bcm_port_t port, int index_to_skip,
                          int *entry_index, int *alt_index)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    my_station_tcam_entry_t *entry, entry0, entry1, entry2, mask0, mask1;
    int index0, index1, free_index, i, entry_words;
    int start, end, step;
    bcm_mac_t mac_mask;
    uint32 port_mask;

    LOG_INFO(BSL_LS_BCM_L2,
             (BSL_META_U(unit,
                         "_bcm_tr3_my_station_lookup: unit=%d "
                         "mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d port=%d "
                         "index_to_skip=%d\n"),
              unit, vlan, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
              port, index_to_skip));

    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
    port_mask =
        (1 << soc_mem_field_length(unit, MY_STATION_TCAMm, ING_PORT_NUMf)) - 1;

    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    /*
     * entry0, mask0 are for target entry
     * entry1, mask1 are for alternate entry
     * if (port == -1)
     *     entry0 is: vlan/mac/00/fff/ffffffffffff/ff
     *     entry1 is: vlan/mac/xx/fff/ffffffffffff/ff
     *     start from last entry (free index needs to have larger index than
     *     any alternate entry)
     * else
     *     entry0 is: vlan/mac/port/fff/ffffffffffff/ff
     *     entry1 is: vlan/mac/00/fff/ffffffffffff/ff
     *     start from first entry (free index needs to have smaller index than
     *     the alternate entry)
     */
    sal_memset(&entry0, 0, sizeof(entry0));
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &entry0, VALIDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &entry0, VLAN_IDf, vlan);
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, &entry0, MAC_ADDRf, mac);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &entry0, VLAN_ID_MASKf, 0xfff);
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, &entry0, MAC_ADDR_MASKf,
                         mac_mask);
    mask0 = entry0;
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &mask0, VLAN_IDf, 0xfff);
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, &mask0, MAC_ADDRf, mac_mask);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &mask0, ING_PORT_NUMf,
                        port_mask);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &mask0, ING_PORT_NUM_MASKf,
                        port_mask);
    if (alt_index != NULL) {
        entry1 = entry0;
        mask1 = mask0;
    }

    if (port == -1) {
        start = soc_mem_index_max(unit, MY_STATION_TCAMm);
        end = -1;
        step = -1;
    } else {
        soc_mem_field32_set(unit, MY_STATION_TCAMm, &entry0, ING_PORT_NUMf,
                            port);
        soc_mem_field32_set(unit, MY_STATION_TCAMm, &entry0,
                            ING_PORT_NUM_MASKf, port_mask);
        start = 0;
        end = soc_mem_index_count(unit, MY_STATION_TCAMm);
        step = 1;
    }

    if (alt_index != NULL) {
        if (port == -1) {
            soc_mem_field32_set(unit, MY_STATION_TCAMm, &mask1, ING_PORT_NUMf,
                                0);
        }

        /* entry2 is for checking VALID bit */
        sal_memset(&entry2, 0, sizeof(entry2));
        soc_mem_field32_set(unit, MY_STATION_TCAMm, &entry2, VALIDf, 1);
    }

    index1 = -1;
    free_index = -1;
    for (index0 = start; index0 != end; index0 += step) {
        /* Skip the entry that we want to move (we know it matches) */
        if (index0 == index_to_skip) {
            continue;
        }

        entry = &info->my_station_shadow[index0];

        /* Try matching target entry */
        for (i = 0; i < entry_words; i++) {
            if ((entry->entry_data[i] ^ entry0.entry_data[i]) &
                mask0.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            /* Target entry is found, no more action needed */
            *entry_index = index0;
            LOG_INFO(BSL_LS_BCM_L2,
                     (BSL_META_U(unit,
                                 "_bcm_tr3_my_station_lookup: found entry_index=%d\n"),
                      *entry_index));
            return BCM_E_NONE;
        }

        if (alt_index == NULL) { /* lookup only */
            continue;
        }

        /* Keep track index of the first free entry */
        if (free_index == -1) {
            for (i = 0; i < entry_words; i++) {
                if (entry->entry_data[i] & entry2.entry_data[i]) {
                    break;
                }
            }
            if (i == entry_words) {
                if (index1 != -1) {
                    /* Both free entry and alternate entry are found */
                    *entry_index = index0;
                    *alt_index = index1;
                    LOG_INFO(BSL_LS_BCM_L2,
                             (BSL_META_U(unit,
                                         "_bcm_tr3_my_station_lookup: not found "
                                         "entry_index=%d alt_index=%d\n"),
                              *entry_index, *alt_index));
                    return BCM_E_NOT_FOUND;
                } else {
                    free_index = index0;
                    continue;
                }
            }
        }

        /* Try matching alternate entry. If alternate entry is found, it
         * implies target entry is not in the table */
        for (i = 0; i < entry_words; i++) {
            if ((entry->entry_data[i] ^ entry1.entry_data[i]) &
                mask1.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            if (free_index != -1) {
                /* both free entry and alternate entry are found */
                *entry_index = free_index;
                *alt_index = index0;
                LOG_INFO(BSL_LS_BCM_L2,
                         (BSL_META_U(unit,
                                     "_bcm_tr3_my_station_lookup: not found "
                                     "entry_index=%d alt_index=%d\n"),
                          *entry_index, *alt_index));
                return BCM_E_NOT_FOUND;
            } else {
                index1 = index0;
                continue;
            }
        }
    }

    *entry_index = free_index;
    if (alt_index != NULL) {
        *alt_index = index1;
    }
    LOG_INFO(BSL_LS_BCM_L2,
             (BSL_META_U(unit,
                         "_bcm_tr3_my_station_lookup: not found "
                         "entry_index=%d alt_index=%d\n"),
              *entry_index, alt_index != NULL ? *alt_index : -100));

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_tr3_l2cache_myStation_set
 * Purpose:
 *     Add MY_STATION_TCAM entry from L2 cache API
 * Parameters:
 *     unit              Unit number
 *     index             Index
 *     l2caddr           L2 cache API data structure
 */
int
_bcm_tr3_l2cache_myStation_set(int unit, int index, bcm_l2_cache_addr_t *l2caddr)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, entry_index, alt_index;
    my_station_tcam_entry_t *entry;
    l2u_entry_t l2u_entry;
    bcm_vlan_t vlan;
    bcm_mac_t mac;
    bcm_port_t port;

    VLAN_CHK_ID(unit, l2caddr->vlan);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    entry_index = -1;
    entry = &info->my_station_shadow[index];
    if (!soc_mem_field32_get(unit, MY_STATION_TCAMm, entry, VALIDf)) {
        /* Entry is not in used */
        entry_index = index;
    } else {
        /* Check if the existing entry is added by l2cache API */
        rv = soc_l2u_get(unit, &l2u_entry, index);
        if (BCM_SUCCESS(rv) &&
            (soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                              RESERVED_0f))) {
            entry_index = index;
        }
    }
    if (entry_index != -1) {
        /* The entry is not used by API other than l2cache */
        sal_memset(entry, 0, sizeof(*entry));
        _bcm_tr3_l2cache_to_my_station(unit, entry, l2caddr);
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           entry);
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return rv;
    }

    /* Need to move the current entry */
    vlan = soc_mem_field32_get(unit, MY_STATION_TCAMm, entry, VLAN_IDf);
    soc_mem_mac_addr_get(unit, MY_STATION_TCAMm, entry, MAC_ADDRf, mac);
    if (soc_mem_field32_get(unit, MY_STATION_TCAMm, entry,
                            ING_PORT_NUM_MASKf) == 0) {
        port = -1;
    } else {
        port = soc_mem_field32_get(unit, MY_STATION_TCAMm, entry,
                                   ING_PORT_NUMf);
    }
    rv = _bcm_tr3_my_station_lookup(unit, mac, vlan, port, index, &entry_index,
                                   &alt_index);
    if (rv == BCM_E_NOT_FOUND && entry_index == -1) {
        /* No free entry available for insertion */
        rv = BCM_E_FULL;
    }
    if (rv != BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return rv;
    }

    if (alt_index != -1 &&
        ((port == -1 && alt_index > entry_index) ||
         (port != -1 && alt_index < entry_index))) {
        /* Free entry is available for moving the target entry and alternate
         * entry exists. However need to swap these 2 entries to maintain
         * proper lookup precedence */
        info->my_station_shadow[entry_index] =
            info->my_station_shadow[alt_index];
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, entry_index,
                           &info->my_station_shadow[entry_index]);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return rv;
        }
        entry_index = alt_index;
    }

    /* Move the entry at specified index to the available free location */
    info->my_station_shadow[entry_index] = *entry;
    rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, entry_index,
                       entry);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return rv;
    }

    /* Add the new entry */
    sal_memset(entry, 0, sizeof(*entry));
    _bcm_tr3_l2cache_to_my_station(unit, entry, l2caddr);
    rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                       entry);

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2cache_myStation_delete
 * Purpose:
 *    Delete l2cache entry from myStation TCAM
 * Parameters:
 *      unit -   [IN] Unit number
 *      index -  [IN] index to a table
 */
STATIC int 
_bcm_tr3_l2cache_myStation_delete(int unit, int index)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, i, entry_words;
    my_station_tcam_entry_t *entry;
    l2u_entry_t l2u_entry;
    uint32 *l3_mask, *tunnel_mask;

    l3_mask = info->my_station_l3_mask.entry_data;
    tunnel_mask = info->my_station_tunnel_mask.entry_data;
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    entry = &info->my_station_shadow[index];

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry->entry_data[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return BCM_E_NOT_FOUND;
    }

    /* Check if the entry is valid */
    if (!soc_mem_field32_get(unit, MY_STATION_TCAMm, entry, VALIDf)) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return BCM_E_NOT_FOUND;
    }

    /* Check if the entry is added by l2cache API */
    rv = soc_l2u_get(unit, &l2u_entry, index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return rv;
    }
    if (!soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                           RESERVED_0f)) {
        return BCM_E_NOT_FOUND;
    }
    /* Check to see if we need to delete or modify the entry */
    for (i = 0; i < entry_words; i++) {
        if (entry->entry_data[i] & tunnel_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        /* Delete the entry if no tunnel related flag is enabled */
        sal_memset(entry, 0, sizeof(*entry));
    } else {
        /* Modify the entry if any tunnel related flag is enabled */
        for (i = 0; i < entry_words; i++) {
            entry->entry_data[i] &= ~l3_mask[i];
        }
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, DISCARDf, 0);
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, COPY_TO_CPUf, 0);
    }
    rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index, entry);

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2cache_myStation_get
 * Purpose:
 *      Get l2cache entry for L2 processing.
 * Parameters:
 *      unit -          [IN] Unit number
 *      index -         [IN] index to a table
 *      l2addr -        [OUT] pointer to l2_cache structure to fill
 */
STATIC int 
_bcm_tr3_l2cache_myStation_get(int unit, int index, bcm_l2_cache_addr_t *l2caddr)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int i, entry_words;
    my_station_tcam_entry_t *entry;
    l2u_entry_t l2u_entry;
    uint32 *l3_mask;

    l3_mask = info->my_station_l3_mask.entry_data;
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    entry = &info->my_station_shadow[index];

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry->entry_data[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        return BCM_E_NOT_FOUND;
    }

    /* Check if the entry is valid */
    if (!soc_mem_field32_get(unit, MY_STATION_TCAMm, entry, VALIDf)) {
        return BCM_E_NOT_FOUND;
    }

    /* Check if the entry is added by l2cache API */
    SOC_IF_ERROR_RETURN(soc_l2u_get(unit, &l2u_entry, index));
    if (! soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,    
                           RESERVED_0f)) {
        return BCM_E_NOT_FOUND;
    }

    _bcm_tr3_l2cache_from_my_station(unit, l2caddr, entry);

    return BCM_E_NONE;

}


/*
 * Function:
 *      _bcm_tr3_l2_learn_limit_system_get
 * Purpose:
 *      Get MAC learn limit for a system
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask
 *      limit - System limit of MAC addresses to learn
 *              Negative if limit disabled
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_learn_limit_system_get(int unit, uint32 *flags, int *limit)
{
    uint32  rval;

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    *flags &= ~(BCM_L2_LEARN_LIMIT_ACTION_DROP |
                BCM_L2_LEARN_LIMIT_ACTION_CPU);
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                          SYS_OVER_LIMIT_TOCPUf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                          SYS_OVER_LIMIT_DROPf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    /*
     * When MAC limiting is enabled, ENABLE_INTERNAL_L2_ENTRYf
     * is always set. So only checking for this field
     * to retrieve MAC limit value
     */
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                           ENABLE_INTERNAL_L2_ENTRYf)) {
        *limit = soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                                   SYS_LIMITf);
    } else {
        *limit = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_learn_limit_get
 * Purpose:
 *      Get MAC learn limit for a port/trunk/vlan
 * Parameters:
 *      unit  - BCM unit number
 *      mem   - PORT_OR_TRUNK_MAC_LIMITm or VLAN_OR_VFI_MAC_LIMITm
 *      index - for PORT_OR_TRUNK_MAC_LIMITm
 *                  0 - 127: trunk identifier
 *                  128 - 181: port identifier + 128
 *              for VLAN_OR_VFI_MAC_LIMITm
 *                  0 - 4095: VLAN identifier
 *                  4096 - 5119: virtual forwarding instance + 4096
 *      flags - Action bitmask
 *      limit - Max number of MAC addresses can be learned for the specified
 *              identifier
 *              Negative if limit disabled (i.e. unlimit)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_learn_limit_get(int unit, soc_mem_t mem, int index, uint32* flags,
                           int* limit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));
    *flags &= ~(BCM_L2_LEARN_LIMIT_ACTION_DROP |
                BCM_L2_LEARN_LIMIT_ACTION_CPU);
    if (soc_mem_field32_get(unit, mem, &entry, OVER_LIMIT_DROPf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    if (soc_mem_field32_get(unit, mem, &entry, OVER_LIMIT_TOCPUf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }
    *limit = soc_mem_field32_get(unit, mem, &entry, LIMITf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tr3_l2_learn_limit_set
 * Purpose:
 *      Set MAC learn limit for a port/trunk/vlan
 * Parameters:
 *      unit  - BCM unit number
 *      mem   - PORT_OR_TRUNK_MAC_LIMITm or VLAN_OR_VFI_MAC_LIMITm
 *      index - for PORT_OR_TRUNK_MAC_LIMITm
 *                  0 - 127: trunk identifier
 *                  128 - 181: port identifier + 128
 *              for VLAN_OR_VFI_MAC_LIMITm
 *                  0 - 4095: VLAN identifier
 *                  4096 - 5119: virtual forwarding instance + 4096
 *      flags - Action bitmask
 *      limit - Max number of MAC addresses can be learned for the specified
 *              identifier
 *              Negative if limit disabled (i.e. unlimit)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_learn_limit_set(int unit, soc_mem_t mem, int index, uint32 flags,
                           int limit)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    int rv;
    port_or_trunk_mac_limit_entry_t init_entry;
    int32 max_limit;

    if (limit < 0) {
        soc_mem_lock(unit, mem);
        sal_memcpy(&init_entry, soc_mem_entry_null(unit,mem),
                                         sizeof(init_entry));

        /* Reset Limit value is different than soc_mem_entry_null
         * for devices with ESM */
        max_limit = BCM_MAC_LIMIT_MAX(unit);

        soc_mem_field32_set(unit, mem, &init_entry, LIMITf, max_limit);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &init_entry);

        soc_mem_unlock(unit, mem);
        return rv;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, mem, &entry, OVER_LIMIT_TOCPUf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0);
        soc_mem_field32_set(unit, mem, &entry, OVER_LIMIT_DROPf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0);
        soc_mem_field32_set(unit, mem, &entry, LIMITf, limit);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry);
    }
    soc_mem_unlock(unit, mem);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Registers RSEL1_MISC_CONTROL and SYS_MAC_LIMIT_CONTROL are 
     * identical in config. Enable if any of Port, Trunk, Vlan, 
     * VFI, or system learn limits are enabled, disable only when 
     * system limit is disabled */

    BCM_IF_ERROR_RETURN(READ_RSEL1_MISC_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, RSEL1_MISC_CONTROLr, &rval,
                      MAC_LIMIT_ENABLEf, 1); 
    BCM_IF_ERROR_RETURN(WRITE_RSEL1_MISC_CONTROLr(unit, rval));

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));

    /*
     * Enable MAC limiting for internal L2 and also for external L2
     * if it is available
     */
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, 
                      ENABLE_INTERNAL_L2_ENTRYf, 1);
    if (soc_feature(unit, soc_feature_esm_support)) {
        if ((SOC_MEM_IS_VALID(unit, EXT_L2_ENTRY_1m) &&
             (0 < soc_mem_index_count(unit, EXT_L2_ENTRY_1m))) ||
            ((SOC_MEM_IS_VALID(unit, EXT_L2_ENTRY_2m) &&
             (0 < soc_mem_index_count(unit, EXT_L2_ENTRY_2m))))) {
            soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval,
                    ENABLE_EXTERNAL_L2_ENTRYf, 1);
        }
    }
    return (WRITE_SYS_MAC_LIMIT_CONTROLr(unit, rval));
}

/*
 * Function:
 *      _bcm_tr3_l2_to_my_station
 * Purpose:
 *      Convert an L2 API data structure to a Triumph3 MY_STATION_TCAM entry
 * Parameters:
 *      unit              Unit number
 *      entry            (OUT) MY_STATION_TCAM entry
 *      l2addr            L2 API data structure
 */
STATIC void
_bcm_tr3_l2_to_my_station(int unit, my_station_tcam_entry_t *entry,
                         bcm_l2_addr_t *l2addr, int incl_key_mask)
{
    bcm_mac_t mac_mask;
    uint32 fval;

    if (incl_key_mask) {
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, VALIDf, 1);
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, VLAN_IDf,
                            l2addr->vid);
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, VLAN_ID_MASKf, 0xfff);
        soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, entry, MAC_ADDRf,
                             l2addr->mac);
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, entry, MAC_ADDR_MASKf,
                              mac_mask);
    }

    fval = l2addr->flags & BCM_L2_L3LOOKUP ? 1: 0;
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry,
              IPV4_TERMINATION_ALLOWEDf, fval);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry,
              IPV6_TERMINATION_ALLOWEDf, fval);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry,
              ARP_RARP_TERMINATION_ALLOWEDf, fval);

    fval = l2addr->flags & BCM_L2_DISCARD_DST ? 1 : 0;
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, DISCARDf, fval);
    fval = l2addr->flags & BCM_L2_COPY_TO_CPU ? 1 : 0;
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, COPY_TO_CPUf, fval);
}

/*
 * Function:
 *      _bcm_tr3_l2_from_my_station
 * Purpose:
 *      Convert a Triumph3 MY_STATION_TCAM entry to an L2 API data structure
 * Parameters:
 *      unit              Unit number
 *      l2addr            (OUT) L2 API data structure
 *      my_station_entry  MY_STATION_TCAM entry
 */
STATIC void
_bcm_tr3_l2_from_my_station(int unit, bcm_l2_addr_t *l2addr,
                           my_station_tcam_entry_t *my_station_entry)
{
    sal_memset(l2addr, 0, sizeof(*l2addr));

    /* Valid bit is ignored here; entry is assumed valid */

    soc_mem_mac_addr_get(unit, MY_STATION_TCAMm, (uint32 *)my_station_entry,
                         MAC_ADDRf, l2addr->mac);
    l2addr->vid = soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                                      VLAN_IDf);

    if (soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                            IPV4_TERMINATION_ALLOWEDf)) {
        l2addr->flags |= BCM_L2_L3LOOKUP;
    }

    if (soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                            DISCARDf)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }
    if (soc_mem_field32_get(unit, MY_STATION_TCAMm, my_station_entry,
                            COPY_TO_CPUf)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }
    l2addr->flags |= BCM_L2_LEARN_LIMIT_EXEMPT;
}

/*
 * Function:
 *      bcm_tr3_l2_myStation_get
 * Purpose:
 *      Get (MAC, VLAN) entry for L2 processing.
 *      Frames destined to (MAC, VLAN) is subjected to L2/L3 processing.
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
_bcm_tr3_l2_myStation_get (int unit, bcm_mac_t mac, bcm_vlan_t vid,
                         bcm_l2_addr_t *l2addr)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int index, i, entry_words;
    my_station_tcam_entry_t *entry;
    l2u_entry_t l2u_entry;
    uint32 *l3_mask;

    /* Handle VFIs as well as invalid vlans */
    if (vid > BCM_VLAN_MAX) {
        return BCM_E_NOT_FOUND; 
    }

    l3_mask = info->my_station_l3_mask.entry_data;
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    BCM_IF_ERROR_RETURN
        (_bcm_tr3_my_station_lookup(unit, mac, vid, -1, -1, &index, NULL));

    entry = &info->my_station_shadow[index];

    /* Check if the entry has L3 related flag enabled */
    for (i = 0; i < entry_words; i++) {
        if (entry->entry_data[i] & l3_mask[i]) {
            break;
        }
    }
    if (i == entry_words) {
        return BCM_E_NOT_FOUND;
    }

    /* Check if the entry is not added by l2cache API */
    SOC_IF_ERROR_RETURN(soc_l2u_get(unit, &l2u_entry, index));
    if (soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                         RESERVED_0f)) {
        return BCM_E_NOT_FOUND;
    }

    _bcm_tr3_l2_from_my_station(unit, l2addr, entry);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_myStation_add
 * Purpose:
 *      Add l2 entry for L3 processing.
 * Parameters:
 *      unit -         [IN] Unit number
 *      l2addr -       [IN] pointer to l2_addr structure 
 */
STATIC int
_bcm_tr3_l2_myStation_add(int unit, bcm_l2_addr_t *l2addr)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, index, alt_index;
    my_station_tcam_entry_t *entry;
    l2u_entry_t l2u_entry;

    VLAN_CHK_ID(unit, l2addr->vid);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    rv = _bcm_tr3_my_station_lookup(unit, l2addr->mac, l2addr->vid, -1, -1,
                                   &index, &alt_index);
    if (BCM_SUCCESS(rv)) {
        /* Check if the entry is not added by l2cache API */
        rv = soc_l2u_get(unit, &l2u_entry, index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return rv;
        }
        if (soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                             RESERVED_0f)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return BCM_E_EXISTS;
        }
        /* Entry exist, update the entry */
        entry = &info->my_station_shadow[index];
        _bcm_tr3_l2_to_my_station(unit, entry, l2addr, FALSE);
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           entry);
    } else if (rv == BCM_E_NOT_FOUND && index == -1) {
        /* No free entry available for insertion */
        rv = BCM_E_FULL;
    }
    if (rv != BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return rv;
    }

    if (alt_index > index) {
        /* Free entry is available for new entry insertion and alternate entry
         * exists. However need to swap these 2 entries to maintain proper
         * lookup precedence */
        info->my_station_shadow[index] = info->my_station_shadow[alt_index];
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           &info->my_station_shadow[index]);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return rv;
        }
        index = alt_index;
    }

    /* Add the new entry */
    entry = &info->my_station_shadow[index];
    sal_memset(entry, 0, sizeof(*entry));
    _bcm_tr3_l2_to_my_station(unit, entry, l2addr, TRUE);
    rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index, entry);

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2_myStation_delete
 * Purpose:
 *      Delete an l2 entry for L3 processing.
 * Parameters:
 *      unit - (IN) Unit number
 *      mac  - (IN) MAC address to delete
 *      vid  - (IN) VLAN id
 *      index_used -   (OUT) index of entry that was deleted
 */
STATIC int
_bcm_tr3_l2_myStation_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid,
                                  int *index_used)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, index, i, entry_words;
    my_station_tcam_entry_t *entry;
    l2u_entry_t l2u_entry;
    uint32 *l3_mask, *tunnel_mask;

    VLAN_CHK_ID(unit, vid);

    l3_mask = info->my_station_l3_mask.entry_data;
    tunnel_mask = info->my_station_tunnel_mask.entry_data;
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    rv = _bcm_tr3_my_station_lookup(unit, mac, vid, -1, -1, &index, NULL);
    if (BCM_SUCCESS(rv)) {
        entry = &info->my_station_shadow[index];

        /* Check if the entry has L3 related flag enabled */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & l3_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return BCM_E_NOT_FOUND;
        }

        /* Check if the entry is not added by l2cache API */
        rv = soc_l2u_get(unit, &l2u_entry, index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return rv;
        }

        if (soc_mem_field32_get(unit, L2_USER_ENTRYm, &l2u_entry,
                             RESERVED_0f)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return BCM_E_NOT_FOUND;
        }
        /* Check to see if we need to delete or modify the entry */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & tunnel_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            /* Delete the entry if no tunnel related flag is enabled */
            sal_memset(entry, 0, sizeof(*entry));
        } else {
            /* Modify the entry if any tunnel related flag is enabled */
            for (i = 0; i < entry_words; i++) {
                entry->entry_data[i] &= ~l3_mask[i];
            }
            soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, DISCARDf, 0);
            soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, COPY_TO_CPUf,
                                0);
        }
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           entry);
    }

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      _bcm_tr3_l2_bookkeeping_info_init
 * Purpose:
 *      Initialize the Tr3 L2 book keeping info 
 * Parameters:
 *      unit - (IN) Unit number
 */
STATIC int
_bcm_tr3_l2_bookkeeping_info_init(int unit)
{
    int num_station, alloc_size;
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);

    num_station = soc_mem_index_count(unit, MY_STATION_TCAMm);
    alloc_size = sizeof(my_station_tcam_entry_t) * num_station;

    if (NULL == info->my_station_shadow) {
        info->my_station_shadow =
            sal_alloc(alloc_size, "my station shadow");
        if (info->my_station_shadow == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(info->my_station_shadow, 0, alloc_size);
    }

    /* L3 flags */
    soc_mem_field32_set(unit, MY_STATION_TCAMm,
                        &info->my_station_l3_mask,
                        IPV4_TERMINATION_ALLOWEDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm,
                        &info->my_station_l3_mask,
                        IPV6_TERMINATION_ALLOWEDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm,
                        &info->my_station_l3_mask,
                        ARP_RARP_TERMINATION_ALLOWEDf, 1);
    /* Tunnel flags */
    soc_mem_field32_set(unit, MY_STATION_TCAMm,
                        &info->my_station_tunnel_mask,
                        MIM_TERMINATION_ALLOWEDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm,
                        &info->my_station_tunnel_mask,
                        MPLS_TERMINATION_ALLOWEDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm,
                    &info->my_station_tunnel_mask,
                    TRILL_TERMINATION_ALLOWEDf, 1);

    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_l2_cbl_init(int unit) {

    soc_mem_t mem;
    int entry_words;
    int index_max;
    union {
        port_cbl_table_entry_t port_cbl[128];
        uint32 w[1];
    } entry;
    void *entries[1];
    soc_profile_mem_t *profile;
    int module;
    uint32 profile_index;
    int field_len;

    /* Create profile for PORT_CBL_TABLE table */
    if (!SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(soc_mem_clear(unit, PORT_CBL_TABLEm,
                                             COPYNO_ALL, FALSE));
    }
    if (_bcm_l2_tr3_port_cbl_profile[unit] == NULL) {
        _bcm_l2_tr3_port_cbl_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t),
                      "PORT_CBL_TABLE profile");
        if (_bcm_l2_tr3_port_cbl_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_l2_tr3_port_cbl_profile[unit]);
    }
    profile = _bcm_l2_tr3_port_cbl_profile[unit];
    mem = PORT_CBL_TABLEm;
    entry_words = sizeof(port_cbl_table_entry_t) / sizeof(uint32);
    field_len = soc_mem_field_length(unit, PORT_CBL_TABLE_MODBASEm,
                                     BASEf);
    /* coverity[large_shift : FALSE] */
    index_max = (1 << (field_len - 1)) - 1;
    entries[0] = &entry;
    sal_memset(&entry.port_cbl[0], 0, sizeof(entry.port_cbl[0]));
    field_len = soc_mem_field_length(unit, PORT_CBL_TABLEm,
                                     PORT_LEARNING_CLASSf);
    soc_mem_field32_set(unit, PORT_CBL_TABLEm, &entry.port_cbl[0],
                        PORT_LEARNING_CLASSf, (1 << field_len) - 1);

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_index_create(unit, &mem, &entry_words, NULL,
                                      &index_max, entries, 1,
                                      profile));

    if (!SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(soc_mem_clear(unit, PORT_CBL_TABLE_MODBASEm,
                                    COPYNO_ALL, FALSE));
        sal_memset(entry.port_cbl, 0, sizeof(entry.port_cbl));
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, profile, entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1,
                                 &profile_index));
        for (module = 1; module <= SOC_MODID_MAX(unit); module++) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference
                 (unit, profile, profile_index,
                  SOC_PORT_ADDR_MAX(unit) + 1));
        }
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    else {
        port_cbl_table_modbase_entry_t base_entry;

        for (module = 0; module <= SOC_MODID_MAX(unit); module++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, PORT_CBL_TABLE_MODBASEm,
                              MEM_BLOCK_ANY, module, &base_entry));
            profile_index =
                soc_mem_field32_get(unit, PORT_CBL_TABLE_MODBASEm,
                                    &base_entry, BASEf);
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference
                 (unit, profile, profile_index,
                  SOC_PORT_ADDR_MAX(unit) + 1));
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_tr3_l2_reinit
 * Purpose:
 *      Recover L2_INFO s/w state for TR3
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_tr3_l2_reinit(int unit)
{
    int i, num_station;
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    my_station_tcam_entry_t   entry;
    bcm_mac_t                 mac_addr;
    bcm_vlan_t                vid;
    bcm_l2_addr_t             l2addr;
    int                       rv;

    /* Compute hash for each entry */
    num_station = soc_mem_index_count(unit, MY_STATION_TCAMm);
    for (i = 0; i < num_station; i++) {

        sal_memcpy(&entry, soc_mem_entry_null(unit, MY_STATION_TCAMm),
                   sizeof(my_station_tcam_entry_t));
        BCM_IF_ERROR_RETURN
            (READ_MY_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &entry));
        if (!soc_mem_field32_get(unit,
                                  MY_STATION_TCAMm, &entry, VALIDf)) {
            continue;
        }

        soc_mem_mac_addr_get(unit, MY_STATION_TCAMm,
                             &entry, MAC_ADDRf, mac_addr);
        vid = soc_mem_field32_get(unit, MY_STATION_TCAMm,
                                  &entry, VLAN_IDf);

        rv = _bcm_tr3_l2_addr_get(unit, mac_addr, vid, TRUE, &l2addr);
        if (BCM_FAILURE(rv)) {
            continue;
        }

        sal_memcpy(&info->my_station_shadow[i], &entry,
                           sizeof(my_station_tcam_entry_t));
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_tr3_l2_init
 * Purpose:
 *      Initialize the BCM L2 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_init(int unit)
{
    int rv; 
    int frozen;
    uint32 regval = 0;

    BCM_IF_ERROR_RETURN(soc_tr3_l2_is_frozen(unit, &frozen));
    if (TRUE == frozen) {
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(bcm_tr3_l2_detach(unit));

    /* First, determine which memories are active in this device */
    _bcm_tr3_l2_mems_valid[unit] = 0;
    if (0 < soc_mem_index_count(unit, L2_ENTRY_1m)) {
        /* The ISM tables are active, so include them */
        _bcm_tr3_l2_mems_valid[unit] |= _BCM_TR3_L2_SELECT_INTERNAL;
    }
    if (soc_feature(unit, soc_feature_esm_support)) {
        if (SOC_MEM_IS_VALID(unit, EXT_L2_ENTRY_1m) &&
            (0 < soc_mem_index_count(unit, EXT_L2_ENTRY_1m))) {
            /* The ESM standard table is active, so include it */
            _bcm_tr3_l2_mems_valid[unit] |=
                _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1;
        }
        if ((SOC_MEM_IS_VALID(unit, EXT_L2_ENTRY_2m)) &&
            (0 < soc_mem_index_count(unit, EXT_L2_ENTRY_2m))) {
            /* The ESM wide table is active, so include it */
            _bcm_tr3_l2_mems_valid[unit] |=
                _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2;
        }
    }

    /* If, only ESM is available and no ISM, then learn on ESM tables */
    if (!(_bcm_tr3_l2_mems_valid[unit] & _BCM_TR3_L2_SELECT_INTERNAL)) {
        if ((_bcm_tr3_l2_mems_valid[unit] & 
            _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) ||
            (_bcm_tr3_l2_mems_valid[unit] & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2))
        {   
            BCM_IF_ERROR_RETURN(READ_LEARN_CONTROLr(unit, &regval));
            soc_reg_field_set(unit, LEARN_CONTROLr, &regval,
                              EXTERNAL_L2_ENTRYf, 1);
            BCM_IF_ERROR_RETURN(WRITE_LEARN_CONTROLr(unit, regval));
        }
    }
                
    /* Initialize memory accelation structures */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_memacc_init(unit));

    /* Clear the HW state and associated SW data */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_hw_init(unit));

    if (!SOC_WARM_BOOT(unit)) {
        /*
         * Init L2 cache
         */
        rv = bcm_tr3_l2_cache_init(unit);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }

    /*
     * Init L2 learn limit
     */
    rv = _bcm_tr3_l2_learn_limit_init(unit);
    if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_cbl_init(unit));

    /* Create callback data structure */
    _bcm_tr3_l2_data[unit] = sal_alloc(sizeof(*_bcm_tr3_l2_data[unit]),
                                       "BCM TR3 L2 callback data");
    if (NULL == _bcm_tr3_l2_data[unit]) {
        return BCM_E_MEMORY;
    }
    sal_memset(_bcm_tr3_l2_data[unit], 0, sizeof(*_bcm_tr3_l2_data[unit]));

    if (SOC_CONTROL(unit)->l2x_age_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
    }

    /* L2 book keeping info init */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_bookkeeping_info_init(unit));

    if(soc_property_get(unit, spn_RUN_L2_SW_AGING, 0)) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, 0));
    }
    if (soc_feature(unit, soc_feature_l2_overflow)) {
        if(soc_property_get(unit, spn_L2_OVERFLOW_EVENT, 0)) {
            SOC_CONTROL_LOCK(unit);
            SOC_CONTROL(unit)->l2_overflow_enable = TRUE;
            SOC_CONTROL_UNLOCK(unit);
        }
    }
    _bcm_tr3_l2_init[unit] = 1; /* some positive value */


    BCM_IF_ERROR_RETURN(_bcm_tr_l2_station_control_init(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l2_station_reload(unit));
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_reinit(unit));

    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_clear
 * Purpose:
 *      Clear the BCM L2 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_clear(int unit)
{
    BCM_TR3_L2_INIT(unit);

    /* Clear the HW state and associated SW data */
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_hw_init(unit));

    /* Stop l2x thread if callback registration started it. */
    if (_bcm_tr3_l2_data[unit]->flags & _BCM_TR3_L2X_THREAD_STOP) {
        BCM_IF_ERROR_RETURN(soc_tr3_l2x_stop(unit));
    }

    /* Clear registered callbacks from _bcm_tr3_l2_data structure */
    _bcm_tr3_l2_data[unit]->cb_count = 0;
    _bcm_tr3_l2_data[unit]->flags = 0;
    sal_memset(&_bcm_tr3_l2_data[unit]->cb, 0,
               sizeof(_bcm_tr3_l2_data[unit]->cb));

    _bcm_tr3_l2_init[unit] = 1;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_detach
 * Purpose:
 *      Finalize the BCM L2 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_detach(int unit)
{
    int frozen;
    int mem_idx;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        _bcm_tr3_l2_init[unit] = 0;
        return BCM_E_NONE;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Verify the table is not frozen */
    BCM_IF_ERROR_RETURN(soc_tr3_l2_is_frozen(unit, &frozen));
    if (TRUE == frozen) {
        return BCM_E_BUSY;
    }

    if (_tr3_mbi_entries[unit] != NULL) {
        sal_free(_tr3_mbi_entries[unit]);
        _tr3_mbi_entries[unit] = NULL;
    }
 
    /* Deallocate resources used by station control support. */
    BCM_IF_ERROR_RETURN(_bcm_tr_l2_station_control_deinit(unit));

    /* Deallocate _bcm_tr3_l2_data structure if callbacks were registered */
    if (NULL != _bcm_tr3_l2_data[unit]) {
        if (NULL != _bcm_tr3_l2_data[unit]->l2_mutex) {
            sal_mutex_destroy(_bcm_tr3_l2_data[unit]->l2_mutex);
        }
        sal_free(_bcm_tr3_l2_data[unit]);
        _bcm_tr3_l2_data[unit] = NULL;
    }

    /* Deallocate memory acceration structures */
    for (mem_idx = _BCM_TR3_L2_MEM_L2_ENTRY_1;
         mem_idx < _BCM_TR3_L2_MEM_NUM;
         mem_idx++) {
        if (NULL != _bcm_tr3_l2_memacc_list[unit][mem_idx]) {
            sal_free(_bcm_tr3_l2_memacc_list[unit][mem_idx]);
            _bcm_tr3_l2_memacc_list[unit][mem_idx] = NULL;
        }
    }

    _bcm_tr3_l2_init[unit] = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_addr_add
 * Purpose:
 *      Add an L2 address entry to the specified device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to bcm_l2_addr_t containing all valid fields.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    _bcm_tr3_l2_entries_t l2_entries, l2_entries_lookup;
    l2_combo_entry_t ovf_entry;
    uint32 key_ent[SOC_MAX_MEM_WORDS], key_ovf[SOC_MAX_MEM_WORDS];
    soc_mem_t mem_type;
    int rv, mb_index = 0;
    uint8 l2_flags;

    BCM_TR3_L2_INIT(unit);

    /* Input parameters check. */
    if (NULL == l2addr) {
        return (BCM_E_PARAM);
    }

    if (l2addr->flags & BCM_L2_LOCAL_CPU) {
        l2addr->port = CMIC_PORT(unit);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_modid_get(unit, &l2addr->modid));
    }

    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, l2addr->tgid));
    }

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        if (soc_feature(unit, soc_feature_extended_address_class)) {
            if ((l2addr->group > SOC_EXT_ADDR_CLASS_MAX(unit)) ||
                (l2addr->group < 0)) {
                return (BCM_E_PARAM);
            }
        } else if ((l2addr->group > SOC_ADDR_CLASS_MAX(unit)) || 
            (l2addr->group < 0)) {
            return (BCM_E_PARAM);
        }
        if (!BCM_PBMP_IS_NULL(l2addr->block_bitmap)) {
            return (BCM_E_PARAM);
        }
    } else {
        if (l2addr->group)  {
            return (BCM_E_PARAM);
        }
    }
    
    if (l2addr->flags & BCM_L2_L3LOOKUP) {
         BCM_IF_ERROR_RETURN(_bcm_tr3_l2_myStation_add(unit, l2addr));
    }

    /* Does the entry exist already? */
    sal_memset(&l2_entries_lookup, 0, sizeof (l2_entries_lookup));
    sal_memset(&l2_entries, 0, sizeof (l2_entries));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_ALL(unit);
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2api_to_l2hw(unit, &l2_entries,
                                               l2addr, FALSE));

    /* Store the flags which indicates which memory that the entry should go */
    l2_flags = l2_entries.entry_flags;

    /* For lookup purpose set flags to ALL, so that search is done in
     * all the 4 possible tables
     */
    l2_entries.entry_flags =_BCM_TR3_L2_SELECT_ALL(unit);

    _BCM_ALL_L2X_MEM_LOCK(unit);
    rv = _bcm_tr3_l2_entries_lookup(unit, &l2_entries, &l2_entries_lookup);

    /* Restore the flags back to original value */
    l2_entries.entry_flags = l2_flags;

    if (BCM_E_NOT_FOUND == rv) {
        l2_entries_lookup.entry_flags = 0; /* No found entries */
    } else if (BCM_FAILURE(rv)) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }

    if (0 != (l2_entries_lookup.entry_flags & l2_entries.entry_flags)) {
        /* The existing location is capable of holding the requested info,
         * so overwrite it.  Mark the current table location as the
         * only one of interest.
         */
        l2_entries.entry_flags = l2_entries_lookup.entry_flags;
    }

    if (!SOC_L2X_GROUP_ENABLE_GET(unit)) {
        /* Mac blocking, attempt to associate with bitmap entry */
        rv = _bcm_tr3_mac_block_insert(unit, l2addr->block_bitmap,
                                       &mb_index);
        if (BCM_E_NONE != rv) {
            _BCM_ALL_L2X_MEM_UNLOCK(unit);
            return rv;
        }
        _bcm_tr3_l2hw_entries_field32_set(unit, &l2_entries,
                      _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX, mb_index);
    }

    rv = BCM_E_FULL;
    /* In order of efficiency? */
    if (l2_entries.entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        rv = soc_mem_insert_return_old(unit, L2_ENTRY_1m, MEM_BLOCK_ANY, 
                                       (void *)&(l2_entries.l2_entry_1),
                                       (void *)&(l2_entries.l2_entry_1));
    }
    if ((l2_entries.entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_1) &&
        (BCM_E_FULL == rv)) {
        rv = soc_mem_insert_return_old(unit, EXT_L2_ENTRY_1m, MEM_BLOCK_ANY, 
                                       (void *)&(l2_entries.ext_l2_entry_1),
                                       (void *)&(l2_entries.ext_l2_entry_1));
    }
    if ((l2_entries.entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) &&
        (BCM_E_FULL == rv)) {
        rv = soc_mem_insert_return_old(unit, L2_ENTRY_2m, MEM_BLOCK_ANY, 
                                       (void *)&(l2_entries.l2_entry_2),
                                       (void *)&(l2_entries.l2_entry_2));
    }
    if ((l2_entries.entry_flags & _BCM_TR3_L2_SELECT_EXT_L2_ENTRY_2) &&
        (BCM_E_FULL == rv)) {
        rv = soc_mem_insert_return_old(unit, EXT_L2_ENTRY_2m, MEM_BLOCK_ANY, 
                                       (void *)&(l2_entries.ext_l2_entry_2),
                                       (void *)&(l2_entries.ext_l2_entry_2));
    }

    if ((BCM_E_FULL == rv) && (l2addr->flags & BCM_L2_REPLACE_DYNAMIC)) {
        /* coverity[stack_use_callee] */
        /* coverity[stack_use_overflow] */
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        rv = _bcm_tr3_l2_hash_dynamic_replace(unit, &l2_entries);
        _BCM_ALL_L2X_MEM_LOCK(unit);
    } else if ((BCM_E_FULL == rv) && (l2addr->flags & BCM_L2_ADD_OVERRIDE_PENDING)) {
        /* coverity[stack_use_callee] */
        /* coverity[stack_use_overflow] */
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        rv = _bcm_tr3_l2_hash_pending_override(unit, &l2_entries);
        _BCM_ALL_L2X_MEM_LOCK(unit);
    } else if (rv == BCM_E_EXISTS) {
        if (!SOC_L2X_GROUP_ENABLE_GET(unit)) {
            mb_index = _bcm_tr3_l2hw_entries_field32_get(unit, &l2_entries,
                                _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX);
            _bcm_tr3_mac_block_delete(unit, mb_index);
        }
        rv = BCM_E_NONE;
    }

    if ((rv < 0) && !SOC_L2X_GROUP_ENABLE_GET(unit)) {
         _bcm_tr3_mac_block_delete(unit, mb_index);
    }

    if (BCM_SUCCESS(rv)) {
        if (soc_feature(unit, soc_feature_l2_overflow) &&
            (l2addr->flags & BCM_L2_ENTRY_OVERFLOW)) {
            /* Check if this entry is in the overflow memory and resume process */
            if (soc_tr3_l2_overflow_entry_get(unit, &ovf_entry, &mem_type)) {
                _BCM_ALL_L2X_MEM_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            sal_memset(key_ent, 0, sizeof(key_ent));
            sal_memset(key_ovf, 0, sizeof(key_ovf));
            /* Extract and compare key only (for matching mem types) */
            if ((mem_type == L2_ENTRY_1m) &&
                (l2_entries.entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1)) {
                soc_mem_field_get(unit, mem_type, l2_entries.l2_entry_1.entry_data,
                                  L2__KEYf, key_ent);
                soc_mem_field_get(unit, mem_type, ovf_entry.words, L2__KEYf,
                                  key_ovf);
                if (sal_memcmp(key_ent, key_ovf, sizeof(key_ent))) {
                    _BCM_ALL_L2X_MEM_UNLOCK(unit);
                    return BCM_E_PARAM;
                } else {
                    if (soc_tr3_l2_overflow_start(unit)) {
                        _BCM_ALL_L2X_MEM_UNLOCK(unit);
                        return BCM_E_INTERNAL;
                    }
                }
            } else if ((mem_type == L2_ENTRY_2m) &&
               (l2_entries.entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2)) {
                soc_mem_field_get(unit, mem_type, l2_entries.l2_entry_2.entry_data,
                                  L2__KEYf, key_ent);
                soc_mem_field_get(unit, mem_type, ovf_entry.words, L2__KEY_0f,
                                  key_ovf);
                if (sal_memcmp(key_ent, key_ovf, sizeof(key_ent))) {
                    _BCM_ALL_L2X_MEM_UNLOCK(unit);
                    return BCM_E_PARAM;
                } else {
                    if (soc_tr3_l2_overflow_start(unit)) {
                        _BCM_ALL_L2X_MEM_UNLOCK(unit);
                        return BCM_E_INTERNAL;
                    }
                }
            } else {
                _BCM_ALL_L2X_MEM_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        }

        if ((0 != l2_entries_lookup.entry_flags) &&
            (0 == (l2_entries_lookup.entry_flags & l2_entries.entry_flags))) {
            rv = _bcm_tr3_l2_entries_delete(unit, &l2_entries_lookup);
            
            if (!SOC_L2X_GROUP_ENABLE_GET(unit)) {
                mb_index =
                    _bcm_tr3_l2hw_entries_field32_get(unit, &l2_entries_lookup,
                             _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX);
                _bcm_tr3_mac_block_delete(unit, mb_index);
            }
        }
    }
    _BCM_ALL_L2X_MEM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete
 * Purpose:
 *      Delete an L2 address entry from the specified device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) MAC address to delete
 *      vid - (IN) VLAN id
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t  l2addr;
    _bcm_tr3_l2_entries_t l2_entries, l2_entries_lookup;
    int            l2_index, mb_index;
    int            rv = BCM_E_NONE;

    BCM_TR3_L2_INIT(unit);

    if (BCM_VLAN_VALID(vid)) {
        /* Flush any myStation entry */
        rv = _bcm_tr3_l2_myStation_delete(unit, mac, vid, &l2_index);
        if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
            if (rv != BCM_E_NONE) {
                return rv;
            }
        }
    }

    bcm_l2_addr_t_init(&l2addr, mac, vid);

    /* Does the entry exist already? */
    sal_memset(&l2_entries_lookup, 0, sizeof (l2_entries_lookup));
    sal_memset(&l2_entries, 0, sizeof (l2_entries));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_ALL(unit);
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2api_to_l2hw(unit, &l2_entries,
                                               &l2addr, TRUE));

    _BCM_ALL_L2X_MEM_LOCK(unit);
    rv = _bcm_tr3_l2_entries_lookup(unit, (void *)&l2_entries,
                                    (void *)&l2_entries_lookup);

    if (BCM_E_NONE != rv) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }

    if (!SOC_L2X_GROUP_ENABLE_GET(unit)) {
        mb_index = _bcm_tr3_l2hw_entries_field32_get(unit,
                                                     &l2_entries_lookup,
                            _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX);
        _bcm_tr3_mac_block_delete(unit, mb_index);
    }

    rv = _bcm_tr3_l2_entries_delete(unit, &l2_entries_lookup);
    _BCM_ALL_L2X_MEM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_tr3_delete_replace_flags_convert
 * Description:
 *      A helper function to all delete_by APIs to use bcm_l2_replace. 
 *      Converts L2 flags to L2 replace flags compatable to use with bcm_l2_replace
 * Parameters:
 *      unit        [IN]- device unit
 *      flags       [IN] - BCM_L2_DELETE_XXX
 *      repl_flags  [OUT]- Vflags BCM_L2_REPLACE_XXX
 * Returns:
 *      BCM_E_XXX
 */

STATIC int 
_bcm_tr3_delete_replace_flags_convert(int unit, uint32 flags, uint32 *repl_flags)
{
    uint32 tmp_flags = 0;

    if (flags & BCM_L2_REPLACE_AGE){
        tmp_flags = BCM_L2_REPLACE_AGE;
    }else{
        tmp_flags = BCM_L2_REPLACE_DELETE;
    }

    if (flags & BCM_L2_DELETE_PENDING) {
        tmp_flags |= BCM_L2_REPLACE_PENDING;
    }
    if (flags & BCM_L2_DELETE_STATIC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_STATIC; 
    }
    if (flags & BCM_L2_DELETE_NO_CALLBACKS) {
        tmp_flags |= BCM_L2_REPLACE_NO_CALLBACKS;
    }
    if (flags & BCM_L2_LEARN_LIMIT) {
        tmp_flags |= BCM_L2_REPLACE_LEARN_LIMIT;
    }

    if ((flags & BCM_L2_REPLACE_MATCH_UC) && (flags & BCM_L2_REPLACE_MATCH_MC))
    {
        /* Both flags are contradictory and can't be set together */
        return BCM_E_PARAM;
    }

    if (flags & BCM_L2_REPLACE_MATCH_UC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_UC;
    }

    if (flags & BCM_L2_REPLACE_MATCH_MC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_MC;
    }

    *repl_flags = tmp_flags;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_port
 * Purpose:
 *      Delete L2 entries associated with a destination module/port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mod - (IN) <UNDEF>
 *      port - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_port(int unit, 
    bcm_module_t mod, 
    bcm_port_t port, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);
    
    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &match_addr.modid));
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0);
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_mac
 * Purpose:
 *      Delete L2 entries associated with a MAC address.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_mac(int unit, 
    bcm_mac_t mac, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_l2_replace(unit, repl_flags|BCM_L2_REPLACE_VPN_TYPE, &match_addr, 0, 0, 0));
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0);
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_vlan
 * Purpose:
 *      Delete L2 entries associated with a VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vid - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_vlan(int unit, 
    bcm_vlan_t vid, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0);
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_trunk
 * Purpose:
 *      Delete L2 entries associated with a trunk.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_trunk(int unit, 
    bcm_trunk_t tid, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.tgid = tid;
    match_addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0); 
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_mac_port
 * Purpose:
 *      Delete L2 entries associated with a MAC address and a
 *      destination module/port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) <UNDEF>
 *      mod - (IN) <UNDEF>
 *      port - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_mac_port(int unit, 
    bcm_mac_t mac, 
    bcm_module_t mod, 
    bcm_port_t port, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));
    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &match_addr.modid));
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0);
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_vlan_port
 * Purpose:
 *      Delete L2 entries associated with a VLAN and a destination
 *      module/port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vid - (IN) <UNDEF>
 *      mod - (IN) <UNDEF>
 *      port - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_vlan_port(int unit, 
    bcm_vlan_t vid, 
    bcm_module_t mod, 
    bcm_port_t port, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &match_addr.modid));
    } else {
        match_addr.modid = mod;
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN |BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0);
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_by_vlan_trunk
 * Purpose:
 *      Delete L2 entries associated with a VLAN and a destination
 *      module/port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vid - (IN) <UNDEF>
 *      tid - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_delete_by_vlan_trunk(int unit, 
    bcm_vlan_t vid, 
    bcm_trunk_t tid, 
    uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    match_addr.tgid = tid;
    match_addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN
        (_bcm_tr3_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_tr3_l2_replace(unit, repl_flags, &match_addr, 0, 0, 0);
}

/*
 * Function:
 *      _bcm_tr3_l2_addr_get
 * Purpose:
 *      Check if an L2 entry is present in the L2 table.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      mac_addr      - (IN) input MAC address to search
 *      vid           - (IN) input VLAN ID to search
 *      check_l2_only - (IN) only check if the target exist in L2 table.
 *      l2addr        - (OUT) Pointer to bcm_l2_addr_t structureI
 *                            to receive result
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_tr3_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid, 
                     int check_l2_only, bcm_l2_addr_t *l2addr)
{
    _bcm_tr3_l2_entries_t l2_entries, l2_entries_lookup;
    int          rv, rval;

    /* First check if in My Station table in case it's an L3-enabled
     * entry.
     */
    if (!check_l2_only) {
        rv = _bcm_tr3_l2_myStation_get(unit, mac_addr, vid, l2addr);
        if (BCM_E_NOT_FOUND != rv) {
            return rv;
        }
    }

    bcm_l2_addr_t_init(l2addr, mac_addr, vid);

    /* Find if the entry exists */
    sal_memset(&l2_entries_lookup, 0, sizeof (l2_entries_lookup));
    sal_memset(&l2_entries, 0, sizeof (l2_entries));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_ALL(unit);
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2api_to_l2hw(unit, &l2_entries,
                                               l2addr, TRUE));

    _BCM_ALL_L2X_MEM_LOCK(unit);
    rv = _bcm_tr3_l2_entries_lookup(unit, (void *)&l2_entries,
                                    (void *)&l2_entries_lookup);

    if (BCM_E_NONE != rv) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }

    if (check_l2_only) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }
    
    /* Translate back to API abstraction */
    rv = _bcm_tr3_l2api_from_l2hw(unit, l2addr, &l2_entries_lookup);
    _BCM_ALL_L2X_MEM_UNLOCK(unit);

    /* Translate l2mc index */
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType, &rval));
    if ((rval) && BCM_MAC_IS_MCAST(l2addr->mac) &&
                  !(_BCM_MULTICAST_IS_SET(l2addr->l2mc_group))) {
       _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group, _BCM_MULTICAST_TYPE_L2,
                                                            l2addr->l2mc_group);
    }

    return rv;
}

int 
bcm_tr3_l2_addr_get(int unit, bcm_mac_t mac_addr, bcm_vlan_t vid, 
                    bcm_l2_addr_t *l2addr)
{
    return _bcm_tr3_l2_addr_get(unit, mac_addr, vid, FALSE, l2addr);
}

/*
 * Function:
 *      _bcm_tr3_l2_addr_callback
 * Description:
 *      Callback used with chip addr registration functions.
 *      This callback calls all the top level client callbacks.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      l2addr
 *      insert
 *      userdata
 * Returns:
 */
STATIC void
_bcm_tr3_l2_addr_callback(int unit, bcm_l2_addr_t *l2addr, int insert,
                          void *userdata)
{
    _bcm_tr3_l2_data_t *ad = _bcm_tr3_l2_data[unit];
    int                i;

    if (ad->l2_mutex == NULL) {
        return;
    }

    BCM_TR3_L2_CB_LOCK(unit);
    for(i = 0; i < _BCM_TR3_L2_CB_MAX; i++) {
        if(ad->cb[i].fn) {
            ad->cb[i].fn(unit, l2addr, insert, ad->cb[i].fn_data);
        }
    }
    BCM_TR3_L2_CB_UNLOCK(unit);
}

/*
 * Function:
 *      bcm_tr3_l2_addr_register
 * Purpose:
 *      Register/Unregister a callback routine for BCM L2 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 *      callback - (IN) <UNDEF>
 *      userdata - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_register(int unit, 
                         bcm_l2_addr_callback_t callback, 
                         void *userdata)
{
    _bcm_tr3_l2_data_t *ad = _bcm_tr3_l2_data[unit];
    int                i;
#if defined(BCM_XGS_SWITCH_SUPPORT)
    int rv;
    int usec;
#endif /* BCM_XGS_SWITCH_SUPPORT */

    if (!SOC_IS_XGS_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

    BCM_TR3_L2_INIT(unit);

    _bcm_tr3_l2_cbs[unit] = _bcm_tr3_l2_addr_callback;
    _bcm_tr3_l2_cb_data[unit] = NULL;

    BCM_TR3_L2_CB_MUTEX(unit);
    BCM_TR3_L2_CB_LOCK(unit);
#if defined(BCM_XGS_SWITCH_SUPPORT)
    /* Start L2x thread if it isn't running already. */
    if (!soc_tr3_l2x_running(unit, NULL, NULL)) {
        usec = (SAL_BOOT_BCMSIM)? BCMSIM_L2XMSG_INTERVAL : 3000000;
        usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, usec);
        rv = soc_tr3_l2x_start(unit, 0, usec);
        if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
            _bcm_tr3_l2_cbs[unit] = NULL;
            _bcm_tr3_l2_cb_data[unit] = NULL;
            BCM_TR3_L2_CB_UNLOCK(unit);
            return(rv);
        }
        ad->flags |= _BCM_TR3_L2X_THREAD_STOP;
    }
    if (soc_feature(unit, soc_feature_l2_overflow)) {
        rv = soc_tr3_l2_overflow_start(unit);
        if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
            _bcm_tr3_l2_cbs[unit] = NULL;
            _bcm_tr3_l2_cb_data[unit] = NULL;
            BCM_TR3_L2_CB_UNLOCK(unit);
            return(rv);
        }
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* See if the function is already registered (with same data) */
    for (i = 0; i < _BCM_TR3_L2_CB_MAX; i++) {
        if (ad->cb[i].fn == callback && ad->cb[i].fn_data == userdata) {
            BCM_TR3_L2_CB_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }

    /* Not found; add to list. */
    for (i = 0; i < _BCM_TR3_L2_CB_MAX; i++) {
        if (ad->cb[i].fn == NULL) {
            ad->cb[i].fn = callback;
            ad->cb[i].fn_data = userdata;
            ad->cb_count++;
            break;
        }
    }

    BCM_TR3_L2_CB_UNLOCK(unit);
    return i >= _BCM_TR3_L2_CB_MAX ? BCM_E_RESOURCE : BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_addr_unregister
 * Purpose:
 *      Register/Unregister a callback routine for BCM L2 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 *      callback - (IN) <UNDEF>
 *      userdata - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_addr_unregister(int unit, 
                           bcm_l2_addr_callback_t callback, 
                           void *userdata)
{
    _bcm_tr3_l2_data_t *ad = _bcm_tr3_l2_data[unit];
    int                rv = BCM_E_NOT_FOUND;
    int                i;

    if (!SOC_IS_XGS_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

    BCM_TR3_L2_INIT(unit);
    BCM_TR3_L2_CB_MUTEX(unit);
    BCM_TR3_L2_CB_LOCK(unit);

    for (i = 0; i < _BCM_TR3_L2_CB_MAX; i++) {
        if((ad->cb[i].fn == callback) && (ad->cb[i].fn_data == userdata)) {
            rv = BCM_E_NONE;
            ad->cb[i].fn = NULL;
            ad->cb[i].fn_data = NULL;
            ad->cb_count--;
            if (ad->cb_count == 0) {
                _bcm_tr3_l2_cbs[unit] = NULL;
                _bcm_tr3_l2_cb_data[unit] = NULL;

#if defined(BCM_XGS_SWITCH_SUPPORT)
                /* Stop l2x thread if callback registration started it. */
                if (ad->flags & _BCM_TR3_L2X_THREAD_STOP) {
                    rv = soc_tr3_l2x_stop(unit);
                    ad->flags &= ~_BCM_TR3_L2X_THREAD_STOP;
                }
#endif /* BCM_XGS_SWITCH_SUPPORT */
            }
        }
    }

    BCM_TR3_L2_CB_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_tr3_l2_age_timer_set
 * Purpose:
 *      Set/Get the age timer.
 * Parameters:
 *      unit - (IN) Unit number.
 *      age_seconds - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_age_timer_set(int unit, int age_seconds)
{
    int rv, enabled;
    
    if (age_seconds < 0) {
        return BCM_E_PARAM;
    }
    enabled = age_seconds ? 1 : 0;
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, age_seconds, enabled);
    return rv; 
}

/*
 * Function:
 *      bcm_tr3_l2_age_timer_get
 * Purpose:
 *      Set/Get the age timer.
 * Parameters:
 *      unit - (IN) Unit number.
 *      age_seconds - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_age_timer_get(int unit, int *age_seconds)
{
    int rv, seconds, enabled;
    
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled);
    if (enabled) {
        *age_seconds = seconds;
    } else {
        *age_seconds = 0;
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_conflict_get
 * Purpose:
 *      Given an L2 address entry, return existing addresses which
 *      could conflict.
 * Parameters:
 *      unit - (IN) Unit number.
 *      addr - (IN) Pointer to bcm_l2_addr_t structure to seeking conflict
 *      cf_array - (OUT) Array of bcm_l2_addr_t structures to return
 *                       conflicting entries
 *      cf_max - (IN) Maximum entries of supplied cf_array
 *      cf_count - (OUT) Number of conflicting entries returned in cf_array
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                        bcm_l2_addr_t *cf_array, int cf_max, int *cf_count)
{
    uint32 *kt;
    void *entry;
    int rv, index;
    soc_mem_t mem;
    uint32 valid;
    uint8 i, bidx, flag, num_ent;
    _soc_ism_mem_banks_t mem_banks;
    _bcm_tr3_l2_entries_t l2_entries, l2_entries_match;
    uint32 l2_entry[SOC_MAX_MEM_WORDS], result, bucket_index;
    uint32 key_type_1[] = {SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE, SOC_MEM_KEY_L2_ENTRY_1_L2_VFI};
    uint32 key_type_2[] = {SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE, SOC_MEM_KEY_L2_ENTRY_2_L2_VFI};
    
    BCM_TR3_L2_INIT(unit);

    /* Input parameters check. */
    if ((NULL == addr) || (NULL == cf_count) || 
        (NULL == cf_array) || (cf_max <= 0)) {
        return (BCM_E_PARAM);
    }
    sal_memset(&l2_entries, 0, sizeof (l2_entries));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_ALL(unit);
    BCM_IF_ERROR_RETURN(_bcm_tr3_l2api_to_l2hw(unit, &l2_entries,
                                               addr, FALSE));
    /* NOTE: Implementation assumes that enough criteria/info will be set 
             in the bcm_l2_addr_t param to not have any ambiguity between
             ENTRY_1 and ENTRY_2, i.e the following conditions would not
             be true together */ 
    if (l2_entries.entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_1) {
        mem = L2_ENTRY_1m;
        entry = &l2_entries.l2_entry_1;
        kt = key_type_1;
        valid = VALIDf;
        flag = _BCM_TR3_L2_SELECT_L2_ENTRY_1;
    } else if (l2_entries.entry_flags & _BCM_TR3_L2_SELECT_L2_ENTRY_2) {
        mem = L2_ENTRY_2m;
        entry = &l2_entries.l2_entry_2;
        kt = key_type_2;
        valid = VALID_0f;
        flag = _BCM_TR3_L2_SELECT_L2_ENTRY_2;
    } else {
        return BCM_E_PARAM;
    }
    rv = soc_ism_get_banks_for_mem(unit, mem, mem_banks.banks, 
                                   mem_banks.bank_size, &mem_banks.count);
    if (SOC_FAILURE(rv)) {
        return BCM_E_INTERNAL;
    }
    if (mem_banks.count == 0) {
        return BCM_E_NOT_FOUND;
    }
    *cf_count = 0;
    for (bidx = 0; bidx < mem_banks.count; bidx++) {
        rv = soc_generic_hash(unit, mem, entry, (uint32)1 << mem_banks.banks[bidx],
                              0, &index, &result, &bucket_index, &num_ent);
        if (SOC_FAILURE(rv)) {
            return BCM_E_INTERNAL;
        }
        for (i = 0; (i < num_ent) && (*cf_count < cf_max);) {
            rv = soc_mem_read(unit, mem, COPYNO_ALL, index + i, l2_entry);
            if (SOC_FAILURE(rv)) {
                return BCM_E_MEMORY;
            }
            if (soc_mem_field32_get(unit, mem, &l2_entry, valid) && 
                ((soc_mem_field32_get(unit, mem, &l2_entry, KEY_TYPEf) == kt[0]) ||
                 (soc_mem_field32_get(unit, mem, &l2_entry, KEY_TYPEf) == kt[1]))) {
                l2_entries_match.entry_flags = flag;
                if (mem == L2_ENTRY_1m) {
                    sal_memcpy(&l2_entries_match.l2_entry_1, l2_entry,
                               sizeof(l2_entry_1_entry_t));
                } else {
                    sal_memcpy(&l2_entries_match.l2_entry_2, l2_entry,
                               sizeof(l2_entry_2_entry_t));
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_tr3_l2api_from_l2hw(unit, &cf_array[*cf_count], &l2_entries_match));
                *cf_count += 1;
            }
            i += soc_ism_get_bucket_offset(unit, mem, -1, entry, l2_entry);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_port_native
 * Purpose:
 *      Determine if the given port is "native" from the point of view
 *      of L2.
 * Parameters:
 *      unit - (IN) Unit number.
 *      modid - (IN) <UNDEF>
 *      port - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_port_native(int unit, int modid, int port)
{
    int unit_modid;
    int unit_port;  /* In case unit supports multiple module IDs */
    bcm_trunk_t     tgid;
    int             id, isLocal;

    BCM_TR3_L2_INIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &modid,
            &port, &tgid, &id));

        if (-1 != id || BCM_TRUNK_INVALID != tgid) {
            return FALSE;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, modid, &isLocal));
    if (isLocal != TRUE) {
        return FALSE;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &unit_modid));
    if (unit_modid != modid) {
        unit_port = port + 32; 
    } else {
        unit_port = port;
    }

    if (IS_ST_PORT(unit, unit_port)) {
        return FALSE;
    } 

    return TRUE;
}

/*
 * Function:
 *      bcm_tr3_l2_cache_init
 * Purpose:
 *      Initialize the L2 cache.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_cache_init(int unit)
{
    int     skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        bcm_l2_cache_addr_t addr;
        l2u_entry_t entry;
        int index;

        if (!SAL_BOOT_QUICKTURN) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, L2_USER_ENTRYm, COPYNO_ALL, TRUE));
        }
        
        bcm_l2_cache_addr_t_init(&addr);
        addr.flags = BCM_L2_CACHE_CPU | BCM_L2_CACHE_BPDU;
        /* Set default BPDU addresses (01:80:c2:00:00:00) */
        ENET_SET_MACADDR(addr.mac, _soc_mac_spanning_tree);
        ENET_SET_MACADDR(addr.mac_mask, _soc_mac_all_ones);
        
        addr.dest_port = CMIC_PORT(unit);
        
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_cache_to_l2u(unit, &entry, &addr));
        for (index = 0; index < L2U_BPDU_COUNT; index++) {
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, index, &index));
        }

        if (!SAL_BOOT_QUICKTURN) {
            /* Set 01:80:c2:00:00:10 */
            addr.mac[5] = 0x10;
            
            BCM_IF_ERROR_RETURN(_bcm_tr3_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
            /* Set 01:80:c2:00:00:0x */
            addr.mac[5] = 0x00;
            addr.mac_mask[5] = 0xf0;
            
            BCM_IF_ERROR_RETURN(_bcm_tr3_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
            /* Set 01:80:c2:00:00:2x */
            addr.mac[5] = 0x20;
            
            BCM_IF_ERROR_RETURN(_bcm_tr3_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
        }

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr3_l2_cache_set
 * Purpose:
 *      Set an L2 cache entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      index - (IN) <UNDEF>
 *      addr - (IN) <UNDEF>
 *      index_used - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_cache_set(int unit, 
    int index, 
    bcm_l2_cache_addr_t *addr, 
    int *index_used)
{
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    BCM_TR3_L2_INIT(unit);

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        l2u_entry_t         l2u_entry;
        bcm_l2_cache_addr_t l2_addr;

        sal_memcpy(&l2_addr, addr, sizeof(bcm_l2_cache_addr_t));

        if (soc_feature(unit, soc_feature_extended_address_class)) {
            if ((l2_addr.lookup_class > SOC_EXT_ADDR_CLASS_MAX(unit)) ||
                (l2_addr.lookup_class < 0)) {
                return (BCM_E_PARAM);
            }
        } else if ((l2_addr.lookup_class > SOC_ADDR_CLASS_MAX(unit)) || 
            (l2_addr.lookup_class < 0)) {
            return (BCM_E_PARAM);
        }

        /* ESW architecture can't support multiple destinations */
        if (BCM_PBMP_NOT_NULL(l2_addr.dest_ports)) {
            return BCM_E_PARAM;
        }

        
         BCM_IF_ERROR_RETURN(
            _bcm_tr3_l2_cache_to_l2u(unit, &l2u_entry, &l2_addr));
        
        SOC_IF_ERROR_RETURN(
            soc_l2u_insert(unit, &l2u_entry, index, index_used));

        if (l2_addr.flags & BCM_L2_CACHE_L3) {
            return (_bcm_tr3_l2cache_myStation_set(unit, *index_used, &l2_addr));
        }

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr3_l2_cache_get
 * Purpose:
 *      Get an L2 cache entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      index - (IN) <UNDEF>
 *      addr - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_cache_get(int unit, 
    int index, 
    bcm_l2_cache_addr_t *addr)
{
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    BCM_TR3_L2_INIT(unit);

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        l2u_entry_t l2u_entry;

        SOC_IF_ERROR_RETURN(
            soc_l2u_get(unit, &l2u_entry, index));

        BCM_IF_ERROR_RETURN(
            _bcm_tr3_l2_cache_from_l2u(unit, addr, &l2u_entry));

        if (addr->flags & BCM_L2_CACHE_L3) {
            BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2cache_myStation_get(unit, index, addr));
        }

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr3_l2_cache_delete
 * Purpose:
 *      Clear an L2 cache entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      index - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_cache_delete(int unit, int index)
{
    int skip_l2u, rv;
    l2u_entry_t entry;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    BCM_TR3_L2_INIT(unit);

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        if (index < 0 || index > soc_mem_index_max(unit, L2_USER_ENTRYm)) {
            return BCM_E_PARAM;
        }
        soc_mem_lock(unit, L2_USER_ENTRYm);
        rv = READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &entry);

        if (BCM_SUCCESS(rv) && _l2u_field32_get(unit, &entry, RESERVED_0f)) {
            rv = _bcm_tr3_l2cache_myStation_delete(unit, index);
        }

        if (BCM_SUCCESS(rv)) {
            sal_memset(&entry, 0, sizeof(entry));
            rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &entry);
        }

        soc_mem_unlock(unit, L2_USER_ENTRYm);
        return rv;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr3_l2_cache_delete_all
 * Purpose:
 *      Clear all L2 cache entries.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_cache_delete_all(int unit)
{
    int skip_l2u, rv, index, index_max;
    l2u_entry_t entry;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    BCM_TR3_L2_INIT(unit);

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);


    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        index_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

        rv = BCM_E_NONE;
        soc_mem_lock(unit, L2_USER_ENTRYm);
        for (index = 0; index <= index_max; index++) {
            rv = READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &entry);
            if (BCM_SUCCESS(rv) && _l2u_field32_get(unit, &entry, RESERVED_0f)) {
                rv = _bcm_tr3_l2cache_myStation_delete(unit, index);
                /* Ignore entry not found case */
                if (rv == BCM_E_NOT_FOUND) {
                    rv = BCM_E_NONE;
                }
            } 

            if (BCM_SUCCESS(rv)) {
                sal_memset(&entry, 0, sizeof(entry));
                rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &entry);
            }

            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        soc_mem_unlock(unit, L2_USER_ENTRYm);
        return rv;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr3_metro_myStation_add
 * Purpose:
 *      Add a (MAC, VLAN) for tunnel/MPLS/TRILL processing.
 *      Frames destined to (MAC, VLAN) is subjected to MPLS processing.
 *      Shared by MPLS, MIM and TRILL
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 *      port - Ingress port
 */
int
bcm_tr3_metro_myStation_add(int unit, bcm_mac_t mac,
                            bcm_vlan_t vlan, bcm_port_t port)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, index, alt_index, i, entry_words;
    my_station_tcam_entry_t *entry;
    uint32 *tunnel_mask;
    bcm_mac_t mac_mask;
    uint32 port_mask;

    tunnel_mask = info->my_station_tunnel_mask.entry_data;
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    rv = _bcm_tr3_my_station_lookup(unit, mac, vlan, port, -1, &index,
                                   &alt_index);
    if (BCM_SUCCESS(rv)) {
        /* Entry exist, update the entry */
        entry = &info->my_station_shadow[index];
        for (i = 0; i < entry_words; i++) {
            entry->entry_data[i] |= tunnel_mask[i];
        }
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           entry);
    } else if (rv == BCM_E_NOT_FOUND && index == -1) {
        /* No free entry available for insertion */
        rv = BCM_E_FULL;
    }
    if (rv != BCM_E_NOT_FOUND) {
        soc_mem_unlock(unit, MY_STATION_TCAMm);
        return rv;
    }

    if (alt_index != -1 &&
        ((port == -1 && alt_index > index) ||
         (port != -1 && alt_index < index))) {
        /* Free entry is available for new entry insertion and alternate entry
         * exists. However need to swap these 2 entries to maintain proper
         * lookup precedence */
        rv = soc_mem_read(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, alt_index,
                          &info->my_station_shadow[index]);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                               &info->my_station_shadow[index]);
        }
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return rv;
        }
        index = alt_index;
    }

    /* Add the new entry */
    entry = &info->my_station_shadow[index];
    sal_memset(entry, 0, sizeof(*entry));
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, VALIDf, 1);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, VLAN_IDf, vlan);
    soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, VLAN_ID_MASKf, 0xfff);
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, entry, MAC_ADDRf, mac);
    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
    soc_mem_mac_addr_set(unit, MY_STATION_TCAMm, entry, MAC_ADDR_MASKf,
                         mac_mask);
    if (port != -1) {
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, ING_PORT_NUMf,
                            port);
        port_mask = (1 << soc_mem_field_length(unit, MY_STATION_TCAMm,
                                               ING_PORT_NUMf)) - 1;
        soc_mem_field32_set(unit, MY_STATION_TCAMm, entry, ING_PORT_NUM_MASKf,
                            port_mask);
    }
    for (i = 0; i < entry_words; i++) {
        entry->entry_data[i] |= tunnel_mask[i];
    }
    rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index, entry);

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_tunnel_add
 * Purpose:
 *      Add a destination L2 address to trigger tunnel processing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) MAC address
 *      vlan - (IN) VLAN ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    BCM_TR3_L2_INIT(unit);
    return bcm_tr3_metro_myStation_add(unit, mac, vlan, -1);
}

/*
 * Function:
 *      bcm_tr3_metro_myStation_delete
 * Purpose:
 *    Delete a (MAC, VLAN) for tunnel/MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
bcm_tr3_metro_myStation_delete(int unit, bcm_mac_t mac,
                                         bcm_vlan_t vlan, bcm_port_t port)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, index, i, entry_words;
    my_station_tcam_entry_t *entry;
    uint32 *l3_mask, *tunnel_mask;

    l3_mask = info->my_station_l3_mask.entry_data;
    tunnel_mask = info->my_station_tunnel_mask.entry_data;
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    rv = _bcm_tr3_my_station_lookup(unit, mac, vlan, port, -1, &index, NULL);
    if (BCM_SUCCESS(rv)) {
        entry = &info->my_station_shadow[index];

        /* Check if the entry has tunnel related flag enabled */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & tunnel_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            soc_mem_unlock(unit, MY_STATION_TCAMm);
            return BCM_E_NOT_FOUND;
        }

        /* Check to see if we need to delete or modify the entry */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & l3_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            /* Delete the entry if no L3 related flag is enabled */
            sal_memset(entry, 0, sizeof(*entry));
        } else {
            /* Modify the entry if any L3 related flag is enabled */
            for (i = 0; i < entry_words; i++) {
                entry->entry_data[i] &= ~tunnel_mask[i];
            }
        }
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           entry);
    }

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_tunnel_delete
 * Purpose:
 *      Clear a destination L2 address used to trigger tunnel
 *      processing.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac -  (IN) MAC address
 *      vlan - (IN) VLAN ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    return  bcm_tr3_metro_myStation_delete(unit, mac, vlan, -1);
}

/*
 * Function:
 *      bcm_tr3_metro_myStation_delete_all
 * Purpose:
 *      Delete all (MAC, VLAN) for tunnel/MPLS processing.
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 */
int
bcm_tr3_metro_myStation_delete_all(int unit)
{
    _bcm_tr3_l2_bookkeeping_t *info = L2_INFO(unit);
    int rv, index, i, num_entries, entry_words;
    my_station_tcam_entry_t *entry, valid_mask;
    uint32 *l3_mask, *tunnel_mask;

    sal_memset(&valid_mask, 0, sizeof(valid_mask));
    soc_mem_field32_set(unit, MY_STATION_TCAMm, &valid_mask, VALIDf, 1);

    l3_mask = info->my_station_l3_mask.entry_data;
    tunnel_mask = info->my_station_tunnel_mask.entry_data;
    num_entries = soc_mem_index_count(unit, MY_STATION_TCAMm);
    entry_words = soc_mem_entry_words(unit, MY_STATION_TCAMm);

    soc_mem_lock(unit, MY_STATION_TCAMm);

    rv = BCM_E_NONE;
    for (index = 0; index < num_entries; index++) {
        entry = &info->my_station_shadow[index];

        /* Check if the entry has tunnel related flag enabled */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & tunnel_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            continue;
        }

        /* Check if the entry is valid */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & valid_mask.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            continue;
        }

        /* Check to see if we need to delete or modify the entry */
        for (i = 0; i < entry_words; i++) {
            if (entry->entry_data[i] & l3_mask[i]) {
                break;
            }
        }
        if (i == entry_words) {
            /* Delete the entry if no L3 related flag is enabled */
            sal_memset(entry, 0, sizeof(*entry));
        } else {
            /* Modify the entry if any L3 related flag is enabled */
            for (i = 0; i < entry_words; i++) {
                entry->entry_data[i] &= ~tunnel_mask[i];
            }
        }
        rv = soc_mem_write(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL, index,
                           entry);
    }

    soc_mem_unlock(unit, MY_STATION_TCAMm);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_tunnel_delete_all
 * Purpose:
 *      Clear all destination L2 addresses used to trigger tunnel
 *      processing.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_tunnel_delete_all(int unit)
{
    BCM_TR3_L2_INIT(unit);
    return bcm_tr3_metro_myStation_delete_all(unit);
}

/*
 * Function:
 *      bcm_tr3_l2_learn_limit_set
 * Purpose:
 *      Set L2 addresses learn limit.
 * Parameters:
 *      unit - (IN) Unit number.
 *      limit - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_learn_limit_set(int unit, bcm_l2_learn_limit_t *limit)
{
    int32 max_limit;
    uint32 type, action;
    
    BCM_TR3_L2_INIT(unit);

    if (!limit) {
        return BCM_E_PARAM;
    }

    max_limit = BCM_MAC_LIMIT_MAX(unit);

    if (limit->limit > max_limit) {
        return BCM_E_PARAM;
    }

    type = limit->flags &
           (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | 
            BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TRUNK);

    action = limit->flags & 
             (BCM_L2_LEARN_LIMIT_ACTION_DROP | BCM_L2_LEARN_LIMIT_ACTION_CPU |
              BCM_L2_LEARN_LIMIT_ACTION_PREFER);

    if (!type) {
        return BCM_E_PARAM;
    }

    if (type != BCM_L2_LEARN_LIMIT_SYSTEM &&
        (action & BCM_L2_LEARN_LIMIT_ACTION_PREFER)) {
        return BCM_E_PARAM;
    }

    if (type & BCM_L2_LEARN_LIMIT_SYSTEM) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_learn_limit_system_set(unit, action,
                                                            limit->limit));
    }

    if (type & BCM_L2_LEARN_LIMIT_PORT) {
        int index;
        if (BCM_GPORT_IS_SET(limit->port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, limit->port, &(limit->port)));
        }

        if (!SOC_PORT_VALID(unit, limit->port)) {
            return BCM_E_PORT;
        }
            
        /* BCM_L2_LEARN_LIMIT_ACTION_PREFER should be used only in system limit
           and only for Raptor/Raven devices */
        if (limit->flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
            return BCM_E_PARAM;
        }

        index = limit->port + soc_mem_index_count(unit, TRUNK_GROUPm);
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_learn_limit_set(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                        index, action, limit->limit));
    }

    if (type & BCM_L2_LEARN_LIMIT_TRUNK) {
        int index;
        /* BCM_L2_LEARN_LIMIT_ACTION_PREFER should be used only in system limit
           and only for Raptor/Raven devices */
        if (limit->flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
            return BCM_E_PARAM;
        }

       if (limit->trunk < 0 ||
           limit->trunk >= soc_mem_index_count(unit, TRUNK_GROUPm)) {
           return BCM_E_PARAM;
       }
       index = limit->trunk;
       BCM_IF_ERROR_RETURN
           (_bcm_tr3_l2_learn_limit_set(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                       index, action, limit->limit));
    }

    if (type & BCM_L2_LEARN_LIMIT_VLAN) {
        int index, vfi;
        /* BCM_L2_LEARN_LIMIT_ACTION_PREFER should be used only in system limit
           and only for Raptor/Raven devices */
        if (limit->flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
            return BCM_E_PARAM;
        }


    if (_BCM_VPN_VFI_IS_SET(limit->vlan)) {
            vfi = 0;
        _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, limit->vlan);
        if (vfi >= soc_mem_index_count(unit, VFIm)) {
            return BCM_E_PARAM;
        }
        index = vfi + soc_mem_index_count(unit, VLAN_TABm);
    } else {
            if (limit->vlan >= soc_mem_index_count(unit, VLAN_TABm)) {
                return BCM_E_PARAM;
            }
            index = limit->vlan;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_learn_limit_set(unit, VLAN_OR_VFI_MAC_LIMITm,
                                        index, action, limit->limit));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_learn_limit_get
 * Purpose:
 *      Get L2 addresses learn limit.
 * Parameters:
 *      unit - (IN) Unit number.
 *      limit - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_learn_limit_get(int unit, bcm_l2_learn_limit_t *limit)
{
    uint32 type;
    int index, vfi;

    BCM_TR3_L2_INIT(unit);

    if (!limit) {
        return BCM_E_PARAM;
    }

    type = limit->flags &
           (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | 
            BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TRUNK);

    switch (type) {
      case BCM_L2_LEARN_LIMIT_SYSTEM:
        return _bcm_tr3_l2_learn_limit_system_get(unit, &limit->flags,
                                                 &limit->limit);
        break;
      case BCM_L2_LEARN_LIMIT_PORT:
        if (BCM_GPORT_IS_SET(limit->port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, limit->port, &(limit->port)));
        }

        if (!SOC_PORT_VALID(unit, limit->port)) {
            return BCM_E_PORT;
        }
        index = limit->port + soc_mem_index_count(unit, TRUNK_GROUPm);
        return _bcm_tr3_l2_learn_limit_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                          index, &limit->flags,
                                          &limit->limit);
        break;
      case BCM_L2_LEARN_LIMIT_TRUNK:

        if (limit->trunk < 0 ||
            limit->trunk >= soc_mem_index_count(unit, TRUNK_GROUPm)) {
            return BCM_E_PARAM;
        }
        index = limit->trunk;
        return _bcm_tr3_l2_learn_limit_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                          index, &limit->flags,
                                          &limit->limit);
        break;
      case BCM_L2_LEARN_LIMIT_VLAN:
        if (_BCM_VPN_VFI_IS_SET(limit->vlan)) {
             vfi = 0;
             _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, limit->vlan);
             if (vfi >= soc_mem_index_count(unit, VFIm)) {
                return BCM_E_PARAM;
             }
             index = vfi + soc_mem_index_count(unit, VLAN_TABm);
        } else {
            if (limit->vlan >= soc_mem_index_count(unit, VLAN_TABm)) {
                return BCM_E_PARAM;
            }
            index = limit->vlan;
        }
        return _bcm_tr3_l2_learn_limit_get(unit, VLAN_OR_VFI_MAC_LIMITm,
                                          index, &limit->flags,
                                          &limit->limit);
        break;
      default:
        return BCM_E_PARAM;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr3_l2_learn_class_set
 * Purpose:
 *      Set/Get attributes for the specified L2 learning class.
 * Parameters:
 *      unit - (IN) Unit number.
 *      lclass - (IN) <UNDEF>
 *      lclass_prio - (IN) <UNDEF>
 *      flags - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_learn_class_set(int unit, int lclass, int lclass_prio, uint32 flags)
{
    uint32 data;

    BCM_TR3_L2_INIT(unit);

    if (lclass < 0 || lclass >= SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr) ||
        lclass_prio < 0 ||
        lclass_prio >= (1 << soc_reg_field_length(unit, CBL_ATTRIBUTEr,
                                                  PORT_LEARNING_PRIORITYf))) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_CBL_ATTRIBUTEr(unit, lclass, &data));
    soc_reg_field_set(unit, CBL_ATTRIBUTEr, &data, PORT_LEARNING_PRIORITYf,
                      lclass_prio);
    soc_reg_field_set(unit, CBL_ATTRIBUTEr, &data, ALLOW_MOVE_IN_CLASSf,
                      flags & BCM_L2_LEARN_CLASS_MOVE ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_CBL_ATTRIBUTEr(unit, lclass, data));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr3_l2_learn_class_get
 * Purpose:
 *      Set/Get attributes for the specified L2 learning class.
 * Parameters:
 *      unit - (IN) Unit number.
 *      lclass - (IN) <UNDEF>
 *      lclass_prio - (OUT) <UNDEF>
 *      flags - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_learn_class_get(int unit, int lclass, int *lclass_pri, uint32 *flags)
{
    uint32 data;

    BCM_TR3_L2_INIT(unit);

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    if (lclass < 0 || lclass >= SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr) ||
        lclass_pri == NULL || flags == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_CBL_ATTRIBUTEr(unit, lclass, &data));
    *lclass_pri = soc_reg_field_get(unit, CBL_ATTRIBUTEr, data,
                                     PORT_LEARNING_PRIORITYf);
    *flags = 0;
    if (soc_reg_field_get(unit, CBL_ATTRIBUTEr, data, ALLOW_MOVE_IN_CLASSf)) {
        *flags |= BCM_L2_LEARN_CLASS_MOVE;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_tr3_l2_learn_port_class_set
 * Purpose:
 *      Set/Get L2 learning class for the specified port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      lclass - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_learn_port_class_set(int unit, bcm_gport_t port, int lclass)
{
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    soc_profile_mem_t *profile;
    port_cbl_table_modbase_entry_t base_entry;
    port_cbl_table_entry_t cbl_entries[128];
    void *entries[1];
    uint32 old_index, index;
    int rv;
    profile = _bcm_l2_tr3_port_cbl_profile[unit];
    entries[0] = &cbl_entries;

    BCM_TR3_L2_INIT(unit);

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_L2_LEARN_CLASSID_VALID(lclass);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (gport_id != -1) {
        if (!BCM_GPORT_IS_MPLS_PORT(port) &&
            !BCM_GPORT_IS_SUBPORT_GROUP(port) &&
            !BCM_GPORT_IS_SUBPORT_PORT(port) &&
            !BCM_GPORT_IS_MIM_PORT(port) &&
            !BCM_GPORT_IS_VLAN_PORT(port) &&
            !BCM_GPORT_IS_WLAN_PORT(port) &&
            !BCM_GPORT_IS_TRILL_PORT(port) &&
            !BCM_GPORT_IS_NIV_PORT(port) &&
            !BCM_GPORT_IS_L2GRE_PORT(port) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port)) {
            return BCM_E_PARAM;
        }
        return soc_mem_field32_modify(unit, PORT_CBL_TABLEm, gport_id,
                                      VIRTUAL_PORT_LEARNING_CLASSf, lclass);
    } else if (trunk_id != -1) {
        return soc_mem_field32_modify(unit, TRUNK_CBL_TABLEm, trunk_id,
                                      PORT_LEARNING_CLASSf, lclass);
    } else {

        soc_mem_lock(unit, PORT_CBL_TABLE_MODBASEm);

        rv = soc_mem_read(unit, PORT_CBL_TABLE_MODBASEm, MEM_BLOCK_ALL, mod_out,
                          &base_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        old_index = soc_mem_field32_get(unit, PORT_CBL_TABLE_MODBASEm,
                                        &base_entry, BASEf);
        rv = soc_profile_mem_get(unit, profile, old_index,
                                 SOC_PORT_ADDR_MAX(unit) + 1, entries);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        soc_mem_field32_set(unit, PORT_CBL_TABLEm, &cbl_entries[port_out],
                            PORT_LEARNING_CLASSf, lclass);
        rv = soc_profile_mem_add(unit, profile, entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        rv = soc_mem_field32_modify(unit, PORT_CBL_TABLE_MODBASEm, mod_out,
                                    BASEf, index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        rv = soc_profile_mem_delete(unit, profile, old_index);

        soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_learn_port_class_get
 * Purpose:
 *      Set/Get L2 learning class for the specified port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Ingress generic port
 *      lclass - (OUT) learning class id (0 - 3)
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_learn_port_class_get(int unit, 
    bcm_gport_t port, 
    int *lclass)
{
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    soc_profile_mem_t *profile;
    port_cbl_table_modbase_entry_t base_entry;
    port_cbl_table_entry_t cbl_entries[128];
    void *entries[1];
    uint32 index;
    port_cbl_table_entry_t port_entry;
    trunk_cbl_table_entry_t trunk_entry;

    BCM_TR3_L2_INIT(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (lclass == NULL) {
        return (BCM_E_PARAM);
    }

    if (gport_id != -1) {
        if (!BCM_GPORT_IS_MPLS_PORT(port) &&
            !BCM_GPORT_IS_SUBPORT_GROUP(port) &&
            !BCM_GPORT_IS_SUBPORT_PORT(port) &&
            !BCM_GPORT_IS_MIM_PORT(port) &&
            !BCM_GPORT_IS_VLAN_PORT(port) &&
            !BCM_GPORT_IS_WLAN_PORT(port) &&
            !BCM_GPORT_IS_TRILL_PORT(port) &&
            !BCM_GPORT_IS_NIV_PORT(port) &&
            !BCM_GPORT_IS_L2GRE_PORT(port) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL, gport_id, &port_entry));
        *lclass = soc_mem_field32_get(unit, PORT_CBL_TABLEm, &port_entry,
                                      VIRTUAL_PORT_LEARNING_CLASSf);
    } else if (trunk_id != -1) {
        BCM_IF_ERROR_RETURN
            (READ_TRUNK_CBL_TABLEm(unit, MEM_BLOCK_ALL, trunk_id, &trunk_entry));
        *lclass = soc_mem_field32_get(unit, TRUNK_CBL_TABLEm, &trunk_entry,
                                      PORT_LEARNING_CLASSf);
    } else {
        profile = _bcm_l2_tr3_port_cbl_profile[unit];
        entries[0] = &cbl_entries;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, PORT_CBL_TABLE_MODBASEm, MEM_BLOCK_ALL, mod_out,
                          &base_entry));
        index = soc_mem_field32_get(unit, PORT_CBL_TABLE_MODBASEm, &base_entry,
                                    BASEf);

        BCM_IF_ERROR_RETURN
            (soc_profile_mem_get(unit, profile, index,
                                 SOC_PORT_ADDR_MAX(unit) + 1, entries));
        *lclass = soc_mem_field32_get(unit, PORT_CBL_TABLEm,
                                      &cbl_entries[port_out],
                                      PORT_LEARNING_CLASSf);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_tr3_l2_traverse_int_cb
 * Description:
 *      Simple internal callback function for bcm_l2_traverse API
 *      just to call a user provided callback on a given entry.
 * Parameters:
 *      unit         device number
 *      trav_st      traverse structure
 * Return:
 *     BCM_E_XXX
 */

int
_bcm_tr3_l2_traverse_int_cb(int unit, void *trav_st)
{
    _bcm_l2_traverse_t *trv;
    bcm_l2_addr_t      l2_addr;

    trv = (_bcm_l2_traverse_t *)trav_st;

    switch(trv->mem) {
    case L2_ENTRY_1m:
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_from_l2_1(unit, &l2_addr,
                                   (l2_entry_1_entry_t *)trv->data));
        if (_l2_addr_counts.flags & L2_DUMP_COUNT) {
            _l2_addr_counts.l2_count.l2_entry_1++;
        }
        break;
    case L2_ENTRY_2m:
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_from_l2_2(unit, &l2_addr,
                                   (l2_entry_2_entry_t *)trv->data));
        if (_l2_addr_counts.flags & L2_DUMP_COUNT) {
            _l2_addr_counts.l2_count.l2_entry_2++;
        }
        break;
    case EXT_L2_ENTRY_1m:
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_from_ext_l2_1(unit, &l2_addr,
                                       (ext_l2_entry_1_entry_t *)trv->data));
        if (_l2_addr_counts.flags & L2_DUMP_COUNT) {
            _l2_addr_counts.l2_count.ext_l2_entry_1++;
        }
        break;
    case EXT_L2_ENTRY_2m:
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_l2_from_ext_l2_2(unit, &l2_addr,
                                       (ext_l2_entry_2_entry_t *)trv->data));
        if (_l2_addr_counts.flags & L2_DUMP_COUNT) {
            _l2_addr_counts.l2_count.ext_l2_entry_2++;
        }
        break;
    default:
        return BCM_E_INTERNAL;
    }

    return trv->user_cb(unit, &l2_addr, trv->user_data);
}

/*
 * Function:
 *      bcm_tr3_l2_traverse
 * Purpose:
 *      Traverse L2 table
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_st      Traverse structure with all the data.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_tr3_l2_traverse(int unit, _bcm_l2_traverse_t *trav_st)
{
    /* Override generic ESW traverse callback with TR3 version */
    trav_st->int_cb = _bcm_tr3_l2_traverse_int_cb;

    return (_bcm_tr3_l2_traverse(unit, trav_st));
}

STATIC int
_bcm_tr3_l2e_ppa_match(int unit, _soc_tr3_l2_replace_t *rep_st, int mem)
{
    _soc_tr3_l2e_ppa_info_t *ppa_info;
    _soc_tr3_l2e_ppa_vlan_t *ppa_vlan;
    ext_l2_entry_1_entry_t  l2_entry_1, old_l2_entry_1;
    ext_l2_entry_2_entry_t  l2_entry_2, old_l2_entry_2;
    int                     i, imin, imax, rv, nmatches;
    uint32                  entdata, entmask, entvalue;
    soc_memacc_t            *memacc;

    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_POLL) {
        return BCM_E_UNAVAIL;
    }

    if (mem == EXT_L2_ENTRY_1m) {
        /* EXT_L2_ENTRY_1 */
        ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
        ppa_vlan = SOC_CONTROL(unit)->ext_l2_ppa_vlan;
        if (ppa_info == NULL) {
            return BCM_E_NONE;
        }

        imin = soc_mem_index_min(unit, mem);
        imax = soc_mem_index_max(unit, mem);

        /* convert match data */
        entdata = _SOC_TR3_L2E_VALID;
        entmask = _SOC_TR3_L2E_VALID;
        if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
            entdata |= 0x00000000;
            entmask |= _SOC_TR3_L2E_STATIC;
        }
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
            entdata |= (rep_st->key_vlan & _SOC_TR3_L2E_VLAN_MASK) <<
                _SOC_TR3_L2E_VLAN_SHIFT;
            entmask |= _SOC_TR3_L2E_VLAN_MASK << _SOC_TR3_L2E_VLAN_SHIFT;
            imin = ppa_vlan->vlan_min[rep_st->key_vlan];
            imax = ppa_vlan->vlan_max[rep_st->key_vlan];
        }
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (rep_st->match_dest.trunk != -1) {
                entdata |= _SOC_TR3_L2E_T |
                    ((rep_st->match_dest.trunk & _SOC_TR3_L2E_TRUNK_MASK)
                     << _SOC_TR3_L2E_TRUNK_SHIFT);
                entmask |= _SOC_TR3_L2E_T |
                    (_SOC_TR3_L2E_TRUNK_MASK << _SOC_TR3_L2E_TRUNK_SHIFT);
            } else if (rep_st->match_dest.vp != -1) {
                entdata |= _SOC_TR3_L2E_VP |
                    ((rep_st->match_dest.vp & _SOC_TR3_L2E_DESTINATION_MASK)
                     << _SOC_TR3_L2E_DESTINATION_SHIFT);
                entmask |= _SOC_TR3_L2E_VP |
                    (_SOC_TR3_L2E_DESTINATION_MASK << 
                     _SOC_TR3_L2E_DESTINATION_SHIFT);
            } else {
                entdata |= 
                    ((rep_st->match_dest.module & _SOC_TR3_L2E_MOD_MASK) <<
                     _SOC_TR3_L2E_MOD_SHIFT) |
                    ((rep_st->match_dest.port & _SOC_TR3_L2E_PORT_MASK) <<
                     _SOC_TR3_L2E_PORT_SHIFT);
                entmask |= _SOC_TR3_L2E_T | _SOC_TR3_L2E_VP |
                    (_SOC_TR3_L2E_MOD_MASK << _SOC_TR3_L2E_MOD_SHIFT) |
                    (_SOC_TR3_L2E_PORT_MASK << _SOC_TR3_L2E_PORT_SHIFT);
            }
        }

        nmatches = 0;

        if (imin >= 0) {
            if ((imax - imin) > SOC_CONTROL(unit)->ext_l2_ppa_threshold) {
                return BCM_E_UNAVAIL;
            }
            for (i = imin; i <= imax; i++) {
                entvalue = ppa_info[i].data;
                if ((entvalue & entmask) != entdata) {
                    continue;
                }
                if (rep_st->flags & BCM_L2_REPLACE_MATCH_MAC) {
                    if (ENET_CMP_MACADDR(rep_st->key_mac, ppa_info[i].mac)) {
                        continue;
                    }
                }
                nmatches += 1;

                /* lookup the matched entry */
                sal_memset(&l2_entry_1, 0, sizeof(l2_entry_1));
                memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                            _BCM_TR3_L2_MEMACC_VLAN_ID);
                soc_memacc_field32_set(memacc, &l2_entry_1, (entvalue >> 16) & 0xfff);
                memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                        _BCM_TR3_L2_MEMACC_MAC_ADDR);
                soc_memacc_mac_addr_set(memacc, &l2_entry_1, 
                                        ppa_info[i].mac);

                /* In case of a VP entry, also assign the KEY_TYPE */
                if(entvalue & _SOC_TR3_L2E_VP) {
                    memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                                _BCM_TR3_L2_MEMACC_KEY_TYPE);
                    soc_memacc_field32_set(memacc, &l2_entry_1, rep_st->ext_key_type);
                }

                /* operate on matched entry */
                if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
                    rv = soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY, 0,
                                                &l2_entry_1, &l2_entry_1, NULL);
                    if (rv < 0) {
                        return rv;
                    }
                } else {
                    /* replace destination fields */
                    rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY, 0,
                                                &l2_entry_1, &l2_entry_1, NULL);
                    if (rep_st->flags & BCM_L2_REPLACE_NEW_TRUNK) {
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                                    _BCM_TR3_L2_MEMACC_DEST_TYPE);
                        soc_memacc_field32_set(memacc, &l2_entry_1, 1);
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                                    _BCM_TR3_L2_MEMACC_TGID);
                        soc_memacc_field32_set(memacc, &l2_entry_1, rep_st->new_dest.trunk);
                    } else {
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                                    _BCM_TR3_L2_MEMACC_MODULE_ID);
                        soc_memacc_field32_set(memacc, &l2_entry_1, rep_st->new_dest.module);
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_1,
                                                    _BCM_TR3_L2_MEMACC_PORT_NUM);
                        soc_memacc_field32_set(memacc, &l2_entry_1, rep_st->new_dest.port);
                    }
                    /* re-insert entry */
                    rv = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ANY, 0,
                                                &l2_entry_1, &old_l2_entry_1, NULL);
                    if (rv == BCM_E_EXISTS) {
                        rv = BCM_E_NONE;
                    }
                    if (rv < 0) {
                        return rv;
                    }
                }
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "tr_l2e_ppa_match 1: imin=%d imax=%d nmatches=%d flags=0x%x\n"),
                     imin, imax, nmatches, rep_st->flags));
    } else {
        /* EXT_L2_ENTRY_2 */
        ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info_2;
        ppa_vlan = SOC_CONTROL(unit)->ext_l2_ppa_vlan_2;
        if (ppa_info == NULL) {
            return BCM_E_NONE;
        }

        mem = EXT_L2_ENTRY_2m;
        imin = soc_mem_index_min(unit, mem);
        imax = soc_mem_index_max(unit, mem);

        /* convert match data */
        entdata = _SOC_TR3_L2E_VALID;
        entmask = _SOC_TR3_L2E_VALID;
        if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
            entdata |= 0x00000000;
            entmask |= _SOC_TR3_L2E_STATIC;
        }
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
            entdata |= (rep_st->key_vlan & _SOC_TR3_L2E_VLAN_MASK) <<
                _SOC_TR3_L2E_VLAN_SHIFT;
            entmask |= _SOC_TR3_L2E_VLAN_MASK << _SOC_TR3_L2E_VLAN_SHIFT;
            imin = ppa_vlan->vlan_min[rep_st->key_vlan];
            imax = ppa_vlan->vlan_max[rep_st->key_vlan];
        }
        if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (rep_st->match_dest.trunk != -1) {
                entdata |= _SOC_TR3_L2E_T |
                    ((rep_st->match_dest.trunk & _SOC_TR3_L2E_TRUNK_MASK)
                     << _SOC_TR3_L2E_TRUNK_SHIFT);
                entmask |= _SOC_TR3_L2E_T |
                    (_SOC_TR3_L2E_TRUNK_MASK << _SOC_TR3_L2E_TRUNK_SHIFT);
            } else if (rep_st->match_dest.vp != -1) {
                entdata |= _SOC_TR3_L2E_VP |
                    ((rep_st->match_dest.vp & _SOC_TR3_L2E_DESTINATION_MASK)
                     << _SOC_TR3_L2E_DESTINATION_SHIFT);
                entmask |= _SOC_TR3_L2E_VP |
                    (_SOC_TR3_L2E_DESTINATION_MASK << 
                     _SOC_TR3_L2E_DESTINATION_SHIFT);
            } else {
                entdata |= 
                    ((rep_st->match_dest.module & _SOC_TR3_L2E_MOD_MASK) <<
                     _SOC_TR3_L2E_MOD_SHIFT) |
                    ((rep_st->match_dest.port & _SOC_TR3_L2E_PORT_MASK) <<
                     _SOC_TR3_L2E_PORT_SHIFT);
                entmask |= _SOC_TR3_L2E_T | _SOC_TR3_L2E_VP |
                    (_SOC_TR3_L2E_MOD_MASK << _SOC_TR3_L2E_MOD_SHIFT) |
                    (_SOC_TR3_L2E_PORT_MASK << _SOC_TR3_L2E_PORT_SHIFT);
            }
        }

        nmatches = 0;

        if (imin >= 0) {
            if ((imax - imin) > SOC_CONTROL(unit)->ext_l2_ppa_threshold) {
                return BCM_E_UNAVAIL;
            }
            for (i = imin; i <= imax; i++) {
                entvalue = ppa_info[i].data;
                if ((entvalue & entmask) != entdata) {
                    continue;
                }
                if (rep_st->flags & BCM_L2_REPLACE_MATCH_MAC) {
                    if (ENET_CMP_MACADDR(rep_st->key_mac, ppa_info[i].mac)) {
                        continue;
                    }
                }
                nmatches += 1;

                /* lookup the matched entry */
                sal_memset(&l2_entry_2, 0, sizeof(l2_entry_2));
                memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                            _BCM_TR3_L2_MEMACC_VLAN_ID);
                soc_memacc_field32_set(memacc, &l2_entry_2, (entvalue >> 16) & 0xfff);
                memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                        _BCM_TR3_L2_MEMACC_MAC_ADDR);
                soc_memacc_mac_addr_set(memacc, &l2_entry_2, 
                                        ppa_info[i].mac);

                /* In case of a VP entry, also assign the KEY_TYPE */
                if(entvalue & _SOC_TR3_L2E_VP) {
                    memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                                _BCM_TR3_L2_MEMACC_KEY_TYPE);
                    soc_memacc_field32_set(memacc, &l2_entry_1, rep_st->ext_key_type);
                }

                /* operate on matched entry */
                if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
                    rv = soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY, 0,
                                                &l2_entry_2, &l2_entry_2, NULL);
                    if (rv < 0) {
                        return rv;
                    }
                } else {
                    /* replace destination fields */
                    rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY, 0,
                                                &l2_entry_2, &l2_entry_2, NULL);
                    if (rep_st->flags & BCM_L2_REPLACE_NEW_TRUNK) {
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                                    _BCM_TR3_L2_MEMACC_DEST_TYPE);
                        soc_memacc_field32_set(memacc, &l2_entry_2, 1);
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                                    _BCM_TR3_L2_MEMACC_TGID);
                        soc_memacc_field32_set(memacc, &l2_entry_2, rep_st->new_dest.trunk);
                    } else {
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                                    _BCM_TR3_L2_MEMACC_MODULE_ID);
                        soc_memacc_field32_set(memacc, &l2_entry_2, rep_st->new_dest.module);
                        memacc = _BCM_TR3_L2_MEMACC(unit, EXT_L2_ENTRY_2,
                                                    _BCM_TR3_L2_MEMACC_PORT_NUM);
                        soc_memacc_field32_set(memacc, &l2_entry_2, rep_st->new_dest.port);
                    }
                    /* re-insert entry */
                    rv = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ANY, 0,
                                                &l2_entry_2, &old_l2_entry_2, NULL);
                    if (rv == BCM_E_EXISTS) {
                        rv = BCM_E_NONE;
                    }
                    if (rv < 0) {
                        return rv;
                    }
                }
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "tr_l2e_ppa_match 2: imin=%d imax=%d nmatches=%d flags=0x%x\n"),
                     imin, imax, nmatches, rep_st->flags));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_tr3_l2_bulk_control
 * Description:
 *     Setup hardware L2 bulk control registers
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_bulk_control(int unit, _soc_tr3_l2_replace_t *rep_st)
{
    int rv = BCM_E_NONE;
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_L2_BULK_CONTROLr(unit, &rval));
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf,
                          ((rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS) ? 0 : 1));
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf,
                      (rep_st->flags & BCM_L2_REPLACE_AGE ? 3: (rep_st->flags & BCM_L2_REPLACE_DELETE ? 1 : 2)));
    
    /* First work on L2_ENTRY_1m */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_1m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));

    /* Set match mask */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &rep_st->match_mask1));
    /* Set match data */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &rep_st->match_data1));
    /* Set replace mask */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 3, &rep_st->new_mask1));
    /* Set replace data */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 2, &rep_st->new_data1));
    
    BCM_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    
    /* Then work on L2_ENTRY_2 */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_2m));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));

    /* Set match mask */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &rep_st->match_mask2));
    /* Set match data */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &rep_st->match_data2));
    /* Set replace mask */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 3, &rep_st->new_mask2));
    /* Set replace data */
    BCM_IF_ERROR_RETURN
        (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 2, &rep_st->new_data2));
    
    BCM_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    if (!soc_feature(unit, soc_feature_esm_support)) {
        return BCM_E_NONE;
    }
    
    /* First work on EXT_L2_ENTRY_1 */
    if (SOC_CONTROL(unit)->l2e_ppa) {
        rv = _bcm_tr3_l2e_ppa_match(unit, rep_st, EXT_L2_ENTRY_1m);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }
    if (!SOC_CONTROL(unit)->l2e_ppa || (rv == BCM_E_UNAVAIL)) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                          _SOC_TR3_L2_BULK_BURST_MAX);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                          SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 1);
        SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));

        /* Set match mask */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &rep_st->ext_match_mask1));
        /* Set match data */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &rep_st->ext_match_data1));
        /* Set replace mask */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 3, &rep_st->ext_new_mask1));
        /* Set replace data */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 2, &rep_st->ext_new_data1));

        BCM_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
        SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
        /* Issue dummy op */
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
        SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
        SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
    }
    
    rv = BCM_E_NONE;
    /* Then work on EXT_L2_ENTRY_2 */
    if (SOC_CONTROL(unit)->l2e_ppa) {
        rv = _bcm_tr3_l2e_ppa_match(unit, rep_st, EXT_L2_ENTRY_2m);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }
    if (!SOC_CONTROL(unit)->l2e_ppa || (rv == BCM_E_UNAVAIL)) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf,
                          (rep_st->flags & BCM_L2_REPLACE_AGE ? 
                          3 : (rep_st->flags & BCM_L2_REPLACE_DELETE ? 1 : 2)));
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                          SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
        SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));

        /* Set match mask */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &rep_st->ext_match_mask2));
        /* Set match data */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &rep_st->ext_match_data2));
        /* Set replace mask */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 3, &rep_st->ext_new_mask2));
        /* Set replace data */
        BCM_IF_ERROR_RETURN
            (WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 2, &rep_st->ext_new_data2));

        BCM_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
        SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
        /* Issue dummy op */
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
        SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
        SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_tr3_l2_replace_by_hw
 * Description:
 *     Helper function to _bcm_tr3_l2_replace_by_hw
 * Parameters:
 *     unit         device number
 *     rep_st       structure with information of what to replace
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_tr3_l2_replace_by_hw(int unit, _soc_tr3_l2_replace_t *rep_st)
{
    int rv, seconds, enabled;

    if (NULL == rep_st) {
        return BCM_E_PARAM;
    }
    /* Read l2 aging interval. */
    SOC_IF_ERROR_RETURN
        (SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled));
    if (enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
    }
    _BCM_ALL_L2X_MEM_LOCK(unit);
    rv = _bcm_tr3_l2_bulk_control(unit, rep_st);
    _BCM_ALL_L2X_MEM_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        if (enabled) {
            SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
        }
        return rv;
    }

    if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
        if(enabled) {
            SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
        }
        return BCM_E_NONE;
    }

    if (SOC_L2_DEL_SYNC_LOCK(SOC_CONTROL(unit)) < 0) {
        if (enabled) {
            SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
        }
        return BCM_E_RESOURCE;
    }
    rv = _soc_tr3_l2x_sync_replace(unit, rep_st,
                                   rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
                                   SOC_L2X_NO_CALLBACKS : 0);
    SOC_L2_DEL_SYNC_UNLOCK(SOC_CONTROL(unit));
    if (enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
    }
    return rv;
}

/*
 * Function:
 *     _bcm_tr3_l2_replace_dest_setup
 * Description:
 *     Helper function to bcm_l2_replace API to setup a destination
 */
STATIC int
_bcm_tr3_l2_replace_dest_setup(int unit, bcm_module_t module, bcm_port_t port,
                               bcm_trunk_t trunk, int is_trunk,
                               _soc_tr3_l2_replace_dest_t *dest)
{
    /* If all are -1, means ignore the replacement with new destination */
    if (module == -1 && port == -1 && trunk == -1) {
        dest->module = dest->port = dest->trunk = dest->vp = -1;
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_SUBPORT_GROUP(port) ||
            BCM_GPORT_IS_SUBPORT_PORT(port) ||
            BCM_GPORT_IS_MIM_PORT(port) ||
            BCM_GPORT_IS_VLAN_PORT(port) ||
            BCM_GPORT_IS_WLAN_PORT(port) ||
            BCM_GPORT_IS_TRILL_PORT(port) ||
            BCM_GPORT_IS_NIV_PORT(port) ||
            BCM_GPORT_IS_L2GRE_PORT(port) ||
            BCM_GPORT_IS_EXTENDER_PORT(port)) {
            /* all these gport has similar encoding format */
            dest->vp = port & 0xffffff;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, port, &dest->module, &dest->port,
                                        &dest->trunk, &dest->vp));
        }
    } else if (is_trunk) {
        BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, trunk));
        dest->trunk = trunk;
        dest->vp = -1;
    } else {
        PORT_DUALMODID_VALID(unit, port);
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, module, port,
                                     &dest->module, &dest->port));
        if (!SOC_MODID_ADDRESSABLE(unit, dest->module)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, dest->port)) {
            return BCM_E_PORT;
        }
        dest->trunk = -1;
        dest->vp = -1;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_l2_replace_data_mask_setup(int unit, _soc_tr3_l2_replace_t *rep)
{
    bcm_mac_t mac_mask;
    bcm_mac_t mac_data;
    int key_type;
    int field_len;

    sal_memset(&rep->match_mask1, 0, sizeof(l2_entry_1_entry_t));
    sal_memset(&rep->match_data1, 0, sizeof(l2_entry_1_entry_t));
    sal_memset(&rep->new_mask1, 0, sizeof(l2_entry_1_entry_t));
    sal_memset(&rep->new_data1, 0, sizeof(l2_entry_1_entry_t));
    sal_memset(&rep->match_mask2, 0, sizeof(l2_entry_2_entry_t));
    sal_memset(&rep->match_data2, 0, sizeof(l2_entry_2_entry_t));
    sal_memset(&rep->new_mask2, 0, sizeof(l2_entry_2_entry_t));
    sal_memset(&rep->new_data2, 0, sizeof(l2_entry_2_entry_t));
    
    if (soc_feature(unit, soc_feature_esm_support)) {
        sal_memset(&rep->ext_match_mask1, 0, sizeof(ext_l2_entry_1_entry_t));
        sal_memset(&rep->ext_match_data1, 0, sizeof(ext_l2_entry_1_entry_t));
        sal_memset(&rep->ext_new_mask1, 0, sizeof(ext_l2_entry_1_entry_t));
        sal_memset(&rep->ext_new_data1, 0, sizeof(ext_l2_entry_1_entry_t));
        sal_memset(&rep->ext_match_mask2, 0, sizeof(ext_l2_entry_2_entry_t));
        sal_memset(&rep->ext_match_data2, 0, sizeof(ext_l2_entry_2_entry_t));
        sal_memset(&rep->ext_new_mask2, 0, sizeof(ext_l2_entry_2_entry_t));
        sal_memset(&rep->ext_new_data2, 0, sizeof(ext_l2_entry_2_entry_t));
    }
    /* First work on Internal L2 */
    /* First work on L2_ENTRY_1m */
    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1, VALIDf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1, VALIDf, 1);

    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1, WIDEf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1, WIDEf, 0);

    field_len = soc_mem_field_length(unit, L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1, KEY_TYPEf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1, KEY_TYPEf,
                        rep->key_type);

    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep->key_vfi != -1) {
            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__VFIf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1, L2__VFIf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1, L2__VFIf,
                                rep->key_vfi);
        } else {
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                                L2__VLAN_IDf, 0xfff);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__VLAN_IDf, rep->key_vlan);
        }
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                             L2__MAC_ADDRf, mac_mask);
        soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &rep->match_data1,
                             L2__MAC_ADDRf, rep->key_mac);
    }

    if ((rep->flags & BCM_L2_REPLACE_MATCH_UC) && 
        !(rep->flags & BCM_L2_REPLACE_MATCH_MC)) {
        /* The 40th bit of Unicast must be 0 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &rep->match_mask1, L2__MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &rep->match_data1, L2__MAC_ADDRf,
                             mac_data);
    } else if ((rep->flags & BCM_L2_REPLACE_MATCH_MC) &&
         !(rep->flags & BCM_L2_REPLACE_MATCH_UC)) {
        /* The 40th bit of Multicast must be 1 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &rep->match_mask1, L2__MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &rep->match_data1, L2__MAC_ADDRf,
                             mac_data);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__DEST_TYPEf);
        soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                            L2__DEST_TYPEf, (1 << field_len) - 1);
        if (rep->match_dest.vp != -1) {
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m,
                                             L2__DESTINATIONf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                                L2__DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__DESTINATIONf, rep->match_dest.vp);
        } else if (rep->match_dest.trunk != -1) {
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__TGIDf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                                L2__TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__TGIDf, rep->match_dest.trunk);
        } else {
            /* L2__DEST_TYPEf == 0 */

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m,
                                             L2__MODULE_IDf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                                L2__MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__MODULE_IDf, rep->match_dest.module);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__PORT_NUMf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1,
                                L2__PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1,
                                L2__PORT_NUMf, rep->match_dest.port);
        }
    }

    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1, STATIC_BITf,
                            1);
        /* STATIC_BIT field in data is 0 */
    }

    soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_mask1, L2__PENDINGf, 1);
    if (rep->flags & BCM_L2_REPLACE_PENDING) {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->match_data1, L2__PENDINGf,
                            1);
    }

    if (!(rep->flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) {
        if (rep->new_dest.vp != -1) {
            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__DEST_TYPEf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_data1,
                                L2__DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m,
                                             L2__DESTINATIONf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_data1,
                                L2__DESTINATIONf, rep->new_dest.vp);
        } else if (rep->new_dest.trunk != -1) {
            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__DEST_TYPEf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_data1,
                                L2__DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__TGIDf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_data1,
                                L2__TGIDf, rep->new_dest.trunk);
        } else if (rep->new_dest.port != -1 
                   && rep->new_dest.module != -1) {
            /* L2__DEST_TYPEf == 0 */
            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__DEST_TYPEf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__DEST_TYPEf, (1 << field_len) - 1);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m,
                                             L2__MODULE_IDf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_data1,
                                L2__MODULE_IDf, rep->new_dest.module);

            field_len = soc_mem_field_length(unit, L2_ENTRY_1m, L2__PORT_NUMf);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                L2__PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_data1,
                                L2__PORT_NUMf, rep->new_dest.port);
        }

        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2_ENTRY_1m, &rep->new_mask1,
                                HITSAf, 1);
        }
    }

    /* Then work on L2_ENTRY_2 */
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, VALID_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, VALID_1f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, VALID_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, VALID_1f, 1);

    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, WIDE_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, WIDE_1f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, WIDE_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, WIDE_1f, 1);

    field_len = soc_mem_field_length(unit, L2_ENTRY_2m, KEY_TYPE_0f);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, KEY_TYPE_0f,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, KEY_TYPE_1f,
                        (1 << field_len) - 1);
    if (rep->key_type == SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE) {
        key_type = SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE;
    } else if (rep->key_type == SOC_MEM_KEY_L2_ENTRY_1_L2_VFI) {
        key_type = SOC_MEM_KEY_L2_ENTRY_2_L2_VFI;
    } else {
        key_type = rep->key_type;
    }
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, KEY_TYPE_0f,
                        key_type);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, KEY_TYPE_1f,
                        key_type);

    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep->key_vfi != -1) {
            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__VFIf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, L2__VFIf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, L2__VFIf,
                                rep->key_vfi);
        } else {
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                                L2__VLAN_IDf, 0xfff);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__VLAN_IDf, rep->key_vlan);
        }
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                             L2__MAC_ADDRf, mac_mask);
        soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &rep->match_data2,
                             L2__MAC_ADDRf, rep->key_mac);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__DEST_TYPEf);
        soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                            L2__DEST_TYPEf, (1 << field_len) - 1);
        if (rep->match_dest.vp != -1) {
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m,
                                             L2__DESTINATIONf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                                L2__DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__DESTINATIONf, rep->match_dest.vp);
        } else if (rep->match_dest.trunk != -1) {
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__TGIDf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                                L2__TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__TGIDf, rep->match_dest.trunk);
        } else {
            /* L2__DEST_TYPEf == 0 */

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m,
                                             L2__MODULE_IDf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                                L2__MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__MODULE_IDf, rep->match_dest.module);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__PORT_NUMf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                                L2__PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2,
                                L2__PORT_NUMf, rep->match_dest.port);
        }
    }

    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                            STATIC_BIT_0f, 1);
        soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2,
                            STATIC_BIT_1f, 1);
        /* STATIC_BIT field in data is 0 */
    }

    soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_mask2, L2__PENDINGf, 1);
    if (rep->flags & BCM_L2_REPLACE_PENDING) {
        soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->match_data2, L2__PENDINGf,
                            1);
    }

    if (!(rep->flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) {
        if (rep->new_dest.vp != -1) {
            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__DEST_TYPEf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_data2,
                                L2__DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m,
                                             L2__DESTINATIONf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_data2,
                                L2__DESTINATIONf, rep->new_dest.vp);
        } else if (rep->new_dest.trunk != -1) {
            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__DEST_TYPEf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_data2,
                                L2__DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__TGIDf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_data2,
                                L2__TGIDf, rep->new_dest.trunk);
        } else if (rep->new_dest.port != -1 
                   && rep->new_dest.module != -1) {
            /* L2__DEST_TYPEf == 0 */
            field_len = soc_mem_field_length(unit, L2_ENTRY_2m, L2__DEST_TYPEf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__DEST_TYPEf, (1 << field_len) - 1);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m,
                                             L2__MODULE_IDf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_data2,
                                L2__MODULE_IDf, rep->new_dest.module);

            field_len = soc_mem_field_length(unit, L2_ENTRY_2m,
                                             L2__PORT_NUMf);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                L2__PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_data2,
                                L2__PORT_NUMf, rep->new_dest.port);
        }

        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2_ENTRY_2m, &rep->new_mask2,
                                HITSAf, 1);
        }
    }
    
    /* Then work on External L2 */
    if (!soc_feature(unit, soc_feature_esm_support)) {
        return BCM_E_NONE;
    }

    /* First work on EXT_L2_ENTRY_1 */
    field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1, KEY_TYPEf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1, KEY_TYPEf,
                        rep->ext_key_type);

    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep->key_vfi != -1) {
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, VFIf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1, 
                                VFIf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1, 
                                VFIf, rep->key_vfi);
        } else {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                                VLAN_IDf, 0xfff);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                VLAN_IDf, rep->key_vlan);
        }
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                             MAC_ADDRf, mac_mask);
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                             MAC_ADDRf, rep->key_mac);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, DEST_TYPEf);
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                            DEST_TYPEf, (1 << field_len) - 1);
        if (rep->match_dest.vp != -1) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m,
                                             DESTINATIONf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                                DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                DESTINATIONf, rep->match_dest.vp);
        } else if (rep->match_dest.trunk != -1) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, TGIDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                                TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                TGIDf, rep->match_dest.trunk);
        } else {
            /* DEST_TYPEf == 0 */
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m,
                                             MODULE_IDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                                MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                MODULE_IDf, rep->match_dest.module);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, PORT_NUMf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1,
                                PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                                PORT_NUMf, rep->match_dest.port);
        }
    }

    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1, 
                            STATIC_BITf, 1);
        /* STATIC_BIT field in data is 0 */
    }

    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_mask1, PENDINGf,
                        1);
    if (rep->flags & BCM_L2_REPLACE_PENDING) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_match_data1,
                            PENDINGf, 1);
    }

    if (!(rep->flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) {
        if (rep->new_dest.vp != -1) {
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, DEST_TYPEf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_data1,
                                DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m,
                                             DESTINATIONf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_data1,
                                DESTINATIONf, rep->new_dest.vp);
        } else if (rep->new_dest.trunk != -1) {
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, DEST_TYPEf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_data1,
                                DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, TGIDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_data1,
                                TGIDf, rep->new_dest.trunk);
        } else if (rep->new_dest.port != -1 
                   && rep->new_dest.module != -1) {
            /* DEST_TYPEf == 0 */
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, DEST_TYPEf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                DEST_TYPEf, (1 << field_len) - 1);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m,
                                             MODULE_IDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_data1,
                                MODULE_IDf, rep->new_dest.module);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, PORT_NUMf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_data1,
                                PORT_NUMf, rep->new_dest.port);
        }

        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &rep->ext_new_mask1,
                                HITSAf, 1);
        }
    }

    /* Then work on EXT_L2_ENTRY_2 */
    field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, KEY_TYPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2, KEY_TYPEf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2, KEY_TYPEf,
                        rep->ext_key_type);

    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep->key_vfi != -1) {
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, VFIf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2, 
                                VFIf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2, 
                                VFIf, rep->key_vfi);
        } else {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                                VLAN_IDf, 0xfff);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                VLAN_IDf, rep->key_vlan);
        }
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                             MAC_ADDRf, mac_mask);
        soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                             MAC_ADDRf, rep->key_mac);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, DEST_TYPEf);
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                            DEST_TYPEf, (1 << field_len) - 1);
        if (rep->match_dest.vp != -1) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m,
                                             DESTINATIONf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                                DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                DESTINATIONf, rep->match_dest.vp);
        } else if (rep->match_dest.trunk != -1) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, TGIDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                                TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                TGIDf, rep->match_dest.trunk);
        } else {
            /* DEST_TYPEf == 0 */
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m,
                                             MODULE_IDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                                MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                MODULE_IDf, rep->match_dest.module);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, PORT_NUMf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                                PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                                PORT_NUMf, rep->match_dest.port);
        }
    }

    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2,
                            STATIC_BITf, 1);
        /* STATIC_BIT field in data is 0 */
    }

    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_mask2, PENDINGf,
                        1);
    if (rep->flags & BCM_L2_REPLACE_PENDING) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_match_data2,
                            PENDINGf, 1);
    }

    if (!(rep->flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) {
        if (rep->new_dest.vp != -1) {
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, DEST_TYPEf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_data2,
                                DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m,
                                             DESTINATIONf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                DESTINATIONf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_data2,
                                DESTINATIONf, rep->new_dest.vp);
        } else if (rep->new_dest.trunk != -1) {
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, DEST_TYPEf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                DEST_TYPEf, (1 << field_len) - 1);

            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_data2,
                                DEST_TYPEf, 1);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, TGIDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                TGIDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_data2,
                                TGIDf, rep->new_dest.trunk);
        } else if (rep->new_dest.port != -1 
                   && rep->new_dest.module != -1) {
            /* DEST_TYPEf == 0 */
            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, DEST_TYPEf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                DEST_TYPEf, (1 << field_len) - 1);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m,
                                             MODULE_IDf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                MODULE_IDf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_data2,
                                MODULE_IDf, rep->new_dest.module);

            field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m,
                                             PORT_NUMf);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                PORT_NUMf, (1 << field_len) - 1);
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_data2,
                                PORT_NUMf, rep->new_dest.port);
        }

        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &rep->ext_new_mask2,
                                HITSAf, 1);
        }
    }
    return BCM_E_NONE;
}

STATIC int
_soc_tr3_l2_sync_mem_cache(int unit, _soc_tr3_l2_replace_t *rep_st)
{
    uint8 *vmap, *vmap1;
    soc_mem_t mem = L2_ENTRY_1m, ov_mem = L2_ENTRY_2m;
    uint32 *cache, *entry, *match_data, *match_mask;
    uint32 *new_data, *new_mask;
    int index_max, idx, i;
    int entry_dw = soc_mem_entry_words(unit, mem);
    int entry_words = soc_mem_entry_words(unit, mem);
    int copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    
    /* First work on L2_ENTRY_1m */
    cache = SOC_MEM_STATE(unit, mem).cache[copyno];
    if (cache == NULL) {
        return BCM_E_NONE;
    }
    vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
    vmap1 = SOC_MEM_STATE(unit, ov_mem).vmap[copyno];
    index_max = soc_mem_index_max(unit, mem);
    for (idx = 0; idx <= index_max; idx++) {
        if (!CACHE_VMAP_TST(vmap, idx)) {
            continue;
        }
        entry = cache + (idx * entry_dw);
        if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
            continue;
        }
        if (!soc_mem_field32_get(unit, mem, entry, STATIC_BITf)) {
            continue;
        }
        match_data = (uint32 *)&rep_st->match_data1;
        match_mask = (uint32 *)&rep_st->match_mask1;
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        /* Match found, delete or replace entry */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Match found in L2_1 bulk cache op: %d\n"), idx));
        if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
            /* Delete op - invalidate the entry */
            CACHE_VMAP_CLR(vmap, idx);
            CACHE_VMAP_CLR(vmap1, idx/2);
        } else {
            /* Replace op - update entry */
            new_data = (uint32 *)&rep_st->new_data1;
            new_mask = (uint32 *)&rep_st->new_mask1;
            for (i = 0; i < entry_words; i++) {
                entry[i] ^= (entry[i] ^ new_data[i]) & new_mask[i];
            }
        }
    }
    
    /* Then work on L2_ENTRY_2m */
    mem = L2_ENTRY_2m;
    ov_mem = L2_ENTRY_1m;
    entry_dw = soc_mem_entry_words(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);
    copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    cache = SOC_MEM_STATE(unit, mem).cache[copyno];
    if (cache == NULL) {
        return BCM_E_NONE;
    }
    vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
    vmap1 = SOC_MEM_STATE(unit, ov_mem).vmap[copyno];
    index_max = soc_mem_index_max(unit, mem);
    for (idx = 0; idx <= index_max; idx++) {
        if (!CACHE_VMAP_TST(vmap, idx)) {
            continue;
        }
        entry = cache + (idx * entry_dw);
        if (!soc_mem_field32_get(unit, mem, entry, VALID_0f)) {
            continue;
        }
        if (!soc_mem_field32_get(unit, mem, entry, STATIC_BIT_0f)) {
            continue;
        }
        match_data = (uint32 *)&rep_st->match_data2;
        match_mask = (uint32 *)&rep_st->match_mask2;
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        /* Match found, delete or replace entry */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Match found in L2_2 bulk cache op: %d\n"), idx));
        if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
            /* Delete op - invalidate the entry */
            CACHE_VMAP_CLR(vmap, idx);
            CACHE_VMAP_CLR(vmap1, idx/2);
        } else {
            /* Replace op - update entry */
            new_data = (uint32 *)&rep_st->new_data2;
            new_mask = (uint32 *)&rep_st->new_mask2;
            for (i = 0; i < entry_words; i++) {
                entry[i] ^= (entry[i] ^ new_data[i]) & new_mask[i];
            }
        }
    }

    return BCM_E_NONE;
}

STATIC int
_is_soc_tr3_l2_mem_cache_enabled(int unit)
{
    soc_mem_t mem = L2_ENTRY_1m;
    uint32 *cache;
    int copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    
 
    cache = SOC_MEM_STATE(unit, mem).cache[copyno];
    if (cache == NULL) {
        return FALSE;
    }

    return TRUE;
}

STATIC int
_bcm_tr3_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                    bcm_module_t new_module, bcm_port_t new_port,
                    bcm_trunk_t new_trunk)
{
    _soc_tr3_l2_replace_t rep_st;
    int                   rv = BCM_E_UNAVAIL;
    int                   mode;

    sal_memset(&rep_st, 0, sizeof(_soc_tr3_l2_replace_t));

    /*Either of BCM_L2_REPLACE_DELETE or BCM_L2_REPLACE_AGE can bet set, but not both*/
    if ((flags & BCM_L2_REPLACE_DELETE) && (flags & BCM_L2_REPLACE_AGE )) {
        return BCM_E_PARAM;
    }

    rep_st.flags = flags;

    /*Set the detination in rep_st*/
    if (0 == (flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_replace_dest_setup
                            (unit, new_module, new_port, new_trunk,
                             flags & BCM_L2_REPLACE_NEW_TRUNK,
                             &rep_st.new_dest));
    }

    /*Set the match criteria in rep_st*/
    if (flags & BCM_L2_REPLACE_MATCH_DEST) {
        BCM_IF_ERROR_RETURN(_bcm_tr3_l2_replace_dest_setup
                            (unit, match_addr->modid, match_addr->port,
                             match_addr->tgid,
                             match_addr->flags & BCM_L2_TRUNK_MEMBER,
                             &rep_st.match_dest));
        if (rep_st.match_dest.vp != -1) {
            if (!((BCM_GPORT_IS_WLAN_PORT(match_addr->port))  ||
                  (BCM_GPORT_IS_NIV_PORT(match_addr->port))   ||
                  (BCM_GPORT_IS_TRILL_PORT(match_addr->port)) ||
                  (BCM_GPORT_IS_VLAN_PORT(match_addr->port)))) {
                    /* Note: For MPLS, MiM, L2GRE, VXLAN VFI types only */
                    rep_st.key_type = SOC_MEM_KEY_L2_ENTRY_1_L2_VFI;
                    rep_st.ext_key_type = SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI;
            }
        }
    }

    if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
        rep_st.key_vfi = -1;
        if (_BCM_VPN_VFI_IS_SET(match_addr->vid)) {
            _BCM_VPN_GET(rep_st.key_vfi, _BCM_VPN_TYPE_VFI, match_addr->vid);
        }
        if (rep_st.key_vfi != -1) {
            if (rep_st.key_vfi > soc_mem_index_max(unit, VFIm)) {
                return BCM_E_PARAM;
            }
            rep_st.key_type = SOC_MEM_KEY_L2_ENTRY_1_L2_VFI;
            rep_st.ext_key_type = SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI;
        } else {
            VLAN_CHK_ID(unit, match_addr->vid);
            rep_st.key_vlan = match_addr->vid;
        }
    }

    if (flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memcpy(&rep_st.key_mac, match_addr->mac, sizeof(bcm_mac_t));
            /* When replace L2 by MAC, key type must be provided. */
            /* Key type is 0 by default for L2 Bridge. */
            /* Here add support for explicitly assigned VFI type */
        if (flags & BCM_L2_REPLACE_VPN_TYPE) {
             rep_st.key_type      = SOC_MEM_KEY_L2_ENTRY_1_L2_VFI;  /* VFI Type */
             rep_st.ext_key_type  = SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI;  /*EXT VFI Type */
        }
    }

    if ((NULL == match_addr) && (flags & BCM_L2_REPLACE_VPN_TYPE)) {
        rep_st.key_type = SOC_MEM_KEY_L2_ENTRY_1_L2_VFI;
        rep_st.ext_key_type = SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_VFI;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr3_l2_replace_data_mask_setup(unit, &rep_st));

    mode = SOC_CONTROL(unit)->l2x_mode;
    /* Delete operation don't require freeze */
    if ((0 == (flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) ||
        mode != L2MODE_FIFO || _is_soc_tr3_l2_mem_cache_enabled(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_freeze(unit));
    }
    rv = _bcm_tr3_l2_replace_by_hw(unit, &rep_st);

    if (SOC_SUCCESS(rv) && (rep_st.flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        /* update the entries in mem cache with the bulk op results */
        rv = _soc_tr3_l2_sync_mem_cache(unit, &rep_st);
    }

    if ((0 == (flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) ||
        mode != L2MODE_FIFO || _is_soc_tr3_l2_mem_cache_enabled(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_thaw(unit));
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_replace
 * Purpose:
 *      Replace L2 entries matching given criterions
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      match_addr - (IN) <UNDEF>
 *      new_module - (IN) <UNDEF>
 *      new_port - (IN) <UNDEF>
 *      new_trunk - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_tr3_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                   bcm_module_t new_module, bcm_port_t new_port,
                   bcm_trunk_t new_trunk)
{
    BCM_TR3_L2_INIT(unit);

    if (!flags) {
        return BCM_E_PARAM;
    }
    if ((flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE)) &&
        (flags & BCM_L2_REPLACE_NEW_TRUNK)) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_L2_REPLACE_DELETE) && (flags & BCM_L2_REPLACE_MATCH_MAC) &&
        ((flags & BCM_L2_REPLACE_MATCH_UC) || (flags & BCM_L2_REPLACE_MATCH_MC))) {
        return BCM_E_PARAM;
    }

    /* matching L2 address allowed to be NULL together with */
    /* BCM_L2_REPLACE_DELETE/BCM_L2_REPLACE_AGE flag indicating to delete/age all entries. */
    if (NULL == match_addr) {
        if (!(flags & (BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_AGE))) {
            return BCM_E_PARAM;
        }
        flags &= ~(BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_MAC |
                   BCM_L2_REPLACE_MATCH_DEST);

        if (flags & BCM_L2_REPLACE_VLAN_AND_VPN_TYPE) {
            /* coverity[var_deref_model] */
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_l2_replace(unit, flags, match_addr, new_module,
                              new_port, new_trunk));
            flags ^= BCM_L2_REPLACE_VPN_TYPE;
        }
    }

    /*
     * COVERITY
     *
     * Coverity complains that the NULL pointer match_addr is dereferenced
     * in the below call.  But it is only dereferenced for one of the
     * three flags that are explicitly removed above.
     */ 
    /*    coverity[var_deref_model : FALSE]    */
    return _bcm_tr3_l2_replace(unit, flags, match_addr, new_module, new_port,
                               new_trunk);
}

/*
 * Function:
 *      _bcm_tr3_l2_addr_delete_mcast_by_hw
 * Purpose:
 *      Delete all L2 multicast entries by hardware bulk operation.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_tr3_l2_addr_delete_mcast_by_hw(int unit)
{
    bcm_mac_t mac_mask;
    uint32 rval;
    l2_bulk_entry_t l2_bulk;
    l2_entry_1_entry_t l2_entry_1;
    l2_entry_2_entry_t l2_entry_2;
    ext_l2_entry_1_entry_t ext_l2_entry_1;
    ext_l2_entry_2_entry_t ext_l2_entry_2;
    int field_len;

    sal_memset(&mac_mask, 0, sizeof(mac_mask));
    mac_mask[0] = 1; /* bit 40 */

    /* ************************ */
    /* First work on L2_ENTRY_1 */
    /* ************************ */

    /* Set match mask */
    sal_memset(&l2_entry_1, 0, sizeof(l2_entry_1));
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, VALIDf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, WIDEf, 1);
    field_len = soc_mem_field_length(unit, L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf,
                        (1 << field_len) - 1);
    soc_mem_mac_addr_set(unit, L2_ENTRY_1m, &l2_entry_1, L2__MAC_ADDRf,
            mac_mask);

    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk));

    /* Set match data */
    /* VALIDf is already set to 1 */
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, WIDEf, 0);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf, 
                        SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE);
    /* Bit 40 of L2__MAC_ADDRf is already set to 1 */ 
    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk));

    /* Set bulk control */
    rval = 0;
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_1m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));

    /* *********************** */
    /* Then work on L2_ENTRY_2 */
    /* *********************** */

    /* Set match mask */
    sal_memset(&l2_entry_2, 0, sizeof(l2_entry_2));
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, VALID_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, VALID_1f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, WIDE_0f, 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, WIDE_1f, 1);
    field_len = soc_mem_field_length(unit, L2_ENTRY_2m, KEY_TYPE_0f);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f,
                        (1 << field_len) - 1);
    soc_mem_mac_addr_set(unit, L2_ENTRY_2m, &l2_entry_2, L2__MAC_ADDRf,
            mac_mask);

    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk));

    /* Set match data */
    /* VALID_0f and VALID_1f are already set to 1 */
    /* WIDE_0f and WIDE_1f are already set to 1 */
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f, 
                            SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
    soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f, 
                            SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
    /* Bit 40 of L2__MAC_ADDRf is already set to 1 */ 
    sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk));

    /* Set bulk control */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_2m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    
    /* *************************** */
    /* Next work on EXT_L2_ENTRY_1 */
    /* *************************** */

    if (!soc_feature(unit, soc_feature_esm_support)) {
        return BCM_E_NONE;
    }

    /* Set match mask */
    sal_memset(&ext_l2_entry_1, 0, sizeof(ext_l2_entry_1));
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, FREEf, 1); 
    field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, KEY_TYPEf, 
                        (1 << field_len) - 1);
    soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, MAC_ADDRf,
            mac_mask);

    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk));

    /* Set match data */
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, FREEf, 0);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, KEY_TYPEf, 
                            SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE);
    /* Bit 40 of MAC_ADDRf is already set to 1 */ 
    sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk));

    /* Set bulk control */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
    /* Issue dummy op */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
    
    /* ****************************** */
    /* Finally work on EXT_L2_ENTRY_2 */
    /* ****************************** */

    /* Set match mask */
    sal_memset(&ext_l2_entry_2, 0, sizeof(ext_l2_entry_2));
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, FREEf, 1); 
    field_len = soc_mem_field_length(unit, EXT_L2_ENTRY_2m, KEY_TYPEf);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, KEY_TYPEf, 
                        (1 << field_len) - 1);
    soc_mem_mac_addr_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, MAC_ADDRf,
            mac_mask);

    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk));

    /* Set match data */
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, FREEf, 0);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, KEY_TYPEf, 
                            SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE);
    /* Bit 40 of MAC_ADDRf is already set to 1 */ 
    sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
    SOC_IF_ERROR_RETURN(WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk));

    /* Set bulk control */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
    /* Issue dummy op */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
    SOC_IF_ERROR_RETURN(WRITE_L2_BULK_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr));
    SOC_IF_ERROR_RETURN(WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr3_l2_addr_delete_mcast_by_sw
 * Purpose:
 *      Delete all L2 multicast entries by traversing all L2 tables.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_tr3_l2_addr_delete_mcast_by_sw(int unit)
{
    int             rv = BCM_E_NONE;
    int             buf_size, chunksize;
    uint32          *l2_tbl_chnk;
    soc_mem_t       mem;
    int             chnk_idx, chnk_idx_max, mem_idx_max;
    int             modified;
    int             chnk_end, ent_idx;
    uint32          *l2_entry;
    int             key_type;
    bcm_mac_t       mac_addr;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
            L2_MEM_CHUNKS_DEFAULT);
    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 mcast delete");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    /* First work on L2_ENTRY_1 */

    mem = L2_ENTRY_1m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        modified = FALSE;
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPEf);
            if (SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, L2__MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            sal_memcpy(l2_entry, soc_mem_entry_null(unit, mem),
                    sizeof(l2_entry_1_entry_t));
            modified = TRUE;
        }

        if (modified) {
            rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                    chnk_idx, chnk_idx_max, l2_tbl_chnk);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, l2_tbl_chnk);
        return rv;  
    }

    /* Then work on L2_ENTRY_2 */

    mem = L2_ENTRY_2m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        modified = FALSE;
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALID_0f)) {
                continue;
            }
            if (!soc_mem_field32_get(unit, mem, l2_entry, VALID_1f)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPE_0f);
            if (SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, L2__MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            sal_memcpy(l2_entry, soc_mem_entry_null(unit, mem),
                    sizeof(l2_entry_2_entry_t));
            modified = TRUE;
        }

        if (modified) {
            rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                    chnk_idx, chnk_idx_max, l2_tbl_chnk);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, l2_tbl_chnk);
        return rv;  
    }

    /* Next work on EXT_L2_ENTRY_1 */

    if (!soc_feature(unit, soc_feature_esm_support)) {
        return BCM_E_NONE;
    }

    mem = EXT_L2_ENTRY_1m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        modified = FALSE;
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (soc_mem_field32_get(unit, mem, l2_entry, FREEf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPEf);
            if (SOC_MEM_KEY_EXT_L2_ENTRY_1_L2_BRIDGE != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            sal_memcpy(l2_entry, soc_mem_entry_null(unit, mem),
                    sizeof(ext_l2_entry_1_entry_t));
            modified = TRUE;
        }

        if (modified) {
            rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                    chnk_idx, chnk_idx_max, l2_tbl_chnk);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, l2_tbl_chnk);
        return rv;  
    }

    /* Finally work on EXT_L2_ENTRY_2 */

    mem = EXT_L2_ENTRY_2m;
    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
            chnk_idx <= mem_idx_max; 
            chnk_idx += chunksize) {

        sal_memset((void *)l2_tbl_chnk, 0, buf_size);
        chnk_idx_max = ((chnk_idx + chunksize) <= mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (BCM_FAILURE(rv)) {
            break;
        }

        modified = FALSE;
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                    l2_tbl_chnk, ent_idx);

            if (soc_mem_field32_get(unit, mem, l2_entry, FREEf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry,
                    KEY_TYPEf);
            if (SOC_MEM_KEY_EXT_L2_ENTRY_2_L2_BRIDGE != key_type) {
                continue;
            }

            soc_mem_mac_addr_get(unit, mem, l2_entry, MAC_ADDRf, mac_addr);
            if (!BCM_MAC_IS_MCAST(mac_addr)) {
                continue;
            }

            sal_memcpy(l2_entry, soc_mem_entry_null(unit, mem),
                    sizeof(ext_l2_entry_2_entry_t));
            modified = TRUE;
        }

        if (modified) {
            rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                    chnk_idx, chnk_idx_max, l2_tbl_chnk);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    soc_cm_sfree(unit, l2_tbl_chnk);
    return rv;        
}

/*
 * Function:
 *      bcm_tr3_l2_addr_delete_mcast
 * Purpose:
 *      Delete all L2 multicast entries.
 * Parameters:
 *      unit - (IN) Unit number.
 *      bulk - (IN) Hardware bulk delete indication.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_tr3_l2_addr_delete_mcast(int unit, int bulk)
{
    int rv, seconds, enabled;

    SOC_IF_ERROR_RETURN
        (SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled));
    if (enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
    }
    _BCM_ALL_L2X_MEM_LOCK(unit);

    if (bulk) {
        rv = _bcm_tr3_l2_addr_delete_mcast_by_hw(unit);
    } else {
        rv = _bcm_tr3_l2_addr_delete_mcast_by_sw(unit);
    }

    _BCM_ALL_L2X_MEM_UNLOCK(unit);
    if(enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
    }
    return rv;
}

/*
 * Function:
 *      bcm_tr3_cross_connect_add
 * Purpose:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 *      port_1     - First port in the cross-connect
 *      port_2     - Second port in the cross-connect
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr3_l2_cross_connect_add(int unit, bcm_vlan_t outer_vlan, 
                            bcm_vlan_t inner_vlan, bcm_gport_t port_1, 
                            bcm_gport_t port_2)
{
    _bcm_tr3_l2_entries_t l2_entries, l2_entries_lookup;
    int rv, gport_id;
    bcm_port_t port_out;
    bcm_module_t mod_out;
    bcm_trunk_t trunk_id;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&l2_entries, 0, sizeof (l2_entries));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_L2_ENTRY_1;
    if ((outer_vlan < BCM_VLAN_DEFAULT) || (outer_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    } else if (inner_vlan == BCM_VLAN_INVALID) {
        /* Single cross-connect (use only outer_vid) */
        soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), 
                                    KEY_TYPEf, 
                                    SOC_MEM_KEY_L2_ENTRY_1_VLAN_SINGLE_CROSS_CONNECT);
    } else {
        if ((inner_vlan < BCM_VLAN_DEFAULT) || (inner_vlan > BCM_VLAN_MAX)) {
            return BCM_E_PARAM;
        }
        /* Double cross-connect (use both outer_vid and inner_vid) */
        soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), 
                                    KEY_TYPEf, 
                                    SOC_MEM_KEY_L2_ENTRY_1_VLAN_DOUBLE_CROSS_CONNECT);
        soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), VLAN__IVIDf, inner_vlan);
    }
    soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), STATIC_BITf, 1);
    soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), VALIDf, 1);
    soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), VLAN__OVIDf, outer_vlan);

    /* See if the entry already exists */
    sal_memset(&l2_entries_lookup, 0, sizeof (l2_entries_lookup));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_L2_ENTRY_1;
    rv = _bcm_tr3_l2_entries_lookup(unit, &l2_entries, &l2_entries_lookup);
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
         return rv;
    } 

    /* Resolve first port */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port_1, &mod_out, &port_out, &trunk_id,
                                &gport_id));
    if (BCM_GPORT_IS_TRUNK(port_1)) {
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DEST_TYPE_0f, 1);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__TGID_0f, trunk_id);
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port_1)
               || BCM_GPORT_IS_VLAN_PORT(port_1)) {
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DEST_TYPE_0f, 0x2);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DESTINATION_0f, gport_id);
    } else {
        if ((mod_out == -1) ||(port_out == -1)) {
            return BCM_E_PORT;
        }
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DEST_TYPE_0f, 0);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__MODULE_ID_0f, mod_out);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__PORT_NUM_0f, port_out);
    }

    /* Resolve second port */
    BCM_IF_ERROR_RETURN 
        (_bcm_esw_gport_resolve(unit, port_2, &mod_out, &port_out, &trunk_id,
                                &gport_id));
    if (BCM_GPORT_IS_TRUNK(port_2)) {
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DEST_TYPE_1f, 1);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__TGID_1f, trunk_id);
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(port_2)
               || BCM_GPORT_IS_VLAN_PORT(port_2)) {
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DEST_TYPE_1f, 0x2);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DESTINATION_1f, gport_id);
    } else {
        if ((mod_out == -1) ||(port_out == -1)) {
            return BCM_E_PORT;
        }
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__DEST_TYPE_1f, 0);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__MODULE_ID_1f, mod_out);
        soc_L2_ENTRY_1m_field32_set(unit, 
                                    &(l2_entries.l2_entry_1), 
                                    VLAN__PORT_NUM_1f, port_out);
    }

    rv = BCM_E_FULL;
    rv = soc_mem_insert_return_old(unit, L2_ENTRY_1m, MEM_BLOCK_ANY, 
                                       (void *)&(l2_entries.l2_entry_1),
                                       (void *)&(l2_entries.l2_entry_1));
    if (rv == BCM_E_FULL) {
        rv = _bcm_tr3_l2_hash_dynamic_replace(unit, &l2_entries);
    } 
    if (BCM_SUCCESS(rv)) {
        if (soc_feature(unit, soc_feature_ppa_bypass)) {
            SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_cross_connect_delete
 * Purpose:
 *      Delete a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr3_l2_cross_connect_delete(int unit, bcm_vlan_t outer_vlan, 
                               bcm_vlan_t inner_vlan)
{
    _bcm_tr3_l2_entries_t l2_entries, l2_entries_lookup;
    int mb_index;
    int rv;

    BCM_TR3_L2_INIT(unit);

    sal_memset(&l2_entries, 0, sizeof (l2_entries));
    l2_entries.entry_flags = _BCM_TR3_L2_SELECT_L2_ENTRY_1;
    if ((outer_vlan < BCM_VLAN_DEFAULT) || (outer_vlan > BCM_VLAN_MAX)) {
        return BCM_E_PARAM;
    } else if (inner_vlan == BCM_VLAN_INVALID) {
        /* Single cross-connect (use only outer_vid) */
        soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), 
                                    KEY_TYPEf, 
                                    SOC_MEM_KEY_L2_ENTRY_1_VLAN_SINGLE_CROSS_CONNECT);
    } else {
        if ((inner_vlan < BCM_VLAN_DEFAULT) || (inner_vlan > BCM_VLAN_MAX)) {
            return BCM_E_PARAM;
        }
        /* Double cross-connect (use both outer_vid and inner_vid) */
        soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), 
                                    KEY_TYPEf, 
                                    SOC_MEM_KEY_L2_ENTRY_1_VLAN_DOUBLE_CROSS_CONNECT);

        soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), VLAN__IVIDf, inner_vlan);
    }

    soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), STATIC_BITf, 1);
    soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), VALIDf, 1);
    soc_L2_ENTRY_1m_field32_set(unit, &(l2_entries.l2_entry_1), VLAN__OVIDf, outer_vlan);

    _BCM_ALL_L2X_MEM_LOCK(unit);
    rv = _bcm_tr3_l2_entries_lookup(unit, (void *)&l2_entries,
                                    (void *)&l2_entries_lookup);

    if (BCM_E_NONE != rv) {
        _BCM_ALL_L2X_MEM_UNLOCK(unit);
        return rv;
    }

    if (!SOC_L2X_GROUP_ENABLE_GET(unit)) {
        mb_index = _bcm_tr3_l2hw_entries_field32_get(unit, &l2_entries_lookup,
                            _BCM_TR3_L2_MEMACC_MAC_BLOCK_INDEX);
        _bcm_tr3_mac_block_delete(unit, mb_index);
    }

    rv = _bcm_tr3_l2_entries_delete(unit, &l2_entries_lookup);
    _BCM_ALL_L2X_MEM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_cross_connect_delete_all
 * Purpose:
 *      Delete all VLAN cross connect entries
 * Parameters:
 *      unit       - Device unit number
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr3_l2_cross_connect_delete_all(int unit)
{
    int index_min, index_max, index, mem_max;
    l2_entry_1_entry_t *l2_entry_1;
    int rv = BCM_E_NONE;
    int *buffer = NULL;
    int mem_size, idx, chnk_end;
    int chunksize, modified;
    soc_mem_t mem = L2_ENTRY_1m;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    index_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);
    mem_size =  chunksize * sizeof(l2_entry_only_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "L2_ENTRY_1_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2_ENTRY_1m);
    for (idx = index_min; idx <= mem_max; idx += chunksize) {
        index_max = idx + chunksize - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return rv;
        }
        modified = 0;
        chnk_end = index_max - idx;
        for (index = 0; index <= chnk_end; index++) {
            l2_entry_1 =
                soc_mem_table_idx_to_pointer(unit, mem, l2_entry_1_entry_t *,
                                             buffer, index);
            if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VALIDf) &&
                ((soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, KEY_TYPEf) ==
                                  SOC_MEM_KEY_L2_ENTRY_1_VLAN_SINGLE_CROSS_CONNECT) ||
                 (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, KEY_TYPEf) ==
                                  SOC_MEM_KEY_L2_ENTRY_1_VLAN_DOUBLE_CROSS_CONNECT))) {
                sal_memcpy(l2_entry_1, soc_mem_entry_null(unit, mem),
                           sizeof(l2_entry_1_entry_t));
                modified = 1;
            }
        }
        if (modified) {
            if ((rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                         idx, index_max, buffer)) < 0) {
                soc_cm_sfree(unit, buffer);
                soc_mem_unlock(unit, L2_ENTRY_1m);
                return rv;
            }
        }
    }

    if (SOC_CONTROL(unit)->arlShadow != NULL) {
        sal_mutex_take(SOC_CONTROL(unit)->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(SOC_CONTROL(unit)->arlShadow);
        sal_mutex_give(SOC_CONTROL(unit)->arlShadowMutex);
    }
    soc_cm_sfree(unit, buffer);
    soc_mem_unlock(unit, L2_ENTRY_1m);
    
    return rv;
}

/*
 * Function:
 *      bcm_tr3_l2_cross_connect_traverse
 * Purpose:
 *      Walks through the valid cross connect entries and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_tr3_l2_cross_connect_traverse(int unit,
                                 bcm_vlan_cross_connect_traverse_cb cb,
                                 void *user_data)
{
    int index_min, index_max, index, mem_max;
    l2_entry_1_entry_t *l2_entry_1;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx, chunksize, chnk_end;
    bcm_gport_t port_1 = BCM_GPORT_INVALID, port_2 = BCM_GPORT_INVALID;
    bcm_vlan_t outer_vlan, inner_vlan;
    bcm_port_t port_in, port_out;
    bcm_module_t mod_in, mod_out;
    soc_mem_t mem = L2_ENTRY_1m;

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    index_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);
    mem_size =  chunksize * sizeof(l2_entry_1_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "cross connect traverse");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }
    sal_memset((void *)buffer, 0, mem_size);
    
    soc_mem_lock(unit, L2_ENTRY_1m);
    for (idx = index_min; idx <= mem_max; idx += chunksize) {
        index_max = idx + chunksize - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2_ENTRY_1m);
            return rv;
        }
        chnk_end = index_max - idx;
        for (index = 0; index <= chnk_end ; index++) {
            l2_entry_1 = 
                soc_mem_table_idx_to_pointer(unit, mem,
                                             l2_entry_1_entry_t *, buffer, index);
            if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VALIDf)) {
                if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, KEY_TYPEf) ==
                                     SOC_MEM_KEY_L2_ENTRY_1_VLAN_DOUBLE_CROSS_CONNECT) {
                    /* Double cross-connect entry */
                    inner_vlan = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__IVIDf);
                } else if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, KEY_TYPEf) ==
                                     SOC_MEM_KEY_L2_ENTRY_1_VLAN_SINGLE_CROSS_CONNECT) {
                    /* Single cross-connect entry */
                    inner_vlan = BCM_VLAN_INVALID;
                } else {
                    /* Not a cross-connect entry, ignore */
                    continue;
                }
                outer_vlan = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__OVIDf);

                /* Get first port params */
                if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DEST_TYPE_0f) == 2) {
                    int vpg;
                    vpg = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DESTINATION_0f);
                    BCM_GPORT_SUBPORT_GROUP_SET(port_1, vpg);
                } else if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DEST_TYPE_0f) == 1) {
                    BCM_GPORT_TRUNK_SET(port_1, 
                        soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__TGID_0f));
                } else if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DEST_TYPE_0f) == 0) {
                    port_in = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__PORT_NUM_0f);
                    mod_in = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__MODULE_ID_0f);
                    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                 mod_in, port_in, &mod_out, &port_out);
                    if (BCM_FAILURE(rv)) {
                        soc_cm_sfree(unit, buffer);
                        soc_mem_unlock(unit, L2_ENTRY_1m);
                        return rv;
                    }
                    BCM_GPORT_MODPORT_SET(port_1, mod_out, port_out);
                }

                /* Get second port params */
                if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DEST_TYPE_1f) == 2) {
                    int vpg;
                    vpg = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DESTINATION_1f);
                    BCM_GPORT_SUBPORT_GROUP_SET(port_2, vpg);
                } else if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DEST_TYPE_1f) == 1) {
                    BCM_GPORT_TRUNK_SET(port_2, soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__TGID_1f));
                } else if (soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__DEST_TYPE_1f) == 0) {
                    port_in = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__PORT_NUM_1f);
                    mod_in = soc_L2_ENTRY_1m_field32_get(unit, l2_entry_1, VLAN__MODULE_ID_1f);
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                mod_in, port_in, &mod_out, &port_out));
                    BCM_GPORT_MODPORT_SET(port_2, mod_out, port_out);
                }

                /* Call application call-back */
                rv = cb(unit, outer_vlan, inner_vlan, port_1, port_2, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    soc_cm_sfree(unit, buffer);
                    soc_mem_unlock(unit, L2_ENTRY_1m);
                    return rv;
                }
#endif
            }
        }
    }
    soc_cm_sfree(unit, buffer);
    soc_mem_unlock(unit, L2_ENTRY_1m);

    return BCM_E_NONE;
}

int
bcm_tr3_failover_ring_config_set(int unit, bcm_failover_ring_t *failover_ring)
{    
    int match_type = -1, i, rv, gport_id;
    uint32 map_size, rval;
    SHR_BITDCL *vlan_map = NULL, *vpn_map = NULL;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    /*
    * match_type = -1 means forced_flooding/learn_disabling based on physical 
    *              port or VP
    * match_type = 0 means forced_flooding/learn_disabling based on VLAN or 
    *              port + VLAN
    * match_type = 1 means forced_flooding/learn_disabling based on VPN or 
    *              VP + VPN
    */
    if (!failover_ring) {
        return BCM_E_PARAM;
    }
    /*
    * If only one of the ports(port0 or port1) are valid, then we have to write 
    * that port value into both FLOOD_LEARN_MATCH_PORT_Ar and 
    * FLOOD_LEARN_MATCH_PORT_Br due to hardware behavior.
    */
    if ((failover_ring->port0 != BCM_GPORT_INVALID) && 
        (failover_ring->port1 == BCM_GPORT_INVALID)) {
        failover_ring->port1 = failover_ring->port0;   
    }
    if ((failover_ring->port1 != BCM_GPORT_INVALID) && 
        (failover_ring->port0 == BCM_GPORT_INVALID)) {
        failover_ring->port0 = failover_ring->port1;   
    }
    /* Configure h/w tables and memory */
    if (failover_ring->flags & 
        (BCM_FAILOVER_LOOKUP_DISABLE | BCM_FAILOVER_LEARN_DISABLE)) {
        if(failover_ring->port0 != BCM_GPORT_INVALID) {
           BCM_IF_ERROR_RETURN
               (_bcm_esw_gport_resolve(unit, failover_ring->port0, &mod_out, 
                                       &port_out, &trunk_id, &gport_id));
           SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_MATCH_PORT_Ar(unit, &rval));
           if (-1 != gport_id) {
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval,
                                 DESTINATIONf, gport_id);
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval,
                                 DEST_TYPEf, 2);
           } else if (BCM_TRUNK_INVALID != trunk_id) {
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval, 
                                 DESTINATIONf, trunk_id);
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval,
                                 DEST_TYPEf, 1);
           } else if (!(-1 == port_out || -1 == mod_out)) {
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval, 
                                 PORT_NUMf, port_out);
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval, 
                                 MODULE_IDf, mod_out);
               soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Ar, &rval,
                                 DEST_TYPEf, 0);
           } 
           SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_MATCH_PORT_Ar(unit, rval));
        }
        if (failover_ring->port1 != BCM_GPORT_INVALID) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, failover_ring->port1, &mod_out, 
                                        &port_out, &trunk_id, &gport_id));
            SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_MATCH_PORT_Br(unit, &rval));
            if (-1 != gport_id) {
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval,
                                  DESTINATIONf, gport_id);
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval,
                                  DEST_TYPEf, 2);
            } else if (BCM_TRUNK_INVALID != trunk_id) {
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval, 
                                  DESTINATIONf, trunk_id);
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval,
                                  DEST_TYPEf, 1);
            } else if (!(-1 == port_out || -1 == mod_out)) {
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval, 
                                  PORT_NUMf, port_out);
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval, 
                                  MODULE_IDf, mod_out);
                soc_reg_field_set(unit, FLOOD_LEARN_MATCH_PORT_Br, &rval,
                                  DEST_TYPEf, 0);
            }
            SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_MATCH_PORT_Br(unit, rval));
        }
        /* 
        * KT2 and TR3 have hardware restriction that forced_flooding / 
        * learn_disabling can be set only for all VPNs and not for set  
        * of VPNs. Hence, vpn_vector will be used only for future  
        * devices which overcome this hardware restriction.
        */
        if (failover_ring->flags & BCM_FAILOVER_VPN_TYPE) {
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            if (SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit)) {
                match_type = 1;
            } else
#endif	
            {
                map_size = SHR_BITALLOCSIZE(BCM_VPN_MAX+1);
                vpn_map = soc_cm_salloc(unit, map_size, "Failover ring vpn map");
                if (NULL == vpn_map) {
                    return (SOC_E_MEMORY);
                }
                sal_memset(vpn_map, 0, map_size);
                for (i = BCM_VPN_MIN; i < BCM_VPN_MAX; i++) {
                     if (BCM_VPN_VEC_GET(failover_ring->vpn_vector, i)) {
                         SHR_BITSET(vpn_map, i);
                         match_type = 1;  
                     }
                }
                if (match_type == 1) {
                    if ((rv = soc_mem_write_range(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am,
                                                  MEM_BLOCK_ALL, 0, soc_mem_index_max( 
                                                  unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am), 
                                                  vpn_map)) < 0) {
                        soc_cm_sfree(unit, vpn_map);
                        return rv;
                    }
                    if ((rv = soc_mem_write_range(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Bm,
                                                  MEM_BLOCK_ALL, 0, soc_mem_index_max( 
                                                  unit, FLOOD_LEARN_MATCH_VLANS_PORT_Bm), 
                                                  vpn_map)) < 0) {
                        soc_cm_sfree(unit, vpn_map);
                        return rv;
                    }
                }    
                soc_cm_sfree(unit, vpn_map);
            }
            if (match_type == 1) {
                /*
                * As we are writing vlan_vector/vpn_vector into both 
                * FLOOD_LEARN_MATCH_VLANS_PORT_Am and 
                * FLOOD_LEARN_MATCH_VLANS_PORT_Bm, we have to set key_type into 
                * both FLOOD_LEARN_KEY_TYPE_PORT_Ar and 
                * FLOOD_LEARN_KEY_TYPE_PORT_Br. 
                */
                SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, 
                                    &rval));
#ifdef BCM_KATANA2_SUPPORT        
                if (SOC_IS_KATANA2(unit)) {
                    soc_reg_field_set(unit, FLOOD_LEARN_KEY_TYPE_PORT_Ar, &rval, 
                                      KEY_TYPEf, KT2_L2_HASH_KEY_TYPE_VFI);
                }
#endif  
                if (SOC_IS_TRIUMPH3(unit)) {
                    soc_reg_field_set(unit, FLOOD_LEARN_KEY_TYPE_PORT_Ar, &rval, 
                                      KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_1_L2_VFI);
                }
                SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, 
                                    rval));
           
                SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_KEY_TYPE_PORT_Br(unit, 
                                    &rval));
#ifdef BCM_KATANA2_SUPPORT        
                if (SOC_IS_KATANA2(unit)) {
                    soc_reg_field_set(unit, FLOOD_LEARN_KEY_TYPE_PORT_Br, &rval,
                                      KEY_TYPEf, KT2_L2_HASH_KEY_TYPE_VFI);
                }
#endif
                if (SOC_IS_TRIUMPH3(unit)) {
                    soc_reg_field_set(unit, FLOOD_LEARN_KEY_TYPE_PORT_Br, &rval,
                                      KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_1_L2_VFI);
                }
                SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Br(unit, 
                                    rval));
            }
        } else {
            map_size = SHR_BITALLOCSIZE(BCM_VLAN_MAX+1);
            vlan_map = soc_cm_salloc(unit, map_size, "Failover ring vlan map");
            if (NULL == vlan_map) {
                return (SOC_E_MEMORY);
            }
            sal_memset(vlan_map, 0, map_size);
            for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
                 if (BCM_VLAN_VEC_GET(failover_ring->vlan_vector, i)) {
                     SHR_BITSET(vlan_map, i);
                     match_type = 0;
                 }
            }
            if ((rv = soc_mem_write_range(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am,
                                          MEM_BLOCK_ALL, 0, soc_mem_index_max( 
                                          unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am), 
                                          vlan_map)) < 0) {
                 soc_cm_sfree(unit, vlan_map);
                 return rv;
            }
            if ((rv = soc_mem_write_range(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Bm,
                                          MEM_BLOCK_ALL, 0, soc_mem_index_max(
                                          unit, FLOOD_LEARN_MATCH_VLANS_PORT_Bm), 
                                          vlan_map)) < 0) {
                 soc_cm_sfree(unit, vlan_map);
                 return rv;
            }
            soc_cm_sfree(unit, vlan_map);
        }

        SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_CONTROLr(unit, &rval));
        if (failover_ring->flags & BCM_FAILOVER_LOOKUP_DISABLE) {
            soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, FLOOD_FORCEf, 
                              1);
        }
        if (failover_ring->flags & BCM_FAILOVER_LEARN_DISABLE) {
            soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, LEARN_DISABLEf, 
                              1);
        }
        if (match_type != -1) {
            soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                              MATCH_KEY_TYPE_PORT_Af, 1);
            soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                              MATCH_KEY_TYPE_PORT_Bf, 1);
        }
        if ((failover_ring->port0 != BCM_GPORT_INVALID) || 
            (failover_ring->port1 != BCM_GPORT_INVALID)) {
            /*
            * In KT2 and TR3, as we do forced_flooding/learn_disabling for all 
            * VPNs irrespective of VPs, we would not set MATCH_PORT_Af and 
            * MATCH_PORT_Bf if match_type is 1.
            */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            if ((SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit)) && 
                (match_type != 1))
#endif				
            {             
                 soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                                   MATCH_PORT_Af, 1);
                 soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                                   MATCH_PORT_Bf, 1);
            }
        }
        if (match_type != -1) {
            /*
            * In KT2 and TR3, as we would not consider vpn_vector due to
            * hardware restriction, we won't set MATCH_VLANS_PORT_Af and
            * MATCH_VLANS_PORT_Bf if match_type is 1.
            */
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            if ((SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit)) && 
                (match_type != 1))  
#endif		
            {	
                 soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                                   MATCH_VLANS_PORT_Af, 1);
                 soc_reg_field_set(unit, FLOOD_LEARN_CONTROLr, &rval, 
                                   MATCH_VLANS_PORT_Bf, 1);
            }
        }			
        SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_CONTROLr(unit, rval));
    } else if (failover_ring->flags & BCM_FAILOVER_CLEAR) {
        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_MATCH_PORT_Ar(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_MATCH_PORT_Br(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_FLOOD_LEARN_KEY_TYPE_PORT_Br(unit, rval));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am, COPYNO_ALL, 
                           0));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Bm, COPYNO_ALL, 
                           0));
    }
    return BCM_E_NONE;
}

int
bcm_tr3_failover_ring_config_get(int unit, bcm_failover_ring_t *failover_ring)
{
    int i, rv, vp, dest_type = -1;
    uint8 no_clear = 0;
    _bcm_gport_dest_t dest;
    SHR_BITDCL *vlan_map = NULL, *vpn_map = NULL;
    uint32 map_size, rval, is_vfi;

    COMPILER_REFERENCE(vp);
 
    if (!failover_ring) {
        return BCM_E_PARAM;
    }
    bcm_failover_ring_t_init(failover_ring);
    SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_KEY_TYPE_PORT_Ar(unit, &rval));
    is_vfi = soc_reg_field_get(unit, FLOOD_LEARN_KEY_TYPE_PORT_Ar, rval, 
                               KEY_TYPEf);
    /*
    * is_vfi = zero means KEY_TYPE is VLAN
    * is_vfi = non-zero value means KEY_TYPE is VFI
    */
    if (is_vfi) {
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if (!(SOC_IS_KATANA2(unit) || SOC_IS_TRIUMPH3(unit)))
#endif
        {
            /* 
            * KT2 && TR3 have hardware restriction that forced_flooding / 
            * learn_disabling can be set only for all VPNs and not for set of 
            * VPNs. Hence, vpn_vector will be used only for future devices  
            * which overcome this hardware restriction.
            */
            /* Construct vpn vector */
            map_size = SHR_BITALLOCSIZE(BCM_VPN_MAX+1);
            vpn_map = soc_cm_salloc(unit, map_size, "Failover ring vpn map");
            if (NULL == vpn_map) {
                return (SOC_E_MEMORY);
            }
            sal_memset(vpn_map, 0, map_size);
            if ((rv = soc_mem_read_range(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am,
                                         MEM_BLOCK_ALL, 0, soc_mem_index_max(unit, 
                                         FLOOD_LEARN_MATCH_VLANS_PORT_Am), 
                                         vpn_map)) < 0) {
                soc_cm_sfree(unit, vpn_map);
                return rv;
            }
            for (i = BCM_VPN_MIN; i < BCM_VPN_MAX; i++) {
                if (BCM_VPN_VEC_GET(vpn_map, i)) {
                    SHR_BITSET(failover_ring->vpn_vector, i);
                }
            }
            soc_cm_sfree(unit, vpn_map);
        }	
    } else {
        /* Construct vlan vector */
        map_size = SHR_BITALLOCSIZE(BCM_VLAN_MAX+1);
        vlan_map = soc_cm_salloc(unit, map_size, "Failover ring vlan map");
        if (NULL == vlan_map) {
            return (SOC_E_MEMORY);
        }
        sal_memset(vlan_map, 0, map_size);
        if ((rv = soc_mem_read_range(unit, FLOOD_LEARN_MATCH_VLANS_PORT_Am,
                                     MEM_BLOCK_ALL, 0, soc_mem_index_max(unit, 
                                     FLOOD_LEARN_MATCH_VLANS_PORT_Am), 
                                     vlan_map)) < 0) {
            soc_cm_sfree(unit, vlan_map);
            return rv;
        }
        for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
            if (BCM_VLAN_VEC_GET(vlan_map, i)) {
                SHR_BITSET(failover_ring->vlan_vector, i);
            }
        }
        soc_cm_sfree(unit, vlan_map);
    }
    /* Construct flags */    
    SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_CONTROLr(unit, &rval));
    if (soc_reg_field_get(unit, FLOOD_LEARN_CONTROLr, rval, FLOOD_FORCEf)) {
        failover_ring->flags |= BCM_FAILOVER_LOOKUP_DISABLE;
        no_clear++;
    }
    if (soc_reg_field_get(unit, FLOOD_LEARN_CONTROLr, rval, LEARN_DISABLEf)) {
        failover_ring->flags |= BCM_FAILOVER_LEARN_DISABLE;
        no_clear++;
    }
    if (is_vfi) {
        failover_ring->flags |= BCM_FAILOVER_VPN_TYPE;
    }
    if (no_clear == 0) {
        failover_ring->flags |= BCM_FAILOVER_CLEAR;
    }
    /* Construct gport info */
    _bcm_gport_dest_t_init(&dest);
    SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_MATCH_PORT_Ar(unit, &rval));
    if (rval) {
        dest_type = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Ar, rval, 
                                      DEST_TYPEf);
    } else {
        dest_type = -1;
    }    
    if (dest_type == 0) {
        dest.port = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Ar, rval, 
                                      PORT_NUMf);
        dest.modid = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Ar, rval, 
                                       MODULE_IDf);
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
    } else if (dest_type == 1) {
        dest.tgid = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Ar, rval, 
                                      DESTINATIONf);
        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
    } else if (dest_type == 2) {
        vp = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Ar, rval, 
                               DESTINATIONf);
#if defined(INCLUDE_L3)    
        if ((rv = _bcm_vp_gport_dest_fill(unit, vp, &dest)) < 0) {
            return rv;
        }  
#endif        
    }
    if (dest_type != -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_construct(unit, &dest, &failover_ring->port0));
    }        
    _bcm_gport_dest_t_init(&dest);
    SOC_IF_ERROR_RETURN(READ_FLOOD_LEARN_MATCH_PORT_Br(unit, &rval));
    if (rval) {
        dest_type = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Br, rval, 
                                      DEST_TYPEf);
    } else {
        dest_type = -1;
    }    
    if (dest_type == 0) {
        dest.port = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Br, rval, 
                                      PORT_NUMf);
        dest.modid = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Br, rval, 
                                       MODULE_IDf);
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
    } else if (dest_type == 1) {
        dest.tgid = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Br, rval, 
                                      DESTINATIONf);
        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
    } else if (dest_type == 2) {
        vp = soc_reg_field_get(unit, FLOOD_LEARN_MATCH_PORT_Br, rval, 
                               DESTINATIONf);
#if defined(INCLUDE_L3)    
        if ((rv = _bcm_vp_gport_dest_fill(unit, vp, &dest)) < 0) {
            return rv;
        } 
#endif
    }
    if (dest_type != -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_construct(unit, &dest, &failover_ring->port1));
    }        
    return BCM_E_NONE;
}

int
_bcm_tr3_l2_ring_replace(int unit, bcm_l2_ring_t *l2_ring)
{
    int match_type = -1, rv = BCM_E_NONE, gport_id, vp_type = 0;
    uint32 map_size, i, rval;
    SHR_BITDCL *vlan_map = NULL, *vpn_map = NULL;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    uint32 r_flags = 0;

    /*
    * match_type = -1 means flushing based on physical port or VP
    * match_type = 0 means flushing based on VLAN or port + VLAN
    * match_type = 1 means flushing based on VPN or VP + VPN
    */
    if (!l2_ring) {
        return BCM_E_PARAM;
    }

    /* Flag Validation Check */
    if (((l2_ring->flags & BCM_L2_REPLACE_DYNAMIC) &&
         (l2_ring->flags & BCM_L2_REPLACE_MATCH_STATIC)) ||
        ((l2_ring->flags & BCM_L2_REPLACE_MATCH_UC) &&
         (l2_ring->flags & BCM_L2_REPLACE_MATCH_MC))) {
        return BCM_E_PARAM;
    }

    /*
    * If only one of the ports(port0 or port1) are valid, then we have to write 
    * that port value into both L2_BULK_MATCH_PORT_Ar and 
    * L2_BULK_MATCH_PORT_Br due to hardware behavior.
    */
    if ((l2_ring->port0 != BCM_GPORT_INVALID) && 
        (l2_ring->port1 == BCM_GPORT_INVALID)) {
        l2_ring->port1 = l2_ring->port0;   
    }
    if ((l2_ring->port1 != BCM_GPORT_INVALID) && 
        (l2_ring->port0 == BCM_GPORT_INVALID)) {
        l2_ring->port0 = l2_ring->port1;   
    }

    /* Get replace flags */
    r_flags = l2_ring->flags;

    /* Configure h/w tables and memory */
    if (l2_ring->flags & BCM_L2_REPLACE_PROTECTION_RING) {
        if (l2_ring->port0 != BCM_GPORT_INVALID) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, l2_ring->port0, &mod_out, 
                                        &port_out, &trunk_id, &gport_id));
            SOC_IF_ERROR_RETURN(READ_L2_BULK_MATCH_PORT_Ar(unit, &rval));
            if (-1 != gport_id) {
                vp_type = 1;
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval,
                                  DESTINATIONf, gport_id);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval,
                                  DEST_TYPEf, 2);
            } else if (BCM_TRUNK_INVALID != trunk_id) {
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval, 
                                  DESTINATIONf, trunk_id);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval,
                                  DEST_TYPEf, 1);
            } else if (!(-1 == port_out || -1 == mod_out)) {
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval, 
                                  PORT_NUMf, port_out);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval, 
                                  MODULE_IDf, mod_out);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar, &rval,
                                  DEST_TYPEf, 0);
            } 
            SOC_IF_ERROR_RETURN(WRITE_L2_BULK_MATCH_PORT_Ar(unit, rval));
        }
        if (l2_ring->port1 != BCM_GPORT_INVALID) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, l2_ring->port1, &mod_out, 
                                        &port_out, &trunk_id, &gport_id));
            SOC_IF_ERROR_RETURN(READ_L2_BULK_MATCH_PORT_Br(unit, &rval));
            if (-1 != gport_id) {
                vp_type = 1;
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval,
                                  DESTINATIONf, gport_id);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval,
                                  DEST_TYPEf, 2);
            } else if (BCM_TRUNK_INVALID != trunk_id) {
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval, 
                                  DESTINATIONf, trunk_id);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval,
                                  DEST_TYPEf, 1);
            } else if (!(-1 == port_out || -1 == mod_out)) {
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval, 
                                  PORT_NUMf, port_out);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval, 
                                  MODULE_IDf, mod_out);
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br, &rval,
                                  DEST_TYPEf, 0);
            }
            SOC_IF_ERROR_RETURN(WRITE_L2_BULK_MATCH_PORT_Br(unit, rval));
        }
        if (l2_ring->flags & BCM_L2_REPLACE_VPN_TYPE) {
            map_size = SHR_BITALLOCSIZE(BCM_VPN_MAX+1);
            vpn_map = soc_cm_salloc(unit, map_size, "Failover ring vpn map");
            if (NULL == vpn_map) {
                return (SOC_E_MEMORY);
            }
            sal_memset(vpn_map, 0, map_size);
            for (i = BCM_VPN_MIN; i < BCM_VPN_MAX; i++) {
                 if (BCM_VPN_VEC_GET(l2_ring->vpn_vector, i)) {
                     SHR_BITSET(vpn_map, i);
                     match_type = 1;  
                 }
            }
            if (match_type == 1) {
                if ((rv = soc_mem_write_range(unit, L2_BULK_MATCH_VLANS_PORT_Am,
                                              MEM_BLOCK_ALL, 0, soc_mem_index_max( 
                                              unit, L2_BULK_MATCH_VLANS_PORT_Am), 
                                              vpn_map)) < 0) {
                    soc_cm_sfree(unit, vpn_map);
                    return rv;
                }
                if ((rv = soc_mem_write_range(unit, L2_BULK_MATCH_VLANS_PORT_Bm,
                                              MEM_BLOCK_ALL, 0, soc_mem_index_max( 
                                              unit, L2_BULK_MATCH_VLANS_PORT_Bm), 
                                              vpn_map)) < 0) {
                    soc_cm_sfree(unit, vpn_map);
                    return rv;
                }
                /*
                * As we are writing vlan_vector/vpn_vector into both 
                * L2_BULK_MATCH_VLANS_PORT_Am and 
                * L2_BULK_MATCH_VLANS_PORT_Bm, we have to set key_type into 
                * both L2_BULK_KEY_TYPE_PORT_Ar and 
                * L2_BULK_KEY_TYPE_PORT_Br. 
                */
                SOC_IF_ERROR_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
#ifdef BCM_KATANA2_SUPPORT        
                if (SOC_IS_KATANA2(unit)) {
                    soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval, 
                                      KEY_TYPEf, KT2_L2_HASH_KEY_TYPE_VFI);
                }
#endif  
                if (SOC_IS_TRIUMPH3(unit)) {
                    soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval, 
                                      KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_1_L2_VFI);
                }
                SOC_IF_ERROR_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));
           
                SOC_IF_ERROR_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
#ifdef BCM_KATANA2_SUPPORT        
                if (SOC_IS_KATANA2(unit)) {
                    soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                                      KEY_TYPEf, KT2_L2_HASH_KEY_TYPE_VFI);
                }
#endif
                if (SOC_IS_TRIUMPH3(unit)) {
                    soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                                      KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_1_L2_VFI);
                }
                SOC_IF_ERROR_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));
            }
            soc_cm_sfree(unit, vpn_map);
        } else {
            vp_type = 0;
            map_size = SHR_BITALLOCSIZE(BCM_VLAN_MAX+1);
            vlan_map = soc_cm_salloc(unit, map_size, "Failover ring vlan map");
            if (NULL == vlan_map) {
                return (SOC_E_MEMORY);
            }
            sal_memset(vlan_map, 0, map_size);
            for (i = BCM_VLAN_MIN; i < BCM_VLAN_MAX; i++) {
                 if (BCM_VLAN_VEC_GET(l2_ring->vlan_vector, i)) {
                     SHR_BITSET(vlan_map, i);
                     match_type = 0;
                 }
            }
            if ((rv = soc_mem_write_range(unit, L2_BULK_MATCH_VLANS_PORT_Am,
                                          MEM_BLOCK_ALL, 0, soc_mem_index_max( 
                                          unit, L2_BULK_MATCH_VLANS_PORT_Am), 
                                          vlan_map)) < 0) {
                 soc_cm_sfree(unit, vlan_map);
                 return rv;
            }
            if ((rv = soc_mem_write_range(unit, L2_BULK_MATCH_VLANS_PORT_Bm,
                                          MEM_BLOCK_ALL, 0, soc_mem_index_max(
                                          unit, L2_BULK_MATCH_VLANS_PORT_Bm), 
                                          vlan_map)) < 0) {
                 soc_cm_sfree(unit, vlan_map);
                 return rv;
            }

            /*
            * As we are writing vlan_vector/vpn_vector into both
            * L2_BULK_MATCH_VLANS_PORT_Am and
            * L2_BULK_MATCH_VLANS_PORT_Bm, we have to set key_type into
            * both L2_BULK_KEY_TYPE_PORT_Ar and
            * L2_BULK_KEY_TYPE_PORT_Br.
            */
            SOC_IF_ERROR_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Ar(unit, &rval));
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                                  KEY_TYPEf, KT2_L2_HASH_KEY_TYPE_BRIDGE);
            }
#endif
            if (SOC_IS_TRIUMPH3(unit)) {
                soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Ar, &rval,
                                  KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE);
            }
            SOC_IF_ERROR_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Ar(unit, rval));

            SOC_IF_ERROR_RETURN(READ_L2_BULK_KEY_TYPE_PORT_Br(unit, &rval));
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                                  KEY_TYPEf, KT2_L2_HASH_KEY_TYPE_BRIDGE);
            }
#endif
            if (SOC_IS_TRIUMPH3(unit)) {
                soc_reg_field_set(unit, L2_BULK_KEY_TYPE_PORT_Br, &rval,
                                  KEY_TYPEf, SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE);
            }
            SOC_IF_ERROR_RETURN(WRITE_L2_BULK_KEY_TYPE_PORT_Br(unit, rval));
            soc_cm_sfree(unit, vlan_map);
        }
        SOC_IF_ERROR_RETURN(READ_AUX_L2_BULK_CONTROLr(unit, &rval));
        if ((l2_ring->port0 != BCM_GPORT_INVALID) || 
            (l2_ring->port1 != BCM_GPORT_INVALID)) {
            soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, 
                              MATCH_PORT_Af, 1);
            soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, 
                              MATCH_PORT_Bf, 1);
        } 
        if (match_type != -1) {
            soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, 
                              MATCH_VLANS_PORT_Af, 1);
            soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, 
                              MATCH_VLANS_PORT_Bf, 1);
            soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, 
                              MATCH_KEY_TYPE_PORT_Af, 1);
            soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, 
                              MATCH_KEY_TYPE_PORT_Bf, 1);
        }
        soc_reg_field_set(unit, AUX_L2_BULK_CONTROLr, &rval, BULK_OPS_ENABLEf, 
                          1);
        SOC_IF_ERROR_RETURN(WRITE_AUX_L2_BULK_CONTROLr(unit, rval));
        /*
        * match_type is set to 1 in case of flushing based on VP
        */
        if (vp_type) {  
            match_type = 1;
        }    
#ifdef BCM_KATANA2_SUPPORT        
        if (SOC_IS_KATANA2(unit)) {
            rv = _kt2_l2_delete_all_by_hw(unit, match_type, r_flags); 
        } else 
#endif        
        {    
            rv = _tr3_l2_delete_all_by_hw(unit, match_type, r_flags);
        }    
        if (rv < 0) {
            LOG_ERROR(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "Error deleting entries: %d\n"), rv));
        }
 	
        /* Trunk port delete */
        if ((BCM_GPORT_IS_TRUNK(l2_ring->port0))
             || (BCM_GPORT_IS_TRUNK(l2_ring->port1))) {
            SOC_IF_ERROR_RETURN(READ_L2_BULK_MATCH_PORT_Ar(unit, &rval));
            if (soc_reg_field_get(unit, L2_BULK_MATCH_PORT_Ar,
                                  rval, DEST_TYPEf) == 1) {
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Ar,
                                  &rval, MODULE_IDf, 0x80);
            }
            SOC_IF_ERROR_RETURN(WRITE_L2_BULK_MATCH_PORT_Ar(unit, rval));
            
            SOC_IF_ERROR_RETURN(READ_L2_BULK_MATCH_PORT_Br(unit, &rval));
            if (soc_reg_field_get(unit, L2_BULK_MATCH_PORT_Br,
                                  rval, DEST_TYPEf) == 1) {
                soc_reg_field_set(unit, L2_BULK_MATCH_PORT_Br,
                                  &rval, MODULE_IDf, 0x80);
            }
            SOC_IF_ERROR_RETURN(WRITE_L2_BULK_MATCH_PORT_Br(unit, rval));
#ifdef BCM_KATANA2_SUPPORT            
            if (SOC_IS_KATANA2(unit)) {
                rv = _kt2_l2_delete_all_by_hw(unit, match_type, r_flags);
            } else
#endif      
            {
                rv = _tr3_l2_delete_all_by_hw(unit, match_type, r_flags);
            }    
            if (rv < 0) {
	            LOG_ERROR(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "Error deleting entries: %d\n"), rv));
            }
        }
        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_AUX_L2_BULK_CONTROLr(unit, rval));    
    }
    return rv;
}

int
bcm_tr3_l2_ring_replace(int unit, bcm_l2_ring_t *l2_ring)
{
    int rv = BCM_E_NONE;
    _BCM_ALL_L2X_MEM_LOCK(unit);
    rv = _bcm_tr3_l2_ring_replace(unit, l2_ring);
    _BCM_ALL_L2X_MEM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_tr3_bfd_l2_hash_dynamic_replace
 * Purpose:
 *      Replace dynamic L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of dynamic entry.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               Error
 */

int bcm_tr3_bfd_l2_hash_dynamic_replace(int unit, void *l2x_entry)
{
    _bcm_tr3_l2_entries_t l2_entry; 
    
    sal_memset(&l2_entry, 0, sizeof(_bcm_tr3_l2_entries_t)); 
    sal_memcpy(&l2_entry.l2_entry_1, l2x_entry, sizeof(l2_entry_1_entry_t)); 
    l2_entry.entry_flags |= _BCM_TR3_L2_SELECT_L2_ENTRY_1; 
    
    return _bcm_tr3_l2_hash_dynamic_replace(unit, &l2_entry); 
}

#endif /* BCM_TRIUMPH3_SUPPORT */
