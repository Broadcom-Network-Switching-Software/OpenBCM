/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx.h
 * Purpose:     Internal structures and definitions for RX module
 */
#ifndef   _BCM_COMMON_RX_H_
#define   _BCM_COMMON_RX_H_

#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/time.h>
#include <soc/dma.h>
#include <bcm/error.h>
#include <bcm/rx.h>

/*
 * Typedef:
 *      rx_callout_t
 * Purpose:
 *      Describes a registered callout for RX.
 * Notes:
 *      For each interface, a list of callouts is maintained in sorted
 *      order from highest priority to lowest. The callouts are called
 *      in order until one returns "bcm_rx_handled" or
 *      "bcm_rx_handled_owned". If all handlers called and none process
 *      the packet, the default "discard" handler is called.
 */

typedef struct rx_callout_s {
    volatile struct rx_callout_s *rco_next;      /* Next in sorted list */
    const char          *rco_name;      /* Name for handler */
    void                *rco_cookie;    /* Cookie for call out */
    bcm_rx_cb_f         rco_function;   /* Callout function */
    uint32              rco_flags;      /* RCO_F_  flags; see bcm/rx.h */
    uint8               rco_priority;   /* Callout priority 0 == lowest */
    uint32              rco_pkts_handled; /* Counter */
    uint32              rco_pkts_owned; /* Counter */
    SHR_BITDCL          rco_cos[_SHR_BITDCLSIZE(BCM_RX_COS)]; /* cos bitmap */
} rx_callout_t;

#define SETUP_RCO(rco, name, fun, pri, cookie, next, flags)   \
    do {                                                 \
        (rco)->rco_name = (name);                        \
        (rco)->rco_function = (fun);                     \
        (rco)->rco_priority = (pri);                     \
        (rco)->rco_cookie = (cookie);                    \
        (rco)->rco_next = (next);                        \
        (rco)->rco_flags = (flags);                      \
        (rco)->rco_pkts_owned = 0;                       \
        (rco)->rco_pkts_handled = 0;                     \
        sal_memset((void *)((rco)->rco_cos), 0,          \
                   SHR_BITALLOCSIZE(BCM_RX_COS));        \
    } while (0)

#define SETUP_RCO_COS_SET(rco, cos)                      \
    do {                                                 \
        SHR_BITSET((rco)->rco_cos, (cos));               \
    } while (0)

#define SETUP_RCO_COS_CLR(rco, cos)                      \
    do {                                                 \
        SHR_BITCLR((rco)->rco_cos, (cos));               \
    } while (0)

/* Some default values */ 
#define RX_PKT_SIZE_DFLT       (16 * 1024)    /* 12 K packets, plus spare */
#define RX_PPC_DFLT            8             /* 8 pkts/chain */
#define RX_PPS_DFLT            1000          /* 1000 pkts/sec */
#define RX_CHAINS_DFLT         8             /* 8 chains */
#define RX_THREAD_PRI_DFLT     200

#define RX_CMICX_PPC_DFLT      8             /* 8 pkts/chain in cmicx devices */
    
/* Could probably be pretty large now */
#define RX_PPC_MAX             SOC_DV_PKTS_MAX

/* Default unit to use for alloc/free */
#define BCM_RX_DFLT_UNIT 0


/* Queue type for packets, one per channel; possibly per COS in the future */
typedef struct rx_queue_s {
    bcm_pkt_t   *head; 
    bcm_pkt_t   *tail;
    int          pps;          /* Packets/sec for queue; 0 -> disabled */
    int          burst;        /* Burst for queue */
    int          tokens;       /* Token bucket */
    sal_usecs_t  last_fill;    /* Last time bucket updated */
    int          count;        /* Number of pkts in queue */
    int          max_len;      /* Max number of pkts permitted in queue.
                                  Disabled if <= 0. NOT RECOMMENDED. */
#define RX_Q_MAX_DFLT 200
    int          tot_pkts;     /* Total pkts through the queue (debug) */
    int          rate_disc;    /* Pkts discarded due to rate limiting */
    int          qlen_disc;    /* Pkts discarded due to queue len exceeded */
    sal_spinlock_t lock;       /* For locking the queue */
} rx_queue_t;

