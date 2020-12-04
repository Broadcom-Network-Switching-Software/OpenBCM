/*! \file bcma_testcase_lb_phy.h
 *
 * Test case for PHY loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_LB_PHY_H
#define BCMA_TESTCASE_LB_PHY_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for PHY loopback test case. */
#define BCMA_TESTCASE_PHY_LB_DESC \
    "Enable PHY loopback, verify TX/RX on all COSQs and " \
    "different packet lengths.\n"

/*! Flags for PHY loopback test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_PHY_LB_FLAGS (0)

/*!
 * \brief Get PHY loopback test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
bcma_test_op_t *
bcma_testcase_phy_lb_op_get(void);

#endif /* BCMA_TESTCASE_LB_PHY_H */
