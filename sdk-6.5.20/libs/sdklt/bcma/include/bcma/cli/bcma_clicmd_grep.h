/*! \file bcma_clicmd_grep.h
 *
 * CLI 'grep' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_GREP_H
#define BCMA_CLICMD_GREP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_GREP_DESC \
    "Execute system-provided grep command."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_GREP_SYNOP \
    "[<options>] <regex>"

/*! Help for CLI command. */
#define BCMA_CLICMD_GREP_HELP \
    "The grep command is used to filter the output from other CLI\n" \
    "commands. Please refer to the grep man page for a complete\n" \
    "description of command options."

/*! Examples for CLI command */
#define BCMA_CLICMD_GREP_EXAMPLES \
    "get cmic_dev_rev_idr | %s -i rev | %s cmic"

/*!
 * \brief CLI 'grep' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_grep(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_GREP_H */
