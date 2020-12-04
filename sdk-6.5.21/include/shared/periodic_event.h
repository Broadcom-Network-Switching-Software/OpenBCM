
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    periodic_event.h
 * Purpose:     Generic mechanisem for periodic events.
 */

/**
 *  
 * The purpose of this package is to provide an easy way to 
 * create a thread for periodic event. 
 *  
 * A periodic event can be created by using 
 * periodic_event_create, by giving: 
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
 *  -. To free allocated memory periodic_event_destroy should be
 *   called when periodic is no longer required, or upon deinit.
 *  -. Non of the information is WB protected - periodic event
 *   must be re-created after WB.
 */

#ifndef _SHR_PERIODIC_EVENT_H
#define _SHR_PERIODIC_EVENT_H

#include <sal/types.h>
#include <shared/error.h>

/**
 * \brief - Handler for periodic event operations. 
 * To create a handle use  periodic_event_create function.
 */
typedef void* periodic_event_handler_t;

/**
 * \brief - Callback definition for periodic event
 */
typedef _shr_error_t (*periodic_event_f)(int unit, void* user_data);

/**
 * \brief - Periodic Event configuration 
 */
typedef struct periodic_event_config_s {
    /**
     * Periodic event name
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
 */ 
    int interval;

    /**
     * Callback for the periodic operation
     */
    periodic_event_f callback;

    /**
     * The user data will be passed to the periodic callback as is. 
     * periodic_event_config_t_init initialize user_data to
     * null. 
     */
    void* user_data;

    /**
     * Thread scheduling priority (0 = highest, 255 = lowest) 
     * periodic_event_config_t_init initialize this parameter 
     * to default 50. (see sal/core/thread.h) 
     */
    int thread_priority;

    /**
     * When callback returns an error more than error_threshold in a
     * raw the thread will exit. 
     * A value of -1 can be used to ignore callback errors. 
     * periodic_event_config_t_init initialize error_threadhold to 
     * -1 
       */
    int error_threshold;

    /**
     * Indicates whether to start running the periodic thread as 
     * part of create. 
     * periodic_event_config_t_init initialize this parameter to 
     * default 1. 
     */
    int start_operation;
} periodic_event_config_t;

/**
 * \brief - initialize periodic_event_config_t struct
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
void periodic_event_config_t_init(periodic_event_config_t* config);

/**
 * \brief - Create periodic event
 * \param [in] unit - unit #. 
 * \param [in] config - see periodic_event_config_t for details. 
 * \param [out] h - handler for further operations on this
 *        periodic event object.
 * \return
 *   See shr_error_e
 * \remarks
 *   * This operation allocates memory. To free the allocated
 *     memory use periodic_event_destroy.
 *   * This operation creates a thread per create call.
 * \see
 *   * None
 */
int periodic_event_create(int unit, const periodic_event_config_t* config, periodic_event_handler_t* h);

/**
 * \brief - destroy the periodic event. 
 */
int periodic_event_destroy(periodic_event_handler_t* h);

/**
 * \brief - trigget the periodic event operation.
 */
int periodic_event_trigger(periodic_event_handler_t h);

/**
 * \brief - start the periodic event.
 */
int periodic_event_start(periodic_event_handler_t h);

/**
 * \brief - stop the periodic event. 
 * \param [in] dont_wait -  indicate the thread to stop the next time it is triggered,
 *                          don't wait for it to trigger.
 *                          * if dont_wait is 1 the thread will stop next time it is triggered. 
 *                          * if dont_wait is 0 the thread will wait till it is triggered. 
 * \note the function will return error in case thread can't 
 *       exit gracefully.
 */
int periodic_event_stop(periodic_event_handler_t h, int dont_wait);

/**
 * \brief - determines whether event is active.
 */
int periodic_event_is_active_get(periodic_event_handler_t h, int* is_active);

/**
 * \brief - update event interval 
 * (See interval on periodic_event_config_t for details) 
 */
int periodic_event_interval_set(periodic_event_handler_t h, int interval);

/**
 * \brief - get event interval 
 */
int periodic_event_interval_get(periodic_event_handler_t h, int* interval);

/**
 * \brief - whether current thread is in the periodic context 
 * (means it was called from periodic event callback)
 */
int periodic_event_is_periodic_context_get(periodic_event_handler_t h, int* is_periodic_context);

/**
 * \brief - Indicate whether the periodic event has been 
 *        signaled to quit.
 */
int periodic_event_is_quit_signaled_get(periodic_event_handler_t h, int* is_quit_signaled);

#endif /* _SHR_PERIODIC_EVENT_H */
