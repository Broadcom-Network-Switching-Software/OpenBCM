/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Link module definitions internal to the BCM library.
 */

#ifndef _BCM_INT_LINK_H
#define _BCM_INT_LINK_H

#define _BCM_LINK_STATUS_NO_CALLBACK  0x1

extern int _bcm_esw_link_get(int unit, bcm_port_t port, int *link);
extern int _bcm_esw_link_force(int unit, uint32 flags, bcm_port_t port,
                               int force, int link);
extern int _bcm_esw_link_down_tx_set(int unit, bcm_port_t port, int enable);
extern int _bcm_esw_link_down_tx_get(int unit, bcm_port_t port, int *enable);
extern int _bcm_esw_link_failover_set(int unit, bcm_port_t port, int enable);
extern int _bcm_esw_link_failed_clear(int unit, bcm_port_t port);
extern int _bcm_esw_link_port_info_skip_set(int unit, bcm_port_t port, int skip);
extern int _bcm_esw_link_port_info_skip_get(int unit, bcm_port_t port, int *skip);
extern int _bcm_esw_link_fast_set(int unit, bcm_port_t port, int enable);
extern int _bcm_esw_link_fast_get(int unit, bcm_port_t port, int *enable);
extern int _bcm_esw_roe_link_down_tx_set(int unit, bcm_port_t port, int enable);
extern int _bcm_esw_roe_link_down_tx_get(int unit, bcm_port_t port, int *enable);
extern int _bcm_esw_roe_link_down_rx_set(int unit, bcm_port_t port, int enable);
extern int _bcm_esw_roe_link_down_rx_get(int unit, bcm_port_t port, int *enable);


#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_link_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_link_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

int _bcm_esw_lc_lock(int unit);
int _bcm_esw_lc_unlock(int unit);
int _bcm_esw_lc_check_init(int unit);

extern int bcm_esw_linkscan_port_attach(int unit, bcm_port_t port);
#endif	/* !_BCM_INT_LINK_H */
