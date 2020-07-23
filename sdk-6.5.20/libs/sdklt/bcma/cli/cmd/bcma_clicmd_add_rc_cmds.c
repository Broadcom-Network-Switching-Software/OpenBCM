/*! \file bcma_clicmd_add_rc_cmds.c
 *
 * CLI RC script support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_rccache.h>
#include <bcma/cli/bcma_clicmd_rcload.h>
#include <bcma/cli/bcma_clicmd.h>

/* CLI rc command set */
static bcma_cli_command_t cmd_rccache = {
    .name = "RCCache",
    .func = bcma_clicmd_rccache,
    .cleanup = bcma_clicmd_rccache_cleanup,
    .desc = BCMA_CLICMD_RCCACHE_DESC,
    .synop = BCMA_CLICMD_RCCACHE_SYNOP
};

static bcma_cli_command_t cmd_rcload = {
    .name = "RCLoad",
    .func = bcma_clicmd_rcload,
    .desc = BCMA_CLICMD_RCLOAD_DESC,
    .synop = BCMA_CLICMD_RCLOAD_SYNOP,
    .help = {
        BCMA_CLICMD_RCLOAD_HELP
#if BCMA_CLI_CONFIG_MAX_HELP_LINES > 1
      , BCMA_CLICMD_RCLOAD_HELP_2
#endif
    },
    .examples = BCMA_CLICMD_RCLOAD_EXAMPLES
};

int
bcma_clicmd_add_rc_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_rccache, 0);
    bcma_cli_add_command(cli, &cmd_rcload, 0);

    return 0;
}
