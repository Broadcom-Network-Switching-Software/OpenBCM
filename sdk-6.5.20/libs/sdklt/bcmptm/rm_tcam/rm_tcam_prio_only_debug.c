/*! \file rm_tcam_prio_only_debug.c
 *
 * Utilities to debug the issues in prio_only tcam resources
 * management driver. These functions will run only in debug
 * mode.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include <bcmptm/bcmptm_rm_tcam_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_tcam_prio_only_find_loops(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id)
{
    uint32_t idx = 0, idx2 = 0;
    int *entry_hash = NULL;
    int offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int first_offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    int offset2 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t num_offsets = 0;
    uint32_t num_entries = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe_id,
                                                &tcam_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                 ltid,
                                                 ltid_info,
                                                 pipe_id,
                                                 &entry_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_hash_get(unit,
                                                 ltid,
                                                 ltid_info,
                                                 pipe_id,
                                                 1,
                                                 &entry_hash));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit, ltid, ltid_info, &num_entries));
    if (ltid_info->hw_entry_info->view_type != 0) {
        num_entries = num_entries * ltid_info->hw_entry_info->view_type;
    }
    for (idx = 0; idx < tcam_info->entry_hash_size; idx++) {
         first_offset = (int)entry_hash[idx];

         if (first_offset < (int)num_entries &&
             first_offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
             offset1 = (int)(entry_info[first_offset].offset);
         }
         num_offsets = 0;
         while (offset1 != BCMPTM_RM_TCAM_OFFSET_INVALID) {

             if (offset1 >= (int)num_entries) {
                 SHR_ERR_EXIT(SHR_E_INTERNAL);
             }
             offset2 = (int)(entry_info[first_offset].offset);
             for (idx2 = 0; idx2 < num_offsets; idx2++) {

                 if (offset2 >= (int)num_entries) {
                     SHR_ERR_EXIT(SHR_E_INTERNAL);
                 }
                 if (offset2 == offset1) {
                     SHR_ERR_EXIT(SHR_E_INTERNAL);
                 }
                 offset2 = (int)(entry_info[offset2].offset);
             }
             num_offsets++;
             offset1 = (int)(entry_info[offset1].offset);
         }
         offset1 = BCMPTM_RM_TCAM_OFFSET_INVALID;
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_check_shared_index(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmptm_rm_tcam_prio_only_info_t *tcam_info,
                                  int pipe_id,
                                  uint8_t num_key_rows,
                                  uint32_t index,
                                  int flags)
{
    bcmptm_rm_tcam_prio_only_info_t *target_tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *target_entry_info = NULL;
    bcmptm_rm_tcam_prio_only_shr_info_t *shared_info = NULL;
    bcmltd_sid_t target_ltid;
    bcmptm_rm_tcam_lt_info_t target_ltid_info;
    uint8_t shr_idx;
    uint16_t shared_info_count = 0;
    uint32_t target_index;
    uint32_t idx;
    int16_t key_type_val = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    uint8_t is_half_mode = 0;
    uint8_t shr_half_mode = 0;
    uint32_t offset = 0;
    bool is_next_index_considered = FALSE;
    uint8_t mode_different = FALSE;
    uint32_t depth = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_prio_only_shared_info_get(unit,
                                                  ltid,
                                                  ltid_info,
                                                  pipe_id,
                                                  &shared_info,
                                                  &shared_info_count));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_check_butterfly_mode(unit,
                                             ltid,
                                             ltid_info,
                                             &is_half_mode));

    for (shr_idx = 0; shr_idx < shared_info_count; shr_idx++) {
        /*
         * Check if the LTID TCAM index falls under any of
         * the shared TCAM index range.
         */
        mode_different = FALSE;
        is_next_index_considered = FALSE;
        if (shared_info[shr_idx].start_index <= index &&
            shared_info[shr_idx].end_index >= index) {
            /*
             * Target index is TCAM index in target LTID that
             * is mapped to TCAM index in the current LTID.
             */
            target_ltid = shared_info[shr_idx].shr_ltid;
            sal_memset(&target_ltid_info,
                       0,
                       sizeof(bcmptm_rm_tcam_lt_info_t));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_ptrm_info_get(unit,
                                              target_ltid,
                                              0,
                                              0,
                                              ((void *)&target_ltid_info),
                                              NULL,
                                              NULL));
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_check_butterfly_mode(unit,
                                                     target_ltid,
                                                     &target_ltid_info,
                                                     &shr_half_mode));

            offset = index - shared_info[shr_idx].start_index;
            if (1 == is_half_mode  && 0 == shr_half_mode) {
                offset = offset / 2;
                mode_different = TRUE;
            } else if (0 == is_half_mode  && 1 == shr_half_mode) {
                offset = offset * 2;
                is_next_index_considered = TRUE;
                mode_different = TRUE;
            }
            target_index = offset + shared_info[shr_idx].shr_start_index;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                       target_ltid,
                                                       &target_ltid_info,
                                                       pipe_id,
                                                       &target_tcam_info));
            /* Get the target LTID TCAM info. */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                       target_ltid,
                                                       &target_ltid_info,
                                                       pipe_id,
                                                       &target_entry_info));
            key_type_val = target_ltid_info.hw_entry_info->key_type_val[0][0];
            for (idx = target_index;
                 idx < (target_index + num_key_rows);
                 idx++) {
                if ((target_entry_info[idx].entry_type ==
                     key_type_val) ||
                    ((target_entry_info[idx].entry_type ==
                      BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID) &&
                     (target_entry_info[idx].entry_pri ==
                      BCMPTM_RM_TCAM_PRIO_INVALID))) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            if (is_next_index_considered == 1) {
                idx = target_index + 1;
                if ((target_entry_info[idx].entry_type ==
                     key_type_val) ||
                    ((target_entry_info[idx].entry_type ==
                      BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID) &&
                     (target_entry_info[idx].entry_pri ==
                      BCMPTM_RM_TCAM_PRIO_INVALID))) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }

            if (ltid_info->hw_entry_info->view_type) {
                continue;
            }

            if ((mode_different == 1) &&
                (shared_info[shr_idx].shr_flags &
                 BCMPTM_RM_TCAM_BOTH_KEY_KEY0)) {
                offset = index -shared_info[shr_idx].start_index;
                target_index = offset + shared_info[shr_idx].shr_start_index;
                if (is_half_mode == TRUE) {
                    depth = shared_info[shr_idx].end_index -
                                shared_info[shr_idx].start_index;
                    depth = depth / 2;
                    if (offset > depth) {
                        continue;
                    }
                }
                idx = target_index;
                if ((target_entry_info[idx].entry_type == key_type_val) ||
                    ((target_entry_info[idx].entry_type ==
                      BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID) &&
                     (target_entry_info[idx].entry_pri ==
                      BCMPTM_RM_TCAM_PRIO_INVALID))) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_prio_only_validate_entry_info_internal(int unit,
                                        bcmltd_sid_t ltid,
                                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                                        int pipe_id,
                                        int flags)
{
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint32_t idx;
    uint32_t start_idx = 0;
    uint32_t end_idx;
    uint32_t num_key_rows;
    uint8_t key_type_val;
    uint32_t num_entries = 0;
    uint32_t urpf_start = 0;
    uint32_t num_res_rows = 0;
    int32_t prev_priority;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (pipe_id == -1) {
        pipe_id = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                ltid,
                                                ltid_info,
                                                pipe_id,
                                                &tcam_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                 ltid,
                                                 ltid_info,
                                                 pipe_id,
                                                 &entry_info));
    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_num_index_required_get(unit,
                                               ltid,
                                               ltid_info,
                                               &num_key_rows));
    if (ltid_info->hw_entry_info->view_type) {
        /*
         * num_key_rows is used to traverse the entry_info for allocation
         * entry_info is kept for lowest view type. So need
         * to adjust according to the view type so that it works for
         * wider view type
         */
        num_key_rows = num_key_rows * ltid_info->hw_entry_info->view_type;
    }
    start_idx = 0;
    if (tcam_info->num_entries < tcam_info->num_entries_ltid) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    prev_priority = BCMPTM_RM_TCAM_PRIO_INVALID;
    key_type_val = tcam_info->res_ent_key_type_val;
    if (tcam_info->num_entries_ltid > 0) {
        if (tcam_info->reserved_entry_index1 > tcam_info->num_entries) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (!(flags & BCMPTM_RM_TCAM_SKIP_RSVD_IDX_CHECK) &&
            ((entry_info[tcam_info->reserved_entry_index1].entry_pri
                             != BCMPTM_RM_TCAM_PRIO_INVALID) ||
            (entry_info[tcam_info->reserved_entry_index1].entry_type
                              != key_type_val))){
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_check_shared_index(unit,ltid,
                                               ltid_info,
                                               tcam_info,
                                               pipe_id,
                                               num_key_rows,
                                               tcam_info->reserved_entry_index1,
                                               flags));
        if ((ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) &&
            (tcam_info->num_entries_ltid > 1)){
            if (!(flags & BCMPTM_RM_TCAM_SKIP_RSVD_IDX_CHECK) &&
                ((entry_info[tcam_info->reserved_entry_index2].entry_pri
                                 != BCMPTM_RM_TCAM_PRIO_INVALID) ||
                (entry_info[tcam_info->reserved_entry_index2].entry_type
                                  != key_type_val))){
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_check_shared_index(unit,
                                             ltid,
                                             ltid_info,
                                             tcam_info,
                                             pipe_id,
                                             num_key_rows,
                                             tcam_info->reserved_entry_index2,
                                             flags));
        }
    } else if (tcam_info->reserved_entry_index1 !=
               BCMPTM_RM_TCAM_INDEX_INVALID ) {
        if (!(flags & BCMPTM_RM_TCAM_SKIP_RESERVED_ERROR_CHECK)) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    end_idx = tcam_info->num_entries;
    urpf_start = end_idx / 2;
    for (idx = start_idx; idx < end_idx; idx += num_key_rows) {
        if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            if (idx == urpf_start) {
                prev_priority = BCMPTM_RM_TCAM_PRIO_INVALID;
            }
        }
        if (entry_info[idx].entry_type == key_type_val) {
            num_entries++;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_check_shared_index(unit,
                                                   ltid,
                                                   ltid_info,
                                                   tcam_info,
                                                   pipe_id,
                                                   num_key_rows,
                                                   idx,
                                                   flags));
            if ((prev_priority != BCMPTM_RM_TCAM_PRIO_INVALID) &&
                (entry_info[idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID)) {
                if (prev_priority < entry_info[idx].entry_pri) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
            if (entry_info[idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID) {
                prev_priority = entry_info[idx].entry_pri;
            }
        }
    }
    if ((tcam_info->num_lt_sharing == 0) &&
        (tcam_info->num_entries_ltid > 0)) {
        if (ltid_info->rm_more_info->flags &
            BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
            if (tcam_info->num_entries_ltid % num_key_rows) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            num_entries = num_entries * num_key_rows;
        }
        num_res_rows = num_key_rows;
        if ((ltid_info->rm_more_info->flags &
             BCMPTM_RM_TCAM_URPF_ENABLED_LT) &&
           (tcam_info->reserved_entry_index2 !=
            BCMPTM_RM_TCAM_INDEX_INVALID)) {
            num_res_rows = num_res_rows * 2;
        }
        if (flags & BCMPTM_RM_TCAM_SKIP_RSVD_IDX_CHECK) {
            num_res_rows -=  num_key_rows;
        }
        if (num_entries != (tcam_info->num_entries_ltid + num_res_rows))
        {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        SHR_FUNC_EXIT();
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_validate_entry_info(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int flags)
{
    uint32_t shared_count;
    bcmltd_sid_t shr_ltid;
    bcmptm_rm_tcam_lt_info_t shared_ltid_info;
    bcmptm_rm_tcam_prio_only_shr_info_t *shared_info;
    uint16_t shared_info_count;
    uint32_t idx;
    uint32_t idx1;
    uint8_t shared_tcam_info = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_prio_only_validate_entry_info_internal(unit,
                                                               iomd->ltid,
                                                               iomd->ltid_info,
                                                               iomd->pipe_id,
                                                               flags));
    shared_count = iomd->ltid_info->rm_more_info->shared_lt_count;

    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_prio_only_shared_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  &shared_info,
                                                  &shared_info_count));

    flags = 0;

    for (idx = 0; idx < shared_count ; idx++) {
        shr_ltid = iomd->ltid_info->rm_more_info->shared_lt_info[idx];
        if (iomd->ltid == shr_ltid) {
            continue;
        }
        shared_tcam_info = 1;
        for (idx1 =0; idx1 < shared_info_count; idx1++) {
            if (shared_info[idx1].shr_ltid == shr_ltid) {
                shared_tcam_info = 0;
                break;
            }
        }
        if (shared_tcam_info == 1) {
            continue;
        }
        sal_memset(&shared_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          shr_ltid,
                                          0,
                                          0,
                                          ((void *)&shared_ltid_info),
                                          NULL,
                                          NULL));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_validate_entry_info_internal(unit,
                                                          shr_ltid,
                                                          &shared_ltid_info,
                                                          iomd->pipe_id,
                                                          flags));
    }
exit:
    SHR_FUNC_EXIT();
}
