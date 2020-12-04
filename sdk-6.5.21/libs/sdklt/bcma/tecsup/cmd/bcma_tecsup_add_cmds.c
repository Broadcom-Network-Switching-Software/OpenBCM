/*! \file bcma_tecsupcmd_add_cmds.c
 *
 * Add CLI commands for techsupport.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/tecsup/bcma_tecsupcmd.h>
#include <bcma/tecsup/bcma_tecsupcmd_tecsup.h>

static bcma_cli_command_t shcmd_tecsup = {
    .name = "TEChSUPport",
    .func = bcma_tecsupcmd_tecsup,
    .cleanup = bcma_tecsupcmd_tecsup_cleanup,
    .desc = BCMA_TECSUPCMD_TECSUP_DESC,
    .synop = BCMA_TECSUPCMD_TECSUP_SYNOP,
    .help = { BCMA_TECSUPCMD_TECSUP_HELP },
    .examples = BCMA_TECSUPCMD_TECSUP_EXAMPLES
};

int
bcma_tecsupcmd_add_cmds(bcma_cli_t *cli)
{
    return bcma_cli_add_command(cli, &shcmd_tecsup, 0);
}
