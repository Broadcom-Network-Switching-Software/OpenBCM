/*! \file rm_tcam_traverse.c
 *
 * Functions for TCAM traversal
 * This file contains functions used for
 * priority only or priority eid based TCAM traversal.
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/bcmltd_lt_types.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_rm_tcam_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Private Functions
 */
static int
bcmptm_rm_tcam_traverse_next_index_get(int unit,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       int current_pipe,
                                       int current_index,
                                       int *next_pipe,
                                       int *next_index,
                                       int *next_prio)
{
    uint8_t num_pipes = 0;
    uint32_t num_entries = 0;
    uint32_t free_entries = 0;
    bcmptm_rm_tcam_prio_only_info_t *prio_only_tcam_info = NULL;
    uint8_t pipe_id = 0;
    uint32_t entry_idx = 0;
    bcmptm_rm_tcam_prio_eid_entry_info_t *prio_eid_entry_info = NULL;
    bcmptm_rm_tcam_prio_only_entry_info_t *prio_only_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(next_index, SHR_E_PARAM);
    SHR_NULL_CHECK(next_pipe, SHR_E_PARAM);

    if (current_pipe == -1) {
        current_pipe = 0;
    }
    if (current_index == -1) {
        current_index = 0;
    } else {
        current_index += 1;
        if (ltid_info->rm_more_info->flags &
            BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
            current_index +=
                (ltid_info->hw_entry_info[0].num_key_rows - 1);
        }
    }

    *next_pipe = -1;
    *next_index = -1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_pipe_config_get(unit,
                                        ltid,
                                        ltid_info,
                                        ltid_info->rm_more_info->pipe_count,
                                        &num_pipes));

    for (pipe_id = current_pipe;
         pipe_id < num_pipes;
         pipe_id++) {
        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                        ltid,
                                                        ltid_info,
                                                        pipe_id,
                                                        &prio_only_tcam_info));
            if (prio_only_tcam_info->num_entries_ltid == 0) {
                continue;
            }
            num_entries = prio_only_tcam_info->num_entries;
            if (ltid_info->rm_more_info->flags &
                           BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
                num_entries = num_entries / 2;
            }
        } else {
            iomd->pt_dyn_info->tbl_inst = pipe_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_eid_iomd_update(unit, iomd));
            num_entries = iomd->num_entries;
            free_entries = iomd->free_entries;
            if (free_entries == num_entries) {
                continue;
            }
        }

        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                     ltid,
                                                     ltid_info,
                                                     pipe_id,
                                                     &prio_only_entry_info));
        } else {
           prio_eid_entry_info = iomd->entry_info_arr.entry_info[0][0];
        }

        if (pipe_id != current_pipe) {
            current_index = 0;
        }

        for (entry_idx = current_index;
             entry_idx < num_entries;
             entry_idx++) {
            if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
                /* Skip index marked invalid */
                if (prio_only_entry_info[entry_idx].entry_pri ==
                    BCMPTM_RM_TCAM_PRIO_INVALID) {
                    if (ltid_info->rm_more_info->flags &
                        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
                        entry_idx +=
                            (ltid_info->hw_entry_info[0].num_key_rows - 1);
                    }
                    continue;
                }
                /* Skip if entry type is not matching with LTID info */
                if (prio_only_entry_info[entry_idx].entry_type !=
                    ltid_info->hw_entry_info->key_type_val[0][0]) {
                    if (ltid_info->rm_more_info->flags &
                        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
                        entry_idx +=
                            (ltid_info->hw_entry_info[0].num_key_rows - 1);
                    }
                    continue;
                }
                *next_prio = prio_only_entry_info[entry_idx].entry_pri;
                break;
            } else {
                if (prio_eid_entry_info[entry_idx].entry_id ==
                                       BCMPTM_RM_TCAM_EID_INVALID) {
                    continue;
                }
                *next_prio = prio_eid_entry_info[entry_idx].entry_pri;
                break;
            }
        }
        if (entry_idx != num_entries) {
            *next_pipe = pipe_id;
            *next_index = entry_idx;
            break;
        }
        if (ltid_info->rm_more_info->flags &
            BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
            entry_idx +=
                (ltid_info->hw_entry_info[0].num_key_rows - 1);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
/*
 * 1.If snapshot for LTID already exists, release old snapshot and construct
 *   new snapshot
 * 2.If snapshot for LTID is not found, construct new snapshot
 * 3.Copy hash and entry info from SW DB.
 * 4.Based on the entry info and entry_type read only KEY
 *   from pt_cache and build KEY DB.
 * 5.If first valid is found in snapshot, search for the same entry in ptcache
 *   and update ekw and edw in rsp_info.
 */
int
bcmptm_rm_tcam_traverse_entry_get_first_without_snapshot(int unit,
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
    int next_pipe_id = -1;
    int current_pipe_id = -1;
    int next_index = -1;
    int next_prio = -1;
    size_t size = 0;
    bcmptm_rm_tcam_rsp_t *new_rsp_info = NULL;
    uint32_t **rsp_ekw = NULL;
    uint8_t idx = 0;
    uint8_t num_ekw = 0;
    uint8_t part = 0;
    bool restore_pipe_id = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    /*
     * Get the next valid index. if current_index is -1 it will
     * get the first valid index.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_traverse_next_index_get(unit,
                                                ltid,
                                                ltid_info,
                                                iomd,
                                                -1,
                                                -1,
                                                &next_pipe_id,
                                                &next_index,
                                                &next_prio));
    /* There are no more valid entries in the LT. */
    if (next_index == -1) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    current_pipe_id = pt_dyn_info->tbl_inst;
    restore_pipe_id = TRUE;
    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        iomd->pipe_id = next_pipe_id;
        pt_dyn_info->tbl_inst = next_pipe_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                 iomd,
                                                 next_index));
        pt_dyn_info->tbl_inst = current_pipe_id;
    } else {
        iomd->target_index = next_index;
        iomd->pt_dyn_info->tbl_inst = next_pipe_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
        pt_dyn_info->tbl_inst = current_pipe_id;
    }
    rsp_info->rsp_entry_pri = next_prio;
    rsp_info->rsp_entry_pipe = next_pipe_id;

    /*
     * If the response is read from the HW, the key and mask values
     * are in XY format. But LTM can understand only DM format so
     * only key portion in rsp_info needs to be copied from the PTCache.
     */
    if ((iomd->req_flags & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        iomd->req_flags &= ~BCMLT_ENT_ATTR_GET_FROM_HW;

        size = sizeof(bcmptm_rm_tcam_rsp_t);

        SHR_ALLOC(new_rsp_info, size, "bcmptmRmtcamTraverseNewRsp");
        SHR_NULL_CHECK(new_rsp_info, SHR_E_MEMORY);
        sal_memset(new_rsp_info, 0, size);

        size = (sizeof(uint32_t *) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS);

        SHR_ALLOC(rsp_ekw, size, "bcmptmRmtcamTraverseNewRspEkw");
        SHR_NULL_CHECK(rsp_ekw, SHR_E_MEMORY);
        sal_memset(rsp_ekw, 0, size);
        new_rsp_info->rsp_ekw = rsp_ekw;

        new_rsp_info->rsp_edw = rsp_info->rsp_edw;

        size = (sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS);
        for (idx = 0; idx < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; idx++) {
            SHR_ALLOC(rsp_ekw[idx], size, "bcmptmRmtcamTraverseNewRspEkwPart");
            SHR_NULL_CHECK(rsp_ekw[idx], SHR_E_MEMORY);
            sal_memset(rsp_ekw[idx], 0, size);
        }

        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            pt_dyn_info->tbl_inst = next_pipe_id;
            iomd->pipe_id = next_pipe_id;
            iomd->rsp_info = new_rsp_info;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                     iomd,
                                                     next_index));
            pt_dyn_info->tbl_inst = current_pipe_id;
        } else {
            iomd->target_index = next_index;
            iomd->pt_dyn_info->tbl_inst = next_pipe_id;
            iomd->rsp_info = new_rsp_info;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
            iomd->rsp_info = rsp_info;
            pt_dyn_info->tbl_inst = current_pipe_id;
        }
        iomd->req_flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
        if (ltid_info->rm_more_info->flags &
                       BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            num_ekw = ltid_info->num_ekw / 2;
        } else {
            num_ekw = ltid_info->num_ekw;
        }
        for (part = 0; part < num_ekw; part++) {
            sal_memcpy(rsp_info->rsp_ekw[part],
                       new_rsp_info->rsp_ekw[part],
                       (sizeof(uint32_t) *
                       req_info->rsp_ekw_buf_wsize));
        }
    }

