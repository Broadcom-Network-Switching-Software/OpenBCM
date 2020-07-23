/*! \file bcmcth_l2_util.c
 *
 * BCMCTH L2 utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_l2.h>
#include <bcmcth/bcmcth_l2_util.h>

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L2

/******************************************************************************
* Public functions
 */
int
bcmcth_l2_pipe_info_get(int unit, bcmdrd_sid_t sid, bcmcth_l2_pipe_info_t *pi)
{
    int blktype, idx, idx_max;
    uint32_t blk_pipe_map = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pi, SHR_E_PARAM);
    sal_memset(pi, 0, sizeof(bcmcth_l2_pipe_info_t));

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    if (blktype == -1) {
        /* PT may be unavailable on device variant */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &blk_pipe_map));

    /* convert 32 bits to bit array */
    idx = 0;
    while (blk_pipe_map != 0) {
        if (blk_pipe_map & 1) {
            SHR_BITSET(pi->map, idx);
            pi->max_idx = idx;
            pi->act_seq[idx] = pi->act_cnt;
            pi->act_cnt++;
        }
        blk_pipe_map >>= 1;
        idx++;
    }

    idx_max = bcmdrd_pt_index_max(unit, sid);
    if (idx_max < 0) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    } else {
        pi->tbl_size = idx_max + 1;
    }

    pi->entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_per_pipe_mreq_read(int unit,
                             uint32_t trans_id,
                             bcmltd_sid_t lt_id,
                             bcmdrd_sid_t pt_id,
                             int pipe,
                             int index,
                             void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    pt_info.tbl_inst = pipe;
    rsp_entry_wsize  = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_info,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    lt_id,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    entry_data,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_per_pipe_mreq_write(int unit,
                              uint32_t trans_id,
                              bcmltd_sid_t lt_id,
                              bcmdrd_sid_t pt_id,
                              int pipe,
                              int index,
                              void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    pt_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_info,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_WRITE,
                                    entry_data,
                                    0,
                                    lt_id,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_per_pipe_ireq_read(int unit,
                             bcmltd_sid_t lt_id,
                             bcmdrd_sid_t pt_id,
                             int pipe,
                             int index,
                             void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint64_t            req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    pt_info.tbl_inst = pipe;
    rsp_entry_wsize  = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit,
                              req_flags,
                              pt_id,
                              &pt_info,
                              NULL,
                              rsp_entry_wsize,
                              lt_id,
                              entry_data,
                              &rsp_ltid,
                              &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_per_pipe_ireq_write(int unit,
                              bcmltd_sid_t lt_id,
                              bcmdrd_sid_t pt_id,
                              int pipe,
                              int index,
                              void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    pt_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_write(unit,
                               BCMPTM_REQ_FLAGS_NO_FLAGS,
                               pt_id,
                               &pt_info,
                               NULL,
                               entry_data,
                               lt_id,
                               &rsp_ltid,
                               &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_hw_mreq_read_dma(int unit,
                           uint32_t trans_id,
                           bcmltd_sid_t lt_id,
                           bcmdrd_sid_t pt_id,
                           int pipe,
                           int start_index,
                           int entry_cnt,
                           void *entry_buf)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    bcmptm_misc_info_t  misc_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    pt_info.index = start_index;
    pt_info.tbl_inst = pipe;
    misc_info.dma_enable = TRUE;
    misc_info.dma_entry_count = entry_cnt;
    rsp_entry_wsize = entry_cnt * bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, &misc_info,
                                    BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    trans_id, NULL, NULL,
                                    entry_buf, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_hw_mreq_write_dma(int unit,
                            uint32_t trans_id,
                            bcmltd_sid_t lt_id,
                            bcmdrd_sid_t pt_id,
                            int pipe,
                            int start_index,
                            int entry_cnt,
                            void *entry_buf)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    bcmptm_misc_info_t  misc_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    pt_info.index = start_index;
    pt_info.tbl_inst = pipe;
    misc_info.dma_enable = TRUE;
    misc_info.dma_entry_count = entry_cnt;
    entry_wsize = entry_cnt * bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, &misc_info,
                                    BCMPTM_OP_WRITE,
                                    entry_buf, entry_wsize, lt_id,
                                    trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}
