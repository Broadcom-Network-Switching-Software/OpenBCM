/*! \file bcma_testcase_add_counter_cases.c
 *
 * Add test cases for counter registers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/testcase/bcma_testcase_counter.h>
#include <bcma/test/testcase/bcma_testcase_counter_width.h>
#include <bcma/test/testcase/bcma_testcase_counter_wr.h>
#include <bcma/test/testcase/bcma_testcase_counter_snmp.h>

static bcma_test_case_t testcase_counter_width = {
    "counter",
    30,
    "Counter width",
    BCMA_TESTCASE_COUNTER_WIDTH_DESC,
    BCMA_TESTCASE_COUNTER_WIDTH_FLAGS,
    1,
    NULL,
    bcma_testcase_counter_width_op_get,
};

static bcma_test_case_t testcase_counter_wr = {
    "counter",
    31,
    "Counter read write",
    BCMA_TESTCASE_COUNTER_WR_DESC,
    BCMA_TESTCASE_COUNTER_WR_FLAGS,
    1,
    NULL,
    bcma_testcase_counter_wr_op_get,
};

static bcma_test_case_t testcase_counter_snmp = {
    "counter",
    73,
    "SNMP statistics",
    BCMA_TESTCASE_COUNTER_SNMP_DESC,
    BCMA_TESTCASE_COUNTER_SNMP_FLAGS,
    1,
    NULL,
    bcma_testcase_counter_snmp_op_get,
};

void
bcma_testcase_add_counter_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_counter_width, 0);
    bcma_test_testcase_add(tdb, &testcase_counter_wr, 0);
    bcma_test_testcase_add(tdb, &testcase_counter_snmp, 0);
}
