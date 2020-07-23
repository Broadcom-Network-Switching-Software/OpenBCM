/*! \file rm_tcam_fp_entry_mgmt.c
 *
 * Low Level Functions for FP TCAM entry management
 * This file contains low level functions for FP based TCAMs
 * to do operations like TCAM entry Insert/Lookup/Delete/Move.
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
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include "rm_tcam_fp_profiles.h"
#include "rm_tcam_fp_utils.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Private Functions
 */
static int
bcmptm_rm_tcam_stage_entry_lookup(int unit,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_stage_hw_info_t *hw_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);

    SHR_ALLOC(hw_info,
              sizeof(bcmptm_rm_tcam_stage_hw_info_t),
              "bcmptmRmtcamStageAttrsHwInfo");
    sal_memset(hw_info, 0, sizeof(bcmptm_rm_tcam_stage_hw_info_t));

    hw_info->num_slices = iomd->num_slices;
    attrs->stage_hw_info = (void *)hw_info;
    hw_info = NULL;

exit:
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_slice_entry_lookup(int unit,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_slice_hw_info_t *hw_info = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);

    SHR_ALLOC(hw_info,
              sizeof(bcmptm_rm_tcam_slice_hw_info_t),
              "bcmptmRmtcamSliceAttrsHwInfo");

    sal_memset(hw_info, 0, sizeof(bcmptm_rm_tcam_slice_hw_info_t));

    slice_fc = iomd->slice_fc;

    if (slice_fc == NULL) {
        SHR_EXIT();
    }
    if (slice_fc[attrs->slice_num].slice_flags & BCMPTM_SLICE_IN_USE) {
        hw_info->num_entries_created
            = (slice_fc[attrs->slice_num].num_entries -
               slice_fc[attrs->slice_num].free_entries);
    }

    hw_info->num_entries_total = slice_fc[attrs->slice_num].num_entries;
    attrs->slice_hw_info = (void *)hw_info;
    hw_info = NULL;

exit:
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_group_entry_lookup(int unit,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_group_hw_info_t *hw_info = NULL;
    uint8_t slice_id, pri_slice_id, idx1 ,idx2;
    uint8_t prev_slice_id;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    uint8_t num_slices = 0;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint32_t entry_count = 0;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_sid_info_t sid_info;
    uint8_t virtual_slice_num = 0;
    int *next_slice_id_offset = NULL;
    int *right_slice_id_offset = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd->entry_attrs, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);

    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;

    SHR_ALLOC(hw_info,
              sizeof(bcmptm_rm_tcam_group_hw_info_t),
              "bcmptmRmtcamEntryAttrsHwInfo");

    sal_memset(&sid_info, 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    sal_memset(hw_info, 0, sizeof(bcmptm_rm_tcam_group_hw_info_t));

    sid_info.tbl_inst = attrs->pipe_id;
    sid_info.stage_flags = stage_fc->flags;
    sid_info.oper_flags |= BCMPTM_RM_TCAM_OPER_SLICE_MAP_UPDATE;
    sid_info.slice_id = 0;
    sid_info.lt_action_pri = &virtual_slice_num;
    sid_info.num_slices = iomd->num_slices;

    fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    num_slices = iomd->num_slices;

    slice_id = fg->primary_slice_id;
    attrs->group_mode = fg->group_mode;

    idx1 = 0;
    idx2 = 0;

    do {
           pri_slice_id = slice_id;
           do {
                  hw_info->slice[idx1][idx2] = slice_id;
                  sid_info.slice_id = slice_id;
                  if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
                      SHR_IF_ERR_VERBOSE_EXIT
                          (bcmptm_rm_tcam_memreg_oper(unit,
                                          iomd->ltid,
                                          BCMPTM_RM_TCAM_EFP_SLICE_MAP,
                                          BCMFP_RM_TCAM_SID_OPER_READ,
                                          &sid_info));
                  } else if (stage_fc->flags &
                             BCMPTM_STAGE_KEY_TYPE_SELCODE) {
                      SHR_IF_ERR_VERBOSE_EXIT
                          (bcmptm_rm_tcam_memreg_oper(unit,
                                          iomd->ltid,
                                          BCMPTM_RM_TCAM_VFP_SLICE_MAP,
                                          BCMFP_RM_TCAM_SID_OPER_READ,
                                          &sid_info));
                  }
                  hw_info->virtual_slice[idx1][idx2] =*
                                   sid_info.lt_action_pri;
                  prev_slice_id = slice_id;
                  slice_id +=
                  slice_fc[slice_id].next_slice_id_offset[attrs->group_mode];
                  idx2++;
                  next_slice_id_offset =
                      slice_fc[prev_slice_id].next_slice_id_offset;
           } while (slice_id < num_slices &&
                   (next_slice_id_offset[attrs->group_mode] != 0));

           right_slice_id_offset =
               slice_fc[pri_slice_id].right_slice_id_offset;
           slice_id += right_slice_id_offset[attrs->group_mode];
           idx1++;
           hw_info->num_slices_depth_wise = idx2;
           idx2 = 0;
    } while (slice_id < num_slices &&
            (right_slice_id_offset[attrs->group_mode] != 0));

    /* Calculate free entries of the current group slices. */
    slice_id = fg->primary_slice_id;
    do {
        prev_slice_id = slice_id;
        entry_count += slice_fc[slice_id].free_entries;
        next_slice_id_offset = slice_fc[slice_id].next_slice_id_offset;
        slice_id += next_slice_id_offset[attrs->group_mode];
        next_slice_id_offset = slice_fc[prev_slice_id].next_slice_id_offset;
    } while (slice_id < num_slices &&
            (next_slice_id_offset[attrs->group_mode != 0]));


    /* Calculate entries tentative for group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_tentative_entry_calculate(unit,
                                                           iomd,
                                                           &entry_count));
    hw_info->num_entries_tentative = entry_count;
    attrs->grp_hw_info = (void *)hw_info;
    hw_info = NULL;

exit:
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_presel_entries_copy(int unit,
                                      uint8_t new_slice_id,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int16_t from_idx = -1;
    int16_t to_idx = -1;
    uint16_t idx = 0;
    uint32_t num_presel_per_slice;
    int8_t pri_slice_id = -1;
    uint8_t part = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices = 0;
    uint32_t entry_count = 0;
    uint8_t slice_id = 0;
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    bcmptm_rm_tcam_req_t req_info;
    bcmptm_rm_tcam_rsp_t rsp_info;
    uint32_t *entry_key_words = NULL;
    uint32_t *entry_data_words = NULL;
    uint32_t *ekw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    uint32_t *edw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    bcmptm_rm_tcam_lt_info_t presel_ltid_info;
    bcmltd_sid_t  presel_ltid;
    int size = 0;
    bcmptm_rm_tcam_entry_iomd_t *iomd_temp = NULL;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    int *next_slice_id_offset = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd->pt_dyn_info, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;

    SHR_ALLOC(entry_attrs,
              sizeof(bcmptm_rm_tcam_entry_attrs_t),
              "bcmptmRmtcamEntryAttrsCopy");

    sal_memcpy(entry_attrs,
               iomd->entry_attrs,
               sizeof(bcmptm_rm_tcam_entry_attrs_t));

    size = sizeof(bcmptm_rm_tcam_entry_iomd_t);
    SHR_ALLOC(iomd_temp, size, "bcmptmRmTcamEntryIomdTemp");
    sal_memcpy(iomd_temp, iomd, size);
    iomd_temp->entry_attrs = (void *)entry_attrs;

    /* Get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                     iomd->ltid,
                                                     iomd->ltid_info,
                                                     entry_attrs->pipe_id,
                                                     new_slice_id,
                                                     &num_presel_per_slice));
    fg = iomd->group_ptr;
    num_slices = iomd->num_slices;
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    pri_slice_id = fg->primary_slice_id;

    slice_id = pri_slice_id;
    next_slice_id_offset = slice_fc[slice_id].next_slice_id_offset;
    while (slice_id < num_slices &&
          (next_slice_id_offset[entry_attrs->group_mode] != 0)) {
        entry_count += num_presel_per_slice;
        slice_id += next_slice_id_offset[entry_attrs->group_mode];
        next_slice_id_offset = slice_fc[slice_id].next_slice_id_offset;
        if (slice_id == new_slice_id) {
            break;
        }
    }

    if (slice_id != new_slice_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(entry_key_words,
              (sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS),
              "bcmptmRmtcamEntrykeyWords");
    SHR_ALLOC(entry_data_words,
              (sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS),
              "bcmptmRmtcamEntryDataWords");
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        ekw_row[part] = entry_key_words + (part * BCMPTM_MAX_PT_FIELD_WORDS);
        edw_row[part] = entry_data_words + (part * BCMPTM_MAX_PT_FIELD_WORDS);
    }
    req_info.entry_attrs = (void *)entry_attrs;
    req_info.rsp_ekw_buf_wsize = entry_words_size;
    req_info.rsp_edw_buf_wsize = entry_words_size;
    req_info.ekw = ekw_row;
    req_info.edw = edw_row;
    rsp_info.rsp_ekw = ekw_row;
    rsp_info.rsp_edw = edw_row;
    if (ltid_info->rm_more_info->shared_lt_count) {
        presel_ltid = ltid_info->rm_more_info->shared_lt_info[0];
        sal_memset(&presel_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_ptrm_info_get(unit, presel_ltid, 0, 0,
                                         ((void *)&presel_ltid_info),
                                         NULL, NULL));
    } else {
        presel_ltid = iomd->ltid;
        presel_ltid_info = *iomd->ltid_info;
    }

    iomd_temp->ltid = presel_ltid;
    iomd_temp->ltid_info = &presel_ltid_info;
    /*
     * Group entries might be installed per pipe in global pipe aware mode,
     * but the, presel entries needs to be copied to all pipes when group
     * is auto expanded to new slices.
     */
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.index = iomd_temp->pt_dyn_info->index;
    pt_dyn_info.tbl_inst = fg->pipe_id;
    iomd_temp->pt_dyn_info = &pt_dyn_info;
    /*
     * Loop for all presel entries per slice and copy the non-empty entries
     * from primaryslice to the newly expanded slice. copy for the width
     * parts of the TCAM are taken care by hw read/write functions.
     */
    iomd_temp->req_info = &req_info;
    iomd_temp->rsp_info = &rsp_info;
    /* Set the presel entry flag so that presel entry info is fetched */
    SHR_BITSET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY);
    /* Update the iomd temp for the presel. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd_temp));
    entry_info = iomd_temp->entry_info_arr.entry_info[0][0];
    for (idx = 0; idx < num_presel_per_slice; idx++) {

        /* calculate the from and to indexs */
        from_idx = idx;
        to_idx = entry_count + idx;

        /* If the entry is empty skip it. */
        if (entry_info[from_idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            continue;
        }

        sal_memset(entry_key_words, 0,
                   sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
                   BCMPTM_MAX_PT_FIELD_WORDS);
        sal_memset(entry_data_words, 0,
                   sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
                   BCMPTM_MAX_PT_FIELD_WORDS);

        /* Read the entry words from from_idx. */
        iomd_temp->target_index = from_idx;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd_temp));

        /* Write the entry words to to_idx. */
        iomd_temp->target_index = to_idx;
        iomd_temp->hw_only = TRUE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd_temp));
        iomd_temp->hw_only = FALSE;
    }

