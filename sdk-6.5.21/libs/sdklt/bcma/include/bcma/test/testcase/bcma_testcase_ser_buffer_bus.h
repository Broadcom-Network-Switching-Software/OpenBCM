/*! \file bcma_testcase_ser_buffer_bus.h
 *
 * Add test cases for internal buffer and bus.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_SER_BUFFER_BUS_H
#define BCMA_TESTCASE_SER_BUFFER_BUS_H

#include <bcma/test/bcma_testcase.h>

/*! Brief Description for SER test case. */
#define BCMA_TESTCASE_SER_BB_DESC \
    "Inject, correct and validate SER errors for internal buffer and bus.\n"

/*! Flags for SER test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_SER_BB_FLAGS   0

/*!
 * \brief Get SER operation handler.
 *
 * \return SER operation handler.
 */
extern bcma_test_op_t *
bcma_testcase_ser_bb_op_get(void);

#endif /* BCMA_TESTCASE_SER_BUFFER_BUS_H */
