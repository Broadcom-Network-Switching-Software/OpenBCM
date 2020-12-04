/*! \file bcma_bcmpktcmd_rx.h
 *
 * CLI command related to BCMPKT RX access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_RX_H
#define BCMA_BCMPKTCMD_RX_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_RX_DESC \
    "Packet RX debug operations."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_RX_SYNOP \
    "<subcmd> [<action>] [<netif_id>] [options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_RX_HELP \
    "This command is used to create and manage packet watchers. A packet\n" \
    "watcher monitors a specific network interface and dumps all packets\n" \
    "received on this interface in real time. RxPmdList is used for list\n" \
    "all supported fields of RX packet metadata. ReasonList is used for\n" \
    "all supported reasons names.\n" \
    "RX watcher recieved packets can be logged to file by Watcher's subcommand\n" \
    "'Output'. 'OutputFile' parameter specifies the file for storing data.\n" \
    "Logging starts only when LoGging=on issued and stops by command with\n" \
    "LoGging=off. Note that the file may only be read-able after LoGging=off.\n" \
    "The log file will use the same output format as the console.\n" \
    "    Watcher - Create RX watcher on a network interface.\n" \
    "        Create <netif_id> [options]  - Create a RX watcher on a network\n" \
    "                                       interface. If -1 or not input \n" \
    "                                       netif_id, the global RX watcher\n" \
    "                                       will be created.\n" \
    "            ShowPacketData=[yes/no]  - Dump packet raw data.\n" \
    "            ShowMetaData=[yes/no]    - Dump packet metadata.\n" \
    "            ShowRXReason=[yes/no]    - Dump packet RX reason.\n" \
    "            LoopbackData=[yes/no]    - Forward RX data to TX for test.\n" \
    "            ShowRxRate=[yes/no]      - Show each 100k packets' RX rate.\n" \
    "            TerminateNetif=<value>   - Terminate the packets from the\n" \
    "                                       network interface.\n" \
    "            TerminateVlan=<value>    - Terminate the packets from\n" \
    "                                       the VLAN.\n" \
    "            TerminateMatchID=<value> - Terminate the packets with \n" \
    "                                       specified RXPMD MATCH_ID.\n" \
    "        Destroy <netif_id> - Destroy a RX watcher. If -1 or not input\n" \
    "                                       netif_id, all RX watchers will be\n" \
    "                                       destroyed.\n" \
    "        Output [options] - Log RX watcher recieved packets to file. \n" \
    "            OutputFile=<file>        - File to store the packet data.\n" \
    "            LogEnable=[on|off]       - Start/Stop logging data to file.\n" \
    "            ConsoleEnable=[on|off]   - Display data on console. Default\n" \
    "                                       is 'on'.\n" \
    "    RxPmdList [options] - list supported RXPMD field names.\n" \
    "        DeviceType=<value> - Device Type (e.g. BCM56xxx_a0).\n" \
    "        ViewName=<value>   - View name.\n" \
    "    ReasonList  - list all RX_REASON names."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_RX_EXAMPLES \
    "watcher create 1\n" \
    "w c 1 spd=no smd=no srxr=no ld srr tn=1 tv=1 tmid=100\n" \
    "w o of=/tmp/pkt-watch-log le=on\n" \
    "w o le=off\n" \
    "w d 1"

/*!
 * \brief RX command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_rx(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_RX_H */
