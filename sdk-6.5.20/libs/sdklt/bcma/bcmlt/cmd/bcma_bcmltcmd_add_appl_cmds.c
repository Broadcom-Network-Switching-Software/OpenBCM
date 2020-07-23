/*! \file bcma_bcmltcmd_add_appl_cmds.c
 *
 * Add CLI commands support for BCMLT Logical tables application.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmlt/bcma_bcmltcmd_counter.h>
#include <bcma/bcmlt/bcma_bcmltcmd_config.h>
#include <bcma/bcmlt/bcma_bcmltcmd.h>

static bcma_cli_command_t cmd_counter = {
    .name = "CounTeR",
    .func = bcma_bcmltcmd_counter,
    .cleanup = bcma_bcmltcmd_counter_cleanup,
    .desc = BCMA_BCMLTCMD_COUNTER_DESC,
    .synop = BCMA_BCMLTCMD_COUNTER_SYNOP,
    .help = { BCMA_BCMLTCMD_COUNTER_HELP },
    .examples = BCMA_BCMLTCMD_COUNTER_EXAMPLES
};

static bcma_cli_command_t cmd_config = {
    .name = "ConFiG",
    .func = bcma_bcmltcmd_config,
    .desc = BCMA_BCMLTCMD_CONFIG_DESC,
    .synop = BCMA_BCMLTCMD_CONFIG_SYNOP,
    .help = { BCMA_BCMLTCMD_CONFIG_HELP },
};

int
bcma_bcmltcmd_add_appl_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_counter, 0);
    bcma_cli_add_command(cli, &cmd_config, 0);

    return 0;
}
