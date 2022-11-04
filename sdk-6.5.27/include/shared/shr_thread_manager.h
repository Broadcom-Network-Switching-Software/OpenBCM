
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    shr_thread_manager.h
 * Purpose:     Generic mechanisem for threads.
 */

/**
 *  
 * The purpose of this package is to provide an easy way to 
 * create a threads.
 *  
 * A thread can be created by using
 * shr_thread_manager_create, by giving: 
 *  -. name
 *  -. interval
 *  -. callback for periodic execution.
 *  
 * Additional facilities given by this package:
 *  -. Having the ability to trigger the thread for immediate
 *   operation.
 *  -. Having the ability to start\stop the thread.
 *  -. Having the ability to send void* user data for the
 *   callback usage.
 *  -. Providing thread priority.
 *  -. Having error threshold - if the callback failed X time
 *   thread will automatically exit.
 *  
 * Notes:
 *  -. To free allocated memory shr_thread_manager_destroy should be
 *   called when thread is no longer required, or upon deinit.
 *  -. Non of the information is WB protected - thread
 *   must be re-created after WB.
 *  
 *   */

#ifndef _SHR_SHR_THREAD_MANAGER_H
#define _SHR_SHR_THREAD_MANAGER_H

#include <sal/types.h>
#include <shared/error.h>

#define SHR_THREAD_MANAGER_MAX_NAME_SIZE 30

/*
 * Maximum timeout value we may have. This means we are going to wait forever.
 */
#define SHR_THREAD_MANAGER_MAX_TIMEOUT -1

/**
 * \brief - Handler for thread operations.
 * To create a handle use  shr_thread_manager_create function.
 */
typedef void* shr_thread_manager_handler_t;

/**
 * \brief - Callback definition for thread
 */
typedef _shr_error_t (*shr_thread_manager_f)(int unit, void* user_data);

/*
 * \brief - Enum with all types of events.
 */
typedef enum
{
    SHR_THREAD_MANAGER_TYPE_INVALID = -1,
    SHR_THREAD_MANAGER_TYPE_FIRST = 0,
    /**
     * The supplied function will run periodically with the interval that was specified on creation /see shr_thread_manager_config_t
     * or once every time shr_thread_manager_trigger is called. The Interval field would be ignored.
     * Set the interval to sal_sem_FOREVER if the event should happen upon trigger only.
     */
    SHR_THREAD_MANAGER_TYPE_EVENT = SHR_THREAD_MANAGER_TYPE_FIRST,
    /**
     * The supplied function Will run once, when the thread is created, the thread will be terminated when the function returns
     */
    SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION,
    /**
     * Must be last one!
     */
    SHR_THREAD_MANAGER_TYPE_COUNT
} shr_thread_manager_type_e;

/**
 * \brief - Thread configuration
 */
typedef struct shr_thread_manager_config_s {
    /**
     * Thread name
     */
    char* name; 

    /**
     * BSL module that will be used for logging messages. 
     * It's very recommended to use meaningful bsl module from 
     * bslenum.h modules. 
     */
    uint32 bsl_module;

    /**
     * periodic interval in microseconds (usec)
     * a value of sal_sem_FOREVER can be used to wait for trigger,
     * instead of having periodic event. 
     * (see sal/core/sync.h) 
     *  */ 
    int interval;

    /**
     * Callback for the thread operation
     */
    shr_thread_manager_f callback;

    /**
     * The user data will be passed to the thread callback as is.
     * shr_thread_manager_config_t_init initialize user_data to
     * null. 
     */
    void* user_data;

    /**
     * Thread scheduling priority (0 = highest, 255 = lowest) 
     * shr_thread_manager_config_t_init initialize this parameter 
     * to default 50. (see sal/core/thread.h) 
     */
    int thread_priority;

    /**
     * When callback returns an error more than error_threshold in a
     * raw the thread will exit. 
     * A value of -1 can be used to ignore callback errors. 
     * shr_thread_manager_config_t_init initialize error_threadhold to 
     * -1 
       */
    int error_threshold;

    /**
     * Indicates whether to start running the thread as
     * part of create. 
     * shr_thread_manager_config_t_init initialize this parameter to 
     * default 1. 
     */
    int start_operation;
    /**
     * Thread stack size.
     * default SAL_THREAD_STKSZ.
     */
    uint32 thread_stack_size;
    /** 
     * Indicated the timeout for stopping the thread in usec.
     * The timeout will be used when shr_thread_manager_stop is called
     * default 1s set by /ref shr_thread_manager_config_t_init.
     */
    uint32 stop_timeout_usec;
    /**
     * The ype of the thread.
     */
    shr_thread_manager_type_e type;
    /*
     * Marks the thread as joinable see \ref shr_thread_manager_join.
     * Can be used only for threads of type=SHR_THREAD_MANAGER_TYPE_RUN_TO_COMPLETION
     * When a thread is joinable, the thread will still be terminated upon completion
     * but indication will be kept that the thread was completed, when shr_thread_manager_join
     * when shr_thread_manager_join is called, it will wait for the thread to complete and then destroy
     * it, if thread was already completed it will destroy it and return immediately
     */ 
    uint8 joinable;
} shr_thread_manager_config_t;

/**
 * \brief - Thread configuration
 */
typedef struct shr_thread_manager_info_s {
    /*
     * Thread name
     */
    char name[SHR_THREAD_MANAGER_MAX_NAME_SIZE];

    /*
     * periodic interval in microseconds (usec)
     * a value of sal_sem_FOREVER can be used to wait for trigger,
     * instead of having periodic event.
     * (see sal/core/sync.h)
     */
    int interval;

    /*
     * Indicate whether the thread has been signaled to quit.
     */
    int is_quit_signaled;

    /*
     * Is thread active
     */
    int is_active;
} shr_thread_manager_info_t;

