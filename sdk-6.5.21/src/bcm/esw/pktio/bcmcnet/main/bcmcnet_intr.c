/*! \file bcmcnet_intr.c
 *
 * Utility routines for BCMCNET interrupt processing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcnet/bcmcnet_main.h>
#include <bcmcnet/bcmcnet_core.h>
#include "bcmcnet_intr.h"

static struct pdma_intr_ctrl intr_ctrl[NUM_PDMA_DEV_MAX];

/*!
 * Interrupt handling thread
 */
static void
bcmcnet_intr_process(struct shr_thread_ctrl_s *tc, void *arg)
{
    struct pdma_dev *dev = arg;
    struct pdma_intr_ctrl *intr = &intr_ctrl[dev->unit];
    struct pdma_intr_source *src = NULL;
    int qi;

    while (!intr->abort) {
        if (dev->flags & PDMA_VNET_DOCKED) {
            dev->xnet_wait(dev);
        } else {
            shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        }

        /* Handle all the active interrupts */
        do {
            for (qi = 0; qi < dev->num_queues; qi++) {
                src = &intr->intr_src[qi];

                sal_spinlock_lock(src->lock);
                intr->poll_active &= ~(1 << qi);
                if (src->active && src->enabled && src->intr_cb) {
                    intr->poll_active |= 1 << qi;
                    sal_spinlock_unlock(src->lock);

                    /* Call actual callback */
                    src->intr_cb(src->intr_hdl, src->intr_ck);
                } else {
                    sal_spinlock_unlock(src->lock);
                }
            }
        } while (intr->poll_active);
    }

    for (qi = 0; qi < dev->num_queues; qi++) {
        src = &intr->intr_src[qi];
        src->active = 0;
        src->enabled = 0;
    }

    intr->poll_active = 0;
}

/*!
 * Initialize interrupt handling
 */
int
bcmcnet_intr_ctrl_init(struct pdma_dev *dev)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[dev->unit];
    struct pdma_intr_source *src = NULL;
    int qi;

    for (qi = 0; qi < dev->num_queues; qi++) {
        src = &intr->intr_src[qi];
        src->lock = sal_spinlock_create("bcmcnetIntrSourceLock");
        if (!src->lock) {
            goto error;
        }
    }

    /* Create the helper thread to handle the interrupt */
    intr->abort = 0;
    intr->tc = shr_thread_start(dev->name, SAL_THREAD_PRIO_DEFAULT,
                                bcmcnet_intr_process, (void *)dev);
    if (!intr->tc) {
        CNET_PR("Could not start interrupt thread for device %d\n", dev->unit);
        goto error;
    }

    return SHR_E_NONE;

error:
    for (qi = 0; qi < dev->num_queues; qi++) {
        src = &intr->intr_src[qi];
        if (src->lock) {
            sal_spinlock_destroy(src->lock);
            src->lock = NULL;
        }
    }

    return SHR_E_MEMORY;
}

/*!
 * Clean up interrupt handling
 */
int
bcmcnet_intr_ctrl_cleanup(struct pdma_dev *dev)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[dev->unit];
    struct pdma_intr_source *src = NULL;
    int qi;
    int rv = SHR_E_NONE;

    if (intr->tc) {
        intr->abort = 1;
        rv = shr_thread_stop(intr->tc, 1000000);
        if (SHR_FAILURE(rv)) {
            CNET_PR("Interrupt thread will not exit\n");
        }
        intr->tc = NULL;
    }

    for (qi = 0; qi < dev->num_queues; qi++) {
        src = &intr->intr_src[qi];
        src->intr_hdl = NULL;
        src->intr_cb = NULL;
        src->intr_ck = NULL;
        if (src->lock) {
            sal_spinlock_destroy(src->lock);
            src->lock = NULL;
        }
    }

    return rv;
}

/*!
 * Register the callback for the specific interrupt handle
 */
int
bcmcnet_intr_cb_register(struct intr_handle *hdl, pdma_intr_cb_t cb, void *ck)
{
    struct pdma_intr_ctrl *intr = NULL;
    struct pdma_intr_source *src = NULL;

    if (!hdl || !ck) {
        return SHR_E_PARAM;
    }

    intr = &intr_ctrl[hdl->unit];
    src = &intr->intr_src[hdl->chan];

    if (src->intr_cb || src->intr_ck) {
        return SHR_E_PARAM;
    }

    src->intr_hdl = hdl;
    src->intr_cb = cb;
    src->intr_ck = ck;

    return SHR_E_NONE;
}

/*!
 * Unregister the callback according to the specified interrupt handle
 */
int
bcmcnet_intr_cb_unregister(struct intr_handle *hdl, pdma_intr_cb_t cb, void *ck)
{
    struct pdma_intr_ctrl *intr = NULL;
    struct pdma_intr_source *src = NULL;

    if (!hdl || !ck) {
        return SHR_E_PARAM;
    }

    intr = &intr_ctrl[hdl->unit];
    src = &intr->intr_src[hdl->chan];
    if (src->intr_cb == cb && (ck == (void *)-1 || ck == src->intr_ck)) {
        src->intr_hdl = NULL;
        src->intr_cb = NULL;
        src->intr_ck = NULL;
    }

    return SHR_E_NONE;
}

/*!
 * Enable the poll processing for the specified handle
 */
void
bcmcnet_poll_enable(struct intr_handle *hdl)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[hdl->unit];
    struct pdma_intr_source *src = &intr->intr_src[hdl->chan];

    sal_spinlock_lock(src->lock);
    src->enabled = 1;
    sal_spinlock_unlock(src->lock);
}

/*!
 * Disable the poll processing for the specified handle
 */
void
bcmcnet_poll_disable(struct intr_handle *hdl)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[hdl->unit];
    struct pdma_intr_source *src = &intr->intr_src[hdl->chan];

    sal_spinlock_lock(src->lock);
    src->enabled = 0;
    sal_spinlock_unlock(src->lock);
}

/*!
 * Schedule the poll processing for the specified handle
 */
void
bcmcnet_poll_schedule(struct intr_handle *hdl)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[hdl->unit];
    struct pdma_intr_source *src = &intr->intr_src[hdl->chan];

    sal_spinlock_lock(src->lock);
    src->active = 1;
    sal_spinlock_unlock(src->lock);

    shr_thread_wake(intr->tc);
}

/*!
 * Complete the poll processing for the specified handle
 */
void
bcmcnet_poll_complete(struct intr_handle *hdl)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[hdl->unit];
    struct pdma_intr_source *src = &intr->intr_src[hdl->chan];

    sal_spinlock_lock(src->lock);
    src->active = 0;
    sal_spinlock_unlock(src->lock);
}

/*!
 * Check the poll processing for the specified handle
 */
int
bcmcnet_poll_check(struct intr_handle *hdl)
{
    struct pdma_intr_ctrl *intr = &intr_ctrl[hdl->unit];
    struct pdma_intr_source *src = &intr->intr_src[hdl->chan];
    int polling;

    sal_spinlock_lock(src->lock);
    polling = !!(intr->poll_active & 1 << hdl->chan);
    sal_spinlock_unlock(src->lock);

    return polling;
}

