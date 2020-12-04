/*! \file bcmcth_l2_drv.c
 *
 * BCMCTH L2 driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_ha.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_msgq.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmcth/bcmcth_learn_drv.h>
#include <bcmcth/bcmcth_l2.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE                      BSL_LS_BCMCTH_L2

#define L2_LEARN_INTERVAL_SLOW_POLL         500000
#define L2_LEARN_INTERVAL_NORMAL            200000
#define L2_LEARN_INTERVAL_AGRRESIVE         100
/* Count of poll times during which no new entry is added to learn cache */
#define L2_LEARN_THRESHOLD_NO_NEW_ENTRY     64
/* Count of interval during which learn cache keeps empty */
#define L2_LEARN_THRESHOLD_CACHE_EMPTY      16

/*! Device specific attach function type. */
typedef bcmcth_learn_info_drv_t *(*bcmcth_learn_cache_info_drv_get_f)(int unit);

/*! Learn Cache info table drive get. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_learn_cache_data_info_drv_get},
static struct {
    bcmcth_learn_cache_info_drv_get_f drv_get;
} learn_cache_data_info_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};


/*! L2 learn cache entry max size in words. */
#define L2_CACHE_ENTRY_WSIZE_MAX        4
#define L2_CACHE_QUEUE_SIZE_MAX         2048
/*! L2 learn cache HA structure signature. */
#define L2_CACHE_HA_SIGN                0x05050A0A
/*! L2 learn cache entry HA structure signature. */
#define L2_CACHE_ENTRY_HA_SIGN          0xA5A50101

typedef enum l2_cache_entry_status_e {
    /*! Entry has been freed, available to learn new data. */
    ENTRY_STAT_IDLE = 0,

    /*! New entry to be reported. */
    ENTRY_STAT_NEW = 1,

    /*! Entry has been reported to LT. */
    ENTRY_STAT_REPORTED = 2,

    /*! Entry has been deleted from LT. */
    ENTRY_STAT_DEL = 3

} l2_cache_entry_status_t;

static char *stat_name[] = {"IDLE", "NEW", "REPORTED", "DEL"};

typedef struct l2_cache_entry_s {
    /*! Entry status. */
    l2_cache_entry_status_t status;

    /*! Entry data. */
    uint32_t data[L2_CACHE_ENTRY_WSIZE_MAX];
} l2_cache_entry_t;

typedef struct l2_ring_queue_s {
    /*! Count of elements in queue. */
    uint32_t count;

    /*! Index of first element in queue. */
    uint32_t front;

    /*! Next index of last element in queue. */
    uint32_t rear;

    /*! Index of first to-report element in queue. */
    uint32_t report;

    /*! Count of elements to report. */
    uint32_t rpt_cnt;

    /*! New learn entry number during one DMA operation. */
    uint32_t fresh_entry;

    /*! Number of all learned entries. */
    uint32_t learn_sum;

    /*! Number of all reported entries. */
    uint32_t rpt_sum;

    /*! Number of all LT deleted entries. */
    uint32_t del_sum;

    /*! Queue size. */
    uint32_t size;
} l2_ring_queue_t;

typedef struct l2_cache_s {
    /*! Pipe count. */
    uint32_t pipe_cnt;

    /*! Ring queue for each pipe. */
    l2_ring_queue_t *rq;

    /*! Queue elements array. */
    l2_cache_entry_t *elements;
} l2_cache_t;

/*! Working buffer. */
typedef struct l2_wbuf_s {
    /*! Buffer handler. */
    void *buf;

    /*! Buffer physical address for DMA mode. */
    uint64_t paddr;

    /*! DMA work data structure. */
    bcmbd_sbusdma_data_t work_data;

    /*! DMA work structure. */
    bcmbd_sbusdma_work_t work;
} l2_wbuf_t;

static l2_wbuf_t *l2_wbuf[BCML2_DEV_NUM_MAX];
static l2_cache_t l2_cache[BCML2_DEV_NUM_MAX];

/* L2 cache entry HA structure. */
static bcmcth_l2_cache_entry_ha_t *l2_cache_entry_ha[BCML2_DEV_NUM_MAX];
static bcmcth_l2_cache_ha_t *l2_cache_ha[BCML2_DEV_NUM_MAX];


