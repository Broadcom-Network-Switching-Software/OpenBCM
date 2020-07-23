/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc_async.c
 * Purpose:     This file implements the async frame work
 *              used by various soc cimicx modules. Async framework
 *              facilitates the message queuing and call back notification
 *              mechanism is multi threaded environment.
 */

#include <shared/bsl.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <soc/soc_async.h>
#include <soc/util.h>

#ifdef BCM_SOC_ASYNC_SUPPORT

/* This structure is associated of the thread safe queue management.
 * Each queue will be represented by structure pointer.
 */
typedef struct {
    int head;
    int tail;
    int count;
    int size;
    sal_spinlock_t  lock;
    soc_async_msg_t **msg_q;
}soc_async_queue_t;

typedef enum {
    s_stop = 0,
    s_start,
    s_wait,
    s_run,
    s_exit
} thread_state_t;

typedef enum {
    c_stop = 0,
    c_start,
    c_exit
} thread_cmd_t;

typedef struct {
    sal_thread_t       pid;
    sal_sem_t          wait_event;
    thread_state_t     state;
}async_thread_t;


/* This structure is associated with the async thread. */
typedef struct {
    int                 unit;
    soc_async_p_type    type;
    soc_async_queue_t   queue;
    int                 threads;
    thread_cmd_t        cmd;
    sal_spinlock_t      lock;
    int                 count;
    sal_sem_t           stop_sem;
    async_thread_t      *async_threads;
} soc_async_proc_t;

/*******************************************
* @function _async_queue_create
* purpose Create async message que
*
* @param size [in] size of the queue
* @param q [in] pointer to soc_async_queue_t

* @returns SOC_E_NONE
* @returns SOC_E_MEMORY
*
* @end
 */
STATIC int
_async_queue_create(int size, soc_async_queue_t *q)
{
   q->msg_q = sal_alloc(sizeof(soc_async_msg_t *)*size, "Message Q");

   if (q->msg_q == NULL) {
        return SOC_E_MEMORY;
   }

   q->lock = sal_spinlock_create("Msg Lock");

   if (q->lock == NULL) {
        sal_free(q->msg_q);
        q->msg_q = NULL;
        return SOC_E_MEMORY;
   }

   q->size = size;
   q->tail = q->head = 0;

   return SOC_E_NONE;
}

/*******************************************
* @function _async_proc_cmd_set
* purpose Set the proc command
*
* @param proc [in] pointer to soc_async_proc_t
*
* @returns None
*
* @end
 */
STATIC void
_async_proc_cmd_set(soc_async_proc_t *proc, thread_cmd_t cmd)
{
   sal_spinlock_lock(proc->lock);
   proc->cmd = cmd;
   sal_spinlock_unlock(proc->lock);
}

/*******************************************
* @function _async_proc_cmd_get
* purpose Get the proc command
*
* @param proc [in] pointer to soc_async_proc_t
*
* @returns None
*
* @end
 */
STATIC thread_cmd_t
_async_proc_cmd_get(soc_async_proc_t *proc)
{
    thread_cmd_t cmd;

    sal_spinlock_lock(proc->lock);
    cmd = proc->cmd;
    sal_spinlock_unlock(proc->lock);

    return cmd;
}

/*******************************************
* @function _async_proc_count_inc
* purpose increment message count in process.
*
* @param proc [in] pointer to soc_async_proc_t
*
* @returns None
*
* @end
 */
STATIC void
_async_proc_count_inc(soc_async_proc_t *proc)
{
    sal_spinlock_lock(proc->lock);
    proc->count++;
    sal_spinlock_unlock(proc->lock);
}

/*******************************************
* @function _async_proc_count_dec
* purpose decrement message count in process.
*
* @param proc [in] pointer to soc_async_proc_t
*
* @returns None
*
* @end
 */
