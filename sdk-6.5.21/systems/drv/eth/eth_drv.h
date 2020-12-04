/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ETH_DRV_H
#define ETH_DRV_H

typedef int (*rx_cb)(int unit, unsigned char *buf, int len, void *cookie);

extern int eth_drv_init(int unit);
extern int eth_drv_register(int unit, rx_cb _rx, void *cookie);
extern int eth_drv_unregister(int unit, rx_cb _rx);
extern int eth_drv_start(int unit);
extern int eth_drv_stop(int unit);
extern int eth_drv_tx(int unit, unsigned char * pkt, int len, void *cookie);
extern int eth_drv_get_mac(int unit, unsigned char *mac);
extern void et_drv_mii_rx(int unit, unsigned char *b, int *l);

#endif /* ETH_DRV_H */
