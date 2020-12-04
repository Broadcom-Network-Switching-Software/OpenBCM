/*! \file bcmcth_ldh_ctrl.c
 *
 * Latency Distribution Histogram controller
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_ldh.h>
#include <bcmcth/bcmcth_ldh_drv.h>

#include "bcmcth_ldh_internal.h"

/******************************************************************************
 * Local definitions
 */

#define RING_IS_EMPTY(q)     (q->r_cursor == q->w_cursor)
#define RING_IS_FULL(q)      (((q->w_cursor + 1) % q->number) == q->r_cursor)

/*! Maximum length of thread name. */
#define THREAD_NAME_LEN_MAX                   16

/*! The default thread priority. */
#define THREAD_PRI_DEFAULT                    -1

/* Signature id. */
#define RING_TIMING_PANEL_SIGNATURE_ID        0x5a5a5a5a

/* The default size of ring timing panel. */
#define RING_TIMING_PANEL_SIZE_DEFAULT        128

/* The minimum size of ring timing panel. */
#define RING_TIMING_PANEL_SIZE_MIN            8

/* The maximum size of ring timing panel. */
#define RING_TIMING_PANEL_SIZE_MAX            256

/* The default interval of ring timing panel. */
#define RING_TIMING_PANEL_INTERVAL_DEFAULT    50000

/* The minimum interval of ring timing panel. */
#define RING_TIMING_PANEL_INTERVAL_MIN        10000

/* The maximum interval of ring timing panel. */
#define RING_TIMING_PANEL_INTERVAL_MAX        100000

/* Sanity check that it is a valid object. */
#define BAD_PSI(psi)    ((psi)->signature_id != RING_TIMING_PANEL_SIGNATURE_ID)

/* Initialization check. */
#define LDH_CTRL_INIT_CHECK(unit) \
        if (bcmcth_ldh_ctrl[unit] == NULL) { \
            return SHR_E_MEMORY; \
        }

#define LDH_RTP_INIT_CHECK(unit) \
        if (bcmcth_ldh_ctrl[unit]->rtp == NULL) { \
            return SHR_E_MEMORY; \
        }

/* Protect the critical zone. */
#define LDH_CTRL_LOCK(unit) \
    sal_mutex_take(bcmcth_ldh_ctrl[unit]->lock, SAL_MUTEX_FOREVER)

#define LDH_CTRL_UNLOCK(unit) \
    sal_mutex_give(bcmcth_ldh_ctrl[unit]->lock)

/* Collect counters in time series mode. */
#define COLL_TIME_SERIES_MODE      0

/*  Collect counters in accumulation mode. */
#define COLL_ACCRUE_MODE           1

/* The factor of split. */
#define COLL_SPLIT_FC              2

/* The factor of queue. */
#define COLL_QUEUE_FC              12

/* The factor of bucket. */
#define COLL_BUCKET_FC             8

/* The factor of queue. */
#define COLL_Q_SCALE               4

/* The maximum number of layout pools. */
#define POOL_MAX                   16

/* The instance type. */
typedef void *inst_t;

/* The entry type. */
typedef void *entry_t;

/* The handle type. */
typedef void *hndl_t;

/*
 * The state types of internal.
 */
typedef enum {
    INVALID  = -1,
    INIT     = 0,
    ACTIVE   = 1,
    INACTIVE = 2,
    PENDING  = 3,
    SHUTDOWN = 4
} state_type_e;

/*
 * Define structure of state chart.
 */
typedef struct state_chart_s {
    state_type_e id;
    char *desc;
} state_chart_t;

/*
 * Define structure of queue entry.
 */
typedef struct q_payload_s {
    uint32_t    mon_id;
    uint32_t    q_id;
    uint32_t    b_id;

    uint32_t    pkt;
    uint32_t    byte;
} q_entry_t;

typedef struct ring_q_s {
    /* Header of ring queue. */
    uint32_t    r_cursor;
    uint32_t    w_cursor;
    uint32_t    number;

    uint32_t    null_cnt;
    uint32_t    full_cnt;

    /* Payload of ring queue. */
    q_entry_t  *space;
} ring_q_t;

/*
 * Define a structure of layout index.
 */
typedef struct layout_index_s {
    uint32_t index_min;
    uint32_t index_max;
} layout_index_t;

/*
 * Define capability of device information.
 */
typedef struct cap_info_s {
    /* The maximum number of monitor. */
    uint32_t    mon_max;

    /* The maximum number of counter pool. */
    uint32_t    pool_max;

    /* The maximum entries of a counter pool. */
    uint32_t    ctr_max;
} cap_info_t;

/*
 * Define a data structure for layout information.
 */
typedef struct layout_info_s {
    /* Block index. */
    layout_index_t    block[COLL_SPLIT_FC];

    /* Pool id for flex ctr pool. */
    uint32_t          pool_id;

    /* The index of histogram group start. */
    uint32_t          histo_group_start_idx;

    /* The index of histogram group end. */
    uint32_t          histo_group_end_idx;

    /* The size of histogram group. */
    uint32_t          histo_group_size;

    /* The maximum number of histogram group. */
    uint32_t          histo_groups_max;

    /* Previous histogram group. */
    uint32_t          prev_histo_group;

    /* Previous wrap around. */
    uint32_t          prev_wrap_around;
} layout_info_t;

/* Define callback prototype of collector. */
typedef int (*cb_handler_t) (entry_t *element, void *param);

/*
 * Define a data structure of collector.
 */
typedef struct collector_element_s {
    /* Collector list. */
    shr_dq_t         list;

    /* Associated unit. */
    int              unit;

    /* Collector id. */
    uint32_t         id;

    /* The number of rotation. */
    uint32_t         rotation;

    /* Expire time. */
    uint32_t         expire;

    /* Status of collector. */
    int              status;

    /* Collection mode. */
    int              coll_mode;

    /* Layout info of collector. */
    layout_info_t    layout;

    /* Histo valid. */
    uint8_t          histo_valid[COLL_QUEUE_FC][COLL_BUCKET_FC];

    /* Histo packet cache. */
    uint64_t         histo_pkt[COLL_QUEUE_FC][COLL_BUCKET_FC];

    /* Histo byte cache. */
    uint64_t         histo_byte[COLL_QUEUE_FC][COLL_BUCKET_FC];

    /* Queue resource. */
    ring_q_t         *queue;

    /* Callback for collector. */
    cb_handler_t     handle;
} collector_element_t;

