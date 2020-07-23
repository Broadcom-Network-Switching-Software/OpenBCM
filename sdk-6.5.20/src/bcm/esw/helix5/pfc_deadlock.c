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
#include <soc/helix5.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/helix5.h>

#if defined(BCM_HELIX5_SUPPORT)

#define BCM_HX5_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS 4
#define BCM_HX5_PFC_DEADLOCK_CHIP_CONFIG_1_MAX_COS 9


_bcm_hx5_pfc_deadlock_control_t *_bcm_hx5_pfc_deadlock_control[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _bcm_hx5_pfc_lock[SOC_MAX_NUM_DEVICES] = {NULL};
#define HX5_PFC_LOCK(unit) sal_mutex_take(_bcm_hx5_pfc_lock[unit], sal_mutex_FOREVER)
#define HX5_PFC_UNLOCK(unit) sal_mutex_give(_bcm_hx5_pfc_lock[unit])

int
_bcm_hx5_pfc_reg_index_get(int unit, bcm_port_t port, bcm_port_t *mport, int *index)
{
   soc_info_t *si;
   int phy_port, mmu_port;
   if( NULL == index) {
       return BCM_E_PARAM;
   }
   si = &SOC_INFO(unit);
   phy_port = si->port_l2p_mapping[port];
   mmu_port = si->port_p2m_mapping[phy_port];

   *index = (mmu_port > 15)  ? 0 : 1;

   if(NULL != mport) {
       *mport = mmu_port;
   }
   return BCM_E_NONE;
}
int
_bcm_hx5_pfc_deadlock_ignore_pfc_xoff_clear(int unit, int cos, bcm_port_t port)
{
    int priority = 0, index;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }

    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    /* For that port, set ignore_pfc_xoff = 0 (per port reg) */
    BCM_IF_ERROR_RETURN(
        _bcm_hx5_pfc_reg_index_get(unit, port, NULL, &index));

    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config[index], port, 0, &rval));
    rval &= ~(1 << priority);
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config[index], port, 0, rval));

    return BCM_E_NONE;
}

int
_bcm_hx5_pfc_deadlock_hw_cos_index_get(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control->hw_cos_idx_inuse[priority] == TRUE) {
        *hw_cos_index = priority;
        return BCM_E_NONE;
    }
    return BCM_E_NOT_FOUND;
}

int
_bcm_hx5_pfc_deadlock_hw_cos_index_set(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    int rv = BCM_E_NONE;
    int temp_hw_index = -1;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_config = NULL;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);

    rv = _bcm_hx5_pfc_deadlock_hw_cos_index_get(unit, priority, &temp_hw_index);
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
    pfc_deadlock_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);
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
 _bcm_hx5_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port)
{
    int priority = 0;
    uint32 rval, uc_cos_bmp;
    uint64 rval64, rval1;
    int temp;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    bcm_port_t mmu_port;
    int index;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }


    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "PFC Deadlock Recovery ends: Prio %d port=%d\n"),
                    priority, port));

    BCM_IF_ERROR_RETURN(
        _bcm_hx5_pfc_reg_index_get(unit, port, &mmu_port, &index));

    /* For that port, set ignore_pfc_xoff = 0 (per port reg) */
    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config[index], port, 0, &rval));

    rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(unit, priority, port,
                                                 &uc_cos_bmp);

    if(rval != BCM_E_NONE) {
        rval &= ~(1 << priority);
    } else {
        rval &= ~uc_cos_bmp;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config[index], port, 0, rval));

    /* Mask the Intr DD_TIMER_MASK by setting 0 (pbmp) */
    rval = 0;
    COMPILER_64_SET(rval64, 0, 0);
    COMPILER_64_SET(rval1, 0, 0);
    if ( hw_res->timer_mask[index] == Q_SCHED_DD_TIMER_STATUS_MASK_HIQr) {
        BCM_IF_ERROR_RETURN(
                soc_reg64_get(unit, hw_res->timer_mask[index], port, 0, &rval64));

        temp = ~(1 << priority);
        COMPILER_64_SET(rval1, 0, temp);
        COMPILER_64_AND(rval64, rval1);

        BCM_IF_ERROR_RETURN(
                soc_reg64_set(unit, hw_res->timer_mask[index], port, 0, rval64));
    } else {
        BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, hw_res->timer_mask[index], port, 0, &rval));

        rval &= ~(1 << priority);

        BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, hw_res->timer_mask[index], port, 0, rval));
    }
    /* Turn timer off DD_TIMER_ENABLE  by setting 1 (pbmp) */
    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->timer_en[index], port, 0, &rval));

    rval |= (1 << priority);

    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->timer_en[index], port, 0, rval));

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if(pfc_deadlock_cb->pfc_deadlock_cb) {
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port, priority,
                                bcmCosqPfcDeadlockRecoveryEventEnd,
                                pfc_deadlock_cb->pfc_deadlock_userdata);
    }

    BCM_PBMP_PORT_REMOVE(pfc_deadlock_pri_config->deadlock_ports, port);

    return BCM_E_NONE;
}

