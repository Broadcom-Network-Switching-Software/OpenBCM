/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        nh_tx.c
 * Purpose:     Next hop transport services
 * Requires:
 * Notes:
 *    A per-stack port transport pointer is supported.  This is
 *    primarily for supporting communication on Linux between the
 *    kernel and user modes.
 */

#include <shared/bsl.h>

#include <shared/idents.h>

#include <sal/core/sync.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm/error.h>
#include <bcm/l2.h>

#include <appl/cputrans/nh_tx.h>
#include <appl/cputrans/cputrans.h>

#include "t_util.h"

#define STK_PORTS_MAX	CPUDB_CXN_MAX

/* Packet parameters */
static bcm_mac_t nh_dest_mac           = NH_TX_DEST_MAC_DEFAULT;
static bcm_mac_t nh_snap_mac           = NH_TX_SNAP_MAC_DEFAULT;

/* These make resetting easier */
static bcm_mac_t nh_dest_mac_default   = NH_TX_DEST_MAC_DEFAULT;
static bcm_mac_t nh_snap_mac_default   = NH_TX_SNAP_MAC_DEFAULT;

/* The following are in host order */
static uint16 nh_snap_type             = NH_TX_SNAP_TYPE_DEFAULT;
static uint16 nh_local_type            = NH_TX_LOCAL_PKT_TYPE;

static int nh_dest_port                = NH_TX_DEST_PORT_DEFAULT;
static int nh_dest_mod                 = NH_TX_DEST_MOD_DEFAULT;

static int nh_src_port                 = NH_TX_SRC_PORT_DEFAULT;
static int nh_src_mod                  = NH_TX_SRC_MOD_DEFAULT;
static int nh_tx_unknown_modid         = NH_TX_SRC_MOD_DEFAULT;

static bcm_mac_t nh_local_mac;

static sal_mutex_t nh_mutex;

static int init_done;
static int setup_done;

static volatile int pending_count;

static bcm_trans_ptr_t nh_trans_ptr;

/*
 * Local stack ports.  (unit, port) and flag to indicate duplex
 */
static struct stk_port_t {
    int unit;
    int port;
    bcm_trans_ptr_t *trans_ptr;
    int src_mod;   /* For stack-port specific destination info */
    int src_port;
} nh_stk_ports[STK_PORTS_MAX];
static int num_stk_ports;

#define NH_LOCK sal_mutex_take(nh_mutex, sal_sem_FOREVER);
#define NH_UNLOCK sal_mutex_give(nh_mutex);
#define NH_INIT if (!init_done) BCM_IF_ERROR_RETURN(nh_init())

/* Forward declarations */
STATIC int nh_init(void);
STATIC void nh_tx_callback(int unit, bcm_pkt_t *pkt, void *cookie);
STATIC void nh_tx_callback_nofree(int unit, bcm_pkt_t *pkt, void *cookie);
STATIC void _nh_tx_l2_header_setup(uint8 *pkt_buf, uint16 vlan);

typedef struct {
    nh_tx_cb_f callback;
} nh_cb_cookie_t;

STATIC void *
_nh_callback_cookie_set(nh_tx_cb_f callback)
{
    nh_cb_cookie_t *cookie = sal_alloc(sizeof(*cookie), "nh_cookie");

    if (cookie) {
        cookie->callback = callback;
    }

    return (void *)cookie;
}

STATIC void
_nh_callback_cookie_get(void *cookie, nh_tx_cb_f *cb)
{
    if (cookie) {
        nh_cb_cookie_t *nh_cookie = (nh_cb_cookie_t *)cookie;
        *cb = nh_cookie->callback;
        sal_free(nh_cookie);
    } else {
        *cb = NULL;
    }
}

/*
 * Function:
 *      nh_tx_setup
 * Purpose:
 *      Set up the next hop subsystem
 * Parameters:
 *      trans_ptr    - Pointer to transport function structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
nh_tx_setup(bcm_trans_ptr_t *trans_ptr)
{
    NH_INIT;

    if (trans_ptr->tp_tx == NULL) {
        return BCM_E_PARAM;
    }

    sal_memcpy(&nh_trans_ptr, trans_ptr, sizeof(bcm_trans_ptr_t));

    setup_done = TRUE;

    return BCM_E_NONE;
}


/*
 * Function:
 *      nh_tx_setup_done
 * Purpose:
 *      Indicate if nh_tx has been setup
 * Parameters:
 *      None
 * Returns:
 *      Boolean:  FALSE if not setup, TRUE if setup
 * Notes:
 */

