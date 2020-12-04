/*! \file bcma_cintcmd_add_cmds.c
 *
 * Add CLI commands support for CINT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/cint/bcma_cintcmd.h>
#include <bcma/cint/bcma_cintcmd_cint.h>

static bcma_cli_command_t shcmd_cint = {
    .name = "cint",
    .func = bcma_cintcmd_cint,
    .desc = BCMA_CINTCMD_CINT_DESC
};

int
bcma_cintcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_cint, 0);
    return 0;
}
