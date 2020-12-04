/*! \file bcma_testutil_progress.c
 *
 * Progress Report utility
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_time.h>
#include <shr/shr_debug.h>
#include <bcma/test/util/bcma_testutil_progress.h>

#define BSL_LOG_MODULE                 BSL_LS_APPL_TEST
#define PROGRESS_THREAD_STOP_WAITTIME  500000

static void
bcma_testutil_progress_thread(shr_thread_t tc, void *arg)
{
    bcma_testutil_progress_ctrl_t *pc = (bcma_testutil_progress_ctrl_t *)arg;

    if (pc == NULL) {
        return;
    }

    while (1) {
        shr_thread_sleep(tc, pc->report_interval * SECOND_USEC);
        if (shr_thread_stopping(tc)) {
            break;
        }

        sal_spinlock_lock(pc->lock);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(pc->unit,
                             "running... (%"PRIu64".%02"PRIu64" %%)\n"),
                             (pc->cur_count  * 10000 / pc->total_count) / 100,
                             (pc->cur_count  * 10000 / pc->total_count) % 100));
        sal_spinlock_unlock(pc->lock);
   }
}

bcma_testutil_progress_ctrl_t *
bcma_testutil_progress_start(int unit, uint32_t total_count,
                             uint32_t report_interval)
{
    bcma_testutil_progress_ctrl_t *pc = NULL;

    pc = sal_alloc(sizeof(bcma_testutil_progress_ctrl_t),
                   "bcmaTestutilProgress");
    if (pc == NULL) {
        return NULL;
    }
    sal_memset(pc, 0, sizeof(bcma_testutil_progress_ctrl_t));

    pc->unit = unit;
    pc->total_count = total_count;
    pc->cur_count = 0;
    pc->report_interval = report_interval;

    pc->lock = sal_spinlock_create("bcmatestutilprogresslock");
    if (pc->lock == NULL) {
        sal_free(pc);
        return NULL;
    }

    pc->thread_pid = shr_thread_start("bcmatestutilprogressthread",
                                      SAL_THREAD_PRIO_DEFAULT,
                                      bcma_testutil_progress_thread,
                                      (void *)pc);
    if (pc->thread_pid == NULL) {
        sal_spinlock_destroy(pc->lock);
        sal_free(pc);
        return NULL;
    }

    return pc;
}

int
bcma_testutil_progress_update(bcma_testutil_progress_ctrl_t *pc,
                              uint32_t cur_count)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(pc, SHR_E_PARAM);

    sal_spinlock_lock(pc->lock);
    pc->cur_count = cur_count;
    sal_spinlock_unlock(pc->lock);

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_progress_done(bcma_testutil_progress_ctrl_t *pc)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(pc, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (shr_thread_stop(pc->thread_pid, PROGRESS_THREAD_STOP_WAITTIME));
    pc->thread_pid = NULL;

    sal_spinlock_destroy(pc->lock);
    pc->lock = NULL;

    SHR_FREE(pc);
exit:
    SHR_FUNC_EXIT();
}
