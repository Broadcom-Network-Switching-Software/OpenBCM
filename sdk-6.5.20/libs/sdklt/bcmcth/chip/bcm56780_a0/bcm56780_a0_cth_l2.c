/*! \file bcm56780_a0_cth_l2.c
 *
 * L2 drivers for bcm56780 A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <shr/shr_ha.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmbd/bcmbd_ipep_intr.h>
#include <bcmbd/bcmbd_cmicx_lp_intr.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd.h>
#include <bcmcth/bcmcth_l2_util.h>
#include <bcmcth/bcmcth_l2.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmbd/chip/bcm56780_a0_lp_intr.h>
#include <bcmbd/chip/bcm56780_a0_ipep_intr.h>
#include <bcmdrd/bcmdrd_feature.h>

/******************************************************************************
* Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L2

typedef enum l2_learn_data_status_e {
    /* Entry of LEARN_CACHE_DATAt is idle. */
    ENTRY_IDLE = 0,

    /* Entry of L2_LEARN_DATA has been used to report Mac data. */
    ENTRY_REPORTED = 1,

    /* Mac data reported by LEARN_CACHE_DATAt has been inserted into L2 host table. */
    ENTRY_HW_DEL = 2

} l2_learn_data_status_t;

#define  MAX_PIPE_NUM     2

/*
 * These values are used for allocating both physical entry containers
 * and DMA buffers. A DMA buffer must be word-aligned, so the physical
 * entry size in bytes is rounded up to word size equivalent.
 */
#define ENTRY_WSIZE_LEARN_CACHE       SHR_BYTES2WORDS(LEARN_CACHE_CACHEm_SIZE)
#define ENTRY_BSIZE_LEARN_CACHE       SHR_WORDS2BYTES(ENTRY_WSIZE_LEARN_CACHE)
#define L2_LEARN_CACHE_DEPTH          2048

typedef enum l2_sw_fifo_entry_status_e {
    /* Mac data waits for reporting to users by LT LEARN_CACHE_DATAt. */
    ENTRY_NEW = 0,

    /* Mac data has been reported to users by LT LEARN_CACHE_DATAt. */
    ENTRY_PENDING = 1,

    /* Mac data has been inserted into L2_HOST_TABt. */
    ENTRY_HW_LEARN = 2,

    /* FIFO entry can be used to save new Mac data. */
    ENTRY_SW_DEL = 3

} l2_sw_fifo_entry_status_t;

/* Increase ring buffer index */
#define  RING_BUFFER_INDEX_INCR(_index ) \
    (((_index) + 1) == (L2_LEARN_CACHE_DEPTH)) ? 0 : ((_index) + 1)

typedef struct bcm56780_learn_cache_s {
    /* ENTRY_IDLE, ENTRY_REPORTED,  ENTRY_HW_DEL */
    l2_learn_data_status_t   reported;

    /* SW cache FIFO index */
    uint16_t                 fifo_index;
} bcm56780_sw_cache_t;

typedef struct bcm56780_learn_cache_data_s {
    /* ENTRY_NEW,  ENTRY_PENDING, ENTRY_HW_LEARN, ENTRY_SW_DEL */
    l2_sw_fifo_entry_status_t status;

    /* L2 learn data. */
    LEARN_CACHE_CACHEm_t      data;
} bcm56780_sw_cache_data_t;

typedef struct bcm56780_learn_cache_pipe_info_s {
    /* DMA buffer logic address. */
    void *buf_laddr;

    /* DMA buffer physical address. */
    uint64_t buf_paddr;

    /* DMA work data structure. */
    bcmbd_sbusdma_data_t work_data;

    /* DMA work structure. */
    bcmbd_sbusdma_work_t work;

    /* Learn cache FIFO. */
    bcm56780_sw_cache_data_t *learn_cache_fifo;

    /* New learn entry will be saved in this index */
    int learn_cache_fifo_tail;

    /* Retrieve learn entry from FIFO, then insert it to LT. */
    int learn_cache_fifo_head;

    /* Inserted to LEARN_CACHE_DATAt, however it may not be insert to L2 host table. */
    int learn_cache_fifo_pend;

    /* Status of every index of LT LEARN_CACHE_DATAt */
    bcm56780_sw_cache_t *lt_entry_status;

    /* Number of entries in FIFO which will be inserted into LEARN_CACHE_DATAt. */
    int new_fifo_num;

    /*
    * Number of all entries in FIFO, including entries will be inserted into LEARN_CACHE_DATAt,
    * and entries are saved in LEARN_CACHE_DATAt.
    */
    int fifo_num;

    /* New learn entry number during one DMA operation. */
    int new_learn_num;

    /* Latest index which is deleted by users. */
    int last_del_index;

    /* FIFO watermark, used to debugging. */
    int watermark;

    /* Number of all learn entries. */
    int learn_total_num;

} bcm56780_learn_cache_pipe_info_t;

