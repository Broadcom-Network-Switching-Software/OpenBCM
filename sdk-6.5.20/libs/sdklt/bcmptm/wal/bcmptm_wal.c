/*! \file bcmptm_wal.c
 *
 * This file implements WAL sub-block of PTM
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
#include <sal/sal_sleep.h>
#include <sal/sal_mutex.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_wal_internal.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include "./bcmptm_wal_local.h"
#include "./bcmptm_walr_local.h"


/*******************************************************************************
 * Defines
 */
#define ACC_FOR_SER FALSE

#define BCMPTM_UT_WAL_TRANS_MAX_COUNT 1448
#define BCMPTM_UT_WAL_MSG_MAX_COUNT 1448
#define BCMPTM_UT_WAL_WORDS_BUF_MAX_COUNT 491520
#define BCMPTM_UT_WAL_OPS_INFO_MAX_COUNT 49152
#define BCMPTM_UT_WAL_UNDO_OPS_INFO_MAX_COUNT 49152
#define BCMPTM_UT_WAL_UNDO_WORDS_BUF_MAX_COUNT 491520

/* Max number of (cwords, dwords) that can be accommodated in one schan_fifo.
 * WAL writer will chop off ops for a single transaction into multiple chunks
 * each of which will have req_entry_wsize <= what is defined below */
#define BCMPTM_UT_WAL_MAX_WORDS_IN_MSG 352 /* 16 max size writes = 16 * 22
                                               words */

/* Max number of ops that can be accommodated in one schan_fifo.
 * WAL writer will chop off ops for a single transaction into multiple chunks
 * each of which will have num_ops <= what is defined below */
#define BCMPTM_UT_WAL_MAX_OPS_IN_MSG 1000000 /* No limit */

/* Max number of msgs in trans
 * - This option is dont-care for normal mode
 * - This option is meaningful only in auto-commit mode.
 * - When in auto-commit mode, trans will get auto-committed when num_msgs in
 * trans becomes equal to this value.
 */
#define BCMPTM_UT_WAL_MAX_MSGS_IN_TRANS 1000000 /* No limit */

#define BCMPTM_WAL_SCF_IGNORE_SER_ABORT TRUE

#define BSL_LOG_MODULE BSL_LS_BCMPTM_WAL

#define WAL_USLEEP_TIME 1000 /* uSec */
#define WAL_AVAIL_SPACE_RETRY_COUNT_MAX 20

#define WAL_EMPTY_CHECK_USLEEP_TIME 100000 /* uSec */
#define WAL_EMPTY_CHECK_RETRY_COUNT 20

/* Declare error if cannot get lock for wstate after this time.
 * No retries */
#define WSTATE_LOCK_WAIT_USEC 2000000
#define SLICE_MODE_LOCK_WAIT_USEC 2000000
#define ENTRY_MODE_LOCK_WAIT_USEC 2000000

#define C_TRANS_IDX_ILL 0xFFFFFFFF
#define C_MSG_IDX_ILL 0xFFFFFFFF

#define SLICE_MAX 511 /* Any number > 511 means -1 */


/*******************************************************************************
 * Typedefs
 */
typedef enum {
    WAL_MEM_TYPE_NONE = 0,
    WAL_MEM_TYPE_HA, /* allocated using shr_ha_mem_alloc */
    WAL_MEM_TYPE_DMA, /* allocated using hal_dma_alloc */
    WAL_MEM_TYPE_DMA_CRASH_PROOF /* allocated using ha_dma_memget */
} wal_mem_t;

typedef struct wal_trans_s {
    uint32_t cseq_id;
    uint32_t f_msg_idx;       /* 1st msg in this trans - for HA replay (idx for wal_msgs[]) */
    uint32_t num_msgs;        /* total num of messages in this trans - for HA replay */

    /* needed for quick abort and also for error checking */
    uint32_t num_ops;         /* total num of ops in this trans */
    uint32_t num_words;       /* total num of words in this trans */
    uint32_t slam_msg_count; /* total num of slam_msgs in this trans */

    bcmptm_trans_cb_f notify_fn_p;
    void *notify_info;
} wal_trans_t;

typedef enum {
    TRANS_STATE_IDLE = 0,      /* will need to alloc new trans and new msg */
    TRANS_STATE_IN_MSG,        /* must add new op to msg */
    TRANS_STATE_NEED_NEW_MSG,  /* will need to alloc new msg */
    TRANS_STATE_BLOCKING_READ,  /* will need to alloc new msg */
    TRANS_STATE_BLOCKING_READ_AFTER_WR,  /* will need to alloc new msg */
    TRANS_STATE_IDLE_AFTER_WR  /* will need to alloc new msg */
} wal_trans_state_t;

typedef struct wstate_s {
    bool mreq_state_in_trans;
    uint32_t mreq_cseq_id;
    bool posted_scf_write;
    bool sent_pio_write;

    wal_trans_state_t c_trans_state;
    uint32_t c_trans_idx; /* Needed to init trans_idx field in msg.
                             Also needed for abort */
    uint32_t c_msg_idx;   /* Needed when we have trans with multi-messages */
    uint32_t c_msg_commit_count; /* num of msgs committed */

    /* next: where next msg, op, etc will be written */
    uint32_t n_trans_idx;
    uint32_t n_msg_idx;
    uint32_t n_wal_ops_info_idx;
    uint32_t n_words_buf_idx;
    uint32_t n_undo_ops_info_idx;
    uint32_t n_undo_words_buf_idx;
    uint32_t n_umsg_idx;

    uint32_t avail_undo_ops_info_count;
    uint32_t avail_undo_words_buf_count;

    uint32_t w_trans_count;
    uint32_t w_msg_count;
    uint32_t w_wal_ops_info_count;
    uint32_t w_words_buf_count;

    uint32_t r_trans_count;
    uint32_t r_msg_count;
    uint32_t r_wal_ops_info_count;
    uint32_t r_words_buf_count;

    /* Vars related to current transaction being worked on by WAL reader */
    uint32_t n_rdr_msg_idx;  /* Next msg_idx that rdr is expecting to be
                                serviced */
    uint32_t rdr_msg_count;         /* Num of msgs received for given transaction */
    uint32_t rdr_num_ops;           /* num of ops sent to HW */
    uint32_t rdr_num_words;         /* num of words sent to HW */
    uint32_t rdr_slam_msg_count;    /* num of slam_msgs sent to HW */

    /* Info to speedup Abort
     * idx can be restored by setting
     *     n_var_idx = a_var_idx; */
    /* uint32_t a_trans_idx; same as c_trans_idx */
    uint32_t a_msg_idx;
    uint32_t a_wal_ops_info_idx;
    uint32_t a_words_buf_idx;
    uint32_t a_umsg_idx;

    uint32_t a_w_trans_count;
    uint32_t a_w_msg_count;
    uint32_t a_w_wal_ops_info_count;
    uint32_t a_w_words_buf_count;
} wstate_t;

/*! Info to be stored in WAL to allow undo of ptcache and also help with SER
 * correction.
 */
typedef struct wal_undo_ops_s {
    /*! sid to be used for ptcache_undo */
    bcmdrd_sid_t undo_sid;

    /*! dyn_info to be used for ptcache_undo */
    bcmbd_pt_dyn_info_t undo_dyn_info;

    /*! cache_rsp_vbit for ptcache_undo */
    bool undo_cache_vbit;

    /*! dfid for ptcache_undo */
    uint16_t undo_dfid;

    /*! ltid for ptcache_undo */
    bcmltd_sid_t undo_ltid;
} wal_undo_ops_t;

/*! Template for bcmptm_wal_write() */
typedef int(*fn_wal_write_t)(int u, uint64_t flags,
                             bcmdrd_sid_t sid,
                             bcmbd_pt_dyn_info_t *pt_dyn_info,
                             bcmptm_misc_info_t *misc_info,
                             uint32_t *entry_words,
                             bcmptm_rm_op_t op_type,
                             bcmptm_rm_slice_change_t *slice_mode_change_info);

/*! Template for bcmptm_wal_trans_cmd() */
typedef int(*fn_wal_trans_cmd_t)(int u, uint64_t flags,
                                 bcmptm_trans_cmd_t trans_cmd,
                                 uint32_t in_cseq_id,
                                 bcmptm_trans_cb_f notify_fn_p,
                                 void *notify_info);

typedef int(*fn_wal_mreq_state_update_t)(int u, uint32_t in_cseq_id,
                                       uint64_t flags);

/*! WAL functions. */
typedef struct wal_fn_ptr_all_s {
    fn_wal_write_t fn_wal_write;
    fn_wal_trans_cmd_t fn_wal_trans_cmd;
    fn_wal_mreq_state_update_t fn_wal_mreq_state_update;
} wal_fn_ptr_all_t;


/*******************************************************************************
 * Private variables
 */
static wal_fn_ptr_all_t cfg_wal_fn_ptr_all[BCMDRD_CONFIG_MAX_UNITS];
static bool cfg_wal_bypass[BCMDRD_CONFIG_MAX_UNITS];
static bool prev_cfg_wal_bypass[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_dma_op_count_thr[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_slam_op_count_thr[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t cfg_wal_trans_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_msg_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_ops_info_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_words_buf_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_undo_ops_info_max_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_undo_words_buf_max_count[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t cfg_wal_trans_idx_max[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_msg_idx_max[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_ops_info_idx_max[BCMDRD_CONFIG_MAX_UNITS];
/* static uint32_t cfg_wal_words_buf_idx_max[BCMDRD_CONFIG_MAX_UNITS]; */
static uint32_t cfg_undo_ops_info_idx_max[BCMDRD_CONFIG_MAX_UNITS];
/* static uint32_t cfg_undo_words_buf_idx_max[BCMDRD_CONFIG_MAX_UNITS]; */

static uint32_t cfg_wal_max_words_in_msg[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_max_ops_in_msg[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_wal_max_msgs_in_trans[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t cfg_scf_num_chans[BCMDRD_CONFIG_MAX_UNITS];
static bool cfg_wal_stats_enable[BCMDRD_CONFIG_MAX_UNITS];

static wal_trans_t *wal_trans[BCMDRD_CONFIG_MAX_UNITS];
static wal_undo_ops_t *undo_ops_info[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t *undo_words_buf[BCMDRD_CONFIG_MAX_UNITS];

static wstate_t *wstate[BCMDRD_CONFIG_MAX_UNITS];

static sal_mutex_t wstate_mutex[BCMDRD_CONFIG_MAX_UNITS];
static bool have_wstate_mutex[BCMDRD_CONFIG_MAX_UNITS];

static int cfg_wal_mode[BCMDRD_CONFIG_MAX_UNITS];

static uint8_t mc_count[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_RM_MC_GROUP_COUNT_MAX];

static bool wal_disable_undo[BCMDRD_CONFIG_MAX_UNITS];

static bool wal_msg_dq_enable[BCMDRD_CONFIG_MAX_UNITS];
static bool dma_avail[BCMDRD_CONFIG_MAX_UNITS];
static wal_mem_t words_buf_mem_type[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t cfg_scf_init_max_polls[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t cfg_scf_init_flags[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t s_min_avail_trans_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_min_avail_msg_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_min_avail_ops_info_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_min_avail_words_buf_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_min_avail_undo_ops_info_count[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_min_avail_undo_words_buf_count[BCMDRD_CONFIG_MAX_UNITS];

static uint32_t s_max_words_in_msg[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_max_ops_in_msg[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_max_msgs_in_trans[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t s_max_slam_in_trans[BCMDRD_CONFIG_MAX_UNITS];
static const bcmptm_dev_info_t *dev_info[BCMDRD_CONFIG_MAX_UNITS];
static bool wal_dyn_enable_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Global vars exported
 */
uint64_t bcmptm_wal_words_buf_bpa[BCMDRD_CONFIG_MAX_UNITS];
uint32_t *bcmptm_wal_words_buf[BCMDRD_CONFIG_MAX_UNITS];
bcmptm_wal_ops_info_t *bcmptm_wal_ops_info[BCMDRD_CONFIG_MAX_UNITS];

bcmptm_wal_msg_t *bcmptm_wal_msg_bp[BCMDRD_CONFIG_MAX_UNITS];

/* base_ptr for array that stores dq_ready bits */
bool *bcmptm_wal_msg_dq_ready_bp[BCMDRD_CONFIG_MAX_UNITS];

/* Following vars are
 *      - initialized in writer-context once (to get reader started)
 *      - incremented in reader-context (when reader is done sending msg ops *      msg)
 */
bool *bcmptm_wal_msg_dq_ready_p[BCMDRD_CONFIG_MAX_UNITS];
        /* points to dq_ready bit */
uint32_t bcmptm_wal_msg_dq_idx[BCMDRD_CONFIG_MAX_UNITS];
        /* offset for msg to dequeue and it's corresponding ready bit */
bcmptm_wal_msg_t *bcmptm_wal_msg_dq_p[BCMDRD_CONFIG_MAX_UNITS];
        /* points to msg for dequeue */

int bcmptm_wal_scf_num_chans[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
static inline int
posted_wr_status_check(int u, int ch)
{
    int tmp_rv = SHR_E_NONE;
    if (wstate[u]->posted_scf_write) {
        uint32_t num_good_ops = 0, pwc = 1;
        wstate[u]->posted_scf_write = FALSE; /* Even for error case */
        tmp_rv = bcmbd_schanfifo_status_get(u, ch, pwc,
                                            (SCHANFIFO_OF_WAIT_COMPLETE |
                                             SCHANFIFO_OF_CLEAR_START),
                                            &num_good_ops, NULL);
        if ((num_good_ops != pwc)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(u, "Previous_Write.status_get failed: "
                                  "(exp_good_ops=%0d, obs_good_ops=%0d)\n"),
                       pwc, num_good_ops));
            if (tmp_rv == SHR_E_NONE) {
                tmp_rv = SHR_E_INTERNAL;
            }
        }
    }
    return tmp_rv;
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

static inline uint32_t
avail_count_get(int u, uint32_t wc, uint32_t rc, uint32_t cfgc)
{
    uint32_t pc;
    if (wc >= rc) {
        pc = wc - rc;
    } else {
        pc = (0xFFFFFFFF - rc) + (1 + wc);
    }
    if (pc > cfgc) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(u, "avail_count_get: pc > cfgc : "
                              "wc=%0u, rc=%0u, cfgc=%0u, pc=%0u\n"),
                   wc, rc, cfgc, pc));
        indicate_error(u, SHR_E_INTERNAL);
        return 0;
    }
    return (cfgc - pc);
}

static void
wal_avail_count_eval(int u, uint32_t *avail_trans_count,
                     uint32_t *avail_msg_count,
                     uint32_t *avail_wal_ops_info_count,
                     uint32_t *avail_words_buf_count)
{
    *avail_trans_count =
        avail_count_get(u, wstate[u]->w_trans_count,
                        wstate[u]->r_trans_count,
                        cfg_wal_trans_max_count[u]);
    *avail_msg_count =
        avail_count_get(u, wstate[u]->w_msg_count,
                        wstate[u]->r_msg_count,
                        cfg_wal_msg_max_count[u]);
    *avail_wal_ops_info_count =
        avail_count_get(u, wstate[u]->w_wal_ops_info_count,
                        wstate[u]->r_wal_ops_info_count,
                        cfg_wal_ops_info_max_count[u]);
    *avail_words_buf_count =
        avail_count_get(u, wstate[u]->w_words_buf_count,
                        wstate[u]->r_words_buf_count,
                        cfg_wal_words_buf_max_count[u]);
}

static inline bool
pending_trans_count1(int u)
{
    uint32_t avail_trans_count =
        avail_count_get(u, wstate[u]->w_trans_count,
                        wstate[u]->r_trans_count,
                        cfg_wal_trans_max_count[u]);
    return (cfg_wal_trans_max_count[u] - avail_trans_count) == 1;
}

/* Every WAL array depth is normalized to '1000'.
 * If device dependent depth for buffer is d, and
 * - default value of 'mult' is 1000.
 * - if user specifies 'mult' as  500, final depth = (d * 500) / 1000 = d/2
 * - user specifies 'mult' as  2000, final depth = (d * 2000) / 1000 = d*2
 *
 * Consider device-dependent d = 1448 and user specifies 'mult' as 333
 * - (333 * 1448) = 482184
 * - (333 * 1448) / 1000 = 482184 / 1000 = 482 (about one third)
 *
 * Note: multiplication must be done before division.
 */
static inline uint32_t
wal_depth_scale(uint32_t cfg_wal_depth_mult, uint32_t d)
{
    return (cfg_wal_depth_mult * d) / 1000;
}
/* Query bcmcfg for config vars and copy them into local vars.
 * Will be called once - as very first part of bcmptm_wal_init() */
static int
wal_bcmcfg_get(int u)
{
    bcmltd_sid_t lt = DEVICE_WAL_CONFIGt;
    int tmp_rv;
    uint32_t i, cfg_wal_depth_mult;
    uint64_t field_value;
    bool cfg_scf_poll_wait, cfg_scf_ccmdma_wr, cfg_scf_max_polls_override;
    bool cfg_pio_bulk_read, cfg_pio_bulk_write, cfg_pio_bulk_copy;
    bcmptm_wal_info_t device_wal_info; /* device-dependent value */
    struct {
        bcmltd_fid_t lt_fid;
        uint32_t *cfg_var_p;
    } uint32_list[] = {
        {DEVICE_WAL_CONFIGt_WAL_DEPTH_MULTIPLIERf, &cfg_wal_depth_mult},
        {DEVICE_WAL_CONFIGt_FIFO_CHANNELS_MODEf, &cfg_scf_num_chans[u]},
        {DEVICE_WAL_CONFIGt_FIFO_CHANNELS_MAX_POLLSf,
            &cfg_scf_init_max_polls[u]},
        {DEVICE_WAL_CONFIGt_DMA_READ_OP_THRESHOLDf,
            &cfg_wal_dma_op_count_thr[u]},
        {DEVICE_WAL_CONFIGt_DMA_WRITE_OP_THRESHOLDf,
            &cfg_wal_slam_op_count_thr[u]},
    };
    struct {
        bcmltd_fid_t lt_fid;
        bool *cfg_var_p;
    } bool_list[] = {
        {DEVICE_WAL_CONFIGt_FIFO_CHANNELS_POLLf, &cfg_scf_poll_wait},
        {DEVICE_WAL_CONFIGt_FIFO_CHANNELS_MAX_POLLS_OVERRIDEf,
            &cfg_scf_max_polls_override},
        {DEVICE_WAL_CONFIGt_FIFO_CHANNELS_DMAf, &cfg_scf_ccmdma_wr},
        {DEVICE_WAL_CONFIGt_BYPASSf, &cfg_wal_bypass[u]}
    };
    struct {
        bcmltd_fid_t lt_fid;
        bool *cfg_var_p;
    } bool_list2[] = { /* for fields in global DEVICE_CONFIG */
        {DEVICE_CONFIGt_PIO_BULK_WRITEf, &cfg_pio_bulk_write},
        {DEVICE_CONFIGt_PIO_BULK_READf, &cfg_pio_bulk_read},
        {DEVICE_CONFIGt_PIO_BULK_COPYf, &cfg_pio_bulk_copy}
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

    /* Now read DEVICE_CONFIGt to get global config params */
    lt = DEVICE_CONFIGt;
    for (i = 0; i < COUNTOF(bool_list2); i++) {
        /* Get values from config lt. */
        tmp_rv = bcmcfg_field_get(u, lt, bool_list2[i].lt_fid, &field_value);
        if (SHR_SUCCESS(tmp_rv)) {
            *(bool_list2[i].cfg_var_p) = (bool)field_value;
        }
        SHR_IF_ERR_EXIT(tmp_rv);
    }

    cfg_scf_init_flags[u] = 0;
    if (BCMPTM_WAL_SCF_IGNORE_SER_ABORT) {
        cfg_scf_init_flags[u] |= SCHANFIFO_IF_IGNORE_SER_ABORT;
    }
    if (cfg_scf_poll_wait) {
       cfg_scf_init_flags[u] |= SCHANFIFO_IF_POLL_WAIT;
    }
    if (cfg_scf_ccmdma_wr && !cfg_pio_bulk_copy) {
       cfg_scf_init_flags[u] |= SCHANFIFO_IF_CCMDMA_WR;
    }
    if (cfg_pio_bulk_read) {
        cfg_wal_dma_op_count_thr[u] = 0xFFFFFFFF;
    }
    if (cfg_pio_bulk_write) {
        cfg_wal_slam_op_count_thr[u] = 0xFFFFFFFF;
    }

    if (!cfg_scf_max_polls_override) {
        cfg_scf_init_max_polls[u] = 0; /* use SW driver default */
    } /* override with user-defined value
       * else {
       *    use cfg-specified value cfg_scf_init_max_polls[u];
       * }
       */

    SHR_IF_ERR_EXIT(bcmptm_wal_info_get(u, &device_wal_info));
    cfg_wal_trans_max_count[u] =
        wal_depth_scale(cfg_wal_depth_mult, device_wal_info.trans_max_count);

    cfg_wal_msg_max_count[u] =
        wal_depth_scale(cfg_wal_depth_mult, device_wal_info.msg_max_count);

    cfg_wal_words_buf_max_count[u] =
        wal_depth_scale(cfg_wal_depth_mult,
                        device_wal_info.words_buf_max_count);

    cfg_wal_ops_info_max_count[u] =
        wal_depth_scale(cfg_wal_depth_mult, device_wal_info.ops_info_max_count);

    cfg_undo_ops_info_max_count[u] =
        wal_depth_scale(cfg_wal_depth_mult,
                        device_wal_info.undo_ops_info_max_count);

    cfg_undo_words_buf_max_count[u] =
        wal_depth_scale(cfg_wal_depth_mult,
                        device_wal_info.undo_words_buf_max_count);

    cfg_wal_max_words_in_msg[u] = device_wal_info.max_words_in_msg;
exit:
    SHR_FUNC_EXIT();
}

/* Use sbusdma to execute read
 *
 * ASSUME: Caller has taken scf_mutex before calling this function.
 */
static int
wal_dma_read(int u,
             bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *pt_dyn_info,
             uint32_t entry_count,
             uint32_t *rsp_entry_words,
             size_t rsp_buf_wsize)
{
    uint32_t cwords_array[BCMPTM_MAX_BD_CWORDS], num_cwords, pt_entry_wsize;
    int tmp_rv = SHR_E_NONE;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(
        bcmbd_pt_cmd_hdr_get(u, sid, pt_dyn_info, NULL,
                             BCMBD_PT_CMD_READ,
                             BCMPTM_MAX_BD_CWORDS,
                             cwords_array,
                             &num_cwords));
    SHR_IF_ERR_EXIT(
        (num_cwords != dev_info[u]->num_cwords) ? SHR_E_INTERNAL : SHR_E_NONE);
    pt_entry_wsize = bcmdrd_pt_entry_wsize(u, sid);

    /* Prepare dma_data */
    sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
    dma_data.start_addr = cwords_array[1]; /* Addr beat */
    dma_data.data_width = pt_entry_wsize;  /* Num of data beats */
     /* dma_data.addr_gap = 0; */          /* Gap between addresses */
    dma_data.op_code = cwords_array[0];    /* Opcode beat */
    dma_data.op_count = entry_count;       /* Num of locations */
    dma_data.buf_paddr = bcmptm_wal_words_buf_bpa[u];
        /* Phys addr for slam data.
         * Must be on 4 Byte boundary.
         * Should be on 16 Byte boundary for optimum performance.
         * For read, wal is empty, so words_buf_bpa is pa for DMA. */
    /* dma_data.jump_item = 0; */
    /* dma_data.attrs = 0; */
    dma_data.pend_clocks = bcmbd_mor_clks_read_get(u, sid);

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
    dma_work.flags = SBUSDMA_WF_READ_CMD; /* | SBUSDMA_WF_INT_MODE */
    /* dma_work.state = 0; */

    tmp_rv = bcmbd_sbusdma_light_work_init(u, &dma_work);
    if (tmp_rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(u, "bcmptm_wal_dma_read.sbusdma_light_work "
                    "returned rv=%0d\n"), tmp_rv));
    } else {
        tmp_rv = bcmbd_sbusdma_work_execute(u, &dma_work);
        if (tmp_rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "bcmptm_wal_dma_read.sbusdma_work_"
                            "execute returned rv=%0d\n"), tmp_rv));
        } else { /* Copy data from DMA buf (WAL) to user buffer */
            uint32_t exp_buf_wsize = entry_count * pt_entry_wsize;
            SHR_IF_ERR_MSG_EXIT(
                (rsp_buf_wsize < exp_buf_wsize) ? SHR_E_PARAM : SHR_E_NONE,
                (BSL_META_U(u, "rsp_buf_wsize is not enough\n")));
            sal_memcpy(rsp_entry_words, bcmptm_wal_words_buf[u],
                       4 * exp_buf_wsize);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Must be called only when wstate[u]->c_trans_state is NEED_NEW_MSG,IN_MSG
 */
static int
wal_ptcache_undo(int u, wal_trans_t *trans_p)
{
    uint32_t c_msg_idx, entry_count,
             f_undo_ops_info_idx, c_undo_ops_info_idx, c_undo_num_ops,
             f_undo_words_buf_idx, c_undo_words_buf_idx, c_undo_num_words;
    bcmptm_wal_msg_t *c_msg_p;
    wal_undo_ops_t *c_undo_ops_info_p;
    SHR_FUNC_ENTER(u);

    c_msg_idx = wstate[u]->c_msg_idx; /* last (latest) msg */
        /* c_msg_idx is last msg in trans even if c_trans_state=NEED_NEW_MSG */
    c_msg_p = bcmptm_wal_msg_bp[u] + c_msg_idx;

    /* Need to traverse the ops (and hence msgs) from latest (last)
     * to oldest (first) */
    do { /* foreach trans.msg */
        /* ops in msg */
        f_undo_ops_info_idx = c_msg_p->undo_ops_info_idx; /* 1st op */
        switch (c_msg_p->cmd) {
        case BCMPTM_WAL_MSG_CMD_WRITE:
            c_undo_num_ops = c_msg_p->num_ops;
            entry_count = 1;
            break;
        case BCMPTM_WAL_MSG_CMD_SLAM:
            c_undo_num_ops = 1;
            entry_count = c_msg_p->num_ops;
            break;
        default:
            c_undo_num_ops = 0;
            entry_count = 0;
            break;
        } /* c_msg_p->cmd */

        /* words in msg */
        f_undo_words_buf_idx = c_msg_p->undo_words_buf_idx; /* 1st word */
        c_undo_num_words = c_msg_p->undo_num_words;

        /* In each msg:
         * - all undo_words will be consecutive (no wraparound guaranteed)
         * - all undo_ops will be consecutive (no wraparound guaranteed)
         * - first undo_op in msg is oldest
         * - last  undo_op in msg is latest
         * - we want to go from latest to oldest
         */

        /* Work on msg only if there are undo ops */
        if (c_undo_num_ops) {
            SHR_IF_ERR_EXIT(!c_undo_num_words ? SHR_E_INTERNAL : SHR_E_NONE);

            /* last op in msg */
            c_undo_ops_info_idx = f_undo_ops_info_idx + (c_undo_num_ops - 1);
                /* Note: RHS can become 0xFFFFFFFF, hence used () on RHS
                 *       But this requires 1st RHS term to be really big which
                 *       implies we have 4G deep ops array -impossible */
            c_undo_ops_info_p = undo_ops_info[u] + c_undo_ops_info_idx;

            /* last+1 word in msg */
            /* Note: Will following work if RHS is 0 ?
             *       It requires 1st RHS term to be really big which implies we
             *       have 4G deep words array - impossible. */
            c_undo_words_buf_idx = f_undo_words_buf_idx + c_undo_num_words;
            do { /* foreach msg.op */
                if (c_msg_p->cmd == BCMPTM_WAL_MSG_CMD_SLAM) {
                    c_undo_words_buf_idx = f_undo_words_buf_idx;
                } else {
                    /* 1st word of this op (is also last+1 word of prev op) */
                    c_undo_words_buf_idx -=
                        bcmdrd_pt_entry_wsize(u, c_undo_ops_info_p->undo_sid);
                }
                SHR_IF_ERR_EXIT(
                    ((c_undo_ops_info_idx < f_undo_ops_info_idx) ||
                     (c_undo_words_buf_idx < f_undo_words_buf_idx)) ?
                    SHR_E_INTERNAL : SHR_E_NONE);

                SHR_IF_ERR_EXIT(
                    bcmptm_ptcache_update(u, ACC_FOR_SER,
                                          c_undo_ops_info_p->undo_sid,
                                          &(c_undo_ops_info_p->undo_dyn_info),
                                          entry_count,
                                          undo_words_buf[u] +
                                          c_undo_words_buf_idx,

                                          c_undo_ops_info_p->undo_cache_vbit,
                                          c_undo_ops_info_p->undo_ltid,
                                          c_undo_ops_info_p->undo_dfid));

                if (c_undo_ops_info_idx == f_undo_ops_info_idx) {
                    SHR_IF_ERR_EXIT(
                        (c_undo_words_buf_idx != f_undo_words_buf_idx) ?
                        SHR_E_INTERNAL : SHR_E_NONE);
                    break; /* done with all ops in this msg */
                } else {
                    c_undo_ops_info_idx--;
                    c_undo_ops_info_p--;
                }
            } while (1); /* foreach msg.op */
        } /* process ops in cur msg */

        /* Go to earlier(older) msg
         * (ops, data_words) in single msg cannot cross wraparound boundary
         * But, msgs in trans can cross wraparound boundary */
        if (c_msg_idx == trans_p->f_msg_idx) {
            /* Finished processing ops for first (oldest) msg */
            break; /* done */
        } else {
            if (c_msg_idx == 0) {
                c_msg_idx = cfg_wal_msg_idx_max[u];
                c_msg_p = bcmptm_wal_msg_bp[u] + c_msg_idx;
            } else {
                c_msg_idx--;
                c_msg_p--;
            }
        }
    } while (1); /* foreach trans.msg */

exit:
    SHR_FUNC_EXIT();
}

static void
print_wstate_info(int u, bool error, const char *msg_prefix)
{
    BSL_LOG(BSL_LOG_MODULE | (error ? BSL_ERROR : BSL_VERBOSE),
            (BSL_META_U(u, "%s: Wstate_info: c_trans_state=%s(%0d), "
                        "c_trans_idx=%0u, c_msg_idx=%0u, "
                        "c_msg_commit_count=%0u, "

                        "n_trans_idx=%0u, "
                        "n_msg_idx=%0u, "
                        "n_wal_ops_info_idx=%0u, "
                        "n_words_buf_idx=%0u, "
                        "n_undo_ops_info_idx=%0u, "
                        "n_undo_words_buf_idx=%0u, "
                        "n_umsg_idx=%0u, "
                        "avail_undo_ops_info_count=%0u, "
                        "avail_undo_words_buf_count=%0u, "

                        "w_trans_count=%0u, "
                        "w_msg_count=%0u, "
                        "w_wal_ops_info_count=%0u, "
                        "w_words_buf_count=%0u, "

                        "r_trans_count=%0u, "
                        "r_msg_count=%0u, "
                        "r_wal_ops_info_count=%0u, "
                        "r_words_buf_count=%0u, "

                        "n_rdr_msg_idx=%0u, "
                        "rdr_msg_count=%0u, "
                        "rdr_num_ops=%0u, "
                        "rdr_num_words=%0u, "
                        "rdr_slam_msg_count=%0u, "

                        "a_msg_idx=%0u, "
                        "a_wal_ops_info_idx=%0u, "
                        "a_words_buf_idx=%0u, "
                        "a_umsg_idx=%0u, "

                        "a_w_trans_count=%0u, "
                        "a_w_msg_count=%0u, "
                        "a_w_wal_ops_info_count=%0u, "
                        "a_w_words_buf_count=%0u\n\n"),

                        msg_prefix,
                        (wstate[u]->c_trans_state == TRANS_STATE_IN_MSG ?
                         "IN_MSG" :
                         wstate[u]->c_trans_state == TRANS_STATE_NEED_NEW_MSG ?
                         "NEED_NEW_MSG" : "IDLE"),
                        wstate[u]->c_trans_state,
                        wstate[u]->c_trans_idx,
                        wstate[u]->c_msg_idx,
                        wstate[u]->c_msg_commit_count,

                        wstate[u]->n_trans_idx,
                        wstate[u]->n_msg_idx,
                        wstate[u]->n_wal_ops_info_idx,
                        wstate[u]->n_words_buf_idx,
                        wstate[u]->n_undo_ops_info_idx,
                        wstate[u]->n_undo_words_buf_idx,
                        wstate[u]->n_umsg_idx,

                        wstate[u]->avail_undo_ops_info_count,
                        wstate[u]->avail_undo_words_buf_count,

                        wstate[u]->w_trans_count,
                        wstate[u]->w_msg_count,
                        wstate[u]->w_wal_ops_info_count,
                        wstate[u]->w_words_buf_count,

                        wstate[u]->r_trans_count,
                        wstate[u]->r_msg_count,
                        wstate[u]->r_wal_ops_info_count,
                        wstate[u]->r_words_buf_count,

                        wstate[u]->n_rdr_msg_idx,
                        wstate[u]->rdr_msg_count,
                        wstate[u]->rdr_num_ops,
                        wstate[u]->rdr_num_words,
                        wstate[u]->rdr_slam_msg_count,

                        wstate[u]->a_msg_idx,
                        wstate[u]->a_wal_ops_info_idx,
                        wstate[u]->a_words_buf_idx,
                        wstate[u]->a_umsg_idx,

                        wstate[u]->a_w_trans_count,
                        wstate[u]->a_w_msg_count,
                        wstate[u]->a_w_wal_ops_info_count,
                        wstate[u]->a_w_words_buf_count));
}

static void
print_msg_info(int u, bool error, uint32_t msg_idx,
               bcmptm_wal_msg_t *wal_msg_p,
               const char *msg_prefix)
{
    BSL_LOG(BSL_LOG_MODULE | (error ? BSL_ERROR : BSL_VERBOSE),
       (BSL_META_U(u,
        "%s: Msg_info: umsg_idx=%0u, trans_idx=%0u, msg_idx=%0u, cmd=%s, "
        "words_buf_idx=%0u, req_entry_wsize=%0u, "
        "undo_ops_info_idx=%0u, "
        "num_ops=%0u, undo_words_buf_idx=%0u, "
        "undo_num_words=%0u, num_words_skipped=%0u \n\n"),
        msg_prefix, wal_msg_p->umsg_idx, wal_msg_p->trans_idx, msg_idx,
        wal_msg_p->cmd == BCMPTM_WAL_MSG_CMD_WRITE ? "WRITE" :
        wal_msg_p->cmd == BCMPTM_WAL_MSG_CMD_SLAM ? "SLAM" :
        wal_msg_p->cmd == BCMPTM_WAL_MSG_CMD_NOP ? "NOP" : "THREAD_EXIT",
        wal_msg_p->words_buf_idx, wal_msg_p->req_entry_wsize,
        wal_msg_p->undo_ops_info_idx,
        wal_msg_p->num_ops, wal_msg_p->undo_words_buf_idx,
        wal_msg_p->undo_num_words, wal_msg_p->num_words_skipped));
}

static void
print_trans_info(int u, bool error, uint32_t trans_idx,
                 wal_trans_t *trans_p, const char *msg_prefix)
{
    BSL_LOG(BSL_LOG_MODULE | (error ? BSL_ERROR : BSL_VERBOSE),
       (BSL_META_U(u,
                   "%s: Trans_info: trans_idx=%0u, cseq_id=%0u, "
                   "f_msg_idx=%0u, num_msgs=%0u, num_ops=%0u, "
                   "num_words=%0u, "
                   "slam_msg_count=%0u, "
                   "notify_fn=%s \n\n"),
                   msg_prefix, trans_idx, trans_p->cseq_id, trans_p->f_msg_idx,
                   trans_p->num_msgs, trans_p->num_ops,
                   trans_p->num_words,
                   trans_p->slam_msg_count,
                   trans_p->notify_fn_p ? "yes" : "no"));
}

/* !! MUST NOT USE in WAL reader context */
static int
wal_wstate_lock(int u)
{
    int tmp_rv;
    if (have_wstate_mutex[u]) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "!! Already have wstate_mutex. Should not be "
                        "requesting again \n")));
        tmp_rv = SHR_E_INTERNAL;
    } else {
        tmp_rv = bcmptm_mutex_take(u, wstate_mutex[u], WSTATE_LOCK_WAIT_USEC);
        if (tmp_rv == SHR_E_NONE) {
            have_wstate_mutex[u] = TRUE;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "!! Could not get wstate_mutex \n")));
        }
    }
    return tmp_rv;
}

/* !! MUST NOT USE in WAL reader context */
static int
wal_wstate_unlock(int u)
{
    int tmp_rv;
    if (!have_wstate_mutex[u]) {
        tmp_rv = SHR_E_INTERNAL;
    } else {
        tmp_rv = sal_mutex_give(wstate_mutex[u]);
        if (tmp_rv == SHR_E_NONE) {
            have_wstate_mutex[u] = FALSE;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "!! Could not release wstate_mutex \n")));
        }
    }
    return tmp_rv;
}

static int
wal_empty_check(int u, bool show_not_empty_values,
                bool *empty, wal_trans_state_t *wal_c_trans_state)
{
    SHR_FUNC_ENTER(u);
    *empty = FALSE;
    *wal_c_trans_state = wstate[u]->c_trans_state;
    if ((wstate[u]->w_trans_count == wstate[u]->r_trans_count)
        && (wstate[u]->w_msg_count == wstate[u]->r_msg_count)
        && (wstate[u]->w_wal_ops_info_count == wstate[u]->r_wal_ops_info_count)
        && (wstate[u]->w_words_buf_count == wstate[u]->r_words_buf_count)
        && (wstate[u]->avail_undo_ops_info_count ==
            cfg_undo_ops_info_max_count[u])
        && (wstate[u]->avail_undo_words_buf_count ==
            cfg_undo_words_buf_max_count[u])
        && (wstate[u]->n_msg_idx == wstate[u]->n_rdr_msg_idx)
       ) {
        *empty = TRUE;
    } else if (show_not_empty_values) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "wal_empty_check() failed... \n"
            "trans_count: w_ = %0u, r_ = %0u \n,"
            "msg_count: w_ = %0u, r_ = %0u \n,"
            "wal_ops_info_count: w_ = %0u, r_ = %0u \n,"
            "words_buf_count: w_ = %0u, r_ = %0u \n,"
            "avail_undo_ops_info_count: obs = %0u, exp = %0u \n,"
            "avail_undo_words_buf_count: obs = %0u, exp = %0u \n,"
            "c_trans_state: obs = %0d\n,"
            "n_msg_idx = %0u, n_rdr_msg_idx = %0u \n\n"),
            wstate[u]->w_trans_count, wstate[u]->r_trans_count,
            wstate[u]->w_msg_count, wstate[u]->r_msg_count,
            wstate[u]->w_wal_ops_info_count, wstate[u]->r_wal_ops_info_count,
            wstate[u]->w_words_buf_count, wstate[u]->r_words_buf_count,
            wstate[u]->avail_undo_ops_info_count,
            cfg_undo_ops_info_max_count[u],
            wstate[u]->avail_undo_words_buf_count,
            cfg_undo_words_buf_max_count[u],
            wstate[u]->c_trans_state,
            wstate[u]->n_msg_idx, wstate[u]->n_rdr_msg_idx));
    }
