/*! \file bcma_mcscmd_mcs.h
 *
 * CLI 'mcs' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_MCSCMD_MCS_H
#define BCMA_MCSCMD_MCS_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_MCSCMD_MCS_DESC \
    "Microcontroller subsystem management."

/*! Syntax for CLI command. */
#define BCMA_MCSCMD_MCS_SYNOP \
    "load <filename> [UC=<MCSnum>] [AutoConFiG=yes|no]\\\n" \
    "[StartUC=yes|no] [StartMSG=yes|no] [<resources>]\n" \
    "status [<MCSnum>] [ShowConFiG=yes/no] [ShowDeBuG=yes/no]\n" \
    "console <MCSnum> [start|stop]\n" \
    "message <init|start|stop> [<MCSnum>]\n" \
    "rm <MCSnum> <release|show> [<used|free|all>]"

/*! Help for CLI command. */
#define BCMA_MCSCMD_MCS_HELP \
    "The load command is used to download and execute firmware for a\n" \
    "microcontroller subsystem on a switch device.\n" \
    "Options supported by the load command:\n" \
    "    <filename>          - The firmware file name to download. 'NONE'\n" \
    "                          to skip firmware download.\n" \
    "    UC=<MCSnum>         - Microprocessor subsystem ID (0, 1, etc.)" \
    "                          Select uC automatically if undefined\n" \
    "    AutoConFiG=<YES/no> - Set config-options automatically for standard" \
    "                          eApps. Ignores the <resources> parameters\n" \
    "    StartUC=<YES/no>    - Start the microcontroller after downloading\n" \
    "                          the firmware.\n" \
    "    StartMSG=<YES/no>   - Start the uKernel Messaging service after\n" \
    "                          starting the micrcontroller.\n" \
    "    <resources>         - resources required by this firmware\n" \
    "                          load will be aborted if insufficient resources\n" \
    "        SRamSiZe=<int>      - Size (in bytes) of the SRAM area to be\n" \
    "                              used by the firmware. This should conform\n" \
    "                              to the firmware application requirements.\n" \
    "        SRamUncSiZe=<int>   - Size (in bytes) of uncached SRAM to be\n" \
    "                              used by the firmware. This value should\n" \
    "                              be less than SRamSiZe.\n" \
    "        HostRamSiZe=<int>   - Size (in bytes) of the HostRAM area to be\n" \
    "                              used by the firmware.\n" \
    "        NumSbusDMA=<int>    - Number of SBUSDMA Channels required\n" \
    "                              by the firmware.\n" \
    "        NumPktDMA=<int>     - Number of packet DMA Channels required\n" \
    "                              by the firmware.\n" \
    "        NumSchanFIFO=<int>  - Number of Schan FIFO Channels required\n" \
    "                              by the firmware.\n" \
    "\n" \
    "The status command displays the firmware version as well as various\n" \
    "debug information\n" \
    "Options supported by the status command:\n" \
    "    <MCSnum>            - Microprocessor subsystem ID (0, 1, etc.)\n" \
    "                          If not specified, the status for all\n" \
    "                          microcontroller subsystems will be shown.\n" \
    "    ShowConFiG=<yes/NO> - Show the config-options of the firmware.\n" \
    "    ShowDeBuG=<yes/NO>  - Show extra debug info from the firmware.\n" \
    "\n" \
    "The console command provides access to the MCS UART console.\n" \
    "Options supported by the console command:\n" \
    "    <MCSnum>            - Microprocessor subsystem ID (0, 1, etc.)\n" \
    "    start               - Start showing MCS UART console output.\n" \
    "    stop                - Stop showing MCS UART console output.\n" \
    "    cmd <command>       - Send a command to the uKernel CLI.\n" \
    "                          Enclose command in quotes if multiple words.\n" \
    "If neither start or stop or cmd is specified, the command will open an\n" \
    "mcs shell to the uKernel CLI. Type 'quit' or 'exit' to leave this shell.\n" \
    "\n" \
    "The message command provides access to the MCS messaging control APIs.\n" \
    "Options supported by the message command:\n" \
    "    init                - init the messaging infrastructure\n" \
    "    start               - Start messaging on <MCSnum>.\n" \
    "    stop                - Stop messaging on <MCSnum>.\n" \
    "    <MCSnum>            - Microprocessor subsystem ID (0, 1, etc.)\n" \
    "<MCSnum> is must for start / stop operations. Unused for init\n" \
    "\n" \
    "The rm command provides access to different resource manager (RM) \n" \
    "functionalities and information\n" \
    "Options supported by the rm command:\n" \
    "    <MCSnum>            - Microprocessor subsystem ID (0, 1, etc.)\n" \
    "    release             - Releases RM resources reserved by <MCSnum>.\n" \
    "    show                - Shows resources handled by the RM.\n" \
    "    used                - Shows resources reserved by <MCSnum>.\n" \
    "    free                - Shows free resources available for <MCSnum>.\n" \
    "    all                 - Shows total resources accessible by <MCSnum>.\n" \
    "The used, free and all options are valid only for \"show\"subcommand.\n" \
    "The free and all options show resources available for each MCS core,\n" \
    "including the common resources, i.e reservation by one MCS core will\n" \
    "take it away from the other MCS cores that has access to it.\n"

/*! Examples for CLI command. */
#define BCMA_MCSCMD_MCS_EXAMPLES \
    "load BCMxxx_bfd.srec srsz=0x4000\n" \
    "status\n" \
    "console 0\n" \
    "message start 0\n" \
    "rm 0 show used"

/*!
 * \brief CLI MCS.
 *
 * This function can handle various operations of managing the
 * Micro Controller Subsystem.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully
 * \retval BCMA_CLI_CMD_USAGE Invalid command syntax
 * \retval BCMA_CLI_CMD_FAIL Command failed
 */
extern int
bcma_mcscmd_mcs(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_MCSCMD_MCS_H */
