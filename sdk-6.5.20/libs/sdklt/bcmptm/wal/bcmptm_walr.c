/*! \file bcmptm_walr.c
 *
 * Schan Fifo related utils
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
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include "./bcmptm_wal_local.h"
#include "./bcmptm_walr_local.h"


/*******************************************************************************
 * Defines
 */
#define BCMPTM_CFG_WALR_THREAD_PRIO -1 /* Use default thread prio, stack size */
#define BCMPTM_CFG_WALR_THREAD_STOP_WAIT_USEC (10 * SECOND_USEC)

#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_PIO 10
#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SCF 10
#define BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SLAM 3

#define BSL_LOG_MODULE BSL_LS_BCMPTM_WAL

#define BCMBD_SCF_START_TRUE TRUE
#define BCMBD_SCF_START_FALSE FALSE


/*******************************************************************************
 * Typedefs
 */
typedef enum {
    SCF_THREAD_STATE_STOP = 0,
    SCF_THREAD_STATE_RUN
} scf_thread_state_t;

/*! \brief Descr of wal reader thread context */
typedef struct scf_thread_info_s {
    int u;                 /*!< Logical device ID.  */
    scf_thread_state_t state; /*!< Thread state. */
} scf_thread_info_t;

typedef enum {
    CMD_STATE_IDLE = 0,
    CMD_STATE_CH_A_BUSY,
    CMD_STATE_CH_A_BUSY_CH_B_WAIT,
} cmd_state_t;

typedef enum {
    WALR_ERROR_NONE = 0,
    WALR_ERROR_PRE_MSG_CBF,
    WALR_ERROR_POST_MSG_CBF,
    WALR_ERROR_PIO_WRITE,
    WALR_ERROR_CMD_HDR_GET,
    WALR_ERROR_SCF_OPS_SEND,
    WALR_ERROR_SCF_STATUS_GET,
    WALR_ERROR_SLAM_LIGHT_WORK_INIT,
    WALR_ERROR_SLAM_WORK_EXECUTE
} walr_error_t;


/*******************************************************************************
 * Private variables
 */
static scf_thread_info_t *scf_th_info_p[BCMDRD_CONFIG_MAX_UNITS];
static walr_error_t walr_error[BCMDRD_CONFIG_MAX_UNITS];

static shr_thread_t walr_tc[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_walr_use_scf_thr[BCMDRD_CONFIG_MAX_UNITS];
static bool cfg_wal_bypass[BCMDRD_CONFIG_MAX_UNITS];
static const bcmptm_dev_info_t *dev_info[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static inline void
retry_prep(int u)
{
    sal_usleep(1000);
}

/*
 * Use error macros to allow application to catch exceptions from internal
 * thread context.
 */
static inline int
indicate_error(int u, int tmp_rv)
{
    SHR_FUNC_ENTER(u);
    SHR_ERR_EXIT(tmp_rv);
exit:
    SHR_FUNC_EXIT();
}

static inline int
pre_msg_cbf(int u, bcmptm_wal_msg_t *walr_msg, const char *caller_str)
{
    int tmp_rv = SHR_E_NONE;
    SHR_FUNC_ENTER(u);
    if (walr_msg->need_cbf_before_msg) {
        tmp_rv = bcmptm_wal_rdr_msg_done(u, walr_msg, NULL);
        if (tmp_rv != SHR_E_NONE) {
            walr_error[u] = WALR_ERROR_PRE_MSG_CBF;
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "WAL Reader (%s) pre_cbf got rv=%0d\n"),
                 caller_str, tmp_rv));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static inline int
post_msg_cbf(int u, bcmptm_wal_msg_t *walr_msg,
             void *p_cbf_status, const char *caller_str)
{
    int tmp_rv = SHR_E_NONE;
    SHR_FUNC_ENTER(u);
    tmp_rv = bcmptm_wal_rdr_msg_done(u, walr_msg, p_cbf_status);
    if (tmp_rv != SHR_E_NONE) {
        walr_error[u] = WALR_ERROR_POST_MSG_CBF;
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "WAL Reader (%s) post_cbf got rv=%0d\n"),
             caller_str, tmp_rv));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static inline int
scf_ops_send(int u, int ch, bcmptm_wal_msg_t *walr_msg, uint32_t flags,
             const char *caller_str)
{
    int tmp_rv = SHR_E_NONE;
    uint32_t retry_count = 0, *req_entry_words = NULL;
    uint64_t req_entry_words_pa;
    SHR_FUNC_ENTER(u);

    req_entry_words = bcmptm_wal_words_buf[u] + (walr_msg->words_buf_idx);
    req_entry_words_pa = bcmptm_wal_words_buf_bpa[u] +
                         4 * (walr_msg->words_buf_idx);
    SHR_IF_ERR_EXIT(req_entry_words_pa & 0xF ? SHR_E_INTERNAL : SHR_E_NONE);
    do {
        tmp_rv = bcmbd_schanfifo_ops_send(u, ch, walr_msg->num_ops,
                                          req_entry_words,
                                          walr_msg->req_entry_wsize,
                                          req_entry_words_pa, flags);
        if (tmp_rv != SHR_E_NONE) {
             LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(u, "WAL Reader (%s).scf_ops_send "
                             "failed(rv=%0d, ch=%0d, retry_count=%0d)\n"),
                  caller_str, tmp_rv, ch, retry_count));
             retry_count++;
             retry_prep(u);
        }
    } while ((tmp_rv != SHR_E_NONE) &&
             (retry_count < BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SCF));
    if (tmp_rv != SHR_E_NONE) {
        walr_error[u] = WALR_ERROR_SCF_OPS_SEND;
        SHR_ERR_EXIT(SHR_E_IO);
    }
