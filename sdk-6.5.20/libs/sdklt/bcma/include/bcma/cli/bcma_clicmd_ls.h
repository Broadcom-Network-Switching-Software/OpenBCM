/*! \file bcma_clicmd_ls.h
 *
 * CLI 'ls' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_LS_H
#define BCMA_CLICMD_LS_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_LS_DESC \
    "List current directory."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_LS_SYNOP \
    "[-<option>] [<dir|file> ...]"

/*! Help for CLI command. */
#define BCMA_CLICMD_LS_HELP \
    "The ls command is implemented by invoking a platform \"ls\" command.\n" \
    "The supported options are platform specific.\n"

/*!
 * \brief CLI 'ls' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_ls(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_LS_H */
