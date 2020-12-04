/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        enduro.h
 */

#ifndef _SOC_ENDURO_H_
#define _SOC_ENDURO_H_

#include <soc/drv.h>
#include <shared/sram.h>

typedef int (*soc_enduro_oam_handler_t)(int unit, soc_field_t fault_field);

typedef _shr_ext_sram_entry_t en_ext_sram_entry_t;
 
#define SOC_EN_BROADCAST_RANGE_DEFAULT 4096
#define SOC_EN_L2_MULTICAST_RANGE_DEFAULT 1024
#define SOC_EN_IP_MULTICAST_RANGE_DEFAULT 2048

extern int soc_enduro_misc_init(int);
extern int soc_enduro_mmu_init(int);
extern int soc_enduro_age_timer_get(int, int *, int *);
extern int soc_enduro_age_timer_max_get(int, int *);
extern int soc_enduro_age_timer_set(int, int, int);
extern void soc_enduro_oam_handler_register(int unit, soc_enduro_oam_handler_t handler);
extern void soc_enduro_parity_error(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4);
extern int _soc_enduro_mem_parity_control(int unit, soc_mem_t mem,
                                            int copyno, int enable);
extern void soc_enduro_mem_config(int unit);
extern soc_functions_t soc_enduro_drv_funs;
extern void soc_enduro_mem_nack(void *unit_vp, void *addr_vp, void *blk_vp, 
                     void *d3, void *d4);
extern int soc_enduro_ser_inject_error(int unit, uint32 flags, soc_mem_t mem,
                               int pipeTarget, int block, int index);
extern soc_error_t soc_enduro_ser_error_injection_support (int unit, soc_mem_t mem, int pipe);
extern int soc_enduro_l3_defip_index_map(int unit, soc_mem_t mem, int index);
extern int soc_enduro_l3_defip_index_remap(int unit, soc_mem_t mem, int index);
extern int soc_enduro_mem_index_remap(int unit, soc_mem_t mem, int index);
#endif	/* !_SOC_ENDURO_H_ */