/* exit: */
    SHR_FUNC_EXIT();
}

static inline void
wstate_init(int u)
{
    wstate[u]->avail_undo_ops_info_count = cfg_undo_ops_info_max_count[u];
    wstate[u]->avail_undo_words_buf_count = cfg_undo_words_buf_max_count[u];

    wstate[u]->mreq_state_in_trans = FALSE;
    wstate[u]->mreq_cseq_id = 0; /* x when mreq_state_in_trans is FALSE */
    wstate[u]->posted_scf_write = FALSE;
    wstate[u]->sent_pio_write = FALSE;
}

static int
wal_ha_mem_clear(int u, uint8_t submod_id, uint32_t *ref_ha_p)
{
    uint32_t req_size, alloc_size, *alloc_p = NULL;
    const char *submod_name;
    SHR_FUNC_ENTER(u);

    if (!ref_ha_p) {
        /* Nothing to clear */
        SHR_FUNC_EXIT();
    }

    switch (submod_id) {
    case BCMPTM_HA_SUBID_WAL_TRANS:
        submod_name = "ptmWalTrans";
        break;

    case BCMPTM_HA_SUBID_WAL_MSG:
        submod_name = "ptmWalMsg";
        break;

    case BCMPTM_HA_SUBID_WAL_OPS_INFO:
        submod_name = "ptmWalOps";
        break;

    case BCMPTM_HA_SUBID_UNDO_OPS_INFO:
        submod_name = "ptmWalUndoOps";
        break;

    case BCMPTM_HA_SUBID_UNDO_WORDS_BUF:
        submod_name = "ptmWalUndoWords";
        break;

    case BCMPTM_HA_SUBID_WAL_WSTATE:
        submod_name = "ptmWalState";
        break;

    case BCMPTM_HA_SUBID_WAL_WORDS_BUF:
        submod_name = "ptmWalWords";
        break;

    default:
        submod_name = "ptmWalError";
        break;
    }

    /* To know size of blk that was allocated, supply incorrect size.
     * Returned value is correct size. */
    req_size = 1;
    alloc_size = req_size;
    alloc_p = shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                               submod_id,
                               submod_name,
                               &alloc_size);
    SHR_NULL_CHECK(alloc_p, SHR_E_INTERNAL);
    SHR_IF_ERR_EXIT(alloc_size <= req_size ? SHR_E_INTERNAL : SHR_E_NONE);

    /* Note:
     * ref_ha_p points to 2nd uint32_t word (one AFTER ref_sign_word)
     * alloc_p points to 1st uint32_t */
    SHR_IF_ERR_EXIT((alloc_p + 1) != ref_ha_p ?
                    SHR_E_INTERNAL : SHR_E_NONE);

    /* Clear all bytes except the ref_sign_word. */
    sal_memset(ref_ha_p, 0, (alloc_size - 4));
exit:
    SHR_FUNC_EXIT();
}

static int
wal_free(int u)
{
    SHR_FUNC_ENTER(u);

    /* Free non-HA mems */
    if (bcmptm_wal_msg_dq_ready_bp[u] != NULL) {
        sal_free(bcmptm_wal_msg_dq_ready_bp[u]);
        bcmptm_wal_msg_dq_ready_bp[u] = NULL;
    }

    /* If we are using (regular HA mem) || (crash proof DMA mem)
     *  - Do not free mem. Application will do that.
     * If we are using regular DMA mem, we must release it during cleanup. */
    if ((words_buf_mem_type[u] == WAL_MEM_TYPE_DMA) &&
        bcmptm_wal_words_buf[u] && bcmptm_wal_words_buf_bpa[u]) {
        size_t req_size;
        uint64_t pa;
        req_size = cfg_wal_words_buf_max_count[u] * sizeof(uint32_t);
        req_size += (4 * sizeof(uint32_t));
        pa = (bcmptm_wal_words_buf_bpa[u] - 16);
        bcmdrd_hal_dma_free(u, req_size, bcmptm_wal_words_buf[u] - 4, pa);
    }

    if (words_buf_mem_type[u] == WAL_MEM_TYPE_HA) {
         /* Must clear regular HA mem. */
        SHR_IF_ERR_EXIT(
            wal_ha_mem_clear(u, BCMPTM_HA_SUBID_WAL_WORDS_BUF,
                             bcmptm_wal_words_buf[u] - 3));
            /* word_0 stores sign_word
             * word_1,2,3 were skipped
             * bcmptm_wal_words_buf points to word_4
             * Above func expects p to word_1
             */
    }
    bcmptm_wal_words_buf[u] = NULL;
    bcmptm_wal_words_buf_bpa[u] = 0;
    words_buf_mem_type[u] = WAL_MEM_TYPE_NONE;

    /* Clear HA mems (if they were allocated). */
    SHR_IF_ERR_EXIT(
        wal_ha_mem_clear(u, BCMPTM_HA_SUBID_WAL_TRANS,
                         (uint32_t *)wal_trans[u]));
    SHR_IF_ERR_EXIT(
        wal_ha_mem_clear(u, BCMPTM_HA_SUBID_WAL_MSG,
                         (uint32_t *)bcmptm_wal_msg_bp[u]));
    SHR_IF_ERR_EXIT(
        wal_ha_mem_clear(u, BCMPTM_HA_SUBID_WAL_OPS_INFO,
                         (uint32_t *)bcmptm_wal_ops_info[u]));
    SHR_IF_ERR_EXIT(
        wal_ha_mem_clear(u, BCMPTM_HA_SUBID_UNDO_OPS_INFO,
                         (uint32_t *)undo_ops_info[u]));
    SHR_IF_ERR_EXIT(
        wal_ha_mem_clear(u, BCMPTM_HA_SUBID_UNDO_WORDS_BUF,
                         (uint32_t *)undo_words_buf[u]));
    SHR_IF_ERR_EXIT(
        wal_ha_mem_clear(u, BCMPTM_HA_SUBID_WAL_WSTATE,
                         (uint32_t *)wstate[u]));

    /* Prep the WAL state for potential WB */
    wstate_init(u);

    /* Do not free HA mems. Application will do that. */
    wal_trans[u] = NULL;
    bcmptm_wal_msg_bp[u] = NULL;
    bcmptm_wal_ops_info[u] = NULL;
    undo_ops_info[u] = NULL;
    undo_words_buf[u] = NULL;
    wstate[u] = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
wal_do_ha_alloc(int u, bool warm, uint32_t in_req_size,
                const char *submod_name, uint8_t submod_id,
                uint32_t ref_sign, uint32_t **alloc_p)
{
    uint32_t req_size, alloc_size, obs_sign, realloc_size, *realloc_p;
    SHR_FUNC_ENTER(u);
    *alloc_p = NULL;
    req_size = in_req_size + sizeof(uint32_t); /* 1 extra word for signature */
    alloc_size = req_size;
    *alloc_p = shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                submod_id,
                                submod_name,
                                &alloc_size);
    SHR_NULL_CHECK(*alloc_p, SHR_E_MEMORY);
    if (warm) {
        obs_sign = **alloc_p;
        SHR_IF_ERR_MSG_EXIT(
            obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(u, "WB: %s: signature mismatch, "
                        "exp=0x%8x, obs=0x%8x\n"),
             submod_name, ref_sign, obs_sign));
        if (alloc_size < req_size) {
            /* This cannot be warm-crash or warm-vacation case.
             * We can be here only for case of warm-issu and size of wal
             * data-struct has increased. Reasons for size increase:
             *  - we added another var to wal data-struct
             *  - var of type bool changed
             *  - packing mode change resulted in change of wal data-struct size
             */
            realloc_size = req_size;
            realloc_p = shr_ha_mem_realloc(u, *alloc_p, /* p to old block */
                                           realloc_size);
            SHR_NULL_CHECK(realloc_p, SHR_E_MEMORY);
            /* Data from old HA block is copied to new HA block by realloc func.
             * New piece of mem that was added by realloc, however, is not
             * initialized.
             *
             * In case of WAL, we want entire block of mem to be zeroed out.
             */
            sal_memset(realloc_p, 0, realloc_size);
            *realloc_p = ref_sign;
            *alloc_p = realloc_p;
        } /* else {
             We can be here for warm-crash or warm-issu.
             Must not do mem clear for case of warm-crash.
        } */
    } else { /* coldboot */
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        sal_memset(*alloc_p, 0, alloc_size);
        **alloc_p = ref_sign;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "%s: req bytes = %0u, alloc bytes = %0u, \n"),
         submod_name, req_size, alloc_size));
exit:
    SHR_FUNC_EXIT();
}