/* rx_queue_t *q below */
#define _Q_LOCK(q)         sal_spinlock_lock((q)->lock)
#define _Q_UNLOCK(q)       sal_spinlock_unlock((q)->lock)

/* Steal up to max packets from the queue.  Locking. */
#define _Q_STEAL(q, first_pkt, max)                     \
    do {                                                \
        int _i = 0;                                     \
        bcm_pkt_t *last_pkt;                            \
        bcm_pkt_t *prev_pkt = NULL;                     \
        _Q_LOCK(q);                                     \
        last_pkt = (q)->head;                           \
        (first_pkt) = last_pkt;                         \
        while (_i < (max) && last_pkt != NULL) {        \
            _i++;                                       \
            prev_pkt = last_pkt;                        \
            last_pkt = last_pkt->_next;                 \
        }                                               \
        if (prev_pkt != NULL) { /* Just to be safe */   \
             prev_pkt->_next = NULL; /* Mark list end */\
        }                                               \
        (q)->head = last_pkt;                           \
        if (last_pkt == NULL) { /* No more pkts in q */ \
            (q)->tail = NULL;                           \
        }                                               \
        (q)->count -= _i; /* took _i pkts off q */      \
        _Q_UNLOCK(q);                                   \
    } while (0)

/* Steal all packets from the queue.  Locking. */
#define _Q_STEAL_ALL(q, first_pkt)  \
    do {                            \
        _Q_LOCK(q);                 \
        (first_pkt) = (q)->head;    \
        (q)->head = NULL;           \
        (q)->tail = NULL;           \
        (q)->count = 0;             \
        _Q_UNLOCK(q);               \
    } while (0)

#define _Q_EMPTY(q) ((q)->tail == NULL)

#define _Q_ENQUEUE(q, pkt)                               \
    do {                                                 \
        _Q_LOCK(q);                                      \
        (pkt)->_next = NULL;                             \
        if (_Q_EMPTY(q)) (q)->head = (q)->tail = (pkt);  \
        else {                                           \
            (q)->tail->_next = (pkt);                    \
            (q)->tail = (pkt);                           \
        }                                                \
        (q)->count++;                                    \
        (q)->tot_pkts++;                                 \
        _Q_UNLOCK(q);                                    \
    } while (0)

#define RX_CHAINS_MAX 20

/*
 * Typedef:
 *      rx_chan_ctl_t
 * Purpose:
 *      Control structure per channel
 * Notes:
 *      Tokens must be signed
 */

typedef struct rx_chan_ctl_s {
    dv_t            *dv[RX_CHAINS_MAX];
    void            *reserved[4];     /* for compability */
    int              dcb_per_pkt;     /* Number DCB/pkt */

    /* Some stats counters */
    int              rpkt;    /* Received packets */
    int              rbyte;   /* Received bytes */
    int              dpkt;    /* Discard packets */
    int              dbyte;   /* Discard bytes */
    int              mem_fail;  /* How many packet allocation errors */
    bcm_pkt_t       *all_pkts;
} rx_chan_ctl_t;

typedef void (* bcm_rx_parser_f)(int unit, bcm_pkt_t *pkt);

/*
 * Typedef:
 *      rx_ctl_t
 * Purpose:
 *      Control structure per unit for RX
 * Notes:
 *      Tokens must be signed
 */

