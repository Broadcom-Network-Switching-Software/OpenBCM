/*! \file bcma_testcase_cmicd_sbusdma.h
 *
 * Broadcom Sbus Dma test case for CMICd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICD_SBUSDMA_H
#define BCMA_TESTCASE_CMICD_SBUSDMA_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICd Sbusdma test case. */
#define BCMA_TESTCASE_CMICD_SBUSDMA_DESC \
    "Test SBUS DMA access for memory read/write operations.\n"

/*! Flags of CMICd Sbusdma test case. */
#define BCMA_TESTCASE_CMICD_SBUSDMA_FLAGS \
    (BCMA_TEST_F_DETACHED | BCMA_TEST_F_DESTRUCTIVE)

/*!
 * \brief Get CMICd sbusdma test case operation routines.
 *
 * This function gets CMICd sbusdma test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicd_sbusdma_op_get(void);

#endif /* BCMA_TESTCASE_CMICD_SBUSDMA_H */