exit:
    SHR_FREE(iomd_temp);
    SHR_FREE(entry_attrs);
    SHR_FREE(entry_key_words);
    SHR_FREE(entry_data_words);
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_presel_entry_insert(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint16_t idx1 = 0;
    uint16_t idx2 = 0;
    uint32_t num_presel_per_slice;
    uint32_t found_index = 0;
    uint32_t target_index = 0;
    uint8_t slice_id = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices;
    uint8_t update_type = 0;
    int rv = SHR_E_NONE;
    uint8_t found = 0;
    uint32_t entry_count = 0;
    uint8_t mode = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd->pt_dyn_info, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);

    mode = attrs->group_mode;

   /* Check if entry already exists or not. */
    if (iomd->req_info->same_key == true) {
        /* If already looked up, use the results */
        if (same_key_info[unit].index != -1) {
            found_index = same_key_info[unit].index;
            rv = SHR_E_NONE;
        } else {
            found_index = -1;
            rv = SHR_E_NOT_FOUND;
        }
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
        found = 1;
        if (iomd->req_info->entry_pri == iomd->rsp_info->rsp_entry_pri) {
            /* Identify if its a key or data change */
            rv = (bcmptm_rm_tcam_update_type_get(unit,
                                                 &update_type,
                                                 iomd));
            if (rv == SHR_E_EXISTS) {
                SHR_EXIT();
            }
        }
    }

    /* Insert the primary entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_insert(unit, iomd));
    target_index = iomd->target_index;

    fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    /* Initialize field slice pointer. */
    num_slices = iomd->num_slices;
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    /* Get the primary slice index */
    slice_id = fg->primary_slice_id;

    /* Get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                     iomd->ltid,
                                                     iomd->ltid_info,
                                                     attrs->pipe_id,
                                                     slice_id,
                                                     &num_presel_per_slice));

    iomd->hw_only = TRUE;
    while (slice_id < num_slices &&
          (0 != slice_fc[slice_id].next_slice_id_offset[attrs->group_mode])) {

        entry_count += num_presel_per_slice;
         /* go to the next slice if exists */
         slice_id = slice_id +
                    slice_fc[slice_id].next_slice_id_offset[attrs->group_mode];

         /* get the number of presel entries in that slice */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                      iomd->ltid,
                                                      iomd->ltid_info,
                                                      attrs->pipe_id,
                                                      slice_id,
                                                      &num_presel_per_slice));

         iomd->entry_info_arr.entry_info_depth[0] += num_presel_per_slice;
         idx1 = entry_count + target_index;
         idx2 = entry_count + found_index;

         /* Data only update */
         if ((update_type & BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE) &&
             bcmdrd_feature_is_real_hw(unit) &&
             (iomd->ltid_info->hw_entry_info[mode].sid_data_only != NULL)) {
             iomd->target_index = idx2;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_entry_update_data_only(unit, iomd));
         } else {
             /* Write the entry words to HW */
             iomd->target_index = idx1;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd));
             /* Key or prio update */
             if (found) {
                /* Clear the data in HW */
                iomd->target_index = idx2;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_eid_entry_clear_hw(unit,
                                                 iomd->target_index,
                                                 iomd));
            }
        }
    }
    iomd->hw_only = FALSE;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_presel_entry_delete(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint16_t idx = 0;
    uint32_t num_presel_per_slice = 0;
    uint8_t slice_id = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices = 0;
    uint32_t entry_count = 0;
    uint32_t target_index = 0;

    SHR_FUNC_ENTER(unit);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);

    /* Delete the primary entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_delete(unit,
                                          iomd->req_flags,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->pt_dyn_info,
                                          iomd->req_info,
                                          iomd->rsp_info,
                                          iomd->rsp_ltid,
                                          iomd->rsp_flags,
                                          iomd));
    target_index = iomd->target_index;

    fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    /* Initialize field slice pointer. */
    num_slices = iomd->num_slices;
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    /* Get the primary slice index */

    /* Get the primary slice index */
    slice_id = fg->primary_slice_id;
    /* Get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 attrs->pipe_id,
                                                 slice_id,
                                                 &num_presel_per_slice));

    iomd->hw_only = TRUE;
    while (slice_id < num_slices &&
          (0 != slice_fc[slice_id].next_slice_id_offset[attrs->group_mode])) {

         entry_count += num_presel_per_slice;
         /* Go to the next slice if exists */
         slice_id = slice_id +
                    slice_fc[slice_id].next_slice_id_offset[attrs->group_mode];

         /* Get the number of presel entries in that slice */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                            iomd->ltid,
                                            iomd->ltid_info,
                                            attrs->pipe_id,
                                            slice_id,
                                            &num_presel_per_slice));

         iomd->entry_info_arr.entry_info_depth[0] += num_presel_per_slice;

         idx = entry_count + target_index;
         iomd->target_index = idx;
         SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_clear_hw(unit,
                                                 iomd->target_index,
                                                 iomd));
    }
    iomd->hw_only = FALSE;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_entry_segment_check(int unit,
                                      bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      bcmptm_rm_tcam_fp_stage_t *stage_fc,
                                      int pipe_id,
                                      int segment_id,
                                      bool *free_segment)
{
    uint32_t idx = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    int  *entry_hash = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(free_segment, SHR_E_PARAM);

    (*free_segment) = 1;

    num_entries_per_slice = stage_fc->max_entries_per_slice;

    entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t*)
               (((uint8_t *)stage_fc) + (stage_fc->entry_info_seg +
               ((num_entries_per_slice * segment_id) *
               sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t))));

    entry_hash = (int *)(((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg +
                 ((num_entries_per_slice * segment_id) * sizeof(int))));

    for (idx = 0; idx < num_entries_per_slice; idx++) {
        if (entry_info[idx].entry_id != BCMPTM_RM_TCAM_PRIO_INVALID) {
            (*free_segment) = 0;
            break;
        }
    }

    if ((*free_segment) == 0) {
        SHR_EXIT();
    }

    for (idx = 0; idx < num_entries_per_slice; idx++) {
         if (entry_hash[idx] != BCMPTM_RM_TCAM_OFFSET_INVALID) {
            (*free_segment) = 0;
         }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_entry_segment_clear(int unit,
                                      bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      bcmptm_rm_tcam_fp_stage_t *stage_fc,
                                      int pipe_id,
                                      int segment_id)
{
    uint32_t idx = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    int  *entry_hash = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    num_entries_per_slice = stage_fc->max_entries_per_slice;

    entry_info = (bcmptm_rm_tcam_prio_eid_entry_info_t*)
               (((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + ((num_entries_per_slice * segment_id)
               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t))));

    entry_hash = (int*) (((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + ((num_entries_per_slice * segment_id)
               * sizeof(int))));

    for (idx = 0; idx < num_entries_per_slice; idx++) {
        entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        entry_info[idx].entry_id = BCMPTM_RM_TCAM_EID_INVALID;
        entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
        entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    }

    for (idx = 0; idx < num_entries_per_slice; idx++) {
        entry_hash[idx] = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_fp_entry_segment_move(int unit,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bcmptm_rm_tcam_fp_stage_t *stage_fc,
                                     int pipe_id,
                                     int from_id,
                                     int to_id)
{
    uint32_t num_entries_per_slice = 0;
    uint8_t *from_ptr = NULL;
    uint8_t *to_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    num_entries_per_slice = stage_fc->max_entries_per_slice;

    from_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + ((num_entries_per_slice * to_id)
               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));
    to_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_info_seg
               + ((num_entries_per_slice * from_id)
               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));

    sal_memcpy(from_ptr, to_ptr,
               (num_entries_per_slice
               * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t)));

    from_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + (((num_entries_per_slice * to_id))
               * sizeof(int)));

    to_ptr = ((uint8_t *)stage_fc) + (stage_fc->entry_hash_seg
               + (((num_entries_per_slice * from_id))
               * sizeof(int)));

    sal_memcpy(from_ptr, to_ptr, (num_entries_per_slice * sizeof(int)));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_entry_segment_update(int unit,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       bcmptm_rm_tcam_fp_stage_t *stage_fc,
                                       int pipe_id,
                                       bcmptm_rm_tcam_fp_group_t *grp,
                                       int segment_id)
{
    uint32_t num_entries_per_slice = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    num_entries_per_slice = stage_fc->max_entries_per_slice;

    grp->entry_info_seg = (stage_fc->entry_info_seg)
                           + ((num_entries_per_slice * segment_id)
                           * sizeof(bcmptm_rm_tcam_prio_eid_entry_info_t));
    grp->entry_hash_seg = (stage_fc->entry_hash_seg)
                           + ((num_entries_per_slice * segment_id)
                           * sizeof(int));
exit:
    SHR_FUNC_EXIT();

}

static int
bcmptm_rm_tcam_fp_entry_segment_expand(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmptm_rm_tcam_fp_stage_t  *stage_fc,
                                bcmptm_rm_tcam_entry_attrs_t *entry_attr)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_fp_group_t  *first_fg = NULL;
    bcmptm_rm_tcam_fp_group_t  *grp_ptr[32] = {NULL};
    int primary_segment_id = -1;
    int first_free_segment_id = -1;
    int grp_count = 0;
    uint16_t num_slices = 0;
    int seg_id = -1;
    int new_segment_id = -1;
    uint8_t found = 0;
    bcmptm_fp_grp_seg_bmp_t *group_segments = NULL;
    int pipe_id = 0;
    int idx = 0;
    int grp_id = -1;
    bcmptm_fp_entry_seg_bmp_t *stage_entry_seg_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_attr, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    pipe_id = entry_attr->pipe_id;
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    num_slices = stage_fc->num_slices;

    SHR_ALLOC(group_segments,
             (sizeof(bcmptm_fp_grp_seg_bmp_t) * num_slices),
             "bcmptmRmtcamFPEntrySegment");

    stage_entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t*)
                           (((uint8_t *)stage_fc)
                           + stage_fc->entry_seg_bmp_offset);

    /* Clear group_segment bitmap */
    for (idx = 0; idx < stage_fc->num_groups; idx++) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                  SHR_BITCLR(group_segments[seg_id].w, idx);
             }
    }

    /* Create group segment bitmap */
    fg = (bcmptm_rm_tcam_fp_group_t*)(((uint8_t *)stage_fc) +
                                      stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
         if ((fg->valid == 1) && (fg->pipe_id == entry_attr->pipe_id)) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                 if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                     SHR_BITSET(group_segments[seg_id].w,
                                idx);
                 }
             }
         }
         fg++;
    }

    /* Initialize field group pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->group_id,
                                          stage_fc,
                                          &fg));

    /* Find primary segment_id */
    for (seg_id = (num_slices - 1); seg_id >= 0; seg_id--) {
         if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
            break;
         }
    }

    if (seg_id == -1) {
       SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else {
       primary_segment_id = seg_id;
    }

    if (!(SHR_BITGET(stage_entry_seg_bmp[0].w,
          primary_segment_id))) {
       SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    new_segment_id = primary_segment_id+1;

    if (!(SHR_BITGET(stage_entry_seg_bmp[0].w, new_segment_id))) {
       /* Next segment is free to be used
        * Hence no moves required
        */
    } else {
        /* Segment moves are needed */
        /* To check if free segments are available */
        for (seg_id = new_segment_id;
             seg_id < num_slices; seg_id++) {
             if (!(SHR_BITGET(stage_entry_seg_bmp[0].w,seg_id))) {
                found = 1;
                break;
             }
        }
        if (found == 0) {
            /* No free segments are available.
             * Ideally should not hit this condition.
             */
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        first_free_segment_id = seg_id;

        /* Move all segments from new_segment_id till
           first_free_segment_id */
        seg_id = (first_free_segment_id);
        while (seg_id > new_segment_id) {
            grp_count = 0;

            /* Create group segment bitmap */
            first_fg = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);
            for (idx = 0; idx < stage_fc->num_groups; idx++) {
                if (SHR_BITGET(group_segments[seg_id - 1].w, idx)) {
                    /* Get the field group pointer for all groups
                       in current segment.
                     */
                   grp_ptr[grp_count] = &(first_fg[idx]);
                   grp_count++;
                }
            }

            if (grp_count == 0) {
                /* No groups are available.
                * Ideally should not hit this condition.
                */
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }


            /* Move segment from prev to curr */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_entry_segment_move(unit,
                                                      ltid,
                                                      ltid_info,
                                                      stage_fc,
                                                      pipe_id,
                                                      seg_id - 1,
                                                      seg_id));

            /* Update bitmaps */
            SHR_BITCLR(stage_entry_seg_bmp[0].w, seg_id - 1);
            SHR_BITSET(stage_entry_seg_bmp[0].w, seg_id);
            for (grp_id = 0; grp_id < grp_count; grp_id++) {
                SHR_BITCLR(grp_ptr[grp_id]->entry_seg_bmp.w, seg_id - 1);
                SHR_BITSET(grp_ptr[grp_id]->entry_seg_bmp.w, seg_id);
            }

            /* Update the segment pointers in group */
            for (grp_id = 0; grp_id < grp_count; grp_id++) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmptm_rm_tcam_fp_entry_segment_update(unit,
                                                             ltid,
                                                             ltid_info,
                                                             stage_fc,
                                                             pipe_id,
                                                             grp_ptr[grp_id],
                                                             seg_id));
            }

            seg_id--;
        }

    }

    /* Cleanup new segment */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_entry_segment_clear(unit,
                                               ltid,
                                               ltid_info,
                                               stage_fc,
                                               pipe_id,
                                               new_segment_id));

    grp_count = 0;
    first_fg = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
        if (SHR_BITGET(group_segments[primary_segment_id].w, idx)) {
            /* Get the field group pointer for all groups
               in current segment.
             */
            grp_ptr[grp_count] = &(first_fg[idx]);
            grp_count++;
        }
    }

    /* Update auto expansion data to group and stage */
    SHR_BITSET(stage_entry_seg_bmp[0].w,new_segment_id);
    for (grp_id = 0; grp_id < grp_count; grp_id++) {
        SHR_BITSET(grp_ptr[grp_id]->entry_seg_bmp.w, new_segment_id);
    }

