/*! \file bcma_testcase_benchmark.h
 *
 * Add test cases for Benchmark.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_BENCHMARK_H
#define BCMA_TESTCASE_BENCHMARK_H

#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Add benchmark test cases into TDB.
 *
 * \param [in] tdb Test Database.
 */
void
bcma_testcase_add_benchmark_cases(bcma_test_db_t *tdb);

#endif /* BCMA_TESTCASE_BENCHMARK_H */
