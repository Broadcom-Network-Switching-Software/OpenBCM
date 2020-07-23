/*! \file bcmsec_event.c
 *
 * BCMSEC event APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmsec/bcmsec_event.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_utils_internal.h>

#define MAX_PENDING_TRANSACTIONS 256
#define BSL_LOG_MODULE  BSL_LS_BCMSEC_EVENT
/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocates memory for event info.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY No memory. Allocation failed.
 */
static int
bcmsec_event_info_alloc(int unit)
{
    bcmsec_event_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);
    bcmsec_event_info_get(unit, &info);

    if (!info) {
        info = sal_alloc(sizeof(bcmsec_event_info_t), "bcmsecIntrInfo");
        SHR_NULL_CHECK(info, SHR_E_MEMORY);
    }

    sal_memset(info, 0, sizeof(bcmsec_event_info_t));
    bcmsec_event_info_set(unit, info);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free SEC event info memory allocated.
 *
 * \param [in] unit Logical unit number.
 */
static void
bcmsec_event_info_free(int unit)
{
    bcmsec_event_info_t *info;
    bcmsec_event_info_get(unit, &info);

    sal_free(info);
    bcmsec_event_info_set(unit, NULL);
}

/*!
 * \brief SA Expiry handler
 *
 * \param [in] unit Logical unit number.
 * \param [in] dir Encrypt/Decrypt
 *
 * \retval N.A.
 */
static int
bcmsec_sa_expiry (int unit, int blk_id, int dir)
{
    bcmsec_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->sa_expire_handler, SHR_E_UNAVAIL);

    /* Handle SA expiry in PT. */
    if (drv->sa_expire_handler) {
        SHR_IF_ERR_EXIT
            (drv->sa_expire_handler(unit, blk_id, dir));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Event handler for SEC
 *
 * \param [in] unit Logical unit number.
 * \param [in] trigger_info Event info.
 *
 * \retval N.A.
 */
static void
bcmsec_event_handler (int unit, bcmsec_event_trigger_t *trigger_info)
{
    int blk_id, event_num;

    blk_id = trigger_info->blk_id;
    event_num = trigger_info->intr_num;

    if ((event_num == SA_EXPIRY_ENCRYPT) ||
        (event_num == SA_EXPIRY_DECRYPT)) {
        (void)bcmsec_sa_expiry(unit, blk_id, event_num);
    }
    return;
}

/*!
 * \brief SEC event thread routine.
 *
 * \param [in] tc Thread control.
 * \param [in] arg Thread arguments.
 */
static void
bcmsec_event_thread(shr_thread_t tc, void *arg)
{
    bool running = true;
    bcmsec_event_trigger_t trigger_info;
    int rv, unit ;
    bcmsec_event_info_t *info;

    unit = (unsigned long) (arg);
    bcmsec_event_info_get(unit, &info);

    if (!info) {
        return;
    }

    while (running) {
        rv = sal_msgq_recv(info->bcmsec_event_q, (void *)&trigger_info,
                           SAL_MSGQ_FOREVER);
        if (rv != 0) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("Failed to receive SEC trigger message %d\n"), rv));
            break;
        }
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META("Got SEC trigger message for intr_num %d\n"),
                     trigger_info.intr_num));

        switch (trigger_info.type) {
            case SEC_TRIG_NOTIF:
                (void) bcmsec_event_handler(unit, &trigger_info);
                break;
            case SEC_TRIG_EXIT:
                /* Exit SEC thread.*/
                running = false;
                break;
            default:
                break;
        }
    }
    sal_sem_give(info->sec_event_thread);
}

/*******************************************************************************
* Public functions
 */
int
bcmsec_event_init(int unit, int warm)
{
    bcmsec_event_info_t *info;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* SEC_BLOCKt */
    rv = bcmlrd_map_get(unit, SEC_BLOCKt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmsec_event_info_alloc(unit));

    bcmsec_event_info_get(unit, &info);

    if (!info->event_lock) {
        info->event_lock = sal_mutex_create("bcmsecIntrLock");
    }

    SHR_NULL_CHECK(info->event_lock, SHR_E_MEMORY);

    info->sec_event_thread = sal_sem_create("bcmsecIntrSemaphore",
                                    SAL_SEM_BINARY, 0);
    SHR_NULL_CHECK(info->sec_event_thread, SHR_E_MEMORY);

    /*
     * Create message queue for notifications from interrupt handler to
     * SEC event thread.
     */
    info->bcmsec_event_q = sal_msgq_create(sizeof(bcmsec_event_trigger_t),
                                              MAX_PENDING_TRANSACTIONS,
                                              "bcmsecIntrTriggers");
    SHR_NULL_CHECK(info->bcmsec_event_q, SHR_E_MEMORY);

    /* Create the BST event handler thread. */
    info->pid = shr_thread_start("bcmsecIntrThread",
                               SAL_THREAD_PRIO_DEFAULT,
                               bcmsec_event_thread,
                               (void *)(unsigned long)(unit));
    if (info->pid == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
exit:
    if (SHR_FUNC_ERR()) {
        bcmsec_event_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmsec_event_cleanup(int unit)
{
    int rv;
    bcmsec_event_trigger_t trigger_info;
    bcmsec_event_info_t *info;

    SHR_FUNC_ENTER(unit);

    bcmsec_event_info_get(unit, &info);
    if (info == NULL) {
        SHR_EXIT();
    }
    trigger_info.type = SEC_TRIG_EXIT;

    rv = sal_msgq_post(info->bcmsec_event_q, &trigger_info,
                       SAL_MSGQ_HIGH_PRIORITY, SAL_MSGQ_FOREVER);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    (void) sal_sem_take(info->sec_event_thread, SAL_SEM_FOREVER);
    if (info->event_lock) {
        sal_mutex_destroy(info->event_lock);
        info->event_lock = NULL;
    }
    if (info->sec_event_thread) {
        sal_sem_destroy(info->sec_event_thread);
        info->sec_event_thread = NULL;
    }
    sal_msgq_destroy(info->bcmsec_event_q);

    /* Wait for the thread to exit. */
    if (info->pid) {
        SHR_IF_ERR_EXIT
            (shr_thread_stop(info->pid, 500000));
    }
    info->pid = NULL;
    bcmsec_event_info_free(unit);
exit:
    SHR_FUNC_EXIT();
}
