/*! \file bcma_testcase_add_traffic_cases.c
 *
 * Add test cases for Traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_traffic_pair.h>
#include <bcma/test/testcase/bcma_testcase_traffic_snake.h>
#include <bcma/test/testcase/bcma_testcase_traffic_stream.h>
#include <bcma/test/testcase/bcma_testcase_traffic_latency.h>
#include <bcma/test/testcase/bcma_testcase_traffic_flex.h>
#include <bcma/test/testcase/bcma_testcase_traffic.h>

static bcma_test_case_t testcase_traffic_snake = {
    "traffic",
    39,
    "Snake traffic",
    BCMA_TESTCASE_TRAFFIC_SNAKE_DESC,
    BCMA_TESTCASE_TRAFFIC_SNAKE_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_snake_op_get,
};

static bcma_test_case_t testcase_traffic_pair = {
    "traffic",
    72,
    "Paired traffic test",
    BCMA_TESTCASE_TRAFFIC_PAIR_DESC,
    BCMA_TESTCASE_TRAFFIC_PAIR_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_pair_op_get,
};

static bcma_test_case_t testcase_traffic_stream = {
    "traffic",
    501,
    "Streaming traffic",
    BCMA_TESTCASE_TRAFFIC_STREAM_DESC,
    BCMA_TESTCASE_TRAFFIC_STREAM_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_stream_op_get,
};

static bcma_test_case_t testcase_traffic_latency = {
    "traffic",
    507,
    "Latency testing",
    BCMA_TESTCASE_TRAFFIC_LATENCY_DESC,
    BCMA_TESTCASE_TRAFFIC_LATENCY_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_latency_op_get,
};

static bcma_test_case_t testcase_traffic_flex = {
    "traffic",
    514,
    "Flexport traffic",
    BCMA_TESTCASE_TRAFFIC_FLEX_DESC,
    BCMA_TESTCASE_TRAFFIC_FLEX_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_flex_op_get,
};

static bcma_test_case_t testcase_traffic_pair_ext = {
    "traffic",
    572,
    "Extended streaming test",
    BCMA_TESTCASE_TRAFFIC_PAIR_EXT_DESC,
    BCMA_TESTCASE_TRAFFIC_PAIR_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_pair_ext_op_get,
};

void
bcma_testcase_add_traffic_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_traffic_pair, 0);
    bcma_test_testcase_add(tdb, &testcase_traffic_snake, 0);
    bcma_test_testcase_add(tdb, &testcase_traffic_stream, 0);
    bcma_test_testcase_add(tdb, &testcase_traffic_latency, 0);
    bcma_test_testcase_add(tdb, &testcase_traffic_flex, 0);
    bcma_test_testcase_add(tdb, &testcase_traffic_pair_ext, 0);
}
