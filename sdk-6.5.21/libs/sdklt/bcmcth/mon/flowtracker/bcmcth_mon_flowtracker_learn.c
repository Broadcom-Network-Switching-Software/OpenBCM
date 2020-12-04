/*! \file bcmcth_mon_flowtracker_learn.c
 *
 * Flowtracker learning thread and related APIs
 */
/*
 * $copyright:.$
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bsl/bsl_enum.h>
#include <bsl/bsl.h>

#include <bcmdrd_config.h>
#include <bcmbd/bcmbd_ipep_intr.h>

#include <bcmcth/bcmcth_mon_flowtracker_fifo.h>
#include <bcmcth/bcmcth_mon_flowtracker_learn.h>
#include <bcmcth/bcmcth_mon_flowtracker_intr.h>
/******************************************************************************
* Defines
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/* Macros for taking/releasing mutex */
#define FT_LEARN_THREAD_MUTEX_TAKE(unit)\
        rv = sal_mutex_take(ft_learn_mutex[unit],\
                            ft_learn_mutex_timeout);\
        if (rv < 0) {\
            LOG_VERBOSE(BSL_LOG_MODULE,\
                    (BSL_META_U(unit, "Unit%d:\
                                %s mutex take failed\n"),\
                     unit, __FUNCTION__));\
                    SHR_ERR_EXIT(SHR_E_INTERNAL);\
        }

#define FT_LEARN_THREAD_VOID_FN_MUTEX_TAKE(unit)\
        rv = sal_mutex_take(ft_learn_mutex[unit],\
                            ft_learn_mutex_timeout);\
        if (rv < 0) {\
            LOG_VERBOSE(BSL_LOG_MODULE,\
                    (BSL_META_U(unit, "Unit%d:\
                                %s mutex take failed\n"),\
                     unit, __FUNCTION__));\
                    return;\
        }

#define FT_LEARN_THREAD_MUTEX_GIVE(unit)\
    rv = sal_mutex_give(ft_learn_mutex[unit]);\
    if (rv < 0) {\
        LOG_VERBOSE(BSL_LOG_MODULE,\
               (BSL_META_U(unit, "Unit%d:\
               %s mutex give failed\n"),\
                unit, __FUNCTION__));\
    }

/* Macros for accessing datastructure */
#define FT_LEARN_THREAD_PIPE_INFO_SIZE \
        (sizeof(bcmcth_mon_flowtracker_learn_ctrl_pipe_t))
#define FT_LEARN_THREAD_ALL_PIPES_INFO(unit)\
        (ft_learn_ctrl[unit].pipe_info)
#define FT_LEARN_THREAD_PIPE_INFO(unit, pipe)\
    (&(ft_learn_ctrl[unit].pipe_info[pipe]))
#define FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe)\
    (FT_LEARN_THREAD_PIPE_INFO(unit, pipe)->thread_ctrl)
#define FT_LEARN_THREAD_PIPE_THREAD_IS_ACTIVE(unit, pipe)\
    (FT_LEARN_THREAD_PIPE_INFO(unit, pipe)->is_active)
#define FT_LEARN_THREAD_PIPE_THREAD_SLEEP_INTVL(unit, pipe)\
    (FT_LEARN_THREAD_PIPE_INFO(unit, pipe)->sleep_intvl)

#define FT_LEARN_FAIL_THREAD_PIPE_INFO_SIZE \
        (sizeof(bcmcth_mon_flowtracker_learn_ctrl_pipe_t))
#define FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit)\
        (ft_learn_ctrl[unit].fail_pipe_info)
#define FT_LEARN_FAIL_THREAD_PIPE_INFO(unit, pipe)\
    (&(ft_learn_ctrl[unit].fail_pipe_info[pipe]))
#define FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe)\
    (FT_LEARN_FAIL_THREAD_PIPE_INFO(unit, pipe)->thread_ctrl)
#define FT_LEARN_FAIL_THREAD_PIPE_THREAD_IS_ACTIVE(unit, pipe)\
    (FT_LEARN_FAIL_THREAD_PIPE_INFO(unit, pipe)->is_active)
#define FT_LEARN_FAIL_THREAD_PIPE_THREAD_SLEEP_INTVL(unit, pipe)\
    (FT_LEARN_FAIL_THREAD_PIPE_INFO(unit, pipe)->sleep_intvl)


#define FT_LEARN_THREAD_ARG_UNIT_PIPE_SET(unit, pipe)\
            ((void *)(uintptr_t)((unit & 0xffff) | ((pipe & 0xffff) << 16)))

