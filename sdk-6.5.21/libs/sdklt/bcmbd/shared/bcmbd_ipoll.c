/*! \file bcmbd_ipoll.c
 *
 * Software interrupt driver API.
 *
 * This API can be used when multiple software interrupts are tied to
 * a single hardware interrupt.
 *
 * Each software interrupt client will register a handler for a given
 * software interrupt source.
 *
 * For each software interrupt source, the hardware interrupt handler
 * will clear the interrupt condition and call the registered software
 * interrupt handler.
 *
 * Note that the enable/disable functions are typically not supported
 * in hardware, so calling them may have no effect.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_mutex.h>
#include <sal/sal_time.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_hal.h>

#include <bcmbd/bcmbd_internal.h>
#include <bcmbd/bcmbd_ipoll.h>

#define  BSL_LOG_MODULE  BSL_LS_BCMBD_INTR

/*******************************************************************************
 * Local definitions
 */

#define THREAD_STOP_USECS       (5 * SECOND_USEC)

typedef struct ipoll_ctrl_s {

    /*! Unit number. */
    int unit;

    /*! True if polled IRQ mode is enabled. */
    bool enable;

    /*! Delay between calls to the interrupt handler. */
    sal_usecs_t delay;

    /*! Priority of interrupt handler thread. */
    int thread_prio;

    /*! ISR thread control. */
    shr_thread_t thread_ctrl;

    /*! ISR thread lock. */
    sal_mutex_t intr_lock;

    /*! Interrupt handler provided by the SDK. */
    bcmdrd_hal_isr_func_f isr_func;

    /*! Context for the interrupt handler. */
    void *isr_data;

} ipoll_ctrl_t;

/*******************************************************************************
 * Local data
 */

static ipoll_ctrl_t ipoll_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Local functions
 */

static void
intr_thread(shr_thread_t tc, void *data)
{
    ipoll_ctrl_t *ic = (ipoll_ctrl_t *)data;

    while (1) {
        if (shr_thread_stopping(tc)) {
            break;
        }
        if (ic->isr_func) {
            /* Run ISR with sync lock held */
            sal_mutex_take(ic->intr_lock, SAL_MUTEX_FOREVER);
            ic->isr_func(ic->isr_data);
            sal_mutex_give(ic->intr_lock);
        }
        shr_thread_sleep(tc, 100);
    }
}

static int
intr_connect(void *devh, int irq_num,
             bcmdrd_hal_isr_func_f isr_func, void *isr_data)
{
    ipoll_ctrl_t *ic = (ipoll_ctrl_t *)devh;

    ic->intr_lock = sal_mutex_create("bcmbdIpollLock");
    if (ic->intr_lock == NULL) {
        return SHR_E_MEMORY;
    }
    ic->isr_func = isr_func;
    ic->isr_data = isr_data;
    ic->thread_ctrl = shr_thread_start("bcmbdPolledIrq",
                                       ic->thread_prio,
                                       intr_thread, ic);

    return SHR_E_NONE;
}

static int
intr_disconnect(void *devh, int irq_num)
{
    int rv;
    ipoll_ctrl_t *ic = (ipoll_ctrl_t *)devh;

    rv = shr_thread_stop(ic->thread_ctrl, THREAD_STOP_USECS);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    if (ic->intr_lock) {
        sal_mutex_destroy(ic->intr_lock);
        ic->intr_lock = NULL;
    }
    ic->isr_func = NULL;
    ic->isr_data = NULL;

    return SHR_E_NONE;
}

static int
intr_sync(void *devh, int irq_num,
          bcmdrd_intr_sync_cb_f cb, void *data)
{
    int rv;
    ipoll_ctrl_t *ic = (ipoll_ctrl_t *)devh;

    if (!cb) {
        /* Silently ignore */
        return SHR_E_NONE;
    }

    sal_mutex_take(ic->intr_lock, SAL_MUTEX_FOREVER);

    /* Perform callback */
    rv = cb(data);

    sal_mutex_give(ic->intr_lock);

    return rv;
}

static int
intr_configure(void *devh, int irq_num, bcmdrd_hal_intr_info_t *intr_info)
{
    return SHR_E_NONE;
}

static int
intr_mask_write(void *devh, int irq_num, uint32_t offs, uint32_t val)
{
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_ipoll_init(int unit)
{
    int rv;
    bcmdrd_hal_intr_t intr;
    ipoll_ctrl_t *ic;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    ic = &ipoll_ctrl[unit];
    ic->unit = unit;
    ic->enable = false;

    if (bcmbd_config_get(unit, 0,
                         "DEVICE_CONFIG",
                         "POLLED_IRQ_ENABLE") == 0) {
        /* Polled IRQ mode not enabled */
        return SHR_E_NONE;
    }
    ic->enable = true;
    ic->delay = bcmbd_config_get(unit, 0,
                                 "DEVICE_CONFIG",
                                 "POLLED_IRQ_DELAY");
    ic->thread_prio = bcmbd_config_get(unit, 0,
                                       "DEVICE_CONFIG",
                                       "POLLED_IRQ_THREAD_PRIORITY");

    /* Initialize interrupt structure */
    sal_memset(&intr, 0, sizeof(intr));

    /* Callback context is ipoll control */
    intr.devh = ic;

    /*! Connect/disconnect interrupt handler. */
    intr.intr_connect = intr_connect;
    intr.intr_disconnect = intr_disconnect;
    intr.intr_sync = intr_sync;
    intr.intr_configure = intr_configure;
    intr.intr_mask_write = intr_mask_write;

    rv = bcmdrd_dev_hal_intr_init(unit, &intr);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to initialize polled IRQ\n")));
        return rv;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Enabling polled IRQ every %lu usecs (prio: %d)\n"),
              (unsigned long)ic->delay, ic->thread_prio));

    return SHR_E_NONE;
}

bool
bcmbd_ipoll_enabled(int unit)
{
    if (BCMDRD_UNIT_VALID(unit)) {
        return ipoll_ctrl[unit].enable;
    }

    return false;;
}
