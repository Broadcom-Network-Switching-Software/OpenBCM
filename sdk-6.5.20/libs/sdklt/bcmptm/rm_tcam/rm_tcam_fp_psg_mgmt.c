/*! \file rm_tcam_fp_psg_mgmt.c
 *
 *  presel group management APIs.
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
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam_profile.h"
#include "rm_tcam_fp_psg.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Private Functions
 */
STATIC int
bcmptm_rm_tcam_fp_presel_group_copy(int unit,
                      bcmptm_rm_tcam_entry_iomd_t *iomd,
                      uint8_t from_slice_id,
                      uint8_t to_slice_id,
                      bcmdrd_sid_t from_profile_sid,
                      bcmdrd_sid_t to_profile_sid,
                      uint16_t hw_index)
{
    uint32_t *req_ew = NULL;
    uint32_t *rsp_ew = NULL;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t *ref_count_ptr1 = NULL;
    uint32_t *ref_count_ptr2 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pipe_id,
                                              iomd->stage_fc,
                                              from_slice_id,
                                              &ref_count_ptr1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pipe_id,
                                              iomd->stage_fc,
                                              to_slice_id,
                                              &ref_count_ptr2));
    if (ref_count_ptr1[hw_index] > 1)  {
        ref_count_ptr2[hw_index]++;
        SHR_EXIT();
    }

    SHR_ALLOC(req_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgReqEw");
    SHR_ALLOC(rsp_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgRspEw");
    pt_dyn_info.tbl_inst = iomd->pipe_id;
    pt_dyn_info.index = hw_index;
    if (ref_count_ptr1[hw_index] == 1)  {
        rsp_entry_wsize =
            bcmdrd_pt_entry_wsize(unit, from_profile_sid);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                req_flags,
                                from_profile_sid,
                                (void *)&pt_dyn_info,
                                NULL,
                                NULL, /* misc_info */
                                BCMPTM_OP_READ,
                                NULL,
                                rsp_entry_wsize,
                                iomd->ltid,
                                iomd->trans_id,
                                NULL,
                                NULL,
                                rsp_ew,
                                &rsp_ltid,
                                &rsp_flags));
        sal_memcpy(req_ew, rsp_ew, rsp_entry_wsize * 4);
        sal_memset(rsp_ew, 0, rsp_entry_wsize * 4);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                req_flags,
                                to_profile_sid,
                                (void *)&pt_dyn_info,
                                NULL,
                                NULL, /* misc_info */
                                BCMPTM_OP_WRITE,
                                req_ew,
                                rsp_entry_wsize,
                                iomd->ltid,
                                iomd->trans_id,
                                NULL,
                                NULL,
                                rsp_ew,
                                &rsp_ltid,
                                &rsp_flags));
    } else {
       SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FREE(req_ew);
    SHR_FREE(rsp_ew);
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_group_reserve_expand(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       uint8_t slice_id,
                                       uint8_t *hw_indexes)
{
    uint8_t idx = 0;
    uint8_t to_slice_id = 0;
    uint8_t from_slice_id = 0;
    uint8_t to_tile_id = 0;
    uint8_t from_tile_id = 0;
    bcmdrd_sid_t to_profile_sid;
    bcmdrd_sid_t from_profile_sid;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_row = NULL;
    bcmptm_rm_tcam_slice_t *slice_column = NULL;
    bool tile_status[BCMPTM_RM_TCAM_MAX_TILES] = { FALSE };
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    bcmptm_rm_tcam_group_mode_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hw_indexes, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    mode = iomd->entry_attrs->group_mode;

    slice_fc = iomd->slice_fc;

    rm_more_info = iomd->ltid_info->rm_more_info;
    slice_row = &(slice_fc[slice_id]) ;
    if (slice_row->next_slice_id_offset[mode]) {
        slice_row = &(slice_fc[slice_row->slice_id +
                    slice_row->next_slice_id_offset[mode]]);
    } else {
        slice_row = NULL;
    }
    tile_status[rm_more_info->slice_info[slice_id].tile_id] = TRUE;
    while (slice_row != NULL) {
        slice_column = slice_row;
        while (slice_column != NULL) {
            from_slice_id = (slice_id + idx);
            to_slice_id = slice_column->slice_id;
            from_tile_id = rm_more_info->slice_info[from_slice_id].tile_id;
            to_tile_id = rm_more_info->slice_info[to_slice_id].tile_id;
            from_profile_sid =
                rm_more_info->tile_info[from_tile_id].presel_group_sid;
            to_profile_sid =
                rm_more_info->tile_info[to_tile_id].presel_group_sid;
            if (tile_status[to_tile_id] == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_presel_group_copy(unit,
                                                    iomd,
                                                    from_slice_id,
                                                    to_slice_id,
                                                    from_profile_sid,
                                                    to_profile_sid,
                                                    hw_indexes[idx]));
                tile_status[to_tile_id] = TRUE;
            }
            if (slice_column->right_slice_id_offset[mode]) {
                slice_column = &(slice_fc[slice_column->slice_id +
                                 slice_column->right_slice_id_offset[mode]]);
            } else {
                slice_column = NULL;
            }
            idx++;
        }
        if (slice_row->next_slice_id_offset[mode]) {
            slice_row = &(slice_fc[slice_row->slice_id +
                        slice_row->next_slice_id_offset[mode]]);
        } else {
            slice_row = NULL;
        }
        idx = 0;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_group_clear(int unit,
                      bcmptm_rm_tcam_entry_iomd_t *iomd,
                      uint8_t slice_id,
                      bcmdrd_sid_t profile_sid,
                      uint16_t hw_index)
{
    uint32_t *req_ew = NULL;
    uint32_t *rsp_ew = NULL;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t *ref_count_ptr = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                              iomd->ltid,
                                              iomd->ltid_info,
                                              iomd->pipe_id,
                                              stage_fc,
                                              slice_id,
                                              &ref_count_ptr));

    if (ref_count_ptr[hw_index] > 1)  {
        ref_count_ptr[hw_index]--;
        SHR_EXIT();
    } else if (ref_count_ptr[hw_index] == 1) {
        ref_count_ptr[hw_index]--;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_ALLOC(req_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgReqEw");
    SHR_ALLOC(rsp_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgRspEw");
    pt_dyn_info.tbl_inst = iomd->pipe_id;
    pt_dyn_info.index = hw_index;
    rsp_entry_wsize =
        bcmdrd_pt_entry_wsize(unit, profile_sid);
    sal_memset(req_ew, 0, rsp_entry_wsize * 4);
    sal_memset(rsp_ew, 0, rsp_entry_wsize * 4);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                            req_flags,
                            profile_sid,
                            (void *)&pt_dyn_info,
                            NULL,
                            NULL, /* misc_info */
                            BCMPTM_OP_WRITE,
                            req_ew,
                            rsp_entry_wsize,
                            iomd->ltid,
                            iomd->trans_id,
                            NULL,
                            NULL,
                            rsp_ew,
                            &rsp_ltid,
                            &rsp_flags));
exit:
    SHR_FREE(req_ew);
    SHR_FREE(rsp_ew);
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_group_unreserve_expand(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd,
                                         uint8_t slice_id,
                                         uint8_t *hw_indexes)
{
    uint8_t idx = 0;
    uint8_t target_slice_id = 0;
    uint8_t target_tile_id = 0;
    bcmdrd_sid_t target_profile_sid;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_row = NULL;
    bcmptm_rm_tcam_slice_t *slice_column = NULL;
    bool tile_status[BCMPTM_RM_TCAM_MAX_TILES] = { FALSE };
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    bcmptm_rm_tcam_group_mode_t mode = BCMPTM_RM_TCAM_GRP_MODE_SINGLE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_indexes, SHR_E_PARAM);

    slice_fc = iomd->slice_fc;
    rm_more_info = iomd->ltid_info->rm_more_info;
    slice_row = &(slice_fc[slice_id]);
    if (slice_row->next_slice_id_offset[mode]) {
        slice_row = &(slice_fc[slice_row->slice_id +
                    slice_row->next_slice_id_offset[mode]]);
    } else {
        slice_row = NULL;
    }
    tile_status[rm_more_info->slice_info[slice_id].tile_id] = TRUE;
    while (slice_row != NULL) {
        slice_column = slice_row;
        while (slice_column != NULL) {
            target_slice_id = slice_column->slice_id;
            target_tile_id =
                rm_more_info->slice_info[target_slice_id].tile_id;
            target_profile_sid =
                rm_more_info->tile_info[target_tile_id].presel_group_sid;
            if (tile_status[target_tile_id] == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_presel_group_clear(unit,
                                                    iomd,
                                                    target_slice_id,
                                                    target_profile_sid,
                                                    hw_indexes[idx]));
                tile_status[target_tile_id] = TRUE;
            }
            if (slice_column->right_slice_id_offset[mode]) {
                slice_column = &(slice_fc[slice_column->slice_id +
                                 slice_column->right_slice_id_offset[mode]]);
            } else {
                slice_column = NULL;
            }
            idx++;
        }
        if (slice_row->next_slice_id_offset[mode]) {
            slice_row = &(slice_fc[slice_row->slice_id +
                        slice_row->next_slice_id_offset[mode]]);
        } else {
            slice_row = NULL;
        }
        idx = 0;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_groups_copy_sw(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        uint8_t new_slice_id)
{
    uint32_t group_id = 0;
    uint8_t slice_id = 0;
    uint8_t tile_id = 0;
    bcmptm_rm_tcam_fp_group_t *group_fc = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice = NULL;
    uint32_t *ref_count_ptr1 = NULL;
    uint32_t *ref_count_ptr2 = NULL;
    uint8_t presel_group_count = 0;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    group_id = iomd->entry_attrs->group_id;
    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->pipe_id,
                                          group_id,
                                          stage_fc,
                                          &group_fc));
    slice_id = group_fc->primary_slice_id;
    slice = &(slice_fc[slice_id]);
    rm_more_info = iomd->ltid_info->rm_more_info;
    while (slice != NULL) {
        tile_id = rm_more_info->slice_info[slice->slice_id].tile_id;
        presel_group_count =
            rm_more_info->tile_info[tile_id].presel_group_count;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  stage_fc,
                                                  slice_id,
                                                  &ref_count_ptr1));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  stage_fc,
                                                  new_slice_id,
                                                  &ref_count_ptr2));
        sal_memcpy(ref_count_ptr2, ref_count_ptr1,
                   presel_group_count * sizeof(uint32_t));
        if (slice->right_slice_id_offset[iomd->entry_attrs->group_mode]) {
            slice_id +=
                slice->right_slice_id_offset[iomd->entry_attrs->group_mode];
            slice =  &(slice_fc[slice_id]);
        } else {
            slice = NULL;
        }
        new_slice_id++;
        ref_count_ptr1 = NULL;
        ref_count_ptr2 = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_groups_copy_hw(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        uint8_t new_slice_id)
{
    uint8_t slice_id = 0;
    uint8_t tile_id = 0;
    uint8_t new_tile_id = 0;
    bcmptm_rm_tcam_fp_group_t *group_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice = NULL;
    uint32_t *req_ew = NULL;
    uint32_t *rsp_ew = NULL;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t profile_sid;
    uint8_t presel_group_id = 0;
    uint8_t presel_group_count = 0;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    slice_fc = iomd->slice_fc;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->entry_attrs->pipe_id,
                                          iomd->entry_attrs->group_id,
                                          iomd->stage_fc,
                                          &group_fc));

    SHR_ALLOC(req_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgReqEw");
    SHR_ALLOC(rsp_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgRspEw");
    pt_dyn_info.tbl_inst = iomd->entry_attrs->pipe_id;
    rm_more_info = iomd->ltid_info->rm_more_info;

    /* Get the primary slice index */
    slice_id = group_fc->primary_slice_id;
    slice = &(slice_fc[slice_id]);
    while (slice != NULL) {
        tile_id = rm_more_info->slice_info[slice->slice_id].tile_id;
        presel_group_count =
            rm_more_info->tile_info[tile_id].presel_group_count;
        for (presel_group_id = 0;
            presel_group_id < presel_group_count;
            presel_group_id++) {
            pt_dyn_info.index = presel_group_id;
            tile_id = rm_more_info->slice_info[slice_id].tile_id;
            profile_sid =
                rm_more_info->tile_info[tile_id].presel_group_sid;
            rsp_entry_wsize =
                bcmdrd_pt_entry_wsize(unit, profile_sid);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    iomd->ltid,
                                    iomd->trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew,
                                    &rsp_ltid,
                                    &rsp_flags));
            sal_memcpy(req_ew, rsp_ew, rsp_entry_wsize * 4);
            sal_memset(rsp_ew, 0, rsp_entry_wsize * 4);
            new_tile_id = rm_more_info->slice_info[new_slice_id].tile_id;
            profile_sid =
                rm_more_info->tile_info[new_tile_id].presel_group_sid;
            SHR_IF_ERR_VERBOSE_EXIT(
                bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    req_ew,
                                    rsp_entry_wsize,
                                    iomd->ltid,
                                    iomd->trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew,
                                    &rsp_ltid,
                                    &rsp_flags));
        }
        if (slice->right_slice_id_offset[iomd->entry_attrs->group_mode]) {
            slice_id +=
                slice->right_slice_id_offset[iomd->entry_attrs->group_mode];
            slice =  &(slice_fc[slice_id]);
        } else {
            slice = NULL;
        }
        new_slice_id++;
    }
