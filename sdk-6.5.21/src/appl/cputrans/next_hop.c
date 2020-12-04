/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        next_hop.c
 *
 * Purpose:
 *    Implements next hop broadcast.  See next_hop_trans.txt for
 *    more information.
 *
 * Requires:    nh_tx, cputrans
 *
 * Notes:
 *    This module provides a broadcast mechanism allowing one CPU to send
 *    to all others in the system.  It uses the next hop packet transport
 *    sending.  It is not reliable.  All CPUs in the system must have this
 *    running in order for packets to be properly forwarded.
 *
 *    When a NEXT_HOP packet is received, the source CPU KEY and sequence
 *    number are examined.  If the packet has been seen before, it is
 *    discarded.  Otherwise, it is queued for local processing.  If the
 *    packet is a broadcast or if the unicast address is unknown, then
 *    the packet is also forwarded out all local stack ports.
 *
 *    If the source CPU is unknown, it is added to the local (ie,
 *    local to this file) database.  If the DB is full, the variable
 *    key_lru_replace_enable indicates whether the least recently used
 *    CPU key in the data base should be replaced with the new
 *    key.
 *
 *    Future Enhancement:  Permit a callback to be registered that
 *    passes the old and new CPU keys when a replacement occurs.
 *
 *    Stack ports may be marked as "duplex".  Packets received on a duplex
 *    port will not be forwarded back out that port.  It is not required
 *    that ports be marked as duplex:  If a packet is sent back on a duplex
 *    port, the receiving CPU should discard the packet.
 *
 *    Packets may be directed to a specific CPU by KEY.  Currently,
 *    no attempt is made to expedite the routing; the packet still gets
 *    flooded to all CPUs; the difference is that only packets with
 *    the local KEY or broadcast KEY as destination are sent up the
 *    local stack (local callback is made).  In the future, routing
 *    may be implemented so that the packet is only sent out a single
 *    stack port when possible.
 *
 *    This module uses the tx packet allocation routines implemented in
 *    cputrans.
 *
 *    Loop-back is not supported at this layer.
 */



#include <shared/bsl.h>

#include <assert.h>

#include <shared/idents.h>

#include <sal/core/sync.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <shared/alloc.h>
#include <sal/core/time.h>

#include <bcm/types.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <appl/cputrans/nh_tx.h>
#include <appl/cputrans/next_hop.h>
#include <appl/cputrans/cputrans.h>

#include <appl/cpudb/cpudb.h>

#include "t_util.h"

#define STK_PORTS_MAX	CPUDB_CXN_MAX

/****************************************************************
 *
 * Internal Synchronization
 */

static sal_mutex_t next_hop_lock;
static sal_mutex_t next_hop_reg_lock;
static sal_mutex_t next_hop_unit_lock;
static sal_sem_t next_hop_sem;

#define NEXT_HOP_INIT if (next_hop_lock == NULL) \
    BCM_IF_ERROR_RETURN(_next_hop_init())

#define NEXT_HOP_LOCK sal_mutex_take(next_hop_lock, sal_mutex_FOREVER)
#define NEXT_HOP_UNLOCK sal_mutex_give(next_hop_lock)

#define NEXT_HOP_REG_LOCK \
    sal_mutex_take(next_hop_reg_lock, sal_mutex_FOREVER)
#define NEXT_HOP_REG_UNLOCK sal_mutex_give(next_hop_reg_lock)

#define NEXT_HOP_UNIT_LOCK \
    sal_mutex_take(next_hop_unit_lock, sal_mutex_FOREVER)
#define NEXT_HOP_UNIT_UNLOCK sal_mutex_give(next_hop_unit_lock)

/* The Destination KEY address in the NEXT_HOP header */
#define NEXT_HOP_DEST_KEY_SET(pkt_data, key) \
    CPUDB_KEY_PACK(&((pkt_data)[CPUTRANS_DEST_KEY_OFS]), key)
#define NEXT_HOP_DEST_KEY_GET(pkt_data, key) \
    CPUDB_KEY_UNPACK(&((pkt_data)[CPUTRANS_DEST_KEY_OFS]), key)

/* KEY extraction:  The KEY address stored in the NEXT_HOP header */
#define NEXT_HOP_SRC_KEY_SET(pkt_data, key) \
    CPUDB_KEY_PACK(&((pkt_data)[CPUTRANS_SRC_KEY_OFS]), key)
#define NEXT_HOP_SRC_KEY_GET(pkt_data, key) \
    CPUDB_KEY_UNPACK(&((pkt_data)[CPUTRANS_SRC_KEY_OFS]), key)

#define NEXT_HOP_SEQ_NUM_SET(pkt_data, val) \
    PACK_SHORT(&((pkt_data)[CPUTRANS_NH_SEQ_NUM_OFS]), val)
#define NEXT_HOP_SEQ_NUM_GET(pkt_data, val) \
    UNPACK_SHORT(&((pkt_data)[CPUTRANS_NH_SEQ_NUM_OFS]), val)

#define NEXT_HOP_MPLX_NUM_SET(pkt_data, val) \
    PACK_LONG(&((pkt_data)[CPUTRANS_NH_MPLX_NUM_OFS]), val)
#define NEXT_HOP_MPLX_NUM_GET(pkt_data, val) \
    UNPACK_LONG(&((pkt_data)[CPUTRANS_NH_MPLX_NUM_OFS]), val)

/* Init signals and synchronization */
static volatile int setup_done;  /* Are pointers set up? */
static volatile int next_hop_exit;   /* Force thread to exit */
static volatile sal_thread_t nh_thread_id = SAL_THREAD_ERROR;

/****************************************************************
 *
 * Configuration information
 *
 *    next_hop_local_key      - Local CPU's KEY addr for nexthop
 *    next_hop_trans_ptr      - Transport pointer structure
 *    next_hop_mtu            - Max pkt size supported in bytes
 */

static cpudb_key_t next_hop_local_key;
static bcm_trans_ptr_t *next_hop_trans_ptr = NEXT_HOP_TRANS_PTR_DEFAULT;
static int next_hop_thread_priority = NEXT_HOP_THREAD_PRIORITY_DEFAULT;
static int next_hop_rx_priority = NEXT_HOP_RX_PRIORITY_DEFAULT;

static int next_hop_mtu = NEXT_HOP_MTU_DEFAULT;

#define NEXT_HOP_VLAN_DEFAULT 1
#define NEXT_HOP_COS_DEFAULT 0

static int nh_vlan = NEXT_HOP_VLAN_DEFAULT;
static int nh_cos = NEXT_HOP_COS_DEFAULT;

static int next_hop_rx_queue_size = NEXT_HOP_RX_QUEUE_SIZE;
static int next_hop_tx_queue_size = NEXT_HOP_TX_QUEUE_SIZE;

/****************************************************************
 *
 * Stack port management
 */

/*
 * Local stack ports.  (unit, port) and flag to indicate duplex
 */
static struct stk_port_t {
    int unit;
    int port;
    uint32 flags;
#define NH_FLAGS_DUPLEX 0x1
} nh_stk_ports[STK_PORTS_MAX];
static int num_stk_ports;

/* Keep track of which local units we're registered on */
static int units[STK_PORTS_MAX];
static int num_units;


/****************************************************************
 *
 * Callback management
 */

/*
 * Handler list.  Unsorted.  Added in order of registration.
 */
typedef struct cb_ctl_s {
    next_hop_rx_callback_f callback;
    int mplx_num;
    void *cookie;
} cb_ctl_t;
static cb_ctl_t cb_ctl[NEXT_HOP_CALLBACK_MAX];
static int num_cb_ctl;


/****************************************************************
 *
 * TX/RX queue definitions
 *     The RX queue holds packets
 *     The TX queue needs a little more info, so we define the
 *     following structure.
 *
 *     For TX entries, if the _F_PERSISTENT flag is not set, then
 *     the packet will be deallocated when the trx is complete.
 */

typedef struct tx_queue_s tx_queue_t;
struct tx_queue_s {
    bcm_pkt_t *pkt;                     /* Packet to be transmitted */
    next_hop_tx_callback_f callback;    /* Callback when trx is done */
    void *cookie;                       /* Passed to callback */
    uint32 flags;
#define PACKET_F_PERSISTENT    0x1      /* Keep pkt until destroyed */
    volatile tx_queue_t *next;          /* For linked lists. */
};

