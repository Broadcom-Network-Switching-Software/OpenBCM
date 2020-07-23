/*! \file bcma_bcmltcmd_add_cmds.c
 *
 * Add CLI commands support for BCMLT module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcma/bcmlt/bcma_bcmltcmd_lt.h>
#include <bcma/bcmlt/bcma_bcmltcmd_pt.h>
#include <bcma/bcmlt/bcma_bcmltcmd_ltcapture.h>
#include <bcma/bcmlt/bcma_bcmltcmd.h>

static bcma_cli_command_t cmd_lt = {
    .name = "lt",
    .func = bcma_bcmltcmd_lt,
    .desc = BCMA_BCMLTCMD_LT_DESC,
    .synop = BCMA_BCMLTCMD_LT_SYNOP,
    .help = { BCMA_BCMLTCMD_LT_HELP },
    .examples = BCMA_BCMLTCMD_LT_EXAMPLES
};

static bcma_cli_command_t cmd_pt = {
    .name = "pt",
    .func = bcma_bcmltcmd_pt,
    .desc = BCMA_BCMLTCMD_PT_DESC,
    .synop = BCMA_BCMLTCMD_PT_SYNOP,
    .help  = { BCMA_BCMLTCMD_PT_HELP },
    .examples = BCMA_BCMLTCMD_PT_EXAMPLES
};

static bcma_cli_command_t cmd_ltcapture = {
    .name = "LtCAPture",
    .func = bcma_bcmltcmd_ltcapture,
    .cleanup = bcma_bcmltcmd_ltcapture_cleanup,
    .desc = BCMA_BCMLTCMD_LTCAPTURE_DESC,
    .synop = BCMA_BCMLTCMD_LTCAPTURE_SYNOP,
    .help = { BCMA_BCMLTCMD_LTCAPTURE_HELP }
};

int
bcma_bcmltcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_lt, 0);
    bcma_cli_add_command(cli, &cmd_pt, 0);
    bcma_cli_add_command(cli, &cmd_ltcapture, 0);

    return 0;
}