/* The handle of ring timing panel instance. */
typedef int (rtp_instance_create_hdl)(uint32_t size,
                                      uint32_t si,
                                      inst_t *instance);
typedef int (rtp_instance_init_hdl)(inst_t instance);
typedef int (rtp_instance_destroy_hdl)(inst_t instance);
typedef int (rtp_instance_install_hdl)(inst_t instance,
                                       entry_t entry);
typedef int (rtp_instance_cancel_hdl)(inst_t instance,
                                      entry_t entry);
typedef int (rtp_instance_work_hdl)(inst_t instance);
typedef int (rtp_instance_stats_hdl)(inst_t instance);

/* The interface of ring timing panel instance. */
typedef struct rtp_interface_s {
    rtp_instance_create_hdl     *create;
    rtp_instance_init_hdl       *init;
    rtp_instance_destroy_hdl    *destroy;
    rtp_instance_install_hdl    *install;
    rtp_instance_cancel_hdl     *cancel;
    rtp_instance_work_hdl       *work;
    rtp_instance_stats_hdl      *stats;
} rtp_interface_t;

/*
 * Ring timing panel(RTP) data structure.
 */
typedef struct ring_timing_panel_s {
    /* Sanity. */
    uint32_t        signature_id;

    /* Number of tick. */
    uint32_t        ticks_per_panel;

    /* Interval per tick. */
    uint32_t        tick_interval;

    /* Current tick. */
    uint32_t        tick_current_idx;

    /* Factor of tick. */
    int             tick_factor;

    /* Queue of slot. */
    shr_dq_t        *tick_slot;

    /* Interface of object */
    rtp_interface_t ac_func;

    /* stats of panel. */
    uint32_t        stats_ticks;
    uint32_t        stats_active;
    uint32_t        stats_inactive;
    uint32_t        stats_callback;
    uint32_t        stats_cb_failed;
    uint32_t        stats_time;
} ring_timing_panel_t;

/*
 * Latency distribution histogram control data structure.
 */
typedef struct bcmcth_ldh_ctrl_s {
    /* Reference count. */
    SHR_BITDCLNAME(ref_cnt, COLL_NUM_MAX);

    /* Synchronization. */
    sal_mutex_t lock;

    /* Thread ID of control. */
    shr_thread_t lpid;

    /* Thread ID of collector. */
    shr_thread_t cpid;

    /* Periodic timeout. */
    uint32_t interval;

    /* Control status. */
    state_chart_t state;

    /* The capability of monitor. */
    cap_info_t cap;

    /* Ring timing panel. */
    ring_timing_panel_t *rtp;

    /* Collector object. */
    collector_element_t *collector;

} bcmcth_ldh_ctrl_t;

/* LDH control. */
static bcmcth_ldh_ctrl_t *bcmcth_ldh_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/* LDH group. */
static bcmcth_ldh_histo_t *bcmcth_ldh_histo[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private functions
 */
static int panel_create(uint32_t size, uint32_t si, inst_t *rtp_instance);
static int panel_init(inst_t rtp_instance);
static int panel_destroy(inst_t rtp_instance);
static int panel_slot_element_install(inst_t rtp_instance, entry_t element);
static int panel_slot_element_cancel(inst_t rtp_instance, entry_t element);
static int panel_work(inst_t rtp_instance);
static int panel_stats_info(inst_t rtp_instance);

static rtp_interface_t func_hdl = {
    .create  = panel_create,
    .init    = panel_init,
    .destroy = panel_destroy,
    .install = panel_slot_element_install,
    .cancel  = panel_slot_element_cancel,
    .work    = panel_work,
    .stats   = panel_stats_info
};

static char *state_name[] = {
    "INIT",
    "ACTIVE",
    "INACTIVE",
    "PENDING",
    "SHUTDOWN"
};

static int
ring_q_init(ring_q_t **rq, q_entry_t *buf, int num)
{
    ring_q_t *q = NULL;

    if (rq == NULL || buf == NULL) {
        return SHR_E_PARAM;
    }
    q = (ring_q_t *)sal_alloc(sizeof(ring_q_t), "bcmcthLdhRQ");
    if (q == NULL) {
        return SHR_E_MEMORY;
    }

    sal_memset(q, 0, sizeof(ring_q_t));

    q->space  = buf;
    q->number = num;
    *rq = q;

    return SHR_E_NONE;
}

static int
ring_q_free(ring_q_t *rq)
{
    if (rq && rq->space) {
        sal_free(rq->space);
        sal_free(rq);
    }

    return SHR_E_NONE;
}

static int
ring_q_enqueue(ring_q_t *rq, q_entry_t entry)
{
    if (rq == NULL) {
        return SHR_E_PARAM;
    }

    if (RING_IS_FULL(rq)) {
        rq->full_cnt++;
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("LDH RQ is full %d, drop mon[%d] "
                           "q[%d] b[%d] pkt[%d] byte[%d]\n"),
                  rq->full_cnt, entry.mon_id, entry.q_id,
                  entry.b_id, entry.pkt, entry.byte));
        return SHR_E_FULL;
    }

    sal_memcpy(&rq->space[rq->w_cursor], &entry, sizeof(entry));
    rq->w_cursor = (rq->w_cursor + 1) % rq->number;

    return SHR_E_NONE;
}

static int
ring_q_dequeue(ring_q_t *rq, q_entry_t *entry)
{
    if ((rq == NULL) || (entry == NULL)) {
        return SHR_E_PARAM;
    }

    if (RING_IS_EMPTY(rq)) {
        rq->null_cnt++;
        return SHR_E_EMPTY;
    }

    sal_memcpy(entry, &rq->space[rq->r_cursor],
               sizeof(rq->space[rq->r_cursor]));
    rq->r_cursor   = (rq->r_cursor + 1) % rq->number;

    return SHR_E_NONE;
}

static int
list_entry_enqueue(shr_dq_t *head, shr_dq_t *element)
{
    if ((head == NULL) || (element == NULL)) {
        return SHR_E_PARAM;
    }

    SHR_DQ_INSERT_HEAD(head, element);

    return SHR_E_NONE;
}

