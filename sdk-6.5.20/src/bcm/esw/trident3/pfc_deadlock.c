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
#include <soc/trident3.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident3.h>

#if defined(BCM_TRIDENT3_SUPPORT)

#define BCM_TD3_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS 4

_bcm_td3_pfc_deadlock_control_t *_bcm_td3_pfc_deadlock_control[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _bcm_td3_pfc_lock[SOC_MAX_NUM_DEVICES] = {NULL};
#define PFC_LOCK(unit) sal_mutex_take(_bcm_td3_pfc_lock[unit], sal_mutex_FOREVER)
#define PFC_UNLOCK(unit) sal_mutex_give(_bcm_td3_pfc_lock[unit])

int
_bcm_td3_pfc_deadlock_ignore_pfc_xoff_clear(int unit, int cos, bcm_port_t port)
{
    int priority = 0;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }

    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    /* For that port, set ignore_pfc_xoff = 0 (per port reg) */
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config, port, 0, &rval));
    rval &= ~(1 << priority);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config, port, 0, rval));

    return BCM_E_NONE;
}

int
_bcm_td3_pfc_deadlock_hw_cos_index_get(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control->hw_cos_idx_inuse[priority] == TRUE) {
        *hw_cos_index = priority;
        return BCM_E_NONE;
    }
    return BCM_E_NOT_FOUND;
}

int
_bcm_td3_pfc_deadlock_hw_cos_index_set(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    int rv = BCM_E_NONE;
    int temp_hw_index = -1;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_config = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);

    rv = _bcm_td3_pfc_deadlock_hw_cos_index_get(unit, priority, &temp_hw_index);
    if (rv != BCM_E_NONE) {
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    if (temp_hw_index != -1) {
        /* Config for this priority exists already. Donot reprogram */
        *hw_cos_index = temp_hw_index;
        return BCM_E_NONE;
    }

    /* New priority */
    pfc_deadlock_control->hw_cos_idx_inuse[priority] = TRUE;
    pfc_deadlock_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);
    pfc_deadlock_config->priority = priority;
    pfc_deadlock_config->flags |= _BCM_PFC_DEADLOCK_F_ENABLE;
    pfc_deadlock_control->pfc_deadlock_cos_used++;
    *hw_cos_index = priority;
    return BCM_E_NONE;
}

/* Routine to Reset the recovery state of a port and config the port back in
 * original state
 */
int
 _bcm_td3_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port)
{
    int pipe, split, pos, priority = 0;
    uint64 rval64, temp_mask64, temp_en64;
    uint32 rval, uc_cos_bmp;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }

    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));

    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);

    COMPILER_64_ZERO(temp_en64);
    COMPILER_64_ZERO(temp_mask64);

    if (pos < 32) {
        COMPILER_64_SET(temp_mask64, 0, (1 << pos));
    } else {
        COMPILER_64_SET(temp_mask64, (1 << (pos - 32)), 0);
    }

    COMPILER_64_OR(temp_en64, temp_mask64);

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "PFC Deadlock Recovery ends: Prio %d port=%d\n"),
                    priority, port));

    /* For that port, set ignore_pfc_xoff = 0 (per port reg) */
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config, port, 0, &rval));

    rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(unit, priority, port,
                                                 &uc_cos_bmp);

    if(rval != BCM_E_NONE) {
        rval = 0;
        rval &= ~(1 << priority);
    } else {
        rval = 0;
        rval &= ~uc_cos_bmp;
    }

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config, port, 0, rval));

    /* Mask the Intr DD_TIMER_MASK by setting 0 (pbmp) */
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_get(unit, hw_res->timer_mask[split], -1, pipe,
                                 cos, &rval64));
    COMPILER_64_NOT(temp_mask64);
    COMPILER_64_AND(rval64, temp_mask64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_set(unit, hw_res->timer_mask[split], -1, pipe,
                                 cos, rval64));

    /* Turn timer off DD_TIMER_ENABLE  by setting 1 (pbmp) */
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_get(unit, hw_res->timer_en[split], -1, pipe,
                                 cos, &rval64));
    COMPILER_64_OR(rval64, temp_en64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_set(unit, hw_res->timer_en[split], -1, pipe,
                                 cos, rval64));

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if(pfc_deadlock_cb->pfc_deadlock_cb) {
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port, priority,
                                bcmCosqPfcDeadlockRecoveryEventEnd,
                                pfc_deadlock_cb->pfc_deadlock_userdata);
    }

    SOC_PBMP_PORT_REMOVE(pfc_deadlock_pri_config->deadlock_ports, port);

    return BCM_E_NONE;
}

