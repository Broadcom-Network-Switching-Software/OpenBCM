/*! \file rm_tcam_prio_eid.c
 *
 * Low Level Functions for Prioirty based TCAMs
 * This file contains low level functions for priority based TCAMs
 * to do opertaions like TCAM entry Insert/Lookup/Delete/Move.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include "rm_tcam_fp_utils.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

#define IS_ENTRY_INFO_FREE(entry_info, idx, prio, entry_type)             \
        ((entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) &&    \
        ((prio && entry_info[idx].entry_type == entry_type) ||            \
        entry_info[idx].entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID))

#define IS_ENTRY_INFO_KEY_TYPE(entry_info, idx, entry_type)         \
        ((entry_info[idx].entry_type == entry_type) &&              \
        (entry_info[idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID))

#define INDEX_IN_RANGE(a,b,c) (((a > b) && (a < c)) || (( a > c) && (a < b)))

#define IS_TARGET_ENTRY_ELIGIBLE(entry_info, idx, entry_type)               \
           ((entry_info[idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) && \
           ((entry_info[idx].entry_type == entry_type) ||                 \
           (entry_info[idx].entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID)))


/*******************************************************************************
 * Private variables
 */
static uint32_t entry_words[BCMDRD_CONFIG_MAX_UNITS]
                           [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS]
                           [BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t *entry_words_row[BCMDRD_CONFIG_MAX_UNITS]
                                [BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
/*******************************************************************************
 * Private functions
 */
/*
 * Get the pointer to tcam information of given TCAM LTID, pipe id.
 */
static int
bcmptm_rm_tcam_prio_eid_tcam_info_get(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    void *tcam_ptr = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_info_get(unit,
                                     iomd->ltid,
                                     -1,
                                     iomd->ltid_info,
                                     &tcam_ptr));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_info_get(unit,
                                     iomd->ltid,
                                     iomd->pt_dyn_info->tbl_inst,
                                     iomd->ltid_info,
                                     &tcam_ptr));
    }
    size = sizeof(bcmptm_rm_tcam_trans_info_t);
    iomd->tcam_info = (void *)((uint8_t *)tcam_ptr + size);
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_slice_info_get(int unit,
                           bcmptm_rm_tcam_entry_iomd_t *iomd,
                           bcmptm_rm_tcam_prio_eid_slice_info_t **slice_info)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_info, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type != LT_PT_TYPE_FP) {
        tcam_info = iomd->tcam_info;
        offset = tcam_info->slice_info_offset;
        *slice_info = (bcmptm_rm_tcam_prio_eid_slice_info_t *)
                       ((uint8_t *)tcam_info + offset);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_eid_tcam_flags_get(int unit,
                             bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_flags_get(unit,iomd));
    } else {
        tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)iomd->tcam_info;
        iomd->tcam_flags = tcam_info->flags;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_free_count_incr(int unit,
                          bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_prio_eid_slice_info_t *slice_info = NULL;
    uint8_t sid_row;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_free_count_incr(unit,
                                                     iomd->target_index,
                                                     iomd));
    } else {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);
        if (entry_attrs->per_slice_entry_mgmt == FALSE) {
            tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)iomd->tcam_info;
            tcam_info->free_entries += 1;
        } else {
            sid_row = entry_attrs->slice_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_slice_info_get(unit,
                                                    iomd,
                                                    &slice_info));
            slice_info[sid_row].free_entries += 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_entry_free_count_decr(int unit,
                          bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_prio_eid_slice_info_t *slice_info = NULL;
    uint8_t sid_row;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_free_count_decr(unit,
                                                     iomd->target_index,
                                                     iomd));
    } else {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);
        if (entry_attrs->per_slice_entry_mgmt == FALSE) {
            tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)iomd->tcam_info;
            tcam_info->free_entries -= 1;
        } else {
            sid_row = entry_attrs->slice_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_slice_info_get(unit,
                                                    iomd,
                                                    &slice_info));
            slice_info[sid_row].free_entries -= 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_eid_pipe_count_get(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_pipe_count_get(unit,iomd));
    } else {
        iomd->pipe_count = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_calc_target_idx_from_global_idx(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd,
                               int global_idx,
                               uint32_t *target_idx)
{
    uint16_t idx = 0;
    int min_global_idx = 0;
    int max_global_idx = 0;
    uint8_t num_width_parts = 1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *) iomd->entry_attrs;

    *target_idx = 0;
    /* For other than FP Main TCAM, target_idx and global_idx are same */
    if (iomd->ltid_info->pt_type != LT_PT_TYPE_FP ||
        (iomd->ltid_info->pt_type == LT_PT_TYPE_FP &&
        (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)))) {

        *target_idx = global_idx;
    } else {
        if (iomd->entry_info_arr.num_width_parts != 1) {
             /* Width parts equal to primary group width parts */
             num_width_parts = iomd->entry_info_arr.pri_group_width_parts;
        }
        for (idx = 0; idx < iomd->entry_info_arr.num_depth_parts; idx++) {
            /* Per depth part, width is width_parts * slice segment size */
            max_global_idx = iomd->entry_hash_size * num_width_parts;
            if (global_idx >= min_global_idx &&
                global_idx < max_global_idx) {
                /* In a particular depth part, global idx is remainder of depth size */
                *target_idx += global_idx % iomd->entry_info_arr.entry_info_depth[idx];
                break;
            } else {
                /* If not in this depth part, add the depth size to target_idx */
                *target_idx += iomd->entry_info_arr.entry_info_depth[idx];
                /* Move to the next depth part */
                global_idx -=  iomd->entry_hash_size * num_width_parts;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_calc_global_idx_from_target_idx(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd,
                               int target_idx,
                               uint32_t *global_idx)
{
    uint16_t idx = 0;
    int min_global_idx = 0;
    int max_global_idx = 0;
    uint8_t num_width_parts = 1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *) iomd->entry_attrs;
    /* For other than FP Main TCAM, target_idx and global_idx are same */
    if (iomd->ltid_info->pt_type != LT_PT_TYPE_FP ||
        (iomd->ltid_info->pt_type == LT_PT_TYPE_FP &&
        (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)))) {

        *global_idx = target_idx;
    } else {
        if (iomd->entry_info_arr.num_width_parts != 1) {
            /* Width parts equal to primary group width parts */
            num_width_parts = iomd->entry_info_arr.pri_group_width_parts;
        }
        for (idx = 0; idx < iomd->entry_info_arr.num_depth_parts; idx++) {
            max_global_idx += iomd->entry_info_arr.entry_info_depth[idx];
            if (target_idx >= min_global_idx &&
                target_idx < max_global_idx) {
                *global_idx = (target_idx - min_global_idx);
                break;
            }
            min_global_idx += iomd->entry_info_arr.entry_info_depth[idx];
        }
        /* If not in this depth part, add the depth size * num_parts to global_idx */
        *global_idx = *global_idx + idx * iomd->entry_hash_size * num_width_parts;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_entry_info_get_from_array(int unit,
                              bcmptm_rm_tcam_entry_iomd_t *iomd,
                              int global_idx,
                              bcmptm_rm_tcam_prio_eid_entry_info_t **einfo,
                              uint32_t *local_idx)
{
    uint16_t idx = 0;
    int min_global_idx = 0;
    int max_global_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(einfo, SHR_E_PARAM);

    for (idx = 0; idx < iomd->entry_info_arr.num_depth_parts; idx++) {
        max_global_idx += iomd->entry_info_arr.entry_info_depth[idx];
        if (global_idx >= min_global_idx &&
            global_idx < max_global_idx) {
            *local_idx = (global_idx - min_global_idx);

            sal_memcpy(einfo, iomd->entry_info_arr.entry_info[idx],
                sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t *)
                    * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS);
            break;
        }
        min_global_idx += iomd->entry_info_arr.entry_info_depth[idx];
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_eid_find_loops(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t idx = 0, idx2 = 0;
    int *entry_hash = NULL;
    int offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int first_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int offset2 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t num_offsets = 0;
    uint32_t num_entries = 0;
    uint32_t entry_hash_size = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    int num_parts = 0;
    int entry_id = 0;
    int count = 0;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_NONE);
    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *) iomd->entry_attrs;

    entry_hash = iomd->entry_hash_ptr;
    entry_hash_size = iomd->entry_hash_size;
    entry_info = iomd->entry_info_arr.entry_info[0][0];

    if (iomd->ltid_info->pt_type != LT_PT_TYPE_FP ||
        (iomd->ltid_info->pt_type == LT_PT_TYPE_FP &&
        (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)))) {
        num_entries = iomd->num_entries;
    } else {
        if (iomd->entry_info_arr.num_width_parts == 1) {
            num_entries =
                iomd->entry_info_arr.num_depth_parts * iomd->entry_hash_size;
        } else {
            num_entries = iomd->entry_hash_size *
                iomd->entry_info_arr.pri_group_width_parts * iomd->entry_info_arr.num_depth_parts;
        }
    }

    for (idx = 0; idx < entry_hash_size; idx++) {

         first_offset = (int)entry_hash[idx];

         if (first_offset < (int)num_entries &&
             first_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
             offset1 = (int)(entry_info[first_offset].offset);
             if (entry_info[first_offset].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             } else if ((entry_info[first_offset].entry_id % iomd->entry_hash_size ) != idx ) {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             } else {
                entry_id = entry_info[first_offset].entry_id;
                num_parts = entry_info[first_offset].entry_type;
                index = first_offset;
                do {
                    if (entry_info[index].entry_id != entry_id) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    index += iomd->entry_hash_size;
                    count++;
                } while (count < num_parts);
             }
         }

         num_offsets = 0;
         while (offset1 != BCMPTM_RM_TCAM_OFFSET_INVALID) {

             if (offset1 >= (int)num_entries) {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             }
             if (entry_info[offset1].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             } else if ((entry_info[offset1].entry_id % iomd->entry_hash_size ) != idx ) {
                 SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
             } else {
                entry_id = entry_info[offset1].entry_id;
                num_parts = entry_info[offset1].entry_type;
                index = offset1;
                do {
                    if (entry_info[index].entry_id != entry_id) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    index += iomd->entry_hash_size;
                    count++;
                } while (count < num_parts);
             }

             offset2 = (int)(entry_info[first_offset].offset);

             for (idx2 = 0; idx2 < num_offsets; idx2++) {

                 if (offset2 >= (int)num_entries) {
                     SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                 }

                 if (offset1 == offset2) {
                      SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                 }
                 offset2 = (int)(entry_info[offset2].offset);
             }

             num_offsets++;
             offset1 = (int)(entry_info[offset1].offset);
         }
         offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FUNC_EXIT();
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_entry_clear_hw(int unit,
                              uint32_t target_index,
                              bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t part = 0;
    uint32_t rm_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    /* Update the HW with NULL data. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        sal_memset(entry_words[unit][part], 0,
                   sizeof(entry_words[unit][part]));
        entry_words_row[unit][part] = entry_words[unit][part];
    }


    if (iomd->tcam_flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {
        /* Write the entry words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     target_index,
                                     entry_words_row[unit]));
    } else {
        /*  First write the associated data to index. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     target_index,
                                     entry_words_row[unit]));

        /* Next write the key words to index. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_write(unit,
                                     iomd,
                                     rm_flags,
                                     target_index,
                                     entry_words_row[unit]));
    }
exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_prio_eid_iomd_update(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_group_t *fg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *) iomd->entry_attrs;

    iomd->multi_mode = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_num_entries_get(unit, iomd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_free_count_get(unit, iomd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_tcam_flags_get(unit, iomd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_pipe_count_get(unit, iomd));

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_info_get(unit,
                                                   iomd->ltid,
                                                   iomd->ltid_info,
                                                   iomd->entry_attrs,
                                                   &entry_info));
    } else {
        tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)iomd->tcam_info;
        entry_info = NULL;
        offset = tcam_info->entry_info_offset;
        entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t *)
                     ((uint8_t *)tcam_info + offset);
    }
    if (iomd->ltid_info->pt_type != LT_PT_TYPE_FP ||
        (iomd->ltid_info->pt_type == LT_PT_TYPE_FP &&
        (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)))) {
        iomd->entry_info_arr.num_depth_parts = 1;
        iomd->entry_info_arr.num_width_parts = 1;
        iomd->entry_info_arr.entry_info[0][0] = (void *)entry_info;
        iomd->entry_info_arr.entry_info_depth[0] = iomd->num_entries;
        iomd->num_parts = 1;
        iomd->entry_type = 1;
        iomd->num_entries_ltid = iomd->num_entries - iomd->free_entries;
        iomd->reserved_index = -1;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_iomd_entry_info_arr_update(unit,
                                                          entry_info,
                                                          iomd));
            fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
            if (fg->reserve_entry_index_exists) {
                iomd->reserved_index = fg->reserved_entry_index;
            } else {
                iomd->reserved_index = -1;
            }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_hash_table_get(unit, iomd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_hash_table_size_get(unit, iomd));

    iomd->entry_hash_value =
          ((iomd->req_info->entry_id) % (iomd->entry_hash_size));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_iomd_init(int unit,
                                  uint64_t req_flags,
                                  uint32_t cseq_id,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                                  bcmptm_op_type_t req_op,
                                  bcmptm_rm_tcam_req_t *req_info,
                                  bcmptm_rm_tcam_rsp_t *rsp_info,
                                  bcmltd_sid_t *rsp_ltid,
                                  uint32_t *rsp_flags,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (req_info->entry_attrs != NULL) {
        entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)(req_info->entry_attrs);
    } else {
         SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    iomd->trans_id = cseq_id;
    iomd->req_flags = req_flags;
    iomd->rsp_flags = rsp_flags;
    iomd->ltid = ltid;
    iomd->ltid_info = ltid_info;
    iomd->pt_dyn_info = pt_dyn_info;
    iomd->req_op = req_op;
    iomd->req_info = req_info;
    iomd->rsp_info = rsp_info;
    iomd->rsp_ltid = rsp_ltid;
    iomd->entry_attrs = req_info->entry_attrs;
    iomd->pipe_id = entry_attr->pipe_id;
    iomd->stage_id = entry_attr->stage_id;
    iomd->reserved_index = -1;

    rv = (bcmptm_rm_tcam_prio_eid_tcam_info_get(unit, iomd));

    /*
     * When No resources are allocated, tcam_info for that LT is not created.
     * In this case initialize group, stage and slice pointers to NULL.
     */
    if (rv == SHR_E_NOT_FOUND) {
        iomd->group_ptr = NULL;
        iomd->stage_fc = NULL;
        iomd->slice_fc = NULL;
    } else if (ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                              ltid,
                                              ltid_info,
                                              entry_attr->pipe_id,
                                              entry_attr->stage_id,
                                              &stage_fc));
        iomd->stage_fc = (void *)stage_fc;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_slice_info_get(unit,
                                              ltid,
                                              ltid_info,
                                              &iomd->num_slices,
                                              entry_attr->pipe_id,
                                              entry_attr->stage_id,
                                              &slice_fc));
        iomd->slice_fc = (void *)slice_fc;
        rv = bcmptm_rm_tcam_fp_group_info_get(unit,
                                              ltid,
                                              ltid_info,
                                              entry_attr->pipe_id,
                                              entry_attr->group_id,
                                              stage_fc,
                                              &group_ptr);
        if (rv == SHR_E_NOT_FOUND) {
            iomd->group_ptr = NULL;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            iomd->group_ptr = (void *)group_ptr;
            if (group_ptr->reserve_entry_index_exists) {
                iomd->reserved_index = group_ptr->reserved_entry_index;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Fetch the address to TCAM entry hash of the given Logical Table id.
 * This address points to the address of hash zero. To get the address to
 * any other valid hash, increment this start pointer by hash times
 * as memory created to hold the hash information of all valid hash
 * values is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [out] entry_hash Double pointer to hold pointer to start address
 *              of TCAM hash information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
int
bcmptm_rm_tcam_prio_eid_hash_table_get(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_hash_get(unit,iomd));
    } else {
        tcam_info = iomd->tcam_info;
        offset = tcam_info->entry_hash_offset;
        iomd->entry_hash_ptr = (int *)((uint8_t *)tcam_info + offset);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_free_count_get(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_prio_eid_slice_info_t *slice_info = NULL;
    uint8_t sid_row;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_free_count_get(unit,iomd));
    } else {
        entry_attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);
        if (entry_attrs->per_slice_entry_mgmt == FALSE) {
            tcam_info = iomd->tcam_info;
            iomd->free_entries = tcam_info->free_entries;
        } else {
            sid_row = entry_attrs->slice_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_slice_info_get(unit,
                                                    iomd,
                                                    &slice_info));
            iomd->free_entries = slice_info[sid_row].free_entries;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_num_entries_get(int unit,
                          bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_valid_count_get(unit,iomd));
    } else {
        tcam_info = iomd->tcam_info;
        iomd->num_entries = tcam_info->num_entries;
        iomd->num_entries_ltid = tcam_info->num_entries_ltid;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_hash_table_size_get(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_fp_entry_hash_size_get(unit,iomd));
    } else {
        tcam_info = iomd->tcam_info;
        iomd->entry_hash_size = tcam_info->entry_hash_size;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Move TCAM entry from one index to the other index.
 * Both HW and SW state will be updated.
 */
int
bcmptm_rm_tcam_prio_eid_entry_move(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint8_t partition,
                                   uint32_t prefix,
                                   uint32_t from_idx,
                                   uint32_t to_idx,
                                   void *attrs,
                                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int *entry_hash = NULL;
    int orig_tbl_inst = -1;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint8_t part = 0;
    uint16_t hash_val;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *curr_entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *from_entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *to_entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *from_entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *to_entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint32_t from_einfo_idx = 0;
    uint32_t to_einfo_idx = 0;
    uint32_t global_idx = 0;
    uint8_t pipe = 0;
    uint8_t pipe_count = 0;
    uint8_t num_parts = 0;
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    num_parts = iomd->num_parts;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  from_idx,
                                                  from_entry_info_ptr,
                                                  &from_einfo_idx));
    from_entry_info = from_entry_info_ptr[0];

    /* If the entry is empty no need to move. */
    if (from_entry_info[from_einfo_idx].entry_id ==
                                        BCMPTM_RM_TCAM_EID_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  to_idx,
                                                  to_entry_info_ptr,
                                                  &to_einfo_idx));
    to_entry_info = to_entry_info_ptr[0];

    /* Fill entry words with all zeros. */
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
         sal_memset(entry_words[unit][part], 0,
                    sizeof(entry_words[unit][part]));
         entry_words_row[unit][part] = entry_words[unit][part];
    }

    if (from_entry_info[from_einfo_idx].global_to_all_pipes == FALSE) {
        pipe_count = 1;
    } else {
        pipe_count = ltid_info->rm_more_info->pipe_count;
    }

    orig_tbl_inst = iomd->pt_dyn_info->tbl_inst;
    for (pipe = 0; pipe < pipe_count; pipe++) {

        if (from_entry_info[from_einfo_idx].global_to_all_pipes == TRUE) {
            pt_dyn_info->tbl_inst = pipe;
        }

        if (iomd->tcam_flags &  BCMPTM_RM_TCAM_F_AGGR_VIEW) {

            rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;

            /* Read the entry words from from_idx. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_read(unit,
                                        iomd,
                                        rm_flags,
                                        from_idx,
                                        entry_words_row[unit]));
            /* Write the entry words to to_idx. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_write(unit,
                                         iomd,
                                         rm_flags,
                                         to_idx,
                                         entry_words_row[unit]));
        } else {
            /* First move associated data portion of LTID. */
            rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;

            /* Read the Associated data words from PTcache. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_read(unit,
                                        iomd,
                                        rm_flags,
                                        from_idx,
                                        entry_words_row[unit]));

            /* Write the associated data words to PTcache.  */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_write(unit,
                                         iomd,
                                         rm_flags,
                                         to_idx,
                                         entry_words_row[unit]));

            /* Now move TCAM only portion of LTID. */
            rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;

            /* Read the TCAM Only words from PTCache. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_read(unit,
                                        iomd,
                                        rm_flags,
                                        from_idx,
                                        entry_words_row[unit]));

            /* Write the TCAM only words to PTCache. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_write(unit,
                                         iomd,
                                         rm_flags,
                                         to_idx,
                                         entry_words_row[unit]));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_clear_hw(unit, from_idx, iomd));

    iomd->pt_dyn_info->tbl_inst = orig_tbl_inst;

    if (iomd->hw_only == TRUE) {
        SHR_EXIT();
    }

    /* Update the SW state in three steps. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  from_idx,
                                                  from_entry_info_ptr,
                                                  &from_einfo_idx));
    from_entry_info = from_entry_info_ptr[0];

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  to_idx,
                                                  to_entry_info_ptr,
                                                  &to_einfo_idx));
    count = 0;
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        do {
            to_entry_info = to_entry_info_ptr[count];
            if (to_entry_info[to_einfo_idx].entry_id != -1) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            count++;
        } while (count < iomd->num_parts);
    }
    to_entry_info = to_entry_info_ptr[0];
    part = 0;

    do {
        from_entry_info = from_entry_info_ptr[part];
        to_entry_info = to_entry_info_ptr[part];
        /* 1. Update the TCAM entry information at to_idx. */
        to_entry_info[to_einfo_idx].entry_id =
                     from_entry_info[from_einfo_idx].entry_id;
        to_entry_info[to_einfo_idx].entry_pri =
                     from_entry_info[from_einfo_idx].entry_pri;
        if (part == 0) {
            to_entry_info[to_einfo_idx].offset =
                     from_entry_info[from_einfo_idx].offset;
        } else {
            to_entry_info[to_einfo_idx].offset =
                     BCMPTM_RM_TCAM_OFFSET_INVALID;
        }

        to_entry_info[to_einfo_idx].global_to_all_pipes =
                     from_entry_info[from_einfo_idx].global_to_all_pipes;
        to_entry_info[to_einfo_idx].entry_type =
                     from_entry_info[from_einfo_idx].entry_type;

        /* 2. Update the the TCAM SW hash table. */
        if (part == 0) {
            hash_val = ((to_entry_info[to_einfo_idx].entry_id)
                            % (iomd->entry_hash_size));
            entry_hash = iomd->entry_hash_ptr;
            curr_offset = entry_hash[hash_val];
            curr_entry_info = iomd->entry_info_arr.entry_info[0][0];
            while (curr_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
                if (curr_entry_info[curr_offset].entry_id !=
                              to_entry_info[to_einfo_idx].entry_id) {
                    prev_offset = curr_offset;
                    curr_offset = curr_entry_info[curr_offset].offset;
                } else {
                    break;
                }
            }
            /* get global index from to_idx */
            SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_calc_global_idx_from_target_idx(unit,
                                                      iomd,
                                                      to_idx,
                                                      &global_idx));
            if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
                curr_entry_info[prev_offset].offset = global_idx;
            } else {
                entry_hash[hash_val] = global_idx;
            }
        }

        /* 3. Clear the TCAM entry information at from_idx. */
        from_entry_info[from_einfo_idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        from_entry_info[from_einfo_idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        from_entry_info[from_einfo_idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        from_entry_info[from_einfo_idx].global_to_all_pipes = 0;
        from_entry_info[from_einfo_idx].entry_type = iomd->entry_type;

        part++;
    } while (part != num_parts);

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_find_loops(unit, iomd));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Write the entry words to a particulat TCAM index.
 * Updates both HW and SW state
 */
int
bcmptm_rm_tcam_prio_eid_entry_write(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int *entry_hash = NULL;
    uint16_t hash_val = 0;
    uint32_t rm_flags = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    uint32_t einfo_idx = 0;
    uint32_t global_idx = 0;
    uint8_t part = 0;
    uint8_t num_parts = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->sw_only == FALSE) {
    /* Update the HW */
        if (iomd->tcam_flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {
            /* Write the entry words to index. */
            rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_write(unit,
                                         iomd,
                                         rm_flags,
                                         iomd->target_index,
                                         iomd->req_info->ekw));
        } else {
            /*  First write the associated data to index. */
            rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_write(unit,
                                         iomd,
                                         rm_flags,
                                         iomd->target_index,
                                         iomd->req_info->edw));

            /* Next write the key words to index. */
            rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_hw_write(unit,
                                         iomd,
                                         rm_flags,
                                         iomd->target_index,
                                         iomd->req_info->ekw));
        }

        if (iomd->hw_only == TRUE) {
            SHR_EXIT();
        }
    }

    num_parts = iomd->num_parts;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                      iomd,
                                                      iomd->target_index,
                                                      entry_info_ptr,
                                                      &einfo_idx));
    part = 0;
    do {
        entry_info = entry_info_ptr[part];
        /* Update the SW state in two steps. */
        /* 1. Update the TCAM entry information. */
        if (entry_info[einfo_idx].entry_id != BCMPTM_RM_TCAM_EID_INVALID) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        entry_info[einfo_idx].entry_id = iomd->req_info->entry_id;
        entry_info[einfo_idx].entry_pri = iomd->req_info->entry_pri;
        entry_info[einfo_idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[einfo_idx].entry_type = iomd->entry_type;

        if (iomd->req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL) {
            entry_info[einfo_idx].global_to_all_pipes = 1;
        } else {
            entry_info[einfo_idx].global_to_all_pipes = 0;
        }

        /* 2. Update the TCAM SW hash table. */
        if (part == 0) {
            hash_val = iomd->entry_hash_value;
            entry_hash = iomd->entry_hash_ptr;

            entry_info[einfo_idx].offset = entry_hash[hash_val];
            /* calculate global index  from target_idx*/
            SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_calc_global_idx_from_target_idx(unit,
                                                      iomd,
                                                      iomd->target_index,
                                                      &global_idx));
            entry_hash[hash_val] = global_idx;
        }
        part++;
    } while (part != num_parts);

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_find_loops(unit, iomd));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Read the entry words from a particular TCAM index.
 */