int
nh_tx_setup_done(void)
{
    return setup_done;
}

/*
 * Function:
 *      nh_tx_trans_ptr_set
 * Purpose:
 *      Set the transport pointer for the given stack port
 * Parameters:
 *      unit               - The unit on which the stack port lies
 *      port               - The physical port of the stack port
 *      ptr                - The pointer to set to
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Setting to NULL will remove the entry; good for cleaning
 *      up if stack ports are coming and going.
 */

int
nh_tx_trans_ptr_set(int unit, int port, bcm_trans_ptr_t *ptr)
{
    int i, j;

    NH_INIT;
    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
	    nh_stk_ports[i].port == port) {
            if (ptr == NULL) {    /* If ptr == NULL, remove the entry */
                for (j = i; j < num_stk_ports - 1; j++) {
                    nh_stk_ports[j] = nh_stk_ports[j + 1];
                }
                nh_stk_ports[num_stk_ports - 1].trans_ptr = NULL;
                nh_stk_ports[num_stk_ports - 1].unit = -1;
                nh_stk_ports[num_stk_ports - 1].port = -1;
                num_stk_ports--;
            } else {
                nh_stk_ports[i].trans_ptr = ptr;
            }
            NH_UNLOCK;
            return BCM_E_NONE;
        }
    }

    if (ptr != NULL) {       /* Add new entry */
        if (num_stk_ports >= STK_PORTS_MAX) {
            NH_UNLOCK;
            return BCM_E_RESOURCE;
        }
        nh_stk_ports[num_stk_ports].unit = unit;
        nh_stk_ports[num_stk_ports].port = port;
        nh_stk_ports[num_stk_ports].trans_ptr = ptr;
        num_stk_ports++;
    }

    NH_UNLOCK;
    return BCM_E_NONE;
}


/*
 * Function:
 *      nh_tx_trans_ptr_get
 * Purpose:
 *      Get the transport pointer for the given stack port
 * Parameters:
 *      unit               - The unit on which the stack port lies
 *      port               - The physical port of the stack port
 *      ptr                - (OUT) Set to port's pointer
 * Returns:
 *      BCM_E_XXX
 */

int
nh_tx_trans_ptr_get(int unit, int port, bcm_trans_ptr_t **ptr)
{
    int i;
    int rv = BCM_E_NOT_FOUND;

    NH_INIT;
    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
	    nh_stk_ports[i].port == port) {
            *ptr = nh_stk_ports[i].trans_ptr;
            rv = BCM_E_NONE;
            break;
        }
    }
    NH_UNLOCK;

    return rv;
}

/*
 * In general, these should only be called by next_hop to keep
 * the information consistent.
 *
 * Setting src_mod/port to < 0 will result in using the default
 * setting (from nh_mod_id, etc).
 */

int
nh_tx_src_mod_port_set(int unit, int port, int src_mod, int src_port)
{
    int i;
    int rv = BCM_E_NOT_FOUND;

    NH_INIT;
    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
            nh_stk_ports[i].port == port) {
            
            LOG_INFO(BSL_LS_TKS_NH,
                     (BSL_META_U(unit,
                                 "NH: Set unit %d, port %d, src-mod %d, src-port %d\n"),
                      unit, port, src_mod, src_port));

            nh_stk_ports[i].src_mod = src_mod;
            nh_stk_ports[i].src_port = src_port;
            rv = BCM_E_NONE;
            break;
        }
    }

    if (rv == BCM_E_NOT_FOUND) {
        if (num_stk_ports < STK_PORTS_MAX) {
            
            LOG_INFO(BSL_LS_TKS_NH,
                     (BSL_META_U(unit,
                                 "NH: Add unit %d, port %d, src-mod %d, src-port %d\n"),
                      unit, port, src_mod, src_port));
            
            nh_stk_ports[num_stk_ports].unit = unit;
            nh_stk_ports[num_stk_ports].port = port;
            nh_stk_ports[num_stk_ports].src_mod = src_mod;
            nh_stk_ports[num_stk_ports].src_port = src_port;
            num_stk_ports++;
            rv = BCM_E_NONE;
        }
    }

    NH_UNLOCK;

    return rv;
}

