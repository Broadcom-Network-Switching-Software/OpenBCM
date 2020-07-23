/*! \file bcma_cli_var.h
 *
 * Wrappers for tagged CLI variable call-back vectors.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_TVAR_H
#define BCMA_CLI_TVAR_H

#include <bcma/cli/bcma_cli.h>

/*!
 * \brief Install call-back vectors for environment variables with tag support.
 *
 * An application can add support for shell environment variables
 * with tag support by installing a set of call-back functions.
 *
 * Please refer to \ref bcma_cli_tvar_cb_t for more information about each
 * call-back function.
 *
 * \param [in] cli CLI object.
 * \param [in] tvar_cb Call-back functions.
 * \param [in] cookie Pass-thru data for call-back functions.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_tvar_cb_set(bcma_cli_t *cli,
                     const bcma_cli_tvar_cb_t *tvar_cb, void *cookie);

/*!
 * \brief Wrapper for tagged environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * This function will return the value and tag of the specified environment
 * variable or NULL if the variable does not exist.
 *
 * \param [in] cli CLI object.
 * \param [in] name Variable name.
 * \param [out] tag Tag value of the environment variable if not NULL.
 *
 * \retval Value of variable or NULL if not found.
 */
extern const char *
bcma_cli_tvar_get(bcma_cli_t *cli, const char *name, uint32_t *tag);

/*!
 * \brief Wrapper for tagged environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * Variables set in the local scope are hidden when the scope is
 * pushed, and they will be deleted when the scope is popped. Global
 * variables are visible in all scopes.
 *
 * Variable will be deleted if \c value is NULL. Otherwise variable will be
 * update or added by \c value and associate with \c tag.
 *
 * \param [in] cli CLI object.
 * \param [in] name Variable name.
 * \param [in] value Variable value.
 * \param [in] tag Tag value.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_tvar_set(bcma_cli_t *cli, const char *name, const char *value,
                  uint32_t tag, int local);

/*!
 * \brief Wrapper for environment variable call-back function.
 *
 * This function will ensure that parameters are validated and that a
 * default behavior is defined if no call-back is installed.
 *
 * Variables the the same tag value with \c tag will be deleted.
 *
 * \param [in] cli CLI object.
 * \param [in] tag Tag value to match for variables unset.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \return Number of variables being deleted.
 */
extern int
bcma_cli_tvar_unset_by_tag(bcma_cli_t *cli, uint32_t tag, int local);

#endif /* BCMA_CLI_TVAR_H */
