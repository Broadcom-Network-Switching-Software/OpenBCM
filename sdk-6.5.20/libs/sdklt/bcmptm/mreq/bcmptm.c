/*! \file bcmptm.c
 *
 * Interface functions for PTM
 *
 * This file contains the top-level interface func for PTM
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_um_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm_wal_internal.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MREQ

/* Following is the max_tbl_inst value for which we can track insert, delete
 * requests */
#define BCMPTM_TBL_INST_MAX 31

#define LTID_INVALID (uint32_t)-1

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */
static uint32_t cp_entry_buf[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_RSP_MAX_INDEX]\
                            [BCMPTM_MAX_MREQ_ENTRY_WORDS];
static bcmptm_lt_stats_increment_cb lt_stats_cb[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_pt_status_mreq_keyed_lt_cb \
              lt_mreq_keyed_cb[BCMDRD_CONFIG_MAX_UNITS];
static bcmptm_pt_status_mreq_indexed_lt_cb \
              lt_mreq_indexed_cb[BCMDRD_CONFIG_MAX_UNITS];
static bool mreq_feature_enable_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];
static bool mreq_dyn_enable_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t have_seen_insert_delete[BCMDRD_CONFIG_MAX_UNITS];



/*******************************************************************************
 * Private Functions
 */
/* Following will be called for every NOP request from LTM
 * It changes mreq_state_in_trans to FALSE */