static int
wal_alloc(int u, bool warm)
{
    uint32_t req_size, alloc_size, ref_sign, obs_sign, *alloc_p;
    uint64_t pa, *alloc_ptr64;
    SHR_FUNC_ENTER(u);

    /* Always allocate mem assuming we will support atomic_trans */

    /* WAL_TRANS */
    req_size = cfg_wal_trans_max_count[u] * sizeof(wal_trans_t);
    SHR_IF_ERR_EXIT(
        wal_do_ha_alloc(u, warm, req_size, "ptmWalTrans",
                        BCMPTM_HA_SUBID_WAL_TRANS, BCMPTM_HA_SIGN_WAL_TRANS,
                        &alloc_p));
    wal_trans[u] = (wal_trans_t *)(alloc_p + 1);

    /* WAL_MSG */
    req_size = cfg_wal_msg_max_count[u] * sizeof(bcmptm_wal_msg_t);
    SHR_IF_ERR_EXIT(
        wal_do_ha_alloc(u, warm, req_size, "ptmWalMsg",
                        BCMPTM_HA_SUBID_WAL_MSG, BCMPTM_HA_SIGN_WAL_MSG,
                        &alloc_p));
    bcmptm_wal_msg_bp[u] = (bcmptm_wal_msg_t *)(alloc_p + 1);

    /* WAL_MSG_DQ_READY - Non_HA, Non_DMA mem (intentionally).
     * Will always 0 out contents (even for WB/ISSU/crash).
     * Hence no signature. */
    req_size = cfg_wal_msg_max_count[u] * sizeof(bool);
    alloc_size = req_size;
    bcmptm_wal_msg_dq_ready_bp[u] = sal_alloc(req_size, "bcmptmWalMsgDqReady");
    SHR_NULL_CHECK(bcmptm_wal_msg_dq_ready_bp[u], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    sal_memset(bcmptm_wal_msg_dq_ready_bp[u], 0, alloc_size);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "WAL_MSG_DQ_READY: req bytes = %0u, alloc bytes = %0u, \n"),
         req_size, alloc_size));

    /* WAL_OPS_INFO */
    req_size = cfg_wal_ops_info_max_count[u] * sizeof(bcmptm_wal_ops_info_t);
    SHR_IF_ERR_EXIT(
        wal_do_ha_alloc(u, warm, req_size, "ptmWalOps",
                        BCMPTM_HA_SUBID_WAL_OPS_INFO,
                        BCMPTM_HA_SIGN_WAL_OPS_INFO, &alloc_p));
    bcmptm_wal_ops_info[u] = (bcmptm_wal_ops_info_t *)(alloc_p + 1);

    /* UNDO_OPS_INFO */
    req_size = cfg_undo_ops_info_max_count[u] * sizeof(wal_undo_ops_t);
    SHR_IF_ERR_EXIT(
        wal_do_ha_alloc(u, warm, req_size, "ptmWalUndoOps",
                        BCMPTM_HA_SUBID_UNDO_OPS_INFO,
                        BCMPTM_HA_SIGN_WAL_UNDO_OPS_INFO, &alloc_p));
    undo_ops_info[u] = (wal_undo_ops_t *)(alloc_p + 1);

    /* UNDO_WORDS_BUF */
    req_size = cfg_undo_words_buf_max_count[u] * sizeof(uint32_t);
    SHR_IF_ERR_EXIT(
        wal_do_ha_alloc(u, warm, req_size, "ptmWalUndoWords",
                        BCMPTM_HA_SUBID_UNDO_WORDS_BUF,
                        BCMPTM_HA_SIGN_WAL_UNDO_WORDS_BUF, &alloc_p));
    undo_words_buf[u] = alloc_p + 1;

    /* wstate */
    req_size = sizeof(wstate_t);
    SHR_IF_ERR_EXIT(
        wal_do_ha_alloc(u, warm, req_size, "ptmWalState",
                        BCMPTM_HA_SUBID_WAL_WSTATE, BCMPTM_HA_SIGN_WAL_WSTATE,
                        &alloc_p));
    wstate[u] = (wstate_t *)(alloc_p + 1);


    /* WAL_WORDS_BUF - HA_DMAable crash-proof mem */
    req_size = cfg_wal_words_buf_max_count[u] * sizeof(uint32_t);
    req_size += (4 * sizeof(uint32_t));
                /* Note: We want bcmptm_wal_words_buf on 16 Byte boundary.
                 *       So waste first 16 Bytes to store:
                 *       4 Byte signature, 4 Byte unused,
                 *       8 Byte wal_words_buf_bpa */
    alloc_size = req_size;
    alloc_p = NULL;
    if (bcmptm_wal_scf_num_chans[u] || dma_avail[u]) {
        size_t tmp_alloc_size = 0;
        alloc_p = bcmdrd_dev_ha_dma_mem_get(u, &tmp_alloc_size, &pa);
        if (alloc_p && tmp_alloc_size >= req_size) {
            words_buf_mem_type[u] = WAL_MEM_TYPE_DMA_CRASH_PROOF;
            alloc_size = tmp_alloc_size;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "WAL_WORDS_BUF: got crash-proof DMA mem. "
                            "req bytes = %0u, alloc bytes = %0u, \n"),
                 req_size, alloc_size));

        } else { /* use regular DMA mem */
            alloc_p = NULL;
            pa = 0;
            words_buf_mem_type[u] = WAL_MEM_TYPE_DMA;
            alloc_p = bcmdrd_hal_dma_alloc(u, req_size, "ptmWalWords", &pa);
            alloc_size = req_size; /* assume alloc_p is not NULL (we got the mem) */
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "WAL_WORDS_BUF: Did not get crash-proof DMA "
                            "mem. Will try getting standard DMA mem\n")));
        } /* use regular DMA mem */
        SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(!pa ? SHR_E_MEMORY : SHR_E_NONE);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        bcmptm_wal_words_buf[u] = alloc_p + 4; /* Skip first 16 Bytes */
        ref_sign = BCMPTM_HA_SIGN_WAL_WORDS_BUF;
        alloc_ptr64 = (uint64_t *)(alloc_p + 2);
        if (warm &&
            (words_buf_mem_type[u] == WAL_MEM_TYPE_DMA_CRASH_PROOF)) {
            bool same_dma_mem = TRUE;
            /* Note: For WARM
             *       call hal_dma_alloc();
             *       stored prev_wal_words_buf_pa must match pa;
             *       do bcmdrd_hal_dma_alloc();
             *       assert(prev_wal_words_buf_pa == pa);
             *       Main point:
             *       alloc_p will change but physical address and
             *       contents (signature) of wal_words_buf should be same
             *       (preserved).
             */

            obs_sign = *alloc_p;
            if (obs_sign != ref_sign) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "WB: WAL.words_buf array signature "
                                "mismatch, exp=0x%8x, obs=0x%8x\n"),
                     ref_sign, obs_sign));
                same_dma_mem = FALSE;
            }
            if (*alloc_ptr64 != (pa + 16)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "WB: WAL.words_buf array physical_addr "
                                "mismatch, exp= %" PRIx64 "obs= %" PRIx64 "\n"),
                     *alloc_ptr64, pa + 16));
                same_dma_mem = FALSE;
            }

            if (same_dma_mem) {
                /* Re-init wal_words_buf_bpa */
                bcmptm_wal_words_buf_bpa[u] = pa + 16; /* Skip first 16 Bytes */
            } else {
                /* Did not get old DMAable mem - so cannot support crash-recovery.
                 * For now treat this as WB case - where we expect WAL to be
                 * empty.
                 */
                
                sal_memset(alloc_p, 0, alloc_size);
                bcmptm_wal_words_buf_bpa[u] = pa + 16; /* Skip first 16 Bytes */
                *alloc_ptr64 = bcmptm_wal_words_buf_bpa[u];
                *alloc_p = ref_sign;
            }
        } else { /* init ref_sign */
            sal_memset(alloc_p, 0, alloc_size);
            bcmptm_wal_words_buf_bpa[u] = pa + 16; /* Skip first 16 Bytes */
            *alloc_ptr64 = bcmptm_wal_words_buf_bpa[u];
            *alloc_p = ref_sign;
        }
    } else { /* native_sim, no DMA, no schan_fifo */
        words_buf_mem_type[u] = WAL_MEM_TYPE_HA;
        alloc_p = shr_ha_mem_alloc(u, BCMMGMT_PTM_COMP_ID,
                                   BCMPTM_HA_SUBID_WAL_WORDS_BUF,
                                   "ptmWalWords",
                                   &alloc_size);
        SHR_NULL_CHECK(alloc_p, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
        ref_sign = BCMPTM_HA_SIGN_WAL_WORDS_BUF;
        if (warm) {
            obs_sign = *alloc_p;
            SHR_IF_ERR_MSG_EXIT(
                obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
                (BSL_META_U(u, "WB: WAL.words_buf array signature mismatch, "
                            "exp=0x%8x, obs=0x%8x\n"),
                 ref_sign, obs_sign));
            bcmptm_wal_words_buf_bpa[u] = 0; /* implies no DMA mem */
            if (alloc_size < req_size) {
                uint32_t realloc_size, *realloc_p;
                /* This cannot be warm-crash or warm-vacation case.
                 * We can be here only for case of warm-issu and size of wal
                 * data-struct has increased. Reasons for size increase:
                 *  - we added another var to wal data-struct
                 *  - var of type bool changed
                 *  - packing mode change resulted in change of wal data-struct size
                 */
                realloc_size = req_size;
                realloc_p = shr_ha_mem_realloc(u, alloc_p, /* p to old block */
                                               realloc_size);
                SHR_NULL_CHECK(realloc_p, SHR_E_MEMORY);
                /* Data from old HA block is copied to new HA block by realloc func.
                 * New piece of mem that was added by realloc, however, is not
                 * initialized.
                 *
                 * In case of WAL, we want entire block of mem to be zeroed out.
                 */
                sal_memset(realloc_p, 0, realloc_size);
                *realloc_p = ref_sign;
                alloc_p = realloc_p;
            } /* else {
                 We can be here for warm-crash or warm-issu.
                 Must not do mem clear for case of warm-crash.
            } */
        } else { /* init ref_sign */
            sal_memset(alloc_p, 0, alloc_size);
            bcmptm_wal_words_buf_bpa[u] = 0; /* implies no DMA mem */
            *alloc_p = ref_sign;
        }
        bcmptm_wal_words_buf[u] = alloc_p + 4; /* Skip first 16 Bytes */
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "WAL_WORDS_BUF: req bytes = %0u, alloc bytes = %0u, \n"),
         req_size, alloc_size));

exit:
    if (SHR_FUNC_ERR()) {
        wal_free(u);
        SHR_FUNC_EXIT();
    }

    req_size = 0;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof wal_trans_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(wal_trans_t), cfg_wal_trans_max_count[u]));
    req_size += (uint32_t)sizeof(wal_trans_t) *  cfg_wal_trans_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof bcmptm_wal_msg_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(bcmptm_wal_msg_t), cfg_wal_msg_max_count[u]));
    req_size += (uint32_t)sizeof(bcmptm_wal_msg_t) * cfg_wal_msg_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof bcmptm_wal_dq_ready_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(bool), cfg_wal_msg_max_count[u]));
    req_size += (uint32_t)sizeof(bool) * cfg_wal_msg_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof bcmptm_wal_ops_info_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(bcmptm_wal_ops_info_t), cfg_wal_ops_info_max_count[u]));
    req_size += (uint32_t)sizeof(bcmptm_wal_ops_info_t) * cfg_wal_ops_info_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof wal_undo_ops_t = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(wal_undo_ops_t), cfg_undo_ops_info_max_count[u]));
    req_size += (uint32_t)sizeof(wal_undo_ops_t) * cfg_undo_ops_info_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof undo_words = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(uint32_t), cfg_undo_words_buf_max_count[u]));
    req_size += (uint32_t)sizeof(uint32_t) * cfg_undo_words_buf_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "sizeof wal_words = %0u Bytes, count = %0d\n"),
         (uint32_t) sizeof(uint32_t), cfg_wal_words_buf_max_count[u]));
    req_size += (uint32_t)sizeof(uint32_t) * cfg_wal_words_buf_max_count[u];

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "Total WAL size = %0d Bytes\n"), req_size));

    SHR_FUNC_EXIT();
}

/*
 * - There is no change in wstate.
 *
 * - Never returns ERROR.
 *
 * - Can be called with non-zero num_words_skipped (by align_skip())
 *
 * - if zero_out_words_n_buf_idx is returned TRUE, caller must:
 *   check if avail_words_buf_count is at least:
 *          (num_words_skipped + num_words_req)
 *   and then set:
 *          wstate[u]->n_words_buf_idx) = 0;
 *          wstate[u]->w_words_buf_count) += num_words_skipped;
 */
static int
avail_words_skip(int u,
                 uint32_t n_words_buf_idx,
                 uint32_t num_words_req,
                 bool *zero_out_n_words_buf_idx,
                 uint32_t *num_words_skipped)
{
    uint32_t num_words_till_max_idx;
    bool words_fail = FALSE;
    SHR_FUNC_ENTER(u);

    num_words_till_max_idx = (cfg_wal_words_buf_max_count[u] -
                              n_words_buf_idx);
    words_fail = (num_words_till_max_idx < num_words_req);

    if (words_fail) {
        *zero_out_n_words_buf_idx = TRUE;
        *num_words_skipped += num_words_till_max_idx;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Coverage: zero_out_n_words_buf_idx due to "
                        "words boundary_wrap, total_skipped_words = %0u \n"),
                        *num_words_skipped));
    }
/* exit: */
    SHR_FUNC_EXIT();
}

/*
 * This function is called when we start new msg and
 *      - zero_out_n_words_buf_idx = FALSE
 *      - num_words_skipped = 0
 *
 * It first computes skip needed for 16 Byte alignment of n_words_buf_idx
 * and then computes skip needed for avoiding word_boundary_wraparound.
 *
 * Assumes: cfg_wal_words_buf_max_count is multiple of 4.
 *
 * Does not change wstate
 *
 * Cannot return ERROR as avail_words_skip() does not return ERROR.
 *
 * Can exit with:
 *  - case_A1: Alignment not needed, also no words_boundary_wrap condition
 *      - zero_out_n_words_buf_idx = FALSE
 *      - num_words_skipped = 0
 *
 *  - case_A2: Alignment not needed, but words_boundary_wrap condition
 *      - zero_out_n_words_buf_idx = TRUE
 *      - num_words_skipped = !0 = boundary_skip_value
 *
 *  - case_B1: Alignment change needed, but no words_boundary_wrap condition
 *      - zero_out_n_words_buf_idx = FALSE
 *      - num_words_skipped = !0 = align_skip_value
 *
 *  - case_B2: Alignment change needed, also words_boundary_wrap condition
 *      - zero_out_n_words_buf_idx = TRUE
 *      - num_words_skipped = !0 = sum(align_skip_value, boundary_skip_value)
 *
 * Future:
 *       Once WAL writer does skip, WAL reader cannot mix ops of consecutive
 *       msgs into cmd_mem of same channel - because now we have gaps in
 *       words_buf. This matters only when we try to enhance WAL reader to
 *       concatenate ops from consecutive messages.
 */
static int
align_skip(int u,
           uint32_t num_words_req,
           bool *zero_out_n_words_buf_idx,
           uint32_t *num_words_skipped)
{
    uint8_t lsb, inc_val;
    uint32_t tmp_n_words_buf_idx;
    SHR_FUNC_ENTER(u);

    lsb = wstate[u]->n_words_buf_idx & 0x3;
    if (lsb) {
        inc_val = 4 - lsb; /* quad-word alignment */
        *num_words_skipped = inc_val;

        tmp_n_words_buf_idx = wstate[u]->n_words_buf_idx + inc_val;
        SHR_IF_ERR_EXIT(
            (tmp_n_words_buf_idx > cfg_wal_words_buf_max_count[u]) ?
            SHR_E_INTERNAL : SHR_E_NONE);

        /* It is possible for above increment to cause tmp_n_words_buf_idx to
         * become = cfg_wal_words_buf_max_count[u]. For example:
         * max_count=20, max_buf_idx=19, req_entry_words=6
         *
         * If buf_idx was 17, 18, 19 lsb is 1,2,3 and inc_val is 3,2,1
         * so buf_idx + inc_val = 20 = cfg_wal_words_max_count
         *
         * In such case, we need to respond with:
         *      - zero_out_n_words_buf_idx = TRUE
         *      - num_words_skipped = inc_val
         *  And this is exactly what will happen once we call avail_words_skip
         *  below (trace code for avail_words_skip API).
         */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Coverage: align_skip: skipped_words = %0u\n"),
             *num_words_skipped));
    } else {
        /* n_words_buf_idx is already aligned, no adjustment needed */
        *num_words_skipped = 0;
        tmp_n_words_buf_idx = wstate[u]->n_words_buf_idx;
    }

    /* Now we are aligned, but may not have sufficient words till wraparound
     * boundary.
     *
     * Example:
     * max_count=20, max_buf_idx=19, req_entry_words=6, avail_count=20
     *
     * n_buf_idx was 15 when align_skip() is called.
     * lsb=3, inc_val=1, new n_buf_idx=16,
     * and num_words_till_max_idx = 20 - 16 = 4, but this is less than
     * req_entry_words(6), so we cannot start msg at n_buf_idx=16 because this
     * would mean 6 words will include boundary_wrap. Hence we need to do
     * wraparound_skip AFTER align_skip.
     */
    SHR_IF_ERR_EXIT(
        avail_words_skip(u, tmp_n_words_buf_idx, /* aligned */
                         num_words_req,
                         zero_out_n_words_buf_idx,
                         num_words_skipped));
exit:
    SHR_FUNC_EXIT();
}

/* Caller guarantees that:
 * 1. avail words is >= num_words_req
 * 2. we dont have 'max_boundary' in avail words space */
static int
wstate_words_buf_idx_inc(int u, uint32_t num_words_req,
                         bool *close_msg_after_this_op)
{
    uint32_t num_words_till_max_idx;
    SHR_FUNC_ENTER(u);

    num_words_till_max_idx =
        (cfg_wal_words_buf_max_count[u] - wstate[u]->n_words_buf_idx);

    SHR_IF_ERR_EXIT(
        (num_words_till_max_idx < num_words_req) ? SHR_E_INTERNAL : SHR_E_NONE);

    if (num_words_till_max_idx == num_words_req) {
        wstate[u]->n_words_buf_idx = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        /* num_words_till_max_id > num_words_req so ok to inc buf_idx */
        wstate[u]->n_words_buf_idx += num_words_req;
        /* *close_msg_after_this_op = FALSE; Intentionally commented because
         * wstate_wal_ops_info_idx_inc() can also set it */
    }
exit:
    SHR_FUNC_EXIT();
}

/* Caller guarantees that:
 * 1. avail words is >= num_words_req
 * 2. we dont have 'max_boundary' in avail words space */
static int
wstate_undo_words_buf_idx_inc(int u, uint32_t num_undo_words_req,
                              bool *close_msg_after_this_op)
{
    uint32_t num_undo_words_till_max_idx;
    SHR_FUNC_ENTER(u);

    num_undo_words_till_max_idx =
        (cfg_undo_words_buf_max_count[u] - wstate[u]->n_undo_words_buf_idx);

    SHR_IF_ERR_EXIT(
        (num_undo_words_till_max_idx < num_undo_words_req) ? SHR_E_INTERNAL
                                                           : SHR_E_NONE);

    if (num_undo_words_till_max_idx == num_undo_words_req) {
        wstate[u]->n_undo_words_buf_idx = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        /* num_words_till_max_id > num_words_req so ok to inc buf_idx */
        wstate[u]->n_undo_words_buf_idx += num_undo_words_req;
        /* *close_msg_after_this_op = FALSE; Intentionally commented because
         * wstate_wal_ops_info_idx_inc() can also set it */
    }
exit:
    SHR_FUNC_EXIT();
}

static int
wstate_trans_idx_inc(int u)
{
    if (wstate[u]->n_trans_idx == cfg_wal_trans_idx_max[u]) {
        wstate[u]->n_trans_idx = 0;
    } else {
        wstate[u]->n_trans_idx += 1;
    }
    return SHR_E_NONE;
}

static int
wstate_msg_idx_inc(int u)
{
    if (wstate[u]->n_msg_idx == cfg_wal_msg_idx_max[u]) {
        wstate[u]->n_msg_idx = 0;
    } else {
        wstate[u]->n_msg_idx += 1;
    }
    return SHR_E_NONE;
}

static int
wstate_wal_ops_info_idx_inc(int u, bool *close_msg_after_this_op)
{
    if (wstate[u]->n_wal_ops_info_idx == cfg_wal_ops_info_idx_max[u]) {
        wstate[u]->n_wal_ops_info_idx = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        wstate[u]->n_wal_ops_info_idx += 1;
        /* *close_msg_after_this_op = FALSE;
         * Intentionally commented because wstate_undo_ops_info_idx_inc()
         * can also set it */
    }
    return SHR_E_NONE;
}

static int
wstate_undo_ops_info_idx_inc(int u, bool *close_msg_after_this_op)
{
    if (wstate[u]->n_undo_ops_info_idx == cfg_undo_ops_info_idx_max[u]) {
        wstate[u]->n_undo_ops_info_idx = 0;
        *close_msg_after_this_op = TRUE;
    } else {
        wstate[u]->n_undo_ops_info_idx += 1;
        /* *close_msg_after_this_op = FALSE; Intentionally commented because
         * wstate_wal_ops_info_idx_inc() can also set it */
    }
    return SHR_E_NONE;
}

static int
wstate_rdr_msg_idx_inc(int u)
{
    if (wstate[u]->n_rdr_msg_idx == cfg_wal_msg_idx_max[u]) {
        wstate[u]->n_rdr_msg_idx = 0;
    } else {
        wstate[u]->n_rdr_msg_idx += 1;
    }
    return SHR_E_NONE;
}

/* Will be called by SCF logic before ops for wal_msg are sent to HW.
 * Must be called only once per wal_msg.
 * Will execute in WAL reader context.
 *
 * For case when emc_group_id == smc_group_id, we will
 * take mutex once and inc count twice.
 */
static int
wal_rdr_pre_msg(int u, bcmptm_wal_msg_t *walr_msg)
{
    int tmp_rv;
    bcmptm_wal_slice_mode_info_t smc_word;
    sal_mutex_t smc_mutex, emc_mutex;
    uint32_t smc_group_id = 0, emc_group_id = 0;
    SHR_FUNC_ENTER(u);
    smc_word = walr_msg->smc_word;
    SHR_IF_ERR_EXIT((!smc_word.f.slice_change_en &&
                     !smc_word.f.entry_change_en) ? SHR_E_INTERNAL
                                                  : SHR_E_NONE);
    smc_group_id = smc_word.f.slice_group_id;
    if (smc_word.f.slice_change_en) {
        if (mc_count[u][smc_group_id] == 0) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_slice_mode_mutex_info(u, smc_group_id, &smc_mutex));
            tmp_rv = bcmptm_mutex_take(u, smc_mutex,
                                       ENTRY_MODE_LOCK_WAIT_USEC);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "!! Could not take slice_mode_mutex \n")));
            }
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        mc_count[u][smc_group_id] += 1;
    }
    if (smc_word.f.entry_change_en) {
        SHR_IF_ERR_EXIT(
            bcmptm_ser_entry_mode_mutex_info(u, &emc_group_id, &emc_mutex));
        if (mc_count[u][emc_group_id] == 0) {
            tmp_rv = bcmptm_mutex_take(u, emc_mutex,
                                       ENTRY_MODE_LOCK_WAIT_USEC);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "!! Could not take entry_mode_mutex \n")));
            }
            SHR_IF_ERR_EXIT(tmp_rv);
        }
        mc_count[u][emc_group_id] += 1;
    }
exit:
    /* Only error could be in:
     * 1. retrieving group_id- in this case we don't take mutex and did not inc
     *    count
     * 2. taking mutex - in this case we did not inc count
     */
    SHR_FUNC_EXIT();
}

/* Will be called by WAL reader for every attempt of sending wal_msg.
 * Will execute in WAL reader context.
 *
 * For case when emc_group_id == smc_group_id, we will
 * give mutex once and dec count twice.
 */
