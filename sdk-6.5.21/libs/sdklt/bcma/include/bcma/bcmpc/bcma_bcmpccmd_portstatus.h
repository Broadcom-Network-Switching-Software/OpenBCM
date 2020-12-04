/*! \file bcma_bcmpccmd_portstatus.h
 *
 * CLI 'portstatus' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPCCMD_PORTSTATUS_H
#define BCMA_BCMPCCMD_PORTSTATUS_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPCCMD_PORTSTATUS_DESC \
    "Display port status."

/*! Syntax for CLI command. */
#define BCMA_BCMPCCMD_PORTSTATUS_SYNOP \
    "[<port_range>]"

/*! Help for CLI command. */
#define BCMA_BCMPCCMD_PORTSTATUS_HELP \
    "<port_range>: logical port number range, 1-3, port 1 to port 3 \n" \
    "              logical port number list 1,2,3, port 1, 2 and 3" \

/*! Examples for CLI command. */
#define BCMA_BCMPCCMD_PORTSTATUS_EXAMPLES \
    "\n" \
    "1\n" \
    "3,5,8-10"

/*!
 * \brief CLI 'portstatus' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpccmd_portstatus(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPCCMD_PORTSTATUS_H */
