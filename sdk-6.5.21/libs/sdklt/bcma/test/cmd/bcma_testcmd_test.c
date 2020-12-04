/*! \file bcma_testcmd_test.c
 *
 * Broadcom Test CMD based on database
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testcmd_test.h>
#include <bcma/test/bcma_testcmd_testdb.h>

int
bcma_testcmd_test(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_test_db_t *tdb = bcma_test_db_get();
    const char *subcmd;

    subcmd = BCMA_CLI_ARG_GET(args);
    if (subcmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    return bcma_testcmd_testdb(cli, subcmd, args, tdb);
}
