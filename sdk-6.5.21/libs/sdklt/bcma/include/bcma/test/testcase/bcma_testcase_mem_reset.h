/*! \file bcma_testcase_mem_reset.h
 *
 * Test case for memory reset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_RESET_H
#define BCMA_TESTCASE_MEM_RESET_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for memory reset test case. */
#define BCMA_TESTCASE_MEM_RESET_DESC \
    "Fill SOC memories and reset chip, read the memory value " \
    "and compare with expected reset value.\n"

/*! Flags for memory reset test case. */
#define BCMA_TESTCASE_MEM_RESET_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get memory reset test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_reset_op_get(void);

#endif /* BCMA_TESTCASE_MEM_RESET_H */
