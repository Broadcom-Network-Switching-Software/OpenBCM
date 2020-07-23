/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cputrans.h
 * Purpose:     General defines for CPU to CPU communication
 */

#ifndef   _CPUTRANS_CPUTRANS_H_
#define   _CPUTRANS_CPUTRANS_H_

#include <sdk_config.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>

#include <appl/cpudb/cpudb.h>

/* Default values for cputrans_rx_pkt_setup */

#ifndef CPUTRANS_RX_BLK_CNTS_DEFAULT
#define CPUTRANS_RX_BLK_CNTS_DEFAULT {20, 20, 20, 10, 0, 0, 0, 10}
#endif
#ifndef CPUTRANS_RX_LIST_COUNT_DEFAULT
#define CPUTRANS_RX_LIST_COUNT_DEFAULT 8
#endif

/* Default packet pool size */
#ifndef CPUTRANS_PKT_POOL_SIZE_DEFAULT
#define CPUTRANS_PKT_POOL_SIZE_DEFAULT     128
#endif

/* Here's a sample TX reserve setup
#define CPUTRANS_TX_RESERVE_DEFAULT {50, 50, 50, 50, 25, 25, 10, 0}
*/

#ifndef CPUTRANS_TX_RESERVE_DEFAULT
#define CPUTRANS_TX_RESERVE_DEFAULT {0 }
#endif

/*
 * Transmit flags:
 *     CPUTRANS_NO_HEADER_ALLOC    Packet buffer already has header space;
 *                                 default is to use locally maintained
 *                                 buffers for headers.
 *     CPUTRANS_BCAST              Broadcast to all CPUs; currently
 *                                 implies BET and next hop; default is to
 *                                 use dest_key or other addressing means
 *     CPUTRANS_LOOPBACK           Only examined if BCAST is set; In this
 *                                 case, will also send the packet up the
 *                                 local stack; default is to only send
 *                                 out to remote CPUs
 *     CPUTRANS_NO_L2_UPDATE       For nh_tx: Do not update the L2
 *                                 header of the packet; default is to
 *                                 update the L2 header with the NH dest
 *                                 mac and the local CPU.  Packets that
 *                                 are received by next_hop and then
 *                                 retransmitted have this flag set.
 *     CPUTRANS_NO_ACK             Override client flags and send a
 *                                 packet without an ACK.  The flag is
 *                                 conveyed in the packet headers.
 *     CPUTRANS_NEXT_HOP           Override client flags and send a
 *                                 packet using the next hop transport.
 *     CPUTRANS_CRC_REGEN          Regenerate CRC.
 *     CPUTRANS_IMMEDIATE_ACK      Send ACK immediately.  If flag is not
 *                                 set, ACK is sent  asynchronously;
 *                                 packet handling and transmit of the
 *                                 ACK can happen in any order.
 *     CPUTRANS_COS_OVERRIDE       Use the cos priority bits for cos queue
 *                                 packet transmit.
 *                                 COS value should be set with macro
 *                                 CPUTRANS_COS_SET.
 *     CPUTRANS_INT_PRIO_OVERRIDE  Use the internal priority bits for packet
 *                                 transmit.  Internal priority value should
 *                                 be set with macro CPUTRANS_INT_PRIO_SET.
 */

#define CPUTRANS_COS_SHIFT          0
#define CPUTRANS_COS_MASK           (0x7 << CPUTRANS_COS_SHIFT)
#define CPUTRANS_COS_OVERRIDE       0x00000080
#define CPUTRANS_INT_PRIO_SHIFT     8
#define CPUTRANS_INT_PRIO_MASK      (0x7 << CPUTRANS_INT_PRIO_SHIFT)
#define CPUTRANS_INT_PRIO_OVERRIDE  0x00008000

#define CPUTRANS_NO_HEADER_ALLOC    0x00010000 
#define CPUTRANS_BCAST              0x00020000  /* Implies BET and next hop */
#define CPUTRANS_LOOPBACK           0x00040000
#define CPUTRANS_NO_L2_UPDATE       0x00080000
#define CPUTRANS_NO_ACK             0x00100000
#define CPUTRANS_NEXT_HOP           0x00200000
#define CPUTRANS_CRC_REGEN          0x00400000
#define CPUTRANS_IMMEDIATE_ACK      0x00800000

#define CPUTRANS_INT_PRIO_VALID     CPUTRANS_INT_PRIO_OVERRIDE

#define CPUTRANS_COS_SET(flags, val)                             \
   ((flags) = ((flags) & (~CPUTRANS_COS_MASK))              |    \
       (((val) << CPUTRANS_COS_SHIFT) & CPUTRANS_COS_MASK)  |    \
        CPUTRANS_COS_OVERRIDE)

#define CPUTRANS_COS_GET(flags)                                  \
   (((flags) & CPUTRANS_COS_MASK) >> CPUTRANS_COS_SHIFT)

