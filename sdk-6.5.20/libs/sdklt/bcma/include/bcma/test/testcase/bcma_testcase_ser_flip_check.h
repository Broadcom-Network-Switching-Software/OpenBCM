/*! \file bcma_testcase_ser_flip_check.h
 *
 * Add SER flip check.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_SER_FLIP_CHECK_H
#define BCMA_TESTCASE_SER_FLIP_CHECK_H

#include <bcma/test/bcma_testcase.h>

/*! Brief Description for SER In_House test case. */
#define BCMA_TESTCASE_SER_IH_DESC \
    "SER In-House Test, The test writes to all SW accessible memories " \
    "on chip and reads them back to check for flips.\n"

/*! Flags for SER In_House test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_SER_IH_FLAGS   0

/*!
 * \brief Get SER In_House operation handler.
 *
 *
 * \return SER operation handler.
 */
extern bcma_test_op_t *
bcma_testcase_ser_ih_op_get(void);

#endif /* BCMA_TESTCASE_SER_FLIP_CHECK_H */
