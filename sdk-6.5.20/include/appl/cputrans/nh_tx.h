/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        nh_tx.h
 * Purpose:     Header file for next hop transmit functions
 */

#ifndef   _CPUTRANS_NH_TX_H_
#define   _CPUTRANS_NH_TX_H_

#include <sdk_config.h>
#include <shared/idents.h>

#include <bcm/types.h>
#include <bcm/rx.h>

/* Next hop callback function prototype */
typedef int (*nh_tx_cb_f)(int unit, int port, uint8 *pkt_buf, int len,
                           void *cookie);

/* Next hop transmit function prototype */
typedef int (*nh_tx_tx_f)(int unit, int port, uint8 *pkt_buf, int len,
                           nh_tx_cb_f callback, void *cookie);

extern int nh_tx_setup(bcm_trans_ptr_t *trans_ptr);
extern int nh_tx_setup_done(void);

extern void nh_tx_l2_header_setup(uint8 *pkt_buf, uint16 vlan);

extern int nh_tx_dest_set(const bcm_mac_t mac, int dest_mod, int dest_port);
extern int nh_tx_dest_get(bcm_mac_t *mac, int *dest_mod, int *dest_port);
extern int nh_tx_dest_install(int install, int vid);

extern int nh_tx_src_set(int src_mod, int src_port);
extern int nh_tx_src_get(int *src_mod, int *src_port);

extern int nh_tx_trans_ptr_set(int unit, int port, bcm_trans_ptr_t *ptr);
extern int nh_tx_trans_ptr_get(int unit, int port, bcm_trans_ptr_t **ptr);

extern int nh_tx_src_mod_port_set(int unit, int port,
                                  int src_mod, int src_port);
extern int nh_tx_src_mod_port_get(int unit, int port,
                                  int *src_mod, int *src_port);

extern int nh_tx(int unit,
                 int port,
                 uint8 *pkt_buf,
                 int len,
                 int cos,
                 int vlan,
                 uint16 pkt_type,
                 uint32 ct_flags,
                 nh_tx_cb_f callback,
                 void *cookie);

extern void nh_pkt_setup(bcm_pkt_t *pkt, int cos, uint16 vlan,
                         uint16 pkt_type, uint32 ct_flags);
extern void nh_pkt_local_setup(bcm_pkt_t *pkt, int unit, int port);
extern void nh_pkt_final_setup(bcm_pkt_t *pkt, int unit, int port);
extern int nh_pkt_tx(bcm_pkt_t *pkt, nh_tx_cb_f callback, void *cookie);

extern int nh_tx_pkt_recognize(uint8 *pkt_buf, uint16 *pkt_type);

extern int nh_tx_local_mac_set(const bcm_mac_t new_mac);
extern int nh_tx_local_mac_get(bcm_mac_t *local_mac);

extern int nh_tx_snap_set(const bcm_mac_t new_mac, uint16 new_type,
                          uint16 new_local_type);
extern int nh_tx_snap_get(bcm_mac_t snap_mac, uint16 *snap_type,
                          uint16 *local_type);

extern int nh_tx_reset(int reset_to_defaults);

extern int nh_tx_unknown_modid_get(int *modid);
extern int nh_tx_unknown_modid_set(int modid);

#ifndef NH_TX_DEST_MAC_DEFAULT
#define NH_TX_DEST_MAC_DEFAULT      {0x00, 0x10, 0x18, 0xff, 0xff, 0xff}
#endif
#ifndef NH_TX_SNAP_MAC_DEFAULT
#define NH_TX_SNAP_MAC_DEFAULT      SHARED_BRCM_NTSW_SNAP_MAC
#endif
#ifndef NH_TX_SNAP_TYPE_DEFAULT
#define NH_TX_SNAP_TYPE_DEFAULT     SHARED_BRCM_NTSW_SNAP_TYPE
#endif
#define NH_TX_LOCAL_PKT_TYPE        SHARED_PKT_TYPE_NH_TX
#ifndef NH_TX_DEST_PORT_DEFAULT
#define NH_TX_DEST_PORT_DEFAULT     0
#endif
#ifndef NH_TX_DEST_MOD_DEFAULT
#define NH_TX_DEST_MOD_DEFAULT      0
#endif
#ifndef NH_TX_SRC_PORT_DEFAULT
#define NH_TX_SRC_PORT_DEFAULT      0
#endif
#ifndef NH_TX_SRC_MOD_DEFAULT
#define NH_TX_SRC_MOD_DEFAULT       31
#endif
#ifndef NH_TX_NUM_PKTS_DEFAULT
#define NH_TX_NUM_PKTS_DEFAULT      10
#endif


#endif /* _CPUTRANS_NH_TX_H_ */
