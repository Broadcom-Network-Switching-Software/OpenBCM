/*! \file bcma_bcmptmcmd_add_cmds.c
 *
 * Add CLI commands for the management component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/bcmptm/bcma_bcmptmcmd.h>
#include <bcma/bcmptm/bcma_bcmptmcmd_ptm.h>
#include <bcma/bcmptm/bcma_bcmptmcmd_alpm.h>
#include <bcma/bcmptm/bcma_bcmptmcmd_sbr.h>

static bcma_cli_command_t shcmd_ptm = {
    .name = "ptm",
    .func = bcma_bcmptmcmd_ptm,
    .desc = BCMA_BCMPTMCMD_PTM_DESC,
    .synop = BCMA_BCMPTMCMD_PTM_SYNOP,
    .help = { BCMA_BCMPTMCMD_PTM_HELP },
    .examples = BCMA_BCMPTMCMD_PTM_EXAMPLES
};


static bcma_cli_command_t shcmd_alpm = {
    .name = "alpm",
    .func = bcma_bcmptmcmd_alpm,
    .desc = BCMA_BCMPTMCMD_ALPM_DESC,
    .synop = BCMA_BCMPTMCMD_ALPM_SYNOP,
    .help = { BCMA_BCMPTMCMD_ALPM_HELP },
    .examples = BCMA_BCMPTMCMD_ALPM_EXAMPLES
};

static bcma_cli_command_t shcmd_sbr = {
    .name = "sbr",
    .func = bcma_bcmptmcmd_sbr,
    .desc = BCMA_BCMPTMCMD_SBR_DESC,
    .synop = BCMA_BCMPTMCMD_SBR_SYNOP,
    .help = { BCMA_BCMPTMCMD_SBR_HELP },
    .examples = BCMA_BCMPTMCMD_SBR_EXAMPLES
};

int
bcma_bcmptmcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_ptm, 0);
    bcma_cli_add_command(cli, &shcmd_alpm, 0);
    bcma_cli_add_command(cli, &shcmd_sbr, 0);

    return 0;
}