exit:
    SHR_FUNC_EXIT();
}

static inline int
scf_status_get(int u, int ch, bcmptm_wal_msg_t *walr_msg, uint32_t flags,
               const char *caller_str)
{
    int tmp_rv;
    uint32_t num_good_ops = 0;
    SHR_FUNC_ENTER(u);
    tmp_rv = bcmbd_schanfifo_status_get(u, ch, walr_msg->num_ops,
                                        flags, &num_good_ops, NULL);
    if ((tmp_rv != SHR_E_NONE) || (num_good_ops != walr_msg->num_ops)) {
        walr_error[u] = WALR_ERROR_SCF_STATUS_GET;
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "WAL Reader (%s) scf_status_get "
                        "returned rv=%0d, obs_num_ops=%0d, "
                        "exp_num_ops=%0d\n"),
             caller_str, tmp_rv, num_good_ops, walr_msg->num_ops));
        SHR_ERR_EXIT(SHR_E_IO);
    }
exit:
    SHR_FUNC_EXIT();
}

/* No ops to send. Simply call wal_rdr_msg_donc.
 */
static int
walr_msg_cmd_empty_write_exe(int u, bcmptm_wal_msg_t *walr_msg)
{
    SHR_FUNC_ENTER(u);

    /* pre_cbf */
    SHR_IF_ERR_EXIT(pre_msg_cbf(u, walr_msg, "empty_write_exe"));

    /* send ops to HW */

    /* rdr_msg_done */
    SHR_IF_ERR_EXIT(post_msg_cbf(u, walr_msg, NULL, "empty_write_exe"));
exit:
    SHR_FUNC_EXIT();
}

/* Use PIO to send ops to HW.
 */
static int
walr_msg_cmd_write_exe0(int u, bcmptm_wal_msg_t *walr_msg)
{
    uint32_t i, retry_count = 0, *req_entry_words = NULL;
    bcmptm_wal_ops_info_t *ops_info_array;
    int tmp_rv = SHR_E_NONE;
    SHR_FUNC_ENTER(u);

    /* pre_cbf */
    SHR_IF_ERR_EXIT(pre_msg_cbf(u, walr_msg, "write_exe0"));

    /* send ops to HW */
    req_entry_words = bcmptm_wal_words_buf[u] +
                      (walr_msg->words_buf_idx);
    ops_info_array = bcmptm_wal_ops_info[u] + walr_msg->ops_info_idx;
    for (i = 0; i < walr_msg->num_ops; i++) {
        do {
            tmp_rv = bcmbd_pt_write(u,
                                    ops_info_array->sid,
                                    &ops_info_array->dyn_info,
                                    NULL, /* ovrr_info */
                                    req_entry_words + ops_info_array->op_ctrl_wsize);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "bcmptm_walr.write_exe0.bd_pt_write "
                                "failed(rv=%0d, op_index=%0d, sid=%s(%0d), "
                                "retry_count=%0d)\n"),
                     tmp_rv, i, bcmdrd_pt_sid_to_name(u, ops_info_array->sid),
                     ops_info_array->sid, retry_count));
                retry_count++;
                retry_prep(u);
            }
        } while ((tmp_rv != SHR_E_NONE) &&
                 (retry_count <= BCMPTM_CFG_WALR_MSG_RETRY_COUNT_PIO));

        /* Failed to send op to HW even after N retries */
        if (tmp_rv != SHR_E_NONE) {
            walr_error[u] = WALR_ERROR_PIO_WRITE;
            SHR_ERR_EXIT(SHR_E_IO);
        } else {
            req_entry_words += ops_info_array->op_wsize;
            ops_info_array++;
        }
    } /* for */
    if (SHR_FAILURE(tmp_rv) || (i != walr_msg->num_ops)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* rdr_msg_done */
    SHR_IF_ERR_EXIT(post_msg_cbf(u, walr_msg, NULL, "write_exe0"));

exit:
    SHR_FUNC_EXIT();
}

