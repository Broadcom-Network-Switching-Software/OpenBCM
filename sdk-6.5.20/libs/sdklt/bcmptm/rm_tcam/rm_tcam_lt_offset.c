/*! \file rm_tcam_lt_offset.c
 *
 * Miscellaneous.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_um_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */

/*******************************************************************************
 * Public Functions
 */

static int
bcmptm_rm_tcam_fp_lt_hw_info_update(int unit, lt_mreq_info_t *lrd_info)
{
    int rv = 0;
    uint32_t idx = 0, alloc_sz = 0;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    const bcmdrd_sid_t **sid_arr = NULL;
    const bcmdrd_sid_t **sid_data_arr = NULL;
    const uint32_t **entry_count_arr = NULL;
    uint32_t *entry_cnt = NULL;
    uint32_t entry_count_array[BCMPTM_RM_TCAM_PIPE_COUNT_MAX]
                              [BCMPTM_RM_TCAM_MAX_SLICES];
    uint8_t *slice_cnt_arr = NULL;
    bcmptm_rm_tcam_slice_info_t *slice_info = NULL;
    bcmptm_rm_tcam_tile_info_t *tile_info = NULL;
    bcmdrd_sid_t *sid = NULL;
    uint8_t num_key_rows = 0;
    uint8_t new_depth_inst = 0;
    uint8_t num_data_rows = 0;
    bcmdrd_sid_t tcam_sid[BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES]
                         [BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES];
    bcmdrd_sid_t data_sid[BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES]
                         [BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES];
    uint32_t num_slices = 0;
    uint8_t num_sids_per_slice = 0;
    uint8_t count_num_sids_per_slice = 0;
    uint8_t slice_idx = 0;
    uint8_t tile_idx = 0;
    uint8_t num_rows = 0;
    uint32_t next_start_idx = 0;
    uint32_t num_entries = 0;
    uint8_t iter = 0;
    uint8_t pipe = 0;
    bool sid_valid = FALSE;
    bcmdrd_sid_t *main_lt_arr = NULL;
    bcmdrd_sid_t *shared_lt_arr = NULL;


    SHR_FUNC_ENTER(unit);

    more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;
    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                            lrd_info->hw_entry_info;
    SHR_NULL_CHECK(hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(more_info, SHR_E_FAIL);

    /* update RM more info */
    rv = bcmdrd_dev_param_get(unit, "ifp_slices", &num_slices);

    if (rv == -1) {
        num_slices = more_info->slice_count[0];
    }

    alloc_sz = (more_info->pipe_count * sizeof(uint8_t));
    SHR_ALLOC(slice_cnt_arr, alloc_sz, "bcmptmRmSliceCountArr");
    sal_memset(slice_cnt_arr, 0, alloc_sz);

    alloc_sz = (more_info->pipe_count * sizeof(uint32_t *));
    SHR_ALLOC(entry_count_arr, alloc_sz, "bcmptmRmEntryCountArr");
    sal_memset(entry_count_arr, 0, alloc_sz);

    sal_memset(entry_count_array, 0,
               more_info->pipe_count * num_slices * sizeof(uint32_t));
    alloc_sz = (num_slices * sizeof(bcmptm_rm_tcam_slice_info_t));
    SHR_ALLOC(slice_info, alloc_sz, "bcmptmRmSliceInfoArr");
    sal_memset(slice_info, 0, alloc_sz);
    sal_memcpy(slice_info, more_info->slice_info, alloc_sz);

    alloc_sz = ((num_slices/4) * sizeof(bcmptm_rm_tcam_tile_info_t));
    SHR_ALLOC(tile_info, alloc_sz, "bcmptmRmTileInfoArr");
    sal_memset(tile_info, 0, alloc_sz);
    sal_memcpy(tile_info, more_info->tile_info, alloc_sz);

    if (more_info->main_lt_info != NULL) {
        alloc_sz = (1 * sizeof(bcmdrd_sid_t));
        SHR_ALLOC(main_lt_arr, alloc_sz, "bcmptmPreselMainLtArr");
        sal_memset(main_lt_arr, 0, alloc_sz);
        sal_memcpy(main_lt_arr, more_info->main_lt_info, alloc_sz);
        more_info->main_lt_info = main_lt_arr;
    }

    if (more_info->shared_lt_count) {
        alloc_sz = (more_info->shared_lt_count * sizeof(bcmdrd_sid_t));
        SHR_ALLOC(shared_lt_arr, alloc_sz, "bcmptmSharedLtArr");
        sal_memset(shared_lt_arr, 0, alloc_sz);
        sal_memcpy(shared_lt_arr, more_info->shared_lt_info, alloc_sz);
        more_info->shared_lt_info = shared_lt_arr;
    }

    num_sids_per_slice = more_info->tile_info[0].num_tcam_sids_per_slice;
    num_rows = hw_entry_info[0].num_depth_inst;
    count_num_sids_per_slice = 0;
    slice_idx = 0;
    tile_idx = 0;
    next_start_idx = 0;
    num_entries = 0;
    for (iter = 0; iter <= num_rows; iter++) {
        if (iter != 0 && iter % num_sids_per_slice == 0) {
            slice_info[slice_idx].slice_id = slice_idx;
            slice_info[slice_idx].start_index = next_start_idx;
            if (num_entries > 0) {
                slice_info[slice_idx].end_index =
                    slice_info[slice_idx].start_index + num_entries - 1;
            } else {
                slice_info[slice_idx].end_index = 0;
            }
            for (pipe = 0; pipe < more_info->pipe_count; pipe++) {
                entry_count_array[pipe][slice_idx] = num_entries;
            }
            if (slice_idx % 4 == 0) {
                tile_info[tile_idx].num_tcam_sids_per_slice =
                    count_num_sids_per_slice;
                    tile_idx++;
            }
            next_start_idx = slice_info[slice_idx].start_index + num_entries;
            count_num_sids_per_slice = 0;
            slice_idx++;
            num_entries = 0;
        }

        if (iter != num_rows && hw_entry_info[0].sid[iter] != NULL) {
            sid_valid = bcmdrd_dev_sym_valid(unit,
                         *(hw_entry_info[0].sid[iter]));
            if (sid_valid) {
                count_num_sids_per_slice++;
                num_entries += bcmdrd_pt_index_max(unit,
                                            *(hw_entry_info[0].sid[iter])) + 1;
            }

        }
    }
    for (pipe = 0; pipe < more_info->pipe_count; pipe++) {
        entry_cnt = NULL;
        alloc_sz = (num_slices * sizeof(uint32_t));
        SHR_ALLOC(entry_cnt, alloc_sz, "bcmptmRmEntryCount");
        sal_memcpy(entry_cnt, entry_count_array[pipe], alloc_sz);
        entry_count_arr[pipe] = entry_cnt;
        slice_cnt_arr[pipe] = num_slices;
    }
    more_info->entry_count = entry_count_arr;
    more_info->slice_count = slice_cnt_arr;
    more_info->slice_info = slice_info;
    more_info->tile_count =  (num_slices/4);
    more_info->tile_info = tile_info;

    /* Update Hw entry info */
    for (idx = 0; idx < lrd_info->hw_entry_info_count; idx++) {
        sal_memset(tcam_sid, 0,
                   (BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES *
                    BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES *
                    sizeof(bcmdrd_sid_t)));
        sal_memset(data_sid, 0,
                   (BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES *
                    BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES *
                    sizeof(bcmdrd_sid_t)));
        num_rows = hw_entry_info[idx].num_depth_inst;
        new_depth_inst = 0;

        num_key_rows = hw_entry_info[idx].num_key_rows;
        num_data_rows =  hw_entry_info[idx].num_data_rows;

        for (iter = 0; iter < num_rows; iter++) {
            if (hw_entry_info[idx].sid[iter] != NULL) {
                sid_valid = bcmdrd_dev_sym_valid(unit,
                             *(hw_entry_info[idx].sid[iter]));
                if (sid_valid) {
                    sal_memcpy(&(tcam_sid[new_depth_inst]),
                               hw_entry_info[idx].sid[iter],
                               num_key_rows * sizeof(bcmdrd_sid_t));
                }
            }
            if (hw_entry_info[idx].sid_data_only[iter] != NULL) {
                sid_valid = bcmdrd_dev_sym_valid(unit,
                             *(hw_entry_info[idx].sid_data_only[iter]));
                if (sid_valid) {
                    sal_memcpy(&(data_sid[new_depth_inst++]),
                               hw_entry_info[idx].sid_data_only[iter],
                               num_data_rows * sizeof(bcmdrd_sid_t));
                }
            }
        }

        if (new_depth_inst == 0) {
            continue;
        }
        sid_arr = NULL;
        alloc_sz = (new_depth_inst * sizeof(bcmdrd_sid_t *));
        SHR_ALLOC(sid_arr, alloc_sz, "bcmptmRmTcamSidArr");
        SHR_NULL_CHECK(sid_arr, SHR_E_MEMORY);
        sal_memset(sid_arr, 0, alloc_sz);

        sid_data_arr = NULL;
        alloc_sz = (new_depth_inst * sizeof(bcmdrd_sid_t *));
        SHR_ALLOC(sid_data_arr, alloc_sz, "bcmptmRmTcamSidArr");
        SHR_NULL_CHECK(sid_data_arr, SHR_E_MEMORY);
        sal_memset(sid_data_arr, 0, alloc_sz);

        for (iter = 0; iter < new_depth_inst; iter++) {
            sid = NULL;
            alloc_sz = (num_key_rows * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamSid");
            SHR_NULL_CHECK(sid, SHR_E_MEMORY);
            sal_memset(sid, 0, alloc_sz);
            sal_memcpy(sid, tcam_sid[iter], alloc_sz);
            sid_arr[iter] = sid;

            sid = NULL;
            alloc_sz = (num_data_rows * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmDataSid");
            SHR_NULL_CHECK(sid, SHR_E_MEMORY);
            sal_memset(sid, 0, alloc_sz);
            sal_memcpy(sid, data_sid[iter], alloc_sz);
            sid_data_arr[iter] = sid;
        }
        hw_entry_info[idx].sid = sid_arr;
        hw_entry_info[idx].sid_data_only = sid_data_arr;
        hw_entry_info[idx].num_depth_inst = new_depth_inst;
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (iter = 0; iter < lrd_info->hw_entry_info_count; iter++) {
            for (num_rows = 0;
                 num_rows < hw_entry_info[iter].num_depth_inst;
                 num_rows++) {
                SHR_FREE(hw_entry_info[iter].sid[num_rows]);
                SHR_FREE(hw_entry_info[iter].sid_data_only[num_rows]);
            }
        }
        for (idx = 0; idx < more_info->pipe_count; idx++) {
            if (entry_count_arr != NULL) {
                SHR_FREE(entry_count_arr[idx]);
            }
        }
        SHR_FREE(sid_arr);
        SHR_FREE(sid_data_arr);
        SHR_FREE(entry_count_arr);
        SHR_FREE(slice_cnt_arr);
        SHR_FREE(slice_info);
        SHR_FREE(tile_info);
    }
    /*
     * Memory allocated by this function is freed when banks are deallocated.
     * Freeing is done by bcmptm_rm_tcam_dyn_lt_hw_info_cleanup().
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_validate_and_update_ltid_info(int unit, bcmltd_sid_t ltid)
{
    uint32_t alloc_sz = 0, idx = 0;
    const lt_mreq_info_t *orig_lrd_info = NULL;
    lt_mreq_info_t *new_lrd_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *orig_hw_info = NULL;
    uint8_t num_rows = 0;
    bool update_ltid_info = FALSE;
    bool sid_valid = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_lt_mreq_info_get(unit,
                                 ltid,
                                 &orig_lrd_info));
    SHR_NULL_CHECK(orig_lrd_info, SHR_E_FAIL);

    orig_hw_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                   (orig_lrd_info->hw_entry_info);

    /* Verify if all the sid in sid info array are valid. */
    if (orig_hw_info == NULL) {
        SHR_EXIT();
    }

    num_rows = orig_hw_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].num_depth_inst;
    for (idx = 0; idx < num_rows; idx++) {
        if (orig_hw_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].sid[idx] != NULL) {
            sid_valid = bcmdrd_dev_sym_valid(unit,
                         *(orig_hw_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].sid[idx]));
            if (!sid_valid) {
                update_ltid_info = TRUE;
                break;
            }
        }
    }
    if (update_ltid_info == FALSE) {
        SHR_EXIT();
    }

    if (orig_lrd_info->pt_type == LT_PT_TYPE_FP) {

        alloc_sz = sizeof(lt_mreq_info_t);
        SHR_ALLOC(new_lrd_info, alloc_sz, "bcmptmLtMreqInfo");
        sal_memset(new_lrd_info, 0, alloc_sz);
        sal_memcpy(new_lrd_info, orig_lrd_info, alloc_sz);

        alloc_sz = (orig_lrd_info->hw_entry_info_count *
                    sizeof(bcmptm_rm_tcam_hw_entry_info_t));
        SHR_ALLOC(hw_entry_info, alloc_sz, "bcmptmLtHwEntryInfo");
        sal_memset(hw_entry_info, 0, alloc_sz);
        sal_memcpy(hw_entry_info, orig_hw_info, alloc_sz);

        alloc_sz = (orig_lrd_info->rm_more_info_count * sizeof(bcmptm_rm_tcam_more_info_t));
        SHR_ALLOC(more_info, alloc_sz, "bcmptmLtMoreInfo");
        sal_memset(more_info, 0, alloc_sz);
        sal_memcpy(more_info, orig_lrd_info->rm_more_info, alloc_sz);

        new_lrd_info->hw_entry_info = hw_entry_info;
        new_lrd_info->rm_more_info = more_info;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_lt_hw_info_update(unit,
                                                     new_lrd_info));

        /* Update the LT handler in dynamic PTRM table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_dyn_ptrm_tbl_update(unit,
                                                ltid,
                                                new_lrd_info));
    }

exit:
    if (SHR_FUNC_ERR()) {
            SHR_FREE(hw_entry_info);
            SHR_FREE(more_info);
            SHR_FREE(new_lrd_info);
    }
    /*
     * Memory allocated by this function is freed during shutdown
     * by bcmptm_rm_tcam_dyn_lt_mreq_info_cleanup()
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_ltid_identify(int unit, bool warm)
{
    bcmltd_sid_t ltid;
    const lt_mreq_info_t *lrd_info = NULL;
    lt_mreq_info_t *dyn_lrd_info = NULL;
    const bcmptm_rm_tcam_more_info_t *more_info = NULL;
    uint32_t lt_state_offset;
    int tmp_rv;
    bcmptm_sub_phase_t phase;
    SHR_FUNC_ENTER(unit);

    /* Phase 1
     * Identify LTIDs that are managed by rm_tcam
     */
    phase = BCMPTM_SUB_PHASE_1;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_um_comp_config(unit, warm, phase));


    /* There may be 100 tables with LTIDs 0-99 in multi-unit system.
     * Unit may support only 20 tables but with LTIDs ranging from 80 - 99
     * BCMLTD_TABLE_COUNT will be 100 in this case and bcmlrd_table_count_get()
     * will be 20 for this unit. */

    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            switch (lrd_info->pt_type) {
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
                    if (lrd_info->dynamic_banks == TRUE) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmptm_rm_tcam_dyn_lt_mreq_info_init(unit,
                                                                  BCMPTM_DIRECT_TRANS_ID,
                                                                  ltid,
                                                                  warm));
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_lt_reg(unit,
                                               ltid,
                                               warm,
                                               phase,
                                               &lt_state_offset));
                    break;
                default:
                    if (lrd_info->pt_type == LT_PT_TYPE_FP) {
                       /*
                        * Check validity of the SID's in ltid_info. If any of the SID's
                        * are not valid construct ltid_info with only valid SID's and
                        * consider it a dynamic lt.
                        */
                        if (lrd_info->dynamic_banks == FALSE) {
                            SHR_IF_ERR_VERBOSE_EXIT
                               (bcmptm_rm_tcam_validate_and_update_ltid_info(unit,
                                                                             ltid));
                        }

                        more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;
                        /* EM FP and EM_FT are UFT hash banks LT's */
                        if (more_info != NULL &&
                            ((more_info->flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS) ||
                            (lrd_info->dynamic_banks == TRUE &&
                            more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS))) {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmptm_rm_tcam_dyn_lt_mreq_info_init(unit,
                                                                      BCMPTM_DIRECT_TRANS_ID,
                                                                      ltid,
                                                                      warm));
                            tmp_rv = bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit, ltid, &dyn_lrd_info);
                            if (SHR_SUCCESS(tmp_rv)) {
                                more_info = (bcmptm_rm_tcam_more_info_t *)dyn_lrd_info->rm_more_info;
                            }
                        }
                        if (more_info != NULL && more_info->entry_count == NULL) {
                            continue;
                        }
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmptm_rm_tcam_um_lt_reg(unit,
                                                      ltid,
                                                      warm,
                                                      phase,
                                                      &lt_state_offset));
                    }
                    break;
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            /* Error case */
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which means there is no rsrc_mgr_data_struct
             => it is not managed by PTRM
        } */
    } /* foreach ltid */

    /* Phase 2
     * Can store (ltid, corresponding lt_state_offset)
     */
    phase = BCMPTM_SUB_PHASE_2;
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_um_comp_config(unit, warm, phase));
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            switch (lrd_info->pt_type) {
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
                    more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_tcam_lt_reg(unit, ltid, warm, phase,
                                              &lt_state_offset));
                    break;
                default:
                    if (lrd_info->pt_type == LT_PT_TYPE_FP) {
                        more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;
                        if (more_info != NULL &&
                            ((more_info->flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS) ||
                            (lrd_info->dynamic_banks == TRUE &&
                            more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS))) {
                            tmp_rv = bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit, ltid, &dyn_lrd_info);
                            if (SHR_SUCCESS(tmp_rv)) {
                                more_info = (bcmptm_rm_tcam_more_info_t *)dyn_lrd_info->rm_more_info;
                            }
                        }
                        if (more_info != NULL && more_info->entry_count == NULL) {
                            continue;
                        }
                        SHR_IF_ERR_EXIT(
                            bcmptm_rm_tcam_um_lt_reg(unit, ltid, warm, phase,
                                                     &lt_state_offset));
                    }
                    break;
            }
        } else if (tmp_rv != SHR_E_UNAVAIL) {
            /* Error case */
            SHR_IF_ERR_EXIT(tmp_rv);
        } /* else {
             tmp_rv = SHR_E_UNAVAIL which is ok
             Not having rsrc_mgr_data_struct => it is not managed by PTRM
        } */
    } /* foreach ltid */

    /* Phase 3
     * rsrc_mgr can do any remainder tasks
     */
    phase = BCMPTM_SUB_PHASE_3;
    /*
     * Call resource manager phase -3 only for priority TCAM
     * Used for only updating TCAM info meta data for shared
     * TCAM.
     */
    for (ltid = 0; ltid < BCMLTD_TABLE_COUNT; ltid++) {
        lrd_info = NULL;
        tmp_rv = SHR_E_NONE;
        tmp_rv = bcmlrd_lt_mreq_info_get(unit, ltid, &lrd_info);
        if (tmp_rv == SHR_E_NONE) {
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);
            switch (lrd_info->pt_type) {
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                    SHR_IF_ERR_EXIT(
                        bcmptm_rm_tcam_lt_reg(unit, ltid, warm, phase,
                                              &lt_state_offset));
                    break;
                default:
                    break;
            }
        }
    }
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_comp_config(unit, warm, phase));
    SHR_IF_ERR_EXIT(
        bcmptm_rm_tcam_um_comp_config(unit, warm, phase));

exit:
    SHR_FUNC_EXIT();
}