#define CPUTRANS_INT_PRIO_SET(flags, val)                               \
   ((flags) = ((flags) & (~CPUTRANS_INT_PRIO_MASK))                  |  \
       (((val) << CPUTRANS_INT_PRIO_SHIFT) & CPUTRANS_INT_PRIO_MASK) |  \
        CPUTRANS_INT_PRIO_OVERRIDE)

#define CPUTRANS_INT_PRIO_GET(flags)                                    \
   (((flags) & CPUTRANS_INT_PRIO_MASK) >> CPUTRANS_INT_PRIO_SHIFT)


/* Next hop used explicitly or with BET BCAST */
#define CPUTRANS_IS_NEXT_HOP(ct_flags) \
    ((ct_flags) & (CPUTRANS_BCAST | CPUTRANS_NEXT_HOP))

/*
 * These defines indicate the data layout.  Most importantly, they
 * indicate to the application where in a raw packet the payload
 * starts.  By default, applications do not allocate space for
 * the header, but can indicate they have by setting the NO_HEADER_ALLOC
 * flag above.
 *
 * The SNAP header is common for all packet types:
 *       src/dest MAC, VLAN, type/len, SNAP HDR, local type indicator,
 *       client indicator, reserved 16 bits.
 */
#define CPUTRANS_L2_SNAP_OFS  0   /* Just for consistency */

#define CPUTRANS_L2_SNAP_BYTES                                  \
    (2 * sizeof(bcm_mac_t) +   /* S/D MACs */                   \
     sizeof(uint32) +          /* VLAN */                       \
     sizeof(uint16) +          /* Type/Len */                   \
     sizeof(bcm_mac_t) + sizeof(uint16) + /* SNAP */            \
     sizeof(uint16) +          /* TR_TYPE (Local SNAP type) */  \
     sizeof(uint16) +          /* TR_MULT (Client multiplex */  \
     sizeof(uint16))           /* TR_R (Transport reserved */


/*
 * CPU Transport Header, part 1.
 * See cputrans.txt for more info about these
 *      CPUTRANS_DMAC_OFS        0
 *      CPUTRANS_SMAC_OFS        6
 *      CPUTRANS_VTAG_OFS        12
 *      CPUTRANS_LEN_OFS         16
 *      CPUTRANS_SNAP_OFS        18
 *      CPUTRANS_SNAP_TYPE_OFS   26
 *      CPUTRANS_SNAP_BYTES              8
 *
 * Three local words (part of CPU Trans, part 1):
 *      CPUTRANS_TR_TYPE_OFS     28    What transport is using this?
 *      CPUTRANS_TR_MULT_OFS     30    Multiplexor for apps using transport
 *      CPUTRANS_TR_RSVD_OFS     32    2 bytes reserved for transport
 *
 * Next hop transmit stores the TX unit/port in the header.
 *      CPUTRANS_NH_UNIT_OFS     32
 *      CPUTRANS_NH_PORT_OFS     33
 */

#define CPUTRANS_DMAC_OFS    CPUTRANS_L2_SNAP_OFS
#define CPUTRANS_SMAC_OFS    (CPUTRANS_DMAC_OFS + sizeof(bcm_mac_t))
#define CPUTRANS_VTAG_OFS    (CPUTRANS_SMAC_OFS + sizeof(bcm_mac_t))
#define CPUTRANS_LEN_OFS     (CPUTRANS_VTAG_OFS + sizeof(uint32))
#define CPUTRANS_SNAP_OFS    (CPUTRANS_LEN_OFS + sizeof(uint16))
#define CPUTRANS_SNAP_TYPE_OFS    (CPUTRANS_SNAP_OFS + sizeof(bcm_mac_t))
#define CPUTRANS_SNAP_BYTES  (sizeof(bcm_mac_t) + sizeof(uint16))

#define CPUTRANS_TR_TYPE_OFS  (CPUTRANS_SNAP_OFS + CPUTRANS_SNAP_BYTES)
#define CPUTRANS_TR_MULT_OFS  (CPUTRANS_TR_TYPE_OFS + sizeof(uint16))
#define CPUTRANS_TR_RSVD_OFS  (CPUTRANS_TR_MULT_OFS + sizeof(uint16))

#define CPUTRANS_NH_UNIT_OFS  CPUTRANS_TR_RSVD_OFS
#define CPUTRANS_NH_PORT_OFS  (CPUTRANS_NH_UNIT_OFS + 1)

/*
 * Space is left for next hop header always:
 *      NH dest mac, NH src mac, seq number, port (client)
 */

#define CPUTRANS_NEXT_HOP_OFS  CPUTRANS_L2_SNAP_BYTES


