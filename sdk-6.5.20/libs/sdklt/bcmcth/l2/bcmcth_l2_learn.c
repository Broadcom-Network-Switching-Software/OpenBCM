/*! \file bcmcth_l2_drv.c
 *
 * BCMCTH L2 driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_ha.h>
#include <sal/sal_libc.h>
#include <sal/sal_msgq.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_conf.h>

#include <bcmcth/bcmcth_learn_drv.h>
#include <bcmcth/bcmcth_l2.h>

/******************************************************************************
* Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L2

#define L2_LEARN_INTERVAL_NORMAL            200000
#define L2_LEARN_INTERVAL_AGRRESIVE         100
/* Count of poll times during which no new entry is added to learn cache */
#define L2_LEARN_THRESHOLD_NO_NEW_ENTRY     36
/* Count of interval during which learn cache keeps empty */
#define L2_LEARN_THRESHOLD_CACHE_EMPTY      10

/*! Device specific attach function type. */
typedef bcmcth_learn_info_drv_t *(*bcmcth_learn_cache_info_drv_get_f)(int unit);

/*! Learn Cache info table drive get. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_cth_learn_cache_data_info_drv_get},
static struct {
    bcmcth_learn_cache_info_drv_get_f drv_get;
} learn_cache_data_info_drv_get[] = {
    { 0 },
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }
};


/******************************************************************************
* Private functions
 */
static void
l2_learn_thread(shr_thread_t tc, void *arg)
{
    int unit = (uintptr_t)arg;
    int rv = SHR_E_NONE;
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_ctrl_t *learn_ctrl;
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;
    sal_usecs_t interval;
    l2_trav_status_t status;
    int no_new_learn = 0, empty_count = 0;
    bool sim = bcmdrd_feature_is_sim(unit);

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    report_ctrl = drv->report_ctrl;
    if (learn_ctrl == NULL || report_ctrl == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "L2 learn ctrl null, thread exit.\n")));
        return;
    }

    /*
     * L2 learn h/w interrupt does not provide info for which new entry is
     * learnt, s/w has to poll the cache.
     * The interrupt is used as an indicator to start the polling process,
     * when h/w learn disabled or interrupt disabled, learn thread will sleep
     * infinitely after the last round of traverse done.
     */
    do {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "L2 thread poll interval %d usecs.\n"),
                              learn_ctrl->interval));

        shr_thread_sleep(tc, learn_ctrl->interval);
        if (shr_thread_stopping(tc)) {
            break;
        }

        /* Traverse the HW cache and report the learned MAC addresses. */
        sal_memset(&status, 0, sizeof(l2_trav_status_t));
        rv = bcmcth_l2_drv_cache_traverse(unit, &status);
        if (rv != SHR_E_NONE) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Traverse cache error.\n")));
        }
        /* No intr on simulator, always polling at initial interval. */
        if (sim) {
            continue;
        }
        /* Calculate new interval per cache status. */
        if (!status.cache_empty
            || empty_count < L2_LEARN_THRESHOLD_CACHE_EMPTY) {
            if (report_ctrl->slow_poll == 0) {
                if (status.new_learn) {
                    no_new_learn = 0;
                    interval = L2_LEARN_INTERVAL_AGRRESIVE;
                } else {
                    if (status.cache_empty == 0) {
                        empty_count = 0;
                    } else {
                        empty_count++;
                    }

                    if (no_new_learn < L2_LEARN_THRESHOLD_NO_NEW_ENTRY) {
                        /* Increase interval if no new entry in cache. */
                        no_new_learn++;
                        interval = no_new_learn / 4 + 1;
                        interval *= L2_LEARN_INTERVAL_AGRRESIVE;
                    } else {
                        /*
                         * Return to normal interval. Note: cache still
                         * can be full if no entry deleted.
                         */
                        interval = L2_LEARN_INTERVAL_NORMAL;
                    }
                }
            } else {
                if (status.cache_empty == 0) {
                    empty_count = 0;
                } else {
                    empty_count++;
                }
                interval = L2_LEARN_INTERVAL_NORMAL;
            }
        } else {
            /* Block learn thread and wait for interrupt. */
            no_new_learn = 0;
            empty_count = 0;
            interval = SHR_THREAD_FOREVER;
            bcmcth_l2_drv_intr_enable(unit, 1);
        }
        learn_ctrl->interval = interval;
    } while (learn_ctrl->active);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "L2 thread exited.\n")));
}

static void
l2_learn_intr_handler(int unit, uint32_t intr_param)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_ctrl_t *learn_ctrl;

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    if (learn_ctrl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Learn ctrl NULL\n")));
        return;
    }

    if (learn_ctrl->thread_ctrl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Thread ctrl NULL\n")));
        return;
    }

    bcmcth_l2_drv_intr_enable(unit, 0);

    learn_ctrl->interval = L2_LEARN_INTERVAL_AGRRESIVE;

    shr_thread_wake(learn_ctrl->thread_ctrl);
}

