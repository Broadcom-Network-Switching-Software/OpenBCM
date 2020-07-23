/*! \file field_ha.h
 *
 * Field header file.
 * This file contains the HA management for Field module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_FIELD_HA_H
#define BCMI_LTSW_FIELD_HA_H

#include <shr/shr_bitop.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/generated/field_ha.h>

/******************************************************************************
 * Defines
 */
/*! Array size of HA Block IDs */
#define BCM_FIELD_HA_BLK_ID_ARRAY_SIZE 256

/*! Minimum valid block id for FP HA blocks. */
#define BCM_FIELD_HA_BLK_ID_MIN 0x1

/*! Maximum valid block id for FP HA blocks. */
#define BCM_FIELD_HA_BLK_ID_MAX 0xFE

/*! Invalid HA block ID. Used in HA block id validations. */
#define BCM_FIELD_HA_BLK_ID_INVALID 0x0

/*! Signature for Field HA blocks. */
#define BCM_FIELD_HA_STRUCT_SIGN 0xBABA0000

/*! Since block offset is uint32_t type the max offset is ((2^32) -1).
 *  This macros is used to identify invalid HA block offset.
 */
#define BCM_FIELD_HA_BLK_OFFSET_INVALID 0xFFFFFFFF

/*! Macro to get the HA block id reserved for a given stage_id. */
#define BCM_FIELD_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id)     \
    do {                                                      \
        if (stage_id == bcmiFieldStageIngress) {              \
           blk_id = BCM_FIELD_HA_BLK_ID_INGRESS;              \
        } else if (stage_id == bcmiFieldStageEgress) {        \
           blk_id = BCM_FIELD_HA_BLK_ID_EGRESS;               \
        } else if (stage_id == bcmiFieldStageVlan) {          \
           blk_id = BCM_FIELD_HA_BLK_ID_VLAN;                 \
        } else if (stage_id == bcmiFieldStageExactMatch) {    \
           blk_id = BCM_FIELD_HA_BLK_ID_EM;                   \
        } else if (stage_id == bcmiFieldStageFlowTracker) {   \
           blk_id = BCM_FIELD_HA_BLK_ID_FT;                   \
        } else {                                              \
           SHR_ERR_EXIT(SHR_E_INTERNAL);               \
        }                                                     \
    } while(0)

/*! Macro to validate the HA block Id. This will check whether
 *  HA block id is with in the min, max limits of valid block Ids.
 */
#define BCM_FIELD_HA_BLK_ID_VALIDATE(unit, blk_id)    \
    do {                                              \
        SHR_IF_ERR_EXIT(                              \
            (blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ? \
                   SHR_E_INTERNAL : SHR_E_NONE);      \
        SHR_IF_ERR_EXIT(                              \
            (blk_id > BCM_FIELD_HA_BLK_ID_MAX) ?      \
                  SHR_E_INTERNAL : SHR_E_NONE);       \
        SHR_IF_ERR_EXIT(                              \
            (blk_id < BCM_FIELD_HA_BLK_ID_MIN) ?      \
                  SHR_E_INTERNAL : SHR_E_NONE);       \
    } while(0)

/*! Macros to validate the HA block offset. */
#define BCM_FIELD_HA_BLK_OFFSET_VALIDATE(unit, blk_id, blk_offset) \
    do {                                                       \
        void *_ha_ptr = NULL;                                  \
        uint32_t _size = 0;                                    \
        bcmi_field_ha_blk_hdr_t *_blk_hdr = NULL;              \
        SHR_IF_ERR_EXIT(                                       \
            (blk_offset == BCM_FIELD_HA_BLK_OFFSET_INVALID) ?  \
                                SHR_E_INTERNAL: SHR_E_NONE);   \
        if (((blk_offset - sizeof(bcmi_field_ha_blk_hdr_t)) %  \
             sizeof(bcmi_field_ha_element_t)) != 0) {          \
            SHR_ERR_EXIT(SHR_E_INTERNAL);               \
        }                                                      \
        _ha_ptr = BCM_FIELD_HA_PTR(unit, blk_id);              \
        if (_ha_ptr == NULL) {                                 \
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);              \
        }                                                      \
        _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ha_ptr;         \
        SHR_IF_ERR_EXIT(                                       \
            (blk_offset > _blk_hdr->blk_size) ?                \
                     SHR_E_INTERNAL : SHR_E_NONE);             \
        _size = blk_offset + sizeof(bcmi_field_ha_element_t);  \
        SHR_IF_ERR_EXIT(                                       \
            (_size > _blk_hdr->blk_size) ?                     \
               SHR_E_INTERNAL : SHR_E_NONE);                   \
    } while(0)


/*! Reserved FP HA Root block Ids. */
#define BCMI_FIELD_HA_BLK_ID_ROOT  1

typedef struct ltsw_fp_ha_local_info_s {
    uint8_t last_allocated_blk_id;
    uint8_t blk_id[BCMI_FIELD_HA_BLK_TYPE_LAST_COUNT];
    void *blk_hdrs[BCM_FIELD_HA_BLK_ID_MAX];
} ltsw_fp_ha_local_info_t;

extern ltsw_fp_ha_local_info_t *ltsw_fp_ha_info[BCM_MAX_NUM_UNITS];

#define FP_HA_INFO(_u_)                                             \
        ltsw_fp_ha_info[_u_]

#define FP_HA_BLK_HDR_PTR(_u_, _blk_id_)                            \
        FP_HA_INFO(_u_)->blk_hdrs[_blk_id_]

