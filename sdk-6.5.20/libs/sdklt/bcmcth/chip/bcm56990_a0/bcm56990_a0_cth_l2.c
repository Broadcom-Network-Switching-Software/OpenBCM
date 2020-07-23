/*! \file bcm56990_a0_cth_l2.c
 *
 * L2 learn drivers for bcm56990 A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/chip/bcm56990_a0_lp_intr.h>
#include <bcmbd/chip/bcm56990_a0_acc.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmlrd/chip/bcm56990_a0/bcm56990_a0_lrd_enum_ctype.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmptm/bcmptm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_util.h>

#include <bcmcth/bcmcth_l2_util.h>
#include <bcmcth/bcmcth_l2.h>

#define BSL_LOG_MODULE          BSL_LS_BCMCTH_L2

/* L2 fields array IMM handler. */
static shr_famm_hdl_t l2_fld_arr_hdl[BCML2_DEV_NUM_MAX];

static int l2_learn_intr[] = {
    QUAD0_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD1_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD2_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD3_IP_TO_CMIC_LEARN_CACHE_INTR
};


/* Following stuff to be reformed to generic layer */

#define L2_CACHE_QUEUE_SIZE_MAX         32
/*! L2 learn cache HA structure signature. */
#define L2_CACHE_HA_SIGN                0x05050A0A
/*! L2 learn cache entry HA structure signature. */
#define L2_CACHE_ENTRY_HA_SIGN          0xA5A50101

/*!
 *  L2 learn cache entry max size in words,
 *  get from L2_LEARN_CACHEm_SIZE and LEARN_CACHE_CACHEm_SIZE.
 */
#define L2_CACHE_ENTRY_WSIZE_MAX    4

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

    /*! Queue elements array. */
    l2_cache_entry_t *elements;

} l2_ring_queue_t;

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

typedef struct l2_cache_s {
    /*! Pipe count. */
    uint32_t pipe_cnt;

    /*! Ring queue for each pipe. */
    l2_ring_queue_t *rq;

} l2_cache_t;

static l2_wbuf_t *l2_wbuf[BCML2_DEV_NUM_MAX];
static l2_cache_t l2_cache[BCML2_DEV_NUM_MAX];

/* L2 cache entry HA structure. */
static bcmcth_l2_cache_entry_ha_t *l2_cache_entry_ha[BCML2_DEV_NUM_MAX];
static bcmcth_l2_cache_ha_t *l2_cache_ha[BCML2_DEV_NUM_MAX];