static int
proc_nop(int unit, uint64_t flags, uint32_t cseq_id,
         bcmptm_trans_cb_f notify_fn_ptr, void *notify_info)
{
    bcmptm_trans_cmd_t trans_cmd;
    SHR_FUNC_ENTER(unit);

    if (bcmptm_wal_autocommit_enabled(unit) &&
        (flags & BCMPTM_REQ_FLAGS_ABORT)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Abort not supported in autocommit mode \n")));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    have_seen_insert_delete[unit] = 0; /* clean slate */

    if (flags & BCMPTM_REQ_FLAGS_COMMIT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Commit: trans_id = %0d\n"), cseq_id));
        trans_cmd = BCMPTM_TRANS_CMD_COMMIT;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! NOP: received with COMMIT \n")));
    } else if (flags & BCMPTM_REQ_FLAGS_ABORT) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Abort: trans_id = %0d\n"), cseq_id));
        trans_cmd = BCMPTM_TRANS_CMD_ABORT;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! NOP: received with ABORT \n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "!! NOP: received without COMMIT/ABORT \n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Send trans_cmd first to cmdproc(WAL) on purpose:
     * - Need to restore PTcache - as RMs may uses this to reconstruct their
     *   original state.
     * - If user issues Blocking_Read_from_HW, it must follow it
     *   up with commit. wal_abort() will flag error otherwise. */
    SHR_IF_ERR_EXIT(bcmptm_wal_trans_cmd(unit, flags,
                                         trans_cmd,
                                         cseq_id, notify_fn_ptr,
                                         notify_info));
    /* And then to rsrc_mgrs */
    SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_cmd(unit, flags, cseq_id, trans_cmd));
    SHR_IF_ERR_EXIT(bcmptm_rm_tcam_trans_cmd(unit, flags, cseq_id, trans_cmd));
    SHR_IF_ERR_EXIT(bcmptm_rm_tcam_um_trans_cmd(unit, flags, cseq_id, trans_cmd));
    SHR_IF_ERR_EXIT(bcmptm_rm_alpm_trans_cmd(unit, flags, cseq_id, trans_cmd));


    /* If this is commit || abort for dynamically enabled atomic trans */
    if (mreq_dyn_enable_atomic_trans[unit]) {
        /* First disable atomic_trans for WAL, PTcache */
        SHR_IF_ERR_EXIT(bcmptm_wal_dyn_atomic_trans(unit, FALSE));

        /* Reset dyn_enable for atomic_trans */
        mreq_dyn_enable_atomic_trans[unit] = FALSE;

        /* And then to rsrc_mgrs */
        trans_cmd =  BCMPTM_TRANS_CMD_ATOMIC_TRANS_DISABLE;
        SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_cmd(unit, 0, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_tcam_trans_cmd(unit, 0, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_tcam_um_trans_cmd(unit, 0, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_trans_cmd(unit, 0, 0, trans_cmd));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ptm_check_ltm_ptrs(int unit,
                   bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
                   bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
                   unsigned int num_ekw,
                   unsigned int num_edw)
{
    unsigned int i;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltm_req_info->entry_key_words, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_req_info->entry_data_words, SHR_E_PARAM);
    /* ltm_req_info->pdd_l_dtyp can be NULL. */
    for (i = 0; i < num_ekw; i++) {
        SHR_NULL_CHECK(ltm_req_info->entry_key_words[i], SHR_E_PARAM);
    }
    for (i = 0; i < num_edw; i++) {
        SHR_NULL_CHECK(ltm_req_info->entry_data_words[i], SHR_E_PARAM);
    }

    SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_key_words, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_data_words, SHR_E_PARAM);
    /* Ok for this to be NULL (for cases where we have single entry format */
    /* SHR_NULL_CHECK(ltm_rsp_info->rsp_pdd_l_dtyp, SHR_E_PARAM); */
    for (i = 0; i < num_ekw; i++) {
        SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_key_words[i], SHR_E_PARAM);
    }
    for (i = 0; i < num_edw; i++) {
        SHR_NULL_CHECK(ltm_rsp_info->rsp_entry_data_words[i], SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mreq_rm_alpm(int unit,
             uint64_t flags,
             bcmltd_sid_t ltm_req_ltid,
             void *pt_dyn_info,
             bcmptm_op_type_t req_op,
             bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
             uint32_t cseq_id,
             const lt_mreq_info_t *lrd_info,
             bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
             bcmltd_sid_t *ltm_rsp_ltid,
             uint32_t *ltm_rsp_flags)
{
    bcmptm_rm_alpm_req_info_t alpm_req;
    bcmptm_rm_alpm_rsp_info_t alpm_rsp;

/*    uint32_t i, j, *ekw, *edw, *alpm_aggr_buf[1]; */
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Build alpm_req */
    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        alpm_req.ekw = ltm_req_info->entry_key_words[0];
        alpm_req.edw = ltm_req_info->entry_data_words[0];
        alpm_req.pt_words = lrd_info->aggr_ekw_edw;
    } else {
        /* Non-agg view not supported. */
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (ltm_req_info->pdd_l_dtyp) {
        alpm_req.dfield_format_id =
            *(ltm_req_info->pdd_l_dtyp); /* 1st element */
    }
    alpm_req.same_key = ltm_req_info->same_key;
    alpm_req.entry_pri = ltm_req_info->entry_pri;
    alpm_req.rsp_ekw_buf_wsize = ltm_req_info->rsp_entry_key_buf_wsize;
    alpm_req.rsp_edw_buf_wsize = ltm_req_info->rsp_entry_data_buf_wsize;
    alpm_req.hw_info = (bcmptm_rm_alpm_hw_entry_info_t *)lrd_info->hw_entry_info;
    alpm_req.rm_more_info = (bcmptm_rm_alpm_more_info_t *)lrd_info->rm_more_info;

    /* Init ptrs in alpm_rsp */
    alpm_rsp.rsp_ekw = ltm_rsp_info->rsp_entry_key_words[0];
    alpm_rsp.rsp_edw = ltm_rsp_info->rsp_entry_data_words[0];
    alpm_rsp.rsp_pt_op_info = ltm_rsp_info->pt_op_info;
    alpm_rsp.rsp_misc = ltm_rsp_info->rsp_misc;

    if (ltm_rsp_info->pt_op_info) {
        bcmptm_pt_op_info_t *pt_op_info = ltm_rsp_info->pt_op_info;
        sal_memset(pt_op_info, 0, sizeof(bcmptm_pt_op_info_t));
    }

    /* Post req to rsrc_mgr */
    rv = bcmptm_rm_alpm_req(unit,
                            flags,
                            cseq_id,
                            ltm_req_ltid,
                            (bcmbd_pt_dyn_info_t *)pt_dyn_info,
                            req_op,
                            &alpm_req,

                            &alpm_rsp,
                            ltm_rsp_ltid,
                            ltm_rsp_flags);

    if (rv == SHR_E_NOT_FOUND || rv == SHR_E_BUSY) {
        SHR_ERR_EXIT(rv);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Build ltm_rsp_info */
    /* rsp_entry_key_words, rsp_entry_data_words,
       already filled by rsrc_mgr */

    /* Return rsp_pdd_l_dtyp. */
    if (ltm_rsp_info->rsp_pdd_l_dtyp) {
        *(ltm_rsp_info->rsp_pdd_l_dtyp) = 0;
    }
    ltm_rsp_info->rsp_entry_pipe = 0;
    ltm_rsp_info->rsp_entry_pri = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
mreq_rm_hash(int unit,
             uint64_t flags,
             bcmltd_sid_t ltm_req_ltid,
             void *pt_dyn_info,
             bcmptm_op_type_t req_op,
             bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
             uint32_t cseq_id,
             const lt_mreq_info_t *lrd_info,

             bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
             bcmltd_sid_t *ltm_rsp_ltid,
             uint32_t *ltm_rsp_flags)
{
    bcmptm_rm_hash_req_t hash_req_info;
    bcmptm_rm_hash_rsp_t hash_rsp_info;
    uint32_t i, j, *ekw, *edw;
    int tmp_rv;
    SHR_FUNC_ENTER(unit);

    /* clear rsrc_mgr req, rsp info */
    sal_memset(&hash_req_info, 0, sizeof(bcmptm_rm_hash_req_t));
    sal_memset(&hash_rsp_info, 0, sizeof(bcmptm_rm_hash_rsp_t));

    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        /* lrd checks */
        SHR_IF_ERR_EXIT(lrd_info->num_ekw != 1? SHR_E_INTERNAL
                                              : SHR_E_NONE);
        SHR_IF_ERR_EXIT(lrd_info->num_edw != 1? SHR_E_INTERNAL
                                              : SHR_E_NONE);
        SHR_IF_ERR_EXIT(
            ptm_check_ltm_ptrs(unit,
                               ltm_req_info, ltm_rsp_info,
                               1, 1));
        SHR_IF_ERR_EXIT(
            lrd_info->aggr_ekw_edw > BCMPTM_MAX_MREQ_ENTRY_WORDS?
            SHR_E_INTERNAL : SHR_E_NONE);

        i = 0; /* only 1st row is useful for aggr LTs */
        ekw = ltm_req_info->entry_key_words[i];
        edw = ltm_req_info->entry_data_words[i];
        if (req_op != BCMPTM_OP_GET_TABLE_INFO) {
            for (j = 0; j < lrd_info->aggr_ekw_edw; j++) {
                cp_entry_buf[unit][i][j] = ekw[j] | edw[j];
            }
        }
        hash_req_info.entry_words = &cp_entry_buf[unit][i][0];
        if (ltm_req_info->pdd_l_dtyp) {
            hash_req_info.dfield_format_id =
                *(ltm_req_info->pdd_l_dtyp); /* 1st element */
        }
    } else { /* Non Aggr type LT */
        /* for hash tables, we must have aggr_ekw_edw non-zero */
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } /* Non Aggr type LT */

    /* Build remaining hash_req_info */
    hash_req_info.same_key = ltm_req_info->same_key;

    /* Pass entry_attrs to RM hash. */
    if ((lrd_info->pt_type == LT_PT_TYPE_FP_EM) ||
        (lrd_info->pt_type == LT_PT_TYPE_HASH_MPLS_DECAP)){
        hash_req_info.entry_attrs = ltm_req_info->entry_attrs;
    }
    /* Init ptrs in hash_rsp_info */
    hash_rsp_info.rsp_pt_op_info = ltm_rsp_info->pt_op_info;
    hash_rsp_info.rsp_misc = ltm_rsp_info->rsp_misc;

    /* Post req to rsrc_mgr */
    tmp_rv =
        bcmptm_rm_hash_req(unit,
                           flags,
                           cseq_id,
                           ltm_req_ltid,
                           (bcmbd_pt_dyn_info_t *)pt_dyn_info,
                           req_op,
                           &hash_req_info,
                           ltm_req_info->rsp_entry_key_buf_wsize,

                           ltm_rsp_info->rsp_entry_key_words[0],
                           &hash_rsp_info,
                           ltm_rsp_ltid,
                           ltm_rsp_flags);
    if (tmp_rv == SHR_E_NOT_FOUND || tmp_rv == SHR_E_BUSY) {
        SHR_ERR_EXIT(tmp_rv);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
    }

    i = 0; /* only 1st row is useful for aggr LTs */

    /* Ensure that edw is big enough to hold ekw */
    SHR_IF_ERR_EXIT(
        (ltm_req_info->rsp_entry_data_buf_wsize <
         ltm_req_info->rsp_entry_key_buf_wsize)? SHR_E_INTERNAL
                                               : SHR_E_NONE);
    /* Copy */
    sal_memcpy(ltm_rsp_info->rsp_entry_data_words[i], /* dst */
               ltm_rsp_info->rsp_entry_key_words[i], /* src */
               ltm_req_info->rsp_entry_key_buf_wsize * 4);

    /* Return pdd_l_dtyp */
    if (ltm_rsp_info->rsp_pdd_l_dtyp) {
        *(ltm_rsp_info->rsp_pdd_l_dtyp) =
            hash_rsp_info.rsp_dfield_format_id;
    }

    /* Build remaining ltm_rsp_info */
    ltm_rsp_info->rsp_entry_pipe = hash_rsp_info.rsp_entry_pipe;
    ltm_rsp_info->rsp_entry_pri = 0;

    if (ltm_rsp_info->pt_op_info) {
        uint8_t idx;
        bcmptm_pt_op_info_t *pt_op_info = ltm_rsp_info->pt_op_info;
        sal_memset(pt_op_info, 0, sizeof(bcmptm_pt_op_info_t));

        pt_op_info->rsp_entry_index_cnt = hash_rsp_info.rsp_entry_count;
        pt_op_info->rsp_entry_sid_cnt   = hash_rsp_info.rsp_entry_count;
        for (idx = 0; idx < hash_rsp_info.rsp_entry_count; idx++) {
            pt_op_info->rsp_entry_index[idx] = hash_rsp_info.rsp_entry_index[idx];
            pt_op_info->rsp_entry_sid[idx] = hash_rsp_info.rsp_entry_sid[idx];
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
mreq_rm_tcam(int unit,
             uint64_t flags,
             bcmltd_sid_t ltm_req_ltid,
             void *pt_dyn_info,
             bcmptm_op_type_t req_op,
             bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
             uint32_t cseq_id,
             const lt_mreq_info_t *lrd_info,

             bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
             bcmltd_sid_t *ltm_rsp_ltid,
             uint32_t *ltm_rsp_flags)
{
    bcmptm_rm_tcam_req_t tcam_req_info;
    bcmptm_rm_tcam_rsp_t tcam_rsp_info;
    uint32_t i, j, *ekw, *edw, *tcam_aggr_buf[BCMPTM_RSP_MAX_INDEX];
    int tmp_rv, idx;
    uint8_t multi_aggr_no_data_sid = 0;
    SHR_FUNC_ENTER(unit);

    /* Clear rsrc_mgr req, rsp info */
    sal_memset(&tcam_req_info, 0, sizeof(bcmptm_rm_tcam_req_t));
    sal_memset(&tcam_rsp_info, 0, sizeof(bcmptm_rm_tcam_rsp_t));

    /* Aggr type LT */
    if ((lrd_info->aggr_ekw_edw) &&
        (lrd_info->pt_type != LT_PT_TYPE_FP)) {
        /* aggr LTs with multiple rows */
        if (lrd_info->num_ekw > 1) {
            multi_aggr_no_data_sid = 1;
        }
        if (multi_aggr_no_data_sid) {
            /* lrd checks */
            SHR_IF_ERR_EXIT(
                lrd_info->num_ekw != lrd_info->num_edw ? SHR_E_INTERNAL
                                                       : SHR_E_NONE);
            SHR_IF_ERR_EXIT(
                lrd_info->num_ekw > BCMPTM_RSP_MAX_INDEX ? SHR_E_INTERNAL
                                                         : SHR_E_NONE);
            SHR_IF_ERR_EXIT(
                ptm_check_ltm_ptrs(unit,
                                   ltm_req_info, ltm_rsp_info,
                                   lrd_info->num_ekw,
                                   lrd_info->num_edw));
            SHR_IF_ERR_EXIT(
                lrd_info->aggr_ekw_edw > BCMPTM_MAX_MREQ_ENTRY_WORDS?
                SHR_E_INTERNAL : SHR_E_NONE);

            for (i = 0 ; i < lrd_info->num_ekw; i++) {
                ekw = ltm_req_info->entry_key_words[i];
                edw = ltm_req_info->entry_data_words[i];
                if (req_op != BCMPTM_OP_GET_TABLE_INFO) {
                    for (j = 0; j < lrd_info->aggr_ekw_edw; j++) {
                        cp_entry_buf[unit][i][j] = ekw[j] | edw[j];
                    }
                }
                tcam_aggr_buf[i] = cp_entry_buf[unit][i];
            }
        } else {
            /* lrd checks */
            SHR_IF_ERR_EXIT(lrd_info->num_ekw != 1? SHR_E_INTERNAL
                                                  : SHR_E_NONE);
            SHR_IF_ERR_EXIT(lrd_info->num_edw != 1? SHR_E_INTERNAL
                                                  : SHR_E_NONE);
            SHR_IF_ERR_EXIT(
                ptm_check_ltm_ptrs(unit,
                                   ltm_req_info, ltm_rsp_info,
                                   1, 1));
            SHR_IF_ERR_EXIT(
                lrd_info->aggr_ekw_edw > BCMPTM_MAX_MREQ_ENTRY_WORDS?
                SHR_E_INTERNAL : SHR_E_NONE);

            i = 0; /* only 1st row is useful for aggr LTs */
            ekw = ltm_req_info->entry_key_words[i];
            edw = ltm_req_info->entry_data_words[i];
            if (req_op != BCMPTM_OP_GET_TABLE_INFO) {
                for (j = 0; j < lrd_info->aggr_ekw_edw; j++) {
                    cp_entry_buf[unit][i][j] = ekw[j] | edw[j];
                }
            }
            tcam_aggr_buf[0] = cp_entry_buf[unit][i];
        }
        tcam_req_info.ekw = tcam_aggr_buf;
        tcam_req_info.edw = NULL; /* should be dont_care */
    } else if ((lrd_info->aggr_ekw_edw) &&
               (lrd_info->pt_type == LT_PT_TYPE_FP)) {
        SHR_IF_ERR_EXIT(
            ptm_check_ltm_ptrs(unit,
                               ltm_req_info, ltm_rsp_info,
                               BCMPTM_RSP_MAX_INDEX, BCMPTM_RSP_MAX_INDEX));
        SHR_IF_ERR_EXIT(
            lrd_info->aggr_ekw_edw > BCMPTM_MAX_MREQ_ENTRY_WORDS?
            SHR_E_INTERNAL : SHR_E_NONE);

        for (i = 0 ; i < BCMPTM_RSP_MAX_INDEX; i++) {
            ekw = ltm_req_info->entry_key_words[i];
            edw = ltm_req_info->entry_data_words[i];
            if (req_op != BCMPTM_OP_GET_TABLE_INFO) {
                for (j = 0; j < lrd_info->aggr_ekw_edw; j++) {
                    cp_entry_buf[unit][i][j] = ekw[j] | edw[j];
                }
            }
            tcam_aggr_buf[i] = cp_entry_buf[unit][i];
        }
        tcam_req_info.ekw = tcam_aggr_buf;
        tcam_req_info.edw = NULL; /* should be dont_care */
    } else { /* Non Aggr type LT */
        /* lrd checks */
        SHR_IF_ERR_EXIT(
            ptm_check_ltm_ptrs(unit,
                               ltm_req_info, ltm_rsp_info,
                               lrd_info->num_ekw,
                               lrd_info->num_edw));

        /* Must not combine ekw, edw */
        tcam_req_info.ekw = ltm_req_info->entry_key_words;
        tcam_req_info.edw = ltm_req_info->entry_data_words;
    } /* Non Aggr type LT */

    /* Build remaining tcam_req_info */
    tcam_req_info.entry_id = ltm_req_info->entry_id;
    tcam_req_info.entry_pri = ltm_req_info->entry_pri;
    tcam_req_info.same_key = ltm_req_info->same_key;
    tcam_req_info.rsp_ekw_buf_wsize =
        ltm_req_info->rsp_entry_key_buf_wsize;
    tcam_req_info.rsp_edw_buf_wsize =
        ltm_req_info->rsp_entry_data_buf_wsize;
    tcam_req_info.dfield_format_id = ltm_req_info->pdd_l_dtyp;

    if (lrd_info->pt_type == LT_PT_TYPE_FP ||
        lrd_info->pt_type ==  LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {
        tcam_req_info.entry_attrs = ltm_req_info->entry_attrs;
    }

    /* Init ptrs in tcam_rsp_info */
    tcam_rsp_info.rsp_ekw = ltm_rsp_info->rsp_entry_key_words;
    tcam_rsp_info.rsp_edw = ltm_rsp_info->rsp_entry_data_words;
    tcam_rsp_info.rsp_dfield_format_id =
        ltm_rsp_info->rsp_pdd_l_dtyp;
    tcam_rsp_info.rsp_pt_op_info = ltm_rsp_info->pt_op_info;
    tcam_rsp_info.rsp_misc = ltm_rsp_info->rsp_misc;

    /* Post req to rsrc_mgr */
    tmp_rv =
        bcmptm_rm_tcam_req(unit,
                           flags,
                           cseq_id,
                           ltm_req_ltid,
                           (bcmbd_pt_dyn_info_t *)pt_dyn_info,
                           req_op,
                           &tcam_req_info,

                           &tcam_rsp_info,
                           ltm_rsp_ltid,
                           ltm_rsp_flags);
    if (tmp_rv == SHR_E_NOT_FOUND || tmp_rv == SHR_E_BUSY) {
        SHR_ERR_EXIT(tmp_rv);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
    }

    if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
        /* rsrc_mgr returns key, data fields in rsp_ekw */

        /* Ensure that edw is big enough to hold ekw */
        SHR_IF_ERR_EXIT(
            (ltm_req_info->rsp_entry_data_buf_wsize <
             ltm_req_info->rsp_entry_key_buf_wsize)?
            SHR_E_INTERNAL : SHR_E_NONE);

        /* aggr LTs with multiple rows */
        if (multi_aggr_no_data_sid) {
            for (i = 0 ; i < lrd_info->num_ekw; i++) {
                /* Copy */
                sal_memcpy(ltm_rsp_info->rsp_entry_data_words[i], /* dst */
                           ltm_rsp_info->rsp_entry_key_words[i], /* src */
                           ltm_req_info->rsp_entry_key_buf_wsize * 4);
            }
        } else if (lrd_info->pt_type == LT_PT_TYPE_FP) {
            /* To handle multi row aggr FP LT's */
            for (i = 0 ; i < BCMPTM_RSP_MAX_INDEX; i++) {
                /* Copy */
                sal_memcpy(ltm_rsp_info->rsp_entry_data_words[i], /* dst */
                           ltm_rsp_info->rsp_entry_key_words[i], /* src */
                           ltm_req_info->rsp_entry_key_buf_wsize * 4);
            }
        } else {
            i = 0; /* only 1st row is useful for aggr LTs */

            /* Copy */
            sal_memcpy(ltm_rsp_info->rsp_entry_data_words[i], /* dst */
                       ltm_rsp_info->rsp_entry_key_words[i], /* src */
                       ltm_req_info->rsp_entry_key_buf_wsize * 4);
        }
    } else { /* Non Aggr type LT */
        /* rsrc_mgr returns key, data fields in rsp_ekw, rsp_edw
           respectively. */

        /* We must not copy rsp_ekw into rsp_edw */

    } /* Non Aggr type LT */

    /* Build remaining ltm_rsp_info */
    ltm_rsp_info->rsp_entry_pipe = tcam_rsp_info.rsp_entry_pipe;
    ltm_rsp_info->rsp_entry_pri = tcam_rsp_info.rsp_entry_pri;

    if (ltm_rsp_info->pt_op_info) {
        bcmptm_pt_op_info_t *pt_op_info = ltm_rsp_info->pt_op_info;
        sal_memset(pt_op_info, 0, sizeof(bcmptm_pt_op_info_t));

        if (lrd_info->aggr_ekw_edw) { /* Aggr type LT */
            /* aggr LTs with multiple rows */
            if ((multi_aggr_no_data_sid) ||
                (lrd_info->pt_type == LT_PT_TYPE_FP)) {
                if (tcam_rsp_info.rsp_entry_index_count > BCMPTM_RSP_MAX_INDEX) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                for (idx = 0 ; idx < tcam_rsp_info.rsp_entry_index_count; idx++) {
                    pt_op_info->rsp_entry_index[idx] =
                        tcam_rsp_info.rsp_entry_index[idx];
                    pt_op_info->rsp_entry_sid[idx] =
                        tcam_rsp_info.rsp_entry_sid[idx];
                }
                pt_op_info->rsp_entry_index_cnt =
                    tcam_rsp_info.rsp_entry_index_count;;
                pt_op_info->rsp_entry_sid_cnt = tcam_rsp_info.rsp_entry_index_count;
            } else {
                pt_op_info->rsp_entry_index_cnt = 1;
                pt_op_info->rsp_entry_index[0] = tcam_rsp_info.rsp_entry_index[0];
                pt_op_info->rsp_entry_sid_cnt = 1;
                pt_op_info->rsp_entry_sid[0] = tcam_rsp_info.rsp_entry_sid[0];
            }
        } else {
            /* KEY related info */
            if ((tcam_rsp_info.rsp_entry_index_count > BCMPTM_RSP_MAX_INDEX) ||
                (tcam_rsp_info.rsp_entry_data_index_count > BCMPTM_RSP_MAX_INDEX)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            for (idx = 0 ; idx < tcam_rsp_info.rsp_entry_index_count; idx++) {
                pt_op_info->rsp_entry_index[idx] =
                    tcam_rsp_info.rsp_entry_index[idx];
                pt_op_info->rsp_entry_sid[idx] =
                    tcam_rsp_info.rsp_entry_sid[idx];
            }
            pt_op_info->rsp_entry_index_cnt =
                tcam_rsp_info.rsp_entry_index_count;;
            pt_op_info->rsp_entry_sid_cnt = tcam_rsp_info.rsp_entry_index_count;

            /* Data related info */
            for (idx = 0 ; idx < tcam_rsp_info.rsp_entry_data_index_count; idx++) {
                pt_op_info->rsp_entry_index_data[idx] =
                    tcam_rsp_info.rsp_entry_data_index[idx];
                pt_op_info->rsp_entry_sid_data[idx] =
                    tcam_rsp_info.rsp_entry_data_sid[idx];
            }
            pt_op_info->rsp_entry_index_data_cnt =
                tcam_rsp_info.rsp_entry_data_index_count;;
            pt_op_info->rsp_entry_sid_data_cnt =
                tcam_rsp_info.rsp_entry_data_index_count;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static char *
get_req_op_name(bcmptm_op_type_t req_op)
{
    switch (req_op) {
        case BCMPTM_OP_GET_FIRST:
            return "GET_FIRST";
        case BCMPTM_OP_GET_NEXT:
            return "GET_NEXT";
        case BCMPTM_OP_LOOKUP:
            return "LOOKUP";
        case BCMPTM_OP_DELETE:
            return "DELETE";
        case BCMPTM_OP_INSERT:
            return "INSERT";
        case BCMPTM_OP_NOP:
            return "NOP";
        case BCMPTM_OP_WRITE:
            return "WRITE";
        case BCMPTM_OP_READ:
            return "READ";
        case BCMPTM_OP_GET_TABLE_INFO:
            return "GET_TABLE_INFO";
        default:
            return "UNKNOWN";
    }
}


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_mreq_indexed_lt(int unit,
                       uint64_t flags,
                       bcmdrd_sid_t sid,
                       void *in_pt_dyn_info,
                       void *pt_ovrr_info,
                       bcmptm_misc_info_t *misc_info,
                       bcmptm_op_type_t req_op,
                       uint32_t *entry_words,
                       size_t rsp_entry_wsize,
                       bcmltd_sid_t req_ltid,
                       uint32_t cseq_id,
                       bcmptm_trans_cb_f notify_fn_ptr,
                       void *notify_info,

                       uint32_t *rsp_entry_words,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags)
{
    bcmbd_pt_dyn_info_t *pt_dyn_info;
    int tmp_rv;
    bool rsp_entry_cache_vbit = FALSE;
    uint16_t rsp_dfield_format_id = 0;
    bcmptm_rm_op_t tmp_op_type;
    bcmptm_rm_slice_change_t *tmp_slice_mode_change_info;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);
    if (req_op != BCMPTM_OP_NOP) {
        SHR_NULL_CHECK(in_pt_dyn_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_ltid, SHR_E_PARAM);
    }

    pt_dyn_info = (bcmbd_pt_dyn_info_t *)in_pt_dyn_info;

    if (req_op != BCMPTM_OP_NOP) {
        bcmdrd_index_class_t ic = BCMDRD_IC_VALID;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "mreq_indexed_lt: trans_id = %0d\n"), cseq_id));

        ic = bcmdrd_pt_index_class(unit, sid, pt_dyn_info->tbl_inst,
                                   pt_dyn_info->index);
        if (ic == BCMDRD_IC_INVALID) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "mreq_indexed_lt: sid=%s, tbl_inst=%0d, "
                            "index=%0d, index_class=INVALID\n"),
                            bcmdrd_pt_sid_to_name(unit, sid),
                            pt_dyn_info->tbl_inst, pt_dyn_info->index));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    switch (req_op) {
        case BCMPTM_OP_WRITE:
            SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));

            /* update mreq_state, check cseq_id */
            SHR_IF_ERR_EXIT(bcmptm_wal_mreq_state_update(unit, cseq_id, flags));

            if (misc_info && misc_info->slice_mode_change_info) {
                tmp_op_type = BCMPTM_RM_OP_SLICE_MODE_CHANGE;
                tmp_slice_mode_change_info = misc_info->slice_mode_change_info;
            } else {
                tmp_op_type = BCMPTM_RM_OP_NORMAL;
                tmp_slice_mode_change_info = NULL;
            }
            tmp_rv =
                bcmptm_cmdproc_write(unit, /* will check *entry_words */
                                     flags,
                                     cseq_id,
                                     sid,
                                     pt_dyn_info,
                                     misc_info,
                                     entry_words,
                                     TRUE, /* write_hw */
                                     TRUE, /* write_cache */
                                     TRUE, /* set_cache_vbit */
                                     tmp_op_type, tmp_slice_mode_change_info,
                                     req_ltid,
                                     BCMPTM_DFID_MREQ, /* Note */

                                     rsp_flags);
            if (tmp_rv == SHR_E_UNAVAIL) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! ERROR: WRITE: sid = %s is not "
                                "cacheable - should not be on modeled "
                                "path !!\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
            }
            SHR_IF_ERR_EXIT(tmp_rv);

            *rsp_ltid = req_ltid;
            break; /* WRITE */

        case BCMPTM_OP_READ:
            SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));

            /* update mreq_state, check cseq_id */
            SHR_IF_ERR_EXIT(bcmptm_wal_mreq_state_update(unit, cseq_id, flags));

            tmp_rv =
                bcmptm_cmdproc_read(unit, /* will check *rsp_entry_words */
                                    flags,
                                    cseq_id,
                                    sid,
                                    pt_dyn_info,
                                    misc_info,
                                    rsp_entry_wsize,

                                    rsp_entry_words,
                                    &rsp_entry_cache_vbit,
                                    rsp_ltid,
                                    &rsp_dfield_format_id,
                                    rsp_flags);
            if (tmp_rv == SHR_E_UNAVAIL) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! ERROR: READ: sid = %s is not "
                                "cacheable - should not be on modeled "
                                "path !!\n"),
                     bcmdrd_pt_sid_to_name(unit, sid)));
            }
            SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);

            /* In general, PTcache for indexed tables on modeled path does not
             * is expected to store data_format_id.
             *
             * Exception: TCAM type mem
             * - is enabled for dfid storage (coarse)
             * - is initialized by flexcode with BCMPTM_DFID_MREQ on modeled
             *   path or from interactive path with BCMPTM_DFID_IREQ.
             * - is read from modeled path (managed as index table).
             * So, it is possible that
             * rsp_dfid = DFID_MREQ or DFID_IREQ or DFID_RSP_NOT_SUPPORTED
             *
             * Main point: there is no rsp_dfield_format_id for indexed_lt()
             * API, so such tables cannot be storing any non-zero values other
             * than what is listed above.
             */
            if (rsp_entry_cache_vbit && rsp_dfield_format_id &&
                (rsp_dfield_format_id < BCMPTM_DFID_IREQ)) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! READ: Did not expect modeled "
                                "path index table (sid = %s) to support "
                                "DFID = %0d!!\n"),
                     bcmdrd_pt_sid_to_name(unit, sid), rsp_dfield_format_id));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Read was from cache, and cache entry is not valid */
            if (!rsp_entry_cache_vbit &&
                !(flags & BCMLT_ENT_ATTR_GET_FROM_HW)) {

                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "!! Read from PTcache on modeled "
                                "path is returning HW default values - "
                                "as entry (sid, tbl_inst, index) = %s, %0d,"
                                " %0d was never written prior to read \n"),
                     bcmdrd_pt_sid_to_name(unit, sid), pt_dyn_info->tbl_inst,
                     pt_dyn_info->index));

                /* Not asserting error allows multiple LTs to share same PT
                 * entry - because for such LTs, LTM must always do
                 * read-modify-write */
                /* SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND); */

                /* For non-cacheable PTs, we will not be here - because
                   cmdproc_read will return SHR_E_UNAVAIL

                   Even if we init cache with correct 'null entry value',
                   we could still get vbit=FALSE for cacheable PTs - and
                   this indicates that there was no previous_writer (data
                   may / may not be correct depending on whether this is an
                   overlay of wider view, DATA portion of TCAM_DATA view,
                   etc). Even with vbit=TRUE, data could be incorrect. For
                   example if we read DATA_ONLY view of cacheable TCAM+DATA
                   mem then contents of DATA_ONLY are never updated and
                   hence they are not correct. Similarly, if 4x entry was
                   written, 1x reads will return vbit=FALSE.

                   For overlay mems, we can get cache_vbit as FALSE, but data is
                   still valid.
                   For TCAM mems where we don't implement overlay cache_vbit is
                   FALSE, and data is also not valid

                   For regular index tables, PTcache will return null value
                   of entry if cache_vbit=FALSE.
                   - PTcache is init with null entries and PTcache will
                     return entry data from PTcache even when cache_vbit
                     is FALSE */
            }
            break; /* READ */

        case BCMPTM_OP_NOP:
            /* *rsp_ltid = req_ltid; */ /* dont_care for NOP */
            tmp_rv = proc_nop(unit, flags, cseq_id, notify_fn_ptr,
                              notify_info); /* COMMIT/ABORT */
            
            (void)(bcmptm_mreq_unlock(unit));
            SHR_IF_ERR_EXIT(tmp_rv);
            break; /* NOP */

        default: /* ill for this API */
            *rsp_ltid = req_ltid;

            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    } /* req_op */

    /* Debug display */
    if (req_op != BCMPTM_OP_NOP) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Table_Type=%s, req_op=%s, req_sid=%s, "
                        "req_tbl_inst=%0d, req_index=%0d\n"),
             "INDEX", get_req_op_name(req_op), bcmdrd_pt_sid_to_name(unit, sid),
             pt_dyn_info->tbl_inst, pt_dyn_info->index));
    }

