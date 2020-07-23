/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        traverse.c
 * Purpose:     RLINK traverse server
 *
 * The RLINK traverse server provides a way for RPC clients to execute
 * a BCM traverse API on an RPC server and receive callback data via
 * the client's traverse function.
 *
 * The implementation of this server uses a RLINK based protocol where
 * the RPC client issues a request and receives a reply from the
 * traverse server.
 *
 * The traverse server only processes one traverse request at a time.
 * The traverse API is executed on the server for the duration of the
 * client execution, so any blockable resources that the server
 * traverse requests will remain blocked for the entire duration.
 *
 * If a new traverse request is received while a traverse is
 * executing, then that request is queued, and will be executed in order
 * with any other outstanding teaverse requests.
 *
 * Traverse clients may have multiple traverses simultaneously.
 *
 * RLINK traverse protocol uses a standard RLINK message header
 * (msg,type,unit) followed by a client-id and message data
 * appropriate for the traverse message type.
 *
 * <msg><type><unit><c-id><s-id><msg-data>
 *
 * RLINK MSG: RLINK_MSG_TRAVERSE
 *
 * RLINK traverse message types
 *
 *   START  C>S <traverse-id><traverse-arguments>
 *   NEXT   C>S <none>
 *   QUIT   C>S <error-code><none>
 *
 *   ERROR  S>C <error-code>
 *   MORE   S>C <error-code><cb-count><callback-arguments>
 *   DONE   S>C <error-code><cb-count><callback-arguments>
 *
 * Protocol revision:
 *   Delivered protocol message contents are fixed. If there needs to be
 *   protocol extensions, then new message types are added.
 */

/* traverse server resources

   server lock
   server request queue
   server semaphore
   server thread
*/

#include <shared/bsl.h>

#include <assert.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/alloc.h>
#include <bcm/types.h>
#include <bcm_int/rpc/pack.h>
#include <bcm_int/rpc/rlink.h>
#include <bcm_int/control.h>
#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/atp.h>
#include "rlink.h"
#include "traverse.h"
#include "traverse_key.h"

#if !defined(BCM_RXP_DEBUG)
#define bcm_rx_pool_own(b,s)
#endif

#ifdef	BCM_RPC_SUPPORT

/* RLINK TRAVERSE OPTIONS

RLINK_TRAV_THREADED_SERVER

  TRUE:

    Traverse requests are handled by a threaded server that handles
    each traverse API call as a co-routine.

  FALSE:
  
    Traverse requests are handled by calling the traverse API,
    buffering the output, and serving this buffer in pieces.

  This is a memory/performance/complexity tradeoff. The threaded
  server is more complex, but incurs less memory overhead on the
  server. Because the traverse runs in its own thread context, not the
  RPC server thread context, calling BCM APIs (via RPC) from within a
  remote traverse call to the same unit being traversed can lead to
  deadlock if the traverse API holds a lock, and the API being called
  over RPC also takes the same lock. There's no good way to know
  beforehand if this situation will arise, so it's best to avoid
  calling BCM modules recursively in this fashion.

  The non-threaded server required a memory buffer that can
  accommodate the *RPC response* for the entire table. This is likely
  to be larger than the actual table size. Because the entire traverse
  is executed on the remote unit before callbacks are make, the
  non-threaded server is not susceptible to deadlock.

RLINK_TRAVERSE_MAX_REQ_LEN

  This is the maximum size expected for a traverse request. This
  typically does not need to change from the defined value.

RLINK_TRAVERSE_MAX_REPLY_LEN

  This is the maximum size expected for a traverse request. For the
  threaded server, this allows a tradeoff between duration and latency;
  larger buffers are expected to reduce overall traverse duration due
  to communication overhead, at the expense of latency at receiving the
  buffers.

RLINK_TRAVERSE_BUFFER_SIZE

  This is the maximum size expected for a non-threaded traverse
  buffer.  Ignored for a threaded server implementation.

RLINK_TRAV_THREAD_STACK

  Size of the traverse thread stack. Ignored for a non-threaded server
  implementation.
  
RLINK_TRAV_THREAD_PRIO

  Priority of the traverse thread. Ignored for a non-threaded server
  implementation.
  

*/

#ifndef RLINK_TRAV_THREADED_SERVER
#define RLINK_TRAV_THREADED_SERVER 1
#endif

/*
 * Maximum request and reply lengths
 */
#ifndef RLINK_TRAVERSE_MAX_REPLY_LEN
#define RLINK_TRAVERSE_MAX_REPLY_LEN 12*1024
#endif

#ifndef RLINK_TRAVERSE_MAX_REQ_LEN
#define RLINK_TRAVERSE_MAX_REQ_LEN RLINK_TRAVERSE_MAX_REPLY_LEN
#endif

#define TRAVS_MAGIC 0x54525653
#define TRAVC_MAGIC 0x54525643

#define ASSERT_TRAVS(s) assert((s)->magic == TRAVS_MAGIC)
#define ASSERT_TRAVC(s) assert((s)->magic == TRAVC_MAGIC)

typedef struct _rlink_travs_s {
    uint32                          magic;      /* magic numbers */
    uint32                          c_id;       /* client ID */
    uint32                          s_id;       /* server ID */
    int                             unit;       /* traverse unit */
    _bcm_traverse_handler_t         handler;    /* traverse handler */
    uint8                           *msgp;      /* output stream mid-msg ptr */
    int                             num;        /* number of replies */
    int                             flush;      /* true if flushing cb */
    int                             rrv;        /* traverse rv */
    sal_usecs_t                     atime;      /* last access time */
    bcm_rlink_traverse_data_t       *reply;     /* reply data */
    struct _rlink_travs_s           *next;      /* next record */
} _rlink_travs_t;

#ifndef RLINK_TRAVS_TRANSACTION_TIMEOUT
#define RLINK_TRAVS_TRANSACTION_TIMEOUT -1
#endif

#ifndef RLINK_TRAVS_LOCK_TIMEOUT
#define RLINK_TRAVS_LOCK_TIMEOUT sal_mutex_FOREVER
#endif

static _rlink_travs_t   *_rlink_travs_req_current;
int rlink_traverse_transaction_timeout = RLINK_TRAVS_TRANSACTION_TIMEOUT;

#if RLINK_TRAV_THREADED_SERVER

static sal_mutex_t      _rlink_travs_lock;

#define	RLINK_TRAVS_LOCK        sal_mutex_take(_rlink_travs_lock,\
                                               RLINK_TRAVS_LOCK_TIMEOUT)
#define	RLINK_TRAVS_UNLOCK      sal_mutex_give(_rlink_travs_lock)

#ifndef RLINK_TRAV_THREAD_STACK
#define	RLINK_TRAV_THREAD_STACK	SAL_THREAD_STKSZ
#endif

#ifndef  RLINK_TRAV_THREAD_PRIO
#define	RLINK_TRAV_THREAD_PRIO	50
#endif

static sal_sem_t        _rlink_travs_sem;
static sal_sem_t        _rlink_travt_sem;
static volatile sal_thread_t    _rlink_travs_thread = SAL_THREAD_ERROR;
static volatile int              _rlink_travs_thread_exit;


