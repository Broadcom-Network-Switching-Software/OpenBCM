/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PFC Deadlock Detection & Recovery
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/tomahawk.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/tomahawk.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
/* Chip specific init and assign default values */
int
_bcm_th_pfc_deadlock_init(int unit)
{
    _bcm_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_hw_resorces_t *hw_res = NULL;
    int i;
    static soc_reg_t timer_count[2] = {
        Q_SCHED_DD_TIMER_Ar, Q_SCHED_DD_TIMER_Br
    };
    static soc_reg_t timer_en_regs[2] = {
        Q_SCHED_DD_TIMER_ENABLE_Ar, Q_SCHED_DD_TIMER_ENABLE_Br
    };
    static soc_reg_t timer_status_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_Ar, Q_SCHED_DD_TIMER_STATUS_Br
    };
    static soc_reg_t timer_mask_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_A_MASKr, Q_SCHED_DD_TIMER_STATUS_B_MASKr
    };
    static soc_reg_t config_regs[2] = {
        Q_SCHED_DD_CHIP_CONFIG1r, Q_SCHED_DD_CHIP_CONFIG2r
    };
    static soc_field_t deadlock_cos_field[2] = {
        DD_COS_Af, DD_COS_Bf
    };
    static soc_field_t deadlock_time_field[2] = {
        DD_TIMER_INIT_VALUE_COS_Af, DD_TIMER_INIT_VALUE_COS_Bf
    };

    pfc_deadlock_control = _BCM_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    pfc_deadlock_control->pfc_deadlock_cos_max = 2;
    pfc_deadlock_control->cosq_inv_mapping_get = _bcm_th_cosq_inv_mapping_get;

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        hw_res->timer_count[i] = timer_count[i];
        hw_res->timer_status[i] = timer_status_regs[i];
        hw_res->timer_mask[i] = timer_mask_regs[i];
        hw_res->timer_en[i] = timer_en_regs[i];
        hw_res->time_init_val[i] = deadlock_time_field[i];
        hw_res->cos_field[i] = deadlock_cos_field[i];
    }
    hw_res->port_config = Q_SCHED_DD_PORT_CONFIGr;
    hw_res->chip_config[0] = config_regs[0];
    hw_res->chip_config[1] = config_regs[1];
    hw_res->time_unit_field = DD_TIMER_TICK_UNITf;
    hw_res->recovery_action_field = IGNORE_PFC_OFF_PKT_DISCARDf;
    hw_res->deadlock_status_field = DEADLOCK_DETECTEDf;
    return BCM_E_NONE;
}
#endif /* BCM_TOMAHAWK_SUPPORT */