exit:
    SHR_FREE(group_segments);
    SHR_FUNC_EXIT();
}

/* for segment moves during multi mode groups sharig same slices.*/
static int
bcmptm_rm_tcam_fp_group_segment_expand(int unit,
                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_fp_group_t  *pri_fg = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t     *slice_fc = NULL;
    uint8_t pri_slice_id = 0;
    int pipe_id = 0; /* Pipe Id */
    bcmptm_rm_tcam_group_mode_t pri_group_mode;
    uint8_t slice_count = 0;
    uint8_t width_iter = 0;
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;

    SHR_FUNC_ENTER(unit);

    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;

    pipe_id = entry_attr->pipe_id;
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }
    /* Initialize field stage pointer. */
    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;

    /* Initialize field slice pointer. */
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;

    /* Current group ptr */
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_fp_group_info_get(unit,
                                           iomd->ltid,
                                           iomd->ltid_info,
                                           entry_attr->pipe_id,
                                           entry_attr->group_id,
                                           stage_fc,
                                           &fg));
    /* Primary slice index */
    pri_slice_id = fg->primary_slice_id;

    /* Primary group pointer */
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          entry_attr->pipe_id,
                                          slice_fc[pri_slice_id].primary_grp_id,
                                          stage_fc,
                                          &pri_fg));

    pri_group_mode = pri_fg->group_mode;

    if (BCMPTM_RM_TCAM_GRP_MODE_SINGLE == pri_group_mode
            || BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == pri_group_mode) {
        slice_count = 1;
    } else if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == pri_group_mode ||
            (BCMPTM_RM_TCAM_GRP_MODE_QUAD == pri_group_mode &&
             stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE)) {
        slice_count = 2;
    } else if (BCMPTM_RM_TCAM_GRP_MODE_TRIPLE == pri_group_mode &&
            stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
        slice_count = 3;
    }

    /* Allocate the required segments for the auto expanded slices */
    for (width_iter = 0; width_iter < slice_count; width_iter++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_entry_segment_expand(unit,
                                                    iomd->ltid,
                                                    iomd->ltid_info,
                                                    stage_fc,
                                                    entry_attr));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_entry_insert(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    uint8_t new_slice_id;
    int rv = SHR_E_NONE;
    bool new_group = FALSE;

    SHR_FUNC_ENTER(unit);

    if (iomd->req_flags & BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL) {
        /* Check if entry already exists or not. */
        if (iomd->req_info->same_key == true) {
            /* If already looked up, use the results */
            if (same_key_info[unit].index != -1) {
                /* Write the entry words to HW for
                 * all pipes than primary pipe
                 */
                iomd->target_index = same_key_info[unit].index;
                iomd->hw_only = TRUE;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd));
                iomd->hw_only = FALSE;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
            }

            if (same_key_info[unit].prev_index != -1) {
                /*
                 * Clear the entry words from HW for
                 * all pipes than primary pipe
                 */
                iomd->target_index = same_key_info[unit].prev_index;
                iomd->hw_only = TRUE;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_eid_entry_clear(unit, iomd));
                iomd->hw_only = FALSE;
            }
            SHR_EXIT();
        }
    }

    /* Initialize field stage pointer. */
    stage_fc = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;
    entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)iomd->entry_attrs;
    if (!(stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) &&
        SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }
    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    if (group_ptr == NULL) {
         rv = bcmptm_rm_tcam_fp_slice_allocate(unit, iomd);
         /* That means this is the all entries in the slice are used up,
          * expand the group by allocating slices based on the group mode
          * and priority.
          */
         if (rv == SHR_E_RESOURCE) {
         /* Compress the existing groups to make
          * space for the new one.
          */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_fp_group_compress(unit, iomd));
             /* Try the slice expand again after compression */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_fp_slice_allocate(unit, iomd));
        } else if (rv != SHR_E_NONE){
             SHR_ERR_EXIT(rv);
        }
        /* get the group pointer */
        group_ptr = iomd->group_ptr;
        new_group = TRUE;
    } else {
        if (group_ptr->group_mode != entry_attr->group_mode ||
            group_ptr->priority != entry_attr->group_prio ||
            group_ptr->group_ltid != entry_attr->group_ltid ||
            group_ptr->pipe_id != entry_attr->pipe_id) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    /* get the presel entry flag */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {
        /* insert presel entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entry_insert(unit,iomd));
        /* Set the default presel entry flag to group */
        if (SHR_BITGET(entry_attr->flags.w,
                       BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT)) {
            group_ptr->flags |= BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT;
        }

    } else {
         rv = bcmptm_rm_tcam_prio_eid_entry_insert(unit,
                                               iomd);
         SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_FULL);
         if (rv == SHR_E_FULL &&
             (group_ptr->flags & BCMPTM_RM_TCAM_F_AUTO_EXPANSION)) {
             if (!SHR_BITGET(entry_attr->flags.w,
                 BCMPTM_FP_FLAGS_AUTO_EXPAND_ENABLE)) {
                SHR_ERR_EXIT(SHR_E_FULL);
             }
             /* That means this is the all entries in the slice are used up,
              * expand the group by allocating slices based on the group mode
              * and priority.
              */
             rv = bcmptm_rm_tcam_fp_slice_expand(unit, &new_slice_id, iomd);
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);

             if (rv == SHR_E_RESOURCE) {
                 /* Compress the existing groups to make
                  * space for the new one.
                  */
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_group_compress(unit, iomd));
                 /* Try the slice expand again after compression */
                 SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_slice_expand(unit,
                                                                &new_slice_id,
                                                                iomd));
             }

             /* Expand entry segments for Info and hash */
              rv = bcmptm_rm_tcam_fp_group_segment_expand(unit, iomd);

              SHR_IF_ERR_VERBOSE_EXIT(rv);
             /* Try the entry insert after expanding the group
              * Update iomd since new slice is allocated
              * Entry info array will change here
              */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd));

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_prio_eid_entry_insert(unit,iomd));

             /*
              * Copy all the presel entries from primary slice to
              * newly expanded slice.
              */
             if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmptm_rm_tcam_fp_presel_entries_copy(unit,
                                                            new_slice_id,
                                                            iomd));
             }
         }
    }
    /* For 1st entry insert in Fixed key group update slice id in attrs */
    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED &&
        new_group == TRUE) {
        entry_attr->slice_id = group_ptr->primary_slice_id;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entry_delete(int unit,
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
    bcmptm_rm_tcam_entry_attrs_t *entry_attr = NULL;
    bcmptm_rm_tcam_fp_group_t *group_ptr = NULL;
    bcmptm_rm_tcam_slice_t     *slice_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_ptr = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(iomd->ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd->group_ptr, SHR_E_NOT_FOUND);
    SHR_NULL_CHECK(iomd->pt_dyn_info, SHR_E_PARAM);

    if (iomd->entry_attrs != NULL) {
        entry_attr = (bcmptm_rm_tcam_entry_attrs_t *)(iomd->entry_attrs);
    } else {
         SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    group_ptr = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
    slice_ptr = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    stage_ptr = (bcmptm_rm_tcam_fp_stage_t *)iomd->stage_fc;

    /*
     * Should not release the group resources if it is last entry of
     * primary group in a multi mode sharing groups scenario
     */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP)) {

        if (iomd->multi_mode == TRUE) {
            if (slice_ptr[group_ptr->primary_slice_id].primary_grp_id ==
                 (int16_t) entry_attr->group_id) {
                /* Dont release the resources if it is multi mode sharing
                 * primary group, as all the slice_offsets,
                 * auto expansions are based on primary group num parts.
                 */
               LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "\nCannot delete last entry in widest group "
                 "in multi-mode group's sharing scenario.\n"
                 "Delete all other sharing group's entries and try deleting "
                 "widest group's last entry\n")));
               SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }
    /* get the presel entry flag */
    if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_presel_entry_delete(unit, iomd));
        /* if the default presel entry deleted,
         * Reset the default presel entry flag in the group structure
         */
        if (SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT)) {
            /* get the group pointer */
            group_ptr->flags &= ~BCMPTM_RM_TCAM_F_PRESEL_ENTRY_DEFAULT;
        }
    } else {

        SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_prio_eid_entry_delete(unit,
                                               iomd->req_flags,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               iomd->pt_dyn_info,
                                               iomd->req_info,
                                               iomd->rsp_info,
                                               iomd->rsp_ltid,
                                               iomd->rsp_flags,
                                               iomd));
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_fp_profiles_unreserve(unit, iomd));
    }

    if ((stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH &&
         SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP)) ||
        (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) &&
         SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP) &&
         !SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)))  {
        /* Release the group's resources */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_free_group_resources(unit, iomd));
    } else if (!(stage_ptr->flags & BCMPTM_STAGE_ENTRY_TYPE_HASH) &&
               !(SHR_BITGET(entry_attr->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY))
               && (iomd->multi_mode != true )) {
        /*
         * Check after freeing this entry, can we free a slice.
         * we need to account for reserve entry too for entry modifications.
         * num of free_entries - reserve entry should be equal to num entries in
         * a slice. in TH series, slice sizes may vary. that is the reason we
         * should consider last slice num entries to free a slice.
         */
        if ((iomd->free_entries - 1) >=
            iomd->last_slice_num_entries) {
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_fp_group_free_last_slice(unit, iomd));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entry_lookup(int unit,
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
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(req_info->entry_attrs);

    if (SHR_BITGET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_GROUP_INFO)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_group_entry_lookup(unit,iomd));
    } else if (SHR_BITGET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_STAGE_INFO)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_stage_entry_lookup(unit,iomd));
    } else if (SHR_BITGET(attrs->flags.w, BCMPTM_FP_FLAGS_GET_SLICE_INFO)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_slice_entry_lookup(unit,iomd));
    } else {

        /* Input parameter check. */
        SHR_NULL_CHECK(iomd->group_ptr, SHR_E_NOT_FOUND);
        SHR_NULL_CHECK(iomd->pt_dyn_info, SHR_E_PARAM);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_lookup(unit,
                                                  req_flags,
                                                  ltid,
                                                  ltid_info,
                                                  pt_dyn_info,
                                                  req_info,
                                                  rsp_info,
                                                  rsp_ltid,
                                                  rsp_flags,
                                                  index,
                                                  iomd));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entry_move(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               uint8_t partition,
                               uint32_t prefix,
                               uint32_t from_idx,
                               uint32_t to_idx,
                               void *entry_attrs,
                               bcmbd_pt_dyn_info_t *pt_dyn_info,
                               bcmptm_rm_tcam_entry_iomd_t *iomd)
{

    uint16_t from_index = 0;
    uint16_t to_index = 0;
    uint32_t num_presel_per_slice;
    uint16_t pri_from_index = 0;
    uint16_t pri_to_index = 0;
    uint8_t slice_id = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices = 0;        /* Number of Slices */
    uint32_t orig_depth = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);

    attrs = (bcmptm_rm_tcam_entry_attrs_t *)(entry_attrs);

    /* Move primary entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_entry_move(unit,
                                        ltid,
                                        ltid_info,
                                        partition,
                                        prefix,
                                        from_idx,
                                        to_idx,
                                        attrs,
                                        pt_dyn_info,
                                        iomd));

    /* get the presel entry flag */
    if (SHR_BITGET(attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY)) {

       fg = (bcmptm_rm_tcam_fp_group_t *)iomd->group_ptr;
       slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
       num_slices = iomd->num_slices;
       /* get the primary slice index */
       slice_id = fg->primary_slice_id;
       /* get the number of presel entries in that slice */
       SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                      unit, ltid, ltid_info, attrs->pipe_id,
                                      slice_id,
                                      &num_presel_per_slice));

       pri_from_index = (from_idx) - (slice_id * num_presel_per_slice);
       pri_to_index = (to_idx) - (slice_id * num_presel_per_slice);

       while (slice_id < num_slices
                     && (0 != slice_fc[slice_id].next_slice_id_offset[attrs->group_mode])) {
            /* go to the next slice if exists */
            slice_id = slice_id +
                        slice_fc[slice_id].next_slice_id_offset[attrs->group_mode];
            /* get the number of presel entries in that slice */
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_presel_entries_per_slice_get(
                                          unit, ltid, ltid_info, attrs->pipe_id,
                                          slice_id,
                                          &num_presel_per_slice));

           from_index = (slice_id * num_presel_per_slice) + pri_from_index;
           to_index = (slice_id * num_presel_per_slice) + pri_to_index;
           orig_depth = iomd->entry_info_arr.entry_info_depth[0];
           iomd->entry_info_arr.entry_info_depth[0] +=
                                (slice_id * num_presel_per_slice);
           /* Move expanded entry */
           SHR_IF_ERR_VERBOSE_EXIT
               (bcmptm_rm_tcam_prio_eid_entry_move(unit,
                                               ltid,
                                               ltid_info,
                                               partition,
                                               prefix,
                                               from_index,
                                               to_index,
                                               entry_attrs,
                                               pt_dyn_info,
                                               iomd));
           iomd->entry_info_arr.entry_info_depth[0] = orig_depth;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_group_entry_segment_get(int unit,
                                          bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                          uint8_t slice_count,
                                          uint16_t *segment_id)
{
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    uint16_t num_slices = 0;
    uint16_t seg_id = -1;
    bcmptm_fp_entry_seg_bmp_t *entry_seg_bmp = NULL;
    int pipe_id =0;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                         entry_attr->pipe_id,
                                         entry_attr->stage_id,
                                         &stage_fc));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->group_id,
                                          stage_fc,
                                          &fg));

    entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t*) (((uint8_t *)stage_fc)
                     + stage_fc->entry_seg_bmp_offset);

    num_slices = stage_fc->num_slices;

    pipe_id = entry_attr->pipe_id;
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }
    for (seg_id = 0; seg_id < num_slices; seg_id++) {
         if (!(SHR_BITGET((entry_seg_bmp[0]).w,seg_id))) {
            SHR_BITSET((entry_seg_bmp[0]).w,seg_id);
            break;
         }
    }

    *segment_id = seg_id;

    /* Cleanup new segment */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_entry_segment_clear(unit,
                                               ltid,
                                               ltid_info,
                                               stage_fc,
                                               pipe_id,
                                               seg_id));

    /* Segment for an entry is decided by its pipe
     * and the segment_id inside the pipe.
     * Total entry space is divided into pipe count of parts.
     * In every part, one segment is allocated for the group.
     */

    SHR_BITSET(fg->entry_seg_bmp.w,*segment_id);

    while (slice_count > 1) {
        /* Expand entry segments for Info */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_entry_segment_expand(unit,
                                                    ltid,
                                                    ltid_info,
                                                    stage_fc,
                                                    entry_attr));
        slice_count--;

    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_presel_entries_clear(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       uint8_t new_slice_id)
{
    int16_t to_idx = -1;
    uint16_t idx = 0;
    uint32_t num_presel_per_slice;
    int8_t pri_slice_id = -1;
    uint8_t part = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    uint8_t num_slices = 0;
    uint32_t entry_count = 0;
    uint8_t slice_id = 0;
    size_t entry_words_size = BCMPTM_MAX_PT_FIELD_WORDS;
    bcmptm_rm_tcam_req_t req_info;
    bcmptm_rm_tcam_rsp_t rsp_info;
    uint32_t *entry_key_words = NULL;
    uint32_t *entry_data_words = NULL;
    uint32_t *ekw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    uint32_t *edw_row[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
    bcmptm_rm_tcam_lt_info_t presel_ltid_info;
    bcmltd_sid_t  presel_ltid;
    int size = 0;
    bcmptm_rm_tcam_entry_iomd_t *iomd_temp = NULL;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    int *next_slice_id_offset = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd->pt_dyn_info, SHR_E_PARAM);

    ltid_info = iomd->ltid_info;

    SHR_ALLOC(entry_attrs,
              sizeof(bcmptm_rm_tcam_entry_attrs_t),
              "bcmptmRmtcamEntryAttrsCopy");

    sal_memcpy(entry_attrs,
               iomd->entry_attrs,
               sizeof(bcmptm_rm_tcam_entry_attrs_t));

    size = sizeof(bcmptm_rm_tcam_entry_iomd_t);
    SHR_ALLOC(iomd_temp, size, "bcmptmRmTcamEntryIomdTemp");
    sal_memcpy(iomd_temp, iomd, size);
    iomd_temp->entry_attrs = (void *)entry_attrs;

    /* Get the number of presel entries in that slice */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_presel_entries_per_slice_get(unit,
                                                     iomd->ltid,
                                                     iomd->ltid_info,
                                                     entry_attrs->pipe_id,
                                                     new_slice_id,
                                                     &num_presel_per_slice));
    fg = iomd->group_ptr;
    num_slices = iomd->num_slices;
    slice_fc = (bcmptm_rm_tcam_slice_t *)iomd->slice_fc;
    pri_slice_id = fg->primary_slice_id;

    slice_id = pri_slice_id;
    next_slice_id_offset = slice_fc[slice_id].next_slice_id_offset;
    while (slice_id < num_slices &&
          (next_slice_id_offset[entry_attrs->group_mode] != 0)) {
        entry_count += num_presel_per_slice;
        slice_id += next_slice_id_offset[entry_attrs->group_mode];
        next_slice_id_offset = slice_fc[slice_id].next_slice_id_offset;
        if (slice_id == new_slice_id) {
            break;
        }
    }

    if (slice_id != new_slice_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(entry_key_words,
              (sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS),
              "bcmptmRmtcamEntrykeyWords");
    SHR_ALLOC(entry_data_words,
              (sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS),
              "bcmptmRmtcamEntryDataWords");
    for (part = 0; part < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; part++) {
        ekw_row[part] = entry_key_words + (part * BCMPTM_MAX_PT_FIELD_WORDS);
        edw_row[part] = entry_data_words + (part * BCMPTM_MAX_PT_FIELD_WORDS);
    }
    req_info.entry_attrs = (void *)entry_attrs;
    req_info.rsp_ekw_buf_wsize = entry_words_size;
    req_info.rsp_edw_buf_wsize = entry_words_size;
    req_info.ekw = ekw_row;
    req_info.edw = edw_row;
    rsp_info.rsp_ekw = ekw_row;
    rsp_info.rsp_edw = edw_row;
    if (ltid_info->rm_more_info->shared_lt_count) {
        presel_ltid = ltid_info->rm_more_info->shared_lt_info[0];
        sal_memset(&presel_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_ptrm_info_get(unit, presel_ltid, 0, 0,
                                         ((void *)&presel_ltid_info),
                                         NULL, NULL));
    } else {
        presel_ltid = iomd->ltid;
        presel_ltid_info = *iomd->ltid_info;
    }

    iomd_temp->ltid = presel_ltid;
    iomd_temp->ltid_info = &presel_ltid_info;
    /*
     * Group entries might be installed per pipe in global pipe aware mode,
     * but the, presel entries needs to be copied to all pipes when group
     * is auto expanded to new slices.
     */
    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_dyn_info.index = iomd_temp->pt_dyn_info->index;
    pt_dyn_info.tbl_inst = fg->pipe_id;
    iomd_temp->pt_dyn_info = &pt_dyn_info;
    /*
     * Loop for all presel entries per slice and copy the non-empty entries
     * from primaryslice to the newly expanded slice. copy for the width
     * parts of the TCAM are taken care by hw read/write functions.
     */
    iomd_temp->req_info = &req_info;
    iomd_temp->rsp_info = &rsp_info;
    /* Set the presel entry flag so that presel entry info is fetched */
    SHR_BITSET(entry_attrs->flags.w,BCMPTM_FP_FLAGS_PRESEL_ENTRY);
    /* Update the iomd temp for the presel. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd_temp));
    entry_info = iomd_temp->entry_info_arr.entry_info[0][0];
    sal_memset(entry_key_words, 0,
               sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS);
    sal_memset(entry_data_words, 0,
               sizeof(uint32_t) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS * \
               BCMPTM_MAX_PT_FIELD_WORDS);

    for (idx = 0; idx < num_presel_per_slice; idx++) {

        /* calculate the to indexs */
        to_idx = entry_count + idx;

        /* If the entry is empty in primary slice skip it. */
        if (entry_info[idx].entry_id == BCMPTM_RM_TCAM_EID_INVALID) {
            continue;
        }

        /* Write the entry words to to_idx. */
        iomd_temp->target_index = to_idx;
        iomd_temp->hw_only = true;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_write(unit, iomd_temp));
        iomd_temp->hw_only = false;
    }

