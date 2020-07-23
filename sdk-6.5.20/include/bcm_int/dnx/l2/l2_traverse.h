/**
 * \file bcm_int/dnx/l2/l2_traverse.h
 * Internal DNX L2 APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef L2_TRAVERSE_H_INCLUDED
/*
 * {
 */
#define L2_TRAVERSE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <sal/core/sync.h>
#include <sal/core/thread.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/**
 * \brief
 *   DB holding information for running flush traverse in non-blocking mode.
 */
typedef struct l2_flush_traverse_non_blocking_s
{
    /** Thread control */
    sal_sem_t l2_traverse_sem;  /* Semaphore for l2 flush traverse in non-blocking mode */
    sal_thread_t l2_traverse_tid;       /* l2 flush traverse thread id */
    volatile int thread_running;        /* Input signal to thread */
    volatile int thread_exit_complete;  /* Output signal from thread */
    uint32 table_id;            /* The MACT table id (FWD_MACT) */

    /** User callback parameters */
    bcm_l2_traverse_cb trav_fn; /* User callback function */
    void *user_data;            /* User callback function parameters */
} l2_flush_traverse_non_blocking_t;

/*
 * }
 */

/*
 * Internal functions.
 * {
 */

/**
 * \brief - Init the DMA channel of the flush machine
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_flush_dma_init(
    int unit);

/**
 * \brief - Stop and free the dma channel of the flush
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_flush_dma_deinit(
    int unit);

/**
 * \brief - Get the state of the flush machine.
 *
 * \param [in] unit - unit id
 * \param [out] flush_machine_is_done - 1 in case the flush is idle. o otherwise.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_traverse_commit_done_get(
    int unit,
    int32 *flush_machine_is_done);

/**
 * \brief - Create L2 Flush Traverse Thread (for non-blocking mode).
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_traverse_thread_start(
    int unit);

/**
 * \brief - Destroy L2 Flush Traverse Thread (for non-blocking mode).
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_traverse_thread_stop(
    int unit);

/*
 * }
 */
#endif /* L2_TRAVERSE_H_INCLUDED */
