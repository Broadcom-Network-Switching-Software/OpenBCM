 /*
  * 
  *
  * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
  * 
  * Copyright 2007-2020 Broadcom Inc. All rights reserved.
  *
  * File:        ct_tun.c
  * Purpose:
  * Requires:    
  */

/****************************************************************
 *
 * Packet RX tunnelling code:
 *
 * This code originally lived in BCM.  It has been moved to
 * be an ATP application and live in src/appl/cputrans.
 *
 * Tunnelling is meant to forward packets between two different
 * CPUs.  However,
 * the function that actually forwards the packet data is programmed
 * into this API with the ct_rx_tunnel_tx_set call.  Having both
 * tunnel send and tunnel accept running on the same CPU probably will
 * not work.
 *
 * Reliable and best effort tunnelling are both supported.
 *
 * The code is divided into that which runs on the slave (most of
 * the code) and that which runs on the master (just the unpack
 * routine).
 *
 * A local callback may be installed (tunnel_filter) which will be called
 * as a filter on packets that may be tunnelled.  The filter may
 * return:
 *
 *      BCM_CPU_TUNNEL_NONE                 Do not tunnel the packet
 *      BCM_CPU_TUNNEL_PACKET_RELIABLE      Use reliable trx
 *      BCM_CPU_TUNNEL_PACKET_BEST_EFFORT   Use best effort
 *      BCM_CPU_TUNNEL_PACKET               Use the current default mode
 *
 * The default is BCM_CPU_TUNNEL_PACKET. Otherwise, BCM_RX_NOT_HANDLED
 * is returned to the RX thread.
 *
 * If no filter function is installed, then all packets received by the
 * tunnel routine are forwarded.
 *
 * Only one set of data is supported across all local units.  This
 * includes the filter function, the filter cookie (if implemented),
 * and the TX functions.  Note that receive unit is sent to the callback.
 *
 * Packets are queued to a tunneling thread, which then transmits the
 * packet.
 *
 * The current implementation does alloc/frees, so will be very
 * slow.  Packet pools may be added in the future.
 * For now, BCM_RX_HANDLED is returned when tunnelled.  If a queue and
 * separate thread are added in the future, packets will be stolen
 * from BCM RX (BCM_RX_HANDLED_OWNED is returned).
 *
 * TO DO:
 *      Use RX pool code for tunnelled packet data.  See
 *      rx_tunnel_pkt and ct_rx_tunnel_done_cb.
 */
#include <shared/bsl.h>

#include <sdk_config.h>
#include <assert.h>
#include <sal/core/thread.h>

#include <shared/util.h>
#include <shared/idents.h>

#include <bcm/rx.h>
#include <bcm/init.h>
#include <bcm/port.h>

#include <appl/cputrans/ct_tun.h>

#include "t_util.h"

#if defined(BCM_RPC_SUPPORT)

#include <appl/cputrans/atp.h>

/* Filter function callback */
STATIC bcm_cpu_tunnel_mode_t
    _ct_rx_tunnel_filter_default(int unit, bcm_pkt_t *pkt);
static ct_rx_tunnel_filter_f ct_rx_tunnel_filter = _ct_rx_tunnel_filter_default;

static sal_mutex_t _tunnel_cfg_lock;

/* Tunnel TX queueing */

#ifndef CT_TUNNEL_QUEUE_ENABLE
#define CT_TUNNEL_QUEUE_ENABLE 1
#endif

#if CT_TUNNEL_QUEUE_ENABLE

#ifndef CT_TUNNEL_QUEUE_SIZE
#define CT_TUNNEL_QUEUE_SIZE 32
#endif

typedef struct {
    uint8 *data;
    int len;
    void *pkt;
} ct_tunnel_q_element;

static sal_thread_t _tunnel_q_thread = SAL_THREAD_ERROR;
static int          _tunnel_q_top_full;  /* Pkts dropped when queue full */
static int          _tunnel_q_thread_exit; /* Set to exit thread */
static int          _tunnel_q_error;     /* TX error count */
static sal_mutex_t  _tunnel_q_lock;      /* Queue lock */
static sal_sem_t    _tunnel_q_sem;       /* Queue thread semaphore */
static int          _tunnel_q_head;      /* Insertion index */
static int          _tunnel_q_tail;      /* Removal index */
static ct_tunnel_q_element _tunnel_q[CT_TUNNEL_QUEUE_SIZE]; /* Queue */

#define TUNNEL_Q_LOCK   sal_mutex_take(_tunnel_q_lock, sal_mutex_FOREVER)
#define TUNNEL_Q_UNLOCK sal_mutex_give(_tunnel_q_lock)
#define TUNNEL_Q_SLEEP  sal_sem_take(_tunnel_q_sem, sal_sem_FOREVER)
#define TUNNEL_Q_WAKE   sal_sem_give(_tunnel_q_sem)

#ifndef TUNNEL_Q_THREAD_STACK
#define TUNNEL_Q_THREAD_STACK   SAL_THREAD_STKSZ
#endif

#ifndef TUNNEL_Q_THREAD_PRIO
#define TUNNEL_Q_THREAD_PRIO    255
#endif

STATIC int ct_tx_tunnel_thread_init(void);
STATIC int ct_tx_tunnel_thread_stop(void);

#endif

#define CHECK_INIT if (_tunnel_cfg_lock == NULL &&                       \
        ((_tunnel_cfg_lock = sal_mutex_create("rx_tunnel")) == NULL))    \
        return BCM_E_MEMORY

#define TUNNEL_LOCK sal_mutex_take(_tunnel_cfg_lock, sal_mutex_FOREVER)
#define TUNNEL_UNLOCK sal_mutex_give(_tunnel_cfg_lock)

/* These are exposed */
static bcm_cpu_tunnel_mode_t ct_rx_tunnel_mode_default =
        BCM_CPU_TUNNEL_PACKET_BEST_EFFORT;

static int ct_rx_tunnel_priority = 100;   /* Warning: Don't change when running */

