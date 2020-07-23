/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Stack Task:
 *     Stacking coordination software
 *     Finite state machine based module to control discovery, topology
 *     analysis/programming, attaching remote devices and synchronizing
 *     inter-box state.
 *
 * Protected Resources:
 *     st_state             Current state
 *     bcm_st_disc_db       Discovery database
 *     bcm_st_cur_db        Current (active) database
 *     st_new_events
 *     st_pending_events
 *     st_blocked_events
 *                   Event bitmaps
 *     st_disc_tid   Discovery thread ID; used as running signal as well
 *     bcm_st_flags      Convey state (working to eliminate)
 */

#include <shared/bsl.h>

#include <assert.h>

#include <sal/core/sync.h>
#include <sal/core/time.h>
#include <sal/core/alloc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/stack.h>

#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/atp.h>
#include <appl/cputrans/next_hop.h>
#include <appl/discover/disc.h>

#include <appl/stktask/stktask.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/attach.h>
#include <appl/stktask/topo_brd.h>    /* bcm_board_trunk */
#include <appl/stktask/topo_pkt.h>

/* Event Logging */
#include <shared/evlog.h>
SHARED_EVLOG_DECLARE(st_log)

/**************** Stack Task State and Events *************/
bcm_st_state_t st_state; /* Current state of stack task */

/* These should be treated as static but are left visible for debugging */
volatile uint32 st_blocked_events;
volatile uint32 st_new_events;
volatile uint32 st_pending_events;
volatile int st_ev_count;

/* Last time a state transition occurred */
volatile sal_usecs_t st_transition_time;

static volatile sal_thread_t st_disc_tid = SAL_THREAD_ERROR;
int bcm_st_disc_priority = BCM_ST_DISC_PRIORITY_DEFAULT;
int bcm_st_disc_stk_size = BCM_ST_DISC_STACK_SIZE_DEFAULT;

/* Which events cause discovery to start from READY or restart from DISC */
uint32 bcm_st_disc_startable_events = BCM_ST_DISC_STARTABLE_EVENTS_DEFAULT;

/* Stack task configuration */
volatile uint32 bcm_st_flags;
char * bcm_st_flags_strings[] = BCM_ST_FLAGS_STRINGS;

static sal_usecs_t state_timeouts[BCM_STS_MAX] = BCM_STATE_TIMEOUT_DEFAULTS;

char *bcm_st_state_strings[] = BCM_ST_STATE_STRINGS;
char *bcm_st_event_strings[] = BCM_ST_EVENT_STRINGS;

/* Link Scan CB processing thread */
static volatile sal_thread_t st_lscan_tid = SAL_THREAD_ERROR;


/**********************************************************/

/* The local CPUDB pointers */
cpudb_ref_t volatile bcm_st_cur_db  = CPUDB_REF_NULL;
cpudb_ref_t volatile bcm_st_disc_db = CPUDB_REF_NULL;

/**************** Synchronization elements ****************/
static sal_sem_t st_event_sem;
static sal_sem_t st_disc_sem;
static sal_sem_t st_lscan_sem;
static sal_mutex_t st_lock;
static sal_mutex_t st_lscan_lock;

#define ST_SLEEP(_usecs) sal_sem_take(st_event_sem, (_usecs))
#define ST_WAKE sal_sem_give(st_event_sem)

#define ST_DISC_SLEEP(_usecs) sal_sem_take(st_disc_sem, (_usecs))
#define ST_DISC_WAKE sal_sem_give(st_disc_sem)

#define ST_LSCAN_SLEEP(_usecs) sal_sem_take(st_lscan_sem, (_usecs))
#define ST_LSCAN_WAKE sal_sem_give(st_lscan_sem)

#define ST_LOCK sal_mutex_take(st_lock, sal_mutex_FOREVER)
#define ST_UNLOCK sal_mutex_give(st_lock)

#define ST_LSCAN_LOCK sal_mutex_take(st_lscan_lock, sal_mutex_FOREVER)
#define ST_LSCAN_UNLOCK sal_mutex_give(st_lscan_lock)

#define ST_INIT_DONE (st_lock != NULL)
/**********************************************************/

/******** LINK SCAN CALLBACK PROCESSING ELEMENTS **********/
typedef struct bcm_st_lscan_rsp_node_s {
    int                             unit;
    int                             link_status;
    bcm_port_t                      port;
    sal_usecs_t                     link_evt_time;
    struct bcm_st_lscan_rsp_node_s  *nxt;
} bcm_st_lscan_rsp_node_t;

typedef struct bcm_st_lscan_cb_list_s {
    uint8       ready;
    bcm_st_lscan_rsp_node_t *head;
    bcm_st_lscan_rsp_node_t *tail;
} bcm_st_lscan_cb_list_t;

static bcm_st_lscan_cb_list_t st_linkscan_list;

/*
 * If the link scan caillback processing thread is asked to
 * quit then wait for max this many times in chunks of 10000 usecs.
 */
#define BCM_ST_LSCAN_CB_QUIT_RETRIES_MAX 1000

STATIC void _bcm_st_linkscan_list_enable(void);
STATIC void _bcm_st_linkscan_list_disable(void);
STATIC int _bcm_st_linkscan_list_push(int, bcm_port_t, int, sal_usecs_t);
STATIC int _bcm_st_linkscan_list_pop(int *, bcm_port_t *, int *, sal_usecs_t *);
/**********************************************************/

/* The local config structure; contents copied on start */
static bcm_st_config_t st_config;

/*
 * bcm_st_cfg_flags: Configuration variable for various Stack Task
 * properties.  This may change dynamically.
 *
 * bcm_st_link_up_db_usec:  Number of microseconds that a stack port
 * must remain up before an actual link up event is given.  Link down
 * is always payed attention to immediately (if current state is up).
 */

volatile uint32 bcm_st_cfg_flags = BCM_ST_CFG_FLAGS_DEFAULT;
volatile int bcm_st_link_up_db_usec = BCM_ST_LINK_UP_DB_USEC_DEFAULT;

/*
 * Stack Task Stack Port Configuration Info
 *
 * Stack ports may be enabled/disabled by calling bcm_st_stk_port_enable_set.
 * Indexed by stack port; link_up indicates link state per stack port;
 * last_up indicates last time a link up event was seen on that stack port
 *
 *   STK_PORT_LINK_UP(_sp)  Is the link currently up?
 *   STK_PORT_LINK_DOWN(_sp)  Is the link currently down?
 *   STK_PORT_LAST_LINK_UP(_sp)  Was last event "up"?
 *   STK_PORT_DISABLED(_sp) Has this stk port been administratively disabled?
 */

uint32 st_stk_port_flags[CPUDB_CXN_MAX];

int stk_port_last_link_event[CPUDB_CXN_MAX]; /* Up/down for last link event */
int stk_port_link_state[CPUDB_CXN_MAX];  /* ST's vision of stk port's link */
sal_usecs_t stk_port_last_event_us[CPUDB_CXN_MAX]; /* Time of last event */

#define STK_PORT_DISABLED(_sp)  (st_stk_port_flags[_sp] & ST_SPF_DISABLED)
#define STK_PORT_LINK_UP(_sp)   (st_stk_port_flags[_sp] & ST_SPF_LINK_UP)
#define STK_PORT_LINK_DOWN(_sp) (!STK_PORT_LINK_UP(_sp))
#define STK_PORT_LAST_LINK_UP(_sp)  \
    (st_stk_port_flags[_sp] & ST_SPF_LAST_EVENT_UP)

#define STK_PORT_LAST_LINK_SET(_sp, _link) \
    if (_link == BCM_PORT_LINK_STATUS_UP)  \
             st_stk_port_flags[_sp] |= ST_SPF_LAST_EVENT_UP;    \
    else st_stk_port_flags[_sp] &= ~ST_SPF_LAST_EVENT_UP

#define STK_PORT_CUR_LINK_SET(_sp, _link) \
    if (_link) st_stk_port_flags[_sp] |= ST_SPF_LINK_UP;    \
    else st_stk_port_flags[_sp] &= ~ST_SPF_LINK_UP

#define FOREACH_STK_PORT(_sp) \
    for (_sp = 0; _sp < st_config.base.num_stk_ports; _sp++) \
        if (!STK_PORT_DISABLED(_sp))

#define CPUDB_TOPO_DESTROY(db_ref) do { \
        topology_destroy(db_ref); \
        cpudb_destroy(db_ref); \
    } while (0)

volatile uint32 bcm_st_atp_flags = BCM_ST_ATP_FLAGS_DEFAULT;

/* Reserved modid feature enabled:
     reserves modid NH_TX_SRC_MOD_DEFAULT for NH.
     Actual modid via nh_tx_unknown_modid_get/set() */
static int reserved_modid_enable = TRUE;

/* Internal init function */

