/*! \file bcma_testdb.h
 *
 * For Test Database
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTDB_H
#define BCMA_TESTDB_H

#include <bcmdrd_config.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/test/bcma_testcase.h>

/*! To indicate any group in test object key */
#define BCMA_TEST_GROUP_ANY          (NULL)
/*! To indicate any test case in test object key */
#define BCMA_TEST_ID_ANY             (-1)
/*! To indicate any test instance in test object key */
#define BCMA_TEST_INST_ANY           (-1)
/*! To indicate default test instance number */
#define BCMA_TEST_INST_DEFAULT       (0)
/*! To indicate max name string number in Test Component */
#define BCMA_TEST_CONFIG_NAME_BUFFER_MAX 80
/*! To indicate max string number for parsing test object from CLI inputs */
#define BCMA_TEST_CONFIG_OBJECT_PARTITION_MAX 2
/*! To indicate list supported test instances in CLI */
#define BCMA_TEST_LIST_SUPPORT       (0x1 << 0)
/*! To indicate list all test instances in CLI */
#define BCMA_TEST_LIST_ALL           (0x1 << 1)
/*! To indicate list failed test instances in CLI */
#define BCMA_TEST_LIST_FAIL          (0x1 << 2)

/*!
 * \brief Statistics for test instance.
 */
typedef struct bcma_test_stat_s {

    /*! Total number of runs */
    int runs;

    /*! Successful completions */
    int success;

    /*! Failures */
    int fail;

} bcma_test_stat_t;

/*!
 * \brief Override argument information for test instance.
 */
typedef struct bcma_test_override_args_s {

    /*! Override unit #, could be assigned from CLI
     * -1 means all unit
     */
    int unit;

    /*! Override Arguments */
    char *args;

} bcma_test_override_args_t;

/*!
 * \brief Test instance.
 */
typedef struct bcma_test_inst_s {

    /*! Test instance number */
    int inst;

    /*! Flags for test instance */
    uint32_t flags;
    /*! Force to select a test instance */
#define BCMA_TEST_INST_F_SELECT      (0x1 << 0)
    /*! Force to deselect a test instance */
#define BCMA_TEST_INST_F_UNSUPPORT   (0x1 << 1)

    /*! Override on unit # */
    bcma_test_override_args_t override;

    /*! Test Statistic counters */
    bcma_test_stat_t stats;

    /*! Pointer to next test case */
    struct bcma_test_inst_s *next;

} bcma_test_inst_t;

/*!
 * \brief List structure for management test case.
 */
typedef struct bcma_test_list_s {

    /*! Test case */
    bcma_test_case_t *tc;

    /*! Test instance list */
    bcma_test_inst_t *ilist;

    /*! Pointer to next test list */
    struct bcma_test_list_s *next;

} bcma_test_list_t;

/*!
 * \brief Test Group.
 */
typedef struct bcma_test_group_s {

    /*! Test group name */
    char name[BCMA_TEST_CONFIG_NAME_BUFFER_MAX];

    /*! Test List */
    bcma_test_list_t *tlist;

    /*! Pointer to next test group */
    struct bcma_test_group_s *next;

} bcma_test_group_t;

/*!
 * \brief Test device state.
 */
typedef enum bcma_test_dev_state_s {

    BCMA_TEST_DEV_STATE_INIT = 0,                /*! TDB is initialized. */
    BCMA_TEST_DEV_STATE_READY = 1,               /*! Test device is ready
                                                     for testing. */
    BCMA_TEST_DEV_STATE_INDETERMINATE = 2,       /*! Test case failed or test
                                                     device is in indetermine
                                                     state. */
    BCMA_TEST_DEV_STATE_MAX

} bcma_test_dev_state_t;

/*!
 * \brief Test database.
 */
