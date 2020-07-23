/*! \file bcmbd_ccmdma_internal.h
 *
 * CCMDMA APIs and data types used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CCMDMA_INTERNAL_H
#define BCMBD_CCMDMA_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <bcmbd/bcmbd_ccmdma.h>

/*! Maximum number of CCMDMA channel groups. */
#define DEV_GROUP_NUM_MAX       3
/*! Maximum number of CCMDMA channels. */
#define DEV_CHAN_NUM_MAX        6

struct ccmdma_ctrl_s;

/*! Execute CCMDMA work. */
typedef int (*ccmdma_work_execute_t)(struct ccmdma_ctrl_s *ctrl,
                                     bcmbd_ccmdma_work_t *work);

/*!
 * \brief Exported operation functions.
 */
typedef struct ccmdma_ops_s {
    /*! Execute CCMDMA work. */
    ccmdma_work_execute_t       work_execute;
} ccmdma_ops_t;

/*!
 * \brief CCMDMA device control.
 */
typedef struct ccmdma_ctrl_s {
    /*! Device number. */
    int unit;

    /*! Interrupt synchronous semaphores. */
    sal_sem_t intr[DEV_CHAN_NUM_MAX];

    /*! Groups bitmap. */
    uint32_t bm_grp;

    /*! Channels bitmap. */
    uint32_t bm_chan;

    /*! Pointer to the exported funtions structure. */
    ccmdma_ops_t *ops;

    /*! Pointer to device structure. */
    void *dev;

    /*! Indicate if device is attached. */
    int active;
} ccmdma_ctrl_t;

#endif /* BCMBD_CCMDMA_INTERNAL_H */
