/*! \file bcma_bslcmd_add_cmds.c
 *
 * Add CLI commands for BSL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/bsl/bcma_bslcmd_debug.h>
#include <bcma/bsl/bcma_bslcmd_log.h>
#include <bcma/bsl/bcma_bslcmd_console.h>
#include <bcma/bsl/bcma_bslcmd.h>

static bcma_cli_command_t cmd_debug = {
    .name = "debug",
    .func = bcma_bslcmd_debug,
    .desc = BCMA_BSLCMD_DEBUG_DESC,
    .synop = BCMA_BSLCMD_DEBUG_SYNOP,
    .help = {
        BCMA_BSLCMD_DEBUG_HELP
#if BCMA_CLI_CONFIG_MAX_HELP_LINES > 1
      , BCMA_BSLCMD_DEBUG_HELP_2
#endif
    }
};

static bcma_cli_command_t cmd_log = {
    .name = "log",
    .func = bcma_bslcmd_log,
    .desc = BCMA_BSLCMD_LOG_DESC,
    .synop = BCMA_BSLCMD_LOG_SYNOP,
    .help = { BCMA_BSLCMD_LOG_HELP }
};

static bcma_cli_command_t cmd_console = {
    .name = "CONSole",
    .func = bcma_bslcmd_console,
    .desc = BCMA_BSLCMD_CONSOLE_DESC,
    .synop = BCMA_BSLCMD_CONSOLE_SYNOP,
    .help = { BCMA_BSLCMD_CONSOLE_HELP }
};

/*!
 * \brief Add default set of CLI commands for cliironment variables.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
int
bcma_bslcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_debug, 0);
    bcma_cli_add_command(cli, &cmd_log, 0);
    bcma_cli_add_command(cli, &cmd_console, 0);

    return 0;
}
