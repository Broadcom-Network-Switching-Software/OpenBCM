/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Common thread framework built on top of SAL API.
 */

#ifndef _SHR_THREAD_H_
#define _SHR_THREAD_H_

#include <sal/core/sync.h>
#include <sal/core/time.h>
#include <shared/shr_pb.h>

/*! Use this value to wait indefinitely in \ref shr_thread_sleep. */
#define SHR_THREAD_FOREVER      sal_sem_FOREVER

/*! Default stack size. */
#ifndef SHR_THREAD_STKSZ
#define SHR_THREAD_STKSZ        (64 * 1024)
#endif

/* Forward declaration */
struct shr_thread_ctrl_s;

/*!
 * \brief Type for main thread function.
 *
 * This function is provided by the application and will be called
 * from the thread framework once all the associated control
 * structures have been initialized.
 *
 * The application must provide a function of this type, when \ref
 * shr_thread_start is called.
 *
 * \param [in] tc Thread control returned by \ref shr_thread_start.
 * \param [in] arg Context passed in by \ref shr_thread_start.
 *
 * \return Nothing.
 */
typedef void (shr_thread_func_f)(struct shr_thread_ctrl_s *tc, void *arg);

/*!
 * Thread attributes.
 */
typedef struct shr_thread_attr_s {

    /*! Signature which indicates that attributes are initialized. */
    uint32 ta_sig;

    /*!
     * Thread priority.
     *
     * Please refer to SAL_THREAD_PRIO_xxx for details.
     */
    int prio;

    /*!
     * Thread instance.
     *
     * Setting this to a non-negative value will append the instance
     * number to the thread name. For example, if the thread name is
     * "myThread" and the instance is 3, then the actual thread name
     * will become "myThread.3'.
     */
    int inst;

    /*!
     * Thread stack size.
     */
    int stksz;

    /*!
     * Background thread indicator.
     *
     * Background threads may be controlled through a global switch in
     * order to prevent unwanted hardware access during a critical
     * window, e.g. when a reconfiguration or diagnositcs test is
     * executed.
     */
    int bg;

} shr_thread_attr_t;

/*!
 * Thread control object.
 */
typedef struct shr_thread_ctrl_s {

    /*! Signature which indicates that object is initialized. */
    uint32 tc_sig;

    /*! Next thread in global thread list. */
    struct shr_thread_ctrl_s *next;

    /*! Application-provided function to call from thread context. */
    shr_thread_func_f *thread_func;

    /*! Application-provided context for the thread function. */
    void *thread_arg;

    /*! Semaphore to synchronize thread stop/exit flow. */
    sal_sem_t stop_sem;

    /*! Semaphore to allow thread to wait for an event. */
    sal_sem_t sleep_sem;

    /*! Semaphore to support global suspension state. */
    sal_sem_t suspend_sem;

    /*! Thread attributes. */
    shr_thread_attr_t attr;

    /*! Thread name for debug purposes. */
    char name[32];

    /*! Non-zero if thread is active (running or done). */
    int active;

    /*! Non-zero if thread has been requested to stop. */
    int stopping;

    /*! Non-zero if thread is waiting for an event. */
    int waiting;

    /*! Non-zero if thread is suspended. */
    int suspended;

    /*! Thread function has exited and returned to thread control. */
    int done;

    /*! Non-zero if thread encountered an internal state error. */
    int error;

} shr_thread_ctrl_t;

/*!
 * \brief Initialize thread attributes to default values.
 *
 * This function must be called on any \ref shr_thread_attr_t before
 * it is modified.
 *
 * \param [out] attr Thread attribute structure.
 */
extern void
shr_thread_attr_init(shr_thread_attr_t *attr);

/*!
 * \brief Start a thread.
 *
 * This function will launch a thread via the SAL thread API and call
 * into the supplied application function.
 *
 * The thread will use the stack size and the priority provided via
 * the \c attr parameter. Please refer to \ref shr_thread_attr_t for a
 * full description of configurable thread attributes.
 *
 * Example:
 *
 * \code{.c}
 * void my_thread(shr_thread_t th, void *arg)
 * {
 *     ...
 * }
 *
 * void my_main(void)
 * {
 *     shr_thread_t th;
 *     shr_thread_attr_t attr;
 *
 *     shr_thread_attr_init(&attr);
 *     th = shr_thread_attr_start("myThread", &attr, my_thread, NULL);
 *     ...
 * }
 * \endcode
 *
 * \param [in] name Thread name.
 * \param [in] attr Thread attributes.
 * \param [in] func Application function to call from newly created thread.
 * \param [in] arg Transparent context passed in to application function.
 *
 * \return Thread control object or NULL on error.
 */
