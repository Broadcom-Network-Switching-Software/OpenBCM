/*! \file bcma_testcase_add_lb_cases.c
 *
 * Add test cases for Loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_lb_cpu.h>
#include <bcma/test/testcase/bcma_testcase_lb_mac.h>
#include <bcma/test/testcase/bcma_testcase_lb_phy.h>
#include <bcma/test/testcase/bcma_testcase_lb_ext.h>
#include <bcma/test/testcase/bcma_testcase_lb.h>

static bcma_test_case_t testcase_cpu_lb = {
    "lb",
    17,
    "CPU loopback",
    BCMA_TESTCASE_CPU_LB_DESC,
    BCMA_TESTCASE_CPU_LB_FLAGS,
    1,
    NULL,
    bcma_testcase_cpu_lb_op_get,
};

static bcma_test_case_t testcase_mac_lb = {
    "lb",
    18,
    "MAC loopback",
    BCMA_TESTCASE_MAC_LB_DESC,
    BCMA_TESTCASE_MAC_LB_FLAGS,
    1,
    NULL,
    bcma_testcase_mac_lb_op_get,
};

static bcma_test_case_t testcase_phy_lb = {
    "lb",
    19,
    "PHY loopback",
    BCMA_TESTCASE_PHY_LB_DESC,
    BCMA_TESTCASE_PHY_LB_FLAGS,
    1,
    NULL,
    bcma_testcase_phy_lb_op_get,
};

static bcma_test_case_t testcase_ext_lb = {
    "lb",
    20,
    "External loopback",
    BCMA_TESTCASE_EXT_LB_DESC,
    BCMA_TESTCASE_EXT_LB_FLAGS,
    1,
    NULL,
    bcma_testcase_ext_lb_op_get,
};

void
bcma_testcase_add_lb_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_cpu_lb, 0);
    bcma_test_testcase_add(tdb, &testcase_mac_lb, 0);
    bcma_test_testcase_add(tdb, &testcase_phy_lb, 0);
    bcma_test_testcase_add(tdb, &testcase_ext_lb, 0);
}