typedef struct bcm56780_learn_cache_info_s {

    bcm56780_learn_cache_pipe_info_t pipe_learn_cache[MAX_PIPE_NUM];

} bcm56780_learn_cache_info_t;

static bcm56780_learn_cache_info_t learn_cache_info[BCML2_DEV_NUM_MAX];

typedef struct bcm56780_cache_fld_s {
    /* Cache field ID */
    uint32_t    fld_id;
    /* Start bit position */
    uint8_t     start_bit;
    /* End bit position */
    uint8_t     end_bit;
} bcm56780_cache_fld_t;

static bcm56780_cache_fld_t learn_cache_fld[] = {
    { LEARN_CACHE_DATAt_CACHE_FIELD0f, 0,   15 },
    { LEARN_CACHE_DATAt_CACHE_FIELD1f, 16,  31 },
    { LEARN_CACHE_DATAt_CACHE_FIELD2f, 32,  47 },
    { LEARN_CACHE_DATAt_CACHE_FIELD3f, 48,  63 },
    { LEARN_CACHE_DATAt_CACHE_FIELD4f, 64,  79 },
    { LEARN_CACHE_DATAt_CACHE_FIELD5f, 80,  95 },
    { LEARN_CACHE_DATAt_CACHE_FIELD6f, 96,  99 },
    { LEARN_CACHE_DATAt_CACHE_FIELD7f, 100, 103 }
};

/*! L2 fields array lmm handler. */
static shr_famm_hdl_t l2_fld_arr_hdl[BCML2_DEV_NUM_MAX];

static int l2_learn_intr[] = {
    PIPE0_IP_TO_CMIC_INTR,
    PIPE1_IP_TO_CMIC_INTR,
};
#define L2_LEARN_INTR(pipe) (l2_learn_intr[pipe])
#define L2_LEARN_INTR_CNT (COUNTOF(l2_learn_intr))

/******************************************************************************
* Private functions
 */
/* Check whether L2 learn entry is duplicate. */
static bool
learn_data_is_duplicate(int end, int start, LEARN_CACHE_CACHEm_t *lc_entry,
                        bcm56780_sw_cache_data_t *cache_fifo)
{
    LEARN_CACHE_CACHEm_t *fifo_entry;
    int i, k;

    for (i = end; i >= start; i--) {
        k = 0;
        fifo_entry = &(cache_fifo[i].data);
        while (k < ENTRY_WSIZE_LEARN_CACHE) {
            if (LEARN_CACHE_CACHEm_GET(*lc_entry, k) !=
                LEARN_CACHE_CACHEm_GET(*fifo_entry, k)) {
                break;
            }
            k++;
        }
        /* found */
        if (k == ENTRY_WSIZE_LEARN_CACHE) {
            return true;
        }
    }
    return false;
}

/* Get new pending index. */
static void
learn_fifo_entry_pending_index_get(int pend, int head,
                                   bcm56780_sw_cache_data_t *cache_fifo,
                                   int *new_pend, int *fifo_num_decr,
                                   int *succession)
{
    int i = 0;

    for (i = pend; i < head; i++) {
        if (*succession &&
            cache_fifo[i].status == ENTRY_SW_DEL) {
            *new_pend = RING_BUFFER_INDEX_INCR(*new_pend);
            *fifo_num_decr += 1;
        } else if (cache_fifo[i].status == ENTRY_HW_LEARN) {
            cache_fifo[i].status = ENTRY_SW_DEL;
            *succession = 0;
        } else {
            *succession = 0;
        }
    }
}

/* Add the learnt MAC to L2_LEARN_DATA. */
static int
learn_cache_data_insert(int unit, learn_cache_t *data)
{
    bcmltd_fields_t in_flds;
    int i = 0, fld_idx;
    int num = COUNTOF(learn_cache_fld);

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Insert data to learn_cache_data_id=%d, pipe=%d\n"),
                 data->learn_cache_data_id, data->pipe));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\t cache_field0=%d, cache_field1=%d, cache_field2=%d, cache_field3=%d,"
                          " cache_field4=%d, cache_field5=%d, cache_field6=%d, cache_field7=%d\n"),
              data->cache_flds.data[0], data->cache_flds.data[1],
              data->cache_flds.data[2], data->cache_flds.data[3],
              data->cache_flds.data[4], data->cache_flds.data[5],
              data->cache_flds.data[6], data->cache_flds.data[7]));

    in_flds.count = LEARN_CACHE_DATAt_FIELD_COUNT;
    in_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit],
                                   LEARN_CACHE_DATAt_FIELD_COUNT);
    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Key fields */
    i = 0;
    in_flds.field[i]->id   = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_IDf;
    in_flds.field[i]->data = data->learn_cache_data_id;

    i++;
    in_flds.field[i]->id   = LEARN_CACHE_DATAt_PIPEf;
    in_flds.field[i]->data = data->pipe;

    /* Data fields */
    i++;
    for (fld_idx = 0; fld_idx < num; i++, fld_idx++) {
        in_flds.field[i]->id   = learn_cache_fld[fld_idx].fld_id;
        in_flds.field[i]->data = data->cache_flds.data[fld_idx];
    }
    in_flds.field[i]->id   = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_INFO_IDf;
    in_flds.field[i]->data = data->profile_idx;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, LEARN_CACHE_DATAt, &in_flds));