/* Accessors for mode and priority */
int
ct_rx_tunnel_mode_default_set(int mode)
{
    switch (mode) {
    case BCM_CPU_TUNNEL_NONE:
    case BCM_CPU_TUNNEL_PACKET_RELIABLE:
    case BCM_CPU_TUNNEL_PACKET_BEST_EFFORT:
    case BCM_CPU_TUNNEL_PACKET:
        ct_rx_tunnel_mode_default = (bcm_cpu_tunnel_mode_t)mode;
        return BCM_E_NONE;

    default:
        break;
    }

    return BCM_E_PARAM;
}

bcm_cpu_tunnel_mode_t
ct_rx_tunnel_mode_default_get(void)
{
    return ct_rx_tunnel_mode_default;
}

int
ct_rx_tunnel_priority_set(int priority)
{
    ct_rx_tunnel_priority = priority;

    return BCM_E_NONE;
}

int
ct_rx_tunnel_priority_get(void)
{
    return ct_rx_tunnel_priority;
}

#define CT_TUNNEL_RX_REASONS_WORDS (_SHR_BITDCLSIZE(_SHR_RX_REASON_COUNT))
#define CT_TUNNEL_RX_HDR_BYTES (2 + 4 + (6*1) + 2 + 1 + 2 + (2*1) + (2*4) + \
                                2 + (2*1) + (CT_TUNNEL_RX_REASONS_WORDS * 4)\
                                 + (2*4) + 4 + \
                                2 + (2*1) + 2 + (2*1) + (15*4) + (2*4))

/****************************************************************
 *
 * Forward declarations
 */

STATIC bcm_rx_t ct_tx_tunnelled_pkt_handler(cpudb_key_t src_key,
                                            int client_id,
                                            bcm_pkt_t *pkt,
                                            uint8 *payload,
                                            int payload_len,
                                            void *cookie);

/*
 * Function:
 *      ct_rx_tunnel_header_bytes_get
 * Purpose:
 *      Returns the number of bytes in a CT tunneled packet
 * Returns:
 *      BCM_E_XXX on error (unsupported)
 *      Otherwise, number of bytes in the header
 * Notes:
 *      Needed by RPC rlink code
 */

int
ct_rx_tunnel_header_bytes_get(void)
{
    return CT_TUNNEL_RX_HDR_BYTES;
}


/*
 * Tunnel pack and unpack functions
 *
 * These pack up the meta-data; for the pack function the data
 * is copied into the buffer as well.  For unpack, the pointers
 * to the data are provided, but the data is not copied from the
 * original buffer.
 */

uint8 *
ct_rx_tunnel_pkt_pack(bcm_pkt_t *pkt, uint8 *pkt_data)
{
    int i;

#if defined(BROADCOM_DEBUG)
    uint8 *pkt_data0 = pkt_data;
#endif
    /* Change CT_TUNNEL_RX_HDR_BYTES when the packing changes. */
    CT_INCR_PACK_U16(pkt_data, pkt->pkt_len);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_reason);
    CT_INCR_PACK_U8(pkt_data, pkt->rx_unit);
    CT_INCR_PACK_U8(pkt_data, pkt->rx_port);
    CT_INCR_PACK_U8(pkt_data, pkt->rx_cpu_cos);
    CT_INCR_PACK_U8(pkt_data, pkt->rx_untagged);
    CT_INCR_PACK_U8(pkt_data, pkt->cos);
    CT_INCR_PACK_U8(pkt_data, pkt->prio_int);
    CT_INCR_PACK_U16(pkt_data, pkt->src_port);
    CT_INCR_PACK_U8(pkt_data, pkt->src_mod & 0xFF);
    CT_INCR_PACK_U16(pkt_data, pkt->dest_port);
    CT_INCR_PACK_U8(pkt_data, pkt->dest_mod & 0xFF);
    CT_INCR_PACK_U8(pkt_data, pkt->opcode);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_matched);
    CT_INCR_PACK_U32(pkt_data, pkt->flags);
    CT_INCR_PACK_U16(pkt_data, pkt->src_trunk);

    /* to maintain backwards compatibility, pack the extented src & dest mod
     * bits at the end of the buffer
     */
    CT_INCR_PACK_U8(pkt_data, (pkt->src_mod >> 8) & 0xFF);
    CT_INCR_PACK_U8(pkt_data, (pkt->dest_mod >> 8) & 0xFF);

    /* rx_reasons rx_classification_tag rx_timestamp */
    for (i=0; i<CT_TUNNEL_RX_REASONS_WORDS; i++) {
        CT_INCR_PACK_U32(pkt_data, (uint32)pkt->rx_reasons.pbits[i]);
    }
    CT_INCR_PACK_U32(pkt_data, pkt->rx_classification_tag);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_timestamp);

    for (i=0; i<4; i++) {
        CT_INCR_PACK_U8(pkt_data, pkt->_vtag[i]);
    }

    CT_INCR_PACK_U16(pkt_data, pkt->vlan);
    CT_INCR_PACK_U8(pkt_data, pkt->vlan_pri);
    CT_INCR_PACK_U8(pkt_data, pkt->vlan_cfi);
    CT_INCR_PACK_U16(pkt_data, pkt->inner_vlan);
    CT_INCR_PACK_U8(pkt_data, pkt->inner_vlan_pri);
    CT_INCR_PACK_U8(pkt_data, pkt->inner_vlan_cfi);
    CT_INCR_PACK_U32(pkt_data, pkt->color);
    CT_INCR_PACK_U32(pkt_data, pkt->dst_gport);
    CT_INCR_PACK_U32(pkt_data, pkt->src_gport);
    CT_INCR_PACK_U32(pkt_data, pkt->multicast_group);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_flags);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_forward);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_classification_tag);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_pkt_prio);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_dscp);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_load_balancing_number);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_timestamp_upper);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_l3_intf);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_outer_tag_action);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_inner_tag_action);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_encap_id);

    CT_INCR_PACK_U32(pkt_data, pkt->flags2);

    /* OAM SOMBH header fields */
    CT_INCR_PACK_U8(pkt_data, pkt->oam_replacement_type);
    CT_INCR_PACK_U8(pkt_data, pkt->oam_replacement_offset);
    CT_INCR_PACK_U16(pkt_data, pkt->oam_lm_counter_index);

