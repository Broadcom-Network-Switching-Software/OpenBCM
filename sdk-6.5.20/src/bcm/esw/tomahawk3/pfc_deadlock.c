/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PFC Deadlock Detection & Recovery
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/pfc.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk3.h>
#include <soc/scache_dictionary.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw/tomahawk3.h>

#define _BCM_TH3_PFC_DEADLOCK_TIMER_MAX 15
typedef enum _bcm_th3_pfc_deadlock_op_e {
    _BCM_TH3_PFC_DEADLOCK_OP_GET = 0,
    _BCM_TH3_PFC_DEADLOCK_OP_SET = 1
} _bcm_th3_pfc_deadlock_op_t;

typedef enum _bcm_th3_pfc_deadlock_op_origin_e {
    _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL = 0,
    _BCM_TH3_PFC_DEADLOCK_OP_EXTERNAL = 1

} _bcm_th3_pfc_deadlock_op_origin_t;

typedef struct _bcm_th3_pfc_deadlock_port_config_s {
    uint32 recovery_timers[TH3_PFC_PRIORITY_NUM]; /* Recovery timer for Action */
    uint32 recovery_count[TH3_PFC_PRIORITY_NUM]; /* the recovery count for a give priority */
    uint32 recovery_count_acc[TH3_PFC_PRIORITY_NUM]; /* accumulated recovery time for a given priority */
    uint32 enabled_priority; /* Bitmap of enabled PFC priority */
    uint32 deadlock_priority; /* Bitmap of PFC priority currently in Deadlock State */
    uint32 deadlock_exit_mode; /* PFC deadlock exit mode. */

} _bcm_th3_pfc_deadlock_port_config_t;

typedef struct _bcm_th3_pfc_deadlock_control_s {
    uint8 cb_enabled; /* indicate if callback is enabled */
    uint32 cb_interval; /* Callback interval, unit is in microsecond */
    uint32 cb_count; /* Debug ctr to keep track of #times CB function is called */
    _bcm_th3_pfc_deadlock_port_config_t port_config[SOC_MAX_NUM_PORTS];
    bcm_switch_pfc_deadlock_action_t recovery_action; /* chip behavior during deadlock recovery */

} _bcm_th3_pfc_deadlock_control_t;

static _bcm_th3_pfc_deadlock_control_t *_bcm_th3_pfc_deadlock_control[BCM_MAX_NUM_UNITS];
static _bcm_pfc_deadlock_cb_t *_bcm_th3_pfc_deadlock_cb[BCM_MAX_NUM_UNITS];

STATIC sal_mutex_t _bcm_th3_pfc_lock[SOC_MAX_NUM_DEVICES] = {NULL};
#define PFC_LOCK(unit) sal_mutex_take(_bcm_th3_pfc_lock[unit], sal_mutex_FOREVER)
#define PFC_UNLOCK(unit) sal_mutex_give(_bcm_th3_pfc_lock[unit])

#define _BCM_TH3_PFC_DEADLOCK_CB(u) _bcm_th3_pfc_deadlock_cb[(u)]
#define _BCM_TH3_PFC_DEADLOCK_CONTROL(u) _bcm_th3_pfc_deadlock_control[(u)]
#define _BCM_TH3_PFC_DEADLOCK_RECOVERY_TIMER(u, pri, port) \
           _bcm_th3_pfc_deadlock_control[(u)]->port_config[(port)].recovery_timers[(pri)]
#define _BCM_TH3_PFC_DEADLOCK_RECOVERY_ACCUMULATED(u, pri, port) \
           _bcm_th3_pfc_deadlock_control[(u)]->port_config[(port)].recovery_count_acc[(pri)]


#define _BCM_TH3_PFC_DEADLOCK_CONFIG(u, port) \
                (&(_bcm_th3_pfc_deadlock_control[(u)]->port_config[(port)]))
#define _BCM_TH3_PFC_DEADLOCK_CB_ENABLED(u) \
                (_bcm_th3_pfc_deadlock_control[(u)]->cb_enabled)
#define _BCM_TH3_PFC_DEADLOCK_CB_COUNT(u) \
                (_bcm_th3_pfc_deadlock_control[(u)]->cb_count)

#define _BCM_TH3_PFC_DEADLOCK_CB_INTERVAL(u) \
                (_bcm_th3_pfc_deadlock_control[(u)]->cb_interval)
static soc_field_t _timer_init_fields[] = {
                                    INIT_VALUE_PFC0f, INIT_VALUE_PFC1f,
                                    INIT_VALUE_PFC2f, INIT_VALUE_PFC3f,
                                    INIT_VALUE_PFC4f, INIT_VALUE_PFC5f,
                                    INIT_VALUE_PFC6f, INIT_VALUE_PFC7f};
static soc_field_t _timer_tick_fields[] = {
                                    TICK_UNIT_PFC0f, TICK_UNIT_PFC1f,
                                    TICK_UNIT_PFC2f, TICK_UNIT_PFC3f,
                                    TICK_UNIT_PFC4f, TICK_UNIT_PFC5f,
                                    TICK_UNIT_PFC6f, TICK_UNIT_PFC7f,
                                   };

static const int _timer_tick_encoding[3] = {1, 10, 100};
static sal_sem_t th3_pfc_deinit_notify[BCM_MAX_NUM_UNITS];
static int th3_pfc_init[BCM_MAX_NUM_UNITS] = {FALSE};

STATIC int
_bcm_th3_pfc_deadlock_enable_config(int unit, bcm_port_t port,
    int pfc_priority, _bcm_th3_pfc_deadlock_op_t op,
    _bcm_th3_pfc_deadlock_op_origin_t op_origin,
    int *arg);

