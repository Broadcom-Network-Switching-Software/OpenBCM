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
#include <bcm_int/esw/firebolt6.h>

#if defined(BCM_FIREBOLT6_SUPPORT)


_bcm_fb6_pfc_deadlock_control_t *_bcm_fb6_pfc_deadlock_control[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _bcm_fb6_pfc_lock[SOC_MAX_NUM_DEVICES] = {NULL};
#define FB6_PFC_LOCK(unit) sal_mutex_take(_bcm_fb6_pfc_lock[unit], sal_mutex_FOREVER)
#define FB6_PFC_UNLOCK(unit) sal_mutex_give(_bcm_fb6_pfc_lock[unit])


int
_bcm_fb6_pfc_deadlock_ignore_pfc_xoff_clear(int unit, int cos, bcm_port_t port)
{
    int priority = 0;
    uint32 rval;
    int rv;
    int local_port, type, split;
    int index, offset;
    soc_mem_t mem = INVALIDm;
    soc_field_t field1 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }

    BCM_IF_ERROR_RETURN(
        bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));
    priority = pfc_deadlock_control->pfc_cos2pri[cos];

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_mask[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }
    field1 = (offset == 1)  ? IGNORE_PFC_XOFF1f: IGNORE_PFC_XOFF2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    rval &= ~(1 << (priority %8));
    soc_mem_field32_set(unit, mem, entry, field1, rval);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }
    return BCM_E_NONE;
}

int
_bcm_fb6_pfc_deadlock_hw_cos_index_get(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control->hw_cos_idx_inuse[priority] == TRUE) {
        *hw_cos_index = priority;
        return BCM_E_NONE;
    }
    return BCM_E_NOT_FOUND;
}