typedef struct rx_ctl_s {
    int               rc_lock;            /* For SPL locking */

    /**************** LOCAL ONLY UNIT CONTROL         ****************/
    volatile int      hndlr_intr_cnt;     /* Number of interrupt callouts */
    volatile uint32   chan_running;       /* Bitmap of channels enabled */
    int               cur_chan;           /* For fairness in refill/start */
    rx_chan_ctl_t     chan_ctl[BCM_CMICX_RX_CHANNELS]; /* internal chan ctl */


    /**************** LOCAL AND REMOTE UNIT CONTROL   ****************/
    bcm_rx_cfg_t      user_cfg;           /* User configuration */
    volatile rx_callout_t  *rc_callout;   /* Callout linked list */
    volatile int      hndlr_cnt;          /* Non-intr callouts, no disc */
    rx_queue_t        *pkt_queue;         /* Packets to process/cos BCM_RX_COS*/
    bcm_pkt_t       **all_pkts;           /* Packet Descriptors avail for Rx */
    void            **pkt_load;           /* Buffers to give back to hw */
    int              *pkt_load_ids;       /* 1:1 buffer to id array */
    bcm_rx_parser_f   rx_parser;
    
    /* Thread control */
    char              rx_tname[16];    /* Thread name */
    sal_sem_t         pkt_notify;      /* Semaphor for pkt thrd */
    sal_thread_t      rx_tid;          /* packet thread id */
    int               sleep_cur;       /* Current sleep time */
    int               rx_thread_pri;   /* thread priority */
    volatile int      thread_running;  /* Input signal to thread */
    volatile int      thread_exit_complete; /* Output signal from thread */

    /* For global rate control */
    volatile int      tokens;             /* Rate control tokens. */
    sal_usecs_t       last_fill;          /* Last time bucket updated */
    sal_mutex_t       rx_mutex;           /* Sync for handler list */

    int               flags;
    int               olp_match_rule;     /* Olp Match Rule */
#define BCM_RX_F_STARTED          0x1  /* Started? */
#define BCM_RX_REASONS_BFD        0x2  /* BFD reason code configured */
#define BCM_RX_REASONS_MPLS_BFD   0x4  /* MPLS reason code configured using BFD */
    uint8             cpu_port_priorities;
    /**************** Counters                       ****************/
    int               restart_errs;
    int               q_depth;
    int               tot_pkts;
    int               dpkt;
    int               dbyte;
    int               rpkt;
    int               rbyte;
    int               pkts_since_start; /* Since last start, how many pkts */
    int               bad_hndlr_rv;
    int               no_hndlr;
    int               not_running;
    int               thrd_not_running;
    int               dcb_errs;
    int               pkts_owned;
    int               tunnelled;
    int               reasonMapInitialized;
    int               next_unit;       /* Next unit with RX started. */
    bcm_cos_queue_t   queue_max;       /* Maximum number of cpu queues. */
    volatile void    *free_list;       /* Deferred free list */
} rx_ctl_t;

/* DV related defines and DV state transition diagram
 * 
 * State desriptions:
 *    DV_S_NEEDS_FILL     The DV needs to have its packets checked
 *    DV_S_FILLED         The DV has packets and is ready to be sent
 *                        to the SOC layer.
 *    DV_S_SCHEDULED      The DV could not be started due to rate
 *                        limiting, so it has been scheduled for a DPC
 *    DV_S_ACTIVE         The DV has been handed to the SOC layer and
 *                        is receiving packets.
 *    DV_S_CHN_DONE       A chain done interrupt has occurred for this
 *                        DV.  The SOC layer is done with the DV and it
 *                        now contains packets to process.
 *    DV_S_RLD_DONE       A desc done inter occurred and the reload bit
 *                        is set for this DV indicating that the SOC layer
 *                        is done with the DV and DMA has followed the
 *                        reload address.
 *    DV_S_ERROR          An error has occurred.  The DV will not be
 *                        scheduled until RX is restarted.
 *
 * DVs start in the state NEEDS_FILL.
 * 
 *     NEEDS_FILL  -->  FILLED          rx_dv_fill
 *     FILLED      -->  ACTIVE          rx_chain_start
 *     FILLED      -->  SCHEDULED       rx_chain_start_or_schedule
 *     SCHEDULED   -->  ACTIVE          rx_chain_start
 *     ACTIVE      -->  CHN_DONE        rx_done_chain
 *     ACTIVE      -->  RLD_DONE        rx_done_reload
 *     ACTIVE      -->  NEEDS_FILL      all pkts processed, see below
 *     CHN_DONE    -->  NEEDS_FILL      see below
 *     RLD_DONE    -->  NEEDS_FILL      see below
 *
 * A DV transitions from RLD_DONE/CHN_DONE or ACTIVE to NEEDS_FILL when all
 * its packets have been processed.  This is detected by the
 * pkt_done_cnt of DV_INFO reaching the pkts/chain level.  Note that
 * if all pkts are being handled in the interrupt handler, this may
 * occur before the DV has transitioned to CHN_DONE which occurs in
 * the chain done interrupt handler.
 */

