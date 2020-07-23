/*! \file bcma_cli_var.h
 *
 * Wrappers for CLI variable call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_VAR_H
#define BCMA_CLI_VAR_H

#include <bcma/cli/bcma_cli.h>

/*! Default local environment variable name for command execution result. */
#ifndef BCMA_CLI_VAR_RESULT
#define BCMA_CLI_VAR_RESULT "?"
#endif

/*!
 * \brief Install call-back vectors for environment variables.
 *
 * An application can add support for shell environment variables by
 * installing a set of call-back functions.
 *
 * Please refer to \ref bcma_cli_var_cb_t for more information about each
 * call-back function.
 *
 * \param [in] cli CLI object.
 * \param [in] var_cb Call-back functions.
 * \param [in] cookie Pass-thru data for call-back functions.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_var_cb_set(bcma_cli_t *cli,
                    const bcma_cli_var_cb_t *var_cb, void *cookie);

/*!
 * \brief Wrapper for environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 * \param [in] name Variable name.
 *
 * \retval Value of variable or NULL if not found.
 */
extern const char *
bcma_cli_var_get(bcma_cli_t *cli, const char *name);

/*!
 * \brief Wrapper for environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * Variables set in the local scope are hidden when the scope is
 * pushed, and they will be deleted when the scope is popped. Global
 * variables are visible in all scopes.
 *
 * Variable will be deleted if \c value is NULL. Otherwise variable will be
 * update or added by \c value.
 *
 * \param [in] cli CLI object.
 * \param [in] name Variable name.
 * \param [in] value Variable value.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_var_set(bcma_cli_t *cli, const char *name, const char *value,
                 int local);

/*!
 * \brief Set value to specific environment variable.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 * \param [in] rv Integer result value.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_var_result_set(bcma_cli_t *cli, int rv);

/*!
 * \brief Get TRUE/FALSE value from environment variable.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * If the environment variable is not set, the \c deflt value will be returned.
 * The return value will be FALSE if variable \c name is set to '0' or 'false'.
 * Otherwise the return value will be TRUE.
 *
 * \param [in] cli CLI object.
 * \param [in] name Variable name.
 * \param [in] deflt Default variable value.
 *
 * \retval Bool value of variable (TRUE/FALSE).
 */
extern int
bcma_cli_var_bool_get(bcma_cli_t *cli, const char *name, int deflt);

/*!
 * \brief Wrapper for environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_var_scope_push(bcma_cli_t *cli);

/*!
 * \brief Wrapper for environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_var_scope_pop(bcma_cli_t *cli);

#endif /* BCMA_CLI_VAR_H */