exit:
    SHR_FREE(req_ew);
    SHR_FREE(rsp_ew);
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_groups_clear_hw(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd,
                                         uint8_t slice_id)
{
    uint8_t tile_id = 0;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice = NULL;
    uint32_t *req_ew = NULL;
    uint32_t *rsp_ew = NULL;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid;
    size_t rsp_entry_wsize = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t profile_sid;
    uint8_t presel_group_id = 0;
    uint8_t presel_group_count = 0;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    slice_fc = iomd->slice_fc;
    SHR_ALLOC(req_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgReqEw");
    sal_memset(req_ew, 0, (sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS));
    SHR_ALLOC(rsp_ew,
              sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS,
              "bcmptmRmtcamPsgRspEw");
    sal_memset(rsp_ew, 0, (sizeof(uint32_t) * BCMPTM_MAX_PT_FIELD_WORDS));
    pt_dyn_info.tbl_inst = iomd->entry_attrs->pipe_id;
    rm_more_info = iomd->ltid_info->rm_more_info;

    slice = &(slice_fc[slice_id]);
    while (slice != NULL) {
        tile_id = rm_more_info->slice_info[slice_id].tile_id;
        profile_sid =
            rm_more_info->tile_info[tile_id].presel_group_sid;
        rsp_entry_wsize =
            bcmdrd_pt_entry_wsize(unit, profile_sid);
        presel_group_count =
            rm_more_info->tile_info[tile_id].presel_group_count;
        for (presel_group_id = 0;
            presel_group_id < presel_group_count;
            presel_group_id++) {
            pt_dyn_info.index = presel_group_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    req_ew,
                                    rsp_entry_wsize,
                                    iomd->ltid,
                                    iomd->trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew,
                                    &rsp_ltid,
                                    &rsp_flags));
        }
        if (slice->right_slice_id_offset[iomd->entry_attrs->group_mode]) {
            slice_id +=
                slice->right_slice_id_offset[iomd->entry_attrs->group_mode];
            slice =  &(slice_fc[slice_id]);
        } else {
            slice = NULL;
        }
    }