STATIC void
_async_proc_count_dec(soc_async_proc_t *proc)
{
    thread_cmd_t cmd;

    sal_spinlock_lock(proc->lock);
    if (proc->count > 0) {
        proc->count--;
    }
    sal_spinlock_unlock(proc->lock);

    cmd = _async_proc_cmd_get(proc);
    if ((cmd == c_stop) && (proc->count == 0)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "Wake Proc count\n")));
        sal_sem_give(proc->stop_sem);
    }

}

/*******************************************
* @function _async_queue_destroy
* purpose Destroy the message
*
* @param q [in] pointer to soc_async_queue_t
*
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_async_queue_destroy(soc_async_queue_t *q)
{
   if (q->lock) {
      sal_spinlock_destroy(q->lock);
      q->lock = NULL;
   }

   if(q->msg_q) {
      sal_free(q->msg_q);
      q->msg_q = NULL;
   }

   return SOC_E_NONE;
}

/*******************************************
* @function async_thread_func
* purpose Process the  messages
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns None
* As DMA engines has multiple channels. In Async processing
* it is possible to utilize the channels in parallel so that
* total through put can be improved.
* So there is question that how many thread should be used.
* Because if there are more threads than a substential amount of
* time is spend in switching back and forth which will effect
* the throughput in neagtive way. On the other hand if number of
* threads are less than required than channels will be sitting
* idle even if messages are waiting to be processed.
*  It looks like the tradeoff can be achieved if number of threads
* are 50% of available channels and each thread if shecdule and if
* request are in queue get 1st messages program DMA to start processing,
* get another message if present and program DMA to start and go back to
* wait for completrtion of the first utilizing the hardware processing
* time to program . In the mean time the 1st message is more likely to
* be done and callback function can be called ASAP.
* This ping pong scheme seem to be optimum because if suppose we program
* sequecne of messages then 1st request callback function will be waiting
* even if it is done,
*
* @end
 */
STATIC void
_async_thread_msg_proc(soc_async_handle_t handle)
{
    soc_async_proc_t *proc = (soc_async_proc_t *)handle;
    soc_async_msg_t *msg[2];
    int count, rv[2];
    int id = 0;
    int state = 1;
    int n = 0;

    rv[0] = rv[1] = SOC_E_FAIL;
    msg[0] = msg[1] = NULL;
    while (1) {
        switch (state) {
           case 1:
           soc_async_msg_count(handle, &count);
           if (count) {
               /* Get the message 1 */
               rv[id] = soc_async_msg_dequeue(handle, &msg[id]);
               if (rv[id] == SOC_E_NONE) {
                   if (msg[id]->type == proc->type) {
                       /* process the message */
                       if (msg[id]->proc_f != NULL) {
                           n++;
                           _async_proc_count_inc(handle);
                           rv[id] = msg[id]->proc_f(msg[id]->unit,
                                     msg[id]->data, msg[id]->cookie);
                           id ^= 1;
                           state = 2;
                       } else {
                           /* Callback for notifcaition */
                            if (msg[id]->cb_f != NULL) {
                                msg[id]->cb_f(msg[id]->unit,
                                    msg[id]->data, msg[id]->cookie, SOC_E_UNAVAIL);
                            }
                            soc_async_msg_free(handle, msg[id]);
                            msg[id] = NULL;
                       }
                   } else {
                       soc_async_msg_free(handle, msg[id]);
                   }
               } else {
                   state = 4;
               }
           } else {
               state = 4;
           }

           break;
           /* Get message 2 */
           case 2:
               soc_async_msg_count(handle, &count);
               if (count) {
                   rv[id] = soc_async_msg_dequeue(handle, &msg[id]);
                   if (rv[id] == SOC_E_NONE) {
                       if (msg[id]->type == proc->type) {
                           /* process the message */
                           if (msg[id]->proc_f != NULL) {
                               n++;
                               _async_proc_count_inc(handle);
                               rv[id] = msg[id]->proc_f(msg[id]->unit,
                                         msg[id]->data, msg[id]->cookie);
                           } else {
                               /* Callback for notifcaition */
                                if (msg[id]->cb_f != NULL) {
                                    msg[id]->cb_f(msg[id]->unit,
                                        msg[id]->data, msg[id]->cookie, SOC_E_UNAVAIL);
                                    soc_async_msg_free(handle, msg[id]);
                                    msg[id] = NULL;
                                }
                           }
                           state = 3;
                       } else {
                           soc_async_msg_free(handle, msg[id]);
                           msg[id] = NULL;
                       }
                   }
               }
               if (n > 0) {
                   state = 3;
               } else {
                   state = 4;
               }
               id ^= 1;
           break;

           case 3:
               /* Complete message 1 */
               if (rv[id] == SOC_E_NONE) {
                   /* Wait for the processing to complete */
                   if (msg[id]->wait_f != NULL) {
                       rv[id] = msg[id]->wait_f(msg[id]->unit, msg[id]->data,
                                     msg[id]->cookie, 0);
                   }
               }
               /* Callback for notifcaition */
               if (msg[id]->cb_f != NULL) {
                   msg[id]->cb_f(msg[id]->unit,
                       msg[id]->data, msg[id]->cookie, rv[id]);
               }
               soc_async_msg_free(handle, msg[id]);
               msg[id] = NULL;
               n--;
               _async_proc_count_dec(handle);
              if (n > 0) {
                  state = 2;
              } else {
                  state = 4;
              }
           break;

           default:

               return;
        }
    }

}