STATIC int
_bcm_th3_pfc_dd_hw_intr_status_get(int unit, int port, int pfc_priority, int
        *intr_stat) {
    uint32 rval_status, fval_status;
    soc_reg_t int_status_reg = MMU_INTFI_DD_TIMER_INT_STATUSr;
    soc_field_t status_field = PFC_PRIORITY_ISRf;
    if (intr_stat == NULL) {
        return BCM_E_PARAM;
    }
    *intr_stat = 0;
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, int_status_reg, port, 0,
                &rval_status));
    fval_status = soc_reg_field_get(unit,
                    int_status_reg, rval_status, status_field);
    if (fval_status & (1 << pfc_priority)) {
        *intr_stat = 1;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pfc_dd_hw_enable_intr(int unit, int port, int pfc_priority) {
    uint32 rval = 0;
    soc_reg_t reg;
    int timer_enable;

    /* 1 Turn timer of before clearing interrupt */
    timer_enable = 0;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                        _BCM_TH3_PFC_DEADLOCK_OP_SET,
                    _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));
    /* 2: clear interrupt status, set 1 to clear */
    rval = 0;
    reg = MMU_INTFI_DD_TIMER_INT_STATUSr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,
                                        port, 0, &rval));
    rval &= (1U << pfc_priority);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, reg, port, 0, rval));
    /* 3 Turn timer back on */
    timer_enable = 1;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                        _BCM_TH3_PFC_DEADLOCK_OP_SET,
                    _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));

    return BCM_E_NONE;

}

/* Routine to begin recovery when a (Port, PFC priority) is deaclared to be
 * in Deadlock by Hardware
 */
STATIC int
_bcm_th3_pfc_deadlock_recovery_begin(int unit, int port, int pfc_priority)
{
    uint32 rval, temp_mask, ignore_pfc_val;
    int timer_enable;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    soc_reg_t int_mask_reg = MMU_INTFI_DD_TIMER_INT_MASKr,
            ignore_pfc_reg = MMU_INTFI_IGNORE_PORT_PFC_XOFFr;
    soc_field_t mask_field = PFC_PRIORITY_INT_MASKf,
                ignore_pfc_f = PRI_IGNORE_XOFFf;

    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);
    if (!soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) ||
        (soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) &&
         pfc_deadlock_cb->pfc_deadlock_cb == NULL)) {
        /* Auto or SDK sequence mode */
        /* Mask the interrupt, disable hardware detection timer, ignore_pfc_xoff
         * and call user cllback function*/
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "PFC Deadlock Detected: Priority %d port=%d\n"),
                             pfc_priority, port));
        /* 1: Mask the Intr by setting 1  */
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, int_mask_reg,
                                    port, 0, &rval));
        temp_mask = soc_reg_field_get(unit, int_mask_reg, rval, mask_field);
        temp_mask |= 1U << pfc_priority;
        soc_reg_field_set(unit, int_mask_reg, &rval, mask_field, temp_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, int_mask_reg, port, 0, rval));

        /* 2: Turn timer off */
        timer_enable = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                                _BCM_TH3_PFC_DEADLOCK_OP_SET,
                            _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));

        /* 3: For that port, set ignore_pfc_xoff = 1 (per port reg) */
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ignore_pfc_reg,
                                        port, 0, &rval));
        ignore_pfc_val = soc_reg_field_get(unit, ignore_pfc_reg, rval, ignore_pfc_f);
        ignore_pfc_val |= 1U << pfc_priority;
        soc_reg_field_set(unit, ignore_pfc_reg, &rval, ignore_pfc_f, ignore_pfc_val);

        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, ignore_pfc_reg, port, 0, rval));

        /* run user callbcak function */
        if(pfc_deadlock_cb->pfc_deadlock_cb) {
            pfc_deadlock_cb->pfc_deadlock_cb(unit, port, pfc_priority,
                                  bcmCosqPfcDeadlockRecoveryEventBegin,
                                  pfc_deadlock_cb->pfc_deadlock_userdata);
        }

        port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
        /* value 1 is added because the _bcm_th3_pfc_deadlock_recovery_update is
         * called immediately
         * first time when recovery begins, i.e before _BCM_TH3_PFC_DEADLOCK_CB_INTERVAL ticks.
         */
        port_config->recovery_count[pfc_priority] = 1 +
                          sal_ceil_func((port_config->recovery_timers[pfc_priority] * 1000) ,
                                _BCM_TH3_PFC_DEADLOCK_CB_INTERVAL(unit));

        port_config->deadlock_priority |= (1U << pfc_priority);
    } else {
        /* User control recovery mode */
        /* Mask the interrupt, disable HW detection timer and run user callback*/
        /* Expect user call back to handle recovery_start and recovery_exit*/
        /* 1: Mask the Intr by setting 1  */
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, int_mask_reg,
                                    port, 0, &rval));
        temp_mask = soc_reg_field_get(unit, int_mask_reg, rval, mask_field);
        temp_mask |= 1U << pfc_priority;
        soc_reg_field_set(unit, int_mask_reg, &rval, mask_field, temp_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, int_mask_reg, port, 0, rval));

        /* 2: Turn timer off */
        timer_enable = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                                _BCM_TH3_PFC_DEADLOCK_OP_SET,
                            _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));

        /* run user callbcak function */
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port, pfc_priority,
                                        bcmCosqPfcDeadlockRecoveryEventBegin,
                                        pfc_deadlock_cb->pfc_deadlock_userdata);

    }
    return BCM_E_NONE;
}

