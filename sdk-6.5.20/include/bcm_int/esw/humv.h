/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt.h
 * Purpose:     Function declarations for Firebolt bcm functions
 */

#ifndef _BCM_INT_HUMV_H_
#define _BCM_INT_HUMV_H_

#include <bcm_int/esw/mbcm.h>

/****************************************************************
 *
 * HUMV functions
 */

extern int bcm_humv_mcast_addr_add(int, bcm_mcast_addr_t *);
extern int bcm_humv_mcast_addr_remove(int, bcm_mac_t, bcm_vlan_t);
extern int bcm_humv_mcast_port_get(int, bcm_mac_t, bcm_vlan_t,
				   bcm_mcast_addr_t *);
extern int bcm_humv_mcast_init(int);
extern int _bcm_humv_mcast_detach(int);
extern int bcm_humv_mcast_addr_add_w_l2mcindex(int unit,
					       bcm_mcast_addr_t *mcaddr);
extern int bcm_humv_mcast_addr_remove_w_l2mcindex(int unit,
						  bcm_mcast_addr_t *mcaddr);
extern int bcm_humv_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr);
extern int bcm_humv_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr);

extern int bcm_humv_cosq_init(int unit);
extern int bcm_humv_cosq_detach(int unit, int software_state_only);
extern int bcm_humv_cosq_config_set(int unit, int numq);
extern int bcm_humv_cosq_config_get(int unit, int *numq);
extern int bcm_humv_cosq_mapping_set(int unit,
				     bcm_port_t port,
				     bcm_cos_t priority,
				     bcm_cos_queue_t cosq);
extern int bcm_humv_cosq_mapping_get(int unit,
				     bcm_port_t port,
				     bcm_cos_t priority,
				     bcm_cos_queue_t *cosq);
extern int bcm_humv_cosq_port_sched_set(int unit, bcm_pbmp_t, int mode,
					const int weights[], int delay);
extern int bcm_humv_cosq_port_sched_get(int unit, bcm_pbmp_t, int *mode,
					int weights[], int *delay);
extern int bcm_humv_cosq_sched_weight_max_get(int unit, int mode,
					      int *weight_max);
extern int bcm_humv_cosq_port_bandwidth_set(int unit, bcm_port_t port,
					    bcm_cos_queue_t cosq,
					    uint32 kbits_sec_min,
					    uint32 kbits_sec_max,
                                            uint32 kbits_sec_burst,
					    uint32 flags);
extern int bcm_humv_cosq_port_bandwidth_get(int unit, bcm_port_t port,
					    bcm_cos_queue_t cosq,
					    uint32 *kbits_sec_min,
					    uint32 *kbits_sec_max,
                                            uint32 *kbits_sec_burst,
					    uint32 *flags);
extern int bcm_humv_cosq_discard_set(int unit, uint32 flags);
extern int bcm_humv_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_humv_cosq_discard_port_set(int unit, bcm_port_t port,
					  bcm_cos_queue_t cosq,
					  uint32 color,
					  int drop_start,
					  int drop_slope,
					  int average_time);
extern int bcm_humv_cosq_discard_port_get(int unit, bcm_port_t port,
					  bcm_cos_queue_t cosq,
					  uint32 color,
					  int *drop_start,
					  int *drop_slope,
					  int *average_time);
#endif  /* !_BCM_INT_HUMV_H_ */
