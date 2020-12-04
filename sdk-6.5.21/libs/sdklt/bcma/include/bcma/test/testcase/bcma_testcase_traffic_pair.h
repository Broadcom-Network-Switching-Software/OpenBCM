/*! \file bcma_testcase_traffic_pair.h
 *
 * Test case for pair traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_TRAFFIC_PAIR_H
#define BCMA_TESTCASE_TRAFFIC_PAIR_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for pair traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_PAIR_DESC \
    "Stress test by connecting two ports as a pair with " \
    "MAC/PHY/external loopback and creating packet storms.\n"

/*! Flags for pair traffic test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_TRAFFIC_PAIR_FLAGS (0)

/*!
 * \brief Get pair traffic test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_pair_op_get(void);

/*!
 * \brief Get pair traffic extension test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_pair_ext_op_get(void);

/*! Brief description for pair traffic extended test case. */
#define BCMA_TESTCASE_TRAFFIC_PAIR_EXT_DESC \
    "Traffic swirl test for L2 multicast/unicast flows on ports, queues.\n  " \
    "After achieving the line rate on all ports, the packets are directed to " \
    "the CPU to check the integrity.\n  " \
    "Supports varying packet sizes, probe count, health checker, " \
    "per queue rate checker, per port rate checker.\n"

#endif /* BCMA_TESTCASE_TRAFFIC_PAIR_H */
