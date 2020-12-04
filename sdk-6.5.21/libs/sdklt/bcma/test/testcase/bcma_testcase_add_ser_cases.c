/*! \file bcma_testcase_add_ser_cases.c
 *
 * Add test cases for SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_ser.h>
#include <bcma/test/testcase/bcma_testcase_ser_pt.h>
#include <bcma/test/testcase/bcma_testcase_ser_flip_check.h>
#include <bcma/test/testcase/bcma_testcase_ser_buffer_bus.h>

static bcma_test_case_t testcase_ser_bb = {
    "ser",
    143,
    "SER internal",
    BCMA_TESTCASE_SER_BB_DESC,
    BCMA_TESTCASE_SER_BB_FLAGS,
    1,
    NULL,
    bcma_testcase_ser_bb_op_get,
};

static bcma_test_case_t testcase_ser = {
    "ser",
    144,
    "SER",
    BCMA_TESTCASE_SER_DESC,
    BCMA_TESTCASE_SER_FLAGS,
    1,
    NULL,
    bcma_testcase_ser_op_get,
};

static bcma_test_case_t testcase_ser_ih = {
    "ser",
    901,
    "SER in-house",
    BCMA_TESTCASE_SER_IH_DESC,
    BCMA_TESTCASE_SER_IH_FLAGS,
    1,
    NULL,
    bcma_testcase_ser_ih_op_get,
};

void
bcma_testcase_add_ser_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_ser_bb, 0);
    bcma_test_testcase_add(tdb, &testcase_ser, 0);
    bcma_test_testcase_add(tdb, &testcase_ser_ih, 0);
}

