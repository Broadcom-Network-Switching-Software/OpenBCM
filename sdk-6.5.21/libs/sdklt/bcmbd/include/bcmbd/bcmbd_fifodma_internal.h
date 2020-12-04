/*! \file bcmbd_fifodma_internal.h
 *
 * FIFODMA APIs and data types used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_FIFODMA_INTERNAL_H
#define BCMBD_FIFODMA_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_spinlock.h>
#include <shr/shr_thread.h>
#include <bcmbd/bcmbd_fifodma.h>

/*! Maximum number of FIFODMA channels */
#define FIFODMA_CHAN_NUM_MAX    12

/*! FIFODMA thread priority */
#define FIFODMA_THREAD_PRI      SAL_THREAD_PRIO_DEFAULT

/*!
 * \brief FIFODMA channel.
 */
typedef struct fifodma_chan_s {
    /*! Host buffer physical address */
    uint64_t buf_paddr;

    /*! Host buffer memory address */
    void *buf_addr;

    /*! Cache memory address */
    void *buf_cache;

    /*! Number of host buffer entries */
    uint32_t num_entries;

    /*! Entry width in words */
    uint32_t data_width;

    /*! Memory size in bytes */
    uint32_t buf_size;

    /*! Ready entry pointer */
    uint32_t ptr;

    /*! Interrupt spinlock */
    sal_spinlock_t lock;

    /*! Channel is started */
    int started;

    /*! Interrupt is enabled */
    int intr_enabled;

    /*! Interrupt is active */
    int intr_active;
} fifodma_chan_t;

struct fifodma_ctrl_s;

/*! Handle FIFODMA */
typedef void (*fifodma_handle_t)(struct fifodma_ctrl_s *ctrl, int chan);
/*! Start FIFODMA */
typedef int (*fifodma_start_t)(struct fifodma_ctrl_s *ctrl, int chan);
/*! Stop FIFODMA */
typedef int (*fifodma_stop_t)(struct fifodma_ctrl_s *ctrl, int chan);
/*! Pop FIFODMA */
typedef int (*fifodma_pop_t)(struct fifodma_ctrl_s *ctrl, int chan,
                             uint32_t *ptr, uint32_t *num);

/*!
 * \brief Exported operation functions.
 */
typedef struct fifodma_ops_s {
    /*! Handle FIFODMA */
    fifodma_handle_t    fifodma_handle;

    /*! Start FIFODMA */
    fifodma_start_t     fifodma_start;

    /*! Stop FIFODMA */
    fifodma_stop_t      fifodma_stop;

    /*! Pop FIFODMA */
    fifodma_pop_t       fifodma_pop;
} fifodma_ops_t;

/*! Notify FIFODMA interrupt */
typedef void (*fifodma_intr_notify_t)(int unit, int chan);

/*!
 * \brief FIFODMA control.
 */
typedef struct fifodma_ctrl_s {
    /*! Device number */
    int unit;

    /*! Channel number */
    int chan_num;

    /*! FIFOs */
    fifodma_chan_t fifo[FIFODMA_CHAN_NUM_MAX];

    /*! FIFODMA work */
    bcmbd_fifodma_work_t work[FIFODMA_CHAN_NUM_MAX];

    /*! Interrupt handler thread control */
    shr_thread_t tc;

    /*! Pointer to the exported funtions structure */
    fifodma_ops_t *ops;

    /*! Notify interrupt */
    fifodma_intr_notify_t intr_notify;

    /*! Device active indicator */
    int active;
} fifodma_ctrl_t;

#endif /* BCMBD_FIFODMA_INTERNAL_H */
