/*! \file bcma_clicmd_loop.h
 *
 * CLI 'loop' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_LOOP_H
#define BCMA_CLICMD_LOOP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_LOOP_DESC \
    "Execute a series of commands in a loop."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_LOOP_SYNOP \
    "<loop-count> \"<command>\" ..."

/*! Help for CLI command. */
#define BCMA_CLICMD_LOOP_HELP \
    "Loop over a series of commands <loop-count> times, if <loop-count>\n" \
    "is * then it loops forever. Each argument is a complete command,\n" \
    "so if it contains more than one word, it must be placed in quotes.\n" \
    "If one of the command iterations fails, the execution will stop,\n" \
    "unless the environment variable '_looperror' is set to 0."

/*!
 * \brief CLI 'loop' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_loop(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_LOOP_H */
