/*! \file bcma_bcmpccmd_add_cmds.c
 *
 * Add CLI commands for Port Control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmpc/bcma_bcmpccmd.h>

#include <bcma/bcmpc/bcma_bcmpccmd_portstatus.h>

static bcma_cli_command_t shcmd_portstatus = {
    .name = "PortStatus",
    .func = bcma_bcmpccmd_portstatus,
    .desc = BCMA_BCMPCCMD_PORTSTATUS_DESC,
    .synop = BCMA_BCMPCCMD_PORTSTATUS_SYNOP,
    .help = { BCMA_BCMPCCMD_PORTSTATUS_HELP },
    .examples = BCMA_BCMPCCMD_PORTSTATUS_EXAMPLES
};

int
bcma_bcmpccmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_portstatus, 0);

    return 0;
}
