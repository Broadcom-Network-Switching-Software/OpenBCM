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

/*
 * Run thread at default priority.
 *
 * Convenience value for thread with no special scheduling needs.
 */
#define SHR_THREAD_PRIO_DEFAULT         (50)

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

    /*! Thread name for debug purposes. */
    char name[32];

    /*! Non-zero if thread is active (running or done). */
    int active;

    /*! Non-zero if thread has been requested to stop. */
    int stopping;

    /*! Non-zero if thread is waiting for an event. */
    int waiting;

    /*! Thread function has exited and returned to thread control. */
    int done;

    /*! Non-zero if thread encountered an internal state error. */
    int error;

} shr_thread_ctrl_t;

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
 * \param [in] usecs Number of microseconds to wait for the thread to stop.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT The thread did not stop within the specified time.
 */
extern int
shr_thread_stop(shr_thread_ctrl_t *tc, sal_usecs_t usecs);

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
 * \param [in] usecs Maximum number of microseconds to wait for a wake event.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to sleep.
 */
extern int
shr_thread_sleep(shr_thread_ctrl_t *tc, sal_usecs_t usecs);

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

#endif /* _SHR_THREAD_H_ */