/* Add opcode beat, addr beat words into req_entry_words */
static int
walr_stuff_cmd_hdr(int u, bcmptm_wal_msg_t *walr_msg)
{
    uint32_t i, num_cwords, *req_entry_words = NULL;
    int tmp_rv;
    bcmptm_wal_ops_info_t *ops_info_array;
    SHR_FUNC_ENTER(u);

    /* Stuff opcode, addr beat for ops */
    req_entry_words = bcmptm_wal_words_buf[u] + (walr_msg->words_buf_idx);
    ops_info_array = bcmptm_wal_ops_info[u] + walr_msg->ops_info_idx;
    for (i = 0; i < walr_msg->num_ops; i++) {
        tmp_rv = bcmbd_pt_cmd_hdr_get(u, ops_info_array->sid,
                                      &ops_info_array->dyn_info,
                                      NULL, /* pt_ovrr_info */
                                      BCMBD_PT_CMD_WRITE,
                                      dev_info[u]->num_cwords,
                                      req_entry_words,
                                      &num_cwords);
        if (SHR_FAILURE(tmp_rv) || (num_cwords != dev_info[u]->num_cwords)) {
            walr_error[u] = WALR_ERROR_CMD_HDR_GET;
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "bcmptm_walr.cmd_hdr_get "
                            "failed(rv=%0d, op_index=%0d, num_cwords=%0d "
                            "sid=%s(%0d)\n"),
                 tmp_rv, i, num_cwords,
                 bcmdrd_pt_sid_to_name(u, ops_info_array->sid),
                 ops_info_array->sid));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        req_entry_words += ops_info_array->op_wsize;
        ops_info_array++;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Use single schanfifo chan (ch0) to send ops to HW.
 */
static int
walr_msg_cmd_write_exe1(int u, bcmptm_wal_msg_t *walr_msg)
{
    int ch = 0;
    SHR_FUNC_ENTER(u);

    /* pre_cbf */
    SHR_IF_ERR_EXIT(pre_msg_cbf(u, walr_msg, "write_exe1"));

    /* stuff cmd_hdr */
    SHR_IF_ERR_EXIT(walr_stuff_cmd_hdr(u, walr_msg));

    /* send ops to HW */
    SHR_IF_ERR_EXIT(
        scf_ops_send(u, ch, walr_msg, SCHANFIFO_OF_SET_START, "write_exe1"));

    /* check status */
    SHR_IF_ERR_EXIT(
        scf_status_get(u, ch, walr_msg,
                       (SCHANFIFO_OF_WAIT_COMPLETE | SCHANFIFO_OF_CLEAR_START),
                       "write_exe1"));

    /* rdr_msg_done */
    SHR_IF_ERR_EXIT(post_msg_cbf(u, walr_msg, NULL, "write_exe1"));

exit:
    SHR_FUNC_EXIT();
}

/* Use sbusdma to execute write
 */