/* Routine to begin recovery when a Port is deaclared to be in Deadlock by
 * Hardware.
 */
STATIC int
_bcm_hx5_pfc_deadlock_recovery_begin(int unit, int cos, bcm_port_t port)
{
    uint32 rval,uc_cos_bmp = 0, user_rval = 0;
    int temp;
    uint64 rval64, rval1;
    int priority = 0;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    bcm_port_t mmu_port;
    int index;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "PFC Deadlock Detected: Cos %d port=%d\n"), cos, port));

    BCM_IF_ERROR_RETURN(
        _bcm_hx5_pfc_reg_index_get(unit, port, &mmu_port, &index));

    /* Mask the Intr DD_TIMER_MASK by setting 1 (pbmp) */
    rval = 0;
    COMPILER_64_SET(rval64, 0, 0);
    COMPILER_64_SET(rval1, 0, 0);
    if ( hw_res->timer_mask[index] == Q_SCHED_DD_TIMER_STATUS_MASK_HIQr) {
        BCM_IF_ERROR_RETURN(
                soc_reg64_get(unit, hw_res->timer_mask[index], port, 0, &rval64));

        temp = 1 << cos;
        COMPILER_64_SET(rval1, 0, temp);
        COMPILER_64_OR(rval64, rval1);

        BCM_IF_ERROR_RETURN(
                soc_reg64_set(unit, hw_res->timer_mask[index], port, 0, rval64));
    } else {
        BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, hw_res->timer_mask[index], port, 0, &rval));
        rval |= (1 << cos);

        BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, hw_res->timer_mask[index], port, 0, rval));
    }
    /* Turn timer off DD_TIMER_ENABLE  by setting 0 (pbmp) */
    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->timer_en[index], port, 0, &rval));
    rval &= ~(1 << cos);

    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->timer_en[index], port, 0, rval));

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, hw_res->port_config[index], port, 0, &rval));
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
                    soc_reg32_set(unit, hw_res->port_config[index], port, 0, rval));
        }
        return BCM_E_NONE;
    }    

    rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(unit, priority, port,
                                                 &uc_cos_bmp);

    if(rval != BCM_E_NONE) {
        rval |= (1 << priority);
    } else {
        rval |= uc_cos_bmp;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, hw_res->port_config[index], port, 0, rval));

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if(pfc_deadlock_cb->pfc_deadlock_cb) {
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port,
                                priority, bcmCosqPfcDeadlockRecoveryEventBegin,
                                pfc_deadlock_cb->pfc_deadlock_userdata);
    }

    pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);

    /* Recovery count calculated from Configured Recovery timer.
     * Note: recovery_timer is in msecs and cb_interval is in usecs.
     */
    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);
    pfc_deadlock_pri_config->port_recovery_count[port] = 1 +
        ((pfc_deadlock_pri_config->recovery_timer * 1000) /
         _BCM_HX5_PFC_DEADLOCK_CB_INTERVAL(0));

    return BCM_E_NONE;
}

/* Routine to update the recovery status of ports which are in Recovery state
 */