typedef struct bcma_test_db_s {

    /*! Test options */
    uint32_t options;
    /*! Stop on Error */
#define BCMA_TEST_O_SOE              (0x1 << 0)
    /*! Abort on Error */
#define BCMA_TEST_O_AOE              (0x1 << 1)
    /*! Batch Mode */
#define BCMA_TEST_O_BATCH            (0x1 << 2)
    /*! Random Mode */
#define BCMA_TEST_O_RANDOM           (0x1 << 3)
    /*! Print test progress */
#define BCMA_TEST_O_PROGRESS         (0x1 << 4)
    /*! Don't print error msgs */
#define BCMA_TEST_O_SILENT           (0x1 << 5)
    /*! Print msg when test run */
#define BCMA_TEST_O_RUN              (0x1 << 6)
    /*! Override Chips */
#define BCMA_TEST_O_OVERRIDE         (0x1 << 7)
    /*! Don't print messages */
#define BCMA_TEST_O_QUIET            (0x1 << 8)
    /*! No reinit */
#define BCMA_TEST_O_NO_REINIT        (0x1 << 9)
    /*! Print debug information */
#define BCMA_TEST_O_DEBUG            (0x1 << 10)

    /*! Signature */
    unsigned int signature;
    /*! Signature for sanity checks. */
#define BCMA_TEST_SIGNATURE   SAL_SIG_DEF('T', 'E', 'S', 'T')

    /*! Test group */
    bcma_test_group_t *tg;

    /*! Test Device State */
    bcma_test_dev_state_t state[BCMDRD_CONFIG_MAX_UNITS];

} bcma_test_db_t;

/*!
 * \brief Test object lookup key.
 */
typedef struct bcma_test_object_key_s {

    /*! Device number */
    int unit;

    /*! Test group name */
    const char *gn;

    /*! Test identifier */
    int id;

    /*! Test instance number */
    int inst;

} bcma_test_object_key_t;

/*!
 * \brief Test object.
 */
typedef struct bcma_test_object_s {

    /*! Test group */
    bcma_test_group_t *tg;

    /*! Test case */
    bcma_test_case_t *tc;

    /*! Test instance */
    bcma_test_inst_t *ti;

} bcma_test_object_t;

/*!
 * \brief Test object handler.
 */
typedef struct bcma_test_object_hndlr_s {

    /*!
     * \brief Test object callback handler function.
     *
     * \param [in] tdb Test Database.
     * \param [in] to Test Object.
     * \param [in] user_data User Data.
     *
     * \retval SHR_E_NONE Execute test object handler successfully.
     * \retval SHR_E_PARAM Invalid parameters.
     * \retval SHR_E_MEMORY Memory Allocated fail.
     * \retval SHR_E_UNIT Invalid unit number.
     * \retval SHR_E_INTERNAL Ctrl-C break return.
     */
    int (*cb)(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data);

    /*! Test object user data */
    void *user_data;

} bcma_test_object_hndlr_t;

/*!
 * \brief Test engine data.
 */
typedef struct bcma_test_engine_data_s {

    /*! CLI object. */
    bcma_cli_t *cli;

    /*! Arguments from CLI. */
    bcma_cli_args_t *a;

    /*! Arguments parsed from override or default Args. */
    bcma_cli_args_t *av;

    /*! Test loops. */
    int loops;

    /*! Test engine flags. */
    uint32_t flags;
    /*! To indicate Test Engine is starting for providing help function. */
#define BCMA_TEST_ENGINE_F_HELP      (0x1 << 0)

    /*! Device number. */
    int unit;

    /*! The buffer pointer for test parameters . */
    void *bp;

    /*! Test database pointer. */
    bcma_test_db_t *tdb;

    /*! Test object. */
    bcma_test_object_t *to;

} bcma_test_engine_data_t;

/*!
 * \brief Initialize base driver and early config.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testdb_init_bd(int unit);

/*!
 * \brief Check if a test database is present for a system..
 *
 * \param [in] tdb Test Database.
 *
 * \return true if tdb is enabled, otherwise false.
 */