#ifndef RLINK_TRAVT_SLEEP_TIMEOUT
#define RLINK_TRAVT_SLEEP_TIMEOUT sal_sem_FOREVER
#endif

#define	RLINK_TRAVT_SLEEP       sal_sem_take(_rlink_travt_sem,\
                                             RLINK_TRAVT_SLEEP_TIMEOUT)
#define	RLINK_TRAVT_WAKE        sal_sem_give(_rlink_travt_sem)

#ifndef RLINK_TRAVS_SLEEP_TIMEOUT
#define RLINK_TRAVS_SLEEP_TIMEOUT sal_sem_FOREVER
#endif

#define	RLINK_TRAVS_SLEEP       sal_sem_take(_rlink_travs_sem,\
                                             RLINK_TRAVS_SLEEP_TIMEOUT)

#define	RLINK_TRAVS_WAKE        sal_sem_give(_rlink_travs_sem)


#else

/* resources for non-threaded traverse handling */

#define	RLINK_TRAVS_LOCK

#define	RLINK_TRAVS_UNLOCK


#ifndef RLINK_TRAVERSE_BUFFER_SIZE
#define RLINK_TRAVERSE_BUFFER_SIZE (4*1024*1024)
#endif

/* needs to come from generated constants */
#define RLINK_TRAVERSE_MIN_REPLY 8

/* Reply buffer overhead (over-estimate is OK) */
#define RLINK_TRAVERSE_REPLY_OVERHEAD 16
#define RLINK_TRAVERSE_REPLY_LIMIT \
        RLINK_TRAVERSE_MAX_REPLY_LEN - RLINK_TRAVERSE_REPLY_OVERHEAD

typedef struct _rlink_trav_info_s {
    int num;
    int len;
    uint8 *buf;
} _rlink_trav_info_t;

/* traverse reply buffer */
static uint8 *_rlink_trav_buffer;

/* size may be set externally prior to initialization */
int rlink_traverse_buffer_size = RLINK_TRAVERSE_BUFFER_SIZE;

/* traverse reply info buffer current length */
static int _rlink_trav_info_max_length;
static int _rlink_trav_info_length;
static int _rlink_trav_info_offset;

/* traverse reply info buffer*/
static _rlink_trav_info_t *_rlink_trav_info;


#endif /* RLINK_TRAV_THREADED_SERVER */

/* traverse client resources

   client lock
   client semaphore
   client queue
*/

typedef enum _rlink_client_state_e {
    client_start,
    client_run,
    client_done
} _rlink_client_state_t;

typedef struct _rlink_travc_s {
    uint32                          magic;      /* struct magic */
    uint32                          c_id;       /* client ID */
    uint32                          s_id;       /* server ID */
    _rlink_client_state_t           state;      /* client state */
    sal_sem_t                       sem;        /* client semaphore */
    bcm_rlink_traverse_data_t       *data;      /* request/reply data */
    int                             rrv;        /* remote error code */
    int                             done;       /* server done */
    int                             num;        /* number of replies */
    struct _rlink_travc_s           *next;      /* next record */
} _rlink_travc_t;


static sal_mutex_t      _rlink_travc_lock;
static _rlink_travc_t   *_rlink_travc_req_head;
static _rlink_travc_t   *_rlink_travc_req_tail;

static uint32 _rlink_traverse_client_id;
static uint32 _rlink_traverse_server_id;

#ifndef RLINK_TRAVC_LOCK_TIMEOUT
#define RLINK_TRAVC_LOCK_TIMEOUT sal_mutex_FOREVER
#endif

#define	RLINK_TRAVC_LOCK	sal_mutex_take(_rlink_travc_lock,\
                                               RLINK_TRAVC_LOCK_TIMEOUT)
#define	RLINK_TRAVC_UNLOCK	sal_mutex_give(_rlink_travc_lock)

#ifndef RLINK_TRAVC_SLEEP_TIMEOUT
#define RLINK_TRAVC_SLEEP_TIMEOUT sal_mutex_FOREVER
#endif

#define	RLINK_TRAVC_SLEEP(t)	sal_sem_take(t->sem, \
                                             rlink_traverse_client_timeout)
#define	RLINK_TRAVC_WAKE(t)	sal_sem_give(t->sem)

int rlink_traverse_client_timeout = RLINK_TRAVC_SLEEP_TIMEOUT;

/* Client side implementation APIs */

/* get an ID, make sure it's not zero */
STATIC uint32
_bcm_rlink_traverse_next_id(uint32 *var)
{
    uint32 id;

    id = *var;

    if (!++id) {
        ++id;
    }
    *var = id;

    return id;
}

STATIC void
_bcm_rlink_traverse_server_message_sanity(bcm_rlink_traverse_data_t *req)
{
    uint8 *ptr;
    rlink_msg_t msg;
    rlink_type_t type;
    int c_id, s_id, unit;
    /* sanity check */
    assert(req->tx_buf);
    assert(req->tx_ptr);
    assert((req->tx_ptr - req->tx_buf) >= 10);
    ptr = req->tx_buf;
    ptr = bcm_rlink_decode(ptr,&msg,&type,&unit);
    BCM_UNPACK_U32(ptr, c_id);
    BCM_UNPACK_U32(ptr, s_id);
    assert(msg == RLINK_MSG_TRAVERSE);
    assert(c_id == req->c_id);
    assert(s_id == req->s_id);
}

/* traverse server send function */
STATIC int
_bcm_rlink_traverse_server_message(int unit, _rlink_travc_t *client,
                                   rlink_type_t ty)
{
    bcm_rlink_traverse_data_t *req = client->data;
    int remunit;
    int rv;
    cpudb_key_t cpu;
    uint8 *tx_buf;
    int tx_len;

    ASSERT_TRAVC(client);
    _bcm_rlink_traverse_server_message_sanity(req);
    atp_rx_free(req->rx_buf, req->rx_pkt);
    req->rx_buf = NULL;
    req->rx_ptr = NULL;
    req->rx_len = 0;

    remunit = BCM_CONTROL(unit)->unit;
    cpu = *(cpudb_key_t *)BCM_CONTROL(unit)->drv_control;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "TRAVERSE server_message %d\n"), ty));
    (void)bcm_rlink_encode(req->tx_buf, RLINK_MSG_TRAVERSE, ty, remunit);
    tx_len = req->tx_ptr - req->tx_buf;

    _bcm_rlink_traverse_server_message_sanity(req);
    tx_buf = req->tx_buf;
    req->tx_buf = NULL;
    req->tx_ptr = NULL;
    req->tx_len = 0;
    rv = atp_tx(cpu, RLINK_CLIENT_ID, tx_buf, tx_len, 0, NULL, NULL);
    atp_tx_data_free(tx_buf);
    if (BCM_SUCCESS(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE server_message sleeping %d\n"),
                     rlink_traverse_client_timeout));
        if (RLINK_TRAVC_SLEEP(client) < 0) {
            rv = BCM_E_TIMEOUT;
        } else {
            /* error if no response received */
            if (req->rx_buf == NULL) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "TRAVERSE server_message not received\n")));
                rv = BCM_E_TIMEOUT;
            }
        }
    }

    return rv;
}

