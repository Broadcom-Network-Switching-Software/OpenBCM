/*! \file bcma_editlinecmd_add_cmds.c
 *
 * Add CLI commands for editline module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/editline/bcma_editlinecmd.h>
#include <bcma/editline/bcma_editlinecmd_history.h>

static bcma_cli_command_t cmd_history = {
    .name = "history",
    .func = bcma_editlinecmd_history,
    .desc = BCMA_EDITLINECMD_HISTORY_DESC,
    .synop = BCMA_EDITLINECMD_HISTORY_SYNOP,
    .help = {
        BCMA_EDITLINECMD_HISTORY_HELP
     },
    .examples = BCMA_EDITLINECMD_HISTORY_EXAMPLES
};

int
bcma_editlinecmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_history, 0);

    return 0;
}
