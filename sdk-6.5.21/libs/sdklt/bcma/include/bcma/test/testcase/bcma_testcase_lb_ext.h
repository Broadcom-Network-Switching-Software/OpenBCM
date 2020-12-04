/*! \file bcma_testcase_lb_ext.h
 *
 * Test case for external loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_LB_EXT_H
#define BCMA_TESTCASE_LB_EXT_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for external loopback test case. */
#define BCMA_TESTCASE_EXT_LB_DESC \
    "Enable external loopback with cable connection, " \
    "verify TX/RX on all COSQs and different packet lengths.\n"

/*! Flags for external loopback test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_EXT_LB_FLAGS (0)

/*!
 * \brief Get external loopback test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
bcma_test_op_t *
bcma_testcase_ext_lb_op_get(void);

#endif /* BCMA_TESTCASE_LB_EXT_H */
