/*! \file bcma_bcmpktcmd_pkttrace.h
 *
 * CLI command related to packet trace.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_PKTTRACE_H
#define BCMA_BCMPKTCMD_PKTTRACE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_PKTTRACE_DESC \
    "Inject a trace packet and get trace data."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_PKTTRACE_SYNOP \
    "[options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_PKTTRACE_HELP \
    "This command is used to inject a trace packet with a masquerade port\n" \
    "into the ingress pipeline. The packet will be treated as if coming\n" \
    "from the masquerade port. The packet's forwarding information in the\n" \
    "ingress pipeline will be captured and displayed.\n" \
    "\n" \
    "If the PArseOnly option is specified, no packet will be injected, and\n" \
    "the Data parameter will instead be parsed as raw trace output.\n" \
    "\n" \
    "Options: \n" \
    "    LeaRN=yes|no       - Enable learning on trace packet.\n" \
    "    NoIFP=yes|no       - Disable ingress FP lookup on trace packet.\n" \
    "    DropTm=yes|no      - Drop trace packet in TM.\n" \
    "    PArseOnly=yes|no   - Skip packet injection and simply parse raw\n" \
    "                         data.\n" \
    "    NetifID=<value>    - Network interface to use for packet injection.\n" \
    "    Port=<portid>      - Masquerade port for trace packet.\n" \
    "    Len=<value>        - Length of the packet, including header,\n" \
    "                         possible tag, and CRC.\n" \
    "    Data=<packet>      - Hex string to be used as raw packet data.\n" \
    "                         Will be parsed as trace output if PArseOnly=yes."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_PKTTRACE_EXAMPLES \
    "netifid=1 port=2 len=65 data=000000BBBBBB000000000002"

/*!
 * \brief packet trace command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_pkttrace(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_PKTTRACE_H */