static int
wal_rdr_post_msg(int u, bcmptm_wal_msg_t *walr_msg,
                 uint32_t num_ops_sent, bool msg_done)
{
    int tmp_rv;
    bcmptm_wal_slice_mode_info_t smc_word;
    sal_mutex_t smc_mutex, emc_mutex;
    uint32_t smc_group_id = 0, emc_group_id = 0;
    SHR_FUNC_ENTER(u);
    smc_word = walr_msg->smc_word;
    smc_group_id = smc_word.f.slice_group_id;
    /* ser slice_mode update happens only when all ops in msg are sent to HW */
    if (msg_done && smc_word.f.slice_change_en) {
        bcmptm_rm_slice_change_t rm_slice_info;
        rm_slice_info.slice_num0 =
            (smc_word.f.slice_num0 > SLICE_MAX ? -1 : smc_word.f.slice_num0);
        rm_slice_info.slice_num1 =
            (smc_word.f.slice_num1 > SLICE_MAX ? -1 : smc_word.f.slice_num1);
        rm_slice_info.slice_mode = smc_word.f.slice_mode;
        rm_slice_info.slice_group_id = smc_word.f.slice_group_id;
        SHR_IF_ERR_EXIT(bcmptm_ser_slice_mode_change(u, &rm_slice_info));
        if (mc_count[u][smc_group_id] == 1) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_slice_mode_mutex_info(u, smc_group_id, &smc_mutex));
            tmp_rv = sal_mutex_give(smc_mutex);
            if (tmp_rv != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "!! Could not give slice_mode_mutex \n")));
            }
            SHR_IF_ERR_EXIT(tmp_rv);
            mc_count[u][smc_group_id] = 0;
        } else {
            mc_count[u][smc_group_id] -= 1;
        }
    }
    /* Because ops are released for every partial msg attempt,
     * ser entry_mode updates must happen for every msg attempt.
     *
     * Release of emc_mutex and counts, however, must be done ONLY once when
     * msg is done.
     */
    if (smc_word.f.entry_change_en) {
        uint32_t i;
        bcmptm_rm_entry_mode_change_t emc_info;
        bcmptm_wal_ops_info_t *ops_info_array =
            bcmptm_wal_ops_info[u] + walr_msg->ops_info_idx;
        for (i = 0; i < num_ops_sent; i++) {
            if (ops_info_array->op_type == BCMPTM_RM_OP_EM_WIDE ||
                ops_info_array->op_type == BCMPTM_RM_OP_EM_NARROW) {
                emc_info.index = (ops_info_array->dyn_info).index;
                emc_info.op_type = ops_info_array->op_type;
                SHR_IF_ERR_EXIT(bcmptm_ser_entry_mode_change(u, &emc_info));
            }
            ops_info_array++;
        }
        if (msg_done) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_entry_mode_mutex_info(u, &emc_group_id, &emc_mutex));
            if (mc_count[u][emc_group_id] == 1) {
                tmp_rv = sal_mutex_give(emc_mutex);
                if (tmp_rv != SHR_E_NONE) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "!! Could not give entry_mode_mutex \n")));
                }
                SHR_IF_ERR_EXIT(tmp_rv);
                mc_count[u][emc_group_id] = 0;
            } else {
                mc_count[u][emc_group_id] -= 1;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
undo_info_get(int u, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *pt_dyn_info,
              uint32_t entry_count, uint32_t undo_entry_buf_wsize,

              /* Outputs */
              uint32_t *undo_entry_words_p,
              wal_undo_ops_t *undo_ops_info_p)
{
    SHR_FUNC_ENTER(u);
    sal_memset(undo_ops_info_p, 0, sizeof(wal_undo_ops_t));
    undo_ops_info_p->undo_sid = sid;
    undo_ops_info_p->undo_dyn_info = *pt_dyn_info;
    SHR_IF_ERR_EXIT(
        bcmptm_ptcache_read(u, ACC_FOR_SER, 0, sid, pt_dyn_info,
                            entry_count,
                            undo_entry_buf_wsize,

                            undo_entry_words_p,
                            &(undo_ops_info_p->undo_cache_vbit),
                            &(undo_ops_info_p->undo_dfid),
                            &(undo_ops_info_p->undo_ltid)));
exit:
    SHR_FUNC_EXIT();
}

/* Set dq_ready for all msgs of a transaction */
static int
wal_send_all_msgs(int u, wal_trans_t *trans_p, bool resend)
{
    uint32_t i, c_msg_idx, c_trans_idx;
    bcmptm_wal_msg_t *c_wal_msg_p;
    bool *c_wal_msg_dq_ready_p;
    uint32_t c_num_msgs, ref_t_num_msgs;
    SHR_FUNC_ENTER(u);

    /* Lock is needed if we are using any var that can be changed by
     * wal_rdr_msg_done() */
    /* SHR_IF_ERR_EXIT(wal_wstate_lock(u)); */

    c_msg_idx = trans_p->f_msg_idx;
    /* But for wal_disable_undo mode, we could have non-zero c_msg_commit_count */
    if (wstate[u]->c_msg_commit_count) {
        for (i = 0; i < wstate[u]->c_msg_commit_count; i++) {
            if (c_msg_idx == cfg_wal_msg_idx_max[u]) {
                c_msg_idx = 0;
            } else {
                c_msg_idx++;
            }
        }
    }
    c_wal_msg_p = bcmptm_wal_msg_bp[u] + c_msg_idx;
    c_wal_msg_dq_ready_p = bcmptm_wal_msg_dq_ready_bp[u] + c_msg_idx;

    c_trans_idx = c_wal_msg_p->trans_idx;
    c_num_msgs = trans_p->num_msgs - wstate[u]->c_msg_commit_count;
    ref_t_num_msgs = trans_p->num_msgs; /* for debug print msgs */

    /* No msgs to commit
     *  - may happen when disable_undo is asserted.
     *
     *  - may also happen if this func was called from bcmptm_wal_read and
     *    trans_state was NEED_NEW_MSG, but all msgs were already sent as
     *    disable_undo was asserted.
     */

    print_trans_info(u, FALSE, c_trans_idx, trans_p, "WCM");

    /* From this point on, we must not reference contents of wal_msg_p,
     * trans_p except for debug. Because the moment we set dq_ready for first
     * msg in trans, wal_rdr can change wal_msg, trans */

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "Will commit %0u msgs for trans_idx = %0u \n"),
                    c_num_msgs, c_trans_idx));

    /* Enable wal rdr to dequeue msgs */
    if (!wal_msg_dq_enable[u]) {
        /* Verify WAL reader thread is in RUN state. */
        SHR_IF_ERR_EXIT(bcmptm_walr_run_check(u));

        /* Initialize dq_idx for WAL reader. */
        bcmptm_wal_msg_dq_idx[u] = c_msg_idx;
        bcmptm_wal_msg_dq_p[u] = c_wal_msg_p;
        bcmptm_wal_msg_dq_ready_p[u] = bcmptm_wal_msg_dq_ready_bp[u] +
                                       c_msg_idx;
        /* Then enable dq */
        wal_msg_dq_enable[u] = TRUE;
    }

    for (i = 0; i < c_num_msgs; i++) {
        /* Must print msg before dq_ready is set - else context may switch to
         * wal_rdr and may result in release of this msg - so wal_msg members
         * will get zeroed out */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Cmt, umsg_idx, %0u, skip_words, %0u, "
                        "words_buf_idx, %0u, words_sent, %0u\n"),
             c_wal_msg_p->umsg_idx,
             c_wal_msg_p->num_words_skipped,
             c_wal_msg_p->words_buf_idx,
             c_wal_msg_p->req_entry_wsize));

        SHR_IF_ERR_EXIT(*c_wal_msg_dq_ready_p ? SHR_E_INTERNAL : SHR_E_NONE);
        if (resend) {
            SHR_IF_ERR_EXIT(!c_wal_msg_p->committed ?
                            SHR_E_INTERNAL : SHR_E_NONE);
        } else {
            /* 'last_msg' MUST be set before setting wal_msg_dq_ready */
            c_wal_msg_p->last_msg = (i == (c_num_msgs - 1)) ? TRUE : FALSE;

            SHR_IF_ERR_EXIT(c_wal_msg_p->committed ?
                            SHR_E_INTERNAL : SHR_E_NONE);
            c_wal_msg_p->committed = TRUE;
        }
        *c_wal_msg_dq_ready_p = TRUE;
        SHR_IF_ERR_MSG_EXIT(
            bcmptm_walr_wake(u),
            (BSL_META_U(u, "Got error when waking up WAL reader thread.\n")));
        wstate[u]->c_msg_commit_count += 1;

        /* next msg */
        if (c_msg_idx == cfg_wal_msg_idx_max[u]) {
            c_wal_msg_p = bcmptm_wal_msg_bp[u];
            c_wal_msg_dq_ready_p = bcmptm_wal_msg_dq_ready_bp[u];
            c_msg_idx = 0;
        } else {
            c_wal_msg_p++;
            c_wal_msg_dq_ready_p++;
            c_msg_idx++;
        }
    } /* for each msg */

    SHR_IF_ERR_EXIT((wstate[u]->c_msg_commit_count != ref_t_num_msgs) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Set dq_ready for msg before transaction commit.
 *
 * Normally: called from bcmptm_wal_write. Such msgs will have last_msg=False.
 * We have not yet received commit for parent transaction.
 *
 * Sometimes, called from bcmptm_wal_commit to send an empty_last_msg (which has
 * last_msg=True).
 */
static int
wal_send_msg_b4tc(int u, uint32_t c_msg_idx)
{
    bcmptm_wal_msg_t *c_wal_msg_p;
    bool *c_wal_msg_dq_ready_p;
    SHR_FUNC_ENTER(u);

    /* Lock is needed if we are using any var that can be changed by
     * wal_rdr_msg_done() */
    /* SHR_IF_ERR_EXIT(wal_wstate_lock(u)); */

    c_wal_msg_p = bcmptm_wal_msg_bp[u] + c_msg_idx;
    c_wal_msg_dq_ready_p = bcmptm_wal_msg_dq_ready_bp[u] + c_msg_idx;

    /* From this point on, we must not reference contents of wal_msg_p,
     * trans_p except for debug. Because the moment we set dq_ready for first
     * msg in trans, wal_rdr can change wal_msg, trans */

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "Will commit c_msg_idx=%0u for trans_idx = %0u \n"),
                    c_msg_idx, wstate[u]->c_trans_idx));

    /* Enable wal rdr to dequeue msgs */
    if (!wal_msg_dq_enable[u]) {
        /* Verify WAL reader thread is in RUN state. */
        SHR_IF_ERR_EXIT(bcmptm_walr_run_check(u));

        /* Initialize dq_idx for WAL reader. */
        bcmptm_wal_msg_dq_idx[u] = c_msg_idx;
        bcmptm_wal_msg_dq_p[u] = c_wal_msg_p;
        bcmptm_wal_msg_dq_ready_p[u] = c_wal_msg_dq_ready_p;
        /* Then enable dq */
        wal_msg_dq_enable[u] = TRUE;
    }

    /* Must print msg before dq_ready is set - else context may switch to
     * wal_rdr and may result in release of this msg - so wal_msg members
     * will get zeroed out */
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "Cmt, umsg_idx, %0u, skip_words, %0u, "
                    "words_buf_idx, %0u, words_sent, %0u\n"),
         c_wal_msg_p->umsg_idx,
         c_wal_msg_p->num_words_skipped,
         c_wal_msg_p->words_buf_idx,
         c_wal_msg_p->req_entry_wsize));

    SHR_IF_ERR_EXIT(
        (*c_wal_msg_dq_ready_p || c_wal_msg_p->committed) ?
        SHR_E_INTERNAL : SHR_E_NONE);
    c_wal_msg_p->committed = TRUE;
    *c_wal_msg_dq_ready_p = TRUE;
    SHR_IF_ERR_MSG_EXIT(
        bcmptm_walr_wake(u),
        (BSL_META_U(u, "Got error when waking up WAL reader thread.\n")));
    wstate[u]->c_msg_commit_count += 1;

exit:
    SHR_FUNC_EXIT();
}

/* Look for in_cseq_id in pending WAL trans.
 * If found: Re-commit. Notification will come when ops are sent to HW.
 * Else:     send immediate notification if requested.
 */
static int
wal_trans_id_unknown(int u,
                     uint32_t in_cseq_id,
                     bcmptm_trans_cb_f notify_fn_p,
                     void *notify_info)
{
    bcmptm_wal_msg_t *o_wal_msg_p;
    uint32_t o_trans_idx;
    wal_trans_t *o_trans_p;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(wal_wstate_lock(u)); /* freeze n_rdr_msg_idx */

    if (wstate[u]->n_rdr_msg_idx == wstate[u]->n_msg_idx) {
        /* WAL is empty */

        /* Assume IMM case */
        
        if (notify_fn_p) {
            notify_fn_p(in_cseq_id, SHR_E_NONE, notify_info);
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "====== Did not find matching trans_id\n")));
        SHR_EXIT(); /* Can send NOT_FOUND, if
                                              caller so desires */
    } /* WAL is empty */

    o_wal_msg_p = bcmptm_wal_msg_bp[u] + wstate[u]->n_rdr_msg_idx;
    o_trans_idx = o_wal_msg_p->trans_idx;
    o_trans_p = wal_trans[u] + o_trans_idx;

    do {
        /* No more trans in WAL */
        if (o_trans_idx == wstate[u]->n_trans_idx) {
            /* Assume IMM case */
            
            if (notify_fn_p) {
                notify_fn_p(in_cseq_id, SHR_E_NONE, notify_info);
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "====== Did not find matching trans_id\n")));
            SHR_EXIT(); /* Can send NOT_FOUND, if
                                                  caller so desires */
        }

        /* Pending trans in WAL cannot be empty */
        SHR_IF_ERR_EXIT(!o_trans_p->num_msgs ? SHR_E_INTERNAL : SHR_E_NONE);

        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "trans_idx=%0d, in_cseq_id=%0d, pending trans "
                        "cseq_id = %0d\n"),
             o_trans_idx, in_cseq_id, o_trans_p->cseq_id));

        /* Re-commit case */
        if (in_cseq_id == o_trans_p->cseq_id) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "====== Found matching trans_id\n")));

            /* re-record notify info */
            o_trans_p->notify_fn_p = notify_fn_p;
            o_trans_p->notify_info = notify_info;

            /* re-send all messages for old trans to wal rdr */
            SHR_IF_ERR_EXIT(wal_send_all_msgs(u, o_trans_p,
                                              TRUE)); /* resend */
        } /* found an old trans */

        /* Try next trans */
        if (o_trans_idx == cfg_wal_trans_idx_max[u]) {
            o_trans_idx = 0;
            o_trans_p = wal_trans[u];
        } else {
            o_trans_idx++;
            o_trans_p++;
        }
    } while (1);

exit:
    if (have_wstate_mutex[u]) {
        (void)wal_wstate_unlock(u);
    }
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
/* Called in WAL reader thread context */
uint32_t
bcmptm_wal_next_msg_idx(int u, uint32_t c_msg_idx, bool adjust_dq_p)
{
    if (c_msg_idx == cfg_wal_msg_idx_max[u]) {
        if (adjust_dq_p) {
            bcmptm_wal_msg_dq_ready_p[u] = bcmptm_wal_msg_dq_ready_bp[u];
            bcmptm_wal_msg_dq_p[u] = bcmptm_wal_msg_bp[u];
        }
        return 0;
    } else {
        if (adjust_dq_p) {
            bcmptm_wal_msg_dq_ready_p[u]++;
            bcmptm_wal_msg_dq_p[u]++;
        }
        return ++c_msg_idx;
    }
}

/* Will always be called at end of every msg sending attempt.
 *
 * Can also be called before sending of msg.
 *
 * Will execute in WAL rdr context !! */
int
bcmptm_wal_rdr_msg_done(int u, bcmptm_wal_msg_t *walr_msg,
                        bcmptm_walr_status_t *scf_status)
{
    wal_trans_t *trans_p;
    uint32_t obs_wal_msg_idx;
    bool rdr_has_wstate_lock = FALSE;
    bcmptm_trans_cb_f trans_notify_fn;
    bool slam_msg = FALSE, release_trans = FALSE;
    uint32_t release_num_ops, release_words_buf_count;
    SHR_FUNC_ENTER(u);

    trans_p = wal_trans[u] + walr_msg->trans_idx;
    slam_msg = (walr_msg->cmd == BCMPTM_WAL_MSG_CMD_SLAM);

    if (walr_msg->need_cbf_before_msg && !slam_msg) {
        /* Means callback is BEFORE the msg.
         * These pre_msg callbacks need not happen in normal order - so we should
         * not be checking for expected msg_idx */
        SHR_IF_ERR_EXIT(
            wal_rdr_pre_msg(u, walr_msg));
        /* cbf before msg must happen only ONCE */
        walr_msg->need_cbf_before_msg = 0;
        SHR_EXIT();
    }

    obs_wal_msg_idx = walr_msg - bcmptm_wal_msg_bp[u];
    if (obs_wal_msg_idx != wstate[u]->n_rdr_msg_idx) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "rdr_msg_done: exp_msg_idx = %0u, obs_wal_msg_idx"
                        " = %0u\n"),
             wstate[u]->n_rdr_msg_idx, obs_wal_msg_idx));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT(trans_p->num_msgs < 1 ? SHR_E_INTERNAL
                                            : SHR_E_NONE);

    if (walr_msg->words_buf_idx & 0x3) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u, "rdr_msg_done: align_ERROR, msg.words_buf_idx = "
                        "%0u\n"),
             walr_msg->words_buf_idx));
    }

    print_wstate_info(u, FALSE, "RDR");
    print_msg_info(u, FALSE, wstate[u]->n_rdr_msg_idx, walr_msg, "RDR");

    /* Ignore scf_status */

    { /* successful completion */
        /* If we are here as continuation for failure condition, we already have
         * wstate_lock - ok to be less efficient when we have errors.
         *
         * If we are here for success case, we don't have lock. This is the
         * mainline hi-performance path. Must take lock only when we release
         * resources (increment wstate[u]->r_counts).
         */
        switch (walr_msg->cmd) {
            case BCMPTM_WAL_MSG_CMD_WRITE:
            case BCMPTM_WAL_MSG_CMD_EMPTY_WRITE:
            case BCMPTM_WAL_MSG_CMD_SLAM:
                /* Update slice_mode, entry_mode.
                 * Must be done before update of msg vars, release of ops */
                if (!slam_msg) {
                    SHR_IF_ERR_EXIT(
                        wal_rdr_post_msg(u, walr_msg, walr_msg->num_ops,
                                         TRUE));
                }

                if (cfg_wal_stats_enable[u]) {
                    if (s_max_words_in_msg[u] < walr_msg->req_entry_wsize &&
                        !slam_msg) {
                        s_max_words_in_msg[u] = walr_msg->req_entry_wsize;
                    }

                    if (s_max_ops_in_msg[u] < walr_msg->num_ops && !slam_msg) {
                        s_max_ops_in_msg[u] = walr_msg->num_ops;
                    }

                    if (s_max_msgs_in_trans[u] < trans_p->num_msgs) {
                        s_max_msgs_in_trans[u] = trans_p->num_msgs;
                    }
                    if (s_max_slam_in_trans[u] < trans_p->slam_msg_count) {
                        s_max_slam_in_trans[u] = trans_p->slam_msg_count;
                    }
                }

                /* Optional - needed for error checking. */
                /* Don't modify trans_p->f_msg_idx, num_msg, etc */
                wstate[u]->rdr_num_ops += slam_msg ? 1 : walr_msg->num_ops;
                wstate[u]->rdr_num_words += (walr_msg->req_entry_wsize +
                                             walr_msg->num_words_skipped);
                if (slam_msg) {
                    wstate[u]->rdr_slam_msg_count += 1;
                }

                wstate[u]->rdr_msg_count += 1;
                if (walr_msg->last_msg &&
                    wstate[u]->rdr_msg_count == trans_p->num_msgs) {

                    print_trans_info(u, FALSE, walr_msg->trans_idx,
                                     trans_p, "RDR");
                    release_trans = TRUE;
                    if (trans_p->num_ops != wstate[u]->rdr_num_ops ||
                        trans_p->num_words != wstate[u]->rdr_num_words ||
                        trans_p->slam_msg_count !=
                        wstate[u]->rdr_slam_msg_count) {
                        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(u, "Trans has non-zero rsrc_counts "
                                        "even when last msg is done !! \n")));
                        print_trans_info(u, TRUE, walr_msg->trans_idx,
                                         trans_p, "RDR");
                        SHR_ERR_EXIT(SHR_E_FAIL);
                    }
                    wstate[u]->rdr_msg_count = 0; /* cleanup */
                    wstate[u]->rdr_num_ops = 0;
                    wstate[u]->rdr_num_words = 0;
                    wstate[u]->rdr_slam_msg_count = 0;

                    /* Call notification function */
                    if (trans_p->notify_fn_p) {
                        trans_notify_fn = trans_p->notify_fn_p;
                        trans_notify_fn(trans_p->cseq_id, SHR_E_NONE,
                                        trans_p->notify_info);
                    }
                } /* all msgs done */

                /* Count wal_msg related resource release counts */
                release_num_ops = slam_msg ? 1 : walr_msg->num_ops;
                release_words_buf_count =
                    (walr_msg->req_entry_wsize +
                     walr_msg->num_words_skipped);

                /* Take wstate_lock before releasing resources. */
                if (!rdr_has_wstate_lock) {
                    SHR_IF_ERR_EXIT(
                        bcmptm_mutex_take(u, wstate_mutex[u],
                                          WSTATE_LOCK_WAIT_USEC));
                    rdr_has_wstate_lock = TRUE;
                }

                if (cfg_wal_stats_enable[u]) {
                    uint32_t avail_trans_count, avail_msg_count,
                             avail_wal_ops_info_count, avail_words_buf_count;
                    /* Note: Following evaluation is happening in WAL reader
                     *       context - and looks at wstate[u]->w_abc_count vars -
                     *       which change in WAL writer context. And this is ok,
                     *       we are only looking at the vars. There is no
                     *       requirement that these should not be changing while
                     *       we are evaluating the counts.
                     */
                    wal_avail_count_eval(u, &avail_trans_count,
                                         &avail_msg_count,
                                         &avail_wal_ops_info_count,
                                         &avail_words_buf_count);
                    if (s_min_avail_trans_count[u] >
                        avail_trans_count) {
                        s_min_avail_trans_count[u] = avail_trans_count;
                    }
                    if (s_min_avail_msg_count[u] >
                        avail_msg_count) {
                        s_min_avail_msg_count[u] = avail_msg_count;
                    }
                    if (s_min_avail_ops_info_count[u] >
                        avail_wal_ops_info_count) {
                        s_min_avail_ops_info_count[u] =
                            avail_wal_ops_info_count;
                    }
                    if (s_min_avail_words_buf_count[u] >
                        avail_words_buf_count) {
                        s_min_avail_words_buf_count[u] =
                            avail_words_buf_count;
                    }
                }

                /* Increment of these counts gives Writer the permission to use
                 * the corresponding resources. Best to do it once all
                 * processing is complete.
                 */
                wstate[u]->r_wal_ops_info_count += release_num_ops;
                wstate[u]->r_words_buf_count += release_words_buf_count;
                if (release_trans) {
                    
                    sal_memset(trans_p, 0, sizeof(wal_trans_t));
                    wstate[u]->r_trans_count += 1;
                }

                /* Cleanup wal_msg */
                sal_memset(walr_msg, 0, sizeof(bcmptm_wal_msg_t));
                SHR_IF_ERR_EXIT(wstate_rdr_msg_idx_inc(u));
                wstate[u]->r_msg_count += 1;
                break; /* CMD_WRITE, SLAM */

            default:
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "rdr_msg_done cannot support unknown "
                                "type msg_cmd = %d !! \n"), walr_msg->cmd));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
                break;
        } /* msg_cmd */
    } /* successful completion */

    /* Update WAL read pointers */

    /* Release WAL resources */

    /* Notification */

exit:
    if (rdr_has_wstate_lock) {
        int tmp_rv = SHR_E_NONE;
        tmp_rv = sal_mutex_give(wstate_mutex[u]);
        if (SHR_FAILURE(tmp_rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "rdr_msg_done could not release wstate_mutex "
                            "!!\n")));
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_cfg_set(int u)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(u);

    /* Read configured values */
    SHR_IF_ERR_EXIT(wal_bcmcfg_get(u));

    /* Atomic trans enable mode */
    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || (feature_conf.enable_atomic_trans == 0)) {
        wal_disable_undo[u] = TRUE; /* must be done before alloc */
    } else {  /* Feature is disabled */
        wal_disable_undo[u] = FALSE; /* must be done before alloc */
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "wal_disable_undo is %s\n"),
         wal_disable_undo[u] ?  "True" : "False"));

    /* Override values for utptm */
    switch (cfg_wal_mode[u]) {
    case -1:
        /* Initial Auto-commit settings:
         * One op per msg.
         * One msg per trans
         */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = FALSE;

        cfg_wal_trans_max_count[u] = 10;
        cfg_wal_msg_max_count[u] = 100;
        cfg_wal_words_buf_max_count[u] = (100*40);
        cfg_wal_ops_info_max_count[u] = 100;
        cfg_undo_ops_info_max_count[u] = 100;
        cfg_undo_words_buf_max_count[u] = 100*40;
        cfg_wal_max_words_in_msg[u] = 80;
        cfg_wal_max_ops_in_msg[u] = 1;
        cfg_wal_max_msgs_in_trans[u] = 1;
        break;
    case -2: /* No WAL bypass (two thread mode) */
        /* Auto-commit settings that give more WAL coverage:
         * 5 ops per msg
         * 3 msgs per trans
         * Almost ALL utptm tests use this mode.
         */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = FALSE;

        cfg_wal_trans_max_count[u] = BCMPTM_UT_WAL_TRANS_MAX_COUNT;
        cfg_wal_msg_max_count[u] = BCMPTM_UT_WAL_MSG_MAX_COUNT;
        cfg_wal_words_buf_max_count[u] = BCMPTM_UT_WAL_WORDS_BUF_MAX_COUNT;
        cfg_wal_ops_info_max_count[u] = BCMPTM_UT_WAL_OPS_INFO_MAX_COUNT;
        cfg_undo_ops_info_max_count[u] = BCMPTM_UT_WAL_UNDO_OPS_INFO_MAX_COUNT;
        cfg_undo_words_buf_max_count[u] = BCMPTM_UT_WAL_UNDO_WORDS_BUF_MAX_COUNT;
        cfg_wal_max_words_in_msg[u] = 100;
        cfg_wal_max_ops_in_msg[u] = 5;
        cfg_wal_max_msgs_in_trans[u] = 3;
        break;


    case -10: /* WAL Bypass, PIO (one-thread mode) */
    case 110: /* same as -10, but not auto-commit mode */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = TRUE; /* even if device needs smc, emc msgs */
        cfg_scf_num_chans[u] = 0;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "=== WAL_BYPASS, SCF0(PIO)\n")));

        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = 4;
        break;
    case 120: /* Atomic trans disabled, but WAL not bypassed, SCF0 */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = FALSE;
        cfg_scf_num_chans[u] = 0;

        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = 4;
        break;


    case -11: /* WAL Bypass, SCF1 (one-thread mode) */
    case 111: /* same as -11, but not auto-commit mode */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = TRUE; /* even if device needs smc, emc msgs */
        cfg_scf_num_chans[u] = 1;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "=== WAL_BYPASS, SCF1\n")));

        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = 4;
        break;
    case 121: /* Atomic trans disabled, but WAL not bypassed, SCF1 */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = FALSE;
        cfg_scf_num_chans[u] = 1;

        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = 4;
        break;


    case -12: /* WAL Bypass, SCF2 (one-thread mode) */
    case 112: /* same as -12, but remove auto-commit mode */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = TRUE; /* even if device needs smc, emc msgs */
        cfg_scf_num_chans[u] = 2;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "=== WAL_BYPASS, SCF2\n")));

        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = 4;
        break;
    case 122: /* Atomic trans disabled, but WAL not bypassed, SCF2 */
        wal_disable_undo[u] = TRUE;
        cfg_wal_bypass[u] = FALSE;
        cfg_scf_num_chans[u] = 2;

        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = 4;
        break;


    case 1:
        /* NOT auto-commit mode
         * Just for utptm tests - to create lot of messages in trans
         * Only utptm_test_trans.c uses this mode */
        /* Requires user to issue commit, abort */

        /* No overrides for wal_bypass, disable_undo */

        cfg_wal_trans_max_count[u] = BCMPTM_UT_WAL_TRANS_MAX_COUNT;
        cfg_wal_msg_max_count[u] = BCMPTM_UT_WAL_MSG_MAX_COUNT;
        cfg_wal_words_buf_max_count[u] = BCMPTM_UT_WAL_WORDS_BUF_MAX_COUNT;
        cfg_wal_ops_info_max_count[u] = BCMPTM_UT_WAL_OPS_INFO_MAX_COUNT;
        cfg_undo_ops_info_max_count[u] = BCMPTM_UT_WAL_UNDO_OPS_INFO_MAX_COUNT;
        cfg_undo_words_buf_max_count[u] = BCMPTM_UT_WAL_UNDO_WORDS_BUF_MAX_COUNT;
        cfg_wal_max_words_in_msg[u] = BCMPTM_UT_WAL_MAX_WORDS_IN_MSG;
        cfg_wal_max_ops_in_msg[u] = 2;
        cfg_wal_max_msgs_in_trans[u] = BCMPTM_UT_WAL_MAX_MSGS_IN_TRANS;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u,
                        "=== Not auto-commit mode, and also not real commit "
                        "mode, %0x\n"),
             cfg_wal_mode[u]));
        break;
    default:
        /* Real Commit mode
         * - Requires user to select WAL bypass, enable/disable atomic trans.
         * - Used by all SQA tests.
         * - Requires user to issue commit, abort irrespective of
         *   whether atomic_trans is 0 / 1.
         */
        cfg_wal_max_ops_in_msg[u] = BCMPTM_UT_WAL_MAX_OPS_IN_MSG;
        cfg_wal_max_msgs_in_trans[u] = BCMPTM_UT_WAL_MAX_MSGS_IN_TRANS;
        /* No overrides for wal buffer sizes */
        break;
    } /* cfg_wal_mode */

    cfg_wal_trans_idx_max[u] = 0;
    cfg_wal_msg_idx_max[u] = 0;
    cfg_wal_ops_info_idx_max[u] = 0;
    /* cfg_wal_words_buf_idx_max[u] = 0; */
    cfg_undo_ops_info_idx_max[u] = 0;
    /* cfg_undo_words_buf_idx_max[u] = 0; */

    if (cfg_wal_trans_max_count[u]) {
        cfg_wal_trans_idx_max[u] = cfg_wal_trans_max_count[u] - 1;
    }
    if (cfg_wal_msg_max_count[u]) {
        cfg_wal_msg_idx_max[u] = cfg_wal_msg_max_count[u] - 1;
    }
    if (cfg_wal_ops_info_max_count[u]) {
        cfg_wal_ops_info_idx_max[u] = cfg_wal_ops_info_max_count[u] - 1;
    }
    /*
    if (cfg_wal_words_buf_max_count[u]) {
        cfg_wal_words_buf_idx_max[u] = cfg_wal_words_buf_max_count[u] - 1;
    }
    */
    if (cfg_undo_ops_info_max_count[u]) {
        cfg_undo_ops_info_idx_max[u] = cfg_undo_ops_info_max_count[u] - 1;
    }
    /*
    if (cfg_undo_words_buf_max_count[u]) {
        cfg_undo_words_buf_idx_max[u] = cfg_undo_words_buf_max_count[u] - 1;
    }
    */

    SHR_IF_ERR_EXIT(
        (!cfg_wal_trans_idx_max[u] ||
         !cfg_wal_msg_idx_max[u] ||
         !cfg_wal_ops_info_idx_max[u] ||
         /* !cfg_wal_words_buf_idx_max[u] || */
         /* !cfg_undo_words_buf_idx_max[u] || */
         !cfg_undo_ops_info_idx_max[u]) ? SHR_E_INTERNAL : SHR_E_NONE);

    /*  Error Checking:
     *
     *      disable_undo     bypass     comment
     *      0                0          Atomic Trans enabled.
     *
     *      0                1          illegal
     *
     *      1                0          Atomic Trans disabled.
     *                                  WAL in 2 thread mode.
     *
     *      1                1          Atomic Trans disabled.
     *                                  WAL in 1 thread mode.
     */
    SHR_IF_ERR_EXIT(cfg_wal_bypass[u] && !wal_disable_undo[u] ?
                    SHR_E_INTERNAL : SHR_E_NONE);

    /* When not in autocommit mode, we cannot use wal_bypass mode if slice_mode
     * or entry_mode changes need to be communicated to SER logic.
     * Note: only wal_mode < 0 means autocommit. */
    SHR_IF_ERR_EXIT(cfg_wal_bypass[u] &&
                    (cfg_wal_mode[u] == 0) &&
                    (dev_info[u]->need_smc_msg ||
                     dev_info[u]->need_emc_msg) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u,
                    "cfg_wal_mode = %0d, "
                    "wal_disable_undo = %s, "
                    "cfg_wal_bypass = %s, "
                    "cfg_scf_num_chans = %0d, "
                    "cfg_wal_trans_max_count = %0u, "
                    "cfg_wal_msg_max_count = %0u, "
                    "cfg_wal_words_buf_max_count = %0u, "
                    "cfg_wal_ops_info_max_count = %0u, "
                    "cfg_wal_undo_ops_info_max_count = %0u, "
                    "cfg_wal_undo_words_buf_max_count = %0u, "
                    "cfg_wal_max_words_in_msg = %0u, "
                    "cfg_wal_max_ops_in_msg = %0u, "
                    "cfg_wal_max_msgs_in_trans = %0u, \n"),
                    cfg_wal_mode[u],
                    wal_disable_undo[u] ? "True" : "False",
                    cfg_wal_bypass[u] ? "True" : "False",
                    cfg_scf_num_chans[u],
                    cfg_wal_trans_max_count[u],
                    cfg_wal_msg_max_count[u],
                    cfg_wal_words_buf_max_count[u],
                    cfg_wal_ops_info_max_count[u],
                    cfg_undo_ops_info_max_count[u],
                    cfg_undo_words_buf_max_count[u],
                    cfg_wal_max_words_in_msg[u],
                    cfg_wal_max_ops_in_msg[u],
                    cfg_wal_max_msgs_in_trans[u]));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_wal_cseq_id_check(int u, uint32_t in_cseq_id)
{
    SHR_FUNC_ENTER(u);
    if (bcmptm_wal_autocommit_enabled(u)) {
        SHR_EXIT();
    } else if (wstate[u]->mreq_state_in_trans) {
        /* enable_atomic_trans can be 0 or 1 */
        /* cfg_wal_bypass can be 0 or 1 */
        SHR_IF_ERR_MSG_EXIT(
            ((in_cseq_id != wstate[u]->mreq_cseq_id) ? SHR_E_INTERNAL
                                                     : SHR_E_NONE),
            (BSL_META_U(u, "Received cseq_id = %0u, expected cseq_id = %0u\n"),
             in_cseq_id, wstate[u]->mreq_cseq_id));
    } /* normal mode */
exit:
    SHR_FUNC_EXIT();
}