static int
walr_msg_cmd_slam_exe(int u, bcmptm_wal_msg_t *walr_msg)
{
    bcmptm_wal_ops_info_t *ops_info_array;
    uint32_t cwords_array[BCMPTM_MAX_BD_CWORDS], num_cwords, retry_count = 0;
    int tmp_rv = SHR_E_NONE;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    SHR_FUNC_ENTER(u);
    ops_info_array = bcmptm_wal_ops_info[u] + walr_msg->ops_info_idx;
    tmp_rv =
        bcmbd_pt_cmd_hdr_get(u, ops_info_array->sid,
                             &(ops_info_array->dyn_info),
                             NULL, /* pt_ovrr_info */
                             BCMBD_PT_CMD_WRITE,
                             BCMPTM_MAX_BD_CWORDS,
                             cwords_array,
                             &num_cwords);
    if (SHR_FAILURE(tmp_rv) || (num_cwords != dev_info[u]->num_cwords)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = cwords_array[1]; /* Addr beat */
    dma_data.data_width = bcmdrd_pt_entry_wsize(u, ops_info_array->sid);
                                           /* Num of data beats */
     /* dma_data.addr_gap = 0; */          /* Gap between addresses */
    dma_data.op_code = cwords_array[0];    /* Opcode beat */
    dma_data.op_count = walr_msg->num_ops; /* Num of locations */
    dma_data.buf_paddr = bcmptm_wal_words_buf_bpa[u] +
                         4*(walr_msg->words_buf_idx);
    /* Phys addr for slam data.
     * Must be on 4 Byte boundary.
     * Should be on 16 Byte boundary for optimum performance. */
    if (dma_data.buf_paddr & 0xF) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    /* dma_data.jump_item = 0; */
    /* dma_data.attrs = 0; */
    dma_data.pend_clocks = bcmbd_mor_clks_write_get(u, ops_info_array->sid);

    /* Prepare dma_work */
    sal_memset(&dma_work, 0, sizeof(bcmbd_sbusdma_work_t));
    dma_work.data = &dma_data;
    dma_work.items = 1;
    /* dma_work.link = 0; */
    /* dma_work.desc = 0; */
    /* dma_work.pc = 0; */
    /* dma_work.pc_data = 0; */
    /* dma_work.cb = 0; */
    /* dma_work.cb_data = 0; */
    /* dma_work.priv = 0; */
    dma_work.flags = SBUSDMA_WF_WRITE_CMD; /* | SBUSDMA_WF_INT_MODE */
    /* dma_work.state = 0; */

    /* pre_cbf */
    SHR_IF_ERR_EXIT(pre_msg_cbf(u, walr_msg, "slam_exe"));

    tmp_rv = bcmbd_sbusdma_light_work_init(u, &dma_work);
    if (tmp_rv != SHR_E_NONE) {
        walr_error[u] = WALR_ERROR_SLAM_LIGHT_WORK_INIT;
        LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(u, "bcmptm_walr.slam_exe.sbusdma_light_work "
                    " returned rv=%0d\n"), tmp_rv));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* send ops to HW */
    do {
        tmp_rv = bcmbd_sbusdma_work_execute(u, &dma_work);
        if (tmp_rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "bcmptm_walr.slam_exe.sbusdma_work_execute "
                            "failed(rv=%0d, sid=%s(%0d), retry_count=%0d)\n"),
                 tmp_rv, bcmdrd_pt_sid_to_name(u, ops_info_array->sid),
                 ops_info_array->sid, retry_count));
            retry_count++;
            retry_prep(u);
        }
    } while ((tmp_rv != SHR_E_NONE) &&
             (retry_count <= BCMPTM_CFG_WALR_MSG_RETRY_COUNT_SLAM));
    if (tmp_rv != SHR_E_NONE) {
        walr_error[u] = WALR_ERROR_SLAM_WORK_EXECUTE;
        SHR_ERR_EXIT(SHR_E_IO);
    }

    /* rdr_msg_done */
    SHR_IF_ERR_EXIT(post_msg_cbf(u, walr_msg, NULL, "slam_exe"));
exit:
    SHR_FUNC_EXIT();
}

/* - For each u there will be separate thread_func instance - and also this
 *   function will never exit.
 *   Thus all local vars declared in this function are per u variables -
 *   implicitly.
 *
 * - shr_thr framework will call thread_func (below) with arg = th_info_p
 *   (last param specified for shr_thread_start()
 */