/* Routine to begin recovery when a Port is deaclared to be in Deadlock by
 * Hardware.
 */
STATIC int
_bcm_td3_pfc_deadlock_recovery_begin(int unit, int cos, bcm_port_t port)
{
    uint32 rval,uc_cos_bmp = 0, user_rval = 0;
    int pipe, split, pos, priority = 0;
    uint64 rval64, temp_mask64, temp_en64;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;
    SOC_IF_ERROR_RETURN(
        soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));

    COMPILER_64_ZERO(temp_en64);
    COMPILER_64_ZERO(temp_mask64);

    if (pos < 32) {
        COMPILER_64_SET(temp_mask64, 0, (1 << pos));
    } else {
        COMPILER_64_SET(temp_mask64, (1 << (pos - 32)), 0);
    }

    COMPILER_64_OR(temp_en64, temp_mask64);

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "PFC Deadlock Detected: Cos %d port=%d\n"), cos, port));

    /* Mask the Intr DD_TIMER_MASK by setting 1 (pbmp) */
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_get(unit, hw_res->timer_mask[split], -1, pipe,
                                 cos, &rval64));
    COMPILER_64_OR(rval64, temp_mask64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_set(unit, hw_res->timer_mask[split], -1, pipe,
                                 cos, rval64));

    /* Turn timer off DD_TIMER_ENABLE  by setting 0 (pbmp) */
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_get(unit, hw_res->timer_en[split], -1, pipe,
                                 cos, &rval64));
    COMPILER_64_NOT(temp_en64);
    COMPILER_64_AND(rval64, temp_en64);
    SOC_IF_ERROR_RETURN(
        soc_trident3_xpe_reg_set(unit, hw_res->timer_en[split], -1, pipe,
                                 cos, rval64));

    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config, port, 0, &rval));
    priority = pfc_deadlock_control->pfc_cos2pri[cos];

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) &&
        pfc_deadlock_cb->pfc_deadlock_cb) {
        user_rval = pfc_deadlock_cb->pfc_deadlock_cb(unit, port,
                                priority, bcmCosqPfcDeadlockRecoveryEventBegin,
                                pfc_deadlock_cb->pfc_deadlock_userdata);
        if (user_rval) {
            rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(
                        unit, priority, port, &uc_cos_bmp);
            if (rval != BCM_E_NONE) {
                rval = 0;
                rval |= (1 << priority);
            } else {
                rval = 0;
                rval |= uc_cos_bmp;
            }

            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, hw_res->port_config, port, 0, rval));
        }
        return BCM_E_NONE;
    }

    rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(unit, priority, port,
                                                 &uc_cos_bmp);

    if(rval != BCM_E_NONE) {
        rval = 0;
        rval |= (1 << priority);
    } else {
        rval = 0;
        rval |= uc_cos_bmp;
    }

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config, port, 0, rval));

    if(pfc_deadlock_cb->pfc_deadlock_cb) {
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port,
                                priority, bcmCosqPfcDeadlockRecoveryEventBegin,
                                pfc_deadlock_cb->pfc_deadlock_userdata);
    }

    pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);

    /* Recovery count calculated from Configured Recovery timer.
     * Note: recovery_timer is in msecs and cb_interval is in usecs.
     */
    pfc_deadlock_pri_config->port_recovery_count[port] = 1 +
                        ((pfc_deadlock_pri_config->recovery_timer * 1000) /
                        _BCM_TD3_PFC_DEADLOCK_CB_INTERVAL(0));
    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);

    return BCM_E_NONE;
}

/* Routine to update the recovery status of ports which are in Recovery state
 */
