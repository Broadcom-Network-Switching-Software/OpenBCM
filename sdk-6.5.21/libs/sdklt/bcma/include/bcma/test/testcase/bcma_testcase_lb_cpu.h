/*! \file bcma_testcase_lb_cpu.h
 *
 * Test case for CPU loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_LB_CPU_H
#define BCMA_TESTCASE_LB_CPU_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CPU loopback test case. */
#define BCMA_TESTCASE_CPU_LB_DESC \
    "Enable DMA loopback, verify TX/RX on all COSQs and " \
    "different packet lengths.\n"

/*! Flags for CPU loopback test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_CPU_LB_FLAGS (0)

/*!
 * \brief Get CPU loopback test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
bcma_test_op_t *
bcma_testcase_cpu_lb_op_get(void);

#endif /* BCMA_TESTCASE_LB_CPU_H */