#if defined(BROADCOM_DEBUG)
    assert((pkt_data - pkt_data0) == CT_TUNNEL_RX_HDR_BYTES);
#endif

    sal_memcpy(pkt_data, pkt->pkt_data[0].data, pkt->pkt_len);


    return pkt_data + pkt->pkt_len;
}

int
ct_rx_tunnel_pkt_unpack(uint8 *pkt_data, int pkt_len, bcm_pkt_t *pkt,
                        uint8 **payload_start, int *payload_len)
{
    uint8 tmp;
    int i;

    if (pkt_len < CT_TUNNEL_RX_HDR_BYTES) {
        return BCM_E_PARAM;
    }

    CT_INCR_UNPACK_U16(pkt_data, pkt->pkt_len);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_reason);
    CT_INCR_UNPACK_U8(pkt_data, pkt->rx_unit);
    CT_INCR_UNPACK_U8(pkt_data, pkt->rx_port);
    CT_INCR_UNPACK_U8(pkt_data, pkt->rx_cpu_cos);
    CT_INCR_UNPACK_U8(pkt_data, pkt->rx_untagged);
    CT_INCR_UNPACK_U8(pkt_data, pkt->cos);
    CT_INCR_UNPACK_U8(pkt_data, pkt->prio_int);
    CT_INCR_UNPACK_U16(pkt_data, pkt->src_port);
    CT_INCR_UNPACK_U8(pkt_data, pkt->src_mod);
    CT_INCR_UNPACK_U16(pkt_data, pkt->dest_port);
    CT_INCR_UNPACK_U8(pkt_data, pkt->dest_mod);
    CT_INCR_UNPACK_U8(pkt_data, pkt->opcode);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_matched);
    CT_INCR_UNPACK_U32(pkt_data, pkt->flags);
    CT_INCR_UNPACK_U16(pkt_data, pkt->src_trunk);

    /* to maintain backwards compatibility, pack the extented src & dest mod
     * bits at the end of the buffer
     */
    CT_INCR_UNPACK_U8(pkt_data, tmp);
    pkt->src_mod |= tmp << 8;

    CT_INCR_UNPACK_U8(pkt_data, tmp);
    pkt->dest_mod |= tmp << 8;

    for (i=0; i<CT_TUNNEL_RX_REASONS_WORDS; i++) {
        CT_INCR_UNPACK_U32(pkt_data, pkt->rx_reasons.pbits[i]);
    }
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_classification_tag);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_timestamp);

    for (i=0; i<4; i++) {
        CT_INCR_UNPACK_U8(pkt_data, pkt->_vtag[i]);
    }

    CT_INCR_UNPACK_U16(pkt_data, pkt->vlan);
    CT_INCR_UNPACK_U8(pkt_data, pkt->vlan_pri);
    CT_INCR_UNPACK_U8(pkt_data, pkt->vlan_cfi);
    CT_INCR_UNPACK_U16(pkt_data, pkt->inner_vlan);
    CT_INCR_UNPACK_U8(pkt_data, pkt->inner_vlan_pri);
    CT_INCR_UNPACK_U8(pkt_data, pkt->inner_vlan_cfi);
    CT_INCR_UNPACK_U32(pkt_data, pkt->color);
    CT_INCR_UNPACK_U32(pkt_data, pkt->dst_gport);
    CT_INCR_UNPACK_U32(pkt_data, pkt->src_gport);
    CT_INCR_UNPACK_U32(pkt_data, pkt->multicast_group);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_flags);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_forward);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_classification_tag);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_pkt_prio);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_dscp);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_load_balancing_number);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_timestamp_upper);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_l3_intf);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_outer_tag_action);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_inner_tag_action);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_encap_id);

    CT_INCR_UNPACK_U32(pkt_data, pkt->flags2);

    /* OAM SOMBH header fields */
    CT_INCR_UNPACK_U8(pkt_data, pkt->oam_replacement_type);
    CT_INCR_UNPACK_U8(pkt_data, pkt->oam_replacement_offset);
    CT_INCR_UNPACK_U16(pkt_data, pkt->oam_lm_counter_index);

    *payload_start = pkt_data;
    *payload_len = pkt_len - CT_TUNNEL_RX_HDR_BYTES;

    return BCM_E_NONE;
}


static ct_rx_tunnel_direct_f _rx_tunnel_direct = NULL;

int
ct_rx_tunnel_direct_set(ct_rx_tunnel_direct_f dif_fn)
{
    _rx_tunnel_direct = dif_fn;

    return BCM_E_NONE;
}

int
ct_rx_tunnel_direct_get(ct_rx_tunnel_direct_f *dif_fn)
{
    *dif_fn = _rx_tunnel_direct;

    return BCM_E_NONE;
}

/*
 * ct_rx_tunnel_check:
 *
 * Check if a packet should be tunnelled; if so, see if it should
 * be directed to some particular CPU.
 *
 * Returns BCM_CPU_TUNNEL_NONE if packet should _not_ be tunnelled;
 * Any other value means packet should be tunnelled.
 */

