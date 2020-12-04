/*! \file bcma_testcase_cmicx_intr.h
 *
 * Broadcom interrupt test case for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_INTR_H
#define BCMA_TESTCASE_CMICX_INTR_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICx Interrupt test case. */
#define BCMA_TESTCASE_CMICX_INTR_DESC \
    "Trigger a hardware interrupt and verify that " \
    "it is received by software handler.\n"

/*! Flags of CMICx Interrupt test case. */
#define BCMA_TESTCASE_CMICX_INTR_FLAGS \
    (BCMA_TEST_F_DETACHED)

/*!
 * \brief Get CMICx interrupt test case operation routines.
 *
 * This function gets CMICx interrupt test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicx_intr_op_get(void);

#endif /* BCMA_TESTCASE_CMICX_INTR_H */