exit:
    
    SHR_FUNC_EXIT();
}

int
bcmptm_mreq_keyed_lt(int unit,
                     uint64_t flags,
                     bcmltd_sid_t ltm_req_ltid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     bcmptm_op_type_t req_op,
                     bcmptm_keyed_lt_mreq_info_t *ltm_req_info,
                     uint32_t cseq_id,

                     bcmptm_keyed_lt_mrsp_info_t *ltm_rsp_info,
                     bcmltd_sid_t *ltm_rsp_ltid,
                     uint32_t *ltm_rsp_flags)
{
    const lt_mreq_info_t *lrd_info = NULL;
    char *req_table_type = "UNKNOWN";
    int tmp_rv;
    SHR_FUNC_ENTER(unit);

    /* API guard check */
    SHR_IF_ERR_MSG_EXIT(
        !bcmptm_ptm_is_ready(unit) ? SHR_E_BUSY : SHR_E_NONE,
        (BSL_META_U(unit, "PTM is not ready to accept mreq, ireq API calls\n")));

    /* Param check */
    SHR_IF_ERR_EXIT(flags & BCMPTM_REQ_FLAGS_RSVD ? SHR_E_PARAM : SHR_E_NONE);
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_req_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ltm_rsp_info, SHR_E_PARAM);
    if (req_op == BCMPTM_OP_GET_TABLE_INFO) {
        SHR_NULL_CHECK(ltm_rsp_info->rsp_misc, SHR_E_PARAM);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "mreq_keyed_lt: trans_id = %0d\n"), cseq_id));
    switch (req_op) {
        case BCMPTM_OP_DELETE:
        case BCMPTM_OP_INSERT:
            if (!mreq_feature_enable_atomic_trans[unit] &&
                !mreq_dyn_enable_atomic_trans[unit] &&
                !bcmptm_wal_autocommit_enabled(unit)) {
                uint32_t have_seen_ins_del = have_seen_insert_delete[unit];
                int tmp_tbl_inst =
                    ((bcmbd_pt_dyn_info_t *)pt_dyn_info)->tbl_inst;

                 if ((tmp_tbl_inst < 0) ||
                     (tmp_tbl_inst >= BCMPTM_TBL_INST_MAX)) {
                     tmp_tbl_inst = BCMPTM_TBL_INST_MAX;
                         /* Use MSB for global case and when tbl_inst is
                          * out-of-range. */
                 }
                 if ((have_seen_ins_del >> tmp_tbl_inst) && 0x1) {
                     LOG_ERROR(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Multiple inserts, deletes are not "
                                     "allowed for tbl_inst %0d (bin %0d) when"
                                     " enable_atomic_trans = 0\n"),
                          ((bcmbd_pt_dyn_info_t *)pt_dyn_info)->tbl_inst,
                          tmp_tbl_inst));
                     /* clear bit corresponding to tbl_inst */
                     have_seen_ins_del &= ~(1 << (tmp_tbl_inst % 32));

                     SHR_ERR_EXIT(SHR_E_INTERNAL);
                 } else {
                     /* set bit corresponding to tbl_inst */
                     have_seen_ins_del |= (1 << (tmp_tbl_inst % 32));
                 }
                 have_seen_insert_delete[unit] = have_seen_ins_del;
            }
            /* coverity[fallthrough: FALSE] */
        case BCMPTM_OP_GET_TABLE_INFO:
            /* DONT_USE_CACHE flag is only for LOOKUP, READ, TRAVERSE */
            flags &= ~BCMLT_ENT_ATTR_GET_FROM_HW;
            /* coverity[fallthrough: FALSE] */
        case BCMPTM_OP_LOOKUP:
        case BCMPTM_OP_GET_FIRST:
        case BCMPTM_OP_GET_NEXT:
            SHR_IF_ERR_EXIT(bcmptm_mreq_lock(unit));

            /* update mreq_state, check cseq_id */
            SHR_IF_ERR_EXIT(bcmptm_wal_mreq_state_update(unit, cseq_id, flags));

            /* query for LT info */
            SHR_IF_ERR_MSG_EXIT(
                bcmlrd_lt_mreq_info_get(unit, ltm_req_ltid, &lrd_info),
                (BSL_META_U(unit, "Logical Table info needed by RM is not "
                            "available for Logical Table ID %0u\n"),
                 ltm_req_ltid));
            SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);

            switch (lrd_info->pt_type) {
                case LT_PT_TYPE_HASH:
                case LT_PT_TYPE_FP_EM:
                case LT_PT_TYPE_HASH_MPLS_DECAP:
                    req_table_type = "HASH";
                    tmp_rv = mreq_rm_hash(unit,
                                          flags,
                                          ltm_req_ltid,
                                          pt_dyn_info,
                                          req_op,
                                          ltm_req_info,
                                          cseq_id,
                                          lrd_info,

                                          ltm_rsp_info,
                                          ltm_rsp_ltid,
                                          ltm_rsp_flags);
                    break; /* rm_hash */

                case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
                case LT_PT_TYPE_TCAM_PRI_ONLY:
                case LT_PT_TYPE_FP:
                    req_table_type = "TCAM_PRI";
                    tmp_rv = mreq_rm_tcam(unit,
                                          flags,
                                          ltm_req_ltid,
                                          pt_dyn_info,
                                          req_op,
                                          ltm_req_info,
                                          cseq_id,
                                          lrd_info,

                                          ltm_rsp_info,
                                          ltm_rsp_ltid,
                                          ltm_rsp_flags);
                    break; /* rm_tcam */

                case LT_PT_TYPE_LPM:
                    req_table_type = "ALPM";
                    tmp_rv = mreq_rm_alpm(unit,
                                          flags,
                                          ltm_req_ltid,
                                          pt_dyn_info,
                                          req_op,
                                          ltm_req_info,
                                          cseq_id,
                                          lrd_info,
                                          ltm_rsp_info,
                                          ltm_rsp_ltid,
                                          ltm_rsp_flags);
                    break;

                default: /* ill pt_type */
                    tmp_rv = SHR_E_UNAVAIL;
                    break;
            } /* pt_type */
            SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);

            /* Debug display */
            if (ltm_rsp_info->pt_op_info) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Table_Type=%s, req_op=%s, rsp_sid=%s, "
                                "rsp_index=%0d, rsp_entry_pri=%0d\n"),
                     req_table_type,
                     get_req_op_name(req_op),
                     bcmdrd_pt_sid_to_name(
                         unit, ltm_rsp_info->pt_op_info->rsp_entry_sid[0]),
                     ltm_rsp_info->pt_op_info->rsp_entry_index[0],
                     ltm_rsp_info->rsp_entry_pri));
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Table_Type=%s, req_op=%s, "
                                "rsp_entry_pri=%0d\n"),
                     req_table_type,
                     get_req_op_name(req_op),
                     ltm_rsp_info->rsp_entry_pri));
            }
            break; /* LOOKUP, DELETE, INSERT */

        default: /* ill req_op */
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
            /* break; */
    } /* req_op */

