/*! \file bcma_testcase_ser_pt.h
 *
 * Add test cases for SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_SER_PT_H
#define BCMA_TESTCASE_SER_PT_H

#include <bcma/test/bcma_testcase.h>

/*! Brief Description for SER test case. */
#define BCMA_TESTCASE_SER_DESC \
    "Inject, correct and validate SER errors.\n"

/*! Flags for SER test case.
 *
 * Indicate the test case is destructive.
 */
#define BCMA_TESTCASE_SER_FLAGS   (BCMA_TEST_F_DESTRUCTIVE)

/*!
 * \brief Get SER operation handler.
 *
 *
 * \return SER operation handler.
 */
extern bcma_test_op_t *
bcma_testcase_ser_op_get(void);

#endif /* BCMA_TESTCASE_SER_PT_H */
