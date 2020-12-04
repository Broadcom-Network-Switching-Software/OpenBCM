/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TXBEACON_H__
#define __BCM_TXBEACON_H__

#include <bcm/types.h>
#include <bcm/pkt.h>

/* bcm_txbeacon_t flags definitions */
#define BCM_TXBEACON_WITH_ID    (1 << 0)   /* Set if TX Beacon structure has ID */
#define BCM_TXBEACON_AUTOSTART  (1 << 1)   /* Set if uCs should start beacon on
                                              loss of keepalive */

/* Unique packet ID shared with uCs. */
typedef uint32 bcm_txbeacon_pkt_id_t;

/* Tx Beacon basic data structure. */
typedef struct bcm_txbeacon_s {
    uint32 flags;                   /* Flags indicating action of API */
    uint16 len;                     /* length of packet in bytes. NOTE: IN/OUT */
    uint16 maxlen;                  /* maximum length in case of modification.
                                       NOTE: IN/OUT */
    bcm_txbeacon_pkt_id_t pkt_id;   /* Packet ID from the uC.  NOTE: IN/OUT */
    uint16 port;                    /* port to send packet to */
    uint16 interval;                /* interval to send packet in mSeconds */
    uint16 count;                   /* Number of packets to send or 0 (forever) */
    uint8 *pkt_data;                /* Pointer to packet NOTE: IN/OUT */
} bcm_txbeacon_t;

/* tx beacon traverse callback */
typedef int (*bcm_txbeacon_traverse_cb_t)(
    int unit, 
    bcm_txbeacon_t *tx, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* Traverse the tx beacon packets */
extern int bcm_txbeacon_traverse(
    int unit, 
    bcm_txbeacon_traverse_cb_t trav_fn, 
    void *user_data);

/* Init routine for TX beacon */
extern int bcm_txbeacon_init(
    int unit, 
    int uC);

/* Add a packet for TX beacon */
extern int bcm_txbeacon_pkt_setup(
    int unit, 
    bcm_txbeacon_t *txbeacon);

/* Remove this packet from the txbeacon list */
extern int bcm_txbeacon_destroy(
    int unit, 
    bcm_txbeacon_pkt_id_t pkt_id);

/* Lookup packet beacon info structure */
extern int bcm_txbeacon_pkt_get(
    int unit, 
    bcm_txbeacon_t *txbeacon);

/* Start beacon for this packet or all */
extern int bcm_txbeacon_start(
    int unit, 
    bcm_txbeacon_pkt_id_t pkt_id);

/* Start beacon for this packet or all */
extern int bcm_txbeacon_stop(
    int unit, 
    bcm_txbeacon_pkt_id_t pkt_id);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TXBEACON_H__ */
