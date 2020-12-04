/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        disc.c
 * Purpose:     Broadcom Discovery Algorithm
 * Requires:
 *
 * The discovery procedure is run using next hop transmit only.
 * It builds a local database with static variables, so it is
 * not re-entrant.
 *
 * This version of discovery runs in the callers thread of
 * execution.  No async option is provided.
 *
 * We assume that a full probe packet can be sent in a packet
 * that is returned by RX.  Each probe entry takes less than 20 bytes,
 * so a 1K buffer can hold 50 entries.
 */

#include <shared/bsl.h>

#include <sdk_config.h>
#include <assert.h>

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/time.h>

#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <appl/discover/disc.h>
#include <appl/cputrans/next_hop.h>
#include <appl/cputrans/cputrans.h>
#include <appl/cputrans/atp.h>

#include <appl/cpudb/cpudb.h>

#include "disc_int.h"

#ifndef DISC_COS_DEFAULT
#define DISC_COS_DEFAULT 0
#endif

#ifndef DISC_VLAN_DEFAULT
#define DISC_VLAN_DEFAULT 1
#endif

#ifndef DISC_THREAD_STACK
#define DISC_THREAD_STACK        SAL_THREAD_STKSZ
#endif

#ifndef DISC_THREAD_PRIORITY
#define DISC_THREAD_PRIORITY     100
#endif

#define DISC_TASK (_disc_tid != SAL_THREAD_ERROR)

static volatile sal_thread_t     _disc_tid = SAL_THREAD_ERROR;

typedef struct disc_callback_handler_s {
    disc_cb_t callback;
    void *user_data;
} disc_callback_handler_t;

typedef enum disc_task_cmd_e {
    DISC_TASK_CMD_UNKNOWN,
    DISC_TASK_CMD_RUN,
    DISC_TASK_CMD_STOP
} disc_task_cmd_t;

typedef enum disc_status_e {
    DISC_STATUS_INACTIVE,
    DISC_STATUS_SYNC_RUNNING,
    DISC_STATUS_TASK_RUNNING,
    DISC_STATUS_TASK_IDLE
} disc_status_t;

#define DISC_TASK_SLEEP sal_sem_take(disc_task_sem, sal_sem_FOREVER)
#define DISC_TASK_WAKE sal_sem_give(disc_task_sem)


static int disc_cos = DISC_COS_DEFAULT;
static int disc_vlan = DISC_VLAN_DEFAULT;

/* Locks and synchronization,
 *     disc_lock               Mutual exclusion for queues
 *     disc_sem                Signal to main processing loop
 * Macros:
 *     DISC_LOCK               Take mutex
 *     DISC_UNLOCK             Give mutex
 *     INIT_DONE               See if discovery module init is complete
 *     DISC_INIT_CHECK         if not initialized, attempt to init system.
 */

/* Thread and synchronization */
static sal_mutex_t              disc_lock;
static sal_sem_t                disc_sem;

static int disc_version  = DISCOVERY_VERSION_DEFAULT;
static int disc_fallback = TRUE;  /* Indicates version auto-fallback */

#define DISC_LOCK       sal_mutex_take(disc_lock, sal_mutex_FOREVER)
#define DISC_UNLOCK     sal_mutex_give(disc_lock)
#define DISC_SLEEP(_t)  sal_sem_take(disc_sem, _t)
#define DISC_WAKE       sal_sem_give(disc_sem)

#define INIT_DONE       (disc_lock != NULL)
#define DISC_INIT_CHECK if (!INIT_DONE) BCM_IF_ERROR_RETURN(_disc_init())

/****************************************************************
 *
 * Local discovery configuration and active information
 *      disc_trans_ptr        Low level transport driver
 *      disc_m_elect          Master election function
 *      disc_timeout_us       Overall timeout
 *      disc_cfg_to_us        Set ports that have not received
 *                            probe packets as inactive
 *      disc_retrx_us         Retransmit timeout
 *      disc_retrx_min_us     Min time between pkts out
 *
 * Macros
 *      LOCAL_KEY(db_ref)     Local CPU entry's DB key
 *      LOCAL_MAC(db_ref)     Local CPU entry's MAC address
 *      LOCAL_FLAGS(db_ref)   Local CPU entry's flags
 *      STKP_UNIT(db_ref, i)  Local CPU entry's unit for stack port [i]
 *      STKP_PORT(db_ref, i)  Local CPU entry's port for stack port [i]
 *      STK_PORT(db_ref, i)   Local CPU entry's stack port [i] (flags, etc)
 *      STK_FLAGS(db_ref, i)  Flags for local stack port i
 */

STATIC int disc_m_elect_default(cpudb_ref_t db_ref, void *user_data);

/* The database reference to use */
static bcm_trans_ptr_t *disc_trans_ptr = TRANS_PTR_DEFAULT;

static volatile int disc_timeout_us    = DISC_TIMEOUT_DEFAULT;
static volatile int disc_cfg_to_us     = DISC_CFG_TO_DEFAULT;
static volatile int disc_retrx_us      = DISC_RETRX_DEFAULT;
int disc_retrx_min_us                  = DISC_RETRX_MIN_DEFAULT;
static disc_election_cb_t disc_m_elect = disc_m_elect_default;
static disc_start_election_cb_t disc_start_elect = NULL;
static void *disc_m_user_data;

#define LOCAL_KEY(db_ref)       (db_ref->local_entry->base.key)
#define LOCAL_MAC(db_ref)       (db_ref->local_entry->base.mac)
#define LOCAL_FLAGS(db_ref)     (db_ref->local_entry->flags)

#define STKP_UNIT(db_ref, i)    (db_ref->local_entry->base.stk_ports[i].unit)
#define STKP_PORT(db_ref, i)    (db_ref->local_entry->base.stk_ports[i].port)

/* Stack port info other than unit/port */
#define STK_PORT(db_ref, i)     (db_ref->local_entry->sp_info[i])
#define STK_FLAGS(db_ref, i)    STK_PORT(db_ref, i).flags

#define DATA_SALLOC(size)       atp_tx_data_alloc(size)
#define DATA_SFREE(ptr)         atp_tx_data_free(ptr)

#define DISC_TTL_FULL_DUPLEX 2  /* full duplex TTL */

/*
 * If disc_ttl is enabled (> 0), then TTL is sent and examined in probe
 * packets.
 *
 * The TTL increased each time "send init info" is called up to
 * disc_ttl_max.
 */

uint8 disc_ttl_min = DISC_TTL_MIN_DEFAULT;
uint8 disc_ttl_max = DISC_TTL_MAX_DEFAULT;
uint8 disc_ttl;

/* Discovery packet rate limiting

  Large numbers of stack ports can easily overwhelm the Next Hop
  protocol, which has a limited capacity to detect stale packets. Rate
  limiting the transmission of probe and route packets helps keep the
  Next Hop protocol working correctly.

  Define DISC_PPS <= 0 to disable discovery packet rate limiting.

*/

#ifndef DISC_PPS
#define DISC_PPS 100
#endif

#if DISC_PPS > 0
static int disc_tx_init;
static sal_usecs_t disc_tx_prev;
#endif

/****************************************************************
 *
 * Discovery packet coordination
 *
 * Packet structure to maintain info about packet until serviced
 * in discovery thread.
 *
 * Probe and routing packet queues.
 */

/*
 * This is used for queuing probe and routing packets
 * The queues are very simple.  We either add one entry, or remove
 * all entries in the queue at once; so it can be an array with
 * a length.
 */

typedef struct disc_pkt_s disc_pkt_t;
struct disc_pkt_s {
    cpudb_key_t src_key;
    uint8* pkt_buf;
    uint8* data;
    int len;
    int rx_unit, rx_port;
};

#ifndef DISC_PKTS_MAX
#define DISC_PKTS_MAX 32 /* Max packets allowed queued at a time */
#endif


static disc_pkt_t          probe_pkt_queue[DISC_PKTS_MAX];
static volatile int        probe_pkt_count;

static disc_pkt_t          routing_pkt_queue[DISC_PKTS_MAX];
static volatile int        routing_pkt_count;

/* Output packets */
static uint8 *route_pkt_buf[ROUTE_PKTS_MAX];
static int route_pkt_len[ROUTE_PKTS_MAX];

/*
 * To handle ports that have link, but on which there is no
 * activity, a check is made of whether a probe packet has been
 * seen on the port since the beginning of discovery.  If none
 * has been seen for a given amount of time, the port will be
 * marked inactive and ignored for the rest of the processing.
 * If discovery then completes successfully, those ports will be
 * marked as "no-link".
 *
 * probe_pkt_seen is a bit-map indexed by local stack port index.
 */
static uint32 probe_pkt_seen[_SHR_BITDCLSIZE(CPUDB_STK_PORTS_MAX)];

#define PP_SEEN_GET(n)      SHR_BITGET(probe_pkt_seen, n)
#define PP_SEEN_SET(n)      SHR_BITSET(probe_pkt_seen, n)
#define PP_SEEN_CLR(n)      SHR_BITCLR(probe_pkt_seen, n)
#define PP_SEEN_ZERO        sal_memset(probe_pkt_seen, 0, \
                                        SHR_BITALLOCSIZE(CPUDB_STK_PORTS_MAX))

/****************************************************************
 *
 * State
 *     disc_flags
 *         Divided into transient and non-transient flags.
 *         Transient flags are reset when starting.
 *             SETUP_DONE          disc_setup has been called.
 *             DISC_RUNNING        Discovery thread is running.
 *             DISC_ABORT          Terminate discovery
 *             ABORT_ACK           Respond that ACK seen
 *             DISC_ERROR          An error occurred, terminating discovery
 *             DISC_TIMEOUT        Discovery timed out (implies error)
 *             DB_UPDATED          Local DB has been changed.
 *             INFO_IN             Enough info is in to send routing pkts
 *                                 can be sent
 *             PROBE_SEND          Force probe packets to be sent the
 *                                 next cycle; used by restart.
 *             RESTART             Request controlling application
 *                                 restart discovery with same sequence
 *                                 number
 *             RESTART_DSEQ_NUM    Request controlling application
 *                                 restart discovery with a new sequence
 *                                 number
 *             TX_KNOWN            Some TX is known; this indicates
 *                                 we can send out routing pkts
 *
 *     disc_start_time         When did discovery start
 *     disc_tx_pkt_err         Counter:  TX errors
 *     disc_rx_pkt_err         Counter:  RX processing errors
 *     disc_alloc_fail         Counter:  Memory (external) resource error
 *     disc_resource_err       Counter:  Internal resource error
 *     disc_external_err       Counter:  External resource error
 *     disc_internal_err       Counter:  Internal error
 */

volatile uint32          disc_flags;
#define DF_SETUP_DONE           0x1
#define DF_DISC_RUNNING         0x2
#define DF_DISC_ABORT           0x4
#define DF_ABORT_ACK            0x8   /* ACK that abort occurred */
#define DF_DISC_ERROR           0x10
#define DF_DISC_TIMEOUT         0x20
#define DF_DB_UPDATED           0x40
#define DF_INFO_IN              0x80
#define DF_DISC_SUCCESS         0x100
#define DF_TX_KNOWN             0x200
#define DF_IGNORE_PACKETS       0x400
#define DF_SEND_CFG_PKT         0x800
#define DF_CFG_PKT_SENT         0x1000

/* On abort, return value to use is saved here */
static volatile int             disc_abort_rv;

#define DISC_EXIT_FLAGS \
    (DF_DISC_ERROR | DF_DISC_ABORT | DF_DISC_TIMEOUT | DF_DISC_SUCCESS)

#define DISC_EXIT (disc_flags & DISC_EXIT_FLAGS)

/* When did discovery start; will restart if timeout_set is called */
static volatile sal_usecs_t disc_start_time;
static volatile sal_usecs_t disc_last_probe_time;
static volatile sal_usecs_t disc_last_route_time;

#define CUR_DSEQ_NUM(db_ref) (db_ref->local_entry->base.dseq_num)

/* Some counters */
int                        disc_tx_pkt_err    = 0;
int                        disc_rx_pkt_err    = 0;
int                        disc_alloc_fail    = 0;
int                        disc_resource_err  = 0;
int                        disc_external_err  = 0;
int                        disc_internal_err  = 0;
int                        disc_tot_err       = 0;

/* Will fail if this many total errors occurs. */

#ifndef DISC_MAX_ERRORS
#define DISC_MAX_ERRORS 20
#endif

static cpudb_ref_t disc_task_db = NULL;
static disc_callback_handler_t _disc_callback;
static disc_task_cmd_t disc_task_cmd;
static sal_sem_t disc_task_sem;
static disc_status_t disc_stat = DISC_STATUS_INACTIVE;
static sal_mutex_t disc_status_lock;

/****************************************************************
 *
 * Forward declarations
 */

STATIC bcm_rx_t disc_rx_pkt(cpudb_key_t src_key, int port_num,
                            int unit, int port, uint8 *pkt_buf,
                            int len, void *cookie);
STATIC int _disc_init(void);

STATIC int disc_prep(cpudb_ref_t db_ref);
STATIC void queues_clear(void);
STATIC cpudb_entry_t *_disc_key_resolve(cpudb_ref_t db_ref,
                                        cpudb_key_t key,
                                        bcm_mac_t mac,
                                        int dseq_num);

STATIC int disc_done_check(cpudb_ref_t db_ref, int *rv, int unreg);
STATIC void disc_status_update(cpudb_ref_t db_ref);

STATIC void routing_pkts_send(cpudb_ref_t db_ref);
STATIC void routing_pkt_process(cpudb_ref_t db_ref, disc_pkt_t *routing_pkt);
STATIC void probe_pkt_process(cpudb_ref_t db_ref, disc_pkt_t *probe_pkt);
STATIC void probe_pkts_generate(cpudb_ref_t db_ref);
STATIC int _disc_run(cpudb_ref_t db_ref);
STATIC int disc_callout(cpudb_ref_t db_ref, int status);


/****************************************************************
 *
 * Configuration functions for timeout, transport pointer and
 * done callback function
 */

/*
 * Function:
 *      disc_timeout_set/get
 * Purpose:
 *      Set/get the overall timeout and retransmit times in us
 * Parameters:
 *      timeout_us     - Overall timeout in us (OUT for get)
 *      retrx_us       - Retransmit timeout in us (OUT for get)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If either parameter is < 0 on set, it is ignored.
 *      The timeout or cfg timeout may be changed while running;
 *      in this case, (if timeout_us > 0) the start time is
 *      recalculated to the present time.
 */

int
disc_timeout_set(int timeout_us, int cfg_to_us, int retrx_us)
{
    DISC_INIT_CHECK;

    if (timeout_us >= 0) {
        disc_timeout_us = timeout_us;
        if (disc_flags & DF_DISC_RUNNING) {
            disc_start_time = sal_time_usecs();
        }
    }

    if (cfg_to_us >= 0) {
        disc_cfg_to_us = cfg_to_us;
        if (disc_flags & DF_DISC_RUNNING) {
            disc_start_time = sal_time_usecs();
        }
    }

    if (retrx_us >= 0) {
        disc_retrx_us = retrx_us;
    }

    return BCM_E_NONE;
}

int
disc_timeout_get(int *timeout_us, int *cfg_to_us, int *retrx_us)
{
    DISC_INIT_CHECK;

    *timeout_us = disc_timeout_us;
    *cfg_to_us = disc_cfg_to_us;
    *retrx_us = disc_retrx_us;

    return BCM_E_NONE;
}

/*
 * Function:
 *      disc_(cos,vlan)_(set,get)
 * Purpose:
 *      Set/get the COS and VLAN settings used by discovery
 * Notes:
 *      The RX subsystem and chip COS configurations must be
 *      set up consistently with the values used here; similarly
 *      for the VLAN settings.
 */

int
disc_cos_set(int cos)
{
    disc_cos = cos;

    return BCM_E_NONE;
}

int
disc_cos_get(int *cos)
{
    *cos = disc_cos;

    return BCM_E_NONE;
}

int
disc_vlan_set(int vlan)
{
    disc_vlan = vlan;

    return BCM_E_NONE;
}

int
disc_vlan_get(int *vlan)
{
    *vlan = disc_vlan;

    return BCM_E_NONE;
}

/*
 * Sets/gets the current discovery version used when sending
 * discovery packets.
 */