extern shr_thread_ctrl_t *
shr_thread_attr_start(char *name, shr_thread_attr_t *attr,
                      shr_thread_func_f func, void *arg);

/*!
 * \brief Start a thread.
 *
 * This function will launch a thread via the SAL thread API and call
 * into the supplied application function.
 *
 * The thread will use the default SAL stack size, and the priority
 * uses the SAL API definitions directly. If \c prio is set to -1,
 * then \c SAL_THREAD_PRIO_DEFAULT will be used.
 *
 * Example:
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     ...
 * }
 *
 * void my_main(void)
 * {
 *     shr_thread_ctrl_t *tc;
 *
 *     tc = shr_thread_start("myThread", -1, my_thread, NULL);
 *     ...
 * }
 * \endcode
 *
 * \param [in] name Thread name.
 * \param [in] prio Thread priority (use -1 for default priority).
 * \param [in] func Application function to call from newly created thread.
 * \param [in] arg Transparent context passed in to application function.
 *
 * \return Thread control object or NULL on error.
 */
extern shr_thread_ctrl_t *
shr_thread_start(char *name, int prio, shr_thread_func_f func, void *arg);

/*!
 * \brief Terminate an active thread.
 *
 * This function is used to terminate a thread, which was started by
 * \ref shr_thread_start. The caller must specify the maximum time to
 * wait for the thread to terminate itself.
 *
 * If the thread has already exited, then this function will simply
 * clean up the resources associated with the thread.
 *
 * If the thread is still running, then a stop request will be issued,
 * and the function will wait for the thread to exit.
 *
 * If a timeout value of zero is specified, the function will return
 * immediately. In this case \ref shr_thread_stop must be called again
 * later with a non-zero timeout value to verify that the thread did
 * indeed stop. This mechanism allows a caller to wait in parallel for
 * multiple threads to terminate.
 *
 * An event-driven thread function will typical look as shown
 * below. When \ref shr_thread_stop is called, a stop request will be
 * issued and the thread will be woken up. The thread must check for a
 * stop request using \ref shr_thread_stopping and exit accordingly.
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     while (1) {
 *         shr_thread_sleep(tc, SHR_THREAD_FOREVER);
 *         if (shr_thread_stopping(tc)) {
 *             break;
 *         }
 *         ...
 *     }
 * }
 *
 * void my_shutdown(void)
 * {
 *     int rv;
 *
 *     rv = shr_thread_stop(tc, 500000);
 *     ...
 * }
 * \endcode
 *
 * By provoding a timeout value, the caller may choose to wait for the
 * thread to terminate.
 *
 * \param [in] tc Thread control returned by \ref shr_thread_start.
 * \param [in] usec Number of microseconds to wait for the thread to stop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT The thread did not stop within the specified time.
 */
extern int
shr_thread_stop(shr_thread_ctrl_t *tc, sal_usecs_t usec);

/*!
 * \brief Check for stop request.
 *
 * This function should be called from an event-driven thread to check
 * if it should terminate itself.
 *
 * A typical event-driven thread is shown below.
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     while (1) {
 *         shr_thread_sleep(tc, SHR_THREAD_FOREVER);
 *         if (shr_thread_stopping(tc)) {
 *             break;
 *         }
 *         ...
 *     }
 * }
 * \endcode
 *
 * \param [in] tc Thread control passed in as a function argument.
 *
 * \retval TRUE A stop request was submitted, exit as soon as possible.
 * \retval FALSE No stop request was submitted, continue processing.
 */
extern int
shr_thread_stopping(shr_thread_ctrl_t *tc);

