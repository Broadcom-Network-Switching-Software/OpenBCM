/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt.h
 * Purpose:     Function declarations for Bradley bcm functions
 */

#ifndef _BCM_INT_BRADLEY_H_
#define _BCM_INT_BRADLEY_H_

#include <bcm_int/esw/mbcm.h>

/****************************************************************
 *
 * Bradley functions
 */

extern int bcm_bradley_mcast_addr_add(int, bcm_mcast_addr_t *);
extern int bcm_bradley_mcast_addr_remove(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_bradley_mcast_port_get(int, bcm_mac_t, bcm_vlan_t,
				      bcm_mcast_addr_t *);
extern int bcm_bradley_mcast_init(int);
extern int bcm_bradley_mcast_addr_add_w_l2mcindex(int unit,
						  bcm_mcast_addr_t *mcaddr);
extern int bcm_bradley_mcast_addr_remove_w_l2mcindex(int unit,
						     bcm_mcast_addr_t *mcaddr);
extern int bcm_bradley_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr);
extern int bcm_bradley_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr);

extern int bcm_bradley_cosq_init(int unit);
extern int bcm_bradley_cosq_detach(int unit, int software_state_only);
extern int bcm_bradley_cosq_config_set(int unit, int numq);
extern int bcm_bradley_cosq_config_get(int unit, int *numq);
extern int bcm_bradley_cosq_mapping_set(int unit,
					bcm_port_t port,
					bcm_cos_t priority,
					bcm_cos_queue_t cosq);
extern int bcm_bradley_cosq_mapping_get(int unit,
					bcm_port_t port,
					bcm_cos_t priority,
					bcm_cos_queue_t *cosq);
extern int bcm_bradley_cosq_port_sched_set(int unit, bcm_pbmp_t, int mode,
					   const int weights[], int delay);
extern int bcm_bradley_cosq_port_sched_get(int unit, bcm_pbmp_t, int *mode,
					   int weights[], int *delay);
extern int bcm_bradley_cosq_sched_weight_max_get(int unit, int mode,
						 int *weight_max);
extern int bcm_bradley_cosq_port_bandwidth_set(int unit, bcm_port_t port,
					       bcm_cos_queue_t cosq,
					       uint32 kbits_sec_min,
					       uint32 kbits_sec_max,
                                               uint32 kbits_sec_burst,
					       uint32 flags);
extern int bcm_bradley_cosq_port_bandwidth_get(int unit, bcm_port_t port,
					       bcm_cos_queue_t cosq,
					       uint32 *kbits_sec_min,
					       uint32 *kbits_sec_max,
                                               uint32 *kbits_sec_burst,
					       uint32 *flags);
extern int bcm_bradley_cosq_discard_set(int unit, uint32 flags);
extern int bcm_bradley_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_bradley_cosq_discard_port_set(int unit, bcm_port_t port,
					     bcm_cos_queue_t cosq,
					     uint32 color,
					     int drop_start,
					     int drop_slope,
					     int average_time);
extern int bcm_bradley_cosq_discard_port_get(int unit, bcm_port_t port,
					     bcm_cos_queue_t cosq,
					     uint32 color,
					     int *drop_start,
					     int *drop_slope,
					     int *average_time);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_bradley_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_bradley_cosq_sw_dump(int unit);
#endif /*  BCM_SW_STATE_DUMP_DISABLE */

extern int _bcm_xgs3_ipmc_bitmap_set(int unit, int ipmc_idx,
                                     bcm_pbmp_t pbmp);
extern int _bcm_xgs3_ipmc_bitmap_get(int unit, int ipmc_idx,
                                     bcm_pbmp_t *pbmp);
extern int _bcm_xgs3_ipmc_bitmap_del(int unit, int ipmc_idx,
                                     bcm_pbmp_t pbmp);
extern int _bcm_xgs3_ipmc_bitmap_clear(int unit, int ipmc_idx);

#endif  /* !_BCM_INT_BRADLEY_H_ */
