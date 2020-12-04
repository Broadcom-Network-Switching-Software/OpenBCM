/*! \file bcmbd_fifodma.c
 *
 * FIFODMA driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <shr/shr_thread.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmbd/bcmbd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_FIFODMA

static fifodma_ctrl_t fifodma_ctrl[FIFODMA_DEV_NUM_MAX];

static void
fifodma_intr_handler(shr_thread_t tc, void *arg)
{
    fifodma_ctrl_t *ctrl = arg;
    fifodma_chan_t *fifo = NULL;
    bcmbd_fifodma_work_t *work = NULL;
    int chan;

    do {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }
        for (chan = 0; chan < ctrl->chan_num; chan++) {
            fifo = &ctrl->fifo[chan];
            sal_spinlock_lock(fifo->lock);
            if (fifo->intr_active && fifo->intr_enabled) {
                fifo->intr_active = 0;
                sal_spinlock_unlock(fifo->lock);
                ctrl->ops->fifodma_handle(ctrl, chan);
            } else {
                sal_spinlock_unlock(fifo->lock);
                continue;
            }
            work = &ctrl->work[chan];
            if (work->cb && fifo->intr_enabled) {
                work->cb(ctrl->unit, work->cb_data);
            }
        }
    } while (ctrl->active);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(ctrl->unit, "Fifodma handler thread exited.\n")));
}

static void
fifodma_intr_notify(int unit, int chan)
{
    fifodma_ctrl_t *ctrl = &fifodma_ctrl[unit];
    fifodma_chan_t *fifo = &ctrl->fifo[chan];

    sal_spinlock_lock(fifo->lock);
    fifo->intr_active = 1;
    sal_spinlock_unlock(fifo->lock);

    shr_thread_wake(ctrl->tc);
}

int
bcmbd_fifodma_attach(int unit)
{
    fifodma_ctrl_t *ctrl = NULL;
    fifodma_chan_t *fifo = NULL;
    int chan;

    SHR_FUNC_ENTER(unit);

    ctrl = bcmbd_fifodma_ctrl_get(unit);
    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(ctrl, 0, sizeof(*ctrl));
    ctrl->unit = unit;
    ctrl->chan_num = FIFODMA_CHAN_NUM_MAX;

    for (chan = 0; chan < ctrl->chan_num; chan++) {
        fifo = &ctrl->fifo[chan];
        fifo->lock = sal_spinlock_create("bcmbdFifoDmaIntrLock");
        if (!fifo->lock) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    ctrl->tc = shr_thread_start("bcmbdFifodma", FIFODMA_THREAD_PRI,
                                fifodma_intr_handler, (void *)ctrl);
    if (!ctrl->tc) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Could not start fifodma handler thread.\n")));
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    ctrl->intr_notify = fifodma_intr_notify;

    ctrl->active = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (ctrl) {
            for (chan = 0; chan < ctrl->chan_num; chan++) {
                fifo = &ctrl->fifo[chan];
                if (fifo->lock) {
                    sal_spinlock_destroy(fifo->lock);
                    fifo->lock = NULL;
                }
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmbd_fifodma_detach(int unit)
{
    fifodma_ctrl_t *ctrl = &fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;
    int chan;
    int rv;

    if (!ctrl->active) {
        return SHR_E_NONE;
    }

    ctrl->active = 0;

    rv = shr_thread_stop(ctrl->tc, 500000);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fifodma handler thread will not exit.\n")));
    }
    ctrl->tc = NULL;

    for (chan = 0; chan < ctrl->chan_num; chan++) {
        fifo = &ctrl->fifo[chan];
        if (fifo->lock) {
            sal_spinlock_destroy(fifo->lock);
            fifo->lock = NULL;
        }
    }

    return rv;
}

int
bcmbd_fifodma_start(int unit, int chan, bcmbd_fifodma_work_t *work)
{
    fifodma_ctrl_t *ctrl = &fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;
    int rv;

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->chan_num || !work) {
        return SHR_E_PARAM;
    }

    if (work->flags & FIFODMA_WF_INT_MODE && !work->cb) {
        return SHR_E_PARAM;
    }

    fifo = &ctrl->fifo[chan];
    if (fifo->started) {
        return SHR_E_RESOURCE;
    }

    fifo->num_entries = work->data->num_entries;
    fifo->data_width = work->data->data_width;
    fifo->buf_size = fifo->num_entries * fifo->data_width * sizeof(uint32_t);
    fifo->buf_cache = sal_alloc(fifo->buf_size, "bcmbdFifoDmaCache");
    if (!fifo->buf_cache) {
        return SHR_E_MEMORY;
    }
    fifo->buf_addr = bcmdrd_hal_dma_alloc(unit, fifo->buf_size, "bcmbdFifoDmaBuffer",
                                          &fifo->buf_paddr);
    if (!fifo->buf_addr) {
        sal_free(fifo->buf_cache);
        return SHR_E_MEMORY;
    }
    sal_memcpy(&ctrl->work[chan], work, sizeof(bcmbd_fifodma_work_t));

    rv = ctrl->ops->fifodma_start(ctrl, chan);
    if (SHR_FAILURE(rv)) {
        bcmdrd_hal_dma_free(unit, fifo->buf_size, fifo->buf_addr, fifo->buf_paddr);
        fifo->buf_addr = NULL;
        sal_free(fifo->buf_cache);
        fifo->buf_cache = NULL;
        return rv;
    }

    work->buf_cache = fifo->buf_cache;

    fifo->started = 1;

    return SHR_E_NONE;
}

int
bcmbd_fifodma_stop(int unit, int chan)
{
    fifodma_ctrl_t *ctrl = &fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;
    int rv;

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->chan_num) {
        return SHR_E_PARAM;
    }

    fifo = &ctrl->fifo[chan];
    if (!fifo->started) {
        return SHR_E_NONE;
    }

    rv = ctrl->ops->fifodma_stop(ctrl, chan);

    if (fifo->buf_addr) {
        bcmdrd_hal_dma_free(unit, fifo->buf_size, fifo->buf_addr, fifo->buf_paddr);
        fifo->buf_addr = NULL;
    }
    if (fifo->buf_cache) {
        sal_free(fifo->buf_cache);
        fifo->buf_cache = NULL;
    }

    fifo->started = 0;

    return rv;
}

int
bcmbd_fifodma_pop(int unit, int chan, uint32_t timeout, uint32_t *ptr, uint32_t *num)
{
    fifodma_ctrl_t *ctrl = &fifodma_ctrl[unit];
    fifodma_chan_t *fifo = NULL;

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->chan_num || !ptr || !num) {
        return SHR_E_PARAM;
    }

    fifo = &ctrl->fifo[chan];
    if (!fifo->started) {
        return SHR_E_UNAVAIL;
    }

    if (timeout) {
        sal_usleep(timeout);
    }

    return ctrl->ops->fifodma_pop(ctrl, chan, ptr, num);
}

fifodma_ctrl_t *
bcmbd_fifodma_ctrl_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &fifodma_ctrl[unit];
}
