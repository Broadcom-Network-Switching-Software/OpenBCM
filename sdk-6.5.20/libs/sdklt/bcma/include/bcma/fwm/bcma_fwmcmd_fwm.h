/*! \file bcma_fwmcmd_fwm.h
 *
 * CLI command for Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_FWMCMD_FWM_H
#define BCMA_FWMCMD_FWM_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_FWMCMD_FWM_DESC  "Firmware Management command."

/*! Syntax for CLI command. */
#define BCMA_FWMCMD_FWM_SYNOP \
    "query <fpkg_file>\n" \
    "load [Processor=<procname>[.<inst>]] <fpkg_file> [<op1> [<op2> ...]]\n" \
    "exec <fwname>[.<inst>] [<op1> [<op2> ...]]\n" \
    "status\n" \
    "start <procname>[.<inst>]\n" \
    "stop <procname>[.<inst>]\n" \
    "reset <procname>[.<inst>]\n" \
    "read [Format=x1|x2|x4] <procname>[.<inst>] <addr>\n" \
    "write [Format=x1|x2|x4] <procname>[.<inst>] <addr> <value>\n" \
    "dump [Format=x1|x2|x4] <procname>[.<inst>] <addr> <count>"

/*! Help for CLI command. */
#define BCMA_FWMCMD_FWM_HELP \
    "query  - query information contained in the specified firmware package\n" \
    "         file. For example, firmware name, version, supported chips,\n" \
    "         image format, supported operations for this firmware, etc.\n" \
    "\n" \
    "load   - load firmware contained in the specified firmware package to\n" \
    "         the processor and optionally perform operations after loaded.\n" \
    "         If a processor instance is specified by Processor=, it will\n" \
    "         be loaded to that processor; otherwise, default processor\n" \
    "         specified in the firmware package file will be used instead.\n" \
    "         The processor should be specified in the format of\n" \
    "         <processor>[.<instance>]. For convenience, processor instance\n" \
    "         is not case sensitive and '.0' can be omitted for the first\n" \
    "         instance. For example, M0 and m0.0 refer to the same thing.\n" \
    "         Optional operations can be specified following the package\n" \
    "         file. The operations will be performed right after firmware\n" \
    "         is loaded. This is to avoid the delay with separate commands.\n" \
    "         See help of sub command 'exec' for more details on\n" \
    "         operations.\n" \
    "\n" \
    "exec   - perform specified operation(s) on a loaded firmware instance.\n" \
    "         Operations are provided by firmware feature for controlling\n" \
    "         the behavior, such as start it or change a parameter.\n" \
    "         An operation can be supplied with an optional integer\n" \
    "         argument (depending on the design of the operation) in the\n" \
    "         format of <operation>[=<value>]. Default value is 0 if not\n" \
    "         specified. For example, fwm exec LED.0 mode=1. \n" \
    "         Multiple operations can be specified in the same command and\n" \
    "         they will be executed in the order they're specified.\n" \
    "         Operations are performed on the specified firmware instance\n" \
    "         in the format of <firmware>[.<instance>]. For convenience,\n" \
    "         the firmware instance is not case sensitive and '.0' can be\n" \
    "         omitted for the first instance. For example, LED or led.0.\n" \
    "         Information on loaded firmware instances can be retrieved by\n" \
    "         sub command 'status'. If no operation is specifed, available\n" \
    "         operations provided by the specified firmware will be listed.\n" \
    "\n" \
    "status - show status of all processors and loaded firmware instances.\n" \
    "\n" \
    "start  - start the specified processor.\n" \
    "         See 'load' for the format to specify the processor.\n" \
    "         To get a list of available processor instances, use 'status'.\n" \
    "         Note that these commands that work with processors directly\n" \
    "         are provided for debugging purpose. To start/stop firmware or\n" \
    "         control firmware behavior, use operations provided by the\n" \
    "         firmware with 'exec' or 'load' command.\n" \
    "\n" \
    "stop   - stop the specified processor.\n" \
    "         See 'load' for the format to specify the processor.\n" \
    "\n" \
    "reset  - reset the specified processor.\n" \
    "         See 'load' for the format to specify the processor.\n" \
    "\n" \
    "read   - read a value from memory of the specified processor.\n" \
    "         <addr> is the address to read in processor view.\n" \
    "         Format=x1: in bytes (8-bit). \n" \
    "         Format=x2: in halfwords (16-bit). \n" \
    "         Format=x4: in words (32-bit). It's the default.\n" \
    "         See 'load' for the format to specify the processor.\n" \
    "\n" \
    "write  - write a value to memory of the specified processor.\n" \
    "         See 'read' for the format of <addr> and data width.\n" \
    "         See 'load' for the format to specify the processor.\n" \
    "\n" \
    "dump   - dump values from memory of the specified processor.\n" \
    "         See 'read' for the format of <addr> and data width.\n" \
    "         See 'load' for the format to specify the processor."

/*! Examples for CLI command. */
#define BCMA_FWMCMD_FWM_EXAMPLES \
    "query firmware.fpkg\n" \
    "load firmware.fpkg start startmsg\n" \
    "load p=R5.0 firmware.fpkg start\n" \
    "exec led\n" \
    "exec LED start\n" \
    "exec led.1 disable mode=2 enable\n" \
    "status\n" \
    "start M0.0\n" \
    "stop m0.1\n" \
    "reset M0\n" \
    "read M0.0 0x1000\n" \
    "write f=x1 m0 0x1000 0xaa\n" \
    "dump Format=x2 M0 0x1000 0x100"

/*!
 * \brief CLI 'fwm' command for Firmware Management.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_fwmcmd_fwm(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_FWMCMD_FWM_H */
