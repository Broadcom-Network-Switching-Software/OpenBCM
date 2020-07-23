/*! \file bcma_testcase_cmicd_fifodma.h
 *
 * Broadcom FIFO DMA test case for CMICd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICD_FIFODMA_H
#define BCMA_TESTCASE_CMICD_FIFODMA_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICd FIFODMA test case. */
#define BCMA_TESTCASE_CMICD_FIFODMA_DESC \
    "Test FIFO DMA access for memory read/write operations.\n"

/*! Flags of CMICd FIFODMA test case. */
#define BCMA_TESTCASE_CMICD_FIFODMA_FLAGS \
    (0)

/*!
 * \brief Get CMICd FIFODMA test case operation routines.
 *
 * This function gets CMICd FIFODMA test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicd_fifodma_op_get(void);

#endif /* BCMA_TESTCASE_CMICD_FIFODMA_H */