typedef enum dv_states_e {
    DV_S_NEEDS_FILL,          /* Packets are processed, needs refill */
    DV_S_FILLED,              /* DV filled, ready to be started */
    DV_S_SCHEDULED,           /* DV has been scheduled via a DPC */
    DV_S_ACTIVE,              /* DV has been started (given to soc layer) */
    DV_S_CHN_DONE,            /* DV is done, needs service (chain done) */
    DV_S_RLD_DONE,            /* DV is done, but pkts may not be drained */
    DV_S_ERROR                /* DV has seen error */
} dv_states_t;

#define DV_STATE_STRINGS { \
    "Needs Fill",          \
    "Filled",              \
    "Scheduled",           \
    "Active",              \
    "Chain Done",          \
    "Error"}

/* This is all the extra information associated with a DV for RX */
typedef struct rx_dv_info_s {
    volatile dv_states_t state;
    volatile sal_usecs_t sched_time;   /* When was DV scheduled */
    volatile int time_diff;            /* Delta time until started */
    volatile int abort_cleanup;        /* First packet after Start Channel */
    uint8 idx;
    uint8 chan;
    volatile uint8 pkt_done_cnt;
} rx_dv_info_t;

/*
 * DV and DCB related macros
 */
#define _DV_INFO dv_public1.ptr
#define DV_INFO(dv)          (((rx_dv_info_t *)(((dv_t *)dv)->_DV_INFO)))

/*
 * Record both the time that the DV is scheduled and how many
 * usecs in the future to schedule.
 */
#define DV_TIME_DIFF(dv)     (DV_INFO(dv)->time_diff)
#define DV_SCHED_TIME(dv)    (DV_INFO(dv)->sched_time)

/*
 * How much futher into the future to schedule.  If this is <= 0,
 * it indicates the DV is ready.
 */
#define DV_FUTURE_US(dv, cur_us)                                  \
    (DV_TIME_DIFF(dv) - (int)((cur_us >= DV_SCHED_TIME(dv)) ?     \
     (SAL_USECS_SUB(cur_us, DV_SCHED_TIME(dv))) :                 \
     (SAL_USECS_ADD(cur_us, SAL_USECS_SUB(0xffffffff, DV_SCHED_TIME(dv))))))

#define DV_INDEX(dv)                 (DV_INFO(dv)->idx)
#define DV_STATE(dv)                 (DV_INFO(dv)->state)
#define DV_CHANNEL(dv)               (DV_INFO(dv)->chan)
#define DV_ABORT_CLEANUP(dv)         (DV_INFO(dv)->abort_cleanup)

#define DV_PKT(dv, i) \
    (RX_PKT(dv->dv_unit, DV_CHANNEL(dv), DV_INFO(dv)->idx, i))
#define DV_PKTS_PROCESSED(dv)        (DV_INFO(dv)->pkt_done_cnt)

#define DV_RX_IDX(dv)                ((dv)->dv_public2.u32)
#define DV_RX_IDX_SET(dv, val)       (dv)->dv_public2.u32 = (val)


/****************************************************************
 *
 * The following defines can only be used in bcm/rx.c as they
 * refer to the static variable rx_ctl
 */
extern rx_ctl_t    *rx_ctl[];

/*
 * RX_LOCK/UNLOCK should only be used to synchronize with
 * interrupt handlers.  Code protected by these calls should
 * only involve memory references.
 */

#define RX_LOCK(unit)        \
    sal_mutex_take(rx_ctl[unit]->rx_mutex, sal_mutex_FOREVER)
#define RX_UNLOCK(unit)      \
    sal_mutex_give(rx_ctl[unit]->rx_mutex)