/******************************************************************************
* Private functions
 */
static void
l2_cache_entry_state_transit(int unit, l2_ring_queue_t *rq,
                             l2_cache_entry_t *elements, uint32_t index,
                             l2_cache_entry_status_t new_state)
{
    l2_cache_entry_status_t cur_state;
    bool invalid = false;

    if (index >= rq->size) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid index %d.\n"),
                              index));
        return;
    }

    cur_state = elements[index].status;
    switch (cur_state) {
    case ENTRY_STAT_IDLE:
        if (new_state != ENTRY_STAT_NEW) {
            invalid = true;
        }
        break;
    case ENTRY_STAT_NEW:
        if (new_state != ENTRY_STAT_REPORTED) {
            invalid = true;
        }
        break;
    case ENTRY_STAT_REPORTED:
        if (new_state != ENTRY_STAT_DEL) {
            invalid = true;
        }
        break;
    case ENTRY_STAT_DEL:
        if (new_state != ENTRY_STAT_IDLE) {
            invalid = true;
        }
        break;
    default:
        break;
    }

    if (invalid) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "[%d]: %s->%s, front %d rear %d rpt %d\n"),
                              index,
                              stat_name[cur_state], stat_name[new_state],
                              rq->front, rq->rear, rq->report));
    }

    elements[index].status = new_state;
}

