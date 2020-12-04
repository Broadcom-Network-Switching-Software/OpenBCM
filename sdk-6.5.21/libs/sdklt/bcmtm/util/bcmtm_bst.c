/*! \file bcmtm_bst.c
 *
 * TM BST utility functions.
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
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_BST

/*******************************************************************************
* Local definitions
 */

static bcmtm_bst_dev_info_t *bcmtm_bst_dev[BCMDRD_CONFIG_MAX_UNITS];

#define MAX_PENDING_TRANSACTIONS 256
#define ONE_SEC_WAIT 1000000

/*******************************************************************************
* Private functions
 */
/*!
 * \brief Allocates memory for BST device info.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY No memory. Allocation failed.
 */
static int
bcmtm_bst_dev_info_alloc(int unit)
{
    bcmtm_bst_dev_info_t *bd = NULL;

    SHR_FUNC_ENTER(unit);

    bd = sal_alloc(sizeof(bcmtm_bst_dev_info_t), "bcmtmBstDevInfo");
    SHR_NULL_CHECK(bd, SHR_E_MEMORY);

    sal_memset(bd, 0, sizeof(bcmtm_bst_dev_info_t));
    bcmtm_bst_dev[unit] = bd;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free BST device info memeory allocated by bcmtm_bst_dev_info_alloc.
 *
 * \param [in] unit Logical unit number.
 */
static void
bcmtm_bst_dev_info_free(int unit)
{
    bcmtm_bst_dev_info_t *bd;
    bd = bcmtm_bst_dev[unit];
    sal_free(bd);
    bcmtm_bst_dev[unit] = NULL;
}

/*!
 * \brief BST thread routine.
 *
 * This reads the interrupt message from message queue and then takes the action
 * for BST.
 *
 * \param [in] tc Thread control.
 * \param [in] arg Thread arguments.
 */
static void
bcmtm_bst_thread(shr_thread_t tc, void *arg)
{
    bool running = true;
    bcmtm_bst_trigger_t trigger_info;
    int rv, unit ;
    bcmtm_bst_dev_info_t *bd;
    bcmtm_bst_drv_t bst_drv;
    bcmtm_drv_t *drv;

    unit = (unsigned long) (arg);
    bd = bcmtm_bst_dev[unit];

    if (!bd) {
        /* BST device is not initialized */
        return;
    }

    (void)(bcmtm_drv_get(unit, &drv));
    drv->bst_drv_get(unit, &bst_drv);
    while (running) {
        rv = sal_msgq_recv(bd->bcmtm_bst_trigger_q, (void *)&trigger_info,
                           SAL_MSGQ_FOREVER);
        if (rv != 0) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("Failed to receive BST trigger message %d\n"), rv));
            break;
        }
        LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META("Got BST trigger message from message queue")));
        switch (trigger_info.type) {
            case BST_TRIG_NOTIF:
                /* process BST message from message queue. */
                if (bst_drv.bst_trigger) {
                    rv = bst_drv.bst_trigger(unit, &trigger_info);
                    if (SHR_FAILURE(rv)) {
                        LOG_ERROR(BSL_LOG_MODULE,
                                 (BSL_META("BST thread exit. %s\n"),
                                  shr_errmsg(rv)));
                        running = false;
                    }
                }
                break;
            case BST_TRIG_EXIT:
                /* Exit BST thread.*/
                running = false;
                break;
            default:
                break;
        }
    }
    sal_sem_give(bd->bst_thread);
}