int
bcm_th3_pfc_deadlock_recovery_start(int unit, int port, int pfc_priority)
{
    uint32 rval, temp_mask, ignore_pfc_val;
    int timer_enable;
    soc_reg_t int_mask_reg = MMU_INTFI_DD_TIMER_INT_MASKr,
            ignore_pfc_reg = MMU_INTFI_IGNORE_PORT_PFC_XOFFr;
    soc_field_t mask_field = PFC_PRIORITY_INT_MASKf,
                ignore_pfc_f = PRI_IGNORE_XOFFf;

        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "PFC Deadlock Detected: Priority %d port=%d\n"),
                             pfc_priority, port));
        /* 1: Mask the Intr by setting 1  */
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, int_mask_reg,
                                    port, 0, &rval));
        temp_mask = soc_reg_field_get(unit, int_mask_reg, rval, mask_field);
        temp_mask |= 1U << pfc_priority;
        soc_reg_field_set(unit, int_mask_reg, &rval, mask_field, temp_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, int_mask_reg, port, 0, rval));

        /* 2: Turn timer off */
        timer_enable = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                                _BCM_TH3_PFC_DEADLOCK_OP_SET,
                            _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));

        /* 3: For that port, set ignore_pfc_xoff = 1 (per port reg) */
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ignore_pfc_reg,
                                        port, 0, &rval));
        ignore_pfc_val = soc_reg_field_get(unit, ignore_pfc_reg, rval, ignore_pfc_f);
        ignore_pfc_val |= 1U << pfc_priority;
        soc_reg_field_set(unit, ignore_pfc_reg, &rval, ignore_pfc_f, ignore_pfc_val);

        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, ignore_pfc_reg, port, 0, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pfc_deadlock_recovery_end(int unit, int port, int pfc_priority)
{
    uint32 rval, temp_mask, ignore_pfc_val;
    int timer_enable;
    soc_reg_t reg, int_mask_reg = MMU_INTFI_DD_TIMER_INT_MASKr,
            ignore_pfc_reg = MMU_INTFI_IGNORE_PORT_PFC_XOFFr;
    soc_field_t mask_field = PFC_PRIORITY_INT_MASKf,
                ignore_pfc_f = PRI_IGNORE_XOFFf;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "PFC Deadlock Recovery ends: Prio %d port=%d\n"),
                         pfc_priority, port));

    /* 1: For that port, set ignore_pfc_xoff = 0 (per port reg) */
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ignore_pfc_reg,
                                            port, 0, &rval));
    ignore_pfc_val = soc_reg_field_get(unit, ignore_pfc_reg, rval, ignore_pfc_f);
    ignore_pfc_val &= ~(1U << pfc_priority);
    soc_reg_field_set(unit, ignore_pfc_reg, &rval, ignore_pfc_f, ignore_pfc_val);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, ignore_pfc_reg, port, 0, rval));

    /* 2: Unmask the by setting 0 */
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, int_mask_reg,
                                port, 0, &rval));
    temp_mask = soc_reg_field_get(unit, int_mask_reg, rval, mask_field);
    temp_mask &= ~(1U << pfc_priority);
    soc_reg_field_set(unit, int_mask_reg, &rval, mask_field, temp_mask);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, int_mask_reg, port, 0, rval));

    /* 3: clear interrupt status, set 1 to clear */
    rval = 0;
    reg = MMU_INTFI_DD_TIMER_INT_STATUSr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,
                                        port, 0, &rval));
    rval &= (1U << pfc_priority);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, reg, port, 0, rval));
    /* 4 Turn timer back on */
    timer_enable = 1;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                    _BCM_TH3_PFC_DEADLOCK_OP_SET,
                    _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));

    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);
    if(pfc_deadlock_cb->pfc_deadlock_cb) {
        pfc_deadlock_cb->pfc_deadlock_cb(unit, port, pfc_priority,
                              bcmCosqPfcDeadlockRecoveryEventEnd,
                              pfc_deadlock_cb->pfc_deadlock_userdata);
    }

    port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
    port_config->deadlock_priority &= ~(1U << pfc_priority);


    return BCM_E_NONE;
}

