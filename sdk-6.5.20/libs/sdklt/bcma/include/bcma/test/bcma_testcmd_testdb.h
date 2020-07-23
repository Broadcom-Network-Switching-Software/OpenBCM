/*! \file bcma_testcmd_testdb.h
 *
 * Test CLI command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCMD_TESTDB_H
#define BCMA_TESTCMD_TESTDB_H

#include <bcma/cli/bcma_cli.h>
#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Test command per database.
 *
 * \param [in] cli CLI object
 * \param [in] subcmd sub-command
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return Always 0.
 */
extern int
bcma_testcmd_testdb(bcma_cli_t *cli, const char *subcmd, bcma_cli_args_t *a,
                    bcma_test_db_t *tdb);

#endif /* BCMA_TESTCMD_TESTDB_H */