#define FP_HA_INFO_UPDATE_ALLOCATED_BLK_ID(_u_, _id)                \
        {                                                           \
           FP_HA_INFO(_u_)->last_allocated_blk_id = _id;            \
        }

#define FP_HA_NEW_BLK_ID(_u, _id)                                     \
        {                                                             \
            _id = FP_HA_INFO(_u)->last_allocated_blk_id + 1;          \
            if ((_id > BCM_FIELD_HA_BLK_ID_MAX) ||                    \
                (FP_HA_BLK_HDR_PTR(_u, _id) != NULL)) {               \
                for (_id = 2; _id < BCM_FIELD_HA_BLK_ID_MAX; _id++) { \
                    if (FP_HA_BLK_HDR_PTR(_u, _id) != NULL) {         \
                        continue;                                     \
                    }                                                 \
                    break;                                            \
                }                                                     \
                if (_id == BCM_FIELD_HA_BLK_ID_MAX) {                 \
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);          \
                }                                                     \
            }                                                         \
            FP_HA_INFO(_u)->last_allocated_blk_id = _id;              \
        }

#define FP_BLK_ID_INFO_SET(_u_, _blk_hdr, _blk_id)                  \
        {                                                           \
            FP_HA_INFO(_u_)->blk_id[_blk_hdr->blk_type] = _blk_id;  \
        }

#define FP_HA_ROOT_BLK_ID_BMP_SET(_u_, _blk_id)                         \
        {                                                               \
            void *_ptr;                                                 \
            bcmi_field_ha_root_blk_t *_root;                            \
            _ptr = FP_HA_BLK_HDR_PTR(unit, BCMI_FIELD_HA_BLK_ID_ROOT);  \
            _root = (bcmi_field_ha_root_blk_t *)                        \
                    ((uint8_t *)_ptr + sizeof(bcmi_field_ha_blk_hdr_t));\
            SHR_BITSET(_root->in_use_ha_blk_id_bmp.w, _blk_id);         \
        }

#define FP_HA_ROOT_BLK_ID_BMP_CLR(_u_, _blk_id)                         \
        {                                                               \
            void *_ptr;                                                 \
            bcmi_field_ha_root_blk_t *_root;                            \
            _ptr = FP_HA_BLK_HDR_PTR(unit, BCMI_FIELD_HA_BLK_ID_ROOT);  \
            _root = (bcmi_field_ha_root_blk_t *)                        \
                    ((uint8_t *)_ptr + sizeof(bcmi_field_ha_blk_hdr_t));\
            SHR_BITCLR(_root->in_use_ha_blk_id_bmp.w, _blk_id);         \
        }

#define FP_ROOT_BLK_ID(_u_)                         \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_ROOT];

#define FP_IFP_GRP_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_INFO];

#define FP_IFP_GRP_HASH_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_HASH];

#define FP_IFP_PRESEL_INFO_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_INFO];

#define FP_IFP_PRESEL_HASH_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_HASH];

#define FP_IFP_ENT_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_INFO];

#define FP_IFP_ENT_HASH_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_HASH];

#define FP_IFP_SBR_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_SBR_OPER];

#define FP_EFP_PRESEL_INFO_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_INFO];

#define FP_EFP_PRESEL_HASH_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_HASH];

#define FP_EFP_GRP_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_INFO];

#define FP_EFP_GRP_HASH_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_HASH];

#define FP_EFP_ENT_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_INFO];

#define FP_EFP_ENT_HASH_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_HASH];

#define FP_EFP_SBR_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_SBR_OPER];

#define FP_VFP_PRESEL_INFO_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_INFO];

#define FP_VFP_PRESEL_HASH_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_HASH];

#define FP_VFP_GRP_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_INFO];

#define FP_VFP_GRP_HASH_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_HASH];

#define FP_VFP_ENT_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_INFO];

#define FP_VFP_ENT_HASH_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_HASH];

#define FP_VFP_SBR_INFO_BLK_ID(_u_)                 \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_VFP_SBR_OPER];

#define FP_EMFP_PRESEL_INFO_BLK_ID(_u_)             \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_INFO];

#define FP_EMFP_PRESEL_HASH_BLK_ID(_u_)             \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_HASH];

#define FP_EMFP_GRP_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_INFO];

#define FP_EMFP_GRP_HASH_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_HASH];

#define FP_EMFP_ENT_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_INFO];

#define FP_EMFP_ENT_HASH_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_HASH];

#define FP_EMFP_SBR_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFP_SBR_OPER];

#define FP_EMFT_PRESEL_INFO_BLK_ID(_u_)             \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_INFO];

#define FP_EMFT_PRESEL_HASH_BLK_ID(_u_)             \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_HASH];

#define FP_EMFT_GRP_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_INFO];

#define FP_EMFT_GRP_HASH_BLK_ID(_u_)              \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_HASH];

#define FP_EMFT_ENT_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_INFO];

#define FP_EMFT_ENT_HASH_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_HASH];

#define FP_EMFT_SBR_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EMFT_SBR_OPER];

#define FP_IFP_COLOR_TBL_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_INFO];

#define FP_IFP_COLOR_TBL_PDD_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_PDD_INFO];

#define FP_IFP_COLOR_TBL_SBR_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_SBR_INFO];

#define FP_EFP_COLOR_TBL_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_INFO];

#define FP_EFP_COLOR_TBL_PDD_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_PDD_INFO];