static int
l2_wbuf_init(int unit, uint32_t pipe, l2_wbuf_t *wbuf)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcmbd_sbusdma_data_t *dma_data = NULL;
    bcmbd_sbusdma_work_t *dma_work = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t header[4], hd_size = 0, req_size;
    bool real_hw = bcmdrd_feature_is_real_hw(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(wbuf, SHR_E_INTERNAL);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT
        (bcmbd_pt_cmd_hdr_get(unit,
                              drv->state_ptsid,
                              &pt_dyn_info,
                              NULL,
                              BCMBD_PT_CMD_READ,
                              4,
                              header,
                              &hd_size));

    req_size = 4 * pi->entry_wsize * pi->tbl_size;

    if (real_hw) {
        wbuf->buf = bcmdrd_hal_dma_alloc(unit,
                                         req_size,
                                         "bcmcthL2LearnDmaBuf",
                                         &(wbuf->paddr));
        SHR_NULL_CHECK(wbuf->buf, SHR_E_MEMORY);
        sal_memset(wbuf->buf, 0, req_size);

        /* Prepare dma_data */
        dma_data = &(wbuf->work_data);
        sal_memset(dma_data, 0, sizeof(bcmbd_sbusdma_data_t));
        dma_data->op_code = header[0];
        dma_data->start_addr = header[1];
        dma_data->data_width = pi->entry_wsize;
        dma_data->op_count = pi->tbl_size;
        dma_data->buf_paddr = wbuf->paddr;
        dma_data->pend_clocks = bcmbd_mor_clks_read_get(unit, drv->state_ptsid);

        /* Prepare dma_work */
        dma_work = &(wbuf->work);
        sal_memset(dma_work, 0, sizeof(bcmbd_sbusdma_work_t));
        dma_work->data = dma_data;
        dma_work->items = 1;
        dma_work->flags = SBUSDMA_WF_READ_CMD;
        dma_work->queue = 2;
    } else {
        /* Simulator only support PIO */
        wbuf->buf = sal_alloc(req_size, "bcmcthL2LearnPioBuf");
        SHR_NULL_CHECK(wbuf->buf, SHR_E_MEMORY);
        sal_memset(wbuf->buf, 0, req_size);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
l2_wbuf_read(int unit, uint32_t pipe, l2_wbuf_t *wbuf)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcmbd_sbusdma_work_t *dma_work = NULL;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;
    uint32_t *buf = NULL;
    uint32_t idx = 0, attrib = BCMLT_ENT_ATTR_GET_FROM_HW;
    bool real_hw = bcmdrd_feature_is_real_hw(unit);

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    if (real_hw) {
        dma_work = &(wbuf->work);

        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_light_work_init(unit, dma_work));

        SHR_IF_ERR_EXIT
            (bcmbd_sbusdma_work_execute(unit, dma_work));
    } else {
        buf = (uint32_t *)wbuf->buf;

        for (idx = 0; idx < pi->tbl_size; idx++) {
            pt_info.index = idx;
            pt_info.tbl_inst = pipe;
            req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(attrib);

            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_read(unit,
                                      req_flags,
                                      drv->state_ptsid,
                                      &pt_info,
                                      NULL,
                                      pi->entry_wsize,
                                      drv->state_sid,
                                      buf,
                                      &rsp_ltid,
                                      &rsp_flags));

            buf += pi->entry_wsize;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static bool
l2_ring_queue_iterate(int unit, l2_ring_queue_t *rq, l2_cache_entry_t *elements,
                      void *data, uint32_t wsize)
{
    uint32_t idx;

    if (rq->count == 0) {
        return false;
    }

    for (idx = rq->front; idx != rq->rear; idx = (idx + 1) % rq->size) {
        if (elements[idx].status == ENTRY_STAT_IDLE ||
            elements[idx].status == ENTRY_STAT_DEL) {
            continue;
        }

        if (0 == sal_memcmp((void *)elements[idx].data, data, wsize * 4)) {
            return true;
        }
    }

    return false;
}

static bool
l2_ring_queue_rpt_pop(int unit, l2_ring_queue_t *rq, l2_cache_entry_t *elements)
{
    /* Queue empty */
    if (rq->rpt_cnt == 0) {
        return false;
    }

    l2_cache_entry_state_transit(unit, rq, elements, rq->report,
                                 ENTRY_STAT_REPORTED);
    rq->report = (rq->report + 1) % rq->size;
    rq->rpt_cnt--;
    return true;
}

static bool
l2_ring_queue_rpt_entry_get(int unit, l2_ring_queue_t *rq,
                            l2_cache_entry_t *elements,
                            uint32_t *idx, void **data)
{
    /* Queue empty */
    if (rq->rpt_cnt == 0) {
        return false;
    }

    *idx = rq->report;
    *data = elements[*idx].data;
    return true;
}

static bool
l2_ring_queue_push(int unit, l2_ring_queue_t *rq, l2_cache_entry_t *elements,
                   void *data, uint32_t wsize)
{
    /* Queue full */
    if (rq->count == rq->size) {
        return false;
    }

    sal_memcpy((void *)elements[rq->rear].data, data, wsize * 4);
    l2_cache_entry_state_transit(unit, rq, elements, rq->rear,
                                 ENTRY_STAT_NEW);

    rq->rear = (rq->rear + 1) % rq->size;
    rq->count++;
    rq->rpt_cnt++;
    return true;
}

static bool
l2_ring_queue_pop(int unit, l2_ring_queue_t *rq, l2_cache_entry_t *elements)
{
    /* Queue empty or waiting for LT delete */
    if (rq->count == 0 ||
        elements[rq->front].status != ENTRY_STAT_DEL) {
        return false;
    }

    l2_cache_entry_state_transit(unit, rq, elements, rq->front,
                                 ENTRY_STAT_IDLE);
    rq->front = (rq->front + 1) % rq->size;
    rq->count--;
    return true;
}

/* Traverse pt to populate entries to IMM. */
static int
l2_learn_cache_traverse(int unit, l2_trav_status_t *status)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_cache_t *cache = NULL;
    l2_ring_queue_t *rq = NULL;
    l2_cache_entry_t *elements = NULL;
    l2_wbuf_t *wbuf = NULL;
    void *data = NULL;
    uint32_t *hw_entry = NULL;
    uint32_t pipe, seq, idx, data_id = 0;
    bool found, rv;
    int ret = SHR_E_NONE;
    uint32_t entry_queued = 0, entry_to_report = 0, rpt_threshold = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(status, SHR_E_PARAM);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);
    cache = &(l2_cache[unit]);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        /* For each active pipe */
        seq = pi->act_seq[pipe];
        wbuf = l2_wbuf[unit] + seq;
        rq = cache->rq + seq;
        elements = cache->elements + seq * rq->size;

        ret = l2_wbuf_read(unit, pipe, wbuf);
        if (ret != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "WBuf[%d]: read failed.\n"), pipe));
        }

        /* Iterate queue to filter out learnt entries, add new entry to queue */
        hw_entry = (uint32_t *)wbuf->buf;
        rq->fresh_entry = 0;
        for (idx = 0; idx < pi->tbl_size; idx++) {
            if (!bcmcth_l2_drv_entry_valid(unit, hw_entry)) {
                break;
            }

            found = l2_ring_queue_iterate(unit, rq, elements,
                                          hw_entry, pi->entry_wsize);
            if (!found) {
                rv = l2_ring_queue_push(unit, rq, elements,
                                        hw_entry, pi->entry_wsize);
                if (rv == false) {
                    if (rq->rpt_cnt != 0) {
                        LOG_INFO(BSL_LOG_MODULE,
                                 (BSL_META_U(unit, "Push[%d] full: count %d, "
                                                   "lsum %d, rsum %d, "
                                                   "dsum %d.\n"),
                                 pipe, rq->count, rq->learn_sum,
                                 rq->rpt_sum, rq->del_sum));
                    }
                    break;
                } else {
                    rq->learn_sum++;
                    rq->fresh_entry++;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Push[%d] rear %d, count %d, "
                                                  "rptcnt %d.\n"),
                                 pipe, rq->rear, rq->count, rq->rpt_cnt));
                }
            } else {
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "WBuf[%d]: dup entry %d.\n"),
                          pipe, idx));
            }

            hw_entry += pi->entry_wsize;
        }

        /* report learnt entry by insert IMM table */
        rpt_threshold = pi->tbl_size;
        while (rpt_threshold--) {
            rv = l2_ring_queue_rpt_entry_get(unit, rq, elements,
                                             &data_id, &data);
            if (rv == false) {
                break;
            }
            if (true == l2_ring_queue_rpt_pop(unit, rq, elements)) {
                rq->rpt_sum++;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "RptPop[%d]: report %d, "
                                               "rptcnt %d.\n"),
                             pipe, rq->report, rq->rpt_cnt));
            }
            ret = bcmcth_l2_drv_entry_insert(unit, pipe, data_id, data);
            if (ret != SHR_E_NONE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Rpt[%d]: insert entry %d.\n"),
                          pipe, data_id));
                /* New learning mac is lost. */
                l2_cache_entry_state_transit(unit, rq, elements, data_id,
                                             ENTRY_STAT_DEL);
                break;
            }
        }

        /* update status of entries acked by user */
        do {
            rv = l2_ring_queue_pop(unit, rq, elements);
            if (rv) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Pop[%d]: front %d, cnt %d.\n"),
                             pipe, rq->front, rq->count));
            }
        } while (rv == true);

        /* traverse status for current pipe */
        if (rq->count) {
            entry_queued = 1;
        }
        if (rq->fresh_entry) {
            entry_to_report = 1;
        }
    }

    /* update traverse result */
    status->cache_empty = !entry_queued;
    status->new_learn = entry_to_report;

