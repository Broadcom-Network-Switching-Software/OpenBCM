/*! \file bcma_clicmd_if.h
 *
 * CLI 'if' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_IF_H
#define BCMA_CLICMD_IF_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_IF_DESC \
    "Conditionally execute commands."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_IF_SYNOP \
    "<condition> [<command> ... [else <command> ...]]"

/*! Help for CLI command. */
#define BCMA_CLICMD_IF_HELP \
    "Execute a list of command lines only if <condition> is a\n" \
    "non-zero integer or a successful sub-command-line.\n" \
    "Command lines of more than one word must be quoted.\n" \
    "Simple left-to-right boolean operations are supported.\n" \
    "The return value from this command is the result of the last\n" \
    "executed command.\n"

/*! Help for CLI command. */
#define BCMA_CLICMD_IF_HELP_2 \
    "If one of the commands in the list fails, the execution will stop,\n" \
    "unless the environment variable '_iferror' is set to 0."

/*! Examples for CLI command */
#define BCMA_CLICMD_IF_EXAMPLES \
    "1 \"echo hello\" \"echo world\"\n" \
    "$?quickturn && !\"bist arl\" \"echo BIST failed\""

/*!
 * \brief CLI 'if' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_if(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_IF_H */
