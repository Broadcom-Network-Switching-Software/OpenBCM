/*! \file bcma_testcmd_testlist.h
 *
 * CLI 'TestList' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCMD_TESTLIST_H
#define BCMA_TESTCMD_TESTLIST_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_TESTCMD_TESTLIST_DESC \
    "List or Display loaded tests and status."

/*! Syntax for CLI command. */
#define BCMA_TESTCMD_TESTLIST_SYNOP \
    "[*|<group>[.<name>]|<id>[.<inst>]]"

/*! Examples for CLI command */
#define BCMA_TESTCMD_TESTLIST_EXAMPLES \
    "\n" \
    "*\n" \
    "hmi\n" \
    "hmi.sbusdma\n" \
    "11\n" \
    "11.0"

/*!
 * \brief CLI 'TestList' command implementation.
 *
 * A wrapper command for 'test list'
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_testcmd_testlist(bcma_cli_t *cli, bcma_cli_args_t *args);
#endif /* BCMA_TESTCMD_TESTLIST_H */
