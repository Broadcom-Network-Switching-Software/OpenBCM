/*! \file bcma_testcase_benchmark_cpu.h
 *
 * Test case for CPU benchmark.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_BENCHMARK_CPU_H
#define BCMA_TESTCASE_BENCHMARK_CPU_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CPU benchmark test case. */
#define BCMA_TESTCASE_BENCHMARK_CPU_DESC \
    "Get benchmark result of memory, register accessing or lock operations.\n"

/*! Flags for CPU benchmark test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_BENCHMARK_CPU_FLAGS (0)

/*!
 * \brief Get CPU benchmark test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_benchmark_cpu_op_get(void);


#endif /* BCMA_TESTCASE_BENCHMARK_CPU_H */
