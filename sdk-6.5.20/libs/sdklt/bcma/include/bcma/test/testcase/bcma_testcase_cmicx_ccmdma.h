/*! \file bcma_testcase_cmicx_ccmdma.h
 *
 * Broadcom Cross Couple Memory Dma test case for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_CCMDMA_H
#define BCMA_TESTCASE_CMICX_CCMDMA_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICx ccmdma test case. */
#define BCMA_TESTCASE_CMICX_CCMDMA_DESC \
    "Test CCM DMA operations.\n"

/*! Flags of CMICx ccmdma test case. */
#define BCMA_TESTCASE_CMICX_CCMDMA_FLAGS \
    (0)

/*!
 * \brief Get CMICx ccmdma test case operation routines.
 *
 * This function gets CMICx ccmdma test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicx_ccmdma_op_get(void);

#endif /* BCMA_TESTCASE_CMICX_CCMDMA_H */
