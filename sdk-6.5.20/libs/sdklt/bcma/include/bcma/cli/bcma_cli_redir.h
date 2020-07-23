/*! \file bcma_cli_redir.h
 *
 * Wrappers for CLI I/O redirection ("|") call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_REDIR_H
#define BCMA_CLI_REDIR_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Install call-back vectors for CLI I/O redirection.
 *
 * An application can add support for CLI I/O redirection by
 * installing a set of call-back functions. The main purpose of these
 * functions is to allow support for streaming commands like grep.
 *
 * Please refer to \ref bcma_cli_redir_cb_t for more information about
 * each call-back function.
 *
 * \param [in] cli CLI object.
 * \param [in] redir_cb Call-back functions.
 * \param [in] cookie Pass-thru data for call-back functions.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_redir_cb_set(bcma_cli_t *cli, const bcma_cli_redir_cb_t *redir_cb,
                      void *cookie);

/*!
 * \brief Wrapper for CLI I/O redirection enable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 * \param [in] enable Enable (1) or disable (0) I/O redirection.
 *                    Disable and reset (-1) I/O redirection.
 *
 * \retval 0 No errors
 */
extern int
bcma_cli_redir_enable_set(bcma_cli_t *cli, int enable);

/*!
 * \brief Wrapper for CLI redirection command done call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 */
extern int
bcma_cli_redir_cmd_done(bcma_cli_t *cli);

/*!
 * \brief Wrapper for CLI redirection input source call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 */
extern const char *
bcma_cli_redir_input_name_get(bcma_cli_t *cli);

/*!
 * \brief Wrapper for CLI redirection output source call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 */
extern const char *
bcma_cli_redir_output_name_get(bcma_cli_t *cli);

#endif /* BCMA_CLI_REDIR_H */
