/*! \file bcma_testcmd_testdb.c
 *
 * Broadcom Test subcommand implementation based on Test Database
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>

#include <sal/sal_libc.h>

#include <bcmbd/bcmbd.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/bcma_testcmd_testdb.h>
#include <bcma/test/util/bcma_testutil_drv.h>

/*!
 * \brief Initialize a TDB.
 *
 * Initialize a specific TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 */
static int
testcmd_testdb_init(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    int result;

    COMPILER_REFERENCE(cli);
    COMPILER_REFERENCE(a);

    result = bcma_testdb_init(tdb);

    return result < 0 ? BCMA_CLI_CMD_FAIL : BCMA_CLI_CMD_OK;
}

/*!
 * \brief Configure the global options for a TDB.
 *
 * Configure the global options for a specific TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static int
testcmd_testdb_mode(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    int stoponerror, abortonerror, quiet, progress, random, silent;
    int run, override, noreinit, debug;
    bcma_cli_parse_table_t pt;

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

#define TESTDB_O_GET(_tdb, _option, _value) \
    do { \
        _value = _tdb->options & _option ? true : false; \
    } while (0)

#define TESTDB_O_SET(_tdb, _option, _value) \
    do { \
        if (_value) { \
            _tdb->options |=  _option; \
        } else { \
            _tdb->options &= ~_option; \
        } \
    } while (0)

    TESTDB_O_GET(tdb, BCMA_TEST_O_SOE, stoponerror);
    TESTDB_O_GET(tdb, BCMA_TEST_O_AOE, abortonerror);
    TESTDB_O_GET(tdb, BCMA_TEST_O_QUIET, quiet);
    TESTDB_O_GET(tdb, BCMA_TEST_O_PROGRESS, progress);
    TESTDB_O_GET(tdb, BCMA_TEST_O_RANDOM, random);
    TESTDB_O_GET(tdb, BCMA_TEST_O_SILENT, silent);
    TESTDB_O_GET(tdb, BCMA_TEST_O_RUN, run);
    TESTDB_O_GET(tdb, BCMA_TEST_O_OVERRIDE, override);
    TESTDB_O_GET(tdb, BCMA_TEST_O_NO_REINIT, noreinit);
    TESTDB_O_GET(tdb, BCMA_TEST_O_DEBUG, debug);

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Stoponerror", "bool",
                             &stoponerror, NULL);
    bcma_cli_parse_table_add(&pt, "Abortonerror", "bool",
                             &abortonerror, NULL);
    bcma_cli_parse_table_add(&pt, "Quiet", "bool",
                             &quiet, NULL);
    bcma_cli_parse_table_add(&pt, "Progress", "bool",
                             &progress, NULL);
    bcma_cli_parse_table_add(&pt, "RANDom", "bool",
                             &random, NULL);
    bcma_cli_parse_table_add(&pt, "SIlent", "bool",
                             &silent, NULL);
    bcma_cli_parse_table_add(&pt, "Run", "bool",
                             &run, NULL);
    bcma_cli_parse_table_add(&pt, "Override", "bool",
                             &override, NULL);
    bcma_cli_parse_table_add(&pt, "NoReinit", "bool",
                             &noreinit, NULL);
    bcma_cli_parse_table_add(&pt, "Debug", "bool",
                             &debug, NULL);

    if (BCMA_CLI_ARG_CNT(a) == 0) {
        bcma_cli_parse_table_show(&pt, NULL);
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_OK;
    }

    if (bcma_cli_parse_table_do_args(&pt, a) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    if (BCMA_CLI_ARG_CNT(a) > 0) {
        cli_out("%s: Unknown argument %s\n",
                BCMA_CLI_ARG_CMD(a), BCMA_CLI_ARG_CUR(a));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);

    TESTDB_O_SET(tdb, BCMA_TEST_O_SOE, stoponerror);
    TESTDB_O_SET(tdb, BCMA_TEST_O_AOE, abortonerror);
    TESTDB_O_SET(tdb, BCMA_TEST_O_QUIET, quiet);
    TESTDB_O_SET(tdb, BCMA_TEST_O_PROGRESS, progress);
    TESTDB_O_SET(tdb, BCMA_TEST_O_RANDOM, random);
    TESTDB_O_SET(tdb, BCMA_TEST_O_SILENT, silent);
    TESTDB_O_SET(tdb, BCMA_TEST_O_RUN, run);
    TESTDB_O_SET(tdb, BCMA_TEST_O_OVERRIDE, override);
    TESTDB_O_SET(tdb, BCMA_TEST_O_NO_REINIT, noreinit);
    TESTDB_O_SET(tdb, BCMA_TEST_O_DEBUG, debug);

#undef TESTDB_O_GET
#undef TESTDB_O_SET

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Select a specific test instance in a TDB.
 *
 * Select a specific test instance in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static int
testcmd_testdb_select(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_select(tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Deselect a specific test instance in a TDB.
 *
 * Deselect a specific test instance in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static int
testcmd_testdb_deselect(bcma_cli_t *cli,
                        bcma_cli_args_t *a,
                        bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_deselect(tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief List or display a list of test instances in a TDB.
 *
 * List or display a list of test instances in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 */
