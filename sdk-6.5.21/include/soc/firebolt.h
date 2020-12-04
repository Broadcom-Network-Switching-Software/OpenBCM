/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt.h
 */

#ifndef _SOC_FIREBOLT_H_
#define _SOC_FIREBOLT_H_

#include <soc/drv.h>

extern int soc_firebolt_misc_init(int);
extern int soc_firebolt_internal_mmu_init(int);
extern int soc_firebolt_mmu_init(int);
extern int soc_firebolt_age_timer_get(int, int *, int *);
extern int soc_firebolt_age_timer_max_get(int, int *);
extern int soc_firebolt_age_timer_set(int, int, int);
extern int soc_firebolt_stat_get(int, soc_port_t, int, uint64*);

extern void soc_fb_mmu_parity_error(void *unit, void *d1, void *d2,
                                    void *d3, void *d4 );
extern int soc_fb_xq_mem(int unit, soc_port_t port, soc_mem_t *xq);

extern soc_functions_t soc_firebolt_drv_funs;

extern void soc_helix_mem_config(int unit);
extern void soc_bcm53300_mem_config(int unit);

#ifdef BCM_RAPTOR_SUPPORT
extern void soc_bcm53724_mem_config(int unit);
extern void soc_bcm56225_mem_config(int unit);
#endif

/* Modport port to hg bitmap mapping function */
extern int soc_xgs3_port_to_higig_bitmap(int unit, soc_port_t port, 
                                         uint32 *hg_reg);
extern int soc_xgs3_higig_bitmap_to_port(int unit, uint32 hg_reg, 
                                         soc_port_t *port);
extern int soc_xgs3_higig_bitmap_to_port_all(int unit, uint32 hg_pbm,
                                             int port_max,
                                             soc_port_t *port_array,
                                             int *port_count);
extern int soc_xgs3_higig_bitmap_to_bitmap(int unit, pbmp_t hg_pbmp, pbmp_t *pbmp);
extern int soc_xgs3_bitmap_to_higig_bitmap(int unit, pbmp_t pbmp, pbmp_t *hg_pbm);
extern int soc_xgs3_higig_bitmap_to_higig_port_num(uint32 hg_pbm, uint32 *hg_port_num);
extern int soc_xgs3_port_num_to_higig_port_num(int unit, int port,
                                               int *hg_port);


#ifdef BCM_FIREBOLT2_SUPPORT
extern void soc_firebolt2_mem_config(int unit);
#endif /* BCM_FIREBOLT2_SUPPORT */

#define SOC_FB2_PROD_CFG_HX_SHIFT       8
#define SOC_FB2_PROD_CFG_HX_MASK        0xf

#endif	/* !_SOC_FIREBOLT_H_ */
