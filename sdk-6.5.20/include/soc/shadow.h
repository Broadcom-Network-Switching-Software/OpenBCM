/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        shadow.h
 */

#ifndef _SOC_SHADOW_H_
#define _SOC_SHADOW_H_

#include <soc/drv.h>

extern int soc_shadow_misc_init(int);
extern int soc_shadow_petrab_misc_init(int);
extern int soc_shadow_mmu_init(int);
extern int soc_shadow_petrab_mmu_init(int);
extern int soc_shadow_age_timer_get(int, int *, int *);
extern int soc_shadow_age_timer_max_get(int, int *);
extern int soc_shadow_age_timer_set(int, int, int);

extern void soc_shadow_mmu_parity_error(void *unit, void *d1, void *d2,
                                    void *d3, void *d4 );
extern soc_functions_t soc_shadow_drv_funs;
extern int _bcm_shadow_port_lanes_set(int unit, soc_port_t port, int value);
extern int
soc_shadow_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);
#endif	/* !_SOC_SHADOW_H_ */