/* trans_cmd handler when WAL bypassed with 0 schan_fifo channels.
 * No mreq_state, cseq_id are maintained. */
static int
wal_trans_cmd_byp(int u, uint64_t flags,
                  bcmptm_trans_cmd_t trans_cmd,
                  uint32_t in_cseq_id, bcmptm_trans_cb_f notify_fn_p,
                  void *notify_info)
{
    int tmp_rv = SHR_E_NONE;
    SHR_FUNC_ENTER(u);

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            /*
             * mreq_state_in_trans = 0 case:
             *      Even though there were no PTM/WAL ops in cur trans, there
             *      may have been ops for IMM tables.
             *
             *      This cannot be crash-recovery case, because when WAL is
             *      engaged in single-thread mode - all ops of trans were
             *      flushed when we received commit - we always start new
             *      transaction with empty WAL. Thus there can be no pending
             *      transactions - except for current transaction.
             *
             *      Can we support crash-recovery when WAL is in single-thread
             *      mode? Not really, because enable_atomic_trans = 0 is
             *      precondition for engaging WAL in single-thread mode.
             *      Crash-recovery involves
             *          - recommitting of pending transactions
             *          - aborting of failed transaction (undo changes to
             *            PTcache, etc).
             *          - rebuilding failed transaction, sending of failed
             *          transaction.
             *       But because we cannot support undo of transaction in
             *       single_thread mode, we cannot support Abort and thus no
             *       crash-recovery support.
             *
             * In either case, send notification, if needed.
             * were READ, LOOKUP, GET_FIRST, GET_NEXT
             */

            /* Get status for posted write */
            tmp_rv = posted_wr_status_check(u, 0);
            if (notify_fn_p) {
                notify_fn_p(in_cseq_id, tmp_rv, notify_info);
            }
            break;

        case BCMPTM_TRANS_CMD_ABORT:
            /*
             * mreq_state_in_trans = 0 case:
             *      Even though there were no PTM/WAL ops in cur trans, there
             *      may have been ops for IMM tables.
             */

            /* We had already sent a write for this trans to HW, so abort is
             * illegal. */
            if (wstate[u]->posted_scf_write || wstate[u]->sent_pio_write) {

                /* Get status for posted write - to clear posted_scf_write
                 */
                tmp_rv = posted_wr_status_check(u, 0);
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Got abort when WAL state is not IDLE "
                                "(wal_bypass), enable_atomic_trans = 0.\n")));

                /* ignore tmp_rv */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else {
                /* ignore - treat it as NOP. */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Abort ignored when enable_atomic_trans "
                                "= 0\n")));
            }
            break;

        default: /* BCMPTM_TRANS_CMD_IDLE_CHECK */
            /* We could have reverted from dynamic atomic_trans to bypass mode,
             * so wstate->n_wal_ops_info_idx may not be 0 and hence should not
             * check for this condition.
             */

            if (wstate[u]->mreq_state_in_trans) {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "!! ERROR: mreq_state NOT IDLE when "
                                "stop (IDLE_CHECK) was requested \n")));
                tmp_rv = SHR_E_INTERNAL;
            }
            if (!bcmptm_wal_autocommit_enabled(u)) {
                if (wstate[u]->posted_scf_write) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "!! ERROR: posted_scf_write NOT 0 "
                                    "when stop (IDLE_CHECK) was requested \n")));
                    tmp_rv = SHR_E_INTERNAL;
                }
                if (wstate[u]->sent_pio_write) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "!! ERROR: sent_pio_write NOT 0 "
                                    "when stop (IDLE_CHECK) was requested \n")));
                    tmp_rv = SHR_E_INTERNAL;
                }
            }
            /* After idle_check, bcmptm_mreq_stop() will call bcmptm_wal_drain -
             * which will call posted_wr_status_check() and result in clear
             * of wstate->posted_scf_write
             */

            break;
    } /* trans_cmd */
    SHR_IF_ERR_EXIT(tmp_rv);
exit:
    wstate[u]->sent_pio_write = FALSE;
    SHR_FUNC_EXIT();
}

/* trans_cmd handler when WAL is not bypassed */
static int
wal_trans_cmd_normal(int u, uint64_t flags,
                     bcmptm_trans_cmd_t trans_cmd,
                     uint32_t in_cseq_id,
                     bcmptm_trans_cb_f notify_fn_p,
                     void *notify_info)
{
    SHR_FUNC_ENTER(u);

    switch (trans_cmd) {
        case BCMPTM_TRANS_CMD_COMMIT:
            SHR_IF_ERR_EXIT(bcmptm_wal_cseq_id_check(u, in_cseq_id));

            SHR_IF_ERR_EXIT(
                bcmptm_wal_commit(u, flags, wstate[u]->mreq_state_in_trans,
                                  in_cseq_id, notify_fn_p,
                                  notify_info));
            wstate[u]->mreq_state_in_trans = FALSE;
            break;
        case BCMPTM_TRANS_CMD_ABORT:
            SHR_IF_ERR_EXIT(bcmptm_wal_cseq_id_check(u, in_cseq_id));

            if (!wstate[u]->mreq_state_in_trans) {
                /* LTM manages index tables and has the 'valid' state info for every
                 * 'key'(index) of such table. Lookup for such table may not create
                 * any PTM request - so PTM may be in IDLE.
                 *
                 * Thus, if lookup was targeted for indexed table to LTM and it
                 * fails, TRM will still issue Abort in this case - which LTM will
                 * forward to PTM - even when PTM and WAL were in IDLE.
                 * So getting Abort when mreq, PTRM, WAL are in IDLE is not an
                 * error case.
                 */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(u, "!! NOP: received with ABORT even when there "
                                "were no prior ops \n")));
            }

            SHR_IF_ERR_EXIT(
                bcmptm_wal_abort(u, flags, in_cseq_id));
                /* Note: wal_abort() will also Undo changes to PTcache */

            wstate[u]->mreq_state_in_trans = FALSE;
            break;
        default: /* BCMPTM_TRANS_CMD_IDLE_CHECK */
            if (bcmptm_wal_autocommit_enabled(u)) {
                /* We already ensured that LTM (and hence TRM) has stopped,
                 * so we know LTM is not issuing more requests */

                wstate[u]->mreq_state_in_trans = FALSE;
                wstate[u]->mreq_cseq_id = 0;
            } else if (wstate[u]->mreq_state_in_trans) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "!! ERROR: mreq_state NOT IDLE when "
                                    "stop (IDLE_CHECK) was requested \n")));
                    SHR_ERR_EXIT(SHR_E_FAIL);
            }
            break;
    } /* trans_cmd */

exit:
    SHR_FUNC_EXIT();
}

/* Following will be called for every non-NOP request from LTM
 * It must be called even for READ, LOOKUP, GET_FIRST, GET_NEXT, etc ops
 * It changes mreq_state_in_trans to TRUE.
 */
static int
wal_mreq_state_update(int u, uint32_t in_cseq_id, uint64_t flags)
{
    SHR_FUNC_ENTER(u);
    if (wstate[u]->mreq_state_in_trans) { /* check cseq_id */
        SHR_IF_ERR_EXIT(
            bcmptm_wal_cseq_id_check(u, in_cseq_id));
    } else { /* record cseq_id for new trans */
        wstate[u]->mreq_cseq_id = in_cseq_id;
        wstate[u]->mreq_state_in_trans = TRUE;
    }
exit:
    SHR_FUNC_EXIT();
}

/* mreq_state_update handler when WAL bypassed with 0 schan_fifo channels.
 * No mreq_state, cseq_id are maintained. */
static int
wal_mreq_state_update_byp(int u, uint32_t in_cseq_id, uint64_t flags)
{
    return SHR_E_NONE;
}

