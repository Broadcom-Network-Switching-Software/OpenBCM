/*! \file bcma_testcase_reg_wr.h
 *
 * Test cases for register write/read.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_REG_WR_H
#define BCMA_TESTCASE_REG_WR_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for register write/read test case. */
#define BCMA_TESTCASE_REG_WR_DESC \
    "Write register with specific patterns then read back and compare.\n"

/*! Flags for register write/read test case. */
#define BCMA_TESTCASE_REG_WR_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get register write/read test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_reg_wr_op_get(void);

#endif /* BCMA_TESTCASE_REG_WR_H */