int
_bcm_fb6_pfc_deadlock_hw_cos_index_set(int unit,
                  bcm_cos_t priority, int *hw_cos_index)
{
    int rv = BCM_E_NONE;
    int temp_hw_index = -1;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_config = NULL;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);

    rv = _bcm_fb6_pfc_deadlock_hw_cos_index_get(unit, priority, &temp_hw_index);
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
    pfc_deadlock_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);
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
 _bcm_fb6_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port)
{
    int priority = 0;
    uint32 rval, rval1;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    soc_mem_t mem = INVALIDm;
    soc_field_t field1 = INVALIDf;
    soc_field_t field2 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, local_port, split;
    int type, offset, rv;


    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    if (port >= MAX_PORT(unit)) {
        return BCM_E_PARAM; /* Process for valid ports */
    }


    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "PFC Deadlock Recovery ends: Prio %d port=%d\n"),
                    priority, port));

    BCM_IF_ERROR_RETURN(
            bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));

    /* For that port, set ignore_pfc_xoff = 0  */
    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_mask[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }
    field1 = (offset == 1)  ? IGNORE_PFC_XOFF1f: IGNORE_PFC_XOFF2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    rval &= ~(1 << cos);

    soc_mem_field32_set(unit, mem, entry, field1, rval);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

    /* Mask the Intr STATUS_MASK by setting 0 and
     Turn timer off TIMER_ENABLE  by setting 1 */

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_en[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

    field1 = (offset == 1)  ? ENABLE_1f : ENABLE_2f;
    field2 = (offset == 1)  ? STATUS_MASK1f : STATUS_MASK2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    rval1 = soc_mem_field32_get(unit, mem, entry, field2);

    rval |= (1 << cos);    /*enable  the timer by setting 1 */
    rval1 &= ~(1 << cos);  /* set the status mask to 0*/

    soc_mem_field32_set(unit, mem, entry, field1, rval);
    soc_mem_field32_set(unit, mem, entry, field2, rval1);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

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
_bcm_fb6_pfc_deadlock_recovery_begin(int unit, int cos, bcm_port_t port)
{
    uint32 cos_bmp = 0, user_rval = 0;
    uint32 rval, rval1;
    int priority = 0;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    soc_mem_t mem = INVALIDm;
    soc_field_t field1 = INVALIDf;
    soc_field_t field2 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, local_port, split;
    int type, offset, rv;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "PFC Deadlock Detected: Cos %d port=%d\n"), cos, port));


    BCM_IF_ERROR_RETURN(
            bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_en[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

    field1 = (offset == 1)  ? ENABLE_1f : ENABLE_2f;
    field2 = (offset == 1)  ? STATUS_MASK1f : STATUS_MASK2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    rval1 = soc_mem_field32_get(unit, mem, entry, field2);

    rval &= ~(1 << cos);  /* disable the timer by setting 0 */
    rval1 |= (1 << cos);  /* set the status mask to 1*/

    soc_mem_field32_set(unit, mem, entry, field1, rval);
    soc_mem_field32_set(unit, mem, entry, field2, rval1);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

    priority = pfc_deadlock_control->pfc_cos2pri[cos];

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_mask[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }
    field1 = (offset == 1)  ? IGNORE_PFC_XOFF1f: IGNORE_PFC_XOFF2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) &&
            pfc_deadlock_cb->pfc_deadlock_cb) {
        user_rval = pfc_deadlock_cb->pfc_deadlock_cb(unit, port,
                priority, bcmCosqPfcDeadlockRecoveryEventBegin,
                pfc_deadlock_cb->pfc_deadlock_userdata);
        if (user_rval) {
            rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(
                    unit, priority, port, &cos_bmp);
            if (rval != BCM_E_NONE) {
                rval = 0;
                rval |= (1 << priority);
            } else {
                rval = 0;
                rval |= cos_bmp;
            }
            soc_mem_field32_set(unit, mem, entry, field1, rval);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
            if (BCM_FAILURE(rv)) {
                FB6_PFC_UNLOCK(unit);
                return rv;
            }

        }
        return BCM_E_NONE;
    }

    rval = _bcm_pfc_deadlock_ignore_pfc_xoff_gen(unit, priority, port,
                                                 &cos_bmp);

    if(rval != BCM_E_NONE) {
        rval |= (1 << priority);
    } else {
        rval |= cos_bmp;
    }
    soc_mem_field32_set(unit, mem, entry, field1, rval);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if(pfc_deadlock_cb->pfc_deadlock_cb) {
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port,
                                priority, bcmCosqPfcDeadlockRecoveryEventBegin,
                                pfc_deadlock_cb->pfc_deadlock_userdata);
    }

    pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);

    /* Recovery count calculated from Configured Recovery timer.
     * Note: recovery_timer is in msecs and cb_interval is in usecs.
     */
    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);
    pfc_deadlock_pri_config->port_recovery_count[port] = 1 +
        ((pfc_deadlock_pri_config->recovery_timer * 1000) /
         _BCM_FB6_PFC_DEADLOCK_CB_INTERVAL(0));

    return BCM_E_NONE;
}

/* Routine to update the recovery status of ports which are in Recovery state
 */
