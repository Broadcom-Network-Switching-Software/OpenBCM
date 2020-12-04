/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane.h
 * Purpose:     Function declarations for Hurricane  bcm functions
 */

#ifndef _BCM_INT_HURRICANE2_H_
#define _BCM_INT_HURRICANE2_H_
#include <bcm_int/esw/l2.h>
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>

#ifdef INCLUDE_L3
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#endif
extern int bcm_hu2_cosq_init(int unit);
extern int bcm_hu2_cosq_detach(int unit, int software_state_only);
extern int bcm_hu2_cosq_config_set(int unit, int numq);
extern int bcm_hu2_cosq_config_get(int unit, int *numq);
extern int bcm_hu2_cosq_mapping_set(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_hu2_cosq_mapping_get(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int     
bcm_hu2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                              int mode, const int weights[], int delay);
extern int
bcm_hu2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int weights[], int *delay);
extern int
bcm_hu2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags);
extern int
bcm_hu2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *kbits_sec_min,
                               uint32 *kbits_sec_max,
                               uint32 *kbits_sec_burst,
                               uint32 *flags);

extern int bcm_hr2_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_hr2_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_hr2_cosq_port_burst_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_hr2_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *burst);

extern int
bcm_hu2_cosq_discard_set(int unit, uint32 flags);
extern int
bcm_hu2_cosq_discard_get(int unit, uint32* flags);
extern int
bcm_hu2_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope,
                             int average_time);
extern int
bcm_hu2_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope,
                             int *average_time);

extern int
bcm_hu2_cosq_sched_weight_max_get(int unit, int mode, int *weight_max);

extern int
bcm_hu2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags);

extern int
bcm_hu2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight);

extern int
bcm_hu2_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                            bcm_cos_queue_t cosq, int *mode, int *weight);

extern int
bcm_hu2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst);

extern int
bcm_hu2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 *kbits_burst);

extern int
bcm_hu2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags);

extern int
bcm_hu2_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard);

extern int
bcm_hu2_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard);

extern int
bcm_hr2_dual_port_mode_reinit(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_hu2_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_hu2_cosq_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef INCLUDE_L3
extern int _bcm_hu2_lpm_get(int unit, _bcm_defip_cfg_t *lpm_cfg, int *nh_ecmp_idx);
extern int _bcm_hu2_lpm_add(int unit, _bcm_defip_cfg_t *lpm_cfg, int nh_ecmp_idx);
extern int _bcm_hu2_lpm_del(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int _bcm_hu2_lpm_update_match(int unit, _bcm_l3_trvrs_data_t *trv_data);
#endif  /* INCLUDE_L3 */

#ifdef BCM_FIELD_SUPPORT 
extern int _bcm_field_hu2_init(int unit, _field_control_t *fc);
extern int
_bcm_field_hu2_selcodes_install(int unit, _field_group_t *fg,
                                uint8 slice_numb, bcm_pbmp_t pbmp,
                                int selcode_index);
extern int
_bcm_field_hu2_mode_set(int unit, uint8 slice_numb, _field_group_t *fg, uint8 flags);
extern int
_bcm_field_hu2_lookup_mode_set(int unit, uint8 slice_numb,
                              _field_group_t *fg, uint8 flags);
extern int
_bcm_field_hu2_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs);

#endif

#endif  /* BCM_HURRICANE2_SUPPORT */
#endif  /* !_BCM_INT_HURRICANE2_H_ */