int
bcmptm_rm_tcam_prio_eid_entry_read(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t rm_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    /* Read the TCAM entry words. */
    if (iomd->tcam_flags & BCMPTM_RM_TCAM_F_AGGR_VIEW) {
        /*
         * Read the  the entry words from index using
         * aggregated view.
         */
        rm_flags = BCMPTM_RM_TCAM_F_AGGR_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    iomd->target_index,
                                    iomd->rsp_info->rsp_ekw));
    } else {
        /* Read the  the entry words from index using TCAM only view. */
        rm_flags = BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    iomd->target_index,
                                    iomd->rsp_info->rsp_ekw));

        /* Read the  the data words from index using DATA only view. */
        rm_flags = BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_hw_read(unit,
                                    iomd,
                                    rm_flags,
                                    iomd->target_index,
                                    iomd->rsp_info->rsp_edw));
    }

    if (iomd->pt_dyn_info->tbl_inst == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        iomd->rsp_info->rsp_entry_pipe = 0;
    } else {
        iomd->rsp_info->rsp_entry_pipe = iomd->pt_dyn_info->tbl_inst;
    }

exit:
    SHR_FUNC_EXIT();
}



/*
 * Clear a TCAM entry at specific index. Involves writing all zeros to a
 * particular TCAM index. Updates both HW and SW state.
 */