static void
l2_cache_entry_state_transit(int unit, l2_ring_queue_t *rq, uint32_t index,
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

    cur_state = rq->elements[index].status;
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

    rq->elements[index].status = new_state;
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
l2_ring_queue_iterate(int unit, l2_ring_queue_t *rq, void *data, uint32_t wsize)
{
    uint32_t idx;

    if (rq->count == 0) {
        return false;
    }

    for (idx = rq->front; idx != rq->rear; idx = (idx + 1) % rq->size) {
        if (rq->elements[idx].status == ENTRY_STAT_IDLE ||
            rq->elements[idx].status == ENTRY_STAT_DEL) {
            continue;
        }

        if (0 == sal_memcmp((void *)rq->elements[idx].data, data, wsize * 4)) {
            return true;
        }
    }

    return false;
}

static bool
l2_ring_queue_rpt_pop(int unit, l2_ring_queue_t *rq)
{
    /* Queue empty */
    if (rq->rpt_cnt == 0) {
        return false;
    }

    l2_cache_entry_state_transit(unit, rq, rq->report, ENTRY_STAT_REPORTED);
    rq->report = (rq->report + 1) % rq->size;
    rq->rpt_cnt--;
    return true;
}

static bool
l2_ring_queue_rpt_entry_get(int unit, l2_ring_queue_t *rq,
                            uint32_t *idx, void **data)
{
    /* Queue empty */
    if (rq->rpt_cnt == 0) {
        return false;
    }

    *idx = rq->report;
    *data = rq->elements[*idx].data;
    return true;
}

static bool
l2_ring_queue_push(int unit, l2_ring_queue_t *rq, void *data, uint32_t wsize)
{
    /* Queue full */
    if (rq->count == rq->size) {
        return false;
    }

    sal_memcpy((void *)rq->elements[rq->rear].data, data, wsize * 4);
    l2_cache_entry_state_transit(unit, rq, rq->rear, ENTRY_STAT_NEW);

    rq->rear = (rq->rear + 1) % rq->size;
    rq->count++;
    rq->rpt_cnt++;
    return true;
}

static bool
l2_ring_queue_pop(int unit, l2_ring_queue_t *rq)
{
    /* Queue empty or waiting for LT delete */
    if (rq->count == 0 ||
        rq->elements[rq->front].status != ENTRY_STAT_DEL) {
        return false;
    }

    l2_cache_entry_state_transit(unit, rq, rq->front, ENTRY_STAT_IDLE);
    rq->front = (rq->front + 1) % rq->size;
    rq->count--;
    return true;
}


static int
bcm56990_a0_cth_l2_entry_insert(int unit, uint32_t pipe, uint32_t index,
                                void *data)
{
    L2_LEARN_CACHEm_t *hw_entry;
    bcmltd_fields_t in_flds;
    int i = 0;
    uint32_t fval32[2] = {0};
    uint32_t type;

    SHR_FUNC_ENTER(unit);

    /* L2_LEARN_DATAt_MODIDf not mapped */
    in_flds.count = L2_LEARN_DATAt_FIELD_COUNT - 1;
    in_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit], in_flds.count);
    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    hw_entry = (L2_LEARN_CACHEm_t *)data;

    /* key fields */
    i = 0;
    in_flds.field[i]->id = L2_LEARN_DATAt_PIPEf;
    in_flds.field[i]->data = pipe;

    i++;
    in_flds.field[i]->id = L2_LEARN_DATAt_L2_LEARN_DATA_IDf;
    in_flds.field[i]->data = index;

    /* data fields */
    i++;
    in_flds.field[i]->id = L2_LEARN_DATAt_VLAN_IDf;
    in_flds.field[i]->data = L2_LEARN_CACHEm_L2_VLAN_IDf_GET(*hw_entry);

    i++;
    in_flds.field[i]->id = L2_LEARN_DATAt_MAC_ADDRf;
    L2_LEARN_CACHEm_L2_MAC_ADDRf_GET(*hw_entry, fval32);
    in_flds.field[i]->data = fval32[1];
    in_flds.field[i]->data = in_flds.field[i]->data << 32 | fval32[0];

    i++;
    in_flds.field[i]->id = L2_LEARN_DATAt_SRC_TYPEf;
    in_flds.field[i]->data = L2_LEARN_CACHEm_L2_DEST_TYPEf_GET(*hw_entry);
    type = L2_LEARN_CACHEm_L2_DEST_TYPEf_GET(*hw_entry);

    i++;
    in_flds.field[i]->id = L2_LEARN_DATAt_TRUNK_IDf;
    if (type == BCM56990_A0_LRD_L2_LEARN_SRC_T_T_TRUNK) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_TGIDf_GET(*hw_entry);
    }

    i++;
    in_flds.field[i]->id = L2_LEARN_DATAt_MODPORTf;
    if (type == BCM56990_A0_LRD_L2_LEARN_SRC_T_T_PORT) {
        in_flds.field[i]->data = L2_LEARN_CACHEm_L2_PORT_NUMf_GET(*hw_entry);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, L2_LEARN_DATAt, &in_flds));

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "LT insert entry: [%d][%d].\n"),
              pipe, index));

exit:
    if (in_flds.field != NULL) {
        shr_famm_free(l2_fld_arr_hdl[unit], in_flds.field, in_flds.count);
    }

    SHR_FUNC_EXIT();
}

static bool
bcm56990_a0_cth_l2_entry_valid(int unit, void *data)
{
    L2_LEARN_CACHEm_t *hw_entry;

    hw_entry = (L2_LEARN_CACHEm_t *)data;

    if (L2_LEARN_CACHEm_VALIDf_GET(*hw_entry) == 1) {
        return true;
    } else {
        return false;
    }
}

