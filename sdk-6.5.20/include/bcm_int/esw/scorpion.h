/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        scorpion.h
 * Purpose:     Function declarations for Scorpion bcm functions
 */

#ifndef _BCM_INT_SCORPION_H_
#define _BCM_INT_SCORPION_H_
#if defined(BCM_SCORPION_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#ifdef BCM_FIELD_SUPPORT 
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */

/****************************************************************
 *
 * Scorpion functions
 */
#if defined(INCLUDE_L3)
extern int _bcm_sc_defip_init(int unit);
extern int _bcm_sc_defip_deinit(int unit);
#endif /* INCLUDE_L3 */

#if defined(BCM_FIELD_SUPPORT)

extern int _bcm_field_sc_init(int unit, _field_control_t *fc);

#endif /* BCM_FIELD_SUPPORT */

extern int bcm_sc_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 kbits_sec_min,
                                          uint32 kbits_sec_max,
                                          uint32 kbits_sec_burst,
                                          uint32 flags);
extern int bcm_sc_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *kbits_sec_min,
                                          uint32 *kbits_sec_max,
                                          uint32 *kbits_sec_burst,
                                          uint32 *flags);
extern int bcm_sc_cosq_discard_set(int unit, uint32 flags);
extern int bcm_sc_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_sc_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_sc_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);

extern int bcm_sc_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_sc_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);

#endif  /* BCM_SCORPION_SUPPORT */

#endif  /* !_BCM_INT_SCORPION_H_ */
