/*! \file rm_tcam_prio_only_index_mgmt.c
 *
 *  Resource(TCAM index) allocation utilities for prio only
 *  TCAM LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include <bcmptm/bcmptm_rm_tcam_fp.h>
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

#define IS_ENTRY_INFO_FREE(entry_info, idx, prio, entry_type)             \
        ((entry_info[idx].entry_pri == BCMPTM_RM_TCAM_PRIO_INVALID) &&    \
        ((prio && entry_info[idx].entry_type == entry_type) ||            \
        entry_info[idx].entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID))

#define IS_ENTRY_INFO_KEY_TYPE(entry_info, idx, entry_type)         \
        ((entry_info[idx].entry_type == entry_type) &&              \
        (entry_info[idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID))

#define INDEX_IN_RANGE(a,b,c) (((a > b) && (a < c)) || (( a > c) && (a < b)))

/*******************************************************************************
 * Public Functions
 */

/*
 * Create a new free slot to insert an entry. This routine is used
 * in case there are no entries existing in LTID. In this case first
 * free slot can be used for allocation.
 */
static int
bcmptm_rm_tcam_prio_only_entry_first_free_index_allocate(uint32_t unit,
                                          bcmptm_rm_tcam_entry_iomd_t *iomd,
                                          uint32_t start_idx,
                                          uint32_t end_idx,
                                          uint32_t num_key_rows,
                                          uint32_t entry_start_boundary,
                                          uint16_t entry_type,
                                          int *target_index)
{
    uint32_t idx =0;
    uint32_t idx_temp = 0;
    uint32_t count = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(target_index, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_info = iomd->entry_info_arr.entry_info[0][0];
    for (idx = start_idx; idx < end_idx; idx += num_key_rows) {

        if (IS_ENTRY_INFO_FREE(entry_info, idx, 0, entry_type)){
            if (num_key_rows == 1 &&
                entry_start_boundary == 0) {
                *target_index = idx;
                break;
            } else if ((idx % entry_start_boundary) == 0) {
                /*
                 * Entry index should be multiple of start boundary.
                 * check for entry type number of free entries.
                 */
                count = 1;
                idx_temp = idx;
                idx_temp++;
                while (count != num_key_rows) {
                    if (IS_ENTRY_INFO_FREE(entry_info,
                                           idx_temp,
                                           0,
                                           entry_type)) {
                        count++;
                        idx_temp++;
                    } else {
                        /* Got num_key_rows continious entries */
                        break;
                    }
                }
                if (count == num_key_rows) {
                    *target_index = idx;
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_only_validate_alloc_space(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int index,
                                        int pipe_id,
                                        uint8_t prio_change)
{
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint16_t entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_type = iomd->ltid_info->hw_entry_info->key_type_val[0][0];
    entry_info = iomd->entry_info_arr.entry_info[0][0];

    if (IS_ENTRY_INFO_FREE(entry_info,
                           index,
                           prio_change,
                           entry_type)) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_only_validate_alloc_space_prio(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int index,
                                        int entry_pri,
                                        int pipe_id,
                                        int start_index,
                                        int end_index,
                                        uint8_t prio_change)
{
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint16_t entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    int32_t temp_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_type = iomd->ltid_info->hw_entry_info->key_type_val[0][0];
    entry_info = iomd->entry_info_arr.entry_info[0][0];

    SHR_IF_ERR_EXIT
        (bcmptm_rm_tcam_prio_only_validate_alloc_space(unit,
                                                       iomd,
                                                       index,
                                                       pipe_id,
                                                       prio_change));
    temp_index = index;
    while (temp_index >= start_index) {
        if (IS_ENTRY_INFO_KEY_TYPE(entry_info,
                                   temp_index,
                                   entry_type)) {
            if (entry_info[temp_index].entry_pri < entry_pri) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
        temp_index--;
    }
    temp_index = index;
    while (temp_index < end_index) {
        if (IS_ENTRY_INFO_KEY_TYPE(entry_info,
                                   temp_index,
                                   entry_type)) {
            if (entry_info[temp_index].entry_pri > entry_pri) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
        temp_index++;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_compress_array(uint32_t *array,
                              uint32_t index,
                              uint32_t num_key_rows)
{
    uint32_t idx;
    array[index] = 0;
    for (idx = index; idx < (num_key_rows - 1); idx++) {
        array[index] = array[index + 1];
    }
    return SHR_E_NONE;
}

static int
bcmptm_rm_tcam_remove_common_and_compress(uint32_t *from_idx,
                                          uint32_t *to_idx,
                                          int32_t *num_key_rows)
{
    uint32_t idx, idx1;
    uint32_t from_index = 0;
    uint32_t key_rows;
    key_rows = *num_key_rows;

    for (idx = 0; idx < key_rows; idx++) {
        from_index = from_idx[idx];
        for (idx1= 0; idx1 < key_rows; idx1++) {
            if (from_index == to_idx[idx1]) {
                bcmptm_rm_tcam_compress_array(to_idx,
                                              idx1,
                                              key_rows);
                bcmptm_rm_tcam_compress_array(from_idx,
                                              idx,
                                              key_rows);
                key_rows--;
            }
        }
    }
    *num_key_rows = key_rows;
    return SHR_E_NONE;
}

static int
bcmptm_rm_tcam_create_space_in_shared_tcam(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd,
                                           bool *space_allocated)
{
    bcmptm_rm_tcam_prio_only_shr_info_t *shared_info = NULL;
    uint16_t shared_info_count;
    int idx = 0;
    bcmltd_sid_t cand_ltid = -1;
    bcmptm_rm_tcam_lt_info_t shr_ltid_info;
    bcmptm_rm_tcam_prio_only_entry_info_t *shr_entry_info = NULL;
    bcmptm_rm_tcam_prio_only_info_t *shr_tcam_info = NULL;
    uint16_t entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    int32_t ent_idx;
    int32_t temp_idx;
    int32_t start_index = 0;
    int32_t end_index = 0;
    uint32_t slice_boundry;
    uint32_t *from_idx = NULL;
    uint32_t *to_idx = NULL;
    int32_t from_index = 0;
    int32_t to_index = 0;
    uint32_t validate_index = 0;
    bool from_idx_found = FALSE;
    int32_t num_key_rows = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info_debug = NULL;
    bcmptm_rm_tcam_prio_only_info_t shr_tcam_info_debug;
    uint32_t debug_flags = 0;
    bcmptm_rm_tcam_entry_iomd_t *shared_iomd = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(space_allocated, SHR_E_PARAM);

    *space_allocated = FALSE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_shared_info_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  &shared_info,
                                                  &shared_info_count));
    if (shared_info_count == 0) {
        SHR_EXIT();
    }
    size = iomd->ltid_info->hw_entry_info->num_key_rows * sizeof(uint32_t);
    SHR_ALLOC(from_idx,size,"bcmptmFromIndex");
    sal_memset(from_idx,0,size);
    SHR_ALLOC(to_idx,size,"bcmptmToIndex");
    sal_memset(from_idx,0,size);

    while (idx < (shared_info_count - 1)) {
        /*
         * Check whether 2 continuous shared info
         * has same ltid. If it is has same ltid
         * it is a candidate for movement to create space.
         */
        cand_ltid = -1;
        if (shared_info[idx].shr_ltid
            != shared_info[idx + 1].shr_ltid) {
            idx++;
            continue;
        }
        /*
         * If more than one shared info has same ltid
         * find the overlapping start and end index and
         * slice boundry. We should move entries such
         * that same entry in multiple slice should be free
         */
        start_index = shared_info[idx].shr_start_index;
        cand_ltid = shared_info[idx].shr_ltid;
        slice_boundry = shared_info[idx].end_index + 1;
        idx++;
        while ((idx < shared_info_count)
            && (shared_info[idx].shr_ltid == cand_ltid)){
            end_index = shared_info[idx].shr_end_index;
            idx++;
        }
        sal_memset(&shr_ltid_info,0,sizeof(shr_ltid_info));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          cand_ltid,
                                          0,
                                          0,
                                          ((void *)&shr_ltid_info),
                                          NULL,
                                          NULL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                    cand_ltid,
                                                    &shr_ltid_info,
                                                    iomd->pipe_id,
                                                    &shr_tcam_info));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                     cand_ltid,
                                                     &shr_ltid_info,
                                                     iomd->pipe_id,
                                                     &shr_entry_info));
        entry_type = shr_ltid_info.hw_entry_info->key_type_val[0][0];
        ent_idx = end_index;
        num_key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
        /*
         * Currently not optimized for minimizing the moves
         * Algorithm uses the last 2 free entries
         * Can be optimized later to minimize the moves.
         * Go over the indexes and find an index which can be used
         * the corresponding entries will be updated in the from_idx.
         * condition for finding usable index, entries in other slices
         * should also belong to same type or should be free.
         */
        while (ent_idx >= start_index) {
            if (from_idx_found == FALSE) {
                if (IS_ENTRY_INFO_FREE(shr_entry_info,
                                       ent_idx,
                                       0,
                                       entry_type) ||
                    IS_ENTRY_INFO_KEY_TYPE(shr_entry_info,
                                           ent_idx,
                                           entry_type)) {
                    temp_idx = ent_idx;
                    from_index = 0;
                    from_idx[from_index++] = temp_idx;
                    temp_idx -=slice_boundry;
                    while (temp_idx > start_index) {
                        if (IS_ENTRY_INFO_FREE(shr_entry_info,
                                               temp_idx,
                                               0,
                                               entry_type) ||
                            IS_ENTRY_INFO_KEY_TYPE(shr_entry_info,
                                                   temp_idx,
                                                   entry_type)) {
                            from_idx[from_index++] = temp_idx;
                            temp_idx -=slice_boundry;
                        } else {
                            sal_memset(from_idx, 0, size);
                            break;
                        }
                        if (from_index >= num_key_rows) {
                            from_idx_found = TRUE;
                            break;
                        }
                    }
                }
            }
            ent_idx--;
        }
    }

    if ((from_idx_found == TRUE)) {
        ent_idx= shr_tcam_info->num_entries - 1;
        to_index = 0;
        while (ent_idx >= 0) {
            if (IS_ENTRY_INFO_FREE(shr_entry_info,
                                   ent_idx,
                                   0,
                                   entry_type) &&
                        to_index < num_key_rows) {
                to_idx[to_index++] = ent_idx;
            }
            if ((to_index >= num_key_rows) &&
                (from_idx_found == TRUE)) {
                *space_allocated = TRUE;
                break;
            }
            ent_idx--;
        }
    }

    /*
     * Space allocated is true when target index and free
     * index is found.Move the free index to target index.
     * if one of the free index position changes during move,
     * update the free index location with the new location.
     * Free index location will change if the free index
     * falls in move range.
     */

    if (*space_allocated == TRUE) {
        validate_index = from_idx[num_key_rows - 1];
        if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            size = (sizeof(bcmptm_rm_tcam_prio_only_entry_info_t) *
                    end_index);
            SHR_ALLOC(entry_info_debug, size, "bcmptmRmTcamEntryInfoDebug");
            sal_memcpy(entry_info_debug, shr_entry_info, size);
            size = sizeof(bcmptm_rm_tcam_prio_only_info_t);
            sal_memcpy(&shr_tcam_info_debug, shr_tcam_info, size);
        }

        /* Allocate shared iomd and update it. */
        size = sizeof(bcmptm_rm_tcam_entry_iomd_t);
        SHR_ALLOC(shared_iomd, size, "bcmptmRmTcamSharedIomd");
        sal_memset(shared_iomd, 0, size);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_iomd_init(unit,
                                                iomd->req_flags,
                                                iomd->trans_id,
                                                cand_ltid,
                                                &shr_ltid_info,
                                                iomd->pt_dyn_info,
                                                iomd->req_op,
                                                iomd->req_info,
                                                iomd->rsp_info,
                                                iomd->rsp_ltid,
                                                iomd->rsp_flags,
                                                shared_iomd));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_iomd_update(unit, shared_iomd));

        bcmptm_rm_tcam_remove_common_and_compress(from_idx,
                                                  to_idx,
                                                  &num_key_rows);
        for (from_index = num_key_rows - 1;
             from_index >= 0;
             from_index--) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_move_range(unit,
                                                           shared_iomd,
                                                           from_idx[from_index],
                                                           to_idx[from_index],
                                                           entry_type));
        }
        if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_prio_only_validate_alloc_space(unit,
                                                               iomd,
                                                               validate_index,
                                                               iomd->pipe_id,
                                                               0));
            debug_flags |= BCMPTM_RM_TCAM_SKIP_RESERVED_ERROR_CHECK;
            SHR_IF_ERR_EXIT
                (bcmptm_rm_tcam_prio_only_validate_entry_info(unit,
                                                              iomd,
                                                              debug_flags));
        }
        SHR_EXIT();
    }
