/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    shr_thread_manager.c
 * Purpose:     Generic mechanisem for threads.
 */

#include <shared/shr_thread_manager.h>
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

/* 
 
Note about termonology of objects and handlers in this file: 
------------------------------------------------ 
shr_thread_manager_handler_t (which is void*) and shr_thread_manager_object_t* 
are the same entiry. 
*/

/* THREAD_MANAGER internal description - shouldn't be exposed to shared thread manager user*/
typedef struct shr_thread_manager_object_s {
    int unit;                                       /* unit # */
    char name[SHR_THREAD_MANAGER_MAX_NAME_SIZE];    /* name given by the user (see shr_thread_manager.h)*/
    shr_thread_manager_f callback;                  /* callback provided by the user (see shr_thread_manager.h)*/
    void* user_data;                                /* user data pointer provided by the user (see shr_thread_manager.h)*/
    sal_sem_t sema;                                 /* Semaphore handler for shr_thread_manager_thread usage */
    volatile int interval;                          /* interval provided by the user (see shr_thread_manager.h)*/
    int quit;                                       /* indication from main thread whether thread should be exit */
    int thread_priority;                            /* thread priority provided by the user (see shr_thread_manager.h)*/
    volatile sal_thread_t tid;                      /* created thread ID */
    sal_mutex_t tid_mtx;                            /* Mutex to protect the tid variable */
    sal_mutex_t is_active_mtx;                      /* Indicator if the thread is active (locked when active) */
    volatile int is_active;                         /* Indicator if the thread is active (volatile to prevent optimizations)*/
    sal_mutex_t is_running_mtx;                     /* Indicator if the thread is running (locked when active) */
    volatile int is_running;                        /* Indicator if the thread is running (volatile to prevent optimizations)*/
    int error_threshold;                            /* error count threshold provided by the user (see shr_thread_manager.h)*/
    uint32 bsl_module;                              /* BSL module for logging messages */
    uint32 thread_stack_size;                       /* stack size of the thread (see shr_thread_manager.h)*/
    uint32 stop_timeout_usec;                       /* Indicated the timeout for stopping the thread in usec. */
    struct shr_thread_manager_object_s *next;       /*! Next thread in global thread list. */
    shr_thread_manager_type_e type;                 /* indicates the thread's type. */
    uint8 joinable;                                 /* Marks the thread as joinable  (see shr_thread_manager.h)*/
    int thread_failure_code;                        /* The error code resulting from the most recent callback invocation. */
} shr_thread_manager_object_t;

/* Callback type for the wait callback used in shr_thread_manager_timeout.*/
typedef int(
    *shr_thread_manager_wait_cb) (
    shr_thread_manager_handler_t h);

/* This function performs busy wait with a given timeout*/
static int
shr_thread_manager_timeout(shr_thread_manager_handler_t h, uint32 timeout, shr_thread_manager_wait_cb wait_cb)
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /*
     * If the timeout is not the shr thread manager maximum timeout we are going to use it.
     */
    if (timeout != SHR_THREAD_MANAGER_MAX_TIMEOUT)
    {
        soc_timeout_t to;

        /*
         * wait for the cb to return 0. If the timeout is reached return error.
         */
        soc_timeout_init(&to, timeout, 100);
        while(wait_cb(h))
        {
            if (soc_timeout_check(&to)) {
                LOG_WARN(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to wait the thread to finish.\n"), obj->name));
                return _SHR_E_TIMEOUT;
            }
        }
    }
    else
    {
        /*
         * Otherwise we should wait for the cb to return 0 forever.
         */
        while(wait_cb(h))
        {
            sal_usleep(1);
        }
    }

    return obj->thread_failure_code;
}

/*
 * Used for thread protection of adding/removing nodes in/from thread_root.
 * IMPORTANT: Please note that the first add and the last remove are not thread safe.
 */
static sal_mutex_t thread_lock[SOC_MAX_NUM_DEVICES];
/*
 * List of all active threads.
 */
static shr_thread_manager_object_t *thread_root[SOC_MAX_NUM_DEVICES];

/** This function will be called from shr_thread_manager_destroy to remove a event
 *  from the thread_root variable.
 *  The function operation:
 *   1. Lock the resource(thread_root).
 *   2. Remove the event of the global list(thread_root).
 *   3. Unlock the resource(thread_root).
 *   4. Destroy the protection mutex, if there are no more entries in the thread_root list.
 *   2. Free the allocated memory for the thread.
 *  */