exit:
    SHR_FUNC_EXIT();
}


static void
l2_learn_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;
    int rv = SHR_E_NONE;
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_ctrl_t *learn_ctrl;
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;
    sal_usecs_t interval;
    l2_trav_status_t status;
    int no_new_learn = 0, empty_count = 0;
    bool sim = bcmdrd_feature_is_sim(unit);

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    report_ctrl = drv->report_ctrl;
    if (learn_ctrl == NULL || report_ctrl == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "L2 learn ctrl null, thread exit.\n")));
        return;
    }

    /*
     * L2 learn h/w interrupt does not provide info for which new entry is
     * learnt, s/w has to poll the cache.
     * The interrupt is used as an indicator to start the polling process,
     * when h/w learn disabled or interrupt disabled, learn thread will sleep
     * infinitely after the last round of traverse done.
     */
    do {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "L2 thread poll interval %d usecs.\n"),
                              learn_ctrl->interval));

        shr_thread_sleep(tc, learn_ctrl->interval);
        if (shr_thread_stopping(tc)) {
            break;
        }

        /* Traverse the HW cache and report the learned MAC addresses. */
        sal_memset(&status, 0, sizeof(l2_trav_status_t));
        rv = l2_learn_cache_traverse(unit, &status);
        if (rv != SHR_E_NONE) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Traverse cache error.\n")));
        }
        /* No intr on simulator, always polling at initial interval. */
        if (sim) {
            continue;
        }
        /* Calculate new interval per cache status. */
        if (!status.cache_empty
            || empty_count < L2_LEARN_THRESHOLD_CACHE_EMPTY) {
            if (report_ctrl->slow_poll == 0) {
                if (status.new_learn) {
                    no_new_learn = 0;
                    interval = L2_LEARN_INTERVAL_AGRRESIVE;
                } else {
                    if (status.cache_empty == 0) {
                        empty_count = 0;
                    } else {
                        empty_count++;
                    }
                    if (no_new_learn < L2_LEARN_THRESHOLD_NO_NEW_ENTRY) {
                        /* Increase interval if no new entry in cache. */
                        no_new_learn++;
                        interval = no_new_learn / 16 + 2;
                    } else {
                        interval = 6;
                    }
                    interval *= L2_LEARN_INTERVAL_AGRRESIVE;
                }
            } else {
                if (status.cache_empty == 0) {
                    empty_count = 0;
                } else {
                    empty_count++;
                }
                interval = L2_LEARN_INTERVAL_SLOW_POLL;
            }
        } else {
            /* Block learn thread and wait for interrupt. */
            no_new_learn = 0;
            empty_count = 0;
            interval = SHR_THREAD_FOREVER;
            bcmcth_l2_drv_intr_enable_set(unit, 1, 0);
        }
        learn_ctrl->interval = interval;
    } while (learn_ctrl->active);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "L2 thread exited.\n")));
}

