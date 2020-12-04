/*! \file bcma_cli_ctrlc.h
 *
 * Wrappers for CLI Ctrl-C call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_CTRLC_H
#define BCMA_CLI_CTRLC_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Install call-back vectors for Ctrl-C management.
 *
 * An application can add support for Ctrl-C traps by installing a set
 * of call-back functions. The main purpose of these functions is to
 * allow active CLI commands to be interrupted by a Ctrl-C keypress.
 *
 * Please refer to \ref bcma_cli_ctrlc_cb_t for more information about each
 * call-back function.
 *
 * \param [in] cli CLI object.
 * \param [in] ctrlc_cb Call-back functions.
 * \param [in] cookie Pass-thru data for call-back functions.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_ctrlc_cb_set(bcma_cli_t *cli, const bcma_cli_ctrlc_cb_t *ctrlc_cb,
                      void *cookie);

/*!
 * \brief Wrapper for Ctrl-C call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 * \param [in] enable Enable (1) or disable (0) Ctrl-C traps.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_ctrlc_enable_set(bcma_cli_t *cli, int enable);

/*!
 * \brief Execute function with Ctrl-C hook.
 *
 * This function is used to allow a Ctrl-C keypress to interrupt an
 * active CLI command, e.g. if it was invoked in error or the user
 * does not want to wait for normal command completion.
 *
 * Use argument \c num_soft_break to define the number of pending
 * Ctrl-C signals before the active CLI command is interrupted
 * immediately (hard-break). The pending Ctrl-C signals can be checked
 * through \ref bcma_cli_ctrlc_break within the function \c func to perform
 * a soft-break from \c func itself.
 *
 * Set -1 to \c num_soft_break to disable hard-break, and set 0 to
 * \c num_soft_break to disable soft-break.
 *
 * \param [in] cli CLI object.
 * \param [in] func Function to call.
 * \param [in] data Function context.
 * \param [in] num_soft_break Number of pending Ctr-C signals allowed before
 *                            hard-break from Ctrl-C.
 *
 * \return BCMA_CLI_CMD_INTR if the CLI command is interrupted by Ctrl-C,
 *         otherwise the return value from the function \c func.
 */
extern int
bcma_cli_ctrlc_exec(bcma_cli_t *cli, int (*func)(void *), void *data,
                    int num_soft_break);

/*!
 * \brief Check whether a Ctrl-C signal is sent.
 *
 * This function will take effect only when \ref bcma_cli_ctrlc_exec is
 * executed with non-zero value for input paremeter \c num_soft_break.
 *
 * The function \c func executed by \ref bcma_cli_ctrlc_exec can call this API
 * periodically to check whether a Ctrl-C signal is sent. When this API returns
 * true, function \c func should return by itself and \ref bcma_cli_ctrlc_exec
 * will return \c BCMA_CLI_CMD_INTR as a soft-break for Ctrl-C.
 *
 * \retval true if a Ctrl-C signal was sent, otherwise false.
 */
extern bool
bcma_cli_ctrlc_break(void);

/*!
 * \brief Ctrl-C signal emulator for soft-break.
 *
 * This API can be used in an environment without signals or an I/O Ctrl-C
 * handler. Application can use this API to interrupt an active CLI command
 * through the soft-break apporach.
 *
 * \param [in] sig_active Set false to reset the soft-break process.
 *                        Set true to activate the soft-break process.
 *
 * \retval Always 0.
 */
extern int
bcma_cli_ctrlc_sig_set(bool sig_active);

#endif /* BCMA_CLI_CTRLC_H */
