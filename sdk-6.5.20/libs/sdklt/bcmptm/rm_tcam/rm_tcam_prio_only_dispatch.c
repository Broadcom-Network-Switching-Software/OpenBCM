/*! \file rm_tcam_prio_only_dispatch.c
 *
 * Low Level Functions for pri_key(priority, Entry Key information)
 * based TCAMs. This file contains low level functions for pri_key
 * based TCAMs to do opertaions like TCAM entry insert, lookup, and
 * delete.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_util.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_atomicity.h"
#include "rm_tcam_traverse.h"
#include "rm_tcam.h"
#include <bcmptm/bcmptm_rm_tcam_fp.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Public Functions
 */
static int
bcmptm_rm_tcam_prio_only_entry_insert_internal(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int rv = SHR_E_NONE;
    uint32_t target_idx = -1;
    uint32_t found_index = -1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    int8_t entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    bool shared_reorder = FALSE;
    bool reserve_done = FALSE;
    uint32_t debug_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_attrs = iomd->entry_attrs;

    /* Check if entry already exists or not. */
    if (iomd->req_info->same_key == true &&
        entry_attrs->lookup_id == 1) {
        /* If already looked up, use the results */
        if (same_key_info[unit].index != -1) {
            found_index = same_key_info[unit].index;
            iomd->target_index = same_key_info[unit].index;
            rv = SHR_E_NONE;
        } else {
            found_index = -1;
            rv = SHR_E_NOT_FOUND;
        }
        /* For prio update check */
        iomd->rsp_info->rsp_entry_pri =
                        same_key_info[unit].priority;
    } else {
        /* Check if entry id exists or not. */
        rv = bcmptm_rm_tcam_prio_only_entry_lookup(unit,
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

    tcam_info = iomd->tcam_info;
    if (tcam_info->num_entries == 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    if (tcam_info->num_entries_ltid == 0 ||
        (entry_attrs->lookup_id == 2 &&
        tcam_info->num_entries_ltid == 1)) {
        entry_type =
            iomd->ltid_info->hw_entry_info->key_type_val[0][0];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                                iomd,
                                                BCMPTM_RM_TCAM_INDEX_INVALID,
                                                entry_type,
                                                entry_attrs->lookup_id,
                                                FALSE));
        reserve_done = TRUE;
    }

    if (rv == SHR_E_NONE) {
        /* Update the TCAM entry */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_entry_update(unit, iomd));
    } else {
        /* Find Index to Insert the TCAM entry */
        rv = bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                           iomd,
                                                           0,
                                                           &target_idx,
                                                           NULL,
                                                           &shared_reorder);
        if (rv != SHR_E_RESOURCE) {
            if (rv == SHR_E_FULL) {
                /*
                 * Fix for returning SHR_E_RESOURCE in case
                 * we run out of resources
                 */
                if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
                    debug_flags |= BCMPTM_RM_TCAM_SKIP_RESERVED_ERROR_CHECK;
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_tcam_prio_only_validate_entry_info(unit,
                                                                 iomd,
                                                                 debug_flags));
                }
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            /*call compress API and call allocate again */
            if (shared_reorder == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_prio_only_compress(unit, iomd));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_index_allocate(unit,
                                                               iomd,
                                                               0,
                                                               &target_idx,
                                                               NULL,
                                                               NULL));
        }
        /* Insert the new entry at target_idx. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_write(unit, iomd, 0, target_idx));

        /* Read the inserted data to the response */
        if (entry_attrs->lookup_id == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_read(unit, iomd, target_idx));
        }

        /* Fill in the entry attributes to the response */
        iomd->rsp_info->rsp_entry_pri = iomd->req_info->entry_pri;

        if (iomd->ltid_info->rm_more_info->flags &
            BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
            tcam_info->num_entries_ltid +=
                iomd->ltid_info->hw_entry_info->num_key_rows;
        } else {
            tcam_info->num_entries_ltid += 1;
        }
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_validate_entry_info(unit, iomd, 0));
    }