exit:
    
    SHR_FUNC_EXIT();
}

void
bcmptm_lt_stat_increment_register(int unit,
                                  bcmptm_lt_stats_increment_cb lt_stat_cb)
{
    lt_stats_cb[unit] = lt_stat_cb;
}

void
bcmptm_lt_stat_increment(int unit, uint32_t ltid, uint32_t stat_field)
{
    if (ltid != LTID_INVALID) {
        if (lt_stats_cb[unit] != NULL) {
            lt_stats_cb[unit](unit, ltid, stat_field);
        }
    }
}

void
bcmptm_pt_status_mreq_keyed_lt_register(int unit,
                      bcmptm_pt_status_mreq_keyed_lt_cb pt_status_cb)
{
    lt_mreq_keyed_cb[unit] = pt_status_cb;
}

void
bcmptm_pt_status_mreq_indexed_lt_register(int unit,
                      bcmptm_pt_status_mreq_indexed_lt_cb pt_status_cb)
{
    lt_mreq_indexed_cb[unit] = pt_status_cb;
}

int
bcmptm_ltm_mreq_indexed_lt(int unit,
                           uint64_t flags,
                           bcmdrd_sid_t sid,
                           void *pt_dyn_info,
                           void *pt_ovrr_info,
                           bcmptm_misc_info_t *misc_info,
                           bcmptm_op_type_t req_op,
                           uint32_t *entry_words,
                           size_t rsp_entry_wsize,
                           bcmltd_sid_t req_ltid,
                           uint32_t cseq_id,
                           bcmptm_trans_cb_f notify_fn_ptr,
                           void *notify_info,
                           uint32_t *rsp_entry_words,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);

    bcmptm_lt_stat_increment(unit, req_ltid,
                             TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);

    SHR_IF_ERR_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                flags,
                                sid,
                                pt_dyn_info,
                                pt_ovrr_info,
                                misc_info,
                                req_op,
                                entry_words,
                                rsp_entry_wsize,
                                req_ltid,
                                cseq_id,
                                notify_fn_ptr,
                                notify_info,
                                rsp_entry_words,
                                rsp_ltid,
                                rsp_flags));

    if (lt_mreq_indexed_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_mreq_indexed_cb[unit](unit, req_ltid, cseq_id, sid,
                                      pt_dyn_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_lt_stat_increment(unit, req_ltid,
                                 TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ltm_mreq_keyed_lt(int unit,
                         uint64_t flags,
                         bcmltd_sid_t req_ltid,
                         void *pt_dyn_info,
                         void *pt_ovrr_info,
                         bcmptm_op_type_t req_op,
                         bcmptm_keyed_lt_mreq_info_t *req_info,
                         uint32_t cseq_id,
                         bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid,
                         uint32_t *rsp_flags)
{
    SHR_FUNC_ENTER(unit);

    bcmptm_lt_stat_increment(unit, req_ltid,
                             TABLE_STATSt_PHYSICAL_TABLE_OP_CNTf);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_keyed_lt(unit,
                              flags,
                              req_ltid,
                              pt_dyn_info,
                              pt_ovrr_info,
                              req_op,
                              req_info,
                              cseq_id,
                              rsp_info,
                              rsp_ltid,
                              rsp_flags));

    if (lt_mreq_keyed_cb[unit] != NULL) {
        SHR_IF_ERR_EXIT
            (lt_mreq_keyed_cb[unit](unit, req_ltid, cseq_id, pt_dyn_info,
                                    rsp_info));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_lt_stat_increment(unit, req_ltid,
                                 TABLE_STATSt_PHYSICAL_TABLE_OP_ERROR_CNTf);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_mreq_init(int unit, bool warm)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        mreq_feature_enable_atomic_trans[unit] = FALSE;
    } else {
        mreq_feature_enable_atomic_trans[unit] = TRUE;
    }

    /* Initially */
    mreq_dyn_enable_atomic_trans[unit] = FALSE;
    have_seen_insert_delete[unit] = 0;

/* exit: */
    SHR_FUNC_EXIT();
}


int
bcmptm_mreq_stop(int unit)
{
    bcmptm_trans_cmd_t trans_cmd = BCMPTM_TRANS_CMD_IDLE_CHECK;
    uint64_t flags = 0;
    int tmp_rv;
    bool failed = FALSE;
    SHR_FUNC_ENTER(unit);

    if (bcmptm_wal_autocommit_enabled(unit)) {
        /* rsrc_mgrs - check they are in idle */

        /* RM code already supports trans_cmd - so RMs will get to IDLE state
         * only when COMMIT is passed to RMs.
         *
         * But RM unittests dont issue any commit - they rely on auto-commit mode
         * of WAL - and thus there are no commits to RMs and hence RMs in
         * unit-tests end up in non-IDLE state.
         *
         * Hence, following will reduce severity to WARN and not create failure
         * when in auto-commit mode.
         */
        tmp_rv = bcmptm_rm_hash_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Hash rsrc mgr is not in IDLE \n")));
        }
        tmp_rv = bcmptm_rm_tcam_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Tcam rsrc mgr is not in IDLE \n")));
        }
        tmp_rv = bcmptm_rm_tcam_um_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Tcam_um rsrc mgr is not in IDLE \n")));
        }
        tmp_rv = bcmptm_rm_alpm_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! ALPM rsrc mgr is not in IDLE \n")));
        }
    } else {
        /* Must do idle_check for all RMs, WAL - even if some of them fail */
        tmp_rv = bcmptm_rm_hash_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Hash rsrc mgr is not in IDLE "
                            "(rv = %0d)\n"), tmp_rv));
            failed = TRUE;
        }
        tmp_rv = bcmptm_rm_tcam_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Tcam rsrc mgr is not in IDLE "
                            "(rv = %0d)\n"), tmp_rv));
            failed = TRUE;
        }
        tmp_rv = bcmptm_rm_tcam_um_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! Tcam_um rsrc mgr is not in IDLE "
                            "(rv = %0d)\n"), tmp_rv));
            failed = TRUE;
        }
        tmp_rv = bcmptm_rm_alpm_trans_cmd(unit, flags, 0, trans_cmd);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "!! ALPM rsrc mgr is not in IDLE "
                            "(rv = %0d)\n"), tmp_rv));
            failed = TRUE;
        }
    }

    /* We already ensured that LTM (and hence TRM) has stopped, so we know LTM
     * is not issuing more requests */

    /* wal_mreq_idle_check */
    SHR_IF_ERR_EXIT(
        bcmptm_wal_trans_cmd(unit, flags, trans_cmd, 0, NULL, NULL));

    /* wal drain: TRUE => end_state must be IDLE */
    SHR_IF_ERR_EXIT(bcmptm_wal_drain(unit, TRUE));

    /* Case where we have no errors from wal - but one or more RMs had failed */
    if (failed) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_mreq_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    (void)(bcmptm_mreq_unlock(unit));

    mreq_feature_enable_atomic_trans[unit] = FALSE;
    mreq_dyn_enable_atomic_trans[unit] = FALSE;
    have_seen_insert_delete[unit] = 0;

