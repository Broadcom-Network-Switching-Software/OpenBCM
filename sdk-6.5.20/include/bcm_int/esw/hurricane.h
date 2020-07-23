/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane.h
 * Purpose:     Function declarations for Hurricane  bcm functions
 */

#ifndef _BCM_INT_HURRICANE_H_
#define _BCM_INT_HURRICANE_H_
#include <bcm_int/esw/l2.h>
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */

#if defined(BCM_HURRICANE_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>


extern int bcm_hu_cosq_init(int unit);
extern int bcm_hu_cosq_detach(int unit, int software_state_only);
extern int bcm_hu_cosq_config_set(int unit, int numq);
extern int bcm_hu_cosq_config_get(int unit, int *numq);
extern int bcm_hu_cosq_mapping_set(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_hu_cosq_mapping_get(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int     
bcm_hu_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                              int mode, const int weights[], int delay);
extern int
bcm_hu_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int weights[], int *delay);
extern int
bcm_hu_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags);
extern int
bcm_hu_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *kbits_sec_min,
                               uint32 *kbits_sec_max,
                               uint32 *kbits_sec_burst,
                               uint32 *flags);

extern int
bcm_hu_cosq_discard_set(int unit, uint32 flags);
extern int
bcm_hu_cosq_discard_get(int unit, uint32* flags);
extern int
bcm_hu_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope,
                             int average_time);
extern int
bcm_hu_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope,
                             int *average_time);

extern int
bcm_hu_cosq_sched_weight_max_get(int unit, int mode, int *weight_max);

extern int
bcm_hu_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags);

extern int
bcm_hu_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight);

extern int
bcm_hu_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                            bcm_cos_queue_t cosq, int *mode, int *weight);

extern int
bcm_hu_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst);

extern int
bcm_hu_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 *kbits_burst);

extern int
bcm_hu_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags);

extern int
bcm_hu_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard);

extern int
bcm_hu_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_hu_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_hu_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef INCLUDE_L3
extern int _bcm_hu_l3_intf_mtu_get(int unit, _bcm_l3_intf_cfg_t *intf_info);
extern int _bcm_hu_l3_intf_mtu_set(int unit, _bcm_l3_intf_cfg_t *intf_info);
#endif  /* INCLUDE_L3 */

extern int _bcm_hu_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_hu_port_lanes_get(int unit, bcm_port_t port, int *value);

#ifdef BCM_FIELD_SUPPORT 
extern int _bcm_field_hu_init(int unit, _field_control_t *fc);
#endif

#endif  /* BCM_HURRICANE_SUPPORT */
#endif  /* !_BCM_INT_HURRICANE_H_ */
