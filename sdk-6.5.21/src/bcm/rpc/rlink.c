/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Remote asynchronous operations including linkscan, L2 address
 * change notification and RX tunnelling.  Only provides connection
 * service for tunnelling.  The tunnelled packet handler is registered
 * directly with RX and doesn't go through RLink notify.
 *
 * The client side:
 *	- remembers the application callbacks
 *	- sends an RPC to the server side indicating that registration has
 *	  happended
 *	- registers with ATP to receive remote link notifications
 *	- For linkscanning, when an ATP packet is received, does a
 *        bcm_port_info_get to the remote side and then calls the
 *        application callback
 *
 * The server side:
 *	- receives the registration RPC from the client
 *	- remembers which units have been registered
 *	- Registers with the proper local function
 *        (eg, bcm_linkscan_register) on the local unit
 *	- when a local callback happens, sends an ATP back to any
 *	  registered clients
 *
 * There is a thread that's started on both client and server sides to
 * handle the RPCs and packet handling without deadlock.  A single thread
 * handles both client and server side.
 *
 * rlink was originally designed for a master-slave architecture.
 * There are two master change situations that may occur:  The stack
 * changes so that the master is still present but is no longer the
 * master.  For this case, there are two sub cases:  The slave remains
 * a slave to the new master, or it becomes the master itself.
 *
 * The second scenario is that the master disappears from the stack.
 * This has the same two subcases as above.
 *
 * In the second case, we might get a "de-registration" call from the
 * old master when it detaches.  But if communication is already torn
 * down, we may not; and in the second case, we definitely won't get
 * a teardown message.
 *
 * Because of these situations, rlink now will overwrite information if
 * a new registration appears from a different requesting CPU.  This
 * has two implications.
 *
 * First, rlink will not support peer-to-peer connections.  Some L2
 * synchronization code may be peer-to-peer; either it won't use rlink,
 * or rlink will change.
 *
 * Second, this may open a security hole since a rogue might be able
 * to overwrite the current info.  rlink expects a layer of security
 * above its calls.
 */

#include <shared/bsl.h>

#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/rx.h>
#include <bcm/auth.h>

#include <soc/property.h>
#include <soc/drv.h>

#include <bcm_int/control.h>
#include <bcm_int/rpc/pack.h>
#include <bcm_int/rpc/rlink.h>
#include <bcm_int/esw/rx.h>

#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/atp.h>
#include <appl/cputrans/ct_tun.h>
#include "rlink.h"
#include "traverse.h"

#ifdef	BCM_RPC_SUPPORT

#ifndef RLINK_THREAD_STACK
#define	RLINK_THREAD_STACK	SAL_THREAD_STKSZ
#endif

#ifndef RLINK_THREAD_PRIO
#define	RLINK_THREAD_PRIO	50
#endif

#define	RLINK_INIT	(_rlink_lock != NULL)
#define	RLINK_SLEEP	sal_sem_take(_rlink_sem, sal_sem_FOREVER)
#define	RLINK_WAKE	sal_sem_give(_rlink_sem)
#define	RLINK_LOCK	sal_mutex_take(_rlink_lock, sal_mutex_FOREVER)
#define	RLINK_UNLOCK	sal_mutex_give(_rlink_lock)
#define	RLINK_NOTIFY_LOCK	\
			sal_mutex_take(_rlink_notify_lock, sal_mutex_FOREVER)
#define	RLINK_NOTIFY_UNLOCK	\
			sal_mutex_give(_rlink_notify_lock)

#define RLINK_ENQUEUE_NOTIFY(_rnot) do {                        \
        _rnot->next = NULL;                                     \
        RLINK_NOTIFY_LOCK;                                      \
        if (_rlink_notify_tail == NULL) {                       \
            _rlink_notify_head = _rlink_notify_tail = _rnot;    \
        } else {                                                \
            _rlink_notify_tail->next = _rnot;                   \
            _rlink_notify_tail = _rnot;                         \
        }                                                       \
        ++_rlink_queue_size;                                    \
        RLINK_NOTIFY_UNLOCK;                                    \
        RLINK_WAKE;                                             \
    } while (0)

#define MSG_STR(_m) \
    (((_m) == RLINK_MSG_ADD) ? "add" : \
     ((_m) == RLINK_MSG_DELETE) ? "delete" : \
     ((_m) == RLINK_MSG_NOTIFY) ? "notify" : \
     ((_m) == RLINK_MSG_TRAVERSE) ? "traverse" : \
     "?")

#define TYPE_STR(_t) \
    (((_t) == RLINK_TYPE_LINK) ? "link" : \
     ((_t) == RLINK_TYPE_AUTH) ? "auth" : \
     ((_t) == RLINK_TYPE_L2) ? "l2" : \
     ((_t) == RLINK_TYPE_RX) ? "rx" : \
     ((_t) == RLINK_TYPE_OAM) ? "oam" : \
     ((_t) == RLINK_TYPE_BFD) ? "bfd" : \
     ((_t) == RLINK_TYPE_ADD) ? "add" : \
     ((_t) == RLINK_TYPE_DELETE) ? "delete" : \
     ((_t) == RLINK_TYPE_START) ? "start" : \
     ((_t) == RLINK_TYPE_NEXT) ? "next" : \
     ((_t) == RLINK_TYPE_QUIT) ? "quit" : \
     ((_t) == RLINK_TYPE_ERROR) ? "error" : \
     ((_t) == RLINK_TYPE_MORE) ? "more" : \
     ((_t) == RLINK_TYPE_DONE) ? "done" : \
	"?")

/* Arguments for registration routines */
typedef struct _rlink_register_args_s {
    union {
        struct {
            bcm_oam_event_types_t events;
        } oam;
#if defined(INCLUDE_BFD)
        struct {
            bcm_bfd_event_types_t events;
        } bfd;
#endif
    } u;
} _rlink_register_args_t;

/* client side link handlers */
typedef struct _rlink_handle_s {
    struct _rlink_handle_s	*next;
    int				unit;
    rlink_type_t		type;
    union {
        void                    *generic_func_ptr;
        bcm_linkscan_handler_t  link;
        bcm_auth_cb_t           auth;
        bcm_l2_addr_callback_t  l2;
        /* No function registration is needed for RX tunnelling since
         * applications use bcm_rx_register directly. */
        bcm_oam_event_cb        oam;
#if defined(INCLUDE_BFD)
        bcm_bfd_event_cb        bfd;
#endif
    } func;
    void			*cookie;
    cpudb_key_t			cpu;
    int				cunit;
} _rlink_handle_t;

/* server side link handlers */
typedef struct _rlink_scan_s {
    struct _rlink_scan_s	*next;
    int				unit;
    rlink_type_t		type;
    cpudb_key_t			cpu;
} _rlink_scan_t;

/* client side notifications (also used for server side add/delete/send) */
typedef struct _rlink_notify_s {
    struct _rlink_notify_s	*next;
    cpudb_key_t			cpu;
    int				unit;
    rlink_type_t		type;
    int				pkt_send;  /* Boolean: Send notify pkt? */
    union {
        struct {   /* For client notification ops */
            uint8               *buf;
            int                 len;
            int                 check_cpu;
            uint32              ct_flags;
        } cli;
        struct {
            bcm_port_t          port;
            bcm_port_info_t     info;
        } link;
        struct {
            bcm_port_t          port;
            int                 reason;
        } auth;
        struct {
            int                 insert;
            bcm_l2_addr_t       addr;
        } l2;
        struct {
            uint32               flags;
            bcm_oam_event_type_t event_type;
            bcm_oam_group_t      group;
            bcm_oam_endpoint_t   endpoint;
        } oam;
#if defined(INCLUDE_BFD)
        struct {
            uint32               flags;
            bcm_bfd_event_types_t event_types;
            bcm_bfd_endpoint_t   endpoint;
        } bfd;
#endif
        struct {
            rlink_type_t	type;
        } msg;
        struct {
            bcm_pkt_t           *pkt;
            uint8               *buf;
            int                 len;
        } traverse;
    } u;
    _rlink_register_args_t    register_args;
} _rlink_notify_t;

static sal_mutex_t		_rlink_lock;
static sal_mutex_t		_rlink_notify_lock;
static sal_sem_t		_rlink_sem;
static sal_thread_t		_rlink_thread = SAL_THREAD_ERROR;
static int			_rlink_thread_exit;

/*
 * Example RLINK call sequence:
 *
 * CLIENT CPU:
 * App -> linkscan_register -> client_linkscan_register
 *      RLINK adds entry to _handle_ list.
 *      Makes remote MSG_ADD call to remote CPU controlling unit
 *      ===> to remote CPU
 *           -> _rlink_pkt_handler ->_rlink_scan_add
 *           RLINK adds entry to _scan_ list.
 * Later, a link event comes in on the remote system
 *      -> _bcm_rlink_linkscan_handler on REMOTE system
 *      Enqueues entry on _notify_ list
 *      -> rlink_thread runs through _scan_ list and
 *      sends MSG_NOTIFY to CLIENT CPUs.
 *          CLIENT CPU enqueues request on its _notify_ list.
 *          CLIENT RLINK thread dequeues notify event,
 *          runs through _handle_ list and makes callbacks.
 *
 * The length of the _notify_ list may be limited to avoid
 * overload due to RX or L2 notifies.
 */

static _rlink_handle_t		*_rlink_handle_head;
static _rlink_handle_t		*_rlink_handle_tail;
static _rlink_scan_t		*_rlink_scan_head;
static _rlink_scan_t		*_rlink_scan_tail;
static _rlink_notify_t		*_rlink_notify_head;
static _rlink_notify_t		*_rlink_notify_tail;