int
_bcm_td3_pfc_deadlock_recovery_update(int unit, int cos)
{
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    bcm_port_t port;
    int priority = 0;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);

    BCM_PBMP_ITER(pfc_deadlock_pri_config->deadlock_ports, port) {
        if (port >= MAX_PORT(unit)) {
            break; /* Process for valid ports */
        }
        /*
         * COVERITY
         *
         * Max Port num will be updated dynamically per device.
         */
        /* coverity[overrun-local : FALSE] */
        if (pfc_deadlock_pri_config->port_recovery_count[port]) {
            pfc_deadlock_pri_config->port_recovery_count[port]--;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_td3_pfc_deadlock_recovery_end(unit, cos, port));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td3_pfc_deadlock_monitor
 * Purpose:
 *     1) Monitor the hardware for Deadlock status
 *     2) Start Recovery process for the Deadlocked port/queue
 *     3) Reset Port back to original state on expiry of recovery phase
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Step 1 - Monitor (Deadlock Detection phase):
 *          Sw polls for Deadlock detection intr set for port
 *          Q_SCHED_DD_TIMER_STATUS_SPLIT0|1 (pbmp)
 *
 *     Step 2: Recovery Begin phase
 *          2.a: Mask the Intr Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT0|1 by setting 1 (pbmp)
 *          2.b: Turn timer off Q_SCHED_DD_TIMER_ENABLE_SPLITE0|1 by setting 0 (pbmp)
 *          2.c: For that port, set ignore_pfc_xoff = 1 (per port reg)
 *          2.4: Start recovery timer
 *      Step 3: Recovery End phase (On expiry of recovery timer)
 *          3.a: Reset ignore_xoff; set ignore_pfc_xoff = 0
 *          3.b: UnMask the Intr Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT0|1 by setting 0 (pbmp)
 *          3.c: Turn timer off Q_SCHED_DD_TIMER_ENABLE_SPLITE0|1 by setting 1 (pbmp)
 */
int
_bcm_td3_pfc_deadlock_monitor(int unit)
{
    int rv = 0;
    int cos = 0, priority = 0, user_control = 0;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    uint64 status64, mask64;
    int pipe, split, pos;
    bcm_port_t port;

    COMPILER_64_ZERO(status64);
    COMPILER_64_ZERO(mask64);
    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) &&
        pfc_deadlock_cb->pfc_deadlock_cb) {
        user_control = 1;
    }

    PFC_LOCK(unit);
    for (cos = 0; cos < pfc_deadlock_control->pfc_deadlock_cos_max; cos++) {
        /* Check if index is in Use */
        if (pfc_deadlock_control->hw_cos_idx_inuse[cos] == TRUE) {
            /* Check Hardware if New ports have been declared to be in
             * Deadlock condition
             */
            PBMP_ALL_ITER(unit, port) {
                /* PFC deadlock feature not supported on CPU and Loopback ports */
                if ((IS_CPU_PORT(unit, port)) || (IS_LB_PORT(unit, port))) {
                    continue;
                }
                rv = soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos);
                if (BCM_FAILURE(rv)) {
                    PFC_UNLOCK(unit);
                    return rv;
                }
                rv = soc_trident3_xpe_reg_get(unit, hw_res->timer_status[split], -1,
                                             pipe, cos, &status64);
                if (BCM_FAILURE(rv)) {
                    PFC_UNLOCK(unit);
                    return rv;
                }
                rv = soc_trident3_xpe_reg_get(unit, hw_res->timer_mask[split], -1,
                                             pipe, cos, &mask64);
                if (BCM_FAILURE(rv)) {
                    PFC_UNLOCK(unit);
                    return rv;
                }
                /* Mask - bit 0 indicates enabled for reporting */
                COMPILER_64_NOT(mask64);
                COMPILER_64_AND(status64, mask64);

                if (COMPILER_64_IS_ZERO(status64) == 0) {
                    /* New ports declared to be in Deadlock status */
                    if (pos < 32) {
                        if ((COMPILER_64_LO(status64) &
                            (1 << pos)) == FALSE) {
                            continue;
                        }
                    } else {
                        if ((COMPILER_64_HI(status64) &
                            (1 << (pos - 32))) == FALSE) {
                            continue;
                        }
                    }
                    rv = _bcm_td3_pfc_deadlock_recovery_begin(unit, cos, port);
                    if (BCM_FAILURE(rv)) {
                        PFC_UNLOCK(unit);
                        return rv;
                    }
                }
            }

            if (!user_control) {
                /* Update the count for ports already in Deadlock state and Reset
                * those ports where recovery timer has expired.
                */
                priority = pfc_deadlock_control->pfc_cos2pri[cos];
                pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);
                /* Updates required for Enabled COS */
                if (SOC_PBMP_NOT_NULL(pfc_deadlock_pri_config->deadlock_ports)) {
                    rv = _bcm_td3_pfc_deadlock_recovery_update(unit, cos);
                    if (BCM_FAILURE(rv)) {
                        PFC_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        }
    }

    PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

/* TD3 Q_SCHED_DD_CHIP_CONFIG CONTENTS
 * Q_SCHED_DD_CHIP_CONFIG0:DD_TIMER_INIT_VALUE_COS_0~DD_TIMER_INIT_VALUE_COS_4
 * Q_SCHED_DD_CHIP_CONFIG1:DD_TIMER_INIT_VALUE_COS_5~DD_TIMER_INIT_VALUE_COS_9
 * Q_SCHED_DD_CHIP_CONFIG2: ignore_pfc_off_pkt_discard
 */
int
_bcm_td3_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config)
{
    if (priority <= BCM_TD3_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG0r;
    } else {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG1r;
    }
    return BCM_E_NONE;
}

