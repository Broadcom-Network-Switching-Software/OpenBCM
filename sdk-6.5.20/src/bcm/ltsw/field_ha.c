/*! \file field_ha.c
 *
 * Field Module HA for warmboot.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/field.h>

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

ltsw_fp_ha_local_info_t *ltsw_fp_ha_info[BCM_MAX_NUM_UNITS];
/* Link all HA elements to its next HA element. */
#define FP_HA_BLK_ELEMENTS_LINK(_ha_elem, _elem_size, _num_elems,  \
                                _blk_id, _elem_off)                \
    {                                                              \
         int _idx;                                                 \
         for (_idx = 0; _idx < _num_elems; _idx++) {               \
             _elem_off += _elem_size;                              \
             (_ha_elem[_idx].next).blk_id = _blk_id;               \
             (_ha_elem[_idx].next).blk_offset = _elem_off;         \
         }                                                         \
    }


STATIC int
ltsw_field_ha_blk_hdr_validate(int unit, uint8_t blk_id, void *ha_blk_ptr)
{
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_blk_ptr, SHR_E_PARAM);

    blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;

    if (blk_hdr->signature !=
        (uint32_t)(BCM_FIELD_HA_STRUCT_SIGN + blk_id)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_grp_hash_alloc(int unit,
                             bcmint_field_stage_info_t *stage_info,
                             bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_info_t *grp_hash = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        FP_GRP_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    } else {
        int idx;
        int blk_id;
        bcmi_field_ha_group_info_t *grp_info = NULL;

        FP_GRP_INFO_BLK_ID(unit, stage_info->stage_id, blk_id);
        if (blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        grp_info = stage_info->group_info;
        SHR_NULL_CHECK(grp_info, SHR_E_INTERNAL);

        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                      (sizeof(bcmi_field_ha_blk_info_t) * grp_info->hash_size);

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpGrpHashHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_blk_info_t),
                                          grp_info->hash_size,
                                          BCMI_FIELD_HA_BLK_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_STAGE_GRP_HASH_BLK_TYPE(unit, stage_info->stage_id,
                                   blk_hdr->blk_type);

        grp_hash = (bcmi_field_ha_blk_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        for (idx = 0; idx < grp_info->hash_size; idx++) {
            grp_hash->blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
            grp_hash->blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        }
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_grp_info_alloc(int unit,
                             bcmint_field_stage_info_t *stage_info,
                             bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_group_info_t *grp_info = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        FP_GRP_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        grp_info = (bcmi_field_ha_group_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    } else {
        uint64_t min_value = 0;
        uint64_t max_value = 0;

        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                      sizeof(bcmi_field_ha_group_info_t);

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpGrpInfoHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_group_info_t),
                                          1,
                                          BCMI_FIELD_HA_GROUP_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_STAGE_GRP_INFO_BLK_TYPE(unit, stage_info->stage_id,
                                   blk_hdr->blk_type);

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_field_value_range_get(
                     unit,
                     stage_info->tbls_info->group_sid,
                     stage_info->tbls_info->group_key_fid,
                     &min_value,
                     &max_value));

        grp_info = (bcmi_field_ha_group_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        /* Initialize group hash */
        if (max_value > BCMI_LTSW_FIELD_HASH_DIV_FACTOR) {
            grp_info->hash_size =
                    (max_value / BCMI_LTSW_FIELD_HASH_DIV_FACTOR);
        } else {
            grp_info->hash_size = max_value;
        }
        grp_info->gid_max = max_value;
        grp_info->last_allocated_gid = 0;
        grp_info->last_allocated_prio = (BCMINT_FIELD_GROUP_PRIO_ANY);
        BCMINT_FIELD_GRPID_BMP_INIT(grp_info->grpid_bmp);
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

    stage_info->group_info = grp_info;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_entry_hash_alloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_info_t *ent_hash = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        FP_ENTRY_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    } else {
        int idx;
        bcmi_field_ha_entry_info_t *ent_info = NULL;

        ent_info = stage_info->entry_info;
        if (ent_info == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                      (sizeof(bcmi_field_ha_blk_info_t) * ent_info->hash_size);

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpEntHashHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_blk_info_t),
                                          ent_info->hash_size,
                                          BCMI_FIELD_HA_BLK_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_STAGE_ENTRY_HASH_BLK_TYPE(unit, stage_info->stage_id,
                                   blk_hdr->blk_type);

        ent_hash = (bcmi_field_ha_blk_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        for (idx = 0; idx < ent_info->hash_size; idx++) {
            ent_hash->blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
            ent_hash->blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        }
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

exit:
    SHR_FUNC_EXIT();
}


STATIC int
ltsw_field_ha_entry_info_alloc(int unit,
                             bcmint_field_stage_info_t *stage_info,
                             bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_entry_info_t *ent_info = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        FP_ENTRY_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        ent_info = (bcmi_field_ha_entry_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    } else {
        uint64_t min_value = 0;
        uint64_t max_value = 0;

        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                      sizeof(bcmi_field_ha_entry_info_t);

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpEntInfoHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_entry_info_t),
                                          1,
                                          BCMI_FIELD_HA_ENTRY_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_STAGE_ENTRY_INFO_BLK_TYPE(unit, stage_info->stage_id,
                                     blk_hdr->blk_type);

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_lt_field_value_range_get(
                     unit,
                     stage_info->tbls_info->entry_sid,
                     stage_info->tbls_info->entry_key_fid,
                     &min_value,
                     &max_value));

        ent_info = (bcmi_field_ha_entry_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        /* Initialize entry hash */
        if (max_value > BCMI_LTSW_FIELD_HASH_DIV_FACTOR) {
            ent_info->hash_size =
                    (max_value / BCMI_LTSW_FIELD_HASH_DIV_FACTOR);
        } else {
            ent_info->hash_size = max_value;
        }
        ent_info->eid_max = max_value;
        ent_info->last_allocated_eid = 0;
        sal_memset(&(ent_info->eid_bmp), 0x0, sizeof(bcmi_field_eid_bmp_t));

        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

    stage_info->entry_info = ent_info;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_presel_hash_alloc(int unit,
                                bcmint_field_stage_info_t *stage_info,
                                bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_info_t *presel_hash = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        FP_PRESEL_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    } else {
        int idx;
        bcmi_field_ha_presel_info_t *presel_info = NULL;

        presel_info = stage_info->presel_info;
        if (presel_info == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                  (sizeof(bcmi_field_ha_blk_info_t) * presel_info->hash_size);

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpPreselHashHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_blk_info_t),
                                          presel_info->hash_size,
                                          BCMI_FIELD_HA_BLK_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_STAGE_PRESEL_HASH_BLK_TYPE(unit, stage_info->stage_id,
                                   blk_hdr->blk_type);

        presel_hash = (bcmi_field_ha_blk_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        for (idx = 0; idx < presel_info->hash_size; idx++) {
            presel_hash->blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
            presel_hash->blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        }
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

exit:
    SHR_FUNC_EXIT();
}


STATIC int
ltsw_field_ha_color_info_alloc(int unit,
                                bcmint_field_stage_info_t *stage_info,
                                bool warm)
{
    int oper_mode = 0;
    int num_pools = 0;
    uint32_t num_pipes = 0;
    uint32_t ha_blk_size = 0, tbl_size = 0, max_entries = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_profile_info_t *color_tbl_profile = NULL;
    bcmi_field_ha_profile_info_t *color_pdd_profile = NULL;
    bcmi_field_ha_profile_info_t *color_sbr_profile = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        /* Retrieve color table info. */
        FP_COLOR_TBL_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        color_tbl_profile = (bcmi_field_ha_profile_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));

        /* Retrieve color pdd table info. */
        FP_COLOR_TBL_PDD_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        color_pdd_profile = (bcmi_field_ha_profile_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));

        /* Retrieve color sbr table info. */
        FP_COLOR_TBL_SBR_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        color_sbr_profile = (bcmi_field_ha_profile_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    } else {
        /* Retrieve operational mode status */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_mode_get(unit,
                                                stage_info,
                                                &oper_mode));

        if (oper_mode == bcmiFieldOperModePipeUnique) {
            num_pipes = bcmi_ltsw_dev_max_pp_pipe_num(unit);
        } else {
            num_pipes = 1;
        }

        color_tbl_info = stage_info->color_action_tbls_info;
        SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);

        /* Allocate memoy for color table info. */
        num_pools =  stage_info->color_action_tbls_info->num_pools;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit,
                                  color_tbl_info->tbl_sid[0],
                                  &tbl_size));

        max_entries = (tbl_size * num_pools * num_pipes);
        ha_blk_size = (sizeof(bcmi_field_ha_blk_hdr_t) +
                       (sizeof(bcmi_field_ha_profile_info_t ) * max_entries));

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpMeterColorInfoHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_profile_info_t),
                                          max_entries,
                                          BCMI_FIELD_HA_PROFILE_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_COLOR_TBL_INFO_BLK_TYPE(unit, stage_info->stage_id,
                                   blk_hdr->blk_type);

        color_tbl_profile = (bcmi_field_ha_profile_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        /* Allocate memoy for color pdd info. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit,
                                  color_tbl_info->pdd_tbl_sid,
                                  &tbl_size));

        max_entries = (tbl_size * num_pipes);
        ha_blk_size = (sizeof(bcmi_field_ha_blk_hdr_t) +
                       (sizeof(bcmi_field_ha_profile_info_t ) * max_entries));

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpMeterColorPddInfoHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_profile_info_t),
                                          max_entries,
                                          BCMI_FIELD_HA_PROFILE_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_COLOR_TBL_PDD_INFO_BLK_TYPE(unit, stage_info->stage_id,
                                       blk_hdr->blk_type);

        color_pdd_profile = (bcmi_field_ha_profile_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        /* Allocate memoy for color sbr info. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit,
                                  color_tbl_info->sbr_sid,
                                  &tbl_size));

        max_entries = (tbl_size * num_pipes);
        ha_blk_size = (sizeof(bcmi_field_ha_blk_hdr_t) +
                       (sizeof(bcmi_field_ha_profile_info_t ) * max_entries));

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpMeterColorSbrInfoHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_profile_info_t),
                                          max_entries,
                                          BCMI_FIELD_HA_PROFILE_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_COLOR_TBL_SBR_INFO_BLK_TYPE(unit, stage_info->stage_id,
                                       blk_hdr->blk_type);

        color_sbr_profile = (bcmi_field_ha_profile_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

    stage_info->color_tbl_profile = color_tbl_profile;
    stage_info->color_pdd_tbl_profile = color_pdd_profile;
    stage_info->color_sbr_tbl_profile = color_sbr_profile;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_presel_info_alloc(int unit,
                                bcmint_field_stage_info_t *stage_info,
                                bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_presel_info_t *presel_info = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    if (warm == TRUE) {
        FP_PRESEL_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        presel_info = (bcmi_field_ha_presel_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    } else {
        uint64_t min_value = 0;
        uint64_t max_value = 0;

        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                      sizeof(bcmi_field_ha_presel_info_t);

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpPreselInfoHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_presel_info_t),
                                          1,
                                          BCMI_FIELD_HA_PRESEL_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_STAGE_PRESEL_INFO_BLK_TYPE(unit, stage_info->stage_id,
                                   blk_hdr->blk_type);

        presel_info = (bcmi_field_ha_presel_info_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        presel_info->last_allocated_pid = 0;
        if (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL) {
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmi_lt_field_value_range_get(
                       unit,
                       stage_info->tbls_info->presel_entry_sid,
                       stage_info->tbls_info->presel_entry_key_fid,
                       &min_value,
                       &max_value));
            presel_info->pid_max = max_value;
        }

        if (stage_info->flags & BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC) {
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmi_lt_field_value_range_get(
                       unit,
                       stage_info->tbls_info->presel_group_sid,
                       stage_info->tbls_info->presel_group_key_fid,
                       &min_value,
                       &max_value));
            presel_info->gid_max = max_value;
            presel_info->last_allocated_pgid = 0;
        }

        /* Initialize group hash */
        if (max_value > BCMI_LTSW_FIELD_HASH_DIV_FACTOR) {
            presel_info->hash_size =
                        (max_value / BCMI_LTSW_FIELD_HASH_DIV_FACTOR);
        } else {
            presel_info->hash_size = max_value;
        }

        sal_memset(&presel_info->pid_bmp, 0x0, sizeof(bcmi_field_pid_bmp_t));
        sal_memset(&presel_info->pgid_bmp, 0x0, sizeof(bcmi_field_pgid_bmp_t));

        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }

    stage_info->presel_info = presel_info;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_hints_alloc(int unit,
                          bool warm)
{
    uint32_t ha_blk_size = 0;
    void *ha_blk_ptr = NULL;
    bcmi_field_ha_hints_t *hints_list = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmint_field_control_info_t *fp_control = NULL;
    shr_ha_sub_id ha_blk_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_control_get(unit, &fp_control));

    if (warm == TRUE) {
        ha_blk_id = FP_HINT_LIST_BLK_ID(unit);
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

        /* Set the block ID in the local info */
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);

        hints_list = (bcmi_field_ha_hints_t*)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));

    } else {

        ha_blk_size = (sizeof(bcmi_field_ha_blk_hdr_t) +
                       (sizeof(bcmi_field_ha_hints_t) *
                        BCMI_FIELD_HA_HINT_ID_MAX));

        FP_HA_NEW_BLK_ID(unit, ha_blk_id);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpHintListHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_hints_t),
                                          BCMI_FIELD_HA_HINT_ID_MAX,
                                          BCMI_FIELD_HA_BLK_INFO_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        FP_HINT_LIST_BLK_TYPE(unit, blk_hdr->blk_type);

        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
        hints_list = (bcmi_field_ha_hints_t*)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    }

    fp_control->hints_hash = hints_list;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hint_init(unit));

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_stage_init(int unit,
                         bcmint_field_stage_info_t *stage_info,
                         bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_grp_info_alloc(unit, stage_info, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_grp_hash_alloc(unit, stage_info, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_entry_info_alloc(unit, stage_info, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_entry_hash_alloc(unit, stage_info, warm));

    if (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_presel_info_alloc(unit, stage_info, warm));

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_presel_hash_alloc(unit, stage_info, warm));
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_color_info_alloc(unit, stage_info, warm));
    }

    if (warm == TRUE) {
        if (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_reinit(unit, stage_info));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_ha_stages_init(int unit, bool warm)
{
    bcmi_ltsw_field_stage_t stage;
    bcmint_field_stage_info_t *stage_info = NULL;
    int rv = 0;
    SHR_FUNC_ENTER(unit);

    for (stage = bcmiFieldStageIngress;
         stage < bcmiFieldStageCount;
         stage++) {
        rv = (bcmint_field_stage_info_get(unit,
                                  stage, &stage_info));
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (SHR_E_NOT_FOUND == rv) {
            /* Given stage is not supported on the device. */
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_stage_init(unit, stage_info, warm));
    }

exit:
    SHR_FUNC_EXIT();
}


STATIC int
ltsw_field_ha_root_init(int unit, bool warm)
{
    void *ha_blk_ptr = NULL;
    uint32_t ha_blk_size = 0;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_root_blk_t *root = NULL;
    shr_ha_sub_id ha_blk_id = BCMI_FIELD_HA_BLK_ID_ROOT;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        ha_blk_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        if (ha_blk_ptr == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));
    } else {
        ha_blk_size = sizeof(bcmi_field_ha_blk_hdr_t) +
                      sizeof(bcmi_field_ha_root_blk_t);
        FP_HA_MEM_ALLOC(unit,
                        BCMI_HA_COMP_ID_FIELD,
                        ha_blk_id,
                        ha_blk_size,
                        ha_blk_ptr,
                        "bcmFpRootHaAlloc",
                        warm);
        sal_memset(ha_blk_ptr, 0x0, ha_blk_size);
        FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;
        FP_HA_INFO_UPDATE_ALLOCATED_BLK_ID(unit, ha_blk_id);

        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          0,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          1,
                                          BCMI_FIELD_HA_BLK_HDR_T_ID);
        bcmi_ltsw_issu_struct_info_report(unit,
                                          BCMI_HA_COMP_ID_FIELD,
                                          ha_blk_id,
                                          sizeof(bcmi_field_ha_blk_hdr_t),
                                          sizeof(bcmi_field_ha_root_blk_t),
                                          1,
                                          BCMI_FIELD_HA_ROOT_BLK_T_ID);
        /* Update root block ID bitmap */
        FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_blk_ptr;
        blk_hdr->blk_size = ha_blk_size;
        blk_hdr->backup_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        blk_hdr->free_blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
        blk_hdr->free_ha_element_count = 0;
        blk_hdr->signature = (BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id);
        blk_hdr->blk_type = BCMI_FIELD_HA_BLK_TYPE_ROOT;
        root = (bcmi_field_ha_root_blk_t *)
            ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
        root->min_ha_blk_id = BCM_FIELD_HA_BLK_ID_MIN;
        root->max_ha_blk_id = BCM_FIELD_HA_BLK_ID_MAX;
        sal_memset(root->in_use_ha_blk_id_bmp.w,
                0, sizeof(bcmi_field_ha_blk_id_bmp_t));
        SHR_BITSET(root->in_use_ha_blk_id_bmp.w, ha_blk_id);
        /* Set the block ID in the local info */
        FP_BLK_ID_INFO_SET(unit, blk_hdr, ha_blk_id);
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_warm_ha_init(int unit)
{
    uint32_t blk_size = 0;
    bcmi_field_ha_blk_hdr_t *ha_blk_ptr = NULL;
    bcmi_field_ha_root_blk_t *root = NULL;
    uint8_t ha_blk_id = BCMI_FIELD_HA_BLK_ID_ROOT;
    SHR_FUNC_ENTER(unit);

    FP_HA_MEM_ALLOC(unit,
                      BCMI_HA_COMP_ID_FIELD,
                      ha_blk_id,
                      blk_size,
                      ha_blk_ptr,
                      "bcmFpWbInitRootHaAlloc",
                      TRUE);
    FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));

    root = (bcmi_field_ha_root_blk_t *)
           ((uint8_t *)ha_blk_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    if (root->min_ha_blk_id != BCM_FIELD_HA_BLK_ID_MIN ||
        root->max_ha_blk_id != BCM_FIELD_HA_BLK_ID_MAX) {
        /* upgrade or downgrade case. */
    }

    for (ha_blk_id = root->min_ha_blk_id;
         ha_blk_id <= root->max_ha_blk_id;
         ha_blk_id++) {
        if (SHR_BITGET(root->in_use_ha_blk_id_bmp.w, ha_blk_id)) {
            blk_size = 0;

            FP_HA_MEM_ALLOC(unit,
                            BCMI_HA_COMP_ID_FIELD,
                            ha_blk_id,
                            blk_size,
                            ha_blk_ptr,
                            "bcmFpWbInitBlkHaAlloc",
                            TRUE);
            FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_blk_ptr;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_field_ha_blk_hdr_validate(unit, ha_blk_id, ha_blk_ptr));
            /* Set the block ID in the local info */
            FP_BLK_ID_INFO_SET(unit, ha_blk_ptr, ha_blk_id);
            /* Update Free running block ID */
            FP_HA_INFO_UPDATE_ALLOCATED_BLK_ID(unit, ha_blk_id);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_ha_cleanup(int unit, bool warm)
{
    uint8_t blk_id = BCMI_FIELD_HA_BLK_ID_ROOT;
    bcmi_field_ha_blk_hdr_t *blk_hdr;
    SHR_FUNC_ENTER(unit);

    if (FP_HA_INFO(unit) == NULL) {
        SHR_EXIT();
    }

    if (warm == FALSE) {
       for (; blk_id < BCM_FIELD_HA_BLK_ID_MAX; blk_id++) {
           blk_hdr = FP_HA_BLK_HDR_PTR(unit, blk_id);
           if (blk_hdr == NULL) {
               continue;
           }

           SHR_IF_ERR_VERBOSE_EXIT
               (bcmi_ltsw_ha_mem_free(unit, (void *)blk_hdr));

           FP_HA_BLK_HDR_PTR(unit, blk_id) = NULL;
       }
    }

    BCMINT_FIELD_MEM_FREE(FP_HA_INFO(unit));
    FP_HA_INFO(unit) = NULL;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_ha_init(int unit, bool warm)
{
    ltsw_fp_ha_local_info_t *ha_info = NULL;
    SHR_FUNC_ENTER(unit);

    BCMINT_FIELD_MEM_ALLOC(ha_info,
                           sizeof(ltsw_fp_ha_local_info_t),
                           "FP HA Info alloc.");
    SHR_NULL_CHECK(ha_info, SHR_E_MEMORY);

    FP_HA_INFO(unit) = ha_info;

    /* WARM BOOT */
    if (warm == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT(ltsw_field_warm_ha_init(unit));
    }

    /* Root HA block creation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(ltsw_field_ha_root_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_hints_alloc(unit, warm));

    /* HA block creation and initialization for all supported stages. */
    SHR_IF_ERR_VERBOSE_EXIT(ltsw_field_ha_stages_init(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        bcmint_field_ha_cleanup(unit, warm);
    }
    SHR_FUNC_EXIT();
}

STATIC int
ltsw_fp_ha_blk_create(int unit,
                      uint8_t *blk_id,
                      uint32_t blk_size,
                      bcmi_field_ha_blk_type_t blk_type)
{
    void *ha_ptr = NULL;
    shr_ha_sub_id ha_blk_id;
    uint32_t num_ha_elements = 0;
    uint32_t ha_blk_size = blk_size;
    uint32_t element_offset = 0;
    uint32_t element_size = 0;
    bcmissu_struct_id_t id = 0;
    void *ha_element = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bcmi_field_ha_hint_entry_t *hint_oper = NULL;

    SHR_FUNC_ENTER(unit);

    ha_blk_size += sizeof(bcmi_field_ha_blk_hdr_t);
    FP_HA_NEW_BLK_ID(unit, ha_blk_id);
    FP_HA_MEM_ALLOC(unit,
                    BCMI_HA_COMP_ID_FIELD,
                    ha_blk_id,
                    ha_blk_size,
                    ha_ptr,
                    "bcmFpBlkCreateHaAlloc",
                    FALSE);
    FP_HA_BLK_HDR_PTR(unit, ha_blk_id) = ha_ptr;
    sal_memset(ha_ptr, 0, ha_blk_size);

    /* Inialize the HA elements which starts after block header. */
    ha_element = (void *)((uint8_t *)ha_ptr +
                                  sizeof(bcmi_field_ha_blk_hdr_t));
    element_offset = sizeof(bcmi_field_ha_blk_hdr_t);

    switch (blk_type) {
        case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER:
             id = BCMI_FIELD_HA_GROUP_OPER_T_ID;
             element_size = sizeof(bcmi_field_ha_group_oper_t);
             SHR_IF_ERR_VERBOSE_EXIT(
                 (blk_size % element_size != 0) ? SHR_E_PARAM : SHR_E_NONE);
             num_ha_elements = blk_size/element_size;
             SHR_IF_ERR_EXIT(
                 (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
             group_oper = (bcmi_field_ha_group_oper_t *)ha_element;
             FP_HA_BLK_ELEMENTS_LINK(group_oper, element_size, num_ha_elements,
                                     ha_blk_id, element_offset);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER:
             id = BCMI_FIELD_HA_PRESEL_OPER_T_ID;
             element_size = sizeof(bcmi_field_ha_presel_oper_t);
             SHR_IF_ERR_VERBOSE_EXIT(
                 (blk_size % element_size != 0) ? SHR_E_PARAM : SHR_E_NONE);
             num_ha_elements = blk_size/element_size;
             SHR_IF_ERR_EXIT(
                 (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
             presel_oper = (bcmi_field_ha_presel_oper_t *)ha_element;
             FP_HA_BLK_ELEMENTS_LINK(presel_oper, element_size, num_ha_elements,
                                     ha_blk_id, element_offset);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER:
             id = BCMI_FIELD_HA_ENTRY_OPER_T_ID;
             element_size = sizeof(bcmi_field_ha_entry_oper_t);
             SHR_IF_ERR_VERBOSE_EXIT(
                 (blk_size % element_size != 0) ? SHR_E_PARAM : SHR_E_NONE);
             num_ha_elements = blk_size/element_size;
             SHR_IF_ERR_EXIT(
                 (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
             entry_oper = (bcmi_field_ha_entry_oper_t *)ha_element;
             FP_HA_BLK_ELEMENTS_LINK(entry_oper, element_size, num_ha_elements,
                                     ha_blk_id, element_offset);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY:
             id = BCMI_FIELD_HA_HINT_ENTRY_T_ID;
             element_size = sizeof(bcmi_field_ha_hint_entry_t);
             SHR_IF_ERR_VERBOSE_EXIT(
                 (blk_size % element_size != 0) ? SHR_E_PARAM : SHR_E_NONE);
             num_ha_elements = blk_size/element_size;
             SHR_IF_ERR_EXIT(
                 (num_ha_elements == 0) ? SHR_E_INTERNAL : SHR_E_NONE);
             hint_oper = (bcmi_field_ha_hint_entry_t *)ha_element;
             FP_HA_BLK_ELEMENTS_LINK(hint_oper, element_size, num_ha_elements,
                                     ha_blk_id, element_offset);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_SBR_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_SBR_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_SBR_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_SBR_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_SBR_OPER:
             break;
        default:
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    bcmi_ltsw_issu_struct_info_report(unit,
                                      BCMI_HA_COMP_ID_FIELD,
                                      ha_blk_id,
                                      0,
                                      sizeof(bcmi_field_ha_blk_hdr_t),
                                      1,
                                      BCMI_FIELD_HA_BLK_HDR_T_ID);
    bcmi_ltsw_issu_struct_info_report(unit,
                                      BCMI_HA_COMP_ID_FIELD,
                                      ha_blk_id,
                                      sizeof(bcmi_field_ha_blk_hdr_t),
                                      element_size,
                                      blk_size/element_size,
                                      id);
    /* Update root block ID bitmap */
    FP_HA_ROOT_BLK_ID_BMP_SET(unit, ha_blk_id);

    /* Initialize the block header. */
    blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_ptr;
    blk_hdr->free_blk_offset = sizeof(bcmi_field_ha_blk_hdr_t);
    blk_hdr->free_ha_element_count = num_ha_elements;
    blk_hdr->blk_size = ha_blk_size;
    blk_hdr->blk_type = blk_type;
    blk_hdr->signature = BCM_FIELD_HA_STRUCT_SIGN + ha_blk_id;

    *blk_id = ha_blk_id;
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_ha_blk_element_release(
    int unit,
    uint8_t blk_id,
    uint32_t blk_offset,
    bcmi_field_ha_blk_type_t blk_type)
{
    uint32_t oper_blk_size = 0;
    uint32_t num_ha_elements = 0;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bcmi_field_ha_hint_entry_t *hint_oper = NULL;

    SHR_FUNC_ENTER(unit);
    BCM_FIELD_HA_BLK_ID_VALIDATE(unit, blk_id);
    blk_hdr = (bcmi_field_ha_blk_hdr_t *)FP_HA_BLK_HDR_PTR(unit, blk_id);
    oper_blk_size = blk_hdr->blk_size - sizeof(bcmi_field_ha_blk_hdr_t);

    switch (blk_type) {
        case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER:
             group_oper =
               (bcmi_field_ha_group_oper_t *)((uint8_t *)blk_hdr + blk_offset);
             sal_memset(group_oper, 0x0, sizeof(bcmi_field_ha_group_oper_t));
             (group_oper->next).blk_id = blk_id;
             (group_oper->next).blk_offset = blk_hdr->free_blk_offset;
             num_ha_elements = oper_blk_size/sizeof(bcmi_field_ha_group_oper_t);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER:
             entry_oper =
               (bcmi_field_ha_entry_oper_t *)((uint8_t *)blk_hdr + blk_offset);
             sal_memset(entry_oper, 0x0, sizeof(bcmi_field_ha_entry_oper_t));
             (entry_oper->next).blk_id = blk_id;
             (entry_oper->next).blk_offset = blk_hdr->free_blk_offset;
             num_ha_elements = oper_blk_size/sizeof(bcmi_field_ha_entry_oper_t);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER:
             presel_oper =
               (bcmi_field_ha_presel_oper_t *)((uint8_t *)blk_hdr + blk_offset);
             sal_memset(presel_oper, 0x0, sizeof(bcmi_field_ha_presel_oper_t));
             (presel_oper->next).blk_id = blk_id;
             (presel_oper->next).blk_offset = blk_hdr->free_blk_offset;
             num_ha_elements = oper_blk_size/sizeof(bcmi_field_ha_presel_oper_t);
             break;
        case BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY:
             hint_oper =
               (bcmi_field_ha_hint_entry_t *)((uint8_t *)blk_hdr + blk_offset);
             sal_memset(hint_oper, 0x0, sizeof(bcmi_field_ha_hint_entry_t));
             (hint_oper->next).blk_id = blk_id;
             (hint_oper->next).blk_offset = blk_hdr->free_blk_offset;
             num_ha_elements = (oper_blk_size/sizeof(bcmi_field_ha_hint_entry_t));
             break;
        default:
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
             break;
    }

    blk_hdr->free_blk_offset = blk_offset;
    blk_hdr->free_ha_element_count += 1;

    /* Deallocate the blk if all the elements are free'd */
    if (blk_hdr->free_ha_element_count == num_ha_elements) {
        FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
        FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_ha_free_blk_id_get(
    int unit,
    bcmi_field_ha_blk_type_t blk_type,
    uint8_t *ha_blk_id)
{
    int id;
    void *ha_ptr = NULL;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    *ha_blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    for (id = 0; id < BCM_FIELD_HA_BLK_ID_MAX; id++) {
       ha_ptr = FP_HA_BLK_HDR_PTR(unit, id);
       if (ha_ptr == NULL) {
           continue;
       }
       blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_ptr;

       if ((blk_hdr->blk_type != blk_type) ||
           (blk_hdr->free_ha_element_count == 0)) {
           continue;
       }

       *ha_blk_id = id;
       break;
   }

   SHR_FUNC_EXIT();
}

static int
ltsw_field_ha_blk_type_num_pipe_get(
    int unit,
    bcmi_field_ha_blk_type_t blk_type,
    uint8_t *num_pipe)
{
    int oper_mode;
    bcmi_ltsw_field_stage_t stage_id;
    bcmint_field_stage_info_t *stage_info = NULL;
    SHR_FUNC_ENTER(unit);

    switch (blk_type) {
        case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER:
             stage_id = bcmiFieldStageIngress;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER:
             stage_id = bcmiFieldStageVlan;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER:
             stage_id = bcmiFieldStageEgress;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER:
             stage_id = bcmiFieldStageExactMatch;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER:
             stage_id = bcmiFieldStageFlowTracker;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY:
             /*
              * Hint database is shared across stages.
              * Specifying a stage here so that this
              * function doesn't return error.
              */
             stage_id = bcmiFieldStageIngress;
             break;
        default:
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
             break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                           stage_info,
                                           &oper_mode));

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        *num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    } else {
        *num_pipe = 1;
    }

exit:
    SHR_FUNC_EXIT();
}



int
bcmint_field_ha_blk_element_acquire(
    int unit,
    bcmi_field_ha_blk_type_t blk_type,
    void **ha_element,
    uint8_t *ha_blk_id,
    uint32_t *ha_blk_offset)
{
    uint8_t num_pipe = 0;
    uint32_t blk_size = 0;
    uint32_t blk_offset = 0;
    uint8_t blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bcmi_field_ha_hint_entry_t *hint_oper = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ha_element, SHR_E_PARAM);
    SHR_NULL_CHECK(ha_blk_id, SHR_E_PARAM);
    SHR_NULL_CHECK(ha_blk_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_ha_free_blk_id_get(unit, blk_type, ha_blk_id));

    blk_id = *ha_blk_id;
    if (blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {
        BCM_FIELD_HA_BLK_ID_VALIDATE(unit, blk_id);
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)FP_HA_BLK_HDR_PTR(unit, blk_id);
        if (blk_hdr->free_ha_element_count != 0) {
            blk_offset = blk_hdr->free_blk_offset;
            blk_hdr->free_ha_element_count -= 1;
        } else {
            blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
        }
    }

    /*
     * If it is new blk_id or no free elements in the given block, then
     * create a new HA block.
     */
    if (blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {
        /* retrieve group's operational mode and num pipes */
        SHR_IF_ERR_VERBOSE_EXIT(ltsw_field_ha_blk_type_num_pipe_get(unit,
                                                   blk_type,
                                                   &num_pipe));
        FP_HA_DYNAMIC_BLK_SIZE(unit, num_pipe, blk_type, blk_size);
        if (blk_size == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_ha_blk_create(unit,
                                   &blk_id,
                                   blk_size,
                                   blk_type));
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)FP_HA_BLK_HDR_PTR(unit, blk_id);
        blk_offset = blk_hdr->free_blk_offset;
        blk_hdr->free_ha_element_count -= 1;
    }

    switch (blk_type) {
        case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER:
             group_oper =
                (bcmi_field_ha_group_oper_t *)((uint8_t *)blk_hdr + blk_offset);
             /*
              * Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = group_oper->next.blk_offset;

             (group_oper->next).blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
             (group_oper->next).blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
             *ha_element = (void *)group_oper;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER:
             entry_oper =
                (bcmi_field_ha_entry_oper_t *)((uint8_t *)blk_hdr + blk_offset);
             /*
              * Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = entry_oper->next.blk_offset;

             (entry_oper->next).blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
             (entry_oper->next).blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
             *ha_element = (void *)entry_oper;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER:
        case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER:
             presel_oper =
                (bcmi_field_ha_presel_oper_t *)((uint8_t *)blk_hdr + blk_offset);
             /*
              * Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = presel_oper->next.blk_offset;

             (presel_oper->next).blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
             (presel_oper->next).blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
             *ha_element = (void *)presel_oper;
             break;
        case BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY:
             hint_oper =
                 (bcmi_field_ha_hint_entry_t *)((uint8_t *)blk_hdr + blk_offset);
             /*
              * Remove the HA element from free HA elements linked list
              * of the HA block and update the number of free HA elements.
              */
             blk_hdr->free_blk_offset = hint_oper->next.blk_offset;

             (hint_oper->next).blk_id = BCM_FIELD_HA_BLK_ID_INVALID;
             (hint_oper->next).blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
             *ha_element = (void *)hint_oper;
             break;

        default:
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
             break;
    }

    *ha_blk_id = blk_id;
    *ha_blk_offset = blk_offset;
exit:
    /* Release the acquired HA elements if there is an error. */
    if (SHR_FUNC_ERR() && (*ha_element != NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_ha_blk_element_release(unit, blk_id,
                                               blk_offset, blk_type));
    }
    SHR_FUNC_EXIT();
}


void ltsw_fp_ha_local_info_dump(int unit)
{
    int type, id;
    void *ha_ptr;
    bcmi_field_ha_blk_hdr_t *blk_hdr = NULL;
    ltsw_fp_ha_local_info_t *info;

    info = FP_HA_INFO(unit);
    if (info == NULL) {
        LOG_CLI(("Field HA not initialized!\n\r"));
        return;
    }

    LOG_CLI(("last_allocated_blk_id: [%d]\n\r", info->last_allocated_blk_id));
    for (type = 1; type < BCMI_FIELD_HA_BLK_TYPE_LAST_COUNT; type++) {
       LOG_CLI(("type:[%d] blk_id:[%d]\n\r", type, info->blk_id[type]));
    }

        LOG_CLI(("blk_id  blk_size  free_blk_offset  free_elem_cnt  signature  blk_type\n\r"));
        LOG_CLI(("------  --------  ---------------  -------------  ---------  ------------\n\r"));
    for (id = 0; id < BCM_FIELD_HA_BLK_ID_MAX; id++) {
        ha_ptr = FP_HA_BLK_HDR_PTR(unit, id);
        if (ha_ptr == NULL) {
            continue;
        }
        blk_hdr = (bcmi_field_ha_blk_hdr_t *)ha_ptr;

        LOG_CLI(("%3d", id));
        LOG_CLI(("%13d", blk_hdr->blk_size));
        LOG_CLI(("%17d", blk_hdr->free_blk_offset));
        LOG_CLI(("%15d", blk_hdr->free_ha_element_count));
        LOG_CLI(("%11x", blk_hdr->signature));
        switch(blk_hdr->blk_type)  {
          case BCMI_FIELD_HA_BLK_TYPE_ROOT:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"ROOT"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_PRESEL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_PRESEL_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_PRESEL_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_PRESEL_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_GROUP_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_GROUP_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_GROUP_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_GROUP_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_ENTRY_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_ENTRY_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_ENTRY_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_ENTRY_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_SBR_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_SBR_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_PRESEL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_PRESEL_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_PRESEL_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_PRESEL_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_GROUP_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_GROUP_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_GROUP_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_GROUP_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_ENTRY_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_ENTRY_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_ENTRY_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_ENTRY_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_VFP_SBR_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"VFP_SBR_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_PRESEL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_PRESEL_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_PRESEL_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_PRESEL_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_GROUP_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_GROUP_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_GROUP_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_GROUP_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_ENTRY_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_ENTRY_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_ENTRY_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_ENTRY_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_SBR_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_SBR_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_PRESEL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_PRESEL_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_PRESEL_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_PRESEL_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_GROUP_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_GROUP_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_GROUP_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_GROUP_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_ENTRY_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_ENTRY_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_ENTRY_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_ENTRY_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFP_SBR_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFP_SBR_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_COLOR_TBL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_PDD_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_COLOR_TBL_PDD_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_IFP_COLOR_TBL_SBR_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"IFP_COLOR_TBL_SBR_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_COLOR_TBL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_PDD_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_COLOR_TBL_PDD_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EFP_COLOR_TBL_SBR_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EFP_COLOR_TBL_SBR_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_PRESEL_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_PRESEL_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_PRESEL_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_PRESEL_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_GROUP_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_GROUP_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_GROUP_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_GROUP_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_INFO:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_ENTRY_INFO"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_HASH:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_ENTRY_HASH"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_ENTRY_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_ENTRY_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_EMFT_SBR_OPER:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"EMFT_SBR_OPER"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_HINT_LIST:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"HINT_LIST"));
               break;
          case BCMI_FIELD_HA_BLK_TYPE_HINT_ENTRY:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"HINT_ENTRY"));
               break;
          default:
               LOG_CLI(("%8d : %s", blk_hdr->blk_type,"#UNKNOWN#"));
               	break;
        }
        LOG_CLI(("\n\r"));
    }
}