/* Some counters */
#if defined(BROADCOM_DEBUG)
#define INCR_COUNTER(counter) ++(counter)
volatile int rlink_link_out;
volatile int rlink_link_out_disc;
volatile int rlink_link_in;
volatile int rlink_link_in_disc;
volatile int rlink_auth_out;
volatile int rlink_auth_out_disc;
volatile int rlink_auth_in;
volatile int rlink_auth_in_disc;
volatile int rlink_l2_out;
volatile int rlink_l2_out_disc;
volatile int rlink_l2_in;
volatile int rlink_l2_in_disc;
volatile int rlink_rx_out;
volatile int rlink_rx_out_disc;
volatile int rlink_rx_in;
volatile int rlink_rx_in_disc;
volatile int rlink_oam_out;
volatile int rlink_oam_out_disc;
volatile int rlink_oam_in;
volatile int rlink_oam_in_disc;
volatile int rlink_bfd_out;
volatile int rlink_bfd_out_disc;
volatile int rlink_bfd_in;
volatile int rlink_bfd_in_disc;

volatile int rlink_add_req_in;
volatile int rlink_del_req_in;
volatile int rlink_notify_in;
volatile int rlink_trav_req_in;
volatile int rlink_buff_alloc_fail;
volatile int rlink_pkt_send_fail;
#else
#define INCR_COUNTER(counter)
#endif /* BROADCOM_DEBUG */

/* The rlink queue length may be limited */
static int                      _rlink_queue_size;

/*
 * Since notify requests are now queued, rather than sent in
 * the calling thread context, a queue size limiting mechanism
 * is implemented.
 *
 * If foo_max > 0 and queue_size > foo_max, then foo requests are
 * not enqueued (but without notice).  This applies to L2 notification,
 * link notification and RX tunnels.  For RX, the max is checked on a
 * per-COS basis.
 *
 * In addition, the local and remote operations are checked independently,
 * although local RX requests go directly to RX which does its own
 * rate limiting.
 *
 * It is recommended that link notifications not be limited.
 *
 * Currently, these are exported to allow applications to set them
 * directly.  In the future, accessors or "bcm_rlink_control" may be
 * implemented depending on scalability requirements.
 */

int bcm_rlink_link_remote_max;
int bcm_rlink_auth_remote_max;
int bcm_rlink_l2_remote_max = BCM_RLINK_L2_REMOTE_MAX_DEFAULT;
int bcm_rlink_rx_remote_max[BCM_COS_COUNT] = BCM_RLINK_RX_REMOTE_MAX_DEFAULT;
int bcm_rlink_oam_remote_max;
int bcm_rlink_bfd_remote_max;

int bcm_rlink_link_local_max;  /* 0 -> no limit; recommended setting */
int bcm_rlink_auth_local_max;  /* 0 -> no limit; recommended setting */
int bcm_rlink_l2_local_max;
int bcm_rlink_oam_local_max;   /* 0 -> no limit; recommended setting */
int bcm_rlink_bfd_local_max;   /* 0 -> no limit; recommended setting */

int	_rlink_nexthop = 0;


/* Array size for bcm_oam_event_types_t bit declare type */
#define _BCM_OAM_EVENT_TYPES_ARRAY_SIZE    _SHR_BITDCLSIZE(bcmOAMEventCount)
#if defined(INCLUDE_BFD)
/* Array size for bcm_bfd_event_types_t bit declare type */
#define _BCM_BFD_EVENT_TYPES_ARRAY_SIZE    _SHR_BITDCLSIZE(bcmBFDEventCount)
#endif

STATIC void
_bcm_rlink_send_enqueue(int unit,
			rlink_type_t type,
			uint8 *buf,
			int len,
			int check_cpu,
			cpudb_key_t cpu,
			uint32 ct_flags)
{
    _rlink_notify_t     *rnot;

    rnot = sal_alloc(sizeof(*rnot), "bcm_rlink_notify");
    if (rnot == NULL) {
        return; /* Failure */
    }

    CPUDB_KEY_COPY(rnot->cpu, cpu);
    rnot->unit = unit;
    rnot->type = type;
    rnot->pkt_send = TRUE;
    rnot->u.cli.buf = buf;
    rnot->u.cli.len = len;
    rnot->u.cli.check_cpu = check_cpu;
    rnot->u.cli.ct_flags = ct_flags;

    RLINK_ENQUEUE_NOTIFY(rnot);
}


/*
 * Server side handlers.
 * Registered for any units of interest to requesting clients.
 * Send off a notify packet to any clients interested in this unit.
 */
#define cpudb_key_ignore cpudb_bcast_key

/*
 * Tail end of linkscan and l2 handlers.
 * Send notify to appropriate clients.
 */
STATIC void
_bcm_rlink_send_notify(int unit,
                       rlink_type_t type,
                       uint8 *buf,
                       int len,
                       int check_cpu,
                       cpudb_key_t cpu,
                       uint32 ct_flags)
{
    int rv = BCM_E_NONE;
    _rlink_scan_t	*rscan;

    LOG_VERBOSE(BSL_LS_BCM_RX,
                (BSL_META_U(unit,
                            "Sending RLINK %s notify for unit %d\n"),
                 TYPE_STR(type), unit));
    RLINK_LOCK;
    for (rscan = _rlink_scan_head; rscan != NULL; rscan = rscan->next) {
        if (rscan->unit == unit && rscan->type == type) {
            if (check_cpu) {
                if (!CPUDB_KEY_EQUAL(cpu, rscan->cpu)) {
                    continue;
                }
            }
            LOG_VERBOSE(BSL_LS_BCM_RX,
                        (BSL_META_U(unit,
                                    "RLINK to " CPUDB_KEY_FMT_EOLN),
                         CPUDB_KEY_DISP(rscan->cpu)));
            rv = atp_tx(rscan->cpu, RLINK_CLIENT_ID,
                            buf, len, ct_flags, NULL, NULL);
            if (BCM_FAILURE(rv)) {
                INCR_COUNTER(rlink_pkt_send_fail);
            }
        }
    }
    RLINK_UNLOCK;

    /* coverity[identical_branches] */
    if (type != RLINK_TYPE_RX) {
        atp_tx_data_free(buf);
    } else {   
        atp_tx_data_free(buf);
    }
}

/****************************************************************
 *
 * Callbacks registered at the BCM layer on local units on the
 * (rlink) server side.  These handle the async events on the
 * CPUs where they first occur.  They forward notification via
 * "send_notify" (above) when configuration is right for the
 * given event.
 */

/*
 * Registered with RX at server to check if pkts should be
 * tunnelled.
 */

STATIC bcm_rx_t
_bcm_rlink_rx_handler(int unit, bcm_pkt_t *pkt, void *cookie)
{
    uint32 ct_flags = 0;
    bcm_cpu_tunnel_mode_t tunnel_pkt;
    int no_ack;
    uint8 *buf, *bp;
    int len;
    cpudb_key_t cpu;
    int check_cpu;

    if (_rlink_thread_exit) { /* Not running */
        return BCM_RX_NOT_HANDLED;
    }

    /*
     * Find out if the packet should be tunnelled; if so, should
     * it be directed to one CPU?
     */
    tunnel_pkt = ct_rx_tunnel_check(unit, pkt, &no_ack, &check_cpu, &cpu);

    if (tunnel_pkt == BCM_CPU_TUNNEL_NONE) {  /* Don't tunnel */
        return BCM_RX_NOT_HANDLED;
    }

    INCR_COUNTER(rlink_rx_out);

    if (bcm_rlink_rx_remote_max[pkt->cos] > 0 &&
            _rlink_queue_size > bcm_rlink_rx_remote_max[pkt->cos]) {
        INCR_COUNTER(rlink_rx_out_disc);
        /* No space in queue */
        return BCM_RX_NOT_HANDLED;
    }

    CPUTRANS_COS_SET(ct_flags, pkt->cos);
    if (no_ack) {   /* Mark packet as tunnelled via best effort */
        ct_flags |= CPUTRANS_NO_ACK;
    }

    /* msg, type, unit, port, info */
    len = 1 + 1 + 4 + ct_rx_tunnel_header_bytes_get() + pkt->pkt_len;

    
    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "RLINK Tunnel Src: alloc failed\n")));
        return BCM_RX_NOT_HANDLED;
    }

    bp = bcm_rlink_encode(buf, RLINK_MSG_NOTIFY, RLINK_TYPE_RX, unit);
    (void)ct_rx_tunnel_pkt_pack(pkt, bp);

    _bcm_rlink_send_enqueue(unit, RLINK_TYPE_RX, buf, len, check_cpu,
                            cpu, ct_flags);

    return BCM_RX_HANDLED;
}


/*
 * Server side linkscan handler
 */
STATIC void
_bcm_rlink_linkscan_handler(int unit,
			    bcm_port_t port,
			    bcm_port_info_t *info)
{
    uint8	*buf, *bp;
    int		len;

    if (_rlink_thread_exit) { /* Not running */
        return;
    }

    INCR_COUNTER(rlink_link_out);

    if (bcm_rlink_link_remote_max > 0 &&
            _rlink_queue_size > bcm_rlink_link_remote_max) {
        INCR_COUNTER(rlink_link_out_disc);
        /* No space in queue */
        return;
    }

    /* msg, type, unit, port, info */
    len = 1 + 1 + 4 + 4 + BCM_PACKLEN_PORT_INFO;
    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        return;
    }
    bp = bcm_rlink_encode(buf, RLINK_MSG_NOTIFY, RLINK_TYPE_LINK, unit);
    BCM_PACK_U32(bp, port);
    (void) _bcm_pack_port_info(bp, info);

    _bcm_rlink_send_enqueue(unit, RLINK_TYPE_LINK, buf, len, FALSE,
                            cpudb_key_ignore, 0);
}

/*
 * Server side linkscan handler
 */
STATIC void
_bcm_rlink_auth_handler(void *cookie,
                        int unit,
                        int port,
                        int reason)
{
    uint8	*buf, *bp;
    int		len;

    if (_rlink_thread_exit) { /* Not running */
        return;
    }

    INCR_COUNTER(rlink_auth_out);

    if (bcm_rlink_auth_remote_max > 0 &&
            _rlink_queue_size > bcm_rlink_auth_remote_max) {
        INCR_COUNTER(rlink_auth_out_disc);
        /* No space in queue */
        return;
    }

    /* msg, type, unit, port, reason */
    len = 1 + 1 + 4 + 4 + 4;
    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        return;
    }
    bp = bcm_rlink_encode(buf, RLINK_MSG_NOTIFY, RLINK_TYPE_AUTH, unit);
    BCM_PACK_U32(bp, port);
    BCM_PACK_U32(bp, reason);

    _bcm_rlink_send_enqueue(unit, RLINK_TYPE_AUTH, buf, len, FALSE,
                            cpudb_key_ignore, 0);
}