static tx_queue_t *tx_queue_alloc_ptr;
static volatile tx_queue_t *tx_queue_freelist;
static volatile tx_queue_t *tx_queue;
static volatile tx_queue_t *tx_queue_tail;

static bcm_pkt_t *rx_queue_alloc_ptr;
static volatile bcm_pkt_t *rx_queue;
static volatile bcm_pkt_t *rx_queue_tail;
static volatile bcm_pkt_t *rx_queue_freelist;

/*
 * Per system CPU information.
 * Circular queues of sequence numbers per CPU; use local index here.
 * The indexes are purely local; we want next hop to work independent of
 * what CPUDBs might exist and what their states might be.
 */
#define CPU_KEY_MAX (2 * CPUDB_CPU_MAX)
typedef struct {
    cpudb_key_t key;    /* The key to use in addressing CPU */

    int tx_unit;        /* Not yet used; for directed pkts */
    int tx_port;

    uint16 seq_nums_seen[NEXT_HOP_SEQ_NUM_TRACK];
    int seq_num_last;   /* Index in seq_nums_seen where latest entry put */
    sal_time_t last_ref;    /* Last time CPU was referenced; for LRU */
} cpu_seq_list_t;

static cpu_seq_list_t *cpu_seq_list[CPU_KEY_MAX];
/* Allow KEY add to replace least-recently-used entry if full */
static int key_lru_replace_enable = FALSE;

static int rx_pkt_drop_count;
static int tx_error_count;

/****************************************************************
 *
 * Forward declarations
 */

STATIC void next_hop_thread(void *cookie);
STATIC void _next_hop_cleanup(void);
STATIC bcm_rx_t next_hop_rx_callback(int unit, bcm_pkt_t *pkt, void *cookie);
STATIC int _next_hop_init(void);
static INLINE int _nh_key_find(cpudb_key_t key);
static INLINE int _nh_key_add(cpudb_key_t key);
STATIC int _port_add(int unit, int port, int duplex, int *reg);


/****************************************************************
 *
 * Start and Stop API
 */

/*
 * Function:
 *      
 * Purpose:
 *      
 * Parameters:
 *      thread_priority  - Priority to start task at; < 0 -> use default
 *      rx_priority      - RX Register priority; < 0 -> use default
 *      local_key        - Local CPU key to use to identify this CPU
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Cannot be called when next hop is running.
 */