int
disc_version_set(int version)
{
    int rv = BCM_E_NONE;

    switch (version) {
    case DISCOVERY_VERSION_0:
    case DISCOVERY_VERSION_1:
    case DISCOVERY_VERSION_2:
        disc_version = version;
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}

int
disc_version_get(int *version)
{
    if (version != NULL) {
        *version = disc_version;
    }

    return BCM_E_NONE;
}

/*
 * Enables/disables discovery version auto fallback.
 *
 *   When set, the current discovery version is set back
 *   to the received discovery version (lower) in case of a discovery
 *   version mismatch.
 */
int
disc_fallback_set(int enable)
{
    if (enable) {
        disc_fallback = TRUE;
    } else {
        disc_fallback = FALSE;
    }
    return BCM_E_NONE;
}

int
disc_fallback_get(int *enable)
{
    if (enable != NULL) {
        *enable = disc_fallback;
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      disc_trans_ptr_set/get
 * Purpose:
 *      Set/get the transport pointer used by discovery
 * Parameters:
 *      trans_ptr        - (OUT for "get") Pointer to table of drivers
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      trans_ptr may be NULL on get.
 */

int
disc_trans_ptr_set(bcm_trans_ptr_t *trans_ptr)
{
    if (trans_ptr == NULL) {
        return BCM_E_PARAM;
    }

    disc_trans_ptr = trans_ptr;
    return BCM_E_NONE;
}

int
disc_trans_ptr_get(bcm_trans_ptr_t **trans_ptr)
{
    if (trans_ptr != NULL) {
        *trans_ptr = disc_trans_ptr;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      disc_ttl_min/max_set/get
 * Purpose:
 *      Set/get the probe minimum and maximum TTL
 * Parameters:
 *      min_ttl        - Minimum TTL (OUT for get)
 *      max_ttl        - Maximum TTL (OUT for get)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If either parameter is less than DISC_TTL_MIN_DEFAULT,
 *      then the corresponding value will not be set, and return
 *      BCM_E_PARAM.
 */

int
disc_ttl_min_set(int ttl_min)
{
    int rv = BCM_E_NONE;

    if (ttl_min >= DISC_TTL_MIN_DEFAULT) {
        disc_ttl_min = ttl_min;
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

int
disc_ttl_min_get(int *ttl_min)
{
    if (ttl_min) {
        *ttl_min = disc_ttl_min;
    }

    return BCM_E_NONE;
}


int
disc_ttl_max_set(int ttl_max)
{
    int rv = BCM_E_NONE;

    if (ttl_max >= DISC_TTL_MIN_DEFAULT) {
        disc_ttl_max = ttl_max;
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

int
disc_ttl_max_get(int *ttl_max)
{
    if (ttl_max) {
        *ttl_max = disc_ttl_max;
    }

    return BCM_E_NONE;
}


#if defined(BROADCOM_DEBUG)
int _check_start_values(cpudb_ref_t db_ref)
{

    if (!cpudb_valid(db_ref)) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR:  Bad DB reference\n")));
        return BCM_E_PARAM;
    }

    if (db_ref->local_entry == NULL) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR:  Can't find local DB entry\n")));
        return BCM_E_PARAM;
    }

    if (disc_retrx_min_us >= disc_retrx_us) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR:  retrx mis-configuration. min %d max %d\n"),
                   disc_retrx_min_us, disc_retrx_us));
        return BCM_E_PARAM;
    }

    if (db_ref->local_entry->base.num_units < 0 ||
        db_ref->local_entry->base.dest_port < 0 ||
        db_ref->local_entry->base.dest_unit < 0) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc WARN: Bad local DB info\n")));
    }

    if (disc_ttl_min == 1 && disc_ttl_max == 1) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("DISC WARN: TTL min == max == 1.  Discovery will fail\n")));
    }

    return BCM_E_NONE;
}
#endif /* BROADCOM_DEBUG */


/****************************************************************
 *
 * Setup, start and abort functions
 */

/* Get internal Discovery subsystem status */
STATIC int
disc_status_get(disc_status_t *status)
{
    int rv = BCM_E_PARAM;

    if (status) {
        sal_mutex_take(disc_status_lock, sal_mutex_FOREVER);
        *status = disc_stat;
        sal_mutex_give(disc_status_lock);
        rv = BCM_E_NONE;
    }
    
    return rv;
}

#define STEAL_QUEUE(_q, _cnt, _dest_q, _dest_cnt)                          \
    do {                                                                   \
        int _bytes;                                                        \
        DISC_LOCK;                                                         \
        _bytes = (_cnt) * sizeof(disc_pkt_t);                              \
        sal_memcpy(_dest_q, _q, _bytes);                                   \
        sal_memset(_q, 0, _bytes);                                         \
        _dest_cnt = _cnt;                                                  \
        _cnt = 0;                                                          \
        DISC_UNLOCK;                                                       \
    } while (0)

/* Temporary holding pen for handling a discovery queue. */
static disc_pkt_t               _proc_queue[DISC_PKTS_MAX];
static volatile int             _proc_count;

/* Handle incoming probe/routing pkts */

static INLINE void
disc_incoming_pkts_process(cpudb_ref_t db_ref)
{
    int i;

    if (probe_pkt_count > 0) { /* Received probe pkts to forward */
        /* Steal then process the probe packets */
        STEAL_QUEUE(probe_pkt_queue, probe_pkt_count,
                    _proc_queue, _proc_count);

        /* clear fowarded flag */
        for (i = 0; i < db_ref->local_entry->base.num_stk_ports; i++) {
            STK_FLAGS(db_ref, i) &=  ~CPUDB_SPF_TX_FORWARDED;
        }

        for (i = 0; i < _proc_count; i++) {
            probe_pkt_process(db_ref, &_proc_queue[i]);
            if (DISC_EXIT) {
                return;
            }
        }
    }

    if (routing_pkt_count > 0) { /* Received routing packet */
        /* Steal then process the probe packets */
        STEAL_QUEUE(routing_pkt_queue, routing_pkt_count,
                    _proc_queue, _proc_count);

        for (i = 0; i < _proc_count; i++) {
            routing_pkt_process(db_ref, &_proc_queue[i]);
            if (DISC_EXIT) {
                return;
            }
        }
    }
}

#undef STEAL_QUEUE

/*
 * Function:
 *      disc_pkts_process
 * Purpose:
 *      Handle any pending packets queued from RX; check for timeout.
 * Returns:
 *      Boolean, TRUE => done;  FALSE => continue
 * Notes:
 *      Results of discovery are stored in disc_flags
 */

static INLINE void
disc_pkts_process(cpudb_ref_t db_ref)
{
    sal_usecs_t cur_time;
    int dt;

    cur_time = sal_time_usecs();
    /* See if probe packets should be resent. */
    dt = SAL_USECS_SUB(cur_time, disc_last_probe_time);
    if (dt < 0 || dt > disc_retrx_us) {
        if (!(LOCAL_FLAGS(db_ref) & CPUDB_F_LOCAL_COMPLETE)) {
            probe_pkts_generate(db_ref); /* send probes */
        }
        disc_last_probe_time = cur_time;
    }

    if (DISC_EXIT) {
        return;
    }

    /* Check for queued probe and routing pkts */
    disc_incoming_pkts_process(db_ref);

    if (DISC_EXIT) {
        return;
    }

    cur_time = sal_time_usecs();
    if (disc_flags & DF_INFO_IN) {   /* Have info for routing pkts too */
        /*
         * IF     (Never sent out rte pkt) OR
         *        (DB updated and min time elapsed) OR
         *        (retransmit timeout)
         * THEN send route pkts
         */
        dt = SAL_USECS_SUB(cur_time, disc_last_route_time);
        if (disc_last_route_time == 0 ||
            ((disc_flags & DF_DB_UPDATED) &&
             (dt > disc_retrx_min_us)) ||
            (dt > disc_retrx_us) ||
            (dt < 0)) {  /* Send out routing pkts */

            routing_pkts_send(db_ref);
            disc_last_route_time = cur_time;
        }
    }
}


/*
 * Function:
 *      disc_start
 * Purpose:
 *      Start the discovery process
 * Parameters:
 *      db_ref       - The database reference to use; see notes
 *      m_elect      - Master election callback (DEPRECATED)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If db_ref < 0, then a new DB is created and used.
 *
 *      This routine is not re-entrant.
 *
 *      The discovery sequence number will be extracted from the local
 *      CPU DB entry.
 *
 *      Calling disc_election_register() to register an election function
 *      is preferred over passing the election function to disc_start().
 */
int
disc_start(cpudb_ref_t db_ref, disc_start_election_cb_t m_elect)
{
    disc_status_t status;
    int rv;

    BCM_IF_ERROR_RETURN(disc_status_get(&status));

    if (status != DISC_STATUS_INACTIVE) {
        return BCM_E_BUSY;
    }

    if (m_elect != NULL) {
        disc_start_elect = m_elect;
    }
    rv = _disc_run(db_ref);

    return rv;
}

/*
 * Low level init routine; set up sync objects
 * Should only be called once.
 */

STATIC int
_disc_init(void)
{
    int delay, retries;
    int to;

    /* Set up status lock */
    if (disc_status_lock != NULL) {
        sal_mutex_destroy(disc_status_lock);
    }

    disc_status_lock = sal_mutex_create("discstatus");
    if (disc_status_lock == NULL) {
        return BCM_E_MEMORY;
    }
    
    
    /* Set up mutex */
    if (disc_lock != NULL) {
        sal_mutex_destroy(disc_lock);
        disc_lock = NULL;
    }

    disc_lock = sal_mutex_create("discovery");
    if (disc_lock == NULL) {
        sal_mutex_destroy(disc_status_lock);
        return BCM_E_MEMORY;
    }

    /* Set up disc_sem */
    if (disc_sem != NULL) {
        sal_sem_destroy(disc_sem);
    }

    disc_sem = sal_sem_create("disc_sem", sal_sem_BINARY, 0);
    if (disc_sem == NULL) {
        sal_mutex_destroy(disc_lock);
        sal_mutex_destroy(disc_status_lock);
        disc_lock = NULL;
        return BCM_E_MEMORY;
    }

    /* Initialize time out based on ATP settings */
    atp_timeout_get(&delay, &retries);
    to = delay * (retries + 1);
    if (to > DISC_TIMEOUT_DEFAULT) {
        disc_timeout_us = to;
    }
    
    return BCM_E_NONE;
}



/*
 * Function:
 *      disc_abort
 * Purpose:
 *      Request discovery exit with possible restart
 * Parameters:
 *      disc_rv     - Value discovery should return on exit
 *      timeout     - if > 0, us to wait for "running" flag to clear
 *                    before returning fail
 * Returns:
 *      BCM_E_NONE if "running" flag clears before timeout
 *      BCM_E_FAIL if "running" flag still set after timeout
 */

int
disc_abort(int disc_rv, int timeout_us)
{
    int i;
    int retries;
    int rv = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("DISC abort: RV %d, to %d\n"),
                 disc_rv,
                 timeout_us));

    DISC_LOCK;
    disc_abort_rv = disc_rv;
    disc_flags &= ~DF_ABORT_ACK;
    disc_flags |= DF_DISC_ABORT;

    if (!(disc_flags & DF_DISC_RUNNING)) {
        DISC_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("DISC abort: not running\n")));
        return BCM_E_NONE;
    }
    DISC_UNLOCK;

    DISC_WAKE;

    if (timeout_us > 0) {  /* Block until restart noted */
        retries = timeout_us/10000 + 1;  /* At least 1. */

        /* Wait for exit */
        for (i = 0; i < retries; i++) {
            if (disc_flags & DF_ABORT_ACK) {
                break;
            }
            sal_usleep(10000);
        }

        if (!(disc_flags & DF_ABORT_ACK)) {
            LOG_WARN(BSL_LS_TKS_DISCOVER,
                     (BSL_META("DISC WARN: Discovery did not exit in "
                      "%d us\n"), timeout_us));
            rv = BCM_E_FAIL;
        }
    }

    return rv;
}


/* Prepare local variables and send out initial pkts for discovery.
   Assumes lock held.
*/

STATIC int
disc_prep(cpudb_ref_t db_ref)
{
    int rv;
    int i;

    /* Make sure routing packets are allocated */
    for (i = 0; i < ROUTE_PKTS_MAX; i++) {
        if (route_pkt_buf[i] == NULL) {
            route_pkt_buf[i] = DATA_SALLOC(ROUTE_PKT_BYTES_MAX);
            if (route_pkt_buf[i] == NULL) {
                LOG_WARN(BSL_LS_TKS_DISCOVER,
                         (BSL_META("disc WARN: Route pkt alloc %d\n"),
                          i));
                return BCM_E_MEMORY;
            }
        }
    }

    

#if defined(BROADCOM_DEBUG)
    if (CPUDB_ENTRY_COUNT_GET(db_ref) > 1) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc WARN: Local DB has > 1 entry\n")));
    }
#endif /* BROADCOM_DEBUG */

    /* Init error counters */
    disc_tx_pkt_err = disc_rx_pkt_err = 0;
    disc_resource_err = disc_internal_err = disc_tot_err = 0;

    /* Ignore packets (and let them drain) for the first sleep interval */
    disc_flags |= DF_IGNORE_PACKETS;

    if (!next_hop_running()) {
        nh_tx_local_mac_set(LOCAL_MAC(db_ref));
        rv = next_hop_start(&db_ref->local_entry->base);
        if (BCM_FAILURE(rv)) {
            LOG_WARN(BSL_LS_TKS_DISCOVER,
                     (BSL_META("disc WARN: Error starting next hop\n")));
            disc_flags |= DF_DISC_ERROR;
            return rv;
        }
    }

    /* Set up initial state */
    PP_SEEN_ZERO;  /* Clear out stack port markings */
    disc_ttl = disc_ttl_min;
    disc_last_probe_time = disc_start_time = sal_time_usecs();
    disc_last_route_time = 0; /* Signal not yet sent */
#if DISC_PPS > 0
    disc_tx_init = 0;
#endif

    /* Initialize discovery flags, but preserve ABORT_ACK */
    if (disc_flags & DF_ABORT_ACK) {
        disc_flags = DF_DISC_RUNNING | DF_SETUP_DONE | DF_ABORT_ACK;
    } else {
        disc_flags = DF_DISC_RUNNING | DF_SETUP_DONE;
    }

    return BCM_E_NONE;
}

/* Clean up discovery on exit; does not deallocate or do final cleanup */

STATIC void
queues_clear(void)
{
    int i;
    disc_pkt_t *pkt;

    /* Clean up any packets in receive queues */
    for (i = 0; i < DISC_PKTS_MAX; i++) {
        pkt = &probe_pkt_queue[i];
        if (pkt->pkt_buf != NULL) {
            bcm_rx_free(pkt->rx_unit, pkt->pkt_buf);
            pkt->pkt_buf = NULL;
        }
    }
    probe_pkt_count = 0;
    sal_memset(probe_pkt_queue, 0, sizeof(disc_pkt_t) * DISC_PKTS_MAX);

    for (i = 0; i < DISC_PKTS_MAX; i++) {
        pkt = &routing_pkt_queue[i];
        if (pkt->pkt_buf != NULL) {
            bcm_rx_free(pkt->rx_unit, pkt->pkt_buf);
            pkt->pkt_buf = NULL;
        }
    }
    routing_pkt_count = 0;
    sal_memset(routing_pkt_queue, 0, sizeof(disc_pkt_t) * DISC_PKTS_MAX);

    for (i = 0; i < DISC_PKTS_MAX; i++) {
        pkt = &_proc_queue[i];
        if (pkt->pkt_buf != NULL) {
            bcm_rx_free(pkt->rx_unit, pkt->pkt_buf);
            pkt->pkt_buf = NULL;
        }
    }
    _proc_count = 0;
    sal_memset(_proc_queue, 0, sizeof(disc_pkt_t) * DISC_PKTS_MAX);
}

/****************************************************************
 *
 * Discovery packet handling
 */
STATIC int stk_entry_bytes_get(int version);
STATIC int route_entry_bytes_get(int num_sp, int num_units, int version);

STATIC int probe_pkt_local_cpu_key(cpudb_ref_t db_ref,
                                          uint8 *buf, int entry_count);
STATIC void probe_pkt_with_local_key(cpudb_ref_t db_ref,
                                           disc_pkt_t *probe_pkt,
                                           int local_idx,
                                           int entry_count);
STATIC int stk_port_find(cpudb_ref_t db_ref, int unit, int port);
STATIC void probe_pkt_forward(cpudb_ref_t db_ref,
                              disc_pkt_t *probe_pkt,
                              int entry_count, int rx_sp_idx, int fdo);
STATIC int disc_config_send(cpudb_ref_t db_ref);

STATIC bcm_rx_t disc_config_pkt_handler(cpudb_key_t src_key,
                                        int client_id,
                                        bcm_pkt_t *pkt,
                                        uint8 *payload,
                                        int payload_len,
                                        void *cookie);
STATIC int route_entry_process(uint8 *buf, cpudb_entry_t *entry, int version);

/*
 * The following routines returns the buffer size for
 * routing packet entry, as follows:
 *
 *   Route-entry = Route-base + Stk-entries
 *   Stk-entries = Stk-entry * num-stk-ports
 */

