/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Remote Linkscan
 */

#ifndef	_BCM_INT_RLINK_H
#define	_BCM_INT_RLINK_H

#include <sdk_config.h>
#include <bcm/pkt.h>
#include <bcm/l2.h>

#ifdef	BCM_RPC_SUPPORT

#ifndef BCM_RLINK_RX_REMOTE_MAX_DEFAULT
#define BCM_RLINK_RX_REMOTE_MAX_DEFAULT {10, 10, 10, 20, 20, 20, 50, 80}
#endif

#ifndef BCM_RLINK_L2_REMOTE_MAX_DEFAULT
#define BCM_RLINK_L2_REMOTE_MAX_DEFAULT 50
#endif

typedef int (*bcm_rlink_l2_cb_f)(int unit, bcm_l2_addr_t *l2addr, int insert);

extern int	bcm_rlink_start(void);
extern int	bcm_rlink_stop(void);

extern void     bcm_rlink_device_clear(int unit);
extern int      bcm_rlink_device_detach(int unit);

extern void     bcm_rlink_server_clear(void);
extern void     bcm_rlink_client_clear(void);
extern void     bcm_rlink_clear(void);

extern int bcm_rlink_rx_connect(int unit);
extern int bcm_rlink_rx_disconnect(int unit);

extern void bcm_rlink_l2_cb_set(bcm_rlink_l2_cb_f l2_cb);
extern void bcm_rlink_l2_cb_get(bcm_rlink_l2_cb_f *l2_cb);

extern void bcm_rlink_l2_native_only_set(int native_only);
extern void bcm_rlink_l2_native_only_get(int *native_only);

extern int bcm_rlink_traverse_message(bcm_pkt_t *rx_pkt,
                                      uint8 *data_in, int len_in,
                                      uint8 *data_out, int len_out,
                                      int *len_out_actual);

extern int bcm_rlink_traverse_init(void);
extern int bcm_rlink_traverse_deinit(void);
extern int bcm_rlink_traverse_client_init(void);
extern int bcm_rlink_traverse_client_deinit(void);
extern int bcm_rlink_traverse_server_init(void);
extern int bcm_rlink_traverse_server_deinit(void);
extern int bcm_rlink_traverse_server_clear(void);
extern int bcm_rlink_traverse_client_clear(void);
#if defined(BROADCOM_DEBUG)
extern void bcm_rlink_dump(void);
#endif /* BROADCOM_DEBUG */
#endif	/* BCM_RPC_SUPPORT */
#endif	/* _BCM_INT_RLINK_H */
