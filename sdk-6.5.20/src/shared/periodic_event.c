
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    periodic_event.c
 * Purpose:     Generic mechanisem for periodic events.
 */

#include <shared/periodic_event.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <sal/core/time.h>
#include <sal/types.h>
#include <shared/error.h>
#include <shared/bsl.h>
#include <soc/util.h>
#include <soc/drv.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_er_threading.h>
#endif

#define MAX_NAME_SIZE 30

/* 
 
Note about termonology of objects and handlers in this file: 
------------------------------------------------ 
periodic_event_handler_t (which is void*) and periodic_event_object_t* 
are the same entiry. 
*/

/* PE internal description - shouldn't be exposed to periodic event user*/
typedef struct periodic_event_object_s {
    int unit;                       /* unit # */
    char name[MAX_NAME_SIZE];       /* name given by the user (see periodic_event.h)*/
    periodic_event_f callback;      /* callback provided by the user (see periodic_event.h)*/
    void* user_data;                /* user data pointer provided by the user (see periodic_event.h)*/
    sal_sem_t sema;                 /* Semaphore handler for periodic_event_thread usage */
    int interval;                   /* interval provided by the user (see periodic_event.h)*/
    int quit;                       /* indication from main thread whether thread should be exit */
    int thread_priority;            /* thread priority provided by the user (see periodic_event.h)*/
    sal_thread_t tid;               /* created thread ID */
    int error_threshold;            /* error count threshold provided by the user (see periodic_event.h)*/
    uint32 bsl_module;              /* BSL module for logging messages */
} periodic_event_object_t;


/** This function will be registered to sal_thread_create
 *  The function operation:
 *   1. Iterate unless inidcate to quit
 *      1.a. Wait on a semaphore (until triggered or until
 *      timeout provided by the user is passing
 *      1.b. Execute user callback.
 *      1.c. Check call back error
 *          if error inc error count, otherwise zero error
 *          count.
 *      2.d. If error_count > 0 quit the loop.
 *   2. When thread is about to exit:
 *      2.a indicate active is false by changing tid to invalid.
 *      2.b. do sal_thread_exit.
 */
static void periodic_event_thread(periodic_event_object_t* obj)
{
    int rv, err_count = 0;

    LOG_VERBOSE(obj->bsl_module, (BSL_META("PE: Thraed is starting.\n")));

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(obj->unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_ADD(obj->unit);
    }
#endif

    /* iterate until indicate to exit */
    while (!obj->quit) {

        /* wait for trigger or timeout */
        (void)sal_sem_take(obj->sema, obj->interval);

        /* break out if we're told to quit */
        if (obj->quit == 1) {
            break;
        }

        /* run callback */
        rv = obj->callback(obj->unit, obj->user_data);

        /* update error count */
        if (_SHR_E_FAILURE(rv)) {
            LOG_WARN(obj->bsl_module, (BSL_META("PE: Callback failure occured on the %d time in a raw. rv: %d\n"), err_count, rv));
            err_count++;
        } else {
            err_count = 0;
        }

        /* Exist if passed error count threshold. 
           -1 means ignore error counting */
        if (err_count >= obj->error_threshold && obj->error_threshold != -1) {
            LOG_ERROR(obj->bsl_module, (BSL_META("PE: Error threshold acceded. Thread will exit.\n")));
            break;
        }

    }

    LOG_VERBOSE(obj->bsl_module, (BSL_META("PE: Thraed is stopping.\n")));

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(obj->unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_PERIODIC_EVENT_THREAD_REMOVE(obj->unit);
    }
#endif

   /* indicate thread isn't active anymore */
    obj->tid = SAL_THREAD_ERROR;

    /* exit thread */
    sal_thread_exit(0);
}

/** see .h file */
void periodic_event_config_t_init(periodic_event_config_t* config)
{
    sal_memset(config, 0, sizeof(periodic_event_config_t));

    /* set defaults */
    config->thread_priority = 50;
    config->error_threshold = -1;
    config->start_operation = 1;
    config->bsl_module = -1;
}