/*
 * Server side l2 handler.  Not all L2 events should be sent to
 * clients.  This is determined by a callback that can be
 * registered here.
 *
 * In addition, the most important criteria is whether the L2
 * address is "native" to the unit (learned on a front panel
 * port).  This can be set up directly using the calls below.
 */

static bcm_rlink_l2_cb_f _l2_cb = NULL;
static uint32 _l2_native_only = FALSE;

void
bcm_rlink_l2_cb_set(bcm_rlink_l2_cb_f l2_cb)
{
    _l2_cb = l2_cb;
}

void
bcm_rlink_l2_cb_get(bcm_rlink_l2_cb_f *l2_cb)
{
    *l2_cb = _l2_cb;
}

void
bcm_rlink_l2_native_only_set(int native_only)
{
    _l2_native_only = native_only;
}

void
bcm_rlink_l2_native_only_get(int *native_only)
{
    *native_only = _l2_native_only;
}

STATIC void
_bcm_rlink_l2_handler(int unit,
		      bcm_l2_addr_t *l2addr,
		      int insert,
		      void *cookie)
{
    uint8		*buf, *bp;
    int			len;

    if (_rlink_thread_exit) { /* Not running */
        return;
    }

    /* See if callback present; if it is and returns FALSE, return */
    if (_l2_cb != NULL) {
        if (_l2_cb(unit, l2addr, insert) == FALSE) {
            return;
        }
    }

    /* Check for native only setting */
    if (_l2_native_only == TRUE) {
        if (!(l2addr->flags & BCM_L2_NATIVE)) {
            return;
        }
    }

    INCR_COUNTER(rlink_l2_out);

    if (bcm_rlink_l2_remote_max > 0 &&
            _rlink_queue_size > bcm_rlink_l2_remote_max) {
        INCR_COUNTER(rlink_l2_out_disc);
        /* No space in queue */
        return;
    }

    /* msg, type, unit, insert, l2addr */
    len = 1 + 1 + 4 + 1 + BCM_PACKLEN_L2_ADDR;
    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        return;
    }
    bp = bcm_rlink_encode(buf, RLINK_MSG_NOTIFY, RLINK_TYPE_L2, unit);
    BCM_PACK_U8(bp, insert ? 1 : 0);
    (void) _bcm_pack_l2_addr(bp, l2addr);

    _bcm_rlink_send_enqueue(unit, RLINK_TYPE_L2, buf, len, FALSE,
                            cpudb_key_ignore, 0);
}

/*
 * Server side oam event handler
 */
STATIC int
_bcm_rlink_oam_handler(int unit, 
                       uint32 flags, 
                       bcm_oam_event_type_t event_type, 
                       bcm_oam_group_t group, 
                       bcm_oam_endpoint_t endpoint, 
                       void *cookie)
{
    uint8	*buf, *bp;
    int		len;

    if (_rlink_thread_exit) { /* Not running */
        return BCM_E_FAIL;
    }

    INCR_COUNTER(rlink_oam_out);

    if (bcm_rlink_oam_remote_max > 0 &&
        _rlink_queue_size > bcm_rlink_oam_remote_max) {
        INCR_COUNTER(rlink_oam_out_disc);
        /* No space in queue */
        return BCM_E_FULL;
    }

    /* msg, type, unit, flags, event_type, group, endpoint */
    len = 1 + 1 + 4 + 4 + 4 + 4 + 4;
    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        return BCM_E_MEMORY;
    }
    bp = bcm_rlink_encode(buf, RLINK_MSG_NOTIFY, RLINK_TYPE_OAM, unit);
    BCM_PACK_U32(bp, flags);
    BCM_PACK_U32(bp, event_type);
    BCM_PACK_U32(bp, group);
    BCM_PACK_U32(bp, endpoint);

    _bcm_rlink_send_enqueue(unit, RLINK_TYPE_OAM, buf, len, FALSE,
                            cpudb_key_ignore, 0);
    return BCM_E_NONE;
}

#if defined(INCLUDE_BFD)
/*
 * Server side bfd event handler
 */
STATIC int
_bcm_rlink_bfd_handler(int unit, 
                       uint32 flags, 
                       bcm_bfd_event_types_t event_types, 
                       bcm_bfd_endpoint_t endpoint, 
                       void *cookie)
{
    uint8	*buf, *bp;
    int		len, i;

    if (_rlink_thread_exit) { /* Not running */
        return BCM_E_FAIL;
    }

    INCR_COUNTER(rlink_bfd_out);

    if (bcm_rlink_bfd_remote_max > 0 &&
        _rlink_queue_size > bcm_rlink_bfd_remote_max) {
        INCR_COUNTER(rlink_bfd_out_disc);
        /* No space in queue */
        return BCM_E_FULL;
    }

    /* msg, type, unit, flags, group */
    len = 1 + 1 + 4 + 4 + 4;
    /* events */
    for (i = 0; i < _BCM_BFD_EVENT_TYPES_ARRAY_SIZE; i++) {
        len += 4;
    }
    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        return BCM_E_MEMORY;
    }
    bp = bcm_rlink_encode(buf, RLINK_MSG_NOTIFY, RLINK_TYPE_BFD, unit);
    BCM_PACK_U32(bp, flags);
    for (i = 0; i < _BCM_BFD_EVENT_TYPES_ARRAY_SIZE; i++) {
        BCM_PACK_U32(bp, event_types.w[i]);
    }
    BCM_PACK_U32(bp, endpoint);

    _bcm_rlink_send_enqueue(unit, RLINK_TYPE_BFD, buf, len, FALSE,
                            cpudb_key_ignore, 0);
    return BCM_E_NONE;
}
#endif

/****************************************************************
 *
 * Server side registration control.
 * Invoked when a client CPU sends an "add" or "delete" request.
 */

STATIC void
_bcm_rlink_scan_add(cpudb_key_t cpu, int unit, rlink_type_t type,
                    _rlink_register_args_t *args)
{
    _rlink_scan_t	*rscan;
    int                 present;

    if(!BCM_CONTROL_UNIT_LEGAL(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "RLINK unit is out of range\n")));
        return;
    }

    /* For RX in particular, unit must be local */
    if (!BCM_IS_LOCAL(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "RLINK scan add on non-local unit %d, "
                             "type %s\n"), unit, TYPE_STR(type)));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "RLINK add on unit %d, type %s for CPU "
                            CPUDB_KEY_FMT_EOLN), unit, TYPE_STR(type),
                 CPUDB_KEY_DISP(cpu)));

    /* If (cpu, unit, type) is in scan chain, don't add again */

    RLINK_LOCK;
    present = FALSE;
    for (rscan = _rlink_scan_head; rscan != NULL; rscan = rscan->next) {
	if (type == rscan->type && unit == rscan->unit &&
               CPUDB_KEY_EQUAL(cpu, rscan->cpu)) {
            present = TRUE;
	    break;
	}
    }

    if (!present) { /* Okay, add entry */
        rscan = sal_alloc(sizeof(*rscan), "bcm_rlink_scan");
        if (rscan == NULL) {
            RLINK_UNLOCK;
            return;		/* failure */
        }
        CPUDB_KEY_COPY(rscan->cpu, cpu);
        rscan->unit = unit;
        rscan->type = type;
        rscan->next = NULL;

        if (_rlink_scan_tail == NULL) {
            _rlink_scan_head = _rlink_scan_tail = rscan;
        } else {
            _rlink_scan_tail->next = rscan;
            _rlink_scan_tail = rscan;
        }
    }
    RLINK_UNLOCK;

    /* always attempt to re-register to lower level */
    switch (type) {
    case RLINK_TYPE_LINK:
        (void)bcm_linkscan_register(unit,
                                    _bcm_rlink_linkscan_handler);
        break;
    case RLINK_TYPE_AUTH:
        (void)bcm_auth_unauth_callback(unit,
                                       _bcm_rlink_auth_handler, NULL);
        break;
    case RLINK_TYPE_L2:
        (void)bcm_l2_addr_register(unit,
                                   _bcm_rlink_l2_handler, NULL);
        break;
    case RLINK_TYPE_RX:
        (void)bcm_rx_queue_register(unit, "rlink",
                                    BCM_RX_COS_ALL,
                                    _bcm_rlink_rx_handler,
                                    ct_rx_tunnel_priority_get(), NULL,
                                    BCM_RCO_F_ALL_COS);

        break;
    case RLINK_TYPE_OAM:
        (void)bcm_oam_event_register(unit, 
                                     args->u.oam.events,
                                     _bcm_rlink_oam_handler,
                                     NULL);
        break;
    case RLINK_TYPE_BFD:
#if defined(INCLUDE_BFD)
        (void)bcm_bfd_event_register(unit, 
                                     args->u.bfd.events,
                                     _bcm_rlink_bfd_handler,
                                     NULL);
#endif
        break;
    default:
	break;
    }
}


