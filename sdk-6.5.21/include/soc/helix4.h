/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix4.h
 */

#ifndef _SOC_HELIX4_H_
#define _SOC_HELIX4_H_

#include <soc/drv.h>

extern soc_functions_t soc_helix4_drv_funs;
extern int soc_hx4_mem_config(int unit, int dev_id);
extern int _soc_hx4_tcam_ser_init(int unit);
extern void soc_hx4_ser_fail(int unit);
extern int soc_hx4_port_config_init(int unit, uint16 dev_id);
extern int soc_hx4_get_port_mapping(int unit, uint16 dev_id);
extern int soc_hx4_init_port_mapping(int unit);
extern int soc_hx4_init_num_cosq(int unit);
extern int soc_hx4_port_lanes_get(int unit, soc_port_t port, int *lanes, soc_block_type_t *pbt);
extern void _soc_hx4_mmu_tdm_array_init(int unit);
extern void soc_hx4_blk_counter_config(int unit);
extern int _soc_hx4_ledup_init(int unit);
#endif  /* !_SOC_HELIX4_H_ */

