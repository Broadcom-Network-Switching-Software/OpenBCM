/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        draco.h
 */

#ifndef _SOC_DRACO_H_
#define _SOC_DRACO_H_

#include <soc/drv.h>

extern int soc_draco_misc_init(int);
extern int soc_draco_mmu_init(int);
extern int soc_draco_age_timer_get(int, int *, int *);
extern int soc_draco_age_timer_max_get(int, int *);
extern int soc_draco_age_timer_set(int, int, int);
extern int soc_draco_stat_get(int, soc_port_t, int, uint64*);

extern soc_functions_t soc_draco_drv_funs;
extern soc_functions_t soc_draco15_drv_funs;

extern int soc_draco15_mmu_init(int);
extern int soc_draco15_mac_hash(int unit, uint8 *key);
extern int soc_draco15_mac_software_hash(int unit, int hash_sel, 
					    uint8 *key);

extern int soc_mem_draco15_vlan_mac_search(int unit, int *index_ptr, 
					   void *entry_data);
extern int soc_mem_draco15_vlan_mac_insert(int unit, void *entry_data);
extern int soc_mem_draco15_vlan_mac_delete(int unit, void *key_data); 

#define SOC_DRACO_PORT_TYPE_DEFAULT 0
#define SOC_DRACO_PORT_TYPE_TRUNK 1
#define SOC_DRACO_PORT_TYPE_STACKING 2
#define SOC_DRACO_PORT_TYPE_RESERVED 3

#endif	/* !_SOC_DRACO_H_ */