STATIC void
_bcm_rlink_scan_delete(cpudb_key_t cpu, int unit, rlink_type_t type,
                       _rlink_register_args_t *args)
{
    _rlink_scan_t	*rscan, *pscan;
    int			last;

    if(!BCM_CONTROL_UNIT_LEGAL(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "RLINK unit is out of range\n")));
        return;
    }

    if (!BCM_IS_LOCAL(unit)) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "RLINK scan delete on non-local unit %d, "
                             "type %s\n"), unit, TYPE_STR(type)));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "RLINK scan delete on unit %d, type %s for CPU "
                            CPUDB_KEY_FMT_EOLN), unit, TYPE_STR(type),
                 CPUDB_KEY_DISP(cpu)));

    RLINK_LOCK;
    pscan = NULL;
    for (rscan = _rlink_scan_head; rscan != NULL;
	 pscan = rscan, rscan = rscan->next) {
	if (rscan->type == type && rscan->unit == unit &&
	    CPUDB_KEY_EQUAL(rscan->cpu, cpu)) {
	    if (pscan == NULL) {
		_rlink_scan_head = rscan->next;
	    } else {
		pscan->next = rscan->next;
	    }
	    if (rscan == _rlink_scan_tail) {
		_rlink_scan_tail = pscan;
	    }
	    break;
	}
    }

    if (rscan == NULL) {
	RLINK_UNLOCK;
	return;			/* not found */
    }

    /* Check if no more entries of this type, on this unit */
    last = TRUE;
    for (pscan = _rlink_scan_head; pscan != NULL; pscan = pscan->next) {
	if (type == pscan->type && unit == pscan->unit) {
	    last = FALSE;
	    break;
	}
    }

    RLINK_UNLOCK;

    sal_free(rscan);

    if (last) {
	switch (type) {
	case RLINK_TYPE_LINK:
	    (void)bcm_linkscan_unregister(unit,
					  _bcm_rlink_linkscan_handler);
	    break;
	case RLINK_TYPE_AUTH: /* Clear callback with NULL as function */
	    (void)bcm_auth_unauth_callback(unit, NULL, NULL);
	    break;
	case RLINK_TYPE_L2:
	    (void)bcm_l2_addr_unregister(unit,
					 _bcm_rlink_l2_handler, NULL);
	    break;
	case RLINK_TYPE_RX:
	    (void)bcm_rx_unregister(unit, _bcm_rlink_rx_handler,
				    ct_rx_tunnel_priority_get());
	    break;
	case RLINK_TYPE_OAM:
	    (void)bcm_oam_event_unregister(unit, 
                                       args->u.oam.events,
                                       _bcm_rlink_oam_handler);
	    break;
	case RLINK_TYPE_BFD:
#if defined(INCLUDE_BFD)
	    (void)bcm_bfd_event_unregister(unit, 
                                       args->u.bfd.events,
                                       _bcm_rlink_bfd_handler);
#endif
	    break;
	default:
	    break;
	}
    }
}

/*
 * Function:
 *      bcm_rlink_device_clear
 * Purpose:
 *      Clear the RLINK state for the given LOCAL unit (server clear).
 * Parameters:
 *      unit     - LOCAL unit number
 * Notes:
 *      Removes all RLINK refs for the given unit.  It might be
 *      better to do this on a per-CPU basis as well, but currently,
 *      only the master CPU ever gets referenced here.  In addition,
 *      this could probably be done once for all local units
 *      controlled, but this is more consistent with the rest of the API.
 *
 *      See rlink_device_detach for sending a cleanup call to a remote
 *      device.
 */

void
bcm_rlink_device_clear(int unit)
{
    _rlink_scan_t       *cur_entry, *next_entry, *prev_entry;
    bcm_oam_event_types_t oam_events;
#if defined(INCLUDE_BFD)
    bcm_bfd_event_types_t bfd_events;
#endif
    if (!RLINK_INIT) {
        return;
    }

    prev_entry = NULL;

    RLINK_LOCK;
    for (cur_entry = _rlink_scan_head; cur_entry != NULL;
         cur_entry = next_entry) {
        next_entry = cur_entry->next;

        if (cur_entry->unit == unit) {  /* Remove entry */
            if (prev_entry != NULL) {
                prev_entry->next = cur_entry->next;
            } else {
                _rlink_scan_head = cur_entry->next;
            }
            sal_free(cur_entry);

        } else {
            prev_entry = cur_entry;
        }
    }
    RLINK_UNLOCK;

    /* Set all bits to clear all oam events */
    sal_memset(&(oam_events), 1, sizeof(bcm_oam_event_types_t));
#if defined(INCLUDE_BFD)
    sal_memset(&(bfd_events), 1, sizeof(bcm_bfd_event_types_t));
#endif
    (void)bcm_linkscan_unregister(unit, _bcm_rlink_linkscan_handler);
    (void)bcm_auth_unauth_callback(unit, NULL, NULL);
    (void)bcm_l2_addr_unregister(unit, _bcm_rlink_l2_handler, NULL);
    (void)bcm_rx_unregister(unit, _bcm_rlink_rx_handler,
                            ct_rx_tunnel_priority_get());
    (void)bcm_oam_event_unregister(unit, oam_events,
                                   _bcm_rlink_oam_handler);
#if defined(INCLUDE_BFD)
    (void)bcm_bfd_event_unregister(unit, bfd_events,
                                   _bcm_rlink_bfd_handler);
#endif
}

/*
 * Function:
 *      bcm_rlink_server_clear
 * Purpose:
 *      Clear all RLINK server (local) information.
 * Notes:
 *      Forces de-registration of rlink callbacks on local units
 */

void
bcm_rlink_server_clear(void)
{
    _rlink_scan_t       *cur_entry, *next_entry;
    int			unit;
    bcm_oam_event_types_t oam_events;
#if defined(INCLUDE_BFD)
    bcm_bfd_event_types_t bfd_events;
#endif

    /* Set all bits to clear all oam events */
    sal_memset(&(oam_events), 1, sizeof(bcm_oam_event_types_t));
#if defined(INCLUDE_BFD)
    sal_memset(&(bfd_events), 1, sizeof(bcm_bfd_event_types_t));
#endif
    RLINK_LOCK;

    /* Clear server information */
    for (cur_entry = _rlink_scan_head; cur_entry != NULL;
             cur_entry = next_entry) {
        next_entry = cur_entry->next;
        sal_free(cur_entry);
    }

    _rlink_scan_head = _rlink_scan_tail = NULL;

    for (unit = 0; unit < BCM_MAX_NUM_UNITS; unit++) {
        (void)bcm_linkscan_unregister(unit, _bcm_rlink_linkscan_handler);
        (void)bcm_auth_unauth_callback(unit, NULL, NULL);
        (void)bcm_l2_addr_unregister(unit, _bcm_rlink_l2_handler, NULL);
        (void)bcm_rx_unregister(unit, _bcm_rlink_rx_handler,
                                ct_rx_tunnel_priority_get());
        (void)bcm_oam_event_unregister(unit, oam_events,
                                       _bcm_rlink_oam_handler);
#if defined(INCLUDE_BFD)
        (void)bcm_bfd_event_unregister(unit, bfd_events,
                                       _bcm_rlink_bfd_handler);
#endif
    }

    bcm_rlink_traverse_server_clear();    
    RLINK_UNLOCK;
}

/*
 * Function:
 *      bcm_rlink_client_clear
 * Purpose:
 *      Clear all client (remote related) information
 */

void
bcm_rlink_client_clear(void)
{
    _rlink_handle_t	*cur_hand, *next_hand;

    RLINK_LOCK;

    /* Clear client information */
    for (cur_hand = _rlink_handle_head; cur_hand != NULL;
             cur_hand = next_hand) {
        next_hand = cur_hand->next;
        sal_free(cur_hand);
    }

    _rlink_handle_head = _rlink_handle_tail = NULL;

    bcm_rlink_traverse_client_clear();    
    RLINK_UNLOCK;
}

/*
 * Function:
 *      bcm_rlink_clear
 * Purpose:
 *      Clear both client and server state
 */

void
bcm_rlink_clear(void)
{
    bcm_rlink_server_clear();
    bcm_rlink_client_clear();
}

/*
 * Client side packet send
 */
STATIC int
_bcm_rlink_send_control(cpudb_key_t cpu, int unit,
                        rlink_msg_t msg, rlink_type_t type,
                        _rlink_register_args_t *args)
{
    uint8	*buf, *bp;
    int		len;
    int     i;
    int     rv = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "RLINK ctl msg %s, cpu-unit %d, type %s to "
                            CPUDB_KEY_FMT_EOLN), MSG_STR(msg), unit, TYPE_STR(type),
                 CPUDB_KEY_DISP(cpu)));
    len = 1 + 1 + 4;	/* msg, type, unit */
    /* Other arguments for register function */
    if (args) {
        switch (type) {
        case RLINK_TYPE_OAM:    /* event_types */
            for (i = 0; i < _BCM_OAM_EVENT_TYPES_ARRAY_SIZE; i++) {
                len += 4;
            }
            break;
        case RLINK_TYPE_BFD:    /* event_types */
#if defined(INCLUDE_BFD)
            for (i = 0; i < _BCM_BFD_EVENT_TYPES_ARRAY_SIZE; i++) {
                len += 4;
            }
#endif
            break;
        default:
            break;
        }
    }

    buf = atp_tx_data_alloc(len);
    if (buf == NULL) {
        INCR_COUNTER(rlink_buff_alloc_fail);
        return BCM_E_MEMORY;
    }
    bp = bcm_rlink_encode(buf, msg, type, unit);

    /* Other arguments for register function */
    if (args) {
        switch (type) {
        case RLINK_TYPE_OAM:
            for (i = 0; i < _BCM_OAM_EVENT_TYPES_ARRAY_SIZE; i++) {
                BCM_PACK_U32(bp, args->u.oam.events.w[i]);
            }
            break;
        case RLINK_TYPE_BFD:
#if defined(INCLUDE_BFD)
            for (i = 0; i < _BCM_BFD_EVENT_TYPES_ARRAY_SIZE; i++) {
                BCM_PACK_U32(bp, args->u.bfd.events.w[i]);
            }
#endif
            break;
        default:
            break;
        }
    }

    rv = atp_tx(cpu, RLINK_CLIENT_ID, buf, len, 0, NULL, NULL);
    if (BCM_FAILURE(rv)) {
        INCR_COUNTER(rlink_pkt_send_fail);
    }
    atp_tx_data_free(buf);
    return rv;
}