/* NOTE: Below macros SHOULD ONLY be used by bcmcth_mon_ft_learn_thread */
#define FT_LEARN_THREAD_ARG_TO_UNIT(arg) (((uintptr_t)arg) & 0xffff)
#define FT_LEARN_THREAD_ARG_TO_PIPE(arg) ((((uintptr_t)arg) >> 16) & 0xffff)

#define FT_LEARN_THREAD_REENABLE_INTR\
                rv = bcmcth_mon_ft_learn_intr_enable(unit, pipe, true);\
                if (SHR_FAILURE(rv)) {\
                    LOG_WARN(BSL_LOG_MODULE,\
                            (BSL_META_U(unit,\
                                        "Ft learn thread(%d:%d) Intr enable set failed.\n"), unit,pipe));\
                }

#define FT_LEARN_THREAD_ERR_HANDLE(rv, str)\
            if (SHR_FAILURE(rv)) {\
                LOG_WARN(BSL_LOG_MODULE,\
                        (BSL_META_U(unit,\
                                    "Ft learn thread(%d:%d):" str"\n"),unit, pipe));\
                FT_LEARN_THREAD_REENABLE_INTR;\
                continue;\
            }
/******************************************************************************
* Private variables
 */
static bcmcth_mon_flowtracker_learn_ctrl_t ft_learn_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/* Mutex for protecting the access to ft_learn_ctrl structure */
static sal_mutex_t ft_learn_mutex[BCMDRD_CONFIG_MAX_UNITS];

/* The mutex timeout in usecs */
static const int ft_learn_mutex_timeout = 1000000; /* 1 second */

/******************************************************************************
* Private functions
 */
static void
bcmcth_mon_ft_learn_fail_thread(shr_thread_t tc, void *arg)
{
    int unit = FT_LEARN_THREAD_ARG_TO_UNIT(arg);
    int pipe = FT_LEARN_THREAD_ARG_TO_PIPE(arg);
    int rv = SHR_E_NONE;

    /* Forever */
    for (;;) {
        /* Sleep until woken up by ISR */
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            break;
        }

        /* Post the interrupt to the state LT */
        rv = bcmcth_mon_ft_learn_fail_event_state_entry_update(unit, pipe,
            BCMLTD_COMMON_MON_FLOWTRACKER_LEARN_FAIL_EVENT_STATE_T_T_TRIGGERED);
        if (SHR_FAILURE(rv)) {
            /* Unexpected. Log it */
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Ft learn fail thread(%d:%d) unable to update state LT.\n"),
                     unit, pipe));
        }
        rv = bcmcth_mon_ft_learn_fail_counter_clear(unit, pipe);
        if (SHR_FAILURE(rv)) {
            /* Unexpected. Log it */
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Ft learn fail thread(%d:%d) unable to clear failure counter.\n"),
                     unit, pipe));
        }
        /*
         * NOTE: Learn fail thread will not re-enable the interrupt.
         * The user application or HSDK is expected to re-enable the interrupt
         * throught the ACTION LT.
         */
    }
    LOG_DEBUG(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "Ft learn fail thread exited.\n")));
}

