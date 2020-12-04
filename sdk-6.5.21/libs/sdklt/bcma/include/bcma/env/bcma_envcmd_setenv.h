/*! \file bcma_envcmd_setenv.h
 *
 * CLI 'setenv' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_ENVCMD_SETENV_H
#define BCMA_ENVCMD_SETENV_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_ENVCMD_SETENV_DESC \
    "Show/clear/set global environment variables."

/*! Syntax for CLI command. */
#define BCMA_ENVCMD_SETENV_SYNOP \
    "[<var-name> [<value>]]"

/*! Help for CLI command. */
#define BCMA_ENVCMD_SETENV_HELP \
    "If no parameters are specified, all global variables are displayed.\n" \
    "If only the variable name is specified, the variable is deleted.\n" \
    "If both the variable name and value are specified, the variable is\n" \
    "assigned."

/*!
 * \brief Show/clear/set global environment variable.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully
 * \retval BCMA_CLI_CMD_USAGE Invalid command syntax
 * \retval BCMA_CLI_CMD_FAIL Command failed
 */
extern int
bcma_envcmd_setenv(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_ENVCMD_SETENV_H */