/*
 * Function:
 *  _bcm_rlink_handle_list_add
 *
 * Purpose:
 *     Internal function to add data to list _rlink_handle_head/tail.
 *     Data would only be added if it is already not exising in the
 *     list. Fields 'unit', 'type' and 'function' from the parameter
 *     'data_node' would be used as keys to search the list.
 *
 * Parameters:
 *     data_node - (IN) Node with data to be added.
 *
 * Returns:
 *     BCM_E_NONE - Successfully added the data to the list
 *     BCM_E_EXISTS - Data not added as it already exists in the list
 *     BCM_E_MEMORY - Data not added as memory Allocation failed
 *
 * Note:
 */
STATIC int
_bcm_rlink_handle_list_add(_rlink_handle_t *data_node) {

    int rv = BCM_E_NONE;
    _rlink_handle_t *rhand = NULL;

    /* Lock the list */
    RLINK_LOCK;

    /* See if already registered */
    for (rhand = _rlink_handle_head; rhand != NULL; rhand = rhand->next) {
        if (rhand->type == data_node->type &&
            rhand->unit == data_node->unit &&
            rhand->func.generic_func_ptr == data_node->func.generic_func_ptr) {
                RLINK_UNLOCK;
                return BCM_E_EXISTS;
        }
    }

    /* Allocate memory for the node */
    rhand = sal_alloc(sizeof(*rhand), "bcm_rlink_handle");
    if (rhand == NULL) {
        RLINK_UNLOCK;
        return BCM_E_MEMORY;
    }

    /* Fill in the data */
    rhand->unit = data_node->unit;
    rhand->type = data_node->type;
    rhand->func.generic_func_ptr = data_node->func.generic_func_ptr;
    rhand->cookie = data_node->cookie;
    CPUDB_KEY_COPY(rhand->cpu, data_node->cpu);
    rhand->cunit = data_node->cunit;
    rhand->next = NULL;

    /* Call routine to add it to the list */
    if (_rlink_handle_tail == NULL) {
        _rlink_handle_head = _rlink_handle_tail = rhand;
    } else {
        _rlink_handle_tail->next = rhand;
        _rlink_handle_tail = rhand;
    }

    /* Unlock the list */
    RLINK_UNLOCK;

    return rv;
}

/*
 * Function:
 *  _bcm_rlink_handle_list_delete
 *
 * Purpose:
 *     Internal function to delete data from list _rlink_handle_head/tail.
 *     Data would only be deleted if it  exists in the list. Fields 'unit'
 *     , 'type' and 'function' in parameter data_node would be used as keys
 *     to search the list for pre-existence check.
 *
 * Parameters:
 *     data_node - (IN/OUT) Node with data to be deleted. It acts in INPUT for
 *                  fields unit, type, and OUTPUT for fields cookie, cpukey,
 *                  cunit
 *     last - (OUT) Indicates if the deleted node was the last of its kind
 *
 * Returns:
 *     BCM_E_NONE - Successfully deleted the data from the list
 *     BCM_E_NOT_FOUND - Data did not delete as it does not exist
 *
 * Note:
 *     1) Matching Node will be removed from the list and its memory deallocated
 *     2) Fields cookie, cpu and cunit in the given node are populated before
 *         the node is de-allocated.
 */
STATIC int
_bcm_rlink_handle_list_delete(_rlink_handle_t *data_node, int *last) {

    int rv = BCM_E_NONE;
    _rlink_handle_t *rhand = NULL, *phand = NULL;

    /* Lock the list */
    RLINK_LOCK;

    for (rhand = _rlink_handle_head;
         rhand != NULL;
         phand = rhand, rhand = rhand->next) {
            if (rhand->type == data_node->type &&
              rhand->unit == data_node->unit &&
              rhand->func.generic_func_ptr == data_node->func.generic_func_ptr)
            {
                if (phand == NULL) {
                    _rlink_handle_head = rhand->next;
                } else {
                    phand->next = rhand->next;
                }
                if (rhand == _rlink_handle_tail) {
                    _rlink_handle_tail = phand;
                }
                rhand->next = NULL;
                break;
            }
    }

    if (NULL == rhand) {
        /* Since entry not found, mark it so */
        rv = BCM_E_NOT_FOUND;
    } else {
        /* Save cookie, cpu_key and cunit infortmation */
        CPUDB_KEY_COPY(data_node->cpu, rhand->cpu);
        data_node->cunit = rhand->cunit;
        data_node->cookie = rhand->cookie;

        /* De-allocate the node memory */
        sal_free(rhand);

        /* See if this was the last entry for this unit */
        if (NULL != last) {
            *last = 1; /* Mark it to be the last at first */

            for (phand = _rlink_handle_head; phand != NULL; phand = phand->next)
            {
                if (phand->type == data_node->type &&
                    phand->unit == data_node->unit) {
                    *last = 0;
                    break;
                }
            }
        }
    }

    /* Unlock the list */
    RLINK_UNLOCK;

    return rv;
}

/*
 * Function:
 *      _bcm_rlink_register
 *
 * Purpose:
 *      Generic function to register a rlink callback.
 *      Initially the callback data is added to a client rlink list pointed
 *      by '_rlink_handle_head'. Next the remote device is sent a RLINK_MSG
 *      with type ADD. If the MSG returns back with an error or the send
 *      itself failed then the previous step of adding the data to the list
 *      is reverted by deleting it back from the list.
 *
 * Parameters:
 *      node - (IN/OUT) List node.
 *      args - (IN) register event, if any
 *
 * Returns:
 *      BCM_E_NONE - Successful registration
 *      BCM_E_XXX - Other failures
 */
