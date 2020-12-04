/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        atp_int.h
 * Purpose:     Internal ATP function headers
 */

#ifndef   _ATP_INT_H_
#define   _ATP_INT_H_

#include <sdk_config.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>

#include <bcm/types.h>
#include <bcm/pkt.h>

#include <appl/cputrans/atp.h>

/*
 * This is the max number of objects that can be marked deleted
 * before the garbage collection task is forced to block
 *
 * GARBAGE_WAITING_SLEEP is how long the RX thread sleeps when
 * there's garbage waiting to be collected.  Thus, will wait at
 * most ATP_GARBAGE_MAX * ATP_GARBAGE_WAITING_SLEEP microseconds
 * before it blocks.
 */

#ifndef ATP_GARBAGE_MAX
#define ATP_GARBAGE_MAX 15
#endif

#ifndef ATP_GARBAGE_WAITING_SLEEP
#define ATP_GARBAGE_WAITING_SLEEP 50000
#endif

#define ATP_VERSION       0

#define ATP_HEADER_BYTES \
    (3 * sizeof(uint32) + 2 * sizeof(uint16) + 2 * sizeof(uint8))

#define ATP_HEADER_START(pkt) (&((pkt)[CPUTRANS_ATP_OFS]))

/* Sequence number offset:  version + client + hdr_flags */
#define ATP_SEQ_NUM_OFS \
    (CPUTRANS_ATP_OFS + 2 * sizeof(uint16) + sizeof(uint32))

/* Packet opcodes */
typedef enum _atp_opcodes_e {
    _ATP_OPC_DATA,
    _ATP_OPC_ACK
} _atp_opcodes_t;

/* RX queue info */

#ifndef ATP_PKT_DATA_QUEUE_LEN
#define ATP_PKT_DATA_QUEUE_LEN 100
#endif

static volatile struct _atp_pkt_data_s {
    int len;
    uint8 *pkt_buf;
    uint32 flags;
    volatile struct _atp_pkt_data_s *next;
} _atp_rx_pkt[ATP_PKT_DATA_QUEUE_LEN];

typedef volatile struct _atp_pkt_data_s _atp_pkt_data_t;

/*
 * ATP header information:
 *      This maintains the information sent/received in the ATP/BET
 *      header and additional info related to a packet segment transaction.
 *
 * Flags:
 *      HDR_NO_ACK         Don't ACK the packet.
 *      HDR_NEXT_HOP       Next hop transport being used
 *      HDR_RETRANSMIT     Retransmit of packet (data only, not ACK)
 *      HDR_IMMEDIATE_ACK  Send ACK immediately
 */

typedef struct _atp_hdr_s _atp_hdr_t;
struct _atp_hdr_s {
    int            client_id;     /* Client for packet */
    uint16         version;       /* Version number */
    uint32         hdr_flags;
    uint16         seq_num;       /* Sequence number of transaction */
    uint16         tot_bytes;     /* Total bytes in payload */
    uint16         tot_segs;      /* Total segments in transaction */
    uint16         segment;       /* Segment number */

    /*
     * For TX, start_byte is first byte of payload in this segment.
     * For ACK, start_byte is number of (contiguous) bytes received.
     * (which is the next byte that is expected).
     */
    uint16         start_byte; 
    uint8          opcode;        /* ACK, DATA, etc; see above */
    uint8          cos;           /* Cos */
};

#define _ATP_HDR_NO_ACK         0x1  /* No need to ACK this packet */
#define _ATP_HDR_NEXT_HOP       0x2  /* Use next hop transport if available */
#define _ATP_HDR_RETRANSMIT     0x4  /* This is a retrx packet */
#define _ATP_HDR_IMMEDIATE_ACK  0x8  /* Send ACK immediately */

/* Number of bytes allocated for an ACK packet */
#define _ATP_ACK_BYTES (CPUTRANS_HEADER_BYTES < 68 ? 68 : CPUTRANS_HEADER_BYTES)

/* Transaction control block structure */
typedef volatile struct _atp_rx_trans_s _atp_rx_trans_t;
struct _atp_rx_trans_s {
    _atp_hdr_t       _atp_hdr;        /* From first portion of pkt received */
    int            src_cpu;       /* Where is transact from */

    int            rcv_segs;      /* How many segments received */
    int            ack_count;     /* Number of times an ACK sent */
    int            payload_len;   /* Total length of the packet data */

    uint8         *ack_data;      /* This transaction's ACK packet */
    bcm_pkt_t     *pkt;           /* Pkt for data; setup by cputrans */
    uint8         *lb_data;       /* For loopback packets */
    sal_usecs_t    rcvd_time;     /* When was request received */

    uint32 flags;
#define _ATP_RX_F_HANDLED     0x1   /* Has the local client been called */
#define _ATP_RX_F_DATA_READY  0x2   /* For reassembly */
#define _ATP_RX_F_LOOPBACK    0x10  /* Packet is loopback packet */
#define _ATP_RX_F_ENQUEUED    0x20  /* For deferred deletion */

    volatile struct _atp_client_s   *client;
    _atp_rx_trans_t *next;
    _atp_rx_trans_t *prev;
};

#define RX_TRANS_DATA_DONE(_trans) \
   (_trans)->flags & (_ATP_RX_F_DATA_READY | _ATP_RX_F_HANDLED)

#define RX_TRANS_NO_ACK(_trans) \
    ((_trans)->_atp_hdr.hdr_flags & _ATP_HDR_NO_ACK)