_bcm_traverse_handler_t
_bcm_rlink_traverse_lookup(uint32 *key)
{
    uint32                 key0;
    _bcm_traverse_handler_t rtn;
    int                     lo, hi, new, i, match;
    _bcm_traverse_lookup_t *sarr;

    key0 = key[0];
    sarr = _bcm_traverse_lookup;
    lo = -1;
    hi = BCM_TRAVERSE_LOOKUP_COUNT;
    match = 0;
    while (hi-lo > 1) {
        new = (hi + lo) / 2;
        if (sarr[new].key[0] > key0) {
            hi = new;
        } else if (sarr[new].key[0] < key0) {
            lo = new;
        } else {
            /* key0 is equal, check the rest */
            match = 1;
            for (i = 1; i < BCM_TRAVERSE_LOOKUP_KEYLEN; i++) {
                if (sarr[new].key[i] > key[i]) {
                    hi = new;
                    match = 0;
                    break;
                } else if (sarr[new].key[i] < key[i]) {
                    lo = new;
                    match = 0;
                    break;
                }
            }
            if (match) {
                lo = new;
                break;
            }
        }
    }
    if (match) {
        rtn = sarr[lo].routine;
    } else {
        rtn = NULL /* _bcm_rlink_traverse_handler_unavail */;
    }
    return rtn;
}

/*
  Called by a client traverse implementation to start a remote traverse.
  Initializes and registers a traverse request record.
  Does *not* send any RLINK messages to the RLINK server.
      lookup traverse ID
      init client record {
        allocate client sem
        allocate client TX buffer
        init reply count
        init client ID
        init server ID
        ...
      }
      LOCK {
        add to client list
      }
 */
int
bcm_rlink_traverse_request_start(int unit,
                                 bcm_rlink_traverse_data_t *req,
                                 uint32 *key)
{
    int rv = BCM_E_FAIL;
    int i;
    _rlink_travc_t *trav = NULL;
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "TRAVERSE request_start\n")));

    trav = sal_alloc(sizeof(*trav),"bcmTRAVC");
    if (!trav) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(trav, 0, sizeof(*trav));

    trav->sem = sal_sem_create("bcmTRAVC", sal_sem_BINARY, 0);
    if (!trav->sem) {
        rv = BCM_E_RESOURCE;
        goto error;
    }

    trav->magic = TRAVC_MAGIC;
    trav->state = client_start;
    trav->data = req;
    trav->rrv = BCM_E_NONE;
    req->parent = (void *)trav;
    trav->c_id = req->c_id =
        _bcm_rlink_traverse_next_id(&_rlink_traverse_client_id);
    trav->s_id = req->s_id = 0;
    req->rx_buf = NULL;
    req->rx_ptr = NULL;
    req->rx_len = 0;
    req->unit = unit;
    req->tx_buf = atp_tx_data_alloc(RLINK_TRAVERSE_MAX_REQ_LEN);
    if (!req->tx_buf) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "TRAVERSE request id:%x:%x\n"),
                 req->c_id, req->s_id));
    bcm_rx_pool_own(req->tx_buf,(void *)FUNCTION_NAME());
    req->tx_len = RLINK_TRAVERSE_MAX_REQ_LEN;

    /* just advance the tx_ptr - actual data rewritten later */
    req->tx_ptr = bcm_rlink_encode(req->tx_buf, RLINK_MSG_TRAVERSE, 0, 0);

    /* pack c_id, s_id */
    BCM_PACK_U32(req->tx_ptr, req->c_id);
    BCM_PACK_U32(req->tx_ptr, req->s_id);

    /* pack t_id */
    for (i=0; i<BCM_TRAVERSE_LOOKUP_KEYLEN; i++) {
        BCM_PACK_U32(req->tx_ptr, key[i]);
    }

    RLINK_TRAVC_LOCK;
    {
        /* Add traverse request to client list */
        if (_rlink_travc_req_tail == NULL) {
            _rlink_travc_req_head = _rlink_travc_req_tail = trav;
        } else {
            _rlink_travc_req_tail->next = trav;
            _rlink_travc_req_tail = trav;
        }
    }
    RLINK_TRAVC_UNLOCK;
    rv = BCM_E_NONE;
    goto done;
 error:
    if (req->tx_buf) {
        atp_tx_data_free(req->tx_buf);
        req->tx_buf = NULL;
        req->tx_ptr = NULL;
        req->tx_len = 0;
    }
    if (trav) {
        if (trav->sem) {
            sal_sem_destroy(trav->sem);
        }
        trav->magic = ~trav->magic;
        sal_free(trav);
    }
 done:
    return rv;
}

/*
 *Get a traverse reply. If there are no replies, and the remote
 *traverse has not started, or signaled completion, then issue a request
 *for traverse data and wait for a response.
 *    if no replies
 *      if server not complete
 *        send server message NEXT
 *        wait for server response
 *        when  MORE
 *          // skip to below
 *        when  DONE
 *          set server-complete
 *        when  ERROR
 *          set server-complete
 *          set rrv to server-error
 *          return false
 *        else
 *          set server-complete
 *          set rrv to internal-error
 *          return false
 *      else
 *        set rrv to BCM_E_NONE
 *        return false
 *      end
 *    end
 *    unpack reply
 *    return true
 *    end
 */

int
bcm_rlink_traverse_reply_get(int unit, bcm_rlink_traverse_data_t *req)
{
    _rlink_travc_t *parent = (_rlink_travc_t *)req->parent;
    int rv;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "TRAVERSE reply_get\n")));
    ASSERT_TRAVC(parent);
    switch (parent->state) {
    case client_start:
        /* always send a message when in client_start. */
        rv = _bcm_rlink_traverse_server_message(unit, parent,
                                                RLINK_TYPE_START);
        if (BCM_FAILURE(rv)) {
            /* no response, or client shutting down */
            parent->state = client_done;
            parent->rrv = BCM_E_FAIL;
        }
        break;
    case client_run:
        /* Set s_id for first response */
        if (!parent->s_id) {
            parent->s_id = req->s_id;
        } else {
            assert(parent->c_id == req->c_id);
            assert(parent->s_id == req->s_id);
        }
        /* send a message if need more responses and server not done */
        if (parent->num == 0) {
            if (!parent->done) {
                rv = _bcm_rlink_traverse_server_message(unit, parent,
                                                        RLINK_TYPE_NEXT);
                if (BCM_FAILURE(rv)) {
                    /* no response, or client shutting down */
                    parent->state = client_done;
                    parent->rrv = BCM_E_FAIL;
                }
            } else {
                /* that's it, then */
                parent->state = client_done;
            }
        }
        break;
    case client_done:
        /* done - no more messages are sent */
        break;
    }

    if (parent->state == client_run && parent->num > 0) {
        /* one response is extracted at a time */
        parent->num--;
    }

    assert(parent->state != client_start);
    return (parent->state == client_run);
}



