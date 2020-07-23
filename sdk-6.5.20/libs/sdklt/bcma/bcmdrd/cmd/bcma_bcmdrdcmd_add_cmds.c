/*! \file bcma_bcmdrdcmd_add_cmds.c
 *
 * Add CLI commands for DRD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/bcmdrd/bcma_bcmdrdcmd.h>

#include <bcma/bcmdrd/bcma_bcmdrdcmd_unit.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd_symtbl.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd_pbmp.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd_dma.h>

static bcma_cli_command_t shcmd_unit = {
    .name = "unit",
    .func = bcma_bcmdrdcmd_unit,
    .desc = BCMA_BCMDRDCMD_UNIT_DESC,
    .synop = BCMA_BCMDRDCMD_UNIT_SYNOP,
    .help = { BCMA_BCMDRDCMD_UNIT_HELP },
    .examples = BCMA_BCMDRDCMD_UNIT_EXAMPLES
};

static bcma_cli_command_t shcmd_symtbl = {
    .name = "symtbl",
    .func = bcma_bcmdrdcmd_symtbl,
    .desc = BCMA_BCMDRDCMD_SYMTBL_DESC,
    .synop = BCMA_BCMDRDCMD_SYMTBL_SYNOP,
    .help = { BCMA_BCMDRDCMD_SYMTBL_HELP },
    .examples = BCMA_BCMDRDCMD_SYMTBL_EXAMPLES
};

static bcma_cli_command_t shcmd_pbmp = {
    .name = "pbmp",
    .func = bcma_bcmdrdcmd_pbmp,
    .desc = BCMA_BCMDRDCMD_PBMP_DESC,
    .synop = BCMA_BCMDRDCMD_PBMP_SYNOP,
    .help = { BCMA_BCMDRDCMD_PBMP_HELP },
    .examples = BCMA_BCMDRDCMD_PBMP_EXAMPLES
};

static bcma_cli_command_t shcmd_dma = {
    .name = "DMA",
    .func = bcma_bcmdrdcmd_dma,
    .cleanup = bcma_bcmdrdcmd_dma_cleanup,
    .desc = BCMA_BCMDRDCMD_DMA_DESC,
    .synop = BCMA_BCMDRDCMD_DMA_SYNOP,
    .help = { BCMA_BCMDRDCMD_DMA_HELP },
    .examples = BCMA_BCMDRDCMD_DMA_EXAMPLES
};

int
bcma_bcmdrdcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_unit, 0);
    bcma_cli_add_command(cli, &shcmd_symtbl, 0);
    bcma_cli_add_command(cli, &shcmd_pbmp, 0);
    bcma_cli_add_command(cli, &shcmd_dma, 0);

    return 0;
}