extern int
bcma_testdb_is_enabled(bcma_test_db_t *tdb);

/*!
 * \brief Initialize Test database.
 *
 * This function initializes test database.
 *
 * \param [in] tdb Test Database.
 *
 * \retval SHR_E_NONE Initialize test database successfully.
 * \retval SHR_E_MEMORY Memory Allocated fail.
 */
extern int
bcma_testdb_init(bcma_test_db_t *tdb);

/*!
 * \brief Cleanup Test database.
 *
 * This function cleans up the test database.
 *
 * \param [in] tdb Test Database.
 *
 * \retval SHR_E_NONE Cleanup test database successfully.
 */
extern int
bcma_testdb_cleanup(bcma_test_db_t *tdb);

/*!
 * \brief Add test case into test database.
 *
 * This function adds test case into test database.
 *
 * \param [in] tdb Test Database.
 * \param [in] tc Test Case.
 * \param [in] feature Device Feature.
 *
 * \retval SHR_E_NONE Add test case into test database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_MEMORY Memory Allocated fail.
 */
extern int
bcma_testdb_testcase_add(bcma_test_db_t *tdb, bcma_test_case_t *tc, int feature);

/*!
 * \brief Parse test object key from CLI inputs.
 *
 * This function parses test object key from CLI inputs.
 *
 * \param [in] cli CLI Object.
 * \param [in] a CLI Arguments.
 * \param [in] tdb Test Database.
 * \param [out] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Add test case into test database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 */
extern int
bcma_testdb_parser(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
                   bcma_test_object_key_t *tokey);

/*!
 * \brief Select a specific test case.
 *
 * This function selects a specific test case.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Select a specific test case successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_select(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey);

/*!
 * \brief Deselect a specific test case.
 *
 * This function deselects a specific test case.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Select a specific test case successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_deselect(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey);

/*!
 * \brief List or Display test case information.
 *
 * This function lists or displays test case information.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 * \param [in] flags Test List flags. See BCMA_TEST_LIST_xxx
 *
 * \retval SHR_E_NONE List a specific test case successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_list(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey,
                 uint32_t flags);

/*!
 * \brief Clear the statistic counters for test instances.
 *
 * This function clears the statistic counters for test instances.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Clear test instance statistic successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_clear(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey);

/*!
 * \brief Remove test instances.
 *
 * This function removes test instances.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Remove test instances successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_remove(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey);

/*!
 * \brief Clone a specific test instance.
 *
 * This function clones a specific test instance.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 * \param [in] args CLI arguments for test parameters.
 *
 * \retval SHR_E_NONE Clone test instances successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_clone(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey,
                  char *args);

/*!
 * \brief Set parameters for a specific test instance.
 *
 * This function sets parameters for a specific test instance.
 *
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 * \param [in] args CLI arguments for test parameters.
 *
 * \retval SHR_E_NONE Set paramters for a specific test instances successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_parameters(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey,
                       char *args);

/*!
 * \brief Print help information.
 *
 * This function prints help information for a specific test case and instance.
 *
 * \param [in] cli CLI Object.
 * \param [in] a CLI Arguments.
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Print help information successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 */
extern int
bcma_testdb_help(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
                 bcma_test_object_key_t *tokey);

/*!
 * \brief Run test cases.
 *
 * This function runs test cases in TDB.
 *
 * \param [in] cli CLI Object.
 * \param [in] a CLI Arguments.
 * \param [in] tdb Test Database.
 * \param [in] tokey Test Objecte Key.
 *
 * \retval SHR_E_NONE Execute test case procedures successfully.
 * \retval SHR_E_NOT_FOUND Can not find the test object in TDB.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_MEMORY Memory Allocated fail.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_INTERNAL Ctrl-C break return.
 */
extern int
bcma_testdb_run(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
                bcma_test_object_key_t *tokey);

#endif /* BCMA_TESTDB_H */