exit:
    if ((reserve_done == TRUE) && SHR_FUNC_VAL_IS(SHR_E_RESOURCE)) {
        /*
         * If reserved entry is allocated and inserted entry
         * is failed reserved entry is deleted. THis is needed
         * since reserved entry is inserted based on number of
         * entries for that LTID present
         */
        rv = bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                             iomd,
                                             tcam_info->reserved_entry_index1,
                                             entry_type,
                                             1,
                                             TRUE);
    }
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_prio_only_entry_delete_internal(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t index = BCMPTM_RM_TCAM_INDEX_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    int8_t entry_type = -1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;
    entry_attrs = iomd->entry_attrs;
    if (tcam_info->num_entries == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Check if entry id exists or not. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_lookup(unit,
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
        (bcmptm_rm_tcam_prio_only_entry_clear(unit, iomd, 0, index));

    if (iomd->ltid_info->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        tcam_info->num_entries_ltid -=
            iomd->ltid_info->hw_entry_info->num_key_rows;
    } else {
        tcam_info->num_entries_ltid -= 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_traverse_info_entry_delete(unit,
                                                   iomd->ltid,
                                                   iomd->ltid_info,
                                                   iomd->pt_dyn_info,
                                                   iomd->req_info));

    if (tcam_info->num_entries_ltid == 0) {
        entry_type = tcam_info->res_ent_key_type_val;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                            iomd,
                                            tcam_info->reserved_entry_index1,
                                            entry_type,
                                            1,
                                            TRUE));
        if (entry_attrs->lookup_id == 2) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_reserve_entries_for_ltid(unit,
                                            iomd,
                                            tcam_info->reserved_entry_index2,
                                            entry_type,
                                            2,
                                            TRUE));
        }
    }

    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_prio_only_validate_entry_info(unit, iomd, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_entry_insert(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint32_t num_lookups = 1;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    int rv;
    bcmptm_rm_tcam_lt_info_t *ltid_info;
    int *entry_hash_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    ltid_info = iomd->ltid_info;
    entry_attrs = iomd->entry_attrs;

    if (ltid_info->rm_more_info->flags &
                   BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        num_lookups = 2;
    }

    do {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_insert_internal(unit, iomd));
        num_lookups--;
        if (num_lookups > 0) {
           /* Increment the lookup_id to 2 in entry_attrs.*/
           entry_attrs->lookup_id++;
           entry_hash_ptr = iomd->entry_hash_ptr;
           iomd->entry_hash_ptr = iomd->entry_hash_ptr_2;
        }
    } while (num_lookups > 0);
exit:
    if (SHR_FUNC_VAL_IS(SHR_E_RESOURCE)) {
        if ((iomd->ltid_info->rm_more_info->flags &
             BCMPTM_RM_TCAM_URPF_ENABLED_LT) &&
            (num_lookups == 1)) {
            entry_attrs->lookup_id--;
            iomd->entry_hash_ptr = entry_hash_ptr;
            rv = bcmptm_rm_tcam_prio_only_entry_delete_internal(unit, iomd);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_entry_lookup(int unit,
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
    int rv = SHR_E_NONE;
    int *entry_hash = NULL;
    int offset = BCMPTM_RM_TCAM_OFFSET_INVALID;
    uint32_t hash_val = 0;
    uint32_t num_ekw_rows = 0;
    uint16_t start_idx = 0;
    uint16_t end_idx = 0;
    uint16_t num_entries = 0;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    same_key_info[unit].index = -1;
    same_key_info[unit].priority = -1;

    entry_attrs = iomd->entry_attrs;

    tcam_info = iomd->tcam_info;

    if (tcam_info->num_entries == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    entry_info = iomd->entry_info_arr.entry_info[0][0];
    entry_hash = iomd->entry_hash_ptr;
    hash_val = iomd->entry_hash_value;
    num_ekw_rows = iomd->num_index_required;
    offset = entry_hash[hash_val];
    num_entries = tcam_info->num_entries;

    while (offset != BCMPTM_RM_TCAM_OFFSET_INVALID) {
        if (entry_attrs->per_slice_entry_mgmt == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_logical_space_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  entry_attrs->slice_id,
                                                  &start_idx,
                                                  &end_idx));
            if (iomd->ltid_info->hw_entry_info->view_type) {
                end_idx =
                    (end_idx * iomd->ltid_info->hw_entry_info->view_type);
            }
            if (offset < start_idx || offset > end_idx) {
                offset = entry_info[offset].offset;
                continue;
            }
            num_entries = (end_idx - start_idx + 1);
        }
        if ((entry_info[offset].entry_type ==
            iomd->ltid_info->hw_entry_info->key_type_val[0][0]) &&
            (offset + num_ekw_rows) <= num_entries) {
            rv = bcmptm_rm_tcam_prio_only_entry_compare(unit, iomd, offset);
            if (SHR_SUCCESS(rv)) {
                if (index != NULL) {
                    *index = offset;
                }
                break;
            }
        }
        offset = entry_info[offset].offset;
    }

    if (offset == BCMPTM_RM_TCAM_OFFSET_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    iomd->target_index = offset;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_read(unit, iomd, offset));

    iomd->rsp_info->rsp_entry_pri = entry_info[offset].entry_pri;
    if (entry_attrs->lookup_id == 1) {
        same_key_info[unit].index = offset;
        same_key_info[unit].priority = entry_info[offset].entry_pri;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_prio_only_entry_delete(int unit,
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
    bcmptm_rm_tcam_entry_attrs_t *entry_attrs = NULL;
    uint8_t num_lookups = 1;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    tcam_info = iomd->tcam_info;

    if (tcam_info->num_entries == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    num_lookups = tcam_info->num_lookups;
    entry_attrs = iomd->entry_attrs;
    do {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_delete_internal(unit, iomd));
        num_lookups--;
        if (num_lookups > 0) {
           /* Increment the lookup_id to 2 in entry_attrs.*/
           entry_attrs->lookup_id++;
           iomd->entry_hash_ptr = iomd->entry_hash_ptr_2;
        }
    } while (num_lookups > 0);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_get_table_info(int unit,
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
    uint8_t num_pipes = 0;
    uint8_t pipe_count = 0;
    uint32_t max_limit = 0;
    uint32_t key_rows = 0;
    uint32_t unused_entries = 0;
    uint32_t num_entries = 0;
    uint8_t view_type = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    pipe_count = iomd->ltid_info->rm_more_info->pipe_count;
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmptm_rm_tcam_pipe_config_get(unit,
                                       iomd->ltid,
                                       iomd->ltid_info,
                                       pipe_count,
                                       &num_pipes));

    table_info = (bcmptm_table_info_t *)(iomd->rsp_info->rsp_misc);
    *rsp_ltid = ltid;

    if (iomd->ltid_info->hw_entry_info->num_depth_inst == 0) {
        table_info->entry_limit = 0;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_depth_get(unit,
                                      iomd->ltid,
                                      iomd->ltid_info,
                                      &num_entries));

        if (num_entries != 0) {
            max_limit = table_info->entry_limit;
            table_info->entry_limit = num_entries;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_num_index_required_get(unit,
                                                       iomd->ltid,
                                                       iomd->ltid_info,
                                                       &unused_entries));
            table_info->entry_limit =
                        table_info->entry_limit - unused_entries;
            table_info->entry_limit =
                        table_info->entry_limit * num_pipes;
            if (iomd->ltid_info->rm_more_info->flags &
                BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
                key_rows = iomd->ltid_info->hw_entry_info->num_key_rows;
                view_type = iomd->ltid_info->hw_entry_info->view_type;
                if (iomd->ltid_info->hw_entry_info->view_type) {
                    key_rows = (key_rows / view_type);
                }
                table_info->entry_limit =
                            (table_info->entry_limit / key_rows);
            }
            if (max_limit != 0) {
                table_info->entry_limit =
                            SHR_MIN(max_limit, table_info->entry_limit);
            }
        } else {
            table_info->entry_limit = 0;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

