/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bradley.h
 */

#ifndef _SOC_BRADLEY_H_
#define _SOC_BRADLEY_H_

#include <soc/drv.h>

extern int soc_bradley_misc_init(int);
extern int soc_bradley_mmu_init(int);
extern int soc_bradley_age_timer_get(int, int *, int *);
extern int soc_bradley_age_timer_max_get(int, int *);
extern int soc_bradley_age_timer_set(int, int, int);

extern void soc_bradley_mmu_parity_error(void *unit, void *d1, void *d2,
                                    void *d3, void *d4 );
#ifdef BCM_SCORPION_SUPPORT
extern void soc_scorpion_mem_config(int unit);
extern void soc_scorpion_parity_error(void *unit, void *d1, void *d2,
                                      void *d3, void *d4 );
extern int
soc_scorpion_l2_overflow_start(int unit);
extern int
soc_scorpion_l2_overflow_stop(int unit);
extern void
soc_scorpion_l2_overflow_interrupt_handler(void *unit_vp, void *d1, void *d2,
                                     void *d3, void *d4);
#endif /* BCM_SCORPION_SUPPORT */

extern soc_functions_t soc_bradley_drv_funs;

#define SOC_PIPE_SELECT_X      0
#define SOC_PIPE_SELECT_Y      1

#define SOC_HBX_HIGIG2_MULTICAST_RANGE_MAX 0xffff
#define SOC_HBX_MULTICAST_RANGE_DEFAULT 4096

extern int soc_hbx_higig2_mcast_sizes_set(int unit, int bcast_size,
                                          int mcast_size, int ipmc_size);
extern int soc_hbx_higig2_mcast_sizes_get(int unit, int *bcast_size,
                                          int *mcast_size, int *ipmc_size);

extern int soc_hbx_mcast_size_set(int unit, int mc_size);
extern int soc_hbx_mcast_size_get(int unit, int *mc_base, int *mc_size);
extern int soc_hbx_ipmc_size_set(int unit, int ipmc_size);
extern int soc_hbx_ipmc_size_get(int unit, int *ipmc_base, int *ipmc_size);

#endif	/* !_SOC_BRADLEY_H_ */
