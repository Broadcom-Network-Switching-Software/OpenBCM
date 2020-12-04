/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tx.h
 * Purpose:     
 */

#ifndef   _BCM_INT_TX_H_
#define   _BCM_INT_TX_H_

#include <bcm/pkt.h>

/* This is all the extra information associated with a DV for RX */
typedef struct tx_dv_info_s {
    volatile bcm_pkt_t    **pkt;
    int                     pkt_count;
    volatile uint8          pkt_done_cnt;
    bcm_pkt_cb_f            chain_done_cb;
    void                   *cookie;
} tx_dv_info_t;

/*
 * TX DV INFO structure maintains information about a TX chain.
 * This includes:  Per packet callbacks and the chain done callback.
 *
 * Packets are processed in the order they return if any packet in
 * the chain requires a call back.  If none does, then only a chain
 * done callback is setup/made.
 *
 * A "current" packet is maintained in the order the packet's are
 * set up.  The packet-done interrupts will occur in this order.
 *
 * TX DV info macros:
 *     
 *    TX_INFO_SET(dv, val)      Assign the TX info ptr for a DV
 *    TX_INFO(dv)               Access the TX info ptr of a DV
 *    TX_INFO_PKT_ADD(dv)       Add pkt ptr to info struct; advance count
 *    TX_INFO_CUR_PKT(dv)       Pointer to "current" pkt done being processed
 *    TX_INFO_PKT_MARK_DONE(dv) Advances "current" packet.
 */

#define TX_INFO_SET(dv, val)      ((dv)->dv_public1.ptr = val)
#define TX_INFO(dv)               ((tx_dv_info_t *)((dv)->dv_public1.ptr))
#define TX_INFO_PKT_ADD(dv, pkt) \
    TX_INFO(dv)->pkt[TX_INFO(dv)->pkt_count++] = (pkt)
#define TX_INFO_CUR_PKT(dv)       (TX_INFO(dv)->pkt[TX_INFO(dv)->pkt_done_cnt])
#define TX_INFO_PKT_MARK_DONE(dv) ((TX_INFO(dv)->pkt_done_cnt)++)

#define TX_DV_NEXT(dv)            ((dv_t *)((dv)->dv_public2.ptr))
#define TX_DV_NEXT_SET(dv, dv_next) \
    ((dv)->dv_public2.ptr = (void *)(dv_next))

/*
 * Define:  TX_EXTRA_DCB_COUNT:
 *      The number of extra DCBs needed to accomodate portions of the
 * packet such as the HiGig header, SL tag, breaking up a block due
 * to VLAN tag insertion, etc.
 */
#define TX_EXTRA_DCB_COUNT 6

extern void
_bcm_rcpu_tx_packet_done_cb(int unit, bcm_pkt_t *pkt);

#endif	/* !_BCM_INT_TX_H_ */