static void
scf_thread_func(shr_thread_t tc, void *arg)
{
    scf_thread_info_t *th_info_p = (scf_thread_info_t *)arg;
    int u, tmp_rv = SHR_E_NONE;
    uint8_t tx_ch = 0, rx_ch = 0, old_rx_ch = 0;
    uint32_t n_msg_dq_idx = 0;
    bool thread_done = FALSE, *n_walr_msg_dq_ready_p = NULL;
    bcmptm_wal_msg_t *n_walr_msg_p = NULL;
    cmd_state_t cmd_state = CMD_STATE_IDLE;

    th_info_p->state = SCF_THREAD_STATE_RUN;
    u = th_info_p->u;

    /* Must wait for nudge from WAL writer to enter endless loop.
     * WAL writer will set init value for dq_msg_idx and only then wake up WAL
     * reader to start the dqueue of msgs. */
    shr_thread_sleep(tc, SHR_THREAD_FOREVER);
    if (shr_thread_stopping(tc)) {
        thread_done = TRUE;
    }
    while (!thread_done) {
        /* Expect bcmptm_wal_msg_dq_idx[u], DQ_PTR, DQ_READY_PTR are set correctly */

        switch (cmd_state) {
/* -------------------------------------------------------------------------- */
        case CMD_STATE_IDLE:
            if (*bcmptm_wal_msg_dq_ready_p[u]) {
                switch (bcmptm_wal_msg_dq_p[u]->cmd) {
                case BCMPTM_WAL_MSG_CMD_SLAM:
                    *bcmptm_wal_msg_dq_ready_p[u] = FALSE; /* for current msg */
                    /* Must do clear of dq_ready before calling slam_exe
                     * because msg freed by post_cbf can be reused by
                     * writer and resent */
                    walr_msg_cmd_slam_exe(u, bcmptm_wal_msg_dq_p[u]);

                    /* Next msg prep */
                    bcmptm_wal_msg_dq_idx[u] =
                        bcmptm_wal_next_msg_idx(u, bcmptm_wal_msg_dq_idx[u],
                                                TRUE); /* also adjust dq_p */
                    break;

                case BCMPTM_WAL_MSG_CMD_EMPTY_WRITE:
                    *bcmptm_wal_msg_dq_ready_p[u] = FALSE; /* for current msg */
                    /* Must do clear of dq_ready before calling slam_exe
                     * because msg freed by post_cbf can be reused by
                     * writer and resent */
                    walr_msg_cmd_empty_write_exe(u, bcmptm_wal_msg_dq_p[u]);

                    /* Next msg prep */
                    bcmptm_wal_msg_dq_idx[u] =
                        bcmptm_wal_next_msg_idx(u, bcmptm_wal_msg_dq_idx[u],
                                                TRUE); /* also adjust dq_p */
                    break;

                case BCMPTM_WAL_MSG_CMD_WRITE:
                    switch (bcmptm_wal_scf_num_chans[u]) {
                    case 0:
                    case 1:
                        *bcmptm_wal_msg_dq_ready_p[u] = FALSE; /* for cur msg */
                        /* Must do clear of dq_ready before calling exe0, exe1
                         * because msg freed by post_cbf can be reused by
                         * writer and resent */

                        if (bcmptm_wal_scf_num_chans[u] &&
                            (bcmptm_wal_msg_dq_p[u]->num_ops >
                             cfg_walr_use_scf_thr[u])) {

                            walr_msg_cmd_write_exe1(u,
                                                    bcmptm_wal_msg_dq_p[u]);
                        } else {
                            walr_msg_cmd_write_exe0(u,
                                                    bcmptm_wal_msg_dq_p[u]);
                        }

                        /* Next msg prep */
                        bcmptm_wal_msg_dq_idx[u] =
                            bcmptm_wal_next_msg_idx(u, bcmptm_wal_msg_dq_idx[u],
                                                    TRUE); /* also adjust dq_p */
                        break;
                    default: /* 2 */
                        pre_msg_cbf(u, bcmptm_wal_msg_dq_p[u],
                                    "write_exe2_IDLE"); /* pre_cbf */

                        walr_stuff_cmd_hdr(u, bcmptm_wal_msg_dq_p[u]);

                        /* Send_ops to idle_ch with start=1 */
                        tx_ch = 0; /* always start with ch0 when in IDLE */
                        scf_ops_send(u, tx_ch,
                                     bcmptm_wal_msg_dq_p[u],
                                     SCHANFIFO_OF_SET_START,
                                     "write_exe2_IDLE");
                        *bcmptm_wal_msg_dq_ready_p[u] = FALSE;
                        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(u, "\tSent ops to ch%0d with start=1"
                                        " (msg_id=%0d)\n"),
                             tx_ch, bcmptm_wal_msg_dq_idx[u]));

                        /* Next cmd_state */
                        cmd_state = CMD_STATE_CH_A_BUSY;
                        rx_ch = 0; /* for status_get (busy_ch) */
                        tx_ch = 1; /* for next ops_send (idle_ch) */

                        /* Next msg prep - none */
                        break; /* 2 */
                    } /* bcmptm_wal_scf_num_chans[u] */
                    break; /* BCMPTM_WAL_MSG_CMD_WRITE */

                default: /* Error */
                    /* Illegal msg_cmd */
                    if (bcmptm_wal_msg_dq_p[u]->last_msg) {
                        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(u, "Unknown msg_cmd %0d for umsg_idx="
                                        "%0d, last_msg=T\n"),
                             bcmptm_wal_msg_dq_p[u]->cmd,
                             bcmptm_wal_msg_dq_p[u]->umsg_idx));
                    } else {
                        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(u, "Unknown msg_cmd %0d for umsg_idx="
                                        "%0d, last_msg=F\n"),
                             bcmptm_wal_msg_dq_p[u]->cmd,
                             bcmptm_wal_msg_dq_p[u]->umsg_idx));
                    }
                    indicate_error(u, SHR_E_INTERNAL);
                    break;
                } /* switch (bcmptm_wal_msg_dq_p[u]->cmd) */
            } else { /* No more msgs (for now) */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "msg_dq_ready is FALSE (msg_idx=%0d)\n"),
                     bcmptm_wal_msg_dq_idx[u]));
                /* Enter deep sleep */
                shr_thread_sleep(tc, SHR_THREAD_FOREVER);
                if (shr_thread_stopping(tc)) {
                    thread_done = TRUE;
                }
            }
        break; /* CMD_STATE_IDLE */

