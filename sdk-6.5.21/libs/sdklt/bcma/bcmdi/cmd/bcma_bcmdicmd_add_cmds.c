/*! \file bcma_bcmdicmd_add_cmds.c
 *
 * Add CLI commands support for device initialization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/bcmdi/bcma_bcmdicmd.h>
#include <bcma/bcmdi/bcma_bcmdicmd_di.h>

static bcma_cli_command_t shcmd_di = {
    .name = "di",
    .func = bcma_bcmdicmd_di,
    .desc = BCMA_BCMDICMD_DI_DESC,
    .synop = BCMA_BCMDICMD_DI_SYNOP,
    .help = { BCMA_BCMDICMD_DI_HELP },
    .examples = BCMA_BCMDICMD_DI_EXAMPLES
};

int
bcma_bcmdicmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_di, 0);
    return 0;
}