exit:
    SHR_FREE(req_ew);
    SHR_FREE(rsp_ew);
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_fp_presel_groups_clear_sw(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd,
                                         uint8_t slice_id)
{
    uint8_t tile_id = 0;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice = NULL;
    uint32_t *ref_count_ptr1 = NULL;
    uint8_t presel_group_count = 0;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;

    slice = &(slice_fc[slice_id]);
    rm_more_info = iomd->ltid_info->rm_more_info;
    while (slice != NULL) {
        tile_id = rm_more_info->slice_info[slice->slice_id].tile_id;
        presel_group_count =
            rm_more_info->tile_info[tile_id].presel_group_count;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  stage_fc,
                                                  slice_id,
                                                  &ref_count_ptr1));

        sal_memset(ref_count_ptr1,
                   0,
                   presel_group_count * sizeof(uint32_t));
        if (slice->right_slice_id_offset[iomd->entry_attrs->group_mode]) {
            slice_id +=
                slice->right_slice_id_offset[iomd->entry_attrs->group_mode];
            slice =  &(slice_fc[slice_id]);
        } else {
            slice = NULL;
        }
        ref_count_ptr1 = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_presel_group_reserve(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       uint8_t slice_id,
                                       uint8_t num_slices,
                                       uint8_t *hw_indexes)
{
    int idx = 0;
    uint8_t tile_id = 0;
    uint32_t *profile_data[] = { NULL };
    uint32_t *ref_count_ptr = NULL;
    uint32_t *ref_count_ptrs[] = { NULL, NULL, NULL };
    bcmdrd_sid_t profile_sid;
    int hw_index = -1;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    shr_error_t rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hw_indexes, SHR_E_PARAM);
    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    rm_more_info = iomd->ltid_info->rm_more_info;
    for (idx = 0; idx < num_slices; idx++) {
        hw_indexes[idx] = 0;
    }
    for (idx = 0; idx < num_slices; idx++) {
        /* Get reference count for PDD profiles in a slice. */
        rv = bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                                 iomd->ltid,
                                                 iomd->ltid_info,
                                                 iomd->pipe_id,
                                                 iomd->stage_fc,
                                                 (slice_id + idx),
                                                 &ref_count_ptr);
        if (SHR_FAILURE(rv)) {
            break;
        }
        ref_count_ptrs[idx] = ref_count_ptr;
        /*
         * Add the PDD profile. If the similar profile is already
         * present then use the same index otheriwse allocate a
         * new profile index.
         */
        tile_id = rm_more_info->slice_info[slice_id + idx].tile_id;
        profile_sid =
            rm_more_info->tile_info[tile_id].presel_group_sid;
        profile_data[0] = iomd->entry_attrs->presel_group_info[idx];
        rv = bcmptm_rm_tcam_profile_add(unit,
                                        iomd->trans_id,
                                        iomd->pipe_id,
                                        iomd->ltid,
                                        1,
                                        &profile_sid,
                                        profile_data,
                                        ref_count_ptr,
                                        &hw_index);
        if (SHR_FAILURE(rv)) {
            break;
        }
        hw_indexes[idx] = hw_index;
        hw_index = 0;
        ref_count_ptr = NULL;
    }

    if (idx != num_slices) {
       idx--;
       while (idx >= 0)  {
           /*
            * Delete the successfully created profiles as
            * PDD profiles in all slices are not created.
            */
           tile_id = rm_more_info->slice_info[slice_id + idx].tile_id;
           profile_sid =
               rm_more_info->tile_info[tile_id].presel_group_sid;
           SHR_IF_ERR_VERBOSE_EXIT
             (bcmptm_rm_tcam_profile_delete(unit,
                                            iomd->trans_id,
                                            iomd->pipe_id,
                                            iomd->ltid,
                                            1,
                                            &profile_sid,
                                            ref_count_ptrs[idx],
                                            hw_indexes[idx]));
           hw_indexes[idx] = 0;
           idx--;
       }
    }
    _func_rv = rv;
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_reserve_expand(unit,
                                                           iomd,
                                                           slice_id,
                                                           hw_indexes));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_presel_group_unreserve(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    uint8_t idx = 0;
    uint8_t tile_id = 0;
    bcmptm_rm_tcam_fp_group_t *group_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice = NULL;
    uint32_t *ref_count_ptr = NULL;
    uint32_t group_id = 0;
    uint8_t slice_id = 0;
    bcmdrd_sid_t profile_sid;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    uint8_t hw_indexes[3];
    bcmptm_rm_tcam_fp_stage_t *stage_fc;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    group_id = iomd->entry_attrs->group_id;
    stage_fc = iomd->stage_fc;
    slice_fc = iomd->slice_fc;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_group_info_get(unit,
                                          iomd->ltid,
                                          iomd->ltid_info,
                                          iomd->pipe_id,
                                          group_id,
                                          stage_fc,
                                          &group_fc));

    slice_id = group_fc->primary_slice_id;
    slice = &(slice_fc[slice_id]);
    rm_more_info = iomd->ltid_info->rm_more_info;
    while (slice != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_ref_count_get(unit,
                                                  iomd->ltid,
                                                  iomd->ltid_info,
                                                  iomd->pipe_id,
                                                  stage_fc,
                                                  slice_id,
                                                  &ref_count_ptr));

        tile_id = rm_more_info->slice_info[slice->slice_id].tile_id;
        profile_sid = rm_more_info->tile_info[tile_id].presel_group_sid;

        hw_indexes[idx] = iomd->entry_attrs->presel_group_indexes[idx];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_clear(unit,
                                                  iomd,
                                                  slice->slice_id,
                                                  profile_sid,
                                                  hw_indexes[idx]));
        ref_count_ptr = NULL;
        idx++;

        if (slice->right_slice_id_offset[iomd->entry_attrs->group_mode]) {
            slice_id +=
                slice->right_slice_id_offset[iomd->entry_attrs->group_mode];
            slice =  &(slice_fc[slice_id]);
        } else {
            slice = NULL;
        }
    }
    slice_id = group_fc->primary_slice_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_group_unreserve_expand(unit,
                                                         iomd,
                                                         slice_id,
                                                         hw_indexes));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_presel_groups_copy(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t new_slice_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_groups_copy_hw(unit, iomd, new_slice_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_groups_copy_sw(unit, iomd, new_slice_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_presel_groups_clear(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd,
                                      uint8_t slice_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_groups_clear_hw(unit, iomd, slice_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_presel_groups_clear_sw(unit, iomd, slice_id));
exit:
    SHR_FUNC_EXIT();
}
