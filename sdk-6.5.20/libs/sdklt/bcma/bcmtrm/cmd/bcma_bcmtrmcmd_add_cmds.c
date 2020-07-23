/*! \file bcma_bcmtrmcmd_add_cmds.c
 *
 * Add CLI commands for the TRM component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/bcmtrm/bcma_bcmtrmcmd.h>

#include <bcma/bcmtrm/bcma_bcmtrmcmd_trm.h>

static bcma_cli_command_t shcmd_trm = {
    .name = "TRM",
    .func = bcma_bcmtrmcmd_trm,
    .desc = BCMA_BCMTRMCMD_TRM_DESC,
    .synop = BCMA_BCMTRMCMD_TRM_SYNOP,
    .help = { BCMA_BCMTRMCMD_TRM_HELP }
};

int
bcma_bcmtrmcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_trm, 0);

    return 0;
}
