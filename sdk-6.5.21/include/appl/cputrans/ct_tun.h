/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ct_tun.h
 * Purpose:     
 */

#ifndef   _CPUTRANS_CT_TUN_H_
#define   _CPUTRANS_CT_TUN_H_

#include <bcm/rx.h>

#include <appl/cpudb/cpudb.h>
#include <appl/cputrans/atp.h>

#if defined(BCM_RPC_SUPPORT)

/*
 * Transmit function to call to tunnel data.  REQUIRED.  pkt_data does not
 * include any transport header (eg no CPUTRANS_HEADER).  But other
 * encapsulation data (source port, original COS, etc) is included.
 */

typedef void (*ct_tunnel_done_cb_f)(uint8 *pkt_data, void *cookie, int rv);

/*
 * Tunnel filter.  Optional.  If present (set), this function is
 * called before tunnelling the packet.
 */

typedef bcm_cpu_tunnel_mode_t (*ct_rx_tunnel_filter_f)(int unit,
                                                       bcm_pkt_t *pkt);

extern int ct_rx_tunnel_filter_set(ct_rx_tunnel_filter_f t_cb);
extern int ct_rx_tunnel_filter_get(ct_rx_tunnel_filter_f *t_cb);

typedef int (*ct_rx_tunnel_direct_f)(int unit, bcm_pkt_t *pkt,
                                     cpudb_key_t *cpu);

extern int ct_rx_tunnel_direct_set(ct_rx_tunnel_direct_f dif_fn);
extern int ct_rx_tunnel_direct_get(ct_rx_tunnel_direct_f *dif_fn);

extern int ct_rx_tunnel_mode_default_set(int mode);
extern bcm_cpu_tunnel_mode_t ct_rx_tunnel_mode_default_get(void);
extern int ct_rx_tunnel_priority_set(int priority);
extern int ct_rx_tunnel_priority_get(void);

extern void ct_rx_tunnelled_pkt_handler(cpudb_key_t src_key,
                                        uint8 *payload, int len);

extern bcm_cpu_tunnel_mode_t ct_rx_tunnel_check(int unit, bcm_pkt_t *pkt,
                                                int *no_ack, int *check_cpu,
                                                cpudb_key_t *cpu);

extern int ct_rx_tunnel_header_bytes_get(void);
extern uint8 *ct_rx_tunnel_pkt_pack(bcm_pkt_t *pkt, uint8 *buf);
extern int ct_rx_tunnel_pkt_unpack(uint8 *pkt_data, int pkt_len,
                                   bcm_pkt_t *pkt, uint8 **payload_start,
                                   int *payload_len);

/****************************************************************
 *
 * Packet TX tunnelling code
 */

extern int ct_tx_tunnel_setup(void);
extern int ct_tx_tunnel_stop(void);

extern int ct_tx_tunnel(bcm_pkt_t *pkt,
                        int dest_unit,   /* BCM unit */
                        int remote_port,
                        uint32 flags,  /* See include/bcm/pkt.h */
                        bcm_cpu_tunnel_mode_t mode);

extern int ct_tx_tunnel_forward(uint8 *pkt_data, int len);

#endif /* BCM_RPC_SUPPORT */

#endif /* !_CPUTRANS_CT_TUN_H_ */
