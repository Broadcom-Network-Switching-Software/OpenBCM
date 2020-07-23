/*! \file bcma_clicmd_add_basic_cmds.c
 *
 * CLI basic commands support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_quit.h>
#include <bcma/cli/bcma_clicmd_help.h>
#include <bcma/cli/bcma_clicmd_bhelp.h>
#include <bcma/cli/bcma_clicmd.h>

/* CLI default command set */
static bcma_cli_command_t cmd_quit = {
    .name = "Quit",
    .func = bcma_clicmd_quit,
    .desc = BCMA_CLICMD_QUIT_DESC
};

static bcma_cli_command_t cmd_exit = {
    .name = "Exit",
    .func = bcma_clicmd_quit,
    .desc = BCMA_CLICMD_QUIT_DESC
};

static bcma_cli_command_t cmd_help = {
    .name = "Help",
    .func = bcma_clicmd_help,
    .desc = BCMA_CLICMD_HELP_DESC,
    .synop = BCMA_CLICMD_HELP_SYNOP
};

static bcma_cli_command_t cmd_bhelp = {
    .name = "?",
    .func = bcma_clicmd_bhelp,
    .desc = BCMA_CLICMD_BHELP_DESC
};

int
bcma_clicmd_add_basic_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_quit, 0);
    bcma_cli_add_command(cli, &cmd_exit, 0);
    bcma_cli_add_command(cli, &cmd_help, 0);
    bcma_cli_add_command(cli, &cmd_bhelp, 0);

    return 0;
}
