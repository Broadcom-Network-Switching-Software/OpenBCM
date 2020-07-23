/*! \file bcma_clicmd_echo.h
 *
 * CLI 'echo' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_ECHO_H
#define BCMA_CLICMD_ECHO_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_ECHO_DESC \
    "Echo command line."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_ECHO_SYNOP \
    "[-n] <Text>"

/*! Help for CLI command. */
#define BCMA_CLICMD_ECHO_HELP \
    "Echo whatever follows on command line, followed by a newline.\n" \
    "The newline may be suppressed by using the -n option.\n"

/*!
 * \brief CLI 'echo' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_echo(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_ECHO_H */
