/*! \file bcma_cfgdbcmd_add_cmds.c
 *
 * CLI Configuration database manager
 *
 * Add CLI commands to control the configuration data manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/cfgdb/bcma_cfgdbcmd_config.h>
#include <bcma/cfgdb/bcma_cfgdbcmd.h>

static bcma_cli_command_t cmd_config = {
    .name = "config",
    .func = bcma_cfgdbcmd_config,
    .desc = BCMA_CFGDBCMD_CONFIG_DESC,
    .synop = BCMA_CFGDBCMD_CONFIG_SYNOP,
    .help = { BCMA_CFGDBCMD_CONFIG_HELP }
};

int
bcma_cfgdbcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_config, 0);

    return 0;
}
