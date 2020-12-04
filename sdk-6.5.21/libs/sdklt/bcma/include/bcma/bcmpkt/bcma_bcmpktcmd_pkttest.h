/*! \file bcma_bcmpktcmd_pkttest.h
 *
 * CLI command related to BCMPKT PKTTEST.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_PKTTEST_H
#define BCMA_BCMPKTCMD_PKTTEST_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_PKTTEST_DESC \
    "Packet IO performance test."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_PKTTEST_SYNOP \
    "<subcmd> [options] "\

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_PKTTEST_HELP \
    "This command can be used to measure either Rx packet performance or Tx\n"\
    "packet performance. The options supported for each type of performance  \n"\
    "test is shown below.\n\n"\
    "    RX - Rx performance test for Packet IO.\n"\
    "        Time=<value>         - The test duration for each \n"\
    "                               packet length (default=2).\n"\
    "        LengthStart=<value>  - The start packet length (default=64).\n"\
    "        LengthEnd=<value>    - The end packet length (default=1536).\n"\
    "        LengthInc=<value>    - The increasing step of packet length.\n"\
    "                               (default=64)\n"\
    "    TX - Tx performance test for Packet IO.\n"\
    "        SendCount=<value>    - The number of packets to be sent for\n"\
    "                               each packet length (default=50000).\n"\
    "        LengthStart=<value>  - The start packet length (default=64).\n"\
    "        LengthEnd=<value>    - The end packet length (default=1536).\n"\
    "        LengthInc=<value>    - The increasing step of packet length \n"\
    "                               (default=64)."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_PKTTEST_EXAMPLES \
    "rx t=2 ls=128 le=512 li=128\n"\
    "tx sc=100000 ls=128 le=512 li=128"

/*!
 * \brief PKTTEST command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_pkttest(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_PKTTEST_H */