int
next_hop_config_set(bcm_trans_ptr_t *trans_ptr,
                    int thread_priority,
                    int rx_priority)
{
    if (setup_done) {
        return BCM_E_BUSY;
    }

    if (trans_ptr != NULL) {
        if (trans_ptr->tp_data_free == NULL) {
            return BCM_E_PARAM;
        }
        next_hop_trans_ptr = trans_ptr;
    }

    if (thread_priority >= 0) {
        next_hop_thread_priority = thread_priority;
    }

    if (rx_priority >= 0) {
        next_hop_rx_priority = rx_priority;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      
 * Purpose:
 *      
 * Parameters:
 *      thread_priority  - Priority to start task at; < 0 -> use default
 *      rx_priority      - RX Register priority; < 0 -> use default
 *      local_key        - Local CPU key to use to identify this CPU
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
next_hop_config_get(bcm_trans_ptr_t **trans_ptr,
                    int *thread_priority,
                    int *rx_priority)
{

    if (trans_ptr != NULL) {
        *trans_ptr = next_hop_trans_ptr;
    }

    if (thread_priority != NULL) {
        *thread_priority = next_hop_thread_priority;
    }

    if (rx_priority != NULL) {
        *rx_priority = next_hop_rx_priority;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_(cos,vlan)_(set,get)
 * Purpose:
 *      Set/get the COS, internal priority and VLAN settings used by
 * Notes:
 *      The RX subsystem and chip COS configurations must be
 *      set up consistently with the values used here; similarly
 *      for the VLAN settings.
 *
 *      These values are only used when -1 is specified to the
 *      tx/pkt_create functions for cos/vlan parameters.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
next_hop_cos_set(int cos)
{
    nh_cos = cos;

    return BCM_E_NONE;
}

int
next_hop_cos_get(int *cos)
{
    *cos = nh_cos;

    return BCM_E_NONE;
}

int
next_hop_vlan_set(int vlan)
{
    nh_vlan = vlan;

    return BCM_E_NONE;
}

int
next_hop_vlan_get(int *vlan)
{
    *vlan = nh_vlan;

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_queue_size_set
 * Purpose:
 *      Set the RX and TX queue sizes.
 * Parameters:
 *      rx_size - RX queue size:
 *                If value is positive, set new value
 *                If value is negative, do not change current value
 *      tx_size - TX queue size:
 *                If value is positive, set new value
 *                If value is negative, do not change current value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      API cannot be called when next hop is running.
 */
int
next_hop_queue_size_set(int rx_size, int tx_size)
{
    if (setup_done) {
        return BCM_E_BUSY;
    }

    if ((rx_size == 0) || (tx_size == 0)) {
        return BCM_E_PARAM;
    }

    if (rx_size > 0) {
        next_hop_rx_queue_size = rx_size;
    }
    if (tx_size > 0) {
        next_hop_tx_queue_size = tx_size;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      next_hop_queue_size_get
 * Purpose:
 *      Get the RX and TX queue sizes.
 * Parameters:
 *      rx_size - (OUT) If non-null, returns RX queue size
 *      tx_size - (OUT) If non-null, returns TX queue size
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
next_hop_queue_size_get(int *rx_size, int *tx_size)
{
    if (rx_size != NULL) {
        *rx_size = next_hop_rx_queue_size;
    }
    if (tx_size != NULL) {
        *tx_size = next_hop_tx_queue_size;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_start
 * Purpose:
 *      Start Next Hop broadcast packet reception
 * Parameters:
 *      local_base        - Base information about local CPU
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *
 *      This function does not add stack ports by default; some
 *      external agent must register stack ports with next hop either
 *      before or after this function is called.  This is because
 *      there may be other qualifiers that make a stack port listed in
 *      the base configuration ineligible to receive next hop packets,
 *      such as the port being in a non-stacking mode, or the port
 *      does not have link.
 */

int
next_hop_start(const cpudb_base_t *local_base)
{
    int rv;
    int i;

    if (setup_done) {  /* Stop before restart */
        next_hop_stop();
    }

    if (!cputrans_tx_setup_done()) {
        BCM_IF_ERROR_RETURN(cputrans_tx_pkt_setup(-1, next_hop_trans_ptr));
    }

    NEXT_HOP_INIT;
    NEXT_HOP_LOCK;

    nh_tx_setup(next_hop_trans_ptr);

    CPUDB_KEY_COPY(next_hop_local_key, local_base->key);
    nh_tx_local_mac_set(local_base->mac);

    nh_thread_id = sal_thread_create("bcmNHOP", SAL_THREAD_STKSZ,
				     next_hop_thread_priority,
				     next_hop_thread, NULL);
    if (nh_thread_id == SAL_THREAD_ERROR) {
        NEXT_HOP_UNLOCK;
        return BCM_E_RESOURCE;
    }

    /* Register with all known units; may be redundant with above */
    for (i = 0; i < num_units; i++) {
        rv = cputrans_rx_unit_register(units[i], "next_hop",
                                       next_hop_rx_callback,
                                       next_hop_rx_priority, NULL,
                                       BCM_RCO_F_ALL_COS);
        if (rv < 0) {
            cputrans_rx_unregister(next_hop_rx_callback,
                                   next_hop_rx_priority);
            NEXT_HOP_UNLOCK;
            return rv;
        }
    }

    tx_error_count = 0;
    rx_pkt_drop_count = 0;
    setup_done = TRUE;

    NEXT_HOP_UNLOCK;

    return BCM_E_NONE;
}


int
next_hop_update(const cpudb_base_t *local_base)
{
    NEXT_HOP_INIT;
    NEXT_HOP_LOCK;

    CPUDB_KEY_COPY(next_hop_local_key, local_base->key);
    nh_tx_local_mac_set(local_base->mac);

    NEXT_HOP_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_stop
 * Purpose:
 *      Stop next-hop broadcast packet reception
 * Returns:
 *      BCM_E_XXX
 */

int
next_hop_stop(void)
{
    int i;

    NEXT_HOP_INIT;

    if (!setup_done) {
        return BCM_E_NONE;
    }

    /* Keep any callbacks from doing anything */
    setup_done = FALSE;

    /* Unregister from units  */
    cputrans_rx_unregister(next_hop_rx_callback,
                           next_hop_rx_priority);

    /* Force the thread to exit */
    if (nh_thread_id != SAL_THREAD_ERROR) {
        next_hop_exit = TRUE;
        sal_sem_give(next_hop_sem);
        /* Allow thread to exit */
        for (i = 0; i < 50; i++) {
            if (nh_thread_id == SAL_THREAD_ERROR) {
                break;
            }
            sal_usleep(10000);
        }
        if (nh_thread_id != SAL_THREAD_ERROR) {
            LOG_INFO(BSL_LS_TKS_NH,
                     (BSL_META("Warning:  NEXT_HOP thread did not exit\n")));
        }
    }

    /* Deallocate local resouces */
    _next_hop_cleanup();

    return BCM_E_NONE;
}


/****************************************************************
 *
 * Callback registration
 */


/*
 * Function:
 *      next_hop_register
 * Purpose:
 *      Register a callback for the given port number
 * Parameters:
 *      callback        - Call back function
 *      cookie          - Passed back on callback
 *      mplx_num        - Which port number to listen on
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
next_hop_register(next_hop_rx_callback_f callback, void *cookie,
                  int mplx_num)
{
    int i;

    NEXT_HOP_INIT;
    NEXT_HOP_REG_LOCK;

    /* See if already registered */
    for (i = 0; i < num_cb_ctl; i++) {
        if (cb_ctl[i].mplx_num == mplx_num &&
            cb_ctl[i].callback == callback &&
            cb_ctl[i].cookie == cookie) {
            NEXT_HOP_REG_UNLOCK;
            return BCM_E_NONE;
        }
    }

    LOG_INFO(BSL_LS_TKS_NH,
             (BSL_META("Registering %p\n"),
              callback));

    if (num_cb_ctl >= NEXT_HOP_CALLBACK_MAX) {
        NEXT_HOP_REG_UNLOCK;
        return BCM_E_RESOURCE;
    }

    cb_ctl[num_cb_ctl].mplx_num = mplx_num;
    cb_ctl[num_cb_ctl].callback = callback;
    cb_ctl[num_cb_ctl].cookie = cookie;
    num_cb_ctl++;

    NEXT_HOP_REG_UNLOCK;
    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_unregister
 * Purpose:
 *      Unregister a callback for the given port number
 * Parameters:
 *      callback        - Call back function
 *      mplx_num        - Which port number to listen on
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
next_hop_unregister(next_hop_rx_callback_f callback, int mplx_num)
{
    int idx;
    int j;

    NEXT_HOP_INIT;
    NEXT_HOP_REG_LOCK;

    for (idx = 0; idx < num_cb_ctl; idx++) {
        if (cb_ctl[idx].callback == callback &&
                  cb_ctl[idx].mplx_num == mplx_num) {
            for (j = idx + 1; j < num_cb_ctl; j++) {
                sal_memcpy(&cb_ctl[j - 1], &cb_ctl[j],
                           sizeof(struct cb_ctl_s));
            }
            num_cb_ctl--;
            NEXT_HOP_REG_UNLOCK;
            return BCM_E_NONE;
        }
    }

    NEXT_HOP_REG_UNLOCK;
    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      next_hop_running
 * Purpose:
 *      Indicate if Next Hop is set up
 * Returns:
 *      TRUE if setup has been done.
 */

int
next_hop_running(void)
{
    return setup_done;
}

/* Local initialization:  Allocate needed structures */
STATIC int
_next_hop_init(void)
{
    int bytes;
    int i;
    int rv = BCM_E_NONE;

    /* Allocate synchronization structures */
    next_hop_lock = sal_mutex_create("next_hop_lock");
    if (next_hop_lock == NULL) {
        return BCM_E_MEMORY;
    }

    next_hop_reg_lock = sal_mutex_create("next_hop_reg_lock");
    if (next_hop_reg_lock == NULL) {
        _next_hop_cleanup();
        return BCM_E_MEMORY;
    }

    next_hop_unit_lock = sal_mutex_create("next_hop_unit_lock");
    if (next_hop_unit_lock == NULL) {
        _next_hop_cleanup();
        return BCM_E_MEMORY;
    }

    next_hop_sem = sal_sem_create("next_hop_sem", sal_sem_BINARY, 0);
    if (next_hop_sem == NULL) {
        _next_hop_cleanup();
        return BCM_E_MEMORY;
    }

    /* Allocate RX queue */
    bytes = sizeof(bcm_pkt_t) * next_hop_rx_queue_size;
    rx_queue_alloc_ptr = sal_alloc(bytes, "next_hop_queue");
    if (rx_queue_alloc_ptr == NULL) {
        _next_hop_cleanup();
        return BCM_E_MEMORY;
    }
    sal_memset(rx_queue_alloc_ptr, 0, bytes);

    for (i = 0; i < next_hop_rx_queue_size; i++) {
        rx_queue_alloc_ptr[i].next = &rx_queue_alloc_ptr[i + 1];
    }
    rx_queue_alloc_ptr[i - 1].next = NULL;
    rx_queue_freelist = rx_queue_alloc_ptr;

    rx_queue = rx_queue_tail = NULL;

    /* Allocate TX queue */
    bytes = sizeof(tx_queue_t) * next_hop_tx_queue_size;
    tx_queue_alloc_ptr = sal_alloc(bytes, "next_hop_tx_queue");
    if (tx_queue_alloc_ptr == NULL) {
        _next_hop_cleanup();
        return BCM_E_MEMORY;
    }
    sal_memset(tx_queue_alloc_ptr, 0, bytes);

    for (i = 0; i < next_hop_tx_queue_size; i++) {
        tx_queue_alloc_ptr[i].next = &tx_queue_alloc_ptr[i + 1];
    }
    tx_queue_alloc_ptr[i - 1].next = NULL;
    tx_queue_freelist = tx_queue_alloc_ptr;

    tx_queue = tx_queue_tail = NULL;

    return rv;
}

#define CHECK_FREE(id)            \
    do {                          \
        if ((id) != NULL) {       \
            sal_free(id);         \
            (id) = NULL;          \
        }                         \
    } while (0)

/* Local deallocation if error in allocation */
STATIC void
_next_hop_cleanup(void)
{
    if (next_hop_lock != NULL) {
        sal_mutex_destroy(next_hop_lock);
        next_hop_lock = NULL;
    }
    if (next_hop_reg_lock != NULL) {
        sal_mutex_destroy(next_hop_reg_lock);
        next_hop_reg_lock = NULL;
    }
    if (next_hop_unit_lock != NULL) {
        sal_mutex_destroy(next_hop_unit_lock);
        next_hop_unit_lock = NULL;
    }
    if (next_hop_sem != NULL) {
        sal_sem_destroy(next_hop_sem);
        next_hop_sem = NULL;
    }
    CHECK_FREE(rx_queue_alloc_ptr);
    CHECK_FREE(tx_queue_alloc_ptr);
    tx_queue = tx_queue_tail = NULL;
    rx_queue = rx_queue_tail = NULL;
}

#undef CHECK_FREE

/****************************************************************
 *
 * CPU key address management:
 *     External:  max_entries_get, key_get, key_invalidate.
 */

/*
 * Function:
 *      next_hop_max_entries_get
 * Purpose:
 *      Get the max possible CPU entries in internal DB
 * Returns:
 *      Integer number of entries
 * Notes:
 *      This can be used to then run through the entries with key_get
 *      to determine which CPU keys are in the system
 */

int
next_hop_max_entries_get(void)
{
    return CPU_KEY_MAX;
}


/*
 * Function:
 *      next_hop_key_get
 * Purpose:
 *      Get the i-th next hop CPU key.  Index is local to nexthop.
 * Parameters:
 *      idx    - which internal index to lookup
 * Returns:
 *      Pointer to CPU's KEY; CPUDB_KEY_NULL if not valid
 */

const cpudb_key_t *
next_hop_key_get(int idx)
{
    if (cpu_seq_list[idx] != NULL) {
        return (const cpudb_key_t *) &(cpu_seq_list[idx]->key);
    }

    return NULL;
}

/*
 * Function:
 *      next_hop_key_invalidate
 * Purpose:
 *      Invalidate an existing CPU key
 * Parameters:
 *      key
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      No error if not found.
 *
 *      The number of CPU keys that next hop tracks is twice the
 *      number of supported CPUs.  But if the system is reconfiguring
 *      over time, then higher layer applications should invalidate
 *      departed keys to make room for new ones.
 */

int
next_hop_key_invalidate(cpudb_key_t key)
{
    int local_idx;

    NEXT_HOP_INIT;
    NEXT_HOP_LOCK;
    local_idx = _nh_key_find(key);
    if (local_idx >= 0) {
        sal_free(cpu_seq_list[local_idx]);
        cpu_seq_list[local_idx] = NULL;
    }

    NEXT_HOP_UNLOCK;
    return BCM_E_NONE;
}


/****************************************************************
 *
 * Configuration information
 *     MTU  Max packet size
 *     LRU  Should CPU key be bumped in least-recently-used
 *          fashion.
 */

/*
 * Function:
 *      next_hop_mtu_get
 * Purpose:
 *      Get the current max transmit unit size in bytes
 * Parameters:
 *      mtu       - (OUT) Where to store value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Can be called anytime
 */

int
next_hop_mtu_get(int *mtu)
{
    *mtu = next_hop_mtu;

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_mtu_set
 * Purpose:
 *      Set the max transmit unit size in bytes
 * Parameters:
 *      mtu       - The max transmit unit size in bytes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Must be called before any initialization is done.
 */

int
next_hop_mtu_set(int mtu)
{

    if (next_hop_lock != NULL) {
        return BCM_E_BUSY;
    }

    next_hop_mtu = mtu;

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_lru_enable_get
 * Purpose:
 *      Return current value of KEY LRU replacement policy
 * Returns:
 *      Boolean indicating current state:
 *           TRUE     LRU replacement of CPU key on ADD is enabled.
 *           FALSE    LRU replacement of CPU key on ADD is disabled.
 */

int
next_hop_lru_enable_get(void)
{
    return key_lru_replace_enable;
}

/*
 * Function:
 *      next_hop_lru_enable_set
 * Purpose:
 *      Set the KEY LRU replacement policy
 * Parameters:
 *      lru         - The boolean value to set policy to.
 *           TRUE     LRU replacement of CPU key on ADD is enabled.
 *           FALSE    LRU replacement of CPU key on ADD is disabled.
 */

void
next_hop_lru_enable_set(int lru)
{
    key_lru_replace_enable = lru;
}



/****************************************************************
 *
 * Forward declarations, stack port configuration
 */

static INLINE int _unit_on_list(int unit);
static INLINE void _update_unit_list(void);

STATIC int
_port_add(int unit, int port, int duplex, int *reg)
{
    int i;

    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
	    nh_stk_ports[i].port == port) {
            nh_stk_ports[i].flags = duplex ? NH_FLAGS_DUPLEX : 0;
            return BCM_E_NONE;
        }
    }

    if (num_stk_ports >= STK_PORTS_MAX) {
        return BCM_E_RESOURCE;
    }

    /* If new unit and setup_done, register callback */
    *reg = (_unit_on_list(unit) < 0 && setup_done);

    nh_stk_ports[num_stk_ports].unit = unit;
    nh_stk_ports[num_stk_ports].port = port;
    nh_stk_ports[num_stk_ports].flags = duplex ? NH_FLAGS_DUPLEX : 0;
    num_stk_ports++;
    _update_unit_list();

    return BCM_E_NONE;
}

/*
 * Function:
 *      next_hop_port_add
 * Purpose:
 *      Add a port from the list of stack ports
 * Parameters:
 *      unit         - Local physical unit number
 *      port         - Local physical port on unit
 *      duplex       - Is the connection known to be duplex?
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Can re-add a port and change duplex setting.
 *      Not an error if the port is already known.
 */

int
next_hop_port_add(int unit, int port, int duplex)
{
    int rv;
    int reg = FALSE;
    NEXT_HOP_INIT;

    NEXT_HOP_UNIT_LOCK;
    NEXT_HOP_LOCK;
    rv = _port_add(unit, port, duplex, &reg);
    NEXT_HOP_UNLOCK;
    if (BCM_SUCCESS(rv) && reg) {
        rv = cputrans_rx_unit_register(unit, "next_hop",
                                       next_hop_rx_callback,
                                       next_hop_rx_priority, NULL,
                                       BCM_RCO_F_ALL_COS);
    }
    NEXT_HOP_UNIT_UNLOCK;
    if (BCM_FAILURE(rv) && reg) {
        next_hop_port_remove(unit, port);
    } else {
        LOG_INFO(BSL_LS_TKS_NH,
                 (BSL_META_U(unit,
                             "Added port (%d,%d)=%d\n"),
                  unit, port, rv));
    }
    return rv;
}


/*
 * Function:
 *      next_hop_port_remove
 * Purpose:
 *      Remove a port from the list of stack ports
 * Parameters:
 *      unit         - Local physical unit number
 *      port         - Local physical port on unit
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Returns NOT_FOUND if the port isn't on the list
 */

int
next_hop_port_remove(int unit, int port)
{
    int i, j, found, unreg;

    NEXT_HOP_INIT;

    unreg = FALSE;
    NEXT_HOP_UNIT_LOCK;
    NEXT_HOP_LOCK;
    found = FALSE;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
            nh_stk_ports[i].port == port) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        NEXT_HOP_UNLOCK;
        NEXT_HOP_UNIT_UNLOCK;
        return BCM_E_NOT_FOUND;
    }

    for (j = i + 1; j < num_stk_ports; j++) {
        nh_stk_ports[j - 1].unit = nh_stk_ports[j].unit;
        nh_stk_ports[j - 1].port = nh_stk_ports[j].port;
    }
    num_stk_ports--;

    /* Update the unit list and see if removed; unregister if so */
    _update_unit_list();
    unreg = (_unit_on_list(unit) < 0 && setup_done);

    NEXT_HOP_UNLOCK;
    if (unreg) {
        cputrans_rx_unit_unregister(unit, next_hop_rx_callback,
                                    next_hop_rx_priority);
    }
    NEXT_HOP_UNIT_UNLOCK;
    LOG_INFO(BSL_LS_TKS_NH,
             (BSL_META_U(unit,
                         "Removed port (%d,%d)\n"),
              unit,port));
    return BCM_E_NONE;
}


/* Manage list of known units; return unit index if found; otherwise -1 */
static INLINE int
_unit_on_list(int unit)
{
    int i;
    for (i = 0; i < num_units; i++) {
        if (units[i] == unit) {
            return i;
        }
    }
    return -1;
}

/* Updates unit list; Assumes lock is held; completely regenerates
 * the unit list from the stack port list.
 */
STATIC INLINE void
_update_unit_list(void)
{
    int idx, i;

    num_units = 0;
    for (i = 0; i < num_stk_ports; i++) {
        idx = _unit_on_list(nh_stk_ports[i].unit);
        if (idx < 0) {
            units[num_units++] = nh_stk_ports[i].unit;
        }
    }
}


/*
 * Function:
 *      next_hop_num_ports_get
 * Purpose:
 *      Get the number of ports in NH database
 * Returns:
 *      Integer number of stack ports
 */

int
next_hop_num_ports_get(void)
{
    return num_stk_ports;
}


/*
 * Function:
 *      next_hop_port_get
 * Purpose:
 *      Get a next hop port
 * Parameters:
 *      idx           - Which index to look up
 *      unit          - Where to put unit; may be NULL
 *      port          - Where to put port; may be NULL
 *      duplex        - Where to put duplex setting; may be NULL
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
next_hop_port_get(int idx, int *unit, int *port, int *duplex)
{
    if (idx < 0 || idx >= num_stk_ports) {
        return BCM_E_NOT_FOUND;
    }

    if (unit != NULL) {
        *unit = nh_stk_ports[idx].unit;
    }
    if (port != NULL) {
        *port = nh_stk_ports[idx].port;
    }
    if (duplex != NULL) {
        *duplex = nh_stk_ports[idx].flags & NH_FLAGS_DUPLEX;
    }

    return BCM_E_NONE;
}


/****************************************************************
 *
 * Primary API calls:
 *       next_hop_pkt_create      Create a (persistent) packet
 *       next_hop_pkt_send        Send packet from create
 *       next_hop_pkt_destroy     Release packet from create
 *       next_hop_tx              Send buffer once
 */

static INLINE uint16 next_hop_seq_num_get(void);
static INLINE int _tx_packet_enqueue(bcm_pkt_t *pkt,
                                      next_hop_tx_callback_f callback,
                                      void *cookie,
                                      uint32 flags);
STATIC int _packet_send(bcm_pkt_t *pkt, int forward,
                        next_hop_tx_callback_f callback,
                        void *cookie);

/*
 * Function:
 *      next_hop_pkt_create
 * Purpose:
 *      Set up and allocate a packet or list of packets to be transmitted
 *      by next hop
 * Parameters:
 *      pkt_buf   - The packet buffer to send
 *      len       - Number of bytes of buffer to send
 *      vlan      - VLAN used on packet
 *      cos       - COS and internal priority used on packet
 *      seg_len   - Number of bytes in a segment
 *      ct_flags     - Bitmap of flags passed in
 *          CPUTRANS_NO_HEADER_ALLOC   Space for transport header available
 *                                     at start of packet
 *      mplx_num  - Port number to place in packet
 *      nh_dest_key  - Broadcast or destination CPU key used inside of
 *                  nexthop packet (not L2 header).
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      See next_hop_tx for notes.  This routine sets up a tx transaction
 *      but does not enqueue it.
 *
 *      Once created, the packet may be sent repeatedly using the
 *      next_hop_pkt_update and next_hop_pkt_send routines.  When done,
 *      next_hop_pkt_destroy must be called on the packet.
 *
 *      If cos (or vlan) is < 0, the default is used.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

#define NO_BCM_UNIT 255

bcm_pkt_t *
next_hop_pkt_create(uint8 *pkt_buf,
                    int len,
                    int cos,
                    int vlan,
                    int seg_len,
                    uint32 ct_flags,
                    int mplx_num,
                    const cpudb_key_t nh_dest_key,
                    int *tot_segs,
                    int *rvp)
{
    bcm_pkt_t *pkt, *cur_pkt;
    uint8 *hdr_buf;
    int rv;

    if (next_hop_lock == NULL) {
        if ((rv=_next_hop_init()) < 0) {
            *rvp = rv;
            return NULL;
        }
    }

    if (!setup_done) {
        *rvp = BCM_E_INIT;
        return NULL;
    }

    pkt = cputrans_tx_pkt_list_alloc(pkt_buf, len, seg_len, ct_flags,
                                     tot_segs);
    if (pkt == NULL) {
        *rvp = BCM_E_MEMORY;
        return NULL;
    }

    pkt->rx_unit = NO_BCM_UNIT;
    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        /* Set up the packet header for each segment, always in first block. */
        hdr_buf = cur_pkt->pkt_data[0].data;

        NEXT_HOP_SRC_KEY_SET(hdr_buf, next_hop_local_key);
        NEXT_HOP_SEQ_NUM_SET(hdr_buf, next_hop_seq_num_get());
        if (ct_flags & CPUTRANS_BCAST) {
            NEXT_HOP_DEST_KEY_SET(hdr_buf, cpudb_bcast_key);
        } else {
            NEXT_HOP_DEST_KEY_SET(hdr_buf, nh_dest_key);
        }
        NEXT_HOP_MPLX_NUM_SET(hdr_buf, mplx_num);

        cur_pkt->call_back = NULL;
    }

    cos = (cos < 0) ? nh_cos : cos;
    vlan = (vlan < 0) ? nh_vlan : vlan;

    nh_pkt_setup(pkt, cos, vlan, NEXT_HOP_PKT_TYPE, 0);
    *rvp = BCM_E_NONE;
    return pkt;
}

/*
 * Function:
 *      next_hop_pkt_update
 * Purpose:
 *      Update a packet or list of packets created by next_hop_pkt_create,
 *      cputrans_tx_pkt_alloc or cputrans_tx_pkt_list_alloc.
 * Parameters:
 *      pkt            - Packet to update
 *      mplx_num       - Port number to use in packet
 *      nh_dest_key    - Destination key to use to address CPU
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Initializes the next hop and nh_tx (transport) headers
 *
 *      This will change the sequence number for each packet as
 *      well, so that the packet appears as a different NH packet
 *      when sent.
 */

int
next_hop_pkt_update(bcm_pkt_t *pkt, int mplx_num,
                    const cpudb_key_t nh_dest_key)
{
    uint8 *hdr_buf;
    bcm_pkt_t *cur_pkt;

    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        hdr_buf = cur_pkt->pkt_data[0].data;
        NEXT_HOP_DEST_KEY_SET(hdr_buf, nh_dest_key);
        NEXT_HOP_MPLX_NUM_SET(hdr_buf, mplx_num);
        NEXT_HOP_SEQ_NUM_SET(hdr_buf, next_hop_seq_num_get());
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_buffer_init
 * Purpose:
 *      Initialize a buffer that is to be sent using nh_tx and received
 *      by the next hop layer
 * Parameters:
 *      hdr_buf       - Pointer to header to update
 *      mplex_num     - Multiplexing port number for packet
 *      nh_dest_key   - Next hop destination CPU key to use
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      hdr_buf must point to the start of the CPUTRANS header for
 *      the packet.
 *
 *      For now, this sets the type/length field to get the pkt thru.
 */

int
next_hop_buffer_init(uint8 *hdr_buf, int mplex_num,
                     const cpudb_key_t nh_dest_key)
{
    int len_fld_val = 0x601;  

    PACK_SHORT(&hdr_buf[CPUTRANS_LEN_OFS], len_fld_val);
    NEXT_HOP_SRC_KEY_SET(hdr_buf, next_hop_local_key);
    NEXT_HOP_DEST_KEY_SET(hdr_buf, nh_dest_key);
    NEXT_HOP_MPLX_NUM_SET(hdr_buf, mplex_num);
    NEXT_HOP_SEQ_NUM_SET(hdr_buf, next_hop_seq_num_get());

    return BCM_E_NONE;
}

/*
 * Function:
 *      next_hop_pkt_send
 * Purpose:
 *      Transmit a packet or linked list of pkts prepared by next_hop_pkt_create
 * Parameters:
 *      pkt       - The packet(s) prepared by next_hop_pkt_create
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The packet(s) passed to this routine must have been created by
 *      next_hop_pkt_create
 */

int
next_hop_pkt_send(bcm_pkt_t *pkt,
                  next_hop_tx_callback_f callback,
                  void *cookie)
{
    int rv = BCM_E_NONE;

    if (callback != NULL) {   /* Send async */
        rv = _tx_packet_enqueue(pkt, callback, cookie, PACKET_F_PERSISTENT);
    } else {
        rv = _packet_send(pkt, FALSE, callback, cookie);
    }

    return rv;
}


/*
 * Function:
 *      next_hop_pkt_destroy
 * Purpose:
 *      "Free" a packet or list of packets created by next_hop_pkt_create
 * Parameters:
 *      pkt       - The packet pointer returned by next_hop_pkt_create
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      After calling this routine, the packet should no longer be
 *      referenced.
 */

int
next_hop_pkt_destroy(bcm_pkt_t *pkt)
{
    cputrans_tx_pkt_list_free(pkt);

    return BCM_E_NONE;
}


/*
 * Function:
 *      next_hop_tx
 * Purpose:
 *      Transmit a next hop packet
 * Parameters:
 *      pkt_buf      - The packet buffer to send
 *      len          - Number of bytes of buffer to send
 *      cos       - COS and internal priority used on packet
 *      vlan      - VLAN used on packet
 *      seg_len      - Number of bytes in a segment
 *      ct_flags        - Bitmap of flags passed in
 *          NEXT_HOP_NO_HEADER_ALLOC   Header space is available at
 *                                     beginning of packet buffer.
 *      mplx_num     - Port number to place in packet
 *      nh_dest_key  - Broadcast or destination key used inside of
 *                  nexthop packet (not L2 header).
 *      callback     - If non-NULL send asynchronously
 *      cookie       - Passed to callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Note that the application does not need to forward the
 *      packets that it receives; that happens automatically at this layer.
 *
 *      If HEADER_ALLOC flag is NOT set, then the packet buffer must
 *      have space (NEXT_HOP_HEADER_BYTES bytes) for next hop info at
 *      the head of packet buffer.
 *
 *      If asynchronous send is done, the callback must call
 *      next_hop_tx_done with the cookie passed back in tx_cookie.
 *
 *      If cos (or vlan) is < 0, the default is used.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
next_hop_tx(uint8 *pkt_buf,
            int len,
            int cos,
            int vlan,
            int seg_len,
            uint32 ct_flags,
            int mplx_num,
            const cpudb_key_t nh_dest_key,
            next_hop_tx_callback_f callback,
            void *cookie)
{
    bcm_pkt_t *pkt;
    int rv = BCM_E_NONE;

    NEXT_HOP_INIT;
    if (!setup_done) {
        return BCM_E_INIT;
    }

    pkt = next_hop_pkt_create(pkt_buf,
                              len,
                              cos,
                              vlan,
                              seg_len,
                              ct_flags,
                              mplx_num,
                              nh_dest_key, NULL, &rv);

    if (pkt == NULL) {
        return BCM_E_RESOURCE;
    }

    if (callback != NULL) {   /* Send async, dealloc pkt when done */
        rv = _tx_packet_enqueue(pkt, callback, cookie, 0);
    } else {                  /* Send sync */
        rv = _packet_send(pkt, FALSE, callback, cookie);
    }

    if (callback == NULL || rv != BCM_E_NONE) {
        next_hop_pkt_destroy(pkt);
    }

    return rv;
}

/*
 * Function:
 *      next_hop_data_free
 * Purpose:
 *      Free data buffers from stolen packet pointers
 * Parameters:
 *      pkt_buf      - The buffer to free
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

void
next_hop_data_free(uint8 *pkt_buf)
{
    next_hop_trans_ptr->tp_data_free(-1, pkt_buf);
}

/****************************************************************
 *
 * Next Hop Thread
 */


/****************************************************************
 *
 * Next hop thread functions
 *      next_hop_thread     The main thread control function
 *      handle_txrx_queue   Process all pending operations in tx/rx queueu
 */

static INLINE void handle_tx_queue(void);
static INLINE void handle_rx_queue(void);
static INLINE void _packet_up_stack(bcm_pkt_t *pkt);
static INLINE void _rx_packet_free(bcm_pkt_t *pkt);
static INLINE void _tx_queue_free(tx_queue_t *entry);

/*
 * Function:
 *      next_hop_thread
 * Purpose:
 *      Main thread for Next Hop Broadcast transport
 * Parameters:
 *      cookie       - ignored
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Simple loop to service TX and RX queues.  Will exit when
 * next_hop_exit is set.
 */

STATIC void
next_hop_thread(void *cookie)
{

    COMPILER_REFERENCE(cookie);

    next_hop_exit = FALSE;
    while (!next_hop_exit) {
        sal_sem_take(next_hop_sem, sal_sem_FOREVER);

        if (next_hop_exit) {
            break;
        }

        handle_tx_queue();
        handle_rx_queue();
    }

    nh_thread_id = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Handle TX packet queue
 */

static INLINE void
handle_tx_queue(void)
{
    tx_queue_t *local_tx_queue, *entry;

    if (tx_queue != NULL) {

        /* Grab TX queue and process entries */
        NEXT_HOP_LOCK;
        local_tx_queue = (tx_queue_t *)tx_queue;
        tx_queue = NULL;
        tx_queue_tail = NULL;
        NEXT_HOP_UNLOCK;

        while (local_tx_queue != NULL) {
            entry = local_tx_queue;
            local_tx_queue = (tx_queue_t *)local_tx_queue->next;

            (void)_packet_send(entry->pkt, FALSE,
                               entry->callback, entry->cookie);

            if (!(entry->flags & PACKET_F_PERSISTENT)) {
                cputrans_tx_pkt_list_free(entry->pkt);
            }
            _tx_queue_free(entry);
        }
    }
}

/*
 * Handle RX packet queue
 */

static INLINE void
handle_rx_queue(void)
{
    bcm_pkt_t *local_rx_queue;
    bcm_pkt_t *pkt;
    int is_bcast;            /* Packet is broadcast */
    int is_directed;         /* Packet sent to a specific key */
    int is_directed_local;   /* Packet sent to local cpu directly */
    cpudb_key_t key;         /* From next hop header */
    int is_neighbor;         /* Don't forward. */

    if (rx_queue != NULL) {

        /* Grab RX queue and process entries */
        NEXT_HOP_LOCK;
        local_rx_queue = (bcm_pkt_t *)rx_queue;
        rx_queue = NULL;
        rx_queue_tail = NULL;
        NEXT_HOP_UNLOCK;

        while (local_rx_queue != NULL) {
            pkt = local_rx_queue;
            local_rx_queue = local_rx_queue->next;
	    pkt->next = NULL;

            /* See if packet should go up local stack */
            NEXT_HOP_DEST_KEY_GET(pkt->pkt_data[0].data, key);
            is_bcast = !CPUDB_KEY_BCAST_COMPARE(key);
            is_neighbor = !CPUDB_KEY_NEIGHBOR_COMPARE(key);
            is_directed = !(is_bcast || is_neighbor);
            is_directed_local = !CPUDB_KEY_COMPARE(key, next_hop_local_key);

            LOG_DEBUG(BSL_LS_TKS_NH,
                      (BSL_META("nh pkt " CPUDB_KEY_FMT
                                ". bcast %d dir %d local %d neighbor %d\n"),
                       CPUDB_KEY_DISP(key), is_bcast, is_directed,
                       is_directed_local, is_neighbor));

            /* Transmit the data first in case application steals the packet */
            if (is_bcast || (is_directed && !is_directed_local)) {
                pkt->pkt_data[0].len = pkt->pkt_len;
                pkt->flags |= BCM_TX_CRC_REGEN;
                (void)_packet_send(pkt, TRUE, NULL, NULL);
            }

            if (is_bcast || is_directed_local || is_neighbor) {
                _packet_up_stack(pkt);
            }

            _rx_packet_free(pkt);
        }
    }
}


/****************************************************************
 *
 * RX queue management
 */

/*
 * Allocate an RX packet and copy data from another packet
 */
static INLINE bcm_pkt_t *
_rx_packet_alloc(bcm_pkt_t *pkt)
{
    bcm_pkt_t *new_pkt;

    NEXT_HOP_LOCK;
    if (rx_queue_freelist == NULL) {
        NEXT_HOP_UNLOCK;
        return NULL;
    }
    new_pkt = (bcm_pkt_t *)rx_queue_freelist;
    rx_queue_freelist = rx_queue_freelist->next;
    NEXT_HOP_UNLOCK;

    sal_memcpy(new_pkt, pkt, sizeof(bcm_pkt_t));
    new_pkt->pkt_data = &new_pkt->_pkt_data;

    return new_pkt;
}

/*
 * Free an RX packet and associated data
 */
static INLINE void
_rx_packet_free(bcm_pkt_t *pkt)
{
    if (pkt->alloc_ptr != NULL) {
        next_hop_trans_ptr->tp_data_free(pkt->rx_unit, pkt->alloc_ptr);
    }
    NEXT_HOP_LOCK;
    pkt->next = (bcm_pkt_t *)rx_queue_freelist;
    rx_queue_freelist = pkt;
    NEXT_HOP_UNLOCK;
}

/*
 * Enqueue an RX packet; grab a packet from the rx freelist
 */
static INLINE int
_rx_packet_enqueue(bcm_pkt_t *pkt)
{
    bcm_pkt_t *new_pkt;

    LOG_DEBUG(BSL_LS_TKS_NH,
              (BSL_META("next hop rx enqueue\n")));

    new_pkt = _rx_packet_alloc(pkt);
    if (new_pkt == NULL) {
        LOG_INFO(BSL_LS_TKS_NH,
                 (BSL_META("NH: no queue, resource\n")));
        return BCM_E_RESOURCE;
    }
#if defined(BCM_RXP_DEBUG)
    bcm_rx_pool_own(pkt->alloc_ptr, "nh_pkt_enqueue");
#endif

    NEXT_HOP_LOCK;
    if (rx_queue_tail == NULL) {   /* Queue is currently empty */
        rx_queue = new_pkt;
    } else {
        rx_queue_tail->next = new_pkt;
    }
    rx_queue_tail = new_pkt;
    NEXT_HOP_UNLOCK;

    sal_sem_give(next_hop_sem);      /* Wake handling thread */

    return BCM_E_NONE;
}

/****************************************************************
 *
 * TX queue management
 */

/*
 * Enqueue a TX packet
 */
static INLINE int
_tx_packet_enqueue(bcm_pkt_t *pkt,
                   next_hop_tx_callback_f callback,
                   void *cookie,
                   uint32 flags)
{
    tx_queue_t *entry;

    LOG_DEBUG(BSL_LS_TKS_NH,
              (BSL_META("next hop tx enqueue\n")));

    NEXT_HOP_LOCK;
    if (tx_queue_freelist == NULL) {
        NEXT_HOP_UNLOCK;
        return BCM_E_RESOURCE;
    }
    entry = (tx_queue_t *)tx_queue_freelist;
    tx_queue_freelist = entry->next;
    entry->pkt = pkt;
    entry->callback = callback;
    entry->cookie = cookie;
    entry->next = NULL;
    entry->flags = flags;

    if (tx_queue_tail == NULL) {   /* Queue is currently empty */
        tx_queue = entry;
    } else {
        tx_queue_tail->next = entry;
    }
    tx_queue_tail = entry;
    NEXT_HOP_UNLOCK;

    sal_sem_give(next_hop_sem);      /* Wake handling thread */

    return BCM_E_NONE;
}

/* Release a tx queue entry to the freelist */
static INLINE void
_tx_queue_free(tx_queue_t *entry)
{
    NEXT_HOP_LOCK;
    entry->next = tx_queue_freelist;
    tx_queue_freelist = entry;
    NEXT_HOP_UNLOCK;
}

/****************************************************************
 *
 * Low level transmit function
 *     Transmit the packet or packets pointed to by pkt.
 *     Always synchronous send from here.
 *     Slightly different behavior if this is a received packet
 * that is being forwarded versus a packet from the application layer
 * being sent out.
 */

STATIC int
_packet_send(bcm_pkt_t *pkt,
             int forward,  /* Is received packet being forwarded? */
             next_hop_tx_callback_f callback,
             void *cookie)
{
    int rv = BCM_E_NONE;
    int tmp_rv, i;
    bcm_pkt_t *cur_pkt;

    for (i = 0; i < num_stk_ports; i++) {
        if (forward) {
            /* Since the packet is being forwarded at the next hop
             * level, it doesn't need to be updated at all.  We do
             * check for duplex source port */
            if (pkt->rx_unit == nh_stk_ports[i].unit &&
                pkt->rx_port == nh_stk_ports[i].port &&
                nh_stk_ports[i].flags & NH_FLAGS_DUPLEX) {
                continue; /* Skip duplex source port */
            }
            /* Need to do minimal packet setup, esp for SL tag */
            nh_pkt_final_setup(pkt, nh_stk_ports[i].unit,
                               nh_stk_ports[i].port);
        } else {
            /* Set up the packet now that we know the unit/port */
            nh_pkt_local_setup(pkt, nh_stk_ports[i].unit,
                               nh_stk_ports[i].port);
        }
        for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
            BCM_PBMP_PORT_SET(cur_pkt->tx_pbmp, nh_stk_ports[i].port);
            pkt->unit = nh_stk_ports[i].unit;
        }

        tmp_rv = nh_pkt_tx(pkt, NULL, NULL);
        if (tmp_rv < 0) {
            LOG_INFO(BSL_LS_TKS_NH,
                     (BSL_META("NEXT_HOP: Failed to send to unit %d, port %d\n"),
                      nh_stk_ports[i].unit, nh_stk_ports[i].port));
            rv = tmp_rv;
            ++tx_error_count;
        }
    }

    /* See if there's a callback to make for the packet */
    if (callback != NULL) {
        callback(rv, pkt->alloc_ptr, cookie);
    }

    return rv;
}

/****************************************************************
 *
 * Send a packet up the local stack to callbacks registered for
 * the port number in the packet (or -1.)
 */

static INLINE void
_packet_up_stack(bcm_pkt_t *pkt)
{
    cpudb_key_t src_key;
    int idx;
    int mplx_num;
    uint8 *pkt_data;
    bcm_rx_t rv;
    int len;

    pkt_data = pkt->pkt_data[0].data;

    NEXT_HOP_SRC_KEY_GET(pkt_data, src_key);
    NEXT_HOP_MPLX_NUM_GET(pkt_data, mplx_num);

    len = pkt->pkt_len;  /* Strip CRC if not already done */
    if (!(pkt->flags & BCM_RX_CRC_STRIP)) {
        len -= 4;
    }

    NEXT_HOP_REG_LOCK;
#if defined(BCM_RXP_DEBUG)
    bcm_rx_pool_own(pkt->alloc_ptr, "nh_app");
#endif
    for (idx = 0; idx < num_cb_ctl; idx++) {
        if (cb_ctl[idx].mplx_num == mplx_num ||
            cb_ctl[idx].mplx_num == -1) {
            rv = cb_ctl[idx].callback(src_key, mplx_num,
                                      pkt->rx_unit,
                                      pkt->rx_port,
                                      pkt_data,
                                      len,
                                      cb_ctl[idx].cookie);
            if (rv == BCM_RX_HANDLED) {
                break;
            } else if (rv == BCM_RX_HANDLED_OWNED) {
                pkt->alloc_ptr = NULL;
                break;
            }
        }
    }
    NEXT_HOP_REG_UNLOCK;
}


/****************************************************************
 *
 * Callback function to register to receive packets
 *        next_hop_rx_callback       Get packets from RX
 *
 * Plus helper functions
 *        _rx_key_seq_seen           Handle seq num for the key
 *        _nh_key_find               Look for key in local DB
 *        _nh_key_add                Add a key to the local DB
 *        _key_lru_replace           Replace least recently used key
 */

static INLINE int _rx_key_seq_seen(cpudb_key_t key, uint16 seq_num);
static INLINE int _seq_num_seen(int cpu_idx, uint16 seq_num);
static INLINE int _seq_num_add(int cpu_idx, uint16 seq_num);

/*
 * Function:
 *      next_hop_rx_callback
 * Purpose:
 *      Packet handler for unclassified RX packets
 * Parameters:
 *      unit             - On which unit was pkt received
 *      pkt              - The received packet
 *      cookie           - Ignored
 * Returns:
 *      bcm_rx_t:  Indication if packet handled/stolen
 * Notes:
 */

STATIC bcm_rx_t
next_hop_rx_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    uint16 pkt_type;         /* From the lower level, extracted from packet */
    cpudb_key_t key;         /* From next hop header */
    uint16 seq_num;          /* next hop sequence for detecting "seen" pkts */
    int seen;

    LOG_DEBUG(BSL_LS_TKS_NH,
              (BSL_META("NEXT_HOP pkt in\n")));

    if (next_hop_lock == NULL || !setup_done ||
                              nh_thread_id == SAL_THREAD_ERROR) {
        LOG_INFO(BSL_LS_TKS_NH,
                 (BSL_META_U(unit,
                             "exit: %p, %d, %p\n"),
                  next_hop_lock, setup_done, nh_thread_id));
        return BCM_RX_NOT_HANDLED;
    }

    /* Is this a next hop packet? */
    if (!nh_tx_pkt_recognize(pkt->pkt_data[0].data, &pkt_type)) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NEXT_HOP pkt not recognized\n")));
        return BCM_RX_NOT_HANDLED;
    }
    if (pkt_type != NEXT_HOP_PKT_TYPE) {    /* Is this an NEXT_HOP packet */
        LOG_INFO(BSL_LS_TKS_NH,
                 (BSL_META_U(unit,
                             "NEXT_HOP pkt not proper type\n")));
        return BCM_RX_NOT_HANDLED;
    }

    /* Good packet, have we seen it before? */
    NEXT_HOP_SRC_KEY_GET(pkt->pkt_data[0].data, key);
    /* Is local CPU the source of the packet? */
    if (!CPUDB_KEY_COMPARE(key, next_hop_local_key)) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NEXT_HOP source is local\n")));
        return BCM_RX_HANDLED;
    }

    /* Check for a seq num we've seen before. */
    NEXT_HOP_SEQ_NUM_GET(pkt->pkt_data[0].data, seq_num);
    LOG_DEBUG(BSL_LS_TKS_NH,
              (BSL_META("From KEY " CPUDB_KEY_FMT "; seq %d\n"),
               CPUDB_KEY_DISP(key), seq_num));
    NEXT_HOP_LOCK;
    seen = _rx_key_seq_seen(key, seq_num);
    NEXT_HOP_UNLOCK;
    if (seen) {   /* Really, (seen == TRUE || seen < 0) */
        return BCM_RX_HANDLED;
    }

    /* Try to enqueue the packet for later retrx and passing up stack */
    if (_rx_packet_enqueue(pkt) < 0) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NH: Dropped RX pkt %d\n"), seq_num));
        ++rx_pkt_drop_count;
        return BCM_RX_HANDLED;
    }

    return BCM_RX_HANDLED_OWNED;
}

/****************************************************************
 *
 * Sequence number checking and CPU key management
 */

/*
 * Check the CPU key/seq num pair; returns TRUE if seen before.
 */
static INLINE int
_rx_key_seq_seen(cpudb_key_t key, uint16 seq_num)
{
    int local_idx;           /* Index in local DB of CPU key */

    local_idx = _nh_key_find(key);
    if (local_idx < 0) {   /* New CPU key; hence new packet */
        local_idx = _nh_key_add(key);
        if (local_idx < 0) {
            LOG_INFO(BSL_LS_TKS_NH,
                     (BSL_META("NEXT_HOP key rsrc err\n")));
            return BCM_E_RESOURCE;
        }
    } else {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("Local idx %d\n"), local_idx));
        if (_seq_num_seen(local_idx, seq_num)) {  /* Seen it */
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NEXT_HOP pkt previously seen\n")));
            return TRUE;
        }
    }

    _seq_num_add(local_idx, seq_num);
    return FALSE;
}


