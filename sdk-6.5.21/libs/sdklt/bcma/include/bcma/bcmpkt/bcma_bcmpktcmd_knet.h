/*! \file bcma_bcmpktcmd_knet.h
 *
 * CLI command related to KNET.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_KNET_H
#define BCMA_BCMPKTCMD_KNET_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_KNET_DESC \
    "KNET access."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_KNET_SYNOP \
    "<object> <action> [<id>] [options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_KNET_HELP \
    "This command is used to manage KNET objects such as virtual network\n" \
    "interfaces and packet filters.\n" \
    "    Netif - Network interface operations.\n" \
    "        Create [options] - Create a new network interface.\n" \
    "            Name=<value>        - Netif name string.\n" \
    "            RcpuEncap=[yes/no]  - Enable RCPU header encapsulation.\n" \
    "            BindrxCHannel=[yes/no] - Bind RX Channel.\n" \
    "            BindtxPort=[yes/no] - Bind TX port.\n" \
    "            Vlan=<value>        - VLAN ID.\n" \
    "            MACaddr=<value>     - MAC address.\n" \
    "            Port=<value>        - Port number for TX binding.\n" \
    "            PortEncap=<value>   - The binding port's encapsulation type\n" \
    "                                  (\"higig\" | \"higig2\" | \"ieee\").\n" \
    "            Queue=<value>       - Output queue ID of the binding port.\n" \
    "            CHan=<value>        - Channel ID for RX Channel binding.\n" \
    "            FrameSize=<value>   - Maximum frame size allowed.\n" \
    "        Destroy <id>   - Destroy a network interface.\n" \
    "        Info [<id>]    - Show a network interface's configuration.\n" \
    "    Filter - Filter operations.\n" \
    "        Create [options] - Create a filter.\n" \
    "            DESC=<value>        - Filter's description.\n" \
    "            Priority=<value>    - Filter's priority.\n" \
    "            DestType=<type>     - Destination type to forward.\n" \
    "            DestID=<value>      - Destination ID to forward.\n" \
    "            DestProto=<value>   - Overwrite protocol.\n" \
    "            MirrorType=<value>  - Mirroring destination type.\n" \
    "            MirrorID=<value>    - Mirroring destination ID.\n" \
    "            MirrorProto=<value> - Overwrite protocol for mirror copy.\n" \
    "            StripTag=[yes/no]   - Enable to trip VLAN Tag.\n" \
    "            Vlan=<value>        - 16 bit TCI to match.\n" \
    "            IngressPort=<value> - Ingress port ID to match.\n" \
    "            SrcModePort=<value> - Source module port to match.\n" \
    "            SrcModeID=<value>   - Source module ID to match.\n" \
    "            Queue=<value>       - Source CPU port queue to match.\n" \
    "            CHan=<value>        - Source DMA Channel ID. \n" \
    "            FPRule=<value>      - FP rule number to match.\n" \
    "            Reason=<reason>     - Specific reason to match.\n" \
    "            RawData=<data>      - Raw data string to match.\n" \
    "            RawMask=<mask>      - Raw data mask string to match.\n" \
    "            MetaData=<mdata>    - Raw metadata with 32-bit values in\n" \
    "                                  words. See input format in the example.\n" \
    "            MetaDataMask=<mdata_mask> - Raw metadata mask to match.\n" \
    "        Destroy <id>   - Destroy a filter.\n" \
    "        Info [<id>]    - Show a filter configuration."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_KNET_EXAMPLES \
    "netif create name=\"bcm0_1\" mac=00:11:22:33:44:55 framesize=1518\n" \
    "n c n=\"bcm0_2\" mac=00:01:02:03:04:05 fs=1518\n" \
    "filter create chan=1 priority=10 desttype=netif destid=1\\\n" \
    "              ingressport=1\n" \
    "f c dt=netif did=1 MetaData=0x00000000_00000000_00000000\\\n" \
    "    _00000000_00020000_00000000_00000000_00000000_00000000\\\n" \
    "    _00000000_00000000_00000000_00000000_00000000_00000000\\\n" \
    "    _00000000_00000000_00000000 MetaDataMask=0x00000000_00000000\\\n" \
    "    _00000000_00000000_ffff0000_00000000_00000000_00000000_00000000\\\n" \
    "    _00000000_00000000_00000000_00000000_00000000_00000000_00000000\\\n" \
    "    _00000000_00000000\n" \
    "f c ch=1 p=20 dt=netif did=1 q=2\n" \
    "f destroy 1"

/*!
 * \brief KNET management command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_knet(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_KNET_H */