/*
  Indicate that this request is done. If the traverse server has not
  indicated completion, then issue a done message.
      if server not complete
        send server message QUIT with error code
        wait for server response
      end
      LOCK {
        unlink from client list
      }
      deallocate RX/TX buffer
      deallocate sem
 */
int
bcm_rlink_traverse_request_done(int unit, int trav_rv,
                                bcm_rlink_traverse_data_t *req)
{
    _rlink_travc_t *parent = (_rlink_travc_t *)req->parent;
    int rv;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "TRAVERSE request_done\n")));
    ASSERT_TRAVC(parent);
    rv = parent->rrv;
    switch (parent->state) {
    case client_start:
        /* This should never be in client_start. Either
           bcm_rlink_traverse_request_start() should have returned an
           error, and this routine never reached, or a message
           received a a result of bcm_rlink_traverse_reply_get() would have
           updated the client state. */
        assert(1);
        break;
    case client_run:
        /* If the server is not done, send one last message. There
           will be at least one subsequent message from the server,
           but it will be effectively ignored. */
        if (!parent->done) {
            BCM_PACK_U32(req->tx_ptr, trav_rv);
            rv = _bcm_rlink_traverse_server_message(unit, parent,
                                                    RLINK_TYPE_QUIT);
            if (BCM_SUCCESS(rv)) {
                rv = parent->rrv;
            }
        } else {
            /* Server was done, so synthesize the client return value
               based on the typical behavior of traverse implementations,
               which is to return the last value of the callback */
            rv = trav_rv;
        }
        break;
    case client_done:
        break;
    }
    RLINK_TRAVC_LOCK;
    {
        if (_rlink_travc_req_head == parent) {
            _rlink_travc_req_head = parent->next;
            if (_rlink_travc_req_tail == parent) {
                _rlink_travc_req_tail = NULL;
            }
        } else {
            _rlink_travc_t *preq;
            for (preq = _rlink_travc_req_head; preq; preq = preq->next) {
                if (preq->next == parent) {
                    preq->next = parent->next;
                    if (_rlink_travc_req_tail == parent) {
                        _rlink_travc_req_tail = parent->next;
                    }
                    break;
                }
            }
        }
    }
    RLINK_TRAVC_UNLOCK;

    /* If there are still TX or RX buffers, free them now. */
    if (req->rx_buf) {
        atp_rx_free(req->rx_buf, req->rx_pkt);
    }
    if (req->tx_buf) {
        atp_tx_data_free(req->tx_buf);
    }
    sal_sem_destroy(parent->sem);
    parent->magic = ~parent->magic;
    sal_free(parent);

    return rv;
}



#if RLINK_TRAV_THREADED_SERVER

/* Threaded Server implementation APIs */

STATIC int
_bcm_rlink_traverse_client_message(_rlink_travs_t *server, rlink_type_t ty)
{
    bcm_rlink_traverse_data_t *req = server->reply;
    int rv = BCM_E_NONE;

    assert(server != NULL);
    assert(req != NULL);
    (void)bcm_rlink_encode(req->tx_buf, RLINK_MSG_TRAVERSE, ty, req->unit);
    req->tx_len = req->tx_ptr - req->tx_buf;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE client_message waking travs\n")));
#if 0
    if (ty == RLINK_TYPE_MORE) {
        /* If MORE, then zero the reply pointer to check if a response
           was received. */
        server->reply = NULL;
    }
#else
    /* done with reply */
    server->reply = NULL;
#endif
    RLINK_TRAVS_WAKE;
    if (ty == RLINK_TYPE_MORE) {
        int s_rv;

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE client_message sleeping travt\n")));
        s_rv = RLINK_TRAVT_SLEEP;
        /* If a message was received, then the data pointer has been
           updated. */
        if (s_rv < 0 ||
            server->reply == NULL || server->reply->rx_buf == NULL) {
            /* nothing received */
            rv = BCM_E_FAIL;
        } else {
            rv = BCM_E_NONE;
        }
    }

    return rv;
}


/*
  Check to see if there's enough room for a reply. If not,
  send out current data and wait for a response.

  If there's an error sending out a message, then return that error code.
  Normally behaving traverse functions should then exit.
 */
int
bcm_rlink_traverse_reply_check(bcm_rlink_traverse_data_t *data, int size)
{
    _rlink_travs_t *parent;
    int rv = BCM_E_NONE;
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE reply_check%s\n"),
                 data ? "" : " - no data"));

    if (data) {
        int len;

        parent = (_rlink_travs_t *)data->parent;
        ASSERT_TRAVS(parent);
        assert(parent->reply == data);
        if (!parent->flush) {
            len = data->tx_ptr - data->tx_buf;
            if ((len + size) > data->tx_len) {
                BCM_PACK_U32(parent->msgp, 0);
                BCM_PACK_U32(parent->msgp, parent->num);
                rv = _bcm_rlink_traverse_client_message(parent,
                                                        RLINK_TYPE_MORE);
                parent->num = 0;
            }
            parent->num++;
        } else {
            rv = parent->rrv;
        }
    } else {
        rv = BCM_E_FAIL;
    }

    return rv;
}

/*
  Flush any outstanding callback data.
      if server error code
        send ERROR message
      else
        # edge case: if the last MORE message contained all remaining
        # callbacks, there are no new callback to be sent. However,
        # the client doesn't know that, so send an empty DONE message
        update callback count
        send DONE message
      end
 */

int
bcm_rlink_traverse_reply_done(bcm_rlink_traverse_data_t *data, int rv)
{
    _rlink_travs_t *parent;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE reply_done%s\n"),
                 data ? "" : " - no data"));
    if (data) {
        /* If there's a pending request from a client */
        parent = (_rlink_travs_t *)data->parent;

        if (parent) {
            ASSERT_TRAVS(parent);
            assert(parent->reply == data);
            /* Even when flushing, always send a done message */
            BCM_PACK_U32(parent->msgp, rv);
            BCM_PACK_U32(parent->msgp, parent->flush ? 0 : parent->num);
            (void) _bcm_rlink_traverse_client_message(parent, RLINK_TYPE_DONE);
        } else if (BCM_SUCCESS(rv)) {
            rv = BCM_E_FAIL;
        }
    }

    return rv;
}

int
bcm_rlink_traverse_device_clear(int unit)
{
    int rv = BCM_E_NOT_FOUND;

    RLINK_TRAVS_LOCK;
    {
        if (_rlink_travs_req_current) {
            if (unit < 0 || _rlink_travs_req_current->unit == unit) {
                _rlink_travs_req_current->flush = TRUE;
                _rlink_travs_req_current->rrv = BCM_E_FAIL;
                rv = BCM_E_NONE;
            }
        }
    }
    RLINK_TRAVS_UNLOCK;

    if (BCM_SUCCESS(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE server_clear waking travt\n")));
        RLINK_TRAVT_WAKE;
    }

    return rv;
}

/*
  Clear any outstanding traverse server messages
      LOCK {
      throw away all records except head
      for server record head
        mark shutdown
        signal server lock
      }
 */
int
bcm_rlink_traverse_server_clear(void)
{
    return bcm_rlink_traverse_device_clear(-1);
}

/*
 * Traverse server thread
 */
STATIC void
_bcm_rlink_travs_thread(void *cookie)
{
    _rlink_travs_t *cur = NULL;

    COMPILER_REFERENCE(cookie);

    while (!_rlink_travs_thread_exit) {

	RLINK_TRAVS_LOCK;
        {
            /* get a traverse request */
            cur =  _rlink_travs_req_current;
        }
	RLINK_TRAVS_UNLOCK;

	/* execute traverse */
	if (cur) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("TRAVERSE begin\n")));
	    cur->handler(&cur->reply);
            RLINK_TRAVS_LOCK;
            {
                _rlink_travs_req_current = NULL;
            }
            RLINK_TRAVS_UNLOCK;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("TRAVERSE end\n")));
            cur->magic = ~cur->magic;
            sal_free(cur);
            cur = NULL;
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("TRAVERSE travs_thread sleeping\n")));
            RLINK_TRAVT_SLEEP;
        }
    }
    _rlink_travs_thread = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}


