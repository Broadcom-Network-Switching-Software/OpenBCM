/*! \file bcm56990_b0_pkt_trace_internal.c
 *
 * Packet trace internal function
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bsl/bsl.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ltm_types.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmpkt/bcmpkt_trace_internal.h>

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_TRACE

const bcmpkt_trace_pt_to_dop_info_get_t bcm56990_b0_trace_pt_to_dop_info_get = {
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    }
};

static int
bcm56990_b0_trace_init(int unit)
{
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t rsp_flags, entry_words;
    CPU_PKT_PROFILE_1r_t pkt_prof_1;
    CPU_PKT_PROFILE_2r_t pkt_prof_2;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;

    sid = CPU_PKT_PROFILE_1r;
    req_ltid = sid;
    CPU_PKT_PROFILE_1r_CLR(pkt_prof_1);
    CPU_PKT_PROFILE_1r_DO_NOT_LEARNf_SET(pkt_prof_1, 0xaa);
    CPU_PKT_PROFILE_1r_IFP_KEY_CONTROLf_SET(pkt_prof_1, 0xcc);
    entry_words = CPU_PKT_PROFILE_1r_GET(pkt_prof_1);
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           BCMPTM_REQ_FLAGS_PASSTHRU,
                           sid,
                           &pt_dyn_info,
                           NULL,
                           &entry_words,
                           req_ltid,
                           &rsp_ltid,
                           &rsp_flags));

    sid = CPU_PKT_PROFILE_2r;
    req_ltid = sid;
    CPU_PKT_PROFILE_2r_CLR(pkt_prof_2);
    CPU_PKT_PROFILE_2r_DROP_TO_MMUf_SET(pkt_prof_2, 0xf0);
    entry_words = CPU_PKT_PROFILE_2r_GET(pkt_prof_2);
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           BCMPTM_REQ_FLAGS_PASSTHRU,
                           sid,
                           &pt_dyn_info,
                           NULL,
                           &entry_words,
                           req_ltid,
                           &rsp_ltid,
                           &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_trace_data_get(int unit, int port, uint32_t *len, uint8_t *data)
{
    int pipe_num, result_avil = 0, blktype;
    uint8_t mem_idx, index, max_idx;
    uint32_t rsp_flags, byte_cnt, mem_width, temp;
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    const bcmltd_sid_t mem_info[] = {
        PTR_RESULTS_BUFFER_IADAPTm,
        PTR_RESULTS_BUFFER_IPARSm,
        PTR_RESULTS_BUFFER_IFWDm,
        PTR_RESULTS_BUFFER_ISWm
    };

    SHR_FUNC_ENTER(unit);

    for (mem_idx=0, byte_cnt=0 ; mem_idx < COUNTOF(mem_info) ; mem_idx++)
    {
        sid = mem_info[mem_idx];
        mem_width = bcmdrd_pt_entry_wsize(unit, sid);
        max_idx = bcmdrd_pt_index_max(unit, sid);
        blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        pipe_num = bcmdrd_dev_blk_port_pipe(unit, blktype, port);
        pt_dyn_info.tbl_inst = pipe_num;
        req_ltid = sid;
        for (index=0 ; index <= max_idx; index++)
        {
            pt_dyn_info.index = index;
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_read(unit,
                                  BCMPTM_REQ_FLAGS_PASSTHRU,
                                  sid,
                                  &pt_dyn_info,
                                  NULL,
                                  mem_width,
                                  req_ltid,
                                  (uint32_t *)(data + byte_cnt),
                                  &rsp_ltid,
                                  &rsp_flags));
            temp = *(uint32_t *)(data + byte_cnt);
            *(uint32_t *)(data + byte_cnt) = *(uint32_t *)(data + byte_cnt + 4);
            *(uint32_t *)(data + byte_cnt + 4) = temp;

            if (*(uint32_t *)(data + byte_cnt) != 0) {
                result_avil = 1;
            }
            byte_cnt += mem_width * 4;

        }
    }
    if (!result_avil) {
        *len = 0;
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    *len = byte_cnt;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_trace_get_txpmd_val(uint32_t dev_type, int fid, int *val)
{
    if (fid == BCMPKT_TXPMD_SET_L2BM) {
        *val = 1;
    }

    if (fid == BCMPKT_TXPMD_UNICAST) {
        *val = 0;
    }

    return SHR_E_NONE;
}

static int
bcm56990_b0_trace_get_lbpipe(int unit, int pipe, int *lbpipe)
{
    if (pipe % 2 == 0) {
        *lbpipe = pipe + 1;
    }

    return SHR_E_NONE;
}

static void
bcm56990_b0_trace_counter_shift(uint32_t *counter,
                                uint32_t *shift_counter)
{
    *shift_counter = ((counter[0] & 0x1fff) << 19) |
                     ((counter[1] >> 13) & 0x7ffff);
}

int
bcm56990_b0_trace_drv_attach(bcmpkt_trace_drv_t *trace_drv)
{
    if (trace_drv == NULL) {
        return SHR_E_FAIL;
    }

    trace_drv->profile_init = bcm56990_b0_trace_init;
    trace_drv->get_lbpipe = bcm56990_b0_trace_get_lbpipe;
    trace_drv->data_read = bcm56990_b0_trace_data_get;
    trace_drv->get_txpmd_val = bcm56990_b0_trace_get_txpmd_val;
    trace_drv->counter_shift = bcm56990_b0_trace_counter_shift;

    return SHR_E_NONE;
}