int
_bcm_fb6_pfc_deadlock_recovery_update(int unit, int cos)
{
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    bcm_port_t port;
    int priority = 0;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    priority = pfc_deadlock_control->pfc_cos2pri[cos];
    pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);

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
                    _bcm_fb6_pfc_deadlock_recovery_end(unit, cos, port));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_pfc_deadlock_monitor
 * Purpose:
 *     1) Monitor the hardware for Deadlock status
 *     2) Start Recovery process for the Deadlocked port/queue
 *     3) Reset Port back to original state on expiry of recovery phase
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Step 1  - Monitor (Deadlock Detection phase):
 *          1.a Sw polls for Deadlock detection intr set for port
 *              dd_timer_status_l0 - 7, where each bit will represent one L0 node.
 *              ex Q_SCHED_DD_TIMER_STATUS_L0.1  =  L0.0 - L0.63
 *          1.b check the status bit in the DD_TIMER_STATE_SPLIT0_PIEP0/1/2/3
 *              based on the L0 number to find the with L1s under the L0 are in
 *              dead lock state.
 *
 *     Step 2: Recovery Begin phase
 *          2.a: Mask the Intr by setting 1 in STATUS_MASK1/2 to corresponding L1 node
 *               in DD_TIMER_CONFIG1_SPLIT0/1/2/3
 *          2.a: Turn the timer off  by setting 0 in ENABLE_1/2 to corresponding L1 node
 *               in DD_TIMER_CONFIG1_SPLIT0/1/2/3
 *          2.c: For that L1, set ignore_pfc_xoff by setting 1 in IGNORE_PFC_XOFF1/2 to
 *               corresponding L1 node in DD_TIMER_CONFIG2_SPLIT0/1/2/3
 *          2.4: Start recovery timer
 *      Step 3: Recovery End phase (On expiry of recovery timer)
 *          3.a: Reset ignore_xoff; set ignore_pfc_xoff = 0 IGNORE_PFC_XOFF1/2 in DD_TIMER_CONFIG2_SPLIT0/1/2/3
 *          3.b: UnMask the Intr by setting 0 STATUS_MASK1/2 in  DD_TIMER_CONFIG1_SPLIT0/1/2/3
 *          3.c: Turn timer on by setting 1 ENABLE_1/2 in DD_TIMER_CONFIG1_SPLIT0/1/2/3 
 */
int
_bcm_fb6_pfc_deadlock_monitor(int unit)
{
    int cos = 0, priority = 0, user_control = 0, rv = 0;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    int index, local_port, type, split, offset;
    uint64 rval64;
    uint32 lo=0, hi=0, dset = 0;
    int l0_num, num_l0s;
    uint32 status;
    bcm_port_t port;
    soc_mem_t mem = Q_SCHED_DD_TIMER_STATUS_L0_PIPE0m;
    soc_field_t field = DEADLOCK_DETECTEDf;
    soc_mem_t mem1 = INVALIDm;
    soc_field_t field1 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    pfc_deadlock_cb = _BCM_UNIT_PFC_DEADLOCK_CB(unit);
    if (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) &&
            pfc_deadlock_cb->pfc_deadlock_cb) {
        user_control = 1;
    }


    FB6_PFC_LOCK(unit);
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
                if (!IS_STK_PORT(unit, port) && !IS_UPLINK_PORT(unit, port) && (cos > 7)) {
                   continue;
                }
                if (IS_STK_PORT(unit, port) && (cos > 17)) {
                    continue;
                }
                BCM_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, port, &l0_num, &num_l0s));
                sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                COMPILER_64_ZERO(rval64);
                index =  l0_num / 64;
                if (index > 8)
                    cli_out("%d  lo_num = %d  index = %d  port = %d\n" ,
                            __LINE__, l0_num, index, port);
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
                if (BCM_FAILURE(rv)) {
                    FB6_PFC_UNLOCK(unit);
                    return rv;
                }

                soc_mem_field64_get(unit, mem, entry, field, &rval64);
                dset = 0;
                lo = 0;
                hi = 0;
                COMPILER_64_TO_32_LO(lo, rval64);
                COMPILER_64_TO_32_HI(hi, rval64);

                if (l0_num % 64 >= 32) {
                   dset = (hi | (1 << (l0_num % 32))) ? 1 : 0;
                } else {
                   dset = (lo | (1 << (l0_num % 32))) ? 1 : 0;
                }

                if (dset) {
                    BCM_IF_ERROR_RETURN(
                            bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));
                    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                    mem1 = hw_res->timer_status[split];
                    rv = soc_mem_read(unit, mem1, MEM_BLOCK_ALL, index, &entry);
                    if (BCM_FAILURE(rv)) {
                        FB6_PFC_UNLOCK(unit);
                        return rv;
                    }
                    field1 = (offset == 1)  ? STATUS1f: STATUS2f;

                    status = 0;
                    status = soc_mem_field32_get(unit, mem1, entry, field1);
                    if ((status != 0) && (status  | (1 << (cos % 8)))) {
                        rv = _bcm_fb6_pfc_deadlock_recovery_begin(unit, cos, port);
                        if (BCM_FAILURE(rv)) {
                            FB6_PFC_UNLOCK(unit);
                            return rv;
                        }
                    }
                }
            }
            if (!user_control) {
                /* Update the count for ports already in Deadlock state and Reset
                 * those ports where recovery timer has expired.
                 */
                priority = pfc_deadlock_control->pfc_cos2pri[cos];
                pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);
                /* Updates required for Enabled COS */
                if (SOC_PBMP_NOT_NULL(pfc_deadlock_pri_config->deadlock_ports)) { 
                    rv =_bcm_fb6_pfc_deadlock_recovery_update(unit, cos);
                    if (BCM_FAILURE(rv)) {
                        FB6_PFC_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        }
    }
    FB6_PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

