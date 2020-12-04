/*! \file bcma_testcase_mem_rand.h
 *
 * Test case for memory random address/data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_RAND_H
#define BCMA_TESTCASE_MEM_RAND_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for memory random address/data test case. */
#define BCMA_TESTCASE_MEM_RAND_DESC \
    "Fill SOC memories and verify random address indices using random data.\n"

/*! Flags for memory random address/data test case. */
#define BCMA_TESTCASE_MEM_RAND_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get memory random address/data test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_rand_op_get(void);

#endif /* BCMA_TESTCASE_MEM_RAND_H */