int
nh_tx_src_mod_port_get(int unit, int port, int *src_mod, int *src_port)
{
    int i;
    int rv = BCM_E_NOT_FOUND;

    NH_INIT;
    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
            nh_stk_ports[i].port == port) {

            if (src_mod != NULL) {
                *src_mod = nh_stk_ports[i].src_mod;
            }
            if (src_port != NULL) {
                *src_port = nh_stk_ports[i].src_port;
            }
            rv = BCM_E_NONE;
            break;
        }
    }
    NH_UNLOCK

    return rv;
}

/*
 * Set the src mod-id, src port for a list of packets according
 * to which stack port they're going out
 */
STATIC void
pkt_list_src_mod_id_port_set(bcm_pkt_t *pkt)
{
    int smod = nh_src_mod;      /* Default values */
    int sport = nh_src_port;
    int port;
    int i;
    bcm_pkt_t *cur_pkt;

    /* Find the port on which we're transmitting */
    BCM_PBMP_ITER(pkt->tx_pbmp, port) {
        break;
    }

    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == pkt->unit &&
	    nh_stk_ports[i].port == port) {
            if (nh_stk_ports[i].src_mod >= 0) {
                smod = nh_stk_ports[i].src_mod;
            }
            if (nh_stk_ports[i].src_port >= 0) {
                sport = nh_stk_ports[i].src_port;
            }
        }
    }
    NH_UNLOCK;

    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        
        cur_pkt->flags |= (BCM_TX_SRC_MOD|BCM_TX_SRC_PORT);
        cur_pkt->src_mod = smod;
        cur_pkt->src_port = sport;
    }
}

/*
 * If the unit/port has a transport pointer assigned, use that.
 * otherwise, use the high level one
 */
STATIC bcm_trans_ptr_t *
resolve_trans_ptr(int unit, int port)
{
    int i;
    bcm_trans_ptr_t *rv =  &nh_trans_ptr;

    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
	    nh_stk_ports[i].port == port) {
	    if (nh_stk_ports[i].trans_ptr != NULL) {
                rv = nh_stk_ports[i].trans_ptr;
            }
            break;
        }
    }
    NH_UNLOCK;

    return rv;
}

/*
 * As above, but for packet
 */
STATIC bcm_trans_ptr_t *
resolve_pkt_trans_ptr(bcm_pkt_t *pkt)
{
    bcm_port_t port;

    BCM_PBMP_ITER(pkt->tx_pbmp, port) {
        break;
    }

    return resolve_trans_ptr(pkt->unit, port);
}

/*
 * Function:
 *      nh_tx_dest_(set|get)
 * Purpose:
 *      Set/get the parameters used for default next hop destination settings
 *      Note that these values may be overridden on a per-stack-port
 *      basis.
 * Parameters:
 *      mac              - (OUT for get) The destination mac address used
 *      dest_mod         - (OUT for get) The destination mod id used
 *      dest_port        - (OUT for get) The destination port used
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      No checking is done for "legality" of values.
 */

int
nh_tx_dest_set(const bcm_mac_t mac, int dest_mod, int dest_port)
{
    nh_dest_mod = dest_mod;
    nh_dest_port = dest_port;
    sal_memcpy(nh_dest_mac, mac, sizeof(bcm_mac_t));

    return BCM_E_NONE;
}

int
nh_tx_dest_get(bcm_mac_t *mac, int *dest_mod, int *dest_port)
{
    if (dest_mod != NULL) {
        *dest_mod = nh_dest_mod;
    }
    if (dest_port != NULL) {
        *dest_port = nh_dest_port;
    }
    if (mac != NULL) {
        sal_memcpy(mac, nh_dest_mac, sizeof(bcm_mac_t));
    }

    return BCM_E_NONE;
}

/*
 * Install or remove the NH TX dest mac into all local L2 tables
 */

