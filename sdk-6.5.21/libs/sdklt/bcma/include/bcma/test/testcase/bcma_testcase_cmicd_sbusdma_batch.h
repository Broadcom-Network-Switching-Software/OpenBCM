/*! \file bcma_testcase_cmicd_sbusdma_batch.h
 *
 * Broadcom Sbus Dma Batch test case for CMICd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_H
#define BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICd Sbusdma Batch test case. */
#define BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_DESC \
    "Test SBUS DMA batch access for memory read/write operations.\n"

/*! Flags of CMICd Sbusdma Batch test case. */
#define BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_FLAGS \
    (BCMA_TEST_F_DETACHED)

/*!
 * \brief Get CMICd sbusdma batch test case operation routines.
 *
 * This function gets CMICd sbusdma batch test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicd_sbusdma_batch_op_get(void);

#endif /* BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_H */