static int
panel_create(uint32_t size, uint32_t si, inst_t *rtp_instance)
{
    ring_timing_panel_t *panel = NULL;
    uint32_t len;

    panel = (ring_timing_panel_t *)sal_alloc(sizeof(ring_timing_panel_t),
                                             "bcmcthLdhPanel");

    if (panel == NULL) {
        return SHR_E_MEMORY;
    }

    sal_memset(panel, 0, sizeof(ring_timing_panel_t));

    len = size * sizeof(shr_dq_t);

    panel->tick_slot = (shr_dq_t *)sal_alloc(len,
                                             "bcmcthLdhTickSlots");
    if (panel->tick_slot == NULL) {
        sal_free(panel);
        return SHR_E_MEMORY;
    }

    sal_memset(panel->tick_slot, 0, len);

    panel->signature_id = RING_TIMING_PANEL_SIGNATURE_ID;
    panel->ticks_per_panel = size;
    panel->tick_interval = si;
    panel->tick_current_idx = 0;
    panel->tick_factor = 0;
    panel->ac_func = func_hdl;

    *rtp_instance = panel;

    return SHR_E_NONE;
}

static int
panel_init(inst_t rtp_instance)
{
    int i, size;

    ring_timing_panel_t *panel = NULL;
    shr_dq_t *slot = NULL;

    if (rtp_instance == NULL) {
        return SHR_E_PARAM;
    }

    panel = (ring_timing_panel_t *)rtp_instance;

    if (BAD_PSI(panel)) {
        return SHR_E_INTERNAL;
    }

    size = panel->ticks_per_panel;
    slot = &panel->tick_slot[0];

    for (i=0; i < size; i++) {
        slot->flink = slot;
        slot->blink = slot;
        slot++;
    }

    return SHR_E_NONE;
}

static int
panel_collector_create(uint32_t size, inst_t *instance)
{
    collector_element_t *collector = NULL;
    uint32_t len;

    if (instance == NULL) {
        return SHR_E_PARAM;
    }

    len = size * sizeof(collector_element_t);

    collector = (collector_element_t *)sal_alloc(len,
                                                 "bcmcthLdhCollector");

    if (collector == NULL) {
        return SHR_E_MEMORY;
    }

    sal_memset(collector, 0, len);

    *instance = collector;

    return SHR_E_NONE;
}

static int
panel_collector_init(uint32_t size, inst_t instance)
{
    collector_element_t *collector = NULL;
    uint32_t i;

    if (instance == NULL) {
        return SHR_E_PARAM;
    }

    collector = (collector_element_t *)instance;

    for (i=0; i < size; i++) {
        collector->unit = INVALID;
        collector->id = i;
        collector->status = INIT;
        collector->coll_mode = COLL_ACCRUE_MODE;
        collector++;
    }

    return SHR_E_NONE;
}

static int
panel_slot_element_install(inst_t rtp_instance, entry_t element)
{
    int rv = SHR_E_NONE;
    ring_timing_panel_t *panel;
    collector_element_t *entry;
    uint32_t interval;
    uint32_t ticks;
    uint32_t ts;

    shr_dq_t *cur_slot, *new_entry;

    if ((rtp_instance == NULL) || (element == NULL)) {
        return SHR_E_PARAM;
    }

    panel = (ring_timing_panel_t *)rtp_instance;
    entry = (collector_element_t *)element;

    if (BAD_PSI(panel)) {
        return SHR_E_INTERNAL;
    }

    interval = entry->expire;

    /* Number of tick to expire. */
    ticks = (interval / panel->tick_interval);
    ticks = (interval <  panel->tick_interval) ? 1 : ticks;

    /* Number of rotation around the panel to tick slot. */
    entry->rotation = (ticks / panel->ticks_per_panel);

    /* The location of tick slot from current cursor. */
    ts = ((panel->tick_current_idx + ticks) % panel->ticks_per_panel);

    cur_slot = &panel->tick_slot[ts];
    new_entry = &entry->list;

    rv = list_entry_enqueue(cur_slot, new_entry);

    if (rv == SHR_E_NONE) {
        /* collector active. */
        entry->status = ACTIVE;

        panel->stats_active++;
    }

    return rv;
}

static int
panel_slot_element_cancel(inst_t rtp_instance, entry_t element)
{
    ring_timing_panel_t *panel;
    collector_element_t *entry;

    shr_dq_t *next, *prev;

    if ((rtp_instance == NULL) || (element == NULL)) {
        return SHR_E_PARAM;
    }

    panel = (ring_timing_panel_t *)rtp_instance;
    entry = (collector_element_t *)element;
    next  = entry->list.flink;

    if (BAD_PSI(panel)) {
        return SHR_E_INTERNAL;
    }

    if(next) {
        prev = entry->list.blink;
        next->blink = prev;
        prev->flink = next;
        entry->list.flink = NULL;
        entry->list.blink = NULL;

        panel->stats_inactive++;
    }

    return SHR_E_NONE;
}

static int
panel_destroy(inst_t rtp_instance)
{
    ring_timing_panel_t *panel = NULL;
    shr_dq_t *slot;
    collector_element_t *entry = NULL;
    uint32_t i;

    if (rtp_instance == NULL) {
        return SHR_E_PARAM;
    }

    panel = (ring_timing_panel_t *)rtp_instance;

    if (BAD_PSI(panel)) {
        return SHR_E_INTERNAL;
    }

    for (i=0; i < panel->ticks_per_panel; i++) {
        slot = &panel->tick_slot[i];
        entry = (collector_element_t *)slot->flink;

        while ((shr_dq_t *)entry != slot) {
            panel_slot_element_cancel(rtp_instance, entry);
            entry = (collector_element_t *)slot->flink;
        }
    }

    sal_free(panel->tick_slot);
    sal_free(panel);

    return SHR_E_NONE;
}

static int
panel_work(inst_t rtp_instance)
{
    int rv = SHR_E_NONE;
    ring_timing_panel_t *panel;
    shr_dq_t *element, *next, *prev;
    collector_element_t *coll;
    void *collector;
    shr_dq_t *cur_slot, *new_entry;
    uint32_t index;
    uint32_t interval, ticks, ts;

    if (rtp_instance == NULL) {
        return SHR_E_PARAM;
    }

    panel = (ring_timing_panel_t *)rtp_instance;

    index = panel->tick_current_idx;
    index = (index + 1) % panel->ticks_per_panel;
    panel->tick_current_idx = index;

    element = &panel->tick_slot[index];
    coll = (collector_element_t *)element->flink;
    collector = element->flink;
    panel->stats_ticks++;

    while ((shr_dq_t *)coll != element) {
        next = (shr_dq_t *)coll->list.flink;
        prev = (shr_dq_t *)coll->list.blink;

        if(coll->rotation != 0) {
            coll->rotation--;
        } else {
            prev->flink = next;
            next->blink = prev;
            coll->list.flink = NULL;
            coll->list.blink = NULL;

            /*
             * callback for collector
             */
            if (coll->handle) {
                rv = (coll->handle)(collector, NULL);
                if (rv != SHR_E_NONE) {
                    panel->stats_cb_failed++;
                }
                panel->stats_callback++;
            }

            interval = coll->expire;
            ticks = (interval / panel->tick_interval);
            coll->rotation = (ticks / panel->ticks_per_panel);
            ts = ((index + ticks) % panel->ticks_per_panel);
            cur_slot = &panel->tick_slot[ts];
            new_entry = &coll->list;

            list_entry_enqueue(cur_slot, new_entry);
        }

        coll = (collector_element_t *)next;
    }

    return rv;
}