int
_bcm_hx5_pfc_deadlock_recovery_update(int unit, int cos)
{
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    bcm_port_t port;
    int priority = 0;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);

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
                    _bcm_hx5_pfc_deadlock_recovery_end(unit, cos, port));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_hx5_pfc_deadlock_monitor
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
 *          Q_SCHED_DD_TIMER_STATUS/HIQ (pbmp)
 *
 *     Step 2: Recovery Begin phase
 *          2.a: Mask the Intr Q_SCHED_DD_TIMER_STATUS_MASK/HIQ by setting 1 (pbmp)
 *          2.b: Turn timer off Q_SCHED_DD_TIMER_ENABLE/HIQ by setting 0 (pbmp)
 *          2.c: For that port, set ignore_pfc_xoff = 1 (per port reg)
 *          2.4: Start recovery timer
 *      Step 3: Recovery End phase (On expiry of recovery timer)
 *          3.a: Reset ignore_xoff; set ignore_pfc_xoff = 0
 *          3.b: UnMask the Intr Q_SCHED_DD_TIMER_STATUS_MASK/HIQ by setting 0 (pbmp)
 *          3.c: Turn timer off Q_SCHED_DD_TIMER_ENABLE/HIQ by setting 1 (pbmp)
 */
int
_bcm_hx5_pfc_deadlock_monitor(int unit)
{
    int cos = 0, priority = 0, user_control = 0, rv = 0;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    int index, mmu_port;
    uint32 status;
    bcm_port_t port;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) &&
            pfc_deadlock_cb->pfc_deadlock_cb) {
        user_control = 1;
    }


    HX5_PFC_LOCK(unit);
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
                rv = _bcm_hx5_pfc_reg_index_get(unit, port, &mmu_port, &index);
                if (BCM_FAILURE(rv)) {
                    HX5_PFC_UNLOCK(unit);
                    return rv;
                }
                /* Downlink ports will have only 16 queues */
                if( mmu_port > 15 && cos > 15) {
                    continue;
                }
                status = 0;
                rv = soc_reg32_get(unit, hw_res->timer_status[index], port, 0, &status);
                if (BCM_FAILURE(rv)) {
                    HX5_PFC_UNLOCK(unit);
                    return rv;
                }

                if (status &(1 << cos)) {
                    rv = _bcm_hx5_pfc_deadlock_recovery_begin(unit, cos, port);
                    if (BCM_FAILURE(rv)) {
                        HX5_PFC_UNLOCK(unit);
                        return rv;
                    }
                }
            }

            if (!user_control) {
                /* Update the count for ports already in Deadlock state and Reset
                 * those ports where recovery timer has expired.
                 */
                priority = pfc_deadlock_control->pfc_cos2pri[cos];
                pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);
                /* Updates required for Enabled COS */
                if (SOC_PBMP_NOT_NULL(pfc_deadlock_pri_config->deadlock_ports)) { 
                    rv =_bcm_hx5_pfc_deadlock_recovery_update(unit, cos);
                    if (BCM_FAILURE(rv)) {
                        HX5_PFC_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        }
    }
    HX5_PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

/* HX5 Q_SCHED_DD_CHIP_CONFIG CONTENTS
 * Q_SCHED_DD_CHIP_CONFIG0:DD_TIMER_INIT_VALUE_COS_0~DD_TIMER_INIT_VALUE_COS_4
 * Q_SCHED_DD_CHIP_CONFIG1:DD_TIMER_INIT_VALUE_COS_5~DD_TIMER_INIT_VALUE_COS_9
 * Q_SCHED_DD_CHIP_CONFIG2: ignore_pfc_off_pkt_discard
 * Q_SCHED_DD_CHIP_CONFIG3:DD_TIMER_INIT_VALUE_COS_10~DD_TIMER_INIT_VALUE_COS_26
 */
int
_bcm_hx5_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config)
{
    if (priority <= BCM_HX5_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG0r;
    } else if ( priority > BCM_HX5_PFC_DEADLOCK_CHIP_CONFIG_0_MAX_COS && priority <= BCM_HX5_PFC_DEADLOCK_CHIP_CONFIG_1_MAX_COS) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG1r;
    } else {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG3r;
    }

    return BCM_E_NONE;
}

