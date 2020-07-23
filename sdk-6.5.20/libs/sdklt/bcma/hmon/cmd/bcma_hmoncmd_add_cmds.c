/*! \file bcma_hmoncmd_add_cmds.c
 *
 * Add CLI commands for hmon.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/hmon/bcma_hmoncmd.h>
#include <bcma/hmon/bcma_hmoncmd_hmon.h>

static bcma_cli_command_t shcmd_hmon = {
    .name = "hmon",
    .func = bcma_hmoncmd_hmon,
    .desc = BCMA_HMONCMD_CMD_DESC,
    .synop = BCMA_HMONCMD_CMD_SYNOP,
    .help = { BCMA_HMONCMD_CMD_HELP },
    .examples = BCMA_HMONCMD_CMD_EXAMPLES
};

int
bcma_hmoncmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_hmon, 0);

    return 0;
}

