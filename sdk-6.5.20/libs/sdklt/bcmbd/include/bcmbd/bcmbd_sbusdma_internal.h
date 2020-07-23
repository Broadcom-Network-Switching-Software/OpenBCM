/*! \file bcmbd_sbusdma_internal.h
 *
 * SBUSDMA APIs and data types used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SBUSDMA_INTERNAL_H
#define BCMBD_SBUSDMA_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <sal/sal_spinlock.h>
#include <shr/shr_thread.h>
#include <bcmbd/bcmbd_sbusdma.h>

/*! Maximum number of SBUSDMA device groups. */
#define DEV_GROUP_NUM_MAX       3
/*! Maximum number of SBUSDMA queues. */
#define DEV_QUEUE_NUM_MAX       16

/*! SBUSDMA thread priority. */
#define SBUSDMA_THREAD_PRI      SAL_THREAD_PRIO_HIGH

/*!
 * \brief SBUSDMA work queue.
 */
typedef struct sbusdma_workqueue_s {
    /*! Queue head. */
    void *head;

    /*! Queue rear. */
    void *rear;

    /*! Queued work count. */
    uint32_t count;

    /*! Queue lock. */
    sal_spinlock_t lock;
} sbusdma_workqueue_t;

struct sbusdma_ctrl_s;

/*! Initialize light work. */
typedef int (*sbusdma_light_work_init_t)(struct sbusdma_ctrl_s *ctrl, bcmbd_sbusdma_work_t *work);
/*! Create batch work. */
typedef int (*sbusdma_batch_work_create_t)(struct sbusdma_ctrl_s *ctrl, bcmbd_sbusdma_work_t *work);
/*! Delete batch work. */
typedef int (*sbusdma_batch_work_delete_t)(struct sbusdma_ctrl_s *ctrl, bcmbd_sbusdma_work_t *work);
/*! Do light or batch work. */
typedef int (*sbusdma_work_execute_t)(struct sbusdma_ctrl_s *ctrl, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Exported operation functions.
 */
typedef struct sbusdma_ops_s {
    /*! Initialize light work. */
    sbusdma_light_work_init_t   light_work_init;

    /*! Create batch work. */
    sbusdma_batch_work_create_t batch_work_create;

    /*! Delete batch work. */
    sbusdma_batch_work_delete_t batch_work_delete;

    /*! Do light or batch work. */
    sbusdma_work_execute_t      work_execute;
} sbusdma_ops_t;

/*!
 * \brief SBUSDMA device control.
 */
typedef struct sbusdma_ctrl_s {
    /*! Device number. */
    int unit;

    /*! Work queue. */
    sbusdma_workqueue_t queue[SBUSDMA_WORK_QUE_NUM];

    /*! Interrupt synchronous semaphore per queue. */
    sal_sem_t intr[DEV_QUEUE_NUM_MAX];

    /*! Work schedule thread control. */
    shr_thread_t tc[SBUSDMA_WORK_QUE_NUM];

    /*! Work queue number. */
    int qn;

    /*! Groups bitmap. */
    uint32_t bm_grp;

    /*! Queues bitmap. */
    uint32_t bm_que;

    /*! Pointer to the exported funtions structure. */
    sbusdma_ops_t *ops;

    /*! Device active indicator. */
    int active;
} sbusdma_ctrl_t;

#endif /* BCMBD_SBUSDMA_INTERNAL_H */