/* Routine to perform the Set/Get operation for the Priority */
int
_bcm_hx5_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config)
{
    int hw_cos_index = -1;
    int rv = 0;
    uint32 rval = 0;
    uint64 rval64;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    uint32 detection_granularity = 0;
    soc_reg_t   chip_config;
    soc_field_t cos_init_timer_field;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;
    detection_granularity = (_BCM_HX5_PFC_DEADLOCK_TIME_UNIT(unit) ==
        bcmSwitchPFCDeadlockDetectionInterval10MiliSecond) ? 10 : 100;

    BCM_IF_ERROR_RETURN(
        _bcm_hx5_pfc_deadlock_chip_config_get(unit, priority, &chip_config));

    rval = 0;
    if ( chip_config == Q_SCHED_DD_CHIP_CONFIG3r)
    {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, chip_config,
                    REG_PORT_ANY, 0, &rval64));
    } else {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                    REG_PORT_ANY, 0, &rval));
    }

    HX5_PFC_LOCK(unit);
    if (operation == _bcmPfcDeadlockOperGet) {

        if ( chip_config == Q_SCHED_DD_CHIP_CONFIG3r)
        {
            config->detection_timer = soc_reg64_field32_get(unit,
                                                       chip_config, rval64,
                                                       hw_res->time_init_val[priority]);
        } else {
            config->detection_timer = soc_reg_field_get(unit,
                                                       chip_config, rval,
                                                       hw_res->time_init_val[priority]);
        }
        config->detection_timer *= detection_granularity;
    } else { /* _bcmPfcDeadlockOperSet */
        rv = _bcm_hx5_pfc_deadlock_hw_cos_index_set(unit, priority, &hw_cos_index);
        if (BCM_FAILURE(rv)) {
            HX5_PFC_UNLOCK(unit);
            return rv;
        }
        if (hw_cos_index == -1) {
            /* No matching or free hw_index available for use */
            HX5_PFC_UNLOCK(unit);
            return BCM_E_RESOURCE;
        }

        cos_init_timer_field = hw_res->time_init_val[hw_cos_index];
        if ( chip_config == Q_SCHED_DD_CHIP_CONFIG3r)
        {
            soc_reg64_field32_set(unit, chip_config, &rval64,
                    cos_init_timer_field,
                    (config->detection_timer / detection_granularity));
            rv = soc_reg64_set(unit, chip_config, REG_PORT_ANY, 0, rval64);
            if (BCM_FAILURE(rv)) {
                HX5_PFC_UNLOCK(unit);
                return rv;
            }
        } else {
            soc_reg_field_set(unit, chip_config, &rval,
                    cos_init_timer_field,
                    (config->detection_timer / detection_granularity));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, chip_config,
                        REG_PORT_ANY, 0, rval));
        }
        config->priority = priority;
        pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = priority;
        pfc_deadlock_control->pfc_pri2cos[priority] = hw_cos_index;
    }

    HX5_PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