/****************************************************************
 * Access macros for rx_ctl:
 *     RX_INIT_DONE(unit)            Has init been called?
 *     RX_UNIT_CHECK(unit)           Legal unit number?
 *     RX_INIT_CHECK(unit)           Return error if init not done
 *     RX_CHAN_USED(unit, chan)      Has the channel been setup?
 *     RX_CHAN_RUNNING(unit, chan)   Is channel currently running (active)?
 *
 *     RX_PPC(unit)                  Packets/chain
 *     RX_PPS(unit)                  Global pkts/second rate limit
 *     RX_BURST(unit)                Global burst rate (full bucket setting)
 *
 *     RX_CHAN_COS(unit, chan)       Channel's COS bitmap
 *     RX_CHAN_BURST(unit)           Channel's burst rate (full bucket)
 *
 *     RX_DV(unit, chan, i)          Get pointer to i-th dv of channel
 *     RX_DV_PKT(unit, chan, dv_idx, pkt_idx)
 *                                   Pointer to packet for given dv
 */

/* Access macros for rx_ctl: */
#define RX_INIT_DONE(unit)          (rx_ctl[unit] != NULL)
#define RX_UNIT_CHECK(unit)         \
    if (((unit) < 0) || ((unit) >= BCM_CONTROL_MAX)) return BCM_E_UNIT
#define RX_INIT_CHECK(unit)         \
    do { \
        RX_UNIT_CHECK(unit); \
        if (!RX_INIT_DONE(unit)) \
            BCM_IF_ERROR_RETURN(bcm_rx_init(unit)); \
    } while (0)
#define RX_CHAN_USED(unit, chan)    (RX_CHAN_CFG(unit, chan).chains != 0)
#define RX_CHAN_RUNNING(unit, chan) (rx_ctl[unit]->chan_running & (1 << chan))

/* Global configuration */
#define RX_PPC(unit)                (rx_ctl[unit]->user_cfg.pkts_per_chain)
#define RX_PPS(unit)                (rx_ctl[unit]->user_cfg.global_pps)
#define RX_BURST(unit)              (rx_ctl[unit]->user_cfg.max_burst)
#define RX_THREAD_PRI(unit)         (rx_ctl[unit]->user_cfg.thread_pri)
#define RX_TOKENS(unit)             (rx_ctl[unit]->tokens)
#define RX_QUEUE_MAX(unit)          (rx_ctl[unit]->queue_max)
#define RX_OLP_MATCH_RULE(unit)     (rx_ctl[unit]->olp_match_rule)

/* Channel configuration */
#define RX_CHAN_CFG(unit, chan)     (rx_ctl[unit]->user_cfg.chan_cfg[chan])
#define RX_CHAN_FLAGS(unit, chan)   (RX_CHAN_CFG(unit, chan).flags)
#define RX_CHAINS(unit, chan)       (RX_CHAN_CFG(unit, chan).chains)
#define RX_CRC_STRIP(unit, chan) \
    (RX_CHAN_FLAGS(unit, chan) & BCM_RX_F_CRC_STRIP)
#define RX_VTAG_STRIP(unit, chan) \
    (RX_CHAN_FLAGS(unit, chan) & BCM_RX_F_VTAG_STRIP)
#define RX_PKT_SIZE(unit)     (rx_ctl[unit]->user_cfg.pkt_size)
#define RX_USER_FLAGS(unit)   (rx_ctl[unit]->user_cfg.flags)
#define RX_IGNORE_HG(unit)    (RX_USER_FLAGS(unit) & BCM_RX_F_IGNORE_HGHDR)
#define RX_IGNORE_SL(unit)    (RX_USER_FLAGS(unit) & BCM_RX_F_IGNORE_SLTAG)

/* Per COS rate control */
#define RX_QUEUE(unit, cos)         (&(rx_ctl[unit]->pkt_queue[cos]))
#define RX_COS_PPS(unit, cos)       (RX_QUEUE(unit, cos)->pps)
#define RX_COS_BURST(unit, cos)     (RX_QUEUE(unit, cos)->burst)
#define RX_COS_MAX_LEN(unit, cos)   (RX_QUEUE(unit, cos)->max_len)
#define RX_COS_TOKENS(unit, cos)    (RX_QUEUE(unit, cos)->tokens)

