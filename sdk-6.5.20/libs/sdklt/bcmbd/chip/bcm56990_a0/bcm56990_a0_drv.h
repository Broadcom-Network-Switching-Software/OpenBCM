/*! \file bcm56990_a0_drv.h
 *
 * Local definitions for bcm56990_a0 driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_DRV_H
#define BCM56990_A0_DRV_H

extern int
bcm56990_a0_bd_dev_reset(int unit);

extern int
bcm56990_a0_bd_dev_init(int unit);

extern int
bcm56990_a0_bd_ipep_intr_drv_init(int unit);

extern int
bcm56990_a0_bd_ipep_intr_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_mmu_intr_drv_init(int unit);

extern int
bcm56990_a0_bd_mmu_intr_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_port_intr_drv_init(int unit);

extern int
bcm56990_a0_bd_port_intr_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_mmui_intr_drv_init(int unit);

extern int
bcm56990_a0_bd_mmui_intr_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_db_intr_drv_init(int unit);

extern int
bcm56990_a0_bd_db_intr_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_pvt_intr_drv_init(int unit);

extern int
bcm56990_a0_bd_pvt_intr_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_pvt_drv_init(int unit);

extern int
bcm56990_a0_bd_pvt_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_ts_drv_init(int unit);

extern int
bcm56990_a0_bd_ts_drv_cleanup(int unit);

extern uint32_t
bcm56990_a0_bd_mor_clks_read_get(int unit, bcmdrd_sid_t sid);

extern int
bcm56990_a0_bd_m0ssq_drv_init(int unit);

extern int
bcm56990_a0_bd_m0ssq_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_led_drv_init(int unit);

extern int
bcm56990_a0_bd_led_drv_cleanup(int unit);

extern int
bcm56990_a0_bd_avs_voltage_get(int unit, uint32_t *val);

#endif /* BCM56990_A0_DRV_H */