int
_bcm_fb6_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config)
{
    if (priority < 8) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG0r;
    } else if ( priority >= 8 && priority < 16) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG1_64r;
    } else if ( priority >= 16 && priority < 24) {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG2r;
    } else {
        *chip_config = Q_SCHED_DD_CHIP_CONFIG3r;
    }

    return BCM_E_NONE;
}

/* Routine to perform the Set/Get operation for the Priority */
int
_bcm_fb6_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config)
{
    int hw_cos_index = -1;
    int rv = 0;
    uint32 rval = 0;
    uint64 rval64;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    uint32 detection_granularity = 0;
    soc_reg_t   chip_config;
    soc_field_t cos_init_timer_field;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;
    detection_granularity = (_BCM_FB6_PFC_DEADLOCK_TIME_UNIT(unit) ==
        bcmSwitchPFCDeadlockDetectionInterval10MiliSecond) ? 10 : 100;

    BCM_IF_ERROR_RETURN(
        _bcm_fb6_pfc_deadlock_chip_config_get(unit, priority, &chip_config));

    rval = 0;
    if ( (chip_config == Q_SCHED_DD_CHIP_CONFIG3r) || (chip_config == Q_SCHED_DD_CHIP_CONFIG1_64r))
    {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, chip_config,
                    REG_PORT_ANY, 0, &rval64));
    } else {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, chip_config,
                    REG_PORT_ANY, 0, &rval));
    }

    FB6_PFC_LOCK(unit);
    if (operation == _bcmPfcDeadlockOperGet) {

        if ( (chip_config == Q_SCHED_DD_CHIP_CONFIG3r) || (chip_config == Q_SCHED_DD_CHIP_CONFIG1_64r))
        {
            config->detection_timer = soc_reg64_field32_get(unit,
                                                       chip_config, rval64,
                                                       hw_res->time_init_val[priority%8]);
        } else {
            config->detection_timer = soc_reg_field_get(unit,
                                                       chip_config, rval,
                                                       hw_res->time_init_val[priority%8]);
        }
        config->detection_timer *= detection_granularity;
    } else { /* _bcmPfcDeadlockOperSet */
        rv = _bcm_fb6_pfc_deadlock_hw_cos_index_set(unit, priority, &hw_cos_index);
        if (BCM_FAILURE(rv)) {
            FB6_PFC_UNLOCK(unit);
            return rv;
        }
        if (hw_cos_index == -1) {
            /* No matching or free hw_index available for use */
            FB6_PFC_UNLOCK(unit);
            return BCM_E_RESOURCE;
        }

        cos_init_timer_field = hw_res->time_init_val[hw_cos_index];
        if ( (chip_config == Q_SCHED_DD_CHIP_CONFIG3r) || (chip_config == Q_SCHED_DD_CHIP_CONFIG1_64r))
        {
            soc_reg64_field32_set(unit, chip_config, &rval64,
                    cos_init_timer_field,
                    (config->detection_timer / detection_granularity));
            rv = soc_reg64_set(unit, chip_config, REG_PORT_ANY, 0, rval64);
            if (BCM_FAILURE(rv)) {
                FB6_PFC_UNLOCK(unit);
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

    FB6_PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

int
_bcm_fb6_pfc_deadlock_q_config_helper(int unit,
                                     _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status)
{
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    int split, type;
    int index, offset;
    int priority, hw_cos_index = -1;
    uint32 rval;
    soc_mem_t mem = INVALIDm;
    soc_field_t field1 = INVALIDf;
    soc_field_t field2 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);

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
        bcm_fb6_pfc_deadlock_queue_info_get(unit, gport, &local_port, &type, &split, &index, &offset));
    FB6_PFC_LOCK(unit);
    pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);


    rv = _bcm_fb6_pfc_deadlock_hw_cos_index_get(unit, priority, &hw_cos_index);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }
    if (hw_cos_index == -1) {
        /* No matching or free hw_index available for use */
        FB6_PFC_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_en[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        FB6_PFC_UNLOCK(unit);
        return rv;
    }

    field1 = (offset == 1)  ? ENABLE_1f : ENABLE_2f;
    field2 = (offset == 1)  ? INIT_VALUE_SEL1f: INIT_VALUE_SEL2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    if (operation == _bcmPfcDeadlockOperGet) { /* GET */
        if (enable_status) {
            *enable_status =
                BCM_PBMP_MEMBER(pfc_deadlock_pri_config->deadlock_ports,
                        local_port);
        }
        if (config) {
            config->enable = rval & (1 << (priority % 8)) ? TRUE : FALSE;
        }
        FB6_PFC_UNLOCK(unit);
        return BCM_E_NONE;
    } else { /* _bcmPfcDeadlockOperSet */
        if (config->enable) {
            rval |= (1 << priority);
            BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports,
                    local_port);
        } else {
            rval &= ~(1 << (priority % 8));
            BCM_PBMP_PORT_REMOVE(pfc_deadlock_pri_config->enabled_ports,
                    local_port);
        }

        soc_mem_field32_set(unit, mem, entry, field1, rval);
        soc_mem_field32_set(unit, mem, entry, field2, type);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
        if (BCM_FAILURE(rv)) {
            FB6_PFC_UNLOCK(unit);
            return rv;
        }
        /*First for that port, set ignore_pfc_xoff = 0 (per port reg) */
        if (SOC_PBMP_IS_NULL(pfc_deadlock_pri_config->enabled_ports)) {
            rv =_bcm_fb6_pfc_deadlock_ignore_pfc_xoff_clear(unit, hw_cos_index, local_port);
            if (BCM_FAILURE(rv)) {
                FB6_PFC_UNLOCK(unit);
                return rv;
            }
            pfc_deadlock_control->hw_cos_idx_inuse[hw_cos_index] = FALSE;
            pfc_deadlock_pri_config->flags &= ~_BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_control->pfc_cos2pri[hw_cos_index] = -1;
            pfc_deadlock_control->pfc_pri2cos[priority] = -1;
        }
    }
    FB6_PFC_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_update_cos_used(unit));
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
_bcm_fb6_pfc_deadlock_recovery_reset(int unit)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    int priority = 0, i;
    bcm_port_t port;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        priority = pfc_deadlock_control->pfc_cos2pri[i];
        /* Skipping invalid priorities */
        if ((priority < 0) || (priority >= FB6_PFC_DEADLOCK_MAX_COS)) {
            continue;
        }
        pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);

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
                    _bcm_fb6_pfc_deadlock_recovery_end(unit, i, port));
        }
    }
    pfc_deadlock_control->cb_enabled = FALSE;
    BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_update_cos_used(unit));
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
_bcm_fb6_pfc_deadlock_control_set(int unit, bcm_switch_control_t type, int arg)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    uint32 field_val = 0;
    uint64 rval64;
    soc_reg_t chip_config;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
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

            if (arg == bcmSwitchPFCDeadlockDetectionInterval10MiliSecond) {
                field_val = 0;
            } else {
                field_val = 1; /* Default Unit - Order of 100ms */
            }

            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }
            COMPILER_64_SET(rval64, 0, 0);

            BCM_IF_ERROR_RETURN(soc_reg64_get(unit, chip_config, REG_PORT_ANY, 0, &rval64));
            soc_reg64_field32_set(unit, chip_config, &rval64, hw_res->time_unit_field, field_val);
            BCM_IF_ERROR_RETURN(soc_reg64_set(unit, chip_config, REG_PORT_ANY, 0, rval64));
            pfc_deadlock_control->time_unit = arg;
            break;
        case bcmSwitchPFCDeadlockRecoveryAction:
            if ((arg < 0) ||
                (arg >= bcmSwitchPFCDeadlockActionMaxCount)) {
                return BCM_E_PARAM;
            }

            chip_config = hw_res->chip_config[3];
            if (arg == bcmSwitchPFCDeadlockActionDrop) {
                field_val = 1;
            } else {
                field_val = 0; /* Default action - Transmit */
            }

            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }

            COMPILER_64_SET(rval64, 0, 0);
            BCM_IF_ERROR_RETURN(soc_reg64_get(unit, chip_config, REG_PORT_ANY, 0, &rval64));
            soc_reg64_field32_set(unit, chip_config, &rval64, hw_res->recovery_action_field, field_val);
            BCM_IF_ERROR_RETURN(soc_reg64_set(unit, chip_config, REG_PORT_ANY, 0, rval64));
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
_bcm_fb6_pfc_deadlock_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
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
 *      _bcm_fb6_pfc_deadlock_control_get_hw
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
_bcm_fb6_pfc_deadlock_control_get_hw(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    uint64 rval;
    int field_val = 0;
    soc_reg_t chip_config;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
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
            COMPILER_64_ZERO(rval);
            BCM_IF_ERROR_RETURN(soc_reg64_get(unit, chip_config, REG_PORT_ANY, 0, &rval));
            field_val = soc_reg64_field32_get(unit, chip_config, rval, hw_res->time_unit_field);
            if (field_val == 0) {
                *arg = bcmSwitchPFCDeadlockDetectionInterval10MiliSecond;
            } else {
                *arg = bcmSwitchPFCDeadlockDetectionInterval100MiliSecond;
            }
            break;
        case bcmSwitchPFCDeadlockRecoveryAction:
            chip_config = hw_res->chip_config[3];
            if (chip_config == INVALIDr) {
                return BCM_E_UNAVAIL;
            }
            COMPILER_64_ZERO(rval);
            BCM_IF_ERROR_RETURN(soc_reg64_get(unit, chip_config, REG_PORT_ANY, 0, &rval));
            field_val = soc_reg64_field32_get(unit, chip_config, rval, hw_res->recovery_action_field);

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
_bcm_fb6_pfc_deadlock_default(int unit)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    int i = 0;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);

    _BCM_FB6_PFC_DEADLOCK_HW_RES_INIT(&pfc_deadlock_control->hw_regs_fields);
    pfc_deadlock_control->pfc_deadlock_cos_max = 0;
    pfc_deadlock_control->pfc_deadlock_cos_used = 0;

    for (i = 0; i < COUNTOF(pfc_deadlock_control->hw_cos_idx_inuse); i++) {
        pfc_deadlock_control->hw_cos_idx_inuse[i] = FALSE;
    }
    for (i = 0; i < FB6_PFC_DEADLOCK_MAX_COS; i++) {
        pfc_deadlock_control->pfc_cos2pri[i] = -1;
        pfc_deadlock_control->pfc_pri2cos[i] = -1;
    }
    sal_memset(pfc_deadlock_control->pfc_cos2pri, -1,
            (FB6_PFC_DEADLOCK_MAX_COS * sizeof(bcm_cos_t)));
    sal_memset(pfc_deadlock_control->pfc_pri2cos, -1,
            (FB6_PFC_DEADLOCK_MAX_COS * sizeof(bcm_cos_t)));
    pfc_deadlock_control->cb_enabled = FALSE;
    pfc_deadlock_control->time_unit = 0; /* 100 ms */
    pfc_deadlock_control->cb_interval = 100 * 1000; /* 100 ms */
    return BCM_E_NONE;
}

