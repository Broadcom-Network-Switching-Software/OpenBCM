/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        atp.c
 * Purpose:     Acknowledged Transport Protocol and
 *              Best Effort Transport
 * Requires:    CPU-to-CPU module
 *
 * ATP is based on Jerry Zhao's implementation of "RDP", the reliable
 * data protocol.  ATP provides reliable packet transfer between
 * applications on different CPUs.  Reliability is achieved by
 * sending an ACK for each packet exchanged.  However, a packet
 * may be segmented.  ACKs may not (or may) be sent for each segment.
 *
 * When an ACK is not required, the transport is called BET for Best
 * Effort Transport.
 *
 * The requirement of an ACK may be relaxed in several ways:
 *
 *     1.  The client may be registered as a BET client.
 *     2.  An atp_tx request can be marked "no ACK required".
 *         This overrides the client's setting.
 *     3.  The CPU may be marked as "no ACK required" and all packets
 *         destined for that CPU will be marked no-ack.  This is used
 *         in particular when ATP is used to communicate across the
 *         Linux user/kernel boundary.  This overrides the client's
 *         setting.
 *
 * In any of these cases, a flag is set in the transmitted packet
 * and this alone determines whether the receive side will send an
 * ACK for the packet.
 *
 * On a per-CPU basis, the atp_tx operation can be short-circuited and
 * an application's callback registered to replace the operation.  This
 * is done by calling atp_tx_override_set, giving the destination key
 * and the override function.  Setting this function to NULL will
 * revert to normal atp_tx operation.
 *
 * When the atp function is overridden, the packet format must be
 * carefully formed.  If it matches the atp format, the receiving side
 * may send the packet into the atp stack.  This is okay, but the
 * override TX function must abide by all ATP conventions (which are
 * not that well documented.)  Alternatively (and more likely) the
 * application will handle the reception of the packet.  It can then
 * insert the packet into the callback portion of ATP by calling
 * atp_rx_inject.  The two (extra) pieces of information that must be
 * provided to this call are the source CPU key and the client ID.
 *
 * Note that overriding atp_tx must be done carefully when used
 * in conjunction with TKS discovery protocols.  One possibility is
 * to use different keys for a given CPU to select the transport to
 * use.
 *
 * Other Important Notes
 *
 *    For an RX client, if "REASSEM_BUF" is _not_ specified and a
 * multiple segment ATP transfer occurs (so the segments are communicated
 * up to the callback), this is indicated by the
 * "payload" parameter of the client callback be NULL.  In this case,
 * the pkt->pkt_data blocks are set up with even pointers (0, 2, 4...)
 * pointing to the CPU transport headers of each segment and
 * the odd pointers (1, 3, 5...) pointing to the data portion of
 * each segment.  pkt->blk_count is 2 * (number of segments) in this
 * case.  The data pointers for each segment must be freed, but only
 * one for each pair.  That is, free either the transport header pointer
 * or the data pointer, but not both.
 *    If only one segment occurs, then "payload" is set to point to
 * the proper location in the single buffer.
 *
 * Table of contents (sections to look for below):
 *
 *     TX thread
 *          Check for retransmit timeouts
 *          Check BET queue separately
 *     RX thread
 *          Check for RX callbacks that are ready
 *     Received Packet handling
 *          Handle ATP data (create/update RX transaction)
 *          Handle BET data (make callback from RX handler)
 *          Handle ACK (update TX operation)
 *          ACK out (sent immediately from RX handler)
 *     ATP API functions
 *          Config functions
 *          start/stop
 *          atp_register/unregister (clients)
 *          atp_tx
 */

#include <shared/bsl.h>

#include <assert.h>

#define ATP_ASSERT(stuff) assert stuff

#include <shared/idents.h>

#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <shared/alloc.h>

#include <sal/appl/sal.h>	/* sal_dma_alloc/_free */

#include <bcm/types.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <appl/cputrans/cpu2cpu.h>
#include <appl/cputrans/next_hop.h>
#include <appl/cputrans/atp.h>
#include <appl/cputrans/cputrans.h>
#include <appl/cpudb/cpudb.h>

#include "atp_int.h"
#include "t_util.h"

/****************************************************************
 *
 * Client pointers
 *    A short array, hashed by client ID.
 */

#define _ATP_CLIENT_HASH(client_id) (client_id % _ATP_CLIENT_HASH_MAX)
_atp_client_t *_atp_client_buckets[_ATP_CLIENT_HASH_MAX];

/* Include deleted clients */
#define FOREACH_CLIENT(_client, _bkt) \
    for (_bkt = 0; _bkt  < _ATP_CLIENT_HASH_MAX; _bkt++) \
        for (_client = _atp_client_buckets[_bkt]; _client != NULL; \
             _client = _client->next)

/* Use with ATP client flags */
#define CLI_IS_BET(_cli)  ((_cli)->flags & ATP_F_NO_ACK)
#define CLI_IS_NEXT_HOP(_cli)  ((_cli)->flags & ATP_F_NEXT_HOP)

/* ATP keeps its own hash of CPUDB keys */

/*
 * Per-CPU flags:
 *     CPU_NO_ACK           Force NO ACK on transmits to this CPU
 *                          even for reliable calls.
 */

typedef struct _atp_cpu_info_s _atp_cpu_info_t;
struct _atp_cpu_info_s {
    cpudb_key_t key;
    uint32 flags;
    atp_tx_f override_tx;            /* Use override TX if !NULL */
};
#define _ATP_CPU_VALID           0x1 /* Is entry occupied */
#define _ATP_CPU_NO_ACK          0x2 /* All TX to this CPU are no-ack */
#define _ATP_TX_CXN_INIT         0x4 /* Has a reliable TX convo started */
#define _ATP_RX_CXN_INIT         0x8 /* Has a reliable RX convo started */

static int atp_cpu_max;

static _atp_cpu_info_t _atp_cpu_info[CPUDB_CPU_MAX];

#define CPU_VALID_IDX(_idx) ((_idx) >= 0 && ((_idx) < CPUDB_CPU_MAX))

#define CPU_VALID(_idx) (CPU_VALID_IDX(_idx) && \
    (_atp_cpu_info[_idx].flags & _ATP_CPU_VALID))

#define CPU_KEY(_idx) \
    (*(CPU_VALID(_idx) ? &_atp_cpu_info[_idx].key : &cpudb_bcast_key))

/* This tracks number of times DB has been updated; used for TX transmits */
static int atp_db_update_count;


#if defined(BROADCOM_DEBUG) && defined(ATP_LONG_CALLBACK_TRACKER)
volatile sal_usecs_t atp_rx_in;
volatile sal_usecs_t atp_rx_out;
volatile int atp_rx_long_callbacks;
volatile atp_client_cb_f atp_rx_long_cb_ptr;
volatile atp_client_cb_f atp_rx_ptr;
#endif /* BROADCOM_DEBUG && ATP_LONG_CALLBACK_TRACKER */

/****************************************************************
 * Local configuration variables
 */
static int atp_tx_thread_priority = ATP_THREAD_PRIORITY_DEFAULT;
static int atp_rx_thread_priority = ATP_THREAD_PRIORITY_DEFAULT;

/*
 * The driver list of low level transport calls; currently only
 * used for RX registration and init-time allocation.
 */
static bcm_trans_ptr_t *_atp_trans_ptr = &bcm_trans_ptr;

/****************************************************************
 *
 * Threads and synchronization
 *
 * There are two threads in ATP:  ATP_RX and ATP_TX.  In addition,
 * callbacks are registered with RX, so that thread is involved.
 * Finally, application threads will call ATP functions.
 *
 * ATP_TX takes care of retransmitting packets (when ACKs do not
 * arrive in time) and indicating timeouts.  It also processes
 * packets received from BCM-RX that have the ACK opcode.
 *
 * Most of ATP RX action happens in RX packet handling callbacks,
 * _atp_rx_callback and _atp_next_hop_callback which are registered
 * to get packets from lower layers.  But ATP callbacks (to ATP
 * client registered functions) happen in the ATP_RX thread.
 *
 * Synchronization, 11/05:  Two mutexes, one for RX and one for
 * TX, are now used.  To change the configuration, both should
 * be taken.  RX callbacks hold the RX mutex and may call TX,
 * so the order of taking mutexes MUST be:
 *
 *    ATP_RX_LOCK -> ATP_TX_LOCK -> ATP_TX_UNLOCK -> ATP_RX_UNLOCK
 *
 * In addition (to allow TX loopback to enqueue an RX transaction)
 * there is a low level queue mutex used on RX transaction queues.
 *
 * Programming requirement:
 *    An ATP_TX callback MUST NOT call an ATP function that takes
 * the RX lock.
 */

static sal_mutex_t atp_tx_mutex;  /* High level TX thread */
static sal_mutex_t atp_rx_mutex;  /* High level RX thread */
static sal_mutex_t atp_rxq_mutex;  /* Low level queue protection */

static sal_sem_t atp_tx_sem;
static sal_sem_t atp_rx_sem;

static volatile sal_thread_t atp_tx_thread_id        = SAL_THREAD_ERROR;
static volatile sal_thread_t atp_rx_thread_id        = SAL_THREAD_ERROR;
static volatile int atp_tx_thread_exit;   /* Forces exit when true */
static volatile int atp_rx_thread_exit;   /* Forces exit when true */

/* Transmit thread lock */
#define ATP_TX_LOCK          sal_mutex_take(atp_tx_mutex, sal_sem_FOREVER)
#define ATP_TX_UNLOCK        sal_mutex_give(atp_tx_mutex)

/* Receive thread lock */
#define ATP_RX_LOCK          sal_mutex_take(atp_rx_mutex, sal_sem_FOREVER)
#define ATP_RX_UNLOCK        sal_mutex_give(atp_rx_mutex)

/* Low level RX transaction queue lock */
#define ATP_RXQ_LOCK         sal_mutex_take(atp_rxq_mutex, sal_sem_FOREVER)
#define ATP_RXQ_UNLOCK       sal_mutex_give(atp_rxq_mutex)

/* Configuration (both TX and RX) lock */
#define ATP_LOCK             do { ATP_RX_LOCK; ATP_TX_LOCK; } while (0)
#define ATP_UNLOCK           do { ATP_TX_UNLOCK; ATP_RX_UNLOCK; } while (0)

static volatile int init_done;
static volatile int base_init_done;
static volatile int _atp_running;     /* Busy? */

/****************************************************************
 * Preallocation pointers
 */

static int atp_retry_timeout    = ATP_RETRY_TIMEOUT_DEFAULT;
static int atp_retry_count      = ATP_RETRY_COUNT_DEFAULT;
static int atp_tx_pool_size     = ATP_TX_TRANSACT_DEFAULT;
static int atp_rx_pool_size     = ATP_RX_TRANSACT_DEFAULT;
static atp_timeout_cb_f atp_timeout_cb;

/* For now, one BET queue; maybe by priority later */
static _atp_tx_trans_t *bet_queue;
static _atp_tx_trans_t *bet_queue_tail;

/****************************************************************
 * Parameters
 */

static uint32 _atp_flags;       /* Setup flags; see atp.h  */
static uint32 _atp_units;       /* Unit bitmap for register/unregister */

static volatile int _atp_seg_len      = ATP_SEG_LEN_DEFAULT;

/****************************************************************
 * Default client parameters
 */

static int _atp_cos = ATP_COS_DEFAULT;
static int _atp_vlan = ATP_VLAN_DEFAULT;


/****************************************************************
 * Counters
 */

static volatile int bet_rx_drop;    /* Number of BET RX dropped pkts */
static volatile int atp_rx_drop;    /* Number of ATP RX dropped pkts */
static volatile int mem_rx_drop;    /* Could alloc mem and dropped pkt */
static volatile int slf_rx_drop;    /* Source look up failure */

static volatile int old_rx_trans_drop;  /* Source look up failure */

static volatile int tx_sleep_count;    /* Current number of TX waits */

#if defined(BROADCOM_DEBUG)
#define INCR_COUNTER(counter) ++(counter)
static volatile uint32 rxt_create;     /* RX trans create */
static volatile uint32 txt_create;
static volatile uint32 rxt_free;       /* RX trans free */
static volatile uint32 rxraw_free;     /* RX trans free, raw */
static volatile uint32 rxraw_grab;     /* RX trans grab, raw */
static volatile uint32 txraw_grab;
static volatile uint32 cli_del_tx_busy;  /* Client delete failed for busy TX */
static volatile uint32 clients_deleted;
static volatile uint32 reassem_alloc_fail;
static volatile uint32 rx_trans_fail;
static volatile uint32 rxt_pkt_alloc_fail;
static volatile uint32 rx_mseg_alloc_fail;
static volatile uint32 tx_trans_fail;
static volatile uint32 txt_pkt_alloc_fail;
static volatile uint32 lb_buf_alloc_fail;
static volatile uint32 gc_deferrals;
static volatile uint32 gc_blocked;
static volatile uint32 tx_timeout_cnt;
static volatile uint32 tx_retry_cnt;
static volatile uint32 stale_rx_trans;
static volatile uint32 rx_pkt_drops;
static volatile uint32 ack_pkt_drops;
static volatile uint32 tx_data_alloc_fail;
static volatile uint32 rx_data_alloc_fail;
static volatile uint32 invalid_client_cnt;
static volatile uint32 invalid_dest_cpu_cnt;
static volatile uint32 lb_pkt_send_fail;
static volatile uint32 tx_simple_send_fail;
static volatile uint32 tx_trans_setup_fail;
static volatile uint32 tx_send_fail;
static volatile uint32 atp_not_running;
#else
#define INCR_COUNTER(counter)
#endif /* BROADCOM_DEBUG */

/*
 * Both ATP-TX and ATP-RX need to receive packets.  Each has its
 * own queue, both protected by the following mutex.
 * If both this and ATP_LOCK both are taken, the order should be
 *     ATP_LOCK -> ATP_PKT_DATA_LOCK -> ATP_PKT_DATA_UNLOCK -> ATP_UNLOCK
 */

static sal_mutex_t atp_pkt_data_mutex;

#define ATP_PKT_DATA_LOCK     sal_mutex_take(atp_pkt_data_mutex, \
                              sal_sem_FOREVER)
#define ATP_PKT_DATA_UNLOCK   sal_mutex_give(atp_pkt_data_mutex)

_atp_pkt_data_t *_atp_pkt_data_freelist;

_atp_pkt_data_t *_atp_rcv_data_queue;   /* For ATP-RX */
_atp_pkt_data_t *_atp_rcv_data_queue_tail;

_atp_pkt_data_t *_atp_trx_data_queue;   /* For ATP-TX */
_atp_pkt_data_t *_atp_trx_data_queue_tail;

/****************************************************************
 *
 * TX and RX Transactions:
 *
 * Each ATP operation results in a TX transaction being tracked
 * by the sender and an RX transaction being tracked by the
 * receiver.  Transactions are maintained on a per-client,
 * per-CPU (other side) basis.
 *
 * Whether a transaction has been sent to/received from a
 * CPU is indicated by the TX_CXN/RX_CXN bit in atp_cpu_info.
 * These are detected by sequence numbers as described below.
 *
 * tx/rx_trans_pool is an allocation pointer used for freeing later.
 * tx/rx_trans_freelist is a pointer to the head of the freelist
 *
 * The queued transactions are kept in the client structure,
 * indexed by CPU.
 *
 * RX transactions have an ACK packet data pointer associated
 * at allocation time which is reused.  They use one packet which
 * holds pointers to all the data that arrives.  Each data packet
 * uses 2 pointer blocks, one for the header and one for the
 * start of the payload.
 *
 * For the situation where the queue is empty, but an old
 * transaction is received, an ACK packet data buffer is kept
 * in the client structure.
 * 
 * ATP transactions are tracked by "sequence numbers" which are
 * kept on a per-client, per-CPU basis and generated on TX.
 * The ATP sequence number 0 is treated specially.  It is only
 * used on TX on the first transaction from a CPU (no matter what
 * client).  This marks the beginning of a "connection" which
 * is a series of unidirectional transactions between two CPUs.
 * If the sequence number 0 is ever seen again, this is an
 * indication that the remote side has reset and the transaction
 * information for that CPU is purged.
 *
 * Otherwise, sequence numbers are tracked per (client, dest-CPU)
 * pair.  They are set to 1 on a "wrap" condition.
 */

/* Transmit transaction queues, preallocated */ 
static _atp_tx_trans_t *tx_trans_pool;               /* Allocated */
static _atp_tx_trans_t *tx_trans_freelist;  /* Current free list */

static _atp_rx_trans_t *rx_trans_pool;               /* Allocated once */
static _atp_rx_trans_t *rx_trans_freelist;  /* Current free list */

static uint8 *ack_pkt_data;
int atp_ack_pkt_data_from_heap = 0; /* DMA pool to begin-with */

/* How many TX operations pending */
volatile int atp_tx_pending;

static cpudb_key_t _atp_local_key;

#define IS_LOCAL_CPU_KEY(_key) (CPUDB_KEY_EQUAL(_atp_local_key, _key))

#define ATP_TX_THREAD_WAKE         sal_sem_give(atp_tx_sem)
#define ATP_RX_THREAD_WAKE         sal_sem_give(atp_rx_sem)

/* If init not done, call init function */
#define ATP_INIT_CHECK          \
    if (!init_done) BCM_IF_ERROR_RETURN(_atp_init())

/* If init not done, call init function */
#define BASE_INIT_CHECK          \
    if (!base_init_done) BCM_IF_ERROR_RETURN(_atp_base_init())

#define ATP_TIMEOUT             (atp_retry_timeout * atp_retry_count)

/* Don't need to timeout if nothing happening */
#define ATP_LONG_TIMEOUT        sal_sem_FOREVER

/*
 * Set up a packet block entry from the given data; the CPUTRANS header
 * goes in the first block and the payload ptr in the second.
 */
#define SET_PKT_BLK_DATA(_seg, _pkt, _buf, _len)                         \
    do {                                                                 \
        int _d_ofs;                                                      \
        _d_ofs = 2 * (_seg);                                             \
        /* CPUTRANS header.... */                                        \
        (_pkt)->pkt_data[_d_ofs].data = _buf;                            \
        (_pkt)->pkt_data[_d_ofs++].len = CPUTRANS_HEADER_BYTES;          \
        /* .... Payload data */                                          \
        (_pkt)->pkt_data[_d_ofs].data = (_buf) + CPUTRANS_HEADER_BYTES;  \
        (_pkt)->pkt_data[_d_ofs].len =                                   \
             (_len) - CPUTRANS_HEADER_BYTES;                             \
    } while (0)


/*
 * Get the current sequence number for a BET transaction controlled by pkt
 * We only use this cookie while the packet is being accumulated.
 */

#define BET_RX_SEQ_NUM(pkt) (PTR_TO_INT((pkt)->cookie))
#define BET_RX_SEQ_NUM_SET(pkt, seq) (pkt)->cookie = INT_TO_PTR(((int)(seq)))

/*
 * Set and update a sequence number; on wrap, skip 0 which indicates a reset.
 * This should never return 0 which is a special marker for start of convo.
 */
#define _SEQ_NUM_GET(_dest, _sn)                                        \
    if (++(_sn) == 0) (_dest) = (_sn) = 1;                              \
    else (_dest) = (_sn)

/*
 * Set a TX sequence number; depends on if BET or ACK
 * If BET, _cpu is ignored.
 * If reliable, check CPU's TX_CXN_INIT flag and use SN 0 if not set.
 * Set the TX_CXN_INIT bit at that time.
 */
#define TX_SEQ_NUM_GET(no_ack, _dest, _cli, _cpu) do {                  \
        if (no_ack) {                                                   \
            _SEQ_NUM_GET(_dest, (_cli)->bet_tx_seq_num);                \
        } else {                                                        \
            if (!(_atp_cpu_info[_cpu].flags & _ATP_TX_CXN_INIT)) {      \
                _dest = (_cli)->cpu[_cpu].tx_seq_num = 0;               \
                _atp_cpu_info[_cpu].flags |= _ATP_TX_CXN_INIT;          \
            } else {                                                    \
                _SEQ_NUM_GET(_dest, (_cli)->cpu[_cpu].tx_seq_num);      \
        }                                                               \
        }                                                               \
    } while (0)