static void
bcmcth_mon_ft_learn_thread(shr_thread_t tc, void *arg)
{
    int unit = FT_LEARN_THREAD_ARG_TO_UNIT(arg);
    int pipe = FT_LEARN_THREAD_ARG_TO_PIPE(arg);
    int rv = SHR_E_NONE;
    sal_usecs_t learn_interval = 0;
    uint32_t count = {0};

    /* Forever */
    for (;;) {
        /* Get the learn interval */
        rv = bcmcth_mon_ft_learn_thread_intvl_get(unit, pipe,
                &learn_interval);
        if (SHR_FAILURE(rv)) {
            /* Unexpected. Simply set it to forever */
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Ft learn thread(%d:%d) unable to get interval.Setting it to forever!.\n"),
                     unit, pipe));
            learn_interval = SHR_THREAD_FOREVER;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Ft learn thread(%d:%d) poll interval %d usecs.\n"),
                              unit, pipe, learn_interval));
        /* Sleep until woken up by ISR or timeout */
        shr_thread_sleep(tc, learn_interval);
        if (shr_thread_stopping(tc)) {
            break;
        }

        count = 0;
        rv = bcmcth_mon_ft_fifo_entry_count_get(unit, pipe, &count);
        FT_LEARN_THREAD_ERR_HANDLE(rv, "Count get failed.");

        if (count <= 0) {
            /* Nothing to do. Enable the interrupts back. */
            FT_LEARN_THREAD_REENABLE_INTR;
            continue;
        }

        /* Refresh the SW cache */
        rv = bcmcth_mon_ft_fifo_sw_cache_refresh(unit, pipe, count);
        FT_LEARN_THREAD_ERR_HANDLE(rv, "SW cache refresh failed.");

        /*
         * Traverse the SW cache and update the
         * MON_FLOWTRACKER_LEARN_EVENT_LOG logical table
         */
        rv = bcmcth_mon_ft_fifo_sw_cache_traverse_and_report(unit, pipe,
                                                            count);
        FT_LEARN_THREAD_ERR_HANDLE(rv, "SW cache traverse and report failed.");
        /* Check if there are more FIFO entries to handle */
        count = 0;
        rv = bcmcth_mon_ft_fifo_entry_count_get(unit, pipe, &count);
        FT_LEARN_THREAD_ERR_HANDLE(rv, "Count get failed.");
        if (count > 0) {
            /*
             * There are some more entries in HW.
             * Set aggressive learn interval
             */
            rv = bcmcth_mon_ft_learn_thread_intvl_set(unit, pipe,
                    FT_LEARN_THREAD_AGGRESSIVE_INTVL);
            FT_LEARN_THREAD_ERR_HANDLE(rv, "Aggressive interval set failed.");
        } else {
            /* No further FIFO entries. Re-enable the interrupts */
            FT_LEARN_THREAD_REENABLE_INTR;
            /* Reset the interval to forever until interrupted. */
            rv = bcmcth_mon_ft_learn_thread_intvl_set(unit, pipe,
                    SHR_THREAD_FOREVER);
            FT_LEARN_THREAD_ERR_HANDLE(rv, "Forever interval set failed.");
        }
    }
    LOG_DEBUG(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "Ft learn thread exited.\n")));
}

static int bcmcth_mon_ft_learn_thread_create(int unit, int pipe)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    if (FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe) == NULL) {
        FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe) =
            shr_thread_start("bcmcthFtLearn", -1, bcmcth_mon_ft_learn_thread,
                             (FT_LEARN_THREAD_ARG_UNIT_PIPE_SET(unit,pipe)));
        SHR_NULL_CHECK(FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe), SHR_E_FAIL);
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Ft learn thread(%d:%d) already running!!.\n"),unit, pipe));
    }
exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

/*
 * NOTE: This should be called only when de-initializing the module.
 *       Not if you want to disable the interrupt handling. For that, the
 *       thread needs to be put to sleep to forever.
 */
static int bcmcth_mon_ft_learn_thread_stop(int unit, int pipe)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    if (FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe) != NULL) {
        rv = shr_thread_stop(FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe),
                             2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "FT learn thread will not exit.\n")));
        }
        FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe) = NULL;
    }
exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

/* Uninitialze the mutex */
static void bcmcth_mon_ft_learn_mutex_deinit(int unit)
{
    if (ft_learn_mutex[unit] != NULL) {
        /* Destroy the mutex */
        sal_mutex_destroy(ft_learn_mutex[unit]);
        ft_learn_mutex[unit] = NULL;
    }
}

/* Initialize the mutex */
static int bcmcth_mon_ft_learn_mutex_init(int unit)
{
    char *mutex_desc = "bcmCthMonFtLearnMutex";
    SHR_FUNC_ENTER(unit);
    /* Create the mutex lock for accessing learn info */
    ft_learn_mutex[unit] = sal_mutex_create(mutex_desc);
    SHR_NULL_CHECK(ft_learn_mutex[unit], SHR_E_MEMORY);
exit:
    SHR_FUNC_EXIT();
}

/* Uninitialize the datastructures */
static void bcmcth_mon_ft_learn_threads_ds_deinit(int unit)
{
    int rv = SHR_E_NONE;
    /* Take the lock */
    FT_LEARN_THREAD_VOID_FN_MUTEX_TAKE(unit);
    if (FT_LEARN_THREAD_ALL_PIPES_INFO(unit) != NULL) {
        SHR_FREE(FT_LEARN_THREAD_ALL_PIPES_INFO(unit));
        FT_LEARN_THREAD_ALL_PIPES_INFO(unit) = NULL;
    }
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
}