int
bcmptm_rm_tcam_prio_eid_entry_clear(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int *entry_hash = NULL;
    int curr_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int prev_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t clear_einfo_offset = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *clear_entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *curr_entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *clear_entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint16_t hash_val;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    num_parts = iomd->num_parts;

    /* if entry is invalid no need to clear it. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                       iomd,
                                       iomd->target_index,
                                       clear_entry_info_ptr,
                                       &clear_einfo_offset));

    clear_entry_info = clear_entry_info_ptr[0];

    if ((clear_entry_info[clear_einfo_offset].entry_id ==
        BCMPTM_RM_TCAM_EID_INVALID) && iomd->hw_only == FALSE) {
        SHR_EXIT();
    }

    if (iomd->sw_only == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_clear_hw(unit,
                                                 iomd->target_index,
                                                 iomd));
        if (iomd->hw_only == TRUE) {
            SHR_EXIT();
        }
    }

    part = 0;
    do {
        /* Update the SW state in two steps. */
        clear_entry_info = clear_entry_info_ptr[part];

        if (part == 0) {
            /* 1. Update the TCAM SW hash table. */
            hash_val = iomd->entry_hash_value;
            entry_hash = iomd->entry_hash_ptr;

            curr_offset = entry_hash[hash_val];
            curr_entry_info = iomd->entry_info_arr.entry_info[0][0];
            while (curr_entry_info[curr_offset].entry_id !=
                   iomd->req_info->entry_id) {
                prev_offset = curr_offset;
                curr_offset = curr_entry_info[curr_offset].offset;
            }
            if (prev_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
                curr_entry_info[prev_offset].offset =
                                curr_entry_info[curr_offset].offset;
            } else {
                entry_hash[hash_val] =
                          curr_entry_info[curr_offset].offset;
            }
        }
        /* 2. Update the TCAM entry information. */
        clear_entry_info[clear_einfo_offset].entry_id =
                                             BCMPTM_RM_TCAM_EID_INVALID;
        clear_entry_info[clear_einfo_offset].entry_pri =
                                             BCMPTM_RM_TCAM_PRIO_INVALID;
        clear_entry_info[clear_einfo_offset].offset =
                                             BCMPTM_RM_TCAM_OFFSET_INVALID;
        clear_entry_info[clear_einfo_offset].global_to_all_pipes = FALSE;
        clear_entry_info[clear_einfo_offset].entry_type =
                                             BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
        part++;
    } while (part != num_parts);


    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_find_loops(unit, iomd));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Insert the new TCAM entry or update the existing TCAM entry in
 * priority based TCAMs.
 *
 * Algorithm:
 *  1.Application provides TCAM Entry Data.
 *    1.1.For priority based TCAM; Entry Data contains
 *         KEY + MASK + POLICY + entry_id + PRIOIRTY.
 *  2.Do Lookup for the given TCAM entry Data to find the entry index.
 *    2.1.For priority based TCAMs, lookup happens with entry_id.
 *  3.If entry index is Non Negative (meaning entry existed)?
 *    3.1.If application is asking for Entry Replace?
 *        3.1.1.Call south bound interface to give the write command with
 *              update entry data (New KEY + New MASK + New POLICY) at
 *              entry index.
 *        3.1.2. Return SUCCESS
 *    3.2.If application is not asking for Entry Replace?
 *        3.2.1.Return error code ENTRY EXISTS back to upper layers.
 *  4.If entry index is Negative (meaning entry does not exist)?
 *    4.1.Check if at least one free slot is available in TCAM by checking TCAM
 *        free indexes status.
 *    4.2.If at least one free slot is available run the algorithm to find the
 *        target_index to use for the entry.
 *        4.2.1.Search the last entry with priority greater than given
 *              priority. That entries index + 1 become the target_index.
 *        4.2.2.If target_index is free then insert the entry at that free slot.
 *        4.2.3.If target_index is not free, find first free slot after and
 *              before the target_index.
 *        4.2.4.Whichever direction (up/down) takes less number of moves then
 *              move all entries from target_index to free index move up/down
 *              one index to create a free slot for the target TCAM entry.
 *        4.2.5.Call the south bound interface to give the write command with
 *              new entry data (KEY + MASK + POLICY) at new index allocated.
 *        4.2.6.Return SUCCESS.
 *    4.3.If at least one free index is not available?
 *        4.3.1.Return error code TABLE FULL.
 */