exit:
    if (in_flds.field != NULL) {
        shr_famm_free(l2_fld_arr_hdl[unit], in_flds.field, in_flds.count);
    }
    SHR_FUNC_EXIT();
}

static void
learn_drv_lc_to_learn_cache_data(int unit, LEARN_CACHE_CACHEm_t lc_entry,
                                 learn_cache_t *data)
{
    uint32_t   cache_entry[4] = {0};
    uint32_t   profile_idx = 0;
    uint32_t   fval32 = 0, fld_idx = 0, num = COUNTOF(learn_cache_fld);
    int start, end;

    LEARN_CACHE_CACHEm_CACHE_KEYf_GET(lc_entry, cache_entry);

    for ( ; fld_idx < num ; fld_idx++) {
        start = learn_cache_fld[fld_idx].start_bit;
        end = learn_cache_fld[fld_idx].end_bit;
        (void)bcmdrd_field_get(cache_entry, start, end, &fval32);
        data->cache_flds.data[fld_idx] = (uint16_t)fval32;
    }
    profile_idx = LEARN_CACHE_CACHEm_CACHE_KEY_TYPEf_GET(lc_entry);
    data->profile_idx = (uint8_t)profile_idx;
    return;
}

static int
learn_cache_fields_data_to_fifo_entry(int unit, bcmltd_fields_t *out_flds,
                                      LEARN_CACHE_CACHEm_t *fifo_entry)
{
    uint32_t   fval32 = 0, fld_idx, num = COUNTOF(learn_cache_fld);
    int found, start, end;
    LEARN_CACHE_CACHEm_t *lc_entry = (LEARN_CACHE_CACHEm_t *)fifo_entry;
    uint32_t cache_entry[4] = {0};
    size_t i;

    SHR_FUNC_ENTER(unit);

    for (fld_idx = 0; fld_idx < num; fld_idx++) {
        found = 0;
        for (i = 0; i < out_flds->count; i++) {
            if (out_flds->field[i]->id == learn_cache_fld[fld_idx].fld_id) {
                found = 1;
                break;
            }
        }
        if (found == 0) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        start = learn_cache_fld[fld_idx].start_bit;
        end = learn_cache_fld[fld_idx].end_bit;
        fval32 = out_flds->field[i]->data;
        (void)bcmdrd_field_set(cache_entry, start, end, &fval32);
    }
    LEARN_CACHE_CACHEm_VALID_EOPf_SET(*lc_entry, 1);

    LEARN_CACHE_CACHEm_CACHE_KEYf_SET(*lc_entry, cache_entry);

    found = 0;
    for (i = 0; i < out_flds->count; i++) {
        if (out_flds->field[i]->id == LEARN_CACHE_DATAt_LEARN_CACHE_DATA_INFO_IDf) {
            found = 1;
            break;
        }
    }
    if (found == 0) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    fval32 = out_flds->field[i]->data;
    LEARN_CACHE_CACHEm_CACHE_KEY_TYPEf_SET(*lc_entry, fval32);

exit:
    SHR_FUNC_EXIT();
}

static void
bcm56780_a0_cth_l2_learn_cache_info_deinit(int unit)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    int pipe, buffer_size;
    bool sim = bcmdrd_feature_is_sim(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    for (pipe = 0; pipe < MAX_PIPE_NUM; pipe++) {
        pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];

        buffer_size = ENTRY_BSIZE_LEARN_CACHE * pi->tbl_size;
        if (pipe_cache_ptr->buf_laddr) {
            if (sim) {
                sal_free(pipe_cache_ptr->buf_laddr);
            } else {
                bcmdrd_hal_dma_free(unit, buffer_size,
                                    pipe_cache_ptr->buf_laddr,
                                    pipe_cache_ptr->buf_paddr);
            }
            pipe_cache_ptr->buf_laddr = NULL;
        }

        if (pipe_cache_ptr->lt_entry_status) {
            sal_free(pipe_cache_ptr->lt_entry_status);
            pipe_cache_ptr->lt_entry_status = NULL;
        }

        if (pipe_cache_ptr->learn_cache_fifo) {
            sal_free(pipe_cache_ptr->learn_cache_fifo);
            pipe_cache_ptr->learn_cache_fifo = NULL;
        }
    }
}