#define FP_EFP_COLOR_TBL_SBR_INFO_BLK_ID(_u_)                \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_SBR_INFO];

#define FP_HINT_LIST_BLK_ID(_u_)                            \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_HINT_LIST];

#define FP_HINT_ENTRY_BLK_ID(_u_)                            \
    FP_HA_INFO(_u_)->blk_id[BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY];

#define FP_GRP_INFO_BLK_ID(_u_, _stage_, _blk_id)              \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_GRP_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_GRP_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_GRP_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_GRP_INFO_BLK_ID(_u_);        \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_GRP_INFO_BLK_ID(_u_);        \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_GRP_HASH_BLK_ID(_u_, _stage_, _blk_id)              \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_GRP_HASH_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_GRP_HASH_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_GRP_HASH_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_GRP_HASH_BLK_ID(_u_);        \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_GRP_HASH_BLK_ID(_u_);        \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_PRESEL_INFO_BLK_ID(_u_, _stage_, _blk_id)           \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_PRESEL_INFO_BLK_ID(_u_);      \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_PRESEL_INFO_BLK_ID(_u_);      \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_PRESEL_INFO_BLK_ID(_u_);      \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_PRESEL_INFO_BLK_ID(_u_);     \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_PRESEL_INFO_BLK_ID(_u_);     \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_PRESEL_HASH_BLK_ID(_u_, _stage_, _blk_id)           \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_PRESEL_HASH_BLK_ID(_u_);      \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_PRESEL_HASH_BLK_ID(_u_);      \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_PRESEL_HASH_BLK_ID(_u_);      \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_PRESEL_HASH_BLK_ID(_u_);     \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_PRESEL_HASH_BLK_ID(_u_);     \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_ENTRY_INFO_BLK_ID(_u_, _stage_, _blk_id)            \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_ENT_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_ENT_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_ENT_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_ENT_INFO_BLK_ID(_u_);        \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_ENT_INFO_BLK_ID(_u_);        \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_ENTRY_HASH_BLK_ID(_u_, _stage_, _blk_id)            \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_ENT_HASH_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_ENT_HASH_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_ENT_HASH_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_ENT_HASH_BLK_ID(_u_);        \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_ENT_HASH_BLK_ID(_u_);        \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_SBR_INFO_BLK_ID(_u_, _stage_, _blk_id)              \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_SBR_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageVlan:                           \
                _blk_id = FP_VFP_SBR_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_SBR_INFO_BLK_ID(_u_);         \
                break;                                         \
            case bcmiFieldStageExactMatch:                     \
                _blk_id = FP_EMFP_SBR_INFO_BLK_ID(_u_);        \
                break;                                         \
            case bcmiFieldStageFlowTracker:                    \
                _blk_id = FP_EMFT_SBR_INFO_BLK_ID(_u_);        \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_COLOR_TBL_INFO_BLK_ID(_u_, _stage_, _blk_id)        \
    {                                                          \
        switch (_stage_) {                                     \
            case bcmiFieldStageIngress:                        \
                _blk_id = FP_IFP_COLOR_TBL_INFO_BLK_ID(_u_);   \
                break;                                         \
            case bcmiFieldStageEgress:                         \
                _blk_id = FP_EFP_COLOR_TBL_INFO_BLK_ID(_u_);   \
                break;                                         \
            default:                                           \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);       \
         }                                                     \
    }

#define FP_COLOR_TBL_PDD_INFO_BLK_ID(_u_, _stage_, _blk_id)     \
    {                                                           \
        switch (_stage_) {                                      \
            case bcmiFieldStageIngress:                         \
                _blk_id = FP_IFP_COLOR_TBL_PDD_INFO_BLK_ID(_u_);\
                break;                                          \
            case bcmiFieldStageEgress:                          \
                _blk_id = FP_EFP_COLOR_TBL_PDD_INFO_BLK_ID(_u_);\
                break;                                          \
            default:                                            \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);        \
         }                                                      \
    }

#define FP_COLOR_TBL_SBR_INFO_BLK_ID(_u_, _stage_, _blk_id)     \
    {                                                           \
        switch (_stage_) {                                      \
            case bcmiFieldStageIngress:                         \
                _blk_id = FP_IFP_COLOR_TBL_SBR_INFO_BLK_ID(_u_);\
                break;                                          \
            case bcmiFieldStageEgress:                          \
                _blk_id = FP_EFP_COLOR_TBL_SBR_INFO_BLK_ID(_u_);\
                break;                                          \
            default:                                            \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);        \
         }                                                      \
    }

#define FP_COLOR_TBL_INFO_BLK_TYPE(_u_, _stage_, _blk_type)             \
    {                                                                   \
        switch (_stage_) {                                              \
            case bcmiFieldStageIngress:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_INFO;  \
                break;                                                  \
            case bcmiFieldStageEgress:                                  \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_INFO;  \
                break;                                                  \
            default:                                                    \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);                \
         }                                                              \
    }

#define FP_COLOR_TBL_PDD_INFO_BLK_TYPE(_u_, _stage_, _blk_type)             \
    {                                                                       \
        switch (_stage_) {                                                  \
            case bcmiFieldStageIngress:                                     \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_PDD_INFO;  \
                break;                                                      \
            case bcmiFieldStageEgress:                                      \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_PDD_INFO;  \
                break;                                                      \
            default:                                                        \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);                    \
         }                                                                  \
    }