/* Initialize the datastructures */
static int bcmcth_mon_ft_learn_threads_ds_init(int unit, int num_pipes)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);

    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    /* Allocate learn info */
    SHR_ALLOC(FT_LEARN_THREAD_ALL_PIPES_INFO(unit),
              (FT_LEARN_THREAD_PIPE_INFO_SIZE * num_pipes),
              "bcmCthMonFtLearnCtrlInfo");
    SHR_NULL_CHECK(FT_LEARN_THREAD_ALL_PIPES_INFO(unit), SHR_E_MEMORY);
    sal_memset(FT_LEARN_THREAD_ALL_PIPES_INFO(unit), 0,
               (FT_LEARN_THREAD_PIPE_INFO_SIZE * num_pipes));

exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

static int bcmcth_mon_ft_learn_fail_thread_create(int unit, int pipe)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    if (FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe) == NULL) {
        FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe) =
            shr_thread_start("bcmcthFtLearnFail", -1, bcmcth_mon_ft_learn_fail_thread,
                             (FT_LEARN_THREAD_ARG_UNIT_PIPE_SET(unit,pipe)));
        SHR_NULL_CHECK(FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe), SHR_E_FAIL);
    } else {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Ft learn fail thread(%d:%d) already running!!.\n"),unit, pipe));
    }
exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

static int bcmcth_mon_ft_learn_fail_thread_stop(int unit, int pipe)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    if (FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe) != NULL) {
        rv = shr_thread_stop(FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe),
                             2 * SECOND_USEC);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "FT learn fail thread will not exit.\n")));
        }
        FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe) = NULL;
    }
exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

/* Uninitialize the datastructures */
static void bcmcth_mon_ft_learn_fail_threads_ds_deinit(int unit)
{
    int rv = SHR_E_NONE;
    /* Take the lock */
    FT_LEARN_THREAD_VOID_FN_MUTEX_TAKE(unit);
    if (FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit) != NULL) {
        SHR_FREE(FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit));
        FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit) = NULL;
    }
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
}

/* Initialize the datastructures */
static int bcmcth_mon_ft_learn_fail_threads_ds_init(int unit, int num_pipes)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);

    /* Allocate learn fail info */
    SHR_ALLOC(FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit),
              (FT_LEARN_FAIL_THREAD_PIPE_INFO_SIZE * num_pipes),
              "bcmCthMonFtLearnFailCtrlInfo");
    SHR_NULL_CHECK(FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit), SHR_E_MEMORY);
    sal_memset(FT_LEARN_FAIL_THREAD_ALL_PIPES_INFO(unit), 0,
               (FT_LEARN_FAIL_THREAD_PIPE_INFO_SIZE * num_pipes));

exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

static void bcmcth_mon_ft_learn_thread_top_lvl_intr_hdlr (int unit,
                                                uint32_t intr_param)
{
    uint8_t pipe;
    int rv;
    bool learn = false;
    bool learn_fail = false;
    bool learn_intr_ena = false;
    bool learn_fail_intr_ena = false;

    /* Get the pipe number from intr_param */
    pipe = IPEP_INTR_INST_GET(intr_param);

    /* Get the intr enable status to figure out if intr is enabled or not */
    rv = bcmcth_mon_ft_learn_intr_enable_get(unit, pipe, &learn_intr_ena,
                                            &learn_fail_intr_ena);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Ft learn thread(%d:%d) Intr status get failed.\n"), unit,pipe));
        return;
    }

    /* Get the intr status to figure out what type of intr it is */
    rv = bcmcth_mon_ft_learn_intr_status_get(unit, pipe, &learn, &learn_fail);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Ft learn thread(%d:%d) Intr status get failed.\n"), unit,pipe));
        return;
    }

    /* Clear the intr status */
    rv = bcmcth_mon_ft_learn_intr_status_clear(unit, pipe);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Ft learn thread(%d:%d) Intr status clear failed.\n"), unit,pipe));
        return;
    }

    if (learn_intr_ena && learn) {
        /* Disable the interrupt first */
        rv = bcmcth_mon_ft_learn_intr_enable(unit, pipe, false);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Ft learn thread(%d:%d) Intr disable set failed.\n"), unit,pipe));
            return;
        }
        /* Wake up the appropriate interrupt thread */
        shr_thread_wake(FT_LEARN_THREAD_PIPE_THREAD_CTRL(unit, pipe));
    }

    if (learn_fail_intr_ena && learn_fail) {
        /* Disable the fail interrupt first */
        rv = bcmcth_mon_ft_learn_fail_intr_enable(unit, pipe, false);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Ft learn thread(%d:%d) Fail Intr disable set failed.\n"), unit,pipe));
            return;
        }
        /* Wake up the appropriate interrupt thread */
        shr_thread_wake(FT_LEARN_FAIL_THREAD_PIPE_THREAD_CTRL(unit, pipe));
    }
}