/*******************************************
* @function async_thread_func
* purpose This is async thread fucntion to handle messages
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns None
*
* @end
 */
STATIC void
async_thread_func(soc_async_handle_t handle)
{
    soc_async_proc_t *proc = (soc_async_proc_t *)handle;
    int id = proc->threads;
    async_thread_t *async_threads = &proc->async_threads[id - 1];
    thread_cmd_t cmd;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "id [%d] type [%d] started\n"),
                         id, proc->type));

    while(async_threads->state != s_exit) {
        cmd = _async_proc_cmd_get(proc);
         switch(async_threads->state) {

         case s_stop:
             if (cmd == c_exit) {
                 async_threads->state = s_exit;
             } else if (cmd == c_start) {
                 async_threads->state = s_start;
             } else {
                 /* Yield to delay */
                 LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(proc->unit,
                          "Yield for stop\n")));
                 sal_thread_yield();
             }
         break;
         case s_start:
             if (cmd == c_exit) {
                 async_threads->state = s_exit;
             } else if (cmd == c_stop) {
                 async_threads->state = s_stop;
             } else {
                 async_threads->state = s_wait;
             }
         break;
         case s_wait:
             if (cmd == c_exit) {
                 async_threads->state = s_exit;
             } else if (cmd == c_stop) {
                 async_threads->state = s_stop;
             } else {
                int count;
                soc_async_msg_count(handle, &count);
                if (count == 0) {
                    (void)sal_sem_take(async_threads->wait_event,
                                          sal_sem_FOREVER);
                }
                async_threads->state = s_run;
             }
         break;
         case s_run:
             if (cmd == c_exit) {
                 async_threads->state = s_exit;
             } else if (cmd == c_stop) {
                 async_threads->state = s_stop;
            } else {
                 _async_thread_msg_proc(handle);
                 async_threads->state = s_wait;
             }
         break;
         default:
             LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(proc->unit,
                          "id [%d] Non Option\n"), id));
         break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "id [%d] Terminated\n"), id));
    async_threads->pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);

}