/* Can return SHR_E_MEMORY */
static int
wal_slam_exe(int u,
             bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *pt_dyn_info,
             uint32_t *entry_words,
             uint32_t entry_count)
{
    uint32_t cwords_array[BCMPTM_MAX_BD_CWORDS], num_cwords = 0,
             pt_entry_wsize, *w_words_p;
    bcmbd_sbusdma_data_t dma_data;
    bcmbd_sbusdma_work_t dma_work;
    SHR_FUNC_ENTER(u);

    w_words_p = bcmptm_wal_words_buf[u];
    pt_entry_wsize = entry_count * bcmdrd_pt_entry_wsize(u, sid);

    if (cfg_wal_words_buf_max_count[u] >= pt_entry_wsize) {
        if (bcmdrd_pt_attr_is_cam(u, sid)) {
            SHR_IF_ERR_EXIT( /* KM -> TCAM format conversion */
                bcmptm_pt_tcam_km_to_xy(u, sid,
                                        entry_count,   /* 1 for !SLAM */
                                        entry_words,   /* km_entry */
                                        w_words_p)); /* xy_entry */
        } else {
            sal_memcpy(w_words_p, entry_words, 4 * pt_entry_wsize);
        }

        SHR_IF_ERR_EXIT(
            bcmbd_pt_cmd_hdr_get(u,
                                 sid,
                                 pt_dyn_info,
                                 NULL,
                                 BCMBD_PT_CMD_WRITE,
                                 BCMPTM_MAX_BD_CWORDS,
                                 cwords_array,
                                 &num_cwords));
        SHR_IF_ERR_EXIT(
            (num_cwords != dev_info[u]->num_cwords) ?
            SHR_E_INTERNAL : SHR_E_NONE);

        /* Prepare dma_data */
        sal_memset(&dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
        dma_data.start_addr = cwords_array[1]; /* Addr beat */
        dma_data.data_width = bcmdrd_pt_entry_wsize(u, sid);
                                               /* Num of data beats */
         /* dma_data.addr_gap = 0; */          /* Gap between addresses */
        dma_data.op_code = cwords_array[0];    /* Opcode beat */
        dma_data.op_count = entry_count; /* Num of locations */
        dma_data.buf_paddr = bcmptm_wal_words_buf_bpa[u];
        SHR_IF_ERR_EXIT(
            (dma_data.buf_paddr & 0xF) ? SHR_E_INTERNAL : SHR_E_NONE);
            /* Phys addr for slam data.
             * Must be on 4 Byte boundary.
             * Should be on 16 Byte boundary for optimum performance. */
        /* dma_data.jump_item = 0; */
        /* dma_data.attrs = 0; */
        dma_data.pend_clocks = bcmbd_mor_clks_write_get(u, sid);

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

        SHR_IF_ERR_EXIT(bcmbd_sbusdma_light_work_init(u, &dma_work));
        SHR_IF_ERR_EXIT(bcmbd_sbusdma_work_execute(u, &dma_work));

    } else { /* Not enough space */
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(u,
                        "Not enough space in WAL buffers for SLAM\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
exit:
    SHR_FUNC_EXIT();
}

/* wal_write handler when WAL bypassed with 0 schan_fifo channels.
 * - No change in wstate when returning with error. */
static int
wal_write_byp_pio(int u,
                  uint64_t flags,
                  bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                  bcmptm_misc_info_t *misc_info,
                  uint32_t *entry_words,
                  bcmptm_rm_op_t op_type,
                  bcmptm_rm_slice_change_t *slice_mode_change_info)
{
    SHR_FUNC_ENTER(u);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    /* Slice mode, entry mode changes not supported in wal bypass mode */
    /* assert(!slice_mode_change_info); */
    /* assert(!op_type); */

    if (misc_info && misc_info->dma_enable) {
        SHR_IF_ERR_EXIT(
            wal_slam_exe(u, sid, pt_dyn_info, entry_words,
                         misc_info->dma_entry_count));
    } else { /* single entry write */
        /* Assume: WAL words buf is big enough to store one entry of any
         *         table. So, space check is not needed.
 */
        if (bcmdrd_pt_attr_is_cam(u, sid)) {
            uint32_t *w_words_p = bcmptm_wal_words_buf[u];
            SHR_IF_ERR_EXIT( /* KM -> TCAM format conversion */
                bcmptm_pt_tcam_km_to_xy(u, sid,
                                        1,             /* entry_count */
                                        entry_words,   /* km_entry */
                                        w_words_p)); /* xy_entry */
            SHR_IF_ERR_EXIT(
                bcmbd_pt_write(u, sid,
                               pt_dyn_info, NULL,
                               w_words_p));
        } else {
            SHR_IF_ERR_EXIT(
                bcmbd_pt_write(u, sid,
                               pt_dyn_info, NULL,
                               entry_words));
        }
    } /* single entry write */

    /* Set following when pio / slam executes successfully */
    wstate[u]->sent_pio_write = TRUE;
exit:
    SHR_FUNC_EXIT();
}

/* wal_write handler when WAL bypassed with non-zero schan_fifo channels.
 * No mreq_state, cseq_id are maintained. */
static int
wal_write_byp_scf(int u,
                  uint64_t flags,
                  bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                  bcmptm_misc_info_t *misc_info,
                  uint32_t *entry_words,
                  bcmptm_rm_op_t op_type,
                  bcmptm_rm_slice_change_t *slice_mode_change_info)
{
    int ch = 0;
    SHR_FUNC_ENTER(u);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    /* Slice mode, entry mode changes not supported in wal bypass mode */
    /* assert(!slice_mode_change_info); */
    /* assert(!op_type); */

    if (misc_info && misc_info->dma_enable) {
        if (wstate[u]->posted_scf_write) {
            /* A write was posted to HW and now we see slam_req.
             *
             * If we first call posted_wr_status_check() - it will clear
             * wstate->posted_scf_write.
             *
             * Subsequently:
             * 1. if we get abort - it is illegal - but we cannot detect this -
             *    because wstate->posted_scf_write was cleared..
             *
             * 2. if we get request to enable atomic_trans dynamically, we
             *    cannot catch that this enable is being requested in middle of
             *    trans.
             *
             * Simple way to fix above problems is to first set
             * wstate->sent_pio_write and then call
             * posted_wr_status_check().
             */

            /* first (because status_check may exit with error */
            wstate[u]->sent_pio_write = TRUE;

            /* then - check status of posted writes (byp_scf) */
            SHR_IF_ERR_EXIT(posted_wr_status_check(u, 0));
        }

        /* Now post the slam req */
        SHR_IF_ERR_EXIT(
            wal_slam_exe(u, sid, pt_dyn_info, entry_words,
                         misc_info->dma_entry_count));
    } else { /* single entry write */
        /* Assume: WAL words buf is big enough to store one entry of any
         *         table. So, space check is not needed.
 */
        uint32_t num_cwords = 0;
        uint32_t *w_words_p = bcmptm_wal_words_buf[u];
        uint32_t pt_entry_wsize = bcmdrd_pt_entry_wsize(u, sid);

        /* Store opcode, address beat words */
        SHR_IF_ERR_EXIT(
            bcmbd_pt_cmd_hdr_get(u, sid, pt_dyn_info, NULL,
                                 BCMBD_PT_CMD_WRITE,
                                 BCMPTM_MAX_BD_CWORDS,
                                 w_words_p,
                                 &num_cwords));
        /* assert(num_cwords == dev_info[u]->num_cwords); */
        w_words_p += num_cwords;

        /* Store entry_words */
        if (bcmdrd_pt_attr_is_cam(u, sid)) {
            SHR_IF_ERR_EXIT( /* KM -> TCAM format conversion */
                bcmptm_pt_tcam_km_to_xy(u, sid,
                                        1,             /* entry_count */
                                        entry_words,   /* km_entry */
                                        w_words_p)); /* xy_entry */
        } else {
            sal_memcpy(w_words_p, entry_words, 4 * pt_entry_wsize);
        }

        /* Check status and then clear start bit (that was set during previous write) */
        SHR_IF_ERR_EXIT(posted_wr_status_check(u, ch));

        /* Send ops */
        SHR_IF_ERR_EXIT(
            bcmbd_schanfifo_ops_send(u, ch,
                                     1, /* entry_count */
                                     bcmptm_wal_words_buf[u],
                                        /* req_entry_words */
                                     (num_cwords + pt_entry_wsize),/* words_buf_wsize */
                                     bcmptm_wal_words_buf_bpa[u],
                                        /* req_entry_words_pa */
                                     SCHANFIFO_OF_SET_START));
        wstate[u]->posted_scf_write = TRUE; /* set for successful send */
    } /* single entry write */

exit:
    SHR_FUNC_EXIT();
}

static int
wal_write_normal(int u,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 bcmptm_misc_info_t *misc_info,
                 uint32_t *entry_words,
                 bcmptm_rm_op_t op_type,
                 bcmptm_rm_slice_change_t *slice_mode_change_info)
{
    uint32_t w_trans_idx, w_msg_idx, w_wal_ops_info_idx, w_words_buf_idx,
             w_undo_ops_info_idx, w_undo_words_buf_idx;
    uint32_t pt_entry_wsize, max_pt_entry_wsize;
    uint32_t undo_entry_wsize = 0;
    uint32_t *w_words_p, w_num_words;
    uint32_t entry_count = 1, num_cwords = 0;
    bool done = FALSE, close_msg_before_this_op = FALSE, wal_full = FALSE,
         close_msg_after_this_op = FALSE, slam_en,
         zero_out_n_words_buf_idx = FALSE;
    uint32_t num_words_skipped = 0, retry_count = 0;
    uint32_t req_trans_count = 0, req_msg_count = 0,
             req_undo_ops_info_count = 0;
    uint32_t avail_trans_count, avail_msg_count, avail_wal_ops_info_count,
             avail_words_buf_count;

    wal_trans_t *w_trans_p;
    bcmptm_wal_msg_t *w_msg_p;
    bcmptm_wal_ops_info_t *w_wal_ops_info_p;
    wal_undo_ops_t *w_undo_ops_info_p;
    bcmptm_wal_slice_mode_info_t smc_word;
    SHR_FUNC_ENTER(u);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    slam_en = (misc_info && misc_info->dma_enable);
    if (slam_en) {
        /* Error checks */
        SHR_IF_ERR_EXIT(slice_mode_change_info ? SHR_E_INTERNAL : SHR_E_NONE);
        SHR_IF_ERR_EXIT(op_type ? SHR_E_INTERNAL : SHR_E_NONE);
        SHR_IF_ERR_MSG_EXIT(
            !dma_avail[u] ? SHR_E_UNAVAIL : SHR_E_NONE,
            (BSL_META_U(u, "DMA support is not available.\n")));

        /* Main */
        entry_count = misc_info->dma_entry_count;
    }

    print_wstate_info(u, FALSE, "WR ");

    /* dwords needed for this entry */
    pt_entry_wsize = entry_count * bcmdrd_pt_entry_wsize(u, sid);
                                /* without cwords */
    SHR_IF_ERR_EXIT(!pt_entry_wsize ? SHR_E_INTERNAL : SHR_E_NONE);
    if (slam_en) {
        max_pt_entry_wsize = pt_entry_wsize;
    } else {
        max_pt_entry_wsize = pt_entry_wsize + BCMPTM_MAX_BD_CWORDS;
    }

    if (!wal_disable_undo[u]) {
        undo_entry_wsize = pt_entry_wsize; /* without cwords */
        req_undo_ops_info_count = 1; /* we need this info for all wal_writes to
                                        support abort.
                                        Is 1 even for slam type msg. */
    }

    /* Do we need new trans and/or new msg? */
    switch (wstate[u]->c_trans_state) {
        case TRANS_STATE_IN_MSG:
            req_trans_count = 0;

            if ((op_type == BCMPTM_RM_OP_SLICE_MODE_CHANGE) || slam_en) {
                /* Only one slice_mode_change per wal_msg.
                 * slice_mode_change is 1st op in msg.
                 * Use dedicated msg for SLAM. */
                close_msg_before_this_op = TRUE;
            } else {
                /* check if adding this op will cause schan_fifo viol */
                uint32_t tmp_num_words, tmp_num_ops;
                bool words_max, ops_max;
                w_msg_p = bcmptm_wal_msg_bp[u] + (wstate[u]->c_msg_idx);
                tmp_num_words = w_msg_p->req_entry_wsize;
                tmp_num_ops = w_msg_p->num_ops;
                words_max = (tmp_num_words + max_pt_entry_wsize) >
                             cfg_wal_max_words_in_msg[u];
                ops_max = (tmp_num_ops == cfg_wal_max_ops_in_msg[u]);
                if (words_max || ops_max) {
                    close_msg_before_this_op = TRUE;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "Coverage: close_msg due to "
                                    "msg_words_max=%s, msg_ops_max=%s \n"),
                                    words_max ? "TRUE" : "FALSE",
                                    ops_max ? "TRUE" : "FALSE"));
                    print_msg_info(u, FALSE, wstate[u]->c_msg_idx,
                                   bcmptm_wal_msg_bp[u] + wstate[u]->c_msg_idx,
                                   "WR");
                }
            }

            if (!close_msg_before_this_op) {
                /* Check if we need to close the msg because current op will
                 * result in words_buf wraparound. */
                SHR_IF_ERR_EXIT(
                    avail_words_skip(u,
                                     wstate[u]->n_words_buf_idx,
                                     max_pt_entry_wsize, /* cwords + dwords */
                                     &zero_out_n_words_buf_idx,
                                     &num_words_skipped));
            }

            if (close_msg_before_this_op || zero_out_n_words_buf_idx) {
                /* Only one of above terms can be TRUE.
                 *
                 * We need to send ops of prev_msg (and start new msg)
                 * because of:
                 * 1. slice_mode_change_op or slam_op or hitting msg_max_limit:
                 *    - close_msg_before_this_op = TRUE
                 *    - zero_out_n_words_buf_idx = FALSE
                 *    - num_words_skipped = 0
                 *
                 * 2. words_buf boundary wraparound:
                 *    - close_msg_before_this_op = FALSE
                 *    - zero_out_n_words_buf_idx = TRUE
                 *    - num_words_skipped != 0
                 */
                if (wal_disable_undo[u]) {
                    SHR_IF_ERR_EXIT(wal_send_msg_b4tc(u, wstate[u]->c_msg_idx));
                }

                req_msg_count = 1;
            } /* else {
                 Can allow next op in same msg to be consecutive in words_buf
            } */
            break;
        case TRANS_STATE_BLOCKING_READ: /* like TRANS_STATE_NEED_NEW_MSG */
        case TRANS_STATE_BLOCKING_READ_AFTER_WR:
            /* Don't allow any write ops after BLOCKING_HW_READ */
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "WRITE not allowed after BLOCKING_HW_READ"
                            " in same Atomic Transaction \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
            break;
        case TRANS_STATE_NEED_NEW_MSG:
            req_trans_count = 0;
            req_msg_count = 1;

            /* ASSUME:
             * single op can never violate BCMPTM_WAL_OPS_IN MSG_MAX,
             * cfg_wal_max_words_in_msg limits */
            break;
        default: /* TRANS_STATE_IDLE, IDLE_AFTER_WR */
            req_trans_count = 1;
            req_msg_count = 1;

            /* ASSUME:
             * single op can never violate BCMPTM_WAL_OPS_IN MSG_MAX,
             * cfg_wal_max_words_in_msg limits */

            break;
    } /* trans_state */

    if (req_msg_count) {
        /* We need to start new msg because trans_state was:
         *  - IN_MSG with
         *      - words_buf_wraparound case
         *      - close_msg_before_this_op = FALSE
         *      - zero_out_n_words_buf_idx = TRUE
         *      - num_words_skipped != 0 (case_1)
         *      - In this case, we will be zeroing out n_words_buf_idx and so
         *        we will be already aligned - there is no need to call
         *        align_skip().
         *
         *  - IN_MSG with
         *      - slice_mode_change_op or slam_op or hitting wal_msg_limit case
         *      - close_msg_before_this_op = TRUE
         *      - zero_out_n_words_buf_idx = FALSE
         *      - num_words_skipped = 0 (case_2)
         *
         *  - IDLE or or IDLE_AFTER_WR or NEED_NEW_MSG with:
         *      - close_msg_before_this_op = FALSE
         *      - zero_out_n_words_buf_idx = FALSE
         *      - num_words_skipped = 0 (case_3)
 */
        if (!num_words_skipped) { /* case_2 || case_3 */
            /*
             * We know zero_out_n_words_buf_idx = FALSE, num_words_skipped = 0
             *
             * Current op will be stored in new msg, so we need to ensure:
             * - 16 Byte physical_byte_address = 4 word alignment of
             *   word_buf_idx
             * - AND, also need to ensure that after the alignment, this op will
             *   not hit wraparound_boundary.
             */
            SHR_IF_ERR_EXIT(align_skip(u, max_pt_entry_wsize,
                                       &zero_out_n_words_buf_idx,
                                       &num_words_skipped));
            /* Above never exits with error. */
        } /******* else {
            - case_1
            - New msg will start at word_buf_idx = 0 and we need to check
            - availability of (max_pt_entry_wsize + num_words_skipped).
        } *******/
    } /* starting new msg */

    done = FALSE;
    retry_count = 0;
    do {
        wal_avail_count_eval(u, &avail_trans_count, &avail_msg_count,
                             &avail_wal_ops_info_count, &avail_words_buf_count);

        if ((avail_trans_count >= req_trans_count) &&
            (avail_msg_count >= req_msg_count) &&
            (avail_wal_ops_info_count >= 1) &&
            (avail_words_buf_count >=
             (max_pt_entry_wsize + num_words_skipped)) &&
            (wstate[u]->avail_undo_ops_info_count >= req_undo_ops_info_count) &&
            (wstate[u]->avail_undo_words_buf_count >= undo_entry_wsize)) {

            /* Record undo values of idx - new trans only */
            if (req_trans_count) {
                wstate[u]->a_msg_idx = wstate[u]->n_msg_idx;
                wstate[u]->a_wal_ops_info_idx = wstate[u]->n_wal_ops_info_idx;
                wstate[u]->a_words_buf_idx = wstate[u]->n_words_buf_idx;
                wstate[u]->a_umsg_idx = wstate[u]->n_umsg_idx;

                wstate[u]->a_w_trans_count = wstate[u]->w_trans_count;
                wstate[u]->a_w_msg_count = wstate[u]->w_msg_count;
                wstate[u]->a_w_wal_ops_info_count =
                    wstate[u]->w_wal_ops_info_count;
                wstate[u]->a_w_words_buf_count = wstate[u]->w_words_buf_count;
            }

            /* WAL_WORDS_BUF - determine num_cwords & write cwords */
            if (num_words_skipped) {
                /* Must write words at correct words_buf_idx */
                if (zero_out_n_words_buf_idx) {
                    wstate[u]->n_words_buf_idx = 0;
                } else {
                    wstate[u]->n_words_buf_idx += num_words_skipped;
                }
            }
            w_words_buf_idx = wstate[u]->n_words_buf_idx;
            w_words_p = bcmptm_wal_words_buf[u] + w_words_buf_idx;
            if (bcmptm_wal_scf_num_chans[u] && !slam_en) {
                num_cwords = dev_info[u]->num_cwords;
                w_words_p += num_cwords;
            }

            /* Decrement the counts.
             * - Observe: we are now using actual pt_entry_wsize and
             *            undo_entry_wsize
             */
            w_num_words = (num_cwords + pt_entry_wsize);
            wstate[u]->w_trans_count += req_trans_count;
            wstate[u]->w_msg_count += req_msg_count;
            wstate[u]->w_wal_ops_info_count += 1;
            wstate[u]->w_words_buf_count += w_num_words + num_words_skipped;

            if (req_undo_ops_info_count) {
                wstate[u]->avail_undo_ops_info_count -= req_undo_ops_info_count;
                wstate[u]->avail_undo_words_buf_count -= undo_entry_wsize;

                if (cfg_wal_stats_enable[u]) {
                    if (s_min_avail_undo_ops_info_count[u] >
                        wstate[u]->avail_undo_ops_info_count) {
                        s_min_avail_undo_ops_info_count[u] =
                            wstate[u]->avail_undo_ops_info_count;
                    }
                    if (s_min_avail_undo_words_buf_count[u] >
                        wstate[u]->avail_undo_words_buf_count) {
                        s_min_avail_undo_words_buf_count[u] =
                            wstate[u]->avail_undo_words_buf_count;
                    }
                }
            }

            /* WAL_WORDS_BUF - write dwords */
            if (bcmdrd_pt_attr_is_cam(u, sid)) {
                SHR_IF_ERR_EXIT( /* KM -> TCAM format conversion */
                    bcmptm_pt_tcam_km_to_xy(u, sid,
                                            entry_count,   /* 1 for !SLAM */
                                            entry_words,   /* km_entry */
                                            w_words_p)); /* xy_entry */
            } else {
                sal_memcpy(w_words_p, entry_words, 4 * pt_entry_wsize);
            }

            /* UNDO_OPS_INFO, UNDO_WORDS_BUF */
            w_undo_ops_info_idx = wstate[u]->n_undo_ops_info_idx;
            w_undo_ops_info_p = undo_ops_info[u] + w_undo_ops_info_idx;
            w_undo_words_buf_idx = wstate[u]->n_undo_words_buf_idx;
            if (req_undo_ops_info_count) {
                SHR_IF_ERR_EXIT(
                    undo_info_get(u, sid, pt_dyn_info,
                                  entry_count, undo_entry_wsize,

                                  undo_words_buf[u] + w_undo_words_buf_idx,
                                  w_undo_ops_info_p));
            }

            /* WAL_OPS_INFO */
            w_wal_ops_info_idx = wstate[u]->n_wal_ops_info_idx;
            w_wal_ops_info_p = bcmptm_wal_ops_info[u] + w_wal_ops_info_idx;
            w_wal_ops_info_p->op_wsize = w_num_words;
            w_wal_ops_info_p->op_ctrl_wsize = num_cwords;
            w_wal_ops_info_p->sid = sid;
            w_wal_ops_info_p->dyn_info = *pt_dyn_info;
            w_wal_ops_info_p->op_type = op_type;

            if (req_trans_count) {
                w_trans_idx = wstate[u]->n_trans_idx;
                w_msg_idx = wstate[u]->n_msg_idx;
            } else if (req_msg_count) {
                w_trans_idx = wstate[u]->c_trans_idx;
                w_msg_idx = wstate[u]->n_msg_idx;
            } else {
                w_trans_idx = wstate[u]->c_trans_idx;
                w_msg_idx = wstate[u]->c_msg_idx;
            }
            w_msg_p = bcmptm_wal_msg_bp[u] + w_msg_idx;
            if (req_msg_count) { /* new msg */
                /* WAL_MSG: init */
                w_msg_p->cmd = slam_en ? BCMPTM_WAL_MSG_CMD_SLAM
                                         : BCMPTM_WAL_MSG_CMD_WRITE;
                w_msg_p->words_buf_idx = w_words_buf_idx;
                w_msg_p->req_entry_wsize = w_num_words;
                w_msg_p->num_ops = slam_en ? entry_count : 1; /* Note */
                w_msg_p->ops_info_idx = w_wal_ops_info_idx;

                smc_word.entry = 0;
                switch (op_type) {
                    case BCMPTM_RM_OP_EM_NARROW:
                    case BCMPTM_RM_OP_EM_WIDE:
                        w_msg_p->need_cbf_before_msg = 1;

                        smc_word.f.entry_change_en = 1;
                        break;
                    case BCMPTM_RM_OP_SLICE_MODE_CHANGE:
                        w_msg_p->need_cbf_before_msg = 1;

                        SHR_NULL_CHECK(slice_mode_change_info, SHR_E_PARAM);

                        SHR_IF_ERR_EXIT(
                            ((slice_mode_change_info->slice_num0 > SLICE_MAX) ||
                             (slice_mode_change_info->slice_num1 > SLICE_MAX) ||
                             (slice_mode_change_info->slice_mode >= 8) ||
                             (slice_mode_change_info->slice_group_id >= 8)) ?
                            SHR_E_PARAM : SHR_E_NONE);

                        smc_word.f.slice_num0 = slice_mode_change_info->slice_num0;
                        smc_word.f.slice_num1 = slice_mode_change_info->slice_num1;
                        smc_word.f.slice_mode = slice_mode_change_info->slice_mode;
                        smc_word.f.slice_change_en = 1;
                        smc_word.f.slice_group_id = slice_mode_change_info->slice_group_id;
                        break;
                    default:
                        w_msg_p->need_cbf_before_msg = 0;
                        break;
                }
                w_msg_p->smc_word = smc_word;

                w_msg_p->undo_ops_info_idx = w_undo_ops_info_idx;
                w_msg_p->undo_words_buf_idx = w_undo_words_buf_idx;
                w_msg_p->undo_num_words = undo_entry_wsize;
                w_msg_p->trans_idx = w_trans_idx;
                w_msg_p->num_words_skipped = num_words_skipped;
                w_msg_p->umsg_idx = wstate[u]->n_umsg_idx;
            } else { /* we are in same msg (cannot be slam) */
                /* WAL_MSG: update */
                w_msg_p->req_entry_wsize += w_num_words;
                w_msg_p->num_ops += 1;

                if (op_type == BCMPTM_RM_OP_EM_WIDE ||
                    op_type == BCMPTM_RM_OP_EM_NARROW) {
                    w_msg_p->need_cbf_before_msg = 1;

                    smc_word = w_msg_p->smc_word; /* could be non-zero */
                    smc_word.f.entry_change_en = 1;
                    w_msg_p->smc_word = smc_word;
                }
                w_msg_p->undo_num_words += undo_entry_wsize;
                w_msg_p->num_words_skipped += num_words_skipped;
            } /* we are in same msg */

            /* WAL_TRANS */
            w_trans_p = wal_trans[u] + w_trans_idx;
            if (req_trans_count) { /* New Trans */
                w_trans_p->f_msg_idx = w_msg_idx; /* 1st msg of trans */
                w_trans_p->cseq_id = 0; /* Will be initialized with commit */

                w_trans_p->num_msgs = 1;
                w_trans_p->num_ops = 1; /* even for slam msg */
                w_trans_p->num_words = (w_num_words + num_words_skipped);
                w_trans_p->slam_msg_count = slam_en ? 1 : 0;
            } else {
                w_trans_p->num_msgs += req_msg_count;
                w_trans_p->num_ops += 1; /* even for slam msg */
                w_trans_p->num_words += (w_num_words + num_words_skipped);
                if (slam_en) {
                    w_trans_p->slam_msg_count += 1;
                }
            }

            /* Update wstate[u]->c_vars and wstate[u]->n_vars */
            if (req_trans_count) {
                wstate[u]->c_trans_idx = w_trans_idx;
                SHR_IF_ERR_EXIT(wstate_trans_idx_inc(u));
                wstate[u]->c_msg_commit_count = 0;
            }
            if (req_msg_count) {
                wstate[u]->c_msg_idx = w_msg_idx;
                SHR_IF_ERR_EXIT(wstate_msg_idx_inc(u));
                wstate[u]->n_umsg_idx += 1; /* ok to wrap */
            }
            SHR_IF_ERR_EXIT(
                wstate_wal_ops_info_idx_inc(u, &close_msg_after_this_op));
            SHR_IF_ERR_EXIT(
                wstate_words_buf_idx_inc(u, w_num_words,
                                         &close_msg_after_this_op));
            if (req_undo_ops_info_count) {
                SHR_IF_ERR_EXIT(
                    wstate_undo_ops_info_idx_inc(u, &close_msg_after_this_op));
                SHR_IF_ERR_EXIT(
                    wstate_undo_words_buf_idx_inc(u, undo_entry_wsize,
                                                  &close_msg_after_this_op));
            }
            if (slam_en) {
                close_msg_after_this_op = TRUE; /* Use dedicated msg for SLAM */
            }

            /* Update wstate[u]->trans_state */
            /* if (c_trans_state == IDLE or IDLE_AFTER_WR) {
             *     we allocated new msg and added one op to it
             *     c_trans_state = IN_MSG;
             * } else if (c_trans_state == NEED_NEW_MSG) {
             *     we allocated new msg and added one op to it
             *     c_trans_state = IN_MSG;
             * } else {
             *     we added one more op to existing msg
             *     c_trans_state = IN_MSG;
             * }
             */
            wstate[u]->c_trans_state = TRANS_STATE_IN_MSG;

            /* Update trans_state (again) if needed
             * This is the only way, we can get to NEED_NEW_MSG state */
            if (close_msg_after_this_op) {
                wstate[u]->c_trans_state = TRANS_STATE_NEED_NEW_MSG;
                print_msg_info(u, FALSE, wstate[u]->c_msg_idx, w_msg_p, "WR");
                if (wal_disable_undo[u]) {
                    SHR_IF_ERR_EXIT(wal_send_msg_b4tc(u, wstate[u]->c_msg_idx));
                }
                /* Note - wstate[u]->c_msg_idx is still pointing to last msg
                 *        and not to new msg. We have not allocated new msg
                 *        resource - we are only remembering that we will need
                 *        to start with new_msg when we get the next op
                 */

                /* We already updated wstate[u] 'next' values */
            }

            done = TRUE;
        } else { /* no space, wait */
            if (pending_trans_count1(u) && !req_trans_count) {
                /* No pending transactions in system (except for current trans),
                 * so there is no possibility of WAL reader thread releasing
                 * any space.
                 *
                 * Current transaction is not complete, and current WAL size
                 * cannot accommodate it.
                 *
                 * Summary: No need to wait, declare memory full immediately.
                 */
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                     "\n Current transaction cannot fit in WAL !! \n"
                     "avail_trans_count=%0u, req_trans_count=%0u, "
                     "avail_msg_count=%0u, req_msg_count=%0u, "
                     "avail_wal_ops_info_count=%0u, req_wal_ops_info_count=%0u, "
                     "avail_words_buf_count=%0u, max_pt_entry_wsize=%0u, "
                     "avail_undo_ops_info_count=%0u, req_undo_ops_info_count=%0u, "
                     "avail_undo_words_buf_count=%0u, undo_entry_wsize=%0u, "
                     "retry_count=%0d\n"),
                      avail_trans_count, req_trans_count,
                      avail_msg_count, req_msg_count,
                      avail_wal_ops_info_count, 1,
                      avail_words_buf_count, max_pt_entry_wsize,
                      wstate[u]->avail_undo_ops_info_count,
                      req_undo_ops_info_count,
                      wstate[u]->avail_undo_words_buf_count, undo_entry_wsize,
                      retry_count));
                print_wstate_info(u, FALSE, "WR ");
                wal_full = TRUE;
                break; /* from while with done = FALSE */
            }
            if (retry_count >= 10) {
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                     "\n"
                     "avail_trans_count=%0u, req_trans_count=%0u, "
                     "avail_msg_count=%0u, req_msg_count=%0u, "
                     "avail_wal_ops_info_count=%0u, req_wal_ops_info_count=%0u, "
                     "avail_words_buf_count=%0u, max_pt_entry_wsize=%0u, "
                     "avail_undo_ops_info_count=%0u, req_undo_ops_info_count=%0u, "
                     "avail_undo_words_buf_count=%0u, undo_entry_wsize=%0u, "
                     "retry_count=%0d\n"),
                      avail_trans_count, req_trans_count,
                      avail_msg_count, req_msg_count,
                      avail_wal_ops_info_count, 1,
                      avail_words_buf_count, max_pt_entry_wsize,
                      wstate[u]->avail_undo_ops_info_count,
                      req_undo_ops_info_count,
                      wstate[u]->avail_undo_words_buf_count, undo_entry_wsize,
                      retry_count));
                print_wstate_info(u, FALSE, "WR ");
            }
            sal_usleep(WAL_USLEEP_TIME << retry_count);
            retry_count++;
        } /* no space, wait */
    } while (!done && (retry_count < WAL_AVAIL_SPACE_RETRY_COUNT_MAX));
    if (!done) {
        if (wal_full) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Not enough space in WAL buffers for %s \n"),
                 slam_en ? "SLAM" : "WRITE"));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Timeout while waiting for space in WAL "
                            "buffers for %s \n"),
                 slam_en ? "SLAM" : "WRITE"));
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }

    if (bcmptm_wal_autocommit_enabled(u)) {
        if (w_trans_p->num_msgs == cfg_wal_max_msgs_in_trans[u]) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Coverage: Trans_idx = %0d Auto_Committed "
                            "due to trans_msg_max = %d \n"),
                            wstate[u]->c_trans_idx,
                            cfg_wal_max_msgs_in_trans[u]));
            SHR_IF_ERR_EXIT(
                bcmptm_wal_commit(u, flags, TRUE, wstate[u]->c_trans_idx,
                                  NULL, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
wal_send_empty_last_msg(int u)
{
    uint32_t w_msg_idx, retry_count, avail_msg_count;
    bcmptm_wal_msg_t *w_msg_p;
    wal_trans_t *w_trans_p;
    bcmptm_wal_slice_mode_info_t smc_word;
    bool done, wal_full = FALSE;
    SHR_FUNC_ENTER(u);

    done = FALSE;
    retry_count = 0;
    do {
        avail_msg_count = avail_count_get(u, wstate[u]->w_msg_count,
                                          wstate[u]->r_msg_count,
                                          cfg_wal_msg_max_count[u]);
        if (avail_msg_count) {
            wstate[u]->w_msg_count += 1;
            w_trans_p = wal_trans[u] + wstate[u]->c_trans_idx;

            w_msg_idx = wstate[u]->n_msg_idx;
            w_msg_p = bcmptm_wal_msg_bp[u] + w_msg_idx;

            /* WAL_MSG: init */
            w_msg_p->cmd = BCMPTM_WAL_MSG_CMD_EMPTY_WRITE;
            w_msg_p->words_buf_idx = 0;
            w_msg_p->req_entry_wsize = 0;
            w_msg_p->num_ops = 0;
            w_msg_p->ops_info_idx = 0;

            smc_word.entry = 0;
            w_msg_p->smc_word = smc_word;
            w_msg_p->undo_ops_info_idx = 0;
            w_msg_p->undo_words_buf_idx = 0;
            w_msg_p->undo_num_words = 0;
            w_msg_p->trans_idx = wstate[u]->c_trans_idx;
            w_msg_p->num_words_skipped = 0;
            w_msg_p->umsg_idx = wstate[u]->n_umsg_idx;

            w_msg_p->last_msg = TRUE; /* Note */

            /* WAL_TRANS: update */
            w_trans_p->num_msgs += 1;
            /* w_trans_p->num_ops += 0; */
            /* w_trans_p->num_words += 0; */
            /* w_trans_p->slam_msg_count += 0; */

            /* wstate */
            wstate[u]->c_msg_idx = w_msg_idx;
            SHR_IF_ERR_EXIT(wstate_msg_idx_inc(u));
            wstate[u]->n_umsg_idx += 1; /* ok to wrap */

            /* Now send the msg to walr - with last_msg TRUE */
            SHR_IF_ERR_EXIT(wal_send_msg_b4tc(u, wstate[u]->c_msg_idx));
            done = TRUE;
        } else { /* no space, wait */
            if (pending_trans_count1(u)) {
                /* We know wstate->c_trans_state = TRANS_STATE_NEED_NEW_MSG and
                 * so it is not TRANS_STATE_IDLE. */

                /* Above implies WAL reader has nothing to release */
                wal_full = TRUE;
                break; /* with done = FALSE */
                /* Above is unreachable because:
                 *  - wal_send_empty_last_msg() is called only because all
                 *    previous msgs of this trans are sent to walr.
                 *
                 *    If all these msgs have already been sent to HW, then we
                 *    cannot see avail_msg_count = 0.
                 *
                 *    If none of these msgs have been sent to HW, then there is
                 *    chance that walr will release at least one msg - so WAL
                 *    cannot be full.
                 *
                 *    If some msgs have not been sent to HW, then some msgs have
                 *    been sent - so WAL cannot be full.
                 */
            }
            if (retry_count >= 10) {
                LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(u,
                     "\n"
                     "avail_msg_count=%0u, req_msg_count=%0u, "
                     "retry_count=%0d\n"),
                      avail_msg_count, 1,
                      retry_count));
                print_wstate_info(u, FALSE, "WR ");
            }
            sal_usleep(WAL_USLEEP_TIME << retry_count);
            retry_count++;
        } /* no space, wait */
    } while (!done && (retry_count < WAL_AVAIL_SPACE_RETRY_COUNT_MAX));

    if (!done) {
        if (wal_full) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Not enough space in WAL msg buffer for %s \n"),
                 "EMPTY_WRITE msg"));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Timeout while waiting for space in msg buffer. \n")));
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/* Helps send all pending ops to HW.
 *
 * - Can be here in following 2 cases:
 *   Atomic trans enabled (disable_undo is F)
 *      - WAL in 2 thread mode.
 *      - for reads
 *
 *   Atomic trans disabled (disable_undo is T)
 *      - WAL is not bypassed (is in 2 thread mode).
 *      - for reads or for SERC interruption
 *
 * - Normally called by bcmptm_wal_read()
 * - Can also be called by bcmptm_wal_allow_serc_check()
 *
 * - What does this do?
 *      - Send all pending msgs (including current msg) to wal_reader. Note that
 *        the last msg will have 'last_msg' == TRUE, so wal_rdr will be
 *        clearing and freeing trans descriptor. We still expect commit.
 *      - Wait for WAL to drain (become empty)
 *      - Reset few vars - now that the WAL is empty.
 *      - Change the trans_state accordingly.
 */