#define RX_TRANS_ACK(_trans)  (!RX_TRANS_NO_ACK(_trans))
#define RX_TRANS_NEXT_HOP(_trans) \
    ((_trans)->_atp_hdr.hdr_flags & _ATP_HDR_NEXT_HOP)
#define RX_TRANS_C2C(_trans) (!RX_TRANS_NEXT_HOP(_trans))

/* Transaction control block structure */
typedef volatile struct _atp_tx_trans_s _atp_tx_trans_t;
struct _atp_tx_trans_s {
    _atp_hdr_t     _atp_hdr;      /* Basic header for the transaction */
    int            dest_cpu;      /* Where's it going */
    uint8         *pkt_buf;       /* Pointer to original data */
    int            len;           /* Length of payload */
    uint32         ct_flags;      /* CPUTRANS flags */
    int            db_update;     /* What was DB update count last tx? */

    bcm_pkt_t     *pkt_list;      /* Linked list from cputrans alloc */
    sal_sem_t      tx_sem;        /* For sync transmit of ATP pkt */
    int            tx_count;      /* How many times sent */
    int            bytes_acked;   /* How many bytes in payload ACK'd */
    sal_usecs_t    last_tx; /* Last time sent */

    atp_tx_cb_f    callback;      /* If asynch, indicates callback */
    void          *cookie;        /* Passed to callback. */
    int            tx_rv;         /* Result of tx operation when async */

    int            seg_len;       /* Record in case dyncamically changed */
    uint32         flags;
#define _ATP_TX_F_DONE        0x1   /* ACK seen */
#define _ATP_TX_F_PENDING     0x2   /* Is TX being done? */
#define _ATP_TX_F_TIMEOUT     0x4   /* TX timed out */
#define _ATP_TX_ERROR_SEEN    0x8
#define _ATP_TX_F_SYNC_LOCK   0x10  /* Transaction used for sync op */
#define _ATP_TX_F_SEM_WAITING 0x20  /* Has TX semaphore been given? */
#define _ATP_TX_F_DONE_MARK   0x40  /* Has done been handled already */
#define _ATP_TX_F_ENQUEUED    0x80

    volatile struct _atp_client_s   *client;  /* Controlling client */
    _atp_tx_trans_t *next;          /* Linked lists of transactions */
    _atp_tx_trans_t *prev;          /* For dequeuing */
};

#define TX_TRANS_NO_ACK(_trans) \
    ((_trans)->_atp_hdr.hdr_flags & _ATP_HDR_NO_ACK)
#define TX_TRANS_ACK(_trans)  (!TX_TRANS_NO_ACK(_trans))
#define TX_TRANS_NEXT_HOP(_trans) \
    ((_trans)->_atp_hdr.hdr_flags & _ATP_HDR_NEXT_HOP)
#define TX_TRANS_C2C(_trans) (!TX_TRANS_NEXT_HOP(_trans))

/* Number of hash buckets allowed for client ID lists */
#define _ATP_CLIENT_HASH_MAX 31

#define ACK_PKT_SIZE            CPUTRANS_HEADER_BYTES

/*
 * ATP client CPU structure.
 * Holds callback for RX operations, TX and RX queues for ATP and BET.
 * Holds sequence numbers per dest CPU.
 */

typedef volatile struct _atp_client_cpu_s {
    _atp_rx_trans_t * rx_trans;  /* Current RX transaction */
    _atp_rx_trans_t * rx_tail;   /* Tail for RX trans queue */
    _atp_tx_trans_t * tx_trans;  /* TX trans linked list */
    _atp_tx_trans_t * tx_tail;   /* Tail for TX trans queues */
    uint8 cpu_flags;             /* Per CPU flags */
#define ATP_CPU_RX_TRANS_SEEN     0x1  /* Seen an RX here, RX seq valid */
#define ATP_CPU_ACK_PENDING       0x2  /* Is an ACK pending? */

    uint16 tx_seq_num;           /* Per-CPU TX Sequence number */
    uint16 rx_seq_num;           /* Per-CPU current ATP RX Sequence number */

    _atp_hdr_t atp_hdr;          /* Hdr from latest RX transact */
    bcm_pkt_t * bet_rx_pkts;     /* For BET packet accumulation */
    int bet_segs;                /* Segs in pkt: To be implemented. */

    uint8 *rx_ack_data;          /* Ack data when RX trans queue is empty */
} _atp_client_cpu_t;

/*
 * ATP client structure.
 * Holds callback for RX operations.
 * Holds CPU info per CPU.
 */

typedef volatile struct _atp_client_s _atp_client_t;
struct _atp_client_s {
    int client_id;              /* The actual ID */
    uint32 flags;               /* Flags; see atp.h */
/* Remark:  atp.h reserves internal flags as:  ATP_F_RESERVED  0xffff0000 */

    /* TX params */
    int cos;                    /* COS and internal priority for packets
                                   for this client */
    int vlan;                   /* VLAN for packets for this client */

    _atp_client_t *next;        /* Linked lists for hash buckets */
    _atp_client_t *prev;        /* Linked lists for hash buckets */

    uint16 bet_tx_seq_num;      /* For best effort TX ops. */

    _atp_client_cpu_t cpu[CPUDB_CPU_MAX];     /* Per CPU info */

    atp_client_cb_f callback;   /* Callback for the client */
    void *cookie;               /* Passed to callback */
};

#endif /* _ATP_INT_H_ */
