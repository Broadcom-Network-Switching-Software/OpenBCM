/*! \file bcma_testcase_traffic_snake.h
 *
 * Test case for snake traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_TRAFFIC_SNAKE_H
#define BCMA_TESTCASE_TRAFFIC_SNAKE_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for snake traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_SNAKE_DESC \
    "Verify maximum throughput using snake topology " \
    "by connecting a series of ports.\n"

/*! Flags for snake traffic test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_TRAFFIC_SNAKE_FLAGS (0)

/*!
 * \brief Get snake traffic test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_snake_op_get(void);


#endif /* BCMA_TESTCASE_TRAFFIC_SNAKE_H */