bcm_cpu_tunnel_mode_t
ct_rx_tunnel_check(int unit, bcm_pkt_t *pkt, int *no_ack, int *check_cpu,
                   cpudb_key_t *cpu)
{
    bcm_cpu_tunnel_mode_t mode = BCM_CPU_TUNNEL_PACKET;

    if (ct_rx_tunnel_filter != NULL) {
        mode = ct_rx_tunnel_filter(unit, pkt);
    }

    switch (mode) {
    case BCM_CPU_TUNNEL_PACKET_RELIABLE:
        *no_ack = FALSE;
        break;
    case BCM_CPU_TUNNEL_PACKET_BEST_EFFORT:
        *no_ack = TRUE;
        break;
    case BCM_CPU_TUNNEL_PACKET:  /* Check if default is currently no-ack */
        *no_ack = (ct_rx_tunnel_mode_default ==
                   BCM_CPU_TUNNEL_PACKET_BEST_EFFORT);
        break;
    case BCM_CPU_TUNNEL_NONE:  /* Don't tunnel */
    default:    /* Bad mode; treat as none. */
        return BCM_CPU_TUNNEL_NONE;

    }

    if (_rx_tunnel_direct != NULL) {
        *check_cpu = _rx_tunnel_direct(unit, pkt, cpu);
    } else {
        *check_cpu = FALSE;
    }

    return mode;
}


/****************************************************************
 *
 * RX filter function accessors and default.
 *
 * Default tunnel filter callback.  This filters out packets
 * that come from stack ports.
 */

STATIC bcm_cpu_tunnel_mode_t
_ct_rx_tunnel_filter_default(int unit, bcm_pkt_t *pkt)
{
    bcm_port_config_t port_config;

    if (bcm_port_config_get(unit, &port_config) < 0) {
        LOG_ERROR(BSL_LS_TKS_TUNNEL,
                  (BSL_META_U(unit,
                              "Tunnel filter: could not get port config "
                              " for %d\n"),
                   unit));
        return BCM_CPU_TUNNEL_NONE;
    }

    if (BCM_PBMP_MEMBER(port_config.stack_ext, pkt->rx_port)) {
        return BCM_CPU_TUNNEL_NONE;
    }

    return BCM_CPU_TUNNEL_PACKET;
}


/*
 * Function:
 *      ct_rx_tunnel_filter_set/get
 * Purpose:
 *      Set/get the RX tunnel filter function
 * Parameters:
 *      t_cb       - The function to use.  May be NULL.
 * Returns:
 *      BCM_E_NONE
 */

int
ct_rx_tunnel_filter_set(ct_rx_tunnel_filter_f t_cb)
{
    CHECK_INIT;
    TUNNEL_LOCK;
    ct_rx_tunnel_filter = t_cb;
    TUNNEL_UNLOCK;

    return BCM_E_NONE;
}


int
ct_rx_tunnel_filter_get(ct_rx_tunnel_filter_f *t_cb)
{
    *t_cb = ct_rx_tunnel_filter;

    return BCM_E_NONE;
}

/****************************************************************
 *
 * Client Side RX tunnelling code.  Handle a tunnel (encapsulated)
 * packet.
 */


/*
 * Function:
 *      ct_rx_tunnelled_pkt_handler
 * Purpose:
 *      Handler for RX tunnelled packets,
 * Parameters:
 *      src_key     - Source of tunnelled pkt
 *      payload     - Pointer to start of tunnelled encap pkt
 *      len         - Total length of pkt with encap header
 * Returns:
 *      BCM_E_XXX
 */

void
ct_rx_tunnelled_pkt_handler(cpudb_key_t src_key, uint8 *payload, int len)
{
    uint8 *ap_data;
    int ap_len;
    bcm_pkt_t *rx_pkt;
    int rv;
    int bcm_unit;
    char keybuf[CPUDB_KEY_STRING_LEN];

    LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                (BSL_META("Tunnel handler from " CPUDB_KEY_FMT_EOLN),
                 CPUDB_KEY_DISP(src_key)));

    if (payload == NULL || len < CT_TUNNEL_RX_HDR_BYTES) {
        LOG_WARN(BSL_LS_TKS_TUNNEL,
                 (BSL_META("Tunnel RX pkt: no payload (%p, len %d)\n"),
                  payload, len - CT_TUNNEL_RX_HDR_BYTES));
        return;
    }

    rx_pkt = NULL;
    rv = bcm_rx_remote_pkt_alloc(len - CT_TUNNEL_RX_HDR_BYTES, &rx_pkt);
    if (rv != BCM_E_NONE) {
        LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                    (BSL_META("Tunnel RX pkt: remote pkt alloc failed "
                              "for %d bytes\n"),
                     len - CT_TUNNEL_RX_HDR_BYTES));
        return;
    }

    rv = ct_rx_tunnel_pkt_unpack(payload, len, rx_pkt, &ap_data, &ap_len);

    if (rv < 0) {
        LOG_ERROR(BSL_LS_TKS_TUNNEL,
                  (BSL_META("Tunnel RX pkt: unpack failed\n")));
        bcm_rx_remote_pkt_free(rx_pkt);
        return;
    }

    rx_pkt->flags |= BCM_RX_TUNNELLED;

    /* Copy the data into the packet */
    sal_memcpy(rx_pkt->_pkt_data.data, ap_data, ap_len);

    /* Map the cpu/unit to the BCM unit number */
    cpudb_key_format(src_key, keybuf, CPUDB_KEY_STRING_LEN);
    bcm_unit = bcm_find("client", keybuf, rx_pkt->rx_unit);
    if (bcm_unit < 0) {
        LOG_INFO(BSL_LS_TKS_TUNNEL,
                 (BSL_META("Tunnel RX pkt: unit/cpu not found: %s.%d;"
                           CPUDB_KEY_FMT_EOLN),
                  keybuf, rx_pkt->rx_unit,
                  CPUDB_KEY_DISP(src_key)));
        bcm_rx_remote_pkt_free(rx_pkt);
        return;
    }
    /* Insert the packet into the RX queues */

    /* Overwrite unit number with locally meaningful unit number */
    rx_pkt->rx_unit = bcm_unit;

    LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                (BSL_META("Tunnel handler bcm_u %d. u %d p %d. sm %d "
                          "sp %d cos %d prio_int %d.\n"),
                 bcm_unit, rx_pkt->rx_unit, rx_pkt->rx_port,
                 rx_pkt->src_mod, rx_pkt->src_port, rx_pkt->cos,
                 rx_pkt->prio_int));

    rv = bcm_rx_remote_pkt_enqueue(bcm_unit, rx_pkt);
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_TUNNEL,
                 (BSL_META("Tunnel RX pkt: rx enqueue failed %d: %s\n"),
                  rv, bcm_errmsg(rv)));
        bcm_rx_remote_pkt_free(rx_pkt);
    }

    return;
}