static int
bcm56780_a0_cth_l2_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (l2_fld_arr_hdl[unit] != NULL) {
        shr_famm_hdl_delete(l2_fld_arr_hdl[unit]);
        l2_fld_arr_hdl[unit] = NULL;
    }

    bcm56780_a0_cth_l2_learn_cache_info_deinit(unit);

    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_intr_enable(int unit, int enable)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t pipe;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    if (enable) {
        SHR_IF_ERR_EXIT
            (bcmbd_ipep_intr_enable(unit, LEARN_CACHE_INTR));

        SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
            bcmbd_cmicx_lp_intr_enable(unit, L2_LEARN_INTR(pipe));
        }
    } else {
        SHR_IF_ERR_EXIT
            (bcmbd_ipep_intr_disable(unit, LEARN_CACHE_INTR));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_intr_handler_set(
    int unit,
    bcmcth_l2_drv_intr_handler_f  learn_isr)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ipep_intr_func_set(unit, LEARN_CACHE_INTR, learn_isr, 0));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_learn_cache_dma_read(int unit, int pipe)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    int rv = SHR_E_NONE;
    uint32_t i;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    bcmbd_sbusdma_work_t *work_ptr;
    bool sim = bcmdrd_feature_is_sim(unit);
    uint32_t *pio_ptr = NULL;
    LEARN_CACHE_CACHEm_t *lc_entry;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];

    if (sim == 0) {
        work_ptr = &pipe_cache_ptr->work;

        rv = bcmbd_sbusdma_light_work_init(unit, work_ptr);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmbd_sbusdma_work_execute(unit, work_ptr);
        SHR_IF_ERR_EXIT(rv);
    } else {
        pio_ptr = (uint32_t *)pipe_cache_ptr->buf_laddr;

        for (i = 0; i < pi->tbl_size; i++) {
            rv = bcmcth_l2_per_pipe_ireq_read(unit, LEARN_CACHE_DATAt,
                                              LEARN_CACHE_CACHEm,
                                              pipe, i, (void *)pio_ptr);
            SHR_IF_ERR_EXIT(rv);

            lc_entry = (LEARN_CACHE_CACHEm_t *)pio_ptr;
            if (!LEARN_CACHE_CACHEm_VALID_EOPf_GET(*lc_entry)) {
                break;
            }
            pio_ptr += ENTRY_WSIZE_LEARN_CACHE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bcm56780_a0_cth_l2_learn_fifo_status_set(int unit, int pipe,
                                         int fifo_index)
{
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    bcm56780_sw_cache_data_t *cache_fifo;

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
    cache_fifo = pipe_cache_ptr->learn_cache_fifo;

    cache_fifo[fifo_index].status = ENTRY_HW_LEARN;
}

static void
bcm56780_a0_cth_l2_learn_pend_index_calc(int unit, int pipe)
{
    int pend, head, tail;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    bcm56780_sw_cache_data_t *cache_fifo;
    int succession = 1, new_pend, fifo_num_decr = 0;

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
    if (pipe_cache_ptr->fifo_num == 0) {
        return;
    }
    pend = pipe_cache_ptr->learn_cache_fifo_pend;
    head = pipe_cache_ptr->learn_cache_fifo_head;
    tail = pipe_cache_ptr->learn_cache_fifo_tail;
    cache_fifo = pipe_cache_ptr->learn_cache_fifo;
    new_pend = pend;

    if (pipe_cache_ptr->fifo_num == L2_LEARN_CACHE_DEPTH &&
        head == tail &&
        cache_fifo[pend].status == ENTRY_PENDING) {
        /* User forget to or fail to delete the entry by LEARN_CACHE_DATA. */
        cache_fifo[pend].status = ENTRY_SW_DEL;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "User should delete learn data by LEARN_CACHE_DATA.\n")));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Tail %d, Head %d, Pending %d.\n"),
                 tail, head, pend));
    }
    if (pend == head) {
        if (cache_fifo[pend].status == ENTRY_SW_DEL) {
            fifo_num_decr++;
        } else if (cache_fifo[pend].status == ENTRY_HW_LEARN) {
            cache_fifo[pend].status = ENTRY_SW_DEL;
        }
    } else if (pend < head) {
        learn_fifo_entry_pending_index_get(pend, head, cache_fifo, &new_pend,
                                           &fifo_num_decr, &succession);
    } else {
        learn_fifo_entry_pending_index_get(pend, L2_LEARN_CACHE_DEPTH,
                                           cache_fifo, &new_pend,
                                           &fifo_num_decr, &succession);

        learn_fifo_entry_pending_index_get(0, head, cache_fifo, &new_pend,
                                           &fifo_num_decr, &succession);
    }
    pipe_cache_ptr->learn_cache_fifo_pend = new_pend;
    pipe_cache_ptr->fifo_num -= fifo_num_decr;
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Learn pending entry from [%d] to [%d], num [%d].\n"),
               new_pend, head, pipe_cache_ptr->fifo_num));
}