/** see .h file */
int periodic_event_create(int unit, const periodic_event_config_t* config, periodic_event_handler_t* h)
{
    int rv;
    periodic_event_object_t *obj;

    *h = NULL;

    /* check mandatory configs*/
    if (config->bsl_module == -1) {
        return _SHR_E_CONFIG;
    }

    if (config->name[0] == 0) {
        LOG_ERROR(config->bsl_module, (BSL_META("PE: Name is mandatory\n")));
        return _SHR_E_CONFIG;
    }


    if (config->interval <= 0 && config->interval != sal_sem_FOREVER) {
        LOG_ERROR(config->bsl_module, (BSL_META("PE[%s]: Interval must be > 0\n"), config->name));
        return _SHR_E_CONFIG;
    }

    if (config->callback == NULL) {
        LOG_ERROR(config->bsl_module, (BSL_META("PE[%s]: Callback must be assigned \n"), config->name));
        return _SHR_E_CONFIG;
    }

    /* allocate obj */
    obj = sal_alloc(sizeof(periodic_event_object_t), config->name);
    if (obj == NULL) {
        LOG_ERROR(config->bsl_module, (BSL_META("PE[%s]: Failed to allocate memory\n"), config->name));
        return _SHR_E_MEMORY;
    }

    /* initialize obj */
    sal_memset(obj, 0, sizeof(periodic_event_object_t));

    obj->unit = unit;
    sal_strncpy(obj->name, config->name, MAX_NAME_SIZE-1);
    obj->callback = config->callback;
    obj->user_data = config->user_data;
    obj->interval = config->interval;
    obj->quit = 0;
    obj->tid = SAL_THREAD_ERROR;
    obj->thread_priority = config->thread_priority;
    obj->error_threshold = config->error_threshold;
    obj->bsl_module = config->bsl_module;

    /* initialize semaphore */
    obj->sema = sal_sem_create(config->name, sal_sem_BINARY, 0);
    if (obj->sema == NULL) {
        LOG_ERROR(config->bsl_module, (BSL_META("PE[%s]: Failed to allocate semaphore\n"), config->name));
        sal_free(obj);
        obj = NULL;
        return _SHR_E_MEMORY;
    }

    /* init the thraed if requested */
    if (config->start_operation) {
        rv = periodic_event_start((periodic_event_handler_t)obj);
        if (_SHR_E_FAILURE(rv)) {
            LOG_ERROR(config->bsl_module, (BSL_META("PE[%s]: Failed to start event\n"), config->name));
            sal_sem_destroy(obj->sema);
            sal_free(obj);
            return rv;
        }
    }

    *h = (periodic_event_handler_t)obj;

    return _SHR_E_NONE;
 
}


/** see .h file */
int periodic_event_destroy(periodic_event_handler_t* h)
{
    int rv;

    if (h != NULL && *h != NULL) {

        periodic_event_object_t *obj = (periodic_event_object_t *)(*h);

        /* stop thread if still running.*/
        rv = periodic_event_stop(*h, 0);
        if (rv != _SHR_E_NONE) {
            LOG_ERROR(obj->bsl_module, (BSL_META("PE[%s]: Failed to stop the thread \n"), obj->name));
            return rv;
        }

        /* deallocate */
        if (obj != NULL) {
            if (obj->sema != NULL) {
                sal_sem_destroy(obj->sema);
            }

            sal_free(obj);

        }

        *h = NULL;
    }

    return _SHR_E_NONE;

}

/* this function return 1 if thread was created, 0 otherwise*/
static int is_thread_created(periodic_event_handler_t h) 
{
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    if(obj->tid == SAL_THREAD_ERROR) {
        return 0; /* tid indicate thread was not created*/
    } else {
        return 1;
    }
}

/** see .h file */
int periodic_event_is_active_get(periodic_event_handler_t h, int* is_active)
{
    *is_active = is_thread_created(h);

    return _SHR_E_NONE;
}

/** see .h file */
int periodic_event_trigger(periodic_event_handler_t h)
{
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    /* if thread isn't running - return an error*/
    if (!is_thread_created(h)) {
        LOG_ERROR(obj->bsl_module, (BSL_META("PE[%s]: Thread isn't running \n"), obj->name));
        return _SHR_E_DISABLED;
    }

    /* trigger event */
    (void)sal_sem_give(obj->sema);

    return _SHR_E_NONE;
}

