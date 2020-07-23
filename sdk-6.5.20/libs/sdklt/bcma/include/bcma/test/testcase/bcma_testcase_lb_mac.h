/*! \file bcma_testcase_lb_mac.h
 *
 * Test case for MAC loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_LB_MAC_H
#define BCMA_TESTCASE_LB_MAC_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for MAC loopback test case. */
#define BCMA_TESTCASE_MAC_LB_DESC \
    "Enable MAC loopback, verify TX/RX on all COSQs and " \
    "different packet lengths.\n"

/*! Flags for MAC loopback test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_MAC_LB_FLAGS (0)

/*!
 * \brief Get MAC loopback test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
bcma_test_op_t *
bcma_testcase_mac_lb_op_get(void);

#endif /* BCMA_TESTCASE_LB_MAC_H */