STATIC int
_bcm_rlink_register(_rlink_handle_t *node, _rlink_register_args_t *args)
{
    int is_new_data = 1;
    int rv = BCM_E_NONE;

    if (!RLINK_INIT) {
        return BCM_E_UNAVAIL;
    }

    /* Deduce and store CPUDB_KEY and CUNIT */
    CPUDB_KEY_COPY(node->cpu,
                   *((cpudb_key_t *)BCM_CONTROL(node->unit)->drv_control));
    node->cunit = BCM_CONTROL(node->unit)->unit;

    /* Call internal routine to add the data to the list */
    rv = _bcm_rlink_handle_list_add(node);
    if (BCM_E_EXISTS == rv) {
        is_new_data = 0;
    } else if(BCM_FAILURE(rv)) {
        return rv;
    }

    /* always send an ADD, just in case */
    rv = _bcm_rlink_send_control(
            node->cpu, node->cunit, RLINK_MSG_ADD, node->type, args);

    /* Perform graceful error handling */
    if (BCM_FAILURE(rv) && (1 == is_new_data)) {
        /* Delete the just added entry from the list */
        (void)_bcm_rlink_handle_list_delete(node, NULL);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_rlink_unregister
 *
 * Purpose:
 *      Generic function to deregister a rlink callback.
 *      Initially the callback data is deleted from the client rlink list
 *      pointed by '_rlink_handle_head'. Next the remote device is sent a
 *      RLINK_MSG with type DELETE. If the MSG returns back with an error
 *      or the send itself failed then the previous step of removing the
 *      data from the list is reverted by adding it back to the list.
 *
 * Parameters:
 *      node - (IN/OUT) Node with data which has to be deleted.
 *      args - (IN) register event, if any
 *
 * Returns:
 *      BCM_E_NONE - Successful de-registration
 *      BCM_E_XXX - Other failures
 */
STATIC int
_bcm_rlink_unregister(_rlink_handle_t *node, _rlink_register_args_t *args)
{
    int         last = 1;
    int         rv = BCM_E_NONE;

    if (!RLINK_INIT) {
        return BCM_E_UNAVAIL;
    }

    /* Delete the details from the internal list */
    rv = _bcm_rlink_handle_list_delete(node, &last);

    if (BCM_E_NOT_FOUND == rv) { /* Entry not found- NOOP and return success */
        rv = BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {  /* Found an entry which was removed */
        if (last) {
            rv = _bcm_rlink_send_control(node->cpu, node->cunit,
                                         RLINK_MSG_DELETE, node->type, args);
        }

        if (BCM_FAILURE(rv)) {
            /* If send Msg failed then revert the list changes */
            (void) _bcm_rlink_handle_list_add(node);
        }
    }

    return rv;
}

uint8 *bcm_rlink_decode(uint8 *buf,
                        rlink_msg_t *msgp, rlink_type_t *typep, int *unitp)
{
    
    uint8 *bp = buf;
    rlink_msg_t msg;
    rlink_type_t type;
    int unit;
    
    BCM_UNPACK_U8(bp, msg);
    BCM_UNPACK_U8(bp, type);
    BCM_UNPACK_U32(bp, unit);

    *msgp = msg;
    *typep = type;
    *unitp = unit;
    return bp;
}

uint8 *bcm_rlink_encode(uint8 *buf,
                        rlink_msg_t msg, rlink_type_t type, int unit)
{
    
    uint8 *bp = buf;
    
    BCM_PACK_U8(bp, msg);
    BCM_PACK_U8(bp, type);
    BCM_PACK_U32(bp, unit);

    return bp;
}

/*
 * Packet handler.
 * On client side receives notify packets from server.
 * On server side receives add/delete packets from client.
 */
STATIC bcm_rx_t
_bcm_rlink_pkt_handler(cpudb_key_t cpu,
                       int client_id,
                       bcm_pkt_t *pkt,
                       uint8 *buf,
                       int buf_len,
                       void *cookie)
{
    uint8               *bp;
    int                 unit;
    rlink_msg_t		msg;
    rlink_type_t	type;
    _rlink_notify_t     *rnot;
    int                 i;
    bcm_rx_t            rv;

    COMPILER_REFERENCE(client_id);
    COMPILER_REFERENCE(cookie);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("RLINK msg IN from "
                          CPUDB_KEY_FMT_EOLN),
                 CPUDB_KEY_DISP(cpu)));

    if (!RLINK_INIT || _rlink_thread_exit) {
	return BCM_RX_NOT_HANDLED;
    }

    bp = bcm_rlink_decode(buf, &msg, &type, &unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "RLINK IN msg %s, local-unit %d, type %s\n"),
                 MSG_STR(msg), unit, TYPE_STR(type)));

    rnot = NULL;
    rv = BCM_RX_HANDLED;

    switch (msg) {
    case RLINK_MSG_TRAVERSE:
        INCR_COUNTER(rlink_trav_req_in);
        rnot = sal_alloc(sizeof(*rnot), "bcm_rlink_notify");
        if (rnot == NULL) {
            return BCM_RX_HANDLED;              /* failure */
        }
        CPUDB_KEY_COPY(rnot->cpu, cpu);
        rnot->unit = unit;
        rnot->type = type;
        rnot->pkt_send = FALSE;
        rnot->next = NULL;
        rnot->u.traverse.pkt = pkt;
        rnot->u.traverse.buf = buf;
        rnot->u.traverse.len = buf_len;
        rv = BCM_RX_HANDLED_OWNED;
        break;
    case RLINK_MSG_ADD:
        INCR_COUNTER(rlink_add_req_in);
        rnot = sal_alloc(sizeof(*rnot), "bcm_rlink_notify");
        if (rnot == NULL) {
            return BCM_RX_HANDLED;              /* failure */
        }
        CPUDB_KEY_COPY(rnot->cpu, cpu);
        rnot->unit = unit;
        rnot->type = RLINK_TYPE_ADD;
        rnot->u.msg.type = type;
        rnot->pkt_send = FALSE;
        rnot->next = NULL;
        /* Other parameters in the registration routine */
        switch (type) {
        case RLINK_TYPE_OAM:
            for (i = 0; i < _BCM_OAM_EVENT_TYPES_ARRAY_SIZE; i++) {
                BCM_UNPACK_U32(bp, rnot->register_args.u.oam.events.w[i]);
            }
            break;
        case RLINK_TYPE_BFD:
#if defined(INCLUDE_BFD)
            for (i = 0; i < _BCM_BFD_EVENT_TYPES_ARRAY_SIZE; i++) {
                BCM_UNPACK_U32(bp, rnot->register_args.u.bfd.events.w[i]);
            }
#endif
            break;
        default:
            break;
        }
        break;
    case RLINK_MSG_DELETE:
        INCR_COUNTER(rlink_del_req_in);
        rnot = sal_alloc(sizeof(*rnot), "bcm_rlink_notify");
        if (rnot == NULL) {
            return BCM_RX_HANDLED;              /* failure */
        }
        CPUDB_KEY_COPY(rnot->cpu, cpu);
        rnot->unit = unit;
        rnot->type = RLINK_TYPE_DELETE;
	rnot->u.msg.type = type;
        rnot->pkt_send = FALSE;
        rnot->next = NULL;
	break;
    case RLINK_MSG_NOTIFY:
        INCR_COUNTER(rlink_notify_in);
        /* See if currently space in the queue */
        switch (type) {
        case RLINK_TYPE_LINK:
            INCR_COUNTER(rlink_link_in);
            if (bcm_rlink_link_local_max > 0 &&
		_rlink_queue_size > bcm_rlink_link_local_max) {
                INCR_COUNTER(rlink_link_in_disc);
                return BCM_RX_HANDLED;
            }
            break;
        case RLINK_TYPE_AUTH:
            INCR_COUNTER(rlink_auth_in);
            if (bcm_rlink_auth_local_max > 0 &&
		_rlink_queue_size > bcm_rlink_auth_local_max) {
                INCR_COUNTER(rlink_auth_in_disc);
                return BCM_RX_HANDLED;
            }
            break;
        case RLINK_TYPE_L2:
            INCR_COUNTER(rlink_l2_in);
            if (bcm_rlink_l2_local_max > 0 &&
		_rlink_queue_size > bcm_rlink_l2_local_max) {
                INCR_COUNTER(rlink_l2_in_disc);
                return BCM_RX_HANDLED;
            }
            break;
        case RLINK_TYPE_RX:
            INCR_COUNTER(rlink_rx_in);
            ct_rx_tunnelled_pkt_handler(cpu, bp, buf_len - 1 - 1 - 4);
	    return BCM_RX_HANDLED;
	    break;
        case RLINK_TYPE_OAM:
            INCR_COUNTER(rlink_oam_in);
            if (bcm_rlink_oam_local_max > 0 &&
                _rlink_queue_size > bcm_rlink_oam_local_max) {
                INCR_COUNTER(rlink_oam_in_disc);
                return BCM_RX_HANDLED;
            }
            break;
	default:
	    break;
        }
        rnot = sal_alloc(sizeof(*rnot), "bcm_rlink_notify");
        if (rnot == NULL) {
            return BCM_RX_HANDLED;              /* failure */
        }
        CPUDB_KEY_COPY(rnot->cpu, cpu);
        rnot->unit = unit;
        rnot->type = type;
        rnot->pkt_send = FALSE;
        rnot->next = NULL;
        switch (type) {
        case RLINK_TYPE_LINK:
            BCM_UNPACK_U32(bp, rnot->u.link.port);
            (void) _bcm_unpack_port_info(bp, &rnot->u.link.info);
            break;
        case RLINK_TYPE_AUTH:
            BCM_UNPACK_U32(bp, rnot->u.auth.port);
            BCM_UNPACK_U32(bp, rnot->u.auth.reason);
            break;
        case RLINK_TYPE_L2:
            BCM_UNPACK_U8(bp, rnot->u.l2.insert);
            (void) _bcm_unpack_l2_addr(bp, &rnot->u.l2.addr);
            break;
        case RLINK_TYPE_OAM:
            BCM_UNPACK_U32(bp, rnot->u.oam.flags);
            BCM_UNPACK_U32(bp, rnot->u.oam.event_type);
            BCM_UNPACK_U32(bp, rnot->u.oam.group);
            BCM_UNPACK_U32(bp, rnot->u.oam.endpoint);
            break;
	default:
	    break;
        }
        break;
    }

    if (rnot != NULL) {
	RLINK_ENQUEUE_NOTIFY(rnot);
    }
    return rv;
}

/*
 * Process a notify record.
 */
STATIC void
_bcm_rlink_notify(_rlink_notify_t *rnot)
{
    _rlink_handle_t	*rhand, *nhand;

    if (rnot->pkt_send) { /* Send out prepared packet */
	_bcm_rlink_send_notify(rnot->unit,
			       rnot->type,
			       rnot->u.cli.buf,
			       rnot->u.cli.len,
			       rnot->u.cli.check_cpu,
			       rnot->cpu,
			       rnot->u.cli.ct_flags);
	return;
    }

    /* server side scan add/delete requests */
    switch (rnot->type) {
    case RLINK_TYPE_ADD:
	_bcm_rlink_scan_add(rnot->cpu, rnot->unit, rnot->u.msg.type,
                        &rnot->register_args);
	return;
    case RLINK_TYPE_DELETE:
	_bcm_rlink_scan_delete(rnot->cpu, rnot->unit, rnot->u.msg.type,
                           &rnot->register_args);
	return;

        /* traverse server and client messages are handles identically */
    case RLINK_TYPE_START:
    case RLINK_TYPE_NEXT:
    case RLINK_TYPE_QUIT:
    case RLINK_TYPE_ERROR:
    case RLINK_TYPE_MORE:
    case RLINK_TYPE_DONE:
        (void)bcm_rlink_traverse_request(rnot->type,
                                         rnot->cpu,
                                         rnot->u.traverse.pkt,
                                         rnot->u.traverse.buf,
                                         rnot->u.traverse.len);
        return;
    default:
	break;
    }

    /* client side handler callbacks */
    RLINK_LOCK;
    for (rhand = _rlink_handle_head; rhand != NULL; rhand = nhand) {
	nhand = rhand->next;
	if (rhand->cunit != rnot->unit) {
	    continue;
	}
	if (rhand->type != rnot->type) {
	    continue;
	}
	if (CPUDB_KEY_COMPARE(rhand->cpu, rnot->cpu) != 0) {
	    continue;
	}
	switch (rnot->type) {
	case RLINK_TYPE_LINK:
	    (*rhand->func.link)(rhand->unit,
				rnot->u.link.port,
				&rnot->u.link.info);
	    break;
	case RLINK_TYPE_AUTH:
	    (*rhand->func.auth)(rhand->cookie,
                                rhand->unit,
                                rnot->u.auth.port,
                                rnot->u.auth.reason);
	    break;
	case RLINK_TYPE_L2:
	    (*rhand->func.l2)(rhand->unit,
			      &rnot->u.l2.addr,
			      rnot->u.l2.insert,
			      rhand->cookie);
	    break;
	case RLINK_TYPE_OAM:
	    (*rhand->func.oam)(rhand->unit,
                           rnot->u.oam.flags,
                           rnot->u.oam.event_type,
                           rnot->u.oam.group,
                           rnot->u.oam.endpoint,
                           rhand->cookie);
	    break;
        default:
            break;
	}
    }
    RLINK_UNLOCK;
}

/*
 * Client (and now server) side callback thread.
 * Notify packets are sent to this thread to be executed.
 */
STATIC void
_bcm_rlink_thread(void *cookie)
{
    _rlink_notify_t	*rnot, *fnot;

    COMPILER_REFERENCE(cookie);

    _rlink_thread_exit = 0;
    for (;;) {
	RLINK_SLEEP;

	/* grab a set of notifications off the queue */
	RLINK_NOTIFY_LOCK;
	rnot = _rlink_notify_head;
	_rlink_notify_head = _rlink_notify_tail = NULL;
        _rlink_queue_size = 0;
	RLINK_NOTIFY_UNLOCK;

	/* run the notifications */
	while (rnot != NULL) {
	    _bcm_rlink_notify(rnot);
            fnot = rnot;
            rnot = rnot->next;
            sal_free(fnot);
        }
	if (_rlink_thread_exit) {
	    _rlink_thread = SAL_THREAD_ERROR;
	    sal_thread_exit(0);
	    return;
	}
    }
}

/*
 * Externally visible api routines.
 * Thread start/stop controls.
 * Client registration functions.
 */

