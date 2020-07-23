/*! \file bcm56990_a0_tm_pfc.h
 *
 * File containing PFC related defines and internal function for
 * bcm56990_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_PFC_H
#define BCM56990_A0_TM_PFC_H

#include <bcmpc/bcmpc_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>

int
bcm56990_a0_pfc_deadlock_recovery_begin(int unit, int lport, int pri);

int
bcm56990_a0_pfc_deadlock_recovery_end(int unit, int lport, int pri);

int
bcm56990_a0_pfc_deadlock_hw_status_get(int unit, int32_t lport,
                                       uint32_t *mask, uint32_t *status);
int
bcm56990_a0_pfc_deadlock_config_hw_set(int unit,
                                       uint32_t fid,
                                       uint32_t lport,
                                       uint32_t pri,
                                       uint32_t val);
int
bcm56990_a0_tm_pfc_egr_port_set(int unit,
                                bcmlrd_sid_t sid,
                                bcmpc_lport_t lport,
                                uint32_t pfc_enable,
                                uint32_t profile_id);
int
bcm56990_a0_tm_pfc_rx_pri_cos_map_set(int unit,
                                      int profile_id,
                                      tm_pfc_pri_to_cos_map_config_t map[]);

#endif /* BCM56990_A0_TM_PFC_H */
