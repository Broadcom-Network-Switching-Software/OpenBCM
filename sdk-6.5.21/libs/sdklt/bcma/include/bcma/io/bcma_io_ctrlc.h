/*! \file bcma_io_ctrlc.h
 *
 * System inteface for handling Ctrl-C traps.
 *
 * The purpose is to allow Ctrl-C to perform a programmatic exception
 * for an active function. For C source code this is usually done
 * through the setjmp and longjmp C library functions.
 *
 * Any resources allocated by the active function are *not* freed, so
 * ideally all resources required for the operation should be
 * allocated up-front.
 *
 * The SAL multi-thread and locking code should use the mode_set API
 * to disable Ctrl-C actions when locks are held by the CLI thread,
 * because otherwise the locks may never be released, and this could
 * easily lead to a fatal system error.
 *
 * Ideally an implementation should support multiple levels of
 * functions calls.
 *
 * In the SDK, the Ctrl-C traps are used to permit interruption of CLI
 * commands, typically if these are long-running or infinite.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_IO_CTRLC_H
#define BCMA_IO_CTRLC_H

/*!
 * \brief Enable/disable Ctrl-C traps.
 *
 * When enabled, a Ctrl-C trap handler is installed, and when
 * disabled, the original Ctrl-C behavior is restored.
 *
 * \param [in] enable Enable (1) or disable (0) Ctrl-C traps.
 *
 * \retval 0 No errors.
 */
extern int
bcma_io_ctrlc_enable_set(int enable);

/*!
 * \brief Suspend/resume Ctrl-C handler.
 *
 * Intended for temporary suspension of Ctrl-C traps while in a
 * critical section, e.g. when a mutex is held.
 *
 * \param [in] suspend Suspend (1) or resume (0) Ctrl-C traps.
 *
 * \retval 0 No errors.
 */
extern int
bcma_io_ctrlc_suspend_set(int suspend);

/*!
 * \brief Function wrapper for Ctrl-C traps.
 *
 * Functions \c func called through this API will return immediately
 * (hard-break) when Ctrl-C is pressed and the Ctrl-C callback function
 * \c ctrlc_sig_cb is NULL or returning -1. In case of a Ctrl-C exception,
 * any resources allocated will be lost and any locks held will not be released
 * if \c func is hard-break from this API.
 *
 * If not NULL, function \c ctrlc_sig_cb will be called when Ctrl-C is
 * pressed.
 *
 * Returning 0 from \c ctrlc_sig_cb will prevent the Ctrl-C handler
 * from aborting function \c func immediately, thus eliminating the
 * possibility of unreleased resources.
 *
 * Typically the \c ctrlc_sig_cb function will request a controlled
 * abort of \c func (e.g. by setting a flag) to ensure that all
 * resources are properly released.
 *
 * \param [in] func Function to call with Ctrl-C trap enabled.
 * \param [in] data Pass-thru function context.
 * \param [in] ctrlc_sig_cb Callback function when Ctrl-C is pressed.
 * \param [in] cb_data User data for \c ctrlc_sig_cb.
 * \param [in] rv_intr Return value when interrupted by Ctrl-C.
 *
 * \return The function \c func returned value if not interrupted by Ctrl-C.
 *         Otherwise \c rv_intr.
 */
extern int
bcma_io_ctrlc_exec(int (*func)(void *), void *data,
                   int (*ctrlc_sig_cb)(void *), void *cb_data,
                   int rv_intr);

#endif /* BCMA_IO_CTRLC_H */