static bcma_cmd_result_t
testcmd_testdb_list(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;
    uint32_t flags = 0;
    char *args;

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(a) == 0) {
        if (bcma_testdb_list(tdb, NULL, flags) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    } else {
        args = BCMA_CLI_ARG_CUR(a);
        if (sal_strcasecmp(args, "all") == 0) {
            flags |= BCMA_TEST_LIST_ALL;
            BCMA_CLI_ARG_NEXT(a);
        } else if (sal_strcasecmp(args, "fail") == 0) {
            flags |= BCMA_TEST_LIST_FAIL;
            BCMA_CLI_ARG_NEXT(a);
        } else {
            flags |= BCMA_TEST_LIST_SUPPORT;
        }

        if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }

        if (bcma_testdb_list(tdb, &tokey, flags) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Clear statistics for a list of test instances in a TDB.
 *
 * Clear statistics for a list of test instances in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static bcma_cmd_result_t
testcmd_testdb_clear(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_clear(tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Remove a list of test instances in a TDB.
 *
 * Remove a list of test instances in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static int
testcmd_testdb_remove(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_remove(tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Clone a test instance for a specific test case in a TDB.
 *
 * Clone a test instance for a specific test case in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static int
testcmd_testdb_clone(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;
    char *arg;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(a);
    if (bcma_testdb_clone(tdb, &tokey, arg) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Set override arguments for a specific test instance in a TDB.
 *
 * Set override arguments for a specific test instance in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static int
testcmd_testdb_parameters(bcma_cli_t *cli,
                          bcma_cli_args_t *a,
                          bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;
    char *arg;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(a);
    if (bcma_testdb_parameters(tdb, &tokey, arg) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Run or execute a list of test instances in a TDB.
 *
 * Run or execute a list of test instances in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 */
static int
testcmd_testdb_run(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_run(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/*!
 * \brief Help a specific test instance in a TDB.
 *
 * Help a specific test instance in a TDB.
 *
 * \param [in] cli CLI object
 * \param [in] a Argument list
 * \param [in] tdb Test database
 *
 * \return BCMA_CLI_CMD_OK Success.
 * \return BCMA_CLI_CMD_FAIL Failure.
 * \return BCMA_CLI_CMD_USAGE Invalid argument.
 */
static bcma_cmd_result_t
testcmd_testdb_help(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb)
{
    bcma_test_object_key_t tokey;

    if (0 == BCMA_CLI_ARG_CNT(a)) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_testdb_is_enabled(tdb) == false) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_parser(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (bcma_testdb_help(cli, a, tdb, &tokey) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

int
bcma_testcmd_testdb(bcma_cli_t *cli, const char *subcmd, bcma_cli_args_t *a,
                    bcma_test_db_t *tdb)
{
    int unit;

    if (tdb == NULL) {
        return BCMA_CLI_CMD_INTR;
    }

    /* Attach base driver if needed */
    unit = cli->cmd_unit;
    if (!bcmbd_attached(unit)) {
        if (SHR_FAILURE(bcma_testdb_init_bd(unit))) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    /* Attach test driver */
    bcma_testutil_drv_attach(unit);

    if (bcma_cli_parse_cmp("Init", subcmd, ' ')) {
        return testcmd_testdb_init(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Mode", subcmd, ' ')) {
        return testcmd_testdb_mode(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Select", subcmd, ' ')) {
        return testcmd_testdb_select(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Deselect", subcmd, ' ')) {
        return testcmd_testdb_deselect(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("List", subcmd, ' ')) {
        return testcmd_testdb_list(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Clear", subcmd, ' ')) {
        return testcmd_testdb_clear(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("ReMove", subcmd, ' ')) {
        return testcmd_testdb_remove(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("CLone", subcmd, ' ')) {
        return testcmd_testdb_clone(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Parameters", subcmd, ' ')) {
        return testcmd_testdb_parameters(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Run", subcmd, ' ')) {
        return testcmd_testdb_run(cli, a, tdb);
    } else if (bcma_cli_parse_cmp("Help", subcmd, ' ')) {
        return testcmd_testdb_help(cli, a, tdb);
    }

    return BCMA_CLI_CMD_USAGE;
}