int
bcmptm_rm_tcam_prio_eid_entry_insert(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int rv = SHR_E_NONE;
    uint32_t found_index = -1;
    uint16_t target_idx = -1;
    uint8_t reserved_used = 0;

    SHR_FUNC_ENTER(unit);

    if (iomd->req_info->entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    iomd->not_direct_lookup = TRUE;

    /* Check if entry already exists or not. */
    if (iomd->req_info->same_key == true) {
        /* If already looked up, use the results */
        if (same_key_info[unit].index != -1) {
            found_index = same_key_info[unit].index;
            iomd->target_index = found_index;
            rv = SHR_E_NONE;
        } else {
            found_index = -1;
            rv = SHR_E_NOT_FOUND;
        }
        /* For prio update check */
        iomd->rsp_info->rsp_entry_pri = same_key_info[unit].priority;

    } else {
        /* Check if entry id exists or not. */
        rv = bcmptm_rm_tcam_prio_eid_entry_lookup(unit,
                                              iomd->req_flags,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pt_dyn_info,
                                              iomd->req_info,
                                              iomd->rsp_info,
                                              iomd->rsp_ltid,
                                              iomd->rsp_flags,
                                              &found_index,
                                              iomd);

        if (SHR_FAILURE(rv) && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
    }

    if (rv == SHR_E_NONE) {
        /* Update the TCAM entry */
        SHR_ERR_EXIT
            (bcmptm_rm_tcam_prio_entry_update(unit,iomd));
    } else {
       /* Find Index to Insert the TCAM entry */
       iomd->prio_change = FALSE;
       rv = (bcmptm_rm_tcam_prio_eid_entry_index_allocate(unit,
                                        iomd,
                                        &reserved_used));

       /* No free entries */
       if (rv != SHR_E_NONE) {
           SHR_ERR_EXIT(rv);
       }
       target_idx = iomd->target_index;

       /* Insert the new entry at target_idx. */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd));

       /* Read the inserted data to the response */
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));


       /* Fill in the entry attributes to the response */
       iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;

       if (iomd->req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL &&
           iomd->req_info->same_key == 0) {
           same_key_info[unit].index = target_idx;
           same_key_info[unit].priority = iomd->req_info->entry_pri;
           same_key_info[unit].prev_index = -1;
       }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_update_reserve_entry_index(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int old_rsvd_index,
                                        int new_rsvd_index)
{
    bcmptm_rm_tcam_fp_group_t *fg = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    int grp_idx;
    uint32_t local_idx = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    int part = 0;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_fp_group_t *orig_fg = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    fg = iomd->group_ptr;
    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;
    orig_fg = iomd->group_ptr;

    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    if (fg->reserved_entry_index == old_rsvd_index) {

        /* Mark old reserved entry as invalid type if it is empty */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  old_rsvd_index,
                                                  entry_info_ptr,
                                                  &local_idx));

        for (part = 0; part < iomd->num_parts; part++) {
           entry_info = entry_info_ptr[part];
            if (entry_info[local_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID &&
               entry_info[local_idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) {
               entry_info[local_idx].entry_type = iomd->entry_type;

           }
        }

        /* Mark new reserved entry as iomd->entry_type */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  new_rsvd_index,
                                                  entry_info_ptr,
                                                  &local_idx));
        for (part = 0; part < iomd->num_parts; part++) {
            entry_info = entry_info_ptr[part];
            if (entry_info[local_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID &&
               entry_info[local_idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) {
               entry_info[local_idx].entry_type = iomd->entry_type;
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }

        /* Get the primary group from current group */
        grp_idx = slice_fc[fg->primary_slice_id].primary_grp_id;
        do {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_group_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->entry_attrs->pipe_id,
                                                  grp_idx,
                                                  iomd->stage_fc,
                                                  &fg));
            if (fg->reserve_entry_index_exists &&
                (fg->reserved_entry_index == old_rsvd_index) &&
                (orig_fg->group_mode == fg->group_mode)) {
                fg->reserved_entry_index = new_rsvd_index;
            }
            /* Update reserve entry for all shared groups */
            grp_idx = fg->next_shared_group_id;
        } while (grp_idx != -1);
        /* Update reserved index in iomd structure */
        iomd->reserved_index = new_rsvd_index;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 *  Move the range of TCAM entries one index up or down.
 */
int
bcmptm_rm_tcam_prio_eid_entry_move_range(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint8_t partition,
                                uint32_t idx_start,
                                uint32_t idx_end,
                                int8_t entry_type,
                                void *attrs,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int rv = SHR_E_NONE;
    int idx = 0;
    uint32_t count = 0;
    uint32_t local_to_idx = 0;
    uint32_t local_from_idx = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info1 = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info2 = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint32_t count_idx = 0;
    int reserved_index = -1;
    bool reserved_move = false;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    /* Parameters Check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    reserved_index = iomd->reserved_index;

    /* Shift entries in down ward direction(i.e. one index up). */
    if (idx_end > idx_start) {
        for (idx = (idx_end - 1); (idx >= (int)idx_start)
             && (idx < (int)idx_end); idx--) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  idx,
                                                  entry_info_ptr,
                                                  &local_from_idx));
             entry_info1 = entry_info_ptr[0];

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  idx + 1+ count,
                                                  entry_info_ptr,
                                                  &local_to_idx));
            if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                count_idx = 0;
                do {
                    entry_info2 = entry_info_ptr[count_idx];
                    if (entry_info2[local_to_idx].entry_id !=
                                    BCMPTM_RM_TCAM_EID_INVALID) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    count_idx++;
                } while (count_idx < iomd->num_parts);
            }
            entry_info2 = entry_info_ptr[0];
            if ((entry_info1[local_from_idx].entry_type == entry_type) &&
                 (entry_info2[local_to_idx].entry_type == entry_type)) {
                if (reserved_index == (int32_t)idx) {
                    if (entry_info1[local_from_idx].entry_pri ==
                        BCMPTM_RM_TCAM_PRIO_INVALID) {
                        reserved_move = true;
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }

                rv = bcmptm_rm_tcam_entry_move(unit,
                                               ltid,
                                               ltid_info,
                                               partition,
                                               idx ,
                                               idx + 1+ count,
                                               attrs,
                                               pt_dyn_info,
                                               iomd);
                if (reserved_move == true) {
                    /* Update FP group's and shared groups reserved index */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_update_reserve_entry_index(unit,
                                                          iomd,
                                                          reserved_index,
                                                          (idx + 1 + count)));
                    reserved_index = idx + 1 + count;
                    reserved_move = false;
                }
                count = 0;
            } else {
                count++;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }
    /* Shift entries in upward direction(i.e. one index down). */
    if (idx_end < idx_start) {
        for (idx = idx_end + 1; idx <
             (int)idx_start + 1; idx++) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  idx,
                                                  entry_info_ptr,
                                                  &local_from_idx));
             entry_info1 = entry_info_ptr[0];
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                                  iomd,
                                                  idx - 1 - count,
                                                  entry_info_ptr,
                                                  &local_to_idx));
            if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                count_idx = 0;
                do {
                    entry_info2 = entry_info_ptr[count_idx];
                    if (entry_info2[local_to_idx].entry_id !=
                                    BCMPTM_RM_TCAM_EID_INVALID) {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    count_idx++;
                } while (count_idx < iomd->num_parts);
            }
            entry_info2 = entry_info_ptr[0];
            if ((entry_info1[local_from_idx].entry_type == entry_type) &&
                 (entry_info2[local_to_idx].entry_type == entry_type)) {
                if (reserved_index == (int32_t)idx) {
                    if (entry_info1[local_from_idx].entry_pri ==
                        BCMPTM_RM_TCAM_PRIO_INVALID) {
                        reserved_move = true;
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                    }
                }

                rv = bcmptm_rm_tcam_entry_move(unit,
                                               ltid,
                                               ltid_info,
                                               partition,
                                               idx,
                                               idx - 1 - count,
                                               attrs,
                                               pt_dyn_info,
                                               iomd);
                if (reserved_move == true) {
                    /* Update FP group's and shared groups reserved index */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_update_reserve_entry_index(unit,
                                                          iomd,
                                                          reserved_index,
                                                          (idx - 1 - count)));
                    reserved_index = idx - 1 - count;
                    reserved_move = false;
                }
                count = 0;
            } else {
                count++;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Create a new free slot to insert an entry. */
int
bcmptm_rm_tcam_prio_eid_entry_index_allocate(int unit,
                              bcmptm_rm_tcam_entry_iomd_t *iomd,
                              uint8_t *reserved_used)
{
    int target_idx = -1;
    uint32_t idx = -1;
    int up_free_block_first_idx = -1;
    int up_free_idx = -1;
    int down_free_idx = -1;
    int from_idx = -1;
    int to_idx = -1;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info_width = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t
        *entry_info_ptr[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS] = {NULL};
    uint32_t free_entries = 0;
    uint32_t num_entries = 0;
    uint8_t num_parts = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bool per_slice_entry_mgmt = FALSE;
    uint32_t total_entry_info_depth = 0;
    uint16_t entry_info_idx = 0;
    uint32_t entry_info_depth = 0;
    uint16_t start_idx = 0;
    uint16_t end_idx = 0;
    uint8_t count = 0;
    uint32_t num_entries_traversed = 0;
    int last_higher_prio_entry_type_idx = -1;
    bool found_target = FALSE;
    int8_t entry_type = -1;
    bool prio_change = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attrs = iomd->req_info->entry_attrs;

    num_parts = iomd->num_parts;

    free_entries = iomd->free_entries;
    num_entries = iomd->num_entries;

    /*
     * If free entries are not available, entry cannot be inserted.
     * If HW atomicity is supported then full range can be used.
     * For entries which require more than one key row atomicity
     * can't be achieved even if HW supports.
     */
    if ((iomd->prio_change == FALSE) &&
        ((iomd->ltid_info->rm_more_info->hw_atomicity_support == 0) ||
         (num_parts > 1) || iomd->ltid_info->non_aggr) &&
         (free_entries == 1)) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

    if (free_entries == 0) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

    prio_change = iomd->prio_change;
    entry_type = iomd->entry_type;
    entry_attrs = iomd->req_info->entry_attrs;
    SHR_NULL_CHECK(entry_attrs, SHR_E_PARAM);

    if (entry_attrs->per_slice_entry_mgmt == TRUE) {
        per_slice_entry_mgmt = TRUE;
    } else {
        per_slice_entry_mgmt = FALSE;
    }

    for (entry_info_idx = 0;
        entry_info_idx < iomd->entry_info_arr.num_depth_parts;
        entry_info_idx++) {
        entry_info_depth =
            iomd->entry_info_arr.entry_info_depth[entry_info_idx];
        entry_info =
            iomd->entry_info_arr.entry_info[entry_info_idx][0];
        if (iomd->ltid_info->pt_type == LT_PT_TYPE_FP) {
            if ((iomd->multi_mode == FALSE) &&
                (!(SHR_BITGET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)))) {
                if (iomd->entry_info_arr.entry_info_free[entry_info_idx] == 0) {
                    if (entry_info[entry_info_depth - 1].entry_pri >
                                        iomd->req_info->entry_pri) {
                        num_entries_traversed += entry_info_depth;
                        total_entry_info_depth += entry_info_depth;
                        continue;
                    }
                }
            }
        }
        if (per_slice_entry_mgmt == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_logical_space_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              entry_attrs->slice_id,
                                              &start_idx,
                                              &end_idx));
            end_idx = end_idx + 1;
        } else {
            start_idx = 0;
            end_idx = entry_info_depth;
        }
        /* If no valid entries allocate the very first entry in TCAM. */
        if (free_entries == num_entries && per_slice_entry_mgmt == FALSE) {
            /* Get the first free index other than reserve entry */
            for (idx = start_idx; idx < end_idx; idx++) {
                if (IS_ENTRY_INFO_FREE(entry_info,
                       idx,
                       prio_change,
                       entry_type)) {

                    iomd->target_index = idx;

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_prio_entry_free_count_decr(unit,
                                                                   iomd));
                    SHR_FUNC_EXIT();
                }
            }
        }
        for (idx = start_idx; idx < end_idx; idx++) {
            /*
             * Record the last free tcam index before target
             * index is found.
             */
            if ((num_entries_traversed == iomd->num_entries_ltid) &&
                      (up_free_idx != -1)) {
                found_target = 1;
                break;
            }
            if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
                if (IS_ENTRY_INFO_FREE(entry_info,
                       idx,
                       prio_change,
                       entry_type)) {
                    /*
                     * Entry index should be multiple of start boundary.
                     * check for entry type number of free entries.
                     */
                    count = 1;
                    while (count != num_parts) {
                        entry_info_width =
                            iomd->entry_info_arr.entry_info[entry_info_idx][count];
                        if (IS_ENTRY_INFO_FREE(entry_info_width,
                                               idx,
                                               prio_change,
                                               entry_type)) {
                            count++;
                        } else {
                            /* count num_key_rows entries*/
                            break;
                        }
                    }
                    if (count == num_parts) {
                        up_free_idx = (idx + total_entry_info_depth);
                        /* Record first index when there a block of
                         * free entries.
                         */
                        if (up_free_block_first_idx == -1) {
                            up_free_block_first_idx = (idx + total_entry_info_depth);
                        }
                    }
                    /* if it couldnt find num_parts free entires width wise, set
                     * up_free_idx to -1 and continue
                     */
                    continue;
                }
            }

            /* Record the target index for the given prioirty. */
            if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
                if (entry_info[idx].entry_type == iomd->entry_type &&
                    entry_info[idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID) {
                    num_entries_traversed++;
                    if (entry_info[idx].entry_pri <
                        iomd->req_info->entry_pri) {
                        target_idx = (idx + total_entry_info_depth);
                        /*
                         * If free entries block is just above
                         * the target index up_free_block_first_idx
                         * is the target index for the new entry.
                         */
                        if (up_free_block_first_idx != -1) {
                            found_target =TRUE;
                            break;
                        }
                    } else if (
                     (entry_info[idx].entry_pri == iomd->req_info->entry_pri) &&
                     (entry_info[idx].entry_id == iomd->req_info->entry_id)) {
                        /*
                         * For key or data part update, we need to retain the
                         * same Hw index.
                         */
                        target_idx = (idx + total_entry_info_depth);
                        /*
                         * If free entries block is just above
                         * the target index up_free_block_first_idx
                         * is the target index for the new entry.
                         */
                        if (up_free_block_first_idx != -1) {
                            found_target =TRUE;
                            break;
                        }
                    } else {
                        up_free_block_first_idx = -1;
                        last_higher_prio_entry_type_idx =
                            (idx + total_entry_info_depth);
                    }
                    continue;
                }
            }

            /*
             * Record the first free tcam index after target
             * index is found.
             */
            if (IS_ENTRY_INFO_FREE(entry_info,
                                   idx,
                                   prio_change,
                                   entry_type)) {
                /*
                 * Entry index should be multiple of start boundary.
                 * check for entry type number of free entries.
                 */
                count = 1;
                while (count != num_parts) {
                    entry_info_width =
                        iomd->entry_info_arr.entry_info[entry_info_idx][count];
                    if (IS_ENTRY_INFO_FREE(entry_info_width,
                       idx,
                       prio_change,
                       entry_type)) {
                        count++;
                    } else {
                        /* count num_key_rows entries*/
                        break;
                    }
                }

                if (count == num_parts) {
                    down_free_idx = (idx + total_entry_info_depth);
                    found_target = TRUE;
                    break;
                }
            }
        }
        if (found_target == TRUE) {
            break;
        }
        total_entry_info_depth += entry_info_depth;
    }

    /*
     * For FP groups, as multi mode groups can share same resources,
     * control might reach here when only reserved entries are free and
     * all other entries are used up in the slices allocated. Reserved
     * entries can only used for entry update scenario's. Returning SHR_E_FULL
     * will make the caller to call compress or slice expand functions.
     */
    if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }
    /* Free index is found only in up ward direction. */
    if (up_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
        /*
         * Free entries block just before the actual target index or
         * free entries block is till the end of slice
         */
       if (up_free_block_first_idx != -1) {
            target_idx = up_free_block_first_idx;
            up_free_idx = up_free_block_first_idx;
        }
        /* All existing valid entries have greater priority than
         * target entry.
         */
        else if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            /* Approach: find the first entry from bottom of the
             * entry_info[idx] which is free or which is of its
             * entry_type where the first entry of this type is
             * found.
             */
            /* check if last higher prio entry is -1, if so then
             * this entry is first entry of this entry_type
             * so set target_idx = up_free_idx found
             */
             if (last_higher_prio_entry_type_idx == -1) {
                  target_idx = up_free_idx;
             }
             /* some entries of this entry_type exists but this
              * is the least priority entry. So last higher prio
              * entry type idx will have the last entry position.
              * Check if the up_free_idx is after this entry.
              * If so we can write directly to up_free_idx place.
              * else we have to move the all the entries from
              * last higher prio entry type idx to up_free_idx
              * and write the new entry at last higher prio
              * entry type idx.
              */
             else if (last_higher_prio_entry_type_idx < up_free_idx) {
                 target_idx = up_free_idx;
             } else {
                 target_idx = last_higher_prio_entry_type_idx;
             }
        } else {
            /* check if last higher prio entry is less than up_free_idx,
             * if so this entry need to be inserted at top for this
             * entry_type so set target_idx = up_free_idx found
             */
             if (last_higher_prio_entry_type_idx < up_free_idx) {
                  target_idx = up_free_idx;
             } else {
                 target_idx = last_higher_prio_entry_type_idx;
             }
        }
        /* Move entries up ward direction. */
        from_idx = target_idx;
        to_idx = up_free_idx;
    }

    /* Free index is found only in down ward direction. */
    if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID) {

        /* Move entries down ward direction. */
        from_idx = target_idx;
        to_idx = down_free_idx;

    }

    /* Free index is found in both up and down ward direction. */
    if (up_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx != BCMPTM_RM_TCAM_INDEX_INVALID) {
        if ((target_idx - up_free_idx) >=
            (down_free_idx - target_idx)) {
            /* Move entries down ward direction. */
            from_idx = target_idx;
            to_idx = down_free_idx;
        } else {
            /* Move entries up ward direction. */
            if (last_higher_prio_entry_type_idx < up_free_idx) {
                target_idx = up_free_idx;
            } else {
                target_idx = last_higher_prio_entry_type_idx;
            }
            from_idx = target_idx;
            to_idx = up_free_idx;
        }
    }

    /* update entry type at to_idx */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                       iomd,
                                       to_idx,
                                       entry_info_ptr,
                                       &idx));
    entry_info = entry_info_ptr[0];
    for (count = 0; count < num_parts; count++) {
        entry_info = entry_info_ptr[count];
        entry_info[idx].entry_type = iomd->entry_type;
        if (entry_info[idx].entry_id != BCMPTM_RM_TCAM_EID_INVALID) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (from_idx != to_idx) {
        /* Move entries either up or down ward direction. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_move_range(unit,
                                             iomd->ltid,
                                             iomd->ltid_info,
                                             0,
                                             from_idx,
                                             to_idx,
                                             iomd->entry_type,
                                             iomd->req_info->entry_attrs,
                                             iomd->pt_dyn_info,
                                             iomd));
    }
    iomd->target_index = to_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_entry_free_count_decr(unit, iomd));
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_entry_info_get_from_array(unit,
                                           iomd,
                                           target_idx,
                                           entry_info_ptr,
                                           &idx));
        for (count = 0; count < num_parts; count++) {
            entry_info = entry_info_ptr[count];
            if (entry_info[idx].entry_id != -1) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }
    iomd->target_index = target_idx;
    /*
     * Below code is used to update the reserved entry.
     * In the case of prio change, reserve entry can be used if it is the
     * nearest free entry. In this case update the reserve entry to a new
     * entry. This is done by the caller.
     * Some additional error checks are done. Reserve entry should never
     * be in move range in the case of prio change since reserve entry should
     * get allocated in prio change case. Error checks can be removed later.
 */
    if (prio_change == 1 && iomd->reserved_index != -1) {
        if (iomd->reserved_index == to_idx) {
            if (reserved_used) {
                *reserved_used = 1;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        } else {
            if (from_idx < to_idx) {
                if (INDEX_IN_RANGE(iomd->reserved_index, from_idx, to_idx)) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else if (from_idx > to_idx) {
                if (INDEX_IN_RANGE(iomd->reserved_index, to_idx, from_idx)) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Delete the TCAM entry for a given Entry id.
 *
 * Algorithm:
 *  1. Application provides required information about the TCAM Entry
 *     to identify the entry location in HW.
 *     1.1. For priority based TCAM; this information contains entry_id
 *  2. Do Lookup for the given TCAM information to find the entry index.
 *     2.1. For priority based TCAM lookup happens on entry_id.
 *  3. If entry index is Non Negative (meaning entry is existed)?
 *     3.1. Call south bound interface to give the write command with
 *          NULL entry data (meaning all zero) at entry index.
 *     3.2. Return SUCCESS
 *  4. If entry index is Negative (meaning entry does not exist)?
 *     4.1. Return the error code ENTRY NOT FOUND.
 */
int
bcmptm_rm_tcam_prio_eid_entry_delete(int unit,
                                     uint64_t req_flags,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                                     bcmptm_rm_tcam_req_t *req_info,
                                     bcmptm_rm_tcam_rsp_t *rsp_info,
                                     bcmltd_sid_t *rsp_ltid,
                                     uint32_t *rsp_flags,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t index = BCMPTM_RM_TCAM_INDEX_INVALID;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    /* Check if entry id exists or not. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_lookup(unit,
                                          iomd->req_flags,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->pt_dyn_info,
                                          iomd->req_info,
                                          iomd->rsp_info,
                                          iomd->rsp_ltid,
                                          iomd->rsp_flags,
                                          &index,
                                          iomd));
    /* Clear the data in HW and SW state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_clear(unit, iomd));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_free_count_incr(unit, iomd));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_entry_delete(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pt_dyn_info,
                                                  iomd->req_info));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Do search for given TCAM entry id and read the corresponding entry words.
 *
 * Algorithm:
 *  1. Application provides required information about the TCAM Entry
 *     to identify the entry location in HW.
 *     1.1. For priority based TCAM; this information contains entry_id
 *  2. Caliculate the hash for the given TCAM entry Id.
 *  3. Check TCAM entry id is present in linked list of Tcam entry ids
 *     having the same hash value.
 *  4. If entry_id exists, get the TCAM entry Index from TCAM entry metadata.
 *     4.1. If TCAM Entry Index is non negative read entry data (KEY + MASK +
 *          POLICY) will be given back to application.
 *     4.2. If TCAM entry index is negative, then there is a bug in the code and
 *          return error code Internal Error
 *  5. If entry_id does not exist, return error code Entry not Found.
 */
int
bcmptm_rm_tcam_prio_eid_entry_lookup(int unit,
                                     uint64_t req_flags,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                                     bcmptm_rm_tcam_req_t *req_info,
                                     bcmptm_rm_tcam_rsp_t *rsp_info,
                                     bcmltd_sid_t *rsp_ltid,
                                     uint32_t *rsp_flags,
                                     uint32_t *index,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int *entry_hash = NULL;
    int offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint16_t hash_val;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    uint32_t target_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    if (iomd->req_info->entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    same_key_info[unit].index = -1;
    same_key_info[unit].priority = -1;
    same_key_info[unit].prev_index = -1;

    entry_hash = iomd->entry_hash_ptr;
    hash_val = iomd->entry_hash_value;

    /*  Search the list of entries till the id matches */
    offset = entry_hash[hash_val];
    entry_info = iomd->entry_info_arr.entry_info[0][0];
        while (offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (entry_info[offset].entry_id ==
            iomd->req_info->entry_id) {
            break;
        }
        offset = entry_info[offset].offset;
    }

    if (offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get global index from to_idx */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_calc_target_idx_from_global_idx(unit,
                                                        iomd,
                                                        offset,
                                                        &target_idx));
    if (index != NULL) {
        *index = target_idx;
    }
    iomd->target_index = target_idx;

    /* Read the entry to set the response fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));

   iomd->rsp_info->rsp_entry_pri =
                   entry_info[offset].entry_pri;
   same_key_info[unit].index = offset;
   same_key_info[unit].priority =
                       entry_info[offset].entry_pri;
   same_key_info[unit].prev_index = -1;
exit:
    SHR_FUNC_EXIT();
}

/* Calculate memory size required for Prio Only type TCAMs */
int
bcmptm_rm_tcam_prio_eid_calc_mem_required(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 void *user_data)
{
    uint32_t size = 0;
    uint32_t trans_req_size = 0;
    uint32_t num_entries = 0;
    uint32_t *total_size = user_data;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Parameters Check. */
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    /* This should always be the first structure in
     * memory allocation for every LTID.
     */
    trans_req_size = sizeof(bcmptm_rm_tcam_trans_info_t);

    size += sizeof(bcmptm_rm_tcam_prio_eid_info_t);

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_PER_SLICE_MANAGEMENT) {
        size += (sizeof(bcmptm_rm_tcam_prio_eid_slice_info_t) *
                    ltid_info->hw_entry_info->num_depth_inst);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit,
                                  ltid,
                                  ltid_info,
                                  &num_entries));
    /*
     * Memory required for TCAM entry information consists of
     * (eid, priority and offset)
     */
    size += (num_entries *
             sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));

    /* Memory space required for TCAM hash table. */
    size += (sizeof(int) * (num_entries / 4));

    (*total_size) += (trans_req_size + size);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the metadata of priority based TCAM in
 * bcmptm_rm_tcam_prio_eid_info_t format in cold boot.
 */
STATIC int
prio_eid_metadata_cb_process(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             uint8_t *start_ptr,
                             uint8_t sub_component_id)
{
    int *entry_hash = NULL;
    uint32_t num_entries = 0;
    uint32_t idx = 0;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint32_t offset = 0;
    uint32_t tcam_info_offset = 0;
    bcmptm_rm_tcam_prio_eid_slice_info_t *slice_info;
    uint8_t s_row;
    bcmdrd_sid_t sid;
    int min_idx;
    int max_idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info, &num_entries));

    /* Report trans_info structure offset */
    offset = 0;
    bcmissu_struct_info_report(unit,
                               BCMMGMT_RM_TCAM_COMP_ID,
                               sub_component_id,
                               offset,
                               sizeof(bcmptm_rm_tcam_trans_info_t), 1,
                               BCMPTM_RM_TCAM_TRANS_INFO_T_ID);

    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr + offset);
    trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
    trans_info->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;

    /* Report prio_only_info structure offset */
    offset += sizeof(bcmptm_rm_tcam_trans_info_t);
    bcmissu_struct_info_report(unit,
                               BCMMGMT_RM_TCAM_COMP_ID,
                               sub_component_id,
                               offset,
                               sizeof(bcmptm_rm_tcam_prio_eid_info_t), 1,
                               BCMPTM_RM_TCAM_PRIO_EID_INFO_T_ID);
    tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)(start_ptr + offset);
    tcam_info->ltid = ltid;
    tcam_info->flags = 0;

    /* Set tcam info flags according to LT_INFO flags */
    if (ltid_info->non_aggr == 0) {
        tcam_info->flags |= BCMPTM_RM_TCAM_F_AGGR_VIEW;
    }
    if (ltid_info->non_aggr == 1) {
        tcam_info->flags |= BCMPTM_RM_TCAM_F_TCAM_ONLY_VIEW;
        tcam_info->flags |= BCMPTM_RM_TCAM_F_DATA_ONLY_VIEW;
    }

    /* Set the enty counts */
    tcam_info->num_entries = num_entries;
    tcam_info->free_entries =  num_entries;

    /* Set the entry info and hash details */
    if (tcam_info->num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
        tcam_info->entry_hash_size = (num_entries /
                               BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR);
    } else {
        tcam_info->entry_hash_size = num_entries;
    }

    offset += sizeof(bcmptm_rm_tcam_prio_eid_info_t);
    tcam_info_offset += sizeof(bcmptm_rm_tcam_prio_eid_info_t);

    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_PER_SLICE_MANAGEMENT) {
        tcam_info->slice_info_offset = tcam_info_offset;

        bcmissu_struct_info_report(unit,
                                  BCMMGMT_RM_TCAM_COMP_ID,
                                  sub_component_id,
                                  offset,
                                  sizeof(bcmptm_rm_tcam_prio_eid_slice_info_t),
                                  ltid_info->hw_entry_info->num_depth_inst,
                                  BCMPTM_RM_TCAM_PRIO_EID_SLICE_INFO_T_ID);
        slice_info = (bcmptm_rm_tcam_prio_eid_slice_info_t *)(start_ptr + offset);
        for (s_row = 0;
                s_row < ltid_info->hw_entry_info->num_depth_inst;
                s_row++) {
                sid = ltid_info->hw_entry_info->sid[s_row][0];
                min_idx = bcmptm_pt_index_min(unit, sid);
                max_idx = bcmptm_pt_index_max(unit, sid);
                slice_info[s_row].free_entries = max_idx - min_idx + 1;
        }
        tcam_info_offset += (sizeof(bcmptm_rm_tcam_prio_eid_slice_info_t) *
                             ltid_info->hw_entry_info->num_depth_inst);

        offset += (sizeof(bcmptm_rm_tcam_prio_eid_slice_info_t) *
                             ltid_info->hw_entry_info->num_depth_inst);
    }

    /* Set the entry info offset. */
    tcam_info->entry_info_offset = tcam_info_offset;

    bcmissu_struct_info_report(unit,
                              BCMMGMT_RM_TCAM_COMP_ID,
                              sub_component_id,
                              offset,
                              sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t),
                              tcam_info->num_entries,
                              BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID);

    entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t *)(start_ptr + offset);
    for (idx = 0; idx < tcam_info->num_entries; idx++) {
        entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    offset += (num_entries * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    tcam_info->entry_hash_offset = (tcam_info->entry_info_offset +
               (num_entries * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));

    entry_hash = (int *)(start_ptr + offset);

    for (idx = 0; idx < tcam_info->entry_hash_size; idx++) {
        entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }
    offset += (tcam_info->entry_hash_size * sizeof(int));
    trans_info->state_size = offset;

exit:
     SHR_FUNC_EXIT();
}

/*
 * Initialize the metadata of priority based TCAM in
 * bcmptm_rm_tcam_prio_eid_info_t format in cold boot.
 */
STATIC int
prio_eid_init_metadata_cb(int unit,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info)
{
    uint8_t pipe = 0;
    uint8_t *start_ptr = NULL;
    uint8_t sub_comp_id = 0;
    bcmptm_rm_tcam_md_t *lt_md = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_md_get(unit, ltid, &lt_md));

    for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {
        sub_comp_id = lt_md->active_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->active_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_eid_metadata_cb_process(unit,
                                          ltid,
                                          ltid_info,
                                          start_ptr,
                                          sub_comp_id));
        sub_comp_id = lt_md->backup_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->backup_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_eid_metadata_cb_process(unit,
                                          ltid,
                                          ltid_info,
                                          start_ptr,
                                          sub_comp_id));
    }

exit:
     SHR_FUNC_EXIT();
}