/*
 * Start the remote registration system
 */
int
bcm_rlink_start(void)
{
    int		rv, flags, i;
    int     rlink_auth_remote_max, rlink_auth_local_max;
    int     rlink_l2_remote_max, rlink_l2_local_max;
    int     rlink_link_remote_max, rlink_link_local_max;
    int     rlink_oam_remote_max, rlink_oam_local_max;
    int     rlink_rx_remote_max[BCM_COS_COUNT] = BCM_RLINK_RX_REMOTE_MAX_DEFAULT;
    char    *rlink_rx_remote_properties[BCM_COS_COUNT] = {spn_RLINK_RX0_REMOTE_MAX,
                                                          spn_RLINK_RX1_REMOTE_MAX,
                                                          spn_RLINK_RX2_REMOTE_MAX,  
                                                          spn_RLINK_RX3_REMOTE_MAX,  
                                                          spn_RLINK_RX4_REMOTE_MAX,  
                                                          spn_RLINK_RX5_REMOTE_MAX,  
                                                          spn_RLINK_RX6_REMOTE_MAX,  
                                                          spn_RLINK_RX7_REMOTE_MAX};  

    if (RLINK_INIT) {
	return BCM_E_BUSY;
    }

    _rlink_lock = sal_mutex_create("bcm_rlink");
    _rlink_notify_lock = sal_mutex_create("bcm_rlink_notify");
    _rlink_sem = sal_sem_create("bcm_rlink", sal_sem_BINARY, 0);

    /* Get max values if different then default */
    /* Givving 0 hard coded as unit to  soc_property_get is fine 
       since these are unitless settings anyway */
    if (SOC_UNIT_VALID(0)) {
        rlink_auth_remote_max = soc_property_get(0, spn_RLINK_AUTH_REMOTE_MAX, -1);
        rlink_auth_local_max = soc_property_get(0, spn_RLINK_AUTH_LOCAL_MAX, -1);
        rlink_l2_remote_max = soc_property_get(0, spn_RLINK_L2_REMOTE_MAX, -1);
        rlink_l2_local_max = soc_property_get(0, spn_RLINK_L2_LOCAL_MAX, -1);
        rlink_link_remote_max = soc_property_get(0, spn_RLINK_LINK_REMOTE_MAX, -1);
        rlink_link_local_max = soc_property_get(0, spn_RLINK_LINK_LOCAL_MAX, -1);
        rlink_oam_remote_max = soc_property_get(0, spn_RLINK_OAM_REMOTE_MAX, -1);
        rlink_oam_local_max = soc_property_get(0, spn_RLINK_OAM_LOCAL_MAX, -1);
    } else {
        rlink_auth_remote_max = -1;
        rlink_auth_local_max = -1;
        rlink_l2_remote_max = -1;
        rlink_l2_local_max = -1;
        rlink_link_remote_max = -1;
        rlink_link_local_max = -1;
        rlink_oam_remote_max = -1;
        rlink_oam_local_max = -1;
    }

    if (-1 != rlink_auth_remote_max) {
        bcm_rlink_auth_remote_max = rlink_auth_remote_max;
    }
    if (-1 != rlink_auth_local_max) {
        bcm_rlink_auth_local_max = rlink_auth_local_max;
    }
    if (-1 != rlink_l2_remote_max) {
        bcm_rlink_l2_remote_max = rlink_l2_remote_max;
    }
    if (-1 != rlink_l2_local_max) {
        bcm_rlink_l2_local_max = rlink_l2_local_max;
    }
    if (-1 != rlink_link_remote_max) {
        bcm_rlink_link_remote_max = rlink_link_remote_max;
    }
    if (-1 != rlink_link_local_max) {
        bcm_rlink_link_local_max = rlink_link_local_max;
    }
    if (-1 != rlink_oam_remote_max) {
        bcm_rlink_oam_remote_max = rlink_oam_remote_max;
    }
    if (-1 != rlink_oam_local_max) {
        bcm_rlink_oam_local_max = rlink_oam_local_max;
    }

    for (i = 0 ; i < BCM_COS_COUNT; i++) {
        if (SOC_UNIT_VALID(0)) {
            rlink_rx_remote_max[i] = soc_property_get(0, rlink_rx_remote_properties[i], -1);
        } else {
            rlink_rx_remote_max[i] = -1;
        }
        if (-1 != rlink_rx_remote_max[i]) {
            bcm_rlink_rx_remote_max[i] = rlink_rx_remote_max[i];
        }
    }

    _rlink_thread = sal_thread_create("bcmRLINK",
				      RLINK_THREAD_STACK,
				      RLINK_THREAD_PRIO,
				      _bcm_rlink_thread,
				      NULL);
    if (_rlink_thread == SAL_THREAD_ERROR) {
    	sal_sem_destroy(_rlink_sem);
    	sal_mutex_destroy(_rlink_notify_lock);
    	_rlink_notify_lock = NULL;
    	sal_mutex_destroy(_rlink_lock);
    	_rlink_lock = NULL;
    
        return BCM_E_RESOURCE;
    }

    /* can be configured to use nexthop or cpu2cpu */
    if (_rlink_nexthop) {
        flags = ATP_F_REASSEM_BUF | ATP_F_NEXT_HOP;
    } else {
        flags = ATP_F_REASSEM_BUF;
    }
    rv = atp_register(RLINK_CLIENT_ID, flags,
                      _bcm_rlink_pkt_handler, NULL, -1, -1);

    if (BCM_SUCCESS(rv)) {
        rv = bcm_rlink_traverse_init();
    }
    return rv;
}

/*
 * Stop the remote registration system
 */
int
bcm_rlink_stop(void)
{
    _rlink_notify_t	*rnot, *nnot;

    if (!RLINK_INIT) {
	return BCM_E_NONE;
    }
    atp_unregister(RLINK_CLIENT_ID);

    /* destroy thread */
    _rlink_thread_exit = 1;
    RLINK_WAKE;
    sal_thread_yield();
    while (_rlink_thread != SAL_THREAD_ERROR) {
	RLINK_WAKE;
	sal_usleep(10000);
    }

    /* free any queued notification */
    RLINK_NOTIFY_LOCK;
    rnot = _rlink_notify_head;
    _rlink_notify_head = _rlink_notify_tail = NULL;
    _rlink_queue_size = 0;
    RLINK_NOTIFY_UNLOCK;
    while (rnot != NULL) {
	nnot = rnot->next;
	sal_free(rnot);
	rnot = nnot;
    }

    sal_sem_destroy(_rlink_sem);
    sal_mutex_destroy(_rlink_notify_lock);
    _rlink_notify_lock = NULL;
    sal_mutex_destroy(_rlink_lock);
    _rlink_lock = NULL;
    (void) bcm_rlink_traverse_deinit();
    return BCM_E_NONE;
}

/*
 * When a unit is detached from the system, this should be called.
 * It will flush all rlink handlers asssociated with the unit.
 * It should only be called for remote units.  Since it assumes the
 * unit is gone, it does not do commands back to the controlling CPU.
 */

int
bcm_rlink_device_detach(int unit)
{
    _rlink_handle_t	*rhand, *phand, *nhand;

    if (!RLINK_INIT) {
	return BCM_E_UNAVAIL;
    }

    RLINK_LOCK;
    phand = NULL;
    for (rhand = _rlink_handle_head; rhand != NULL; /* see below */) {
        nhand = rhand->next;   /* rhand my be deallocated */
        if (unit == rhand->unit) {  /* Remove from list */
            if (phand == NULL) {
                _rlink_handle_head = rhand->next;
            } else {
                phand->next = rhand->next;
            }
            if (rhand == _rlink_handle_tail) {
                _rlink_handle_tail = phand;
            }
            sal_free(rhand);
        } else {
            phand = rhand;
        }
        rhand = nhand;
    }
    RLINK_UNLOCK;

    return BCM_E_NONE;
}

/*
 * Auth registration for de-authorization callback.
 *
 * Due to the implementation of the auth_unauth callback
 * structure (there is only one call; it's not a register/unregister
 * protocol) we make the following convention:  Multiple callbacks
 * may be registered per device (unit).
 *
 * To de-register, func is passed as NULL.  This causes all
 * registered entries that match unit and cookie to be deleted.
 */

int
bcm_client_auth_unauth_callback(int unit,
                                bcm_auth_cb_t func,
                                void *cookie)
{
    int rv = BCM_E_NONE;

    if (!RLINK_INIT) {
        return BCM_E_UNAVAIL;
    }

    if (func == NULL) { /* Unregister entries matching unit and cookie */
        int last = 1;
        int cunit = 0;
        int deleted = 0;
        cpudb_key_t cpukey;
        _rlink_handle_t *rhand = NULL; /* Iterator thru handler list */
        _rlink_handle_t *phand = NULL; /* Track previous node in the list */
        _rlink_handle_t *to_delete = NULL;
        _rlink_handle_t *tmp_list_head = NULL;
        _rlink_handle_t *tmp_list_tail = NULL;

        RLINK_LOCK;
        for (rhand = _rlink_handle_head; rhand != NULL; ) {
            if (rhand->type == RLINK_TYPE_AUTH && rhand->unit == unit) {
                if (rhand->cookie == cookie) {
                    /* Record info in case disconnection done later */
                    deleted = 1;
                    CPUDB_KEY_COPY(cpukey, rhand->cpu);
                    cunit = rhand->cunit;
                    /* Remove this entry */
                    if (phand == NULL) { /* First item on list being removed */
                        _rlink_handle_head = rhand->next;
                    } else {
                        phand->next = rhand->next;
                    }
                    if (rhand == _rlink_handle_tail) { /* Last in list */
                        _rlink_handle_tail = phand;
                    }
                    /* Safe deletion of pointer; don't update phand */
                    to_delete = rhand;
                    rhand = rhand->next;

                    /* Insert node to be deleted in the tmp list */
                    to_delete->next = NULL;
                    if (NULL == tmp_list_tail) {
                        tmp_list_head = tmp_list_tail = to_delete;
                    } else {
                        tmp_list_tail->next = to_delete;
                        tmp_list_tail = to_delete;
                    }
                } else { /* Entry matches type and device, but not cookie */
                    last = 0; /* Won't disconnect from CPU */
                    phand = rhand;           /* Update pointers */
                    rhand = rhand->next;
                }
            } else {  /* Move to next entry in list */
                phand = rhand;
                rhand = rhand->next;
            }
        }
        RLINK_UNLOCK;

        if (deleted) {
            if (last) {
                rv = _bcm_rlink_send_control(cpukey, cunit,
                                             RLINK_MSG_DELETE, RLINK_TYPE_AUTH,
                                             NULL);
            }

            if (BCM_SUCCESS(rv)) {
                /* Now de-allocate the deleted nodes */
                for (rhand = tmp_list_head; rhand != NULL;) {
                    to_delete = rhand;
                    rhand = rhand->next;
                    sal_free(to_delete);
                }
            } else {
                /* DEL Msg send failed, add back removed nodes to the list */
                RLINK_LOCK;
                if (NULL == _rlink_handle_tail) {
                    _rlink_handle_head = tmp_list_head;
                } else {
                    _rlink_handle_tail->next = tmp_list_head;
                }
                _rlink_handle_tail = tmp_list_tail;
                RLINK_UNLOCK;
            }
        }
    } else {  /* Non-null function, register */
        _rlink_handle_t node = {0};

        /* Fill in required details */
        node.unit = unit;
        node.type = RLINK_TYPE_AUTH;
        node.func.auth = func;
        node.cookie = cookie;

        rv = _bcm_rlink_register(&node, NULL);
    }

    return rv;
}

