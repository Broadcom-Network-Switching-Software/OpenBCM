/*! \file bcma_bcmpccmd_phy.h
 *
 * CLI 'PHY' command for PHY based operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPCCMD_PHY_H
#define BCMA_BCMPCCMD_PHY_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPCCMD_PHY_DESC  "PHY based commands."

/*! Syntax for CLI command. */
#define BCMA_BCMPCCMD_PHY_SYNOP \
    "info [<port>|<port-range>]\n" \
    "<port> list [raw] <reg-name>\n" \
    "<port>|<port-range> [raw] <reg-expr> [<val>|<field>=<val> ...]\n" \
    "<port>|<port-range> <addr-expr> [<val>]\n" \
    "raw [pm]<pm-id> <addr-expr> [<val>]\n" \
    "diag ..."

/*! Help for CLI command. */
#define BCMA_BCMPCCMD_PHY_HELP \
    "<reg-expr> := <reg>[.<lane-index>][.<pll-index>]\n" \
    "<addr-expr> := <addr>[.<lane-index>][.<pll-index>]\n" \
    "\n" \
    "List or read/write internal PHY registers.\n" \
    "This command takes the logical port as the port number domain.\n" \
    "If <lane-index> is specified, it will override the default lane index\n" \
    "of the associate port. If value 1 is prefix before register address,\n" \
    "it will access the register of specified devad device. If <pll-index>\n" \
    "is specified, it will override the default register pll index.\n" \
    "Raw register command takes the port macro (PM) ID (starting with 1)\n" \
    "and register address as input to access registers of the PM in\n" \
    "an early stage before the underlying driver is attached.\n" \
    "\n" \
    "The syntax for \"phy diag\" is identical to the \"phydiag\" command.\n" \
    "Use \"help phydiag\" to see the detailed help text."

/*! Examples for CLI command. */
#define BCMA_BCMPCCMD_PHY_EXAMPLES \
    "info 1\n" \
    "info 1-10\n" \
    "1 list PHYID\n" \
    "1 PHYID2r\n" \
    "1 0x1d147.0.1\n" \
    "1 0x9000.0\n" \
    "1,2,3 0xc050\n" \
    "1-5 sc_x4_control_control\n" \
    "raw pm1 0x9000"

/*!
 * \brief CLI 'phy' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpccmd_phy(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clean up the resource used in the phy command.
 *
 * This function can be called before the CLI object is about to be destroyed
 * to prevent resource leak.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpccmd_phy_cleanup(bcma_cli_t *cli);

#endif /* BCMA_BCMPCCMD_PHY_H */
