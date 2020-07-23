/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.h
 * Purpose:     Internal Field Processor data structure definitions for the
 *              BCM library.
 */

#ifndef _BCM_INT_FIELD_H
#define _BCM_INT_FIELD_H

#include <bcm/field.h>
#include <bcm_int/common/field.h>

#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/er_tcam.h>

#include <sal/core/sync.h>
#include <bcm_int/esw/xgs4.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_common.h>
#include <bcm_int/esw/flowtracker/ft_field.h>

#ifdef BCM_FIREBOLT6_SUPPORT
#include <bcm_int/esw/field_compression/field_trie_mgmt.h>
#endif

#ifdef BCM_FIELD_SUPPORT

/*
 *Macro:
 *     FP_LOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define FP_LOCK(unit) \
    sal_mutex_take(SOC_CONTROL(unit)->fp_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     FP_UNLOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define FP_UNLOCK(unit) \
    sal_mutex_give(SOC_CONTROL(unit)->fp_lock);

#define BCM_IF_ERROR_CLEANUP(rv)            \
    do {                                    \
        if (rv < 0) {                       \
            goto cleanup;                   \
        }                                   \
    } while(0)

#define _FP_PORT_BITWIDTH(_unit_)    \
                   ((SOC_IS_TOMAHAWKX(_unit_) || SOC_IS_TRIDENT3X(_unit_)) ? 8 : \
                   (SOC_IS_TD_TT(_unit_) || SOC_IS_KATANAX(_unit_) || \
                    SOC_IS_TRIUMPH3(_unit_) || \
                    SOC_IS_GREYHOUND2(_unit_)) ? 7 : 6)

#define _FP_MCAST_D_TYPE_OFFSET(_unit_)  \
                 ((SOC_IS_TRIDENT2X(unit) ||SOC_IS_TITAN2PLUS(unit) ||      \
                 SOC_IS_TRIUMPH3(unit)) ? (18) : \
                  ((SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit)) ? (16) :    \
                   (14)));

#define _FP_GLOBAL_INST (-1)

#define _FP_2PIPE_MEM_IDENTIFIER_GET(_mem_, _name1_, _name2_, _inst_) \
        {                                                       \
           if (_inst_ == 0) {                                   \
              _mem_ = _name1_ ## _ ## _name2_ ## 0 ## m;        \
           } else if (_inst_ == 1) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 1 ## m;        \
           } else if (_inst_ == _FP_GLOBAL_INST) {              \
              _mem_ = _name1_ ## m;                             \
           }                                                    \
        }

#define _FP_4PIPE_MEM_IDENTIFIER_GET(_mem_, _name1_, _name2_, _inst_) \
        {                                                       \
           if (_inst_ == 0) {                                   \
              _mem_ = _name1_ ## _ ## _name2_ ## 0 ## m;        \
           } else if (_inst_ == 1) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 1 ## m;        \
           } else if (_inst_ == 2) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 2 ## m;        \
           } else if (_inst_ == 3) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 3 ## m;        \
           } else if (_inst_ == _FP_GLOBAL_INST) {              \
              _mem_ = _name1_ ## m;                             \
           }                                                    \
        }

#define _FP_8PIPE_MEM_IDENTIFIER_GET(_mem_, _name1_, _name2_, _inst_) \
        {                                                       \
           if (_inst_ == 0) {                                   \
              _mem_ = _name1_ ## _ ## _name2_ ## 0 ## m;        \
           } else if (_inst_ == 1) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 1 ## m;        \
           } else if (_inst_ == 2) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 2 ## m;        \
           } else if (_inst_ == 3) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 3 ## m;        \
           } else if (_inst_ == 4) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 4 ## m;        \
           } else if (_inst_ == 5) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 5 ## m;        \
           } else if (_inst_ == 6) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 6 ## m;        \
           } else if (_inst_ == 7) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 7 ## m;        \
           } else if (_inst_ == _FP_GLOBAL_INST) {              \
              _mem_ = _name1_ ## m;                             \
           }                                                    \
        }

#define _FP_2PIPE_REG_IDENTIFIER_GET(_reg_, _name1_, _name2_, _inst_) \
        {                                                       \
           if (_inst_ == 0) {                                   \
              _reg_ = _name1_ ## _ ## _name2_ ## 0 ## r;        \
           } else if (_inst_ == 1) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 1 ## r;        \
           } else if (_inst_ == _FP_GLOBAL_INST) {              \
              _reg_ = _name1_ ## r;                             \
           }                                                    \
        }

#define _FP_4PIPE_REG_IDENTIFIER_GET(_mem_, _name1_, _name2_, _inst_) \
        {                                                       \
           if (_inst_ == 0) {                                   \
              _mem_ = _name1_ ## _ ## _name2_ ## 0 ## r;        \
           } else if (_inst_ == 1) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 1 ## r;        \
           } else if (_inst_ == 2) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 2 ## r;        \
           } else if (_inst_ == 3) {                            \
              _mem_ = _name1_ ## _ ## _name2_ ## 3 ## r;        \
           } else if (_inst_ == _FP_GLOBAL_INST) {              \
              _mem_ = _name1_ ## r;                             \
           }                                                    \
        }

#define _FP_8PIPE_REG_IDENTIFIER_GET(_reg_, _name1_, _name2_, _inst_) \
        {                                                       \
           if (_inst_ == 0) {                                   \
              _reg_ = _name1_ ## _ ## _name2_ ## 0 ## r;        \
           } else if (_inst_ == 1) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 1 ## r;        \
           } else if (_inst_ == 2) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 2 ## r;        \
           } else if (_inst_ == 3) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 3 ## r;        \
           } else if (_inst_ == 4) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 4 ## r;        \
           } else if (_inst_ == 5) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 5 ## r;        \
           } else if (_inst_ == 6) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 6 ## r;        \
           } else if (_inst_ == 7) {                            \
              _reg_ = _name1_ ## _ ## _name2_ ## 7 ## r;        \
           } else if (_inst_ == _FP_GLOBAL_INST) {              \
              _reg_ = _name1_ ## r;                             \
           }                                                    \
        }

#define _FIELD_COUNT_NUM_OF_BITS_SET(_bmp_, _width_)  \
    do  {                                     \
        uint32 _val_ = 0;                     \
        _val_ = _bmp_;                        \
        for (_width_ = 0; _val_; _width_++) { \
            _val_ &= _val_ - 1;               \
        }                                     \
    } while(0);

#define _FIELD_IS_INTERNAL_UDF_QUAL(_qid_) ((_bcmFieldQualifyData0 == (_qid_)) \
                                            || (_bcmFieldQualifyData1 == (_qid_)) \
                                            || (_bcmFieldQualifyData2 == (_qid_)) \
                                            || (_bcmFieldQualifyData3 == (_qid_)) \
                                            || (_bcmFieldQualifyData4 == (_qid_)) \
                                            || (_bcmFieldQualifyData5 == (_qid_)) \
                                            || (_bcmFieldQualifyData6 == (_qid_)) \
                                            || (_bcmFieldQualifyData7 == (_qid_)) \
                                            || (_bcmFieldQualifyData8 == (_qid_)) \
                                            || (_bcmFieldQualifyData9 == (_qid_)) \
                                            || (_bcmFieldQualifyData10 == (_qid_)) \
                                            || (_bcmFieldQualifyData11 == (_qid_)) \
                                            || (_bcmFieldQualifyData12 == (_qid_)) \
                                            || (_bcmFieldQualifyData13 == (_qid_)) \
                                            || (_bcmFieldQualifyData14 == (_qid_)) \
                                            || (_bcmFieldQualifyData15 == (_qid_)))

#define _FP_INVALID_INDEX            (-1)

/* 64 bit software counter collection bucket. */
#define _FP_64_COUNTER_BUCKET  512

/* Maximum number of paired slices.*/
#define _FP_PAIR_MAX   (3)
extern int
_bcm_internal_field_action_add(int unit,
                         bcm_field_entry_t src_entry,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 param0,
                         uint32 param1);
#if defined (BCM_TRIUMPH3_SUPPORT)

typedef struct _field_action_l2mod_index_bmp_s {
    SHR_BITDCL  *w;
} _field_action_l2mod_index_bmp_t;

#endif

#if defined (BCM_KATANA_SUPPORT)
    /* linked list to hold fabricQueue action params*/
    typedef struct _field_fabricQueue_action_s {
        uint32  queue_index; /* cosQ index */
        uint32  profile_index; /* cosQ profile index */
        int  reference_count;  /* Reference count */
        struct _field_fabricQueue_action_s *next;
    } _field_fabricQueue_action_t;
#endif /* (BCM_KATANA_SUPPORT)*/

/* Macro to find the PBMP support in a multi-pipe  tcam slice mode */
#if defined(BCM_TOMAHAWK_SUPPORT)
    #define _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(_g_flags)     \
                      (((_g_flags & _FP_GROUP_SPAN_SINGLE_SLICE) && \
                        (_g_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) || \
                       (_g_flags & _FP_GROUP_SPAN_DOUBLE_SLICE) || \
                       (_g_flags & _FP_GROUP_SPAN_TRIPLE_SLICE))
#else
    #define _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(_g_flags)  0
#endif /*BCM_TOMAHAWK_SUPPORT */
/*
 * Meter mode.
 */
#define _BCM_FIELD_METER_BOTH (BCM_FIELD_METER_PEAK | \
                               BCM_FIELD_METER_COMMITTED)

/* Value for control->tcam_ext_numb that indicates there is no external TCAM. */
#define FP_EXT_TCAM_NONE _FP_INVALID_INDEX

/*
 * Initial group IDs and entry IDs.
 */
#define _FP_ID_BASE 1
#define _FP_ID_MAX  (0x1000000)

/* This IDs are reserved for internal use */
#define _FP_INTERNAL_RESERVED_ID_MAX 0xFFFFFFFF
#define _FP_INTERNAL_RESERVED_ID_MIN 0xFFFFF000
/* Currently used reserved id */
#define _FP_INTERNAL_RESERVED_ID (_FP_INTERNAL_RESERVED_ID_MIN)
/* Used for TH3 EM COS SW WAR */
#define _FP_INTERNAL_RESERVED_EM_ID (_FP_INTERNAL_RESERVED_ID_MIN + 1)
#define _FP_PRESEL_INTERNAL_RESERVED_ID (_BCM_FIELD_PRESEL_ID_MAX - 1)

/*
 * Field Group IDs reserved for FLOWTRACKER module
 * To create EXACTMATCH groups.
 */
#define _FP_FLOWTRACKER_RESERVED_EM_GID_MAX         0xF910117F
#define _FP_FLOWTRACKER_RESERVED_EM_GID_MIN         0xF9101170
#define _FP_FLOWTRACKER_RESERVED_EM_GID_AFTER_SHIFT 0xF910117
#define _FP_FLOWTRACKER_RESERVED_EM_GID_SHIFT_VAL   0x4

/*
 * Range of Hint Ids.
 */
#define _FP_HINT_ID_BASE         (0x1)
#define _FP_HINT_ID_MAX          (0x400)
#define _FP_MAX_HINTS_PER_HINTID (0x100)

/*
 * Action flags.
 */
/* Action valid. */
#define _FP_ACTION_VALID              (1 << 0)
/* Reinstall is required. */
#define _FP_ACTION_DIRTY              (1 << 1)
/* Remove action from hw. */
#define _FP_ACTION_RESOURCE_FREE      (1 << 2)
/* Action was updated free previously used hw resources. */
#define _FP_ACTION_OLD_RESOURCE_FREE  (1 << 3)

#define _FP_ACTION_MODIFY             (1 << 4)

#define _FP_ACTION_HW_FREE (_FP_ACTION_RESOURCE_FREE | \
                            _FP_ACTION_OLD_RESOURCE_FREE)

#define _FP_ACTION_POLICER_SW_UPDATE  (1 << 5)
#if defined (BCM_TOMAHAWK_SUPPORT)
#define _BCM_FIELD_ACTION_CTC_MANIPULATE_FLAG (1 << 6)
#endif /* BCM_TOMAHAWK_SUPPORT */
#define _BCM_FIELD_ACTION_REF_STALE   (1 << 7)

/* Oam Stat Action flags */
#define _FP_OAM_STAT_ACTION_VALID    (1 << 0)
#define _FP_OAM_STAT_ACTION_DIRTY    (1 << 1)

/* Field policy table Cos queue priority hw modes */
#define _FP_ACTION_UCAST_MCAST_QUEUE_NEW_MODE   0x1
#define _FP_ACTION_UCAST_QUEUE_CHANGE           0x2
#define _FP_ACTION_UCAST_QUEUE_NEW_MODE         0x8
#define _FP_ACTION_MCAST_QUEUE_NEW_MODE         0x9
/* Action set unicast queue value */
#define _FP_ACTION_UCAST_QUEUE_SET(q) ((q) & (0xf))
/* Action set multicast queue value */
#define _FP_ACTION_MCAST_QUEUE_SET(q) (((q) & 0x7) << 4)
/* Action set multicast queue value with width */
#define _FP_ACTION_MCAST_QUEUE_WITH_WIDTH_SET(q,w) (((q) & w) << 4)
/* Action get unicast queue value */
#define _FP_ACTION_UCAST_QUEUE_GET(q) ((q) & (0xf))
/* Action get multicast queue value */
#define _FP_ACTION_MCAST_QUEUE_GET(q) (((q) >> 4) & 0x7)
/* Action set both unicast and multicast queue value */
#define _FP_ACTION_UCAST_MCAST_QUEUE_SET(u,m) ((_FP_ACTION_UCAST_QUEUE_SET(u)) | \
                                                (_FP_ACTION_MCAST_QUEUE_SET(m)))
/* Action set both unicast and multicast queue value with width */
#define _FP_ACTION_UCAST_MCAST_QUEUE_WITH_WIDTH_SET(u,m,w) ( \
                                (_FP_ACTION_UCAST_QUEUE_SET(u)) | \
                                (_FP_ACTION_MCAST_QUEUE_WITH_WIDTH_SET(m,w)))

#define _FP_MCAST_QUEUE_WIDTH_GET(unit) \
     ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) ? (0xf) : (0x7))

/*
 * Maximum number of meter pools
 *     Should change when new chips are defined
 */
#define _FIELD_MAX_METER_POOLS 20

#define _FIELD_MAX_CNTR_POOLS  20

/*
 * Internal version of qset add that will allow Data[01] to be set.
 */
#define BCM_FIELD_QSET_ADD_INTERNAL(qset, q)  SHR_BITSET(((qset).w), (q))
#define BCM_FIELD_QSET_REMOVE_INTERNAL(qset, q)  SHR_BITCLR(((qset).w), (q))
#define BCM_FIELD_QSET_TEST_INTERNAL(qset, q) SHR_BITGET(((qset).w), (q))

/*
 * Operation between two qsets.
 */
#define _FIELD_BITS_OP(_a, _b, _max, _op)                        \
        {                                                        \
           int _ct;                                              \
           int _max_w = (_max + SHR_BITWID - 1)/SHR_BITWID;      \
           for (_ct = 0; _ct < (_max_w); _ct++) {                \
               (_a)[_ct] _op (_b)[_ct];                          \
           }                                                     \
        }

#define _BCM_FIELD_QSET_OR(_qset, _qset_new, _max_count)  \
        _FIELD_BITS_OP(_qset.w, _qset_new.w, _max_count, |=)


/* User-Defined Field (UDF) IP Flags. */
#define _BCM_FIELD_USER_THREE_MPLS_LABELS   (18 << 6)
#define _BCM_FIELD_USER_FOUR_MPLS_LABELS    (19 << 6)
#define _BCM_FIELD_USER_FIVE_MPLS_LABELS    (20 << 6)

/* Qualifier data/mask buffer. */
#define _FP_UDF_DATA_WORDS  (8)
typedef uint32 _bcm_field_udf_data_t[_FP_UDF_DATA_WORDS];

#ifdef BCM_WARM_BOOT_SUPPORT
/* Version change to 32 - for 6.5.20
 * This is to sync and recover port_type for srcGport qualifier in VFP stage
 * For this version, there is no chage in size of scache
 */
#define BCM_FIELD_WB_VERSION_1_32               SOC_SCACHE_VERSION(1, 32)
/* Version change to 31 - for 6.5.19
 * This is to sync and recover new AMFTFP/AEFTFP stages
 */
#define BCM_FIELD_WB_VERSION_1_31               SOC_SCACHE_VERSION(1, 31)
/* Version change to 30 - for 6.5.18
 * This is to sync and recover the EFP meter table for FB6 
 */
#define BCM_FIELD_WB_VERSION_1_30               SOC_SCACHE_VERSION(1, 30)
/* Version change to 29 - for 6.5.16
 * Also, to enable sync and recovery of fpf3 selcode on EFP for 
 * TD and TT devices
 */
#define BCM_FIELD_WB_VERSION_1_29               SOC_SCACHE_VERSION(1, 29)
/* Version change to 28 - for 6.5.15
 */
#define BCM_FIELD_WB_VERSION_1_28               SOC_SCACHE_VERSION(1, 28)

/* Version change to 27 - for flowtracker stage
 * Also, sync the slice indexes of disabled entries for EFP/VFP of TH WB
 */
#define BCM_FIELD_WB_VERSION_1_27               SOC_SCACHE_VERSION(1, 27)

/* Version change to 26 - for 6.5.14
 * Number of bytes increased to sync instance info in slice map and
 * group information for EFP/VFP
 */
#define BCM_FIELD_WB_VERSION_1_26               SOC_SCACHE_VERSION(1, 26)
/* Version change to 25 - for 6.5.13
 * TH3 related new changes added.
 */
#define BCM_FIELD_WB_VERSION_1_25               SOC_SCACHE_VERSION(1, 25)
/* Version change to 23  - For 6.5.11
 * There is an issue with sync of extractor code for TH WB.
 * The extractor codes of the first part is copied to remaining 
 * parts of the group. The is fixed both in sync and recovery
 * This version is used as a checkpoint to devide on whether to
 * use fix in recovery. all release after this version will not
 * use the fix in recovery code
 */
#define BCM_FIELD_WB_VERSION_1_24               SOC_SCACHE_VERSION(1, 24)
/* Version change to 23  - For 6.5.10
 * There are couple of requirements for this change
 * 1. Ingress_logical_policer_pools_mode is synced at wrong location earlier.
 * It has to be synced at the begining of the ingress
 * before slice recovery is done. Hence syncing it here again.
 * The earlier sync of Ingress_logical_policer_pools_mode is retained not
 * to disturb legacy code.
 * 2. To recover group loopback type selcode
 */
#define BCM_FIELD_WB_VERSION_1_23               SOC_SCACHE_VERSION(1, 23)
/* Version change to 22  - For 6.5.7 NO Scache size increment */
#define BCM_FIELD_WB_VERSION_1_22               SOC_SCACHE_VERSION(1, 22)
/* Version change to 21  - For 6.5.6 */
#define BCM_FIELD_WB_VERSION_1_21               SOC_SCACHE_VERSION(1, 21)
/* Version change to 20 to recover MirrorIngress/Egress Action. - For 6.5.5 */
#define BCM_FIELD_WB_VERSION_1_20               SOC_SCACHE_VERSION(1, 20)
/* Version change to 19 to recover IngressMplsPortSet Action. - For 6.5.4 */
#define BCM_FIELD_WB_VERSION_1_19               SOC_SCACHE_VERSION(1, 19)
/* Version Change to 18 to recover SVP, DVP TYPE for each part of entry from scache. */
#define BCM_FIELD_WB_VERSION_1_18               SOC_SCACHE_VERSION(1, 18)
/* Version Change to 17 to recover policer configurations from scache. */
#define BCM_FIELD_WB_VERSION_1_17               SOC_SCACHE_VERSION(1, 17)
/* Version Change to 16 to recover EFP KEY for TH from scache. */
#define BCM_FIELD_WB_VERSION_1_16               SOC_SCACHE_VERSION(1, 16)
/* Version Change to 15 to recover actions from scache. */
#define BCM_FIELD_WB_VERSION_1_15               SOC_SCACHE_VERSION(1, 15)
/* Version Change..TR3 stat recovery for stageExternal */
#define BCM_FIELD_WB_VERSION_1_14               SOC_SCACHE_VERSION(1, 14)
#define BCM_FIELD_WB_VERSION_1_13               SOC_SCACHE_VERSION(1, 13)
#define BCM_FIELD_WB_VERSION_1_12               SOC_SCACHE_VERSION(1, 12)
#define BCM_FIELD_WB_VERSION_1_11               SOC_SCACHE_VERSION(1, 11)
#define BCM_FIELD_WB_VERSION_1_10               SOC_SCACHE_VERSION(1, 10)
#define BCM_FIELD_WB_VERSION_1_9                SOC_SCACHE_VERSION(1,9)
#define BCM_FIELD_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_FIELD_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_FIELD_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_FIELD_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_FIELD_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_FIELD_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_FIELD_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_FIELD_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_FIELD_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_FIELD_WB_DEFAULT_VERSION            BCM_FIELD_WB_VERSION_1_32
#endif

#if defined(BCM_FLOWTRACKER_SUPPORT)
/* Keygen profile types */
#define _BCM_FIELD_FT_KEYGEN_PROFILE_AAB     (1 << 0)
#define _BCM_FIELD_FT_KEYGEN_PROFILE_AB      (1 << 1)

/* Maximum value for group key type */
#define _BCM_FIELD_FT_GROUP_KEY_TYPE_MAX 128

/*
 * Typedef:
 *  _field_ft_keygen_type_a_ext_t
 * Purpose:
 *  Keygen type A extractors information
 */
typedef struct _field_ft_keygen_type_a_ext_s {
    int             l1_e16_sel[10];
    int             l2_e16_ext_mode[10];
    int             l2_e16_ext_section[10];
} _field_ft_keygen_type_a_ext_t;

/*
 * Typedef:
 *  _field_ft_keygen_type_a_ext_mask_t
 *
 * Purpose:
 *  Keygen type A extractors Mask information
 */
typedef struct _field_ft_keygen_type_a_ext_mask_s {
    uint32          mask[4];
} _field_ft_keygen_type_a_ext_mask_t;

/*
 * Typedef:
 *  _field_ft_keygen_type_b_ext_t
 * Purpose:
 *  Keygen type B extractors information.
 */
typedef struct _field_ft_keygen_type_b_ext_s {
    int             l1_e16_sel[8];
} _field_ft_keygen_type_b_ext_t;

/*
 * Typedef:
 *  _field_ft_keygen_type_c_ext_t
 * Purpose:
 *  Keygen type C extractors information.
 */
typedef struct _field_ft_keygen_type_c_ext_s {
    int             l1_e1_sel[16];
} _field_ft_keygen_type_c_ext_t;

/*
 * Typedef:
 *  _field_ft_keygen_type_c_ext_mask_t
 *
 * Purpose:
 *  Keygen type C extractors Mask information
 */
typedef struct _field_ft_keygen_type_c_ext_mask_s {
    uint32          mask;
} _field_ft_keygen_type_c_ext_mask_t;

/*
 * Typedef:
 *  _field_ft_keygen_ext_codes_t
 * Purpose
 *  Keygen extraction codes information
 */
typedef struct _field_ft_keygen_ext_codes_s {
    int                                 keygen_profile;
    _field_ft_keygen_type_a_ext_t       type_a_extractors[2];
    _field_ft_keygen_type_a_ext_mask_t  type_a_mask[2];
    _field_ft_keygen_type_b_ext_t       type_b_extractors;
    _field_ft_keygen_type_c_ext_t       type_c_extractors;
    _field_ft_keygen_type_c_ext_mask_t  type_c_mask;
} _field_ft_keygen_ext_codes_t;

/*
 * Typedef:
 *      bcmi_ft_tracking_param_qual_map_t
 * Purpose:
 *      Flow group tracking param and qualifier map.
 */
typedef struct bcmi_ft_tracking_param_qual_map_s {
    uint16 param;
    bcm_field_qualify_t qual;
    uint16 start;
    uint16 width;
    uint32 flags;
    uint16 db_ft_type_bmp;
} _field_ft_tracking_param_qual_map_t;

/* Number of tracking elements */
#define _FIELD_FT_TRACKING_ELEMENT_TYPE_MAX                 \
                    bcmFlowtrackerTrackingParamTypeCount

/* Number of tracking alu data. */
#define _FIELD_FT_TRACKING_ALU_DATA_MAX                     \
                    _bcmFieldFtAluDataCount

/*
 * Maximum number of tracking params. Maximum value include exposed
 * export elements and internal tracking param (related with flowchecks
 * and Alu Data)
 */
#define _FIELD_FT_TRACKING_PARAM_MAX                        \
                    (_FIELD_FT_TRACKING_ELEMENT_TYPE_MAX +  \
                    _FIELD_FT_TRACKING_ALU_DATA_MAX)

/*
 * Typedef:
 *      _field_ft_tracking_param_qual_map_db_t
 * Purpose:
 *      Database of Flow group tracking param and qualifier map.
 */
typedef struct _field_ft_tracking_param_qual_map_db_s {
    uint32 num_params;
    _field_ft_tracking_param_qual_map_t *param_qual_map[_FIELD_FT_TRACKING_PARAM_MAX];
} _field_ft_tracking_param_qual_map_db_t;

/*
 * Typedef:
 *     _field_ft_info_t
 * Purpose:
 *     flowtracker stage specific information structure.
 */
typedef struct _field_ft_info_s {
    SHR_BITDCL key_type_bmp[2][_SHR_BITDCLSIZE(_BCM_FIELD_FT_GROUP_KEY_TYPE_MAX)];
    soc_profile_mem_t session_key_profile;  /* Profile for session Key */
    soc_profile_mem_t session_key_user_profile;  /* Profile for session Key
                                                  * entries for user installed flows.
                                                  */
    soc_profile_mem_t session_data_profile; /* Profile for session data */
    soc_profile_mem_t alu_data_profile;     /* Profile for Alu data */
    bcmi_keygen_qual_cfg_info_db_t *flow_key_qual_cfg; /* Maintains the database for
                                            qualifiers and its attributes
                                            valid for session Key */
    bcmi_keygen_qual_cfg_info_db_t *flow_data_qual_cfg; /* Maintains the database for
                                            qualifiers and its attributes
                                            valid for session data */
    bcmi_keygen_ext_cfg_db_t **flow_key_ext_cfg_arr; /* Maintains the database
                                            for extractors for session Key */
    bcmi_keygen_ext_cfg_db_t *flow_data_ext_cfg; /* Maintains the database
                                            for extractors for session Data */
    _field_ft_tracking_param_qual_map_db_t *param_qual_map_db; /* Maintains the
                                            database for valid tracking param and
                                            qualifier map */
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    soc_profile_mem_t agg_session_data_profile; /* Profile for aggregate
                                            session data */
    soc_profile_mem_t agg_alu_data_profile;     /* Profile for aggregate
                                            Alu data */
#endif
} _field_ft_info_t;

/* Extractor offsets per qualifier */
typedef struct _field_ft_qual_sel_s {
    /* Number of extractors Offset. */
    uint8 num_offsets;
    /* Extractor offsets. */
    uint16 ext_num[BCMI_KEYGEN_QUAL_OFFSETS_MAX];
} _field_ft_qual_sel_t;

typedef struct _field_ft_qual_ext_offset_info_s {
    /* Qualifier array. */
    bcm_field_qualify_t *qid_arr;
    /* Offset and width information corresponding to
     * qualifiers in qid_arr. */
    bcmi_keygen_qual_offset_t *offset_arr;
    /* Size of the qid_arr and offset_arr arrays */
    /* Selector offsets array */
    _field_ft_qual_sel_t *sel_offset_arr;
    /* Number of quals */
    uint16 size;
} _field_ft_qual_ext_offset_info_t;

#endif /* BCM_FLOWTRACKER_SUPPORT */

/*
 * Typedef:
 *     _field_sel_t
 * Purpose:
 */
typedef struct _field_sel_s {
    int8    fpf0;              /* FPF0 field(s) select.              */
    int8    fpf1;              /* FPF1 field(s) select.              */
    int8    fpf2;              /* FPF2 field(s) select.              */
    int8    fpf3;              /* FPF3 field(s) select.              */
    int8    fpf4;              /* FPF4 field(s) selector.            */
    int8    extn;              /* field(s) select external.          */
    int8    src_class_sel;     /* Source lookup class selection.     */
    int8    dst_class_sel;     /* Destination lookup class selection.*/
    int8    intf_class_sel;    /* Interface class selection.         */
    int8    ingress_entity_sel;/* Ingress entity selection.          */
    int8    src_entity_sel;    /* Src port/trunk entity selection.          */
    int8    dst_fwd_entity_sel;/* Destination forwarding  entity.    */
    int8    fwd_field_sel;     /* Forwarding field type select.      */
    int8    loopback_type_sel; /* Loopback/Tunnel type selection.    */
    int8    ip_header_sel;     /* Inner/Outer Ip header selection.   */
    int8    ip6_addr_sel;      /* Ip6 address format selection.      */
    int8    intraslice;        /* Intraslice double wide selection.  */
    int8    secondary;
    int8    inner_vlan_overlay;
    int8    intraslice_vfp_sel;
    int8    aux_tag_1_sel;
    int8    aux_tag_2_sel;
    int8    oam_overlay_sel;   /* Oam Overlay Selection*/
    int8    ttl_class_sel;
    int8    tcp_class_sel;
    int8    tos_class_sel;
    int8    normalize_ip_sel;
    int8    normalize_mac_sel;
    int8    egr_class_f1_sel;   /* EFP KEY1 Class ID Selector */
    int8    egr_class_f2_sel;   /* EFP KEY2 Class ID Selector */
    int8    egr_class_f3_sel;   /* EFP KEY3 Class ID Selector */
    int8    egr_class_f4_sel;   /* EFP KEY4 Class ID Selector */
    int8    egr_class_f6_sel;   /* EFP KEY6 Class ID Selector */
    int8    egr_class_f7_sel;   /* EFP KEY7 Class ID Selector */
    int8    egr_class_f8_sel;   /* EFP KEY8 Class ID Selector */
    int8    src_dest_class_f1_sel;
    int8    src_dest_class_f3_sel;
    int8    src_type_sel;
    int8    egr_key4_dvp_sel;  /* EFP KEY4 DVP Secondary selector */
    int8    egr_key8_dvp_sel;  /* EFP KEY8 DVP Secondary selector */
    int8    egr_dest_port_f1_sel;
    int8    egr_dest_port_f5_sel;
    int8    egr_key4_mdl_sel;  /* EFP KEY4 MDL Secondary selector */
    int8    egr_oam_overlay_sel; /* EFP Oam Overlay Selection*/
    int8    oam_intf_class_sel; /* Oam SVP Intf Class Selector */
    int8    egr_classId_B_f1_sel;   /* EFP KEY1 Class ID B Selector */
    int8    egr_classId_B_f2_sel;   /* EFP KEY2 Class ID B Selector */
    int8    egr_classId_B_f3_sel;   /* EFP KEY3 Class ID B Selector */
    int8    egr_classId_B_f4_sel;   /* EFP KEY4 Class ID B Selector */
    int8    egr_classId_B_f5_sel;   /* EFP KEY5 Class ID B Selector */
    int8    egr_classId_B_f6_sel;   /* EFP KEY6 Class ID B Selector */
    int8    egr_classId_B_f7_sel;   /* EFP KEY7 Class ID B Selector */
    int8    egr_classId_B_f8_sel;   /* EFP KEY8 Class ID B Selector */
    int8    egr_key4_l3_classId_sel;/* EFP KEY4 L3 Class ID Selector */
    int8    egr_key8_l3_classId_sel;/* EFP KEY4 L3 Class ID Selector */
} _field_sel_t;


/*
 * Count number of bits in a memory field.
 */
#define _FIELD_MEM_FIELD_LENGTH(_u_, _mem_, _fld_) \
             (SOC_MEM_FIELD_VALID((_u_), (_mem_), (_fld_)) ?  \
              (soc_mem_field_length((_u_), (_mem_), (_fld_))) : 0)


/*
 * Field select code macros
 *
 * These are for resolving if the select code has meaning or is really a don't
 * care.
 */

#define _FP_SELCODE_DONT_CARE (-1)
#define _FP_SELCODE_DONT_USE  (-2)

#define _FP_SELCODE_INVALIDATE(selcode) \
     ((selcode) = _FP_SELCODE_DONT_CARE);

#define _FP_SELCODE_IS_VALID(selcode) \
     (((selcode) == _FP_SELCODE_DONT_CARE || \
       (selcode) == _FP_SELCODE_DONT_USE) ? 0 : 1)

#define _FP_SELCODE_IS_INVALID(selcode) \
     (0 == _FP_SELCODE_IS_VALID(selcode))

#define _FP_MAX_FPF_BREAK(_selcode, _stage_fc, _fpf_id) \
      if (_FP_SELCODE_IS_VALID(_selcode))  {           \
         if (NULL == (_stage_fc->_field_table_fpf ## _fpf_id[_selcode])) { \
             break; \
         } \
      }

#define _FP_SELCODE_SET(_selcode_qset, _selcode) \
    sal_memset((_selcode_qset), (int8)_selcode, sizeof(_field_sel_t));


/*
 * Macros for packing and unpacking L3 information for field actions
 * into and out of a uint32 value.
 */

#define MAX_CNT_BITS 10

#define _FP_L3_ACTION_PACK_ECMP(ecmp_ptr, ecmp_cnt) \
    ( 0x80000000 | ((ecmp_ptr) << MAX_CNT_BITS) | (ecmp_cnt) )
#define _FP_L3_ACTION_UNPACK_ECMP_MODE(cookie) \
    ( (cookie) >> 31 )
#define _FP_L3_ACTION_UNPACK_ECMP_PTR(cookie) \
    ( ((cookie) & 0x7fffffff) >> MAX_CNT_BITS )
#define _FP_L3_ACTION_UNPACK_ECMP_CNT(cookie) \
    ((cookie) & ~(0xffffffff << MAX_CNT_BITS))

#define _FP_L3_ACTION_PACK_NEXT_HOP(next_hop) \
    ( (next_hop) & 0x7fffffff )
#define _FP_L3_ACTION_UNPACK_NEXT_HOP(cookie) \
    ( (cookie) & 0x7fffffff )


#define _FP_MAX_ENTRY_WIDTH   4 /* Maximum field entry width. */
#define _FP_TH_MAX_ENTRY_WIDTH  3 /* Field entry parts count in Tomahawk */

/* Both for EFP and External Stage, TCAM Key Generation logic
 * is packet dependent. In EFP, based on the TCAM Slice_Mode and incoming
 * packet type, TCAM Lookup Key is generated.
 * Each entry in EFP TCAM can match on a KEY_MATCH_TYPE field
 * (4 Most Significant Bits) + actual KEY fields from the packet
 * + Pipeline Meta data.
 * KEY_MATCH_TYPE is an encoded value generated by the EFP key generation
 * logic that can be applied to all entries in EFP TCAM to guarantee that
 * each entry matches only on the desired Packet type/s."
 */
#define _FP_MAX_ENTRY_TYPES 2


#define _FP_ENTRY_TYPE_0 0  /* Entries which use Default Slice mode and
                             * KEY_MATCH_TYPE (only EFP)selected by Group
                             * Create logic.
                             */

#define _FP_ENTRY_TYPE_1 1 /* Entries in which alternate KEY_MATCH_TYPE is used
                            * based on the qualifiers applied to the entry.
                            * Typically used to match Not L3 and Not FCoE
                            * Packets.
                            */

#define _FP_ENTRY_TYPE_DEFAULT _FP_ENTRY_TYPE_0

/*  Macros for packing and unpacking Eight Bytes Payload qualifier information */
#define _FP_PACK_L2_EIGHT_BYTES_PAYLOAD(qual_in_1, qual_in_2, qual_out) do {\
        (qual_out)[1] = (uint32) (qual_in_1); \
        (qual_out)[0] = (uint32) (qual_in_2); \
    } while (0)

#define _FP_UNPACK_L2_EIGHT_BYTES_PAYLOAD(qual_in, qual_out_1, qual_out_2) do {\
        (*qual_out_1) = (uint32) ((qual_in)[1]); \
        (*qual_out_2) = (uint32) ((qual_in)[0]); \
    } while (0)

/*
 * Macro: _BCM_FIELD_ENTRIES_IN_SLICE
 *
 * Purpose:
 *     Returns number of entries in a slice, depending on mode
 *
 * Parameters:
 *     fg - pointer to entry's field group struct
 *     fs - pointer to entry's field slice struct
 *     result - return value
 */
#if defined (BCM_TOMAHAWK_SUPPORT)
#define _BCM_FIELD_ENTRIES_IN_SLICE(_u_, _fg_, _fs_, _result_)       \
    do {                                                             \
      if (!(soc_feature(unit, soc_feature_field_multi_pipe_support) &&       \
            (_fg_->stage_id == _BCM_FIELD_STAGE_INGRESS)) &&         \
          ((_fg_)->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {       \
        (_result_) = ((_fs_)->entry_count >> 1);                     \
      } else {                                                       \
        (_result_) = (_fs_)->entry_count;                            \
      }                                                              \
    } while (0)
#else
#define _BCM_FIELD_ENTRIES_IN_SLICE(_u_, _fg_, _fs_, _result_)       \
    do {                                                             \
      if ((_fg_)->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {         \
        (_result_) = ((_fs_)->entry_count >> 1);                     \
      } else {                                                       \
        (_result_) = (_fs_)->entry_count;                            \
      }                                                              \
    } while (0)
#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Macro:
 *     _BCM_FIELD_TH_NARROW_WIDE_PRIO_ORDER_TCAM_INDEX_GET
 * Purpose:
 *     To retrieve the right priority index for the given index for the groups
 *     operated in narrow wide (80-bit) TCAMs.
 * Note: It is the SW WAR to fix the issue reported RTL JIRA TH-4424.
 */
#if defined(BCM_TOMAHAWK_SUPPORT)
#define _BCM_FIELD_TH_NARROW_WIDE_PRIO_ORDER_TCAM_INDEX_GET(__u__,             \
                                                            __stage__,         \
                                                            __g_flags__,       \
                                                            __slice_no__,      \
                                                            __tid__)           \
        do {                                                                   \
              if (soc_feature(__u__, soc_feature_fp_nw_tcam_prio_order_war) && \
                  ((__stage__ == _BCM_FIELD_STAGE_INGRESS) &&                  \
                   ((__g_flags__ & _FP_GROUP_SPAN_SINGLE_SLICE) &&             \
                    !(__g_flags__ & _FP_GROUP_INTRASLICE_DOUBLEWIDE)))) {      \
                 LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,                    \
                           "Changed TCAM_ID: %d -"), __tid__));                \
                 if (__tid__ % 2) {                                            \
                    __tid__ = 256 + (__slice_no__ * 256) + (__tid__/2);        \
                 } else {                                                      \
                    __tid__ = (__slice_no__ * 256) + (__tid__/2);              \
                 }                                                             \
                 LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,                    \
                           " %d for slice:%d\n\r"), __tid__,__slice_no__));    \
              }                                                                \
        } while(0);
#endif /*BCM_TOMAHAWK_SUPPORT */

/*
 * Macro:_FP_QUAL_INFO_SET
 *
 * Purpose:
 *    Set the fields of a _qual_info_t struct.
 */
#define _FP_QUAL_INFO_SET(qual, field, off, wid, fq)  \
    (fq)->qid    = (qual);                                    \
    (fq)->fpf    = (field);                                   \
    (fq)->offset = (off);                                     \
    (fq)->width  = (wid);                                     \
    (fq)->next   = NULL;
/*
 * Macro:
 *     _FIELD_SELCODE_CLEAR (internal)
 * Purpose:
 *     Set all fields of a field select code to invalid
 * Parameters:
 *     selcode - _field_selcode_t struct
 */

#define _FIELD_SELCODE_CLEAR(_selcode_)      \
                _FP_SELCODE_SET(&(_selcode_), _FP_SELCODE_DONT_CARE)

/*
 * Macro:
 *     _FIELD_NEED_I_WRITE
 * Purpose:
 *     Test if it is necessary to write to "I" version of FP_PORT_FIELD_SEL
 *     table.
 * Parameters:
 *     _u_ - BCM device number
 *     _p_ - port
 *     _m_ - Memory (usually IFP_PORT_FIELD_SEL)
 */
#define _FIELD_NEED_I_WRITE(_u_, _p_, _m_) \
     ((SOC_MEM_IS_VALID((_u_), (_m_))) && \
      (IS_XE_PORT((_u_), (_p_)) || IS_ST_PORT((_u_),(_p_)) || \
       IS_CPU_PORT((_u_),(_p_))))


#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)

/* Egress FP slice modes. */
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2      (0x0)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3      (0x1)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE         (0x2)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY  (0x3)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY  (0x4)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6   (0x5)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_FCOE    (0x6)
#if defined(BCM_TSN_SUPPORT) || defined(BCM_TSN_SR_SUPPORT)
#define  _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_SR      (0x6)
#endif /* BCM_TSN_SUPPORT || BCM_TSN_SR_SUPPORT */


/* Egress FP slice v6 key  modes. */
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6         (0x0)
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6         (0x1)
#define  _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64   (0x2)


/* Egress FP possible keys. */
#define  _BCM_FIELD_EFP_KEY1                    (0x0)  /* IPv4 key.      */
#define  _BCM_FIELD_EFP_KEY2                    (0x1)  /* IPv6 key.      */
#define  _BCM_FIELD_EFP_KEY3                    (0x2)  /* IPv6 dw key.   */
#define  _BCM_FIELD_EFP_KEY4                    (0x3)  /* L2 key.        */
#define  _BCM_FIELD_EFP_KEY1_NO_V4              (0x4)  /* Ip Any key.    */
#define  _BCM_FIELD_EFP_KEY5                    (0x5)  /* HiGiG Key.     */
#define  _BCM_FIELD_EFP_KEY6                    (0x6)  /* HiGiG Key.     */
#define  _BCM_FIELD_EFP_KEY7                    (0x7)  /* Loopback Key.  */
#define  _BCM_FIELD_EFP_KEY8                    (0x8)  /* Bytes after
                                                          L2 Header key. */
#define _BCM_EFP_FIELD_KEY_STRINGS \
{                                  \
    "KEY1",                        \
    "KEY2",                        \
    "KEY3",                        \
    "KEY4",                        \
    "KEY1_NO_V4",                  \
    "KEY5",                        \
    "KEY6",                        \
    "KEY7",                        \
    "KEY8",                        \
}

/* KEY_MATCH type field for EFP_TCAM encoding. for every key. */
#define  _BCM_FIELD_EFP_KEY1_MATCH_TYPE       (0x1)
#define  _BCM_FIELD_EFP_KEY2_MATCH_TYPE       (0x2)
#define  _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE  (0x3)
#define  _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE  (0x4)
#define  _BCM_FIELD_EFP_KEY4_MATCH_TYPE       (0x5)
#define  _BCM_FIELD_EFP_KEY2_KEY4_MATCH_TYPE  (0x6)
#define  _BCM_FIELD_EFP_KEY5_MATCH_TYPE       (0x7)
#define  _BCM_FIELD_EFP_KEY5_KEY4_MATCH_TYPE  (0x8)
#define  _BCM_FIELD_EFP_KEY6_MATCH_TYPE       (0x9)

/* Key Match Type Values used in Tomahawk */
#define KEY_TYPE_INVALID            (0x0)
#define KEY_TYPE_IPv4_SINGLE        (0x1)
#define KEY_TYPE_IPv6_SINGLE        (0x2)
#define KEY_TYPE_IPv6_DOUBLE        (0x3)
#define KEY_TYPE_IPv4_L2_L3_DOUBLE  (0x4)
#define KEY_TYPE_L2_SINGLE          (0x5)
#define KEY_TYPE_IPv4_IPv6_DOUBLE   (0x6)
#define KEY_TYPE_HIGIG_DOUBLE       (0x9)
#define KEY_TYPE_LOOPBACK_DOUBLE    (0xa)
#define KEY_TYPE_L2_DOUBLE          (0xb)
#define KEY_TYPE_FCOE_SINGLE        (0xc)
#define KEY_TYPE_FCOE_DOUBLE        (0xd)

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#define FPF_SZ_MAX                           (32)

#define _FP_HASH_SZ(_fc_)   \
           (((_fc_)->flags & _FP_EXTERNAL_PRESENT) ? (0x1000) : (0x100))
#define _FP_HASH_INDEX_MASK(_fc_) (_FP_HASH_SZ(_fc_) - 1)

#define _FP_HASH_INSERT(_hash_ptr_, _inserted_ptr_, _index_)    \
           do {                                                 \
              (_inserted_ptr_)->next = (_hash_ptr_)[(_index_)]; \
              (_hash_ptr_)[(_index_)] = (_inserted_ptr_);       \
           } while (0)

#define _FP_HASH_REMOVE(_hash_ptr_, _entry_type_, _removed_ptr_, _index_)  \
           do {                                                    \
               _entry_type_ *_prev_ent_ = NULL;                    \
               _entry_type_ *_lkup_ent_ = (_hash_ptr_)[(_index_)]; \
               while (NULL != _lkup_ent_) {                        \
                   if (_lkup_ent_ != (_removed_ptr_))  {           \
                       _prev_ent_ = _lkup_ent_;                    \
                       _lkup_ent_ = _lkup_ent_->next;              \
                       continue;                                   \
                   }                                               \
                   if (_prev_ent_!= NULL) {                        \
                       _prev_ent_->next = (_removed_ptr_)->next;   \
                   } else {                                        \
                       (_hash_ptr_)[(_index_)] = (_removed_ptr_)->next; \
                   }                                               \
                   break;                                          \
               }                                                   \
           } while (0)

#define _FP_HINTS_LINKLIST_INSERT(_head_, _node_)     \
    do {                                              \
        if (*_head_ != NULL) {                        \
            (_node_)->next  = (*_head_);        \
        }                                             \
        (*_head_) = (_node_);                         \
    }while (0)

#define _FP_HINTS_LINKLIST_DELETE(_head_, _entry_type_, _node_)        \
    do {                                                               \
        _entry_type_ *prev = NULL;                                     \
        _entry_type_ *curr = NULL;                                     \
        for (prev = curr = *_head_; curr != NULL; curr = curr->next) { \
            if (curr == _node_) {                                      \
                prev->next = curr->next;                               \
                if (curr == *_head_) {                                 \
                    *_head_ = (*_head_)->next;                         \
                }                                                      \
                if (curr->hint != NULL) {                              \
                    sal_free (curr->hint);                             \
                    curr->hint = NULL;                                 \
                }                                                      \
                sal_free (curr);                                       \
                curr = NULL;                                           \
                break;                                                 \
            }                                                          \
            prev = curr;                                               \
        }                                                              \
    }while (0)

#define _FP_HINTS_LINKLIST_DELETE_ALL(_head_, _entry_type_)         \
    do {                                                            \
        _entry_type_ *node = NULL;                                  \
        while (*_head_ != NULL) {                                   \
            node = *_head_;                                         \
            *_head_ = (*_head_)->next;                              \
            if (node->hint != NULL) {                               \
                sal_free (node->hint);                              \
                node->hint = NULL;                                  \
            }                                                       \
            sal_free (node);                                        \
            node = NULL;                                            \
        }                                                           \
    }while (0)

#define _FP_LINKLIST_INSERT(_head_, _node_, _rc_)                   \
    do {                                                            \
        if ((_head_ == NULL) || (_node_ == NULL)) {                 \
            *_rc_ = BCM_E_PARAM;                                    \
        } else {                                                    \
            if (*_head_ != NULL) {                                  \
                (_node_)->next  = (*_head_);                        \
            }                                                       \
            (*_head_) = (_node_);                                   \
            *_rc_ = BCM_E_NONE;                                     \
        }                                                           \
    }while (0)

#define _FP_ACTION_IS_COLOR_BASED(action)               \
     ((bcmFieldActionRpDrop <= (action) &&              \
      (action) <= bcmFieldActionYpPrioIntCancel) ||  \
      (bcmFieldActionGpDrop <= (action)                 \
       && (action) <= bcmFieldActionGpPrioIntCancel))

/* Different types of packet types for External TCAM */
#define _FP_EXT_NUM_PKT_TYPES 3
#define _FP_EXT_L2   0
#define _FP_EXT_IP4  1
#define _FP_EXT_IP6  2

/*
 * Different types of Databases in External TCAM (Triumph)
 * Each type of database is considered as a slice (in S/W)
 * The number of entries in each type of database is configurable
 * The field_slice_t.num_ext_entries denotes the number of entries
 *
 * See include/soc/er_tcam.h for the database types:
 * slice_numb    database type (soc_tcam_partition_type_t)   meaning
 *     0         TCAM_PARTITION_ACL_L2C                      ACL_144_L2
 *     1         TCAM_PARTITION_ACL_L2                       ACL_L2
 *     2         TCAM_PARTITION_ACL_IP4C                     ACL_144_IPV4
 *     3         TCAM_PARTITION_ACL_IP4                      ACL_IPV4
 *     4         TCAM_PARTITION_ACL_L2IP4                    ACL_L2_IPV4
 *     5         TCAM_PARTITION_ACL_IP6C                     ACL_144_IPV6
 *     6         TCAM_PARTITION_ACL_IP6S                     ACL_IPV6_SHORT
 *     7         TCAM_PARTITION_ACL_IP6F                     ACL_IPV6_FULL
 *     8         TCAM_PARTITION_ACL_L2IP6                    ACL_L2_IPV6
 */
#define _FP_EXT_ACL_144_L2     0
#define _FP_EXT_ACL_L2         1
#define _FP_EXT_ACL_144_IPV4   2
#define _FP_EXT_ACL_IPV4       3
#define _FP_EXT_ACL_L2_IPV4    4
#define _FP_EXT_ACL_144_IPV6   5
#define _FP_EXT_ACL_IPV6_SHORT 6
#define _FP_EXT_ACL_IPV6_FULL  7
#define _FP_EXT_ACL_L2_IPV6    8
#define _FP_EXT_NUM_PARTITIONS  9

extern soc_tcam_partition_type_t _bcm_field_fp_tcam_partitions[];
extern soc_mem_t _bcm_field_ext_data_mask_mems[];
extern soc_mem_t _bcm_field_ext_data_mems[];
extern soc_mem_t _bcm_field_ext_mask_mems[];
extern soc_mem_t _bcm_field_ext_policy_mems[];
extern soc_mem_t _bcm_field_ext_counter_mems[];


/*
 * Structure for Group auto-expansion feature
 *     S/W copy of FP_SLICE_MAP
 */

typedef struct _field_virtual_map_s {
    int valid;           /* Map is valid flag.                           */
    int vmap_key;        /* Key = Physical slice number + stage key base */
    int virtual_group;   /* Virtual group id.                            */
    int priority;        /* Virtual group priority.                      */
    int flags;           /* Field group flags.                           */
} _field_virtual_map_t;

/* Number of entries in a single map.*/
#define _FP_VMAP_SIZE       (17)
/* Default virtual map id.           */
#define _FP_VMAP_DEFAULT    (_FP_EXT_L2)
/* Number of virtual maps per stage.*/
#define _FP_VMAP_CNT        (_FP_EXT_NUM_PKT_TYPES)

/* Encode physical slice into vmap key. */
#define _FP_VMAP_IDX_TO_KEY(_slice_, _stage_)                          \
    (((_stage_) == _BCM_FIELD_STAGE_EXTERNAL) ?                        \
     ((_FP_VMAP_SIZE_) - 1 ) :  (_slice_))

/*
 * Typedef:
 *     _bcm_field_device_sel_t
 * Purpose:
 *     Enumerate per device controls used for qualifier selection.
 */
typedef enum _bcm_field_device_sel_e {
    _bcmFieldDevSelDisable,          /* Don't care selection.            */
    _bcmFieldDevSelInnerVlanOverlay, /* Inner vlan overlay enable.       */
    _bcmFieldDevSelIntrasliceVfpKey, /* Intraslice vfp key select.       */
    _bcmFieldDevSelCount             /* Always Last. Not a usable value. */
} _bcm_field_device_sel_t;

/*
 * Typedef:
 *     _bcm_field_fwd_entity_sel_t
 * Purpose:
 *     Enumerate Forwarding Entity Type selection.
 */
typedef enum _bcm_field_fwd_entity_sel_e {
    _bcmFieldFwdEntityAny,           /* Don't care selection.            */
    _bcmFieldFwdEntityMplsGport,     /* MPLS gport selection.            */
    _bcmFieldFwdEntityMimGport,      /* Mim gport selection.             */
    _bcmFieldFwdEntityWlanGport,     /* Wlan gport selection.            */
    _bcmFieldFwdEntityL3Egress,      /* Next hop selection.              */
    _bcmFieldFwdEntityGlp,           /* (MOD/PORT/TRUNK) selection.      */
    _bcmFieldFwdEntityModPortGport,  /* MOD/PORT even if port is part of
                                        the trunk.                       */
    _bcmFieldFwdEntityMulticastGroup,/* Mcast group id selection.        */
    _bcmFieldFwdEntityMultipath,     /* Multpath id selection.           */
    _bcmFieldFwdEntityPortGroupNum,  /* Ingress port group and number */
    _bcmFieldFwdEntityCommonGport,   /* Mod/Port gport or MPLS/MiM/
                                        Wlan/Niv gport selection.        */
    _bcmFieldFwdEntityVxlanGport,    /* vxlan gport selection.           */
    _bcmFieldFwdEntityNivGport,      /* Niv gport selection.            */
    _bcmFieldFwdEntityVlanGport,      /* Vlan gport selection.            */
    _bcmFieldFwdEntityCount          /* Always Last. Not a usable value. */
} _bcm_field_fwd_entity_sel_t;


/*
 * Typedef:
 *     _bcm_field_oam_intf_class_sel_t
 * Purpose:
 *     Enumerate Oam Class Id from SVP table selection.
 */
typedef enum _bcm_field_oam_intf_class_sel_e {
    _bcmFieldOamInterfaceClassSelectAny,
    _bcmFieldOamInterfaceClassSelectNiv,
    _bcmFieldOamInterfaceClassSelectMim,
    _bcmFieldOamInterfaceClassSelectVxlan,
    _bcmFieldOamInterfaceClassSelectVlan,
    _bcmFieldOamInterfaceClassSelectMpls,
    _bcmFieldOamInterfaceClassSelectCount
} _bcm_field_oam_intf_class_sel_t;

/*
 * Typedef:
 *     _bcm_field_fwd_field_sel_t
 * Purpose:
 *     Enumerate Forwarding Field Type selection.
 */
typedef enum _bcm_field_fwd_field_sel_e {
    _bcmFieldFwdFieldAny,           /* Don't care selection.            */
    _bcmFieldFwdFieldVrf,           /* L3 Vrf id selection.             */
    _bcmFieldFwdFieldVpn,           /* VPLS/VPWS VFI selection.         */
    _bcmFieldFwdFieldVlan,          /* Forwarding vlan selection.       */
    _bcmFieldFwdFieldCount          /* Always Last. Not a usable value. */
} _bcm_field_fwd_field_sel_t;

/*
 * Typedef:
 *     _bcm_field_aux_tag_sel_t
 * Purpose:
 *     Enumerate Auxiliary Tag Type selection.
 */
typedef enum _bcm_field_aux_tag_sel_e {
    _bcmFieldAuxTagAny = 0,         /* Don't care selection.            */
    _bcmFieldAuxTagVn = 1,          /* VN Tag selection.                */
    _bcmFieldAuxTagCn = 2,          /* CN Tag selection.                */
    _bcmFieldAuxTagMimlHeader = 2, /* MIML Header. */
    _bcmFieldAuxTagFabricQueue = 3, /* Fabric Queue Tag selection.      */
    _bcmFieldAuxTagCustomHeader = 3, /* Custom header */
    _bcmFieldAuxTagMplsFwdingLabel = 4,
    _bcmFieldAuxTagCapwapInfo = 4, /* Capwap Info */
    _bcmFieldAuxTagMplsCntlWord = 5,
    _bcmFieldAuxTagRtag7A = 6,
    _bcmFieldAuxTagRtag7B = 7,
    _bcmFieldAuxTagVxlanOrLLTag = 8, /* Vxlan Flags,24 bit VNI/
                                       16 bit Logical Link ID */
    _bcmFieldAuxTagVxlanReserved = 9, /* Vxlan Header Reserved Field */
    _bcmFieldAuxTagCount = 10        /* Always Last. Not a usable value. */
} _bcm_field_aux_tag_sel_t;

/*
 * Typedef:
 *    _bcm_field_nat_oam_sel_t
 * Purpose:
 *     Enumerate Auxiliary Tag Type selection.
 */
typedef enum _bcm_field_nat_oam_sel_e {
    _bcmFieldNatOamInvalid = 0,   /* NAT & OAM Invalid */
    _bcmFieldNatOamValidNat = 1,  /* NAT is valid */
    _bcmFieldNatOamValidOam = 2,  /* OAM is Valid */
    _bcmFieldNatOamReserved = 3,  /* Reserved Field */
    _bcmFieldNatOamCount = 4     /* Always Last. Not a usable value. */
} _bcm_field_nat_oam_sel_t;

/*
 * Typedef:
 *     _bcm_field_oam_overlay_sel_t
 * Purpose:
 *     Enumerate Oam Overlay Enable selector value.
 */
typedef enum _bcm_field_oam_overlay_sel_e {
    _bcmFieldOamOverlayDisable = 0, /* Oam Overlay Disable */
    _bcmFieldOamOverlayEnable  = 1, /* Oam Overlay Enable */
    _bcmFieldOamOverlayCount   = 2  /* Always Last. Not a usable value. */
} _bcm_field_oam_overlay_sel_t;


/*
 * Typedef:
 *     _bcm_field_slice_sel_t
 * Purpose:
 *     Enumerate per slice controls used for qualifier selection.
 */
typedef enum _bcm_field_slice_sel_e {
    _bcmFieldSliceSelDisable,        /* Don't care selection.             */
    _bcmFieldSliceSelFpf1,           /* Field selector 1.                 */
    _bcmFieldSliceSelFpf2,           /* Field selector 2.                 */
    _bcmFieldSliceSelFpf3,           /* Field selector 3.                 */
    _bcmFieldSliceSelFpf4,           /* Field selector 3.                 */
    _bcmFieldSliceSelExternal,       /* Field selector External key.      */
    _bcmFieldSliceSrcClassSelect,    /* Field source class selector.      */
    _bcmFieldSliceDstClassSelect,    /* Field destination class selector. */
    _bcmFieldSliceIntfClassSelect,   /* Field interface class selector.   */
    _bcmFieldSliceIpHeaderSelect,    /* Inner/Outer Ip header selector.   */
    _bcmFieldSliceLoopbackTypeSelect,/* Loopback vs Tunnel type selector. */
    _bcmFieldSliceIngressEntitySelect,/* Ingress entity type selector.    */
    _bcmFieldSliceSrcEntitySelect,   /* Src entity type selector.    */
    _bcmFieldSliceDstFwdEntitySelect, /* Destination entity type selector. */
    _bcmFieldSliceIp6AddrSelect,      /* Sip/Dip/Dip64_Sip64 selector.     */
    _bcmFieldSliceFwdFieldSelect,     /* Vrf/Vfi/Forwarding vlan selector. */
    _bcmFieldSliceAuxTag1Select,      /* VN/CN/FabricQueue tag selector 1. */
    _bcmFieldSliceAuxTag2Select,      /* VN/CN/FabricQueue tag selector 2. */
    _bcmFieldSliceOamOverLayEnable,   /* OAM Overlay enable selector on TD2+*/
    _bcmFieldSliceFpf1SrcDstClassSelect,
    _bcmFieldSliceFpf3SrcDstClassSelect,
    _bcmFieldSliceSelEgrClassF1,
    _bcmFieldSliceSelEgrClassF2,
    _bcmFieldSliceSelEgrClassF3,
    _bcmFieldSliceSelEgrClassF4,
    _bcmFieldSliceSrcTypeSelect,
    _bcmFieldSliceTtlClassSelect,        /* Secondary selector for
                                           Ttl class */
    _bcmFieldSliceTcpClassSelect,        /* Secondary selector for
                                           Tcp class */
    _bcmFieldSliceTosClassSelect,        /* Secondary selector for
                                           Tos class */
    _bcmFieldSliceSelEgrDvpKey4,      /* Secondary Selector for
                                       * EFP_KEY4_DVP_SELECTORr
                                       */
    _bcmFieldSliceSelEgrMdlKey4,      /* Secondary Selector for
                                       * EFP_KEY4_MDL_SELECTORr
                                       */

    _bcmFieldSliceSelEgrOamOverlayKey4, /* Egress OAM Overlay enable selector
                                          on TD2+ */
    _bcmFieldSliceSelEgrClassF6, /* Select CLASS ID from IFP/L3 next Hop/
                                    L3 intf/ Egress DVP for EFP_KEY6 */
    _bcmFieldSliceSelEgrClassF7, /* Select CLASS ID from IFP/L3 next Hop/
                                    L3 intf/ Egress DVP for EFP_KEY7 */
    _bcmFieldSliceSelEgrClassF8, /* Select IFP Class ID/Egress Port/CPU COS
                                  *  for EFP_KEY8
                                  */
    _bcmFieldSliceSelEgrDvpKey8, /* Secondary Selector for
                                  * EFP_KEY8_DVP_SELECTORr
                                  */
    _bcmFieldSliceOamInterfaceClassSelect, /* OAM SVP class selector on TD2+ */
    _bcmFieldSliceSelEgrDestPortF1,      /* Secondary Selector for KEY1
                                          * EFP_DEST_PORT_SELECTORr
                                          */
    _bcmFieldSliceSelEgrDestPortF5,      /* Secondary Selector for KEY5
                                          * EFP_DEST_PORT_SELECTORr
                                          */
   _bcmFieldSliceSelEgrClassIdBF1,
   _bcmFieldSliceSelEgrClassIdBF2,
   _bcmFieldSliceSelEgrClassIdBF3,
   _bcmFieldSliceSelEgrClassIdBF4,
   _bcmFieldSliceSelEgrClassIdBF5,
   _bcmFieldSliceSelEgrClassIdBF6, /* Select CLASS ID from IFP/L3 next Hop/
                                      L3 intf/ Egress DVP for EFP_KEY6 */
   _bcmFieldSliceSelEgrClassIdBF7, /* Select CLASS ID from IFP/L3 next Hop/
                                      L3 intf/ Egress DVP for EFP_KEY7 */
   _bcmFieldSliceSelEgrClassIdBF8, /* Select IFP Class ID/Egress Port/CPU COS
                                    * for EFP_KEY8
                                    */
   _bcmFieldSliceSelEgrClassKey4,  /* Select L3 CLASS_ID for EFP_KEY4 */
   _bcmFieldSliceSelEgrClassKey8,  /* Select L3 CLASS_ID for EFP_KEY8 */
    _bcmFieldSliceCount               /* Always Last. Not a usable value.  */
} _bcm_field_slice_sel_t;

/*
 * Typedef:
 *     _bcm_field_selector_t
 * Purpose:
 *     Select codes configuration required for qualifer.
 */
typedef struct _bcm_field_selector_s {
    _bcm_field_device_sel_t   dev_sel;         /* Per device Selector.     */
    uint8                     dev_sel_val;     /* Device selector value.   */
    _bcm_field_slice_sel_t    pri_sel;         /* Primary slice selector.  */
    uint8                     pri_sel_val;     /* Primary selector value.  */
    _bcm_field_slice_sel_t    sec_sel;         /* Secondary slice selector.*/
    uint8                     sec_sel_val;     /* Secondary selector value.*/
    _bcm_field_slice_sel_t    ter_sel;         /* Tertiary slice selector. */
    uint8                     ter_sel_val;     /* Tertiary selector value. */
    uint8                     intraslice;      /* Intra-slice double wide. */
    uint8                     secondary;       /* Slice pairing overlay.   */
    uint8                     primary;         /* Primary slice pairing
                                                  overlay.   */
    uint8                     update_count;    /* Number of selcodes updates
                                                  required for selection.*/
} _bcm_field_selector_t;

/* Max Qualifiers Offsets */
#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Current Max offsets value is set as 64.
 * In TH qualifiers can be split across TCAM slices in wide mode.
 * Per-Slice upto 32 extractors are supported in HW.
 */
#define _BCM_FIELD_QUAL_OFFSET_MAX  64
#else
#define _BCM_FIELD_QUAL_OFFSET_MAX  3
#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Field Qualifier Type
 */
typedef enum _bcm_field_qual_type_e {
     _bcmFieldQualifierTypeDefault = 0,
     _bcmFieldQualifierTypePresel  = 1,
     _bcmFieldQualifierTypeCount   = 2
} _bcm_field_qual_type_t;


/*
 * Typedef:
 *     _bcm_field_qual_offset_t
 * Purpose:
 *     Qualifier offsets in FP tcam.
 */
typedef struct _bcm_field_qual_offset_s {
    soc_field_t field;                              /* Field name.            */
    uint8       num_offsets;                        /* Number of Offsets.     */
    uint16      offset[_BCM_FIELD_QUAL_OFFSET_MAX]; /* Qualifier offset.      */
    uint8       width[_BCM_FIELD_QUAL_OFFSET_MAX];  /* Qualifier offset width.*/
    uint8       secondary;
    uint8       bit_pos;                            /* Qualifier Bit Position.*/
    uint16      qual_width;                         /* Actual Qualifier Width.*/
    uint8       flags;                              /* Qualifier Flags.       */
} _bcm_field_qual_offset_t;

/*
 * Typedef:
 *     _bcm_field_qual_data_t
 * Purpose:
 *     Qualifier data/mask field buffer.
 */
#define _FP_QUAL_DATA_WORDS  (8)
typedef uint32 _bcm_field_qual_data_t[_FP_QUAL_DATA_WORDS];   /* Qualifier data/mask buffer. */

#define _FP_QUAL_DATA_CLEAR(_data_) \
        sal_memset((_data_), 0, _FP_QUAL_DATA_WORDS * sizeof(uint32))


/*
 * Typedef:
 *     _bcm_field_qual_conf_s
 * Purpose:
 *     Select codes/offsets and other qualifier attributes.
 */
typedef struct _bcm_field_qual_conf_s {
    _bcm_field_selector_t     selector;    /* Per slice/device
                                              configuration required. */
    _bcm_field_qual_offset_t  offset;      /* Qualifier tcam offsets. */
} _bcm_field_qual_conf_t;

/*
 * Typedef:
 *     _bcm_field_entry_part_qual_s
 * Purpose:
 *     Field entry part qualifiers information.
 */
typedef struct _bcm_field_group_qual_s {
    uint16                   *qid_arr;    /* Qualifier id.           */
    _bcm_field_qual_offset_t *offset_arr; /* Qualifier tcam offsets. */
    uint16                   size;        /* Qualifier array size.   */
} _bcm_field_group_qual_t;

/*
 * Typedef:
 *    _FP_QUAL_OFFSET_XXX
 * Purpose:
 *    Field qualifier offset attribute flags.
 */
#define _BCM_FIELD_QUAL_OFFSET_DEFAULT                0
#define _BCM_FIELD_QUAL_OFFSET_NO_IFP_SUPPORT        (1 << 0)
#define _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT         (1 << 1)
#define _BCM_FIELD_QUAL_OFFSET_NO_FT_SUPPORT         (1 << 2)
#define _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE (1 << 3)
#define _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2   (1 << 4)
#define _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3   (1 << 5)


/* _FP_QUAL_* macros require the following declaration
   in any function which uses them.
 */
#define _FP_QUAL_DECL \
    int _rv_;         \
    _bcm_field_qual_conf_t _fp_qual_conf_; \
    soc_field_t _key_fld_ = SOC_IS_TD_TT(unit) ? KEYf : DATAf;

/* Add qualifier to stage qualifiers set. */
#define __FP_QUAL_EXT_ADD(_unit_, _stage_fc_, _qual_id_,                      \
                          _dev_sel_, _dev_sel_val_,                           \
                          _pri_sel_, _pri_sel_val_,                           \
                          _sec_sel_, _sec_sel_val_,                           \
                          _ter_sel_, _ter_sel_val_,                           \
                          _intraslice_,                                       \
                          _offset_0_, _width_0_,                              \
                          _offset_1_, _width_1_,                              \
                          _offset_2_, _width_2_,                              \
                          _secondary_, _primary_)                             \
            do {                                                              \
                _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                (_fp_qual_conf_).selector.dev_sel       = (_dev_sel_);        \
                (_fp_qual_conf_).selector.dev_sel_val   = (_dev_sel_val_);    \
                (_fp_qual_conf_).selector.pri_sel       = (_pri_sel_);        \
                (_fp_qual_conf_).selector.pri_sel_val   = (_pri_sel_val_);    \
                (_fp_qual_conf_).selector.sec_sel       = (_sec_sel_);        \
                (_fp_qual_conf_).selector.sec_sel_val   = (_sec_sel_val_);    \
                (_fp_qual_conf_).selector.ter_sel       = (_ter_sel_);        \
                (_fp_qual_conf_).selector.ter_sel_val   = (_ter_sel_val_);    \
                (_fp_qual_conf_).selector.intraslice    = (_intraslice_);     \
                (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                (_fp_qual_conf_).offset.num_offsets     = 3;                  \
                (_fp_qual_conf_).offset.offset[0]       = _offset_0_ ;        \
                (_fp_qual_conf_).offset.width[0]        = _width_0_;          \
                (_fp_qual_conf_).offset.offset[1]       = _offset_1_ ;        \
                (_fp_qual_conf_).offset.width[1]        = _width_1_;          \
                (_fp_qual_conf_).offset.offset[2]       = _offset_2_ ;        \
                (_fp_qual_conf_).offset.width[2]        = _width_2_;          \
                (_fp_qual_conf_).offset.secondary       = (_secondary_);      \
                (_fp_qual_conf_).selector.secondary    = (_secondary_);       \
                (_fp_qual_conf_).selector.primary       = (_primary_);        \
                _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),     \
                                             (_qual_id_), &(_fp_qual_conf_)); \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

#define _FP_QUAL_EXT_ADD(_unit_, _stage_fc_, _qual_id_,         \
                         _dev_sel_, _dev_sel_val_,              \
                         _pri_sel_, _pri_sel_val_,              \
                         _sec_sel_, _sec_sel_val_,              \
                         _intraslice_,                          \
                         _offset_0_, _width_0_,                 \
                         _offset_1_, _width_1_,                 \
                         _offset_2_, _width_2_,                 \
                         _secondary_)                           \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),      \
                      (_dev_sel_), (_dev_sel_val_),             \
                      (_pri_sel_), (_pri_sel_val_),             \
                      (_sec_sel_), (_sec_sel_val_),             \
                      _bcmFieldDevSelDisable, 0,                \
                      (_intraslice_),                           \
                      (_offset_0_), (_width_0_),                \
                      (_offset_1_), (_width_1_),                \
                      (_offset_2_), (_width_2_),                \
                      (_secondary_), 0                          \
                      )

#define _FP_QUAL_PRIMARY_EXT_ADD(_unit_, _stage_fc_, _qual_id_,     \
                         _dev_sel_, _dev_sel_val_,              \
                         _pri_sel_, _pri_sel_val_,              \
                         _sec_sel_, _sec_sel_val_,              \
                         _intraslice_,                          \
                         _offset_0_, _width_0_,                 \
                         _offset_1_, _width_1_,                 \
                         _offset_2_, _width_2_,                 \
                         _secondary_, _primary_)                \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),      \
                      (_dev_sel_), (_dev_sel_val_),             \
                      (_pri_sel_), (_pri_sel_val_),             \
                      (_sec_sel_), (_sec_sel_val_),             \
                      _bcmFieldDevSelDisable, 0,                \
                      (_intraslice_),                           \
                      (_offset_0_), (_width_0_),                \
                      (_offset_1_), (_width_1_),                \
                      (_offset_2_), (_width_2_),                \
                      (_secondary_), (_primary_)                \
                      )

#define _FP_QUAL_PRIMARY_ADD(_unit_, _stage_fc_, _qual_id_,                            \
                     _pri_slice_sel_, _pri_slice_sel_val_,                     \
                     _offset_0_, _width_0_)                                    \
                     _FP_QUAL_PRIMARY_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      _bcmFieldSliceSelDisable, 0, 0,          \
                                      (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 1)

/* Add single offset & two slice selectors qualifier. */
#define _FP_QUAL_PRIMARY_TWO_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _pri_slice_sel_, _pri_slice_sel_val_,       \
                                   _sec_slice_sel_, _sec_slice_sel_val_,       \
                                   _offset_0_, _width_0_)                      \
                     _FP_QUAL_PRIMARY_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                      (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      (_sec_slice_sel_), (_sec_slice_sel_val_),\
                                    0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 1)

/* Add single offset & three slice selectors qualifier. */
#define _FP_QUAL_PRIMARY_THREE_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,     \
                                     _pri_slice_sel_, _pri_slice_sel_val_, \
                                     _sec_slice_sel_, _sec_slice_sel_val_, \
                                     _ter_slice_sel_, _ter_slice_sel_val_, \
                                     _offset_0_, _width_0_)             \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),               \
                      _bcmFieldDevSelDisable, 0,                        \
                      (_pri_slice_sel_), (_pri_slice_sel_val_),         \
                      (_sec_slice_sel_), (_sec_slice_sel_val_),         \
                      (_ter_slice_sel_), (_ter_slice_sel_val_),         \
                      0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 0, 1)

/* Add basic (single offset & single per slice selector. */
#define _FP_QUAL_ADD(_unit_, _stage_fc_, _qual_id_,                            \
                     _pri_slice_sel_, _pri_slice_sel_val_,                     \
                     _offset_0_, _width_0_)                                    \
                     _FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      _bcmFieldSliceSelDisable, 0, 0,          \
                                      (_offset_0_), (_width_0_), 0, 0, 0, 0, 0)

/* Add single offset & two slice selectors qualifier. */
#define _FP_QUAL_TWO_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,              \
                                   _pri_slice_sel_, _pri_slice_sel_val_,       \
                                   _sec_slice_sel_, _sec_slice_sel_val_,       \
                                   _offset_0_, _width_0_)                      \
                     _FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                      (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      (_sec_slice_sel_), (_sec_slice_sel_val_),\
                                    0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0)

/* Add single offset & three slice selectors qualifier. */
#define _FP_QUAL_THREE_SLICE_SEL_ADD(_unit_, _stage_fc_, _qual_id_,     \
                                     _pri_slice_sel_, _pri_slice_sel_val_, \
                                     _sec_slice_sel_, _sec_slice_sel_val_, \
                                     _ter_slice_sel_, _ter_slice_sel_val_, \
                                     _offset_0_, _width_0_)             \
    __FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),               \
                      _bcmFieldDevSelDisable, 0,                        \
                      (_pri_slice_sel_), (_pri_slice_sel_val_),         \
                      (_sec_slice_sel_), (_sec_slice_sel_val_),         \
                      (_ter_slice_sel_), (_ter_slice_sel_val_),         \
                      0, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0, 0)

/* Add single offset & two slice selectors qualifier. */
#define _FP_QUAL_INTRASLICE_ADD(_unit_, _stage_fc_, _qual_id_,                 \
                                _pri_slice_sel_, _pri_slice_sel_val_,          \
                                _sec_slice_sel_, _sec_slice_sel_val_,          \
                                _offset_0_, _width_0_)                         \
                     _FP_QUAL_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),      \
                                      _bcmFieldDevSelDisable, 0,               \
                                      (_pri_slice_sel_), (_pri_slice_sel_val_),\
                                      (_sec_slice_sel_), (_sec_slice_sel_val_),\
                                  0x1, (_offset_0_), (_width_0_), 0, 0, 0, 0, 0)


/* Add qualifier to stage qualifiers set. */
#define __FP_QUAL_MULTI_EXT_ADD(_unit_, _stage_fc_, _qual_id_,                \
                                _dev_sel_, _dev_sel_val_,                     \
                                _pri_sel_, _pri_sel_val_,                     \
                                _sec_sel_, _sec_sel_val_,                     \
                                _ter_sel_, _ter_sel_val_,                     \
                                _intraslice_, _num_offsets_,                  \
                                _offset_arr_, _width_arr_)                    \
            do {                                                              \
                _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                (_fp_qual_conf_).selector.dev_sel       = (_dev_sel_);        \
                (_fp_qual_conf_).selector.dev_sel_val   = (_dev_sel_val_);    \
                (_fp_qual_conf_).selector.pri_sel       = (_pri_sel_);        \
                (_fp_qual_conf_).selector.pri_sel_val   = (_pri_sel_val_);    \
                (_fp_qual_conf_).selector.sec_sel       = (_sec_sel_);        \
                (_fp_qual_conf_).selector.sec_sel_val   = (_sec_sel_val_);    \
                (_fp_qual_conf_).selector.ter_sel       = (_ter_sel_);        \
                (_fp_qual_conf_).selector.ter_sel_val   = (_ter_sel_val_);    \
                (_fp_qual_conf_).selector.intraslice    = (_intraslice_);     \
                (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                (_fp_qual_conf_).offset.num_offsets     = (_num_offsets_);    \
                sal_memcpy(&(_fp_qual_conf_).offset.offset, _offset_arr_,     \
                     sizeof(_fp_qual_conf_.offset.offset)*_num_offsets_t);    \
                sal_memcpy(&(_fp_qual_conf_).offset.width, _width_arr_,       \
                     sizeof(_fp_qual_conf_.offset.width)*_num_offsets_t);     \
                (_fp_qual_conf_).selector.secondary    = 0;                   \
                _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),     \
                                             (_qual_id_), &(_fp_qual_conf_)); \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

#define _FP_QUAL_MULTI_EXT_ADD(_unit_, _stage_fc_, _qual_id_,         \
                               _dev_sel_, _dev_sel_val_,              \
                               _pri_sel_, _pri_sel_val_,              \
                               _sec_sel_, _sec_sel_val_,              \
                               _intraslice_,                          \
                               _num_offsets_,                         \
                               _offset_arr_, _width_arr_)             \
       __FP_QUAL_MULTI_EXT_ADD((_unit_), (_stage_fc_), (_qual_id_),   \
                         (_dev_sel_), (_dev_sel_val_),             \
                         (_pri_sel_), (_pri_sel_val_),             \
                         (_sec_sel_), (_sec_sel_val_),             \
                          _bcmFieldDevSelDisable, 0,               \
                         (_intraslice_),                           \
                          _num_offsets_,                           \
                          _offset_arr_, _width_arr_)


/* Add multiple offsets. */
#define _FP_QUAL_MULTI_ADD(_unit_, _stage_fc_, _qual_id_,                      \
                           _pri_slice_sel_, _pri_slice_sel_val,                \
                           _num_offsets_, offset_arr_, _width_arr_)            \
                  _FP_QUAL_MULTI_EXT_ADD((_unit_), (_stage_fc_),(_qual_id_),   \
                                    _bcmFieldDevSelDisable, 0,                 \
                                    (_pri_slice_sel_), (_pri_slice_sel_val_),  \
                                     _bcmFieldSliceSelDisable, 0, 0,           \
                                     _num_offsets_, _offset_arr_, _width_arr_)

/* Add basic (single offset per slice selector with qualifier bit position. */
#define _FP_QUAL_MULTI_INFO_ADD(_unit_, _stage_fc_, _qual_id_,                 \
                            _pri_slice_sel_, _pri_slice_sel_val_,              \
                            _offset_0_, _width_0_, _actual_width_,             \
                            _bit_pos_)                                         \
         do {                                                                  \
             _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                     \
            (_fp_qual_conf_).selector.pri_sel       = (_pri_slice_sel_);       \
            (_fp_qual_conf_).selector.pri_sel_val   = (_pri_slice_sel_val_);   \
            (_fp_qual_conf_).offset.field           = (_key_fld_);             \
            (_fp_qual_conf_).offset.num_offsets     = 1;                       \
            (_fp_qual_conf_).offset.offset[0]       = (_offset_0_);            \
            (_fp_qual_conf_).offset.width[0]        = (_width_0_);             \
            (_fp_qual_conf_).offset.qual_width      = (_actual_width_);        \
            (_fp_qual_conf_).offset.bit_pos         = (_bit_pos_);             \
             _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),         \
                                          (_qual_id_), &(_fp_qual_conf_));     \
             BCM_IF_ERROR_RETURN(_rv_);                                        \
         } while (0)

#define _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(_q_offset_, _width_)    \
       do {                                                        \
            int i;                                                 \
            _width_ = 0;                                           \
            for (i = 0; i < (_q_offset_)->num_offsets; i++)  {     \
                (_width_) = (_width_) + (_q_offset_)->width[i];    \
            }                                                      \
       } while(0)


/* Add basic (single offset & single per slice selector with flags. */
#define _FP_QUAL_ADD_WITH_FLAGS(_unit_, _stage_fc_, _qual_id_,_flags_,         \
                                _pri_slice_sel_, _pri_slice_sel_val_,          \
                                _offset_0_, _width_0_,                         \
                                _actual_width_, _bit_pos_)                     \
    _FP_QUAL_EXT_ADD_WITH_FLAGS((_unit_), (_stage_fc_),(_qual_id_),(_flags_),  \
                                 _bcmFieldDevSelDisable, 0,                    \
                                (_pri_slice_sel_), (_pri_slice_sel_val_),      \
                                 _bcmFieldSliceSelDisable, 0, 0,               \
                                (_offset_0_), (_width_0_), 0, 0, 0, 0, 0,      \
                                (_actual_width_), (_bit_pos_))

#define _FP_QUAL_EXT_ADD_WITH_FLAGS(_unit_, _stage_fc_, _qual_id_,_flags_,     \
                                    _dev_sel_, _dev_sel_val_,                  \
                                    _pri_sel_, _pri_sel_val_,                  \
                                    _sec_sel_, _sec_sel_val_,                  \
                                    _intraslice_,                              \
                                    _offset_0_, _width_0_,                     \
                                    _offset_1_, _width_1_,                     \
                                    _offset_2_, _width_2_,                     \
                                    _secondary_,                               \
                                    _actual_width_, _bit_pos_)                 \
    __FP_QUAL_EXT_ADD_WITH_FLAGS((_unit_), (_stage_fc_), (_qual_id_),(_flags_),\
                                 (_dev_sel_), (_dev_sel_val_),                 \
                                 (_pri_sel_), (_pri_sel_val_),                 \
                                 (_sec_sel_), (_sec_sel_val_),                 \
                                  _bcmFieldDevSelDisable, 0,                   \
                                 (_intraslice_),                               \
                                 (_offset_0_), (_width_0_),                    \
                                 (_offset_1_), (_width_1_),                    \
                                 (_offset_2_), (_width_2_),                    \
                                 (_secondary_), 0,                             \
                                 (_actual_width_), (_bit_pos_)                 \
                                )

#define __FP_QUAL_EXT_ADD_WITH_FLAGS(_unit_, _stage_fc_, _qual_id_,_flags_,    \
                           _dev_sel_, _dev_sel_val_,                           \
                           _pri_sel_, _pri_sel_val_,                           \
                           _sec_sel_, _sec_sel_val_,                           \
                           _ter_sel_, _ter_sel_val_,                           \
                           _intraslice_,                                       \
                           _offset_0_, _width_0_,                              \
                           _offset_1_, _width_1_,                              \
                           _offset_2_, _width_2_,                              \
                           _secondary_, _primary_,                             \
                           _actual_width_, _bit_pos_)                          \
             do {                                                              \
                 _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                 (_fp_qual_conf_).selector.dev_sel       = (_dev_sel_);        \
                 (_fp_qual_conf_).selector.dev_sel_val   = (_dev_sel_val_);    \
                 (_fp_qual_conf_).selector.pri_sel       = (_pri_sel_);        \
                 (_fp_qual_conf_).selector.pri_sel_val   = (_pri_sel_val_);    \
                 (_fp_qual_conf_).selector.sec_sel       = (_sec_sel_);        \
                 (_fp_qual_conf_).selector.sec_sel_val   = (_sec_sel_val_);    \
                 (_fp_qual_conf_).selector.ter_sel       = (_ter_sel_);        \
                 (_fp_qual_conf_).selector.ter_sel_val   = (_ter_sel_val_);    \
                 (_fp_qual_conf_).selector.intraslice    = (_intraslice_);     \
                 (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                 (_fp_qual_conf_).offset.num_offsets     = 3;                  \
                 (_fp_qual_conf_).offset.offset[0]       = _offset_0_ ;        \
                 (_fp_qual_conf_).offset.width[0]        = _width_0_;          \
                 (_fp_qual_conf_).offset.offset[1]       = _offset_1_ ;        \
                 (_fp_qual_conf_).offset.width[1]        = _width_1_;          \
                 (_fp_qual_conf_).offset.offset[2]       = _offset_2_ ;        \
                 (_fp_qual_conf_).offset.width[2]        = _width_2_;          \
                 (_fp_qual_conf_).offset.secondary       = (_secondary_);      \
                 (_fp_qual_conf_).selector.secondary     = (_secondary_);      \
                 (_fp_qual_conf_).selector.primary       = (_primary_);        \
                 (_fp_qual_conf_).offset.qual_width      = (_actual_width_);   \
                 (_fp_qual_conf_).offset.bit_pos         = (_bit_pos_);        \
                 (_fp_qual_conf_).offset.flags           = (_flags_);          \
                 _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_), (0),     \
                                              (_qual_id_), &(_fp_qual_conf_)); \
                 BCM_IF_ERROR_RETURN(_rv_);                                    \
                } while (0)

/*
 * Typedef:
 *     _bcm_field_qual_t
 * Purpose:
 */
typedef struct _bcm_field_qual_info_s {
    uint16                 qid;          /* Qualifer id.               */
    _bcm_field_qual_conf_t *conf_arr;    /* Configurations array.      */
    uint8                  conf_sz;      /* Configurations array size. */
} _bcm_field_qual_info_t;

/*
 * Typedef:
 *     _qual_info_t
 * Purpose:
 *     Holds format info for a particular qualifier's access parameters. These
 *     nodes are stored in qualifier lists for groups and in the FPFx tables
 *     for each chip.
 */
typedef struct _qual_info_s {
    int                    qid;     /* Which Qualifier              */
    soc_field_t            fpf;     /* FPFx field choice            */
    int                    offset;  /* Offset within FPFx field     */
    int                    width;   /* Qual width within FPFx field */
    struct _qual_info_s    *next;
} _qual_info_t;

/* Typedef:
 *     _field_fpf_info_t
 * Purpose:
 *     Hardware memory details of field qualifier
 */
typedef struct _field_fpf_info_s {
    _qual_info_t      **qual_table;
    bcm_field_qset_t  *sel_table;
    soc_field_t       field;
} _field_fpf_info_t;

/*
 * Struct:
 *     _field_counter32_collect_s
 * Purpose:
 *     Holds the accumulated count of FP Counters
 *         Useful for wrap around and movement.
 *     This is used when h/w counter width is <= 32 bits
 */
typedef struct _field_counter32_collect_s {
    uint64 accumulated_counter;
    uint32 last_hw_value;
} _field_counter32_collect_t;

/*
 * Struct:
 *     _field_counter64_collect_s
 * Purpose:
 *     Holds the accumulated count of FP Counters
 *         Useful for wrap around and movement.
 *     This is used when h/w counter width is > 32 bits
 *         e.g. Bradley, Triumph Byte counters
 */
typedef struct _field_counter64_collect_s {
    uint64 accumulated_counter;
    uint64 last_hw_value;
} _field_counter64_collect_t;

/*
 * Typedef:
 *     _field_meter_bmp_t
 * Purpose:
 *     Meter bit map for tracking allocation state of group's meter pairs.
 */
typedef struct _field_meter_bmp_s {
    SHR_BITDCL  *w;
} _field_meter_bmp_t;

#define _FP_METER_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_METER_BMP_ADD(bmp, mtr)     SHR_BITSET(((bmp).w), (mtr))
#define _FP_METER_BMP_REMOVE(bmp, mtr)  SHR_BITCLR(((bmp).w), (mtr))
#define _FP_METER_BMP_TEST(bmp, mtr)    SHR_BITGET(((bmp).w), (mtr))


/*
 * struct:
 *     _field_meter_pool_s
 * Purpose:
 *     Global meter pool
 * Note:
 *     Note that the 2nd parameter is slice_id and not group_id. This
 *     is because more than one group can share the same slice (pbmp based
 *     groups)
 *
 *     Due to this, it needs to be enforced that for an entry in a group
 *     which is not in the group's first slice (due to group auto-expansion/
 *     virtual groups), slice_id denotes the entry's group's first slice.
 */
typedef struct _field_global_meter_pool_s {
    int8               level;       /* Level meter pool attached.            */
    int                slice_id;    /* First slice which uses this meter pool*/
    uint16             size;        /* Number of valid meters in pool.       */
    uint16             pool_size;   /* Total number of meters in pool.       */
    uint16             free_meters; /* Number of free meters                 */
    uint16             num_meter_pairs; /* Number of meter pairs in a pool.  */
    _field_meter_bmp_t meter_bmp;       /* Meter usage bitmap                */
} _field_meter_pool_t;


typedef struct _field_global_counter_pool_s {
    int8               slice_id;   /* First slice which uses this pool */
    uint16             size;       /* Number of counters in pool */
    uint16             free_cntrs; /* Number of free counters */
    _field_meter_bmp_t cntr_bmp;   /* Counter usage bitmap */
} _field_cntr_pool_t;


/*
 * DATA qualifiers section.
 */

/* Instead of the old model of n sets of m 4-consecutive-byte chunks of ingress
   packet (currently: n = 2 and m = 4), consider all user-defined 4-byte chunks
   of ingress packet data presented to the FP as equivalent -- the only proviso
   is that any one data qualifier cannot use chunks that are in different "udf
   numbers" (since those are in different FP selectors).
   So, call the index of a 4-byte chunk in the set of (n*m) of them the "data
   qualifier index".  This is also the "column index" in the FP_UDF_OFFSET
   table. An API-level data qualifier that uses multiple 4-byte
   chunks will be assigned a data qualifier index (see _field_data_qualifier_t)
   based on the lowest numbered chunk index it uses.
*/
#define BCM_FIELD_USER_DQIDX(udf_num, user_num) \
    ((udf_num) * (BCM_FIELD_USER_MAX_USER_NUM + 1) + (user_num))
/* Maximum number of data qualifiers. */
#define BCM_FIELD_USER_MAX_DQS \
    BCM_FIELD_USER_DQIDX((BCM_FIELD_USER_MAX_UDF_NUM + 1), 0)
#define BCM_FIELD_USER_DQIDX_TO_UDF_NUM(dqidx)   ((dqidx) / (BCM_FIELD_USER_MAX_USER_NUM + 1))
#define BCM_FIELD_USER_DQIDX_TO_USER_NUM(dqidx)  ((dqidx) % (BCM_FIELD_USER_MAX_USER_NUM + 1))

/* Maximum packet depth data qualifier can qualify */
#define _FP_DATA_OFFSET_MAX      (128)

/* Maximum packet depth data qualifier can qualify */
#define _FP_DATA_WORDS_COUNT         (8)

/* Number of UDF_OFFSETS allocated for ethertype based qualifiers. */
#define _FP_DATA_ETHERTYPE_MAX              (8)
/* Number of UDF_OFFSETS allocated for IPv4 protocol based qualifiers. */
#define _FP_DATA_IP_PROTOCOL_MAX            (2)
/* Number of UDF_OFFSETS allocated for IPv6 next header based qualifiers. */
#define _FP_DATA_NEXT_HEADER_MAX            (2)

/* Hw specific config.*/
#define _FP_DATA_DATA0_WORD_MIN     (0)
#define _FP_DATA_DATA0_WORD_MAX     (3)
#define _FP_DATA_DATA1_WORD_MIN     (4)
#define _FP_DATA_DATA1_WORD_MAX     (7)

/* Ethertype match L2 format. */
#define  _BCM_FIELD_DATA_FORMAT_ETHERTYPE (0x3)

/* Ip protocol match . */
#define  _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0 (BCM_FIELD_DATA_FORMAT_IP6 + 0x1)
#define  _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1 (BCM_FIELD_DATA_FORMAT_IP6 + 0x2)
#define  _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0 (BCM_FIELD_DATA_FORMAT_IP6 + 0x3)
#define  _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1 (BCM_FIELD_DATA_FORMAT_IP6 + 0x4)

/* Packet content (data) qualification object flags. */
#define _BCM_FIELD_DATA_QUALIFIER_FLAGS                       \
     (BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST |    \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_IP6_EXTENSIONS_ADJUST | \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_GRE_OPTIONS_ADJUST |    \
      BCM_FIELD_DATA_QUALIFIER_OFFSET_FLEX_HASH |             \
      BCM_FIELD_DATA_QUALIFIER_STAGE_LOOKUP)

/* UDF spec - offset is valid flag   */
#define _BCM_FIELD_USER_OFFSET_VALID   (1 << 31)

#define _BCM_FIELD_USER_OFFSET_FLAGS           \
            (_BCM_FIELD_USER_OFFSET_VALID |    \
             _BCM_FIELD_USER_OPTION_ADJUST |   \
             _BCM_FIELD_USER_GRE_OPTION_ADJUST)

/* Data qualifiers tcam priorities. */
 /* Specified packet l2 format. */
#define _FP_DATA_QUALIFIER_PRIO_L2_FORMAT   (0x01)
 /* Specified number of vlan tags. */
#define _FP_DATA_QUALIFIER_PRIO_VLAN_FORMAT (0x01)
 /* Specified l3 format tunnel types/inner/outer ip type. */
#define _FP_DATA_QUALIFIER_PRIO_L3_FORMAT   (0x01)
 /* Specified MPLS format tunnel. */
#define _FP_DATA_QUALIFIER_PRIO_MPLS_FORMAT (0x01)
 /* Specified FCoE format tunnel. */
#define _FP_DATA_QUALIFIER_PRIO_FCOE_FORMAT (0x01)
 /* Specified ip protocol.  */
#define _FP_DATA_QUALIFIER_PRIO_IPPROTO     (0x01)
 /* Specified ip protocol/ethertype.  */
#define _FP_DATA_QUALIFIER_PRIO_MISC        (0x01)
 /* Specified priority for misc flags */
#define _FP_DATA_QUALIFIER_PRIO_FLAGS       (0x01)
 /* Maximum udf tcam entry priority.  */
#define _FP_DATA_QUALIFIER_PRIO_HIGHEST     (0xff)

/* MPLS data qualifier label priority */
#define _FP_DATA_QUALIFIER_PRIO_MPLS_ANY         (0x00)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_ONE_LABEL   (0x01)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_TWO_LABEL   (0x02)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_THREE_LABEL (0x03)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_FOUR_LABEL  (0x04)
#define _FP_DATA_QUALIFIER_PRIO_MPLS_FIVE_LABEL  (0x05)

/*
 * Typedef:
 *     _field_data_qualifier_s
 * Purpose:
 *     Data qualifiers description structure.
 */
typedef struct _field_data_qualifier_s {
    int    qid;                     /* Qualifier id.                     */
    bcm_field_udf_spec_t *spec;     /* Hw adjusted offsets array.        */
    bcm_field_data_offset_base_t offset_base; /* Offset base adjustment. */
    int    offset;                  /* Master word offset.               */
    uint8  byte_offset;             /* Data offset inside the word.      */
    uint32 hw_bmap;                 /* Allocated hw words.               */
    uint32 flags;                   /* Offset adjustment flags.          */
    uint8  elem_count;              /* Number of hardware elements required. */
    int    length;                  /* Matched data byte length.         */
    struct _field_data_qualifier_s *next;/* Next installed  qualifier.   */
} _field_data_qualifier_t, *_field_data_qualifier_p;

/*
 * Typedef:
 *     _field_data_ethertype_s
 * Purpose:
 *     Ethertype based data qualifiers description structure.
 */
typedef struct _field_data_ethertype_s {
    int ref_count;                  /* Reference count.        */
    uint16 l2;                      /* Packet l2 format.       */
    uint16 vlan_tag;                /* Packet vlan tag format. */
    bcm_port_ethertype_t ethertype; /* Ether type value.       */
    int relative_offset;            /* Packet byte offset      */
                                    /* relative to qualifier   */
                                    /* byte offset.            */
} _field_data_ethertype_t;

/*
 * Typedef:
 *     _field_data_protocol_s
 * Purpose:
 *     Protocol based data qualifiers description structure.
 */
typedef struct _field_data_protocol_s {
    int ip4_ref_count;    /* Ip4 Reference count.    */
    int ip6_ref_count;    /* Ip6 Reference count.    */
    uint32 flags;         /* Ip4/Ip6 flags.          */
    uint8 ip;             /* Ip protocol id.         */
    uint16 l2;            /* Packet l2 format.       */
    uint16 vlan_tag;      /* Packet vlan tag format. */
    int relative_offset;  /* Packet byte offset      */
                          /* relative to qualifier   */
                          /* byte offset.            */
} _field_data_protocol_t;

/*
 * Typedef:
 *     _field_data_tcam_entry_s
 * Purpose:
 *     Field data tcam entry structucture. Used for
 *     tcam entry allocation and organization by relative priority.
 */
typedef struct _field_data_tcam_entry_s {
   uint8 ref_count;           /* udf tcam entry reference count.  */
   uint8 priority;            /* udf tcam entry priority.         */
} _field_data_tcam_entry_t;

/*
 * Typedef:
 *     _field_data_control_s
 * Purpose:
 *     Field data qualifiers control structucture.
 */
typedef struct _field_data_control_s {
   uint32 usage_bmap;                 /* Offset usage bitmap.          */
   _field_data_qualifier_p data_qual; /* List of data qualifiers.      */
                                      /* Ethertype based qualifiers.   */
   _field_data_ethertype_t etype[_FP_DATA_ETHERTYPE_MAX];
                                      /* IP protocol based qualifiers. */
   _field_data_protocol_t  ip[_FP_DATA_IP_PROTOCOL_MAX];
   _field_data_tcam_entry_t *tcam_entry_arr;/* Tcam entries/keys array.*/
   int                     elem_size; /* Number of bytes per element. */
   int                     num_elems; /* Number of elements per UDF. */
} _field_data_control_t;

/* Katana2 can support 170 pp_ports */
#define _FP_ACTION_PARAM_SZ             (6)

#define _FP_ACTION_MAX_PARAM_ENC        (5)
/*
 * Typedef:
 *     _bcm_field_action_offset_t
 * Purpose:
 *     Action offsets in FP policy table.
 */
typedef struct _bcm_field_action_offset_s {
    uint32      flags;     /* Action flags _BCM_FIELD_ACTION_XXX. */
    uint16      offset[_FP_ACTION_PARAM_SZ]; /* Action offset.    */
    uint8       width[_FP_ACTION_PARAM_SZ];  /* Action width.     */
    uint8       aset;      /*Action set */
    int         value[_FP_ACTION_PARAM_SZ];  /* Action Value.     */
    uint32      encode_bmp;  /* Bitmap of offsets which hold action encodings*/
    uint32      param_bmp;  /* Bitmap of offsets which hold user given parameters */
    struct _bcm_field_action_offset_s *next;
} _bcm_field_action_offset_t;

typedef struct _bcm_field_action_conf_s {
    bcm_field_action_t         action;  /* Field Action bcmFieldActionXXX. */
    _bcm_field_action_offset_t *offset; /* Action offset information.      */
} _bcm_field_action_conf_t;


#ifdef BCM_TOMAHAWK_SUPPORT
typedef struct _field_action_params_s {
    int    cosq_new;
    int    nh_ecmp_id;
    uint8  is_dvp;
    uint8  is_trunk;
    uint8  fabric_tag_type;
    uint8  ucast_redirect_control;
    uint8  mcast_redirect_control;
    uint32 flags;
    uint32 i2e_cl_sel;
    uint32 i2e_hg_cl_sel;
    uint32 fabric_tag;
    uint32 redirect_value;
#ifdef INCLUDE_L3
    int32  hw_idx;
    int32  hw_half;
#endif
    int mtp_index;
    int mtp_type_index;
    uint32 eh_tag;
    uint8 eh_tag_mask_index;
} _field_action_params_t;

#define _BCM_FIELD_ACTION_REDIRECT_DGLP     (1 << 0)
#define _BCM_FIELD_ACTION_REDIRECT_DVP      (1 << 1)
#define _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP (1 << 2)
#define _BCM_FIELD_ACTION_REDIRECT_ECMP     (1 << 3)
#define _BCM_FIELD_ACTION_REDIRECT_TRUNK    (1 << 4)
#define _BCM_FIELD_ACTION_NO_IFP_SUPPORT    (1 << 5)
#define _BCM_FIELD_ACTION_NO_EM_SUPPORT     (1 << 6)

#define _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP (1 << 0)
#define _BCM_FIELD_ACTION_L3SWITCH_ECMP     (1 << 1)

#define _FP_INVALID_ACTION_VAL (-1)

/* _FP_ACTION_* macros require the following declaration
 *    in any function which uses them.
 */
#define _FP_ACTION_DECL \
    int _rv_;         \
    _bcm_field_action_offset_t _fp_action_offset_;

/* Add Action to stage actions set. */
#define _FP_ACTION_ADD(_unit_, _stage_fc_, _action_, _flags_,                 \
                       _aset_, _offset1_, _width1_, _value1_,                 \
                               _offset2_, _width2_, _value2_,                 \
                               _offset3_, _width3_, _value3_,                 \
                               _offset4_, _width4_, _value4_,                 \
                               _offset5_, _width5_, _value5_)                 \
            do {                                                              \
                sal_memset(&(_fp_action_offset_), 0,                          \
                           sizeof(_fp_action_offset_));                       \
                (_fp_action_offset_).flags            |= (_flags_);           \
                (_fp_action_offset_).aset        =  (_aset_);                 \
                (_fp_action_offset_).offset[0]   = (_offset1_);               \
                (_fp_action_offset_).width[0]    = (_width1_);                \
                (_fp_action_offset_).value[0]    = (_value1_);                \
                (_fp_action_offset_).offset[1]   = (_offset2_);               \
                (_fp_action_offset_).width[1]    = (_width2_);                \
                (_fp_action_offset_).value[1]    = (_value2_);                \
                (_fp_action_offset_).offset[2]   = (_offset3_);               \
                (_fp_action_offset_).width[2]    = (_width3_);                \
                (_fp_action_offset_).value[2]    = (_value3_);                \
                (_fp_action_offset_).offset[3]   = (_offset4_);               \
                (_fp_action_offset_).width[3]    = (_width4_);                \
                (_fp_action_offset_).value[3]    = (_value4_);                \
                (_fp_action_offset_).offset[4]   = (_offset5_);               \
                (_fp_action_offset_).width[4]    = (_width5_);                \
                (_fp_action_offset_).value[4]    = (_value5_);                \
                (_fp_action_offset_).next      = NULL;                        \
                _rv_ =_bcm_field_action_insert((_unit_), (_stage_fc_),        \
                                       (_action_), &(_fp_action_offset_));    \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

/* Add single offset, width, value for action. */
#define _FP_ACTION_ADD_ONE(_unit_, _stage_fc_, _action_, _flags_,             \
                                   _offset1_, _width1_, _value1_)             \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),        \
                                   (_flags_), 0, (_offset1_), (_width1_),     \
                                      (_value1_), 0, 0, 0, 0, 0, 0, 0, 0, 0,  \
                                      0, 0, 0)

#define _FP_ACTION_ADD_TWO(_unit_, _stage_fc_, _action_, _flags_,              \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), 0, (_offset1_), (_width1_),   \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define _FP_ACTION_ADD_THREE(_unit_, _stage_fc_, _action_, _flags_,            \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), 0, (_offset1_), (_width1_),   \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                      (_value3_), 0, 0, 0, 0, 0, 0)

#define _FP_ACTION_ADD_FOUR(_unit_, _stage_fc_, _action_, _flags_,             \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_,         \
                                        _offset4_, _width4_, _value4_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), 0, (_offset1_), (_width1_),   \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                      (_value3_), (_offset4_), (_width4_),     \
                                      (_value4_), 0, 0, 0)

#define _FP_ACTION_ADD_FIVE(_unit_, _stage_fc_, _action_, _flags_,             \
                                        _offset1_, _width1_, _value1_,         \
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_,         \
                                        _offset4_, _width4_, _value4_,         \
                                        _offset5_, _width5_, _value5_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                      (_flags_), 0, (_offset1_), (_width1_),   \
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                      (_value3_), (_offset4_), (_width4_),     \
                                      (_value4_), (_offset5_), (_width5_),     \
                                                               (_value5_))

/*Add actions along with width, offset, value, flags and action set */
#define _FP_ACTION_WITH_ASET_ADD_ONE(_unit_, _stage_fc_, _action_, _flags_,    \
                                   _aset_, _offset1_, _width1_, _value1_)      \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_), (_action_),        \
                                    (_flags_), (_aset_), (_offset1_),          \
                                    (_width1_), (_value1_), 0, 0, 0, 0, 0,     \
                                          0, 0, 0, 0, 0, 0, 0)

#define _FP_ACTION_WITH_ASET_ADD_TWO(_unit_, _stage_fc_, _action_, _flags_,    \
                                         _aset_, _offset1_, _width1_, _value1_,\
                                        _offset2_, _width2_, _value2_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_), (_action_),        \
                                  (_flags_), (_aset_), (_offset1_), (_width1_),\
                                   (_value1_), (_offset2_), (_width2_),       \
                                   (_value2_), 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define _FP_ACTION_WITH_ASET_ADD_THREE(_unit_, _stage_fc_, _action_, _flags_,  \
                                        _aset_, _offset1_, _width1_, _value1_, \
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                  (_flags_), (_aset_), (_offset1_), (_width1_),\
                                   (_value1_), (_offset2_), (_width2_),        \
                                   (_value2_), (_offset3_), (_width3_),        \
                                   (_value3_), 0, 0, 0, 0, 0, 0)

#define _FP_ACTION_WITH_ASET_ADD_FOUR(_unit_, _stage_fc_, _action_, _flags_,   \
                                         _aset_, _offset1_, _width1_, _value1_,\
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_,         \
                                        _offset4_, _width4_, _value4_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                  (_flags_), (_aset_), (_offset1_), (_width1_),\
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                      (_value3_), (_offset4_), (_width4_),     \
                                      (_value4_), 0, 0, 0)

#define _FP_ACTION_WITH_ASET_ADD_FIVE(_unit_, _stage_fc_, _action_, _flags_,   \
                                         _aset_, _offset1_, _width1_, _value1_,\
                                        _offset2_, _width2_, _value2_,         \
                                        _offset3_, _width3_, _value3_,         \
                                        _offset4_, _width4_, _value4_,         \
                                        _offset5_, _width5_, _value5_)         \
                     _FP_ACTION_ADD((_unit_), (_stage_fc_),(_action_),         \
                                  (_flags_), (_aset_), (_offset1_), (_width1_),\
                                      (_value1_), (_offset2_), (_width2_),     \
                                      (_value2_), (_offset3_), (_width3_),     \
                                      (_value3_), (_offset4_), (_width4_),     \
                                      (_value4_), (_offset5_), (_width5_),     \
                                      (_value5_))

#define ACTION_CHECK(width, param)                          \
                         if (((1 << width) - 1) < param) {  \
                             return BCM_E_PARAM;            \
                         }

#define ACTION_GET(_unit_, _f_ent_, _buf_, _action_, _flags_,  _a_offset_)   \
            do {                                                             \
               int _rv_;                                                     \
               _rv_ = _bcm_field_action_val_get((_unit_), (_f_ent_),         \
                                                   (_buf_), (_action_),      \
                                                   (_flags_), (_a_offset_)); \
               BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

#define ACTION_OFFSET_GET(_unit_, _stagefc_, _action_, _a_offset_, _flags_)  \
            do {                                                             \
               int _rv_;                                                     \
               _rv_ = _bcm_field_action_offset_get((_unit_), (_stagefc_),    \
                                                (_action_), (_a_offset_),    \
                                                              (_flags_));    \
               BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)

#define ACTION_SET(_unit_, _f_ent_, _buf_, _a_offset_)                         \
            do {                                                               \
               int _rv_;                                                       \
               _rv_ = _bcm_field_action_val_set((_unit_), (_f_ent_),           \
                                                   (_buf_),  (_a_offset_)); \
               BCM_IF_ERROR_RETURN(_rv_);                                      \
            } while (0)

#define ACTION_RESOLVE(_unit_, _fent_, _fa_, _params_)                \
            do {                                                      \
                int _rv_;                                             \
                _rv_ = _bcm_field_action_resolve((_unit_), (_fent_),  \
                                                 (_fa_), (_params_)); \
                BCM_IF_ERROR_RETURN(_rv_);                            \
            } while (0)


/* Internal Actions */
typedef enum _bcm_field_internal_action_e {
    _bcmFieldActionEditCtrlId = bcmFieldActionCount,/* [0x00] Edit Ctrl ID.  */
    _bcmFieldActionExtractionCtrlId,                /* [0x01] Extraction Ctrl ID. */
    _bcmFieldActionOpaque1,                         /* [0x02] Opaque1. */
    _bcmFieldActionOpaque2,                         /* [0x03] Opaque2. */
    _bcmFieldActionOpaque3,                         /* [0x04] Opaque3. */
    _bcmFieldActionOpaque4,                         /* [0x05] Opaque4. */
    _bcmFieldActionCount                            /* [0x06] Always last not used. */
} _bcm_field_internal_action_t;

#define _BCM_FIELD_MAX_ACTIONS_PER_DB 16
/*
 * Used to specify the actions that can be taken by the entries in a
 * group
 */
typedef struct _bcm_field_aset_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(_bcmFieldActionCount)];
    bcm_field_action_width_t actions_width[_BCM_FIELD_MAX_ACTIONS_PER_DB];
                             /* Actions width struct, used by
                                bcm_petra_field_action_width_set to
                                set width for certain action, 16 is
                                max_actions_per_db */
} _bcm_field_aset_t;

/* EXTRACTION CTRL IDs */
#define _BCM_FIELD_EXTR_CTRL_ID_INT_ENABLE            0xEE   /* INT (Inband-Telemetry) Encap Enable */
#define _BCM_FIELD_EXTR_CTRL_ID_ING_OUTER_VLAN_INSERT 0x10   /* Insert ING Outer Vlan to Egr Outer Vlan */
#define _BCM_FIELD_EXTR_CTRL_ID_ERSPAN3_VLAN_PKT_COPY 0x1    /* To add Incoming VLAN to ERSPAN3 header */

/* EDIT CTRL IDs */
#define _BCM_FIELD_EDIT_CTRL_ID_INT_ENABLE            0xE0   /* INT (Inband-Telemetry) Encap Enable */
#define _BCM_FIELD_EDIT_CTRL_ID_INT_TURNAROUND        0xD0   /* INT (Inband-Telemetry) Turnaround */ 
#endif /* BCM_TOMAHAWK_SUPPORT */
/*
 * Stage flags.
 */

/* Separate packet byte counters. */
#define _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS (1 << 0)

/* Global meter pools . */
#define _FP_STAGE_GLOBAL_METER_POOLS            (1 << 1)

/* Global counters. */
#define _FP_STAGE_GLOBAL_COUNTERS               (1 << 2)

/* Auto expansion support. */
#define _FP_STAGE_AUTO_EXPANSION                (1 << 3)

/* Slice enable/disable support. */
#define _FP_STAGE_SLICE_ENABLE                  (1 << 4)

/* Only first half of slice is valid.. */
#define _FP_STAGE_HALF_SLICE                    (1 << 5)

/* Global counter pools . */
#define _FP_STAGE_GLOBAL_CNTR_POOLS             (1 << 6)

/* Only first quarter of slice is valid.. */
#define _FP_STAGE_QUARTER_SLICE                 (1 << 7)

#define _FP_STAGE_MULTI_PIPE_COUNTERS           (1 << 8)

#define _FP_STAGE_COMPRESSION                   (1 << 9)

/* NUM_PIPE(unit) defines the number of
   pipes supported for a specific chip */
/* Maximum number of FP instances. */
#if defined (BCM_TOMAHAWK3_SUPPORT)
#define _FP_MAX_NUM_PIPES 8
#else
#define _FP_MAX_NUM_PIPES 4
#endif

#define _FP_GLOBAL_INST (-1)

/* Default FP instance. */
#define _FP_DEF_INST (0)


#if defined(BCM_TOMAHAWK_SUPPORT)
/* Maximum number of logical table partitions supported. */
#define _FP_MAX_LT_PARTS(unit) _bcm_field_th_max_lt_parts_get(unit)

/* Total number of logical tables supported. */
#define _FP_MAX_NUM_LT (32)

/* Maximum value supported for a LT partition in hardware. */
#define _FP_MAX_LT_PART_PRIO(unit) _bcm_field_max_lt_part_prio_get(unit)

/* Total number of extractor levels. */
#define _FP_EXT_LEVEL_COUNT (4)

/*
 * Field Extractors selector macros
 *
 * These are for resolving if the select code has meaning or is really
 * a don't care.
 */
#define _FP_EXT_SELCODE_DONT_CARE (-1)
#define _FP_EXT_SELCODE_DONT_USE (-2)

#define _FP_EXT_SELCODE_SET(_selcode_qset, _selcode) \
    sal_memset((_selcode_qset), (int8)_selcode, sizeof(_field_ext_sel_t));

#define _FP_EXT_SELCODE_CLEAR(_selcode_) \
    _FP_EXT_SELCODE_SET(&(_selcode_), _FP_EXT_SELCODE_DONT_CARE)

/* Maximum number of bus chunks used by a qualifier. */
#define _FP_QUAL_MAX_CHUNKS (128)

/* Qualify Set/Get Flags. */
#define _FP_QUALIFIER_ADD  1 << 0
#define _FP_QUALIFIER_DEL  1 << 1

#if defined (BCM_TOMAHAWK_SUPPORT)

#define BCM_FIELD_METER_POOL_USED_BY_NONE        (0)
#define BCM_FIELD_METER_POOL_USED_BY_IFP         (1)
#define BCM_FIELD_METER_POOL_USED_BY_EM          (2)

#endif
/*
 * Typedef:
 *  _field_em_mode_t
 * Purpose:
 *  Field Exact Match Modes
 */
typedef enum _field_em_mode_e {
    _FieldExactMatchMode128 = 0,
    _FieldExactMatchMode160 = 1,
    _FieldExactMatchMode320 = 2
} _field_em_mode_t;

/*
 * Typedef:
 *  _bcm_field_action_set_t
 * Purpose:
 *  Field Action set size and hw Fields
 */
typedef struct _bcm_field_action_set_s {
    uint16                   size;        /* Action set size.   */
    uint8                    is_color_dependent;
                             /*set to 1 for color dependent action sets */
    soc_field_t              hw_field;
                             /* hardware field name associated with aset */
} _bcm_field_action_set_t;

/*
 * Typedef:
 *  _field_action_set_type_t
 * Purpose:
 *  Field Action Set for EM and IFP
 */
typedef enum _field_action_set_type_e {
    _FieldActionSetNone = 0,
    _FieldActionProtectionSwitchingDropOverrideSet = 1,
    _FieldActionIfpDlbAlternatePathControlSet = 2,
    _FieldActionEcmpDlbActionSet = 3,
    _FieldActionHgtLagDlbActionSet = 4,
    _FieldActionEcmp2RhActionSet = 5,
    _FieldActionEcmp1RhActionSet = 6,
    _FieldActionLagRhActionSet = 7,
    _FieldActionHgtRhActionSet = 8,
    _FieldActionLoopbackProfileActionSet = 9,
    _FieldActionExtractionCtrlIdActionSet = 10,
    _FieldActionOpaque4ActionSet = 11,
    _FieldActionOpaque3ActionSet = 12,
    _FieldActionOpaque2ActionSet = 13,
    _FieldActionOpaque1ActionSet = 14,
    _FieldActionTxTimestampInsertionSet = 15,
    _FieldActionRxTimestampInsertionSet = 16,
    _FieldActionIgnoreFcoEZoneCheckSet = 17,
    _FieldActionGreenToPidSet = 18,
    _FieldActionMirrorOverrideSet = 19,
    _FieldActionNatOverrideSet = 20,
    _FieldActionSflowSet = 21,
    _FieldActionCutThrOverrideSet = 22,
    _FieldActionUrpfOverrideSet = 23,
    _FieldActionTtlOverrideSet = 24,
    _FieldActionLbControlSet = 25,
    _FieldActionDropSet = 26,
    _FieldActionChangeCpuCosSet = 27,
    _FieldActionMirrorSet = 28,
    _FieldActionNatSet = 29,
    _FieldActionCopyToCpuSet = 30,
    _FieldActionL3SwChangeL2Set = 31,
    _FieldActionRedirectSet = 32,
    _FieldActionCounterSet = 33,
    _FieldActionChangeEcnSet = 34,
    _FieldActionChangePktPriSet = 35,
    _FieldActionChangeDscpTosSet = 36,
    _FieldActionChangeDropPrecendenceSet = 37,
    _FieldActionChangeCosOrIntPriSet = 38,
    _FieldActionChangeIntCNSet = 39,
    _FieldActionChangeInputPrioritySet = 40,
    _FieldActionInstrumentationSet = 41,
    _FieldActionEditCtrlIdSet = 42,
    _FieldActionFcoEVsanSet = 43,
    _FieldActionMeterSet = 44,
    _FieldActionTimestampInsertionSet = 45,
    _FieldActionDelayedDropSet = 46,
    _FieldActionDelayedRedirectSet = 47,
    _FieldActionEcmpDlbMonitorSet = 48,
    _FieldActionElephantTrapSet = 49,
    _FieldActionInbandTelemetrySet = 50,
    _FieldActionMacSecModidBase = 51,
    _FieldActionTimestampTypeSet = 52,
    _FieldActionTimestampSource0Set = 53,
    _FieldActionTimestampSource1Set= 54,
    _FieldActionSetCount = 55
} _field_action_set_type_t;
#if defined(BCM_TOMAHAWK_SUPPORT)
/* Aset size for Trident3 */
#define _FP_ASET_DATA_SIZE_FOR_POLICY_NARROW     181
#define _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE       367
#define _FP_ASET_DATA_SIZE_WITH_ALL_ACTIONS      428

/* Max Action profile entries*/
#define _FP_FIELD_ACTION_PROFILE_ENTRIES         32
/* Max Action profile entries per group
   Depends on which mode the group is in
    1 if group is narrow or intraslice mode
    2 if group is in doublewide mode
    3 if group is in triple wide mode
*/
#define MAX_ACT_PROF_ENTRIES_PER_GROUP            3
/* IFP meter pools information. */
#define BCM_FIELD_INGRESS_TH_NUM_METER_POOLS 8
#define BCM_FIELD_INGRESS_TH3_NUM_METER_POOLS 2
#define BCM_FIELD_INGRESS_TH_NUM_METER_PAIRS_PER_POOL 128
#define BCM_FIELD_INGRESS_TD3_NUM_METER_PAIRS_PER_POOL 384
#define BCM_FIELD_ACTION_PRIO_PKT_AND_INT_TEST(action) \
            ((action == bcmFieldActionPrioPktAndIntCancel) \
          || (action == bcmFieldActionGpPrioPktAndIntCancel) \
          || (action == bcmFieldActionYpPrioPktAndIntCancel) \
          || (action == bcmFieldActionRpPrioPktAndIntCancel) \
          || (action == bcmFieldActionPrioPktAndIntTos) \
          || (action == bcmFieldActionGpPrioPktAndIntTos) \
          || (action == bcmFieldActionYpPrioPktAndIntTos) \
          || (action == bcmFieldActionRpPrioPktAndIntTos) \
          || (action == bcmFieldActionPrioPktAndIntNew) \
          || (action == bcmFieldActionGpPrioPktAndIntNew) \
          || (action == bcmFieldActionYpPrioPktAndIntNew) \
          || (action == bcmFieldActionGpPrioPktAndIntNew) \
          || (action == bcmFieldActionPrioPktAndIntCopy) \
          || (action == bcmFieldActionGpPrioPktAndIntCopy) \
          || (action == bcmFieldActionYpPrioPktAndIntCopy) \
          || (action == bcmFieldActionRpPrioPktAndIntCopy))

/* Exact Match Action Set Size in Bits. */
#define EM_ACTION_METER_SET_SIZE                  18
#define EM_ACTION_GREEN_TO_PID_SET_SIZE            1
#define EM_ACTION_MIRROR_OVERRIDE_SET_SIZE         1
#define EM_ACTION_NAT_OVERRIDE_SET_SIZE            1
#define EM_ACTION_SFLOW_SET_SIZE                   1
#define EM_ACTION_CUT_THR_OVERRIDE_SET_SIZE        1
#define EM_ACTION_URPF_OVERRIDE_SET_SIZE           1
#define EM_ACTION_TTL_OVERRIDE_SET_SIZE            1
#define EM_ACTION_LB_CONTROL_SET_SIZE              3
#define EM_ACTION_DROP_SET_SIZE                    6
#define EM_ACTION_CHANGE_CPU_COS_SET_SIZE          8
#define EM_ACTION_MIRROR_SET_SIZE                 12
#define EM_ACTION_NAT_SET_SIZE                    12
#define EM_ACTION_COPY_TO_CPU_SET_SIZE            17
#define EM_ACTION_L3SW_CHANGE_L2_SET_SIZE         23
#define EM_ACTION_EM_REDIRECT_SET_SIZE            25
#define EM_ACTION_COUNTER_SET_SIZE                25
#define EM_ACTION_COUNTER_SET_SIZE_TH2            26
#define EM_ACTION_TIMESTAMP_SET_SIZE              4
#define EM_ACTION_DLB_ECMP_SET_SIZE               2
#define EM_ACTION_DLB_HGT_LAG_SET_SIZE            2
#define EM_ACTION_PROTECTION_SWITCHING_SET_SIZE   1
#define EM_ACTION_INSTRUMENTATION_SET_SIZE        1
#define EM_ACTION_DLB_ALTERNATE_PATH_CONTROL_SET_SIZE  10

/* Exact Match Key and Action Data Size in Bits. */
#define EM_MODE128_KEY_SIZE                      128
#define EM_MODE128_KEY_PART0_SIZE                101
#define EM_MODE128_KEY_PART1_SIZE                 27
#define EM_MODE128_ACTION_DATA_SIZE               50
#define EM_MODE160_KEY_SIZE                      160
#define EM_MODE160_KEY_PART0_SIZE                101
#define EM_MODE160_KEY_PART1_SIZE                 59
#define EM_MODE160_ACTION_DATA_SIZE               18
#define EM_MODE320_KEY_SIZE                      320
#define EM_MODE320_KEY_PART0_SIZE                101
#define EM_MODE320_KEY_PART1_SIZE                101
#define EM_MODE320_KEY_PART2_SIZE                101
#define EM_MODE320_KEY_PART3_SIZE                 17
#define TD3_EM_MODE128_KEY_PART0_SIZE            103
#define TD3_EM_MODE128_KEY_PART1_SIZE             25
#define TD3_EM_MODE160_KEY_PART0_SIZE            103
#define TD3_EM_MODE160_KEY_PART1_SIZE             57
#define TD3_EM_MODE320_KEY_PART0_SIZE            103
#define TD3_EM_MODE320_KEY_PART1_SIZE            105
#define TD3_EM_MODE320_KEY_PART2_SIZE            105
#define TD3_EM_MODE320_KEY_PART3_SIZE              7
#define TH3_EM_MODE128_KEY_PART0_SIZE            115
#define TH3_EM_MODE128_KEY_PART1_SIZE             13
#define TH3_EM_MODE160_KEY_PART0_SIZE            115
#define TH3_EM_MODE160_KEY_PART1_SIZE             45
#define TH3_EM_MODE320_KEY_PART0_SIZE            115
#define TH3_EM_MODE320_KEY_PART1_SIZE            117
#define TH3_EM_MODE320_KEY_PART2_SIZE             88
#define EM_MODE320_ACTION_DATA_SIZE               60

#define EM_ENTRY_PART0_KEYSIZE                   160
#define EM_ENTRY_PART1_KEYSIZE                   160

/* Exact Match Default Policy Action Data Size in Bits. */
#define EM_DEFAULT_POLICY_ACTION_DATA_SIZE        60
#endif
/*
 * Typedef:
 *  _field_em_action_set_t
 * Purpose:
 *  To set individual action set data
 */
typedef struct _field_action_set_s {
    uint8  valid;
    uint32 data[2];
} _field_em_action_set_t;

/*
 * Typedef:
 *     _field_keygen_profiles_t
 * Purpose:
 *     TCAM Key generation program profile tables information structure.
 */
typedef struct _field_keygen_profiles_s {
    soc_profile_mem_t profile;      /* keygen profile.                  */
} _field_keygen_profiles_t;

/*
 * Typedef:
 *     _field_lt_slice_t
 * Purpose:
 *     Logical Table Selection TCAM slice information structure.
 */
typedef struct _field_lt_slice_s {
    uint8 slice_number;             /* Slice number.                        */
    int start_tcam_idx;             /* Slice first entry TCAM index.        */
    int entry_count;                /* Number of entries in the slice.      */
    int free_count;                 /* Number of free entries.              */
    _field_stage_id_t stage_id;     /* Pipeline Stage ID.                   */
    struct _field_lt_entry_s **entries; /* List of entries pointer.         */
    struct _field_presel_entry_s **p_entries; /* List of Presel entries pointer.*/
    struct _field_lt_slice_s *next; /* Linked list of auto expanded groups. */
    struct _field_lt_slice_s *prev; /* Linked list of auto expanded groups. */
    uint16 slice_flags;             /* Slice flags.                         */
    uint16 group_flags;             /* Group flags.                         */
} _field_lt_slice_t;

/*
 * Typedef:
 *     _field_lt_tcam_t
 * Purpose:
 *     Logical Table Selection TCAM Entry Key, Mask and Data information
 *     structure.
 */
typedef struct _field_lt_tcam_s {
    uint32 *key;        /* HW entry key replica.    */
    uint32 *mask;       /* HW entry mask replica.   */
    uint16 key_size;    /* HW entry Key size.       */
    uint32 *data;       /* HW entry data replica.   */
    uint16 data_size;   /* HW entry Data size.      */
} _field_lt_tcam_t;

/*
 * Typedef:
 *     _field_lt_entry_t
 * Purpose:
 *     Logical Table Selection TCAM Entry information structure.
 */
typedef struct _field_lt_entry_s {
    uint32 flags;                    /* Entry flags _FP_ENTRY_XXX.            */
    bcm_field_entry_t eid;           /* Unique identifier.                    */
    int index;                       /* Entry slice index.                    */
    int prio;                        /* Entry priority.                       */
    _field_lt_slice_t *lt_fs;        /* LT slice where entry lives.           */
    struct _field_lt_tcam_s tcam;    /* Fields to be written in logical table
                                        select TCAM.                          */
    struct _field_action_s *actions; /* Lined list of actions per entry.      */
    struct _field_group_s *group;    /* Group with which this entry
                                        associated.                           */
    struct _field_lt_entry_s *next;  /* LT entry lookup linked list           */
} _field_lt_entry_t;

/*
 * Typedef:
 *     _field_lt_config_t
 * Purpose:
 *     Logical Tables configuration information structure.
 */
typedef struct _field_lt_config_s {
    int valid;                          /* Map is valid.                       */
    int lt_id;                          /* Logical Table ID.                   */
    int *lt_part_pri;                   /* Logical table partition priority.   */
    uint32 lt_part_map;                 /* Logical Table to Physical slice
                                           mapping.                            */
    int lt_action_pri;                  /* Logical Table action priority.      */
    int priority;                       /* Group priority.                     */
    int flags;                          /* Group flags.                        */
    bcm_field_entry_t lt_entry;         /* LT selection entry ID.              */
    uint32 lt_ref_count;                /* Reference count for number groups
                                           using this LT ID. */
} _field_lt_config_t;

/*
 * Typedef:
 *     _field_group_lt_status_t
 * Purpose:
 *     Field Group Logical Table Status structure.
 */
typedef struct _field_group_lt_status_s {
    int entries_total;  /* Total possible entries in group.     */
    int entries_free;   /* Unused entries in LT slice.          */
    int entry_count;    /* Number of entries in the group.      */
} _field_group_lt_status_t;

/*
 * Typedef:
 *     _field_ext_sel_t
 * Purpose:
 *     Logical table key generation program control selectors.
 */
typedef struct _field_ext_sel_s {
    int8 l1_e32_sel[4];         /* Level 1 32b extractors.              */
    int8 l1_e16_sel[10];        /* Level 1 16b extractors.              */
    int8 l1_e8_sel[7];          /* Level 1 8b extractors.               */
    int8 l1_e4_sel[10];         /* Level 1 4b extractors.               */
    int8 l1_e2_sel[8];          /* Level 1 2b extractors.               */
    int8 l2_e16_sel[10];        /* Level 2 16b extractors.              */
    int8 l2_e4_sel[16];         /* Level 2 4b extractors.               */
    int8 l2_e2_sel[8];          /* Level 2 2b extractors.               */
    int16 l2_e1_sel[3];         /* Level 2 1b extractors.               */
    int16 l3_e1_sel[2];         /* Level 3 1b extractors.               */
    int8 l3_e2_sel[5];          /* Level 3 2b extractors.               */
    int8 l3_e4_sel[21];         /* Level 3 4b extractors.               */
    int8 pmux_sel[15];          /* Post muxing extractors.              */
    int8 intraslice;            /* Intraslice double wide selection.    */
    int8 secondary;             /* Secondary selector.                  */
    int8 ipbm_present;          /* Post mux IPBM in final key.          */
    int8 normalize_l3_l4_sel;   /* Normalize L3 and L4 address.         */
    int8 normalize_mac_sel;     /* Normalize L2 SA & DA.                */
    int8 aux_tag_a_sel;         /* Auxiliary Tag A Selector.            */
    int8 aux_tag_b_sel;         /* Auxiliary Tag B Selector.            */
    int8 aux_tag_c_sel;         /* Auxiliary Tag C Selector.            */
    int8 aux_tag_d_sel;         /* Auxiliary Tag d Selector.            */
    int8 tcp_fn_sel;            /* TCP function selector.               */
    int8 tos_fn_sel;            /* TOS function selector.               */
    int8 ttl_fn_sel;            /* TTL function selector.               */
    int8 class_id_cont_a_sel;   /* Class ID container A selector.       */
    int8 class_id_cont_b_sel;   /* Class ID container B selector.       */
    int8 class_id_cont_c_sel;   /* Class ID container C selector.       */
    int8 class_id_cont_d_sel;   /* Class ID container D selector.       */
    int8 src_cont_a_sel;        /* SRC container A selector.            */
    int8 src_cont_b_sel;        /* SRC container B selector.            */
    int8 src_dest_cont_0_sel;   /* SRC or DST container 0 selector.     */
    int8 src_dest_cont_1_sel;   /* SRC or DST container 1 selector.     */
    int8 dst_cont_a_sel;        /* DST container 0 selector.            */
    int8 dst_cont_b_sel;        /* DST container 1 selector.            */
    int8 udf_sel;               /* UDF selector.                        */
    uint32 keygen_index;        /* LT keygen program profile index.     */
    int8 ipbm_source;           /* Ingress Port Bitmap Source.          */
    int8 alt_ttl_fn_sel;        /* ALT TTL function selector            */
} _field_ext_sel_t;

/*
 * Typedef:
 *     _field_keygen_ext_sel_t
 * Purpose:
 *     Key Generation extractor selector encodings.
 */
typedef enum _field_keygen_ext_sel_e {
    _FieldKeygenExtSelDisable   = 0,  /* Selector disabled.               */
    _FieldKeygenExtSelL1E32     = 1,  /* Level1 32bit extractor.          */
    _FieldKeygenExtSelL1E16     = 2,  /* Level1 16bit extractor.          */
    _FieldKeygenExtSelL1E8      = 3,  /* Level1 8bit extractor.           */
    _FieldKeygenExtSelL1E4      = 4,  /* Level1 4bit extractor.           */
    _FieldKeygenExtSelL1E2      = 5,  /* Level1 2bit extractor.           */
    _FieldKeygenExtSelL2E16     = 6,  /* Level2 16bit extractor.          */
    _FieldKeygenExtSelL2E4      = 7,  /* 104 Passthru bits.               */
    _FieldKeygenExtSelL3E16     = 8,  /* Level3 16bit extractor.          */
    _FieldKeygenExtSelL3E4      = 9,  /* Level3 4bit extractor.           */
    _FieldKeygenExtSelL3E2      = 10, /* Level3 2bit extractor.           */
    _FieldKeygenExtSelL3E1      = 11, /* Level3 16bit extractor.          */
    _FieldKeygenExtSelL4        = 12, /* Level4 output.                   */
    _FieldKeygenExtSelL4A       = 13, /* Level4 Slice A output.           */
    _FieldKeygenExtSelL4B       = 14, /* Level4 Slice B output.           */
    _FieldKeygenExtSelL4C       = 15, /* Level4 Slice C output.           */
    _FieldKeygenExtSelL2AE16    = 16, /* Level2 Slice A 16bit extractor.  */
    _FieldKeygenExtSelL2BE16    = 17, /* Level2 Slice B 16 bit extractor. */
    _FieldKeygenExtSelL2CE16    = 18, /* Level2 Slice C 16 bit extractor. */
    _FieldKeygenExtSelL2AE4     = 19, /* Level2 Slice A 4bit extractor.   */
    _FieldKeygenExtSelL2BE4     = 20, /* Level2 Slice B 4bit extractor.   */
    _FieldKeygenExtSelL2CE4     = 21, /* Level2 Slice C 4bit extractor.   */
    _FieldKeygenExtSelL3AE16    = 22, /* Level3 Slice A 16bit extractor.  */
    _FieldKeygenExtSelL3BE16    = 23, /* Level3 Slice B 16bit extractor.  */
    _FieldKeygenExtSelL3CE16    = 24, /* Level3 Slice C 16bit extractor.  */
    _FieldKeygenExtSelL3AE4     = 25, /* Level3 Slice A 4bit extractor.   */
    _FieldKeygenExtSelL3BE4     = 26, /* Level3 Slice B 4bit extractor.   */
    _FieldKeygenExtSelL3CE4     = 27, /* Level3 Slice C 4bit extractor.   */
    _FieldKeygenExtSelCount     = 28  /* Always Last. Not a usable value. */
} _field_keygen_ext_sel_t;

#define _BCM_FIELD_KEYGEN_EXT_SEL_STRINGS \
{ \
    "Disable", \
    "L1E32", \
    "L1E16", \
    "L1E8", \
    "L1E4", \
    "L1E2", \
    "L2E16", \
    "L2E4", \
    "L3E16", \
    "L3E4", \
    "L3E", \
    "L3E1", \
    "L4", \
    "L4A", \
    "L4B", \
    "L4C", \
    "L2AE16", \
    "L2BE16", \
    "L2CE16", \
    "L2AE4", \
    "L2BE4", \
    "L2CE4", \
    "L3AE16", \
    "L3BE16", \
    "L3CE16", \
    "L3AE4", \
    "L3BE4", \
    "L3CE4", \
    "Count" \
}

/* Max Preselector Entries Per Group */
#define _FP_PRESEL_ENTRIES_MAX_PER_GROUP  4

/*
 * Typedef:
 *      _field_class_table_bmp_t
 * Purpose:
 *      Class table bitmap for tracking allocation.
 */
typedef struct _field_class_table_bmp_s {
    SHR_BITDCL *w;
} _field_class_table_bmp_t;

#define _FP_CLASS_TABLE_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_CLASS_TABLE_BMP_ADD(bmp, cl)     SHR_BITSET(((bmp).w), (cl))
#define _FP_CLASS_TABLE_BMP_REMOVE(bmp, cl)  SHR_BITCLR(((bmp).w), (cl))
#define _FP_CLASS_TABLE_BMP_TEST(bmp, cl)    SHR_BITGET(((bmp).w), (cl))

/*
 * Typedef:
 *     _field_class_type_t
 * Purpose:
 *      Field Class Types.
 */
typedef enum _field_class_type_e {
    _FieldClassEtherType = 0,
    _FieldClassTtl = 1,
    _FieldClassToS = 2,
    _FieldClassIpProto = 3,
    _FieldClassL4SrcPort = 4,
    _FieldClassL4DstPort = 5,
    _FieldClassTcp = 6,
    _FieldClassSrcCompression = 7,
    _FieldClassDstCompression = 8,
    _FieldClassIpTunnelTtl = 9,
    _FieldClassCount = 10
} _field_class_type_t;

#define _BCM_FIELD_CLASS_TYPE_STRINGS \
{"EtherType",       \
 "TTL",             \
 "TOS",             \
 "IPProtocol",      \
 "L4SrcPort",       \
 "L4DstPort",       \
 "TCP",             \
 "SrcCompression",  \
 "DstCompression",  \
 "IpTunnelTtl"}

/*
 * Typedef:
 *    _field_class_info_s {
 * Purpose:
 *    Field Class Table information.
 */
typedef struct _field_class_info_s {
    uint32 flags;
    _field_class_type_t classtype;
    uint32 total_entries_used;
    uint32 total_entries_available;
    _field_class_table_bmp_t class_bmp;
} _field_class_info_t;

/* Maximum number of FP Class tables. */
#define _FP_MAX_NUM_CLASS_TYPES                  _FieldClassCount

/* Total number of entries in class table */
#define _FP_MAX_NUM_CLASS_TTL                    soc_mem_index_count(unit,TTL_FNm)
#define _FP_MAX_NUM_CLASS_TOS                    soc_mem_index_count(unit,TOS_FNm)
#define _FP_MAX_NUM_CLASS_IP_PROTO               soc_mem_index_count(unit,IP_PROTO_MAPm)
#define _FP_MAX_NUM_CLASS_TCP                    soc_mem_index_count(unit,TCP_FNm)
#define _FP_MAX_NUM_CLASS_SRC_COMPRESSION        soc_mem_index_count(unit,SRC_COMPRESSIONm)
#define _FP_MAX_NUM_CLASS_DST_COMPRESSION        soc_mem_index_count(unit,DST_COMPRESSIONm)
#define _FP_MAX_NUM_CLASS_ETHERTYPE              16
#define _FP_MAX_NUM_CLASS_L4_SRC_PORT            16
#define _FP_MAX_NUM_CLASS_L4_DST_PORT            16
#define _FP_MAX_NUM_CLASS_IPTUNNELTTL            soc_mem_index_count(unit,ALT_TTL_FNm)

/* Class Entry Size */
#define _FP_SIZE_CLASS_TTL                  4
#define _FP_SIZE_CLASS_TOS                  4
#define _FP_SIZE_CLASS_IP_PROTO             4
#define _FP_SIZE_CLASS_TCP                  4
#define _FP_SIZE_CLASS_SRC_COMPRESSION      44
#define _FP_SIZE_CLASS_DST_COMPRESSION      44
#define _FP_SIZE_CLASS_ETHERTYPE            4
#define _FP_SIZE_CLASS_L4_SRC_PORT          4
#define _FP_SIZE_CLASS_L4_DST_PORT          4
#define _FP_SIZE_CLASS_IPTUNNELTTL          4

/* Class Size in Bits*/
#define _FP_QUAL_CLASS_SIZE_TTL                  8
#define _FP_QUAL_CLASS_SIZE_TOS                  8
#define _FP_QUAL_CLASS_SIZE_IP_PROTO             4
#define _FP_QUAL_CLASS_SIZE_TCP                  8
#define _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION      36
#define _FP_QUAL_CLASS_SIZE_DST_COMPRESSION      36
#define _FP_QUAL_CLASS_SIZE_ETHERTYPE            4
#define _FP_QUAL_CLASS_SIZE_L4_SRC_PORT          4
#define _FP_QUAL_CLASS_SIZE_L4_DST_PORT          4
#define _FP_QUAL_CLASS_SIZE_IPTUNNELTTL          8

/* L4 Src/Dst Port Match Types */
#define _FP_MATCH_L4_PORT_TYPE              0
#define _FP_MATCH_EXCHANGE_ID_TYPE          1

/* Src/Dst Compression Key Type */
#define _FP_KEY_RESERVED_TYPE               0
#define _FP_KEY_IPv4_TYPE                   1
#define _FP_KEY_IPv6_TYPE                   2
#define _FP_KEY_FCoE_TYPE                   3

/* Src/Dst Compression Key Type Mask */
#define _FP_KEY_TYPE_MASK                   3

/* Exact Match Lookup Information. */
#define BCM_FIELD_EXACT_MATCH_LOOKUP_0      0
#define BCM_FIELD_EXACT_MATCH_LOOKUP_1      1

#define IS_SUDO_QUALIFIER(qid) \
            ((bcmFieldQualifyStage == qid) \
            || (bcmFieldQualifyStageIngress == qid) \
            || (bcmFieldQualifyStageIngressExactMatch == qid) \
            || (bcmFieldQualifyStageIngressFlowtracker == qid) \
            || (bcmFieldQualifyIp4 == qid) \
            || (bcmFieldQualifyIp6 == qid) \
            || (bcmFieldQualifyExactMatchHitStatus == qid) \
            || (bcmFieldQualifyExactMatchGroupClassId == qid) \
            || (bcmFieldQualifyExactMatchActionClassId == qid) \
            || (bcmFieldQualifyNormalizeIpAddrs == qid) \
            || (bcmFieldQualifyNormalizeMacAddrs == qid))

/*
 * Macro to validate the PortBitmap Qualifiers in the given Qset.
 */
#define _BCM_FIELD_QSET_PBMP_TEST(_qset_)                                    \
          ((BCM_FIELD_QSET_TEST(_qset_, bcmFieldQualifyInPorts)) ||          \
           (BCM_FIELD_QSET_TEST(_qset_, bcmFieldQualifyDevicePortBitmap)) || \
           (BCM_FIELD_QSET_TEST(_qset_, bcmFieldQualifySystemPortBitmap)) || \
           (BCM_FIELD_QSET_TEST(_qset_, bcmFieldQualifySourceGportBitmap)))

#define _BCM_FIELD_IS_PBMP_QUALIFIER(_qid_)                  \
          ((_qid_ == bcmFieldQualifyInPorts) ||              \
           (_qid_ == bcmFieldQualifyDevicePortBitmap) ||     \
           (_qid_ == bcmFieldQualifySystemPortBitmap) ||     \
           (_qid_ == bcmFieldQualifySourceGportBitmap))


#endif /* !BCM_TOMAHAWK_SUPPORT */

#define _FP_MAX_IBUS_CONTS 4    /* Maximum IBUS Containers A/B/C/D supported */

#if defined BCM_TRIDENT3_SUPPORT

/* Ingress Field Bus Container fields */
typedef enum _field_ing_field_bus_cont_e {
   _bcmFieldIngFieldBusCont0 = 0,
   _bcmFieldIngFieldBusCont1 = 1,
   _bcmFieldIngFieldBusCont2 = 2,
   _bcmFieldIngFieldBusContOuterVlan = 3,
   _bcmFieldIngFieldBusCont4 = 4,
   _bcmFieldIngFieldBusContLastCount = 5  /* Last Count - Not in use */
} _field_ing_field_bus_cont_t;

typedef struct _field_auxtag_ing_fbus_cont_status_s {
   _field_ing_field_bus_cont_t cont_val; /* Ing Field Bus container field */
   uint32 ref_count;                     /* Reference count */
} _field_auxtag_ing_fbus_cont_status_t;

#endif /* BCM_TRIDENT3_SUPPORT */

typedef enum _bcm_field_compress_type_e {
    _BCM_FIELD_COMPRESS_TYPE_SRC_IPV4 = 0,
    _BCM_FIELD_COMPRESS_TYPE_DST_IPV4,
    _BCM_FIELD_COMPRESS_TYPE_SRC_IPV6,
    _BCM_FIELD_COMPRESS_TYPE_DST_IPV6,
    _BCM_FIELD_COMPRESS_TYPE_MAX /* Last */
} _bcm_field_compress_type_t;

/*
 * Typedef:
 *     _field_stage_t
 * Purpose:
 *     Pipeline stage field processor information.
 */
typedef struct _field_stage_s {
    _field_stage_id_t      stage_id;        /* Pipeline stage id.           */
    uint32                 flags;           /* Stage flags.                 */
    int                    tcam_sz;         /* Number of entries in TCAM.   */
    int                    tcam_slices;     /* Number of internal slices.   */
    int                    num_instances;   /* Number of active FP instances. */
    int                    num_pipes;       /* Number of pipelines in a stage */
    struct _field_slice_s  *slices[_FP_MAX_NUM_PIPES]; /* Array of slices.*/
    struct _field_range_s  *ranges;         /* List of all ranges allocated.*/
    uint32                 range_id;        /* Seed ID for range creation.  */
                                        /* Virtual map for slice extension */
                                        /* and group priority management.  */
    _field_virtual_map_t   vmap[_FP_MAX_NUM_PIPES][_FP_VMAP_CNT][_FP_VMAP_SIZE];
    bcm_field_qset_t _field_supported_qset;
    _bcm_field_qual_info_t  **f_qual_arr;  /* Stage qualifiers config array. */
    _bcm_field_qual_info_t  **f_presel_qual_arr; /* Presel qualifiers config array. */
    _bcm_field_action_conf_t **f_action_arr;

    int counter_collect_table; /* Used for counter collection in chunks */
    int counter_collect_index; /* Used for counter collection in chunks */

    int num_meter_pools;
    _field_meter_pool_t *meter_pool[_FP_MAX_NUM_PIPES][_FIELD_MAX_METER_POOLS];
#ifdef BCM_TRIUMPH3_SUPPORT
    int num_logical_meter_pools;
    _field_meter_pool_t *logical_meter_pool[_FIELD_MAX_METER_POOLS];
#endif
    unsigned num_cntr_pools;
    _field_cntr_pool_t *cntr_pool[_FIELD_MAX_CNTR_POOLS];

    _field_counter32_collect_t *_field_x32_counters;
                              /* X pipeline 32 bit counter collect      */
#if defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
    _field_counter32_collect_t *_field_ext_counters;
                              /* External counter collect */
#endif /* BCM_EASYRIDER_SUPPORT || BCM_TRIUMPH_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    _field_counter64_collect_t *_field_x64_counters;
                              /* X pipeline 64 bit counter collect */
    _field_counter64_collect_t *_field_y64_counters;
                              /* Y pipeline 64 bit counter collect  */
#endif /* BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
    _field_counter32_collect_t *_field_y32_counters;
                              /* Y pipeline packet counter collect */
#endif /* BCM_SCORPION_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
    _field_counter32_collect_t  *_field_32_counters[_FP_MAX_NUM_PIPES];
    _field_counter64_collect_t  *_field_64_counters[_FP_MAX_NUM_PIPES];
    int                          lt_tcam_sz;     /* Logical table TCAM size.  */
    int                          num_logical_tables;
                                                 /* Number of logical tables. */
    bcm_field_qset_t             presel_qset;
                                                 /* Preselector Qualifiers Set*/
    _field_lt_slice_t            *lt_slices[_FP_MAX_NUM_PIPES];
                                                 /* Array of LT slices.       */
    _field_keygen_profiles_t     keygen_profile[_FP_MAX_NUM_PIPES];
                                                 /* Keygen Profiles.          */
    soc_profile_mem_t            action_profile[_FP_MAX_NUM_PIPES];
                                                 /* Action Profiles.          */
    soc_profile_mem_t            qos_action_profile[_FP_MAX_NUM_PIPES];
                                                 /* QoS Action Profile.       */
    uint32                       lt_action_pri;  /* Logical Action Priority.  */
    int                          num_ext_levels; /* No. of extractor leves.   */
    _field_class_info_t          **class_info_arr[_FP_MAX_NUM_PIPES];
                                                 /* Class Status Array. */
    bcm_field_src_class_mode_t   field_src_class_mode[BCM_PIPES_MAX];
                                                  /* Field Srouce Class Mode */
    _bcm_field_action_set_t      *action_set_ptr; /* Action set pointer */


    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db;

    bcmi_keygen_ext_cfg_db_t **ext_cfg_db_arr;

    int                          group_running_prio; /* This value keeps incrementing while assigning it
                                                        to the group if the group's prio assigned as
                                                        BCM_FIELD_GROUP_PRIO_ANY */
#endif /* BCM_TOMAHAWK_SUPPORT */

    soc_memacc_t               *_field_memacc_counters;
                              /* Memory access info for FP counter fields */

#if defined(BCM_TRIDENT2_SUPPORT)
    soc_profile_mem_t hash_select[2];    /* Hash select action memory profile */
#endif
    soc_profile_mem_t ext_act_profile;      /* Action profile for external. */
    _field_data_control_t *data_ctrl;       /* Data qualifiers control. */
    bcm_field_group_oper_mode_t  oper_mode; /* Group Operational Mode */

#if defined (BCM_TRIDENT3_SUPPORT)
    _field_auxtag_ing_fbus_cont_status_t ifbus_cont_stat[BCM_PIPES_MAX][_FP_MAX_IBUS_CONTS];
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
    _field_ft_info_t           *ft_info;    /* flowtracker specific info. */
#endif

#ifdef BCM_FIREBOLT6_SUPPORT
    _bcm_field_shr_trie_md_t *compress_tries[_BCM_FIELD_COMPRESS_TYPE_MAX];
#endif
    struct _field_stage_s *next;            /* Next pipeline FP stage. */
} _field_stage_t;

/* Indexes into the memory access list for FP counter acceleration */
typedef enum _field_counters_memacc_type_e {
    _FIELD_COUNTER_MEMACC_BYTE,
    _FIELD_COUNTER_MEMACC_PACKET,
    _FIELD_COUNTER_MEMACC_BYTE_Y,
    _FIELD_COUNTER_MEMACC_PACKET_Y,
    _FIELD_COUNTER_MEMACC_NUM,       /* The max size of the memacc list */
    /* If the device doesn't have packet and byte counters in a single
     * counter mem entry, then these aliases are used. */
    _FIELD_COUNTER_MEMACC_COUNTER = _FIELD_COUNTER_MEMACC_BYTE,
    _FIELD_COUNTER_MEMACC_COUNTER_Y = _FIELD_COUNTER_MEMACC_PACKET
} _field_counters_memacc_type_t;

/* Indexes into the memory access list for FP counter acceleration */
typedef enum _field_counters_multi_pipe_memacc_type_e {
    _FIELD_COUNTER_MEMACC_PIPE0_BYTE   = 0,
    _FIELD_COUNTER_MEMACC_PIPE0_PACKET = 1,
    _FIELD_COUNTER_MEMACC_PIPE1_BYTE   = 2,
    _FIELD_COUNTER_MEMACC_PIPE1_PACKET = 3,
    _FIELD_COUNTER_MEMACC_PIPE2_BYTE   = 4,
    _FIELD_COUNTER_MEMACC_PIPE2_PACKET = 5,
    _FIELD_COUNTER_MEMACC_PIPE3_BYTE   = 6,
    _FIELD_COUNTER_MEMACC_PIPE3_PACKET = 7,
    _FIELD_COUNTER_MEMACC_PIPE_NUM     = 8 /* The max size of the
                                               memacc list */
} _field_counters_multi_pipe_memacc_type_t;

#define _FIELD_FIRST_MEMORY_COUNTERS                 (1 << 0)
#define _FIELD_SECOND_MEMORY_COUNTERS                (1 << 1)
#define _FIELD_MULTI_PIPE_MEMORY_COUNTERS            (1 << 3)

#define FILL_FPF_TABLE(_fpf_info, _qid, _offset, _width, _code)        \
    BCM_IF_ERROR_RETURN                                                \
        (_field_qual_add((_fpf_info), (_qid), (_offset), (_width), (_code)))

/* Generic memory allocation routine. */
#define _FP_XGS3_ALLOC(_ptr_,_size_,_descr_)                 \
            do {                                             \
                if (NULL == (_ptr_)) {                       \
                   (_ptr_) = sal_alloc((_size_), (_descr_)); \
                }                                            \
                if((_ptr_) != NULL) {                        \
                    sal_memset((_ptr_), 0, (_size_));        \
                }  else {                                    \
                    LOG_ERROR(BSL_LS_BCM_FP, \
                              (BSL_META("FP Error: Allocation failure %s\n"), (_descr_))); \
                }                                          \
            } while (0)

/*
 * Typedef:
 *     _field_udf_t
 * Purpose:
 *     Holds user-defined field (UDF) hardware metadata.
 */
typedef struct _field_udf_s {
    uint8                  valid;     /* Indicates valid UDF             */
    int                    use_count; /* Number of groups using this UDF */
    bcm_field_qualify_t    udf_num;   /* UDFn (UDF0 or UDF1)             */
    uint8                  user_num;  /* Which user field in UDFn        */
} _field_udf_t;


/*
 * Typedef:
 *     _field_tcam_int_t
 * Purpose:
 *     These are the fields that are written into or read from FP_TCAM_xxx.
 */
typedef struct _field_tcam_s {
    uint32                 *key;
    uint32                 *key_hw;  /* Hardware replica */
    uint32                 *mask;
    uint32                 *mask_hw; /* Hardware replica */
    uint16                 key_size;
    uint32                 f4;
    uint32                 f4_mask;
    uint8                  higig;         /* 0/1 non-HiGig/HiGig    */
    uint8                  higig_mask;    /* 0/1 non-HiGig/HiGig    */
     uint8                 ip_type;
#if defined(BCM_FIREBOLT2_SUPPORT)
    uint8                  drop;         /* 0/1 don't Drop/Drop     */
    uint8                  drop_mask;    /* 0/1 don't Drop/Drop     */
#endif /* BCM_FIREBOLT2_SUPPORT */
} _field_tcam_t;

/*
 * Typedef:
 *     _field_tcam_mem_info_t
 * Purpose:
 *     TCAM memory name and chip specific field names.
 */
typedef struct _field_tcam_mem_info_s{
    soc_mem_t      memory;     /* Tcam memory name.    */
    soc_field_t    key_field;  /* Tcam key field name. */
    soc_field_t    mask_field; /* Tcam mask field name.*/
} _field_tcam_mem_info_t;

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
typedef struct _field_pbmp_s {
    bcm_pbmp_t data;
    bcm_pbmp_t mask;
} _field_pbmp_t;
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

/*
 * Typedef:
 *     _field_counter_t
 * Purpose:
 *     Holds the counter parameters to be written into FP_POLICY_TABLE
 *     (Firebolt) or FP_INTERNAL (Easyrider).
 */
typedef struct _field_counter_s {
    int                    index;
    uint16                 entries;    /* Number of entries using counter */
} _field_counter_t;

/*
 * Typedef:
 *     _field_counter_bmp_t
 * Purpose:
 *     Counter bit map for tracking allocation state of slice's counter pairs.
 */
typedef struct _field_counter_bmp_s {
    SHR_BITDCL  *w;
} _field_counter_bmp_t;

#define _FP_COUNTER_BMP_FREE(bmp, size)   sal_free((bmp).w)
#define _FP_COUNTER_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_COUNTER_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_COUNTER_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))

/* Max number of counter pools. Tomahawk has 20 Flex counter pools which is
 * maximum among all devices. But only 16 are exposed for now.
 */
#define _FIELD_MAX_COUNTER_POOLS 20

/* per Group Counter Pool Bitmap */
typedef struct _field_counter_pool_bmp_s {
    SHR_BITDCL w[_FIELD_MAX_COUNTER_POOLS];
} _field_counter_pool_bmp_t;

#define _FP_COUNTER_POOL_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_COUNTER_POOL_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_COUNTER_POOL_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))


/*
 * Structure for priority management
 * Currently used only on External TCAM
 */
typedef struct _field_prio_mgmt_s {
    int prio;
    uint32 start_index;
    uint32 end_index;
    uint32 num_free_entries;
    struct _field_prio_mgmt_s *prev;
    struct _field_prio_mgmt_s *next;
} _field_prio_mgmt_t;

/* Slice specific flags. */
#define _BCM_FIELD_SLICE_EXTERNAL              (1 << 0)
#define _BCM_FIELD_SLICE_INTRASLICE_CAPABLE    (1 << 1)
#define _BCM_FIELD_SLICE_SIZE_SMALL            (1 << 2)
#define _BCM_FIELD_SLICE_SIZE_LARGE            (1 << 3)
#define _BCM_FIELD_SLICE_IPBM_CAPABLE          (1 << 4)
/* Indicates whether the slice is operationally Enabled */
#define _BCM_FIELD_SLICE_HW_ENABLE             (1 << 7)
/* configure the flag to provide the SW atomicity for a slice. */
#define _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT  (1 << 8)
/* configure the flag if hw supports atomicity for a slice. */
#define _BCM_FIELD_SLICE_HW_ATOMICITY_SUPPORT  (1 << 9)
/* configure the flag if slice supports more than 1 group modes. */
#define _BCM_FIELD_SLICE_MULTIMODE_ENTRIES    (1 << 10)
/* Slices are indepedent */
#define _BCM_FIELD_SLICE_INDEPENDENT          (1 << 11)

#define _FP_INTRA_SLICE_PART_0        (0)
#define _FP_INTRA_SLICE_PART_1        (1)
#define _FP_INTRA_SLICE_PART_2        (2)
#define _FP_INTRA_SLICE_PART_3        (3)

#define _FP_INTER_SLICE_PART_0        (0)
#define _FP_INTER_SLICE_PART_1        (1)
#define _FP_INTER_SLICE_PART_2        (2)

/*
 * Typedef:
 *     _field_slice_t
 * Purpose:
 *     This has the fields specific to a hardware slice.
 * Notes:
 */
typedef struct _field_slice_s {
    uint8                  slice_number;  /* Hardware slice number.         */
    int                    start_tcam_idx;/* Slice first entry tcam index.  */
    int                    entry_count;   /* Number of entries in the slice.*/
    int                    free_count;    /* Number of free entries.        */
    int                    counters_count;/* Number of counters accessible. */
    int                    meters_count;  /* Number of meters accessible.   */
    int                    hw_ent_count;  /* Number of entries installed
                                             in the slice.                  */
    _field_counter_bmp_t   counter_bmp;   /* Bitmap for counter allocation. */
    _field_meter_bmp_t     meter_bmp;     /* Bitmap for meter allocation.   */
    _field_stage_id_t      stage_id;      /* Pipeline stage slice belongs.  */
    bcm_pbmp_t             pbmp;          /* Ports in use by groups.        */

    struct _field_entry_s  **entries;     /* List of entries pointers.      */
    _field_prio_mgmt_t     *prio_mgmt;    /* Priority management of entries.*/

    uint8 pkt_type[_FP_EXT_NUM_PKT_TYPES];/* Packet types supported
                                             by this slice (aka database).  */

    bcm_pbmp_t ext_pbmp[_FP_EXT_NUM_PKT_TYPES];/* Bmap for each packet type.*/

    struct _field_slice_s  *next;  /* Linked list for auto-expand of groups.*/
    struct _field_slice_s  *prev;  /* Linked list for auto-expand of groups.*/
    uint32                 slice_flags;   /* _BCM_FIELD_SLICE_XXX flags.    */
    uint8                  group_flags;   /* Intalled groups _FP_XXX_GROUP. */
    int8                   doublewide_key_select;
                                          /* Key selection for the          */
                                          /* intraslice double wide mode.   */
    int8                   src_class_sel; /* Source lookup class selection.*/
    int8                   dst_class_sel; /* Destination lookup class.     */
    int8                   intf_class_sel;/* Interface class selection.    */
    int8                   loopback_type_sel;/* Loopback/Tunnel selection.  */
    int8                   ingress_entity_sel;/* Ingress entity selection.  */
    int8                   src_entity_sel;    /* Src port/trunk entity selection.          */
    int8                   dst_fwd_entity_sel;/* Destination forwarding     */
    int8                   fwd_field_sel; /* Forwarding field vrf/vpn/vid   */
                                              /* entity selection.          */
    int8                   ttl_class_sel; /* Ttl class Selecti */
    int8                   tos_class_sel; /* Tos class Selecti */
    int8                   tcp_class_sel; /* Tcp class Selecti */
    int8                   aux_tag_1_sel;
    int8                   aux_tag_2_sel;
    int8                   oam_overlay_sel;   /* Oam Overlay Selection*/
    int8                   egr_class_f1_sel;
    int8                   egr_class_f2_sel;
    int8                   egr_class_f3_sel;
    int8                   egr_class_f4_sel;
    int8                   src_dest_class_f1_sel;
    int8                   src_dest_class_f3_sel;
    int8                   egr_key4_dvp_sel;
    int8                   egr_key4_mdl_sel;
    int8                   egr_dest_port_f1_sel;
    int8                   egr_dest_port_f5_sel;
    int8                   egr_oam_overlay_sel;   /* Egress Oam Overlay Selection*/
    int8                   oam_intf_class_sel;   /* Oam SVP Class Id Selection*/
#if defined(BCM_TOMAHAWK_SUPPORT)
    uint32                 lt_map;        /* Bitmap of LT IDs on this slice. */
    uint8                  lt_partition_pri;  /* LT Partition Priority. */
    _field_ext_sel_t       ext_sel[_FP_MAX_NUM_LT];
#endif
} _field_slice_t;

/* Macro: _BCM_FIELD_SLICE_SIZE
 * Purpose:
 *        Given stage, slice get number of entries in the slice.
 */
#define _BCM_FIELD_SLICE_SIZE(_stage_fc_, instance, _slice_)     \
       (((_stage_fc_)->slices[instance] + (_slice_))->entry_count)


#define _FP_GROUP_ENTRY_ARR_BLOCK (0xff)
typedef struct _field_entry_s _field_entry_t;
/*
 * Typedef:
 *     _field_group_t
 * Purpose:
 *     This is the logical group's internal storage structure. It has 1, 2 or
 *     3 slices, each with physical entry structures.
 * Notes:
 *   'ent_qset' should always be a subset of 'qset'.
 */
typedef struct _field_group_s {
    bcm_field_group_t      gid;            /* Opaque handle.                */
    int                    priority;       /* Field group priority.         */
    bcm_field_qset_t       qset;           /* This group's Qualifier Set.   */
    uint32                 flags;          /* Group configuration flags.    */
    _field_slice_t         *slices;        /* Pointer into slice array.     */
    bcm_pbmp_t             pbmp;           /* Ports in use this group.      */
    _field_sel_t           sel_codes[_FP_MAX_ENTRY_WIDTH]; /* Select codes for
                                                              slice(s).     */
    _bcm_field_group_qual_t qual_arr[_FP_MAX_ENTRY_TYPES][_FP_MAX_ENTRY_WIDTH];
                                           /* Qualifiers available in each
                                              individual entry part.        */
    _field_stage_id_t      stage_id;       /* FP pipeline stage id.         */

    _field_entry_t         **entry_arr;    /* FP group entry array.         */
    uint16                 ent_block_count;/* FP group entry array size  .  */
    /*
     * Public data for each group: The number of used and available entries,
     * counters, and meters for a field group.
     */
    bcm_field_group_status_t group_status;
#if defined(BCM_TOMAHAWK_SUPPORT)
    _bcm_field_aset_t        aset;
#else
    bcm_field_aset_t         aset;
#endif /* BCM_TOMAHAWK_SUPPORT */
    _field_counter_pool_bmp_t counter_pool_bmp; /* counter pools in use for
                                                 * this group.
                                                 */
    int                    instance;       /* FP Instance.                   */
#if defined(BCM_TOMAHAWK_SUPPORT)
    _field_ext_sel_t       ext_codes[_FP_MAX_ENTRY_WIDTH]; /* KeyGen select
                                                              codes. */
    _field_lt_slice_t      *lt_slices;     /* Pointer into LT slices arrary. */
    int lt_id;                             /* Logical Table ID.              */
    _field_lt_entry_t      **lt_entry_arr; /* FP Group LT entry array.       */
    uint16                 lt_ent_blk_cnt; /* FP group LT entry array size.  */
    _field_group_lt_status_t lt_grp_status;/* FP Group LT status.            */
    int                     qset_size;     /* Qset key size.                 */
    _bcm_field_group_qual_t presel_qual_arr[_FP_MAX_ENTRY_TYPES][_FP_MAX_ENTRY_WIDTH];
                                           /* Preselector Qualifiers available
                                            * in each individual presel entry part. */
    struct _field_presel_entry_s *presel_ent_arr[_FP_PRESEL_ENTRIES_MAX_PER_GROUP];
                                     /* Preselector entries associated with
                                        the group arranged on priority basis. */
    _field_em_mode_t        em_mode;       /* Exact Match Mode.               */
    int8                    action_profile_idx[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                           /* Action Profile index      */
#endif /* BCM_TOMAHAWK_SUPPORT */
    bcm_field_hintid_t       hintid;         /* Hintid linked to a group */
    int                      action_res_id;  /* Action resolution id */
    int                      vmap_group[_FP_PAIR_MAX];
    uint32                   max_group_size; /* Max Size of a group */

#if defined(BCM_FLOWTRACKER_SUPPORT)
    _field_ft_keygen_ext_codes_t *ft_ext_codes; /* Flowtracker ext. codes. */
#endif
    struct _field_group_s   *next;         /* For storing in a linked-list  */
} _field_group_t;

/*
 * Typedef:
 *     _field_action_t
 * Purpose:
 *     This is the real action storage structure that is hidden behind
 *     the opaque handle bcm_field_action_t.
 */
typedef struct _field_action_s {
    bcm_field_action_t     action;       /* Action type                  */
    uint32                 param[_FP_ACTION_PARAM_SZ];
                                         /* Action specific parameters   */
    int                    hw_index;     /* Allocated hw resource index. */
    int                    old_index;    /* Hw resource to be freed, in  */
                                         /* case action parameters were  */
                                         /* modified.                    */
    uint8                  flags;        /* Field action flags.          */
    bcm_pbmp_t             *egress_pbmp; /* Egress Pbmp for delayed
                                          * actions which need special handling.
                                          */
    uint8                  elephant_pkts_only; /* Enable action only for
                                                * elephant flows.
                                                */
    struct _field_action_s *next;
} _field_action_t;

#define _FP_RANGE_STYLE_FIREBOLT    1


#define PolicyMax(_unit_, _mem_, _field_)                                 \
    ((soc_mem_field_length((_unit_), (_mem_) , (_field_)) < 32) ?         \
        ((1 << soc_mem_field_length((_unit_), (_mem_), (_field_))) - 1) : \
        (0xFFFFFFFFUL))

#define PolicyGet(_unit_, _mem_, _entry_, _field_) \
    soc_mem_field32_get((_unit_), (_mem_), (_entry_), (_field_))

#define PolicySet(_unit_, _mem_, _entry_, _field_, _value_)    \
    soc_mem_field32_set((_unit_), (_mem_), (_entry_), (_field_), (_value_))

#define PolicyCheck(_unit_, _mem_, _field_, _value_)                      \
    if (0 == ((uint32)(_value_) <=                                        \
              (uint32)PolicyMax((_unit_), (_mem_), (_field_)))) {         \
        LOG_ERROR(BSL_LS_BCM_FP,                                        \
                  (BSL_META("FP(unit %d) Error: Policy _value_ %d > %d (max) mem (%d)" \
                        "field (%d).\n"), _unit_, (_value_),            \
                (uint32)PolicyMax((_unit_), (_mem_), (_field_)), (_mem_), \
                (_field_)));                                              \
        return (BCM_E_PARAM);                                             \
    }

/*
 * Typedef:
 *     _field_range_t
 * Purpose:
 *     Internal management of Range Checkers. There are two styles or range
 *     checkers, the Firebolt style that only chooses source or destination
 *     port, without any sense of TCP vs. UDP or inverting. This style writes
 *     into the FP_RANGE_CHECK table. The Easyrider style is able to specify
 *     TCP vs. UDP.
 *     The choice of styles is based on the user supplied flags.
 *     If a Firebolt style checker is sufficient, that will be used. If an
 *     Easyrider style checker is required then that will be used.
 */
typedef struct _field_range_s {
    uint32                 flags;
    bcm_field_range_t      rid;
    bcm_l4_port_t          min, max;
    int                    hw_index;
    uint8                  style;        /* Simple (FB) or more complex (ER) */
    struct _field_range_s *next;
} _field_range_t;

/*
 * Entry status flags.
 */

/*
 * Software entry differs from one in hardware.
 * This flag has to be set for any change in
 * FP_TCAM or FP_POLICY_TABLE
 */
#define _FP_ENTRY_DIRTY                      (1 << 0)

/* Entry is in primary slice. */
#define _FP_ENTRY_PRIMARY                    (1 << 1)

/* Entry is in secondary slice of wide-mode group. */
#define _FP_ENTRY_SECONDARY                  (1 << 2)

/* Entry is in tertiary slice of wide-mode group. */
#define _FP_ENTRY_TERTIARY                   (1 << 3)

/* Entry has an ingress Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_ING0                   (1 << 4)

/* Entry has an ingress 1 Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_ING1                   (1 << 5)

/* Entry has an egress Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_EGR0                   (1 << 6)

/* Entry has an egress 1 Mirror-To-Port reserved. */
#define _FP_ENTRY_MTP_EGR1                   (1 << 7)

/* Second part of double wide intraslice entry. */
#define _FP_ENTRY_SECOND_HALF                (1 << 8)

/* Field entry installed in hw. */
#define _FP_ENTRY_INSTALLED                  (1 << 9)

/* Treat all packets as green. */
#define _FP_ENTRY_COLOR_INDEPENDENT          (1 << 10)

/* Meter installed in secondary slice . */
#define _FP_ENTRY_POLICER_IN_SECONDARY_SLICE   (1 << 11)

/* Counter installed in secondary slice . */
#define _FP_ENTRY_STAT_IN_SECONDARY_SLICE    (1 << 12)

/* Allocate meters/counters from secondary slice. */
#define _FP_ENTRY_ALLOC_FROM_SECONDARY_SLICE (1 << 13)

/* Entry uses secondary overlay */
#define _FP_ENTRY_USES_IPBM_OVERLAY (1 << 14)

/*
 * Entry POLICY_TABLE dirty
 * It can be used to set along with _FP_ENTRY_DIRTY
 * if and only if any change only in FP_POLICY_TABLE fields.
 * It will be set for change in actions/STATs/Policers
 * and will be reset for change in Qualifiers
 * as qualifier needs FP_TCAM write.
 */
#define _FP_ENTRY_POLICY_TABLE_ONLY_DIRTY (1 << 15)

/* Field entry enabled in hw. */
#define _FP_ENTRY_ENABLED                    (1 << 16)


/* Entry slice identification flags. */
#define _FP_ENTRY_SLICE_FLAGS (_FP_ENTRY_PRIMARY |  _FP_ENTRY_SECONDARY | \
                               _FP_ENTRY_TERTIARY)

/* Entry slice identification flags. */
#define _FP_ENTRY_MIRROR_ON  (_FP_ENTRY_MTP_ING0 |  _FP_ENTRY_MTP_ING1 | \
                               _FP_ENTRY_MTP_EGR0 | _FP_ENTRY_MTP_EGR1)

/* There is no action enum for Oam Stat Actions.
 * Oam Stat Actions conflict with NAT actions.
 * Using Flags to check conflict of actions.
 */
#define _FP_ENTRY_OAM_STAT_VALID            (1 << 17)
#define _FP_ENTRY_OAM_STAT_NOT_ALLOWED      (1 << 18)

#define _FP_ENTRY_MIXED_SRC_CLASS_QUALIFIED (1 << 19)

/* Default Group Entry for Exact Match. */
#define _FP_ENTRY_EXACT_MATCH_GROUP_DEFAULT (1 << 20)

/* Field Entry modifies Slice Sw Entry count. */
#define _FP_ENTRY_MODIFY_SLICE_SW_ENTRY_COUNT (1 << 21)

/* Field Entry meter is modified. */
#define _FP_ENTRY_METER_MODIFIED (1 << 22)

/*
 * A flag to indicate whether the created entry's priority is not set,
 * this flag has to be set during the creation of entry
 */
#define _FP_ENTRY_CREATED_WITHOUT_PRIO  (1 << 23)

/* To identify type of port in SrcGPort qualifier for VFP */
#define _FP_ENTRY_QUAL_PORT_TYPE_TRUNK   (1 << 24)

/*
 * Group status flags.
 */

/* Group resides in a single slice. */
#define _FP_GROUP_SPAN_SINGLE_SLICE          (1 << 0)

/* Group resides in a two paired slices. */
#define _FP_GROUP_SPAN_DOUBLE_SLICE          (1 << 1)

/* Group resides in three paired slices. */
#define _FP_GROUP_SPAN_TRIPLE_SLICE          (1 << 2)

/* Group entries are double wide in each slice. */
#define _FP_GROUP_INTRASLICE_DOUBLEWIDE      (1 << 3)

/* Group span flags mask. */
#define _FP_GROUP_SPAN_MASK                  (0xF)

/*
 * Group has slice lookup enabled
 *     This is default, unless it is disabled by call to
 *     bcm_field_group_enable_set with enable=0
 */
#define _FP_GROUP_LOOKUP_ENABLED             (1 << 4)

/* Group for WLAN tunnel terminated packets. */
#define _FP_GROUP_WLAN                       (1 << 5)

/* Group resides on the smaller slice */
#define _FP_GROUP_SELECT_SMALL_SLICE         (1 << 6)

/* Group resides on the larger slice */
#define _FP_GROUP_SELECT_LARGE_SLICE         (1 << 7)

/* Group supports auto expansion */
#define _FP_GROUP_SELECT_AUTO_EXPANSION      (1 << 8)

/* Flags to set auto expansion slice preference */
#define _FP_GROUP_AUTO_EXPAND_SMALL_SLICE    (1 << 9)
#define _FP_GROUP_AUTO_EXPAND_LARGE_SLICE    (1 << 10)

/* Max Group Size Hard Limit */
#define _FP_GROUP_MAX_SIZE_HARD_LIMIT        (1 << 11)

/* Group supports Preselector */
#define _FP_GROUP_PRESELECTOR_SUPPORT        (1 << 12)

#define _FP_GROUP_STATUS_MASK        (_FP_GROUP_SPAN_SINGLE_SLICE | \
                                      _FP_GROUP_SPAN_DOUBLE_SLICE | \
                                      _FP_GROUP_SPAN_TRIPLE_SLICE | \
                                      _FP_GROUP_INTRASLICE_DOUBLEWIDE | \
                                      _FP_GROUP_WLAN)

/* Group is Port base */
#define _FP_GROUP_PER_PORT_OR_PBMP            (1 << 13)

/* Group creation to be verified with given configs */
#define _FP_GROUP_CONFIG_VALIDATE             (1 << 14)

/* Group creation for FLOWTRACKER Module */
#define _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC   (1 << 15)

/* Group QSET is update */
#define _FP_GROUP_QSET_UPDATE                   (1 << 16)

/* Group entries are double-wide and continuous in same slice */
#define _FP_GROUP_DW_DEPTH_EXPANDED             (1 << 17)

/* Use another group's LT ID */
#define _FP_GROUP_IDENTICAL_LT_ID               (1 << 18)


/* Group to be configured in secondary slice */
#define _FP_GROUP_SELECT_SECONDARY_SLICE        (1 << 19)

/* Group will use AACL compression */
#define _FP_GROUP_COMPRESSED                    (1 << 20)

/* Mask for Group Mode flags */
#define _FP_GROUP_MODE_MASK          (_FP_GROUP_SPAN_SINGLE_SLICE | \
                                      _FP_GROUP_SPAN_DOUBLE_SLICE | \
                                      _FP_GROUP_SPAN_TRIPLE_SLICE | \
                                      _FP_GROUP_INTRASLICE_DOUBLEWIDE | \
                                      _FP_GROUP_DW_DEPTH_EXPANDED)

/* Flowtracker Group key type shift */
#define _FP_GROUP_FT_KEYTYPE_SHIFT              1

/* Flowtracker group key type mask */
#define _FP_GROUP_FT_KEYTYPE_MASK               0x7F

/* Internal DATA qualifiers. */
typedef enum _bcm_field_internal_qualify_e {
    _bcmFieldQualifyData0 = bcmFieldQualifyCount,/* [0x00] Data qualifier 0.  */
    _bcmFieldQualifyData1,                    /* [0x01] Data qualifier 1.     */
    _bcmFieldQualifyData2,                    /* [0x02] Data qualifier 2.     */
    _bcmFieldQualifyData3,                    /* [0x03] Data qualifier 3.     */
    _bcmFieldQualifySvpValid,                 /* [0x04] SVP valid             */
    _bcmFieldQualifyPreLogicalTableId,        /* [0x05] Preselector LT ID.    */
    _bcmFieldQualifyData4,                    /* [0x06] Data qualifier 4.     */
    _bcmFieldQualifyData5,                    /* [0x07] Data qualifier 5.     */
    _bcmFieldQualifyData6,                    /* [0x08] Data qualifier 6.     */
    _bcmFieldQualifyData7,                    /* [0x09] Data qualifier 7.     */
    _bcmFieldQualifyData8,                    /* [0x0a] Data qualifier 8.     */
    _bcmFieldQualifyData9,                    /* [0x0b] Data qualifier 9.     */
    _bcmFieldQualifyRangeCheckBits24_31,      /* [0x0c] Upper 8 bits of the
                                               *        Range Check qualifier.*/
    _bcmFieldQualifyExactMatchHitStatusLookup0,
                                              /* [0x0d] Exact Match Lookup Zero
                                               *        Hit Status.           */
    _bcmFieldQualifyExactMatchHitStatusLookup1,
                                              /* [0x0e] Exact Match Lookup One
                                               *        Hit Status.           */
    _bcmFieldQualifyExactMatchGroupClassIdLookup0,
                                              /* [0x0f] Exact Match Lookup Zero
                                               *        Group Class Id (Logical
                                               *        Table Id), applicable
                                               *        per group.            */
    _bcmFieldQualifyExactMatchGroupClassIdLookup1,
                                              /* [0x10] Exact Match Lookup One
                                               *        Group Class Id (Logical
                                               *        Table Id), applicable
                                               *        per group.            */
    _bcmFieldQualifyExactMatchActionClassIdLookup0,
                                              /* [0x11] Exact Match Lookup Zero
                                               *        Action Class Id,
                                               *        applicable per exact
                                               *        match entry.          */
    _bcmFieldQualifyExactMatchActionClassIdLookup1,
                                              /* [0x12] Exact Match Lookup One
                                               *        Action Class Id,
                                               *        applicable per exact
                                               *        entry.                */
    _bcmFieldQualifyData10,                   /* [0x13] Data qualifier 10.    */
    _bcmFieldQualifyData11,                   /* [0x14] Data qualifier 11.    */
    _bcmFieldQualifyData12,                   /* [0x15] Data qualifier 12.    */
    _bcmFieldQualifyData13,                   /* [0x16] Data qualifier 13.    */
    _bcmFieldQualifyData14,                   /* [0x17] Data qualifier 14.    */
    _bcmFieldQualifyData15,                   /* [0x18] Data qualifier 15.    */
    _bcmFieldQualifyInPorts_1,                /* Refers to logical ports 34 to 49. */
    _bcmFieldQualifyInPorts_2,                /* Refers to logical ports 50 to 65. */
    _bcmFieldQualifyPktFlowTypeTunnelNone,    /* Pkt Flow Type Non-Tunnel */
    _bcmFieldQualifyPktFlowTypeTunnelL2,      /* Pkt Flow Type L2Tunnel */
    _bcmFieldQualifyPktFlowTypeTunnelL3,      /* Pkt Flow Type L3Tunnel */
    _bcmFieldQualifyCpuTxCtrl,                /* Cpu Tx Control Info */
    _bcmFieldQualifyPreLogicalTableId1,       /* Preselector LT ID for part 1. */
    _bcmFieldQualifyPreLogicalTableId2,       /* Preselector LT ID for part 2. */
    _bcmFieldQualifySrcIpv4CompressedClassId, /* Ipv4 Src Ip */
    _bcmFieldQualifySrcIpv6CompressedClassId, /* Ipv6 Src Ip. */
    _bcmFieldQualifyDstIpv4CompressedClassId, /* Ipv4 Dst Ip */
    _bcmFieldQualifyDstIpv6CompressedClassId, /* Ipv6 Dst Ip */
    _bcmFieldQualifyCount                     /* Always last not used. */
} _bcm_field_internal_qualify_t;


#define _BCM_FIELD_QUALIFY_STRINGS \
{ \
    "_Data0", \
    "_Data1", \
    "_Data2", \
    "_Data3", \
    "_SvpValid", \
    "_PreLogicalTableId", \
    "_Data4", \
    "_Data5", \
    "_Data6", \
    "_Data7", \
    "_Data8", \
    "_Data9", \
    "_RangeCheckBits24_31", \
    "_ExactMatchHitStatusLookup0", \
    "_ExactMatchHitStatusLookup1", \
    "_ExactMatchGroupClassIdLookup0", \
    "_ExactMatchGroupClassIdLookup1", \
    "_ExactMatchActionClassIdLookup0", \
    "_ExactMatchActionClassIdLookup1", \
    "_Data10", \
    "_Data11", \
    "_Data12", \
    "_Data13", \
    "_Data14", \
    "_Data15", \
    "_InPorts_1", \
    "_InPorts_2", \
    "_PktFlowTypeTunnelNone", \
    "_PktFlowTypeTunnelL2", \
    "_PktFlowTypeTunnelL3", \
    "_CpuTxCtrl", \
    "_PreLogicalTableId1", \
    "_PreLogicalTableId2", \
}

/* Committed portion in sw doesn't match hw. */
#define _FP_POLICER_COMMITTED_DIRTY     (0x80000000)

/* Peak portion in sw doesn't match hw. */
#define _FP_POLICER_PEAK_DIRTY          (0x40000000)

/* Policer created through meter APIs.  */
#define _FP_POLICER_INTERNAL            (0x20000000)

/* Policer using excess meter. */
#define _FP_POLICER_EXCESS_METER        (0x10000000)

#define _FP_POLICER_DIRTY             (_FP_POLICER_COMMITTED_DIRTY | \
                                       _FP_POLICER_PEAK_DIRTY)

#define _FP_POLICER_LEVEL_COUNT       (2)

/* Flow mode policer using committed meter in hardware. */
#define _FP_POLICER_COMMITTED_HW_METER(f_pl)                \
        (bcmPolicerModeCommitted == (f_pl)->cfg.mode        \
         && !((f_pl)->hw_flags & _FP_POLICER_EXCESS_METER))

/* Flow mode policer using excess meter in hardware. */
#define _FP_POLICER_EXCESS_HW_METER(f_pl)                   \
        (bcmPolicerModeCommitted == (f_pl)->cfg.mode        \
         && ((f_pl)->hw_flags & _FP_POLICER_EXCESS_METER))

/* Set excess meter bit. */
#define _FP_POLICER_EXCESS_HW_METER_SET(f_pl)               \
        ((f_pl)->hw_flags |= _FP_POLICER_EXCESS_METER)

/* Clear excess meter bit. */
#define _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl)             \
        ((f_pl)->hw_flags &= ~_FP_POLICER_EXCESS_METER)

/* Check for Flow mode policer. */
#define _FP_POLICER_IS_FLOW_MODE(f_pl)                      \
        (bcmPolicerModeCommitted == (f_pl)->cfg.mode)

/*
 * Typedef:
 *     _field_policer_t
 * Purpose:
 *     This is the policer description structure.
 *     Indexed by bcm_policer_t handle.
 */
typedef struct _field_policer_s {
    bcm_policer_t        pid;         /* Unique policer identifier.       */
    bcm_policer_config_t cfg;         /* API level policer configuration. */
    uint16               sw_ref_count;/* SW object use reference count.   */
    uint16               hw_ref_count;/* HW object use reference count.   */
    uint8                level;       /* Policer attachment level.        */
    int8                 pool_index;
                             /* Physical Meter pool/slice policer resides.*/
    /* the variable with old_ prefix are added to store the policer info
     * prior to updating with bcm_field_policer_set api. The stored is used
     * to retieve in case meter allocation failns during entry install
     */
    uint8                old_level;
    int8                 old_pool_index;
    bcm_policer_mode_t   old_mode;    /* Policer mode. */
    int                  old_hw_index;
    uint8                old_sw_ref_count;
    uint16               old_hw_ref_count;
    bcm_policer_config_t old_cfg;
    int32               old_hw_flags;
#ifdef    BCM_TRIUMPH3_SUPPORT
    int8                 logical_pool_index;
                              /* Logical Meter pool/slice policer resides.*/
    int8                 old_logical_pool_index;
#endif
    int                  hw_index;    /* HW index policer resides.        */
    uint32               hw_flags;    /* HW installation status flags.    */
    _field_stage_id_t    stage_id;    /* Attached entry stage id.         */
    struct _field_policer_s *next;    /* Policer lookup linked list.      */
}_field_policer_t;


#define _FP_POLICER_VALID                (1 << 0)
#define _FP_POLICER_INSTALLED            (1 << 1)
#define _FP_POLICER_DETACH               (1 << 2)

/*
 * Typedef:
 *     _field_entry_policer_t
 * Purpose:
 *     This is field entry policers description structure.
 *     Used to form an array for currently attached policers.
 */
typedef struct _field_entry_policer_s {
    bcm_policer_t  pid;         /* Unique policer identifier. */
    uint8          flags;       /* Policer/entry flags.       */
}_field_entry_policer_t;

/* _bcm_field_stat_e  - Internal counter types. */
typedef enum _bcm_field_stat_e {
    _bcmFieldStatCount = bcmFieldStatAcceptedBytes /* Internal STAT count
                                                      for XGS devices. Not
                                                      a usable value. */
} _bcm_field_stat_t;

#define _BCM_FIELD_STAT \
{ \
    "BytesEven", \
    "BytesOdd", \
    "PacketsEven", \
    "PacketsOdd" \
}

#define _FP_STAT_HW_MODE_MAX  (0xf)
#define _FP_TRIDENT_STAT_HW_MODE_MAX  (0x3F)

/* Statistics entity was  created through counter APIs.  */
#define _FP_STAT_INTERNAL          (1 << 0)
/* UpdateCounter action was used with NO_YES/YES_NO. */
#define _FP_STAT_COUNTER_PAIR      (1 << 1)
/* Arithmetic operations. */
#define _FP_STAT_ADD               (1 << 2)
#define _FP_STAT_SUBSTRACT         (1 << 3)
/* Packet/bytes selector. */
#define _FP_STAT_BYTES             (1 << 4)
/* Sw entry doesn't match hw. */
#define _FP_STAT_DIRTY             (1 << 5)
/* Stat Create with ID */
#define _FP_STAT_CREATE_ID         (1 << 6)
/* Stat uses Flex Stat resources. */
#define _FP_STAT_FLEX_CNTR         (1 << 7)
/* Stat Create with Internal Advanced Flex Counter feature  */
#define _FP_STAT_INTERNAL_FLEX_COUNTER (1 << 8)
/* Stat Created by flex stat module using bcm_stat_group_create API. */
#define _FP_STAT_CREATED_BY_FLEX_MODULE (1 << 9)
/* Stat uses VLAN Flow Count Stat resources. */
#define _FP_STAT_FLOWCNT_CNTR         (1 << 10)




/* Action conflict check macro. */
#define _FP_ACTIONS_CONFLICT(_val_)    \
    if (action == _val_) {             \
        return (BCM_E_CONFIG);         \
    }


/*
 * Typedef:
 *     _field_stat_t
 * Purpose:
 *     This is the statistics collection entity description structure.
 *     Indexed by int sid (statistics id) handle.
 */
typedef struct _field_stat_s {
    uint32               sid;           /* Unique stat entity identifier.  */

    /* Reference counters  information.*/
    uint16               sw_ref_count;  /* SW object use reference count.   */
    uint16               hw_ref_count;  /* HW object use reference count.   */
    /* Allocated hw resource information.*/
    int8                 offset_mode;   /* Mode to compute final counter idx*/
    int8                 pool_index;    /* Counter pool/slice stat resides. */
    int                  hw_index;      /* HW index stat resides.           */
    /* Reinstall flags. */
    uint32               hw_flags;      /* HW installation status flags.    */
    /* Application requested statistics. */
    uint8                nstat;         /* User requested stats array size. */
    bcm_field_stat_t     *stat_arr;     /* User requested stats array.      */
    /* HW supported statistics. */
    uint32               hw_stat;       /* Statistics supported by HW.      */
    uint16               hw_mode;       /* Hw configuration mode.           */
    uint8                hw_entry_count;/* Number of counters needed.       */
    bcm_field_group_t    gid;           /* Group statistics entity attached.*/
    _field_stage_id_t    stage_id;      /* Attached entry stage id.         */
    struct _field_stat_s *next;         /* Stat lookup linked list.         */
    /* Values after last detach. */
    uint64               *stat_values;  /* Stat value after it was detached */
                                        /* from a last entry.               */
    uint32              flex_mode;      /* Flex stat entity identifier.     */
} _field_stat_t;

typedef enum _field_stat_color_s {
    _bcmFieldStatColorNoColor = 0,
    _bcmFieldStatColorGreen,
    _bcmFieldStatColorYellow,
    _bcmFieldStatColorRed,
    _bcmFieldStatColorNotGreen,
    _bcmFieldStatColorNotYellow,
    _bcmFieldStatColorNotRed,
    _bcmFieldStatColorGreenYellow,
    _bcmFieldStatColorGreenRed,
    _bcmFieldStatColorYellowRed,
    _bcmFieldStatColorGreenNotGreen,
    _bcmFieldStatColorYellowNotYellow,
    _bcmFieldStatColorRedNotRed,
    _bcmFieldStatColorGreenYellowRed,
    _bcmFieldStatColorCount
} _field_stat_color_t;

/*
 * Typedef:
 *     _field_entry_stat_t
 * Purpose:
 *     This is field entry statistics collector descriptor structure.
 */
typedef struct _field_entry_stat_s {
    int            sid;          /* Unique statistics entity id. */
    int            extended_sid; /* Statistics entity id for Flex counter
                                  * in IFP */
    uint16         flags;        /* Statistics entity/entry flags.*/
    bcm_field_stat_action_t
                    stat_action; /* Stat Action. */
}_field_entry_stat_t;
/* Statistics entity attached to fp entry flag. */
#define _FP_ENTRY_STAT_VALID                (1 << 0)
/* Statistics entity installed in HW. */
#define _FP_ENTRY_STAT_INSTALLED            (1 << 1)
/* Statistics entity doesn't have any counters attached. */
#define _FP_ENTRY_STAT_EMPTY                (1 << 2)
/* Statistics entity use even counter. */
#define _FP_ENTRY_STAT_USE_EVEN             (1 << 3)
/* Statistics entity use odd counter. */
#define _FP_ENTRY_STAT_USE_ODD              (1 << 4)
/* In Trident2 EFP , Statistics action conflicts with
 * Vxlan Actions. Below flag will be used in case of
 * conflicts */
#define _FP_ENTRY_STAT_NOT_ALLOWED          (1 << 5)
#define _FP_ENTRY_EXTENDED_STAT_VALID       (1 << 6)
#define _FP_ENTRY_EXTENDED_STAT_NOT_ALLOWED (1 << 7)
#define _FP_ENTRY_EXTENDED_STAT_INSTALLED   (1 << 8)
#define _FP_ENTRY_EXTENDED_STAT_RESERVED    (1 << 9)
/* Statistics entity Dirty flag to fp entry flag. */
#define _FP_ENTRY_STAT_DIRTY                (1 << 10)


typedef struct _field_oam_stat_action_s {
    bcm_field_oam_stat_action_t *oam_stat_action; /* Oam Stat Action */
    uint8  flex_pool_id;                          /* Flex Stat Pool Id */
    int8   pool_instance;                         /* Flex Pool Instance in
                                                     FP_POLICY_TABLE */
    uint16 flags;                                 /* Action Flags */
    struct _field_oam_stat_action_s *next;        /* Next Pointer */
} _field_oam_stat_action_t;

typedef enum _field_oam_stat_pool_config_e {
    _bcmFieldOamStatPoolInstanceFree = 0,
    _bcmFieldOamStatPoolInstanceReserve = 1,
    _bcmFieldOamStatPoolInstanceGet = 2,
    _bcmFieldOamStatPoolInstanceFreeAll = 3,
    _bcmFieldOamStatPoolInstanceCount = 4
} _field_oam_stat_pool_config_t;

#define _FP_EGRESS_OAM_CLASS_ID  0x1
#define _FP_EGRESS_SAT_CLASS_ID  0x2

#define _FP_ETHOAM_ETHER_TYPE    0x8902
#define _FP_SAT_ETHER_TYPE       0x88b7

/*
 * Typedef:
 *     _field_entry_t
 * Purpose:
 *     This is the physical entry structure, hidden behind the logical
 *     bcm_field_entry_t handle.
 * Notes:
 *     Entries are stored in linked list in the under a slice's _field_slice_t
 *     structure.
 *
 *     Each entry can use 0 or 1 counters. Multiple entries may use the
 *     same counter. The only requirement is that the counter be within
 *     the same slice as the entry.
 *
 *     Similarly each entry can use 0 or 1 meter. Multiple entries may use
 *     the same meter. The only requirement is that the meter be within
 *     the same slice as the entry.
 */
struct _field_entry_s {
    bcm_field_entry_t      eid;        /* BCM unit unique entry identifier   */
    int                    prio;       /* Entry priority                     */
    uint32                 slice_idx;  /* Field entry tcam index.            */
    uint32                 ext_act_profile_idx; /* External field entry action profile index. */
    uint32                 flags;      /* _FP_ENTRY_xxx flags                */
    int8                   efp_key_match_type; /* Holds info(0/1) of which
                                                  tcam (tcam/key_match_tcam) has
                                                  to be written into EFP_TCAM
                                                  during entry install */
    _field_tcam_t          tcam;       /* Fields to be written into FP_TCAM  */
    _field_tcam_t          extra_tcam;
    _field_tcam_t          key_match_tcam; /* Fields to be written
                                              into FP_TCAM */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
    _field_pbmp_t          pbmp;       /* Port bitmap                        */
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
    _field_action_t        *actions;   /* linked list of actions for entry   */
    _field_slice_t         *fs;        /* Slice where entry lives            */
    _field_group_t         *group;     /* Group where entry lives            */
    _field_entry_stat_t    statistic;  /* Statistics collection entity.      */
                                       /* Policers attached to the entry.    */
    _field_oam_stat_action_t *field_oam_stat_action; /* Oam Stat Actions     */
    uint8                   oam_pool_instance; /* Oam Stat Pool Instance */
    _field_entry_policer_t policer[_FP_POLICER_LEVEL_COUNT];
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    _field_entry_policer_t global_meter_policer;
#endif
    uint8                  ing_mtp_slot_map;  /* Ingress Mirror
                                                 Slot Container Allocated */
    uint8                  egr_mtp_slot_map;  /* Egress  Mirror
                                                 Slot Container Allocated */
    struct _field_entry_s  *ent_copy;
    uint8                  dvp_type;          /* Type of DVP used in qualification  */
    uint8                  svp_type;          /* Type of SVP used in qualification  */
#if defined(BCM_APACHE_SUPPORT)
    uint8                  eth_oam_ctrl_mep_type;  /* MEP type for
                                                      ETH OAM Control Drop Qualifiers */
    uint8                  eth_oam_data_mep_type;  /* MEP type for
                                                      ETH OAM Data Drop Qualifiers */
    uint8                  mpls_oam_ctrl_mep_type; /* MEP type for
                                                      Mpls OAM Control Drop Qualifiers */
    uint8                  mpls_oam_data_mep_type; /* MEP type for
                                                      Mpls OAM Data Drop Qualifiers */
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    bcm_vlan_action_set_t  *vlan_action_set;  /* Reference to VLAN Action set. */
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    /* Compression Key basic info.
     * For WB this is synced under FIELD_COMPRESSION module handle
     */
    _bcm_field_basic_info_key_t key_info[_BCM_FIELD_COMPRESS_TYPE_MAX];

    /* Compression Key basic info */
    _bcm_field_basic_info_key_t new_key_info[_BCM_FIELD_COMPRESS_TYPE_MAX];
#endif
    struct _field_entry_s  *next;      /* Entry lookup linked list.          */
};

typedef struct _field_control_s _field_control_t;

typedef struct _field_entry_recover_egr_ports_s {
    bcm_field_entry_t eid;
    struct _field_entry_recover_egr_ports_s *next;
}_field_entry_recover_egr_ports_t;

typedef struct _field_egr_ports_entry_ids_list_s {
    bcm_field_entry_t eid;
    bcm_pbmp_t egr_ports_pbmp;
    struct _field_egr_ports_entry_ids_list_s *next;
}_field_egr_ports_entry_ids_list_t;

typedef struct _field_egr_ports_recovery_s {
   _field_entry_recover_egr_ports_t **entry_recover_egr_ports;
}_field_egr_ports_recovery_t;


/* Sw WorkAround for EgressPortsAdd Action */
extern _field_egr_ports_recovery_t
                   *_field_egr_ports_recovery[BCM_MAX_NUM_UNITS];

/* Warmboot recovery for bcmFieldActionDscpMapNew */
typedef struct _field_dscp_map_new_eid_list_s {
    bcm_field_entry_t eid;
    struct _field_dscp_map_new_eid_list_s *next;
}_field_dscp_map_new_eid_list_t;

/*
 * Typedef:
 *     _field_funct_t
 * Purpose:
 *     Function pointers to device specific Field functions
 */
typedef struct _field_funct_s {
    int(*fp_detach)(int, _field_control_t *fc);  /* destructor function */
    int(*fp_data_qualifier_ethertype_add)(int, int,
                                          bcm_field_data_ethertype_t *);
    int(*fp_data_qualifier_ethertype_delete)(int, int,
                                          bcm_field_data_ethertype_t *);
    int(*fp_data_qualifier_ip_protocol_add)(int, int,
                                          bcm_field_data_ip_protocol_t *);
    int(*fp_data_qualifier_ip_protocol_delete)(int, int,
                                          bcm_field_data_ip_protocol_t *);
    int(*fp_data_qualifier_packet_format_add)(int, int,
                                          bcm_field_data_packet_format_t *);
    int(*fp_data_qualifier_packet_format_delete)(int, int,
                                          bcm_field_data_packet_format_t *);
    int(*fp_group_install)(int, _field_group_t *fg);
    int(*fp_selcodes_install)(int unit, _field_group_t *fg,
                              uint8 slice_numb, bcm_pbmp_t pbmp,
                              int selcode_index);
    int(*fp_slice_clear)(int unit, _field_group_t *fg, _field_slice_t *fs);
    int(*fp_entry_remove)(int unit, _field_entry_t *f_ent, int tcam_idx);
    int(*fp_entry_move) (int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new);
    int(*fp_selcode_get)(int unit, _field_stage_t*, bcm_field_qset_t*,
                         _field_group_t*);
    int(*fp_selcode_to_qset)(int unit, _field_stage_t *stage_fc,
                             _field_group_t *fg,
                             int code_id,
                             bcm_field_qset_t *qset);
    int(*fp_qual_list_get)(int unit, _field_stage_t *, _field_group_t*);
    int(*fp_tcam_policy_clear)(int unit, _field_entry_t *f_ent,
                               _field_stage_id_t stage_id, int idx);
    int(*fp_tcam_policy_install)(int unit, _field_entry_t *f_ent, int idx);
    int(*fp_tcam_policy_reinstall)(int unit, _field_entry_t *f_ent, int idx);
    int(*fp_policer_install)(int unit, _field_entry_t *f_ent,
                             _field_policer_t *f_pl);
    int(*fp_write_slice_map)(int unit, _field_stage_t *stage_fc,
                             _field_group_t *fg);
    int(*fp_qualify_ip_type)(int unit, bcm_field_entry_t entry,
                             bcm_field_IpType_t type, bcm_field_qualify_t qual);
    int(*fp_qualify_ip_type_get)(int unit, bcm_field_entry_t entry,
                                 bcm_field_IpType_t *type, bcm_field_qualify_t qual);
    int(*fp_action_support_check)(int unit, _field_entry_t *f_ent,
                                  bcm_field_action_t action, int *result);
    int(*fp_action_conflict_check)(int unit, _field_entry_t *f_ent,
                                   bcm_field_action_t action,
                                   bcm_field_action_t action1);
    int(*fp_action_params_check)(int unit, _field_entry_t *f_ent,
                                 _field_action_t *fa);
    int(*fp_action_depends_check)(int unit, _field_entry_t *f_ent,
                                  _field_action_t *fa);
    int (*fp_egress_key_match_type_set)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_install)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_reinstall)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_remove)(int unit, _field_entry_t *f_ent);
    int (*fp_external_entry_prio_set)(int unit, _field_entry_t *f_ent,
                                      int prio);
    int (*fp_counter_get)(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count,
                          uint64 *byte_count);
    int (*fp_counter_set)(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count,
                          uint64 *byte_count);
    int (*fp_stat_index_get)(int unit, _field_stat_t *f_st,
                             bcm_field_stat_t stat, int *idx1,
                             int *idx2, int *idx3, uint32 *out_flags);
    int (*fp_control_set)(int unit,  _field_control_t *fc,
                          bcm_field_control_t control, uint32 state);
    int (*fp_control_get)(int unit,  _field_control_t *fc,
                          bcm_field_control_t control, uint32 *state);
    int (*fp_stat_value_get)(int unit, int sync_mode, _field_stat_t *f_st,
                              bcm_field_stat_t stat, uint64 *value);
    int (*fp_stat_value_set)(int unit, _field_stat_t *f_st,
                              bcm_field_stat_t stat, uint64 value);
    int (*fp_stat_hw_mode_get)(int unit, _field_stat_t *f_st,
                                         _field_stage_id_t stage_id);
    int (*fp_stat_hw_alloc)(int unit, _field_entry_t *f_ent);
    int (*fp_stat_hw_free)(int unit, _field_entry_t *f_ent);
    int (*fp_group_add)(int unit, uint32 flags, bcm_field_group_config_t *grp_conf);
    int (*fp_entry_enable)(int unit, _field_entry_t *f_ent, int enable_flag);
    int (*fp_qualify_svp)(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual, uint32 data,
                          uint32 mask, int svp_valid);
    int (*fp_qualify_dvp)(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual, uint32 data,
                          uint32 mask, int dvp_valid);
    int (*fp_qualify_trunk)(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            bcm_trunk_t data, bcm_trunk_t mask);
    int (*fp_qualify_trunk_get)(int unit,
                                bcm_field_entry_t entry,
                                bcm_field_qualify_t qual,
                                bcm_trunk_t *data, bcm_trunk_t *mask);
    int (*fp_qualify_inports)(int unit,
                              bcm_field_entry_t entry,
                              bcm_field_qualify_t qual,
                              bcm_pbmp_t data,
                              bcm_pbmp_t mask);
    int (*fp_entry_stat_extended_attach)(int unit, _field_entry_t *f_ent,
            int stat_id, bcm_field_stat_action_t stat_action);
    int (*fp_entry_stat_extended_get)(int unit, _field_entry_t *f_ent,
            int *stat_id, bcm_field_stat_action_t *stat_action);
    int (*fp_entry_stat_detach)(int unit, _field_entry_t *f_ent, int stat_id);
    int (*fp_class_size_get)(int unit, bcm_field_qualify_t qual,
                                            uint16 *class_size);
    int (*fp_policer_packet_counter_get32)(int unit,
                          _field_policer_t *f_pl, uint32 *count);
    int (*fp_policer_packet_reset_counter_get32)(int unit,
                          _field_policer_t *f_pl, uint32 *count);
    int (*fp_eh_mask_profile_hw_alloc)(int unit, _field_entry_t *f_ent);
    int (*fp_eh_mask_profile_hw_free)(int unit,
                                      _field_entry_t *f_ent, uint32 flags);
    int (*fp_zone_match_id_partition_init)(int unit, _field_stage_t *stage_fc);
    int (*fp_vlan_actions_profile_hw_alloc)(int unit, _field_entry_t *f_ent);
    int (*fp_vlan_actions_profile_hw_free)(int unit,
                                      _field_entry_t *f_ent, uint32 flags);
    int (*fp_qset_id_multi_set)(int unit, bcm_field_qualify_t qualifier, 
                                int num_objects, int objects_list[],
                                bcm_field_qset_t *qset);
    int (*fp_qset_id_multi_get)(int unit, bcm_field_qset_t qset,
                                bcm_field_qualify_t qualifier, int max,
                                int *objects_list, int *actual);
    int (*fp_qset_id_multi_delete)(int unit, bcm_field_qualify_t qualifier,
                                   int num_objects, int objects_list[],
                                   bcm_field_qset_t *qset);
    int (*fp_qualify_udf)(int unit, bcm_field_entry_t eid,
                          bcm_udf_id_t udf_id, int length,
                          uint8 *data, uint8 *mask);
    int (*fp_qualify_udf_get)(int unit, bcm_field_entry_t eid,
                              bcm_udf_id_t udf_id, int max_length,
                              uint8 *data, uint8 *mask, int *actual_length);
    int (*fp_qualify_packet_res)(int unit, bcm_field_entry_t entry,
                                 uint32 *data, uint32 *mask);
    int (*fp_qualify_packet_res_get)(int unit, bcm_field_entry_t entry,
                                 uint32 *data, uint32 *mask);
    int (*fp_ddrop_action_alloc)(int unit, _field_entry_t *f_ent,
                                        _field_action_t *fa);
    int (*fp_dredirect_action_alloc)(int unit, _field_entry_t *f_ent,
                                        _field_action_t *fa);
} _field_funct_t;

/* If given index in slice is free to be used. */
#define _BCM_FIELD_SLICE_IDX_TYPE_FREE       1
/* If given index in slice has different group mode. */
#define _BCM_FIELD_SLICE_IDX_TYPE_DIFF       2
/* If given index in slice has same group mode. */
#define _BCM_FIELD_SLICE_IDX_TYPE_SAME       3

/*
 * Typedef:
 *  _bcm_field_entry_shift_params_t
 * Purpose:
 *  For Entry movement due to change in priority requires to search for
 *  free indices in the slice belonging to the field group. These free
 *  indices in slices decides how entry is moved within the group slices
 *  to acheive desired configuration.
 */
typedef struct _bcm_field_entry_shift_params_s {
    /* Target field slice where this entry need to be moved. */
    _field_slice_t *target_fs;
    /* Slice index in target slice. */
    int slice_idx_target;
    /* Slice where free index is found just after deciding target_fs/index. */
    _field_slice_t *next_null_fs;
    /* Corresopnding index in next_null_fs. */
    int next_null_index;
    /* slice where free index is found just before reaching target_fs/index. */
    _field_slice_t *prev_null_fs;
    /* corresponding index in prev_null_fs. */
    int prev_null_index;
    /* operator if decrement in target_idx is required. */
    int decr_on_shift_up;
    /* Flag which represent free entry is not present in field group. */
    int flag_no_free_entries;
    /* operator if only move is required for desired configuration. */
    int only_move;

    /* Remove it later */
    _field_slice_t *first_null_fs;
    int first_null_index;
    int first_null_flag;

} _bcm_field_entry_shift_params_t;

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TOMAHAWK_SUPPORT

typedef struct _field_group_entry_arr_pos_s _field_group_entry_arr_pos_t;

struct _field_dynamic_map_entry_s {
    int entry_id;
    _field_entry_t *entry;
    struct _field_dynamic_map_entry_s *next;
};
typedef struct _field_dynamic_map_entry_s _field_dynamic_map_entry_t;

struct _field_dynamic_map_lt_entry_s {
    int lt_entry_id;
    _field_lt_entry_t *lt_entry;
    struct _field_dynamic_map_lt_entry_s *next;
};
typedef struct _field_dynamic_map_lt_entry_s _field_dynamic_map_lt_entry_t;


struct _field_dynamic_map_s {
    _field_dynamic_map_entry_t *entry_map;
    _field_dynamic_map_lt_entry_t *lt_entry_map;
};

typedef struct _field_dynamic_map_s _field_dynamic_map_t;


/*
 * Typedef:
 *     _field_stat_bmp_t
 * Purpose:
 *     Stat bit map for tracking configured stat types for each statid,
 *     used only in WB.
 */
typedef struct _field_stat_bmp_s {
    SHR_BITDCL  *w;
} _field_stat_bmp_t;

#define _FP_STAT_BMP_FREE(bmp)   sal_free((bmp).w)
#define _FP_STAT_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_STAT_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_STAT_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))


/* Flags */
#define _FP_WB_TLV_LEN_PRESENT      0x00000001
#define _FP_WB_TLV_LEN_ENCODED      0x00000002
#define _FP_WB_TLV_NO_VALUE         0x00000004
#define _FP_WB_TLV_NO_TYPE          0x00000008
#define _FP_WB_TLV_NO_INGRESS       0x00010000
#define _FP_WB_TLV_CLASS            0x00020000
#define _FP_WB_TLV_NO_EM            0x00040000
#define _FP_WB_TLV_NO_FT            0x00080000

/* TLV Processing */
#define _FP_WB_TLV_TYPES_MASK	        0xffc00000
#define _FP_WB_TLV_TYPE_SHIFT		    22
#define _FP_WB_TLV_LENGTH_MASK	        0x003fffff
#define _FP_WB_TLV_BASIC_TYPE_SHIFT 	28
#define _FP_WB_TLV_BASIC_TYPE_MASK	    0xf0000000
#define _FP_WB_TLV_ELEM_TYPE_MASK	    0x0fffffff

/* end markers */
#define _FIELD_WB_EM_CONTROL           0xceab1122
#define _FIELD_WB_EM_STAT              0xceab3344
#define _FIELD_WB_EM_POLICER           0xceab5566
#define _FIELD_WB_EM_HINT              0xceab7788
#define _FIELD_WB_EM_STAGE             0xceab9900
#define _FIELD_WB_EM_RANGE             0xcead1122
#define _FIELD_WB_EM_METER             0xcead3344
#define _FIELD_WB_EM_CNTR              0xcead5566
#define _FIELD_WB_EM_LTCONF            0xcead7788
#define _FIELD_WB_EM_DATACONTROL       0xcead9900
#define _FIELD_WB_EM_DATAQUAL          0xceab1234
#define _FIELD_WB_EM_DATAETYPE         0xceab5678
#define _FIELD_WB_EM_DATAPROT          0xceab4321
#define _FIELD_WB_EM_DATATCAM          0xceab8765
#define _FIELD_WB_EM_QUAL              0xcead4321
#define _FIELD_WB_EM_GROUP             0xcead8765
#define _FIELD_WB_EM_QUALOFFSET        0xcead1234
#define _FIELD_WB_EM_UDF               0xcead5678
#define _FIELD_WB_EM_LTENTRY           0xcead9012
#define _FIELD_WB_EM_ENTRY             0xcead3456
#define _FIELD_WB_EM_SLICE             0xcead7890
#define _FIELD_WB_EM_LTSLICE           0xcead0987
#define _FIELD_WB_EM_EXTRACTOR         0xcead6543
#define _FIELD_WB_EM_CLASS             0xcead2109
#define _FIELD_WB_EM_PRESEL            0xcead5432
#define _FIELD_WB_EM_AUXTAG_IBUS       0xcead0001

/*
 *  Contains a type for every element that needs to be stored
 *  in field structures
 */
typedef enum _bcm_field_internal_element_e {
    _bcmFieldInternalType = 0,
    _bcmFieldInternalExtractor = 1,
    _bcmFieldInternalGroup = 2,
    _bcmFieldInternalGroupId = 3,
    _bcmFieldInternalGroupPri = 4,
    _bcmFieldInternalGroupQset = 5,
    _bcmFieldInternalQsetW = 6,
    _bcmFieldInternalQsetUdfMap = 7,
    _bcmFieldInternalGroupFlags = 8,
    _bcmFieldInternalGroupPbmp = 9,
    _bcmFieldInternalGroupSlice = 10,
    _bcmFieldInternalGroupQual = 11,
    _bcmFieldInternalQualQid = 12,
    _bcmFieldInternalQualOffset = 13,
    _bcmFieldInternalQualSize = 14,
    _bcmFieldInternalGroupStage = 15,
    _bcmFieldInternalGroupEntry = 16,
    _bcmFieldInternalGroupBlockCount = 17,
    _bcmFieldInternalGroupGroupStatus = 18,
    _bcmFieldInternalGroupGroupAset = 19,
    _bcmFieldInternalGroupCounterBmp = 20,
    _bcmFieldInternalGroupInstance = 21,
    _bcmFieldInternalGroupExtCodes = 22,
    _bcmFieldInternalGroupLtSlice = 23,
    _bcmFieldInternalGroupLtConfig = 24,
    _bcmFieldInternalGroupLtEntry = 25,
    _bcmFieldInternalGroupLtEntrySize = 26,
    _bcmFieldInternalGroupLtEntryStatus = 27,
    _bcmFieldInternalLtStatusEntriesFree = 28,
    _bcmFieldInternalLtStatusEntriesTotal = 29,
    _bcmFieldInternalLtStatusEntriesCnt = 30,
    _bcmFieldInternalGroupQsetSize = 31,
    _bcmFieldInternalGroupHintId = 32,
    _bcmFieldInternalGroupMaxSize = 33,
    _bcmFieldInternalEndStructGroup = 34,
    _bcmFieldInternalQualOffsetField = 35,
    _bcmFieldInternalQualOffsetNumOffset = 36,
    _bcmFieldInternalQualOffsetOffsetArr = 37,
    _bcmFieldInternalQualOffsetWidth = 38,
    _bcmFieldInternalQualOffsetSec = 39,
    _bcmFieldInternalQualOffsetBitPos = 40,
    _bcmFieldInternalQualOffsetQualWidth = 41,
    _bcmFieldInternalGroupPartCount = 42,
    _bcmFieldInternalEntryEid = 43,
    _bcmFieldInternalEntryPrio = 44,
    _bcmFieldInternalEntrySliceId = 45,
    _bcmFieldInternalEntryFlagsPart1 = 46,
    _bcmFieldInternalEntryFlagsPart2 = 47,
    _bcmFieldInternalEntryFlagsPart3 = 48,
    _bcmFieldInternalEntryPbmp = 49,
    _bcmFieldInternalEntryAction = 50,
    _bcmFieldInternalEntrySlice = 51,
    _bcmFieldInternalEntryGroup = 52,
    _bcmFieldInternalEntryStat = 53,
    _bcmFieldInternalEntryPolicer = 54,
    _bcmFieldInternalEntryIngMtp = 55,
    _bcmFieldInternalEntryEgrMtp = 56,
    _bcmFieldInternalEntryDvp = 57,
    _bcmFieldInternalEntryCopy = 58,
    _bcmFieldInternalEntryCopyTypePointer = 59,
    _bcmFieldInternalPbmpData = 60,
    _bcmFieldInternalPbmpMask = 61,
    _bcmFieldInternalEntryActionsPbmp = 62,
    _bcmFieldInternalActionParam = 63,
    _bcmFieldInternalGroupClassAct = 64,
    _bcmFieldInternalActionHwIdx = 65,
    _bcmFieldInternalActionFlags = 66,
    _bcmFieldInternalStatFlags = 67,
    _bcmFieldInternalPolicerFlags = 68,
    _bcmFieldInternalExtl1e32Sel = 69,
    _bcmFieldInternalExtl1e16Sel = 70,
    _bcmFieldInternalExtl1e8Sel = 71,
    _bcmFieldInternalExtl1e4Sel = 72,
    _bcmFieldInternalExtl1e2Sel = 73,
    _bcmFieldInternalExtl2e16Sel = 74,
    _bcmFieldInternalExtl3e1Sel = 75,
    _bcmFieldInternalExtl3e2Sel = 76,
    _bcmFieldInternalExtl3e4Sel = 77,
    _bcmFieldInternalExtPmuxSel = 78,
    _bcmFieldInternalExtIntraSlice = 79,
    _bcmFieldInternalExtSecondary = 80,
    _bcmFieldInternalExtIpbmpRes = 81,
    _bcmFieldInternalExtNorml3l4 = 82,
    _bcmFieldInternalExtNormmac = 83,
    _bcmFieldInternalExtAuxTagaSel = 84,
    _bcmFieldInternalExtAuxTagbSel = 85,
    _bcmFieldInternalExtAuxTagcSel = 86,
    _bcmFieldInternalExtAuxTagdSel = 87,
    _bcmFieldInternalExtTcpFnSel = 88,
    _bcmFieldInternalExtTosFnSel = 89,
    _bcmFieldInternalExtTtlFnSel = 90,
    _bcmFieldInternalExtClassIdaSel = 91,
    _bcmFieldInternalExtClassIdbSel = 92,
    _bcmFieldInternalExtClassIdcSel = 93,
    _bcmFieldInternalExtClassIddSel = 94,
    _bcmFieldInternalExtSrcContaSel = 95,
    _bcmFieldInternalExtSrcContbSel = 96,
    _bcmFieldInternalExtSrcDestCont0Sel = 97,
    _bcmFieldInternalExtSrcDestCont1Sel = 98,
    _bcmFieldInternalExtKeygenIndex = 99,
    _bcmFieldInternalLtEntrySlice = 100,
    _bcmFieldInternalLtEntryIndex = 101,
    _bcmFieldInternalGroupCount = 102,
    _bcmFieldInternalSliceCount = 103,
    _bcmFieldInternalLtSliceCount = 104,
    _bcmFieldInternalEndStructGroupQual = 105,
    _bcmFieldInternalEndStructQualOffset = 106,
    _bcmFieldInternalEndStructEntryDetails = 107,
    _bcmFieldInternalEndStructEntry = 108,
    _bcmFieldInternalEntryPbmpData = 109,
    _bcmFieldInternalEntryPbmpMask = 110,
    _bcmFieldInternalEndStructAction = 111,
    _bcmFieldInternalGlobalEntryPolicer = 112,
    _bcmFieldInternalGlobalEntryPolicerPid = 113,
    _bcmFieldInternalGlobalEntryPolicerFlags = 114,
    _bcmFieldInternalEndGroupExtractor = 115,
    _bcmFieldInternalControl = 116,
    _bcmFieldInternalControlFlags = 117,
    _bcmFieldInternalStage = 118,
    _bcmFieldInternalControlGroups = 119,
    _bcmFieldInternalControlStages = 120,
    _bcmFieldInternalControlPolicerHash = 121,
    _bcmFieldInternalControlPolicerCount = 122,
    _bcmFieldInternalControlStathash = 123,
    _bcmFieldInternalControlStatCount = 124,
    _bcmFieldInternalControlHintBmp = 125,
    _bcmFieldInternalControlHintHash = 126,
    _bcmFieldInternalControlLastAllocatedLtId = 127,
    _bcmFieldInternalEndStructControl = 128,
    _bcmFieldInternalPolicerPid = 129,
    _bcmFieldInternalPolicerSwRefCount = 130,
    _bcmFieldInternalPolicerHwRefCount = 131,
    _bcmFieldInternalPolicerLevel = 132,
    _bcmFieldInternalPolicerPoolIndex = 133,
    _bcmFieldInternalPolicerHwIndex = 134,
    _bcmFieldInternalPolicerHwFlags = 135,
    _bcmFieldInternalPolicerStageId = 136,
    _bcmFieldInternalEndStructPolicer = 137,
    _bcmFieldInternalStatsId = 138,
    _bcmFieldInternalStatSwRefCount = 139,
    _bcmFieldInternalStatHwRefCount = 140,
    _bcmFieldInternalStatOffsetMode = 141,
    _bcmFieldInternalStatPoolIndex = 142,
    _bcmFieldInternalStatHwIndex = 143,
    _bcmFieldInternalStatHwFlags = 144,
    _bcmFieldInternalEndStructStage = 145,
    _bcmFieldInternalStatnStat = 146,
    _bcmFieldInternalStatArr = 147,
    _bcmFieldInternalStatHwStat = 148,
    _bcmFieldInternalStatHwMode = 149,
    _bcmFieldInternalStatHwEntryCount = 150,
    _bcmFieldInternalStatGid = 151,
    _bcmFieldInternalStatStageId = 152,
    _bcmFieldInternalStatStatValues = 153,
    _bcmFieldInternalStatFlexMode = 154,
    _bcmFieldInternalEndStructStat = 155,
    _bcmFieldInternalHintHintid = 156,
    _bcmFieldInternalHintHints = 157,
    _bcmFieldInternalHintsHinttype = 158,
    _bcmFieldInternalHintsQual = 159,
    _bcmFieldInternalHintsMaxValues = 160,
    _bcmFieldInternalHintsStartbit = 161,
    _bcmFieldInternalHintsEndbit = 162,
    _bcmFieldInternalHintsFlags = 163,
    _bcmFieldInternalHintsMaxGrpSize = 164,
    _bcmFieldInternalEndStructHints = 165,
    _bcmFieldInternalHintGrpRefCount = 166,
    _bcmFieldInternalHintCount = 167,
    _bcmFieldInternalEndStructHint = 168,
    _bcmFieldInternalPolicerCfgFlags = 169,
    _bcmFieldInternalPolicerCfgMode = 170,
    _bcmFieldInternalPolicerCfgCkbitsSec = 171,
    _bcmFieldInternalPolicerCfgMaxCkbitsSec = 172,
    _bcmFieldInternalPolicerCfgCkbitsBurst = 173,
    _bcmFieldInternalPolicerCfgPkbitsSec = 174,
    _bcmFieldInternalPolicerCfgMaxPkbitsSec = 175,
    _bcmFieldInternalPolicerCfgPkbitsBurst  = 176,
    _bcmFieldInternalPolicerCfgKbitsCurrent = 177,
    _bcmFieldInternalPolicerCfgActionId = 178,
    _bcmFieldInternalPolicerCfgSharingMode = 179,
    _bcmFieldInternalPolicerCfgEntropyId = 180,
    _bcmFieldInternalPolicerCfgPoolId = 181,
    _bcmFieldInternalControlEndStructUdf = 182,
    _bcmFieldInternalHintHintsHintType = 183,
    _bcmFieldInternalHintHintsQual = 184,
    _bcmFieldInternalHintHintsMaxValues = 185,
    _bcmFieldInternalHintHintsStartBit = 186,
    _bcmFieldInternalHintHintsEndBit = 187,
    _bcmFieldInternalHintHintsFlags = 188,
    _bcmFieldInternalHintHintsMaxGrpSize = 189,
    _bcmFieldInternalEndStructHintHints = 190,
    _bcmFieldInternalStageStageid = 191,
    _bcmFieldInternalStageFlags = 192,
    _bcmFieldInternalStageTcamSz = 193,
    _bcmFieldInternalStageTcamSlices = 194,
    _bcmFieldInternalStageNumInstances = 195,
    _bcmFieldInternalStageNumPipes = 196,
    _bcmFieldInternalStageRanges = 197,
    _bcmFieldInternalStageRangeId = 198,
    _bcmFieldInternalStageNumMeterPools = 199,
    _bcmFieldInternalStageMeterPool = 200,
    _bcmFieldInternalStageNumCntrPools = 201,
    _bcmFieldInternalStageCntrPools = 202,
    _bcmFieldInternalStageLtTcamSz = 203,
    _bcmFieldInternalStageNumLogicalTables = 204,
    _bcmFieldInternalStageLtInfo = 205,
    _bcmFieldInternalStageExtLevels = 206,
    _bcmFieldInternalStageOperMode = 207,
    _bcmFieldInternalRangeFlags = 208,
    _bcmFieldInternalRangeRid = 209,
    _bcmFieldInternalRangeMin = 210,
    _bcmFieldInternalRangeMax = 211,
    _bcmFieldInternalRangeHwIndex = 212,
    _bcmFieldInternalRangeStyle = 213,
    _bcmFieldInternalMeterLevel = 214,
    _bcmFieldInternalMeterSliceId = 215,
    _bcmFieldInternalMeterSize = 216,
    _bcmFieldInternalMeterPoolSize = 217,
    _bcmFieldInternalMeterFreeMeters = 218,
    _bcmFieldInternalMeterNumMeterPairs = 219,
    _bcmFieldInternalMeterBmp = 220,
    _bcmFieldInternalCntrSliceId = 221,
    _bcmFieldInternalCntrSize = 222,
    _bcmFieldInternalCntrFreeCntrs = 223,
    _bcmFieldInternalCntrBmp = 224,
    _bcmFieldInternalLtConfigValid = 225,
    _bcmFieldInternalLtConfigLtId = 226,
    _bcmFieldInternalLtConfigLtPartPri = 227,
    _bcmFieldInternalLtConfigLtPartMap = 228,
    _bcmFieldInternalLtConfigLtActionPri = 229,
    _bcmFieldInternalLtConfigPri = 230,
    _bcmFieldInternalLtConfigFlags = 231,
    _bcmFieldInternalLtConfigEntry = 232,
    _bcmFieldInternalEndStructRanges = 233,
    _bcmFieldInternalEndStructMeter = 234,
    _bcmFieldInternalEndStructCntr = 235,
    _bcmFieldInternalEndStructLtConfig = 236,
    _bcmFieldInternalSlice = 237,
    _bcmFieldInternalSliceStartTcamIdx = 238,
    _bcmFieldInternalSliceNumber = 239,
    _bcmFieldInternalSliceEntryCount = 240,
    _bcmFieldInternalSliceFreeCount = 241,
    _bcmFieldInternalSliceCountersCount = 242,
    _bcmFieldInternalSliceMetersCount = 243,
    _bcmFieldInternalSliceInstalledEntriesCount = 244,
    _bcmFieldInternalSliceCounterBmp = 245,
    _bcmFieldInternalSliceMeterBmp = 246,
    _bcmFieldInternalSliceStageId = 247,
    _bcmFieldInternalSlicePortPbmp = 248,
    _bcmFieldInternalSliceEntriesInfo = 249,
    _bcmFieldInternalSliceNextSlice = 250,
    _bcmFieldInternalSlicePrevSlice = 251,
    _bcmFieldInternalSliceFlags = 252,
    _bcmFieldInternalSliceGroupFlags = 253,
    _bcmFieldInternalSliceLtMap = 254,
    _bcmFieldInternalEndStructSlice = 255,
    _bcmFieldInternalEndStructExtractor = 256,
    _bcmFieldInternalLtSliceSliceNum = 257,
    _bcmFieldInternalLtSliceStartTcamIdx = 258,
    _bcmFieldInternalLtSliceEntryCount = 259,
    _bcmFieldInternalLtSliceFreeCount = 260,
    _bcmFieldInternalLtSliceStageid = 261,
    _bcmFieldInternalLtSliceEntryinfo = 262,
    _bcmFieldInternalLtSliceNextSlice = 263,
    _bcmFieldInternalLtSlicePrevSlice = 264,
    _bcmFieldInternalLtSliceFlags = 265,
    _bcmFieldInternalLtSliceGroupFlags = 266,
    _bcmFieldInternalDataControlStart = 267,
    _bcmFieldInternalDataControlUsageBmp = 268,
    _bcmFieldInternalDataControlDataQualStruct = 269,
    _bcmFieldInternalDataControlDataQualQid = 270,
    _bcmFieldInternalDataControlDataQualUdfSpec = 271,
    _bcmFieldInternalDataControlDataQualOffsetBase = 272,
    _bcmFieldInternalDataControlDataQualOffset = 273,
    _bcmFieldInternalDataControlDataQualByteOffset = 274,
    _bcmFieldInternalDataControlDataQualHwBmp = 275,
    _bcmFieldInternalDataControlDataQualFlags = 276,
    _bcmFieldInternalDataControlDataQualElemCount = 277,
    _bcmFieldInternalDataControlDataQualLength = 278,
    _bcmFieldInternalDataControlEndStructDataQual = 279,
    _bcmFieldInternalDataControlEthertypeStruct = 280,
    _bcmFieldInternalDataControlEthertypeRefCount = 281,
    _bcmFieldInternalDataControlEthertypeL2 = 282,
    _bcmFieldInternalDataControlEthertypeVlanTag = 283,
    _bcmFieldInternalDataControlEthertypePortEt = 284,
    _bcmFieldInternalDataControlEthertypeRelOffset = 285,
    _bcmFieldInternalDataControlProtStart = 286,
    _bcmFieldInternalDataControlProtIp4RefCount = 287,
    _bcmFieldInternalDataControlProtIp6RefCount = 288,
    _bcmFieldInternalDataControlProtFlags = 289,
    _bcmFieldInternalDataControlProtIp = 290,
    _bcmFieldInternalDataControlProtL2 = 291,
    _bcmFieldInternalDataControlProtVlanTag = 292,
    _bcmFieldInternalDataControlProtRelOffset = 293,
    _bcmFieldInternalDataControlTcamStruct = 294,
    _bcmFieldInternalDataControlTcamRefCount = 295,
    _bcmFieldInternalDataControlTcamPriority = 296,
    _bcmFieldInternalDataControlElemSize = 297,
    _bcmFieldInternalDataControlNumElem = 298,
    _bcmFieldInternalEndStructDataControl = 299,
    _bcmFieldInternalControlUdfValid = 300,
    _bcmFieldInternalControlUdfUseCount = 301,
    _bcmFieldInternalControlUdfNum = 302,
    _bcmFieldInternalControlUdfUserNum = 303,
    _bcmFieldInternalControlUdfDetails = 304,
    _bcmFieldInternalDataControlEndStructEtype = 305,
    _bcmFieldInternalDataControlEndStructProt = 306,
    _bcmFieldInternalDataControlEndStructTcam = 307,
    _bcmFieldInternalEntryStatSid = 308,
    _bcmFieldInternalEntryStatExtendedSid = 309,
    _bcmFieldInternalEntryStatFlags = 310,
    _bcmFieldInternalEntryStatAction = 311,
    _bcmFieldInternalEndStructLtEntryDetails = 312,
    _bcmFieldInternalSliceLtPartitionPri = 313,
    _bcmFieldInternalEntryPolicerPid = 314,
    _bcmFieldInternalEntryPolicerFlags = 315,
    _bcmFieldInternalEndStructEntPolicer = 316,
    _bcmFieldInternalEndStructIFP = 317,
    _bcmFieldInternalStageClass = 318,
    _bcmFieldInternalClassFlags = 319,
    _bcmFieldInternalClassEntUsed = 320,
    _bcmFieldInternalClassBmp = 321,
    _bcmFieldInternalEndStructClass = 322,
    _bcmFieldInternalClassOperMode = 323,
    _bcmFieldInternalEntryTcamIptype = 324,
    _bcmFieldInternalEndStageClass = 325,
    _bcmFieldInternalEntryPbmpFullData = 326,
    _bcmFieldInternalEntryPbmpFullMask = 327,
    _bcmFieldInternalStageLtActionPri = 328,
    _bcmFieldInternalStagePreselQset = 329,
    _bcmFieldInternalControlPreselInfo = 330,
    _bcmFieldInternalGroupPreselQual = 331,
    _bcmFieldInternalPreselId = 332,
    _bcmFieldInternalPreselFlags = 333,
    _bcmFieldInternalPreselPri = 334,
    _bcmFieldInternalPreselHwIndex = 335,
    _bcmFieldInternalPreselSliceIndex = 336,
    _bcmFieldInternalPreselTcamIndex = 337,
    _bcmFieldInternalPreselClassAction = 338,
    _bcmFieldInternalPreselLtDataSize = 339,
    _bcmFieldInternalPreselQset = 340,
    _bcmFieldInternalEndStructPreselInfo = 341,
    _bcmFieldInternalPreselInfoLimit = 342,
    _bcmFieldInternalPreselLastAllocatedId = 343,
    _bcmFieldInternalPreselSet = 344,
    _bcmFieldInternalPreselOperationalSet = 345,
    _bcmFieldInternalPreselKeySize = 346,
    _bcmFieldInternalPreselEntrySlice = 347,
    _bcmFieldInternalPreselEntryGroup = 348,
    _bcmFieldInternalEMOperMode = 349,
    _bcmFieldInternalEndStageEM = 350,
    _bcmFieldInternalEMEntryarr = 351,
    _bcmFieldInternalEMGroupMode = 352,
    _bcmFieldInternalEMGroupAset = 353,
    _bcmFieldInternalPreselPbmp  = 354,
    _bcmFieldInternalQualOffsetQualWidth16 = 355,
    _bcmFieldInternalStatBmp = 356,
    _bcmFieldInternalQsetQual = 357,
    _bcmFieldInternalQsetUdf  = 358,
    _bcmFieldInternalQualifyCount = 359,
    _bcmFieldInternalHintHintsPri = 360,
    /* _bcmFieldInternalEMGroupAset was not storing length.
       Bug fixed through this enum */
    _bcmFieldInternalEMGroupAset2 = 361,
    _bcmFieldInternalEntrySvpType = 362,
    /* bcmFieldActionParam was not storing parameters
     * for copytocpu and timetsamptocpu,
     * Bug fixed through this Enum */
    _bcmFieldInternalActionParam1 = 363,
    _bcmFieldInternalGroupActionProfIdx = 364,
    _bcmFieldInternalGroupFlagsMsb16 = 365,
    _bcmFieldInternalSliceFlagsMsb24 = 366,
    _bcmFieldInternalExtAltTtlFnSel = 367,
    _bcmFieldInternalStageClassInfo = 368,
    _bcmFieldInternalControlMeterInUse = 369,
    _bcmFieldInternalHintHintsDosAttackEventFlags = 370,
    _bcmFieldInternalFTOperMode = 371,
    _bcmFieldInternalFTEntryArr = 372,
    _bcmFieldInternalEndStageFT = 373,
    _bcmFieldInternalStageAuxTagIBusContStatus = 374,
    _bcmFieldInternalAuxTagIbusContVal = 375,
    _bcmFieldInternalAuxTagIbusRefCount = 376,
    _bcmFieldInternalEndStructAuxTagIbus = 377,
    _bcmFieldInternalStageGrpRunningPrio = 378, 
    _bcmFieldInternalEMActionPbmOpaqueObject1 = 379,
    _bcmFieldInternalEMActionPbmOpaqueObject2 = 380,
    _bcmFieldInternalEMActionPbmGbpSrcIdNew = 381,
    _bcmFieldInternalEMActionPbmGbpDstIdNew = 382,
    _bcmFieldInternalEMActionPbmEgressFlowControlEnable = 383,
    _bcmFieldInternalEMActionPbmOpaqueObject3 = 384,
    _bcmFieldInternalEMActionPbmAssignNatClassId = 385,
    _bcmFieldInternalEMActionPbmAssignChangeL2FieldsClassId = 386,
    _bcmFieldInternalEMActionPbmEgressFlowEncapEnable = 387,
    _bcmFieldInternalLtConfigLtRefCount = 388,
    _bcmFieldInternalEMActionPbmEncapIfaMetadataHdr = 389,
    _bcmFieldInternalEndStageAMFTFP = 390,
    _bcmFieldInternalEndStageAEFTFP = 391,
    _bcmFieldInternalElementCount = 392
} _bcm_field_internal_element_t;

#define BCM_FIELD_TYPE_ENUM_PROPERTIES 0xabcdef12

#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Typedef:
 *     _field_action_bmp_t
 * Purpose:
 *     Action bit map for tracking configured actions for each entry,
 *     used only in WB.
 */
typedef struct _field_action_bmp_s {
    SHR_BITDCL  *w;
} _field_action_bmp_t;

#define _FP_ACTION_BMP_FREE(bmp)   sal_free((bmp).w)
#define _FP_ACTION_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_ACTION_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_ACTION_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))

/*
 * Typedef:
 *     _field_selector_bmp_t
 * Purpose:
 *     Action bit map for tracking configured actions for each entry,
 *     used only in WB.
 */
typedef struct _field_selector_bmp_s {
    SHR_BITDCL  *w;
} _field_selector_bmp_t;

#define _FP_SEL_BMP_FREE(bmp)   sal_free((bmp).w)
#define _FP_SEL_BMP_ADD(bmp, ctr)     SHR_BITSET(((bmp).w), (ctr))
#define _FP_SEL_BMP_REMOVE(bmp, ctr)  SHR_BITCLR(((bmp).w), (ctr))
#define _FP_SEL_BMP_TEST(bmp, ctr)    SHR_BITGET(((bmp).w), (ctr))

typedef struct _field_slice_sel_info_s {
    bcm_field_group_t     gid;
    _field_selector_bmp_t slice_sel[_FP_MAX_ENTRY_WIDTH];
    uint8 slice_sel_count;
} _field_slice_sel_info_t;

typedef struct _field_slice_group_info_s {
    bcm_field_group_t                  gid; /* Temp recovered GID       */
    int                           instance; /* Temp recovered Pipe Instance. */
    int                           priority; /* Temp recovered group priority */
    bcm_field_qset_t                  qset; /* Temp QSET retrieved      */
    bcm_pbmp_t                        pbmp; /* Temp recovered rep. port */
    int                              found; /* Group has been found     */
    int                              flags; /* Group flags */
    int                      action_res_id; /* Group Action Resolution id */
    int8                       efp_pri_key; /* EFP primary fpf3 value */
    int8                       efp_sec_key; /* EFP secondary fpf3 value */
    int8                       sel_ingress_entity[_FP_MAX_ENTRY_WIDTH];
    int8                       sel_dst_fwd_entity[_FP_MAX_ENTRY_WIDTH];
    int8                       sel_src_entity[_FP_MAX_ENTRY_WIDTH];
    int8                       sel_loopback_type[_FP_MAX_ENTRY_WIDTH];
    struct _field_slice_group_info_s *next;
#if defined(BCM_FIRELIGHT_SUPPORT)
    uint8                       ifp_dw_f3; /* IFP primary fpf3 value */
#endif
} _field_slice_group_info_t;

typedef struct _field_hw_qual_info_s {
    _field_sel_t pri_slice[2]; /* Potentially intraslice */
    _field_sel_t sec_slice[2]; /* Potentially intraslice */
} _field_hw_qual_info_t;

typedef struct _meter_config_s {
    uint8 meter_mode;
    uint8 meter_mode_modifier;
    uint16 meter_idx;
    uint8 meter_update_odd;
    uint8 meter_test_odd;
    uint8 meter_update_even;
    uint8 meter_test_even;
} _meter_config_t;
typedef struct _field_table_pointers_s {
    char *fp_global_mask_tcam_buf;
    char *fp_gm_tcam_x_buf;
    char *fp_gm_tcam_y_buf;
    uint32 *fp_tcam_buf;
    uint32 *fp_tcam_x_buf;
    uint32 *fp_tcam_y_buf;
} _field_table_pointers_t;


/* Flags for is_present_bmp in _field_entry_oam_wb_t */
#define _FP_OAM_STAT_ACTION_WB_SYNC       (1 << 0)
#define _FP_EXTENDED_STATS_WB_SYNC        (1 << 1)

typedef struct _field_oam_stat_action_wb_s {
    bcm_stat_object_t stat_object;
    uint32 stat_mode_id;
} _field_oam_stat_action_wb_t;

typedef struct _field_oam_action_recover_s {
    int8 flex_pool_id;
    bcm_field_stat_action_t action;
}_field_oam_action_recover_t;

typedef struct _field_extended_stat_wb_s {
    uint32 flex_mode;
    int hw_flags;
    uint8 flex_stat_map;
    bcm_field_stat_t stat_id;
} _field_extended_stat_wb_t;

/* Per entry data that can be synced and recovered */
typedef struct _field_entry_oam_wb_s {
    uint32 is_present_bmp; /* Bitmap to notify if data present */
    /* Oam Lm Stat Actions */
    uint8 oam_stat_action_count;     /* Number of oam stat actions */
    _field_oam_stat_action_wb_t *oam_stat_action_wb;
                                     /* Oam Stat Action */
    /* Extended Stats */
    _field_extended_stat_wb_t *extended_stat_wb;
                                    /* Extended Stats */
} _field_entry_oam_wb_t;

/* per Entry facricQueue param uinfo to be synced and recovered */
typedef struct _field_action_fabricQueue_wb_s {
    uint32 cosq_gport;
#if defined(BCM_KATANA_SUPPORT)
    uint32 profile_idx;
#endif
} _field_action_fabricQueue_wb_t;
/* Per entry policer data that can be synced and recovered */
typedef struct _field_entry_policer_wb_s {
    uint16             policer_delta_rates[_FP_POLICER_LEVEL_COUNT][4];
                        /* Delta rates between actual hw and configured
                         Committed rate, Committed burst,
                         Peak rate, Peak burst at each level of policer.*/
} _field_entry_policer_wb_t;

typedef struct _field_action_redir_wb_s {
    bcm_field_redirect_config_t params;
    uint8 valid;
} _field_action_redir_wb_t;

typedef struct _field_action_copytocpu_wb_s {
    bcm_field_CopyToCpu_config_t params;
    uint8 valid;
} _field_action_copytocpu_wb_t;

typedef struct _field_action_OamStatObjSId_wb_s {
    bcm_stat_object_t   stat_object;
    uint8 valid;
} _field_action_OamStatObjSId_wb_t;

typedef struct _field_entry_wb_info_s {
    bcm_field_entry_t                 eid;                         /* Entry id */
    int                               prio;                        /* Entry Priority */
    bcm_field_stat_t                  sid;                         /* Entry stat id */
    bcm_policer_t                     pid;                         /* Entry level-0 policer id */
    bcm_policer_t                     pid1;                        /* Entry level-1 policer id */
    uint8                             dvp_type[_FP_MAX_ENTRY_WIDTH]; /* Entry DVP type */
    uint8                             svp_type[_FP_MAX_ENTRY_WIDTH]; /* Entry SVP type */
    uint8                             color_independent;           /* Entry Color flag */
    _field_entry_oam_wb_t             *f_ent_oam_wb;               /* Entry OAM info */
    _field_action_bmp_t               *action_bmp;                 /* Entry action param bit-map */
    uint8                             ing_gport_type;              /* Entry action Ingress Gport Type. */
    uint32                            port_prio_int_gport;         /* Entry action Prio int CosQ Gport*/
    uint32                            yp_port_prio_int_gport;      /* Entry action Prio int CosQ Gport*/
    uint32                            rp_port_prio_int_gport;      /* Entry action Prio int CosQ Gport*/
    bcm_gport_t                       ingress_mirror[4];           /* Ingress Mirror Gport */
    bcm_gport_t                       egress_mirror[4];            /* Egress Mirror Gport */
#ifdef BCM_APACHE_SUPPORT
    uint8                             ingress_oam_mep_type[4];     /* Entry OAM Drop Control */
    _field_action_redir_wb_t          action_redir_wb;             /* Entry Redirect Action Info */
    _field_action_copytocpu_wb_t      action_copytocpu_wb;         /* Entry CopyToCpu Action Info */
    _field_action_OamStatObjSId_wb_t  action_StatObjSId_wb;        /* Entry Stat Object Action Info */
#endif
    _field_entry_policer_wb_t         *f_ent_policer_wb;           /* Entry Policer info */
    _field_action_fabricQueue_wb_t    action_fabricQueue_wb;      /*hold params for fabricQueue action */
    uint8                             vfp_gport_type;             /* gport type 1 for trunk and 0 for modport*/
} _field_entry_wb_info_t;

#ifdef BCM_TOMAHAWK_SUPPORT
/* Type of elements that are being stored */
typedef enum _bcm_field_internal_type_e {
    _bcmFieldInternalVariable = 0,
    _bcmFieldInternalArray = 1,
    _bcmFieldInternalTypeCount = 2
} _bcm_field_internal_type_t;


typedef struct _field_tlv_s {
    _bcm_field_internal_element_t type;
    _bcm_field_internal_type_t basic_type;
    uint32 length;
    void *value;
} _field_tlv_t;


typedef struct _field_type_map_s {
    _bcm_field_internal_element_t element;
    int size;
    uint32 flags;
} _field_type_map_t;

typedef struct _field_wb_entry_s {
    _field_action_bmp_t act_bmp;
    int *hw_idx_array;
    /* This is used to store port larger than
     * 128 for actions like EgressPortsAdd.*/
    int *val_array0;
    int *val_array;
    uint32 flags[_FP_TH_MAX_ENTRY_WIDTH];
    uint32 action_count;
    uint8 action_param_type;
} _field_wb_entry_t;

/*
 * Structure to maintain the list of entries and actions
 * dependent on Mirror module and are to be recovered after
 * Mirror module reinit.
 */
typedef struct _bcm_field_mirror_actions_recover_s {
      _field_entry_t  *f_ent;
      bcm_field_action_t action;
      bcm_gport_t mtp_gport;
      struct _bcm_field_mirror_actions_recover_s *next;
} _bcm_field_mirror_actions_recover_t;
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined INCLUDE_L3
/*
 * Structure to maintain the list of entries and actions
 * dependent on L3 module and are to be recovered after
 * L3 module reinit.
 */
typedef struct _bcm_field_l3_callback_recover_s {
      _field_entry_t  *f_ent;
      bcm_field_action_t action;
      int param0;
      int hw_index;
      struct _bcm_field_l3_callback_recover_s *next;
} _bcm_field_l3_callback_recover_t;
extern _bcm_field_l3_callback_recover_t *_field_l3_cb;

extern int _bcm_field_l3_recover_callback(int unit);
#endif /* INCLUDE_L3 */
#endif /* BCM_WARM_BOOT_SUPPORT */

/* This reflects the 64,000bps granularity */
#define _FP_POLICER_REFRESH_RATE 64

typedef struct _field_stage_funct_s {
    int (*fp_stages_add)(int unit, _field_control_t *fc);
    int (*fp_stages_delete)(int unit, _field_control_t *fc);
} _field_stage_funct_t;

/*
 * Typedef:
 *     _field_sliceidx_bmp_t
 * Purpose:
 *     Bitmap to store the slice indexes of disabled entries.
 */
typedef struct _field_sliceidx_bmp_s{
    SHR_BITDCL  *w;
} _field_sliceidx_bmp_t;

#define _FP_SLICEIDX_BMP_ADD(bmp, sliceidx)     SHR_BITSET(((bmp).w), (sliceidx))
#define _FP_SLICEIDX_BMP_REMOVE(bmp, sliceidx)  SHR_BITCLR(((bmp).w), (sliceidx))
#define _FP_SLICEIDX_BMP_TEST(bmp, sliceidx)    SHR_BITGET(((bmp).w), (sliceidx))



/*
 * Typedef:
 *     _field_hintid_bmp_t
 * Purpose:
 *     Hint bit map for tracking allocation of hintids.
 */
typedef struct _field_hintid_bmp_s{
    SHR_BITDCL  *w;
} _field_hintid_bmp_t;

#define _FP_HINTID_BMP_ADD(bmp, hintid)     SHR_BITSET(((bmp).w), (hintid))
#define _FP_HINTID_BMP_REMOVE(bmp, hintid)  SHR_BITCLR(((bmp).w), (hintid))
#define _FP_HINTID_BMP_TEST(bmp, hintid)    SHR_BITGET(((bmp).w), (hintid))

typedef struct _field_hint_s {
    bcm_field_hint_t     *hint;  /* hint datastructure */
    struct _field_hint_s *next;  /* Next pointer to hint structure */
}_field_hint_t;

typedef struct _field_hints_s {
    bcm_field_hintid_t  hintid;         /* hint id */
    _field_hint_t      *hints;          /* List of hints linked to hintid */
    uint16              grp_ref_count;  /* Count of groups linked to a hintid */
    uint16              hint_count;     /* Count of hints linked to a hintid */
    struct _field_hints_s *next;        /* Pointer to next hint_id present
                                           in the hash_index */
}_field_hints_t;

/*
 * Typedef:
 *     _field_control_t
 * Purpose:
 *     One structure for each StrataSwitch Device that holds the global
 *     field processor metadata for one device.
 */
struct _field_control_s {
    int                    init;          /* TRUE if field module has been */
                                          /* initialized                   */
    uint8                  flags;         /* Module specific flags, as follows */
#define _FP_COLOR_INDEPENDENT          (1 << 0)
#define _FP_INTRASLICE_ENABLE          (1 << 1)
#define _FP_EXTERNAL_PRESENT           (1 << 2)
#ifdef BCM_WARM_BOOT_SUPPORT
#define _FP_STABLE_SAVE_LONG_IDS       (1 << 3)
#endif
#define _FP_STAT_SYNC_ENABLE           (1 << 4)
#define _FP_POLICER_GROUP_SHARE_ENABLE (1 << 5)
#define _FP_EXACTMATCH_STAGE_SKIP_DETACH (1 << 6) /* If set EM stage won't be disturbed during bcm_field_init */

/* This is enabled or disabled through fieldControl
 * EgressAutoExpandWithPolicer
 */
#define _FP_AUTOEXPAND_WITH_POLICER (1 << 7)

    bcm_field_stage_t      stage;         /* Default FP pipeline stage.    */
    int                    tcam_ext_numb; /* Slice number for external.    */
                                          /* TCAM (-1 if not present).     */
    _field_udf_t           *udf;          /* field_status->group_total     */
                                          /* elements indexed by priority. */
    struct _field_group_s  *groups;       /* List of groups in unit.       */
    struct _field_stage_s  *stages;       /* Pipeline stage FP info.       */
    _field_funct_t         functions;     /* Device specific functions.    */
    _field_policer_t       **policer_hash;/* Policer lookup hash.          */
    uint32                 policer_count; /* Number of active policers.    */
#ifdef BCM_TRIUMPH3_SUPPORT
    uint8                  ingress_logical_policer_pools_mode;
#endif
    _field_stat_t          **stat_hash;   /* Counter lookup hash.          */
    uint32                 stat_count;    /* Number of active counters.    */
    _field_hintid_bmp_t    hintid_bmp;    /* Bitmap of Created Hint Ids    */
    _field_hints_t         **hints_hash;  /* Hints lookup hash             */
#ifdef BCM_WARM_BOOT_SUPPORT
#define _FIELD_SCACHE_PART_COUNT 0x2
    /* Size of Level2 warm boot cache */
    uint32                 scache_size[_FIELD_SCACHE_PART_COUNT];
    uint32                 scache_usage;  /* Actual scache usage            */
    uint32                 scache_pos;   /* Current position of scache
                                            pointer in _FIELD_ACACHE_PART_0 */
    uint32                 scache_pos1;  /* Current position of scache
                                            pointer in _FIELD_ACACHE_PART_1 */
    /* Pointer to scache section */
    uint8                  *scache_ptr[_FIELD_SCACHE_PART_COUNT];
    uint8                  l2warm;        /* Use the stored scache data     */
    _field_slice_group_info_t *group_arr; /* Temp recovered group info      */
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    uint32                 last_allocated_lt_eid; /* Last allocated LT ID.  */
    struct _field_presel_entry_s **presel_db; /* Presel Entry lookup database.*/
    struct _field_presel_info_s  *presel_info;  /* Preselector information. */
    _field_lt_config_t           *lt_info[_FP_MAX_NUM_PIPES][_FP_MAX_NUM_LT];

    /* all number of possible combinations of _bcmFieldQualifyData0~9 */
#define _EXTD_QSET_COMB_SIZE (1023)
    /* number_of_qualify, _bcmFieldQualifyData0~9 */
#define _EXTD_QSET_DATA_NUM  (11)
    /* qset for Tomahawk and later devices */
    int extd_qset_comb[_EXTD_QSET_COMB_SIZE][_EXTD_QSET_DATA_NUM];
    int extd_qset_comb_init;
#endif /* !BCM_TOMAHAWK_SUPPORT */
    _field_stage_funct_t   stage_func;   /* Stage Init and Destroy functions. */
#if defined(BCM_TRIUMPH3_SUPPORT)
    int _fp_egr_ifp_mod_fields_size;
    _field_action_l2mod_index_bmp_t field_action_l2mod_index_bmp;
#endif /* (BCM_TRIUMPH3_SUPPORT) */

#if defined (BCM_KATANA_SUPPORT)
    _field_fabricQueue_action_t *field_fabricQueue_action_list; /* fabricQ action list*/
#endif /* (BCM_KATANA_SUPPORT)*/
    uint32 last_allocated_eid;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint16 wb_current_version;
    uint16 wb_recovered_version;
#endif
    uint32 pipe_map;
#if defined(BCM_TOMAHAWK_SUPPORT)
    uint8 ifp_em_meter_in_use[_FP_MAX_NUM_PIPES][_FIELD_MAX_METER_POOLS];
    soc_profile_mem_t            eh_mask_profile; /* HiGiG2 extended Header
                                                   * Mask Profile.
                                                   */
#endif
    soc_profile_mem_t redirect_profile;     /* Redirect action memory profile.*/
#if defined(BCM_TOMAHAWK3_SUPPORT)
    soc_profile_mem_t redirect_profile1;    /* Redirect action memory profile.*/
#endif
    soc_profile_mem_t ddrop_profile;        /* MatchPbmpDrop action memory profile.*/
    soc_profile_mem_t dredirect_profile;    /* MatchPbmpDrop action memory profile.*/
};


/*
 *  _field_group_add_fsm struct.
 *  Field group creation state machine.
 *  Handles sanity checks, resources selection, allocation.
 */
typedef struct _field_group_add_fsm_s {
    /* State machine data. */
    uint8                 fsm_state;     /* FSM state.                     */
    uint8                 fsm_state_prev;/* Previous FSM state.            */
    uint32                flags;         /* State specific flags.          */
    int                   rv;            /* Error code.                    */

    /* Field control info. */
    _field_control_t      *fc;           /* Field control structure.       */
    _field_stage_t        *stage_fc;     /* Field stage control structure. */


    /* Group information. */
    int                    priority;     /* New group priority.            */
    bcm_field_group_t      group_id;     /* SW Group id.                   */
    bcm_pbmp_t             pbmp;         /* Input port bitmap.             */
    bcm_field_qset_t       qset;         /* Qualifiers set.                */
    bcm_field_aset_t       aset;         /* Action set.                    */
    bcm_field_group_mode_t mode;         /* Group mode.                    */
    bcm_field_presel_set_t preselset;    /* set of preselector Ids.        */
    uint16                 aset_size;    /* Group's Aset size              */
    /* Allocated data structures. */
    _field_group_t         *fg;          /* Allocated group structure.     */
    bcm_field_hintid_t     hintid;       /* HintId to be linked to a group */
    int                    action_res_id;  /* Action resolution id */
} _field_group_add_fsm_t;

#define _BCM_FP_GROUP_ADD_STATE_START                  (0x1)
#define _BCM_FP_GROUP_ADD_STATE_ALLOC                  (0x2)
#define _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE            (0x3)
#define _BCM_FP_GROUP_ADD_STATE_ASET_VALIDATE          (0x4)
#define _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET          (0x5)
#define _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE         (0x6)
#define _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE         (0x7)
#define _BCM_FP_GROUP_ADD_STATE_HW_QUAL_LIST_GET       (0x8)
#define _BCM_FP_GROUP_ADD_STATE_UDF_UPDATE             (0x9)
#define _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP     (0xa)
#define _BCM_FP_GROUP_ADD_STATE_EXTERNAL_GROUP_CREATE  (0xb)
#define _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS           (0xc)
#define _BCM_FP_GROUP_ADD_STATE_SLICE_ALIGN            (0xd)
#define _BCM_FP_GROUP_ADD_STATE_END                    (0xe)

#define _BCM_FIELD_PIPE0_GROUP              (0x0)
#define _BCM_FIELD_PIPE1_GROUP              (0x1)
#define _BCM_FIELD_PIPE2_GROUP              (0x2)
#define _BCM_FIELD_PIPE3_GROUP              (0x3)
#define _BCM_FIELD_PIPE4_GROUP              (0x4)
#define _BCM_FIELD_PIPE5_GROUP              (0x5)
#define _BCM_FIELD_PIPE6_GROUP              (0x6)
#define _BCM_FIELD_PIPE7_GROUP              (0x7)
#define _BCM_FIELD_GLOBAL_GROUP             (0xf)

#define _BCM_FP_GROUP_ADD_STATE_STRINGS \
{                                  \
    "None", \
    "GroupAddStart",               \
    "GroupAddAlloc",               \
    "GroupAddStateQsetUpdate",     \
    "GroupAddSelCodesGet",         \
    "GroupAddQsetAlternate",       \
    "GroupAddSliceAllocate",       \
    "GroupAddHwQualListGet",       \
    "GroupAddUdfUpdate",           \
    "GroupAddAdjustVirtualMap",    \
    "GroupAddExternalGroupCreate", \
    "GroupAddCamCompress",         \
    "GroupAddEnd"                  \
}

#define _BCM_FP_GROUP_ADD_INTRA_SLICE          (1 << 0)
#define _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY     (1 << 1)
#define _BCM_FP_GROUP_ADD_WLAN                 (1 << 2)
#define _BCM_FP_GROUP_ADD_SMALL_SLICE          (1 << 3)
#define _BCM_FP_GROUP_ADD_LARGE_SLICE          (1 << 4)

/* _bcm_field_DestType_e  - Destination types. */
typedef enum _bcm_field_DestType_e {
    _bcmFieldDestTypeNhi,   /* Next Hop Interface */
    _bcmFieldDestTypeL3mc,  /* IPMC group         */
    _bcmFieldDestTypeL2mc,  /* L2 Multicast group */
    _bcmFieldDestTypeDvp,   /* Virtual Port       */
    _bcmFieldDestTypeDglp,  /* Mod/Port/Trunk     */
    _bcmFieldDestTypeNone,  /* None               */
    _bcmFieldDestTypeEcmp,  /* ECMP group         */
    _bcmFieldDestTypeCount  /* Always Last. Not a usable value. */
} _bcm_field_DestType_t;

#define _FIELD_D_TYPE_MASK  (0x7)
#define _FIELD_D_TYPE_POS   \
       ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit) || \
         SOC_IS_TRIUMPH3(unit)) ? (18) : \
       ((SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit) || \
         SOC_IS_GREYHOUND2(unit)) ? (16) : \
       ((SOC_IS_HURRICANE3(unit)) ? (15) : (14))))
#define _FIELD_D_TYPE_INSERT(_d_, _dtype_)     \
       ((_d_) |= ((uint32) (_dtype_) << _FIELD_D_TYPE_POS))
#define _FIELD_D_TYPE_RESET(_d_)                \
       ((_d_) &= ~(_FIELD_D_TYPE_MASK << _FIELD_D_TYPE_POS))
#define _FIELD_D_TYPE_MASK_INSERT(_d_)          \
       ((_d_) |= (_FIELD_D_TYPE_MASK << _FIELD_D_TYPE_POS))

/* Insert destination type value at specific bit offset */
#define _FIELD_D_TYPE_INSERT_BITPOS(_d_, _dtype_, _bitpos_)   \
       ((_d_) |= ((uint32) (_dtype_) << (_bitpos_)))
#define _FIELD_D_TYPE_RESET_BITPOS(_d_, _bitpos_)             \
       ((_d_) &= ~(_FIELD_D_TYPE_MASK << (_bitpos_)))
#define _FIELD_D_TYPE_MASK_INSERT_BITPOS(_d_, _bitpos_)              \
       ((_d_) |= (_FIELD_D_TYPE_MASK << (_bitpos_)))

extern soc_field_t _trx_src_class_id_sel[], _trx_dst_class_id_sel[],
    _trx_interface_class_id_sel[];

#define COPY_TO_CPU_CANCEL                 0x2
#define SWITCH_TO_CPU_CANCEL               0x3
#define COPY_AND_SWITCH_TO_CPU_CANCEL      0x6

/*
 * Prototypes of Field Processor utility funtions
 */
extern int _bcm_field_meter_free_get(const _field_stage_t *stage_fc,
                                     const _field_group_t *fg, const _field_slice_t *fs);
extern int _bcm_field_meters_total_get(const _field_stage_t *stage_fc,
                                       int instance, const _field_slice_t *fs);
extern int _bcm_field_counter_free_get(const _field_stage_t *stage_fc,
                                       const _field_slice_t *fs);
extern int _bcm_field_counters_total_get(const _field_stage_t *stage_fc,
                                         const _field_slice_t *fs);
extern int _bcm_field_entries_free_get(int unit, _field_slice_t *fs,
                                       _field_group_t *fg, int *free_cnt);
extern char * _field_action_name(bcm_field_action_t action);

extern int _bcm_field_qualifiers_info_get(int unit, _field_stage_t *stage_fc,
                                          bcm_field_qset_t *qset_req,
                                          _bcm_field_qual_info_t ***f_qual_arr,
                                          uint16 *qual_arr_size);
extern void _bcm_field_selcode_qual_arr_free(int unit,
                                          _bcm_field_qual_info_t ***f_qual_arr,
                                          uint16 qual_count);
extern int _bcm_field_group_linked_list_insert(int unit,
                                               _field_group_add_fsm_t *fsm_ptr);
extern int _bcm_field_group_linked_list_remove(int unit, _field_group_t *fg);
extern int _bcm_field_group_add(int unit, _field_group_add_fsm_t *fsm_ptr);
extern int _bcm_field_group_stage_get(int unit, bcm_field_qset_t *qset_p,
                                      _field_stage_id_t *stage);
extern int _bcm_field_group_stage_validate(int unit, _field_stage_id_t stage);
extern int _bcm_field_control_set(int unit,  _field_control_t *fc,
                                  bcm_field_control_t control, uint32 state);
extern int _bcm_field_control_get(int unit,  _field_control_t *fc,
                                  bcm_field_control_t control, uint32 *state);
extern int _bcm_field_stage_fpf_destroy(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_stage_fpf_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_data_qualifier_destroy(int unit, int qid);
extern int _bcm_field_group_slice_count_get(uint32 flags, int *count);
extern int _field_egr_ports_recovery_control_get(int unit,
                 _field_egr_ports_recovery_t **f_egr_recovery);
extern int _bcm_trx_field_egr_ports_recovery_add (int unit,
                                             bcm_field_entry_t entry,
                                             bcm_pbmp_t pbmp);
extern int _bcm_trx_field_egr_ports_recovery_search (int unit, int port,
                                                bcm_field_entry_t entry);
extern int _bcm_trx_field_egr_ports_recovery_print (int unit);
extern int _bcm_trx_field_egr_ports_recovery_entry_ids_list_get (int unit,
                                   _field_egr_ports_entry_ids_list_t **entries,
                                   int *entry_count);
extern int _bcm_trx_field_egr_ports_recovery_entry_list_destroy (int unit,
                                 _field_egr_ports_entry_ids_list_t **entries);
extern int _bcm_trx_field_egr_ports_recovery_entry_remove (int unit,
                                                     bcm_field_entry_t entry);
extern int _bcm_trx_field_egr_ports_recovery_entry_pbmp_get (int unit,
                                                   bcm_field_entry_t entry,
                                                   bcm_pbmp_t *redirectPbmp);
extern int _field_egr_ports_recovery_scache_recover (int unit,
                          _field_control_t *fc, _field_stage_t   *stage_fc,
                                                            uint8 *scache_ptr);
extern int _field_egr_ports_recovery_scache_sync(int unit, _field_control_t *fc,
                                _field_stage_t   *stage_fc, uint8 *scache_ptr);
extern int _field_egr_ports_recovery_wb_upgrade_add (int unit,
                                                  _field_group_t *fg);
extern int _field_hints_control_get (int unit, bcm_field_hintid_t hint_id,
                                     _field_hints_t **f_ht);
extern int _bcm_field_hints_group_info_update (int unit,_field_group_t *fg);
extern int _bcm_field_hints_group_count_update (int unit,
                                                bcm_field_hintid_t hintid,
                                                uint8 action);
extern int _bcm_field_hints_add (int unit, bcm_field_hintid_t hint_id,
                                  bcm_field_hint_t *hint);
extern int _bcm_field_hints_node_get (int unit, _field_hint_t *hint_list,
                             bcm_field_hint_t *hint, _field_hint_t **hint_node);
extern int _bcm_field_hints_get(int unit, bcm_field_hintid_t hint_id,
                                bcm_field_hint_t *hint);
extern int _bcm_field_hints_delete (int unit, bcm_field_hintid_t hint_id,
                                    bcm_field_hint_t *hint);
extern int _bcm_field_hints_delete_all (int unit, bcm_field_hintid_t hint_id);
extern int _bcm_field_hints_dump (int unit);
extern int _bcm_field_hints_display (int unit, bcm_field_hintid_t hint_id);
extern int _bcm_field_hints_scache_sync (int unit, _field_control_t *fc,
                                  _field_stage_t *stage_fc, uint8 *scache_ptr);
extern int _bcm_field_hints_scache_recover (int unit, _field_control_t *fc,
                                  _field_stage_t *stage_fc, uint8 *scache_ptr);
extern int _bcm_hints_scache_hintid_sync (int unit, _field_stage_t *stage_fc, uint8 *scache_ptr);
extern int _bcm_hints_scache_hintid_recover (int unit, _field_stage_t *stage_fc, _field_control_t *fc,
                                   uint8 *scache_ptr);
extern int _field_group_slice_selectors_sync (int unit, uint8 *scache_ptr,
                                              _field_stage_id_t stage_id);
extern int _field_group_slice_selectors_recover (int unit, uint8 *scache_ptr,
                                                 _field_stage_id_t stage_id);
extern int _bcm_field_slice_group_get_next(int unit,
                                   int instance,
                                   _field_stage_id_t stage_id,
                                   int slice_id,
                                   _field_group_t **curr_group,
                                   _field_group_t **next_group);
extern int _field_fill_slice_array(int unit, _field_group_t *fg, uint8 *slice_array);
extern int _field_control_get(int unit, _field_control_t **fc);
extern int _field_stage_control_get(int unit, _field_stage_id_t stage,
                                    _field_stage_t **stage_fc);
extern int _bcm_field_stat_hw_alloc(int unit, _field_entry_t *f_ent);
extern int _bcm_field_stat_array_init(int unit, _field_control_t *fc,
                                      _field_stat_t *f_st, int nstat,
                                      bcm_field_stat_t *stat_arr);
extern int _field_stat_destroy(int unit, int sid);
extern int _bcm_field_stage_slice_count_get(
                int unit,
                bcm_field_stage_t stage_id,
                int *slice_count);
extern int _bcm_field_stage_slice_info_get(int unit,
               bcm_field_stage_t stage_id,
               bcm_pbmp_t pbmp,
               int slice_no,
               bcm_field_stage_slice_info_t *slice_info);
extern int _field_group_get(int unit, bcm_field_group_t gid,
                            _field_group_t **group_p);
extern int _bcm_field_group_id_generate(int unit, bcm_field_group_t *group);
extern int _bcm_field_group_default_aset_set(int unit, _field_group_t *fg);
extern int _field_entry_get(int unit, bcm_field_entry_t eid, uint32 flags,
                            _field_entry_t **enty_p);
extern int _field_group_entry_add (int unit, _field_group_t *fg,
                                   _field_entry_t *f_ent);
extern int _field_group_entry_delete (int unit, _field_group_t *fg,
                                      _field_entry_t *f_ent);
extern int _bcm_field_entry_tcam_parts_count (int unit,
                                              _field_stage_id_t stage_id,
                                              uint32 group_flags,
                                              int *part_count);
extern int _bcm_field_lt_entry_tcam_parts_count (int unit,
                                                 _field_stage_id_t stage_id,
                                                 uint32 group_flags,
                                                 int *part_count);
extern int _bcm_field_entry_flags_to_tcam_part (int unit, uint32 entry_flags,
                                                _field_group_t *fg,
                                                uint8 *entry_part);
extern int _bcm_field_tcam_part_to_entry_flags(int unit, int entry_part,
                                               _field_group_t *fg,
                                               uint32 *entry_flags);
extern int
_field_udf_hints_present_get(int unit, bcm_field_entry_t eid,
                             bcm_udf_id_t udf_id,
                             uint8 *hints_present);
extern int _field_port_filter_enable_set(int unit, _field_control_t *fc,
                                         uint32 state);
extern int _field_meter_refresh_enable_set(int unit, _field_control_t *fc,
                                           uint32 state);
extern int _field_qual_info_create(bcm_field_qualify_t qid, soc_mem_t mem,
                                   soc_field_t fpf, int offset, int width,
                                    _qual_info_t **fq_p);
extern int _field_qualify32(int unit, bcm_field_entry_t entry,
                            int qual, uint32 data, uint32 mask);
extern int _field_qual_offset_get(int unit, _field_entry_t *f_ent, int qid,
                                  _bcm_field_qual_offset_t **offset);
extern int _bcm_field_qualify_trunk(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_qualify_t qual,
                                    bcm_trunk_t data,
                                    bcm_trunk_t mask);
extern int _bcm_field_qualify_trunk_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t qual,
                                        bcm_trunk_t *data,
                                        bcm_trunk_t *mask);
extern int _bcm_field_qualify_InPorts(int unit,
                                      bcm_field_entry_t entry,
                                      bcm_field_qualify_t qual,
                                      bcm_pbmp_t data,
                                      bcm_pbmp_t mask);
extern int _field_qset_union(const bcm_field_qset_t *qset1,
                             const bcm_field_qset_t *qset2,
                             bcm_field_qset_t *qset_union);
extern int _field_qset_is_subset(const bcm_field_qset_t *qset1,
                                 const bcm_field_qset_t *qset2);
extern bcm_field_qset_t _field_qset_diff(const bcm_field_qset_t qset_1,
                                         const bcm_field_qset_t qset_2);
extern int _field_qset_is_empty(const bcm_field_qset_t qset);
extern int
_field_selector_diff(int unit, _field_sel_t *sel_arr, int part_idx,
                     _bcm_field_selector_t *selector,
                     uint8 *diff_count);

extern int _field_trans_flags_to_index(int unit, int flags, uint8 *tbl_idx);
extern int _field_qual_add (_field_fpf_info_t *fpf_info, int qid, int offset,
                            int width, int code);
extern int _bcm_field_tpid_hw_encode(int unit, uint16 tpid, uint32 *hw_code);
extern int _field_tpid_hw_decode(int unit, uint32 hw_code, uint16 *tpid);
extern int _field_entry_prio_cmp(int prio_first, int prio_second);
extern int _bcm_field_prio_mgmt_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_prio_mgmt_deinit(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_entry_prio_mgmt_update(int unit, _field_entry_t *f_ent,
                                              int flag, uint32 old_location);
extern int
_bcm_field_prio_mgmt_slice_reinit(int            unit,
                                  _field_stage_t *stage_fc,
                                  _field_slice_t *fs);
extern int _bcm_field_entry_target_location(int unit, _field_stage_t *stage_fc,
                   _field_entry_t *f_ent, int new_prio, uint32 *new_location);
extern int _bcm_field_entry_counters_move(int unit, _field_stage_t *stage_fc,
                                          int old_index, int new_index);
extern int _bcm_field_sw_counter_update(int unit, _field_stage_t *stage_fc,
                                        soc_mem_t mem, int idx_min, int idx_max,
                                        char *buf, int flags);
extern int _bcm_field_counter_mem_get(int            unit,
                                      _field_stage_t *stage_fc,
                                      soc_mem_t      *counter_x_mem,
                                      soc_mem_t      *counter_y_mem
                                      );
extern int _bcm_field_counter_hw_alloc(int            unit,
                                       _field_entry_t *f_ent,
                                       _field_slice_t *fs
                                       );
extern int _bcm_field_stat_hw_free(int unit, _field_entry_t *f_ent);
extern int _bcm_field_entry_stat_detach(int unit, _field_entry_t *f_ent, int stat_id);
#ifdef INCLUDE_L3
#if defined (BCM_KATANA_SUPPORT) || defined (BCM_TRIDENT_SUPPORT) \
         || defined (BCM_TRIUMPH3_SUPPORT) || defined (BCM_GREYHOUND_SUPPORT)
extern int _field_nh_ref_count_incr (int unit, _field_entry_t *f_ent);
#endif /* BCM_KATANA_SUPPORT || BCM_TRIDENT_SUPPORT || BCM_TRIUMPH3_SUPPORT \
          || BCM_GREYHOUND_SUPPORT */

#endif
extern int _field_control_free(int unit, _field_control_t *fc);
extern int _field_egr_ports_recovery_control_free (int unit,
                        _field_egr_ports_recovery_t *f_egr_recovery);
extern int _field_stages_init(int unit, _field_control_t *fc);
extern int _field_stages_destroy(int unit, _field_control_t *fc);
extern int _field_stage_delete(int unit, _field_control_t *fc, _field_stage_t *stage_fc);

extern void _bcm_esw_fp_counters_collect(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _field_scache_pointer_init_wb(int unit, _field_control_t  *fc);
extern int _field_scache_pointer_init(int unit, _field_control_t  *fc);
#endif
extern int _field_hints_id_alloc (int unit, bcm_field_hintid_t *hint_id);

extern unsigned _field_stage_action_support_check(int              unit,
                                                  _field_group_t   *fg,
                                                  unsigned         action);
extern int _field_aset_install(int unit, _field_group_t *fg);
extern int _bcm_esw_field_group_mode_get(uint32 flags, bcm_field_group_mode_t *mode);
extern int _field_group_qset_update(int unit, _field_group_t *fg);
extern void _field_udf_usecount_update(_field_udf_t *udf,
                           bcm_field_qset_t *qset_old,
                           bcm_field_qset_t *qset_new);
extern int _field_group_vmap_delete(int unit, _field_stage_t *stage_fc,
                                    _field_group_t *fg, int install);
extern int _field_group_compress(int unit, _field_group_t *fg,
                                 _field_stage_t *stage_fc);
extern int _field_group_vmap_add(int unit, _field_stage_t *stage_fc,
                                 _field_group_t *fg);
extern int _field_fb_entries_enable_set(int unit, _field_group_t *fg,
                                 _field_slice_t *fs, int enable);
extern int _field_range_flags_check(int unit, uint32 flags);
extern int _field_fb_er_range_check_set(int unit, int range, uint32 flags, int enable,
                             bcm_l4_port_t min, bcm_l4_port_t max);
extern int _bcm_field_modify_range_checker(int unit, _field_stage_id_t stage_id,
        int range_id, uint32 flags, bcm_l4_port_t min, bcm_l4_port_t max);
extern int _field_action_delete(int unit, bcm_field_entry_t entry,
                        bcm_field_action_t action,
                        uint32 param0, uint32 param1);
extern void _bcm_field_linkscan_update(int unit, bcm_port_t port,
                                               bcm_port_info_t *portInfo);
extern int _field_group_deinit(int unit, _field_group_t *fg);

extern int _field_range_create(int unit, bcm_field_range_t *range,
                    uint32 flags, bcm_l4_port_t min,
                    bcm_l4_port_t max, bcm_if_group_t group);
extern int
_field_group_slice_entry_reserve(int unit, _field_group_t *fg,
                                 _field_slice_t **fs);
extern int
_bcm_field_slice_idx_allocate(int unit, _field_stage_t *stage_fc,
                              _field_group_t *fg, _field_slice_t *fs,
                              uint32 *slice_idx);
extern int
_field_entry_create_id(int unit, bcm_field_group_t group,
                       bcm_field_entry_t entry);
extern int
_bcm_field_entry_create_id(int unit,
                              bcm_field_group_t group,
                              bcm_field_entry_t entry);
extern int
_bcm_esw_field_detach(int unit);
extern int
_bcm_esw_field_init(int unit);
extern int
_field_entry_phys_destroy (int unit, _field_entry_t *f_ent);
extern int
_field_group_free_unused_slices(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg);
extern int
_field_entry_action_dirty(int unit, _field_entry_t *f_ent, int *dirty);
extern int
_field_entry_dirty(int unit, _field_entry_t *f_ent, int *dirty);
extern int
_field_tcam_policy_reinstall(int unit, bcm_field_entry_t entry);
extern int
_field_tcam_policy_install(int unit, bcm_field_entry_t entry);
extern int
_field_entry_remove(int unit, _field_control_t *fc, bcm_field_entry_t entry);
extern int
_field_entry_prio_set(int unit, bcm_field_entry_t entry, int prio);
extern int
_field_entry_qualifiers_init(int unit, _field_entry_t *f_ent);
extern int
_field_qual_offset_get_by_entry_type(int unit, _field_entry_t *f_ent, int qid,
                                     uint8 entry_type,
                                     _bcm_field_qual_offset_t **offset);
extern int
_field_efp_key_match_type_set_on_qual_delete(int unit, bcm_field_entry_t entry);
extern int _field_qualifier_gport_resolve(int unit, bcm_gport_t gport, int mask,
                                          int *out_p, int *out_m, int *out_t);
extern int _field_qualify_Port(int unit, bcm_field_entry_t entry,
                    bcm_module_t data_modid, bcm_module_t mask_modid,
                    bcm_port_t   data_port,  bcm_port_t   mask_port,
                    bcm_field_qualify_t qual);
extern int _field_qualify_L2PayloadFirstEightBytes_get(int unit,
                                            bcm_field_entry_t entry,
                                            bcm_field_qualify_t qual,
                                            uint32 *data1, uint32 *data2,
                                            uint32 *mask1, uint32 *mask2);
extern int _field_qualify_L2PayloadFirstEightBytes(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t qual,
                                        uint32 data1, uint32 data2,
                                        uint32 mask1, uint32 mask2);
extern int _field_efp_qualify32(int unit, bcm_field_entry_t entry,
                                int qual, uint32 *data, uint32 *mask);
extern int _field_dest_type_qualify(int unit, bcm_field_entry_t entry,
            bcm_field_qualify_t qual, uint32 *data, uint32 *mask,
            uint32 flags);
extern int
_field_qualify_VlanTranslationHit(int               unit,
                                  bcm_field_entry_t entry,
                                  uint8             *data,
                                  uint8             *mask
                                 );
extern int
_field_qualify_PacketRes(int               unit,
                         bcm_field_entry_t entry,
                         uint32            *data,
                         uint32            *mask
                         );
extern int _field_qualify_ip6(int unit, bcm_field_entry_t entry,
                   bcm_field_qualify_t qual,
                   bcm_ip6_t data, bcm_ip6_t mask);
extern int _field_qualify_macaddr(int unit, bcm_field_entry_t entry,
                    bcm_field_qualify_t qual,
                    bcm_mac_t data, bcm_mac_t mask);
extern int _bcm_field_entry_group_find(int unit, bcm_field_entry_t eid,
                            _field_group_t **group_p);
extern int _field_qualify_source_virtual_port(int unit, bcm_field_entry_t entry,
                       int qual, uint32 data, uint32 mask, int ingress_entity);
extern int _field_qualify_VlanFormat(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual_id,
                          uint8 data, uint8 mask);
extern int _field_qualify_VlanFormat_get(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual_id,
                          uint8 *data, uint8 *mask);
extern int _field_qualify_IpProtocolCommon(int unit, bcm_field_entry_t entry,
                                bcm_field_qualify_t qual_id,
                                bcm_field_IpProtocolCommon_t protocol);
extern int _field_qualify_IpProtocolCommon_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_qualify_t qual_id,
                                    bcm_field_IpProtocolCommon_t *protocol);
extern int _field_qualify_IpFrag(int unit, bcm_field_entry_t entry,
                      bcm_field_qualify_t qual_id,
                      bcm_field_IpFrag_t frag_info);
extern int _field_params_api_to_hw_adapt(int unit, bcm_field_action_t action,
                              uint32 *param0, uint32 *param1);
extern int _field_action_add(int unit, _field_control_t *fc,
                  bcm_field_entry_t entry, _field_action_t *fa);
extern int _bcm_field_action_ports_add(int unit,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 param0, uint32 param1,
                         uint32 param2, uint32 param3,
                         uint32 param4, uint32 param5);
extern int _field_params_hw_to_api_adapt(int unit, bcm_field_action_t action,
                              uint32 *param0, uint32 *param1);
extern int
_bcm_esw_field_qualify_aux_tag(int                 unit,
                               bcm_field_entry_t   entry,
                               bcm_field_qualify_t qual,
                               uint32              data,
                               uint32              mask
                               );
extern int _field_stat_create(int unit, bcm_field_group_t group, int nstat,
                   bcm_field_stat_t *stat_arr, uint32 flags, int *stat_id);
extern int _field_entry_stat_attach(int unit, _field_entry_t *f_ent,
                                    int stat_id);
extern int _field_policer_hw_flags_set(int unit, _field_policer_t *f_pl, uint32 flags);
extern int _field_policer_mode_support(int unit, _field_entry_t *f_ent,
                                int level, _field_policer_t *f_pl);
extern int _field_entry_policer_detach(int unit, _field_entry_t *f_ent, int level);
extern int _field_policer_destroy2(int unit, _field_control_t *fc,
                                   _field_policer_t *f_pl);
extern int _field_policer_destroy(int unit, bcm_policer_t pid);

extern int _field_data_qualifier_create(int unit,
                                  bcm_field_data_qualifier_t *data_qualifier);
extern void _field_data_qualifier_debug(int unit,
                            _field_stage_t *stage_fc,
                            _field_data_qualifier_t *f_dq);
extern int _field_data_qualifier_destroy_all(int unit);
extern int _field_qualify_data (int unit, bcm_field_entry_t eid, int qual_id,
                     uint8 *data, uint8 *mask, uint16 length);
extern int _field_dest_type_clear(int unit, bcm_field_entry_t entry,
        bcm_field_qualify_t qual, uint32 *data, uint32 *mask);
extern int _field_qualify_Port_get(int unit, bcm_field_entry_t entry,
                        bcm_module_t *data_modid,
                        bcm_module_t *mask_modid,
                        bcm_port_t   *data_port,
                        bcm_port_t   *mask_port,
                        bcm_field_qualify_t qual);
extern int _bcm_field_action_ports_get(int unit,
                         bcm_field_entry_t entry,
                         bcm_field_action_t action,
                         uint32 *param0, uint32 *param1,
                         uint32 *param2, uint32 *param3,
                         uint32 *param4, uint32 *param5);
extern int _field_policer_create(int unit, bcm_policer_config_t *pol_cfg,
                                 uint32 flags, bcm_policer_t *pid);
extern int _field_qualify_ip6_get(int unit, bcm_field_entry_t entry,
                   bcm_field_qualify_t qual,
                   bcm_ip6_t *data, bcm_ip6_t *mask);
extern int _field_qualify_macaddr_get(int unit, bcm_field_entry_t entry,
                           bcm_field_qualify_t qual,
                           bcm_mac_t *data, bcm_mac_t *mask);
extern int _field_qualify_IpFrag_get(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual_id,
                          bcm_field_IpFrag_t *frag_info);
extern int _field_qualify_cpu_queue(int unit, bcm_field_entry_t entry,
                         int qual, uint32 data, uint32 mask);
extern int _field_qualify_cpu_queue_get(int unit, bcm_field_entry_t entry,
                         int qual, uint8 *data, uint8 *mask);
extern int _field_invalid_actions_remove(int unit, _field_entry_t *f_ent);
extern int _field_entry_prio_set(int unit, bcm_field_entry_t entry, int prio);
extern int _field_qualifier32_input_range_check (int unit, int qual,
                              bcm_field_entry_t entry, uint8 *data, int size);
extern int
_bcm_esw_field_stat_counter_get(int unit, int sync_mode, uint32 stat_id,
                               bcm_field_stat_t stat, uint32 num_entries,
                               uint32 *counter_indexes,
                               bcm_stat_value_t *counter_values);
extern int
_bcm_esw_field_stat_get(int unit, int sync_mode, int stat_id,
                       bcm_field_stat_t stat, uint64 *value);
extern int
_bcm_esw_field_stat_get32(int unit, int sync_mode, int stat_id,
                         bcm_field_stat_t stat, uint32 *value);
extern int
_bcm_field_qualify_InPorts_get(int unit,  bcm_field_entry_t entry,
                               bcm_field_qualify_t qual,
                               bcm_pbmp_t *data,
                               bcm_pbmp_t *mask);
extern int
_field_qualify_VlanTranslationHit_get(int               unit,
                                      bcm_field_entry_t entry,
                                      uint8             *data,
                                      uint8             *mask);
extern int
_bcm_esw_field_udf_chunks_to_int_qset(int unit, uint32 udf_chunks_bmap,
                                      bcm_field_qset_t *qset);
extern int
_bcm_xgs4_field_qualify_udf_data_elem(int unit, bcm_field_entry_t eid,
                         bcmi_xgs4_udf_offset_info_t *offset_info,
                         int data_idx, uint32 data, uint32 mask);

extern int
_bcm_esw_field_group_config_create(int unit,
                                  bcm_field_group_config_t *group_config,
                                  uint32 flags);
extern int
_bcm_esw_field_group_config_destroy(int unit,
                                  bcm_field_group_t group,
                                  uint32 flags);
extern void _field_range_dump(const char *pfx, _field_range_t *fr);
extern void _field_entry_phys_dump(int unit, _field_entry_t *f_ent,
                                   uint8 entry_part);
extern void _field_action_dump(const _field_action_t *fa);
extern void _field_policers_dump(int unit, _field_entry_t *f_ent);
extern int _field_stat_dump(int unit, const _field_entry_t *f_ent);
extern void _field_qual_list_dump(char *prefix, _field_group_t *fg,
                                  uint8 entry_part, char *suffix);
extern void _field_selcode_dump(int unit, char *prefix, _field_sel_t *sel_codes,
                    char *suffix, _field_stage_id_t stage_id);
extern void _field_slice_dump(int unit, char *prefix, _field_group_t *fg,
                              _field_slice_t *fs, char *suffix);
extern void _field_group_status_dump(const bcm_field_group_status_t *gstat);
extern int _field_action_delete(int unit, bcm_field_entry_t entry,
                                bcm_field_action_t action,
                                uint32 param0, uint32 param1);
extern int _bcm_field_is_entry_stage_valid(int unit, bcm_field_entry_t entry,
                                _field_stage_id_t stage);
extern int _field_stat_destroy_all(int unit);
extern int _bcm_field_entry_backup(int unit, bcm_field_entry_t entry_id);
extern int _bcm_field_entry_restore(int unit, bcm_field_entry_t entry_id);
extern int _bcm_field_entry_cleanup(int unit, bcm_field_entry_t entry_id);
extern int _field_qualify_PacketRes_get(int unit, bcm_field_entry_t entry,
                                        uint32 *data, uint32 *mask);
extern int
_field_virtual_priority_support(int unit, _field_stage_t *stage_fc,
                                int *support);
extern int
_field_state_cleanup(int unit, _field_control_t *fc);
#if defined(BCM_TRX_SUPPORT)
extern int
_bcm_field_flex_port_detach(int unit, bcm_port_t port);
extern int
_bcm_field_flex_port_attach(int unit, bcm_port_t port);
extern int
_bcm_field_flex_port_update(int unit,
                            bcm_pbmp_t detached_pbmp,
                            bcm_pbmp_t attached_pbmp);
extern int
_bcm_esw_field_group_ports_add(
    int unit,
    bcm_field_group_t group,
    bcm_pbmp_t pbmp);
extern int
_bcm_esw_field_group_ports_remove(
    int unit,
    bcm_field_group_t group,
    bcm_pbmp_t pbmp);

extern int _bcm_field_pfs_entry_clear(int unit,
                               _field_group_t *fg,
                               bcm_port_t port);
#endif
extern int _bcm_esw_field_group_config_validate(
                          int unit,
                          bcm_field_group_config_t *group_config,
                          bcm_field_group_mode_t *group_mode);
#if defined (BCM_TRIUMPH2_SUPPORT)
extern int
bcmi_xgs4_field_qset_udf_id_multi_set(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset
    );
extern int
bcmi_xgs4_field_qset_udf_id_multi_get(
    int unit,
    bcm_field_qset_t qset,
    int max,
    int objects_list[],
    int *actual
    );
extern int
bcmi_xgs4_field_qset_udf_id_multi_delete(
    int unit,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset
    );
extern int
bcmi_xgs4_field_qualify_udf_data (int unit, bcm_field_entry_t eid,
    bcm_udf_id_t udf_id, int length, uint8 *data, uint8 *mask);
extern int
bcmi_xgs4_field_qualify_udf_get(int unit, bcm_field_entry_t eid,
                              bcm_udf_id_t udf_id, int max_length,
                              uint8 *data, uint8 *mask, int *actual_length);
#endif

#if defined BCM_TRX_SUPPORT
extern int _bcm_field_action_stat_flag_update (int unit, _field_entry_t *f_ent,
                                                _field_action_t *fa,
                                                uint8 is_action_added);
extern int _bcm_field_action_stat_conflict_check (int unit, _field_entry_t *f_ent,
                                                         _field_action_t *fa);
#endif
extern int _bcm_field_stat_destroy(int unit, int sid);
extern int _bcm_field_stat_hw_mode_get(int unit, _field_stat_t *f_st,
                                          _field_stage_id_t stage_id);
extern int _bcm_field_stat_array_to_bmap(int unit,
                                         _field_stat_t *f_st,
                                         uint32 *bmap);
extern int _bcm_field_stages_supported_qset_init(int unit,
                                                 _field_control_t *fc);
extern int _bcm_field_policer_get(int unit, bcm_policer_t pid,
                                  _field_policer_t **policer_p);
extern int _bcm_field_group_stat_destroy(int unit, bcm_field_group_t group);
extern int _bcm_field_stat_get(int unit, int sid, _field_stat_t **stat_p);
extern int _bcm_field_stat_id_alloc(int unit, int *sid);
extern int _field_stat_array_init(int unit, _field_control_t *fc, _field_stat_t *f_st,
                               int nstat, bcm_field_stat_t *stat_arr);
extern int _bcm_field_meter_pair_mode_get(int unit, _field_policer_t *f_pl,
                                          uint32 *mode);
extern int _field_policer_id_alloc(int unit, bcm_policer_t *pid);
extern int _field_meter_pool_free(int unit, _field_stage_t *stage_fc,
                                  int instance, uint8 pool_idx);
extern int _bcm_field_29bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter32_collect_t *result);

#if defined BCM_KATANA2_SUPPORT
extern int _bcm_field_kt2_entry_stat_extended_attach (int unit,
                                               _field_entry_t *f_ent, int stat_id,
                                               bcm_field_stat_action_t stat_action);
extern int _bcm_field_kt2_entry_stat_extended_get (int unit,
                                               _field_entry_t *f_ent, int *stat_id,
                                               bcm_field_stat_action_t *stat_action);
extern int _bcm_field_kt2_entry_stat_detach (int unit,
        _field_entry_t *f_ent, int stat_id);
extern int _bcm_field_kt2_extended_stat_hw_free (int unit, _field_entry_t *f_ent);
#endif
#if defined BCM_TRIDENT2PLUS_SUPPORT
extern int _field_td2plus_qualifiers_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_oam_stat_action_add (int unit, bcm_field_entry_t entry,
                                         bcm_field_oam_stat_action_t *oam_stat_action);
extern int _bcm_field_oam_stat_action_delete (int unit, bcm_field_entry_t entry,
                                         bcm_field_oam_stat_action_t *oam_stat_action);
extern int _bcm_field_oam_stat_action_delete_all (int unit,
                                                      bcm_field_entry_t entry);
extern int _bcm_field_oam_stat_action_get (int unit, bcm_field_entry_t entry,
                                         bcm_field_oam_stat_action_t *oam_stat_action);
extern int _bcm_field_oam_stat_action_get_all(int unit,
                                         bcm_field_entry_t entry, int oam_stat_max,
                                         bcm_field_oam_stat_action_t *oam_stat_action,
                                         int *oam_stat_count);
extern int _bcm_field_oam_stat_action_set (int unit, _field_entry_t *f_ent,
                                             soc_mem_t policy_mem, uint32 *policy_buf);
extern int _bcm_field_oam_stat_invalid_actions_remove (int unit,
                                                     _field_entry_t *f_ent);
extern int _field_oam_stat_action_dump (int unit, _field_entry_t *f_ent);
extern int _field_oam_stat_action_display (int unit);
extern int _bcm_field_oam_stat_actions_destroy (int unit, _field_entry_t *f_ent);
extern int _bcm_field_td2plus_entry_stat_extended_attach (int unit,
                                               _field_entry_t *f_ent, int stat_id,
                                               bcm_field_stat_action_t stat_action);
extern int _bcm_field_td2plus_entry_stat_extended_get (int unit,
                                               _field_entry_t *f_ent, int *stat_id,
                                               bcm_field_stat_action_t *stat_action);
extern void _field_td2plus_functions_init (_field_funct_t *functions);
extern int _bcm_field_td2plus_entry_stat_detach (int unit,
                                              _field_entry_t *f_ent, int stat_id);
extern int _bcm_field_td2plus_extended_stat_action_set (int unit,
                                               _field_entry_t *f_ent, soc_mem_t mem,
                                              int tcam_idx, uint32 *buf);
extern int _bcm_field_td2plus_extended_stat_hw_free (int unit, _field_entry_t *f_ent);
extern int _bcm_field_td2plus_stat_hw_alloc (int unit, _field_entry_t *f_ent);
extern int _bcm_field_td2plus_stat_hw_free (int unit, _field_entry_t *f_ent);
extern int _field_oam_lm_stat_pool_id_get (int unit, bcm_stat_object_t stat_object,
                                           uint32 stat_mode_id, int8 *flex_stat_pool_id);
#if defined BCM_WARM_BOOT_SUPPORT
extern int _field_oam_stat_action_count_get (int unit,
                uint8 oam_pool_instance, int8 *oam_stat_count);
extern int _field_oam_stat_action_wb_add (int unit, _field_entry_t *f_ent,
                                   _field_oam_action_recover_t *oam_action);
extern int _field_oam_stat_action_scache_sync (int unit, _field_stage_t *stage_fc,
                                  _field_entry_t *f_ent,_field_entry_oam_wb_t *f_ent_oam_wb);
extern int _field_oam_stat_action_scache_recover (int unit, _field_stage_t   *stage_fc,
                                  _field_entry_t *f_ent, _field_entry_oam_wb_t *f_ent_oam_wb);
extern int _field_extended_stat_scache_sync (int unit, _field_stage_t   *stage_fc,
                _field_entry_t   *f_ent,  _field_entry_oam_wb_t *f_ent_oam_wb);
extern int _field_extended_flex_counter_recover(int unit, _field_entry_t *f_ent,
                                       int part, bcm_field_stat_t sid,
                                       _field_entry_oam_wb_t *f_ent_oam_wb,
                                       bcm_field_stat_action_t action);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined BCM_APACHE_SUPPORT
extern void _field_apache_functions_init (_field_funct_t *functions);
extern int _field_apache_qualifiers_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_apache_qualify_OamDropReason(int unit,
                                bcm_field_entry_t entry, int qual_id,
                                bcm_field_oam_drop_mep_type_t mep_type);
extern int _bcm_field_apache_qualify_OamDropReason_get(int unit,
                                bcm_field_entry_t entry, int qual_id,
                                bcm_field_oam_drop_mep_type_t *mep_type);
extern int _bcm_field_apache_qualify_OamDropReason_delete(int unit,
                                bcm_field_entry_t entry, int qual_id);
extern int _bcm_field_apache_sat_macsa_add(int unit, bcm_mac_t mac,
                                uint32 class_id);
extern int _bcm_field_apache_sat_macsa_get(int unit, bcm_mac_t mac,
                                uint32 *class_id, int *idx);
extern int _bcm_field_apache_sat_macsa_delete(int unit, bcm_mac_t mac);
extern int _bcm_field_apache_sat_macsa_delete_all(int unit);
#if defined BCM_WARM_BOOT_SUPPORT
extern int _field_apache_entry_info_sync(int     unit,
                               _field_control_t  *fc,
                               _field_stage_t    *stage_fc,
                               _field_entry_t    *f_ent);
extern int _field_apache_entry_info_recovery(int      unit,
                               _field_control_t       *fc,
                               _field_stage_t         *stage_fc,
                               _field_entry_wb_info_t *f_ent_wb_info);
extern int _field_apache_actions_recovery(int         unit,
                               soc_mem_t              policy_mem,
                               uint32                 *policy_entry,
                               _field_entry_t         *f_ent,
                               _field_entry_wb_info_t *f_ent_wb_info);

#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_field_30bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter32_collect_t *result);
#endif /* BCM_TRIDENT_SUPPORT || BCM_TRIUMPH3_SUPPORT */

extern int _bcm_field_32bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter32_collect_t *result);
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_field_36bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter64_collect_t *result);
#endif /* BCM_BRADLEY_SUPPORT||BCM_TRIUMPH3_SUPPORT*/
#ifdef BCM_TRIDENT_SUPPORT
extern int _bcm_field_37bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter64_collect_t *result);
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
extern int _bcm_field_26bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter32_collect_t *result);
extern int _bcm_field_34bit_counter_update(int unit, uint32 *new_val,
                                           _field_counter64_collect_t *result);
#if defined (BCM_WARM_BOOT_SUPPORT)
extern int _bcm_field_th_stage_ingress_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_lookup_reinit(int unit,
                                             _field_control_t *fc,
                                             _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_egress_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_ingress_sync(int unit,
                                             _field_stage_t   *stage_fc);
extern int _bcm_field_th_stage_class_sync(int unit, _field_control_t *fc,
                                        _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_class_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_em_sync(int unit, _field_control_t *fc,
                                                _field_stage_t *stage_fc);
extern int _bcm_field_th_stage_em_reinit(int unit, _field_control_t *fc,
                                                  _field_stage_t *stage_fc);
extern int _bcm_field_scache_th_pointer_init(int unit, _field_control_t  *fc);
extern int _bcm_field_scache_th_pointer_realloc(int unit,
                                                _field_control_t  *fc);
extern void _bcm_field_th_tlv_recovery_map_free(int unit);
extern int _bcm_field_action_val_get(int unit,
                                     _field_entry_t *f_ent,
                                     uint32 *entbuf,
                                     _bcm_field_internal_action_t action,
                                     uint32 flags,
                                     _bcm_field_action_offset_t *a_offset);
#endif
#endif
extern int _bcm_field_entry_counter_move(int unit, _field_stage_t *stage_fc,
                                         uint8 old_slice, int old_hw_index,
                                         _field_stat_t *f_st_old,
                                         _field_stat_t *f_st);
extern int _bcm_field_qset_test(bcm_field_qualify_t qid,
                                bcm_field_qset_t *qset, uint8 *result);
extern int _bcm_field_entry_get_by_id(int unit, bcm_field_entry_t eid,
                                      _field_entry_t **f_ent);
extern int _field_qual_tcam_key_mask_get(int unit, _field_entry_t *f_ent,
                                         _field_tcam_t *tcam, int ipbm_overlay);
extern int _bcm_field_qual_tcam_key_mask_get(int unit, _field_entry_t *f_ent);
extern int _bcm_field_qual_list_append(_field_group_t *fg, uint8 entry_part,
                                     _qual_info_t *fq_src, int offset);
extern int _bcm_field_qual_value_set(int unit, _bcm_field_qual_offset_t *q_offset,
                                     _field_entry_t *f_ent,
                                     uint32 *p_data, uint32 *p_mask);
extern int _bcm_field_qual_value_set_by_entry_type(int unit,
                                             _bcm_field_qual_offset_t *q_offset,
                                             _field_entry_t *f_ent,
                                             uint32 *p_data,
                                             uint32 *p_mask,
                                             uint8 idx);
extern int _bcm_field_qual_value_get(int unit,
                          _bcm_field_qual_offset_t *q_offset,
                          _field_entry_t *f_ent, _bcm_field_qual_data_t p_data,
                          _bcm_field_qual_data_t p_mask);
extern int _bcm_field_qual_partial_data_get(uint32 *in_data, uint8 start_pos,
                                            uint16 len, uint32 *out_data);
extern int _bcm_field_qual_partial_data_set(uint32 *in_data, uint8 start_pos,
                                            uint16 len, uint32 *out_data);
extern int _bcm_field_stages_supported_qset_init(int unit, _field_control_t *fc);
extern int _bcm_field_group_qualifiers_free(_field_group_t *fg, int idx);
extern int _bcm_field_group_default_entry_set(int unit, bcm_field_group_t group,
                                              bcm_field_entry_t entry, int enable);
extern int _bcm_field_group_default_entry_get(int unit, bcm_field_group_t group,
                                              bcm_field_entry_t *entry);
extern int _bcm_field_qual_insert (int unit, _field_stage_t *stage_fc,
                                   int qual_type, int qual_id,
                                   _bcm_field_qual_conf_t *ptr);
extern int _bcm_field_tcam_part_to_slice_number(int unit, int entry_part,
                                                _field_group_t *fg,
                                                uint8 *slice_number);
extern int _bcm_field_group_status_init(int unit,
                                        bcm_field_group_status_t *entry);
extern int _bcm_field_group_status_calc(int unit, _field_group_t *fg);
extern int _bcm_field_stage_entries_free(int unit,
                                         _field_stage_t *stage_fc);
extern int _bcm_field_selcode_get(int unit, _field_stage_t *stage_fc,
                                      bcm_field_qset_t *qset_req,
                                      _field_group_t *fg);

extern int _field_selcode_assign(int unit, bcm_field_qset_t qset,
                                 int selcode_clear,_field_group_t *fg);
extern int
_field_entry_qualifier_key_get(int unit,
                               bcm_field_entry_t entry,
                               int qual_id,
                               _bcm_field_qual_data_t q_data,
                               _bcm_field_qual_data_t q_mask);

extern void _bcm_field_qual_conf_t_init(_bcm_field_qual_conf_t *ptr);
extern void _bcm_field_qual_info_t_init(_bcm_field_qual_info_t *ptr);
extern int _bcm_field_selcode_to_qset(int unit, _field_stage_t *stage_fc,
                                          _field_group_t *fg, int code_id,
                                          bcm_field_qset_t *qset);
extern int _bcm_field_qual_lists_get(int unit, _field_stage_t *stage_fc,
                                         _field_group_t *fg);
extern int _bcm_field_entry_part_tcam_idx_get(int unit, _field_entry_t *f_ent,
                                              uint32 idx_pri, uint8 ent_part,
                                              int *idx_out);
extern int _bcm_field_data_qualifier_get(int unit, _field_stage_t *stage_fc,
                                         int qid,
                                         _field_data_qualifier_t **data_qual);
extern int _bcm_field_data_qualifier_alloc(int unit,
                                           _field_data_qualifier_t **qual);
extern int _bcm_field_data_qualifier_free(int unit,
                                          _field_data_qualifier_t *qual);
extern int _bcm_field_entry_qualifier_uint32_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 int qual_id, uint32 *data,
                                                 uint32 *mask);
extern int _bcm_field_entry_qualifier_uint64_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 int qual_id, uint64 *data,
                                                 uint64 *mask);
extern int _bcm_field_entry_qualifier_uint16_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 int qual_id, uint16 *data,
                                                 uint16 *mask);
extern int _bcm_field_entry_qualifier_uint8_get(int unit,
                                                bcm_field_entry_t entry,
                                                int qual_id, uint8 *data,
                                                uint8 *mask);
extern int _bcm_field_qual_tcam_key_mask_free(int unit, _field_entry_t *f_ent);
extern int _bcm_field_action_dest_check(int unit, _field_action_t *fa);
extern int _bcm_field_policy_set_l3_nh_resolve(int unit,  int value,
                                               uint32 *flags, int *nh_ecmp_id);
extern int _bcm_field_virtual_map_size_get(int unit, _field_stage_t *stage_fc,
                                           int *size);
extern int _bcm_field_entry_tcam_idx_get(int unit,_field_entry_t *f_ent,
                                         int *tcam_idx);
extern int _bcm_field_tcam_idx_to_slice_offset(int unit,
                                             _field_stage_t *stage_fc,
                                             int instance, int tcam_idx,
                                             int *slice, int *offset);
extern int _bcm_field_slice_offset_to_tcam_idx(int unit,
                                               _field_stage_t *stage_fc,
                                               int instance, int slice,
                                               int offset, int *tcam_idx);
extern int _field_action_alloc(int unit, bcm_field_action_t action,
                               uint32 param0, uint32 param1,
                               uint32 param2, uint32 param3,
                               uint32 param4, uint32 param5,
                               _field_action_t **fa);

extern void _bcm_field_action_conf_t_init(_bcm_field_action_conf_t *ptr);
extern int _bcm_field_action_insert(int unit, _field_stage_t *stage_fc,
                              int action, _bcm_field_action_offset_t *ptr);

extern int _bcm_field_th_action_set(int unit, soc_mem_t mem,
                                    _field_entry_t *f_ent, int tcam_idx,
                                    _field_action_t *fa, uint32 *entbuf);
extern int _field_th_ingress_actions_init(int unit, _field_stage_t *stage_fc);

extern int
_field_qualify_VlanFormat_get(int                 unit,
                              bcm_field_entry_t   entry,
                              bcm_field_qualify_t qual_id,
                              uint8               *data,
                              uint8               *mask
                              );
extern int
_field_data_qualifier_get(int unit, int qual_id,
                           _field_data_qualifier_t *data_qual);
extern int
_field_data_qualifier_id_alloc(int unit, _field_stage_t *stage_fc,
                               bcm_field_data_qualifier_t *data_qualifier);
extern int
_field_data_qualifier_init(int                        unit,
                           _field_stage_t             *stage_fc,
                           _field_data_qualifier_t    *f_dq,
                           bcm_field_data_qualifier_t *data_qualifier
                           );
extern int
_field_data_qualifier_init2(int                        unit,
                            _field_stage_t             *stage_fc,
                            _field_data_qualifier_t    *f_dq
                            );
extern int
_field_data_qual_recover(int              unit,
                         _field_control_t *fc,
                         _field_stage_t   *stage_fc
                         );
extern void
_field_qset_udf_bmap_reinit(int              unit,
                            _field_stage_t   *stage_fc,
                            bcm_field_qset_t *qset,
                            int              qual_id
                            );
extern int
_field_trx2_udf_tcam_entry_l3_parse(int unit, uint32 *hw_buf,
                                    bcm_field_data_packet_format_t *pkt_fmt);

extern int
_field_trx2_udf_tcam_entry_flags_parse(int unit, uint32 *hw_buf,
                                       uint32 *flags);

extern int
_field_trx2_udf_tcam_entry_misc_parse(int unit, uint32 *hw_buf,
                                    bcm_field_data_packet_format_t *pkt_fmt);
extern int
_field_trx2_udf_tcam_entry_vlanformat_parse(int unit, uint32 *hw_buf,
                                    uint16 *vlanformat);
extern int
_field_trx2_udf_tcam_entry_l2format_parse(int unit, uint32 *hw_buf,
                                    uint16 *l2format);

extern void
_field_scache_sync_data_quals_write(_field_control_t *fc, _field_data_control_t *data_ctrl);

extern int
_field_counter_mem_get(int unit, _field_stage_t *stage_fc,
                       soc_mem_t *counter_x_mem, soc_mem_t *counter_y_mem);

extern int
_bcm_field_counter32_collect_alloc(int unit, soc_mem_t mem, char *descr,
                                   _field_counter32_collect_t **ptr);
extern int
_bcm_field_counter_collect_deinit(int unit, _field_stage_t *stage_fc);

extern int _bcm_field_policer_mode_support(int              unit,
                                           _field_entry_t   *f_ent,
                                           int              level,
                                           _field_policer_t *f_pl
                                           );
extern int _field_stat_value_set(int unit,
                                 _field_stat_t *f_st,
                                 bcm_field_stat_t stat,
                                 uint64 value);

extern int _field_stat_value_get(int unit,
                                 int sync_mode,
                                 _field_stat_t *f_st,
                                 bcm_field_stat_t stat,
                                 uint64 *value);
extern int _field_flex_counter_free(int unit, _field_entry_t *f_ent,
                                                    _field_stat_t *f_st);
extern int _field_group_default_aset_set(int unit,
                                         _field_group_t *fg);
extern int _bcm_field_trx_egress_mode_get(int unit, _field_group_t *fg,
                                          uint8 *slice_mode);
extern int _field_group_slice_vmap_recovery(int unit,
                                        _field_control_t *fc,
                                        _field_stage_t *stage_fc);

extern int _field_slice_map_write(int unit,
                                        _field_control_t *fc,
                                        _field_stage_t *stage_fc);

extern int _bcm_field_is_stage_range_check_exist (int unit,
        _field_stage_id_t stage);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
extern int _bcm_field_egress_key_attempt(int unit, _field_stage_t *stage_fc,
                              bcm_field_qset_t *qset_req,
                              uint8 key_pri, uint8 key_sec,
                              _field_group_t *fg);
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
extern int _bcm_field_35bit_counter_update(int unit, uint32 *new_val,
                                    _field_counter64_collect_t *result);
extern int _field_trx_action_copy_to_cpu(int unit, soc_mem_t mem,
                                         _field_entry_t *f_ent,
                                         _field_action_t *fa, uint32 *buf);
extern int
_bcm_field_counter64_collect_alloc(int unit, soc_mem_t mem, char *descr,
                                   _field_counter64_collect_t **ptr);

#endif /* BCM_TRX_SUPPORT */

extern int _bcm_esw_field_qset_udf_offsets_alloc(int unit,
    _field_stage_id_t stage, bcm_field_qset_t qset,
    int req_offsets, int offset_array[], int *max_chunks,
    int pipe_num);

extern int _bcm_esw_field_qual_udf_offsets_get(int unit, int qid,  _field_group_t *fg,
                                               int chunk_order[], int *num_chunks,
                                               int pipe_num);

#if defined BCM_TRIDENT2_SUPPORT
extern int _field_td2_group_install(int unit, _field_group_t *fg);
extern int _field_td2_selcodes_install(int unit, _field_group_t *fg,
                           uint8 slice_num, bcm_pbmp_t pbmp, int selcode_idx);
extern int _field_td2_slice_clear(int unit, _field_group_t *fg,
                                              _field_slice_t *fs);
extern int _field_td2_entry_move(int unit, _field_entry_t *f_ent,
                      int parts_count, int *tcam_idx_old, int *tcam_idx_new);
extern int _field_td2_action_support_check(int unit, _field_entry_t *f_ent,
                                    bcm_field_action_t action, int *result);
extern int _bcm_field_td2_action_conflict_check (int unit, _field_entry_t *f_ent,
                             bcm_field_action_t action1, bcm_field_action_t action);
extern int _field_td2_action_params_check (int unit, _field_entry_t *f_ent,
                                                  _field_action_t *fa);
extern int _field_td2_selcode_get(int unit, _field_stage_t *stage_fc,
                                     bcm_field_qset_t *qset_req,
                                     _field_group_t *fg);

extern int _bcm_field_td2_hash_select_profile_alloc_reserve_entry(int unit,
                                                 _field_stage_t *stage_fc);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#define _FIELD_IFP_DATA_START 0xDEADBEE0
#define _FIELD_IFP_DATA_END   0XDEADBEEF
#define _FIELD_EFP_DATA_START 0xDEADBEE1
#define _FIELD_EFP_DATA_END   0XDEADBEEE
#define _FIELD_VFP_DATA_START 0xDEADBEE2
#define _FIELD_VFP_DATA_END   0XDEADBEED
#define _FIELD_EXTFP_DATA_START 0xDEADBEE3
#define _FIELD_EXTFP_DATA_END   0XDEADBEEC
#define _FIELD_CLASS_DATA_START 0xDEADBEE4
#define _FIELD_CLASS_DATA_END   0xDEADBEED
#define _FIELD_EM_DATA_START    0xDEADBEE5
#define _FIELD_EM_DATA_END      0xDEADBEEA
#define _FIELD_FT_DATA_START    0xDEADBEE6
#define _FIELD_FT_DATA_END      0xDEADBEE9
#define _FIELD_AMFTFP_DATA_START  0xDEADBED0
#define _FIELD_AMFTFP_DATA_END    0xDEADBEDF
#define _FIELD_AEFTFP_DATA_START  0xDEADBED1
#define _FIELD_AEFTFP_DATA_END    0xDEADBEDE
#define _FIELD_SCACHE_PART_0    0x0
#define _FIELD_SCACHE_PART_1    0x1

#define _FIELD_GROUP_NEXT_GROUP_VALID (0x1 << 0) /* Next group is valid
                                                  * in scache
                                                  */
#define _FIELD_GROUP_LOOKUP_ENABLED   (0x1 << 5) /* Indicated group
                                                  * is lookup enabled
                                                  * in scache
                                                  */
#define _FIELD_GROUP_WLAN             (0x1 << 6) /* Indicated group
                                                  * is WLAN Group
                                                  * in scache
                                                  */
#define _FIELD_GROUP_AUTO_EXPANSION   (0x1 << 7) /* Indicates group can be
                                                  * expanded to multiple slices
                                                  */

#define _FIELD_QSET_ITER(qset, q) \
        for ((q) = 0; (q) < (int)bcmFieldQualifyCount; (q)++) \
            if (BCM_FIELD_QSET_TEST((qset), (q)))
#define _FIELD_QSET_INCL_INTERNAL_ITER(qset, q) \
        for ((q) = 0; (q) < (int)_bcmFieldQualifyCount; (q)++) \
            if (BCM_FIELD_QSET_TEST((qset), (q)))

#define _FIELD_WB_ENTRY_ACTION_OAM_STATOBJECT_SESSION_ID  (0x1 << 0) /* Indicates OamStatObjectSessionId */
#define _FIELD_WB_ENTRY_ACTION_COPYTOCPU                  (0x1 << 1) /* Indicates ActionCopyToCpu */
#define _FIELD_WB_ENTRY_ACTION_REDIRECT                   (0x1 << 2) /* Indicates ActionRedirect */
#define _FIELD_WB_ENTRY_ACTION_REDIRECT_TRUNK             (0x1 << 3) /* Indicates ActionRedirect
                                                                        as trunkid instead of gport */
#define _FIELD_WB_ENTRY_ACTION_REDIRECT_SOURCE_GPORT      (0x1 << 4) /* Indicates ActionRedirect
                                                                        source gport type present */

extern int _field_table_read(int unit, soc_mem_t mem, char **buffer_p,
    const char *buffer_name_p);
extern void _bcm_field_last_alloc_eid_incr(int unit);
extern int _bcm_field_last_alloc_eid_get(int unit);
extern int _field_slice_expanded_status_get(int unit, _field_control_t *fc, _field_stage_t *stage_fc,
                                 int *expanded, int *slice_master_idx);
extern int _mask_is_set(int unit, _bcm_field_qual_offset_t *offset, uint32 *buf,
             _field_stage_id_t stage_id);
extern int _field_physical_to_virtual(int unit, int instance, int slice_idx,
                                      _field_stage_t *stage_fc);
extern int _bcm_esw_field_scache_sync(int unit);
extern int _bcm_esw_field_tr2_ext_scache_size(int       unit,
                                              unsigned  part_idx
                                              );
extern int _field_trx_redirect_profile_get(int unit, soc_profile_mem_t
                                           **redirect_profile);
extern unsigned _field_trx_ext_scache_size(int       unit,
                                                  unsigned  part_idx,
                                                  soc_mem_t *mems
                                                  );
extern int _field_group_info_retrieve(int               unit,
                                      bcm_port_t        port,
                                      int               instance,
                                      bcm_field_group_t *gid,
                                      int               *priority,
                                      int               *action_res_id,
                                      bcm_pbmp_t        *group_pbmp,
                                      int8              *efp_key_info,
                                      int               *group_flags,
                                      bcm_field_qset_t  *qset,
                                      _field_control_t  *fc
                                      );
extern int _field_scache_slice_group_recover(int unit, _field_control_t *fc,
    int slice_num, int *multigroup);
extern int _field_entry_info_retrieve(int unit, _field_control_t *fc, _field_stage_t *stage_fc,
        int multigroup, int *prev_prio, _field_entry_wb_info_t *f_ent_wb_info);
extern void _field_scache_slice_group_free(int unit, _field_control_t *fc,
    int slice_num);
extern void _field_scache_stage_hdr_save(_field_control_t *fc,
                                         uint32           header
                                         );
extern int _field_scache_stage_hdr_chk(_field_control_t *fc,
                                       uint32           header
                                       );
extern int _field_range_check_reinit(int unit, _field_stage_t *stage_fc,
                                     _field_control_t *fc);

extern int _field_table_pointers_init(int unit,
                                      _field_table_pointers_t *field_tables);

#ifdef BCM_FIREBOLT_SUPPORT
extern int
_field_fb_meter_recover(int             unit,
                        _field_entry_t  *f_ent,
                        _meter_config_t *meter_conf,
                        int             part,
                        bcm_policer_t   pid
                        );
extern int _field_fb_stage_reinit(int unit, _field_control_t *fc,
    _field_stage_t *stage_fc);
extern int _field_scache_sync(int unit, _field_control_t *fc,
        _field_stage_t *stage_fc);
extern int _field_fb_slice_is_primary(int unit, int slice_index,
    int *is_primary_p, int *slice_mode_p);
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT)
extern int _field_fb2_stage_ingress_reinit(int unit, _field_control_t *fc,
                                           _field_stage_t *stage_fc);
extern int _field_fb2_stage_egress_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _field_fb2_stage_lookup_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
#endif
#if defined(BCM_RAVEN_SUPPORT)
extern int _field_raven_stage_reinit(int unit, _field_control_t *fc,
                                     _field_stage_t *stage_fc);
#endif


#if defined(BCM_TRX_SUPPORT)
extern int _field_tr2_stage_ingress_reinit(int unit, _field_control_t *fc,
                                           _field_stage_t *stage_fc);
extern int _field_tr2_stage_egress_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _field_tr2_stage_lookup_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
extern int _field_tr2_stage_external_reinit(int unit, _field_control_t *fc,
                                            _field_stage_t *stage_fc);
extern int _field_tr2_scache_sync(int unit, _field_control_t *fc,
                                  _field_stage_t *stage_fc);
extern int _field_tr2_ifp_slice_expanded_status_get(int unit, _field_control_t *fc,
                                                    _field_stage_t *stage_fc,
                                                    int *expanded, int *slice_master_idx);
extern int _bcm_field_trx_egress_secondary_selcodes_set(int unit,
                                                        _field_group_t *fg,
                                                        int slice_idx,
                                                        uint8 part_idx);
extern int _field_trx_scache_slice_group_recover(int unit,
                                                 _field_control_t *fc,
                                                 int instance,
                                                 int slice_num,
                                                 int *multigroup,
                                                 _field_stage_t *stage_fc,
                                                 int recovered_count);
extern int _field_tr2_slice_key_control_entry_recover(int unit,
                                                      unsigned slice_num,
                                                      _field_sel_t *sel);
extern int _field_tr2_group_construct_alloc(int unit, _field_group_t **pfg);
extern void _field_tr2_ingress_entity_get(int unit,
                                          int slice_idx,
                                          uint32 *fp_tcam_buf,
                                          int slice_ent_cnt,
                                          _field_stage_t *stage_fc,
                                          int8 *ingress_entity_sel);
extern int _field_tr2_group_construct(int unit, int inst,
                                      _field_hw_qual_info_t *hw_sel,
                                      int intraslice,
                                      int paired,
                                      _field_control_t *fc,
                                      bcm_port_t port,
                                     _field_stage_id_t stage_id,
                                     int slice_idx);
extern int _field_tr2_group_construct_quals_with_sel_update(int unit,
                                      _field_control_t *fc,
                                      _field_stage_t   *stage_fc);
extern int _field_trx_entry_info_retrieve(int unit,
                               _field_control_t  *fc,
                               _field_stage_t    *stage_fc,
                               int               multigroup,
                               int               *prev_prio,
                               _field_entry_wb_info_t *f_ent_wb_info);
extern int _field_tr2_actions_recover(int unit,
                                      soc_mem_t policy_mem,
                                      uint32 *policy_entry,
                                      _field_entry_t *f_ent,
                                      int part,
                                      _field_entry_wb_info_t *f_ent_wb_info);
extern int
_field_trx_group_construct_quals_add(int unit,
                              _field_control_t *fc,
                              _field_group_t   *fg,
                              uint8            fg_entry_type,
                              bcm_field_qset_t *new_qset,
                              int8             pri_key1,
                              int8             pri_key2);
extern int
_field_trx_actions_recover_action_add(int                unit,
                                      _field_entry_t     *f_ent,
                                      bcm_field_action_t action,
                                      uint32 param0, uint32 param1,
                                      uint32 param2, uint32 param3,
                                      uint32 param4, uint32 param5,
                                      uint32 hw_index);
extern int _field_tr2_stage_reinit_all_groups_cleanup(int unit,
                                                      _field_control_t *fc,
                                                      unsigned stage_id,
                                _field_table_pointers_t *fp_table_pointers);
extern int _field_tr2_group_entry_write(int unit, int instance, int slice_idx,
                                        _field_slice_t *fs,
                                        _field_control_t *fc,
                                        _field_stage_t *stage_fc);

extern int _bcm_field_trx_meter_rate_burst_recover(uint32 unit,
                                    uint32 meter_table,
                                    uint32 mem_idx,
                                    uint32   *prate,
                                    uint32   *pburst,
                                    uint32 *cfg_flags
                                    );
extern int _field_trx_meter_recover_configured_rates(int unit, _field_entry_t *f_ent,
        uint32 level, _field_entry_wb_info_t *f_ent_wb_info);
extern int _field_adv_flex_stat_info_retrieve(int unit, int stat_id);
extern int _field_flex_counter_recover(int unit, _field_entry_t *f_ent,
                                       int part, int sid);
extern int _field_entry_oam_wb_free (int unit, _field_entry_oam_wb_t **f_ent_oam_wb);
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
extern void _field_extract(uint32 *buf, int offset, int width, uint32 *value);
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_field_tr3_stage_lookup_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_tr3_stage_ingress_reinit(int unit,
                                               _field_control_t *fc,
                                               _field_stage_t   *stage_fc);
extern int _bcm_field_tr3_stage_egress_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_tr3_stage_external_reinit(int unit,
                                                _field_control_t *fc,
                                                _field_stage_t *stage_fc);
extern int _bcm_field_tr3_counter_recover(int unit,
                                          _field_entry_t *f_ent,
                                          uint32 *policy_entry,
                                          int part,
                                          bcm_field_stat_t sid);
extern int _bcm_field_tr3_scache_sync(int unit,
                                      _field_control_t *fc,
                                      _field_stage_t *stage_fc);
extern int _bcm_field_tr3_meter_recover(int unit,
                                        _field_entry_t *f_ent,
                                        int part,
                                        bcm_policer_t pid,
                                        uint32 level,
                                        soc_mem_t policy_mem,
                                        uint32 *policy_buf,
                                        uint8 *already_recovered
                                        );

extern int _field_tr3_actions_recover_replaceL2Fields(int unit,
                                      int                index,
                                      _field_entry_t     *f_ent
                                      );
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
extern int _bcm_field_td2_scache_sync(int unit,
                                      _field_control_t *fc,
                                      _field_stage_t *stage_fc);
extern int _bcm_field_td2_stage_lookup_reinit(int unit,
                                              _field_control_t *fc,
                                              _field_stage_t *stage_fc);
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
extern int _field_kt2_stage_lookup_reinit(int unit, _field_control_t *fc,
                                          _field_stage_t *stage_fc);
#endif /* BCM_KATANA2_SUPPORT */

extern int _field_entry_no_param_actions_scache_sync(int unit,
                                     _field_stage_t   *stage_fc,
                                     _field_control_t *fc,
                                     _field_entry_t   *f_ent,
                                     uint8 *scache_buf);

#if defined(BCM_HELIX5_SUPPORT)
extern int
_bcm_field_stage_ft_sync(int unit,
                    _field_control_t *fc,
                    _field_stage_t *stage_fc);

extern int
_bcm_field_stage_ft_reinit(int unit,
                    _field_control_t *fc,
                    _field_stage_t *stage_fc);
#endif /* BCM_HELIX5_SUPPORT */

#if defined(BCM_FIREBOLT6_SUPPORT)
extern int
_bcm_field_stage_amftfp_sync(int unit,
                    _field_control_t *fc,
                    _field_stage_t *stage_fc);

extern int
_bcm_field_stage_amftfp_reinit(int unit,
                    _field_control_t *fc,
                    _field_stage_t *stage_fc);

extern int
_bcm_field_stage_aeftfp_sync(int unit,
                    _field_control_t *fc,
                    _field_stage_t *stage_fc);

extern int
_bcm_field_stage_aeftfp_reinit(int unit,
                    _field_control_t *fc,
                    _field_stage_t *stage_fc);

#endif /* BCM_FIREBOLT6_SUPPORT */

#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm__field_tr3_action_replaceL2Fields(int unit,
                                                  _field_action_t *fa);
extern int _bcm__field_tr3_action_replaceL2Fields_remove_HWEntries( int unit,
                                      _field_entry_t *f_ent,
                                      _field_action_t *fa);
#endif

#ifdef BCM_FIREBOLT_SUPPORT
extern int _bcm_field_fb_slice_enable_set(int            unit,
                                          _field_group_t *fg,
                                          uint8          slice,
                                          int enable
                                          );
#endif

#if defined(INCLUDE_L3)
extern
int _bcm_field_l3_egr_object_ifp_type_index_get(int unit, bcm_if_t nh_index,
                                                int *ifp_type, uint32 *nh_flags,
                                                int *nh_ecmp_id);
extern int
_field_trx_policy_set_l3_info(int unit, soc_mem_t mem, int value, uint32 *buf);
#endif /* INCLUDE_L3 */

extern int
 _bcm_field_initialized_status_get(int unit, uint8 *status);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_field_cleanup(int unit);
#else
#define _bcm_field_cleanup(u)        (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

extern uint8 wb_write_disable;

extern void _field_qset_debug(bcm_field_qset_t qset);
extern void _field_qset_dump(char *prefix, bcm_field_qset_t qset, char* suffix);
#if defined(BCM_TOMAHAWK_SUPPORT)
extern void _field_aset_dump(char *prefix, _bcm_field_aset_t aset, char* suffix);
#else
extern void _field_aset_dump(char *prefix, bcm_field_aset_t aset, char* suffix);
#endif /* BCM_TOMAHAWK_SUPPORT */
extern int _field_group_dump_brief(int unit, bcm_field_group_t group);
extern int _field_dump_brief(int unit, const char *pfx);
extern int _field_dump_class(int unit, const char *pfx, int flag);
extern int _bcm_field_presel_dump(int unit, bcm_field_presel_t presel_id, int stage);
#if defined(BCM_TOMAHAWK_SUPPORT)
extern int
 _bcm_field_presel_qualifiers_dump(int unit, bcm_field_presel_t presel_id);
#endif /* BCM_TOMAHAWK_SUPPORT */
extern int _field_calc_cache_size(int unit);
extern char *_field_qual_name(bcm_field_qualify_t qid);

extern int _field_is_inited(int unit);
extern int _bcm_field_stage_qualifiers_free(int unit, _field_stage_t *stage_fc);

/* User-Defined Field (UDF) L2 Flags. (0 to 2 bits) */
#define _BCM_FIELD_USER_L2_MASK 0x00000003
#define _BCM_FIELD_USER_L2_ETHERNET2 (0 << 0)
#define _BCM_FIELD_USER_L2_SNAP (1 << 0)
#define _BCM_FIELD_USER_L2_LLC (2 << 0)
#define _BCM_FIELD_USER_L2_OTHER (3 << 0)

/* User-Defined Field (UDF) VLAN Flags. (3 to 4 bits) */
#define _BCM_FIELD_USER_VLAN_MASK 0x00000018
#define _BCM_FIELD_USER_VLAN_NOTAG (0 << 3)
#define _BCM_FIELD_USER_VLAN_ONETAG (1 << 3)
#define _BCM_FIELD_USER_VLAN_TWOTAG (2 << 3)
#define _BCM_FIELD_USER_VLAN_NOTUSED (3 << 3)

/* User-Defined Field (UDF) IP Flags. (6 to 10 bits) */
#define _BCM_FIELD_USER_IP_MASK 0x000007c0
#define _BCM_FIELD_USER_IP4_HDR_ONLY (0 << 6)
#define _BCM_FIELD_USER_IP6_HDR_ONLY (1 << 6)
#define _BCM_FIELD_USER_IP6_FRAGMENT (2 << 6)
#define _BCM_FIELD_USER_IP4_OVER_IP4 (3 << 6)
#define _BCM_FIELD_USER_IP6_OVER_IP4 (4 << 6)
#define _BCM_FIELD_USER_IP4_OVER_IP6 (5 << 6)
#define _BCM_FIELD_USER_IP6_OVER_IP6 (6 << 6)
#define _BCM_FIELD_USER_GRE_IP4_OVER_IP4 (7 << 6)
#define _BCM_FIELD_USER_GRE_IP6_OVER_IP4 (8 << 6)
#define _BCM_FIELD_USER_GRE_IP4_OVER_IP6 (9 << 6)
#define _BCM_FIELD_USER_GRE_IP6_OVER_IP6 (10 << 6)
#define _BCM_FIELD_USER_ONE_MPLS_LABEL (11 << 6)
#define _BCM_FIELD_USER_TWO_MPLS_LABELS (12 << 6)
#define _BCM_FIELD_USER_IP6_FRAGMENT_OVER_IP4 (13 << 6)
#define _BCM_FIELD_USER_IP_NOTUSED (14 << 6)
#define _BCM_FIELD_USER_ANY_MPLS_LABELS (15 << 6)
#define _BCM_FIELD_USER_ONE_FCOE_HEADER (16 << 6)
#define _BCM_FIELD_USER_TWO_FCOE_HEADER (17 << 6)

/* Flags for Auto Adjust Offset with IP options length. */
#define _BCM_FIELD_USER_OPTION_ADJUST (1 << 30)

/* Flags for Auto Adjust Offset with GRE header options length. */
#define _BCM_FIELD_USER_GRE_OPTION_ADJUST (1 << 29)

/* User-Defined Field (UDF) Miscellaneous Flags (13 to 21 bits) */
#define _BCM_FIELD_USER_F_ENCAP_STACK_ANY (0 << 13)
#define _BCM_FIELD_USER_F_NO_ENCAP_STACK  (1 << 13)
#define _BCM_FIELD_USER_F_ENCAP_STACK     (2 << 13)

#define _BCM_FIELD_USER_F_VNTAG_ANY      (0 << 15)
#define _BCM_FIELD_USER_F_NO_VNTAG       (1 << 15)
#define _BCM_FIELD_USER_F_VNTAG          (2 << 15)

#define _BCM_FIELD_USER_F_CNTAG_ANY      (0 << 17)
#define _BCM_FIELD_USER_F_NO_CNTAG       (1 << 17)
#define _BCM_FIELD_USER_F_CNTAG          (2 << 17)

#define _BCM_FIELD_USER_F_ETAG_ANY       (0 << 19)
#define _BCM_FIELD_USER_F_NO_ETAG        (1 << 19)
#define _BCM_FIELD_USER_F_ETAG           (2 << 19)

#define _BCM_FIELD_USER_F_ICNM_ANY       (0 << 21)
#define _BCM_FIELD_USER_F_NO_ICNM        (1 << 21)
#define _BCM_FIELD_USER_F_ICNM           (2 << 21)

#if defined(BCM_WARM_BOOT_SUPPORT)
typedef enum _bcmFieldActionNoParam_e {
    _bcmFieldActionNoParamPrioPktNoColor = 0,
    _bcmFieldActionNoParamPrioIntNoColor = 1,
    _bcmFieldActionNoParamCopyToCpuNoColor = 2,
    _bcmFieldActionNoParamDropNoColor = 3,
    _bcmFieldActionNoParamOamUpMep = 4,
    _bcmFieldActionNoParamOamTx = 5,
    _bcmFieldActionNoParamOamLmepEnable = 6,
    _bcmFieldActionNoParamOamLmEnable = 7,
    _bcmFieldActionNoParamOamDmEnable = 8,
    _bcmFieldActionNoParamOamLmepMdl = 9,
    _bcmFieldActionNoParamOamLmBasePtr = 10,
    _bcmFieldActionNoParamOamServicePriMappingPtr = 11,
    _bcmFieldActionNoParamOamPbbteLookupEnable = 12,
    _bcmFieldActionNoParamOamLmDmSampleEnable = 13,
    _bcmFieldActionNoParamCopyToCpuZeroParam1 = 14,
    _bcmFieldActionNoParamYpCopyToCpuZeroParam1 = 15,
    _bcmFieldActionNoParamRpCopyToCpuZeroParam1 = 16,
    _bcmFieldActionNoParamNewClassId = 17,
    _bcmFieldActionNoParamDscpNoColor = 18,
    _bcmFieldActionNoParamInnerVlanPriNewNoColor = 19,
    _bcmFieldActionNoParamOuterVlanPriNewNoColor = 20,
    _bcmFieldActionNoParamInnerVlanCfiNewNoColor = 21,
    _bcmFieldActionNoParamOuterVlanCfiNewNoColor = 22,
    _bcmFieldActionNoParamEcnNewNoColor = 23,
    _bcmFieldActionNoParamDropPrecedenceNoColor = 24,
    _bcmFieldActionNoParamGpDropPrecedence = 25,
    _bcmFieldActionNoParamYpDropPrecedence = 26,
    _bcmFieldActionNoParamRpDropPrecedence = 27,
    _bcmFieldActionNoParamIngressGportSet = 28,
    _bcmFieldActionNoParamRedirectIpmcWlan = 29,
    _bcmFieldActionNoParamRedirectIpmcVpls = 30,
    _bcmFieldActionNoParamRedirectIpmcL3 = 31,
    _bcmFieldActionNoParamIncomingMplsPortSet = 32,
    _bcmFieldActionNoParamPortPrioIntCosQNew = 33,
    _bcmFieldActionNoParamRpPortPrioIntCosQNew = 34,
    _bcmFieldActionNoParamYpPortPrioIntCosQNew = 35,
    _bcmFieldActionNoParamGpPortPrioIntCosQNew = 36,
    _bcmFieldActionNoParamMirrorIngress = 37,
    _bcmFieldActionNoParamMirrorEgress = 38,
    _bcmFieldActionNoParamRedirDropPrecedenceNoColor = 39,
    _bcmFieldActionNoParamFabricQueue = 40,
    _bcmFieldActionNoParamMirrorIngress1 = 41, /* Each enty can have atmost 4 IngressMirror Actions per entry */
    _bcmFieldActionNoParamMirrorIngress2 = 42,
    _bcmFieldActionNoParamMirrorIngress3 = 43,
    _bcmFieldActionNoParamMirrorEgress1 = 44, /* Each enty can have atmost 4 EngressMirror Actions per entry */
    _bcmFieldActionNoParamMirrorEgress2 = 45,
    _bcmFieldActionNoParamMirrorEgress3 = 46,
    _bcmFieldActionNoParamEnableVlanCheck = 47,
    _bcmFieldActionNoParamHiGigIntPriNew = 48,
    _bcmFieldActionNoParamHiGigDropPrecedenceNew = 49,
    _bcmFieldActionNoParamRedirectIpmcVlan = 50,
    _bcmFieldActionNoParamRedirectMpls = 51,
    _bcmFieldActionNoParamRedirectMim = 52,
    _bcmFieldActionNoParamRedirectNiv = 53,
    _bcmFieldActionNoParamRedirectTrill = 54,
    _bcmFieldActionNoParamRedirectL2Gre = 55,
    _bcmFieldActionNoParamRedirectVxLan = 56,
    _bcmFieldActionNoParamRedirectTrunk = 57,
    _bcmFieldActionNoParamMacSecSubPortEncrypt = 58,
    _bcmFieldActionNoParamMacSecSubPortDecrypt = 59,
    _bcmFieldActionNoParamSRNetIdSource = 60,
    _bcmFieldActionNoParamOamLmCounterPoolId = 61,
    _bcmFieldActionNoParamSvtagSignature = 62,
    _bcmFieldActionNoParamSvtagOffset = 63,
    _bcmFieldActionNoParamSvtagPrio = 64,
    _bcmFieldActionNoParamSvtagDeviceId = 65,
    _bcmFieldActionNoParamSvtagPktType = 66,
    _bcmFieldActionNoParamSvtagSubportNum = 67,
    _bcmFieldActionNoParamCount /* 68 */
} _bcmFieldActionNoParam_t;


typedef enum _bcmFieldGroupSliceSelectors_e {
    _bcmFieldGroupSliceSrcClassSel = 0,
    _bcmFieldGroupSliceDstClassSel = 1,
    _bcmFieldGroupSliceIntfClassSel = 2,
    _bcmFieldGroupSliceIngressEntitySel = 3,
    _bcmFieldGroupSliceSrcEntitySel = 4,
    _bcmFieldGroupSliceDstFwdEntitySel = 5,
    _bcmFieldGroupSliceFwdFieldSel = 6,
    _bcmFieldGroupSliceLoopbackTypeSel = 7,
    _bcmFieldGroupSliceIpHeaderSel = 8,
    _bcmFieldGroupSliceIp6AddrSel = 9,
    _bcmFieldGroupSliceAuxTag1Sel = 10,
    _bcmFieldGroupSliceAuxTag2Sel = 11,
    _bcmFieldGroupSliceNormalizeIpSel = 12,
    _bcmFieldGroupSliceNormalizeMacSel = 13,
    _bcmFieldGroupSliceTtlClassSelect= 14,
    _bcmFieldGroupSliceTosClassSelect= 15,
    _bcmFieldGroupSliceTcpClassSelect= 16,
    _bcmFieldGroupSliceCount = 17,
}_bcmFieldGroupSliceSelectors_t;

#define _BCM_FIELD_ACTION_NO_PARAM_STRINGS \
{ \
    "NoParamPrioPktNoColor" \
    "NoParamPrioIntNoColor", \
    "NoParamCopyToCpuNoColor", \
    "NoParamDropNoColor", \
    "NoParamOamUpMep", \
    "NoParamOamTx", \
    "NoParamOamLmepEnable", \
    "NoParamOamLmEnable", \
    "NoParamOamDmEnable", \
    "NoParamOamLmepMdl", \
    "NoParamOamLmBasePtr", \
    "NoParamOamServicePriMappingPtr", \
    "NoParamOamPbbteLookupEnable", \
    "NoParamOamLmDmSampleEnable", \
    "NoParamCopyToCpuZeroParam1", \
    "NoParamYpCopyToCpuZeroParam1", \
    "NoParamRpCopyToCpuZeroParam1", \
    "NoParamNewClassId", \
    "NoParamDscpNoColor", \
    "NoParamInnerVlanPriNewNoColor", \
    "NoParamOuterVlanPriNewNoColor", \
    "NoParamInnerVlanCfiNewNoColor", \
    "NoParamOuterVlanCfiNewNoColor", \
    "NoParamEcnNewNoColor", \
    "NoParamDropPrecedenceNoColor", \
    "NoParamGpDropPrecedence", \
    "NoParamYpDropPrecedence", \
    "NoParamRpDropPrecedence", \
    "NoParamIngressGportSet", \
    "NoParamRedirectIpmcWlan", \
    "NoParamRedirectIpmcVpls" \
    "NoParamRedirectIpmcL3" \
}

#define _FP_WB_END_OF_SLICES 0xAB

#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)


/*
 * Reserved Source Class ID to be SET along with CopyToCpu
 * action for Lookup/VFP stage.
 */
#define _BCM_FIELD_COPYTOCPU_WAR_RESERVED_CLASSID 0x3FF

/* Memory Types */
#define _BCM_FIELD_MEM_TYPE_IFP_LT    1
#define _BCM_FIELD_MEM_TYPE_EM_LT     2
#define _BCM_FIELD_MEM_TYPE_FT_LT     3
#define _BCM_FIELD_MEM_TYPE_FT_LT_I(_index_)      \
                (_BCM_FIELD_MEM_TYPE_FT_LT + ((_index_) << 24))

/* Memory View Type flags */
#define _BCM_FIELD_MEM_VIEW_TYPE_TCAM           1  /* TCAM view */
#define _BCM_FIELD_MEM_VIEW_TYPE_DATA           2  /* DATA view */
#define _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB 4  /* TCAM_DATA combined view */


/* ---- Macros/Enums/Structures related to Preselector Module ---- */

/* Max Preselector Entries Per Pipe */
#define _FP_FIELD_PRESEL_ENTRIES_PER_PIPE  31

/* Max Presel IDs possible value. */
#define _BCM_FIELD_PRESEL_ID_MAX BCM_FIELD_PRESEL_SEL_MAX

/* Validate the Presel ID range */
#define _BCM_FIELD_IS_PRESEL_VALID(_id)          \
            ((_id < 0 || _id >= _BCM_FIELD_PRESEL_ID_MAX) ? FALSE:TRUE)

/* Validate the Presel ID */
#define _BCM_FIELD_IS_PRESEL_ENTRY(_e)           \
         (((_e & 0xf0000000) == BCM_FIELD_QUALIFY_PRESEL) ? TRUE:FALSE)

/* Retrieve the Presel ID from the Entry ID */
#define _BCM_FIELD_PRESEL_FROM_ENTRY(_e)    \
            (_e & ~(BCM_FIELD_QUALIFY_PRESEL))

/*
 * Typedef:
 *  _field_class_cmp_info_s
 * Purpose:
 *  Field Class Compare information.
 */
typedef struct _field_class_cmp_info_s {
    _field_entry_t   *f_ent;
    uint32           new_entry_idx;
    uint32           cur_entry_idx;
    int              entry_prio;
} _field_class_cmp_info_t;

/* Enums related to Preselector Flags */
typedef enum _bcm_field_presel_flags_e {
   _bcmFieldPreselCreateWithId = 1,    /* Indicates Presel creation with id. */
   _bcmFieldPreselCount                /* Indicates Last entry - not in use. */
} _bcm_field_presel_flags_t;

/* Structure related to Preselector */
typedef struct _field_presel_info_s {
   uint32                  presel_limit;       /* max preselectors */
   uint32                  last_allocated_id;  /* Last allocated id */
   bcm_field_presel_set_t  presel_set;         /* set of preselectors created */
   bcm_field_presel_set_t  operational_set;    /* Indicates Operational status
                                                  of a presel IDs */
} _field_presel_info_t;

/* Add qualifier to Preselector stage qualifiers set. */
#define _FP_PRESEL_QUAL_INSERT(_unit_, _stage_fc_, _qual_id_,                 \
                     _flags_,                                                 \
                     _pri_slice_sel_, _pri_slice_sel_val_,                    \
                     _qual_type,                                              \
                     _num_offsets_,                                           \
                     _offset_0_, _width_0_,                                   \
                     _offset_1_, _width_1_)                                   \
            do {                                                              \
                _bcm_field_qual_conf_t_init(&_fp_qual_conf_);                 \
                (_fp_qual_conf_).offset.field           = (_key_fld_);        \
                (_fp_qual_conf_).offset.num_offsets     = _num_offsets_;      \
                (_fp_qual_conf_).offset.offset[0]       = _offset_0_;         \
                (_fp_qual_conf_).offset.width[0]        = _width_0_;          \
                (_fp_qual_conf_).offset.offset[1]       = _offset_1_;         \
                (_fp_qual_conf_).offset.width[1]        = _width_1_;          \
                (_fp_qual_conf_).offset.flags           = _flags_;            \
                _rv_ =_bcm_field_qual_insert((_unit_), (_stage_fc_),          \
                                             (_qual_type), (_qual_id_),       \
                                             &(_fp_qual_conf_));              \
                BCM_IF_ERROR_RETURN(_rv_);                                    \
            } while (0)


#define _FP_PRESEL_QUAL_ADD(_u_, _stage_fc_, _qual_id_,                       \
                     _flags_,                                                 \
                     _pri_slice_sel_, _pri_slice_sel_val_,                    \
                     _qual_type,                                              \
                     _offset_0_, _width_0_)                                   \
        _FP_PRESEL_QUAL_INSERT(_u_, _stage_fc_, _qual_id_,                    \
                     _flags_,                                                 \
                     _pri_slice_sel_, _pri_slice_sel_val_,                    \
                     _qual_type,                                              \
                     1,                                                       \
                     _offset_0_, _width_0_,                                   \
                     0, 0)                                                    \


#define _FP_PRESEL_QUAL_TWO_ADD(_u_, _stage_fc_, _qual_id_,                   \
                     _flags_,                                                 \
                     _pri_slice_sel_, _pri_slice_sel_val_,                    \
                     _qual_type,                                              \
                     _offset_0_, _width_0_,                                   \
                     _offset_1_, _width_1_)                                   \
        _FP_PRESEL_QUAL_INSERT(_u_, _stage_fc_, _qual_id_,                    \
                     _flags_,                                                 \
                     _pri_slice_sel_, _pri_slice_sel_val_,                    \
                     _qual_type,                                              \
                     2,                                                       \
                     _offset_0_, _width_0_,                                   \
                     _offset_1_, _width_1_)                                   \


/* Preselector TCAM Info */
typedef struct _field_presel_tcam_s {
    uint32 *key;        /* HW entry key replica.    */
    uint32 *mask;       /* HW entry mask replica.   */
    uint16 key_size;    /* HW entry Key size.       */
} _field_presel_tcam_t;


/* Preselector HW Data */
typedef struct _field_presel_data_s {
    uint32 *data;       /* HW entry data replica.   */
    uint16 data_size;   /* HW entry Data size.      */
} _field_presel_data_t;

/* Preselector Entry Structure */
typedef struct _field_presel_entry_s {
   bcm_field_presel_t     presel_id;    /* Unique Identifier */
   uint32                 flags;        /* Entry flags _FP_ENTRY_XXX */
   int                    priority;     /* Entry Priority */
   int                    hw_index;     /* Allocated hw resource index. */
   int                    slice_index;  /* Entry Presel slice index. */
   _field_presel_tcam_t   lt_tcam;      /* Fields to be written in LT TCAM */
   _field_presel_data_t   lt_data;      /* Fields to be written in LT DATA */
   bcm_field_qset_t       p_qset;       /* Contains Preselector Qualifiers
                                           associated with the presel entry */
   _field_action_t        *actions;     /* Reference to lined list of Actions
                                           associate with entry. */
   _field_group_t         *group;       /* Reference to the Group with this
                                           the entry is associated */
   _field_lt_slice_t      *lt_fs;       /* LT slice where entry lives. */
    bcm_pbmp_t            mixed_src_class_pbmp;          /* Ports in use by entry.        */

   struct _field_presel_entry_s *next;  /* Reference to next presel entry */
} _field_presel_entry_t;

extern int
_bcm_field_th_stage_preselector_init(int unit,
                                     _field_control_t *fc,
                                     _field_stage_t *stage_fc);
extern int
_field_th_lt_config_init(int unit, _field_control_t *fc);

extern int
_bcm_field_th_preselector_deinit(int unit,
                                 _field_control_t *fc,
                                 _field_stage_t *stage_fc);
extern int
_bcm_field_th_presel_create(int unit,
                            _bcm_field_presel_flags_t flags,
                            bcm_field_presel_t *presel);
extern int
_bcm_field_th_presel_destroy(int unit,
                             bcm_field_presel_t presel_id);
extern int
_bcm_field_presel_entry_get(int unit,
                            bcm_field_presel_t presel_id,
                            _field_presel_entry_t **presel);
extern int
_bcm_field_presel_qualify_set(int unit,
                              bcm_field_entry_t eid,
                              bcm_field_qualify_t qual,
                              uint32 *data,
                              uint32 *mask);
extern int
_bcm_field_th_lt_tcam_policy_mem_get(int unit, _field_stage_t *stage_fc,
                                     int instance, int mem_type, int view_type,
                                     soc_mem_t *tcam_mem, soc_mem_t *data_mem);
extern int
_bcm_field_presel_qualifier_get(int unit,
                                bcm_field_entry_t entry,
                                int qual_id,
                                _bcm_field_qual_data_t q_data,
                                _bcm_field_qual_data_t q_mask);
extern int
_bcm_field_presel_action_add(int unit, _field_control_t *fc,
                             bcm_field_entry_t entry, _field_action_t *fa);
extern int
_bcm_field_presel_action_delete(int unit, bcm_field_entry_t entry,
                                bcm_field_action_t action,
                                uint32 param0, uint32 param1);
extern int
_bcm_field_presel_action_get(int unit,
                             bcm_field_entry_t entry,
                             bcm_field_action_t action,
                             uint32 *param0,
                             uint32 *param1);
extern int
_bcm_field_presel_group_add(int unit, _field_group_t *fg,
                            bcm_field_presel_t presel);
extern int
_bcm_field_presel_entry_prio_set(int unit, bcm_field_entry_t entry, int prio);
extern int
_bcm_field_presel_entry_prio_get(int unit, bcm_field_entry_t entry, int *prio);
extern int
_bcm_field_presel_entry_tcam_idx_get(int unit, _field_presel_entry_t *f_presel,
                                     _field_lt_slice_t *lt_fs, int *tcam_idx);
extern int
_bcm_field_th_presel_tcam_key_mask_get(int unit,
                                   _field_stage_t *stage_fc,
                                   int inst,
                                   _field_presel_entry_t *f_presel,
                                   _field_presel_tcam_t  *tcam);
extern int
_bcm_field_th_presel_mem_data_get(int unit,
                                  _field_stage_t *stage_fc,
                                  int inst,
                                  _field_presel_entry_t *f_presel,
                                  _field_presel_data_t *p_data);
extern int
_bcm_field_presel_entry_actions_set(int unit, soc_mem_t data_mem,
                                    _field_presel_entry_t *f_presel,
                                    uint32 *buf);
extern int
_bcm_field_th_lt_entry_config_set(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, _field_lt_slice_t *lt_fs,
                                  _field_presel_entry_t *f_presel);
extern int
_field_th_lt_entry_data_config_set(int unit, _field_stage_t *stage_fc,
                                   _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_lt_entry_hw_install(int unit, soc_mem_t lt_tcam_mem, int tcam_idx,
                                  uint32 *key, uint32 *mask, uint32 *data,
                                  uint8 valid);
extern int
_bcm_field_th_lt_entry_parts_install(int unit, _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_lt_entry_install(int unit, _field_presel_entry_t *f_presel,
                               _field_lt_slice_t *lt_fs);
extern int
_bcm_field_th_group_keygen_profiles_index_alloc(int unit,
                                           _field_stage_t *stage_fc,
                                           _field_group_t *fg, int part_index);
extern int
_bcm_field_th_lt_entry_data_value_set(int unit, _field_stage_t *stage_fc,
                                      _field_group_t *fg, int index,
                                      soc_mem_t lt_data_mem, uint32 *data);
extern int
_bcm_field_th_lt_entry_part_tcam_idx_get(int unit, _field_group_t *fg,
                                         uint32 idx_pri, uint8 ent_part,
                                         int *idx_out);
extern int
_bcm_field_th_lt_entry_phys_destroy(int unit, _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_group_presel_set(int unit, bcm_field_group_t group,
                               bcm_field_presel_set_t *presel);
extern int
_bcm_field_th_group_presel_get(int unit, bcm_field_group_t group,
                               bcm_field_presel_set_t *presel);
extern int
_bcm_field_th_ingress_logical_table_map_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg);
extern int
_bcm_field_th_ingress_lt_action_prio_install(int unit,
                                             _field_stage_t *stage_fc,
                                             _field_group_t *fg);
extern int
_bcm_field_th_ingress_lt_partition_prio_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg, int lt_id,
                                              int slice);
extern int
_bcm_field_th_lt_part_prio_value_get(int unit, _field_group_t *fg,
                                     int *lt_part_prio);
extern int
_bcm_field_th_lt_part_prio_reset(int unit, _field_stage_t *stage_fc,
                                 _field_group_t *fg, _field_slice_t *fs);
extern int
_bcm_field_th_presel_entry_hw_remove(int unit, _field_presel_entry_t *f_presel);
extern int
_bcm_field_th_tcam_policy_mem_get(int unit,
                                  _field_entry_t *f_ent,
                                  soc_mem_t *tcam_mem,
                                  soc_mem_t *policy_mem);
extern int
_bcm_field_th_group_instance_set(int unit, _field_group_t *fg);
extern int
_bcm_field_th_tcam_policy_clear(int unit, _field_entry_t *f_ent, int tcam_idx);
extern int
_field_th_ext_code_assign(int unit,
                          int salcode_clear, _field_group_add_fsm_t *fsm_ptr);
typedef struct _field_counter_hw_mode_s {
    uint32 hw_bmap;
    uint8  hw_entry_count;
    _field_stat_color_t color;
} _field_counter_hw_mode_t;

extern _field_counter_hw_mode_t th_ingress_cntr_hw_mode_tbl[];
extern int th_ingress_cntr_hw_mode_tbl_size;
extern int _bcm_field_th_group_compress(int unit, _field_group_t *fg,
                                        _field_stage_t *stage_fc);
extern int _bcm_field_th_resync(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_th_class_qual_support_check(int unit, int qual);
extern int
_field_th_entry_action_profile_data_set(int unit,
                                   _field_entry_t *f_ent,
                                   uint32 *ap_idx,
                                   soc_mem_t policy_mem,
                                   uint32 *abuf);
extern int
_bcm_field_th_flex_stat_action_set(int unit,
                                   _field_entry_t *f_ent,
                                   soc_mem_t mem,
                                   int tcam_idx,
                                   uint32 *buf);
extern void
_bcm_field_th_functions_init(_field_funct_t *functions);
extern int
_bcm_field_th_entry_flags_to_tcam_part(int unit, uint32 entry_flags,
                                       uint32 group_flags,
                                       uint8 *entry_part);
extern int
_bcm_field_action_val_set(int unit, _field_entry_t *f_ent, uint32 *entbuf,
                                     _bcm_field_action_offset_t *a_offset);
extern int
_bcm_field_action_offset_get(int unit, _field_stage_t *stage_fc,
                            _bcm_field_internal_action_t action,
                                _bcm_field_action_offset_t *ptr,
                                                   uint32 flags);

extern int
_bcm_field_action_profile_hw_stat_fields_get(int unit, int index,
                                             soc_mem_t mem,
                                             uint32 *offset_mode,
                                             uint32 *pool_number,
                                             uint32 *base_idx);
extern int _field_th_class_max_entries(int unit,
                                   int cpipe,
                                   _field_class_type_t ctype,
                                   int *max_entries);
extern int
_field_th_em_entry_set(int unit,
                            _field_entry_t *f_ent,
                            soc_mem_t mem,
                            uint32 *bufp, int search_entry);
extern int
_field_td3_em_entry_set(int unit,
                            _field_entry_t *f_ent,
                            soc_mem_t mem,
                            uint32 *bufp, int search_entry);
extern int
_bcm_field_th_em_entry_tcam_idx_get(int unit,
                            _field_entry_t *f_ent, int *hw_index);
extern int
_field_th3_em_entry_set(int unit,
                            _field_entry_t *f_ent,
                            soc_mem_t mem,
                            uint32 *bufp, int search_entry);

#endif  /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
extern int
_bcm_field_hx5_ingress_lt_partition_prio_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg, int lt_id,
                                              int slice);
#endif  /* BCM_HELIX5_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
extern int _field_td3_ingress_action_profiles_init(int unit,
                                          _field_stage_t *stage_fc);

extern int _field_td3_common_actions_init(int unit,
                                          _field_stage_t *stage_fc);
extern int
_field_td3_ingress_entry_policy_mem_install(int unit, _field_entry_t *f_ent,
                                           soc_mem_t policy_mem, int tcam_idx);
extern int
_field_td3_field_group_aset_update(int unit, _field_group_t *fg,
                                             bcm_field_aset_t  *aset);
extern void _bcm_field_td3_functions_init(_field_funct_t *functions);
extern int _bcm_field_td3_action_get(int unit, soc_mem_t mem, _field_entry_t *f_ent,
                                     int tcam_idx, _field_action_t *fa, uint32 *buf);
extern int _bcm_field_td3_vlan_action_hw_alloc(int unit, _field_entry_t *f_ent);
extern int _bcm_field_td3_vlan_action_hw_free(int unit, _field_entry_t *f_ent, uint32 flags);
extern int _bcm_field_td3_vlan_action_profile_entry_get(int unit,
                                                 soc_mem_t policy_mem,
                                                 uint32 *policy_entry,
                                                 uint32 profile_index,
                                                 bcm_vlan_action_set_t *actions);
extern int _bcm_field_action_vlan_actions_add(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_field_action_t action,
                                   bcm_vlan_action_set_t *vlan_action_set);
extern int _bcm_field_action_vlan_actions_get(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_field_action_t action,
                                   bcm_vlan_action_set_t *vlan_action_set);
extern int _field_td3_stage_quals_ibus_map_init(int unit,
                                             _field_stage_t *stage_fc);
extern int
_field_td3_egress_qualifiers_init(int unit, _field_stage_t *stage_fc);

extern int
_field_td3_presel_qualifiers_init(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_td3_em_key_attributes_init(int unit,
                                      _field_stage_t *stage_fc);

extern int
_field_td3_egress_selcodes_install(int unit, _field_group_t *fg,
                                   uint8 slice_num, int selcode_idx);
extern int
_bcm_field_td3_egress_secondary_selcodes_set(int unit, _field_group_t *fg,
                                             int slice_idx, uint8 part_idx);
extern int
_bcm_field_td3_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask);
extern int
_bcm_field_td3_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type);
extern int
_bcm_field_td3_tpid_hw_encode(int unit, uint16 tpid, uint16 outer_tpid,
                                                     uint32 *hw_code);
extern int
_bcm_field_td3_tpid_hw_decode(int unit, uint32 hw_code, uint16 outer_tpid,
                                                     uint16 *tpid);
extern int
_bcm_field_td3_drop_vector_mask_init(int unit);
extern int
_bcm_field_td3_qualify_PktType(int unit, bcm_field_pkt_type_t type,
                               uint32     *tcam_data,
                               uint32     *tcam_mask);
extern int
_bcm_field_td3_qualify_PktType_get(int unit, uint32 tcam_data, uint32 tcam_mask,
                                   bcm_field_pkt_type_t *type);
extern int
_bcm_field_td3_qualify_IpOptionHdrType(int unit, bcm_field_qualify_t qual_id,
                                       bcm_field_ip_option_hdr_type_t type,
                                       uint8 *data, uint8 *mask);
extern int
_bcm_field_td3_qualify_IpOptionHdrType_get(int unit, bcm_field_qualify_t qual_id,
                                           uint8     data,
                                           uint8     mask,
                                           bcm_field_ip_option_hdr_type_t *type);
extern int
_bcm_field_td3_qualify_L2PktType(int unit, bcm_field_l2_pkt_type_t type,
                                 uint8 *data,
                                 uint8 *mask);
extern int
_bcm_field_td3_qualify_L2PktType_get(int unit, uint8 data, uint8 mask,
                                     bcm_field_l2_pkt_type_t *type);
extern int
_bcm_field_td3_qualify_NetworkTagType(int unit, bcm_field_network_tag_type_t type,
                                      uint8 *data, uint8 *mask);
extern int
_bcm_field_td3_qualify_NetworkTagType_get(int unit, uint8 data, uint8 mask,
                                          bcm_field_network_tag_type_t *type);
extern int
_bcm_field_td3_qualify_SysHdrType(int unit, bcm_field_sys_hdr_type_t type,
                                  uint8 *data, uint8 *mask);
extern int
_bcm_field_td3_qualify_SysHdrType_get(int unit, uint8 data, uint8 mask,
                                      bcm_field_sys_hdr_type_t *type);
extern int
_bcm_field_td3_qualify_TimeStampTxPktType(int unit,
                       bcm_field_timestamp_transmit_pkt_type_t ts_pkt_type,
                       uint16 *data,
                       uint16 *mask);
extern int
_bcm_field_td3_qualify_TimeStampTxPktType_get(int unit,
                       uint16 data,
                       uint16 mask,
                       bcm_field_timestamp_transmit_pkt_type_t *ts_pkt_type);
extern int
_bcm_field_td3_qualify_TunnelType(int unit, bcm_field_TunnelType_t tunnel_type,
                                  uint32 *tcam_data, uint32 *tcam_mask);
extern int
_bcm_field_td3_qualify_TunnelType_get(int unit, uint8 tcam_data, uint8 tcam_mask,
                                      bcm_field_TunnelType_t *tunnel_type);
extern int
_bcm_field_td3_qualify_LoopbackType(bcm_field_LoopbackType_t loopback_type,
                                    uint32                   *tcam_data,
                                    uint32                   *tcam_mask);
extern int
_bcm_field_td3_qualify_LoopbackType_get(uint8                    tcam_data,
                                        uint8                    tcam_mask,
                                        bcm_field_LoopbackType_t *loopback_type);
extern int
_bcm_field_td3_qualify_flow_type(int unit, int qual_id, uint8 data, uint8 mask,
                                 uint8 *tcam_data, uint8 *tcam_mask);
extern int
_bcm_field_td3_qualify_flow_type_get(int unit, int qual_id, uint8 tcam_data,
                                     uint8 tcam_mask, uint8 *data, uint8 *mask);
extern int
_bcm_field_td3_qualify_HiGigProxyTunnelType(bcm_field_higig_proxy_tunnel_type_t type,
                                            uint8 *tcam_data, uint8 *tcam_mask);
extern int
_bcm_field_td3_qualify_HiGigProxyTunnelType_get(uint8 tcam_data, uint8 tcam_mask,
                                       bcm_field_higig_proxy_tunnel_type_t *type);
extern int
_bcm_field_td3_qualify_InterfaceClassL2Type(bcm_field_interface_class_l2_type_t type,
                                            uint8 *tcam_data, uint8 *tcam_mask);
extern int
_bcm_field_td3_qualify_InterfaceClassL2Type_get(uint8 tcam_data, uint8 tcam_mask,
                                       bcm_field_interface_class_l2_type_t *type);
extern int
_bcm_field_td3_zone_match_id_partition_init(int unit,
                                            _field_stage_t *stage_fc);
extern int
_bcm_field_td3_internal_action_profile_data_set(int unit,
                                                _field_entry_t *f_ent,
                                                _field_action_t *fa,
                                                _field_em_action_set_t *acts_buf);
extern int
_field_td3_flex_action_set(int unit,
                           _field_action_t *fa,
                           _bcm_field_action_offset_t *a_offset);
extern int
_bcm_field_td3_qualify_LoopbackSubtype(int unit,
                                       bcm_field_loopback_subtype_t subtype,
                                       uint32  *hw_data, uint32 *hw_mask);
extern int
_bcm_field_td3_qualify_LoopbackSubtype_get(int unit,
                                           uint8                    tcam_data,
                                           uint8                    tcam_mask,
                                           bcm_field_loopback_subtype_t *subtype);
extern int
_bcm_field_td3_qualify_TunnelSubType(int unit, bcm_field_TunnelSubType_t type,
                                     uint32 *tcam_data, uint32 *tcam_mask);
extern int
_bcm_field_td3_qualify_TunnelSubType_get(int unit, uint8 tcam_data, uint8 tcam_mask,
                                         bcm_field_TunnelSubType_t *type);
extern int
_bcm_field_td3_qualify_VlanFormat(int unit,
                                  bcm_field_qualify_t qual_id,
                                  uint8 data,
                                  uint8 mask,
                                  uint8 *hw_data,
                                  uint8 *hw_mask);
extern int
_bcm_field_td3_qualify_VlanFormat_get(int unit,
                                  bcm_field_qualify_t qual_id,
                                  uint8 hw_data,
                                  uint8 hw_mask,
                                  uint8 *data,
                                  uint8 *mask);
extern int
_bcm_field_td3_qualify_MplsType(int unit, bcm_field_qualify_t qual_id,
                                uint8 data, uint8 mask,
                                uint8 *hw_data, uint8 *hw_mask);
extern int
_bcm_field_td3_qualify_MplsType_get(int unit, bcm_field_qualify_t qual_id,
                                    uint8 hw_data, uint8 hw_mask,
                                    uint8 *data, uint8 *mask);
extern int
_bcm_field_td3_flex_qual_ceh_db_init(int unit);
extern int
_bcm_field_td3_flex_qual_ceh_db_clear(int unit);
extern int
_bcm_field_td3_ifp_inports_entry_tcam_install(int unit, _field_entry_t *f_ent,
                                              int tcam_idx, uint8 set_pbmp,
                                              _field_pbmp_t *_f_pbmp,
                                              int num_pipe);
extern int
_bcm_field_td3_inports_flex_init(int unit);

#if defined (BCM_CHANNELIZED_SWITCHING_SUPPORT)
extern int
_bcm_field_td3_qual_EgressPortCtrlType_get(int unit,
                                           bcm_field_entry_t entry,
                                           uint32 data, uint32 mask,
                                           bcm_field_egress_port_ctrl_t *ctrl_type);

extern int
_bcm_field_td3_qual_EgressPortCtrlType_set(int unit,
                                           bcm_field_entry_t entry,
                                           bcm_field_egress_port_ctrl_t ctrl_type,
                                           uint32 *data, uint32 *mask);
extern int
_field_td3_flex_data_mask_set(int unit, _field_control_t *fc);
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined (BCM_TOMAHAWK3_SUPPORT)
extern int
_bcm_field_th3_em_key_attributes_init(int unit,
                                      _field_stage_t *stage_fc,
                                      _field_control_t *fc);
#endif /* BCM_TOMAHAWK3_SUPPORT */

extern int
_bcm_field_valid_pbmp_get(int unit, bcm_pbmp_t *pbm);


extern int
_bcm_esw_field_class_map_set(
                       int unit,
                       bcm_field_qualify_t qual,
                       uint32 num_entries,
                       uint32 *orig_values_array,
                       uint32 *map_values_array);

extern int
_bcm_esw_field_class_map_get(
                       int unit,
                       bcm_field_qualify_t qual,
                       uint32 num_entries,
                       uint32 *orig_values_array,
                       uint32 *map_values_array);
extern int
_bcm_esw_field_entry_dump(int unit, bcm_field_entry_t entry, int stage);
extern int
_bcm_esw_field_group_dump(int unit, bcm_field_group_t group, int stage);
extern int
_bcm_esw_field_show(int unit, const char *pfx, int stage);
extern int
_bcm_esw_field_range_show(int unit, const char *pfx, int stage);
extern int
_bcm_esw_field_udf_show(int unit, const char *pfx);
extern int
_bcm_esw_field_group_info_key_get(int unit,
                                  bcm_field_group_t group,
                                  int max_parts_count,
                                  _bcm_field_group_qual_t **qual_arr,
                                  int *num_parts_count);

extern int
_bcm_field_reg_instance_get(int unit, soc_reg_t reg,
                               int inst, soc_reg_t *reg_out);

extern int
_bcm_field_mem_instance_get(int unit, soc_mem_t mem,
                               int inst, soc_mem_t *mem_out);

#if defined (BCM_TOMAHAWK_SUPPORT)
extern int
_bcm_th_qual_hint_bmp_get(int unit, int qual,
                          uint8 em_keygen_mask,
                          _field_hint_t *hint_node,
                          uint32 *qual_bitmap,
                          uint8 partial,
                          bcm_field_qset_t *qset,
                          bcmi_keygen_qual_info_t *qual_info);
extern int
_bcm_field_th_class_ctype_supported(int unit,
                                 _field_class_type_t ctype);
extern int
_bcm_field_max_lt_part_prio_get(int unit);

extern int
_bcm_field_th_max_lt_parts_get(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
_bcm_field_wb_slice_index_bmp_write (int unit,
                                 _field_control_t *fc,
                                 _field_stage_t *stage_fc);
#endif /*BCM_WARM_BOOT_SUPPORT*/
extern int
_bcm_field_th_group_presel_enable_set(int unit,
                                      _field_stage_t *stage_fc,
                                      _field_group_t *fg,
                                      uint32 valid);
extern int
_bcm_field_th_slice_enable_set(int                unit,
                               _field_group_t     *fg,
                               _field_slice_t     *fs,
                                uint8             enable);
#endif /*BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_FLOWTRACKER_SUPPORT)
extern int
_bcm_field_hx5_qualify_PktDstAddrType(int unit,
                    bcm_field_qualify_t qual,
                    uint32 dst_addr_type,
                    uint32 *hw_data,
                    uint32 *hw_mask);
extern int
_bcm_field_hx5_qualify_PktDstAddrType_get(int unit,
                    bcm_field_qualify_t qual,
                    uint32 hw_data,
                    uint32 hw_mask,
                    uint32 *dst_addr_type);

extern int
_bcm_field_hx5_dosattack_event_qual_bitmap(int unit,
                    _field_hint_t *hint_node,
                    bcmi_keygen_qual_info_t *qual_info);

extern int
_bcm_field_hx5_ft_group_aset_update(int unit,
                    _field_group_t *fg,
                    bcm_field_aset_t *aset);

#endif  /* BCM_FLOWTRACKER_SUPPORT */

extern int _field_action_match_config_alloc (int unit,
               bcm_field_action_match_config_t *match_config,
               _field_action_t *fa);

extern int
_bcm_field_ddrop_profile_get(int unit,
                                 soc_profile_mem_t **ddrop_profile);
extern int
_bcm_field_ddrop_profile_ref_count_get(int unit,
                                       int index,
                                       int *ref_count);
extern int
_bcm_field_ddrop_profile_delete(int unit, int index);
extern int
_bcm_field_dredirect_profile_get(int unit,
                                 soc_profile_mem_t **dredirect_profile);
extern int
_bcm_field_dredirect_profile_ref_count_get(int unit,
                                              int index,
                                              int *ref_count);
extern int
_bcm_field_dredirect_profile_delete(int unit, int index);

extern int
_field_delayed_profile_hw_alloc(int unit, _field_entry_t *f_ent);

extern int
_field_delayed_profile_hw_free(int unit, _field_entry_t *f_ent, uint32 flags);

#if defined(BCM_HURRICANE4_SUPPORT)
#if defined(INCLUDE_XFLOW_MACSEC)
extern int
_bcm_hr4_field_qualify_MacSecFlow(
                int unit,
                bcm_field_entry_t entry_id,
                uint8 data,
                uint8 mask);
extern int
_bcm_hr4_field_qualify_MacSecFlow_get(
                int unit,
                bcm_field_entry_t entry_id,
                uint32 *data,
                uint32 *mask);
#endif /* INCLUDE_XFLOW_MACSEC */
#endif /* BCM_HURRICANE4_SUPPORT */

#if defined (BCM_TRIDENT3_SUPPORT)
extern int
_bcm_field_td3_qualify_VlanTranslationHit(int               unit,
                                          bcm_field_entry_t entry,
                                          uint8             *data,
                                          uint8             *mask);
extern int
_bcm_field_td3_em_udf_keygen_mask_get(int unit,
                                      _field_group_t *fg,
                                      uint16 qid, uint32 *mask);
extern int
_bcm_field_qual_cfg_info_db_update(int unit,
                                   int instance,
                                   bcm_field_qualify_t qual,   
                                   _field_stage_t *stage_fc);
extern int
_bcm_field_td3_field_bus_cont_sel_install(int unit,
                                          int instance,
                                          _field_stage_t *stage_fc,
                                          _field_group_t *fg,
                                          bcm_field_qualify_t qual);
extern int
_bcm_field_td3_field_bus_cont_sel_remove(int unit,
                                         int instance,
                                         _field_stage_t *stage_fc,
                                          _field_group_t *fg,
                                         bcm_field_qualify_t qual);
#endif /* BCM_TRIDENT3_SUPPORT */
extern int
_bcm_field_priority_group_get_next(int unit,
                                   int instance,
                                   _field_stage_id_t stage_id,
                                   int priority,
                                   uint32 group_mode,
                                   _field_group_t **curr_group,
                                   _field_group_t **next_group);
extern int
_bcm_field_group_count_get(int unit,
                           int instance,
                           _field_stage_t *stage_fc,
                           int group_priority,
                           uint32 group_mode,
                           uint16 *group_count);
extern int _bcm_field_mem_width_bmp_get(int unit, soc_mem_t mem, uint32 *width);

#if defined(BCM_FIREBOLT6_SUPPORT)
extern int
_field_fb6_aftfp_group_aset_set(int unit,
                    _field_group_add_fsm_t *fsm_ptr);
#endif /* BCM_FIREBOLT6_SUPPORT */

#if defined (BCM_TOMAHAWK_SUPPORT)
extern int
_bcm_field_em_entry_info_get(int unit, bcm_field_entry_t eid, 
                             soc_mem_t *mem, uint32* key,
                             int *key_size, int *hw_index);
#endif /* BCM_TOMAHAWK_SUPPORT */
#endif  /* BCM_FIELD_SUPPORT */
#endif  /* !_BCM_INT_FIELD_H */