/*
 * Register a linkscan callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_linkscan_register(int unit, bcm_linkscan_handler_t f)
{
    _rlink_handle_t node = {0};

    /* Fill in required details */
    node.unit = unit;
    node.type = RLINK_TYPE_LINK;
    node.func.link = f;
    node.cookie = NULL;

    return _bcm_rlink_register(&node, NULL);
}

/*
 * Remove a linkscan callback function on the rpc client
 * Referenced from the client dispatch table
 */

int
bcm_client_linkscan_unregister(int unit, bcm_linkscan_handler_t f)
{
    _rlink_handle_t node = {0};

    /* Fill in required details */
    node.unit = unit;
    node.type = RLINK_TYPE_LINK;
    node.func.link = f;
    node.cookie = NULL;

    return _bcm_rlink_unregister(&node, NULL);
}

/*
 * Register an l2 callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_l2_addr_register(int unit,
			    bcm_l2_addr_callback_t f,
			    void *cookie)
{
    _rlink_handle_t node = {0};

    /* Fill in required details */
    node.unit = unit;
    node.type = RLINK_TYPE_L2;
    node.func.l2 = f;
    node.cookie = cookie;

    return _bcm_rlink_register(&node, NULL);
}

/*
 * Remove an l2 callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_l2_addr_unregister(int unit,
			      bcm_l2_addr_callback_t f,
			      void *cookie)
{
    _rlink_handle_t node = {0};

    /* Fill in required details */
    node.unit = unit;
    node.type = RLINK_TYPE_L2;
    node.func.l2 = f;
    node.cookie = cookie;
;
    return _bcm_rlink_unregister(&node, NULL);
}


/*
 * Register an oam event callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_oam_event_register(int unit,
                              bcm_oam_event_types_t event_types,
                              bcm_oam_event_cb f,
                              void *cookie)
{
    _rlink_handle_t node = {0};
    _rlink_register_args_t register_args;

    /* Fill in required details */
    node.unit = unit;
    node.type = RLINK_TYPE_OAM;
    node.func.oam = f;
    node.cookie = cookie;
    register_args.u.oam.events = event_types;

    return _bcm_rlink_register(&node, &register_args);
}

/*
 * Remove an oam event callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_oam_event_unregister(int unit,
                                bcm_oam_event_types_t event_types, 
                                bcm_oam_event_cb f)
{
    _rlink_handle_t node = {0};
    _rlink_register_args_t register_args;

    /* Fill in required details */
    node.unit = unit;
    node.type = RLINK_TYPE_OAM;
    node.func.oam = f;
    node.cookie = NULL;
    register_args.u.oam.events = event_types;

    return _bcm_rlink_unregister(&node, &register_args);
}

#if defined(INCLUDE_BFD)
/*
 * Register an bfd event callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_bfd_event_register(int unit,
                              uint32 event_types,
                              bcm_bfd_event_cb f,
                              void *cookie)
{
    /* to be done for BFD, including this function to compile */
    return BCM_E_NONE;
}

/*
 * Remove an bfd event callback function on the rpc client
 * Referenced from the client dispatch table
 */
int
bcm_client_bfd_event_unregister(int unit,
                                uint32 event_types, 
                                bcm_bfd_event_cb f)
{
    /* to be done for BFD, including this function to compile */
    return BCM_E_NONE;
}
#endif

/*
 * Special connect and disconnect code for RX.
 *
 * These correspond to register and unregister above, but are slightly
 * different because the function callbacks for RX are handled by
 * the RX subsystem (so that the packets can go through the normal
 * RX stack).
 *
 * The functions below make the proper setup happen for the RLink portion
 * of the process.
 *
 * This assumes that the upper layer is keeping track of redundant
 * calls for connecting, so no checking of that is done here.
 *
 * Unit must be a remote unit number; it is translated here to
 * a physical unit number on the remote CPU.
 */

int
bcm_rlink_rx_connect(int unit)
{
    cpudb_key_t *cpu;
    int cpu_unit;

    if (!RLINK_INIT) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_IS_LOCAL(unit)) {
        return BCM_E_PARAM;
    }

    cpu = (cpudb_key_t *)BCM_CONTROL(unit)->drv_control;
    cpu_unit = BCM_CONTROL(unit)->unit;

    return (_bcm_rlink_send_control(
                *cpu, cpu_unit, RLINK_MSG_ADD, RLINK_TYPE_RX, NULL));
}

int
bcm_rlink_rx_disconnect(int unit)
{
    cpudb_key_t *cpu;
    int cpu_unit;

    if (!RLINK_INIT) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_IS_LOCAL(unit)) {
        return BCM_E_PARAM;
    }

    cpu = (cpudb_key_t *)BCM_CONTROL(unit)->drv_control;
    cpu_unit = BCM_CONTROL(unit)->unit;

    return (_bcm_rlink_send_control(
                *cpu, cpu_unit, RLINK_MSG_DELETE, RLINK_TYPE_RX, NULL));
}

#if defined(BROADCOM_DEBUG)
void
bcm_rlink_dump(void)
{
    _rlink_handle_t *h_ent;
    _rlink_scan_t *s_ent;

    if (_rlink_lock == NULL) {
        LOG_CLI((BSL_META("RLink not initialized\n")));
        return;
    }

    LOG_CLI((BSL_META("RLink thread %x; thread exit %d\n"),
             PTR_TO_INT(_rlink_thread), _rlink_thread_exit));

    LOG_CLI((BSL_META("Server side scan list\n")));
    for (s_ent = _rlink_scan_head; s_ent != NULL; s_ent = s_ent->next) {
        LOG_CLI((BSL_META("    Unit %d. Type %s. CPU " CPUDB_KEY_FMT_EOLN),
                 s_ent->unit, TYPE_STR(s_ent->type),
                 CPUDB_KEY_DISP(s_ent->cpu)));
    }

    LOG_CLI((BSL_META("Client side handler registrants\n")));
    for (h_ent = _rlink_handle_head; h_ent != NULL; h_ent = h_ent->next) {
        LOG_CLI((BSL_META("    Fn %p. Unit %d. Type %s. CPU " CPUDB_KEY_FMT_EOLN),
                          h_ent->func.link,
                          h_ent->unit,
                          TYPE_STR(h_ent->type),
                 CPUDB_KEY_DISP(h_ent->cpu)));
    }

    LOG_CLI((BSL_META("Counters:\n")));
    LOG_CLI((BSL_META("  l2 in %d. l2 in disc %d. l2 out %d. l2 out disc %d\n"),
             rlink_l2_in, rlink_l2_in_disc,
             rlink_l2_out, rlink_l2_out_disc));
    LOG_CLI((BSL_META("  link in %d. link in disc %d. link out %d. link out "
                      "disc %d\n"), rlink_link_in, rlink_link_in_disc,
             rlink_link_out, rlink_link_out_disc));
    LOG_CLI((BSL_META("  auth in %d. auth in disc %d. auth out %d. auth out disc %d\n"),
             rlink_auth_in, rlink_auth_in_disc,
             rlink_auth_out, rlink_auth_out_disc));
    LOG_CLI((BSL_META("  rx in %d. rx in disc %d. rx out %d. rx out disc %d\n"),
             rlink_rx_in, rlink_rx_in_disc,
             rlink_rx_out, rlink_rx_out_disc));
    LOG_CLI((BSL_META("  oam event in %d. oam event in disc %d. oam event out %d. "
                      "oam event out disc %d\n"),
             rlink_oam_in, rlink_oam_in_disc,
             rlink_oam_out, rlink_oam_out_disc));
    LOG_CLI((BSL_META("  bfd event in %d. bfd event in disc %d. bfd event out %d. "
                      "bfd event out disc %d\n"),
             rlink_bfd_in, rlink_bfd_in_disc,
             rlink_bfd_out, rlink_bfd_out_disc));
    LOG_CLI((BSL_META("  add req %d. del req %d. notify %d\n"),
             rlink_add_req_in, rlink_del_req_in, rlink_notify_in));
    LOG_CLI((BSL_META("  trav req %d.\n"),
             rlink_trav_req_in));
    LOG_CLI((BSL_META("  pkt buffer alloc failure %d. pkt send failure %d\n"),
             rlink_buff_alloc_fail, rlink_pkt_send_fail));

    bcm_linkscan_dump(0);
}
#endif	/* BROADCOM_DEBUG */

#endif  /* BCM_RPC_SUPPORT */