/*!
 * \brief Check if thread is done.
 *
 * This function may be called from a parent thread to check if a
 * child thread is done.
 *
 * If the function returns TRUE, then the parent thread must call \ref
 * shr_thread_stop to clean up the thread completely.
 *
 * \param [in] tc Thread control passed in as a function argument.
 *
 * \retval TRUE Thread is done.
 * \retval FALSE Thread is still running.
 */
extern int
shr_thread_done(shr_thread_ctrl_t *tc);

/*!
 * \brief Wait for event.
 *
 * This function may be called from an event-driven thread when there
 * is no more work to do.
 *
 * Another thread must wake up the sleeping thread by calling \ref
 * shr_thread_wake.  The thread will wait indefinitely if the timeout
 * value SHR_THREAD_FOREVER is used.
 *
 * A typical event-driven thread is shown below.
 *
 * \code{.c}
 * void my_thread(shr_thread_ctrl_t *tc, void *arg)
 * {
 *     while (1) {
 *         shr_thread_sleep(tc, SHR_THREAD_FOREVER);
 *         if (shr_thread_stopping(tc)) {
 *             break;
 *         }
 *         ...
 *     }
 * }
 * \endcode
 *
 * Note that the timeout value can also be used to simply wake up the
 * thread at fixed intervals.
 *
 * \param [in] tc Thread control passed in as a function argument.
 * \param [in] usec Maximum number of microseconds to wait for a wake event.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to sleep.
 */
extern int
shr_thread_sleep(shr_thread_ctrl_t *tc, sal_usecs_t usec);

/*!
 * \brief Wake an event-driven thread.
 *
 * Use this function to wake up an event-driven thread, which is
 * asleep after calling \ref shr_thread_sleep.
 *
 * This function is safe to call from interrupt context.
 *
 * \param [in] tc Thread control for the thread to be woken.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
shr_thread_wake(shr_thread_ctrl_t *tc);

/*!
 * \brief Set thread waiting state.
 *
 * This function can be used to ensure that the proper thread state is
 * maintained for conditional wait implementations beyond \ref
 * shr_thread_sleep.
 *
 * A typical use case would be a thread waiting for messages from a
 * message queue. Ideally the thread state will be maintained from
 * within the message queue implementation, such that the \ref
 * shr_thread_waiting_set function never has to be called directly
 * from the driver code.
 *
 * \param [in] tc Thread control for the thread to change state.
 * \param [in] waiting Zero or non-zero to indicate new state.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
shr_thread_waiting_set(shr_thread_ctrl_t *tc, int waiting);

/*!
 * \brief Suspend all background threads.
 *
 * Put all backgound threads to sleep as soon as possible.
 *
 * A background thread is any thread with the \c bg attribute set to a
 * non-zero value.
 *
 * Each backgound thread will be suspended as soon as it enters the
 * waiting state either via \ref shr_thread_sleep or \ref
 * shr_thread_waiting_set.
 *
 * Use \ref shr_thread_bg_resume to restart all suspended background
 * threads.
 *
 * Calling \ref shr_thread_stop will also resume a suspended thread.
 */
extern void
shr_thread_bg_suspend(void);

/*!
 * \brief Resume all suspended background threads.
 *
 * Please refer to \ref shr_thread_bg_suspend for a full description.
 */
extern void
shr_thread_bg_resume(void);

/*!
 * \brief Check if all background threads are suspended.
 *
 * After calling \ref shr_thread_bg_suspend, the caller may use this
 * function to check that all background threads have indeed been
 * suspended.
 *
 * \retval 1 All background threads have been suspended.
 * \retval 0 One or more background threads are still running.
 */
extern int
shr_thread_bg_suspended(void);

/*!
 * \brief Dump status of all threads to a print buffer.
 *
 * The \c pb parameter may be specified as NULL if only the number of
 * active threads is of interest.
 *
 * \param [in] pb Print buffer object.
 *
 * \return Number of active threads.
 */
extern int
shr_thread_dump(shr_pb_t *pb);

/*!
 * \brief Get thread by name.
 *
 * \param [in] name Thread name.
 *
 * \retval SHR_E_NONE No errors.
 * \retval _SHR_E_FAIL The thread does not exist.
 */
extern shr_thread_ctrl_t *
shr_thread_get_by_name(char *name);

#endif /* _SHR_THREAD_H_ */
