/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        atp_bet.h
 * Purpose:     
 */

#ifndef   _CPUTRANS_ATP_H_
#define   _CPUTRANS_ATP_H_

#include <sdk_config.h>
#include <shared/idents.h>

#include <bcm/types.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <appl/cputrans/cputrans.h>

/*
 * Function prototype for ATP callbacks.  Callback may steal the packet.
 *
 * If the application steals the packet, it must free the data using
 * atp_free on the allocation pointer (alloc_ptr).
 *
 * NOTE:  For BET callbacks and loopback packets, the pkt pointer
 * may be NULL, and only the payload pointer is set.
 */

typedef bcm_rx_t (*atp_client_cb_f)(cpudb_key_t src_key,
                                    int client_id,
                                    bcm_pkt_t *pkt,  /* Pass to free */
                                    uint8 *payload,  /* Pass to free */
                                    int payload_len,
                                    void *cookie);

typedef void (*atp_tx_cb_f)(uint8 *pkt, void *cookie, int rv);

/*
 * ATP Timeout callback.  If an ATP transaction times out and
 * a timeout callback is registered, it is called just before
 * the timeout failure is returned to the calling application.
 */
typedef void (*atp_timeout_cb_f)(cpudb_key_t dest_key);

#ifdef INCLUDE_USER_KERNEL_TRANSPORTS
#ifndef ATP_RETRY_TIMEOUT_DEFAULT
#define ATP_RETRY_TIMEOUT_DEFAULT 1000000
#endif
#ifndef ATP_RETRY_TIMEOUT_MIN
#define ATP_RETRY_TIMEOUT_MIN     500000
#endif
#else
#ifndef ATP_RETRY_TIMEOUT_DEFAULT
#define ATP_RETRY_TIMEOUT_DEFAULT 300000
#endif
#ifndef ATP_RETRY_TIMEOUT_MIN
#define ATP_RETRY_TIMEOUT_MIN     100000
#endif
#endif

#ifndef ATP_RETRY_COUNT_DEFAULT
#define ATP_RETRY_COUNT_DEFAULT   10
#endif

/* Convenience */
#ifndef ATP_TIMEOUT_DEFAULT
#define ATP_TIMEOUT_DEFAULT       \
    (ATP_RETRY_COUNT_DEFAULT * ATP_RETRY_TIMEOUT_DEFAULT)
#endif
#ifndef ATP_RX_TRANSACT_DEFAULT
#define ATP_RX_TRANSACT_DEFAULT   32
#endif
#ifndef ATP_TX_TRANSACT_DEFAULT
#define ATP_TX_TRANSACT_DEFAULT   32
#endif

#ifndef ATP_COS_DEFAULT
#define ATP_COS_DEFAULT       0
#endif
#ifndef ATP_VLAN_DEFAULT
#define ATP_VLAN_DEFAULT      1
#endif

#define ATP_PKT_TYPE         SHARED_PKT_TYPE_ATP

#ifndef ATP_RX_PRIORITY
#define ATP_RX_PRIORITY           100   /* Priority used registering w/ RX */
#endif
#ifndef ATP_THREAD_PRIORITY_DEFAULT
#define ATP_THREAD_PRIORITY_DEFAULT   100
#endif
#ifndef ATP_SEG_LEN_DEFAULT
#define ATP_SEG_LEN_DEFAULT   (1518 - CPUTRANS_HEADER_BYTES)
#endif

#ifndef ATP_MTU
#define ATP_MTU               (64 * 1024)
#endif

#ifndef ATP_CPU_COUNT_DEFAULT
#define ATP_CPU_COUNT_DEFAULT  CPUDB_CPU_MAX
#endif

#define ATP_UNITS_ALL 0xffffffff

/*
 * This global variable would determine if the allocated memory for the ATP ACK
 * pkt data would be from DMA pool or Heap. By default the value of this is 0
 * which means the allocation happens from DMA capable pool. Applications would
 * need to set it to a nonzero value if they would like SDK to use system Heap.
 */
extern int atp_ack_pkt_data_from_heap;

/*
 * Applications may override atp_tx on a per-CPU basis.
 * This provides the type description for such a function pointer.
 * Requires using atp_rx_inject to inject packets at the receive end.
 */

typedef int (*atp_tx_f)(cpudb_key_t dest_key,
                        int client_id,
                        uint8 *pkt_buf,
                        int len,
                        uint32 ct_flags,
                        atp_tx_cb_f callback,
                        void *cookie);