int
nh_tx_dest_install(int install, int vid)
{
    bcm_l2_addr_t nh_l2_addr;
    int unit;
    int rv;

    bcm_l2_addr_t_init(&nh_l2_addr, nh_dest_mac, vid);
    nh_l2_addr.flags |= BCM_L2_STATIC;
    nh_l2_addr.flags |= BCM_L2_LOCAL_CPU;
    BCM_FOREACH_LOCAL_UNIT(unit) {
        if (install) {
            rv = bcm_l2_addr_add(unit, &nh_l2_addr);
            if ((rv != BCM_E_UNAVAIL) && (rv != BCM_E_NONE)) {
                LOG_ERROR(BSL_LS_TKS_NH,
                          (BSL_META("NH TX ERROR adding L2 addr to "
                                    "unit %d: %s\n"),
                           unit, bcm_errmsg(rv)));
                return rv;
            }
        } else {
            (void)bcm_l2_addr_delete(unit, nh_dest_mac, vid);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      nh_tx_src_(set|get)
 * Purpose:
 *      Set/get the parameters used for next hop source settings
 * Parameters:
 *      src_mod   - (OUT for get) The source mod id used
 *      src_port  - (OUT for get) The source port used
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      No checking is done for "legality" of values.
 */

int
nh_tx_src_set(int src_mod, int src_port)
{
    nh_src_mod = src_mod;
    nh_src_port = src_port;

    return BCM_E_NONE;
}

int
nh_tx_src_get(int *src_mod, int *src_port)
{
    *src_mod = nh_src_mod;
    *src_port = nh_src_port;

    return BCM_E_NONE;
}


/* Resolve dest mod/port IDs for packet, based on transmit unit/port */

STATIC void
_nh_src_mod_id_port_get(int unit, int port, int *src_mod, int *src_port)
{
    int i;

    *src_mod = nh_src_mod;            /* Default values */
    *src_port = nh_src_port;

    /* Check to see if overridden by port specific values */
    NH_LOCK;
    for (i = 0; i < num_stk_ports; i++) {
        if (nh_stk_ports[i].unit == unit &&
            nh_stk_ports[i].port == port) {
            if (nh_stk_ports[i].src_mod >= 0) {
                *src_mod = nh_stk_ports[i].src_mod;
            }
            if (nh_stk_ports[i].src_port >= 0) {
                *src_port = nh_stk_ports[i].src_port;
            }
            break;
        }
    }
    NH_UNLOCK;
}

/*
 * Set up a next hop packet before we know what (unit, port) it will
 * go out on; this sets up the dest port/mod, the COS, the internal priority,
 * and gives it the default source mod/port.  If the CPUTRANS_NO_L2_UPDATE
 * is not set, then the L2 header is also updated.
 *
 * The 'cos' parameter contains the cos and internal priority
 * values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 * to set values accordingly.  The internal priority is optional;
 * if this is not provided, the cos value is used for internal priority.
 */

STATIC void
_nh_pkt_setup(bcm_pkt_t *pkt, int cos, uint16 vlan,
              uint16 type, uint32 flags)
{
    uint8 *pkt_data;

    pkt->opcode = BCM_HG_OPCODE_CPU;
    pkt->dest_port = nh_dest_port;
    pkt->dest_mod = nh_dest_mod;
    pkt->src_mod = nh_src_mod; /* Set up with defaults */
    pkt->src_port = nh_src_port;
    pkt->flags |= BCM_TX_SRC_MOD | BCM_TX_SRC_PORT;

    /* If internal priority is not provided, set value to cos */
    pkt->cos = CPUTRANS_COS_GET(cos);
    if (cos & CPUTRANS_INT_PRIO_VALID) {
        pkt->prio_int = CPUTRANS_INT_PRIO_GET(cos);
    } else {
        pkt->prio_int = pkt->cos;
    }
    pkt->flags |= BCM_TX_PRIO_INT;

    /* Pack the transport multiplexing type and the unit/port into pkt */
    pkt_data = &(pkt->pkt_data[0].data[CPUTRANS_TR_MULT_OFS]);
    PACK_SHORT(pkt_data, type);
    if (!(flags & CPUTRANS_NO_L2_UPDATE)) {
        _nh_tx_l2_header_setup(pkt->pkt_data[0].data,
                               BCM_VLAN_CTRL(cos, 0, vlan));
    }
}

/*
 * Final setup of a next hop packet once the local TX unit, port are known
 * This sets the source mod/port on a per-stack port basis (if configured).
 * The transmit unit and port are set in the packet data.  If a final
 * setup is programmed into the transport pointer, that is called as
 * well.
 */

STATIC void
_nh_pkt_local_setup(bcm_pkt_t *pkt, int unit, int port, int nh_hdr_update)
{
    uint8 *pkt_data;
    bcm_trans_ptr_t *tp;
    int smod, sport;

    _nh_src_mod_id_port_get(unit, port, &smod, &sport);
    pkt->flags |= (BCM_TX_SRC_MOD|BCM_TX_SRC_PORT);
    pkt->src_mod = smod;
    pkt->src_port = sport;
    
    pkt->unit = unit;
    BCM_PBMP_PORT_SET(pkt->tx_pbmp, port);

    if (nh_hdr_update) {
        /* Set the unit/port in the header */
        pkt_data = &(pkt->pkt_data[0].data[CPUTRANS_TR_RSVD_OFS]);
        *pkt_data++ = (uint8)unit;
        *pkt_data = (uint8)port;
    }

    tp = resolve_trans_ptr(unit, port);
    if (tp->tp_setup_tx != NULL) {
        /* Final setup of packet for unit type */
        tp->tp_setup_tx(unit, pkt);
    }
}

/*
 * Function:
 *      nh_tx
 * Purpose:
 *      Transmit a next hop packet.
 * Parameters:
 *      unit       - Which unit to send on
 *      port       - Which port to send out
 *      pkt_buf    - Packet buffer
 *      len        - Total length of packet
 *      cos        - Cos and internal priority
 *      vlan       - VLAN
 *      pkt_type   - Type marker to use in header
 *      ct_flags   - See below.
 *      callback   - If non-null, send async and make callback
 *      cookie     - Cookie for callback.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Does not do segmentation.
 *
 *      Flags:
 *          CPUTRANS_NO_HEADER_ALLOC   - The packet already contains
 *              space for the CPUTRANS header.
 *          CPUTRANS_NO_L2_UPDATE      - Do not update the L2 header
 *              of the packet before forwarding.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.   Use CPUTRANS_COS_SET() CPUTRANS_INT_PRIO_SET()
 *      to set values accordingly.  The internal priority is optional;
 *      if this is not provided, the cos value is used for internal priority.
 */

int
nh_tx(int unit,
      int port,
      uint8 *pkt_buf,
      int len,
      int cos,
      int vlan,
      uint16 pkt_type,
      uint32 ct_flags,
      nh_tx_cb_f callback,
      void *cookie)
{
    bcm_pkt_t *pkt;
    int rv;
    bcm_trans_ptr_t *tp;
    nh_cb_cookie_t *nh_cookie = NULL;

    if (!setup_done) {
        return BCM_E_INIT;
    }

    LOG_DEBUG(BSL_LS_TKS_NH,
              (BSL_META_U(unit,
                          "NHTX: (%d, %d). %p, len %d, type %d. flags %x "
                          "cb %p, cookie %p\n"),
               unit, port, pkt_buf, len, pkt_type, ct_flags,
               callback, cookie));

    /* Force single packet allocation */
    pkt = cputrans_tx_pkt_alloc(pkt_buf, len, ct_flags);
    if (pkt == NULL) {
        return BCM_E_RESOURCE;
    }

    _nh_pkt_setup(pkt, cos, vlan, pkt_type, ct_flags);
    _nh_pkt_local_setup(pkt, unit, port, TRUE);

    if (callback != NULL) {
        pkt->cookie = cookie;
        pkt->call_back = nh_tx_callback;
        nh_cookie = _nh_callback_cookie_set(callback);
    } else {
        pkt->call_back = NULL;
    }

    tp = resolve_trans_ptr(unit, port);
    rv = tp->tp_tx(pkt->unit, pkt, nh_cookie);

    if (rv != BCM_E_NONE || callback == NULL) {
        cputrans_tx_pkt_list_free(pkt);
    }

    return rv;
}


/*
 * Function:
 *      nh_pkt_setup
 * Purpose:
 *      Set up a packet or list of packets to be sent using nh_pkt_tx
 * Parameters:
 *      pkt         - Pointer to the packet (or list of packets) to setup
 *      cos         - COS and internal priority on which to send packet
 *      vlan        - VLAN on which to send packet
 *      pkt_type    - For multiplexing on RX end.
 *      ct_flags    - Only CPUTRANS_NO_L2_UPDATE applies
 * Notes:
 *      This is a preliminary setup function used before the
 *      actual (unit, port) is known.
 *
 *      If !(ct_flags & CPUTRANS_NO_L2_UPDATE), then the L2 header
 *      will be updated meaning that the L2 dest MAC address will be set
 *      to the broadcast address.
 *
 *      The 'cos' parameter contains the cos and internal priority
 *      values encoded.  If the internal priority valid bit
 *      CPUTRANS_INT_PRIO_VALID is not set, use the cos value for
 *      internal priority.  Use CPUTRANS_COS_SET() to set cos value and
 *      CPUTRANS_INT_PRIO_SET() to set internal priority.
 */

void
nh_pkt_setup(bcm_pkt_t *pkt, int cos, uint16 vlan,
             uint16 pkt_type, uint32 ct_flags)
{
    bcm_pkt_t *cur_pkt;

    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        _nh_pkt_setup(cur_pkt, cos, vlan, pkt_type, ct_flags);
        cur_pkt->call_back = NULL;
    }
}

/*
 * Function:
 *      nh_pkt_port_setup
 * Purpose:
 *      Set local info in the packet based on the TX (unit, port)
 * Parameters:
 *      pkt         - Pointer to the packet (or list of packets) to setup
 *      unit        - Device on which packet will be sent
 *      port        - Port on unit on which packet will be sent
 */

void
nh_pkt_local_setup(bcm_pkt_t *pkt, int unit, int port)
{
    bcm_pkt_t *cur_pkt;

    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        _nh_pkt_local_setup(cur_pkt, unit, port, TRUE);
    }
}


/*
 * Function:
 *      nh_pkt_final_setup
 * Purpose:
 *      Set final info in the packet based on the TX (unit, port)
 * Parameters:
 *      pkt         - Pointer to the packet (or list of packets) to setup
 *      unit        - Device on which packet will be sent
 *      port        - Port on unit on which packet will be sent
 * Notes:
 *      This is used when the packet is being forwarded and so
 * the next hop info is not updated (to keep the original source
 * info accurate) but, e.g., the SL tag or HG tag may need updating.
 */

void
nh_pkt_final_setup(bcm_pkt_t *pkt, int unit, int port)
{
    bcm_pkt_t *cur_pkt;

    for (cur_pkt = pkt; cur_pkt != NULL; cur_pkt = cur_pkt->next) {
        _nh_pkt_local_setup(cur_pkt, unit, port, FALSE);
    }
}

/*
 * Function:
 *      nh_pkt_tx
 * Purpose:
 *      Transmit a next hop packet from a bcm_pkt_t structure
 * Parameters:
 *      pkt        - Pointer to packet or linked list of packets to send
 *      callback   - If non-null, send async and make callback
 *      cookie     - Cookie for callback.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The caller MUST provide NH_TX_DATA_OFFSET bytes at the beginning
 *      of first data block of each packet in the list for the header.
 *
 *      The pkt->next pointer must be NULL if this is a single packet.
 *      The application is responsible for freeing the packet.
 *
 *      It is assumed that nh_pkt_setup has been called on the pkt or list.
 *      HOWEVER the source mod id/port will be remapped (if set up)
 *      according to the stack port on which the packet is being transmitted.
 */

int
nh_pkt_tx(bcm_pkt_t *pkt, nh_tx_cb_f callback, void *cookie)
{
    bcm_pkt_cb_f local_cb;
    bcm_trans_ptr_t *tp;
    nh_cb_cookie_t *nh_cookie = NULL;

    if (!setup_done) {
        return BCM_E_INIT;
    }

    local_cb = NULL;
    if (callback != NULL) {
        local_cb = nh_tx_callback_nofree;
        nh_cookie = _nh_callback_cookie_set(callback);
    }

    pkt_list_src_mod_id_port_set(pkt);
    tp = resolve_pkt_trans_ptr(pkt);
    return tp->tp_tx_list(pkt->unit, pkt, local_cb, nh_cookie);
}

/* Call back from TX layer for async transmits */
STATIC void
nh_tx_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    nh_tx_cb_f callback;

    _nh_callback_cookie_get(cookie, &callback);
    if (callback) {
        callback(unit, pkt->rx_port, pkt->pkt_data[0].data, pkt->pkt_len,
                 pkt->cookie);
    }

    cputrans_tx_pkt_free(pkt);
}