STATIC int
_bcm_rlink_travs_thread_signal(void)
{
    int rv;
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE signal waking travt\n")));
    RLINK_TRAVT_WAKE;

    /* wait for traverse server to signal back */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE signal sleeping travs\n")));
    rv = RLINK_TRAVS_SLEEP;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE signal woken up travs\n")));

    return rv < 0 ? BCM_E_TIMEOUT : BCM_E_NONE;
}

STATIC int
_bcm_rlink_traverse_run(_rlink_travs_t *trav)
{
    COMPILER_REFERENCE(trav);
    return _bcm_rlink_travs_thread_signal();
}

#else /* !THREADED_SERVER */

STATIC void
_bcm_rlink_set_current_len(int len)
{
    if (len > _rlink_trav_info[_rlink_trav_info_offset].len) {
        /* .len may not be entirely accurate, but should be at least an
           overestimate */
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("TRAVERSE set_current_len len=%d actual=%d\n"),
                  _rlink_trav_info[_rlink_trav_info_offset].len, len));
    }
    /* set to calculated length */
    _rlink_trav_info[_rlink_trav_info_offset].len = len;
}

/*
  If there's no room in the current segment, bump to the next info ptr.
 */
int
bcm_rlink_traverse_reply_check(bcm_rlink_traverse_data_t *data, int size)
{
    int len;

    len = data->tx_ptr - _rlink_trav_info[_rlink_trav_info_offset].buf;
    if ((len + size) > RLINK_TRAVERSE_REPLY_LIMIT) {
        _bcm_rlink_set_current_len(len);

        /* next segment */
        _rlink_trav_info_offset++;
        assert(_rlink_trav_info_offset < _rlink_trav_info_max_length);
        _rlink_trav_info[_rlink_trav_info_offset].num = 0;
        _rlink_trav_info[_rlink_trav_info_offset].len = 0;
        _rlink_trav_info[_rlink_trav_info_offset].buf = data->tx_ptr;
    }
    
    _rlink_trav_info[_rlink_trav_info_offset].len += size;
    _rlink_trav_info[_rlink_trav_info_offset].num++;

    return BCM_E_NONE; /* always pack response */
}

/*
  Save RV
 */
int
bcm_rlink_traverse_reply_done(bcm_rlink_traverse_data_t *data, int rv)
{
    _rlink_travs_t *parent = (_rlink_travs_t *)data->parent;
    int len;

    ASSERT_TRAVS(parent);
    len = data->tx_ptr - _rlink_trav_info[_rlink_trav_info_offset].buf;
    _bcm_rlink_set_current_len(len);
    parent->rrv = rv;
    return BCM_E_NONE;
}

/*
 */
int
bcm_rlink_traverse_server_clear(void)
{
    RLINK_TRAVS_LOCK;
    {
        _rlink_travs_req_current = NULL;
    }
    RLINK_TRAVS_UNLOCK;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE server_clear\n")));
    return BCM_E_NONE;
}


STATIC int
_bcm_rlink_trav_done(bcm_rlink_traverse_data_t *data)
{
    (void)bcm_rlink_encode(data->tx_buf,
                           RLINK_MSG_TRAVERSE,
                           RLINK_TYPE_DONE, data->unit);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE sync traverse done\n")));
    /* mark done */
    _rlink_trav_info_offset = 0;
    _rlink_trav_info_length = 0;
    bcm_rlink_traverse_server_clear();

    return BCM_E_NONE;
}


STATIC int
_bcm_rlink_trav_send_reply(bcm_rlink_traverse_data_t *data)
{
    _rlink_travs_t *parent = (_rlink_travs_t *)data->parent;

    ASSERT_TRAVS(parent);
    assert(parent->reply == data);

    sal_memcpy(data->tx_ptr,
               _rlink_trav_info[_rlink_trav_info_offset].buf,
               _rlink_trav_info[_rlink_trav_info_offset].len);
    data->tx_ptr += _rlink_trav_info[_rlink_trav_info_offset].len;
    BCM_PACK_U32(parent->msgp, 0);
    BCM_PACK_U32(parent->msgp, _rlink_trav_info[_rlink_trav_info_offset].num);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE sync traverse send reply %d/%d\n"),
                 _rlink_trav_info_offset,_rlink_trav_info_length));
    _rlink_trav_info_offset++;
    if (_rlink_trav_info_offset >= _rlink_trav_info_length) {
        _bcm_rlink_trav_done(data);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_rlink_traverse_run(_rlink_travs_t *trav)
{
    bcm_rlink_traverse_data_t reply;
    bcm_rlink_traverse_data_t *replyp = &reply;
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE traverse run sync begin\n")));
    assert(_rlink_trav_info);
    assert(_rlink_trav_buffer);
    assert(_rlink_travs_req_current);

    /* save original reply info */
    reply = *trav->reply;

    /* Use the traverse buffer instead of the reply buffer in a copy
       of the reply structure. This allows all the traverse replies
       to be saved at once. */
    reply.tx_buf = reply.tx_ptr = _rlink_trav_buffer;

    /* init response info */
    _rlink_trav_info_offset = 0;
    _rlink_trav_info[_rlink_trav_info_offset].num = 0;
    _rlink_trav_info[_rlink_trav_info_offset].len = 0;
    _rlink_trav_info[_rlink_trav_info_offset].buf = reply.tx_ptr;

    /* run traverse */
    trav->handler(&replyp);

    /* send first reply */
    _rlink_trav_info_length = _rlink_trav_info_offset+1;
    _rlink_trav_info_offset = 0;
    _bcm_rlink_trav_send_reply(trav->reply);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("TRAVERSE traverse run sync end\n")));

    return BCM_E_NONE;
}



#endif /* RLINK_TRAV_THREADED_SERVER */

/* RLINK module interfaces */



/*
  Clear any outstanding client traverses
        foreach client record
          mark shutdown
          signal client lock
 */
int
bcm_rlink_traverse_client_clear(void)
{
    _rlink_travc_t *trav;
    RLINK_TRAVC_LOCK;
    {
        trav = _rlink_travc_req_head;
        while (trav) {
            trav->state = client_done;
            sal_sem_give(trav->sem);
        }
    }
    RLINK_TRAVC_UNLOCK;

    return BCM_E_NONE;
}