static int
panel_stats_info(inst_t rtp_instance)
{
    ring_timing_panel_t *panel;

    if (rtp_instance == NULL) {
        return SHR_E_PARAM;
    }

    panel = (ring_timing_panel_t *)rtp_instance;

    LOG_CLI((BSL_META("panel tick stats     = %u\n"),
             panel->stats_ticks));
    LOG_CLI((BSL_META("panel active stats   = %u\n"),
             panel->stats_active));
    LOG_CLI((BSL_META("panel inactive stats = %u\n"),
             panel->stats_inactive));
    LOG_CLI((BSL_META("panel cb stats       = %u\n"),
             panel->stats_callback));
    LOG_CLI((BSL_META("panel cb failed      = %u\n"),
             panel->stats_cb_failed));
    LOG_CLI((BSL_META("panel time consume   = %u\n"),
             panel->stats_time));

    return SHR_E_NONE;
}

static bool
panel_collector_active(collector_element_t *element)
{
    if (element == NULL) {
        return FALSE;
    }

    if ((element->list.flink != NULL) ||
        (element->status == ACTIVE)) {
        return TRUE;
    }

    return FALSE;
}

static int
ldh_ring_q_init(int unit, int mon_id, int size)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    ring_q_t *queue = NULL;
    q_entry_t *buf = NULL;

    SHR_FUNC_ENTER(unit);

    LDH_CTRL_INIT_CHECK(unit);

    buf = sal_alloc(sizeof(q_entry_t) * size, "bcmcthLdhRQbuf");

    if (buf == NULL) {
        return SHR_E_MEMORY;
    }
    sal_memset(buf, 0, sizeof(q_entry_t) * size);

    ring_q_init(&queue, buf, size);

    if (queue == NULL) {
        return SHR_E_MEMORY;
    }

    ldhc->collector[mon_id].queue = queue;

    SHR_FUNC_EXIT();
}

static int
ldh_panel_collector_install(int unit,
                            int id,
                            int delay,
                            cb_handler_t handle)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll = &ldhc->collector[id];
    ring_timing_panel_t *rtp = ldhc->rtp;
    rtp_interface_t *rtp_drv = &rtp->ac_func;

    SHR_FUNC_ENTER(unit);

    LDH_CTRL_LOCK(unit);

    coll->unit = unit;
    coll->expire = delay;
    coll->handle = handle;
    coll->list.flink = NULL;
    coll->list.blink = NULL;

    SHR_IF_ERR_EXIT
        (rtp_drv->install(rtp, coll));

    LDH_CTRL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

static int
ldh_panel_collector_cancel(int unit, int id)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll = &ldhc->collector[id];
    ring_timing_panel_t *rtp = ldhc->rtp;
    rtp_interface_t *rtp_drv = &rtp->ac_func;

    SHR_FUNC_ENTER(unit);

    LDH_CTRL_LOCK(unit);

    SHR_IF_ERR_EXIT
        (rtp_drv->cancel(rtp, coll));

    LDH_CTRL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

static int
ldh_panel_init(int unit)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    ring_timing_panel_t *rtp = NULL;
    rtp_interface_t *rtp_drv = NULL;
    ring_timing_panel_t init_panel;
    void *panel;
    void *element = NULL;

    SHR_FUNC_ENTER(unit);

    LDH_CTRL_INIT_CHECK(unit);

    sal_memset(&init_panel, 0, sizeof(ring_timing_panel_t));
    init_panel.ac_func = func_hdl;
    init_panel.ac_func.create(RING_TIMING_PANEL_SIZE_DEFAULT,
                              RING_TIMING_PANEL_INTERVAL_DEFAULT,
                              &panel);
    SHR_NULL_CHECK(panel, SHR_E_INIT);
    ldhc->rtp = (ring_timing_panel_t *)panel;
    rtp = ldhc->rtp;
    rtp_drv = &rtp->ac_func;

    SHR_IF_ERR_EXIT
        (rtp_drv->init(rtp));

    SHR_IF_ERR_EXIT
        (panel_collector_create(COLL_NUM_MAX, &element));
    ldhc->collector = (collector_element_t *)element;
    SHR_IF_ERR_EXIT
        (panel_collector_init(COLL_NUM_MAX, ldhc->collector));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "FAILED to init LDH panel.\n")));
        if (rtp && rtp_drv) {
            rtp_drv->destroy(rtp);
            ldhc->rtp = NULL;
        }
        if (ldhc->collector) {
            sal_free(ldhc->collector);
            ldhc->collector = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ldh_control_cleanup(int unit)
{
    bcmcth_ldh_ctrl_t *ldhc = NULL;
    collector_element_t *coll = NULL;
    ring_timing_panel_t *rtp = NULL;
    rtp_interface_t *rtp_drv = NULL;
    bcmcth_ldh_histo_t *histo = NULL;
    int i, max_num;

    SHR_FUNC_ENTER(unit);

    ldhc = bcmcth_ldh_ctrl[unit];

    if (ldhc) {

        max_num = ldhc->cap.mon_max;
        LDH_CTRL_LOCK(unit);
        ldhc->state.id = SHUTDOWN;
        LDH_CTRL_UNLOCK(unit);

        if (ldhc->lpid) {
            if (SHR_FAILURE(shr_thread_stop(ldhc->lpid,
                                            RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "LDH ctrl thread doesn't exit.\n")));
            }
        }

        if (ldhc->cpid) {
            if (SHR_FAILURE(shr_thread_stop(ldhc->cpid,
                                            RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "LDH coll thread doesn't exit.\n")));
            }
        }

        for (i=0; i < max_num; i++) {
            coll = &ldhc->collector[i];
            if (coll) {
                if (SHR_FAILURE(ring_q_free(coll->queue))) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "LDH coll memory free failed.\n")));
                }
            }
        }

        rtp = ldhc->rtp;
        if (rtp) {
            rtp_drv = &rtp->ac_func;
            if (rtp_drv) {
                if (SHR_FAILURE(rtp_drv->destroy(rtp))) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "LDH rtp destroy failed.\n")));
                }
            }
        }

        if (ldhc->collector) {
            sal_free(ldhc->collector);
            ldhc->collector = NULL;
        }

        if (ldhc->lock) {
            sal_mutex_destroy(ldhc->lock);
        }

        histo = bcmcth_ldh_histo[unit];
        if (histo) {
            sal_free(histo);
        }

        sal_free(ldhc);
        bcmcth_ldh_histo[unit] = NULL;
        bcmcth_ldh_ctrl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

