/*! \file bcma_testcase_mem_rand_w.h
 *
 * Test case for memory random address sets.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_RAND_W_H
#define BCMA_TESTCASE_MEM_RAND_W_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for memory random address sets test case. */
#define BCMA_TESTCASE_MEM_RAND_W_DESC \
    "Fill SOC memories  and verify random sets of address " \
    "indices using XOR data patterns.\n"

/*! Flags for memory random address sets test case. */
#define BCMA_TESTCASE_MEM_RAND_W_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get memory random address sets test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_rand_w_op_get(void);

#endif /* BCMA_TESTCASE_MEM_RAND_W_H */