STATIC int
st_init(void)
{
    if (st_lock == NULL) {
        st_lock = sal_mutex_create("bcm_stk_task");
        if (st_lock == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (st_event_sem == NULL) {
        st_event_sem = sal_sem_create("bcm_stk_task_sem", sal_sem_BINARY, 0);
        if (st_event_sem == NULL) {
            sal_mutex_destroy(st_lock);
            st_lock = NULL;
            return BCM_E_MEMORY;
        }
    }

    if (st_disc_sem == NULL) {
        st_disc_sem = sal_sem_create("bcm_stk_task_disc", sal_sem_BINARY, 0);
        if (st_disc_sem == NULL) {
            sal_mutex_destroy(st_lock);
            st_lock = NULL;
            return BCM_E_MEMORY;
        }
    }

    if (NULL == st_lscan_sem) {

        st_lscan_sem = sal_sem_create("bcm_stk_task_lscan_sem", sal_sem_BINARY, 0);
        if (NULL == st_lscan_sem) {
            sal_mutex_destroy(st_lock);
            st_lock = NULL;
            return BCM_E_MEMORY;
        }
    }

    if (NULL == st_lscan_lock) {
        st_lscan_lock = sal_mutex_create("bcm_stk_task_lscan_mtx");
        if (NULL == st_lscan_lock) {
            sal_mutex_destroy(st_lock);
            st_lock = NULL;
            return BCM_E_MEMORY;
        }
    }

    return BCM_E_NONE;
}

/****************************************************************
 *
 * Per state event processing routines
 */

/*
 * Applications may want to turn off state warnings; for example,
 * if the application handles link down events specially.
 */

int bcm_st_max_bad_event_warnings = BCM_ST_MAX_BAD_EVENT_WARNINGS_DEFAULT;

STATIC void
st_bad_event_warn(bcm_st_state_t state, bcm_st_event_t event)
{
    static int warn_count = 0;

    if (warn_count < bcm_st_max_bad_event_warnings) {
        ++warn_count;
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST ERR: Unexpected event %s in state %s.\n"),
                     BCM_STE_VALID(event) ?
                     bcm_st_event_strings[event] : "(invalid)",
                     BCM_STS_VALID(state) ?
                     bcm_st_state_strings[state] : "(invalid)"));
    }
}

/*
 * BLOCKED state:
 *     Go to ready on timeout or unblock;
 *     Otherwise, record discovery restart events in blocked_events
 */

STATIC bcm_st_state_t
st_blocked_event_process(bcm_st_event_t event)
{
    bcm_st_state_t new_state = st_state;

    if (event == BCM_STE_UNBLOCK || event == BCM_STE_TIMEOUT) {
        new_state = BCM_STS_READY;
        st_new_events |= st_blocked_events;
    } else if (BCM_STE_DISC_STARTABLE(event)) {
        /* Record events that might restart discovery */
        st_blocked_events |= BCM_STE_FLAG(event);
    }

    return new_state;
}

/*
 * READY state:
 *     Go to DISC on restart events.
 */

STATIC bcm_st_state_t
st_ready_event_process(bcm_st_event_t event)
{
    bcm_st_state_t new_state = st_state;

    if (BCM_STE_DISC_STARTABLE(event)) { /* Start discovery */
        new_state = BCM_STS_DISC;
    } else if (event == BCM_STE_BLOCK || event == BCM_STE_TIMEOUT) {  /* block */
        new_state = BCM_STS_BLOCKED;
    } else {
        st_bad_event_warn(BCM_STS_READY, event);
    }

    return new_state;
}

/*
 * DISC state:
 *     Go to TOPO on success; restart DISC, block or ignore otherwise
 */

STATIC bcm_st_state_t
st_disc_event_process(bcm_st_event_t event)
{
    bcm_st_state_t new_state = st_state;

    if (BCM_STE_DISC_STARTABLE(event)) { /* Re-start discovery */
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: State DISC, event %s restarting discovery\n"),
                     bcm_st_event_strings[event]));
        /* Signal discovery to restart */
        st_config.st_disc_abort(DISC_RESTART_NEW_SEQ, 0);
        /* If currently sleeping, wake it */
        ST_LOCK;
        if (bcm_st_flags & BCM_STF_DISC_SLEEPING) {
            ST_DISC_WAKE;
        }
        ST_UNLOCK;
    } else if (event == BCM_STE_DISC_SUCCESS) {  /* Success */
        new_state = BCM_STS_TOPO;
    } else if (    (event == BCM_STE_BLOCK) ||
                   (event == BCM_STE_TIMEOUT) ||
                   (event == BCM_STE_DISC_FAILURE)) {  /* block */
        new_state = BCM_STS_BLOCKED;
    } else {
        st_bad_event_warn(BCM_STS_DISC, event);
    }

    return new_state;
}

/*
 * TOPO state:
 *     Go to ATTACH on success; restart DISC, block or ignore otherwise
 */

STATIC bcm_st_state_t
st_topo_event_process(bcm_st_event_t event)
{
    bcm_st_state_t new_state = st_state;

    if (event == BCM_STE_DISC_RESTART) {  /* Special case for restart */
        new_state = BCM_STS_DISC;
    } else if (BCM_STE_DISC_STARTABLE(event)) { /* Record blocked restart */
        st_blocked_events |= BCM_STE_FLAG(event);
    } else if (event == BCM_STE_TOPO_SUCCESS) {  /* Success */
        new_state = BCM_STS_ATTACH;
    } else if (   (event == BCM_STE_BLOCK) ||
                  (event == BCM_STE_TIMEOUT) ||
                  (event == BCM_STE_TOPO_FAILURE)) {  /* block */
        new_state = BCM_STS_BLOCKED;
    } else {
        st_bad_event_warn(BCM_STS_TOPO, event);
    }

    return new_state;
}

/*
 * ATTACH state:
 *     Go to BLOCKED on success; restart DISC, block or ignore otherwise
 */

STATIC bcm_st_state_t
st_attach_event_process(bcm_st_event_t event)
{
    bcm_st_state_t new_state = st_state;

    if (event == BCM_STE_DISC_RESTART) {  /* Special case for restart */
        new_state = BCM_STS_DISC;
    } else if (BCM_STE_DISC_STARTABLE(event)) { /* Record blocked restart */
        st_blocked_events |= BCM_STE_FLAG(event);
    } else if (  (event == BCM_STE_ATTACH_SUCCESS) ||
                 (event == BCM_STE_ATTACH_FAILURE) ||
                 (event == BCM_STE_BLOCK) ||
                 (event == BCM_STE_TIMEOUT)  ) {
        new_state = BCM_STS_BLOCKED;
    } else {
        st_bad_event_warn(BCM_STS_ATTACH, event);
    }

    return new_state;
}

/* LOCK HELD; start discovery. */

STATIC void
discovery_start(void)
{
    /* Give warning if discovery thread not detected or not sleeping */
    if (st_disc_tid == SAL_THREAD_ERROR) {
        /* Throw error; abort and return to caller */
        bcm_st_flags |= BCM_STF_ABORT;
        LOG_ERROR(BSL_LS_TKS_STKTASK,
                  (BSL_META("ST: Discovery thread not running on start; aborting\n")));
        return;
    }
    if (!(bcm_st_flags & BCM_STF_DISC_SLEEPING)) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Discovery thread not sleeping on start\n")));
        
    }

    /* Clear all possible restart events */
    st_blocked_events &= ~bcm_st_disc_startable_events;
    st_pending_events &= ~bcm_st_disc_startable_events;
    st_new_events &= ~bcm_st_disc_startable_events;

    ST_DISC_WAKE;
}

/* LOCK NOT HELD; stop discovery. */

STATIC int
discovery_stop(int retry_max)
{
    int rv = BCM_E_NONE;
    int retries = 0;

    rv = st_config.st_disc_abort(BCM_E_FAIL, 0);
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Discovery abort (fail) returns %s\n"),
                  bcm_errmsg(rv)));
    }

    while (!(bcm_st_flags & BCM_STF_DISC_SLEEPING) &&
           (retries++ < retry_max)) {
        sal_thread_yield();
        sal_usleep(10000);
    }
    if (!(bcm_st_flags & BCM_STF_DISC_SLEEPING)) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Discovery thread won't sleep; aborting\n")));
        ST_LOCK;
        bcm_st_flags |= BCM_STF_ABORT;
        ST_UNLOCK;
        rv = BCM_E_FAIL;
    }
    return rv;
}

/* LOCK NOT HELD; quit discovery. */

STATIC int
discovery_quit(int retry_max)
{
    int rv = BCM_E_NONE;
    int retries = 0;
    int flags;

    ST_LOCK;
    flags = (bcm_st_flags |= BCM_STF_ABORT);
    ST_UNLOCK;
    if (!(flags & BCM_STF_DISC_SLEEPING)) {
        rv = st_config.st_disc_abort(BCM_E_FAIL, 0);
        if (rv < 0) {
            LOG_WARN(BSL_LS_TKS_STKTASK,
                     (BSL_META("ST: Discovery abort (fail) returns %s\n"),
                      bcm_errmsg(rv)));
        }
    } else {
        ST_DISC_WAKE;
    }

    while (st_disc_tid != SAL_THREAD_ERROR && (retries++ < retry_max)) {
        sal_thread_yield();
        sal_usleep(10000);
    }
    if (st_disc_tid != SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Discovery thread won't quit\n")));
        rv = BCM_E_FAIL;
    }
    return rv;
}

/* LOCK HELD; Transition discovery DB to current DB. */

STATIC void
promote_disc_database(void)
{
    if (cpudb_valid(bcm_st_cur_db)) {
        if (cpudb_valid(bcm_st_cur_db->old_db)) {
            CPUDB_TOPO_DESTROY(bcm_st_cur_db->old_db);
        }
    }
    bcm_st_disc_db->old_db = bcm_st_cur_db;
    bcm_st_cur_db = bcm_st_disc_db;
    bcm_st_disc_db = CPUDB_REF_NULL;
}

/*
 * The state is changing.  Notify the appl before change occurs.  Then,
 * there are a few transitions that require some extra work.
 */