/****************************************************************
 *
 * Packet TX tunnelling code
 *
 * TX Tunnelling is meant to forward packets from the master to slave
 * units only.  Again, the transmit function is programmable.
 *
 * Reliable and best effort tunnelling are both supported.
 *
 * The code is divided into that which runs on the slave and that
 * which runs on the master.
 *
 *      BCM_CPU_TUNNEL_PACKET_RELIABLE      Use reliable trx
 *      BCM_CPU_TUNNEL_PACKET_BEST_EFFORT   Use best effort
 *      BCM_CPU_TUNNEL_PACKET               Use best effort if present;
 *                                      otherwise use reliable.
 *
 * The current implementation does alloc/frees, so will be very
 * slow.  Packet pools may be added in the future.
 */


/****************************************************************
 *
 * TUNNEL TX CODE, SOURCE (master)
 */

/* Function called back after TX */
STATIC void
ct_tx_tunnel_done_cb(uint8 *pkt_data, void *cookie, int rv)
{
    bcm_pkt_t *pkt = (bcm_pkt_t *)cookie;

    /* allocated in ct_tx_tunnel */
    atp_tx_data_free(pkt_data);
    if (pkt != NULL && pkt->call_back != NULL) {
        pkt->call_back(pkt->unit, pkt, NULL);
    }
}

/* Bytes in a packed port bitmap */
#define PBMP_BYTES (4 * _SHR_PBMP_WORD_MAX)

#define TUNNEL_TX_HEADER_BYTES \
        (2 + (2*1) + 2 + 1 + 2 + (2*1) + 4 + 1 + 1 + 4 + \
         (2 * PBMP_BYTES) + 1 + 1 + 4 + 2 + (13*4) + (2*4))

int
ct_tx_tunnel_setup(void)
{
    int rv;

#if CT_TUNNEL_QUEUE_ENABLE
    rv = ct_tx_tunnel_thread_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_TKS_TUNNEL,
                  (BSL_META("Could not init tunnel thread %d: %s"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }
#endif

    rv = atp_register(
        SHARED_CLIENT_ID_TUNNEL_TX_PKT,
        ATP_F_REASSEM_BUF,
        ct_tx_tunnelled_pkt_handler, NULL, -1, -1);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_TKS_TUNNEL,
                  (BSL_META("Tunnel: Failed to register for tx tunnel %d: %s\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    rv = atp_register(
        SHARED_CLIENT_ID_TUNNEL_TX_PKT_NO_ACK,
        ATP_F_NO_ACK | ATP_F_REASSEM_BUF,
        ct_tx_tunnelled_pkt_handler, NULL, -1, -1);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_TKS_TUNNEL,
                  (BSL_META("Tunnel: Failed to register for tx best effort %d: %s\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    /* Install the CT TX tunnel routine to the BCM layer */
    rv = bcm_tx_cpu_tunnel_set(ct_tx_tunnel);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_TKS_TUNNEL,
                  (BSL_META("Tunnel: Failed to register tunnel TX "
                            "with BCM layer %d: %s\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    return BCM_E_NONE;
}

int
ct_tx_tunnel_stop(void)
{
    int rv;

    /* Unregister hook */
    rv = bcm_tx_cpu_tunnel_set(NULL);

    /* Unregister ATP */
    atp_unregister(SHARED_CLIENT_ID_TUNNEL_TX_PKT);
    atp_unregister(SHARED_CLIENT_ID_TUNNEL_TX_PKT_NO_ACK);

#if CT_TUNNEL_QUEUE_ENABLE
    /* Stop thread */
    rv = ct_tx_tunnel_thread_stop();
#endif

    return rv;
}

/* Pack/unpack TX tunnelled header data; return the updated data pointer */
STATIC uint8 *
ct_tx_tunnel_pack(uint8 *pkt_data, bcm_pkt_t *pkt, int dest_unit,
                  int remote_port, uint32 flags, int dlen)
{
    uint32 word;
    int i;
    int remunit;
#if defined(BROADCOM_DEBUG)
    uint8 *pkt_data0 = pkt_data;
#endif

    /* Change TUNNEL_TX_HEADER_BYTES when the packing changes. */
    CT_INCR_PACK_U16(pkt_data, dlen);
    CT_INCR_PACK_U8(pkt_data, pkt->cos);
    CT_INCR_PACK_U8(pkt_data, pkt->prio_int);
    CT_INCR_PACK_U16(pkt_data, pkt->src_port);
    CT_INCR_PACK_U8(pkt_data, pkt->src_mod & 0xFF);
    CT_INCR_PACK_U16(pkt_data, pkt->dest_port);
    CT_INCR_PACK_U8(pkt_data, pkt->dest_mod & 0xFF);
    CT_INCR_PACK_U8(pkt_data, pkt->opcode);
    CT_INCR_PACK_U32(pkt_data, pkt->flags);

    (void)bcm_unit_remote_unit_get(dest_unit, &remunit);
    CT_INCR_PACK_U8(pkt_data, remunit);
    CT_INCR_PACK_U8(pkt_data, remote_port);
    CT_INCR_PACK_U32(pkt_data, flags);

    if (flags & BCM_CPU_TUNNEL_F_PBMP) {
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            word = _SHR_PBMP_WORD_GET(pkt->tx_pbmp, i);
            _SHR_PACK_LONG(pkt_data, word);
            pkt_data += 4;
        }
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            word = _SHR_PBMP_WORD_GET(pkt->tx_upbmp, i);
            _SHR_PACK_LONG(pkt_data, word);
            pkt_data += 4;
        }
    }

    /* to maintain backwards compatibility, pack the extented src & dest mod
     * bits at the end of the buffer
     */
    CT_INCR_PACK_U8(pkt_data, (pkt->src_mod >> 8) & 0xFF);
    CT_INCR_PACK_U8(pkt_data, (pkt->dest_mod >> 8) & 0xFF);
    
    for (i=0; i<4; i++) {
        CT_INCR_PACK_U8(pkt_data, pkt->_vtag[i]);
    }
    
    CT_INCR_PACK_U16(pkt_data, pkt->vlan);
    CT_INCR_PACK_U32(pkt_data, pkt->color);
    CT_INCR_PACK_U32(pkt_data, pkt->dst_gport);
    CT_INCR_PACK_U32(pkt_data, pkt->src_gport);
    CT_INCR_PACK_U32(pkt_data, pkt->multicast_group);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_flags);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_forward);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_classification_tag);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_pkt_prio);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_dscp);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_load_balancing_number);
    CT_INCR_PACK_U32(pkt_data, pkt->rx_timestamp_upper);
    CT_INCR_PACK_U32(pkt_data, pkt->timestamp_flags);
    CT_INCR_PACK_U32(pkt_data, pkt->stk_encap_id);

    CT_INCR_PACK_U32(pkt_data, pkt->flags2);

    /* OAM SOMBH header fields */
    CT_INCR_PACK_U8(pkt_data, pkt->oam_replacement_type);
    CT_INCR_PACK_U8(pkt_data, pkt->oam_replacement_offset);
    CT_INCR_PACK_U16(pkt_data, pkt->oam_lm_counter_index);