static bool
bcm56780_a0_cth_l2_learn_cache_fifo_push(int unit, int pipe,
                                         LEARN_CACHE_CACHEm_t *lc_entry)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    int tail_new, tail, pend, found;
    uint32_t i;
    bcm56780_sw_cache_data_t *cache_fifo = NULL;
    bool full = false;

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
    tail = pipe_cache_ptr->learn_cache_fifo_tail;
    pend = pipe_cache_ptr->learn_cache_fifo_pend;
    cache_fifo = pipe_cache_ptr->learn_cache_fifo;

    if (pipe_cache_ptr->fifo_num == L2_LEARN_CACHE_DEPTH) {
        return true;
    }
    tail_new = tail;
    pipe_cache_ptr->new_learn_num = 0;
    for (i = 0; i < pi->tbl_size; i++, lc_entry++) {
        if (!LEARN_CACHE_CACHEm_VALID_EOPf_GET(*lc_entry)) {
            break;
        }
        found = false;
        if (tail_new >= pend) {
            found = learn_data_is_duplicate(tail_new, pend, lc_entry,
                                            cache_fifo);
        } else {
            found = learn_data_is_duplicate(tail_new, 0, lc_entry,
                                            cache_fifo);
            if (found == false) {
                found = learn_data_is_duplicate(L2_LEARN_CACHE_DEPTH - 1,
                                                pend, lc_entry,
                                                cache_fifo);
            }
        }
        if (found && pipe_cache_ptr->fifo_num != 0) {
            continue;
        } else {
            if (pipe_cache_ptr->fifo_num == L2_LEARN_CACHE_DEPTH) {
                full = true;
                break;
            }
            if (tail == pend && pipe_cache_ptr->fifo_num == 0) {
                tail_new = pipe_cache_ptr->learn_cache_fifo_tail;
            } else {
                tail_new = pipe_cache_ptr->learn_cache_fifo_tail;
                tail_new = RING_BUFFER_INDEX_INCR(tail_new);
                pipe_cache_ptr->learn_cache_fifo_tail = tail_new;
            }
            sal_memcpy(&(cache_fifo[tail_new].data), lc_entry,
                       ENTRY_BSIZE_LEARN_CACHE);

            cache_fifo[tail_new].status = ENTRY_NEW;

            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Push entry to learn cache fifo [%d].[%d], num [%d].\n"),
                       pipe, tail_new, pipe_cache_ptr->fifo_num));
            pipe_cache_ptr->fifo_num++;
            pipe_cache_ptr->new_fifo_num++;
            if (pipe_cache_ptr->fifo_num > pipe_cache_ptr->watermark) {
                pipe_cache_ptr->watermark = pipe_cache_ptr->fifo_num;
            }
            pipe_cache_ptr->new_learn_num++;
        }
    }
    pipe_cache_ptr->learn_total_num += pipe_cache_ptr->new_learn_num;

    return full;
}

static bool
bcm56780_a0_cth_l2_learn_cache_fifo_pop(int unit, int pipe,
                                        learn_cache_t *entry_data,
                                        int *fifo_index)
{
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    int head, tail;
    bcm56780_sw_cache_data_t *cache_fifo = NULL;
    LEARN_CACHE_CACHEm_t *lc_entry;

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
    head = pipe_cache_ptr->learn_cache_fifo_head;
    tail = pipe_cache_ptr->learn_cache_fifo_tail;
    cache_fifo = pipe_cache_ptr->learn_cache_fifo;

    if (pipe_cache_ptr->new_fifo_num == 0) {
        return true;
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "Pop entry from learn cache fifo [%d].[%d], num [%d].\n"),
               pipe, head, pipe_cache_ptr->new_fifo_num));

    if (cache_fifo[head].status != ENTRY_NEW) {
        head = RING_BUFFER_INDEX_INCR(head);
    }
    if (cache_fifo[head].status != ENTRY_NEW) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "There is internal error!!!.\n")));
    }
    sal_memset(entry_data, 0, sizeof(learn_cache_t));
    lc_entry = &cache_fifo[head].data;
    learn_drv_lc_to_learn_cache_data(unit, *lc_entry, entry_data);

    *fifo_index = head;
    cache_fifo[head].status = ENTRY_PENDING;
    if (head != tail) {
        head = RING_BUFFER_INDEX_INCR(head);
    }
    pipe_cache_ptr->learn_cache_fifo_head = head;
    pipe_cache_ptr->new_fifo_num--;

    return false;
}

static bool
bcm56780_a0_cth_l2_lt_entry_index_find(int unit, int pipe, int *index)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    bcm56780_sw_cache_t *lt_entry_status = NULL;
    uint32_t i;

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
    lt_entry_status = pipe_cache_ptr->lt_entry_status;
    i = 0;
    for (; i < pi->tbl_size * 2; i++) {
        if (lt_entry_status[i].reported == ENTRY_IDLE) {
            *index = i;
            return true;
        }
    }
    return false;
}

