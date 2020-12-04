/*! \file bcma_testcase_traffic_flex.h
 *
 * Test case for flexing traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_TRAFFIC_FLEX_H
#define BCMA_TESTCASE_TRAFFIC_FLEX_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for flexing traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_FLEX_DESC \
    "Verify functionality of port-down and port-up sequences for " \
    "transfering port mode among quad/tri/dual/single.\n"

/*! Flags for flexing traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_FLEX_FLAGS (BCMA_TEST_F_DESTRUCTIVE)

/*!
 * \brief Get flexing traffic test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_flex_op_get(void);


#endif /* BCMA_TESTCASE_TRAFFIC_FLEX_H */
