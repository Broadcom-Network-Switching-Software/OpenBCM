/*! \file bcma_pciecmd_add_cmds.c
 *
 * Add CLI commands for PCIe.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/pcie/bcma_pciecmd.h>
#include <bcma/pcie/bcma_pciecmd_pciephy.h>

static bcma_cli_command_t shcmd_pciephy = {
    .name = "PCIEphy",
    .func = bcma_pciecmd_pciephy,
    .desc = BCMA_PCIECMD_PCIEPHY_DESC,
    .synop = BCMA_PCIECMD_PCIEPHY_SYNOP,
    .help = { BCMA_PCIECMD_PCIEPHY_HELP },
    .examples = BCMA_PCIECMD_PCIEPHY_EXAMPLES
};

int
bcma_pciecmd_add_cmds(bcma_cli_t *cli)
{
    return bcma_cli_add_command(cli, &shcmd_pciephy, BCMDRD_FT_CMICX);
}
