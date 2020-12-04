/*! \file bcma_bcmmgmtcmd_add_cmds.c
 *
 * Add CLI commands for the management component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/bcmmgmt/bcma_bcmmgmtcmd.h>

#include <bcma/bcmmgmt/bcma_bcmmgmtcmd_dev.h>

static bcma_cli_command_t shcmd_dev = {
    .name = "dev",
    .func = bcma_bcmmgmtcmd_dev,
    .desc = BCMA_BCMMGMTCMD_DEV_DESC,
    .synop = BCMA_BCMMGMTCMD_DEV_SYNOP,
    .help = { BCMA_BCMMGMTCMD_DEV_HELP }
};

int
bcma_bcmmgmtcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_dev, 0);

    return 0;
}
