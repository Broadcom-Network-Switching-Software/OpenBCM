/*! \file bcma_testcase_add_benchmark_cases.c
 *
 * Add test cases for Benchmark.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_benchmark.h>
#include <bcma/test/testcase/bcma_testcase_benchmark_cpu.h>

static bcma_test_case_t testcase_benchmark_pair = {
    "benchmark",
    21,
    "CPU benchmark",
    BCMA_TESTCASE_BENCHMARK_CPU_DESC,
    BCMA_TESTCASE_BENCHMARK_CPU_FLAGS,
    1,
    NULL,
    bcma_testcase_benchmark_cpu_op_get,
};

void
bcma_testcase_add_benchmark_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_benchmark_pair, 0);
}