static int
ldh_collector_layout_update(int unit, int mon_id, int type)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll = &ldhc->collector[mon_id];
    layout_info_t *layout = &coll->layout;
    uint32_t group_start_index = 0;
    uint32_t group_end_index = 0;
    uint32_t group_size = 0;
    uint32_t groups_max = 0;
    int      avg_group_num = 0;
    int      i, size;

    SHR_FUNC_ENTER(unit);

    if (type) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ldh_control_imm_get(unit,
                                        mon_id,
                                        LDH_MON_OFFSET_IDX,
                                        &group_start_index));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ldh_control_imm_get(unit,
                                        mon_id,
                                        LDH_MON_CTR_INCR,
                                        &group_size));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ldh_control_imm_get(unit,
                                        mon_id,
                                        LDH_MON_GROUP_MAX,
                                        &groups_max));

        group_end_index = group_size * groups_max + group_start_index - 1;
        LDH_CTRL_LOCK(unit);
        layout->histo_group_start_idx = group_start_index;
        layout->histo_group_end_idx = group_end_index;
        layout->histo_group_size = group_size;
        layout->histo_groups_max = groups_max;
        LDH_CTRL_UNLOCK(unit);
    } else {
        LDH_CTRL_LOCK(unit);
        group_start_index = layout->histo_group_start_idx;
        group_size = layout->histo_group_size;
        groups_max = layout->histo_groups_max;
        LDH_CTRL_UNLOCK(unit);
    }

    avg_group_num = groups_max / COLL_SPLIT_FC;

    if (!group_start_index || !group_size || !groups_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    LDH_CTRL_LOCK(unit);
    for (i = 0; i < COLL_SPLIT_FC; i ++) {
        layout->block[i].index_min = \
            group_start_index + i * avg_group_num * group_size;

        layout->block[i].index_max = \
            layout->block[i].index_min + \
            avg_group_num * group_size - 1;
    }
    LDH_CTRL_UNLOCK(unit);

    if (coll->coll_mode == COLL_TIME_SERIES_MODE) {
        size = COLL_BUCKET_FC * group_size * groups_max * COLL_Q_SCALE;
        if (size == 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (ldh_ring_q_init(unit, mon_id, size));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ldh_collector_layout_cleanup(int unit, int mon_id)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll = &ldhc->collector[mon_id];

    sal_memset(coll->histo_pkt,
               0,
               COLL_QUEUE_FC * COLL_BUCKET_FC * sizeof(uint64_t));
    sal_memset(coll->histo_byte,
               0,
               COLL_QUEUE_FC * COLL_BUCKET_FC * sizeof(uint64_t));

    return SHR_E_NONE;
}

static int
ldh_collector_handle_cb(entry_t *ele, void *param)
{
    int rv = SHR_E_NONE;
    bcmcth_ldh_ctrl_t *ldhc;
    ring_timing_panel_t *rtp;
    collector_element_t *coll;
    layout_info_t *layout;
    collector_element_t *inst;
    int unit, pool_id, mon_id;
    int cur_histo_group = 0;
    int cur_wrap_around = 0;
    int avg_group_num = 0;

    uint32_t pkt=0, byte=0, pkt_sum=0, byte_sum=0;
    uint32_t block_id, index, index_min, index_max;
    uint32_t l_idx, l_idx_min, l_idx_max;
    uint32_t q_num, q_idx, q_min, q_max;
    uint32_t b_idx;

    ring_q_t *queue = NULL;
    q_entry_t entry;

    inst = (collector_element_t *)ele;
    unit = inst->unit;
    mon_id = inst->id;
    ldhc = bcmcth_ldh_ctrl[unit];
    rtp = ldhc->rtp;
    coll = &ldhc->collector[mon_id];
    layout = &coll->layout;
    pool_id = layout->pool_id;

    rv = bcmcth_ldh_cur_histo_group_get(unit, mon_id, &cur_histo_group);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv = bcmcth_ldh_ctr_wrap_around_get(unit, mon_id, &cur_wrap_around);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    if (cur_histo_group == 0 && cur_wrap_around == 0) {
        return SHR_E_INIT;
    }

    avg_group_num = layout->histo_groups_max / COLL_SPLIT_FC;

    if (cur_histo_group >= avg_group_num) {
        block_id = 0;
    } else {
        block_id = 1;
    }

    index_min = layout->block[block_id].index_min;
    index_max = layout->block[block_id].index_max;
    l_idx_min = index_min - layout->histo_group_start_idx;
    l_idx_max = index_max - layout->histo_group_start_idx;
    q_num = layout->histo_group_size / COLL_BUCKET_FC;
    q_min = l_idx_min % (layout->histo_group_size);
    q_min = (q_min / COLL_BUCKET_FC) % q_num;
    q_max = l_idx_max % (layout->histo_group_size);
    q_max = (q_max / COLL_BUCKET_FC) % q_num;

    for (index = index_min; index < index_max; index ++) {
        bcmcth_ldh_ctr_data_get(unit, pool_id, index, &pkt, &byte);
        l_idx = index - layout->histo_group_start_idx;
        l_idx = l_idx % (layout->histo_group_size);
        q_idx = (l_idx / COLL_BUCKET_FC) % q_num;
        b_idx = index % COLL_BUCKET_FC;

        if (coll->coll_mode == COLL_ACCRUE_MODE) {
            if (coll->histo_valid[q_idx][b_idx]) {
                coll->histo_pkt[q_idx][b_idx] += pkt;
                coll->histo_byte[q_idx][b_idx] += byte;
            }
        }

        if (coll->coll_mode == COLL_TIME_SERIES_MODE) {

            queue = coll->queue;

            if (queue == NULL) {
                return SHR_E_INIT;
            }

            entry.mon_id = mon_id;
            entry.q_id = q_idx;
            entry.b_id = b_idx;
            entry.pkt = pkt;
            entry.byte = byte;

            if (ring_q_enqueue(queue, entry) == SHR_E_FULL) {
                break;
            }
        }
    }

    if (coll->coll_mode == COLL_ACCRUE_MODE) {
        for (q_idx = q_min; q_idx < q_num; q_idx ++)
            for (b_idx = 0; b_idx < COLL_BUCKET_FC; b_idx ++) {
                pkt_sum = coll->histo_pkt[q_idx][b_idx];
                byte_sum = coll->histo_byte[q_idx][b_idx];
                if (coll->histo_valid[q_idx][b_idx]) {
                    bcmcth_ldh_stats_imm_set(unit,
                                             mon_id,
                                             q_idx,
                                             b_idx,
                                             pkt_sum,
                                             byte_sum,
                                             1);
                }

                LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META("mon[%d] q[%d] b[%d] "
                                  "pkt[%d] byte[%d] last[%d]\n"),
                         mon_id, q_idx, b_idx,
                         pkt_sum, byte_sum,
                         rtp->stats_time));
        }
    }

    layout->prev_histo_group = cur_histo_group;
    layout->prev_wrap_around = cur_wrap_around;

    LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META("unit.%d coll.%d pool[%d] "
                      "q_num[%d] q_min[%d] q_max[%d] "
                      "pkt[%d] byte[%d] last[%d]\n"),
                       unit, inst->id, pool_id,
                       q_num, q_min, q_max,
                       pkt_sum, byte_sum,
                       rtp->stats_time));
    return rv;
}