/*
  Handle a traverse request
    Client and server sides have slightly different buffer management
    requirements.

    For the server:
      allocate TX buffer
      rlink_traverse_message()
      atp_rx_free(rx buffer)
      atp_tx(data_out, len_out_actual)
      atp_tx_free(tx buffer)
      
    For the client:
      allocate TX buffer
      rlink_traverse_message()
        TX and RX free happens in _bcm_rlink_traverse_server_message()
 */

int
bcm_rlink_traverse_request(rlink_type_t type,
                           cpudb_key_t cpu, bcm_pkt_t *rx_pkt,
                           uint8 *rx_buf, int len)
{
    uint8 *tx_buf;
    int tx_len = RLINK_TRAVERSE_MAX_REQ_LEN;
    int actual;
    int rv = BCM_E_FAIL;
    int server;

    server = (type == RLINK_TYPE_START ||
              type == RLINK_TYPE_NEXT ||
              type == RLINK_TYPE_QUIT);
    tx_buf = atp_tx_data_alloc(tx_len);
    if (tx_buf) {
        bcm_rx_pool_own(tx_buf, (void *)FUNCTION_NAME());
        bcm_rx_pool_own(rx_buf, (void *)FUNCTION_NAME());
        rv = bcm_rlink_traverse_message(rx_pkt,
                                        rx_buf, len, tx_buf, tx_len, &actual);
        if (server) {
            atp_rx_free(rx_buf, rx_pkt);
            if (BCM_SUCCESS(rv)) {
                assert(actual > 0);
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META("TRAVERSE traverse_request %d\n"),
                             actual));
                rv = atp_tx(cpu, RLINK_CLIENT_ID, tx_buf, actual, 0,
                            NULL, NULL);
            }
            atp_tx_data_free(tx_buf);
        }
    } else {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("TRAVERSE traverse_request could not alloc tx\n")));
        atp_rx_free(rx_buf, rx_pkt);
        rv = BCM_E_MEMORY;
    }

    return rv;
}


STATIC void
_bcm_rlink_trav_msg_prep_reply(bcm_rlink_traverse_data_t *data)
{
    _rlink_travs_t *trav = (_rlink_travs_t *)data->parent;

    ASSERT_TRAVS(trav);
    /* prepare MORE message, but could be overwritten by DONE */
    data->tx_ptr = bcm_rlink_encode(data->tx_ptr,
                                    RLINK_MSG_TRAVERSE,
                                    RLINK_TYPE_MORE, data->unit);
    BCM_PACK_U32(data->tx_ptr, data->c_id);
    BCM_PACK_U32(data->tx_ptr, data->s_id);
    /* save the location for error and traverse reply count to be
       filled in later */
    trav->msgp = data->tx_ptr;
    trav->num = 0;
    trav->rrv = BCM_E_NONE;
    /* init return value and reply count to 0,
       will be updated when packet sent */
    BCM_PACK_U32(data->tx_ptr, 0);
    BCM_PACK_U32(data->tx_ptr, 0);
}

/* handle START message
   received by server from client
      when START
        _rlink_trav_msg_start()
          create server record
          link to server list
          handle stale traverse head
          wake up traverse thread

   Need to have queuing and non-queuing versions.

          
*/

STATIC void
_bcm_rlink_trav_prep_error(bcm_rlink_traverse_data_t *data, int rv)
{
    /* prepare an error response */
    data->tx_ptr = bcm_rlink_encode(data->tx_ptr,
                                    RLINK_MSG_TRAVERSE,
                                    RLINK_TYPE_ERROR, data->unit);
    BCM_PACK_U32(data->tx_ptr, data->c_id);
    BCM_PACK_U32(data->tx_ptr, data->s_id);
    BCM_PACK_U32(data->tx_ptr, rv);
}

STATIC int
_bcm_rlink_trav_msg_start(bcm_rlink_traverse_data_t *data)
{
    int rv = BCM_E_FAIL;
    int i;
    uint32 key[BCM_TRAVERSE_LOOKUP_KEYLEN];
    _rlink_travs_t *trav;
    _bcm_traverse_handler_t handler;

    trav = NULL;
    /* unpack t_id */
    for (i=0; i<BCM_TRAVERSE_LOOKUP_KEYLEN; i++) {
        BCM_UNPACK_U32(data->rx_ptr, key[i]);
    }

    handler = _bcm_rlink_traverse_lookup(key);
    if (handler == NULL) {
        _bcm_rlink_trav_prep_error(data, BCM_E_NOT_FOUND);
        return BCM_E_NONE;
    }

    /* If not queuing requests, then, if the server thread is busy,
       reject the request */
    RLINK_TRAVS_LOCK;
    {
        trav = _rlink_travs_req_current;
    }
    RLINK_TRAVS_UNLOCK;

    if (trav) {
        int duration;
        /* If stale transaction timeouts are enabled, check to see if
           trav is stale. If not, return BUSY, otherwise clear it out. */
        duration = SAL_USECS_SUB(sal_time_usecs(), trav->atime);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_start: cur:%d\n"),
                     duration));
        if (rlink_traverse_transaction_timeout <= 0 ||
            (rlink_traverse_transaction_timeout > 0 &&
             duration > 0 && duration < rlink_traverse_transaction_timeout)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("TRAVERSE msg_start: busy\n")));
            _bcm_rlink_trav_prep_error(data, BCM_E_BUSY);
            return BCM_E_NONE;
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META("TRAVERSE msg_start: old transaction\n")));
            /* For old transactions, just recycle the old server record.
               Buffered server doesn't need to do anything. */
#if RLINK_TRAV_THREADED_SERVER
            /* Threaded server needs to quit the current traverse execution
               and wait for it to complete. */
            trav->flush = TRUE;
            trav->rrv = BCM_E_FAIL;
            _bcm_rlink_travs_thread_signal();
#endif
        }
    } else {
        /* Allocating a server record even for a single threaded or
           non-threaded server will accomodate future multiple
           transaction designs. */
        trav = sal_alloc(sizeof(*trav),"bcmTRAVS");
    }

    if (trav) {
        sal_memset(trav, 0, sizeof(*trav));
        trav->magic = TRAVS_MAGIC;
        trav->atime = sal_time_usecs();
        trav->c_id = data->c_id;
        assert(data->s_id == 0);
        trav->s_id = data->s_id =
            _bcm_rlink_traverse_next_id(&_rlink_traverse_server_id);
        trav->handler = handler;
        trav->reply = data;
        trav->reply->parent = (void *)trav;
        _bcm_rlink_trav_msg_prep_reply(trav->reply);
        RLINK_TRAVS_LOCK;
        {
            _rlink_travs_req_current = trav;
        }
        RLINK_TRAVS_UNLOCK;
        rv = _bcm_rlink_traverse_run(trav);
    } else {
        rv = BCM_E_MEMORY;
    }
    return rv;
}

