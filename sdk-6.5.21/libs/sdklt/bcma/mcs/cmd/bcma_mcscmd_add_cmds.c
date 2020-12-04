/*! \file bcma_mcscmd_add_cmds.c
 *
 * Add CLI commands support for MCS module
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/mcs/bcma_mcscmd_mcs.h>
#include <bcma/mcs/bcma_mcscmd.h>

static bcma_cli_command_t cmd_mcs = {
    .name = "mcs",
    .func = bcma_mcscmd_mcs,
    .desc = BCMA_MCSCMD_MCS_DESC,
    .synop = BCMA_MCSCMD_MCS_SYNOP,
    .help = { BCMA_MCSCMD_MCS_HELP },
    .examples = BCMA_MCSCMD_MCS_EXAMPLES
};

int
bcma_mcscmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_mcs, 0);

    return 0;
}