STATIC void
st_state_change(bcm_st_state_t new_state, bcm_st_event_t event)
{
    int rv;
    sal_usecs_t ttime;

    ttime = sal_time_usecs();
    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("ST: Trans from %s to %s on event %s [T=%u]\n"),
                 bcm_st_state_strings[st_state],
                 bcm_st_state_strings[new_state],
                 bcm_st_event_strings[event],
                 ttime));

    /* Tell application about transition */
    rv = st_config.st_transition(st_state, event, new_state,
                                 bcm_st_disc_db, bcm_st_cur_db);
    if (rv < 0) { /* Error from appl state transition */
        LOG_ERROR(BSL_LS_TKS_STKTASK,
                  (BSL_META("ST: Appl trans returns %s; State was %s.\n"),
                   bcm_errmsg(rv), bcm_st_state_strings[st_state]));
        new_state = BCM_STS_BLOCKED;
    }

    /* new state could change due to above; make sure still different */
    if (new_state == st_state) {
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: Trans from %s cancelled\n"),
                     bcm_st_state_strings[st_state]));
        return;
    }

    ST_LOCK;
    st_state = new_state;

    /* Clear timeout flag as that would have applied to previous state */
    st_pending_events &= ~BCM_STE_FLAG(BCM_STE_TIMEOUT);
    st_new_events &= ~BCM_STE_FLAG(BCM_STE_TIMEOUT);

    /*
     * Some states require a bit of post processing
     *     Enter switch w/ lock; exit unlocked.
     */
    switch (new_state) {
    case BCM_STS_BLOCKED: /* Stop discovery if running */
        ST_UNLOCK;
        (void)discovery_stop(BCM_ST_DISC_STOP_RETRIES_MAX);
        break;
    case BCM_STS_DISC: /* Start discovery */
        discovery_start();
        ST_UNLOCK;
        break;
    case BCM_STS_TOPO: /* Start topology processing */
        ST_UNLOCK;
        if ((rv = st_config.st_topo(bcm_st_disc_db)) < 0) {
            LOG_WARN(BSL_LS_TKS_STKTASK,
                     (BSL_META("ST: st_topo returns %s\n"),
                      bcm_errmsg(rv)));
        }
        break;
    case BCM_STS_ATTACH: /* Switch database over; call stack attach update */
        promote_disc_database();
        ST_UNLOCK;
        if ((rv = st_config.st_attach(bcm_st_cur_db)) < 0) {
            LOG_WARN(BSL_LS_TKS_STKTASK,
                     (BSL_META("ST: st_attach returns %s\n"),
                      bcm_errmsg(rv)));
        }
        break;
    default:
        ST_UNLOCK;
        break;
    }
    st_transition_time = ttime;
}

/*
 * Process one event.  Just check the current state and call it's event
 * processing routine.
 */
STATIC void
st_event_process(bcm_st_event_t event)
{
    bcm_st_state_t new_state = BCM_STS_INVALID;
#if defined(INCLUDE_SHARED_EVLOG)
    bcm_st_state_t prev_state = BCM_STS_INVALID;
#endif
        
    switch (st_state) {
    case BCM_STS_BLOCKED:
        new_state = st_blocked_event_process(event);
        break;
    case BCM_STS_READY:
        new_state = st_ready_event_process(event);
        break;
    case BCM_STS_DISC:
        new_state = st_disc_event_process(event);
        break;
    case BCM_STS_TOPO:
        new_state = st_topo_event_process(event);
        break;
    case BCM_STS_ATTACH:
        new_state = st_attach_event_process(event);
        break;
    default:  /* Shouldn't get here */
        assert(0 && "ST: INVALID STATE");
        break;
    }

    if (new_state != BCM_STS_INVALID && new_state != st_state) {
#if defined(INCLUDE_SHARED_EVLOG)
        prev_state = st_state;        
        SHARED_EVENT_LOG(st_log, "START",
                         st_state, new_state, event, 0);
#endif
        
        st_state_change(new_state, event);

#if defined(INCLUDE_SHARED_EVLOG)
        SHARED_EVENT_LOG(st_log, "END",
                         prev_state, st_state, event, 0);
#endif
    }
}

#define PENDING_FLAG(event)  (st_pending_events & BCM_STE_FLAG(event))
#define CLEAR_PENDING_FLAG(event)  st_pending_events &= ~BCM_STE_FLAG(event)

/*
 * Event precedence:
 *     BLOCK over UNBLOCK
 *     SUCCESS over FAILURE in general
 *
 * These are resolved by the order in which the events are processed,
 * which in turn is the order in which they're defined.  See stktask.h.
 */

STATIC void
st_pending_events_handle(void)
{
    int i;

    for (i = 0; i < BCM_STE_MAX; i++) {
        if (PENDING_FLAG(i)) {
            st_event_process((bcm_st_event_t)i);
        }
        CLEAR_PENDING_FLAG(i);
    }

    if (st_pending_events != 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Clearing illegal pending flags: 0x%x\n"),
                  st_pending_events));
        st_pending_events = 0;
    }
}

#undef PENDING_FLAG
#undef CLEAR_PENDING_FLAG

/*
 * Returns number of usecs until next time out;
 *     0 if already timedout.
 *     sal_sem_FOREVER if no timeout set
 */

STATIC sal_usecs_t
st_timeout_get(void)
{
    int diff;
    sal_usecs_t timeout = (sal_usecs_t)sal_sem_FOREVER;

    if (state_timeouts[st_state] != 0) {
        /* Timeout out active for this state */
        diff = SAL_USECS_SUB(sal_time_usecs(), st_transition_time);
        if ((diff < 0) || (diff > state_timeouts[st_state])) {
            timeout = 0;
        } else { /* diff < timeout */
            timeout = state_timeouts[st_state] - diff;
        }
    }

    return timeout;
}

/*
 * Is port's link state pending (needing attention)?
 *    That is, link is curently down and was last reported up.
 */
#define STK_PORT_LINK_PENDING(_i) \
    (STK_PORT_LINK_DOWN(_i) && STK_PORT_LAST_LINK_UP(_i))

/*
 * Checks for pending link change events; finds time to next
 * link change consideration if there are any; otherwise, returns time
 * until next timeout (if any)
 */

STATIC sal_usecs_t
st_sleep_time_get(void)
{
    sal_usecs_t sleep_time;
    sal_usecs_t link_check = 0; /* Compiler warning */
    sal_usecs_t cur_time;
    int diff;
    int i, found = FALSE;

    sleep_time = st_timeout_get();

    /* Look for pending link change events */
    cur_time = sal_time_usecs();
    FOREACH_STK_PORT(i) {
        if (STK_PORT_LINK_PENDING(i)) {

            /* Get time since last event; then subtract from debounce time */
            diff = SAL_USECS_SUB(cur_time, stk_port_last_event_us[i]);
            diff = bcm_st_link_up_db_usec - diff;
            diff = diff < 0 ? 0 : diff;
            /* diff is now time until pending link event should debounce */

            if (found) {  /* Get minimum of those found */
                if (diff < link_check) {
                    link_check = diff;
                }
            } else {
                link_check = diff;
                found = TRUE;
            }
        }
    }

    if (found && link_check < sleep_time) {
        sleep_time = link_check;
    }

    return sleep_time;
}


STATIC void
link_up_debounce_check(void)
{
    sal_usecs_t cur_time;
    int diff;
    int i;

    cur_time = sal_time_usecs();

    FOREACH_STK_PORT(i) {
        if (STK_PORT_LINK_PENDING(i)) {
            diff = SAL_USECS_SUB(cur_time, stk_port_last_event_us[i]);
            if (diff >= bcm_st_link_up_db_usec || diff < 0) {
                /* Set port link state to 1 and give link up event */
                STK_PORT_CUR_LINK_SET(i, TRUE);
                st_new_events |= BCM_STE_FLAG(BCM_STE_LINK_UP);
            } /* Debounce time has elapsed */
        } /* last event up and currently down */
    } /* Foreach connection */
}

/****************************************************************
 *
 * Stack event loop
 */

STATIC void
st_event_loop(void)
{
    sal_usecs_t sleep_time;

    while (!(bcm_st_flags & BCM_STF_ABORT)) {
        if (st_new_events == 0 && st_pending_events == 0) {
            ST_LOCK;
            sleep_time = st_sleep_time_get();
            ST_UNLOCK;
            if (sleep_time > 0) {
                ST_SLEEP(sleep_time);
            }
        }

        if (bcm_st_flags & BCM_STF_ABORT) {
            break;  /* Signalled to exit */
        }
        /* Check again to see if timeout has occurred */
        ST_LOCK;
        if (st_timeout_get() == 0) {
            st_new_events |= BCM_STE_FLAG(BCM_STE_TIMEOUT);
        }

        /* Check if link debounce needs attention */
        link_up_debounce_check();

        /* Transition new flags to pending flags */
        st_pending_events |= st_new_events;
        st_new_events = 0;
        ST_UNLOCK;

        st_pending_events_handle();
    }
}


/****************************************************************
 *
 * Utility functions, stack ports, etc.
 */

STATIC bcm_rx_t st_nh_callback(cpudb_key_t src_key,
                               int mplx_num,
                               int rx_unit,
                               int rx_port,
                               uint8 *pkt_buf,
                               int len,
                               void *cookie);

STATIC void st_linkscan_handler(int unit,
                                bcm_port_t port,
                                bcm_port_info_t *info);

STATIC void st_comm_failure(cpudb_key_t key);

/* Generate the list of units with stack ports from local information */

static int st_stk_units[CPUDB_UNITS_MAX];
static int st_num_stk_units;

STATIC int
stk_units_gen(void)
{
    cpudb_unit_port_t *sp;
    int i, j;
    int found;

    st_num_stk_units = 0;

    /* Create list of unique entries */
    for (i = 0; i < st_config.base.num_stk_ports; i++) {
        found = FALSE;
        sp = (cpudb_unit_port_t *)&st_config.base.stk_ports[i];
        for (j = 0; j < st_num_stk_units; j++) {
            if (st_stk_units[j] == sp->unit) {
                found = TRUE;
                break;
            }
        }
        if (!found) {
            if (st_num_stk_units >= CPUDB_UNITS_MAX) {
                LOG_ERROR(BSL_LS_TKS_STKTASK,
                          (BSL_META("ST: cannot stack more than %d units\n"),
                           CPUDB_UNITS_MAX));
                return BCM_E_FAIL;
            }
            st_stk_units[st_num_stk_units++] = sp->unit;
        }
    }
    
    return BCM_E_NONE;
}

/* Link and communications setup.  Called once during ST initialization */