exit:
    SHR_FREE(iomd_temp);
    SHR_FREE(entry_attrs);
    SHR_FREE(entry_key_words);
    SHR_FREE(entry_data_words);
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_entry_segment_compress(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     uint32_t group_id,
                     int pipe,
                     bcmptm_rm_tcam_fp_stage_id_t stage_id,
                     uint8_t num_slices_freed,
                     bool group_delete)
{
    bcmptm_rm_tcam_fp_group_t  *fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *first_fg = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *group_ptr = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *group_ptr_update = NULL;
                               /* Field Group structure pointer. */
    bcmptm_rm_tcam_fp_group_t  *grp_ptr[32] = {NULL};
                               /* Field Group structure pointer. */
    bool free_segment = 0;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;
    int seg_id = -1;
    int segments_free = 0;
    int first_free_segment_id = -1;
    int last_segment_id = -1;
    int first_segment_id = -1;
    bcmptm_fp_entry_seg_bmp_t *stage_entry_seg_bmp = NULL;
    bcmptm_fp_grp_seg_bmp_t *group_segments = NULL;
    uint8_t num_slices = 0;
    int grp_count = 0;
    uint8_t found = 0;
    int idx = 0;
    uint32_t grp_idx = 0;
    int grp_id = -1;
    int pipe_id = 0;
    uint8_t clear_flag ;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_fp_stage_info_get(unit, ltid, ltid_info,
                                         pipe,
                                         stage_id,
                                         &stage_fc));
    num_slices = stage_fc->num_slices;

    SHR_ALLOC(group_segments, (sizeof(bcmptm_fp_grp_seg_bmp_t)
              * num_slices), "bcmptmRmtcamFPEntrySegment");

    /* Clear group_segment bitmap */
    sal_memset(group_segments, 0 , (sizeof(bcmptm_fp_grp_seg_bmp_t) * num_slices));

    stage_entry_seg_bmp = (bcmptm_fp_entry_seg_bmp_t*)
                           (((uint8_t *)stage_fc)
                           + stage_fc->entry_seg_bmp_offset);

    /* Create group segment bitmap */
    fg = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    for (idx = 0; idx < stage_fc->num_groups; idx++) {
         if ((fg->valid == 1) && (fg->pipe_id == pipe)) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                 if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                     SHR_BITSET(group_segments[seg_id].w,
                                idx);
                 }
             }
         }
         fg++;
    }

    if (pipe == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    } else {
        pipe_id = pipe;
    }

    /* Initialize field group pointer to current group. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          pipe_id,
                                          group_id,
                                          stage_fc,
                                          &fg));

    /* Initialize field group pointer */
    first_fg = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

    if (!group_delete) {
        for (seg_id = 0; seg_id < num_slices; seg_id++) {
            free_segment = 0;
            if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                /* check for free segment */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_entry_segment_check(unit,
                                                           ltid,
                                                           ltid_info,
                                                           stage_fc,
                                                           pipe_id,
                                                           seg_id,
                                                           &free_segment));
                /* If free clear bitmaps for all associated groups */
                if (free_segment && (segments_free < num_slices_freed)) {
                    segments_free++;
                    /* Update auto compression data to stage */
                    SHR_BITCLR(stage_entry_seg_bmp[0].w, seg_id);
                    for (idx = 0; idx < stage_fc->num_groups; idx++) {
                         if (SHR_BITGET(group_segments[seg_id].w, idx)) {
                             /* Get the field group pointer for all groups
                                in current segment.
                              */
                             group_ptr = &(first_fg[idx]);
                             /* Update auto compression data to group */
                             SHR_BITCLR(group_ptr->entry_seg_bmp.w,seg_id);

                         }
                    }
                }
            }
        }

    } else {
        for (seg_id = 0; seg_id < num_slices; seg_id++) {
            clear_flag = TRUE;
            if (SHR_BITGET(fg->entry_seg_bmp.w,seg_id)) {
                SHR_BITCLR(fg->entry_seg_bmp.w,seg_id);
                /* Check if any other group is using the same
                 * seg id
                 */
                for (grp_idx = 0; grp_idx < stage_fc->num_groups; grp_idx++) {
                    if ((first_fg[grp_idx].group_id != fg->group_id) &&
                         SHR_BITGET(group_segments[seg_id].w, grp_idx)) {
                         clear_flag = FALSE;
                    }
                }
                /* Mark seg id is free only if not shared with
                 * other groups
                 */
                if (clear_flag) {
                    SHR_BITCLR(stage_entry_seg_bmp[0].w, seg_id);
                }
            }
        }
        fg->entry_info_seg = 0;
        fg->entry_hash_size =0;
        fg->entry_hash_seg = 0;
        fg->presel_entry_info_seg = 0;
        fg->presel_entry_hash_size = 0;
        fg->presel_entry_hash_seg = 0;

    }

    /* Find first free segment */
    for (seg_id = 0;
         seg_id < num_slices; seg_id++) {
         if (!(SHR_BITGET(stage_entry_seg_bmp[0].w,seg_id))) {
            first_free_segment_id = seg_id;
            found = 1;
            break;
         }
    }
    /* some segments are freed */
    if (found == 1) {
        /* find first segment block to move */
        for (seg_id = first_free_segment_id;
             seg_id < num_slices; seg_id++) {
             if ((SHR_BITGET(stage_entry_seg_bmp[0].w,seg_id))) {
                first_segment_id = seg_id;
                break;
             }
        }

        /* All segments are free, no moves needed */
        if (first_segment_id == -1) {
            SHR_EXIT();
        }

        /* find last segment block to move */
        for (seg_id = first_free_segment_id;
             seg_id < num_slices; seg_id++) {
             if ((SHR_BITGET(stage_entry_seg_bmp[0].w,seg_id))) {
                last_segment_id = seg_id;
             }
        }
    } else {
        /* No free segments (Eg: slice sharing groups) */
        SHR_EXIT();
    }


    seg_id = first_segment_id;
    while (seg_id <= last_segment_id) {
          grp_count = 0;
            for (idx = 0; idx < stage_fc->num_groups; idx++) {
                if (SHR_BITGET(group_segments[seg_id].w, idx)) {
                    /* Get the field group pointer for all groups
                       in current segment.
                     */
                    grp_ptr[grp_count] = &(first_fg[idx]);
                    grp_count++;
                }
            }

            if (grp_count == 0) {
                /* No groups are available.
                * Ideally should not hit this condition.
                */
               SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }


         /* Move segment from seg_id to first_free_segment */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_fp_entry_segment_move(unit,
                                                   ltid,
                                                   ltid_info,
                                                   stage_fc,
                                                   pipe_id,
                                                   seg_id,
                                                   first_free_segment_id));
         /* Cleanup previous segment */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_fp_entry_segment_clear(unit,
                                                    ltid,
                                                    ltid_info,
                                                    stage_fc,
                                                    pipe_id,
                                                    seg_id));
         /* Update bitmaps */
         SHR_BITCLR(stage_entry_seg_bmp[0].w, seg_id);
         SHR_BITSET(stage_entry_seg_bmp[0].w, first_free_segment_id);
         for (grp_id = 0; grp_id < grp_count; grp_id++) {
             SHR_BITCLR(grp_ptr[grp_id]->entry_seg_bmp.w, seg_id);
             SHR_BITSET(grp_ptr[grp_id]->entry_seg_bmp.w,
                        first_free_segment_id);
         }

         /* Move next */
         seg_id++;
         first_free_segment_id++;
    }

     /* Fetch the group segment start pointer */
    group_ptr_update = (bcmptm_rm_tcam_fp_group_t*) (((uint8_t *)stage_fc)
                  + stage_fc->group_info_seg);

   /* Update all groups to point to first segment in its list */
    for (idx =0;idx < stage_fc->num_groups; idx++) {
        if ((group_ptr_update->valid == 1)
             && (group_ptr_update->pipe_id == pipe)) {
             for (seg_id = 0; seg_id < num_slices; seg_id++) {
                 if (SHR_BITGET(group_ptr_update->entry_seg_bmp.w,seg_id)) {
                     SHR_IF_ERR_VERBOSE_EXIT
                         (bcmptm_rm_tcam_fp_entry_segment_update(unit,
                                                             ltid,
                                                             ltid_info,
                                                             stage_fc,
                                                             pipe_id,
                                                             group_ptr_update,
                                                             seg_id));
                     break;

                 }
             }
        }

        group_ptr_update = group_ptr_update + 1;
    }


