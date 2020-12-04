/*! \file bcma_bslcmd_log.h
 *
 * CLI 'log' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLCMD_LOG_H
#define BCMA_BSLCMD_LOG_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BSLCMD_LOG_DESC \
    "Enable/Disable logging and set log file."

/*! Syntax for CLI command. */
#define BCMA_BSLCMD_LOG_SYNOP \
    "[file=<filename>] [append=<yes|no>] [quiet=<yes|no>] [on|off]"

/*! Help for CLI command. */
#define BCMA_BSLCMD_LOG_HELP \
    "Specify a log file or turn file logging on/off. Specifying a log file\n" \
    "implicitly turns on logging unless \'off\' is specified as well.\n" \
    "The \'quiet\' option can be used to suppress informational messages\n" \
    "from the command itself. No other console output is affected by this\n" \
    "option."

/*!
 * \brief CLI command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bslcmd_log(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BSLCMD_LOG_H */
