/*! \file bcma_envcmd_add_cmds.c
 *
 * CLI Environment
 *
 * Add CLI commands to control the environment.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/env/bcma_envcmd_printenv.h>
#include <bcma/env/bcma_envcmd_setenv.h>
#include <bcma/env/bcma_envcmd_local.h>
#include <bcma/env/bcma_envcmd.h>

static bcma_cli_command_t cmd_printenv = {
    .name = "printenv",
    .func = bcma_envcmd_printenv,
    .desc = BCMA_ENVCMD_PRINTENV_DESC
};

static bcma_cli_command_t cmd_setenv = {
    .name = "setenv",
    .func = bcma_envcmd_setenv,
    .desc = BCMA_ENVCMD_SETENV_DESC,
    .synop = BCMA_ENVCMD_SETENV_SYNOP,
    .help = { BCMA_ENVCMD_SETENV_HELP }
};

static bcma_cli_command_t cmd_local = {
    .name = "local",
    .func = bcma_envcmd_local,
    .desc = BCMA_ENVCMD_LOCAL_DESC,
    .synop = BCMA_ENVCMD_LOCAL_SYNOP,
    .help = { BCMA_ENVCMD_LOCAL_HELP }
};

int
bcma_envcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_printenv, 0);
    bcma_cli_add_command(cli, &cmd_setenv, 0);
    bcma_cli_add_command(cli, &cmd_local, 0);

    return 0;
}