STATIC void
st_comm_setup(void)
{
    int i, rv;
    int unit, port;

    /* Register for communication failure notification */
    if (bcm_st_cfg_flags & BCM_STC_COMM_FAIL_REGISTER) {
        atp_timeout_register(st_comm_failure);
    }

    for (i = 0; i < st_config.base.num_stk_ports; i++) {
        unit = st_config.base.stk_ports[i].unit;
        port = st_config.base.stk_ports[i].port;
        rv = next_hop_port_add(unit, port, 0);
        if (rv < 0) {
            LOG_WARN(BSL_LS_TKS_STKTASK,
                     (BSL_META("ST: stkport %d.%d nexthop add failed: %s\n"),
                      unit, port, bcm_errmsg(rv)));
        }
    }

    /* Register to receive discovery packets; warn if NH not running */
    if (bcm_st_cfg_flags & BCM_STC_DISC_PKT_REGISTER) {
        rv = next_hop_register(st_nh_callback, NULL, SHARED_PKT_TYPE_DISC_NH);
        if (rv < 0) {
            LOG_WARN(BSL_LS_TKS_STKTASK,
                     (BSL_META("ST: nexthop register failed: %s\n"),
                      bcm_errmsg(rv)));
        }
    }
    if (!next_hop_running()) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Nexthop is not running\n")));
    }
    if ((rv = next_hop_update((cpudb_base_t *)&st_config.base)) < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Nexthop update returns %s\n"),
                  bcm_errmsg(rv)));
    }

    if (bcm_st_cfg_flags & BCM_STC_START_ATP) { /* Start and bring up ATP */
        if (!atp_running()) {
            uint32 unit_bmp = 0;

            for (i = 0; i < st_num_stk_units; i++) {
                unit_bmp |= (1 << st_stk_units[i]);
            }
            if ((rv = atp_start(bcm_st_atp_flags, unit_bmp,
                                BCM_RCO_F_ALL_COS)) < 0) {
                LOG_WARN(BSL_LS_TKS_STKTASK,
                         (BSL_META("ST: ATP start returns %s\n"),
                          bcm_errmsg(rv)));
            }
        }
    }

    /* Add local key to ATP */
    if ((rv = atp_key_add(st_config.base.key, TRUE)) < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: ATP key add returns %s\n"),
                  bcm_errmsg(rv)));
    }
}

/* Undo above code on shutdown */

STATIC void
st_comm_shutdown(void)
{
    int i;

    for (i = 0; i < st_num_stk_units; i++) {
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: linkscan unregister\n")));
        (void)bcm_linkscan_unregister(st_stk_units[i], st_linkscan_handler);
    }
    if (bcm_st_cfg_flags & BCM_STC_COMM_FAIL_REGISTER) {
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: unregister comm fail\n")));
        (void)atp_timeout_register(NULL);
    }
    if (bcm_st_cfg_flags & BCM_STC_DISC_PKT_REGISTER) {
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: disc packet unregister\n")));
        (void)next_hop_unregister(st_nh_callback, SHARED_PKT_TYPE_DISC_NH);
    }
    if (bcm_st_cfg_flags & BCM_STC_START_ATP) {
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: stopping ATP\n")));
        (void)atp_stop();
    }
}

/****************************************************************
 *
 * Next hop packet handler (to check for discovery pkts)
 */

/*
 * Return index of stack port if present, -1 if not
 * Note that disabled ports are ignored.
 */

STATIC int
st_stk_port_find(int unit, int port, int include_disabled)
{
    int i;
    cpudb_unit_port_t *sp_info;

    if (!(bcm_st_flags & BCM_STF_CONFIG_LOADED)) {
        return -1;
    }

    if (include_disabled) {
        for (i = 0; i < st_config.base.num_stk_ports; i++) {
            sp_info = (cpudb_unit_port_t *)&st_config.base.stk_ports[i];
            if (unit == sp_info->unit && port == sp_info->port) {
                return i;
            }
        }
    } else {
        FOREACH_STK_PORT(i) {
            sp_info = (cpudb_unit_port_t *)&st_config.base.stk_ports[i];
            if (unit == sp_info->unit && port == sp_info->port) {
                return i;
            }
        }
    }
    return -1;
}

STATIC bcm_rx_t
st_nh_callback(cpudb_key_t src_key,
               int mplx_num,
               int rx_unit,
               int rx_port,
               uint8 *pkt_buf,
               int len,
               void *cookie)
{
    int idx;

    if (!(bcm_st_flags & BCM_STF_RUNNING)) {        /* Task isn't running */
        return BCM_RX_NOT_HANDLED;
    }
    if (bcm_st_flags & BCM_STF_ABORT) {             /* Aborted */
        return BCM_RX_NOT_HANDLED;
    }
    if (!(bcm_st_flags & BCM_STF_DISC_SLEEPING)) {  /* Discovery running */
        return BCM_RX_NOT_HANDLED;
    }

    /*
     * If received on a stack port and it's a probe packet,
     * count as an event
     */
    idx = st_stk_port_find(rx_unit, rx_port, FALSE);
    if (idx < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Nexthop pkt on non-stk port (%d, %d)\n"),
                  rx_unit, rx_port));
        return BCM_RX_NOT_HANDLED;
    }

    /* If link state of stack link is not up, ignore packet */
    if (!STK_PORT_LINK_UP(idx)) {
        return BCM_RX_HANDLED;
    }

    if (disc_pkt_type_get(pkt_buf) != SHARED_PKT_TYPE_PROBE) {
        /* Only pay attention to probe packets */
        return BCM_RX_HANDLED;
    }

    if (st_state == BCM_STS_TOPO) {
        /* If we're in TOPO and we receive a probe, fail TOPO.
           There's no point in completing topology configuration if
           there's another system that needs to be part of the stack.
        */
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: Probe pkt in TOPO from " CPUDB_KEY_FMT_EOLN),
                     CPUDB_KEY_DISP(src_key)));

        bcm_st_event_send(BCM_STE_TOPO_FAILURE);

    } else {
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: Probe pkt in from " CPUDB_KEY_FMT_EOLN),
                     CPUDB_KEY_DISP(src_key)));
        /* Discovery pkt on stack port */
        bcm_st_event_send(BCM_STE_DISC_PKT);
    }


    return BCM_RX_HANDLED;
}

/****************************************************************
 *
 * Callback for link change event
 */

STATIC void
st_linkscan_handler(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    /* Enqueue the response to a list which will be read aysncronously */
    _bcm_st_linkscan_list_push(unit, port, info->linkstatus, sal_time_usecs());

    /* Wake the linkscan processing thread to process the link event */
    ST_LSCAN_WAKE;
}

/*
 * Function:
 *      st_link_state_set/get
 * Purpose:
 *      Get/set the internal (stacktask) link state of stack ports
 * Parameters:
 *      unit, port   -- of stack port to examine
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Applications may control link signalling to stack task by
 * clearing the BCM_STC_LINK_REGISTER in bcm_st_cfg_flags and setting up
 * their own routines to send signals.  Note, however, that stack task
 * maintains a lot of link state and does its own debounce.  If the
 * application sends link signals, it must call st_link_state_set as
 * well.  Debounce is then disabled.
 */

int
bcm_st_link_state_set(int unit, bcm_port_t port, int link)
{
    int idx;

    ST_LOCK;
    idx = st_stk_port_find(unit, port, FALSE);
    if (idx < 0) {   /* Not a stack port */
        ST_UNLOCK;
        return BCM_E_NOT_FOUND;
    }

    /* Setting both disables debounce */
    STK_PORT_CUR_LINK_SET(idx, link);
    STK_PORT_LAST_LINK_SET(idx, link);
    ST_UNLOCK;

    return BCM_E_NONE;
}

int
bcm_st_link_state_get(int unit, bcm_port_t port, int *link)
{
    int idx;

    if (link == NULL) {
        return BCM_E_PARAM;
    }

    ST_LOCK;
    idx = st_stk_port_find(unit, port, FALSE);
    if (idx < 0) {   /* Not a stack port */
        ST_UNLOCK;
        return BCM_E_NOT_FOUND;
    }

    *link = STK_PORT_LINK_UP(idx);        
    ST_UNLOCK;

    return BCM_E_NONE;
}


/****************************************************************
 *
 * Callback for communication failure registered with
 * atp_timeout_register to catch ATP transmission timeouts.
 */

STATIC void
st_comm_failure(cpudb_key_t key)
{
    LOG_WARN(BSL_LS_TKS_STKTASK,
             (BSL_META("STACK: communication timeout to " CPUDB_KEY_FMT_EOLN),
              CPUDB_KEY_DISP(key)));

    bcm_st_event_send(BCM_STE_COMM_FAILURE);
}

/****************************************************************
 *
 * Discovery related code
 */

/* Assumes LOCK held; called before each entry into discovery */

STATIC int
pre_discovery_prep(void)
{
    if (bcm_st_disc_db != CPUDB_REF_NULL) { /* Destroy disc db if present */
        CPUDB_TOPO_DESTROY(bcm_st_disc_db);
        bcm_st_disc_db = CPUDB_REF_NULL;
    }

    bcm_st_disc_db = cpudb_create();
    if (bcm_st_disc_db == CPUDB_REF_NULL) {
        LOG_ERROR(BSL_LS_TKS_STKTASK,
                  (BSL_META("ST ERR: Error creating DB.\n")));
        return -1;
    }
    st_config.base.dseq_num = st_ev_count;
    if (cpudb_local_base_info_set(bcm_st_disc_db,
                                  (cpudb_base_t *)&st_config.base) < 0) {
        LOG_ERROR(BSL_LS_TKS_STKTASK,
                  (BSL_META("ST ERR: Error creating local entry.\n")));
        CPUDB_TOPO_DESTROY(bcm_st_disc_db);
        bcm_st_disc_db = CPUDB_REF_NULL;
        return -1;
    }

    bcm_st_disc_db->old_db = bcm_st_cur_db;

    return 0;
}