extern int atp_tx_override_set(cpudb_key_t dest_cpu, atp_tx_f override_tx);
extern int atp_tx_override_get(cpudb_key_t dest_cpu, atp_tx_f *override_tx);

extern int atp_cpu_no_ack_set(cpudb_key_t dest_cpu, int no_ack);
extern int atp_cpu_no_ack_get(cpudb_key_t dest_cpu, int *no_ack);

extern bcm_rx_t atp_rx_inject(cpudb_key_t src_key, int client_id,
                              uint8 *pkt_buf, int len);


/****************************************************************
 *
 * ATP/BET client flags
 *   NO_ACK   Register client for best effort (no ACK).  Default is ACK.
 *   NEXT_HOP Register client to use next hop.  Default is C2C.
 *   REASSEM_BUF     When multi-segment packets are received,
 *            the ATP/BET layer can recopy into a new, single buffer
 *            and pass that back to the caller.  The cost here is
 *            memory allocation and copy.  Alternatively, the
 *            ATP/BET can pass back a packet structure with the
 *            segments in the pkt_blk structures.  The default is to
 *            pass back the segments in the packet.  If the client
 *            registers with this flag set, then the packet will be
 *            reassembled into a single buffer.
 *
 * ATP/BET overall flags
 *   LEARN_SLF
 *            If a valid packet is received, and the source is
 *            unknown, should it be added to the local keys?
 */

/* Client flags */
#define ATP_F_NO_ACK                     0x1
#define ATP_F_NEXT_HOP                   0x2
#define ATP_F_REASSEM_BUF                0x4

/* Start flags */
#define ATP_F_LEARN_SLF                  0x8

/* Reserved flags for internal use */
#define ATP_F_RESERVED                   0xffff0000

extern int atp_config_get(int *tx_thrd_pri, int *rx_thrd_pri,
                              bcm_trans_ptr_t **trans_ptr);
extern int atp_config_set(int tx_thrd_pri, int rx_thrd_pri,
                              bcm_trans_ptr_t *trans_ptr);

extern int atp_start(uint32 flags,
                         uint32 unit_bmp,
                         uint32 rco_flags);


extern void *atp_rx_data_alloc(int bytes);

/*
 * Must be called on the payload pointer of stolen packets
 */
extern void atp_rx_free(void *payload_ptr, void *pkt_ptr);

extern void *atp_tx_data_alloc(int bytes);
extern void atp_tx_data_free(void *ptr);

extern int atp_stop(void);
extern int atp_running(void);

extern int atp_client_add(int client_id);
extern int atp_register(int client_id,
                            uint32 flags,
                            atp_client_cb_f callback,
                            void *cookie,
                            int cos,
                            int vlan);

extern void atp_unregister(int client_id);

/*
 * Flags for ATP/BET transmit functions:  See CPUTRANS_ flags.
 */

extern int atp_tx(cpudb_key_t dest_key,
                      int client_id,
                      uint8 *pkt_data,
                      int len,
                      uint32 ct_flags,
                      atp_tx_cb_f callback,
                      void *cookie);

extern int atp_timeout_set(int retry_usecs, int num_retries);
extern int atp_timeout_get(int *retry_usecs, int *num_retries);
extern int atp_timeout_register(atp_timeout_cb_f callback);

extern int atp_segment_len_set(int seg_len);
extern int atp_segment_len_get(void);

extern int atp_pool_size_set(int tx_max, int rx_max);
extern int atp_pool_size_get(int *tx_max, int *rx_max);

extern int atp_cpu_count_set(int count);
extern int atp_cpu_count_get(void);

extern int atp_key_add(cpudb_key_t key, int is_local);
extern int atp_cpudb_keys_add(cpudb_ref_t db_ref);
extern int atp_key_remove(cpudb_key_t key);
extern int atp_key_purge(cpudb_key_t key);

extern void atp_attach_callback(int unit, int attach, cpudb_entry_t *cpuent,
                                int cpuunit);

extern int atp_cos_vlan_set(int cos, int vlan);
extern int atp_cos_vlan_get(int *cos, int *vlan);

extern int atp_db_update(cpudb_ref_t db_ref);
extern void atp_db_update_notify(void);
#if defined(BROADCOM_DEBUG)
extern void atp_dump(int verbose);
#endif /* BROADCOM_DEBUG */
#endif /* _CPUTRANS_ATP_H_ */