/*******************************************
* @function _async_thread_destroy
* purpose Destroy the async threads
*
* @param proc [in] pointer to soc_async_proc_t
*
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_async_thread_destroy(soc_async_proc_t *proc)
{
   soc_timeout_t to;
   int i;
   async_thread_t *async_threads = proc->async_threads;

   if ((proc->threads == 0) || (async_threads == NULL)) {
       return SOC_E_NONE;
   }

   /* Give thread a few seconds to wake up and exit */
   soc_timeout_init(&to, 50 * 1000000, 0);
   _async_proc_cmd_set(proc, c_exit);

   for (i = 0 ; i < proc->threads; i++) {
       /* Wake up thread so it will check the exit flag */
       while (async_threads[i].pid != SAL_THREAD_ERROR) {
            sal_sem_give(async_threads[i].wait_event);
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(proc->unit,
                              "thread will not exit\n")));
                break;
            }
            /* Delay for some time */
            sal_usleep(200);
       }
       if (async_threads[i].wait_event) {
           sal_sem_destroy(async_threads[i].wait_event);
           async_threads[i].wait_event = NULL;
       }
   }

   if (proc->lock) {
      sal_spinlock_destroy(proc->lock);
      proc->lock = NULL;
   }

   if (proc->stop_sem) {
       sal_sem_destroy(proc->stop_sem);
       proc->stop_sem = NULL;
   }

   return SOC_E_NONE;
}

/*******************************************
* @function _async_thread_create
* purpose Create the async threads
*
* @param proc [in] pointer to soc_async_proc_t
* @param threads [in] number of threads
* @returns SOC_E_NONE
*
* @end
 */

STATIC int
_async_thread_create(soc_async_proc_t *proc, soc_async_prop_t *prop)
{
   async_thread_t  *async_threads = NULL;
   int rv = SOC_E_NONE;
   int i;

    /* Create Async processing threads */
    async_threads =
               sal_alloc(prop->threads * sizeof(async_thread_t),
                         "Async thread");

    if (async_threads == NULL) {
        return SOC_E_MEMORY;
    }

    do {
        proc->lock = sal_spinlock_create("Proc Lock");

        if (proc->lock == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        proc->stop_sem = sal_sem_create("stop wait", sal_sem_BINARY, 0);

        if (proc->stop_sem == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        proc->async_threads = async_threads;
        proc->cmd = c_start;
        sal_memset(async_threads, 0, prop->threads * sizeof(async_thread_t));
        for (i = 0 ; i < prop->threads; i++) {
            proc->threads = i + 1;
            async_threads[i].wait_event =
                      sal_sem_create("Msg event", sal_sem_BINARY, 0);

            if (async_threads[i].wait_event == NULL) {
                rv = SOC_E_MEMORY;
                break;
            }
            async_threads[i].state = s_stop;
            async_threads[i].pid = sal_thread_create("Async thread",
                                    SAL_THREAD_STKSZ,
                                    prop->prio,
                                    async_thread_func,
                                    proc);
            if (async_threads[i].pid == SAL_THREAD_ERROR) {
                rv = SOC_E_FAIL;
                break;
            }
            /* Let the thread started */
            sal_thread_yield();

        }
        if (rv != SOC_E_NONE) {
            break;
        } else {
          return rv;
        }
    } while(0);

    (void)_async_thread_destroy(proc);
    sal_free(async_threads);
    proc->async_threads = NULL;
    proc->threads = 0;

    return rv;

}

/*******************************************
* @function _async_thread_wake
* purpose wake up async threads
*
* @param proc [in] pointer to soc_async_proc_t
*
* @returns SOC_E_NONE
*
* @end
 */
STATIC int
_async_thread_wake(soc_async_proc_t *proc)
{
    int i, j;
    int count;
    async_thread_t *async_threads = proc->async_threads;

    soc_async_msg_count(proc, &count);
    /* Unblock the threads to process message */
    /* One thread will process 2 messages simulteneously */
    count = (count > 2 * proc->threads) ? \
             2 * proc->threads : count;
    count = (count > 1) ? (count >> 1) : count;
    for (i = 0, j = 0; (i < count) && (j < proc->threads); j++) {
         if (async_threads[j].state == s_wait) {
             sal_sem_give(async_threads[j].wait_event);
             i++;
         }
    }
    /* Let's yield for processing */
    sal_thread_yield();

    return SOC_E_NONE;

}

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
int soc_async_proc_deinit(soc_async_handle_t handle)
{
   int rv = SOC_E_NONE;
   soc_async_proc_t *proc = (soc_async_proc_t *)handle;

   if (!handle) {
       return SOC_E_PARAM;
   }

   soc_async_flush_queue(handle);

   rv = _async_thread_destroy(proc);
   sal_free(proc->async_threads);
   proc->async_threads = NULL;
   proc->threads = 0;

   if (rv != SOC_E_NONE)
       return rv;
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "Async threads Destroyed\n")));

   LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "Message queue flushed\n")));

   rv = _async_queue_destroy(&proc->queue);

   LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "Async proc Destroy Success\n")));

   sal_free(proc);

   return rv;
}