/* Stack Entry */
STATIC int
stk_entry_bytes_get(int version)
{
    int bytes = 0;

    switch (version) {
    case DISCOVERY_VERSION_0:
        bytes = STK_ENTRY_RX_IDX_OFS + sizeof(uint32);
        break;
    case DISCOVERY_VERSION_1:
        bytes = STK_ENTRY_BFLAGS_OFS + sizeof(uint32);
        break;
    case DISCOVERY_VERSION_2:
    default:
        bytes = STK_ENTRY_PORT_OFS + sizeof(uint32);
        break;
    }

    return bytes;
}

/* Route Entry */
STATIC int
route_entry_bytes_get(int num_sp, int num_units, int version)
{
    return (ROUTE_BASE_BYTES(num_units) +
            (num_sp * stk_entry_bytes_get(version)));
}

STATIC void
routing_pkt_process(cpudb_ref_t db_ref, disc_pkt_t *routing_pkt)
{
    int entry_count;
    uint8 *buf;
    uint8 *buf_upper_limit;
    int i;
    cpudb_entry_t *entry;
    cpudb_key_t key;
    int bytes;
    int stk_count;
    int num_units;
    int src_dseq_num;
    int dseq_num;
    int version;

    if (routing_pkt->pkt_buf == NULL) {
            ++disc_rx_pkt_err;
            ++disc_tot_err;
            return;
    }
    if (routing_pkt->len == 0) {
            ++disc_rx_pkt_err;
            ++disc_tot_err;
            bcm_rx_free(routing_pkt->rx_unit, routing_pkt->pkt_buf);
            routing_pkt->pkt_buf = NULL;
            return;
    }

    /* buf points to start of disc packet */
    buf = routing_pkt->pkt_buf + CPUTRANS_HEADER_BYTES;
    buf_upper_limit = routing_pkt->pkt_buf + routing_pkt->len;
    entry_count = (int)(buf[ENTRY_COUNT_OFS]);
    UNPACK_LONG(&buf[SRC_DSEQ_NUM_OFS], src_dseq_num);
    version = buf[DISC_VER_OFS];
    buf += DISC_HEADER_BYTES;

    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Processing rte pkt\n")));

    /* If src is not in DB or sequence numbers mismatch, discard pkt */
    CPUDB_KEY_SEARCH(db_ref, routing_pkt->src_key, entry);
    if (entry == NULL) {
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: Rte src key not found " CPUDB_KEY_FMT_EOLN),
                     CPUDB_KEY_DISP(routing_pkt->src_key)));
        bcm_rx_free(routing_pkt->rx_unit, routing_pkt->pkt_buf);
        routing_pkt->pkt_buf = NULL;
        return;
    }
    if (entry->base.dseq_num != src_dseq_num) {
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: Rte src dseq num mismatch; pkt %d, db %d.\n"),
                     src_dseq_num, entry->base.dseq_num));
        bcm_rx_free(routing_pkt->rx_unit, routing_pkt->pkt_buf);
        routing_pkt->pkt_buf = NULL;
        return;
    }

    for (i = 0; i < entry_count; i++) {

        if (buf > buf_upper_limit) {
            ++disc_rx_pkt_err;
            ++disc_tot_err;
            break;
        }

        /*
         * Get entry CPU key and discovery sequence number
         *
         * It is possible for a routing packet to arrive sooner
         * than a probe packet for a given remote CPU. In this case,
         * the entry for the CPU will be created.
         *
         * The CPU entry must contain the correct discovery
         * sequence number to avoid sequence mismatch.
         */
        CPUDB_KEY_UNPACK(buf, key);
        UNPACK_LONG(&buf[ROUTE_DSEQ_NUM_OFS], dseq_num);

        /* Add entry to DB if not already present. */
        entry = _disc_key_resolve(db_ref, key, &buf[ROUTE_MAC_OFS], dseq_num);

        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc: Rte entry %p key " CPUDB_KEY_FMT_EOLN),
                   entry, CPUDB_KEY_DISP(key)));
        if (entry == NULL || entry == db_ref->local_entry) {
            UNPACK_LONG(&buf[ROUTE_NUM_UNITS_OFS], num_units);
            UNPACK_LONG(&buf[ROUTE_STK_COUNT_OFS], stk_count);
            buf += route_entry_bytes_get(stk_count, num_units, version);
            continue;
        }

        bytes = route_entry_process(buf, entry, version);
        if (bytes < 0) {
            ++disc_rx_pkt_err;
            ++disc_tot_err;
            break;
        }
        buf += bytes;
    }

    bcm_rx_free(routing_pkt->rx_unit, routing_pkt->pkt_buf);
    routing_pkt->pkt_buf = NULL;
}

STATIC int
disc_done_check(cpudb_ref_t db_ref, int *rv, int unreg)
{
    sal_usecs_t cur_time;
    int done = TRUE;
    int dt;

    cur_time = sal_time_usecs();

    dt = SAL_USECS_SUB(cur_time, disc_start_time);
    DISC_LOCK;
    if (disc_flags & DF_DISC_ERROR) { /* Error occurred */
        *rv = BCM_E_FAIL;
    } else if (disc_flags & DF_DISC_SUCCESS) { /* Completed successfully */
        *rv = BCM_E_NONE;
    } else if (disc_flags & DF_DISC_ABORT) { /* Exit forced */
        *rv = disc_abort_rv;
    } else if (dt < 0 || dt > disc_timeout_us) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR:  Timeout occurred\n")));
        disc_flags |= DF_DISC_TIMEOUT | DF_DISC_ERROR;
        *rv = BCM_E_TIMEOUT;
    } else if (disc_tot_err >= DISC_MAX_ERRORS) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR:  Error limit exceeded\n")));
        *rv = BCM_E_FAIL;
    } else {
        done = FALSE;
    }

    if (done) {
        /* Clear the running/abort flags for future calls */
        disc_flags &= ~DF_DISC_RUNNING;
        queues_clear();
    }

    DISC_UNLOCK;

    if (unreg && done) {
        /* Unregister callbacks */
        next_hop_unregister(disc_rx_pkt, DISC_NH_PKT_TYPE);
    }

    return done;
}


STATIC INLINE int
all_stk_entries_resolved(cpudb_ref_t db_ref)
{
    cpudb_entry_t *key_entry, *entry;
    int stk_idx;
    uint32 flags;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry->base.num_stk_ports <= 0 && entry != db_ref->local_entry) {
            /* Haven't gotten routing info on this non-local entry yet. */
            return FALSE;
        }
        for (stk_idx = 0; stk_idx < entry->base.num_stk_ports;
             stk_idx++) {
            flags = entry->sp_info[stk_idx].flags;
            if (flags & CPUDB_SPF_NO_LINK ||
                flags & CPUDB_SPF_INACTIVE ||
                flags & CPUDB_SPF_ETHERNET) {
                continue;  /* Ignore disabled ports */
            }
            if (!(flags & CPUDB_SPF_TX_RESOLVED) ||
                !(flags & CPUDB_SPF_RX_RESOLVED)) {
                /* Missing info on this stack port */
                return FALSE;
            }

            /*
             * CPU keys can only be added by probe packets.  It's
             * possible they're referenced by a stack port, but
             * have not been added to the local DB yet.
             */
            CPUDB_KEY_SEARCH(db_ref,
                       entry->sp_info[stk_idx].tx_cpu_key, key_entry);
            if (key_entry == NULL) {
                return FALSE;
            }
            CPUDB_KEY_SEARCH(db_ref,
                       entry->sp_info[stk_idx].rx_cpu_key, key_entry);
            if (key_entry == NULL) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/* Check for config timeout and if so, mark ports w/o PP_SEEN as inactive */

static INLINE void
inactive_ports_check(cpudb_ref_t db_ref)
{
    cpudb_entry_t *lentry;
    sal_usecs_t cur_time;
    int stk_idx;
    int dt;

    if (LOCAL_FLAGS(db_ref) & CPUDB_F_INACTIVE_MARKED) {
        return;
    }

    cur_time = sal_time_usecs();

    dt = SAL_USECS_SUB(cur_time, disc_start_time);

    /*
     * Run through local stack ports; if no probe packet seen on a
     * port w/i timeout, mark as inactive.
     */
    if (dt < 0 || dt > disc_cfg_to_us) {
        lentry = db_ref->local_entry;
        for (stk_idx = 0; stk_idx < lentry->base.num_stk_ports;
             stk_idx++) {
            if (PP_SEEN_GET(stk_idx) == 0) {
                STK_FLAGS(db_ref, stk_idx) |= CPUDB_SPF_INACTIVE;
                LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                            (BSL_META("DISC: Marking SP %d inactive\n"),
                             stk_idx));
            }
        }
        LOCAL_FLAGS(db_ref) |= CPUDB_F_INACTIVE_MARKED;
    }
}


/*
 * Global flags for ATP config client; applications may make
 * this "no-ACK".  This must be set before discovery (or stack
 * task) is started.
 */
uint32 disc_config_atp_flags = DISC_CONFIG_ATP_FLAGS_DEFAULT;

/* Set up ATP once local complete is known */
static INLINE void
lc_atp_setup(cpudb_ref_t db_ref)
{
    int rv;
    cpudb_entry_t *entry;

    /* Register CONFIG client */
    rv = atp_register(DISC_CONFIG_CLIENT_ID, disc_config_atp_flags,
                      disc_config_pkt_handler, db_ref, disc_cos, disc_vlan);
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc WARN: could not register cfg client\n")));
    }

    /* Make sure ATP knows about all keys in DB */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry != db_ref->local_entry) {
            atp_key_add(entry->base.key,
                entry->flags & CPUDB_F_IS_LOCAL);
        }
    }
}

/*
 * The master entry in DB has been set (first time)
 * Do what needs to be done
 */

static INLINE void
master_is_set(cpudb_ref_t db_ref)
{
    int rv;
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("Disc: Master is set to " CPUDB_KEY_FMT " w/ dseq num %d\n"),
                 CPUDB_KEY_DISP(db_ref->master_entry->base.key),
                 db_ref->master_entry->base.dseq_num));
    if (db_ref->master_entry == db_ref->local_entry) {
        /* Send out config pkts */
        rv = disc_config_send(db_ref);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("Disc ERR: cfg send returns %d: %s\n"),
                       rv, bcm_errmsg(rv)));
            DISC_LOCK;
            disc_flags |= DF_DISC_ERROR;
            DISC_UNLOCK;
        } else {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("Disc: master cfg pkts out sn %d\n"),
                         db_ref->master_entry->base.dseq_num));
       }
    }
}

/* Local configuration is complete; continue */
static INLINE void
local_complete_set(cpudb_ref_t db_ref)
{
    /* If we get here, the local DB is complete; make callback */
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("Disc: Local DB complete\n")));
    LOCAL_FLAGS(db_ref) |= CPUDB_F_LOCAL_COMPLETE;

    lc_atp_setup(db_ref);
}

static INLINE void
global_complete_set(cpudb_ref_t db_ref)
{
    int rv;

    LOCAL_FLAGS(db_ref) |= CPUDB_F_GLOBAL_COMPLETE;
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("Disc: All DB entries report complete\n")));

    if (db_ref->master_entry != NULL) {
        /* Assume this means that we're not the master. */
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("DISC: Global done, but master known.\n")));
    }

    /* Attempt to elect master. If the old election callback interface
       is present, use the old one, otherwise use the new one. */
    if (disc_start_elect) {
        rv = disc_start_elect(db_ref);
    } else {
        rv = disc_m_elect(db_ref, disc_m_user_data);
    }
    if (rv != BCM_E_NONE) {
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("Disc: Master elect returns %d causing abort: %s\n"),
                     rv, rv == DISC_RESTART_NEW_SEQ ? "Restarting" :
                     bcm_errmsg(rv)));
        DISC_LOCK;
        disc_flags |= DF_DISC_ABORT;
        disc_abort_rv = rv;
        DISC_UNLOCK;
    } else if (db_ref->master_entry == NULL) {
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("Disc WARN:  Global done; no master\n")));
    } else {
        master_is_set(db_ref);
    }        
}       
        
/*      
 * Check for completed discovery and set flags appropriately
 * Updates local CPU flags when all other CPUs indicate disc done
 */     
        
STATIC void
disc_status_update(cpudb_ref_t db_ref)
{       
    cpudb_entry_t *entry;
    int rv, flags = 0;
        
    /* First, see if cfg pkt should go out; if so, discovery done */
    if ((disc_flags & DF_SEND_CFG_PKT) &&
        !(disc_flags & DF_CFG_PKT_SENT)) {
        rv = disc_config_send(db_ref);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("DISC:  Failed to respond with cfg pkt: %s\n"),
                       bcm_errmsg(rv)));
        } else {
            flags |= DF_DISC_SUCCESS;
        }

        flags |= DF_CFG_PKT_SENT;

        if (flags & DF_DISC_SUCCESS) {
            DISC_LOCK;
            disc_flags |= flags;
            DISC_UNLOCK;
            return;
        }
    }

    /* Look for configuration timeout; mark inactive ports */
    inactive_ports_check(db_ref);

    if (!(LOCAL_FLAGS(db_ref) & CPUDB_F_LOCAL_COMPLETE)) {
        /* Run through all stack ports in CPUDB and see if resolved */
        if (!all_stk_entries_resolved(db_ref)) { 
            return;  /* Nope, continue processing */
        }
        /* All info in DB is resolved.  Set local complete */
        local_complete_set(db_ref);
    }

    if (!(LOCAL_FLAGS(db_ref) & CPUDB_F_GLOBAL_COMPLETE)) {
        /* Now check whether every CPU entry has local complete set */
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            if (!(entry->flags & CPUDB_F_LOCAL_COMPLETE)) {
                return;
            }
        }
        global_complete_set(db_ref);
    } else if (db_ref->local_entry == db_ref->master_entry) {
        /* See if all (other) CPUs have responded with cfg pkt ACKs. */
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            if (entry == db_ref->master_entry) {
                continue;
            }
            if (!(entry->flags & CPUDB_F_CONFIG_IN)) {
                return;
            }
        }

        /* All CPU entries have indicated they accept the configuration */
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("DISC:  All DB entries accept config.\n")));
        flags |= DF_DISC_SUCCESS;
        DISC_LOCK;
        disc_flags |= flags;
        DISC_UNLOCK;
    }
}

/* Check if a local port is duplex; if so, tell next hop */
#define CHECK_IS_DUPLEX(db_ref, sp)                                   \
    if ((STK_FLAGS(db_ref, sp) & CPUDB_SPF_TX_RESOLVED) &&            \
            (STK_FLAGS(db_ref, sp) & CPUDB_SPF_RX_RESOLVED) &&        \
            (CPUDB_KEY_COMPARE(STK_PORT(db_ref, sp).rx_cpu_key,       \
                       STK_PORT(db_ref, sp).tx_cpu_key) == 0)) do {   \
        STK_FLAGS(db_ref, sp) |= CPUDB_SPF_DUPLEX;                    \
        next_hop_port_add(STKP_UNIT(db_ref, sp),                      \
                          STKP_PORT(db_ref, sp), TRUE);               \
    } while (0)


/*
 * Set the RX/TX key if not already known; check for duplex
 * Also checks if "local config" already done; if this
 * occurred on a port that was assumed to be inactive,
 * discovery restart is required.
 */

static INLINE int
stk_port_rx_set(cpudb_ref_t db_ref, int sp, cpudb_key_t cpu_key,
                int remote_sp_idx, uint32 flags)
{
    if (!(STK_FLAGS(db_ref, sp) & CPUDB_SPF_RX_RESOLVED)) {
        if (LOCAL_FLAGS(db_ref) & CPUDB_F_LOCAL_COMPLETE ||
            STK_FLAGS(db_ref, sp) & CPUDB_SPF_INACTIVE) {
            /* Signal restart request to controlling application */
            DISC_LOCK;
            disc_flags |= DF_DISC_ABORT;
            disc_abort_rv = DISC_RESTART_NEW_SEQ;
            DISC_UNLOCK;
            return -1;
        }
        CPUDB_KEY_COPY(STK_PORT(db_ref, sp).rx_cpu_key, (cpu_key));
        STK_PORT(db_ref, sp).rx_stk_idx = (int)(remote_sp_idx);
        STK_FLAGS(db_ref, sp) |= CPUDB_SPF_RX_RESOLVED | (flags);
        DISC_LOCK;
        disc_flags |= DF_DB_UPDATED;
        DISC_UNLOCK;
        CHECK_IS_DUPLEX(db_ref, sp);
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: Set RX for sp %d\n"),
                     sp));
    }

    return 0;
}