/*
 * st_encap_check()
 *
 * Check stack port encapsulation. Note that we can't just check to
 * see if the port configuration says it's an external stack port (via
 * stack_ext), because the port could have changed mode without a link
 * state event.
 *
 * Record the encapsulation found in both the CPU database stack port
 * flags, and stktask stack ports flags, and add or remove the port
 * from the next hop database.
 *
 * Also, set or clear CPUDB_SPF_INACTIVE so the CPU database flags
 * field is still compatible with older software that doesn not
 * support CPUDB_SPF_ETHERNET directly.
 */

STATIC void
st_encap_check(cpudb_entry_t *local_entry)
{
    int i;
    cpudb_stk_port_t *stk_ports;

    stk_ports = local_entry->sp_info;

    /* Check for current link encapsulation mode of stack ports */
    for (i = 0; i < local_entry->base.num_stk_ports; i++) {
        int unit = local_entry->base.stk_ports[i].unit;
        int port = local_entry->base.stk_ports[i].port;
        bcm_port_encap_config_t mode;
        uint32 stk_mode;

        sal_memset(&mode, 0, sizeof(mode));
        if (BCM_SUCCESS(bcm_port_encap_config_get(unit, port, &mode)) &&
            BCM_SUCCESS(bcm_stk_mode_get(unit, &stk_mode))) {
            int idx = st_stk_port_find(unit, port, FALSE);

            LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                        (BSL_META_U(unit,
                        "ST: sp(%d,%d) idx=%d encap=%d stkmode=%d\n"),
                         unit,port,idx,mode.encap, stk_mode));

            if (mode.encap != BCM_PORT_ENCAP_IEEE || stk_mode == BCM_STK_SL) {
                stk_ports[i].flags &= ~(CPUDB_SPF_ETHERNET|CPUDB_SPF_INACTIVE);
                next_hop_port_add(unit, port, 0);
                if (idx >= 0) {
                    st_stk_port_flags[idx] &= ~ST_SPF_ETHERNET;
                }
            } else {
                stk_ports[i].flags |= (CPUDB_SPF_ETHERNET|CPUDB_SPF_INACTIVE);
                next_hop_port_remove(unit, port);
                if (idx >= 0) {
                    st_stk_port_flags[idx] |= ST_SPF_ETHERNET;
                }
            }
        } else {
            LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                        (BSL_META_U(unit,
                        "ST: sp(%d,%d): could not get encap/mode.\n"),
                         unit,port));
        }
    }
}

STATIC void
st_link_check(cpudb_entry_t *local_entry)
{
    int i;
    cpudb_stk_port_t *stk_ports;

    stk_ports = local_entry->sp_info;

    /* Check for current mode and link states of stack ports */
    for (i = 0; i < local_entry->base.num_stk_ports; i++) {
        int unit = local_entry->base.stk_ports[i].unit;
        int port = local_entry->base.stk_ports[i].port;

        if (STK_PORT_LINK_UP(i)) {
            if (!(stk_ports[i].flags & CPUDB_SPF_ETHERNET)) {
                next_hop_port_add(unit, port, 0);
            }
            stk_ports[i].flags &= ~CPUDB_SPF_NO_LINK;
        } else {
            stk_ports[i].flags |= CPUDB_SPF_NO_LINK;
            next_hop_port_remove(unit, port);
        }
    }
}

/*
 * Loop through discovery until success, failure or abort.
 * ST_LOCK is held on entry and exit of this function, but released
 * while discovery is running.
 */

STATIC int
st_disc_loop(void)
{
    int rv = BCM_E_FAIL;

    /* Allocate and/or initialize discovery DB; disable fabric trunking */
    if (pre_discovery_prep() < 0) { /* Error during preparation */
        return BCM_E_FAIL;
    }

    do { /* Repeat discovery while it returns "restart" */
        cpudb_clear(bcm_st_disc_db, TRUE);
        /* Get latest link info */
        st_encap_check(bcm_st_disc_db->local_entry); 
        st_link_check(bcm_st_disc_db->local_entry); 

        ST_UNLOCK;
        rv = st_config.st_disc_start(bcm_st_disc_db, st_config.st_master);
        ST_LOCK;

        ++st_ev_count; /* End of discovery is an event */
        if (rv == DISC_RESTART_NEW_SEQ) { /* Update sequence number */
            bcm_st_disc_db->local_entry->base.dseq_num = st_ev_count;
        }
    } while (((rv == DISC_RESTART_REQUEST) ||
              (rv == DISC_RESTART_NEW_SEQ)) &&
             (!(bcm_st_flags & BCM_STF_ABORT)));

    return rv;  /* ST_LOCK held */
}

#define BREAK_IF_ABORT(_state) if ((_state) & BCM_STF_ABORT) break

/* Thread to provide context to discovery */

STATIC void
st_disc_thread(void *cookie)
{
    int rv = BCM_E_NONE;

    COMPILER_REFERENCE(cookie);
    /* Synchronize with calling thread */
    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("ST [T=%u]: Disc thread started, id %d\n"),
                 sal_time_usecs(), PTR_TO_INT(sal_thread_self())));
    /* Signal about to sleep */
    ST_LOCK;
    bcm_st_flags |= BCM_STF_DISC_SLEEPING;
    ST_UNLOCK;

    /* NOTE: LOCK is held on exit from this loop */
    while (TRUE) {
        ST_DISC_SLEEP(sal_sem_FOREVER);

        ST_LOCK;
        bcm_st_flags &= ~BCM_STF_DISC_SLEEPING;
        BREAK_IF_ABORT(bcm_st_flags);
        if (st_state != BCM_STS_DISC) { /* Bad state */
            bcm_st_flags |= BCM_STF_DISC_SLEEPING;
            ST_UNLOCK;
            LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                        (BSL_META("ST: Disc thread awake, but state %s; continuing\n"),
                         bcm_st_state_strings[st_state]));
            continue;
        }

        /* disc_loop unlocks ST for discovery; returns locked */
        rv = st_disc_loop();

        BREAK_IF_ABORT(bcm_st_flags);
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: Disc ends: %d [T=%u]\n"),
                     rv, sal_time_usecs()));
        if (rv == BCM_E_NONE) {
            st_new_events |= BCM_STE_FLAG(BCM_STE_DISC_SUCCESS);
        } else {
            st_new_events |= BCM_STE_FLAG(BCM_STE_DISC_FAILURE);
        }
        ST_WAKE;
        /* Signal about to sleep */
        bcm_st_flags |= BCM_STF_DISC_SLEEPING;
        ST_UNLOCK;
    }

    /* LOCK IS HELD HERE */
    st_disc_tid = SAL_THREAD_ERROR;
    ST_UNLOCK;

    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("STACK: Disc thread exiting\n")));
    sal_thread_exit(rv);
}

/*
 * Function:
 *    st_linkscan_cb_thread_quit
 * Purpose:
 *      This function checks if the linkscan cb thread has gracefully
 *      shutdown itself. This is needed as when the Appl is shutdown the
 *      a global flag is set to indicate it. All stacking threads look for
 *      this flag periodically and shuts itself gracefully by marking the
 *      global variable of its thread ID to be SAL_THREAD_ERROR.
 *      Top level thread should only return back if all the threads it had
 *      spawned gracefully exits.
 * Parameters:
 *
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_FAIL
 *
 * Notes:
 */
STATIC int
st_linkscan_cb_thread_quit(int retry_max)
{
    int rv = BCM_E_NONE;
    int retries = 0;

    ST_LOCK;
    bcm_st_flags |= BCM_STF_ABORT;
    ST_UNLOCK;

    /*
     * Wake the link scan cb processing thread so that it
     * can see the abort flag and exit itself gracefully.
     */
    ST_LSCAN_WAKE;

    sal_thread_yield();
    while (st_lscan_tid != SAL_THREAD_ERROR && (retries++ < retry_max)) {
        sal_usleep(10000);
    }

    if (st_lscan_tid != SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: LinkScan CB thread won't quit\n")));
        rv = BCM_E_FAIL;
    }
    return rv;
}


#define BREAK_IF_NOT_RUNNING_OR_ABORT(_state)   \
    if (!((_state) & BCM_STF_RUNNING) || ((_state) & BCM_STF_ABORT)) break
/*
 * Function:
 *    st_linkscan_cb_thread
 * Purpose:
 *      This function is the starting point for the linkscan cb processing
 *      thread. It just waits on the linkscan cb list. If it finds an entry
 *      in it grabs it and processes the data.
 *      This function is the sole consumer of the linkscan cb list which makes
 *      it easy to manage the list as there is only one producer for the list.
 *      All through the infinite loop there are checks to see if the Appl
 *      was marked to abort or is now not running. In such case it will break
 *      the loop, mark the thread as INVALID to indicate that the thread is no
 *      longer working. Before it exits, it also marks the linkscan cb list as
 *      not ready as the sole consumer of it is going down.
 * Parameters:
 *      cookie: (IN) User data when the thread was created
 *
 * Returns:
 *
 * Notes:
 */