/* Routine to perform the Set/Get operation for the Priority */
int
_bcm_td3_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config)
{
    int rv = 0;
    int hw_cos_index = -1;
    uint32 rval = 0;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    uint32 detection_granularity = 0;
    soc_reg_t   chip_config;
    soc_field_t cos_init_timer_field;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;
    detection_granularity = (_BCM_TD3_PFC_DEADLOCK_TIME_UNIT(unit) ==
        bcmSwitchPFCDeadlockDetectionInterval10MiliSecond) ? 10 : 100;

    BCM_IF_ERROR_RETURN(
        _bcm_td3_pfc_deadlock_chip_config_get(unit, priority, &chip_config));

    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                      REG_PORT_ANY, 0, &rval));

    PFC_LOCK(unit);
    if (operation == _bcmPfcDeadlockOperGet) {

        config->detection_timer = soc_reg_field_get(unit,
                                        chip_config, rval,
                                        hw_res->time_init_val[priority]);
        config->detection_timer *= detection_granularity;
    } else { /* _bcmPfcDeadlockOperSet */
        rv = _bcm_td3_pfc_deadlock_hw_cos_index_set(unit, priority, &hw_cos_index);
        if (BCM_FAILURE(rv)) {
            PFC_UNLOCK(unit);
            return rv;
        }
        if (hw_cos_index == -1) {
            /* No matching or free hw_index available for use */
            PFC_UNLOCK(unit);
            return BCM_E_RESOURCE;
        }

        cos_init_timer_field = hw_res->time_init_val[hw_cos_index];
        soc_reg_field_set(unit, chip_config, &rval,
                          cos_init_timer_field,
                          (config->detection_timer / detection_granularity));
        rv = soc_reg32_set(unit, chip_config, REG_PORT_ANY, 0, rval);
        if (BCM_FAILURE(rv)) {
            PFC_UNLOCK(unit);
            return rv;
        }

        config->priority = priority;
        pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = priority;
        pfc_deadlock_control->pfc_pri2cos[priority] = hw_cos_index;
    }

    PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