/*
 * _seq_num_seen:
 *
 * Boolean:  Returns true if the pair (cpu_idx, seq_num) has
 * been seen before;
 *
 * This is currently implemented as a simple circular queue.
 * The main assumption is that the queue is big enough that by
 * the time an entry is overwritten, the corresponding packet will
 * not be seen again.  Also, that the queue is small enough that
 * wrap around of sequence numbers isn't a problem.
 */

static INLINE int
_seq_num_seen(int local_idx, uint16 seq_num)
{
    int sn_idx;
    int i;

    sn_idx = cpu_seq_list[local_idx]->seq_num_last;
    for (i = 0; i < NEXT_HOP_SEQ_NUM_TRACK; i++) {
        if (cpu_seq_list[local_idx]->seq_nums_seen[sn_idx] == seq_num) {
            return TRUE;
        }
        if (--sn_idx < 0) {
            sn_idx = NEXT_HOP_SEQ_NUM_TRACK - 1;
        }
    }

    return FALSE;
}

/*
 * _seq_num_add:
 *
 * Add a sequence number to the "seen" list for the given CPU index.
 * Assumes lock held.
 *
 * This is currently implemented as a simple circular queue.  See above
 */

static INLINE int
_seq_num_add(int local_idx, uint16 seq_num)
{
    int last;

    if (++(cpu_seq_list[local_idx]->seq_num_last) >=
                   NEXT_HOP_SEQ_NUM_TRACK) {
        cpu_seq_list[local_idx]->seq_num_last = 0;
    }

    last = cpu_seq_list[local_idx]->seq_num_last;
    cpu_seq_list[local_idx]->seq_nums_seen[last] = seq_num;

    return FALSE;
}