#if defined(BROADCOM_DEBUG)
    assert((pkt_data - pkt_data0) == TUNNEL_TX_HEADER_BYTES);
#endif

    return pkt_data;
}

STATIC uint8 *
ct_tx_tunnel_unpack(uint8 *pkt_data, int len, bcm_pkt_t *pkt, int *pkt_len_p,
                    int *local_unit, int *local_port, uint32 *flags)
{
    int i;
    uint32 word;
    uint8 tmp;
    int pkt_len;

    CT_INCR_UNPACK_U16(pkt_data, pkt_len);
    if (len < pkt_len + TUNNEL_TX_HEADER_BYTES) {
        return NULL;   /* Not enough bytes in packet */
    }

    *pkt_len_p = pkt_len;
    CT_INCR_UNPACK_U8(pkt_data, pkt->cos);
    CT_INCR_UNPACK_U8(pkt_data, pkt->prio_int);
    CT_INCR_UNPACK_U16(pkt_data, pkt->src_port);
    CT_INCR_UNPACK_U8(pkt_data, pkt->src_mod);
    CT_INCR_UNPACK_U16(pkt_data, pkt->dest_port);
    CT_INCR_UNPACK_U8(pkt_data, pkt->dest_mod);
    CT_INCR_UNPACK_U8(pkt_data, pkt->opcode);
    CT_INCR_UNPACK_U32(pkt_data, pkt->flags);
    CT_INCR_UNPACK_U8(pkt_data, *local_unit);
    CT_INCR_UNPACK_U8(pkt_data, *local_port);
    CT_INCR_UNPACK_U32(pkt_data, *flags);


    if (*flags & BCM_CPU_TUNNEL_F_PBMP) {
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            _SHR_UNPACK_LONG(pkt_data, word);
            pkt_data += 4;
            _SHR_PBMP_WORD_SET(pkt->tx_pbmp, i, word);
        }
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            _SHR_UNPACK_LONG(pkt_data, word);
            pkt_data += 4;
            _SHR_PBMP_WORD_SET(pkt->tx_upbmp, i, word);
        }
    }

    /* to maintain backwards compatibility, unpack the extented src & dest mod
     * bits at the end of the buffer
     */
    CT_INCR_UNPACK_U8(pkt_data, tmp);
    pkt->src_mod |= tmp << 8;

    CT_INCR_UNPACK_U8(pkt_data, tmp);
    pkt->dest_mod |= tmp << 8;

    for (i=0; i<4; i++) {
        CT_INCR_UNPACK_U8(pkt_data, pkt->_vtag[i]);
    }

    CT_INCR_UNPACK_U16(pkt_data, pkt->vlan);
    CT_INCR_UNPACK_U32(pkt_data, pkt->color);
    CT_INCR_UNPACK_U32(pkt_data, pkt->dst_gport);
    CT_INCR_UNPACK_U32(pkt_data, pkt->src_gport);
    CT_INCR_UNPACK_U32(pkt_data, pkt->multicast_group);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_flags);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_forward);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_classification_tag);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_pkt_prio);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_dscp);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_load_balancing_number);
    CT_INCR_UNPACK_U32(pkt_data, pkt->rx_timestamp_upper);
    CT_INCR_UNPACK_U32(pkt_data, pkt->timestamp_flags);
    CT_INCR_UNPACK_U32(pkt_data, pkt->stk_encap_id);

    CT_INCR_UNPACK_U32(pkt_data, pkt->flags2);

    /* OAM SOMBH header fields */
    CT_INCR_UNPACK_U8(pkt_data, pkt->oam_replacement_type);
    CT_INCR_UNPACK_U8(pkt_data, pkt->oam_replacement_offset);
    CT_INCR_UNPACK_U16(pkt_data, pkt->oam_lm_counter_index);

    return pkt_data;
}

/*
 * Function:
 *      ct_tx_tunnel
 * Purpose:
 *      Tunnel a packet out a single port via a remote CPU
 * Parameters:
 *      pkt         - The packet data and meta-data to tunnel
 *      dest_unit   - The BCM unit to tunnel to
 *      remote_port - The port on dest_unit on which packet should egress
 *      flags       - Indicates untagged settings
 *      mode        - Reliable/best effort tunnelling
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Always sends async, but data is copied from parameter;
 *      If a callback is indicated in the packet, that will be made after
 *      local transmit
 *
 *      If flags & BCM_CPU_TUNNEL_F_PBMP is set, then remote_port is ignored
 *      and the port bitmaps are taken from the packet directly.
 */