#define FP_COLOR_TBL_SBR_INFO_BLK_TYPE(_u_, _stage_, _blk_type)             \
    {                                                                       \
        switch (_stage_) {                                                  \
            case bcmiFieldStageIngress:                                     \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_SBR_INFO;  \
                break;                                                      \
            case bcmiFieldStageEgress:                                      \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_SBR_INFO;  \
                break;                                                      \
            default:                                                        \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);                    \
         }                                                                  \
    }

#define FP_STAGE_GRP_INFO_BLK_TYPE(_u_, _stage_, _blk_type)         \
    {                                                               \
        switch (_stage_) {                                          \
            case bcmiFieldStageIngress:                             \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_INFO;  \
                break;                                              \
            case bcmiFieldStageVlan:                                \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_INFO;  \
                break;                                              \
            case bcmiFieldStageEgress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_INFO;  \
                break;                                              \
            case bcmiFieldStageExactMatch:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_INFO; \
                break;                                              \
            case bcmiFieldStageFlowTracker:                         \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_INFO; \
                break;                                              \
            default:                                                \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);            \
         }                                                          \
    }

#define FP_STAGE_GRP_HASH_BLK_TYPE(_u_, _stage_, _blk_type)         \
    {                                                               \
        switch (_stage_) {                                          \
            case bcmiFieldStageIngress:                             \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_HASH;  \
                break;                                              \
            case bcmiFieldStageVlan:                                \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_HASH;  \
                break;                                              \
            case bcmiFieldStageEgress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_HASH;  \
                break;                                              \
            case bcmiFieldStageExactMatch:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_HASH; \
                break;                                              \
            case bcmiFieldStageFlowTracker:                         \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_HASH; \
                break;                                              \
            default:                                                \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);            \
         }                                                          \
    }

#define FP_STAGE_GRP_OPER_BLK_TYPE(_u_, _stage_, _blk_type)         \
    {                                                               \
        switch (_stage_) {                                          \
            case bcmiFieldStageIngress:                             \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER;  \
                break;                                              \
            case bcmiFieldStageVlan:                                \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER;  \
                break;                                              \
            case bcmiFieldStageEgress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER;  \
                break;                                              \
            case bcmiFieldStageExactMatch:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER; \
                break;                                              \
            case bcmiFieldStageFlowTracker:                         \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER; \
                break;                                              \
            default:                                                \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);            \
         }                                                          \
    }


#define FP_STAGE_PRESEL_INFO_BLK_TYPE(_u_, _stage_, _blk_type)       \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_INFO;  \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_INFO;  \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_INFO;  \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_INFO; \
                break;                                               \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_INFO; \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_STAGE_PRESEL_HASH_BLK_TYPE(_u_, _stage_, _blk_type)       \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_HASH;  \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_HASH;  \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_HASH;  \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_HASH; \
                break;                                              \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_HASH; \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_STAGE_PRESEL_OPER_BLK_TYPE(_u_, _stage_, _blk_type)       \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER;  \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER;  \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER;  \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER; \
                break;                                               \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER; \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_STAGE_ENTRY_INFO_BLK_TYPE(_u_, _stage_, _blk_type)        \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_INFO;   \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_INFO;   \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_INFO;   \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_INFO;  \
                break;                                               \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_INFO;  \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_STAGE_ENTRY_HASH_BLK_TYPE(_u_, _stage_, _blk_type)        \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_HASH;   \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_HASH;   \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_HASH;   \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_HASH;  \
                break;                                               \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_HASH;  \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_STAGE_ENTRY_OPER_BLK_TYPE(_u_, _stage_, _blk_type)        \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER;   \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER;   \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER;   \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER;  \
                break;                                               \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER;  \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_STAGE_SBR_INFO_BLK_TYPE(_u_, _stage_, _blk_type)          \
    {                                                                \
        switch (_stage_) {                                           \
            case bcmiFieldStageIngress:                              \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_IFP_SBR_INFO;     \
                break;                                               \
            case bcmiFieldStageVlan:                                 \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_VFP_SBR_INFO;     \
                break;                                               \
            case bcmiFieldStageEgress:                               \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EFP_SBR_INFO;     \
                break;                                               \
            case bcmiFieldStageExactMatch:                           \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFP_SBR_INFO;    \
                break;                                               \
            case bcmiFieldStageFlowTracker:                          \
                _blk_type = BCMI_FIELD_HA_BLK_TYPE_EMFT_SBR_INFO;    \
                break;                                               \
            default:                                                 \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);             \
         }                                                           \
    }

#define FP_HINT_LIST_BLK_TYPE(_u_, _blk_type)                       \
    {                                                               \
        _blk_type = BCMI_FIELD_HA_BLK_TYPE_HINT_LIST;               \
    }

#define FP_HINT_ENTRY_BLK_TYPE(_u_, _blk_type)                      \
    {                                                               \
        _blk_type = BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY;              \
    }

