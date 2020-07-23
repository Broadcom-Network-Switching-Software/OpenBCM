/*! \file bcma_testcmd_add_cmds.c
 *
 * Add CLI commands for Test.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/test/bcma_testcmd.h>
#include <bcma/test/bcma_testcmd_test.h>
#include <bcma/test/bcma_testcmd_testrun.h>
#include <bcma/test/bcma_testcmd_testlist.h>

static bcma_cli_command_t cmd_test = {
    .name = "test",
    .func = bcma_testcmd_test,
    .desc = BCMA_TESTCMD_TEST_DESC,
    .synop = BCMA_TESTCMD_TEST_SYNOP,
    .help = { BCMA_TESTCMD_TEST_HELP },
    .examples = BCMA_TESTCMD_TEST_EXAMPLES
};

static bcma_cli_command_t cmd_testrun = {
    .name = "TestRun",
    .func = bcma_testcmd_testrun,
    .desc = BCMA_TESTCMD_TESTRUN_DESC,
    .synop = BCMA_TESTCMD_TESTRUN_SYNOP,
    .examples = BCMA_TESTCMD_TESTRUN_EXAMPLES
};

static bcma_cli_command_t cmd_testlist = {
    .name = "TestList",
    .func = bcma_testcmd_testlist,
    .desc = BCMA_TESTCMD_TESTLIST_DESC,
    .synop = BCMA_TESTCMD_TESTLIST_SYNOP,
    .examples = BCMA_TESTCMD_TESTLIST_EXAMPLES
};

void
bcma_testcmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &cmd_test, 0);
    bcma_cli_add_command(cli, &cmd_testrun, 0);
    bcma_cli_add_command(cli, &cmd_testlist, 0);
}
