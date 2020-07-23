/*! \file bcma_bcmbdcmd_sbusdma.h
 *
 * CLI command related to SBUS DMA access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_SBUSDMA_H
#define BCMA_BCMBDCMD_SBUSDMA_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_SBUSDMA_DESC \
    "FIFODMA operations."

/*! Syntax for CLI command. */
#define BCMA_SBUSDMA_SYNOP \
    "<subcmd> [<action>] [options]"

/*! Help for CLI command. */
#define BCMA_SBUSDMA_HELP \
    "This command is used for access data entries from SBUSDMA.\n" \
    "The command support the following sub-commands and options:\n" \
    "    Read [options] - Read register/memory.\n" \
    "        Memory=<name>  - Memory name.\n" \
    "        Pipe=<value>   - Pipe index.\n" \
    "        InDeX=<value>  - Start entry index.\n" \
    "        Count=<value>  - Number of entries.\n" \
    "    Write [options] - Write register/memory.\n" \
    "        Memory=<name>  - Memory name.\n" \
    "        Pipe=<value>   - Pipe index.\n" \
    "        InDeX=<value>  - Write entry index.\n" \
    "        DATA=<str>     - Data for write.\n" \
    "    Push [options] - Push data into a device FIFO.\n" \
    "        Memory=<name>  - Memory name.\n" \
    "        Pipe=<value>   - Pipe index.\n" \
    "        InDeX=<value>  - Write entry index.\n" \
    "        DATA=<str>     - Data for push."

/*! Examples for CLI command. */
#define BCMA_SBUSDMA_EXAMPLES \
    "read m=EPC_LINK_BMAPm p=0 c=1\n" \
    "write m=EPC_LINK_BMAPm p=0 data=0x12345678_12345678_12345678\n" \
    "push m=MOS_OSH0_TX_FIFOm data=0x12121212_34343434"

/*!
 * \brief SBUSDMA command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_sbusdma(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_SBUSDMA_H */