STATIC void
st_linkscan_cb_thread(void *cookie)
{
    int rv = BCM_E_NONE;
    int unit;
    bcm_port_t port;
    int link_status;
    sal_usecs_t timestamp;
    int idx;
    static const char *ll_status[] = {"down", "up", "failed", "fault"};

    COMPILER_REFERENCE(cookie);
    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("ST [T=%u]: LinkScan CB thread started, id %d\n"),
                 sal_time_usecs(), PTR_TO_INT(sal_thread_self())));

    /* NOTE: LOCK is held on exit from this loop */
    while (TRUE) {
        rv = _bcm_st_linkscan_list_pop(&unit, &port, &link_status, &timestamp);
        if(BCM_E_EMPTY == rv) {
            ST_LOCK;
            BREAK_IF_NOT_RUNNING_OR_ABORT(bcm_st_flags);
            ST_UNLOCK;
            ST_LSCAN_SLEEP(sal_sem_FOREVER);
            continue;
        }

        ST_LOCK;
        BREAK_IF_NOT_RUNNING_OR_ABORT(bcm_st_flags);

        idx = st_stk_port_find(unit, port, FALSE);
        if (idx < 0 || st_stk_port_flags[idx] & ST_SPF_ETHERNET) {
            /* Not a stack port */
            ST_UNLOCK;
            continue;
        }

        STK_PORT_LAST_LINK_SET(idx, link_status);
        stk_port_last_event_us[idx] = timestamp;

        /* If port was up, send link down now; */
        if ((link_status != BCM_PORT_LINK_STATUS_UP) &&
             STK_PORT_LINK_UP(idx)) {

            STK_PORT_CUR_LINK_SET(idx, FALSE);
            /*
             * Rapid Recovery
             *
             * If rapid recovery was successful, just return.
             * Note:  This method will NOT trigger any state
             *        transition for the Stack task.  This approach
             *        causes the minimal impact in the previous Stacking
             *        application.
             */
            if (BCM_SUCCESS(topo_board_rapid_recovery(bcm_st_cur_db,
                                                      unit, port))) {
                ST_UNLOCK;
                LOG_INFO(BSL_LS_TKS_STKTASK,
                            (BSL_META_U(unit,
                            "Rapid rcvry ok for (%d,%d)\n"),
                             unit, port));
                continue;
            }

            st_new_events |= BCM_STE_FLAG(BCM_STE_LINK_DOWN);

            /* Clear the NH TX source mod ID for the port if the reserved
               modid feature is not enabled. */
            if (!bcm_st_reserved_modid_enable_get()) {
                if ((rv = nh_tx_src_mod_port_set(unit, port, -1, -1)) < 0) {
                    LOG_WARN(BSL_LS_TKS_STKTASK,
                             (BSL_META_U(unit,
                             "ST: Link down, error clearing mod/port: %s\n"),
                              bcm_errmsg(rv)));
                }
            }
        }
        /* Check if we have to abort before waking up the event thread */
        BREAK_IF_NOT_RUNNING_OR_ABORT(bcm_st_flags);

        /* In either case, wake the thread so it will check for link debounce */
        ST_WAKE;
        ST_UNLOCK;

        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META_U(unit,
                    "ST: Link change idx %d, (%d, %d) %s\n"),
                     idx, unit, port,
                     ((link_status >= BCM_PORT_LINK_STATUS_DOWN &&
                       link_status <= BCM_PORT_LINK_STATUS_REMOTE_FAULT) ?
                               ll_status[link_status] : "Invalid")));
    }

    /* LOCK IS HELD HERE */
    st_lscan_tid = SAL_THREAD_ERROR;
    ST_UNLOCK;

    /* Disable the list enqueuing and deallocate the nodes if any */
    _bcm_st_linkscan_list_disable();

    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("STACK: Link Scan CB processing thread exiting\n")));
    sal_thread_exit(rv);
}
#undef BREAK_IF_NOT_RUNNING_OR_ABORT
#undef BREAK_IF_ABORT

/****************************************************************
 *
 * Main stack task API functions
 */

/* This local variable indicates if any stack link is found up
 * during initial check.  It is only used in st_disc_thread_started
 * to check if a kick-start is needed.
 */

static int st_init_link_up_found = FALSE;

/* Initialize the link state for the given stack port */

STATIC void
stk_port_link_state_init(int unit, bcm_port_t port, int sp)
{
    int rv;
    int link;

    /* Get this port's current link state */
    ST_LOCK;
    rv = bcm_port_link_status_get(unit, port, &link);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_TKS_STKTASK,
                  (BSL_META_U(unit,
                  "ST: activating stkport %d.%d link status failed: %s\n"),
                   unit, port, bcm_errmsg(rv)));
        link = 0;
    }

    STK_PORT_CUR_LINK_SET(sp, FALSE);
    STK_PORT_LAST_LINK_SET(sp, link);
    stk_port_last_event_us[sp] = sal_time_usecs();

    if (link) {
        st_init_link_up_found = TRUE;
    }
    ST_UNLOCK;
}

/* Initialize all stack port link states */

STATIC void
st_link_init(void)
{
    int i;
    int unit, port;
    int rv;

    /* Register for link changes on stack units */
    if (bcm_st_cfg_flags & BCM_STC_LINK_REGISTER) {
        /* Init the linkscan cb list before the handler is registered */
        _bcm_st_linkscan_list_enable();

        for (i = 0; i < st_num_stk_units; i++) {
            rv = bcm_linkscan_register(st_stk_units[i], st_linkscan_handler);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_TKS_STKTASK,
                          (BSL_META("ST: link register failed: %s\n"),
                           bcm_errmsg(rv)));
            }
        }
    }

    /* Check for current link states of stack ports */
    FOREACH_STK_PORT(i) {
        unit = st_config.base.stk_ports[i].unit;
        port = st_config.base.stk_ports[i].port;
        stk_port_link_state_init(unit, port, i);
    }
}

/* Program the stand alone topology for the board and set up link states */

STATIC int
local_board_setup(cpudb_key_t key)
{
    cpudb_ref_t local_ref = NULL;
    int rv;

    if ((local_ref = cpudb_create()) == NULL) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Could not create setup DB\n")));
        return BCM_E_MEMORY;
    }

    
    if ((rv = cpudb_local_base_info_set(local_ref, &st_config.base)) < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Setup could not set base info: %s\n"),
                  bcm_errmsg(rv)));
        return rv;
    }
    
    st_encap_check(local_ref->local_entry); 

    if ((rv = topology_mod_ids_assign(local_ref)) < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Could not setup modids: %s\n"),
                  bcm_errmsg(rv)));
        return rv;
    }
    
    if ((rv = topo_board_setup(local_ref)) < 0) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Could not setup local topology: %s\n"),
                  bcm_errmsg(rv)));
        return rv;
    }

    CPUDB_TOPO_DESTROY(local_ref);

    return BCM_E_NONE;
}


STATIC int
_config_load(bcm_st_config_t *config)
{
    if (bcm_st_flags & BCM_STF_RUNNING) {
        return BCM_E_BUSY;
    }

    /* config may be NULL if already loaded a configuration */
    if (config == NULL) {
        if (!(bcm_st_flags & BCM_STF_CONFIG_LOADED)) {
            return BCM_E_PARAM;
        } else {
            return BCM_E_NONE;
        }
    }

    if ((config->st_disc_start == NULL) || (config->st_disc_abort == NULL)) {
        return BCM_E_PARAM;
    }

    sal_memcpy((void*)&st_config, config, sizeof(bcm_st_config_t));
    /* Default topo/attach/transition settings */
    if (st_config.st_topo == NULL) {
        st_config.st_topo = bcm_stack_topo_update;
    }
    if (st_config.st_attach == NULL) {
        st_config.st_attach = bcm_stack_attach_update;
    }
    if (st_config.st_transition == NULL) {
        st_config.st_transition = bcm_st_transition;

    }

    ST_LOCK;
    bcm_st_flags |= BCM_STF_CONFIG_LOADED;
    ST_UNLOCK;
    
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_st_config_load
 * Purpose:
 *      Initialize stack task without starting it.
 * Parameters:
 *      config     - The configuration structure to use
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_st_config_load(bcm_st_config_t *config)
{
    int rv;

    if (!ST_INIT_DONE) {
        rv = st_init();
        if (rv < 0) {
            return rv;
        }
    }

    ST_LOCK;
    rv = _config_load(config);
    ST_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Wait for disc thread to indicate it's there and sleeping; 2 seconds.
 * See if we need to kick-start due to no links up.
 */

STATIC int
st_disc_thread_started(void)
{
    int count = 0;

    while (!(bcm_st_flags & (BCM_STF_DISC_SLEEPING|BCM_STF_ABORT))) {
        sal_thread_yield();
        sal_usleep(10000);
        if (++count > 400) {
            LOG_WARN(BSL_LS_TKS_STKTASK,
                     (BSL_META("ST: Discovery thread not alive on startup\n")));
            return BCM_E_TIMEOUT;
        }
    }

    /* Check for all links down and in ready state; send event if so */
    if ((st_state == BCM_STS_READY) && !st_init_link_up_found &&
        (!(bcm_st_flags & BCM_STF_ABORT))) {
        bcm_st_event_send(BCM_STE_LINK_DOWN);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_st_start
 * Purpose:
 *      Start the stack manager task
 * Parameters:
 *      config           - Config structure; see stktask.h
 *      enabled          - Start discovery immediately?
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Requires a thread context; does not start its own thread.
 *      However, does launch thread for discovery.
 */

int
bcm_st_start(bcm_st_config_t *config, int enable)
{
    int rv;

    if (!ST_INIT_DONE && ((rv = st_init()) < 0)) {
        return rv;
    }

    if (st_disc_tid != SAL_THREAD_ERROR) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: Discovery thread is running\n")));
        return BCM_E_FAIL;
    }

    if (!atp_running() && !(bcm_st_cfg_flags & BCM_STC_START_ATP)) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("ST: ATP is not running\n")));
    }

    ST_LOCK;
    if ((rv = _config_load(config)) < 0) {
        ST_UNLOCK;
        return rv;
    }
    bcm_st_flags |= BCM_STF_RUNNING;
    local_board_setup(st_config.base.key);

    /* Init flags, state and transition time */
    bcm_st_flags &= ~(BCM_STF_ABORT | BCM_STF_DISC_SLEEPING);

    if (enable) {
        st_state = BCM_STS_READY;
    } else {
        st_state = BCM_STS_BLOCKED;
    }
    st_transition_time = sal_time_usecs();

    /* Generate the list of stack units; setup link and communications */
    rv = stk_units_gen();
    if (BCM_FAILURE(rv)) {
        bcm_st_flags &= ~BCM_STF_RUNNING;
        ST_UNLOCK;   
        return rv;
    }

    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("Stack task started [T=%u]: %s with %d stk unit%s.\n"),
                 st_transition_time, enable ? "ready" : "blocked",
                 st_num_stk_units, st_num_stk_units != 1 ? "s" : ""));

    ST_UNLOCK;
    st_link_init();
    st_comm_setup();
    ST_LOCK;

    /* Start discovery thread */
    st_disc_tid = sal_thread_create("bcmDISC",
                                    bcm_st_disc_stk_size,
                                    bcm_st_disc_priority,
                                    st_disc_thread,
                                    (void *)NULL);

    if (st_disc_tid == SAL_THREAD_ERROR) {
        bcm_st_flags &= ~BCM_STF_RUNNING;
        ST_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: Could not create discovery thread\n")));
        st_comm_shutdown();
        return BCM_E_FAIL;
    }

    ST_UNLOCK;

    rv = st_disc_thread_started();
    if (BCM_FAILURE(rv)) {
        ST_LOCK;
        bcm_st_flags &= ~BCM_STF_RUNNING;
        ST_UNLOCK;
        st_comm_shutdown();
        return rv;
    }

    /* Start the link scan callback response thread*/
    ST_LOCK;
    st_lscan_tid = sal_thread_create("bcmStkLScan",
                                    SAL_THREAD_STKSZ,
                                    BCM_ST_LSCAN_PRIORITY_DEFAULT,
                                    st_linkscan_cb_thread,
                                    (void *)NULL);

    if (st_lscan_tid == SAL_THREAD_ERROR) {
        bcm_st_flags &= ~BCM_STF_RUNNING;
        ST_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                    (BSL_META("ST: Could not create linkscan cb thread\n")));
        st_comm_shutdown();
        /* End discovery thread */
        rv = discovery_quit(BCM_ST_DISC_STOP_RETRIES_MAX);
        return rv;
    }

    ST_UNLOCK;

    /* Process events until forced to exit */
    st_event_loop();

    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("ST: Aborting [T=%u]\n"),
                 sal_time_usecs()));
    st_comm_shutdown();

    ST_LOCK;
    bcm_st_flags &= ~BCM_STF_RUNNING;
    ST_UNLOCK;

    /* End discovery thread */
    rv = discovery_quit(BCM_ST_DISC_STOP_RETRIES_MAX);

    if(BCM_SUCCESS(rv)) {
        /* If the discovery thread has exited successfully
         * then retrieve the status of linkscan thread exit
         */
        rv = st_linkscan_cb_thread_quit(BCM_ST_LSCAN_CB_QUIT_RETRIES_MAX);
    } else {
        /* If the discovery thread has not exited successfully then still
         * try to exit the linkscan thread. Do not bother to collect its
         * status as we are going to return failure due to unsuccessfull
         * exit of discovery thread.
         */
        st_linkscan_cb_thread_quit(BCM_ST_LSCAN_CB_QUIT_RETRIES_MAX);
    }
    return rv;
}