/* -------------------------------------------------------------------------- */
        case CMD_STATE_CH_A_BUSY:
            /* When in CH_A_BUSY state
             *     bcmptm_wal_msg_dq_p[u], bcmptm_wal_msg_dq_ready_p[u] point
             *     to busy_ch_A (which was sent first to HW).
             *
             *     n_walr_msg_p, n_walr_msg_dq_ready_p can be dont_care
             */
            /* rx_ch is busy_ch */
            /* tx_ch is idle_ch */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "\nSTATE_CH_A_BUSY: busy_ch=%0d, "
                            "idle_ch=%0d\n"),
                 rx_ch, tx_ch));

            /* Strategy: Attempt to keep HW busy by sending ops to idle_ch
             *           and then check status of busy_ch */

            /* Check if next msg is ready for dq */
            n_msg_dq_idx =
                bcmptm_wal_next_msg_idx(u, bcmptm_wal_msg_dq_idx[u],
                                        FALSE); /* don't adjust dq_p */
            if (n_msg_dq_idx) {
                n_walr_msg_dq_ready_p = bcmptm_wal_msg_dq_ready_p[u] + 1;
                n_walr_msg_p = bcmptm_wal_msg_dq_p[u] + 1;
            } else {
                n_walr_msg_dq_ready_p = bcmptm_wal_msg_dq_ready_bp[u];
                n_walr_msg_p = bcmptm_wal_msg_bp[u];
            }

            if (*n_walr_msg_dq_ready_p &&
                (n_walr_msg_p->cmd == BCMPTM_WAL_MSG_CMD_WRITE) &&
                (n_walr_msg_p->num_ops > cfg_walr_use_scf_thr[u])) {

                pre_msg_cbf(u, n_walr_msg_p, "write_exe2_CH_A_BUSY");
                walr_stuff_cmd_hdr(u, n_walr_msg_p);

                /* Send_ops to idle_ch with start=0 */
                 scf_ops_send(u, tx_ch, n_walr_msg_p, 0,
                              "write_exe2_CH_A_BUSY");
                *n_walr_msg_dq_ready_p = FALSE; /* for next msg */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "\tSent ops to ch%0d with start=0 "
                                "(msg_id=%0d)\n"), tx_ch, n_msg_dq_idx));

                /* Next cmd_state */
                cmd_state = CMD_STATE_CH_A_BUSY_CH_B_WAIT;
                /* use rx_ch for status_get (busy_ch) */
                /* use tx_ch for setting start = 1 (wait_ch) */

                /* Next msg prep - none */
            } else {
                /* Next msg not ready or next msg that does not need to
                 * be sent to scf or serc is asking to pause wall reader.
                 *
                 * If next msg was SLAM/PIO, it cannot be started to
                 * preserve ORDER. Must wait for busy_ch completion.
                 *
                 * If next msg is not valid, it is ok to simplify.
                 */

                /* Wait for busy_ch done, and clear start for busy_ch */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "\tPolling status for ch%0d (wait_done, "
                                "clear_start=1)\n"), rx_ch));
                tmp_rv = scf_status_get(u, rx_ch, bcmptm_wal_msg_dq_p[u],
                                        (SCHANFIFO_OF_WAIT_COMPLETE |
                                         SCHANFIFO_OF_CLEAR_START),
                                        "write_exe2_CH_A_BUSY");
                if (tmp_rv == SHR_E_NONE) {
                    post_msg_cbf(u, bcmptm_wal_msg_dq_p[u], NULL,
                                 "write_exe2_CH_A_BUSY");

                    /* Next cmd_state */
                    cmd_state = CMD_STATE_IDLE;
                    /* tx_ch, rx_ch not meaningful in IDLE */

                    /* Next msg prep */
                    bcmptm_wal_msg_dq_idx[u] = n_msg_dq_idx;
                    bcmptm_wal_msg_dq_ready_p[u] = n_walr_msg_dq_ready_p;
                    bcmptm_wal_msg_dq_p[u] = n_walr_msg_p;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "\nSTATE_IDLE: msg_idx=%0d\n"),
                         bcmptm_wal_msg_dq_idx[u]));
                }
            }
            break; /* CMD_STATE_CH_A_BUSY */

