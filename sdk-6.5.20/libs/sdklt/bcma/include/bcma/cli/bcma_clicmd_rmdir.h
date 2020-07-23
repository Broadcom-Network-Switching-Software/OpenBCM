/*! \file bcma_clicmd_rmdir.h
 *
 * CLI 'rmdir' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_RMDIR_H
#define BCMA_CLICMD_RMDIR_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_RMDIR_DESC \
    "Delete a directory."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_RMDIR_SYNOP \
    "<directory>"

/*! Help for CLI command. */
#define BCMA_CLICMD_RMDIR_HELP \
    "The directory to be deleted must be empty.\n"

/*!
 * \brief CLI 'rmdir' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_rmdir(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_RMDIR_H */
