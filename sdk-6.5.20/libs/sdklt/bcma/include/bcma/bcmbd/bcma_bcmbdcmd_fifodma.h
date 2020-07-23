/*! \file bcma_bcmbdcmd_fifodma.h
 *
 * CLI command related to DMA FIFO access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_FIFODMA_H
#define BCMA_BCMBDCMD_FIFODMA_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_FIFODMA_DESC \
    "FIFODMA operations."

/*! Syntax for CLI command. */
#define BCMA_FIFODMA_SYNOP \
    "<subcmd> [<action>] [options]"

/*! Help for CLI command. */
#define BCMA_FIFODMA_HELP \
    "This command is used for popping data entries from DMA FIFOs.\n" \
    "The corresponding eviction (and interrupt) must be enabled through\n" \
    "the PT command. The command support the following sub-commands\n" \
    "and options:\n" \
    "    ChanConfig <chan> [options] - Configure a FIFO DMA channel.\n" \
    "        Memory=<name>    - FIFO memory name.\n" \
    "        Pipe=<value>     - Pipe index.\n" \
    "        SiZe=<value>     - FIFO size.\n" \
    "        THReshold=<value> - Interrupt threshold [1-100].\n" \
    "    START <chan> - Start a channel Eviction.\n" \
    "    Poll <chan>  - Poll data out from a channel FIFO, for interupt\n" \
    "                   disabled case.\n" \
    "    STOP <chan>  - Stop a channel Eviction.\n" \
    "    STatus       - Show DMA FIFO status."


/*! Examples for CLI command. */
#define BCMA_FIFODMA_EXAMPLES \
    "cc 4 m=ETRAP_EVENT_FIFO_INST0m p=0\n" \
    "start 4\n" \
    "stop 4 \n" \
    "poll 4 \n" \
    "status"

/*!
 * \brief FIFODMA command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_fifodma(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_FIFODMA_H */