#define KEY_BUF_LEN 8
int
ct_tx_tunnel(bcm_pkt_t *pkt,
             int dest_unit,
             int remote_port,
             uint32 flags,
             bcm_cpu_tunnel_mode_t mode)
{
    uint8 *pkt_data, *alloc_ptr;
    int len, dlen;
    int i;
    int rv;
    int client_id;
    uint32 ct_flags = 0;
    cpudb_key_t dest_key;
    char dest_key_str[CPUDB_KEY_STRING_LEN];

    LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                (BSL_META("CT Tunnel pkt out to (%d, %d), flags 0x%x, mode %d\n"),
                 dest_unit, remote_port, flags, mode));

    if (!bcm_unit_valid(dest_unit)) {
        return BCM_E_PARAM;
    }

    if (bcm_unit_local(dest_unit)) {
        return BCM_E_PARAM;
    }

    dlen = 0;
    for (i = 0; i < pkt->blk_count; i++) {
        dlen += pkt->pkt_data[i].len;
    }

    len = dlen + TUNNEL_TX_HEADER_BYTES;
    /* Freed either in this function (on error) or in ct_tx_tunnel_done_cb */
    pkt_data = alloc_ptr = atp_tx_data_alloc(len);
    if (alloc_ptr == NULL) {
        return BCM_E_MEMORY;
    }

    /* Pack up the metadata for the packet */
    pkt_data = ct_tx_tunnel_pack(pkt_data, pkt, dest_unit, remote_port,
                                 flags, dlen);

    /* Copy the packet data to send out */
    for (i = 0; i < pkt->blk_count; i++) {
        sal_memcpy(pkt_data, pkt->pkt_data[i].data, pkt->pkt_data[i].len);
        pkt_data += pkt->pkt_data[i].len;
    }

    if (mode == BCM_CPU_TUNNEL_PACKET_RELIABLE) {
        client_id = SHARED_CLIENT_ID_TUNNEL_TX_PKT;
    } else {
        client_id = SHARED_CLIENT_ID_TUNNEL_TX_PKT_NO_ACK;
    }

    CPUTRANS_COS_SET(ct_flags, pkt->cos);
    if (pkt->flags & BCM_TX_PRIO_INT) {
        CPUTRANS_INT_PRIO_SET(ct_flags, pkt->prio_int);
    }

    /* Only pass the packet as a cookie if the callback is != NULL;
     * Otherwise app may free pkt before it's examined by callback.
     */

    rv = bcm_unit_subtype_get(dest_unit, dest_key_str,
                               sizeof(dest_key_str));
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_TUNNEL,
                 (BSL_META("Tunnel TX: Could not get dest key string, %d: %s\n"),
                  rv, bcm_errmsg(rv)));
        atp_tx_data_free(alloc_ptr);
        return rv;
    }


    rv = cpudb_key_parse(dest_key_str, &dest_key);
    if (rv < 0) {
        LOG_WARN(BSL_LS_TKS_TUNNEL,
                 (BSL_META("Tunnel TX: Could not parse dest key string, %d: %s\n"),
                  rv, bcm_errmsg(rv)));
        atp_tx_data_free(alloc_ptr);
        return rv;
    }

    LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                (BSL_META("CT Tunnel dest key %x %x %x %x %x %x\n"),
                 dest_key.key[0],
                 dest_key.key[1],
                 dest_key.key[2],
                 dest_key.key[3],
                 dest_key.key[4],
                 dest_key.key[5]));
    rv = atp_tx(dest_key,
                client_id,
                alloc_ptr,
                len,
                ct_flags,
                ct_tx_tunnel_done_cb,
                (void *)(pkt->call_back != NULL ? pkt : NULL));

    if (rv < 0) {
        LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                    (BSL_META("Tunnel TX: Error sending %d: %s\n"),
                     rv, bcm_errmsg(rv)));
        atp_tx_data_free(alloc_ptr);
    }

    return rv;
}

/****************************************************************
 *
 * TUNNEL TX CODE, FORWARDER (slave)
 *
 *    This is the code that accepts a tunnelled packet and
 * forwards it out the indicated local port.
 */

#if CT_TUNNEL_QUEUE_ENABLE

STATIC int
ct_tx_tunnel_enqueue(void *pkt, bcm_rx_t *handled, uint8 *pkt_data, int len)
{
    int head, new_head, rv;

    TUNNEL_Q_LOCK;
    head = new_head = _tunnel_q_head;
    rv = BCM_E_NONE;

    /* Incr new_head, handle wraparound */
    if (++new_head >= CT_TUNNEL_QUEUE_SIZE) {
        new_head = 0;
    }

    /* Check for queue full */
    if (new_head != _tunnel_q_tail) {
        _tunnel_q_head = new_head;
        _tunnel_q[head].data = pkt_data;
        _tunnel_q[head].len  = len;
        _tunnel_q[head].pkt  = pkt;
    } else {
        rv = BCM_E_RESOURCE;
        _tunnel_q_top_full++;
    }

    TUNNEL_Q_UNLOCK;

    if (BCM_SUCCESS(rv)) {
        TUNNEL_Q_WAKE;
        *handled = BCM_RX_HANDLED_OWNED;
    } else {
        *handled = BCM_RX_HANDLED;
    }

    return rv;
}

STATIC ct_tunnel_q_element *
ct_tx_tunnel_dequeue(void)
{
    ct_tunnel_q_element *element = NULL;

    TUNNEL_Q_LOCK;
    if (_tunnel_q_head != _tunnel_q_tail) {
        element = _tunnel_q + _tunnel_q_tail;
        /* Incr tail, handle wraparound */
        if (++_tunnel_q_tail >= CT_TUNNEL_QUEUE_SIZE) {
            _tunnel_q_tail = 0;
        }
    }
    TUNNEL_Q_UNLOCK;

    return element;
}