/*
 * Function:
 *      bcm_st_stop
 * Purpose:
 *      Stop the stack manager task
 * Parameters:
 *      timeout_us     - How long to wait for exit before returning error
 * Returns:
 *      BCM_E_FAIL if task does not appear stopped after time out.
 */

int
bcm_st_stop(int timeout_us)
{
    int retries;
    int i;

    if (!(bcm_st_flags & BCM_STF_RUNNING)) {
        return BCM_E_NONE;
    }

    ST_LOCK;
    bcm_st_flags |= BCM_STF_ABORT;
    ST_UNLOCK;

    ST_WAKE;
    sal_thread_yield();
    if (timeout_us > 0) {
        retries = timeout_us / 10000 + 1;
        for (i = 0; i < retries; i++) {
            if (!(bcm_st_flags & BCM_STF_RUNNING)) {
                break;
            }
            sal_usleep(10000);
        }
    }

    return (bcm_st_flags & BCM_STF_RUNNING) ? BCM_E_FAIL : BCM_E_NONE;
}

/*
 * Function:
 *      bcm_st_base_update
 * Purpose:
 *      Update the CPUDB base information for the system
 * Parameters:
 *      base      - Pointer to structure w/ info to update
 *      restart   - Should restart event be sent
 *                  NOTE: This parameter is ignored. Discovery
 *                  will restart if a stack port is either
 *                  already up, or transitions from down to up
 *                  through the normal stack port link state checking.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Copies the information to a local copy.  Checks if discovery
 *      is currently under way in which case it returns _E_BUSY.
 */

int
bcm_st_base_update(cpudb_base_t *base, int restart)
{
    int rv = BCM_E_NONE;
    int i, unit;
    bcm_port_t port;

    if (st_lock == NULL) {
        return BCM_E_INIT;
    }

    ST_LOCK;
    sal_memcpy((void *)&st_config.base, base, sizeof(*base));

    /* Check for current link states of stack ports from base config */
    FOREACH_STK_PORT(i) {
        unit = st_config.base.stk_ports[i].unit;
        port = st_config.base.stk_ports[i].port;
        stk_port_link_state_init(unit, port, i);
    }

    if (bcm_st_disc_db) {
        bcm_st_disc_db->local_entry->base.master_pri = st_config.base.master_pri;
    }
    if (bcm_st_cur_db) {
        bcm_st_cur_db->local_entry->base.master_pri = st_config.base.master_pri;
    }
    
    ST_UNLOCK;

    COMPILER_REFERENCE(restart);

    return rv;
}


/*
 * Function:
 *      bcm_st_event_send
 * Purpose:
 *      Send an event to the stack task
 * Parameters:
 *      event        - Event type.  See stktask.h
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Although any event can be sent through this routine,
 *      it should not be used for link events or discovery packet
 *      events.
 */

int
bcm_st_event_send(bcm_st_event_t event)
{
    if (!ST_INIT_DONE) {
        return BCM_E_INIT;
    }

    if (!BCM_STE_VALID(event)) {
        return BCM_E_PARAM;
    }

    ST_LOCK;
    st_new_events |= BCM_STE_FLAG(event);
    ST_UNLOCK;
    ST_WAKE;

    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("ST: sending event %s [T=%u]\n"),
                 bcm_st_event_strings[event],
                 sal_time_usecs()));

    return BCM_E_NONE;
}

/****************************************************************
 *
 * Configuration API
 */

/*
 * Function:
 *      bcm_st_timeout_set/get
 * Purpose:
 *      Access the timeout associated with a state
 * Parameters:
 *      state     - Which timeout to update
 *      to        - Timeout in microseconds
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      A timeout of 0 disables the timer for that state.
 */

int
bcm_st_timeout_set(bcm_st_state_t state, sal_usecs_t to)
{
    int rv = BCM_E_PARAM;

    if (BCM_STS_VALID(state)) {
        state_timeouts[state] = to;
        rv = BCM_E_NONE;
    }

    return rv;
}