exit:
    SHR_FREE(group_segments);
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_get_table_info(int unit,
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
    bcmptm_table_info_t *table_info = NULL;
    uint8_t num_slices = 0;
    uint32_t slice_id = 0;
    uint32_t num_entries = 0;
    uint8_t num_pipes = 0;
    uint32_t pipe_id = 0;
    uint32_t num_entries_per_slice = 0;
    bcmptm_rm_tcam_group_mode_t ref_grp_mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info->rsp_misc, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info->entry_attrs,SHR_E_PARAM);

    /* Fetch the TCAM information for the given ltid, pipe_id. */
    table_info = (bcmptm_table_info_t *)rsp_info->rsp_misc;
    *rsp_ltid = ltid;

    if (iomd->slice_fc == NULL) {
        table_info->entry_limit = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_max_entry_mode_get(unit,
                                              ltid_info,
                                              &ref_grp_mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_pipe_config_get(unit,
                                        ltid,
                                        ltid_info,
                                        ltid_info->rm_more_info[0].pipe_count,
                                        &num_pipes));
    for (pipe_id =0; pipe_id < num_pipes; pipe_id++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                ltid, ltid_info,
                                                pipe_id,
                                                &num_slices));
        for (slice_id =0; slice_id < num_slices; slice_id++) {
            SHR_IF_ERR_VERBOSE_EXIT(
             bcmptm_rm_tcam_stage_attr_entries_per_slice_get(
                                    unit, ltid, ltid_info,
                                    slice_id, pipe_id, ref_grp_mode,
                                    &num_entries_per_slice));
            num_entries += num_entries_per_slice;
        }
    }
    table_info->entry_limit = num_entries;

exit:
    /* Log the function exit. */
    SHR_FUNC_EXIT();
}