static int wal_empty_make(int u, bool for_wal_read)
{
    wal_trans_t *c_trans_p;
    SHR_FUNC_ENTER(u);
    if (!for_wal_read) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "Entering WAL_empty_make with trans_state: %0d\n"),
             wstate[u]->c_trans_state));
    }

    switch (wstate[u]->c_trans_state) {
        case TRANS_STATE_NEED_NEW_MSG:
        case TRANS_STATE_IN_MSG:
            c_trans_p = wal_trans[u] + wstate[u]->c_trans_idx;

            if (wal_disable_undo[u] &&
                wstate[u]->c_trans_state == TRANS_STATE_NEED_NEW_MSG) {
                /* No pending msg to send. Must send 'last' msg to release
                 * trans descr. */
                SHR_IF_ERR_EXIT(
                    (c_trans_p->num_msgs != wstate[u]->c_msg_commit_count) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
                SHR_IF_ERR_EXIT(wal_send_empty_last_msg(u));
            } else {
                /* Send all messages for current transaction to wal rdr */
                SHR_IF_ERR_EXIT(wal_send_all_msgs(u, c_trans_p,
                                                  FALSE)); /* not resend */
            }
            if (for_wal_read) {
                wstate[u]->c_trans_state = TRANS_STATE_BLOCKING_READ_AFTER_WR;
            } else {
                wstate[u]->c_trans_state = TRANS_STATE_IDLE_AFTER_WR;
            }
            break;
        case TRANS_STATE_BLOCKING_READ:
            /* WAL is already empty. There were no writes before the read.
             * - We could be here for atomic_trans_enabled state.
             *   So, we cannot be here because of wal_allow_serc_check because
             *   interruption for SERC is supported only when atomic_trans is
             *   disabled.
             *
             * - We could be here when atomic_trans is disabled (we got HW read
             *   but have not received any commit for this trans yet).
             *   - We cannot be here from wal_allow_serc_check because
             *     this function is called at end of write_op, so state cannot
             *     be BLOCKING_READ.
             */
            SHR_IF_ERR_EXIT(!for_wal_read ? SHR_E_INTERNAL : SHR_E_NONE);

            /* No state change */
            break;

        case TRANS_STATE_BLOCKING_READ_AFTER_WR:
            /* WAL is already empty. There were HW_read after write. This is
             * possible when atomic trans are enabled as user may have packed
             * multiple read_from_hw in one transaction. Caller is
             * bcmptm_wal_read().
             *
             * We, however, cannot be here for atomic_trans_disabled case.
             * Single user entry operation can never result in writes followed
             * by reads from HW. */

            /* We also cannot be here because of wal_allow_serc_check because
             * this function is called at end of write_op, so state cannot
             * be BLOCKING_READ_AFTER_WR. */
            SHR_IF_ERR_EXIT(wal_disable_undo[u] || !for_wal_read ?
                            SHR_E_INTERNAL : SHR_E_NONE);

            /* No state change if we are here when atomic_trans is enabled. */
            break;
        case TRANS_STATE_IDLE_AFTER_WR:
            /* Means we were interrupted once and this means atomic trans were
             * disabled, So, we cannot be here when atomic transactions are
             * enabled.
             *
             * When atomic transactions are disabled:
             *
             * Tt means we had sent all ops to HW and changed state to IDLE_AFTER_WR.
             * WAL is already empty. Because atomic trans are disabled, we
             * cannot then see a read from HW (because trans cannot have
             * more than one entry op). Thus we cannot be here with
             * for_wal_read = TRUE.
             *
             * We also cannot be here because of 2nd wal_allow_serc_check as
             * this function is called at end of write_op, so state cannot
             * be IDLE_AFTER_WR.
             *
             * In short, we cannot be here.
             */
            SHR_ERR_EXIT(SHR_E_INTERNAL);

            wstate[u]->c_trans_state = TRANS_STATE_IDLE;
            break;
        default: /* TRANS_STATE_IDLE */
            /* WAL is already empty */

            /* We cannot be here because of wal_allow_serc_check because
             * this function is called at end of write_op, so state cannot
             * be IDLE. */
            /* We can be here only for_wal_read and enable_atomic_trans can
             * be 0 or 1 */
            SHR_IF_ERR_EXIT(!for_wal_read ? SHR_E_INTERNAL : SHR_E_NONE);
            wstate[u]->c_trans_state = TRANS_STATE_BLOCKING_READ;
            break;
    }

        /* Release prev_data(undo) resources - were needed only for Abort.
         * These are not released by cbf. */
        /* wstate-> restore n_idx */
        wstate[u]->n_undo_ops_info_idx = 0;
        wstate[u]->n_undo_words_buf_idx = 0;
        /* wstate-> restore avail counts */
        wstate[u]->avail_undo_ops_info_count = cfg_undo_ops_info_max_count[u];
        wstate[u]->avail_undo_words_buf_count = cfg_undo_words_buf_max_count[u];

        /* Wait for wal empty.
         * Following will also check for avail_undo_ops_info_count,
         * avail_undo_words_buf_count to be correct - hence they were released
         * above before calling wal_drain below. */
        
        SHR_IF_ERR_EXIT(bcmptm_wal_drain(u, FALSE));
            /* trans_state need not be IDLE even when wal becomes empty.
             * (see the state changes that we did in switch statement above) */

        /* Align word_buf_idx */
        wstate[u]->n_words_buf_idx = 0; /* WAL is empty, so simplify */

        /* Now this trans has 0 messages.
         * WAL reader thread has released all resources for this transaction and
         * also cleared contents of trans pointed by wstate->c_trans_idx (if
         * last_msg was asserted.
         *
         * Following notes when caller is bcmptm_wal_read(). Some comments also
         * apply when caller is bcmptm_wal_allow_serc_check().:
         *
         *      In fact, entire WAL is empty (all ops before BLOCKING_HW_READ were
         *      also sent).
         *
         *      - wstate->a_vars are all useless when in such state (because
         *      - wstate->c_trans_state was set to BLOCKING_READ earlier.
         *        everything has already been committed - nothing to undo)
         *      - Possibilities of what we can get after BLOCKING_HW_READ:
         *        - BLOCKING_HW_READ for trans when num_msgs = 0 (ok - this
         *        means we means we
         *        can support multiple BLOCKING_HW_READs in single transaction)
         *        - COMMIT for trans when num_msgs = 0 (ok)
         *        - wal_write for trans when num_msgs = 0 (NOT ok)
         *        - ABORT for trans when num_msgs = 0 (NOT ok - wal pushed all
         *        uncommitted ops to hw and it will be incorrect for rsrc_mgr to do
         *        abort - as it will leave HW and rsrc_mgr out of sync.
         *      - In summary, once user sends a BLOCKING_HW_READ, it can only send
         *      more BLOCKING_HW_READs or COMMIT. Everything else is ILLEGAL. This
         *      feature is intended for debug only.
         */
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_read(int u,
                uint64_t flags,
                bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *pt_dyn_info,
                bcmptm_misc_info_t *misc_info,
                size_t rsp_entry_wsize,

                uint32_t *rsp_entry_words)
{
/* - bcmptm_wal_read() is called only when DONT_USE_CACHE flag is set
 *
 * - Default behavior: All uncommitted ops, if any, will be committed,
 *   and we will wait for all ops to finish, issue PIO read,
 *   return read data to caller. Thus reads are BLOCKING by default.
 *
 * - Note: read op is executed 'in order' meaning read will be
 *   performed when all preceding ops in WAL have finished executing.
 */
    bool slam_en = FALSE, wo_aggr = FALSE;
    bool ser_failure = FALSE, retry_needed = FALSE;
    int tmp_rv;
    bcmptm_wo_aggr_info_t wo_aggr_info;
    SHR_FUNC_ENTER(u);

    /* Param check */
    SHR_NULL_CHECK(pt_dyn_info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    print_wstate_info(u, FALSE, "WAL_RD_REQ ");

    if (cfg_wal_bypass[u]) {
        if (wstate[u]->posted_scf_write) {
            /* A write was posted to HW and now we see req to read_from_HW.
             *
             * If we first call posted_wr_status_check() - it will clear
             * wstate->posted_scf_write.
             *
             * Subsequently:
             * 1. if we get abort - it is illegal - but we cannot detect this -
             *    because wstate->posted_scf_write was cleared..
             *
             * 2. if we get request to enable atomic_trans dynamically, we
             *    cannot catch that this enable is being requested in middle of
             *    trans.
             *
             * Simple way to fix above problems is to first set
             * wstate->sent_pio_write and then call
             * posted_wr_status_check().
             */

            /* first (because status_check may exit with error */
            wstate[u]->sent_pio_write = TRUE;

            /* then - check status of posted writes (byp_scf) */
            SHR_IF_ERR_EXIT(posted_wr_status_check(u, 0));
        }
    } else { /* normal 2 thread mode - atomic_trans_enable can be 0 or 1 */
        /* req_dwords needed for this entry - none for read */

        /* undo_words needed for this entry - none for read */
        SHR_IF_ERR_EXIT(wal_empty_make(u, TRUE)); /* for_wal_read */
    } /* normal mode */

    /* Determine if sid is wo_aggr */
    sal_memset(&wo_aggr_info, 0, sizeof(bcmptm_wo_aggr_info_t));
    tmp_rv = bcmptm_pt_wo_aggr_info_get(u, sid, &wo_aggr_info);
    switch (tmp_rv) {
    case SHR_E_NONE:
        SHR_IF_ERR_EXIT(wo_aggr_info.n_sid_count < 2 ?
                        SHR_E_INTERNAL : SHR_E_NONE);
        SHR_NULL_CHECK(wo_aggr_info.n_sid_list, SHR_E_INTERNAL); /* is not NULL */
        wo_aggr = TRUE;
        break;
    case SHR_E_UNAVAIL:
        wo_aggr = FALSE;
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(tmp_rv);
        break;
    }
    slam_en = misc_info && misc_info->dma_enable && dma_avail[u];
    if (wo_aggr) {
        SHR_IF_ERR_EXIT(slam_en ? SHR_E_PARAM : SHR_E_NONE);
        tmp_rv =
            bcmptm_wo_aggr_read_hw(u, flags, sid, pt_dyn_info, NULL,
                                   &wo_aggr_info, rsp_entry_wsize,
                                   rsp_entry_words, &ser_failure, &retry_needed);
    } else { /* Normal flow */
        if (slam_en) {
            tmp_rv = wal_dma_read(u,
                                  sid,
                                  pt_dyn_info,
                                  misc_info->dma_entry_count,
                                  rsp_entry_words,
                                  rsp_entry_wsize);
        } else {
            /* Issue read from HW (pio) */
            tmp_rv = bcmbd_pt_read(u,
                                   sid,
                                   pt_dyn_info,
                                   NULL,
                                   rsp_entry_words, /* read_data */
                                   rsp_entry_wsize);
        }
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
            ser_failure = TRUE;
            if (bcmptm_pt_ser_resp(u, sid) != BCMDRD_SER_RESP_NONE) {
                retry_needed = TRUE;
            }
        }
    } /* Normal flow */

    if (ser_failure) {
        if (!retry_needed) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Will not retry failed mreq read\n")));
        } else {
            /* If read completed but with error then let this be retried - BUT at
             * mreq level - as we may be blocking SERC from doing correction. */
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }
    SHR_IF_ERR_EXIT(tmp_rv);

    /* TCAM format -> KM conversion */
    if (bcmdrd_pt_attr_is_cam(u, sid) &&
        !(flags & BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM)
       ) {
        SHR_IF_ERR_EXIT(
            bcmptm_pt_tcam_xy_to_km(u, sid,
                                    slam_en ? misc_info->dma_entry_count : 1,
                                    rsp_entry_words,   /* xy_entry */
                                    rsp_entry_words)); /* km_entry */
        /* Note: we are requesting called func to REPLACE xy words
         * with km words to save on un-necessary copy from km words array
         * back to rsp words array.
         */
    }

    if (bcmptm_wal_autocommit_enabled(u)) {
        wstate[u]->c_trans_state = TRANS_STATE_IDLE;
        wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
        wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
        wstate[u]->c_msg_commit_count = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Can be here when enable_atomic_trans = 0 or 1, but not when cfg_wal_bypass = 1 */
int
bcmptm_wal_commit(int u,
                  uint64_t flags,
                  bool mreq_state_in_trans,
                  uint32_t in_cseq_id,
                  bcmptm_trans_cb_f notify_fn_p,
                  void *notify_info)
{
    
    wal_trans_t *c_trans_p;
    SHR_FUNC_ENTER(u);

    switch (wstate[u]->c_trans_state) {
        case TRANS_STATE_NEED_NEW_MSG:
        case TRANS_STATE_IN_MSG:
            c_trans_p = wal_trans[u] + wstate[u]->c_trans_idx;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Committing cseq_id=%0d, trans_idx=%0d\n"),
                 in_cseq_id, wstate[u]->c_trans_idx));

            if (!bcmptm_wal_autocommit_enabled(u)) { /* normal mode */
                SHR_IF_ERR_EXIT(!mreq_state_in_trans ?
                                SHR_E_INTERNAL : SHR_E_NONE);
            } /* else {
                 auto-commit mode
            } */

            /* Record notify info */
            c_trans_p->cseq_id = in_cseq_id;
            c_trans_p->notify_fn_p = notify_fn_p;
            c_trans_p->notify_info = notify_info;

            if (wal_disable_undo[u] &&
                wstate[u]->c_trans_state == TRANS_STATE_NEED_NEW_MSG) {
                /* No pending msg to send. Must send 'last' msg. */
                SHR_IF_ERR_EXIT(
                    (c_trans_p->num_msgs != wstate[u]->c_msg_commit_count) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
                SHR_IF_ERR_EXIT(wal_send_empty_last_msg(u));
            } else {
                /* Send all messages for current transaction to wal rdr */
                SHR_IF_ERR_EXIT(wal_send_all_msgs(u, c_trans_p, FALSE));
            }

            /* Reset c_trans related wstate vars */
            wstate[u]->c_trans_state = TRANS_STATE_IDLE;
            wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
            wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
            wstate[u]->c_msg_commit_count = 0;

            /* Release prev_data(undo) resources - were needed only for Abort */
            /* wstate: restore n_idx */
            wstate[u]->n_undo_ops_info_idx = 0;
            wstate[u]->n_undo_words_buf_idx = 0;
            /* wstate: restore avail counts */
            wstate[u]->avail_undo_ops_info_count =
                cfg_undo_ops_info_max_count[u];
            wstate[u]->avail_undo_words_buf_count =
                cfg_undo_words_buf_max_count[u];
            break;
        case TRANS_STATE_BLOCKING_READ:
        case TRANS_STATE_BLOCKING_READ_AFTER_WR:
            /* All write ops before blocking read, if any, were already sent to
             * HW.
             * All resources (msgs, trans) were already released.
             * Trans was already zeroed, we cannot check cseq_id */
            

            /* Reset c_trans related wstate vars */
            wstate[u]->c_trans_state = TRANS_STATE_IDLE;
            wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
            wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
            wstate[u]->c_msg_commit_count = 0;

            if (notify_fn_p) {
                notify_fn_p(in_cseq_id, SHR_E_NONE, notify_info);
            }
            break;
        default: /* TRANS_STATE_IDLE, IDLE_AFTER_WR */
            /* In auto-commit mode, if we get commit from LTM or u-test
             * just ignore the commit request */

            if (wstate[u]->c_trans_state != TRANS_STATE_IDLE &&
                wstate[u]->c_trans_state != TRANS_STATE_IDLE_AFTER_WR) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            if (mreq_state_in_trans || wal_disable_undo[u]) {
                /* For enable_atomic_trans = 0, 1 with mreq_state_in_trans = 1
                 * means:
                 * Even though there were no WAL ops in cur trans, there
                 * were READ, LOOKUP, GET_FIRST, GET_NEXT - so we
                 * return notification immediately if requested (will
                 * be out of order w.r.t what comes from WAL reader.
                 *
                 * For enable_atomic_trans = 0 with mreq_state_in_trans = 0
                 * case, as there is no support for crash, this cannot be
                 * crash_recommit and hence this is IMM case. Send notification.
                 *
                 * For enable_atomic_trans = 0 with mreq_state_in_trans == 1 and
                 * state is IDLE_AFTER_WR, WAL is currently empty - but there
                 * were write ops in this transaction and this sequence was
                 * interrupted by SERC and we never got any subsequent writes.
                 * Even in this case send notification.
                 */

                /* Call notification function */
                if (notify_fn_p) {
                    notify_fn_p(in_cseq_id, SHR_E_NONE, notify_info);
                }
            } else { /* enable_atomic_trans = 1 but with mreq_state IDLE */
                /* Even though there were no PTM/WAL ops in cur trans, there may
                 * have been ops for IMM tables.
                 * Also, this could be crash recovery case.
                 * One way to know if this is crash recovery: search for
                 * in_cseq_id in list of pending WAL trans. If it does not exist
                 * then this is IMM case else this is re-commit for previous
                 * trans.
                 *
                 * At present, interruption of trans is not supported when
                 * enable_atomic_trans = 1, so we cannot be here with
                 * IDLE_AFTER_WR state.
                 */

                SHR_IF_ERR_EXIT(
                    (wstate[u]->c_trans_state != TRANS_STATE_IDLE) ?
                    SHR_E_INTERNAL : SHR_E_NONE);
                SHR_IF_ERR_EXIT(
                    wal_trans_id_unknown(u, in_cseq_id, notify_fn_p,
                                         notify_info));
            }
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/* Can be here when enable_atomic_trans = 0 / 1,
 * but not when cfg_wal_bypass = 1 */
int
bcmptm_wal_abort(int u,
                 uint64_t flags,
                 uint32_t in_cseq_id)
{
    wal_trans_t *c_trans_p;
    bcmptm_wal_msg_t *f_wal_msg_p;
    uint32_t num_msgs_from_f_to_max;

    SHR_FUNC_ENTER(u);

    switch (wstate[u]->c_trans_state) {
        case TRANS_STATE_BLOCKING_READ:
            /* This means prev cmd was BLOCKING_HW_READ and no writes were sent
             * to HW before the blocking read.
             *
             * Also, we know that TRM will issue Abort - even if result of
             * result of LOOKUP(from HW or from PTcache) is NOT_FOUND.
             * Thus we can be in BLOCKING_READ state and get an Abort - and this
             * is not error. We can treat this as NOP.
             *
             * Q: If PTRM gets LOOKUP with 'read from HW', then it will first
             * search entry in PTcache and if it does not exist, it will not
             * issue read to HW - so how can WAL be in BLOCKING_READ state and
             * get abort?
             * A: Consider a successful lookup to PTRM with 'read from HW' -
             * this gets WAL in BLOCKING_READ state. Now TRM issues lookup for
             * indexed LT which fails - and then TRM issues Abort which also
             * comes to PTM. Now WAL is in BLOCKING_READ state and PTM gets
             * abort. We cannot get into this case by doing cmd line INSERT,
             * LOOKUP - because these are single entry trans. To exercise this
             * case we need single trans with:
             * INSERT, LOOKUP from HW - for PTRM keyed table
             * LOOKUP for non-existent key in indexed table (LTM managed)

             * We must do cleanup of trans_state - because we will not get any
             * commit for this trans.
             */
            wstate[u]->c_trans_state = TRANS_STATE_IDLE;
            wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
            wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
            wstate[u]->c_msg_commit_count = 0;

            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Ignored ABORT for trans with "
                            "BLOCKING_HW_READ \n")));
            break;
        case TRANS_STATE_BLOCKING_READ_AFTER_WR:
            /* This means prev cmd was BLOCKING_HW_READ and some writes were
             * already sent to HW before the blocking read.
             *
             * In this case we had already committed all prev write ops, sent
             * them to HW and also all resources including msg, trans have been already
             * released. There is nothing to release.
             *
             * Issuing abort is ILLEGAL (even for failed LOOKUP) - because WAL
             * cannot UNDO ops which were already sent to HW before the blocking
             * read.
             *
             * We must do cleanup of trans_state - because we will not get any
             * commit for this trans.
             */
            wstate[u]->c_trans_state = TRANS_STATE_IDLE;
            wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
            wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
            wstate[u]->c_msg_commit_count = 0;

            /* If atomic trans are disabled, we cannot get into
             * blocking_read_after_write state.
             * Irrespective of this, getting abort in this state is illegal. */
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Abort received when WAL state is "
                            "BLOCKING_READ_AFTER_WR (%0d), "
                            "enable_atomic_trans = %0d\n"),
                 wstate[u]->c_trans_state, wal_disable_undo[u] ? 0 : 1));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
            break;
        case TRANS_STATE_IDLE_AFTER_WR:
            /* This can happen only when atomic_trans were disabled: we had
             * sent some writes to HW, gave chance to SERC, and now seeing the
             * abort.
             *
             * In this case we had already committed all prev write ops, sent
             * them to HW and also all resources including msg, trans have been already
             * released. There is nothing to release.
             *
             * Issuing abort is ILLEGAL (even for failed LOOKUP) - because WAL
             * cannot UNDO ops which were already sent to HW before the blocking
             * read.
             *
             * We must do cleanup of trans_state - because we will not get any
             * commit for this trans.
             */
            wstate[u]->c_trans_state = TRANS_STATE_IDLE;
            wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
            wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
            wstate[u]->c_msg_commit_count = 0;

            /* atomic_trans disabled, 2 thread mode */
            SHR_IF_ERR_EXIT(!wal_disable_undo[u] ? SHR_E_INTERNAL : SHR_E_NONE);
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "Abort received when WAL state is "
                            "IDLE_AFTER_WR (%0d), enable_atomic_trans = 0\n"),
                 wstate[u]->c_trans_state));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
            break;
        case TRANS_STATE_IN_MSG:
        case TRANS_STATE_NEED_NEW_MSG:
            c_trans_p = wal_trans[u] + wstate[u]->c_trans_idx;
            f_wal_msg_p = bcmptm_wal_msg_bp[u] + c_trans_p->f_msg_idx;

            if (wal_disable_undo[u]) {
                /* We are in 2 thread mode - so we could have already sent few
                 * msgs, ops to reader side - and now we got abort.
                 * Must report error but how to cleanup WAL state and release
                 * WAL resources for ops that were already sent to reader-side?
                 *
                 * Solution:
                 * If WAL was bypassed using PIO, for example, at present all
                 * ops would have been sent to HW and resources were all freed.
                 *
                 * For 2 thread mode, we can do the same. Pretend that we got
                 * commit - send pending ops, if any to HW,
                 *        - send 'empty last' msg if needed to free resources,
                 *        - cleanup c_trans related wstate vars
                 *        - wait for WAL empty
                 *        - and then declare error.
                 */

                /* Error message */
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(u, "Abort received when WAL state is "
                                "not IDLE (%0d), enable_atomic_trans = 0\n"),
                     wstate[u]->c_trans_state));

                /* For abort case - no callback notify_fn */
                c_trans_p->notify_fn_p = NULL;

                /* Send remaining ops, if any - as if we got commit */
                if (wstate[u]->c_trans_state == TRANS_STATE_NEED_NEW_MSG) {
                    /* No pending msg to send. Must send 'last' msg. */
                    SHR_IF_ERR_EXIT(
                        c_trans_p->num_msgs != wstate[u]->c_msg_commit_count ?
                        SHR_E_INTERNAL : SHR_E_NONE);
                    SHR_IF_ERR_EXIT(wal_send_empty_last_msg(u));
                } else { /* IN_MSG */
                    /* Send last message with pending ops */
                    SHR_IF_ERR_EXIT(
                        wal_send_all_msgs(u, c_trans_p, FALSE));
                }

                /* Reset c_trans related wstate vars. */
                wstate[u]->c_trans_state = TRANS_STATE_IDLE;
                wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
                wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
                wstate[u]->c_msg_commit_count = 0;

                /* Undo related wstate vars were already correct because we
                 * were operating in atomic_trans disabled mode. But to be
                 * conservative. */
                wstate[u]->n_undo_ops_info_idx = 0;
                wstate[u]->n_undo_words_buf_idx = 0;
                /* wstate: restore avail counts */
                wstate[u]->avail_undo_ops_info_count =
                    cfg_undo_ops_info_max_count[u];
                wstate[u]->avail_undo_words_buf_count =
                    cfg_undo_words_buf_max_count[u];

                /* Wait for wal_empty - end_state must be IDLE */
                SHR_IF_ERR_EXIT(bcmptm_wal_drain(u, TRUE));

                /* Return error */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else { /* atomic_trans enabled case */
                SHR_IF_ERR_EXIT(wal_ptcache_undo(u, c_trans_p));

                /* wstate: restore n_idx */
                wstate[u]->n_trans_idx = wstate[u]->c_trans_idx;
                wstate[u]->n_msg_idx = wstate[u]->a_msg_idx;
                wstate[u]->n_wal_ops_info_idx = wstate[u]->a_wal_ops_info_idx;
                wstate[u]->n_words_buf_idx = wstate[u]->a_words_buf_idx;
                wstate[u]->n_undo_ops_info_idx = 0;
                wstate[u]->n_undo_words_buf_idx = 0;
                wstate[u]->n_umsg_idx = wstate[u]->a_umsg_idx;

                /* wstate[u]-> restore avail counts */
                wstate[u]->w_trans_count = wstate[u]->a_w_trans_count;
                wstate[u]->w_msg_count = wstate[u]->a_w_msg_count;
                wstate[u]->w_wal_ops_info_count =
                    wstate[u]->a_w_wal_ops_info_count;
                wstate[u]->w_words_buf_count = wstate[u]->a_w_words_buf_count;
                wstate[u]->avail_undo_ops_info_count =
                    cfg_undo_ops_info_max_count[u];
                wstate[u]->avail_undo_words_buf_count =
                    cfg_undo_words_buf_max_count[u];

                /* wstate[u]-> c_vars */
                wstate[u]->c_trans_state = TRANS_STATE_IDLE;
                wstate[u]->c_trans_idx = C_TRANS_IDX_ILL; /* x */
                wstate[u]->c_msg_idx = C_MSG_IDX_ILL;     /* x */
                wstate[u]->c_msg_commit_count = 0;

                /* wstate[u]->a_idx */
                wstate[u]->a_msg_idx = (uint32_t)(-1);
                wstate[u]->a_wal_ops_info_idx = (uint32_t)(-1);
                wstate[u]->a_words_buf_idx = (uint32_t)(-1);

                /* Clean all wal_msg - must take into account wraparound. */
                num_msgs_from_f_to_max =
                    cfg_wal_msg_idx_max[u] - c_trans_p->f_msg_idx + 1;

                if (num_msgs_from_f_to_max > c_trans_p->num_msgs) {
                    /* Normal case - there is no wraparound. */
                    sal_memset(f_wal_msg_p, 0,
                               sizeof(bcmptm_wal_msg_t) * c_trans_p->num_msgs);
                } else { /* wraparound case */
                    uint32_t num_msgs_till_last =
                        c_trans_p->num_msgs - num_msgs_from_f_to_max;

                    /* First clear msgs up to max_msg_idx */
                    sal_memset(f_wal_msg_p, 0,
                               sizeof(bcmptm_wal_msg_t) * num_msgs_from_f_to_max);

                    /* Then clear msgs from msgs_idx = 0 up to last_msg_idx */
                    sal_memset(bcmptm_wal_msg_bp[u], 0,
                               sizeof(bcmptm_wal_msg_t) * num_msgs_till_last);
                }

                /* Finally clean trans */
                sal_memset(c_trans_p, 0, sizeof(wal_trans_t));
            }
            break; /* NEED_NEW_MSG, IN_MSG */

        default: /* TRANS_STATE_IDLE */
            /* Nothing to release */
            SHR_IF_ERR_EXIT(
                (wstate[u]->c_trans_state != TRANS_STATE_IDLE ? SHR_E_INTERNAL
                                                              : SHR_E_NONE));
            /* Ignore Abort even for enable_atomic_trans = 0 case */
            break;
    } /* trans_state */