/* Set the TX key if not already known; check for duplex */
static INLINE int
stk_port_tx_set(cpudb_ref_t db_ref, int sp, cpudb_key_t cpu_key,
                int remote_sp_idx, uint32 flags)
{
    if (!(STK_FLAGS(db_ref, sp) & CPUDB_SPF_TX_RESOLVED)) {
        if (LOCAL_FLAGS(db_ref) & CPUDB_F_LOCAL_COMPLETE ||
            STK_FLAGS(db_ref, sp) & CPUDB_SPF_INACTIVE) {
            /* Signal restart request to controlling application */
            DISC_LOCK;
            disc_flags |= DF_DISC_ABORT;
            disc_abort_rv = DISC_RESTART_NEW_SEQ;
            DISC_UNLOCK;
            return -1;
        }
        CPUDB_KEY_COPY(STK_PORT(db_ref, sp).tx_cpu_key, (cpu_key));
        STK_PORT(db_ref, sp).tx_stk_idx = (int)(remote_sp_idx);
        STK_FLAGS(db_ref, sp) |= CPUDB_SPF_TX_RESOLVED | (flags);
        DISC_LOCK;
        disc_flags |= DF_DB_UPDATED | DF_TX_KNOWN;
        DISC_UNLOCK;
        CHECK_IS_DUPLEX(db_ref, sp);
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: Set TX for sp %d\n"),
                     sp));
    }

    return 0;
}


/*
 * Get the type out of a discovery packet.  Does not check if it's
 * a discovery packet
 */

int
disc_pkt_type_get(uint8 *pkt_buf)
{
    uint8 *data_start;

    data_start = pkt_buf + CPUTRANS_HEADER_BYTES;
    return (int)data_start[LOCAL_PKT_TYPE_OFS];
}


/*
 * Function:
 *      disc_probe_pkt_new
 * Purpose:
 *      Determine if pkt_buf holds a discovery packet with new
 *      probe information.
 * Returns:
 *      FALSE if not a new probe packet
 *      TRUE if it is a new probe packet
 * Notes:
 *      Should not be called when discovery is running
 *      This is used by stack task to see if new info has
 *      appeared and discovery should be restarted.
 */

int
disc_probe_pkt_new(cpudb_ref_t db_ref, cpudb_key_t src_key, uint8 *pkt_buf)
{
    int pkt_type;
    uint8 *data_start, *entry_buf;
    cpudb_entry_t *entry;
    int i;
    cpudb_key_t key;
    int entry_count;
    int dseq_num;

    /* First, is it a discovery probe packet */
    /* pkt_buf points to start of packet */
    data_start = pkt_buf + CPUTRANS_HEADER_BYTES;
    pkt_type = (int)data_start[LOCAL_PKT_TYPE_OFS];

    if (pkt_type != DISC_PKT_TYPE_PROBE) {
        return FALSE;  /* Not a probe packet */
    }

    /* If no db, definitely a new packet */
    if (db_ref == CPUDB_REF_NULL) {
        return TRUE;
    }

    /* If new source key, it's a new packet */
    CPUDB_KEY_SEARCH(db_ref, src_key, entry);
    if (entry == NULL) {
        return TRUE;
    }

    entry_count = (int)(data_start[ENTRY_COUNT_OFS]);
    entry_buf = data_start + PROBE_ENTRY_OFS(0);

    /*
     * If sequence number different, this is harder.  For now only return if
     * greater than old seq num
     */
    for (i = 0; i < entry_count; i++) {
        CPUDB_KEY_UNPACK(entry_buf, key);
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        UNPACK_LONG(&entry_buf[PROBE_DSEQ_NUM_OFS], dseq_num);
        if (entry == NULL) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("disc probe chk: New key "
                                  CPUDB_KEY_FMT_EOLN), 
                         CPUDB_KEY_DISP(src_key)));
            return TRUE;   /* New CPU key */
        } else {
            if (dseq_num - entry->base.dseq_num > 0) {
                /* pkt SN newer than DB */
                LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                            (BSL_META("disc probe chk: New SN %d for key "
                                      CPUDB_KEY_FMT_EOLN),
                             dseq_num,
                             CPUDB_KEY_DISP(entry->base.key)));
                return TRUE;
            }
        }
        entry_buf += PROBE_ENTRY_BYTES;
    }

    return FALSE;
}

#define PROBE_PKT_OK          0
#define PROBE_PKT_DROP       -1
#define PROBE_PKT_DB_CLEAR   -2
#define PROBE_PKT_MEMORY     -3


/*
 * If found, return index of entry in a probe pkt containing the local mac.
 * buf points to beginning of list of entries.
 */
STATIC INLINE int
probe_pkt_local_cpu_key(cpudb_ref_t db_ref, uint8 *buf, int entry_count)
{
    int i;
    cpudb_key_t key;

    for (i = 0; i < entry_count; i++) {
        CPUDB_KEY_UNPACK(buf, key);
        if (!CPUDB_KEY_COMPARE(key, LOCAL_KEY(db_ref))) {
            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc: Local key in probe pkt, %d\n"),
                       i));
            return i;
        }

        buf += PROBE_ENTRY_BYTES;
    }

    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Local key not in probe pkt\n")));
    return -1;
}

STATIC int probe_pkt_dseq_num_check(cpudb_ref_t db_ref,
                                          uint8 *pkt_start,
                                          int entry_count);
#define _DROP_PKT do { \
        bcm_rx_free(probe_pkt->rx_unit, probe_pkt->pkt_buf); \
        probe_pkt->pkt_buf = NULL; \
    } while (0)

/* Process a probe packet */




STATIC void
probe_pkt_process(cpudb_ref_t db_ref, disc_pkt_t *probe_pkt)
{
    int entry_count, last_entry;
    uint8 *entry_buf;
    int remote_tx_idx;
    int local_idx;
    cpudb_key_t key;
    cpudb_entry_t *remote_ent;
    int rx_sp_idx;
    int dseq_num;
    int drop_pkt;
    int result;


    /* See disc_int.h for packet format description */

    /* Unpack the number of entries */
    entry_count = (int)(probe_pkt->data[ENTRY_COUNT_OFS]);
    last_entry = entry_count - 1;

    /* First, check RX for stk port on which pkt received */
    entry_buf = probe_pkt->data + PROBE_ENTRY_OFS(last_entry);
    CPUDB_KEY_UNPACK(entry_buf, key);
    remote_tx_idx = (int)(entry_buf[PROBE_TX_IDX_OFS]);
    UNPACK_LONG(&entry_buf[PROBE_DSEQ_NUM_OFS], dseq_num);

    /* Record local connection for RX of this stack port */
    rx_sp_idx = stk_port_find(db_ref, probe_pkt->rx_unit,
                              probe_pkt->rx_port);
    if (rx_sp_idx < 0) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Could not find stk port for "
                   "probe from unit %d port %d\n"),
                   probe_pkt->rx_unit, probe_pkt->rx_port));
        ++disc_rx_pkt_err;
        ++disc_tot_err;
        _DROP_PKT;
        return;
    }

    if (!(STK_FLAGS(db_ref, rx_sp_idx) & CPUDB_SPF_RX_RESOLVED)) {
        remote_ent = _disc_key_resolve(db_ref, key,
                                       &entry_buf[PROBE_MAC_OFS], dseq_num);
        if (remote_ent == NULL) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: Error adding KEY to DB\n")));
            ++disc_rx_pkt_err;
            ++disc_tot_err;
            _DROP_PKT;
            return;
        } else if (stk_port_rx_set(db_ref, rx_sp_idx, key,
                                   remote_tx_idx, 0) < 0) {
            _DROP_PKT;
            return;
        }
    }

    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Prb pkt Rmt TX %d RX %d key "
               CPUDB_KEY_FMT " cnt %d\n"), remote_tx_idx,
               rx_sp_idx, CPUDB_KEY_DISP(key), entry_count));

    /*
     * Update the key entries in the DB and check for changed
     * sequence numbers
     */
    entry_buf = probe_pkt->data + PROBE_ENTRY_OFS(0);
    drop_pkt = TRUE;

    if (entry_count > disc_ttl) {
        /* A probe packet with a number of entries greater than the
           TTL means that this is a probe forwarded because of a
           it was found to have a sequence number greater than
           what was found in the receivers database. */
        result = PROBE_PKT_DB_CLEAR;
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: probe entry count %d > %d - forcing forwarding\n"),
                     entry_count, disc_ttl));
    } else {
        result = probe_pkt_dseq_num_check(db_ref, entry_buf, entry_count);
    }

    switch (result) {
    case PROBE_PKT_OK:
        drop_pkt = FALSE;
        break;
    case PROBE_PKT_DROP:
        break;
    case PROBE_PKT_DB_CLEAR:        /* Clear DB and process packet */
        DISC_LOCK;
        disc_flags |= DF_DISC_ABORT;
        disc_abort_rv = DISC_RESTART_REQUEST;
        DISC_UNLOCK;
        break;
    case PROBE_PKT_MEMORY:
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: Could not add probe entry\n")));
        break;
    default:
        assert(0 && "Bad rv dseq_num_check");
        break;
    }

    /* Look for local CPU key in packet; */               
    local_idx = probe_pkt_local_cpu_key(db_ref,
                                        &probe_pkt->data[ENTRY_START_OFS],
                                        entry_count);

    if (!DISC_EXIT && !drop_pkt) {  /* Default behavior */
        if (local_idx < 0) {
            probe_pkt_forward(db_ref, probe_pkt, entry_count,
                              rx_sp_idx, TRUE);
        } else {
            /* Local CPU key is in packet; analyze it */
            probe_pkt_with_local_key(db_ref, probe_pkt,
                                     local_idx, entry_count);
        }

    } else if ((result == PROBE_PKT_DB_CLEAR) && (local_idx < 0)) {
        /*
         * When discovery is restarted due to a probe packet with
         * a new sequence number, always forward the packet to
         * trigger discovery to restart in the rest of the systems.
         *
         * Increasing the TTL by 1 if the ttl is at one will ensure
         * such forwarding will occur for packets that are about to be
         * dropped.
         */
        uint8 *ttl_ptr = &probe_pkt->data[DISC_PROBE_TTL_OFS];
        
        if (*ttl_ptr == 1) {
            (*ttl_ptr)++;
        }
        probe_pkt_forward(db_ref, probe_pkt, entry_count,
                          rx_sp_idx, FALSE);
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("Restart discovery, forward probe from "
                            CPUDB_KEY_FMT_EOLN),
                   CPUDB_KEY_DISP(key)));
    }

    /* Free the buffer */
    bcm_rx_free(probe_pkt->rx_unit, probe_pkt->pkt_buf);
    probe_pkt->pkt_buf = NULL;

    return;
}

#undef _DROP_PKT

/* Check stack port data from the packet */

static INLINE void
stk_port_analyze(cpudb_entry_t *entry, int sp_idx, uint8 *buf, int new_entry,
                 int version)
{
    int new_flags, flags;
    cpudb_stk_port_t *sp_p;

    sp_p = &entry->sp_info[sp_idx];

    UNPACK_U32_INCR(buf, new_flags);
    flags = sp_p->flags;

    /* If new entry or new flags are set, copy data from pkt */
    if (new_entry || ((flags | new_flags) != flags)) { /* Unpack */
        sp_p->flags = new_flags;
        CPUDB_KEY_UNPACK(buf, sp_p->tx_cpu_key);
        buf += CPUDB_KEY_BYTES;
        UNPACK_U32_INCR(buf, sp_p->tx_stk_idx);
        CPUDB_KEY_UNPACK(buf, sp_p->rx_cpu_key);
        buf += CPUDB_KEY_BYTES;
        UNPACK_U32_INCR(buf, sp_p->rx_stk_idx);
        if (version >= DISCOVERY_VERSION_1) {
            UNPACK_U32_INCR(buf, entry->base.stk_ports[sp_idx].weight);
            UNPACK_U32_INCR(buf, entry->base.stk_ports[sp_idx].bflags);
        }
        if (version >= DISCOVERY_VERSION_2) {
            UNPACK_U32_INCR(buf, entry->base.stk_ports[sp_idx].unit);
            UNPACK_U32_INCR(buf, entry->base.stk_ports[sp_idx].port);
        } else {
            entry->base.stk_ports[sp_idx].unit = -1;
            entry->base.stk_ports[sp_idx].port = -1;
        }
        DISC_LOCK;
        disc_flags |= DF_DB_UPDATED;
        DISC_UNLOCK;
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc:  Stk update flags %x\n"),
                   new_flags));
    }
}

#if defined(BROADCOM_DEBUG)
STATIC int
_check_route_pkt(uint8 *buf, cpudb_entry_t *entry)
{
    int stk_port_count;

    /* Could check all of entry, but just check num stk ports for now */
    if (entry->base.num_stk_ports > 0) {  /* Have seen before */
        UNPACK_LONG(&buf[ROUTE_STK_COUNT_OFS], stk_port_count);
        if (entry->base.num_stk_ports != stk_port_count) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: stk port count mismatch for %x:%x\n"),
                       entry->base.mac[4], entry->base.mac[5]));
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("    was %d. new %d\n"),
                       entry->base.num_stk_ports, stk_port_count));
            return BCM_E_FAIL;   /* Do not analyze further */
        }
    }

    return BCM_E_NONE;
}
#endif   /* BROADCOM_DEBUG */

/*
 * Process a DB entry pointed to by buf; buf points to the beginning
 * of the routing entry (the key).
 * Returns number of bytes processed on success.  < 0 if error
 */

STATIC int
route_entry_process(uint8 *buf, cpudb_entry_t *entry, int version)
{
    uint32 combined_flags;
    int new_entry = FALSE;
    int sp_idx;
    int i;
    int num_units;
    int num_stk_ports;
    int dseq_num;
    bcm_mac_t mac;
    uint32 flags;
    int slot_id;
    int master_pri;
    uint8 app_data[CPUDB_APP_DATA_BYTES];
    uint32 base_flags;
    CPUDB_BOARD_ID board_id;
    int dest_unit;
    int dest_port;

    /* Extract the Seq Number from the packet entry */
    UNPACK_LONG(&buf[ROUTE_DSEQ_NUM_OFS], dseq_num);

    /* Do not update CPUDB with stale info from route, treat it as NOOP */
    if (dseq_num < entry->base.dseq_num) {
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: stale info for "
                              CPUDB_KEY_FMT " seq %d->%d\n"),
                     CPUDB_KEY_DISP(entry->base.key),
                     entry->base.dseq_num, dseq_num));
        UNPACK_LONG(&buf[ROUTE_NUM_UNITS_OFS], num_units);
        UNPACK_LONG(&buf[ROUTE_STK_COUNT_OFS], num_stk_ports);
        if (num_units > CPUDB_UNITS_MAX) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: Rte unit cnt %d > DB limit %d\n"),
                       num_units, CPUDB_UNITS_MAX));
            return BCM_E_FAIL;
        }
        if (num_stk_ports > CPUDB_STK_PORTS_MAX) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: Rte stk port cnt %d > DB limit %d\n"),
                       num_stk_ports, CPUDB_STK_PORTS_MAX));
            return BCM_E_FAIL;
        }
        return route_entry_bytes_get(num_stk_ports, num_units, version);
    }

    /* Extract flags from packet entry data */
    UNPACK_LONG(&buf[ROUTE_FLAGS_OFS], flags);

    /* Treat the higher seq number as a new entry. Higher than expected seq
     * number would mean that the existng information is stale and should
     * be all overwritten by the information in the incoming route entry.
     */
    if (!(entry->flags & CPUDB_F_BASE_INIT_DONE) ||
        (dseq_num > entry->base.dseq_num)) {
        if (!(entry->flags & CPUDB_F_BASE_INIT_DONE)) {
            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc: Base init key " CPUDB_KEY_FMT_EOLN),
                   CPUDB_KEY_DISP(entry->base.key)));
            flags |= CPUDB_F_BASE_INIT_DONE;
        } else {
            /* If the entry was already base init'd then append
             * existing flags to the flags from incoming packet
             */
            flags |= entry->flags;
        }

        /* Init base info */
        if (dseq_num > entry->base.dseq_num) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("disc: new info for " CPUDB_KEY_FMT " seq %d->%d\n"),
                         CPUDB_KEY_DISP(entry->base.key),
                         entry->base.dseq_num, dseq_num));
        }

        /* Extract the unretrieved data from the packet entry */
        sal_memcpy(mac, &buf[ROUTE_MAC_OFS], sizeof(bcm_mac_t));
        buf += ROUTE_SLOT_ID_OFS;
        UNPACK_U32_INCR(buf, slot_id);
        if (entry->db_ref->local_entry->base.flags & CPUDB_BASE_F_CHASSIS) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("Found slot %d\n"),
                         slot_id));
        }
        UNPACK_U32_INCR(buf, master_pri);

        sal_memcpy(app_data, buf, CPUDB_APP_DATA_BYTES);
        buf += CPUDB_APP_DATA_BYTES;
        board_id = cpudb_board_id_unknown;
        base_flags = 0;
        /* Get application data: Board ID and Flag */
        if (version >= DISCOVERY_VERSION_2) {
#if defined(DISCOVER_APP_DATA_BOARDID)
            uint8 *app_buf = app_data;
            UNPACK_U32_INCR(app_buf, board_id);
            UNPACK_U32_INCR(app_buf, base_flags);
#endif /* DISCOVER_APP_DATA_BOARDID */
        }

        /* Get unit count and check database limit */
        UNPACK_U32_INCR(buf, num_units);
        if (num_units > CPUDB_UNITS_MAX) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: Rte unit count %d exceeds DB limit %d\n"),
                       num_units, CPUDB_UNITS_MAX));
            return BCM_E_FAIL;
        }
        UNPACK_U32_INCR(buf, dest_unit);
        UNPACK_U32_INCR(buf, dest_port);

        /* Get stack port count and check database limit */
        UNPACK_U32_INCR(buf, num_stk_ports);
        if (num_stk_ports > CPUDB_STK_PORTS_MAX) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: Rte stk port count %d exceeds DB limit %d\n"),
                       num_stk_ports, CPUDB_STK_PORTS_MAX));
            return BCM_E_FAIL;
        }

        sal_memcpy(entry->base.mac, mac, sizeof(bcm_mac_t));
        entry->base.dseq_num = dseq_num;
        entry->flags = flags;
        entry->base.slot_id = slot_id;
        entry->base.master_pri = master_pri;
        sal_memcpy(entry->base.app_data, app_data, CPUDB_APP_DATA_BYTES);
        entry->base.board_id = board_id;
        entry->base.flags = base_flags;
        entry->base.num_units = num_units;
        entry->base.dest_unit = dest_unit;
        entry->base.dest_port = dest_port;
        entry->base.num_stk_ports = num_stk_ports;

        /* Unpack per unit info */
        for (i = 0; i < entry->base.num_units; i++) {
            UNPACK_U32_INCR(buf, entry->base.mod_ids_req[i]);
            UNPACK_U32_INCR(buf, entry->base.pref_mod_id[i]);
            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc:  Rte unit %d. mr %d. pref %d\n"),
                       i, entry->base.mod_ids_req[i],
                       entry->base.pref_mod_id[i]));
        }


        DISC_LOCK;
        disc_flags |= DF_DB_UPDATED;
        DISC_UNLOCK;
        new_entry = TRUE;
    } else {
        /* Check for new flags */
        combined_flags = (flags | entry->flags);
        if (combined_flags != entry->flags) {
            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc:  Rte flag update %x->%x\n"),
                       entry->flags, combined_flags));
            entry->flags = combined_flags;
            DISC_LOCK;
            disc_flags |= DF_DB_UPDATED;
            DISC_UNLOCK;
        }