static int
shr_thread_manager_ctrl_destroy(int unit, shr_thread_manager_object_t *shr_thread_manager_control)
{
    shr_thread_manager_object_t **list;

    if (!SOC_UNIT_VALID(unit))
    {
        return _SHR_E_UNIT;
    }

    if (shr_thread_manager_control == NULL)
    {
        return _SHR_E_MEMORY;
    }

    if (thread_lock[unit] == NULL)
    {
        return _SHR_E_MEMORY;
    }

    sal_mutex_take(thread_lock[unit], sal_mutex_FOREVER);
    list = &thread_root[unit];
    while (*list)
    {
        if (*list == shr_thread_manager_control)
        {
            *list = shr_thread_manager_control->next;
            break;
        }
        list = &((*list)->next);
    }
    sal_mutex_give(thread_lock[unit]);

    if (thread_root[unit] == NULL)
    {
        sal_mutex_destroy(thread_lock[unit]);
        thread_lock[unit] = NULL;
    }

    sal_mutex_destroy(shr_thread_manager_control->is_active_mtx);
    sal_mutex_destroy(shr_thread_manager_control->is_running_mtx);
    sal_mutex_destroy(shr_thread_manager_control->tid_mtx);

    sal_free(shr_thread_manager_control);

    return _SHR_E_NONE;
}

/** This function will be called from shr_thread_manager_create to insert the event
 *  in the thread_root variable.
 *  The function operation:
 *   1. Create the protection mutex, if needed.
 *   2. Allocate memory for the thread.
 *   3. Lock the resource(thread_root).
 *   4. Insert the event to the global list(thread_root).
 *   5. Unlock the resource(thread_root).
 *  */
static shr_thread_manager_object_t *
shr_thread_manager_ctrl_create(int unit, char *event_name)
{
    shr_thread_manager_object_t *shr_thread_manager_control;

    /*
     * Check if the unit is valid.
     */
    if (!SOC_UNIT_VALID(unit))
    {
        return NULL;
    }

    /*
     * The first create should create the protection mutex.
     */
    if (thread_root[unit] == NULL)
    {
        thread_lock[unit] = sal_mutex_create("shr_thread_manager_lock");
    }

    /*
     * Check if the mutex is created.
     */
    if (thread_lock[unit] == NULL)
    {
        return NULL;
    }

    /*
     * Allocate memory for the thread
     */
    shr_thread_manager_control = sal_alloc(sizeof(*shr_thread_manager_control), event_name);
    if (shr_thread_manager_control == NULL)
    {
        return NULL;
    }

    sal_memset(shr_thread_manager_control, 0, sizeof(*shr_thread_manager_control));

    /*
     * Create a mutex for is_active indication.
     */
    shr_thread_manager_control->is_active_mtx = sal_mutex_create("shr_thread_manager_is_active_lock");
    shr_thread_manager_control->is_running_mtx = sal_mutex_create("shr_thread_manager_is_running_lock");
    shr_thread_manager_control->tid_mtx = sal_mutex_create("shr_thread_manager_tid_lock");
    if (shr_thread_manager_control->is_active_mtx == NULL
        || shr_thread_manager_control->is_running_mtx == NULL
        || shr_thread_manager_control->tid_mtx == NULL)
    {
        sal_free(shr_thread_manager_control);
        return NULL;
    }

    /*
     * insert the event to the global list.
     */
    sal_mutex_take(thread_lock[unit], sal_mutex_FOREVER);
    shr_thread_manager_control->next = thread_root[unit];
    thread_root[unit] = shr_thread_manager_control;
    sal_mutex_give(thread_lock[unit]);

    return shr_thread_manager_control;
}

/**
 * Mark the thread as active/inactive, protected by mutex as a mean to gurantee synchronization between the threads
 * */
static void shr_thread_manager_is_active_set(shr_thread_manager_object_t* obj, int is_active)
{
    sal_mutex_take(obj->is_active_mtx, sal_mutex_FOREVER);
    obj->is_active = is_active;
    sal_mutex_give(obj->is_active_mtx);
}

/**
 * Mark the thread as running/not-running, protected by mutex as a mean to guarantee synchronization between the threads
 * */