/*******************************************
* @function soc_async_proc_init
* @purpose  Initialize the async proc.
*
* @param unit [in] unit number
* @param prop [in] property of  <soc_async_prop_t>
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments The function will allocate resources , create thread safe
*  queues and processing threads. This will return handle to caller
*
* @end
 */
int soc_async_proc_init(
              int unit,
              soc_async_prop_t   *prop,
              soc_async_handle_t *handle)
{
    int rv = SOC_E_NONE;
    soc_async_proc_t *proc;

    if (!prop) {
        return SOC_E_PARAM;
    }
    if((prop->q_size == 0) || (prop->threads == 0)) {
        return SOC_E_PARAM;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                         "type = %d, size = %u, threads =%d\n"),
                         prop->type, (unsigned int)prop->q_size, prop->threads));

    proc = sal_alloc(sizeof(*proc), "Async PROC");

    if (proc == NULL) {
        return SOC_E_MEMORY;
    }

    sal_memset(proc, 0, sizeof(*proc));
    proc->unit = unit;
    proc->type = prop->type;

    /* Initialize message queue */
    rv = _async_queue_create(prop->q_size, &proc->queue);

    if (rv != SOC_E_NONE) {
        goto error;
    }

    rv = _async_thread_create(proc, prop);

    if (rv != SOC_E_NONE) {
        goto error;
    }

    *handle = (soc_async_handle_t)proc;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                         "Async proc type[%d] create Success\n"),
                         prop->type));

    return SOC_E_NONE;
error:

    LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                         "Error in initialize Async thread.\n")));
    soc_async_proc_deinit(proc);
   return rv;
}

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
int soc_async_msg_alloc(
                  soc_async_handle_t handle,
                  soc_async_msg_t **msg)
{

    /* If required, to optimize the peformance application
     * specific allocation method can be implemented
     */
    *msg = sal_alloc(sizeof(soc_async_msg_t), "Async MSG");

    if (*msg == NULL) {
        return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

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
int soc_async_msg_free(
                  soc_async_handle_t handle,
                  soc_async_msg_t *msg)
{
    if (msg == NULL) {
        return SOC_E_PARAM;
    }
    sal_free(msg);
    return SOC_E_NONE;
}

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
int soc_async_msg_queue(
                  soc_async_handle_t handle,
                  soc_async_msg_t *msg)
{
   soc_async_proc_t *proc = (soc_async_proc_t *)handle;
   soc_async_queue_t *q = &proc->queue;
   int count;
   thread_cmd_t cmd;

   cmd = _async_proc_cmd_get(proc);
   if((cmd == c_exit) || (cmd == c_stop)) {
       return SOC_E_UNAVAIL;
   }

   sal_spinlock_lock(q->lock);
   count = q->tail - q->head;
   if ((count == -1) || (count == q->size - 1)) {
       sal_spinlock_unlock(q->lock);
       return SOC_E_MEMORY;
   }
   q->msg_q[q->tail] = msg;
   q->count++;
   q->tail = (q->tail + 1) % q->size;
   count = q->count;
   sal_spinlock_unlock(q->lock);

   LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(proc->unit,
              "Messages = %d\n"),
               count));

   _async_thread_wake(proc);

   return SOC_E_NONE;
}