#if defined(BROADCOM_DEBUG)
        BCM_IF_ERROR_RETURN(_check_route_pkt(buf, entry));
#endif  /* BROADCOM_DEBUG */
        buf += ROUTE_BASE_BYTES(entry->base.num_units);
    }

    /* buf now points to the beginning of the stack ports.  Analyze them */
    for (sp_idx = 0; sp_idx < entry->base.num_stk_ports; sp_idx++) {
        stk_port_analyze(entry, sp_idx, buf, new_entry, version);
        buf += stk_entry_bytes_get(version);
    }

    return route_entry_bytes_get(entry->base.num_stk_ports,
                                 entry->base.num_units, version);
}

/****************************************************************
 * RX next hop packet handler; At this point, we know it's a
 * discovery packet, but don't know what kind.
 */

/* Assumes lock */

static INLINE int
enqueue_probe_pkt(cpudb_key_t src_key, int unit, int port,
                  uint8 *pkt_buf, int len)
{
    disc_pkt_t *pkt_info;

    if (probe_pkt_count + 1 >= DISC_PKTS_MAX) {
        ++disc_resource_err;
        return BCM_E_RESOURCE;
    }

    pkt_info = &probe_pkt_queue[probe_pkt_count++];
    CPUDB_KEY_COPY(pkt_info->src_key, src_key);
    pkt_info->rx_unit = unit;
    pkt_info->rx_port = port;
    pkt_info->pkt_buf = pkt_buf;
    pkt_info->len = len;
    pkt_info->data = pkt_buf + CPUTRANS_HEADER_BYTES;

    return BCM_E_NONE;
}

/* Assumes lock */

static INLINE int
enqueue_routing_pkt(cpudb_key_t src_key, int unit, int port,
                    uint8 *pkt_buf, int len)
{
    disc_pkt_t *pkt_info;

    if (routing_pkt_count + 1 >= DISC_PKTS_MAX) {
        ++disc_resource_err;
        return BCM_E_RESOURCE;
    }

    pkt_info = &routing_pkt_queue[routing_pkt_count++];
    CPUDB_KEY_COPY(pkt_info->src_key, src_key);
    pkt_info->rx_unit = unit;
    pkt_info->rx_port = port;
    pkt_info->pkt_buf = pkt_buf;
    pkt_info->len = len;

    return BCM_E_NONE;
}

/* Assumes lock held */
static INLINE int
stack_port_active_mark(cpudb_ref_t db_ref, int unit, int port)
{
    int idx;

    idx = stk_port_find(db_ref, unit, port);
    if (idx >= 0) {
        if (STK_FLAGS(db_ref, idx) & CPUDB_SPF_INACTIVE) {
            /*
             * An inactive port has seen a discovery packet.  This
             * counts as a stack event.  Request restart
             */
            disc_flags |= DF_DISC_ABORT;
            disc_abort_rv = DISC_RESTART_NEW_SEQ;
            return -1;
        }
        PP_SEEN_SET(idx);  /* Probe packet seen on port */
    }

    return 0;
}

/* Supports versions 0, 1, and 2 packets */

STATIC bcm_rx_t
disc_rx_pkt_ver(cpudb_key_t src_key, 
                int unit, int port, uint8 *pkt_buf,
                int len, cpudb_ref_t db_ref)
{
    int pkt_type;

    /* Called with DISC_LOCK held */
    pkt_type = disc_pkt_type_get(pkt_buf);

    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META_U(unit,
              "disc: %s (%d) pkt in\n"),
               pkt_type == DISC_PKT_TYPE_PROBE ? "PROBE" : "ROUTING" ,
               pkt_type));
    switch (pkt_type) {
    case DISC_PKT_TYPE_PROBE:          /* Probe packet */

        /* Mark the stack port active if found */
        if (stack_port_active_mark(db_ref, unit, port) < 0) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META_U(unit,
                        "disc: Inactive Stack port %d %d now active.\n"),
                         unit, port));
            DISC_UNLOCK;
            DISC_WAKE;
            return BCM_RX_HANDLED;
        }
        if (enqueue_probe_pkt(src_key, unit, port, pkt_buf, len) < 0) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META_U(unit,
                        "disc: Discarding probe pkt\n")));
            DISC_UNLOCK;
            return BCM_RX_HANDLED;
        }
        break;
    case DISC_PKT_TYPE_ROUTING:        /* Routing packet */
        if (enqueue_routing_pkt(src_key, unit, port, pkt_buf, len) < 0) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META_U(unit,
                        "disc: Discarding routing pkt\n")));
            DISC_UNLOCK;
            return BCM_RX_HANDLED;
        }
        break;
    default:
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META_U(unit,
                 "disc WARN: Unsupported packet type %d"),
                  pkt_type));
        ++disc_tot_err;
        DISC_UNLOCK;
        return BCM_RX_NOT_HANDLED;

    }

#if defined(BCM_RXP_DEBUG)
    bcm_rx_pool_own(pkt_buf, "disc_pkt");
#endif

    /* Normal exit */
    DISC_UNLOCK;
    DISC_WAKE;

    return BCM_RX_HANDLED_OWNED;
}

STATIC bcm_rx_t
disc_rx_pkt(cpudb_key_t src_key, int port_num,
            int unit, int port, uint8 *pkt_buf,
            int len, void *cookie)
{
    int d_ver;
    cpudb_ref_t db_ref;
    static int version_warn = TRUE;

    COMPILER_REFERENCE(port_num);

    if (!INIT_DONE) {
        return BCM_RX_NOT_HANDLED;
    }

    DISC_LOCK;

    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META_U(unit,
              "disc: disc_rx_pkt\n")));
    if (!(disc_flags & DF_DISC_RUNNING)) {
        DISC_UNLOCK;
        /* If no task_db or discovery not running, then sink probe */
        if (disc_pkt_type_get(pkt_buf) == DISC_PKT_TYPE_PROBE &&
            disc_task_db) {
            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                      (BSL_META_U(unit,
                      "disc: rx while idle.\n")));
            (void)disc_callout(disc_task_db, DISC_PROBE_RECEIVED);
        }
        return BCM_RX_NOT_HANDLED;
    }

    if (disc_flags & DF_IGNORE_PACKETS) {
        DISC_UNLOCK;
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META_U(unit,
                  "disc: rx ignored.\n")));
        return BCM_RX_NOT_HANDLED;
    }

    db_ref = *(cpudb_ref_t *)cookie;
    if (!cpudb_valid(db_ref)) {
        DISC_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META_U(unit,
                    "disc: Config pkt in when db_ref is invalid.\n")));
        return BCM_RX_NOT_HANDLED;
    }

    d_ver = pkt_buf[CPUTRANS_HEADER_BYTES + DISC_VER_OFS];

    /* Fallback only if received discovery packet version is lower */
    if (disc_fallback && (disc_version > d_ver)) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META_U(unit,
                 "DISC WARN: Protocol version changed from %d to %d\n"),
                  disc_version, d_ver));
        disc_version = d_ver;
        DISC_UNLOCK;
        return BCM_RX_HANDLED;
    } else if (d_ver != disc_version && version_warn) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META_U(unit,
                 "DISC WARN: Received discovery version %d, sending %d\n"),
                  d_ver, disc_version));
        version_warn = FALSE;
    }

    switch (d_ver) {
    case DISCOVERY_VERSION_0:
    case DISCOVERY_VERSION_1:
    case DISCOVERY_VERSION_2:
        return disc_rx_pkt_ver(src_key, unit, port, pkt_buf, len, db_ref);
    }

    /* Unknown version - throw packet away */
    DISC_UNLOCK;
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META_U(unit,
                "disc: Unknown discovery version %d in probe/route\n"),
                 d_ver));
    return BCM_RX_NOT_HANDLED;
}


/*
 * Set up the header of a discovery pkt.
 *
 * Note that "ttl" is put in the reserved field.  If DISC TTL is not
 * enabled, then this value should be 0.
 */

STATIC INLINE void
disc_pkt_setup(uint8 *pkt_buf, uint8 pkt_type, uint8 entry_count,
               uint8 ttl_rsvd, int dseq_num, int version)
{
    pkt_buf[DISC_VER_OFS] = (uint8)version;
    pkt_buf[DISC_RSVD_OFS] = (uint8)ttl_rsvd;
    pkt_buf[LOCAL_PKT_TYPE_OFS] = pkt_type;
    pkt_buf[ENTRY_COUNT_OFS] = entry_count;
    PACK_LONG(&pkt_buf[SRC_DSEQ_NUM_OFS], dseq_num);
}


/*
 * Function:
 *      disc_header_setup
 * Purpose:
 *      Set up the header for a discovery packet
 * Parameters:
 *      pkt_buf        - Buffer to setup
 *      pkt_type       - Type for discovery header
 *      entry_count    - How many entries (if applicable)
 * Returns:
 *      BCM_E_XXX
 */

void
disc_header_setup(uint8 *pkt_buf, uint8 pkt_type, uint8 entry_count,
                  uint8 ttl_rsvd, int dseq_num)
{
    disc_pkt_setup(pkt_buf, pkt_type, entry_count, ttl_rsvd, dseq_num,
                   disc_version);
}

#if DISC_PPS > 0

#define DISC_TX_DELAY (1000000/DISC_PPS) /* usec between transmissions */

STATIC void
disc_rate_limit(void)
{
    sal_usecs_t now = sal_time_usecs();
    int delta = 0;

    if (disc_tx_init) {
        delta = SAL_USECS_SUB(now, disc_tx_prev);
        if (delta > 0 && delta < DISC_TX_DELAY) {
            sal_usleep(DISC_TX_DELAY - delta);
        }
    } else {
        disc_tx_init = !disc_tx_init;
    }

    disc_tx_prev = now;
}
#else
/* rate limiting disabled */
#define disc_rate_limit()
#endif /*  DISC_PPS */

/****************************************************************
 *
 * Probe packet handling
 */

/* Explicitly send probe pkt to each stk port so that idx can be updated */

STATIC void
probe_pkt_tx(cpudb_ref_t db_ref, uint8 *buf, int len, uint8 *cur_entry,
             disc_pkt_t *probe_pkt)
{
    int i;
    int rv = BCM_E_NONE;
    int unit, port;

    /* if probe_pkt is not NULL, detect full duplex optimization */

    for (i = 0; i < db_ref->local_entry->base.num_stk_ports; i++) {
        if (STK_FLAGS(db_ref, i) & CPUDB_SPF_NO_LINK ||
            STK_FLAGS(db_ref, i) & CPUDB_SPF_ETHERNET) {
            continue;
        }
        unit = STKP_UNIT(db_ref, i);
        port = STKP_PORT(db_ref, i);

        if (probe_pkt &&
            (unit != probe_pkt->rx_unit || port != probe_pkt->rx_port)) {
            /* full duplex optimization: only forward packets to original
               sender. */
            continue;
        }
        if (STK_FLAGS(db_ref, i) & CPUDB_SPF_TX_RESOLVED &&
            STK_FLAGS(db_ref, i) & CPUDB_SPF_RX_RESOLVED &&
            STK_FLAGS(db_ref, i) & CPUDB_SPF_TX_FORWARDED) {
            /* this port already resolved locally, and a probe
               has been forwarded via this port. */
           continue;
        }
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META_U(unit,
                  "DISC:  Sending probe pkt cos %d to port %d (%d, %d)\n"),
                   disc_cos, i, unit, port));

        next_hop_buffer_init(buf, DISC_NH_PKT_TYPE, cpudb_neighbor_key);
        cur_entry[PROBE_TX_IDX_OFS] = (uint8)i;

        disc_rate_limit();

        rv = nh_tx(unit,
                   port,
                   buf,
                   len,
                   disc_cos,
                   disc_vlan,
                   NEXT_HOP_PKT_TYPE,
                   CPUTRANS_NO_HEADER_ALLOC | CPUTRANS_CRC_REGEN,
                   NULL,
                   NULL);
        if (BCM_FAILURE(rv)) {
            ++disc_tx_pkt_err;
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META_U(unit,
                      "disc ERR %d: probe pkt tx to "
                       "stkport %d: %s\n"), rv, i, bcm_errmsg(rv)));
            break;
        } else {
            if (probe_pkt) {
                STK_FLAGS(db_ref, i) |= CPUDB_SPF_TX_FORWARDED; 
            }
        }
    }
}


/* Send out initial probe packets */
STATIC void
probe_pkts_generate(cpudb_ref_t db_ref)
{
    uint8 *buf, *start_buf;
    int len;

    /* Just one entry in initial packets sent out */
    len = CPUTRANS_HEADER_BYTES + PROBE_ENTRY_OFS(1) + sizeof(uint32);
    buf = start_buf = DATA_SALLOC(len);
    if (buf == NULL) {
        ++disc_alloc_fail;
        ++disc_tot_err;
        return;
    }

    buf += CPUTRANS_HEADER_BYTES;
    disc_pkt_setup(buf, DISC_PKT_TYPE_PROBE, 1, disc_ttl,
                   db_ref->local_entry->base.dseq_num, disc_version);
    buf += ENTRY_START_OFS;
    CPUDB_KEY_PACK(buf, LOCAL_KEY(db_ref));
    sal_memcpy(&buf[PROBE_MAC_OFS], LOCAL_MAC(db_ref), sizeof(bcm_mac_t));
    /* Set sequence number */
    PACK_LONG(&buf[PROBE_DSEQ_NUM_OFS], CUR_DSEQ_NUM(db_ref));
    probe_pkt_tx(db_ref, start_buf, len, buf, NULL);

    /* Update TTL if enabled (disc_ttl > 0) */
    if (disc_ttl > 0 && ++disc_ttl > disc_ttl_max) {
        disc_ttl = disc_ttl_max;
    } else {
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("DISC: TTL is %d\n"),
                   disc_ttl));
    }

    DATA_SFREE(start_buf);
}


/*
 * Clear all mod ids
 */
STATIC INLINE void
db_entry_clear_mod_ids(cpudb_entry_t *entry)
{
    int  unit;
    int  max_units;

    max_units = COUNTOF(entry->base.pref_mod_id);
    for (unit = 0; unit < max_units; unit++) {
        entry->base.pref_mod_id[unit] = -1;
    }

    max_units = COUNTOF(entry->mod_ids);
    for (unit = 0; unit < max_units; unit++) {
        entry->mod_ids[unit] = -1;
    }

    return;
}

