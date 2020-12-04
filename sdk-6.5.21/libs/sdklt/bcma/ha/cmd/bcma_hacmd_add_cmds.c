/*! \file bcma_hacmd_add_cmds.c
 *
 * Add CLI commands for HA.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/ha/bcma_hacmd.h>

#include <bcma/ha/bcma_hacmd_ha.h>

static bcma_cli_command_t shcmd_ha = {
    .name = "HA",
    .func = bcma_hacmd_ha,
    .desc = BCMA_HACMD_HA_DESC,
    .synop = BCMA_HACMD_HA_SYNOP,
    .help = { BCMA_HACMD_HA_HELP }
};

int
bcma_hacmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_ha, 0);

    return 0;
}