/* -------------------------------------------------------------------------- */
        case CMD_STATE_CH_A_BUSY_CH_B_WAIT:
            /* When in CH_A_BUSY_CH_B_WAIT state
             *     bcmptm_wal_msg_dq_p[u], bcmptm_wal_msg_dq_ready_p[u] are
             *     pointing to busy_ch_A (which was sent first to HW).
             *
             *     n_walr_msg_p, n_walr_msg_dq_ready_p are pointing to
             *     wait_ch_B (which was sent next to HW).
             */

            /* use rx_ch for status_get (busy_ch) */
            /* use tx_ch for setting start = 1 (wait_ch) */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "\nSTATE_CH_A_BUSY_CH_B_WAIT: busy_ch=%0d"
                            ", wait_ch=%0d\n"), rx_ch, tx_ch));

            /* Wait for busy_ch done, and clear start */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "\tPolling status for ch%0d (wait_done, "
                            "clear_start=1)\n"), rx_ch));
            tmp_rv = scf_status_get(u, rx_ch, bcmptm_wal_msg_dq_p[u],
                                    (SCHANFIFO_OF_WAIT_COMPLETE |
                                     SCHANFIFO_OF_CLEAR_START),
                                    "write_exe2_CH_A_BUSY_CH_B_WAIT");
            if (tmp_rv == SHR_E_NONE) {
                /* First make HW busy and then process cbf of done_ch */

                /* Set START for wait_ch */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "\tMaking start=1 for ch%0d\n"), tx_ch));
                bcmbd_schanfifo_set_start(u, tx_ch,
                                          BCMBD_SCF_START_TRUE);
                /* Above makes wait_ch (tx_ch) as the new busy_ch */

                /* post_cbf for busy_ch */
                post_msg_cbf(u, bcmptm_wal_msg_dq_p[u], NULL,
                             "write_exe2_CH_A_BUSY_CH_B_WAIT");
                /* Now (previously) busy_ch (rx_ch) becomes new idle_ch */

                /* Next cmd_state */
                cmd_state = CMD_STATE_CH_A_BUSY;
                old_rx_ch = rx_ch; /* want to xchange rx_ch, tx_ch values */
                rx_ch = tx_ch;     /* for status_get (busy_ch) */
                tx_ch = old_rx_ch; /* for next ops_send (idle_ch) */

                /* Next msg prep
                 * - make bcmptm_wal_msg_dq_p[u], bcmptm_wal_msg_dq_ready_p[u]
                 * to new busy_ch */
                bcmptm_wal_msg_dq_idx[u] = n_msg_dq_idx;
                bcmptm_wal_msg_dq_ready_p[u] = n_walr_msg_dq_ready_p;
                bcmptm_wal_msg_dq_p[u] = n_walr_msg_p;
            }
            break; /* CMD_STATE_CH_A_BUSY_CH_B_WAIT */

/* -------------------------------------------------------------------------- */
        default: /* Unknown cmd_state */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(u, "Unknown cmd_state %0d\n"), cmd_state));
            thread_done = TRUE;
            indicate_error(u, SHR_E_INTERNAL);
            break; /* unknown cmd_state */
        } /* cmd_state */
    } /* while (!thread_done) */

    /* Can be here only if we exit while loop */
    th_info_p->state = SCF_THREAD_STATE_STOP;
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "WAL reader thread function exited !!\n")));
}