/* Like above, but don't free the packet */
STATIC void
nh_tx_callback_nofree(int unit, bcm_pkt_t *pkt, void *cookie)
{
    nh_tx_cb_f callback;

    _nh_callback_cookie_get(cookie, &callback);
    if (callback) {
        callback(unit, pkt->rx_port, pkt->pkt_data[0].data, pkt->pkt_len,
                 pkt->cookie);
    }
}

/*
 * Function:
 *      nh_tx_pkt_recognize
 * Purpose:
 *      Boolean:  Check if packet is recognized as next hop pkt
 * Parameters:
 *      pkt_buf    - The start of the packet, from L2 header
 *      local_type - (OUT) Returns local packet type here
 * Returns:
 *      Boolean:  TRUE if packet is next hop packet.
 * Notes:
 *      Does not check source MAC or VLAN.
 */

int
nh_tx_pkt_recognize(uint8 *pkt_buf, uint16 *pkt_type)
{
    uint16 val16;

    if (sal_memcmp(pkt_buf, nh_dest_mac, sizeof(bcm_mac_t))) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NHTX: dest_mac not recognized\n")));
        return FALSE;
    }

    if (sal_memcmp(&pkt_buf[CPUTRANS_SNAP_OFS], nh_snap_mac,
                   sizeof(bcm_mac_t))) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NHTX: snap_mac not recognized\n")));
        return FALSE;
    }

    UNPACK_SHORT(&pkt_buf[CPUTRANS_SNAP_TYPE_OFS], val16);
    if (val16 != nh_snap_type) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NHTX: snap_type not recognized\n")));
        return FALSE;
    }

    UNPACK_SHORT(&pkt_buf[CPUTRANS_TR_TYPE_OFS], val16);
    if (val16 != nh_local_type) {
        LOG_DEBUG(BSL_LS_TKS_NH,
                  (BSL_META("NHTX: Local NH type %d mismatch %d\n"),
                   nh_local_type, val16));
        return FALSE;
    }

    /* Looks good, extract the packet type */
    UNPACK_SHORT(&pkt_buf[CPUTRANS_TR_MULT_OFS], val16);
    *pkt_type = val16;

    return TRUE;
}