#define TX_BET_SEQ_NUM_GET(_dest, _cli) \
   _SEQ_NUM_GET(_dest, (_cli)->bet_tx_seq_num)

STATIC void atp_tx_thread(void *cookie);
STATIC void atp_rx_thread(void *cookie);
STATIC bcm_rx_t _atp_rx_callback(int unit, bcm_pkt_t *pkt, void *cookie);

STATIC int _atp_base_init(void);
STATIC int _atp_init(void);
STATIC void atp_cleanup(void);

STATIC bcm_rx_t _atp_next_hop_callback(cpudb_key_t src_key,
                                       int mplx_num,
                                       int unit,
                                       int port,
                                       uint8* pkt_buf,
                                       int len,
                                       void *cookie);

STATIC int _atp_end_threads(int retries);

STATIC void bet_nh_free_tx_cb(int rv, uint8 *pkt_buf, void *cookie);
STATIC void bet_c2c_free_tx_cb(uint8 *pkt_buf, void *cookie);

STATIC void _atp_c2c_tx_callback(uint8 *pkt_buf, void *cookie);
STATIC void _atp_nh_tx_callback(int rv, uint8 *pkt_buf, void *cookie);

STATIC void atp_rx_ack(_atp_client_t *cli, int cpu, _atp_rx_trans_t *rx_trans,
                       uint16 seq_num,
                       uint8 *pkt_buf, int pkt_len);

STATIC void bet_rx_pkt_drop(_atp_client_t *client, int src_cpu, int err);

STATIC int atp_db_update_locked(cpudb_ref_t db_ref);

STATIC bcm_rx_t atp_data_handle(int src_cpu, uint8 *pkt_buf, int pkt_len,
                                _atp_hdr_t *_atp_hdr);
STATIC bcm_rx_t bet_data_handle(int src_cpu, uint8 *pkt_buf, int len,
                                _atp_hdr_t *_atp_hdr);
STATIC bcm_rx_t atp_ack_handle(int src_cpu, _atp_hdr_t *_atp_hdr);

/*
 * Function:
 *      _atp_hdr_pack/unpack
 * Purpose:
 *      Pack/Unpack and ATP header into/from a packet buffer
 * Parameters:
 *
 * Returns:
 *      BCM_E_XXX
 */

/* Set the sequence number given the start of CPU trans header */
#define ATP_SEQ_NUM_SET(hdr_data, val) \
    PACK_SHORT(&(hdr_data[ATP_SEQ_NUM_OFS]), val)

/* Header offset; client ID is packed as a uint16 */
#define _ATP_HDR_OPCODE_OFFSET  ((sizeof(uint16) * 5) + sizeof(uint32) + 2)

STATIC INLINE uint8 _atp_opcode_get(uint8 *hdr_data)
{
    hdr_data = ATP_HEADER_START(hdr_data);
    return hdr_data[_ATP_HDR_OPCODE_OFFSET];
}

STATIC INLINE void
_atp_hdr_unpack(uint8 *hdr_data, _atp_hdr_t *hdr)
{
    uint8 *ptr;
    uint16 val16;

    ptr = ATP_HEADER_START(hdr_data);
    UNPACK_SHORT(ptr, hdr->version);       /* ATP version */
    ptr += sizeof(uint16);
    UNPACK_SHORT(ptr, val16);              /* clientid */
    hdr->client_id = val16;
    ptr += sizeof(uint16);
    UNPACK_LONG(ptr, hdr->hdr_flags);     /* flags */
    ptr += sizeof(uint32);
    UNPACK_SHORT(ptr, hdr->seq_num);      /* Sequence number */
    ptr += sizeof(uint16);
    UNPACK_SHORT(ptr, hdr->tot_bytes);    /* total payload length */
    ptr += sizeof(uint16);
    UNPACK_SHORT(ptr, hdr->start_byte);   /* payload offset */
    ptr += sizeof(uint16);
    hdr->tot_segs = *ptr++;               /* total number of segments */
    hdr->segment = *ptr++;                /* segment */
    hdr->opcode = *ptr++;                 /* opcode */
    hdr->cos = *ptr++;                    /* cos */
}

STATIC INLINE void
_atp_hdr_pack(uint8 *hdr_data, _atp_hdr_t *hdr)
{
    uint8 *ptr;
    uint16 val16;

    ptr = ATP_HEADER_START(hdr_data);
    PACK_SHORT(ptr, ATP_VERSION);         /* ATP version */
    ptr += sizeof(uint16);
    val16 = hdr->client_id;
    PACK_SHORT(ptr, val16);               /* clientid */
    ptr += sizeof(uint16);
    PACK_LONG(ptr, hdr->hdr_flags);       /* flags */
    ptr += sizeof(uint32);
    PACK_SHORT(ptr, hdr->seq_num);        /* Sequence number */
    ptr += sizeof(uint16);
    PACK_SHORT(ptr, hdr->tot_bytes);      /* Total payload length */
    ptr += sizeof(uint16);
    PACK_SHORT(ptr, hdr->start_byte);     /* Offset in payload */
    ptr += sizeof(uint16);
    *ptr++ = hdr->tot_segs;               /* Total number of segments */
    *ptr++ = hdr->segment;                /* Which segment */
    *ptr++ = hdr->opcode;                 /* opcode */
    *ptr++ = hdr->cos;                    /* cos */
}


/*****************************************************************
                                                                  *
 * Client and CPU info management
 * Client allocation/find/free routines
 *     The clients are organized in a small hash, each bucket
 *     being a linked list.
 */

/*
 * Return a pointer to the client structure for the ID, or NULL if not found
 * Assumes lock is held.
 */

STATIC _atp_client_t *
client_find(int client_id)
{
    int b_idx;   /* Bucket index */
    _atp_client_t *cli;

    b_idx = _ATP_CLIENT_HASH(client_id);
    cli = _atp_client_buckets[b_idx];
    while (cli != NULL) {
        /* Ignore "deleted" clients */
        if (cli->client_id == client_id) {
            break;
        }
        cli = cli->next;
    }

    return cli;
}

#define RELEASE_CLIENT_DATA(cli) \
    _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit, \
                                 (cli)->cpu[0].rx_ack_data)

/* Assumes ATP_LOCK held */
STATIC _atp_client_t *
client_id_add(int client_id)
{
    int b_idx;   /* Bucket index */
    _atp_client_t *new_ptr = NULL;
    int cpu;
    uint8 *rx_ack_ptr;
    int bytes;

    new_ptr = sal_alloc(sizeof(_atp_client_t), "ATP-cli");
    if (new_ptr == NULL) {
        return NULL;
    }
    sal_memset((void *)new_ptr, 0, sizeof(_atp_client_t));

    /* Allocate RX ack data, per-client, per-CPU */
    bytes = _ATP_ACK_BYTES * CPUDB_CPU_MAX;
    rx_ack_ptr = NULL;
    _atp_trans_ptr->tp_data_alloc(_atp_trans_ptr->tp_unit,
                                  bytes, 0, (void*)&rx_ack_ptr);
    if (rx_ack_ptr == NULL) {
        sal_free((void *)new_ptr);
        return NULL;
    }
    for (cpu = 0; cpu < CPUDB_CPU_MAX; cpu++) {
        new_ptr->cpu[cpu].rx_ack_data = &rx_ack_ptr[cpu * _ATP_ACK_BYTES];
    }

    new_ptr->client_id = client_id;

    /* Set up default client values */
    new_ptr->cos = _atp_cos;
    new_ptr->vlan = _atp_vlan;

    /* Link client into list */
    b_idx = _ATP_CLIENT_HASH(client_id);
    new_ptr->next = _atp_client_buckets[b_idx];
    _atp_client_buckets[b_idx] = new_ptr;

    return new_ptr;
}

/* Add a key to db; assumes key is not already present */

STATIC int
_atp_key_add(cpudb_key_t key)
{
    int idx;

    for (idx = 0; idx < CPUDB_CPU_MAX; idx++) {
        if (!(_atp_cpu_info[idx].flags & _ATP_CPU_VALID)) {
            sal_memset(&_atp_cpu_info[idx], 0, sizeof(_atp_cpu_info_t));
            _atp_cpu_info[idx].flags = _ATP_CPU_VALID;
            CPUDB_KEY_COPY(_atp_cpu_info[idx].key, key);
            LOG_VERBOSE(BSL_LS_TKS_ATP,
                        (BSL_META("ATP: Adding CPU %d " CPUDB_KEY_FMT_EOLN),
                         idx,
                         CPUDB_KEY_DISP(key)));
            if (idx >= atp_cpu_max) {
                atp_cpu_max = idx+1;
            }
            return idx;
        }
    }

    return -1;
}

/* Find a key in the DB */

STATIC int
_atp_key_lookup(cpudb_key_t key)
{
    int idx;

    for (idx = 0; idx < CPUDB_CPU_MAX; idx++) {
        if ((_atp_cpu_info[idx].flags & _ATP_CPU_VALID) &&
            CPUDB_KEY_EQUAL(_atp_cpu_info[idx].key, key)) {
            return idx;
        }
    }

    return -1;
}

/****************************************************************
 *
 * Low level data handling
 */

/*
 * Check if BET data is ready for service.  This is the case when all
 * the data pointers are non-zero.
 */

STATIC int
packet_data_done(bcm_pkt_t *pkt)
{
    int i;

    for (i = 0; i < pkt->blk_count; i += 2) {
        if (pkt->pkt_data[i].data == NULL) {
            return FALSE;
        }
    }

    return TRUE;
}

/****************************************************************
 *
 * RX handling
 *
 *    TX and RX are mostly independent.  The exception is that
 * ACKs for TX are received by RX.
 */

/*
 * If extra data was allocated, it is pointed to by pkt->alloc_ptr
 */

STATIC void
_atp_rx_pkt_free(bcm_pkt_t *pkt)
{
    int i;

    if (pkt == NULL) {
        return;
    }

    /* Free the alloc ptr if present */
    if (pkt->alloc_ptr != NULL) {
        _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit, pkt->alloc_ptr);
        pkt->alloc_ptr = NULL;
    }

    /* Free the data pointers if present */
    for (i = 0; i < pkt->blk_count; i += 2) {
        if (pkt->pkt_data[i].data != NULL) {
            _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                         pkt->pkt_data[i].data);
            pkt->pkt_data[i].data = NULL;
        }
    }

    /* Return the packet to the CPU transport pool */
    cputrans_rx_pkt_free(pkt);
}

/*
 * Try to delete a transaction; if it's sync and sem has not
 * been given, give the sem and return.
 */

STATIC void
atp_tx_trans_delete(_atp_tx_trans_t *tx_trans)
{
    _atp_client_t *client;  /* Controlling client */
    int cpu;

    client = tx_trans->client;
    cpu = tx_trans->dest_cpu;

    if (tx_trans->flags & _ATP_TX_F_ENQUEUED) { /* dequeue */
        if (tx_trans->prev != NULL) {
            tx_trans->prev->next = tx_trans->next;
        } else { /* First on list */
            if (TX_TRANS_NO_ACK(tx_trans)) {
                bet_queue = tx_trans->next;
            } else {
                if (CPU_VALID_IDX(cpu)) {
                    client->cpu[cpu].tx_trans = tx_trans->next;
                } else {
                    LOG_VERBOSE(BSL_LS_TKS_ATP,
                                (BSL_META("ATP: invalid cpu index %d\n"),
                                 cpu));
                }
            }
        }

        if (tx_trans->next != NULL) {
            tx_trans->next->prev = tx_trans->prev;
        } else { /* Last on list */
            if (TX_TRANS_NO_ACK(tx_trans)) {
                bet_queue_tail = tx_trans->prev;
            } else {
                if (CPU_VALID_IDX(cpu)) {
                    client->cpu[cpu].tx_tail = tx_trans->prev;
                } else {
                    LOG_VERBOSE(BSL_LS_TKS_ATP,
                                (BSL_META("ATP: invalid cpu index %d\n"),
                                 cpu));
                }
            }
        }

        if (TX_TRANS_ACK(tx_trans)) {
            --atp_tx_pending;
        }

        tx_trans->flags &= ~_ATP_TX_F_ENQUEUED;
    }

    if ((tx_trans->tx_sem != NULL) &&
            (tx_trans->flags & _ATP_TX_F_SEM_WAITING)) {
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("TT delete deferred %p\n"),
                   tx_trans));
	tx_trans->flags &= ~_ATP_TX_F_SEM_WAITING;
	sal_sem_give(tx_trans->tx_sem);
        return;
    }
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("TT delete %p\n"),
               tx_trans));

    /* Deallocate the packet */
    if (tx_trans->pkt_list != NULL) {
        ATP_ASSERT((tx_trans->pkt_list->next != tx_trans->pkt_list));
        cputrans_tx_pkt_list_free(tx_trans->pkt_list);
        tx_trans->pkt_list = NULL;
    }

    if (tx_trans->tx_sem != NULL) {
        sal_sem_destroy(tx_trans->tx_sem);
        tx_trans->tx_sem = NULL;
    }
    tx_trans->next = tx_trans_freelist;
    tx_trans_freelist = tx_trans;
}

STATIC void
atp_tx_trans_delete_all(_atp_client_t *client, int cpu)
{
    _atp_tx_trans_t *tx_trans;
    _atp_tx_trans_t *tx_trans_next;

    ATP_TX_LOCK;
    tx_trans = client->cpu[cpu].tx_trans;
    while (tx_trans) {

        /* Call TX callback if necessary */
        if (tx_trans->callback != NULL) {
            tx_trans->callback(tx_trans->pkt_buf, tx_trans->cookie,
                               BCM_E_FAIL);
        }

        tx_trans_next = (_atp_tx_trans_t *)tx_trans->next;
        atp_tx_trans_delete(tx_trans);
        tx_trans = tx_trans_next;
    }
    ATP_TX_UNLOCK;
}

STATIC void
atp_rx_trans_enqueue(_atp_rx_trans_t *new_trans, _atp_client_t *client,
                     int cpu)
{
    new_trans->next = NULL;
    ATP_RXQ_LOCK;
    if (client->cpu[cpu].rx_tail == NULL) {  /* Queue now empty */
        client->cpu[cpu].rx_trans = client->cpu[cpu].rx_tail = new_trans;
    } else {
        new_trans->prev = client->cpu[cpu].rx_tail;
        client->cpu[cpu].rx_tail->next = new_trans;
        client->cpu[cpu].rx_tail = new_trans;
    }
    new_trans->flags |= _ATP_RX_F_ENQUEUED;
    ATP_RXQ_UNLOCK;
}


/* Free and dequeue an RX transaction */

STATIC void
atp_rx_trans_delete(_atp_rx_trans_t *rx_trans)
{
    _atp_client_t *client;  /* Controlling client */
    int cpu;

    client = rx_trans->client;
    cpu = rx_trans->src_cpu;

    ATP_RXQ_LOCK;
    if (rx_trans->flags & _ATP_RX_F_ENQUEUED) { /* dequeue */
        if (rx_trans->prev != NULL) {
            rx_trans->prev->next = rx_trans->next;
        } else { /* First on list */
            client->cpu[cpu].rx_trans = rx_trans->next;
        }

        if (rx_trans->next != NULL) {
            rx_trans->next->prev = rx_trans->prev;
        } else { /* Last on list */
            client->cpu[cpu].rx_tail = rx_trans->prev;
        }

        rx_trans->flags &= ~_ATP_RX_F_ENQUEUED;
    }
    ATP_RXQ_UNLOCK;

    if (rx_trans->pkt != NULL) {
        _atp_rx_pkt_free(rx_trans->pkt);
        rx_trans->pkt = NULL;
    }

    if (rx_trans->lb_data != NULL) {
        _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                     rx_trans->lb_data);
        rx_trans->lb_data = NULL;
    }

    ATP_RXQ_LOCK;
    rx_trans->next = rx_trans_freelist;
    rx_trans_freelist = rx_trans;
    ATP_RXQ_UNLOCK;

    INCR_COUNTER(rxt_free);
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("RT delete %p\n"),
               rx_trans));
}

STATIC void
atp_rx_trans_delete_all(_atp_client_t *client, int cpu)
{
    _atp_rx_trans_t *rx_trans;
    _atp_rx_trans_t *rx_trans_next;

    rx_trans = client->cpu[cpu].rx_trans;
    while (rx_trans) {
        rx_trans_next = (_atp_rx_trans_t *)rx_trans->next;
        atp_rx_trans_delete(rx_trans);
        rx_trans = rx_trans_next;
    }
}

STATIC void
_atp_cpu_purge(int idx)
{
    int bkt;
    _atp_client_t *client;

    FOREACH_CLIENT(client, bkt) {
        if (client->cpu[idx].cpu_flags & ATP_CPU_ACK_PENDING) {
            LOG_WARN(BSL_LS_TKS_ATP,
                     (BSL_META("ATP: purge cpu %d while ack pending\n"),
                      idx));
        }
        client->cpu[idx].cpu_flags = 0;
        client->cpu[idx].tx_seq_num = 0;
        client->cpu[idx].rx_seq_num = 0;

        atp_tx_trans_delete_all(client, idx);
        if (client->cpu[idx].rx_tail != NULL) {
            LOG_WARN(BSL_LS_TKS_ATP,
                     (BSL_META("ATP: purge cpu %d while RX trans pending\n"),
                      idx));
        }
        atp_rx_trans_delete_all(client, idx);
        if (client->cpu[idx].bet_rx_pkts != NULL) {
            bet_rx_pkt_drop(client, idx, FALSE);
        }
    }

    /* Clear convo init flags */
    _atp_cpu_info[idx].flags &= ~(_ATP_TX_CXN_INIT | _ATP_RX_CXN_INIT);
}

/* Assumes lock held */

STATIC void
_atp_cpu_remove(int idx)
{
    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: Removing CPU %d\n"),
                 idx));
    _atp_cpu_purge(idx);
    _atp_cpu_info[idx].flags = 0;
}

/*
 * Delete a client.  
 * If check_tx is set, then the TX queue is checked to see if any
 * transactions are _present_.  If so, _E_BUSY is returned.
 *
 * Otherwise RX queue is emptied and the client is removed.
 */

STATIC int
client_delete(_atp_client_t *client, int check_tx)
{
    int cpu;

    if (check_tx) {
        /* Return BUSY if any TX transactions are present */
        for (cpu = 0; cpu < CPUDB_CPU_MAX; cpu++) {
            if (client->cpu[cpu].tx_trans != NULL) {
                INCR_COUNTER(cli_del_tx_busy);
                return BCM_E_BUSY;
            }
        }
    }

    INCR_COUNTER(clients_deleted);
    if (client->prev == NULL) {
        _atp_client_buckets[_ATP_CLIENT_HASH(client->client_id)] = client->next;
    } else {
        client->prev->next = client->next;
    }
    if (client->next != NULL) {
        client->next->prev = client->prev;
    }

    /* Clean up old RX transactions */
    for (cpu = 0; cpu < CPUDB_CPU_MAX; cpu++) {
        atp_rx_trans_delete_all(client, cpu);
    }    

    /* Clean up TX transactions */
    for (cpu = 0; cpu < CPUDB_CPU_MAX; cpu++) {
        atp_tx_trans_delete_all(client, cpu);
    }    

    RELEASE_CLIENT_DATA(client);
    sal_free((void *)client);

    return BCM_E_NONE;
}

/*
 * Allocate a data buffer and copy the rx_trans data into it.
 * This reassembles the packet data into a uniform buffer.
 */

STATIC uint8 *
_pkt_reassem(_atp_rx_trans_t *rx_trans)
{
    uint8 *pkt_buf;
    bcm_pkt_blk_t *blk;
    int i;
    int offset;

    /* Allocate enough space for one CPUTRANS header all pkt data */
    pkt_buf = NULL;
    _atp_trans_ptr->tp_data_alloc(_atp_trans_ptr->tp_unit,
        rx_trans->payload_len + CPUTRANS_HEADER_BYTES, 0, (void*)&pkt_buf);
    if (pkt_buf == NULL) {
        INCR_COUNTER(reassem_alloc_fail);
        return NULL;
    }

    /* Copy in the first CPU header block */
    blk = &rx_trans->pkt->pkt_data[0];
    sal_memcpy(pkt_buf, blk->data, CPUTRANS_HEADER_BYTES);
    offset = CPUTRANS_HEADER_BYTES;

    /* Copy in data from odd blocks of packet */
    for (i = 0; i < rx_trans->_atp_hdr.tot_segs; i++) {
        blk = &rx_trans->pkt->pkt_data[2 * i + 1];
        sal_memcpy(&pkt_buf[offset], blk->data, blk->len);
        offset += blk->len;
    }

    return pkt_buf;
}

