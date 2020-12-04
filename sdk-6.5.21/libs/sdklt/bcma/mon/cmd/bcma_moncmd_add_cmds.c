/*! \file bcma_moncmd_add_cmds.c
 *
 * Add CLI commands for monitor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/mon/bcma_moncmd.h>
#include <bcma/mon/bcma_moncmd_hchk.h>
#include <bcma/mon/bcma_moncmd_hmon.h>
#include <bcma/mon/bcma_moncmd_ft.h>

static bcma_cli_command_t shcmd_hchk = {
    .name = "HealthCHecK",
    .func = bcma_moncmd_hchk,
    .desc = BCMA_MONCMD_HCHK_DESC,
    .help = { BCMA_MONCMD_HCHK_HELP },
};

static bcma_cli_command_t shcmd_hmon = {
    .name = "HealthMONitor",
    .func = bcma_moncmd_hmon,
    .desc = BCMA_MONCMD_HMON_DESC,
    .synop = BCMA_MONCMD_HMON_SYNOP,
    .help = { BCMA_MONCMD_HMON_HELP },
    .examples = BCMA_MONCMD_HMON_EXAMPLES
};

static bcma_cli_command_t shcmd_ft = {
    .name = "FlowTracker",
    .func = bcma_moncmd_ft,
    .desc = BCMA_MONCMD_FT_DESC,
    .synop = BCMA_MONCMD_FT_SYNOP,
    .help = { BCMA_MONCMD_FT_HELP },
    .examples = BCMA_MONCMD_FT_EXAMPLES
};

int
bcma_moncmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_hchk, 0);
    bcma_cli_add_command(cli, &shcmd_hmon, 0);
    bcma_cli_add_command(cli, &shcmd_ft, 0);

    return 0;
}