/*!
 * \brief Initialize TM_BST_EVENT_STATEt.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FULL Entry list full.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
static int
bcmtm_bst_event_state_init (int unit)
{
    bcmltd_fields_t in;
    bcmltd_sid_t ltid = TM_BST_EVENT_STATEt;
    bcmltd_fid_t fid;
    uint64_t min, max, buffer_pool;
    const bcmlrd_map_t *map = NULL;
    int rv, class;

    SHR_FUNC_ENTER(unit);

    /* default initialization */
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_BST_EVENT_STATEt_FIELD_COUNT, &in));
    in.count = 0;
    fid = TM_BST_EVENT_STATEt_BUFFER_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &min, &max));

    for (buffer_pool = min; buffer_pool <= max; buffer_pool++) {
        if (bcmtm_itm_valid_get(unit, buffer_pool) != SHR_E_NONE) {
            continue;
        }
        fid = TM_BST_EVENT_STATEt_BUFFER_POOLf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0, buffer_pool));
        for (class = 0; class < BST_EVENT_CLASS_MAX; class++) {
            fid = TM_BST_EVENT_STATEt_CLASSf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, class));
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit, ltid, &in));
        }
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize TM_BST_EVENT_SOURCE_INGt.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FULL Entry list full.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
static int
bcmtm_bst_ing_event_source_init(int unit)
{
    bcmltd_fields_t in;
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_INGt;
    bcmltd_fid_t fid;
    uint64_t min, max, pipe;
    uint32_t valid_pipe_bmp;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* default initialization */
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    /* only one key : pipe */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));

    in.count = 0;
    fid = TM_BST_EVENT_SOURCE_INGt_TM_PIPEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &min, &max));

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &valid_pipe_bmp));

    for (pipe = min; pipe <= max; pipe++) {
        if ((1 << pipe) & valid_pipe_bmp) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, pipe));
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit, ltid, &in));
        }
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize TM_BST_EVENT_SOURCE_EGRt.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FULL Entry list full.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
static int
bcmtm_bst_egr_event_source_init(int unit)
{
    bcmltd_fields_t in;
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_EGRt;
    bcmltd_fid_t fid;
    uint64_t min, max, buffer_pool;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* default initialization */
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }
    /* only one key : buffer pool */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));

    in.count = 0;
    fid = TM_BST_EVENT_SOURCE_EGRt_BUFFER_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &min, &max));

    for (buffer_pool = min; buffer_pool <= max; buffer_pool++) {
        if (bcmtm_itm_valid_get(unit, buffer_pool) != SHR_E_NONE) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0, buffer_pool));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize TM_BST_EVENT_SOURCE_REPL_Qt.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FULL Entry list full.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
static int
bcmtm_bst_repl_q_event_source_init(int unit)
{
    bcmltd_fields_t in;
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_REPL_Qt ;
    bcmltd_fid_t fid;
    uint64_t min, max, buffer_pool;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* default initialization */
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, ltid, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }
    /* only one key : buffer pool */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));

    in.count = 0;
    fid = TM_BST_EVENT_SOURCE_REPL_Qt_BUFFER_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &min, &max));

    for (buffer_pool = min; buffer_pool <= max; buffer_pool++) {
        if (bcmtm_itm_valid_get(unit, buffer_pool) != SHR_E_NONE) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0, buffer_pool));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, ltid, &in));
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */
void
bcmtm_bst_dev_info_get(int unit,
                       bcmtm_bst_dev_info_t **dev)
{
    *dev = bcmtm_bst_dev[unit];
}