/*
 * Process a complete RX transaction.
 */

STATIC void
rx_trans_process(int cpu,
                 int client_id,
                 _atp_client_t *client,
                 _atp_rx_trans_t *rx_trans)
{
    bcm_rx_t rv = BCM_RX_HANDLED;
    bcm_pkt_t *pkt = NULL;
    uint8 *payload = NULL;
    int do_callback = TRUE;

    if (rx_trans->flags & _ATP_RX_F_LOOPBACK) {
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("ATP RX: Loopback packet cli %d\n"),
                   client->client_id));
        payload = rx_trans->lb_data + CPUTRANS_HEADER_BYTES;
    } else { /* Non-loopback; check for reassembly and multiple segments */
        pkt = rx_trans->pkt;
        if (client->flags & ATP_F_REASSEM_BUF) {
            pkt->alloc_ptr = _pkt_reassem(rx_trans);
            if (pkt->alloc_ptr == NULL) {
                LOG_WARN(BSL_LS_TKS_ATP,
                         (BSL_META("ATP RX: Failed to alloc for reassem\n")));
                do_callback = FALSE;
            } else {  /* alloc pointer is set */
                payload = (uint8 *)pkt->alloc_ptr + CPUTRANS_HEADER_BYTES;
            }
        } else {
            if (rx_trans->_atp_hdr.tot_segs == 1) {
                payload = pkt->pkt_data[1].data;
            } else {   /* Multiple segments, indicate w/ NULL payload */
                payload = NULL;
            }
        }
    }

    if (do_callback) {
        if (client->callback != NULL) {
#if defined(BROADCOM_DEBUG) && defined(ATP_LONG_CALLBACK_TRACKER)
            atp_rx_in = sal_time_usecs();
            atp_rx_out = 0;
            atp_rx_ptr = client->callback;
#endif   /* BROADCOM_DEBUG */
            rv = client->callback(CPU_KEY(cpu),
                                  client_id,
                                  pkt,
                                  payload,
                                  rx_trans->payload_len,
                                  client->cookie);
#if defined(BROADCOM_DEBUG) && defined(ATP_LONG_CALLBACK_TRACKER)
            atp_rx_out = sal_time_usecs();
            if (atp_rx_out - atp_rx_in > 1000000) {
                ++atp_rx_long_callbacks;
                if (atp_rx_long_cb_ptr == NULL) {
                    atp_rx_long_cb_ptr = atp_rx_ptr;
                }
            }
#endif /* BROADCOM_DEBUG */
        }
    }

    if (rv == BCM_RX_HANDLED_OWNED) {
        rx_trans->lb_data = NULL;
        rx_trans->pkt = NULL;
    } else {
        /* The packet is not stolen, free it now. */
        if (rx_trans->flags & _ATP_RX_F_LOOPBACK) {
            _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                         rx_trans->lb_data);
            rx_trans->lb_data = NULL;
        } else {
            _atp_rx_pkt_free(rx_trans->pkt);
        }
    }

    rx_trans->pkt = NULL;
    rx_trans->flags |= _ATP_RX_F_HANDLED;
}

STATIC int
rx_stale(_atp_rx_trans_t *_trans, sal_usecs_t _cur_time)
{
    int rv = FALSE;

    if (!(_trans->flags & _ATP_RX_F_DATA_READY)) {
        int dt = SAL_USECS_SUB(_cur_time, _trans->rcvd_time);
        rv = ((dt < 0) || (dt > (4 * ATP_TIMEOUT)));
    }
        
    return rv;

}


STATIC void
_atp_rcv_data_queue_process(_atp_client_t *client, int cpu)
{
    _atp_rx_trans_t *rx_trans;

    rx_trans = client->cpu[cpu].rx_trans;

    if (rx_trans != NULL) {
        sal_usecs_t cur_time;
        _atp_rx_trans_t *rx_trans_next;

        cur_time = sal_time_usecs();

        while (rx_trans != NULL) {
            
            rx_trans_next = rx_trans->next;
            
            /* Mark stale RX transactions */
            if (rx_stale(rx_trans, cur_time)) {
                INCR_COUNTER(stale_rx_trans);
                atp_rx_trans_delete(rx_trans);
            } else {
                if (rx_trans->flags & _ATP_RX_F_DATA_READY &&
                    !(rx_trans->flags & _ATP_RX_F_HANDLED)) {
                    rx_trans_process(cpu, client->client_id, client, rx_trans);
                }
                
                if (rx_trans->flags & _ATP_RX_F_HANDLED) {
                    atp_rx_trans_delete(rx_trans);
                }
            }
            rx_trans = rx_trans_next;
        }
    }
}

/*
 * Go through all RX transactions; send up the stack if ready to go.
 */

STATIC void
rx_callbacks_check(void)
{
    int idx;
    _atp_client_t *cli;
    int cpu;

    ATP_RX_LOCK;
    FOREACH_CLIENT(cli, idx) {
        for (cpu = 0; cpu < atp_cpu_max; cpu++) {
            _atp_rcv_data_queue_process(cli, cpu);
        }
    }
    ATP_RX_UNLOCK;
}

/* pkt_buf points to start of L2 header */

STATIC bcm_rx_t
_atp_rx_pkt_process(int src_cpu, uint8 *pkt_buf, int pkt_len)
{
    _atp_hdr_t _atp_hdr;
    bcm_rx_t rv = BCM_RX_NOT_HANDLED;

    _atp_hdr_unpack(pkt_buf, &_atp_hdr);

    if (!(_atp_hdr.hdr_flags & _ATP_HDR_NO_ACK)) {
        rv = atp_data_handle(src_cpu, pkt_buf, pkt_len, &_atp_hdr);
    } else {
        rv = bet_data_handle(src_cpu, pkt_buf, pkt_len, &_atp_hdr);
    }

#if defined(BCM_RXP_DEBUG)
    if (rv == BCM_RX_HANDLED_OWNED) {
        bcm_rx_pool_own(pkt_buf, "atp_pkt_proc");
    }
#endif
    return rv;
}

STATIC int
_handle_rx_data(_atp_pkt_data_t *pkt_p)
{
    cpudb_key_t src_key;
    int src_cpu;
    uint8 *pkt_buf;
    int len;

    pkt_buf = pkt_p->pkt_buf;

    /* Look for the source CPU */
    CPUDB_KEY_UNPACK(&pkt_buf[CPUTRANS_SRC_KEY_OFS], src_key);
    src_cpu = _atp_key_lookup(src_key);
    if (src_cpu < 0) {
        if (_atp_flags & ATP_F_LEARN_SLF) {
            src_cpu = _atp_key_add(src_key);
            if (src_cpu < 0) {
                LOG_VERBOSE(BSL_LS_TKS_ATP,
                            (BSL_META("ATP pkt: could not add key\n")));
                ++mem_rx_drop;
                return BCM_RX_HANDLED;
            }
        } else {
            LOG_VERBOSE(BSL_LS_TKS_ATP,
                        (BSL_META("ATP pkt: SLF drop\n")));
            ++slf_rx_drop;
            return BCM_RX_HANDLED;
        }
    }

    /* Process the packet */
    len = pkt_p->len;

    return _atp_rx_pkt_process(src_cpu, pkt_buf, len);
}

STATIC void
rx_process_pkt_data(void)
{
    _atp_pkt_data_t *cur_p;
    _atp_pkt_data_t *next_p;
    int rv;

    ATP_RX_LOCK;
    ATP_PKT_DATA_LOCK; /* Steal the current queue of pkts */
    cur_p = _atp_rcv_data_queue;
    _atp_rcv_data_queue_tail = NULL;
    _atp_rcv_data_queue = NULL;
    ATP_PKT_DATA_UNLOCK;

    while (cur_p != NULL) {
        next_p = cur_p->next;

        rv = _handle_rx_data(cur_p);
        if (rv != BCM_RX_HANDLED_OWNED) { /* Free packet data */
            _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                         cur_p->pkt_buf);
        }
        ATP_PKT_DATA_LOCK; /* Free the pkt struct */
        cur_p->next = _atp_pkt_data_freelist;
        _atp_pkt_data_freelist = cur_p;
        ATP_PKT_DATA_UNLOCK;
        
        cur_p = next_p;
    }
    ATP_RX_UNLOCK;
}

/****************************************************************
 *
 * ATP RX Thread
 */

STATIC void
atp_rx_thread(void *cookie)
{
    COMPILER_REFERENCE(cookie);

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: RX Thread starting\n")));
    atp_rx_thread_exit = FALSE;
    while (1) {
        int sleep_time = sal_sem_FOREVER;

        sal_sem_take(atp_rx_sem, sleep_time);
        if (atp_rx_thread_exit) {  /* Exit forced */
            break;
        }

        rx_process_pkt_data();
        rx_callbacks_check();

    }

    atp_rx_thread_id = SAL_THREAD_ERROR;
    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: RX Thread exiting\n")));
    sal_thread_exit(0);
}

/****************************************************************
 *
 * Packet handling routines
 */

/* RX trans create assumes ATP_RX_LOCK is held and if loopback is false,
   _atp_hdr is valid
 */

STATIC _atp_rx_trans_t *
atp_rx_trans_create(_atp_client_t *client, int src_cpu, uint8 *pkt_buf,
                    int pkt_len, _atp_hdr_t *_atp_hdr, int loopback)
{
    _atp_rx_trans_t *rx_trans;
    bcm_pkt_t *pkt;
    int seg;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP new RX, cpu %d, new seq %d, cli %d, old seq %d,"
                        "flags: %sseen\n"),
               src_cpu, (_atp_hdr != NULL) ? _atp_hdr->seq_num : -1,
               client->client_id,
               client->cpu[src_cpu].rx_seq_num,
               client->cpu[src_cpu].cpu_flags & ATP_CPU_RX_TRANS_SEEN ?
               "" : "not "));
    if (!loopback) {
        if (_atp_hdr->segment >= _atp_hdr->tot_segs) {
            LOG_WARN(BSL_LS_TKS_ATP,
                     (BSL_META("ATP RX: Bad seg num %d >= tot %d.\n"),
                      _atp_hdr->segment, _atp_hdr->tot_segs));
            return NULL;
        }
    }

    /* Grab a transaction from the free list */
    rx_trans = NULL;
    ATP_RXQ_LOCK;
    if (rx_trans_freelist != NULL) {
        rx_trans = (_atp_rx_trans_t *)rx_trans_freelist;
        rx_trans_freelist = rx_trans->next;
    }
    ATP_RXQ_UNLOCK;

    if (rx_trans == NULL) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP RX freelist empty\n")));
        INCR_COUNTER(rx_trans_fail);
        return NULL;
    }
    INCR_COUNTER(rxt_create);

    rx_trans->next = NULL;
    rx_trans->flags = 0;
    rx_trans->client = client;
    rx_trans->src_cpu = src_cpu;
    rx_trans->rcvd_time = sal_time_usecs();
    rx_trans->lb_data = NULL;

    if (loopback) {
        rx_trans->flags = _ATP_RX_F_DATA_READY | _ATP_RX_F_LOOPBACK;
        rx_trans->lb_data = pkt_buf;
        rx_trans->pkt = NULL;
        rx_trans->payload_len = pkt_len - CPUTRANS_HEADER_BYTES;
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("RT create LB %p\n"),
                   rx_trans));
        return rx_trans;
    }

    /* Payload length doesn't include CPUTRANS header */
    rx_trans->payload_len = pkt_len - CPUTRANS_HEADER_BYTES;

    /* Not a loopback packet; Copy the header, and init members */
    sal_memcpy((void *)&rx_trans->_atp_hdr, _atp_hdr, sizeof(_atp_hdr_t));
    rx_trans->ack_count = 0;
    rx_trans->rcv_segs = 1;

    /* Allocate a receive pkt w/ enough pointers:
     *     Use 2 pointers per segment and link
     *     headers in pointers 0, 2, 4... and data in ptrs 1, 3, 5...
 */
    pkt = rx_trans->pkt = cputrans_rx_pkt_alloc(2 * _atp_hdr->tot_segs);
    if (pkt == NULL) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP RX Could not allocate packet\n")));

        /* Return to freelist; transaction not yet queued */
        atp_rx_trans_delete(rx_trans);
        INCR_COUNTER(rxt_pkt_alloc_fail);
        return NULL;
    }
    pkt->alloc_ptr = NULL;

    /* Put pkt header in first block and payload in second block */
    seg = _atp_hdr->segment;
    SET_PKT_BLK_DATA(seg, pkt, pkt_buf, pkt_len);

    if (_atp_hdr->tot_segs == 1) {
        rx_trans->flags |= _ATP_RX_F_DATA_READY;
    }

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("RT create %p\n"),
               rx_trans));
    return rx_trans;
}


/*
 * Free the BET RX packet controlled by client, for given source CPU.
 * Allocation pointers are stored in the even numbered blocks.
 * err indicates if dropping counts as an error.
 */

STATIC void
bet_rx_pkt_drop(_atp_client_t *client, int src_cpu, int err)
{
    bcm_pkt_t *pkt;
    int count;
    bcm_pkt_blk_t *pkt_blk;
    int i;

    pkt = client->cpu[src_cpu].bet_rx_pkts;
    client->cpu[src_cpu].bet_rx_pkts = NULL;

    if (pkt != NULL) {
        count = pkt->blk_count;
        pkt_blk = pkt->pkt_data;
        for (i = 0; i < count; i += 2) {
            if (pkt_blk[i].data != NULL) {
                _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                             pkt_blk[i].data);
                pkt_blk[i].data = NULL;
            }
        }
        cputrans_rx_pkt_free(pkt);
        if (err) {
            ++bet_rx_drop;
        }
    }
}

/* Allows ACKs to be sent asynchronously */

STATIC void
atp_rx_ack_cb(uint8 *pkt_buf, void *cookie)
{
    _atp_client_cpu_t *cp;

    COMPILER_REFERENCE(pkt_buf);

    cp = (_atp_client_cpu_t *)cookie;
    cp->cpu_flags &= ~ATP_CPU_ACK_PENDING;
}

STATIC void
atp_rx_ack_nh_cb(int rv, uint8 *pkt_buf, void *cookie)
{
    _atp_client_cpu_t *cp;

    COMPILER_REFERENCE(pkt_buf);
    COMPILER_REFERENCE(rv);

    cp = (_atp_client_cpu_t *)cookie;
    cp->cpu_flags &= ~ATP_CPU_ACK_PENDING;
}

/*
 * Send an ACK for the client/cpu; rx_trans MAY BE NULL
 */

STATIC void
atp_rx_ack(_atp_client_t *cli, int cpu, _atp_rx_trans_t *rx_trans,
           uint16 seq_num,
           uint8 *pkt_buf, int pkt_len)
{
    int ack_bytes = 0;
    bcm_pkt_t *pkt;
    int i;
    int rv;
    int cos;
    int next_hop;
    uint32 ct_flags;
    _atp_client_cpu_t *cp;
    uint8 *ack_data;
    next_hop_tx_callback_f ack_nh_cb;
    c2c_cb_f ack_c2c_cb;
        
    if (rx_trans == NULL) {  /* ACK entire frame */
        ack_bytes = 0; /* Signals all done */
        ack_data = cli->cpu[cpu].rx_ack_data;
    } else {  /* RX trans there */
        if (RX_TRANS_DATA_DONE(rx_trans) || rx_trans->pkt == NULL) {
            ack_bytes = rx_trans->payload_len;
        } else {
            pkt = rx_trans->pkt;
            for (i = 1; i < pkt->blk_count; i += 2) {
                if (pkt->pkt_data[i].data != NULL) {
                    ack_bytes += pkt->pkt_data[i].len;
                } else {  /* Break on first unknown data */
                    break;
                }
            }
            if (ack_bytes == 0) {
                /* Don't ack 0-bytes here because that would imply all done */
                return;
            }
        }
    ack_data = rx_trans->ack_data;
    }

    cp = &cli->cpu[cpu];
    cp->atp_hdr.seq_num = seq_num;
    cp->atp_hdr.opcode = _ATP_OPC_ACK;
    cp->atp_hdr.start_byte = ack_bytes;
    cos = cli->cos;
    next_hop = cp->atp_hdr.hdr_flags & _ATP_HDR_NEXT_HOP;
    _atp_hdr_pack(ack_data, (_atp_hdr_t *)&cp->atp_hdr);

    /*
     * If ACK is to be sent immediately, do not set callbacks.
     * Otherwise, set corresponding rx ack callback routines.
     */
    if (cp->atp_hdr.hdr_flags & _ATP_HDR_IMMEDIATE_ACK) {
        ack_nh_cb  = NULL;
        ack_c2c_cb = NULL;
    } else {
        ack_nh_cb  = atp_rx_ack_nh_cb;
        ack_c2c_cb = atp_rx_ack_cb;
        cp->cpu_flags |= ATP_CPU_ACK_PENDING;
    }


    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("Sending ACK to %d cli %d seq %d bytes %d\n"),
               cpu,
               cli->client_id, cp->atp_hdr.seq_num, cp->atp_hdr.start_byte));

    /* To ensure proper TX pkt allocation */
    ct_flags = CPUTRANS_NO_HEADER_ALLOC;
    CPUTRANS_COS_SET(ct_flags, cos);    /* Only cos is needed */

    if (next_hop) {
        rv = next_hop_tx(ack_data,
                         CPUTRANS_HEADER_BYTES,
                         cos,
                         cli->vlan,
                         _atp_seg_len,
                         ct_flags,
                         ATP_PKT_TYPE,
                         CPU_KEY(cpu),
                         ack_nh_cb,
                         (void *)cp);
    } else {
        rv = c2c_tx(CPU_KEY(cpu),
                    ack_data,
                    CPUTRANS_HEADER_BYTES,
                    cos,
                    cli->vlan,
                    CPUTRANS_HEADER_BYTES,    /* Force one segment */
                    ATP_PKT_TYPE,
                    ct_flags,
                    ack_c2c_cb,
                    (void *)cp);
    }

    if (rv != BCM_E_NONE) {
#ifdef BROADCOM_DEBUG
        {
            int proto = ((pkt_buf[52]<<8) |  pkt_buf[53]); /* Always Big Endian */
            LOG_VERBOSE(BSL_LS_TKS_ATP,
                        (BSL_META("ATP: Failed sending ACK to %x:%x for protocol %d (%d): %s\n"),
                         pkt_buf[10], pkt_buf[11], proto, rv, bcm_errmsg(rv)));
        }
#endif /* BROADCOM_DEBUG */
        cp->cpu_flags &= ~ATP_CPU_ACK_PENDING;
    } else if (rx_trans != NULL) {
        rx_trans->ack_count++;
    }
}

/*
 * new_trans is a new RX transaction; verify tail_trans is done and update
 * client pointers.  Assumes ATP_RX_LOCK is held.
 */
