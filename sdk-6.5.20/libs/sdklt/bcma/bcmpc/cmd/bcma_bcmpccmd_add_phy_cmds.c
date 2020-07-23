/*! \file bcma_bcmpccmd_add_phy_cmds.c
 *
 * Add CLI commands for "phy".
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmpc/bcma_bcmpccmd.h>

#include <bcma/bcmpc/bcma_bcmpccmd_phy.h>
#include <bcma/bcmpc/bcma_bcmpccmd_phydiag.h>

static bcma_cli_command_t shcmd_phy = {
    .name = "PHY",
    .func = bcma_bcmpccmd_phy,
    .cleanup = bcma_bcmpccmd_phy_cleanup,
    .desc = BCMA_BCMPCCMD_PHY_DESC,
    .synop = BCMA_BCMPCCMD_PHY_SYNOP,
    .help = { BCMA_BCMPCCMD_PHY_HELP },
    .examples = BCMA_BCMPCCMD_PHY_EXAMPLES
};

static bcma_cli_command_t shcmd_phydiag = {
    .name = "PHYDiag",
    .func = bcma_bcmpccmd_phydiag,
    .cleanup = bcma_bcmpccmd_phydiag_cleanup,
    .desc = BCMA_BCMPCCMD_PHYDIAG_DESC,
    .synop = BCMA_BCMPCCMD_PHYDIAG_SYNOP,
    .help = { BCMA_BCMPCCMD_PHYDIAG_HELP },
    .examples = BCMA_BCMPCCMD_PHYDIAG_EXAMPLES
};

int
bcma_bcmpccmd_add_phy_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_phy, 0);
    bcma_cli_add_command(cli, &shcmd_phydiag, 0);

    return 0;
}