#define FP_HA_DYNAMIC_BLK_SIZE(_u_, _p, _blk_type, _blk_size)                 \
    {                                                                         \
        switch (_blk_type) {                                                  \
            case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER:                       \
                 /* _blk_size =  pipe * max_groups * sizeof(group_oper) */    \
                 _blk_size = _p * 20 * sizeof(bcmi_field_ha_group_oper_t);    \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER:                       \
            case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER:                       \
                 _blk_size = _p * 4 * sizeof(bcmi_field_ha_group_oper_t);     \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER:                      \
            case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER:                      \
                 _blk_size = _p * 4 * sizeof(bcmi_field_ha_group_oper_t);     \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER:                       \
                 _blk_size = _p * 4096 * sizeof(bcmi_field_ha_entry_oper_t);  \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER:                       \
            case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER:                       \
                 _blk_size = _p * 1024 * sizeof(bcmi_field_ha_entry_oper_t);  \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER:                      \
            case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER:                      \
                 _blk_size = _p * 10240 * sizeof(bcmi_field_ha_entry_oper_t); \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER:                      \
            case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER:                      \
            case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER:                      \
            case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER:                     \
            case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER:                     \
                 _blk_size = _p * 32 * sizeof(bcmi_field_ha_presel_oper_t);   \
                 break;                                                       \
            case BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY:                             \
                 _blk_size = 40 * sizeof(bcmi_field_ha_hint_entry_t);         \
                 break;                                                       \
            default:                                                          \
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);                      \
         }                                                                    \
    }

#define FP_HA_MEM_ALLOC(_u_, _comp_, _sub_comp_,                     \
                          _size_, _ha_ptr_, _descr_, _w_)            \
        {                                                            \
            uint32_t _ha_size_ = _size_;                             \
            _ha_ptr_ = bcmi_ltsw_ha_mem_alloc(_u_,                   \
                                              _comp_,                \
                                              _sub_comp_,            \
                                              _descr_,               \
                                              &_ha_size_);           \
            if (_w_ == 0) {                                          \
                SHR_NULL_CHECK(_ha_ptr_, SHR_E_MEMORY);      \
                SHR_IF_ERR_VERBOSE_EXIT((_ha_size_ < _size_) ?       \
                                    SHR_E_MEMORY : SHR_E_NONE);      \
            } else {                                                 \
                SHR_NULL_CHECK(_ha_ptr_, SHR_E_INTERNAL);    \
                _size_ = _ha_size_;                                  \
            }                                                        \
        }


#define FP_HA_GROUP_OPER_DEF_ENTRY_ASET_ARR_GET(_u_, _grp_oper, _aset)         \
     {                                                               \
         bcm_field_action_t _a;                                      \
         for (_a = 0; _a < _grp_oper->def_action_cnt; _a++) {            \
             BCM_FIELD_ASET_ADD(_aset, _grp_oper->def_aset_arr[_a]);     \
         }                                                           \
     }

#define FP_HA_GROUP_OPER_DEF_ENTRY_ASET_ARR_SET(_u_, _grp_oper, _aset)        \
     {                                                              \
         uint16_t _ct = 0;                                          \
         bcm_field_action_t _a;                                     \
         for (_a = 0; _a < bcmFieldActionCount; _a++) {             \
             if (BCM_FIELD_ASET_TEST(_aset, _a)) {                  \
                 _grp_oper->def_aset_arr[_ct++] = _a;                   \
             }                                                      \
         }                                                          \
         _grp_oper->def_action_cnt = _ct;                               \
     }

#define FP_HA_GROUP_OPER_DEF_ENTRY_ASET_TEST(_grp_oper, _act, _f)             \
     {                                                              \
         bcm_field_action_t _a;                                     \
         _f = FALSE;                                                \
         for (_a = 0; _a < _grp_oper->def_action_cnt; _a++) {           \
             if (_grp_oper->def_aset_arr[_a] == _act) {                 \
                 _f = TRUE;                                         \
                 break;                                             \
             }                                                      \
         }                                                          \
     }

#define FP_HA_GROUP_OPER_ASET_ARR_GET(_u_, _grp_oper, _aset)         \
     {                                                               \
         bcm_field_action_t _a;                                      \
         for (_a = 0; _a < _grp_oper->action_cnt; _a++) {            \
             BCM_FIELD_ASET_ADD(_aset, _grp_oper->aset_arr[_a]);     \
         }                                                           \
     }

#define FP_HA_GROUP_OPER_ASET_ARR_SET(_u_, _grp_oper, _aset)        \
     {                                                              \
         uint16_t _ct = 0;                                          \
         bcm_field_action_t _a;                                     \
         for (_a = 0; _a < bcmFieldActionCount; _a++) {             \
             if (BCM_FIELD_ASET_TEST(_aset, _a)) {                  \
                 _grp_oper->aset_arr[_ct++] = _a;                   \
             }                                                      \
         }                                                          \
         _grp_oper->action_cnt = _ct;                               \
     }

#define FP_HA_GROUP_OPER_ASET_TEST(_grp_oper, _act, _f)             \
     {                                                              \
         bcm_field_action_t _a;                                     \
         _f = FALSE;                                                \
         for (_a = 0; _a < _grp_oper->action_cnt; _a++) {           \
             if (_grp_oper->aset_arr[_a] == _act) {                 \
                 _f = TRUE;                                         \
                 break;                                             \
             }                                                      \
         }                                                          \
     }

#define FP_HA_GROUP_OPER_QSET_ARR_GET(_u_, _grp_oper, _qset)        \
     {                                                              \
         bcmi_field_qualify_t _q;                                   \
         for (_q = 0; _q < _grp_oper->qual_cnt; _q++) {             \
             BCM_FIELD_QSET_ADD(_qset, _grp_oper->qset_arr[_q]);    \
         }                                                          \
     }