/* Offsets for (original) source CPU key and NEXT_HOP seq no */
#define CPUTRANS_DEST_KEY_OFS    CPUTRANS_NEXT_HOP_OFS
#define CPUTRANS_SRC_KEY_OFS     (CPUTRANS_DEST_KEY_OFS + sizeof(cpudb_key_t))
#define CPUTRANS_NH_SEQ_NUM_OFS  (CPUTRANS_SRC_KEY_OFS + sizeof(cpudb_key_t))
#define CPUTRANS_NH_MPLX_NUM_OFS (CPUTRANS_NH_SEQ_NUM_OFS + sizeof(uint16))

#define CPUTRANS_NEXT_HOP_BYTES  \
    (2 * CPUDB_KEY_BYTES +     /* D/S keys */        \
     sizeof(uint16) +          /* Sequence num */    \
     sizeof(uint32))           /* Client multiplexing value */

/*
 * The ATP/BET header:
 *      Client ID, Flags, Sequence number, Segment count,
 *      Length, Opcode, Priority
 */

#define CPUTRANS_ATP_OFS \
    (CPUTRANS_L2_SNAP_BYTES + CPUTRANS_NEXT_HOP_BYTES)

#define CPUTRANS_ATP_BYTES \
    (sizeof(uint8)  +          /* Version */         \
     sizeof(uint8)  +          /* Reserved */        \
     sizeof(uint16) +          /* Client ID */       \
     sizeof(uint32) +          /* Flags */           \
     sizeof(uint16) +          /* Sequence Number */ \
     sizeof(uint16) +          /* Total Payload */   \
     sizeof(uint16) +          /* Payload Offset */  \
     sizeof(uint8)  +          /* Total Segments */  \
     sizeof(uint8)  +          /* Segment Number */  \
     sizeof(uint8)  +          /* Opcode */          \
     sizeof(uint8))            /* Cos */

/*
 * How deep into the packet before the data starts.
 * This is also an overhead counter for extra space at start
 * of packet for all transports.
 */

#define CPUTRANS_HEADER_BYTES                        \
    (CPUTRANS_L2_SNAP_BYTES +                        \
     CPUTRANS_NEXT_HOP_BYTES +                       \
     CPUTRANS_ATP_BYTES)

#define CPUTRANS_DATA_OFS         CPUTRANS_HEADER_BYTES

/*
 * cputrans_error_count counts errors that may cause or may indicate
 * internal corruption.
 */

extern volatile int cputrans_error_count;
extern int cputrans_error_count_get(int clear);

extern int        cputrans_tx_pkt_setup(int pkt_pool_size,
                                        bcm_trans_ptr_t *trans_ptr);
extern void       cputrans_tx_pkt_cleanup(void);
extern bcm_pkt_t *cputrans_tx_pkt_alloc(uint8 *pkt_buf, int len, uint32 flags);
extern int        cputrans_tx_setup_done(void);
extern bcm_pkt_t *cputrans_tx_pkt_list_alloc(uint8 *pkt_buf,
                                             int len,
                                             int seg_len,
                                             uint32 ct_flags,
                                             int *num_segs);
extern void       cputrans_tx_pkt_list_free(bcm_pkt_t *pkt);

extern int        cputrans_tx_alloc_limit_set(int cos, int reserve);
extern int        cputrans_tx_alloc_limit_get(int cos, int *reserve);

#define cputrans_tx_pkt_free(_pkt) cputrans_tx_pkt_list_free(_pkt)


extern void       cputrans_rx_pkt_cleanup(void);
extern int        cputrans_rx_pkt_setup(int list_count, int *blk_cnts);
extern int        cputrans_rx_setup_done(void);
extern bcm_pkt_t *cputrans_rx_pkt_alloc(int num_blks);
extern void       cputrans_rx_pkt_free(bcm_pkt_t *pkt);

extern int cputrans_trans_count(void);
extern int cputrans_trans_add(bcm_trans_ptr_t *trans);
extern int cputrans_trans_remove(bcm_trans_ptr_t *trans);
extern int cputrans_rx_register(const char *name,
                                bcm_rx_cb_f callback,
                                uint8 priority,
                                void *cookie,
                                uint32 flags);
extern int cputrans_rx_unit_register(int unit,
                                     const char *name,
                                     bcm_rx_cb_f callback,
                                     uint8 priority,
                                     void *cookie,
                                     uint32 flags);
extern int cputrans_rx_bmp_register(uint32 unit_bmp,
                                    const char *name,
                                    bcm_rx_cb_f callback,
                                    uint8 priority,
                                    void *cookie,
                                    uint32 flags);
extern int cputrans_rx_unregister(bcm_rx_cb_f callback,
                                  uint8 priority);
extern int cputrans_rx_unit_unregister(int unit,
                                       bcm_rx_cb_f callback,
                                       uint8 priority);
extern int cputrans_rx_bmp_unregister(uint32 unit_bmp,
                                      bcm_rx_cb_f callback,
                                      uint8 priority);

#endif /* _CPUTRANS_CPUTRANS_H_ */