/*
 * Run through the entries in a probe packet.  Add new entries to
 * the DB.  For existing entries, check for changes in sequence number.
 *
 * In order of precedence, returns
 *    PROBE_PKT_DROP if packet should be dropped due to old seq num.
 *    PROBE_PKT_DB_CLEAR if newer sequence number found;
 *    PROBE_PKT_MEMORY if unable to add an entry
 *    PROBE_PKT_OK if all okay;
 *
 * pkt_buf points to the beginning of the first entry in the packet
 */

STATIC int
probe_pkt_dseq_num_check(cpudb_ref_t db_ref, uint8 *pkt_start,
                        int entry_count)
{
    cpudb_key_t key;
    cpudb_entry_t *entry;
    int i;
    int dseq_num;
    int new_entries = 0;
    int diff;
    uint8 *pkt_buf, *first_new;

    first_new = NULL;
    pkt_buf = pkt_start;
    /* First, run thru entries looking for different seq nums (drop pkt) */
    for (i = 0; i < entry_count; i++) {
        CPUDB_KEY_UNPACK(pkt_buf, key);
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        UNPACK_LONG(&pkt_buf[PROBE_DSEQ_NUM_OFS], dseq_num);
        if (entry == NULL) {
            new_entries++;
            if (first_new == NULL) {
                first_new = pkt_buf;
            }
        } else {
            diff = dseq_num - entry->base.dseq_num;
            if (diff > 0) {  /* pkt SN is newer than local DB */
                LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                            (BSL_META("disc: New seq num %d for key "
                                      CPUDB_KEY_FMT_EOLN),
                             dseq_num,
                             CPUDB_KEY_DISP(entry->base.key)));
                return PROBE_PKT_DB_CLEAR;
            } else if (diff < 0) { /* pkt SN is older than DB */
                LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                            (BSL_META("disc: Old seq num. DB SN %d. Pkt SN %d. key "
                                      CPUDB_KEY_FMT_EOLN),
                             entry->base.dseq_num,
                             dseq_num, CPUDB_KEY_DISP(entry->base.key)));
                return PROBE_PKT_DROP;
            }
        }
        pkt_buf += PROBE_ENTRY_BYTES;
    }

    /* Now update any missing entries */
    pkt_buf = first_new;
    while (new_entries > 0) {
        CPUDB_KEY_UNPACK(pkt_buf, key);
        CPUDB_KEY_SEARCH(db_ref, key, entry);
        UNPACK_LONG(&pkt_buf[PROBE_DSEQ_NUM_OFS], dseq_num);
        if (entry == NULL) {
            new_entries--;
            DISC_LOCK;
            disc_flags |= DF_DB_UPDATED;
            DISC_UNLOCK;
            entry = cpudb_entry_create(db_ref, key, FALSE);
            if (entry == NULL) {
                return PROBE_PKT_MEMORY;
            }
            db_entry_clear_mod_ids(entry);
            entry->base.dseq_num = dseq_num;
        }

        pkt_buf += PROBE_ENTRY_BYTES;
    }
    return PROBE_PKT_OK;
}

/*
 * Local CPU key is not in this probe pkt;
 * Add local info to pkt and send off to all stack ports.
 *
 * ASSUMPTION:  The RX packet we're manipulating is long enough
 * to hold a max length probe packet.
 *
 * Disable full duplex optimization if enable_fdo is false.
 */

STATIC void
probe_pkt_forward(cpudb_ref_t db_ref, disc_pkt_t *probe_pkt,
                  int entry_count, int rx_sp_idx, int enable_fdo)
{
    uint8 *local_entry;
    int tx_len;
    uint8 ttl_rsvd;
    disc_pkt_t *fd_opt = NULL;

    ttl_rsvd = probe_pkt->data[DISC_PROBE_TTL_OFS];
    switch (ttl_rsvd) {
    case 0:  /* Ignore TTL */
        break;
    case 1:  /* TTL expired, do not forward */
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("DISC: Not forwarding probe pkt due to TTL == 1\n")));
        return;
    default: /* Decrement TTL and forward */
        probe_pkt->data[DISC_PROBE_TTL_OFS] = ttl_rsvd - 1;

        if (disc_ttl_min == DISC_TTL_FULL_DUPLEX &&
            disc_ttl_max == DISC_TTL_FULL_DUPLEX &&
            enable_fdo) {
            fd_opt = probe_pkt;
        }

        break;
    }

    local_entry = &probe_pkt->data[PROBE_ENTRY_OFS(entry_count++)];
    CPUDB_KEY_PACK(local_entry, LOCAL_KEY(db_ref));
    sal_memcpy(&local_entry[PROBE_MAC_OFS], LOCAL_MAC(db_ref),
               sizeof(bcm_mac_t));
    local_entry[PROBE_RX_IDX_OFS] = (uint8)rx_sp_idx;
    PACK_LONG(&local_entry[PROBE_DSEQ_NUM_OFS], CUR_DSEQ_NUM(db_ref));

    /* Update the entry count in the packet */
    probe_pkt->data[ENTRY_COUNT_OFS] = (uint8)entry_count;

    /* TX UNIT and TX PORT are filled in by receiving CPU */
    tx_len = CPUTRANS_HEADER_BYTES + DISC_HEADER_BYTES +
               entry_count * PROBE_ENTRY_BYTES + sizeof(uint32);
    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Prb pkt out, ent %d len %d\n"),
               entry_count, tx_len));
    probe_pkt_tx(db_ref, probe_pkt->pkt_buf, tx_len, local_entry, fd_opt);
}


/*
 * Handle a probe packet with the local CPU key which has traveled
 * off the board (it has some other CPU info as well.)
 *
 * The transmit portion of the original stack port is now resolved.
 *
 * Otherwise, we scan through the entries and add the CPU keys.
 * We could discern a lot of stack ports, but that will come to us
 * from routing packets.
 */

static INLINE void
off_board_pkt_handle(cpudb_ref_t db_ref,
                     uint8 *buf,       /* Points to first new entry */
                     int tx_sp_idx,    /* index of orig TX stk port */
                     int new_entries)  /* How many new entries in pkt --
                                          that is, >= local entry */
{
    int i;
    cpudb_entry_t *entry;
    cpudb_key_t key;
    int remote_rx_idx;
    int dseq_num;

    /* From the first entry after the local key, we can determine the
     * TX connection of the stack port.  Note:  new_entries >= 1.  */

    remote_rx_idx = (int)buf[PROBE_RX_IDX_OFS];
    CPUDB_KEY_UNPACK(buf, key);
    UNPACK_LONG(&buf[PROBE_DSEQ_NUM_OFS], dseq_num);
    entry = _disc_key_resolve(db_ref, key, &buf[PROBE_MAC_OFS], dseq_num);
    if (entry == NULL) {
        ++disc_rx_pkt_err;
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: bad key, off board 0\n")));
        return;
    }

    if (stk_port_tx_set(db_ref, tx_sp_idx, key, remote_rx_idx, 0) < 0) {
        return;
    }

    /* Have seen an offboard packet now */
    DISC_LOCK;
    disc_flags |= DF_INFO_IN;
    DISC_UNLOCK;

    /* Put other MACs in the local DB */
    buf += PROBE_ENTRY_BYTES;  /* Next entry */
    for (i = 1; i < new_entries; i++) {  /* One done above */
        CPUDB_KEY_UNPACK(buf, key);
        UNPACK_LONG(&buf[PROBE_DSEQ_NUM_OFS], dseq_num);
        entry = _disc_key_resolve(db_ref, key, &buf[PROBE_MAC_OFS], dseq_num);
        if (entry == NULL) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: bad key, off board %d\n"),
                       i));
            ++disc_rx_pkt_err;
            return;
        }
        buf += PROBE_ENTRY_BYTES;  /* Next entry */
    }
}


/* Handle a probe packet that has the local MAC address in an entry
 */

STATIC void
probe_pkt_with_local_key(cpudb_ref_t db_ref,
                         disc_pkt_t *probe_pkt,  /* The pkt */
                         int local_idx,   /* Where is local MAC */
                         int entry_count) /* How many entries in pkt */
{
    uint8 *buf;
    int tx_sp_idx;
    int rx_sp_idx;

    /* Local MAC found.  Update local DB.  Set buf to start of entries */
    buf = &probe_pkt->data[PROBE_ENTRY_OFS(local_idx)];

    /* Grab the originating unit/port info for this pkt */
    tx_sp_idx = (int)buf[PROBE_TX_IDX_OFS];
    rx_sp_idx = stk_port_find(db_ref, probe_pkt->rx_unit, probe_pkt->rx_port);

    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Prb pkt local. RX %d. TX %d, ent cnt %d\n"),
               rx_sp_idx, tx_sp_idx, entry_count));

#if defined(BROADCOM_DEBUG)
    if (tx_sp_idx < 0 || tx_sp_idx > db_ref->local_entry->base.num_stk_ports) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Did not find stk port in probe pkt")));
        return;
    }
    if (rx_sp_idx < 0) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Did not find rx stk port of probe pkt")));
        return;
    }
#endif /* BROADCOM_DEBUG */

    if (entry_count == 1) { /* It's an on-board connection */
        /*
         * (cpu, tx_sp_idx) --> (cpu, rx_sp_idx);
         * so TX of tx_sp_idx goes to local CPU, rx_sp_idx;
         * and pkts received on rx_sp_idx were sent by local cpu to tx_sp_idx.
         * Got that?  The RX one was already set in probe_pkt_process.
         */
        stk_port_tx_set(db_ref, tx_sp_idx, LOCAL_KEY(db_ref),
                        rx_sp_idx, CPUDB_SPF_ON_BOARD);
    } else {
        off_board_pkt_handle(db_ref, buf + PROBE_ENTRY_BYTES, tx_sp_idx,
                             entry_count - local_idx - 1);
    }
}


/****************************************************************
 *
 * Routing packet handling
 */
STATIC INLINE int db_entry_pack(uint8 *buf, cpudb_entry_t *entry,
                                int max_bytes, int version);

/*
 * Send a route packet specifically to each stack port using
 * next hop tx.
 */
static INLINE void
route_pkt_tx(cpudb_ref_t db_ref, uint8 *buf, int len)
{
    int i;
    int rv = BCM_E_NONE;
    cpudb_entry_t *dest_entry;

    /* Keep track of TX keys to which we've sent info; up to 4 keys */
    cpudb_key_t sent_keys[CPUDB_STK_PORTS_MAX];
    int sk_count = 0, sk_idx;
    int cpu_done;

    for (i = 0; i < db_ref->local_entry->base.num_stk_ports; i++) {
        if (!(STK_FLAGS(db_ref, i) & CPUDB_SPF_TX_RESOLVED) ||
            (STK_FLAGS(db_ref, i) & CPUDB_SPF_NO_LINK) ||
            (STK_FLAGS(db_ref, i) & CPUDB_SPF_ETHERNET) ) {
            continue;
        }

        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc rte pkt out len %d to %d @ " CPUDB_KEY_FMT_EOLN),
                   len, i, CPUDB_KEY_DISP(STK_PORT(db_ref, i).tx_cpu_key)));
        CPUDB_KEY_SEARCH(db_ref, STK_PORT(db_ref, i).tx_cpu_key, dest_entry);
        if (dest_entry != NULL) {
            cpu_done = FALSE;     /* See if info sent to this CPU already */
            for (sk_idx = 0; sk_idx < sk_count; sk_idx++) {
                if (CPUDB_KEY_EQUAL(sent_keys[sk_idx], dest_entry->base.key)) {
                    cpu_done = TRUE;
                    break;
                }
            }
            if (cpu_done) {
                continue;
            }

            if (sk_count < CPUDB_STK_PORTS_MAX) {  /* Record destination key */
                CPUDB_KEY_COPY(sent_keys[sk_count], dest_entry->base.key);
                sk_count++;
            }
        }

        next_hop_buffer_init(buf, DISC_NH_PKT_TYPE, cpudb_neighbor_key);

        disc_rate_limit();

        rv = nh_tx(STKP_UNIT(db_ref, i),
                   STKP_PORT(db_ref, i),
                   buf,
                   len,
                   disc_cos,
                   disc_vlan,
                   NEXT_HOP_PKT_TYPE,
                   CPUTRANS_NO_HEADER_ALLOC,
                   NULL,
                   NULL);
        if (BCM_FAILURE(rv)) {
            ++disc_tx_pkt_err;
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: sending rte to stkport %d\n"),
                       i));
            break;
        }
        if (DISC_EXIT) {
            break;
        }
    }
}


/* Pack the current database into routing packets */
static INLINE int
routing_pkts_form(cpudb_ref_t db_ref)
{
    uint8 *pkt_buf = NULL;
    cpudb_entry_t *entry;
    int cur_offset = 0; /* Byte offset into current packet */
    int tot_entries;    /* Total number of route entries to pack */
    int tot_pkts;       /* Total number of route packets */
    int entry_bytes;    /* Number of bytes in route entry */
    int num_entries;    /* Number of entries already packed in the packet */
    int pkt_bytes_left; /* Remaining bytes in current route packet */
    int cur_pkt_idx;    /* Current route packet being packed */
    int version = disc_version;


    /*
     * A routing packet has the following format
     * 
     *   +----------------+----------------+-------------------+
     *   | <cputrans-hdr> | <disc-pkt-hdr> | <route-entry> * n |
     *   +----------------+----------------+-------------------+
     *
     * Where, n is the number of route entries in the packet
     */

    /* Remaining bytes available in packet buffer */
    pkt_bytes_left = ROUTE_PKT_BYTES_MAX -
                     (CPUTRANS_HEADER_BYTES + ENTRY_START_OFS);

    tot_pkts    = 0;
    tot_entries = 0;
    num_entries = 0;
    cur_pkt_idx = -1;
    
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (!(entry->flags & CPUDB_F_BASE_INIT_DONE)) {
            continue;
        }
        
        /* Get size required by route entry */
        entry_bytes = route_entry_bytes_get(entry->base.num_stk_ports,
                                            entry->base.num_units,
                                            version);
        /*
         * Route packet should fit at least one entry
         */
        if ((entry_bytes > pkt_bytes_left) && (num_entries == 0)) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: route pkt entry too long, need %d, left %d\n"),
                       entry_bytes, pkt_bytes_left));
            ++disc_internal_err;
            ++disc_tot_err;
            return -1;
        }

        /*
         * Start a new packet if:
         *   - Current number of entries is 0, or
         *   - There is no enough space left in packet, or
         *   - Number of entries per packet has reached the limit
         */
        if ((num_entries == 0) || (entry_bytes > pkt_bytes_left) ||
            (num_entries >= ROUTE_ENTRIES_PER_PKT_MAX)) {

            /* Complete setup for current packet, if any */
            if (cur_pkt_idx >= 0) {
                /* Set header and record length */
                disc_pkt_setup(pkt_buf + CPUTRANS_HEADER_BYTES,
                               DISC_PKT_TYPE_ROUTING, num_entries, 0,
                               db_ref->local_entry->base.dseq_num, version);
                route_pkt_len[cur_pkt_idx] = ROUTE_PKT_BYTES_MAX -
                                             pkt_bytes_left;
            }

            /* Check route packets limit */
            if ((++cur_pkt_idx) >= ROUTE_PKTS_MAX) {
                LOG_ERROR(BSL_LS_TKS_DISCOVER,
                          (BSL_META("disc ERR: number of route pkts exceeds limit %d\n"),
                           ROUTE_PKTS_MAX));
                ++disc_internal_err;
                ++disc_tot_err;
                return -1;
            }

            num_entries    = 0;
            pkt_buf        = route_pkt_buf[cur_pkt_idx];
            pkt_bytes_left = ROUTE_PKT_BYTES_MAX -
                             (CPUTRANS_HEADER_BYTES + ENTRY_START_OFS);
        }

        /* Pack the entry */
        cur_offset = ROUTE_PKT_BYTES_MAX - pkt_bytes_left;
        entry_bytes = db_entry_pack(&pkt_buf[cur_offset], entry,
                                    pkt_bytes_left, version);
        if (entry_bytes < 0) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR: cannot pack route entry %d in packet %d\n"),
                       num_entries + 1, cur_pkt_idx));
            ++disc_internal_err;
            ++disc_tot_err;
            return -1;
        }

        pkt_bytes_left -= entry_bytes;
        num_entries++;
        tot_entries++;
        
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc: Packing route entry %d in packet %d, "
                   "buffer left %d\n"),
                   num_entries, cur_pkt_idx, pkt_bytes_left));
    }

    /* Complete setup for last packet, if needed */
    if (num_entries != 0) {
        disc_pkt_setup(pkt_buf + CPUTRANS_HEADER_BYTES,
                       DISC_PKT_TYPE_ROUTING, num_entries, 0,
                       db_ref->local_entry->base.dseq_num, version);
        route_pkt_len[cur_pkt_idx] = ROUTE_PKT_BYTES_MAX - pkt_bytes_left;
    }

    tot_pkts = cur_pkt_idx + 1;
    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Packed %d entries in %d pkts\n"),
               tot_entries, tot_pkts));

    return tot_pkts;
}


