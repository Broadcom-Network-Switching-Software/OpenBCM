/*! \file bcma_testcase_counter_width.h
 *
 * Broadcom Counter Width test case.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_COUNTER_WIDTH_H
#define BCMA_TESTCASE_COUNTER_WIDTH_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for counter width test case. */
#define BCMA_TESTCASE_COUNTER_WIDTH_DESC \
    "Verify the field widths of all counters compared to " \
    "the expected values in the register profile.\n"

/*! Flags of counter width test case. */
#define BCMA_TESTCASE_COUNTER_WIDTH_FLAGS \
    (0)

/*!
 * \brief Get counter width test case operation routines.
 *
 * This function gets counter width test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_counter_width_op_get(void);

#endif /* BCMA_TESTCASE_COUNTER_WIDTH_H */