int
bcmtm_bst_init(int unit, bool warm)
{
    bcmtm_bst_dev_info_t *bd;

    SHR_FUNC_ENTER(unit);

    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmtm_bst_event_state_init(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_bst_ing_event_source_init(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_bst_egr_event_source_init(unit));
        SHR_IF_ERR_EXIT
            (bcmtm_bst_repl_q_event_source_init(unit));
    }

    SHR_IF_ERR_EXIT
        (bcmtm_bst_dev_info_alloc(unit));

    bd = bcmtm_bst_dev[unit];

    if (!bd->bst_dev_lock) {
        bd->bst_dev_lock = sal_mutex_create("bcmtmBstLock");
    }

    SHR_NULL_CHECK(bd->bst_dev_lock, SHR_E_MEMORY);

    bd->bst_thread = sal_sem_create("bcmtmBstThreadSemaphore",
                                    SAL_SEM_BINARY, 0);
    SHR_NULL_CHECK(bd->bst_thread, SHR_E_MEMORY);

    /*
     * Create message queue for notifications from interrupt handler to
     * BST event thread.
     */
    bd->bcmtm_bst_trigger_q = sal_msgq_create(sizeof(bcmtm_bst_trigger_t),
                                              MAX_PENDING_TRANSACTIONS,
                                              "bcmtmBstTriggers");
    SHR_NULL_CHECK(bd->bcmtm_bst_trigger_q, SHR_E_MEMORY);

    /* Create the BST event handler thread. */
    bd->pid = shr_thread_start("bcmtmBstEvent",
                               SAL_THREAD_PRIO_DEFAULT,
                               bcmtm_bst_thread,
                               (void *)(unsigned long)(unit));
    if (bd->pid == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmtm_bst_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_cleanup(int unit)
{
    int rv;
    bcmtm_bst_trigger_t trigger_info;
    bcmtm_bst_dev_info_t *bd;

    SHR_FUNC_ENTER(unit);

    bd = bcmtm_bst_dev[unit];
    trigger_info.type = BST_TRIG_EXIT;

    rv = sal_msgq_post(bd->bcmtm_bst_trigger_q, &trigger_info,
                       SAL_MSGQ_HIGH_PRIORITY, SAL_MSGQ_FOREVER);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    (void) sal_sem_take(bd->bst_thread, SAL_SEM_FOREVER);
    if (bd->bst_dev_lock) {
        sal_mutex_destroy(bd->bst_dev_lock);
        bd->bst_dev_lock = NULL;
    }
    if (bd->bst_thread) {
        sal_sem_destroy(bd->bst_thread);
    }
    sal_msgq_destroy(bd->bcmtm_bst_trigger_q);
    bd->bcmtm_bst_trigger_q = NULL;

    /* Wait for the thread to exit. */
    if (bd->pid) {
        SHR_IF_ERR_EXIT
            (shr_thread_stop(bd->pid, 500000));
    }
    bd->pid = NULL;
    bcmtm_bst_dev_info_free(unit);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_event_src_ing_reset(int unit,
                              bcmltd_fields_t *in)
{
    /* Reset the boolean types. */
    bcmltd_fid_t fid[] = {
        TM_BST_EVENT_SOURCE_INGt_SERVICE_POOLf,
        TM_BST_EVENT_SOURCE_INGt_HEADROOM_POOLf,
        TM_BST_EVENT_SOURCE_INGt_PRI_GRPf,
        TM_BST_EVENT_SOURCE_INGt_PRI_GRP_HEADROOMf,
        TM_BST_EVENT_SOURCE_INGt_PORT_SERVICE_POOLf
    };
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_INGt;
    const bcmlrd_field_data_t  *field;
    int count;

    SHR_FUNC_ENTER(unit);

    for (count = 0; count < COUNTOF(fid); count++) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, ltid, fid[count], &field)) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, in, fid[count], 0, 0));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_event_src_egr_reset(int unit,
                              bcmltd_fields_t *in)
{
    /* Reset the boolean types. */
    bcmltd_fid_t fid[] = {
        TM_BST_EVENT_SOURCE_EGRt_UC_Qf,
        TM_BST_EVENT_SOURCE_EGRt_MC_Qf,
        TM_BST_EVENT_SOURCE_EGRt_SERVICE_POOL_UCf,
        TM_BST_EVENT_SOURCE_EGRt_SERVICE_POOL_MCf,
        TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_UCf,
        TM_BST_EVENT_SOURCE_EGRt_PORT_SERVICE_POOL_MCf
    };
    bcmltd_sid_t ltid = TM_BST_EVENT_SOURCE_EGRt;
    const bcmlrd_field_data_t  *field;
    int count;

    SHR_FUNC_ENTER(unit);

    for (count = 0; count < COUNTOF(fid); count++) {
        if (SHR_E_UNAVAIL == bcmlrd_field_get(unit, ltid, fid[count], &field)) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, in, fid[count], 0, 0));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_bst_event_state_update(int unit,
                             bcmtm_bst_event_cfg_t *bst_event_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_fid_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* default initialization */
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_BST_EVENT_STATEt_FIELD_COUNT, &in));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_BST_EVENT_STATEt_FIELD_COUNT, &out));

    in.count = 0;
    /* Keys */
    /* TM_BST_EVENT_STATEt_BUFFER_POOLf */
    fid = TM_BST_EVENT_STATEt_BUFFER_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, bst_event_cfg->buffer_pool));

    /* TM_BST_EVENT_STATEt_CLASSf */
    fid = TM_BST_EVENT_STATEt_CLASSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, bst_event_cfg->class));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, TM_BST_EVENT_STATEt, &in, &out));

    /* TM_BST_EVENT_STATEt_STATEf */
    fid = TM_BST_EVENT_STATEt_STATEf;
    if (SHR_FAILURE(bcmtm_field_list_get(unit, &out, fid, 0, &fval))) {
        fval = 0;
    }

    /* previous state is same as current state no update. */
    if (fval == bst_event_cfg->state) {
        SHR_EXIT();
    }

    /* Update the state to new state */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, bst_event_cfg->state));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, TM_BST_EVENT_STATEt, &in));

exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}