int
_bcm_hx5_pfc_deadlock_q_config_helper(int unit,
                                     _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status)
{
    int rv = BCM_E_NONE;
    uint32 rval;
    bcm_port_t local_port, mmu_port;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    int split, type;
    int priority, hw_cos_index = -1;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);

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

    BCM_IF_ERROR_RETURN(
        bcm_hx5_pfc_deadlock_queue_info_get(unit, gport, &mmu_port, &local_port, &type, &split));
    priority += (type * 8);
    HX5_PFC_LOCK(unit);
    pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);


    rv = _bcm_hx5_pfc_deadlock_hw_cos_index_get(unit, priority, &hw_cos_index);
    if (BCM_FAILURE(rv)) {
        HX5_PFC_UNLOCK(unit);
        return rv;
    }
    if (hw_cos_index == -1) {
        /* No matching or free hw_index available for use */
        HX5_PFC_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    rval = 0;
    rv = soc_reg32_get(unit, hw_res->timer_en[split], local_port, 0, &rval);
    if (BCM_FAILURE(rv)) {
        HX5_PFC_UNLOCK(unit);
        return rv;
    }

    if (operation == _bcmPfcDeadlockOperGet) { /* GET */
        if (enable_status) {
            *enable_status =
                BCM_PBMP_MEMBER(pfc_deadlock_pri_config->deadlock_ports,
                        local_port);
        }
        if (config) {
            config->enable = rval & (1 << priority) ? TRUE : FALSE;
        }
        HX5_PFC_UNLOCK(unit);
        return BCM_E_NONE;
    } else { /* _bcmPfcDeadlockOperSet */
        if (config->enable) {
            rval |= (1 << priority);
            BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports,
                    local_port);
        } else {
            rval &= ~(1 << priority);
            BCM_PBMP_PORT_REMOVE(pfc_deadlock_pri_config->enabled_ports,
                    local_port);
        }

        rv = soc_reg32_set(unit, hw_res->timer_en[split], local_port, 0, rval);
        if (BCM_FAILURE(rv)) {
            HX5_PFC_UNLOCK(unit);
            return rv;
        }
        /*First for that port, set ignore_pfc_xoff = 0 (per port reg) */
        if (SOC_PBMP_IS_NULL(pfc_deadlock_pri_config->enabled_ports)) {
            rv =_bcm_hx5_pfc_deadlock_ignore_pfc_xoff_clear(unit, hw_cos_index, local_port);
            if (BCM_FAILURE(rv)) {
                HX5_PFC_UNLOCK(unit);
                return rv;
            }
            pfc_deadlock_control->hw_cos_idx_inuse[hw_cos_index] = FALSE;
            pfc_deadlock_pri_config->flags &= ~_BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = -1;
            pfc_deadlock_control->pfc_pri2cos[priority] = -1;
        }
    }

    HX5_PFC_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(_bcm_hx5_pfc_deadlock_update_cos_used(unit));
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
_bcm_hx5_pfc_deadlock_recovery_reset(int unit)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    int priority = 0, i;
    bcm_port_t port;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        priority = pfc_deadlock_control->pfc_cos2pri[i];
        /* Skipping invalid priorities */
        if ((priority < 0) || (priority >= HX5_PFC_DEADLOCK_MAX_COS)) {
            continue;
        }
        pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);

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
                    _bcm_hx5_pfc_deadlock_recovery_end(unit, i, port));
        }
    }
    pfc_deadlock_control->cb_enabled = FALSE;
    BCM_IF_ERROR_RETURN(_bcm_hx5_pfc_deadlock_update_cos_used(unit));
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
_bcm_hx5_pfc_deadlock_control_set(int unit, bcm_switch_control_t type, int arg)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval = 0, field_val = 0;
    soc_reg_t chip_config;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
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

            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, chip_config, &rval,
                              hw_res->time_unit_field, field_val);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, chip_config,
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

            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                                REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, chip_config, &rval,
                              hw_res->recovery_action_field, field_val);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, chip_config,
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
_bcm_hx5_pfc_deadlock_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
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
 *      _bcm_hx5_pfc_deadlock_control_get_hw
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
_bcm_hx5_pfc_deadlock_control_get_hw(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    uint32 rval = 0, field_val = 0;
    soc_reg_t chip_config;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
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
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config, REG_PORT_ANY, 0,
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
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
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
_bcm_hx5_pfc_deadlock_default(int unit)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    int i = 0;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);

    _BCM_HX5_PFC_DEADLOCK_HW_RES_INIT(&pfc_deadlock_control->hw_regs_fields);
    pfc_deadlock_control->pfc_deadlock_cos_max = 0;
    pfc_deadlock_control->pfc_deadlock_cos_used = 0;

    for (i = 0; i < COUNTOF(pfc_deadlock_control->hw_cos_idx_inuse); i++) {
        pfc_deadlock_control->hw_cos_idx_inuse[i] = FALSE;
    }
    for (i = 0; i < HX5_PFC_DEADLOCK_MAX_COS; i++) {
        pfc_deadlock_control->pfc_cos2pri[i] = -1;
        pfc_deadlock_control->pfc_pri2cos[i] = -1;
    }
    sal_memset(pfc_deadlock_control->pfc_cos2pri, -1,
            (HX5_PFC_DEADLOCK_MAX_COS * sizeof(bcm_cos_t)));
    sal_memset(pfc_deadlock_control->pfc_pri2cos, -1,
            (HX5_PFC_DEADLOCK_MAX_COS * sizeof(bcm_cos_t)));
    pfc_deadlock_control->cb_enabled = FALSE;
    pfc_deadlock_control->time_unit = 0; /* 100 ms */
    pfc_deadlock_control->cb_interval = 100 * 1000; /* 100 ms */
    return BCM_E_NONE;
}

