/*! \file bcma_testcmd_testlist.c
 *
 * Broadcom TestList CMD based on Test Database
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/cli/bcma_cli.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testcmd_testdb.h>
#include <bcma/test/bcma_testcmd_testlist.h>

int
bcma_testcmd_testlist(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_test_db_t *tdb = bcma_test_db_get();
    const char *subcmd = "List";

    return bcma_testcmd_testdb(cli, subcmd, args, tdb);
}
