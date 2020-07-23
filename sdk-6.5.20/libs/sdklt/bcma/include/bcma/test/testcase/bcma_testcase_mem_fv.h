/*! \file bcma_testcase_mem_fv.h
 *
 * Test case for memory fill/verify.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_FV_H
#define BCMA_TESTCASE_MEM_FV_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for memory fill/verify test case. */
#define BCMA_TESTCASE_MEM_FV_DESC \
    "Fill SOC memories and verify all address indices " \
    "from bottom to top using various data patterns.\n"

/*! Flags for memory fill/verify test case. */
#define BCMA_TESTCASE_MEM_FV_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get memory fill/verify test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_fv_op_get(void);

#endif /* BCMA_TESTCASE_MEM_FV_H */