STATIC bcm_rx_t
new_rx_trans_add(_atp_client_t *client, _atp_hdr_t *_atp_hdr,
                 _atp_rx_trans_t *new_trans, int src_cpu,
                 uint8 *pkt_buf, int pkt_len)
{
    _atp_rx_trans_t *tail_trans;

    tail_trans = client->cpu[src_cpu].rx_tail;
    if (tail_trans != NULL) {
        /* New transaction; previous pkt should be done */
        if (!(tail_trans->flags & _ATP_RX_F_DATA_READY)) {
            /* Drop tail packet by marking all done */
            LOG_WARN(BSL_LS_TKS_ATP,
                     (BSL_META("ATP RX Dropping non-ready rx trans, cpu %d, "
                      "client %d, seq %d\n"), src_cpu, client->client_id,
                      _atp_hdr->seq_num));
            tail_trans->flags |= _ATP_RX_F_DATA_READY | _ATP_RX_F_HANDLED;
            atp_rx_drop++;
        }
    }

    /* Indicate transaction seen */
    client->cpu[src_cpu].cpu_flags |= ATP_CPU_RX_TRANS_SEEN;
    client->cpu[src_cpu].rx_seq_num = _atp_hdr->seq_num;

    /*
     * If sequence number is > 0, indicate a conversation has
     * started with this CPU
     */
    if (_atp_hdr->seq_num > 0) {
        _atp_cpu_info[src_cpu].flags |= _ATP_RX_CXN_INIT;
    }

    sal_memcpy((_atp_hdr_t *)&client->cpu[src_cpu].atp_hdr, _atp_hdr,
               sizeof(_atp_hdr_t));

    atp_rx_ack(client, src_cpu, new_trans, _atp_hdr->seq_num,
               pkt_buf, pkt_len);

    atp_rx_trans_enqueue(new_trans, client, src_cpu);
    
    return BCM_RX_HANDLED_OWNED;
}

/*
 * Update the current transaction (tail of queue) in cur_trans with
 * info from _atp_hdr.  TAIL must not be NULL on entry.
 */

STATIC bcm_rx_t
current_rx_trans_update(_atp_client_t *client,
                        int src_cpu,
                        _atp_hdr_t *_atp_hdr,
                        uint8 *pkt_buf,
                        int pkt_len)

{
    int seg_idx;
    bcm_rx_t rv = BCM_RX_HANDLED;
    bcm_pkt_blk_t    *pkt_blks;
    _atp_rx_trans_t *cur_trans = NULL;

    /* On going transaction */
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP Ongoing RX: client %d. cpu %d. seq %d\n"),
               client->client_id, src_cpu, client->cpu[src_cpu].rx_seq_num));
    if (_atp_hdr->segment >= _atp_hdr->tot_segs) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP RX: Bad seg num %d >= tot %d.\n"),
                  _atp_hdr->segment, _atp_hdr->tot_segs));
        return BCM_RX_HANDLED;
    }

    cur_trans = client->cpu[src_cpu].rx_tail;  /* May be NULL */
    if ((cur_trans != NULL) &&
        (!(cur_trans->flags & _ATP_RX_F_DATA_READY))) {
        /* Add data to this transaction */
        seg_idx = _atp_hdr->segment;
        pkt_blks = cur_trans->pkt->pkt_data;

        if (pkt_blks[2 * seg_idx].data == NULL) {     /* New data */
            LOG_DEBUG(BSL_LS_TKS_ATP,
                      (BSL_META("ATP new data\n")));
            SET_PKT_BLK_DATA(seg_idx, cur_trans->pkt, pkt_buf, pkt_len);
            cur_trans->payload_len +=
                pkt_len - CPUTRANS_HEADER_BYTES;
            if (++cur_trans->rcv_segs == _atp_hdr->tot_segs) {
                /* Packet is complete */
                cur_trans->flags |= _ATP_RX_F_DATA_READY;
            }
            rv = BCM_RX_HANDLED_OWNED;
        } else {
            LOG_DEBUG(BSL_LS_TKS_ATP,
                      (BSL_META("ATP old data\n")));
            rv = BCM_RX_HANDLED;
        }
    }

    /* Always ACK the data */
    atp_rx_ack(client, src_cpu, cur_trans, _atp_hdr->seq_num,
               pkt_buf, pkt_len);

    return rv;
}


/****************************************************************
 *
 * ATP handle data ACK
 */

/* Handle an ATP ACK */
STATIC bcm_rx_t
atp_ack_handle(int src_cpu, _atp_hdr_t *_atp_hdr)
{
    _atp_tx_trans_t *trans;
    _atp_client_t *client;
    uint16 new_seq_num;

    new_seq_num = _atp_hdr->seq_num;
    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP ACK from %d cli %d. seq %d. bytes %d. tot %d\n"),
               src_cpu,
               _atp_hdr->client_id, new_seq_num, _atp_hdr->start_byte,
               _atp_hdr->tot_bytes));

    /* Look for the TX operation this is ACK-ing */
    client = client_find(_atp_hdr->client_id);
    if (client == NULL) {
        INCR_COUNTER(invalid_client_cnt);
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP: ACK on NULL client %d\n"),
                  _atp_hdr->client_id));
        return BCM_RX_HANDLED;
    }

    trans = client->cpu[src_cpu].tx_trans;
    while (trans != NULL) {
        if (new_seq_num == trans->_atp_hdr.seq_num) {
            break; /* Found ack'd transaction. */
        }
        trans = trans->next;
    }

    /*
     * At this point, only care if the ACK number == transaction number
     * and this is the first time the ACK has been seen.
     */
    if (trans != NULL &&
            (new_seq_num == trans->_atp_hdr.seq_num) &&
            !(trans->flags & _ATP_TX_F_DONE)) {
        if (_atp_hdr->start_byte == 0 || /* 0 means all ACK'd */
            _atp_hdr->start_byte >= _atp_hdr->tot_bytes) {
            LOG_DEBUG(BSL_LS_TKS_ATP,
                      (BSL_META("ATP ACK SN %d marking done.\n"),
                       new_seq_num));
            trans->flags |= _ATP_TX_F_DONE;
        } else if (trans->bytes_acked < _atp_hdr->start_byte) {
            trans->bytes_acked = _atp_hdr->start_byte;
        }
    } else { /* Redundant ACK */
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("ATP extra ACK SN %d (trans SN %d) from %d\n"),
                   new_seq_num,
                   trans != NULL ? trans->_atp_hdr.seq_num : -1, src_cpu));
    }

    return BCM_RX_HANDLED;
}

/****************************************************************
 * ATP handle packet data.  Create a new transaction if
 * necessary.  If transaction exists, check if this data
 * segment is new.  Indicate if ACK needed and wake thread.
 */

STATIC bcm_rx_t
atp_data_handle(int src_cpu, uint8 *pkt_buf, int pkt_len,
                _atp_hdr_t *_atp_hdr)
{
    _atp_rx_trans_t *new_trans = NULL;
    _atp_client_t *client;
    bcm_rx_t rv = BCM_RX_HANDLED;
    uint16 new_seq_num;
    int16 seq_num_diff; /* Used signed value of same size for difference */

    new_seq_num = _atp_hdr->seq_num;

    client = client_find(_atp_hdr->client_id);
    if (client == NULL) {
        INCR_COUNTER(invalid_client_cnt);
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP rx: Unknown client id %d\n"),
                     _atp_hdr->client_id));
        return BCM_RX_NOT_HANDLED;
    }

    if (!CPU_VALID_IDX(src_cpu)) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP rx: invalid cpu index %d\n"),
                     src_cpu));
        return BCM_RX_NOT_HANDLED;
    }

    /* Protect this queue while updating */

    seq_num_diff = new_seq_num - client->cpu[src_cpu].rx_seq_num;

    /* First check for reset condition:  Seq num goes back to 0 after
     * RX_CXN_INIT is set (that is, after other transactions seen).
     * This is only allowed on the first packet to avoid the race
     * condition of a retransmit on the first packet.
     */
    if ((_atp_cpu_info[src_cpu].flags & _ATP_RX_CXN_INIT) &&
            (new_seq_num == 0) &&
            (!(_atp_hdr->hdr_flags & _ATP_HDR_RETRANSMIT))) {
        /* Special case indicating remote reset; clear CPU state */
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP: New CPU data for cpu %d, cli %d\n"),
                     src_cpu,
                     _atp_hdr->client_id));
        _atp_cpu_purge(src_cpu);
    } else {
        if ((seq_num_diff > 1) && (new_seq_num > 1)) {
            LOG_VERBOSE(BSL_LS_TKS_ATP,
                        (BSL_META("ATP Warning: seq num jump cpu %d, cli %d: new %d. "
                         "diff %d.\n"), src_cpu, client->client_id, new_seq_num,
                         seq_num_diff));
        }
        if ((!(client->cpu[src_cpu].cpu_flags & ATP_CPU_RX_TRANS_SEEN)) ||
                (seq_num_diff != 0) ||
                (new_seq_num == 0)  ||
                (new_seq_num == 1)) {
            /* New transaction */
            new_trans = atp_rx_trans_create(client, src_cpu, pkt_buf,
                                        pkt_len, _atp_hdr, FALSE);
            if (new_trans == NULL) {
                LOG_VERBOSE(BSL_LS_TKS_ATP,
                            (BSL_META("ATP Could not allocate new RX transaction\n")));
            } else {
                rv = new_rx_trans_add(client, _atp_hdr, new_trans, src_cpu,
                                  pkt_buf, pkt_len);
            }
        } else if (seq_num_diff == 0) {
            rv = current_rx_trans_update(client, src_cpu, _atp_hdr,
                                         pkt_buf, pkt_len);
        } else { /* seq_num_diff < 0; old transaction, bad data */
            LOG_VERBOSE(BSL_LS_TKS_ATP,
                        (BSL_META("ATP: old RX data SN %d. Diff %d. cpu %d\n"),
                         new_seq_num,
                         seq_num_diff,
                         /* coverity[dead_error_begin] */
                         src_cpu));
            ++old_rx_trans_drop;
        }
    }
    return rv;
}

STATIC uint8 *
_bet_pkt_reassem(_atp_client_t *client, int src_cpu, int *tot_len)
{
    bcm_pkt_t *pkt;
    int i;
    uint8 *pkt_buf;
    int pi = 0; /* packet buffer offset */

    /* Find the total size needed and allocate the buffer */
    pkt = client->cpu[src_cpu].bet_rx_pkts;
    *tot_len = 0;
    for (i = 1; i < pkt->blk_count; i += 2) {
        *tot_len += pkt->pkt_data[i].len;
    }
    pkt_buf = NULL;
    _atp_trans_ptr->tp_data_alloc(_atp_trans_ptr->tp_unit,
                                  *tot_len, 0, (void*)&pkt_buf);
    if (pkt_buf == NULL) {
        INCR_COUNTER(reassem_alloc_fail);
        return NULL;
    }

    /* Gather up the segments into the new buffer */
    for (i = 1; i < pkt->blk_count; i += 2) {
        sal_memcpy(&pkt_buf[pi], pkt->pkt_data[i].data, pkt->pkt_data[i].len);
        pi += pkt->pkt_data[i].len;
    }

    return pkt_buf;
}

/*
 * Check a multisegment BET packet for consistency with what's being
 * received; create new accumulation packet if necessary.
 */

STATIC bcm_rx_t
bet_rx_multi_seg_check(_atp_client_t *client, int src_cpu, _atp_hdr_t *_atp_hdr,
                       uint8 *pkt_buf, int len, int *make_callback)
{
    int seg_idx;
    bcm_pkt_t *pkt;
    int make_new_pkt = FALSE;
    bcm_rx_t rv = BCM_RX_HANDLED;

    *make_callback = FALSE;

    seg_idx = _atp_hdr->segment;              /* This pkt's segment ID */
    pkt = client->cpu[src_cpu].bet_rx_pkts;       /* Current pkt accumulating */
    if (pkt != NULL) {
        if (_atp_hdr->seq_num != BET_RX_SEQ_NUM(pkt)) {
            /* New packet doesn't match current sequence number; drop
               accumulated packets.
            */
            bet_rx_pkt_drop(client, src_cpu, TRUE);
            make_new_pkt = TRUE;
        } else if ((2 * seg_idx) >= pkt->blk_count) {
            LOG_WARN(BSL_LS_TKS_ATP,
                     (BSL_META("ATP BET bad segment index\n")));
            bet_rx_pkt_drop(client, src_cpu, TRUE);
        } else {
            if (pkt->pkt_data[2 * seg_idx].data == NULL) { /* New segment */
                SET_PKT_BLK_DATA(seg_idx, pkt, pkt_buf, len);
            }

            if (packet_data_done(pkt)) {
                *make_callback = TRUE;
            }
            rv = BCM_RX_HANDLED_OWNED;
        }
    } else {   /* No current packet */
        make_new_pkt = TRUE;
    }

    if (make_new_pkt) {
        if (seg_idx > 0) {   /* Dump the packet; didn't get seg 0 */
            rv = BCM_RX_HANDLED;
        } else {
            /* Alloc new pkt and copy data pointers to this pkt */
            pkt = client->cpu[src_cpu].bet_rx_pkts =
                cputrans_rx_pkt_alloc(2 * _atp_hdr->tot_segs);
            if (pkt == NULL) {
                INCR_COUNTER(rx_mseg_alloc_fail);
                rv = BCM_RX_HANDLED;
            } else {
                SET_PKT_BLK_DATA(seg_idx, pkt, pkt_buf, len);
                BET_RX_SEQ_NUM_SET(pkt, _atp_hdr->seq_num);
                rv = BCM_RX_HANDLED_OWNED;
            }
        }
    }

    return rv;
}

/*
 * Handle Best Effort data; assumes client_index set in _atp_hdr
 * This must support src_cpu < 0 for single segment packets.
 */

STATIC bcm_rx_t
bet_data_handle(int src_cpu, uint8 *pkt_buf, int len, _atp_hdr_t *_atp_hdr)
{
    bcm_rx_t rv = BCM_RX_HANDLED;
    _atp_client_t *client;
    int make_callback = FALSE;
    int cb_len = 0;
    int i;

    /* The callback information */
    uint8 *cb_pkt_payload = NULL;
    bcm_pkt_t *cb_pkt_ptr = NULL;

    /* Find the client pointer for this pkt */
    client = client_find(_atp_hdr->client_id);
    if (client == NULL) {
        INCR_COUNTER(invalid_client_cnt);
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("BET rx: Unknown client id %d\n"),
                     _atp_hdr->client_id));
        return BCM_RX_NOT_HANDLED;
    }

    if (!CPU_VALID_IDX(src_cpu)) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("BET rx: invalid cpu index %d\n"),
                     src_cpu));
        return BCM_RX_NOT_HANDLED;
    }

    /* Is there more than one segment for this packet? */
    if (_atp_hdr->tot_segs > 1) {   /* Yes, multiple segments */
        rv = bet_rx_multi_seg_check(client, src_cpu, _atp_hdr, pkt_buf, len,
                                    &make_callback);
        if (make_callback) {
            /* Check if accumulation necessary */
            if (client->flags & ATP_F_REASSEM_BUF) {
                cb_pkt_payload = _bet_pkt_reassem(client, src_cpu, &cb_len);
                if (cb_pkt_payload == NULL) {
                    make_callback = FALSE;
                } /* else, make_callback is already true */
            } else {
                cb_pkt_ptr = client->cpu[src_cpu].bet_rx_pkts;
                /* Calculate payload length of packet */
                for (i = 1; i < cb_pkt_ptr->blk_count; i += 2) {
                    cb_len += cb_pkt_ptr->pkt_data[i].len;
                }
            }
        }
    } else {   /* Single segment for the packet; make callback w/ data */
        make_callback = TRUE;
        cb_pkt_payload = pkt_buf + CPUTRANS_HEADER_BYTES;
        cb_len = len - CPUTRANS_HEADER_BYTES;
    }

    if (make_callback) {
        if (client->callback != NULL) {
            rv = client->callback(CPU_KEY(src_cpu),
                                  _atp_hdr->client_id,
                                  cb_pkt_ptr,
                                  cb_pkt_payload,
                                  cb_len,
                                  client->cookie);
        }

        if (_atp_hdr->tot_segs > 1) {
            /* Multi segment BET */

            if (rv == BCM_RX_HANDLED_OWNED) {
                /*
                  Callback owns the buffers. If reassembly was
                  requested, the callback owns cb_pkt_payload, and
                  cb_pkt_ptr is NULL. If reassembly was *not*
                  requested, cb_pkt_payload is NULL, and the callback
                  owns cb_pkt_ptr.
                */

                if (cb_pkt_ptr) {
                    /* Callback owns packet - remove reference */
                    client->cpu[src_cpu].bet_rx_pkts = NULL;
                } else {
                    /* Reassembly - release accumulator */
                    bet_rx_pkt_drop(client, src_cpu, FALSE);
                }
            } else { /* Callback does not own the buffer(s) */

                bet_rx_pkt_drop(client, src_cpu, FALSE);

                if (cb_pkt_payload) {
                    /* Free reassembly buffer */
                    _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                                 cb_pkt_payload);
                }
            }

            /*
              In all final states of a multi-segment BET packet, the
              buffer passed to this function is either owned by the
              callback, or has been dropped by bet_rx_pkt_drop(), so
              claim ownership of the buffer.
            */
            rv = BCM_RX_HANDLED_OWNED;
        }

        /* For single segment BET, the ownership of the buffer is
           completely determined by the return value of the
           callback. */
    }

    return rv;
}


/*
 * Enqueue the packet data for later handling.  Currently, we
 * assume a single packet buffer and that the first 4 bytes
 * can be used as a "next" pointer for the linked list.
 */

STATIC int
enqueue_atp_data(uint8 *pkt_buf, int len, uint32 flags)
{
    _atp_pkt_data_t *atp_pkt;
    uint8 opcode;

    /* Check opcode; and split into TX related data and RX related data */
    opcode = _atp_opcode_get(pkt_buf);
    switch (opcode) {
    case _ATP_OPC_DATA:             /* ATP data */
        ATP_PKT_DATA_LOCK;
        if (_atp_pkt_data_freelist == NULL) { /* No free space in rx queue */
            ATP_PKT_DATA_UNLOCK;
            INCR_COUNTER(rx_pkt_drops);
            return -1;
        }
        atp_pkt = _atp_pkt_data_freelist;
        _atp_pkt_data_freelist = _atp_pkt_data_freelist->next;
        atp_pkt->len = len;
        atp_pkt->pkt_buf = pkt_buf;
        atp_pkt->flags = flags;
        if (_atp_rcv_data_queue_tail == NULL) { /* Queue is empty */
            _atp_rcv_data_queue = atp_pkt;
        } else {
            _atp_rcv_data_queue_tail->next = atp_pkt;
        }
        _atp_rcv_data_queue_tail = atp_pkt;
        atp_pkt->next = NULL;
        ATP_PKT_DATA_UNLOCK;
        ATP_RX_THREAD_WAKE;
        break;

    case _ATP_OPC_ACK:              /* ACK for packet we sent */
        ATP_PKT_DATA_LOCK;
        if (_atp_pkt_data_freelist == NULL) { /* No free space in rx queue */
            ATP_PKT_DATA_UNLOCK;
            INCR_COUNTER(ack_pkt_drops);
            return -1;
        }
        atp_pkt = _atp_pkt_data_freelist;
        _atp_pkt_data_freelist = _atp_pkt_data_freelist->next;
        atp_pkt->len = len;
        atp_pkt->pkt_buf = pkt_buf;
        atp_pkt->flags = flags;
        if (_atp_trx_data_queue_tail == NULL) { /* Queue is empty */
            _atp_trx_data_queue = atp_pkt;
        } else {
            _atp_trx_data_queue_tail->next = atp_pkt;
        }
        _atp_trx_data_queue_tail = atp_pkt;
        atp_pkt->next = NULL;
        ATP_PKT_DATA_UNLOCK;
        ATP_TX_THREAD_WAKE;
        break;

    default:                       /* Uh oh. */
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP: Bad packet opcode: %d\n"),
                   opcode));
        return -1;
    }
    

    return 0;
}

/* This routine is registered with BCM RX to receive C2C packets */

STATIC bcm_rx_t
_atp_rx_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    uint8 *pkt_buf;
    uint16 mplx_num;
    cpudb_key_t src_key;
    int len;

    COMPILER_REFERENCE(cookie);

    if (!_atp_running) {
        INCR_COUNTER(atp_not_running);
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META_U(unit,
                              "ATP pkt in, not running\n")));
        return BCM_RX_NOT_HANDLED;
    }

    pkt_buf = pkt->pkt_data[0].data;
    if (c2c_pkt_recognize(pkt_buf, &src_key, &mplx_num) != BCM_E_NONE) {
        return BCM_RX_NOT_HANDLED;
    }

    /* Local ID field matches ATP? */
    if (mplx_num != ATP_PKT_TYPE) {
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META_U(unit,
                              "ATP pkt in, bad pkt type %d\n"),
                   mplx_num));
        return BCM_RX_NOT_HANDLED;
    }

    len = pkt->pkt_len;

    /* Strip CRC if not already done by receiver */
    if (!(pkt->flags & BCM_RX_CRC_STRIP)) {
        len -= 4;
    }
    if (enqueue_atp_data(pkt_buf, len, pkt->flags) < 0) {
        return BCM_RX_HANDLED;
    }


    return BCM_RX_HANDLED_OWNED;
}

