/*! \file bcma_bcmpktcmd_pktdev.h
 *
 * CLI command related to packet device configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_PKTDEV_H
#define BCMA_BCMPKTCMD_PKTDEV_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_PKTDEV_DESC \
    "Manage packet device."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_PKTDEV_SYNOP \
    "[<resource>] <action> [<id>] [options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_PKTDEV_HELP \
    "This command performs operations on a packet device or a packet device\n" \
    "resource.\n" \
    "    ATtach [options] - Attach a device driver.\n" \
    "        Type=<value>        - Device driver type.\n" \
    "    DeTach [options] - Detach device driver.\n" \
    "        CleanUp=[yes/no]    - Cleanup the packet device. Default is yes.\n" \
    "    Init [options] - Init a packet device.\n" \
    "        Name=<value>        - Device name string.\n" \
    "        DefPath=[yes/no]    - Create default path to enable TX/RX.\n" \
    "        MACaddr=<value>     - Network interface MAC address.\n" \
    "        CHGrp=<value>       - Channel group valid bitmap.\n" \
    "        FrameSize=<value>   - Defaut path's Max frame size.\n" \
    "    Up           - Bringup device and default network interface.\n" \
    "    Down         - Pull down device and default network interface.\n" \
    "    CleanUp      - Cleanup the packet device.\n" \
    "    InFo         - Show the packet device status.\n" \
    "    Stats \n" \
    "        Show     - Show the packet device's statistics.\n" \
    "        Clear    - Clear the packet device's statistics.\n" \
    "    Chan \n" \
    "        Set <id> [options] - Configure a DMA channel.\n" \
    "            Dir=<value>      - Direction, TX or RX.\n" \
    "            RingSize=<value> - DMA Channel ring size.\n" \
    "            FrameSize=<value>- Maximum frame size allowed.\n" \
    "        QueueMap <id> [options] - Map queues to a RX channel.\n" \
    "            HighWord=<value> - High 32 bits queue bitmap.\n" \
    "            LowWord=<value>  - Low 32 bits queue bitmap.\n" \
    "        Info [<id>] - Show a DMA channel's configuration.\n" \
    "    BufPool \n" \
    "        Create [options]  - Create a buffer pool.\n" \
    "            Shared=[yes/no] - For share or for this device.\n" \
    "            BufSize=<value> - Each buffer size.\n" \
    "            BufCount=<value>- Number of buffers.\n" \
    "        Destroy [option] - Destroy a buffer pool.\n" \
    "            Shared=[yes/no] - shared pool or this unit's pool.\n" \
    "        CleanUp    - Cleanup all buffer pools.\n" \
    "        Info [All] - Dump buffer pool information.\n" \
    "    PacketPool \n" \
    "        Create [options]  - Create packet pool.\n" \
    "            PacketCount=<value>- Number of packets.\n" \
    "        Destroy    - Destroy packet pool.\n" \
    "        Info       - Dump packet pool information.\n" \
    "    RateLimit \n" \
    "        Set        - Set RX rate limit.\n" \
    "            RxRate=<value>  - RX Rate limit value in KPPS.\n" \
    "                              If -1, disable RX rate limit.\n" \
    "        Info       - Show RX rate limit configuration."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_PKTDEV_EXAMPLES \
    "init\n" \
    "i n=\"bcm1\" defpath=yes\n" \
    "chan set 0 dir=TX ringsize=32\n" \
    "chan qm 1 hw=0xffff lw=0xfffffff"

/*!
 * \brief Packet device management command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_pktdev(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_PKTDEV_H */
