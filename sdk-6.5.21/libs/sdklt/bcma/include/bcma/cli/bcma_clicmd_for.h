/*! \file bcma_clicmd_for.h
 *
 * CLI 'for' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_FOR_H
#define BCMA_CLICMD_FOR_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_FOR_DESC \
    "Execute a series of commands in a loop."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_FOR_SYNOP \
    "<var>=<start>,<stop>[,<step>[,<fmt>]] '<command>' ..."

/*! Help for CLI command. */
#define BCMA_CLICMD_FOR_HELP \
    "Iterate a series of commands, each time setting <var> to the\n" \
    "loop value. Each argument is a complete command, so if it contains\n" \
    "more than one word, it must be placed in quotes.\n" \
    "Use single quotes can avoid expanding the environment variable\n" \
    "before executing the loop.\n" \
    "<fmt> defaults to %d (decimal), but can be any other standard\n" \
    "printf-style format string.\n"

/*! Help for CLI command. */
#define BCMA_CLICMD_FOR_HELP_2 \
    "If one of the command iterations fails, the execution will stop,\n" \
    "unless the environment variable '_looperror' is set to 0."

/*! Examples for CLI command */
#define BCMA_CLICMD_FOR_EXAMPLES \
    "port=0,23 'echo port=$port'"

/*!
 * \brief CLI 'for' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_for(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_FOR_H */
