/*! \file bcma_clicmd_add_dir_cmds.c
 *
 * CLI directory commands support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/cli/bcma_clicmd_cd.h>
#include <bcma/cli/bcma_clicmd_pwd.h>
#include <bcma/cli/bcma_clicmd_ls.h>
#include <bcma/cli/bcma_clicmd_mkdir.h>
#include <bcma/cli/bcma_clicmd_rmdir.h>
#include <bcma/cli/bcma_clicmd.h>

/* CLI directory command set */
static bcma_cli_command_t cmd_cd = {
    .name = "cd",
    .func = bcma_clicmd_cd,
    .desc = BCMA_CLICMD_CD_DESC,
    .synop = BCMA_CLICMD_CD_SYNOP,
    .help = { BCMA_CLICMD_CD_HELP }
};

static bcma_cli_command_t cmd_pwd = {
    .name = "pwd",
    .func = bcma_clicmd_pwd,
    .desc = BCMA_CLICMD_PWD_DESC
};

static bcma_cli_command_t cmd_ls = {
    .name = "ls",
    .func = bcma_clicmd_ls,
    .desc = BCMA_CLICMD_LS_DESC,
    .synop = BCMA_CLICMD_LS_SYNOP,
    .help = { BCMA_CLICMD_LS_HELP }
};

static bcma_cli_command_t cmd_mkdir = {
    .name = "mkdir",
    .func = bcma_clicmd_mkdir,
    .desc = BCMA_CLICMD_MKDIR_DESC,
    .synop = BCMA_CLICMD_MKDIR_SYNOP
};

static bcma_cli_command_t cmd_rmdir = {
    .name = "rmdir",
    .func = bcma_clicmd_rmdir,
    .desc = BCMA_CLICMD_RMDIR_DESC,
    .synop = BCMA_CLICMD_RMDIR_SYNOP,
    .help = { BCMA_CLICMD_RMDIR_HELP }
};

int
bcma_clicmd_add_dir_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_cd, 0);
    bcma_cli_add_command(cli, &cmd_pwd, 0);
    bcma_cli_add_command(cli, &cmd_ls, 0);
    bcma_cli_add_command(cli, &cmd_mkdir, 0);
    bcma_cli_add_command(cli, &cmd_rmdir, 0);

    return 0;
}
