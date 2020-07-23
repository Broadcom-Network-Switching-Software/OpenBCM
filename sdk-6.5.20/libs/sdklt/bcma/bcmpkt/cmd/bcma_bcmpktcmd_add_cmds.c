/*! \file bcma_bcmpktcmd_add_cmds.c
 *
 * Add CLI commands support for Packet I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pktdev.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_knet.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_socket.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_tx.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_rx.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pkttest.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pkttrace.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pmddecode.h>

static bcma_cli_command_t shcmd_pktdev = {
    .name = "pktdev",
    .func = bcma_bcmpktcmd_pktdev,
    .desc = BCMA_BCMPKTCMD_PKTDEV_DESC,
    .synop = BCMA_BCMPKTCMD_PKTDEV_SYNOP,
    .help = { BCMA_BCMPKTCMD_PKTDEV_HELP },
    .examples = BCMA_BCMPKTCMD_PKTDEV_EXAMPLES
};

static bcma_cli_command_t shcmd_knet = {
    .name = "knet",
    .func = bcma_bcmpktcmd_knet,
    .desc = BCMA_BCMPKTCMD_KNET_DESC,
    .synop = BCMA_BCMPKTCMD_KNET_SYNOP,
    .help = { BCMA_BCMPKTCMD_KNET_HELP },
    .examples = BCMA_BCMPKTCMD_KNET_EXAMPLES
};

static bcma_cli_command_t shcmd_socket = {
    .name = "socket",
    .func = bcma_bcmpktcmd_socket,
    .desc = BCMA_BCMPKTCMD_SOCKET_DESC,
    .synop = BCMA_BCMPKTCMD_SOCKET_SYNOP,
    .help = { BCMA_BCMPKTCMD_SOCKET_HELP },
    .examples = BCMA_BCMPKTCMD_SOCKET_EXAMPLES
};

static bcma_cli_command_t shcmd_pkttx = {
    .name = "tx",
    .func = bcma_bcmpktcmd_tx,
    .desc = BCMA_BCMPKTCMD_TX_DESC,
    .synop = BCMA_BCMPKTCMD_TX_SYNOP,
    .help = { BCMA_BCMPKTCMD_TX_HELP },
    .examples = BCMA_BCMPKTCMD_TX_EXAMPLES
};

static bcma_cli_command_t shcmd_pktrx = {
    .name = "rx",
    .func = bcma_bcmpktcmd_rx,
    .desc = BCMA_BCMPKTCMD_RX_DESC,
    .synop = BCMA_BCMPKTCMD_RX_SYNOP,
    .help = { BCMA_BCMPKTCMD_RX_HELP },
    .examples = BCMA_BCMPKTCMD_RX_EXAMPLES
};

static bcma_cli_command_t shcmd_pkttest = {
    .name = "pkttest",
    .func = bcma_bcmpktcmd_pkttest,
    .desc = BCMA_BCMPKTCMD_PKTTEST_DESC,
    .synop = BCMA_BCMPKTCMD_PKTTEST_SYNOP,
    .help = { BCMA_BCMPKTCMD_PKTTEST_HELP },
    .examples = BCMA_BCMPKTCMD_PKTTEST_EXAMPLES
};

static bcma_cli_command_t shcmd_pkttrace = {
    .name = "pkttrace",
    .func = bcma_bcmpktcmd_pkttrace,
    .desc = BCMA_BCMPKTCMD_PKTTRACE_DESC,
    .synop = BCMA_BCMPKTCMD_PKTTRACE_SYNOP,
    .help = { BCMA_BCMPKTCMD_PKTTRACE_HELP },
    .examples = BCMA_BCMPKTCMD_PKTTRACE_EXAMPLES
};

static bcma_cli_command_t shcmd_pmddecode = {
    .name = "pmddecode",
    .func = bcma_bcmpktcmd_pmddecode,
    .desc = BCMA_BCMPKTCMD_PMDDECODE_DESC,
    .synop = BCMA_BCMPKTCMD_PMDDECODE_SYNOP,
    .help = { BCMA_BCMPKTCMD_PMDDECODE_HELP },
    .examples = BCMA_BCMPKTCMD_PMDDECODE_EXAMPLES
};

int
bcma_bcmpktcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_pktrx, 0);
    bcma_cli_add_command(cli, &shcmd_pkttx, 0);
    bcma_cli_add_command(cli, &shcmd_knet, 0);
    bcma_cli_add_command(cli, &shcmd_socket, 0);
    bcma_cli_add_command(cli, &shcmd_pktdev, 0);
    bcma_cli_add_command(cli, &shcmd_pkttest, 0);
    bcma_cli_add_command(cli, &shcmd_pkttrace, 0);
    bcma_cli_add_command(cli, &shcmd_pmddecode, 0);
    return 0;
}

