/*! \file bcma_testcase_counter_wr.h
 *
 * Broadcom Counter Write/Read test case.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_COUNTER_WR_H
#define BCMA_TESTCASE_COUNTER_WR_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for counter write/read test case. */
#define BCMA_TESTCASE_COUNTER_WR_DESC \
    "Verify write/read of all counters.\n"

/*! Flags of counter write/read test case. */
#define BCMA_TESTCASE_COUNTER_WR_FLAGS \
    (0)

/*!
 * \brief Get counter write/read test case operation routines.
 *
 * This function gets counter write/read test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_counter_wr_op_get(void);

#endif /* BCMA_TESTCASE_COUNTER_WR_H */