STATIC int
prio_eid_metadata_wb_process(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             uint8_t *start_ptr,
                             uint8_t sub_component_id)
{
    uint32_t offset = 0;
    uint32_t tcam_info_offset = 0;
    uint32_t issu_offset = 0;
    uint32_t num_entries = 0;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(start_ptr, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info, &num_entries));

    offset = 0;
    /* Get trans_info structure offset */
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_TRANS_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    trans_info = (bcmptm_rm_tcam_trans_info_t *)(start_ptr + offset);
    if (trans_info->sign_word != BCMPTM_HA_SIGN_LT_OFFSET_INFO) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Get prio_eid_info structure offset and ensure that ltid,
     * num_entries and hash size is not changed during warmboot.
     */
    offset += sizeof(bcmptm_rm_tcam_trans_info_t);
    /* Get prio_eid_info structure offset */
    issu_offset = 0;
    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_EID_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    tcam_info = (bcmptm_rm_tcam_prio_eid_info_t *)(start_ptr + offset);
    if (tcam_info->ltid != ltid ||
        tcam_info->num_entries != num_entries) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (num_entries > BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR) {
        if (tcam_info->entry_hash_size !=
           (num_entries / BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else if (tcam_info->entry_hash_size != num_entries) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Update the entry_info and entry_hash offsets. These offsets
     * might change due to the addition of new fields in existing
     * data structures.
     */
    offset += sizeof(bcmptm_rm_tcam_prio_eid_info_t);
    tcam_info_offset += sizeof(bcmptm_rm_tcam_prio_eid_info_t);
    /* Get prio_eid_entry_info structure offset */
    issu_offset = 0;
    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_PER_SLICE_MANAGEMENT) {
        tcam_info->slice_info_offset = tcam_info_offset;
        offset += (sizeof(bcmptm_rm_tcam_prio_eid_slice_info_t) *
                             ltid_info->hw_entry_info->num_depth_inst);
        tcam_info_offset += (sizeof(bcmptm_rm_tcam_prio_eid_slice_info_t) *
                             ltid_info->hw_entry_info->num_depth_inst);
    }

    bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_component_id,
                            &issu_offset);
    if (offset != issu_offset) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    tcam_info->entry_info_offset = tcam_info_offset;
    offset += (num_entries * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    tcam_info->entry_hash_offset = (tcam_info->entry_info_offset +
               (num_entries * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));
    offset += (tcam_info->entry_hash_size * sizeof(int));
    trans_info->state_size = offset;

exit:
     SHR_FUNC_EXIT();
}

/*
 * Initialize the metadata of priority based TCAM in
 * bcmptm_rm_tcam_prio_eid_info_t format in warm boot.
 */
STATIC int
prio_eid_init_metadata_wb(int unit,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info)
{
    uint8_t pipe = 0;
    uint8_t *start_ptr = NULL;
    uint8_t sub_comp_id = 0;
    bcmptm_rm_tcam_md_t *lt_md = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_md_get(unit, ltid, &lt_md));

    for (pipe = 0; pipe < lt_md->num_pipes; pipe++) {
        sub_comp_id = lt_md->active_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->active_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_eid_metadata_wb_process(unit,
                                          ltid,
                                          ltid_info,
                                          start_ptr,
                                          sub_comp_id));
        sub_comp_id = lt_md->backup_sub_component_id[pipe];
        start_ptr = (uint8_t *)lt_md->backup_ptrs[pipe];
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_eid_metadata_wb_process(unit,
                                          ltid,
                                          ltid_info,
                                          start_ptr,
                                          sub_comp_id));
    }