/******************************************************************************
* Public functions
 */
/*!
 * \brief Get the sleep interval of the FT learn thread.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pipe Pipe number.
 * \param [out]  intvl Sleep interval.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmcth_mon_ft_learn_thread_intvl_get(int unit, int pipe, sal_usecs_t *intvl)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    *intvl = FT_LEARN_THREAD_PIPE_THREAD_SLEEP_INTVL(unit, pipe);
exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}
/*!
 * \brief Set the sleep interval of the FT learn thread.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pipe Pipe number.
 * \param [in]  intvl Sleep interval.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmcth_mon_ft_learn_thread_intvl_set(int unit, int pipe, sal_usecs_t intvl)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(unit);
    /* Take the lock */
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    FT_LEARN_THREAD_PIPE_THREAD_SLEEP_INTVL(unit, pipe) = intvl;
exit:
    /* Release the lock */
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the FT learn threads.
 *
 * \param [in]  unit Unit number.
 *
 * \retval None.
 */
int bcmcth_mon_ft_learn_threads_deinit(int unit)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int pipe;
    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);
    if (ft_drv == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    /* Stop the threads */
    for (pipe = 0; pipe < ft_drv->num_pipes; pipe++) {
        SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_thread_stop(unit, pipe));
        SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_fail_thread_stop(unit, pipe));
    }

    /* Disable the top level interrupt. */
    SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_top_lvl_intr_enable(unit,
                false));

    /* Uninitialize the data structures */
    bcmcth_mon_ft_learn_threads_ds_deinit(unit);
    bcmcth_mon_ft_learn_fail_threads_ds_deinit(unit);
    bcmcth_mon_ft_learn_mutex_deinit(unit);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the FT learn threads.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmcth_mon_ft_learn_threads_init(int unit)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int pipe;
    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);
    if (ft_drv == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    /* Initialize the datastructures */
    SHR_IF_ERR_EXIT(
        bcmcth_mon_ft_learn_mutex_init(unit));
    SHR_IF_ERR_EXIT(
        bcmcth_mon_ft_learn_threads_ds_init(unit, ft_drv->num_pipes));
    SHR_IF_ERR_EXIT(
        bcmcth_mon_ft_learn_fail_threads_ds_init(unit, ft_drv->num_pipes));

    /* For each pipe */
    for (pipe = 0; pipe < ft_drv->num_pipes; pipe++) {
        /* Set sleep interval to forever */
        SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_thread_intvl_set(unit, pipe,
                    SHR_THREAD_FOREVER));
        /* Create learn thread */
        SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_thread_create(unit, pipe));
        /* Create learn fail thread */
        SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_fail_thread_create(unit, pipe));
    }
    /* Enable the top level interrupt. */
    SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_top_lvl_intr_enable(unit,
                true));

    /* Set interrupt handler routine */
    SHR_IF_ERR_EXIT(bcmcth_mon_ft_learn_intr_handler_set(unit,
                &bcmcth_mon_ft_learn_thread_top_lvl_intr_hdlr,
                0));
exit:
    if (SHR_FUNC_ERR()) {
        /* Ignore the return value */
        (void)bcmcth_mon_ft_learn_threads_deinit(unit);
        bcmcth_mon_ft_learn_mutex_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump the FT learn threads info.
 *
 * \param [in]  unit Unit number.
 * \param [out] info Info structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int bcmcth_mon_ft_learn_thread_info_dump(
                        int unit,
                        bcmcth_mon_flowtracker_learn_ctrl_debug_t *info)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int rv;
    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);
    if (ft_drv == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    info->num_pipes = ft_drv->num_pipes;
    FT_LEARN_THREAD_MUTEX_TAKE(unit);
    sal_memcpy(info->pipe_info, FT_LEARN_THREAD_ALL_PIPES_INFO(unit),
               (FT_LEARN_THREAD_PIPE_INFO_SIZE * ft_drv->num_pipes));
exit:
    FT_LEARN_THREAD_MUTEX_GIVE(unit);
    SHR_FUNC_EXIT();
}
