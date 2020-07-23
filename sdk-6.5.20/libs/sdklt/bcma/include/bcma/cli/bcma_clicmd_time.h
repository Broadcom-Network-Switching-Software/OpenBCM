/*! \file bcma_clicmd_time.h
 *
 * CLI 'time' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_TIME_H
#define BCMA_CLICMD_TIME_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_TIME_DESC \
    "Measure the execution time for one or more CLI commands."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_TIME_SYNOP \
    "[<command> [<command>] ...]"

/*! Help for CLI command. */
#define BCMA_CLICMD_TIME_HELP \
    "If no commands are specified, the current time will be displayed.\n" \
    "Each command must be a single CLI token, i.e. if the command contains any\n" \
    "arguments, the command must be enclosed in quotation marks."

/*! Examples for CLI command. */
#define BCMA_CLICMD_TIME_EXAMPLES \
    "\"sleep 1\" \"sleep 2\""

/*!
 * \brief CLI 'time' command implementation.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_time(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_TIME_H */