exit:
     SHR_FUNC_EXIT();
}

/*
 * Initialize the metadata of priority based TCAM in
 * bcmptm_rm_tcam_prio_eid_info_t format.
 */
int
bcmptm_rm_tcam_prio_eid_init_metadata(int unit,
                                  bool warm,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (warm == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_eid_init_metadata_cb(unit, ltid, ltid_info));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (prio_eid_init_metadata_wb(unit, ltid, ltid_info));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_eid_tcam_get_table_info(int unit,
                                uint64_t flags,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_req_t *req_info,
                                bcmptm_rm_tcam_rsp_t *rsp_info,
                                bcmltd_sid_t *rsp_ltid,
                                uint32_t *rsp_flags,
                                bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_table_info_t *table_info;
    bcmptm_rm_tcam_prio_eid_info_t *tcam_info;
    uint32_t unused_entries;
    uint32_t max_limit;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info->rsp_misc, SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_tcam_info_get(unit, iomd));
    tcam_info = iomd->tcam_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_num_index_required_get(unit,
                                               ltid,
                                               ltid_info,
                                               &unused_entries));

    table_info = (bcmptm_table_info_t *)rsp_info->rsp_misc;
    max_limit = table_info->entry_limit;
    table_info->entry_limit = tcam_info->num_entries - unused_entries;
    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        table_info->entry_limit =
            table_info->entry_limit/ltid_info->hw_entry_info->num_key_rows;
    }
    if (max_limit != 0) {
        table_info->entry_limit = SHR_MIN(max_limit,table_info->entry_limit);
    }
exit:
    SHR_FUNC_EXIT();
}