static void
l2_learn_intr_handler(int unit, uint32_t intr_param)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_ctrl_t *learn_ctrl;

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    if (learn_ctrl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Learn ctrl NULL\n")));
        return;
    }

    if (learn_ctrl->thread_ctrl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Thread ctrl NULL\n")));
        return;
    }

    bcmcth_l2_drv_intr_enable_set(unit, 0, 1);

    learn_ctrl->interval = L2_LEARN_INTERVAL_AGRRESIVE;

    shr_thread_wake(learn_ctrl->thread_ctrl);
}

static int
l2_learn_cache_info_init(int unit, bool warm)
{
    bcmcth_learn_info_drv_t *info_drv = NULL;
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;
    bcmcth_info_table_t data;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    variant = bcmlrd_variant_get(unit);
    /* Perform ingress info table setup */
    data.table_id = 0xFFFFFFFF;
    info_drv = learn_cache_data_info_drv_get[variant].drv_get(unit);
    if (info_drv && info_drv->get_data) {
        SHR_IF_ERR_EXIT
            (info_drv->get_data(unit, &data));

        SHR_IF_ERR_EXIT
            (info_tbl_init(unit, &data, warm));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
l2_learn_control_init(int unit, bool warm)
{
    bcmcth_l2_drv_t *drv;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    drv->learn_ctrl = NULL;
    drv->report_ctrl = NULL;

    /* Allocate learn control. */
    SHR_ALLOC(drv->learn_ctrl,
              sizeof(bcmcth_l2_learn_ctrl_t),
              "bcmcthL2LearnCtrl");
    SHR_NULL_CHECK(drv->learn_ctrl, SHR_E_MEMORY);
    sal_memset(drv->learn_ctrl, 0, sizeof(bcmcth_l2_learn_ctrl_t));

    /* Allocate report control */
    ha_req_size = sizeof(bcmcth_l2_learn_report_ctrl_t);
    ha_alloc_size = ha_req_size;
    drv->report_ctrl = shr_ha_mem_alloc(unit,
                                        BCMMGMT_L2_COMP_ID,
                                        BCML2_LEARN_CTRL_COMP_ID,
                                        "bcmcthL2LearnReportCtrl",
                                        &ha_alloc_size);

    SHR_NULL_CHECK(drv->report_ctrl, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        /* Allocated size may exceed requested size, clear the whole memory */
        sal_memset(drv->report_ctrl, 0, ha_alloc_size);

        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_L2_COMP_ID,
                                        BCML2_LEARN_CTRL_COMP_ID, 0,
                                        sizeof(bcmcth_l2_learn_report_ctrl_t),
                                        1,
                                        BCMCTH_L2_LEARN_REPORT_CTRL_T_ID));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (drv->report_ctrl != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_L2_LEARN_REPORT_CTRL_T_ID,
                                      BCMMGMT_L2_COMP_ID,
                                      BCML2_LEARN_CTRL_COMP_ID);
            shr_ha_mem_free(unit, drv->report_ctrl);
            drv->report_ctrl = NULL;
        }
        if (drv->learn_ctrl) {
            SHR_FREE(drv->learn_ctrl);
        }
    }

    SHR_FUNC_EXIT();
}