/* This routine is registered with Next Hop to receive (next hop)
   packets. Next Hop packets should not have a CRC at the end, so no
   need to handle here.
*/

STATIC bcm_rx_t
_atp_next_hop_callback(cpudb_key_t src_key, int mplx_num, int unit, int port,
                       uint8* pkt_buf, int len, void *cookie)
{
    COMPILER_REFERENCE(cookie);
    COMPILER_REFERENCE(mplx_num);

    if (!_atp_running) {
        INCR_COUNTER(atp_not_running);
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META_U(unit,
                              "BETNH pkt in, not running\n")));
        return BCM_RX_NOT_HANDLED;
    }

    if (enqueue_atp_data(pkt_buf, len, 0) < 0) {
        return BCM_RX_HANDLED;
    }

    return BCM_RX_HANDLED_OWNED;
}

/****************************************************************
 *
 * TX handling
 */

/*
 * Create a TX transaction.  Sets up all the bcm_pkt_t structs necessary
 * as a linked list.  Assumes TX lock held.
 *
 * CPUTRANS_COS_OVERRIDE and CPUTRANS_INT_PRIO_OVERRIDE in ct_flags
 * override default cos and internal priority values.
 */

STATIC _atp_tx_trans_t *
_atp_tx_trans_create(int dest_cpu,
                     _atp_client_t *cli,
                     int no_ack,
                     uint8 *pkt_buf,
                     int len,
                     uint32 ct_flags,
                     atp_tx_cb_f cb,
                     void *cookie)
{
    _atp_tx_trans_t *trans;
    bcm_pkt_t *pkt, *cur_pkt;
    int tot_segs;
    int tot_bytes;
    _atp_hdr_t *_atp_hdr;
    int i;
    int cos;
    int next_hop;
    int immediate_ack;
    int rv;

    tot_segs = 0;
    tot_bytes = len;
    if (ct_flags & CPUTRANS_NO_HEADER_ALLOC) {
        tot_bytes -= CPUTRANS_HEADER_BYTES;
    }

    if (tot_bytes > ATP_MTU) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP TX:  Packet too big (%d bytes)\n"),
                   tot_bytes));
        return NULL;
    }

    trans = tx_trans_freelist;
    if (trans == NULL) {  /* Allocation failed */
        INCR_COUNTER(tx_trans_fail);
        return NULL;
    }
    tx_trans_freelist = trans->next;
    INCR_COUNTER(txt_create);

    /* Is client or TX operation next hop? (or broadcast?) */
    next_hop = CLI_IS_NEXT_HOP(cli) || (ct_flags & CPUTRANS_NEXT_HOP) ||
                (ct_flags & CPUTRANS_BCAST);

    /* Is ack to be sent immediately? */
    immediate_ack = ct_flags & CPUTRANS_IMMEDIATE_ACK;
    
    /* Default COS comes from client; flags may override */
    cos = cli->cos;
    if (ct_flags & CPUTRANS_COS_OVERRIDE) {
        CPUTRANS_COS_SET(cos, CPUTRANS_COS_GET(ct_flags));
    } else {
        /* To ensure proper TX pkt allocation */
        CPUTRANS_COS_SET(ct_flags, cos);    /* Only cos is needed */
    }

    /* If internal priority is provided in flag, override default value */
    if (ct_flags & CPUTRANS_INT_PRIO_OVERRIDE) {
        CPUTRANS_INT_PRIO_SET(cos, CPUTRANS_INT_PRIO_GET(ct_flags));
    }
    
    sal_memset((void *)trans, 0, sizeof(_atp_tx_trans_t));
    trans->seg_len = _atp_seg_len;
    trans->next = NULL;

    trans->db_update = atp_db_update_count;
    /* Set up the transaction; note CPU_KEY is bcast if dest invalid */
    if (next_hop) {
        pkt = trans->pkt_list =
            next_hop_pkt_create(pkt_buf,
                                len,
                                cos,
                                cli->vlan,
                                trans->seg_len,
                                ct_flags,
                                ATP_PKT_TYPE,
                                CPU_KEY(dest_cpu),
                                &tot_segs,
                                &rv);
    } else { /* c2c packet */
        pkt = trans->pkt_list =
            c2c_pkt_create(CPU_KEY(dest_cpu),
                           pkt_buf,
                           len,
                           cos,
                           cli->vlan,
                           trans->seg_len,
                           ATP_PKT_TYPE,
                           ct_flags,
                           &tot_segs,
                           &rv);
    }

    if (rv != BCM_E_NONE) {   /* Failed to allocate */
        atp_tx_trans_delete(trans);
        INCR_COUNTER(txt_pkt_alloc_fail);
        return NULL;
    }

    /* Setup transaction and header */
    trans->client                 = cli;
    trans->callback               = cb;
    trans->cookie                 = cookie;
    trans->pkt_buf                = pkt_buf;
    trans->len                    = len;
    trans->dest_cpu               = dest_cpu;
    trans->ct_flags               = ct_flags;

    _atp_hdr                        = (_atp_hdr_t *)&(trans->_atp_hdr);
    _atp_hdr->client_id             = cli->client_id;

    _atp_hdr->tot_bytes             = tot_bytes;
    _atp_hdr->tot_segs              = tot_segs;
    _atp_hdr->opcode                = _ATP_OPC_DATA;
    _atp_hdr->cos                   = CPUTRANS_COS_GET(cos);  /* Only cos */
    if (next_hop) {
        _atp_hdr->hdr_flags |= _ATP_HDR_NEXT_HOP;
    }
    if (no_ack) {
        _atp_hdr->hdr_flags |= _ATP_HDR_NO_ACK;
    }
    if (immediate_ack) {
        _atp_hdr->hdr_flags |= _ATP_HDR_IMMEDIATE_ACK;
    }

    /* Pack the headers for each segment */
    for (i = 0, cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        _atp_hdr->segment = i++;
        _atp_hdr_pack(cur_pkt->pkt_data[0].data, (_atp_hdr_t *)&trans->_atp_hdr);
    }
    _atp_hdr->segment = 0;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("TT create %p\n"),
               trans));
    return trans;
}


/* Loopback an ATP packet */
STATIC int
_atp_loopback(_atp_client_t *client, int local_cpu,
              uint8 *pkt_buf, int len, uint32 ct_flags)
{
    _atp_rx_trans_t *new_trans = NULL;
    uint8 *rx_buf;
    uint8 *payload = pkt_buf;
    int alloc_len = len;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP TX: Loopback packet len %d, "
                        "flgs 0x%x\n"),
               len, ct_flags));

    if (ct_flags & CPUTRANS_NO_HEADER_ALLOC) {
        payload += CPUTRANS_HEADER_BYTES;
    } else {   /* Need header space for consistency on free */
        alloc_len += CPUTRANS_HEADER_BYTES;
    }

    /*
     * Create an RX transaction and enqueue, sync'd w/ RX thread.
     * The buffer is copied so that the TX can return before the RX
     * handles the buffer.  To be consistent with buffer reassembly,
     * a NULL pointer is added to the start of the data.
     *
     * ATP header bytes are present in RX buffer for consistency
     */
    rx_buf = NULL;
    _atp_trans_ptr->tp_data_alloc(_atp_trans_ptr->tp_unit,
                                  alloc_len, 0, (void*)&rx_buf);
    if (rx_buf == NULL) {
        INCR_COUNTER(lb_buf_alloc_fail);
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP LB Could not alloc RX pkt\n")));
        return BCM_E_RESOURCE;
    }
    sal_memcpy(rx_buf + CPUTRANS_HEADER_BYTES, payload, len);

    new_trans = atp_rx_trans_create(client, local_cpu, rx_buf,
                                    alloc_len, NULL, TRUE);
    if (new_trans == NULL) {
        _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit, rx_buf);
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP LB Could not alloc RX trans %d\n"),
                  alloc_len));
        return BCM_E_RESOURCE;
    }

    /* Enqueue the transaction and wake the RX thread */
    atp_rx_trans_enqueue(new_trans, client, local_cpu);
    sal_sem_give(atp_rx_sem);

    return BCM_E_NONE;
}

/****************************************************************
 *
 * ATP API
 */


/*
 * Function:
 *      atp_config_done
 * Purpose:
 *      Indicate if ATP is set up
 * Returns:
 *      TRUE if configured and running.
 */

int
atp_running(void)
{
    return _atp_running;
}


/*
 * Function:
 *      atp_config_get
 * Purpose:
 *      Get the configurable parameters for ATP
 * Parameters:
 *      tx_thrd_pri      - (OUT) The priority to use when starting ATP thread
 *      rx_thrd_pri      - (OUT) The priority to use when starting ATP thread
 *      trans_ptr        - (OUT) Pointer to transport struct
 * Returns:
 *      BCM_E_XXX
 */

