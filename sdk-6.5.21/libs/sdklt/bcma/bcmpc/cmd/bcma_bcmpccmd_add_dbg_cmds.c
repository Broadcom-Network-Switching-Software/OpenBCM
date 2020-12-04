/*! \file bcma_bcmpccmd_add_dbg_cmds.c
 *
 * Add CLI debug commands for Port Control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmpc/bcma_bcmpccmd.h>

#include <bcma/bcmpc/bcma_bcmpccmd_pcmmu.h>

static bcma_cli_command_t shcmd_pcmmu = {
    .name = "pcmmu",
    .func = bcma_bcmpccmd_pcmmu,
    .desc = BCMA_BCMPCCMD_PCMMU_DESC,
    .synop = BCMA_BCMPCCMD_PCMMU_SYNOP,
    .help = { BCMA_BCMPCCMD_PCMMU_HELP }
};

int
bcma_bcmpccmd_add_dbg_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_pcmmu, 0);

    return 0;
}
