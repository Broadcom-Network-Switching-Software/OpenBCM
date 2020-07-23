/*! \file bcma_bslcmd_console.h
 *
 * CLI 'console' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLCMD_CONSOLE_H
#define BCMA_BSLCMD_CONSOLE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BSLCMD_CONSOLE_DESC \
    "Control console options."

/*! Syntax for CLI command. */
#define BCMA_BSLCMD_CONSOLE_SYNOP \
    "on|off|cli"

/*! Help for CLI command. */
#define BCMA_BSLCMD_CONSOLE_HELP \
    "Turn on or off output to the console.\n" \
    "This does not affect logging to other output sinks.\n" \
    "Use 'cli' mode to hide driver messages not originating from the CLI."

/*!
 * \brief CLI command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bslcmd_console(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BSLCMD_CONSOLE_H */