/*
 * Function:
 *      nh_tx_reset
 * Purpose:
 *      Reset the next hop layer, freeing allocated structures
 * Parameters:
 *      reset_to_defaults   - Boolean; if TRUE, will also reset
 *                            parameters to default values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
nh_tx_reset(int reset_to_defaults)
{
    if (nh_mutex == NULL) {
        return BCM_E_INIT;
    }

    NH_LOCK;
    if (pending_count) {
        sal_usleep(100000);
        if (pending_count) {
            LOG_INFO(BSL_LS_TKS_NH,
                     (BSL_META("NHTX: %d Packets still pending on reset\n"),
                      pending_count));
            NH_UNLOCK;
            return BCM_E_FAIL;
        }
    }

    if (reset_to_defaults) {
        sal_memcpy(nh_dest_mac, nh_dest_mac_default, sizeof(bcm_mac_t));
        sal_memcpy(nh_snap_mac, nh_snap_mac_default, sizeof(bcm_mac_t));

        nh_snap_type          = NH_TX_SNAP_TYPE_DEFAULT;
        nh_local_type         = NH_TX_LOCAL_PKT_TYPE;
        nh_dest_port          = NH_TX_DEST_PORT_DEFAULT;
        nh_dest_mod           = NH_TX_DEST_MOD_DEFAULT;
        nh_src_port           = NH_TX_SRC_PORT_DEFAULT;
        nh_src_mod            = NH_TX_SRC_MOD_DEFAULT;
    }
    NH_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *      nh_tx_local_mac_set
 * Purpose:
 *      Set the local mac address used for source
 * Parameters:
 *      new_mac   - MAC to use
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
nh_tx_local_mac_set(const bcm_mac_t new_mac)
{
    sal_memcpy(nh_local_mac, new_mac, sizeof(bcm_mac_t));

    return BCM_E_NONE;
}


/*
 * Function:
 *      nh_tx_local_mac_get
 * Purpose:
 *      Get the local mac address used for source
 * Parameters:
 *      local_mac   - (OUT) Local MAC being used
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
nh_tx_local_mac_get(bcm_mac_t *local_mac)
{
    sal_memcpy(local_mac, nh_local_mac, sizeof(bcm_mac_t));

    return BCM_E_NONE;
}


/*
 * Function:
 *      nh_tx_snap_set
 * Purpose:
 *      Set the SNAP mac and type ID
 * Parameters:
 *      new_mac   - MAC to use
 *      new_type  - Type ID to use; host order
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      It defaults to Broadcom, so generally not required
 */