/*
 * Send out routing packets to each off board port; may need to send
 * more than one packet to cover all entries in the DB.
 */
STATIC void
routing_pkts_send(cpudb_ref_t db_ref)
{
    int num_pkts;
    int i;

    if (!(disc_flags & DF_TX_KNOWN)) {
        LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc: No route out; no TX known\n")));
        return;
    }

    num_pkts = routing_pkts_form(db_ref);
    if (num_pkts <= 0) {
        return;
    }
    
    DISC_LOCK;
    disc_flags &= ~DF_DB_UPDATED;  /* Clear updated flag */
    DISC_UNLOCK;
    
    for (i = 0; i < num_pkts; i++) {
        route_pkt_tx(db_ref, route_pkt_buf[i], route_pkt_len[i]);
        if (DISC_EXIT) {
            break;
        }
    }
}


STATIC INLINE void
db_stk_entry_pack(uint8 *buf, cpudb_entry_t *entry, int sp, int version)
{
    cpudb_stk_port_t *sp_p;

    sp_p = &entry->sp_info[sp];
    PACK_U32_INCR(buf, sp_p->flags);
    CPUDB_KEY_PACK(buf, sp_p->tx_cpu_key);
    buf += CPUDB_KEY_BYTES;
    PACK_U32_INCR(buf, sp_p->tx_stk_idx);
    CPUDB_KEY_PACK(buf, sp_p->rx_cpu_key);
    buf += CPUDB_KEY_BYTES;
    PACK_U32_INCR(buf, sp_p->rx_stk_idx);
    if (version >= DISCOVERY_VERSION_1) {
        PACK_U32_INCR(buf, entry->base.stk_ports[sp].weight);
        PACK_U32_INCR(buf, entry->base.stk_ports[sp].bflags);
    }
    if (version >= DISCOVERY_VERSION_2) {
        PACK_U32_INCR(buf, entry->base.stk_ports[sp].unit);
        PACK_U32_INCR(buf, entry->base.stk_ports[sp].port);
    }
}


STATIC INLINE int
db_entry_pack(uint8 *buf, cpudb_entry_t *entry, int max_bytes, int version)
{
    int bytes = 0;
    int sp;
    int i;

    bytes = route_entry_bytes_get(entry->base.num_stk_ports,
                                  entry->base.num_units, version);
    if (bytes > max_bytes) {
        return -1;
    }

    CPUDB_KEY_PACK(buf, entry->base.key);
    buf += CPUDB_KEY_BYTES;
    sal_memcpy(buf, entry->base.mac, sizeof(bcm_mac_t));
    buf += sizeof(bcm_mac_t);
    PACK_U32_INCR(buf, entry->base.dseq_num);
    /* coverity[result_independent_of_operands] */
    PACK_U32_INCR(buf, entry->flags & CPUDB_F_FORWARD_MASK);
    PACK_U32_INCR(buf, entry->base.slot_id);
    PACK_U32_INCR(buf, entry->base.master_pri);

    /* Set application data with Board ID and Flag information */
    if (version >= DISCOVERY_VERSION_2) {
#if defined(DISCOVER_APP_DATA_BOARDID)
        uint8 *app_buf = entry->base.app_data;
        PACK_U32_INCR(app_buf, entry->base.board_id);
        PACK_U32_INCR(app_buf, entry->base.flags);
#endif /* DISCOVER_APP_DATA_BOARDID */
    }
    sal_memcpy(buf, entry->base.app_data, CPUDB_APP_DATA_BYTES);
    buf += CPUDB_APP_DATA_BYTES;

    PACK_U32_INCR(buf, entry->base.num_units);
    PACK_U32_INCR(buf, entry->base.dest_unit);
    PACK_U32_INCR(buf, entry->base.dest_port);
    PACK_U32_INCR(buf, entry->base.num_stk_ports);

    for (i = 0; i < entry->base.num_units; i++) {
        PACK_U32_INCR(buf, entry->base.mod_ids_req[i]);
        PACK_U32_INCR(buf, entry->base.pref_mod_id[i]);
    }

    for (sp = 0; sp < entry->base.num_stk_ports; sp++) {
        db_stk_entry_pack(buf, entry, sp, version);
        buf += stk_entry_bytes_get(version);
    }

    return bytes;
}


/****************************************************************
 *
 * Local DB access routines
 */

/*
 * If key exists in CPUDB, return pointer to entry; otherwise add it
 * and return the new entry.  If entry is added, update discovery
 * updated timer.
 */
STATIC cpudb_entry_t *
_disc_key_resolve(cpudb_ref_t db_ref, cpudb_key_t key, bcm_mac_t mac,
                  int dseq_num)
{
    cpudb_entry_t *entry;


    CPUDB_KEY_SEARCH(db_ref, key, entry);

    if (entry != NULL) {
        return entry;
    }

    DISC_LOCK;
    disc_flags |= DF_DB_UPDATED;
    DISC_UNLOCK;
    entry = cpudb_entry_create(db_ref, key, FALSE);
    if (entry == NULL) {
        disc_external_err++;
        ++disc_tot_err;
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Error adding key\n")));
        return NULL;
    }
    sal_memcpy(entry->base.mac, mac, sizeof(bcm_mac_t));
    db_entry_clear_mod_ids(entry);
    entry->base.dseq_num = dseq_num;
    LOG_DEBUG(BSL_LS_TKS_DISCOVER,
              (BSL_META("disc: Added key " CPUDB_KEY_FMT
               "), mac %x:%x seq %d to DB\n"), CPUDB_KEY_DISP(key),
               mac[4], mac[5], dseq_num));

    return entry;
}

/* Return index local stack port arrays if found */
STATIC int
stk_port_find(cpudb_ref_t db_ref, int unit, int port)
{
    int i;

    for (i = 0; i < db_ref->local_entry->base.num_stk_ports; i++) {
        if (unit == STKP_UNIT(db_ref, i) && port == STKP_PORT(db_ref, i)) {
            return i;
        }
    }

    return -1;
}

/* Handles versions 0, 1, and 2 config packets */

STATIC bcm_rx_t
disc_config_pkt_handler_ver(cpudb_key_t src_key,
                        int client_id,
                        bcm_pkt_t *pkt,
                        uint8 *payload,
                        int payload_len,
                        cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;
    uint32 src_dseq_num;
    uint32 master_dseq_num;
    int num_cpus;
    cpudb_key_t master_key;

    /* Called with DISC_LOCK held */
    /* See cfg_header_pack below */
    num_cpus = payload[ENTRY_COUNT_OFS];
    UNPACK_LONG(&payload[SRC_DSEQ_NUM_OFS], src_dseq_num);
    CPUDB_KEY_UNPACK(&payload[CFG_MASTER_KEY_OFS], master_key);
    UNPACK_LONG(&payload[CFG_MSEQ_NUM_OFS], master_dseq_num);

    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("disc: Config pkt in from " CPUDB_KEY_FMT " disc SN %d\n"),
                 CPUDB_KEY_DISP(src_key), src_dseq_num));
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("disc: Cfg pkt master key " CPUDB_KEY_FMT " disc SN %d\n"),
                 CPUDB_KEY_DISP(master_key), master_dseq_num));

    /*
     * Search for the src key entry.
     * Check the discovery sequence numbers agree.
     */
    CPUDB_KEY_SEARCH(db_ref, src_key, entry);
    if (entry == NULL) {
        ++disc_rx_pkt_err;
        ++disc_tot_err;
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Config pkt, bad source key "
                            CPUDB_KEY_FMT_EOLN),
                   CPUDB_KEY_DISP(src_key)));
        DISC_UNLOCK;
        return BCM_RX_HANDLED;
    }
    if (entry->base.dseq_num != src_dseq_num) {
        ++disc_rx_pkt_err;
        ++disc_tot_err;
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc warn: Config pkt from " CPUDB_KEY_FMT
                  "), seq num mismatch: pkt %d, db %d\n"),
                  CPUDB_KEY_DISP(src_key),
                  src_dseq_num, entry->base.dseq_num));
        DISC_UNLOCK;
        return BCM_RX_HANDLED;
    }
    if (db_ref->num_cpus != num_cpus) {
        ++disc_rx_pkt_err;
        ++disc_tot_err;
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc warn: Config pkt from " CPUDB_KEY_FMT
                  "), num CPU mismatch: pkt %d db %d\n"),
                  CPUDB_KEY_DISP(src_key),
                  num_cpus, db_ref->num_cpus));
        DISC_UNLOCK;
        return BCM_RX_HANDLED;
    }

    /* If local is master, this should be a confirmation packet */
    if (db_ref->local_entry == db_ref->master_entry) {
        if (!CPUDB_KEY_EQUAL(master_key, db_ref->local_entry->base.key)) {
            DISC_UNLOCK;
            LOG_WARN(BSL_LS_TKS_DISCOVER,
                     (BSL_META("disc ERR: Config pkt from " CPUDB_KEY_FMT
                      "), master mismatch: pkt " CPUDB_KEY_FMT
                      "), (local master)\n"),
                      CPUDB_KEY_DISP(src_key),
                      CPUDB_KEY_DISP(master_key)));
            return BCM_RX_HANDLED;
        }
        entry->flags |= CPUDB_F_CONFIG_IN;
    } else { /* I'm not the master. */
        if (db_ref->master_entry != NULL &&
            db_ref->master_entry != entry) {
            LOG_WARN(BSL_LS_TKS_DISCOVER,
                     (BSL_META("disc WARN: Config pkt w/ different master; "
                      "overwriting\n")));
        }
        db_ref->master_entry = entry;
        entry->flags |= CPUDB_F_IS_MASTER;
        disc_flags |= DF_SEND_CFG_PKT;
        disc_flags &= ~DF_CFG_PKT_SENT;
    }

    DISC_UNLOCK;
    DISC_WAKE;

    return BCM_RX_HANDLED;
}

STATIC bcm_rx_t
disc_config_pkt_handler(cpudb_key_t src_key,
                        int client_id,
                        bcm_pkt_t *pkt,
                        uint8 *payload,
                        int payload_len,
                        void *cookie)
{
    uint8 d_ver;
    cpudb_ref_t db_ref;

    if (!INIT_DONE) {
        return BCM_RX_NOT_HANDLED;
    }

    DISC_LOCK;

    if (!(disc_flags & DF_DISC_RUNNING) ||
            (disc_flags & DF_DISC_SUCCESS)) {
        DISC_UNLOCK;
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc: Config pkt in when disc_flags are %x.\n"),
                  disc_flags));
        return BCM_RX_NOT_HANDLED;
    }

    db_ref = (cpudb_ref_t)cookie;
    if (!cpudb_valid(db_ref) || (db_ref->local_entry == NULL)) {
        DISC_UNLOCK;
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc: Config pkt in when db_ref is invalid.\n")));
        return BCM_RX_NOT_HANDLED;
    }

    if (!(db_ref->local_entry->flags & CPUDB_F_LOCAL_COMPLETE)) {
        DISC_UNLOCK;
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc: Config pkt in when local DB not complete\n")));
        return BCM_RX_NOT_HANDLED;
    }

    d_ver = payload[DISC_VER_OFS];

    switch (d_ver) {
    case DISCOVERY_VERSION_0:
    case DISCOVERY_VERSION_1:
    case DISCOVERY_VERSION_2:
        return disc_config_pkt_handler_ver(src_key, client_id, pkt,
                                           payload, payload_len, db_ref);

    }

    /* Unknown version - throw packet away */
    DISC_UNLOCK;
    LOG_WARN(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc: Unknown discovery version %d in config\n"),
              d_ver));
    return BCM_RX_HANDLED;
}

STATIC void
cfg_header_pack(cpudb_ref_t db_ref, uint8 *buf)
{
    buf += CPUTRANS_HEADER_BYTES;
    disc_pkt_setup(buf, DISC_PKT_TYPE_CONFIG, db_ref->num_cpus,
                   0, db_ref->local_entry->base.dseq_num, disc_version);
    buf += DISC_HEADER_BYTES;
    CPUDB_KEY_PACK(buf, db_ref->master_entry->base.key);
    buf += CPUDB_KEY_BYTES;
    PACK_LONG(buf, db_ref->master_entry->base.dseq_num);
    buf += sizeof(uint32);
}

/*
 * Send out config pkts to each other CPUs in DB; if master, send to
 * all other CPUs.  Otherwise, just direct to the master.
 *
 * Returns BCM_E_XXX
 */
STATIC int
disc_config_send(cpudb_ref_t db_ref)
{
    uint8 *buf, *start_buf;
    int len;
    int rv = BCM_E_NONE;
    cpudb_entry_t *entry;

    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("DISC:  Generating config packet(s)\n")));

    if (!cpudb_valid(db_ref) || (db_ref->master_entry == NULL)) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc WARN: cfg send; master is NULL\n")));
        return BCM_E_PARAM;
    }

    if (!(LOCAL_FLAGS(db_ref) & CPUDB_F_LOCAL_COMPLETE)) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("disc WARN: cfg send, but not local complete\n")));
    }

    len = CPUTRANS_HEADER_BYTES + CFG_HEADER_BYTES;
    buf = start_buf = DATA_SALLOC(len);
    if (buf == NULL) {
        ++disc_alloc_fail;
        ++disc_tot_err;
        return BCM_E_MEMORY;
    }
    cfg_header_pack(db_ref, buf);

    if (db_ref->local_entry != db_ref->master_entry) {
        /* Send only to master */
        rv = atp_tx(db_ref->master_entry->base.key,
                    DISC_CONFIG_CLIENT_ID,
                    start_buf,
                    len,
                    CPUTRANS_NO_HEADER_ALLOC,
                    NULL,
                    NULL);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc ERR:  %s (%d) Failed to tx cfg pkt to "
                                CPUDB_KEY_FMT_EOLN),
                       bcm_errmsg(rv), rv,
                       CPUDB_KEY_DISP(db_ref->master_entry->base.key)));
        }
    } else {
        CPUDB_FOREACH_ENTRY(db_ref, entry) {
            if (entry == db_ref->local_entry) {
                continue;
            }

            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc: Cfg pkt out to " CPUDB_KEY_FMT_EOLN),
                       CPUDB_KEY_DISP(entry->base.key)));
            rv = atp_tx(entry->base.key,
                        DISC_CONFIG_CLIENT_ID,
                        start_buf,
                        len,
                        CPUTRANS_NO_HEADER_ALLOC,
                        NULL,
                        NULL);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_TKS_DISCOVER,
                          (BSL_META("disc ERR:  %s (%d) Failed to tx pkt to "
                                    CPUDB_KEY_FMT_EOLN),
                           bcm_errmsg(rv), rv,
                           CPUDB_KEY_DISP(entry->base.key)));
                break;
            }
        }
    }

    DATA_SFREE(start_buf);
    return BCM_E_NONE;
}


/*
 * Function:
 *      disc_m_elect_default
 * Purpose:
 *      Default master election function
 * Parameters:
 *      db_ref          - The DB to analyze
 * Returns:
 *      DISC_RESTART_NEW_SEQ is supported
 *      BCM_E_XXX
 * Notes:
 *      Used if no other master elect is provided.
 *      Only set the local master if "global complete" is set.  This ensures
 *      info is spread to other CPUs.
 *
 *      If local is not master, set master_entry so that discovery will
 *      register to receive configuration packets
 *
 *      Uses highest master priority/lowest key as master;
 */

STATIC int
disc_m_elect_default(cpudb_ref_t db_ref, void *user_data)
{
    cpudb_entry_t *entry, *best_entry;
    int high_priority;

    COMPILER_REFERENCE(user_data);

    if (db_ref == CPUDB_REF_NULL || db_ref->local_entry == NULL) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc m_elect ERR:  Bad DB or local entry\n")));
        return BCM_E_FAIL;
    }

    /*
     * First, find the highest priority in the DB;
     * Then find the lowest key of those with that priority
     */
    best_entry = db_ref->local_entry;
    high_priority = db_ref->local_entry->base.master_pri;
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry == db_ref->local_entry) {
            continue;
        }
        if (entry->base.master_pri > high_priority) {
            best_entry = entry;
            high_priority = entry->base.master_pri;
        }
    }

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (entry == db_ref->local_entry || entry == best_entry) {
            continue;
        }
        if (entry->base.master_pri == high_priority) {
            if (CPUDB_KEY_COMPARE(entry->base.key,
                                  best_entry->base.key) < 0) {
                best_entry = entry;
            }
        }
    }

    if (best_entry == db_ref->local_entry) {    /* See notes */
        if (!(LOCAL_FLAGS(db_ref) & CPUDB_F_GLOBAL_COMPLETE)) {
            LOG_DEBUG(BSL_LS_TKS_DISCOVER,
                      (BSL_META("disc m_elect:  "
                       "Local is master; wait for global complete\n")));
            return BCM_E_NONE;
        }
    }

    if (db_ref->master_entry != NULL && db_ref->master_entry != best_entry) {
        LOG_WARN(BSL_LS_TKS_DISCOVER,
                 (BSL_META("DISC m_elect:  Master set, but changing\n")));
    }

    db_ref->master_entry = best_entry;
    best_entry->flags |= CPUDB_F_IS_MASTER;
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("disc m_elect: Master is "
                          CPUDB_KEY_FMT ", %slocal\n"),
                 CPUDB_KEY_DISP(best_entry->base.key),
                 best_entry == db_ref->local_entry ? "" : "not "));
    return BCM_E_NONE;
}

