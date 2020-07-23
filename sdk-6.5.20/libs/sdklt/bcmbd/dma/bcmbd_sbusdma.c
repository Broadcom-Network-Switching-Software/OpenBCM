/*! \file bcmbd_sbusdma.c
 *
 * SBUSDMA driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <sal/sal_spinlock.h>
#include <shr/shr_thread.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_SBUSDMA

static sbusdma_ctrl_t sbusdma_ctrl[SBUSDMA_DEV_NUM_MAX];

inline static int
sbusdma_workqueue_init(sbusdma_workqueue_t *queue)
{
    queue->head = queue->rear = NULL;
    queue->count = 0;
    queue->lock = sal_spinlock_create("bcmbdSbusDmaWorkqueueLock");
    if (!queue->lock) {
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

inline static int
sbusdma_workqueue_cleanup(sbusdma_workqueue_t *queue)
{
    sal_spinlock_lock(queue->lock);
    queue->head = queue->rear = NULL;
    queue->count = 0;
    sal_spinlock_unlock(queue->lock);

    sal_spinlock_destroy(queue->lock);
    queue->lock = NULL;

    return SHR_E_NONE;
}

inline static int
sbusdma_workqueue_is_empty(sbusdma_workqueue_t *queue)
{
    return queue->head == NULL;
}

inline static int
sbusdma_work_enqueue(sbusdma_workqueue_t *queue, bcmbd_sbusdma_work_t *work)
{
    sal_spinlock_lock(queue->lock);
    if (!queue->head) {
        queue->head = work;
    } else {
        ((bcmbd_sbusdma_work_t *)queue->rear)->link = work;
    }
    queue->rear = work;
    queue->count++;
    sal_spinlock_unlock(queue->lock);

    return SHR_E_NONE;
}

inline static int
sbusdma_work_dequeue(sbusdma_workqueue_t *queue, bcmbd_sbusdma_work_t **work)
{
    sal_spinlock_lock(queue->lock);
    *work = (bcmbd_sbusdma_work_t *)queue->head;
    if (*work) {
        queue->head = (*work)->link;
        (*work)->link = NULL;
        queue->count--;
    }
    sal_spinlock_unlock(queue->lock);

    return SHR_E_NONE;
}

static void
sbusdma_work_process(shr_thread_t tc, void *arg)
{
    sbusdma_ctrl_t *ctrl = arg;
    int qn = ctrl->qn;
    bcmbd_sbusdma_work_t *work = NULL;

    ctrl->qn = -1;
    do {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        while (!sbusdma_workqueue_is_empty(&ctrl->queue[qn])) {
            sbusdma_work_dequeue(&ctrl->queue[qn], &work);
            if (!work) {
                break;
            }
            work->state = SBUSDMA_WORK_SCHED;
            if (work->pc) {
                work->pc(ctrl->unit, work->pc_data);
            }
            ctrl->ops->work_execute(ctrl, work);
            if (work->cb) {
                work->cb(ctrl->unit, work->cb_data);
            }
        }
    } while (ctrl->active);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(ctrl->unit, "Sbusdma work process thread exited.\n")));
}

int
bcmbd_sbusdma_attach(int unit)
{
    sbusdma_ctrl_t *ctrl = NULL;
    char name[32];
    int i;

    SHR_FUNC_ENTER(unit);

    ctrl = bcmbd_sbusdma_ctrl_get(unit);
    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(ctrl, 0, sizeof(*ctrl));
    ctrl->unit = unit;
    ctrl->qn = -1;

    for (i = 0; i < SBUSDMA_WORK_QUE_NUM; i++) {
        sbusdma_workqueue_init(&ctrl->queue[i]);

        sal_snprintf(name, sizeof(name), "%s%d", "bcmbdSbusdma", i);
        while (ctrl->qn != -1) {
            sal_usleep(1000);
        }
        ctrl->qn = i;
        ctrl->tc[i] = shr_thread_start(name, SBUSDMA_THREAD_PRI,
                                       sbusdma_work_process, (void *)ctrl);
        if (!ctrl->tc[i]) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Could not start sbusdma work process thread.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    for (i = 0; i < DEV_QUEUE_NUM_MAX; i++) {
        ctrl->intr[i] = sal_sem_create("bcmbdSbusDmaIntrSem", SAL_SEM_BINARY, 0);
        if (!ctrl->intr[i]) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    ctrl->active = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (ctrl) {
            for (i = 0; i < DEV_QUEUE_NUM_MAX; i++) {
                if (ctrl->intr[i]) {
                    sal_sem_destroy(ctrl->intr[i]);
                    ctrl->intr[i] = NULL;
                }
            }
            for (i = 0; i < SBUSDMA_WORK_QUE_NUM; i++) {
                if (ctrl->tc[i]) {
                    if (SHR_FAILURE(shr_thread_stop(ctrl->tc[i], 500000))) {
                        SHR_IF_ERR_CONT(SHR_E_TIMEOUT);
                    }
                    ctrl->tc[i] = NULL;
                }
                sbusdma_workqueue_cleanup(&ctrl->queue[i]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmbd_sbusdma_detach(int unit)
{
    sbusdma_ctrl_t *ctrl = &sbusdma_ctrl[unit];
    bcmbd_sbusdma_work_t *work = NULL;
    int i;
    int rv = SHR_E_NONE;

    if (!ctrl->active) {
        return SHR_E_NONE;
    }

    ctrl->active = 0;

    for (i = 0; i < DEV_QUEUE_NUM_MAX; i++) {
        if (ctrl->intr[i]) {
            sal_sem_destroy(ctrl->intr[i]);
            ctrl->intr[i] = NULL;
        }
    }

    for (i = 0; i < SBUSDMA_WORK_QUE_NUM; i++) {
        if (SHR_FAILURE(shr_thread_stop(ctrl->tc[i], 500000))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Sbusdma work process thread will not exit.\n")));
            rv = SHR_E_TIMEOUT;
        }
        ctrl->tc[i] = NULL;
        while (1) {
            sbusdma_work_dequeue(&ctrl->queue[i], &work);
            if (!work) {
                break;
            } else {
                work->state = SBUSDMA_WORK_CREATED;
                ctrl->ops->batch_work_delete(ctrl, work);
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Should not have pended work at this moment.\n")));
            }
        }
        sbusdma_workqueue_cleanup(&ctrl->queue[i]);
    }

    return rv;
}

int
bcmbd_sbusdma_light_work_init(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = &sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work) {
        return SHR_E_PARAM;
    }

    return ctrl->ops->light_work_init(ctrl, work);
}

int
bcmbd_sbusdma_batch_work_create(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = &sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work || !work->data) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_NULL &&
        work->state != SBUSDMA_WORK_DESTROYED) {
        return SHR_E_PARAM;
    }

    return ctrl->ops->batch_work_create(ctrl, work);
}

int
bcmbd_sbusdma_batch_work_delete(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = &sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work) {
        return SHR_E_PARAM;
    }

    if (work->state == SBUSDMA_WORK_NULL ||
        work->state == SBUSDMA_WORK_DESTROYED) {
        return SHR_E_NONE;
    }

    return ctrl->ops->batch_work_delete(ctrl, work);
}

int
bcmbd_sbusdma_work_execute(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = &sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work || !work->data) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_INITED &&
        work->state != SBUSDMA_WORK_CREATED &&
        work->state < SBUSDMA_WORK_DONE) {
        return SHR_E_PARAM;
    }

    return ctrl->ops->work_execute(ctrl, work);
}

int
bcmbd_sbusdma_work_add(int unit, bcmbd_sbusdma_work_t *work)
{
    sbusdma_ctrl_t *ctrl = &sbusdma_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!work || !work->data) {
        return SHR_E_PARAM;
    }

    if (work->state != SBUSDMA_WORK_INITED &&
        work->state != SBUSDMA_WORK_CREATED &&
        work->state < SBUSDMA_WORK_DONE) {
        return SHR_E_PARAM;
    }

    work->state = SBUSDMA_WORK_QUEUED;

    if ((uint32_t)work->queue >= SBUSDMA_WORK_QUE_NUM) {
        return SHR_E_PARAM;
    }
    sbusdma_work_enqueue(&ctrl->queue[work->queue], work);
    shr_thread_wake(ctrl->tc[work->queue]);

    return SHR_E_NONE;
}

sbusdma_ctrl_t *
bcmbd_sbusdma_ctrl_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &sbusdma_ctrl[unit];
}
