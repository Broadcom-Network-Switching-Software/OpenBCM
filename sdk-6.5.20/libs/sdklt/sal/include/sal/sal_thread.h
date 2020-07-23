/*! \file sal_thread.h
 *
 * Thread API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAL_THREAD_H
#define SAL_THREAD_H

#include <sal/sal_types.h>

/*! Special \ref sal_thread_t error handle. */
#define SAL_THREAD_ERROR                ((sal_thread_t) -1)

/*! Default stack size for threads created via \ref sal_thread_create. */
#ifndef SAL_THREAD_STKSZ
#define	SAL_THREAD_STKSZ                16384
#endif

/*! Opaque thread handle. */
typedef struct sal_thread_s *sal_thread_t;

/*!
 * \brief Run thread at highest acceptable priority.
 *
 * The primary use of this priority level is for user threads
 * executing interrupt handlers.
 */
#define SAL_THREAD_PRIO_NO_PREEMPT      -1

/*!
 * \brief Run thread at default priority.
 *
 * To be used for all threads with no special scheduling needs.
 * Most SDK threads will use this value.
 */
#ifndef SAL_THREAD_PRIO_DEFAULT
#define SAL_THREAD_PRIO_DEFAULT         50
#endif

/*!
 * \brief Run thread at background priority.
 *
 * Used for threads, which only have to run occasionally, e.g. to
 * check overall system health.
 *
 * Support for this priority is optional, i.e. it may fall back to the
 * system default priority, if system support is unavailable.
 */
#ifndef SAL_THREAD_PRIO_LOW
#define SAL_THREAD_PRIO_LOW             40
#endif

/*!
 * \brief Run thread at elevated priority.
 *
 * Used for threads, which cannot be stalled for an extended amount of
 * time, e.g. to prevent hardware counter overruns.
 *
 * Support for this priority is optional, i.e. it may fall back to the
 * system default priority, if system support is unavailable.
 */
#ifndef SAL_THREAD_PRIO_HIGH
#define SAL_THREAD_PRIO_HIGH            60
#endif

/*!
 * \brief Run thread at high priority.
 *
 * This priority should make a thread run before all normal threads,
 * and it will typically be used for time-sensitive application
 * callbacks.
 */
#ifndef SAL_THREAD_PRIO_CRITICAL
#define SAL_THREAD_PRIO_CRITICAL        80
#endif

/*!
 * \brief Run thread at interrupt priority.
 *
 * This priority should make a thread run before all other threads,
 * with the exception of threads with the same priority.
 */
#ifndef SAL_THREAD_PRIO_INTERRUPT
#define SAL_THREAD_PRIO_INTERRUPT       SAL_THREAD_PRIO_NO_PREEMPT
#endif

/*! Per-thread data type. */
typedef void sal_thread_data_t;

/*!
 * \brief Thread function.
 *
 * A function of this type must be supplied to \ref sal_thread_create,
 * and upon successful creation of a thread, this function will be
 * called from the newly created thread, and upon return the thread
 * will terminate.
 *
 * \param [in] arg Thread context supplied via \ref sal_thread_create.
 *
 * \return Nothing.
 */
typedef void (sal_thread_func_f)(void *arg);

/*!
 * \brief Thread data desctructor function.
 *
 * If a function of this type is supplied to \ref
 * sal_thread_data_create, then it will be called when the thread
 * exits if the associated per-thread data key is non-NULL. The data
 * key will be supplied as the parameter.
 *
 * \param [in] arg Data key supplied via \ref sal_thread_data_set.
 *
 * \return Nothing.
 */
typedef void (sal_thread_data_destroy_f)(void *arg);

/*!
 * \brief Create new thread.
 *
 * \param [in] name Thread name.
 * \param [in] stksz Stack size for this thread.
 * \param [in] prio Thread priority (0-99 or SAL_THREAD_PRIO_xxx).
 * \param [in] func Thread function to be called by new thread.
 * \param [in] arg Context to be passed to thread function.
 *
 * \return Thread handle or SAL_THREAD_ERROR on failure.
 */
extern sal_thread_t
sal_thread_create(char *name, int stksz, int prio,
                  sal_thread_func_f func, void *arg);

/*!
 * \brief Get thread handle of current thread.
 *
 * \return Thread handle or SAL_THREAD_ERROR if not launched by \ref
 *         sal_thread_create.
 */
extern sal_thread_t
sal_thread_self(void);

/*!
 * \brief Get thread name.
 *
 * \param [in] thread Thread handle.
 * \param [out] thread_name Buffer where to  store the thread name.
 * \param [in] thread_name_size Size of \c thread_name buffer.
 *
 * \return Pointer to \c thread_name or NULL if thread was not found.
 */
extern char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size);

/*!
 * \brief Clean up thread.
 *
 * A thread function supplied to \ref sal_thread_create does not need
 * to call this API directly, as it will be called when the thread
 * function returns.
 *
 * \param [in] rc Thread exit code (0 means normal exit).
 *
 * \return Nothing.
 */
extern void
sal_thread_exit(int rc);

/*!
 * \brief Yield execution.
 *
 * Voluntarily yield execution of the current thread to allow other
 * threads of equal scheduling priority to run.
 *
 * \return Nothing.
 */
extern void
sal_thread_yield(void);

/*!
 * \brief Set priority of current thread.
 *
 * This function may be used to avoid priority inversion by elevating
 * the thread priority while holding a lock.
 *
 * \param [in] prio Thread priority (SAL_THREAD_PRIO_xxx).
 *
 * \retval 0 No errors.
 * \retval -1 Unsupported priority or failed to set priority.
 */
extern int
sal_thread_prio_set(int prio);

/*!
 * \brief Get priority of current thread.
 *
 * This function may be used to avoid priority inversion by elevating
 * the thread priority while holding a lock.
 *
 * \param [out] prio Thread priority (SAL_THREAD_PRIO_xxx).
 *
 * \retval 0 No errors.
 * \retval -1 Unsupported priority or failed to get priority.
 */
extern int
sal_thread_prio_get(int *prio);

/*!
 * \brief Create per-thread data object.
 *
 * This function will create a new data key for all threads belonging
 * to the parent process. The initial value of the key will be NULL
 * for all threads.
 *
 * \param [in] destructor Destructor function to be called on thread exit.
 *
 * \return Per-thread data handle.
 */
extern sal_thread_data_t *
sal_thread_data_create(sal_thread_data_destroy_f destructor);

/*!
 * \brief Set per-thread data key for this thread.
 *
 * If non-NULL, this key will be supplied to the \c destructor
 * function upon thread exit.
 *
 * \param [in] tdata Per-thread data handle from \ref sal_thread_data_create.
 * \param [in] val New per-thread data key.
 *
 * \retval 0 No errors
 */
extern int
sal_thread_data_set(sal_thread_data_t *tdata, void *val);

/*!
 * \brief Get per-thread data key for this thread.
 *
 * \param [in] tdata Per-thread data handle from \ref sal_thread_data_create.
 *
 * \return 0 if no errors, otherwise non-zero.
 */
extern void *
sal_thread_data_get(sal_thread_data_t *tdata);

/*!
 * \brief Delete per-thread data object.
 *
 * Calling this function will invoke the destructor function supplied
 * to \ref sal_thread_data_create.
 *
 * \param [in] tdata Per-thread data handle.
 *
 * \retval 0 No errors
 */
extern int
sal_thread_data_delete(sal_thread_data_t *tdata);

#endif /* SAL_THREAD_H */