int
atp_config_get(int *tx_thrd_pri, int *rx_thrd_pri,
               bcm_trans_ptr_t **trans_ptr)
{
    if (tx_thrd_pri != NULL) {
        *tx_thrd_pri = atp_tx_thread_priority;
    }
    if (rx_thrd_pri != NULL) {
        *rx_thrd_pri = atp_rx_thread_priority;
    }
    if (trans_ptr != NULL) {
        *trans_ptr = _atp_trans_ptr;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_config_set
 * Purpose:
 *      Set the configurable parameters for ATP
 * Parameters:
 *      tx_thrd_pri      - The priority to use for ATP TX thread
 *      rx_thrd_pri      - The priority to use for ATP RX thread
 *      trans_ptr        - Pointer to transport struct
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      thread priorities are ignored if < 0; trans_ptr is ignored if NULL.
 */

int
atp_config_set(int tx_thrd_pri, int rx_thrd_pri,
               bcm_trans_ptr_t *trans_ptr)
{
    if (tx_thrd_pri >= 0) {
        atp_tx_thread_priority = tx_thrd_pri;
    }
    if (rx_thrd_pri >= 0) {
        atp_rx_thread_priority = rx_thrd_pri;
    }
    if (trans_ptr != NULL) {
        if     (trans_ptr->tp_data_alloc == NULL ||
                trans_ptr->tp_data_free == NULL) {
            return BCM_E_PARAM;
        }
        _atp_trans_ptr = trans_ptr;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_start
 * Purpose:
 *      Start the ATP thread and register with RX
 * Parameters:
 *      flags          - See atp.h; mainly, learn on src miss
 *      unit_bmp       - bitmap of units on which to register with RX
 *      rco_flags      - BCM RX callout flags.  See include/bcm/rx.h.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If desired, atp_config_set should be called before this to
 *      set transport pointer, etc.
 */

int
atp_start(uint32 flags,
          uint32 unit_bmp,
          uint32 rco_flags)
{
    int rv;

    if (_atp_running) {
        INCR_COUNTER(atp_not_running);
        return BCM_E_BUSY;
    }

    ATP_INIT_CHECK;     /* Call init if that's not done yet */

    /* Register to receive next hop packets for ATP */
    if (!next_hop_running()) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP Warning:  next hop is not running\n")));
    }
    rv = next_hop_register(_atp_next_hop_callback, NULL, ATP_PKT_TYPE);
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP Warning:  cannot register with next hop\n")));
    }

    /* Start the threads */
    atp_tx_thread_id = sal_thread_create("bcmATP-TX",
                                         SAL_THREAD_STKSZ,
                                         atp_tx_thread_priority,
                                         atp_tx_thread, NULL);
    if (atp_tx_thread_id == SAL_THREAD_ERROR) {
        return BCM_E_MEMORY;
    }
    atp_rx_thread_id = sal_thread_create("bcmATP-RX",
                                         SAL_THREAD_STKSZ,
                                         atp_rx_thread_priority,
                                         atp_rx_thread, NULL);
    if (atp_rx_thread_id == SAL_THREAD_ERROR) {
        sal_thread_destroy(atp_tx_thread_id);
        atp_tx_thread_id = SAL_THREAD_ERROR;
        return BCM_E_MEMORY;
    }

    /*
     * Register to receive packets from active units
     */
    rv = cputrans_rx_bmp_register(unit_bmp, "atp", _atp_rx_callback,
                                  ATP_RX_PRIORITY, NULL,
                                  rco_flags);
    if (rv != BCM_E_NONE) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP Warning: Could not register RX %d: %s\n"),
                  rv, bcm_errmsg(rv)));
    }

    _atp_flags = flags;
    _atp_units = unit_bmp;
    _atp_running = TRUE;

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: Started\n")));

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_stop
 * Purpose:
 *      Stop the ATP thread, unregister and clean up.
 * Parameters:
 *      
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
atp_stop(void)
{
    cputrans_rx_bmp_unregister(_atp_units, _atp_rx_callback, ATP_RX_PRIORITY);

    ATP_LOCK;
    _atp_running = FALSE;
    atp_cleanup();
    ATP_UNLOCK;

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: Stopped\n")));

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_cos_vlan_set
 * Purpose:
 *      Set the default cos, internal priority, and vlan for clients
 * Parameters:
 *      cos       - COS and internal priority value to use as default;
 *                  if < 0, do not change
 *      vlan      - VLAN value to use as default if valid
 * Returns:
 *      BCM_E_NONE
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
atp_cos_vlan_set(int cos, int vlan)
{
    if (cos >= 0) {
        _atp_cos = CPUTRANS_COS_GET(cos);
        if (cos & CPUTRANS_INT_PRIO_VALID) {
            CPUTRANS_INT_PRIO_SET(_atp_cos, CPUTRANS_INT_PRIO_GET(cos));
        }
    }

    if (vlan >= 0 && vlan < 4096) {
        _atp_vlan = vlan;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_cos_vlan_get
 * Purpose:
 *      Get the default cos, internal priority, and vlan for clients
 * Parameters:
 *      cos        - (OUT) Pointer where to store cos/internal priority value
 *      vlan       - (OUT) Pointer where to store vlan value
 * Returns:
 *      BCM_E_NONE
 * Notes:
 *      Pointers may be NULL; then ignored.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
atp_cos_vlan_get(int *cos, int *vlan)
{
    if (cos != NULL) {
        *cos = _atp_cos;
    }
    if (vlan != NULL) {
        *vlan = _atp_vlan;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_db_update
 * Purpose:
 *      Update ATP with the indicated CPU database
 * Parameters:
 *      db_ref       - The new database
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */


STATIC int
atp_db_update_locked(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;
    int idx;

    for (idx = 0; idx < CPUDB_CPU_MAX; idx++) {
        if (CPU_VALID(idx)) {
            CPUDB_KEY_SEARCH(db_ref, _atp_cpu_info[idx].key, entry);
            if (entry == NULL) {
                next_hop_key_invalidate(_atp_cpu_info[idx].key);
                _atp_cpu_remove(idx);
            }
        }
    }
    if (db_ref->num_cpus > CPUDB_CPU_MAX) {
        return BCM_E_MEMORY;
    }
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        if (_atp_key_lookup(entry->base.key) == -1) {
            if (_atp_key_add(entry->base.key) < 0) {
                return BCM_E_RESOURCE;
            }
        }
    }
    atp_db_update_count++;
    return BCM_E_NONE;
}

int
atp_db_update(cpudb_ref_t db_ref)
{
    int rv;

    /* Remove CPUs not present in DB */
    ATP_INIT_CHECK;     /* Call init if that's not done yet */
    ATP_LOCK;
    rv = atp_db_update_locked(db_ref);
    ATP_UNLOCK;

    if (rv == BCM_E_NONE) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP: Updating c2c db\n")));
        rv = c2c_cpu_update(db_ref);
    } else if (rv == BCM_E_MEMORY) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP ERROR:  Too many CPUs in DB\n")));
    } else if (rv == BCM_E_RESOURCE) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP WARN: Failed to add CPU key\n")));
    }
    return rv;
}

#define FREE_CHECK(id)            \
    do {                          \
        if ((id) != NULL) {       \
            sal_free((void *)id); \
            (id) = NULL;          \
        }                         \
    } while (0)

/* End threads if possible */

STATIC int
_atp_end_threads(int retries)
{
    int rv = BCM_E_NONE;
    int i;

    if (atp_tx_thread_id != SAL_THREAD_ERROR ||
            atp_rx_thread_id != SAL_THREAD_ERROR) {
        atp_tx_thread_exit = TRUE;
        atp_rx_thread_exit = TRUE;
        ATP_TX_THREAD_WAKE;
        ATP_RX_THREAD_WAKE;
        /* Allow thread to exit */
        for (i = 0; i < retries; i++) {
            if (atp_tx_thread_id == SAL_THREAD_ERROR &&
                atp_rx_thread_id == SAL_THREAD_ERROR) {
                break;
            }
            sal_usleep(10000);
        }
        if (atp_rx_thread_id != SAL_THREAD_ERROR ||
            atp_tx_thread_id != SAL_THREAD_ERROR) {
            if (atp_tx_thread_id != SAL_THREAD_ERROR) {
                LOG_WARN(BSL_LS_TKS_ATP,
                         (BSL_META("Warning:  ATP TX thread did not exit\n")));
            }
            if (atp_rx_thread_id != SAL_THREAD_ERROR) {
                LOG_WARN(BSL_LS_TKS_ATP,
                         (BSL_META("Warning:  ATP RX thread did not exit\n")));
            }
            return BCM_E_FAIL;
        }
    }

    return rv;
}

/*
 * Clean up the ATP subsystem.
 * Assumes lock is held.  Does not remove existing clients, but clears
 * out their pending transactions.
 */

STATIC void
atp_cleanup(void)
{
    int i;
    _atp_client_t *client, *next_cli;
    int cpu;

    if (!init_done) {
        return;
    }

    (void)_atp_end_threads(50);
    sal_thread_yield();

    /* Release any pending TX sync ops */
    FOREACH_CLIENT(client, i) {
        for (cpu = 0; cpu < CPUDB_CPU_MAX; cpu++) {
            atp_tx_trans_delete_all(client, cpu);
            atp_rx_trans_delete_all(client, cpu);
        }
    }

    /* Clear all clients */
    for (i = 0; i < _ATP_CLIENT_HASH_MAX; i++) {
        client = _atp_client_buckets[i];
        while (client != NULL) {
            next_cli = client->next;
            (void)client_delete(client, FALSE);
            client = next_cli;
        }
    }

    for (i = 0; i < CPUDB_CPU_MAX; i++) {
        _atp_cpu_remove(i);
    }

    if (ack_pkt_data != NULL) {
        atp_ack_pkt_data_from_heap ?
            sal_free(ack_pkt_data) : sal_dma_free(ack_pkt_data);
        ack_pkt_data = NULL;
    }

    init_done = FALSE;
}

#undef FREE_CHECK

/****************************************************************
 *
 * ATP configuration functions
 */


/*
 * Function:
 *      atp_timeout_set
 * Purpose:
 *      Set the timeout parameters for an ATP transmit
 * Parameters:
 *      retry_usecs    - The timeout value in microseconds
 *      num_retries    - The number of retry attempts
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The system clock usually has a minimal tick size of about 10000
 *      usecs.
 *
 *      Checks the parameters against minimum allowable
 */

int
atp_timeout_set(int retry_usecs, int num_retries)
{
    if (retry_usecs < ATP_RETRY_TIMEOUT_MIN) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP Warning: changing retry timeout "
                  "from %d to %d\n"), retry_usecs, ATP_RETRY_TIMEOUT_MIN));
        atp_retry_timeout = ATP_RETRY_TIMEOUT_MIN;
    } else {
        atp_retry_timeout = retry_usecs;
    }

    if (num_retries < 1) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP Warning: changing retry count from "
                  "%d to %d\n"), num_retries, 1));
        atp_retry_count = 1;
    } else {
        atp_retry_count = num_retries;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_timeout_get
 * Purpose:
 *      Get the timeout parameters for an ATP transmit
 * Parameters:
 *      retry_usecs    - (OUT) The timeout value in microseconds
 *      num_retries    - (OUT) The number of retry attempts
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
atp_timeout_get(int *retry_usecs, int *num_retries)
{
    *retry_usecs = atp_retry_timeout;
    *num_retries = atp_retry_count;

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_timeout_register
 * Purpose:
 *      Set the timeout callback function that will be called
 *      when an ATP transmission fails due to timeout.
 * Parameters:
 *      callback        - The callback function
 *                         (or NULL for no callback)
 * Returns:
 *      BCM_E_XXX
 */
int
atp_timeout_register(atp_timeout_cb_f callback)
{
    BASE_INIT_CHECK;

    ATP_LOCK;
    atp_timeout_cb = callback;
    ATP_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_segment_len_set
 * Purpose:
 *      Set the segmentation length
 * Parameters:
 *      seg_len        - Max length of a segment in bytes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Checks the parameter against minimum allowable
 */

#define SEG_LEN_MIN 16  /* Let's be reasonable here..... */

int
atp_segment_len_set(int seg_len)
{
    int rv = BCM_E_NONE;

    BASE_INIT_CHECK;
    ATP_LOCK;

    if (seg_len < SEG_LEN_MIN) {
        rv = BCM_E_PARAM;
    } else {
        _atp_seg_len = seg_len;
    }

    ATP_UNLOCK;
    return rv;
}


/*
 * Function:
 *      atp_segment_len_get
 * Purpose:
 *      Get the segmentation length
 * Returns:
 *      Current segment length
 */

int
atp_segment_len_get(void)
{
    return _atp_seg_len;
}


/*
 * Function:
 *      atp_pool_size_set
 * Purpose:
 *      Set the maximum allowed number of TX/RX transaction buffers
 * Parameters:
 *      tx_max      - Max number of tx transaction buffers
 *      rx_max      - Max number of rx transaction buffers
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Applies to TX and RX independently.
 *      Effects will not take affect until restart.
 *
 *      If parameter is < 0, use the default value.
 */

int
atp_pool_size_set(int tx_max, int rx_max)
{
    if (tx_max < 0) {
        atp_tx_pool_size = ATP_TX_TRANSACT_DEFAULT;
    } else {
        atp_tx_pool_size = tx_max;
    }

    if (rx_max < 0) {
        atp_rx_pool_size = ATP_RX_TRANSACT_DEFAULT;
    } else {
        atp_rx_pool_size = rx_max;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_pool_size_get
 * Purpose:
 *      Get the current max pending operations setting
 * Parameters:
 *      tx_max      - (OUT) Max number of tx transaction buffers
 *      rx_max      - (OUT) Max number of rx transaction buffers
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
atp_pool_size_get(int *tx_max, int *rx_max)
{
    *tx_max = atp_tx_pool_size;
    *rx_max = atp_rx_pool_size;

    return BCM_E_NONE;
}


/****************************************************************
 *
 * Init and clean up functions
 */

#define ALLOC_CHECK(id, bytes, rv)                                      \
    do {                                                                \
        (id) = sal_alloc((bytes), "ATP");                               \
        if ((id) == NULL) {                                             \
            atp_cleanup();                                              \
            ATP_UNLOCK;                                                 \
            return rv;                                                  \
        }                                                               \
        sal_memset((void *)(id), 0, bytes);                             \
    } while (0)

#define RX_ALLOC_CHECK(id, bytes, from_heap)                            \
    do {                                                                \
        (id) = (from_heap) ?                                            \
            sal_alloc((bytes), "ATP") : sal_dma_alloc((bytes), "ATP");  \
        if ((id) == NULL) {                                             \
            atp_cleanup();                                              \
            ATP_UNLOCK;                                                 \
            return BCM_E_MEMORY;                                        \
        }                                                               \
        sal_memset((void *)(id), 0, bytes);                             \
    } while (0)

/*
 * Function:
 *      _atp_base_init
 * Purpose:
 *      Set up mutex and semaphores
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Should be called before ATP_LOCK is taken
 */

STATIC int
_atp_base_init(void)
{
    if (atp_tx_mutex == NULL) {
        atp_tx_mutex = sal_mutex_create("atp_tx_mutex");
        if (atp_tx_mutex == NULL) {
            return BCM_E_MEMORY;
        }
    }
    if (atp_rx_mutex == NULL) {
        atp_rx_mutex = sal_mutex_create("atp_rx_mutex");
        if (atp_rx_mutex == NULL) {
            return BCM_E_MEMORY;
        }
    }

    base_init_done = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _atp_base_init_check
 * Purpose:
 *      Set up mutex and semaphores if needed
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Should be called before ATP_LOCK is taken
 */

STATIC int
_atp_base_init_check(void)
{
    BASE_INIT_CHECK;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _atp_init
 * Purpose:
 *      Initialize the ATP subsystem
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Pre-allocates TX transactions and sets up a free list.
 *      Pre-allocates RX transactions and sets up a free list; installs
 *           an ACK buffer into each RX transaction.
 */

STATIC int
_atp_init(void)
{
    int i;
    int bytes;

    BASE_INIT_CHECK;

    ATP_LOCK;

    /* Set up ATP RX transaction queue mutex */
    if (atp_rxq_mutex == NULL) {
        atp_rxq_mutex = sal_mutex_create("atp_rxq");
        if (atp_rxq_mutex == NULL) {
            ATP_UNLOCK;
            return BCM_E_MEMORY;
        }
    }

    /* Set up BCM-RX queue and mutex */
    if (atp_pkt_data_mutex == NULL) {
        atp_pkt_data_mutex = sal_mutex_create("atp_pkt_data");
        if (atp_pkt_data_mutex == NULL) {
            ATP_UNLOCK;
            return BCM_E_MEMORY;
        }
    }

    for (i = 0; i < ATP_PKT_DATA_QUEUE_LEN - 1; i++) {
        _atp_rx_pkt[i].next = &_atp_rx_pkt[i + 1];
    }
    _atp_rx_pkt[ATP_PKT_DATA_QUEUE_LEN - 1].next = NULL;
    _atp_rcv_data_queue = NULL;
    _atp_rcv_data_queue_tail = NULL;
    _atp_pkt_data_freelist = &_atp_rx_pkt[0];

    if (atp_tx_sem == NULL) {
        atp_tx_sem = sal_sem_create("atp_tx_sem", sal_sem_BINARY, 0);
        if (atp_tx_sem == NULL) {
	    ATP_UNLOCK;
            return BCM_E_MEMORY;
        }
    }

    if (atp_rx_sem == NULL) {
        atp_rx_sem = sal_sem_create("atp_rx_sem", sal_sem_BINARY, 0);
        if (atp_rx_sem == NULL) {
	    ATP_UNLOCK;
            return BCM_E_MEMORY;
        }
    }

    /* Allocate the tx transactions and set up the free list */
    if (tx_trans_pool != NULL) {
        sal_free((void *)tx_trans_pool);
    }
    bytes = sizeof(_atp_tx_trans_t) * atp_tx_pool_size;
    ALLOC_CHECK(tx_trans_pool, bytes, BCM_E_MEMORY);
    tx_trans_freelist = tx_trans_pool;

    for (i = 0; i < atp_tx_pool_size; i++) {
        tx_trans_freelist[i].next = &tx_trans_freelist[i + 1];
    }
    tx_trans_freelist[atp_tx_pool_size - 1].next = NULL;

    /* Allocate and setup RX transactions and ACKs for each */
    if (rx_trans_pool != NULL) {
        sal_free((void *)rx_trans_pool);
    }
    bytes = atp_rx_pool_size * sizeof(_atp_rx_trans_t);
    ALLOC_CHECK(rx_trans_pool, bytes, BCM_E_MEMORY);

    bytes = _ATP_ACK_BYTES * atp_rx_pool_size;
    RX_ALLOC_CHECK(ack_pkt_data, bytes, atp_ack_pkt_data_from_heap);

    rx_trans_freelist = rx_trans_pool;
    for (i = 0; i < atp_rx_pool_size; i++) {
        rx_trans_freelist[i].next = &rx_trans_freelist[i + 1];
        rx_trans_freelist[i].ack_data = &ack_pkt_data[_ATP_ACK_BYTES * i];
    }
    rx_trans_freelist[atp_rx_pool_size - 1].next = NULL;

    init_done = TRUE;
    ATP_UNLOCK;

    return BCM_E_NONE;
}

/****************************************************************
 *
 * ATP Callback Registration/De-registration
 */


/*
 * Function:
 *      atp_client_add
 * Purpose:
 *      Register a client ID for sending packets only
 * Parameters:
 *      client_id   - The client ID to register for
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Just calls atp_register with trivial arguments.
 */

int
atp_client_add(int client_id)
{
    return atp_register(client_id, 0, NULL, NULL, -1, -1);
}


/*
 * Function:
 *      atp_register
 * Purpose:
 *      Register a callback for an ATP client
 * Parameters:
 *      client_id   - The client ID to register for
 *      flags       - See atp.h.  Indicates NH/C2C/ATP.
 *      callback    - Callback routine
 *      cookie      - Passed to callback
 *      cos         - The COS and internal priority
 *                    to use sending pkts for this client
 *                    If < 0, use default
 *                    (Unless overridden by flags in transmit)
 *      vlan        - The VLAN to use sending pkts for this client if valid
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      A client can register to send packets only (no RX) by registering
 *      with a callback of NULL.
 *      Will overwrite existing entry as long as flags agree.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
atp_register(int client_id,
             uint32 flags,
             atp_client_cb_f callback,
             void *cookie,
             int cos,
             int vlan)
{
    _atp_client_t *client;

    BASE_INIT_CHECK;     /* Call init if that's not done yet */

    /* Create client if doesn't exist;
     * Fail if client exists with different flags */
    ATP_LOCK;
    client = client_find(client_id);
    if (client == NULL) {
        client = client_id_add(client_id);
        if (client == NULL) {
            ATP_UNLOCK;
            return BCM_E_MEMORY;
        }
    } else {
        if (client->flags != flags) {
            ATP_UNLOCK;
            return BCM_E_EXISTS;
        }
    }

    if (cos >= 0) {
        client->cos = CPUTRANS_COS_GET(cos);
        if (cos & CPUTRANS_INT_PRIO_VALID) {
            CPUTRANS_INT_PRIO_SET(client->cos, CPUTRANS_INT_PRIO_GET(cos));
        }
    }

    if (vlan >= 0 && vlan < 4096) {
        client->vlan = vlan;
    }

    client->flags = flags;
    client->callback = callback;
    client->cookie = cookie;

    ATP_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_unregister
 * Purpose:
 *      Unregister a callback for an ATP client registered with atp_register
 * Parameters:
 *      client_id   - Client ID to unregister
 * Returns:
 *      BCM_E_XXX
 */

void
atp_unregister(int client_id)
{
    _atp_client_t *cli;

    if (!init_done) {
        return;
    }

    ATP_LOCK;
    cli = client_find(client_id);
    if (cli != NULL) {
        client_delete(cli, TRUE);
    }
    ATP_UNLOCK;
}


/****************************************************************
 *
 * ATP Transmit Support
 */

/*
 * If synchronous ATP operation, wait on semaphor.
 *
 * The return code is stored via a pointer put into the
 * transaction.  This routine is responsible for deleting
 * the transaction.
 *
 * A timeout is detected by retransmit attempts.
 */

STATIC int
_atp_sync_check(_atp_tx_trans_t *trans, atp_tx_cb_f callback, int dest_cpu)
{
    int rv = BCM_E_NONE;

    if (TX_TRANS_ACK(trans) && (callback == NULL)) {
        ++tx_sleep_count;
        if (sal_sem_take(trans->tx_sem, sal_sem_FOREVER) < 0) {
            rv = BCM_E_INTERNAL;
        } else {
            rv = trans->tx_rv;
        }
        ATP_TX_LOCK;
        atp_tx_trans_delete(trans);
        ATP_TX_UNLOCK;
        --tx_sleep_count;
    }

    return rv;
}

/****************************************************************
 *
 * Send out a TX transaction.
 *     For BET, just send out everything and return.
 *     For ATP, check next expected byte from RX and start with
 *     that segment.
 */

STATIC void
_atp_tx_trans_send(_atp_tx_trans_t *trans)
{
    bcm_pkt_t *first_pkt;  /* In list to send out */
    int rv = BCM_E_NONE;
    c2c_cb_f c2c_cb = NULL;
    next_hop_tx_callback_f nh_cb = NULL;
    int i;
    int next_hop;
    int no_ack;

    next_hop = TX_TRANS_NEXT_HOP(trans);
    no_ack = TX_TRANS_NO_ACK(trans);

    first_pkt = trans->pkt_list;
    assert(first_pkt->next != first_pkt);
    if (!no_ack) {  /* Use ACK */
        LOG_DEBUG(BSL_LS_TKS_ATP,
                  (BSL_META("TX ATP send cli %d, seq %d txcount %d lasttx %u\n"),
                   trans->client->client_id, 
                   trans->_atp_hdr.seq_num,
                   trans->tx_count, trans->last_tx));
        /* ATP packet; check bytes acked; always send sync */
        for (i = 1; trans->bytes_acked >= (i * trans->seg_len); i++) {
            first_pkt = first_pkt->next;
        }
        ATP_ASSERT((first_pkt != NULL));
        c2c_cb = _atp_c2c_tx_callback;
        nh_cb = _atp_nh_tx_callback;
    }
    trans->flags |= _ATP_TX_F_PENDING;

    if (next_hop) {
        if (trans->tx_count > 0) { /* Update with new seq num, etc */
            next_hop_pkt_update(first_pkt, ATP_PKT_TYPE,
                                CPU_KEY(trans->dest_cpu));
        }
        rv = next_hop_pkt_send(first_pkt, NULL, NULL); /* Always sync */
        if (nh_cb != NULL) {
            nh_cb(rv, NULL, (void *)trans);
        }
    } else {                                         /* Send C2C */
        int update; /* Protect from update changing in midstream */
        update = atp_db_update_count;
        if (update - trans->db_update > 0) {
            /* DB has been updated; freshen packet */
            if (c2c_pkt_update(first_pkt, CPU_KEY(trans->dest_cpu)) >= 0) {
                /* Success, indicate db updated for transaction */
                trans->db_update = update;
            }
        }

        rv = c2c_pkt_send(first_pkt, NULL, NULL); /* Always sync */
        if (c2c_cb != NULL) {
            c2c_cb(NULL, (void *)trans);
        }
    }

    if (rv != BCM_E_NONE) {
        trans->flags |= _ATP_TX_ERROR_SEEN;
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP TX error %d: %s\n"),
                   rv, bcm_errmsg(rv)));
    } else {
        if (!no_ack) {
            if (trans->tx_count++ > 0) {
                INCR_COUNTER(tx_retry_cnt);
            }
            trans->last_tx = sal_time_usecs();
        }
    }
}

/* Allocate and store a sequence number for a transaction */

STATIC void
_atp_seq_num_set(int no_ack, _atp_tx_trans_t *trans)
{
    bcm_pkt_t *cur_pkt;
    uint16 seq_num;

    TX_SEQ_NUM_GET(no_ack, seq_num, trans->client, trans->dest_cpu);
    trans->_atp_hdr.seq_num = seq_num;
    /* Pack the sequence number for each segment */
    for (cur_pkt = trans->pkt_list; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        ATP_SEQ_NUM_SET(cur_pkt->pkt_data[0].data, seq_num);
    }
}

/*
 * Transaction is an ATP request.  If sync, create a semaphor
 * to wait on; enqueue the packet.
 */


/* Set up ATP or BET transaction according to client flags */
STATIC int
_atp_tx_trans_setup(_atp_tx_trans_t *trans, int *trans_deleted)
{
    atp_tx_cb_f callback;

    callback = trans->callback;
    trans->next = NULL;

    if (TX_TRANS_NO_ACK(trans)) { /* Best effort */
        if (callback == NULL) {   /* Send out immediately */
            _atp_seq_num_set(TRUE, trans);
            _atp_tx_trans_send(trans);
            atp_tx_trans_delete(trans);
            *trans_deleted = TRUE;
        } else { /* Place in BET queue */
            _atp_seq_num_set(TRUE, trans);
            trans->prev = bet_queue_tail;
            if (bet_queue_tail != NULL) {
                bet_queue_tail->next = trans;
            } else {
                bet_queue = trans;
            }
            bet_queue_tail = trans;
            trans->flags |= _ATP_TX_F_ENQUEUED;
            *trans_deleted = FALSE;
        }
    } else {  /* ACK required */
        _atp_client_t *client;
        int dest_cpu;

        client = trans->client;
        dest_cpu = trans->dest_cpu;

        if (callback == NULL) { /* Sync send; create semaphor */
            trans->tx_sem = sal_sem_create("atp_tx", sal_sem_BINARY, 0);
            if (trans->tx_sem == NULL) {
                atp_tx_trans_delete(trans);
                *trans_deleted = TRUE;
                LOG_ERROR(BSL_LS_TKS_ATP,
                          (BSL_META("ATP TX:  Failed to create sem\n")));
                return BCM_E_MEMORY;
            }
            trans->flags |= _ATP_TX_F_SEM_WAITING;
        }

        _atp_seq_num_set(FALSE, trans);
        /* Enqueue transaction */
        trans->prev = client->cpu[dest_cpu].tx_tail;
        if (client->cpu[dest_cpu].tx_tail != NULL) {
            client->cpu[dest_cpu].tx_tail->next = trans;
        } else {            /* Queue was empty; add to front */
            client->cpu[dest_cpu].tx_trans = trans;
        }
        client->cpu[dest_cpu].tx_tail = trans;
        trans->flags |= _ATP_TX_F_ENQUEUED;
        ++atp_tx_pending;
        *trans_deleted = FALSE;
    }

    return BCM_E_NONE;
}

/*
 * Send a packet without ACK.  No checking is done here.  It is assumed
 * that the CPUTRANS header is allocated at the beginning of the packet,
 * and that the packet fits in a single segment.
 *
 * Note:  Transactions are used here in a bad way.  They are used to
 * convey the callback and cookie to the async transport call.
 * In those, they are freed directly back to the transaction free list
 * rather than being marked deleted.
 *
 * CPUTRANS_COS_OVERRIDE and CPUTRANS_INT_PRIO_OVERRIDE in ct_flags
 * override default cos and internal priority values.
 */

STATIC int
_atp_simple_send(cpudb_key_t dest_key,
                 _atp_client_t *client,
                 uint8 *pkt_buf,
                 int len,
                 uint32 ct_flags,
                 atp_tx_cb_f callback,
                 void *cookie)
{
    _atp_tx_trans_t *trans = NULL;
    _atp_hdr_t _atp_hdr;
    int rv = BCM_E_NONE;
    int next_hop;
    int cos;

    next_hop = (client->flags & ATP_F_NEXT_HOP) ||
        (ct_flags & CPUTRANS_NEXT_HOP);

    /* Default COS comes from client; flags may override */
    cos = client->cos;
    if (ct_flags & CPUTRANS_COS_OVERRIDE) {
        CPUTRANS_COS_SET(cos, CPUTRANS_COS_GET(ct_flags));
    } else {
        /* To ensure proper TX pkt allocation */
        CPUTRANS_COS_SET(ct_flags, cos);    /* Only cos is needed */
    }

    /* If internal priority is provided in flag, override default value */
    if (ct_flags & CPUTRANS_INT_PRIO_OVERRIDE) {
        CPUTRANS_INT_PRIO_SET(cos, CPUTRANS_INT_PRIO_GET(ct_flags));
    }

    _atp_hdr.client_id             = client->client_id;
    TX_BET_SEQ_NUM_GET(_atp_hdr.seq_num, client);
    _atp_hdr.tot_bytes             = len - CPUTRANS_HEADER_BYTES;
    _atp_hdr.start_byte            = 0;
    _atp_hdr.tot_segs              = 1;
    _atp_hdr.opcode                = _ATP_OPC_DATA;
    _atp_hdr.segment               = 0;
    _atp_hdr.hdr_flags             = _ATP_HDR_NO_ACK;
    _atp_hdr.cos                   = CPUTRANS_COS_GET(cos);  /* Only cos */
    if (next_hop) {
        _atp_hdr.hdr_flags |= _ATP_HDR_NEXT_HOP;
    }
    _atp_hdr_pack(pkt_buf, &_atp_hdr);

    if (callback != NULL) {  /* Async */
        /* For aync, use a TX trans to pass back cookie and callback */
        trans = tx_trans_freelist;
        if (trans == NULL) {  /* Allocation failed */
            return BCM_E_RESOURCE;
        }
        tx_trans_freelist = trans->next;
        INCR_COUNTER(txraw_grab);

        sal_memset((void *)trans, 0, sizeof(_atp_tx_trans_t));
        trans->callback = callback;
        trans->cookie = cookie;
    }

    LOG_VERBOSE(BSL_LS_TKS_TX,
                (BSL_META("ATP simple %d:  NH %d. ctf %x. cb %p\n"),
                 client->client_id, next_hop, ct_flags, callback));
    if (next_hop) {  /* Next hop packet */
        rv = next_hop_tx(pkt_buf,
                         len,
                         cos,
                         client->vlan,
                         _atp_seg_len,
                         ct_flags,
                         ATP_PKT_TYPE,
                         dest_key,
                         callback == NULL ? NULL : bet_nh_free_tx_cb,
                         (void *)trans);
    } else { /* c2c directed packet */
        rv = c2c_tx(dest_key, pkt_buf, len,
                    cos, client->vlan, _atp_seg_len,
                    ATP_PKT_TYPE, ct_flags,
                    callback == NULL ? NULL : bet_c2c_free_tx_cb,
                    (void *)trans);
    }

    return rv;
}

/****************************************************************
 *
 * Simple BET support and TX Callbacks
 */

/* This is registered with c2c transmit routine when sending async */
STATIC void
_atp_c2c_tx_callback(uint8 *pkt_buf, void *cookie)
{
    _atp_tx_trans_t *trans;

    COMPILER_REFERENCE(pkt_buf);

    trans = (_atp_tx_trans_t *)cookie;

    
    trans->flags &= ~_ATP_TX_F_PENDING;
}

/* This is registered with nexthop transmit routine when sending async */
STATIC void
_atp_nh_tx_callback(int rv, uint8 *pkt_buf, void *cookie)
{
    _atp_tx_trans_t *trans;

    COMPILER_REFERENCE(pkt_buf);

    trans = (_atp_tx_trans_t *)cookie;

    
    trans->tx_rv = rv;
    trans->flags &= ~_ATP_TX_F_PENDING;
}

/****************************************************************
 * BET single segment fast track support; only called by _atp_simple_send
 */

/*
 * Async callback which releases transaction; these are
 * for best-effort only, so no semaphor in TX transaction.
 */

STATIC void
_tx_free_callback(_atp_tx_trans_t *trans, uint8 *pkt_buf, int rv)
{
    atp_tx_cb_f cb;

    cb = trans->callback;

    if (cb != NULL) {
        cb(pkt_buf, trans->cookie, rv);
    }

    ATP_TX_LOCK;
    atp_tx_trans_delete(trans);
    ATP_TX_UNLOCK;
}

/* Next hop async callback for simple BET */
STATIC void
bet_nh_free_tx_cb(int rv, uint8 *pkt_buf, void *cookie)
{
    _tx_free_callback((_atp_tx_trans_t *)cookie, pkt_buf, rv);
}

/* C2C async callback for simple BET */
STATIC void
bet_c2c_free_tx_cb(uint8 *pkt_buf, void *cookie)
{
    _tx_free_callback((_atp_tx_trans_t *)cookie, pkt_buf, BCM_E_NONE);
}

/****************************************************************
 *
 * ATP TX Thread
 */

/* Assumes lock held */

STATIC void
tx_done_handle(_atp_client_t *client, int cpu, _atp_tx_trans_t *trans)
{

    if (trans->flags & _ATP_TX_F_TIMEOUT) {
        LOG_ERROR(BSL_LS_TKS_ATP,
                  (BSL_META("ATP: TX timeout, seq %d. " CPUDB_KEY_FMT 
                   " cli %d. to %d tx cnt %d.\n"),
                   trans->_atp_hdr.seq_num,
                   CPUDB_KEY_DISP(_atp_cpu_info[trans->dest_cpu].key),
                   trans->client->client_id,
                   trans->dest_cpu,
                   trans->tx_count));
        if (atp_timeout_cb != NULL) {
            (*atp_timeout_cb)(CPU_KEY(trans->dest_cpu));
        }
    }

    if (trans->callback != NULL) {
        trans->callback(trans->pkt_buf, trans->cookie, trans->tx_rv);
    }
    atp_tx_trans_delete(trans);
}

STATIC void
_set_retrx_flag(_atp_tx_trans_t *trans)
{
    uint8 *ptr;

    if (!(trans->_atp_hdr.hdr_flags & _ATP_HDR_RETRANSMIT)) {
        trans->_atp_hdr.hdr_flags |= _ATP_HDR_RETRANSMIT;

        ptr = ATP_HEADER_START(trans->pkt_list->pkt_data[0].data);
        ptr += (sizeof(uint16) + sizeof(uint16)); /* version + cli ID */
        PACK_LONG(ptr, trans->_atp_hdr.hdr_flags);
    }
}

/* Return boolean, TRUE means transaction is active */
STATIC int
tx_trans_retransmit_check(_atp_tx_trans_t *trans)
{
    if (trans->flags & _ATP_TX_F_DONE) {
        return FALSE;   /* Not active */
    }

    if (trans->last_tx == 0) {   /* Never sent before */
        _atp_tx_trans_send(trans);
    } else {
        int dt = SAL_USECS_SUB(sal_time_usecs(), trans->last_tx);

        if (dt < 0 || dt > atp_retry_timeout) {
            /* Set retransmit flag in ATP header */
            _set_retrx_flag(trans);
        
            /* Time to retransmit this data; check for too many retries */
            if (trans->tx_count > atp_retry_count) {
                trans->tx_rv = BCM_E_TIMEOUT;
                trans->flags |= _ATP_TX_F_DONE | _ATP_TX_F_TIMEOUT;
                INCR_COUNTER(tx_timeout_cnt);
                return FALSE;   /* No long active. */
            } else {
                if (!(trans->flags & (_ATP_TX_F_PENDING | _ATP_TX_F_DONE))) {
                    _atp_tx_trans_send(trans);
                }
            }
        }
    }

    /* This is the active transmit transaction */
    return TRUE;
}

/*
 * Go through all clients and check the state of any pending transmit
 * transactions.  At most one is active for any given client/dest CPU.
 * It may require retransmitting.
 *
 * Only send out data for the first active (non-ack'd) transaction.
 */

STATIC void
tx_transactions_check(void)
{
    int cpu;
    int idx;
    _atp_client_t *client;
    _atp_tx_trans_t *trans;
    _atp_tx_trans_t *trans_next;
    int active_found = FALSE;
    
    ATP_TX_LOCK;
    FOREACH_CLIENT(client, idx) {
        for (cpu = 0; cpu < atp_cpu_max; cpu++) {
            active_found = FALSE;
            trans = client->cpu[cpu].tx_trans;
            while (trans != NULL) {
                /* Need to store next transaction now, in case current
                   transaction is deleted from list */
                trans_next = trans->next;

                if (!active_found) {
                    /* At most one transaction per client/dest CPU may
                       be active at a time */
                    active_found = tx_trans_retransmit_check(trans);
                }
                if (trans->flags & _ATP_TX_F_DONE &&
                        !(trans->flags & _ATP_TX_F_PENDING)) {
                    LOG_DEBUG(BSL_LS_TKS_ATP,
                              (BSL_META("TX done cli %d, to %d, seq %d\n"),
                               client->client_id, cpu,
                               trans->_atp_hdr.seq_num));
                    tx_done_handle(client, cpu, trans);
                }
                
                trans = trans_next;
            }
        }
    }
    ATP_TX_UNLOCK;
}

/* This is max BET queue entries processed before returning */

#ifndef MAX_BET_COUNT
#define MAX_BET_COUNT 4
#endif


STATIC void
bet_tx_queue_handle(void)
{
    _atp_tx_trans_t *entry;
    int bet_count = 0;

    if (atp_tx_thread_exit) {  /* Exit forced */
        return;
    }

    ATP_TX_LOCK;
    while (bet_queue != NULL) {
        if (bet_count++ >= MAX_BET_COUNT) {
            break;
        }
        entry = bet_queue;
        bet_queue = bet_queue->next;
        if (bet_queue == NULL) {
            bet_queue_tail = NULL;
        }

        _atp_tx_trans_send(entry);
        if (entry->callback != NULL) {
            entry->callback(entry->pkt_buf,
                            entry->cookie, BCM_E_NONE);
        }
        atp_tx_trans_delete(entry);
    }
    ATP_TX_UNLOCK;
}

STATIC int
_handle_tx_data(_atp_pkt_data_t *pkt_p)
{
    cpudb_key_t src_key;
    int src_cpu;
    uint8 *pkt_buf;
    _atp_hdr_t _atp_hdr;

    pkt_buf = pkt_p->pkt_buf;
    _atp_hdr_unpack(pkt_buf, &_atp_hdr);

    /* Look for the source CPU */
    CPUDB_KEY_UNPACK(&pkt_buf[CPUTRANS_SRC_KEY_OFS], src_key);
    src_cpu = _atp_key_lookup(src_key);
    if (src_cpu < 0) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP ACK pkt: could not find source CPU key\n")));
        return BCM_RX_HANDLED;
    }

    return atp_ack_handle(src_cpu, &_atp_hdr);
}

/* Process transmit related packet queue */

STATIC void
trx_process_pkt_data(void)
{
    _atp_pkt_data_t *cur_p;
    _atp_pkt_data_t *next_p;
    int rv;

    ATP_TX_LOCK;
    ATP_PKT_DATA_LOCK; /* Steal the current queue of pkts */
    cur_p = _atp_trx_data_queue;
    _atp_trx_data_queue_tail = NULL;
    _atp_trx_data_queue = NULL;
    ATP_PKT_DATA_UNLOCK;

    while (cur_p != NULL) {
        next_p = cur_p->next;

        rv = _handle_tx_data(cur_p);
        if (rv != BCM_RX_HANDLED_OWNED) {   /* Free packet data */
            _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit,
                                         cur_p->pkt_buf);
        } 
        ATP_PKT_DATA_LOCK; /* Free the pkt struct */
        cur_p->next = _atp_pkt_data_freelist;
        _atp_pkt_data_freelist = cur_p;
        ATP_PKT_DATA_UNLOCK;
        
        cur_p = next_p;
    }
    ATP_TX_UNLOCK;
}

STATIC void
atp_tx_thread(void *cookie)
{
    int cur_timeout;

    COMPILER_REFERENCE(cookie);

    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: TX Thread starting\n")));
    atp_tx_thread_exit = FALSE;
    while (1) {
        cur_timeout = atp_tx_pending ? atp_retry_timeout : ATP_LONG_TIMEOUT;
        if (bet_queue == NULL) { /* Don't sleep if best effort transactions */
            sal_sem_take(atp_tx_sem, cur_timeout);
        }

        if (atp_tx_thread_exit) {  /* Exit forced */
            break;
        }

        if (_atp_trx_data_queue != NULL) {
            trx_process_pkt_data();
        }

        tx_transactions_check();
        bet_tx_queue_handle();
    }

    /* Give any pending TX transactions */
    atp_tx_thread_id = SAL_THREAD_ERROR;
    LOG_VERBOSE(BSL_LS_TKS_ATP,
                (BSL_META("ATP: TX Thread exiting\n")));
    sal_thread_exit(0);
}

/*
 * Macro to check if this is a single segment, no-ACK packet with
 * the transport header already allocated.
 */

#define _ATP_SIMPLE_CHECK(_no_ack, _ct_flags, _len) \
    ((_no_ack) && ((_ct_flags) & CPUTRANS_NO_HEADER_ALLOC) && \
        ((_len) - CPUTRANS_HEADER_BYTES <= _atp_seg_len))

/*
 * Function:
 *      atp_tx
 * Purpose:
 *      Send out a packet to given CPU using ATP; override may occur per CPU
 * Parameters:
 *      dest_key     - Dest CPU to send to
 *      client_id    - Client ID to send to
 *      pkt_buf      - Pointer to data to transmit
 *      len          - Length of packet from pkt_buf to end, excluding CRC
 *                     unless CPUTRANS_CRC_REGEN is set
 *      ct_flags     - Bitmap of flags passed in
 *          CPUTRANS_NO_HEADER_ALLOC   Packet contains space for headers
 *          CPUTRANS_BROADCAST         Send to all CPUs;
 *                 Currently only supported on NO ACK, next hop.
 *          CPUTRANS_COS_OVERRIDE      Override default cos
 *          CPUTRANS_INT_PRIO_OVERRIDE Overrude default internal priority
 *          CPUTRANS_CRC_REGEN         Caller must allocate space for Ethernet
 *                                     transport CRC
 *      callback     - If not NULL, do async and callback
 *      cookie       - Passed on callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The client must have been registered.
 *
 *      Depending on the flags (CPUTRANS_NO_HEADER_ALLOC), the
 *      pkt_buf pointer may either point to the beginning of the
 *      payload or the beginning of the actual packet.
 *
 *      When callback is specified, it will take place after
 *      the packet has been acknowledged.
 *
 *      The application is responsible for freeing the data sent.  If
 *      the data are sent asynchronously then the application must not
 *      deallocate the data until the callback is made indicating the
 *      transmit is complete.
 *
 *      atp_tx creates a BCM packet via cputrans_tx_pkt_list_alloc
 *      and frees that structure on completion of the transaction.
 */

int
atp_tx(cpudb_key_t dest_key,
       int client_id,
       uint8 *pkt_buf,
       int len,
       uint32 ct_flags,
       atp_tx_cb_f callback,
       void *cookie)
{
    _atp_tx_trans_t *trans;
    int rv = BCM_E_NONE;
    _atp_client_t *client;
    int dest_cpu;
    int no_ack = FALSE;   /* If true, packet is best effort */
    atp_tx_f callout;
    int trans_deleted;

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("ATP tx cli %d%s\n"),
               client_id,
               _atp_running ? "" : " (not running)"));

    ATP_INIT_CHECK;

    if (ct_flags & CPUTRANS_CRC_REGEN) {
        ct_flags &= ~CPUTRANS_CRC_REGEN;
        len -= sizeof(uint32); /* accept flag, but ignore */
    }

    /* Check for override of TX function (before "running" check) */
    ATP_TX_LOCK;
    dest_cpu = _atp_key_lookup(dest_key);
    if (dest_cpu >= 0) {  /* Found dest CPU */
        if (_atp_cpu_info[dest_cpu].override_tx != NULL) { /* Override TX */
            callout = _atp_cpu_info[dest_cpu].override_tx;
            ATP_TX_UNLOCK;
            LOG_DEBUG(BSL_LS_TKS_ATP,
                      (BSL_META("ATP tx calling override %d\n"),
                       dest_cpu));
            return callout(dest_key, client_id, pkt_buf, len, ct_flags,
                           callback, cookie);
        }
        if (_atp_cpu_info[dest_cpu].flags & _ATP_CPU_NO_ACK) {
            no_ack = TRUE;  /* CPU forces NO-ACK */
        }
    }

    if (!_atp_running) {
        ATP_TX_UNLOCK;
        INCR_COUNTER(atp_not_running);
        return BCM_E_INIT;
    }

    client = client_find(client_id);
    if (client == NULL) {  /* Client must exist */
        ATP_TX_UNLOCK;
        INCR_COUNTER(invalid_client_cnt);
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("ATP TX: Client not found\n")));
        return BCM_E_NOT_FOUND;
    }

    LOG_DEBUG(BSL_LS_TKS_ATP,
              (BSL_META("TX cli %d, flags 0x%x ctflags 0x%x, cb %p to "
                        CPUDB_KEY_FMT_EOLN), 
               client_id, client->flags, ct_flags, callback,
               CPUDB_KEY_DISP(dest_key)));

    if (ct_flags & CPUTRANS_BCAST) {  /* Forces no-ACK and next hop */
        no_ack = TRUE;
    } else {  /* Not a broadcast pkt */
        if (IS_LOCAL_CPU_KEY(dest_key)) {  /* Packet is directed loopback */
            rv = _atp_loopback(client, dest_cpu, pkt_buf, len, ct_flags);
            ATP_TX_UNLOCK;
            if(BCM_FAILURE(rv)) {
                INCR_COUNTER(lb_pkt_send_fail);
            }

            /* All done with operation */
            if (callback != NULL) {
                callback(pkt_buf, cookie, rv);
            }
            return rv;
        }
        if (dest_cpu < 0) {
            ATP_TX_UNLOCK;
            INCR_COUNTER(invalid_dest_cpu_cnt);
            return BCM_E_NOT_FOUND;
        }

        if (CLI_IS_BET(client) || (ct_flags & CPUTRANS_NO_ACK)) {
            /* Client is BET or packet forces no-ack */
            no_ack = TRUE;
        }

        if (_ATP_SIMPLE_CHECK(no_ack, ct_flags, len)) {
            /* Okay, it's a simple operation, send it out */
            rv = _atp_simple_send(dest_key, client, pkt_buf,
                                  len, ct_flags, callback, cookie);
            ATP_TX_UNLOCK;
            if (BCM_FAILURE(rv)) {
                INCR_COUNTER(tx_simple_send_fail);
            }
            return rv;
        }
    }

    /* Create and setup the transaction */
    trans = _atp_tx_trans_create(dest_cpu, client, no_ack, pkt_buf,
                                 len, ct_flags, callback, cookie);

    if (trans == NULL) {
        ATP_TX_UNLOCK;
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META("TX unable to alloc trans/pkt, cli %d\n"),
                     client_id));
        return BCM_E_RESOURCE;
    }

    rv = _atp_tx_trans_setup(trans, &trans_deleted);
    
    ATP_TX_UNLOCK;
    if (rv == BCM_E_NONE) {
        ATP_TX_THREAD_WAKE;
        /* If ATP sync send, wait on semaphor for completion */
        if (!trans_deleted) {
            rv = _atp_sync_check(trans, callback, dest_cpu);
            if (BCM_FAILURE(rv)) {
                INCR_COUNTER(tx_send_fail);
            }
        }
    } else {
        INCR_COUNTER(tx_trans_setup_fail);
    }

    return rv;
}

