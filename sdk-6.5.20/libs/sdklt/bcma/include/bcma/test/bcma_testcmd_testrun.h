/*! \file bcma_testcmd_testrun.h
 *
 * CLI 'TestRun' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCMD_TESTRUN_H
#define BCMA_TESTCMD_TESTRUN_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_TESTCMD_TESTRUN_DESC \
    "Run a specific or selected tests."

/*! Syntax for CLI command. */
#define BCMA_TESTCMD_TESTRUN_SYNOP \
    "[*|<group>[.<name>]|<id>[.<inst>]] [arguments]"

/*! Examples for CLI command */
#define BCMA_TESTCMD_TESTRUN_EXAMPLES \
    "\n" \
    "*\n" \
    "hmi\n" \
    "hmi.sbusdma\n" \
    "hmi.sbusdma M=L3_ENTRY_ONLYm\n" \
    "11\n" \
    "11.0\n" \
    "11 M=L3_ENTRY_ONLYm"

/*!
 * \brief CLI 'TestRun' command implementation.
 *
 * A wrapper command for 'test run'
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_testcmd_testrun(bcma_cli_t *cli, bcma_cli_args_t *args);
#endif /* BCMA_TESTCMD_TESTRUN_H */