/* HW res init */
int
_bcm_hx5_pfc_deadlock_hw_res_init(int unit)
{
     int i = 0;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    static soc_reg_t timer_count[2] = {
        Q_SCHED_DD_TIMERr, Q_SCHED_DD_TIMER_HIQr
    };

    static soc_reg_t port_config_regs[2] = {
        Q_SCHED_DD_PORT_CONFIGr, Q_SCHED_DD_PORT_CONFIG_HIQr
    };
    static soc_reg_t timer_en_regs[2] = {
        Q_SCHED_DD_TIMER_ENABLEr, Q_SCHED_DD_TIMER_ENABLE_HIQr
    };
    static soc_reg_t timer_status_regs[2] = {
        Q_SCHED_DD_TIMER_STATUSr, Q_SCHED_DD_TIMER_STATUS_HIQr
    };
    static soc_reg_t timer_mask_regs[2] = {
        Q_SCHED_DD_TIMER_STATUS_MASKr, Q_SCHED_DD_TIMER_STATUS_MASK_HIQr
    };
    static soc_reg_t config_regs[4] = {
        Q_SCHED_DD_CHIP_CONFIG0r, Q_SCHED_DD_CHIP_CONFIG1r, Q_SCHED_DD_CHIP_CONFIG2r, Q_SCHED_DD_CHIP_CONFIG3r
    };

    static soc_field_t deadlock_time_field[26] = {
        DD_TIMER_INIT_VALUE_COS_0f, DD_TIMER_INIT_VALUE_COS_1f,
        DD_TIMER_INIT_VALUE_COS_2f, DD_TIMER_INIT_VALUE_COS_3f,
        DD_TIMER_INIT_VALUE_COS_4f, DD_TIMER_INIT_VALUE_COS_5f,
        DD_TIMER_INIT_VALUE_COS_6f, DD_TIMER_INIT_VALUE_COS_7f,
        DD_TIMER_INIT_VALUE_COS_8f, DD_TIMER_INIT_VALUE_COS_9f,
        DD_TIMER_INIT_VALUE_COS_10f, DD_TIMER_INIT_VALUE_COS_11f,
        DD_TIMER_INIT_VALUE_COS_12f, DD_TIMER_INIT_VALUE_COS_13f,
        DD_TIMER_INIT_VALUE_COS_14f, DD_TIMER_INIT_VALUE_COS_15f,
        DD_TIMER_INIT_VALUE_COS_16f, DD_TIMER_INIT_VALUE_COS_17f,
        DD_TIMER_INIT_VALUE_COS_18f, DD_TIMER_INIT_VALUE_COS_19f,
        DD_TIMER_INIT_VALUE_COS_20f, DD_TIMER_INIT_VALUE_COS_21f,
        DD_TIMER_INIT_VALUE_COS_22f, DD_TIMER_INIT_VALUE_COS_23f,
        DD_TIMER_INIT_VALUE_COS_24f, DD_TIMER_INIT_VALUE_COS_25f
    };

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    hw_res->timer_count[0]  = timer_count[0];
    hw_res->timer_count[1]  = timer_count[1];
    hw_res->timer_status[0] = timer_status_regs[0];
    hw_res->timer_status[1] = timer_status_regs[1];
    hw_res->timer_mask[0]   = timer_mask_regs[0];
    hw_res->timer_mask[1]   = timer_mask_regs[1];
    hw_res->timer_en[0]     = timer_en_regs[0];
    hw_res->timer_en[1]    = timer_en_regs[1];

    for (i = 0; i < HX5_PFC_DEADLOCK_MAX_COS; i++) {
        hw_res->time_init_val[i] = deadlock_time_field[i];
    }
    hw_res->port_config[0] = port_config_regs[0];
    hw_res->port_config[1] = port_config_regs[1];
    hw_res->chip_config[0] = config_regs[0];
    hw_res->chip_config[1] = config_regs[1];
    hw_res->chip_config[2] = config_regs[2];
    hw_res->chip_config[3] = config_regs[3];
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
_bcm_hx5_pfc_deadlock_init(int unit)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    BCM_IF_ERROR_RETURN(_bcm_hx5_pfc_deadlock_default(unit));
    BCM_IF_ERROR_RETURN(_bcm_hx5_pfc_deadlock_hw_res_init(unit));

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    pfc_deadlock_control->pfc_deadlock_cos_max = HX5_PFC_DEADLOCK_MAX_COS;
    pfc_deadlock_control->cosq_inv_mapping_get = _bcm_hx5_cosq_inv_mapping_get;

    _bcm_hx5_pfc_lock[unit] = sal_mutex_create("pfc lock");
    if (_bcm_hx5_pfc_lock[unit] == NULL) {
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
_bcm_hx5_pfc_deadlock_deinit(int unit)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    (void)_bcm_pfc_deadlock_deinit(unit);

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control != NULL) {
        sal_free(pfc_deadlock_control);
        _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit) = NULL;
    }
    if (_bcm_hx5_pfc_lock[unit]) {
        sal_mutex_destroy(_bcm_hx5_pfc_lock[unit]);
        _bcm_hx5_pfc_lock[unit] = NULL;
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
_bcm_hx5_pfc_deadlock_reinit(int unit)
{
    _bcm_hx5_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_hx5_pfc_hw_resorces_t *hw_res = NULL;
    int priority = 0, i, rv, temp_hw_index = -1, arg_value;
    int mmu_port, index;
    int port;
    uint32 rval, rval1;
    uint64 rval64;

    pfc_deadlock_control = _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    BCM_IF_ERROR_RETURN
        (_bcm_hx5_pfc_deadlock_control_get_hw(unit,
                                    bcmSwitchPFCDeadlockDetectionTimeInterval,
                                    &arg_value));
    pfc_deadlock_control->time_unit = arg_value;
    BCM_IF_ERROR_RETURN
        (_bcm_hx5_pfc_deadlock_control_get_hw(unit,
                                    bcmSwitchPFCDeadlockRecoveryAction,
                                    &arg_value));
    pfc_deadlock_control->recovery_action = arg_value;

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        priority = pfc_deadlock_control->pfc_cos2pri[i];
        /* Skipping invalid priorities */
        if ((priority < 0) || (priority >= HX5_PFC_DEADLOCK_MAX_COS)) {
            continue;
        }
        pfc_deadlock_pri_config = _BCM_HX5_PFC_DEADLOCK_CONFIG(unit, priority);

        /* coverity[overrun-call] */
        rv = _bcm_hx5_pfc_deadlock_hw_cos_index_get(unit, priority,
                                                    &temp_hw_index);
        if (BCM_SUCCESS(rv)) {
            pfc_deadlock_pri_config->flags   |= _BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_pri_config->priority = priority;
            /* coverity[overrun-call] */
            BCM_IF_ERROR_RETURN(
                _bcm_hx5_pfc_deadlock_hw_oper(unit, _bcmPfcDeadlockOperGet,
                                          priority, pfc_deadlock_pri_config));

            PBMP_ALL_ITER(unit, port) {
                if ((IS_CPU_PORT(unit, port)) || (IS_LB_PORT(unit, port))) {
                    continue;
                }
                BCM_IF_ERROR_RETURN(
                        _bcm_hx5_pfc_reg_index_get(unit, port, &mmu_port, &index));
                if (mmu_port > 15 && priority > 15 ) {
                    continue;
                }
                rval = 0;
                BCM_IF_ERROR_RETURN(
                        soc_reg32_get(unit, hw_res->timer_en[index], port, 0, &rval));

                rval1 = 0;
                COMPILER_64_SET(rval64, 0, 0);
                if ( hw_res->timer_mask[index] == Q_SCHED_DD_TIMER_STATUS_MASK_HIQr) {
                    BCM_IF_ERROR_RETURN(
                            soc_reg64_get(unit, hw_res->timer_mask[index], port, 0, &rval64));
                } else {
                    BCM_IF_ERROR_RETURN(
                            soc_reg32_get(unit, hw_res->timer_mask[index], port, 0, &rval1));
                }

                if (rval & (1 << priority)) {
                    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                }else if ((rval1 & (1 << priority)) || (COMPILER_64_LO(rval64) & (1 << priority))) {
                    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif
#endif /* BCM_HELIX5_SUPPORT */