/*******************************************
* @function soc_async_msg_dequeue
* purpose Dequeue the message
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param msg [out] message pointer to get the message

* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments The processing Threads will call this function to
*  dequeue the message from the thread safe message queue to
*  process it further.
*
* @end
 */
int soc_async_msg_dequeue(
                  soc_async_handle_t handle,
                  soc_async_msg_t **msg)
{
   soc_async_proc_t *proc = (soc_async_proc_t *)handle;
   soc_async_queue_t *q = &proc->queue;
   int count;
   thread_cmd_t cmd;

   cmd = _async_proc_cmd_get(proc);
   if(cmd == c_exit) {
       return SOC_E_UNAVAIL;
   }

   sal_spinlock_lock(q->lock);

   if (q->tail == q->head) {
       sal_spinlock_unlock(q->lock);
       return SOC_E_MEMORY;
   }
   *msg = q->msg_q[q->head];
   q->head = (q->head + 1) % q->size;
   q->count--;
   count = q->count;
   sal_spinlock_unlock(q->lock);
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
             (BSL_META_U(proc->unit,
              "Messages = %d \n"), count));

   return SOC_E_NONE;
}


/*******************************************
* @function soc_async_flush_queue
* purpose Flush the message queue
*
* @param handle [in] Async handle of type soc_async_handle_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @comments Delete all the messages in the queue and make it empty.
*
* @end
 */
int soc_async_flush_queue(soc_async_handle_t handle)
{
   int i, count;
   soc_async_msg_t *msg;
   soc_async_proc_t *proc = (soc_async_proc_t *)handle;
   int rv;

   soc_async_msg_stop(handle);
   soc_async_msg_count(handle, &count);
   LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "[%d] Messages to Flush\n"), count));

   for ( i = 0 ; i < count ; i++) {
       rv = soc_async_msg_dequeue(handle, &msg);
       if (SOC_SUCCESS(rv)) {
           soc_async_msg_free(handle, msg);
       }
   }
   soc_async_msg_start(handle);
   return SOC_E_NONE;
}

/*******************************************
* @function soc_async_msg_count
* purpose Get the number of messages in the queue
*
* @param handle [in] Async handle of type soc_async_handle_t
* @param count [out] returns message count
*
* @returns SOC_E_NONE
*
* @comments Delete all the messages in the queue and make it empty.
*
* @end
 */
int soc_async_msg_count(soc_async_handle_t handle, int *count)
{
   soc_async_queue_t *q = &((soc_async_proc_t *)handle)->queue;

   sal_spinlock_lock(q->lock);
   *count = q->count;
   sal_spinlock_unlock(q->lock);

   return SOC_E_NONE;
}

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
int soc_async_msg_start(soc_async_handle_t handle)
{
   soc_async_proc_t *proc = (soc_async_proc_t *)handle;
   int count = 0;

   soc_async_msg_count(handle, &count);
   _async_proc_cmd_set(proc, c_start);
   if (count > 0) {
       _async_thread_wake(proc);
   }

   return SOC_E_NONE;
}

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
int soc_async_msg_stop(soc_async_handle_t handle)
{
   soc_async_proc_t *proc = (soc_async_proc_t *)handle;
   int count = 0;

   _async_proc_cmd_set(proc, c_stop);

   sal_spinlock_lock(proc->lock);
   count = proc->count;
   sal_spinlock_unlock(proc->lock);

   if (count > 0) {
       LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(proc->unit,
                         "[%d] Wait Proc count\n"), count));
       (void)sal_sem_take(proc->stop_sem, sal_sem_FOREVER);
   }

   return SOC_E_NONE;
}



#endif /* BCM_SOC_ASYNC_SUPPORT */