/** see .h file */
int periodic_event_start(periodic_event_handler_t h) 
{
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    /* if thread is already runnig - return an error*/
    if (is_thread_created(h)) {
        LOG_ERROR(obj->bsl_module, (BSL_META("PE[%s]: Thread already created \n"), obj->name));
        return _SHR_E_EXISTS;
    }

    /* indicate we don't want to quit */
    obj->quit = 0;

    /* create the trhead */
    obj->tid = sal_thread_create(obj->name,
                      SAL_THREAD_STKSZ,
                      obj->thread_priority,
                      (void (*)(void*))periodic_event_thread,
                      (void*)obj);

    if(obj->tid == SAL_THREAD_ERROR){
        LOG_ERROR(obj->bsl_module, (BSL_META("PE[%s]: Failed to create a thread \n"), obj->name));
        return _SHR_E_MEMORY;
    }

    return _SHR_E_NONE;
}

/* 1 second */
#define PERIODIC_EVENT_THREAD_EXIT_TIMEOUT (1*SECOND_USEC) 

/** see .h file */
int periodic_event_stop(periodic_event_handler_t h, int dont_wait)
{
    int rv = _SHR_E_NONE;
    soc_timeout_t to;
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    /* if already stopped - do nothing */
    if (!is_thread_created(h)) {
        LOG_VERBOSE(obj->bsl_module, (BSL_META("PE[%s]: Thread isn't running. Nothing to do. \n"), obj->name));
        return _SHR_E_NONE;
    }

    /* indicate quit indication */
    obj->quit = 1;

    /* if waiting for thread to exit is not required - get out */
    if(dont_wait)
    {
        LOG_VERBOSE(obj->bsl_module, (BSL_META("PE[%s]: Thread will stop next time it is scheduled.\n"), obj->name));
        return _SHR_E_NONE;
    }

    /* trigger event for quit marking to take place */
    _SHR_E_IF_ERROR_RETURN(periodic_event_trigger(h));

    /* wait for thread to exit */
    soc_timeout_init(&to, PERIODIC_EVENT_THREAD_EXIT_TIMEOUT, 100);

    while (is_thread_created(h)) {
        if (soc_timeout_check(&to)) {
            LOG_WARN(obj->bsl_module, (BSL_META("PE[%s]: Failed to gracefully turn off thread. Will terminate it. \n"), obj->name));
            rv = _SHR_E_TIMEOUT;
            break;
        }
    }

    return rv;
}

/** see .h file */
int periodic_event_interval_set(periodic_event_handler_t h, int interval)
{
    int is_same_thread;
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    if (interval <= 0 && interval != sal_sem_FOREVER) {
        LOG_ERROR(obj->bsl_module, (BSL_META("PE[%s]: Interval must be > 0\n"), obj->name));
        return _SHR_E_CONFIG;
    }

    /* update interval */
    obj->interval = interval;

    /* trigger the event for new interval to take affect*/
    if (is_thread_created(h)) {
        _SHR_E_IF_ERROR_RETURN(periodic_event_is_periodic_context_get(h, &is_same_thread));
        if (!is_same_thread) {
            _SHR_E_IF_ERROR_RETURN(periodic_event_trigger(h));
        }
    }

    return _SHR_E_NONE;
}

/** see .h file */
int periodic_event_interval_get(periodic_event_handler_t h, int* interval)
{
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    *interval = obj->interval;

    return _SHR_E_NONE;
}

/** see .h file */
int periodic_event_is_periodic_context_get(periodic_event_handler_t h, int* is_periodic_context)
{
    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    *is_periodic_context = (obj->tid == sal_thread_self() ? 1 : 0);

    return _SHR_E_NONE;
}

/** see .h file */
int periodic_event_is_quit_signaled_get(periodic_event_handler_t h, int* is_quit_signaled)
{

    periodic_event_object_t *obj = (periodic_event_object_t*)h;

    *is_quit_signaled = obj->quit;

    return _SHR_E_NONE;

}


