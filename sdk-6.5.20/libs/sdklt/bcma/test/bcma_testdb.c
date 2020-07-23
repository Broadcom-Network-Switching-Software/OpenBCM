/*! \file bcma_testdb.c
 *
 * Broadcom HMI test database framework
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_internal.h>
#include <sal/sal_libc.h>
#include <sal/sal_time.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmbd/bcmbd.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/io/bcma_io_ctrlc.h>

#include <bcma/test/bcma_testdb.h>

#include <bcmmgmt/bcmmgmt.h>
#include <bcmlrd/bcmlrd_init.h>
#include <bcmcfg/bcmcfg_init.h>
#include <bcma/bcmmgmt/bcma_bcmmgmt.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

typedef struct testdb_ud_list_s {
    const char *gn;
    uint32_t flags;
    int unit;
    int loops;
    int runs;
    int success;
    int fail;
} testdb_ud_list_t;

typedef struct testdb_ud_parse_s {
    const char *gn;
    const char *tn;
    bcma_test_object_key_t *tokey;
    int done;
} testdb_ud_parse_t;

/*!
 * \brief Allocate memory for test instance.
 *
 * Allocate memory for test instance data structure and
 * initialize the memory buffer.
 *
 * \return Memory pointer for allocated test instance or NULL.
 */
static bcma_test_inst_t *
testdb_test_inst_alloc(void)
{
    bcma_test_inst_t *ti;
    ti = sal_alloc(sizeof(*ti), "bcmaTestList");
    if (ti) {
        sal_memset(ti, 0, sizeof(*ti));
    }
    return ti;
}

/*!
 * \brief Recycle the allocated memory for test instance.
 *
 * Recycle the allocated memory for test instance.
 *
 * \param [in] ti Test Instance
 */
static void
testdb_test_inst_free(bcma_test_inst_t *ti)
{
    bcma_test_override_args_t *toa = &ti->override;
    if (toa->args) {
        SHR_FREE(toa->args);
    }
    SHR_FREE(ti);
}

/*!
 * \brief Check test instance by a given instance number.
 *
 * Checking for whether this test instance can match a given
 * instance number according to a matching rule.
 *
 * \param [in] ti Test Instance
 * \param [in] inst Test Instance Number
 * \param [in] exact Matching Rule[0: fuzzy, 1:exact]
 *
 * \return true/false for matched or not.
 */
static int
testdb_match_test_inst(bcma_test_inst_t *ti, int inst, int exact)
{
    if (exact) {
        if (inst == ti->inst) {
            return true;
        }
    } else {
        if (inst == ti->inst || inst == BCMA_TEST_INST_ANY) {
            return true;
        }
    }
    return false;
}

/*!
 * \brief Check test case list by a given test identifier.
 *
 * Checking for whether this test case can match a given
 * test identifier according to a matching rule.
 *
 * \param [in] tl Test Case List
 * \param [in] unit Device Number
 * \param [in] id Test Case Identifier
 * \param [in] exact Matching Rule[0: fuzzy, 1:exact]
 *
 * \return true/false for matched or not.
 */
static int
testdb_match_test_list(bcma_test_list_t *tl, int unit, int id, int exact)
{
    bcma_test_case_t *tc = tl->tc;
    if (tc->feature == 0 || bcmdrd_feature_enabled(unit, tc->feature)) {
        if (exact) {
            if (id == tc->id) {
                return true;
            }
        } else {
            if (id == tc->id || id == BCMA_TEST_ID_ANY) {
                return true;
            }
        }
    }
    return false;
}

/*!
 * \brief Check test group by a given test group name.
 *
 * Checking for whether this test group can match a given
 * test group name according to a matching rule.
 *
 * \param [in] tg Test Group
 * \param [in] gn Test Group Name
 * \param [in] exact Matching Rule[0: fuzzy, 1:exact]
 *
 * \return true/false for matched or not.
 */
static int
testdb_match_group(bcma_test_group_t *tg, const char *gn, int exact)
{
    if (exact) {
        if (gn != BCMA_TEST_GROUP_ANY &&
            sal_strncasecmp(tg->name, gn,
                            BCMA_TEST_CONFIG_NAME_BUFFER_MAX - 1) == 0) {
            return true;
        }
    } else {
        if (gn == BCMA_TEST_GROUP_ANY ||
            sal_strncasecmp(tg->name, gn,
                            BCMA_TEST_CONFIG_NAME_BUFFER_MAX - 1) == 0) {
            return true;
        }
    }
    return false;
}

/*!
 * \brief Traverse in TDB.
 *
 * Traverse a TDB with a given test object key and matching
 * rule. If any test object matched, call callback function and
 * user data to implement certain target.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 * \param [in] exact Matching Rule[0: fuzzy, 1:exact]
 * \param [in] cb Call back function
 * \param [in] user_data User data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not found any test object.
 * \return SHR_E_xxx For other failures.
 */