int
_bcm_td3_pfc_deadlock_q_config_helper(int unit,
                                     _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status)
{
    int rv = BCM_E_NONE;
    uint32 temp_val_lo = 0, temp_val_hi = 0;
    uint64 rval64;
    bcm_port_t local_port;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    int pipe, split, pos;
    int priority, hw_cos_index = -1;

    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* Invalid Gport */
        return BCM_E_PARAM;
    }
    hw_res = &pfc_deadlock_control->hw_regs_fields;
    if (pfc_deadlock_control->cosq_inv_mapping_get) {
        rv = pfc_deadlock_control->cosq_inv_mapping_get(unit, gport, -1,
                                          BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                          &local_port, &priority);
    } else {
        /* Mapping function not defined */
        return BCM_E_INIT;
    }
    if (rv != BCM_E_NONE) {
        if (rv == BCM_E_NOT_FOUND) {
            /* Given GPort/Queue has No Input priority mapping */
            return BCM_E_RESOURCE;
        }
        return rv;
    }

    PFC_LOCK(unit);
    pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);

    rv = _bcm_td3_pfc_deadlock_hw_cos_index_get(unit, priority, &hw_cos_index);
    if (BCM_FAILURE(rv)) {
        PFC_UNLOCK(unit);
        return rv;
    }
    if (hw_cos_index == -1) {
        /* No matching or free hw_index available for use */
        PFC_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    rv = soc_td3_mmu_bmp_reg_pos_get(unit, local_port, &pipe, &split, &pos);
    if (BCM_FAILURE(rv)) {
        PFC_UNLOCK(unit);
        return rv;
    }

    COMPILER_64_ZERO(rval64);
    rv = soc_trident3_xpe_reg_get(unit, hw_res->timer_en[split], -1, pipe,
                                 hw_cos_index, &rval64);
    if (BCM_FAILURE(rv)) {
        PFC_UNLOCK(unit);
        return rv;
    }

    if (operation == _bcmPfcDeadlockOperGet) { /* GET */
        if (enable_status) {
            *enable_status =
                BCM_PBMP_MEMBER(pfc_deadlock_pri_config->deadlock_ports,
                            local_port);
        }
        if (config) {
            if (pos < 32) {
                config->enable =
                    (COMPILER_64_LO(rval64) & (1 << pos)) ? TRUE : FALSE;
            } else {
                config->enable =
                    (COMPILER_64_HI(rval64) & (1 << (pos - 32))) ?
                                 TRUE : FALSE;
            }
        }

        PFC_UNLOCK(unit);
        return BCM_E_NONE;
    } else { /* _bcmPfcDeadlockOperSet */
        temp_val_lo = COMPILER_64_LO(rval64);
        temp_val_hi = COMPILER_64_HI(rval64);

        if (config->enable) {
            if (pos < 32) {
                temp_val_lo |= (1 << pos);
            } else {
                temp_val_hi |= (1 << (pos - 32));
            }
            BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports,
                              local_port);
        } else {
            if (pos < 32) {
                temp_val_lo &= ~(1 << pos);
            } else {
                temp_val_hi &= ~(1 << (pos - 32));
            }

            SOC_PBMP_PORT_REMOVE(pfc_deadlock_pri_config->enabled_ports,
                                 local_port);
        }

        COMPILER_64_SET(rval64, temp_val_hi, temp_val_lo);
        rv = soc_trident3_xpe_reg_set(unit, hw_res->timer_en[split], -1, pipe,
                                     hw_cos_index, rval64);
        if (BCM_FAILURE(rv)) {
            PFC_UNLOCK(unit);
            return rv;
        }

        if (!config->enable &&
            SOC_PBMP_MEMBER(pfc_deadlock_pri_config->deadlock_ports, local_port)) {
            rv = _bcm_td3_pfc_deadlock_recovery_end(unit, hw_cos_index, local_port);
            if (BCM_FAILURE(rv)) {
                PFC_UNLOCK(unit);
                return rv;
            }
        }
        if (SOC_PBMP_IS_NULL(pfc_deadlock_pri_config->enabled_ports)) {
            /*First for that port, set ignore_pfc_xoff = 0 (per port reg) */
            rv = _bcm_td3_pfc_deadlock_ignore_pfc_xoff_clear(
                unit, hw_cos_index, local_port);
            if (BCM_FAILURE(rv)) {
                PFC_UNLOCK(unit);
                return rv;
            }
            pfc_deadlock_control->hw_cos_idx_inuse[hw_cos_index] = FALSE;
            pfc_deadlock_pri_config->flags &= ~_BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = -1;
            pfc_deadlock_control->pfc_pri2cos[priority] = -1;
        }
    }

    PFC_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(_bcm_td3_pfc_deadlock_update_cos_used(unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_pfc_deadlock_recovery_reset
 * Purpose:
 *     Recover the system back to default state.
 *     Restore all ports from Deadlock/Recovery state, if Warm boot when
 *     recovery was in progress
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td3_pfc_deadlock_recovery_reset(int unit)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    int priority = 0, i;
    bcm_port_t port;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        priority = pfc_deadlock_control->pfc_cos2pri[i];
        /* Skipping invalid priorities */
        if ((priority < 0) || (priority >= _TD3_MMU_NUM_INT_PRI)) {
            continue;
        }
        pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);

        /*
         * COVERITY: Max Port will take care of Port beyond supported
         * range
         */
        /* coverity[overrun-local : FALSE] */
        BCM_PBMP_ITER(pfc_deadlock_pri_config->deadlock_ports, port) {
            if (port >= MAX_PORT(unit)) {
                break; /* Process for valid ports */
            }
            /*
             * COVERITY: Max Port num will be updated dynamically per device.
             */
            /* coverity[overrun-local : FALSE] */
            pfc_deadlock_pri_config->port_recovery_count[port] = 0;
            BCM_IF_ERROR_RETURN(
                _bcm_td3_pfc_deadlock_recovery_end(unit, i, port));
        }
    }
    pfc_deadlock_control->cb_enabled = FALSE;
    BCM_IF_ERROR_RETURN(_bcm_td3_pfc_deadlock_update_cos_used(unit));
    pfc_deadlock_control->cosq_inv_mapping_get = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_pfc_deadlock_control_set
 * Description:
 *      Set PFC Deadlock feature's switch config.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to set.
 *      arg  - Pointer to set value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      bcmSwitchPFCDeadlockCosMax - Set operation is not permitted
 */