static int
l2_learn_cache_init(int unit, bool warm)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_ring_queue_t *rq = NULL;
    l2_wbuf_t *wbuf = NULL;
    uint32_t pipe, seq, req_size, ha_req_size, ha_alloc_size;
    uint32_t offset = 0;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    /* Init working buffer */
    req_size = pi->act_cnt * sizeof(l2_wbuf_t);
    l2_wbuf[unit] = sal_alloc(req_size, "bcmcthL2LearnWbuf");
    SHR_NULL_CHECK(l2_wbuf[unit], SHR_E_MEMORY);
    sal_memset(l2_wbuf[unit], 0, req_size);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        seq = pi->act_seq[pipe];
        wbuf = &(l2_wbuf[unit][seq]);

        SHR_IF_ERR_VERBOSE_EXIT
            (l2_wbuf_init(unit, pipe, wbuf));
    }

    /* Allocate HA struture for ring queue */
    assert(sizeof(l2_ring_queue_t) == sizeof(bcmcth_l2_ring_queue_ha_t));

    ha_req_size = sizeof(bcmcth_l2_cache_ha_t) +
                  pi->act_cnt * sizeof(bcmcth_l2_ring_queue_ha_t);
    ha_alloc_size = ha_req_size;
    l2_cache_ha[unit] = shr_ha_mem_alloc(unit,
                                         BCMMGMT_L2_COMP_ID,
                                         BCML2_L2_CACHE_COMP_ID,
                                         "bcmcthL2LearnCache",
                                         &ha_alloc_size);
    SHR_NULL_CHECK(l2_cache_ha[unit], SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm || SHR_E_NOT_FOUND ==
        bcmissu_struct_info_get(unit,
                                BCMCTH_L2_CACHE_HA_T_ID,
                                BCMMGMT_L2_COMP_ID,
                                BCML2_L2_CACHE_COMP_ID,
                                &offset)) {
        /* If this is new structure not in ISSU DB, init and report. */
        sal_memset(l2_cache_ha[unit], 0, ha_alloc_size);
        l2_cache_ha[unit]->signature = L2_CACHE_HA_SIGN;
        l2_cache_ha[unit]->array_size = pi->act_cnt;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_L2_COMP_ID,
                                        BCML2_L2_CACHE_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_L2_CACHE_HA_T_ID));
    } else {
        l2_cache_ha[unit] = (bcmcth_l2_cache_ha_t *)
                                ((uint8_t *)l2_cache_ha[unit] + offset);
        if (l2_cache_ha[unit]->signature != L2_CACHE_HA_SIGN) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "WB L2Cache: Signature mismatch"
                                       " exp=0x%-8x act=0x%-8x\n"),
                     L2_CACHE_HA_SIGN,
                     l2_cache_ha[unit]->signature));
        }
    }

    /* Allocate HA struture for learn cache entry */
    assert(sizeof(l2_cache_entry_t) == sizeof(bcmcth_l2_entry_ha_t));

    ha_req_size = sizeof(bcmcth_l2_cache_entry_ha_t) + pi->act_cnt *
                  L2_CACHE_QUEUE_SIZE_MAX * sizeof(bcmcth_l2_entry_ha_t);
    ha_alloc_size = ha_req_size;
    l2_cache_entry_ha[unit] = shr_ha_mem_alloc(unit,
                                               BCMMGMT_L2_COMP_ID,
                                               BCML2_L2_CACHE_ENTRY_COMP_ID,
                                               "bcmcthL2LearnCacheEntry",
                                               &ha_alloc_size);
    SHR_NULL_CHECK(l2_cache_entry_ha[unit], SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm || SHR_E_NOT_FOUND ==
        bcmissu_struct_info_get(unit,
                                BCMCTH_L2_CACHE_ENTRY_HA_T_ID,
                                BCMMGMT_L2_COMP_ID,
                                BCML2_L2_CACHE_ENTRY_COMP_ID,
                                &offset)) {
        /* If this is new structure not in ISSU DB, init and report. */
        sal_memset(l2_cache_entry_ha[unit], 0, ha_alloc_size);
        l2_cache_entry_ha[unit]->signature = L2_CACHE_ENTRY_HA_SIGN;
        l2_cache_entry_ha[unit]->array_size = pi->act_cnt *
                                              L2_CACHE_QUEUE_SIZE_MAX;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_L2_COMP_ID,
                                        BCML2_L2_CACHE_ENTRY_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_L2_CACHE_ENTRY_HA_T_ID));
    } else {
        l2_cache_entry_ha[unit] = (bcmcth_l2_cache_entry_ha_t *)
                            ((uint8_t *)l2_cache_entry_ha[unit] + offset);

        if (l2_cache_entry_ha[unit]->signature != L2_CACHE_ENTRY_HA_SIGN) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "WB L2CacheEntry: Signature mismatch"
                                       " exp=0x%-8x act=0x%-8x\n"),
                     L2_CACHE_ENTRY_HA_SIGN,
                     l2_cache_entry_ha[unit]->signature));
        }
    }

    /* Init ring queue */
    l2_cache[unit].pipe_cnt = pi->act_cnt;
    l2_cache[unit].rq = (l2_ring_queue_t *)l2_cache_ha[unit]->rq;
    l2_cache[unit].elements =
                    (l2_cache_entry_t *)l2_cache_entry_ha[unit]->entry;

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        seq = pi->act_seq[pipe];
        rq = l2_cache[unit].rq + seq;
        rq->size = L2_CACHE_QUEUE_SIZE_MAX;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (l2_cache_entry_ha[unit]) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_L2_CACHE_ENTRY_HA_T_ID,
                                      BCMMGMT_L2_COMP_ID,
                                      BCML2_L2_CACHE_ENTRY_COMP_ID);
            shr_ha_mem_free(unit, l2_cache_entry_ha[unit]);
            l2_cache_entry_ha[unit] = NULL;
        }

        if (l2_cache_ha[unit]) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_L2_CACHE_HA_T_ID,
                                      BCMMGMT_L2_COMP_ID,
                                      BCML2_L2_CACHE_COMP_ID);
            shr_ha_mem_free(unit, l2_cache_ha[unit]);
            l2_cache_ha[unit] = NULL;
        }

        if (l2_wbuf[unit]) {
            sal_free(l2_wbuf[unit]);
            l2_wbuf[unit] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
l2_learn_cache_cleanup(int unit)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_wbuf_t *wbuf = NULL;
    uint32_t pipe, seq, req_size;
    bool real_hw = bcmdrd_feature_is_real_hw(unit);

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        /* For each active pipe */
        seq = pi->act_seq[pipe];
        wbuf = &(l2_wbuf[unit][seq]);

        if (wbuf != NULL && wbuf->buf != NULL) {
            if (real_hw) {
                req_size = 4 * pi->entry_wsize * pi->tbl_size;
                bcmdrd_hal_dma_free(unit, req_size, wbuf->buf, wbuf->paddr);
            } else {
                sal_free(wbuf->buf);
            }
            wbuf->buf = NULL;
        }
    }

    if (l2_cache_entry_ha[unit]) {
        l2_cache_entry_ha[unit] = NULL;
    }
    if (l2_cache_ha[unit]) {
        l2_cache_ha[unit] = NULL;
    }
    if (l2_wbuf[unit]) {
        sal_free(l2_wbuf[unit]);
        l2_wbuf[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */
int
bcmcth_l2_learn_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Disable interrupt before software ready. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_enable_set(unit, 0, 0));

    SHR_IF_ERR_EXIT
        (l2_learn_control_init(unit, warm));

    SHR_IF_ERR_EXIT
        (l2_learn_cache_init(unit, warm));

    SHR_IF_ERR_EXIT
        (bcmcth_l2_learn_imm_register(unit));

    SHR_IF_ERR_EXIT
        (l2_learn_cache_info_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_cleanup(int unit)
{
    bcmcth_l2_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    /* Clean up device specific resource. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_cleanup(unit));

    SHR_IF_ERR_EXIT
        (l2_learn_cache_cleanup(unit));

    drv = bcmcth_l2_drv_get(unit);
    SHR_FREE(drv->learn_ctrl);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_start(int unit)
{
    bcmcth_l2_learn_ctrl_t *learn_ctrl;
    bcmcth_l2_drv_t *drv;
    bool sim = bcmdrd_feature_is_sim(unit);
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    report_ctrl = drv->report_ctrl;
    /* Warmboot */
    if (report_ctrl->enable == 0) {
        SHR_EXIT();
    }
    if (sim) {
        /*
         * Simulator cannot raise interrupt, wake up thread if report enabled.
         * Report ctrl enable status is recoverd after warmboot.
         */
        learn_ctrl->interval = L2_LEARN_INTERVAL_NORMAL;
    } else {
        learn_ctrl->interval = 0;
    }
    learn_ctrl->active = 1;

    /* Create L2 learn thread. */
    if (learn_ctrl->thread_ctrl == NULL) {
        learn_ctrl->thread_ctrl =
            shr_thread_start("bcmcthL2Learn", -1, l2_learn_thread,
                             (void *)(uintptr_t)unit);
        SHR_NULL_CHECK(learn_ctrl->thread_ctrl, SHR_E_FAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_handler_set(unit, l2_learn_intr_handler));

    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_enable_set(unit, 1, 0));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_stop(int unit)
{
    bcmcth_l2_learn_ctrl_t *learn_ctrl;
    bcmcth_l2_drv_t *drv;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable interrupt. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_enable_set(unit, 0, 0));

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    if (learn_ctrl == NULL) {
        SHR_EXIT();
    }

    learn_ctrl->active = 0;
    if (learn_ctrl->thread_ctrl != NULL) {
        rv = shr_thread_stop(learn_ctrl->thread_ctrl, 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "L2 learn thread will not exit.\n")));
        }
        learn_ctrl->thread_ctrl = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_report_enable(int unit, uint32_t trans_id, bool enable)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    report_ctrl = drv->report_ctrl;
    SHR_NULL_CHECK(report_ctrl, SHR_E_INIT);

    if (enable) {
        report_ctrl->enable = enable;

        SHR_IF_ERR_EXIT
            (bcmcth_l2_learn_start(unit));

        SHR_IF_ERR_EXIT
            (bcmcth_l2_drv_hw_enable_set(unit, trans_id, 1));
    } else {
        SHR_IF_ERR_EXIT
            (bcmcth_l2_drv_hw_enable_set(unit, trans_id, 0));

        SHR_IF_ERR_EXIT
            (bcmcth_l2_learn_stop(unit));

        report_ctrl->enable = enable;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_slow_poll_enable(int unit, bool slow_poll)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;
    bcmcth_l2_learn_ctrl_t *learn_ctrl;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    report_ctrl = drv->report_ctrl;
    learn_ctrl = drv->learn_ctrl;

    SHR_NULL_CHECK(report_ctrl, SHR_E_INIT);

    report_ctrl->slow_poll = slow_poll;
    if (slow_poll == 0) {
        learn_ctrl->interval = L2_LEARN_INTERVAL_AGRRESIVE;
        shr_thread_wake(learn_ctrl->thread_ctrl);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_queue_entry_delete(int unit, uint32_t pipe, uint32_t data_id)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_cache_t *cache = NULL;
    l2_ring_queue_t *rq = NULL;
    l2_cache_entry_t *elements = NULL;
    uint32_t seq = 0;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);
    cache = &(l2_cache[unit]);

    if (SHR_BITGET(pi->map, pipe) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* update cache entry status */
    seq = pi->act_seq[pipe];
    rq = cache->rq + seq;
    elements = cache->elements + seq * rq->size;
    l2_cache_entry_state_transit(unit, rq, elements, data_id, ENTRY_STAT_DEL);
    rq->del_sum++;

exit:
    SHR_FUNC_EXIT();
}