/* Traverse pt to populate entries to IMM. */
static int
bcm56990_a0_cth_l2_cache_traverse(int unit, l2_trav_status_t *status)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_cache_t *cache = NULL;
    l2_ring_queue_t *rq = NULL;
    l2_wbuf_t *wbuf = NULL;
    void *data = NULL;
    uint32_t *hw_entry = NULL;
    uint32_t pipe, seq, idx, data_id = 0;
    bool found, rv;
    int ret = SHR_E_NONE;
    uint32_t entry_queued = 0, entry_to_report = 0;

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

        ret = l2_wbuf_read(unit, pipe, wbuf);
        if (ret != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "WBuf[%d]: read failed.\n"), pipe));
        }

        do {
            rv = l2_ring_queue_pop(unit, rq);
            if (rv) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Pop[%d]: front %d, cnt %d.\n"),
                             pipe, rq->front, rq->count));
            }
        } while (rv == true);

        hw_entry = (uint32_t *)wbuf->buf;
        rq->fresh_entry = 0;
        for (idx = 0; idx < pi->tbl_size; idx++) {
            if (!bcm56990_a0_cth_l2_entry_valid(unit, hw_entry)) {
                break;
            }

            found = l2_ring_queue_iterate(unit, rq, hw_entry, pi->entry_wsize);
            if (!found) {
                rq->fresh_entry++;

                rv = l2_ring_queue_push(unit, rq, hw_entry, pi->entry_wsize);
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

        while (1) {
            rv = l2_ring_queue_rpt_entry_get(unit, rq, &data_id, &data);
            if (rv == false) {
                break;
            }

            ret = bcm56990_a0_cth_l2_entry_insert(unit, pipe, data_id, data);
            if (ret == SHR_E_NONE) {
                if (true == l2_ring_queue_rpt_pop(unit, rq)) {
                    rq->rpt_sum++;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "RptPop[%d]: report %d, "
                                                   "rptcnt %d.\n"),
                                 pipe, rq->report, rq->rpt_cnt));
                }
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Rpt[%d]: insert entry %d.\n"),
                          pipe, data_id));
                break;
            }
        }

        /* traverse status for current pipe */
        if (rq->count) {
            entry_queued = 1;
        }
        if (rq->rpt_cnt) {
            entry_to_report = 1;
        }
    }

    /* update traverse result */
    status->cache_empty = !entry_queued;
    status->new_learn = entry_to_report;

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_cth_l2_intr_enable(int unit, int enable)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
        if (enable) {
            bcmbd_cmicx_lp_intr_enable(unit, l2_learn_intr[idx]);
        } else {
            bcmbd_cmicx_lp_intr_disable_nosync(unit, l2_learn_intr[idx]);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_l2_intr_handler_set(int unit, bcmcth_l2_drv_intr_handler_f cb)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicx_lp_intr_func_set(unit, l2_learn_intr[idx], cb, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_l2_hw_enable(int unit, uint32_t trans_id, bool enable)
{
    L2_LEARN_COPY_CACHE_CTRLr_t cache_ctrl;
    LEARN_AT_EOPr_t learn_at_eop;

    SHR_FUNC_ENTER(unit);

    /* Enable Learn at EOP */
    SHR_IF_ERR_EXIT
        (READ_LEARN_AT_EOPr(unit, &learn_at_eop));
    LEARN_AT_EOPr_ENABLEf_SET(learn_at_eop, 1);
    SHR_IF_ERR_EXIT
        (WRITE_LEARN_AT_EOPr(unit, learn_at_eop));

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_read(unit,
                                      trans_id,
                                      L2_LEARN_CONTROLt,
                                      L2_LEARN_COPY_CACHE_CTRLr,
                                      0,
                                      0,
                                      &cache_ctrl));

    L2_LEARN_COPY_CACHE_CTRLr_CLEAR_ON_READ_ENf_SET(cache_ctrl, 1);
    L2_LEARN_COPY_CACHE_CTRLr_CACHE_INTERRUPT_CTRLf_SET(cache_ctrl, 1);
    L2_LEARN_COPY_CACHE_CTRLr_L2_LEARN_CACHE_ENf_SET(cache_ctrl, enable);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_write(unit,
                                       trans_id,
                                       L2_LEARN_CONTROLt,
                                       L2_LEARN_COPY_CACHE_CTRLr,
                                       0,
                                       0,
                                       &cache_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_l2_entry_delete(int unit, void *data)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_cache_t *cache = NULL;
    l2_ring_queue_t *rq = NULL;
    uint32_t pipe = 0, data_id = 0, seq = 0;
    const bcmltd_field_t *lt_field;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);
    cache = &(l2_cache[unit]);

    lt_field = (bcmltd_field_t *)data;
    while (lt_field) {
        fid = lt_field->id;
        switch (fid) {
        case L2_LEARN_DATAt_PIPEf:
            pipe = (uint8_t)lt_field->data;
            break;
        case L2_LEARN_DATAt_L2_LEARN_DATA_IDf:
            data_id = (uint16_t)lt_field->data;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

    if (SHR_BITGET(pi->map, pipe) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "LT delete entry: [%d][%d].\n"),
              pipe, data_id));

    /* update cache entry status */
    seq = pi->act_seq[pipe];
    rq = cache->rq + seq;
    l2_cache_entry_state_transit(unit, rq, data_id, ENTRY_STAT_DEL);
    rq->del_sum++;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_l2_entry_parse(int unit, const bcmltd_field_t *key,
                               const bcmltd_field_t *data, void *lt)
{
    l2_learn_control_info_t *lt_info = (l2_learn_control_info_t *)lt;
    const bcmltd_field_t *lt_field = data;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        switch (fid) {
        case L2_LEARN_CONTROLt_REPORTf:
            lt_info->enable = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_REPORT);
            break;

        case L2_LEARN_CONTROLt_SLOW_POLLf:
            lt_info->slow_poll = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_SLOW_POLL);
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_l2_init(int unit, bool warm)
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

    l2_cache[unit].pipe_cnt = pi->act_cnt;
    l2_cache[unit].rq = (l2_ring_queue_t *)l2_cache_ha[unit]->rq;


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

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        seq = pi->act_seq[pipe];
        rq = l2_cache[unit].rq + seq;

        rq->size = L2_CACHE_QUEUE_SIZE_MAX;
        rq->elements = (l2_cache_entry_t *)l2_cache_entry_ha[unit]->entry +
                       seq * L2_CACHE_QUEUE_SIZE_MAX;
    }

    /* IMM */
    SHR_IF_ERR_VERBOSE_EXIT
        (shr_famm_hdl_init(L2_LEARN_DATAt_FIELD_COUNT - 1,
                           &l2_fld_arr_hdl[unit]));

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
bcm56990_a0_cth_l2_cleanup(int unit)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    l2_wbuf_t *wbuf = NULL;
    uint32_t pipe, seq, req_size;
    bool real_hw = bcmdrd_feature_is_real_hw(unit);

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    if (l2_fld_arr_hdl[unit] != NULL) {
        shr_famm_hdl_delete(l2_fld_arr_hdl[unit]);
        l2_fld_arr_hdl[unit] = NULL;
    }

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

int
bcm56990_a0_cth_l2_attach(int unit)
{
    bcmcth_l2_drv_t *drv = bcmcth_l2_drv_get(unit);

    drv->cfg_sid = L2_LEARN_CONTROLt;
    drv->state_sid = L2_LEARN_DATAt;
    drv->state_ptsid = L2_LEARN_CACHEm;

    drv->init = bcm56990_a0_cth_l2_init;
    drv->cleanup = bcm56990_a0_cth_l2_cleanup;
    drv->hw_enable = bcm56990_a0_cth_l2_hw_enable;
    drv->intr_enable = bcm56990_a0_cth_l2_intr_enable;
    drv->intr_handler_set = bcm56990_a0_cth_l2_intr_handler_set;
    drv->cache_traverse = bcm56990_a0_cth_l2_cache_traverse;
    drv->entry_delete = bcm56990_a0_cth_l2_entry_delete;
    drv->entry_parse = bcm56990_a0_cth_l2_entry_parse;

    return SHR_E_NONE;
}