static int
ldh_collector_histo_cb(int unit, int mon_id)
{
    bcmcth_ldh_ctrl_t *ldhc = NULL;
    bcmcth_ldh_histo_t *ldh_histo = NULL;
    ring_q_t *queue = NULL;
    q_entry_t entry;
    bcmcth_ldh_histo_group_t histo;

    int i, entry_num;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bcmcth_ldh_ctrl[unit], SHR_E_INIT);
    SHR_NULL_CHECK(bcmcth_ldh_histo[unit], SHR_E_INIT);
    SHR_NULL_CHECK(bcmcth_ldh_ctrl[unit]->collector[mon_id].queue, SHR_E_INIT);

    ldhc = bcmcth_ldh_ctrl[unit];
    ldh_histo = bcmcth_ldh_histo[unit];
    queue = ldhc->collector[mon_id].queue;
    entry_num = ldhc->collector[mon_id].layout.histo_group_size;

    for (i=0; i < entry_num; i++) {
        rv = ring_q_dequeue(queue, &entry);
        if (rv == SHR_E_EMPTY) {
            continue;
        }

        if (ldh_histo->ldh_histo_f[mon_id] != NULL) {
            histo.entry[i].queue_id = entry.q_id;
            histo.entry[i].bucket_id = entry.b_id;
            histo.entry[i].pkt = entry.pkt;
            histo.entry[i].byte = entry.byte;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("mon.%d dequeue q[%d] b[%d] pkt[%d] byte[%d] "
                          "r_cur[%d] w_cur[%d]\n"),
                 entry.mon_id, entry.q_id,
                 entry.b_id, entry.pkt, entry.byte,
                 queue->r_cursor, queue->w_cursor));
    }

    if (ldh_histo->ldh_histo_f[mon_id] != NULL) {
        histo.len = entry_num;
        ldh_histo->ldh_histo_f[mon_id](unit, mon_id, &histo);
    }

exit:
     SHR_FUNC_EXIT();
}

static void
ldh_control_thread(shr_thread_t tc, void *arg)
{
    int rv = SHR_E_NONE;
    int unit = (unsigned long)(arg);
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    ring_timing_panel_t *rtp = ldhc->rtp;
    rtp_interface_t *rtp_drv = &rtp->ac_func;
    sal_usecs_t interval;
    sal_usecs_t time_start;
    sal_usecs_t time_end;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LDH Control starting\n")));

    while (!ldhc->state.id) {
        sal_thread_yield();
    }

    time_start = 0;
    time_end = 0;
    interval = ldhc->interval;

    while (interval) {

        shr_thread_sleep(tc, interval);

        if (shr_thread_stopping(tc)) {
            break;
        }

        time_start = sal_time_usecs();

        LDH_CTRL_LOCK(unit);
        rv = rtp_drv->work(rtp);
        LDH_CTRL_UNLOCK(unit);

        time_end = sal_time_usecs() - time_start;
        rtp->stats_time = time_end;

        if (rv != SHR_E_NONE) {
            break;
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LDH Control exiting\n")));

    if (ldhc->lpid) {
        ldhc->lpid = NULL;
    }
    ldhc->state.id = SHUTDOWN;
    ldhc->state.desc = state_name[SHUTDOWN];
}

static void
ldh_collect_thread(shr_thread_t tc, void *arg)
{
    int unit = (unsigned long)(arg);
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];

    sal_usecs_t interval;
    int i, max_num;

    while (!ldhc->state.id) {
        sal_thread_yield();
    }

    max_num = bcmcth_ldh_ctrl[unit]->cap.mon_max;
    interval = ldhc->interval;

    while (ldhc->state.id == ACTIVE) {
        shr_thread_sleep(tc, interval);

        if (shr_thread_stopping(tc)) {
            break;
        }

        for (i = 0; i < max_num; i++) {
            if (ldhc->collector[i].coll_mode == COLL_TIME_SERIES_MODE) {
                ldh_collector_histo_cb(unit, i);
            }
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "LDH Coll exiting\n")));

    if (ldhc->cpid) {
        ldhc->cpid = NULL;
    }
}

static int
ldh_control_thread_start(int unit)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    int ref_cnt = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ldhc, SHR_E_INIT);

    SHR_BITCOUNT_RANGE(ldhc->ref_cnt, ref_cnt, 0, COLL_NUM_MAX);
    if (ref_cnt) {
        SHR_EXIT();
    }

    ldhc->lpid = shr_thread_start("bcmcthLDH",
                                  THREAD_PRI_DEFAULT,
                                  ldh_control_thread,
                                  ((void *)(unsigned long)(unit)));

    if (ldhc->lpid == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    ldhc->cpid = shr_thread_start("bcmcthColl",
                                  THREAD_PRI_DEFAULT,
                                  ldh_collect_thread,
                                  ((void *)(unsigned long)(unit)));

    if (ldhc->cpid == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_FAIL);
    }

    ldhc->state.id = ACTIVE;
    ldhc->state.desc = state_name[ACTIVE];

