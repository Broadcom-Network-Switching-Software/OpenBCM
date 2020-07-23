/*! \file bcma_cli_path.h
 *
 * Functions related to retrieve default CLI path variable.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_PATH_H
#define BCMA_CLI_PATH_H

#include <bcma/cli/bcma_cli.h>

/*! Default name of environement variable for path. */
#ifndef BCMA_CLI_VAR_PATH
#define BCMA_CLI_VAR_PATH "path"
#endif

/*!
 * \brief Get each path string from environment variable \ref BCMA_CLI_VAR_PATH.
 *
 * Multiple path strings can be stored in environemnt variable \ref
 * BCMA_CLI_VAR_PATH in concatenation of space or colon.
 * The function is used to retrieve each path string from
 * environment variable \ref BCMA_CLI_VAR_PATH. The usage of the function is
 * similar to strtok_r, except the string 's' is allocated in this function. Caller
 * should free 's' after finishing it's usage.
 *
 * Example:
 *
 * \code{.c}
 * void get_path(bcma_cli_t *cli)
 * {
 *     char *s = NULL, *s2, *path;
 *
 *     path = bcma_cli_path_tok_get(cli, &s, &s2);
 *     while (path != NULL) {
 *         ...
 *         ...
 *         path = bcma_cli_path_tok_get(cli, NULL, &s2);
 *     }
 *
 *     if (s != NULL) {
 *         sal_free(s);
 *     }
 * }
 * \endcode
 *
 * \param [in] cli CLI object
 * \param [in] s String to be duplicated in bcma_cli_var_path_tok_get
 * \param [in] s2 Next token search starting point
 *
 * \retval Pointer of each path string. NULL for no more tokens.
 */
extern char *
bcma_cli_path_tok_get(bcma_cli_t *cli, char **s, char **s2);

#endif /* BCMA_CLI_PATH_H */