int
_bcm_td3_pfc_deadlock_control_set(int unit, bcm_switch_control_t type, int arg)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval = 0, field_val = 0;
    soc_reg_t chip_config;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    if (pfc_deadlock_control == NULL) {
        return BCM_E_UNAVAIL;
    }
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    switch (type) {
        case bcmSwitchPFCDeadlockDetectionTimeInterval:
            if ((arg < 0) ||
                (arg >= bcmSwitchPFCDeadlockDetectionIntervalCount)) {
                return BCM_E_PARAM;
            }
            chip_config = hw_res->chip_config[1];

            rval = 0;
            if (arg == bcmSwitchPFCDeadlockDetectionInterval10MiliSecond) {
                field_val = 0;
            } else {
                field_val = 1; /* Default Unit - Order of 100ms */
            }

            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }

            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, chip_config, &rval,
                              hw_res->time_unit_field, field_val);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, chip_config,
                                REG_PORT_ANY, 0, rval));
            pfc_deadlock_control->time_unit = arg;
            break;
        case bcmSwitchPFCDeadlockRecoveryAction:
            if ((arg < 0) ||
                (arg >= bcmSwitchPFCDeadlockActionMaxCount)) {
                return BCM_E_PARAM;
            }

            chip_config = hw_res->chip_config[2];
            rval = 0;
            if (arg == bcmSwitchPFCDeadlockActionDrop) {
                field_val = 1;
            } else {
                field_val = 0; /* Default action - Transmit */
            }

            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }

            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, chip_config, &rval,
                              hw_res->recovery_action_field, field_val);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, chip_config,
                                REG_PORT_ANY, 0, rval));
            pfc_deadlock_control->recovery_action = arg;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_pfc_deadlock_control_get
 * Description:
 *      Get PFC Deadlock feature's switch config.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg  - Pointer to retrieved value
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_td3_pfc_deadlock_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    if (pfc_deadlock_control == NULL) {
        return BCM_E_UNAVAIL;
    }
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    switch (type) {
        case bcmSwitchPFCDeadlockDetectionTimeInterval:
            if (hw_res->chip_config[1] == INVALIDr) {
                return BCM_E_UNAVAIL;
            }
            *arg = pfc_deadlock_control->time_unit;
            break;
        case bcmSwitchPFCDeadlockRecoveryAction:
            if (hw_res->chip_config[2] == INVALIDr) {
                return BCM_E_UNAVAIL;
            }
            *arg = pfc_deadlock_control->recovery_action;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pfc_deadlock_control_get_hw
 * Description:
 *      Retrieve PFC Deadlock feature's switch config.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg  - Pointer to retrieved value
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_td3_pfc_deadlock_control_get_hw(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval = 0, field_val = 0;
    soc_reg_t chip_config;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    if (pfc_deadlock_control == NULL) {
        return BCM_E_UNAVAIL;
    }
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    switch (type) {
        case bcmSwitchPFCDeadlockDetectionTimeInterval:
            chip_config = hw_res->chip_config[1];
            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }
            rval = 0;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config, REG_PORT_ANY, 0,
                                              &rval));
            field_val = soc_reg_field_get(unit, chip_config,
                                          rval, hw_res->time_unit_field);
            if (field_val == 0) {
                *arg = bcmSwitchPFCDeadlockDetectionInterval10MiliSecond;
            } else {
                *arg = bcmSwitchPFCDeadlockDetectionInterval100MiliSecond;
            }
            break;
        case bcmSwitchPFCDeadlockRecoveryAction:
            chip_config = hw_res->chip_config[2];
            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }
            rval = 0;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                REG_PORT_ANY, 0, &rval));
            field_val = soc_reg_field_get(unit, chip_config, rval,
                                          hw_res->recovery_action_field);

            if (field_val == 1) {
                *arg = bcmSwitchPFCDeadlockActionDrop;
            } else {
                *arg = bcmSwitchPFCDeadlockActionTransmit;
            }
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/* Set Chip level defautl values */
STATIC int
_bcm_td3_pfc_deadlock_default(int unit)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    int i = 0;

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);

    _BCM_TD3_PFC_DEADLOCK_HW_RES_INIT(&pfc_deadlock_control->hw_regs_fields);
    pfc_deadlock_control->pfc_deadlock_cos_max = 0;
    pfc_deadlock_control->pfc_deadlock_cos_used = 0;

    for (i = 0; i < COUNTOF(pfc_deadlock_control->hw_cos_idx_inuse); i++) {
        pfc_deadlock_control->hw_cos_idx_inuse[i] = FALSE;
    }
    for (i = 0; i < TD3_PFC_DEADLOCK_MAX_COS; i++) {
        pfc_deadlock_control->pfc_cos2pri[i] = -1;
        pfc_deadlock_control->pfc_pri2cos[i] = -1;
    }
    sal_memset(pfc_deadlock_control->pfc_cos2pri, -1,
            (TD3_PFC_DEADLOCK_MAX_COS * sizeof(bcm_cos_t)));
    sal_memset(pfc_deadlock_control->pfc_pri2cos, -1,
            (TD3_PFC_DEADLOCK_MAX_COS * sizeof(bcm_cos_t)));
    pfc_deadlock_control->cb_enabled = FALSE;
    pfc_deadlock_control->time_unit = 0; /* 100 ms */
    pfc_deadlock_control->cb_interval = 100 * 1000; /* 100 ms */
    return BCM_E_NONE;
}

