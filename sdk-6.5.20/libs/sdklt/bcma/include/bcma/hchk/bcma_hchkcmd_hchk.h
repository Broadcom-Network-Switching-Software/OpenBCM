/*! \file bcma_hchkcmd_hchk.h
 *
 * CLI 'hchk' command for debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_HCHKCMD_CMD_H
#define BCMA_HCHKCMD_CMD_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_HCHKCMD_CMD_DESC  "Check health of device."

/*! Syntax for CLI command. */
#define BCMA_HCHKCMD_CMD_SYNOP \
    "\n"

/*! Help for CLI command. */
#define BCMA_HCHKCMD_CMD_HELP \
    "HealthCHecK \n" \
    "    - check the current state of device i.e." \
    "      all traffic has cleared out cleanly from datapath \n" \
    "      no fatal interrupts set \n" \
    "      any credits/pointers used are returned properly.\n" \
    "      Could be run before starting any new traffic in the system. \n"

/*! Examples for CLI command. */
#define BCMA_HCHKCMD_CMD_EXAMPLES \
    "\n"

/*!
 * \brief CLI 'whos' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_hchkcmd_hchk(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_HCHKCMD_CMD_H */