static int
testdb_traverse(bcma_test_db_t *tdb,
                bcma_test_object_key_t *tokey,
                int exact,
                int (*cb)(bcma_test_db_t *tdb,
                          bcma_test_object_t *to,
                          void *user_data),
                void *user_data)
{
    bcma_test_group_t *tg;
    bcma_test_list_t *tl, *tl_next;
    bcma_test_inst_t *ti, *ti_next;
    bcma_test_object_t to;
    int match = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    tg = tdb->tg;
    while (tg) {
        if (testdb_match_group(tg, tokey->gn, false)) {
            tl = tg->tlist;
            while (tl) {
                tl_next = tl->next;
                if (testdb_match_test_list(tl, tokey->unit, tokey->id, exact)) {
                    ti = tl->ilist;
                    while (ti) {
                        ti_next = ti->next;
                        if (testdb_match_test_inst(ti, tokey->inst, exact)) {
                            match = 1;
                            if (cb) {
                                to.tg = tg;
                                to.tc = tl->tc;
                                to.ti = ti;
                                SHR_IF_ERR_VERBOSE_EXIT
                                    (cb(tdb, &to, user_data));
                            }
                            if (exact) {
                                SHR_EXIT();
                            }
                        }
                        ti = ti_next;
                    }
                }
                tl = tl_next;
            }
        }
        tg = tg->next;
    }

    if (!match) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A test lookup engine in TDB.
 *
 * A test lookup engine in TDB for searching a list of test objects
 * with a test object key and matching rule. Then, the matched
 * test object will be handled by a given handler function.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 * \param [in] exact Matching Rule[0: fuzzy, 1:exact]
 * \param [in] toh Test Object Handler function
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not found any test object.
 * \return SHR_E_xxx For other failures.
 */
static int
testdb_lookup_engine(bcma_test_db_t *tdb,
                     bcma_test_object_key_t *tokey,
                     int exact,
                     bcma_test_object_hndlr_t *toh)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);
    SHR_NULL_CHECK(toh, SHR_E_PARAM);

    if (exact) {
        if (tokey->id != BCMA_TEST_ID_ANY &&
            tokey->inst == BCMA_TEST_INST_ANY) {
            tokey->inst = BCMA_TEST_INST_DEFAULT;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_traverse(tdb, tokey, exact, toh->cb, toh->user_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find a test instance from a test case list in a test group.
 *
 * Find a test instance from a test case list in a test group by
 * a given test instance number.
 *
 * \param [in] tl Test Case List
 * \param [in] inst Test Instance Number
 *
 * \return test instance pointer or NULL.
 */
static bcma_test_inst_t *
testdb_group_list_find_inst(bcma_test_list_t *tl, int inst)
{
    bcma_test_inst_t *ti = tl->ilist;
    while(ti) {
        if (testdb_match_test_inst(ti, inst, true)) {
            return ti;
        }
        ti = ti->next;
    }
    return NULL;
}

/*!
 * \brief Add a test instance into a test case list.
 *
 * Add a test instance into a test case list.
 *
 * \param [in] tl Test Case List
 * \param [in] ti Test Instance
 */
static void
testdb_group_list_add_inst(bcma_test_list_t *tl, bcma_test_inst_t *ti)
{
    bcma_test_inst_t *_ti, *_ti_prev;

    if (tl->ilist == NULL) {
        tl->ilist = ti;
        if (ti->inst == BCMA_TEST_INST_ANY) {
            ti->inst = BCMA_TEST_INST_DEFAULT;
        }
    } else {
        _ti = tl->ilist;
        _ti_prev = NULL;
        while (_ti) {
            if (ti->inst > BCMA_TEST_INST_ANY &&
                ti->inst < _ti->inst) {
                ti->next = _ti;
                if (_ti_prev == NULL) {
                    tl->ilist = ti;
                } else {
                    _ti_prev->next = ti;
                }
                break;
            }
            _ti_prev = _ti;
            _ti = _ti->next;
        }
        if (_ti == NULL) {
            _ti_prev->next = ti;
            ti->inst = _ti_prev->inst + 1;
        }
    }
}

/*!
 * \brief Remove a test instance from a test case list.
 *
 * Remove a test instance from a test case list by a given
 * test instance number.
 *
 * \param [in] tl Test Case List
 * \param [in] inst Test Instance Number
 */
static void
testdb_group_list_remove_inst(bcma_test_list_t *tl, int inst)
{
    bcma_test_inst_t *ti, *ti_next;

    if (tl->ilist) {
        ti = tl->ilist;
        ti_next = ti->next;
        if (testdb_match_test_inst(ti, inst, true)) {
            tl->ilist = ti_next;
            testdb_test_inst_free(ti);
        } else {
            while (ti_next) {
                if (testdb_match_test_inst(ti_next, inst, true)) {
                    ti->next = ti_next->next;
                    testdb_test_inst_free(ti_next);
                    break;
                }
                ti = ti_next;
                ti_next = ti_next->next;
            }
        }
    }
}

/*!
 * \brief Remove all test instance in a test case list.
 *
 * Remove all test instance in a test case list.
 *
 * \param [in] tl Test Case List
 */
static void
testdb_group_list_remove_inst_all(bcma_test_list_t *tl)
{
    bcma_test_inst_t *ti, *ti_next;
    ti = tl->ilist;
    while (ti) {
        ti_next = ti->next;
        testdb_group_list_remove_inst(tl, ti->inst);
        ti = ti_next;
    }
    tl->ilist = NULL;
}

/*!
 * \brief Add a test case list node into a test group.
 *
 * Add a test case list node into a test group.
 *
 * \param [in] tg Test Group
 * \param [in] tl Test Case List
 */
static void
testdb_group_add_list(bcma_test_group_t *tg, bcma_test_list_t *tl)
{
    bcma_test_list_t *_tl, *_tl_prev;
    bcma_test_case_t *_tc, *tc;

    tc = tl->tc;

    if (tg->tlist == NULL) {
        tg->tlist = tl;
    } else {
        _tl = tg->tlist;
        _tl_prev = NULL;
        while (_tl) {
            _tc = _tl->tc;
            if (_tc->id > tc->id) {
                tl->next = _tl;
                if (_tl_prev == NULL) {
                    tg->tlist = tl;
                } else {
                    _tl_prev->next = tl;
                }
                break;
            }
            _tl_prev = _tl;
            _tl = _tl->next;
        }
        if (_tl == NULL) {
            _tl_prev->next = tl;
        }
    }
}

/*!
 * \brief Find a test case list node from a test group.
 *
 * Find a test case list node from a test group by given
 * Test case identifier and device feature.
 *
 * \param [in] tg Test Group
 * \param [in] id Test Case Identifier
 * \param [in] feature Device Feature
 *
 * \return a test case list node or NULL.
 */
static bcma_test_list_t *
testdb_group_find_list(bcma_test_group_t *tg, int id, int feature)
{
    bcma_test_list_t *tl = tg->tlist;
    bcma_test_case_t *tc;

    while (tl) {
        tc = tl->tc;
        if ((tc->id == id) && (tc->feature == feature)) {
            return tl;
        }
        tl = tl->next;
    }

    return NULL;
}

/*!
 * \brief Add a test instance into a test case list of a test group.
 *
 * Add a test instance into a test case list of a test group.
 *
 * \param [in] tg Test Group
 * \param [in] tc Test Case
 * \param [in] ti Test Instance
 */
static void
testdb_group_add_inst(bcma_test_group_t *tg, bcma_test_case_t *tc,
                      bcma_test_inst_t *ti)
{
    bcma_test_list_t *tl;
    tl = tg->tlist;
    while (tl) {
        if (tl->tc == tc) {
            testdb_group_list_add_inst(tl, ti);
        }
        tl = tl->next;
    }
}

/*!
 * \brief Remove a test instance from a test case list of a test group.
 *
 * Add a test instance into a test case list of a test group
 * by a given test instance number.
 *
 * \param [in] tg Test Group
 * \param [in] tc Test Case
 * \param [in] inst Test Instance Number
 */
static void
testdb_group_remove_inst(bcma_test_group_t *tg, bcma_test_case_t *tc, int inst)
{
    bcma_test_list_t *tl;
    tl = tg->tlist;
    while (tl) {
        if (tl->tc == tc) {
            testdb_group_list_remove_inst(tl, inst);
        }
        tl = tl->next;
    }
}

/*!
 * \brief Remove all test instances of all test cases in a test group.
 *
 * Remove all test instances of all test cases in a test group.
 *
 * \param [in] tg Test Group
 */
static void
testdb_group_remove_list_inst_all(bcma_test_group_t *tg)
{
    bcma_test_list_t *tl, *tl_next;
    tl = tg->tlist;
    while (tl) {
        tl_next = tl->next;
        testdb_group_list_remove_inst_all(tl);
        SHR_FREE(tl);
        tl = tl_next;
    }
    tg->tlist = NULL;
}

/*!
 * \brief Find a specific test group in TDB.
 *
 * Find a specific test group in TDB by given Test
 * Group Name.
 *
 * \param [in] tdb Test Database
 * \param [in] gn Test Group Name
 *
 * \return test group pointer or NULL.
 */
static bcma_test_group_t *
testdb_find_group(bcma_test_db_t *tdb, const char *gn)
{
    bcma_test_group_t *tg = tdb->tg;
    while (tg) {
        if (sal_strncasecmp(tg->name, gn,
                            BCMA_TEST_CONFIG_NAME_BUFFER_MAX - 1) == 0) {
            return tg;
        }
        tg = tg->next;
    }
    return NULL;
}

/*!
 * \brief Find or create a specific test group in TDB.
 *
 * Find or create a specific test group in TDB by given Test
 * Group Name. If group does not exist, create a new group
 * and return the test group pointer.
 *
 * \param [in] tdb Test Database
 * \param [in] gn Test Group Name
 *
 * \return test group pointer or NULL.
 */
static bcma_test_group_t *
testdb_find_or_create_group(bcma_test_db_t *tdb, const char *gn)
{
    bcma_test_group_t *tg = NULL;
    int str_len;

    str_len = sal_strlen(gn) + 1;
    if (str_len > BCMA_TEST_CONFIG_NAME_BUFFER_MAX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Group name exceeds buffer length\n")));
        return NULL;
    }

    tg = testdb_find_group(tdb, gn);

    /*! Not found, Create a new one */
    if (tg == NULL) {
        tg = sal_alloc(sizeof(*tg), "bcmaTestGroup");
        if (tg) {
            sal_memset(tg, 0, sizeof(*tg));
            sal_strcpy(tg->name, gn);
            tg->next = tdb->tg;
            tdb->tg = tg;
        }
    }

    return tg;
}

/*!
 * \brief converse the input parameters into test override arguments.
 *
 * converse the input parameters into test override arguments.
 *
 * \param [in] arg The input override arguments
 * \param [in] toa Test override arguments
 */
static void
testdb_override_args_resolve(char *arg, bcma_test_override_args_t *toa)
{
    int unit, len, len_p;
    char p[128], *ptr;

    /* Format:
     *      "[<unit>:]<arguments>"
     * e.g. test parameters 11 "0:M=L2Xm Write=0 IntMode=0"
     *      test clone 11 "0:M=L2Xm Write=0 IntMode=1"
     */
    if (arg && *arg) {
        if ((ptr = sal_strchr(arg, ':')) != NULL) {
            len_p = (sizeof(p) - 1);
            len = (ptr - arg) <= len_p ? (ptr - arg) : len_p;
            sal_strncpy(p, arg, len);
            p[len] = '\0';
            ptr++;
            if (sal_strcmp(p, "*") == 0) {
                unit = -1;
            } else if (bcma_cli_parse_is_int(p)) {
                unit = sal_atoi(p);
            } else {
                unit = -1;
            }
        } else {
            ptr = arg;
            unit = -1;
        }
        toa->args = sal_strdup(ptr);
        if (toa->args == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Error: Out of memory\n")));
            toa->unit = 0;
        } else {
            toa->unit = unit;
        }
    }
}

/*!
 * \brief A Ctrl-C protected execution function.
 *
 * A Ctrl-C protected execution function.
 *
 * \param [in] func A function which needs Ctrl-C protection
 * \param [in] ted Test Engine Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Receive Ctrl-C Signal.
 * \return SHR_E_xxx Other failures.
 */
static int
testdb_ctrlc_exec(int (*func)(void *), bcma_test_engine_data_t *ted)
{
    return bcma_io_ctrlc_exec(func, ted, NULL, NULL, SHR_E_INTERNAL);
}

/*!
 * \brief Check whether a test instance of a test case is selected or not.
 *
 * Check whether a test instance of a test case is selected or not.
 *
 * \param [in] unit Unit number
 * \param [in] tc Test Case
 * \param [in] ti Test Instance
 *
 * \return true/false for selected or not.
 */
static int
testdb_test_inst_is_selected(int unit, bcma_test_case_t *tc,
                             bcma_test_inst_t *ti)
{
    int select, u, flags;
    bcma_test_op_t *top;
    bcma_test_override_args_t *toa;

    toa = &ti->override;
    top = tc->op;
    u = (toa->args && toa->unit >= 0) ? toa->unit : unit;
    flags = ti->flags;

    if (u == unit) {
        select = top->select && top->select(u);
        select = select || (flags & BCMA_TEST_INST_F_SELECT ? true : false);
        select = select && (flags & BCMA_TEST_INST_F_UNSUPPORT ? false : true);
    } else {
        /* Skip it if override arguments are NOT for this unit */
        select = false;
    }

    return select;
}

/*!
 * \brief Check TDB is enabled or not.
 *
 * Check TDB is enabled or not.
 *
 * \param [in] tdb Test Database
 *
 * \return true/false for enabled or not.
 */
static int
testdb_is_enabled(bcma_test_db_t *tdb)
{
    if (tdb && tdb->signature == BCMA_TEST_SIGNATURE) {
        return true;
    }
    return false;
}

/*!
 * \brief Initialize the TDB.
 *
 * Initialize a specific TDB.
 *
 * \param [in] tdb Test Database
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_MEMORY No Memory Resource.
 */
static int
testdb_init(bcma_test_db_t *tdb)
{
    bcma_test_group_t *tg;
    bcma_test_list_t *tl;
    bcma_test_inst_t *ti;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);

    if (testdb_is_enabled(tdb)) {
        tdb->options = 0;
        tg = tdb->tg;
        while (tg) {
            tl = tg->tlist;
            while (tl) {
                testdb_group_list_remove_inst_all(tl);
                ti = testdb_test_inst_alloc();
                if (ti == NULL) {
                    SHR_ERR_EXIT(SHR_E_MEMORY);
                }
                ti->inst = BCMA_TEST_INST_DEFAULT;
                testdb_group_list_add_inst(tl, ti);
                tl = tl->next;
            }
            tg = tg->next;
        }
        tdb->signature = 0;
    }
    tdb->signature = BCMA_TEST_SIGNATURE;
    /* Set default device state to BCMA_TEST_DEV_STATE_INIT state. */
    sal_memset(tdb->state, 0, sizeof(tdb->state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up the TDB.
 *
 * Clean up a specific TDB.
 *
 * \param [in] tdb Test Database
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_cleanup(bcma_test_db_t *tdb)
{
    bcma_test_group_t *tg, *tg_next;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);

    if (testdb_is_enabled(tdb)) {
        tdb->options = 0;
        tg = tdb->tg;
        while (tg) {
            tg_next = tg->next;
            testdb_group_remove_list_inst_all(tg);
            SHR_FREE(tg);
            tg = tg_next;
        }
        tdb->tg = NULL;
        tdb->signature = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a test case into a test group of TDB.
 *
 * Add a test case into a test group of TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tc Test Case
 * \param [in] feature Device Feature
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_MEMORY No Memory Resource.
 */
static int
testdb_testcase_add(bcma_test_db_t *tdb, bcma_test_case_t *tc, int feature)
{

    bcma_test_group_t *tg;
    bcma_test_list_t *tl;
    bcma_test_inst_t *ti;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tc, SHR_E_PARAM);

    tg = testdb_find_or_create_group(tdb, tc->group);
    if (tg == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    tl = testdb_group_find_list(tg, tc->id, feature);
    if (tl == NULL) {
        SHR_ALLOC(tl, sizeof(*tl), "bcmaTestList");
        if (tl == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(tl, 0, sizeof(*tl));
        tc->feature = feature;
        tc->op = tc->op_get();
        tl->tc = tc;
        testdb_group_add_list(tg, tl);
    }

    ti = testdb_group_list_find_inst(tl, BCMA_TEST_INST_DEFAULT);
    if (ti == NULL) {
        ti = testdb_test_inst_alloc();
        if (ti == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        ti->inst = BCMA_TEST_INST_DEFAULT;
        testdb_group_list_add_inst(tl, ti);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a test case into a test group of TDB.
 *
 * Add a test case into a test group of TDB.
 *
 * \param [in] id Test Case Identifier
 * \param [in] inst Test Instance Number
 * \param [in/out] s Test Case Id formatted strings
 * \param [in] sz Device Feature
 */
static void
testdb_id_format(int id, int inst, char *s, int sz)
{
    int len = 0;
    len += sal_snprintf(s + len, sz, "%"PRId32"", id);
    if (inst > BCMA_TEST_INST_DEFAULT) {
        len += sal_snprintf(s + len, sz, ".%"PRId32"", inst);
    }
}

/*!
 * \brief A callback function of Test Object Handler for
 * parsing test object key.
 *
 * A callback function of Test Object Handler for parsing test object key.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_parse_tokey_cb(bcma_test_db_t *tdb, bcma_test_object_t *to,
                      void *user_data)
{
    testdb_ud_parse_t *ud_parse = (testdb_ud_parse_t *)user_data;
    bcma_test_group_t *tg;
    bcma_test_case_t *tc;
    bcma_test_object_key_t *tokey;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);
    SHR_NULL_CHECK(ud_parse, SHR_E_PARAM);

    tg = to->tg;
    tc = to->tc;
    tokey = ud_parse->tokey;

    if (!ud_parse->done) {
        if (ud_parse->tn) {
            if (sal_strncasecmp(tc->name,
                                ud_parse->tn, sal_strlen(tc->name)) == 0) {
                tokey->id = tc->id;
                ud_parse->gn = tg->name;
                ud_parse->done = true;
            }
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse a test group by test group name.
 *
 * Parse a test group by test group name.
 *
 * \param [in] tdb Test Database
 * \param [in] gn Test Group Name
 * \param [out] tg Test Group
 *
 * \return true/false Parse the test group successfully or not.
 */
static int
testdb_parse_tokey_group(bcma_test_db_t *tdb, const char *gn,
                         bcma_test_group_t **tg)
{
    if ((*tg = testdb_find_group(tdb, gn)) != NULL) {
        return true;
    }
    return false;
}

/*!
 * \brief Parse test object key with CLI input strings.
 *
 * Parse test object key with CLI input strings.
 *
 * \param [in] tdb Test Database
 * \param [in] s CLI strings
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_parse_tokey_string(bcma_test_db_t *tdb, const char *s,
                          bcma_test_object_key_t *tokey)
{
    char *ptr, sd[256];
    int argc = 0;
    char *argv[BCMA_TEST_CONFIG_OBJECT_PARTITION_MAX];
    bcma_test_group_t *tg;
    bcma_test_object_hndlr_t toh;
    testdb_ud_parse_t ud_parse;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(s, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    sal_strlcpy(sd, s, sizeof(sd));

    argv[argc++] = sd;
    while ((ptr = sal_strchr(argv[argc - 1], '.')) != NULL) {
        if (argc < BCMA_TEST_CONFIG_OBJECT_PARTITION_MAX) {
            *ptr = '\0';
            ptr++;
            argv[argc++] = ptr;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    switch (argc) {
        case 1: /* <id> | <gn> | <tn> */
            if (bcma_cli_parse_is_int(argv[0])) {
                tokey->id = sal_atoi(argv[0]);
            } else {
                if (testdb_parse_tokey_group(tdb, argv[0], &tg)) {
                    tokey->gn = tg->name;
                } else {
                    ud_parse.gn = NULL;
                    ud_parse.tn = argv[0];
                    ud_parse.tokey = tokey;
                    ud_parse.done = false;
                    toh.cb = testdb_parse_tokey_cb;
                    toh.user_data = &ud_parse;
                    SHR_IF_ERR_EXIT
                        (testdb_lookup_engine(tdb, tokey, false, &toh));
                    if (!ud_parse.done) {
                        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                    }
                    tokey->gn = ud_parse.gn;
                }
            }
            break;
        case 2: /* <id.inst> | <gn.tn> */
            if (bcma_cli_parse_is_int(argv[0])) {
                tokey->id = sal_atoi(argv[0]);
                if (bcma_cli_parse_is_int(argv[1])) {
                    tokey->inst = sal_atoi(argv[1]);
                } else {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            } else {
                if (testdb_parse_tokey_group(tdb, argv[0], &tg)) {
                    tokey->gn = tg->name;
                    ud_parse.gn = NULL;
                    ud_parse.tn = argv[1];
                    ud_parse.tokey = tokey;
                    ud_parse.done = false;
                    toh.cb = testdb_parse_tokey_cb;
                    toh.user_data = &ud_parse;
                    SHR_IF_ERR_EXIT
                        (testdb_lookup_engine(tdb, tokey, false, &toh));
                    if (!ud_parse.done) {
                        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                    }
                } else {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            break;
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Error: Unable to parse object: %s (%"PRId32")\n"),
                   s, argc));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse test object key with CLI input arguments.
 *
 * Parse test object key with CLI input arguments.
 *
 * \param [in] cli CLI Object
 * \param [in] a CLI arguments
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_parse(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
             bcma_test_object_key_t *tokey)
{
    char *arg;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    tokey->unit = cli->cmd_unit;
    tokey->gn = BCMA_TEST_GROUP_ANY;
    tokey->id = BCMA_TEST_ID_ANY;
    tokey->inst = BCMA_TEST_INST_ANY;

    if (BCMA_CLI_ARG_CNT(a) == 0) {
        SHR_EXIT();
    }

    arg = BCMA_CLI_ARG_GET(a);
    if (sal_strcmp(arg, "*") == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_parse_tokey_string(tdb, arg, tokey));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * selecting/deselecting a test object.
 *
 * A callback function of Test Object Handler for
 * selecting/deselecting a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_select_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    int select;
    bcma_test_inst_t *ti;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);
    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    select = *(int *)user_data;
    ti = to->ti;

    if (select) {
        ti->flags |=  BCMA_TEST_INST_F_SELECT;
        ti->flags &= ~BCMA_TEST_INST_F_UNSUPPORT;
    } else {
        ti->flags &= ~BCMA_TEST_INST_F_SELECT;
        ti->flags |=  BCMA_TEST_INST_F_UNSUPPORT;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Select a list of test instances into TDB.
 *
 * Select a list of test instances into TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_select(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    int select = true;
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    toh.cb = testdb_select_cb;
    toh.user_data = &select;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, false, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deselect a list of test instances into TDB.
 *
 * Deselect a list of test instances into TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_deselect(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    int select = false;
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    toh.cb = testdb_select_cb;
    toh.user_data = &select;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, false, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Print header for list command.
 *
 * Print header for list command.
 */
static void
testdb_list_print_header(int status_report)
{
    if (status_report) {
        cli_out("-----+----+----+--- TEST STATUS REPORT ----+"
                "-----+-----+-----+-----+-----------\n");
    }

    cli_out("U/A/S|Test|Inst|            Test           |"
            "Loop | Run |Pass |Fail |  Arguments\n");
    cli_out("     | #  |    |            Name           |"
            "Count|Count|Count|Count|\n");
}

/*!
 * \brief Print Test Group Name for list command.
 *
 * Print Test Group Name for list command.
 */
static void
testdb_list_print_group_name(const char *gn)
{
    cli_out("%s:\n", gn);
}

/*!
 * \brief Print separator for list command.
 *
 * Print separator for list command.
 */
static void
testdb_list_print_separator(void)
{
    cli_out("-----+----+----+---------------------------+"
            "-----+-----+-----+-----+-----------\n");
}

/*!
 * \brief Print a test instance entry for list command.
 *
 * Print a test instance entry for list command.
 */
static void
testdb_list_print_entry(int unit, bcma_test_case_t *tc, bcma_test_inst_t *ti)
{
    int selected;
    bcma_test_override_args_t *toa;
    char ustr[128] = {0};
    char buf[26];
    const char *ptr, *line, *space;
    int max, len, multiline = 0;

    toa = &ti->override;

    if (toa->args && toa->unit >= 0) {
        sal_snprintf(ustr, 128, "%"PRId32":", toa->unit);
    }

    selected = testdb_test_inst_is_selected(unit, tc, ti);


    line = ptr = space = tc->name;
    max = 26 - 1;

    do {
        while (*line == ' ') {
            line++;
            ptr++;
        }
        if (*line == '\0') {
            break;
        }
        if (*ptr == ' ') {
            space = ptr;
        }
        len = ptr - line;
        if (len >= max || *ptr == '\0') {
            if (*ptr != '\0' && *ptr != ' ') {
                if (space > line && space < ptr) {
                    ptr = space;
                }
                len = ptr - line;
            }
            sal_memcpy(buf, line, len);
            buf[len] = '\0';
            if (multiline == 0) {
                cli_out(" %c%c%c |%4"PRId32"|%4"PRId32"| %-26s|%5"PRId32"|"
                        "%5"PRId32"|%5"PRId32"|%5"PRId32"| %s%s\n",
                        selected ? ' ' : 'U',
                        ' ',
                        selected ? 'S' : ' ',
                        tc->id, ti->inst, buf, tc->loops, ti->stats.runs,
                        ti->stats.success, ti->stats.fail,
                        toa->args ? ustr : "",
                        toa->args ? toa->args :
                                    tc->dflt_args ? tc->dflt_args : "(none)");
                multiline++;
            } else {
                cli_out("     |    |    | %-26s|     |     |     |     |\n",
                        buf);
            }
            line = ptr;
        }
    } while (*ptr++);
}

/*!
 * \brief Print summary information for list command.
 *
 * Print summary information for list command.
 */
static void
testdb_list_print_summary(int loops, int runs, int successes, int failures)
{
    cli_out("     |    |    | %-26s|%5d|%5d|%5d|%5d|\n",
            "TOTAL", loops, runs, successes, failures);
}

/*!
 * \brief A callback function of Test Object Handler for
 * listing/displaying a test object.
 *
 * A callback function of Test Object Handler for
 * listing/displaying a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_list_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    testdb_ud_list_t *ud = (testdb_ud_list_t *)user_data;
    bcma_test_group_t *tg;
    bcma_test_case_t *tc;
    bcma_test_inst_t *ti;
    int selected;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);
    SHR_NULL_CHECK(ud, SHR_E_PARAM);

    tg = to->tg;
    tc = to->tc;
    ti = to->ti;

    if (ud->flags & BCMA_TEST_LIST_FAIL) {
        if (ti->stats.fail == 0) {
            SHR_EXIT();
        }
    } else if (ud->flags & BCMA_TEST_LIST_SUPPORT) {
        selected = testdb_test_inst_is_selected(ud->unit, tc, ti);
        if (!selected) {
            SHR_EXIT();
        }
    }

    if (ud->gn == NULL ||
        sal_strncmp(tg->name, ud->gn, sal_strlen(tg->name)) != 0) {
        testdb_list_print_separator();
        testdb_list_print_group_name(tg->name);
        testdb_list_print_separator();
        ud->gn = tg->name;
    }
    testdb_list_print_entry(ud->unit, tc, ti);
    ud->loops += tc->loops;
    ud->runs += ti->stats.runs;
    ud->success += ti->stats.success;
    ud->fail += ti->stats.fail;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief List or display a list of test instances in TDB.
 *
 * List or display a list of test instances in TDB. This function
 * can list supported, failed, all test instances separately by
 * a given flags.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 * \param [in] flags Test List Flags
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 * \return SHR_E_FAIL List failed.
 */
static int
testdb_list(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey, uint32_t flags)
{
    testdb_ud_list_t ud = {0};
    bcma_test_group_t *tg;
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);

    if (tokey == NULL) {
        tg = tdb->tg;
        while (tg) {
            cli_out(" %s", tg->name);
            tg = tg->next;
        }
        cli_out("\n");
        SHR_EXIT();
    }

    ud.flags = flags;
    ud.unit = tokey->unit;

    testdb_list_print_header(TRUE);

    toh.cb = testdb_list_cb;
    toh.user_data = &ud;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (testdb_lookup_engine(tdb, tokey, false, &toh), SHR_E_NOT_FOUND);

    testdb_list_print_separator();
    testdb_list_print_summary(ud.loops, ud.runs, ud.success, ud.fail);
    testdb_list_print_separator();

    if (flags & BCMA_TEST_LIST_FAIL) {
        if (ud.fail == 0) {
            cli_out("All tests passed\n");
        }
    }

    if (flags & (BCMA_TEST_LIST_ALL | BCMA_TEST_LIST_SUPPORT)) {
        if (ud.runs == 0 || ud.fail > 0) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * clearing statistic of a test object.
 *
 * A callback function of Test Object Handler for
 * clearing statistic of a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_clear_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    bcma_test_inst_t *ti;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);

    COMPILER_REFERENCE(user_data);

    ti = to->ti;
    sal_memset(&ti->stats, 0, sizeof(ti->stats));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear statistics for a list of test instances in TDB.
 *
 * Clear statistics for a list of test instances in TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_clear(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    toh.cb = testdb_clear_cb;
    toh.user_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, false, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * removing a test object.
 *
 * A callback function of Test Object Handler for
 * removing a test object. The default test instance
 * can not be removed by this function.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_remove_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    bcma_test_inst_t *ti;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);

    COMPILER_REFERENCE(user_data);

    ti = to->ti;

    if (ti->inst != BCMA_TEST_INST_DEFAULT) {
        testdb_group_remove_inst(to->tg, to->tc, ti->inst);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a list of test instances in TDB.
 *
 * Remove a list of test instances in TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_remove(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    toh.cb = testdb_remove_cb;
    toh.user_data = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, false, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * cloning a test object.
 *
 * A callback function of Test Object Handler for
 * cloning a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_clone_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    char *args = (char *)user_data;
    bcma_test_group_t *tg;
    bcma_test_case_t *tc;
    bcma_test_override_args_t *toa;
    bcma_test_inst_t *ti_new;
    char istr[128];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);

    tg = to->tg;
    tc = to->tc;

    ti_new = testdb_test_inst_alloc();
    if (ti_new == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    ti_new->inst = BCMA_TEST_INST_ANY;
    testdb_group_add_inst(tg, tc, ti_new);

    testdb_id_format(tc->id, ti_new->inst, istr, 128);
    cli_out("clone instance (%s)\n", istr);

    if (args && *args) {
        toa = &ti_new->override;
        testdb_override_args_resolve(args, toa);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clone a new test instance from a existing test instances in TDB.
 *
 * Clone a new test instance from a existing test instances in TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 * \param [in] args Test Override Arguments
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_clone(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey, char *args)
{
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    toh.cb = testdb_clone_cb;
    toh.user_data = args;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, true, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * setting override arguments to a test object.
 *
 * A callback function of Test Object Handler for
 * setting override arguments to a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_parameters_cb(bcma_test_db_t *tdb,
                     bcma_test_object_t *to,
                     void *user_data)
{
    char *args = (char *)user_data;
    bcma_test_case_t *tc;
    bcma_test_inst_t *ti;
    bcma_test_override_args_t *toa;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);

    tc = to->tc;
    ti = to->ti;
    toa = &ti->override;

    if (!args || !*args) {                /* Clear parameter */
        if (!toa->args) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Warning: No arguments to clear for test: %s\n"),
                      tc->name));
            SHR_EXIT();
        }
        SHR_FREE(toa->args);
        toa->unit = 0;
    } else {                        /* Set Arguments */
        if (toa->args) {
            SHR_FREE(toa->args);
            toa->unit = 0;
        }
        testdb_override_args_resolve(args, toa);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Parameter set: %s (\"%"PRId32":%s\")\n"),
                     tc->name, toa->unit, toa->args));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setting/Remove override arguments for a existing test instances
 * in TDB.
 *
 * Setting/Remove override arguments for a existing test instances
 * in TDB.
 *
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 * \param [in] args Test Override Arguments
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_NOT_FOUND Can not find Test Object.
 */
static int
testdb_parameters(bcma_test_db_t *tdb,
                  bcma_test_object_key_t *tokey,
                  char *args)
{
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    toh.cb = testdb_parameters_cb;
    toh.user_data = args;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, true, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check select status for a test object in TDB on a given unit.
 *
 * Check select status for a test object in TDB on a given unit.
 *
 * \param [in] unit Unit number
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 *
 * \return selected or not.
 */
static int
testdb_run_selected(int unit, bcma_test_db_t *tdb, bcma_test_object_t *to)
{
    /* coverity[dont_call] */
    int rand32 = sal_rand();
    int selected;

    selected = testdb_test_inst_is_selected(unit, to->tc, to->ti);
    if (tdb->options & BCMA_TEST_O_RANDOM) {
        selected = selected && (rand32 & 0x1);
    }

    return selected;
}

/*!
 * \brief Validate the test result for execution phase.
 *
 * Validate the test result for execution phase.
 *
 * \param [in] result Test Result
 * \param [in] tdb Test Database
 * \param [in] tc Test Case
 * \param [in] ti Test Instance
 * \param [in] unit Unit number
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_xxx Other failures.
 */
static int
testdb_run_result_validate(int result,
                           bcma_test_db_t *tdb,
                           bcma_test_case_t *tc,
                           bcma_test_inst_t *ti,
                           int unit)
{
    bcma_test_stat_t *tstat;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tc, SHR_E_PARAM);
    SHR_NULL_CHECK(ti, SHR_E_PARAM);

    tstat = &ti->stats;

    if (result == SHR_E_NONE) {
        tstat->success++;
        if (tc->flags & BCMA_TEST_F_DESTRUCTIVE) {
            tdb->state[unit] = BCMA_TEST_DEV_STATE_INDETERMINATE;
        }
    } else {
        tstat->fail++;
        tdb->state[unit] = BCMA_TEST_DEV_STATE_INDETERMINATE;
        if (tdb->options & BCMA_TEST_O_AOE) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(result);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse phase operation function for Test Engine.
 *
 * Parse phase operation function for Test Engine.
 *
 * \param [in/out] data Test Engine Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_MEMORY No Memory Resource.
 * \return SHR_E_FAIL Test Parse Failed.
 */
static int
testdb_run_parse(void *data)
{
    bcma_test_engine_data_t *ted = (bcma_test_engine_data_t *)data;
    bcma_test_object_t *to;
    bcma_test_inst_t *ti;
    bcma_test_case_t *tc;
    bcma_test_op_t *top;
    bcma_test_override_args_t *toa;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(ted, SHR_E_PARAM);

    to = ted->to;
    tc = to->tc;
    ti = to->ti;
    top = tc->op;
    toa = &ti->override;

    if (ted->a == NULL) {
        ted->av = NULL;
        SHR_ALLOC(ted->av, sizeof(bcma_cli_args_t), "bcmaTestArgs");
        if (ted->av == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(ted->av, 0, sizeof(bcma_cli_args_t));

        if (bcma_cli_parse_args(ted->cli,
                                toa->args ? toa->args : tc->dflt_args,
                                NULL,
                                ted->av) < 0) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    /* Initialize the test case */
    if (ted->flags & BCMA_TEST_ENGINE_F_HELP) {
        cli_out("Test Arguments:\n");
    }
    if (top->parser) {
        SHR_IF_ERR_VERBOSE_EXIT
            (top->parser(ted->unit, ted->cli,
                         ted->av ? ted->av : ted->a,
                         ted->flags, &ted->bp));
    } else {
        if (ted->flags & BCMA_TEST_ENGINE_F_HELP) {
            cli_out("  None.\n");
        }
    }

    if (ted->flags & BCMA_TEST_ENGINE_F_HELP) {
        cli_out("Test Parameters:\n");
        if (top->help) {
            top->help(ted->unit, ted->bp);
        } else {
            cli_out("  None.\n");
        }
    }

exit:
    if (ted->av) {
        SHR_FREE(ted->av);
    }
    if (SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Error: Parsing parameters failed.\n")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Execution phase operation function for Test Engine.
 *
 * Execution phase operation function for Test Engine.
 *
 * \param [in/out] data Test Engine Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_xxx Other failures.
 */
static int
testdb_run_execution(void *data)
{
    bcma_test_engine_data_t *ted = (bcma_test_engine_data_t *)data;
    int u, i, j, result = SHR_E_NONE;
    bcma_test_db_t *tdb;
    bcma_test_object_t *to;
    bcma_test_group_t *tg;
    bcma_test_case_t *tc;
    bcma_test_inst_t *ti;
    bcma_test_op_t *top;
    bcma_test_stat_t *tstat;
    volatile int vloops;
    bcma_test_proc_t *proc;
    sal_usecs_t start, end;
    uint32_t second, usecond;
    char istr[128];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(ted, SHR_E_PARAM);

    u = ted->unit;
    vloops = ted->loops;
    tdb = ted->tdb;
    to = ted->to;
    tg = to->tg;
    tc = to->tc;
    ti = to->ti;
    top = tc->op;
    tstat = &ti->stats;

    if (-1 == vloops) {
        vloops = tc->loops;
    }

    testdb_id_format(tc->id, ti->inst, istr, 128);

    for (i = 0; i < vloops; i++) {
        if (vloops > 1) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META("Test %"PRId32": %s (%s)."
                                  "Starting iteration %"PRId32".\n"),
                         tc->id, tc->name, tg->name, i + 1));
        }

        if (tdb->options & BCMA_TEST_O_RUN) {
            cli_out("Test %s.%s (%s) Started ...\n", tg->name, tc->name, istr);
        }

        tstat->runs++;

        start = sal_time_usecs();
        for (j = 0; j < top->proc_count; j++) {
            proc = &top->procs[j];
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META("Test (%s): proc %"PRId32" : %s\n"),
                         tc->name, j, proc->desc ? proc->desc : ""));
            if (proc->cb) {
                result = proc->cb(u, ted->bp, tdb->options);
                if (SHR_FAILURE(result)) {
                    break;
                }
            }
        }
        end = sal_time_usecs();
        end = SAL_USECS_SUB(end,start);
        second = end / 1000000;
        usecond = end % 1000000;

        if (tdb->options & BCMA_TEST_O_RUN) {
            cli_out("Test %s.%s (%s) %s! "
                    "Duration(%"PRId32".%06"PRId32" Seconds)\n",
                    tg->name, tc->name, istr,
                    SHR_FAILURE(result) ? "Failure" : "Success",
                    second, usecond);
        }
        if (SHR_FAILURE(result)) {
            break;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_run_result_validate(result, tdb, tc, ti, u));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Teardown phase operation function for Test Engine.
 *
 * Teardown phase operation function for Test Engine.
 *
 * \param [in/out] data Test Engine Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_run_teardown(bcma_test_engine_data_t *ted)
{
    bcma_test_object_t *to;
    bcma_test_case_t *tc;
    bcma_test_op_t *top;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(ted, SHR_E_PARAM);

    to = ted->to;
    tc = to->tc;
    top = tc->op;

    if (ted->bp) {
        if (top->recycle) {
            top->recycle(ted->unit, ted->bp);
        }
        SHR_FREE(ted->bp);
    }

    if (ted->av) {
        SHR_FREE(ted->av);
    }

    if (ted->a) {
        BCMA_CLI_ARG_DISCARD(ted->a);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Test Engine Routine.
 *
 * Test Engine Routine.
 *
 * \param [in/out] data Test Engine Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_MEMORY No Memory Resource.
 * \return SHR_E_FAIL Test Parse Failed.
 * \return SHR_E_xxx Other Failures.
 */
static int
testdb_run_engine(bcma_test_engine_data_t *ted)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(ted, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_ctrlc_exec(testdb_run_parse, ted));
    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_ctrlc_exec(testdb_run_execution, ted));

exit:
    SHR_IF_ERR_CONT
        (testdb_run_teardown(ted));
    SHR_FUNC_EXIT();
}

/*!
 * \brief Pre-test phase before running test object.
 *
 * Pre-test phase before running test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] unit Device Number
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_run_pre_test(bcma_test_db_t *tdb, bcma_test_object_t *to, int unit)
{
    bcma_test_case_t *tc;
    bool mgmt_attached = bcmmgmt_dev_attached(unit);

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);

    tc = to->tc;

    if (tdb->state[unit] == BCMA_TEST_DEV_STATE_INIT ||
        tdb->state[unit] == BCMA_TEST_DEV_STATE_INDETERMINATE ||
        (mgmt_attached == true && tc->flags & BCMA_TEST_F_DETACHED) ||
        (mgmt_attached == false && !(tc->flags & BCMA_TEST_F_DETACHED)) ||
        tc->flags & BCMA_TEST_F_RC) {
        if (!(tdb->options & BCMA_TEST_O_NO_REINIT)) {

            if (bcmmgmt_dev_attached(unit)) {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Shutting down device\n")));
                SHR_IF_ERR_EXIT(bcmmgmt_dev_detach(unit));
            } else if (bcmbd_attached(unit)) {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Stop running base driver\n")));
                SHR_IF_ERR_EXIT(bcmbd_dev_stop(unit));
                SHR_IF_ERR_EXIT(bcmbd_detach(unit));
            }

            if (tc->flags & BCMA_TEST_F_DETACHED) {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Bring up early config and base driver only\n")));
                SHR_IF_ERR_EXIT(bcma_testdb_init_bd(unit));
            } else {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Initializing device\n")));
                SHR_IF_ERR_EXIT(bcmmgmt_dev_attach(unit, false));
            }

            tdb->state[unit] = BCMA_TEST_DEV_STATE_READY;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * running a test object.
 *
 * A callback function of Test Object Handler for
 * running a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_UNIT Invalid Device number.
 * \return SHR_E_MEMORY No Memory Resource.
 * \return SHR_E_FAIL Test Parse Failed.
 * \return SHR_E_xxx Other Failures.
 */
static int
testdb_run_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    bcma_test_engine_data_t *ted = (bcma_test_engine_data_t *)user_data;
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);
    SHR_NULL_CHECK(ted, SHR_E_PARAM);

    unit = ted->unit;

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (testdb_run_selected(unit, tdb, to)) {
        /* Pre-test Phase, To check the TDB state. */
        SHR_IF_ERR_VERBOSE_EXIT
            (testdb_run_pre_test(tdb, to, unit));

        /* From Test Database. */
        ted->tdb = tdb;
        ted->to = to;

        /* Start Test Engine. */
        SHR_IF_ERR_VERBOSE_EXIT
            (testdb_run_engine(ted));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Routine for running a list of test instance.
 *
 * Routine for running a list of test instance.
 *
 * \param [in] cli CLI Object
 * \param [in] a CLI arguments
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_MEMORY No Memory Resource.
 * \return SHR_E_FAIL Test Parse Failed.
 * \return SHR_E_xxx Other Failures.
 */
static int
testdb_run(bcma_cli_t *cli,
           bcma_cli_args_t *a,
           bcma_test_db_t *tdb,
           bcma_test_object_key_t *tokey)
{
    char *arg;
    int loops = -1;
    bcma_test_engine_data_t ted;
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    sal_memset(&ted, 0, sizeof(ted));

    /* If there is an argument which is an integer, it's a loop count */
    arg = BCMA_CLI_ARG_CUR(a);
    if (arg) {
        if (bcma_cli_parse_is_int(arg)) {
            loops = sal_ctoi(arg, NULL);
            BCMA_CLI_ARG_NEXT(a);
        }
    }
    /* From CLI */
    ted.cli = cli;
    ted.a = BCMA_CLI_ARG_CNT(a) ? a : NULL;
    ted.av = NULL;
    ted.loops = loops;
    ted.flags = 0;
    ted.unit = cli->cmd_unit;

    toh.cb = testdb_run_cb;
    toh.user_data = &ted;
    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, false, &toh));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief A callback function of Test Object Handler for
 * helping a test object.
 *
 * A callback function of Test Object Handler for
 * helping a test object.
 *
 * \param [in] tdb Test Database
 * \param [in] to Test Object
 * \param [in] user_data User Data
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 */
static int
testdb_help_cb(bcma_test_db_t *tdb, bcma_test_object_t *to, void *user_data)
{
    bcma_test_engine_data_t *ted = (bcma_test_engine_data_t *)user_data;
    bcma_test_group_t *tg;
    bcma_test_inst_t *ti;
    bcma_test_case_t *tc;
    bcma_test_op_t *top;
    bcma_test_proc_t *proc;
    int i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(to, SHR_E_PARAM);
    SHR_NULL_CHECK(ted, SHR_E_PARAM);

    tg = to->tg;
    ti = to->ti;
    tc = to->tc;
    top = tc->op;

    ted->tdb = tdb;
    ted->to = to;

    cli_out("Test Group:\n");
    cli_out("  %s\n", tg->name);
    cli_out("Test Name:\n");
    cli_out("  %s(%"PRId32".%"PRId32")\n", tc->name, tc->id, ti->inst);
    cli_out("Test Description:\n");
    cli_out("  %s", tc->desc ? tc->desc : "(null)\n");
    if (tdb->options & BCMA_TEST_O_DEBUG) {
        cli_out("Test Procedures:\n");
        for (i = 0; i < top->proc_count; i++) {
            proc = &top->procs[i];
            cli_out("  %"PRId32". %s", i, proc->desc ? proc->desc : "NULL\n");
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_run_engine(ted));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Routine for help a specific test instance.
 *
 * Routine for help a specific test instance.
 *
 * \param [in] cli CLI Object
 * \param [in] a CLI arguments
 * \param [in] tdb Test Database
 * \param [in] tokey Test Object Key
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid Parameter.
 * \return SHR_E_MEMORY No Memory Resource.
 * \return SHR_E_FAIL Test Parse Failed.
 * \return SHR_E_xxx Other Failures.
 */
static int
testdb_help(bcma_cli_t *cli,
            bcma_cli_args_t *a,
            bcma_test_db_t *tdb,
            bcma_test_object_key_t *tokey)
{
    bcma_test_engine_data_t ted;
    bcma_test_object_hndlr_t toh;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(cli, SHR_E_PARAM);
    SHR_NULL_CHECK(a, SHR_E_PARAM);
    SHR_NULL_CHECK(tdb, SHR_E_PARAM);
    SHR_NULL_CHECK(tokey, SHR_E_PARAM);

    sal_memset(&ted, 0, sizeof(ted));

    ted.cli = cli;
    ted.a = BCMA_CLI_ARG_CNT(a) ? a : NULL;
    ted.av = NULL;
    ted.loops = 0;
    ted.flags = BCMA_TEST_ENGINE_F_HELP;
    ted.unit = cli->cmd_unit;

    toh.cb = testdb_help_cb;
    toh.user_data = &ted;

    SHR_IF_ERR_VERBOSE_EXIT
        (testdb_lookup_engine(tdb, tokey, true, &toh));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testdb_init_bd(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmcfg_ready(unit)) {
        SHR_IF_ERR_EXIT(bcmlrd_init());
        SHR_IF_ERR_EXIT(bcma_bcmmgmt_config_load(false));
        SHR_IF_ERR_EXIT(bcmcfg_init(unit, false));
        SHR_IF_ERR_EXIT(bcmcfg_config(unit, false));
    }
    SHR_IF_ERR_EXIT(bcmbd_attach(unit));
    SHR_IF_ERR_EXIT(bcmbd_dev_reset(unit));
    SHR_IF_ERR_EXIT(bcmbd_dev_init(unit));
    SHR_IF_ERR_EXIT(bcmbd_dev_start(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testdb_is_enabled(bcma_test_db_t *tdb)
{
    return testdb_is_enabled(tdb);
}

int
bcma_testdb_init(bcma_test_db_t *tdb)
{
    return testdb_init(tdb);
}

int
bcma_testdb_cleanup(bcma_test_db_t *tdb)
{
    return testdb_cleanup(tdb);
}

int
bcma_testdb_testcase_add(bcma_test_db_t *tdb, bcma_test_case_t *tc, int feature)
{
    return testdb_testcase_add(tdb, tc, feature);
}

int
bcma_testdb_parser(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
                   bcma_test_object_key_t *tokey)
{
    return testdb_parse(cli, a, tdb, tokey);
}

int
bcma_testdb_select(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    return testdb_select(tdb, tokey);
}

int
bcma_testdb_deselect(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    return testdb_deselect(tdb, tokey);
}

int
bcma_testdb_list(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey,
                 uint32_t flags)
{
    return testdb_list(tdb, tokey, flags);
}

int
bcma_testdb_clear(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    return testdb_clear(tdb, tokey);
}

int
bcma_testdb_remove(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey)
{
    return testdb_remove(tdb, tokey);
}

int
bcma_testdb_clone(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey,
                  char *args)
{
    return testdb_clone(tdb, tokey, args);
}

int
bcma_testdb_parameters(bcma_test_db_t *tdb, bcma_test_object_key_t *tokey,
                       char *args)
{
    return testdb_parameters(tdb, tokey, args);
}

int
bcma_testdb_run(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
                bcma_test_object_key_t *tokey)
{
    return testdb_run(cli, a, tdb, tokey);
}

int
bcma_testdb_help(bcma_cli_t *cli, bcma_cli_args_t *a, bcma_test_db_t *tdb,
                 bcma_test_object_key_t *tokey)
{
    return testdb_help(cli, a, tdb, tokey);
}