STATIC void
ct_tx_tunnel_thread(void *cookie)
{
    ct_tunnel_q_element *element = NULL;
    int rv;

    COMPILER_REFERENCE(cookie);
    _tunnel_q_thread_exit = 0;
    for (;;) {

        TUNNEL_Q_SLEEP;

        while ((element = ct_tx_tunnel_dequeue()) != NULL) {
            rv = ct_tx_tunnel_forward(element->data, element->len);
            atp_rx_free(element->data, element->pkt);
            if (rv < 0) {
                _tunnel_q_error++;
            }
        }

        if (_tunnel_q_thread_exit) {
            _tunnel_q_thread = SAL_THREAD_ERROR;
            sal_thread_exit(0);
            return;
        }
    }
}

STATIC int
ct_tx_tunnel_thread_init(void)
{
    if (_tunnel_q_thread != SAL_THREAD_ERROR) {
        return BCM_E_BUSY;
    }

    /* Reset variables */
    _tunnel_q_top_full = 0;
    _tunnel_q_error = 0;
    _tunnel_q_head = _tunnel_q_tail = 0;

    _tunnel_q_lock = sal_mutex_create("bcm_tunnel_q");
    _tunnel_q_sem = sal_sem_create("bcm_tunnel_q", sal_sem_BINARY, 0);
    _tunnel_q_thread = sal_thread_create("bcmTUNQ",
                                         TUNNEL_Q_THREAD_STACK,
                                         TUNNEL_Q_THREAD_PRIO,
                                         ct_tx_tunnel_thread,
                                         NULL);
    if (_tunnel_q_thread == SAL_THREAD_ERROR) {
        sal_sem_destroy(_tunnel_q_sem);
        sal_mutex_destroy(_tunnel_q_lock);
        _tunnel_q_lock = NULL;
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;

}

/*
 * Stop the tunnel queue thread
 */
STATIC int
ct_tx_tunnel_thread_stop(void)
{
    if (_tunnel_q_thread == SAL_THREAD_ERROR) {
        return BCM_E_NONE;
    }
    _tunnel_q_thread_exit = 1;
    TUNNEL_Q_WAKE;
    sal_thread_yield();
    while (_tunnel_q_thread != SAL_THREAD_ERROR) {
        TUNNEL_Q_WAKE;
        sal_usleep(10000);
    }
    sal_sem_destroy(_tunnel_q_sem);
    sal_mutex_destroy(_tunnel_q_lock);
    _tunnel_q_lock = NULL;
    return BCM_E_NONE;
}

#define ct_tx_tunnel_forward_function ct_tx_tunnel_enqueue

#else

#define ct_tx_tunnel_forward_function ct_tx_tunnel_forward_sync

STATIC int
ct_tx_tunnel_forward_sync(void *pkt, bcm_rx_t *handled,
                          uint8 *pkt_data, int len)
{
    COMPILER_REFERENCE(pkt);
    *handled = BCM_RX_HANDLED;
    return ct_tx_tunnel_forward(pkt_data, len);
}

#endif /* CT_TUNNEL_QUEUE_ENABLE */

/* ATP handler for TX tunnelled packet */
STATIC bcm_rx_t
ct_tx_tunnelled_pkt_handler(cpudb_key_t src_key,
                            int client_id,
                            bcm_pkt_t *pkt,
                            uint8 *payload,
                            int payload_len,
                            void *cookie)
{
    int rv;
    bcm_rx_t handled;

    LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                (BSL_META("CT Tunnel pkt in from " CPUDB_KEY_FMT " cli %d, len %d\n"),
                 CPUDB_KEY_DISP(src_key), client_id, payload_len));

    if (payload == NULL) {
        /* Unsupported forwarding mode */
        LOG_WARN(BSL_LS_TKS_TUNNEL,
                 (BSL_META("Tunnel TX: Received segmented packet to "
                  "forward.  Unsupported\n")));
        return BCM_RX_HANDLED;
    }

    rv = ct_tx_tunnel_forward_function(pkt, &handled,
                                       payload, payload_len);

    if (rv < 0) {
        LOG_VERBOSE(BSL_LS_TKS_TUNNEL,
                    (BSL_META("Tunnel TX: Error forwarding %d: %s\n"),
                     rv, bcm_errmsg(rv)));
    }

    return handled;
}

/*
 * Function:
 *      ct_tx_tunnel_forward
 * Purpose:
 *      Forward a packet received for tunnelling
 * Parameters:
 *      pkt_data  - The start of the encapsulated packet
 *      len       - Length of encapsulated packet
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Sends sync only.
 *      Assumes data in pkt_data is DMA'able
 */

int
ct_tx_tunnel_forward(uint8 *pkt_data, int len)
{
    int pkt_len;
    int local_unit;
    int local_port; /* Not used if PBMP flag is set */
    bcm_pkt_t pkt;
    uint32 flags;
    int rv;

    if (len <= TUNNEL_TX_HEADER_BYTES) {
        return BCM_E_PARAM;
    }
    sal_memset(&pkt, 0, sizeof(pkt));

    /* Unpack the metadata */
    pkt_data =
        ct_tx_tunnel_unpack(pkt_data, len, &pkt, &pkt_len,
                            &local_unit, &local_port, &flags);
    if (pkt_data == NULL) {
        return BCM_E_RESOURCE;   /* Not enough bytes in packet */
    }
    /* pkt_data now points to start of payload to send out */

    if (!(flags & BCM_CPU_TUNNEL_F_PBMP)) {
        /* Generate bitmap from single local port */
        BCM_PBMP_PORT_SET(pkt.tx_pbmp, local_port);
        if (flags & BCM_CPU_TUNNEL_F_UNTAGGED) {
            BCM_PBMP_PORT_SET(pkt.tx_upbmp, local_port);
        }
    }

    BCM_PKT_ONE_BUF_SETUP(&pkt, pkt_data, pkt_len);

    /* Send synchronosly using packet data passed to us. */
    rv = bcm_tx(local_unit, &pkt, NULL);


    return rv;
}

#endif /* BCM_RPC_SUPPORT */