exit:
    if (SHR_FUNC_ERR()) {
        if (ldhc->lpid) {
            if (SHR_FAILURE(shr_thread_stop(ldhc->lpid,
                                            RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "LDH ctrl thread doesn't exit.\n")));
            }
            ldhc->lpid = NULL;
            ldhc->state.id = PENDING;
            ldhc->state.desc = state_name[PENDING];
        }
    }
    SHR_FUNC_EXIT();
}

static int
ldh_control_thread_stop(int unit)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    int ref_cnt = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ldhc, SHR_E_INIT);

    SHR_BITCOUNT_RANGE(ldhc->ref_cnt, ref_cnt, 0, COLL_NUM_MAX);
    if (ref_cnt) {
        SHR_EXIT();
    }

    if (ldhc->lpid) {
        if (SHR_FAILURE(shr_thread_stop(ldhc->lpid,
                                        RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "LDH ctrl thread doesn't exit.\n")));
        }
        ldhc->lpid = NULL;
    }

    if (ldhc->cpid) {
        if (SHR_FAILURE(shr_thread_stop(ldhc->cpid,
                                        RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "LDH coll thread doesn't exit.\n")));
        }
        ldhc->cpid = NULL;
    }

    ldhc->state.id = SHUTDOWN;
    ldhc->state.desc = state_name[SHUTDOWN];

exit:
    SHR_FUNC_EXIT();
}

static int
ldh_mon_ctrl_validation(int unit, uint32_t *entry, int num)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll;
    int key;
    int i, val;
    int en = TRUE;
    int status;

    SHR_FUNC_ENTER(unit);

    key = entry[0];
    coll = &ldhc->collector[key];

    for (i = 1; i < num; i ++) {
        val = entry[i];
        if (val < 0) {
            continue;
        }

        if (i == LDH_MON_ENABLE) {
            en = val;
            break;
        }
    }

    status = panel_collector_active(coll);

    if (status && en && (ldhc->state.id == ACTIVE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Allow to go through with disable. */
    if (!en) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ldh_mon_enable_set(unit, key, FALSE));
        SHR_IF_ERR_VERBOSE_EXIT
            (ldh_collector_layout_cleanup(unit, key));
        SHR_IF_ERR_VERBOSE_EXIT
            (ldh_panel_collector_cancel(unit, key));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ldh_collector_info(int unit, int id)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ldhc, SHR_E_INIT);

    LOG_CLI((BSL_META("ldh collector %d install\n"), id));
    LOG_CLI((BSL_META("\tstart= %d, end= %d, "
                      "size= %d, max= %d, "
                      "expire= %d, rotation= %d\n"),
             ldhc->collector[id].layout.histo_group_start_idx,
             ldhc->collector[id].layout.histo_group_end_idx,
             ldhc->collector[id].layout.histo_group_size,
             ldhc->collector[id].layout.histo_groups_max,
             ldhc->collector[id].expire,
             ldhc->collector[id].rotation));
exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