#define FP_HA_GROUP_OPER_QSET_ARR_SET(_u_, _grp_oper, _qset)        \
     {                                                              \
         uint16_t _ct = 0;                                          \
         bcmi_field_qualify_t _q;                                   \
         for (_q = 0; _q < bcmiFieldQualifyLastCount; _q++) {       \
             if (BCM_FIELD_QSET_TEST(_qset, _q)) {                  \
                 _grp_oper->qset_arr[_ct++] = _q;                   \
             }                                                      \
         }                                                          \
         _grp_oper->qual_cnt = _ct;                                 \
     }

#define FP_HA_GROUP_OPER_QSET_TEST(_grp_oper, _qual, _f)            \
     {                                                              \
         bcmi_field_qualify_t _q;                                   \
         _f = FALSE;                                                \
         for (_q = 0; _q < _grp_oper->qual_cnt; _q++) {             \
             if (_grp_oper->qset_arr[_q] == _qual) {                \
                 _f = TRUE;                                         \
                 break;                                             \
             }                                                      \
         }                                                          \
     }

#define FP_HA_GROUP_OPER_PRESEL_ARR_GET(_u_, _grp_oper, _pset)      \
     {                                                              \
         uint16_t _p;                                               \
         for (_p = 0; _p < _grp_oper->presel_cnt; _p++) {           \
             BCM_FIELD_PRESEL_ADD(_pset, _grp_oper->presel_arr[_p]);\
         }                                                          \
     }


#define FP_HA_GROUP_OPER_PRESEL_ARR_SET(_u_, _grp_oper, _pset)      \
     {                                                              \
         uint16_t _ct = 0;                                          \
         uint16_t _p;                                               \
         for (_p = 0; _p < BCM_FIELD_PRESEL_SEL_MAX; _p++) {        \
             if (BCM_FIELD_PRESEL_TEST(_pset, _p)) {                \
                 _grp_oper->presel_arr[_ct++] = _p;                 \
             }                                                      \
         }                                                          \
         _grp_oper->presel_cnt = _ct;                               \
     }


#define FP_HA_GROUP_OPER_PRESEL_TEST(_grp_oper, _presel, _f)        \
     {                                                              \
         int _p;                                                    \
         _f = FALSE;                                                \
         for (_p = 0; _p < _grp_oper->presel_cnt; _p++) {           \
             if (_grp_oper->presel_arr[_p] == _presel) {            \
                 _f = TRUE;                                         \
                 break;                                             \
             }                                                      \
         }                                                          \
     }

#define FP_HA_GRP_BASE_BLK_GET(_u, _stage, _hash_off, _grp_base)    \
     {                                                              \
         int _blk_type;                                             \
         void *_hash_blk = NULL;                                    \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;             \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                         \
         FP_GRP_HASH_BLK_ID(_u, _stage, _blk_id);                   \
         _hash_blk = FP_HA_BLK_HDR_PTR(_u, _blk_id);                \
         if ((_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||            \
             (_hash_blk == NULL)) {                                 \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                   \
         }                                                          \
         FP_STAGE_GRP_HASH_BLK_TYPE(_u, _stage, _blk_type);         \
         _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_hash_blk;           \
         if (_blk_hdr->blk_type != _blk_type) {                     \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                   \
         }                                                          \
         _grp_base = (bcmi_field_ha_blk_info_t *)                   \
                     (_hash_blk + sizeof(bcmi_field_ha_blk_hdr_t)); \
         _grp_base = _grp_base + _hash_off;                         \
     }

#define FP_HA_GRP_OPER_GET(_u, _stage, _hash_off, _oper)                 \
     {                                                                   \
         int _blk_type;                                                  \
         void *_ptr = NULL;                                              \
         bcmi_field_ha_blk_info_t *_head;                                \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;                  \
         uint32_t _blk_off;                                              \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                              \
         FP_HA_GRP_BASE_BLK_GET(_u, _stage, _hash_off, _head);           \
         _blk_id = _head->blk_id;                                        \
         _blk_off = _head->blk_offset;                                   \
         if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                   \
             _oper = NULL;                                               \
         } else {                                                        \
             FP_STAGE_GRP_OPER_BLK_TYPE(_u, _stage, _blk_type);          \
             _ptr = FP_HA_BLK_HDR_PTR(unit, _blk_id);                    \
             _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ptr;                 \
             if (_blk_hdr->blk_type != _blk_type) {                      \
                 SHR_ERR_EXIT(SHR_E_INTERNAL);                    \
             }                                                           \
             _oper = (bcmi_field_ha_group_oper_t *)(_ptr + _blk_off);    \
         }                                                               \
     }

#define FP_HA_GRP_BASE_OPER_GET(_u, _stage, _g_base, _hash_off, _oper)   \
     {                                                                   \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;                  \
         uint32_t _blk_off;                                              \
         uint32_t _blk_type;                                             \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                              \
         bcmi_field_ha_blk_info_t *_b_info;                              \
         void *_ptr;                                                     \
         if (_g_base == NULL) {                                          \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                        \
         }                                                               \
         _b_info = _g_base + _hash_off;                                  \
         _blk_id = _b_info->blk_id;                                      \
         if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                   \
             _oper = NULL;                                               \
         } else {                                                        \
             _blk_off = _b_info->blk_offset;                             \
             FP_STAGE_GRP_OPER_BLK_TYPE(_u, _stage, _blk_type);          \
             _ptr = FP_HA_BLK_HDR_PTR(unit, _blk_id);                    \
             _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ptr;                 \
             if (_blk_hdr->blk_type != _blk_type) {                      \
                 SHR_ERR_EXIT(SHR_E_INTERNAL);                    \
             }                                                           \
             _oper = (bcmi_field_ha_group_oper_t *)(_ptr + _blk_off);    \
         }                                                               \
     }

