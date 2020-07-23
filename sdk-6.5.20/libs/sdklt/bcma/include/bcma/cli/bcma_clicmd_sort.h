/*! \file bcma_clicmd_sort.h
 *
 * CLI 'sort' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_SORT_H
#define BCMA_CLICMD_SORT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_SORT_DESC \
    "Execute system-provided sort command."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_SORT_SYNOP \
    "[<options>]"

/*! Help for CLI command. */
#define BCMA_CLICMD_SORT_HELP \
    "The sort command is used to sort the output from other CLI\n" \
    "commands. Please refer to the sort man page for a complete\n" \
    "description of command options."

/*! Examples for CLI command */
#define BCMA_CLICMD_SORT_EXAMPLES \
    "list raw cmic_dev | %s -f"

/*!
 * \brief CLI 'sort' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_sort(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_SORT_H */