/* HW res init */
int
_bcm_td3_pfc_deadlock_hw_res_init(int unit)
{
     int i = 0;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    static soc_reg_t timer_count[1] = {
        Q_SCHED_DD_TIMERr
    };
    static soc_reg_t timer_en_regs[2] = {
        Q_SCHED_DD_TIMER_ENABLE_SPLIT0r, Q_SCHED_DD_TIMER_ENABLE_SPLIT1r
    };
    static soc_reg_t timer_status_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_SPLIT0r, Q_SCHED_DD_TIMER_STATUS_SPLIT1r
    };
    static soc_reg_t timer_mask_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT0r, Q_SCHED_DD_TIMER_STATUS_MASK_SPLIT1r
    };
    static soc_reg_t config_regs[3] = {
        Q_SCHED_DD_CHIP_CONFIG0r, Q_SCHED_DD_CHIP_CONFIG1r, Q_SCHED_DD_CHIP_CONFIG2r
    };

    static soc_field_t deadlock_time_field[10] = {
        DD_TIMER_INIT_VALUE_COS_0f, DD_TIMER_INIT_VALUE_COS_1f,
        DD_TIMER_INIT_VALUE_COS_2f, DD_TIMER_INIT_VALUE_COS_3f,
        DD_TIMER_INIT_VALUE_COS_4f, DD_TIMER_INIT_VALUE_COS_5f,
        DD_TIMER_INIT_VALUE_COS_6f, DD_TIMER_INIT_VALUE_COS_7f,
        DD_TIMER_INIT_VALUE_COS_8f, DD_TIMER_INIT_VALUE_COS_9f
    };

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    hw_res->timer_count[0]  = timer_count[0];
    hw_res->timer_status[0] = timer_status_regs[0];
    hw_res->timer_status[1] = timer_status_regs[1];
    hw_res->timer_mask[0]   = timer_mask_regs[0];
    hw_res->timer_mask[1]   = timer_mask_regs[1];
    hw_res->timer_en[0]     = timer_en_regs[0];
    hw_res->timer_en[1]    = timer_en_regs[1];

    for (i = 0; i < TD3_PFC_DEADLOCK_MAX_COS; i++) {
        hw_res->time_init_val[i] = deadlock_time_field[i];
    }
    hw_res->port_config = Q_SCHED_DD_PORT_CONFIGr;
    hw_res->chip_config[0] = config_regs[0];
    hw_res->chip_config[1] = config_regs[1];
    hw_res->chip_config[2] = config_regs[2];
    /* Q_SCHED_DD_CHIP_CONFIG1 */
    hw_res->time_unit_field = DD_TIMER_TICK_UNITf;
    /* Q_SCHED_DD_CHIP_CONFIG2 */
    hw_res->recovery_action_field = IGNORE_PFC_OFF_PKT_DISCARDf;
    /* Q_SCHED_DD_TIMER_STATUS_SPLIT0/1 */
    hw_res->deadlock_status_field = DEADLOCK_DETECTEDf;
    return BCM_E_NONE;
}

