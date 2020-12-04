/*! \file bcma_testcase_mem_dma.h
 *
 * Test case for memory DMA.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_DMA_H
#define BCMA_TESTCASE_MEM_DMA_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for memory DMA test case. */
#define BCMA_TESTCASE_MEM_DMA_DESC \
    "Fill SOC memories and verify DMA mode with bottom-to-top or " \
    "top-to-bottom direction.\n"

/*! Flags for memory DMA test case. */
#define BCMA_TESTCASE_MEM_DMA_FLAGS \
    (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get memory DMA test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_dma_op_get(void);

#endif /* BCMA_TESTCASE_MEM_DMA_H */