int
nh_tx_snap_set(const bcm_mac_t new_mac, uint16 new_type,
               uint16 new_local_type)
{
    sal_memcpy(nh_snap_mac, new_mac, sizeof(bcm_mac_t));
    nh_snap_type = new_type;
    nh_local_type = new_local_type;

    return BCM_E_NONE;
}


/*
 * Function:
 *      nh_tx_snap_get
 * Purpose:
 *      Get the SNAP mac and type ID
 * Parameters:
 *      snap_mac   - (OUT) MAC to use
 *      snap_type  - (OUT) Type ID to use; host order
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
nh_tx_snap_get(bcm_mac_t snap_mac, uint16 *snap_type, uint16 *local_type)
{
    sal_memcpy(snap_mac, nh_snap_mac, sizeof(bcm_mac_t));
    *snap_type = nh_snap_type;
    *local_type = nh_local_type;

    return BCM_E_NONE;
}

/*
 * Function:
 *      nh_tx_unknown_modid_get
 * Purpose:
 *      Gets nh_tx_unknown_modid
 * Parameters:
 *      modid  - (OUT) nh_tx_unknown_modid value
 */
int
nh_tx_unknown_modid_get(int *modid)
{
    *modid = nh_tx_unknown_modid;
    return BCM_E_NONE;
}

