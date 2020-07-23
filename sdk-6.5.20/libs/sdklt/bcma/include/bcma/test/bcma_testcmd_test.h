/*! \file bcma_testcmd_test.h
 *
 * CLI 'test' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCMD_TEST_H
#define BCMA_TESTCMD_TEST_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_TESTCMD_TEST_DESC \
    "Test operation."

/*! Syntax for CLI command. */
#define BCMA_TESTCMD_TEST_SYNOP \
    "<subcmd> [*|<group>[.<name>]|<id>[.<inst>]] [option|arguments]"

/*! Help for CLI command. */
#define BCMA_TESTCMD_TEST_HELP \
    "Subcmd:\n" \
    "  Init - initialize current test database\n" \
    "  Mode [options]\n" \
    "    Stoponerror=[yes|no]  - requests a diagnostic shell be invoked\n" \
    "                            when a test fails. This allows the user to\n" \
    "                            inspect SOC state using the diagshell\n" \
    "    Abortonerror=[yes|no] - requests a test be aborted on failure, and \n" \
    "                            execution continues with next test.\n" \
    "    Quiet=[yes|no]        - Be quiet when running a test\n" \
    "    Progress=[yes|no]     - requests a simple progress report be\n" \
    "                            displayed as tests are running\n" \
    "    RANDom=[yes|no]       - requests tests be run in pseudo random order\n" \
    "    SIlent=[yes|no]       - requests test error messages not be printed,\n" \
    "                            but errors are counted\n" \
    "    Run=[yes|no]          - requests a message is printed when a test\n" \
    "                            is dispatched\n" \
    "    Override=[yes|no]     - allows tests not supported on the specified\n" \
    "                            chip to be selected and run\n" \
    "    NoReinit=[yes|no]     - Do not reinit the system\n" \
    "    Debug=[yes|no]        - Prints debugging information\n" \
    "  Select - select specified tests by given id and inst.\n" \
    "  Deselect - deselect specified tests by given id and inst.\n" \
    "  List - list test status\n" \
    "  Clear - clear test counters\n" \
    "  ReMove - remove instances from given test database by id and inst.\n" \
    "  CLone - clone instances from given test database by test id.\n" \
    "  Parameters - Override the default parameters for the test instance\n" \
    "  Run - run test instances\n" \
    "  Help - show test help information"

/*! Examples for CLI command */
#define BCMA_TESTCMD_TEST_EXAMPLES \
    "init\n" \
    "mode\n" \
    "mode Stoponerror=yes\n" \
    "select hmi.sbusdma\n" \
    "deselect hmi.sbusdma\n" \
    "list *\n" \
    "list hmi\n" \
    "clear *\n" \
    "clear 11.0\n" \
    "remove hmi\n" \
    "clone hmi.sbusdma\n" \
    "clone 11 \"M=L3_ENTRY_ONLYm Write=yes IntMode=true\"\n" \
    "parameters 11\n" \
    "parameters 11 \"M=L3_ENTRY_ONLYm Write=yes\"\n" \
    "run\n" \
    "run 11 M=L3_ENTRY_ONLYm Write=yes IntMode=true\n" \
    "help hmi.sbusdma\n" \
    "help 11 M=L3_ENTRY_ONLYm Write=yes IntMode=true"

/*!
 * \brief CLI command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_testcmd_test(bcma_cli_t *cli, bcma_cli_args_t *args);
#endif /* BCMA_TESTCMD_TEST_H */
