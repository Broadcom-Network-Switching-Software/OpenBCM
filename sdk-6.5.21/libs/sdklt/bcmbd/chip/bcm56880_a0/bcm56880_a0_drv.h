/*! \file bcm56880_a0_drv.h
 *
 * Local definitions for bcm56880_a0 driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_DRV_H
#define BCM56880_A0_DRV_H

extern int
bcm56880_a0_bd_dev_reset(int unit);

extern int
bcm56880_a0_bd_dev_init(int unit);

extern int
bcm56880_a0_bd_mmu_intr_drv_init(int unit);

extern int
bcm56880_a0_bd_mmu_intr_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_port_intr_drv_init(int unit);

extern int
bcm56880_a0_bd_port_intr_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_ipep_intr_drv_init(int unit);

extern int
bcm56880_a0_bd_ipep_intr_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_ts_drv_init(int unit);

extern int
bcm56880_a0_bd_ts_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_mcs_drv_init(int unit);

extern int
bcm56880_a0_bd_mcs_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_m0ssq_drv_init(int unit);

extern int
bcm56880_a0_bd_m0ssq_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_led_drv_init(int unit);

extern int
bcm56880_a0_bd_led_drv_cleanup(int unit);

extern uint32_t
bcm56880_a0_bd_mor_clks_read_get(int unit, bcmdrd_sid_t sid);

extern int
bcm56880_a0_bd_pvt_intr_drv_init(int unit);

extern int
bcm56880_a0_bd_pvt_intr_drv_cleanup(int unit);

extern int
bcm56880_a0_bd_avs_voltage_get(int unit, uint32_t *val);

#endif /* BCM56880_A0_DRV_H */
