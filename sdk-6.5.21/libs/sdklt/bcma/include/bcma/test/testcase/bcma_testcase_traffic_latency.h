/*! \file bcma_testcase_traffic_latency.h
 *
 * Test case for latency traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_TRAFFIC_LATENCY_H
#define BCMA_TESTCASE_TRAFFIC_LATENCY_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for latency traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_LATENCY_DESC \
    "Latency test to check basic L2 packet switching."

/*! Flags for latency traffic test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_TRAFFIC_LATENCY_FLAGS (0)

/*!
 * \brief Get latency traffic test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_latency_op_get(void);

#endif /* BCMA_TESTCASE_TRAFFIC_LATENCY_H */
