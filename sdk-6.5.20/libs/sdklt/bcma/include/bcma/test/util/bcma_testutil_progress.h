/*! \file bcma_testutil_progress.h
 *
 * BCMA test progress utility
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TEST_UTIL_PROGRESS_H
#define BCMA_TEST_UTIL_PROGRESS_H

#include <sal/sal_types.h>
#include <shr/shr_thread.h>
#include <sal/sal_spinlock.h>

/*! default interval (in sec) */
#define BCMA_TESTUTIL_PROGRESS_REPORT_INTERVAL     (30)

/*!
 * Test progress utility object.
 */
typedef struct bcma_testutil_progress_ctrl_s {
    /*! unit number */
    int unit;

    /*! total count in testing progress */
    uint64_t total_count;

    /*! current count in testing progress */
    uint64_t cur_count;

    /*! interval (in sec) between each progress report */
    uint32_t report_interval;

    /*! thread of progress report */
    shr_thread_t thread_pid;

    /*! lock of progress report */
    sal_spinlock_t lock;
} bcma_testutil_progress_ctrl_t;

/*!
 * \brief Start the progress.
 *
 * This function starts the progress report.
 *
 * \param [in] unit Unit number.
 * \param [in] total_count Total count in testing progress.
 * \param [in] report_interval Interval (in sec) between each progress report.
 *
 * \return Progress object or NULL on error.
 */
extern bcma_testutil_progress_ctrl_t *
bcma_testutil_progress_start(int unit, uint32_t total_count,
                             uint32_t report_interval);

/*!
 * \brief Update the progress counter.
 *
 * This function updates the progress counter.
 *
 * \param [in] pc Progress object returned by \ref bcma_testutil_progress_start.
 * \param [in] cur_count Current count in testing progress.
 *
 * \return SHR_E_NONE No errors, others on failure.
 */
extern int
bcma_testutil_progress_update(bcma_testutil_progress_ctrl_t *pc,
                              uint32_t cur_count);

/*!
 * \brief The progress done.
 *
 * This function hints the progress done.
 *
 * \param [in] pc Progress object returned by \ref bcma_testutil_progress_start.
 *
 * \return SHR_E_NONE No errors, others on failure.
 */
extern int
bcma_testutil_progress_done(bcma_testutil_progress_ctrl_t *pc);

#endif /* BCMA_TEST_UTIL_PROGRESS_H */
