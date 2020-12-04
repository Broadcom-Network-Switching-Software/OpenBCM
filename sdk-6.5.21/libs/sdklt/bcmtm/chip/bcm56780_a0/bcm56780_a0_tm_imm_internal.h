/*! \file bcm56780_a0_tm_imm_internal.h
 *
 * BCMTM in-memory table (IMM) utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TM_IMM_INTERNAL_H
#define BCM56780_A0_TM_IMM_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*
 * Operational state enums:
 * Defines must be consistent with the enum define in FLTG.
 */
/* TM_ENTRY_STATE_T (to be removed) */
#define VALID                         0
#define PORT_INFO_UNAVAILABLE         1


#define PORT_DELETE 0
#define PORT_ADD    1
#define UPDATE_ONLY 2

extern int
bcm56780_a0_tm_thd_ing_port_pri_grp_register(int unit);

extern int
bcm56780_a0_tm_imm_init(int unit);

extern int
bcm56780_a0_tm_thd_ing_port_pri_grp_populate(int unit);

extern int
bcm56780_a0_tm_thd_ing_port_pri_grp_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_imm_populate(int unit);

extern int
bcm56780_a0_tm_imm_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_ing_port_service_pool_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_ing_port_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_thd_ing_port_service_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_egr_mc_port_service_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_egr_mc_port_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_thd_egr_mc_port_service_pool_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_egr_uc_port_service_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_egr_uc_port_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_thd_egr_uc_port_service_pool_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_port_mc_q_to_service_pool_register(int unit);

extern int
bcm56780_a0_tm_port_mc_q_to_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_port_mc_q_to_service_pool_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_port_uc_q_to_service_pool_register(int unit);

extern int
bcm56780_a0_tm_port_uc_q_to_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_port_uc_q_to_service_pool_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_mc_q_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_q_grp_register(int unit);

extern int
bcm56780_a0_tm_thd_q_grp_populate(int unit);

extern int
bcm56780_a0_tm_thd_q_grp_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_mc_q_populate(int unit);

extern int
bcm56780_a0_tm_thd_mc_q_register(int unit);

extern int
bcm56780_a0_tm_thd_uc_q_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_uc_q_populate(int unit);

extern int
bcm56780_a0_tm_thd_uc_q_register(int unit);

extern int
bcm56780_a0_tm_ing_port_register(int unit);

extern int
bcm56780_a0_tm_ing_port_populate(int unit);

extern int
bcm56780_a0_tm_ing_port_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_ing_port_pri_grp_register(int unit);

extern int
bcm56780_a0_tm_ing_port_pri_grp_populate(int unit);

extern int
bcm56780_a0_tm_ing_port_pri_grp_update(int unit, int pport, int up);

extern int
bcm56780_a0_tm_thd_egr_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_thd_egr_service_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_ing_headroom_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_ing_headroom_pool_populate(int unit);

extern int
bcm56780_a0_tm_thd_mc_egr_service_pool_populate(int unit);

extern int
bcm56780_a0_tm_thd_mc_egr_service_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_ing_service_pool_register(int unit);

extern int
bcm56780_a0_tm_thd_ing_service_pool_populate(int unit);
#endif /* BCM56780_A0_TM_IMM_INTERNAL_H */