exit:
    /* ALWAYS re-enable wal_abort_error checking once we see a commit, abort */
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_serc_data_find(int u,
                          uint64_t flags,
                          bcmdrd_sid_t req_sid,
                          bcmbd_pt_dyn_info_t *req_dyn_info,
                          size_t rsp_entry_wsize,
                          int req_slice_num,
                          bcmptm_rm_slice_mode_t req_slice_mode,

                          uint32_t *rsp_entry_words)
{
    SHR_FUNC_ENTER(u);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(u, "====== Did not find matching entry in WAL\n")));

    SHR_ERR_EXIT(SHR_E_UNAVAIL); /* restore data not in WAL */
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_drain(int u, bool end_check)
{
    int retry_count;
    bool empty = FALSE;
    wal_trans_state_t wal_c_trans_state;
    SHR_FUNC_ENTER(u);

    if (bcmptm_wal_autocommit_enabled(u)) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(u, "In wal drain \n")));

        /* We could have uncommitted messages, so commit these first */
        if (wstate[u]->c_trans_state == TRANS_STATE_IN_MSG ||
            wstate[u]->c_trans_state == TRANS_STATE_NEED_NEW_MSG) {
            SHR_IF_ERR_EXIT(
                bcmptm_wal_commit(u, 0, TRUE, wstate[u]->c_trans_idx, NULL,
                                  NULL));
            /* Note: Sending 0 as flags, mreq_state_in_trans = TRUE.
             *
             *       Above code is for 'fake' TRANS_AUTO_COMMIT mode.
             *       In normal case, wal_drain  does not call
             *       bcmptm_wal_commit
             */
        }
    }

    /* Note: wal_drain could be called when reverting from dynamic_atomic_trans
     * to WAL_BYPASS mode, so just checking of posted_write_status is not
     * good enough. We could have write ops pending from the
     * dynamic_atomic_trans.
     *
     * In summary, we need to check for posted_write_status and also 2 thread
     * mode WAL vars - to know if WAL is drained.
     */

    /* 1 thread bypass mode checking. */
    SHR_IF_ERR_EXIT(posted_wr_status_check(u, 0));

    /* 2 thread mode vars check */
    retry_count = 0;
    wal_c_trans_state = wstate[u]->c_trans_state;
    do {
        SHR_IF_ERR_EXIT(
            wal_empty_check(u,
                            (retry_count == WAL_EMPTY_CHECK_RETRY_COUNT),
                            /* show_not_empty_values */
                            &empty, &wal_c_trans_state));
        if (empty) {
            if (wal_c_trans_state != TRANS_STATE_IDLE) {
                if (end_check) {
                    LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(u, "WAL is empty, but wal_c_trans_state "
                                    "is not IDLE (it is %0d) !!\n"),
                         wal_c_trans_state));
                    SHR_ERR_EXIT(SHR_E_FAIL);
                } else {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(u, "WAL is empty, but wal_c_trans_state "
                                    "is not IDLE (it is %0d) !!\n"),
                         wal_c_trans_state));
                }
            }
            break;
        }
        if (retry_count > WAL_EMPTY_CHECK_RETRY_COUNT) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(u, "WAL not empty !!\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "Waiting for WAL to become empty\n")));
            sal_usleep(WAL_EMPTY_CHECK_USLEEP_TIME << retry_count);
            retry_count++;
        }
    } while (retry_count <= WAL_EMPTY_CHECK_RETRY_COUNT);
exit:
    SHR_FUNC_EXIT();
}

/* Register bypass mode related wal functions */
static void
wal_fn_register_bypass(int u)
{
    if (bcmptm_wal_scf_num_chans[u]) {
        /* Use schan_fifo as fast_dedicated_schan_pio */
        cfg_wal_fn_ptr_all[u].fn_wal_write = &wal_write_byp_scf;
        cfg_wal_fn_ptr_all[u].fn_wal_trans_cmd = &wal_trans_cmd_byp;
        cfg_wal_fn_ptr_all[u].fn_wal_mreq_state_update =
            &wal_mreq_state_update_byp;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "\nWill work in WAL_Bypass_SCF (single thread"
                        ") mode, flags = %0x\n"),
              cfg_scf_init_flags[u]));
    } else { /* no scf channels */
        cfg_wal_fn_ptr_all[u].fn_wal_write = &wal_write_byp_pio;
        cfg_wal_fn_ptr_all[u].fn_wal_trans_cmd = &wal_trans_cmd_byp;
        cfg_wal_fn_ptr_all[u].fn_wal_mreq_state_update =
            &wal_mreq_state_update_byp;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "\nWill work in WAL_Bypass_PIO (single thread"
                        ") mode\n")));
    }
}

/* NOTE:
 *     - bcmptm_wal_mode_sel() must be called before bcmptm_wal_init()
 */
int
bcmptm_wal_init(int u, bool warm)
{
    int tmp_rv, num_chans = 0;
    size_t cmd_mem_wsize = 0;
    SHR_FUNC_ENTER(u);

    /* Get general device info */
    SHR_IF_ERR_EXIT(bcmptm_dev_info_get(u, &dev_info[u]));

    /* cfg_wal_mode[u] = 0; */
        /* By default, cfg_wal_mode[u] will be 0.
         * User must call bcmptm_wal_mode_sel() before init
         * to choose one of the auto-commit modes */

    SHR_IF_ERR_EXIT(bcmptm_wal_cfg_set(u));
        /* will adjust numbers as per wal_mode */

    dma_avail[u] = !bcmdrd_feature_enabled(u, BCMDRD_FT_PASSIVE_SIM);
        /* DMA is supported by BCMSIM, EMUL, HW */

    /* schanfifo driver init */
    if (cfg_scf_num_chans[u] && !bcmdrd_feature_is_sim(u)) {
        tmp_rv = bcmbd_schanfifo_info_get(u, &num_chans, &cmd_mem_wsize);
        if (tmp_rv == SHR_E_NONE) {
            SHR_IF_ERR_EXIT(num_chans <= 0 ? SHR_E_INTERNAL : SHR_E_NONE);
            if ((uint32_t)num_chans > cfg_scf_num_chans[u]) {
                bcmptm_wal_scf_num_chans[u] = cfg_scf_num_chans[u];
            } else { /* Use HW defined num_channels */
                bcmptm_wal_scf_num_chans[u] = num_chans;
            }
            if (cfg_wal_bypass[u]) {
                cfg_scf_init_flags[u] &= ~SCHANFIFO_IF_CCMDMA_WR;
            }
            SHR_IF_ERR_EXIT(
                bcmbd_schanfifo_init(u,
                                     cfg_scf_init_max_polls[u],
                                     cfg_scf_init_flags[u]));

            SHR_IF_ERR_EXIT(!cmd_mem_wsize ? SHR_E_INIT : SHR_E_NONE);
            if (bcmptm_wal_scf_num_chans[u] && /* schan_fifo use is enabled */
                cmd_mem_wsize < cfg_wal_max_words_in_msg[u]) {
                /* Use HW defined cmd_mem_wsize */
                cfg_wal_max_words_in_msg[u] = cmd_mem_wsize;
            }

            /* Make words_buf_max_count multiple of 4, but don't exceed
             * user-limit (irrespective of whether schan_fifo use is enabled or
             * not). */
            cfg_wal_words_buf_max_count[u] &= 0xFFFFFFFC;
            SHR_IF_ERR_EXIT(!cfg_wal_words_buf_max_count[u] ?
                            SHR_E_INTERNAL : SHR_E_NONE);
        } /* else - scf is not avail */
    }

    /* Allocate WAL mem */
    SHR_IF_ERR_EXIT(wal_alloc(u, warm));

    wstate_mutex[u] = sal_mutex_create("WAL_WSTATE_LOCK");
    SHR_NULL_CHECK(wstate_mutex[u], SHR_E_MEMORY);
    have_wstate_mutex[u] = FALSE;
    wal_dyn_enable_atomic_trans[u] = FALSE;

    if (!warm) {
        wstate_init(u);
    }

    /* All of these become meaningful only when wal_send_all_msgs()
     * is called */
    bcmptm_wal_msg_dq_idx[u] = 0;
    bcmptm_wal_msg_dq_p[u] = NULL;
    bcmptm_wal_msg_dq_ready_p[u] = NULL;
    wal_msg_dq_enable[u] = FALSE;

    /* Initialize stats */
    s_min_avail_trans_count[u] = cfg_wal_trans_max_count[u];
    s_min_avail_msg_count[u] = cfg_wal_msg_max_count[u];
    s_min_avail_ops_info_count[u] = cfg_wal_ops_info_max_count[u];
    s_min_avail_words_buf_count[u] = cfg_wal_words_buf_max_count[u];
    s_min_avail_undo_ops_info_count[u] = cfg_undo_ops_info_max_count[u];
    s_min_avail_undo_words_buf_count[u] = cfg_undo_words_buf_max_count[u];

    s_max_words_in_msg[u] = 0;
    s_max_ops_in_msg[u] = 0;
    s_max_msgs_in_trans[u] = 0;
    s_max_slam_in_trans[u] = 0;

    if (cfg_wal_bypass[u]) {
        wal_fn_register_bypass(u);
    } else {
        cfg_wal_fn_ptr_all[u].fn_wal_write = &wal_write_normal;
        cfg_wal_fn_ptr_all[u].fn_wal_trans_cmd = &wal_trans_cmd_normal;
        cfg_wal_fn_ptr_all[u].fn_wal_mreq_state_update =
            &wal_mreq_state_update;
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(u, "\nWill work in WAL_Two_Thread (normal) mode\n")));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_cleanup(int u)
{
    int i;
    SHR_FUNC_ENTER(u);

    SHR_IF_ERR_EXIT(wal_free(u));

    SHR_IF_ERR_EXIT(bcmptm_wal_stats_info_enable(u, FALSE));
    SHR_IF_ERR_EXIT(bcmptm_wal_stats_info_clear(u));

    dev_info[u] = NULL;
    sal_mutex_destroy(wstate_mutex[u]);
    have_wstate_mutex[u] = FALSE;

    wal_dyn_enable_atomic_trans[u] = FALSE;

    cfg_wal_mode[u] = 0;

    for (i = 0; i < BCMPTM_RM_MC_GROUP_COUNT_MAX; i++) {
        mc_count[u][i] = 0;
    }
    wal_disable_undo[u] = FALSE;

    wal_msg_dq_enable[u] = FALSE;
    dma_avail[u] = FALSE;
    words_buf_mem_type[u] = WAL_MEM_TYPE_NONE;

    bcmptm_wal_msg_dq_ready_p[u] = NULL;
    bcmptm_wal_msg_dq_idx[u] = 0;
    bcmptm_wal_msg_dq_p[u] = NULL;
    /* WAL reader cannot start draining the 'committed' msgs as soon as
     * wal_rdr thread is enabled - WAL writer must wake up reader only
     * AFTER it initializes the msg_dq_idx, msg_dq_p, msg_dq_ready_p.
     *
     * For crash-recovery cases this allows WAL writer a chance to
     * re-initialize 'walr_dq_idx' and re-commit the messages when it gets
     * 're-commit for old transaction' from TRM.
     */

    /* schanfifo_cleanup will happen as part of schanfifo_detach, so no
     * need for WAL to call this.
     */
    bcmptm_wal_scf_num_chans[u] = 0;

    cfg_wal_bypass[u] = FALSE;
    prev_cfg_wal_bypass[u] = FALSE;

    cfg_wal_fn_ptr_all[u].fn_wal_write = NULL;
    cfg_wal_fn_ptr_all[u].fn_wal_trans_cmd = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_stats_info_enable(int u, bool enable)
{
    SHR_FUNC_ENTER(u);
    cfg_wal_stats_enable[u] = enable;
/* exit: */
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_stats_info_get(int u, bcmptm_wal_stats_info_t *info_p)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(info_p, SHR_E_PARAM);
    /*
     * Possible that WAL Reader thread is modifying RHS values when this
     * function is called. This is ok - we are only looking at the vars and not
     * modifying them - no need for this function to take lock.
     */
    info_p->stats_enabled = cfg_wal_stats_enable[u];

    info_p->max_trans_count = cfg_wal_trans_max_count[u] -
                              s_min_avail_trans_count[u];

    info_p->max_msg_count = cfg_wal_msg_max_count[u] -
                            s_min_avail_msg_count[u];

    info_p->max_op_count = cfg_wal_ops_info_max_count[u] -
                           s_min_avail_ops_info_count[u];

    info_p->max_word_count = cfg_wal_words_buf_max_count[u] -
                             s_min_avail_words_buf_count[u];

    info_p->max_undo_op_count = cfg_undo_ops_info_max_count[u] -
                                s_min_avail_undo_ops_info_count[u];

    info_p->max_undo_word_count = cfg_undo_words_buf_max_count[u] -
                                  s_min_avail_undo_words_buf_count[u];

    info_p->max_words_in_msg = s_max_words_in_msg[u];
    info_p->max_ops_in_msg = s_max_ops_in_msg[u];
    info_p->max_msgs_in_trans = s_max_msgs_in_trans[u];
    info_p->max_slam_msgs_in_trans = s_max_slam_in_trans[u];

    info_p->cfg_trans_count = cfg_wal_trans_max_count[u];
    info_p->cfg_msg_count = cfg_wal_msg_max_count[u];
    info_p->cfg_op_count = cfg_wal_ops_info_max_count[u];
    info_p->cfg_word_count = cfg_wal_words_buf_max_count[u];
    info_p->cfg_undo_op_count = cfg_undo_ops_info_max_count[u];
    info_p->cfg_undo_word_count = cfg_undo_words_buf_max_count[u];
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_stats_info_clear(int u)
{
    SHR_FUNC_ENTER(u);

    /* Following stats may be overwritten by WAL writer, reader even before this
     * function finishes - if WAL was not empty when this function is called.
     * And this is ok. These stats are for debug and we cannot introduce locks
     * on WAL writer, reader just to implement 'pedantic clear of all stats'.
     */
    s_min_avail_trans_count[u] = cfg_wal_trans_max_count[u];
    s_min_avail_msg_count[u] = cfg_wal_msg_max_count[u];
    s_min_avail_ops_info_count[u] = cfg_wal_ops_info_max_count[u];
    s_min_avail_words_buf_count[u] = cfg_wal_words_buf_max_count[u];
    s_min_avail_undo_ops_info_count[u] = cfg_undo_ops_info_max_count[u];
    s_min_avail_undo_words_buf_count[u] = cfg_undo_words_buf_max_count[u];

    s_max_words_in_msg[u] = 0;
    s_max_ops_in_msg[u] = 0;
    s_max_msgs_in_trans[u] = 0;
    s_max_slam_in_trans[u] = 0;

/* exit: */
    SHR_FUNC_EXIT();
}

int
bcmptm_wal_mode_sel(int u, int wal_mode)
{
    SHR_FUNC_ENTER(u);

    cfg_wal_mode[u] = wal_mode;

    if (wal_mode < 0) {
        LOG_WARN(BSL_LOG_MODULE,
            (BSL_META_U(u, "Will operate WAL in Auto-Commit mode (%0d)\n"),
             wal_mode));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_wal_autocommit_enabled(int u)
{
    if (cfg_wal_mode[u] < 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void
bcmptm_wal_dma_avail(int u, bool read_op, uint32_t entry_count,
                     bool *wal_dma_avail)
{
    if (wal_dma_avail) { /* !NULL */
        if (read_op){
            *wal_dma_avail = dma_avail[u] &&
                             (entry_count >= cfg_wal_dma_op_count_thr[u]);
        } else {
            *wal_dma_avail = dma_avail[u] &&
                             (entry_count >= cfg_wal_slam_op_count_thr[u]);
        }
    }
}

int
bcmptm_wal_write(int u,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 bcmptm_misc_info_t *misc_info,
                 uint32_t *entry_words,
                 bcmptm_rm_op_t op_type,
                 bcmptm_rm_slice_change_t *slice_mode_change_info)
{
    return cfg_wal_fn_ptr_all[u].fn_wal_write(
        u, flags, sid, pt_dyn_info, misc_info,
        entry_words, op_type, slice_mode_change_info);
}

int
bcmptm_wal_trans_cmd(int u, uint64_t flags,
                     bcmptm_trans_cmd_t trans_cmd,
                     uint32_t in_cseq_id,
                     bcmptm_trans_cb_f notify_fn_p,
                     void *notify_info)
{
    return cfg_wal_fn_ptr_all[u].fn_wal_trans_cmd(
        u, flags, trans_cmd, in_cseq_id, notify_fn_p, notify_info);
}

int
bcmptm_wal_mreq_state_update(int u, uint32_t in_cseq_id, uint64_t flags)
{
    return cfg_wal_fn_ptr_all[u].fn_wal_mreq_state_update(
        u, in_cseq_id, flags);
}

/* Following API will not be called when in autocommit mode */
int bcmptm_wal_dyn_atomic_trans(int u, bool enable)
{
    SHR_FUNC_ENTER(u);

    if (enable) {
        /* Override configured mode of WAL and
         * temporarily operate WAL in atomic transaction enabled mode.
         */
        SHR_IF_ERR_EXIT(!wal_disable_undo[u] ? SHR_E_INTERNAL : SHR_E_NONE);
        /* we must be operating in atomic_trans disabled mode */
        if (cfg_wal_bypass[u]) {
            /* We are in BYPASS mode and want to enable atomic_trans.
             * Commit/Abort has already been processed for previous commit seq.
             * WAL should be empty.
             */
            if (wstate[u]->posted_scf_write || wstate[u]->sent_pio_write) {
                /* This means write was already sent/posted for current
                 * trans - so request to enable atomic trans is in middle of
                 * current trans. This is an error.
                 */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(u, "CTH is requesting dynamic enable "
                                      "of atomic_trans in middle of a "
                                      "transaction (WAL in bypass mode)\n")));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Register non_byp (normal) APIs */
            cfg_wal_fn_ptr_all[u].fn_wal_write = &wal_write_normal;
            cfg_wal_fn_ptr_all[u].fn_wal_trans_cmd = &wal_trans_cmd_normal;
            cfg_wal_fn_ptr_all[u].fn_wal_mreq_state_update =
                &wal_mreq_state_update;
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(u, "\nWill temporarily work in WAL_Two_Thread "
                            "(normal) mode\n")));

            /* Enable undo */
            wal_disable_undo[u] = FALSE;

        } else {
            /* Commit/Abort has already been processed for previous commit seq.
             * So, we can expect mreq_state_in_trans, trans_state to be IDLE.
             */

            /* But, because we were operating WAL in 2 thread mode with
             * enable_atomic_trans = 0. so WAL may not be empty.
             *
             * Waiting for WAL to become empty is not needed. We can start
             * adding more ops and let WAL reader continue with dequeue of
             * previous ops.
             */

            if (wstate[u]->c_trans_state != TRANS_STATE_IDLE) {
                /* Above means some writes were already sent/posted for current
                 * trans - so request to enable atomic trans is in middle of
                 * current trans. This is an error.
                 */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(u, "CTH is requesting dynamic enable "
                                      "of atomic_trans in middle of a "
                                      "transaction (trans_state = %0d)\n"),
                           wstate[u]->c_trans_state));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            /* Non_byp (normal) APIs were already registered. No need to
             * re-register.  */

            /* Enable undo */
            wal_disable_undo[u] = FALSE;
        }

        /* Record state of cfg_wal_bypass */
        prev_cfg_wal_bypass[u] = cfg_wal_bypass[u];

        /* Then reset cfg_wal_bypass mode - to make sure functions like
         * bcmptm_wal_read, etc work as expected when atomic_trans are enabled.
         */
        cfg_wal_bypass[u] = FALSE;

        wal_dyn_enable_atomic_trans[u] = TRUE;
    } else { /* enable = 0 */
        /* Revert WAL mode from atomic_trans enable to configured mode.
         * - Configured mode MUST have been atomic_trans disabled mode because
         *   otherwise there would be no calls to this function.
         */

        /* Commit/Abort has already been processed for dyn_atomic_trans.
         * So, we can expect mreq_state_in_trans, trans_state to be IDLE.
         */
        SHR_IF_ERR_EXIT(
            (wstate[u]->mreq_state_in_trans ||
             (wstate[u]->c_trans_state != TRANS_STATE_IDLE)) ?
            SHR_E_INTERNAL : SHR_E_NONE);

        if (prev_cfg_wal_bypass[u]) {
            /* When reverting to WAL_BYPASS mode, we need to ensure that new
             * write_ops do not go out of order (as compared to write ops posted
             * as part of atomic trans.
             * Wait for WAL drain
             */
            SHR_IF_ERR_EXIT(
                bcmptm_wal_drain(u, TRUE)); /* end state must be IDLE */

            /* Re-register bypass mode related wal functions */
            wal_fn_register_bypass(u);

            /* Revert disable_undo */
            wal_disable_undo[u] = TRUE;
        } else { /* atomic_trans disabled - but in 2 thread mode */
            /* No out-of-order issue because new write_ops will result in
             * sending of messages which will be executed after all previous
             * messages have been sent to HW.
             * Only difference is that new messages will be sent to reader-side
             * without waiting for commit.
             *
             * In summary: no need to wait for WAL to drain.
             */

            /* Normal wal functions were already registered - no need to
             * re-register.
             */

            /* Revert disable_undo */
            wal_disable_undo[u] = TRUE;
        }

        /* Revert cfg_wal_bypass */
        cfg_wal_bypass[u] = prev_cfg_wal_bypass[u];

        wal_dyn_enable_atomic_trans[u] = FALSE;
    } /* enable = 0 */

exit:
    SHR_FUNC_EXIT();
}

int bcmptm_wal_allow_serc_check(int u, bool *allow_serc)
{
    SHR_FUNC_ENTER(u);

    if (wal_disable_undo[u] && !wal_dyn_enable_atomic_trans[u]) {
        /* Atomic trans are disabled */

        if (cfg_wal_bypass[u]) {
            /* WAL is already empty */

            /* Check status and then clear start bit (that was set during
             * previous write) */
            SHR_IF_ERR_EXIT(posted_wr_status_check(u, 0));

            /* Nothing is pending in WAL, so PTcache and HW are in sync */
            *allow_serc = TRUE;
        } else { /* 2 thread mode */
            /* WAL may not be empty */
            SHR_IF_ERR_EXIT(wal_empty_make(u, FALSE));
            *allow_serc = TRUE;
        }
    }
    /* else we are in middle of an atomic trans statically or dynamically and so
     * we cannot give SERC a chance. */

exit:
    SHR_FUNC_EXIT();
}

/*
 * When in 2 thread mode:
 *
 * READ:
 *      - Atomic_trans enabled:
 *        We send_all_msgs, assert last_msg and change state to BLOCKING_READ,
 *        BLOCKING_READ_AFTER_WR. This ensures that trans descriptor is freed
 *        and wait for WAL to become empty.
 *
 *
 *        We are now in BLOCKING_READ or BLOCKING_READ_AFTER_WR state.
 *        If we subsequently see:
 *              - Another Read
 *                  WAL is empty.
 *                  We remain in same trans_state.
 *
 *              - Write
 *                  WAL is empty.
 *                  We return error. We don't allow write after hw_read..
 *
 *              - Commit
 *                  WAL is empty.
 *                  Change state to IDLE.
 *                  If requested, call notification from writer thread.
 *
 *              - Abort
 *                  WAL is empty.
 *                  If we were in READ_AFTER_WR state, flag error.
 *                  Change state to IDLE.
 *                  If requested, call notification from writer thread.
 *
 *
 *      - Atomic_trans disabled:
 *        If we have something pending on writer side, we send one more msg with
 *        last_msg asserted. This ensures that trans descriptor is freed
 *
 *        If we have nothing pending on writer side, we must send one empty msg
 *        with last_msg asserted. This ensures that trans descriptor is freed
 *
 *        Change state to BLOCKING_READ, BLOCKING_READ_AFTER_WR.
 *
 *        Wait for WAL to become empty.
 *
 *        We are now in BLOCKING_READ or BLOCKING_READ_AFTER_WR state.
 *        If we subsequently see:
 *              - Read
 *                  WAL is empty.
 *                  We remain in same trans_state.
 *
 *              - Write
 *                  WAL is empty.
 *                  We return error. We don't allow write after hw_read..
 *
 *              - Commit
 *                  WAL is empty.
 *                  Change state to IDLE.
 *                  If requested, call notification from writer thread.
 *
 *              - Abort
 *                  WAL is empty.
 *                  If we were in READ_AFTER_WR state, flag error.
 *                  Change state to IDLE.
 *                  Even if requested, no call notification from writer thread.
 *
 *
 * INTERRUPT at end of cmdproc_write:
 *      - Atomic_trans disabled:
 *        If we have something pending on writer side, we send one more msg with
 *        last_msg asserted. This ensures that trans descriptor is freed
 *
 *        If we have nothing pending on writer side, we must send one empty msg
 *        with last_msg asserted. This ensures that trans descriptor is freed
 *
 *        Change state to IDLE_AFTER_WRITE
 *
 *        Wait for WAL to become empty.
 *
 *        We are now in IDLE_AFTER_WR state.
 *        If we subsequently see:
 *              - Read
 *                  WAL is empty.
 *                  We go to BLOCKING_READ_AFTER_WR
 *
 *              - Write
 *                  WAL is empty.
 *                  We treat it like IDLE and get to IN_MSG/NEED_NEW_MSG, etc as
 *                  if we are in IDLE state.
 *
 *              - Commit
 *                  WAL is empty.
 *                  We treat it like IDLE.
 *                  Change state to IDLE.
 *                  If requested, call notification from writer thread.
 *
 *              - Abort
 *                  WAL is empty.
 *                  We treat state like BLOCKING_READ_AFTER_WRITE
 *                  Flag error - we have already sent writes for this trm_seq.
 *                  Change state to IDLE.
 *                  Even if requested, no call notification from writer thread.
 *
 *      - Atomic_trans enabled:
 *          - Interrupting in atomic_trans enabled mode is not supported (yet).
 *
 * INTERRUPT at end of cmdproc_read:
 *          Interrupting in atomic_trans disabled mode is not supported at end
 *          of cmdproc_read because reads:
 *              - from HW on modeled path are only for dbg, so we don't want to
 *              optimize this case.
 *              - reads from PTcache do not take long time - so we are not
 *              blocking SERC. Only case where this may become important is that
 *              RM is doing some heave computation (eg: deciding if we can move
 *              an entry from one bucket to another) and is taking long time. In
 *              the meantime SERC event is waiting to be serviced. FIXME.
 */