int
bcmcth_ldh_control_init(int unit)
{
    bcmcth_ldh_ctrl_t *ldhc = NULL;
    bcmcth_ldh_histo_t *ldh_histo = NULL;

    SHR_FUNC_ENTER(unit);

    if (bcmcth_ldh_ctrl[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    ldh_histo = sal_alloc(sizeof(bcmcth_ldh_histo_t), "bcmcthLdhHisto");
    if (ldh_histo == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(ldh_histo, 0, sizeof(bcmcth_ldh_histo_t));

    ldhc = sal_alloc(sizeof(bcmcth_ldh_ctrl_t), "bcmcthLdhCtrl");
    if (ldhc == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(ldhc, 0, sizeof(bcmcth_ldh_ctrl_t));

    ldhc->lock = sal_mutex_create("bcmcthLdhCtrlLock");
    if (ldhc->lock == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    ldhc->state.id = INIT;
    ldhc->state.desc = state_name[INIT];

    ldhc->interval = RING_TIMING_PANEL_INTERVAL_DEFAULT;

    bcmcth_ldh_ctrl[unit] = ldhc;
    bcmcth_ldh_histo[unit] = ldh_histo;

    SHR_IF_ERR_EXIT(ldh_panel_init(unit));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "FAILED to init LDH ctrl.\n")));
        if (ldh_histo) {
            sal_free(ldh_histo);
            ldh_histo = NULL;
        }
        if (ldhc) {
            if (ldhc->lock) {
                sal_mutex_destroy(ldhc->lock);
            }
            sal_free(ldhc);
            ldhc = NULL;
        }
        bcmcth_ldh_ctrl[unit] = NULL;
        bcmcth_ldh_histo[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_control_cleanup(int unit)
{
    return ldh_control_cleanup(unit);
}

int
bcmcth_ldh_monitor_max_init(int unit, int max_num)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ldhc, SHR_E_INIT);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "The maximum number of monitor is %d\n"),
              max_num));

    ldhc->cap.mon_max = max_num;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_ctr_pool_max_init(int unit, int max_num)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ldhc, SHR_E_INIT);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "The maximum counter pool is %d\n"),
              max_num));

    ldhc->cap.pool_max = max_num;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_ctr_entry_max_init(int unit, int max_num)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ldhc, SHR_E_INIT);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "The maximum number of counter entry is %d\n"),
              max_num));

    ldhc->cap.ctr_max = max_num;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_stats_update(int unit,
                        int op,
                        ldh_stats_key_t key,
                        uint64_t data)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll;
    uint32_t mon_id;
    uint32_t q_id;
    uint32_t bkt_id;

    SHR_FUNC_ENTER(unit);

    mon_id = key.mon_id;
    q_id = key.q_id;
    bkt_id = key.bkt_id;

    coll = &ldhc->collector[mon_id];

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("op:[%d] key:[%d][%d][%d] data:%"PRIu64"\n"),
                 op, mon_id, q_id, bkt_id, data));

    switch (op) {
        case LDH_STATS:
            LDH_CTRL_LOCK(unit);
            coll->histo_valid[q_id][bkt_id] = 1;
            LDH_CTRL_UNLOCK(unit);
            break;
        case LDH_STATS_BYTES:
            LDH_CTRL_LOCK(unit);
            coll->histo_byte[q_id][bkt_id] = data;
            LDH_CTRL_UNLOCK(unit);
            break;
        case LDH_STATS_PACKETS:
            LDH_CTRL_LOCK(unit);
            coll->histo_pkt[q_id][bkt_id] = data;
            LDH_CTRL_UNLOCK(unit);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_ctrl_op(int unit,
                       int op,
                       uint32_t key,
                       uint32_t data)
{
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll;
    layout_info_t *layout;

    int expire_time = 0;
    int split_fc = 0;
    int status;

    SHR_FUNC_ENTER(unit);

    LDH_CTRL_INIT_CHECK(unit);
    LDH_RTP_INIT_CHECK(unit);

    coll = &ldhc->collector[key];
    layout = &coll->layout;
    status = panel_collector_active(coll);

    switch (op) {
        case LDH_MON_ENABLE:
            /* update */
            if (data) {
                SHR_IF_ERR_EXIT(ldh_control_thread_start(unit));
            }
            if (data == TRUE && status == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ldh_collector_layout_update(unit, key, 0));

                expire_time = coll->expire;

                SHR_IF_ERR_VERBOSE_EXIT
                    (ldh_panel_collector_install(unit,
                                                 key,
                                                 expire_time,
                                                 ldh_collector_handle_cb));

                LDH_CTRL_LOCK(unit);
                SHR_BITSET(ldhc->ref_cnt, key);
                LDH_CTRL_UNLOCK(unit);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_enable_set(unit, key, data));
            if (data == FALSE && status == TRUE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ldh_collector_layout_cleanup(unit, key));
                SHR_IF_ERR_VERBOSE_EXIT
                    (ldh_panel_collector_cancel(unit, key));

                if (SHR_BITGET(ldhc->ref_cnt, key)) {
                    LDH_CTRL_LOCK(unit);
                    SHR_BITCLR(ldhc->ref_cnt, key);
                    LDH_CTRL_UNLOCK(unit);
                }
            }
            if (!data) {
                SHR_IF_ERR_EXIT(ldh_control_thread_stop(unit));
            }
            break;
        case LDH_MON_COUNT_MODE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_count_mode_set(unit, key, data));
            break;
        case LDH_MON_COLL_MODE:
            LDH_CTRL_LOCK(unit);
            coll->coll_mode = data;
            LDH_CTRL_UNLOCK(unit);
            break;
        case LDH_MON_TIME_STEP:
            expire_time = layout->histo_groups_max * data;
            split_fc = COLL_SPLIT_FC;

            LDH_CTRL_LOCK(unit);
            coll->expire = expire_time / split_fc;
            LDH_CTRL_UNLOCK(unit);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_time_step_set(unit, key, data));
            break;
        case LDH_MON_POOL_ID:
            LDH_CTRL_LOCK(unit);
            layout->pool_id = data;
            LDH_CTRL_UNLOCK(unit);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_pool_id_set(unit, key, data));
            break;
        case LDH_MON_OFFSET_MODE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_pool_mode_set(unit, key, data));
            break;
        case LDH_MON_OFFSET_IDX:
            LDH_CTRL_LOCK(unit);
            layout->histo_group_start_idx = data;
            LDH_CTRL_UNLOCK(unit);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_base_offset_set(unit, key, data));
            break;
        case LDH_MON_CTR_INCR:
            LDH_CTRL_LOCK(unit);
            layout->histo_group_size = data * COLL_BUCKET_FC;
            LDH_CTRL_UNLOCK(unit);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_counter_incr_set(unit, key, data));
            break;
        case LDH_MON_GROUP_MAX:
            LDH_CTRL_LOCK(unit);
            layout->histo_groups_max = data;
            layout->histo_group_end_idx = \
                data * layout->histo_group_size + \
                layout->histo_group_start_idx - 1;
            LDH_CTRL_UNLOCK(unit);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ldh_mon_group_max_set(unit, key, data));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (SHR_FUNC_ERR() && (op == LDH_MON_ENABLE) && data) {
        if (ldhc->lpid) {
            if (SHR_FAILURE(shr_thread_stop(ldhc->lpid,
                                            RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "LDH ctrl thread doesn't exit.\n")));
            }
        }

        if (ldhc->cpid) {
            if (SHR_FAILURE(shr_thread_stop(ldhc->cpid,
                                            RING_TIMING_PANEL_INTERVAL_DEFAULT))) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "LDH coll thread doesn't exit.\n")));
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_mon_ctrl_update(int unit, uint32_t *entry, int num)
{
    int key;
    int i, val;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ldh_mon_ctrl_validation(unit, entry, num));

    key = entry[0];

    for (i = 1; i < num; i++) {
        val = entry[i];
        if (val < 0) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmcth_ldh_mon_ctrl_op(unit, i, key, val));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_histo_register(int unit, int mon_id, bcmcth_ldh_histo_group_cb fn)
{
    bcmcth_ldh_histo_t *ldh_histo = bcmcth_ldh_histo[unit];

    if (ldh_histo == NULL) {
        return SHR_E_MEMORY;
    }

    ldh_histo->ldh_histo_f[mon_id] = fn;

    return SHR_E_NONE;
}

int
bcmcth_ldh_histo_unregister(int unit, int mon_id, bcmcth_ldh_histo_group_cb fn)
{
    bcmcth_ldh_histo_t *ldh_histo = bcmcth_ldh_histo[unit];

    if (ldh_histo->ldh_histo_f[mon_id] == fn) {
        ldh_histo->ldh_histo_f[mon_id] = NULL;
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }

    return SHR_E_NONE;
}

int
bcmcth_ldh_info_dump(int unit)
{
    int ret, id;
    bcmcth_ldh_ctrl_t *ldhc = bcmcth_ldh_ctrl[unit];
    collector_element_t *coll;
    ring_timing_panel_t *rtp;

    if ((ldhc == NULL) ||
        (ldhc->rtp == NULL) ||
        (ldhc->collector == NULL)) {
        return SHR_E_INIT;
    }
    rtp = ldhc->rtp;

    panel_stats_info(rtp);

    for (id = 0; id < COLL_NUM_MAX; id++) {
        coll = &ldhc->collector[id];
        ret = panel_collector_active(coll);
        if (ret) {
            ldh_collector_info(unit, id);
        }
    }
    return SHR_E_NONE;
}