/*
 * Function:
 *      nh_tx_unknown_modid_set
 * Purpose:
 *      Sets nh_tx_unknown_modid
 * Parameters:
 *      modid  - Modid to use as nh_tx_unknown_modid
 */
int
nh_tx_unknown_modid_set(int modid)
{
    nh_tx_unknown_modid = modid;
    return BCM_E_NONE;
}

/*
 * Function:
 *      nh_tx_l2_header_setup
 * Purpose:
 *      Set up the L2 header for next hop packet
 * Parameters:
 *      pkt_buf     - The L2 header buffer to set up
 */

void
nh_tx_l2_header_setup(uint8 *pkt_buf, uint16 vlan)
{
    _nh_tx_l2_header_setup(pkt_buf, vlan);
}


/*
 * Function:
 *      _nh_tx_l2_header_setup
 * Purpose:
 *      See above; 
 * Parameters:
 *      pkt_buf     - The L2 header buffer to set up
 */

STATIC void
_nh_tx_l2_header_setup(uint8 *pkt_buf, uint16 vlan)
{
    /* Set up the L2 header in the packet */
    sal_memcpy(&pkt_buf[CPUTRANS_DMAC_OFS], nh_dest_mac, sizeof(bcm_mac_t));
    sal_memcpy(&pkt_buf[CPUTRANS_SMAC_OFS], nh_local_mac, sizeof(bcm_mac_t));
    PACK_SHORT(&pkt_buf[CPUTRANS_VTAG_OFS], 0x8100);
    PACK_SHORT(&pkt_buf[CPUTRANS_VTAG_OFS + sizeof(uint16)], vlan);
    /* Type/len field is skipped */
    sal_memcpy(&pkt_buf[CPUTRANS_SNAP_OFS], nh_snap_mac, sizeof(bcm_mac_t));
    PACK_SHORT(&pkt_buf[CPUTRANS_SNAP_TYPE_OFS], nh_snap_type);
    PACK_SHORT(&pkt_buf[CPUTRANS_TR_TYPE_OFS], nh_local_type);
}


/* Initialize next hop layer. Can only be called once, so use NH_INIT macro */

STATIC int
nh_init(void)
{
    int i;

    if (nh_mutex == NULL) {
        nh_mutex = sal_mutex_create("nh_tx_mutex");
        if (nh_mutex == NULL) {
            return BCM_E_MEMORY;
        }
    }

    NH_LOCK;
    /* Initialize stack port structures */
    for (i = 0; i < STK_PORTS_MAX; i++) {
        nh_stk_ports[i].trans_ptr = NULL;
        nh_stk_ports[i].unit = -1;
        nh_stk_ports[i].port = -1;
    }

    init_done = TRUE;
    NH_UNLOCK;
    return BCM_E_NONE;
}