static void shr_thread_manager_is_running_set(shr_thread_manager_object_t* obj, int is_running)
{
    sal_mutex_take(obj->is_running_mtx, sal_mutex_FOREVER);
    obj->is_running = is_running;
    sal_mutex_give(obj->is_running_mtx);
}

/**
 * Simply return the thread interval.
 */
static int shr_thread_manager_interval_internal_get(shr_thread_manager_object_t* obj)
{
    return obj->interval;
}

/**
 * Simply return the thread ID.
 */
static sal_thread_t shr_thread_manager_id_get(shr_thread_manager_object_t* obj)
{
    return obj->tid;
}

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
 * 
 *  */
static void shr_thread_manager_thread(shr_thread_manager_object_t* obj)
{
    int rv, err_count = 0, interval;

    if (obj == NULL)
    {
        sal_thread_exit(0);
        return;
    }

    LOG_VERBOSE(obj->bsl_module, (BSL_META("THREAD_MANAGER: Thread is starting.\n")));

    /* iterate until indicate to exit */
    while (!obj->quit) {

        /* When thread's type is RUN_TO_COMPLETION it should start immediately */
        if (obj->type != SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
        {
            interval = shr_thread_manager_interval_internal_get(obj);
            /* wait for trigger or timeout */
            (void)sal_sem_take(obj->sema, interval);

            /* break out if we're told to quit */
            if (obj->quit == 1) {
                break;
            }
        }
        /* Signal that the thread callback is running */
        shr_thread_manager_is_running_set(obj, TRUE);

        /* run callback */
        rv = obj->callback(obj->unit, obj->user_data);

        /* Signal that the thread callback is done */
        shr_thread_manager_is_running_set(obj, FALSE);

        /* RUN_TO_COMPLETION thread exists immediately after first callback invocation */
        if (obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
        {
            if (_SHR_E_FAILURE(rv)) {
                LOG_WARN(obj->bsl_module, (BSL_META("THREAD_MANAGER: Failure occured on thread %s. rv: %d\n"), obj->name, rv));
                obj->thread_failure_code = _SHR_E_FAIL;
            }
            break;
        }

        /* update error count */
        if (_SHR_E_FAILURE(rv)) {
            LOG_WARN(obj->bsl_module, (BSL_META("THREAD_MANAGER: Callback failure occured on the %d time in a raw. rv: %d\n"), err_count, rv));
            err_count++;
        } else {
            err_count = 0;
        }

        /* Exist if passed error count threshold. 
           -1 means ignore error counting */
        if (err_count >= obj->error_threshold && obj->error_threshold != -1) {
            LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER: Error threshold acceded. Thread will exit.\n")));
            break;
        }
    }

    LOG_VERBOSE(obj->bsl_module, (BSL_META("THREAD_MANAGER: Thread is exiting.\n")));

    /* in case thread is RUN_TO_COMPLETION, we destroy it but keep the handle alive (if clause). If thread is joinable (else clause) destruction is done at the join() function */
    if (obj != NULL && obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION && !obj->joinable)
    {
        /* mark the thread as inactive */
        shr_thread_manager_is_active_set(obj, 0);

        /* destroy the object - no one will need it anymore */
        if (shr_thread_manager_ctrl_destroy(obj->unit, obj))
        {
            LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to destroy the thread control.\n"), obj->name));
        }
    }
    else
    {
        sal_mutex_take(obj->tid_mtx, sal_mutex_FOREVER);
        /* in case obj was not destroyed by this routine indicate thread isn't active anymore */
        obj->tid = SAL_THREAD_ERROR;
        sal_mutex_give(obj->tid_mtx);

        /* is_active is used as an indicator volatile flag (it is marked with the volatile keyword), volatile hints the compiler that this variable may unexpectedly be changed by a different thread so it must be read from memory and not from cache*/
        shr_thread_manager_is_active_set(obj, 0);
    }

    /* exit thread */
    sal_thread_exit(0);
}

/** see .h file */
void shr_thread_manager_config_t_init(shr_thread_manager_config_t* config)
{
    sal_memset(config, 0, sizeof(shr_thread_manager_config_t));

    /* set defaults */
    config->thread_priority = 50;
    config->error_threshold = -1;
    config->start_operation = 1;
    config->bsl_module = -1;
    config->thread_stack_size = SAL_THREAD_STKSZ;
#if (VALGRIND_SUPPORT == 1)
    config->stop_timeout_usec = (10 * SECOND_USEC);
#else
    config->stop_timeout_usec = (1 * SECOND_USEC);
#endif    
    config->type = SHR_THREAD_MANAGER_TYPE_INVALID;
    config->joinable = FALSE;
}

/** see .h file */
int shr_thread_manager_create(int unit, const shr_thread_manager_config_t* config, shr_thread_manager_handler_t* h)
{
    int rv;
    shr_thread_manager_object_t *obj;

    *h = NULL;

    /* check mandatory configs*/
    if (config->bsl_module == -1) {
        return _SHR_E_CONFIG;
    }

    if (config->name[0] == 0) {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER: Name is mandatory\n")));
        return _SHR_E_CONFIG;
    }

    if (config->type == SHR_THREAD_MANAGER_TYPE_INVALID)
    {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Thread's type must be specified.\n"), config->name));
        return _SHR_E_CONFIG;
    }

    if (config->type == SHR_THREAD_MANAGER_TYPE_EVENT && config->interval < 0 && config->interval != sal_sem_FOREVER) {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Interval must be >= 0\n"), config->name));
        return _SHR_E_CONFIG;
    }

    if (config->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION && config->interval != 0) {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Interval must be 0 if thread's type is RUN_TO_COMPLETION\n"), config->name));
        return _SHR_E_CONFIG;
    }

    if (config->callback == NULL) {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Callback must be assigned \n"), config->name));
        return _SHR_E_CONFIG;
    }

    if (!config->start_operation && config->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION) {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: RUN_TO_COMPLETION thread must start immediately \n"), config->name));
        return _SHR_E_CONFIG;
    }


    if (config->joinable == TRUE && config->type != SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
    {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: All joinable threads should be with type RUN_TO_COMPLETION.\n"), config->name));
        return _SHR_E_CONFIG;
    }

    if (!SOC_UNIT_VALID(unit))
    {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: unit should be valid \n"), config->name));
        return _SHR_E_UNIT;
    }

    obj = shr_thread_manager_ctrl_create(unit, config->name);
    if (obj == NULL) {
        LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to allocate memory\n"), config->name));
        return _SHR_E_MEMORY;
    }

    obj->unit = unit;
    sal_strncpy(obj->name,
                config->name,
                (sal_strnlen(config->name, SHR_THREAD_MANAGER_MAX_NAME_SIZE-1)));
    obj->callback = config->callback;
    obj->user_data = config->user_data;
    obj->interval = (config->type == SHR_THREAD_MANAGER_TYPE_EVENT) ? config->interval : sal_sem_FOREVER;
    obj->quit = 0;
    obj->tid = SAL_THREAD_ERROR;
    obj->is_active = 0;
    obj->is_running = 0;
    obj->thread_priority = config->thread_priority;
    obj->error_threshold = config->error_threshold;
    obj->bsl_module = config->bsl_module;
    obj->thread_stack_size = config->thread_stack_size;
    obj->stop_timeout_usec = config->stop_timeout_usec;
    obj->type = config->type;
    obj->joinable = config->joinable;

    /* initialize semaphore */
    if (obj->type != SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
    {
        obj->sema = sal_sem_create(config->name, sal_sem_BINARY, 0);
        if (obj->sema == NULL) {
            LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to allocate semaphore\n"), config->name));
            sal_free(obj);
            obj = NULL;
            return _SHR_E_MEMORY;
        }
    }

    /* init the thraed if requested */
    if (config->start_operation) {
        rv = shr_thread_manager_start((shr_thread_manager_handler_t)obj);
        if (_SHR_E_FAILURE(rv)) {
            LOG_ERROR(config->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to start event\n"), config->name));
            if (obj->type != SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
            {
                sal_sem_destroy(obj->sema);
            }
            sal_free(obj);
            return rv;
        }
    }
  
    *h = (shr_thread_manager_handler_t)obj;

    /* in the case below there is no need for a handle, return NULL */
    if (obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION && !obj->joinable)
    {
        *h = NULL;
    }

    return _SHR_E_NONE;
 
}


/** see .h file */
int shr_thread_manager_destroy(shr_thread_manager_handler_t* h)
{
    int rv;

    if (h != NULL && *h != NULL) {

        shr_thread_manager_object_t *obj = (shr_thread_manager_object_t *)(*h);

        if (obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
        {
            LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: RUN_TO_COMPLETION threads should not call destroy.\n"), obj->name));
            return _SHR_E_PARAM;
        }

        /* stop thread if still running.*/
        rv = shr_thread_manager_stop(*h, 0);
        if (rv != _SHR_E_NONE) {
            LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to stop the thread \n"), obj->name));
            return rv;
        }

        /* deallocate */
        if (obj != NULL) {
            if (obj->sema != NULL) {
                sal_sem_destroy(obj->sema);
            }

            rv = shr_thread_manager_ctrl_destroy(obj->unit, obj);
            if (rv != _SHR_E_NONE) {
                LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to destroy the thread control.\n"), obj->name));
                return rv;
            }

        }

        *h = NULL;
    }

    return _SHR_E_NONE;

}

/* this function return 1 if thread was created, 0 otherwise*/
static int is_thread_created(shr_thread_manager_handler_t h)
{
    int rv = 1;
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /* if obj is NULL consider thread as not created */
    if(obj == NULL) {
        return 0;
    }

    /* The obj->is_active is marked as volatile and is used instead of relying on obj->tid in order to avoid compiler's optimizations that may cause obj->tid
       to have a non up to date value if it was set by a different thread than the current thread. In addition it is protected by a mutex, mainly to gurantee that modifications to this flag by a different thread will not be optimized out by the compiler (in some systems volatile keyword alone will not gurantee it) */
    sal_mutex_take(obj->is_active_mtx, sal_mutex_FOREVER);
    if(obj->is_active == 0) {
        rv = 0;
    }

    sal_mutex_give(obj->is_active_mtx);
    return rv;

}

/* this function return 1 if thread is running, 0 otherwise*/
static int is_thread_running(shr_thread_manager_handler_t h)
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /* if obj is NULL consider thread is not running */
    if(obj == NULL) {
        return 0;
    }

    return obj->is_running;
}

/** see .h file */
int shr_thread_manager_is_active_get(shr_thread_manager_handler_t h, int* is_active)
{
    *is_active = is_thread_created(h);

    return _SHR_E_NONE;
}

/** see .h file */
int shr_thread_manager_trigger(shr_thread_manager_handler_t h)
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /* if thread isn't running - return an error*/
    if (!is_thread_created(h)) {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Thread isn't running \n"), obj->name));
        return _SHR_E_DISABLED;
    }

    if (obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
    {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: RUN_TO_COMPLETION threads cannot be triggered.\n"), obj->name));
        return _SHR_E_PARAM;
    }

    /* trigger event */
    (void)sal_sem_give(obj->sema);

    return _SHR_E_NONE;
}

/** see .h file */
int shr_thread_manager_start(shr_thread_manager_handler_t h) 
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /* if thread is already runnig - return an error*/
    if (is_thread_created(h)) {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Thread already created \n"), obj->name));
        return _SHR_E_EXISTS;
    }

    /* indicate we don't want to quit */
    obj->quit = 0;

    /* indicate the thread is active by raising this flag, in adition a mutex will be taken when the thread is created and released when it exists */
    shr_thread_manager_is_active_set(obj, 1);

    sal_mutex_take(obj->tid_mtx, sal_mutex_FOREVER);
    /* create the trhead */
    obj->tid = sal_thread_create(obj->name,
                      obj->thread_stack_size,
                      obj->thread_priority,
                      (void (*)(void*))shr_thread_manager_thread,
                      (void*)obj);

    if(shr_thread_manager_id_get(obj) == SAL_THREAD_ERROR){
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to create a thread \n"), obj->name));
        /* mark thread as inactive */
        shr_thread_manager_is_active_set(obj, 0);
        sal_mutex_give(obj->tid_mtx);
        return _SHR_E_MEMORY;
    }
    sal_mutex_give(obj->tid_mtx);

    return _SHR_E_NONE;
}

/** see .h file */
int shr_thread_manager_stop(shr_thread_manager_handler_t h, int dont_wait)
{
    int rv = _SHR_E_NONE;
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /* if already stopped - do nothing */
    if (!is_thread_created(h)) {
        LOG_VERBOSE(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Thread isn't running. Nothing to do. \n"), obj->name));
        return _SHR_E_NONE;
    }

    if (obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION)
    {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: RUN_TO_COMPLETION threads should cannot be stoped.\n"), obj->name));
        return _SHR_E_PARAM;
    }

    /* indicate quit indication */
    obj->quit = 1;

    /* if waiting for thread to exit is not required - get out */
    if(dont_wait)
    {
        LOG_VERBOSE(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Thread will stop next time it is scheduled.\n"), obj->name));
        return _SHR_E_NONE;
    }

    /* trigger event for quit marking to take place */
    _SHR_E_IF_ERROR_RETURN(shr_thread_manager_trigger(h));

    _SHR_E_IF_ERROR_RETURN(shr_thread_manager_timeout(h, obj->stop_timeout_usec, is_thread_created));

    return rv;
}


/** see .h file */
int shr_thread_manager_join(shr_thread_manager_handler_t* h, uint32 timeout)
{
    int rv = _SHR_E_NONE;
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)(*h);

    if (obj == NULL) {
        return _SHR_E_MEMORY;
    }

    if (!obj->joinable)
    {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: thread should be joinable to call join.\n"), obj->name));
        return _SHR_E_PARAM;
    }

    /* wait for the thread to lower its is_active volatile variable as an indication thread is inactive */
    _SHR_E_IF_ERROR_RETURN(shr_thread_manager_timeout(*h, timeout, is_thread_created));

    rv = obj->thread_failure_code;

    if (shr_thread_manager_ctrl_destroy(obj->unit, obj))
    {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Failed to destroy the thread control.\n"), obj->name));
    }

    *h = NULL;

    return rv;
}

/** see .h file */
int shr_thread_manager_wait(shr_thread_manager_handler_t h, uint32 timeout)
{
    int is_active = 0;
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    /* Check if the thread is active/created */
    if (shr_thread_manager_is_active_get(h, &is_active))
    {
        return _SHR_E_MEMORY;
    }

    /* If the thread is not active, simply exit with no error */
    if (is_active == FALSE)
    {
        return _SHR_E_NONE;
    }

    /* wait for the thread to lower its is_running volatile variable as an indication thread is not running */
    _SHR_E_IF_ERROR_RETURN(shr_thread_manager_timeout(h, timeout, is_thread_running));

    return obj->thread_failure_code;
}

/** see .h file */
int shr_thread_manager_interval_set(shr_thread_manager_handler_t h, int trigger_on_interval_update, int interval)
{
    int is_same_thread;
    uint32 force_trigger = 0;
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    if (obj->type != SHR_THREAD_MANAGER_TYPE_EVENT) {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Interval is only relevant for EVENT thread\n"), obj->name));
        return _SHR_E_CONFIG;
    }

    if (interval < 0 && interval != sal_sem_FOREVER) {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Interval must be >= 0\n"), obj->name));
        return _SHR_E_CONFIG;
    }

    /* if interval is the same - nothing to do */
    if(obj->interval == interval) {
        return _SHR_E_NONE;
    }

    /* if previous interval is forever - trigger is mandatory */
    if(obj->interval == sal_sem_FOREVER) {
        force_trigger  = 1;
    }

    /* update interval */
    obj->interval = interval;

    /* trigger the event for new interval to take affect
       this operation is done if requested by user (trigger_on_interval_update) or previous interval was forever (force_trigger)*/
    if(force_trigger || trigger_on_interval_update) {
        if (is_thread_created(h)) {
            _SHR_E_IF_ERROR_RETURN(shr_thread_manager_context_get(h, &is_same_thread));
            if (!is_same_thread) {
                    _SHR_E_IF_ERROR_RETURN(shr_thread_manager_trigger(h));
                }
            }
    }

    return _SHR_E_NONE;
}

/** see .h file */
int shr_thread_manager_interval_get(shr_thread_manager_handler_t h, int* interval)
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    if (obj->type != SHR_THREAD_MANAGER_TYPE_EVENT) {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: Interval is only relevant for EVENT thread\n"), obj->name));
        return _SHR_E_CONFIG;
    }

    *interval = shr_thread_manager_interval_internal_get(obj);

    return _SHR_E_NONE;
}

/** see .h file */
int shr_thread_manager_context_get(shr_thread_manager_handler_t h, int* is_thread_context)
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    *is_thread_context = (obj->tid == sal_thread_self() ? 1 : 0);

    return _SHR_E_NONE;
}

/** see .h file */
int shr_thread_manager_is_quit_signaled_get(shr_thread_manager_handler_t h, int* is_quit_signaled)
{
    shr_thread_manager_object_t *obj = (shr_thread_manager_object_t*)h;

    if (obj->type == SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION) {
        LOG_ERROR(obj->bsl_module, (BSL_META("THREAD_MANAGER[%s]: RUN_TO_COMPLETION thread cannot be signaled to quit\n"), obj->name));
        return _SHR_E_CONFIG;
    }

    *is_quit_signaled = obj->quit;

    return _SHR_E_NONE;

}

/** see .h file */
int
shr_thread_manager_count_get(int unit)
{
    shr_thread_manager_object_t *shr_thread_manager_control;
    int counter = 0;

    if (!SOC_UNIT_VALID(unit))
    {
        /*
         * return 0, if the user gave us invalid unit.
         */
        return counter;
    }

    if (thread_lock[unit])
    {
        sal_mutex_take(thread_lock[unit], sal_mutex_FOREVER);
        shr_thread_manager_control = thread_root[unit];
        while (shr_thread_manager_control)
        {
            counter++;
            shr_thread_manager_control = shr_thread_manager_control->next;
        }
        sal_mutex_give(thread_lock[unit]);
    }
    return counter;
}

/** see .h file */
int
shr_thread_manager_info_data_get(int unit, int* num_of_elements, shr_thread_manager_info_t *info)
{
    shr_thread_manager_object_t *shr_thread_manager_control;
    int idx = 0;
    int rv = _SHR_E_NONE;

    if (!SOC_UNIT_VALID(unit))
    {
        return _SHR_E_UNIT;
    }

    if (thread_lock[unit]) {
        sal_mutex_take(thread_lock[unit], sal_mutex_FOREVER);
        shr_thread_manager_control = thread_root[unit];
        idx = 0;
        while (shr_thread_manager_control)
        {
            if (idx < *num_of_elements)
            {
                sal_strncpy((info[idx]).name,
                            shr_thread_manager_control->name,
                            (sal_strnlen(shr_thread_manager_control->name,
                                         SHR_THREAD_MANAGER_MAX_NAME_SIZE-1)));
                (info[idx]).interval = shr_thread_manager_control->interval;
                (info[idx]).is_quit_signaled = shr_thread_manager_control->quit;
                rv = shr_thread_manager_is_active_get((shr_thread_manager_handler_t)shr_thread_manager_control, &((info[idx]).is_active));
                if (rv != _SHR_E_NONE)
                {
                    return rv;
                }
                shr_thread_manager_control = shr_thread_manager_control->next;
            }
            idx++;
        }
        sal_mutex_give(thread_lock[unit]);
        /*
         * Return error in case the max number of elements and actual number of elements
         *  are different.
         */
        if (*num_of_elements != idx)
        {
            rv = _SHR_E_FULL;
        }
        *num_of_elements = idx;
        return rv;
    }
    return _SHR_E_INTERNAL;
}

/** see .h file */
int
shr_thread_manager_get_by_name(int unit, char *name, shr_thread_manager_handler_t* h)
{
    shr_thread_manager_object_t *shr_thread_manager_control = NULL;
    uint8 shr_thread_manager_exist = FALSE;

    /*
     * Initialize *h to null, so it won't return garbage if the thread is not found
     */
    *h = NULL;

    if (!SOC_UNIT_VALID(unit))
    {
        return _SHR_E_UNIT;
    }

    if (thread_lock[unit])
    {
        sal_mutex_take(thread_lock[unit], sal_mutex_FOREVER);
        shr_thread_manager_control = thread_root[unit];

        while (shr_thread_manager_control)
        {
            if (!sal_strncmp(name, shr_thread_manager_control->name, sizeof(shr_thread_manager_control->name)))
            {
                *h = (shr_thread_manager_handler_t)shr_thread_manager_control;
                shr_thread_manager_exist = TRUE;
                break;
            }
            shr_thread_manager_control = shr_thread_manager_control->next;
        }
        sal_mutex_give(thread_lock[unit]);
        if (shr_thread_manager_exist == TRUE)
        {
            return _SHR_E_NONE;
        }
        else
        {
            return _SHR_E_EXISTS;
        }
    }

    return _SHR_E_INTERNAL;
}