#define FP_HA_GRP_OPER_NEXT(_u, _oper, _oper_next)                        \
     {                                                                    \
         if (_oper->next.blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {         \
             _oper_next = (bcmi_field_ha_group_oper_t *)                  \
                      (FP_HA_BLK_HDR_PTR(_u, _oper->next.blk_id) +        \
                       _oper->next.blk_offset);                           \
         } else {                                                         \
             _oper_next = NULL;                                           \
         }                                                                \
     }


#define FP_HA_ENT_BASE_BLK_GET(_u, _stage, _hash_off, _ent_base)    \
     {                                                              \
         int _blk_type;                                             \
         void *_hash_blk = NULL;                                    \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;             \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                         \
         FP_ENTRY_HASH_BLK_ID(_u, _stage, _blk_id);                 \
         _hash_blk = FP_HA_BLK_HDR_PTR(_u, _blk_id);                \
         if ((_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||            \
             (_hash_blk == NULL)) {                                 \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                   \
         }                                                          \
         FP_STAGE_ENTRY_HASH_BLK_TYPE(_u, _stage, _blk_type);       \
         _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_hash_blk;           \
         if (_blk_hdr->blk_type != _blk_type) {                     \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                   \
         }                                                          \
         _ent_base = (bcmi_field_ha_blk_info_t *)                   \
                     (_hash_blk + sizeof(bcmi_field_ha_blk_hdr_t)); \
         _ent_base = _ent_base + _hash_off;                         \
     }

#define FP_HA_ENT_OPER_GET(_u, _stage, _hash_off, _oper)                 \
     {                                                                   \
         int _blk_type;                                                  \
         void *_ptr = NULL;                                              \
         bcmi_field_ha_blk_info_t *_ent_head;                            \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;                  \
         uint32_t _blk_off;                                              \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                              \
         FP_HA_ENT_BASE_BLK_GET(_u, _stage, _hash_off, _ent_head);       \
         _blk_id = _ent_head->blk_id;                                    \
         _blk_off = _ent_head->blk_offset;                               \
         if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                   \
             _oper = NULL;                                               \
         } else {                                                        \
             FP_STAGE_ENTRY_OPER_BLK_TYPE(_u, _stage, _blk_type);        \
             _ptr = FP_HA_BLK_HDR_PTR(unit, _blk_id);                    \
             _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ptr;                 \
             if (_blk_hdr->blk_type != _blk_type) {                      \
                 SHR_ERR_EXIT(SHR_E_INTERNAL);                    \
             }                                                           \
             _oper = (bcmi_field_ha_entry_oper_t *) (_ptr + _blk_off);   \
         }                                                               \
     }

#define FP_HA_ENT_BASE_OPER_GET(_u, _stage, _e_base, _hash_off, _oper)   \
     {                                                                   \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;                  \
         uint32_t _blk_off;                                              \
         uint32_t _blk_type;                                             \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                              \
         bcmi_field_ha_blk_info_t *_b_info;                              \
         void *_ptr;                                                     \
         if (_e_base == NULL) {                                          \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                        \
         }                                                               \
         _b_info = _e_base + _hash_off;                                  \
         _blk_id = _b_info->blk_id;                                      \
         if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                   \
             _oper = NULL;                                               \
         } else {                                                        \
             _blk_off = _b_info->blk_offset;                             \
             FP_STAGE_ENTRY_OPER_BLK_TYPE(_u, _stage, _blk_type);        \
             _ptr = FP_HA_BLK_HDR_PTR(unit, _blk_id);                    \
             _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ptr;                 \
             if (_blk_hdr->blk_type != _blk_type) {                      \
                 SHR_ERR_EXIT(SHR_E_INTERNAL);                    \
             }                                                           \
             _oper = (bcmi_field_ha_entry_oper_t *)(_ptr + _blk_off);    \
         }                                                               \
     }

#define FP_HA_ENT_OPER_NEXT(_u, _oper, _oper_next)                    \
     {                                                                \
         if (_oper->next.blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {     \
             _oper_next = (bcmi_field_ha_entry_oper_t *)              \
                      (FP_HA_BLK_HDR_PTR(_u, _oper->next.blk_id) +    \
                       _oper->next.blk_offset);                       \
         } else {                                                     \
             _oper_next = NULL;                                       \
         }                                                            \
     }

#define FP_HA_PRESEL_BASE_BLK_GET(_u, _stage, _hash_off, _ent_base)   \
     {                                                                \
         int _blk_type;                                               \
         void *_hash_blk = NULL;                                      \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;               \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                           \
         FP_PRESEL_HASH_BLK_ID(_u, _stage, _blk_id);                  \
         _hash_blk = FP_HA_BLK_HDR_PTR(_u, _blk_id);                  \
         if ((_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) ||              \
             (_hash_blk == NULL)) {                                   \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                     \
         }                                                            \
         FP_STAGE_PRESEL_HASH_BLK_TYPE(_u, _stage, _blk_type);        \
         _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_hash_blk;             \
         if (_blk_hdr->blk_type != _blk_type) {                       \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                     \
         }                                                            \
         _ent_base = (bcmi_field_ha_blk_info_t *)                     \
                     (_hash_blk + sizeof(bcmi_field_ha_blk_hdr_t));   \
         _ent_base = _ent_base + _hash_off;                           \
     }

#define FP_HA_PRESEL_OPER_GET(_u, _stage, _hash_off, _oper)              \
     {                                                                   \
         int _blk_type;                                                  \
         void *_ptr = NULL;                                              \
         bcmi_field_ha_blk_info_t *_ent_head;                            \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;                  \
         uint32_t _blk_off;                                              \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                              \
         FP_HA_PRESEL_BASE_BLK_GET(_u, _stage, _hash_off, _ent_head);    \
         _blk_id = _ent_head->blk_id;                                    \
         _blk_off = _ent_head->blk_offset;                               \
         if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                   \
             _oper = NULL;                                               \
         } else {                                                        \
             FP_STAGE_PRESEL_OPER_BLK_TYPE(_u, _stage, _blk_type);       \
             _ptr = FP_HA_BLK_HDR_PTR(unit, _blk_id);                    \
             _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ptr;                 \
             if (_blk_hdr->blk_type != _blk_type) {                      \
                 SHR_ERR_EXIT(SHR_E_INTERNAL);                    \
             }                                                           \
             _oper = (bcmi_field_ha_presel_oper_t *) (_ptr + _blk_off);  \
         }                                                               \
     }