/**
 * \brief - initialize shr_thread_manager_config_t struct
 * \param [out] config - the structure to be initialized.
 * \return
 *   See shr_error_e
 * \remarks
 *   * Name, interval, callback and bsl_module must be set after
 *     struct initialization. The rest of the parameters are set with
 *     usable default.
 * \see
 *   * None
 */
void shr_thread_manager_config_t_init(shr_thread_manager_config_t* config);

/**
 * \brief - Create thread
 * \param [in] unit - unit #. 
 * \param [in] config - see shr_thread_manager_config_t for details. 
 * \param [out] h - handler for further operations on this thread object.
 * \return
 *   See shr_error_e
 * \remarks
 *   * This operation allocates memory. To free the allocated
 *     memory use shr_thread_manager_destroy.
 *   * This operation creates a thread per create call.
 * \see
 *   * None
 */
int shr_thread_manager_create(int unit, const shr_thread_manager_config_t* config, shr_thread_manager_handler_t* h);

/**
 * \brief - destroy the thread.
 */
int shr_thread_manager_destroy(shr_thread_manager_handler_t* h);

/**
 * \brief - trigget the thread operation.
 * \note cannot be called for RUN_TO_COMPLETION type threads.
 */
int shr_thread_manager_trigger(shr_thread_manager_handler_t h);

/**
 * \brief - start the thread.
 * \note for RUN_TO_COMPLETION type threads start is only allowed by the manager module itself.
 */
int shr_thread_manager_start(shr_thread_manager_handler_t h);

/**
 * \brief - stop the thread.
 * \param [in] dont_wait -  indicate the thread to stop the next time it is triggered,
 *                          don't wait for it to trigger.
 *                          * if dont_wait is 1 the thread will stop next time it is triggered. 
 *                          * if dont_wait is 0 the thread will wait till it is triggered. 
 * \note the function will return error in case thread can't 
 *       exit gracefully.
 * \note cannot be called for RUN_TO_COMPLETION type threads.
 */
int shr_thread_manager_stop(shr_thread_manager_handler_t h, int dont_wait);

/**
 * \brief - join the thread. The code will call /ref shr_thread_manager_destroy.
 * \note the thread must be declared as joinable in shr_thread_manager_config_t
 * \note the function will return error in case thread can't
 *       join gracefully.
 */
int shr_thread_manager_join(shr_thread_manager_handler_t* h, uint32 timeout);

/**
 * \brief - wait for thread callback to finish.
 * \param [in] h - handler for thread object.
 * \param [in] timeout - maximum time to wait for the callback to be executed.
 * \note the function will return error in case thread can't 
 *       execute the cb in time.
 */
int shr_thread_manager_wait(shr_thread_manager_handler_t h, uint32 timeout);
/**
 * \brief - determines whether event is active.
 */
int shr_thread_manager_is_active_get(shr_thread_manager_handler_t h, int* is_active);

/**
 * \brief - update event interval 
 * \param [in] trigger_on_interval_update - trigger on interval 
 *      update so it'll take immediate effect. Note: when
 *      previous interval is forever - trigger will be done
 *      anyway
 * \param [in] interval - See interval on 
 *        shr_thread_manager_config_t for details)
 * \note cannot be called for RUN_TO_COMPLETION type threads.
 */
int shr_thread_manager_interval_set(shr_thread_manager_handler_t h, int trigger_on_interval_update, int interval);

/**
 * \brief - get event interval 
 * \note cannot be called for RUN_TO_COMPLETION type threads.
 */
int shr_thread_manager_interval_get(shr_thread_manager_handler_t h, int* interval);

/**
 * \brief - whether current thread is in the the one specified in the input handler 
 * (means it was called from the thread's callback)
 * \param [out] is_periodic_context - 1: thread context 0:not thread context
 */
int shr_thread_manager_context_get(shr_thread_manager_handler_t h, int* is_thread_context);

/**
 * \brief - Indicate whether the thread has been signaled to quit.
 * \note cannot be called for RUN_TO_COMPLETION type threads.
 */
int shr_thread_manager_is_quit_signaled_get(shr_thread_manager_handler_t h, int* is_quit_signaled);

/**
 * \brief - Returns the count of created threads.
 */
int shr_thread_manager_count_get(int unit);

/**
 * \brief - Gets information for the created threads. The function will get the information
 *     for up to num_of_elements threads (not necessarily all threads)
 * \param [in] unit - unit.
 * \param [in,out] num_of_elements - Number of threads.
 *   As input:
 *     Caller provides number of expected threads
 *   As output
 *     Saves the actual number of threads. It is used for verification.
 * \param [out] info - Pointer to memory in which the thread's info will be saved.
 *      The user is responsible to allocated the memory.
 * \return
 *   See shr_error_e
 * \remarks
 *   None
 * \see
 *   * None
 */
int shr_thread_manager_info_data_get(int unit, int* num_of_elements, shr_thread_manager_info_t *info);

/**
 * \brief - Gets thread handler by given name.
 * \param [in] unit - unit.
 * \param [in] name - Thread name
 * \param [out] h - handler for further operations on this thread object.
 * \return
 *   See shr_error_e
 * \remarks
 *   None
 * \see
 *   * None
 */
int shr_thread_manager_get_by_name(int unit, char *name, shr_thread_manager_handler_t* h);

#endif /* _SHR_SHR_THREAD_MANAGER_H */
