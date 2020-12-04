/*! \file bcma_testcase_cmicd_ccmdma.h
 *
 * Broadcom Cross Couple Memory Dma test case for CMICd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICD_CCMDMA_H
#define BCMA_TESTCASE_CMICD_CCMDMA_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICd ccmdma test case. */
#define BCMA_TESTCASE_CMICD_CCMDMA_DESC \
    "Test CCM DMA operations.\n"

/*! Flags of CMICd ccmdma test case. */
#define BCMA_TESTCASE_CMICD_CCMDMA_FLAGS \
    (0)

/*!
 * \brief Get CMICd ccmdma test case operation routines.
 *
 * This function gets CMICd ccmdma test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicd_ccmdma_op_get(void);

#endif /* BCMA_TESTCASE_CMICD_CCMDMA_H */