STATIC int
_bcm_rlink_travs_get(bcm_rlink_traverse_data_t *data, _rlink_travs_t **tp)
{
    _rlink_travs_t *trav = NULL;
    int rv = BCM_E_NOT_FOUND;

    RLINK_TRAVS_LOCK;
    {
        if (_rlink_travs_req_current != NULL &&
            _rlink_travs_req_current->s_id == data->s_id &&
            _rlink_travs_req_current->c_id == data->c_id) {
            trav = _rlink_travs_req_current;
            rv = BCM_E_NONE;
        }
    }
    RLINK_TRAVS_UNLOCK;

    if (BCM_SUCCESS(rv)) {
        /* do some common setup */
        ASSERT_TRAVS(trav);
        data->parent = (void *)trav;
        trav->atime = sal_time_usecs();
        trav->reply = data;
        _bcm_rlink_trav_msg_prep_reply(data);
        *tp = trav;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("TRAVERSE could not find id %x:%x\n"),
                   data->c_id, data->s_id));
        _bcm_rlink_trav_prep_error(data, BCM_E_NOT_FOUND);
    }

    return rv;
}


/* handle NEXT message
   received by server from client
      when NEXT
        _rlink_trav_msg_next()
          if c-id == current-c-id
            update server record
            wake up traverse thread
          else
            error
          end

*/
STATIC int
_bcm_rlink_trav_msg_next(bcm_rlink_traverse_data_t *data)
{
    int rv = BCM_E_FAIL;
    _rlink_travs_t *trav;

    rv = _bcm_rlink_travs_get(data, &trav);
    if (BCM_SUCCESS(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_next waking travt id=%x:%x\n"),
                     data->c_id, data->s_id));
#if RLINK_TRAV_THREADED_SERVER
        _bcm_rlink_travs_thread_signal();
#else
        _bcm_rlink_trav_send_reply(data);
#endif /* RLINK_TRAV_THREADED_SERVER */
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_next id %x:%x not found\n"),
                     data->c_id, data->s_id));
    }

    /* travs_get prepares a reply error message */
    return BCM_E_NONE;
}


/* handle QUIT message
   received by server from client
      when QUIT
        _rlink_trav_msg_quit()
          if c-id == current-c-id
            update server record
              set client-quit
            wake up traverse thread
          else
            error
          end

*/
STATIC int
_bcm_rlink_trav_msg_quit(bcm_rlink_traverse_data_t *data)
{
    int rv = BCM_E_FAIL;
    _rlink_travs_t *trav;

    rv = _bcm_rlink_travs_get(data, &trav);

    if (BCM_SUCCESS(rv)) {
        trav->flush = TRUE;
        BCM_UNPACK_U32(data->rx_ptr, trav->rrv);

#if RLINK_TRAV_THREADED_SERVER
        _bcm_rlink_travs_thread_signal();
#else
        _bcm_rlink_trav_done(data);
#endif /* RLINK_TRAV_THREADED_SERVER */
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_quit id %x:%x not found\n"),
                     data->c_id, data->s_id));
    }

    /* travs_get prepares a reply error message */
    return BCM_E_NONE;
}

STATIC int
_bcm_rlink_travc_get(bcm_rlink_traverse_data_t *data, _rlink_travc_t **tp)
{
    int rv = BCM_E_NOT_FOUND;
    _rlink_travc_t *trav;
    RLINK_TRAVC_LOCK;
    {
        for (trav = _rlink_travc_req_head; trav; trav=trav->next) {
            if (trav->c_id == data->c_id) {
                rv = BCM_E_NONE;
                break;
            }
        }
    }
    RLINK_TRAVC_UNLOCK;
    if (BCM_SUCCESS(rv)) {
        ASSERT_TRAVC(trav);
        /* update data pointers */
        *trav->data = *data;
        trav->data->parent = (void *)trav;
        *tp = trav;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("TRAVERSE could not find c_id %x\n"),
                   data->c_id));
    }

    return rv;
}

/* handle ERROR message
   received by client from server
      when ERROR
        _rlink_trav_msg_error()
          if exists c-id
            update client record
            wake up client traverse
          else
            error
          end

*/
STATIC int
_bcm_rlink_trav_msg_error(bcm_rlink_traverse_data_t *data)
{
    _rlink_travc_t *trav;
    int rv;

    rv = _bcm_rlink_travc_get(data,&trav);
    if (BCM_SUCCESS(rv)) {

        BCM_UNPACK_U32(trav->data->rx_ptr, trav->rrv);
        trav->state = client_done;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_err (%d) waking\n"),
                     trav->rrv));
        RLINK_TRAVC_WAKE(trav);
    }
    return rv;
}


/* handle MORE message
   received by client from server
      when MORE
        _rlink_trav_msg_more()
          if exists c-id
            update client record
            wake up client traverse
          else
            error
          end

*/
STATIC int
_bcm_rlink_trav_msg_more(bcm_rlink_traverse_data_t *data)
{
    _rlink_travc_t *trav;
    int rv;

    rv = _bcm_rlink_travc_get(data,&trav);
    if (BCM_SUCCESS(rv)) {

        /* get return value and number of callback records */
        BCM_UNPACK_U32(trav->data->rx_ptr, trav->rrv);
        BCM_UNPACK_U32(trav->data->rx_ptr, trav->num);
    
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_more (%d) waking\n"),
                     trav->rrv));
        /* Prepare next TX message */
        trav->data->tx_ptr = bcm_rlink_encode(trav->data->tx_buf,
                                              RLINK_MSG_TRAVERSE,
                                              RLINK_TYPE_NEXT, 0);
        BCM_PACK_U32(trav->data->tx_ptr, data->c_id);
        BCM_PACK_U32(trav->data->tx_ptr, data->s_id);
        trav->state = client_run;
        RLINK_TRAVC_WAKE(trav);
    }
    return rv;
}


/* handle DONE message
   received by client from server
      when DONE
        _rlink_trav_msg_done()
          if exists c-id
            update client record
            wake up client traverse
          else
            error
          end

*/
STATIC int
_bcm_rlink_trav_msg_done(bcm_rlink_traverse_data_t *data)
{
    _rlink_travc_t *trav;
    int rv;

    rv = _bcm_rlink_travc_get(data,&trav);
    if (BCM_SUCCESS(rv)) {

        BCM_UNPACK_U32(trav->data->rx_ptr, trav->rrv);
        BCM_UNPACK_U32(trav->data->rx_ptr, trav->num);
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("TRAVERSE msg_done (%d) waking\n"),
                     trav->rrv));
        trav->state = trav->num > 0 ? client_run : client_done;
        trav->done = TRUE;
        RLINK_TRAVC_WAKE(trav);
    }
    return rv;
}


/*
  Handle a traverse server request, and receive a response buffer
      decode RLINK header (msg,type,unit)
      decode TRAVERSE c-id
      else
        send ERROR
 */
