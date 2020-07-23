/*! \file bcma_pciecmd_pciephy.h
 *
 * CLI command 'pciephy' description.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_PCIECMD_PCIEPHY_H
#define BCMA_PCIECMD_PCIEPHY_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_PCIECMD_PCIEPHY_DESC  "PCIe diagnostics."

/*! Syntax for CLI command. */
#define BCMA_PCIECMD_PCIEPHY_SYNOP \
    "getepreg <addr>\n" \
    "setepreg <addr> <value>\n" \
    "getreg <addr>\n" \
    "setreg <addr> <value>\n" \
    "pramread <addr> <size>\n" \
    "diag <lane_map> <diag_type>\n" \
    "fwload <filename>\n" \
    "fwinfo"

/*! Help for CLI command. */
#define BCMA_PCIECMD_PCIEPHY_HELP \
    "getepreg\n" \
    "   Display value of given PCIe core (endpoint) register.\n" \
    "setepreg\n" \
    "   Write given value into given PCIe core (endpoint) register.\n" \
    "getreg [count [lane]]\n" \
    "   Display values start from given PCIe PHY (SerDes) register address.\n" \
    "   count - number of registers to be display.\n" \
    "   lane - Lane index for per lane registers.\n" \
    "setreg [lane]\n" \
    "   Write given value into given PCIe PHY (SerDes) register.\n" \
    "   lane - Lane index for per lane register.\n" \
    "pramread\n" \
    "   Display PCIe PHY RAM content at given address.\n" \
    "   <size>: The number of 16-bit words to read.\n" \
    "diag\n" \
    "   Display SerDes lane information.\n" \
    "   <lane_map>: The lane bitmap to diagnose.\n" \
    "               Bit 0 starts from the lane 0 of the first SerDes core.\n" \
    "   <diag_type>:\n" \
    "       dsc - Display core state and lane state.\n" \
    "       state - Display core state, all lanes state and event log.\n" \
    "       eyescan - Display eyescan for selected lane.\n" \
    "fwload\n" \
    "   Program contents of the given firmware file to QSPI flash.\n" \
    "fwinfo\n" \
    "   Display firmware loading information.\n"


/*! Examples for CLI command. */
#define BCMA_PCIECMD_PCIEPHY_EXAMPLES \
    "getepreg 0xbc\n" \
    "setepreg 0xbc 0x12345678\n" \
    "getreg 0xd230\n" \
    "setreg 0xd230 0xa555\n" \
    "pramread 0x0 0x20\n" \
    "diag 0x3 dsc\n" \
    "diag 0x3 state\n" \
    "diag 0x3 eyescan\n" \
    "fwver\n" \
    "fwload pcieg3fw.bin"

/*!
 * \brief CLI 'pciephy' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_pciecmd_pciephy(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_PCIECMD_PCIEPHY_H */
