/*! \file bcma_testcase_reg_reset.h
 *
 * Test case for register reset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_REG_RESET_H
#define BCMA_TESTCASE_REG_RESET_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for register reset test case. */
#define BCMA_TESTCASE_REG_RESET_DESC \
    "Reset the chip, read the register value and " \
    "compare with expected reset value.\n"

/*! Flags for register reset test case. */
#define BCMA_TESTCASE_REG_RESET_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get register reset test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_reg_reset_op_get(void);

#endif /* BCMA_TESTCASE_REG_RESET_H */

