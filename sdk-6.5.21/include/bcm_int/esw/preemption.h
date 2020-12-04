/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _BCM_INT_PREEMPTION_H
#define _BCM_INT_PREEMPTION_H

#include <bcm/port.h>

#if defined(BCM_PREEMPTION_SUPPORT)
extern int
bcmi_esw_preemption_mac_config_set(int unit, bcm_port_t port,
                                   bcm_port_preempt_control_t type,
                                   uint32 arg);
extern int
bcmi_esw_preemption_hold_request_mode_set(int unit, bcm_port_t port,
                                          uint32 arg);
extern int
bcmi_esw_preemption_queue_bitmap_set(int unit, bcm_port_t port,
                                     uint32 arg);
extern int
bcmi_esw_preemption_capability_set(int unit, bcm_port_t port,
                                   uint32 arg);
extern int
bcmi_esw_preemption_frag_config_tx_set(int unit, bcm_port_t port,
                                       int is_final, uint32 arg);
extern int
bcmi_esw_preemption_mac_config_get(int unit, bcm_port_t port,
                                   bcm_port_preempt_control_t type,
                                   uint32* arg);
extern int
bcmi_esw_preemption_hold_request_mode_get(int unit, bcm_port_t port,
                                          uint32* arg);
extern int
bcmi_esw_preemption_queue_bitmap_get(int unit, bcm_port_t port,
                                     uint32* arg);
extern int
bcmi_esw_preemption_capability_get(int unit, bcm_port_t port,
                                   uint32* arg);
extern int
bcmi_esw_preemption_frag_config_tx_get(int unit, bcm_port_t port,
                                       int is_final, uint32* arg);
extern int
bcmi_esw_preemption_status_tx_get(int unit, bcm_port_t port,
                                  uint32* value);
extern int
bcmi_esw_preemption_status_verify_get(int unit, bcm_port_t port,
                                      uint32* value);
extern void
bcmi_esw_preemption_linkscan_cb(int unit, bcm_port_t port,
                                bcm_port_info_t *info);
extern void
bcmi_esw_preemption_linkscan_update(int unit,
                                    bcm_port_t port,
                                    bcm_port_info_t *portInfo);
extern int
bcmi_esw_preemption_init(int unit);

extern void
bcmi_esw_preemption_cleanup(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcmi_esw_preemption_sync(int unit, soc_scache_handle_t scache_handle);

extern int
bcmi_esw_preemption_wb_alloc(int unit, soc_scache_handle_t scache_handle);

extern int
bcmi_esw_preemption_recover(int unit, soc_scache_handle_t scache_handle);
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* BCM_PREEMPTION_SUPPORT */
#endif /* !_BCM_INT_PREEMPTION_H */