#define RX_CHAN_COS(unit, chan)     (RX_CHAN_CFG(unit, chan).cos_bmp)
#define RX_CHAN_BURST(unit, chan)   (RX_CHAINS(unit, chan) * RX_PPC(unit))

/* Internal configuration/control */
#define RX_CHAN_CTL(unit, chan)     (rx_ctl[unit]->chan_ctl[chan])
#define RX_CHAN_PKTS(unit, chan)    (rx_ctl[unit]->chan_ctl[chan].all_pkts)

#define RX_PKT(unit, chan, dv_idx, idx) \
    (&(RX_CHAN_CTL(unit, chan).all_pkts[dv_idx * RX_PPC(unit) + idx]))

/* DV and DCB related macros */
#define RX_DCB_PER_PKT(unit, chan)  (RX_CHAN_CTL(unit, chan).dcb_per_pkt)
#define RX_DCB_PER_DV(unit, chan) \
    (RX_DCB_PER_PKT(unit, chan) * RX_PPC(unit))

#define RX_DV(unit, chan, i) (RX_CHAN_CTL(unit, chan).dv[i])

/* For each channel that is currently running */
#define FOREACH_RUNNING_CHANNEL(unit, chan) \
    for (chan = 0; chan < BCM_CMICX_RX_CHANNELS; chan++) \
        if (RX_CHAN_RUNNING(unit, chan))

/* For each channel that is setup (could be or is active) */
#define FOREACH_SETUP_CHANNEL(unit, chan) \
    for (chan = 0; chan < BCM_CMICX_RX_CHANNELS; chan++) \
        if (RX_CHAN_USED(unit, chan))

#ifdef BCM_RPC_SUPPORT
extern int     _bcm_rx_sl_mode_update(int unit);
#endif

extern int rxp_memory_source_heap;
/* Default values */

#if !defined(BCM_RX_POOL_COUNT_DEFAULT)
#ifdef BCM_DNX_SUPPORT
#define BCM_RX_POOL_COUNT_DEFAULT      960
#else
#define BCM_RX_POOL_COUNT_DEFAULT      256
#endif
#endif

#if !defined(BCM_RX_POOL_BYTES_DEFAULT)
#define BCM_RX_POOL_BYTES_DEFAULT      RX_PKT_SIZE_DFLT
#endif

#define LEGAL_COS(cos) ((cos) >= BCM_RX_COS_ALL && (cos) < BCM_RX_COS)

extern int                      rx_spl;
#define RX_INTR_LOCK            rx_spl = sal_splhi()
#define RX_INTR_UNLOCK          sal_spl(rx_spl)

#define BCM_RX_CTRL_ACTIVE_UNITS_UPDATE (1 << 0)

/* Single RX control structure, common for all units */
typedef struct rx_control_s {
    /* Thread control */
    sal_sem_t         pkt_notify;       /* Semaphore for pkt thrd */
    volatile int      pkt_notify_given; /* Semaphore already given */
    sal_thread_t      rx_tid;           /* packet thread id */
    sal_thread_t      adapter_rx_tid;    /* adapter packet thread id */
    int               sleep_cur;        /* Current sleep time */
#define MIN_SLEEP_US 5000               /* Always sleep at least 5 ms */
#define MAX_SLEEP_US 100000             /* For non-rate limiting, .1 sec */
    volatile int      thread_running;   /* Input signal to thread */
    volatile int      thread_exit_complete; /* Output signal from thread */

    /* System wide rate limiting */
    int               system_pps;
    int               system_tokens;
    sal_usecs_t       system_last_fill;

    /* Thread protection mutexes. */
    sal_mutex_t       system_lock;
    sal_mutex_t       start_lock;

    /* RX started units misc info. */
    int               rx_unit_first;
    bcm_cos_queue_t   system_cosq_max;

    /* System flags. */
    int               system_flags;

    /* Scheduler function pointer. */
    bcm_rx_sched_cb   rx_sched_cb;
} rx_control_t;

extern rx_control_t rx_control;