/* Traverse HW L2_LEARN_CACHEm to populate entries to SW cache. */
static int
bcm56780_a0_cth_l2_cache_traverse(int unit, l2_trav_status_t *status)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    int rv  = 0, lt_index;
    uint32_t insert_time;
    uint32_t pipe;
    learn_cache_t learn_cache_data;
    int valid_entry = 0;
    int full, empty, lt_no_full, fifo_index;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    bcm56780_sw_cache_t *lt_entry_status = NULL;
    LEARN_CACHE_CACHEm_t *lc_entry;

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];

        rv = bcm56780_a0_cth_l2_learn_cache_dma_read(unit, pipe);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to read data from pipe=%d by DMA mode\n"),
                       pipe));
            continue;
        }
        /* Calucate pending index. */
        bcm56780_a0_cth_l2_learn_pend_index_calc(unit, pipe);

        lc_entry = (LEARN_CACHE_CACHEm_t *)pipe_cache_ptr->buf_laddr;
        full = bcm56780_a0_cth_l2_learn_cache_fifo_push(unit, pipe, lc_entry);
        if (full) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "L2 learn fifo cache of pipe [%d] is full.\n"),
                         pipe));
        }
        if (pipe_cache_ptr->fifo_num) {
            valid_entry = 1;
        }
        if (pipe_cache_ptr->new_fifo_num) {
            status->new_learn = 1;
        }
        lt_entry_status = pipe_cache_ptr->lt_entry_status;
        insert_time = 0;
        while (insert_time < pi->tbl_size * 2) {
            lt_no_full = bcm56780_a0_cth_l2_lt_entry_index_find(unit, pipe, &lt_index);
            if (!lt_no_full) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "L2 learn cache data LT for pipe [%d]"
                                        " is full.\n"), pipe));
                break;
            }
            empty = bcm56780_a0_cth_l2_learn_cache_fifo_pop(unit, pipe,
                                                            &learn_cache_data,
                                                            &fifo_index);
            if (empty) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "L2 learn fifo cache of pipe [%d]"
                                        "is empty.\n"), pipe));
                break;
            }
            learn_cache_data.learn_cache_data_id = lt_index;
            learn_cache_data.pipe = pipe;
            lt_entry_status[lt_index].reported = ENTRY_REPORTED;
            lt_entry_status[lt_index].fifo_index = fifo_index;
            rv = learn_cache_data_insert(unit, &learn_cache_data);
            if (rv < SHR_E_NONE) {
                lt_entry_status[lt_index].reported = ENTRY_IDLE;
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to insert new entry to"
                                      " learn cache data LT (%d).\n"), rv));
                break;
            }
            insert_time++;
        }
    }
    status->cache_empty = !valid_entry;

    return SHR_E_NONE;
}