exit:
    if (restore_pipe_id == TRUE) {
        pt_dyn_info->tbl_inst = current_pipe_id;
    }
    if (new_rsp_info != NULL) {
        if (new_rsp_info->rsp_ekw != NULL) {
            for (idx = 0;
                 idx < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS;
                 idx++) {
                if (new_rsp_info->rsp_ekw[idx] != NULL) {
                    SHR_FREE(new_rsp_info->rsp_ekw[idx]);
                }
            }
            SHR_FREE(new_rsp_info->rsp_ekw);
        }
        SHR_FREE(new_rsp_info);
    }
    SHR_FUNC_EXIT();
}

/*
 * 1.If snapshot for LTID is not found, return SHR_E_UNAVAIL.
 * 2.Search for the key provided in traverse DB.
 * 3.If entry is not found, delete snapshot and return SHR_E_NOT_FOUND.
 * 4.Find the next valid entry in KEY DB.
 * 5.If next valid entry is not found, delete snapshot and return
 *   SHR_E_NOT_FOUND.
 * 6.If next valid entry is found in snapshot, search for the same entry
 *   in ptcache and update ekw and edw in rsp_info.
 */
int
bcmptm_rm_tcam_traverse_entry_get_next_without_snapshot(int unit,
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
    int next_pipe_id = -1;
    int next_index = -1;
    int next_prio = -1;
    int current_pipe_id = -1;
    int current_index = -1;
    uint32_t found_index = 0;
    size_t size = 0;
    bcmptm_rm_tcam_rsp_t *new_rsp_info = NULL;
    uint32_t **rsp_ekw = NULL;
    uint8_t idx = 0;
    uint8_t num_ekw = 0;
    uint8_t part = 0;
    bool restore_pipe_id = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_flags, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_lookup(unit,
                                                   req_flags,
                                                   ltid,
                                                   ltid_info,
                                                   pt_dyn_info,
                                                   req_info,
                                                   rsp_info,
                                                   rsp_ltid,
                                                   rsp_flags,
                                                   &found_index,
                                                   iomd));
    } else {
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
                                                  &found_index,
                                                  iomd));
    }
    current_pipe_id = pt_dyn_info->tbl_inst;
    restore_pipe_id = TRUE;
    current_index = found_index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_traverse_next_index_get(unit,
                                                ltid,
                                                ltid_info,
                                                iomd,
                                                current_pipe_id,
                                                current_index,
                                                &next_pipe_id,
                                                &next_index,
                                                &next_prio));
    /* There are no more valid entries in the LT. */
    if (next_index == -1) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        pt_dyn_info->tbl_inst = next_pipe_id;
        iomd->pipe_id = next_pipe_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                 iomd,
                                                 next_index));
        pt_dyn_info->tbl_inst = current_pipe_id;
        pt_dyn_info->index = current_index;
    } else {
        iomd->target_index = next_index;
        iomd->pt_dyn_info->tbl_inst = next_pipe_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
        iomd->pt_dyn_info->tbl_inst = current_pipe_id;
        iomd->pt_dyn_info->index = current_index;
    }
    rsp_info->rsp_entry_pri = next_prio;
    rsp_info->rsp_entry_pipe = next_pipe_id;

    /*
     * If the response is read from the HW, the key and mask values
     * are in XY format. But LTM can understand only DM format so
     * only key portion in rsp_info needs to be copied from the PTCache.
     */
    if ((iomd->req_flags & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        iomd->req_flags &= ~BCMLT_ENT_ATTR_GET_FROM_HW;

        size = sizeof(bcmptm_rm_tcam_rsp_t);

        SHR_ALLOC(new_rsp_info, size, "bcmptmRmtcamTraverseNewRsp");
        SHR_NULL_CHECK(new_rsp_info, SHR_E_MEMORY);
        sal_memset(new_rsp_info, 0, size);

        size = (sizeof(uint32_t *) * BCMPTM_RM_TCAM_MAX_WIDTH_PARTS);

        SHR_ALLOC(rsp_ekw, size, "bcmptmRmtcamTraverseNewRspEkw");
        SHR_NULL_CHECK(rsp_ekw, SHR_E_MEMORY);
        sal_memset(rsp_ekw, 0, size);
        new_rsp_info->rsp_ekw = rsp_ekw;

        new_rsp_info->rsp_edw = rsp_info->rsp_edw;

        size = (sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS);
        for (idx = 0; idx < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS; idx++) {
            SHR_ALLOC(rsp_ekw[idx], size, "bcmptmRmtcamTraverseNewRspEkwPart");
            SHR_NULL_CHECK(rsp_ekw[idx], SHR_E_MEMORY);
            sal_memset(rsp_ekw[idx], 0, size);
        }

        if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
            pt_dyn_info->tbl_inst = next_pipe_id;
            iomd->pipe_id = next_pipe_id;
            iomd->rsp_info = new_rsp_info;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_entry_read(unit,
                                                     iomd,
                                                     next_index));
            pt_dyn_info->tbl_inst = current_pipe_id;
            pt_dyn_info->index = current_index;
        } else {
            iomd->target_index = next_index;
            iomd->pt_dyn_info->tbl_inst = next_pipe_id;
            iomd->rsp_info = new_rsp_info;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_eid_entry_read(unit, iomd));
            iomd->rsp_info = rsp_info;
            pt_dyn_info->tbl_inst = current_pipe_id;
            pt_dyn_info->index = current_index;
        }
        iomd->req_flags |= BCMLT_ENT_ATTR_GET_FROM_HW;
        if (ltid_info->rm_more_info->flags &
                       BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
            num_ekw = ltid_info->num_ekw / 2;
        } else {
            num_ekw = ltid_info->num_ekw;
        }
        for (part = 0; part < num_ekw; part++) {
            sal_memcpy(rsp_info->rsp_ekw[part],
                       new_rsp_info->rsp_ekw[part],
                       (sizeof(uint32_t) *
                       req_info->rsp_ekw_buf_wsize));
        }
    }

exit:
    if (restore_pipe_id == TRUE) {
        pt_dyn_info->tbl_inst = current_pipe_id;
    }
    if (new_rsp_info != NULL) {
        if (new_rsp_info->rsp_ekw != NULL) {
            for (idx = 0;
                 idx < BCMPTM_RM_TCAM_MAX_WIDTH_PARTS;
                 idx++) {
                if (new_rsp_info->rsp_ekw[idx] != NULL) {
                    SHR_FREE(new_rsp_info->rsp_ekw[idx]);
                }
            }
            SHR_FREE(new_rsp_info->rsp_ekw);
        }
        SHR_FREE(new_rsp_info);
    }
    SHR_FUNC_EXIT();
}