int
bcm_st_timeout_get(bcm_st_state_t state, sal_usecs_t *to)
{
    int rv = BCM_E_PARAM;

    if (BCM_STS_VALID(state)) {
        if (to != NULL) {
            *to = state_timeouts[state];
            rv = BCM_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_st_stk_port_enable_set/get
 * Purpose:
 *      Set/get status of stack ports
 * Parameters:
 *      unit.port     - Stack port indication
 *      enable        - (OUT for get) is port enabled
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      By default, all stack ports are enabled when config is loaded.
 */

int
bcm_st_stk_port_enable_set(int unit, int port, int enable)
{
    int sp;
    int rv = BCM_E_NOT_FOUND;

    ST_LOCK;
    sp = st_stk_port_find(unit, port, TRUE);
    if (sp >= 0) {
        if (enable) {
            st_stk_port_flags[sp] &= ~ST_SPF_DISABLED;
            stk_port_link_state_init(unit, port, sp);
        } else { /* Mark port as disabled */
            st_stk_port_flags[sp] |= ST_SPF_DISABLED;
        }
        rv = BCM_E_NONE;
    }
    ST_UNLOCK;
    return rv;
}

int
bcm_st_stk_port_enable_get(int unit, int port, int *enable)
{
    int sp;

    if (enable == NULL) {
        return BCM_E_PARAM;
    }

    sp = st_stk_port_find(unit, port, TRUE);
    if (sp < 0) {
        return BCM_E_NOT_FOUND;
    }

    if (STK_PORT_DISABLED(sp)) {
        *enable = FALSE;
    } else {
        *enable = TRUE;
    }

    return BCM_E_NONE;
}


int
bcm_st_reserved_modid_enable_set(int value)
{
    reserved_modid_enable = value;

    return BCM_E_NONE;
}

int
bcm_st_reserved_modid_enable_get(void)
{
    return reserved_modid_enable;
}


/****************************************************************
 *
 * Default stack task transition function
 */

/*
 * Stack task event transition handler (default version)
 *
 * This callback occurs as the last step before changing the st_state
 * variable on a state transition.  It may return an error (this
 * version doesn't) which will force the state to blocked (without
 * an additional callback).
 *
 * The application may override this function by specifying
 * st_config.st_transition.
 */

int
bcm_st_transition(bcm_st_state_t from,
                  bcm_st_event_t event,
                  bcm_st_state_t to,
                  cpudb_ref_t disc_db,
                  cpudb_ref_t cur_db)
{
    if (event == BCM_STE_TIMEOUT && from != BCM_STS_BLOCKED) {
        LOG_WARN(BSL_LS_TKS_STKTASK,
                 (BSL_META("TKS ST TIMEOUT in state %s\n"),
                  bcm_st_state_strings[from]));
    }
    LOG_VERBOSE(BSL_LS_TKS_STKTASK,
                (BSL_META("TKS ST transition: disc_db %p. cur_db %p\n"),
                 disc_db, cur_db));

    /* Always clear topo expect moving out of TOPO...unless into DISC too */
    if (from == BCM_STS_TOPO) {
        topo_pkt_expect_set(FALSE);
    }

    switch (to) {
    case BCM_STS_DISC:
        topo_pkt_expect_set(TRUE);
        break;
    case BCM_STS_BLOCKED:
        topo_pkt_expect_set(FALSE);
        if (bcm_st_cfg_flags & BCM_STC_AUTO_B2R) {
            /* Auto transition unblocked from blocked */
            bcm_st_event_send(BCM_STE_UNBLOCK);
        }
        break;
    case BCM_STS_TOPO:
        /*
         * During this state, the discovery database contains the most
         * update list of known CPUs.  However, information on all
         * CPUs are at its initial state.
         *
         * In order to reduce the amount of disruption while the
         * stack task completes the discovery/topology cycle,
         * previous information on still existing CPUs is retrieved
         * and used during the CPU update.
         */
        if (disc_db != NULL) {
            cpudb_ref_t    transition_db;
            cpudb_entry_t  *entry, *cur_entry;
        
            transition_db = cpudb_copy(disc_db);
            if (cur_db != NULL) {
                CPUDB_FOREACH_ENTRY(transition_db, entry) {
                    /* Replace previous information on existing entries */
                    CPUDB_KEY_SEARCH(cur_db, entry->base.key, cur_entry);
                    if (cur_entry != NULL) {
                        cpudb_entry_copy(entry, cur_entry);
                    }
                }
            }
            atp_db_update(transition_db);
            cpudb_destroy(transition_db);
        }
        break;
    case BCM_STS_ATTACH:
        atp_db_update(disc_db);
        break;
    default:
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_st_current_db_get
 * Purpose:
 *      Get a copy of the stktask current CPU database
 * Parameters:
 *      none
 * Returns:
 *      CPU database
 */

cpudb_ref_t bcm_st_current_db_get(void)
{
    cpudb_ref_t dbref = NULL;

    if (ST_INIT_DONE) {
        ST_LOCK;
        dbref = cpudb_copy(bcm_st_cur_db);
        ST_UNLOCK;
    }

    return dbref;
}

/*
 * Function:
 *      bcm_st_discovery_db_get
 * Purpose:
 *      Get a copy of the stktask discovery CPU database
 * Parameters:
 *      none
 * Returns:
 *      CPU database
 */

cpudb_ref_t bcm_st_discovery_db_get(void)
{
    cpudb_ref_t dbref = NULL;

    if (ST_INIT_DONE) {
        ST_LOCK;
        dbref = cpudb_copy(bcm_st_disc_db);
        ST_UNLOCK;
    }
    
    return dbref;
}

/*
 * Function:
 *     __bcm_st_linkscan_list_clean
 * Purpose:
 *      Iterate through all the nodes in the list and deallocate
 *      the memory one by one. When the function returns the link
 *      scan cb list should be empty.
 * Parameters:
 *
 * Returns:
 *
 * Notes:
 *      This function assumes that the caller would acquire the mutex
 *      before calling it and will release the mutex after the call.
 *      Hence no synchronization is taken care within it.
 */
STATIC
void __bcm_st_linkscan_list_clean(void) {
    bcm_st_lscan_rsp_node_t *node = NULL;
    /* Iterate through all the nodes and free its memnory*/
    while (st_linkscan_list.head) {
        node = st_linkscan_list.head;
        st_linkscan_list.head = st_linkscan_list.head->nxt;
        sal_free(node);
    }
    /* Mark the tail pointer to NULL as the list is empty now */
    st_linkscan_list.tail = NULL;
}

/*
 * Function:
 *     _bcm_st_linkscan_list_enable
 * Purpose:
 *      This function restores the linkscan cb list to its original
 *      status i.e. empty list. At first it marks the list as not ready
 *      to inform other threads that the list can not be used currently.
 *      Then it empties the list gracefully and then mark it ready to be
 *      used.
 * Parameters:
 *
 * Returns:
 *
 * Notes:
 *      This function does acquire and release the linkscan mutex.
 *      Hence none of the other threads can get access to the list while
 *      its being prepared for usage.
 */
STATIC
void _bcm_st_linkscan_list_enable(void) {

    ST_LSCAN_LOCK;
    /* Mark the list as not usable */
    st_linkscan_list.ready = 0;
    /* Delete the list gracefully */
    __bcm_st_linkscan_list_clean();
    /* Mark the list as ready to be used */
    st_linkscan_list.ready = 1;
    ST_LSCAN_UNLOCK;
}

/*
 * Function:
 *     _bcm_st_linkscan_list_disable
 * Purpose:
 *      This function restores the linkscan cb list to its original
 *      status i.e. empty list and marks it non-ready so that producer
 *      for the link list would not be able to enqueue nodes to it.
 * Parameters:
 *
 * Returns:
 *
 * Notes:
 *      This function does acquire and release the linkscan mutex.
 *      Hence none of the other threads can get access to the list while
 *      its being purged.
 *      The user of the linkscan cb list should check for the ready field
 *      before any operation can be performed on it.
 */
STATIC
void _bcm_st_linkscan_list_disable(void) {
    ST_LSCAN_LOCK;
    /* Mark the list as not usable */
    st_linkscan_list.ready = 0;
    /* Delete the list gracefully */
    __bcm_st_linkscan_list_clean();
    ST_LSCAN_UNLOCK;
}

/*
 * Function:
 *     _bcm_st_linkscan_list_push
 * Purpose:
 *      This function creates a node with the user given data and inserts
 *      it at the end of the list. If the list is marked as non-ready
 *      then it skips insertion and returns with SUCCESS status. Hence the
 *      caller can not know if the list enqueue  happened ot not.
 * Parameters:
 *      unit            - (IN) Device Unit #
 *      port            - (IN) BCM local port #
 *      link_status     - (IN) Link status from the linkscan thread.
 *      snapshot_time   - (IN) Time in usecs when the link_status was recorded
 *
 * Returns:
 *      BCM_E_MEMORY
 *      BCM_E_NONE
 *
 * Notes:
 *      Caller is returned with SUCCESS status when the list is not ready
 *      to be used although the data was not pushed to it. This behavior
 *      is by design as there is only one producer at this time - the
 *      linkscan cb handler. The cb handler should not delay the return
 *      to the linkscan thread and further if the list is not ready that
 *      would mean that the stk task has been stopped and there is no one
 *      to process the list contents.
 */
STATIC
int _bcm_st_linkscan_list_push(
    int unit, bcm_port_t port, int link_status, sal_usecs_t snapshot_time) {

    uint8 node_pushed = 0;
    bcm_st_lscan_rsp_node_t *rsp_node = NULL;

    /* Allocate memory for the node */
    rsp_node = (bcm_st_lscan_rsp_node_t *)
        sal_alloc(sizeof(bcm_st_lscan_rsp_node_t), "CB_Node");

    if (NULL == rsp_node) {
        return BCM_E_MEMORY;
    }

    /* Populate the node with the data */
    rsp_node->unit = unit;
    rsp_node->port = port;
    rsp_node->link_evt_time = snapshot_time;
    rsp_node->link_status = link_status;
    rsp_node->nxt = NULL;

    ST_LSCAN_LOCK;
    /* Skip the insertion if the list is not ready */
    if (st_linkscan_list.ready) {
        /* Insert the node at the end of the list */
        if (st_linkscan_list.tail) {
            st_linkscan_list.tail->nxt = rsp_node;
        } else {
            /* If this is the 1st node then head pointer should point to it */
            st_linkscan_list.head = rsp_node;
        }
        /* Update the tail pointer to point to the last node */
        st_linkscan_list.tail = rsp_node;
        node_pushed = 1;
    }
    ST_LSCAN_UNLOCK;

    /* If the node could not be inserted then release it to avoid mem leak */
    if (0 == node_pushed) {
        sal_free(rsp_node);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_st_linkscan_list_pop
 * Purpose:
 *      This function retrieves a node and fills in the user given buffers.
 *      It deletes the node from the list. The Node retrieved is the first
 *      Node pointed at by the Head pointer.
 * Parameters:
 *      unit            - (OUT) Device Unit #
 *      port            - (OUT) BCM local port #
 *      link_status     - (OUT) Link status from the linkscan thread.
 *      snapshot_time   - (OUT) Time in usecs when the link_status was recorded
 *
 * Returns:
 *      BCM_E_EMPTY
 *      BCM_E_NONE
 *
 * Notes:
 *      The function assumes that the given buffers are valid addresses.
 *      The values returned in the given buffers would make sense only
 */
STATIC
int _bcm_st_linkscan_list_pop(
    int *unit, bcm_port_t *port, int *link_status, sal_usecs_t *snapshot_time) {

    bcm_st_lscan_rsp_node_t *rsp_node;

    ST_LSCAN_LOCK;
    /* Retrieve the first node from the list*/
    rsp_node = st_linkscan_list.head;
    /* Since it is a pop operation delink the node from the list */
    if (st_linkscan_list.head) {
        if (NULL == st_linkscan_list.head->nxt) {
            /* If the list had only one Node then Tail should point to NULL */
            st_linkscan_list.tail = st_linkscan_list.head->nxt;
        }
        /*
         * Move the Head to the next node. If list had only
         * one Node then now The HEAD would point to NULL too.
         */
        st_linkscan_list.head = st_linkscan_list.head->nxt;
    }
    ST_LSCAN_UNLOCK;

    if (rsp_node) {
        /* If the Node was pop'd then fill in the user given parameters */
        *unit = rsp_node->unit;
        *port = rsp_node->port;
        *link_status = rsp_node->link_status;
        *snapshot_time = rsp_node->link_evt_time;

        /* Deallocate the memory for the node */
        sal_free(rsp_node);
        return BCM_E_NONE;
    } else {
        /* List was empty to begin with */
        return BCM_E_EMPTY;
    }
}
