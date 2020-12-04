/*! \file bcma_testcase_cmicx_sbusdma_perf.h
 *
 * Broadcom Sbus Dma test case for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_SBUSDMA_PERF_H
#define BCMA_TESTCASE_CMICX_SBUSDMA_PERF_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICx Sbusdma performance test case. */
#define BCMA_TESTCASE_CMICX_SBUSDMA_PERF_DESC \
    "SBUSDMA performance test.\n"

/*! Flags of CMICx Sbusdma performance test case. */
#define BCMA_TESTCASE_CMICX_SBUSDMA_PERF_FLAGS \
    (BCMA_TEST_F_DETACHED | BCMA_TEST_F_DESTRUCTIVE)

/*!
 * \brief Get CMICx sbusdma performance test case operation routines.
 *
 * This function gets CMICx sbusdma performance test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicx_sbusdma_perf_op_get(void);

#endif /* BCMA_TESTCASE_CMICX_SBUSDMA_PERF_H */