exit:
    SHR_FREE(entry_info_debug);
    SHR_FREE(from_idx);
    SHR_FREE(to_idx);
    SHR_FREE(shared_iomd);
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_reserve_entries_for_ltid(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int index,
                                        int8_t entry_type,
                                        uint8_t lookup_id,
                                        bool clear)
{
    uint32_t entry_start_boundary = 0;
    uint32_t start_idx = 0;
    uint32_t end_idx = 0;
    uint32_t num_key_rows = 0;
    uint32_t idx = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    int target_idx = BCMPTM_RM_TCAM_INDEX_INVALID;
    bool shared_reorder = FALSE;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    target_idx = index;
    tcam_info = iomd->tcam_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_num_index_required_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               &num_key_rows));

    entry_info = iomd->entry_info_arr.entry_info[0][0];
    if (iomd->ltid_info->hw_entry_info->view_type) {
        /*
         * num_key_rows is used to traverse the entry_info for allocation
         * entry_info is kept for lowest view type. So need
         * to adjust according to the view type so that it works for
         * wider view type
         */
        num_key_rows = (num_key_rows *
                        iomd->ltid_info->hw_entry_info->view_type);
    }

    if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
        entry_start_boundary =
                    iomd->ltid_info->hw_entry_info->entry_boundary;
        if (tcam_info->num_lookups == 2) {
            if (lookup_id == 1) {
                start_idx = 0;
                end_idx = tcam_info->num_entries/2;
            } else {
                start_idx = tcam_info->num_entries/2;
                end_idx = tcam_info->num_entries;
            }
        } else {
            start_idx = 0;
            end_idx = tcam_info->num_entries;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_first_free_index_allocate(unit,
                                                       iomd,
                                                       start_idx,
                                                       end_idx,
                                                       num_key_rows,
                                                       entry_start_boundary,
                                                       entry_type,
                                                       &target_idx));
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if (!(tcam_info->flags & BCMPTM_RM_TCAM_SHARED_REORDER_DISABLED)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_create_space_in_shared_tcam(unit,
                                                           iomd,
                                                           &shared_reorder));
            }
            if (shared_reorder == TRUE) {
                SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_first_free_index_allocate(unit,
                                                           iomd,
                                                           start_idx,
                                                           end_idx,
                                                           num_key_rows,
                                                           entry_start_boundary,
                                                           entry_type,
                                                           &target_idx));
            }
        }
    }
    if (target_idx != BCMPTM_RM_TCAM_INDEX_INVALID) {
        for (idx = target_idx; idx < (target_idx + num_key_rows); idx++) {
            if (clear == TRUE) {
                entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
                entry_info[idx].entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
                if (lookup_id == 1) {
                    tcam_info->reserved_entry_index1 =
                               BCMPTM_RM_TCAM_INDEX_INVALID;
                } else {
                    tcam_info->reserved_entry_index2 =
                               BCMPTM_RM_TCAM_INDEX_INVALID;
                }
                tcam_info->res_ent_key_type_val =
                           BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
            } else {
                entry_info[idx].entry_pri = 0;
                entry_info[idx].entry_type = entry_type;
                if (lookup_id == 1) {
                    tcam_info->reserved_entry_index1 = target_idx;
                } else {
                    tcam_info->reserved_entry_index2 = target_idx;
                }

                tcam_info->res_ent_key_type_val = entry_type;
            }
            entry_info[idx].offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
            if (tcam_info->shr_info_count != 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_only_shared_info_update(unit,
                                                                 iomd,
                                                                 clear,
                                                                 idx));
            }
            entry_info[idx].entry_pri = BCMPTM_RM_TCAM_PRIO_INVALID;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_entry_index_allocate(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd,
                               uint8_t prio_change,
                               uint32_t *target_index,
                               uint8_t *reserve_used,
                               bool *shared_reorder)
{
    int target_idx = BCMPTM_RM_TCAM_INDEX_INVALID;
    int idx = -1;
    int idx_temp = -1;
    int up_free_block_first_idx = -1;
    int up_free_idx = -1;
    int down_free_idx = -1;
    int from_idx = -1;
    int to_idx = -1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint32_t num_key_rows;
    uint32_t count = 0;
    uint32_t entry_start_boundary = 0;
    int last_higher_prio_entry_type_idx = -1;
    uint16_t start_idx = 0;
    uint16_t end_idx = 0;
    uint8_t sid_row = 0;
    uint32_t num_entries;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bool per_slice_entry_mgmt = FALSE;
    uint16_t entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    uint16_t num_entries_ltid = 0;
    uint16_t num_entries_traversed = 0;
    bool least_prio_entry_type_idx = FALSE;
    int32_t reserved_index = 0;
    uint32_t free_count = 0;
    int debug_flags = 0;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info_debug = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(target_index, SHR_E_PARAM);

    entry_attrs = iomd->entry_attrs;
    SHR_NULL_CHECK(entry_attrs, SHR_E_PARAM);

    /*
     * Per slice management is true for flex digest LT's
     * where LT is mapped to multiple TCAM's and moves
     * and inserts are managed per TCAM.
     */
    if (entry_attrs != NULL) {
        if (entry_attrs->per_slice_entry_mgmt == TRUE) {
            per_slice_entry_mgmt = TRUE;
        } else {
            per_slice_entry_mgmt = FALSE;
        }
    } else {
        per_slice_entry_mgmt = FALSE;
    }

    tcam_info = iomd->tcam_info;
    num_entries_ltid = tcam_info->num_entries_ltid;
    entry_start_boundary =
                iomd->ltid_info->hw_entry_info->entry_boundary;

    /* Fetch the entry type details */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_num_index_required_get(unit,
                                               iomd->ltid,
                                               iomd->ltid_info,
                                               &num_key_rows));

    /*
     * If free entries are not available, entry cannot be inserted
     * Even though HW ataomicity is supported, one entry(precisely,
     * it is number of physical entries mapped to single LT entry)
     * must be free always to support entry priority changes.
     */
    num_entries = tcam_info->num_entries;
    /*
     * In case of aggregate with key rows 2
     * unused entries will be based on number of key rows in single wide.
     * Divide it by view type to get the unused entries for this LT.
     */
    entry_type = iomd->ltid_info->hw_entry_info->key_type_val[0][0];
    if (iomd->ltid_info->hw_entry_info->view_type) {
        /*
         * num_key_rows is used to traverse the entry_info for
         * allocation entry_info is kept for lowest view type.
         * So need to adjust according to the view type so that
         * it works for wider view type
         */
        num_key_rows = (num_key_rows *
                        iomd->ltid_info->hw_entry_info->view_type);
    }

    entry_info = iomd->entry_info_arr.entry_info[0][0];

    reserved_index = tcam_info->reserved_entry_index1;

    if (per_slice_entry_mgmt == TRUE) {
        sid_row = entry_attrs->slice_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_logical_space_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              sid_row,
                                              &start_idx,
                                              &end_idx));
        end_idx = end_idx + 1;
        if (iomd->ltid_info->hw_entry_info->view_type) {
            end_idx = (end_idx *
                       iomd->ltid_info->hw_entry_info->view_type);
        }
    } else if (tcam_info->num_lookups == 2) {
        if (entry_attrs->lookup_id == 1) {
            start_idx = 0;
            end_idx = num_entries / 2;
        } else {
            start_idx = num_entries / 2;
            end_idx = num_entries;
            reserved_index = tcam_info->reserved_entry_index2;
        }
        num_entries_ltid = num_entries_ltid / 2;
    } else {
        start_idx = 0;
        end_idx = num_entries;
    }

    if ((num_entries_ltid == 0) &&
                per_slice_entry_mgmt == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_first_free_index_allocate(unit,
                                                       iomd,
                                                       start_idx,
                                                       end_idx,
                                                       num_key_rows,
                                                       entry_start_boundary,
                                                       entry_type,
                                                       &target_idx));

        if (target_idx != BCMPTM_RM_TCAM_INDEX_INVALID) {
            *target_index = target_idx;
            SHR_EXIT();
        }
    }
    entry_type = tcam_info->res_ent_key_type_val;
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        size = (sizeof(bcmptm_rm_tcam_prio_only_entry_info_t) *
                num_entries);
        SHR_ALLOC(entry_info_debug,
                  size,
                  "bcmptmRmTcamEntryInfoDebug");
        sal_memcpy(entry_info_debug, entry_info, size);
    }
    /* Main loop to find the target_index */
    for (idx = start_idx; idx < end_idx; idx += num_key_rows) {

        /*
         * Record the last free tcam index before target index is found.
         * In the case of prio change, reserved entry is considered free entry
         * else only free entries are considered.
         * IS_ENTRY_INFO_FREE macros takes care of above conditions.
         */
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if (IS_ENTRY_INFO_FREE(entry_info,
                                   idx,
                                   prio_change,
                                   entry_type)) {
                free_count++;
                if (num_key_rows == 1 &&
                    entry_start_boundary == 0) {
                    if (TRUE == least_prio_entry_type_idx) {
                        up_free_idx = idx;
                        break;
                    }
                    up_free_idx = idx;
                    /*
                     * Record first index when there a block of
                     * free entries.
                     */
                    if (up_free_block_first_idx == -1) {
                        up_free_block_first_idx = idx;
                    }
                    continue;
                } else if ((idx % entry_start_boundary) == 0) {
                    /*
                     * Entry index should be multiple of start boundary.
                     * check for entry type number of free entries.
                     */
                    count = 1;
                    idx_temp = idx;
                    idx_temp++;
                    while (count != num_key_rows) {
                        if (IS_ENTRY_INFO_FREE(entry_info,
                                               idx_temp,
                                               prio_change,
                                               entry_type)) {
                            count++;
                            idx_temp++;
                        } else {
                            /* Count get num_key_rows continious entries. */
                            break;
                        }
                    }
                    if (count == num_key_rows) {
                        if (TRUE == least_prio_entry_type_idx) {
                            up_free_idx = idx;
                            break;
                        }
                        up_free_idx = idx;
                        /*
                         * Record first index when there a block of
                         * free entries.
                         */
                        if (up_free_block_first_idx == -1) {
                            up_free_block_first_idx = idx;
                        }
                        continue;
                    }
                }
            }
        }

        /* Record the target index for the given prioirty. */
        if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            if (entry_info[idx].entry_type ==
                iomd->ltid_info->hw_entry_info->key_type_val[0][0] &&
                entry_info[idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID) {
                num_entries_traversed++;
                if (entry_info[idx].entry_pri <
                    iomd->req_info->entry_pri) {
                    target_idx = idx;
                    /*
                     * If free entries block is just above the target index
                     * up_free_block_first_idx is the target index
                     * for the new entry
                     */
                    if (up_free_block_first_idx != -1) {
                        break;
                    }
                    continue;
                } else {
                    /*
                     * Free entries block is just before entry with same prio.
                     * use up_free_block_first_idx as the target index
                     */
                    if (num_entries_ltid == num_entries_traversed) {
                        least_prio_entry_type_idx = TRUE;
                    }
                    if ((up_free_block_first_idx != -1) &&
                        (entry_info[idx].entry_pri ==
                         iomd->req_info->entry_pri)) {
                        break;
                    }
                    up_free_block_first_idx = -1;
                    last_higher_prio_entry_type_idx = idx;
                    continue;
                }
            }
            /* Reset up_free_block_first_idx to -1 */
            if (up_free_block_first_idx != -1) {
                up_free_block_first_idx = -1;
            }
        }

        /* Record the first free tcam index after target index is found. */
        if (IS_ENTRY_INFO_FREE(entry_info,idx,prio_change,entry_type)) {
            free_count++;
            if (num_key_rows == 1 &&
                entry_start_boundary == 0) {
                down_free_idx = idx;
                break;
            } else if ((idx % entry_start_boundary) == 0) {
                /* Entry index should be multiple of start boundary */
                /* Check for entry type number of free entries */
                count = 1;
                idx_temp = idx;
                idx_temp++;
                while (count != num_key_rows) {
                    if (IS_ENTRY_INFO_FREE(entry_info,
                                           idx_temp,
                                           prio_change,
                                           entry_type)) {
                        count++;
                        idx_temp++;
                    } else {
                        /* Count get 4 continious entries*/
                        break;
                    }
                }
                if (count == num_key_rows) {
                    down_free_idx = idx;
                    break;
                }
            }
        }
    }

    /*
     * Ideally should not hit this case as the above loop will be
     * executed only if atleast one free index is available in TCAM.
     */
    if (up_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID &&
        down_free_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
        if (num_key_rows == 1) {
        /*
         * This case is hit when multiple LT's share same resources,
         * and when resources are exhausted in the TCAM.
         */
            if (shared_reorder != NULL) {
                if (tcam_info->flags & BCMPTM_RM_TCAM_SHARED_REORDER_DISABLED) {
                    SHR_ERR_EXIT(SHR_E_FULL);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_create_space_in_shared_tcam(unit,
                                                           iomd,
                                                           shared_reorder));
                if (*shared_reorder== TRUE) {
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }
            SHR_ERR_EXIT(SHR_E_FULL);
        } else if (free_count > num_key_rows) {
        /*
         * Free entries are present but couldnot allocate
         * as they are not continuous. call compression API
         */
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        SHR_ERR_EXIT(SHR_E_FULL);
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
        /*
         * All existing valid entries have greater priority than
         * target entry.
         */
        else if (target_idx == BCMPTM_RM_TCAM_INDEX_INVALID) {
            /*
             * Approach: find the first entry from bottom of the
             * entry_info[idx] which is free or which is of its
             * entry_type where the first entry of this type is
             * found.
             */
            /*
             * Check if last higher prio entry is -1, if so then
             * this entry is first entry of this entry_type
             * so set target_idx = up_free_idx found
             */
             if (last_higher_prio_entry_type_idx == -1) {
                  target_idx = up_free_idx;
             }
             /*
              * Some entries of this entry_type exists but this
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
            /*
             * Check if last higher prio entry is less than up_free_idx,
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

    if (from_idx != to_idx) {
        /* Move entries either up or down ward direction. */
        entry_type =iomd-> ltid_info->hw_entry_info->key_type_val[0][0];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_move_range(unit,
                                                       iomd,
                                                       from_idx,
                                                       to_idx,
                                                       entry_type));
    }
    /*
     * Below code is used to update the reserved entry.
     * In the case of prio change, reserve entry can be used if it is the
     * nearest free entry. In this case update the reserve entry to a new
     * entry. This is done by the caller.
     * Some additional error checks are done. Reserve entry should never
     * be in move range in the case of prio change since reserve entry should
     * get allocated in prio change case. Error checks can be removed later.
 */
    if (prio_change == 1) {
        if (reserved_index == to_idx) {
            if (reserve_used ) {
                *reserve_used = 1;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        } else {
            if (from_idx < to_idx) {
                if (INDEX_IN_RANGE(reserved_index, from_idx, to_idx)) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else if (from_idx > to_idx) {
                if (INDEX_IN_RANGE(reserved_index, to_idx, from_idx)) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
        if ((*reserve_used == 1) && (from_idx != to_idx)) {
            debug_flags |= BCMPTM_RM_TCAM_SKIP_RSVD_IDX_CHECK;
        }
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_validate_entry_info(unit,
                                                          iomd,
                                                          debug_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_validate_alloc_space_prio(unit,
                                                     iomd,
                                                     target_idx,
                                                     iomd->req_info->entry_pri,
                                                     iomd->pipe_id,
                                                     start_idx,
                                                     end_idx,
                                                     prio_change));
    }
    *target_index = target_idx;

exit:
    SHR_FREE(entry_info_debug);
    SHR_FUNC_EXIT();
}