#ifdef ADAPTER_SERVER_MODE
void rx_adapter_process_packet(int unit, bcm_pkt_t *pkt);
#endif 

#define _BCM_RX_SYSTEM_LOCK                                                \
          sal_mutex_take(rx_control.system_lock, sal_mutex_FOREVER)

#define _BCM_RX_SYSTEM_UNLOCK sal_mutex_give(rx_control.system_lock)

#define _BCM_RX_START_LOCK                                                \
          sal_mutex_take(rx_control.start_lock, sal_mutex_FOREVER)

#define _BCM_RX_START_UNLOCK sal_mutex_give(rx_control.start_lock)

#define _BCM_RX_SCHED_DEFAULT_CB (_bcm_rx_default_scheduler)
#define _BCM_RX_CHECK_THREAD_DONE  if (!rx_control.thread_running) break

#define RX_IS_SETUP(unit)     ((((unit) >= 0) && \
                                ((unit) < BCM_CONTROL_MAX)) \
                                && (rx_ctl[unit] != NULL))
#define RX_UNIT_STARTED(unit) \
    (RX_IS_SETUP(unit) && rx_ctl[unit]->flags & BCM_RX_F_STARTED)
#define RX_UNIT_REASONS_BFD_SET(unit) \
    if(RX_IS_SETUP(unit)) {rx_ctl[unit]->flags |= BCM_RX_REASONS_BFD;}
#define RX_UNIT_REASONS_MPLS_BFD_SET(unit) \
    if(RX_IS_SETUP(unit)) {rx_ctl[unit]->flags |= BCM_RX_REASONS_MPLS_BFD;}
#define RX_UNIT_REASONS_BFD_GET(unit, _x_) \
    if(RX_IS_SETUP(unit)) {_x_ = rx_ctl[unit]->flags & BCM_RX_REASONS_BFD;}
#define RX_UNIT_REASONS_MPLS_BFD_GET(unit, _x_) \
    if(RX_IS_SETUP(unit)) {_x_ = rx_ctl[unit]->flags & BCM_RX_REASONS_MPLS_BFD;}
#define RX_UNIT_REASONS_BFD_CLEAR(unit) \
    if(RX_IS_SETUP(unit)) {rx_ctl[unit]->flags &= ~BCM_RX_REASONS_BFD;}
#define RX_UNIT_REASONS_MPLS_BFD_CLEAR(unit) \
    if(RX_IS_SETUP(unit)) {rx_ctl[unit]->flags &= ~BCM_RX_REASONS_MPLS_BFD;}

/* RCPU units are not local, they are remote */
#define RX_IS_RCPU(unit)        (SOC_UNIT_VALID(unit) && SOC_IS_RCPU_UNIT(unit))

#if defined(BCM_RPC_SUPPORT)
/* Accept that BCM may not be set up; then all units local */
#define RX_IS_LOCAL(unit) \
    (RX_IS_SETUP(unit) && (!BCM_UNIT_VALID(unit) || \
      (BCM_IS_LOCAL(unit) && !RX_IS_RCPU(unit))))
#define RX_IS_REMOTE(unit)    (RX_IS_SETUP(unit) && !RX_IS_LOCAL(unit))
#else
#define RX_IS_LOCAL(unit)     (RX_IS_SETUP(unit) && !RX_IS_RCPU(unit))
#define RX_IS_REMOTE(unit)    FALSE
#endif

/*
 * give semaphore.  Notification is given:
 *      When a packet is marked as processed.
 *      When the thread is stopped
 *      When a channel is enabled
 *      When a packet is queued to be serviced
 *      When a packet is queued to be freed
 */
#define RX_THREAD_NOTIFY(unit) {                                        \
    if (!rx_control.pkt_notify_given) {                                 \
        rx_control.pkt_notify_given = TRUE;                             \
        sal_sem_give(rx_control.pkt_notify);                            \
    }                                                                   \
}

extern int
_bcm_common_rx_sched_register(int unit, bcm_rx_sched_cb sched_cb);

extern int
_bcm_common_rx_sched_unregister(int unit);

