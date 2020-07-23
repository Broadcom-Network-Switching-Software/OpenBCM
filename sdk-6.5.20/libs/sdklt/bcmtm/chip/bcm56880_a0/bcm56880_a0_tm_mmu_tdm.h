/*! \file bcm56880_a0_tm_mmu_tdm.h
 *
 * File contains MMU init and flexport related defines for bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_MMU_TDM_H
#define BCM56880_A0_TM_MMU_TDM_H

typedef enum {
    TDM_MMU_MAIN_SCHED,
    TDM_MMU_EB_PORT_SCHED,
    TDM_MMU_EB_PKT_SCHED
} mmu_sched_inst_name_e;

extern int
td4_mmu_tdm_port_down(int unit, int *down_bmp);

extern int
td4_mmu_tdm_port_up(int unit, int *up_bmp);

extern int
td4_mmu_get_speed_decode(int unit, int speed, uint32_t *speed_decode);

extern int
td4_tdm_global_to_local_phy_num(int phy_port, uint32_t *local_phy_num);

#endif /* BCM56880_A0_TM_MMU_TDM_H */