static int
bcm56780_a0_cth_l2_entry_delete(int unit, void *data)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    int index = 0, pipe = 0, last_index = 0;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    bcm56780_sw_cache_t *lt_entry_status = NULL;
    const bcmltd_field_t *lt_field;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    lt_field = (bcmltd_field_t *)data;
    while (lt_field) {
        fid = lt_field->id;
        switch (fid) {
        case LEARN_CACHE_DATAt_PIPEf:
            pipe = (uint8_t)lt_field->data;
            break;
        case LEARN_CACHE_DATAt_LEARN_CACHE_DATA_IDf:
            index = (uint16_t)lt_field->data;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

    if (SHR_BITGET(pi->map, pipe) == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
    lt_entry_status = pipe_cache_ptr->lt_entry_status;
    lt_entry_status[index].reported = ENTRY_HW_DEL;

    bcm56780_a0_cth_l2_learn_fifo_status_set(unit, pipe,
                                             lt_entry_status[index].fifo_index);

    last_index = pipe_cache_ptr->last_del_index;
    if (lt_entry_status[last_index].reported == ENTRY_HW_DEL) {
        lt_entry_status[last_index].reported = ENTRY_IDLE;
    }
    pipe_cache_ptr->last_del_index = index;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "delete data from learn_cache_data_id=%d, pipe=%d\n"),
                 index, pipe));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_entry_parse(int unit, const bcmltd_field_t *key,
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
        case LEARN_CACHE_CONTROLt_REPORTf:
            lt_info->enable = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_REPORT);
            break;

        case LEARN_CACHE_CONTROLt_SLOW_POLLf:
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
bcm56780_a0_cth_l2_hw_enable(int unit, uint32_t trans_id, bool enable)
{
    LEARN_CACHE_CTRLr_t  cache_ctrl;
    LEARN_CACHE_INTR_ENABLEr_t  intr_enable;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_read(unit,
                                      trans_id,
                                      LEARN_CACHE_CONTROLt,
                                      LEARN_CACHE_CTRLr,
                                      0,
                                      0,
                                      &cache_ctrl));
    LEARN_CACHE_CTRLr_CLEAR_ON_READ_ENf_SET(cache_ctrl, 1);
    LEARN_CACHE_CTRLr_CLEAR_ENTRY_ON_PURGEf_SET(cache_ctrl, enable);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_write(unit,
                                       trans_id,
                                       LEARN_CACHE_CONTROLt,
                                       LEARN_CACHE_CTRLr,
                                       0,
                                       0,
                                       &cache_ctrl));
    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_read(unit,
                                      trans_id,
                                      LEARN_CACHE_CONTROLt,
                                      LEARN_CACHE_INTR_ENABLEr,
                                      0,
                                      0,
                                      &intr_enable));
    LEARN_CACHE_INTR_ENABLEr_LEARN_CACHE_NOT_EMPTYf_SET
                                       (intr_enable, enable);
    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_write(unit,
                                       trans_id,
                                       LEARN_CACHE_CONTROLt,
                                       LEARN_CACHE_INTR_ENABLEr,
                                       0,
                                       0,
                                       &intr_enable));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_learn_cache_sync(int unit)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcmltd_fields_t in_flds, out_flds;
    int index = 0, max_index, rv, fifo_index;
    bcm56780_sw_cache_t *lt_entry_status = NULL;
    uint32_t pipe;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    LEARN_CACHE_CACHEm_t lc_fifo_entry[2];
    learn_cache_t entry_data;
    bool empty, full;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    in_flds.field = NULL;
    out_flds.field = NULL;

    /* Key fields */
    in_flds.count = 2;
    in_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit], 2);
    SHR_NULL_CHECK(in_flds.field, SHR_E_MEMORY);

    /* Data fields */
    out_flds.count = 9;
    out_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit], 9);
    SHR_NULL_CHECK(out_flds.field, SHR_E_MEMORY);

    max_index = pi->tbl_size * 2;

    in_flds.field[0]->id   = LEARN_CACHE_DATAt_LEARN_CACHE_DATA_IDf;
    in_flds.field[1]->id   = LEARN_CACHE_DATAt_PIPEf;

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe) {
        pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
        lt_entry_status = pipe_cache_ptr->lt_entry_status;
        for (index = 0; index < max_index; index++) {
            in_flds.field[0]->data = index;
            in_flds.field[1]->data = pipe;
            rv = bcmimm_entry_lookup(unit, LEARN_CACHE_DATAt, &in_flds, &out_flds);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
            if (rv == SHR_E_NOT_FOUND) {
                lt_entry_status[index].reported = ENTRY_IDLE;
                continue;
            }
            lt_entry_status[index].reported = ENTRY_REPORTED;

            sal_memset(lc_fifo_entry, 0, sizeof(lc_fifo_entry));
            SHR_IF_ERR_EXIT
                (learn_cache_fields_data_to_fifo_entry(unit, &out_flds,
                                                       lc_fifo_entry));

            full = bcm56780_a0_cth_l2_learn_cache_fifo_push(unit, pipe,
                                                            lc_fifo_entry);
            if (full) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            empty = bcm56780_a0_cth_l2_learn_cache_fifo_pop(unit, pipe,
                                                            &entry_data,
                                                            &fifo_index);
            if (empty) {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            lt_entry_status[index].fifo_index = fifo_index;
        }
    }

exit:
    if (in_flds.field) {
        shr_famm_free(l2_fld_arr_hdl[unit], in_flds.field, 2);
    }
    if (out_flds.field) {
        shr_famm_free(l2_fld_arr_hdl[unit], out_flds.field, 9);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_learn_cache_info_init(int unit)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    bcm56780_learn_cache_info_t *cache_ptr = &learn_cache_info[unit];
    bcm56780_learn_cache_pipe_info_t *pipe_cache_ptr = NULL;
    int pipe = 0, buffer_size, rv;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t cwords_array[4], num_cwords;
    bcmbd_sbusdma_data_t *work_data_ptr;
    bcmbd_sbusdma_work_t *work_ptr;
    int req_size;
    bool sim = bcmdrd_feature_is_sim(unit);

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    for (pipe = 0; pipe < MAX_PIPE_NUM; pipe++) {
        pipe_cache_ptr = &cache_ptr->pipe_learn_cache[pipe];
        sal_memset(pipe_cache_ptr, 0, sizeof(bcm56780_learn_cache_pipe_info_t));

        req_size =  pi->tbl_size * 2;
        req_size *= sizeof(bcm56780_sw_cache_t);
        pipe_cache_ptr->lt_entry_status = sal_alloc(req_size,
                                                    "bcmcthL2LearnDrvCache");
        SHR_NULL_CHECK(pipe_cache_ptr->lt_entry_status, SHR_E_MEMORY);
        sal_memset(pipe_cache_ptr->lt_entry_status, 0, req_size);

        req_size = L2_LEARN_CACHE_DEPTH * sizeof(bcm56780_sw_cache_data_t);
        pipe_cache_ptr->learn_cache_fifo = sal_alloc(req_size,
                                                     "bcmcthL2LearnCacheData");
        SHR_NULL_CHECK(pipe_cache_ptr->learn_cache_fifo, SHR_E_MEMORY);
        sal_memset(pipe_cache_ptr->learn_cache_fifo, 0, req_size);

        work_data_ptr = &pipe_cache_ptr->work_data;
        work_ptr = &pipe_cache_ptr->work;

        sal_memset(work_data_ptr, 0, sizeof(bcmbd_sbusdma_data_t));
        sal_memset(work_ptr, 0, sizeof(bcmbd_sbusdma_work_t));

        buffer_size = ENTRY_BSIZE_LEARN_CACHE * pi->tbl_size;
        if (sim) {
            pipe_cache_ptr->buf_laddr = sal_alloc(buffer_size, "bcmptmL2PioLearn");
        } else {
            pipe_cache_ptr->buf_laddr = bcmdrd_hal_dma_alloc(unit, buffer_size,
                                                             "bcmptmL2DmaLearn",
                                                             &pipe_cache_ptr->buf_paddr);
        }
        if (NULL == pipe_cache_ptr->buf_laddr) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to allocate DMA buffer used to clear PTs.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
        }
        sal_memset(pipe_cache_ptr->buf_laddr, 0, buffer_size);
        if (sim) {
            /* Don't use DMA on sim */
            continue;
        }
        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe;
        rv = bcmbd_pt_cmd_hdr_get(unit, LEARN_CACHE_CACHEm, &pt_dyn_info, NULL,
                                  BCMBD_PT_CMD_READ, 4, cwords_array, &num_cwords);
        SHR_IF_ERR_EXIT(rv);

        work_data_ptr->start_addr = cwords_array[1];
        work_data_ptr->op_count = pi->tbl_size;
        work_data_ptr->data_width = ENTRY_WSIZE_LEARN_CACHE;
        work_data_ptr->op_code = cwords_array[0];
        work_data_ptr->pend_clocks =
            bcmbd_mor_clks_read_get(unit, LEARN_CACHE_CACHEm);
        work_data_ptr->buf_paddr = pipe_cache_ptr->buf_paddr;

        work_ptr->data = work_data_ptr;
        work_ptr->items = 1;
        work_ptr->flags = SBUSDMA_WF_READ_CMD;
        work_ptr->queue = 2;


    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_l2_init(int unit, bool warm)
{
    uint32_t index;
    uint32_t profile_depth;
    LEARN_CACHE_BITP_PROFILEm_t  bitp_profile;

    SHR_FUNC_ENTER(unit);

    if (l2_fld_arr_hdl[unit] == NULL) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(LEARN_CACHE_DATAt_FIELD_COUNT,
                               &l2_fld_arr_hdl[unit]));
    }

    SHR_IF_ERR_EXIT
        (bcm56780_a0_cth_l2_learn_cache_info_init(unit));

    if (warm) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_cth_l2_learn_cache_sync(unit));
    } else {
        profile_depth =
            bcmdrd_pt_index_max(unit, LEARN_CACHE_BITP_PROFILEm) + 1;
        for (index = 0; index < profile_depth; index++) {
            SHR_IF_ERR_EXIT
                (bcmcth_l2_per_pipe_ireq_read(unit,
                                              LEARN_CACHE_DATAt,
                                              LEARN_CACHE_BITP_PROFILEm,
                                              0,
                                              index,
                                              &bitp_profile));
            LEARN_CACHE_BITP_PROFILEm_CACHE_KEY_TYPEf_SET(bitp_profile, index);

            SHR_IF_ERR_EXIT
                (bcmcth_l2_per_pipe_ireq_write(unit,
                                               LEARN_CACHE_DATAt,
                                               LEARN_CACHE_BITP_PROFILEm,
                                               0,
                                               index,
                                               &bitp_profile));
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcm56780_a0_cth_l2_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */
int
bcm56780_a0_cth_l2_attach(int unit)
{
    bcmcth_l2_drv_t *drv = bcmcth_l2_drv_get(unit);

    drv->cfg_sid = LEARN_CACHE_CONTROLt;
    drv->state_sid = LEARN_CACHE_DATAt;
    drv->state_ptsid = LEARN_CACHE_CACHEm;

    drv->init = bcm56780_a0_cth_l2_init;
    drv->cleanup = bcm56780_a0_cth_l2_cleanup;
    drv->hw_enable = bcm56780_a0_cth_l2_hw_enable;
    drv->intr_enable = bcm56780_a0_cth_l2_intr_enable;
    drv->intr_handler_set = bcm56780_a0_cth_l2_intr_handler_set;
    drv->cache_traverse = bcm56780_a0_cth_l2_cache_traverse;
    drv->entry_delete = bcm56780_a0_cth_l2_entry_delete;
    drv->entry_parse = bcm56780_a0_cth_l2_entry_parse;

    return SHR_E_NONE;
}