extern int
_bcm_common_rx_unit_next_get(int unit, int *unit_next);

extern int
_bcm_common_rx_queue_max_get(int unit, bcm_cos_queue_t  *cosq);

extern int
_bcm_common_rx_queue_packet_count_get(int unit, bcm_cos_queue_t cosq, int *packet_count);

extern int
_bcm_common_rx_queue_rate_limit_status_get(int unit, bcm_cos_queue_t cosq,
                                       int *packet_tokens);

extern int
_bcm_common_rx_init(int unit);

extern int
_bcm_common_rx_cfg_init(int unit);

extern int
_bcm_common_rx_start(int unit, bcm_rx_cfg_t *cfg);

extern int
_bcm_common_rx_stop(int unit, bcm_rx_cfg_t *cfg);

extern int
_bcm_common_rx_clear(int unit);

extern int
_bcm_common_rx_cfg_get(int unit, bcm_rx_cfg_t *cfg);

extern int
_bcm_common_rx_queue_register(int unit, const char *name, bcm_cos_queue_t cosq,
                          bcm_rx_cb_f callback, uint8 priority, void *cookie,
                          uint32 flags);

extern int
_bcm_common_rx_register(int unit, const char *name, bcm_rx_cb_f callback,
                uint8 priority, void *cookie, uint32 flags);

extern int
_bcm_common_rx_unregister(int unit, bcm_rx_cb_f callback, uint8 priority);

extern int
_bcm_common_rx_queue_unregister(int unit, bcm_cos_queue_t cosq,
                            bcm_rx_cb_f callback, uint8 priority);

extern int
_bcm_common_rx_reasons_get (int unit, bcm_rx_reasons_t *reasons);

extern int
_bcm_common_rx_queue_channel_set (int unit, bcm_cos_queue_t queue_id,
                              bcm_rx_chan_t chan_id);
extern int
_bcm_common_rx_queue_channel_set_helper (int unit, bcm_cos_queue_t queue_id,
                              bcm_rx_chan_t chan_id, int cmc);

extern int
_bcm_common_rx_queue_channel_get(int unit, bcm_cos_queue_t queue_id,
                             bcm_rx_chan_t *chan_id);

extern int
_bcm_common_rx_active(int unit);

extern int
_bcm_common_rx_channels_running(int unit, uint32 *channels);

extern int
_bcm_common_rx_alloc(int unit, int pkt_size, uint32 flags, void **buf);

extern int
_bcm_common_rx_free(int unit, void *pkt_data);

extern int
_bcm_common_rx_free_enqueue(int unit, void *pkt_data);

extern int
_bcm_common_rx_olp_match_rule_set (int unit, int olp_match_rule);

extern int
_bcm_common_rx_olp_match_rule_get (int unit, int *olp_match_rule);

extern int
_bcm_common_rx_rate_set(int unit, int pps);

extern int
_bcm_common_rx_rate_get(int unit, int *pps);

extern int
_bcm_common_rx_cpu_rate_set(int unit, int pps);

extern int
_bcm_common_rx_cpu_rate_get(int unit, int *pps);

extern int
_bcm_common_rx_burst_set(int unit, int burst);

extern int
_bcm_common_rx_burst_get(int unit, int *burst);

extern int
_bcm_common_rx_cos_rate_set(int unit, int cos, int pps);

extern int
_bcm_common_rx_cos_rate_get(int unit, int cos, int *pps);

extern int
_bcm_common_rx_cos_burst_set(int unit, int cos, int burst);

extern int
_bcm_common_rx_cos_burst_get(int unit, int cos, int *burst);

extern int
_bcm_common_rx_cos_max_len_set(int unit, int cos, int max_q_len);

extern int
_bcm_common_rx_cos_max_len_get(int unit, int cos, int *max_q_len);
  
extern int
_bcm_common_rx_remote_pkt_enqueue(int unit, bcm_pkt_t *pkt);

#if defined(BROADCOM_DEBUG)
extern int
_bcm_common_rx_show(int unit);
#endif /* BROADCOM_DEBUG */

extern int
_bcm_rx_shutdown(int unit);
#endif
