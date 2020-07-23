/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _BCM_INT_PREEMPTION_CTR_H_
#define _BCM_INT_PREEMPTION_CTR_H_
#include <bcm/port.h>
#include <sal/core/sync.h>

#if defined(BCM_PREEMPTION_SUPPORT)
/* device-specific function */
typedef struct bcmi_preemption_counte_func_s {
    int (*preemption_counter_init) (int);
    int (*preemption_counter_clean) (int);
    int (*preemption_counter_get)(int, int, bcm_port_t,
                                  bcm_port_stat_t, uint64*);
    int (*preemption_counter_set)(int, bcm_port_t,
                                  bcm_port_stat_t, uint64);
} bcmi_preemption_counte_func_t;

extern int
bcmi_esw_preemption_counter_set(int unit, bcm_port_t local_port,
                                bcm_port_stat_t stat, uint64 val);
extern int
bcmi_esw_preemption_counter_set32(int unit, bcm_port_t local_port,
                                  bcm_port_stat_t stat, uint32 val);
extern int
bcmi_esw_preemption_counter_multi_set(int unit,
                                      bcm_port_t local_port, int nstat,
                                      bcm_port_stat_t *stat_arr,
                                      uint64 *value_arr);
extern int
bcmi_esw_preemption_counter_multi_set32(int unit,
                                        bcm_port_t local_port, int nstat,
                                        bcm_port_stat_t *stat_arr,
                                        uint32 *value_arr);
extern int
bcmi_esw_preemption_counter_get(int unit, int sync_mode, bcm_port_t local_port,
                                bcm_port_stat_t stat, uint64 *val);
extern int
bcmi_esw_preemption_counter_get32(int unit, int sync_mode, bcm_port_t local_port,
                                  bcm_port_stat_t stat, uint32 *val);
extern int
bcmi_esw_preemption_counter_multi_get(int unit, int sync_mode,
                                      bcm_port_t local_port, int nstat,
                                      bcm_port_stat_t *stat_arr,
                                      uint64 *value_arr);
extern int
bcmi_esw_preemption_counter_multi_get32(int unit, int sync_mode,
                                        bcm_port_t local_port, int nstat,
                                        bcm_port_stat_t *stat_arr,
                                        uint32 *value_arr);
extern int
bcmi_esw_preemption_counter_init(int unit);
extern int
bcmi_esw_preemption_counter_cleanup(int unit);

#endif /* BCM_PREEMPTION_SUPPORT */
#endif /* !_BCM_INT_PREEMPTION_CTR_H_ */