/****************************************************************
 *
 * Data and packet free routines
 */

/*
 * Function:
 *      atp_rx_data_alloc
 * Purpose:
 *      Allocate a buffer from the ATP transport memory pool
 * Parameters:
 *      bytes      - Number of bytes to be allocated for payload
 * Returns:
 *      Pointer to payload buffer if successful
 *      NULL if failure
 * Notes:
 *      Buffer allocated must be freed with 'atp_rx_free()'.
 *      Application must free the buffer ONLY if the called
 *      routine (to which the buffer is given) return code
 *      is NOT BCM_RX_HANDLED_OWNED.
 */

void *
atp_rx_data_alloc(int bytes)
{
    uint8 *buffer = NULL;
    uint8 *data_ptr = NULL;

    /* Allocate space from transport memory pool */

    _atp_trans_ptr->tp_data_alloc(_atp_trans_ptr->tp_unit, 
                                  bytes + CPUTRANS_HEADER_BYTES, 
                                  0, (void*)&buffer);
    if (buffer != NULL) {
        data_ptr = buffer + CPUTRANS_HEADER_BYTES;
    } else {
        INCR_COUNTER(rx_data_alloc_fail);
    }

    return (void *)data_ptr;
}


/*
 * Function:
 *      atp_rx_free
 * Purpose:
 *      Free buffers that are stolen by atp callbacks
 * Parameters:
 *      payload_ptr     - Payload pointer passed to callback
 *      pkt_ptr         - Pointer to BCM packet struct passed to callback
 * Returns:
 *      void
 * Notes:
 *      When a callback returns BCM_RX_HANDLED_OWNED, the packet
 *      data has been "stolen" by the application.  In order to free
 *      this data, the application MUST call atp_rx_free
 *      on the payload pointer and packet pointer passed to the callback.
 */

