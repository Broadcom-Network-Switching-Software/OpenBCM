/*! \file bcma_envcmd_printenv.h
 *
 * CLI 'printenv' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_ENVCMD_PRINTENV_H
#define BCMA_ENVCMD_PRINTENV_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_ENVCMD_PRINTENV_DESC \
    "Show all environment variables."

/*!
 * \brief Show list of environment variables.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully
 * \retval BCMA_CLI_CMD_USAGE Invalid command syntax
 * \retval BCMA_CLI_CMD_FAIL Command failed
 */
extern int
bcma_envcmd_printenv(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_ENVCMD_PRINTENV_H */
