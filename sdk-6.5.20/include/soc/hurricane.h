/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane.h
 */

#ifndef _SOC_HURRICANE_H_
#define _SOC_HURRICANE_H_

#include <soc/drv.h>
#include <shared/sram.h>

typedef int (*soc_hurricane_oam_handler_t)(int unit, soc_field_t fault_field);

typedef _shr_ext_sram_entry_t hu_ext_sram_entry_t;
 
extern int soc_hurricane_misc_init(int);
extern int soc_hurricane_mmu_init(int);
extern int soc_hurricane_age_timer_get(int, int *, int *);
extern int soc_hurricane_age_timer_max_get(int, int *);
extern int soc_hurricane_age_timer_set(int, int, int);
extern void soc_hurricane_oam_handler_register(soc_hurricane_oam_handler_t handler);
extern void soc_hurricane_parity_error(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4);
extern int _soc_hurricane_mem_parity_control(int unit, soc_mem_t mem,
                                            int copyno, int enable);                                     
extern int soc_hu_xqport_mode_change(int unit, soc_port_t port, 
                                      soc_mac_mode_t mode);
extern soc_functions_t soc_hurricane_drv_funs;

extern int soc_hu_xq_mem(int unit, soc_port_t port, soc_mem_t *xq);
#endif	/* !_SOC_HURRICANE_H_ */