/* HW res init */
int
_bcm_fb6_pfc_deadlock_hw_res_init(int unit)
{
     int i = 0;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    static soc_mem_t port_config_mems[4] = {
        DD_TIMER_CONFIG1_SPLIT0_PIPE0m, DD_TIMER_CONFIG1_SPLIT1_PIPE0m,
        DD_TIMER_CONFIG1_SPLIT2_PIPE0m, DD_TIMER_CONFIG1_SPLIT3_PIPE0m
    };
    static soc_mem_t timer_en_mems[4] = {
        DD_TIMER_CONFIG1_SPLIT0_PIPE0m, DD_TIMER_CONFIG1_SPLIT1_PIPE0m,
        DD_TIMER_CONFIG1_SPLIT2_PIPE0m, DD_TIMER_CONFIG1_SPLIT3_PIPE0m
    };

    static soc_mem_t timer_status_mems[4] = {
        DD_TIMER_STATE_SPLIT0_PIPE0m, DD_TIMER_STATE_SPLIT1_PIPE0m,
        DD_TIMER_STATE_SPLIT2_PIPE0m, DD_TIMER_STATE_SPLIT3_PIPE0m
    };

    static soc_mem_t timer_mask_mems[4] = {
        DD_TIMER_CONFIG2_SPLIT0_PIPE0m, DD_TIMER_CONFIG2_SPLIT1_PIPE0m,
        DD_TIMER_CONFIG2_SPLIT2_PIPE0m, DD_TIMER_CONFIG2_SPLIT3_PIPE0m
    };
    static soc_reg_t config_regs[4] = {
        Q_SCHED_DD_CHIP_CONFIG0r, Q_SCHED_DD_CHIP_CONFIG1_64r,
        Q_SCHED_DD_CHIP_CONFIG2r, Q_SCHED_DD_CHIP_CONFIG3r
    };

    static soc_field_t deadlock_time_field[8] = {
        DD_TIMER_INIT_VALUE_COS_0f, DD_TIMER_INIT_VALUE_COS_1f,
        DD_TIMER_INIT_VALUE_COS_2f, DD_TIMER_INIT_VALUE_COS_3f,
        DD_TIMER_INIT_VALUE_COS_4f, DD_TIMER_INIT_VALUE_COS_5f,
        DD_TIMER_INIT_VALUE_COS_6f, DD_TIMER_INIT_VALUE_COS_7f
    };

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    for (i = 0; i < 4; i++) {
        hw_res->timer_status[i] = timer_status_mems[i];
        hw_res->timer_mask[i]   = timer_mask_mems[i];
        hw_res->timer_en[i]     = timer_en_mems[i];
        hw_res->port_config[i] = port_config_mems[i];
        hw_res->chip_config[i] = config_regs[i];
    }

    for (i = 0; i < FB6_PFC_DEADLOCK_MAX_COS; i++) {
        hw_res->time_init_val[i] = deadlock_time_field[i%8];
    }
    /* Q_SCHED_DD_CHIP_CONFIG1 */
    hw_res->time_unit_field = DD_TIMER_TICK_UNITf;
    /* Q_SCHED_DD_CHIP_CONFIG2 */
    hw_res->recovery_action_field = IGNORE_PFC_OFF_PKT_DISCARDf;
    /* Q_SCHED_DD_TIMER_STATUS_L0/1/2/3 */
    hw_res->deadlock_status_field = DEADLOCK_DETECTEDf;
    return BCM_E_NONE;
}