int
bcm_th3_pfc_deadlock_recovery_exit(int unit, int port, int pfc_priority)
{
    uint32 rval, temp_mask, ignore_pfc_val;
    int timer_enable;
    soc_reg_t reg, int_mask_reg = MMU_INTFI_DD_TIMER_INT_MASKr,
            ignore_pfc_reg = MMU_INTFI_IGNORE_PORT_PFC_XOFFr;
    soc_field_t mask_field = PFC_PRIORITY_INT_MASKf,
                ignore_pfc_f = PRI_IGNORE_XOFFf;
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "PFC Deadlock Recovery ends: Prio %d port=%d\n"),
                         pfc_priority, port));

    /* 1: For that port, set ignore_pfc_xoff = 0 (per port reg) */
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ignore_pfc_reg,
                                            port, 0, &rval));
    ignore_pfc_val = soc_reg_field_get(unit, ignore_pfc_reg, rval, ignore_pfc_f);
    ignore_pfc_val &= ~(1U << pfc_priority);
    soc_reg_field_set(unit, ignore_pfc_reg, &rval, ignore_pfc_f, ignore_pfc_val);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, ignore_pfc_reg, port, 0, rval));

    /* 2: Unmask the by setting 0 */
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, int_mask_reg,
                                port, 0, &rval));
    temp_mask = soc_reg_field_get(unit, int_mask_reg, rval, mask_field);
    temp_mask &= ~(1U << pfc_priority);
    soc_reg_field_set(unit, int_mask_reg, &rval, mask_field, temp_mask);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, int_mask_reg, port, 0, rval));

    /* 3: clear interrupt status, set 1 to clear */
    rval = 0;
    reg = MMU_INTFI_DD_TIMER_INT_STATUSr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,
                                        port, 0, &rval));
    rval &= (1U << pfc_priority);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, reg, port, 0, rval));
    /* 4 Turn timer back on */
    timer_enable = 1;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_pfc_deadlock_enable_config(unit, port, pfc_priority,
                    _BCM_TH3_PFC_DEADLOCK_OP_SET,
                    _BCM_TH3_PFC_DEADLOCK_OP_INTERNAL, &timer_enable));


    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pfc_deadlock_recovery_update(int unit, int port, int pfc_priority)
{
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    int recovery_exited = 0;
    int intr_status, pri_enabled = 0;
    port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);

    if ((--port_config->recovery_count[pfc_priority]) == 0) {
        if (port_config->deadlock_exit_mode == 1) {
            BCM_IF_ERROR_RETURN(_bcm_th3_pfc_dd_hw_intr_status_get
                                    (unit, port, pfc_priority, &intr_status));
            pri_enabled = port_config->enabled_priority & (1U << pfc_priority);
            if (intr_status == 0 || pri_enabled == 0) {
                recovery_exited = 1;
                BCM_IF_ERROR_RETURN(
                    _bcm_th3_pfc_deadlock_recovery_end(unit, port, pfc_priority));
            } else {
                /* Re-initalize the recovery count to continue in recovery mode */
                port_config->recovery_count[pfc_priority] = 1 +
                          sal_ceil_func((port_config->recovery_timers[pfc_priority] * 1000) ,
                                _BCM_TH3_PFC_DEADLOCK_CB_INTERVAL(unit));
                /* Re-enable hardware timer and interrupt */
                BCM_IF_ERROR_RETURN(_bcm_th3_pfc_dd_hw_enable_intr(unit, port,
                            pfc_priority));
            }
        } else {
            recovery_exited = 1;
            BCM_IF_ERROR_RETURN(
                _bcm_th3_pfc_deadlock_recovery_end(unit, port, pfc_priority));
        }
    }
    if (recovery_exited == 0) {
        /* port_config->recovery_count is set to value + 1, for the reason
         * first time i.e when recovery is initiated this is called immediately,
         * and before _BCM_TH3_PFC_DEADLOCK_CB_INTERVAL ticks.
         */
        port_config->recovery_count_acc[pfc_priority]++;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pfc_deadlock_monitor(int unit)
{
    int i, port, rv = 0;
    uint32 rval_status, rval_mask, fval_status, fval_mask;
    soc_reg_t int_status_reg = MMU_INTFI_DD_TIMER_INT_STATUSr,
                int_mask_reg = MMU_INTFI_DD_TIMER_INT_MASKr;
    soc_field_t status_field = PFC_PRIORITY_ISRf,
                mask_field = PFC_PRIORITY_INT_MASKf;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    _bcm_th3_pfc_deadlock_control_t *dd_control =
                                    _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    if(dd_control == NULL) {
        return BCM_E_INIT;
    }

    PFC_LOCK(unit);
    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);
    /* check interrupt status for each port */
    PBMP_PORT_ITER(unit, port) {
        port_config = &(dd_control->port_config[port]);
        if (port_config->enabled_priority == 0) {
            /* port is not enabled for PFC deadlock detection */

            continue;
        }
        rv = soc_reg32_get(unit, int_status_reg, port, 0, &rval_status);
        if (SOC_FAILURE(rv)) {
            PFC_UNLOCK(unit);
            return rv;
        }

        fval_status = soc_reg_field_get(unit,
                        int_status_reg, rval_status, status_field);
        rv = soc_reg32_get(unit, int_mask_reg, port, 0, &rval_mask);
        if (SOC_FAILURE(rv)) {
            PFC_UNLOCK(unit);
            return rv;
        }
        fval_mask = soc_reg_field_get(unit, int_mask_reg, rval_mask,
                            mask_field);
        fval_status = fval_status & (~fval_mask);
        if (fval_status != 0) {
            for (i = 0; i < TH3_PFC_PRIORITY_NUM; i++) {
                if (fval_status & (1U << i)) {
                    rv = _bcm_th3_pfc_deadlock_recovery_begin(unit, port, i);
                    if (BCM_FAILURE(rv)) {
                        PFC_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        }
        if (!soc_property_get(unit, spn_PFC_DEADLOCK_SEQ_CONTROL, 0) ||
            (pfc_deadlock_cb->pfc_deadlock_cb == NULL)) {
            /* Update the count for priority already in Deadlock state and Reset
             * those priority where recovery timer has expired.
             */
            for (i = 0; i < TH3_PFC_PRIORITY_NUM; i++) {
                if ((port_config->deadlock_priority & (1U << i)) != 0) {
                    rv = _bcm_th3_pfc_deadlock_recovery_update(unit, port, i);
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


/* Callback routine registered with DPC framework for,
 *  1) Monitoring Deadlock status
 *  2) Start/Stop Recovery process
 */
STATIC void
_th3_pfc_deadlock_cb(void* owner, void* time_as_ptr, void *unit_as_ptr,
                void *unused_1, void* unused_2)
{
    int callback_time = (int)(size_t)time_as_ptr;
    int unit = (int)(size_t)unit_as_ptr;
    _bcm_th3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;

    sal_dpc_cancel(INT_TO_PTR(&_th3_pfc_deadlock_cb));

    if (!th3_pfc_init[unit]) {
        sal_sem_give(th3_pfc_deinit_notify[unit]);
        return;
    }

    pfc_deadlock_control = _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    if (pfc_deadlock_control == NULL) {
        /* Should not happen, defensive check */
        return;
    }

    _bcm_th3_pfc_deadlock_monitor(unit);

    if (_BCM_TH3_PFC_DEADLOCK_CB_ENABLED(unit)) {
        sal_dpc_time(callback_time, &_th3_pfc_deadlock_cb, owner,
                     time_as_ptr, unit_as_ptr, 0, 0);
    }
    _BCM_TH3_PFC_DEADLOCK_CB_COUNT(unit)++;
}

STATIC int
_bcm_th3_pfc_deadlock_cb_update(int unit, int arg)
{
    int port, dd_enabled = 0;
    _bcm_th3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;

    pfc_deadlock_control = _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    if (arg == 0) {
        /* check if dpc routine should be stopped */
        PFC_LOCK(unit);
        PBMP_PORT_ITER(unit, port) {
            port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
            if (port_config->enabled_priority) {
                dd_enabled = 1;
                break;
            }
        }
        PFC_UNLOCK(unit);
        if (dd_enabled == 0) {
            sal_dpc_cancel(INT_TO_PTR(&_th3_pfc_deadlock_cb));
            pfc_deadlock_control->cb_enabled = FALSE;
        }
    } else {
        /* enable background thread if not already enabled*/
        if (pfc_deadlock_control->cb_enabled == FALSE) {
            pfc_deadlock_control->cb_enabled = TRUE;
            _th3_pfc_deadlock_cb(INT_TO_PTR(&_th3_pfc_deadlock_cb),
                            INT_TO_PTR(_BCM_TH3_PFC_DEADLOCK_CB_INTERVAL(unit)),
                            INT_TO_PTR(unit), 0, 0);
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th3_pfc_deadlock_enable_config
 * Purpose:
 *      Get/Set per- (port, PFC priority) PFC deadlock detection and
 *      recovery enable.
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) logical port number.
 *      pfc_priority - (IN) PFC priority.
 *      op          - (IN) Operation type.
 *      arg         - (IN/OUT) Config value.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcm_th3_pfc_deadlock_enable_config(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    _bcm_th3_pfc_deadlock_op_t op,
    _bcm_th3_pfc_deadlock_op_origin_t op_origin,
    int *arg)
{
    uint32 rval;
    soc_reg_t reg = MMU_INTFI_DD_TIMER_ENABLEr;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;

    if ((op == _BCM_TH3_PFC_DEADLOCK_OP_SET) &&  *arg != 0 && *arg != 1) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));

    if(op == _BCM_TH3_PFC_DEADLOCK_OP_GET) {
        *arg = (rval & (1U << pfc_priority)) > 0 ? 1 : 0;
    } else if (op == _BCM_TH3_PFC_DEADLOCK_OP_SET) {
        if(*arg == 0) {
            /* disable PFC deadlock detection and recovery */
            rval &= ~(1U << pfc_priority);
        } else {
            rval |= 1U << pfc_priority;
        }
        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, reg, port, 0, rval));

        if (op_origin == _BCM_TH3_PFC_DEADLOCK_OP_EXTERNAL) {
            PFC_LOCK(unit);
            port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
            if(*arg == 0) {
                port_config->enabled_priority &= ~(1U << pfc_priority);
                if (port_config->deadlock_priority & (1U << pfc_priority)) {
                    BCM_IF_ERROR_RETURN(
                            _bcm_th3_pfc_deadlock_recovery_update(unit, port,
                                pfc_priority));
                }
            } else {
                port_config->enabled_priority |= 1U << pfc_priority;
            }
            PFC_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(
                _bcm_th3_pfc_deadlock_cb_update(unit, *arg));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pfc_deadlock_timer_config_get(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int *arg)
{
    soc_reg_t reg = MMU_INTFI_DD_TIMER_CFGr;
    soc_field_t field = INVALIDf;
    uint64 rval64;
    int timer_tick, timer_gran;
    if(arg == NULL) {
        return BCM_E_PARAM;
    }
    switch (type) {
        case bcmCosqPFCDeadlockDetectionTimer:
            field = _timer_init_fields[pfc_priority];
            break;
        case bcmCosqPFCDeadlockRecoveryTimer:
            PFC_LOCK(unit);
            *arg = _BCM_TH3_PFC_DEADLOCK_RECOVERY_TIMER
                                    (unit, pfc_priority, port);
            PFC_UNLOCK(unit);
            break;
        case bcmCosqPFCDeadlockTimerGranularity:
            field = _timer_tick_fields[pfc_priority];
            break;
        default:
            return BCM_E_PARAM;
    }
    if(bcmCosqPFCDeadlockRecoveryTimer != type) {
        SOC_IF_ERROR_RETURN(
                    soc_reg64_get(unit, reg, port, 0, &rval64));
        *arg = soc_reg64_field32_get(unit, reg, rval64, field);
        if(bcmCosqPFCDeadlockDetectionTimer == type) {
            BCM_IF_ERROR_RETURN(_bcm_th3_pfc_deadlock_timer_config_get
                (unit, port, pfc_priority, bcmCosqPFCDeadlockTimerGranularity, &timer_tick));
            if(timer_tick < 0 || timer_tick >= bcmCosqPFCDeadlockTimerIntervalCount) {
                return BCM_E_INTERNAL;
            }
            timer_gran = _timer_tick_encoding[timer_tick];
            *arg *= timer_gran;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_pfc_deadlock_timer_config_set(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int arg)
{
    soc_reg_t reg = MMU_INTFI_DD_TIMER_CFGr;
    soc_field_t field;
    uint64 rval64;
    int timer_tick = 0, timer_gran = 0;
    switch (type) {
        case bcmCosqPFCDeadlockDetectionTimer:
            BCM_IF_ERROR_RETURN(_bcm_th3_pfc_deadlock_timer_config_get
                (unit, port, pfc_priority, bcmCosqPFCDeadlockTimerGranularity, &timer_tick));
            if(timer_tick < 0 || timer_tick >= bcmCosqPFCDeadlockTimerIntervalCount) {
                return BCM_E_INTERNAL;
            }
            timer_gran = _timer_tick_encoding[timer_tick];
            arg = arg / timer_gran;
            if ( arg < 0 ||  arg > _BCM_TH3_PFC_DEADLOCK_TIMER_MAX) {
                return BCM_E_PARAM;
            }
            field = _timer_init_fields[pfc_priority];
            break;
        case bcmCosqPFCDeadlockRecoveryTimer:
            if(arg < 0) {
                return BCM_E_PARAM;
            }
            PFC_LOCK(unit);
            _BCM_TH3_PFC_DEADLOCK_RECOVERY_TIMER
                               (unit, pfc_priority, port) = arg;
            PFC_UNLOCK(unit);
            break;
        case bcmCosqPFCDeadlockTimerGranularity:
            if(arg < bcmCosqPFCDeadlockTimerInterval1MiliSecond ||
                arg >= bcmCosqPFCDeadlockTimerIntervalCount) {
                return BCM_E_PARAM;
            }
            field = _timer_tick_fields[pfc_priority];
            break;
        default:
            return BCM_E_PARAM;
    }

    if(type != bcmCosqPFCDeadlockRecoveryTimer) {
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, field, arg);
        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, port, 0, rval64));
    }

    return BCM_E_NONE;
}

int
bcm_tomahawk3_cosq_pfc_deadlock_control_get(
    int unit, bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int *arg)
{
    if (!SOC_PORT_VALID(unit, port) || arg == NULL) {
        return BCM_E_PARAM;
    }
    if (pfc_priority < 0 || pfc_priority >= TH3_PFC_PRIORITY_NUM) {
        return BCM_E_PARAM;
    }

    if (type < 0 || type >= bcmCosqPFCDeadlockControlTypeCount) {
        return BCM_E_PARAM;
    }
    switch (type) {
        case bcmCosqPFCDeadlockDetectionAndRecoveryEnable:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_pfc_deadlock_enable_config(unit, port,
                        pfc_priority, _BCM_TH3_PFC_DEADLOCK_OP_GET,
                        _BCM_TH3_PFC_DEADLOCK_OP_EXTERNAL, arg));
            break;
        case bcmCosqPFCDeadlockDetectionTimer:
        case bcmCosqPFCDeadlockRecoveryTimer:
        case bcmCosqPFCDeadlockTimerGranularity:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_pfc_deadlock_timer_config_get(unit, port,
                        pfc_priority, type, arg));
            break;
        case bcmCosqPFCDeadlockRecoveryOccurrences:
            PFC_LOCK(unit);
            *arg = _BCM_TH3_PFC_DEADLOCK_RECOVERY_ACCUMULATED
                                    (unit, pfc_priority, port);
            *arg = *arg * (_BCM_TH3_PFC_DEADLOCK_CB_INTERVAL(unit));
            *arg = *arg / 1000; /* milli seconds */
            PFC_UNLOCK(unit);
            break;
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
bcm_tomahawk3_cosq_pfc_deadlock_control_set(
    int unit, bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int arg)
{
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PARAM;
    }
    if (pfc_priority < 0 || pfc_priority >= TH3_PFC_PRIORITY_NUM) {
        return BCM_E_PARAM;
    }
    if (type < 0 || type >= bcmCosqPFCDeadlockControlTypeCount) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmCosqPFCDeadlockDetectionAndRecoveryEnable:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_pfc_deadlock_enable_config(unit, port,
                        pfc_priority, _BCM_TH3_PFC_DEADLOCK_OP_SET,
                        _BCM_TH3_PFC_DEADLOCK_OP_EXTERNAL, &arg));
            break;
        case bcmCosqPFCDeadlockDetectionTimer:
        case bcmCosqPFCDeadlockRecoveryTimer:
        case bcmCosqPFCDeadlockTimerGranularity:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_pfc_deadlock_timer_config_set(unit, port,
                        pfc_priority, type, arg));
            break;
        case bcmCosqPFCDeadlockRecoveryOccurrences:
            if (arg == 0) {
                PFC_LOCK(unit);
                _BCM_TH3_PFC_DEADLOCK_RECOVERY_ACCUMULATED
                                  (unit, pfc_priority, port) = arg;
                PFC_UNLOCK(unit);
                break;
            } else {
                return BCM_E_PARAM;
            }
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_pfc_switch_control_set
 * Description:
 *      Set PFC Deadlock feature's switch config.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg  - Set value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      bcmSwitchPFCDeadlockCosMax - Set operation is not permitted
 */
int _bcm_th3_pfc_switch_control_set(int unit, bcm_switch_control_t type, int arg)
{
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 rval;
    if (bcmSwitchPFCDeadlockDetectionTimeInterval == type) {
        return BCM_E_UNAVAIL;
    }
    if (bcmSwitchPFCDeadlockRecoveryAction == type) {
        if(arg < 0 || arg > 1) {
            return BCM_E_PARAM;
        }
        reg = MMU_INTFI_IGNORE_PFC_XOFF_PKT_DISCARDr;
        field = PURGEf;
        rval = 0;
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, field, arg);
        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_pfc_switch_control_get
 * Description:
 *      Get PFC Deadlock feature's switch config.
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg  - Pointer to retrieved value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      bcmSwitchPFCDeadlockCosMax - Set operation is not permitted
 */
int _bcm_th3_pfc_switch_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 rval;

    if(arg == NULL) {
        return BCM_E_PARAM;
    }
    if (bcmSwitchPFCDeadlockDetectionTimeInterval == type) {
        return BCM_E_UNAVAIL;
    }
    if (bcmSwitchPFCDeadlockRecoveryAction == type) {

        reg = MMU_INTFI_IGNORE_PFC_XOFF_PKT_DISCARDr;
        field = PURGEf;
        rval = 0;
        SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        *arg = soc_reg_field_get(unit, reg, rval, field);

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_pfc_deadlock_info_get
 * Purpose:
 *     Get bitmap of Enabled(Admin) and Current Deadlock ports status for a
 *     given priority in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     priority         - (IN) PFC priority
 *     info             - (OUT) Info for a given priority
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_th3_pfc_deadlock_info_get(int unit, int priority,
                                bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info)
{
    int port;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    if(priority < 0 || priority >= TH3_PFC_PRIORITY_NUM) {
        return BCM_E_PARAM;
    }
    PBMP_ALL_ITER(unit, port) {
        port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
        if((port_config->enabled_priority & (1U << priority)) != 0) {
            BCM_PBMP_PORT_ADD(pfc_deadlock_info->enabled_pbmp, port);
        }
        if((port_config->deadlock_priority & (1U << priority)) != 0) {
            BCM_PBMP_PORT_ADD(pfc_deadlock_info->deadlock_pbmp, port);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_pfc_deadlock_queue_status_get
 * Purpose:
 *     Get the current Deadlock status for the given Port.
 * Parameters:
 *     unit             - (IN) unit number
 *     gport            - (IN) gport
 *     deadlock_status  - (OUT) Deatlock status for the given port
 *                              bit[0]->PFC priority 0 ...
 *                              bit[7] -> PFC priority 7
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_th3_pfc_deadlock_port_status_get
                       (int unit, bcm_gport_t gport, uint8 *deadlock_status)
{
    int i, local_port;
    uint8 deadlock_bmp = 0;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;

    if (deadlock_status == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if (!BCM_GPORT_IS_LOCAL(gport)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, (bcm_port_t *) &local_port));
    } else {
        local_port = gport;
    }

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PARAM;
    }
    PFC_LOCK(unit);
    port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, local_port);
    for(i = 0; i< TH3_PFC_PRIORITY_NUM; i++) {
        if((port_config->deadlock_priority & (1U << i)) != 0) {
            deadlock_bmp |= (1U << i);
        }
    }
    *deadlock_status = deadlock_bmp;
    PFC_UNLOCK(unit);
    return BCM_E_NONE;
}


int
bcm_tomahawk3_cosq_pfc_dd_recovery_mode_set(int unit, bcm_port_t port,
        bcm_cosq_pfc_dd_recovery_mode_t recovery_mode)
{
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PARAM;
    }
    PFC_LOCK(unit);
    port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
    if (recovery_mode == bcmCosqPfcDdRecoveryModeTimer) {
        port_config->deadlock_exit_mode = 0;
    } else if (recovery_mode == bcmCosqPfcDdRecoveryModePFCState) {
        port_config->deadlock_exit_mode = 1;
    } else {
        return BCM_E_PARAM;
    }
    PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

int
bcm_tomahawk3_cosq_pfc_dd_recovery_mode_get(int unit, bcm_port_t port,
        bcm_cosq_pfc_dd_recovery_mode_t *recovery_mode)
{
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;

    if (recovery_mode == NULL) {
        return BCM_E_PARAM;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PARAM;
    }
    PFC_LOCK(unit);
    port_config = _BCM_TH3_PFC_DEADLOCK_CONFIG(unit, port);
    if (port_config->deadlock_exit_mode == 0) {
        *recovery_mode = bcmCosqPfcDdRecoveryModeTimer;
    } else if (port_config->deadlock_exit_mode == 1) {
        *recovery_mode = bcmCosqPfcDdRecoveryModePFCState;
    }
    PFC_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_pfc_deadlock_callback_register
 * Purpose:
 *      Register pfc deadlock recovery callback in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     callback         - (IN) callback function
 *     userdata         - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */

int _bcm_th3_pfc_deadlock_recovery_event_register(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata)
{
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;

    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);

    pfc_deadlock_cb->pfc_deadlock_cb = callback;
    pfc_deadlock_cb->pfc_deadlock_userdata = userdata;

    return BCM_E_NONE;

}

/*
 * Function:
 *     _bcm_th3_pfc_deadlock_callback_unregister
 * Purpose:
 *      Unregister pfc deadlock recovery callback in PFC deadlock feature.
 * Parameters:
 *     unit             - (IN) unit number
 *     callback         - (IN) callback function
 *     userdata         - (IN) user data
 * Returns:
 *     BCM_E_XXX
 */

int _bcm_th3_pfc_deadlock_recovery_event_unregister(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata)
{
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;

    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);

    pfc_deadlock_cb->pfc_deadlock_cb = NULL;
    pfc_deadlock_cb->pfc_deadlock_userdata = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_pfc_deadlock_init
 * Purpose:
 *     Initialize (clear) all states/data-structures
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_pfc_deadlock_init(int unit)
{
    _bcm_th3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    int port;

    pfc_deadlock_control = _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    if (NULL == pfc_deadlock_control) {
        pfc_deadlock_control =
            sal_alloc(sizeof(_bcm_th3_pfc_deadlock_control_t), "pfc_deadlock_ctrl");
        if (!pfc_deadlock_control) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(pfc_deadlock_control, 0, sizeof(_bcm_th3_pfc_deadlock_control_t));
    _BCM_TH3_PFC_DEADLOCK_CONTROL(unit) = pfc_deadlock_control;

    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);

    if (NULL == pfc_deadlock_cb) {
        pfc_deadlock_cb =
            sal_alloc(sizeof(_bcm_pfc_deadlock_cb_t), "pfc_deadlock_cb");
        if (!pfc_deadlock_cb) {
            if (_BCM_TH3_PFC_DEADLOCK_CONTROL(unit) != NULL) {
                sal_free(_BCM_TH3_PFC_DEADLOCK_CONTROL(unit));
                _BCM_TH3_PFC_DEADLOCK_CONTROL(unit) = NULL;
            }
            return BCM_E_MEMORY;
        }
    }
    sal_memset(pfc_deadlock_cb, 0, sizeof(_bcm_pfc_deadlock_cb_t));
    pfc_deadlock_cb->pfc_deadlock_cb = NULL;
    pfc_deadlock_cb->pfc_deadlock_userdata = NULL;
    _BCM_TH3_PFC_DEADLOCK_CB(unit) = pfc_deadlock_cb;

    pfc_deadlock_control->cb_enabled = FALSE;
    pfc_deadlock_control->cb_interval = 100 * 1000; /* 100 ms */

    _bcm_th3_pfc_lock[unit] = sal_mutex_create("pfc lock");
    if (_bcm_th3_pfc_lock[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    if (th3_pfc_deinit_notify[unit] == NULL) {
        th3_pfc_deinit_notify[unit] = sal_sem_create("pfc deinit notify", sal_sem_BINARY, 0);
        if (th3_pfc_deinit_notify[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    PBMP_PORT_ITER(unit, port) {
        pfc_deadlock_control->port_config[port].deadlock_exit_mode = 0;
    }
    th3_pfc_init[unit] = TRUE;

    return BCM_E_NONE;
}

int _bcm_th3_pfc_deadlock_deinit(int unit)
{
    _bcm_th3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_pfc_deadlock_cb_t *pfc_deadlock_cb = NULL;
    uint8 cb_enable = FALSE;

    pfc_deadlock_control = _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    if (pfc_deadlock_control != NULL) {
        cb_enable = pfc_deadlock_control->cb_enabled;
    }
    if (th3_pfc_init[unit]) {
        th3_pfc_init[unit] = FALSE;
        if (cb_enable == TRUE) {
            sal_sem_take(th3_pfc_deinit_notify[unit], sal_sem_FOREVER);
        }
    }
    sal_dpc_cancel(INT_TO_PTR(&_th3_pfc_deadlock_cb));

    if (pfc_deadlock_control != NULL) {
        sal_free(pfc_deadlock_control);
        _BCM_TH3_PFC_DEADLOCK_CONTROL(unit) = NULL;
    }
    pfc_deadlock_cb = _BCM_TH3_PFC_DEADLOCK_CB(unit);
    if (pfc_deadlock_cb != NULL) {
        sal_free(pfc_deadlock_cb);
        _BCM_TH3_PFC_DEADLOCK_CB(unit) = NULL;
    }
    if (_bcm_th3_pfc_lock[unit]) {
        sal_mutex_destroy(_bcm_th3_pfc_lock[unit]);
        _bcm_th3_pfc_lock[unit] = NULL;
    }

    if (th3_pfc_deinit_notify[unit] != NULL) {
        sal_sem_destroy(th3_pfc_deinit_notify[unit]);
        th3_pfc_deinit_notify[unit] = NULL;
    }

    return BCM_E_NONE;
}

int _bcm_th3_pfc_deadlock_port_detach(int unit, bcm_port_t port)
{
    _bcm_th3_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;

    pfc_deadlock_control = _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);

    if (pfc_deadlock_control == NULL) {
        return BCM_E_INIT;
    }
    port_config = &pfc_deadlock_control->port_config[port];
    sal_memset(port_config->recovery_timers, 0,
                                sizeof(uint32) * TH3_PFC_PRIORITY_NUM);
    sal_memset(port_config->recovery_count, 0,
                                sizeof(uint32) * TH3_PFC_PRIORITY_NUM);
    sal_memset(port_config->recovery_count_acc, 0,
                                sizeof(uint32) * TH3_PFC_PRIORITY_NUM);
    port_config->deadlock_exit_mode = 0;
    port_config->deadlock_priority = 0;
    port_config->enabled_priority = 0;
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
/*
 * Function:
 *     _bcm_th3_pfc_deadlock_reinit
 * Purpose:
 *     Reinitialize all states/data-structures from HW(Level 1 warmboot)
 *     and scache.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_pfc_deadlock_reinit(int unit, uint8 **scache_ptr, uint16 recovered_ver)
{
    _bcm_th3_pfc_deadlock_control_t *dd_control =
                                    _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    int pri = 0,dd_enable;
    uint32 rval_mask, fval_mask;
    soc_reg_t int_mask_reg = MMU_INTFI_DD_TIMER_INT_MASKr;
    soc_field_t mask_field = PFC_PRIORITY_INT_MASKf;
    int port, max_num_ports = 0;
    uint16 *u16_scache_p;
    uint32 *u32_scache_p;

    u16_scache_p = (uint16 *)(*scache_ptr);

    max_num_ports = soc_scache_dictionary_entry_get(unit,
                                     ssden_SOC_MAX_NUM_PORTS,
                                     SOC_MAX_NUM_PORTS);

    *scache_ptr += (max_num_ports * 8 * sizeof(uint16));
    u32_scache_p = (uint32 *)(*scache_ptr);

    *scache_ptr += (max_num_ports * 8 * sizeof(uint32));

    if (dd_control == NULL) {
        return BCM_E_INIT;
    }

    for (port = 0; port <  max_num_ports; port++) {
        for (pri = 0; pri < TH3_PFC_PRIORITY_NUM; pri++) {
             dd_control->port_config[port].recovery_timers[pri] = *u16_scache_p++;
             dd_control->port_config[port].recovery_count_acc[pri] = *u32_scache_p++;
        }
    }

    if (recovered_ver >= BCM_WB_VERSION_1_1) {
        u16_scache_p = (uint16 *)(*scache_ptr);
        for (port = 0; port <  max_num_ports; port++) {
            dd_control->port_config[port].deadlock_exit_mode = *u16_scache_p++;
        }
        *scache_ptr += (max_num_ports * sizeof(uint16));
    }

    /* check if any ports under dead lock recovery. */
    PBMP_PORT_ITER(unit, port) {
        port_config = &(dd_control->port_config[port]);
        SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, int_mask_reg, port, 0, &rval_mask));
        fval_mask = soc_reg_field_get(unit, int_mask_reg, rval_mask,
                            mask_field);
        if (fval_mask != 0) {
            for (pri = 0; pri < TH3_PFC_PRIORITY_NUM; pri++) {
                if (fval_mask & (1U << pri)) {
                    port_config->deadlock_priority |= (1U << pri);
                }
            }
        }

        for (pri = 0; pri < TH3_PFC_PRIORITY_NUM; pri++) {
            if (bcm_tomahawk3_cosq_pfc_deadlock_control_get(unit, port, pri,
                                  bcmCosqPFCDeadlockDetectionAndRecoveryEnable,
                                  &dd_enable) == BCM_E_NONE) {
                port_config->enabled_priority |= (dd_enable << pri) ;
            }
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_pfc_switch_control_get(unit,
                               bcmSwitchPFCDeadlockRecoveryAction, &dd_enable));
    dd_control->recovery_action = dd_enable;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_pfc_deadlock_recovery_reset
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
_bcm_th3_pfc_deadlock_recovery_reset(int unit)
{
    int pri = 0, dd_enabled = 0;
    bcm_port_t port;

    _bcm_th3_pfc_deadlock_port_config_t *port_config = NULL;
    _bcm_th3_pfc_deadlock_control_t *dd_control =
                                    _BCM_TH3_PFC_DEADLOCK_CONTROL(unit);
    if (dd_control == NULL) {
        return BCM_E_INIT;
    }

    PBMP_PORT_ITER(unit, port) {
        port_config = &(dd_control->port_config[port]);
        for (pri = 0; pri < TH3_PFC_PRIORITY_NUM; pri++) {
            if ((port_config->deadlock_priority & (1U << pri)) != 0) {
                port_config->recovery_count[pri] = 1;
                BCM_IF_ERROR_RETURN(
                    _bcm_th3_pfc_deadlock_recovery_update(unit, port, pri));
            }
        }
    }
    PBMP_PORT_ITER(unit, port) {
        port_config = &(dd_control->port_config[port]);
        if ( port_config->enabled_priority != 0 ) {
             dd_enabled = 1;
             break;
        }
    }
    if (dd_enabled == 1) {
        /* enable background thread if not already enabled. */
        if (dd_control->cb_enabled == FALSE) {
            dd_control->cb_enabled = TRUE;
            _th3_pfc_deadlock_cb(INT_TO_PTR(&_th3_pfc_deadlock_cb),
                                 INT_TO_PTR(_BCM_TH3_PFC_DEADLOCK_CB_INTERVAL(unit)),
                                 INT_TO_PTR(unit), 0, 0);
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif

