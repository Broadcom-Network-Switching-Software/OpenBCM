/*! \file bcma_clicmd_each.h
 *
 * CLI 'each' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_EACH_H
#define BCMA_CLICMD_EACH_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_EACH_DESC \
    "Execute a series of commands in a loop."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_EACH_SYNOP \
    "<V>=<A1>,...,<An> '<command>' ...\n" \
    "{<V1>:...:<Vm>}={<A1>:...:<M1>},...,{<An>:...:<Mn>} '<command>' ..."

/*! Help for CLI command. */
#define BCMA_CLICMD_EACH_HELP \
    "Iterate a series of commands, each time setting variable <V> or\n" \
    "a group of variables <V1>:...:<Vm> to the loop value(s).\n" \
    "Each argument is a complete command, so if it contains\n" \
    "more than one word, it must be placed in quotes.\n" \
    "Use single quotes to avoid expanding the environment variable\n" \
    "before executing the loop.\n" \
    "This loop command accepts a list of variable values by specifying\n" \
    "commas (,) to separate values.\n" \
    "For multiple variables support, braces ({}) must be used to group each\n" \
    "set of variables and values for each round, and a colon (:) must\n" \
    "be used to separate variables and values within each of these group."

/*! Help for CLI command. */
#define BCMA_CLICMD_EACH_HELP_2 \
    "If one of the command iterations fails, the execution will stop,\n" \
    "unless the environment variable '_looperror' is set to 0."

/*! Examples for CLI command */
#define BCMA_CLICMD_EACH_EXAMPLES \
    "port=1,4,5,7 'echo $port'\n" \
    "color=red,blue,green 'echo $color'\n" \
    "{port:color}={1:red},{4:blue},{7:green} 'echo $port $color'"

/*!
 * \brief CLI 'each' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_each(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_EACH_H */
