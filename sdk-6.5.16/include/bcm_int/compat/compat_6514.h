/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.14 routines
 */

#ifndef _COMPAT_6514_H_
#define _COMPAT_6514_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/cosq.h>

#ifdef INCLUDE_TCB
typedef struct bcm_compat6514_cosq_tcb_config_s {
    bcm_cosq_tcb_scope_t scope_type;    /* TCB monitor scope type */
    bcm_gport_t gport;                  /* TCB monitor entity, can be a Unicast
                                           queue gport or a physical port gport */
    bcm_cos_queue_t cosq;               /* Reserved field */
    uint32 trigger_reason;              /* Define the event which could trigger
                                           the capture */
    uint32 pre_freeze_capture_num;      /* Define the capture number after TCB
                                           enter freeze status */
    uint32 pre_freeze_capture_time;     /* Define the capture time after TCB
                                           enter freeze status. Unit is usec */
    uint32 post_sample_probability;     /* The sample probability in
                                           post-trigger phase. Step is 1/16,
                                           valid configure range is 1 to 16, 1
                                           means 1/16 sample, 16 means sample
                                           all */
    uint32 pre_sample_probability;      /* The sample probability in pre-trigger
                                           phase. Step is 1/16, valid configure
                                           range is 1 to 16, 1 means 1/16
                                           sample, 16 means sample all */
} bcm_compat6514_cosq_tcb_config_t;

extern int bcm_compat6514_cosq_tcb_config_get(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6514_cosq_tcb_config_t *config);

extern int bcm_compat6514_cosq_tcb_config_set(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6514_cosq_tcb_config_t *config);
#endif
#endif
#endif /* !_COMPAT_6514_H */