#if defined(BROADCOM_DEBUG)
void
disc_counter_dump(void)
{
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_alloc_fail    = %d\n"),
              disc_alloc_fail));
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_resource_err  = %d\n"),
              disc_resource_err));
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_internal_err  = %d\n"),
              disc_internal_err));
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_tx_pkt_err    = %d\n"),
              disc_tx_pkt_err));
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_rx_pkt_err    = %d\n"),
              disc_rx_pkt_err));
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_tot_err       = %d\n"),
              disc_tot_err));
    LOG_INFO(BSL_LS_TKS_DISCOVER,
             (BSL_META("disc_external_err  = %d\n"),
              disc_external_err));
}
#endif /* BROADCOM_DEBUG */

static void
disc_status_set(disc_status_t status)
{
    sal_mutex_take(disc_status_lock, sal_mutex_FOREVER);
    disc_stat = status;
    sal_mutex_give(disc_status_lock);
}

/*
 * Function:
 *     int disc_election_register(*disc_election_cb_t m_elect,
 *                                void *user_data)
 * Purpose:
 *     Register a master election function
 * Parameters:
 *     m_elect - election function
 *     user_data - currently ignored
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_PARAM    - m_elect is NULL
 * Notes:
 *     Only one registration at a time, and each new registration
 *     overwrites the previous one.
 */

int
disc_election_register(disc_election_cb_t m_elect, void *user_data)
{
    int rv = BCM_E_PARAM;

    if (m_elect != NULL) {
        disc_m_elect = m_elect;
        disc_m_user_data = user_data;

        /* Don't use an old callback interface */
        disc_start_elect = NULL;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     disc_election_unregister(*disc_election_cb_t m_elect,
 *                              void *user_data)
 * Purpose:
 *     Deregister a master election function
 * Parameters:
 *     m_elect - election function
 *     user_data - currently ignored
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_PARAM    - m_elect and/or user_data was not initially registered
 * Notes:
 *     Election reverts to default election function
 */

int
disc_election_unregister(disc_election_cb_t m_elect, void *user_data)
{
    int rv = BCM_E_PARAM;

    if (disc_m_elect == m_elect && disc_m_user_data == user_data) {
        disc_m_elect = disc_m_elect_default;
        disc_m_user_data = NULL;
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      disc_run
 * Purpose:
 *      Run the discovery process
 * Parameters:
 *      db_ref       - The database reference to use; see notes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This routine is not re-entrant.
 *
 *      The discovery sequence number will be extracted from the local
 *      CPU DB entry.
 */
STATIC int
_disc_run_prep(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;

    DISC_LOCK;
    if (disc_flags & DF_DISC_RUNNING) {
        DISC_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("_disc_run_prep: terminating existing disc\n")));
        if (disc_abort(BCM_E_FAIL, 500000) != BCM_E_NONE) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("%s: disc_abort failed.\n"),
                         FUNCTION_NAME()));
            return BCM_E_BUSY;
        }
        DISC_LOCK;
    } else {
        /* reset internal flags */
        disc_flags = 0;
    }

    if (disc_trans_ptr == NULL) {
        DISC_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("_disc_run ERR: Need transport pointers\n")));
        return BCM_E_FAIL;
    }

    if (db_ref == NULL) {
        DISC_UNLOCK;
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("%s ERR: Need DB ref pointers\n"),
                   FUNCTION_NAME()));
        return BCM_E_PARAM;
    }

#if defined(BROADCOM_DEBUG)
    rv = _check_start_values(db_ref);
    if (BCM_FAILURE(rv)) {
        DISC_UNLOCK;
        return rv;
    }
#endif  /* BROADCOM_DEBUG */

    rv = disc_prep(db_ref);
    DISC_UNLOCK;

    return rv;
}

/*
 * Function:
 *
 * Purpose:
 *      Register discovery packet processing
 * Parameters:
 *      db_ref       - The database reference to use; see notes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This routine is not re-entrant.
 *
 *      The discovery sequence number will be extracted from the local
 *      CPU DB entry.
 */
STATIC int
_disc_run_reg(cpudb_ref_t *db_refp)
{
    int rv = BCM_E_NONE;

    /* Register with next hop */
    rv = next_hop_register(disc_rx_pkt, db_refp, DISC_NH_PKT_TYPE);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Could not register RX callback")));
        DISC_LOCK;
        disc_flags |= DF_DISC_ERROR;
        DISC_UNLOCK;
    } else {
        LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                    (BSL_META("disc: registered with nexthop\n")));
    }
    return rv;
}

/*
 * Function:
 *      _disc_run_unreg
 * Purpose:
 *      Unregister discovery packet processing
 * Parameters:
 *      none
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_disc_run_unreg(void)
{
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("disc: unregistered with nexthop\n")));
    return next_hop_unregister(disc_rx_pkt, DISC_NH_PKT_TYPE);
}

/*
 * Function:
 *      _disc_run_loop
 * Purpose:
 *      Procoess discovery packets until discovery termination
 * Parameters:
 *      db_ref       - The database reference to use; see notes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_disc_run_loop(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;

    /* Main discovery loop */
    while (TRUE) {
        DISC_SLEEP(disc_retrx_us);
        DISC_LOCK;
        disc_flags &= ~DF_IGNORE_PACKETS;  /* Time to check pkts */
        DISC_UNLOCK;

        disc_status_update(db_ref);
        if (disc_done_check(db_ref, &rv, FALSE)) {
            break;
        }

        disc_pkts_process(db_ref);  /* incoming and outgoing pkts */
        if (disc_done_check(db_ref, &rv, FALSE)) {
            break;
        }

        disc_status_update(db_ref);
        if (disc_done_check(db_ref, &rv, FALSE)) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *      _disc_run_complete
 * Purpose:
 *      Mark completion of the discovery process
 * Parameters:
 *      db_ref       - The database reference to use; see notes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_disc_run_complete(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;

    DISC_LOCK;
    disc_flags |= DF_ABORT_ACK;
    DISC_UNLOCK;

    return rv;
}

/*
 * Function:
 *      _disc_run
 * Purpose:
 *      Run the discovery process once through
 * Parameters:
 *      db_ref       - The database reference to use; see notes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This routine is not re-entrant.
 *
 *      The discovery sequence number will be extracted from the local
 *      CPU DB entry.
 */
STATIC int
_disc_run(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;

    DISC_INIT_CHECK;

    BCM_IF_ERROR_RETURN(_disc_run_prep(db_ref));

    if (!DISC_TASK) {
        BCM_IF_ERROR_RETURN(_disc_run_reg(&db_ref));
    }

    rv = _disc_run_loop(db_ref);

    if (!DISC_TASK) {
        BCM_IF_ERROR_RETURN(_disc_run_unreg());
    }

    BCM_IF_ERROR_RETURN(_disc_run_complete(db_ref));

    return rv;
}

static int _disc_sequence_number;


STATIC int
_disc_run_restart_loop(cpudb_ref_t db_ref)
{
    int rv = BCM_E_NONE;
    int co_rv = BCM_E_NONE;

    do {
        /* clear any non-local entries, either from the cpudb supplied,
           or a populated cpudb from a restart. */
        cpudb_clear(db_ref, TRUE);

        /* set sequence number */
        if (rv != DISC_RESTART_REQUEST) {
            CUR_DSEQ_NUM(db_ref) = _disc_sequence_number++;
        }

        /* run discovery */
        rv = _disc_run(db_ref);
        co_rv = disc_callout(db_ref, rv);


        /* Keep looping while there is a restart return value from
           _disc_run() and the callout has allowed the restart. */

    } while ( (rv == DISC_RESTART_REQUEST || rv == DISC_RESTART_NEW_SEQ) &&
              BCM_SUCCESS(co_rv) );

    return rv;
}

STATIC int
disc_callout(cpudb_ref_t db_ref, int status)
{
    int rv = BCM_E_PARAM;

    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("disc: callout status:%d\n"),
                 status));
    if (_disc_callback.callback) {
        rv = _disc_callback.callback(db_ref, status, _disc_callback.user_data);
    }
    LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                (BSL_META("disc: callout returns %d\n"),
                 rv));

    return rv;
}

/*
 * Discovery thread for asynchronous callback
 */
STATIC void
_disc_run_thread(void *user_data)
{
    do {
        disc_status_set(DISC_STATUS_TASK_IDLE);
        DISC_TASK_SLEEP;

        if (disc_task_cmd == DISC_TASK_CMD_RUN) {
            (void)_disc_run_restart_loop(disc_task_db);
        }
    } while (disc_task_cmd == DISC_TASK_CMD_RUN);
    disc_status_set(DISC_STATUS_INACTIVE);

    _disc_tid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Start discovery thread.
 */
STATIC int
disc_run_task(void)
{
    int  rv = BCM_E_NONE;

    if ((_disc_tid = sal_thread_create("bcmDISC",
                                       DISC_THREAD_STACK,
                                       DISC_THREAD_PRIORITY,
                                       _disc_run_thread,
                                       NULL))
        == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_TKS_DISCOVER,
                  (BSL_META("disc ERR: Could not create discovery thread\n")));
        rv = BCM_E_FAIL;
    }
    return rv;
}

/*
 * Function:
 *     int disc_run(cpudb_ref_t db_ref)
 * Purpose:
 *     Initiate the discovery protocol
 * Parameters:
 *     db_ref - CPUDB containing local entry
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_BUSY     - the discovery protocol is currently running.
 *     BCM_E_INIT     - module uninitialized
 * Notes:
 *     Discovery completion status is returned via a discovery
 *     completion callback registered by disc_register()
 *
 *     Only one instance of discovery, regardless of the mode,
 *     can be running at one time.
 *
 *     CPUDB is cleared to just the local entry.
 */

int
disc_run(cpudb_ref_t db_ref)
{
    int  rv = BCM_E_INTERNAL;
    cpudb_ref_t disc_db;

    if (!_disc_callback.callback) {
        return BCM_E_CONFIG;
    }

    sal_mutex_take(disc_status_lock, sal_mutex_FOREVER);
    {
        if (disc_stat == DISC_STATUS_INACTIVE) {
            rv = BCM_E_INIT;
        } else if (disc_stat != DISC_STATUS_TASK_IDLE) {
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("disc_run busy = %d @ %d\n"),
                         disc_stat, sal_time_usecs()));
            rv = BCM_E_BUSY;
        } else {
            rv = BCM_E_NONE;
            disc_stat = DISC_STATUS_TASK_RUNNING;
        }
    }
    sal_mutex_give(disc_status_lock);

    if (BCM_SUCCESS(rv)) {
        disc_db = cpudb_copy(db_ref);
        if (disc_db) {
            disc_task_db = disc_db;
            LOG_VERBOSE(BSL_LS_TKS_DISCOVER,
                        (BSL_META("disc_run @ %d\n"),
                         sal_time_usecs()));
            DISC_TASK_WAKE;
            sal_thread_yield();
        } else {
            rv = BCM_E_MEMORY;
        }
    }

    return rv;
}


/*
 * Function:
 *     disc_register(disc_cb_t callback, void *user_data)
 * Purpose:
 *     Register a discovery completion callback
 * Parameters:
 *     callback - function to call on completion
 *     user_data - user data passed to callback
 * Returns:
 *     BCM_E_PARAM    - callback is NULL
 *     BCM_E_NONE     - no errors
 * Notes:
 *     Only one registration allowed at a time. New registrations
 *     overwrite previous registrations.
 *
 *     Callback status:
 *
 *     BCM_E_NONE
 *
 *       Discovery has completed successfully.
 *
 *     BCM_E_TIMEOUT
 *
 *       Discovery has timed out while trying to complete.
 *
 *     BCM_E_FAIL
 *     
 *       Discovery has failed for some other reason.
 *
 *     DISC_RESTART_REQUEST
 *
 *       Discovery requests restart with same sequence number.  Request
 *       is accepted by returning BCM_E_NONE, denied by returning
 *       BCM_E_FAIL from the status callback.
 *
 *     DISC_RESTART_NEW_SEQ
 *
 *       Discovery requests restart with new sequence number.
 *       Request is accepted by returning BCM_E_NONE, denied by
 *       returning BCM_E_FAIL from the status callback.
 *
 *     DISC_PROBE_RECEIVED
 *
 *       A probe packet has been received which the discovery
 *       process is idle.
 *
 *       This will not automatically initiate discovery - the event
 *       callback is responsible for calling or scheduling
 *       disc_run() if discovery needs to be initiated. Called from
 *       a packet receive callback context.
 */

int
disc_register(disc_cb_t callback, void *user_data)
{
    int rv = BCM_E_PARAM;

    if (callback) {
        _disc_callback.callback = callback;
        _disc_callback.user_data = user_data;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     disc_unregister(disc_cb_t callback, void *user_data)
 * Purpose:
 *     Unregister a discovery completion callback
 * Parameters:
 *     callback - function to unregister
 *     user_data - user data passed to callback
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_PARAM    - callback and/or user_data was not initially registered
 * Notes:
 */

int
disc_unregister(disc_cb_t callback, void *user_data)
{
    int rv = BCM_E_PARAM;

    if (_disc_callback.callback  == callback &&
        _disc_callback.user_data == user_data) {
        _disc_callback.callback = NULL;
        _disc_callback.user_data = NULL;
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     disc_init(void)
 * Purpose:
 *     Initialize discovery subsystem.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE     - no errors
 *     BCM_E_FAIL     - Discovery thread failed to initialize
 *     BCM_E_MEMORY   - not enough memory
 * Notes:
 *     Does not initiate discovery protocol.
 *     Not required if application just uses disc_start().
 */

int
disc_init(void)
{
    int rv = BCM_E_INTERNAL;

    if (disc_task_sem) {
        disc_deinit();
    }
    
    DISC_INIT_CHECK;
    
    if (disc_task_sem != NULL) {
        sal_sem_destroy(disc_task_sem);
    }

    disc_task_sem = sal_sem_create("disc_task_sem", sal_sem_BINARY, 0);

    if (disc_task_sem != NULL) {
        disc_task_cmd = DISC_TASK_CMD_RUN;
        rv = disc_run_task();
        if (BCM_SUCCESS(rv)) {
            disc_stat = DISC_STATUS_TASK_IDLE;
            rv = _disc_run_reg(&disc_task_db);
        }
    } else {
        rv = BCM_E_MEMORY;
    }

    if (BCM_FAILURE(rv)) {
        (void)disc_deinit();
    }

    return rv;
}


/*
 * Function:
 *     int disc_deinit(void)
 * Purpose:
 *     Shutdown discovery and deallocate resources.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE     - no errors
 * Notes:
 */

#ifndef DISC_DEINIT_RETRY
#define DISC_DEINIT_RETRY 5
#endif

#ifndef DISC_DEINIT_SLEEP
#define DISC_DEINIT_SLEEP 10000
#endif


int
disc_deinit(void)
{
    disc_status_t stat;
    int i;

    for ( i=0; i<DISC_DEINIT_RETRY; i++ ) {
        BCM_IF_ERROR_RETURN(disc_status_get(&stat));

        /* Stop running disc protocol */
        if (stat == DISC_STATUS_TASK_RUNNING ||
            stat == DISC_STATUS_SYNC_RUNNING) {
            disc_abort(BCM_E_FAIL, 0);
            sal_thread_yield();
            sal_usleep(DISC_DEINIT_SLEEP);
        } else {
            break;
        }
    }

    if (DISC_TASK) {
        (void) _disc_run_unreg();
        /* Signal thread to stop */
        disc_task_cmd = DISC_TASK_CMD_STOP;
        DISC_TASK_WAKE;
        /* May need to take extra steps if thread doesn't exit */
    }

    if (disc_task_sem) {
         sal_sem_destroy(disc_task_sem);
    }

    if (disc_sem) {
         sal_sem_destroy(disc_sem);
    }

    if (disc_lock) {
         sal_mutex_destroy(disc_lock);
    }

    if (disc_status_lock) {
         sal_mutex_destroy(disc_status_lock);
    }

    return BCM_E_NONE;
}


