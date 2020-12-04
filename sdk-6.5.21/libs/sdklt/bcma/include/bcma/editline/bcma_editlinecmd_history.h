/*! \file bcma_editlinecmd_history.h
 *
 * CLI 'history' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_EDITLINECMD_HISTORY_H
#define BCMA_EDITLINECMD_HISTORY_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_EDITLINECMD_HISTORY_DESC \
    "Display command history."

/*! Syntax for CLI command. */
#define BCMA_EDITLINECMD_HISTORY_SYNOP \
    "[count]"

/*! Help for CLI command. */
#define BCMA_EDITLINECMD_HISTORY_HELP \
    "If count is given, output is limited to at most count lines.\n" \
    "History expansion is supported by !n or !-n for referring to\n" \
    "command line n or the current command minus n."

/*! Examples for CLI command. */
#define BCMA_EDITLINECMD_HISTORY_EXAMPLES \
    "\n" \
    "7"

/*!
 * \brief CLI 'history' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_editlinecmd_history(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_EDITLINECMD_HISTORY_H */