/* Chip specific init and assign default values */
int
_bcm_fb6_pfc_deadlock_init(int unit)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_default(unit));
    BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_hw_res_init(unit));

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    pfc_deadlock_control->pfc_deadlock_cos_max = FB6_PFC_DEADLOCK_MAX_COS;
    pfc_deadlock_control->cosq_inv_mapping_get = _bcm_fb6_cosq_inv_mapping_get;

    _bcm_fb6_pfc_lock[unit] = sal_mutex_create("pfc lock");
    if (_bcm_fb6_pfc_lock[unit] == NULL) {
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
_bcm_fb6_pfc_deadlock_deinit(int unit)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    (void)_bcm_pfc_deadlock_deinit(unit);

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control != NULL) {
        sal_free(pfc_deadlock_control);
        _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit) = NULL;
    }
    if (_bcm_fb6_pfc_lock[unit]) {
        sal_mutex_destroy(_bcm_fb6_pfc_lock[unit]);
        _bcm_fb6_pfc_lock[unit] = NULL;
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
_bcm_fb6_pfc_deadlock_reinit(int unit)
{
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_config_t *pfc_deadlock_pri_config = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    int priority = 0, i, temp_hw_index = -1, arg_value;
    int port;
    soc_mem_t mem = INVALIDm;
    soc_field_t field1 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, local_port, split;
    int type, offset, rv;
    uint32 rval, rval1;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_pfc_deadlock_control_get_hw(unit,
                                              bcmSwitchPFCDeadlockDetectionTimeInterval,
                                              &arg_value));
    pfc_deadlock_control->time_unit = arg_value;
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_pfc_deadlock_control_get_hw(unit,
                                              bcmSwitchPFCDeadlockRecoveryAction,
                                              &arg_value));
    pfc_deadlock_control->recovery_action = arg_value;

    for (i = 0; i < pfc_deadlock_control->pfc_deadlock_cos_max; i++) {
        priority = pfc_deadlock_control->pfc_cos2pri[i];
        /* Skipping invalid priorities */
        if ((priority < 0) || (priority >= FB6_PFC_DEADLOCK_MAX_COS)) {
            continue;
        }
        pfc_deadlock_pri_config = _BCM_FB6_PFC_DEADLOCK_CONFIG(unit, priority);

        /* coverity[overrun-call] */
        rv = _bcm_fb6_pfc_deadlock_hw_cos_index_get(unit, priority,
                &temp_hw_index);
        if (BCM_SUCCESS(rv)) {
            pfc_deadlock_pri_config->flags   |= _BCM_PFC_DEADLOCK_F_ENABLE;
            pfc_deadlock_pri_config->priority = priority;
            /* coverity[overrun-call] */
            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_pfc_deadlock_hw_oper(unit, _bcmPfcDeadlockOperGet,
                        priority, pfc_deadlock_pri_config));

            PBMP_ALL_ITER(unit, port) {
                if ((IS_CPU_PORT(unit, port)) || (IS_LB_PORT(unit, port))) {
                    continue;
                }
                BCM_IF_ERROR_RETURN(
                        bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));
                sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                mem = hw_res->timer_mask[split];
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
                if (BCM_FAILURE(rv)) {
                    FB6_PFC_UNLOCK(unit);
                    return rv;
                }
                field1 = (offset == 1)  ? IGNORE_PFC_XOFF1f: IGNORE_PFC_XOFF2f;
                rval1 = soc_mem_field32_get(unit, mem, entry, field1);

                sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                mem = hw_res->timer_en[split];
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
                if (BCM_FAILURE(rv)) {
                    FB6_PFC_UNLOCK(unit);
                    return rv;
                }

                field1 = (offset == 1)  ? ENABLE_1f : ENABLE_2f;
                rval = soc_mem_field32_get(unit, mem, entry, field1);

                if (rval & (1 << priority)) {
                    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                }else if (rval1 & (1 << priority)) {
                    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->enabled_ports, port);
                    BCM_PBMP_PORT_ADD(pfc_deadlock_pri_config->deadlock_ports, port);
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif
#endif /* BCM_FIREBOLT6_SUPPORT */