int
bcm_rlink_traverse_message(bcm_pkt_t *rx_pkt,
                           uint8 *data_in, int len_in,
                           uint8 *data_out, int len_out, int *actual)
{
    rlink_msg_t msg;
    rlink_type_t type;
    int unit;
    bcm_rlink_traverse_data_t data;
    int rv = BCM_E_FAIL;

    data.rx_pkt = rx_pkt;
    data.rx_buf = data_in;
    data.rx_ptr = bcm_rlink_decode(data.rx_buf, &msg, &type, &unit);
    data.rx_len = len_in;
    data.tx_buf = data_out;
    data.tx_ptr = data_out;
    data.tx_len = len_out;
    data.unit   = unit;
    data.parent = NULL;
    BCM_UNPACK_U32(data.rx_ptr, data.c_id);
    BCM_UNPACK_U32(data.rx_ptr, data.s_id);
    switch (type) {
    case RLINK_TYPE_START:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE START %d (%x:%x)\n"),
                     unit, data.c_id, data.s_id));
        rv = _bcm_rlink_trav_msg_start(&data);
        break;
    case RLINK_TYPE_NEXT:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE NEXT %d (%x:%x)\n"),
                     unit, data.c_id, data.s_id));
        rv = _bcm_rlink_trav_msg_next(&data);
        break;
    case RLINK_TYPE_QUIT:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE QUIT %d (%x:%x)\n"),
                     unit, data.c_id, data.s_id));
        rv = _bcm_rlink_trav_msg_quit(&data);
        break;
    case RLINK_TYPE_ERROR:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE ERROR %d (%x:%x)\n"),
                     unit, data.c_id, data.s_id));
        rv = _bcm_rlink_trav_msg_error(&data);
        break;
    case RLINK_TYPE_MORE:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE MORE %d (%x:%x)\n"),
                     unit, data.c_id, data.s_id));
        rv = _bcm_rlink_trav_msg_more(&data);
        break;
    case RLINK_TYPE_DONE:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE DONE %d (%x:%x)\n"),
                     unit, data.c_id, data.s_id));
        rv = _bcm_rlink_trav_msg_done(&data);
        break;
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "TRAVERSE %d? %d\n"), type, unit));
        rv = BCM_E_NOT_FOUND;
        break;
    }

    if (actual) {
        *actual = data.tx_ptr - data.tx_buf;
    }

    return rv;
}

/*
  Initialize RLINK traverse subsystem
 */
int
bcm_rlink_traverse_server_init(void)
{
    int rv;

#if RLINK_TRAV_THREADED_SERVER
    _rlink_travs_lock = sal_mutex_create("bcmTRAVS");
    if (_rlink_travs_lock == NULL) {
        rv = BCM_E_RESOURCE;
        goto error;
    }

    _rlink_travs_sem  = sal_sem_create("bcmTRAVS", sal_sem_BINARY, 0);
    if (_rlink_travs_sem == NULL) {
        rv = BCM_E_RESOURCE;
        goto error;
    }

    _rlink_travt_sem  = sal_sem_create("bcmTRAVT", sal_sem_BINARY, 0);
    if (_rlink_travt_sem == NULL) {
        rv = BCM_E_RESOURCE;
        goto error;
    }

    /* init traverse thread */
    _rlink_travs_thread_exit = FALSE;
    _rlink_travs_thread = sal_thread_create("bcmTRAVS",
                                               RLINK_TRAV_THREAD_STACK,
                                               RLINK_TRAV_THREAD_PRIO,
                                               _bcm_rlink_travs_thread,
                                               NULL);
    if (_rlink_travs_thread == SAL_THREAD_ERROR) {
        rv = BCM_E_RESOURCE;
        goto error;
    }
#else
    /* non threaded initialization */
    _rlink_trav_buffer = sal_alloc(rlink_traverse_buffer_size, "bcmTRAVS");
    if (_rlink_trav_buffer == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    /* Calculate the maximum number of buffer pointers needed based on
     the traverse buffer size, traverse reply length and traverse
     reply overhead, rounding up. CPP complains if this calculation is
     done at compile time. */

    _rlink_trav_info_max_length = rlink_traverse_buffer_size;
    _rlink_trav_info_max_length /= RLINK_TRAVERSE_REPLY_LIMIT;
    _rlink_trav_info_max_length++;

    _rlink_trav_info =
        sal_alloc(_rlink_trav_info_max_length * sizeof(*_rlink_trav_info),
                  "bcmTRAVS");
    if (_rlink_trav_info == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
#endif /* RLINK_TRAV_THREADED_SERVER */
    rv = BCM_E_NONE;
    goto done;
 error:
    (void)bcm_rlink_traverse_deinit();
 done:
    return rv;
}

int
bcm_rlink_traverse_client_init(void)
{
    int rv = BCM_E_NONE;

    /* init locks */
    _rlink_travc_lock = sal_mutex_create("bcmTRAVC");
    if (_rlink_travc_lock == NULL) {
        rv = BCM_E_RESOURCE;
    }

    return rv;
}

int
bcm_rlink_traverse_init(void)
{
    int rv;

    rv = bcm_rlink_traverse_client_init();
    if (BCM_SUCCESS(rv)) {
        rv = bcm_rlink_traverse_server_init();
        if (BCM_FAILURE(rv)) {
            (void)bcm_rlink_traverse_client_deinit();
        }
    }

    return rv;
}


/*
  Deinitialize RLINK traverse subsystem
 */
int
bcm_rlink_traverse_server_deinit(void)
{
#if RLINK_TRAV_THREADED_SERVER
    /* stop traverse thread */
    if (_rlink_travs_thread != SAL_THREAD_ERROR) {
        _rlink_travs_thread_exit = TRUE;
        sal_sem_give(_rlink_travt_sem);
        sal_thread_yield();
        while (_rlink_travs_thread != SAL_THREAD_ERROR) {
            sal_sem_give(_rlink_travt_sem);
            sal_usleep(10000);
        }
        _rlink_travs_thread_exit = FALSE;
    }
    /* free locks */
    if (_rlink_travs_sem) {
    	sal_sem_destroy(_rlink_travs_sem);
        _rlink_travs_sem = NULL;
    }
    if (_rlink_travt_sem) {
    	sal_sem_destroy(_rlink_travt_sem);
        _rlink_travt_sem = NULL;
    }
    if (_rlink_travs_lock) {
        /* Deal with any outstanding server requests */
        bcm_rlink_traverse_server_clear();
    	sal_mutex_destroy(_rlink_travs_lock);
        _rlink_travs_lock = NULL;
    }
#else
    if (_rlink_trav_buffer) {
        sal_free(_rlink_trav_buffer);
        _rlink_trav_buffer = NULL;
    }
    if (_rlink_trav_info) {
        sal_free(_rlink_trav_info);
        _rlink_trav_info = NULL;
    }
#endif /* RLINK_TRAV_THREADED_SERVER */

    return BCM_E_NONE;
}

int
bcm_rlink_traverse_client_deinit(void)
{
    if (_rlink_travc_lock) {
        /* Deal with any outstanding client requests */
        bcm_rlink_traverse_client_clear();
    	sal_mutex_destroy(_rlink_travc_lock);
        _rlink_travc_lock = NULL;
    }

    return BCM_E_NONE;
}



int
bcm_rlink_traverse_deinit(void)
{
    (void)bcm_rlink_traverse_client_deinit();
    (void)bcm_rlink_traverse_server_deinit();
    return BCM_E_NONE;
}

#endif /* BCM_RPC_SUPPORT */