static int
l2_learn_cache_info_init(int unit, bool warm)
{
    bcmcth_learn_info_drv_t *info_drv = NULL;
    bcmdrd_dev_type_t dev_type;
    bcmlrd_variant_t variant;
    bcmcth_info_table_t data;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    variant = bcmlrd_variant_get(unit);
    /* Perform ingress info table setup */
    data.table_id = 0xFFFFFFFF;
    info_drv = learn_cache_data_info_drv_get[variant].drv_get(unit);
    if (info_drv && info_drv->get_data) {
        SHR_IF_ERR_EXIT
            (info_drv->get_data(unit, &data));

        SHR_IF_ERR_EXIT
            (info_tbl_init(unit, &data, warm));
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */
int
bcmcth_l2_learn_init(int unit, bool warm)
{
    bcmcth_l2_drv_t *drv;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    drv->learn_ctrl = NULL;
    drv->report_ctrl = NULL;

    /* Disable interrupt before software ready. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_enable(unit, 0));

    /* Allocate learn control. */
    SHR_ALLOC(drv->learn_ctrl,
              sizeof(bcmcth_l2_learn_ctrl_t),
              "bcmcthL2LearnCtrl");
    SHR_NULL_CHECK(drv->learn_ctrl, SHR_E_MEMORY);
    sal_memset(drv->learn_ctrl, 0, sizeof(bcmcth_l2_learn_ctrl_t));

    /* Allocate report control */
    ha_req_size = sizeof(bcmcth_l2_learn_report_ctrl_t);
    ha_alloc_size = ha_req_size;
    drv->report_ctrl = shr_ha_mem_alloc(unit,
                                        BCMMGMT_L2_COMP_ID,
                                        BCML2_LEARN_CTRL_COMP_ID,
                                        "bcmcthL2LearnReportCtrl",
                                        &ha_alloc_size);

    SHR_NULL_CHECK(drv->report_ctrl, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        /* Allocated size may exceed requested size, clear the whole memory */
        sal_memset(drv->report_ctrl, 0, ha_alloc_size);

        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_L2_COMP_ID,
                                        BCML2_LEARN_CTRL_COMP_ID, 0,
                                        sizeof(bcmcth_l2_learn_report_ctrl_t), 1,
                                        BCMCTH_L2_LEARN_REPORT_CTRL_T_ID));
    }

    SHR_IF_ERR_EXIT
        (bcmcth_l2_learn_imm_register(unit));

    SHR_IF_ERR_EXIT
        (l2_learn_cache_info_init(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        if (drv->report_ctrl != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_L2_LEARN_REPORT_CTRL_T_ID,
                                      BCMMGMT_L2_COMP_ID,
                                      BCML2_LEARN_CTRL_COMP_ID);
            shr_ha_mem_free(unit, drv->report_ctrl);
            drv->report_ctrl = NULL;
        }
        if (drv->learn_ctrl) {
            SHR_FREE(drv->learn_ctrl);
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_cleanup(int unit)
{
    bcmcth_l2_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    /* Clean up device specific resource. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_cleanup(unit));

    drv = bcmcth_l2_drv_get(unit);
    SHR_FREE(drv->learn_ctrl);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_start(int unit)
{
    bcmcth_l2_learn_ctrl_t *learn_ctrl;
    bcmcth_l2_drv_t *drv;
    bool sim = bcmdrd_feature_is_sim(unit);
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;
    report_ctrl = drv->report_ctrl;
    /* Warmboot */
    if (report_ctrl->enable == 0) {
        SHR_EXIT();
    }
    if (sim) {
        /*
         * Simulator cannot raise interrupt, wake up thread if report enabled.
         * Report ctrl enable status is recoverd after warmboot.
         */
        learn_ctrl->interval = L2_LEARN_INTERVAL_NORMAL;
    } else {
        learn_ctrl->interval = 0;
    }
    learn_ctrl->active = 1;

    /* Create L2 learn thread. */
    if (learn_ctrl->thread_ctrl == NULL) {
        learn_ctrl->thread_ctrl =
            shr_thread_start("bcmcthL2Learn", -1, l2_learn_thread,
                             (void *)(uintptr_t)unit);
        SHR_NULL_CHECK(learn_ctrl->thread_ctrl, SHR_E_FAIL);
    }
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_handler_set(unit, l2_learn_intr_handler));

    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_enable(unit, 1));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_stop(int unit)
{
    bcmcth_l2_learn_ctrl_t *learn_ctrl;
    bcmcth_l2_drv_t *drv;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Disable interrupt. */
    SHR_IF_ERR_EXIT
        (bcmcth_l2_drv_intr_enable(unit, 0));

    drv = bcmcth_l2_drv_get(unit);
    learn_ctrl = drv->learn_ctrl;

    learn_ctrl->active = 0;
    if (learn_ctrl->thread_ctrl != NULL) {
        rv = shr_thread_stop(learn_ctrl->thread_ctrl, 2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "L2 learn thread will not exit.\n")));
        }
        learn_ctrl->thread_ctrl = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_report_enable(int unit, uint32_t trans_id, bool enable)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    report_ctrl = drv->report_ctrl;
    SHR_NULL_CHECK(report_ctrl, SHR_E_INIT);

    if (enable) {
        report_ctrl->enable = enable;

        SHR_IF_ERR_EXIT
            (bcmcth_l2_learn_start(unit));

        SHR_IF_ERR_EXIT
            (bcmcth_l2_drv_hw_enable(unit, trans_id, 1));
    } else {
        SHR_IF_ERR_EXIT
            (bcmcth_l2_drv_hw_enable(unit, trans_id, 0));

        SHR_IF_ERR_EXIT
            (bcmcth_l2_learn_stop(unit));

        report_ctrl->enable = enable;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l2_learn_slow_poll_enable(int unit, bool slow_poll)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_learn_report_ctrl_t *report_ctrl;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    report_ctrl = drv->report_ctrl;
    SHR_NULL_CHECK(report_ctrl, SHR_E_INIT);

    report_ctrl->slow_poll = slow_poll;

exit:
    SHR_FUNC_EXIT();
}