void
atp_rx_free(void *payload_ptr, void *pkt_ptr)
{
    bcm_pkt_t *pkt = (bcm_pkt_t *)pkt_ptr;
    uint8 *data = (uint8 *)payload_ptr;

    if (pkt == NULL) {
        /* It's a generic RX pointer, no packet to free */
        if (data != NULL) {
            _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit, data);
        }
    } else {
        _atp_rx_pkt_free(pkt);
    }
}

/*
 * Function:
 *      atp_tx_data_alloc
 * Purpose:
 *      Allocate buffer from the ATP transport memory pool
 * Parameters:
 *      bytes      - Number of bytes to be allocated for payload
 * Returns:
 *      Pointer to payload buffer if successful
 *      NULL if failure
 * Notes:
 *      Buffer allocated must be freed with 'atp_tx_data_free()'.
 */

void *
atp_tx_data_alloc(int bytes)
{
    uint8 *buffer = NULL;

    /* Allocate space from transport memory pool */

    _atp_trans_ptr->tp_data_alloc(_atp_trans_ptr->tp_unit, bytes,
                                  0, (void*)&buffer);
    if (NULL == buffer) {
        INCR_COUNTER(tx_data_alloc_fail);
    }
    return (void *)buffer;
}


/*
 * Function:
 *      atp_tx_data_free
 * Purpose:
 *      Free buffer allocated by atp_tx_data_alloc
 * Parameters:
 *      buffer     - buffer to free
 * Returns:
 *      void
 * Notes:
 *      Unlike atp_rx_free, the buffer pointer must be exactly
 *      the pointer returned by atp_tx_data_alloc().
 */

void
atp_tx_data_free(void *buffer)
{
    _atp_trans_ptr->tp_data_free(_atp_trans_ptr->tp_unit, buffer);
}

/****************************************************************
 *
 * ATP Packet Receive functions
 */


/*
 * Function:
 *      atp_cpudb_keys_add
 * Purpose:
 *      Add all CPU keys in a CPU DB to those known by ATP
 * Parameters:
 *      db_ref         - Reference to CPU DB to scan
 * Returns:
 *      BCM_E_XXX 
 * Notes:
 *      It's not an error if entries already exist
 */

int
atp_cpudb_keys_add(cpudb_ref_t db_ref)
{
    cpudb_entry_t *entry;

    /* Scan the DB for new addresses */
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
        BCM_IF_ERROR_RETURN(atp_key_add(entry->base.key,
                                        entry->flags & CPUDB_F_IS_LOCAL));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      atp_key_add
 * Purpose:
 *      Add a CPU key to those known to ATP
 * Parameters:
 *      key         - CPUDB key of CPU to add
 *      is_local    - Update local record if set
 * Returns:
 *      The key index if successful; else error.
 * Notes:
 *      It's not an error if the entry already exists
 */

int
atp_key_add(cpudb_key_t key, int is_local)
{
    int idx;
    int rv;

    ATP_INIT_CHECK;

    ATP_LOCK;
    if (is_local) {
        sal_memcpy(&_atp_local_key, &key, sizeof(cpudb_key_t));
    }
    idx = _atp_key_lookup(key);
    if (idx >= 0) {
        ATP_UNLOCK;
        return BCM_E_NONE;    /* Already exists, no error */
    }
    rv = _atp_key_add(key);
    ATP_UNLOCK;

    return rv < 0 ? BCM_E_RESOURCE : BCM_E_NONE;
}

/* Callback to purge queues on detach */
void
atp_attach_callback(int unit, int attach, cpudb_entry_t *cpuent, int cpuunit)
{
    if (attach) {
        LOG_VERBOSE(BSL_LS_TKS_ATP,
                    (BSL_META_U(unit,
                                "ATP attach unit %d; key " CPUDB_KEY_FMT_EOLN),
                     unit, CPUDB_KEY_DISP(cpuent->base.key)));
        atp_key_add(cpuent->base.key, cpuent->flags & CPUDB_F_IS_LOCAL);
    } else {
        if (!(cpuent->flags & CPUDB_F_IS_LOCAL)) {
            LOG_VERBOSE(BSL_LS_TKS_ATP,
                        (BSL_META_U(unit,
                                    "ATP detach unit %d; key " CPUDB_KEY_FMT_EOLN),
                         unit, CPUDB_KEY_DISP(cpuent->base.key)));
            atp_key_remove(cpuent->base.key);
	    next_hop_key_invalidate(cpuent->base.key);
        }
    }
}

/*
 * Function:
 *      atp_key_purge
 * Purpose:
 *      Purge all the current TX and RX transactions associated with a cpu
 * Parameters:
 *      key         - CPUDB key of CPU to purge
 * Returns:
 *      BCM_E_XXX;
 * Notes:
 *      ATP must be initialized.
 */

int
atp_key_purge(cpudb_key_t key)
{
    int rv = BCM_E_NONE;
    int idx;

    if (!init_done) {
        return BCM_E_INIT;
    }

    ATP_LOCK;
    idx = _atp_key_lookup(key);
    if (idx < 0) {
        rv = BCM_E_NOT_FOUND;
    } else {
        _atp_cpu_purge(idx);
    }
    ATP_UNLOCK;

    return rv;
}

/*
 * Function:
 *      atp_cpu_remove
 * Purpose:
 *      Remove a CPU from those known to ATP
 * Parameters:
 *      key         - CPUDB key of CPU to remove
 * Returns:
 *      BCM_E_NONE
 * Notes:
 *      It's not an error if the entry does not exist, but ATP must
 *      be initialized.
 */

int
atp_key_remove(cpudb_key_t key)
{
    int idx;
    int rv = BCM_E_NONE;

    if (!init_done) {
        return BCM_E_INIT;
    }

    ATP_LOCK;
    idx = _atp_key_lookup(key);
    if (idx < 0) {
        rv = BCM_E_NONE;    /* Not there, no error */
    } else {
        _atp_cpu_remove(idx);
    }
    ATP_UNLOCK;

    return rv;
}


/*
 * Function:
 *      atp_cpu_remove_all
 * Purpose:
 *      Remove all CPUs from ATP
 * Returns:
 *      BCM_E_NONE
 */

int
atp_cpu_remove_all(void)
{
    int idx;

    ATP_INIT_CHECK;
    ATP_LOCK;
    for (idx = 0; idx < CPUDB_CPU_MAX; idx++) {
        if (_atp_cpu_info[idx].flags & _ATP_CPU_VALID) {
            _atp_cpu_remove(idx);
        }
    }
    ATP_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Function:
 *      atp_cpu_count_set
 * Purpose:
 *      DEPRECATED:  Set the max number of CPUs that ATP supports
 * Parameters:
 *      count     - Number of CPUs to support
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      No longer a configurable parameter
 */

int
atp_cpu_count_set(int count)
{
    if (!init_done) {
        return BCM_E_INIT;
    }

    if (count > CPUDB_CPU_MAX) {
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      atp_cpu_count_get
 * Purpose:
 *      DEPRECATED: Get the number of CPUs supported by ATP
 * Returns:
 *      Number of CPUs supported by ATP
 */

int
atp_cpu_count_get(void)
{
    return CPUDB_CPU_MAX;
}


/*
 * Function:
 *      atp_tx_override_set/get
 * Purpose:
 *      Set (get) the atp_tx override function for the given dest CPU key
 * Parameters:
 *      dest_cpu       - Which CPU entry to update
 *      override_tx    - The function pointer to use (get: OUTPUT)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      On set, will add the CPU key to the local records if
 *      not found.
 */

int
atp_tx_override_set(cpudb_key_t dest_cpu, atp_tx_f override_tx)
{
    int idx;

    ATP_INIT_CHECK;
    /* See if CPU exists; if not add it */
    ATP_LOCK;
    idx = _atp_key_lookup(dest_cpu);
    if (idx < 0) {
        idx = _atp_key_add(dest_cpu);
    }
    if (idx >= 0) {
        _atp_cpu_info[idx].override_tx = override_tx;
    }
    ATP_UNLOCK;

    return idx < 0 ? BCM_E_FAIL : BCM_E_NONE;
}

int
atp_tx_override_get(cpudb_key_t dest_cpu, atp_tx_f *override_tx)
{
    int idx;
    int rv = BCM_E_NONE;

    if (override_tx == NULL) {
        return BCM_E_PARAM;
    }

    ATP_INIT_CHECK;

    /* See if CPU exists; if not add it */
    ATP_LOCK;
    idx = _atp_key_lookup(dest_cpu);
    if (idx < 0) {
        *override_tx = NULL;
        rv = BCM_E_NOT_FOUND;
    } else {
        *override_tx = _atp_cpu_info[idx].override_tx;
    }
    ATP_UNLOCK;

    return rv;
}


/*
 * Function:
 *      atp_tx_override_set/get
 * Purpose:
 *      Set (get) the atp_tx override function for the given dest CPU key
 * Parameters:
 *      dest_cpu       - Which CPU entry to update
 *      override_tx    - The function pointer to use (get: OUTPUT)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      On set, will add the CPU key to the local records if
 *      not found.
 */

int
atp_cpu_no_ack_set(cpudb_key_t dest_cpu, int no_ack)
{
    int idx;

    ATP_INIT_CHECK;
    /* See if CPU exists; if not add it */
    ATP_LOCK;
    idx = _atp_key_lookup(dest_cpu);
    if (idx < 0) {
        idx = _atp_key_add(dest_cpu);
    }
    if (idx >= 0) {
        if (no_ack) {
            _atp_cpu_info[idx].flags |= _ATP_CPU_NO_ACK;
        } else {
            _atp_cpu_info[idx].flags &= ~_ATP_CPU_NO_ACK;
        }
    }
    ATP_UNLOCK;

    return idx < 0 ? BCM_E_FAIL : BCM_E_NONE;
}

int
atp_cpu_no_ack_get(cpudb_key_t dest_cpu, int *no_ack)
{
    int idx;
    int rv = BCM_E_NONE;

    if (no_ack == NULL) {
        return BCM_E_PARAM;
    }

    ATP_INIT_CHECK;

    /* See if CPU exists; if not add it */
    ATP_LOCK;
    idx = _atp_key_lookup(dest_cpu);
    if (idx < 0) {
        rv = BCM_E_NOT_FOUND;
    } else {
        *no_ack = (_atp_cpu_info[idx].flags & _ATP_CPU_NO_ACK) != 0;
    }
    ATP_UNLOCK;

    return rv;
}


/*
 * Function:
 *      atp_rx_inject
 * Purpose:
 *      Inject a packet into the ATP callback sequence
 * Parameters:
 *      src_key           - From whence arrived
 *      client_id         - Who to give the packet to
 *      pkt_buf           - Pointer to data
 *      len               - Length of data in bytes
 * Returns:
 *      BCM RX return type indicating if packet data is stolen.
 * Notes:
 *      Generally used in conjunction with overriding atp_tx.  See
 *      comments at top of file.
 *
 *      This always passes NULL for the packet pointer to the callback.
 */

bcm_rx_t
atp_rx_inject(cpudb_key_t src_key, int client_id, uint8 *pkt_buf, int len)
{
    _atp_client_t *client;
    atp_client_cb_f cb;
    void *cookie;
    bcm_rx_t rv = BCM_RX_HANDLED;

    if (BCM_FAILURE(_atp_base_init_check())) {
        return BCM_RX_HANDLED;
    }

    ATP_RX_LOCK;
    client = client_find(client_id);
    if (client == NULL) {
        LOG_WARN(BSL_LS_TKS_ATP,
                 (BSL_META("ATP rx inject: Unknown client id %d\n"),
                  client_id));
        ATP_RX_UNLOCK;
        INCR_COUNTER(invalid_client_cnt);
        return BCM_RX_HANDLED;
    }

    if (client->callback != NULL) {
        cb = client->callback;
        cookie = client->cookie;
        ATP_RX_UNLOCK;
        rv = cb(src_key, client_id, NULL, pkt_buf, len, cookie);
    } else {
        ATP_RX_UNLOCK;
    }

    return rv;
}

/*
 * Function:
 *      atp_db_update_notify
 * Purpose:
 *      Atomically increment 'atp_db_update_count'.
 *      It will be used to inform the ATP send pasth that
 *      the DB entry has changed so refresh the pkt hdr.
 * Returns:
 *      none
 */
void
atp_db_update_notify(void)
{
    ATP_LOCK;
    atp_db_update_count++;
    ATP_UNLOCK;
}

#if defined(BROADCOM_DEBUG)

void
atp_counter_dump(void)
{
    LOG_CLI((BSL_META("tx_retry_cnt              = %d\n"),
              tx_retry_cnt));
    LOG_CLI((BSL_META("tx_timeout_cnt            = %d\n"),
              tx_timeout_cnt));
    LOG_CLI((BSL_META("reassem_alloc_fail        = %d\n"),
              reassem_alloc_fail));
    LOG_CLI((BSL_META("rx_trans_fail             = %d\n"),
              rx_trans_fail));
    LOG_CLI((BSL_META("stale_rx_trans            = %d\n"),
              stale_rx_trans));
    LOG_CLI((BSL_META("rx_pkt_drops              = %d\n"),
              rx_pkt_drops));
    LOG_CLI((BSL_META("ack_pkt_drops             = %d\n"),
              ack_pkt_drops));
    LOG_CLI((BSL_META("stale_rx_trans            = %d\n"),
              stale_rx_trans));
    LOG_CLI((BSL_META("rxt_pkt_alloc_fail        = %d\n"),
              rxt_pkt_alloc_fail));
    LOG_CLI((BSL_META("rx_mseg_alloc_fail        = %d\n"),
              rx_mseg_alloc_fail));
    LOG_CLI((BSL_META("tx_trans_fail             = %d\n"),
              tx_trans_fail));
    LOG_CLI((BSL_META("txt_pkt_alloc_fail        = %d\n"),
              txt_pkt_alloc_fail));
    LOG_CLI((BSL_META("lb_buf_alloc_fail         = %d\n"),
              lb_buf_alloc_fail));
    LOG_CLI((BSL_META("gc_deferrals              = %d\n"),
              gc_deferrals));
    LOG_CLI((BSL_META("gc_blocked                = %d\n"),
              gc_blocked));
    LOG_CLI((BSL_META("cli_del_tx_busy           = %d\n"),
              cli_del_tx_busy));
    LOG_CLI((BSL_META("clients_deleted           = %d\n"),
              clients_deleted));
    LOG_CLI((BSL_META("tx_data_alloc_fail        = %d\n"),
              tx_data_alloc_fail));
    LOG_CLI((BSL_META("rx_data_alloc_fail        = %d\n"),
              rx_data_alloc_fail));
    LOG_CLI((BSL_META("invalid_client_cnt        = %d\n"),
              invalid_client_cnt));
    LOG_CLI((BSL_META("invalid_dest_cpu_cnt      = %d\n"),
              invalid_dest_cpu_cnt));
    LOG_CLI((BSL_META("lb_pkt_send_fail          = %d\n"),
              lb_pkt_send_fail));
    LOG_CLI((BSL_META("tx_simple_send_fail       = %d\n"),
              tx_simple_send_fail));
    LOG_CLI((BSL_META("tx_trans_setup_fail       = %d\n"),
              tx_trans_setup_fail));
    LOG_CLI((BSL_META("tx_send_fail              = %d\n"),
              tx_send_fail));
    LOG_CLI((BSL_META("atp_not_running           = %d\n"),
              atp_not_running));
}

void
atp_dump(int verbose)
{
    int i, cpu;
    _atp_tx_trans_t *tx_trans;
    _atp_rx_trans_t *rx_trans;
    _atp_client_t *client;
    int cos, int_prio;

    LOG_CLI((BSL_META("Init %d. run %d.\n"),
              init_done, _atp_running));
    if (verbose) {
        for (i = 0; i < CPUDB_CPU_MAX; i++) {
            if (CPU_VALID(i)) {
                LOG_CLI((BSL_META("  CPU %d " CPUDB_KEY_FMT_EOLN),
                          i, CPUDB_KEY_DISP(CPU_KEY(i))));
            }
        }
    }

    LOG_CLI((BSL_META("atp_tx_mutex %p\n"),
              atp_tx_mutex));
    LOG_CLI((BSL_META("atp_rx_mutex %p\n"), 
              atp_rx_mutex));
    LOG_CLI((BSL_META("atp_rxq_mutex %p\n"),
              atp_rxq_mutex));
    LOG_CLI((BSL_META("atp_tx_sem: %p\n"),
              atp_tx_sem));
    LOG_CLI((BSL_META("atp_rx_sem: %p\n"),
              atp_rx_sem));
    LOG_CLI((BSL_META("tx_pending %d. sleep cnt %d.\n"),
              atp_tx_pending, tx_sleep_count));
    LOG_CLI((BSL_META("Cntrs: rxt_cr %d. rxt_free %d. rx raw free %d.\n"),
              rxt_create, rxt_free, rxraw_free));
    LOG_CLI((BSL_META("  txt_cr %d\n"),
              txt_create));
    LOG_CLI((BSL_META("  rx raw grab %d. tx raw grab %d.\n"),
              rxraw_grab, txraw_grab));
    LOG_CLI((BSL_META("Drops: bet %d. atp %d. mem %d. slf %d. "
              "old rx %d.\n"),
              bet_rx_drop, atp_rx_drop, mem_rx_drop, slf_rx_drop,
              old_rx_trans_drop));

    LOG_CLI((BSL_META("tx free %p. rx free %p.\n"),
              tx_trans_freelist,
              rx_trans_freelist));
             
    if (verbose) { /* Show counters */
        atp_counter_dump();
    }

    FOREACH_CLIENT(client, i) {
        cos = CPUTRANS_COS_GET(client->cos);
        if (client->cos & CPUTRANS_INT_PRIO_VALID) {
            int_prio = CPUTRANS_INT_PRIO_GET(client->cos);
        } else {
            int_prio = cos;
        }

        LOG_CLI((BSL_META("Client %d. fl 0x%x. cos %d int_prio %d. vl %d. bet sn %d\n"),
                  client->client_id, client->flags, cos, int_prio,
                  client->vlan, client->bet_tx_seq_num));
        if (verbose) {
            for (cpu = 0; cpu < CPUDB_CPU_MAX ; cpu++) {
                if (client->cpu[cpu].cpu_flags != 0) {
                    LOG_CLI((BSL_META("  CPU %d: Flags 0x%x. rx SN %d. tx SN %d.\n"),
                              cpu, client->cpu[cpu].cpu_flags,
                              client->cpu[cpu].rx_seq_num,
                              client->cpu[cpu].tx_seq_num));
                }
                
                for (rx_trans = client->cpu[cpu].rx_trans;
                     rx_trans != NULL; rx_trans = rx_trans->next) {
                    LOG_CLI((BSL_META("    CPU %d: RX %p: flags 0x%x. cpu %d. rsegs %d. "
                              "ack %d.\n"), cpu, rx_trans, rx_trans->flags,
                              rx_trans->src_cpu, rx_trans->rcv_segs,
                              rx_trans->ack_count));
                    LOG_CLI((BSL_META("      time %u. cli %d. pkt %p. len %d. seq %d\n"),
                              rx_trans->rcvd_time, rx_trans->client->client_id,
                              rx_trans->pkt, rx_trans->payload_len,
                              rx_trans->_atp_hdr.seq_num));
                }

                for (tx_trans = client->cpu[cpu].tx_trans;
                     tx_trans != NULL; tx_trans = tx_trans->next) {
                    LOG_CLI((BSL_META("    CPU %d: TX %p: flags 0x%x. ct_flags 0x%x. "
                              "cpu %d. len %d.\n"), cpu, tx_trans,
                              tx_trans->flags, tx_trans->ct_flags,
                              tx_trans->dest_cpu, tx_trans->len));
                    LOG_CLI((BSL_META("      b ack %d. last tx %u. tx_rv %d. "
                              "cli %d. sem %p\n"), tx_trans->bytes_acked,
                              tx_trans->last_tx, tx_trans->tx_rv,
                              tx_trans->client->client_id,
                              tx_trans->tx_sem));
                }
            }
        }
    }

    LOG_CLI((BSL_META("BET queue %p tail %p\n"),
              bet_queue, bet_queue_tail));
    LOG_CLI((BSL_META("atp_cpu_max: %d\n"),
              atp_cpu_max));
}

#endif /* BROADCOM_DEBUG */
