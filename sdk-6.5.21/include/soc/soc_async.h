/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * File:        soc_async.h
 * Purpose:     This file implements the async frame work
 *              used by various soc cimix modules. Async framework
 *              facilitates the message queuing and call back notification
 *              mechanism is multi threaded environment.
 */

#ifndef _SOC_ASYNC_H
#define _SOC_ASYNC_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <soc/error.h>

/* Enum representing async proc type */

typedef enum soc_async_proc {
    p_sbus_dma = 0,
    p_packet_dma,
    p_fifo_dma,
    p_schan_fifo,
    p_schan_pio
} soc_async_p_type;


/* typedefs */

/*DMA/ SCHAN message processing function */
typedef int (*soc_async_msg_proc_f)(int unit, void *data, void *cookie);

/* Wait to  complete the message process either
 * interrupt based or polling based.
 */
typedef int (*soc_async_wait_complete_f)(int unit, void *data, void *cookie, uint32 do_not_free_channel);

/* Callback function pointer passed by the upper
 * layer need to be called to notify on completion.
 */
typedef void (*soc_async_cb_f)(int unit, void *data, void *cookie, int status);

/* Handle returned from the Async frame work init function */
typedef  void *soc_async_handle_t;

/* This message structure will be populated by subsystem
 * e.g. SBUS, SCHAN, Packet DMA etc to queue the message
 * to the associated queue for async handling.
 */
typedef struct {
    int unit;                          /* Unit */
    soc_async_p_type type;             /* Message queue type */
    void *data;                        /* abstract data for processing */
    void *cookie;                      /* abstract cookie, application/driver use */
    soc_async_msg_proc_f   proc_f;     /* message processing function */
    soc_async_wait_complete_f wait_f;  /* wait to complete processing */
    soc_async_cb_f  cb_f;              /* Application call back function */
}soc_async_msg_t;

typedef struct {
    soc_async_p_type type;     /*Message queue type */
    size_t           q_size;   /* queue size */
    unsigned int     threads;  /* # of threads */
    int              prio;     /* Priority of threads */
}soc_async_prop_t;

/*******************************************
* @function soc_async_proc_init
* @purpose  Initialize the async proc.
*
* @param unit [in] unit number
* @param prop [in] property of  <soc_async_prop_t>
*@param prop [out] property of  <soc_async_handle_t>, This handle is required
*                               to access async access functions
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments The function will allocate resources , create thread safe
*  queues and processing threads. This will return handle to caller
*
* @end
 */
extern int soc_async_proc_init(
              int unit,
              soc_async_prop_t   *prop,
              soc_async_handle_t *handle);

/*******************************************
* @function soc_async_msg_queue
* purpose Queue the message
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param msg [in] message pointer

* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments This will queue the message "soc_async_msg_t" and
   trigger the event so that associated thread  is unblocked,
   dequeue and process the message.
*
* @end
 */
extern int soc_async_msg_queue(
                  soc_async_handle_t handle,
                  soc_async_msg_t *msg);

/*******************************************
* @function soc_async_msg_dequeue
* purpose Dequeue the message
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param msg [out] message pointer to get the message
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments The processing Threads will call this function to
*  dequeue the message from the thread safe message queue to
*  process it further.
*
* @end
 */
extern int soc_async_msg_dequeue(
                  soc_async_handle_t handle,
                  soc_async_msg_t **msg);

/*******************************************
* @function soc_async_flush_queue
* purpose Flush the message queue
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments Discard all the messages in the queue and make it empty/reset.
*
* @end
 */
extern int soc_async_flush_queue(soc_async_handle_t handle);


/*******************************************
* @function soc_async_proc_deinit
* purpose Deinit async proc.
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments The function will destroy message queues and processing
*  threads, unbind associated channels and free resources.
*
* @end
 */
extern int soc_async_proc_deinit(soc_async_handle_t handle);

/*******************************************
* @function soc_async_msg_count
* purpose Get the number of messages in the queue
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param count [out] returns message count
*
* @returns SOC_E_NONE
*
* @end
 */
extern int soc_async_msg_count(soc_async_handle_t handle, int *count);

/*******************************************
* @function soc_async_msg_alloc
* purpose Allocate the message
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param msg [in] pointer to pointer of message

* @returns SOC_E_NONE
* @returns SOC_E_MEMORY
*
* @end
 */
extern int soc_async_msg_alloc(
                  soc_async_handle_t handle,
                  soc_async_msg_t **msg);

/*******************************************
* @function soc_async_msg_free
* purpose Free the previously allocated the message
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param msg [in] pointer of message

* @returns SOC_E_NONE
* @returns SOC_E_PARAM
*
* @end
 */
extern int soc_async_msg_free(
                  soc_async_handle_t handle,
                  soc_async_msg_t *msg);

/*******************************************
* @function soc_async_msg_start
* purpose start the message queue processing
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_async_msg_start(soc_async_handle_t handle);

/*******************************************
* @function soc_async_msg_stop
* purpose Stop the message queue processing
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_async_msg_stop(soc_async_handle_t handle);


#endif