/* Chip specific init and assign default values */
int
_bcm_td3_pfc_deadlock_init(int unit)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    BCM_IF_ERROR_RETURN(_bcm_td3_pfc_deadlock_default(unit));
    BCM_IF_ERROR_RETURN(_bcm_td3_pfc_deadlock_hw_res_init(unit));

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    pfc_deadlock_control->pfc_deadlock_cos_max = TD3_PFC_DEADLOCK_MAX_COS;
    pfc_deadlock_control->cosq_inv_mapping_get = _bcm_td3_cosq_inv_mapping_get;

    _bcm_td3_pfc_lock[unit] = sal_mutex_create("pfc lock");
    if (_bcm_td3_pfc_lock[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_pfc_deadlock_deinit
 * Purpose:
 *     De-Initialize allocated memory for PFC Deadlock feature
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td3_pfc_deadlock_deinit(int unit)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    (void)_bcm_pfc_deadlock_deinit(unit);

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control != NULL) {
        sal_free(pfc_deadlock_control);
        _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit) = NULL;
    }

    if (_bcm_td3_pfc_lock[unit]) {
        sal_mutex_destroy(_bcm_td3_pfc_lock[unit]);
        _bcm_td3_pfc_lock[unit] = NULL;
    }


    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *     _bcm_pfc_deadlock_reinit
 * Purpose:
 *     Reinitialize all states/data-structures from HW(Level 1 warmboot)
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td3_pfc_deadlock_reinit(int unit)
{
    _bcm_td3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_td3_pfc_hw_resorces_t *hw_res = NULL;
    int priority = 0, i, split, rv, temp_hw_index = -1, arg_value;
    int port, pipe, pos;
    uint64 en_rval64[2];
    uint64 mask_rval64[2];

    pfc_deadlock_control = _BCM_TD3_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    BCM_IF_ERROR_RETURN
        (_bcm_td3_pfc_deadlock_control_get_hw(unit,
                                    bcmSwitchPFCDeadlockDetectionTimeInterval,
                                    &arg_value));
    pfc_deadlock_control->time_unit = arg_value;
    BCM_IF_ERROR_RETURN
        (_bcm_td3_pfc_deadlock_control_get_hw(unit,
                                    bcmSwitchPFCDeadlockRecoveryAction,
                                    &arg_value));
    pfc_deadlock_control->recovery_action = arg_value;

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        priority = pfc_deadlock_control->pfc_cos2pri[i];
        /* Skipping invalid priorities */
        if ((priority < 0) || (priority >= PFC_DEADLOCK_MAX_COS)) {
            continue;
        }
        pfc_deadlock_pri_config = _BCM_TD3_PFC_DEADLOCK_CONFIG(unit, priority);

        /* coverity[overrun-call] */
        rv = _bcm_td3_pfc_deadlock_hw_cos_index_get(unit, priority,
                                                    &temp_hw_index);
        if (BCM_SUCCESS(rv)) {
            pfc_deadlock_pri_config->flags   |= _BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_pri_config->priority = priority;
            /* coverity[overrun-call] */
            BCM_IF_ERROR_RETURN(
                _bcm_td3_pfc_deadlock_hw_oper(unit, _bcmPfcDeadlockOperGet,
                                          priority, pfc_deadlock_pri_config));

            for (split = 0; split < 2; split++) {
                COMPILER_64_ZERO(en_rval64[split]);
                COMPILER_64_ZERO(mask_rval64[split]);

                SOC_IF_ERROR_RETURN(
                    soc_trident3_xpe_reg_get(unit, hw_res->timer_en[split], -1,
                                             0, i, &en_rval64[split]));
                SOC_IF_ERROR_RETURN(
                    soc_trident3_xpe_reg_get(unit, hw_res->timer_mask[split], -1,
                                0, i, &mask_rval64[split]));
            }

            PBMP_ALL_ITER(unit, port) {
                 SOC_IF_ERROR_RETURN(
                     soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split,
                                                 &pos));
                 if (pos < 32) {
                     if (COMPILER_64_LO(en_rval64[pipe]) & (1 << pos)) {
                         BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                     } else if (COMPILER_64_LO(mask_rval64[pipe]) & (1 << pos)) {
                         BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                         BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);
                     }
                 } else {
                     if (COMPILER_64_HI(en_rval64[pipe]) & (1 << (pos - 32))) {
                         BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                     } else if (COMPILER_64_LO(mask_rval64[pipe]) & (1 << (pos - 32))) {
                         BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                         BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);
                     }
                 }
            }
        }
    }

    return BCM_E_NONE;
}
#endif
#endif /* BCM_TRIDENT3_SUPPORT */
