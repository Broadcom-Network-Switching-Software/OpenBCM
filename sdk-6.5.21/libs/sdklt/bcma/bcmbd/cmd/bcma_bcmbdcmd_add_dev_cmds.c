/*! \file bcma_bcmbdcmd_add_dev_cmds.c
 *
 * Add CLI commands for device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/cli/bcma_cli.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_dev.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_bdctl.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_qspi.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_ts.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_tsfifo.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_fifodma.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_sbusdma.h>

static bcma_cli_command_t shcmd_bdctl = {
    .name = "bdctl",
    .func = bcma_bcmbdcmd_bdctl,
    .desc = BCMA_BCMBDCMD_BDCTL_DESC,
    .synop = BCMA_BCMBDCMD_BDCTL_SYNOP,
};

static bcma_cli_command_t shcmd_qspi = {
    .name = "QSPI",
    .func = bcma_bcmbdcmd_qspi,
    .desc = BCMA_BCMBDCMD_QSPI_DESC,
    .synop = BCMA_BCMBDCMD_QSPI_SYNOP,
    .help = { BCMA_BCMBDCMD_QSPI_HELP },
    .examples = BCMA_BCMBDCMD_QSPI_EXAMPLES
};

static bcma_cli_command_t shcmd_ts = {
    .name = "TS",
    .func = bcma_bcmbdcmd_ts,
    .desc = BCMA_BCMBDCMD_TS_DESC,
    .synop = BCMA_BCMBDCMD_TS_SYNOP,
    .help = { BCMA_BCMBDCMD_TS_HELP },
};

static bcma_cli_command_t shcmd_tsfifo = {
    .name = "TimeSyncFIFO",
    .func = bcma_bcmbdcmd_tsfifo,
    .desc = BCMA_BCMBDCMD_TSFIFO_DESC,
    .synop = BCMA_BCMBDCMD_TSFIFO_SYNOP,
    .help = { BCMA_BCMBDCMD_TSFIFO_HELP },
    .examples = BCMA_BCMBDCMD_TSFIFO_EXAMPLES
};

static bcma_cli_command_t shcmd_fifodma = {
    .name = "fifodma",
    .func = bcma_bcmbdcmd_fifodma,
    .desc = BCMA_FIFODMA_DESC,
    .synop = BCMA_FIFODMA_SYNOP,
    .help = { BCMA_FIFODMA_HELP },
    .examples = BCMA_FIFODMA_EXAMPLES
};

static bcma_cli_command_t shcmd_sbusdma = {
    .name = "sbusdma",
    .func = bcma_bcmbdcmd_sbusdma,
    .desc = BCMA_SBUSDMA_DESC,
    .synop = BCMA_SBUSDMA_SYNOP,
    .help = { BCMA_SBUSDMA_HELP },
    .examples = BCMA_SBUSDMA_EXAMPLES
};

void
bcma_bcmbdcmd_add_dev_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_bdctl, 0);
    bcma_cli_add_command(cli, &shcmd_fifodma, 0);
    bcma_cli_add_command(cli, &shcmd_sbusdma, 0);
    bcma_cli_add_command(cli, &shcmd_ts, 0);
    bcma_cli_add_command(cli, &shcmd_qspi, BCMDRD_FT_CMICX);
    bcma_cli_add_command(cli, &shcmd_tsfifo, BCMDRD_FT_CMICX2);
}