/* Query bcmcfg for config vars and copy them into local vars. */
static int
walr_bcmcfg_get(int u)
{
    bcmltd_sid_t lt = DEVICE_WAL_CONFIGt;
    int tmp_rv;
    uint32_t i;
    uint64_t field_value;
    struct {
        bcmltd_fid_t lt_fid;
        uint32_t *cfg_var_p;
    } uint32_list[] = {
        {DEVICE_WAL_CONFIGt_WAL_READER_USE_FIFO_CHANNEL_OP_THRESHOLDf,
            &cfg_walr_use_scf_thr[u]}
    };
    struct {
        bcmltd_fid_t lt_fid;
        bool *cfg_var_p;
    } bool_list[] = {
        {DEVICE_WAL_CONFIGt_BYPASSf, &cfg_wal_bypass[u]}
    };
    SHR_FUNC_ENTER(u);
    for (i = 0; i < COUNTOF(uint32_list); i++) {
        /* Get values from config lt. */
        tmp_rv = bcmcfg_field_get(u, lt, uint32_list[i].lt_fid, &field_value);
        if (SHR_SUCCESS(tmp_rv)) {
            *(uint32_list[i].cfg_var_p) = (uint32_t)field_value;
        }
        SHR_IF_ERR_EXIT(tmp_rv);
    }
    for (i = 0; i < COUNTOF(bool_list); i++) {
        /* Get values from config lt. */
        tmp_rv = bcmcfg_field_get(u, lt, bool_list[i].lt_fid, &field_value);
        if (SHR_SUCCESS(tmp_rv)) {
            *(bool_list[i].cfg_var_p) = (bool)field_value;
        }
        SHR_IF_ERR_EXIT(tmp_rv);
    }

exit:
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public Functions
 */
int
bcmptm_walr_run_check(int u)
{
    scf_thread_info_t *th_info_p;
    SHR_FUNC_ENTER(u);
    th_info_p = scf_th_info_p[u];
    SHR_IF_ERR_MSG_EXIT(
        (th_info_p->state != SCF_THREAD_STATE_RUN) ? SHR_E_INTERNAL : SHR_E_NONE,
        (BSL_META_U(u, "WAL reader thread is not in RUN state\n")));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_walr_wake(int u)
{
    SHR_FUNC_ENTER(u);
    SHR_IF_ERR_EXIT(shr_thread_wake(walr_tc[u]));
exit:
    SHR_FUNC_EXIT();
}

/* WAL reader init must execute BEFORE WAL writer init.
 *
 * Note: bcmptm_walr_init() must execute in either 'init' or 'comp_config'
 *       phase and only depends on BCMCFG to have completed the 'init'
 *       (because cfg related to thread happen must be known).
 */
int
bcmptm_walr_init(int u)
{
    scf_thread_info_t *th_info_p = NULL;
    SHR_FUNC_ENTER(u);

    /* Get general device info */
    SHR_IF_ERR_EXIT(bcmptm_dev_info_get(u, &dev_info[u]));

    if (scf_th_info_p[u] != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Alloc space for th_info struct */
    SHR_ALLOC(th_info_p, sizeof(scf_thread_info_t), "bcmptmWalRdrThreadHandle");
    SHR_NULL_CHECK(th_info_p, SHR_E_MEMORY);
    sal_memset(th_info_p, 0, sizeof(scf_thread_info_t));

    /* Init th_info */
    th_info_p->u = u;
    th_info_p->state = SCF_THREAD_STATE_STOP;
    scf_th_info_p[u] = th_info_p;

    /* Create and start WAL reader thread */
    walr_tc[u] = shr_thread_start("bcmptmWalRd",
                                            BCMPTM_CFG_WALR_THREAD_PRIO,
                                            scf_thread_func,
                                            th_info_p);
    SHR_IF_ERR_MSG_EXIT(
        ((walr_tc[u] == NULL) ? SHR_E_FAIL : SHR_E_NONE),
        (BSL_META_U(u, "Failed to create WAL reader thread.\n")));

    SHR_IF_ERR_EXIT(walr_bcmcfg_get(u));

exit:
    if (SHR_FUNC_ERR()) {
        if (walr_tc[u]) {
            SHR_IF_ERR_MSG_EXIT(
                shr_thread_stop(walr_tc[u],
                                BCMPTM_CFG_WALR_THREAD_STOP_WAIT_USEC),
                (BSL_META_U(u, "WAL reader thread stop failed ")));
            walr_tc[u] = NULL;
        }
        if (th_info_p) {
            SHR_FREE(th_info_p);
        }
        scf_th_info_p[u] = NULL;

        dev_info[u] = NULL;
    } else {
    }
    SHR_FUNC_EXIT();
}

/* WAL reader cleanup must execute AFTER WAL writer cleanup.
 */
int
bcmptm_walr_cleanup(int u)
{
    scf_thread_info_t *th_info_p;
    SHR_FUNC_ENTER(u);

    dev_info[u] = NULL;

    th_info_p = scf_th_info_p[u];
    if (th_info_p == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* walr_cleanup() is called AFTER wal_cleanup() is done.
     * This ensures that WAL is empty - and so ok to stop WAL reader thread.
     */
    if (walr_tc[u]) {
        SHR_IF_ERR_MSG_EXIT(
            shr_thread_stop(walr_tc[u],
                            BCMPTM_CFG_WALR_THREAD_STOP_WAIT_USEC),
            (BSL_META_U(u, "WAL reader thread stop failed ")));
        SHR_IF_ERR_MSG_EXIT(
            (th_info_p->state != SCF_THREAD_STATE_STOP ? SHR_E_INTERNAL
                                                       : SHR_E_NONE),
            (BSL_META_U(u, "WAL reader thread state is not STOP !!")));
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    walr_tc[u] = NULL;

    /* Release th_info struct */
    SHR_FREE(th_info_p);
    scf_th_info_p[u] = NULL;

exit:
    SHR_FUNC_EXIT();
}
