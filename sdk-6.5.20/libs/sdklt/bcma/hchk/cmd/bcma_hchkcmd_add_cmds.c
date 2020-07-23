/*! \file bcma_hchkcmd_add_cmds.c
 *
 * Add CLI commands for hchk.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/hchk/bcma_hchkcmd.h>
#include <bcma/hchk/bcma_hchkcmd_hchk.h>

static bcma_cli_command_t shcmd_hchk = {
    .name = "HealthCHecK",
    .func = bcma_hchkcmd_hchk,
    .desc = BCMA_HCHKCMD_CMD_DESC,
    .synop = BCMA_HCHKCMD_CMD_SYNOP,
    .help = { BCMA_HCHKCMD_CMD_HELP },
    .examples = BCMA_HCHKCMD_CMD_EXAMPLES
};

int
bcma_hchkcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_hchk, 0);

    return 0;
}

