/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hercules.h
 */

#ifndef _SOC_HERCULES_H_
#define _SOC_HERCULES_H_

#include <soc/drv.h>

extern int soc_hercules_misc_init(int);
extern int soc_hercules_mmu_init(int);
extern int soc_hercules_age_timer_get(int, int *, int *);
extern int soc_hercules_age_timer_max_get(int, int *);
extern int soc_hercules_age_timer_set(int, int, int);
extern int soc_hercules_stat_get(int, soc_port_t, int, uint64*);
extern int soc_hercules_mem_read(int unit, soc_mem_t mem, int copyno,
                                 int index, void *entry_data); 
extern int soc_hercules_mem_read_word(int unit, uint32 addr, void *word_data);
extern int soc_hercules_mem_write(int unit, soc_mem_t mem, int copyno,
                                 int index, void *entry_data); 
extern int soc_hercules_mem_write_word(int unit, uint32 addr, void *word_data);

extern soc_functions_t soc_hercules_drv_funs;
extern soc_functions_t soc_hercules15_drv_funs;

extern int soc_hercules_uc_port_set_all(int unit, int modid, pbmp_t pbmp);
extern int soc_hercules_vlan_port_set_all(int unit, vlan_id_t vid, pbmp_t pbmp);
extern int soc_hercules_mmu_limits_config(int unit, int port, int num_ports, 
                              int num_cos, int lossless);
extern int soc_hercules15_mmu_limits_config(int unit, int port, int num_ports, 
                              int num_cos, int lossless);

#endif	/* !_SOC_HERCULES_H_ */