/* Look for CPU key in local DB */

static INLINE int
_nh_key_find(cpudb_key_t key)
{
    int i;

    for (i = 0; i < CPU_KEY_MAX; i++) {
        if (cpu_seq_list[i] != NULL &&
            !CPUDB_KEY_COMPARE(cpu_seq_list[i]->key, key)) {
            cpu_seq_list[i]->last_ref = sal_time();
            return i;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Look for least recently used CPU key in DB and replace it; assumes all
 * entries are occupied
 */

static INLINE int
_key_lru_replace(cpudb_key_t key)
{
    int idx, i;

    for (idx = 0, i = 1; i < CPU_KEY_MAX; i++) {
        if (cpu_seq_list[i]->last_ref < cpu_seq_list[idx]->last_ref) {
            idx = i;
        }
    }

    
    sal_memset(cpu_seq_list[idx], 0, sizeof(cpu_seq_list_t));
    CPUDB_KEY_COPY(cpu_seq_list[idx]->key, key);
    /* see _nh_key_add() below for seq_num_last initialization */
    cpu_seq_list[idx]->seq_num_last = NEXT_HOP_SEQ_NUM_TRACK - 1;
    cpu_seq_list[idx]->last_ref = sal_time();

    return idx;
}

/* Add a CPU key to local DB.  Check LRU mode if table is full */

static INLINE int
_nh_key_add(cpudb_key_t key)
{
    int i;
    int rv = BCM_E_NONE;

    for (i = 0; i < CPU_KEY_MAX; i++) {
        if (cpu_seq_list[i] == NULL) {
            cpu_seq_list[i] = sal_alloc(sizeof(cpu_seq_list_t), "seq_num");
            if (cpu_seq_list[i] == NULL) {
                rv = BCM_E_MEMORY;
            } else {
                sal_memset(cpu_seq_list[i], 0, sizeof(cpu_seq_list_t));
                CPUDB_KEY_COPY(cpu_seq_list[i]->key, key);
                /* init seq_num_last to the end so the preincrement in
                   _seq_num_add() starts at 0 */
                cpu_seq_list[i]->seq_num_last = NEXT_HOP_SEQ_NUM_TRACK - 1;
                cpu_seq_list[i]->last_ref = sal_time();
            }
            break;
        }
    }

    /* Couldn't find an entry */
    if (rv == BCM_E_NONE && i >= CPU_KEY_MAX) {
        if (key_lru_replace_enable) {
            i = _key_lru_replace(key);
        } else {
            rv = BCM_E_MEMORY;
        }
    }

    return BCM_SUCCESS(rv) ? i : rv;
}

/****************************************************************
 *
 * Get a new sequence number to put in a packet
 */

/* Get a sequence number for NEXT_HOP pkts; Will never return 0 */
static INLINE uint16
next_hop_seq_num_get(void)
{
    static uint16 _seq_num;
    uint16 new_seq_num;

    NEXT_HOP_LOCK;
    if (++_seq_num >= NEXT_HOP_SEQ_NUM_MAX) {
        _seq_num = 1;
    }
    new_seq_num = _seq_num;
    NEXT_HOP_UNLOCK;

    return new_seq_num;
}

#if defined(BROADCOM_DEBUG)
void
next_hop_dump(void)
{
    int cos, int_prio;

    cos = CPUTRANS_COS_GET(nh_cos);
    if (nh_cos & CPUTRANS_INT_PRIO_VALID) {
        int_prio = CPUTRANS_INT_PRIO_GET(nh_cos);
    } else {
        int_prio = cos;
    }

    LOG_CLI((BSL_META("Next Hop\n")));

    LOG_CLI((BSL_META("  %s.  %s.\n"),
              (next_hop_lock != NULL) ? "Initialized":"Not initialized",
             setup_done ? "Running":"Not running"));
    LOG_CLI((BSL_META("  thread_priority   = %d\n"),
              next_hop_thread_priority));
    LOG_CLI((BSL_META("  rx_priority       = %d\n"),
              next_hop_rx_priority));
    LOG_CLI((BSL_META("  rx_queue_size     = %d\n"),
              next_hop_rx_queue_size));
    LOG_CLI((BSL_META("  tx_queue_size     = %d\n"),
              next_hop_tx_queue_size));
    LOG_CLI((BSL_META("  mtu               = %d\n"),
              next_hop_mtu));
    LOG_CLI((BSL_META("  vlan              = %d\n"),
              nh_vlan));
    LOG_CLI((BSL_META("  cos               = %d\n"),
              cos));
    LOG_CLI((BSL_META("  int_prio          = %d\n"),
              int_prio));
    LOG_CLI((BSL_META("  local_key         = " CPUDB_KEY_FMT "\n"),
              CPUDB_KEY_DISP(next_hop_local_key)));
    LOG_CLI((BSL_META("  rx_pkt_drop_count = %d\n"),
              rx_pkt_drop_count));
    LOG_CLI((BSL_META("  tx_error_count    = %d\n"),
              tx_error_count));

    return;
}
#endif  /* BROADCOM_DEBUG */
