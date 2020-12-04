/*! \file rm_tcam_ser_correction_info.c
 *
 * SER correction functions for TCAM resource manager.
 * This file contains ser call back fucntion and
 * ser correction info related functions.
 * TCAMs software structure.
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
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmcfg/bcmcfg_lt.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

int bcmptm_rm_tcam_create_ser_correction_info(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_rm_tcam_lt_info_t *ltid_info)
{
    bcmptm_rm_tcam_lt_info_t shr_ltid_info;
    bcmltd_sid_t shr_ltid = BCMLTD_SID_INVALID;
    uint8_t idx = 0;
    uint8_t i = 0;
    bcmptm_rm_tcam_ser_correction_info_t *ser_info = NULL;
    bcmptm_rm_tcam_ser_correction_info_t *ser_ptr = NULL;
    bool found = false;
    int min_idx = 0;
    int max_idx = 0;
    bcmdrd_sid_t tcam_only_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES];
    bcmdrd_sid_t data_only_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES];
    bcmdrd_sid_t tcam_data_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES];
    bcmdrd_sid_t paired_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES];
    bcmdrd_sid_t *sid = NULL;
    uint32_t alloc_sz = 0;
    uint8_t sid_count = 0;
    bool urpf_enabled_lt = false;
    uint8_t depth = 0;
    uint8_t width = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Check ser info whether the data is already present */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_ser_correction_info_global_get(unit,
                                                      &ser_info));
    ser_ptr = ser_info;

    while (ser_ptr != NULL && ser_ptr->ltid != ltid) {
        ser_ptr = ser_ptr->next;
    }
    if (ser_ptr != NULL && ser_ptr->ltid == ltid) {
        found = true;
    }

    /*
     * For all non-dynamic LT's, only during coldboot/warmboot init,
     * this function is called. ser_info is computed only once.
     */
    if (found == true) {
        /*
         * For Dynamic LT's, This function is called multiple times
         * whenever banks are added/deleted from this LT.
         * Free previous allocated ser info.
         * Compute ser info again for dynamic LT's
         */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_ser_corr_info_global_remove(unit, ser_ptr));
    }

    if (ltid_info->hw_entry_info[0].num_depth_inst == 0) {
        /* Do not add ser info for this LT */
        SHR_EXIT();
    }

    ser_ptr = NULL;
    SHR_ALLOC(ser_ptr, sizeof(bcmptm_rm_tcam_ser_correction_info_t),
                "bcmptmRmTcamSerCorrectionInfoPtr");
    sal_memset(ser_ptr, 0, sizeof(bcmptm_rm_tcam_ser_correction_info_t));

    ser_ptr->ltid = ltid;

    if (ltid_info->rm_more_info[0].flags & BCMPTM_RM_TCAM_URPF_ENABLED_LT) {
        ser_ptr->num_depth_inst =
            ltid_info->hw_entry_info[0].num_depth_inst * 2;
            urpf_enabled_lt = true;
    } else {
        ser_ptr->num_depth_inst =
            ltid_info->hw_entry_info[0].num_depth_inst;
    }

    alloc_sz = (ser_ptr->num_depth_inst * sizeof(bcmdrd_sid_t *));
    SHR_ALLOC(ser_ptr->hw_sid_tcam_only_arr, alloc_sz,
                "bcmptmRmtcamSerTcamOnlyarrPtr");
    SHR_ALLOC(ser_ptr->hw_sid_data_only_arr, alloc_sz,
                "bcmptmRmtcamSerDataOnlyarrPtr");
    SHR_ALLOC(ser_ptr->hw_sid_tcam_data_arr, alloc_sz,
                "bcmptmRmtcamSerTcamDataarrPtr");
    SHR_ALLOC(ser_ptr->hw_sid_paired_arr, alloc_sz,
                "bcmptmRmtcamSerTcamDataarrPtr");

    sal_memset(ser_ptr->hw_sid_tcam_only_arr, 0, alloc_sz);
    sal_memset(ser_ptr->hw_sid_data_only_arr, 0, alloc_sz);
    sal_memset(ser_ptr->hw_sid_tcam_data_arr, 0, alloc_sz);
    sal_memset(ser_ptr->hw_sid_paired_arr, 0, alloc_sz);

    for (i = 0; i <  ser_ptr->num_depth_inst ; i++) {

        sal_memset(tcam_only_sid, 0,
             BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES * sizeof(bcmdrd_sid_t));
        sal_memset(data_only_sid, 0,
             BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES * sizeof(bcmdrd_sid_t));
        sal_memset(tcam_data_sid, 0,
             BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES * sizeof(bcmdrd_sid_t));
        sal_memset(paired_sid, 0,
             BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES * sizeof(bcmdrd_sid_t));
        depth = i;
        width = 0;
        if (urpf_enabled_lt &&
            (i >= ltid_info->hw_entry_info[0].num_depth_inst)) {
            depth = i - ltid_info->hw_entry_info[0].num_depth_inst;
            width = 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_ser_corr_sid_info_get(unit,
                            ltid_info->hw_entry_info[0].sid[depth][width],
                            tcam_only_sid,
                            data_only_sid,
                            tcam_data_sid,
                            paired_sid,
                            &sid_count));

        ser_ptr->num_width_inst = sid_count;

        alloc_sz = (sid_count * sizeof(bcmdrd_sid_t));
        sid = NULL;
        SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamOnlySid");
        sal_memcpy(sid, tcam_only_sid, alloc_sz);
        ser_ptr->hw_sid_tcam_only_arr[i] = sid;

        sid = NULL;
        SHR_ALLOC(sid, alloc_sz, "bcmptmRmDataOnlySid");
        sal_memcpy(sid, data_only_sid, alloc_sz);
        ser_ptr->hw_sid_data_only_arr[i] = sid;

        sid = NULL;
        SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamDataSid");
        sal_memcpy(sid, tcam_data_sid, alloc_sz);
        ser_ptr->hw_sid_tcam_data_arr[i] = sid;

        sid = NULL;
        SHR_ALLOC(sid, sizeof(bcmdrd_sid_t), "bcmptmRmPairedSid");
        sal_memcpy(sid, paired_sid, sizeof(bcmdrd_sid_t));
        ser_ptr->hw_sid_paired_arr[i] = sid;

        if (i == 0) {
            min_idx = bcmptm_pt_index_min(unit, tcam_only_sid[0]);
            max_idx = bcmptm_pt_index_max(unit, tcam_only_sid[0]);
            ser_ptr->hw_sid_size = (max_idx - min_idx + 1);

            min_idx = bcmptm_pt_index_min(unit, paired_sid[0]);
            max_idx = bcmptm_pt_index_max(unit, paired_sid[0]);
            ser_ptr->paired_sid_size = (max_idx - min_idx + 1);

            min_idx = bcmptm_pt_index_min(unit, data_only_sid[0]);
            max_idx = bcmptm_pt_index_max(unit, data_only_sid[0]);
            ser_ptr->data_sid_size = (max_idx - min_idx + 1);
        }
    }

    ser_ptr->shared_lt_count = ltid_info->rm_more_info->shared_lt_count;

    SHR_ALLOC(ser_ptr->entry_type_arr,
                (ser_ptr->shared_lt_count * sizeof(uint8_t)),
                "bcmptmRmtcamSerEntryTypeArr");
    SHR_ALLOC(ser_ptr->shr_ltid_arr,
                (ser_ptr->shared_lt_count * sizeof(bcmltd_sid_t)),
                "bcmptmRmtcamSerShrLtidArr");

    for (idx = 0; idx < ser_ptr->shared_lt_count; idx++) {
        shr_ltid = ltid_info->rm_more_info->shared_lt_info[idx];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      shr_ltid,
                                      0,
                                      0,
                                      ((void *)&shr_ltid_info),
                                      NULL,
                                      NULL));
        ser_ptr->entry_type_arr[idx] =
            shr_ltid_info.hw_entry_info->key_type_val[0][0];

        ser_ptr->shr_ltid_arr[idx] = shr_ltid;
    }
    ser_ptr->next = NULL;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_ser_corr_info_global_update(unit, &ser_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_ser_sid_index_map_cb(int unit, bcmdrd_sid_t hw_sid,
                          bcmbd_pt_dyn_info_t hw_dyn_info,
                          bcmdrd_sid_t *out_sid,
                          bcmbd_pt_dyn_info_t *out_dyn_info,
                          int *out_count)
{
    bcmptm_rm_tcam_lt_info_t shr_ltid_info;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmltd_sid_t shr_ltid = BCMLTD_SID_INVALID;
    uint8_t i = 0, j = 0;
    bcmptm_rm_tcam_ser_correction_info_t *ser_info = NULL;
    bcmptm_rm_tcam_ser_correction_info_t *ser_ptr = NULL;
    bool found = false;
    bcmltd_sid_t ltid = BCMLTD_SID_INVALID;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint32_t sw_idx = 0;
    uint32_t inp_index = 0;
    int found_depth_idx = 0;
    int pipe = -1;
    uint8_t num_pipes = 0;
    bcmdrd_sid_t out_hw_sid_local = BCMDRD_INVALID_ID;
    bcmdrd_sid_t out_hw_sid_local2 = BCMDRD_INVALID_ID;
    bcmdrd_sid_t out_paired_sid_local = BCMDRD_INVALID_ID;
    bool inp_paired_view = false;
    int sw_entry_type = BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID;
    bool is_reserve_index = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(out_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(out_count, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "\nCb input:\n hw_sid:%d, hw_dyn_info.index:%d"
                "\nhw_dyn_info.tbl_inst:%d.\n"),
                hw_sid, hw_dyn_info.index, hw_dyn_info.tbl_inst));

    /* Get global PTRM-TCAM SER correction info */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_ser_correction_info_global_get(unit,
                                                      &ser_info));
    ser_ptr = ser_info;

    while (ser_ptr != NULL) {
        for (i = 0; i < ser_ptr->num_depth_inst; i++) {
            for (j = 0; j < ser_ptr->num_width_inst; j++) {
                if (hw_sid == ser_ptr->hw_sid_tcam_only_arr[i][j] ||
                    hw_sid == ser_ptr->hw_sid_data_only_arr[i][j] ||
                    hw_sid == ser_ptr->hw_sid_tcam_data_arr[i][j] ||
                    hw_sid == ser_ptr->hw_sid_paired_arr[i][0]) {
                    /* Received hw_sid matches with ser_info hw_sid */
                    found_depth_idx = i;
                    out_paired_sid_local = ser_ptr->hw_sid_paired_arr[i][0];
                    out_hw_sid_local = ser_ptr->hw_sid_tcam_data_arr[i][j];
                    if (hw_sid == ser_ptr->hw_sid_paired_arr[i][0]) {
                        inp_paired_view = true;
                        if (ser_ptr->num_width_inst > 1) {
                            out_hw_sid_local2 =
                                ser_ptr->hw_sid_tcam_data_arr[i][1];
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (found == true) {
                break;
            }
        }
        if (found == true) {
            /* Ser correction info is available for this LT */
            ltid = ser_ptr->ltid;
            break;
        }
        ser_ptr = ser_ptr->next;
    }

    if (found == false) {
        /* Ser correction info is not available for received hw_sid */
        *out_sid = hw_sid;
        out_dyn_info[0] = hw_dyn_info;
        *out_count = 0;
        SHR_EXIT();

    }

    /* Get Ltid info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      ltid,
                                      0,
                                      0,
                                      ((void *)&ltid_info),
                                      NULL,
                                      NULL));

    if (hw_sid == ser_ptr->hw_sid_data_only_arr[i][j] &&
        !(ltid_info.hw_entry_info->view_type)) {
        if (hw_dyn_info.index % 2 == 1) {
            /* If it is an odd index and it is non view_type LT,
             * return the corresponding single wide
             * combined view sid and hw_dyn_info/2 index.
             * Odd index is used only for half mode LT's. So,
             * return hw_sid is always single wide
             * combined view sid.
             */
            out_sid[0] = out_hw_sid_local;
            out_dyn_info[0].index = (hw_dyn_info.index / 2);
            out_dyn_info[0].tbl_inst = hw_dyn_info.tbl_inst;
            *out_count = 1;
            SHR_EXIT();
        }
    }

    /* Set the default values in case entry is unused */
    *out_count = 1;
    out_dyn_info[0] = hw_dyn_info;
    if (hw_sid == out_paired_sid_local) {
        out_sid[0] = out_paired_sid_local;
    } else {
        out_sid[0] = out_hw_sid_local;
    }

    /* Compute sw index */
    sw_idx = found_depth_idx * ser_ptr->hw_sid_size;

    if (hw_sid == ser_ptr->hw_sid_data_only_arr[i][j] &&
        !(ltid_info.hw_entry_info->view_type)) {
        /*
         * For TD4, data index is twice the tcam index.
         * Sw entry info is as per tcam index.
         */
        inp_index = hw_dyn_info.index/2;
    } else {
        inp_index = hw_dyn_info.index;
    }

    out_dyn_info[0].index = inp_index;

    /* Half mode Lt's will have twice the size of Hw depth. */
    if (ltid_info.rm_more_info->flags &
             BCMPTM_RM_TCAM_HALF_MODE) {
        sw_idx += 2 * inp_index;
    } else {
        sw_idx += inp_index;
    }

    /* Get whether entry_info is written per pipe or global mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_pipe_config_get(unit,
                                        ltid,
                                        &ltid_info,
                                        ltid_info.rm_more_info->pipe_count,
                                        &num_pipes));
    /*
     * If num_pipes is 1, means entry_info is written in global mode.
     * Use pipe_id -1 for all hw_dyn_info.tbl_inst which are greater than
     * num_pipes. Ser error may occur in any pipe, in global mode, we
     * wirte the same entry to all pipes in HW. So we can use pipe_id -1
     * in such cases.
     */
    if (hw_dyn_info.tbl_inst < num_pipes) {
        pipe = hw_dyn_info.tbl_inst;
    } else {
        pipe = -1;
    }

    /* Get entry info for this LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_entry_info_get(unit,
                                                 ltid,
                                                 &ltid_info,
                                                 pipe,
                                                 &entry_info));

    /*
     * Before extracting entry type from sw index, check whether
     * it is a reserve entry. Reserve entry is only a sw reservation
     * and should not be considered as used entry in HW.
     * reserve entry is identified by entry_prio invalid and valid entry_type.
     */
    if (entry_info[sw_idx].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID) {
        sw_entry_type = entry_info[sw_idx].entry_type;
    }

    if (sw_entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID
        && ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
        /*
         * LT is half mode LT. for a given hw_idx, 2 half mode
         * entries can be written to hw. we should verify
         * entry_info[sw_idx+1] for a given
         * hw_idx also before marking it as unused entry.
         */
        if (entry_info[sw_idx+1].entry_pri != BCMPTM_RM_TCAM_PRIO_INVALID) {
            sw_entry_type =  entry_info[sw_idx+1].entry_type;
            sw_idx  = sw_idx + 1;
        }
    }

    if (sw_entry_type == BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID) {
        /* Entry is not yet written.
         * In case input hw_sid is paired view sid, Return same paired_sid
         * In case input hw_sid is data_onlym or tcam_onlym or combined view,
         * return combined view hw_sid.
         */
        if (hw_sid == out_paired_sid_local) {
            out_sid[0] = out_paired_sid_local;
        } else {
            out_sid[0] = out_hw_sid_local;
        }
    } else {
        /*
         * hw entry is one of the shared LT's entry.
         * Verify entry_type of computed sw_idx for given hw_idx.
         */
        for (i = 0; i < ser_ptr->shared_lt_count; i++) {
            if (sw_entry_type == ser_ptr->entry_type_arr[i]) {
                /*
                 * If the entry_type matches with shared LT,
                 * return the mapped sid and depending on view_type
                 * compute new hw_dyn_info.index.
                 */
                shr_ltid = ser_ptr->shr_ltid_arr[i];
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ptrm_info_get(unit,
                                                  shr_ltid,
                                                  0,
                                                  0,
                                                  ((void *)&shr_ltid_info),
                                                  NULL,
                                                  NULL));
                /* Check if it is a sw reserve entry */
                if (entry_info[sw_idx].entry_pri == 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_check_reserve_entry(unit,
                                                            ltid,
                                                            &ltid_info,
                                                            pipe,
                                                            shr_ltid,
                                                            &shr_ltid_info,
                                                            sw_idx,
                                                            &is_reserve_index));
                    if (is_reserve_index ==  true) {
                        /* Entry is not yet written to hw.
                         * In case input hw_sid is paired view sid, Return same
                         * paired_sid. In case input hw_sid is data_onlym or
                         * tcam_onlym or combined view,
                         * return combined view hw_sid.
                         */
                        if (hw_sid == out_paired_sid_local) {
                            out_sid[0] = out_paired_sid_local;
                        } else {
                            out_sid[0] = out_hw_sid_local;
                        }
                        break;
                    }
                }
                /*
                 * Paired view's are handled in PTRM-TCAM either by using
                 * view_type variable or is_multi_mem_aggr in ltid_info
                 */
                if (shr_ltid_info.hw_entry_info->view_type) {
                    /* For LT's wich use view_type for paired views */
                    if (inp_paired_view == false) {
                        /* In is single wide view */
                        if (shr_ltid_info.hw_entry_info->view_type == 2) {
                            /* Out is paired view */
                            if (hw_dyn_info.index >= (int)ser_ptr->paired_sid_size) {
                                out_dyn_info[0].index =
                                   hw_dyn_info.index - ser_ptr->paired_sid_size;
                            } else {
                                out_dyn_info[0].index =  inp_index;
                            }
                            out_sid[0] = out_paired_sid_local;
                        } else if (shr_ltid_info.hw_entry_info->view_type == 1) {
                            out_sid[0] = out_hw_sid_local;
                        } else {
                            /* Only view type 1 or 2 are valid */
                            /* Do nothing */
                        }
                    } else {
                        /* In is paired view */
                        if (shr_ltid_info.hw_entry_info->view_type == 2) {
                            /* Out is also paired view */
                            out_sid[0] = out_paired_sid_local;
                        } else if (shr_ltid_info.hw_entry_info->view_type == 1) {
                            /* Out is single wide view */
                            out_sid[0] = out_hw_sid_local;
                            out_sid[1] = out_hw_sid_local;
                            out_dyn_info[0].index =  inp_index;
                            out_dyn_info[1] = hw_dyn_info;
                            out_dyn_info[1].index =
                                ser_ptr->paired_sid_size + inp_index;
                            *out_count = 2;
                        } else {
                            /* Only view type 1 or 2 are valid */
                            /* Do nothing */
                        }
                    }
                } else {
                    /* For LT's which use is_multi_mem_aggr for paired views */
                    if (inp_paired_view == false) {
                        /* In single wide view */
                        if (shr_ltid_info.rm_more_info[0].is_multi_mem_aggr == 0) {
                            /* Both are single wide views */
                            out_sid[0] = out_hw_sid_local;
                        } else {
                            /* Out is paired view */
                            out_sid[0] = out_paired_sid_local;
                            /*
                             * Note: multi mem aggr uses same hw_idx in 2
                             * memories to write a single wide mode entry to hw.
                             * There will be no change in hw index.
                             */
                        }
                    } else {
                        /* In is paired view */
                        if (shr_ltid_info.rm_more_info[0].is_multi_mem_aggr == 1) {
                            /* Out is paired view */
                            out_sid[0] = out_paired_sid_local;
                        } else  {
                            /* Out is single wide view */
                            out_sid[0] = out_hw_sid_local;
                            out_sid[1] = out_hw_sid_local2;
                            out_dyn_info[1].index = inp_index;
                            out_dyn_info[1].tbl_inst = hw_dyn_info.tbl_inst;
                            *out_count = 2;
                        }
                    }
                }
            }
        }
    }
    if (*out_count == 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
            "\ncb output: out_count:%d,\nout_hw_sid_0:%d, "
            "out_hw_dyn_info_0.index:%d \nout_hw_dyn_info.tbl_inst:%d.\n"),
            *out_count, out_sid[0], out_dyn_info[0].index,
            out_dyn_info[0].tbl_inst));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
            "\ncb output: out_count:%d,\nout_hw_sid_0:%d, "
            "out_hw_dyn_info_0.index:%d \nout_hw_sid_1:%d, "
            "out_hw_dyn_info_1.index:%d \nout_hw_dyn_info.tbl_inst:%d.\n"),
            *out_count, out_sid[0], out_dyn_info[0].index,
            out_sid[1], out_dyn_info[1].index, out_dyn_info[0].tbl_inst));
    }

exit:
    SHR_FUNC_EXIT();
}
