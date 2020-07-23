/*! \file bcma_clicmd_sleep.h
 *
 * CLI 'sleep' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_SLEEP_H
#define BCMA_CLICMD_SLEEP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_SLEEP_DESC \
    "Suspend the CLI task for specified amount of time."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_SLEEP_SYNOP \
    "[quiet] [<seconds> [<useconds>]]"

/*! Help for CLI command. */
#define BCMA_CLICMD_SLEEP_HELP \
    "If \"quiet\" is specified, no output is generated, otherwise the\n" \
    "message \"sleeping for # seconds\" is printed. <seconds> specifies\n" \
    "the number of seconds to sleep. <useconds> specifies an additional\n" \
    "number of microseconds to sleep (0-999999).\n"

/*!
 * \brief CLI 'sleep' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_sleep(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_SLEEP_H */