/* exit: */
    SHR_FUNC_EXIT();
}

int
bcmptm_mreq_atomic_trans_enable(int unit)
{
    bcmptm_trans_cmd_t trans_cmd;
    bool enable;
    SHR_FUNC_ENTER(unit);

    /* Works only when feature.enable_atomic_trans = 0, autocommit mode is not
     * enabled. */
    if (!mreq_feature_enable_atomic_trans[unit] &&
        !bcmptm_wal_autocommit_enabled(unit)) {
        /* 1st WAL, PTcache */
        enable = TRUE;
        SHR_IF_ERR_EXIT(bcmptm_wal_dyn_atomic_trans(unit, enable));

        /* Then RMs */
        trans_cmd = BCMPTM_TRANS_CMD_ATOMIC_TRANS_ENABLE;
        SHR_IF_ERR_EXIT(bcmptm_rm_hash_trans_cmd(unit, 0, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_tcam_trans_cmd(unit, 0, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_tcam_um_trans_cmd(unit, 0, 0, trans_cmd));
        SHR_IF_ERR_EXIT(bcmptm_rm_alpm_trans_cmd(unit, 0, 0, trans_cmd));

        /* remember we are in dynamically enabled atomic trans */
        mreq_dyn_enable_atomic_trans[unit] = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}