#define FP_HA_PRESEL_BASE_OPER_GET(_u, _stage, _e_base, _hash_off, _oper)  \
     {                                                                   \
         uint8_t _blk_id = BCM_FIELD_HA_BLK_ID_INVALID;                  \
         uint32_t _blk_off;                                              \
         int _blk_type;                                                  \
         void *_ptr;                                                     \
         bcmi_field_ha_blk_hdr_t *_blk_hdr;                              \
         bcmi_field_ha_blk_info_t *_b_info;                              \
         if (_e_base == NULL) {                                          \
             SHR_ERR_EXIT(SHR_E_INTERNAL);                        \
         }                                                               \
         _b_info = _e_base + _hash_off;                                  \
         _blk_id = _b_info->blk_id;                                      \
         if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                   \
             _oper = NULL;                                               \
         } else {                                                        \
             _blk_off = _b_info->blk_offset;                             \
             FP_STAGE_PRESEL_OPER_BLK_TYPE(_u, _stage, _blk_type);       \
             _ptr = FP_HA_BLK_HDR_PTR(unit, _blk_id);                    \
             _blk_hdr = (bcmi_field_ha_blk_hdr_t *)_ptr;                 \
             if (_blk_hdr->blk_type != _blk_type) {                      \
                 SHR_ERR_EXIT(SHR_E_INTERNAL);                    \
             }                                                           \
             _oper = (bcmi_field_ha_presel_oper_t *) (_ptr + _blk_off);  \
         }                                                               \
     }

#define FP_HA_PRESEL_OPER_NEXT(_u, _oper, _oper_next)                     \
     {                                                                    \
         if (_oper->next.blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {         \
             _oper_next = (bcmi_field_ha_presel_oper_t *)                 \
                      (FP_HA_BLK_HDR_PTR(_u, _oper->next.blk_id) +        \
                       _oper->next.blk_offset);                           \
         } else {                                                         \
             _oper_next = NULL;                                           \
         }                                                                \
     }


#define FP_HINT_ENTRY_OPER_FIRST(_u, blk_id, blk_off, _oper_first)        \
    {                                                                     \
        if (blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {                      \
            _oper_first = (bcmi_field_ha_hint_entry_t *)                  \
                (FP_HA_BLK_HDR_PTR(_u, blk_id) + blk_off);                \
        } else {                                                          \
            _oper_first = NULL;                                           \
        }                                                                 \
    }

#define FP_HINT_ENTRY_OPER_NEXT(_u, _oper, _oper_next)                    \
     {                                                                    \
         if (_oper->next.blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {         \
             _oper_next = (bcmi_field_ha_hint_entry_t *)                  \
                      (FP_HA_BLK_HDR_PTR(_u, _oper->next.blk_id) +        \
                       _oper->next.blk_offset);                           \
         } else {                                                         \
             _oper_next = NULL;                                           \
         }                                                                \
     }


#define FP_HA_BLK_ID_DEALLOCATE(_u, _blk_id)                          \
    {                                                                 \
        void *_ptr;                                                   \
        if (_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {                 \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);                  \
        }                                                             \
        _ptr = FP_HA_BLK_HDR_PTR(_u, _blk_id);                        \
        if (_ptr == NULL) {                                           \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);                  \
        }                                                             \
        SHR_IF_ERR_VERBOSE_EXIT                                       \
            (bcmi_ltsw_ha_mem_free(_u, _ptr));                        \
        FP_HA_BLK_HDR_PTR(unit, blk_id) = NULL;                       \
    }


extern int
bcmint_field_ha_blk_element_acquire(
    int unit,
    bcmi_field_ha_blk_type_t blk_type,
    void **ha_element,
    uint8_t *ha_blk_id,
    uint32_t *ha_blk_offset);

extern int
bcmint_field_ha_blk_element_release(
    int unit,
    uint8_t blk_id,
    uint32_t blk_offset,
    bcmi_field_ha_blk_type_t blk_type);
extern int
bcmint_field_ha_init(int unit, bool warm);
extern int
bcmint_field_ha_cleanup(int unit, bool warm);

#endif /* BCMI_LTSW_FIELD_HA_H */
